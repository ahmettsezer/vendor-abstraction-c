/**
 * @file    Toyota.h
 * @brief   [fleetConfig] Toyota Corolla: OTOMOBIL arayuzunu implement eden vendor.
 *
 *          Disariya tek bir arayuz tablosu acar: Toyota_CarIfc.
 *          Ortak islemler (bilgi, sur, yakit, bakim) ve otomobile ozel
 *          islemler ayni tablonun alanlaridir.
 */
#ifndef TOYOTA_H
#define TOYOTA_H

#include "VehicleIfc.h"

/* Derleme aninda dolu, flash'ta duran arayuz tablosu. */
extern const CarIfc_t Toyota_CarIfc;

/* Aracin baslangic durumunu yukler (km, yakit, yolcu). */
MM_Ret_t Toyota_Init(void);

#endif /* TOYOTA_H */
