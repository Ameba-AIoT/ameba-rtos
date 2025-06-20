# Example Description

This is an example to use the MQTT APIs in `example_mqtt.c`.

# HW Configuration

None

# SW configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

2. if encounter compile error:
	```
	#error "lwip_sanity_check: WARNING: TCP_WND is larger than space provided by PBUF_POOL_SIZE * (PBUF_POOL_BUFSIZE - protocol headers). If you know what you are doing, define LWIP_DISABLE_TCP_SANITY_CHECKS to 1 to disable this error"
	```
	please increase `PBUF_POOL_SIZE` in `lwipopts.h` (add definition manually if not exist):.
	```C
	#define PBUF_POOL_SIZE          20
	```

# Expect result

In the example, MQTT task is created to present MQTT connection and message processing, including:
1. Check WIFI status and wait WIFI to be connected. MQTT will start after device connected with AP and got IP.

2. Establish TCP/IP connection with MQTT server.

3. Send a CONNECT message to server and wait for a CONNACK message from server.

4. Subscribe to a topic, sending SUBSCRIBE to server and wait for SUBACK from server.

5. Publish message to server every 5 seconds.

6. Read and response message. Keep alive with server.

7. If mqtt status is set to MQTT_START, the client will close the TCP/IP socket connection, and restart the session by opening a new socket to the server and issuing a CONNECT message. The client will subscribe to the topic again.

Some strategies are used to manage connection exception
1. `Lwip_select` is used to check data arrival and connection exception. Message is read only if tcp data has arrived. If exception fd is set, MQTT will restart.

2. `SO_KEEPALIVE` and `TCP_KEEPIDLE` are set to clear TCP buffer when network is bad. For if TCP buffer is full and can't allocate more memory, the situation will last for about 20 minutes until MAX data retries reached, and then MQTT will not restart successfully during this time for allocating memory failed.

# Note

For more details, please refer to `UM0060 Realtek Ameba-all MQTT User Guide.pdf`.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
