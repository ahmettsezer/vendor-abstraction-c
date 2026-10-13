/**
 * @file    FleetMgr.h
 * @brief   [fleetConfigA] Filo yoneticisi - A: TIPLI ALANLAR.
 *
 *          Filo tek bir tablo; her satir bir arac:
 *              { id, type, pCar, pTruck }
 *          Araca uymayan tipe ozel alan NULL kalir. Alanlar tipli oldugu icin
 *          otomobil alanina tir arayuzu yazmak derleyici tarafindan yakalanir;
 *          tip ile dolu alanin uyumunu da FleetMgr_Init dogrular.
 *
 *          Kural: FleetMgr_Init tek yerden, bir kez cagrilir.
 */
#ifndef FLEET_MGR_H
#define FLEET_MGR_H

#include "FleetMgr_Types.h"
#include "VehicleIfc.h"

/* VEHICLE_TYPE_CAR   -> pCar dolu,   pTruck NULL
 * VEHICLE_TYPE_TRUCK -> pTruck dolu, pCar NULL   */
typedef struct
{
    VehicleId_t        id;
    VehicleType_t      type;
    const CarIfc_t    *pCar;
    const TruckIfc_t  *pTruck;
} FleetMgr_VehicleCfg_t;

typedef struct
{
    const FleetMgr_VehicleCfg_t *pVehicles;
    uint8_t                      vehicleCount;
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
