/* weight_balance.c — a consumer: knows the contract only */
/* [payloadConfig] Kokteki 'Payload' dosyasindan; icerik degistirilmedi,
 * sadece include'lar eklendi. */
#include "payload.h"
#include "weight_balance.h"

float total_payload_mass_kg(void)
{
    float total = 0.0f;
    for (uint32_t s = 0u; s < N_STATIONS; s++) {
        total += payload_mass_kg(s);
    }
    return total;
}
