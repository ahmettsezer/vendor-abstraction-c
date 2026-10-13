/**
 * @file    Volvo.h
 * @brief   [fleetConfig] Volvo FH: TIR arayuzunu implement eden vendor.
 *
 *          Disariya tek bir arayuz tablosu acar: Volvo_TruckIfc.
 *          Ortak islemler (bilgi, sur, yakit, bakim) ve tira ozel
 *          islemler ayni tablonun alanlaridir.
 */
#ifndef VOLVO_H
#define VOLVO_H

#include "VehicleIfc.h"

extern const TruckIfc_t Volvo_TruckIfc;

MM_Ret_t Volvo_Init(void);

#endif /* VOLVO_H */
