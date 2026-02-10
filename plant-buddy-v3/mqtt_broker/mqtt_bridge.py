import paho.mqtt.client as mqtt
import json
from influxdb_client_3 import InfluxDBClient3, Point

# ------------------------------------------------------------------
# CONFIGURATION
# ------------------------------------------------------------------

# MQTT Settings
MQTT_BROKER = "localhost" # Since script runs on same Mac as Docker
MQTT_TOPIC = "home/sensors/soil"

# InfluxDB 3 Settings
INFLUX_URL = "http://localhost:8181"
INFLUX_TOKEN = "TOKEN_STRING"
INFLUX_ORG = "docs" 
INFLUX_DB = "soil_data"

# ------------------------------------------------------------------
# 1. SETUP INFLUXDB 3 CLIENT
# ------------------------------------------------------------------
# Notice how much cleaner this init is compared to v2!
client = InfluxDBClient3(
    host=INFLUX_URL,
    token=INFLUX_TOKEN,
    org=INFLUX_ORG,
    database=INFLUX_DB
)

# ------------------------------------------------------------------
# 2. DEFINE MQTT CALLBACKS
# ------------------------------------------------------------------

def on_connect(client, userdata, flags, rc, properties=None):
    print(f"Connected to MQTT Broker (Code: {rc})")
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    try:
        # A. Decode the JSON payload
        payload_str = msg.payload.decode("utf-8")
        data = json.loads(payload_str)
        
        print(f"Received: {data}")

        # B. Create the Data Point
        # We write to a NEW measurement: "environment_mqtt"
        point = Point("environment_mqtt") \
            .tag("device", "arduino_uno_r4") \
            .tag("location", "home_office") \
            .field("humidity_percent", int(data["percent"])) \
            .field("humidity_raw", int(data["raw"]))

        # C. Write to InfluxDB
        # The v3 client handles batching and retries automatically if needed,
        # but for single points, this is instant.
        client.write(point)
        print(" -> Written to DB")

    except Exception as e:
        print(f"Error: {e}")

# ------------------------------------------------------------------
# 3. RUN THE LISTENER
# ------------------------------------------------------------------
mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

print("Connecting to MQTT Broker...")
mqtt_client.connect(MQTT_BROKER, 1883, 60)

# Start the loop (blocks forever)
try:
    mqtt_client.loop_forever()
except KeyboardInterrupt:
    print("\nStopping...")
    mqtt_client.disconnect()