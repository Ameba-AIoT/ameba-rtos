# Example Description

This example describes how to use UART TX to simulate clock source by DMA.

# HW Configuration

Required Components: Oscilloscope.

* On RTL8730E
  - Connect `UART_TX(_PA_3)` to oscilloscope
  - Connect `GND` to `GND` of oscilloscope
* On RTL8726E/RTL8720E/RTL8713E/RTL8710E
  - Connect `UART_TX(_PA_28)` to oscilloscope
  - Connect `GND` to `GND` of oscilloscope
* On RTL8721Dx
  - Connect `UART_TX(_PA_26)` to oscilloscope
  - Connect `GND` to `GND` of oscilloscope
* On RTL8721F
  - Connect `UART_TX(_PA_18)` to oscilloscope
  - Connect `GND` to `GND` of oscilloscope

# SW Configuration

By default, `UART0` is used to generate clock source signal whose frequency is `40KHz`.
1. `UART_DEV` and `OUT_CLK_HZ` can be modified to configure desired UART device and clock frequency.
2. Accordingly, `UART_TX` and `UART_RX` in example_uart_ext.h should be updated if `UART_DEV` is modified.
   For more info of UART pins, refer to pinmux spec.
3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. Open oscilloscope and simulated clock source signal can be detected on UART0 TX pin.
2. Clock signal will be on for `2000ms` and be off for `3000ms` periodically.

#  Note

None

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
