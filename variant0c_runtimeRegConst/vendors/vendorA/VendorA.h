/**
 * @file    VendorA.h
 * @brief   Ornek vendor A: kendisine yazilan SON degeri saklar.
 *          read -> en son yazilan degeri doner.
 */
#ifndef VENDOR_A_H
#define VENDOR_A_H

#include "ModuleMgr_Types.h"

#define VENDOR_A_ID     ((VendorId_t)0x0A01u)

/* Entegrator tarafindan cagrilir; kendini ModuleMgr'a register eder. */
MM_Ret_t VendorA_Init(void);

#endif /* VENDOR_A_H */
