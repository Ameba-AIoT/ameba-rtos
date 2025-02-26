# Example Description

Using the most common event `WIFI_EVENT_JOIN_STATUS` to show how to use wifi event.

# HW Configuration

None

# SW configuration

How to use:
   - Run `./build.py -a wifi_event` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

1. Use AT command to do wifi connect:
	1. Will show join success log if use correct connect information.
	2. Will show join fail log if use incorrect connect information.

2. Use different way to do wifi disconnect to show different types of disconnect resaon:
	1. Use `AT+WLDISCONN` to do wifi disconnect
	2. Modify AP's setting
	3. Power off AP

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
