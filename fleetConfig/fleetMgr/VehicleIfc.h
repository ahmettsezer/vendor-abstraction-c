/**
 * @file    VehicleIfc.h
 * @brief   [fleetConfig] Araclarin (vehicles/) implement ettigi arayuzler.
 *
 *          Her arac tipinin TEK bir arayuzu vardir. Tum araclarda ortak olan
 *          islemler (init, bilgi, sur, yakit, bakim) her arayuzde ayni isim ve
 *          imzayla, dogrudan alan olarak bulunur; ardindan tipe ozel
 *          islemler gelir.
 *
 *          'init' entegrator tarafindan degil, FleetMgr_Init tarafindan
 *          konfigurasyondaki her arac icin cagrilir.
 *
 *          Bir arayuz tablosu = filodaki BIR arac. Ayni tablo iki farkli ID'ye
 *          verilemez; FleetMgr_Init bunu reddeder.
 *
 *          FleetMgr_Init tum fonksiyon pointer'larinin dolu (NULL degil)
 *          oldugunu dogrular; bu yuzden cagri aninda tekrar kontrol edilmez.
 */
#ifndef VEHICLE_IFC_H
#define VEHICLE_IFC_H

#include "FleetMgr_Types.h"

/* ---- Otomobil arayuzu --------------------------------------------------- */
typedef struct
{
    /* ortak */
    FM_Ret_t (*init)         (void);             /* FleetMgr_Init cagirir  */
    FM_Ret_t (*getInfo)      (VehicleInfo_t *pInfo);
    FM_Ret_t (*drive)        (uint32_t km);      /* km ekler, yakit harcar  */
    FM_Ret_t (*refuel)       (void);             /* depoyu doldurur         */
    FM_Ret_t (*doService)    (void);             /* bakim sayacini sifirlar */
    /* otomobile ozel */
    FM_Ret_t (*getSeatCount) (uint8_t *pSeats);
    FM_Ret_t (*setPassengers)(uint8_t count);    /* 0 = yolcular indi       */
} CarIfc_t;

/* ---- Tir arayuzu -------------------------------------------------------- */
typedef struct
{
    /* ortak */
    FM_Ret_t (*init)         (void);             /* FleetMgr_Init cagirir  */
    FM_Ret_t (*getInfo)      (VehicleInfo_t *pInfo);
    FM_Ret_t (*drive)        (uint32_t km);      /* km ekler, yakit harcar  */
    FM_Ret_t (*refuel)       (void);             /* depoyu doldurur         */
    FM_Ret_t (*doService)    (void);             /* bakim sayacini sifirlar */
    /* tira ozel */
    FM_Ret_t (*getMaxLoadKg) (uint32_t *pKg);
    FM_Ret_t (*getLoadKg)    (uint32_t *pKg);
    FM_Ret_t (*loadCargo)    (uint32_t kg);      /* mevcut yuke ekler       */
    FM_Ret_t (*unloadCargo)  (void);             /* tum yuku bosaltir       */
} TruckIfc_t;

#endif /* VEHICLE_IFC_H */
