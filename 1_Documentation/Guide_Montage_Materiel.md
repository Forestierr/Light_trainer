# Guide de Montage & Fabrication Matérielle — Light Trainer V3

Ce guide détaille les étapes de montage, le schéma de câblage et la liste des composants (BOM) pour fabriquer un prototype **Veroboard** ou assembler le **PCB V3**.

---

## 1. Liste des Composants (BOM - Bill of Materials)

Pour fabriquer **1 Pod Light Trainer V3**, vous avez besoin de :

| Réf. Schéma | Désignation | Spécifications | Quantité | Rôle dans le circuit |
| :--- | :--- | :--- | :--- | :--- |
| **`U1`** | **Seeed Studio XIAO ESP32-C3** | Microcontrôleur WiFi/BLE + chargeur LiPo | 1 | Cerveau du pod, gestion RF et I/O |
| **`U2`** | **VL53L0X** | Module télémètre laser ToF (I2C) | 1 | Détection gestuelle sans contact |
| **`D1`..`D7`** | **Anneau LEDs WS2812B** | 7 LEDs RGB adressables 5050 (ou ruban) | 1 | Éclairage, jauges et animations visuelles |
| **`BT1`** | **Batterie LiPo 3.7V** | 1S (ex: 500 mAh à 1200 mAh avec protection) | 1 | Alimentation autonome nomade |
| **`SW1`** | **Interrupteur à glissière SPDT** | 1 pôle, 2 directions (ON/OFF) | 1 | Interrupteur matériel marche/arrêt |
| **`R1` à `R6`** | **Résistances $100\text{ k}\Omega$** | $1/4\text{ W}$, tolérance 1% ou 5% | **6** | Ponts diviseurs et pull-down unifiés |
| **`C1`** | **Condensateur $100\text{ nF}$** | Céramique multicouche (100nF / 50V) | 1 | Filtrage HF de la mesure ADC batterie |
| **`VERO`** | **Plaque d'essai Veroboard** | À bandes cuivrées ou à pastilles | 1 | Support mécanique et électrique |
| **`FILS`** | **Fils de câblage souples** | 28 AWG / 30 AWG (Siliconés recommandés) | Q.S. | Connexions inter-modules |

> [!TIP]
> **Uniformité des résistances** : Toutes les résistances ($R_1$ à $R_6$) ont été unifiées à la valeur standard **$100\text{ k}\Omega$**. Vous n'avez besoin que d'une seule référence de résistance pour l'ensemble du projet !

---

## 2. Schéma Électrique Détaillé

Le schéma complet est disponible au format PDF dans [`1_Documentation/DOC_FAB/schema.pdf`](file:///c:/Users/Robin/Desktop/GitHub/Light_trainer/1_Documentation/DOC_FAB/schema.pdf).

### Vue d'Ensemble des Blocs Fonctionnels :

```
             +-------------------------------------------------------+
             |               Seeed Studio XIAO ESP32-C3              |
             |                                                       |
 [BAT+] ---->| (Pad 21) BAT+                             3V3_OUT (12)|----+
 [BAT-] ---->| (Pad 22) GND                                 VBUS (14)|--+ |
             |                                                       |  | |
             | (D1 / Pin 2)  BAT_ADC                         GND (13)|  | |
             | (D2 / Pin 3)  SW_PIN / WAKEUP                         |  | |
             | (D4 / Pin 5)  SCL                                     |  | |
             | (D5 / Pin 6)  SDA                                     |  | |
             | (D7 / Pin 8)  LED_DATA                                |  | |
             | (D10 / Pin 11) CHARGE_PIN                             |  | |
             +-------------------------------------------------------+  | |
                                                                        | |
  +---------------------------------------------------------------------+ |
  | (Ligne VBUS 5V)                                                       |
  |                                                                       |
  +--[ R4: 100k ]----+----> Broche D10 (Détection USB)                   |
                     |                                                    |
             [ R5+R6: 200k ] (2x 100k en série)                           |
                     |                                                    |
                    GND                                                   |
                                                                          |
  +-----------------------------------------------------------------------+
  | (Ligne 3V3_OUT)
  |
  +----[ Broche 2 de SW1 ]
         [ Broche 1 de SW1 ] ----+---> Rail 3V3_S (Alimentation commutée)
                                 |
                                 +---> Broche D2 (Détection ON/OFF & Wakeup)
                                 |
                             [ R3: 100k ] (Pull-down)
                                 |
                                GND

  Ligne Batterie BAT+
  |
  +--[ R1: 100k ]----+----> Broche D1 (Mesure ADC Batterie)
                     |
               +-----+-----+
               |           |
           [ R2: 100k ] [ C1: 100nF ]
               |           |
               +-----+-----+
                     |
                    GND
```

---

## 3. Instructions de Montage Pas-à-Pas (Veroboard)

### Étape 1 : Préparation du XIAO ESP32-C3

1. Retournez le XIAO ESP32-C3 pour accéder aux pastilles de soudure de batterie sous la carte :
   - Soudez le fil **ROUGE (+)** de la batterie sur le pad **`BAT+`** (Pin 21).
   - Soudez le fil **NOIR (-)** de la batterie sur le pad **`GND`** (Pin 22).
2. Soudez deux rangées de connecteurs mâles (ou soudez directement les fils sur les pastilles latérales `D0` à `D10`).

### Étape 2 : Câblage de l'Interrupteur $SW_1$ & Rail $3\text{V}3\_S$

1. Connectez la **broche centrale (Broche 2)** de l'interrupteur $SW_1$ à la broche **`3V3_OUT`** (Pin 12) du XIAO.
2. Connectez la **broche de sortie (Broche 1)** de $SW_1$ au rail d'alimentation commuté nommé **`3V3_S`**.
3. Reliez ce rail `3V3_S` à :
   - La broche **`D2`** du XIAO (signal de réveil / détection d'état).
   - Une résistance de pull-down **$R_3 = 100\text{ k}\Omega$** reliée à la masse `GND`.
   - La broche **`VIN`** du capteur VL53L0X.
   - La broche **`VDD / +5V`** de l'anneau de LEDs WS2812B.

### Étape 3 : Câblage des Ponts Diviseurs

1. **Pont de mesure batterie (Broche D1)** :
   - Connectez **$R_1$ ($100\text{ k}\Omega$)** entre `BAT+` et `D1`.
   - Connectez **$R_2$ ($100\text{ k}\Omega$)** et **$C_1$ ($100\text{ nF}$)** en parallèle entre `D1` et `GND`.
2. **Pont de détection charge USB (Broche D10)** :
   - Connectez **$R_4$ ($100\text{ k}\Omega$)** entre `VBUS` (Pin 14) et `D10`.
   - Mettez **deux résistances $100\text{ k}\Omega$ en série ($R_5 + R_6 = 200\text{ k}\Omega$)** entre `D10` et `GND`.

### Étape 4 : Raccordement des Périphériques

1. **Capteur ToF VL53L0X** :
   - `VIN` $\rightarrow$ Rail `3V3_S`
   - `GND` $\rightarrow$ `GND`
   - `SCL` $\rightarrow$ Broche **`D4`** du XIAO
   - `SDA` $\rightarrow$ Broche **`D5`** du XIAO
2. **Anneau de LEDs WS2812B (7 LEDs)** :
   - `DIN` $\rightarrow$ Broche **`D7`** du XIAO
   - `VDD` $\rightarrow$ Rail `3V3_S`
   - `GND` $\rightarrow$ `GND`

---

## 4. Points de Contrôle Avant Mise sous Tension

Avant de brancher la batterie LiPo ou le câble USB, effectuez les contrôles suivants au multimètre en mode continuité (bip) :

1. [ ] **Pas de court-circuit `BAT+` / `GND`** : Résistance infinie ou $> 100\text{ k}\Omega$.
2. [ ] **Pas de court-circuit `3V3_OUT` / `GND`**.
3. [ ] **Pas de court-circuit `VBUS` / `GND`**.
4. [ ] **Fonctionnement de l'interrupteur $SW_1$** :
   - En position **ON** : Continuité entre `3V3_OUT` et `3V3_S`.
   - En position **OFF** : Résistance de $100\text{ k}\Omega$ entre `3V3_S` et `GND` (due à $R_3$).
5. [ ] **Tension sur D10 avec câble USB branché** : Doit être de **$\approx 3.33\text{ V}$** (diviseur 5V par 100k / 200k).
6. [ ] **Tension sur D1 avec batterie à 4.0V** : Doit être de **$\approx 2.00\text{ V}$** (diviseur 100k / 100k).
