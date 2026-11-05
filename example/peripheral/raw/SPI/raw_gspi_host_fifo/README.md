> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

This example is a long-run bidirectional GSPI throughput and stress test, running the **Host** (SPI Master) role.

GSPI is Realtek's proprietary protocol carried over a standard SPI bus. It shares both the pins and part of the hardware with the SDIO Device interface, so the two cannot work at the same time. The interface defaults to SDIO Device mode; it stays in SPI Slave mode only if the Host's very first bus transaction is a SPI write to the `SPI_CFG` register.

- **Link up:** Writes `SPI_CFG` to lock the Device into SPI Slave mode, reads it back to confirm, then completes a `"Hello Device"` / `"Hello Host! I am Here"` handshake over the TX/RX FIFO domain.
- **TP test:** Continuously sends TP packets to the Device while draining and verifying the packets the Device sends back. Each packet carries a CRC16-CCITT checksum. Statistics are printed at a fixed interval and the test runs indefinitely by default.

This example must be used together with the `mbed_sdio_device_tp` example running on a second board. That Device example needs **no modification and no GSPI-specific code** — the bus mode is decided entirely by this Host.

# HW Configuration

Required Components: Two RTL8721F boards.

Connect the signals between the Host board and the Device board as follows:

| GSPI signal | Host board (PA) | Device board (PA) | Device SDIO signal |
|-------------|-----------------|-------------------|--------------------|
| SPI_CLK     | PA18            | PA18              | CLK                |
| SPI_MOSI    | PA19            | PA19              | CMD                |
| SPI_MISO    | PA28            | PA28              | D0                 |
| SPI_CS      | PA21            | PA21              | D3                 |
| SPI_INT     | PA20            | PA20              | D1                 |
| GND         | GND             | GND               | —                  |

Refer to `example_gspi_fifo_ext.h` for the pin definitions.

> The table above applies to pin group 2, selected by `SDIO_Pin_Grp` in `component/soc/usrcfg/amebagreen2/ameba_intfcfg.c`. Both boards must use the same group. The Device's D2 (PA27) is unused by GSPI.

Build and flash `mbed_sdio_device_tp` onto the Device board first, then reset the Device board before the Host board.

# SW Configuration

Test parameters are defined as macros at the top of `example_raw_gspi_host_fifo.c`:

| Macro | Default | Description |
|-------|---------|-------------|
| `GSPI_BUS_FREQ_HZ` | 10000000 | Bus clock applied after link up, up to 25MHz |
| `TP_TX_PAYLOAD_SIZE` | 1000 | Host to Device payload bytes per packet; 0 makes the test RX-only |
| `TP_TX_INTERVAL_MS` | 0 | Host TX pacing (ms); 0 sends as fast as free TX BDs allow |
| `STATS_INTERVAL_MS` | 5000 | Statistics print period (ms) |
| `TEST_DURATION_MS` | 0 | Total duration (ms); 0 runs forever |
| `RX_READS_PER_ROUND` | 8 | Max RX_FIFO reads per main-loop round |
| `TP_MAX_PAYLOAD_SIZE` | 2000 | Upper bound used to size the static buffers |
| `GSPI_FREQ_ACTIVATE` | 1000000 | Bus clock used for the activation frame |
| `GSPI_USE_DMA` | 1 | Use GDMA for FIFO data phases |
| `GSPI_DMA_THRESHOLD` | 64 | Data-phase bytes from which GDMA is used |

`TP_TX_PAYLOAD_SIZE` must not exceed `TP_MAX_PAYLOAD_SIZE`, and the resulting packet must fit the Device's RX buffer (`EX_SPDIO_RX_BUFSZ` in `mbed_sdio_device_tp`, 2112 bytes by default).

Build and Download:
1. Refer to the SDK Examples section of the online documentation to generate images.
2. Download images to the board by Ameba Image Tool.

# Expected Result

1. The Host locks the bus mode and completes the handshake:
   ```
   [GSPI_HOST-I] mode-lock OK, SPI_CFG=0x01 CPU_IND=0x01 (CPU_RDY=1)
   [GSPI_HOST-I] handshake OK, Device replied "Hello Host! I am Here"
   [GSPI_HOST-I] TP test started: bus=10000000Hz tx_payload=1000B tx_interval=0ms duration=0ms
   ```
2. Statistics are printed every `STATS_INTERVAL_MS`:
   ```
   [GSPI_HOST-I] [5s] TX ok:284 nobd:0 err:0 | RX pkt ok:2272 err:0 bytes:2290176 | 447KB/s
   [GSPI_HOST-I] [10s] TX ok:567 nobd:0 err:0 | RX pkt ok:4536 err:0 bytes:4572288 | 446KB/s
   [GSPI_HOST-I] [15s] TX ok:850 nobd:0 err:0 | RX pkt ok:6800 err:0 bytes:6854400 | 446KB/s
   ```
   A sustained `err:0` on both directions indicates a healthy GSPI link.
3. The Device independently verifies the Host's packets and prints its own statistics:
   ```
   [SDIO_DEV-I] [Phase 0] RX 12 bytes, type=0x83
   [SDIO_DEV-I] [Phase 0] Received: "Hello Device"
   [SDIO_DEV-I] [Stats @ RX=1000] TX sent:7995 | RX total:1000 ok:1000 err:0 (0%)
   ```
4. If `TEST_DURATION_MS` is non-zero, the test ends with `"GSPI Demo: success"` or `"GSPI Demo: fail"`.

# Note

All bus access goes through the SoC driver `ameba_gspi.c` (`GSPI_Init`, `GSPI_Configuration`, `GSPI_ReadReg*`/`GSPI_WriteReg*`, `GSPI_WriteTxFifo`/`GSPI_ReadRxFifo`). The example only builds the TX descriptor, validates payloads and keeps statistics; frame layout, byte order, 4-byte padding, the HIMR shadow and the DMA data path live in the driver. With `GSPI_USE_DMA = 1` the FIFO data phases use GDMA: 715KB/s versus 446KB/s with per-byte polling at 10MHz, both error-free. The notes below record the protocol details that drove the driver's design.

1. The Host's very first bus transaction must be the `SPI_CFG` write, and it must be byte-swapped because the Device still interprets the bus with its default little-endian setting at that moment. Any other bus activity first — including a stray CS assertion during boot — makes the hardware fall back to SDIO Device mode. This is why CS is driven high before the boot delay, latching the high level into the output register before enabling the output driver.
2. CS is driven by GPIO rather than the SPI controller's hardware CS: one GSPI transaction must keep CS low across its whole Command/Data/Status sequence, which automatic CS cannot guarantee.
3. Transfers use `spi_master_write()` per byte rather than `spi_master_write_read_stream()`. The latter is interrupt-driven and returns before the bytes shift out, which lets CS rise mid-transfer.
4. `PINMUX_FUNCTION_SPI0`/`SPI1` (function ID 9/10) select the SoC's one fixed SPI pin group and are not valid on the SDIO-shared pins used here. `spi_init()` is therefore bootstrapped on that unused fixed group purely for its clock-enable and controller setup, and the real pins are routed afterwards with the per-signal extended function IDs (`SPI0_CLK`/`SPI0_MISO`/`SPI0_MOSI`).
5. Two descriptor details differ from UM0505 (Ameba-Z II) and follow this SoC's actual driver instead:
   - The descriptor is 16 bytes (`INIC_TX_DESC`, 4 dwords), not 24. See the comment in `ameba_inic.h`.
   - `txpktsize` carries the payload length **only**, excluding the descriptor, even though UM0505 Table 14-4 states otherwise. Including the descriptor makes the hardware compute `offset + txpktsize` as the packet total, overshoot the delivered bus length, and silently drop the packet — the Device's RX callback never runs.

   The reference for both is `example/peripheral/raw/SDH/raw_sdio_host_tp`, the SDIO path already proven on this SoC.
   A packet rejected this way comes back with `0x00000008` in the first word of the returned Status, which goes to `0x00000000` once `txpktsize` is correct. Note that the meaning of that bit is ambiguous: the reference driver's `GET_STATUS_HISR` macro treats the word as the raw `SPI_HISR` (making it bit3, `TX_AGG_SIZE_MISMATCH`), whereas UM0505 Figure 14-22 packs only `SPI_HISR[1:0]` and `[19:17]` into the Status, which would make it `CPWM1_INT` instead and would mean the TX error bits are not visible in the Status at all. Treat the value as an empirical marker for this failure, not as a confirmed bit name.
6. The Command word needs a "Fun" field set at bit29, which is absent from UM0505's Command field tables.
7. **The two directions are asymmetric about descriptors, and the RX side is an application convention rather than a protocol rule.**
   - Host to Device: the descriptor is mandatory. `SPDIO_TxBd_DataReady_DeviceRx()` parses an `INIC_TX_DESC` at the start of every packet, forwards `buf + offset` for `txpktsize` bytes to the Device's callback, and drops the packet outright ("Invalid TxDesc packet, Just drop it") if `txpktsize + offset` exceeds the Device's RX buffer.
   - Device to Host: the driver adds nothing. `SPDIO_DeviceTx()` puts the caller's buffer into the RX BD verbatim — its own comment reads "first BD of the packet carries FS=1 (no separate RX_Desc BD)" — and the `INIC_RX_DESC` array that `spdio_init()` allocates is never populated or transmitted, only zeroed.

   So whether a descriptor appears in the RX stream depends entirely on what the Device application puts in `tx_buf->buf_addr`. `mbed_sdio_device_tp` hands over raw payload, so this example reads the payload straight from offset 4 — correct for this pairing, but **not** a protocol invariant. A WHC/iNIC-style Device application prepends an `INIC_RX_DESC`, and a Host driver for it must parse one. Treat the RX payload as opaque bytes at the transport layer and let the upper layer decide.

   `RX0_REQ_LEN` is rounded up to 4 bytes, so a reported length can exceed the real message; the tail is padding.
8. The descriptor's `type` field is a pass-through tag: the Device hands it to its `rx_done_cb` without acting on it (`mbed_sdio_device_tp` marks the parameter `UNUSED` and only logs it), so any value completes this test. Use `0x41` for user data — the values come from `enum spdio_rx_data_t` in `ameba_sdio_device.h` (`NULL` 0x00, `ETH` 0x83, `ATCMD` 0x11, `USER` 0x41). Note that `raw_sdio_host_tp` defines a local macro named `SPDIO_RX_DATA_USER` with the value `0x83`, shadowing the SDK enum with the value that actually means "Ethernet packet"; do not copy it.
9. The Device's `tx_sema timeout — TX BD exhausted?` message is normal back-pressure, not a fault. `mbed_sdio_device_tp` holds a limited number of TX tokens, returned only after the Host reads the data out. It appears whenever the Host stops draining, and disappears once draining resumes.
10. Throughput is Host-bound rather than bus-bound: every packet costs one byte-by-byte polled FIFO read plus a `RX0_REQ_LEN` register poll. Raising `GSPI_BUS_FREQ_HZ` helps only up to the point where this per-byte software overhead dominates.
11. **`SPI_INT` (0x04) and `SPI_HIMR` (0x14) cannot be read on this SoC.** Writes to them do take effect; only the readback is unavailable.

    Root cause, per the SoC design team: the SPDIO registers span two power domains, and the address map was laid out so bit[7] of the offset separates them — off domain at `0x04`..`0x70` (bit7 = 0), on domain at `0x80`..`0xF0` (bit7 = 1). The hardware uses `addr[7]` directly as the domain select for the read-data mux. The two interrupt-configuration registers were later moved into the on domain but kept their original `0x04`/`0x14` addresses, and the domain-select signal was not updated. A read therefore sees `addr[7] = 0`, gets routed to the off domain, finds nothing mapped, and returns 0. Verified on target: reading `SPI_HIMR` at `0x14`, `0x94`, `0x114` and `0x8014` all return 0.

    Only these two registers are affected — both the off-domain and on-domain read/write paths are otherwise healthy. Verified by a register sweep on this hardware: writing `0x5A3C0F69` to `0x4C` reads back identically, and `0x4C` shares the same off-domain routing as `SPI_HIMR`; a byte write of `0x5A` to `0x84` likewise reads back exactly. `SPI_CFG` (0xF0) and `CPU_IND` (aligned to 0x84) read normally as on-domain registers with bit7 set. Treat `SPI_HIMR` as write-only and keep a software shadow, as `raw_sdio_host_tp` does with `g_sdio_himr`.

    Notes when interpreting a sweep:
    - Registers whose reset value is 0 cannot be judged by a read alone, which is why the write-readback step exists.
    - `0x40`, `0x44` and `0x48` only implement bits[11:0], so a 32-bit write-readback legitimately returns just the low 12 bits — a narrow field, not a read failure. UM0505 lists `0x40`/`0x44` as `31:0 R/W`, wider than the implementation.
    - `0x84` must be written byte-wise (Byte_En = 0x1). `CPU_IND` sits at `0x87` in the same 32-bit word and a word write would clobber it.
    - The sweep deliberately does not write `0x24`/`0x38`/`0x3A`/`0x50` (read-only from the Host), `0x80`/`0x8C` (writable but disruptive — HRPWM raises a Device power-state interrupt, DELY_LINE_SEL changes bus sampling timing), or `0x28`/`0x30`/`0x60`/`0x70`/`0x90`/`0x94` (not defined in UM0505 chapter 14.5, so safety is unknown).
12. RX detection polls `RX0_REQ_LEN` rather than the `SPI_INT` pin, which is configured as a GPIO input but not used. The pin does work — it is the level output of `HISR & HIMR` — but this example drains immediately, so the masked HISR bits clear within microseconds and the line is only low for that long. `raw_gspi_host_ps` demonstrates the pin as a wake source.
13. Bit0 of the Status trailer (`RX_REQUEST`) routinely appears after a TX write to signal that the Device has RX data pending. It is a normal notification, so only the TX error bits are treated as a rejection.
14. A TX_FIFO write must clock out a 4-byte-aligned number of data bytes; the length in the Command is in 4-byte units on the SDIO side (UM0505 Table 14-3). With an unaligned length the packet is still delivered, but the Device does not drive the Status phase and the trailer reads back as the idle bus level (`0x000000ff`), which is easy to mistake for a rejected packet. This example pads the wire length and keeps `txpktsize` at the true payload length, so payload sizes that are not multiples of 4 work too. Bits 5..16 of `SPI_HISR` are unused, so they are used to recognise an undriven trailer.

# Supported IC

RTL8721F
