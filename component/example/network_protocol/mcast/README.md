# Example Description

This example will join multicast group of 224.0.0.251 and listen on port 5353.

Send packet with the content of received packet to multicast group of 224.0.0.251

# HW Configuration

None

# SW configuration

How to use:
   - Run `./build.py -a mcast` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

A multicast example thread will be started automatically when booting.

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
