/**
 * @file    ModuleMgr.c
 * @brief   Generic modul yoneticisi implementasyonu.
 *
 *          Bu dosya hicbir vendor header'ini include ETMEZ.
 *          Vendor'lara tek erisim yolu register edilmis VendorIfc_t tablosudur.
 *
 *          [variant0c] variant0'dan tek fark: tablolar ve ModuleMgr'in tuttugu
 *          pointer'lar const. ModuleMgr tabloyu sadece okuyabilir (MISRA 8.13);
 *          vendor tablolari flash'ta durur. Register hala runtime'da yapilir.
 */
#include <stddef.h>
#include "ModuleMgr.h"

typedef struct
{
    bool               used;
    VendorId_t         id;
    const VendorIfc_t *pIfc;
} VendorEntry_t;

static VendorEntry_t s_vendors[MODULE_MGR_MAX_VENDORS];
static uint8_t       s_count;
static bool          s_isInit;

/* ------------------------------------------------------------------------ */
static VendorEntry_t *findEntry(VendorId_t id)
{
    for (uint8_t i = 0u; i < MODULE_MGR_MAX_VENDORS; i++)
    {
        if (s_vendors[i].used && (s_vendors[i].id == id))
        {
            return &s_vendors[i];
        }
    }
    return NULL;
}

/* Ortak on kontrol: init + id lookup. */
static MM_Ret_t getIfc(VendorId_t id, const VendorIfc_t **ppIfc)
{
    if (!s_isInit)
    {
        return MM_ERR_NOT_INIT;
    }

    const VendorEntry_t *pEntry = findEntry(id);
    if (pEntry == NULL)
    {
        return MM_ERR_NOT_FOUND;
    }

    *ppIfc = pEntry->pIfc;
    return MM_OK;
}

/* ---- Yasam dongusu ------------------------------------------------------ */
MM_Ret_t ModuleMgr_Init(void)
{
    for (uint8_t i = 0u; i < MODULE_MGR_MAX_VENDORS; i++)
    {
        s_vendors[i].used = false;
        s_vendors[i].id   = VENDOR_ID_INVALID;
        s_vendors[i].pIfc = NULL;
    }
    s_count  = 0u;
    s_isInit = true;
    return MM_OK;
}

MM_Ret_t ModuleMgr_DeInit(void)
{
    s_isInit = false;
    s_count  = 0u;
    return MM_OK;
}

/* ---- Register ----------------------------------------------------------- */
MM_Ret_t ModuleMgr_Register(VendorId_t id, const VendorIfc_t *pIfc)
{
    if (!s_isInit)
    {
        return MM_ERR_NOT_INIT;
    }
    if ((pIfc == NULL) || (id == VENDOR_ID_INVALID))
    {
        return MM_ERR_PARAM;
    }
    if (findEntry(id) != NULL)
    {
        return MM_ERR_DUPLICATE_ID;
    }

    for (uint8_t i = 0u; i < MODULE_MGR_MAX_VENDORS; i++)
    {
        if (!s_vendors[i].used)
        {
            s_vendors[i].used = true;
            s_vendors[i].id   = id;
            s_vendors[i].pIfc = pIfc;
            s_count++;
            return MM_OK;
        }
    }
    return MM_ERR_FULL;
}

MM_Ret_t ModuleMgr_Unregister(VendorId_t id)
{
    if (!s_isInit)
    {
        return MM_ERR_NOT_INIT;
    }

    VendorEntry_t *pEntry = findEntry(id);
    if (pEntry == NULL)
    {
        return MM_ERR_NOT_FOUND;
    }

    pEntry->used = false;
    pEntry->id   = VENDOR_ID_INVALID;
    pEntry->pIfc = NULL;
    s_count--;
    return MM_OK;
}

/* ---- VendorId uzerinden erisim ----------------------------------------- */
MM_Ret_t ModuleMgr_Read(VendorId_t id, uint32_t *pValue)
{
    const VendorIfc_t *pIfc = NULL;
    MM_Ret_t ret = getIfc(id, &pIfc);
    if (ret != MM_OK)
    {
        return ret;
    }
    if (pValue == NULL)
    {
        return MM_ERR_PARAM;
    }
    if (pIfc->readIfc == NULL)
    {
        return MM_ERR_NOT_SUPPORTED;
    }
    return pIfc->readIfc(pValue);
}

MM_Ret_t ModuleMgr_Write(VendorId_t id, uint32_t value)
{
    const VendorIfc_t *pIfc = NULL;
    MM_Ret_t ret = getIfc(id, &pIfc);
    if (ret != MM_OK)
    {
        return ret;
    }
    if (pIfc->writeIfc == NULL)
    {
        return MM_ERR_NOT_SUPPORTED;
    }
    return pIfc->writeIfc(value);
}

MM_Ret_t ModuleMgr_Execute(VendorId_t id, uint32_t cmd)
{
    const VendorIfc_t *pIfc = NULL;
    MM_Ret_t ret = getIfc(id, &pIfc);
    
    if (ret != MM_OK)
    {
        return ret;
    }
    if (pIfc->execute == NULL)
    {
        return MM_ERR_NOT_SUPPORTED;
    }
    return pIfc->execute(cmd);
}

VendorStatus_t ModuleMgr_GetStatus(VendorId_t id)
{
    const VendorIfc_t *pIfc = NULL;
    if ((getIfc(id, &pIfc) != MM_OK) || (pIfc->getStatus == NULL))
    {
        return VENDOR_STATUS_UNKNOWN;
    }
    return pIfc->getStatus();
}

/* ---- Dolasma ------------------------------------------------------------ */
uint8_t ModuleMgr_GetVendorCount(void)
{
    return s_isInit ? s_count : 0u;
}

VendorId_t ModuleMgr_GetVendorIdAt(uint8_t index)
{
    if (!s_isInit)
    {
        return VENDOR_ID_INVALID;
    }

    /* index, kayitli vendor'lar arasindaki sira (bosluklar atlanir). */
    uint8_t n = 0u;
    for (uint8_t i = 0u; i < MODULE_MGR_MAX_VENDORS; i++)
    {
        if (s_vendors[i].used)
        {
            if (n == index)
            {
                return s_vendors[i].id;
            }
            n++;
        }
    }
    return VENDOR_ID_INVALID;
}

const char *ModuleMgr_GetVendorName(VendorId_t id)
{
    const VendorIfc_t *pIfc = NULL;
    if ((getIfc(id, &pIfc) != MM_OK) || (pIfc->pName == NULL))
    {
        return "?";
    }
    return pIfc->pName;
}

MM_Ret_t ModuleMgr_ExecuteAll(uint32_t cmd, uint8_t *pFailCount)
{
    if (!s_isInit)
    {
        return MM_ERR_NOT_INIT;
    }

    uint8_t fails = 0u;
    for (uint8_t i = 0u; i < MODULE_MGR_MAX_VENDORS; i++)
    {
        if (s_vendors[i].used && (s_vendors[i].pIfc->execute != NULL))
        {
            if (s_vendors[i].pIfc->execute(cmd) != MM_OK)
            {
                fails++;
            }
        }
    }

    if (pFailCount != NULL)
    {
        *pFailCount = fails;
    }
    return (fails == 0u) ? MM_OK : MM_ERR_VENDOR;
}
