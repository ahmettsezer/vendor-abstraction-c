/**
 * @file    Integrator_Cfg.c
 * @brief   [constConfig] Entegratorun ModuleMgr konfigurasyonu.
 *
 *          Sembol adini ModuleMgr DAYATMAZ ('extern' yok); entegrator istedigi
 *          adi verir ve konfigurasyonu Init'e parametre olarak gecer.
 *          Tablo 'const' -> flash'ta, degismez; ID <-> vendor eslesmesi
 *          derleme aninda kesin.
 *
 *          Yeni vendor = s_entries'e bir satir + main'de Init cagrisi.
 */
#include "Integrator_Cfg.h"
#include "VendorA.h"
#include "VendorB.h"

static const ModuleMgr_CfgEntry_t s_entries[] =
{
    { VENDOR_A_ID, &VendorA_Ifc },
    { VENDOR_B_ID, &VendorB_Ifc },
};

const ModuleMgr_Config_t Integrator_ModuleMgrCfg =
{
    .pEntries = s_entries,
    .count    = (uint8_t)(sizeof(s_entries) / sizeof(s_entries[0])),
};
