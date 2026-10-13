/**
 * @file    ModuleMgr.c
 * @brief   [fleetConfig] Filo yoneticisi implementasyonu.
 *
 *          - Hicbir vendor (marka) header'i include edilmez.
 *          - Konfigurasyon Init'te BIR KEZ atanan tek bir const pointer'da
 *            tutulur; gosterdigi tablo entegratorde 'const' ve flash'ta.
 *          - Her aracin tek bir arayuzu vardir (tipine gore CarIfc_t veya
 *            TruckIfc_t). Ortak islemler bu arayuzun 'common' alanindan,
 *            tipten bagimsiz olarak cagrilir (getCommon).
 *          - Tipe ozel islemlerde once arac tipi kontrol edilir; tip
 *            uymuyorsa vendor'a hic gidilmez (MM_ERR_WRONG_TYPE).
 *          - MISRA: tek cikis noktasi (15.5), switch'te break (16.3),
 *            && sag tarafinda yan etkili cagri yok (13.5).
 */
#include <stddef.h>
#include "ModuleMgr.h"

/* NULL = init edilmemis. */
static const ModuleMgr_Config_t *s_pCfg = NULL;

/* ======================================================================== */
/*  Ortak arayuze erisim                                                    */
/* ======================================================================== */

/* Aracin tipine gore ortak islemlerin bulundugu yeri doner.
 * Init'ten gecmis bir konfigurasyonda NULL donmez. */
static const VehicleCommonIfc_t *getCommon(const ModuleMgr_VehicleCfg_t *pV)
{
    const VehicleCommonIfc_t *pCommon = NULL;

    switch (pV->type)
    {
        case VEHICLE_TYPE_CAR:
            if (pV->pCar != NULL)
            {
                pCommon = &pV->pCar->common;
            }
            break;

        case VEHICLE_TYPE_TRUCK:
            if (pV->pTruck != NULL)
            {
                pCommon = &pV->pTruck->common;
            }
            break;

        default:
            break;
    }
    return pCommon;
}

/* ======================================================================== */
/*  Konfigurasyon dogrulama                                                 */
/* ======================================================================== */

static bool isCommonIfcValid(const VehicleCommonIfc_t *pIfc)
{
    return (pIfc->getInfo != NULL) && (pIfc->drive != NULL) &&
           (pIfc->refuel != NULL) && (pIfc->doService != NULL);
}

static bool isCarIfcValid(const CarIfc_t *pIfc)
{
    return (pIfc != NULL) && isCommonIfcValid(&pIfc->common) &&
           (pIfc->getSeatCount != NULL) && (pIfc->setPassengers != NULL);
}

static bool isTruckIfcValid(const TruckIfc_t *pIfc)
{
    return (pIfc != NULL) && isCommonIfcValid(&pIfc->common) &&
           (pIfc->getMaxLoadKg != NULL) && (pIfc->getLoadKg != NULL) &&
           (pIfc->loadCargo != NULL) && (pIfc->unloadCargo != NULL);
}

/* Tip ile arayuz pointer'lari uyumlu mu? */
static bool isTypeIfcValid(const ModuleMgr_VehicleCfg_t *pV)
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

/* Iki satir ayni araci mi gosteriyor? (ayni ID veya ayni arayuz tablosu) */
static bool isDuplicate(const ModuleMgr_VehicleCfg_t *pA, const ModuleMgr_VehicleCfg_t *pB)
{
    bool sameIfc = ((pA->pCar   != NULL) && (pA->pCar   == pB->pCar)) ||
                   ((pA->pTruck != NULL) && (pA->pTruck == pB->pTruck));

    return (pA->id == pB->id) || sameIfc;
}

static MM_Ret_t validateVehicle(const ModuleMgr_Config_t *pCfg, uint8_t index)
{
    MM_Ret_t ret = MM_OK;
    const ModuleMgr_VehicleCfg_t *pV = &pCfg->pVehicles[index];

    if ((pV->id == VEHICLE_ID_INVALID) || !isTypeIfcValid(pV))
    {
        ret = MM_ERR_CONFIG;
    }
    else
    {
        for (uint8_t j = (uint8_t)(index + 1u); (ret == MM_OK) && (j < pCfg->vehicleCount); j++)
        {
            if (isDuplicate(pV, &pCfg->pVehicles[j]))
            {
                ret = MM_ERR_CONFIG;
            }
        }
    }
    return ret;
}

static MM_Ret_t validateCfg(const ModuleMgr_Config_t *pCfg)
{
    MM_Ret_t ret = MM_OK;

    if ((pCfg == NULL) || (pCfg->pVehicles == NULL))
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        for (uint8_t i = 0u; (ret == MM_OK) && (i < pCfg->vehicleCount); i++)
        {
            ret = validateVehicle(pCfg, i);
        }
    }
    return ret;
}

/* ======================================================================== */
/*  Arac bulma                                                              */
/* ======================================================================== */

/* id'yi konfigurasyonda arar; bulamazsa NULL. s_pCfg != NULL varsayar. */
static const ModuleMgr_VehicleCfg_t *findVehicle(VehicleId_t id)
{
    const ModuleMgr_VehicleCfg_t *pFound = NULL;

    for (uint8_t i = 0u; (i < s_pCfg->vehicleCount) && (pFound == NULL); i++)
    {
        if (s_pCfg->pVehicles[i].id == id)
        {
            pFound = &s_pCfg->pVehicles[i];
        }
    }
    return pFound;
}

/* Ortak on kontrol: init + id. */
static MM_Ret_t getVehicle(VehicleId_t id, const ModuleMgr_VehicleCfg_t **ppV)
{
    MM_Ret_t ret = MM_OK;

    if (s_pCfg == NULL)
    {
        ret = MM_ERR_NOT_INIT;
    }
    else
    {
        *ppV = findVehicle(id);
        if (*ppV == NULL)
        {
            ret = MM_ERR_NOT_FOUND;
        }
    }
    return ret;
}

/* Ortak islemler icin: init + id + aracin ortak arayuzu. */
static MM_Ret_t getVehicleCommon(VehicleId_t id, const VehicleCommonIfc_t **ppCommon)
{
    const ModuleMgr_VehicleCfg_t *pV = NULL;
    MM_Ret_t ret = getVehicle(id, &pV);

    if (ret == MM_OK)
    {
        *ppCommon = getCommon(pV);
        if (*ppCommon == NULL)
        {
            ret = MM_ERR_CONFIG;   /* Init'ten gecmis konfigurasyonda olmaz */
        }
    }
    return ret;
}

/* Tipe ozel islemler icin: init + id + TIP kontrolu. */
static MM_Ret_t getVehicleOfType(VehicleId_t id, VehicleType_t type,
                                 const ModuleMgr_VehicleCfg_t **ppV)
{
    MM_Ret_t ret = getVehicle(id, ppV);

    if ((ret == MM_OK) && ((*ppV)->type != type))
    {
        ret = MM_ERR_WRONG_TYPE;
    }
    return ret;
}

/* ======================================================================== */
/*  Yasam dongusu ve dolasma                                                */
/* ======================================================================== */

MM_Ret_t ModuleMgr_Init(const ModuleMgr_Config_t *pCfg)
{
    MM_Ret_t ret = validateCfg(pCfg);

    /* Hatali konfigurasyon asla saklanmaz -> init edilmemis kalir. */
    s_pCfg = (ret == MM_OK) ? pCfg : NULL;
    return ret;
}

MM_Ret_t ModuleMgr_DeInit(void)
{
    s_pCfg = NULL;
    return MM_OK;
}

uint8_t ModuleMgr_GetVehicleCount(void)
{
    return (s_pCfg != NULL) ? s_pCfg->vehicleCount : 0u;
}

VehicleId_t ModuleMgr_GetVehicleIdAt(uint8_t index)
{
    VehicleId_t id = VEHICLE_ID_INVALID;

    if ((s_pCfg != NULL) && (index < s_pCfg->vehicleCount))
    {
        id = s_pCfg->pVehicles[index].id;
    }
    return id;
}

MM_Ret_t ModuleMgr_GetVehicleType(VehicleId_t id, VehicleType_t *pType)
{
    const ModuleMgr_VehicleCfg_t *pV = NULL;
    MM_Ret_t ret = getVehicle(id, &pV);

    if (ret == MM_OK)
    {
        if (pType == NULL)
        {
            ret = MM_ERR_PARAM;
        }
        else
        {
            *pType = pV->type;
        }
    }
    return ret;
}

/* ======================================================================== */
/*  Ortak islemler: tipten bagimsiz, 'common' uzerinden                     */
/* ======================================================================== */

MM_Ret_t ModuleMgr_GetInfo(VehicleId_t id, VehicleInfo_t *pInfo)
{
    const VehicleCommonIfc_t *pCommon = NULL;
    MM_Ret_t ret = getVehicleCommon(id, &pCommon);

    if (ret == MM_OK)
    {
        if (pInfo == NULL)
        {
            ret = MM_ERR_PARAM;
        }
        else
        {
            ret = pCommon->getInfo(pInfo);
        }
    }
    return ret;
}

MM_Ret_t ModuleMgr_Drive(VehicleId_t id, uint32_t km)
{
    const VehicleCommonIfc_t *pCommon = NULL;
    MM_Ret_t ret = getVehicleCommon(id, &pCommon);

    if (ret == MM_OK)
    {
        ret = pCommon->drive(km);
    }
    return ret;
}

MM_Ret_t ModuleMgr_Refuel(VehicleId_t id)
{
    const VehicleCommonIfc_t *pCommon = NULL;
    MM_Ret_t ret = getVehicleCommon(id, &pCommon);

    if (ret == MM_OK)
    {
        ret = pCommon->refuel();
    }
    return ret;
}

MM_Ret_t ModuleMgr_Service(VehicleId_t id)
{
    const VehicleCommonIfc_t *pCommon = NULL;
    MM_Ret_t ret = getVehicleCommon(id, &pCommon);

    if (ret == MM_OK)
    {
        ret = pCommon->doService();
    }
    return ret;
}

/* ======================================================================== */
/*  Otomobil arayuzu                                                        */
/* ======================================================================== */

MM_Ret_t ModuleMgr_Car_GetSeatCount(VehicleId_t id, uint8_t *pSeats)
{
    const ModuleMgr_VehicleCfg_t *pV = NULL;
    MM_Ret_t ret = getVehicleOfType(id, VEHICLE_TYPE_CAR, &pV);

    if (ret == MM_OK)
    {
        if (pSeats == NULL)
        {
            ret = MM_ERR_PARAM;
        }
        else
        {
            ret = pV->pCar->getSeatCount(pSeats);
        }
    }
    return ret;
}

MM_Ret_t ModuleMgr_Car_SetPassengers(VehicleId_t id, uint8_t count)
{
    const ModuleMgr_VehicleCfg_t *pV = NULL;
    MM_Ret_t ret = getVehicleOfType(id, VEHICLE_TYPE_CAR, &pV);

    if (ret == MM_OK)
    {
        ret = pV->pCar->setPassengers(count);
    }
    return ret;
}

/* ======================================================================== */
/*  Tir arayuzu                                                             */
/* ======================================================================== */

MM_Ret_t ModuleMgr_Truck_GetMaxLoad(VehicleId_t id, uint32_t *pKg)
{
    const ModuleMgr_VehicleCfg_t *pV = NULL;
    MM_Ret_t ret = getVehicleOfType(id, VEHICLE_TYPE_TRUCK, &pV);

    if (ret == MM_OK)
    {
        if (pKg == NULL)
        {
            ret = MM_ERR_PARAM;
        }
        else
        {
            ret = pV->pTruck->getMaxLoadKg(pKg);
        }
    }
    return ret;
}

MM_Ret_t ModuleMgr_Truck_GetLoad(VehicleId_t id, uint32_t *pKg)
{
    const ModuleMgr_VehicleCfg_t *pV = NULL;
    MM_Ret_t ret = getVehicleOfType(id, VEHICLE_TYPE_TRUCK, &pV);

    if (ret == MM_OK)
    {
        if (pKg == NULL)
        {
            ret = MM_ERR_PARAM;
        }
        else
        {
            ret = pV->pTruck->getLoadKg(pKg);
        }
    }
    return ret;
}

MM_Ret_t ModuleMgr_Truck_LoadCargo(VehicleId_t id, uint32_t kg)
{
    const ModuleMgr_VehicleCfg_t *pV = NULL;
    MM_Ret_t ret = getVehicleOfType(id, VEHICLE_TYPE_TRUCK, &pV);

    if (ret == MM_OK)
    {
        ret = pV->pTruck->loadCargo(kg);
    }
    return ret;
}

MM_Ret_t ModuleMgr_Truck_UnloadCargo(VehicleId_t id)
{
    const ModuleMgr_VehicleCfg_t *pV = NULL;
    MM_Ret_t ret = getVehicleOfType(id, VEHICLE_TYPE_TRUCK, &pV);

    if (ret == MM_OK)
    {
        ret = pV->pTruck->unloadCargo();
    }
    return ret;
}

/* ======================================================================== */
/*  Filo seviyesi: tipe gore yonetim                                        */
/* ======================================================================== */

MM_Ret_t ModuleMgr_GetFleetSummary(ModuleMgr_FleetSummary_t *pSummary)
{
    MM_Ret_t ret = MM_OK;

    if (s_pCfg == NULL)
    {
        ret = MM_ERR_NOT_INIT;
    }
    else if (pSummary == NULL)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        *pSummary = (ModuleMgr_FleetSummary_t){ { 0u }, 0u, 0u, 0u };

        for (uint8_t i = 0u; i < s_pCfg->vehicleCount; i++)
        {
            const ModuleMgr_VehicleCfg_t *pV      = &s_pCfg->pVehicles[i];
            const VehicleCommonIfc_t     *pCommon = getCommon(pV);
            VehicleInfo_t info;
            uint8_t  seats = 0u;
            uint32_t maxKg = 0u;

            pSummary->vehicleCount[pV->type]++;   /* type, Init'te dogrulandi */

            if (pCommon != NULL)
            {
                if (pCommon->getInfo(&info) == MM_OK)
                {
                    pSummary->totalOdometerKm += info.odometerKm;
                }
            }

            /* Tipe ozel bilgi: ModuleMgr tipe gore dogru arayuzu secer. */
            switch (pV->type)
            {
                case VEHICLE_TYPE_CAR:
                    if (pV->pCar->getSeatCount(&seats) == MM_OK)
                    {
                        pSummary->totalSeats += seats;
                    }
                    break;

                case VEHICLE_TYPE_TRUCK:
                    if (pV->pTruck->getMaxLoadKg(&maxKg) == MM_OK)
                    {
                        pSummary->totalMaxLoadKg += maxKg;
                    }
                    break;

                default:
                    break;
            }
        }
    }
    return ret;
}

/* Arac bos mu? (ortak islemlerden) */
static bool isAvailable(const ModuleMgr_VehicleCfg_t *pV)
{
    const VehicleCommonIfc_t *pCommon = getCommon(pV);
    VehicleInfo_t info;
    bool available = false;

    if (pCommon != NULL)
    {
        if (pCommon->getInfo(&info) == MM_OK)
        {
            available = (info.status == VEHICLE_STATUS_AVAILABLE);
        }
    }
    return available;
}

MM_Ret_t ModuleMgr_FindCarForPassengers(uint8_t passengers, VehicleId_t *pId)
{
    MM_Ret_t ret = MM_ERR_NOT_FOUND;

    if (s_pCfg == NULL)
    {
        ret = MM_ERR_NOT_INIT;
    }
    else if (pId == NULL)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        uint8_t bestSeats = 0xFFu;

        for (uint8_t i = 0u; i < s_pCfg->vehicleCount; i++)
        {
            const ModuleMgr_VehicleCfg_t *pV = &s_pCfg->pVehicles[i];
            uint8_t seats = 0u;

            if ((pV->type == VEHICLE_TYPE_CAR) && isAvailable(pV))
            {
                if (pV->pCar->getSeatCount(&seats) == MM_OK)
                {
                    /* yeten en kucuk arac: buyuk araclar buyuk gorevlere kalsin */
                    if ((seats >= passengers) && (seats < bestSeats))
                    {
                        bestSeats = seats;
                        *pId      = pV->id;
                        ret       = MM_OK;
                    }
                }
            }
        }
    }
    return ret;
}

MM_Ret_t ModuleMgr_FindTruckForLoad(uint32_t kg, VehicleId_t *pId)
{
    MM_Ret_t ret = MM_ERR_NOT_FOUND;

    if (s_pCfg == NULL)
    {
        ret = MM_ERR_NOT_INIT;
    }
    else if (pId == NULL)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        uint32_t bestKg = 0xFFFFFFFFu;

        for (uint8_t i = 0u; i < s_pCfg->vehicleCount; i++)
        {
            const ModuleMgr_VehicleCfg_t *pV = &s_pCfg->pVehicles[i];
            uint32_t maxKg = 0u;

            if ((pV->type == VEHICLE_TYPE_TRUCK) && isAvailable(pV))
            {
                if (pV->pTruck->getMaxLoadKg(&maxKg) == MM_OK)
                {
                    if ((maxKg >= kg) && (maxKg < bestKg))
                    {
                        bestKg = maxKg;
                        *pId   = pV->id;
                        ret    = MM_OK;
                    }
                }
            }
        }
    }
    return ret;
}

MM_Ret_t ModuleMgr_IsServiceDue(VehicleId_t id, bool *pDue)
{
    const ModuleMgr_VehicleCfg_t *pV = NULL;
    VehicleInfo_t info;
    MM_Ret_t ret = getVehicle(id, &pV);

    if (ret == MM_OK)
    {
        if (pDue == NULL)
        {
            ret = MM_ERR_PARAM;
        }
        else
        {
            ret = ModuleMgr_GetInfo(id, &info);
            if (ret == MM_OK)
            {
                /* Bakim araligi aracin TIPINE gore konfigurasyondan gelir. */
                *pDue = (info.kmSinceService >= s_pCfg->serviceIntervalKm[pV->type]);
            }
        }
    }
    return ret;
}

MM_Ret_t ModuleMgr_RefuelAll(uint8_t *pFailCount)
{
    MM_Ret_t ret = MM_OK;
    uint8_t fails = 0u;

    if (s_pCfg == NULL)
    {
        ret = MM_ERR_NOT_INIT;
    }
    else
    {
        for (uint8_t i = 0u; i < s_pCfg->vehicleCount; i++)
        {
            const VehicleCommonIfc_t *pCommon = getCommon(&s_pCfg->pVehicles[i]);

            if (pCommon == NULL)
            {
                fails++;
            }
            else if (pCommon->refuel() != MM_OK)
            {
                fails++;
            }
            else
            {
                /* basarili */
            }
        }
    }

    if (pFailCount != NULL)
    {
        *pFailCount = fails;
    }
    return ret;
}
