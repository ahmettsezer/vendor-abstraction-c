/**
 * @file    Integrator_Cfg.h
 * @brief   [fleetConfigIndexed] Entegratorun filo konfigurasyonu.
 *
 *          Bu header'i SADECE entegrator kullanir; FleetMgr bilmez.
 *          Konfigurasyon FleetMgr_Init(&Integrator_FleetCfg) ile verilir.
 *
 *          Arac ID'leri ardisik bir enum: ID = filo tablosundaki satir.
 *          0 = NO_VEHICLE_ID ("arac yok", defansif). Yeni arac:
 *          VEHICLE_ID_COUNT'tan once bir isim + Integrator_Cfg.c'de bir satir.
 */
#ifndef INTEGRATOR_CFG_H
#define INTEGRATOR_CFG_H

#include "FleetMgr.h"

typedef enum
{
    NO_VEHICLE_ID = VEHICLE_ID_NONE,   /* 0: tablo[0], "arac yok"       */
    VEHICLE_ID_TOYOTA_COROLLA,         /* 1                              */
    VEHICLE_ID_FORD_TOURNEO,           /* 2                              */
    VEHICLE_ID_MAN_TGX,                /* 3                              */
    VEHICLE_ID_VOLVO_FH,               /* 4                              */
    VEHICLE_ID_COUNT                   /* tablo boyutu (0. satir dahil)  */
} FleetVehicleId_t;

extern const FleetMgr_Config_t Integrator_FleetCfg;

#endif /* INTEGRATOR_CFG_H */
