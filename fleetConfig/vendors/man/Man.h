/**
 * @file    Man.h
 * @brief   [fleetConfig] MAN TGX: TIR arayuzunu implement eden vendor.
 *
 *          Disariya tek bir arayuz tablosu acar: Man_TruckIfc.
 *          Ortak islemler tablonun 'common' alaninda, tira ozel islemler
 *          tablonun kendisindedir.
 */
#ifndef MAN_H
#define MAN_H

#include "VehicleIfc.h"

extern const TruckIfc_t Man_TruckIfc;

MM_Ret_t Man_Init(void);

#endif /* MAN_H */
