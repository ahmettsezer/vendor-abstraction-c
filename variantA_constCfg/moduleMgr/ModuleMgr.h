/**
 * @file    ModuleMgr.h
 * @brief   [Variant A] Generic modul yoneticisi - const konfigurasyon tablosu.
 *
 *          Runtime Register YOK. Hangi vendor'un hangi ID ile calisacagi,
 *          entegratorun derleme aninda yazdigi 'const' tablodadir.
 *          ModuleMgr tabloyu TANIMLAMAZ, sadece 'extern' olarak okur;
 *          boylece vendor'lari hala tanimaz.
 */
#ifndef MODULE_MGR_H
#define MODULE_MGR_H

#include "ModuleMgr_Types.h"

/* ---- Konfigurasyon (entegrator doldurur: ModuleMgr_Cfg.c) -------------- */
typedef struct
{
    VendorId_t         id;
    const VendorIfc_t *pIfc;
} ModuleMgr_CfgEntry_t;

extern const ModuleMgr_CfgEntry_t g_moduleMgrCfg[];
extern const uint8_t              g_moduleMgrCfgSize;

/* ---- Yasam dongusu ------------------------------------------------------ */
MM_Ret_t ModuleMgr_Init(void);      /* konfigurasyonu dogrular */
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
