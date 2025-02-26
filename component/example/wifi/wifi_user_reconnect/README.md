# Example Description

This example shows how to customize wifi reconnect task. Realtek auto reconnect and fast reconnect has been disabled in this example.

# HW Configuration

None

# SW configuration

1. Set `test_ssid` and `test_password` in `example_wifi_user_reconnect.c`.

2. How to use:
   - Run `./build.py -a wifi_user_reconnect` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

3. Modify AP's channel, Ameba will detect AP change and reconnect success.

4. Modify AP's password, Ameba will detect AP change and reconnect fail.

# Expect result

The example thread will connect `test_ap` after wifi inited, and do auto reconnect after disconnected.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
