# Example Description

Example for IPV6.

# HW Configuration

Two Ameba development boards are needed. Choose one board to works as the server, and another board works as the client.

# SW configuration

1. `component/lwip/lwip_2.1.2/src/include/lwip/opt.h`
	```C
	#define MEMP_NUM_MLD6_GROUP			6
	```
	If MEMP_NUM_MLD6_GROUP is not big enough, it may occur
	```
	[ERROR] Register to ipv6 multicast group failed
	```

2. `example_ipv6.h`
	```C
	#define UDP_SERVER_IP    "fe80:0000:0000:0000:cd3c:24de:386d:9ad1"
	#define TCP_SERVER_IP    "fe80:0000:0000:0000:cd3c:24de:386d:9ad1"
	```
	Change the ipv6 address based on output of server example to run client.

	Select test type for IPV6.

	- Test IPV6 UDP:
		```C
		#define EXAMPLE_IPV6_UDP    1
		```
		for server:

		```C
		#define UDP_SERVER          1
		```
		for client

		```C
		#define UDP_SERVER          0
		```

	- Test IPV6 UDP:
		```C
		#define EXAMPLE_IPV6_UDP    1
		```
		for server:

		```C
		#define TCP_SERVER          1
		```
		for client

		```C
		#define TCP_SERVER          0
		```
	- Test IPV6 MCAST:
		```C
		#define EXAMPLE_IPV6_MCAST  1
		```
		for server:

		```C
		#define MCAST_SERVER        1
		```
		for client:

		```C
		#define MCAST_SERVER        0
		```
	
	`EXAMPLE_IPV6_UDP`, `EXAMPLE_IPV6_TCP` and `EXAMPLE_IPV6_MCAST` should be enabled one at a time.

3. How to use:
   - Run `./build.py -a ipv6` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

A IPV6 example thread will be started automatically when booting.

Need to run server example first to get the server IP and use this IP for client example.

# Note

Enable one example at one time to verify client or server examples.

Modify `UDP_SERVER_IP` and `TCP_SERVER_IP` for client example according to server output.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
