/**
 * @file    Integrator_Cfg.c
 * @brief   [fleetConfigIndexed] Filoda hangi aracin, hangi tipte ve hangi markanin
 *          arayuzuyle yonetilecegi SADECE burada yazilir.
 *
 *          Tablo 'const' -> flash'ta, degismez. ID, tip ve arayuz eslesmesi
 *          derleme aninda kesin; statik analiz araci satir satir okur.
 *
 *          Tablo arac ID'si ile index'lenir: s_fleet[ID] = { tip, arac arayuzu }.
 *          tablo[0] = NO_VEHICLE_ID ("arac yok"). Bir aracin satiri unutulursa
 *          sifirla dolar (VEHICLE_TYPE_NONE) ve FleetMgr_Init reddeder.
 *          DIKKAT: arayuz 'const void*' olarak saklanir; tipin arayuzle
 *          uyumlu olmasi (CAR -> CarIfc_t, TRUCK -> TruckIfc_t) BU DOSYANIN
 *          sorumlulugundadir, FleetMgr dogrulayamaz.
 *          Bir arayuz tablosu tek bir araci temsil eder; ayni tablo iki
 *          satirda kullanilamaz.
 *
 *          Yeni arac = ID enum'una bir isim + bu tabloya bir satir.
 */
#include <stddef.h>
#include "Integrator_Cfg.h"
#include "Toyota.h"
#include "Ford.h"
#include "Man.h"
#include "Volvo.h"

static const FleetMgr_VehicleCfg_t s_fleet[VEHICLE_ID_COUNT] =
{
    /* ID (satir)                    type                 arac (type'a uygun arayuz) */
    [NO_VEHICLE_ID]             = { VEHICLE_TYPE_NONE,  NULL             },
    [VEHICLE_ID_TOYOTA_COROLLA] = { VEHICLE_TYPE_CAR,   &Toyota_CarIfc   },
    [VEHICLE_ID_FORD_TOURNEO]   = { VEHICLE_TYPE_CAR,   &Ford_CarIfc     },
    [VEHICLE_ID_MAN_TGX]        = { VEHICLE_TYPE_TRUCK, &Man_TruckIfc    },
    [VEHICLE_ID_VOLVO_FH]       = { VEHICLE_TYPE_TRUCK, &Volvo_TruckIfc  },
};

const FleetMgr_Config_t Integrator_FleetCfg =
{
    .pVehicles    = s_fleet,
    .vehicleCount = (uint8_t)VEHICLE_ID_COUNT,

    /* Filo politikasi: bakim araligi arac TIPINE gore. */
    .serviceIntervalKm =
    {
        [VEHICLE_TYPE_CAR]   = 15000u,
        [VEHICLE_TYPE_TRUCK] = 30000u,
    },
};
