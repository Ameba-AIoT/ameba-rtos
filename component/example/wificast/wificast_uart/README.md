# Uart Example

This example demonstrates how to use the WIFI-CAST data to communicate between devices.

## Functionality

One device receives data through the user-defined serial port and broadcasts to other devices through WIFI-CAST data. Other devices will output the data through the loguart when receive the data.

## Setup Enviornment

At least two Ameba development boards are needed. Choose one board to connect user-defined serial port with a USB-UART adapter and works as the sender, and another board works as the receiver.

> Note: The role of `sender` and `receiver` will be on the devices at the same time. When user connect user-defined serial port with a USB-UART adapter on one device, this device acts as sender and sends data to other devices. The device receives the data will act as receiver. 

## User-defined Frame Format

Config the user-defined data header in User Payload field.
|   Type   |   Len   | 
|:--------:|:-------:|
|  2 bytes | 2 bytes |
* Type: The Type field is set to indicate the WIFI-CAST uart data.
    ```
    #define WIFI_CAST_UART_DATA		BIT(13)
    ```
* Len: The Len field is set as the length of uart data.

## How to Use the Example

### Step 1: Config the Serial Port

The default PIN in the example is set as below, change them if necessary.
```
#define UART_TX_PIN    PA_28
#define UART_RX_PIN    PA_29
```
The default baudrate is set to 1500000 bps, change it if necessary.
```
#define UART_BAUD_RATE 1500000
```
Ther default buffer size is set to 200, change it if necessary.
```
#define UART_RX_BUF_SZ	200
```

### Step 2: Connect the Serial Port

Choose one board as the sender and connect the user-defined serial port with PC serial terminal through an USB-UART adapter.

### Step 3: Build & Flash

1. Navigate to the project directory according to the chip you use. For GCC enviornment, type command `make EXAMPLE=wificast_uart` to compile the example.
2. Download the image into flash both sender and receiver.

### Step 4: Run Sender and Receiver

1. Reset both sender and receiver.
2. The sender inputs data through the user-defined serial port.

## Example Output
* Output sample from the receving device:
```
[14:23:36:906][example_main-I] ------------->start
[14:23:36:906][WIFI CAST-I] device already connected to ssid: WDSTEST, channel: 6
[14:23:36:908][WLAN-A] En promisc 0
[14:23:36:910][WIFI CAST-I] wifi_cast_add_node, node: ff:ff:ff:ff:ff:ff, set encrypt: 0
[14:23:36:910][WIFI CAST-I] wifi_cast_add_node, ff:ff:ff:ff:ff:ff add success
[14:23:36:910][WIFI CAST-I] wifi_cast_init, success
[14:23:36:910][SERIAL-I] uart idx: 0
[14:23:36:914][example_main-I] example_uart_read_task, enter
[14:23:45:504][example_main-I] 00:e0:4c:00:03:c2, rssi: -16, recv count: 1, size: 194, data: 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
[14:23:45:513]
[14:24:00:880][example_main-I] 00:e0:4c:00:03:c2, rssi: -16, recv count: 2, size: 194, data: 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
[14:24:00:891]
```
* Output sample from the sending device:
```
[14:23:38:521][example_main-I] ------------->start
[14:23:38:521][WIFI CAST-I] device already connected to ssid: WDSTEST, channel: 6
[14:23:38:523][WLAN-A] En promisc 0
[14:23:38:523][WIFI CAST-I] wifi_cast_add_node, node: ff:ff:ff:ff:ff:ff, set encrypt: 0
[14:23:38:524][WIFI CAST-I] wifi_cast_add_node, ff:ff:ff:ff:ff:ff add success
[14:23:38:524][WIFI CAST-I] wifi_cast_init, success
[14:23:38:524][SERIAL-I] uart idx: 0
[14:23:38:529][example_main-I] example_uart_read_task, enter
[14:23:45:495][example_main-I] send count: 1, size: 194, data: 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
[14:23:45:503]
[14:24:00:881][example_main-I] send count: 2, size: 194, data: 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
[14:24:00:888]
```