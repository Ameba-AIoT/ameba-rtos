# WIFI-CAST
Here provide examples to simplify the use of WIFI-CAST.  

## Introduction

WIFI-CAST is a kind of connectionless Wi-Fi communication protocol that is defined by Realtek. In WIFI-CAST, the application data can be transmitted without wifi connection amongst devices. It supports one-to-many and many-to-many device connection and communication.

Different from traditional Wi-Fi protocols, the data can be transmitted directly to data link layer through WIFI-CAST, which reduces the delay caused by packet loss under congested network, and leads to quickly response time.

## Quick Start
### Hardware Preparation
Chips of AmebaLite, AmebaSmart and AmebaDplus are recommended.

### Set up Development Environment
Setting the environment by following the readme in each [example](#application-examples).

## Frame Format
In WIFI-CAST, QoS data frame is transmited between devices.

|  MAC Header  | QoS Control |   SNAP   | WIFI-CAST Header |  Pading    |   User Payload   |   FCS   |
|:------------:|:-----------:|:--------:|:----------------:|:----------:|:----------------:|:-------:|
|    24 bytes  |   2 bytes   |  8 bytes |     16 bytes     | 6-14 bytes |    0-1500 bytes  | 4 bytes |

* If Security is enabled, the IV with 8 bytes will be added after SNAP. 
* Pading: The Pading field is used to keep the header align with 32 bytes. 
* User Payload: The User Payload content contains user data, which is defined by user.
* WIFI-CAST Header: WIFI-CAST Header content contains fields as follow:

    |   Magic  |   Type  |   Seq   | 
    |:--------:|:-------:|:-------:|
    | 12 bytes | 2 bytes | 2 bytes |
    
* Magic: The Magic field is set to the fixed value(Wcast), indicating the WIFI-CAST application.
* Type: The Type field is set to the value(0x2000), indicating the WIFI-CAST data. Set to the value(0x4000), indicating the WIFI-CAST data ack.
* Seq: The Seq field is increased by 1 when sending packet to avoid duplicate packet.

## Initialization and Deinitialization

Call `wifi_cast_init()` to initialize WIFI-CAST and `wifi_cast_deinit()` to de-initialize WIFI-CAST. WIFI-CAST data must be transmitted after Wi-Fi is started, so it is recommended to start Wi-Fi before initializing WIFI-CAST and stop Wi-Fi after de-initializing WIFI-CAST.

A device with a broadcast MAC address will be added during initialization. The range of the channel is from 1 to 14. If the channel is set to 0 and device is not connected to a router, the initialization will be aborted and returns an error code. And if the device is connected to a router, the channel will be set to the current channel. Otherwise, the channel will be set as user defined. All the devices need communicate through WIFI-CAST must be set at the same channel.

When `wifi_cast_deinit()` is called, all of the information of paired devices are deleted.

## Device Management

Call `wifi_cast_add_node()` to add the device to the paired device list before you send data to this device. If security is enabled, the Key must be set. The maximun length of key is 16 and the maximum number of paired devices is 16 for now.

Call `wifi_cast_del_node()` to remove the device from paried device list.

Call `wifi_cast_get_node_info` to get device parameters and check if the device exists before sending WIFI-CAST data. 

## Send WIFI-CAST Data

Call `wifi_cast_send()` to send WIFI-CAST data. It will return WIFI_CAST_OK if send success. Otherwise, it will return with error code. Several reasons can lead to fails to send data. For example, the destination device does not exist; the channels of the devices are not the same; the data frame is lost when transmitting on the air, etc. 

### Config Send Information

In order to ensure the application layer can receive the data, the `struct wifi_cast_frame_info` parameter in `wifi_cast_send()` is provided. 
* Sender can set the `ack` field to 1 and set `wait_ms` field to a timeout before send data, and receiver send back ack data when receiving WIFI-CAST data. If receiving ack data timeouts, retransmit the WIFI-CAST data. A sequence number can also be assigned to WIFI-CAST data to drop the duplicate data.
* Sender can set the `retry_limit` field to config the MAC layer retransmition times.
* Sender can set the `retransmit_count` field to config the application layer retransmission times.
* Sender can set the `channel` to WIFI_CAST_CHANNEL_ALL when not all the receiver are in the same channel.
* Sender can set the `tx_rate` field to config the MAC layer transmition rate.

## Receiving WIFI-CAST Data

Call `wifi_cast_register_recv_cb()` to register receiving callback function. Call the receiving callback function when receiving WIFI-CAST data. The receiving callback function runs from the Wi-Fi task. So, do not do block operations in the callback function. Instead, post the necessary data to a queue and handle it from a lower priority task.

## Application Examples

Examples of sending and receiving WIFI-CAST data between two devices:
* [control](wificast_control)
* [security](wificast_secruity)
* [uart](wificast_uart)
* [ota](wificast_ota)

## API Reference
### Header file
[component/wifi/wifi_cast/wifi_intf_drv_app_cast.h](component/wifi/wifi_cast/wifi_intf_drv_app_cast.h)
### Functions
```
wcast_err_t wifi_cast_init(wifi_cast_config_t *pconfig);
```
```
wcast_err_t wifi_cast_deinit(wifi_cast_config_t *pconfig);
```
```
wcast_err_t wifi_cast_add_node(wifi_cast_node_t *pnode);
```
```
wcast_err_t wifi_cast_del_node(wifi_cast_node_t *pnode);
```
```
wifi_cast_node_t *wifi_cast_get_node_info(wifi_cast_node_t *pnode);
```
```
wcast_err_t wifi_cast_send(wifi_cast_node_t *pnode, unsigned char *data, int data_len, wifi_cast_frame_info_t *info);
```
```
wcast_err_t wifi_cast_register_recv_cb(wifi_cast_recv_cb_t recv_cb);
```
```
typedef void (*wifi_cast_recv_cb_t)(wifi_cast_node_t *pnode, unsigned char *buf, unsigned int len, signed char rssi);
```
### Structures
WIFI-CAST device parameters.
```
typedef struct wifi_cast_node {
	wifi_cast_addr_t mac; /* node mac address */
	wifi_cast_key_t  key; /* key for encrypt data */
	unsigned char encrypt; /* set to 1 if security enable */
	void *priv;	/* user define */
} wifi_cast_node_t;
```
WIFI-CAST data information parameters.
```
typedef struct wifi_cast_frame_info {
	unsigned int wait_ms;           /* set wait timeout when ack set to true */
	unsigned char ack;              /* enable or disable ACK, set to true if need rx node response with ack */
	unsigned char retry_limit;      /* tx packet retry times (hardware retry limit times) */
	unsigned char retransmit_count; /* tx packet restansmit count by software */
	unsigned char channel;          /* tx packet channel, set to WIFI_CAST_CHANNEL_CURRENT or WIFI_CAST_CHANNEL_ALL */
	unsigned char tx_rate;     /* tx packet rate, val: RTW_RATE_1M, RTW_RATE_2M...*/
} wifi_cast_frame_info_t;
```
WIFI-CAST config parameters.
```
typedef struct wifi_cast_config {
	unsigned char 	channel; /* all wifi cast nodes need set in the same channel */
} wifi_cast_config_t;
```
### Definitions
```
WIFI_CAST_OK               /* execute functions success */
WIFI_CAST_ERR              /* execute functions fail */
WIFI_CAST_ERR_NO_MEMORY    /* system memory not enough */
WIFI_CAST_ERR_INVALID_DATA /* data invalid */
WIFI_CAST_ERR_NODE_EXIST   /* the device is already added */
WIFI_CAST_ERR_WAIT_TIMEOUT /* send data timeout when ack and time_ms is set */
```
```
MAX_NODE_NUM      /* maximum number of devices */
WIFI_CAST_KEY_LEN /* maximum length of the key */
```
```
WIFI_CAST_CHANNEL_CURRENT      /* Only in the current channel */
WIFI_CAST_CHANNEL_ALL          /* All supported channels */
```