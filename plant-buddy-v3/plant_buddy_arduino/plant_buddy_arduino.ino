/*
 * Arduino Uno R4 WiFi -> InfluxDB 3 (Direct HTTP Write)
 * - Bypasses incompatible ESP8266 libraries
 * - Uses Native InfluxDB 3 /api/v3/write_lp endpoint
 */

#include "WiFiS3.h"

// ------------------------------------------------------------------
// NETWORK CONFIGURATION
// ------------------------------------------------------------------
char ssid[] = "WIFI_NAME";    
char pass[] = "WIFI_PASSWORD";

// ------------------------------------------------------------------
// INFLUXDB 3 CONFIGURATION
// ------------------------------------------------------------------
// Your Mac's IP: 192.168.0.70
IPAddress serverIp(192, 168, 0, 70); 
const int serverPort = 8181;

const char* database = "soil_data"; 

// Your API Token
const char* token = "apiv3_ZkkLDtQHM6wxvXMqIKRZsTSfsHV2fMOFV4LjrK4zppJF0k-piLMYtcTNXYGf0Xr_U8svX84w6-Nk6Tr8eESzFw";

WiFiClient client;

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for Serial Monitor

  // 1. Connect to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // ------------------------------------------------
  // 1. READ SENSOR
  // ------------------------------------------------
  int rawValue = analogRead(A0);
  
  // Map 1023 (Dry) -> 300 (Wet) to 0-100%
  // Adjust these 1023/300 limits if your calibration is different
  int moisturePercent = map(rawValue, 1023, 300, 0, 100); 
  moisturePercent = constrain(moisturePercent, 0, 100);

  Serial.print("Reading: ");
  Serial.print(moisturePercent);
  Serial.println("%");

  // ------------------------------------------------
  // 2. FORMAT DATA (INFLUX LINE PROTOCOL)
  // ------------------------------------------------
  // Format: measurement,tags fields timestamp(implicit)
  // We use the measurement name "environment" to match your previous code
  
  String lineProtocol = "environment,device=arduino_uno_r4,location=potted_plant "; 
  lineProtocol += "humidity_percent=" + String(moisturePercent) + ",humidity_raw=" + String(rawValue);

  // ------------------------------------------------
  // 3. SEND TO INFLUXDB
  // ------------------------------------------------
  sendToInflux(lineProtocol);

  delay(5000); // Write every 5 seconds
}

void sendToInflux(String data) {
  if (client.connect(serverIp, serverPort)) {
    Serial.print("Sending to InfluxDB... ");

    // POST request to InfluxDB 3 native endpoint
    client.println("POST /api/v3/write_lp?db=" + String(database) + " HTTP/1.1");
    
    // Headers
    client.print("Host: ");
    client.println(serverIp);
    
    // Auth Token
    client.print("Authorization: Token "); // "Token" usually works, if not try "Bearer"
    client.println(token);
    
    client.println("Connection: close");
    client.println("Content-Type: text/plain; charset=utf-8");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println(); // End of headers
    
    // Payload
    client.println(data);

    // Optional: Read response if you want to debug errors
    /*
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
    }
    */
    
    client.stop();
    Serial.println("Success!");
  } else {
    Serial.println("Connection Failed! Check Mac Firewall.");
  }
}