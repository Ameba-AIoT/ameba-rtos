# Example Description

This example describes how to use UART to communicate with PC by DMA.

This example does not support the loguart command response.

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

By default, `UART0` is used to communicate with PC under `38400bps` by DMA.

1. `UART_IDX` and `UART_BAUD` can be modified to configure desired UART device and baudrate.
2. Accordingly, `UART_TX` and `UART_RX` in uart_ext.h should be updated if `UART_IDX` is modified.
   For more info of UART pins, refer to pinmux spec.
3. How to use:
    * Copy `main.c` and `uart_ext.h` to path`project\realtek_xx_va0_example\src\src_yy\`, and replace old main.c.
    * Run `make all` and `download` image to board.

# Expected Result

1. Open super terminal or Ameba trace tool and set baudrate to `38400bps`, `1 stopbit`, `no parity`, `no flow control`.
2. When log `"Ready to receive N-byte-data..."` is printed, it reminds you to enter `N characters` to terminal.
3. Characters from terminal input, number of which varies from `2` to `33`, will be received and transferred to predefined buffer by DMA. Received characters will be transmitted to PC by DMA subsequently. UART will wait until characters of specified number are received.
4. Characters sent from PC will be displayed on the terminal.

# Note

1. When D-Cache is enabled, DMA buffer size must be a multiple of 32 bytes, and buffer address must be 32-byte aligned.
2. End characters CRLF(CR: "0x0D" and LF: "0x0A") will be appended to input character(s) of terminal by default.
3. If too few or too many charaters are entered, display of characters on the terminal may be completed after several tries.

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus