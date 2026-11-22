# Example Description

In this application, Ameba is an USB DRD (dual role device) that builds several USB examples into one image and switches between them at runtime with the `AT+USBAPP` command, in any direction: device <-> host, host <-> host and device <-> device. Only one app owns the USB controller at a time.

The apps built in are the examples whose class is enabled in `prj.conf`. By default only `usbd_msc` and `usbh_msc` are enabled; the others are listed there commented out, uncomment a group to add its app.

Each app behaves as its own example; see the README of that example for its test procedure and expected log. An app that has an AT command names it `AT+USB<D|H><APP>` after itself, so no two apps share a command.

At most one network app (`usbd_cdc_ecm`, `usbd_cdc_ncm`, `usbh_cdc_ecm`, `usbh_cdc_ncm`, `usbh_composite_cdc_acm_ecm` or `usbh_wifi_bridge`) can be built into an image: each one defines the TX hook of the lwIP USB netif, so the build stops with an error when two are enabled. The ECM host class builds one of its three apps: `usbh_wifi_bridge` in ECM bridge mode, otherwise `usbh_composite_cdc_acm_ecm` when the ACM host class has its 4G dongle option on (which also leaves `usbh_cdc_acm` out), otherwise `usbh_cdc_ecm`. A stop of the network app takes the netif down (DHCP server or client stopped, link down, default route back to the Wi-Fi netif, and for the bridge the Wi-Fi STA input hook restored) before the USB stack goes away, and a later start brings it up again from scratch. `usbh_wifi_bridge` needs an SoC with `SUPPORT_USBH_ECM_BRIDGE` (such as RTL8730E and RTL8721F).

Classes with exclusive Kconfig choices can only take one value per image: the device HID type (mouse for `usbd_hid`, headset buttons for `usbd_composite_hid_uac`), the UAC version, record option and record format, and the MSC media are the ones selected in `prj.conf`.

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
			USB Mode (DRD)  --->
	()		Auto-start USB app
			*** USB Device Class Selection ***
	[*] 	MSC
				Select storage media (SD card, SDIO mode)  --->
			*** USB Host Class Selection ***
	[*] 	MSC
	```

	```
	CONFIG File System --->
	[*] SD card, SDIO mode
			File system on SD card (FATFS)  --->
	[*] USB host mass storage (FATFS)
		FATFS options  --->
			[*] FATFS Support EXFAT
	```

2. Build

	```
	./ameba.py build -a usb_drd -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usb_drd`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

	`prj.conf` of this example selects `USB Mode (DRD)` and groups the options of each app, with comments; it takes effect only when the example is built with `-p` (see step 2), which also re-applies it after an edit. Uncomment a group to add its app, comment it out to leave the app out. The class options (e.g. the MSC storage media) are those of the standalone examples, see their README.

	`CONFIG_USB_APP_DEFAULT` (`CONFIG USB` > `Auto-start USB app`) names an app started at boot, e.g. `"usbd_cdc_acm"`; leave it empty to start none.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, listing the apps built in:
	```
	[DRD-I] USB DRD demo start
	[APP-I] Apps (2):
	  usbd_msc device
	  usbh_msc host
	```

2. Commands

	| Command | Description |
	| --- | --- |
	| `AT+USBAPP=list` | List the apps built in, `*` marks the running one. |
	| `AT+USBAPP=start,<app>` | Start an app; the controller must be free. |
	| `AT+USBAPP=stop` | Stop the running app and release everything it owns. |
	| `AT+USBAPP=switch,<app>[,<ms>]` | Stop the running app, wait `ms` (0..10000, default 500) so the peer sees the disconnect, then start `app`. |
	| `AT+USBAPP=status` | Print the running app, the switch count and the free heap. |
	| `AT+USBAPP=help` | Print the usage. |

	Every command ends with `OK` or `ERROR: <code>`. App names are case-insensitive. `help` works at any time; `stop` with no app running does nothing and ends with `OK`. Start and stop print the free heap, which shall stay stable across repeated switches:
	```
	[APP-I] Start usbd_cdc_acm (device), heap 0x...
	[APP-I] Stop usbd_cdc_acm, heap 0x...->0x...
	```

3. Test flow with the default `prj.conf` (`usbd_msc` and `usbh_msc`); `usbd_msc` uses the SD card (SDIO mode) media by default, so insert an SD card, and `usbh_msc` needs a U-disk on the host port. The log of the app itself (see its README) is printed between these lines:
	```
	AT+USBAPP=start,usbd_msc
	[APP-I] Start usbd_msc (device), heap 0x...
	OK

	AT+USBAPP=start,usbh_msc
	[APP-E] usbd_msc running, stop or switch
	ERROR: -3

	AT+USBAPP=switch,usbh_msc,1000
	[APP-I] Stop usbd_msc, heap 0x...->0x...
	[APP-I] Start usbh_msc (host), heap 0x...
	OK

	AT+USBAPP=status
	[APP-I] Cur: usbh_msc, switch 1, heap 0x...
	OK

	AT+USBAPP=list
	[APP-I] Apps (2):
	  usbd_msc device
	  usbh_msc host *
	OK

	AT+USBAPP=stop
	[APP-I] Stop usbh_msc, heap 0x...->0x...
	OK
	```
	Repeat the `switch` in both directions; the switch count shall increase and the heap shall stay stable.

4. Error cases:
	| Case | Log | End |
	| --- | --- | --- |
	| `start` or `switch` with an app not built in | `[APP-E] Unknown app, try AT+USBAPP=list` | `ERROR: -2` |
	| `switch` with `ms` out of 0..10000 or not a number | `[APP-E] Invalid ms: 0..10000` | `ERROR: -2` |
	| `start` while an app runs | `[APP-E] <app> running, stop or switch` | `ERROR: -3` |
	| Unknown sub command | `[APP-E] Invalid cmd: <cmd>, try AT+USBAPP=help` | `ERROR: -2` |
	| The app start fails | `[APP-E] Start <app> fail: <ret>` | `ERROR: <ret>` |
	| The app stop fails | `[APP-E] Stop <app> fail: <ret>` | `ERROR: <ret>` |
	| `start` or `switch` after a failed stop | `[APP-E] A stop failed, reset the board` | `ERROR: -7` |
	| `CONFIG_USB_APP_DEFAULT` names an app not built in (at boot) | `[APP-E] Auto-start app <app> not built in` | - |
	| The auto-start app fails to start (at boot) | `[APP-E] Auto-start <app> fail` | - |

5. If a stop, or the rollback of a failed start, times out (an app thread does not leave in time) the app keeps its resources, `status` reports `broken` and no further app can be started until the board is reset.

# Note

- Stop the running app before using the USB verify commands (`AT+USBDVERIFY`, `AT+USBHVERIFY`, ...): they drive the controller on their own.
- Host apps need VBUS on the host port, see the board rework table above.

# Supported IC

RTL8730E
RTL8721F
RTL8735C
