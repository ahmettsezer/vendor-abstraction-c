/**
 * @file    Toyota.c
 * @brief   [fleetConfig] Toyota otomobil implementasyonu.
 *
 *          Yakit tuketimi: 100 km'de depo yuzdesi olarak sabit (modele gore).
 *          Not: printf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "Toyota.h"

/* Modelin degismeyen ozellikleri + teslim alindigi andaki durumu. */
typedef struct
{
    const char *pModel;
    uint16_t    modelYear;
    FuelType_t  fuelType;
    uint8_t     seats;
    uint8_t     consumptionPctPer100Km;
    uint32_t    initOdometerKm;
    uint32_t    initKmSinceService;
    uint8_t     initFuelPct;
} ToyotaModel_t;

/* Aracin degisen durumu. */
typedef struct
{
    uint32_t odometerKm;
    uint32_t kmSinceService;
    uint8_t  fuelPct;
    uint8_t  passengers;
} ToyotaState_t;

static const ToyotaModel_t s_models[TOYOTA_VEHICLE_COUNT] =
{
    /* TOYOTA_COROLLA      */ { "Corolla",      2021u, FUEL_HYBRID, 5u, 5u, 48200u, 14800u, 70u },
    /* TOYOTA_PROACE_VERSO */ { "Proace Verso", 2022u, FUEL_DIESEL, 8u, 8u, 31500u,  9000u, 55u },
};

static ToyotaState_t s_state[TOYOTA_VEHICLE_COUNT];

/* ---- Ortak arayuz ------------------------------------------------------- */
static MM_Ret_t toyota_getInfo(uint8_t instance, VehicleInfo_t *pInfo)
{
    MM_Ret_t ret = MM_OK;

    if (instance >= TOYOTA_VEHICLE_COUNT)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        const ToyotaModel_t *pM = &s_models[instance];
        const ToyotaState_t *pS = &s_state[instance];

        pInfo->pBrand         = "Toyota";
        pInfo->pModel         = pM->pModel;
        pInfo->modelYear      = pM->modelYear;
        pInfo->fuelType       = pM->fuelType;
        pInfo->odometerKm     = pS->odometerKm;
        pInfo->kmSinceService = pS->kmSinceService;
        pInfo->fuelLevelPct   = pS->fuelPct;
        pInfo->status         = (pS->passengers > 0u) ? VEHICLE_STATUS_IN_USE : VEHICLE_STATUS_AVAILABLE;
    }
    return ret;
}

static MM_Ret_t toyota_drive(uint8_t instance, uint32_t km)
{
    MM_Ret_t ret = MM_OK;

    if (instance >= TOYOTA_VEHICLE_COUNT)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        ToyotaState_t *pS = &s_state[instance];
        uint32_t needPct = (km * s_models[instance].consumptionPctPer100Km) / 100u;

        if (needPct > (uint32_t)pS->fuelPct)
        {
            (void)printf("    [Toyota] %s: %lu km icin yakit yetersiz (gerekli %%%lu, depoda %%%u)\n",
                         s_models[instance].pModel, (unsigned long)km,
                         (unsigned long)needPct, (unsigned)pS->fuelPct);
            ret = MM_ERR_LOW_FUEL;
        }
        else
        {
            pS->odometerKm     += km;
            pS->kmSinceService += km;
            pS->fuelPct         = (uint8_t)((uint32_t)pS->fuelPct - needPct);
            (void)printf("    [Toyota] %s: %lu km gidildi, depo %%%u\n",
                         s_models[instance].pModel, (unsigned long)km, (unsigned)pS->fuelPct);
        }
    }
    return ret;
}

static MM_Ret_t toyota_refuel(uint8_t instance)
{
    MM_Ret_t ret = MM_OK;

    if (instance >= TOYOTA_VEHICLE_COUNT)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        s_state[instance].fuelPct = 100u;
        (void)printf("    [Toyota] %s: depo dolduruldu\n", s_models[instance].pModel);
    }
    return ret;
}

static MM_Ret_t toyota_doService(uint8_t instance)
{
    MM_Ret_t ret = MM_OK;

    if (instance >= TOYOTA_VEHICLE_COUNT)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        s_state[instance].kmSinceService = 0u;
        (void)printf("    [Toyota] %s: bakim yapildi\n", s_models[instance].pModel);
    }
    return ret;
}

/* ---- Otomobil arayuzu --------------------------------------------------- */
static MM_Ret_t toyota_getSeatCount(uint8_t instance, uint8_t *pSeats)
{
    MM_Ret_t ret = MM_OK;

    if (instance >= TOYOTA_VEHICLE_COUNT)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        *pSeats = s_models[instance].seats;
    }
    return ret;
}

static MM_Ret_t toyota_setPassengers(uint8_t instance, uint8_t count)
{
    MM_Ret_t ret = MM_OK;

    if (instance >= TOYOTA_VEHICLE_COUNT)
    {
        ret = MM_ERR_PARAM;
    }
    else if (count > s_models[instance].seats)
    {
        (void)printf("    [Toyota] %s: %u yolcu sigmaz (koltuk %u)\n",
                     s_models[instance].pModel, (unsigned)count, (unsigned)s_models[instance].seats);
        ret = MM_ERR_OVER_CAPACITY;
    }
    else
    {
        s_state[instance].passengers = count;
        if (count > 0u)
        {
            (void)printf("    [Toyota] %s: %u yolcu bindi\n", s_models[instance].pModel, (unsigned)count);
        }
        else
        {
            (void)printf("    [Toyota] %s: yolcular indi\n", s_models[instance].pModel);
        }
    }
    return ret;
}

/* ---- Arayuz tablolari: const, tanimlandigi satirda dolu ----------------- */
const VehicleCommonIfc_t Toyota_CommonIfc =
{
    .getInfo   = toyota_getInfo,
    .drive     = toyota_drive,
    .refuel    = toyota_refuel,
    .doService = toyota_doService,
};

const CarIfc_t Toyota_CarIfc =
{
    .getSeatCount  = toyota_getSeatCount,
    .setPassengers = toyota_setPassengers,
};

MM_Ret_t Toyota_Init(void)
{
    for (uint8_t i = 0u; i < TOYOTA_VEHICLE_COUNT; i++)
    {
        s_state[i].odometerKm     = s_models[i].initOdometerKm;
        s_state[i].kmSinceService = s_models[i].initKmSinceService;
        s_state[i].fuelPct        = s_models[i].initFuelPct;
        s_state[i].passengers     = 0u;
    }
    return MM_OK;
}
