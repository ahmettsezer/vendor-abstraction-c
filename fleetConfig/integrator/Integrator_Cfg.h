/**
 * @file    Integrator_Cfg.h
 * @brief   [fleetConfig] Entegratorun filo konfigurasyonu.
 *
 *          Bu header'i SADECE entegrator kullanir; ModuleMgr bilmez.
 *          Konfigurasyon ModuleMgr_Init(&Integrator_FleetCfg) ile verilir.
 */
#ifndef INTEGRATOR_CFG_H
#define INTEGRATOR_CFG_H

#include "ModuleMgr.h"

/* Filo numaralari: 1xx otomobil, 2xx tir (sadece okunabilirlik icin;
 * ModuleMgr tipi numaradan degil konfigurasyondaki 'type' alanindan okur). */
#define FLEET_ID_TOYOTA_COROLLA ((VehicleId_t)101u)
#define FLEET_ID_FORD_TOURNEO   ((VehicleId_t)102u)
#define FLEET_ID_MAN_TGX        ((VehicleId_t)201u)
#define FLEET_ID_VOLVO_FH       ((VehicleId_t)202u)

extern const ModuleMgr_Config_t Integrator_FleetCfg;

#endif /* INTEGRATOR_CFG_H */
