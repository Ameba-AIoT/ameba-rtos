# Example Description

Example of TCP bidirectional transmission with use two threads for TCP tx/rx on one socket.

Example uses non-blocking recv and semaphore for TCP send/recv mutex.

# HW Configuration

None

# SW configuration

1. Modify `SERVER_PORT` in `example_socket_tcp_multithread.c` for listen port. e.g.
	```C
	#define SERVER_PORT 5001
	```

2. How to use:
   - Run `./build.py -a socket_tcp_multithread` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

3. suggest test with client which has both send and recv.

4. test with iperf client:
	1. modify send into non-blocking
		```C
		ret = send(client_fd, buffer, sizeof(buffer), MSG_DONTWAIT);
		```
	2. modify recv into blocking and no-delay
		```C
		ret = recv(client_fd, buffer, sizeof(buffer), 0);
		...
		// rtos_time_delay_ms(10);
		```
	3. `iperf -c <tcp_server_IP_address> -i 1 -t 60`

# Expect result

A socket TCP trx example thread will be started automatically when booting.

A TCP server will be started to wait for connection.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
