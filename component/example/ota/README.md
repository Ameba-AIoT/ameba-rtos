# Example Description

This example is designed for firmware update by Over-the-air programming (OTA) via Wireless Network Connection.

Download `ota_all.bin` from the download server (in `tools\DownloadServer(HTTP)`) automatically.

# HW Configuration

None

# SW configuration

1. Set type to `OTA_HTTP` to verify downloading firmware from http server.
	```C
	ota_update_init(ctx, (char *)host, PORT, (char *)resource, OTA_HTTP);
	```

2. Set type to `OTA_HTTPS` to verify downloading firmware from https server.
	```C
	ota_update_init(ctx, (char *)host, PORT, (char *)resource, OTA_HTTPS);
	```
   - `mbedtls_config.h`
		```C
		#define MBEDTLS_SSL_MAX_CONTENT_LEN 16384
		#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
		#define MBEDTLS_ECDH_C
		```

3. Set type to `OTA_SDCARD` to verify downloading firmware from sdcard.
	```C
	ota_update_init(ctx, NULL, NULL, (char *)resource, OTA_SDCARD);
	```

4. For sdcard update, only AmebaSmart support.
	1. users only need to set resource.
		```C
		#define RESOURCE			"ota_all.bin" // binary file path in the sdcard.
		```

	2. Add this line in `component\file_system\vfs\vfs_fatfs.h`
		```C
		#define FATFS_DISK_SD		1
		```

	3. in `component\file_system\fatfs\r0.14b\include\ffconf.h`
		```C
		#define FF_LFN_UNICODE		2
		```

	4. ensure that SDcard is in FAT32 format

	5. comment wifi connection if needed
		```C
		// Delay to check successful WiFi connection and obtain of an IP address
		// LwIP_Check_Connectivity();
		```

	6. Type command `./menuconfig.py` and choose `CONFIG VFS`  
	For `VFS_FATFS` with `VFS_INF_SD`: (Only AmebaSmart support)
		```
		[ ] Enable VFS LITTLEFS
		[*] Enable VFS FATFS
		FATFS Memory Type (SD)
		```

6. For http or https update, modify `PORT`, `HOST` and `RESOURCE` based on your download server. e.g. [SERVER]: http://m-apps.oss-cn-shenzhen.aliyuncs.com/051103061600.bin
	```C
	#define PORT    8082
	#define HOST    "m-apps.oss-cn-shenzhen.aliyuncs.com"
	#define RESOURCE    "051103061600.bin"
	```

	Or set it with `IP` and `ota_all.bin`. e.g.
	```C
	#define PORT    8082
	#define HOST    "192.168.1.100"
	#define RESOURCE    "ota_all.bin"
	```

7. How to use:
   - Run `./build.py -a ota` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

A http download example thread will be started automatically when booting.

Using the example with the tool in `tools\DownloadServer(HTTP)` with RESOURCE file.

# Note

Remember to Set the server `start.bat` with the same `PORT` and `RESOURCE`.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
