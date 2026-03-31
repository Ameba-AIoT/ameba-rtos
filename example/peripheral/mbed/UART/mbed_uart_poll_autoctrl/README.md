# Example Description

This example describes how to UART auto-flow-control function.

# HW Configuration

Required Components: another peer device.

* On RTL8730E
	- Connect `UART_TX(_PA_3)` to `UART_RX(_PA_2)` of peer device
	- Connect `UART_RX(_PA_2)` to `UART_TX(_PA_3)` of peer device
	- Connect `UART_RTS(_PA_5)` to `UART_CTS(_PA_4)` of peer device
	- Connect `UART_CTS(_PA_4)` to `UART_RTS(_PA_5)` of peer device
	- Connect `GND` to `GND` of peer device
* On RTL8726E/RTL8720E/RTL8713E/RTL8710E
	- Connect `UART_TX(_PA_28)` to `UART_RX(_PA_29)` of peer device
	- Connect `UART_RX(_PA_29)` to `UART_TX(_PA_28)` of peer device
	- Connect `UART_RTS(_PA_30)` to `UART_CTS(_PA_31)` of peer device
	- Connect `UART_CTS(_PA_31)` to `UART_RTS(_PA_30)` of peer device
	- Connect `GND` to `GND` of peer device
* On RTL8721Dx
	- Connect `UART_TX(_PB_17)` to `UART_RX(_PB_18)` of peer device
	- Connect `UART_RX(_PB_18)` to `UART_TX(_PB_17)` of peer device
	- Connect `UART_RTS(_PB_19)` to `UART_CTS(_PB_20)` of peer device
	- Connect `UART_CTS(_PB_20)` to `UART_RTS(_PB_19)` of peer device
	- Connect `GND` to `GND` of peer device
* On RTL8721F
	- Connect `UART_TX(_PA_18)` to `UART_RX(_PA_19)` of peer device
	- Connect `UART_RX(_PA_19)` to `UART_TX(_PA_18)` of peer device
	- Connect `UART_RTS(_PA_25)` to `UART_CTS(_PA_26)` of peer device
	- Connect `UART_CTS(_PA_26)` to `UART_RTS(_PA_25)` of peer device
	- Connect `GND` to `GND` of peer device

# SW Configuration

By default, `UART0` is used to transmit or receive `1000` data to or from another device and `RxFifoTrigLevel` is `1Byte`.

1. `UART_IDX` and `UART_BAUD` can be modified to configure desired UART device and baudrate.
2. Accordingly, `UART_TX`/`UART_RX`/`UART_RTS`/`UART_CTS` in example_uart_ext.h should be updated if `UART_IDX` is modified.
   For more info of UART pins, refer to pinmux spec.
3. `UART_BUF_SIZE` can be modified to set desired data number.
4. `FifoLv1Byte` can be modified to `0 ~ 3`, which represents different rx fifo trigger level, refer to function serial_rx_fifo_level() for more info.
5. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. The first powered board will be `the TX side`, and the other one will be `the RX side`.
2. The RX side will make some delay every 16-byte data received, by this way the flow control mechanism is triggered.
3. After transmit-receive is completed, responding end log will be printed out.

# Note

* On RTL8730E
	- Only `UART0/1/2` are designed with RTS/CTS pins.
* On RTL8726E/RTL8720E/RTL8713E/RTL8710E
	- Only `UART0/3` are designed with RTS/CTS pins.
* On RTL8721Dx
	- Only `UART0/2` are designed with RTS/CTS pins.
* On RTL8721F
	- Only `UART0/3` are designed with RTS/CTS pins.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
