/**
 * @file    Man.c
 * @brief   [fleetConfig] MAN tir implementasyonu.
 *
 *          Yakit tuketimi (100 km'de depo yuzdesi):
 *              bos tuketim + her 5 ton yuk icin %1
 *          Not: printf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "Man.h"

#define MAN_KG_PER_EXTRA_PCT    (5000u)   /* 5 ton yuk = 100 km'de +%1 */

typedef struct
{
    const char *pModel;
    uint16_t    modelYear;
    FuelType_t  fuelType;
    uint32_t    maxLoadKg;
    uint8_t     baseConsumptionPctPer100Km;
    uint32_t    initOdometerKm;
    uint32_t    initKmSinceService;
    uint8_t     initFuelPct;
} ManModel_t;

typedef struct
{
    uint32_t odometerKm;
    uint32_t kmSinceService;
    uint8_t  fuelPct;
    uint32_t loadKg;
} ManState_t;

static const ManModel_t s_models[MAN_VEHICLE_COUNT] =
{
    /* MAN_TGX */ { "TGX 18.510", 2020u, FUEL_DIESEL, 20000u, 9u, 412000u, 29700u, 80u },
};

static ManState_t s_state[MAN_VEHICLE_COUNT];

/* ---- Ortak arayuz ------------------------------------------------------- */
static MM_Ret_t man_getInfo(uint8_t instance, VehicleInfo_t *pInfo)
{
    MM_Ret_t ret = MM_OK;

    if (instance >= MAN_VEHICLE_COUNT)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        const ManModel_t *pM = &s_models[instance];
        const ManState_t *pS = &s_state[instance];

        pInfo->pBrand         = "MAN";
        pInfo->pModel         = pM->pModel;
        pInfo->modelYear      = pM->modelYear;
        pInfo->fuelType       = pM->fuelType;
        pInfo->odometerKm     = pS->odometerKm;
        pInfo->kmSinceService = pS->kmSinceService;
        pInfo->fuelLevelPct   = pS->fuelPct;
        pInfo->status         = (pS->loadKg > 0u) ? VEHICLE_STATUS_IN_USE : VEHICLE_STATUS_AVAILABLE;
    }
    return ret;
}

static MM_Ret_t man_drive(uint8_t instance, uint32_t km)
{
    MM_Ret_t ret = MM_OK;

    if (instance >= MAN_VEHICLE_COUNT)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        ManState_t *pS = &s_state[instance];
        uint32_t pctPer100Km = (uint32_t)s_models[instance].baseConsumptionPctPer100Km +
                               (pS->loadKg / MAN_KG_PER_EXTRA_PCT);
        uint32_t needPct = (km * pctPer100Km) / 100u;

        if (needPct > (uint32_t)pS->fuelPct)
        {
            (void)printf("    [MAN] %s: %lu km icin yakit yetersiz (gerekli %%%lu, depoda %%%u)\n",
                         s_models[instance].pModel, (unsigned long)km,
                         (unsigned long)needPct, (unsigned)pS->fuelPct);
            ret = MM_ERR_LOW_FUEL;
        }
        else
        {
            pS->odometerKm     += km;
            pS->kmSinceService += km;
            pS->fuelPct         = (uint8_t)((uint32_t)pS->fuelPct - needPct);
            (void)printf("    [MAN] %s: %lu km gidildi (%lu kg yukle), depo %%%u\n",
                         s_models[instance].pModel, (unsigned long)km,
                         (unsigned long)pS->loadKg, (unsigned)pS->fuelPct);
        }
    }
    return ret;
}

static MM_Ret_t man_refuel(uint8_t instance)
{
    MM_Ret_t ret = MM_OK;

    if (instance >= MAN_VEHICLE_COUNT)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        s_state[instance].fuelPct = 100u;
        (void)printf("    [MAN] %s: depo dolduruldu\n", s_models[instance].pModel);
    }
    return ret;
}

static MM_Ret_t man_doService(uint8_t instance)
{
    MM_Ret_t ret = MM_OK;

    if (instance >= MAN_VEHICLE_COUNT)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        s_state[instance].kmSinceService = 0u;
        (void)printf("    [MAN] %s: bakim yapildi\n", s_models[instance].pModel);
    }
    return ret;
}

/* ---- Tir arayuzu -------------------------------------------------------- */
static MM_Ret_t man_getMaxLoadKg(uint8_t instance, uint32_t *pKg)
{
    MM_Ret_t ret = MM_OK;

    if (instance >= MAN_VEHICLE_COUNT)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        *pKg = s_models[instance].maxLoadKg;
    }
    return ret;
}

static MM_Ret_t man_getLoadKg(uint8_t instance, uint32_t *pKg)
{
    MM_Ret_t ret = MM_OK;

    if (instance >= MAN_VEHICLE_COUNT)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        *pKg = s_state[instance].loadKg;
    }
    return ret;
}

static MM_Ret_t man_loadCargo(uint8_t instance, uint32_t kg)
{
    MM_Ret_t ret = MM_OK;

    if (instance >= MAN_VEHICLE_COUNT)
    {
        ret = MM_ERR_PARAM;
    }
    else if ((s_state[instance].loadKg + kg) > s_models[instance].maxLoadKg)
    {
        (void)printf("    [MAN] %s: +%lu kg yuklenemez (mevcut %lu, kapasite %lu kg)\n",
                     s_models[instance].pModel, (unsigned long)kg,
                     (unsigned long)s_state[instance].loadKg,
                     (unsigned long)s_models[instance].maxLoadKg);
        ret = MM_ERR_OVER_CAPACITY;
    }
    else
    {
        s_state[instance].loadKg += kg;
        (void)printf("    [MAN] %s: %lu kg yuklendi (toplam %lu kg)\n",
                     s_models[instance].pModel, (unsigned long)kg,
                     (unsigned long)s_state[instance].loadKg);
    }
    return ret;
}

static MM_Ret_t man_unloadCargo(uint8_t instance)
{
    MM_Ret_t ret = MM_OK;

    if (instance >= MAN_VEHICLE_COUNT)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        s_state[instance].loadKg = 0u;
        (void)printf("    [MAN] %s: yuk bosaltildi\n", s_models[instance].pModel);
    }
    return ret;
}

/* ---- Arayuz tablolari: const, tanimlandigi satirda dolu ----------------- */
const VehicleCommonIfc_t Man_CommonIfc =
{
    .getInfo   = man_getInfo,
    .drive     = man_drive,
    .refuel    = man_refuel,
    .doService = man_doService,
};

const TruckIfc_t Man_TruckIfc =
{
    .getMaxLoadKg = man_getMaxLoadKg,
    .getLoadKg    = man_getLoadKg,
    .loadCargo    = man_loadCargo,
    .unloadCargo  = man_unloadCargo,
};

MM_Ret_t Man_Init(void)
{
    for (uint8_t i = 0u; i < MAN_VEHICLE_COUNT; i++)
    {
        s_state[i].odometerKm     = s_models[i].initOdometerKm;
        s_state[i].kmSinceService = s_models[i].initKmSinceService;
        s_state[i].fuelPct        = s_models[i].initFuelPct;
        s_state[i].loadKg         = 0u;
    }
    return MM_OK;
}
