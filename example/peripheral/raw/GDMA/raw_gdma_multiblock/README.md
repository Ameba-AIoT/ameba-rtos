> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

This example describes how to use GDMA's multi-block transfer mode.

# HW Configuration

None.

# SW Configuration

`GDMA_TYPE` is a compile-time macro; set it via `-D` to select the transfer mode:

| `-D` Option | Macro Value | Behavior |
|---|---|---|
| `-D GDMA_TYPE=ReloadDst` (default) | `#define GDMA_TYPE ReloadDst` | transfer multiple blocks from the source address to the same block at the destination address |
| `-D GDMA_TYPE=ReloadSrc` | `#define GDMA_TYPE ReloadSrc` | transfer one block from the source address to multi blocks at the destination address |
| `-D GDMA_TYPE=ReloadSrcDst` | `#define GDMA_TYPE ReloadSrcDst` | transfer multiple blocks from the source address to multi blocks at the destination address |

> **Note**: `SingleBlock` mode is defined in code but unavailable (assertion fails if used).

Build and Download:
   * `./ameba.py build -a raw_gdma_multiblock -D GDMA_TYPE=ReloadDst` (default)
   * Or `./ameba.py build -a raw_gdma_multiblock -D GDMA_TYPE=ReloadSrc`
   * Or `./ameba.py build -a raw_gdma_multiblock -D GDMA_TYPE=ReloadSrcDst`
   * Download images to board by Ameba Image Tool.

# Expected Result

GDMA will display "DMA data complete MaxMuliBlock = x " in serial port tool.

# Note

1. If only a single block is being transferred, reference raw/single block example.

2.  If the output character sequence of the serial port tool is disordered, please enable `CONFIG_LOGUART_AGG_EN` in menuconfig (`User Config` > `Boot Configuration` > `Enable Loguart AGG`) according to the chip ID.

3. Since the lp core of RTL8730E does not compile GDMA driver code, the compilation of this example will fail on this core. But the core supports GDMA function.

4. If RTL8726E/RTL8720E/RTL8713E/RTL8710E outputs `IO Driver Parameters error! file_name:BOOT_ChipInfo_PSRAMType`, which means that the board is not configured with PSRAM. There are two solutions:
   - Modify the menuconfig:
     - set the `CONFIG LINK option` to `CodeInXip_DataHeapInSram[No_Psram]`
     - save and exit
     - Users need to select the correct **.rdev** file when downloading image.
   - Change the development board, and select 8726E.

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
