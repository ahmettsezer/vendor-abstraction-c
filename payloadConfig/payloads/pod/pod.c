/* pod.c — vendor C */
/* [payloadConfig] Kokteki 'Payload' dosyasindan; icerik degistirilmedi,
 * sadece include eklendi. */
#include "pod.h"

float    pod_mass_kg(uint32_t s) { (void)s; return 200.0f; }
uint32_t pod_bit_ok(uint32_t s)  { (void)s; return 1u; }
