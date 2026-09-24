# Example Description

This example shows how to run radar detect.

# HW Configuration

None

# SW configuration

1. In `component/soc/usrcfg/amebaxxx/ameba_wificfg.c`.
	```C
	wifi_user_config.ips_enable = 0;
	wifi_user_config.lps_enable = 0;
	```
2. Use menuconfig and select `CONFIG WIFI`->`Enable WIFI RADAR`.

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images [cmd: `./ameba.py build -a radar_detect`].
   * `Download` images to board by Ameba Image Tool.

4. For `DUT_IC`.
	1. reset `DUT_IC`

# Expect result

After compile success, the `wifi_radar_thread` may run and handle radar report after manually starting the radar, and will show radar header and raw data in trace tool or other uart tools at regular intervals.

	```
	--------------Radar Rpt Type 0-------------------
	[RADAR] radar header info:
	# rpt_type              = 3
	# rpt_seg_start         = 0
	# rpt_seg_end           = 0
	# bw_idx                = 1[0-70M;1-40M;2-20M]
	# chirp_width           = 3
	# chirp_num             = 14
	# frame_num             = 32
	# frame_interval        = 15
	# fft_strt_idx          = 0
	# fft_num_sub           = 44
	# channel               = 7
	# doppler_sample_num    = 128
	# isolation             = 40
	# range_leakage_dBx10   = 17
	# doppler_t2f_strt_idx  = -64 / 43 / 43
	# doppler_t2f_end_idx   = 63 / 42 / 42
	# doppler_t2f_step      = 0 / 0 / 0
	[RADAR] radar raw data: len = 11264 [rpt_len=11320]

	[0000]0019 001b 0019 0017 0013 000f 000b 0009
	[0008]000b 000f 0011 0012 0012 0011 0010 0011
	[RADAR] raw data done!
	```

# Note

It is currently used for internal radar detect.

# Supported IC

RTL8720F
