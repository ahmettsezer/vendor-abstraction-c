/**
 * @file    main.c
 * @brief   Entegrator ornegi: ModuleMgr'i ayaga kaldirir, secili vendor'lari
 *          init eder ve ModuleMgr uzerinden generic olarak kullanir.
 *
 *          Cikti okuma rehberi:
 *            "ModuleMgr_xxx(...)"  -> entegratorun yaptigi cagri
 *            "    [VendorX] ..."   -> ModuleMgr'in yonlendirdigi vendor fonksiyonu
 *            "  = ..."             -> ModuleMgr'in donus degeri
 */
#include <stdio.h>
#include "ModuleMgr.h"
#include "Integrator_Cfg.h"
#include "VendorA.h"   /* sadece VENDOR_A_ID icin */
#include "VendorB.h"   /* sadece VENDOR_B_ID icin */

static const char *retStr(MM_Ret_t r)
{
    switch (r)
    {
        case MM_OK:                return "OK";
        case MM_ERR_PARAM:         return "ERR_PARAM";
        case MM_ERR_NOT_INIT:      return "ERR_NOT_INIT";
        case MM_ERR_FULL:          return "ERR_FULL";
        case MM_ERR_DUPLICATE_ID:  return "ERR_DUPLICATE_ID";
        case MM_ERR_NOT_FOUND:     return "ERR_NOT_FOUND";
        case MM_ERR_NOT_SUPPORTED: return "ERR_NOT_SUPPORTED";
        case MM_ERR_VENDOR:        return "ERR_VENDOR";
        default:                   return "?";
    }
}

static const char *statusStr(VendorStatus_t s)
{
    switch (s)
    {
        case VENDOR_STATUS_IDLE:  return "IDLE";
        case VENDOR_STATUS_BUSY:  return "BUSY";
        case VENDOR_STATUS_ERROR: return "ERROR";
        default:                  return "UNKNOWN";
    }
}

/* ---- Cagrilari ekrana yazan kucuk yardimcilar -------------------------- */
static void doWrite(VendorId_t id, uint32_t value)
{
    printf("ModuleMgr_Write(0x%04X, %lu)\n", id, (unsigned long)value);
    MM_Ret_t ret = ModuleMgr_Write(id, value);
    printf("  = %s\n", retStr(ret));
}

static void doRead(VendorId_t id)
{
    uint32_t value = 0u;
    printf("ModuleMgr_Read(0x%04X)\n", id);
    MM_Ret_t ret = ModuleMgr_Read(id, &value);
    printf("  = %s, value = %lu\n", retStr(ret), (unsigned long)value);
}

static void doExecute(VendorId_t id, uint32_t cmd)
{
    printf("ModuleMgr_Execute(0x%04X, %lu)\n", id, (unsigned long)cmd);
    MM_Ret_t ret = ModuleMgr_Execute(id, cmd);
    printf("  = %s\n", retStr(ret));
}

static void printAllStatus(void)
{
    for (uint8_t i = 0u; i < ModuleMgr_GetVendorCount(); i++)
    {
        VendorId_t id = ModuleMgr_GetVendorIdAt(i);
        printf("  0x%04X %-8s status = %s\n",
               id, ModuleMgr_GetVendorName(id), statusStr(ModuleMgr_GetStatus(id)));
    }
}

int main(void)
{
    /* stdout'u buffer'siz yap: debugger altinda (pipe) printf ciktilari
     * program bitene kadar beklemesin, her satir aninda gorunsun. */
    setvbuf(stdout, NULL, _IONBF, 0);

    /* 1) ModuleMgr init, ardindan vendor'lar kendini register eder */
    printf("\n===== 1) Init + Register =====\n");
    ModuleMgr_Init();
    for (uint8_t i = 0u; i < g_vendorInitTableSize; i++)
    {
        MM_Ret_t ret = g_vendorInitTable[i].init();
        printf("%s_Init() = %s\n", g_vendorInitTable[i].pName, retStr(ret));
    }
    printAllStatus();

    /* 2) START edilmeden yazma -> vendor reddeder */
    printf("\n===== 2) START oncesi Write =====\n");
    doWrite(VENDOR_A_ID, 10u);

    /* 3) Tum vendor'lari tek cagriyla baslat */
    printf("\n===== 3) ExecuteAll(START) =====\n");
    printf("ModuleMgr_ExecuteAll(START)\n");
    printf("  = %s\n", retStr(ModuleMgr_ExecuteAll(MM_CMD_START, NULL)));
    printAllStatus();

    /* 4) AYNI cagrilar, FARKLI davranis:
     *    VendorA son degeri saklar, VendorB toplar. */
    printf("\n===== 4) Ayni Write'lar, iki vendor =====\n");
    doWrite(VENDOR_A_ID, 10u);
    doWrite(VENDOR_A_ID, 41u);
    doWrite(VENDOR_B_ID, 34u);
    doWrite(VENDOR_B_ID, 27u);

    printf("\n===== 5) Read =====\n");
    doRead(VENDOR_A_ID);   /* beklenen: 5  */
    doRead(VENDOR_B_ID);   /* beklenen: 15 */

    /* 6) Sadece tek vendor'a komut */
    printf("\n===== 6) Sadece VendorB'ye RESET =====\n");
    doExecute(VENDOR_A_ID, MM_CMD_RESET);
    doRead(VENDOR_A_ID);   /* degismedi: 5 */
    doRead(VENDOR_B_ID);   /* sifirlandi: 0 */

    /* 7) Hata durumlari ModuleMgr tarafinda yakalanir */
    printf("\n===== 7) Hata durumlari =====\n");
    doWrite(0x1234u, 1u);                    /* kayitli olmayan ID */
    printf("VendorA_Init() (2. kez) = %s\n", retStr(VendorA_Init()));

    /* 8) Kapat */
    printf("\n===== 8) ExecuteAll(STOP) =====\n");
    printf("ModuleMgr_ExecuteAll(STOP)\n");
    printf("  = %s\n", retStr(ModuleMgr_ExecuteAll(MM_CMD_STOP, NULL)));
    printAllStatus();

    ModuleMgr_DeInit();
    return 0;
}
