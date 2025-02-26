# Example Description

This example is for key-value module which wraps littlefs.

# HW Configuration

None

# SW configuration

How to use:
   - Run `./build.py -a kv` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

1. Set key-value pair (key = TEST_FILE, value = KV module works normally!!!) success with value length = 28

2. Get key-value pair success with value length = 28

3. Delete key-value pair success

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
