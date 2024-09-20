#include <WiFiS3.h>
#include <ArduinoMqttClient.h>

#define MINUTE 60000
#define SECOND 1000
#define DEBOUNCE_DELAY 100

// Wi-Fi credentials
const char ssid[] = "YOUR-WIFI-NAME";
const char pass[] = "YOUR-WiFi-PASSWORD";

// MQTT broker settings
const char mqtt_broker[] = "YOUR_MQRR_HOST_IP"; // your localhost if you run it locally
const int mqtt_port = 1883;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

int ledPin = 4;
int pirPin = 2;
int lastPirVal = LOW;
int pirVal;
unsigned long myTime;
unsigned long lastMotionTime;
unsigned long currentTime;
char printBuffer[128];

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  Serial.begin(9600);
  while (!Serial);

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to WiFi");

  // Connect to MQTT broker
  Serial.print("Connecting to MQTT broker");
  if (!mqttClient.connect(mqtt_broker, mqtt_port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }
  Serial.println("\nConnected to MQTT broker");
}

void sendMqttMessage(const char* topic, const char* message) {
  mqttClient.beginMessage(topic);
  mqttClient.print(message);
  mqttClient.endMessage();
}

void loop() {
  currentTime = millis();
  pirVal = digitalRead(pirPin);

  if (pirVal == HIGH) {
    if (lastPirVal == LOW && (currentTime - lastMotionTime) > DEBOUNCE_DELAY) {
      digitalWrite(ledPin, HIGH);
      myTime = millis();
      sprintf(printBuffer, "motion_sensor,location=room1 state=1 %llu", myTime * 1000000ULL);
      sendMqttMessage("arduino/motion", printBuffer);
      Serial.println("Motion detected, sent to MQTT");
      lastPirVal = HIGH;
      lastMotionTime = currentTime;
    }
  } 
  else {
    if (lastPirVal == HIGH && (currentTime - lastMotionTime) > DEBOUNCE_DELAY) {
      digitalWrite(ledPin, LOW);
      myTime = millis();
      sprintf(printBuffer, "motion_sensor,location=room1 state=0 %llu", myTime * 1000000ULL);
      sendMqttMessage("arduino/motion", printBuffer);
      Serial.println("Motion ended, sent to MQTT");
      lastPirVal = LOW;
      lastMotionTime = currentTime;
    }
  }

  mqttClient.poll();
  delay(100);
}