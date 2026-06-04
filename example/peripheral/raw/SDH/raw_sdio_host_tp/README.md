# Example Description

This example demonstrates bidirectional SDIO throughput testing using the SDIO Host (master) role.

- **Phase 0 (Handshake):** Periodically sends `"Hello Device"` to the SDIO Device board (up to 30 attempts, 1-second interval). Waits for the `"Hello Host! I am Here"` response before proceeding.
- **Phase 1 (TP Test):** Continuously sends 1000-byte packets to the device (every 1 ms) while receiving and verifying packets from the device. Each packet carries a CRC16-CCITT checksum. TX/RX statistics are printed every 1000 received packets.

This example must be used together with the `mbed_sdio_device_tp` example running on a second board acting as the SDIO Device.

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

> The table above applies to the default pin group (Group 2). The group is selected by `SDH_Pin_Grp` in `component/soc/usrcfg/amebagreen2/ameba_intfcfg.c`.

# SW Configuration

Test parameters are defined as macros at the top of `example_raw_sdio_host_tp.c`:

| Macro | Default | Description |
|-------|---------|-------------|
| `TP_PAYLOAD_SIZE` | 1000 | Payload bytes per TP packet |
| `TP_SEND_INTERVAL_MS` | 1 | TX interval (ms) |
| `TP_STATS_INTERVAL` | 1000 | Print stats every N received packets |
| `HELLO_MAX_RETRIES` | 30 | Maximum Phase 0 greeting attempts |
| `HELLO_SEND_INTERVAL_MS` | 1000 | Interval between greeting attempts (ms) |
| `SDIO_POLLING_INTERVAL_MS` | 50 | HISR polling interval for Device→Host data detection (ms) |

To change the SDIO pin group, modify `SDH_Pin_Grp` in `component/soc/usrcfg/amebagreen2/ameba_intfcfg.c`. For available pin groups, refer to the `SDIO_PAD` table in the same file.

Build and Download:
1. Refer to the SDK Examples section of the online documentation to generate images.
2. Download images to the board by Ameba Image Tool.

# Expected Result

Power on the Device board **before** the Host board, or reset both boards at roughly the same time.

1. After boot, the Host initializes the SD controller and starts Phase 0:
   ```
   [SDIO_HOST-I] SDIO host init OK
   [SDIO_HOST-I] Phase 0: Sending greeting to device...
   [SDIO_HOST-I] [Phase 0] Sent "Hello Device" (attempt 1/30)
   ```
2. Once the Device responds, the handshake completes:
   ```
   [SDIO_HOST-I] [Phase 0] Received: "Hello Host! I am Here" (40 bytes total)
   ```
3. Phase 1 TP test starts and statistics are printed every 1000 received packets:
   ```
   [SDIO_HOST-I] [Stats @ RX=1000] TX sent:12216 fail:0 | RX total:1000 ok:1000 err:0 (0%)
   [SDIO_HOST-I] [Stats @ RX=2000] TX sent:24467 fail:0 | RX total:2000 ok:2000 err:0 (0%)
   ```
   A sustained `err:0 (0%)` and `fail:0` count indicates a healthy SDIO link. The Host TX count is much higher than the Device TX count — this throughput asymmetry is expected behavior (see Note).

# Note

1. Device→Host data is detected by polling the HISR register every `SDIO_POLLING_INTERVAL_MS` milliseconds, rather than using the `SD_IRQ_NOTIFY` interrupt callback. This avoids a symbol conflict with the WiFi driver, which also registers `SD_IRQ_NOTIFY` via `whc_sdio_init.c`.
2. Card Detect (CD) pin checking is disabled (`sdioh_cd_pin = _PNC`) because the SDIO Device does not assert a CD signal.
3. Each CMD53 write buffer prepends a 16-byte `INIC_TX_DESC` header. This header is required by the Device-side SDIO IP to locate the payload offset and length within the DMA buffer.
4. Host TX throughput is significantly higher than Device TX throughput. The Host issues CMD53 writes autonomously, while the Device TX rate is limited by its buffer descriptor pool and 1 ms TX interval.
5. If Phase 0 fails after `HELLO_MAX_RETRIES` attempts, the test aborts with an error log. Verify that the Device board is running and all SDIO wires are correctly connected before retrying.

# Supported IC

RTL8721F
