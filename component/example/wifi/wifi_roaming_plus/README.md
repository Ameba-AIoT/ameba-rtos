# Example Description

This example realizes wifi roaming among local area network with the same SSID.

The roaming strategy is prescan before roaming and store the connected channel to flash for prescan. Also, it can store the sta info to flash for fast DHCP.

Enable `CONFIG_EXAMPLE_WLAN_FAST_CONNECT` for fast connection.

Use kv to store the AP and station infomation for roaming.

# HW Configuration

None

# SW configuration

1. If using fast DHCP, remember to enable it. Type command `./menuconfig.py` in auto_build and choose `CONFIG LWIP` -> `Enable Fast DHCP` save and exit.

2. Set the `RSSI_SCAN_THRESHOLD` and `RSSI_ROAMING_THRESHOLD` and `FIND_BETTER_RSSI_DELTA` according to the WLAN envrionment.

3. Config `MAX_CH_NUM` and `MAX_AP_NUM` for your application usage. But with more scan `MAX_CH_NUM`, pre-scan time may takes longer.

4. IF support 5G channels. enable `SUPPORT_SCAN_5G_CHANNEL` and set the `roaming_channel_plan`.

5. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.


# Expect result

The device can connect to a better RSSI AP and store the connection message to flash.

# Note

None

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
