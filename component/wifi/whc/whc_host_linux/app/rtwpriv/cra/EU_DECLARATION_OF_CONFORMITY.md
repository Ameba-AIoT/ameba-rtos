# EU Declaration of Conformity (DoC)

**In accordance with Regulation (EU) 2024/2847 — Cyber Resilience Act (CRA)**

---

## 1. Product Identification

| Field | Value |
|-------|-------|
| **Product name** | rtwpriv |
| **Version** | V6.0.1_20260618 |
| **Description** | Realtek WiFi RF HW/SW Tx testing utility for mass production line RF calibration and regulatory (FCC/CE/MKK) certification testing |
| **Category** | Default (non-critical, not listed in CRA Annex III or IV) |
| **Conformity procedure** | Module A — Internal control (self-declaration) |

## 2. Manufacturer

| Field | Value |
|-------|-------|
| **Name** | Realtek Semiconductor Corp. |
| **Address** | No. 2, Innovation Road II, Hsinchu Science Park, Hsinchu 300, Taiwan |
| **Website** | https://www.realtek.com |
| **PSIRT contact** | willistang@realtek.com |

## 3. Compliance Statement

We, Realtek Semiconductor Corp., declare under our sole responsibility that
the product identified above complies with the essential requirements set out
in **Annex I (Part I and Part II)** of Regulation (EU) 2024/2847 (Cyber
Resilience Act).

### 3.1 Annex I Part I — Cybersecurity requirements for products with digital elements

| Requirement | How rtwpriv fulfills it |
|-------------|--------------------------|
| **§1(a)** — Secure by default | Netlink mode (Generic Netlink `whc_demo`) is the default transport (`NL_DEFAULT=y`). ioctl fallback available but opt-in. |
| **§1(b)** — No default passwords | N/A — rtwpriv has no authentication mechanism. |
| **§1(c)** — Vulnerability handling | See [`SECURITY.md`](SECURITY.md) for CVD process, PSIRT contact, and 90-day patch timeline. |
| **§1(d)** — Data minimization | rtwpriv does not collect, transmit, or store user data. |
| **§1(e)** — Input validation | Commands are parsed and validated by `rtw_parse.cpp` before sending to kernel. |
| **§1(f)** — Attack surface reduction | Static linking (`-static`) eliminates runtime library injection vectors. |
| **§1(g)** — Tamper protection | Stack canary (`-fstack-protector-strong`) and `_FORTIFY_SOURCE=2` enabled at build. |
| **§1(h)** — Security updates | Provided per [`SECURITY.md`](SECURITY.md). Support period is 5 years from release. |

### 3.2 Annex I Part II — Vulnerability handling requirements

| Requirement | How rtwpriv fulfills it |
|-------------|--------------------------|
| **§2(1)** — SBOM | CycloneDX 1.5 SBOM maintained at [`bom.json`](bom.json). |
| **§2(2)** — Timely patches | Policy: acknowledgment within 72 h, patch within 90 days. |
| **§2(3)** — Coordinated disclosure | CVD procedure documented in [`SECURITY.md`](SECURITY.md). |
| **§2(4)** — Security advisory | Advisories published via Realtek PSIRT. |
| **§2(5)** — Known vulnerability reporting | Automatic scanning via `osv-scanner` (see [`scan-cve.sh`](scan-cve.sh)). |
| **§2(6)** — Support period | 5 years from each major version's release date. |

## 4. Supporting Documents

The following technical documentation is maintained as part of the product
and is available upon request:

| Document | Location |
|----------|----------|
| SBOM (CycloneDX 1.5) | [`bom.json`](bom.json) |
| Vulnerability disclosure policy | [`SECURITY.md`](SECURITY.md) |
| Security architecture | [`ARCHITECTURE.md`](ARCHITECTURE.md) |
| Build configuration | [`Makefile`](../Makefile) |
| Usage and safety notes | [`readme.txt`](../readme.txt) |
| CVE scanning automation | [`scan-cve.sh`](scan-cve.sh) |

## 5. CE Marking

CE marking was first affixed in: **2026**

## 6. Place and Date of Issue

| Field | Value |
|-------|-------|
| **Place** | Hsinchu, Taiwan |
| **Date** | 2026-07-06 |

---

```
_________________________________________
Authorised signatory:

Name:   [Manufacturer Representative]
Title:  [Title]
```

> **Note to the user**: This is a self-declaration template. Replace the
> bracketed fields `[Manufacturer Representative]` and `[Title]` with the
> actual signatory before formal issuance.
