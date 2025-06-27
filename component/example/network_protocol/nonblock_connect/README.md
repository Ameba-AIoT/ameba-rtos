# Example Description

TCP nonblocking connect with use of `select()` for connection timeout handling.

# HW Configuration

None

# SW configuration

1. `lwipopts.h`
	```C
	#define LWIP_SOCKET_SET_ERRNO 1
	```

2. Modify `SERVER_IP` and `SERVER_PORT` in `example_nonblock_connect.c` for server connection

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

A socket nonblocking connect example thread will be started automatically when booting.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
- AmebaGreen2
