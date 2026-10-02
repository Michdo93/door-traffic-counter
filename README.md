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
