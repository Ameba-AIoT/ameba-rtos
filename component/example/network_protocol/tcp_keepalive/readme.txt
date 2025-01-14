LWIP TCP KEEPALIVE EXAMPLE

Description:
Example for socket client and server implementation to break out of blocking read by TCP keepalive timeout.

Configuration:
1.[lwipopts.h]
    #define LWIP_TCP_KEEPALIVE               1

2.Setup TEST_MODE in example_tcp_keepalive.c to verify client or server examples. Modify SERVER_IP and SERVER_PORT for client keepalive test. Modify SERVER_PORT for server keepalive test. Modify keepalive timeout by setting keepalive idle, interval and count value to socket options based on requirement.
	(a) For client TCP keepalive example, example thread connects to indicated server and continue to perform blocking read.
	When using ATWD command to disconnect wifi, client thread will break out of blocking read after keepalive timeout

	(b). For server TCP keepalive example, example thread listens on indicated port and read data from the accepted connection.
	Please use a client program to connect to server port of example thread 
	When using ATWD command to disconnect wifi, server thread will get select read event after keepalive timeout

3.GCC:use CMD "make all EXAMPLE=tcp_keepalive" to compile tcp_keepalive example.

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A TCP keepalive example thread will be started automatically when booting.

[Supported List]
	Supported :
		RTL8730A, RTL872XE
