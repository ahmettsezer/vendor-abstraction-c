/**
 * @file    Volvo.h
 * @brief   [fleetConfig] Volvo FH: TIR arayuzunu implement eden vendor.
 *
 *          Disariya tek bir arayuz tablosu acar: Volvo_TruckIfc.
 *          Ortak islemler tablonun 'common' alaninda, tira ozel islemler
 *          tablonun kendisindedir.
 */
#ifndef VOLVO_H
#define VOLVO_H

#include "VehicleIfc.h"

extern const TruckIfc_t Volvo_TruckIfc;

MM_Ret_t Volvo_Init(void);

#endif /* VOLVO_H */
