# Example Description

This example shows how to fetch csi.

# HW Configuration

None

# SW configuration

1. Use cmd `./menuconfig.py` and select `CONFIG WIFI`->`Enable WIFI CSI`.

2. If sta mode, please modify assoc_ap_mac of wifi_csi_thread() in `example_wifi_csi.c`:
	```
	unsigned char assoc_ap_mac = <mac address of associated AP>
	```

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

4. Enviornment setup: Wifi csi test need at least two device.
	1. `DUT_IC` as softap or softap + sta mode(nolink): another board (ameba IC as sta mode or others) for slave node must be required.
	2. `DUT_IC` as sta mode: another board (ameba IC as softap mode or realAP) for master node must be required.

5. For `DUT_IC`.
	1. reset `DUT_IC`
	2. start `DUT_IC` as softap mode[softap + sta mode(nolink)] or only sta mode
	3. the csi will automatically enable after successful WiFi connection or another device associated with softap

# Expect result

After compile success, the `wifi_csi_thread` may run and enable csi func when connect ok, and will show csi [header info + raw data] in trace tool or other uart tools at regular intervals.

1. `DUT_IC` as softap or softap + sta mode(nolink)

	```
	### SOFTAP Break ###
	[CH INFO] csi header info:
	# sta_mac_addr     = 00:e1:4c:00:06:e3
	# trigger_mac_addr = 00:e0:4c:00:0c:35
	# trigger_flag     = 0
	# timestamp        = 7461751 us
	# csi_data_len     = 104
	# csi_sequence     = 1
	# channel          = 10
	# bandwidth        = 0
	# rx_rate          = 12
	# protocol_mode    = 0 [ofdm(0)/ht(1)/vht(2)/he(3)]
	# num_sub_carrier  = 52
	# num_bit_per_tone = 16 [I:8 bits; Q:8 bits]
	# rssi[0]          = -18 dbm
	# rxsc             = 0
	# csi_valid        = 1
	[CH INFO] csi raw data: len = 104[152]
	[00]0xf105f101f1fef3fb
	[01]0xfa0ff70df40bf208
	[02]0x080f04100111fd10
	[03]0x10040f080d0b0b0d
	[04]0x0ff810fb11fe1101
	[05]0x05ee08f00bf20df5
	[06]0xf6f0f9efffed02ed
	[07]0xeffbf0f8f2f5f4f2
	[08]0xf108f005ef02effe
	[09]0xfb0ff80ef50cf30a
	[10]0x070c040e010ffe0f
	[11]0x0d020c050b08090a
	[12]0x08f80afa0bfc0cff
	[CH INFO] raw data done!
	```

2. `DUT_IC` as sta mode, the log is as follows,
	```
	### STA Break ###
	[CH INFO] csi header info:
	# sta_mac_addr     = 00:e0:4c:00:0a:a3
	# trigger_mac_addr = 00:e1:4c:00:0c:31
	# trigger_flag     = 0
	# timestamp        = 4098649 us
	# csi_data_len     = 104
	# csi_sequence     = 1
	# channel          = 157
	# bandwidth        = 0
	# rx_rate          = 12
	# protocol_mode    = 0 [ofdm(0)/ht(1)/vht(2)/he(3)]
	# num_sub_carrier  = 52
	# num_bit_per_tone = 16 [I:8 bits; Q:8 bits]
	# rssi[0]          = -43 dbm
	# rxsc             = 0
	# csi_valid        = 1
	[CH INFO] csi raw data: len = 104[152]
	[00]0xf2fdf3f9f6f6faf4
	[01]0xf70bf408f205f101
	[02]0x060c020efe0efa0d
	[03]0x0e010d040b08090a
	[04]0x09f30bf60df90efd
	[05]0xf9f0feef02ef06f1
	[06]0xeffcf0f7f1f6f5f2
	[07]0xf50bf209f005ef01
	[08]0x030d000efc0ef90d
	[09]0x0c030b060909070b
	[10]0x09f60bf90cfc0d00
	[11]0xfcf3fff203f306f4
	[12]0xf5fef5fbf6f8f9f5
	[CH INFO] raw data done!
	```

# Note

The CSI_REPORT_BUF_SIZE can be modified according to the configuration parameters, refer to the description of the max_csi_report_size in online documentation or user guide.

The CSI_REPORT_BUF_NUM is generally set to be greater than or equal to the number of enabled peer CSI STAs, otherwise it may cause abnormal queue/dequeue behavior caused by delayed application processing.


# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
