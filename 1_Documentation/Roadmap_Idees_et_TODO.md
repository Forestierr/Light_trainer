# Feuille de Route, Idées d'Évolution & TODO — Light Trainer

Ce document consigne les axes d'amélioration logicielle, les évolutions architecturales et le plan d'action (TODO) pour faire évoluer le projet **Light Trainer** d'un stade de prototype fonctionnel vers un **produit fini, robuste, déployable et industrialisable**.

---

## 🧭 Synthèse des Piliers Stratégiques

```mermaid
mindmap
  root((Light Trainer<br/>Roadmap Production))
    1. Réseau & Flotte
      Master SoftAP Exclusif
      Élection Dynamique du Master
      Heartbeat & Synchro Microseconde
      Veille Synchronisée Mesh
    2. Web & Interface Coach
      WebSockets ou SSE
      Compression Gzip / Assets
      PWA Installable Hors-ligne
      Créateur d'Entraînements & Drills
    3. Optique & ToF
      Auto-calibration Lumière Ambiante
      Détection par Dérivée d(dist)/dt
      Sauvegarde des Profils en NVS
    4. Déploiement & OTA
      Mise à jour Web OTA Multi-Pods
      Gestion des Noms & Identifiants
      Logs Crash Persistants (LittleFS)
    5. Ergonomie & Hardware
      Feedback Audio (Buzzer PWM)
      Jauges & Animations LED Étendues
    6. Toolchain & Qualité
      Migration PlatformIO
      Intégration Continue (CI)
```

---

## 1. Architecture Réseau & Flotte

### 1.1. SoftAP Exclusif au Master (Suppression des Conflits RF)
* **Constat actuel** : Tous les pods démarrent actuellement un point d'accès SoftAP (`LightTrainer` ou `LightTrainer_Config`) sur le canal 1. Lorsque 6 pods sont allumés à proximité, 6 points d'accès diffusent des balises 802.11 balayant le spectre radio et augmentant les collisions de paquets ESP-NOW.
* **Solution cible** :
  - Tous les pods démarrent en mode **`WIFI_STA` pur**.
  - Seul le pod qui prend le rôle de **Master** (démarré via appui long 1.5s ou sélection Web initiale) active son interface SoftAP (`WIFI_AP_STA`).
  - Si le Master s'éteint ou est mis en veille, un autre pod peut reprendre l'AP automatiquement.
* **Bénéfice** : Réduction drastique de la congestion RF 2.4 GHz, gain d'autonomie pour les pods esclaves (pas de balises AP à émettre).

### 1.2. Élection Dynamique du Master & Tolérance aux Pannes
* **Constat actuel** : Le Master est déterminé par le premier pod sur lequel l'utilisateur déclenche un démarrage de jeu ou connecte son smartphone.
* **Solution cible** :
  - Implémenter un protocole d'élection de leader inspiré de Raft/Bully pour les réseaux ad-hoc : le pod avec l'ID MAC le plus bas ou le niveau de batterie le plus élevé assure la coordination si le Master actif tombe en panne.
  - Sauvegarde de l'état de jeu courant dans les trames de heartbeat pour reprise transparente.

### 1.3. Synchronisation Temporelle & Mesure Précise de la Latence ($\mu\text{s}$)
* **Constat actuel** : Le temps de réaction est calculé entre l'activation ordonnée par le Master et la réception du message `CMD_HIT`. Cela inclut la latence aller-retour radio (~2 à 5 ms).
* **Solution cible** :
  - Intégrer un horodatage absolu dans chaque pod synchronisé par trames périodiques ESP-NOW (synchronisation d'horloge type SNTP local / PTP léger).
  - L'esclave horodate la détection physique ToF à la microseconde près et transmet `reactionTimeMs` directement dans le paquet `CMD_HIT`.

---

## 2. Interface Web Coach & Expérience Applicative

### 2.1. Migration HTTP Polling $\rightarrow$ WebSockets ou Server-Sent Events (SSE)
* **Constat actuel** : Le dashboard interroge l'ESP32 toutes les 250 ms (`pollStatus`) et 2000 ms (`pollPods`) via des requêtes HTTP `GET` répétées. Bien que fonctionnel grâce à `send_P`, cela génère un trafic TCP/HTTP constant et sollicite inutilement la stack réseau.
* **Solution cible** :
  - Passer sur une connexion persistante **WebSocket** ou **SSE** (Server-Sent Events).
  - Le Master pousse un événement JSON uniquement lors d'un changement d'état : nouvelle touche, chronomètre, détection batterie ou changement de statut pod.
* **Bénéfice** : Réactivité instantanée (< 5 ms affichage Web), consommation CPU et RF réduite sur l'ESP32.

### 2.2. Compression GZIP & Pipeline d'Assets
* **Constat actuel** : Le code HTML/CSS/JS est stocké en clair sous forme de chaîne `PROGMEM` (~75 Ko).
* **Solution cible** :
  - Pré-compiler et compresser les assets Web en **GZIP** (`index.html.gz` ~20 Ko).
  - Utiliser `server.sendHeader("Content-Encoding", "gzip")` et `server.send_P(200, "text/html", DASHBOARD_GZ, sizeof(DASHBOARD_GZ))`.
* **Bénéfice** : Division par 3.5 du temps de chargement initial de la page sur mobile et économie de 50 Ko de mémoire Flash.

### 2.3. Transformation en Progressive Web App (PWA)
* **Constat actuel** : L'utilisateur ouvre la page via le navigateur ou le portail captif.
* **Solution cible** :
  - Ajouter un `manifest.json` et un Service Worker minimal.
  - Permettre l'installation en "Application native" sur l'écran d'accueil iOS/Android avec icône personnalisée, fonctionnement en plein écran sans barre d'adresse et cache hors-ligne.
* **Est - ce fonctionelle ?**

### 2.4. Créateur d'Entraînements Personnalisés (Drill Builder)
* **Idée** : Permettre au coach de programmer des séquences d'entraînement personnalisées :
  - Exemple : Pod 1 (Rouge) $\rightarrow$ Pod 3 (Bleu) $\rightarrow$ Pause 2s $\rightarrow$ Pod 2 (Vert).
  - Enregistrement des séquences favorites dans la mémoire NVS de l'ESP32 ou dans le `localStorage` du navigateur.

---

## 3. Capteur ToF & Traitement du Signal Optique

### 3.1. Auto-Calibration Dynamique de la Lumière Ambiante
* **Constat actuel** : Le seuil de déclenchement est fixé statiquement à $150\text{ mm}$. En plein soleil ou sous des néons fluorescents pulsés à 100 Hz, le VL53L0X peut renvoyer des valeurs erratiques (code d'erreur 8190 ou distances instables).
* **Solution cible** :
  - À l'allumage, réaliser une séquence d'étalonnage de 1 seconde pour mesurer le bruit de fond ambiant et la distance au plafond / obstacles fixes.
  - Ajuster automatiquement la fenêtre de validité et le seuil de détection.

### 3.2. Détection par Dérivée Temporelle ($d(\text{dist})/dt$)
* **Constat actuel** : Détection sur condition statique `distance <= TOF_TRIGGER_DISTANCE`.
* **Solution cible** :
  - Calculer la vitesse d'approche de la main : si $\frac{\Delta \text{distance}}{\Delta t} < -V_{\text{seuil}}$, la main est en mouvement rapide d'attaque vers le pod.
  - Rejeter les fausses détections dues aux passages lointains ou ombres lentes.

### 3.3. Sauvegarde des Profils de Calibration en Flash NVS
* Sauvegarder un offset optique propre à chaque boîtier (lié à la vitre de protection en polycarbonate/acrylique et aux tolérances mécaniques) directement en mémoire non-volatile via l'API `Preferences` d'ESP32.

---

## 4. Déploiement, Maintenance & Production Industrielle

### 4.1. Mises à Jour Over-The-Air (Web OTA Multi-Pods)
* **Constat actuel** : Chaque mise à jour du firmware nécessite de brancher physiquement chaque pod en USB-C sur le PC et d'utiliser l'IDE Arduino.
* **Solution cible** :
  - Intégrer un endpoint `/update` sur le serveur Web (bibliothèque `Update.h` d'ESP32).
  - Permettre au coach de téléverser un fichier `.bin` depuis son smartphone pour mettre à jour le Master.
  - Option avancée : Propager le nouveau binaire aux pods esclaves via des blocs ESP-NOW (Mesh OTA distribué).

### 4.2. Identification & Nommage Personnalisé des Pods
* Permettre au coach de renommer chaque pod depuis l'interface Web (ex. : *"Pod Poteau Gauche"*, *"Pod Ligne Centrale"*).
* Sauvegarder ce nom personnalisé en Flash NVS sur le pod correspondant.

### 4.3. Journal d'Erreurs Persistant (Crash Dumper)
* Stocker les 20 derniers crashs système ou redémarrages inattendus (cause de reset ESP32, trace mémoire, exception) dans la partition SPIFFS/LittleFS pour faciliter le diagnostic SAV à distance.

---

## 5. Ergonomie & Retour Sensoriel

### 5.1. Intégration du Buzzer Piézoélectrique (Sortie D3 / GPIO5)
* Le schéma matériel V3 prévoit la broche `D3` (TP2) pour un buzzer passif ou magnétique.
* **Évolutions logicielles** :
  - Bips courts de compte à rebours au départ ($3, 2, 1, \text{GO}$).
  - Tonalité aiguë ($2.5\text{ kHz}, 30\text{ ms}$) pour valider une frappe réussie.
  - Tonalité grave ($400\text{ Hz}, 200\text{ ms}$) en cas de fausse couleur ou d'erreur sur le jeu Simon.
  - Option silencieuse configurable depuis le Dashboard Web pour les entraînements en salle fermée.

---

## 6. Chaîne de Compilation & Qualité Logicielle

### 6.1. Migration vers PlatformIO
* **Bénéfices** :
  - Fichier `platformio.ini` standardisant le partitionnement (`huge_app.csv`), la vitesse d'horloge, les flags de compilation (`-O2`, `-DDEBUG`) et les versions strictes des bibliothèques (`FastLED`, `Adafruit_VL53L0X`).
  - Suppression des erreurs de partitionnement manuelles dans l'IDE Arduino.
  - Téléversement et monitoring série en une seule commande en ligne de commande.

### 6.2. Intégration Continue (GitHub Actions)
* Compiler automatiquement le code à chaque push/pull-request pour garantir l'absence de régression de compilation ou de dépassement de mémoire Flash/RAM.

---

## 📋 Tableau de Suivi des Tâches (TODO Backlog)

### 🔴 Priorité P0 — Indispensable pour la Fiabilité en Production
- [x] **Réseau** : Désactiver le point d'accès SoftAP sur tous les pods esclaves (`WIFI_STA` pur) ; n'activer le SoftAP que sur le Master.
- [ ] **Compilation** : Fournir un fichier de partitionnement personnalisé ou formaliser la configuration IDE Arduino `Partition Scheme -> Huge APP (3MB)` dans tous les guides.
- [x] **Réseau** : Gérer la ré-élection du Master si le Master actuel s'éteint ou est mis en veille pendant une session.
- [ ] **Robustesse Web** : Mettre en cache les requêtes DNS sur le portail captif pour limiter les interruptions sur Android / iOS récents.

### 🟡 Priorité P1 — Confort, Performance & UX Avancée
- [x] **Web** : Compresser le payload HTML/CSS/JS en GZIP (`.gz`) dans la mémoire Flash pour réduire le temps de transfert HTTP (15.6 Ko vs 72 Ko).
- [x] **Web** : Remplacer le polling HTTP 250 ms par un Smart Polling adaptatif (100 ms en jeu / 1000 ms en veille, flux déclenché par événements, séparation `/api/config`) pour alléger la boucle `loop()`.
- [ ] **Optique** : Ajouter un algorithme de détection de dérivée de distance pour fiabiliser la détection des frappes rapides.
- [ ] **Énergie** : Implémenter la commande de mise en veille synchronisée de la flotte (`CMD_SLEEP_FLEET`).
- [ ] **Audio** : Implémenter le driver PWM pour le buzzer sur la broche `D3` (départ, hit, erreur Simon).
- [ ] **UX Web** : Ajouter le fichier `manifest.json` pour permettre l'installation PWA sur smartphone.

### 🟢 Priorité P2 — Évolutions Fonctionnelles & Outils
- [ ] **Firmware** : Implémenter la mise à jour Over-The-Air (Web OTA) via l'interface `/update`.
- [ ] **Toolchain** : Ajouter la configuration PlatformIO (`platformio.ini`) à la racine du dossier programmation.
- [ ] **Entraînement** : Créer un éditeur de parcours personnalisés (Drill Builder) dans l'interface Web.
- [ ] **Persistance** : Permettre au coach de nommer les pods individuellement et sauvegarder les noms en NVS.
- [ ] **CI/CD** : Mettre en place un workflow GitHub Actions pour vérifier la compilation automatique du firmware.
