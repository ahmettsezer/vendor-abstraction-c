/**
 * @file    VendorB.h
 * @brief   Ornek vendor B: kendisine yazilan degerleri TOPLAR.
 *          read -> o ana kadarki toplami doner.
 */
#ifndef VENDOR_B_H
#define VENDOR_B_H

#include "ModuleMgr_Types.h"

#define VENDOR_B_ID     ((VendorId_t)0x0B01u)

/* Entegrator tarafindan cagrilir; kendini ModuleMgr'a register eder. */
MM_Ret_t VendorB_Init(void);

#endif /* VENDOR_B_H */
