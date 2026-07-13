# Example Description

In this application, Ameba is designed as an USB MSC host which can communicate with USB MSC device such as U-disk.

The example mounts a FATFS filesystem on the attached U-disk and runs a write/read performance test over `USBH_MSC_MAX_FILES` files (default 10, `TEST0.DAT` … `TEST9.DAT`), each written/read at multiple payload sizes (512 / 1024 / 2048 / 4096 bytes, `USBH_MSC_TEST_ROUNDS` rounds each).

## Hotplug modes

The behaviour after a test pass, and how a detach is handled, is selected by the `CONFIG_USBH_MSC_HOTPLUG` switch at the top of `example_usbh_msc.c`:

| Value | Mode | Behaviour |
|-------|------|-----------|
| `0` (default) | **Replug loop** | Test the files once, then stop and wait for a physical **unplug + replug** before running again. The USB stack stays alive the whole time; the host core re-enumerates on replug. |
| `1` | **Continuous** | Test the files, then repeat immediately without needing a replug. If the device is detached **during** the test, the stack is fully torn down and rebuilt, and the test resumes automatically once the device is re-attached. |

# HW Configuration

Some Ameba boards require hardware rework to support USB Host mode.
Populate the designated resistor footprint with a **0Ohm resistor** (or apply a solder bridge)
to enable VBUS power supply to the USB device.
Refer to the [EVB User Guide](https://aiot.realmcu.com/filelist?document_type=9) for details.

## Board Rework Reference

### RTL8730E

| Board Variant      | Resistor | Rework Action                                  |
|--------------------|----------|------------------------------------------------|
| RTL8730EA / QFN100 | R20      | Populate with 0Ohm resistor or solder bridge     |
| RTL8730EL / QFN144 | R166     | Populate with 0Ohm resistor, or use an OTG cable |

### RTL8721F

| Board Variant    | Resistor | Rework Action                              |
|------------------|----------|--------------------------------------------|
| RTL8721FC0-R00   | R52      | Populate with 0Ohm resistor                 |
| RTL8721FA0-3V0   | R52      | Populate with 0Ohm resistor                 |
| RTL8721FL0-2V0   | R2       | Populate with 0Ohm resistor *(pre-reworked by default)* |

# SW configuration

1. Menuconfig
	Use menuconfig and:
	- Choose `CONFIG USB --->`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] 	MSC
	```
	- Choose `CONFIG VFS --->`:
	```
	[*] Enable VFS FATFS
	```
	Save and exit.

2. (Optional) Select the hotplug mode by editing `CONFIG_USBH_MSC_HOTPLUG` in `example_usbh_msc.c` (see the table above). Default is `0` (replug loop).

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board. After the file system is initialized the demo starts and waits for a device:
	```
	[MSC-I] USBH MSC demo start
	[MSC-I] Wait for device attach...
	```

2. Attach an U disk formatted with FATFS to the USB port of the board with USB OTG cable.

3. Ameba board recognizes the MSC device, mounts it, and runs the write/read performance test over all files. Check the log on the LOGUART console and make sure there is no USB related error reported. Example of the expected PASS log (per file, sizes repeat 512 → 4096):
	```
	[USBH-A] Device attached,speed 0
	[USBH-A] PID: 0x6545
	[USBH-A] VID: 0x0930
	[USBH-A] Address 1 assigned
	[USBH-A] MFG: TOSHIBA
	[USBH-A] PROD: TransMemory
	[USBH-A] SN: C03FD5F7715FE3417000DE76
	[USBH-A] Enum done, total 1 cfg
	[USBH-A] Switch to itf: 0
	[USBH-A] Class: 0x08
	[USBH-A] SubClass: 0x06
	[USBH-A] Protocol: 0x50
	[MSC-I] ATTACH
	[MSC-I] Max lun 1
	[MSC-I] Lun 0
	[MSC-I] SETUP
	[MSC-I] Device ready
	[MSC-I] Free heap: 0x...
	[MSC-I] Open file: 0:/TEST0.DAT
	[MSC-I] W test: size 512, round 20...
	[MSC-I] W rate 204.0 KB/s for 20 round @ 49 ms
	[MSC-I] R test: size = 512 round = 20...
	[MSC-I] R rate 476.1 KB/s for 20 round @ 21 ms
	[MSC-I] W test: size 1024, round 20...
	[MSC-I] W rate 540.5 KB/s for 20 round @ 37 ms
	[MSC-I] R test: size = 1024 round = 20...
	[MSC-I] R rate 800.0 KB/s for 20 round @ 25 ms
	[MSC-I] W test: size 2048, round 20...
	[MSC-I] W rate 655.7 KB/s for 20 round @ 61 ms
	[MSC-I] R test: size = 2048 round = 20...
	[MSC-I] R rate 1212.1 KB/s for 20 round @ 33 ms
	[MSC-I] W test: size 4096, round 20...
	[MSC-I] W rate 1095.8 KB/s for 20 round @ 73 ms
	[MSC-I] R test: size = 4096 round = 20...
	[MSC-I] R rate 1600.0 KB/s for 20 round @ 50 ms
	[MSC-I] File 0:/TEST0.DAT done
	...
	[MSC-I] File 0:/TEST9.DAT done
	[MSC-I] Free heap: 0x...
	```

4. After a full pass, the behaviour depends on `CONFIG_USBH_MSC_HOTPLUG`:

	- **`CONFIG_USBH_MSC_HOTPLUG = 0` (replug loop):** the demo stops and waits for a physical replug. Unplug and replug the U-disk to run the test again.
		```
		[MSC-I] Test done, unplug and replug to repeat
		[MSC-I] Wait for device attach...
		```

	- **`CONFIG_USBH_MSC_HOTPLUG = 1` (continuous):** the demo repeats immediately without a replug.
		```
		[MSC-I] All files done, repeat from 0
		[MSC-I] Wait for device attach...
		[MSC-I] Device ready
		...
		```
		If the U-disk is unplugged **during** the test, the stack is torn down and rebuilt; re-attach the device and the test resumes:
		```
		[MSC-W] W err: device disconnected (rc=1)
		[MSC-I] DETACH
		[MSC-W] I/O interrupted, wait for next attach
		[MSC-I] Hotplug: deinit USB stack
		[MSC-I] Hotplug: USB stack re-initialized
		[MSC-I] Wait for device attach...
		```

# Note

None

# Supported IC

RTL8730E
RTL8721F
