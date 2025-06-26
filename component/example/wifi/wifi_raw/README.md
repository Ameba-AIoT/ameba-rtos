# Example Description

This example shows how to send probe_req and receive probe_rsp packets.

# HW Configuration

None

# SW configuration

1. Change `addr1`, `addr2` and `addr3` as required.
	```C
	u8 addr1[6] = {0xBC, 0x46, 0x99, 0x7B, 0x48, 0x74};		// Address 1, should be the mac address of destination
	u8 addr2[6] = {0x00, 0xE0, 0x4C, 0x00, 0x02, 0xF0};		// Address 2, should be the mac address of myself as master
	u8 addr3[6] = {0xBC, 0x46, 0x99, 0x7B, 0x48, 0x74};		// Address 3, shoule be the BSSID
	```
2. CALL wifi_set_ips_internal(0) to avoid raw tx failure caused by entering IPS When the STA is not connected

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

The example thread will be started automatically when booting.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
