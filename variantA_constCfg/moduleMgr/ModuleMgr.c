/**
 * @file    ModuleMgr.c
 * @brief   [Variant A] Generic modul yoneticisi implementasyonu.
 *
 *          Mevcut versiyondan farklar:
 *            - s_vendors[] RAM tablosu ve Register/Unregister YOK.
 *            - Vendor'lara erisim 'const' g_moduleMgrCfg[] uzerinden.
 *            - MISRA: tek cikis noktasi (15.5), switch'te break (16.3).
 */
#include <stddef.h>
#include "ModuleMgr.h"

static bool s_isInit = false;

/* ------------------------------------------------------------------------ */
/* id'yi const konfigurasyonda arar; bulamazsa NULL doner. */
static const VendorIfc_t *findIfc(VendorId_t id)
{
    const VendorIfc_t *pIfc = NULL;

    for (uint8_t i = 0u; (i < g_moduleMgrCfgSize) && (pIfc == NULL); i++)
    {
        if (g_moduleMgrCfg[i].id == id)
        {
            pIfc = g_moduleMgrCfg[i].pIfc;
        }
    }
    return pIfc;
}

/* Ortak on kontrol: init + id lookup. */
static MM_Ret_t getIfc(VendorId_t id, const VendorIfc_t **ppIfc)
{
    MM_Ret_t ret = MM_OK;

    if (!s_isInit)
    {
        ret = MM_ERR_NOT_INIT;
    }
    else
    {
        *ppIfc = findIfc(id);
        if (*ppIfc == NULL)
        {
            ret = MM_ERR_NOT_FOUND;
        }
    }
    return ret;
}

/* ---- Yasam dongusu ------------------------------------------------------ */
/* Konfigurasyonu dogrular: NULL ifc, gecersiz veya tekrarlanan ID. */
MM_Ret_t ModuleMgr_Init(void)
{
    MM_Ret_t ret = MM_OK;

    for (uint8_t i = 0u; (i < g_moduleMgrCfgSize) && (ret == MM_OK); i++)
    {
        if ((g_moduleMgrCfg[i].pIfc == NULL) || (g_moduleMgrCfg[i].id == VENDOR_ID_INVALID))
        {
            ret = MM_ERR_CONFIG;
        }
        for (uint8_t j = (uint8_t)(i + 1u); (j < g_moduleMgrCfgSize) && (ret == MM_OK); j++)
        {
            if (g_moduleMgrCfg[i].id == g_moduleMgrCfg[j].id)
            {
                ret = MM_ERR_CONFIG;
            }
        }
    }

    s_isInit = (ret == MM_OK);
    return ret;
}

MM_Ret_t ModuleMgr_DeInit(void)
{
    s_isInit = false;
    return MM_OK;
}

/* ---- VendorId uzerinden erisim ----------------------------------------- */
MM_Ret_t ModuleMgr_Read(VendorId_t id, uint32_t *pValue)
{
    const VendorIfc_t *pIfc = NULL;
    MM_Ret_t ret = getIfc(id, &pIfc);

    if (ret == MM_OK)
    {
        if (pValue == NULL)
        {
            ret = MM_ERR_PARAM;
        }
        else if (pIfc->readIfc == NULL)
        {
            ret = MM_ERR_NOT_SUPPORTED;
        }
        else
        {
            ret = pIfc->readIfc(pValue);
        }
    }
    return ret;
}

MM_Ret_t ModuleMgr_Write(VendorId_t id, uint32_t value)
{
    const VendorIfc_t *pIfc = NULL;
    MM_Ret_t ret = getIfc(id, &pIfc);

    if (ret == MM_OK)
    {
        if (pIfc->writeIfc == NULL)
        {
            ret = MM_ERR_NOT_SUPPORTED;
        }
        else
        {
            ret = pIfc->writeIfc(value);   /* hedef: const tablodan kesin */
        }
    }
    return ret;
}

MM_Ret_t ModuleMgr_Execute(VendorId_t id, uint32_t cmd)
{
    const VendorIfc_t *pIfc = NULL;
    MM_Ret_t ret = getIfc(id, &pIfc);

    if (ret == MM_OK)
    {
        if (pIfc->execute == NULL)
        {
            ret = MM_ERR_NOT_SUPPORTED;
        }
        else
        {
            ret = pIfc->execute(cmd);
        }
    }
    return ret;
}

VendorStatus_t ModuleMgr_GetStatus(VendorId_t id)
{
    const VendorIfc_t *pIfc = NULL;
    VendorStatus_t status = VENDOR_STATUS_UNKNOWN;

    if ((getIfc(id, &pIfc) == MM_OK) && (pIfc->getStatus != NULL))
    {
        status = pIfc->getStatus();
    }
    return status;
}

/* ---- Dolasma ------------------------------------------------------------ */
uint8_t ModuleMgr_GetVendorCount(void)
{
    return s_isInit ? g_moduleMgrCfgSize : 0u;
}

VendorId_t ModuleMgr_GetVendorIdAt(uint8_t index)
{
    VendorId_t id = VENDOR_ID_INVALID;

    if (s_isInit && (index < g_moduleMgrCfgSize))
    {
        id = g_moduleMgrCfg[index].id;
    }
    return id;
}

const char *ModuleMgr_GetVendorName(VendorId_t id)
{
    const VendorIfc_t *pIfc = NULL;
    const char *pName = "?";

    if ((getIfc(id, &pIfc) == MM_OK) && (pIfc->pName != NULL))
    {
        pName = pIfc->pName;
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
        for (uint8_t i = 0u; i < g_moduleMgrCfgSize; i++)
        {
            const VendorIfc_t *pIfc = g_moduleMgrCfg[i].pIfc;
            if ((pIfc->execute != NULL) && (pIfc->execute(cmd) != MM_OK))
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
