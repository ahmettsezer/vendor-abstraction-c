/**
 * @file    main.c
 * @brief   [fleetConfigA] Kisa senaryo: init, listele, her tipten bir
 *          islem, hata ornekleri.
 *
 *          "    [Marka] ..." satiri cagrinin araca ulastigini gosterir.
 *          Altinda arac satiri OLMAYAN hatalar FleetMgr'da kalmistir.
 *
 *          Not: printf/setvbuf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "FleetMgr.h"
#include "Integrator_Cfg.h"

static const char *retStr(FM_Ret_t r)
{
    const char *p = "?";

    switch (r)
    {
        case FM_OK:                p = "OK";                break;
        case FM_ERR_PARAM:         p = "ERR_PARAM";         break;
        case FM_ERR_NOT_INIT:      p = "ERR_NOT_INIT";      break;
        case FM_ERR_CONFIG:        p = "ERR_CONFIG";        break;
        case FM_ERR_NOT_FOUND:     p = "ERR_NOT_FOUND";     break;
        case FM_ERR_WRONG_TYPE:    p = "ERR_WRONG_TYPE";    break;
        case FM_ERR_OVER_CAPACITY: p = "ERR_OVER_CAPACITY"; break;
        case FM_ERR_LOW_FUEL:      p = "ERR_LOW_FUEL";      break;
        default:                                            break;
    }
    return p;
}

static void result(FM_Ret_t r)
{
    (void)printf("  = %s\n", retStr(r));
}

static void printFleet(void)
{
    for (uint8_t i = 0u; i < FleetMgr_GetVehicleCount(); i++)
    {
        VehicleId_t   id   = FleetMgr_GetVehicleIdAt(i);
        VehicleType_t type = VEHICLE_TYPE_COUNT;
        VehicleInfo_t info;

        if (FleetMgr_GetType(id, &type) == FM_OK)
        {
            if (FleetMgr_GetInfo(id, &info) == FM_OK)
            {
                (void)printf("  %u %-8s %-6s %-15s km %-7lu depo %%%-3u %s\n",
                             (unsigned)id, (type == VEHICLE_TYPE_CAR) ? "OTOMOBIL" : "TIR",
                             info.pBrand, info.pModel, (unsigned long)info.odometerKm,
                             (unsigned)info.fuelLevelPct,
                             (info.status == VEHICLE_STATUS_IN_USE) ? "GOREV" : "BOS");
            }
        }
    }
}

int main(void)
{
    (void)setvbuf(stdout, NULL, _IONBF, 0);

    (void)printf("\n===== 1) Init =====\n");
    (void)printf("FleetMgr_Init(&Integrator_FleetCfg)\n");
    result(FleetMgr_Init(&Integrator_FleetCfg));
    printFleet();

    (void)printf("\n===== 2) Her tipten bir islem =====\n");
    (void)printf("FleetMgr_Car_SetPassengers(101, 4)\n");
    result(FleetMgr_Car_SetPassengers(VEHICLE_ID_TOYOTA_COROLLA, 4u));
    (void)printf("FleetMgr_Drive(101, 300)\n");
    result(FleetMgr_Drive(VEHICLE_ID_TOYOTA_COROLLA, 300u));
    (void)printf("FleetMgr_Truck_LoadCargo(201, 18000)\n");
    result(FleetMgr_Truck_LoadCargo(VEHICLE_ID_MAN_TGX, 18000u));
    (void)printf("FleetMgr_Drive(201, 450)\n");
    result(FleetMgr_Drive(VEHICLE_ID_MAN_TGX, 450u));

    (void)printf("\n===== 3) Hatalar =====\n");
    (void)printf("FleetMgr_Truck_LoadCargo(101, 500)   <- otomobile yuk\n");
    result(FleetMgr_Truck_LoadCargo(VEHICLE_ID_TOYOTA_COROLLA, 500u));
    (void)printf("FleetMgr_Car_SetPassengers(201, 2)   <- tira yolcu\n");
    result(FleetMgr_Car_SetPassengers(VEHICLE_ID_MAN_TGX, 2u));
    (void)printf("FleetMgr_Car_SetPassengers(101, 9)   <- koltuk yetmez\n");
    result(FleetMgr_Car_SetPassengers(VEHICLE_ID_TOYOTA_COROLLA, 9u));
    (void)printf("FleetMgr_Drive(999, 10)              <- filoda yok\n");
    result(FleetMgr_Drive((VehicleId_t)999u, 10u));

    (void)printf("\n===== 4) Son durum =====\n");
    printFleet();

    (void)FleetMgr_DeInit();
    return 0;
}
