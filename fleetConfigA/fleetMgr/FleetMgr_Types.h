/**
 * @file    FleetMgr_Types.h
 * @brief   [fleetConfigA] Filo yonetimi ortak tipleri.
 */
#ifndef FLEET_MGR_TYPES_H
#define FLEET_MGR_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef uint16_t VehicleId_t;

#define VEHICLE_ID_INVALID   ((VehicleId_t)0xFFFFu)

typedef enum
{
    VEHICLE_TYPE_CAR = 0,
    VEHICLE_TYPE_TRUCK,
    VEHICLE_TYPE_COUNT
} VehicleType_t;

typedef enum
{
    FUEL_PETROL = 0,
    FUEL_DIESEL,
    FUEL_HYBRID,
    FUEL_ELECTRIC
} FuelType_t;

typedef enum
{
    VEHICLE_STATUS_AVAILABLE = 0,   /* bos                  */
    VEHICLE_STATUS_IN_USE           /* yolcu veya yuk var   */
} VehicleStatus_t;

typedef enum
{
    FM_OK = 0,
    FM_ERR_PARAM,           /* Gecersiz parametre / NULL pointer             */
    FM_ERR_NOT_INIT,        /* FleetMgr init edilmemis                       */
    FM_ERR_CONFIG,          /* Konfigurasyon tablosu hatali                  */
    FM_ERR_NOT_FOUND,       /* ID filoda yok                                 */
    FM_ERR_WRONG_TYPE,      /* Islem bu arac tipi icin gecerli degil         */
    FM_ERR_OVER_CAPACITY,   /* Koltuk veya yuk kapasitesi asildi (arac)      */
    FM_ERR_LOW_FUEL         /* Yolculuk icin yakit yetersiz (arac)           */
} FM_Ret_t;

typedef struct
{
    const char      *pBrand;
    const char      *pModel;
    uint16_t         modelYear;
    FuelType_t       fuelType;
    uint32_t         odometerKm;
    uint8_t          fuelLevelPct;
    VehicleStatus_t  status;
} VehicleInfo_t;

#endif /* FLEET_MGR_TYPES_H */
