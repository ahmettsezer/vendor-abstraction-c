/**
 * @file    Integrator_Cfg.c
 * @brief   [fleetIndexConfig] Markalari bilen TEK dosya.
 *
 *          1) Tipe gore AYRI ve TIPLI arayuz tablolari (payload'daki OPS gibi):
 *             otomobil tablosuna tir arayuzu yazmak derleme hatasidir.
 *          2) Filo tablosu: her arac { id, tip, o tipin tablosundaki satir }.
 *
 *          Hepsi const -> flash'ta. 'void*' ve cast yok.
 */
#include "Integrator_Cfg.h"
#include "Toyota.h"
#include "Ford.h"
#include "Man.h"
#include "Volvo.h"

/* ---- Otomobil arayuzleri tablosu ---------------------------------------- */
#define CAR_TOYOTA_COROLLA   (0u)
#define CAR_FORD_TOURNEO     (1u)
#define CAR_COUNT            (2u)

static const CarIfc_t * const s_cars[CAR_COUNT] =
{
    [CAR_TOYOTA_COROLLA] = &Toyota_CarIfc,
    [CAR_FORD_TOURNEO]   = &Ford_CarIfc,
};

/* ---- Tir arayuzleri tablosu --------------------------------------------- */
#define TRUCK_MAN_TGX        (0u)
#define TRUCK_VOLVO_FH       (1u)
#define TRUCK_COUNT          (2u)

static const TruckIfc_t * const s_trucks[TRUCK_COUNT] =
{
    [TRUCK_MAN_TGX]  = &Man_TruckIfc,
    [TRUCK_VOLVO_FH] = &Volvo_TruckIfc,
};

/* ---- Filo tablosu ------------------------------------------------------- */
static const FleetMgr_VehicleCfg_t s_fleet[] =
{
    /* id                         type                index (tipin tablosunda) */
    { VEHICLE_ID_TOYOTA_COROLLA, VEHICLE_TYPE_CAR,   CAR_TOYOTA_COROLLA },
    { VEHICLE_ID_FORD_TOURNEO,   VEHICLE_TYPE_CAR,   CAR_FORD_TOURNEO   },
    { VEHICLE_ID_MAN_TGX,        VEHICLE_TYPE_TRUCK, TRUCK_MAN_TGX      },
    { VEHICLE_ID_VOLVO_FH,       VEHICLE_TYPE_TRUCK, TRUCK_VOLVO_FH     },
};

const FleetMgr_Config_t Integrator_FleetCfg =
{
    .pVehicles    = s_fleet,
    .vehicleCount = (uint8_t)(sizeof(s_fleet) / sizeof(s_fleet[0])),
    .ppCars       = s_cars,
    .carCount     = CAR_COUNT,
    .ppTrucks     = s_trucks,
    .truckCount   = TRUCK_COUNT,
};
