# Example Description

This example shows how to send management frames and data frames through API: wifi_send_raw_frame(), which named wifi_raw_tx.

At the same time, by enabling rx_promisc to explicitly display the packet contents of wifi_raw_tx.

# HW Configuration

None

# SW configuration

1. Change `addr1`, `addr2` and `addr3` as required.
	```C
	u8 addr1[6] = {0xBC, 0x46, 0x99, 0x7B, 0x48, 0x74};		// Address 1, should be the mac address of destination
	u8 addr2[6] = {0x00, 0xE0, 0x4C, 0x00, 0x02, 0xF0};		// Address 2, should be the mac address of myself as master
	u8 addr3[6] = {0xBC, 0x46, 0x99, 0x7B, 0x48, 0x74};		// Address 3, shoule be the BSSID
	```
2. In `component/soc/usrcfg/amebaxxx/ameba_wificfg.c`.
	```C
	wifi_user_config.ips_enable = 0;
	```

3. Enviornment setup: It's best has two device for Wifi raw test.
   * If there is only one device, you can check the wifi_raw_tx packets through a sniffer.

4. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

The example thread will be started automatically when booting.
   * `wifi_raw_tx_thread`: polling and sending management frames and data frames at fixed intervals
   * `wifi_raw_rx_thread`: enabling rx_promisc and show packet contents of wifi_raw_tx
		```
		18:20:31.230  Promisc RX [probe_req: rate = 12, rssi = -41 dBm] ->
		18:20:31.230  40 00 3c 00 bc 46 99 7b 48 74 00 e0 4c 00 02 f0
		18:20:31.230  bc 46 99 7b 48 74 00 00 00 00 01 08 82 84 8b 96
		18:20:31.230  8c 12 98 24 32 04 b0 48
		...
		18:20:31.533  Promisc RX [qos_data: rate = 131, rssi = -41 dBm] ->
		18:20:31.533  88 00 7c 00 bc 46 99 7b 48 74 00 e0 4c 00 02 f0
		18:20:31.533  bc 46 99 7b 48 74 00 00 2a 37 c1 db 40 7c c6 52
		18:20:31.533  fe 2b e3 da a3 4f 77 2c
		```

# Note

Ensure that the countermeasure equipment is on the same channel (e.g. fast connect may cause channel changes).

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
