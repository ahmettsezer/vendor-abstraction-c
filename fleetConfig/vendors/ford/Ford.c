/**
 * @file    Ford.c
 * @brief   [fleetConfig] Ford Tourneo Custom otomobil implementasyonu.
 *
 *          Yakit tuketimi: 100 km'de depo yuzdesi olarak sabit.
 *          Not: printf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "Ford.h"

/* Aracin degismeyen ozellikleri + teslim alindigi andaki durumu. */
#define FORD_MODEL              "Tourneo Custom"
#define FORD_MODEL_YEAR         (2022u)
#define FORD_FUEL_TYPE          (FUEL_DIESEL)
#define FORD_SEATS              (8u)
#define FORD_PCT_PER_100KM      (8u)
#define FORD_INIT_ODOMETER_KM   (31500u)
#define FORD_INIT_SERVICE_KM    (9000u)
#define FORD_INIT_FUEL_PCT      (55u)

/* Aracin degisen durumu. */
static uint32_t s_odometerKm     = 0u;
static uint32_t s_kmSinceService = 0u;
static uint8_t  s_fuelPct        = 0u;
static uint8_t  s_passengers     = 0u;

/* ---- Ortak islemler ----------------------------------------------------- */
static MM_Ret_t ford_getInfo(VehicleInfo_t *pInfo)
{
    pInfo->pBrand         = "Ford";
    pInfo->pModel         = FORD_MODEL;
    pInfo->modelYear      = FORD_MODEL_YEAR;
    pInfo->fuelType       = FORD_FUEL_TYPE;
    pInfo->odometerKm     = s_odometerKm;
    pInfo->kmSinceService = s_kmSinceService;
    pInfo->fuelLevelPct   = s_fuelPct;
    pInfo->status         = (s_passengers > 0u) ? VEHICLE_STATUS_IN_USE : VEHICLE_STATUS_AVAILABLE;
    return MM_OK;
}

static MM_Ret_t ford_drive(uint32_t km)
{
    MM_Ret_t ret = MM_OK;
    uint32_t needPct = (km * FORD_PCT_PER_100KM) / 100u;

    if (needPct > (uint32_t)s_fuelPct)
    {
        (void)printf("    [Ford] " FORD_MODEL ": %lu km icin yakit yetersiz (gerekli %%%lu, depoda %%%u)\n",
                     (unsigned long)km, (unsigned long)needPct, (unsigned)s_fuelPct);
        ret = MM_ERR_LOW_FUEL;
    }
    else
    {
        s_odometerKm     += km;
        s_kmSinceService += km;
        s_fuelPct         = (uint8_t)((uint32_t)s_fuelPct - needPct);
        (void)printf("    [Ford] " FORD_MODEL ": %lu km gidildi, depo %%%u\n",
                     (unsigned long)km, (unsigned)s_fuelPct);
    }
    return ret;
}

static MM_Ret_t ford_refuel(void)
{
    s_fuelPct = 100u;
    (void)printf("    [Ford] " FORD_MODEL ": depo dolduruldu\n");
    return MM_OK;
}

static MM_Ret_t ford_doService(void)
{
    s_kmSinceService = 0u;
    (void)printf("    [Ford] " FORD_MODEL ": bakim yapildi\n");
    return MM_OK;
}

/* ---- Otomobile ozel islemler -------------------------------------------- */
static MM_Ret_t ford_getSeatCount(uint8_t *pSeats)
{
    *pSeats = FORD_SEATS;
    return MM_OK;
}

static MM_Ret_t ford_setPassengers(uint8_t count)
{
    MM_Ret_t ret = MM_OK;

    if (count > FORD_SEATS)
    {
        (void)printf("    [Ford] " FORD_MODEL ": %u yolcu sigmaz (koltuk %u)\n",
                     (unsigned)count, (unsigned)FORD_SEATS);
        ret = MM_ERR_OVER_CAPACITY;
    }
    else
    {
        s_passengers = count;
        if (count > 0u)
        {
            (void)printf("    [Ford] " FORD_MODEL ": %u yolcu bindi\n", (unsigned)count);
        }
        else
        {
            (void)printf("    [Ford] " FORD_MODEL ": yolcular indi\n");
        }
    }
    return ret;
}

/* ---- Arayuz tablosu: const, tanimlandigi satirda dolu ------------------- */
const CarIfc_t Ford_CarIfc =
{
    /* ortak */
    .getInfo   = ford_getInfo,
    .drive     = ford_drive,
    .refuel    = ford_refuel,
    .doService = ford_doService,
    /* tipe ozel */
    .getSeatCount  = ford_getSeatCount,
    .setPassengers = ford_setPassengers,
};

MM_Ret_t Ford_Init(void)
{
    s_odometerKm     = FORD_INIT_ODOMETER_KM;
    s_kmSinceService = FORD_INIT_SERVICE_KM;
    s_fuelPct        = FORD_INIT_FUEL_PCT;
    s_passengers     = 0u;
    return MM_OK;
}
