# Example Description

`MBEDTLS_SSL` client sends request to get https server response.

# HW Configuration

None

# SW configuration

1. Modify `SERVER_HOST`, `SERVER_PORT`, `RESOURCE` in `example_ssl_client_verify_required.c` based on your SSL server.

2. Modify `SSL_MAX_CONTENT_LEN` in SSL config for large size file.  
If the transmitted fils size is larger than 16kbytes, `SSL_MAX_CONTENT_LEN` should be set to 16384.
   - `mbedtls_config.h`
		```C
		#define MBEDTLS_SSL_OUT_CONTENT_LEN             16384
		#define MBEDTLS_SSL_IN_CONTENT_LEN              16384
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
