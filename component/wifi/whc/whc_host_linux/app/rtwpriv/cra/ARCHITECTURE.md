# rtwpriv Architecture

**Version**: V6.0.1_20260618
**License**: GPL-2.0-only
**Language**: C++ (compiled with g++)

---

## 1. Overview

rtwpriv is a Linux userspace command-line tool that configures and controls
Realtek WiFi adapters in **Mass Production (MP) mode** — an RF calibration
and testing mode used during manufacturing and regulatory certification.

```
┌──────────────────────────────────────────────────┐
│                   rtwpriv CLI                      │
│  (rtwpriv.cpp — argument parsing, dispatch)       │
├──────────────────────────────────────────────────┤
│                                                    │
│  ┌────────────┬──────────┬───────────────┐        │
│  │  IOCTL     │ Netlink  │   UDP Server  │        │
│  │ (SIOCDEV-  │ (Generic │ (mp_server)   │        │
│  │  PRIVATE)  │ Netlink) │               │        │
│  └─────┬─────┘─────┬────┴───────┬───────┘        │
│        │           │            │                  │
│        │   rtw_nl.c            │                  │
│        │   rtw_nl.h            │                  │
│        │   rtw_nl_cmd.c        │                  │
│        │   rtw_nl_cmd.h        │                  │
│        │                       │                  │
│  ┌─────┴───────────────────────┴───────┐          │
│  │        rtw_transport.c / .h         │          │
│  │   Transport abstraction layer       │          │
│  └─────┬───────────────────────┬───────┘          │
│        │                       │                  │
│        ▼                       ▼                  │
│  ┌──────────┐          ┌──────────────┐           │
│  │ Driver   │          │ User-defined │           │
│  │ (ioctl)  │          │ handler cmds │           │
│  └──────────┘          └──────────────┘           │
│                                                    │
│  ┌──────────────────────────────────────┐          │
│  │         rtw_parse.c / .h             │          │
│  │   Command parsing & validation       │          │
│  └──────────────────────────────────────┘          │
│                                                    │
│  ┌──────────────────────────────────────┐          │
│  │         rtw_hwtx.c / .h              │          │
│  │   HW TX mode (PMAC) configuration    │          │
│  └──────────────────────────────────────┘          │
│                                                    │
│  ┌──────────────────────────────────────┐          │
│  │      rtw_udpsrv.c / .h              │          │
│  │   UDP server for automated test      │          │
│  └──────────────────────────────────────┘          │
└──────────────────────────────────────────────────┘
                        │
                        ▼
              ┌──────────────────┐
              │   Linux Kernel    │
              │  ┌────────────┐   │
              │  │ whc_demo   │   │  Generic Netlink family
              │  │ (GENL_ID)  │   │
              │  └─────┬──────┘   │
              │        │          │
              │  ┌─────┴──────┐   │
              │  │ WiFi Driver│   │
              │  │ (wlan.ko)  │   │
              │  └─────┬──────┘   │
              └────────┼──────────┘
                       │
                       ▼
              ┌──────────────────┐
              │  WiFi Hardware   │
              └──────────────────┘
```

---

## 2. Module Description

### 2.1 `rtwpriv.cpp` — Entry Point & CLI Dispatcher

- Parses command-line arguments (ifname, flags like `-n`)
- Delegates to the appropriate transport layer via `rtw_xport_from_args()`
- Routes to HW TX (`rtw_hwtx`), UDP server (`rtw_udpsrv`), or generic MP
  command dispatch

### 2.2 `rtw_transport.cpp /.h` — Transport Abstraction

Provides a uniform interface for sending MP commands regardless of the
underlying transport:

```c
typedef enum {
    RTW_XPORT_IOCTL,    // SIOCDEVPRIVATE (ioctl)
    RTW_XPORT_NETLINK,  // Generic Netlink (whc_demo)
} rtw_xport_type_t;

int rtw_mp_send(rtw_xport_ctx_t *ctx, char *cmd, size_t cmdlen, int expect_reply);
```

- **Netlink mode** is the **default** when `NL_DEFAULT=y` (the build default)
- Falls back to ioctl when Netlink fails or the `-n` flag is absent

### 2.3 `rtw_ioctl.cpp /.h` — SIOCDEVPRIVATE Transport

Legacy transport using the **SIOCDEVPRIVATE** ioctl. Opens a raw socket and
sends MP commands to the kernel driver.

### 2.4 `rtw_nl.cpp /.h` — Generic Netlink Transport (whc_demo)

Modern transport using the kernel's **Generic Netlink** family `"whc_demo"`:

1. Opens a `NETLINK_GENERIC` socket
2. Resolves the `"whc_demo"` family ID via `CTRL_CMD_GETFAMILY`
3. Sends `CMD_WIFI_INFO_INIT` to register the socket for reply delivery
4. Each MP command is sent as a netlink message with `WHC_CMD_ECHO` type
5. Waits for `WHC_CMD_EVENT` replies if `expect_reply` is set

**Security attributes**:
- Bound socket (not broadcast)
- Family ID dynamically resolved (no hardcoded magic numbers)

### 2.5 `rtw_nl_cmd.cpp /.h` — Netlink Helper Commands

A small set of **command shortcuts** parsed by rtwpriv itself (not forwarded
as raw MP commands):

| Command | Handler | Description |
|---------|---------|-------------|
| `getmac <idx>` | `handle_getmac` | Read MAC address via `WHC_WIFI_TEST_GET_MAC_ADDR` |
| `getip <idx>` | `handle_getip` | Read IP / gateway via `WHC_WIFI_TEST_GET_IP` |
| `logon` | `handle_logon` | Enable firmware log via `WHC_WIFI_TEST_LOG_ENABLE` |
| `logoff` | `handle_logoff` | Disable firmware log via `WHC_WIFI_TEST_LOG_DISABLE` |
| `dbg <cmd>` | `handle_dbg` | Send a raw debug string via `CMD_WIFI_DBG` |

All other commands are forwarded as-is over `CMD_WIFI_MP`.

### 2.6 `rtw_parse.cpp /.h` — Command Parser

Parses MP command strings into internal data structures. Supports:
- Bandwidth / channel parsing
- Rate ID → rate index conversion (CCK / OFDM / HT / VHT MCS)
- TX mode extraction (continuous, packet, single tone, carrier suppression)
- Power index parsing

### 2.7 `rtw_hwtx.cpp/.h` — HW TX Mode Control

Implements the HW (PMAC) TX path for Wi-Fi chip standards that support it.
Configures fields such as:
- Channel, bandwidth, antenna path
- Rate (MCS), guard interval, STBC, LDPC
- Packet period, packet count, packet pattern
- PPDU type, RU allocation (for AX chipsets)

### 2.8 `rtw_udpsrv.cpp/.h` — UDP Test Server

Provides a lightweight UDP server for automated test frameworks (e.g. RTK
Robot). Receives test commands over network and dispatches them as MP
commands.

---

## 3. Security Architecture

### 3.1 Trust Boundary

```
[User] ──► rtwpriv ──► Kernel (whc_demo) ──► WiFi Firmware ──► RF HW
  ▲                       ▲
  │                       │
  └─── CAP_NET_ADMIN ─────┘
      required for raw
      sockets & ioctl
```

- rtwpriv requires **`CAP_NET_ADMIN`** (typically root) for both ioctl and
  Netlink modes
- No network-facing services are enabled by default; the UDP server
  (`mp_server`) is opt-in via explicit command
- The `whc_demo` Netlink family is registered by the kernel WiFi driver
- Userspace-to-kernel communication is **local only** (not routable)

### 3.2 Attack Surface

| Vector | Exposure | Mitigation |
|--------|----------|------------|
| Malformed command args | Medium | Input parsing in `rtw_parse.cpp` validates format |
| Buffer overflow | Medium | `-fstack-protector-strong` + `_FORTIFY_SOURCE=2` |
| UDP server (mp_server) | Low | Opt-in, no authentication, intended for lab use only |
| Kernel driver interface | Low | Depends on driver integrity (out of scope) |
| SBOM vulnerabilities | Low | Regular scanning via `scan-cve.sh` |

### 3.3 Compile-Time Protections

| Flag | Applied to | Effect |
|------|-----------|--------|
| `-fstack-protector-strong` | All source | Stack canary on functions with local arrays |
| `-D_FORTIFY_SOURCE=2` | All source | Runtime bounds check on `sprintf`/`memcpy`/`strcpy` |
| `-static` | Link | Eliminates shared library injection vectors |
| `-Wall` | All source | All warnings enabled during compilation |

### 3.4 Runtime Considerations

- **Netlink mode is preferred** over ioctl mode (NL_DEFAULT=y)
- rtwpriv includes a clear warning: *"Customer must remove the tool when
  shipping the product to end-customers"*
- The tool has no network listener unless `mp_server` is explicitly started

---

## 4. Build System

See [`Makefile`](../Makefile) and [`Android.mk`](../Android.mk).

Primary build:
```bash
make                # builds rtwpriv (Netlink mode enabled by default)
make NL_DEFAULT=n   # builds rtwpriv with ioctl as the default transport
```

Cross-compilation targets (by setting `CC`):
- `arm-linux-androideabi-g++` → `rtwpriv_arm`
- `aarch64-linux-android-g++` → `rtwpriv_arm64`
- `i686-linux-android-g++` → `rtwpriv_x86`
- `x86_64-linux-android-g++` → `rtwpriv_x86_64`

---

## 5. CRA Compliance References

| Document | Location |
|----------|----------|
| SBOM (CycloneDX 1.5) | [`bom.json`](bom.json) |
| Vulnerability disclosure policy | [`SECURITY.md`](SECURITY.md) |
| Declaration of Conformity | [`EU_DECLARATION_OF_CONFORMITY.md`](EU_DECLARATION_OF_CONFORMITY.md) |
| CVE scan automation | [`scan-cve.sh`](scan-cve.sh) |
