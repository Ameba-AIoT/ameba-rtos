# Example Description

A simple SSL server which can reply for the https request.

# HW Configuration

None

# SW configuration

1. Modify `SERVER_PORT` and response content in `example_ssl_server_verify_required.c` based on your SSL server.

2. (Optional) Test for TLS1.3.  
   - Type command `./menuconfig.py` and choose `CONFIG SSL`-> `Enable TLS 1.3`

3. How to use:
   - Run `./build.py -a ssl_server_verify_required` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

A ssl server example thread will be started automatically when booting.

Then run `ssl_client_verify_required` example on another board.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
