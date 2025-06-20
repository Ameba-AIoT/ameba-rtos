# Example Description

This example describes how to use GDMA's single-block transfer mode.

# HW Configuration

None.

# SW configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

GDMA will display "DMA Copy Memory Compare OK!! " in serial port tool. Otherwise, output info is "DMA Copy Memory Compare Err, xxx-th value1 value2";

# Note

1. If multi blocks is being transferred, reference raw/multi block example.

2. If the output character sequence of the serial port tool is disordered, please configure `Boot_Agg_En = TRUE` in component/soc/xx/usrcfg/ameba_bootcfg.c according to the chip ID.
   - AmebaSmart, amebasmart/usrcfg/ameba_bootcfg.c
   - AmebaLite, amebalite/usrcfg/ameba_bootcfg.c
   - AmebaDplus, amebadplus/usrcfg/ameba_bootcfg.c
3. If the output information contains " irq[xx] priority 10 shall <= 7" , it means that the GDMA interrupt priority setting is abnormal and should be set below 7.

4. Since the lp core of AmebaSmart does not compile GDMA driver code, the compilation of this example will fail on this core. But the core supports GDMA function.

5. If AmebaLite outputs `IO Driver Parameters error! file_name:BOOT_ChipInfo_PSRAMType`, which means that the board is not configured with PSRAM. There are two solutions:
   - Modify the menuconfig:
     - enter `./menuconfig.py` under the **{SDK}\amebaxxx_gcc_project** folder
     - set the `CONFIG LINK option` to `CodeInXip_DataHeapInSram[No_Psram]`
     - save and exit
     - Users need to select the correct **.rdev** file when downloading image
   - Change the development board, and select 8726E.
# Supported IC

- AmebaLite
- AmebaSmart
- AmebaDplus
