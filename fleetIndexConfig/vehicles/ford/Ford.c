/**
 * @file    Ford.c
 * @brief   [fleetIndexConfig] Ford Tourneo Custom otomobil implementasyonu.
 *
 *          Not: printf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "Ford.h"

#define FORD_MODEL              "Tourneo Custom"
#define FORD_MODEL_YEAR         (2022u)
#define FORD_FUEL_TYPE          (FUEL_DIESEL)
#define FORD_SEATS              (8u)
#define FORD_PCT_PER_100KM      (8u)
#define FORD_INIT_ODOMETER_KM   (31500u)
#define FORD_INIT_FUEL_PCT      (55u)

static uint32_t s_odometerKm = 0u;
static uint8_t  s_fuelPct    = 0u;
static uint8_t  s_passengers = 0u;

static FM_Ret_t ford_init(void)
{
    s_odometerKm = FORD_INIT_ODOMETER_KM;
    s_fuelPct    = FORD_INIT_FUEL_PCT;
    s_passengers = 0u;
    (void)printf("    [Ford] " FORD_MODEL ": init\n");
    return FM_OK;
}

static FM_Ret_t ford_getInfo(VehicleInfo_t *pInfo)
{
    pInfo->pBrand       = "Ford";
    pInfo->pModel       = FORD_MODEL;
    pInfo->modelYear    = FORD_MODEL_YEAR;
    pInfo->fuelType     = FORD_FUEL_TYPE;
    pInfo->odometerKm   = s_odometerKm;
    pInfo->fuelLevelPct = s_fuelPct;
    pInfo->status       = (s_passengers > 0u) ? VEHICLE_STATUS_IN_USE : VEHICLE_STATUS_AVAILABLE;
    return FM_OK;
}

static FM_Ret_t ford_drive(uint32_t km)
{
    FM_Ret_t ret = FM_OK;
    uint32_t needPct = (km * FORD_PCT_PER_100KM) / 100u;

    if (needPct > (uint32_t)s_fuelPct)
    {
        ret = FM_ERR_LOW_FUEL;
    }
    else
    {
        s_odometerKm += km;
        s_fuelPct     = (uint8_t)((uint32_t)s_fuelPct - needPct);
        (void)printf("    [Ford] " FORD_MODEL ": %lu km gidildi, depo %%%u\n",
                     (unsigned long)km, (unsigned)s_fuelPct);
    }
    return ret;
}

static FM_Ret_t ford_setPassengers(uint8_t count)
{
    FM_Ret_t ret = FM_OK;

    if (count > FORD_SEATS)
    {
        (void)printf("    [Ford] " FORD_MODEL ": %u yolcu sigmaz (koltuk %u)\n",
                     (unsigned)count, (unsigned)FORD_SEATS);
        ret = FM_ERR_OVER_CAPACITY;
    }
    else
    {
        s_passengers = count;
        (void)printf("    [Ford] " FORD_MODEL ": %u yolcu\n", (unsigned)count);
    }
    return ret;
}

const CarIfc_t Ford_CarIfc =
{
    .init          = ford_init,
    .getInfo       = ford_getInfo,
    .drive         = ford_drive,
    .setPassengers = ford_setPassengers,
};
