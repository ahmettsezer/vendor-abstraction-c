/**
 * @file    Volvo.c
 * @brief   [fleetConfig] Volvo FH tir implementasyonu.
 *
 *          Yakit tuketimi (100 km'de depo yuzdesi):
 *              bos tuketim + her 6 ton yuk icin %1
 *          Not: printf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "Volvo.h"

/* Aracin degismeyen ozellikleri + teslim alindigi andaki durumu. */
#define VOLVO_MODEL                 "FH 500"
#define VOLVO_MODEL_YEAR            (2023u)
#define VOLVO_FUEL_TYPE             (FUEL_DIESEL)
#define VOLVO_MAX_LOAD_KG           (25000u)
#define VOLVO_BASE_PCT_PER_100KM    (8u)
#define VOLVO_KG_PER_EXTRA_PCT      (6000u)   /* 6 ton yuk = 100 km'de +%1 */
#define VOLVO_INIT_ODOMETER_KM      (96000u)
#define VOLVO_INIT_SERVICE_KM       (12000u)
#define VOLVO_INIT_FUEL_PCT         (60u)

/* Aracin degisen durumu. */
static uint32_t s_odometerKm     = 0u;
static uint32_t s_kmSinceService = 0u;
static uint8_t  s_fuelPct        = 0u;
static uint32_t s_loadKg         = 0u;

/* ---- Init: entegrator degil ModuleMgr_Init cagirir ---------------------- */
static MM_Ret_t volvo_init(void)
{
    s_odometerKm     = VOLVO_INIT_ODOMETER_KM;
    s_kmSinceService = VOLVO_INIT_SERVICE_KM;
    s_fuelPct        = VOLVO_INIT_FUEL_PCT;
    s_loadKg         = 0u;
    (void)printf("    [Volvo] " VOLVO_MODEL ": init (%lu km, depo %%%u)\n",
                 (unsigned long)s_odometerKm, (unsigned)s_fuelPct);
    return MM_OK;
}

/* ---- Ortak islemler ----------------------------------------------------- */
static MM_Ret_t volvo_getInfo(VehicleInfo_t *pInfo)
{
    pInfo->pBrand         = "Volvo";
    pInfo->pModel         = VOLVO_MODEL;
    pInfo->modelYear      = VOLVO_MODEL_YEAR;
    pInfo->fuelType       = VOLVO_FUEL_TYPE;
    pInfo->odometerKm     = s_odometerKm;
    pInfo->kmSinceService = s_kmSinceService;
    pInfo->fuelLevelPct   = s_fuelPct;
    pInfo->status         = (s_loadKg > 0u) ? VEHICLE_STATUS_IN_USE : VEHICLE_STATUS_AVAILABLE;
    return MM_OK;
}

static MM_Ret_t volvo_drive(uint32_t km)
{
    MM_Ret_t ret = MM_OK;
    uint32_t pctPer100Km = VOLVO_BASE_PCT_PER_100KM + (s_loadKg / VOLVO_KG_PER_EXTRA_PCT);
    uint32_t needPct     = (km * pctPer100Km) / 100u;

    if (needPct > (uint32_t)s_fuelPct)
    {
        (void)printf("    [Volvo] " VOLVO_MODEL ": %lu km icin yakit yetersiz (gerekli %%%lu, depoda %%%u)\n",
                     (unsigned long)km, (unsigned long)needPct, (unsigned)s_fuelPct);
        ret = MM_ERR_LOW_FUEL;
    }
    else
    {
        s_odometerKm     += km;
        s_kmSinceService += km;
        s_fuelPct         = (uint8_t)((uint32_t)s_fuelPct - needPct);
        (void)printf("    [Volvo] " VOLVO_MODEL ": %lu km gidildi (%lu kg yukle), depo %%%u\n",
                     (unsigned long)km, (unsigned long)s_loadKg, (unsigned)s_fuelPct);
    }
    return ret;
}

static MM_Ret_t volvo_refuel(void)
{
    s_fuelPct = 100u;
    (void)printf("    [Volvo] " VOLVO_MODEL ": depo dolduruldu\n");
    return MM_OK;
}

static MM_Ret_t volvo_doService(void)
{
    s_kmSinceService = 0u;
    (void)printf("    [Volvo] " VOLVO_MODEL ": bakim yapildi\n");
    return MM_OK;
}

/* ---- Tira ozel islemler ------------------------------------------------- */
static MM_Ret_t volvo_getMaxLoadKg(uint32_t *pKg)
{
    *pKg = VOLVO_MAX_LOAD_KG;
    return MM_OK;
}

static MM_Ret_t volvo_getLoadKg(uint32_t *pKg)
{
    *pKg = s_loadKg;
    return MM_OK;
}

static MM_Ret_t volvo_loadCargo(uint32_t kg)
{
    MM_Ret_t ret = MM_OK;

    if ((s_loadKg + kg) > VOLVO_MAX_LOAD_KG)
    {
        (void)printf("    [Volvo] " VOLVO_MODEL ": +%lu kg yuklenemez (mevcut %lu, kapasite %lu kg)\n",
                     (unsigned long)kg, (unsigned long)s_loadKg, (unsigned long)VOLVO_MAX_LOAD_KG);
        ret = MM_ERR_OVER_CAPACITY;
    }
    else
    {
        s_loadKg += kg;
        (void)printf("    [Volvo] " VOLVO_MODEL ": %lu kg yuklendi (toplam %lu kg)\n",
                     (unsigned long)kg, (unsigned long)s_loadKg);
    }
    return ret;
}

static MM_Ret_t volvo_unloadCargo(void)
{
    s_loadKg = 0u;
    (void)printf("    [Volvo] " VOLVO_MODEL ": yuk bosaltildi\n");
    return MM_OK;
}

/* ---- Arayuz tablosu: const, tanimlandigi satirda dolu ------------------- */
const TruckIfc_t Volvo_TruckIfc =
{
    /* ortak */
    .init      = volvo_init,
    .getInfo   = volvo_getInfo,
    .drive     = volvo_drive,
    .refuel    = volvo_refuel,
    .doService = volvo_doService,
    /* tipe ozel */
    .getMaxLoadKg = volvo_getMaxLoadKg,
    .getLoadKg    = volvo_getLoadKg,
    .loadCargo    = volvo_loadCargo,
    .unloadCargo  = volvo_unloadCargo,
};
