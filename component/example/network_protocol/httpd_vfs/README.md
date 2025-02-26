# Example Description

This example is based on HTTPD API, an HTTP server example with a simple website and GET method test pages are provided.

However, PUT&POST method_handler is currently not supported, you can add it in `example_httpd_vfs.c`.

In addition, this example can test captive portal.

# HW Configuration

None

# SW configuration

1. In `project/realtek_ameba<test board>_va0_example/src/src_<ap core>/main.c` `app_filesystem_init` interface, CONFIG `vfs_type` from `VFS_FATFS` to `VFS_LITTLEFS`:
	```C
	ret = vfs_user_register(VFS_PREFIX, VFS_LITTLEFS, VFS_INF_FLASH, VFS_REGION_1, VFS_RW);
	```

2. If you want to enable captive portal, please modify `dhcps.h`.
	```C
	#define CONFIG_ENABLE_CAPTIVE_PORTAL  1
	```

3. How to compile:
   - Run `./build.py -a httpd_vfs` under project path, e.g. `amebasmart_gcc_project/`, to generate images.

4. When you burn images, you need to burn `html.bin`. You can use the following command to generate html.bin with mklittlefs:
	```
	./mklittlefs -c html_demo/ -d 0 -b 4096 -p 1024 -s 131072 html.bin
	```
   - For AmebaDplus or AmebaLite, burn address is 0x08703000 and size is 128KB.
   - For AmebaSmart, burn address is 0x08623000 and size is 128KB.

5. A httpd example thread is started automatically when booting, then you need to set board as ap with the following command:
	```
	AT+WLSTARTAP=ssid,Test_AP,pw,12345678,sec,wpa2
	```

6. Connect your device to board.

# Expect result

If you set `#define CONFIG_ENABLE_CAPTIVE_PORTAL  1`, the browser will automatically open the captive portal. Otherwise, you need to manually enter the IP address of board in the browser.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
