/**
 * @file    Integrator_Cfg.c
 * @brief   [fleetConfig] Filoda hangi aracin, hangi tipte, hangi markanin
 *          hangi arayuzuyle yonetilecegi SADECE burada yazilir.
 *
 *          Tablo 'const' -> flash'ta, degismez. ID, tip ve arayuz eslesmesi
 *          derleme aninda kesin; statik analiz araci satir satir okur.
 *
 *          Yeni arac  = s_fleet'e bir satir (+ yeni markaysa main'de Init).
 *          Yeni marka = markanin Common + tipe ozel arayuzu + bir satir.
 */
#include <stddef.h>
#include "Integrator_Cfg.h"
#include "Toyota.h"
#include "Man.h"
#include "Volvo.h"

static const ModuleMgr_VehicleCfg_t s_fleet[] =
{
    /* id                 type                instance             ortak arayuz        otomobil        tir             */
    { FLEET_ID_COROLLA,   VEHICLE_TYPE_CAR,   TOYOTA_COROLLA,      &Toyota_CommonIfc,  &Toyota_CarIfc, NULL            },
    { FLEET_ID_PROACE,    VEHICLE_TYPE_CAR,   TOYOTA_PROACE_VERSO, &Toyota_CommonIfc,  &Toyota_CarIfc, NULL            },
    { FLEET_ID_MAN_TGX,   VEHICLE_TYPE_TRUCK, MAN_TGX,             &Man_CommonIfc,     NULL,           &Man_TruckIfc   },
    { FLEET_ID_VOLVO_FH,  VEHICLE_TYPE_TRUCK, VOLVO_FH,            &Volvo_CommonIfc,   NULL,           &Volvo_TruckIfc },
};

const ModuleMgr_Config_t Integrator_FleetCfg =
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
