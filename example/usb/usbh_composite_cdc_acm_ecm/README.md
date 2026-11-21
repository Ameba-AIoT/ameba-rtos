# Example Description

In this application, Ameba is designed as a USB CDC ACM + CDC ECM composite host for 4G LTE dongles: it dials the dongle with AT commands over the CDC ACM port, then gets an IP address over the CDC ECM network interface (DHCP, or the static address the dongle reports).
The topology:
```
[Ameba] ---USB Cable--- 4G dongle
```

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
	[*] 	4G LTE dongle support
	[*] CDC ECM
			Select USB Ethernet (USB Ethernet)  --->
	```
	```
	CONFIG LWIP --->
	[*] Enable LWIP USB ETHERNET
	```

2. Build

	```
	./ameba.py build -a usbh_composite_cdc_acm_ecm -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbh_composite_cdc_acm_ecm`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Supported dongles

| Dongle | VID:PID | Dial flow | IP setup |
| --- | --- | --- | --- |
| Quectel EG915 | 2C7C:0901 | `AT+QCFG="usbnet",1`, `AT+CGDCONT=1,"IP","orange.m2m.spec"`, `AT+QNETDEVCTL=1,1,1`, `AT+CGCONTRDP` | Static IP, gateway and DNS taken from `+CGCONTRDP` |
| Quectel EG91 | 2C7C:0191 | `AT+QCFG="usbnet",1`, `AT+CGDCONT=1,"IP"`, `AT+CEREG?`, `AT+qnetifmac="ecm0"` | DHCP, MAC taken from `+QNETIFMAC` |
| Fibocom LE271 | 2CB7:0D01 | `AT+GTUSBMODE=32` if not in ECM mode (the dongle reboots), then SIM, signal and PS attach checks, `AT+GTRNDIS=1,1` | DHCP |
| SIMCOM SIM767X | 05C6:9330 | SIM, signal and PS attach checks, `AT+CGACT=1,1`, `AT+NETOPEN`, `AT+USBNETIP=0` | DHCP |

A Realtek CDC ECM device (VID 0x0BDA) is accepted as well: it needs no dial, the example runs DHCP on it directly. Any other device is reported as `[COMP-E] Vid 0x<vid>/Pid 0x<pid> not match` and gets no IP.

The APN of the EG915 is hard-coded as `orange.m2m.spec` in `usbh_composite_quectel_eg915_ctrl()`; change it there for another SIM card.

# Expect result

1. Set up the test environment as the topology: insert a SIM card with data service into the dongle.

2. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[COMP-I] USBH ACM+ECM comp demo start
	[COMP-I] INIT
	[COMP-I] USB init start
	[COMP-I] Enter link status task
	```

3. Connect the 4G dongle. The example prints the dongle IDs, runs the dial flow of the dongle, and then sets up the IP address. For example with an EG91 (the dial state lines `Enter state(<n>)---...` and `ECM link is down (<n>)` show up while the dial runs):
	```
	[COMP-I] ATTACH
	[COMP-I] Example Pid 0x191/Vid 0x2c7c
	[COMP-I] USB init done
	...
	[COMP-I] MAC:xx:xx:xx:xx:xx:xx
	[COMP-I] Send request done
	[COMP-I] TRX start
	[COMP-I] Do DHCP
	[COMP-I] MAC[xx xx xx xx xx xx]
	[$]wifi got ip:"x.x.x.x"
	[COMP-I] Switch to link
	```
	The dial flow ends with a dongle specific line before `TRX start`:

	| Dongle | Last dial log | IP setup log |
	| --- | --- | --- |
	| Quectel EG915 | `[COMP-I] IP Ready`, `[COMP-I] Get all network info` | `[COMP-I] Set static IP`, `[COMP-I] Run usbh_composite_dongle_netif_init ok` (no `wifi got ip`) |
	| Quectel EG91 | `[COMP-I] Send request done` | `[COMP-I] Do DHCP`, `[$]wifi got ip:"x.x.x.x"` |
	| Fibocom LE271 | `[COMP-I] LE271 has IP, dial OK` | `[COMP-I] Do DHCP`, `[$]wifi got ip:"x.x.x.x"` |
	| SIMCOM SIM767X | `[COMP-I] SIM767X dial OK, waiting for ECM link-up + DHCP` | `[COMP-I] Do DHCP`, `[$]wifi got ip:"x.x.x.x"` |

	The test FAILs if `[COMP-E] Dongle diag fail` (the dial did not finish, the AT port stays usable for debugging) or `[COMP-E] DHCP fail` / `[COMP-E] Static IP fail` is printed. An LE271 that is switched to ECM mode prints `LE271 not in ECM mode, switch and reboot` and dials again after it re-enumerates.

4. After getting the IP address, type command `AT+PING=<host_ip>` (e.g. a public DNS server), make sure the ping succeeds without packet loss, i.e. the result shows `0% packet loss`.

5. The example also supports throughput tests with iperf, run as described in the "Throughput test" section of `example/usb/usbh_cdc_ecm/README.md`, with a server reachable from the cellular network. With iperf3 (`AT+IPERF3`), set `-l` on the client side, 1 to 2920 bytes, 1460 recommended: a PC client without it is refused by the board as server, the board as client defaults to 1460 bytes.

6. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the dongle, following log shall be printed for each cycle, followed by the dial and IP log of step 3:
	```
	[COMP-I] DETACH
	[COMP-I] DEINIT
	[COMP-I] Free heap: 0x<value>
	[COMP-I] INIT
	[COMP-I] ATTACH
	[COMP-I] Example Pid 0x<pid>/Vid 0x<vid>
	...
	[COMP-I] TRX start
	[COMP-I] Reinit done
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak. `[COMP-E] Reinit fail, wait for next detach` means the dial failed after the re-init.

	When the host core resets or power-cycles the dongle by itself (e.g. after a transfer error, or `AT+USBHACMECM=hotplug`), the stack is kept and only the dial is redone: `DETACH` is followed by the dial log and `[COMP-I] Reset done`, without `DEINIT`/`Free heap`/`INIT`.

# Shell commands

The 4G dongle (CDC ACM + ECM) host is driven with `AT+USBHACMECM`. Type the AT commands on the LOGUART console. Parameters are comma separated; the command name is case-sensitive, the parameters are not. `AT+USBHACMECM=help` prints the usage, every command ends with `OK` or `ERROR: <code>`. Except `help`, the commands are refused with `Not running` while the example is not running.

| Command | Description |
| --- | --- |
| `AT+USBHACMECM=help` or `AT+USBHACMECM=?` | Print the usage. |
| `AT+USBHACMECM=send,<at>` | Send one AT command, CR LF appended, to the 4G dongle over its CDC ACM port, e.g. `AT+USBHACMECM=send,AT+CSQ`. `<at>` is taken exactly as typed, commas included, e.g. `AT+USBHACMECM=send,AT+CGDCONT=1,"IP","cmnet"`. At most 1022 characters, and the whole line has to fit the 127-byte console buffer. Only for Quectel, Fibocom LE271 and SIMCOM SIM767X dongles; the send is retried up to 10 times at 1 s interval, so the console can block for about 10 s. The command logs `At cmd(<at>)`, `Pre AtCmd` and `AT cmd(<at>) done`; the reply of the dongle is printed at debug level only. |
| `AT+USBHACMECM=hotplug` | Have the host core power-cycle the dongle through VBUS (`usbh_power_cycle()`): the dongle drops off the bus and is enumerated again, the USB stack is kept. Needs a set-up dongle. Prints `[COMP-W] Not support` and `OK` unless `CONFIG_USBH_COMP_GPIO_POWER_CTRL` is set to 1. It is a `#define` in `example_usbh_composite_cdc_acm_ecm.c` (default 0), not a Kconfig option; when set, the power-control GPIO `CONFIG_USBH_COMP_USB_DEV_PWR_CTRL_GPIO` (`_PA_3`) is registered as the VBUS switch of the host core, which then also power-cycles a dongle that no longer answers a port reset. |

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

RTL8730E
RTL8721F

