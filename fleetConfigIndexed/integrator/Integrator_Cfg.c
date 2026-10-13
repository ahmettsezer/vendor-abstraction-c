/**
 * @file    Integrator_Cfg.c
 * @brief   [fleetConfigIndexed] Markalari bilen TEK dosya - A + ID = INDEX.
 *
 *          Filo tablosu arac ID'si ile index'lenir (designated initializer).
 *          Bir aracin satiri unutulursa sifirla dolar (VEHICLE_TYPE_NONE) ve
 *          FleetMgr_Init bunu reddeder. Otomobil alanina tir arayuzu yazmak
 *          derleme uyarisi/hatasidir.
 */
#include <stddef.h>
#include "Integrator_Cfg.h"
#include "Toyota.h"
#include "Ford.h"
#include "Man.h"
#include "Volvo.h"

static const FleetMgr_VehicleCfg_t s_fleet[VEHICLE_ID_COUNT] =
{
    /* ID (satir)                    type                 otomobil         tir             */
    [NO_VEHICLE_ID]             = { VEHICLE_TYPE_NONE,  NULL,            NULL            },
    [VEHICLE_ID_TOYOTA_COROLLA] = { VEHICLE_TYPE_CAR,   &Toyota_CarIfc,  NULL            },
    [VEHICLE_ID_FORD_TOURNEO]   = { VEHICLE_TYPE_CAR,   &Ford_CarIfc,    NULL            },
    [VEHICLE_ID_MAN_TGX]        = { VEHICLE_TYPE_TRUCK, NULL,            &Man_TruckIfc   },
    [VEHICLE_ID_VOLVO_FH]       = { VEHICLE_TYPE_TRUCK, NULL,            &Volvo_TruckIfc },
};

const FleetMgr_Config_t Integrator_FleetCfg =
{
    .pVehicles    = s_fleet,
    .vehicleCount = (uint8_t)VEHICLE_ID_COUNT,
};
