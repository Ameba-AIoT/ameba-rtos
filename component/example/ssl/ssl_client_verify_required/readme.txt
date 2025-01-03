SSL DOWNLOAD EXAMPLE(VERIFY_REQUIRED)

Description:
MBEDTLS_SSL client sends request to get https server response.

Configuration:
1. Modify SERVER_HOST, SERVER_PORT, RESOURCE in example_ssl_client_verify_required.c based on your SSL server.

2. Modify SSL_MAX_CONTENT_LEN in SSL config for large size file.
	If the transmitted fils size is larger than 16kbytes, SSL_MAX_CONTENT_LEN should be set to 16384.

	[mbedtls_config.h]
	#define MBEDTLS_SSL_OUT_CONTENT_LEN             16384
	#define MBEDTLS_SSL_IN_CONTENT_LEN              16384

3. (Optional)Test for TLS1.3.
	Enable CONFIG SSL -> Support TLS 1.3 protocol

4. GCC:use CMD "./build.py -a ssl_client_verify_required" to compile ssl_client_verify_required example.


Execution:
Run ssl_server_verify_required example on another board first.
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A ssl client example thread will be started automatically when booting.

[Supported List]
	Supported :
	    RTL8730A, RTL872xE, RTL871xE