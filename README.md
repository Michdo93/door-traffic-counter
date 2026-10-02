# door-traffic-counter

An ESP32 and TF-Luna LiDAR based dual-sensor directional doorway counter integrated with openHAB via MQTT.

This project enables real-time tracking of room occupancy across multiple doors using a dual-LiDAR setup per doorframe. It detects the direction of movement (Entrance vs. Exit), accounts for multi-door setups, and reports count changes to openHAB.

---

## 🛠 Features

- **Directional Detection:** Uses two TF-Luna LiDAR sensors placed 10–15 cm apart to reliably distinguish between people entering (`+1`) and exiting (`-1`).
- **High-Speed LiDAR Precision:** Non-contact optical measurement up to 100 Hz sampling rate.
- **Multi-Door Support:** Scalable MQTT architecture allowing openHAB to aggregate occupancy across $N$ doorways.
- **Manual Correction & Overrides:** Includes UI controls in openHAB for one-click resets and direct occupancy overrides.
- **Multi-Language Automation Support:** Includes openHAB rules for Native Python 3, JavaScript (GraalVM), and openHAB DSL.

---

## 📍 Sensor Orientation: Outer vs. Inner

To ensure consistent direction detection, sensor positions are defined as follows relative to the doorway:

* **`Outer` (Sensor A):** Positioned on the side of the doorframe facing **outside** the room (e.g., facing the hallway or corridor).
* **`Inner` (Sensor B):** Positioned on the side of the doorframe facing **inside** the room.

Both sensors are mounted on the same doorframe, offset by **10–15 cm** along the direction of travel:

```text
[ HALLWAY / OUTSIDE ]                                [ INSIDE ROOM ]
                                DOOR
                               |    |
(Entering Direction)   ----->  | A  | B  ----->
                       <-----  | A  | B  <-----  (Exiting Direction)
                               |    |
```

### Directional Logic in Firmware:

1. **Entering the Room (`+1`):**
* Person approaches from the hallway and triggers **Sensor A (`Outer`)** first.
* Person continues into the room and triggers **Sensor B (`Inner`)**.
* **Sequence:** `Outer (A)` $\rightarrow$ `Inner (B)` = **`+1` (ENTERED)**


2. **Exiting the Room (`-1`):**
* Person approaches from inside the room and triggers **Sensor B (`Inner`)** first.
* Person continues out into the hallway and triggers **Sensor A (`Outer`)**.
* **Sequence:** `Inner (B)` $\rightarrow$ `Outer (A)` = **`-1` (EXITED)**

---

## 🧰 Hardware Requirements

- **1x ESP32 Development Board** (NodeMCU / DevKit V1)
- **2x TF-Luna LiDAR Sensors** (including 6-pin JST cables)
- **1x 5V / 2A USB Power Supply**
- **Jumper Wires (Female-to-Female / Male-to-Female)**

---

## 🔌 Wiring & Hardware Setup

### Mounting
1. Mount **Sensor A (Outer)** and **Sensor B (Inner)** horizontally on the doorframe at chest/shoulder height (**1.20 m – 1.40 m**).
2. Space the sensors **10–15 cm apart** along the direction of passage.


```
[ OUTSIDE ]  --->  (Sensor A)  --- 12cm ---  (Sensor B)  --->  [ INSIDE ]
```

### Pinout Configuration

Both sensors are powered via the ESP32 `5V / VIN` rail. Signal communication uses hardware UART ports.

> **Note:** Cross-connect TX to RX and RX to TX!

| Component | Pin / Wire | ESP32 GPIO | Description |
| :--- | :--- | :--- | :--- |
| **Sensor A (Outer)** | VCC (Pin 1) | `5V` / `VIN` | Power (+5V) |
| | RXD (Pin 2) | `GPIO 27` (TX1) | ESP32 TX -> Sensor RX |
| | TXD (Pin 3) | `GPIO 26` (RX1) | Sensor TX -> ESP32 RX |
| | GND (Pin 4) | `GND` | Ground |
| **Sensor B (Inner)** | VCC (Pin 1) | `5V` / `VIN` | Power (+5V) |
| | RXD (Pin 2) | `GPIO 17` (TX2) | ESP32 TX -> Sensor RX |
| | TXD (Pin 3) | `GPIO 16` (RX2) | Sensor TX -> ESP32 RX |
| | GND (Pin 4) | `GND` | Ground |

---

## 📂 Repository Structure

```text
door-traffic-counter/
├── firmware/
│   └── esp32_door_counter.ino       # ESP32 C++ Arduino Sketch
├── openhab/
│   ├── things/
│   │   └── door_counter.things       # MQTT Bridge & Topic Configuration
│   ├── items/
│   │   └── occupancy.items          # Occupancy & Control Items
│   ├── sitemaps/
│   │   └── occupancy.sitemap        # BasicUI / MainUI Layout
│   └── automation/
│       ├── python/
│       │   └── occupancy.py         # Native Python 3 Rules (openHAB 4/5)
│       ├── js/
│       │   └── occupancy.js         # JavaScript (GraalVM) Rules
│       └── rules/
│           └── occupancy.rules      # openHAB DSL Rules
└── README.md
```

---

## ⚙️ Software Setup

### 1. ESP32 Firmware

1. Open `firmware/esp32_door_counter.ino` in Arduino IDE.
2. Install the `PubSubClient` library via Library Manager.
3. Update `WIFI_SSID`, `WIFI_PASS`, `MQTT_SERVER`, and `MQTT_TOPIC_EVENT` with your credentials.
4. Upload to the ESP32.

### 2. openHAB Integration

1. Copy the contents of the `openhab/` directory into your openHAB configuration folder (`/etc/openhab/` or `conf/`).
2. Ensure the **MQTT Binding** and **JSONPath Transformation** are installed in openHAB.
3. Choose your preferred rules language from `openhab/automation/` and remove the unused versions to prevent duplicate executions.

---

## 📡 MQTT Payload Format

The ESP32 publishes state updates whenever a direction sequence is completed:

* **Topic:** `smarthome/door1/event`
* **Payload:** `{"change": 1}` *(Entry)* or `{"change": -1}` *(Exit)*

---

## 📄 License

MIT License. Feel free to modify and build upon this project!
