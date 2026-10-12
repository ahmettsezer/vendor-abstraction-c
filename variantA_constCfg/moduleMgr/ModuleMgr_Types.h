/**
 * @file    ModuleMgr_Types.h
 * @brief   [Variant A] ModuleMgr <-> Vendor sozlesmesi (contract).
 *
 *          Mevcut versiyondan farki: VendorIfc_t her vendor tarafindan
 *          'const' olarak ve tanimlandigi satirda doldurulur (flash'ta durur).
 */
#ifndef MODULE_MGR_TYPES_H
#define MODULE_MGR_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef uint16_t VendorId_t;

#define VENDOR_ID_INVALID   ((VendorId_t)0xFFFFu)

typedef enum
{
    MM_OK = 0,
    MM_ERR_PARAM,           /* Gecersiz parametre / NULL pointer        */
    MM_ERR_NOT_INIT,        /* ModuleMgr init edilmemis                 */
    MM_ERR_CONFIG,          /* Konfigurasyon tablosu hatali             */
    MM_ERR_NOT_FOUND,       /* VendorId konfigurasyonda yok             */
    MM_ERR_NOT_SUPPORTED,   /* Vendor ilgili fonksiyonu sunmuyor (NULL) */
    MM_ERR_VENDOR           /* Vendor kendi icinde hata dondu           */
} MM_Ret_t;

typedef enum
{
    VENDOR_STATUS_UNKNOWN = 0,
    VENDOR_STATUS_IDLE,
    VENDOR_STATUS_BUSY,
    VENDOR_STATUS_ERROR
} VendorStatus_t;

#define MM_CMD_START         (0x0001u)
#define MM_CMD_STOP          (0x0002u)
#define MM_CMD_RESET         (0x0003u)

typedef MM_Ret_t       (*fncptr_readIfc)  (uint32_t *pValue);
typedef MM_Ret_t       (*fncptr_writeIfc) (uint32_t value);
typedef MM_Ret_t       (*fncptr_execute)  (uint32_t cmd);
typedef VendorStatus_t (*fncptr_getStatus)(void);

typedef struct
{
    const char        *pName;
    fncptr_readIfc     readIfc;
    fncptr_writeIfc    writeIfc;
    fncptr_execute     execute;
    fncptr_getStatus   getStatus;
} VendorIfc_t;

#endif /* MODULE_MGR_TYPES_H */
