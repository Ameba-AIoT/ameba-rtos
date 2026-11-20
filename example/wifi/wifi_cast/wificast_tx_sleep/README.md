# WiFi-Cast TX Sleep Example

This example shows how to send WIFI-CAST data periodically with low power: the sender sends 1 KB every `DATA_PERIOD_MS` and sleeps between two transmissions. The sleep mode is clock gating (CG): the system sleeps with `SLEEP_CG` and wifi is in IPS CG when not connected, or in wowlan when connected to an AP.

## Functionality

Two tasks run on the sender after `wificast sleep on`:
* `example_prep_task`: every `DATA_PERIOD_MS` (50 ms) gives a semaphore, then delays.
* `example_tx_task`: blocks on the semaphore. After taking it, the task
  0. writes the sequence number into the 1 KB data (`SLEEP_DATA_LEN`);
  1. acquires wakelock `PMU_OS` and exits IPS;
  2. sends the 1 KB data to the receiver by `wifi_cast_send`, no ACK is requested (`ack = 0`), the receiver does not need to send any data back;
  3. not connected: requests IPS and retries until wifi has entered it; connected: skips the IPS request, the system suspend takes the wowlan path. Then sets the sleep type to `SLEEP_CG` (clock gating), releases the wakelock and blocks on the semaphore again.

The example calls `wifi_cast_init()` with `rx_dis = 1` (send only), so wifi cast does not enable promisc rx and creates no ACK task. If the receiver is not set by `wificast peer`, the frame is broadcast.

When both tasks are idle the MCU enters CG sleep (not power gating) and wifi is in IPS CG (or wowlan when connected).

`wificast sleep off` stops both tasks and leaves IPS.

Shell commands:
* `wificast peer <mac>`: add the receiver mac to the node list and use it as the destination, run it before `sleep on`. Without it the data is broadcast.
* `wificast sleep on/off`: start/stop the periodic send.

## Setup Enviornment

At least two Ameba development boards are needed. One board works as the sender and runs this example. The other board works as the receiver, it only needs to run wifi cast on the same channel (e.g. any other wificast example).

## How to Use the Example

### Step 1: Config the Data Length and Period

```
#define SLEEP_DATA_LEN       (1024)
#define DATA_PERIOD_MS       (50)
```
`SLEEP_DATA_LEN` must not exceed `wifi_cast_get_max_frame_len()`, otherwise `sleep on` is refused. The sleep time is not set by a macro: the system sleeps as long as the tasks block, i.e. about `DATA_PERIOD_MS` between two transmissions.

### Step 2: SW Configuration

1. In `component/soc/usrcfg/xxxx/ameba_wificfg.c`, turn off fast connect, let the user control IPS and select the clock-gating IPS level.
    ```
    wifi_user_config.fast_reconnect_en = 0;
    wifi_user_config.ips_level = RTW_IPS_WIFI_CG;
    wifi_user_config.ips_ctrl_by_usr = 1;
    ```

2. To measure power, turn off logs by setting `RTK_LOG_DEFAULT_LEVEL` to `RTK_LOG_NONE` in the SoC `log.h`.

### Step 3: Build & Flash

Build images by command `./ameba.py build -a wificast_tx_sleep` and download images to the sender.

### Step 4: Run

1. Reset the sender, then add the receiver as peer and start the periodic send.
    ```
    wificast peer xx:xx:xx:xx:xx:xx
    wificast sleep on
    ```
    * The sender sends 1 KB about every `DATA_PERIOD_MS`.
2. Stop when test finished.
    ```
    wificast sleep off
    ```

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8720F
RTL8721Dx
RTL8721F