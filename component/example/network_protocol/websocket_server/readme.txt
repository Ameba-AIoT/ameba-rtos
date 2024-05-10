Websocket Server Example

Description:
A simple websocket server example which echo all the messages.


Configuration:
1.Both WS server and WSS server are supported by this exmaple, and can be changed by modifying USE_WSS in example_ws_server.c.
	[example_ws_server.c]
		#define USE_WSS    1 //acting as the WSS server
		#define USE_WSS    0 //acting as the WS server
		
2.If acting as the WSS server:
	[config_rsa.h]
		#define MBEDTLS_CERTS_C
		#define MBEDTLS_SSL_SRV_C
		#define MBEDTLS_X509_CRT_PARSE_C	

3.GCC:use CMD "make all EXAMPLE=websocket_server" to compile websocket_server example

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A websocket server example thread will be started automatically when booting.
If the connection failed, you can try to modify configTOTAL_HEAP_SIZE in FreRTOSConfig.h to make sure the heap size is enough	


[Supported List]
	Supported :
	     RTL8730A, RTL872XE