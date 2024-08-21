SSL SERVER EXAMPLE

Description:
A simple SSL server which can reply for the https request.

Configuration:
1.Modify SERVER_PORT and response content in example_ssl_server.c based on your SSL server.

2.GCC:use CMD "make all EXAMPLE=ssl_server" to compile ssl_server example

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A ssl server example thread will be started automatically when booting.
Then run ssl_client example on another board.

[Supported List]
	Supported :
	    RTL8730A, RTL872xE, RTL871xE
