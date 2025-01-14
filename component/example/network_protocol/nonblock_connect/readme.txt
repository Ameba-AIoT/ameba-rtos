LWIP SOCKET NONBLOCKING CONNECT EXAMPLE

Description:
TCP nonblocking connect with use of select() for connection timeout handling.

Configuration:
1.[lwipopts.h]
	#define LWIP_SOCKET_SET_ERRNO 1

2.Modify SERVER_IP and SERVER_PORT in example_nonblock_connect.c for server connection

3.GCC: use CMD "make all EXAMPLE=nonblock_connect" to compile nonblock_connect example.

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A socket nonblocking connect example thread will be started automatically when booting.

[Supported List]
	Supported :
	    RTL8730A, RTL872XE