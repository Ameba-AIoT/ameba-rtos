# Example Description

This example synchronizes the AP TSF from the device to a Linux host (Raspberry Pi) connected over SDIO in WHC mode, so host applications can read the current TSF with µs accuracy. The system clock is not modified.

The time phase is carried by a GPIO line (rising edge, ns accuracy) while SDIO only carries the TSF value, where a few ms of latency does not matter. The two are paired by `seq`, whose uniqueness is guaranteed by a stop-and-wait handshake:

```
device                                     host (whc_sdio.ko, in-kernel state machine)
  |- PA0 rising edge --------wire--------> GPIO18 hard irq -> ktime_get_raw()
  |  reads port0 TSF with interrupts off
  |- [WIFI_TEST][TSF_SAMPLE](seq,tsf) -SDIO-> whc_host_tsf_sync_push()
  |  waits for ACK, resends the same         pairs them, publishes the latch to
  |  (seq,tsf) after 500ms, no new pulse     /dev/tsf_latch (seqlock, mmap read-only)
  |<-SDIO- [WIFI_TEST][TSF_ACK](seq,DONE)    ACK sent from a workqueue
     seq++, next round after 500ms
```

Only one `(pulse, tsf, seq)` is in flight at any time, so a mismatched pairing is not possible. Sync points are maintained entirely in the kernel, so no userspace daemon is needed.

# HW Configuration

One wire between the device and the host:

| device | host (Raspberry Pi) | usage |
|---|---|---|
| PA0 | GPIO18 | calibration pulse, once every 0.5s |

The GPIO global number is per-machine since kernel 6.6 (gpiolib assigns chip bases dynamically). Verify it before bring-up and fix `TSF_PULSE_PIN` in `component/wifi/whc/whc_host_linux/common/tsf_sync/whc_host_tsf_sync.h` if it differs:

```bash
sudo cat /sys/kernel/debug/gpio | grep GPIO18
```

# SW configuration

1. Use menuconfig to enable the following on the device side:

	| config | value | reason |
	|---|---|---|
	| `CONFIG_WHC_DEV` | y | example enable condition |
	| `CONFIG_WHC_CMD_PATH` | y | both directions go over the cmd path |

2. On the host side, `CONFIG_WHC_TSF_SYNC = y` in `component/wifi/whc/whc_host_linux/Makefile`.

3. How to use:
	- Run `./ameba.py build -a wifi_tsf_sync` under sdk path to generate images.
	- `Download` images to the board by Ameba Image Tool.
	- Build and load the host driver, then build the application library:
		```bash
		cd component/wifi/whc/whc_host_linux && make
		sudo insmod common/sdio/whc_sdio.ko
		cd app/tsf_sync && make
		```

4. Connect the device to an AP. TSF sync starts automatically on association and stops on disconnect.

5. Applications read the TSF through `tsf_time.c/.h` in `component/wifi/whc/whc_host_linux/app/tsf_sync/`: call `tsf_now_us()` for the current TSF or `tsf_at_mono_us()` for the TSF at a past CLOCK_MONOTONIC_RAW instant. The latch is mmap'ed on first use, so no explicit init is required.

# Expect result

After the device associates with an AP, dmesg on the host prints one pairing line every 0.5s:

```
tsf_sync: seq=12 TSF=1473497380339 us H_mono=4748200415355 ns (pair=12)
```

`tsf_now_us()` then returns a valid TSF. Warnings are printed if a pulse is missing or an ACK does not arrive; both are recovered automatically by the handshake.

# Note

The example is only compiled in when `-a wifi_tsf_sync` is given. Missing it is not an error: the firmware still runs wifi normally, but TSF sync silently does nothing. Check the boot log for `tsf_sync example loaded` to confirm.

# Supported IC

RTL8721F
