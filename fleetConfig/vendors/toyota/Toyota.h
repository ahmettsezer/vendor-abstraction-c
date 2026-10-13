/**
 * @file    Toyota.h
 * @brief   [fleetConfig] Toyota: OTOMOBIL arayuzunu implement eden vendor.
 *
 *          Filoda iki Toyota var; entegrator hangisini kastettigini
 *          'instance' ile belirtir (TOYOTA_COROLLA, TOYOTA_PROACE_VERSO).
 */
#ifndef TOYOTA_H
#define TOYOTA_H

#include "VehicleIfc.h"

/* Toyota'nin bu filodaki araclari (instance numaralari). */
#define TOYOTA_COROLLA          (0u)
#define TOYOTA_PROACE_VERSO     (1u)
#define TOYOTA_VEHICLE_COUNT    (2u)

/* Derleme aninda dolu, flash'ta duran arayuz tablolari. */
extern const VehicleCommonIfc_t Toyota_CommonIfc;
extern const CarIfc_t           Toyota_CarIfc;

/* Araclarin baslangic durumunu yukler (km, yakit, yolcu). */
MM_Ret_t Toyota_Init(void);

#endif /* TOYOTA_H */
