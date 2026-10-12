/**
 * @file    Integrator_Cfg.h
 * @brief   [Variant A2] Entegratorun ModuleMgr konfigurasyonu.
 *
 *          Bu header'i SADECE entegrator kullanir; ModuleMgr bu dosyayi
 *          ve icindeki sembolu bilmez. Konfigurasyon ModuleMgr'a
 *          ModuleMgr_Init(&Integrator_ModuleMgrCfg) ile parametre olarak verilir.
 */
#ifndef INTEGRATOR_CFG_H
#define INTEGRATOR_CFG_H

#include "ModuleMgr.h"

extern const ModuleMgr_Config_t Integrator_ModuleMgrCfg;

#endif /* INTEGRATOR_CFG_H */
