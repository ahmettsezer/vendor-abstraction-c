/**
 * @file    VendorB.c
 * @brief   VendorB implementasyonu. VendorA ile ayni arayuzu sunar ama
 *          davranisi farklidir: yazilan degerleri toplar.
 */
#include <stdio.h>
#include "VendorB.h"
#include "ModuleMgr.h"

static uint32_t       s_total;                          /* yazilanlarin toplami */
static VendorStatus_t s_status = VENDOR_STATUS_UNKNOWN;

/* ---- Ifc implementasyonu ------------------------------------------------ */
static MM_Ret_t vendorB_read(uint32_t *pValue)
{
    *pValue = s_total;
    printf("    [VendorB] Read   -> %lu (toplam)\n", (unsigned long)s_total);
    return MM_OK;
}

static MM_Ret_t vendorB_write(uint32_t value)
{
    if (s_status != VENDOR_STATUS_BUSY)
    {
        printf("    [VendorB] Write  <- %lu  REDDEDILDI (START edilmemis)\n", (unsigned long)value);
        return MM_ERR_VENDOR;
    }
    s_total += value;
    printf("    [VendorB] Write  <- %lu  (toplam = %lu)\n",
           (unsigned long)value, (unsigned long)s_total);
    return MM_OK;
}

static MM_Ret_t vendorB_execute(uint32_t cmd)
{
    switch (cmd)
    {
        case MM_CMD_START:
            s_status = VENDOR_STATUS_BUSY;
            printf("    [VendorB] Execute START\n");
            return MM_OK;

        case MM_CMD_STOP:
            s_status = VENDOR_STATUS_IDLE;
            printf("    [VendorB] Execute STOP\n");
            return MM_OK;

        case MM_CMD_RESET:
            s_total  = 0u;
            s_status = VENDOR_STATUS_IDLE;
            printf("    [VendorB] Execute RESET (toplam = 0)\n");
            return MM_OK;

        default:
            printf("    [VendorB] Execute bilinmeyen komut %lu\n", (unsigned long)cmd);
            return MM_ERR_NOT_SUPPORTED;
    }
}

static VendorStatus_t vendorB_getStatus(void)
{
    return s_status;
}

/* ---- Register edilecek tablo ------------------------------------------- */
/* [variant0c] 'static const' ve tanimlandigi satirda dolu -> flash'ta durur,
 * calisma sirasinda degistirilemez. Statik analiz araci her alanin hedefini
 * bu satirlardan kesin okur. Register hala runtime'da yapilir. */
static const VendorIfc_t s_vendorBIfc =
{
    .pName     = "VendorB",
    .readIfc   = vendorB_read,
    .writeIfc  = vendorB_write,
    .execute   = vendorB_execute,
    .getStatus = vendorB_getStatus,
};

MM_Ret_t VendorB_Init(void)
{
    MM_Ret_t ret = ModuleMgr_Register(VENDOR_B_ID, &s_vendorBIfc);
    if (ret == MM_OK)   /* basarisiz register zaten kayitli instance'i bozmasin */
    {
        s_total  = 0u;
        s_status = VENDOR_STATUS_IDLE;
    }
    return ret;
}
