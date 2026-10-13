/**
 * @file    ModuleMgr.c
 * @brief   [switchDispatch] Generic modul yoneticisi implementasyonu.
 *
 *          Calisma zamaninda kayit (Register) yapan bir tasarima gore farklar:
 *            - Fonksiyon pointer, kayit tablosu, Register YOK.
 *            - Ortak kontroller (init, NULL parametre) burada; asil
 *              yonlendirme entegratorun ModuleMgr_Cfg_xxx() fonksiyonlarinda.
 *            - MISRA: tek cikis noktasi (15.5), switch'te break (16.3).
 */
#include <stddef.h>
#include "ModuleMgr.h"
#include "ModuleMgr_Cfg.h"

static bool s_isInit = false;

/* ---- Yasam dongusu ------------------------------------------------------ */
MM_Ret_t ModuleMgr_Init(void)
{
    s_isInit = true;
    return MM_OK;
}

MM_Ret_t ModuleMgr_DeInit(void)
{
    s_isInit = false;
    return MM_OK;
}

/* ---- VendorId uzerinden erisim ----------------------------------------- */
MM_Ret_t ModuleMgr_Read(VendorId_t id, uint32_t *pValue)
{
    MM_Ret_t ret;

    if (!s_isInit)
    {
        ret = MM_ERR_NOT_INIT;
    }
    else if (pValue == NULL)
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        ret = ModuleMgr_Cfg_Read(id, pValue);
    }
    return ret;
}

MM_Ret_t ModuleMgr_Write(VendorId_t id, uint32_t value)
{
    MM_Ret_t ret;

    if (!s_isInit)
    {
        ret = MM_ERR_NOT_INIT;
    }
    else
    {
        ret = ModuleMgr_Cfg_Write(id, value);   /* dogrudan cagri zinciri */
    }
    return ret;
}

MM_Ret_t ModuleMgr_Execute(VendorId_t id, uint32_t cmd)
{
    MM_Ret_t ret;

    if (!s_isInit)
    {
        ret = MM_ERR_NOT_INIT;
    }
    else
    {
        ret = ModuleMgr_Cfg_Execute(id, cmd);
    }
    return ret;
}

VendorStatus_t ModuleMgr_GetStatus(VendorId_t id)
{
    VendorStatus_t status = VENDOR_STATUS_UNKNOWN;

    if (s_isInit)
    {
        status = ModuleMgr_Cfg_GetStatus(id);
    }
    return status;
}

/* ---- Dolasma ------------------------------------------------------------ */
uint8_t ModuleMgr_GetVendorCount(void)
{
    return s_isInit ? g_moduleMgrVendorCount : 0u;
}

VendorId_t ModuleMgr_GetVendorIdAt(uint8_t index)
{
    VendorId_t id = VENDOR_ID_INVALID;

    if (s_isInit && (index < g_moduleMgrVendorCount))
    {
        id = g_moduleMgrVendorIds[index];
    }
    return id;
}

const char *ModuleMgr_GetVendorName(VendorId_t id)
{
    const char *pName = "?";

    if (s_isInit)
    {
        pName = ModuleMgr_Cfg_GetName(id);
    }
    return pName;
}

MM_Ret_t ModuleMgr_ExecuteAll(uint32_t cmd, uint8_t *pFailCount)
{
    MM_Ret_t ret = MM_OK;
    uint8_t fails = 0u;

    if (!s_isInit)
    {
        ret = MM_ERR_NOT_INIT;
    }
    else
    {
        for (uint8_t i = 0u; i < g_moduleMgrVendorCount; i++)
        {
            if (ModuleMgr_Cfg_Execute(g_moduleMgrVendorIds[i], cmd) != MM_OK)
            {
                fails++;
            }
        }
        if (fails > 0u)
        {
            ret = MM_ERR_VENDOR;
        }
    }

    if (pFailCount != NULL)
    {
        *pFailCount = fails;
    }
    return ret;
}
