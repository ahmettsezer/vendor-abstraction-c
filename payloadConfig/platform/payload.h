/* payload.h — the platform's contract: names no payload type */
/* [payloadConfig] Kokteki 'Payload' dosyasindan; icerik degistirilmedi,
 * sadece header guard eklendi. */
#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <stdint.h>
#define N_STATIONS 4u

typedef enum { PL_EMPTY = 0, PL_DROP_TANK, PL_SENSOR_POD, PL_UNKNOWN, PL_COUNT } payload_id_t;

typedef float    (*mass_fn_t)(uint32_t station);    /* typedefs, so const lands on the pointer */
typedef uint32_t (*bit_fn_t)(uint32_t station);
typedef struct {
    mass_fn_t mass_kg;
    bit_fn_t  bit_ok;
} payload_ops_t;

float    payload_mass_kg(uint32_t s);               /* consumers call these */
uint32_t payload_bit_ok(uint32_t s);
void     payload_bind(uint32_t s, payload_id_t id); /* the inventory manager calls this, at a frame boundary */

#endif /* PAYLOAD_H */
