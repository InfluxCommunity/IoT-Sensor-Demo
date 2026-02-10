/*
 * Arduino Uno R4 WiFi -> MQTT Publisher
 * Broadcasts JSON data to topic: home/sensors/soil
 */

#include <ArduinoMqttClient.h>
#include <WiFiS3.h>

// ------------------------------------------------------------------
// NETWORK CONFIGURATION
// ------------------------------------------------------------------
char ssid[] = "WIFI_NAME";    
char pass[] = "WIFI_PASSWORD";

// MQTT BROKER SETTINGS
// Use your Mac's IP Address here (same as before)
IPAddress broker(192, 168, 0, 70); 
int port = 1883;

const char topic[]  = "home/sensors/soil";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // 1. Connect to WiFi
  Serial.print("Connecting to WiFi");
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }
  Serial.println("\nWiFi Connected!");

  // 2. Connect to MQTT Broker
  Serial.print("Connecting to MQTT Broker at ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1); // Stop if we can't connect
  }

  Serial.println("Connected to MQTT Broker!");
}

void loop() {
  // IMPORTANT: Keeps the connection alive
  mqttClient.poll();

  // ------------------------------------------------
  // 1. READ SENSOR
  // ------------------------------------------------
  int rawValue = analogRead(A0);
  int moisturePercent = map(rawValue, 1023, 300, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  Serial.print("Reading: ");
  Serial.println(moisturePercent);

  // ------------------------------------------------
  // 2. FORMAT DATA (JSON)
  // ------------------------------------------------
  // We send a simple JSON string: {"percent": 45, "raw": 600}
  String payload = "{\"percent\":";
  payload += moisturePercent;
  payload += ", \"raw\":";
  payload += rawValue;
  payload += "}";

  // ------------------------------------------------
  // 3. PUBLISH MESSAGE
  // ------------------------------------------------
  mqttClient.beginMessage(topic);
  mqttClient.print(payload);
  mqttClient.endMessage();

  Serial.println("Sent: " + payload);

  delay(5000); // Publish every 5 seconds
}