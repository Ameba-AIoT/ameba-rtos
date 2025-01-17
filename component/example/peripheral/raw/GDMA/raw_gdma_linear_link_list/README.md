# Example Description

This example describes how to use GDMA's linear link-list mode.

# HW Configuration

None.

# SW configuration

Run cmd: `./build.py -a raw_gdma_linear_link_list` in directory {sdk_path}/amebaXXX_gcc_project.

# Expect result
The log is displayed as:
```shell
Block 0 Over
Block 1 Over
Last Block 1 Complete
transfer over
```
It does not match the actual number of blocks. This is normal.
# Note
1. For linear linked lists, transfer the last block and then stop.
2. link-list will prefetch the next block for transmission. The current block transfer is completed and an interrupt is reported to the CPU. At the same time, DMA continues to transmit the next data block without pausing. When the log is printed in the interrupt function, the dma may have been transferred, so the log may not match the actual number of blocks.
3. When the cache is enabled, if the user wants to stop transmission, first disable llp_src_en/llp_dst_en, and then refresh the cache immediately.
# Supported IC

- AmebaLite
- AmebaSmart
- AmebaDplus
- AmebaGreen2
