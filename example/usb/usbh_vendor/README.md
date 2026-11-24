# Example Description

In this application, Ameba is designed as a vendor-specific USB host as a design reference for users to design customized USB applications. The USB vendor-specific host can communicate with vendor-specific USB device as defined in `usbd_vendor` application.

Currently it is only for RD internal debug usage and not available as default.

The host runs one test sequence per boot: first an ISOC OUT + IN test, then a BULK loopback test and an INTR loopback test in parallel.

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
	[*] 	VENDOR
	```

2. Build

	```
	./ameba.py build -a usbh_vendor -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbh_vendor`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

5. Prepare the vendor device: build the `usbd_vendor` example for another Ameba board (`./ameba.py build -a usbd_vendor -p`), download it and reset that board. It starts automatically and loops back the BULK/INTR/ISOC data the host sends, no command is needed.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[VND-I] USBH VENDOR demo start
	```

2. Within 10 s (`USBH_VENDOR_READY_TIMEOUT_MS`) after reset, connect the vendor-specific USB device (another Ameba board running the `usbd_vendor` application, already powered up) to the USB port of the board with USB cable. Otherwise the test is skipped with:
	```
	[VND-E] Wait attach timeout
	[VND-E] USBH VENDOR demo aborted
	```
	Reset the host board to try again.

3. After enumeration the class opens the pipes and the test runs: the ISOC test first, then the BULK and INTR loopback tests in parallel, so their lines may interleave:
	```
	[VND-I] ATTACH
	[VEN-I] BULK EP86 MPS 512 intv 0
	...
	[VND-I] SETUP
	[VND-I] ISOC test start, times:100, size: 1024
	[VND-I] ISOC test PASS
	[VND-I] BULK loopback test start, times:100, size: 512
	[VND-I] INTR loopback test start, times:100, size: 1024
	[VND-I] BULK loopback test PASS 100/100
	[VND-I] INTR loopback test PASS 100/100
	[VND-I] USBH VENDOR demo done
	```
	The `[VEN-I] <type> EPxx MPS ...` lines list the opened pipes and depend on the device. With `USBH_VENDOR_DEBUG` set to `1` in `usbh_vendor.h`, the class also dumps the ISOC data (`[VEN-I] ISOC OUT test finish 100/100:` and `[VEN-I] ISOC IN test finish 100/100:` followed by the buffer content).

	The test PASSes when `ISOC test PASS` is printed, both loopback lines read `PASS 100/100`, the run ends with `USBH VENDOR demo done` and no `[VND-E]`/`[VEN-E]` log is reported.

	Note on `PASS a/b`: the line is printed whenever a loopback test ends, also after a failure. `a` is the configured round count and `b` is `a` minus the number of mismatched bytes, so only `b == a` with no error log is a pass. Typical failure logs:
	```
	[VND-E] BULK loopback test FAIL: <round>, <byte>, errcnt <n>
	[VND-E] INTR loopback test FAIL: <round>, <byte>, errcnt <n>
	[VND-E] BULK TX timeout
	[VND-E] BULK RX timeout
	[VND-E] INTR TX timeout
	[VND-E] INTR RX timeout
	[VND-E] ISOC TX start fail: <err>
	[VND-E] ISOC TX timeout
	[VND-E] ISOC RX timeout
	[VND-E] Wait ready timeout
	[VND-E] Device disconnect
	```

4. Hotplug (`CONFIG_USBH_VENDOR_HOT_PLUG_TEST` 1 in `example_usbh_vendor.c`, default): unplug the device, the stack is torn down and re-initialized:
	```
	[VND-I] DETACH
	[VND-I] Free heap: 0x...
	[VND-I] Reinit done
	```
	Re-plugging the device prints `ATTACH` and `SETUP` again, but the test sequence runs only once per boot; reset the host board to run it again. The `Free heap` value shall stay stable across cycles. If the stack cannot be rebuilt, `[VND-E] Reinit fail: <err>` and `[VND-E] Hotplug thread exit` are printed.

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

RTL8730E
RTL8721F
RTL8735C
