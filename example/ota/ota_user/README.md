# Example Description

This example demonstrates image update using UART (Universal Asynchronous Receiver-Transmitter) connection. Unlike `ota_http/ota_https`, it doesn't require network connectivity. Once the board is connected to the serial port with the script executed on the PC side, the board can start receiving the image and programming. It is applicable in scenarios of user-customized upgrades and field image updates.

Download `ota_all.bin` from the download server (in `tools\DownloadServer(UART)`) automatically.

# HW Configuration

```
+------------------+          +------------------+
|                  |  UART    |                  |
|  Ameba Board     |<-------->|        PC        |
|  (RTL8730E/...   |  Serial  |   (UART Server)  |
|   RTL8726E, etc) |          |                  |
|                  |          |                  |
+------------------+          +------------------+
```

**Required Components:**
- UART adapter (USB-to-UART converter, e.g., CP2102, CH340, FT232)
- USB cable for connecting PC to UART adapter
- Jumper wires for connecting UART pins

**UART Pin Connection:**
| Ameba Board | UART Adapter |
|-------------|--------------|
| TX          | RX           |
| RX          | TX           |
| CTS         | RTS          |
| RTS         | CTS          |
| GND         | GND          |

**Connection Requirements:**
- Connect UART adapter TX/RX/CTS/RTS/GND pins to the Ameba board's UART pins
- Configure the UART PINs according to your specific board
- Connect the UART adapter to PC via USB

# SW Configuration

1. **Select Image Switch Strategy**

   Refer to [Image Pattern and Version Number](https://aiot.realmcu.com/zh/latest/rtos/ota/index.html#version-number) to select the appropriate image switch strategy for your application.

2. **Build and Flash**
   - Refer to the [SDK Examples](https://aiot.realmcu.com/en/latest/rtos/sdk/sdk_example/index.html) to generate images
   - Download images to board using Ameba Image Tool

3. **Configure UART Server**
   - Configure the script according to `tools\DownloadServer(UART)\readme.txt`
   - Set the correct serial port in `uart_user_ota.py` (e.g., COM3, /dev/ttyUSB0)
   - Execute the server script

4. **Start OTA Update**

   Reset the board to start the download through serial port

# Expected Result

After booting, a UART download example thread will be started automatically. The board will:
1. Initialize the UART interface
2. Receive the `ota_all.bin` image from the PC
3. Verify the downloaded image integrity
4. Automatically reset and boot from the new image after successful OTA

**Note:** The board will boot from the new image only if:
- The image switch strategy is set to "switch based on valid tag"
- The new image version number is greater than the current image version number

**Success Indicators:**
- Serial console shows UART initialization successful
- "OTA SUCCESS" message appears after image update
- Board automatically reboots with the new image

# Notes

- Ensure the serial port in `uart_user_ota.py` is set to the correct COM port
- The UART server script and board must be on the same system
- Ensure stable UART connection during the entire OTA process
- Avoid disconnecting the UART cable during the OTA process

# Supported IC

* RTL8730E
* RTL8726E
* RTL8720E
* RTL8713E
* RTL8710E
* RTL8721Dx
* RTL8721F
