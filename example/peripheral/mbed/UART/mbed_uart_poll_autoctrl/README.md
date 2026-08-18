> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

UART auto-flow-control (RTS/CTS) demo with **polling RX** and **dual tasks** (TX pri 2 / RX pri 1).

Both boards run **identical firmware**. Each board simultaneously sends and receives 1000 bytes over UART at 38400 baud. Hardware flow control (RTS/CTS) protects against FIFO overflow when the lower-priority RX task is starved by the higher-priority TX task.

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
* On RTL8720F
	- Connect `UART_TX(_PA_21)` to `UART_RX(_PA_22)` of peer device
	- Connect `UART_RX(_PA_22)` to `UART_TX(_PA_21)` of peer device
	- Connect `UART_RTS(_PA_23)` to `UART_CTS(_PA_24)` of peer device
	- Connect `UART_CTS(_PA_24)` to `UART_RTS(_PA_23)` of peer device
	- Connect `GND` to `GND` of peer device

# SW Configuration

By default, `UART0` is used to transmit or receive `1000` data to or from another device and `RxFifoTrigLevel` is `1Byte`.

1. `UART_IDX` and `UART_BAUD` can be modified to configure desired UART device and baudrate.
2. Accordingly, `UART_TX`/`UART_RX`/`UART_RTS`/`UART_CTS` in example_uart_ext.h should be updated if `UART_IDX` is modified.
   For more info of UART pins, refer to pinmux spec.
3. `UART_BUF_SIZE` can be modified to set desired data number.
4. `FifoLv1Byte` can be modified to `0 ~ 3`, which represents different rx fifo trigger level, refer to function serial_rx_fifo_level() for more info.
5. Build and Download: both boards use the same firmware
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. Each board creates TX task (pri 2) and RX task (pri 1).
2. After 0xFF sync handshake, TX sends 1000 bytes with a busy-wait every 256 bytes to starve RX.
3. RX polls incoming data using `serial_readable()`. During TX busy-wait, RX is starved and the FIFO fills up → hardware deasserts RTS → peer pauses.
4. When TX yields (`rtos_time_delay_ms`), RX drains the FIFO, flow control recovers.
5. Both boards print `UART TX task done` and `UART RX done: all 1000 bytes match` on completion.

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
RTL8720F
