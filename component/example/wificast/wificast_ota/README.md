# OTA Example

This example demonstrates how to use the WIFI-CAST data to help upgrading other devices.

## Functionality

One device upgrade image through the HTTP server and help to upgrade other devices through WIFI-CAST data. When finish the upgradation, all the devices will reset to the new image.

## Setup Enviornment

At least two same Ameba development boards are needed. Choose one board to upgrade through HTTP server and works as the sender, and another board works as the receiver.

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

Connect the PC to the router.

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
> Note: The `ota_all.bin` should be prepared first by running `./build.py` under the project directory.

### Step 3: Build & Flash

1. Disable wifi fast connection in component/soc/usrcfg/xxxx/ameba_wificfg.c
    ```
    wifi_user_config.fast_reconnect_en = 0;
    ```
2. Choose one board as sender:
    * Change the IP address and resource according to the HTTP server.
        ```
        static const char *host = "192.168.137.1";  //"m-apps.oss-cn-shenzhen.aliyuncs.com"
        static const char *resource = "ota_all.bin";     //"051103061600.bin"
        ```
    * Change the role to sender:
        ```
        static u8 ota_role = WIFI_CAST_OTA_SENDER;
        ```
    * Build by command `./build.py -a wificast_ota` and download images to sender's board by Ameba Image Tool.

3. Choose another board as receiver:
    * Change the role to receiver:
        ```
        static u8 ota_role = WIFI_CAST_OTA_RECEIVER;
        ```
    * Build by command `./build.py -a wificast_ota` and download images to receiver's board by Ameba Image Tool.

### Step 4: Run Sender and Receiver

1. Reset both sender and receiver.
    * Output sample from the sender:
    ```
    [13:43:14:827][example_main-I] ------------->start, ota role: 1
    ```
    * Output sample from the receiver:
    ```
    [13:43:14:827][example_main-I] ------------->start, ota role: 2
    ```
2. Connect sender to the router which the PC connected.
    ```
    AT+WLCONN=ssid,xxx,pw,xxxxxxxxxxx
    ```
3. Then sender inputs shell command to start download new image from HTTP server and then send the downloaded image to other boards.
    ```
    wificast download
    ```
    * Output sample from the sender:
    ```
    [13:56:53:239][OTA-I] host: 192.168.137.1(8082), resource: ota_all.bin
    [13:56:53:589][OTA-I] [ota_update_http_recv_response] Download new image begin, total size : 836768
    [13:57:13:329][example_main-I] example_image_download, server image download is finished, spend time: 12154 ms, total size: 836736
    [13:57:16:789][example_main-I] example_ota_task, scan info num: 1
    [13:57:16:790][WIFI CAST-I] wifi_cast_add_node, node: 00:e0:4c:00:03:c2, set encrypt: 0
    [13:57:16:792][WIFI CAST-I] wifi_cast_add_node, 00:e0:4c:00:03:c2 add success
    [13:57:16:819][example_main-I] example_ota_task, start send image, total_size: 836736 bytes, packet_num: 3269
    [13:57:23:895][example_main-I] example_ota_request_status, send ota request
    [13:57:23:900][example_main-I] example_ota_status_response_cb, recv status response from00:e0:4c:00:03:c2
    [13:57:23:900][example_main-I] example_ota_request_status, recv ota req response, written_size: 654208, progress_index: 0
    [13:57:24:593][example_main-I] example_image_send, round: 20
    [13:57:24:593][example_main-I] example_image_send, devices upgrade completed, unfinished_num: 0, successed_num: 1, spend time: 67540 ms
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
    [13:57:24:570][example_main-I] example_ota_status_request_cb, upgrade done, spend time: 58849 ms
    ```
4. The receiver will reset to the new image after the upgradation finished.
    ```
    [13:57:24:575][SYS-A] [sys_clear_ota_signature] IMGID: 1, current OTA1 Address: 0x00014000, target OTA2 Address: 0x00214000
    ```