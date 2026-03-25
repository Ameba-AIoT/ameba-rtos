# Example Description

This example is designed for firmware update by Over-the-air programming (OTA) via Wireless Network Connection.

Download `ota_all.bin` from the download server (in `tools\DownloadServer(HTTP)`) automatically.

# HW Configuration

None

# SW configuration

1. Modify `PORT`, `HOST` and `RESOURCE` based on your download server. Set it with `IP` and `ota_all.bin`. e.g.
	```C
	#define PORT   8082
	static const char *HOST = "192.168.3.37";
	static const char *RESOURCE = "ota_all.bin";
	```

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

3. Copy the `ota_all.bin` into `tools\DownloadServer(HTTP)`.

4. Configure the server as a http server according to `tools\DownloadServer(HTTP)\readme.txt` first then execute the script.

5. Reset the board and start the download.

# Expect result

A http download example thread will be started automatically when booting.

# Note

The http server script and board need to be on the same local area network.

# Supported IC

* RTL8730E
* RTL8726E
* RTL8720E
* RTL8713E
* RTL8710E
* RTL8721Dx
* RTL8721F
