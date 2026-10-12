/**
 * @file    VendorB.h
 * @brief   [Variant A] Ornek vendor B: kendisine yazilan degerleri TOPLAR.
 */
#ifndef VENDOR_B_H
#define VENDOR_B_H

#include "ModuleMgr_Types.h"

#define VENDOR_B_ID     ((VendorId_t)0x0B01u)

/* Derleme aninda dolu, flash'ta duran arayuz tablosu. */
extern const VendorIfc_t VendorB_Ifc;

/* Sadece vendor'un kendi durumunu hazirlar; register YAPMAZ. */
MM_Ret_t VendorB_Init(void);

#endif /* VENDOR_B_H */
