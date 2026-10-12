# vendor-abstraction-c

C dilinde, vendor'lardan bagimsiz bir modul yoneticisi (**ModuleMgr**) ve bu yoneticinin
**dort farkli dagitim (dispatch) yontemi**. Varyantlar statik analize uygunluk acisindan
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

## Varyantlar

| Klasor | Dagitim yontemi | Fonksiyon pointer | Cagri hedefi derleme aninda belli mi | ModuleMgr'in link bagimliligi |
|---|---|---|---|---|
| [`variant0_runtimeReg`](variant0_runtimeReg/) | Vendor kendini runtime'da `Register` eder | Var | Hayir | Yok |
| [`variantA_constCfg`](variantA_constCfg/) | Entegratorun `const` tablosu, `extern` sembol | Var | Evet | `g_moduleMgrCfg` |
| [`variantA2_cfgParam`](variantA2_cfgParam/) | `const` tablo, `ModuleMgr_Init(&cfg)` parametresi | Var | Evet | Yok |
| [`variantB_switch`](variantB_switch/) | Entegratorun `switch` dagiticisi | Yok | Evet | 5 dagitici fonksiyon |

### Statik analiz acisindan

- **variant0:** Cagri zinciri runtime'da, RAM'deki bir kayit tablosu uzerinden kurulur.
  Araclar `id <-> fonksiyon` eslesmesini kaybeder; call graph ve stack analizi icin
  elle annotation gerekir.
- **variantA / A2:** Fonksiyon pointer'lari kalir, ama hedefleri flash'taki `const`
  tablonun tanimlama satirindan kesin okunur. A2, A'nin link bagimliligini kaldirir
  (kural: `ModuleMgr_Init` tek yerden, bir kez cagrilir).
- **variantB:** Fonksiyon pointer'i yoktur; call graph tamamen statiktir. Yeni bir vendor
  eklemek icin tum `switch`'lere `case` eklemek gerekir (pratikte kod uretici ile kullanilir).

A, A2 ve B'de ek olarak su MISRA C:2012 kurallarina uyulmustur: 15.5 (tek cikis noktasi),
16.3 (`switch` case'leri `break` ile biter), 17.7 (donus degerleri kullanilir veya `(void)`
ile yok sayilir). Demo amacli `printf` kullanimi Rule 21.6 sapmasidir.

## Dokuman

Varyantlarin yapisi, kullanimi, artilari, eksileri ve statik analiz degerlendirmesi:
[`docs/mimari-notlari.html`](docs/mimari-notlari.html) (tarayicida acin; telefona uygun).

## Derleme ve calistirma

Gereken: GCC (C99). Gelistirme MinGW GCC 6.3 ile yapildi.

```sh
cd variantA2_cfgParam
gcc -std=c99 -Wall -Wextra -Wpedantic -ImoduleMgr -Ivendors/vendorA -Ivendors/vendorB -Iintegrator \
    moduleMgr/ModuleMgr.c vendors/vendorA/VendorA.c vendors/vendorB/VendorB.c \
    integrator/Integrator_Cfg.c integrator/main.c -o demo
./demo
```

Not: `variantA_constCfg` ve `variantB_switch` klasorlerinde `integrator/Integrator_Cfg.c`
yerine `integrator/ModuleMgr_Cfg.c` derlenir ve `-Iintegrator` parametresi gerekmez.

### VS Code

- **Ctrl+Shift+B:** dort varyanti birden derler.
- **F5:** her varyant icin ayri bir gdb debug konfigurasyonu vardir.
- **IntelliSense:** dort varyantta da ayni isimli header'lar bulundugu icin her varyantin
  ayri bir konfigurasyonu vardir; calistigin varyanti durum cubugundan secmelisin.
