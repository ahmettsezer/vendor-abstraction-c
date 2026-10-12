/**
 * @file    VendorA.h
 * @brief   [constConfig] Ornek vendor A: kendisine yazilan SON degeri saklar.
 *
 *          runtimeRegister'dan farki: register YOK; arayuz tablosu 'extern const' olarak
 *          disari acilir; entegrator bunu ModuleMgr konfigurasyonuna yazar
 *          ve konfigurasyonu ModuleMgr_Init(&cfg) ile verir.
 */
#ifndef VENDOR_A_H
#define VENDOR_A_H

#include "ModuleMgr_Types.h"

#define VENDOR_A_ID     ((VendorId_t)0x0A01u)

/* Derleme aninda dolu, flash'ta duran arayuz tablosu. */
extern const VendorIfc_t VendorA_Ifc;

/* Sadece vendor'un kendi durumunu hazirlar; register YAPMAZ. */
MM_Ret_t VendorA_Init(void);

#endif /* VENDOR_A_H */
