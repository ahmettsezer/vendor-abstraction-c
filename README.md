# vendor-abstraction-c

C dilinde, vendor'lardan bagimsiz bir modul yoneticisi (**ModuleMgr**) ve bu yoneticinin
**dort farkli dagitim (dispatch) ornegi** ile bunlardan birine dayanan gercekci bir filo yonetimi ornegi. Ornekler statik analize uygunluk acisindan
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
| [`runtimeRegister`](runtimeRegister/) | Vendor kendini runtime'da `Register` eder; tablolar RAM'de | Var | Hayir |
| [`runtimeRegisterConst`](runtimeRegisterConst/) | `runtimeRegister` ile ayni, ama tablolar `static const` (flash) ve ModuleMgr `const` pointer tutar | Var | Kume olarak (A veya B) |
| [`constConfig`](constConfig/) | Register yok; entegratorun `const` tablosu `ModuleMgr_Init(&cfg)` ile verilir | Var | Evet |
| [`switchDispatch`](switchDispatch/) | Entegratorun `switch` dagiticisi, dogrudan cagri | Yok | Evet |

### Statik analiz acisindan

- **runtimeRegister:** Cagri zinciri runtime'da, RAM'deki bir kayit dizisi uzerinden kurulur.
  Araclar `id <-> fonksiyon` eslesmesini kaybeder; call graph ve stack analizi icin elle
  annotation gerekir. Tablolar RAM'de oldugu icin bozulabilir.
- **runtimeRegisterConst:** Tablolarin ici (hangi alan hangi fonksiyonu gosteriyor) derleme
  aninda kesin ve flash'ta; ModuleMgr tabloyu degistiremez (MISRA 8.13). Ama kayit dizisi
  hala runtime'da doldugu icin `id <-> tablo` eslesmesi analizde kaybolur.
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

### payloadConfig: yuk istasyonlari (kokteki `Payload` taslaginin bolunmus hali)

| Klasor | Icerik |
|---|---|
| `platform/` | `payload.h`: sozlesme, hicbir yuk tipini bilmez |
| `payloads/` | `tank/`, `pod/`: yuk tipleri (vendor'lar), istasyon basina kendi durumlari |
| `integrator/` | `integration.c`: `const OPS[PL_COUNT]` tablosu, `payload_bind`, `row_of`, dagitim; `main.c` |
| `inventoryMgr/` | Yuk degisikligi isteklerini toplar, `payload_bind`'i sadece frame sinirinda cagirir (ornek olarak eklendi) |
| `consumers/` | `weight_balance.c`: sadece sozlesmeyi bilen tuketici |

Calisma zamaninda degisen tek sey `type_of[]` icindeki kucuk bir **index**; fonksiyon
pointer'lari ROM'daki `const` tabloda. Olasi cagri hedefleri kapali bir kume, `row_of`
her zaman gecerli bir satir dondurur (gecersiz deger -> `PL_UNKNOWN`), cast ve NULL yok.

### fleetIndexConfig: fleetConfig + payload birlesimi

fleetConfig'teki tipe ozel arayuzler korunur; `void*` yerine payload'daki gibi
**tipe gore ayri, tipli `const` tablolar ve index** kullanilir:

```c
static const CarIfc_t   * const s_cars[]   = { [CAR_TOYOTA_COROLLA] = &Toyota_CarIfc, ... };
static const TruckIfc_t * const s_trucks[] = { [TRUCK_MAN_TGX] = &Man_TruckIfc, ... };
{ VEHICLE_ID_TOYOTA_COROLLA, VEHICLE_TYPE_CAR,   CAR_TOYOTA_COROLLA },   /* { id, type, index } */
```

- Otomobil tablosuna tir arayuzu yazmak derleme uyarisi/hatasidir; FleetMgr'da cast ve `void*` yoktur.
- `carOf()` / `truckOf()` payload'daki `row_of` gibi **her zaman gecerli** bir arayuz doner:
  tip uymazsa her islemi `FM_ERR_WRONG_TYPE` olan guvenli varsayilan arayuz gelir.
- Init: index tipin tablosunda mi, arayuz pointer'lari dolu mu, ayni ID / ayni tablo satiri iki kez mi.
- Bilincli olarak sade: ortak `init`, `getInfo`, `drive`; otomobilde `setPassengers`, tirda `loadCargo`.

`constConfig`, `switchDispatch` ve `fleetConfig` orneklerinde ek olarak su MISRA C:2012 kurallarina
uyulmustur: 15.5 (tek cikis noktasi), 16.3 (`switch` case'leri `break` ile biter), 17.7 (donus
degerleri kullanilir veya `(void)` ile yok sayilir). Demo amacli `printf` kullanimi Rule 21.6
sapmasidir.

## Dokuman

Orneklerin yapisi, kullanimi, artilari, eksileri ve statik analiz degerlendirmesi:
[`docs/mimari-notlari.html`](docs/mimari-notlari.html) (tarayicida acin; telefona uygun).

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
