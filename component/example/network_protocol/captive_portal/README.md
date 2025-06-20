# Example Description

A simple server which provides captive portal page.

# HW Configuration

None

# SW configuration

1. Expand `MEMP_NUM_NETBUF` to support for more connections.
   - `lwipopts.h` (add definition manually if not exist):
		```C
		#define MEMP_NUM_NETBUF                 20
		```

2. Enable captive portal.
   - `dhcps.h`:
		```C
		#define CONFIG_ENABLE_CAPTIVE_PORTAL  1
		```

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

A softap thread will be started automatically when booting.

After the phone connects to the softap `(SOFTAP_SSID: captive_portal_test, SOFTAP_PASSWORD:12345678)`, a captive portal page will automatically appear, which can display information of softap and scan available wifi.

Several buttons are provided.
   - `Submit` button is used to restart softap when you change the setting.
   - `Scan` button is used to scan available wifi.
   - `Connect` button is used to connect target wifi according to your input.
   - `Refresh` button is used to refresh page.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
- AmebaGreen2
