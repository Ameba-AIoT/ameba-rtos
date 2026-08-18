> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

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
     `example/peripheral/{mebd|raw}/{peripheral_name}/{example_folder_name}/`
   Both master's and slave's macros use common SPI pin names (SPI_MOSI, SPI_MISO, SPI_SCLK, SPI_CS) since both sides utilize SPI0.

   Two boards are required for this test. Connect the SPI0 pins of the slave board to the SPI0 pins of the master board:

- On RTL8730E, connect as below
  - Connect `SPI_MOSI (_PA_13)` [Slave] to `SPI_MOSI (_PA_13)` [Master]
  - Connect `SPI_MISO (_PA_14)` [Slave] to `SPI_MISO (_PA_14)` [Master]
  - Connect `SPI_SCLK (_PA_15)` [Slave] to `SPI_SCLK (_PA_15)` [Master]
  - Connect `SPI_CS   (_PA_16)` [Slave] to `SPI_CS   (_PA_16)` [Master]
- On RTL8726E/RTL8720E/RTL8713E/RTL8710E, connect as below
  - Connect `SPI_MOSI (_PA_29)` [Slave] to `SPI_MOSI (_PA_29)` [Master]
  - Connect `SPI_MISO (_PA_30)` [Slave] to `SPI_MISO (_PA_30)` [Master]
  - Connect `SPI_SCLK (_PA_28)` [Slave] to `SPI_SCLK (_PA_28)` [Master]
  - Connect `SPI_CS   (_PA_31)` [Slave] to `SPI_CS   (_PA_31)` [Master]
- On RTL8721Dx, connect as below
  - Connect `SPI_MOSI (_PB_24)` [Slave] to `SPI_MOSI (_PB_24)` [Master]
  - Connect `SPI_MISO (_PB_25)` [Slave] to `SPI_MISO (_PB_25)` [Master]
  - Connect `SPI_SCLK (_PB_23)` [Slave] to `SPI_SCLK (_PB_23)` [Master]
  - Connect `SPI_CS   (_PB_26)` [Slave] to `SPI_CS   (_PB_26)` [Master]
- On RTL8721F, connect as below
  - Connect `SPI_MOSI (_PA_30)` [Slave] to `SPI_MOSI (_PA_30)` [Master]
  - Connect `SPI_MISO (_PA_31)` [Slave] to `SPI_MISO (_PA_31)` [Master]
  - Connect `SPI_SCLK (_PA_29)` [Slave] to `SPI_SCLK (_PA_29)` [Master]
  - Connect `SPI_CS   (_PB_0)`  [Slave] to `SPI_CS   (_PB_0)`  [Master]
- On RTL8720F, connect as below
  - Connect `SPI_MOSI (_PA_8)`  [Slave] to `SPI_MOSI (_PA_8)`  [Master]
  - Connect `SPI_MISO (_PA_9)`  [Slave] to `SPI_MISO (_PA_9)`  [Master]
  - Connect `SPI_SCLK (_PA_7)`  [Slave] to `SPI_SCLK (_PA_7)`  [Master]
  - Connect `SPI_CS   (_PA_10)` [Slave] to `SPI_CS   (_PA_10)` [Master]

1. If you have SPI master device, choose your own master's pins to connect instead of master's pins mentioned in step1, then ignore step3 and goto step4 of HW Configuration. Otherwise goto step3.
2. We offer another example named "`raw_spi_dma_sblk_tx_mstr`", it can be used as SPI master device to communicate data with this example.
Refer the README of example `raw_spi_dma_sblk_tx_mstr` to build master image and download into another EVB board.
1. Reset Slave device first and then Master device.

# SW Configuration

- Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
- By default, this example select one pinmux group as MOSI/MISO/SCLK/CS,
  you can also modify pinmux group settings according to correct pinmux table.
- For example, if you want to modify pinmux group setting in RTL8730E, then you should refer to `UM0602_RTL8730E_pinmux.xls`

# Expected Result

1. After example finishes, SPI slave will print out "SPI rx Demo finished"
2. If SPI master is selected as mentioned by step2(3), then "SPI RX test: OK!" will be shown after data received match sent.

# Note

1. Data array send times can be confiured by Macro definition.
2. Under DMA mode, both CPU and DMA have access to SPI buffer.
3. Start address of SPI buffer should be cacheline size aligned and size of SPI buffer should be multiple of 32 Byte.
4. Data buffer size must be integer multiple of block size.(e.g. data size = 8192, block size = 2048, block size must be less than 4096)

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
RTL8720F
