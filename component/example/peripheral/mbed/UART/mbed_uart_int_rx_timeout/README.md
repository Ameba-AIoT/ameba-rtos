# Example Description

This example describes how to use UART to communicate with PC by interrupt before timeout.

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

By default, `UART0` is used to communicate with PC under `38400bps` by interrupt every `1000ms`.

1. `UART_IDX` and `UART_BAUD` can be modified to configure desired UART device and baudrate.
2. Accordingly, `UART_TX` and `UART_RX` in example_uart_ext.h should be updated if `UART_IDX` is modified.
   For more info of UART pins, refer to pinmux spec.
3. `RX_DATA_INTIME` can be modified to set desired data number to receive before timeout.
4. `RX_TO_MS` can be modified to set rx period in ms.
5. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. Open super terminal or Ameba trace tool and set baudrate to `38400bps`, `1 stopbit`, `no parity`, `no flow control`.
2. Characters from terminal input will be received in `rx_data_ready` ISR. If accumulated received data number reaches `13` before timeout, received characters will be transmitted to PC by interrupt. However, if not enough data is transmitted to demo board, corresponding log will be printed out and no characters will be looped back to PC.
3. Characters sent from PC will be displayed on the terminal.

# Note

1. End characters CRLF(CR: "0x0D" and LF: "0x0A") will be appended to input character(s) of terminal by default.
2. `TIM0` is enabled to count timeout.

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus
* AmebaGreen2
