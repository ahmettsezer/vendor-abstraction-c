/**
 * @file    Ford.h
 * @brief   [fleetConfig] Ford Tourneo Custom: OTOMOBIL arayuzunu implement eden vendor.
 *
 *          Disariya tek bir arayuz tablosu acar: Ford_CarIfc.
 *          Ortak islemler (bilgi, sur, yakit, bakim) ve otomobile ozel
 *          islemler ayni tablonun alanlaridir.
 */
#ifndef FORD_H
#define FORD_H

#include "VehicleIfc.h"

/* Derleme aninda dolu, flash'ta duran arayuz tablosu. */
extern const CarIfc_t Ford_CarIfc;

#endif /* FORD_H */
