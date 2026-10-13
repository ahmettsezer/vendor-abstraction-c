/**
 * @file    Man.c
 * @brief   [fleetConfig] MAN TGX tir implementasyonu.
 *
 *          Yakit tuketimi (100 km'de depo yuzdesi):
 *              bos tuketim + her 5 ton yuk icin %1
 *          Not: printf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "Man.h"

/* Aracin degismeyen ozellikleri + teslim alindigi andaki durumu. */
#define MAN_MODEL                 "TGX 18.510"
#define MAN_MODEL_YEAR            (2020u)
#define MAN_FUEL_TYPE             (FUEL_DIESEL)
#define MAN_MAX_LOAD_KG           (20000u)
#define MAN_BASE_PCT_PER_100KM    (9u)
#define MAN_KG_PER_EXTRA_PCT      (5000u)   /* 5 ton yuk = 100 km'de +%1 */
#define MAN_INIT_ODOMETER_KM      (412000u)
#define MAN_INIT_SERVICE_KM       (29700u)
#define MAN_INIT_FUEL_PCT         (80u)

/* Aracin degisen durumu. */
static uint32_t s_odometerKm     = 0u;
static uint32_t s_kmSinceService = 0u;
static uint8_t  s_fuelPct        = 0u;
static uint32_t s_loadKg         = 0u;

/* ---- Init: entegrator degil ModuleMgr_Init cagirir ---------------------- */
static MM_Ret_t man_init(void)
{
    s_odometerKm     = MAN_INIT_ODOMETER_KM;
    s_kmSinceService = MAN_INIT_SERVICE_KM;
    s_fuelPct        = MAN_INIT_FUEL_PCT;
    s_loadKg         = 0u;
    (void)printf("    [MAN] " MAN_MODEL ": init (%lu km, depo %%%u)\n",
                 (unsigned long)s_odometerKm, (unsigned)s_fuelPct);
    return MM_OK;
}

/* ---- Ortak islemler ----------------------------------------------------- */
static MM_Ret_t man_getInfo(VehicleInfo_t *pInfo)
{
    pInfo->pBrand         = "MAN";
    pInfo->pModel         = MAN_MODEL;
    pInfo->modelYear      = MAN_MODEL_YEAR;
    pInfo->fuelType       = MAN_FUEL_TYPE;
    pInfo->odometerKm     = s_odometerKm;
    pInfo->kmSinceService = s_kmSinceService;
    pInfo->fuelLevelPct   = s_fuelPct;
    pInfo->status         = (s_loadKg > 0u) ? VEHICLE_STATUS_IN_USE : VEHICLE_STATUS_AVAILABLE;
    return MM_OK;
}

static MM_Ret_t man_drive(uint32_t km)
{
    MM_Ret_t ret = MM_OK;
    uint32_t pctPer100Km = MAN_BASE_PCT_PER_100KM + (s_loadKg / MAN_KG_PER_EXTRA_PCT);
    uint32_t needPct     = (km * pctPer100Km) / 100u;

    if (needPct > (uint32_t)s_fuelPct)
    {
        (void)printf("    [MAN] " MAN_MODEL ": %lu km icin yakit yetersiz (gerekli %%%lu, depoda %%%u)\n",
                     (unsigned long)km, (unsigned long)needPct, (unsigned)s_fuelPct);
        ret = MM_ERR_LOW_FUEL;
    }
    else
    {
        s_odometerKm     += km;
        s_kmSinceService += km;
        s_fuelPct         = (uint8_t)((uint32_t)s_fuelPct - needPct);
        (void)printf("    [MAN] " MAN_MODEL ": %lu km gidildi (%lu kg yukle), depo %%%u\n",
                     (unsigned long)km, (unsigned long)s_loadKg, (unsigned)s_fuelPct);
    }
    return ret;
}

static MM_Ret_t man_refuel(void)
{
    s_fuelPct = 100u;
    (void)printf("    [MAN] " MAN_MODEL ": depo dolduruldu\n");
    return MM_OK;
}

static MM_Ret_t man_doService(void)
{
    s_kmSinceService = 0u;
    (void)printf("    [MAN] " MAN_MODEL ": bakim yapildi\n");
    return MM_OK;
}

/* ---- Tira ozel islemler ------------------------------------------------- */
static MM_Ret_t man_getMaxLoadKg(uint32_t *pKg)
{
    *pKg = MAN_MAX_LOAD_KG;
    return MM_OK;
}

static MM_Ret_t man_getLoadKg(uint32_t *pKg)
{
    *pKg = s_loadKg;
    return MM_OK;
}

static MM_Ret_t man_loadCargo(uint32_t kg)
{
    MM_Ret_t ret = MM_OK;

    if ((s_loadKg + kg) > MAN_MAX_LOAD_KG)
    {
        (void)printf("    [MAN] " MAN_MODEL ": +%lu kg yuklenemez (mevcut %lu, kapasite %lu kg)\n",
                     (unsigned long)kg, (unsigned long)s_loadKg, (unsigned long)MAN_MAX_LOAD_KG);
        ret = MM_ERR_OVER_CAPACITY;
    }
    else
    {
        s_loadKg += kg;
        (void)printf("    [MAN] " MAN_MODEL ": %lu kg yuklendi (toplam %lu kg)\n",
                     (unsigned long)kg, (unsigned long)s_loadKg);
    }
    return ret;
}

static MM_Ret_t man_unloadCargo(void)
{
    s_loadKg = 0u;
    (void)printf("    [MAN] " MAN_MODEL ": yuk bosaltildi\n");
    return MM_OK;
}

/* ---- Arayuz tablosu: const, tanimlandigi satirda dolu ------------------- */
const TruckIfc_t Man_TruckIfc =
{
    /* ortak */
    .init      = man_init,
    .getInfo   = man_getInfo,
    .drive     = man_drive,
    .refuel    = man_refuel,
    .doService = man_doService,
    /* tipe ozel */
    .getMaxLoadKg = man_getMaxLoadKg,
    .getLoadKg    = man_getLoadKg,
    .loadCargo    = man_loadCargo,
    .unloadCargo  = man_unloadCargo,
};
