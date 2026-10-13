/**
 * @file    Man.h
 * @brief   [fleetConfig] MAN TGX: TIR arayuzunu implement eden vendor.
 *
 *          Disariya tek bir arayuz tablosu acar: Man_TruckIfc.
 *          Ortak islemler (bilgi, sur, yakit, bakim) ve tira ozel
 *          islemler ayni tablonun alanlaridir.
 */
#ifndef MAN_H
#define MAN_H

#include "VehicleIfc.h"

extern const TruckIfc_t Man_TruckIfc;

#endif /* MAN_H */
