# Light Trainer - ESP32 Game System

Un système de jeu multi-joueurs utilisant ESP32 avec communication ESP-NOW, capteur de distance VL53L0X et LEDs WS2812B.

## Structure du Projet

```
main/
├── main.ino          # Fichier principal Arduino
├── light_trainer.h   # Déclarations des constantes, structures et fonctions
├── light_trainer.cpp # Implémentations des fonctions de jeu
└── README.md         # Ce fichier
```

## Matériel Requis

- **ESP32-C3 XIAO** (ou compatible ESP32)
- **Capteur VL53L0X** (distance laser)
- **LEDs WS2812B** (ruban ou matrice)
- **Batterie 3.7V**

## Configuration des Pins

```cpp
#define LED_PIN     D0  // LEDs WS2812B
#define SDA_PIN     D4  // I2C SDA pour VL53L0X
#define SCL_PIN     D5  // I2C SCL pour VL53L0X
```

## Modes de Jeu

Le système supporte 4 modes de jeu différents :

1. **Mode 0**: 1 couleur (Vert), sans délai
2. **Mode 1**: 2 couleurs (Vert/Rouge), sans délai
3. **Mode 2**: 1 couleur (Bleu), délai aléatoire 0-10s
4. **Mode 3**: 2 couleurs (Bleu/Jaune), délai aléatoire 0-10s

## États du Système

- **MENU**: État initial, attente de configuration
- **CONTROLLER**: Maître du jeu, contrôle la logique
- **CONTROLLER_TARGET**: Le contrôleur est la cible
- **ACTIVE_TARGET**: Esclave sélectionné comme cible
- **IDLE_LISTENER**: Esclave en attente

## Contrôles Série

- `'s'`: Démarrer le jeu
- `'h'`: Simuler un hit
- `'0'-'3'`: Changer de mode

### Types de Messages

- `CMD_START`: Démarrage du jeu
- `CMD_ACTIVATE`: Activation d'une cible
- `CMD_HIT`: Cible touchée
- `CMD_PING`: Découverte des nœuds
- `CMD_ACK`: Accusé de réception

### Structure des Messages

```cpp
typedef struct struct_message {
  uint8_t msgType;    // Type de message
  uint32_t color;     // Couleur à afficher
  uint64_t targetID;  // ID de la cible
  uint64_t senderID;  // ID de l'expéditeur
} struct_message;
```

## Utilisation

### Contrôles par Capteur (VL53L0X)

- **Distance < 70mm**: Démarrer le jeu (en mode MENU)
- **Distance 70-200mm**: Changer de mode (en mode MENU)
- **Distance < 80mm**: Toucher la cible (quand allumé)

### Contrôles Série

- `'s'`: Démarrer le jeu
- `'h'`: Simuler un hit
- `'0'-'3'`: Changer de mode

## Fonctions Principales

### Dans `light_trainer.cpp`

- `setLight(uint32_t color)`: Contrôle des LEDs
- `sendBroadcast()`: Envoi de messages broadcast
- `discoverNodes()`: Découverte des nœuds connectés
- `nextTurn()`: Logique du tour suivant
- `actionStartGame()`: Démarrage du jeu
- `actionHit()`: Gestion d'un hit
- `actionModeChange()`: Changement de mode

## Compilation

1. Ouvrir `main.ino` dans Arduino IDE
2. Sélectionner la carte ESP32 appropriée
3. Installer les bibliothèques requises :
   - FastLED
   - Adafruit_VL53L0X
4. Compiler et téléverser

## Débogage

Le système fournit des messages série détaillés pour le débogage :
- États des capteurs
- Messages ESP-NOW reçus/envoyés
- Changements d'état
- Informations de découverte de nœuds
