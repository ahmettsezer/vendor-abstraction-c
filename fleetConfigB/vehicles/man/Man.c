/**
 * @file    Man.c
 * @brief   [fleetConfigB] MAN TGX tir implementasyonu.
 *
 *          Yakit tuketimi (100 km'de depo yuzdesi): bos tuketim + her 5 ton icin %1
 *          Not: printf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "Man.h"

#define MAN_MODEL                 "TGX 18.510"
#define MAN_MODEL_YEAR            (2020u)
#define MAN_FUEL_TYPE             (FUEL_DIESEL)
#define MAN_MAX_LOAD_KG           (20000u)
#define MAN_BASE_PCT_PER_100KM    (9u)
#define MAN_KG_PER_EXTRA_PCT      (5000u)
#define MAN_INIT_ODOMETER_KM      (412000u)
#define MAN_INIT_FUEL_PCT         (80u)

static uint32_t s_odometerKm = 0u;
static uint8_t  s_fuelPct    = 0u;
static uint32_t s_loadKg     = 0u;

static FM_Ret_t man_init(void)
{
    s_odometerKm = MAN_INIT_ODOMETER_KM;
    s_fuelPct    = MAN_INIT_FUEL_PCT;
    s_loadKg     = 0u;
    (void)printf("    [MAN] " MAN_MODEL ": init\n");
    return FM_OK;
}

static FM_Ret_t man_getInfo(VehicleInfo_t *pInfo)
{
    pInfo->pBrand       = "MAN";
    pInfo->pModel       = MAN_MODEL;
    pInfo->modelYear    = MAN_MODEL_YEAR;
    pInfo->fuelType     = MAN_FUEL_TYPE;
    pInfo->odometerKm   = s_odometerKm;
    pInfo->fuelLevelPct = s_fuelPct;
    pInfo->status       = (s_loadKg > 0u) ? VEHICLE_STATUS_IN_USE : VEHICLE_STATUS_AVAILABLE;
    return FM_OK;
}

static FM_Ret_t man_drive(uint32_t km)
{
    FM_Ret_t ret = FM_OK;
    uint32_t pctPer100Km = MAN_BASE_PCT_PER_100KM + (s_loadKg / MAN_KG_PER_EXTRA_PCT);
    uint32_t needPct     = (km * pctPer100Km) / 100u;

    if (needPct > (uint32_t)s_fuelPct)
    {
        ret = FM_ERR_LOW_FUEL;
    }
    else
    {
        s_odometerKm += km;
        s_fuelPct     = (uint8_t)((uint32_t)s_fuelPct - needPct);
        (void)printf("    [MAN] " MAN_MODEL ": %lu km gidildi (%lu kg yukle), depo %%%u\n",
                     (unsigned long)km, (unsigned long)s_loadKg, (unsigned)s_fuelPct);
    }
    return ret;
}

static FM_Ret_t man_loadCargo(uint32_t kg)
{
    FM_Ret_t ret = FM_OK;

    if ((s_loadKg + kg) > MAN_MAX_LOAD_KG)
    {
        (void)printf("    [MAN] " MAN_MODEL ": +%lu kg yuklenemez (kapasite %lu kg)\n",
                     (unsigned long)kg, (unsigned long)MAN_MAX_LOAD_KG);
        ret = FM_ERR_OVER_CAPACITY;
    }
    else
    {
        s_loadKg += kg;
        (void)printf("    [MAN] " MAN_MODEL ": %lu kg yuklendi\n", (unsigned long)kg);
    }
    return ret;
}

const TruckIfc_t Man_TruckIfc =
{
    .init      = man_init,
    .getInfo   = man_getInfo,
    .drive     = man_drive,
    .loadCargo = man_loadCargo,
};
