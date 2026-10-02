#include <WiFi.h>
#include <PubSubClient.h>

// WiFi Configuration
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

// MQTT Configuration
const char* MQTT_SERVER   = "192.168.1.100"; // IP of your MQTT Broker
const int   MQTT_PORT     = 1883;
const char* MQTT_USER     = "YOUR_MQTT_USER";
const char* MQTT_PASS     = "YOUR_MQTT_PASS";
const char* MQTT_TOPIC_EVENT = "smarthome/door1/event";

// Distance threshold for detection (in cm)
const int DETECTION_THRESHOLD_CM = 80;

// UART Pin Definitions
#define SENSOR_A_RX 26
#define SENSOR_A_TX 27
#define SENSOR_B_RX 16
#define SENSOR_B_TX 17

HardwareSerial SerialSensorA(1);
HardwareSerial SerialSensorB(2);

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// State Machine States
enum State { IDLE, A_FIRST, B_FIRST };
State currentState = IDLE;
unsigned long stateStartTime = 0;
const unsigned long TIMEOUT_MS = 2000; // Reset state if movement stalls

int readTFLuna(HardwareSerial &serialPort) {
  uint8_t buffer[9];
  while (serialPort.available() >= 9) {
    if (serialPort.read() == 0x59) {
      if (serialPort.peek() == 0x59) {
        buffer[0] = 0x59;
        serialPort.read(); // Consume second 0x59
        for (int i = 2; i < 9; i++) {
          buffer[i] = serialPort.read();
        }
        
        // Calculate Checksum
        uint8_t checksum = 0;
        for (int i = 0; i < 8; i++) {
          checksum += buffer[i];
        }
        
        if (checksum == buffer[8]) {
          int distance = buffer[2] | (buffer[3] << 8);
          return distance;
        }
      }
    }
  }
  return -1; // Return -1 on invalid/no frame
}

void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("[WiFi] Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[WiFi] Connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("[MQTT] Attempting connection...");
    String clientId = "ESP32_DoorCounter_" + String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println(" Connected.");
    } else {
      Serial.print(" Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void publishEvent(int change) {
  String payload = "{\"change\":" + String(change) + "}";
  mqttClient.publish(MQTT_TOPIC_EVENT, payload.c_str());
  Serial.print("[MQTT] Published payload: ");
  Serial.println(payload);
}

void setup() {
  Serial.begin(115200);
  
  // TF-Luna default baud rate is 115200
  SerialSensorA.begin(115200, SERIAL_8N1, SENSOR_A_RX, SENSOR_A_TX);
  SerialSensorB.begin(115200, SERIAL_8N1, SENSOR_B_RX, SENSOR_B_TX);

  setupWiFi();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  int distA = readTFLuna(SerialSensorA);
  int distB = readTFLuna(SerialSensorB);

  bool activeA = (distA > 0 && distA < DETECTION_THRESHOLD_CM);
  bool activeB = (distB > 0 && distB < DETECTION_THRESHOLD_CM);

  // Reset state if timeout occurs
  if (currentState != IDLE && (millis() - stateStartTime > TIMEOUT_MS)) {
    Serial.println("[State] Timeout reached. Resetting to IDLE.");
    currentState = IDLE;
  }

  // Direction State Machine
  switch (currentState) {
    case IDLE:
      if (activeA && !activeB) {
        currentState = A_FIRST;
        stateStartTime = millis();
        Serial.println("[State] Entered A_FIRST (Moving Inward)");
      } else if (activeB && !activeA) {
        currentState = B_FIRST;
        stateStartTime = millis();
        Serial.println("[State] Entered B_FIRST (Moving Outward)");
      }
      break;

    case A_FIRST:
      if (activeB) {
        Serial.println("[Event] Person ENTERED the room (+1)");
        publishEvent(1);
        currentState = IDLE;
        delay(500); // Debounce delay
      }
      break;

    case B_FIRST:
      if (activeA) {
        Serial.println("[Event] Person EXITED the room (-1)");
        publishEvent(-1);
        currentState = IDLE;
        delay(500); // Debounce delay
      }
      break;
  }

  delay(20); // Loop cadence (~50 Hz)
}
