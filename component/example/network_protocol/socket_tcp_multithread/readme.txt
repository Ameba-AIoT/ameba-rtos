##################################################################################
#                                                                                #
#                           example_socket_tcp_multithread                       #
#                                                                                #
##################################################################################

Table of Contents
~~~~~~~~~~~~~~~~~
 - Description
 - Setup Guide
 - Parameter Setting and Configuration
 - Result description
 - Supported List

 
Description
~~~~~~~~~~~
    Example of TCP bidirectional transmission with use two threads for TCP tx/rx on one socket.
    Example uses non-blocking recv and semaphore for TCP send/recv mutex.

Setup Guide
~~~~~~~~~~~
        1. For GCC project,  use CMD "make all EXAMPLE=socket_tcp_multithread" to compile socket_tcp_multithread example.

			
Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Modify SERVER_PORT in example_socket_tcp_multithread.c for listen port.
    e.g. #define SERVER_PORT 5001
    Make automatical Wi-Fi connection when booting by using wlan fast connect example.
	1. suggest test with client which has both send and recv.
	2. test with iperf client:
		1) modify send into non-blocking
			ret = send(client_fd, buffer, sizeof(buffer), MSG_DONTWAIT);
		2) iperf -c <tcp_server_IP_address> -i 1 -t 60
		

Result description
~~~~~~~~~~~~~~~~~~
    A socket TCP trx example thread will be started automatically when booting. 
    A TCP server will be started to wait for connection.

Supported List
~~~~~~~~~~~~~~
[Supported List]
        Supported IC :
                RTL8730A, RTL872XE
