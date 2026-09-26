> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../../peripheral/README.md) if available.

# Example Description

This example demonstrates AT command host control over SDIO on RTL8721F acting as the SDIO host (master). UART input on the host board is forwarded to the AT command device over SDIO, and the device's response is returned to the UART terminal.

Additional features:

- **Downstream throughput test**: detects `Downstream Test Start/End` markers from the device and prints throughput statistics to the host UART.
- **Transparent transmission (TT) mode**: set the transfer length via `AT+TEST=1,<N>\r\n` before the command that triggers TT. After the device responds with `>>>`, the host automatically sends `N` bytes of pattern data and respects `[$][TT]:High/Low Watermark` flow-control messages.

This example must be used together with an Ameba SDIO device board running `sdio` in `ATCMD_HOST_CONTROL` mode.

# HW Configuration

Required: two RTL8721F boards (one as SDIO host running this example, one as SDIO device).

Connect the SDIO signals between the two boards as follows:

| Signal | Host board (SDH) | Device board (SPDIO) |
|--------|-----------------|----------------------|
| CLK    | PA18            | PA18                 |
| CMD    | PA19            | PA19                 |
| D0     | PA28            | PA28                 |
| D1     | PA20            | PA20                 |
| D2     | PA27            | PA27                 |
| D3     | PA21            | PA21                 |
| GND    | GND             | GND                  |

> The table above applies to the default pin group (Group 2).
> The host pin group is selected by `SDH_Pin_Grp` in `component/soc/usrcfg/amebagreen2/ameba_intfcfg.c`.
> The device pin group is selected by `SDIO_Pin_Grp` in the same file on the device board.
> For available pin groups, refer to the `SDIO_PAD` table in that file.

Also connect a UART terminal to the **host board** for sending AT commands and viewing responses:

| Signal | Host board |
|--------|-----------|
| TX     | PA14       |
| RX     | PA15       |
| GND    | GND       |

UART configuration: 115200 baud, 8N1.

To change the UART pins or baud rate, modify `HOST_UART_TX`, `HOST_UART_RX`, and `HOST_UART_BAUDRATE` at the top of `example_atcmd_host_sdio.c`.

# SW Configuration

## For the atcmd device board

1. In menuconfig, enable:
   - `CONFIG_SHELL` → `CONFIG_SUPPORT_ATCMD` → `ATCMD_HOST_CONTROL`
   - `CONFIG_SUPPORT_SDIO_DEVICE`

2. In `atcmd_config.json` (loaded from VFS1), set the AT interface to SDIO. Refer to the AT command documentation for the exact field name.

3. Build and download to the device board.

## For the atcmd host board (this example)

1. Set `SDH_Pin_Grp = 0x2` in `component/soc/usrcfg/amebagreen2/ameba_intfcfg.c` (default).

2. Build:
   ```
   ./ameba.py build -a atcmd_host_sdio
   ```
   Or with MCP: `build_firmware(app="atcmd_host_sdio")`.

3. Download images to the host board.

# Expected Result

Power on the device board **before** the host board, or reset both at roughly the same time.

After the host board boots:

```
AT Command Host (SDIO) ready
SDIO init OK
```

You can then send AT commands through the UART terminal and view the responses. Example:

```
AT+TEST\r\n
-> \r\nOK\r\n
```

# Note

1. The Card Detect (CD) pin check is disabled (`sdioh_cd_pin = _PNC`) because the SDIO device does not assert a CD signal.
2. Device→Host data is detected via hardware CARD INT (`SD_IRQ_Notify` weak override), which disables `CARD_INT_STATUS_EN` in the ISR and re-enables it after the RX FIFO is drained. This avoids interrupt storms while DAT1 remains asserted.
3. Each CMD53 write buffer prepends a 16-byte `INIC_TX_DESC` header required by the device-side SDIO IP to locate the payload offset and length.
4. For TT mode: send `AT+TEST=1,<N>\r\n` first to set the byte count, then issue the AT command that triggers TT (e.g., a socket send command). The host starts bulk TX automatically after receiving `>>>` from the device.

# Supported IC

RTL8721F
