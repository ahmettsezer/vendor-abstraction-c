/* main.c — [payloadConfig] demo: yuk isteklerinin frame sinirinda uygulanmasi.
 * EKLENDI. Not: printf/setvbuf yalnizca demo icindir (MISRA 21.6 sapmasi). */
#include <stdio.h>
#include "payload.h"
#include "inventory_mgr.h"
#include "weight_balance.h"

static void report(const char *pTitle)
{
    (void)printf("%s\n", pTitle);
    for (uint32_t s = 0u; s < N_STATIONS; s++) {
        (void)printf("  istasyon %lu: kutle %6.1f kg, BIT %s\n",
                     (unsigned long)s, (double)payload_mass_kg(s),
                     (payload_bit_ok(s) != 0u) ? "OK" : "ARIZA");
    }
    (void)printf("  toplam yuk kutlesi: %.1f kg\n\n", (double)total_payload_mass_kg());
}

int main(void)
{
    (void)setvbuf(stdout, NULL, _IONBF, 0);

    report("1) Baslangic: tum istasyonlar PL_UNKNOWN");

    inventory_request(0u, PL_DROP_TANK);
    inventory_request(1u, PL_EMPTY);
    inventory_request(2u, PL_SENSOR_POD);
    /* istasyon 3 icin istek yok -> PL_UNKNOWN kalir */
    report("2) Istekler alindi, frame siniri HENUZ gelmedi (degisiklik yok)");

    inventory_frame_boundary();
    report("3) Frame siniri: istekler uygulandi");

    payload_bind(1u, (payload_id_t)99);   /* gecersiz deger -> PL_UNKNOWN satiri */
    report("4) Istasyon 1'e gecersiz id (99) baglandi -> guvenli varsayilan");

    return 0;
}
