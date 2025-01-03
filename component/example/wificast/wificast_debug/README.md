# Uart Example

This example demonstrates how to use the WIFI-CAST data to debug WIFI-CAST devices.

## Functionality

One WIFI-CAST device receives debug commands from other WIFI-CAST device through WIFI-CAST data and broadcasts the log data after executes these commands. Also, the device log data will be stored in flash and sent through WIFI-CAST data.

The debug supports all the AT commands and shell commands.

## Setup Enviornment

At least two Ameba development boards are needed. Choose one board works as the monitor device, and another board works as the monitored device.

## User-defined Frame Format

Config the user-defined data header in User Payload field.
|   Type   |   Len   | 
|:--------:|:-------:|
|  2 bytes | 2 bytes |
* Type: The Type field is set to indicate the WIFI-CAST debug data.
    ```
    #define WIFI_CAST_DEBUG_COMMAND  BIT(5)
    #define WIFI_CAST_DEBUG_LOG      BIT(4)
    ```
* Len: The Len field is set as the length of debug data.

## How to Use the Example

### Step 1: Configure the Debug Mode

For debug monitor mode, set WIFI_CAST_MONITORED to 0:

```
#define WIFI_CAST_MONITORED      0
```
For debug monitored mode, set WIFI_CAST_MONITORED to 1:
```
#define WIFI_CAST_MONITORED      1
```

### Step 2: Build & Flash

1. Navigate to the project directory according to the chip you use. For GCC enviornment, type command `make EXAMPLE=wificast_debug` to compile the example.
2. Build and download the image into flash for monitor and monitored device respectively.

### Step 3: Run Monitor and Monitored

1. Reset both monitor and monitored device.
2. The monitor inputs debug commands through the serial port by the following commands.

## Serial Port Commands

1. Scan WIFI-CAST devices.
    ```
    wificast scan
    ```
    Scan to get WIFI-CAST device's MAC address list, which will be used when send commands.

2. List WIFI-CAST devices.
    ```
    wificast addr_list
    ```
    Print out WIFI-CAST devices' MAC address list when scan is finshed.

3. Send debug commands.
    ```
    wificast command <xx:xx:xx:xx:xx:xx> <Command>
    ```
    Send debug command <Command> to device <xx:xx:xx:xx:xx:xx>.

## Example Output
* Output sample from the sending device:
```
[17:49:47:498][example_main-I] ------------->start
[17:49:47:498][WIFI CAST-I] device already connected to ssid: WDSTEST, channel: 6
[17:49:47:502][WLAN-A] En promisc 0
[17:49:47:502][WIFI CAST-I] wifi_cast_add_node, node: ff:ff:ff:ff:ff:ff, set encrypt: 0
[17:49:47:509][WIFI CAST-I] wifi_cast_add_node, ff:ff:ff:ff:ff:ff add success
[17:49:47:509][WIFI CAST-I] wifi_cast_init, success
[17:49:52:669]wificast scan
[17:50:02:496][example_main-I] scan_task, scan info num: 1
[17:50:02:497][WIFI CAST-I] wifi_cast_add_node, node: 00:e0:4c:00:03:c2, set encrypt: 0
[17:50:02:501][WIFI CAST-I] wifi_cast_add_node, 00:e0:4c:00:03:c2 add success
[17:50:02:501][example_main-I] addr_list: 
[17:50:02:501]1. 00:e0:4c:00:03:c2
[17:50:10:344]wificast command 00:e0:4c:00:03:c2 efuse rmap
[17:50:10:344][example_main-I] send command efuse to 00:e0:4c:00:03:c2
```
* Output sample from the receving device:
```
[17:50:10:350][example_main-I] recv command: efuse(11)
[17:50:10:350]efuse rmap 
[17:50:10:352]EFUSE[000]: ff ff ff 00 ff ff ff ff ff ff ff ff ff ff ff ff
[17:50:10:353]EFUSE[010]: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
[17:50:10:353]EFUSE[020]: 00 00 00 00 ff 00 fe fe fe fe fe ff ff ff ff ff
[17:50:10:353]EFUSE[030]: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
[17:50:10:353]EFUSE[040]: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
```