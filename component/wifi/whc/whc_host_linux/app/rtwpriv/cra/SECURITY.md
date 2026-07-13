# Security Policy — rtwpriv

## Supported Versions

| Version | Supported          |
|---------|--------------------|
| 6.0.x   | ✅ Active support  |
| < 6.0   | ❌ End of life     |

> **Support period**: Security updates are provided for **5 years** from the
> release date of each major version (e.g. V6.0.0 → V11.0.0).
> After that period, users must upgrade to a supported version.

---

## Reporting a Vulnerability

### Contact

Please report security vulnerabilities to:

**Email**: `willistang@realtek.com`

### Disclosure Timeline

| Step | Expected time |
|------|---------------|
| Acknowledgment | within **72 hours** of report |
| Initial assessment | within **5 business days** |
| Patch / mitigation plan | within **90 days** (or earlier) |
| Public disclosure | coordinated with reporter |

### What to include

- Product name and version (`rtwpriv -v`)
- Affected source files (if known)
- Description of the vulnerability
- Steps to reproduce (PoC preferred)
- Potential impact

### Scope

The following are **in scope**:
- rtwpriv source code (`rtwpriv.cpp`, `rtw_nl.cpp`, `rtw_ioctl.cpp`, etc.)
- Build scripts (`Makefile`, `Android.mk`)
- rtwpriv's interaction with the kernel `whc_demo` Netlink driver

The following are **out of scope**:
- Third-party kernel drivers (e.g., `wlan.ko`, `whc_demo.ko`)
- Third-party libraries listed in `bom.json`

### Coordinated Disclosure

We follow a **Coordinated Vulnerability Disclosure (CVD)** process:
1. Reporter submits report via email
2. Realtek PSIRT acknowledges and investigates
3. A fix is developed and tested
4. Patch is released to customers / published in repository
5. Public disclosure is made jointly with the reporter

---

## Security Features

### Compile-time protections (Makefile)

| Flag | Purpose |
|------|---------|
| `-fstack-protector-strong` | Stack canary to detect buffer overflows |
| `-D_FORTIFY_SOURCE=2` | Runtime bounds checking on `sprintf`, `memcpy`, `strcpy`, etc. |
| `-static` | Static linking reduces runtime dependency attacks |

### Runtime security notes

- **rtwpriv requires `CAP_NET_ADMIN` or root** to access raw sockets and
  ioctl (SIOCDEVPRIVATE). Run only in trusted manufacturing environments.
- **Netlink mode** (`-n` / `NL_DEFAULT=y`) uses Generic Netlink (`whc_demo`)
  and is preferred over ioctl mode.
- **Remove before shipping**: rtwpriv is a debugging / manufacturing tool
  and must NOT be included in end-customer firmware images.

---

## SBOM

A CycloneDX 1.5 Software Bill of Materials is maintained at `bom.json`.
It lists all direct and transitive dependencies with their licenses.
Regular vulnerability scanning against this SBOM is recommended:

```bash
osv-scanner --sbom=bom.json
```
