> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

This example demonstrates a TrustZone-protected mTLS (mutual TLS) client that connects to
a real Internet server (`client.badssl.com`) over WiFi:

- **Client certificate and private key** are managed in Secure World (image3).  
  The RSA-2048 private key never leaves the Secure World; only the certificate (public
  information) is returned to the Non-Secure side.
- **TLS CertificateVerify signing** is routed from the NS-side mbedtls state machine to
  the S-side via an NSC (Non-Secure Callable) interface using `mbedtls_pk_setup_rsa_alt`.
- **Server certificate verification** uses ISRG Root X1 (Let's Encrypt root CA) embedded
  in the firmware to verify the three-level chain:
  `ISRG Root X1 → R13 (Let's Encrypt) → *.badssl.com`.
- **Cipher suite** negotiated: `TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256`.  
  All application data is encrypted with AES-128-GCM by the TLS record layer automatically.

### Security boundary

| Item | Location | NS-readable |
|---|---|---|
| Client RSA-2048 private key (N/E/D/P/Q raw arrays) | image3 flash `.rodata` | No (HW protected) |
| RSA context (`g_cli_rsa`, imported at runtime) | image3 static SRAM `.bss` | No (HW protected) |
| Client certificate PEM | S-side → returned to NS | Yes (public info) |
| ISRG Root X1 CA | NS `.rodata` | Yes (public info) |
| AES-128-GCM session key | NS `ssl_context` heap | Yes (NS own memory) |

### Key files

| File | Role |
|---|---|
| `example_rdp_mutual_auth_rsa.c` | NS-side: WiFi, TLS client, NSC calls |
| `component/soc/common/image3/secure_client_service_rsa.c` | S-side: RSA private key storage and signing |
| `component/soc/common/image3/include/secure_client_service_rsa.h` | NSC interface declarations |

# HW Configuration

- The target chip must support TrustZone (`CONFIG_TRUSTZONE=y`).
- A WiFi access point reachable from the board is required.
- Internet access is required to reach `client.badssl.com:443`.

# SW Configuration

### 1. Set WiFi credentials

Edit the following macros at the top of `example_rdp_mutual_auth_rsa.c`:

```c
#define WIFI_SSID     "YourSSID"        /* replace with your AP SSID     */
#define WIFI_PASSWORD "YourPassword"    /* replace with your AP password  */
```

### 2. Client certificate and private key (BadSSL test credentials)

The client certificate embedded in `secure_client_service_rsa.c` (`s_cli_cert_pem`) and
the RSA-2048 private key stored as raw parameter arrays (`s_rsa_N`, `s_rsa_E`, `s_rsa_D`,
`s_rsa_P`, `s_rsa_Q`) are derived from the **BadSSL public test credentials**:

```
https://badssl.com/certs/badssl.com-client.pem
```

The private key is stored as five raw big-endian byte arrays rather than a PEM string.
This avoids `mbedtls_pk_parse_key()` and its `pkparse → x509_crt → ssl_tls → ssl_msg` dependency
chain (+61 KB), allowing image3 to fit within TZ_S_SIZE = 64 KB.  
The parameters are imported directly via `mbedtls_rsa_import_raw()` at runtime.

> **Note:** BadSSL rotates these test credentials periodically (approximately every two
> years). If the certificate has expired, download a fresh copy from the URL above, decrypt
> the private key with:
> ```bash
> openssl rsa -in <key.pem> -text -noout
> ```
> Extract the raw N/E/D/P/Q values and update both `s_cli_cert_pem` and the five
> `s_rsa_*` arrays in `secure_client_service_rsa.c`.  
> Current certificate validity: **2026-07-14 ~ 2028-07-13**.

### 3. Server trust anchor (ISRG Root X1)

`isrg_root_x1_pem` embedded in `example_rdp_mutual_auth_rsa.c` is the **ISRG Root X1**
certificate, the root CA used by Let's Encrypt to sign `client.badssl.com`'s certificate.  
Source: `/etc/ssl/certs/ISRG_Root_X1.pem` (or `https://letsencrypt.org/certs/isrgrootx1.pem`).  
Valid until: **2035-06-04**. No update needed until then.

### 4. TLS buffer sizes

`prj.conf` sets reduced TLS record buffer sizes to fit in the available heap:

```
CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN_MENU=4096
CONFIG_MBEDTLS_SSL_OUT_CONTENT_LEN_MENU=2048
```

If connecting to a server with a larger certificate chain, increase
`CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN_MENU` accordingly.

### 5. TrustZone SRAM size

`TZ_S_SIZE` defaults to **128 KB** when `CONFIG_TRUSTZONE_MBEDTLS=y` (set via Kconfig;
`platform_autoconf.h` is generated from it and should not be edited directly).
The RSA service code compiled into image3 occupies approximately **45.4 KB** (46,464 B)
in Flash (XIP) and **48.1 KB** (49,289 B) static SRAM.  
Within the 64 KB SRAM partition: 49,289 B static + 16,247 B S-heap = 65,536 B (64 KB exactly).  
The build system defaults to TZ_S_SIZE = 128 KB when `CONFIG_TRUSTZONE_MBEDTLS=y`;
in practice only the lower 64 KB is consumed, leaving the upper 64 KB as headroom.

### 6. image3 conditional compilation

`secure_client_service_rsa.c` is compiled into image3 **only when this example is built**,
controlled by the `CONFIG_IMG3_RSA_CLIENT_SERVICE=y` flag in `prj.conf`.
Other examples are not affected and get a smaller image3.

### 7. Build and flash

> **Note:** When switching from another example, add `-p` (pristine) to force the
> build system to re-merge `prj.conf` into the Kconfig configuration:

```bash
# Build (add -p when switching from another example)
./ameba.py build -a rdp_mutual_auth_rsa -q -p

# Subsequent incremental builds (no -p needed)
./ameba.py build -a rdp_mutual_auth_rsa -q

# Flash (adjust port and remote server as needed)
./ameba.py flash -p COM18 -b 1500000 \
    --remote-server <server_ip> --remote-password "<password>" \
    --image-dir build_RTL8720F
```

# Expected Result

```
[DEMO-I] === mTLS Client Demo ===
[DEMO-I] Client key in Secure World, sign via NSC
[SEC_CLI-I] RSA key len=2048 bits
[SEC_CLI-I] Client key/cert loaded into Secure World
[DEMO-I] S-side client key ready, cert_len=1687 B
[DEMO-I] Connecting to WiFi "YourSSID" ...
[$]wifi connected
[$]wifi got ip:"x.x.x.x"
[DEMO-I] WiFi ready, IP: x.x.x.x
[DEMO-I] Connecting to client.badssl.com:443 ...
[DEMO-I] TLS handshake ...
[DEMO-I]   cert OK (depth 2)
[DEMO-I]   cert OK (depth 1)
[DEMO-I]   cert OK (depth 0)
[SEC_CLI-I] S-side RSA sign: md_alg=9 hash_len=32
[SEC_CLI-I] S-side RSA sign OK, sig 256 B
[SEC_CLI-I] S heap total:    xxxxx B
[SEC_CLI-I] S heap free now: xxxxx B
[SEC_CLI-I] S heap min ever: xxxxx B
[SEC_CLI-I] S heap peak used:xxxxx B
[DEMO-I] Handshake OK
[DEMO-I] Cipher: TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256
[DEMO-I] Sending HTTP GET / ...
[DEMO-I] HTTP/1.1 200 OK
[DEMO-I] Total bytes received: 954
[DEMO-I] === Demo Complete: mTLS PASS, GCM PASS ===
[DEMO-I] NS heap free now : xxxxx B
[DEMO-I] NS heap min ever : xxxxx B
```

`HTTP/1.1 200 OK` confirms the server accepted the client certificate.  
Without a valid client certificate the server returns `HTTP/1.1 400 Bad Request`.

# Note

- `rtos_create_secure_context(2048)` must be called before any NSC function in the task.
- The secure entry functions `secure_client_init()` and `secure_client_sign()` are marked
  with `NS_ENTRY` and `IMAGE3_ENTRY_SECTION` attributes in image3.
- The `proxy_pk` context on the NS side contains no private key material; it only holds
  function pointers that forward signing requests to the S-side via NSC
  (runtime path: `rsa_alt_sign_wrap` in mbedtls → `nsc_sign_wrapper` → `secure_client_sign` NSC;
  `mbedtls_pk_setup_rsa_alt` is the one-time setup call that registers this chain).
- The private key is stored as raw byte arrays (`s_rsa_N/E/D/P/Q`) rather than PEM,
  imported via `mbedtls_rsa_import_raw()`. This avoids a +61 KB dependency chain
  (`pkparse → x509_crt → ssl_tls → ssl_msg`) that would otherwise overflow image3 SRAM.
- AES-128-GCM encryption/decryption is handled automatically by the TLS record layer;
  no explicit GCM API calls are needed in the application code.
- `CONFIG_IMG3_RSA_CLIENT_SERVICE=y` in `prj.conf` enables conditional compilation of
  `secure_client_service_rsa.c` into image3. This flag is **not set** by default, so other
  examples compile a smaller image3 without the RSA client service.

# Supported IC

RTL8721F
RTL8720F
