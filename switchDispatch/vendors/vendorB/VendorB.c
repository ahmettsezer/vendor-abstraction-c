/**
 * @file    VendorB.c
 * @brief   [switchDispatch] VendorB implementasyonu.
 *
 *          Not: printf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "VendorB.h"

static uint32_t       s_total  = 0u;                    /* yazilanlarin toplami */
static VendorStatus_t s_status = VENDOR_STATUS_UNKNOWN;

MM_Ret_t VendorB_Init(void)
{
    s_total  = 0u;
    s_status = VENDOR_STATUS_IDLE;
    return MM_OK;
}

MM_Ret_t VendorB_Read(uint32_t *pValue)
{
    *pValue = s_total;
    (void)printf("    [VendorB] Read   -> %lu (toplam)\n", (unsigned long)s_total);
    return MM_OK;
}

MM_Ret_t VendorB_Write(uint32_t value)
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

MM_Ret_t VendorB_Execute(uint32_t cmd)
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

VendorStatus_t VendorB_GetStatus(void)
{
    return s_status;
}
