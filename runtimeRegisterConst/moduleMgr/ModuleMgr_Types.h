/**
 * @file    ModuleMgr_Types.h
 * @brief   ModuleMgr <-> Vendor sozlesmesi (contract).
 *
 *          Vendor'larin implement etmesi gereken arayuz burada tanimlidir.
 *          Vendor'lar SADECE bu dosyayi ve ModuleMgr.h'i bilir;
 *          ModuleMgr ise hicbir vendor'u tanimaz.
 */
#ifndef MODULE_MGR_TYPES_H
#define MODULE_MGR_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* Vendor kimligi. Benzersizligi ModuleMgr register sirasinda dogrular. */
typedef uint16_t VendorId_t;

#define VENDOR_ID_INVALID   ((VendorId_t)0xFFFFu)

/* Ortak donus kodu - hem ModuleMgr hem vendor fonksiyonlari kullanir. */
typedef enum
{
    MM_OK = 0,
    MM_ERR_PARAM,           /* Gecersiz parametre / NULL pointer        */
    MM_ERR_NOT_INIT,        /* ModuleMgr init edilmemis                 */
    MM_ERR_FULL,            /* Vendor tablosu dolu                      */
    MM_ERR_DUPLICATE_ID,    /* Ayni VendorId ile ikinci register        */
    MM_ERR_NOT_FOUND,       /* VendorId kayitli degil                   */
    MM_ERR_NOT_SUPPORTED,   /* Vendor ilgili fonksiyonu sunmuyor (NULL) */
    MM_ERR_VENDOR           /* Vendor kendi icinde hata dondu           */
} MM_Ret_t;

/* Vendor'un calisma durumu - getStatus ile sorgulanir. */
typedef enum
{
    VENDOR_STATUS_UNKNOWN = 0,
    VENDOR_STATUS_IDLE,
    VENDOR_STATUS_BUSY,
    VENDOR_STATUS_ERROR
} VendorStatus_t;

/* execute() icin ortak komutlar. Tum vendor'larin anlamasi beklenir
 * (ModuleMgr_ExecuteAll ile hepsine birden gonderilebilir). */
#define MM_CMD_START         (0x0001u)
#define MM_CMD_STOP          (0x0002u)
#define MM_CMD_RESET         (0x0003u)

/* ---- Fonksiyon pointer tipleri ---------------------------------------- */
/* Her fonksiyon tek parametre alir:
 *   readIfc  : vendor'daki degeri pValue'ya yazar
 *   writeIfc : value'yu vendor'a yazar
 *   execute  : cmd komutunu calistirir (MM_CMD_xxx)
 *   getStatus: vendor'un durumunu doner */
typedef MM_Ret_t       (*fncptr_readIfc)  (uint32_t *pValue);
typedef MM_Ret_t       (*fncptr_writeIfc) (uint32_t value);
typedef MM_Ret_t       (*fncptr_execute)  (uint32_t cmd);
typedef VendorStatus_t (*fncptr_getStatus)(void);

/**
 * Her vendor'un doldurup register ettigi arayuz tablosu.
 * ModuleMgr sadece pointer'ini saklar (kopya almaz); bu yuzden vendor
 * bu yapiyi 'static' (program boyunca yasayan) olarak tutmali.
 */
typedef struct
{
    const char        *pName;      /* Debug/log amacli, opsiyonel */
    fncptr_readIfc     readIfc;
    fncptr_writeIfc    writeIfc;
    fncptr_execute     execute;
    fncptr_getStatus   getStatus;
} VendorIfc_t;

#endif /* MODULE_MGR_TYPES_H */
