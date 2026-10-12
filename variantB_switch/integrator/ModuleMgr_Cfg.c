/**
 * @file    ModuleMgr_Cfg.c
 * @brief   [Variant B] Entegratorun dagitici (dispatcher) kodu.
 *
 *          Vendor'lari bilen TEK dosya. Fonksiyon pointer YOK; her cagri
 *          switch ile dogrudan yapilir. Statik analiz araci icin call graph
 *          tamamen aciktir:  ModuleMgr_Write -> ModuleMgr_Cfg_Write -> VendorA_Write
 *
 *          Gercek projelerde bu dosya bir konfigurator araci tarafindan
 *          uretilir. Yeni vendor = asagidaki 5 switch'in hepsine bir case
 *          + ID listesine bir satir.
 */
#include "ModuleMgr_Cfg.h"
#include "VendorA.h"
#include "VendorB.h"

const VendorId_t g_moduleMgrVendorIds[] =
{
    VENDOR_A_ID,
    VENDOR_B_ID,
};

const uint8_t g_moduleMgrVendorCount =
    (uint8_t)(sizeof(g_moduleMgrVendorIds) / sizeof(g_moduleMgrVendorIds[0]));

/* ------------------------------------------------------------------------ */
MM_Ret_t ModuleMgr_Cfg_Read(VendorId_t id, uint32_t *pValue)
{
    MM_Ret_t ret;

    switch (id)
    {
        case VENDOR_A_ID: ret = VendorA_Read(pValue); break;
        case VENDOR_B_ID: ret = VendorB_Read(pValue); break;
        default:          ret = MM_ERR_NOT_FOUND;     break;
    }
    return ret;
}

MM_Ret_t ModuleMgr_Cfg_Write(VendorId_t id, uint32_t value)
{
    MM_Ret_t ret;

    switch (id)
    {
        case VENDOR_A_ID: ret = VendorA_Write(value); break;
        case VENDOR_B_ID: ret = VendorB_Write(value); break;
        default:          ret = MM_ERR_NOT_FOUND;     break;
    }
    return ret;
}

MM_Ret_t ModuleMgr_Cfg_Execute(VendorId_t id, uint32_t cmd)
{
    MM_Ret_t ret;

    switch (id)
    {
        case VENDOR_A_ID: ret = VendorA_Execute(cmd); break;
        case VENDOR_B_ID: ret = VendorB_Execute(cmd); break;
        default:          ret = MM_ERR_NOT_FOUND;     break;
    }
    return ret;
}

VendorStatus_t ModuleMgr_Cfg_GetStatus(VendorId_t id)
{
    VendorStatus_t status;

    switch (id)
    {
        case VENDOR_A_ID: status = VendorA_GetStatus();   break;
        case VENDOR_B_ID: status = VendorB_GetStatus();   break;
        default:          status = VENDOR_STATUS_UNKNOWN; break;
    }
    return status;
}

const char *ModuleMgr_Cfg_GetName(VendorId_t id)
{
    const char *pName;

    switch (id)
    {
        case VENDOR_A_ID: pName = "VendorA"; break;
        case VENDOR_B_ID: pName = "VendorB"; break;
        default:          pName = "?";       break;
    }
    return pName;
}
