

### Sample application code for demonstrating streaming IoT Data to InfluxDB

**Presentation**: https://www.slideshare.net/slideshow/ml-data-processing-for-industrial-iot-with-influxdb/271917286

**Contents**: Work in Progress

1. **pir-detector-influxdb-arduino**: Arduino sketch for Arduino R4(WiFi) board that uses MQTT protocol, PIR motion sensor connected to LED light that detects when PIR sensor is triggerred and saves that information in InfluxDB arduino bucket via Telegraf.
2. **joystick-raspberrypi** : Python code for RaspebrryPi connected to Joystick with ADC sending positional information to InfluxDB Cloud.
3. **telegraf.conf**: Telegraf configuration file using MQTT to connect Arduino to InfluxDB Cloud

**Diagram for hardware and overall system**

![Diagram for system](https://github.com/InfluxCommunity/IoT-Sensor-Demo/blob/main/diagram.png)
