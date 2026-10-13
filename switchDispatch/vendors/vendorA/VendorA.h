/**
 * @file    VendorA.h
 * @brief   [switchDispatch] Ornek vendor A: kendisine yazilan SON degeri saklar.
 *
 *          Calisma zamaninda kayit (Register) yapan bir tasarima gore farki: fonksiyonlar static degil PUBLIC.
 *          Entegratorun dagiticisi (ModuleMgr_Cfg.c) bunlari dogrudan cagirir.
 */
#ifndef VENDOR_A_H
#define VENDOR_A_H

#include "ModuleMgr_Types.h"

#define VENDOR_A_ID     ((VendorId_t)0x0A01u)

MM_Ret_t       VendorA_Init     (void);
MM_Ret_t       VendorA_Read     (uint32_t *pValue);
MM_Ret_t       VendorA_Write    (uint32_t value);
MM_Ret_t       VendorA_Execute  (uint32_t cmd);
VendorStatus_t VendorA_GetStatus(void);

#endif /* VENDOR_A_H */
