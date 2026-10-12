/**
 * @file    ModuleMgr_Cfg.h
 * @brief   [Variant B] ModuleMgr'in entegratordan bekledigi dagitici API.
 *
 *          ModuleMgr bu fonksiyonlari BILDIRIR (declare) ama TANIMLAMAZ.
 *          Tanimlari entegratorun ModuleMgr_Cfg.c dosyasindadir (gercek
 *          projelerde bir konfigurator araci tarafindan uretilir).
 *          Boylece ModuleMgr.c hala hicbir vendor header'i include etmez.
 */
#ifndef MODULE_MGR_CFG_H
#define MODULE_MGR_CFG_H

#include "ModuleMgr_Types.h"

/* Konfigurasyondaki vendor ID listesi (dolasma icin). */
extern const VendorId_t g_moduleMgrVendorIds[];
extern const uint8_t    g_moduleMgrVendorCount;

/* id'ye gore ilgili vendor fonksiyonunu DOGRUDAN cagirir.
 * Bilinmeyen id icin MM_ERR_NOT_FOUND / VENDOR_STATUS_UNKNOWN / "?" doner. */
MM_Ret_t       ModuleMgr_Cfg_Read     (VendorId_t id, uint32_t *pValue);
MM_Ret_t       ModuleMgr_Cfg_Write    (VendorId_t id, uint32_t value);
MM_Ret_t       ModuleMgr_Cfg_Execute  (VendorId_t id, uint32_t cmd);
VendorStatus_t ModuleMgr_Cfg_GetStatus(VendorId_t id);
const char    *ModuleMgr_Cfg_GetName  (VendorId_t id);

#endif /* MODULE_MGR_CFG_H */
