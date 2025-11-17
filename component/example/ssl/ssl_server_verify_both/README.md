# Example Description

A simple SSL server which can reply for the https request.

# HW Configuration

None

# SW configuration

1. Modify `SERVER_PORT` and response content in `example_ssl_server_verify_both.c` based on your SSL server.

2. (Optional) Test for TLS1.3.
   - Type command `./menuconfig.py` and choose `CONFIG SSL`-> `Enable TLS 1.3`

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

A ssl server example thread will be started automatically when booting.

Then run `ssl_client_verify_both` example on another board.

# Note

None

# Supported IC

- AmebaSmart
  * RTL8730E
- AmebaLite
  * RTL8726E
  * RTL8720E
  * RTL8713E
  * RTL8710E
- AmebaDplus
  * RTL8721Dx
