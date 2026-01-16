# Example Description

1. This example describes how to use SPI GDMA single-block continue RX Slave by RAW SPI API.
2. The SPI Interface provides a "Serial Peripheral Interface" Slave.
3. This interface can be used for communication with SPI master devices, such as FLASH memory, LCD screens and other modules or integrated circuits.

# HW Configuration

1. In this example, it use SPI0 as slave to receive data, and need to connect with another SPI master device send data as below:
   `Connect Master's MOSI to Slave's MOSI`
   `Connect Master's MISO to Slave's MISO`
   `Connect Master's SCLK to Slave's SCLK`
   `Connect Master's CS to Slave's CS`
   The related Master's and Slave's pins are defined in the current example's header file.
   - The relevant header file can be found in the directory:
     `component/example/peripheral/{mebd|raw}/{peripheral_name}/{example_folder_name}/`
   In this header file, the macros with prefix of SPI1 are master's pins, and macros with prefix of SPI0 are slave's pins.

   For example:

- On RTL8730E, connect as below
  - Connect `SPI0_MOSI (_PA_13)` to `SPI1_MOSI (_PA_5)`
  - Connect `SPI0_MISO (_PA_14)` to `SPI1_MISO (_PA_4)`
  - Connect `SPI0_SCLK (_PA_15)` to `SPI1_SCLK (_PA_3)`
  - Connect `SPI0_CS   (_PA_16)` to `SPI1_CS (_PA_2)`
- On RTL8726E/RTL8720E/RTL8713E/RTL8710E, connect as below
  - Connect `SPI0_MOSI (_PA_29)` to `SPI1_MOSI (_PB_3)`
  - Connect `SPI0_MISO (_PA_30)` to `SPI1_MISO (_PB_4)`
  - Connect `SPI0_SCLK (_PA_28)` to `SPI1_SCLK (_PB_2)`
  - Connect `SPI0_CS   (_PA_31)` to `SPI1_CS   (_PB_5)`
- On RTL8721Dx, connect as below
  - Connect `SPI0_MOSI (_PB_24)` to `SPI1_MOSI (_PB_19)`
  - Connect `SPI0_MISO (_PB_25)` to `SPI1_MISO (_PB_20)`
  - Connect `SPI0_SCLK (_PB_23)` to `SPI1_SCLK (_PB_18)`
  - Connect `SPI0_CS (_PB_26)` to `SPI1_CS (_PB_21)`
- On RTL8721F, connect as below
  - Connect `SPI0_MOSI (_PA_30)` to `SPI1_MOSI (_PB_30)`
  - Connect `SPI0_MISO (_PA_31)` to `SPI1_MISO (_PB_31)`
  - Connect `SPI0_SCLK (_PA_29)` to `SPI1_SCLK (_PB_29)`
  - Connect `SPI0_CS (_PB_0)` to `SPI1_CS (_PC_0)`

2. If you have SPI master device, choose your own master's pins to connect instead of master's pins mentioned in step1, then ignore step3 and goto step4 of HW Configuration. Otherwise goto step3.
3. We offer another example named "`raw_spi_dma_sblk_tx_mstr`", it can be used as SPI master device to communicate data with this example.
Refer the README of example `raw_spi_dma_sblk_tx_mstr` to build master image and download into another EVB board.
4. Reset Slave device first and then Master device.

# SW configuration

- Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
- By default, this example select one pinmux group as MOSI/MISO/SCLK/CS,
  you can also modify pinmux group settings according to correct pinmux table.
- For example, if you want to modify pinmux group setting in RTL8730E, then you should refer to `UM0602_RTL8730E_pinmux.xls`

# Expect result

1. After example finishes, SPI slave will print out "SPI rx Demo finished"
2. If SPI master is selected as mentioned by step2(3), then "SPI RX test: OK!" will be shown after data received match sent.

# Note

1. Data array send times can be confiured by Macro definition.
2. Under DMA mode, both CPU and DMA have access to SPI buffer.
3. Start address of SPI buffer should be 32-Byte aligned and size of SPI buffer should be multiple of 32 Byte.
4. Data buffer size must be integer multiple of block size.(e.g. data size = 8192, block size = 2048, block size must be less than 4096)

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
