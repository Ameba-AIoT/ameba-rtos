# Example Description

This example shows how R-mesh Provisioning Protocol (RPP) works. Realtek fast reconnect has been disabled in this example.

# HW Configuration

None

# SW configuration

1. In `component/soc/usrcfg/amebaxxx/ameba_wificfg.c`.
	```C
	wifi_user_config.fast_reconnect_en = 0;
   wifi_user_config.wtn_en = 1;
	```

2. Use menuconfig and select `CONFIG BT`->`Enable BT`->`BT Example Demo`->`BLE WiFiMate Device` & `BLE WiFiMate Configurator`, `CONFIG WIFI`->`Enable Wifi`->`Enable RMESH` & `Enable RMESH Socket`.

3. How to use:
   - Run `./ameba.py build -a wifi_rpp` under sdk path to generate images.
   - `Download` images to 2 boards by Ameba Image Tool.

4. Use the wifi provisioning mobile App (located at `{sdk}/tools/R-Mesh_Demo_Tool/MGravitation.apk`) to connect one of the boards to AP. See instructions on https://aiot.realmcu.com/en/latest/rtos/rmesh/zrpp/index.html#phase-i-assisted-provisioning.

5. The other board will connect to AP automatically by Zero R-mesh Provisioning Protocol (ZRPP).

# Expect result

Two boards will finally both be connected to AP.

# Note

None

# Supported IC

RTL8721Dx
RTL8721F
