/* tank.h — vendor A'nin disari actigi fonksiyonlar */
/* [payloadConfig] Eklendi: integration.c'nin OPS tablosu icin prototipler. */
#ifndef TANK_H
#define TANK_H

#include <stdint.h>

float    tank_mass_kg(uint32_t s);
uint32_t tank_bit_ok(uint32_t s);

#endif /* TANK_H */
