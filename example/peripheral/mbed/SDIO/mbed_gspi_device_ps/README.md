# Example Description

Device side of the GSPI power-save test; the Host board runs `raw_gspi_host_ps` and drives the whole
sequence over the GSPI bus. This side provides:

- The Phase 0 handshake of `mbed_sdio_device_tp`, so the Host's link-up code is unchanged.
- `rpwm_cb`: RPWM2 `CG` clears CPU_RDY_IND and drops the wakelock so the board clock-gates, RPWM2
  `ACT` takes the wakelock back and sets CPU_RDY_IND. Same handling as `whc_sdio_dev.c`.
- `WAKE_SRC_SDIO_WIFI` as wake source plus sleep hooks, so the Host's RPWM write is what wakes the
  board and the reason is visible in `WAK_STATUS1`.
- An ASCII command channel, so the Host can sequence each step instead of relying on timing:

| Payload | Action |
|---|---|
| `PS:RDY0` | clear CPU_RDY_IND for `PS_NOT_RDY_MS`, then restore it automatically |
| `PS:CPWM0` / `PS:CPWM1` | clear / set the CPWM2 ACT bit, visible to the Host in HCPWM2 (0x3A) |
| `PS:CP1TG` | toggle CPWM1, the documented alternative way to interrupt the Host |
| `PS:TXDAT` | queue one Device -> Host packet |
| `PS:LATE` | wait `PS_LATE_DELAY_MS`, then queue a packet — wakes a sleeping Host via RX_REQUEST |
| `PS:LTCP1` / `PS:LTCP2` | wait, then toggle CPWM1 / CPWM2 — wakes a sleeping Host via CPWM1_INT / CPWM2_INT |
| `PS:GPIO` | pulse the out-of-band wake line (D2) low |

# HW Configuration

Two RTL8721F boards, `SDIO_Pin_Grp = 2` in `component/soc/usrcfg/amebagreen2/ameba_intfcfg.c`, wired
pad to pad with the Host: CLK/CMD/D0/D3 are the Host's SCLK/MOSI/MISO/CS, D1 is SPI_INT, D2 is the
wake line.

# SW Configuration

| Macro | Default | Meaning |
|---|---|---|
| `PS_NOT_RDY_MS` | 3000 | how long `PS:RDY0` keeps CPU_RDY_IND low |
| `PS_WAKE_PULSE_MS` | 100 | `PS:GPIO` low pulse width |
| `PS_LATE_DELAY_MS` | 5000 | `PS:LATE` delay before queueing data |
| `PS_WAKE_PIN` | `_PA_27` | out-of-band wake line (SDIO D2) |
| `PS_ENABLE_SLEEP` | 1 | 0 keeps the board always awake and ignores RPWM2 CG |
| `PS_SLEEP_TYPE` | `SLEEP_CG` | clock gate only, see the notes |
| `PS_SYSACTIVE_MS` | 2000 | awake time after a resume |

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

```
[GSPI_DEV_PS] ready: sleep=1 type=CG wake_pin=PA27 lockbit=0x00000200
[GSPI_DEV_PS] handshake #1, replying
[SPDIO] RPWM INT triggered, value is 0x0
[SPDIO] RPWM2 INT triggered, value is 0x1
[GSPI_DEV_PS] rpwm_cb ACT: CPU_RDY_IND 1 (RPWM2=0x00000001)
[GSPI_DEV_PS] RDY0: CPU_RDY_IND 0 for 3000 ms, restored
```

One suspend / resume round driven by the Host. The suspend line is cut short because the UART stops
once the clocks gate; `NPCW` / `APCW` are the SDK's clock-gate markers:

```
[GSPI_DEV_PS] rpwm_cb CG: CPU_RDY_IND 0, wakelock released
[GSPI_DEV_PS] suspend
NPCW
APCW
[GSPI_DEV_PS] resume: wake status 00000004 00000002
[SPDIO] SDIO_WIFI_BIT_HOST_WAKE_CPU_INT triggered
[GSPI_DEV_PS] rpwm_cb ACT: CPU_RDY_IND 1 (RPWM2=0x00000001)
```

`WAK_STATUS1` bit1 is `WAKE_SRC_SDIO_WIFI`, so `00000002` confirms the Host's RPWM write woke the
board.

# Note

1. `spdio_api.c` calls `rpwm_cb` without a NULL check and `spdio_structinit()` does not initialise the
   field, so any firmware that can receive an RPWM2 interrupt must install the callback first.
2. Only RPWM2 reaches `rpwm_cb`; RPWM1 is handled inside `SPDIO_Notify_INT()` and only logged.
3. `PS:RDY0` restores CPU_RDY_IND itself: a not-ready Device cannot be reached over the data path, so
   a restoring command could not be delivered.
4. Two defaults have to be overridden, without editing usrcfg files: `WAKE_SRC_SDIO_WIFI` is only a
   wake source for `CONFIG_WHC_INTF_SDIO` builds (`ameba_sleepcfg.c`), and `ameba_pmu.c` defaults to
   `SLEEP_PG` outside WHC builds.
5. Clock gate only. Power gate cuts the SDIO front end, so `spdio_deinit()` / `spdio_init()` would be
   needed around the sleep and the Host could no longer wake the board with an RPWM write — a
   Device-local wake source would be required instead.
6. Wakelock order matters: take `PMU_DEV_USER_BASE` first, then release `PMU_OS`. Releasing the OS
   lock is what enables tickless; holding our own lock keeps the board up until the Host asks for CG.
7. The wake pin must be re-routed to GPIO after `spdio_init()`, which configures all six pads as SDIO
   slave pins.
8. What raises the interrupt at the Host is the CCPWM/CCPWM2 "PS" bit, which `SDIO_CPWM1_Set()` and
   `SDIO_CPWM2_Set()` always set. CPWM1 therefore toggles a scratch bit, while CPWM2 re-asserts ACT so
   the Host's view of the power state stays correct.

# Supported IC

RTL8721F
