# Example Description

This example demonstrates collecting and parsing diagnostic events between a PC and an Ameba device via MQTT. The device-side example is based on `example_wifi_diagnose_mqtt.c`.

# HW Configuration

None

# SW configuration

1. Use menuconfig and select `CONFIG APPLICATION`->`Enable diagnose`.

2. Update MQTT Broker address in `example_wifi_diagnose_mqtt.c`
   ```C
   static const char *address = "192.168.1.106";
   ```

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

In the example, MQTT task is created to present MQTT connection and message processing, including:
1. Check WIFI status and wait WIFI to be connected. MQTT will start after device connected with AP and got IP.

2. Establish TCP/IP connection with MQTT server.

3. Send a CONNECT message to server and wait for a CONNACK message from server.

4. Subscribe to `request_diag_events` topic, sending SUBSCRIBE to server and wait for SUBACK from server.

5. Periodic waiting for receiving PC requests (`AT+DIAG` command), when a response is needed, publish a message with topic `response_diag_events` to the Broker through `rtk_diag_mqtt_send`.

6. Keep alive with server.

7. If mqtt status is set to MQTT_START, the client will close the TCP/IP socket connection, and restart the session by opening a new socket to the server and issuing a CONNECT message. The client will subscribe to the topic again.


# Note

- For more MQTT details, please refer to `UM0060 Realtek Ameba-all MQTT User Guide.pdf`.
- For more R-DIAG details, please refer to the R-DIAG section of the online documentation.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
- AmebaGreen2
