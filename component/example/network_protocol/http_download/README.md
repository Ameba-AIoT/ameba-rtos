# Example Description

This example downloads file from Web server via http.

# HW Configuration

None

# SW configuration

1. Modify `SERVER_HOST`, `SERVER_PORT` and `RESOURCE` in `example_http_download.c` based on your HTTP server

2. How to use:
   - Run `./build.py -a http_download` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

A http download example thread will be started automatically when booting.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
