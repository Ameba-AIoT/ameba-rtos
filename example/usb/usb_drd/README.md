# Example Description

In this application, Ameba is an USB DRD (dual role device) that builds several USB examples into one image and switches between them at runtime with the `AT+USBAPP` command, in any direction: device <-> host, host <-> host and device <-> device. Only one app owns the USB controller at a time.

The apps built in are the examples whose class is enabled in `prj.conf`. By default only `usbd_msc` and `usbh_msc` are enabled; the others are listed there commented out, uncomment a group to add its app:

| App | Role | Peer |
|---|---|---|
| `usbd_cdc_acm` | Device | PC or a board running `usbh_cdc_acm` |
| `usbd_vendor` | Device | A board running `usbh_vendor` |
| `usbd_dfu` | Device | PC (`dfu-util`) or a board running `usbh_dfu` |
| `usbd_msc` | Device | PC or a board running `usbh_msc` |
| `usbd_hid` | Device | PC (HID mouse) |
| `usbd_uvc` | Device | PC (UVC camera viewer) |
| `usbd_composite_cdc_acm_hid` | Device | PC |
| `usbd_composite_cdc_acm_msc` | Device | PC |
| `usbd_cdc_ecm` | Device | PC or a board running `usbh_cdc_ecm` |
| `usbd_cdc_ncm` | Device | PC or a board running `usbh_cdc_ncm` |
| `usbd_uac` | Device | PC (USB speaker) |
| `usbd_composite_cdc_acm_uac` | Device | PC |
| `usbd_composite_hid_uac` | Device | PC |
| `usbh_cdc_acm` | Host | A board running `usbd_cdc_acm` |
| `usbh_vendor` | Host | A board running `usbd_vendor` |
| `usbh_dfu` | Host | A board running `usbd_dfu` |
| `usbh_msc` | Host | U-disk, or a board running `usbd_msc` |
| `usbh_hid` | Host | HID device (headset with media keys) |
| `usbh_cdc_ecm` | Host | ECM dongle, or a board running `usbd_cdc_ecm` |
| `usbh_cdc_ncm` | Host | NCM dongle, or a board running `usbd_cdc_ncm` |
| `usbh_uac` | Host | USB headset or speaker |
| `usbh_uvc` | Host | USB camera |
| `usbh_composite_hid_uac` | Host | USB headset with media keys |
| `usbh_composite_uvc_uac` | Host | USB camera with microphone |
| `usbh_composite_cdc_acm_ecm` | Host | 4G LTE dongle (Quectel, Fibocom LE271, SIMCom SIM767X) |
| `usbh_wifi_bridge` | Host | ECM dongle, bridged to the Wi-Fi STA |

Each app behaves as its own example; see the README of that example for its test procedure and expected log. Each app has its own AT command, named `AT+USB<D|H><APP>` after it (e.g. `AT+USBDHID` of `usbd_hid`, `AT+USBDACMHID` of `usbd_composite_cdc_acm_hid`, `AT+USBHUAC` of `usbh_uac`, `AT+USBHACMECM` of `usbh_composite_cdc_acm_ecm`), so no two apps share a command. It is accepted only while that app runs; otherwise it prints `Not running` and ends with `ERROR`.

At most one network app (`usbd_cdc_ecm`, `usbd_cdc_ncm`, `usbh_cdc_ecm`, `usbh_cdc_ncm`, `usbh_composite_cdc_acm_ecm` or `usbh_wifi_bridge`) can be built into an image: each one defines the TX hook of the lwIP USB netif, so the build stops with an error when two are enabled. The ECM host class builds one of its three apps: `usbh_wifi_bridge` in ECM bridge mode, otherwise `usbh_composite_cdc_acm_ecm` when the ACM host class has its 4G dongle option on (which also leaves `usbh_cdc_acm` out), otherwise `usbh_cdc_ecm`. A stop of the network app takes the netif down (DHCP server or client stopped, link down, default route back to the Wi-Fi netif, and for the bridge the Wi-Fi STA input hook restored) before the USB stack goes away, and a later start brings it up again from scratch. `usbh_wifi_bridge` needs an SoC with `SUPPORT_USBH_ECM_BRIDGE` (RTL8730E, RTL8721F).

Classes with exclusive Kconfig choices can only take one value per image: the device HID type (mouse for `usbd_hid` and `usbd_composite_cdc_acm_hid`, vendor for `usbd_composite_hid_uac`), the UAC version, and the MSC media are the ones selected in `prj.conf`.

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

	`prj.conf` of this example selects `USB Mode (DRD)` and groups the options of each app, with comments; it is applied automatically when building this example. Uncomment a group to add its app, comment it out to leave the app out. The class options (e.g. the MSC storage media) are those of the standalone examples, see their README.

	`CONFIG_USB_APP_DEFAULT` (`CONFIG USB` > `Auto-start USB app`) names an app started at boot, e.g. `"usbd_cdc_acm"`; leave it empty to start none.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

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

	Every command ends with `OK` or `ERROR: <code>`. Start and stop print the free heap, which shall stay stable across repeated switches:
	```
	[APP-I] Start usbd_cdc_acm (device), heap 0x...
	[APP-I] Stop usbd_cdc_acm, heap 0x...->0x...
	```

3. If a stop, or the rollback of a failed start, times out (an app thread does not leave in time) the app keeps its resources, `status` reports `broken` and no further app can be started until the board is reset.

# Note

- Stop the running app before using the USB verify commands (`AT+USBDVERIFY`, `AT+USBHVERIFY`, ...): they drive the controller on their own.
- Host apps need VBUS on the host port, see the board rework table above.

# Supported IC

RTL8730E
RTL8721F
RTL8735C
