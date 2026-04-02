# Example Description

This example demonstrates image update using Over-the-Air (OTA) programming over HTTP protocol via Wireless Network.

The Ameba board downloads `ota_all.bin` from the HTTP download server (in `tools\DownloadServer(HTTP)`) automatically.

# HW Configuration

```
+------------------+          +------------------+           +------------------+
|                  |          |                  |  WiFi/    |                  |
|  Ameba Board     |   WiFi   |   WiFi Router    |  Ethernet |        PC        |
|  (RTL8730E/...   |<-------->|   (Same LAN)     |<--------->|   (HTTP Server)  |
|   RTL8726E, etc) |          |                  |           |                  |
|                  |          |                  |           |                  |
+------------------+          +------------------+           +------------------+
```

**Connection Requirements:**
- Ameba board connects to WiFi router via WiFi
- PC connects to the same router via WiFi or Ethernet (both must be on the same LAN)
- WiFi SSID and password need to be configured in the board
- HTTP server script should be executed on PC before board reset

# SW Configuration

1. **Configure HTTP Server Settings**

   Modify `PORT`, `HOST` and `RESOURCE` in `example_ota_http.c` based on your download server settings:
   ```C
   #define PORT   8082
   static const char *HOST = "192.168.3.37";
   static const char *RESOURCE = "ota_all.bin";
   ```
   **Note:** Replace the IP address with your PC's actual IP address on the local network.

2. **Select Image Switch Strategy**

   Refer to [Image Pattern and Version Number](https://aiot.realmcu.com/zh/latest/rtos/ota/index.html#version-number) to select the appropriate image switch strategy for your application.

3. **Build and Flash**
   - Refer to the [SDK Examples](https://aiot.realmcu.com/en/latest/rtos/sdk/sdk_example/index.html) to generate images
   - Download images to board using Ameba Image Tool

4. **Setup HTTP Server**
   - Copy `ota_all.bin` into `tools\DownloadServer(HTTP)` folder
   - Configure the server as HTTP server according to `tools\DownloadServer(HTTP)\readme.txt`
   - Execute the server script
   - Keep the server running during the OTA process

5. **Start OTA Update**

   Reset the board to start the download

# Expected Result

After booting, an HTTP download example thread will be started automatically. The board will:
1. Connect to the configured WiFi network using the provided SSID and password
2. Connect to the HTTP server on the PC (must be running before board reset)
3. Download the `ota_all.bin` image from the server
4. Verify the downloaded image integrity
5. Automatically reset and boot from the new image after successful OTA

**Note:** The board will boot from the new image only if:
- The image switch strategy is set to "Valid Header Based Switch"
- The new image version number is greater than the current image version number when the image switch strategy is set to "Version Number Based Switch"

**Success Indicators:**
- Serial console shows successful WiFi connection
- HTTP download progress is displayed
- "OTA SUCCESS" message appears after image update
- Board automatically reboots with the new image

# Notes

- The HTTP server script and board must be on the same local area network
- Ensure the HTTP server is running before resetting the board
- The board will automatically start downloading after booting if network and server are ready
- If the board fails to connect to the router automatically, use the AT command `AT+WLCONN=ssid,<ssid>,pw,<password>` to connect manually

# Supported IC

* RTL8730E
* RTL8726E
* RTL8720E
* RTL8713E
* RTL8710E
* RTL8721Dx
* RTL8721F
