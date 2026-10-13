/**
 * @file    FleetMgr.c
 * @brief   [fleetConfig] Filo yoneticisi implementasyonu.
 *
 *          - Hicbir arac (marka) header'i include edilmez.
 *          - Araclarin init'ini entegrator degil FleetMgr_Init cagirir.
 *          - Konfigurasyon Init'te BIR KEZ atanan tek bir const pointer'da
 *            tutulur; gosterdigi tablo entegratorde 'const' ve flash'ta.
 *          - Her aracin tek bir arayuzu vardir (tipine gore CarIfc_t veya
 *            TruckIfc_t). Konfigurasyonda 'const void *pVehicle' olarak
 *            durur; kullanildigi yerde type'a bakilip ilgili arayuz tipine
 *            cast edilir (const CarIfc_t *pCar = (const CarIfc_t *)...).
 *          - Ortak islemler (init, bilgi, sur, yakit, bakim) iki arayuzde de
 *            ayni isimle bulunur; FleetMgr bunlari tipe gore dogru arayuzden
 *            cagiran yardimcilarla (vehicleXxx) kullanir.
 *          - Tipe ozel islemlerde once arac tipi kontrol edilir; tip
 *            uymuyorsa araca hic gidilmez (FM_ERR_WRONG_TYPE).
 *          - MISRA: tek cikis noktasi (15.5), switch'te break (16.3),
 *            && / || sag tarafinda yan etkili cagri yok (13.5).
 *            Sapma: void* -> arayuz donusumu (11.5). Her donusum, type
 *            kontrolunden (switch case veya getVehicleOfType) SONRA yapilir.
 */
#include <stddef.h>
#include "FleetMgr.h"

/* NULL = init edilmemis. */
static const FleetMgr_Config_t *s_pCfg = NULL;

/* ======================================================================== */
/*  Ortak islemler: tipe gore dogru arayuzden cagri                         */
/* ======================================================================== */

static FM_Ret_t vehicleInit(const FleetMgr_VehicleCfg_t *pV)
{
    FM_Ret_t ret;

    switch (pV->type)
    {
        case VEHICLE_TYPE_CAR:
        {
            const CarIfc_t *pCar = (const CarIfc_t *)pV->pVehicle;
            ret = pCar->init();
            break;
        }
        case VEHICLE_TYPE_TRUCK:
        {
            const TruckIfc_t *pTruck = (const TruckIfc_t *)pV->pVehicle;
            ret = pTruck->init();
            break;
        }
        default:
            ret = FM_ERR_CONFIG;
            break;
    }
    return ret;
}

static FM_Ret_t vehicleGetInfo(const FleetMgr_VehicleCfg_t *pV, VehicleInfo_t *pInfo)
{
    FM_Ret_t ret;

    switch (pV->type)
    {
        case VEHICLE_TYPE_CAR:
        {
            const CarIfc_t *pCar = (const CarIfc_t *)pV->pVehicle;
            ret = pCar->getInfo(pInfo);
            break;
        }
        case VEHICLE_TYPE_TRUCK:
        {
            const TruckIfc_t *pTruck = (const TruckIfc_t *)pV->pVehicle;
            ret = pTruck->getInfo(pInfo);
            break;
        }
        default:
            ret = FM_ERR_CONFIG;
            break;
    }
    return ret;
}

static FM_Ret_t vehicleDrive(const FleetMgr_VehicleCfg_t *pV, uint32_t km)
{
    FM_Ret_t ret;

    switch (pV->type)
    {
        case VEHICLE_TYPE_CAR:
        {
            const CarIfc_t *pCar = (const CarIfc_t *)pV->pVehicle;
            ret = pCar->drive(km);
            break;
        }
        case VEHICLE_TYPE_TRUCK:
        {
            const TruckIfc_t *pTruck = (const TruckIfc_t *)pV->pVehicle;
            ret = pTruck->drive(km);
            break;
        }
        default:
            ret = FM_ERR_CONFIG;
            break;
    }
    return ret;
}

static FM_Ret_t vehicleRefuel(const FleetMgr_VehicleCfg_t *pV)
{
    FM_Ret_t ret;

    switch (pV->type)
    {
        case VEHICLE_TYPE_CAR:
        {
            const CarIfc_t *pCar = (const CarIfc_t *)pV->pVehicle;
            ret = pCar->refuel();
            break;
        }
        case VEHICLE_TYPE_TRUCK:
        {
            const TruckIfc_t *pTruck = (const TruckIfc_t *)pV->pVehicle;
            ret = pTruck->refuel();
            break;
        }
        default:
            ret = FM_ERR_CONFIG;
            break;
    }
    return ret;
}

static FM_Ret_t vehicleService(const FleetMgr_VehicleCfg_t *pV)
{
    FM_Ret_t ret;

    switch (pV->type)
    {
        case VEHICLE_TYPE_CAR:
        {
            const CarIfc_t *pCar = (const CarIfc_t *)pV->pVehicle;
            ret = pCar->doService();
            break;
        }
        case VEHICLE_TYPE_TRUCK:
        {
            const TruckIfc_t *pTruck = (const TruckIfc_t *)pV->pVehicle;
            ret = pTruck->doService();
            break;
        }
        default:
            ret = FM_ERR_CONFIG;
            break;
    }
    return ret;
}

/* ======================================================================== */
/*  Konfigurasyon dogrulama                                                 */
/* ======================================================================== */

static bool isCarIfcValid(const CarIfc_t *pIfc)
{
    return (pIfc != NULL) && (pIfc->init != NULL) &&
           (pIfc->getInfo != NULL) && (pIfc->drive != NULL) &&
           (pIfc->refuel != NULL) && (pIfc->doService != NULL) &&
           (pIfc->getSeatCount != NULL) && (pIfc->setPassengers != NULL);
}

static bool isTruckIfcValid(const TruckIfc_t *pIfc)
{
    return (pIfc != NULL) && (pIfc->init != NULL) &&
           (pIfc->getInfo != NULL) && (pIfc->drive != NULL) &&
           (pIfc->refuel != NULL) && (pIfc->doService != NULL) &&
           (pIfc->getMaxLoadKg != NULL) && (pIfc->getLoadKg != NULL) &&
           (pIfc->loadCargo != NULL) && (pIfc->unloadCargo != NULL);
}

/* Tipe gore arayuzun tum fonksiyon pointer'lari dolu mu?
 * Not: pVehicle'in gercekten bu tipte bir arayuzu gosterip gostermedigi
 * (void* oldugu icin) burada DOGRULANAMAZ; entegratorun sorumlulugundadir. */
static bool isTypeIfcValid(const FleetMgr_VehicleCfg_t *pV)
{
    bool ok;

    if (pV->pVehicle == NULL)
    {
        ok = false;
    }
    else
    {
        switch (pV->type)
        {
            case VEHICLE_TYPE_CAR:
                ok = isCarIfcValid((const CarIfc_t *)pV->pVehicle);
                break;

            case VEHICLE_TYPE_TRUCK:
                ok = isTruckIfcValid((const TruckIfc_t *)pV->pVehicle);
                break;

            default:
                ok = false;
                break;
        }
    }
    return ok;
}

/* Iki satir ayni araci mi gosteriyor? (ayni ID veya ayni arayuz tablosu) */
static bool isDuplicate(const FleetMgr_VehicleCfg_t *pA, const FleetMgr_VehicleCfg_t *pB)
{
    return (pA->id == pB->id) || (pA->pVehicle == pB->pVehicle);
}

static FM_Ret_t validateVehicle(const FleetMgr_Config_t *pCfg, uint8_t index)
{
    FM_Ret_t ret = FM_OK;
    const FleetMgr_VehicleCfg_t *pV = &pCfg->pVehicles[index];

    if ((pV->id == VEHICLE_ID_INVALID) || !isTypeIfcValid(pV))
    {
        ret = FM_ERR_CONFIG;
    }
    else
    {
        for (uint8_t j = (uint8_t)(index + 1u); (ret == FM_OK) && (j < pCfg->vehicleCount); j++)
        {
            if (isDuplicate(pV, &pCfg->pVehicles[j]))
            {
                ret = FM_ERR_CONFIG;
            }
        }
    }
    return ret;
}

static FM_Ret_t validateCfg(const FleetMgr_Config_t *pCfg)
{
    FM_Ret_t ret = FM_OK;

    if ((pCfg == NULL) || (pCfg->pVehicles == NULL))
    {
        ret = FM_ERR_PARAM;
    }
    else
    {
        for (uint8_t i = 0u; (ret == FM_OK) && (i < pCfg->vehicleCount); i++)
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
static const FleetMgr_VehicleCfg_t *findVehicle(VehicleId_t id)
{
    const FleetMgr_VehicleCfg_t *pFound = NULL;

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
static FM_Ret_t getVehicle(VehicleId_t id, const FleetMgr_VehicleCfg_t **ppV)
{
    FM_Ret_t ret = FM_OK;

    if (s_pCfg == NULL)
    {
        ret = FM_ERR_NOT_INIT;
    }
    else
    {
        *ppV = findVehicle(id);
        if (*ppV == NULL)
        {
            ret = FM_ERR_NOT_FOUND;
        }
    }
    return ret;
}

/* Tipe ozel islemler icin: init + id + TIP kontrolu. */
static FM_Ret_t getVehicleOfType(VehicleId_t id, VehicleType_t type,
                                 const FleetMgr_VehicleCfg_t **ppV)
{
    FM_Ret_t ret = getVehicle(id, ppV);

    if ((ret == FM_OK) && ((*ppV)->type != type))
    {
        ret = FM_ERR_WRONG_TYPE;
    }
    return ret;
}

/* ======================================================================== */
/*  Yasam dongusu ve dolasma                                                */
/* ======================================================================== */

/* 1) Konfigurasyonu dogrular.
 * 2) Konfigurasyondaki her aracin init'ini cagirir (entegrator cagirmaz).
 * Herhangi bir adim basarisizsa FleetMgr init edilmemis kalir ve hata
 * doner: yarim bir filoyla calismaya baslanmaz (ya hep ya hic). */
FM_Ret_t FleetMgr_Init(const FleetMgr_Config_t *pCfg)
{
    FM_Ret_t ret = validateCfg(pCfg);

    s_pCfg = NULL;

    if (ret == FM_OK)   /* pCfg burada gecerli */
    {
        for (uint8_t i = 0u; (ret == FM_OK) && (i < pCfg->vehicleCount); i++)
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

FM_Ret_t FleetMgr_GetVehicleType(VehicleId_t id, VehicleType_t *pType)
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

/* ======================================================================== */
/*  Ortak islemler: her iki tipte de gecerli                                */
/* ======================================================================== */

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

FM_Ret_t FleetMgr_Refuel(VehicleId_t id)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    FM_Ret_t ret = getVehicle(id, &pV);

    if (ret == FM_OK)
    {
        ret = vehicleRefuel(pV);
    }
    return ret;
}

FM_Ret_t FleetMgr_Service(VehicleId_t id)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    FM_Ret_t ret = getVehicle(id, &pV);

    if (ret == FM_OK)
    {
        ret = vehicleService(pV);
    }
    return ret;
}

/* ======================================================================== */
/*  Otomobil arayuzu                                                        */
/* ======================================================================== */

FM_Ret_t FleetMgr_Car_GetSeatCount(VehicleId_t id, uint8_t *pSeats)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    FM_Ret_t ret = getVehicleOfType(id, VEHICLE_TYPE_CAR, &pV);

    if (ret == FM_OK)   /* tip burada dogrulandi */
    {
        const CarIfc_t *pCar = (const CarIfc_t *)pV->pVehicle;

        if (pSeats == NULL)
        {
            ret = FM_ERR_PARAM;
        }
        else
        {
            ret = pCar->getSeatCount(pSeats);
        }
    }
    return ret;
}

FM_Ret_t FleetMgr_Car_SetPassengers(VehicleId_t id, uint8_t count)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    FM_Ret_t ret = getVehicleOfType(id, VEHICLE_TYPE_CAR, &pV);

    if (ret == FM_OK)   /* tip burada dogrulandi */
    {
        const CarIfc_t *pCar = (const CarIfc_t *)pV->pVehicle;

        ret = pCar->setPassengers(count);
    }
    return ret;
}

/* ======================================================================== */
/*  Tir arayuzu                                                             */
/* ======================================================================== */

FM_Ret_t FleetMgr_Truck_GetMaxLoad(VehicleId_t id, uint32_t *pKg)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    FM_Ret_t ret = getVehicleOfType(id, VEHICLE_TYPE_TRUCK, &pV);

    if (ret == FM_OK)   /* tip burada dogrulandi */
    {
        const TruckIfc_t *pTruck = (const TruckIfc_t *)pV->pVehicle;

        if (pKg == NULL)
        {
            ret = FM_ERR_PARAM;
        }
        else
        {
            ret = pTruck->getMaxLoadKg(pKg);
        }
    }
    return ret;
}

FM_Ret_t FleetMgr_Truck_GetLoad(VehicleId_t id, uint32_t *pKg)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    FM_Ret_t ret = getVehicleOfType(id, VEHICLE_TYPE_TRUCK, &pV);

    if (ret == FM_OK)   /* tip burada dogrulandi */
    {
        const TruckIfc_t *pTruck = (const TruckIfc_t *)pV->pVehicle;

        if (pKg == NULL)
        {
            ret = FM_ERR_PARAM;
        }
        else
        {
            ret = pTruck->getLoadKg(pKg);
        }
    }
    return ret;
}

FM_Ret_t FleetMgr_Truck_LoadCargo(VehicleId_t id, uint32_t kg)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    FM_Ret_t ret = getVehicleOfType(id, VEHICLE_TYPE_TRUCK, &pV);

    if (ret == FM_OK)   /* tip burada dogrulandi */
    {
        const TruckIfc_t *pTruck = (const TruckIfc_t *)pV->pVehicle;

        ret = pTruck->loadCargo(kg);
    }
    return ret;
}

FM_Ret_t FleetMgr_Truck_UnloadCargo(VehicleId_t id)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    FM_Ret_t ret = getVehicleOfType(id, VEHICLE_TYPE_TRUCK, &pV);

    if (ret == FM_OK)   /* tip burada dogrulandi */
    {
        const TruckIfc_t *pTruck = (const TruckIfc_t *)pV->pVehicle;

        ret = pTruck->unloadCargo();
    }
    return ret;
}

/* ======================================================================== */
/*  Filo seviyesi: tipe gore yonetim                                        */
/* ======================================================================== */

FM_Ret_t FleetMgr_GetFleetSummary(FleetMgr_Summary_t *pSummary)
{
    FM_Ret_t ret = FM_OK;

    if (s_pCfg == NULL)
    {
        ret = FM_ERR_NOT_INIT;
    }
    else if (pSummary == NULL)
    {
        ret = FM_ERR_PARAM;
    }
    else
    {
        *pSummary = (FleetMgr_Summary_t){ { 0u }, 0u, 0u, 0u };

        for (uint8_t i = 0u; i < s_pCfg->vehicleCount; i++)
        {
            const FleetMgr_VehicleCfg_t *pV = &s_pCfg->pVehicles[i];
            VehicleInfo_t info;
            uint8_t  seats = 0u;
            uint32_t maxKg = 0u;

            pSummary->vehicleCount[pV->type]++;   /* type, Init'te dogrulandi */

            if (vehicleGetInfo(pV, &info) == FM_OK)
            {
                pSummary->totalOdometerKm += info.odometerKm;
            }

            /* Tipe ozel bilgi: FleetMgr tipe gore dogru arayuzu secer. */
            switch (pV->type)
            {
                case VEHICLE_TYPE_CAR:
                {
                    const CarIfc_t *pCar = (const CarIfc_t *)pV->pVehicle;
                    if (pCar->getSeatCount(&seats) == FM_OK)
                    {
                        pSummary->totalSeats += seats;
                    }
                    break;
                }
                case VEHICLE_TYPE_TRUCK:
                {
                    const TruckIfc_t *pTruck = (const TruckIfc_t *)pV->pVehicle;
                    if (pTruck->getMaxLoadKg(&maxKg) == FM_OK)
                    {
                        pSummary->totalMaxLoadKg += maxKg;
                    }
                    break;
                }

                default:
                    break;
            }
        }
    }
    return ret;
}

/* Arac bos mu? */
static bool isAvailable(const FleetMgr_VehicleCfg_t *pV)
{
    VehicleInfo_t info;
    bool available = false;

    if (vehicleGetInfo(pV, &info) == FM_OK)
    {
        available = (info.status == VEHICLE_STATUS_AVAILABLE);
    }
    return available;
}

FM_Ret_t FleetMgr_FindCarForPassengers(uint8_t passengers, VehicleId_t *pId)
{
    FM_Ret_t ret = FM_ERR_NOT_FOUND;

    if (s_pCfg == NULL)
    {
        ret = FM_ERR_NOT_INIT;
    }
    else if (pId == NULL)
    {
        ret = FM_ERR_PARAM;
    }
    else
    {
        uint8_t bestSeats = 0xFFu;

        for (uint8_t i = 0u; i < s_pCfg->vehicleCount; i++)
        {
            const FleetMgr_VehicleCfg_t *pV = &s_pCfg->pVehicles[i];
            uint8_t seats = 0u;

            if ((pV->type == VEHICLE_TYPE_CAR) && isAvailable(pV))
            {
                const CarIfc_t *pCar = (const CarIfc_t *)pV->pVehicle;

                if (pCar->getSeatCount(&seats) == FM_OK)
                {
                    /* yeten en kucuk arac: buyuk araclar buyuk gorevlere kalsin */
                    if ((seats >= passengers) && (seats < bestSeats))
                    {
                        bestSeats = seats;
                        *pId      = pV->id;
                        ret       = FM_OK;
                    }
                }
            }
        }
    }
    return ret;
}

FM_Ret_t FleetMgr_FindTruckForLoad(uint32_t kg, VehicleId_t *pId)
{
    FM_Ret_t ret = FM_ERR_NOT_FOUND;

    if (s_pCfg == NULL)
    {
        ret = FM_ERR_NOT_INIT;
    }
    else if (pId == NULL)
    {
        ret = FM_ERR_PARAM;
    }
    else
    {
        uint32_t bestKg = 0xFFFFFFFFu;

        for (uint8_t i = 0u; i < s_pCfg->vehicleCount; i++)
        {
            const FleetMgr_VehicleCfg_t *pV = &s_pCfg->pVehicles[i];
            uint32_t maxKg = 0u;

            if ((pV->type == VEHICLE_TYPE_TRUCK) && isAvailable(pV))
            {
                const TruckIfc_t *pTruck = (const TruckIfc_t *)pV->pVehicle;

                if (pTruck->getMaxLoadKg(&maxKg) == FM_OK)
                {
                    if ((maxKg >= kg) && (maxKg < bestKg))
                    {
                        bestKg = maxKg;
                        *pId   = pV->id;
                        ret    = FM_OK;
                    }
                }
            }
        }
    }
    return ret;
}

FM_Ret_t FleetMgr_IsServiceDue(VehicleId_t id, bool *pDue)
{
    const FleetMgr_VehicleCfg_t *pV = NULL;
    VehicleInfo_t info;
    FM_Ret_t ret = getVehicle(id, &pV);

    if (ret == FM_OK)
    {
        if (pDue == NULL)
        {
            ret = FM_ERR_PARAM;
        }
        else
        {
            ret = vehicleGetInfo(pV, &info);
            if (ret == FM_OK)
            {
                /* Bakim araligi aracin TIPINE gore konfigurasyondan gelir. */
                *pDue = (info.kmSinceService >= s_pCfg->serviceIntervalKm[pV->type]);
            }
        }
    }
    return ret;
}

FM_Ret_t FleetMgr_RefuelAll(uint8_t *pFailCount)
{
    FM_Ret_t ret = FM_OK;
    uint8_t fails = 0u;

    if (s_pCfg == NULL)
    {
        ret = FM_ERR_NOT_INIT;
    }
    else
    {
        for (uint8_t i = 0u; i < s_pCfg->vehicleCount; i++)
        {
            if (vehicleRefuel(&s_pCfg->pVehicles[i]) != FM_OK)
            {
                fails++;
            }
        }
    }

    if (pFailCount != NULL)
    {
        *pFailCount = fails;
    }
    return ret;
}
