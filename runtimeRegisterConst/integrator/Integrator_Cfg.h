/**
 * @file    Integrator_Cfg.h
 * @brief   Entegratorun projeye dahil ettigi vendor listesi.
 */
#ifndef INTEGRATOR_CFG_H
#define INTEGRATOR_CFG_H

#include <stdint.h>
#include "ModuleMgr_Types.h"

typedef MM_Ret_t (*fncptr_vendorInit)(void);

typedef struct
{
    const char        *pName;
    fncptr_vendorInit  init;
} VendorInitEntry_t;

extern const VendorInitEntry_t g_vendorInitTable[];
extern const uint8_t           g_vendorInitTableSize;

#endif /* INTEGRATOR_CFG_H */
