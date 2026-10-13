/**
 * @file    VehicleIfc.h
 * @brief   [fleetConfig] Vendor'larin (arac markalarinin) implement ettigi arayuzler.
 *
 *          Her arac ORTAK arayuzu implement eder. Ek olarak tipine gore
 *          SADECE BIR tipe ozel arayuzu implement eder:
 *
 *              Otomobil : VehicleCommonIfc_t + CarIfc_t
 *              Tir      : VehicleCommonIfc_t + TruckIfc_t
 *
 *          'instance': Bir marka filoda birden fazla araca sahip olabilir
 *          (ornegin iki Toyota). Vendor hangi aracin kastedildigini bu
 *          numaradan anlar. Numarayi entegrator konfigurasyonda verir.
 *
 *          ModuleMgr_Init, konfigurasyondaki her arac icin tum fonksiyon
 *          pointer'larinin dolu (NULL degil) oldugunu dogrular. Bu yuzden
 *          ModuleMgr cagri aninda tekrar NULL kontrolu yapmaz.
 */
#ifndef VEHICLE_IFC_H
#define VEHICLE_IFC_H

#include "ModuleMgr_Types.h"

/* ---- Tum araclar ------------------------------------------------------- */
typedef struct
{
    MM_Ret_t (*getInfo)  (uint8_t instance, VehicleInfo_t *pInfo);
    MM_Ret_t (*drive)    (uint8_t instance, uint32_t km);   /* km ekler, yakit harcar */
    MM_Ret_t (*refuel)   (uint8_t instance);                /* depoyu doldurur        */
    MM_Ret_t (*doService)(uint8_t instance);                /* bakim sayacini sifirlar */
} VehicleCommonIfc_t;

/* ---- Sadece otomobiller ------------------------------------------------ */
typedef struct
{
    MM_Ret_t (*getSeatCount) (uint8_t instance, uint8_t *pSeats);
    MM_Ret_t (*setPassengers)(uint8_t instance, uint8_t count);   /* 0 = yolcular indi */
} CarIfc_t;

/* ---- Sadece tirlar ----------------------------------------------------- */
typedef struct
{
    MM_Ret_t (*getMaxLoadKg)(uint8_t instance, uint32_t *pKg);
    MM_Ret_t (*getLoadKg)   (uint8_t instance, uint32_t *pKg);
    MM_Ret_t (*loadCargo)   (uint8_t instance, uint32_t kg);    /* mevcut yuke ekler */
    MM_Ret_t (*unloadCargo) (uint8_t instance);                 /* tum yuku bosaltir */
} TruckIfc_t;

#endif /* VEHICLE_IFC_H */
