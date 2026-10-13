/**
 * @file    VehicleIfc.h
 * @brief   [fleetIndexConfig] Araclarin implement ettigi arayuzler.
 *
 *          Her tipin tek bir arayuzu var; ortak islemler (init, getInfo,
 *          drive) ayni isim ve imzayla, ardindan tipe ozel islem gelir.
 */
#ifndef VEHICLE_IFC_H
#define VEHICLE_IFC_H

#include "FleetMgr_Types.h"

typedef struct
{
    /* ortak */
    FM_Ret_t (*init)         (void);
    FM_Ret_t (*getInfo)      (VehicleInfo_t *pInfo);
    FM_Ret_t (*drive)        (uint32_t km);
    /* otomobile ozel */
    FM_Ret_t (*setPassengers)(uint8_t count);
} CarIfc_t;

typedef struct
{
    /* ortak */
    FM_Ret_t (*init)         (void);
    FM_Ret_t (*getInfo)      (VehicleInfo_t *pInfo);
    FM_Ret_t (*drive)        (uint32_t km);
    /* tira ozel */
    FM_Ret_t (*loadCargo)    (uint32_t kg);
} TruckIfc_t;

#endif /* VEHICLE_IFC_H */
