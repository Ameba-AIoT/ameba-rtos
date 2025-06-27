# Example Description

This example is based on HTTPD API, an HTTP/HTTPS server example with a simple homepage and GET/POST method test pages are provided.

# HW Configuration

None

# SW configuration

1. Both HTTP and HTTPS are supported by this exmaple, and can be changed by modifying `USE_HTTPS` in `example_httpd.c`.
	```C
	#define USE_HTTPS    0 //for HTTP
	#define USE_HTTPS    1 //for HTTPS
	```

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

This example is a httpd example thread is started automatically when booting.

Can test with a Web browser connecting to the homepage of httpd server.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
- AmebaGreen2
