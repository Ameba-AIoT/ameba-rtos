# Example Description

Based on HTTPC API, an HTTP/HTTPS client example to access httpbin.org for test are provided

# HW Configuration

None

# SW configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

A httpc example thread is started automatically when booting.

GET to http://httpbin.org/get and POST to http://httpbin.org/post will be verified.

Both HTTP and HTTPS are supported by this exmaple, and can be changed by modifying `USE_HTTPS`.

Should link PolarSSL `bignum.c` to SRAM to speed up SSL handshake for HTTPS client.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
