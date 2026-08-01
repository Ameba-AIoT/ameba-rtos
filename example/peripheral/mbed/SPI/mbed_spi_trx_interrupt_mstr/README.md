> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

1. This example is the **master** role of the mbed SPI interrupt dual-board test.
2. It uses SPI0 as master with interrupt-driven data transfer.
3. It must work together with `mbed_spi_trx_interrupt_slv` on another board.

# HW Configuration

1. Both boards use the **same SPI0 pins**, connected pin-to-pin:

   - `Master's SPI0_MOSI` to `Slave's SPI0_MOSI`
   - `Master's SPI0_MISO` to `Slave's SPI0_MISO`
   - `Master's SPI0_SCLK` to `Slave's SPI0_SCLK`
   - `Master's SPI0_CS` to `Slave's SPI0_CS`
   - GND to GND

   Refer to `example_spi_ext.h` for the pin definitions of each SoC.

2. Build and flash `mbed_spi_trx_interrupt_slv` onto the slave board first.
3. Reset the slave board first, then the master board.

# SW Configuration

- Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. Master performs Phase1 (full-duplex) and Phase2 (master read).
2. After each phase the result is printed as "success" or "fail".
3. When the test completes, "SPI Demo: success" or "SPI Demo: fail" is printed.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
RTL8720F
