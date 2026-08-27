> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

GSPI power-save test, Host side. The Host is a plain SPI master speaking the GSPI protocol; the
Device board runs `mbed_gspi_device_ps`. Four phases, each switchable by a macro:

| Phase | Who sleeps | What it verifies |
|---|---|---|
| Probe | nobody | every PM register and pin the wake-up flows use, one at a time |
| A | Device | Host suspends/resumes the Device with RPWM2 CG/ACT and waits on CPU_RDY_IND, the same sequence `whc_sdio_host_rpwm_notify()` uses over SDIO |
| B | Host | the Device wakes the clock-gated Host over SPI_INT, once per trigger: queued data (RX_REQUEST), CPWM1 toggle, CPWM2 toggle |
| C | alternating | A and B repeated as a long run, rotating the wake trigger |

Probe items:

| Item | Checks |
|---|---|
| P1 | HRPWM (0x80) bit7 toggle raises RPWM1_INT at the Device |
| P2 | HRPWM2 (0x82) exists in the SPI front end and reaches the Device's `rpwm_cb` |
| P3 | CPU_IND (0x87 bit0) follows the Device's `SDIO_SetReady()` |
| P4 | HCPWM2 (0x3A) mirrors the Device's CPWM2 ACT bit |
| P5 | queued data (method 1) and a CPWM1 toggle (method 2) both drive SPI_INT |
| P5' | the spare D2 line works as an out-of-band wake line |

# HW Configuration

Two RTL8721F boards, `SDIO_Pin_Grp = 2` in `component/soc/usrcfg/amebagreen2/ameba_intfcfg.c`, wired
pad to pad:

| Signal | Host | Device |
|---|---|---|
| SCLK | PA18 | PA18 (CLK) |
| MOSI | PA19 | PA19 (CMD) |
| MISO | PA28 | PA28 (D0) |
| CS | PA21 | PA21 (D3) |
| SPI_INT | PA20 | PA20 (D1) |
| WAKE | PA27 | PA27 (D2) |

# SW Configuration

| Macro | Default | Meaning |
|---|---|---|
| `GSPI_BUS_FREQ_HZ` | 10000000 | bus clock after mode-lock, max 25MHz |
| `PS_RUN_PROBE` | 1 | run the probe phase |
| `PS_RUN_SUSPEND_RESUME` / `PS_CYCLES` | 1 / 3 | phase A and its cycle count |
| `PS_SLEEP_MS` | 3000 | how long the Device is left asleep per cycle |
| `PS_RPWM_TIMEOUT_MS` | 1000 | CPU_RDY_IND handshake timeout |
| `PS_RUN_HOST_SLEEP` / `PS_HOST_SLEEP_CYCLES` | 1 / 2 | phase B and its cycle count per trigger |
| `PS_HOST_WAKE_TIMEOUT_MS` | 10000 | wake timeout; must exceed the Device's `PS_LATE_DELAY_MS` |
| `PS_HOST_SYSACTIVE_MS` | 2000 | Host awake time after a wake |
| `PS_RUN_LONGRUN` / `PS_LONGRUN_ROUNDS` | 1 / 10 | phase C (needs A and B enabled), 0 = forever |
| `PS_WATCH_MS` / `PS_WATCH_STEP_MS` | 500 / 10 | probe observation window and poll period |
| `GSPI_FREQ_ACTIVATE` | 1000000 | clock used for the activation frame |

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

```
[GSPI_HOST_PS] link up: CPU_RDY=1 HIMR=0x000c0003, handshake OK
[GSPI_HOST_PS] ==== probe start ====
[GSPI_HOST_PS] P1 HRPWM: wrote 0x80, readback 0x00 (Device logs RPWM INT)
[GSPI_HOST_PS] P2 HRPWM2: wrote 0x8001, readback 0x0001 (Device logs rpwm_cb)
[GSPI_HOST_PS] P3 CPU_RDY_IND: cleared @0ms, restored @3010ms (2 transitions)
[GSPI_HOST_PS] P4 HCPWM2(ACT bit0): 0x0001 -> CPWM0 0x0000 -> CPWM1 0x0001
[GSPI_HOST_PS] P5a queued data: INT low=1 WAKE low=0, HISR=0x00080001 RX0=0xc0000014
[GSPI_HOST_PS] P5b CPWM1 toggle: INT low=1, CPWM1_INT in HISR=1
[GSPI_HOST_PS] P5' D2 pulse: WAKE low=1 INT low=0
[GSPI_HOST_PS] ==== probe done ====
[GSPI_HOST_PS] cycle 1: CG acked 2ms, asleep CPU_IND=0x00 HCPWM2=0x0001, ACT acked 4ms
[GSPI_HOST_PS] ==== phase A done: 0/3 failed ====
[GSPI_HOST_PS] host resume: wake status 00020004 00000000
[GSPI_HOST_PS] cycle 1(data): woken by SPI_INT, HISR=0x00000001, 20 bytes read, INT now 1
[GSPI_HOST_PS] cycle 1(cpwm1): woken by SPI_INT, HISR=0x00040000, 0 bytes read, INT now 1
[GSPI_HOST_PS] cycle 1(cpwm2): woken by SPI_INT, HISR=0x00080000, 0 bytes read, INT now 1
[GSPI_HOST_PS] ==== phase B done: 0/6 failed ====
[GSPI_HOST_PS] ==== phase C done: 0 suspend fails, 0 wake fails ====
```

P1/P2 are confirmed on the Device side, so capture both logs. In phase B the Host's wake status has
bit17 set (`WAKE_SRC_GPIOA`); the Device's phase A log shows `WAK_STATUS1` bit1
(`WAKE_SRC_SDIO_WIFI`).

# Note

1. All bus access goes through the SoC driver `ameba_gspi.c` (`GSPI_Init`, `GSPI_Configuration`,
   `GSPI_ReadReg*`/`GSPI_WriteReg*`, `GSPI_WriteTxFifo`/`GSPI_ReadRxFifo`). The example only builds
   the TX descriptor and interprets the payload; frame layout, byte order, 4-byte padding, the HIMR
   shadow and DMA all live in the driver. Its own notes cover those details.
2. Both boards must be reset together: resetting one leaves the other mid-sequence.
3. `GSPI_Configuration()` must be the first bus transaction after the Device boots, otherwise the
   Device falls back to SDIO mode. `GSPI_Init()` already latches CS high before enabling its output,
   so the boot delay can be taken after it.
4. HIMR (0x14) and SPI_INT (0x04) cannot be read on this SoC; the driver shadows HIMR and
   `GSPI_GetINTMask()` returns that shadow.
5. `SPI_INT` is the level output of `HISR & HIMR`: low while a masked HISR bit is set, high once the
   cause is cleared. An unmasked cause never reaches the pin, so HIMR must unmask CPWM1/CPWM2 for
   their toggles to be usable as wake triggers.
6. Before sleeping to wait for a `SPI_INT` edge, drain pending RX data, clear the CPWM bits and
   discard stale semaphore counts. Any latched cause holds the pin low, so no new edge can come, and
   a stale edge would end the wait without the Host ever sleeping.
7. `RX0_REQ_LEN` rounds lengths up to 4 bytes, so a 19-byte payload is reported as 20.
8. Waking from clock gate needs no SPI re-initialisation; register and FIFO access work immediately.
9. The Fun-bit probe of earlier versions was dropped: it needed a hand-built Command word behind the
   driver's back. Result, for the record: reading with Fun = 0 returns the same values as Fun = 1, so
   the bit does not select a function or domain.

# Supported IC

RTL8721F
