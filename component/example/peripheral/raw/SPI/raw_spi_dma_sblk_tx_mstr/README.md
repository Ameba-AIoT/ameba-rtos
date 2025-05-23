# Example Description

1. This example describes how to use SPI GDMA single-block continue RX Slave by RAW SPI API.
2. The SPI Interface provides a "Serial Peripheral Interface" Slave.
3. This interface can be used for communication with SPI master devices, such as FLASH memory, LCD screens and other modules or integrated circuits.

This example does not support the loguart command response.

# HW Configuration

1. In this example, it use SPI0 as slave to receive data, and need to connect with another SPI master device send data as below:
   `Connect Master's MOSI to Slave's MOSI`
   `Connect Master's MISO to Slave's MISO`
   `Connect Master's SCLK to Slave's SCLK`
   `Connect Master's CS to Slave's CS`
   the related Master's and Slave's pins are defined in spi_ext.h
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
  
2. If you have SPI master device, then you can ignore step2(3), and goto step3 directly.
3. We offer another example named "`spi_dma_single_block_continue_rx_slave`", it can be used for SPI slave to communicate data with this example.
4. If you use example of "`spi_dma_single_block_continue_rx_slave`", then you should build its image and download into another EVB board.

# SW configuration

- How to compile :
  - perform cmd: './build.py -a raw_spi_dma_single_block_tx_master' in directory {sdk_path}/amebaXXX_gcc_project  
- By default, this example select one pinmux group as MOSI/MISO/SCLK/CS, you can also modify pinmux group settings according to correct pinmux table.
- For example, if you want to modify pinmux group setting in AmebaSmart, then you should refer to `UM0602_RTL8730E_pinmux.xls`.

# Expect result

1. After example finishes, SPI master will print out "SPI tx Demo finished".
2. If SPI master tx fail, it will notify "SPI Timeout".

# Note

1. Data array send times can be confiured by Macro definition.
2. Under DMA mode, both CPU and DMA have access to SPI buffer.
3. Start address of SPI buffer should be 32-Byte aligned and size of SPI buffer should be multiple of 32 Bytes.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
- AmebaD
