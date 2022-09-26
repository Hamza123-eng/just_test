# Linkie-Firmware
Repo for sharing and development of firmware with those outside of the company

## Files

### end_to_end_protoypye.ino

This contains the main logic of the repo. It controls the functionality of the esp32, including connecting to WiFi, storing in memory, playing audio and handling button presses.

It also includes the `messageReceived` function that handles receiving a new MQTT message from Google Cloud.

### esp32-mqtt.h

The devices connects to `Google Cloud IoT Core` and uses this library to do so. 

### ciotc_config.h

This files contains import information to allow the device to connect to the `Google Cloud IoT Core`. This is where you will set your wifi credentials and IoT credentials.
