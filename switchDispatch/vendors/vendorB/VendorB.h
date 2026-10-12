/**
 * @file    VendorB.h
 * @brief   [switchDispatch] Ornek vendor B: kendisine yazilan degerleri TOPLAR.
 */
#ifndef VENDOR_B_H
#define VENDOR_B_H

#include "ModuleMgr_Types.h"

#define VENDOR_B_ID     ((VendorId_t)0x0B01u)

MM_Ret_t       VendorB_Init     (void);
MM_Ret_t       VendorB_Read     (uint32_t *pValue);
MM_Ret_t       VendorB_Write    (uint32_t value);
MM_Ret_t       VendorB_Execute  (uint32_t cmd);
VendorStatus_t VendorB_GetStatus(void);

#endif /* VENDOR_B_H */
