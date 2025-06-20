# Example Description

Example for socket client and server implementation to break out of blocking read by TCP keepalive timeout.

# HW Configuration

None

# SW configuration

1. Setup `TEST_MODE` in `example_tcp_keepalive.c` to verify client or server examples. Modify `SERVER_IP` and `SERVER_PORT` for client keepalive test. Modify `SERVER_PORT` for server keepalive test. Modify keepalive timeout by setting keepalive idle, interval and count value to socket options based on requirement.
	1. For client TCP keepalive example, example thread connects to indicated server and continue to perform blocking read.  
	When using `AT+WLDISCONN` command to disconnect wifi, client thread will break out of blocking read after keepalive timeout.

	2. For server TCP keepalive example, example thread listens on indicated port and read data from the accepted connection.  
	Please use a client program to connect to server port of example thread.  
	When using `AT+WLDISCONN` command to disconnect wifi, server thread will get select read event after keepalive timeout.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

A TCP keepalive example thread will be started automatically when booting.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
