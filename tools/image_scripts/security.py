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

def check_python_lib(lib):
    print('%s Python library is not installed.Please install by command: pip install -r {sdk}/tools/image_scripts/requirements.txt'%(lib))
    sys.exit(-1)

try:
    import mbedtls
except:
    check_python_lib('mbedtls')

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

from mbedtls.pk import Curve
from cryptography.hazmat.primitives.asymmetric import ed25519, ec
from cryptography.hazmat.primitives import serialization
from Crypto.Cipher import AES

ciL = 8
biL = ciL << 3
SIGN_MAX_LEN = 144
PKEY_MAX_LEN = 144
HASH_MAX_LEN = 64

def init_list(length):
    ilist = []
    for _ in range(length):
        ilist.append(0)
    return ilist

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
    MBEDTLS_ECP_DP_CURVE2551 = 9     # Domain parameters for Curve25519. */
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
    def __init__(self, argc, argv):
        self.argc = argc
        self.argv = argv
        self.PrivKey = ''
        self.IsHMAC = 0
        self.HmacKey = ''
        self.HmacKeyLen = 0
        self.SecureBootEn = 0
        self.MdType = ''

    def get_supported_curve(self, auth_alg):
        if auth_alg == mbedtls_ecp_group_id.MBEDTLS_ECP_DP_SECP192R1.value:
            return Curve.SECP192R1
        elif auth_alg == mbedtls_ecp_group_id.MBEDTLS_ECP_DP_SECP224R1.value:
            return Curve.SECP224R1
        elif auth_alg == mbedtls_ecp_group_id.MBEDTLS_ECP_DP_SECP256R1.value:
            return Curve.SECP256R1
        elif auth_alg == mbedtls_ecp_group_id.MBEDTLS_ECP_DP_BP256R1.value:
            return Curve.BRAINPOOLP256R1
        elif auth_alg == mbedtls_ecp_group_id.MBEDTLS_ECP_DP_SECP192K1.value:
            return Curve.SECP192K1
        elif auth_alg == mbedtls_ecp_group_id.MBEDTLS_ECP_DP_SECP224K1.value:
            return Curve.SECP224K1
        elif auth_alg == mbedtls_ecp_group_id.MBEDTLS_ECP_DP_SECP256K1.value:
            return Curve.SECP256K1
        else:
            print("Not supported AUTH_ALG!")
            return -1
    def gen_auth_id(self, strs):
        if strs == "ed25519":
            return Sec_AuthAlg.Sec_AuthID_ED25519.value
        elif strs == "ecdsa_secp192r1":
            return mbedtls_ecp_group_id.MBEDTLS_ECP_DP_SECP192R1.value
        elif strs == "ecdsa_secp224r1":
            return mbedtls_ecp_group_id.MBEDTLS_ECP_DP_SECP224R1.value
        elif strs == "ecdsa_secp256r1":
            return mbedtls_ecp_group_id.MBEDTLS_ECP_DP_SECP256R1.value
        elif strs == "ecdsa_bp256r1":
            return mbedtls_ecp_group_id.MBEDTLS_ECP_DP_BP256R1.value
        elif strs == "ecdsa_secp192k1":
            return mbedtls_ecp_group_id.MBEDTLS_ECP_DP_SECP192K1.value
        elif strs == "ecdsa_secp224k1":
            return mbedtls_ecp_group_id.MBEDTLS_ECP_DP_SECP224K1.value
        elif strs == "ecdsa_secp256k1":
            return mbedtls_ecp_group_id.MBEDTLS_ECP_DP_SECP256K1.value
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
        hash = mbedtls.hashlib.sha256()
        hash.update(pubkey_bytes)
        pubkey_hash = hash.hexdigest().upper()
        # print(pubkey_hash)

        privkey_hex = list_to_hex_str(privkey_bytes)
        pubkey_hex = list_to_hex_str(pubkey_bytes)

        keyinfo['private key'] = privkey_hex
        keyinfo['public key'] = pubkey_hex
        keyinfo['public key hash'] = pubkey_hash
        return

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
        memmove(addressof(sig), sig_bytes, ed25519._ED25519_SIG_SIZE)
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
        elif id == Curve.BRAINPOOLP256R1:
            curve = ec.BrainpoolP256R1()
        elif id == Curve.SECP192K1:
            curve = 'secp192k1'
            use_fastecdsa = 1
        elif id == Curve.SECP224K1:
            curve = 'secp224k1'
            use_fastecdsa = 1
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
                hash = mbedtls.hashlib.new(name=self.MdType)
                hash.update(msg_bytes)
                msg_hash = hash.hexdigest().upper()
                msg_hash_bytes = bytes.fromhex(msg_hash)
                sig_bytes = key.sign(msg_hash_bytes, privkey_bytes, hash=None)
            else:
                hmackey_bytes = bytes.fromhex(self.HmacKey)
                hash = mbedtls.hmac.new(key=hmackey_bytes, digestmod=self.MdType)
                hash.update(msg_bytes)
                msg_hash = hash.hexdigest().upper()
                msg_hash_bytes = bytes.fromhex(msg_hash)
                sig_bytes = key.sign(msg_hash_bytes, privkey_bytes, hash=None)
            # print(len(sig_bytes))
            # print(list_to_hex_str(sig_bytes))
            memmove(addressof(sig), sig_bytes, csize * 2)
        elif id == Curve.SECP224K1:
            csize = 224 // 8
            msg_bytes = string_at(addressof(msg), mlen)
            der = mbedtls_pk_binary_to_der(privkey, string_at(addressof(pubkey), csize * 2).hex())
            key = mbedtls.pk.ECC.from_DER(der)
            sig_bytes = key.sign(msg_bytes, digestmod=self.MdType)
            r,s = ecdsa.util.sigdecode_der(sig_bytes, 0)
            r_arr = point_to_bignum_arr(r, csize, 0)
            # print('r_arr: ', r_arr)
            s_arr = point_to_bignum_arr(s, csize, 0)
            # print('s_arr: ', s_arr)

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
            csize = (curve.key_size + 7) // 8
            new_pubkey = b'\x04' + string_at(addressof(pubkey), csize * 2)
            pubkey = ec.EllipticCurvePublicKey.from_encoded_point(curve, new_pubkey)
            pem = pubkey.public_bytes(
                encoding=serialization.Encoding.PEM,
                format=serialization.PublicFormat.SubjectPublicKeyInfo)
            privkey_bytes = bytes.fromhex(privkey)
            d = mbedtls_mpi_read_binary(privkey, csize)
            privkey = ec.derive_private_key(int(d), curve)
            pem = privkey.private_bytes(encoding=serialization.Encoding.PEM,
                                        format=serialization.PrivateFormat.PKCS8,
                                        encryption_algorithm=serialization.NoEncryption())
            # msg hash
            msg_bytes = string_at(addressof(msg), mlen)
            if self.IsHMAC == 0:
                key = mbedtls.pk.ECC.from_PEM(pem.decode('utf-8'))
                sig_bytes = key.sign(msg_bytes, digestmod=self.MdType)
                r,s = ecdsa.util.sigdecode_der(sig_bytes, 0)
                r_arr = point_to_bignum_arr(r, csize, 0)
                # print('r_arr: ', r_arr)
                s_arr = point_to_bignum_arr(s, csize, 0)
                # print('s_arr: ', s_arr)

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
                # sign for msg hash
                key = ecdsa.SigningKey.from_pem(pem.decode('utf-8'))
                def hashfunc_user(data):
                    return mbedtls.hmac.new(key=hmackey_bytes, buffer=data, digestmod=self.MdType)
                sig_bytes = key.sign(msg_bytes, hashfunc=hashfunc_user)

                # print(len(sig_bytes))
                # print(list_to_hex_str(sig_bytes))
                memmove(addressof(sig), sig_bytes, csize * 2)
        return 0

    def ecdsa_genkey(self, id, keyinfo):
        ecc = mbedtls.pk.ECC(id)
        ecc.generate()
        Q = ecc.export_public_key("POINT")
        d = ecc.export_key("NUM")

        # print('Q: ',Q)
        # print('d: ',d)

        csize = ecc.key_size
        # print('csize: ',csize)
        buflen = csize * 2 + 1
        pubkey = init_list(buflen)
        privkey = init_list(buflen)
        mbedtls_ecp_point_write_binary(Q.x, Q.y, pubkey, buflen, csize)
        # print(pubkey[1:], len(pubkey[1:]))
        d_arr = point_to_bignum_arr(d, csize, 0)
        mbedtls_mpi_write_binary(d_arr, privkey, csize)
        # print(privkey[0:csize], len(privkey[0:csize]))

        pubkey_bytes = list_to_bytes(pubkey[1:])
        hash = mbedtls.hashlib.sha256()
        hash.update(pubkey_bytes)
        pubkey_hash = hash.hexdigest().upper()

        pubkey_hex = list_to_hex_str(pubkey[1:])
        privkey_hex = list_to_hex_str(privkey[0:csize])
        # print(pubkey_hex)
        # print(privkey_hex)

        keyinfo['private key'] = privkey_hex
        keyinfo['public key'] = pubkey_hex
        keyinfo['public key hash'] = pubkey_hash
        return keyinfo

    def gen_image_hash(self, filename, imghash):
        with open(filename, 'rb') as f:
            buf = f.read(1024)
            if self.IsHMAC == 0:
                hash = mbedtls.hashlib.new(name=self.MdType)
                while buf:
                    hash.update(buf)
                    buf = f.read(1024)
                hash_hex = hash.hexdigest().upper()
            else:
                hmackey_bytes = bytes.fromhex(self.HmacKey)
                hash = mbedtls.hmac.new(key=hmackey_bytes, digestmod=self.MdType)
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
        hmac = mbedtls.hmac.new(key=secret_bytes, digestmod='sha256')
        tmp_bytes = label_bytes + seed_bytes
        hmac.update(tmp_bytes)
        hash_bytes =  bytes.fromhex(hmac.hexdigest().upper())

        for i in range(0, md_len):
            tmp[i] = int(hash_bytes[i])

        for i in range(0, dlen, md_len):
            hmac = mbedtls.hmac.new(key=secret_bytes, digestmod='sha256')
            tmp_bytes = bytes.fromhex(list_to_hex_str(tmp[0:md_len+nb]))
            hmac.update(tmp_bytes)
            h_i = bytes.fromhex(hmac.hexdigest().upper())

            hmac = mbedtls.hmac.new(secret_bytes, digestmod='sha256')
            tmp_bytes = bytes.fromhex(list_to_hex_str(tmp[0:md_len]))
            hmac.update(tmp_bytes)
            hash_bytes = bytes.fromhex(hmac.hexdigest().upper())
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
    def __init__(self, argc, argv):
        self.argc = argc
        self.argv = argv
        self.RsipEn = 0
        self.RsipMode = 0
        self.test_ctrkey = ''
        self.test_ecbkey = ''
        self.test_ctrkey1 = ''
        self.test_ecbkey1 = ''
        self.iv_nonce = ''
        self.rsip_key_id = 0
        self.gcm_tag_len = 4

    def RSIP_GetInfo(self, filename, imgtypename):
        with open(filename, 'r') as f:
            content = f.read()
            json_data = json.loads(content)
        json_keys = json_data.keys()
        if 'RSIP_EN' in json_keys:
            self.RsipEn = json_data['RSIP_EN']

        if imgtypename in json_keys:
            img_json_data = json_data[imgtypename]
            img_json_keys = img_json_data.keys()
            if 'RSIP_EN' in img_json_keys:
                self.RsipEn = img_json_data['RSIP_EN']

        if self.RsipEn:
            if 'RSIP_IV' in json_keys:
                self.iv_nonce = json_data['RSIP_IV']
            if 'RSIP_MODE' in json_keys:
                self.RsipMode = json_data['RSIP_MODE']
                if self.RsipMode > 2:
                    print('Rsip mode error: should be 0, 1, 2')
                    return -5
            if 'CTR_KEY' in json_keys:
                self.test_ctrkey = json_data['CTR_KEY']
                if len(self.test_ctrkey) != 64 and len(self.test_ctrkey) != 32:
                    print('CTR_KEY format error: should be 32 bytes or 16 bytes')
                    return -6

            if 'CTR_KEY1' in json_keys:
                self.test_ctrkey1 = json_data['CTR_KEY1']
                if len(self.test_ctrkey1) != 64:
                    print('CTR_KEY1 format error: should be 32 bytes')
                    return -6
            # get ecb key if xts mode
            if 'ECB_KEY' in json_keys:
                self.test_ecbkey = json_data['ECB_KEY']
                if len(self.test_ecbkey) != 64:
                    print('ECB_KEY format error: should be 32 bytes')
                    return -7
            if 'ECB_KEY1' in json_keys:
                self.test_ecbkey1 = json_data['ECB_KEY1']
                if len(self.test_ecbkey1) != 64:
                    print('ECB_KEY1 format error: should be 32 bytes')
                    return -7
            if imgtypename in json_keys:
                img_json_data = json_data[imgtypename]
                img_json_keys = img_json_data.keys()
                if 'RSIP_IV' in img_json_keys:
                    self.iv_nonce = img_json_data['RSIP_IV']
                    if len(self.iv_nonce) != 16:
                        print('RSIP_IV format error: should be 8 bytes')
                        return -4
                if 'RSIP_MODE' in img_json_keys:
                    self.RsipMode = img_json_data['RSIP_MODE']
                    if self.RsipMode > 2:
                        print('Rsip mode error: should be 0, 1, 2')
                        return -8
                if 'GCM_TAG_LEN' in img_json_keys:
                    self.gcm_tag_len = img_json_data['GCM_TAG_LEN']
                    if self.gcm_tag_len != 4 and self.gcm_tag_len != 8 and self.gcm_tag_len != 16:
                        print('Rsip gcm tag len error: should be even in range [4...16]')
                        return -9
                if 'RSIP_KEY_ID' in img_json_keys:
                    self.rsip_key_id = img_json_data['RSIP_KEY_ID']
                    if self.rsip_key_id > 1:
                        print('Rsip key id error: should be 0 or 1')
                        return -8
        return 0

    def RSIP_ImageEncrypt(self):
        addr_str = self.argv[4]
        addr = int(addr_str, 16)

        ret = self.RSIP_GetInfo(self.argv[5], self.argv[6])
        if ret != 0:
            print('Fail to get info, ret: ', ret)
            return

        iv_counter = '0000000000000000'
        iv = self.iv_nonce + iv_counter
        iv = [int(i) for i in bytes.fromhex(iv)]
        if self.rsip_key_id == 1:
            ctrkey_bytes = bytes.fromhex(self.test_ctrkey1)
            ecbkey_bytes = bytes.fromhex(self.test_ecbkey1)
        else:
            ctrkey_bytes = bytes.fromhex(self.test_ctrkey)
            ecbkey_bytes = bytes.fromhex(self.test_ecbkey)

        init_val = [0x2, 0x3]
        i = 0
        # every 16bytes, due to xts mode need the enc result of iv
        if self.RsipMode == 2:
            # print(list_to_hex_str(iv))
            # print(list_to_hex_str([int(i) for i in ctrkey_bytes]))
            tag_name = os.path.join(os.path.dirname(self.argv[3]), os.path.splitext(self.argv[3])[0] + '_tag.bin')
            fw_tag = open(tag_name, 'wb')
            cnt = 32
        else:
            cnt = 16
        fw = open(self.argv[3], 'wb')
        with open(self.argv[2], 'rb') as f:
            buf = f.read(cnt)
            while buf:
                if self.RsipEn:
                    addr &= ~(0xF << 28)  # clear [31:28] to 0
                    tempaddr = addr // 32
                    # iv_prefix = [int(i) for i in bytes.fromhex(iv_prefix)]
                    iv[8] = (tempaddr >> 24) & 0xff
                    iv[9] = (tempaddr >> 16) & 0xff
                    iv[10] = (tempaddr >> 8) & 0xff
                    iv[11] = tempaddr & 0xff
                    iv[12] = 0x0
                    iv[13] = 0x0
                    iv[14] = 0x0
                    iv[15] = init_val[i % 2]
                    if self.RsipMode == 0:
                        ctr_cryptor = AES.new(ctrkey_bytes, AES.MODE_CTR, nonce=bytes(iv[0:8]),initial_value=bytes(iv[8:]))
                        enbuf = ctr_cryptor.encrypt(buf)
                    elif self.RsipMode == 1:
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
                    elif self.RsipMode == 2:
                        gcm_cryptor = AES.new(ctrkey_bytes, AES.MODE_GCM, nonce=bytes(iv[0:12]), mac_len=self.gcm_tag_len)
                        enbuf, tag = gcm_cryptor.encrypt_and_digest(buf)
                        fw_tag.write(tag)

                    fw.write(enbuf)
                else:
                    fw.write(buf)
                buf = f.read(cnt)
                addr += cnt
                i += 1
        fw.close()
        if self.RsipMode == 2:
            fw_tag.close()
        return
    def RSIP_ImageEncrypt_AMEBAD(self):
        addr_str = self.argv[4]
        addr = int(addr_str, 16)

        ret = self.RSIP_GetInfo(self.argv[5], '')
        if ret != 0:
            print('Fail to get info, ret: ', ret)
            return
        if self.RsipMode != 0:
            print('Rsip mode error: should be 0 for AMEBAD')
            return
        iv = [int(i) for i in bytes.fromhex(self.iv_nonce)]
        ctrkey_bytes = bytes.fromhex(self.test_ctrkey)

        # every 16bytes, due to xts mode need the enc result of iv
        cnt = 16
        fw = open(self.argv[3], 'wb')
        with open(self.argv[2], 'rb') as f:
            buf = f.read(16)
            while buf:
                if self.RsipEn:
                    iv[12] = 0
                    iv[13] = (addr >> 20) & 0xff
                    iv[14] = (addr >> 12) & 0xff
                    iv[15] = (addr >> 4) & 0xff
                    ctr_cryptor = AES.new(ctrkey_bytes, AES.MODE_CTR, nonce=bytes(iv[0:12]),initial_value=bytes(iv[12:]))
                    enbuf = ctr_cryptor.encrypt(buf)
                    fw.write(enbuf)
                else:
                    fw.write(buf)
                buf = f.read(cnt)
                addr += cnt
        fw.close()
        return

class RDP():
    def __init__(self, argc, argv):
        self.argc = argc
        self.argv = argv
        self.key = ''
        self.iv = ''
        self.rdp_enc = 0

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
        mode = self.argv[2]
        BUF_SIZE = 4096

        ret = self.rdp_get_info(self.argv[5], 'app')
        if ret != 0:
            return

        key_bytes = bytes.fromhex(self.key)
        iv_bytes = bytes.fromhex(self.iv)
        # encrypt
        fw = open(self.argv[4], 'wb')
        # print(self.argv[4])
        with open(self.argv[3], 'rb') as f:
            buf = f.read()
            # while buf:
            if self.rdp_enc:
                if mode == 'enc':
                    aes_cryptor = mbedtls.cipher.AES.new(key=key_bytes,
                                                        mode=mbedtls.cipher.MODE_CBC,
                                                        iv=iv_bytes)
                    enbuf = aes_cryptor.encrypt(buf)
                elif mode == 'dec':
                    aes_cryptor = mbedtls.cipher.AES.new(key=key_bytes,
                                mode=mbedtls.cipher.MODE_CBC,
                                iv=iv_bytes)
                    enbuf = aes_cryptor.decrypt(buf)
                fw.write(enbuf)
            else:
                fw.write(buf)
                # buf = f.read(BUF_SIZE)
        fw.close()
        return

    def rdp_encrypt_AMEBAD(self):
        print(self.argv)
        ret = self.rdp_get_info(self.argv[5], '')
        if ret != 0:
            return

        key_bytes = bytes.fromhex(self.key)
                # encrypt
        fw = open(self.argv[4], 'wb')
        # print(self.argv[4])
        with open(self.argv[3], 'rb') as f:
            buf = f.read()
            buflen = len(buf)
            new_size = ((buflen - 1) // 4 + 1) * 4
            padcount = new_size - buflen
            for _ in range(padcount):
                buf += (b'\x00')
            # while buf:
            if self.rdp_enc:
                aes_cryptor = AES.new(key=key_bytes, mode=AES.MODE_ECB)
                enbuf = aes_cryptor.encrypt(buf)
                fw.write(enbuf)
            else:
                fw.write(buf)
                # buf = f.read(BUF_SIZE)
        checksum = [0, 0, 0, 0]
        for i in range(0, new_size, 4):
            for j in range(4):
                checksum[j] = (checksum[j]^buf[i + j]) & 0xFF
        fw.write(list_to_bytes(checksum))
        fw.close()
        return
