/**
 * @file    FleetMgr.h
 * @brief   [fleetIndexConfig] Filo yoneticisi: fleetConfig + payload tasarimi.
 *
 *          fleetConfig'ten : tipe ozel arayuzler, init'i FleetMgr cagirir,
 *                            konfigurasyon Init parametresi ('extern' yok).
 *          payload'dan     : 'void*' yerine TIPE GORE AYRI, TIPLI const
 *                            tablolar ve INDEX; her zaman gecerli bir arayuz
 *                            donduren guvenli varsayilan (cast yok, NULL yok).
 *
 *          Kural: FleetMgr_Init tek yerden, bir kez cagrilir.
 */
#ifndef FLEET_MGR_H
#define FLEET_MGR_H

#include "FleetMgr_Types.h"
#include "VehicleIfc.h"

/* Filodaki bir arac: 'index', type'a ait arayuz tablosundaki satirdir.
 *   VEHICLE_TYPE_CAR   -> ppCars[index]
 *   VEHICLE_TYPE_TRUCK -> ppTrucks[index]                                  */
typedef struct
{
    VehicleId_t    id;
    VehicleType_t  type;
    uint8_t        index;
} FleetMgr_VehicleCfg_t;

typedef struct
{
    const FleetMgr_VehicleCfg_t  *pVehicles;
    uint8_t                       vehicleCount;
    const CarIfc_t   * const     *ppCars;      /* otomobil arayuzleri tablosu */
    uint8_t                       carCount;
    const TruckIfc_t * const     *ppTrucks;    /* tir arayuzleri tablosu      */
    uint8_t                       truckCount;
} FleetMgr_Config_t;

/* Konfigurasyonu dogrular, her aracin init'ini cagirir ve saklar.
 * Bir arac init'i hata donerse o hatayi doner; FleetMgr init edilmemis kalir. */
FM_Ret_t    FleetMgr_Init(const FleetMgr_Config_t *pCfg);
FM_Ret_t    FleetMgr_DeInit(void);

uint8_t     FleetMgr_GetVehicleCount(void);
VehicleId_t FleetMgr_GetVehicleIdAt(uint8_t index);
FM_Ret_t    FleetMgr_GetType(VehicleId_t id, VehicleType_t *pType);

/* Ortak islemler */
FM_Ret_t    FleetMgr_GetInfo(VehicleId_t id, VehicleInfo_t *pInfo);
FM_Ret_t    FleetMgr_Drive  (VehicleId_t id, uint32_t km);

/* Tipe ozel islemler (tip uymazsa FM_ERR_WRONG_TYPE) */
FM_Ret_t    FleetMgr_Car_SetPassengers(VehicleId_t id, uint8_t count);
FM_Ret_t    FleetMgr_Truck_LoadCargo  (VehicleId_t id, uint32_t kg);

#endif /* FLEET_MGR_H */
