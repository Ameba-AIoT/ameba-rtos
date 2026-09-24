# WHC GSPI host driver — build & test

The GSPI host driver talks to a Realtek device that runs its SPDIO IP in
SPI-slave mode. The host is an SPI master (SPI0 on the Raspberry Pi) plus a
DEV_INT GPIO. Wire framing, aggregation, power-management (RPWM) and the
suspend/resume flow all live in this directory.

Files:
- `whc_gspi_host_drvio.c`  — GSPI frame transport (CMD/data/Status over spi_sync)
- `whc_gspi_host_init.c`   — activation, IRQ mask, TXBD thresholds, rx-agg enable
- `whc_gspi_host_ops.c`    — TX/RX data path + TX/RX aggregation
- `whc_gspi_host_probe.c`  — SPI driver, DEV_INT IRQ, suspend/resume (RPWM)
- `whc_gspi_host.h`        — all compile-time switches (below)
- `whc_gspi_host_reg.h`    — device register map (mirror of soc `ameba_gspi.h`)

---

## 1. Compile-time switches (in `whc_gspi_host.h`)

|        Macro        | Default |                                  Meaning                                                 |
|---------------------|---------|------------------------------------------------------------------------------------------|
| `GSPI_INPLACE_XFER` |    1    | 1 = tx/rx share one buffer (in-place, bcm2835 OK); 0 = separate rx buffer (portable)     |
| `WHC_TX_AGG`        |   on    | coalesce several queued frames into one bus write                                        |
| `WHC_RX_AGG`        |   on    | split one hardware-aggregated FIFO read into per-packet skbs                             |
| `GSPI_PM_TEST`      |   0     | expose `/sys/.../gspi_pm` to drive suspend/resume manually (see §4)                      |

---

## 2. Build

Out-of-tree kernel module, Kconfig-driven config:

```bash
make prepere     //excute in ameba sdk， and then copy to linux
make menuconfig
make
```

Produces `driver/gspi/whc_gspi.ko`.

---

## 3. Load & basic bring-up

```bash
sudo insmod whc_gspi.ko
```

To see the driver's `dev_dbg` lines:

```bash
echo 'module whc_gspi +p' > /sys/kernel/debug/dynamic_debug/control
```

---

## 4. Test suspend / resume flow WITHOUT kernel suspend

When the kernel has `CONFIG_SUSPEND` off (no `echo mem/freeze`), use the
`GSPI_PM_TEST` hook to call the driver's `.suspend`/`.resume` directly while the
host stays active. This exercises the full RPWM + netif-queue flow.

Find the sysfs node (path depends on the SPI bus/CS):

```bash
ls /sys/bus/spi/devices/*/gspi_pm
P=/sys/bus/spi/devices/spi0.0/gspi_pm     # adjust to the actual path
```

Run the flow (watch dmesg in another shell: `dmesg -w`):

```bash
echo 1 > $P     # -> suspend flow
echo 0 > $P     # -> resume flow
dmesg | grep -iE "gspi_pm|rpwm|suspend|resume|whc_gspi"
```

What each side does:

**suspend** (`echo 1`)
1. refuse if AP (`pndev[1]`) is up
2. `wowlan_state = 1`, `netif_tx_stop_all_queues`
3. RPWM SUSPEND: toggle HRPWM to raise the device RPWM interrupt, clock-gate it,
   wait for CPU_RDY to clear
4. release the DEV_INT irq

**resume** (`echo 0`)
1. RPWM RESUME: toggle HRPWM, wait for CPU_RDY set
2. `netif_tx_start/wake_all_queues`, `wowlan_state = 0`

Notes:
- The device must actually be attached and running: RPWM waits up to ~1 s for
  CPU_RDY. If the device is absent, `echo 1/0` blocks ~1 s then reports a
  non-zero return in dmesg — that also verifies the timeout/rollback path.
- `gspi_pm` is write-only. `echo 1` twice in a row is a no-op the second time
  (already SLEEP); same for `echo 0`.
- Set `GSPI_PM_TEST 0` for production; the sysfs node is then not compiled in.

---

## 5. Testing real system suspend (only if CONFIG_SUSPEND is enabled)

If the kernel does support sleep:

```bash
cat /sys/power/state            # need 'freeze' (s2idle) and/or 'mem'
echo freeze > /sys/power/state  # s2idle: your .suspend is called
# wake via serial console key / USB, then .resume is called
```

`.suspend`/`.resume` are invoked for **any** sleep depth (freeze/mem), so
s2idle is enough to validate the flow. Waking the host from the device pulling
DEV_INT low is a separate concern (needs `enable_irq_wake` + platform GPIO-wake
support) and is NOT required just to test the flow.

