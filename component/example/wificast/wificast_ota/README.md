# OTA Example

This example demonstrates how to use the WIFI-CAST data to help upgrading other devices.

## Functionality

One device upgrade firmware through the HTTP server and help to upgrade other devices through WIFI-CAST data. When finish the upgradation, all the devices will reset to the new firmware.

## Setup Enviornment

At least two Ameba development boards are needed. Choose one board to upgrade through HTTP server and works as the sender, and another board works as the receiver.

One PC which can run the HTTP server and one router which all the development boards and PC will connect to are needed. 

## User-defined Frame Format

Config the user-defined data header in User Payload field.
|   Type   |   Len   |   Seq   |
|:--------:|:-------:|:-------:|
|  2 bytes | 2 bytes | 4 bytes |
* Type: The Type field is set to indicate the WIFI-CAST ota data.
    ```
    #define WIFI_CAST_SCAN_REQUEST          BIT(12)
    #define WIFI_CAST_SCAN_RESPONSE         BIT(11)
    #define WIFI_CAST_OTA_STATUS_REQUEST    BIT(8)
    #define WIFI_CAST_OTA_STATUS_RESPONSE   BIT(7)
    #define WIFI_CAST_OTA_DATA              BIT(6)
    ```
* Len: The Len field is set as the length of ota data.
* Seq: The Seq field is set as the sequence number when transmit the data of WIFI_CAST_OTA_DATA.

## How to Use the Example

### Step 1: Connect the Router

Connect the PC and development boards to the router.

### Step 2: Run the HTTP Server

The built-in python HTTP server locates in [tools\DownloadServer(HTTP)](tools/DownloadServer(HTTP)). Edit the script by following and then open a new terminal to run the HTTP server.

1. Change the IP address according to your PC's IP address.
    ```
    server_ip = '192.168.31.193'
    ```
2. Copy the `ota_all.bin` which is generated under the project directory into this directory.
3. Navigate to [tools\DownloadServer(HTTP)](tools/DownloadServer(HTTP)) and run the script.
    ```
    python http_server.py
    ```
> Note: The `ota_all.bin` should be prepared first by running `make` under the project directory.

### Step 3: Build & Flash

1. Change the IP address and resource according to the HTTP server.
    ```
    static const char *host = "192.168.137.1";  //"m-apps.oss-cn-shenzhen.aliyuncs.com"
    static const char *resource = "ota_all.bin";     //"051103061600.bin"
    ```
2. Navigate to the project directory according to the chip you use. For GCC enviornment, type command `make EXAMPLE=wificast_ota` to compile the example.
3. Download the image into flash for both sender and receiver.

### Step 4: Run Sender and Receiver

1. Reset both sender and receiver.
2. Then sender inputs shell command to start download new firmware from HTTP server and then send the downloaded firmware to other boards.
    ```
    wificast download
    ```
    * Output sample from the sender:
    ```
    [13:56:53:239][OTA-I] host: 192.168.137.1(8082), resource: ota_all.bin
    [13:56:53:589][OTA-I] [ota_update_http_recv_response] Download new firmware begin, total size : 836768
    [13:57:13:329][example_main-I] example_firmware_download, server firmware download is finished, spend time: 12154 ms, total size: 836736
    [13:57:16:789][example_main-I] example_ota_task, scan info num: 1
    [13:57:16:790][WIFI CAST-I] wifi_cast_add_node, node: 00:e0:4c:00:03:c2, set encrypt: 0
    [13:57:16:792][WIFI CAST-I] wifi_cast_add_node, 00:e0:4c:00:03:c2 add success
    [13:57:16:819][example_main-I] example_ota_task, start send firmware, total_size: 836736 bytes, packet_num: 3269
    [13:57:23:895][example_main-I] example_ota_request_status, send ota request
    [13:57:23:900][example_main-I] example_ota_status_response_cb, recv status response from00:e0:4c:00:03:c2
    [13:57:23:900][example_main-I] example_ota_request_status, recv ota req response, written_size: 654208, progress_index: 0
    [13:57:24:593][example_main-I] example_firmware_send, all devices upgrade done, successed_num: 1, spend time: 7758 ms
    ```
    * Output sample from the receiver:
    ```
    [13:57:13:306][WIFI CAST-I] wifi_cast_add_node, node: 00:e0:4c:00:03:5d, set encrypt: 0
    [13:57:13:310][WIFI CAST-I] wifi_cast_add_node, 00:e0:4c:00:03:5d add success
    [13:57:13:310][example_main-I] example_ota_scan_request_cb, recv scan request from src_mac: 00:e0:4c:00:03:5d
    [13:57:13:334][example_main-I] example_ota_scan_request_cb, send response success
    [13:57:22:220][example_main-I] example_ota_status_request_cb, send status response, total_size: 836736, written_size: 0, progress_index: 0
    [13:57:23:881][example_main-I] example_ota_status_request_cb, recv status request from00:e0:4c:00:03:5d
    [13:57:23:882][example_main-I] example_ota_status_request_cb, ota status, image_id: 1, image_addr: 0x214000, total_size: 836736 bytes, written_size: 654208 bytes, packet_num: 3269
    [13:57:24:570][example_main-I] example_ota_status_request_cb, upgrade done, spend time: 2568 ms
    ```
3. The sender and receiver will reset to the new firmware after the upgradation finished.
    ```
    [13:57:24:575][SYS-A] [sys_clear_ota_signature] IMGID: 1, current OTA1 Address: 0x00014000, target OTA2 Address: 0x00214000
    ```

## Note
The role of `sender` and `receiver` will be on the devices at the same time. When run `wificast download` on one device, this device acts as sender and sends data to other devices. The device receives the data will act as receiver.
