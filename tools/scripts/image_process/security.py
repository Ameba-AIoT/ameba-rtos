#!/usr/bin/env python3
# -*- coding:utf-8 -*-
from ctypes import *
from enum import Enum, unique
import array
import struct
import json
import binascii
import sys
import os
import hashlib
import hmac

from manifest_manager import ManifestImageConfig

def check_python_lib(lib):
    print('%s Python library is not installed.'%(lib))
    sys.exit(-1)

try:
    import cryptography
except:
    check_python_lib('cryptography')

try:
    import Crypto
except:
    check_python_lib('Crypto')

try:
    import sslcrypto
except:
    check_python_lib('sslcrypto')

try:
    import ecdsa
except:
    check_python_lib('ecdsa')

from cryptography.hazmat.primitives.asymmetric import ed25519, ec
from cryptography.hazmat.primitives.asymmetric.utils import decode_dss_signature
from cryptography.hazmat.primitives import serialization, hashes
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend
from Crypto.Cipher import AES


@unique
class Curve(Enum):
    SECP192R1 = 'secp192r1'
    SECP224R1 = 'secp224r1'
    SECP256R1 = 'secp256r1'
    SECP384R1 = 'secp384r1'
    SECP521R1 = 'secp521r1'
    BRAINPOOLP256R1 = 'brainpoolP256r1'
    BRAINPOOLP384R1 = 'brainpoolP384r1'
    BRAINPOOLP512R1 = 'brainpoolP512r1'
    SECP192K1 = 'secp192k1'
    SECP224K1 = 'secp224k1'
    SECP256K1 = 'secp256k1'
    CURVE25519 = 'x25519'
    CURVE448 = 'x448'


_CRYPTO_CURVE_MAP = {
    Curve.SECP192R1: ec.SECP192R1,
    Curve.SECP224R1: ec.SECP224R1,
    Curve.SECP256R1: ec.SECP256R1,
    Curve.SECP384R1: ec.SECP384R1,
    Curve.SECP521R1: ec.SECP521R1,
    Curve.BRAINPOOLP256R1: ec.BrainpoolP256R1,
    Curve.BRAINPOOLP384R1: ec.BrainpoolP384R1,
    Curve.BRAINPOOLP512R1: ec.BrainpoolP512R1,
    Curve.SECP256K1: ec.SECP256K1,
}

_SSLCRYPTO_CURVE_NAMES = {
    Curve.SECP192K1: 'secp192k1',
    Curve.SECP224K1: 'secp224k1',
}


def _hash_alg_for(name):
    n = name.lower()
    if n == 'sha256':
        return hashes.SHA256()
    if n == 'sha384':
        return hashes.SHA384()
    if n == 'sha512':
        return hashes.SHA512()
    raise ValueError('Unsupported hash algorithm: %s' % name)


_SECP224K1_N = 0x10000000000000000000000000001DCE8D2EC6184CAF0A971769FB1F7
_SECP224K1_P = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFE56D
_SECP224K1_GX = 0xA1455B334DF099DF30FC28A169A467E9E47075A90F7E650EB6B7A45C
_SECP224K1_GY = 0x7E089FED7FBA344282CAFBD6F7E319F7C0B0BD59E2CA4BDB556D61A5


def _build_secp224k1_ecdsa_curve():
    from ecdsa.ellipticcurve import CurveFp, PointJacobi
    from ecdsa import curves as _ecdsa_curves
    crv = CurveFp(_SECP224K1_P, 0, 5)
    g = PointJacobi(crv, _SECP224K1_GX, _SECP224K1_GY, 1, _SECP224K1_N, generator=False)
    return _ecdsa_curves.Curve("secp224k1", crv, g, (1, 3, 132, 0, 32))

ciL = 8
biL = ciL << 3
SIGN_MAX_LEN = 144
PKEY_MAX_LEN = 144
HASH_MAX_LEN = 64

def init_list(length):
    return [0] * length

def list_to_hex_str(lst):
    hex_data = ['%02X' % int(i) for i in lst]
    hex_str = ''
    # print(hex_data)
    for item in hex_data:
        hex_str += str(item)
    return hex_str

def list_to_bytes(lst):
    return array.array('B', lst).tobytes()

def htonl_to_ntohl(value):
    # big-endian to little-endian
    packed_value = struct.pack('>I', value)
    return struct.unpack('<I', packed_value)[0]

def GET_BYTE( X, i ):
    return (X[i // ciL] >> ((i % ciL) * ciL)) & 0xff

def CHARS_TO_LIMBS(i):
    return (i // ciL) + (i % ciL != 0)

def BITS_TO_LIMBS(i):
    return (i // biL) + (i % biL != 0)

def mpi_uint_bigendian_to_host(x):
    tmp = 0
    for i in range(0, ciL):
        x = x >> 8
        tmp |= (x & 0xFF) << ((ciL - 1 - i) << 3)
    return tmp

def mbedtls_mpi_write_binary(X, buf, buflen):
    stored_bytes = len(X) * ciL
    # print("buflen: %d, stored_bytes: %d"%(buflen, stored_bytes))
    # print(buf)
    offset = 0
    if stored_bytes < buflen:
        bytes_to_copy = stored_bytes
        offset = buflen - stored_bytes
    else:
        bytes_to_copy = buflen
        offset = 0
        for i in range(bytes_to_copy, stored_bytes):
            if GET_BYTE(X, i) != 0:
                return -1
    for i in range(0, bytes_to_copy):
        buf[offset + bytes_to_copy - i - 1] = GET_BYTE(X, i)
    # print(buf)
    return 0

def mbedtls_mpi_read_binary(buf, buflen):
    limbs = CHARS_TO_LIMBS(buflen)
    overhead = (limbs * ciL) - buflen
    hex_str = ''
    for item in buf:
        hex_str += str(item)
    # hex_str = '00'*overhead + hex_str
    num = int(hex_str, 16)
    p = init_list(overhead + limbs)
    for i in range(0, limbs):
        p[i] = (num >> (i * 64)) & 0xFFFFFFFFFFFFFFFF

    # for i in range(overhead, overhead + buflen):
    #     p[i] = int(buf[i - overhead], 16)

    # cur_limb_left = p[0]
    # cur_limb_right = p[limbs - 1]

    # i = 0
    # j = limbs - 1
    # print(limbs)

    # while(i <= j):
    #     tmp = mpi_uint_bigendian_to_host(cur_limb_left)
    #     p[i] = mpi_uint_bigendian_to_host(cur_limb_right)
    #     p[j] = tmp
    #     i += 1
    #     j -= 1
    #     cur_limb_left = p[i]
    #     cur_limb_right = p[j]
    d = 0
    for i in range(0, limbs):
        d |= p[i] << (64 * i)

    return d

def point_to_bignum_arr(X, plen, is_pub):
    m = plen // ciL + (plen % ciL != 0)

    X_arr = []
    for i in range(0, m):
        val = (int(X) >> i * 64) & 0xFFFFFFFFFFFFFFFF
        X_arr.append(val)
    if is_pub:
        for _ in range(0, m):
            X_arr.append(0)

    return X_arr

def mbedtls_ecp_point_write_binary(X, Y, buf, buflen, plen):
    # plen = ecc.key_size
    if buflen < (2 * plen + 1):
        return -1

    X_arr = point_to_bignum_arr(X, plen, 1)
    Y_arr = point_to_bignum_arr(Y, plen, 1)
    buf[0] = 0x04
    buf_x = init_list(plen)
    mbedtls_mpi_write_binary(X_arr, buf_x, plen)
    buf_y = init_list(plen)
    mbedtls_mpi_write_binary(Y_arr, buf_y, plen)

    for i in range(1, plen+1):
        buf[i] = buf_x[i - 1]

    for i in range(plen + 1, buflen):
        buf[i] = buf_y[i - plen - 1]
    return 0

def str_idx_as_int(string, index):
    """Take index'th byte from string, return as integer"""
    val = string[index]
    if isinstance(val, int):
        return val
    return ord(val)

def encode_integer(r):
    assert r >= 0  # can't support negative numbers yet
    h = ("%x" % r).encode()
    if len(h) % 2:
        h = b"0" + h
    s = binascii.unhexlify(h)
    num = str_idx_as_int(s, 0)
    if num <= 0x7F:
        return s
    else:
        # DER integers are two's complement, so if the first byte is
        # 0x80-0xff then we need an extra 0x00 byte to prevent it from
        # looking negative.
        return s
def encode_sequence(*encoded_pieces):
    total_len = sum([len(p) for p in encoded_pieces])
    return b"\x30" + b'h' +b"".join(encoded_pieces)
def mbedtls_pk_binary_to_der(privkey, pubkey):
    a = int(privkey, 16)
    b = int(pubkey, 16)
    decode_der = encode_sequence(ecdsa.der.encode_integer(1),
                                 b"\x04\x1c",
                                 encode_integer(a),
                                 b'\xa0\x07\x06\x05+\x81\x04\x00 \xa1<\x03:\x00\x04',
                                 encode_integer(b))
    return decode_der

@unique
class Sec_AuthAlg(Enum):
    Sec_AuthID_ED25519 = 0
    Sec_AuthID_MLDSA65 = 1

@unique
class mbedtls_ecp_group_id(Enum):
    MBEDTLS_ECP_DP_NONE = 0       # Curve not defined. */
    MBEDTLS_ECP_DP_SECP192R1 = 1      # Domain parameters for the 192-bit curve defined by FIPS 186-4 and SEC1. */
    MBEDTLS_ECP_DP_SECP224R1 = 2      # Domain parameters for the 224-bit curve defined by FIPS 186-4 and SEC1. */
    MBEDTLS_ECP_DP_SECP256R1 = 3      # Domain parameters for the 256-bit curve defined by FIPS 186-4 and SEC1. */
    MBEDTLS_ECP_DP_SECP384R1 = 4      # Domain parameters for the 384-bit curve defined by FIPS 186-4 and SEC1. */
    MBEDTLS_ECP_DP_SECP521R1 = 5      # Domain parameters for the 521-bit curve defined by FIPS 186-4 and SEC1. */
    MBEDTLS_ECP_DP_BP256R1 = 6        # Domain parameters for 256-bit Brainpool curve. */
    MBEDTLS_ECP_DP_BP384R1 = 7        # Domain parameters for 384-bit Brainpool curve. */
    MBEDTLS_ECP_DP_BP512R1 = 8        # Domain parameters for 512-bit Brainpool curve. */
    MBEDTLS_ECP_DP_CURVE25519 = 9     # Domain parameters for Curve25519. */
    MBEDTLS_ECP_DP_SECP192K1 = 10      # Domain parameters for 192-bit "Koblitz" curve. */
    MBEDTLS_ECP_DP_SECP224K1 = 11      # Domain parameters for 224-bit "Koblitz" curve. */
    MBEDTLS_ECP_DP_SECP256K1 = 12      # Domain parameters for 256-bit "Koblitz" curve. */
    MBEDTLS_ECP_DP_CURVE448 = 13       # Domain parameters for Curve448. */

@unique
class HashID(Enum):
    HashID_SHA256 = 0
    HashID_SHA384 = 1
    HashID_SHA512 = 2
    HashID_HMAC256 = 3
    HashID_HMAC384 = 4
    HashID_HMAC512 = 5

class secure_boot():
    # valid input for gen_auth_id
    valid_algorithm = [name.split('_')[-1].lower() for name in mbedtls_ecp_group_id.__members__] + ["ed25519"] + ["ml_dsa_65"]

    def __init__(self):
        self.PrivKey = ''
        self.IsHMAC = 0
        self.HmacKey = ''
        self.HmacKeyLen = 0
        self.SecureBootEn = 0
        self.MdType = ''

    def get_supported_curve(self, auth_alg):
        name = mbedtls_ecp_group_id(auth_alg).name.split('_')[-1]
        if hasattr(Curve, name):
            return getattr(Curve, name)
        else:
            if auth_alg == mbedtls_ecp_group_id.MBEDTLS_ECP_DP_BP256R1.value:
                return Curve.BRAINPOOLP256R1
            elif auth_alg == mbedtls_ecp_group_id.MBEDTLS_ECP_DP_BP384R1.value:
                return Curve.BRAINPOOLP384R1
            elif auth_alg == mbedtls_ecp_group_id.MBEDTLS_ECP_DP_BP512R1.value:
                return Curve.BRAINPOOLP512R1
            else:
                print("Not supported AUTH_ALG!")
                return -1

    def gen_auth_id(self, strs:str):
        if strs == "ed25519":
            return Sec_AuthAlg.Sec_AuthID_ED25519.value
        elif strs == "ml_dsa_65":
            return Sec_AuthAlg.Sec_AuthID_MLDSA65.value
        else:
            name = f"MBEDTLS_ECP_DP_{strs.split('_')[-1].upper()}"
            if hasattr(mbedtls_ecp_group_id, name):
                return getattr(mbedtls_ecp_group_id, name).value
            else:
                print("Not supported AUTH_ALG!")
                return -1

    def gen_hash_id(self, manifest, strs):
        if strs == "hmac256":
            manifest.HashAlg = HashID.HashID_HMAC256.value
            self.MdType = 'sha256'
            self.IsHMAC = 1
        elif strs == "hmac384":
            manifest.HashAlg = HashID.HashID_HMAC384.value
            self.MdType = 'sha384'
            self.IsHMAC = 1
        elif strs == "hmac512":
            manifest.HashAlg = HashID.HashID_HMAC512.value
            self.MdType = 'sha512'
            self.IsHMAC = 1
        elif strs == "sha256":
            manifest.HashAlg = HashID.HashID_SHA256.value
            self.MdType = 'sha256'
        elif strs == "sha384":
            manifest.HashAlg = HashID.HashID_SHA384.value
            self.MdType = 'sha384'
        elif strs == "sha512":
            manifest.HashAlg = HashID.HashID_SHA512.value
            self.MdType = 'sha512'
        else:
            print("Not supported HASH_ALG!\n")
            return -1
        return 0

    def ed25519_genkey(self, keyinfo):
        privkey = ed25519.Ed25519PrivateKey.generate()
        pubkey = privkey.public_key()

        # generate public key and private key
        privkey_bytes = privkey.private_bytes(
            serialization.Encoding.Raw,
            serialization.PrivateFormat.Raw,
            serialization.NoEncryption()
        )
        pubkey_bytes = pubkey.public_bytes(
            serialization.Encoding.Raw,
            serialization.PublicFormat.Raw
        )
        # generate public key hash
        hash = hashlib.sha256()
        hash.update(pubkey_bytes)
        pubkey_hash = hash.hexdigest().upper()
        # print(pubkey_hash)

        privkey_hex = list_to_hex_str(privkey_bytes)
        pubkey_hex = list_to_hex_str(pubkey_bytes)

        keyinfo['sboot_private_key'] = privkey_hex
        keyinfo['sboot_public_key'] = pubkey_hex
        keyinfo['sboot_public_key_hash'] = pubkey_hash
        return

    def ml_dsa_65_genkey(self, keyinfo):
        try:
            import pqcrypto.sign.ml_dsa_65 as ml_dsa_65
        except ImportError:
            print("ML-DSA-65 module not available. Please install pqcrypto.")
            return -1

        # Generate ML-DSA-65 keypair
        public_key, secret_key = ml_dsa_65.generate_keypair()

        # generate public key hash
        hash = hashlib.sha256()
        hash.update(public_key)
        pubkey_hash = hash.hexdigest().upper()

        # Convert to hex strings
        privkey_hex = list_to_hex_str(secret_key)
        pubkey_hex = list_to_hex_str(public_key)

        keyinfo['sboot_pqc_private_key'] = privkey_hex
        keyinfo['sboot_pqc_public_key'] = pubkey_hex
        keyinfo['sboot_pqc_public_key_hash'] = pubkey_hash
        return 0

    def ml_dsa_65_sign(self, privkey_hex, msg, mlen, sig):
        """Generate ML-DSA-65 signature
        Args:
            privkey_hex: Private key in hex string format
            msg: Message bytes to sign
            mlen: Message length
            sig: Signature buffer to fill
        Returns:
            0 on success, -1 on failure
        """
        try:
            import pqcrypto.sign.ml_dsa_65 as ml_dsa_65
        except ImportError:
            print("ML-DSA-65 module not available. Please install pqcrypto.")
            return -1

        try:
            # Convert private key from hex to bytes
            privkey_bytes = bytes.fromhex(privkey_hex)

            # Convert message to bytes if it's not already
            if hasattr(msg, 'cast'):  # ctypes array
                # Convert ctypes array to bytes
                msg_bytes = bytes(msg)
            elif isinstance(msg, (list, tuple)):
                msg_bytes = bytes(msg)
            else:
                msg_bytes = msg

            # Ensure we only use the specified length
            msg_bytes = msg_bytes[:mlen]

            # Generate signature
            signature = ml_dsa_65.sign(privkey_bytes, msg_bytes)

            # Copy signature to output buffer
            sig_len = min(len(signature), len(sig))
            for i in range(sig_len):
                sig[i] = signature[i]

            print(f"ML-DSA-65 signature generated, length: {sig_len}")
            return 0

        except Exception as e:
            print(f"Error generating ML-DSA-65 signature: {e}")
            return -1

    def ed25519_sign(self, privkey, pubkey, msg, mlen, sig):
        privkey_bytes = bytes.fromhex(privkey)
        privkey = ed25519.Ed25519PrivateKey.from_private_bytes(privkey_bytes)

        privkey_bytes = privkey.private_bytes(
            serialization.Encoding.Raw,
            serialization.PrivateFormat.Raw,
            serialization.NoEncryption()
        )
        pubkey = privkey.public_key()
        pubkey_bytes = pubkey.public_bytes(
            serialization.Encoding.Raw,
            serialization.PublicFormat.Raw
        )
        # generate signature
        if isinstance(msg, bytes):
            sig_bytes = privkey.sign(msg)
        else:
            msg_bytes = string_at(addressof(msg), mlen)
            sig_bytes = privkey.sign(msg_bytes)
        # print(list_to_hex_str(sig_bytes))
        memmove(addressof(sig), sig_bytes, 64)  # Ed25519 signature is fixed 64 bytes
        return 0

    def ecdsa_sign(self, id, privkey, pubkey, msg, mlen, sig):
        # msg: manifest
        # mlen: sizeof(Manifest_TypeDef) - SIGN_MAX_LEN
        # sig: Signature[SIGN_MAX_LEN]
        # print(string_at(sig, sizeof(sig)))
        use_fastecdsa = 0
        if id == Curve.SECP192R1:
            curve = ec.SECP192R1()
        elif id == Curve.SECP224R1:
            curve = ec.SECP224R1()
        elif id == Curve.SECP256R1:
            curve = ec.SECP256R1()
        elif id == Curve.SECP384R1:
            curve = ec.SECP384R1()
        elif id == Curve.BRAINPOOLP256R1:
            curve = ec.BrainpoolP256R1()
        elif id == Curve.BRAINPOOLP384R1:
            curve = ec.BrainpoolP384R1()
        elif id == Curve.BRAINPOOLP512R1:
            curve = ec.BrainpoolP512R1()
        elif id == Curve.SECP192K1:
            curve = 'secp192k1'
            use_fastecdsa = 1
        elif id == Curve.SECP224K1:
            curve = 'secp224k1'
        elif id == Curve.SECP256K1:
            curve = ec.SECP256K1()
        else:
            print('not supported curve!')
            return -1
        if use_fastecdsa:
            csize = len(privkey) // 2
            msg_bytes = string_at(addressof(msg), mlen)
            privkey_bytes = bytes.fromhex(privkey)
            key = sslcrypto.ecc.get_curve(curve)
            if self.IsHMAC == 0:
                hash = hashlib.new(self.MdType)
                hash.update(msg_bytes)
                msg_hash = hash.hexdigest().upper()
                msg_hash_bytes = bytes.fromhex(msg_hash)
                sig_bytes = key.sign(msg_hash_bytes, privkey_bytes, hash=None)
            else:
                hmackey_bytes = bytes.fromhex(self.HmacKey)
                hash = hmac.new(hmackey_bytes, digestmod=self.MdType)
                hash.update(msg_bytes)
                msg_hash = hash.hexdigest().upper()
                msg_hash_bytes = bytes.fromhex(msg_hash)
                sig_bytes = key.sign(msg_hash_bytes, privkey_bytes, hash=None)
            # print(len(sig_bytes))
            # print(list_to_hex_str(sig_bytes))
            memmove(addressof(sig), sig_bytes, csize * 2)
        elif id == Curve.SECP224K1:
            # SECP224K1 public key is 224-bits, but signature is 225-bits.
            # cryptography lib does not support secp224k1; use the ecdsa library
            # with a custom curve definition for both HMAC and non-HMAC paths.
            from ecdsa import SigningKey as _EcdsaSignKey
            csize = (225 + 7) // 8
            msg_bytes = string_at(addressof(msg), mlen)
            _SECP224K1 = _build_secp224k1_ecdsa_curve()
            d_int = int(privkey, 16)
            _ekey = _EcdsaSignKey.from_secret_exponent(d_int, curve=_SECP224K1)
            if self.IsHMAC:
                hmackey_bytes = bytes.fromhex(self.HmacKey)
                def _hmac_fn(data, _k=hmackey_bytes, _md=self.MdType):
                    return hmac.new(_k, msg=data, digestmod=_md)
                _raw_sig = _ekey.sign(msg_bytes, hashfunc=_hmac_fn)
            else:
                def _hash_fn(data, _md=self.MdType):
                    h = hashlib.new(_md)
                    h.update(data)
                    return h
                _raw_sig = _ekey.sign(msg_bytes, hashfunc=_hash_fn)
            # ecdsa.SigningKey.sign() default sigencode_string returns r||s
            # big-endian, each padded to ceil(curve.order_bits / 8) = csize
            r = int.from_bytes(_raw_sig[:csize], 'big')
            s = int.from_bytes(_raw_sig[csize:], 'big')
            r_arr = point_to_bignum_arr(r, csize, 0)
            s_arr = point_to_bignum_arr(s, csize, 0)

            buf_x = init_list(csize)
            mbedtls_mpi_write_binary(r_arr, buf_x, csize)
            buf_y = init_list(csize)
            mbedtls_mpi_write_binary(s_arr, buf_y, csize)
            sigs = init_list(csize * 2)
            for i in range(0, csize):
                sigs[i] = buf_x[i]

            for i in range(csize, csize * 2):
                sigs[i] = buf_y[i - csize]

            # print('sigs: ', bytes(sigs).hex())
            memmove(addressof(sig), bytes(sigs), csize * 2)
        else:
            csize = (curve.key_size + 7) // 8
            d_int = int(privkey, 16)
            priv_obj = ec.derive_private_key(d_int, curve)
            msg_bytes = string_at(addressof(msg), mlen)
            if self.IsHMAC == 0:
                hash_alg = _hash_alg_for(self.MdType)
                sig_der = priv_obj.sign(msg_bytes, ec.ECDSA(hash_alg))
                r, s = decode_dss_signature(sig_der)
                r_arr = point_to_bignum_arr(r, csize, 0)
                s_arr = point_to_bignum_arr(s, csize, 0)

                buf_x = init_list(csize)
                mbedtls_mpi_write_binary(r_arr, buf_x, csize)
                buf_y = init_list(csize)
                mbedtls_mpi_write_binary(s_arr, buf_y, csize)
                sigs = init_list(csize * 2)
                for i in range(0, csize):
                    sigs[i] = buf_x[i]

                for i in range(csize, csize * 2):
                    sigs[i] = buf_y[i - csize]

                memmove(addressof(sig), bytes(sigs), csize * 2)
            else:
                hmackey_bytes = bytes.fromhex(self.HmacKey)
                # ecdsa lib supports user-supplied hashfunc for HMAC-as-hash
                pem = priv_obj.private_bytes(
                    encoding=serialization.Encoding.PEM,
                    format=serialization.PrivateFormat.PKCS8,
                    encryption_algorithm=serialization.NoEncryption())
                key = ecdsa.SigningKey.from_pem(pem.decode('utf-8'))
                def hashfunc_user(data):
                    return hmac.new(hmackey_bytes, msg=data, digestmod=self.MdType)
                sig_bytes = key.sign(msg_bytes, hashfunc=hashfunc_user)

                # print(len(sig_bytes))
                # print(list_to_hex_str(sig_bytes))
                memmove(addressof(sig), sig_bytes, csize * 2)
        return 0

    def ecdsa_genkey(self, id, keyinfo):
        # Generate raw X, Y, d big-endian bytes; csize is curve byte length.
        if id in _CRYPTO_CURVE_MAP:
            curve_cls = _CRYPTO_CURVE_MAP[id]
            curve = curve_cls()
            csize = (curve.key_size + 7) // 8
            priv_obj = ec.generate_private_key(curve)
            priv_numbers = priv_obj.private_numbers()
            pub_numbers = priv_obj.public_key().public_numbers()
            d_bytes = priv_numbers.private_value.to_bytes(csize, 'big')
            x_bytes = pub_numbers.x.to_bytes(csize, 'big')
            y_bytes = pub_numbers.y.to_bytes(csize, 'big')
            pubkey_bytes = x_bytes + y_bytes
            privkey_bytes = d_bytes
        elif id in _SSLCRYPTO_CURVE_NAMES:
            ssl_curve = sslcrypto.ecc.get_curve(_SSLCRYPTO_CURVE_NAMES[id])
            privkey_bytes = ssl_curve.new_private_key()
            full_pub = ssl_curve.private_to_public(privkey_bytes)
            # sslcrypto returns 0x04 || X || Y for uncompressed pub keys
            if len(full_pub) and full_pub[0] == 0x04:
                pubkey_bytes = full_pub[1:]
            else:
                pubkey_bytes = full_pub
            csize = len(privkey_bytes)
        else:
            print('Not supported curve for keygen!')
            return -1

        hash = hashlib.sha256()
        hash.update(pubkey_bytes)
        pubkey_hash = hash.hexdigest().upper()

        pubkey_hex = pubkey_bytes.hex().upper()
        privkey_hex = privkey_bytes.hex().upper()

        keyinfo['sboot_private_key'] = privkey_hex
        keyinfo['sboot_public_key'] = pubkey_hex
        keyinfo['sboot_public_key_hash'] = pubkey_hash
        return keyinfo

    def gen_image_hash(self, filename, imghash):
        with open(filename, 'rb') as f:
            buf = f.read(1024)
            if self.IsHMAC == 0:
                hash = hashlib.new(self.MdType)
                while buf:
                    hash.update(buf)
                    buf = f.read(1024)
                hash_hex = hash.hexdigest().upper()
            else:
                hmackey_bytes = bytes.fromhex(self.HmacKey)
                hash = hmac.new(hmackey_bytes, digestmod=self.MdType)
                while buf:
                    hash.update(buf)
                    buf = f.read(1024)
                hash_hex = hash.hexdigest().upper()
        # print(hash_hex)
        hash_bytes = bytes.fromhex(hash_hex)
        memmove(addressof(imghash), hash_bytes, hash.digest_size)
        return 0

    def gen_signature(self, auth_alg_id, privkey, pubkey, msg, mlen, sig):
        if auth_alg_id == Sec_AuthAlg.Sec_AuthID_ED25519.value:
            ret = self.ed25519_sign(privkey, pubkey, msg, mlen, sig)
        else:
            curve = self.get_supported_curve(auth_alg_id)
            ret = self.ecdsa_sign(curve, privkey, pubkey, msg, mlen, sig)
        return ret

    def tls_prf_sha256(self, secret, label, seed, DerivedKey):
        md_len = 32
        dlen = 16
        rlen = 4
        nb = len(label)
        tmp = init_list(128)
        if md_len + nb + rlen > len(tmp):
            return -1
        secret_bytes = bytes.fromhex(secret)
        label_bytes = label.encode('utf-8')
        seed_bytes = seed.to_bytes(4, 'little')
        # print(seed_bytes)

        for i in range(0, nb):
            tmp[md_len + i] = int(label_bytes[i])
        for i in range(0, rlen):
            tmp[md_len + nb + i] = int(seed_bytes[i])

        nb = nb + rlen
        # hmac for label and seed
        hmac_obj = hmac.new(secret_bytes, digestmod='sha256')
        tmp_bytes = label_bytes + seed_bytes
        hmac_obj.update(tmp_bytes)
        hash_bytes =  bytes.fromhex(hmac_obj.hexdigest().upper())

        for i in range(0, md_len):
            tmp[i] = int(hash_bytes[i])

        for i in range(0, dlen, md_len):
            hmac_obj = hmac.new(secret_bytes, digestmod='sha256')
            tmp_bytes = bytes.fromhex(list_to_hex_str(tmp[0:md_len+nb]))
            hmac_obj.update(tmp_bytes)
            h_i = bytes.fromhex(hmac_obj.hexdigest().upper())

            hmac_obj = hmac.new(secret_bytes, digestmod='sha256')
            tmp_bytes = bytes.fromhex(list_to_hex_str(tmp[0:md_len]))
            hmac_obj.update(tmp_bytes)
            hash_bytes = bytes.fromhex(hmac_obj.hexdigest().upper())
            hash_list = ['%02X' % int(j) for j in hash_bytes]
            for j in range(0, md_len):
                tmp[j] = int(hash_list[j], 16)

            if i + md_len > dlen:
                k = dlen % md_len
            else:
                k = md_len
            for i in range(0, k):
                DerivedKey[i] = h_i[i]
        return 0

class RSIP():
    def __init__(self, output_file:str, input_file:str, address:str, image_config:ManifestImageConfig):
        self.output_file = output_file
        self.input_file = input_file
        self.image_config = image_config
        self.address = int(address, 16)

    def RSIP_ImageEncrypt(self) -> int:
        iv_counter = '0000000000000000'
        iv = self.image_config.rsip_iv + iv_counter
        iv = [int(i) for i in bytes.fromhex(iv)]
        if self.image_config.rsip_mode == 1:
            ctrkey_bytes = bytes.fromhex(self.image_config.rsip_key[1])
            ecbkey_bytes = bytes.fromhex(self.image_config.rsip_key[0])
        else:
            ctrkey_bytes = bytes.fromhex(self.image_config.rsip_key[0])

        init_val = [0x2, 0x3]
        i = 0
        # every 16bytes, due to xts mode need the enc result of iv
        tag_name = os.path.join(os.path.dirname(self.output_file), os.path.splitext(os.path.basename(self.output_file))[0] + '_tag.bin')

        if self.image_config.rsip_mode == 2:
            # print(list_to_hex_str(iv))
            # print(list_to_hex_str([int(i) for i in ctrkey_bytes]))
            fw_tag = open(tag_name, 'wb')
            cnt = 32
        else:
            if os.path.exists(tag_name):
                os.remove(tag_name)
            cnt = 16
        fw = open(self.output_file, 'wb')
        with open(self.input_file, 'rb') as f:
            buf = f.read(cnt)
            while buf:
                self.address &= ~(0xF << 28)  # clear [31:28] to 0
                tempaddr = self.address // 32
                # iv_prefix = [int(i) for i in bytes.fromhex(iv_prefix)]
                iv[8] = (tempaddr >> 24) & 0xff
                iv[9] = (tempaddr >> 16) & 0xff
                iv[10] = (tempaddr >> 8) & 0xff
                iv[11] = tempaddr & 0xff
                iv[12] = 0x0
                iv[13] = 0x0
                iv[14] = 0x0
                iv[15] = init_val[i % 2]
                if self.image_config.rsip_mode == 0:
                    ctr_cryptor = AES.new(ctrkey_bytes, AES.MODE_CTR, nonce=bytes(iv[0:8]),initial_value=bytes(iv[8:]))
                    enbuf = ctr_cryptor.encrypt(buf)
                elif self.image_config.rsip_mode == 1:
                    ctr_cryptor = AES.new(ctrkey_bytes, AES.MODE_CTR, nonce=bytes(iv[0:8]),initial_value=bytes(iv[8:]))
                    enbuf_ctr = ctr_cryptor.encrypt(buf)

                    ecb_cryptor = AES.new(ctrkey_bytes, AES.MODE_ECB)
                    encount_buf = ecb_cryptor.encrypt(bytes(iv))

                    ecb_cryptor = AES.new(ecbkey_bytes, AES.MODE_ECB)
                    enbuf_ecb = ecb_cryptor.encrypt(enbuf_ctr)

                    encount_buf_i = [int(i) for i in encount_buf]
                    enbuf_i = [int(i) for i in enbuf_ecb]
                    result = [a ^ b for a, b in zip(encount_buf_i, enbuf_i)]
                    enbuf = bytes(result)
                elif self.image_config.rsip_mode == 2:
                    gcm_cryptor = AES.new(ctrkey_bytes, AES.MODE_GCM, nonce=bytes(iv[0:12]), mac_len=self.image_config.rsip_gcm_tag_len)
                    enbuf, tag = gcm_cryptor.encrypt_and_digest(buf)
                    fw_tag.write(tag)

                fw.write(enbuf)

                buf = f.read(cnt)
                self.address += cnt
                i += 1
        fw.close()
        if self.image_config.rsip_mode == 2:
            fw_tag.close()
        return 0
    def RSIP_ImageEncrypt_AMEBAD(self):
        iv = [int(i) for i in bytes.fromhex(self.image_config.rsip_iv)]
        ctrkey_bytes = bytes.fromhex(self.image_config.ctr_key)

        # every 16bytes, due to xts mode need the enc result of iv
        cnt = 16
        fw = open(self.output_file, 'wb')
        with open(self.input_file, 'rb') as f:
            buf = f.read(16)
            while buf:
                iv[12] = 0
                iv[13] = (self.address >> 20) & 0xff
                iv[14] = (self.address >> 12) & 0xff
                iv[15] = (self.address >> 4) & 0xff
                ctr_cryptor = AES.new(ctrkey_bytes, AES.MODE_CTR, nonce=bytes(iv[0:12]),initial_value=bytes(iv[12:]))
                enbuf = ctr_cryptor.encrypt(buf)
                fw.write(enbuf)

                buf = f.read(cnt)
                self.address += cnt
        fw.close()
        return

class RDP():
    def __init__(self, output_file:str, input_file:str, encrypt:bool, image_config:ManifestImageConfig):
        self.output_file = output_file
        self.input_file = input_file
        self.encrypt = encrypt
        self.image_config = image_config

    def rdp_get_info(self, filename, imgtypename):
        # print(os.getcwd())
        with open(filename, 'r') as f:
            content = f.read()
            json_data = json.loads(content)
        json_keys = json_data.keys()

        if 'RDP_EN' in json_keys:
            self.rdp_enc = json_data['RDP_EN']

        if self.rdp_enc:
            if imgtypename in json_keys:
                img_json_data = json_data[imgtypename]
                img_json_keys = img_json_data.keys()
                if 'RSIP_IV' in img_json_keys:
                    if len(img_json_data['RSIP_IV']) == 16:
                        self.iv += img_json_data['RSIP_IV']
                    else:
                        print('RSIP_IV format error: should be 8 bytes')
                        return -4
            if 'RDP_IV' in json_keys:
                if len(json_data['RDP_IV']) == 16:
                    self.iv += json_data['RDP_IV']
                else:
                    print('RDP_IV format error: should be 8 bytes')
                    return -4
            if 'RDP_KEY' in json_keys:
                if len(json_data['RDP_KEY']) == 64 or len(json_data['RDP_KEY']) == 32:
                    self.key = json_data['RDP_KEY']
                else:
                    print('RDP_KEY format error: should be 32 bytes or 16 bytes')
                    return -5
        return 0

    def rdp_encrypt(self):
        key_bytes = bytes.fromhex(self.image_config.rdp_key)
        iv = ''
        if self.image_config.rsip_iv != None:
            iv += self.image_config.rsip_iv
        if self.image_config.rdp_iv != None:
            iv += self.image_config.rdp_iv
        iv_bytes = bytes.fromhex(iv)
        # encrypt
        fw = open(self.output_file, 'wb')
        with open(self.input_file, 'rb') as f:
            buf = f.read()
            # while buf:
            cipher = Cipher(algorithms.AES(key_bytes), modes.CBC(iv_bytes), backend=default_backend())
            if self.encrypt:
                aes_cryptor = cipher.encryptor()
                enbuf = aes_cryptor.update(buf) + aes_cryptor.finalize()
            else:
                aes_cryptor = cipher.decryptor()
                enbuf = aes_cryptor.update(buf) + aes_cryptor.finalize()
            fw.write(enbuf)

        fw.close()
        return

    def rdp_encrypt_AMEBAD(self):
        key_bytes = bytes.fromhex(self.image_config.rdp_key)
        fw = open(self.output_file, 'wb')
        with open(self.input_file, 'rb') as f:
            buf = f.read()
            buflen = len(buf)
            new_size = ((buflen - 1) // 4 + 1) * 4
            padcount = new_size - buflen
            buf += (b'\x00' * padcount)

            aes_cryptor = AES.new(key=key_bytes, mode=AES.MODE_ECB)
            enbuf = aes_cryptor.encrypt(buf)
            fw.write(enbuf)

        checksum = [0, 0, 0, 0]
        for i in range(0, new_size, 4):
            for j in range(4):
                checksum[j] = (checksum[j]^buf[i + j]) & 0xFF
        fw.write(list_to_bytes(checksum))
        fw.close()
        return
