/**
 * @file    Integrator_Cfg.h
 * @brief   [fleetIndexConfig] Entegratorun filo konfigurasyonu.
 */
#ifndef INTEGRATOR_CFG_H
#define INTEGRATOR_CFG_H

#include "FleetMgr.h"

#define VEHICLE_ID_TOYOTA_COROLLA ((VehicleId_t)101u)
#define VEHICLE_ID_FORD_TOURNEO   ((VehicleId_t)102u)
#define VEHICLE_ID_MAN_TGX        ((VehicleId_t)201u)
#define VEHICLE_ID_VOLVO_FH       ((VehicleId_t)202u)

extern const FleetMgr_Config_t Integrator_FleetCfg;

#endif /* INTEGRATOR_CFG_H */
