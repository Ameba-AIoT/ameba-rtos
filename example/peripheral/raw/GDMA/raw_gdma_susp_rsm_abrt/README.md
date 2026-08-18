> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

This example describes how to use GDMA's suspend/resume/abort functions

# HW Configuration

None.

# SW Configuration

`ABORT_TRANSFER` is a compile-time macro; set it via `-D` to select the desired mode:

| `-D` Option | Macro Value | Behavior |
|---|---|---|
| `-D ABORT_TRANSFER=0` | `#define ABORT_TRANSFER 0` | Suspend/Resume mode — suspends and resumes transfer multiple times, then completes normally |
| `-D ABORT_TRANSFER=1` | `#define ABORT_TRANSFER 1` | Abort mode — suspends then aborts the transfer immediately |

Build and Download:
   * `./ameba.py build -a raw_gdma_susp_rsm_abrt -D ABORT_TRANSFER=0`
   * Or `./ameba.py build -a raw_gdma_susp_rsm_abrt -D ABORT_TRANSFER=1`
   * Download images to board by Ameba Image Tool.

# Expected Result

The macro `ABORT_TRANSFER` corresponds to the runtime variable `abort_transfer` in the source code. The log output differs depending on the chosen mode:

1. **Abort mode** (`ABORT_TRANSFER=1`, `abort_transfer = 1`):
```shell
transfer suspend, and 1024 bytes have been moved
transfer abort
transfer over
```
2. **Suspend/Resume mode** (`ABORT_TRANSFER=0`, `abort_transfer = 0`):
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

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
RTL8720F
RTL8735C
