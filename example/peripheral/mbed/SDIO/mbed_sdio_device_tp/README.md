> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

This example demonstrates bidirectional SDIO throughput testing using the SDIO Device (slave) role.

- **Phase 0 (Handshake):** Waits for a `"Hello Device"` greeting from the SDIO Host board. Upon receiving it, replies with `"Hello Host! I am Here"` to confirm the link.
- **Phase 1 (TP Test):** Continuously sends 1000-byte packets to the host while receiving and verifying packets from the host. Each packet carries a CRC16-CCITT checksum. TX/RX statistics are printed every 1000 received packets.

This example must be used together with the `raw_sdio_host_tp` example running on a second board acting as the SDIO Host.

# HW Configuration

Required Components: Two RTL8721F boards.

Connect the SDIO signals between the Host board and the Device board as follows:

| Signal | Host board (PA) | Device board (PA) |
|--------|-----------------|-------------------|
| CLK    | PA18            | PA18              |
| CMD    | PA19            | PA19              |
| D0     | PA28            | PA28              |
| D1     | PA20            | PA20              |
| D2     | PA27            | PA27              |
| D3     | PA21            | PA21              |
| GND    | GND             | GND               |

> The table above applies to the default pin group (Group 2). The group is selected by `SDIO_Pin_Grp` in `component/soc/usrcfg/amebagreen2/ameba_intfcfg.c`.

# SW Configuration

Test parameters are defined as macros at the top of `example_mbed_sdio_device_tp.c`:

| Macro | Default | Description |
|-------|---------|-------------|
| `TP_PAYLOAD_SIZE` | 1000 | Payload bytes per TP packet |
| `TP_SEND_INTERVAL_MS` | 1 | TX interval (ms) |
| `TP_STATS_INTERVAL` | 1000 | Print stats every N received packets |
| `EX_SPDIO_TX_BD_NUM` | 8 | TX buffer descriptor count |
| `EX_SPDIO_RX_BD_NUM` | 4 | RX buffer descriptor count |

To change the SDIO pin group, modify `SDIO_Pin_Grp` in `component/soc/usrcfg/amebagreen2/ameba_intfcfg.c`. For available pin groups, refer to the `SDIO_PAD` table in the same file.

Build and Download:
1. Refer to the SDK Examples section of the online documentation to generate images.
2. Download images to the board by Ameba Image Tool.

# Expected Result

Power on the Device board **before** the Host board, or reset both boards at roughly the same time.

1. After boot, the Device initializes and waits for the Host:
   ```
   [SDIO_DEV-I] SDIO Device initialized
   [SDIO_DEV-I] TX_BD=8  RX_BD=4  RX_bufsz=2112
   [SDIO_DEV-I] Phase 0: Waiting for host greeting...
   ```
2. When the Host sends its greeting, the Device completes the handshake:
   ```
   [SDIO_DEV-I] [Phase 0] Received: "Hello Device"
   [SDIO_DEV-I] [Phase 0] Sending:  "Hello Host! I am Here"
   ```
3. Phase 1 TP test starts and statistics are printed every 1000 received packets:
   ```
   [SDIO_DEV-I] [Stats @ RX=1000] TX sent:180 | RX total:1000 ok:1000 err:0 (0%)
   [SDIO_DEV-I] [Stats @ RX=2000] TX sent:260 | RX total:2000 ok:2000 err:0 (0%)
   ```
   A sustained `err:0 (0%)` rate indicates a healthy SDIO link.

# Note

1. The SDIO Device peripheral clock and pinmux are configured automatically by `spdio_init()`. No manual pin configuration is required in the example code.
2. Device TX throughput is significantly lower than Host TX throughput. This asymmetry is expected: the Host drives CMD53 write bursts autonomously, while the Device TX path is gated by available TX buffer descriptors (`EX_SPDIO_TX_BD_NUM`).
3. Each TX buffer is dynamically allocated before sending and freed in the TX-done callback. Reducing `EX_SPDIO_TX_BD_NUM` lowers peak heap usage at the cost of TX concurrency.
4. RX buffer descriptors are reallocated in the RX-done callback. If reallocation fails, the affected BD becomes unusable for that session; a warning is logged.

# Supported IC

RTL8721F
