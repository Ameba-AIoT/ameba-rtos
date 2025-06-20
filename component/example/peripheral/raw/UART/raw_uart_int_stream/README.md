# Example Description

This example describes how to use UART to communicate with PC by interrupt.

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

# SW Configuration

By default, `UART0` is used to communicate with PC under `38400bps` by interrupt.

1. `UART_DEV` and `UART_BAUD` can be modified to configure desired UART device and baudrate.
2. Accordingly, `UART_TX` and `UART_RX` in example_uart_ext.h should be updated if `UART_DEV` is modified.
   For more info of UART pins, refer to pinmux spec.
3. Default interrupt configuration:
	- RxFifoTrigLevel: `1BYTES`
	- RxTimeOutCnt: `64`
	- `RUART_BIT_ERBI` | `RUART_BIT_ELSI` | `RUART_BIT_ETOI` enabled
4. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. Open super terminal or Ameba trace tool and set baudrate to `38400bps`, `1 stopbit`, `no parity`, `no flow control`.
2. `8 characters` from terminal input will be received by demo board in UART ISR, and `8 received characters` will be transmitted to PC subsequently.
	- If RX_FIFO_entry > RxFifoTrigLevel, `RUART_BIT_RXFIFO_INT` interrrupt is triggered.
	- If RX FIFO is not empty and UART0 receives none over RxTimeOutCnt time, `RUART_BIT_TIMEOUT_INT` interrupt is triggered.
3. Characters sent from PC will be displayed on the terminal.

# Note

End characters CRLF(CR: "0x0D" and LF: "0x0A") will be appended to input character(s) of terminal by default.

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus
