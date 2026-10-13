/**
 * @file    ModuleMgr.h
 * @brief   [fleetConfig] Filo yoneticisi (constConfig mimarisi).
 *
 *          ModuleMgr markalari TANIMAZ. Entegratorun verdigi const tablodan
 *          her aracin ID'sini, TIPINI ve arayuzlerini okur; islemleri tipe
 *          gore dogru arayuze yonlendirir. Tipe uymayan bir islem
 *          (otomobile yuk yuklemek gibi) vendor'a hic ulasmadan
 *          MM_ERR_WRONG_TYPE ile reddedilir.
 *
 *          Konfigurasyon ModuleMgr_Init(&cfg) ile verilir; 'extern' yok,
 *          ModuleMgr tek basina derlenip link edilebilir.
 *          Kural: ModuleMgr_Init tek yerden, bir kez cagrilir.
 */
#ifndef MODULE_MGR_H
#define MODULE_MGR_H

#include "ModuleMgr_Types.h"
#include "VehicleIfc.h"

/* ---- Konfigurasyon tipleri (ModuleMgr tanimlar, entegrator doldurur) --- */

/* Filodaki bir arac. 'type' hangi arayuzun kullanilacagini belirler;
 * tipe uymayan arayuz pointer'i NULL olmali:
 *   VEHICLE_TYPE_CAR   -> pCar dolu,   pTruck NULL
 *   VEHICLE_TYPE_TRUCK -> pTruck dolu, pCar NULL
 * Ortak islemler (bilgi, sur, yakit, bakim) her iki arayuzde de vardir.  */
typedef struct
{
    VehicleId_t        id;
    VehicleType_t      type;
    const CarIfc_t    *pCar;
    const TruckIfc_t  *pTruck;
} ModuleMgr_VehicleCfg_t;

typedef struct
{
    const ModuleMgr_VehicleCfg_t *pVehicles;
    uint8_t                       vehicleCount;
    uint32_t                      serviceIntervalKm[VEHICLE_TYPE_COUNT];  /* tipe gore bakim araligi */
} ModuleMgr_Config_t;

/* Tip bazinda filo ozeti. */
typedef struct
{
    uint8_t   vehicleCount[VEHICLE_TYPE_COUNT];
    uint32_t  totalSeats;          /* tum otomobillerin koltuk toplami */
    uint32_t  totalMaxLoadKg;      /* tum tirlarin yuk kapasitesi      */
    uint32_t  totalOdometerKm;     /* tum araclarin km toplami         */
} ModuleMgr_FleetSummary_t;

/* ---- Yasam dongusu ------------------------------------------------------ */
/* Konfigurasyonu dogrular, her aracin init'ini cagirir ve saklar.
 * Bir arac init'i hata donerse o hatayi doner; ModuleMgr init edilmemis kalir. */
MM_Ret_t ModuleMgr_Init(const ModuleMgr_Config_t *pCfg);
MM_Ret_t ModuleMgr_DeInit(void);

/* ---- Filoyu dolasma ----------------------------------------------------- */
uint8_t     ModuleMgr_GetVehicleCount(void);
VehicleId_t ModuleMgr_GetVehicleIdAt(uint8_t index);
MM_Ret_t    ModuleMgr_GetVehicleType(VehicleId_t id, VehicleType_t *pType);

/* ---- Tum araclar (ortak arayuz) ---------------------------------------- */
MM_Ret_t ModuleMgr_GetInfo (VehicleId_t id, VehicleInfo_t *pInfo);
MM_Ret_t ModuleMgr_Drive   (VehicleId_t id, uint32_t km);
MM_Ret_t ModuleMgr_Refuel  (VehicleId_t id);
MM_Ret_t ModuleMgr_Service (VehicleId_t id);

/* ---- Sadece otomobil (tir icin MM_ERR_WRONG_TYPE) ----------------------- */
MM_Ret_t ModuleMgr_Car_GetSeatCount (VehicleId_t id, uint8_t *pSeats);
MM_Ret_t ModuleMgr_Car_SetPassengers(VehicleId_t id, uint8_t count);

/* ---- Sadece tir (otomobil icin MM_ERR_WRONG_TYPE) ----------------------- */
MM_Ret_t ModuleMgr_Truck_GetMaxLoad (VehicleId_t id, uint32_t *pKg);
MM_Ret_t ModuleMgr_Truck_GetLoad    (VehicleId_t id, uint32_t *pKg);
MM_Ret_t ModuleMgr_Truck_LoadCargo  (VehicleId_t id, uint32_t kg);
MM_Ret_t ModuleMgr_Truck_UnloadCargo(VehicleId_t id);

/* ---- Filo seviyesi: tipe gore yonetim ----------------------------------- */
MM_Ret_t ModuleMgr_GetFleetSummary(ModuleMgr_FleetSummary_t *pSummary);

/* Bos otomobiller icinde yolcu sayisina YETEN EN KUCUK otomobili bulur. */
MM_Ret_t ModuleMgr_FindCarForPassengers(uint8_t passengers, VehicleId_t *pId);

/* Bos tirlar icinde yuke YETEN EN KUCUK kapasiteli tiri bulur. */
MM_Ret_t ModuleMgr_FindTruckForLoad(uint32_t kg, VehicleId_t *pId);

/* Son bakimdan beri km, aracin TIPINE ait bakim araligini astiysa true. */
MM_Ret_t ModuleMgr_IsServiceDue(VehicleId_t id, bool *pDue);

/* Tum araclarin deposunu doldurur; hata alan arac sayisini doner (NULL olabilir). */
MM_Ret_t ModuleMgr_RefuelAll(uint8_t *pFailCount);

#endif /* MODULE_MGR_H */
