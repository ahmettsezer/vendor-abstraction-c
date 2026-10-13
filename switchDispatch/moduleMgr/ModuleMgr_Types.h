/**
 * @file    ModuleMgr_Types.h
 * @brief   [switchDispatch] Ortak tipler.
 *
 *          Calisma zamaninda kayit (Register) yapan bir tasarima gore farki: fonksiyon pointer typedef'leri ve
 *          VendorIfc_t YOK. Vendor'lar ayni imzada PUBLIC fonksiyonlar sunar,
 *          entegratorun dagitici (switch) kodu onlari dogrudan cagirir.
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
    MM_ERR_NOT_FOUND,       /* VendorId konfigurasyonda yok             */
    MM_ERR_NOT_SUPPORTED,   /* Vendor ilgili fonksiyonu sunmuyor        */
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

#endif /* MODULE_MGR_TYPES_H */
