# Example Description

This example describes how to use GDMA's suspend/resume/abort functions

# HW Configuration

None.

# SW configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result
1. If `abort_trasfer = 1`, the log is displayed as:
```shell
transfer suspend, and 1024 bytes have been moved
transfer abort
transfer over
```
2. If `abort_transfer = 0`, the log is displayed as:
```shell
transfer suspend, and 1024 bytes have been moved
transfer resume
transfer suspend, and 1024 bytes have been moved
transfer resume
Block 0 Over

Last Block 0 Complete
transfer over
```

# Note
1. If users want to use the suspend/resume function, they need to ensure that the size of a single block is greater than 768 bytes.
2. `gdma_dump_memory` can be used to check memory.
3. When suspended, the actual amount of data transferred depends on the transfer speed, not the fixed 1024.
# Supported IC

- AmebaLite
- AmebaSmart
- AmebaDplus
- AmebaGreen2