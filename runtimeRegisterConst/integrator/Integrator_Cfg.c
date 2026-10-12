/**
 * @file    Integrator_Cfg.c
 * @brief   Projeye dahil edilecek vendor'lar SADECE burada listelenir.
 *          Yeni vendor eklemek/cikarmak = bu tabloya satir eklemek/silmek.
 */
#include "Integrator_Cfg.h"
#include "VendorA.h"
#include "VendorB.h"

const VendorInitEntry_t g_vendorInitTable[] =
{
    { "VendorA", VendorA_Init },
    { "VendorB", VendorB_Init },
};

const uint8_t g_vendorInitTableSize =
    (uint8_t)(sizeof(g_vendorInitTable) / sizeof(g_vendorInitTable[0]));
