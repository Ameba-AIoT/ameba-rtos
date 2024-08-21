# Example Description

1. This example describes how to use SPI read/write by RAW SPI API.
2. The SPI Interface provides a "Serial Peripheral Interface" Master.
3. This interface can be used for communication with SPI slave devices, such as FLASH memory, LCD screens and other modules or integrated circuits.
4. This example communicates data in dma mode.

This example does not support the loguart command response.

# HW Configuration

In this example, it use 2 sets of SPI. One is master, the other is slave, by default it use SPI0 as slave, and use SPI1 as master. So we connect them as below:

- Connect `SPI0_MOSI` to `SPI1_MOSI`
- Connect `SPI0_MISO` to `SPI1_MISO`
- Connect `SPI0_SCLK `to `SPI1_SCLK`
- Connect `SPI0_CS `to `SPI1_CS`

For example:

- On AmebaSmart, connect as below
  - Connect `SPI0_MOSI (_PA_13)` to `SPI1_MOSI (_PA_5)`
  - Connect `SPI0_MISO (_PA_14)` to `SPI1_MISO (_PA_4)`
  - Connect `SPI0_SCLK (_PA_15)` to `SPI1_SCLK (_PA_3)`
  - Connect `SPI0_CS   (_PA_16)` to `SPI1_CS (_PA_2)`
- On AmebaLite, connect as below
  - Connect `SPI0_MOSI (_PA_29)` to `SPI1_MOSI (_PB_3)`
  - Connect `SPI0_MISO (_PA_30)` to `SPI1_MISO (_PB_4)`
  - Connect `SPI0_SCLK (_PA_28)` to `SPI1_SCLK (_PB_2)`
  - Connect `SPI0_CS   (_PA_31)` to `SPI1_CS   (_PB_5)`
- On AmebaDplus, connect as below
  - Connect `SPI0_MOSI (_PB_24)` to `SPI1_MOSI (_PB_19)`
  - Connect `SPI0_MISO (_PB_25)` to `SPI1_MISO (_PB_20)`
  - Connect `SPI0_SCLK (_PB_23)` to `SPI1_SCLK (_PB_18)`
  - Connect `SPI0_CS (_PB_26)` to `SPI1_CS (_PB_21)`

# SW configuration

- How to compile :
  - Copy main.c and spi_ext.h under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx
- By default, this example select two pinmux groups as MOSI/MISO/SCLK/CS, you can also modify pinmux group settings according to correct pinmux table.
- For example, if you want to modify pinmux group setting in AmebaSmart, then you should refer to `UM0602_RTL8730E_pinmux.xls`.

# Expect result

1. After boot up, Master SPI and slave SPI communicate with each other, and exchanged data will be displayed and compared.
2. Master SPI Transmits data to slave SPI, and data transmitted by master and data received by slave will be displayed and compared.
3. When transmission finishes, the compared result will be notified as "Test result is ok" or "Test result is error".

# Note

1. Para spi_idx should be assigned first in the initialization process where SPI1(0x1)is for Master and SPI0(0x0) is for Slave
2. Under DMA mode, both CPU and DMA have access to SPI buffer.
3. Start address of SPI buffer should be 32-Byte aligned and size of SPI buffer should be multiple of 32 Byte.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
