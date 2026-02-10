# Example Description

This example demonstrates the following capabilities of A2C:

- **Loop back mode**
- **FIFO mode throughput performance**
- **stress testing**

# HW Configuration

No hardware configuration required.

# SW Configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect Result

The console will output performance statistics every 1000 received frames. You must verify the **Last ID** to check for packet loss.

## 1. How to verify Packet Loss
Since the TX thread sends sequential IDs (0, 1, ..., 255, 0, ...), and we print every **1000** frames:
*   **Expected ID Shift**: `1000 mod 256 = 232` (or `-24` in decimal).
*   **Rule**: The `Last ID` of the current log should be **24 (0x18) less** than the previous log's `Last ID`.

## 2. Successful Test (Zero Packet Loss)
In a stable system, the log should look like this (ID decreases by 0x18 each time):

```shell
A2C RX Thread Started...
[Perf] Total RX: 1000 frames. Last ID: 0xe7
[Perf] Total RX: 2000 frames. Last ID: 0xcf  <-- (0xE7 - 0x18 = 0xCF)
[Perf] Total RX: 3000 frames. Last ID: 0xb7  <-- (0xCF - 0x18 = 0xB7)
[Perf] Total RX: 4000 frames. Last ID: 0x9f  <-- (0xB7 - 0x18 = 0x9F)
...
```
# Note

**Capture protocol frames**: connect the TX/RX pins of the A2C to an external transceiver, and then connect the transceiver's H/L pins to the analyzer's H/L pins.

The pins used in this example are as follows.

| Pin | Function | Note |
| :--- | :--- | :--- |
| **PA 26** | A2C0 RX | Connect to Transceiver RX (if Ext. Loopback) |
| **PA 25** | A2C0 TX | Connect to Transceiver TX (if Ext. Loopback) |
| **STB** | Standby Control | Pull LOW for Normal Mode (if supported by PHY) |

Users can modify the `example_a2c_ext.h` file according to the actual pins used by the A2C on their development board.

# Supported IC

RTL8721F
