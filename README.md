# vendor-abstraction-c

C dilinde, vendor'lardan bagimsiz bir modul yoneticisi (**ModuleMgr**) ve bu yoneticinin
**dort farkli dagitim (dispatch) ornegi**. Ornekler statik analize uygunluk acisindan
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

Dort ornekte de ModuleMgr tek basina derlenip link edilebilir; ornekler arasindaki fark,
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

`constConfig` ve `switchDispatch` orneklerinde ek olarak su MISRA C:2012 kurallarina
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
