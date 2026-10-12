/**
 * @file    ModuleMgr.h
 * @brief   [runtimeRegisterConst] Vendor'lardan bagimsiz, generic modul yoneticisi.
 *
 *          - Vendor'lar ModuleMgr_Register() ile kendilerini kaydeder.
 *          - Diger tum API'ler VendorId uzerinden ilgili vendor'un
 *            fonksiyonunu, vendor'un kim oldugunu bilmeden cagirir.
 */
#ifndef MODULE_MGR_H
#define MODULE_MGR_H

#include "ModuleMgr_Types.h"

/* Ayni anda kayitli olabilecek maksimum vendor sayisi. */
#ifndef MODULE_MGR_MAX_VENDORS
#define MODULE_MGR_MAX_VENDORS  (8u)
#endif

/* ---- Yasam dongusu ------------------------------------------------------ */
MM_Ret_t ModuleMgr_Init(void);
MM_Ret_t ModuleMgr_DeInit(void);

/* ---- Vendor tarafinin kullandigi arayuz -------------------------------- */
MM_Ret_t ModuleMgr_Register(VendorId_t id, const VendorIfc_t *pIfc);
MM_Ret_t ModuleMgr_Unregister(VendorId_t id);

/* ---- VendorId uzerinden erisim ----------------------------------------- */
MM_Ret_t       ModuleMgr_Read     (VendorId_t id, uint32_t *pValue);
MM_Ret_t       ModuleMgr_Write    (VendorId_t id, uint32_t value);
MM_Ret_t       ModuleMgr_Execute  (VendorId_t id, uint32_t cmd);
VendorStatus_t ModuleMgr_GetStatus(VendorId_t id);

/* ---- Kayitli vendor'lari dolasma --------------------------------------- */
uint8_t     ModuleMgr_GetVendorCount(void);
VendorId_t  ModuleMgr_GetVendorIdAt(uint8_t index);
const char *ModuleMgr_GetVendorName(VendorId_t id);

/* Tum kayitli vendor'larda ayni komutu calistirir.
 * Ilk hatada durmaz; hata alan vendor sayisini pFailCount'a yazar (NULL olabilir). */
MM_Ret_t ModuleMgr_ExecuteAll(uint32_t cmd, uint8_t *pFailCount);

#endif /* MODULE_MGR_H */
