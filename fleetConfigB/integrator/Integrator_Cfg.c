/**
 * @file    Integrator_Cfg.c
 * @brief   [fleetConfigB] Markalari bilen TEK dosya - B: HER TIP ICIN AYRI TABLO.
 *
 *          Otomobiller bir tabloda, tirlar bir tabloda. Tip sutunu ve NULL
 *          alan yok. Otomobil tablosuna tir arayuzu yazmak derleme
 *          uyarisi/hatasidir.
 */
#include "Integrator_Cfg.h"
#include "Toyota.h"
#include "Ford.h"
#include "Man.h"
#include "Volvo.h"

static const FleetMgr_CarCfg_t s_cars[] =
{
    /* id                         otomobil arayuzu */
    { VEHICLE_ID_TOYOTA_COROLLA, &Toyota_CarIfc },
    { VEHICLE_ID_FORD_TOURNEO,   &Ford_CarIfc   },
};

static const FleetMgr_TruckCfg_t s_trucks[] =
{
    /* id                         tir arayuzu */
    { VEHICLE_ID_MAN_TGX,        &Man_TruckIfc   },
    { VEHICLE_ID_VOLVO_FH,       &Volvo_TruckIfc },
};

const FleetMgr_Config_t Integrator_FleetCfg =
{
    .pCars      = s_cars,
    .carCount   = (uint8_t)(sizeof(s_cars) / sizeof(s_cars[0])),
    .pTrucks    = s_trucks,
    .truckCount = (uint8_t)(sizeof(s_trucks) / sizeof(s_trucks[0])),
};
