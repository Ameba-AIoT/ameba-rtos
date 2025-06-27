# Example Description

This example describes how to use GDMA's multi-block transfer mode.

# HW Configuration

None.

# SW configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
2. In order to deal with different situation, modify GdmaType variable, and it can be set:
   - **ReloadDst**, transfer multiple blocks from the source address to the same block at the destination address.
   - **ReloadSrc**, transfer one block from the source address to multi blocks at the destination address.
   - **ReloadSrcDst**,  transfer multiple blocks from the source address to multi blocks at the destination address.
   - **SingleBlock**, unavailable.

# Expect result

GDMA will display "DMA data complete MaxMuliBlock = x " in serial port tool.

# Note

1. If only a single block is being transferred, reference raw/single block example.

2.  If the output character sequence of the serial port tool is disordered, please configure `Boot_Agg_En = TRUE` in ameba_bootcfg.c according to the chip ID.

3. Since the lp core of AmebaSmart does not compile GDMA driver code, the compilation of this example will fail on this core. But the core supports GDMA function.

4. If AmebaLite outputs `IO Driver Parameters error! file_name:BOOT_ChipInfo_PSRAMType`, which means that the board is not configured with PSRAM. There are two solutions:
   - Modify the menuconfig:
     - enter `./menuconfig.py` under the **{SDK}\amebaxxx_gcc_project** folder
     - set the `CONFIG LINK option` to `CodeInXip_DataHeapInSram[No_Psram]`
     - save and exit
     - Users need to select the correct **.rdev** file when downloading image.
   - Change the development board, and select 8726E.

# 6. Supported IC

- AmebaLite
- AmebaSmart
- AmebaDplus
- AmebaGreen2