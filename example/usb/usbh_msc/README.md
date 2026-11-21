# Example Description

In this application, Ameba is designed as an USB MSC host which can communicate with USB MSC device such as U-disk.

The example mounts a FATFS filesystem on the attached U-disk and runs a write/read performance test over `USBH_MSC_MAX_FILES` files (default 10, `TEST0.DAT` … `TEST9.DAT`), each written/read at multiple payload sizes (512 / 1024 / 2048 / 4096 bytes, `USBH_MSC_TEST_ROUNDS` rounds each).

## Hotplug modes

The behaviour after a test pass, and how a detach is handled, is selected by the `CONFIG_USBH_MSC_HOTPLUG` switch at the top of `example_usbh_msc.c`:

| Value | Mode | Behaviour |
|-------|------|-----------|
| `0` | **Replug loop** | Test the files once, then stop and wait for a physical **unplug + replug** before running again. The USB stack stays alive the whole time; the host core re-enumerates on replug. |
| `1` (default) | **Continuous** | Test the files, then repeat immediately without needing a replug. If the device is detached **during** the test, the stack is fully torn down and rebuilt, and the test resumes automatically once the device is re-attached. |

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

1. (Optional) Select the hotplug mode by editing `CONFIG_USBH_MSC_HOTPLUG` in `example_usbh_msc.c` (see the table above). Default is `1` (continuous).

2. (Optional) Set `USBH_MSC_CHECK_DATA` to `1` in `example_usbh_msc.c` to compare the read-back data with the written data for every payload size. Default is `0` (no data check, performance only).

3. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Host)  --->
	[*] 	MSC
	```
	```
	CONFIG File System --->
	[*] Enable VFS
	[*] 	USB host mass storage (FATFS)
		FATFS options  --->
			[*] FATFS Support EXFAT
	```

4. Build

	```
	./ameba.py build -a usbh_msc -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

5. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 4, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbh_msc`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

	`FATFS Support EXFAT` is needed to mount exFAT or GPT-partitioned U-disks, keep it enabled for such disks.

6. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expect result

1. Reset the board. After the file system is initialized the demo starts and waits for a device:
	```
	[MSC-I] USBH MSC demo start
	[MSC-I] Wait for device attach...
	```

2. Attach an U disk to the USB port of the board with USB OTG cable. U-disk requirements:
	- Formatted with FAT12/FAT16/FAT32, or exFAT (needs `CONFIG_FATFS_SUPPORT_EXFAT`, set in `prj.conf`).
	- MBR or GPT partition table; a GPT-partitioned disk only mounts with exFAT support enabled.
	- At least 800 KB free space. The test creates or overwrites `TEST0.DAT` … `TEST9.DAT` (80 KB each) in the root directory and leaves them on the disk.

	A disk without a FAT/exFAT volume fails to mount and the demo waits for a replug:
	```
	[MSC-E] Mount logical drive fail, rc=13
	[MSC-W] No FAT volume, replug a FAT/exFAT disk
	```

3. Ameba board recognizes the MSC device, mounts it, and runs the write/read performance test over all files. Check the log on the LOGUART console and make sure there is no USB related error reported. Example of the expected PASS log (per file, sizes repeat 512 → 4096):
	```
	[USBH-A] Dev connected
	[USBH-A] Port en
	[USBH-A] Dev attach, speed 0
	[USBH-A] Addr 1
	[USBH-A] PID 6545
	[USBH-A] VID 0930
	[USBH-A] Cfg 1
	[USBH-A] LANGID 0x0409
	[USBH-A] MFG: TOSHIBA
	[USBH-A] PROD: TransMemory
	[USBH-A] SN: C03FD5F7715FE3417000DE76
	[USBH-A] Enum done, 1 cfg
	[MSC-I] ATTACH
	[MSC-I] Max lun 1
	[MSC-I] Lun 0
	[MSC-I] SETUP
	[MSC-I] Device ready
	[MSC-I] FatFS USB W/R performance test start, free heap: 0x...
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
	[MSC-I] FatFS USB W/R performance test done, free heap: 0x...
	```
	The test PASSes when every file prints `File <drv>:/TESTn.DAT done`, the run ends with `FatFS USB W/R performance test done` and is followed by `All files done, repeat from 0` (continuous mode) or `Test done, unplug and replug to repeat` (replug loop), with no `[MSC-E]` log. `speed 0` means High Speed, `1` Full Speed.

	Typical failure logs:
	```
	[MSC-E] Wait ready timeout
	[MSC-E] Device not responding, replug
	[MSC-E] Lun 0 unusable
	[MSC-E] Mount logical drive fail, rc=<rc>
	[MSC-E] Open file fail: 0:/TESTn.DAT, rc=<rc>
	[MSC-E] W err bw=<n>, rc=<rc>
	[MSC-E] R err br=<n>, rc=<rc>
	[MSC-E] Seek err rc=<rc>
	[MSC-E] WR<size> check err: ...
	```
	`rc` is the FatFs `FRESULT`: 1 `FR_DISK_ERR`, 3 `FR_NOT_READY`, 13 `FR_NO_FILESYSTEM`. The `WR<size> check err` line only appears with `USBH_MSC_CHECK_DATA` set to `1`.

4. After a full pass, the behaviour depends on `CONFIG_USBH_MSC_HOTPLUG`:

	- **`CONFIG_USBH_MSC_HOTPLUG = 0` (replug loop):** the demo stops and waits for a physical replug. Unplug and replug the U-disk to run the test again.
		```
		[MSC-I] Test done, unplug and replug to repeat
		[MSC-I] Wait for device attach...
		```
		If the U-disk is unplugged during the test, `Test interrupted, unplug and replug to repeat` is printed instead of `Test done, ...`.

	- **`CONFIG_USBH_MSC_HOTPLUG = 1` (continuous):** the demo repeats immediately without a replug.
		```
		[MSC-I] All files done, repeat from 0
		[MSC-I] Wait for device attach...
		[MSC-I] Device ready
		...
		```
		If the U-disk is unplugged **during** the test, the stack is torn down and rebuilt; re-attach the device and the test starts again from `TEST0.DAT`:
		```
		[MSC-I] DETACH
		[MSC-W] W err: device disconnected (rc=1)
		[MSC-I] FatFS USB W/R performance test done, free heap: 0x...
		[MSC-W] I/O interrupted, wait for next attach
		[MSC-I] Wait for device attach...
		[MSC-I] Free heap: 0x...
		[MSC-I] Reinit done
		```
		Depending on where the transfer is cut, `R err: device disconnected (rc=1)` or `Device detached during test` may be printed instead of `W err: ...`, and the order of the lines may vary. If the stack cannot be rebuilt, `[MSC-E] Reinit fail` and `[MSC-E] Hotplug thread exit` are printed and the board must be reset.

# Note

- No AT command is provided; the test runs automatically on every attach.
- The test files `TEST0.DAT` … `TEST9.DAT` are not deleted after the test.

# Supported IC

RTL8730E
RTL8721F
RTL8735C
