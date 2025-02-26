# Example Description

This example realizes wifi roaming among local area network with the same SSID.

# HW Configuration

None

# SW configuration

1. set `wifi_fast_connect_enable(1)` in `wlan_initialize()`

2. Set the `RSSI_THRESHOLD` in `example_wifi_roaming.c` according to the WLAN envrionment.

3. How to use:
   - Run `./build.py -a wifi_roaming` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.


# Expect result

1. The device can connect to a better RSSI AP and store the connection message to flash.

2. If the RSSI of connected AP is lower than `RSSI_THRESHOLD`, the roaming decision is triggered to begin to detect whether there is a better AP.

3. If there is an alternative AP with a RSSI higher than already connected AP, it is suggested to switch to that AP.

4. If the RSSI of connected AP is already stronger than `RSSI_THRESHOLD`, then it will not automatically switch to even better AP unless disconnect or reboot from current AP connection.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
