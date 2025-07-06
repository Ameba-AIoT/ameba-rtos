# Example Description

This example shows how to customize wifi reconnect task. Realtek auto reconnect and fast reconnect has been disabled in this example.

# HW Configuration

None

# SW configuration

1. In `component/soc/usrcfg/amebaxxx/ameba_wificfg.c`.
	```C
	wifi_user_config.fast_reconnect_en = 0;
   wifi_user_config.auto_reconnect_en = 0;
	```

2. Set `test_ssid` and `test_password` in `example_wifi_user_reconnect.c`.

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

4. Modify AP's channel, Ameba will detect AP change and reconnect success.

5. Modify AP's password, Ameba will detect AP change and reconnect fail.

# Expect result

The example thread will connect `test_ap` after wifi inited, and do auto reconnect after disconnected.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
