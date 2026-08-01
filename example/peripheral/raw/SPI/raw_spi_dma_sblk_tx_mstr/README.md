> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

1. This example describes how to use SPI GDMA single-block continue TX Master by RAW SPI API.
2. The SPI Interface provides a "Serial Peripheral Interface" Master.
3. This interface can be used for communication with SPI slave devices, such as FLASH memory, LCD screens and other modules or integrated circuits.

# HW Configuration

1. In this example, it use SPI0 as master to send data, and need to connect with another SPI slave device to receive data as below:
   `Connect Master's MOSI to Slave's MOSI`
   `Connect Master's MISO to Slave's MISO`
   `Connect Master's SCLK to Slave's SCLK`
   `Connect Master's CS to Slave's CS`
   The related Master's and Slave's pins are defined in the current example's header file.
   - The relevant header file can be found in the directory:
     `example/peripheral/{mebd|raw}/{peripheral_name}/{example_folder_name}/`
   Both master's and slave's macros use common SPI pin names (SPI_MOSI, SPI_MISO, SPI_SCLK, SPI_CS) since both sides utilize SPI0.

   Two boards are required for this test. Connect the SPI0 pins of the master board to the SPI0 pins of the slave board:

- On RTL8730E, connect as below
  - Connect `SPI_MOSI (_PA_13)` [Master] to `SPI_MOSI (_PA_13)` [Slave]
  - Connect `SPI_MISO (_PA_14)` [Master] to `SPI_MISO (_PA_14)` [Slave]
  - Connect `SPI_SCLK (_PA_15)` [Master] to `SPI_SCLK (_PA_15)` [Slave]
  - Connect `SPI_CS   (_PA_16)` [Master] to `SPI_CS   (_PA_16)` [Slave]
- On RTL8726E/RTL8720E/RTL8713E/RTL8710E, connect as below
  - Connect `SPI_MOSI (_PA_29)` [Master] to `SPI_MOSI (_PA_29)` [Slave]
  - Connect `SPI_MISO (_PA_30)` [Master] to `SPI_MISO (_PA_30)` [Slave]
  - Connect `SPI_SCLK (_PA_28)` [Master] to `SPI_SCLK (_PA_28)` [Slave]
  - Connect `SPI_CS   (_PA_31)` [Master] to `SPI_CS   (_PA_31)` [Slave]
- On RTL8721Dx, connect as below
  - Connect `SPI_MOSI (_PB_24)` [Master] to `SPI_MOSI (_PB_24)` [Slave]
  - Connect `SPI_MISO (_PB_25)` [Master] to `SPI_MISO (_PB_25)` [Slave]
  - Connect `SPI_SCLK (_PB_23)` [Master] to `SPI_SCLK (_PB_23)` [Slave]
  - Connect `SPI_CS   (_PB_26)` [Master] to `SPI_CS   (_PB_26)` [Slave]
- On RTL8721F, connect as below
  - Connect `SPI_MOSI (_PA_30)` [Master] to `SPI_MOSI (_PA_30)` [Slave]
  - Connect `SPI_MISO (_PA_31)` [Master] to `SPI_MISO (_PA_31)` [Slave]
  - Connect `SPI_SCLK (_PA_29)` [Master] to `SPI_SCLK (_PA_29)` [Slave]
  - Connect `SPI_CS   (_PB_0)`  [Master] to `SPI_CS   (_PB_0)`  [Slave]
- On RTL8720F, connect as below
  - Connect `SPI_MOSI (_PA_8)`  [Master] to `SPI_MOSI (_PA_8)`  [Slave]
  - Connect `SPI_MISO (_PA_9)`  [Master] to `SPI_MISO (_PA_9)`  [Slave]
  - Connect `SPI_SCLK (_PA_7)`  [Master] to `SPI_SCLK (_PA_7)`  [Slave]
  - Connect `SPI_CS   (_PA_10)` [Master] to `SPI_CS   (_PA_10)` [Slave]

1. If you have SPI slave device, choose your own slave's pins to connect instead of slave's pins mentioned in step1, then ignore step3 and goto step4 of HW Configuration. Otherwise goto step3.
2. We offer another example named "`raw_spi_dma_sblk_rx_slv`", it can be used as SPI slave device to communicate data with this example.
Refer the README of example `raw_spi_dma_sblk_rx_slv` to build slave image and download into another EVB board.
1. Reset Slave device first and then Master device.

# SW Configuration

- Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
- By default, this example select one pinmux group as MOSI/MISO/SCLK/CS, you can also modify pinmux group settings according to correct pinmux table.
- For example, if you want to modify pinmux group setting in RTL8730E, then you should refer to `UM0602_RTL8730E_pinmux.xls`.

# Expected Result

1. After example finishes, SPI master will print out "SPI tx Demo finished".
2. If SPI master tx fail, it will notify "SPI Timeout".

# Note

1. Data array send times can be confiured by Macro definition.
2. Under DMA mode, both CPU and DMA have access to SPI buffer.
3. Start address of SPI buffer should be cacheline size aligned and size of SPI buffer should be multiple of 32 Bytes.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
RTL8720F
