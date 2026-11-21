# Example Description

In this application, Ameba is designed as an USB CDC ACM host which can communicate with specific USB CDC ACM device as defined in `usbd_cdc_acm` application.

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

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Host)  --->
	[*] CDC ACM
	```

2. Build

	```
	./ameba.py build -a usbh_cdc_acm -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbh_cdc_acm`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expect result

1. Prepare the peer device: build the `usbd_cdc_acm` example (`./ameba.py build -a usbd_cdc_acm -p`) and download it to another Ameba board. It echoes back every bulk OUT transfer, which the loopback test of this example relies on.

2. Reset the host board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[ACM-I] USBH CDC ACM demo start
	[ACM-I] INIT
	[ACM-I] Wait for device attach
	```

3. Connect the USB port of the peer board to the USB port of the host board with USB cable. Each enumerated device runs one test round: the control test (GET/SET/GET_LINE_CODING) and a bulk loopback test of 100 transfers of 1024 bytes. If the test runs successfully, the console will show (the first `DteRate` is the 115200 the host sets during the class setup):
	```
	[ACM-I] ATTACH
	[ACM-I] SETUP
	[ACM-I] Control test start
	[ACM-I] GET_LINE_CODING:DteRate: 115200
	CharFormat: 0
	ParityType: 0
	DataBits: 8
	[ACM-I] SET_LINE_CODING:
	DteRate: 38400
	CharFormat: 0
	ParityType: 0
	DataBits: 8
	[ACM-I] GET_LINE_CODING:DteRate: 38400
	CharFormat: 0
	ParityType: 0
	DataBits: 8
	[ACM-I] CTRL test PASS
	[ACM-I] Bulk loopback test start, times:100, size: 1024
	[ACM-I] Bulk loopback test PASS
	[ACM-I] Test done, wait for device re-attach
	```
	The test FAILs if any of the following is printed: `CTRL test FAIL`, `SET_LINE_CODING fail`, `TX fail: <err>`, `TX timeout`, `RX fail: <err>`, `RX timeout`, `NOT ready for CTRL test`, `Device not ready`, or `Device disconnect` while the device is still plugged in.

4. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle, followed by a new test round as in step 3:
	```
	[ACM-I] DETACH
	[ACM-I] DEINIT
	[ACM-I] Free heap: 0x<value>
	[ACM-I] INIT
	[ACM-I] Reinit done
	[ACM-I] ATTACH
	[ACM-I] SETUP
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

# Note

The interrupt IN (notify) test is disabled by default. To run it, enable `Notification support` under `CDC ACM` (`CONFIG_USBH_CDC_ACM_NOTIFY_MENU=y`) as described in step 3 of `SW configuration`; each round then waits for 100 notifications, 2 s each, and prints `[ACM-I] Intr notify test PASS` or `[ACM-I] Intr notify test FAIL`. The device has to keep sending notifications for this test: the `usbd_cdc_acm` peer sends only one SERIAL_STATE, when DTR is set.

# Supported IC

RTL8730E
RTL8721F
RTL8735C
