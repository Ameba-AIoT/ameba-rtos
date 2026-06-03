# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Module Overview

`component/usb/` is the USB protocol stack for ameba-rtos. It covers three layers:
- **HAL layer** — SoC-specific platform adapter + DWC OTG controller access
- **Core layer** — USB device/host controller drivers (enumeration, transfer engine, interrupt handling)
- **Class layer** — Per-class protocol implementations (CDC ACM, MSC, UAC, HID, UVC, Vendor, etc.)

Application-level usage examples are in `example/usb/`.

## Directory Map

```
component/usb/
├── common/           # Shared: HAL API, OS abstraction, ring buffer, USB Ch9 types
│   ├── usb_hal.h/c   # DWC OTG register operations (FIFO, PHY, mode switch)
│   ├── usb_os.h/c    # OS abstraction wrapping os_wrapper.h
│   ├── usb_phy.c     # PHY register read/write and calibration
│   ├── usb_regs.h    # DWC OTG hardware register definitions
│   ├── usb_ch9.h     # USB Chapter 9 standard types (descriptors, setup requests)
│   ├── usb_ringbuf.h/c
│   └── usb_cdc_acm.h / usb_msc.h / usb_uac1.h / usb_uac2.h / usb_cdc_ecm.h / ...
├── hal/
│   └── ameba<chip>_usb.c  # One file per SoC, implements usb_hal_driver_t
├── device/
│   ├── core/         # usbd.h/c (public API), usbd_core, usbd_hal, usbd_pcd (ISR → events)
│   ├── cdc_acm/      # usbd_cdc_acm.h/c
│   ├── cdc_ecm/      # usbd_cdc_ecm.h/c
│   ├── hid/          # usbd_hid.h/c
│   ├── msc/          # usbd_msc.h/c + usbd_scsi.h/c
│   ├── uac/          # usbd_uac1.c / usbd_uac2.c (UAC 1.0 / 2.0)
│   ├── uvc/          # usbd_uvc.h/c + descriptor + event
│   ├── vendor/       # usbd_vendor.h/c
│   ├── inic/         # usbd_inic (AmebaGreen2)
│   ├── inic_dplus/   # usbd_inic (AmebaDplus)
│   └── composite/    # Multi-function combinations (CDC+HID, CDC+MSC, CDC+UAC, HID+UAC)
└── host/
    ├── core/         # usbh.h/c (public API), usbh_core, usbh_hal, usbh_hcd, usbh_hw_uvc
    ├── cdc_acm/      # usbh_cdc_acm.h/c
    ├── cdc_ecm/      # usbh_cdc_ecm.h/c
    ├── msc/          # usbh_msc.h/c + usbh_msc_scsi + usbh_msc_disk
    ├── uac/          # usbh_uac1.h/c
    ├── uvc/          # usbh_uvc_class/intf/parse/stream
    ├── vendor/       # usbh_vendor.h/c
    └── composite/    # Multi-function combinations (HID+UAC, CDC+ECM)
```

## Three-Layer Architecture

### HAL Layer

`usb_hal_driver_t` (defined in `common/usb_hal.h`) is the SoC-specific adapter interface:

```c
typedef struct {
    int  (*init)(u8 mode);                        // power on, clock, PHY enable
    int  (*deinit)(void);
    void (*enable_interrupt)(u8 priority);
    void (*disable_interrupt)(void);
    void (*register_irq_handler)(void *handler, u8 priority);
    void (*unregister_irq_handler)(void);
    usb_cal_data_t *(*get_cal_data)(u8 mode);     // PHY tuning values
    void (*cg)(u32 ms);                           // clock gating for power management
} usb_hal_driver_t;
```

Each `hal/ameba<chip>_usb.c` defines and exports `usb_hal_driver` (a global instance of this struct). The SoC-specific USB hardware constants — register base addresses (`USB_REG_BASE`, `USB_ADDON_REG_BASE`), hardware limits (`USB_MAX_ENDPOINTS`, `USB_MAX_PIPES`), and default VID/PID — come from:

```
component/soc/<chip>/fwlib/include/ameba_usb.h
```

For example: `component/soc/amebasmart/fwlib/include/ameba_usb.h`. The HAL source files include `ameba_soc.h` (which pulls in this header transitively via `ameba.h` → `ameba_usrcfg.h`).

`common/usb_hal.c` implements the generic OTG controller operations (FIFO config, soft reset, packet read/write, mode switching) on top of `usb_regs.h` register definitions. `common/usb_phy.c` handles PHY register access and calibration (not compiled when `CONFIG_SUPPORT_USB_NO_PHY` is set, e.g. AmebaL2).

### Core Layer

**Device** (`device/core/`):
- `usbd_pcd.c` — Peripheral Controller Driver. The USB ISR entry point. Decodes interrupt status and calls into `usbd_core.c`.
- `usbd_core.c` — Protocol state machine (enumeration, control transfers, EP data stages, connect/disconnect events). Calls `usbd_class_driver_t` callbacks.
- `usbd_hal.c` — Device-specific hardware operations (endpoint activate/deactivate, DMA setup).
- `usbd.c` — Public API surface: `usbd_init/deinit`, `usbd_ep_init/deinit`, `usbd_ep_transmit/receive`, `usbd_ep_set_stall/clear_stall`, `usbd_register_class/unregister_class`.

**Host** (`host/core/`):
- `usbh_hcd.c` — Host Controller Driver. ISR entry point. Channel/pipe management.
- `usbh_core.c` — Enumeration state machine, device attach/detach, SET_CONFIGURATION, class probing.
- `usbh_hal.c` — Host hardware operations (channel open/close, transfer submit).
- `usbh.c` — Public API: `usbh_init/deinit`, `usbh_open/close_pipe`, `usbh_transfer_data/process`, `usbh_ctrl_request`, `usbh_register_class/unregister_class`.
- `usbh_hw_uvc.c` — Hardware-accelerated UVC decoding path (compiled only when `CONFIG_SUPPORT_USBH_UVC_HW_DEC`).

### Class Layer

Device classes implement `usbd_class_driver_t`; host classes implement `usbh_class_driver_t`.

**Device class registration pattern** (from `example/usb/usbd_cdc_acm/`):
```c
// 1. Fill application-level callbacks
static usbd_cdc_acm_cb_t cdc_acm_cb = {
    .init           = cdc_acm_cb_init,
    .deinit         = cdc_acm_cb_deinit,
    .setup          = cdc_acm_cb_setup,      // ISR context
    .received       = cdc_acm_cb_received,   // ISR context
    .transmitted    = cdc_acm_cb_transmitted,// ISR context
    .status_changed = cdc_acm_cb_status_changed, // ISR context
};
// 2. Configure core
static usbd_config_t cdc_acm_cfg = {
    .speed       = USB_SPEED_HIGH,
    .isr_priority = INT_PRI_MIDDLE,
    // SoC-specific FIFO depths if needed (see ameba_usb.h for dedicated vs. shared FIFO)
};
// 3. Init class then core
usbd_cdc_acm_init(..., &cdc_acm_cb);
usbd_init(&cdc_acm_cfg);
```

**Host class registration pattern**:
```c
// 1. Fill class driver callbacks
static usbh_cdc_acm_cb_t acm_cb = { .init=..., .deinit=..., .receive=..., ... };
// 2. Configure and init
usbh_cdc_acm_init(&acm_cb);     // registers usbh_class_driver_t internally
usbh_init(&cfg, &user_cb);
```

## Critical ISR Constraint

**All class driver callbacks execute in interrupt context.** This applies to:
- `usbd_class_driver_t`: `get_descriptor`, `set_config`, `clear_config`, `setup`, `ep0_data_in/out`, `ep_data_in/out`, `status_changed`, `sof`
- `usbh_class_driver_t`: `sof`, `completed`
- All application callbacks registered with a class (e.g. `cdc_acm_cb_t::received`, `cdc_acm_cb_t::transmitted`, `cdc_acm_cb_t::status_changed`)

**Never call from these callbacks:** `malloc/free`, `rtos_sema_take`, `rtos_mutex_lock`, any blocking wait, or long-running operations. Post to a queue and handle in a task instead.

## OS Abstraction

Within the USB stack, use `usb_os_*` APIs from `common/usb_os.h`, not `rtos_*` directly:

| `usb_os_*` | Underlying `os_wrapper.h` type |
|---|---|
| `usb_os_lock_t` | `rtos_mutex_t` |
| `usb_os_sema_t` | `rtos_sema_t` |
| `usb_os_queue_t` | `rtos_queue_t` |
| `usb_os_task_t` | `rtos_task_t` |
| `usb_os_malloc/mfree` | `rtos_mem_malloc/free` |
| `usb_os_sleep_ms` | `rtos_time_delay_ms` |

## DMA Buffer Requirements

Buffers used for USB DMA transfers must be cache-line aligned to prevent cache coherency corruption:
```c
static u8 rx_buf[2048] USB_DMA_ALIGNED;  // __attribute__((aligned(CACHE_LINE_SIZE)))
USB_IS_MEM_DMA_ALIGNED(rx_buf);          // verify at runtime if needed
```

## Build Artifacts and Kconfig

The USB stack ships as prebuilt libraries for core/HAL layers; class code is compiled from source:

| Library | Source cmake | Enabled by |
|---|---|---|
| `lib_usb_hal.a` | `common/CMakeLists.txt` | `CONFIG_USB_HAL_LIB` |
| `lib_usbd.a` | `device/lib.cmake` | `CONFIG_USB_DEVICE_EN` |
| `lib_usbh.a` | `host/lib.cmake` | `CONFIG_USB_HOST_EN` |
| `lib_usb_drd.a` | `drd_lib.cmake` | `CONFIG_USB_DRD_EN` |
| `usb_device` (internal lib) | `device/usbd.cmake` → `class.cmake` | `CONFIG_USB_DEVICE_EN` |
| `usb_host` (internal lib) | `host/usbh.cmake` → `class.cmake` | `CONFIG_USB_HOST_EN` |

Top-level mode selection in Kconfig (`component/usb/Kconfig`):
- `USB_DEVICE_EN_MENU` → device only
- `USB_HOST_EN_MENU` → host only (unavailable when `SUPPORT_USB_DEVICE_ONLY`)
- `USB_DRD_EN_MENU` → simultaneous device + host (unavailable when `SUPPORT_USB_DEVICE_ONLY`)

AmebaDplus (`SUPPORT_USB_NO_PHY`) skips `usb_phy.c`. AmebaDplus/AmebaSmart/AmebaD use shared FIFO (`CONFIG_SUPPORT_USB_SHARED_DFIFO`) which changes `usbd_config_t`/`usbh_config_t` — no per-endpoint FIFO depth fields, different NPTX tuning.

## Adding a New Device Class

1. Create `device/<class>/usbd_<class>.h/c` following the pattern of an existing class.
2. Implement `usbd_class_driver_t` and call `usbd_register_class(&driver)` from the class init.
3. Add to `device/class.cmake` under the appropriate `CONFIG_USBD_XXX` guard.
4. Add `CONFIG_USBD_XXX bool` entry to `Kconfig`.
5. Create `example/usb/usbd_<class>/` with `app_example.c`, `example_usbd_<class>.c`, and `prj.conf`.

## Adding a New Host Class

1. Create `host/<class>/usbh_<class>.h/c` implementing `usbh_class_driver_t`.
2. Populate `id_table` (`usbh_dev_id_t[]`) with device/interface match criteria.
3. Call `usbh_register_class(&driver)` from class init.
4. Add to `host/class.cmake` and `Kconfig`.

## Running Examples

From the chip's gcc_project directory:
```bash
python build.py -a usbd_cdc_acm -p       # USB CDC ACM device
python build.py -a usbh_msc -p           # USB MSC host
python build.py -a usb_drd -p            # DRD (MSC device + MSC host simultaneously)
```

Each example's `prj.conf` sets the required Kconfig options (e.g. `CONFIG_USBD_CDC_ACM=y`).
