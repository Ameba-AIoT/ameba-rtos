# Example Description

A simple websocket server example which echo all the messages.

# HW Configuration

None

# SW configuration

1. Both WS server and WSS server are supported by this exmaple, and can be changed by modifying `USE_WSS` in `example_websocket_server.c`.
   - `example_websocket_server.c`
		```C
		#define USE_WSS    1 //acting as the WSS server
		#define USE_WSS    0 //acting as the WS server
		```

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

Can make automatical Wi-Fi connection when booting by using wlan fast connect example.

A websocket server example thread will be started automatically when booting.

# Note

None

# Supported IC

- AmebaSmart
  * RTL8730E
- AmebaLite
  * RTL8726E
  * RTL8720E
  * RTL8713E
  * RTL8710E
- AmebaDplus
  * RTL8721Dx
- AmebaGreen2
  * RTL8721F
