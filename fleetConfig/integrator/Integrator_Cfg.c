/**
 * @file    Integrator_Cfg.c
 * @brief   [fleetConfig] Filoda hangi aracin, hangi tipte ve hangi markanin
 *          arayuzuyle yonetilecegi SADECE burada yazilir.
 *
 *          Tablo 'const' -> flash'ta, degismez. ID, tip ve arayuz eslesmesi
 *          derleme aninda kesin; statik analiz araci satir satir okur.
 *
 *          Her satir: { id, tip, otomobil arayuzu, tir arayuzu }.
 *          Tipe uymayan arayuz NULL birakilir. Bir arayuz tablosu tek bir
 *          araci temsil eder; ayni tablo iki satirda kullanilamaz.
 *
 *          Yeni arac = markanin arayuzu + bu tabloya bir satir + main'de Init.
 */
#include <stddef.h>
#include "Integrator_Cfg.h"
#include "Toyota.h"
#include "Ford.h"
#include "Man.h"
#include "Volvo.h"

static const FleetMgr_VehicleCfg_t s_fleet[] =
{
    /* id                        type                otomobil         tir             */
    { VEHICLE_ID_TOYOTA_COROLLA, VEHICLE_TYPE_CAR,   &Toyota_CarIfc,  NULL            },
    { VEHICLE_ID_FORD_TOURNEO,   VEHICLE_TYPE_CAR,   &Ford_CarIfc,    NULL            },
    { VEHICLE_ID_MAN_TGX,        VEHICLE_TYPE_TRUCK, NULL,            &Man_TruckIfc   },
    { VEHICLE_ID_VOLVO_FH,       VEHICLE_TYPE_TRUCK, NULL,            &Volvo_TruckIfc },
};

const FleetMgr_Config_t Integrator_FleetCfg =
{
    .pVehicles    = s_fleet,
    .vehicleCount = (uint8_t)(sizeof(s_fleet) / sizeof(s_fleet[0])),

    /* Filo politikasi: bakim araligi arac TIPINE gore. */
    .serviceIntervalKm =
    {
        [VEHICLE_TYPE_CAR]   = 15000u,
        [VEHICLE_TYPE_TRUCK] = 30000u,
    },
};
