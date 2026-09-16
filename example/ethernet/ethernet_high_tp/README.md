# Example Description

This example demonstrates Ethernet driver usage tuned for high throughput. It shows how to initialize the Ethernet interface and register a link status callback function via `eth_register_link_cb()`.

The callback function is invoked when the Ethernet link status changes (UP or DOWN), where IP configuration is performed.

# HW Configuration

- Connect the board to a router or switch that provides DHCP service

# SW configuration

1. Enable Ethernet Feature:

   Use menuconfig and enable `CONFIG ETHERNET`, then select `PHY SOURCE CLK` according to how the PHY on your board is clocked (`50M from Ameba` / `25M from Ameba` / `PHY xtal`).

   See [ETH PHY CLK](https://aiot.realmcu.com/zh/latest/rtos/peripherals/ethernet/index.html#eth-phy-clk) for more details.

2. Select the RMII Pin Group:

   Set `ETHERNET_Pin_Grp` in `component/soc/usrcfg/amebagreen2/ameba_intfcfg.c` to the group (`0x0`-`0x3`) that matches the RMII pins actually wired on your board. The pin mapping of each group is listed in `ETHERNET_PAD[]` in the same file.

3. Throughput tuning:

   The example's `prj.conf` already has the following configured; manually confirm or adjust them via menuconfig if needed:
   * Run IMG2 (application) from PSRAM: menuconfig `CONFIG Link Option` -> `IMG2(Application) running on FLASH, SRAM, or PSRAM?` -> `PSRAM`.
   * Raise the CPU frequency: menuconfig `CONFIG User Config` -> `Clock Configuration` -> `SoC Clock Config` -> `[5] 1.0V, RMII/USB, CPU 320MHz`.
   * Enlarge the TCP send window/buffer size: menuconfig `CONFIG WIFI` -> `Enable HIGH TP`.
   * Increase the pbuf pool buffer size: menuconfig `CONFIG LWIP` -> `Len of pbuf pool` -> `1536`.
   * Double the TX descriptor count: menuconfig `CONFIG ETHERNET` -> `Number of ETH TX descriptors` -> `16` (default is `8`).

4. IP Configuration:

   The example obtains an IP address automatically from the network via DHCP.

5. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect Result

When the Ethernet cable is plugged in:
```
=== Ethernet Link UP ===
Ethernet DHCP got IP: 192.168.1.100
```

When the Ethernet cable is unplugged:
```
=== Ethernet Link DOWN ===
Ethernet IP released
```

Use the `AT+IPERF` command to measure throughput:
* TCP: `AT+IPERF=-s,-i,1` to act as a TCP iperf server, or `AT+IPERF=-c,xx.xx.xx.xx,-i,1,-t,10` to act as a TCP client.
* UDP: `AT+IPERF=-s,-u,-i,1` to act as a UDP iperf server, or `AT+IPERF=-c,xx.xx.xx.xx,-u,-i,1,-t,10,-b,200M` to act as a UDP client.

Type `AT+IPERF` alone for more options.

# Note

- The callback function is called in `eth_link_monitor_thread`.
- `eth_send()` copies packet data with `memcpy()`. If the source and destination buffers are not word-aligned, the copy degrades to a byte-by-byte copy, which lowers throughput. Keep application-side TX buffers word-aligned to get the benefit of word copies.

# Supported IC

RTL8721F
