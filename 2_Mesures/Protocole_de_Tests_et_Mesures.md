# Protocole de Tests et Relevé de Mesures — Light Trainer V3

Ce document sert de cahier d'expérimentation pour consigner les résultats mesurés sur les prototypes Veroboard / PCB V3.

---

## 📊 1. Test d'Autonomie et Décharge Batterie

* **Batterie testée** : LiPo 3.7V (Capacité : _______ mAh)
* **Mode de test** : `TEST_BATTERY_STRESS` ou `TEST_BATTERY_GAME_SIM`

| Temps (h:min) | Tension $V_{\text{batt}}$ (V) | Pourcentage (%) | Consommation mesurée (mA) | Remarques / Comportement |
| :--- | :--- | :--- | :--- | :--- |
| **00:00** | 4.20 V | 100 % | | Début du test |
| **00:30** | | | | |
| **01:00** | | | | |
| **01:30** | | | | |
| **02:00** | | | | |
| **03:00** | | | | |
| **04:00** | | | | |
| **Fin (Coupure)**| 3.00 V | 0 % | | Durée totale : _____ h _____ min |

---

## 📡 2. Test de Portée Radio & Latence ESP-NOW

* **Mode de test** : `TEST_RADIO_PING_RTT_RSSI` (Canal 1, Broadcast, 10 pings/sec)
* **Conditions météo / environnement** : ___________________________

| Distance (m) | Obstacles (Vue directe / Murs) | RSSI moyen (dBm) | Latence RTT (ms) | Perte de paquets (%) | Statut LED |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **1 m** | Vue directe (Bench) | | | | 🟢 Vert |
| **5 m** | Vue directe | | | | |
| **10 m** | Vue directe | | | | |
| **20 m** | Vue directe (Extérieur) | | | | |
| **30 m** | Vue directe (Extérieur) | | | | |
| **50 m** | Vue directe (Extérieur) | | | | |
| **Portée Max** | Décrochage complet | | | | 🔴 Rouge |
| **Intérieur** | 1 Mur béton / brique | | | | |
| **Intérieur** | 2 Murs | | | | |

---

## 🎯 3. Qualification Capteur ToF (VL53L0X) & Tenue au Soleil

* **Mode de test** : `TEST_TOF_BENCHMARK`

| Environnement | Éclairement (Lux / Météo) | Distance max détectée (mm) | Faux Hits / min | Taux d'erreurs optiques (%) | Réactivité / Remarques |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Intérieur sombre** | < 100 lux | | 0 | | Réactivité maximale |
| **Intérieur éclairé**| ~500 lux (Néons/LEDs) | | 0 | | |
| **Extérieur à l'ombre**| ~5 000 lux | | | | |
| **Plein Soleil direct**| > 50 000 lux | | | | Vérifier saturation IR |

---

## ⚡ 4. Consommation Électrique & Veille Profonde (Deep Sleep)

* **Appareil de mesure** : Multimètre en série avec la batterie (calibre $\mu\text{A}$ / $\text{mA}$)

| État du Pod | Tension $V_{\text{batt}}$ (V) | Courant mesuré | Autonomie théorique calculée |
| :--- | :--- | :--- | :--- |
| **Deep Sleep (Switch OFF)** | 3.7 V | **_____ $\mu\text{A}$** | > 1 an |
| **Veille Inactivité (Timeout)** | 3.7 V | **_____ $\mu\text{A}$** | > 1 an |
| **Menu / Écoute passive** | 3.7 V | _____ mA | |
| **Cible active (LEDs 50%)** | 3.7 V | _____ mA | |
| **Stress Test (LEDs 100% Blanc + RF)** | 3.7 V | _____ mA | |
| **Recharge USB en cours** | 5.0 V (USB) | _____ mA | Temps de charge : _____ min |
