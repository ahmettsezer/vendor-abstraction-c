/**
 * @file    ModuleMgr_Cfg.c
 * @brief   [Variant A] Entegratorun ModuleMgr konfigurasyonu.
 *
 *          Projeye dahil edilen vendor'lar ve ID'leri SADECE burada yazilir.
 *          Tablo 'const' -> flash'ta, degismez. ID <-> vendor eslesmesi
 *          derleme aninda kesindir; statik analiz araci bunu satir satir okur.
 *
 *          Yeni vendor eklemek = bu tabloya bir satir + main'de Init cagrisi.
 */
#include "ModuleMgr.h"
#include "VendorA.h"
#include "VendorB.h"

const ModuleMgr_CfgEntry_t g_moduleMgrCfg[] =
{
    { VENDOR_A_ID, &VendorA_Ifc },
    { VENDOR_B_ID, &VendorB_Ifc },
};

const uint8_t g_moduleMgrCfgSize =
    (uint8_t)(sizeof(g_moduleMgrCfg) / sizeof(g_moduleMgrCfg[0]));
