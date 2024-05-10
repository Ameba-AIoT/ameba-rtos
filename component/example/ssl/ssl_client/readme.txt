SSL CLIENT EXAMPLE

Description:
MBEDTLS_SSL client sends request to get https server response.

Configuration:
1. Modify SERVER_HOST, SERVER_PORT and RESOURCE in example_ssl_client.c based on your SSL server.

2. Modify SSL_MAX_CONTENT_LEN in SSL config for large size file.
	If the transmitted fils size is larger than 16kbytes, SSL_MAX_CONTENT_LEN should be set to 16384.

	[config_rsa.h]
	#define MBEDTLS_SSL_MAX_CONTENT_LEN            16384

3. GCC:use CMD "make all EXAMPLE=ssl_client" to compile ssl_client example.


Execution:
Run ssl_server example on another board first.
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A ssl client example thread will be started automatically when booting.

[Supported List]
	Supported :
	    RTL8730A, RTL872xE, RTL871xE