/**
 * @file    VendorB.c
 * @brief   [Variant A] VendorB implementasyonu.
 *
 *          Not: printf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "VendorB.h"

static uint32_t       s_total  = 0u;                    /* yazilanlarin toplami */
static VendorStatus_t s_status = VENDOR_STATUS_UNKNOWN;

/* ---- Ifc implementasyonu ------------------------------------------------ */
static MM_Ret_t vendorB_read(uint32_t *pValue)
{
    *pValue = s_total;
    (void)printf("    [VendorB] Read   -> %lu (toplam)\n", (unsigned long)s_total);
    return MM_OK;
}

static MM_Ret_t vendorB_write(uint32_t value)
{
    MM_Ret_t ret = MM_OK;

    if (s_status != VENDOR_STATUS_BUSY)
    {
        (void)printf("    [VendorB] Write  <- %lu  REDDEDILDI (START edilmemis)\n", (unsigned long)value);
        ret = MM_ERR_VENDOR;
    }
    else
    {
        s_total += value;
        (void)printf("    [VendorB] Write  <- %lu  (toplam = %lu)\n",
                     (unsigned long)value, (unsigned long)s_total);
    }
    return ret;
}

static MM_Ret_t vendorB_execute(uint32_t cmd)
{
    MM_Ret_t ret = MM_OK;

    switch (cmd)
    {
        case MM_CMD_START:
            s_status = VENDOR_STATUS_BUSY;
            (void)printf("    [VendorB] Execute START\n");
            break;

        case MM_CMD_STOP:
            s_status = VENDOR_STATUS_IDLE;
            (void)printf("    [VendorB] Execute STOP\n");
            break;

        case MM_CMD_RESET:
            s_total  = 0u;
            s_status = VENDOR_STATUS_IDLE;
            (void)printf("    [VendorB] Execute RESET (toplam = 0)\n");
            break;

        default:
            (void)printf("    [VendorB] Execute bilinmeyen komut %lu\n", (unsigned long)cmd);
            ret = MM_ERR_NOT_SUPPORTED;
            break;
    }
    return ret;
}

static VendorStatus_t vendorB_getStatus(void)
{
    return s_status;
}

/* ---- Arayuz tablosu: const, tanimlandigi satirda dolu ------------------ */
const VendorIfc_t VendorB_Ifc =
{
    .pName     = "VendorB",
    .readIfc   = vendorB_read,
    .writeIfc  = vendorB_write,
    .execute   = vendorB_execute,
    .getStatus = vendorB_getStatus,
};

MM_Ret_t VendorB_Init(void)
{
    s_total  = 0u;
    s_status = VENDOR_STATUS_IDLE;
    return MM_OK;
}
