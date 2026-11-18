"""swd_auth_host.py and the certificate the packer writes. No board, no ameba_pytest plugin.

  cd tools/scripts/jlink_script/tests && python -m pytest -c pytest.ini --rootdir .
  add --swd-cert=<app_cert.bin> --swd-pub=<X||Y hex> for the certificate cases (a build with swd_auth_public_key)

The debug public key (secp256r1 X||Y big-endian, 64 bytes) takes the last SWD_AUTH_PUBKEY_LEN bytes of the certificate area (boot_swd_auth.h).
"""

import hashlib
import hmac
import os
import subprocess
import sys

import pytest
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives.ciphers.aead import AESGCM

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..'))
import swd_auth_host as host  # noqa: E402

KEYID_SWD_AUTH = 0x53574441
PUBKEY_LEN = 64
ENTRY_SIZE = 4 + 32
KEY = bytes(range(host.SWD_KEY_LEN))


@pytest.fixture(scope="module")
def key_pair():
    priv = ec.generate_private_key(ec.SECP256R1())
    n = priv.public_key().public_numbers()
    return priv, n.x.to_bytes(32, 'big') + n.y.to_bytes(32, 'big')


def test_ref_word_order():
    """REF0 is the first 4 bytes of the SWD key read little endian, which is what the J-Link script writes to AP 0x04."""
    assert host.ref_words(bytes(range(16))) == [0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C]


def test_ref_word_rejects_wrong_length():
    with pytest.raises(ValueError):
        host.ref_words(bytes(15))


def _hmac(key, msg):
    return hmac.new(key, msg, hashlib.sha256).digest()


def _labeled_extract(suite, salt, label, ikm):
    return _hmac(salt or bytes(32), b"HPKE-v1" + suite + label + ikm)


def _labeled_expand(suite, prk, label, info, n):
    return _hmac(prk, n.to_bytes(2, "big") + b"HPKE-v1" + suite + label + info + b"\x01")[:n]


def _hpke_key_nonce(dh, enc, pk_r, info):
    """RFC 9180 section 4.1 and 5.1, Base mode, DHKEM(P-256, HKDF-SHA256), HKDF-SHA256, AES-128-GCM, written with HMAC only like the firmware."""
    kem, suite = b"KEM\x00\x10", b"HPKE\x00\x10\x00\x01\x00\x01"
    shared = _labeled_expand(kem, _labeled_extract(kem, b"", b"eae_prk", dh), b"shared_secret", enc + pk_r, 32)
    ctx = b"\x00" + _labeled_extract(suite, b"", b"psk_id_hash", b"") + _labeled_extract(suite, b"", b"info_hash", info)
    secret = _labeled_extract(suite, shared, b"secret", b"")
    return _labeled_expand(suite, secret, b"key", ctx, 16), _labeled_expand(suite, secret, b"base_nonce", ctx, 12)


def test_rfc9180_a3_1_vector():
    """The key schedule the firmware implements, against the official vector (RFC 9180 A.3.1, Base mode)."""
    sk_e = ec.derive_private_key(int("4995788ef4b9d6132b249ce59a77281493eb39af373d236a1fe415cb0c2d7beb", 16), ec.SECP256R1())
    pk_r = bytes.fromhex("04fe8c19ce0905191ebc298a9245792531f26f0cece2460639e8bc39cb7f706a826a779b4cf969b8a0e539c7f62fb3d30ad6aa8f80e30f1d128aafd68a2ce72ea0")
    enc = bytes.fromhex("04a92719c6195d5085104f469a8b9814d5838ff72b60501e2c4466e5e67b325ac98536d7b61a1af4b78e5b7f951c0900be863c403ce65c9bfcb9382657222d18c4")
    peer = ec.EllipticCurvePublicNumbers(int.from_bytes(pk_r[1:33], "big"), int.from_bytes(pk_r[33:], "big"), ec.SECP256R1()).public_key()
    key, nonce = _hpke_key_nonce(sk_e.exchange(ec.ECDH(), peer), enc, pk_r, bytes.fromhex("4f6465206f6e2061204772656369616e2055726e"))
    assert key.hex() == "868c066ef58aae6dc589b6cfdd18f97e" and nonce.hex() == "4e0bc5018beba4bf004cca59"
    pt = bytes.fromhex("4265617574792069732074727574682c20747275746820626561757479")
    assert AESGCM(key).encrypt(nonce, pt, bytes.fromhex("436f756e742d30")).hex() == (
        "5ad590bb8baa577f8619db35a36311226a896e7342a6d836d8b7bcd2f20b6c7f9076ac232e3ab2523f39513434")


def test_library_decrypts_the_firmware_scheme(key_pair):
    """What the firmware makes (empty info and aad) is what the library decrypts."""
    priv, pub = key_pair
    eph = ec.generate_private_key(ec.SECP256R1())
    n = eph.public_key().public_numbers()
    enc = b"\x04" + n.x.to_bytes(32, "big") + n.y.to_bytes(32, "big")
    key, nonce = _hpke_key_nonce(eph.exchange(ec.ECDH(), priv.public_key()), enc, b"\x04" + pub, b"")
    assert host.decrypt(priv, enc + AESGCM(key).encrypt(nonce, KEY, b"")) == KEY


def test_roundtrip(key_pair):
    priv, _ = key_pair
    assert host.decrypt(priv, host.SUITE.encrypt(KEY, priv.public_key())) == KEY


def test_wrong_private_key_is_rejected(key_pair):
    priv, _ = key_pair
    with pytest.raises(ValueError, match="decrypt failed"):
        host.decrypt(ec.generate_private_key(ec.SECP256R1()), host.SUITE.encrypt(KEY, priv.public_key()))


@pytest.mark.parametrize("pos", [10, 65, -1], ids=["enc", "cipher", "tag"])
def test_a_flipped_bit_is_rejected(key_pair, pos):
    priv, _ = key_pair
    data = bytearray(host.SUITE.encrypt(KEY, priv.public_key()))
    data[pos] ^= 1
    with pytest.raises(ValueError, match="decrypt failed"):
        host.decrypt(priv, bytes(data))


def test_parse_takes_the_last_line():
    assert host.parse_line("SWDAUTH hpke " + "11" * host.HPKE_LEN + "\nboot again\nSWDAUTH hpke " + "55" * host.HPKE_LEN) == b"\x55" * host.HPKE_LEN


def test_parse_rejects_a_missing_or_short_line():
    for text in ("no line here", "SWDAUTH hpke " + "11" * (host.HPKE_LEN - 1), "SWDAUTH hpke zz"):
        with pytest.raises(ValueError):
            host.parse_line(text)


@pytest.mark.parametrize("key", [
    "11111111 22222222 33333333 4444444",       # one digit short
    "zz111111 22222222 33333333 44444444",      # not hex
    "11111111 22222222 33333333 444444444",     # one digit too many
    "",                                         # no key at all
])
def test_jlink_rejects_a_bad_key_before_touching_anything(key):
    """jlink.py -t swd_key checks the key first: exit 1 and no script, so nothing reaches the J-Link."""
    cmd = [sys.executable, os.path.join(HERE, '..', 'jlink.py'), '--chip', 'RLE1509', '-t', 'swd_key']
    out = subprocess.run(cmd + (['--key'] + key.split() if key else []), capture_output=True, text=True)
    assert out.returncode == 1 and 'Jlink Error' in out.stdout, out.stdout + out.stderr
    assert not os.path.exists(os.path.join(HERE, '..', 'AutoGen_SWD_Key.JLinkScript'))


@pytest.fixture(scope="module")
def cert(request):
    path, pub = request.config.getoption("--swd-cert"), request.config.getoption("--swd-pub")
    if not path or not pub:
        pytest.skip("needs --swd-cert and --swd-pub")
    data = open(path, 'rb').read()
    assert len(data) in (0x1000, 0x2000), f"unexpected certificate size {len(data)}"
    return data, bytes.fromhex(pub)


def test_cert_has_one_swd_entry(cert):
    """Once, and in the part the signature covers (the public key at the end is not)."""
    data, _ = cert
    assert data.count(KEYID_SWD_AUTH.to_bytes(4, 'little'), 0, len(data) - PUBKEY_LEN) == 1


def test_cert_entry_hash_is_sha256_of_the_key(cert):
    """SBOOT_Validate_PubKey hashes exactly the 64 bytes X||Y."""
    data, pub = cert
    i = data.index(KEYID_SWD_AUTH.to_bytes(4, 'little'))
    assert data[i + 4:i + ENTRY_SIZE] == hashlib.sha256(pub).digest()


def test_cert_public_key_is_at_the_end(cert):
    data, pub = cert
    assert data[-PUBKEY_LEN:] == pub
