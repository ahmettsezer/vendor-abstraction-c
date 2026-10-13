/**
 * @file    Volvo.c
 * @brief   [fleetIndexConfig] Volvo FH tir implementasyonu.
 *
 *          Yakit tuketimi (100 km'de depo yuzdesi): bos tuketim + her 6 ton icin %1
 *          Not: printf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "Volvo.h"

#define VOLVO_MODEL                 "FH 500"
#define VOLVO_MODEL_YEAR            (2023u)
#define VOLVO_FUEL_TYPE             (FUEL_DIESEL)
#define VOLVO_MAX_LOAD_KG           (25000u)
#define VOLVO_BASE_PCT_PER_100KM    (8u)
#define VOLVO_KG_PER_EXTRA_PCT      (6000u)
#define VOLVO_INIT_ODOMETER_KM      (96000u)
#define VOLVO_INIT_FUEL_PCT         (60u)

static uint32_t s_odometerKm = 0u;
static uint8_t  s_fuelPct    = 0u;
static uint32_t s_loadKg     = 0u;

static FM_Ret_t volvo_init(void)
{
    s_odometerKm = VOLVO_INIT_ODOMETER_KM;
    s_fuelPct    = VOLVO_INIT_FUEL_PCT;
    s_loadKg     = 0u;
    (void)printf("    [Volvo] " VOLVO_MODEL ": init\n");
    return FM_OK;
}

static FM_Ret_t volvo_getInfo(VehicleInfo_t *pInfo)
{
    pInfo->pBrand       = "Volvo";
    pInfo->pModel       = VOLVO_MODEL;
    pInfo->modelYear    = VOLVO_MODEL_YEAR;
    pInfo->fuelType     = VOLVO_FUEL_TYPE;
    pInfo->odometerKm   = s_odometerKm;
    pInfo->fuelLevelPct = s_fuelPct;
    pInfo->status       = (s_loadKg > 0u) ? VEHICLE_STATUS_IN_USE : VEHICLE_STATUS_AVAILABLE;
    return FM_OK;
}

static FM_Ret_t volvo_drive(uint32_t km)
{
    FM_Ret_t ret = FM_OK;
    uint32_t pctPer100Km = VOLVO_BASE_PCT_PER_100KM + (s_loadKg / VOLVO_KG_PER_EXTRA_PCT);
    uint32_t needPct     = (km * pctPer100Km) / 100u;

    if (needPct > (uint32_t)s_fuelPct)
    {
        ret = FM_ERR_LOW_FUEL;
    }
    else
    {
        s_odometerKm += km;
        s_fuelPct     = (uint8_t)((uint32_t)s_fuelPct - needPct);
        (void)printf("    [Volvo] " VOLVO_MODEL ": %lu km gidildi (%lu kg yukle), depo %%%u\n",
                     (unsigned long)km, (unsigned long)s_loadKg, (unsigned)s_fuelPct);
    }
    return ret;
}

static FM_Ret_t volvo_loadCargo(uint32_t kg)
{
    FM_Ret_t ret = FM_OK;

    if ((s_loadKg + kg) > VOLVO_MAX_LOAD_KG)
    {
        (void)printf("    [Volvo] " VOLVO_MODEL ": +%lu kg yuklenemez (kapasite %lu kg)\n",
                     (unsigned long)kg, (unsigned long)VOLVO_MAX_LOAD_KG);
        ret = FM_ERR_OVER_CAPACITY;
    }
    else
    {
        s_loadKg += kg;
        (void)printf("    [Volvo] " VOLVO_MODEL ": %lu kg yuklendi\n", (unsigned long)kg);
    }
    return ret;
}

const TruckIfc_t Volvo_TruckIfc =
{
    .init      = volvo_init,
    .getInfo   = volvo_getInfo,
    .drive     = volvo_drive,
    .loadCargo = volvo_loadCargo,
};
