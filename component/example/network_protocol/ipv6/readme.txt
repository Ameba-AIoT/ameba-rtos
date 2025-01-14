IPV6 EXAMPLE

Description:
Example for IPV6.

Configuration:
1.[lwipopts.h]
    	#define LWIP_IPV6					1

2. [src/include/lwip/opt.h]
	#define MEMP_NUM_MLD6_GROUP         6
If MEMP_NUM_MLD6_GROUP is not big enough, it may occur "[ERROR] Register to ipv6 multicast group failed"

3.[example_ipv6.h]
	#define UDP_SERVER_IP    "fe80:0000:0000:0000:cd3c:24de:386d:9ad1"
	#define TCP_SERVER_IP    "fe80:0000:0000:0000:cd3c:24de:386d:9ad1"
Change the ipv6 address based on output of server example to run client.

4.[example_ipv6.c]  Users can only enable one example at one time.
      example_ipv6_udp_server();
//    example_ipv6_tcp_server();
//    example_ipv6_mcast_server();
//    example_ipv6_udp_client();
//    example_ipv6_tcp_client();
//    example_ipv6_mcast_client();

5.GCC:use CMD "make all EXAMPLE=ipv6" to compile ipv6 example.

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A IPV6 example thread will be started automatically when booting.
Need to run server example first to get the server IP and use this IP for client example.

Enable one example at one time to verify client or server examples. Modify UDP_SERVER_IP and TCP_SERVER_IP for client example according to server output.

[Supported List]
	RTL8730A, RTL872XE
