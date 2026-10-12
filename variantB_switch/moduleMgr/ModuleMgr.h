/**
 * @file    ModuleMgr.h
 * @brief   [Variant B] Generic modul yoneticisi - switch ile dagitim.
 *
 *          Disaridan gorunen API mevcut versiyonla ve Variant A ile ayni.
 *          Fark icerde: fonksiyon pointer yerine ModuleMgr_Cfg_xxx()
 *          dagitici fonksiyonlari cagrilir.
 */
#ifndef MODULE_MGR_H
#define MODULE_MGR_H

#include "ModuleMgr_Types.h"

/* ---- Yasam dongusu ------------------------------------------------------ */
MM_Ret_t ModuleMgr_Init(void);
MM_Ret_t ModuleMgr_DeInit(void);

/* ---- VendorId uzerinden erisim ----------------------------------------- */
MM_Ret_t       ModuleMgr_Read     (VendorId_t id, uint32_t *pValue);
MM_Ret_t       ModuleMgr_Write    (VendorId_t id, uint32_t value);
MM_Ret_t       ModuleMgr_Execute  (VendorId_t id, uint32_t cmd);
VendorStatus_t ModuleMgr_GetStatus(VendorId_t id);

/* ---- Konfigurasyondaki vendor'lari dolasma ----------------------------- */
uint8_t     ModuleMgr_GetVendorCount(void);
VendorId_t  ModuleMgr_GetVendorIdAt(uint8_t index);
const char *ModuleMgr_GetVendorName(VendorId_t id);

MM_Ret_t ModuleMgr_ExecuteAll(uint32_t cmd, uint8_t *pFailCount);

#endif /* MODULE_MGR_H */
