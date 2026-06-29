# Example Description

In this example, AT command control via UART is demonstrated. The host uses
two UART peripherals: one for the user's terminal (HOST_UART) and one for
the link to the AT command slave device (AT_UART). Input typed on HOST_UART
is forwarded to the slave over AT_UART, and slave responses are displayed on
HOST_UART.

```
PC terminal <--115200--> HOST_UART | Host board | AT_UART <--38400--> Slave (AT cmd service)
```

No custom framing is needed: the UART link carries raw bytes and the slave
parses commands terminated by `\r\n`, identical to a direct terminal
connection.

# HW Configuration

Connect host and slave boards with three wires:

| Host pin    | Slave pin   | Signal      |
|-------------|-------------|-------------|
| AT_UART_TX  | UART_RX     | host → slave |
| AT_UART_RX  | UART_TX     | slave → host |
| GND         | GND         | common ground |

Default pin assignments per SoC (see `example_atcmd_host_uart.h`):

| SoC         | HOST_UART (terminal) | AT_UART (slave link) |
|-------------|----------------------|----------------------|
| AmebaSmart  | PA_25 / PA_26        | PA_3 / PA_2          |
| AmebaLite   | PA_28 / PA_29        | PA_22 / PA_21        |
| AmebaDplus  | PA_26 / PA_27        | PB_18 / PB_19        |

The slave-side UART pins match the defaults in `component/at_cmd/at_intf_uart.c`.

# SW Configuration

1. **Slave (AT command device)**
   a. Enable the UART AT interface in `atcmd_config.json`; set `UART_BAUD`
      to match `AT_UART_BAUDRATE` in the host header (default 38400).
   b. Build and flash the slave firmware.

2. **Host**
   a. Set ATCMD Mode in menuconfig to `Manual Test Mode`.
   b. Adjust `HOST_UART_IDX`, `AT_UART_IDX`, and pin macros in
      `example_atcmd_host_uart.h` if the defaults do not match your board.
   c. Set `AT_UART_BAUDRATE` to match the slave's `UART_BAUD`.
   d. Build and flash the host firmware.

# TT Mode

When `TT_MODE_TEST` is enabled (default: 1) the host starts a throughput
sender task (`tt_mode_test_task`) after the slave sends the `>>>\r\n`
trigger. The task sends `TT_TEST_DATA_LEN` bytes (default 2 MB) at the
AT_UART baud rate and responds to inline High/Low Watermark notifications
from the slave for flow control.

Note: at 38400 baud (≈ 3840 B/s), 2 MB takes approximately 9 minutes.
Increase `AT_UART_BAUDRATE` and the slave's `UART_BAUD` for faster testing.

# Expected Result

After boot, HOST_UART prints `UART Host AT Command Demo...`. Type any AT
command (e.g. `AT\r\n`) and the slave response appears on HOST_UART.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
