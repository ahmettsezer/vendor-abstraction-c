/**
 * @file    Toyota.c
 * @brief   [fleetConfigB] Toyota Corolla otomobil implementasyonu.
 *
 *          Not: printf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "Toyota.h"

#define TOYOTA_MODEL              "Corolla"
#define TOYOTA_MODEL_YEAR         (2021u)
#define TOYOTA_FUEL_TYPE          (FUEL_HYBRID)
#define TOYOTA_SEATS              (5u)
#define TOYOTA_PCT_PER_100KM      (5u)
#define TOYOTA_INIT_ODOMETER_KM   (48200u)
#define TOYOTA_INIT_FUEL_PCT      (70u)

static uint32_t s_odometerKm = 0u;
static uint8_t  s_fuelPct    = 0u;
static uint8_t  s_passengers = 0u;

static FM_Ret_t toyota_init(void)
{
    s_odometerKm = TOYOTA_INIT_ODOMETER_KM;
    s_fuelPct    = TOYOTA_INIT_FUEL_PCT;
    s_passengers = 0u;
    (void)printf("    [Toyota] " TOYOTA_MODEL ": init\n");
    return FM_OK;
}

static FM_Ret_t toyota_getInfo(VehicleInfo_t *pInfo)
{
    pInfo->pBrand       = "Toyota";
    pInfo->pModel       = TOYOTA_MODEL;
    pInfo->modelYear    = TOYOTA_MODEL_YEAR;
    pInfo->fuelType     = TOYOTA_FUEL_TYPE;
    pInfo->odometerKm   = s_odometerKm;
    pInfo->fuelLevelPct = s_fuelPct;
    pInfo->status       = (s_passengers > 0u) ? VEHICLE_STATUS_IN_USE : VEHICLE_STATUS_AVAILABLE;
    return FM_OK;
}

static FM_Ret_t toyota_drive(uint32_t km)
{
    FM_Ret_t ret = FM_OK;
    uint32_t needPct = (km * TOYOTA_PCT_PER_100KM) / 100u;

    if (needPct > (uint32_t)s_fuelPct)
    {
        ret = FM_ERR_LOW_FUEL;
    }
    else
    {
        s_odometerKm += km;
        s_fuelPct     = (uint8_t)((uint32_t)s_fuelPct - needPct);
        (void)printf("    [Toyota] " TOYOTA_MODEL ": %lu km gidildi, depo %%%u\n",
                     (unsigned long)km, (unsigned)s_fuelPct);
    }
    return ret;
}

static FM_Ret_t toyota_setPassengers(uint8_t count)
{
    FM_Ret_t ret = FM_OK;

    if (count > TOYOTA_SEATS)
    {
        (void)printf("    [Toyota] " TOYOTA_MODEL ": %u yolcu sigmaz (koltuk %u)\n",
                     (unsigned)count, (unsigned)TOYOTA_SEATS);
        ret = FM_ERR_OVER_CAPACITY;
    }
    else
    {
        s_passengers = count;
        (void)printf("    [Toyota] " TOYOTA_MODEL ": %u yolcu\n", (unsigned)count);
    }
    return ret;
}

const CarIfc_t Toyota_CarIfc =
{
    .init          = toyota_init,
    .getInfo       = toyota_getInfo,
    .drive         = toyota_drive,
    .setPassengers = toyota_setPassengers,
};
