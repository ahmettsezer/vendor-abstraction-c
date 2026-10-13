/**
 * @file    Toyota.c
 * @brief   [fleetConfig] Toyota Corolla otomobil implementasyonu.
 *
 *          Yakit tuketimi: 100 km'de depo yuzdesi olarak sabit.
 *          Not: printf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "Toyota.h"

/* Aracin degismeyen ozellikleri + teslim alindigi andaki durumu. */
#define TOYOTA_MODEL              "Corolla"
#define TOYOTA_MODEL_YEAR         (2021u)
#define TOYOTA_FUEL_TYPE          (FUEL_HYBRID)
#define TOYOTA_SEATS              (5u)
#define TOYOTA_PCT_PER_100KM      (5u)
#define TOYOTA_INIT_ODOMETER_KM   (48200u)
#define TOYOTA_INIT_SERVICE_KM    (14800u)
#define TOYOTA_INIT_FUEL_PCT      (70u)

/* Aracin degisen durumu. */
static uint32_t s_odometerKm     = 0u;
static uint32_t s_kmSinceService = 0u;
static uint8_t  s_fuelPct        = 0u;
static uint8_t  s_passengers     = 0u;

/* ---- Ortak islemler ----------------------------------------------------- */
static MM_Ret_t toyota_getInfo(VehicleInfo_t *pInfo)
{
    pInfo->pBrand         = "Toyota";
    pInfo->pModel         = TOYOTA_MODEL;
    pInfo->modelYear      = TOYOTA_MODEL_YEAR;
    pInfo->fuelType       = TOYOTA_FUEL_TYPE;
    pInfo->odometerKm     = s_odometerKm;
    pInfo->kmSinceService = s_kmSinceService;
    pInfo->fuelLevelPct   = s_fuelPct;
    pInfo->status         = (s_passengers > 0u) ? VEHICLE_STATUS_IN_USE : VEHICLE_STATUS_AVAILABLE;
    return MM_OK;
}

static MM_Ret_t toyota_drive(uint32_t km)
{
    MM_Ret_t ret = MM_OK;
    uint32_t needPct = (km * TOYOTA_PCT_PER_100KM) / 100u;

    if (needPct > (uint32_t)s_fuelPct)
    {
        (void)printf("    [Toyota] " TOYOTA_MODEL ": %lu km icin yakit yetersiz (gerekli %%%lu, depoda %%%u)\n",
                     (unsigned long)km, (unsigned long)needPct, (unsigned)s_fuelPct);
        ret = MM_ERR_LOW_FUEL;
    }
    else
    {
        s_odometerKm     += km;
        s_kmSinceService += km;
        s_fuelPct         = (uint8_t)((uint32_t)s_fuelPct - needPct);
        (void)printf("    [Toyota] " TOYOTA_MODEL ": %lu km gidildi, depo %%%u\n",
                     (unsigned long)km, (unsigned)s_fuelPct);
    }
    return ret;
}

static MM_Ret_t toyota_refuel(void)
{
    s_fuelPct = 100u;
    (void)printf("    [Toyota] " TOYOTA_MODEL ": depo dolduruldu\n");
    return MM_OK;
}

static MM_Ret_t toyota_doService(void)
{
    s_kmSinceService = 0u;
    (void)printf("    [Toyota] " TOYOTA_MODEL ": bakim yapildi\n");
    return MM_OK;
}

/* ---- Otomobile ozel islemler -------------------------------------------- */
static MM_Ret_t toyota_getSeatCount(uint8_t *pSeats)
{
    *pSeats = TOYOTA_SEATS;
    return MM_OK;
}

static MM_Ret_t toyota_setPassengers(uint8_t count)
{
    MM_Ret_t ret = MM_OK;

    if (count > TOYOTA_SEATS)
    {
        (void)printf("    [Toyota] " TOYOTA_MODEL ": %u yolcu sigmaz (koltuk %u)\n",
                     (unsigned)count, (unsigned)TOYOTA_SEATS);
        ret = MM_ERR_OVER_CAPACITY;
    }
    else
    {
        s_passengers = count;
        if (count > 0u)
        {
            (void)printf("    [Toyota] " TOYOTA_MODEL ": %u yolcu bindi\n", (unsigned)count);
        }
        else
        {
            (void)printf("    [Toyota] " TOYOTA_MODEL ": yolcular indi\n");
        }
    }
    return ret;
}

/* ---- Arayuz tablosu: const, tanimlandigi satirda dolu ------------------- */
const CarIfc_t Toyota_CarIfc =
{
    .common =
    {
        .getInfo   = toyota_getInfo,
        .drive     = toyota_drive,
        .refuel    = toyota_refuel,
        .doService = toyota_doService,
    },
    .getSeatCount  = toyota_getSeatCount,
    .setPassengers = toyota_setPassengers,
};

MM_Ret_t Toyota_Init(void)
{
    s_odometerKm     = TOYOTA_INIT_ODOMETER_KM;
    s_kmSinceService = TOYOTA_INIT_SERVICE_KM;
    s_fuelPct        = TOYOTA_INIT_FUEL_PCT;
    s_passengers     = 0u;
    return MM_OK;
}
