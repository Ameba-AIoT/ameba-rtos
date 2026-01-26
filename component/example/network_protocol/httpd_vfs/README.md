# Example Description

This example is based on HTTPD API, an HTTP server example with a simple website and GET method test pages are provided.

However, PUT&POST method_handler is currently not supported, you can add it in `example_httpd_vfs.c`.

In addition, this example can test captive portal.

# HW Configuration

None

# SW configuration

1. Use menuconfig and choose `CONFIG VFS`:
	```C
	[*] Enable VFS FATFS
	[*]     FATFS within APP Image
	```
	Save and exit.

2. Use vfs.py (located in `tools/image_scripts/`) with following command to generate `fatfs.bin` from your html webpages directory `html_demo`. If captive portal is enabled, `html_demo` directory must include `index.html`.
	```
	python vfs.py -t FATFS -s 512 -c 256 -dir html_demo -out fatfs.bin
	```

3. Place `fatfs.bin` under project path, e.g. `amebadplus_gcc_project/`.

4. If you want to enable captive portal, please modify `dhcps.h`.
	```C
	#define CONFIG_ENABLE_CAPTIVE_PORTAL  1
	```

5. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

6. A httpd example thread is started automatically when booting, then you need to set board as ap with the following command:
	```
	AT+WLSTARTAP=ssid,Test_AP,pw,12345678,sec,wpa2
	```

7. Connect your device to board.

# Expect result

If you set `#define CONFIG_ENABLE_CAPTIVE_PORTAL  1`, the browser will automatically open the captive portal. Otherwise, you need to manually enter the IP address of board in the browser.

# Note

None

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
