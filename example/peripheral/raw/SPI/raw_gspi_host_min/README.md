> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

Minimal GSPI bring-up test, running the **Host** (SPI Master) role on top of the SoC driver
`ameba_gspi.c`. It covers the control-register domain only; for the TX/RX FIFO data path and a
throughput test use `raw_gspi_host_fifo`, for the power-save flows `raw_gspi_host_ps`.

Three steps:

| Step | What it does | Why it matters |
|---|---|---|
| 1 | `GSPI_Configuration()` writes `SPI_CFG` and reads it back | activates GSPI mode; must be the first bus transaction after the Device boots |
| 2 | reads CPU_IND, HISR, RX0_REQ_LEN, FREE_TX_BD and HCPWM | proves the read path; a `CPU_IND` of `0xFF` means MISO is only idling at its pull-up |
| 3 | write-readback on HPS_CLKR (0x84) | proves the write path |

The paired Device board needs **no GSPI-specific code**: GSPI and SDIO share the pins and the bus
mode is decided entirely by this Host. `mbed_sdio_device_tp` or `mbed_gspi_device_ps` both work.

# HW Configuration

Required Components: Two RTL8721F boards.

| GSPI signal | Host board | Device board | Device SDIO signal |
|-------------|-----------|--------------|--------------------|
| SPI_CLK | PA18 | PA18 | CLK |
| SPI_MOSI | PA19 | PA19 | CMD |
| SPI_MISO | PA28 | PA28 | D0 |
| SPI_CS | PA21 | PA21 | D3 |
| SPI_INT | PA20 | PA20 | D1 |
| GND | GND | GND | — |

Pins are defined in `example_gspi_ext.h`. The table applies to pin group 2, selected by
`SDIO_Pin_Grp` in `component/soc/usrcfg/amebagreen2/ameba_intfcfg.c`; both boards must use the same
group. The Device's D2 (PA27) is unused here.

Flash the Device board first and reset it before the Host. The Host also waits
`GSPI_BOOT_DELAY_MS` after boot before touching the bus.

# SW Configuration

| Macro | Default | Meaning |
|---|---|---|
| `GSPI_BUS_FREQ_HZ` | 10000000 | bus clock after activation, max 25MHz |
| `GSPI_FREQ_ACTIVATE` | 1000000 | bus clock used for the activation frame |
| `GSPI_BOOT_DELAY_MS` | 3000 | wait before the first bus transaction |

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

```
[GSPI_HOST] Step1: activate GSPI (write SPI_CFG, read back)
[GSPI_HOST]   activated, bus now 10000000Hz
[GSPI_HOST] Step2: register reads
[GSPI_HOST]   CPU_IND(0x87) = 0x01, CPU_RDY=1
[GSPI_HOST]   HISR      (0x18) = 00080000   status 00000000 00000000
[GSPI_HOST]   RX0_REQ   (0x1c) = 40000000   status 00000000 00000000
[GSPI_HOST]   FREE_TX_BD(0x20) = 00000003   status 00000000 00000000
[GSPI_HOST]   HCPWM     (0x38) = 00010000   status 00000000 00000000
[GSPI_HOST] Step3: write-readback on HPS_CLKR(0x84)
[GSPI_HOST]   wrote 0x5a, read 0x5a MATCH (restored 0x00)
[GSPI_HOST] GSPI Demo: success
```

Reading these values: `RX0_REQ_LEN` resets to `0x40000000`, so that value simply means nothing is
pending. `HCPWM`'s upper half is HCPWM2, and its bit0 reflects the Device's CPWM2 ACT bit, set by the
Device's `spdio_init()`. `HISR` bit19 (CPWM2_INT) is latched from that same event.

If activation fails, the Device never left SDIO mode: check the wiring and make sure nothing touches
the bus before step 1.

# Note

1. All bus access goes through `ameba_gspi.c`; this example only calls `GSPI_Init`,
   `GSPI_Configuration` and `GSPI_ReadReg*`/`GSPI_WriteReg*`. Frame layout, byte order and CS timing
   live in the driver.
2. `GSPI_Configuration()` must be the first bus transaction after the Device boots. Any earlier bus
   activity, including a stray CS assertion during boot, makes the Device fall back to SDIO mode —
   which is why `GSPI_Init()` latches CS high before enabling its output driver.
3. Every operation also returns the frame's 8-byte Status phase. `sts.valid == 0` means the Device did
   not drive it, which is reported as "not driven" in the log above.
4. `SPI_CFG` names the endianness from the device's point of view: with `GSPI_BIG_ENDIAN_32` the bus
   carries a 32-bit word LSB first, which is the natural memory order of a little-endian host, so no
   software swap is needed after activation. Only the activation frame itself is encoded for the
   device's reset state (little endian) and swapped by the driver.
5. HPS_CLKR (0x84) must be written byte-wise: CPU_IND sits at 0x87 in the same 32-bit word, and a
   word write would clobber it.

# Supported IC

RTL8721F
