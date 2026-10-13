/**
 * @file    FleetMgr.h
 * @brief   [fleetConfigB] Filo yoneticisi - B: HER TIP ICIN AYRI TABLO.
 *
 *          Otomobiller ve tirlar ayri tablolarda:
 *              otomobil satiri: { id, &CarIfc_t   }
 *              tir satiri     : { id, &TruckIfc_t }
 *          Tip alani YOK: aracin tipi, hangi tabloda oldugundan anlasilir.
 *          Otomobil tablosuna tir arayuzu yazmak derleyici tarafindan yakalanir;
 *          tip ile arayuz uyumsuzlugu yazilamaz.
 *
 *          Kural: FleetMgr_Init tek yerden, bir kez cagrilir.
 */
#ifndef FLEET_MGR_H
#define FLEET_MGR_H

#include "FleetMgr_Types.h"
#include "VehicleIfc.h"

typedef struct
{
    VehicleId_t      id;
    const CarIfc_t  *pIfc;
} FleetMgr_CarCfg_t;

typedef struct
{
    VehicleId_t        id;
    const TruckIfc_t  *pIfc;
} FleetMgr_TruckCfg_t;

typedef struct
{
    const FleetMgr_CarCfg_t    *pCars;
    uint8_t                     carCount;
    const FleetMgr_TruckCfg_t  *pTrucks;
    uint8_t                     truckCount;
} FleetMgr_Config_t;

/* Konfigurasyonu dogrular, her aracin init'ini cagirir ve saklar.
 * Bir arac init'i hata donerse o hatayi doner; FleetMgr init edilmemis kalir. */
FM_Ret_t    FleetMgr_Init(const FleetMgr_Config_t *pCfg);
FM_Ret_t    FleetMgr_DeInit(void);

/* Filoyu dolasma: once otomobiller, sonra tirlar. */
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
