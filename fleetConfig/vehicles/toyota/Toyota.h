/**
 * @file    Toyota.h
 * @brief   [fleetConfig] Toyota Corolla: OTOMOBIL arayuzunu implement eden arac.
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

#endif /* TOYOTA_H */
