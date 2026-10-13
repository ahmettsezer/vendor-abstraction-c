/**
 * @file    FleetMgr.h
 * @brief   [fleetConfigIndexed] Filo yoneticisi - A (tipli alanlar) + ID = INDEX.
 *
 *          Filo tek bir tablo ve tablo ARAC ID'si ile index'lenir:
 *              tablo[id] = { type, pCar, pTruck }
 *          FleetMgr ID'yi tabloda ARAMAZ; tek bir sinir kontrolunden sonra
 *          dogrudan tablo[id]'ye erisir. tablo[0] = VEHICLE_ID_NONE ("arac yok").
 *
 *          Araca uymayan tipe ozel alan NULL kalir. Alanlar tipli oldugu icin
 *          otomobil alanina tir arayuzu yazmak derleyici tarafindan yakalanir.
 *
 *          Kural: FleetMgr_Init tek yerden, bir kez cagrilir.
 */
#ifndef FLEET_MGR_H
#define FLEET_MGR_H

#include "FleetMgr_Types.h"
#include "VehicleIfc.h"

/* Satirin numarasi aracin ID'sidir; satirda ayrica id alani yok.
 * VEHICLE_TYPE_NONE  -> pCar NULL,   pTruck NULL  (sadece tablo[0])
 * VEHICLE_TYPE_CAR   -> pCar dolu,   pTruck NULL
 * VEHICLE_TYPE_TRUCK -> pTruck dolu, pCar NULL   */
typedef struct
{
    VehicleType_t      type;
    const CarIfc_t    *pCar;
    const TruckIfc_t  *pTruck;
} FleetMgr_VehicleCfg_t;

typedef struct
{
    const FleetMgr_VehicleCfg_t *pVehicles;     /* index = arac ID'si          */
    uint8_t                      vehicleCount;  /* tablo boyutu (0. satir dahil) */
} FleetMgr_Config_t;

/* Konfigurasyonu dogrular, her aracin init'ini cagirir ve saklar.
 * Bir arac init'i hata donerse o hatayi doner; FleetMgr init edilmemis kalir. */
FM_Ret_t    FleetMgr_Init(const FleetMgr_Config_t *pCfg);
FM_Ret_t    FleetMgr_DeInit(void);

FM_Ret_t    FleetMgr_GetType(VehicleId_t id, VehicleType_t *pType);

/* Ortak islemler */
FM_Ret_t    FleetMgr_GetInfo(VehicleId_t id, VehicleInfo_t *pInfo);
FM_Ret_t    FleetMgr_Drive  (VehicleId_t id, uint32_t km);

/* Tipe ozel islemler (tip uymazsa FM_ERR_WRONG_TYPE) */
FM_Ret_t    FleetMgr_Car_SetPassengers(VehicleId_t id, uint8_t count);
FM_Ret_t    FleetMgr_Truck_LoadCargo  (VehicleId_t id, uint32_t kg);

#endif /* FLEET_MGR_H */
