# Throughput Example

This example demonstrates how to measure the WIFI-CAST throughput with the plain `wifi_cast_*` interfaces (no wrapper layer).

## Functionality

One device sends fixed-size packets back-to-back with `wifi_cast_send()` for a given time. The other device counts the received packets and prints the receive rate every second. When the sender finishes, the receiver prints the total rate and the number of lost packets.

The sender also prints its own send rate. Note that the TX rate only counts packets accepted by `wifi_cast_send()`, so the **RX rate is the real throughput**.

## Setup Enviornment

At least two Ameba development boards are needed. Both boards run the same image: any board can be the sender or the receiver.

## User-defined Frame Format

The test packet carries the following header in the User Payload field, followed by padding up to the requested length.
|  Magic  | Session | Seq     | Flags   |
|:-------:|:-------:|:-------:|:-------:|
| 4 bytes | 4 bytes | 4 bytes | 4 bytes |
* Session: Changes on every `wificast tx`, so the receiver can tell tests apart.
* Seq: Packet sequence number. In the final packet (`Flags` = END) it carries the total packet count, which is used to count the lost packets.

## How to Use the Example

### Step 1: Build & Flash

1. Disable wifi fast connection in component/soc/usrcfg/xxxx/ameba_wificfg.c
    ```
    wifi_user_config.fast_reconnect_en = 0;
    ```
2. Build images by command `./ameba.py build -a wificast_throughput` and download images to both boards.

### Step 2: Run

1. Reset both boards. Both boards use the same channel (default 6). To test on another channel, run `wificast channel <ch>` on **both** boards.
2. Broadcast test (no setup needed). On the sender:
    ```
    wificast tx
    ```
3. Unicast test. Unicast data is only received from known nodes, so register the sender on the receiver first, and the receiver on the sender:
    ```
    (receiver) wificast peer <sender mac>
    (sender)   wificast peer <receiver mac>
    (sender)   wificast tx <receiver mac> 1400 10 1
    ```

## Serial Port Commands

```
wificast tx [dst] [len] [sec] [ack] [rate] [retransmit] [retry_limit] [wait_ms]
```
| Argument   | Description                                                              | Default            |
|:----------:|:-------------------------------------------------------------------------|:-------------------|
| dst        | Destination mac, or `bcast` for broadcast                                | bcast              |
| len        | Packet length in bytes (16 ~ `wifi_cast_get_max_frame_len()`)            | max frame length   |
| sec        | Test duration in seconds (1 ~ 86400)                                     | 10                 |
| ack        | 1: wait for the ACK of every packet (unicast only), 0: no ACK            | 0                  |
| rate       | `6m` `24m` `54m` `mcs0` `mcs7` `he0` `he7` `he9`                         | 54m                |
| retransmit | Extra sends of every packet by software                                  | 0                  |
| retry_limit| Hardware retry times of every packet (0 ~ 254), takes effect on unicast  | driver default (4) |
| wait_ms    | Total ACK wait budget in ms when `ack` = 1, split across (1 + retransmit) attempts | 100 |

```
wificast peer <mac>   add a node on this device
wificast channel [ch] switch to channel <ch>, or show the current channel without argument
wificast stop         stop the running test
wificast help         show the usage
```

## Output Example

Sender:
```
TX start: dst ff:ff:ff:ff:ff:ff, len 1400, 10 s, ack 0, rate 0x6c, retransmit 0, retry_limit 4, wait 100 ms
TX: ... KB in 1000 ms, ... Mbps
...
TX total: ... KB in 10000 ms, ... Mbps
TX packets: ... ok, ... fail
ACK latency: ... samples, min ... us, avg ... us, max ... us   (only with ack = 1)
ACK latency histogram: <1ms ..., 1~2ms ..., 2~5ms ..., 5~20ms ..., 20~100ms ..., >=100ms ...
```
Receiver:
```
RX: ... KB in 1000 ms, ... Mbps
RX total: ... KB in ... ms, ... Mbps
RX packets: ... received, ... expected, ... lost, ... dup
```

## Notes

* `ack = 1` blocks on every packet until the ACK arrives, so it measures the reliable throughput, which is lower than the `ack = 0` throughput.
* With `ack = 1` the sender also prints the ACK latency: the time from calling `wifi_cast_send()` until it returns successfully, i.e. until the ACK is received. Only successful sends are sampled. A send that needed a retry (hardware or `retransmit`) includes the wait of the lost attempts, so the max can reach `wait_ms`; use `retransmit` 0 and `retry_limit` 0 to see the single round-trip time.
* Broadcast frames are not acknowledged by the MAC layer and are sent at the configured rate with no hardware retry, so the lost count is only meaningful with the receiver close to the sender.
* The RX time runs from the first to the last data packet. A packet whose sequence number is not higher than the highest one seen is counted as `dup` and excluded from the rate (with `ack = 1` the sender reuses the sequence number when only the ACK was lost). A reordered packet is also counted as `dup`, which is rare on a single link.
* The receiver only counts packets of the current session. If the END packet is lost, the receiver reports after 1 s of silence and does not count the tail loss.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8720F
RTL8721Dx
RTL8721F