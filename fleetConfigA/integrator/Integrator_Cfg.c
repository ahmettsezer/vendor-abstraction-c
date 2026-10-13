/**
 * @file    Integrator_Cfg.c
 * @brief   [fleetConfigA] Markalari bilen TEK dosya - A: TIPLI ALANLAR.
 *
 *          Tek filo tablosu; her satir bir arac. Tipe uymayan alan NULL.
 *          Otomobil alanina tir arayuzu yazmak derleme uyarisi/hatasidir.
 */
#include <stddef.h>
#include "Integrator_Cfg.h"
#include "Toyota.h"
#include "Ford.h"
#include "Man.h"
#include "Volvo.h"

static const FleetMgr_VehicleCfg_t s_fleet[] =
{
    /* id                         type                otomobil         tir             */
    { VEHICLE_ID_TOYOTA_COROLLA, VEHICLE_TYPE_CAR,   &Toyota_CarIfc,  NULL            },
    { VEHICLE_ID_FORD_TOURNEO,   VEHICLE_TYPE_CAR,   &Ford_CarIfc,    NULL            },
    { VEHICLE_ID_MAN_TGX,        VEHICLE_TYPE_TRUCK, NULL,            &Man_TruckIfc   },
    { VEHICLE_ID_VOLVO_FH,       VEHICLE_TYPE_TRUCK, NULL,            &Volvo_TruckIfc },
};

const FleetMgr_Config_t Integrator_FleetCfg =
{
    .pVehicles    = s_fleet,
    .vehicleCount = (uint8_t)(sizeof(s_fleet) / sizeof(s_fleet[0])),
};
