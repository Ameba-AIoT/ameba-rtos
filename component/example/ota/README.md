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

	If encounter the following errors in an SSL connection. Set `MBEDTLS_SSL_IN_CONTENT_LEN` by `./menuconfig.py` and choose `CONFIG SSL`-> `Maximum len of incoming fragments` -> set large size.

	```C
	#define MBEDTLS_ERR_SSL_BAD_INPUT_DATA                    -0x7100
	#define MBEDTLS_ERR_SSL_INVALID_RECORD                    -0x7200
	```

3. For http or https update, modify `PORT`, `HOST` and `RESOURCE` based on your download server. Set it with `IP` and `ota_all.bin`. e.g.
	```C
	#define PORT    8082
	#define HOST    "192.168.1.100"
	#define RESOURCE    "ota_all.bin"
	```

4. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

5. Copy the `ota_all.bin` into `tools\DownloadServer(HTTP)`, and configure the script according to `tools\DownloadServer(HTTP)\readme.txt` first then execute the script.

6. Reset the board and start the download.

# Expect result

A http download example thread will be started automatically when booting.

# Note

The http server script and board need to be on the same local area network.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
