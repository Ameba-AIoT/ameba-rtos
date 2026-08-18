> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

1. This example is the **slave** role of the SPI polling dual-board test.
2. It uses SPI0 as slave to perform full-duplex and unidirectional data transfer.
3. It must work together with `raw_spi_trx_polling_mstr` on another board.

# HW Configuration

1. Connect two boards using the same SPI0 pins (pin-to-pin):

   - `Slave's SPI_MOSI` to `Master's SPI_MOSI`
   - `Slave's SPI_MISO` to `Master's SPI_MISO`
   - `Slave's SPI_SCLK` to `Master's SPI_SCLK`
   - `Slave's SPI_CS` to `Master's SPI_CS`
   - GND to GND

   Refer to `example_spi_ext.h` for the pin definitions of each SoC.

2. This example is the slave-side; the master-side is `raw_spi_trx_polling_mstr`.

# SW Configuration

- Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. Slave performs Phase1 (full-duplex) and Phase2 (slave read).
2. After each phase the result is printed as "success" or "fail".
3. When the test completes, "SPI Demo: success" or "SPI Demo: fail" is printed.

# Note

1. This is a dual-board test; both boards must be properly connected and powered.
2. The slave board should be reset before the master board.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
RTL8720F
