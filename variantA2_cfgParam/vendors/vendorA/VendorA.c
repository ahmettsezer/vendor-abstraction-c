/**
 * @file    VendorA.c
 * @brief   [Variant A2] VendorA implementasyonu.
 *
 *          Not: printf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "VendorA.h"

static uint32_t       s_value  = 0u;                    /* son yazilan deger */
static VendorStatus_t s_status = VENDOR_STATUS_UNKNOWN;

/* ---- Ifc implementasyonu ------------------------------------------------ */
static MM_Ret_t vendorA_read(uint32_t *pValue)
{
    *pValue = s_value;
    (void)printf("    [VendorA] Read   -> %lu\n", (unsigned long)s_value);
    return MM_OK;
}

static MM_Ret_t vendorA_write(uint32_t value)
{
    MM_Ret_t ret = MM_OK;

    if (s_status != VENDOR_STATUS_BUSY)
    {
        (void)printf("    [VendorA] Write  <- %lu  REDDEDILDI (START edilmemis)\n", (unsigned long)value);
        ret = MM_ERR_VENDOR;
    }
    else
    {
        s_value = value;
        (void)printf("    [VendorA] Write  <- %lu  (saklandi)\n", (unsigned long)value);
    }
    return ret;
}

static MM_Ret_t vendorA_execute(uint32_t cmd)
{
    MM_Ret_t ret = MM_OK;

    switch (cmd)
    {
        case MM_CMD_START:
            s_status = VENDOR_STATUS_BUSY;
            (void)printf("    [VendorA] Execute START\n");
            break;

        case MM_CMD_STOP:
            s_status = VENDOR_STATUS_IDLE;
            (void)printf("    [VendorA] Execute STOP\n");
            break;

        case MM_CMD_RESET:
            s_value  = 0u;
            s_status = VENDOR_STATUS_IDLE;
            (void)printf("    [VendorA] Execute RESET (deger = 0)\n");
            break;

        default:
            (void)printf("    [VendorA] Execute bilinmeyen komut %lu\n", (unsigned long)cmd);
            ret = MM_ERR_NOT_SUPPORTED;
            break;
    }
    return ret;
}

static VendorStatus_t vendorA_getStatus(void)
{
    return s_status;
}

/* ---- Arayuz tablosu: const, tanimlandigi satirda dolu ------------------ */
/* Statik analiz araci her alanin hedefini bu satirlardan kesin okur. */
const VendorIfc_t VendorA_Ifc =
{
    .pName     = "VendorA",
    .readIfc   = vendorA_read,
    .writeIfc  = vendorA_write,
    .execute   = vendorA_execute,
    .getStatus = vendorA_getStatus,
};

MM_Ret_t VendorA_Init(void)
{
    s_value  = 0u;
    s_status = VENDOR_STATUS_IDLE;
    return MM_OK;
}
