/**
 * @file    FleetMgr_Types.h
 * @brief   [fleetConfigIndexed] Filo yonetimi ortak tipleri.
 *
 *          FleetMgr, entegrator ve tum araclar (vehicles/) bu
 *          dosyayi bilir. Hicbir markaya ozel bilgi icermez.
 */
#ifndef FLEET_MGR_TYPES_H
#define FLEET_MGR_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* Arac kimligi = filo tablosundaki SATIR NUMARASI. Entegrator ID'leri
 * 1'den baslayan ardisik bir enum olarak tanimlar; FleetMgr tabloda arama
 * yapmaz, ID ile dogrudan satira erisir. */
typedef uint8_t VehicleId_t;

/* 0 numarali satir: "arac yok". Defansif: ilk degeri verilmemis (sifir
 * kalmis) bir ID yanlislikla ilk gercek araci secmez. */
#define VEHICLE_ID_NONE      ((VehicleId_t)0u)

/* Arac tipi. FleetMgr hangi tipe ozel arayuzun kullanilacagina buna gore
 * karar verir. VEHICLE_TYPE_COUNT dizi boyutu olarak kullanilir. */
typedef enum
{
    VEHICLE_TYPE_NONE = 0,   /* 0. satir; yazilmayan satirlar da sifirla dolar */
    VEHICLE_TYPE_CAR,
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
    FM_OK = 0,
    FM_ERR_PARAM,           /* Gecersiz parametre / NULL pointer             */
    FM_ERR_NOT_INIT,        /* FleetMgr init edilmemis                      */
    FM_ERR_CONFIG,          /* Konfigurasyon tablosu hatali                  */
    FM_ERR_NOT_FOUND,       /* ID filoda yok / uygun arac bulunamadi         */
    FM_ERR_WRONG_TYPE,      /* Islem bu arac tipi icin gecerli degil         */
    FM_ERR_OVER_CAPACITY,   /* Koltuk veya yuk kapasitesi asildi (arac)      */
    FM_ERR_LOW_FUEL         /* Yolculuk icin yakit yetersiz (arac)           */
} FM_Ret_t;

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

#endif /* FLEET_MGR_TYPES_H */
