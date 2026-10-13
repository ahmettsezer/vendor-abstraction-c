/**
 * @file    main.c
 * @brief   [fleetConfig] Entegrator ornegi: filo yonetimi senaryosu.
 *
 *          Cikti okuma rehberi:
 *            "ModuleMgr_xxx(...)" -> entegratorun yaptigi cagri
 *            "    [Marka] ..."     -> ModuleMgr'in yonlendirdigi vendor fonksiyonu
 *            "  = ..."             -> ModuleMgr'in donus degeri
 *          Altinda vendor satiri OLMAYAN hatalar ModuleMgr'da yakalanmistir
 *          (ornegin otomobile yuk yuklemek: MM_ERR_WRONG_TYPE).
 *
 *          Not: printf/setvbuf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "ModuleMgr.h"
#include "Integrator_Cfg.h"
#include "Toyota.h"
#include "Man.h"
#include "Volvo.h"

/* ======================================================================== */
/*  Yazdirma yardimcilari                                                   */
/* ======================================================================== */

static const char *retStr(MM_Ret_t r)
{
    const char *p = "?";

    switch (r)
    {
        case MM_OK:                p = "OK";                break;
        case MM_ERR_PARAM:         p = "ERR_PARAM";         break;
        case MM_ERR_NOT_INIT:      p = "ERR_NOT_INIT";      break;
        case MM_ERR_CONFIG:        p = "ERR_CONFIG";        break;
        case MM_ERR_NOT_FOUND:     p = "ERR_NOT_FOUND";     break;
        case MM_ERR_WRONG_TYPE:    p = "ERR_WRONG_TYPE";    break;
        case MM_ERR_OVER_CAPACITY: p = "ERR_OVER_CAPACITY"; break;
        case MM_ERR_LOW_FUEL:      p = "ERR_LOW_FUEL";      break;
        default:                                            break;
    }
    return p;
}

static const char *typeStr(VehicleType_t t)
{
    const char *p = "?";

    switch (t)
    {
        case VEHICLE_TYPE_CAR:   p = "OTOMOBIL"; break;
        case VEHICLE_TYPE_TRUCK: p = "TIR";      break;
        default:                                 break;
    }
    return p;
}

static const char *fuelStr(FuelType_t f)
{
    const char *p = "?";

    switch (f)
    {
        case FUEL_PETROL:   p = "Benzin"; break;
        case FUEL_DIESEL:   p = "Dizel";  break;
        case FUEL_HYBRID:   p = "Hibrit"; break;
        case FUEL_ELECTRIC: p = "Elektrik"; break;
        default:                          break;
    }
    return p;
}

static void title(const char *pText)
{
    (void)printf("\n===== %s =====\n", pText);
}

static MM_Ret_t result(MM_Ret_t r)
{
    (void)printf("  = %s\n", retStr(r));
    return r;
}

/* Filo tablosu: ortak bilgiler + tipe ozel bilgi. */
static void printFleet(void)
{
    (void)printf("  ID   TIP       MARKA / MODEL         YIL   YAKIT    KM        DEPO  DURUM  TIPE OZEL\n");

    for (uint8_t i = 0u; i < ModuleMgr_GetVehicleCount(); i++)
    {
        VehicleId_t   id   = ModuleMgr_GetVehicleIdAt(i);
        VehicleType_t type = VEHICLE_TYPE_COUNT;
        VehicleInfo_t info;
        char          name[32];

        if ((ModuleMgr_GetVehicleType(id, &type) == MM_OK) && (ModuleMgr_GetInfo(id, &info) == MM_OK))
        {
            (void)snprintf(name, sizeof(name), "%s %s", info.pBrand, info.pModel);
            (void)printf("  %-4u %-9s %-21s %-5u %-8s %-9lu %%%-4u %-6s ",
                         (unsigned)id, typeStr(type), name, (unsigned)info.modelYear,
                         fuelStr(info.fuelType), (unsigned long)info.odometerKm,
                         (unsigned)info.fuelLevelPct,
                         (info.status == VEHICLE_STATUS_IN_USE) ? "GOREV" : "BOS");

            if (type == VEHICLE_TYPE_CAR)
            {
                uint8_t seats = 0u;
                (void)ModuleMgr_Car_GetSeatCount(id, &seats);
                (void)printf("%u koltuk\n", (unsigned)seats);
            }
            else
            {
                uint32_t load = 0u;
                uint32_t max  = 0u;
                (void)ModuleMgr_Truck_GetLoad(id, &load);
                (void)ModuleMgr_Truck_GetMaxLoad(id, &max);
                (void)printf("yuk %lu / %lu kg\n", (unsigned long)load, (unsigned long)max);
            }
        }
    }
}

static void printFound(VehicleId_t id)
{
    VehicleInfo_t info;

    if (ModuleMgr_GetInfo(id, &info) == MM_OK)
    {
        (void)printf("  -> secilen arac: %u (%s %s)\n", (unsigned)id, info.pBrand, info.pModel);
    }
}

/* ======================================================================== */
/*  Gorevler                                                                */
/* ======================================================================== */

static void passengerJob(uint8_t passengers, uint32_t km)
{
    VehicleId_t id = VEHICLE_ID_INVALID;

    (void)printf("ModuleMgr_FindCarForPassengers(%u)\n", (unsigned)passengers);
    if (result(ModuleMgr_FindCarForPassengers(passengers, &id)) == MM_OK)
    {
        printFound(id);
        (void)printf("ModuleMgr_Car_SetPassengers(%u, %u)\n", (unsigned)id, (unsigned)passengers);
        (void)result(ModuleMgr_Car_SetPassengers(id, passengers));
        (void)printf("ModuleMgr_Drive(%u, %lu)\n", (unsigned)id, (unsigned long)km);
        (void)result(ModuleMgr_Drive(id, km));
    }
}

static void cargoJob(uint32_t kg, uint32_t km)
{
    VehicleId_t id = VEHICLE_ID_INVALID;

    (void)printf("ModuleMgr_FindTruckForLoad(%lu)\n", (unsigned long)kg);
    if (result(ModuleMgr_FindTruckForLoad(kg, &id)) == MM_OK)
    {
        printFound(id);
        (void)printf("ModuleMgr_Truck_LoadCargo(%u, %lu)\n", (unsigned)id, (unsigned long)kg);
        (void)result(ModuleMgr_Truck_LoadCargo(id, kg));
        (void)printf("ModuleMgr_Drive(%u, %lu)\n", (unsigned)id, (unsigned long)km);
        (void)result(ModuleMgr_Drive(id, km));
    }
}

/* ======================================================================== */

int main(void)
{
    (void)setvbuf(stdout, NULL, _IONBF, 0);

    /* 1) Vendor'lar kendi araclarini hazirlar; ModuleMgr filoyu dogrular. */
    title("1) Init");
    (void)printf("Toyota_Init() = %s\n", retStr(Toyota_Init()));
    (void)printf("Man_Init()    = %s\n", retStr(Man_Init()));
    (void)printf("Volvo_Init()  = %s\n", retStr(Volvo_Init()));
    (void)printf("ModuleMgr_Init(&Integrator_FleetCfg) = %s  (tip/arayuz/instance dogrulandi)\n\n",
                 retStr(ModuleMgr_Init(&Integrator_FleetCfg)));
    printFleet();

    /* 2) Tip bazinda filo ozeti: ModuleMgr her araca tipine uygun soruyu sorar. */
    title("2) Filo ozeti (tipe gore)");
    {
        ModuleMgr_FleetSummary_t s;

        if (ModuleMgr_GetFleetSummary(&s) == MM_OK)
        {
            (void)printf("  Otomobil : %u arac, toplam %lu koltuk\n",
                         (unsigned)s.vehicleCount[VEHICLE_TYPE_CAR], (unsigned long)s.totalSeats);
            (void)printf("  Tir      : %u arac, toplam %lu kg yuk kapasitesi\n",
                         (unsigned)s.vehicleCount[VEHICLE_TYPE_TRUCK], (unsigned long)s.totalMaxLoadKg);
            (void)printf("  Filo km  : %lu km\n", (unsigned long)s.totalOdometerKm);
        }
    }

    /* 3) Yolcu gorevleri: yolcu sayisina YETEN EN KUCUK bos otomobil secilir. */
    title("3) Yolcu gorevleri");
    passengerJob(4u, 300u);    /* Corolla (5) yeter, Proace (8) buyuk gorevlere kalir */
    passengerJob(6u, 120u);    /* Corolla yetmez -> Proace                          */

    /* 4) Yuk gorevleri: yuke YETEN EN KUCUK kapasiteli bos tir secilir. */
    title("4) Yuk gorevleri");
    cargoJob(18000u, 450u);    /* MAN (20 t) yeter, Volvo (25 t) buyuk yuklere kalir */
    cargoJob(22000u, 300u);    /* MAN dolu ve yetmez -> Volvo                        */

    /* 5) Kurallar: tip hatasi ModuleMgr'da, kapasite/yakit hatasi vendor'da yakalanir. */
    title("5) Tip ve kapasite kontrolleri");
    (void)printf("ModuleMgr_Truck_LoadCargo(%u, 500)       <- otomobile yuk\n", (unsigned)FLEET_ID_COROLLA);
    (void)result(ModuleMgr_Truck_LoadCargo(FLEET_ID_COROLLA, 500u));
    (void)printf("ModuleMgr_Car_SetPassengers(%u, 2)        <- tira yolcu\n", (unsigned)FLEET_ID_MAN_TGX);
    (void)result(ModuleMgr_Car_SetPassengers(FLEET_ID_MAN_TGX, 2u));
    (void)printf("ModuleMgr_Truck_LoadCargo(%u, 5000)      <- kapasite asimi\n", (unsigned)FLEET_ID_MAN_TGX);
    (void)result(ModuleMgr_Truck_LoadCargo(FLEET_ID_MAN_TGX, 5000u));
    (void)printf("ModuleMgr_Drive(%u, 400)                  <- yakit yetmez\n", (unsigned)FLEET_ID_MAN_TGX);
    (void)result(ModuleMgr_Drive(FLEET_ID_MAN_TGX, 400u));
    (void)printf("ModuleMgr_FindTruckForLoad(30000)         <- uygun tir yok\n");
    {
        VehicleId_t id = VEHICLE_ID_INVALID;
        (void)result(ModuleMgr_FindTruckForLoad(30000u, &id));
    }

    (void)printf("\n");
    printFleet();

    /* 6) Gorevler biter: yolcular iner, yukler bosaltilir. */
    title("6) Gorev sonu");
    (void)printf("ModuleMgr_Car_SetPassengers(%u, 0)\n", (unsigned)FLEET_ID_COROLLA);
    (void)result(ModuleMgr_Car_SetPassengers(FLEET_ID_COROLLA, 0u));
    (void)printf("ModuleMgr_Car_SetPassengers(%u, 0)\n", (unsigned)FLEET_ID_PROACE);
    (void)result(ModuleMgr_Car_SetPassengers(FLEET_ID_PROACE, 0u));
    (void)printf("ModuleMgr_Truck_UnloadCargo(%u)\n", (unsigned)FLEET_ID_MAN_TGX);
    (void)result(ModuleMgr_Truck_UnloadCargo(FLEET_ID_MAN_TGX));
    (void)printf("ModuleMgr_Truck_UnloadCargo(%u)\n", (unsigned)FLEET_ID_VOLVO_FH);
    (void)result(ModuleMgr_Truck_UnloadCargo(FLEET_ID_VOLVO_FH));

    /* 7) Bakim raporu: aralik aracin TIPINE gore (otomobil 15000, tir 30000 km). */
    title("7) Bakim raporu (tipe gore aralik)");
    for (uint8_t i = 0u; i < ModuleMgr_GetVehicleCount(); i++)
    {
        VehicleId_t   id   = ModuleMgr_GetVehicleIdAt(i);
        VehicleType_t type = VEHICLE_TYPE_COUNT;
        VehicleInfo_t info;
        bool          due  = false;
        char          name[32];

        if ((ModuleMgr_GetVehicleType(id, &type) == MM_OK) &&
            (ModuleMgr_GetInfo(id, &info) == MM_OK) &&
            (ModuleMgr_IsServiceDue(id, &due) == MM_OK))
        {
            (void)snprintf(name, sizeof(name), "%s %s", info.pBrand, info.pModel);
            (void)printf("  %u %-8s %-20s bakimdan beri %5lu km / aralik %5lu km  -> %s\n",
                         (unsigned)id, typeStr(type), name,
                         (unsigned long)info.kmSinceService,
                         (unsigned long)Integrator_FleetCfg.serviceIntervalKm[type],
                         due ? "BAKIM GEREKLI" : "ok");
            if (due)
            {
                (void)result(ModuleMgr_Service(id));
            }
        }
    }

    /* 8) Tum filoya yakit. */
    title("8) Tum filoya yakit");
    {
        uint8_t fails = 0u;
        (void)printf("ModuleMgr_RefuelAll()\n");
        (void)result(ModuleMgr_RefuelAll(&fails));
    }

    (void)printf("\n");
    printFleet();

    (void)ModuleMgr_DeInit();
    return 0;
}
