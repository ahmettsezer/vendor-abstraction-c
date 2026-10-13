/* tank.c — vendor A: knows the contract, keeps its own state per station */
/* [payloadConfig] Kokteki 'Payload' dosyasindan; icerik degistirilmedi,
 * sadece include'lar eklendi. */
#include "payload.h"
#include "tank.h"

static float tank_fuel_kg[N_STATIONS];
float    tank_mass_kg(uint32_t s) { return 120.0f + tank_fuel_kg[s]; }
uint32_t tank_bit_ok(uint32_t s)  { return (tank_fuel_kg[s] >= 0.0f) ? 1u : 0u; }
