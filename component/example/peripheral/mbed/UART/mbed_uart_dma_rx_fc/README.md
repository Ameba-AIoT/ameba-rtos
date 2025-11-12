# Example Description

This example describes how to use UART to communicate with PC by DMA before timeout.

# HW Configuration

Required Components: USBtoTTL adapter.

* On AmebaSmart
	- Connect `UART_TX(_PA_3)` to `RXD` of adapter
	- Connect `UART_RX(_PA_2)` to `TXD` of adapter
	- Connect `GND` to `GND` of adapter
* On AmebaLite
	- Connect `UART_TX(_PA_28)` to `RXD` of adapter
	- Connect `UART_RX(_PA_29)` to `TXD` of adapter
	- Connect `GND` to `GND` of adapter
* On AmebaDplus
	- Connect `UART_TX(_PA_26)` to `RXD` of adapter
	- Connect `UART_RX(_PA_27)` to `TXD` of adapter
	- Connect `GND` to `GND` of adapter
* On AmebaGreen2
	- Connect `UART_TX(_PA_18)` to `RXD` of adapter
	- Connect `UART_RX(_PA_19)` to `TXD` of adapter
	- Connect `GND` to `GND` of adapter

# SW Configuration

By default, `UART0` is used to communicate with PC under `38400bps` by DMA before timeout.

1. `UART_IDX` and `UART_BAUD` can be modified to configure desired UART device and baudrate.
2. Accordingly, `UART_TX` and `UART_RX` in example_uart_ext.h should be updated if `UART_IDX` is modified.
   For more info of UART pins, refer to pinmux spec.
3. `RX_TO_MS` can be modified to configure UART RX timeout limit in ms. Maximum value of RX_TO_MS is `65535/UART_BAUD`(s) for AmebaSmart/AmebaLite/AmebaDplus and `6710ms` for AmebaGreen2.
4. `SRX_BUF_SZ` can be modified to set DMA buffer size.
5. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. Open super terminal or Ameba trace tool and set baudrate to `38400bps`, `1 stopbit`, `no parity`, `no flow control`.
2. Characters from terminal input will be received and transferred to predefined buffer by DMA before timeout. Received characters will be transmitted to PC by DMA subsequently. On AmebaGreen2, `'Timeout! Got NByte(s)'` will be displayed if time expires since last data sent to UART, without filling given buffer.
3. Characters sent from PC will be displayed on the terminal.

# Note

1. When D-Cache is enabled, DMA buffer size must be a multiple of 32 bytes, and buffer address must be 32-byte aligned.
2. Size of data sent from PC should not be greater than `SRX_BUF_SZ`, otherwise overflow would happen.
3. End characters CRLF(CR: "0x0D" and LF: "0x0A") will be appended to input character(s) of terminal by default.

# Supported IC

- AmebaSmart
  * RTL8730E
- AmebaLite
  * RTL8726E
  * RTL8720E
  * RTL8713E
  * RTL8710E
- AmebaDplus
  * RTL8721Dx
- AmebaGreen2
  * RTL8721F
