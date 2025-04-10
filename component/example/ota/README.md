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

3. For http or https update, modify `PORT`, `HOST` and `RESOURCE` based on your download server. e.g. [SERVER]: http://m-apps.oss-cn-shenzhen.aliyuncs.com/051103061600.bin
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

4. How to use:
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
