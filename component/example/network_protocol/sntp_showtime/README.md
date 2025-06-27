# Example Description

Show system time maintained by time from NTP server and system tick.

# HW Configuration

None

# SW configuration

1. Can Modify `SNTP_SERVER_ADDRESS` and `SNTP_UPDATE_DELAY` in `sntp.c` under `component/network/sntp` for NTP time update.
For `SNTP_UPDATE_DELAY` to work, `SNTP_STARTUP_DELAY` should be defined

2. Can choose the showing format of the time.
	```C
	#define TIME_MODE    0	// for UTC with microseconds, e.g. 'Tue Dec 12 08:38:51 2023
																 + 739255 usec'
	#define TIME_MODE    1	// for timezone with seconds, e.g. '2023-12-12 16:07:41'
	```

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

Can make automatical Wi-Fi connection (AP with Internet access) when booting by using wlan fast connect example.

A lwip ntp showtime example thread will be started automatically when booting.

1. For this example, UTC+8 (offset in hrs) timezone is used for `TIME_MODE` configuration which is enabled by default.

2. System date and time along with `UTC+8` timezone will be shown per second like the following:
	```
	2023-12-12 16:34:36 UTC+8
	2023-12-12 16:34:37 UTC+8
	2023-12-12 16:34:38 UTC+8
	2023-12-12 16:34:39 UTC+8
	...
	```

3. If `TIME_MODE` is set to 0, system date and time will be shown per second like the following:
	```
	Tue Dec 12 08:38:51 2023
	 + 739255 usec
	Tue Dec 12 08:38:52 2023
	 + 742255 usec
	Tue Dec 12 08:38:53 2023
	 + 745255 usec
	Tue Dec 12 08:38:54 2023
	 + 748255 usec
	...
	```

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
- AmebaGreen2
