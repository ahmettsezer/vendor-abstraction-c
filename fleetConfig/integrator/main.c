/**
 * @file    main.c
 * @brief   [fleetConfig] Entegrator ornegi: filo yonetimi senaryosu.
 *
 *          Cikti okuma rehberi:
 *            "FleetMgr_xxx(...)" -> entegratorun yaptigi cagri
 *            "    [Marka] ..."     -> FleetMgr'in yonlendirdigi arac fonksiyonu
 *            "  = ..."             -> FleetMgr'in donus degeri
 *          Entegratorun main'i markalari TANIMAZ: arac header'i include
 *          edilmez, arac init'leri FleetMgr_Init icinden cagrilir.
 *          Markalari bilen tek dosya Integrator_Cfg.c'dir.
 *
 *          Altinda arac satiri OLMAYAN hatalar FleetMgr'da yakalanmistir
 *          (ornegin otomobile yuk yuklemek: FM_ERR_WRONG_TYPE).
 *
 *          Not: printf/setvbuf yalnizca demo icindir (MISRA 21.6 sapmasi).
 */
#include <stdio.h>
#include "FleetMgr.h"
#include "Integrator_Cfg.h"

/* ======================================================================== */
/*  Yazdirma yardimcilari                                                   */
/* ======================================================================== */

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

static FM_Ret_t result(FM_Ret_t r)
{
    (void)printf("  = %s\n", retStr(r));
    return r;
}

/* Filo tablosu: ortak bilgiler + tipe ozel bilgi. */
static void printFleet(void)
{
    (void)printf("  ID   TIP       MARKA / MODEL         YIL   YAKIT    KM        DEPO  DURUM  TIPE OZEL\n");

    for (uint8_t i = 0u; i < FleetMgr_GetVehicleCount(); i++)
    {
        VehicleId_t   id   = FleetMgr_GetVehicleIdAt(i);
        VehicleType_t type = VEHICLE_TYPE_COUNT;
        VehicleInfo_t info;
        char          name[32];

        if ((FleetMgr_GetVehicleType(id, &type) == FM_OK) && (FleetMgr_GetInfo(id, &info) == FM_OK))
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
                (void)FleetMgr_Car_GetSeatCount(id, &seats);
                (void)printf("%u koltuk\n", (unsigned)seats);
            }
            else
            {
                uint32_t load = 0u;
                uint32_t max  = 0u;
                (void)FleetMgr_Truck_GetLoad(id, &load);
                (void)FleetMgr_Truck_GetMaxLoad(id, &max);
                (void)printf("yuk %lu / %lu kg\n", (unsigned long)load, (unsigned long)max);
            }
        }
    }
}

static void printFound(VehicleId_t id)
{
    VehicleInfo_t info;

    if (FleetMgr_GetInfo(id, &info) == FM_OK)
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

    (void)printf("FleetMgr_FindCarForPassengers(%u)\n", (unsigned)passengers);
    if (result(FleetMgr_FindCarForPassengers(passengers, &id)) == FM_OK)
    {
        printFound(id);
        (void)printf("FleetMgr_Car_SetPassengers(%u, %u)\n", (unsigned)id, (unsigned)passengers);
        (void)result(FleetMgr_Car_SetPassengers(id, passengers));
        (void)printf("FleetMgr_Drive(%u, %lu)\n", (unsigned)id, (unsigned long)km);
        (void)result(FleetMgr_Drive(id, km));
    }
}

static void cargoJob(uint32_t kg, uint32_t km)
{
    VehicleId_t id = VEHICLE_ID_INVALID;

    (void)printf("FleetMgr_FindTruckForLoad(%lu)\n", (unsigned long)kg);
    if (result(FleetMgr_FindTruckForLoad(kg, &id)) == FM_OK)
    {
        printFound(id);
        (void)printf("FleetMgr_Truck_LoadCargo(%u, %lu)\n", (unsigned)id, (unsigned long)kg);
        (void)result(FleetMgr_Truck_LoadCargo(id, kg));
        (void)printf("FleetMgr_Drive(%u, %lu)\n", (unsigned)id, (unsigned long)km);
        (void)result(FleetMgr_Drive(id, km));
    }
}

/* ======================================================================== */

int main(void)
{
    (void)setvbuf(stdout, NULL, _IONBF, 0);

    /* 1) Entegrator SADECE FleetMgr'i init eder. FleetMgr konfigurasyonu
     *    dogrular ve her aracin init'ini arayuzu uzerinden kendisi cagirir. */
    title("1) Init");
    (void)printf("FleetMgr_Init(&Integrator_FleetCfg)\n");
    (void)result(FleetMgr_Init(&Integrator_FleetCfg));
    (void)printf("\n");
    printFleet();

    /* 2) Tip bazinda filo ozeti: FleetMgr her araca tipine uygun soruyu sorar. */
    title("2) Filo ozeti (tipe gore)");
    {
        FleetMgr_Summary_t s;

        if (FleetMgr_GetFleetSummary(&s) == FM_OK)
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
    passengerJob(4u, 300u);    /* Corolla (5) yeter, Tourneo (8) buyuk gorevlere kalir */
    passengerJob(6u, 120u);    /* Corolla yetmez -> Tourneo                          */

    /* 4) Yuk gorevleri: yuke YETEN EN KUCUK kapasiteli bos tir secilir. */
    title("4) Yuk gorevleri");
    cargoJob(18000u, 450u);    /* MAN (20 t) yeter, Volvo (25 t) buyuk yuklere kalir */
    cargoJob(22000u, 300u);    /* MAN dolu ve yetmez -> Volvo                        */

    /* 5) Kurallar: tip hatasi FleetMgr'da, kapasite/yakit hatasi aracin kendisinde yakalanir. */
    title("5) Tip ve kapasite kontrolleri");
    (void)printf("FleetMgr_Truck_LoadCargo(%u, 500)       <- otomobile yuk\n", (unsigned)VEHICLE_ID_TOYOTA_COROLLA);
    (void)result(FleetMgr_Truck_LoadCargo(VEHICLE_ID_TOYOTA_COROLLA, 500u));
    (void)printf("FleetMgr_Car_SetPassengers(%u, 2)        <- tira yolcu\n", (unsigned)VEHICLE_ID_MAN_TGX);
    (void)result(FleetMgr_Car_SetPassengers(VEHICLE_ID_MAN_TGX, 2u));
    (void)printf("FleetMgr_Truck_LoadCargo(%u, 5000)      <- kapasite asimi\n", (unsigned)VEHICLE_ID_MAN_TGX);
    (void)result(FleetMgr_Truck_LoadCargo(VEHICLE_ID_MAN_TGX, 5000u));
    (void)printf("FleetMgr_Drive(%u, 400)                  <- yakit yetmez\n", (unsigned)VEHICLE_ID_MAN_TGX);
    (void)result(FleetMgr_Drive(VEHICLE_ID_MAN_TGX, 400u));
    (void)printf("FleetMgr_FindTruckForLoad(30000)         <- uygun tir yok\n");
    {
        VehicleId_t id = VEHICLE_ID_INVALID;
        (void)result(FleetMgr_FindTruckForLoad(30000u, &id));
    }

    (void)printf("\n");
    printFleet();

    /* 6) Gorevler biter: yolcular iner, yukler bosaltilir. */
    title("6) Gorev sonu");
    (void)printf("FleetMgr_Car_SetPassengers(%u, 0)\n", (unsigned)VEHICLE_ID_TOYOTA_COROLLA);
    (void)result(FleetMgr_Car_SetPassengers(VEHICLE_ID_TOYOTA_COROLLA, 0u));
    (void)printf("FleetMgr_Car_SetPassengers(%u, 0)\n", (unsigned)VEHICLE_ID_FORD_TOURNEO);
    (void)result(FleetMgr_Car_SetPassengers(VEHICLE_ID_FORD_TOURNEO, 0u));
    (void)printf("FleetMgr_Truck_UnloadCargo(%u)\n", (unsigned)VEHICLE_ID_MAN_TGX);
    (void)result(FleetMgr_Truck_UnloadCargo(VEHICLE_ID_MAN_TGX));
    (void)printf("FleetMgr_Truck_UnloadCargo(%u)\n", (unsigned)VEHICLE_ID_VOLVO_FH);
    (void)result(FleetMgr_Truck_UnloadCargo(VEHICLE_ID_VOLVO_FH));

    /* 7) Bakim raporu: aralik aracin TIPINE gore (otomobil 15000, tir 30000 km). */
    title("7) Bakim raporu (tipe gore aralik)");
    for (uint8_t i = 0u; i < FleetMgr_GetVehicleCount(); i++)
    {
        VehicleId_t   id   = FleetMgr_GetVehicleIdAt(i);
        VehicleType_t type = VEHICLE_TYPE_COUNT;
        VehicleInfo_t info;
        bool          due  = false;
        char          name[32];

        if ((FleetMgr_GetVehicleType(id, &type) == FM_OK) &&
            (FleetMgr_GetInfo(id, &info) == FM_OK) &&
            (FleetMgr_IsServiceDue(id, &due) == FM_OK))
        {
            (void)snprintf(name, sizeof(name), "%s %s", info.pBrand, info.pModel);
            (void)printf("  %u %-8s %-20s bakimdan beri %5lu km / aralik %5lu km  -> %s\n",
                         (unsigned)id, typeStr(type), name,
                         (unsigned long)info.kmSinceService,
                         (unsigned long)Integrator_FleetCfg.serviceIntervalKm[type],
                         due ? "BAKIM GEREKLI" : "ok");
            if (due)
            {
                (void)result(FleetMgr_Service(id));
            }
        }
    }

    /* 8) Tum filoya yakit. */
    title("8) Tum filoya yakit");
    {
        uint8_t fails = 0u;
        (void)printf("FleetMgr_RefuelAll()\n");
        (void)result(FleetMgr_RefuelAll(&fails));
    }

    (void)printf("\n");
    printFleet();

    (void)FleetMgr_DeInit();
    return 0;
}
