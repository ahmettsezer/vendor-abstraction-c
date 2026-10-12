/**
 * @file    ModuleMgr.h
 * @brief   [Variant A2] Generic modul yoneticisi - konfigurasyon Init parametresi.
 *
 *          Variant A'dan farki: 'extern g_moduleMgrCfg' YOK.
 *          ModuleMgr disaridan hicbir sembol beklemez (link bagimliligi yok).
 *          Entegrator const konfigurasyonu ModuleMgr_Init(&cfg) ile verir
 *          (AUTOSAR post-build tarzi: Xxx_Init(const Xxx_ConfigType *)).
 *
 *          Kural: ModuleMgr_Init tek yerden, bir kez cagrilir. Boylece statik
 *          analiz araci s_pCfg'nin tek bir const nesneyi gosterdigini bilir.
 */
#ifndef MODULE_MGR_H
#define MODULE_MGR_H

#include "ModuleMgr_Types.h"

/* ---- Konfigurasyon tipleri (ModuleMgr tanimlar, entegrator doldurur) --- */
typedef struct
{
    VendorId_t         id;
    const VendorIfc_t *pIfc;
} ModuleMgr_CfgEntry_t;

typedef struct
{
    const ModuleMgr_CfgEntry_t *pEntries;
    uint8_t                     count;
} ModuleMgr_Config_t;

/* ---- Yasam dongusu ------------------------------------------------------ */
MM_Ret_t ModuleMgr_Init(const ModuleMgr_Config_t *pCfg);   /* dogrular + saklar */
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
