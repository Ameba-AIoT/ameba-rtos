# Example Description

This example shows how to fetch csi.

# HW Configuration

None

# SW configuration

1. Use cmd `./menuconfig.py` and select `CONFIG WIFI`->`Enable WIFI CSI`.

2. If sta mode, please modify line31 in `example_wifi_csi.c`:
	```C
	unsigned char assoc_ap_mac = <mac address of associated AP>
	```

3. How to use:
   - Run `./build.py -a wifi_csi` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

4. Enviornment setup: Wifi csi test need at least two device.
	1. `DUT_IC` as softap or softap + sta mode: another board (ameba IC as sta mode or others) for slave node must be required.
	2. `DUT_IC` as sta mode: another board (ameba IC as softap mode or realAP) for master node must be required.

5. For `DUT_IC`.
	1. reset `DUT_IC`
	2. start `DUT_IC` as softap mode[softap + sta mode] or only sta mode
	3. the csi will automatically enable after successful WiFi connection

# Expect result

After compile success, the `wifi_csi_thread` may run and enable csi func when connect ok, and will show partial csi raw data in trace tool or other uart tools at regular intervals.

1. `DUT_IC` as softap or softap + sta mode

	```
	### SOFTAP Break ###
	[CH INFO] timestamp = 8182223 us, csi data(header + raw data):
                [00]0x06004ce00028abcd
	[01]0xcfcd03004ce000e4
	[02]0x01000c0001007cd9
	[03]0xa700840010003400
	[04]0x0068000000010000
	[05]0x000e030d05010000
	[06]0xf20ef50ff910fc0f
	[07]0xed03ed06ee09f00c
	[CH INFO] ...(only show 64 bytes)
	```

2. `DUT_IC` as sta mdoe, the log is as follows,
	```
	### STA Break ###
	[CH INFO] timestamp = 7303791 us, csi data(header + raw data):
                [00]0x06004ce00028abcd
	[01]0x6f3610467cd8dce3
	[02]0x01000c00a1006f72
	[03]0x9c00440010003400
	[04]0x0068000000010000
	[05]0x08f104f000010000
	[06]0xff0004fd07f809f4
	[07]0xfbf5f9faf9fefb00
	[CH INFO] ...(only show 64 bytes)
	```

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
