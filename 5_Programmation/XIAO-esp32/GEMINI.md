# Light Trainer - ESP32 Game System

## Project Overview

This project is an embedded multi-player game system designed for ESP32 microcontrollers. It facilitates communication between multiple devices using ESP-NOW, incorporates a VL53L0X distance sensor for player interaction, and utilizes WS2812B LEDs for visual feedback. The system supports various game modes and features a web-based configuration portal for easy setup of game parameters, which are persisted across reboots.

The architecture follows a master/slave pattern, where one device acts as a `CONTROLLER` (game master) and others as `IDLE_LISTENER`s or `ACTIVE_TARGET`s. Devices transition through different states like `MENU`, `CONTROLLER_TARGET`, and `ACTIVE_TARGET` based on game progression and configuration.

**Key Technologies:**
*   **Microcontroller:** ESP32 (e.g., ESP32-C3 XIAO)
*   **Wireless Communication:** ESP-NOW
*   **Sensor:** VL53L0X (Time-of-Flight distance sensor)
*   **Actuator:** WS2812B Addressable LEDs
*   **Development Framework:** Arduino
*   **Web Configuration:** Built-in web server for settings (using `WebServer.h` and `Preferences.h`)

## Building and Running

This project is developed using the Arduino IDE (or PlatformIO with Arduino framework).

**Prerequisites:**
1.  **Arduino IDE:** Ensure you have the Arduino IDE installed and configured for ESP32 boards.
2.  **ESP32 Board Support:** Install the ESP32 board definitions in the Arduino IDE Boards Manager.
3.  **Libraries:** Install the following libraries via the Arduino IDE Library Manager:
    *   `FastLED`
    *   `Adafruit VL53L0X`

**Compilation and Upload:**
1.  Open the `main.ino` file in the Arduino IDE.
2.  Select the appropriate ESP32 board (e.g., "XIAO ESP32C3") and port from the "Tools" menu.
3.  Click "Upload" to compile the code and flash it to your ESP32 device.

**Running:**
After uploading, the ESP32 will boot into either `MENU` mode or enter the web configuration mode if triggered (typically by a specific sensor gesture).

*   **Web Configuration Mode:** If the device enters config mode, it will create a Wi-Fi Access Point (AP). Connect to this AP from a computer or phone, then navigate to `192.168.4.1` (or similar) in a web browser to configure game modes and settings.
*   **Game Play:** In `MENU` mode, the game can be started via serial command or sensor input. The controller will then manage game flow, activating targets and detecting hits.

## Development Conventions

*   **Modular Design:** The codebase is structured into logical modules (`Config.h`, `Hardware.h`, `GameNetwork.h`, `GameEngine.h`, `WebConfig.h`) with separate header and implementation files for clarity and maintainability.
*   **Configuration:** Global hardware and system settings, message types, and game states are centrally defined in `Config.h`.
*   **State Machines:** The game logic utilizes a state machine approach (`State` enum) to manage device behavior and game flow.
*   **ESP-NOW Protocol:** Custom `struct_message` is used for inter-device communication, ensuring a lightweight and efficient protocol.
*   **Serial Debugging:** Extensive serial output is used for debugging, providing real-time feedback on sensor states, network messages, and state transitions.
*   **Input Handling:** Both physical sensor input (VL53L0X distance) and serial commands are used for controlling game actions and mode changes.
*   **Non-Volatile Storage:** Game settings configured via the web interface are saved to the ESP32's NVS using the `Preferences` library.
