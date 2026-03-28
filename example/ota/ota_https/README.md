# Example Description

This example is designed for firmware update by Over-the-air programming (OTA) via Wireless Network Connection.

Download `ota_all.bin` from the download server (in `tools\DownloadServer(HTTP)`) automatically.

# HW Configuration

None

# SW configuration

1. If encounter the following errors in an SSL connection. Set `MBEDTLS_SSL_IN_CONTENT_LEN` by using menuconfig and choose `CONFIG SSL`-> `Maximum len of incoming fragments` -> set large size.

	```C
	#define MBEDTLS_ERR_SSL_BAD_INPUT_DATA                    -0x7100
	#define MBEDTLS_ERR_SSL_INVALID_RECORD                    -0x7200
	```

2. Modify `PORT`, `HOST` and `RESOURCE` based on your download server. Set it with `IP` and `ota_all.bin`. e.g.
	```C
	#define PORT   443
	static const char *HOST = "192.168.3.37";
	static const char *RESOURCE = "ota_all.bin";
	```

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

4. Copy the `ota_all.bin` into `tools\DownloadServer(HTTP)`.

5. Configure the server as a https server according to `tools\DownloadServer(HTTP)\readme.txt` first then execute the script.

6. Reset the board and start the download.

# Expect result

A https download example thread will be started automatically when booting.

# Note

The https server script and board need to be on the same local area network.

# Supported IC

* RTL8730E
* RTL8726E
* RTL8720E
* RTL8713E
* RTL8710E
* RTL8721Dx
* RTL8721F
