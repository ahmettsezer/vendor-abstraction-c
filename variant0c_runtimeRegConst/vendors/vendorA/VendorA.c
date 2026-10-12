/**
 * @file    VendorA.c
 * @brief   VendorA implementasyonu. Tum fonksiyonlar static; dis dunyaya
 *          sadece VendorA_Init() acik. ModuleMgr fonksiyonlara register
 *          edilen tablo uzerinden ulasir.
 */
#include <stdio.h>
#include "VendorA.h"
#include "ModuleMgr.h"

static uint32_t       s_value;                          /* son yazilan deger */
static VendorStatus_t s_status = VENDOR_STATUS_UNKNOWN;

/* ---- Ifc implementasyonu ------------------------------------------------ */
static MM_Ret_t vendorA_read(uint32_t *pValue)
{
    *pValue = s_value;
    printf("    [VendorA] Read   -> %lu\n", (unsigned long)s_value);
    return MM_OK;
}

static MM_Ret_t vendorA_write(uint32_t value)
{
    if (s_status != VENDOR_STATUS_BUSY)
    {
        printf("    [VendorA] Write  <- %lu  REDDEDILDI (START edilmemis)\n", (unsigned long)value);
        return MM_ERR_VENDOR;
    }
    s_value = value;
    printf("    [VendorA] Write  <- %lu  (saklandi)\n", (unsigned long)value);
    return MM_OK;
}

static MM_Ret_t vendorA_execute(uint32_t cmd)
{
    switch (cmd)
    {
        case MM_CMD_START:
            s_status = VENDOR_STATUS_BUSY;
            printf("    [VendorA] Execute START\n");
            return MM_OK;

        case MM_CMD_STOP:
            s_status = VENDOR_STATUS_IDLE;
            printf("    [VendorA] Execute STOP\n");
            return MM_OK;

        case MM_CMD_RESET:
            s_value  = 0u;
            s_status = VENDOR_STATUS_IDLE;
            printf("    [VendorA] Execute RESET (deger = 0)\n");
            return MM_OK;

        default:
            printf("    [VendorA] Execute bilinmeyen komut %lu\n", (unsigned long)cmd);
            return MM_ERR_NOT_SUPPORTED;
    }
}

static VendorStatus_t vendorA_getStatus(void)
{
    return s_status;
}

/* ---- Register edilecek tablo ------------------------------------------- */
/* [variant0c] 'static const' ve tanimlandigi satirda dolu -> flash'ta durur,
 * calisma sirasinda degistirilemez. Statik analiz araci her alanin hedefini
 * bu satirlardan kesin okur. Register hala runtime'da yapilir. */
static const VendorIfc_t s_vendorAIfc =
{
    .pName     = "VendorA",
    .readIfc   = vendorA_read,
    .writeIfc  = vendorA_write,
    .execute   = vendorA_execute,
    .getStatus = vendorA_getStatus,
};

MM_Ret_t VendorA_Init(void)
{
    MM_Ret_t ret = ModuleMgr_Register(VENDOR_A_ID, &s_vendorAIfc);
    if (ret == MM_OK)   /* basarisiz register zaten kayitli instance'i bozmasin */
    {
        s_value  = 0u;
        s_status = VENDOR_STATUS_IDLE;
    }
    return ret;
}
