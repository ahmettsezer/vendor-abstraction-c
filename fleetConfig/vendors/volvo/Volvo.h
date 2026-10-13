/**
 * @file    Volvo.h
 * @brief   [fleetConfig] Volvo: TIR arayuzunu implement eden vendor.
 */
#ifndef VOLVO_H
#define VOLVO_H

#include "VehicleIfc.h"

/* Volvo'nun bu filodaki araclari (instance numaralari). */
#define VOLVO_FH            (0u)
#define VOLVO_VEHICLE_COUNT (1u)

extern const VehicleCommonIfc_t Volvo_CommonIfc;
extern const TruckIfc_t         Volvo_TruckIfc;

MM_Ret_t Volvo_Init(void);

#endif /* VOLVO_H */
