# Manuel d'Utilisation & Guide des Modes de Jeu — Light Trainer V3

Ce manuel explique comment faire fonctionner vos pods **Light Trainer**, naviguer dans les menus grâce aux gestes sans contact et profiter des différents modes d'entraînement.

---

## 1. Démarrage Rapide

1. **Mise sous tension** :

   - Basculez l'interrupteur mécanique $SW_1$ sur la position **ON**.
   - Les pods s'allument instantanément en mode **MENU** (couleur du mode actif affichée sur les 7 LEDs).

2. **Synchronisation automatique** :

   - Les pods se découvrent mutuellement via le réseau sans fil maillé ESP-NOW dès l'allumage, sans aucune configuration WiFi nécessaire !

---

## 2. Contrôle Gestuel par Capteur ToF (VL53L0X)

L'intégralité du système se pilote par de simples mouvements de la main au-dessus du pod (portée $< 150\text{ mm}$), sans avoir à toucher physiquement le boîtier.

```
                  [ MAIN DU JOUEUR ]
                         ||
                      ( 15 cm )
                         ||
                 [ CAPTEUR ToF LASER ]
               +-----------------------+
               |  (O) (O) (O) (O) (O)  |  (Anneau 7 LEDs)
               +-----------------------+
```

### Table des Gestes :

| Geste | Durée | Action en Mode MENU | Action en Cours de JEU |
| :--- | :--- | :--- | :--- |
| **Tap Court** | $< 500\text{ ms}$ | **Changement de Mode** (Passe au mode suivant 0 $\rightarrow$ 1 $\rightarrow$ 2 $\rightarrow$ 3 $\rightarrow$ 4 $\rightarrow$ 0) | *Ignoré (évite les fausses frappes)* |
| **Maintien Démarrage (Hold 1.5s)** | $1.5\text{ s}$ | **Lancer la Partie** : Les 7 LEDs affichent une jauge circulaire progressive. À 100%, flash blanc et départ immédiat ! | *Non applicable* |
| **Frappe Rapide (Hit)** | $< 300\text{ ms}$ | *Ignoré* | **Validation de la Cible** : Flash blanc instantané ($60\text{ ms}$) et transmission radio au Master. |
| **Maintien Retour Menu (Hold 3s)** | $3.0\text{ s}$ | *Réinitialisation* | **Arrêter le Jeu & Retour au Menu** : Envoie un ordre d'arrêt synchronisé à tous les pods du réseau. |

---

## 3. Les Modes d'Entraînement Sportif

### 🟢 Mode 0 — Vitesse Simple (Solo Training)
* **Couleur de base** : Vert (`#00FF00`).
* **Délai entre cibles** : $0\text{ seconde}$ (activation instantanée de la cible suivante).
* **Déroulement** :
  1. Dès le départ, une cible s'allume en Vert.
  2. Le joueur frappe la cible.
  3. Dès la touche validée, une nouvelle cible s'allume instantanément sur un autre pod au hasard.
* **Objectif** : Vitesse pure, cardio et travail d'appuis.

---

### 🔴🟢 Mode 1 — Duel & Vitesse Bicolore (2 Joueurs)
* **Couleurs** : Joueur 1 = Rouge (`#FF0000`), Joueur 2 = Vert (`#00FF00`).
* **Délai entre cibles** : $0\text{ seconde}$ (réapparition instantanée).
* **Déroulement & Mécanique Compétitive** :
  1. Dès le départ, **deux pods s'allument simultanément** : l'un en Rouge (pour le Joueur 1), l'autre en Vert (pour le Joueur 2).
  2. Chaque joueur se concentre sur sa propre couleur.
  3. Dès que le Joueur 1 éteint la cible Rouge, **une nouvelle cible Rouge s'allume instantanément** sur un autre pod disponible, sans affecter la cible Verte qui reste allumée !
  4. De la même façon, dès que le Joueur 2 éteint la cible Verte, celle-ci réapparaît immédiatement sur un autre pod.
  5. Il y a donc en permanence deux couleurs actives sur le terrain, permettant une course aux points effrénée et continue.
* **Objectif** : Compétition directe 1 contre 1, vision périphérique et discrimination visuelle sous pression.

---

### 🔵 Mode 2 — Réflexe & Attention Aléatoire
* **Couleur de base** : Bleu (`#0000FF`).
* **Délai aléatoire** : Entre $0.5\text{ seconde}$ et $5\text{ secondes}$.
* **Déroulement** :
  1. Après extinction d'une cible, un temps mort imprévisible s'écoule.
  2. Soudainement, un pod s'allume en Bleu.
* **Objectif** : Temps de réaction au stimulus imprévisible, concentration et posture d'attente active.

---

### 🟡🟣 Mode 3 — Agilité Cognitive & Coordination
* **Couleurs** : Jaune (`#FFFF00`) et Magenta (`#FF00FF`).
* **Délai aléatoire** : Entre $0\text{ s}$ et $4\text{ secondes}$.
* **Déroulement** :
  - Les cibles alternent entre des couleurs différentes imposant des règles motrices (ex: Jaune = main droite / pied droit, Magenta = main gauche / pied gauche).
* **Objectif** : Agilité motrice, coordination et dissociation droite/gauche.

---

### 🔴🟡🔵 Mode 4 — Jeu Simon (Mémoire Séquentielle & Accélération)
* **Couleur de base / Représentation** : Rose système (`#FF2D55`). Chaque pod de la flotte dispose d'une couleur signature unique :
  - Pod 1 (Master) : Vert (`#30D158`)
  - Pod 2 : Rouge (`#FF453A`)
  - Pod 3 : Bleu (`#0A84FF`)
  - Pod 4 : Jaune (`#FFD60A`)
  - Pod 5 : Magenta (`#BF5AF2`)
  - Pod 6 : Cyan (`#30B0C7`)
  *(Si 1 seul pod est connecté, les couleurs alternent séquentiellement sur l'anneau central du Master).*
* **Déroulement** :
  1. **Phase Démonstration** : Les pods s'allument l'un après l'autre selon une séquence générée dynamiquement.
  2. **Phase Joueur** : Le joueur doit reproduire la séquence exacte en frappant les pods dans le bon ordre.
  3. **Progression & Accélération** : À chaque manche réussie, flash vert général, la séquence gagne $+1$ étape et la vitesse de démonstration s'accélère automatiquement selon le coefficient configuré (ex. $-6\%$ par tour).
  4. **Fin de Partie** : En cas d'erreur de frappe ou d'inactivité ($>5\text{ s}$), flash rouge général et enregistrement du score record.
* **Objectif** : Mémoire de travail sous effort, lucidité et vitesse de mémorisation spatiale.

---

## 4. Signaux Lumineux de la Batterie & Recharge

| Couleur des LEDs | Signification | Action Recommandée |
| :--- | :--- | :--- |
| 🟠 **Orange Fixe** | **Câble USB branché** (Recharge de la batterie LiPo en cours). | Laisser branché jusqu'à charge complète. |
| 🔴 **Rouge Clignotant** | **Batterie Faible** ($V_{\text{batt}} < 3.4\text{ V}$, reste environ 10%). | Rebrancher le pod sur un chargeur USB-C. |
| 🔴 **Rouge Fixe puis Extinction** | **Coupure Critique** ($V_{\text{batt}} \le 3.0\text{ V}$). | Le pod se met en sécurité pour préserver la chimie LiPo. |

### Suivi de la Batterie de Toute la Flotte via le Web :
Dans l'onglet **Diagnostics** du portail Web, la section **"Batterie de toute la flotte"** centralise en temps réel :
* La tension exacte ($V$) et le pourcentage restant ($\%$) de chaque pod du maillage.
* Le statut de charge USB (`⚡ Charge USB`).
* L'état de présence : les pods éteints ou en veille prolongée sont signalés par la mention *"💤 En veille"* avec le temps écoulé depuis la dernière émission radio.

---

## 5. Portail Web de Configuration & Entraînement

En plus des gestes physiques ToF, le Light Trainer intègre un portail Web complet et réactif accessible sans installation depuis n'importe quel smartphone, tablette ou ordinateur :

1. Connectez votre appareil au réseau WiFi diffusé par le pod maître : **`LightTrainer_Config`** (ou `LightTrainer`).
2. Ouvrez votre navigateur internet à l'adresse : **`http://192.168.4.1`** (le portail captif s'ouvre généralement automatiquement).
3. Vous disposez alors de 4 espaces de contrôle :
   - **🎯 Entraînement Automatique** : Lancez et pilotez à distance les 5 modes d'entraînement (Vitesse Solo, Duel Bicolore, Réflexe Aléatoire, Agilité Cognitive, Jeu Simon), avec chronomètre géant, graphiques de temps de réaction et analyse comparative Duel / Simon.
   - **🕹️ Mode Manuel Coach** : Allumez n'importe quel pod à la couleur de votre choix d'un simple toucher sur votre écran, mesurez le temps mis par l'athlète pour frapper le pod ordonné, ou déclenchez des flashs d'alerte généraux.
   - **🧪 Tests & Diagnostic** : Visualisez la distance mesurée par le capteur ToF en millimètres, lancez un test de ping radio ESP-NOW mesurant le temps d'aller-retour ($RTT$) et le niveau de signal ($RSSI$) de chaque pod, et contrôlez l'état de batterie de toute la flotte.
   - **⚙️ Réglages & Export CSV** : Réglez la luminosité globale des LEDs ($10\%$ à $100\%$, mémorisée en Flash NVS), consultez la console de logs en direct, et exportez l'historique complet des frappes de votre séance d'entraînement au format CSV.

