#!/usr/bin/env python3
"""
ECDSA mutual-TLS demo server.

Prerequisites:
  - Run gen_certs.sh first to produce server.crt / server.key / ca.crt
  - Copy those three files to the same directory as this script
  - python3 >= 3.7 (stdlib ssl only, no third-party packages needed)

Usage:
  cd <dir containing server.crt server.key ca.crt>
  python3 ecdsa_mtls_server.py [--host 0.0.0.0] [--port 8443]

Cipher forced: ECDHE-ECDSA-AES128-GCM-SHA256
TLS version  : 1.2 only (matches board mbedtls config)
Client auth  : CERT_REQUIRED (mutual TLS)
"""

import argparse
import socket
import ssl
import sys

DEFAULT_HOST = "0.0.0.0"
DEFAULT_PORT = 8443


def build_ssl_context(certfile, keyfile, cafile):
    ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)

    ctx.load_cert_chain(certfile=certfile, keyfile=keyfile)
    ctx.load_verify_locations(cafile=cafile)

    # Require client certificate -> mutual TLS
    ctx.verify_mode = ssl.CERT_REQUIRED

    # Pin to TLS 1.2 to match board mbedtls default
    ctx.minimum_version = ssl.TLSVersion.TLSv1_2
    ctx.maximum_version = ssl.TLSVersion.TLSv1_2

    # Force ECDHE-ECDSA-AES128-GCM-SHA256
    ctx.set_ciphers("ECDHE-ECDSA-AES128-GCM-SHA256")

    return ctx


def handle_client(tls_sock, addr):
    print(f"\n[+] Connection from {addr}")
    try:
        peer_cert = tls_sock.getpeercert()
        subject = dict(x[0] for x in peer_cert.get("subject", []))
        print(f"    Client CN  : {subject.get('commonName', '(none)')}")
        print(f"    Cipher     : {tls_sock.cipher()}")
        print(f"    TLS version: {tls_sock.version()}")

        data = tls_sock.recv(2048)
        if data:
            first_line = data.split(b"\r\n")[0].decode(errors="replace")
            print(f"    Request    : {first_line}")

        body = b"Hello from ECDSA mTLS server\r\nMutual auth OK!\r\n"
        response = (
            b"HTTP/1.1 200 OK\r\n"
            b"Content-Type: text/plain\r\n"
            b"Content-Length: " + str(len(body)).encode() + b"\r\n"
            b"Connection: close\r\n"
            b"\r\n" + body
        )
        tls_sock.sendall(response)
        print(f"    Sent {len(response)} bytes -- mutual auth PASS")

    except ssl.SSLError as e:
        # Client cert missing / untrusted / signature wrong -> expected negative path
        print(f"    [!] TLS error (expected for negative test): {e}")
    except Exception as e:
        print(f"    [!] Error: {e}")
    finally:
        tls_sock.close()


def main():
    parser = argparse.ArgumentParser(description="ECDSA mTLS demo server")
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int, default=DEFAULT_PORT)
    parser.add_argument("--cert",   default="server.crt")
    parser.add_argument("--key",    default="server.key")
    parser.add_argument("--ca",     default="ca.crt")
    args = parser.parse_args()

    for f in (args.cert, args.key, args.ca):
        try:
            open(f)
        except FileNotFoundError:
            print(f"ERROR: {f} not found. Run gen_certs.sh first.", file=sys.stderr)
            sys.exit(1)

    ctx = build_ssl_context(args.cert, args.key, args.ca)

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as srv:
        srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        srv.bind((args.host, args.port))
        srv.listen(5)
        print(f"ECDSA mTLS server listening on {args.host}:{args.port}")
        print(f"  cipher : ECDHE-ECDSA-AES128-GCM-SHA256")
        print(f"  client : CERT_REQUIRED (mutual TLS)")
        print(f"  CA     : {args.ca}")
        print("Ctrl-C to stop.\n")

        while True:
            try:
                raw, addr = srv.accept()
            except KeyboardInterrupt:
                print("\nServer stopped.")
                break

            try:
                tls = ctx.wrap_socket(raw, server_side=True)
                handle_client(tls, addr)
            except ssl.SSLError as e:
                print(f"[!] Handshake failed from {addr}: {e}")
                raw.close()


if __name__ == "__main__":
    main()
