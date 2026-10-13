/* integration.c — the integrator's glue: the closed set of targets, as a const table */
/* [payloadConfig] Kokteki 'Payload' dosyasindan; icerik degistirilmedi,
 * sadece include'lar eklendi. */
#include "payload.h"
#include "tank.h"
#include "pod.h"

static float    no_mass(uint32_t s)  { (void)s; return 0.0f; }
static uint32_t bit_pass(uint32_t s) { (void)s; return 1u; }
static uint32_t bit_flag(uint32_t s) { (void)s; return 0u; }        /* UNKNOWN: flag the station */

static const payload_ops_t OPS[PL_COUNT] = {                        /* const: fixed at build time, in ROM */
    [PL_EMPTY]      = { no_mass,      bit_pass    },
    [PL_DROP_TANK]  = { tank_mass_kg, tank_bit_ok },
    [PL_SENSOR_POD] = { pod_mass_kg,  pod_bit_ok  },
    [PL_UNKNOWN]    = { no_mass,      bit_flag    },
};

static uint8_t type_of[N_STATIONS] = { PL_UNKNOWN, PL_UNKNOWN, PL_UNKNOWN, PL_UNKNOWN };   /* the only thing that moves */

void payload_bind(uint32_t s, payload_id_t id)
{
    if (s < N_STATIONS) {
        type_of[s] = (uint8_t)((id < PL_COUNT) ? id : PL_UNKNOWN);
    }
}

static uint32_t row_of(uint32_t s)                                   /* always a valid row */
{
    uint32_t id = (uint32_t)PL_UNKNOWN;
    if (s < N_STATIONS) {
        id = type_of[s];
    }
    return (id < (uint32_t)PL_COUNT) ? id : (uint32_t)PL_UNKNOWN;
}

float    payload_mass_kg(uint32_t s) { return OPS[row_of(s)].mass_kg(s); }
uint32_t payload_bit_ok(uint32_t s)  { return OPS[row_of(s)].bit_ok(s); }
