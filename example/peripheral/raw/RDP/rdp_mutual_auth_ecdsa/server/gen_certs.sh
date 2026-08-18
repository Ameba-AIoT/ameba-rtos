#!/usr/bin/env bash
# gen_certs.sh -- Generate self-signed CA + server + client ECDSA P-256 certificates
#
# Usage:
#   chmod +x gen_certs.sh
#   SERVER_IP=<server WiFi IP, e.g. 10.194.99.50> ./gen_certs.sh
#
# Output:
#   ca.key / ca.crt          -- Root CA (keep ca.key offline, distribute ca.crt)
#   server.key / server.crt  -- Server cert (deploy both to server)
#   client.key / client.crt  -- Client cert (extract d and crt for firmware)
#   client_d_array.txt       -- Raw d as C array -> paste into secure_ecdsa_client_service.c
#   client_crt_pem.txt       -- client.crt PEM   -> paste into secure_ecdsa_client_service.c
#   ca_crt_pem.txt           -- ca.crt PEM        -> paste into example_rdp_mutual_auth_ecdsa.c

set -e

SERVER_IP="${SERVER_IP:-10.194.99.50}"
echo "Generating ECDSA P-256 certificate suite for SERVER_IP=${SERVER_IP}"

# --------------------------------------------------------------------------
# 1. Root CA
# --------------------------------------------------------------------------
openssl ecparam -name prime256v1 -genkey -noout -out ca.key
openssl req -x509 -new -nodes -key ca.key -sha256 -days 3650 \
    -subj "/O=RTK Demo/CN=RTK Demo Root CA" -out ca.crt
echo "CA  : ca.key / ca.crt"

# --------------------------------------------------------------------------
# 2. Server certificate  (SAN must contain the WiFi IP the board connects to)
# --------------------------------------------------------------------------
openssl ecparam -name prime256v1 -genkey -noout -out server.key
openssl req -new -key server.key \
    -subj "/O=RTK Demo/CN=${SERVER_IP}" -out server.csr
cat > server.ext <<EOF
subjectAltName=IP:${SERVER_IP}
extendedKeyUsage=serverAuth
EOF
openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial \
    -days 825 -sha256 -extfile server.ext -out server.crt
echo "SRV : server.key / server.crt  (SAN=IP:${SERVER_IP})"

# --------------------------------------------------------------------------
# 3. Client certificate (private key d will be embedded in image3 / S-World)
# --------------------------------------------------------------------------
openssl ecparam -name prime256v1 -genkey -noout -out client.key
openssl req -new -key client.key \
    -subj "/O=RTK Demo/CN=RTK Board Client" -out client.csr
cat > client.ext <<EOF
extendedKeyUsage=clientAuth
EOF
openssl x509 -req -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial \
    -days 825 -sha256 -extfile client.ext -out client.crt
echo "CLI : client.key / client.crt"

# --------------------------------------------------------------------------
# 4. Extract client private key d as C byte array
#    Requires: python3 + cryptography  (pip3 install cryptography)
# --------------------------------------------------------------------------
python3 - <<'PYEOF' > client_d_array.txt
import sys
try:
    from cryptography.hazmat.primitives.serialization import load_pem_private_key
    key = load_pem_private_key(open("client.key", "rb").read(), None)
    d = key.private_numbers().private_value.to_bytes(32, "big")
    print("/* client ECDSA P-256 private key d (32 bytes, big-endian) */")
    print("static const unsigned char s_cli_d[32] = {")
    # 8 bytes per line
    for i in range(0, 32, 8):
        row = ", ".join(f"0x{b:02x}" for b in d[i:i+8])
        comma = "," if i + 8 < 32 else ""
        print(f"    {row}{comma}")
    print("};")
except ImportError:
    print("/* ERROR: pip3 install cryptography  then re-run */")
    sys.exit(1)
PYEOF
echo "D   : client_d_array.txt   <- paste into secure_ecdsa_client_service.c"

# --------------------------------------------------------------------------
# 5. Format client.crt PEM as C string literal
# --------------------------------------------------------------------------
python3 - <<'PYEOF' > client_crt_pem.txt
lines = [l for l in open("client.crt").read().splitlines() if l]
print("/* client.crt PEM -- public info, safe to return to NS side */")
print("static const char s_cli_cert_pem[] =")
for line in lines:
    print(f'    "{line}\\r\\n"')
print(";")
PYEOF
echo "CRT : client_crt_pem.txt   <- paste into secure_ecdsa_client_service.c"

# --------------------------------------------------------------------------
# 6. Format ca.crt PEM as C string literal
# --------------------------------------------------------------------------
python3 - <<'PYEOF' > ca_crt_pem.txt
lines = [l for l in open("ca.crt").read().splitlines() if l]
print("/* ca.crt PEM -- trust anchor; hardcode in NS-side example */")
print("static const char ca_crt_pem[] =")
for line in lines:
    print(f'    "{line}\\r\\n"')
print(";")
PYEOF
echo "CA  : ca_crt_pem.txt       <- paste into example_rdp_mutual_auth_ecdsa.c"

# --------------------------------------------------------------------------
# 7. Verify chain
# --------------------------------------------------------------------------
echo ""
echo "=== Chain verification ==="
openssl verify -CAfile ca.crt server.crt && echo "server.crt : OK"
openssl verify -CAfile ca.crt client.crt && echo "client.crt : OK"

echo ""
echo "=== Deploy checklist ==="
echo "  Server machine  : copy server.key server.crt ca.crt -> same dir as ecdsa_mtls_server.py"
echo "  Firmware S-side : paste client_d_array.txt -> secure_ecdsa_client_service.c (s_cli_d)"
echo "                  : paste client_crt_pem.txt  -> secure_ecdsa_client_service.c (s_cli_cert_pem)"
echo "  Firmware NS-side: paste ca_crt_pem.txt      -> example_rdp_mutual_auth_ecdsa.c (ca_crt_pem)"
echo "  SERVER_HOST     : set to ${SERVER_IP} in example_rdp_mutual_auth_ecdsa.c"

# Cleanup temp files
rm -f server.csr server.ext client.csr client.ext ca.srl
echo ""
echo "Done."
