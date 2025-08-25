# Uart Example

This example demonstrates how to use the WIFI-CAST data to process interaction between devices.

## Functionality

By using shell command through loguart and selecting one GPIO as LED on the development board, the controller device sends the control commands by shell commands through WIFI-CAST data and responser devices change the LED status when receive the control commands.

## Setup Enviornment

At least two Ameba development boards are needed. Choose one board works as the controller, and another board selecting one GPIO as LED works as the responser.

> Note: The role of `controller` and `responser` will be on the devices at the same time. When user start the bind command on one device, this device acts as controller and sends control commands to other devices. The device processs the control commands will act as responser. 

## User-defined Frame Format

Config the user-defined data header in User Payload field.
|   Type   |   Len   | 
|:--------:|:-------:|
|  2 bytes | 2 bytes |
* Type: The Type field is set to indicate the WIFI-CAST control data.
    ```
    #define WIFI_CAST_CONTROL_LIGHT		BIT(15)
    #define WIFI_CAST_CONTROL_BIND		BIT(14)
    ```
* Len: The Len field is set as the length of control data.

## How to Use the Example

### Step 1: Config the GPIO

The default GPIO in the example is set to PA_22, change it if necessary.
```
#define LED_PIN	PA_22
```
Connect a LED with this GPIO to show the status.

### Step 2: Build & Flash

1. Disable wifi fast connection in component/soc/usrcfg/xxxx/ameba_wificfg.c
    ```
    wifi_user_config.fast_reconnect_en = 0;
    ```
2. Build images by command `./build.py -a wificast_control` and download images to board by Ameba Image Tool for both controller and responser.

### Step 3: Run Controller and Responser

1. Reset both controller and responser.
2. The controller inputs shell commnad to send bind command.
    ```
    wificast bind
    ```
    * Output sample from the controller:
    ```
    [14:42:09:701]wifi_cast bind
    [14:42:09:701][example_main-I] example_control_bind, status: bind
    ```
    * Output sample from the responser:
    ```
    [14:47:02:080][example_main-I] 00:e0:4c:00:03:5d, len: 2, rssi: -17, type: 40
    [14:47:02:081][example_main-I] 00:e0:4c:00:03:5d, status: bind
    [14:47:02:081][WIFI CAST-I] wifi_cast_add_node, node: 00:e0:4c:00:03:5d, set encrypt: 0
    [14:47:02:081][WIFI CAST-I] wifi_cast_add_node, 00:e0:4c:00:03:5d add success
    [14:47:02:088][example_main-I] 00:e0:4c:00:03:5d, bind success
    ```
3. After responser bound, controller inputs shell command to send light status control command.
    ```
    wificast light
    ```
    > Note: The device can only control the bound devices, so it must be bound before control.
    * Output sample from the controller:
    ```
    [14:42:14:469]wifi_cast light
    [14:42:14:469][example_main-I] example_control_light, status: 0
    [14:42:16:043]wifi_cast light
    [14:42:16:043][example_main-I] example_control_light, status: 1
    ```
    * Output sample from the response:
    ```
    [14:47:10:707][example_main-I] 00:e0:4c:00:03:5d, len: 2, rssi: -17, type: 80
    [14:47:10:707][example_main-I] 00:e0:4c:00:03:5d, status: off
    [14:47:11:783][example_main-I] 00:e0:4c:00:03:5d, len: 2, rssi: -17, type: 80
    [14:47:11:784][example_main-I] 00:e0:4c:00:03:5d, status: on
    ```
4. The controller inputs shell command to send unbind command when test finished.
    ```
    wificast unbind
    ```
    * Output sample from the controller:
    ```
    [14:47:21:663]wifi_cast unbind
    [14:47:21:663][example_main-I] example_control_bind, status: unbind
    ```
    * Output sample from the response:
    ```
    [14:47:21:652][example_main-I] 00:e0:4c:00:03:5d, len: 2, rssi: -17, type: 40
    [14:47:21:652][example_main-I] 00:e0:4c:00:03:5d, status: unbind
    [14:47:21:662][WIFI CAST-I] wifi_cast_del_node, 00:e0:4c:00:03:5d delete success
    [14:47:21:662][example_main-I] 00:e0:4c:00:03:5d, unbind success
    ```