# Example Description

1. This example describes how to use i2c in dma mode.
2. In this case, dma replaces cpu to transfer data from the buffer to i2c.

# HW Configuration

This test needs two demo board, one as master and the other as slave.

Master board I2C0 SDA (`PB_15`) to Slave board I2C0 SDA (`PB_15`) pin,
Master board I2C0 SCL (`PB_16`) to Slave board I2C0 SCL (`PB_16`) pin.
Master's `GND` connect to Slave's `GND` 

# SW Configuration

1. The codes of Master and Slave are distinguished by the value of macro 'I2C_MASTER_DEVICE' in the code.
2. The codes of Master send Slave read mode and Master read Slave send mode are distinguished by the value of macro 'MASTER_SEND' in the code.
3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
4. How to modify gpio pin:
   you can also modify pinmux group settings according to correct pinmux table.
   for example: 
   	If you want to modify pinmux group setting in AmebaSmart, then you should refer to UM0602_RTL8730E_pinmux.xls

# Expected Result

1. The DMA has correctly handled the data and the i2c has successfully transmitted the data.

# Note

none

# Supported IC

* AmebaDplus
