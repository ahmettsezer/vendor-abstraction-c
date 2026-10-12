/**
 * @file    main.c
 * @brief   [switchDispatch] Entegrator ornegi - switch ile dagitim.
 *
 *          Senaryo runtimeRegister ile ayni; fark sadece 1. adimda:
 *          runtime Register yok, vendor'lar ModuleMgr_Cfg.c'de sabit.
 *
 *          Not: printf/setvbuf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "ModuleMgr.h"
#include "VendorA.h"
#include "VendorB.h"

static const char *retStr(MM_Ret_t r)
{
    const char *p = "?";

    switch (r)
    {
        case MM_OK:                p = "OK";                break;
        case MM_ERR_PARAM:         p = "ERR_PARAM";         break;
        case MM_ERR_NOT_INIT:      p = "ERR_NOT_INIT";      break;
        case MM_ERR_NOT_FOUND:     p = "ERR_NOT_FOUND";     break;
        case MM_ERR_NOT_SUPPORTED: p = "ERR_NOT_SUPPORTED"; break;
        case MM_ERR_VENDOR:        p = "ERR_VENDOR";        break;
        default:                                            break;
    }
    return p;
}

static const char *statusStr(VendorStatus_t s)
{
    const char *p = "UNKNOWN";

    switch (s)
    {
        case VENDOR_STATUS_IDLE:  p = "IDLE";  break;
        case VENDOR_STATUS_BUSY:  p = "BUSY";  break;
        case VENDOR_STATUS_ERROR: p = "ERROR"; break;
        default:                               break;
    }
    return p;
}

static void doWrite(VendorId_t id, uint32_t value)
{
    (void)printf("ModuleMgr_Write(0x%04X, %lu)\n", id, (unsigned long)value);
    MM_Ret_t ret = ModuleMgr_Write(id, value);
    (void)printf("  = %s\n", retStr(ret));
}

static void doRead(VendorId_t id)
{
    uint32_t value = 0u;
    (void)printf("ModuleMgr_Read(0x%04X)\n", id);
    MM_Ret_t ret = ModuleMgr_Read(id, &value);
    (void)printf("  = %s, value = %lu\n", retStr(ret), (unsigned long)value);
}

static void doExecute(VendorId_t id, uint32_t cmd)
{
    (void)printf("ModuleMgr_Execute(0x%04X, %lu)\n", id, (unsigned long)cmd);
    MM_Ret_t ret = ModuleMgr_Execute(id, cmd);
    (void)printf("  = %s\n", retStr(ret));
}

static void printAllStatus(void)
{
    for (uint8_t i = 0u; i < ModuleMgr_GetVendorCount(); i++)
    {
        VendorId_t id = ModuleMgr_GetVendorIdAt(i);
        (void)printf("  0x%04X %-8s status = %s\n",
                     id, ModuleMgr_GetVendorName(id), statusStr(ModuleMgr_GetStatus(id)));
    }
}

int main(void)
{
    (void)setvbuf(stdout, NULL, _IONBF, 0);

    /* 1) Vendor'lar kendi durumunu hazirlar (register YOK).
     *    Vendor listesi ModuleMgr_Cfg.c'deki switch'lerde sabit. */
    (void)printf("\n===== 1) Init (switch dispatch) =====\n");
    (void)printf("VendorA_Init()  = %s\n", retStr(VendorA_Init()));
    (void)printf("VendorB_Init()  = %s\n", retStr(VendorB_Init()));
    (void)printf("ModuleMgr_Init() = %s\n", retStr(ModuleMgr_Init()));
    printAllStatus();

    /* 2) START edilmeden yazma -> vendor reddeder */
    (void)printf("\n===== 2) START oncesi Write =====\n");
    doWrite(VENDOR_A_ID, 10u);

    /* 3) Tum vendor'lari tek cagriyla baslat */
    (void)printf("\n===== 3) ExecuteAll(START) =====\n");
    (void)printf("ModuleMgr_ExecuteAll(START)\n");
    (void)printf("  = %s\n", retStr(ModuleMgr_ExecuteAll(MM_CMD_START, NULL)));
    printAllStatus();

    /* 4) AYNI cagrilar, FARKLI davranis: A son degeri saklar, B toplar. */
    (void)printf("\n===== 4) Ayni Write'lar, iki vendor =====\n");
    doWrite(VENDOR_A_ID, 10u);
    doWrite(VENDOR_A_ID, 41u);
    doWrite(VENDOR_B_ID, 34u);
    doWrite(VENDOR_B_ID, 27u);

    (void)printf("\n===== 5) Read =====\n");
    doRead(VENDOR_A_ID);   /* beklenen: 41 */
    doRead(VENDOR_B_ID);   /* beklenen: 61 */

    /* 6) Sadece tek vendor'a komut */
    (void)printf("\n===== 6) Sadece VendorA'ya RESET =====\n");
    doExecute(VENDOR_A_ID, MM_CMD_RESET);
    doRead(VENDOR_A_ID);   /* sifirlandi: 0 */
    doRead(VENDOR_B_ID);   /* degismedi: 61 */

    /* 7) Hata durumu: konfigurasyonda olmayan ID */
    (void)printf("\n===== 7) Hata durumlari =====\n");
    doWrite(0x1234u, 1u);

    /* 8) Kapat */
    (void)printf("\n===== 8) ExecuteAll(STOP) =====\n");
    (void)printf("ModuleMgr_ExecuteAll(STOP)\n");
    (void)printf("  = %s\n", retStr(ModuleMgr_ExecuteAll(MM_CMD_STOP, NULL)));
    printAllStatus();

    (void)ModuleMgr_DeInit();
    return 0;
}
