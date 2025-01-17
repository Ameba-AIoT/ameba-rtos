# Example Description

This example describes how to use GDMA's circle link-list mode.

# HW Configuration

None.

# SW configuration

Run cmd: `./build.py -a raw_gdma_circle_link_list` in directory {sdk_path}/amebaXXX_gcc_project.

# Expect result
1. If `abort_trasfer = 1`, the log is displayed as:
```shell
Block 0 Over
Last Block 0 is over, and transmission terminated

Block 1 Over
Last Block 1 is over, and transmission terminated

transfer over
```
2. If `abort_transfer = 0`, the log is displayed as:
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

- AmebaLite
- AmebaSmart
- AmebaDplus
- AmebaGreen2
