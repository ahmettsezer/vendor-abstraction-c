/**
 * @file    Man.h
 * @brief   [fleetConfig] MAN: TIR arayuzunu implement eden vendor.
 */
#ifndef MAN_H
#define MAN_H

#include "VehicleIfc.h"

/* MAN'in bu filodaki araclari (instance numaralari). */
#define MAN_TGX             (0u)
#define MAN_VEHICLE_COUNT   (1u)

extern const VehicleCommonIfc_t Man_CommonIfc;
extern const TruckIfc_t         Man_TruckIfc;

MM_Ret_t Man_Init(void);

#endif /* MAN_H */
