/**
 * @file    FleetMgr.h
 * @brief   [fleetConfig] Filo yoneticisi (constConfig mimarisi).
 *
 *          FleetMgr markalari TANIMAZ. Entegratorun verdigi const tablodan
 *          her aracin ID'sini, TIPINI ve arayuzlerini okur; islemleri tipe
 *          gore dogru arayuze yonlendirir. Tipe uymayan bir islem
 *          (otomobile yuk yuklemek gibi) araca hic ulasmadan
 *          FM_ERR_WRONG_TYPE ile reddedilir.
 *
 *          Konfigurasyon FleetMgr_Init(&cfg) ile verilir; 'extern' yok,
 *          FleetMgr tek basina derlenip link edilebilir.
 *          Kural: FleetMgr_Init tek yerden, bir kez cagrilir.
 */
#ifndef FLEET_MGR_H
#define FLEET_MGR_H

#include "FleetMgr_Types.h"
#include "VehicleIfc.h"

/* ---- Konfigurasyon tipleri (FleetMgr tanimlar, entegrator doldurur) --- */

/* Filodaki bir arac. 'pVehicle', 'type'a gore su arayuzu gosterir:
 *   VEHICLE_TYPE_CAR   -> const CarIfc_t
 *   VEHICLE_TYPE_TRUCK -> const TruckIfc_t
 * FleetMgr 'type'a bakip pVehicle'i ilgili arayuz tipine cast ederek kullanir.
 *
 * DIKKAT: 'void*' tip bilgisini tasimaz. type ile pVehicle'in uyumunu
 * ENTEGRATOR garanti etmelidir; FleetMgr bunu dogrulayamaz. Uyumsuzluk
 * (ornegin CAR tipi + tir arayuzu) tanimsiz davranistir.                  */
typedef struct
{
    VehicleId_t     id;
    VehicleType_t   type;
    const void     *pVehicle;
} FleetMgr_VehicleCfg_t;

typedef struct
{
    const FleetMgr_VehicleCfg_t *pVehicles;
    uint8_t                       vehicleCount;
    uint32_t                      serviceIntervalKm[VEHICLE_TYPE_COUNT];  /* tipe gore bakim araligi */
} FleetMgr_Config_t;

/* Tip bazinda filo ozeti. */
typedef struct
{
    uint8_t   vehicleCount[VEHICLE_TYPE_COUNT];
    uint32_t  totalSeats;          /* tum otomobillerin koltuk toplami */
    uint32_t  totalMaxLoadKg;      /* tum tirlarin yuk kapasitesi      */
    uint32_t  totalOdometerKm;     /* tum araclarin km toplami         */
} FleetMgr_Summary_t;

/* ---- Yasam dongusu ------------------------------------------------------ */
/* Konfigurasyonu dogrular, her aracin init'ini cagirir ve saklar.
 * Bir arac init'i hata donerse o hatayi doner; FleetMgr init edilmemis kalir. */
FM_Ret_t FleetMgr_Init(const FleetMgr_Config_t *pCfg);
FM_Ret_t FleetMgr_DeInit(void);

/* ---- Filoyu dolasma ----------------------------------------------------- */
uint8_t     FleetMgr_GetVehicleCount(void);
VehicleId_t FleetMgr_GetVehicleIdAt(uint8_t index);
FM_Ret_t    FleetMgr_GetVehicleType(VehicleId_t id, VehicleType_t *pType);

/* ---- Tum araclar (ortak arayuz) ---------------------------------------- */
FM_Ret_t FleetMgr_GetInfo (VehicleId_t id, VehicleInfo_t *pInfo);
FM_Ret_t FleetMgr_Drive   (VehicleId_t id, uint32_t km);
FM_Ret_t FleetMgr_Refuel  (VehicleId_t id);
FM_Ret_t FleetMgr_Service (VehicleId_t id);

/* ---- Sadece otomobil (tir icin FM_ERR_WRONG_TYPE) ----------------------- */
FM_Ret_t FleetMgr_Car_GetSeatCount (VehicleId_t id, uint8_t *pSeats);
FM_Ret_t FleetMgr_Car_SetPassengers(VehicleId_t id, uint8_t count);

/* ---- Sadece tir (otomobil icin FM_ERR_WRONG_TYPE) ----------------------- */
FM_Ret_t FleetMgr_Truck_GetMaxLoad (VehicleId_t id, uint32_t *pKg);
FM_Ret_t FleetMgr_Truck_GetLoad    (VehicleId_t id, uint32_t *pKg);
FM_Ret_t FleetMgr_Truck_LoadCargo  (VehicleId_t id, uint32_t kg);
FM_Ret_t FleetMgr_Truck_UnloadCargo(VehicleId_t id);

/* ---- Filo seviyesi: tipe gore yonetim ----------------------------------- */
FM_Ret_t FleetMgr_GetFleetSummary(FleetMgr_Summary_t *pSummary);

/* Bos otomobiller icinde yolcu sayisina YETEN EN KUCUK otomobili bulur. */
FM_Ret_t FleetMgr_FindCarForPassengers(uint8_t passengers, VehicleId_t *pId);

/* Bos tirlar icinde yuke YETEN EN KUCUK kapasiteli tiri bulur. */
FM_Ret_t FleetMgr_FindTruckForLoad(uint32_t kg, VehicleId_t *pId);

/* Son bakimdan beri km, aracin TIPINE ait bakim araligini astiysa true. */
FM_Ret_t FleetMgr_IsServiceDue(VehicleId_t id, bool *pDue);

/* Tum araclarin deposunu doldurur; hata alan arac sayisini doner (NULL olabilir). */
FM_Ret_t FleetMgr_RefuelAll(uint8_t *pFailCount);

#endif /* FLEET_MGR_H */
