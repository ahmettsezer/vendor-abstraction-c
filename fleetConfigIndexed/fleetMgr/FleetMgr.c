/**
 * @file    FleetMgr.c
 * @brief   [fleetConfigIndexed] Filo yoneticisi - A (tipli alanlar) + ID = INDEX.
 *
 *          - Arac ID'si tablonun satir numarasidir: arama YOK. Tek erisim
 *            noktasi getVehicle(): sinir kontrolu + tablo[id].
 *          - Arac arayuzune tek erisim yolu carOf()/truckOf(): HER ZAMAN
 *            gecerli bir arayuz doner. Tip uymazsa guvenli varsayilan
 *            (s_noCar / s_noTruck) gelir; onun her islemi FM_ERR_WRONG_TYPE.
 *          - 'void*' ve cast yok; cagri aninda NULL kontrolu gerekmez.
 *          - MISRA: tek cikis noktasi (15.5), switch'te break (16.3),
 *            && / || sag tarafinda yan etkili cagri yok (13.5).
 */
#include <stddef.h>
#include "FleetMgr.h"

/* NULL = init edilmemis. */
static const FleetMgr_Config_t *s_pCfg = NULL;

/* ======================================================================== */
/*  Guvenli varsayilan arayuzler                                            */
/* ======================================================================== */

static FM_Ret_t wrongInit(void)                 { return FM_ERR_WRONG_TYPE; }
static FM_Ret_t wrongInfo(VehicleInfo_t *pInfo) { (void)pInfo; return FM_ERR_WRONG_TYPE; }
static FM_Ret_t wrongDrive(uint32_t km)         { (void)km;    return FM_ERR_WRONG_TYPE; }
static FM_Ret_t wrongPassengers(uint8_t count)  { (void)count; return FM_ERR_WRONG_TYPE; }
static FM_Ret_t wrongCargo(uint32_t kg)         { (void)kg;    return FM_ERR_WRONG_TYPE; }

static const CarIfc_t   s_noCar   = { wrongInit, wrongInfo, wrongDrive, wrongPassengers };
static const TruckIfc_t s_noTruck = { wrongInit, wrongInfo, wrongDrive, wrongCargo };

/* ======================================================================== */
/*  Arayuze erisim: her zaman gecerli bir arayuz                            */
/* ======================================================================== */

static const CarIfc_t *carOf(const FleetMgr_VehicleCfg_t *pV)
{
    const CarIfc_t *pCar = &s_noCar;

    if ((pV->type == VEHICLE_TYPE_CAR) && (pV->pCar != NULL))
    {
        pCar = pV->pCar;
    }
    return pCar;
}

static const TruckIfc_t *truckOf(const FleetMgr_VehicleCfg_t *pV)
{
    const TruckIfc_t *pTruck = &s_noTruck;

    if ((pV->type == VEHICLE_TYPE_TRUCK) && (pV->pTruck != NULL))
    {
        pTruck = pV->pTruck;
    }
    return pTruck;
}

/* ======================================================================== */
/*  Ortak islemler: tipe gore dogru alandan                                 */
/* ======================================================================== */

static FM_Ret_t vehicleInit(const FleetMgr_VehicleCfg_t *pV)
{
    FM_Ret_t ret;

    switch (pV->type)
    {
        case VEHICLE_TYPE_CAR:   ret = carOf(pV)->init();   break;
        case VEHICLE_TYPE_TRUCK: ret = truckOf(pV)->init(); break;
        default:                 ret = FM_ERR_CONFIG;       break;
    }
    return ret;
}

static FM_Ret_t vehicleGetInfo(const FleetMgr_VehicleCfg_t *pV, VehicleInfo_t *pInfo)
{
    FM_Ret_t ret;

    switch (pV->type)
    {
        case VEHICLE_TYPE_CAR:   ret = carOf(pV)->getInfo(pInfo);   break;
        case VEHICLE_TYPE_TRUCK: ret = truckOf(pV)->getInfo(pInfo); break;
        default:                 ret = FM_ERR_CONFIG;               break;
    }
    return ret;
}

static FM_Ret_t vehicleDrive(const FleetMgr_VehicleCfg_t *pV, uint32_t km)
{
    FM_Ret_t ret;

    switch (pV->type)
    {
        case VEHICLE_TYPE_CAR:   ret = carOf(pV)->drive(km);   break;
        case VEHICLE_TYPE_TRUCK: ret = truckOf(pV)->drive(km); break;
        default:                 ret = FM_ERR_CONFIG;          break;
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

/* Gercek bir arac satiri mi? Tip ile dolu alan uyumlu mu?
 * (VEHICLE_TYPE_NONE burada gecersizdir: unutulan satir sifirla dolar.) */
static bool isRowValid(const FleetMgr_VehicleCfg_t *pV)
{
    bool ok;

    switch (pV->type)
    {
        case VEHICLE_TYPE_CAR:
            ok = isCarIfcValid(pV->pCar) && (pV->pTruck == NULL);
            break;

        case VEHICLE_TYPE_TRUCK:
            ok = isTruckIfcValid(pV->pTruck) && (pV->pCar == NULL);
            break;

        default:
            ok = false;
            break;
    }
    return ok;
}

/* tablo[0] "arac yok" satiri mi? */
static bool isNoneRow(const FleetMgr_VehicleCfg_t *pV)
{
    return (pV->type == VEHICLE_TYPE_NONE) && (pV->pCar == NULL) && (pV->pTruck == NULL);
}

/* Iki satir ayni araci mi gosteriyor? (ayni arayuz tablosu)
 * Not: ayni ID iki kez yazilamaz; ID satirin kendisidir. */
static bool isDuplicate(const FleetMgr_VehicleCfg_t *pA, const FleetMgr_VehicleCfg_t *pB)
{
    return ((pA->pCar   != NULL) && (pA->pCar   == pB->pCar)) ||
           ((pA->pTruck != NULL) && (pA->pTruck == pB->pTruck));
}

static FM_Ret_t validateCfg(const FleetMgr_Config_t *pCfg)
{
    FM_Ret_t ret = FM_OK;

    if ((pCfg == NULL) || (pCfg->pVehicles == NULL) || (pCfg->vehicleCount == 0u))
    {
        ret = FM_ERR_PARAM;
    }
    else if (!isNoneRow(&pCfg->pVehicles[VEHICLE_ID_NONE]))
    {
        ret = FM_ERR_CONFIG;   /* tablo[0] "arac yok" olmali */
    }
    else
    {
        for (uint8_t i = 1u; (ret == FM_OK) && (i < pCfg->vehicleCount); i++)
        {
            if (!isRowValid(&pCfg->pVehicles[i]))
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
/*  Arac bulma: ARAMA YOK, ID dogrudan satir numarasi                       */
/* ======================================================================== */

static FM_Ret_t getVehicle(VehicleId_t id, const FleetMgr_VehicleCfg_t **ppV)
{
    FM_Ret_t ret = FM_OK;

    if (s_pCfg == NULL)
    {
        ret = FM_ERR_NOT_INIT;
    }
    else if ((id == VEHICLE_ID_NONE) || (id >= s_pCfg->vehicleCount))
    {
        ret = FM_ERR_NOT_FOUND;   /* "arac yok" veya tablo disi */
    }
    else
    {
        *ppV = &s_pCfg->pVehicles[id];
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
        for (uint8_t i = 1u; (ret == FM_OK) && (i < pCfg->vehicleCount); i++)   /* 0: arac yok */
        {
            ret = vehicleInit(&pCfg->pVehicles[i]);
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
        /* Otomobil degilse carOf guvenli varsayilani doner -> FM_ERR_WRONG_TYPE */
        ret = carOf(pV)->setPassengers(count);
    }
    return ret;
}

FM_Ret_t FleetMgr_Truck_LoadCargo(VehicleId_t id, uint32_t kg)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    FM_Ret_t ret = getVehicle(id, &pV);

    if (ret == FM_OK)
    {
        ret = truckOf(pV)->loadCargo(kg);
    }
    return ret;
}
