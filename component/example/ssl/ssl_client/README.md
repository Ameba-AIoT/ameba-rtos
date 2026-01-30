# Example Description

`MBEDTLS_SSL` client sends request to get https server response.

# HW Configuration

None

# SW configuration

1. Modify `SERVER_HOST`, `SERVER_PORT` and `RESOURCE` in `example_ssl_client.c` based on your SSL server.

2. Modify `SSL_MAX_CONTENT_LEN` in SSL config for large size file.  
If the transmitted fils size is larger than 16kbytes, `SSL_MAX_CONTENT_LEN` should be set to 16384.
   - Use menuconfig and choose `CONFIG SSL`-> `Maximum len of incoming fragments` -> 16384.

3. (Optional) Test for TLS1.3.
   - Use menuconfig and choose `CONFIG SSL`-> `Enable TLS 1.3`

4. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

Run `ssl_server` example on another board first.

A ssl client example thread will be started automatically when booting.

# Note

None

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
