> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

1. This example is the **master** role of the SPI DMA dual-board test.
2. It uses SPI0 as master with GDMA-driven data transfer.
3. It must work together with `raw_spi_trx_dma_slv` on another board.

# HW Configuration

1. Connect two boards using the same SPI0 pins (pin-to-pin):

   - `Master's SPI_MOSI` to `Slave's SPI_MOSI`
   - `Master's SPI_MISO` to `Slave's SPI_MISO`
   - `Master's SPI_SCLK` to `Slave's SPI_SCLK`
   - `Master's SPI_CS` to `Slave's SPI_CS`
   - GND to GND

   Refer to `example_spi_ext.h` for the pin definitions of each SoC.

2. Build and flash `raw_spi_trx_dma_slv` onto the slave board first.
3. Reset the slave board first, then the master board.

# SW Configuration

- Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. Master performs Phase1 (full-duplex) and Phase2 (master read) via GDMA.
2. After each phase the result is printed as "success" or "fail".
3. When the test completes, "SPI Demo: success" or "SPI Demo: fail" is printed.

# Note

1. Under DMA mode, both CPU and DMA have access to SPI buffer.
2. Start address of SPI buffer should be cacheline size aligned and size of SPI buffer should be multiple of 32 Bytes.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
RTL8720F
