##################################################################################
#                                                                                #
#                      OTA UPDATING EXAMPLE                                 	 #
#                                                                                #
##################################################################################

Date: 2022-12-30

Table of Contents
~~~~~~~~~~~~~~~~~
 - Description
 - Setup Guide
 - Parameter Setting and Configuration
 - Result description
 - Supported List


Description
~~~~~~~~~~~
    This example is designed for firmware update by Over-the-air programming (OTA) via
        Wireless Network Connection. Download OTA_ALL.bin from the download server
        (in tools\DownloadServer(HTTP) or tools\DownloadServer) automatically.


Setup Guide
~~~~~~~~~~~
		For AmebaLite :AmebaLite Changes how example is compiled, and Removed macro controls (CONFIG_EXAMPLE_XXX)in platform_opts.h
			GCC:use CMD "make all EXAMPLE=ota" to compile ota example.
			
Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		1. For GCC environmnet, type command "make menuconfig" and choose MBED_API => Enable MBED_API.
		   
		2. Set type to OTA_LOCAL to verify downloading firmware from local server.
			ota_update_init(ctx, (char *)host, PORT, (char *)resource, OTA_LOCAL);
			
		3. Set type to OTA_HTTP to verify downloading firmware from http server.
			ota_update_init(ctx, (char *)host, PORT, (char *)resource, OTA_HTTP);
		
		4. Set type to OTA_HTTPS to verify downloading firmware from https server.
			ota_update_init(ctx, (char *)host, PORT, (char *)resource, OTA_HTTPS);
			(1) in \component\ssl\mbedtls-2.16.9\include\mbedtls\config_rsa.h
			#define MBEDTLS_SSL_MAX_CONTENT_LEN 16384 
			#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
			#define MBEDTLS_ECDH_C
		
		5. Set type to OTA_SDCARD to verify downloading firmware from sdcard.
			ota_update_init(ctx, NULL, NULL, (char *)resource, OTA_SDCARD);
		
		6. For sdcard update, only AmebaSmart support.
			(1) users only need to set resource.
			#define RESOURCE    "OTA_ALL.bin" // binary file path in the sdcard.
			(2) Add this line in component\file_system\vfs\vfs_fatfs.h
			#define FATFS_DISK_SD 	 1
			(3) in component\file_system\fatfs\r0.14b\include\ffconf.h
			#define FF_LFN_UNICODE    2
			(4) ensure that SDcard is in FAT32 format
			(5) comment wifi connection if needed
				//while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
				//	ota_printf(_OTA_INFO_, "Wait for WIFI connection ...\n");
				//	rtos_time_delay_ms(1000);
				//}
			(6) Type command "make menuconfig" and choose "CONFIG VFS"
				For VFS_FATFS with VFS_INF_SD: (Only AmebaSmart support)
				[ ] Enable VFS LITTLEFS
				[*] Enable VFS FATFS
				(SD) FATFS Memory Type
		
        7. For http or https update, modify PORT, HOST and RESOURCE based on your download server.
        eg: SERVER: http://m-apps.oss-cn-shenzhen.aliyuncs.com/051103061600.bin
        set:    #define PORT    80
                #define HOST    "m-apps.oss-cn-shenzhen.aliyuncs.com"
                #define RESOURCE    "051103061600.bin"
        For local network download, Set it with IP and OTA_ALL.bin
        e.g.    #define PORT    80
                #define HOST    "192.168.1.100"
                #define RESOURCE    "OTA_ALL.bin"
        Note: Remember to Set the server start.bat with the same PORT and RESOURCE.


Result description
~~~~~~~~~~~~~~~~~~
    Make automatical Wi-Fi connection when booting by using wlan fast connect example.
    A http download example thread will be started automatically when booting.
    Using the example with the tool in tools\DownloadServer(HTTP) with RESOURCE file.


Supported List
~~~~~~~~~~~~~~
[Supported Lis]
        Supported IC:
                AmebaLite, AmebaSmart, AmebaDplus
