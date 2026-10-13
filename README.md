# vendor-abstraction-c

C dilinde, vendor'lardan bagimsiz bir modul yoneticisi (**ModuleMgr**) ve bu yoneticinin
farkli dagitim (dispatch) ornekleri. Ornekler statik analize uygunluk acisindan
(MISRA C, call graph, stack analizi, formel kanit) karsilastirilmistir.

## Mimari

Uc taraf var:

| Taraf | Rolu |
|---|---|
| **Entegrator** | Projeye hangi vendor'larin dahil edilecegine karar verir |
| **Vendor** | Ortak arayuzu (`read` / `write` / `execute` / `getStatus`) kendine gore implement eder |
| **ModuleMgr** | Vendor'lari tanimadan, `VendorId` uzerinden ilgili vendor'un fonksiyonunu cagirir |

Ornek vendor'lar ayni arayuzu sunar ama farkli davranir:

- **VendorA** kendisine yazilan son degeri saklar.
- **VendorB** kendisine yazilan degerleri toplar.

Tum orneklerde ModuleMgr tek basina derlenip link edilebilir; ornekler arasindaki fark,
ModuleMgr'in bir `VendorId` icin hangi fonksiyonu cagiracagini **nereden ogrendigi**.

## Ornekler

| Klasor | Dagitim yontemi | Fonksiyon pointer | Cagri hedefi derleme aninda belli mi |
|---|---|---|---|
| [`constConfig`](constConfig/) | Register yok; entegratorun `const` tablosu `ModuleMgr_Init(&cfg)` ile verilir | Var | Evet |
| [`switchDispatch`](switchDispatch/) | Entegratorun `switch` dagiticisi, dogrudan cagri | Yok | Evet |

### Statik analiz acisindan

> Calisma zamaninda kayit (`Register`) yapan iki ilk ornek (`runtimeRegister`,
> `runtimeRegisterConst`) kaldirildi. Kayit RAM'deki bir diziye calisma zamaninda
> yapildigi icin analiz araclari `id <-> fonksiyon` eslesmesini kaybediyordu; tablolar
> `const` yapilsa bile kayit dizisi bu bagi koparmaya devam ediyordu. Asagidaki ornekler
> bu eslesmeyi derleme aninda sabitler.

- **constConfig:** Fonksiyon pointer'lari kalir, ama hedefleri ve `id <-> vendor` eslesmesi
  flash'taki `const` tablonun tanimlama satirindan kesin okunur. Konfigurasyon Init
  parametresi oldugu icin ModuleMgr'in `extern` bagimliligi yoktur
  (kural: `ModuleMgr_Init` tek yerden, bir kez cagrilir). **Onerilen ornek.**
- **switchDispatch:** Fonksiyon pointer'i yoktur; call graph tamamen statiktir ve tekrarlanan
  ID derleme hatasi verir. Yeni bir vendor eklemek icin tum `switch`'lere `case` eklemek
  gerekir (pratikte kod uretici ile kullanilir). ModuleMgr, entegratorun tanimladigi
  dagitici fonksiyonlara link seviyesinde baglidir.

### fleetConfig: gercek hayata yakin ornek

[`fleetConfig`](fleetConfig/), `constConfig` mimarisini bir filo yonetimi uzerinden gosterir.
Isimler alana gore secilmistir: yonetici `fleetMgr/` (`FleetMgr_*`, donus kodlari `FM_*`),
araclar `vehicles/` (Toyota, Ford, MAN, Volvo), arac kimlikleri `VEHICLE_ID_*`.
Iki arac tipi vardir ve her tipin **tek bir arayuzu** vardir. Tum araclarda ortak olan
islemler her iki arayuzde de ayni isim ve imzayla, dogrudan alan olarak bulunur:

| Arayuz | Kim implement eder | Ortak alanlar | Tipe ozel alanlar |
|---|---|---|---|
| `CarIfc_t` | Toyota Corolla, Ford Tourneo Custom | `init`, `getInfo`, `drive`, `refuel`, `doService` | `getSeatCount`, `setPassengers` |
| `TruckIfc_t` | MAN TGX, Volvo FH | `init`, `getInfo`, `drive`, `refuel`, `doService` | `getMaxLoadKg`, `getLoadKg`, `loadCargo`, `unloadCargo` |

Entegratorun tablosunda her arac icin `{ id, type, &arayuz }` yazilir. Arayuz
`const void *pVehicle` olarak saklanir; FleetMgr `type`'a bakip kullanildigi yerde
ilgili arayuz tipine cast eder (MISRA 11.5 sapmasi; her cast tip kontrolunden sonra). `type` ile
arayuzun uyumunu **entegrator** garanti etmelidir: `void*` tip bilgisi tasimadigi icin
FleetMgr bu uyumu dogrulayamaz. Her arac modulu (`vehicles/` altindaki her marka) tek bir araci temsil eder; bir arayuz tablosu iki
satirda kullanilamaz. FleetMgr:

- Init'te tipe gore tum fonksiyon pointer'larini, NULL arayuzu, tekrarlanan ID'yi ve ayni
  arayuzun iki kez verilmesini dogrular (tip ile arayuz uyumunu dogrulayamaz); ardindan her aracin `init`'ini **kendisi** cagirir.
  Bir arac init'i hata donerse FleetMgr init edilmemis kalir (ya hep ya hic).
- Entegratorun `main`'i markalari tanimaz; sadece `FleetMgr_Init(&cfg)` cagirir. Markalari
  bilen tek dosya konfigurasyon tablosudur (`Integrator_Cfg.c`).
- Ortak islemleri tipe gore dogru arayuzden cagirir (`vehicleDrive` gibi yardimcilarla).
- Tipe uymayan islemi (otomobile yuk) araca gitmeden `FM_ERR_WRONG_TYPE` ile reddeder.
- Tipe gore yonetir: bakim araligi tipe gore (otomobil 15.000, tir 30.000 km), yolcuya yeten
  en kucuk otomobili ve yuke yeten en kucuk tiri bulur, tip bazinda filo ozeti cikarir.

### fleetConfigA ve fleetConfigB: fleetConfig'in `void*` yerine tipli alternatifleri

`fleetConfig` konfigurasyon satirinda `const void *pVehicle` kullanir; tip ile arayuz
uyumunu derleyici ve Init denetleyemez. A ve B ayni araclari, arayuzleri, API'yi ve
`main.c`'yi kullanir (cikti birebir ayni); sadece `FleetMgr.h`, `FleetMgr.c` ve
`Integrator_Cfg.c` farklidir. Islemler bilincli olarak sade tutuldu: ortak `init`,
`getInfo`, `drive`; otomobilde `setPassengers`, tirda `loadCargo`.

**A - Tipli alanlar** (`fleetConfigA/`): tek filo tablosu, tipe uymayan alan `NULL`.
```c
{ VEHICLE_ID_TOYOTA_COROLLA, VEHICLE_TYPE_CAR,   &Toyota_CarIfc, NULL          },
{ VEHICLE_ID_MAN_TGX,        VEHICLE_TYPE_TRUCK, NULL,           &Man_TruckIfc },
```

**B - Her tip icin ayri tablo** (`fleetConfigB/`): tip alani yok; tip, hangi tabloda oldugundan anlasilir.
```c
static const FleetMgr_CarCfg_t   s_cars[]   = { { VEHICLE_ID_TOYOTA_COROLLA, &Toyota_CarIfc }, ... };
static const FleetMgr_TruckCfg_t s_trucks[] = { { VEHICLE_ID_MAN_TGX,        &Man_TruckIfc  }, ... };
```

| | A | B |
|---|---|---|
| Yanlis yere tir arayuzu | Derleyici | Derleyici |
| Tip ile dolu alan uyumsuz | Init yakalar | Yazilamaz |
| Tip alani / NULL alan | Var / satir basina 1 | Yok / yok |
| Tip `switch`'i ve guvenli varsayilan arayuz | Var (`carOf`/`truckOf`) | Gerekmez |
| Tipe ozel islemde arama | Tum filo | Sadece o tipin tablosu |
| ID tekilligi | Tek tablo icinde | Iki tablo arasinda da kontrol edilir |
| `void*` / cast | Yok | Yok |

### fleetConfigIndexed: fleetConfig + arac ID'si = tablo index'i

`fleetConfig` ile ayni (ayni `const void *pVehicle`, ayni fonksiyonlar ve senaryo); tek fark:
FleetMgr ID'yi tabloda **aramaz**. ID'ler entegratorde ardisik bir `typedef enum`, tablo bu
enum ile index'lenir:

```c
typedef enum { NO_VEHICLE_ID = VEHICLE_ID_NONE, VEHICLE_ID_TOYOTA_COROLLA, ..., VEHICLE_ID_COUNT } FleetVehicleId_t;

static const FleetMgr_VehicleCfg_t s_fleet[VEHICLE_ID_COUNT] = {
    [NO_VEHICLE_ID]             = { VEHICLE_TYPE_NONE, NULL           },
    [VEHICLE_ID_TOYOTA_COROLLA] = { VEHICLE_TYPE_CAR,  &Toyota_CarIfc },
    ...
};
```

- Erisim tek sinir kontrolu: `id == NONE || id >= count` ise `FM_ERR_NOT_FOUND`, degilse `tablo[id]`.
- `tablo[0]` = "arac yok" (defansif): sifir kalmis bir ID ilk gercek araci secmez.
- Satirda `id` alani yok; ayni ID iki kez yazilamaz. Unutulan bir satir sifirla dolar
  (`VEHICLE_TYPE_NONE`) ve Init reddeder.
- `void*` kaynakli zayiflik `fleetConfig` ile aynidir: tip ile arayuz uyumsuzlugu yakalanmaz.
- Bedel: ID'ler ardisik ve konumsaldir; dis numaralar (plaka, filo no) gerekiyorsa veri
  olarak tutulup sistem girisinde bir kez ID'ye cevrilir.

`constConfig`, `switchDispatch` ve fleet orneklerinde ek olarak su MISRA C:2012 kurallarina
uyulmustur: 15.5 (tek cikis noktasi), 16.3 (`switch` case'leri `break` ile biter), 17.7 (donus
degerleri kullanilir veya `(void)` ile yok sayilir). Demo amacli `printf` kullanimi Rule 21.6
sapmasidir.

## Dokuman

- [`docs/fleet-konfigurasyonlari.html`](docs/fleet-konfigurasyonlari.html): fleetConfig, fleetConfigA,
  fleetConfigB ve fleetConfigIndexed karsilastirmasi; hangi hatanin nerede yakalandigi, statik
  analiz durumu ve oneri.
- [`docs/mimari-notlari.html`](docs/mimari-notlari.html): ilk orneklerin (runtime kayit, constConfig,
  switchDispatch) karsilastirmasi. Not: runtime kayit ornekleri repodan kaldirildi.

Tarayicida acin; telefona uygundur.

## Derleme ve calistirma

Gereken: GCC (C99). Gelistirme MinGW GCC 6.3 ile yapildi.

```sh
cd constConfig
gcc -std=c99 -Wall -Wextra -Wpedantic -ImoduleMgr -Ivendors/vendorA -Ivendors/vendorB -Iintegrator \
    moduleMgr/ModuleMgr.c vendors/vendorA/VendorA.c vendors/vendorB/VendorB.c \
    integrator/Integrator_Cfg.c integrator/main.c -o demo
./demo
```

Not: `switchDispatch` klasorunde `integrator/Integrator_Cfg.c` yerine
`integrator/ModuleMgr_Cfg.c` derlenir ve `-Iintegrator` parametresi gerekmez.

Windows notu: Exe adinda `patch`, `setup`, `install` veya `update` gecerse Windows onu kurulum
programi sanip yonetici izni ister. Bu yuzden cikti adlari `demo_xxx.exe` seklindedir
(`switchDispatch.exe` yerine `demo_switch.exe`).

### VS Code

- **Ctrl+Shift+B:** tum ornekleri birden derler (`build all`).
- **F5:** her ornek icin ayri bir gdb debug konfigurasyonu vardir.
- **IntelliSense:** tum orneklerde ayni isimli header'lar bulundugu icin her ornegin ayri
  bir konfigurasyonu vardir; calistigin ornegi durum cubugundan secmelisin.
