/**
 * @file    FleetMgr.c
 * @brief   [fleetConfigB] Filo yoneticisi - B: HER TIP ICIN AYRI TABLO.
 *
 *          - Tip alani ve tip switch'i yok. Arac once otomobil tablosunda,
 *            bulunamazsa tir tablosunda aranir (findCar / findTruck).
 *            Hangi tabloda bulunduysa tipi ve arayuzu odur.
 *          - 'void*' ve cast yok.
 *          - MISRA: tek cikis noktasi (15.5), && / || sag tarafinda yan
 *            etkili cagri yok (13.5).
 */
#include <stddef.h>
#include "FleetMgr.h"

/* NULL = init edilmemis. */
static const FleetMgr_Config_t *s_pCfg = NULL;

/* ======================================================================== */
/*  Arac bulma: tipe gore kendi tablosunda ID aramasi (yoksa NULL)          */
/* ======================================================================== */

static const CarIfc_t *findCar(const FleetMgr_Config_t *pCfg, VehicleId_t id)
{
    const CarIfc_t *pCar = NULL;

    for (uint8_t i = 0u; (i < pCfg->carCount) && (pCar == NULL); i++)
    {
        if (pCfg->pCars[i].id == id)
        {
            pCar = pCfg->pCars[i].pIfc;
        }
    }
    return pCar;
}

static const TruckIfc_t *findTruck(const FleetMgr_Config_t *pCfg, VehicleId_t id)
{
    const TruckIfc_t *pTruck = NULL;

    for (uint8_t i = 0u; (i < pCfg->truckCount) && (pTruck == NULL); i++)
    {
        if (pCfg->pTrucks[i].id == id)
        {
            pTruck = pCfg->pTrucks[i].pIfc;
        }
    }
    return pTruck;
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

/* index'teki aracin ID'si (once otomobiller, sonra tirlar). */
static VehicleId_t idAt(const FleetMgr_Config_t *pCfg, uint8_t index)
{
    VehicleId_t id = VEHICLE_ID_INVALID;

    if (index < pCfg->carCount)
    {
        id = pCfg->pCars[index].id;
    }
    else if (index < (uint8_t)(pCfg->carCount + pCfg->truckCount))
    {
        id = pCfg->pTrucks[index - pCfg->carCount].id;
    }
    else
    {
        /* tablo disi: VEHICLE_ID_INVALID */
    }
    return id;
}

static FM_Ret_t validateCfg(const FleetMgr_Config_t *pCfg)
{
    FM_Ret_t ret = FM_OK;

    if ((pCfg == NULL) ||
        ((pCfg->carCount > 0u) && (pCfg->pCars == NULL)) ||
        ((pCfg->truckCount > 0u) && (pCfg->pTrucks == NULL)))
    {
        ret = FM_ERR_PARAM;
    }
    else
    {
        uint8_t total = (uint8_t)(pCfg->carCount + pCfg->truckCount);

        /* Her tablonun kendi arayuzleri dolu mu? Ayni arayuz iki kez mi? */
        for (uint8_t i = 0u; (ret == FM_OK) && (i < pCfg->carCount); i++)
        {
            if (!isCarIfcValid(pCfg->pCars[i].pIfc))
            {
                ret = FM_ERR_CONFIG;
            }
            for (uint8_t j = (uint8_t)(i + 1u); (ret == FM_OK) && (j < pCfg->carCount); j++)
            {
                if (pCfg->pCars[i].pIfc == pCfg->pCars[j].pIfc)
                {
                    ret = FM_ERR_CONFIG;
                }
            }
        }
        for (uint8_t i = 0u; (ret == FM_OK) && (i < pCfg->truckCount); i++)
        {
            if (!isTruckIfcValid(pCfg->pTrucks[i].pIfc))
            {
                ret = FM_ERR_CONFIG;
            }
            for (uint8_t j = (uint8_t)(i + 1u); (ret == FM_OK) && (j < pCfg->truckCount); j++)
            {
                if (pCfg->pTrucks[i].pIfc == pCfg->pTrucks[j].pIfc)
                {
                    ret = FM_ERR_CONFIG;
                }
            }
        }

        /* ID'ler gecerli ve IKI TABLO ARASINDA da tekil mi? */
        for (uint8_t i = 0u; (ret == FM_OK) && (i < total); i++)
        {
            if (idAt(pCfg, i) == VEHICLE_ID_INVALID)
            {
                ret = FM_ERR_CONFIG;
            }
            for (uint8_t j = (uint8_t)(i + 1u); (ret == FM_OK) && (j < total); j++)
            {
                if (idAt(pCfg, i) == idAt(pCfg, j))
                {
                    ret = FM_ERR_CONFIG;
                }
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
        for (uint8_t i = 0u; (ret == FM_OK) && (i < pCfg->carCount); i++)
        {
            ret = pCfg->pCars[i].pIfc->init();
        }
        for (uint8_t i = 0u; (ret == FM_OK) && (i < pCfg->truckCount); i++)
        {
            ret = pCfg->pTrucks[i].pIfc->init();
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
    return (s_pCfg != NULL) ? (uint8_t)(s_pCfg->carCount + s_pCfg->truckCount) : 0u;
}

VehicleId_t FleetMgr_GetVehicleIdAt(uint8_t index)
{
    return (s_pCfg != NULL) ? idAt(s_pCfg, index) : VEHICLE_ID_INVALID;
}

FM_Ret_t FleetMgr_GetType(VehicleId_t id, VehicleType_t *pType)
{
    FM_Ret_t ret = FM_OK;

    if (s_pCfg == NULL)
    {
        ret = FM_ERR_NOT_INIT;
    }
    else if (pType == NULL)
    {
        ret = FM_ERR_PARAM;
    }
    else if (findCar(s_pCfg, id) != NULL)
    {
        *pType = VEHICLE_TYPE_CAR;
    }
    else if (findTruck(s_pCfg, id) != NULL)
    {
        *pType = VEHICLE_TYPE_TRUCK;
    }
    else
    {
        ret = FM_ERR_NOT_FOUND;
    }
    return ret;
}

FM_Ret_t FleetMgr_GetInfo(VehicleId_t id, VehicleInfo_t *pInfo)
{
    FM_Ret_t ret;

    if (s_pCfg == NULL)
    {
        ret = FM_ERR_NOT_INIT;
    }
    else if (pInfo == NULL)
    {
        ret = FM_ERR_PARAM;
    }
    else
    {
        const CarIfc_t   *pCar   = findCar(s_pCfg, id);
        const TruckIfc_t *pTruck = findTruck(s_pCfg, id);

        if (pCar != NULL)
        {
            ret = pCar->getInfo(pInfo);
        }
        else if (pTruck != NULL)
        {
            ret = pTruck->getInfo(pInfo);
        }
        else
        {
            ret = FM_ERR_NOT_FOUND;
        }
    }
    return ret;
}

FM_Ret_t FleetMgr_Drive(VehicleId_t id, uint32_t km)
{
    FM_Ret_t ret;

    if (s_pCfg == NULL)
    {
        ret = FM_ERR_NOT_INIT;
    }
    else
    {
        const CarIfc_t   *pCar   = findCar(s_pCfg, id);
        const TruckIfc_t *pTruck = findTruck(s_pCfg, id);

        if (pCar != NULL)
        {
            ret = pCar->drive(km);
        }
        else if (pTruck != NULL)
        {
            ret = pTruck->drive(km);
        }
        else
        {
            ret = FM_ERR_NOT_FOUND;
        }
    }
    return ret;
}

FM_Ret_t FleetMgr_Car_SetPassengers(VehicleId_t id, uint8_t count)
{
    FM_Ret_t ret;

    if (s_pCfg == NULL)
    {
        ret = FM_ERR_NOT_INIT;
    }
    else
    {
        const CarIfc_t *pCar = findCar(s_pCfg, id);   /* sadece otomobil tablosu */

        if (pCar != NULL)
        {
            ret = pCar->setPassengers(count);
        }
        else if (findTruck(s_pCfg, id) != NULL)
        {
            ret = FM_ERR_WRONG_TYPE;                    /* var ama otomobil degil */
        }
        else
        {
            ret = FM_ERR_NOT_FOUND;
        }
    }
    return ret;
}

FM_Ret_t FleetMgr_Truck_LoadCargo(VehicleId_t id, uint32_t kg)
{
    FM_Ret_t ret;

    if (s_pCfg == NULL)
    {
        ret = FM_ERR_NOT_INIT;
    }
    else
    {
        const TruckIfc_t *pTruck = findTruck(s_pCfg, id);   /* sadece tir tablosu */

        if (pTruck != NULL)
        {
            ret = pTruck->loadCargo(kg);
        }
        else if (findCar(s_pCfg, id) != NULL)
        {
            ret = FM_ERR_WRONG_TYPE;                        /* var ama tir degil */
        }
        else
        {
            ret = FM_ERR_NOT_FOUND;
        }
    }
    return ret;
}
