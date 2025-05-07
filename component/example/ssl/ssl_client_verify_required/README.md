# Example Description

`MBEDTLS_SSL` client sends request to get https server response.

# HW Configuration

None

# SW configuration

1. Modify `SERVER_HOST`, `SERVER_PORT`, `RESOURCE` in `example_ssl_client_verify_required.c` based on your SSL server.

2. If encounter the following errors in an SSL connection. Set `MBEDTLS_SSL_IN_CONTENT_LEN` by `./menuconfig.py` and choose `CONFIG SSL`-> `Maximum len of incoming fragments` -> set large size.
	```C
	#define MBEDTLS_ERR_SSL_BAD_INPUT_DATA                    -0x7100
	#define MBEDTLS_ERR_SSL_INVALID_RECORD                    -0x7200
	```

3. (Optional) Test for TLS1.3.
   - Type command `./menuconfig.py` and choose `CONFIG SSL`-> `Enable TLS 1.3`

4. How to use:
   - Run `./build.py -a ssl_client_verify_required` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

Run `ssl_server_verify_required` example on another board first.

A ssl client example thread will be started automatically when booting.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
