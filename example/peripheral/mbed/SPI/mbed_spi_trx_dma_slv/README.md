> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

1. This example is the **slave** role of the mbed SPI DMA dual-board test.
2. It uses SPI0 as slave with DMA-driven data transfer.
3. It must work together with `mbed_spi_trx_dma_mstr` on another board.

# HW Configuration

1. Both boards use the **same SPI0 pins**, connected pin-to-pin:

   - `Slave's SPI0_MOSI` to `Master's SPI0_MOSI`
   - `Slave's SPI0_MISO` to `Master's SPI0_MISO`
   - `Slave's SPI0_SCLK` to `Master's SPI0_SCLK`
   - `Slave's SPI0_CS` to `Master's SPI0_CS`
   - GND to GND

   Refer to `example_spi_ext.h` for the pin definitions of each SoC.

2. This example is the slave-side; the master-side is `mbed_spi_trx_dma_mstr`.

# SW Configuration

- Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. Slave performs Phase1 (full-duplex) and Phase2 (slave write).
2. After each phase the result is printed as "success" or "fail".
3. When the test completes, "SPI Demo: success" or "SPI Demo: fail" is printed.

# Note

1. Under DMA mode, start address of SPI buffer should be cacheline size aligned and size of SPI buffer should be multiple of 32 Byte.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
RTL8720F
