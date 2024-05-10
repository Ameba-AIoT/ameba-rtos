# Example Description

This example describes how to UART auto-flow-control function.

This example does not support the loguart command response.

# HW Configuration

Required Components: another peer device.

* On AmebaSmart
	- Connect `UART_TX(_PA_3)` to `UART_RX(_PA_2)` of peer device
	- Connect `UART_RX(_PA_2)` to `UART_TX(_PA_3)` of peer device
	- Connect `UART_RTS(_PA_5)` to `UART_CTS(_PA_4)` of peer device
	- Connect `UART_CTS(_PA_4)` to `UART_RTS(_PA_5)` of peer device
	- Connect `GND` to `GND` of peer device
* On AmebaLite
	- Connect `UART_TX(_PA_28)` to `UART_RX(_PA_29)` of peer device
	- Connect `UART_RX(_PA_29)` to `UART_TX(_PA_28)` of peer device
	- Connect `UART_RTS(_PA_30)` to `UART_CTS(_PA_31)` of peer device
	- Connect `UART_CTS(_PA_31)` to `UART_RTS(_PA_30)` of peer device
	- Connect `GND` to `GND` of peer device
* On AmebaDplus
	- Connect `UART_TX(_PB_17)` to `UART_RX(_PB_18)` of peer device
	- Connect `UART_RX(_PB_18)` to `UART_TX(_PB_17)` of peer device
	- Connect `UART_RTS(_PB_19)` to `UART_CTS(_PB_20)` of peer device
	- Connect `UART_CTS(_PB_20)` to `UART_RTS(_PB_19)` of peer device
	- Connect `GND` to `GND` of peer device

# SW Configuration

By default, `UART0` is used to transmit or receive `1000` data to or from another device and `RxFifoTrigLevel` is `1Byte`.

1. `UART_DEV` and `UART_BAUD` can be modified to configure desired UART device and baudrate.
2. Accordingly, `UART_TX`/`UART_RX`/`UART_RTS`/`UART_CTS` in uart_ext.h should be updated if `UART_DEV` is modified.
   For more info of UART pins, refer to pinmux spec.
3. `UART_BUF_SIZE` can be modified to set desired data number.
4. `RxFifoTrigLevel` can be modified to configure rx fifo trigger level, refer to `UART_Rx_FIFO_Trigger_Level` for more options.
5. How to use:
    * Copy `main.c` and `uart_ext.h` to path`project\realtek_xx_va0_example\src\src_yy\`, and replace old main.c.
    * Run `make all` and `download` image to board.

# Expected Result

1. The first powered board will be `the TX side`, and the other one will be `the RX side`.
2. The RX side will make some delay every 16-byte data received, by this way the flow control mechanism is triggered.
3. After transmit-receive is completed, responding end log will be printed out.

# Note

* On AmebaSmart
	- Only `UART0/1/2` are designed with RTS/CTS pins.
* On AmebaLite
	- Only `UART0/3` are designed with RTS/CTS pins.
* On AmebaDplus
	- Only `UART0/2` are designed with RTS/CTS pins.

# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus