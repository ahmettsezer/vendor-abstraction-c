/**
 * @file    FleetMgr.c
 * @brief   [fleetIndexConfig] Filo yoneticisi implementasyonu.
 *
 *          - Hicbir arac header'i include edilmez; 'void*' ve cast yoktur.
 *          - Arac arayuzune tek erisim yolu carOf()/truckOf(): payload'daki
 *            row_of() gibi HER ZAMAN gecerli bir arayuz doner. Tip uymazsa
 *            veya index tablo disindaysa guvenli varsayilan (s_noCar /
 *            s_noTruck) doner; onun her islemi FM_ERR_WRONG_TYPE'tir.
 *            Bu yuzden cagri aninda NULL veya tip kontrolu gerekmez.
 *          - MISRA: tek cikis noktasi (15.5), switch'te break (16.3),
 *            && / || sag tarafinda yan etkili cagri yok (13.5).
 */
#include <stddef.h>
#include "FleetMgr.h"

/* NULL = init edilmemis. */
static const FleetMgr_Config_t *s_pCfg = NULL;

/* ======================================================================== */
/*  Guvenli varsayilan arayuzler (payload'daki PL_UNKNOWN satiri gibi)      */
/* ======================================================================== */

static FM_Ret_t wrongInit(void)                 { return FM_ERR_WRONG_TYPE; }
static FM_Ret_t wrongInfo(VehicleInfo_t *pInfo) { (void)pInfo; return FM_ERR_WRONG_TYPE; }
static FM_Ret_t wrongDrive(uint32_t km)         { (void)km;    return FM_ERR_WRONG_TYPE; }
static FM_Ret_t wrongPassengers(uint8_t count)  { (void)count; return FM_ERR_WRONG_TYPE; }
static FM_Ret_t wrongCargo(uint32_t kg)         { (void)kg;    return FM_ERR_WRONG_TYPE; }

static const CarIfc_t   s_noCar   = { wrongInit, wrongInfo, wrongDrive, wrongPassengers };
static const TruckIfc_t s_noTruck = { wrongInit, wrongInfo, wrongDrive, wrongCargo };

/* ======================================================================== */
/*  Arayuze erisim: her zaman gecerli bir arayuz (row_of benzeri)           */
/* ======================================================================== */

static const CarIfc_t *carOf(const FleetMgr_Config_t *pCfg, const FleetMgr_VehicleCfg_t *pV)
{
    const CarIfc_t *pCar = &s_noCar;

    if ((pV->type == VEHICLE_TYPE_CAR) && (pV->index < pCfg->carCount))
    {
        if (pCfg->ppCars[pV->index] != NULL)
        {
            pCar = pCfg->ppCars[pV->index];
        }
    }
    return pCar;
}

static const TruckIfc_t *truckOf(const FleetMgr_Config_t *pCfg, const FleetMgr_VehicleCfg_t *pV)
{
    const TruckIfc_t *pTruck = &s_noTruck;

    if ((pV->type == VEHICLE_TYPE_TRUCK) && (pV->index < pCfg->truckCount))
    {
        if (pCfg->ppTrucks[pV->index] != NULL)
        {
            pTruck = pCfg->ppTrucks[pV->index];
        }
    }
    return pTruck;
}

/* ======================================================================== */
/*  Ortak islemler: tipe gore dogru tablodan                                */
/* ======================================================================== */

static FM_Ret_t vehicleInit(const FleetMgr_Config_t *pCfg, const FleetMgr_VehicleCfg_t *pV)
{
    FM_Ret_t ret;

    switch (pV->type)
    {
        case VEHICLE_TYPE_CAR:   ret = carOf(pCfg, pV)->init();   break;
        case VEHICLE_TYPE_TRUCK: ret = truckOf(pCfg, pV)->init(); break;
        default:                 ret = FM_ERR_CONFIG;             break;
    }
    return ret;
}

static FM_Ret_t vehicleGetInfo(const FleetMgr_VehicleCfg_t *pV, VehicleInfo_t *pInfo)
{
    FM_Ret_t ret;

    switch (pV->type)
    {
        case VEHICLE_TYPE_CAR:   ret = carOf(s_pCfg, pV)->getInfo(pInfo);   break;
        case VEHICLE_TYPE_TRUCK: ret = truckOf(s_pCfg, pV)->getInfo(pInfo); break;
        default:                 ret = FM_ERR_CONFIG;                       break;
    }
    return ret;
}

static FM_Ret_t vehicleDrive(const FleetMgr_VehicleCfg_t *pV, uint32_t km)
{
    FM_Ret_t ret;

    switch (pV->type)
    {
        case VEHICLE_TYPE_CAR:   ret = carOf(s_pCfg, pV)->drive(km);   break;
        case VEHICLE_TYPE_TRUCK: ret = truckOf(s_pCfg, pV)->drive(km); break;
        default:                 ret = FM_ERR_CONFIG;                  break;
    }
    return ret;
}

/* ======================================================================== */
/*  Konfigurasyon dogrulama                                                 */
/* ======================================================================== */

static bool isCarIfcValid(const CarIfc_t *pIfc)
{
    return (pIfc != NULL) && (pIfc->init != NULL) && (pIfc->getInfo != NULL) &&
           (pIfc->drive != NULL) && (pIfc->setPassengers != NULL);
}

static bool isTruckIfcValid(const TruckIfc_t *pIfc)
{
    return (pIfc != NULL) && (pIfc->init != NULL) && (pIfc->getInfo != NULL) &&
           (pIfc->drive != NULL) && (pIfc->loadCargo != NULL);
}

/* Satirin index'i, kendi tipinin tablosunda gecerli ve dolu bir arayuzu mu gosteriyor? */
static bool isRowValid(const FleetMgr_Config_t *pCfg, const FleetMgr_VehicleCfg_t *pV)
{
    bool ok;

    switch (pV->type)
    {
        case VEHICLE_TYPE_CAR:
            ok = (pV->index < pCfg->carCount) && isCarIfcValid(pCfg->ppCars[pV->index]);
            break;

        case VEHICLE_TYPE_TRUCK:
            ok = (pV->index < pCfg->truckCount) && isTruckIfcValid(pCfg->ppTrucks[pV->index]);
            break;

        default:
            ok = false;
            break;
    }
    return ok && (pV->id != VEHICLE_ID_INVALID);
}

/* Iki satir ayni araci mi gosteriyor? (ayni ID veya ayni tablo satiri) */
static bool isDuplicate(const FleetMgr_VehicleCfg_t *pA, const FleetMgr_VehicleCfg_t *pB)
{
    return (pA->id == pB->id) || ((pA->type == pB->type) && (pA->index == pB->index));
}

static FM_Ret_t validateCfg(const FleetMgr_Config_t *pCfg)
{
    FM_Ret_t ret = FM_OK;

    if ((pCfg == NULL) || (pCfg->pVehicles == NULL) ||
        ((pCfg->carCount > 0u) && (pCfg->ppCars == NULL)) ||
        ((pCfg->truckCount > 0u) && (pCfg->ppTrucks == NULL)))
    {
        ret = FM_ERR_PARAM;
    }
    else
    {
        for (uint8_t i = 0u; (ret == FM_OK) && (i < pCfg->vehicleCount); i++)
        {
            if (!isRowValid(pCfg, &pCfg->pVehicles[i]))
            {
                ret = FM_ERR_CONFIG;
            }
            for (uint8_t j = (uint8_t)(i + 1u); (ret == FM_OK) && (j < pCfg->vehicleCount); j++)
            {
                if (isDuplicate(&pCfg->pVehicles[i], &pCfg->pVehicles[j]))
                {
                    ret = FM_ERR_CONFIG;
                }
            }
        }
    }
    return ret;
}

/* ======================================================================== */
/*  Arac bulma                                                              */
/* ======================================================================== */

static FM_Ret_t getVehicle(VehicleId_t id, const FleetMgr_VehicleCfg_t **ppV)
{
    FM_Ret_t ret = FM_ERR_NOT_FOUND;

    if (s_pCfg == NULL)
    {
        ret = FM_ERR_NOT_INIT;
    }
    else
    {
        for (uint8_t i = 0u; (i < s_pCfg->vehicleCount) && (ret == FM_ERR_NOT_FOUND); i++)
        {
            if (s_pCfg->pVehicles[i].id == id)
            {
                *ppV = &s_pCfg->pVehicles[i];
                ret  = FM_OK;
            }
        }
    }
    return ret;
}

/* ======================================================================== */
/*  API                                                                     */
/* ======================================================================== */

FM_Ret_t FleetMgr_Init(const FleetMgr_Config_t *pCfg)
{
    FM_Ret_t ret = validateCfg(pCfg);

    s_pCfg = NULL;

    if (ret == FM_OK)   /* pCfg burada gecerli */
    {
        for (uint8_t i = 0u; (ret == FM_OK) && (i < pCfg->vehicleCount); i++)
        {
            ret = vehicleInit(pCfg, &pCfg->pVehicles[i]);
        }

        if (ret == FM_OK)
        {
            s_pCfg = pCfg;
        }
    }
    return ret;
}

FM_Ret_t FleetMgr_DeInit(void)
{
    s_pCfg = NULL;
    return FM_OK;
}

uint8_t FleetMgr_GetVehicleCount(void)
{
    return (s_pCfg != NULL) ? s_pCfg->vehicleCount : 0u;
}

VehicleId_t FleetMgr_GetVehicleIdAt(uint8_t index)
{
    VehicleId_t id = VEHICLE_ID_INVALID;

    if ((s_pCfg != NULL) && (index < s_pCfg->vehicleCount))
    {
        id = s_pCfg->pVehicles[index].id;
    }
    return id;
}

FM_Ret_t FleetMgr_GetType(VehicleId_t id, VehicleType_t *pType)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    FM_Ret_t ret = getVehicle(id, &pV);

    if (ret == FM_OK)
    {
        if (pType == NULL)
        {
            ret = FM_ERR_PARAM;
        }
        else
        {
            *pType = pV->type;
        }
    }
    return ret;
}

FM_Ret_t FleetMgr_GetInfo(VehicleId_t id, VehicleInfo_t *pInfo)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    FM_Ret_t ret = getVehicle(id, &pV);

    if (ret == FM_OK)
    {
        if (pInfo == NULL)
        {
            ret = FM_ERR_PARAM;
        }
        else
        {
            ret = vehicleGetInfo(pV, pInfo);
        }
    }
    return ret;
}

FM_Ret_t FleetMgr_Drive(VehicleId_t id, uint32_t km)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    FM_Ret_t ret = getVehicle(id, &pV);

    if (ret == FM_OK)
    {
        ret = vehicleDrive(pV, km);
    }
    return ret;
}

FM_Ret_t FleetMgr_Car_SetPassengers(VehicleId_t id, uint8_t count)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    FM_Ret_t ret = getVehicle(id, &pV);

    if (ret == FM_OK)
    {
        /* Arac otomobil degilse carOf guvenli varsayilani (s_noCar) doner:
         * cagri araca hic ulasmaz, FM_ERR_WRONG_TYPE doner. */
        ret = carOf(s_pCfg, pV)->setPassengers(count);
    }
    return ret;
}

FM_Ret_t FleetMgr_Truck_LoadCargo(VehicleId_t id, uint32_t kg)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    FM_Ret_t ret = getVehicle(id, &pV);

    if (ret == FM_OK)
    {
        ret = truckOf(s_pCfg, pV)->loadCargo(kg);
    }
    return ret;
}
