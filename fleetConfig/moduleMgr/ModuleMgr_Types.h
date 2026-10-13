/**
 * @file    ModuleMgr_Types.h
 * @brief   [fleetConfig] Filo yonetimi ortak tipleri.
 *
 *          ModuleMgr, entegrator ve tum vendor'lar (arac markalari) bu
 *          dosyayi bilir. Hicbir markaya ozel bilgi icermez.
 */
#ifndef MODULE_MGR_TYPES_H
#define MODULE_MGR_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* Filodaki aracin kimligi (plaka/filo numarasi gibi). Entegrator verir. */
typedef uint16_t VehicleId_t;

#define VEHICLE_ID_INVALID   ((VehicleId_t)0xFFFFu)

/* Arac tipi. ModuleMgr hangi tipe ozel arayuzun kullanilacagina buna gore
 * karar verir. VEHICLE_TYPE_COUNT dizi boyutu olarak kullanilir. */
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
    VEHICLE_STATUS_AVAILABLE = 0,   /* bos, goreve verilebilir          */
    VEHICLE_STATUS_IN_USE           /* yolcu veya yuk tasiyor           */
} VehicleStatus_t;

typedef enum
{
    MM_OK = 0,
    MM_ERR_PARAM,           /* Gecersiz parametre / NULL pointer             */
    MM_ERR_NOT_INIT,        /* ModuleMgr init edilmemis                      */
    MM_ERR_CONFIG,          /* Konfigurasyon tablosu hatali                  */
    MM_ERR_NOT_FOUND,       /* ID filoda yok / uygun arac bulunamadi         */
    MM_ERR_WRONG_TYPE,      /* Islem bu arac tipi icin gecerli degil         */
    MM_ERR_OVER_CAPACITY,   /* Koltuk veya yuk kapasitesi asildi (vendor)    */
    MM_ERR_LOW_FUEL         /* Yolculuk icin yakit yetersiz (vendor)         */
} MM_Ret_t;

/* Tum arac tiplerinde ortak bilgiler. */
typedef struct
{
    const char      *pBrand;
    const char      *pModel;
    uint16_t         modelYear;
    FuelType_t       fuelType;
    uint32_t         odometerKm;        /* toplam km                        */
    uint32_t         kmSinceService;    /* son bakimdan beri km             */
    uint8_t          fuelLevelPct;      /* depo doluluk yuzdesi (0..100)    */
    VehicleStatus_t  status;
} VehicleInfo_t;

#endif /* MODULE_MGR_TYPES_H */
