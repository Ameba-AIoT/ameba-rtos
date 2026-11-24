#!/usr/bin/env python3
"""Host side of the SWD debug authentication of the bootloader (boot_swd_auth.c).

Private key + the SWDAUTH line printed on the log uart -> SWD key. Give its REF0..3 line to jlink.py -t swd_key --key.

The private key file is never written anywhere by this tool. Keep it out of the repository.

The SWD key is encrypted to the debug public key with HPKE, RFC 9180 Base mode: DHKEM(P-256, HKDF-SHA256), HKDF-SHA256, AES-128-GCM,
empty info and aad. The line is "SWDAUTH hpke <hex>", the hex being enc (0x04 || X || Y) || cipher || tag, 97 bytes.
"""
import argparse
import re
import sys

from cryptography.exceptions import InvalidTag
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.hpke import AEAD, KDF, KEM, Suite

SWD_KEY_LEN = 16
HPKE_LEN = 65 + SWD_KEY_LEN + 16
SUITE = Suite(KEM.P256, KDF.HKDF_SHA256, AEAD.AES_128_GCM)


def ref_words(swd_key):
    """The SWD key as the four words boot_swd_auth.c loads into SWD_REF0..3: little endian per 4 bytes. This is also the form
    jlink.py -t swd_key wants (REF0 first, each word printed as 8 hex digits, written to AP 0x04..0x10 as is)."""
    if len(swd_key) != SWD_KEY_LEN:
        raise ValueError("SWD key must be %d bytes" % SWD_KEY_LEN)
    return [int.from_bytes(swd_key[4 * i:4 * i + 4], "little") for i in range(4)]


def parse_line(text):
    """The last "SWDAUTH hpke <hex>" line of the log, as bytes."""
    found = None
    for line in text.splitlines():
        m = re.search(r"SWDAUTH hpke ([0-9a-fA-F]+)", line)
        if m:
            found = m.group(1)
    if found is None:
        raise ValueError("no SWDAUTH hpke line found")
    try:
        data = bytes.fromhex(found)
    except ValueError:
        raise ValueError("SWDAUTH hpke is not hex")
    if len(data) != HPKE_LEN:
        raise ValueError("SWDAUTH hpke must be %d bytes, got %d" % (HPKE_LEN, len(data)))
    return data


def load_private_key(path):
    return serialization.load_pem_private_key(open(path, "rb").read(), password=None)


def decrypt(priv, data):
    try:
        return SUITE.decrypt(data, priv)
    except (InvalidTag, ValueError):
        raise ValueError("decrypt failed: wrong private key or damaged line")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--priv-key", required=True, help="PEM private key of the debug key pair (never stored by this tool)")
    ap.add_argument("--log", default="-", help="file with the uart log, '-' for stdin")
    a = ap.parse_args()

    try:
        text = sys.stdin.read() if a.log == "-" else open(a.log, errors="replace").read()
        swd_key = decrypt(load_private_key(a.priv_key), parse_line(text))
    except (ValueError, OSError) as ex:
        print("swd_auth_host error: %s" % ex, file=sys.stderr)
        return 1
    print("SWD key  " + swd_key.hex())
    print("REF0..3  " + " ".join("%08x" % w for w in ref_words(swd_key)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
