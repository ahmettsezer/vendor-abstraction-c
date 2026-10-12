/**
 * @file    ModuleMgr.c
 * @brief   [constConfig] Generic modul yoneticisi implementasyonu.
 *
 *          runtimeRegister'dan farklar:
 *            - Register ve RAM'deki kayit dizisi YOK. Konfigurasyon s_pCfg ile
 *              tutulur (Init'te BIR KEZ atanan tek pointer); gosterdigi tablo
 *              entegratorde 'const' ve flash'ta.
 *            - "Init edildi mi" kontrolu: s_pCfg != NULL.
 *            - MISRA: tek cikis noktasi (15.5), switch'te break (16.3).
 */
#include <stddef.h>
#include "ModuleMgr.h"

/* NULL = init edilmemis. Init'te bir kez atanir, DeInit'te sifirlanir. */
static const ModuleMgr_Config_t *s_pCfg = NULL;

/* ------------------------------------------------------------------------ */
/* Konfigurasyonu dogrular: NULL tablo/ifc, gecersiz veya tekrarlanan ID. */
static MM_Ret_t validateCfg(const ModuleMgr_Config_t *pCfg)
{
    MM_Ret_t ret = MM_OK;

    if ((pCfg == NULL) || (pCfg->pEntries == NULL))
    {
        ret = MM_ERR_PARAM;
    }
    else
    {
        for (uint8_t i = 0u; (ret == MM_OK) && (i < pCfg->count); i++)
        {
            const ModuleMgr_CfgEntry_t *pE = &pCfg->pEntries[i];

            if ((pE->pIfc == NULL) || (pE->id == VENDOR_ID_INVALID))
            {
                ret = MM_ERR_CONFIG;
            }
            for (uint8_t j = (uint8_t)(i + 1u); (ret == MM_OK) && (j < pCfg->count); j++)
            {
                if (pE->id == pCfg->pEntries[j].id)
                {
                    ret = MM_ERR_CONFIG;
                }
            }
        }
    }
    return ret;
}

/* id'yi konfigurasyonda arar; bulamazsa NULL doner. s_pCfg != NULL varsayar. */
static const VendorIfc_t *findIfc(VendorId_t id)
{
    const VendorIfc_t *pIfc = NULL;

    for (uint8_t i = 0u; (i < s_pCfg->count) && (pIfc == NULL); i++)
    {
        if (s_pCfg->pEntries[i].id == id)
        {
            pIfc = s_pCfg->pEntries[i].pIfc;
        }
    }
    return pIfc;
}

/* Ortak on kontrol: init + id lookup. */
static MM_Ret_t getIfc(VendorId_t id, const VendorIfc_t **ppIfc)
{
    MM_Ret_t ret = MM_OK;

    if (s_pCfg == NULL)
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
MM_Ret_t ModuleMgr_Init(const ModuleMgr_Config_t *pCfg)
{
    MM_Ret_t ret = validateCfg(pCfg);

    /* Hatali konfigurasyon asla saklanmaz -> ModuleMgr init edilmemis kalir. */
    s_pCfg = (ret == MM_OK) ? pCfg : NULL;
    return ret;
}

MM_Ret_t ModuleMgr_DeInit(void)
{
    s_pCfg = NULL;
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
            ret = pIfc->writeIfc(value);   /* hedef: Init'e verilen const tablodan */
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
    return (s_pCfg != NULL) ? s_pCfg->count : 0u;
}

VendorId_t ModuleMgr_GetVendorIdAt(uint8_t index)
{
    VendorId_t id = VENDOR_ID_INVALID;

    if ((s_pCfg != NULL) && (index < s_pCfg->count))
    {
        id = s_pCfg->pEntries[index].id;
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

    if (s_pCfg == NULL)
    {
        ret = MM_ERR_NOT_INIT;
    }
    else
    {
        for (uint8_t i = 0u; i < s_pCfg->count; i++)
        {
            const VendorIfc_t *pIfc = s_pCfg->pEntries[i].pIfc;
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
