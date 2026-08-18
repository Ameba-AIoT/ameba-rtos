> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

This example describes how to use GDMA's circle link-list mode.

# HW Configuration

None.

# SW Configuration

`ABORT_TRANSFER` is a compile-time macro; set it via `-D` to select the desired mode:

| `-D` Option | Macro Value | Behavior |
|---|---|---|
| `-D ABORT_TRANSFER=0` | `#define ABORT_TRANSFER 0` | Circle mode — transmission runs forever without stopping |
| `-D ABORT_TRANSFER=1` | `#define ABORT_TRANSFER 1` | Stop mode — terminates after each block completes |

Build and Download:
   * `./ameba.py build -a raw_gdma_circle_link_list -D ABORT_TRANSFER=0`
   * Or `./ameba.py build -a raw_gdma_circle_link_list -D ABORT_TRANSFER=1`
   * Download images to board by Ameba Image Tool.

# Expected Result

The macro `ABORT_TRANSFER` corresponds to the runtime variable `abort_transfer` in the source code. The log output differs depending on the chosen mode:

1. **Stop mode** (`ABORT_TRANSFER=1`, `abort_transfer = 1`):
```shell
Block 0 Over
Last Block 0 is over, and transmission terminated

Block 1 Over
Last Block 1 is over, and transmission terminated

transfer over
```
2. **Circle mode** (`ABORT_TRANSFER=0`, `abort_transfer = 0`):
```shell
Block 0 Over
Block 1 Over
Block 2 Over
Block 3 Over
Block 4 Over
...
```
Because it is a circular linked list, the transmission will not stop.

# Note
1. For linear linked lists, transfer the last block and then stop.For the circle linked list, if it needs to be stopped, the user needs to clear the llp_src_en and llp_dst_en enable bits.
2. link-list will prefetch the next block for transmission. The current block transfer is completed and an interrupt is reported to the CPU. At the same time, dma continues to transfer the next one without pausing. Therefore, if you want to stop transmission in the current block, it will not stop until the next block transmission is completed.
3. When the cache is enabled, if the user wants to stop transmission, first disable llp_src_en/llp_dst_en, and then refresh the cache immediately.
# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
RTL8720F
RTL8735C
