#!/usr/bin/env python3
# -*- coding:utf-8 -*- 
import os
import sys
from ctypes import *
from enum import Enum, unique
import struct
import json
import re
import shutil
import importlib

SIGN_MAX_LEN = 144
PKEY_MAX_LEN = 144
HASH_MAX_LEN = 64

FlashCalibPattern = [0x96969999, 0xFC66CC3F]
ImagePattern = [0x35393138, 0x31313738]

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

def htonl_to_ntohl(value):
    # big-endian to little-endian
    packed_value = struct.pack('>I', value)
    return struct.unpack('<I', packed_value)[0]

class ImgID(Enum):
    IMGID_BOOT = 0
    IMGID_NSPE = 1
    IMGID_SPE = 2
    IMGID_BL1 = 3
    IMGID_CERT = 4

@unique
class AuthAlg(Enum):
    AuthID_ED25519 = 0

class Manifest_TypeDef(Structure):
        _fields_=[('Pattern',c_uint32 * 2),
                  ('Rsvd1',c_uint8 * 8),
                  ('Ver',c_uint8),
                  ('ImgID',c_uint8),
                  ('AuthAlg',c_uint8),
                  ('HashAlg',c_uint8),
                  ('MajorImgVer',c_uint16),
                  ('MinorImgVer',c_uint16),
                  ('ImgSize',c_uint32),
                  ('SecEpoch',c_uint32),
                  ('RsipIV',c_uint8 * 16),
                  ('ImgHash',c_uint8 * HASH_MAX_LEN),
                  ('SBPubKey',c_uint8 * PKEY_MAX_LEN),
                  ('Signature',c_uint8 * SIGN_MAX_LEN)]

class CertEntry_TypeDef(Structure):
    _fields_=[('KeyID',c_uint),
              ('Hash',c_uint8 * 32)]

class Certificate_TypeDef(Structure):
    _fields_=[('Pattern',c_uint8 * 8),
              ('Rsvd1',c_uint8 * 8),
              ('Ver',c_uint8),
              ('ImgID',c_uint8),
              ('AuthAlg',c_uint8),
              ('HashAlg',c_uint8),
              ('MajorKeyVer',c_uint16),
              ('MinorKeyVer',c_uint16),
              ('TableSize',c_uint32),
              ('EntryNum',c_uint32),
              ('SBPubKey',c_uint8 * PKEY_MAX_LEN),
              ('PKInfo',c_uint8 * sizeof(CertEntry_TypeDef) * 5),
              ('Signature',c_uint8 * SIGN_MAX_LEN)]

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
        self.algorithm = ''

    def create_keypair(self):
        lib_security = importlib.import_module('security')
        sboot = lib_security.secure_boot(self.argc, self.argv)
        auth_alg_id = sboot.gen_auth_id(self.argv[2])
        if auth_alg_id == -1:
            print("Fail to create keypair, ret: %d"%auth_alg_id)
            return
        keyinfo = {'algorithm':'', 'private key':'', 'public key':'', 'public key hash':''}
        keyinfo['algorithm'] = self.argv[2]
        
        if auth_alg_id == AuthAlg.AuthID_ED25519.value:
            sboot.ed25519_genkey(keyinfo)
        else:
            curve = sboot.get_supported_curve(auth_alg_id)
            sboot.ecdsa_genkey(curve, keyinfo)
        # save keypair
        with open(self.argv[3], 'w') as f:
            f.write(json.dumps(keyinfo, indent=4))
        print("Create keypair successfully")
        return
    def gen_image_size(self, filename, manifest):
        if not os.path.exists(filename):
            return -1
        imgsize = os.path.getsize(filename)
        manifest.ImgSize = imgsize
        return 0

    def parse_manifest_json(self, filename, imgtypename, manifest):
        rsip_en, rdp_en = 0, 0
        with open(filename, 'r') as f:
            content = f.read()
            json_data = json.loads(content)
        json_keys = json_data.keys()
        if 'MANIFEST_VER' in json_keys:
            manifest.Ver = json_data['MANIFEST_VER']
        if 'SECURE_BOOT_EN' in json_keys:
            self.SecureBootEn = json_data['SECURE_BOOT_EN']
        if 'RSIP_EN' in json_keys:
            rsip_en = json_data['RSIP_EN']
        if imgtypename in json_keys:
            img_json_data = json_data[imgtypename]
            img_json_keys = img_json_data.keys()
            if 'IMG_ID' in img_json_keys:
                manifest.ImgID = int(img_json_data['IMG_ID'])
                if manifest.ImgID == ImgID.IMGID_BOOT.value:
                    FlashCalibPattern_lsb = [htonl_to_ntohl(i) for i in FlashCalibPattern]
                    FlashCalibPattern_bytes = bytes.fromhex(list_to_hex_str(FlashCalibPattern_lsb))
                    memmove(addressof(manifest.Pattern), FlashCalibPattern_bytes, sizeof(manifest.Pattern))
                else:
                    ImagePattern_lsb = [htonl_to_ntohl(i) for i in ImagePattern]
                    ImagePattern_bytes = bytes.fromhex(list_to_hex_str(ImagePattern_lsb))
                    memmove(addressof(manifest.Pattern), ImagePattern_bytes, sizeof(manifest.Pattern))
            if 'IMG_VER_MAJOR' in img_json_keys:
                if manifest.ImgID == ImgID.IMGID_BOOT.value:
                    if img_json_data['IMG_VER_MAJOR'] > 0x7FFF:
                        print('boot IMG_VER_MAJOR should in range 0~32767!!!')
                        return -5
                else:
                    if img_json_data['IMG_VER_MAJOR'] > 0xFFFF:
                        print('app IMG_VER_MAJOR should in range 0~65535!!!')
                        return -5
                manifest.MajorImgVer = img_json_data['IMG_VER_MAJOR']
            if 'IMG_VER_MINOR' in img_json_keys:
                if manifest.ImgID == ImgID.IMGID_BOOT.value:
                    if img_json_data['IMG_VER_MINOR'] > 0x7FFF:
                        print('boot IMG_VER_MINOR should in range 0~32767!!!')
                        return -5
                else:
                    if img_json_data['IMG_VER_MINOR'] > 0xFFFF:
                        print('app IMG_VER_MINOR should in range 0~65535!!!')
                        return -5
                manifest.MinorImgVer = img_json_data['IMG_VER_MINOR']

            if 'SEC_EPOCH' in img_json_keys:
                manifest.SecEpoch = img_json_data['SEC_EPOCH']

            if self.SecureBootEn:
                if 'HASH_ALG' in img_json_keys:
                    self.MdType = img_json_data['HASH_ALG']
                if 'HMAC_KEY' in json_keys:
                    length = len(json_data['HMAC_KEY'])
                    if length != 64:
                        print('HMAC_KEY format error: should be 32 bytes')
                        return -7
                    self.HmacKey = json_data['HMAC_KEY']
                    # print('HMAC_KEY:', self.HmacKey)
                    self.HmacKeyLen = length // 2
            
            if rsip_en:
                if 'RSIP_IV' in img_json_keys:
                    if len(img_json_data['RSIP_IV']) == 16:
                        rsip_iv_bytes = bytes.fromhex(img_json_data['RSIP_IV'])
                        memmove(addressof(manifest.RsipIV), rsip_iv_bytes, 8)
                    else:
                        print('RSIP_IV format error: should be 8 bytes')
                        return -8
        if ImgID.IMGID_NSPE.value == manifest.ImgID:
            if 'RDP_EN' in json_keys:
                rdp_en = json_data['RDP_EN']
            
            if rdp_en:
                if imgtypename in json_keys:
                    img_json_data = json_data[imgtypename]
                    img_json_keys = img_json_data.keys()
                    if 'RSIP_IV' in img_json_keys:
                        if len(img_json_data['RSIP_IV']) == 16:
                            rsip_iv_bytes = bytes.fromhex(img_json_data['RSIP_IV'])
                            memmove(addressof(manifest.RsipIV), rsip_iv_bytes, 8)
                        else:
                            print('RSIP_IV format error: should be 8 bytes')
                            return -8
                if 'RDP_IV' in json_keys:
                    if len(json_data['RDP_IV']) == 16:
                        rdp_iv_bytes = bytes.fromhex(json_data['RDP_IV'])
                        memmove(byref(manifest.RsipIV, 8), rdp_iv_bytes, 8)
                    else:
                        print('RDP_IV format error: should be 8 bytes')
                        return -8
        return 0
    
    def parse_cert_json(self, filename, imgtypename, cert):
        with open(filename, 'r') as f:
            content = f.read()
            json_data = json.loads(content)
        json_keys = json_data.keys()
        if 'MANIFEST_VER' in json_keys:
            cert.Ver = json_data['MANIFEST_VER']
        
        if 'SECURE_BOOT_EN' in json_keys:
            self.SecureBootEn = json_data['SECURE_BOOT_EN']
        
        if imgtypename in json_keys:
            img_json_data = json_data[imgtypename]
            img_json_keys = img_json_data.keys()
            if 'IMG_ID' in img_json_keys:
                cert.ImgID = int(img_json_data['IMG_ID'])
                ImagePattern_lsb = [htonl_to_ntohl(i) for i in ImagePattern]
                ImagePattern_bytes = bytes.fromhex(list_to_hex_str(ImagePattern_lsb))
                memmove(addressof(cert.Pattern), ImagePattern_bytes, sizeof(cert.Pattern))
            if 'IMG_VER_MAJOR' in img_json_keys:
                if img_json_data['IMG_VER_MAJOR'] > 0xFFFF:
                    print('app IMG_VER_MAJOR should in range 0~65535!!!')
                    return -5
                cert.MajorKeyVer = img_json_data['IMG_VER_MAJOR']
            if 'IMG_VER_MINOR' in img_json_keys:
                if img_json_data['IMG_VER_MINOR'] > 0xFFFF:
                    print('app IMG_VER_MINOR should in range 0~65535!!!')
                    return -5
                cert.MinorKeyVer = img_json_data['IMG_VER_MINOR']
            if self.SecureBootEn:
                if 'HASH_ALG' in img_json_keys:
                    self.MdType = img_json_data['HASH_ALG']
                if 'HMAC_KEY' in img_json_keys:
                    length = len(img_json_data['HMAC_KEY'])
                    if length != 64:
                        print('HMAC_KEY format error: should be 32 bytes')
                        return -7
                    self.HmacKey = img_json_data['HMAC_KEY']
                    self.HmacKeyLen = length // 2
        return 0
    
    def parse_key_json(self, filename, imgtypename, manifest, privkey, pubkey, hash):
        with open(filename, 'r') as f:
            content = f.read()
            json_data = json.loads(content)
        json_keys = json_data.keys()
        if imgtypename in json_keys:
            img_json_data = json_data[imgtypename]
            img_json_keys = img_json_data.keys()

            if 'algorithm' in img_json_keys:
                self.algorithm = img_json_data['algorithm']
            if privkey:
                if 'private key' in img_json_keys:
                    self.PrivKey = img_json_data['private key']
            if pubkey:
                if 'public key' in img_json_keys:
                    pubkey_bytes = bytes.fromhex(img_json_data['public key'])
                    memmove(addressof(pubkey), pubkey_bytes, len(pubkey_bytes))
            if hash:
                if 'public key hash' in img_json_keys:
                    hash_bytes = bytes.fromhex(img_json_data['public key hash'])
                    memmove(addressof(hash), hash_bytes, len(hash_bytes))

        return 0

    def create_manifest(self):
        manifest = Manifest_TypeDef()
        memset(addressof(manifest), 0xFF, sizeof(manifest))
        # parse manifest.json
        ret = self.parse_manifest_json(self.argv[2], self.argv[6], manifest)
        if (ret != 0) :
            print('Fail to parse manifest json, ret: ', ret)
            print('Fail to create manifest')
            return
        ret = self.gen_image_size(self.argv[4], manifest)
        if (ret != 0) :
            print("Fail to gen image size, ret: ", ret)
            print('Fail to create manifest')
            return
        if self.SecureBootEn:
            lib_security = importlib.import_module('security')
            sboot = lib_security.secure_boot(self.argc, self.argv)
            # parse key.json
            ret = self.parse_key_json(self.argv[3], self.argv[6], manifest, 1, manifest.SBPubKey, 0)
            if (ret != 0) :
                print("Fail to parse key json, ret: ", ret)
                return
            auth_alg_id = sboot.gen_auth_id(self.algorithm)
            if auth_alg_id == -1:
                return
            manifest.AuthAlg = auth_alg_id
            if 0 != sboot.gen_hash_id(manifest, self.MdType):
                return
            # gen image hash
            ret = sboot.gen_image_hash(self.argv[4], manifest.ImgHash)
            if (ret != 0) :
                print("Fail to gen image hash, ret: ", ret)
                return

            # gen signature
            ret = sboot.gen_signature(manifest.AuthAlg, self.PrivKey, manifest.SBPubKey, manifest, sizeof(Manifest_TypeDef) - SIGN_MAX_LEN, manifest.Signature)
            if (ret != 0) :
                print("Fail to gen signature, ret: %d\n", ret)
                return
        with open(self.argv[5], 'wb') as f:
            f.write(string_at(addressof(manifest), sizeof(Manifest_TypeDef)))
            new_size = ((sizeof(Manifest_TypeDef) - 1) // 4096 + 1) * 4096
            padcount = new_size - sizeof(Manifest_TypeDef)

            for _ in range(padcount):
                f.write(b'\xFF')
        return
        
    def create_certificate(self):
        cert = Certificate_TypeDef()
        memset(addressof(cert), 0xFF, sizeof(cert))

        # parse certificate json
        ret = self.parse_cert_json(self.argv[2], 'app', cert)
        if ret != 0:
            print('Fail to parse certificate json, ret: ', ret)
            return

        entry_num = (self.argc - 5) // 2
        entry_size = entry_num * sizeof(CertEntry_TypeDef)
        cert_entries = []
        for _ in range(0, entry_num):
            entry = CertEntry_TypeDef()
            memset(addressof(entry), 0xFF, sizeof(entry))
            cert_entries.append(entry)

        cert_size = sizeof(Certificate_TypeDef) - sizeof(CertEntry_TypeDef) * 5 - SIGN_MAX_LEN
        cert.TableSize = cert_size + entry_size
        cert.EntryNum = entry_num

        # parse certificate entry json
        for i in range(0, entry_num):
            cert_entries[i].KeyID = int(self.argv[5 + i * 2])
            ret = self.parse_key_json(self.argv[3], self.argv[5 + i * 2 + 1], 0, 0, 0, cert_entries[i].Hash)
            if ret != 0:
                print('Fail to parse key entry json, ret: ', ret)
                return
        if self.SecureBootEn:
            lib_security = importlib.import_module('security')
            sboot = lib_security.secure_boot(self.argc, self.argv)
            # parse signing key json
            ret = self.parse_key_json(self.argv[3], 'cert', cert, 1, cert.SBPubKey, 0)
            if ret != 0:
                print('Fail to parse key json, ret: ', ret)
                return
            auth_alg_id = sboot.gen_auth_id(self.algorithm)
            if auth_alg_id == -1:
                return
            cert.AuthAlg = auth_alg_id
            if 0 != sboot.gen_hash_id(cert, self.MdType):
                return
            for i in range(0, entry_num):
                memmove(byref(cert, cert_size + i * sizeof(cert_entries[i])), addressof(cert_entries[i]), sizeof(cert_entries[i]))
            # gen signature
            ret = sboot.gen_signature(cert.AuthAlg, self.PrivKey, cert.SBPubKey, cert, cert.TableSize, cert.Signature)
            if ret != 0:
                print('Fail to gen signature, ret: ', ret)
                return

        with open(self.argv[4], 'wb') as f:
            f.write(string_at(addressof(cert), cert_size))
            for i in range(0, entry_num):
                f.write(string_at(addressof(cert_entries[i]), sizeof(cert_entries[i])))
            f.write(string_at(addressof(cert.Signature), SIGN_MAX_LEN))
            padcount = 4096 - cert.TableSize - SIGN_MAX_LEN

            for _ in range(padcount):
                f.write(b'\xFF')
        return

    def huk_derivation(self):
        HUK = self.argv[2] # Hardware Unique Key
        SecEpoch = int(self.argv[3])
        print('HUK: ', HUK)
        print('SecEpoch: ', SecEpoch)
        DerivedKey = init_list(16)
        lib_security = importlib.import_module('security')
        sboot = lib_security.secure_boot(self.argc, self.argv)
        ret = sboot.tls_prf_sha256(HUK, "secret derivation", SecEpoch, DerivedKey)
        if ret < 0:
            print("HUK derivation fail, ret = ", ret)
        else:
            print("HUK derivation success\nDerivation Key: ", list_to_hex_str(DerivedKey))

class RSIP():
    def __init__(self, argc, argv):
        self.argc = argc
        self.argv = argv
        self.RsipEn = 0

    def RSIP_GetInfo(self, filename, imgtypename):
        with open(filename, 'r') as f:
            content = f.read()
            json_data = json.loads(content)
        json_keys = json_data.keys()
        if 'RSIP_EN' in json_keys:
            self.RsipEn = json_data['RSIP_EN']
        return 0

    def RSIP_ImageEncrypt(self):
        ret = self.RSIP_GetInfo(self.argv[5], self.argv[6])
        if ret != 0:
            print('Fail to get info, ret: ', ret)
            return
        if self.RsipEn:
            lib_security = importlib.import_module('security')
            rsip = lib_security.RSIP(self.argc, self.argv)
            rsip.RSIP_ImageEncrypt()
        else:
            fw = open(self.argv[3], 'wb')
            with open(self.argv[2], 'rb') as f:
                buf = f.read()
                fw.write(buf)
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
        return 0

    def rdp_encrypt(self):
        ret = self.rdp_get_info(self.argv[5], 'app')
        if ret != 0:
            return
        if self.rdp_enc:
            lib_security = importlib.import_module('security')
            rdp = lib_security.RDP(self.argc, self.argv)
            rdp.rdp_encrypt()
        else:
            fw = open(self.argv[4], 'wb')
            # print(self.argv[4])
            with open(self.argv[3], 'rb') as f:
                buf = f.read()
                fw.write(buf)
            fw.close()
        return

class PADTOOL():
    def __init__(self, argc, argv):
        self.argc = argc
        self.argv = argv

    def pad(self):
        # Get Parameters
        IMAGE_FILENAME=self.argv[2]
        Aligned_Bytes=int(self.argv[3], 10)
        if os.path.exists(IMAGE_FILENAME):
            file_size = os.path.getsize(IMAGE_FILENAME)
            if file_size == 0:
                padcount = 0
            else:
                new_size = ((file_size - 1) // Aligned_Bytes + 1) * Aligned_Bytes
                padcount = new_size - file_size

            with open(IMAGE_FILENAME, "ab+") as file:
                for _ in range(padcount):
                    file.write(b'\xFF')

def CATFILE(header, dst, *src):
    with open(dst, "wb") as dst_fd:
        if header != '':
            dst_fd.write(header)
        for isrc in src:
            if os.path.exists(isrc):
                with open(isrc, "rb") as f:
                    content = f.read()
                    dst_fd.write(content)
class PREPEND_TOOL:
    def __init__(self, argc, argv):
        self.argc = argc
        self.argv = argv
    def grep(self, pattern, src):
        with open(src, 'r') as symbol_file:
            content = symbol_file.read()
            pat = r"[0-9a-fA-F]+\s+\D\s+"+pattern
            line = re.findall(pat, content)
            if len(line) > 0:
                return line[0].strip().split()[0]
        return '0x0'

    def prepend_header(self):
        # Constant Variables
        # Constant Variables
        PATTERN_1=0x99999696
        PATTERN_2=0x3FCC66FC
        RSVD=0xFFFFFFFFFFFFFFFF
        IMG2SIGN=0x3831393538373131

        # Get Parameters
        IMAGE_FILENAME=self.argv[2]
        IMAGE_SECTION_START_NAME=self.argv[3]
        SYMBOL_LIST=self.argv[4]

        IMAGE_LEN = os.path.getsize(IMAGE_FILENAME)
        IMAGE_ADDR = self.grep(IMAGE_SECTION_START_NAME, SYMBOL_LIST)

        IMAGE_FILENAME_NEW=os.path.basename(IMAGE_FILENAME)
        IMAGE_FILENAME_PREPEND = os.path.join(os.path.dirname(IMAGE_FILENAME), 
                                            os.path.splitext(IMAGE_FILENAME_NEW)[0] + '_prepend' + os.path.splitext(IMAGE_FILENAME_NEW)[1])
        # print(IMAGE_FILENAME_PREPEND)

        HEADER_FINAL=''
        if IMAGE_FILENAME_NEW == "ram_1.bin" or IMAGE_FILENAME_NEW == "xip_boot.bin" or IMAGE_FILENAME_NEW == "entry_1.bin":
            HEADER_FINAL = PATTERN_1.to_bytes(4, 'big') + \
                        PATTERN_2.to_bytes(4, 'big')
        else:
            HEADER_FINAL = IMG2SIGN.to_bytes(8, 'big')

        HEADER_FINAL = HEADER_FINAL + IMAGE_LEN.to_bytes(4, 'little') + int(IMAGE_ADDR, 16).to_bytes(4, 'little')
        HEADER_FINAL = HEADER_FINAL + RSVD.to_bytes(8, 'little')
        HEADER_FINAL = HEADER_FINAL + RSVD.to_bytes(8, 'little')

        CATFILE(HEADER_FINAL, IMAGE_FILENAME_PREPEND, IMAGE_FILENAME)

class OTA_PREPEND_TOOL():
    # Constant Variables
    VERSION = 0xFFFFFFFF
    OTA_NUM = 1
    OTA_SIGN = 0x4F544131
    HEADER_LEN = 24
    CHECKSUM = 0
    OFFSET = 32
    IMAGE_ID=0xFFFFFFFF
    HEADER_FINAL = b''
    def __init__(self, argc, argv):
        self.argc = argc
        self.argv = argv

    def MAKEHEADER(self, init, image_len):
        if init == 1:
            self.HEADER_FINAL = self.VERSION.to_bytes(4, 'little') + self.OTA_NUM.to_bytes(4, 'little')
        else:
            self.HEADER_FINAL = self.HEADER_FINAL + \
                            self.OTA_SIGN.to_bytes(4, 'big') + \
                            self.HEADER_LEN.to_bytes(4, 'little') + \
                            self.CHECKSUM.to_bytes(4, 'little') + \
                            image_len.to_bytes(4, 'little') + \
                            self.OFFSET.to_bytes(4, 'little') + \
                            self.IMAGE_ID.to_bytes(4, 'little')

    def CHECKSUMTOOL(self, src, dst, offset):
        if os.path.exists(src) == False:
            return
        with open(src, "rb") as f:
            content = f.read()
            chksum = sum(content)
            print("image_size=", os.path.getsize(src))
            print("checksum={:08x}".format(chksum))
        with open(dst, "rb+") as f:
            f.seek(offset, 0)
            f.write(chksum.to_bytes(4, 'little'))

    def ota_prepend_header(self):
        # Get Parameters
        IMAGE_FILENAME_1=""
        IMAGE_FILENAME_2=""
        IMAGE_FILENAME_1_NEW=""
        IMAGE_FILENAME_2_NEW=""
        IMAGE_LEN = 0
        IMAGE_LEN_1=0
        IMAGE_LEN_2=0
        ENDFLAG=0
        IMAGE2_FILENAME = self.argv[2]

        if self.argc == 4:
            IMAGE_FILENAME_1 = self.argv[3]
            IMAGE_FILENAME_1_NEW = os.path.basename(IMAGE_FILENAME_1)
            self.OFFSET = 56
            self.OTA_NUM = 2
        elif self.argc == 5:
            IMAGE_FILENAME_1 = self.argv[3]
            IMAGE_FILENAME_2 = self.argv[4]
            IMAGE_FILENAME_1_NEW = os.path.basename(IMAGE_FILENAME_1)
            IMAGE_FILENAME_2_NEW = os.path.basename(IMAGE_FILENAME_2)
            self.OFFSET = 80
            self.OTA_NUM = 3

        self.MAKEHEADER(1, 0)
        CURR_PATH = os.path.dirname(IMAGE2_FILENAME)
        IMAGE_FILENAME_PREPEND = os.path.join(CURR_PATH, "ota_all.bin")
        IMAGE_FILENAME_NEW = os.path.basename(IMAGE2_FILENAME)
        IMAGE_LEN = os.path.getsize(IMAGE2_FILENAME)

        if IMAGE_FILENAME_NEW.endswith("app_ns.bin"):
            IMAGE_FILENAME_PREPEND = os.path.join(CURR_PATH, "ota_all_ns.bin")

        # make bootloader header
        if IMAGE_FILENAME_1_NEW == "km4_boot_all.bin" or IMAGE_FILENAME_1_NEW == "km4_boot_all_ns.bin":
            self.IMAGE_ID = 0
            IMAGE_LEN_1 = os.path.getsize(IMAGE_FILENAME_1)
            self.MAKEHEADER(0, IMAGE_LEN_1)

        # make application header
        if IMAGE_FILENAME_NEW.endswith("app_ns.bin") or IMAGE_FILENAME_NEW.endswith("app.bin"):
            self.IMAGE_ID = 1
            self.OFFSET = self.OFFSET + IMAGE_LEN_1
            self.MAKEHEADER(0, IMAGE_LEN)

        # make dsp header
        if IMAGE_FILENAME_1_NEW == "dsp_all.bin":
            self.IMAGE_ID = 2
            self.OFFSET = self.OFFSET + IMAGE_LEN
            IMAGE_LEN_1 = os.path.getsize(IMAGE_FILENAME_1)
            self.MAKEHEADER(0, IMAGE_LEN_1)
            ENDFLAG = 1

        #echo "$ENDFLAG"
        if ENDFLAG != 1:
            if IMAGE_FILENAME_2_NEW == "dsp_all.bin":
                self.IMAGE_ID = 2
                OFFSET = OFFSET + IMAGE_LEN
                IMAGE_LEN_2 = os.path.getsize(IMAGE_FILENAME_2)
                self.MAKEHEADER(0, IMAGE_LEN_2)

        if self.argc == 4:
            if IMAGE_FILENAME_1_NEW == "km4_boot_all.bin" or IMAGE_FILENAME_1_NEW == "km4_boot_all_ns.bin":
                CATFILE(self.HEADER_FINAL, IMAGE_FILENAME_PREPEND, IMAGE_FILENAME_1, IMAGE2_FILENAME, "")
                self.CHECKSUMTOOL(IMAGE_FILENAME_1, IMAGE_FILENAME_PREPEND, 16)
                self.CHECKSUMTOOL(IMAGE2_FILENAME, IMAGE_FILENAME_PREPEND, 40)
            else:
                CATFILE(self.HEADER_FINAL, IMAGE_FILENAME_PREPEND, IMAGE2_FILENAME, IMAGE_FILENAME_1, "")
                self.CHECKSUMTOOL(IMAGE2_FILENAME, IMAGE_FILENAME_PREPEND, 16)
                self.CHECKSUMTOOL(IMAGE_FILENAME_1, IMAGE_FILENAME_PREPEND, 40)
        elif self.argc - 1 == 4:
            if IMAGE_FILENAME_1_NEW == "km4_boot_all.bin" or IMAGE_FILENAME_1_NEW == "km4_boot_all_ns.bin":
                CATFILE(self.HEADER_FINAL, IMAGE_FILENAME_PREPEND, IMAGE_FILENAME_1, IMAGE2_FILENAME, IMAGE_FILENAME_2)
                self.CHECKSUMTOOL(IMAGE_FILENAME_1, IMAGE_FILENAME_PREPEND, 16)
                self.CHECKSUMTOOL(IMAGE2_FILENAME, IMAGE_FILENAME_PREPEND, 40)
                self.CHECKSUMTOOL(IMAGE_FILENAME_2, IMAGE_FILENAME_PREPEND, 64)
            else:
                CATFILE(self.HEADER_FINAL, IMAGE_FILENAME_PREPEND, IMAGE2_FILENAME, IMAGE_FILENAME_1, IMAGE_FILENAME_2)
                self.CHECKSUMTOOL(IMAGE2_FILENAME, IMAGE_FILENAME_PREPEND, 16)
                self.CHECKSUMTOOL(IMAGE_FILENAME_1, IMAGE_FILENAME_PREPEND, 40)
                self.CHECKSUMTOOL(IMAGE_FILENAME_2, IMAGE_FILENAME_PREPEND, 64)
        else:
            CATFILE(self.HEADER_FINAL, IMAGE_FILENAME_PREPEND, IMAGE2_FILENAME, "", "")
            self.CHECKSUMTOOL(IMAGE2_FILENAME, IMAGE_FILENAME_PREPEND, 16)
def print_help():
    print("\tpython axf2bin.py keypair <auth_alg> <filename>")
    print("\tpython axf2bin.py manifest <manifest.json> <key.json> <img_file> <out_file> [app|boot]")
    print("\tpython axf2bin.py cert <manifest.json> <key.json> <out_file> <key_id1> <key1_name> <key_id2> <key2_name>...")
    print("\tpython axf2bin.py rsip <src.bin> <dst.bin> <virtual_addr> <manifest.json> [app|boot]")
    print("\tpython axf2bin.py rdp [enc|dec] <img_file> <out_file> <manifest.json>, Actual IV is composed by app RSIP_IV + RDP_IV")
    print("\tpython axf2bin.py pad [Image Name] [Aligned Bytes(decimal)]")
    print("\tpython axf2bin.py prepend_header [Image Name] [Start Symbol Name] [Symbols List File]")
    print("\tpython axf2bin.py ota_prepend_header [Image2 Name] [Boot Image Name][option] [DSP Image Name][option]")
    print("\tpython axf2bin.py imagetool [Image Name] [Build type] [DSP Image DIR][option]")
    print("\tpython axf2bin.py binary_pading [Src Image Name] [Dst Image Name] [Size]")

def ENCTOOL(key, *value):
    argv = (0, 0) + value
    if key == "keypair":
        if len(argv) < 4:
            print_help()
            sys.exit()
        sboot = secure_boot(len(argv), argv)
        sboot.create_keypair()
    elif key == "manifest":
        if len(argv) < 7:
            print_help()
            sys.exit()
        sboot = secure_boot(len(argv), argv)
        sboot.create_manifest()
    elif key == "cert":
        if len(argv) < 5 or len(argv)%2 == 0:
            print_help()
            sys.exit()
        sboot = secure_boot(len(argv), argv)
        sboot.create_certificate()
    elif key == "rsip":
        if len(argv) < 7:
            print_help()
            sys.exit()
        rsip = RSIP(len(argv), argv)
        rsip.RSIP_ImageEncrypt()
    elif key == "rdp":
        if len(argv) < 6:
            print_help()
            sys.exit()
        rdp = RDP(len(argv), argv)
        rdp.rdp_encrypt()
    elif key == "huk":
        sboot = secure_boot(len(argv), argv)
        sboot.huk_derivation()

class IMAGETOOL():
    IMAGE_FILENAME = ''
    BUILD_TYPE = ''
    MANIFEST_JSON = ''
    manifest = ''
    cert = ''
    pwd = ''
    def __init__(self, argc, argv):
        self.argc = argc
        self.argv = argv

    def binary_pading(self, src, dst, size):
        print('size, ', size)
        with open(src, 'rb') as fd:
            tmp = fd.read(1024 * size)
            with open('temp.bin', 'wb') as fw:
                fw.write(tmp)
                print('%d bytes copied'%(len(tmp)))
                for _ in range(1024 * size):
                    fw.write(b'\x00')
                print('%d bytes copied'%(1024 * size))
        with open('temp.bin', 'rb') as fd:
            tmp = fd.read(1024 * size)
            with open(dst, 'wb') as fw:
                fw.write(tmp)
                print('%d bytes copied'%(len(tmp)))
        os.remove('temp.bin')
    
    def image2_prehandle(self, image2_1, image2_2, image3, app, image_dir):
        if os.path.exists(image2_1):
            shutil.copy(image2_1, image_dir)
        image2_1 = os.path.join(image_dir, os.path.basename(image2_1))
        if os.path.exists(image2_2) == False:
            sys.exit()
        if os.path.exists(image2_1) == False:
            sys.exit()
        if os.path.exists(app):
            os.remove(app)
        if os.path.exists(image3):
            CATFILE('', app, image2_1, image2_2, image3)
        else:
            CATFILE('', app, image2_1, image2_2)
        return image2_1
    
    def image2_posthandle(self, image_dir, *apps):
        if self.BUILD_TYPE != 'MFG':
            return
        for app in apps:
            if os.path.exists(app):
                app_mp = os.path.join(image_dir, os.path.splitext(os.path.basename(app))[0] + '_mp.bin')
                os.rename(app, app_mp)

    def amebalite(self, KM4_IMG_DIR):
        DSP_IMG_DIR = ''
        if self.argc > 4:
            DSP_IMG_DIR = self.argv[4]
        if self.BUILD_TYPE == 'MFG':
            KR4_IMG_DIR = os.path.join(self.pwd, 'project_kr4', 'vsdk', 'image_mp')
        else:
            KR4_IMG_DIR = os.path.join(self.pwd, 'project_kr4', 'vsdk', 'image')
        kr4_image2 = os.path.join(KR4_IMG_DIR, 'kr4_image2_all.bin')
        km4_image2 = os.path.join(KM4_IMG_DIR, 'km4_image2_all.bin')
        km4_image3 = os.path.join(KM4_IMG_DIR, 'km4_image3_all.bin')
        kr4_image2_en = os.path.join(KM4_IMG_DIR, 'kr4_image2_all_en.bin')
        km4_image2_en = os.path.join(KM4_IMG_DIR, 'km4_image2_all_en.bin')
        km4_image3_en = os.path.join(KM4_IMG_DIR, 'km4_image3_all_en.bin')
        dsp_image = os.path.join(DSP_IMG_DIR, 'dsp.bin')
        dsp_image_en = os.path.join(DSP_IMG_DIR, 'dsp_en.bin')
        app = os.path.join(KM4_IMG_DIR, 'kr4_km4_app.bin')
        app_ns = os.path.join(KM4_IMG_DIR, 'kr4_km4_app_ns.bin')
        app_dsp = os.path.join(KM4_IMG_DIR, 'kr4_km4_dsp_app.bin')
        app_dsp_ns = os.path.join(KM4_IMG_DIR, 'kr4_km4_dsp_app_ns.bin')

        if self.IMAGE_FILENAME == 'kr4_image2_all.bin' or self.IMAGE_FILENAME == 'km4_image2_all.bin':
            kr4_image2 = self.image2_prehandle(kr4_image2, km4_image2, km4_image3, app, KM4_IMG_DIR)

            ENCTOOL('cert', self.MANIFEST_JSON, self.MANIFEST_JSON, self.cert, 0, 'app')
            if os.path.exists(dsp_image):
                CATFILE('', app_dsp, app, dsp_image)
                ENCTOOL('manifest', self.MANIFEST_JSON, self.MANIFEST_JSON, app_dsp, self.manifest, 'app')
                ENCTOOL('rsip', dsp_image, dsp_image_en, '0x0D000000', self.MANIFEST_JSON, 'app')
            else:
                ENCTOOL('manifest', self.MANIFEST_JSON, self.MANIFEST_JSON, app, self.manifest, 'app')
            ENCTOOL('rsip', kr4_image2, kr4_image2_en, '0x0C000000', self.MANIFEST_JSON, 'app')
            ENCTOOL('rsip', km4_image2, km4_image2_en, '0x0E000000', self.MANIFEST_JSON, 'app')

            if os.path.exists(self.manifest) == False:
                sys.exit()

            if os.path.exists(km4_image3_en):
                CATFILE('', app_ns, self.cert, self.manifest, kr4_image2, km4_image2, km4_image3)
                CATFILE('', app, self.cert, self.manifest, kr4_image2_en, km4_image2_en, km4_image3_en)
            else:
                CATFILE('', app_ns, self.cert, self.manifest, kr4_image2, km4_image2)
                CATFILE('', app, self.cert, self.manifest, kr4_image2_en, km4_image2_en)
            
            if os.path.exists(dsp_image_en):
                CATFILE('', app_dsp_ns, app_ns, dsp_image)
                CATFILE('', app_dsp, app, dsp_image_en)
                os.remove(app_ns)
                os.remove(app)
            else:
                if os.path.exists(app_dsp_ns):
                    os.remove(app_dsp_ns)
                if os.path.exists(app_dsp):
                    os.remove(app_dsp)

            self.image2_posthandle(KM4_IMG_DIR, app, app_ns, app_dsp, app_dsp_ns)

        if self.IMAGE_FILENAME == 'kr4_image2_all_shrink.bin' or self.IMAGE_FILENAME == 'km4_image2_all_shrink.bin':
            if os.path.exists(os.path.join(KR4_IMG_DIR, 'kr4_image2_all_shrink.bin')):
                shutil.copy(os.path.join(KR4_IMG_DIR, 'kr4_image2_all_shrink.bin'), KM4_IMG_DIR)
            if os.path.exists(os.path.join(KM4_IMG_DIR, 'kr4_image2_all_shrink.bin')) == False:
                sys.exit()
            if os.path.exists(os.path.join(KM4_IMG_DIR, 'km4_image2_all_shrink.bin')) == False:
                sys.exit()
            CATFILE('', app, os.path.join(KM4_IMG_DIR, 'km4_image2_all_shrink.bin'),os.path.join(KM4_IMG_DIR, 'kr4_image2_all_shrink.bin'))
            if self.BUILD_TYPE == 'MFG':
                if os.path.exists(app):
                    os.rename(app, os.path.join(KM4_IMG_DIR, 'kr4_km4_app_mp.bin'))

    def amebadplus(self, KM4_IMG_DIR):
        if self.BUILD_TYPE == 'MFG':
            KM0_IMG_DIR = os.path.join(self.pwd, 'project_km0', 'asdk', 'image_mp')
        else:
            KM0_IMG_DIR = os.path.join(self.pwd, 'project_km0', 'asdk', 'image')
        km0_image2 = os.path.join(KM0_IMG_DIR, 'km0_image2_all.bin')
        km4_image2 = os.path.join(KM4_IMG_DIR, 'km4_image2_all.bin')
        km4_image3 = os.path.join(KM4_IMG_DIR, 'km4_image3_all.bin')
        km0_image2_en = os.path.join(KM4_IMG_DIR, 'km0_image2_all_en.bin')
        km4_image2_en = os.path.join(KM4_IMG_DIR, 'km4_image2_all_en.bin')
        km4_image3_en = os.path.join(KM4_IMG_DIR, 'km4_image3_all_en.bin')
        app = os.path.join(KM4_IMG_DIR, 'km0_km4_app.bin')
        app_ns = os.path.join(KM4_IMG_DIR, 'km0_km4_app_ns.bin')

        if self.IMAGE_FILENAME == 'km0_image2_all.bin' or self.IMAGE_FILENAME == 'km4_image2_all.bin':
            km0_image2 = self.image2_prehandle(km0_image2, km4_image2, km4_image3, app, KM4_IMG_DIR)

            ENCTOOL('cert', self.MANIFEST_JSON, self.MANIFEST_JSON, self.cert, 0, 'app')
            ENCTOOL('manifest', self.MANIFEST_JSON, self.MANIFEST_JSON, app, self.manifest, 'app')
            ENCTOOL('rsip', km0_image2, km0_image2_en, '0x0C000000', self.MANIFEST_JSON, 'app')
            ENCTOOL('rsip', km4_image2, km4_image2_en, '0x0E000000', self.MANIFEST_JSON, 'app')

            if os.path.exists(self.manifest) == False:
                sys.exit()

            if os.path.exists(km4_image3_en):
                CATFILE('', app_ns, self.cert, self.manifest, km0_image2, km4_image2, km4_image3)
                CATFILE('', app, self.cert, self.manifest, km0_image2_en, km4_image2_en, km4_image3_en)
            else:
                CATFILE('', app_ns, self.cert, self.manifest, km0_image2, km4_image2)
                CATFILE('', app, self.cert, self.manifest, km0_image2_en, km4_image2_en)

            self.image2_posthandle(KM4_IMG_DIR, app, app_ns)
    
        if self.IMAGE_FILENAME == 'km0_image2_all_shrink.bin' or self.IMAGE_FILENAME == 'km4_image2_all_shrink.bin':
            if os.path.exists(os.path.join(KM0_IMG_DIR, 'km0_image2_all_shrink.bin')):
                shutil.copy(os.path.join(KM0_IMG_DIR, 'km0_image2_all_shrink.bin'), KM4_IMG_DIR)
            if os.path.exists(os.path.join(KM4_IMG_DIR, 'km0_image2_all_shrink.bin')) == False:
                sys.exit()
            if os.path.exists(os.path.join(KM4_IMG_DIR, 'km4_image2_all_shrink.bin')) == False:
                sys.exit()
            if os.path.exists(app):
                os.remove(app)
            
            self.binary_pading(os.path.join(KM4_IMG_DIR, 'km4_image2_all_shrink.bin'), os.path.join(KM4_IMG_DIR, 'km4_image2_all_shrink_pad.bin'), 150)
            CATFILE('', app, os.path.join(KM4_IMG_DIR, 'km4_image2_all_shrink_pad.bin'), os.path.join(KM4_IMG_DIR, 'km0_image2_all_shrink.bin'))
            
            self.image2_posthandle(KM4_IMG_DIR, app)
    
    def amebasmart(self, KM4_IMG_DIR):
        if self.BUILD_TYPE == 'MFG':
            CA32_IMG_DIR = os.path.join(self.pwd, 'project_ap', 'asdk', 'image_mp')
        else:
            CA32_IMG_DIR = os.path.join(self.pwd, 'project_ap', 'asdk', 'image')
        KM0_IMG_DIR = os.path.join(self.pwd, 'project_lp', 'asdk', 'image')
        km0_image2 = os.path.join(KM0_IMG_DIR, 'km0_image2_all.bin')
        km4_image2 = os.path.join(KM4_IMG_DIR, 'km4_image2_all.bin')
        km4_image3 = os.path.join(KM4_IMG_DIR, 'km4_image3_all.bin')
        km0_image2_en = os.path.join(KM4_IMG_DIR, 'km0_image2_all_en.bin')
        km4_image2_en = os.path.join(KM4_IMG_DIR, 'km4_image2_all_en.bin')
        km4_image3_en = os.path.join(KM4_IMG_DIR, 'km4_image3_all_en.bin')
        ca32_image = os.path.join(CA32_IMG_DIR, 'ap_image_all.bin')
        ca32_image_en = os.path.join(CA32_IMG_DIR, 'ap_image_all_en.bin')
        app = os.path.join(KM4_IMG_DIR, 'km0_km4_app.bin')
        app_ns = os.path.join(KM4_IMG_DIR, 'km0_km4_app_ns.bin')
        app_ca32 = os.path.join(KM4_IMG_DIR, 'km0_km4_ca32_app.bin')
        app_ca32_ns = os.path.join(KM4_IMG_DIR, 'km0_km4_ca32_app_ns.bin')

        if self.IMAGE_FILENAME == 'km0_image2_all.bin' or self.IMAGE_FILENAME == 'km4_image2_all.bin':
            km0_image2 = self.image2_prehandle(km0_image2, km4_image2, km4_image3, app, KM4_IMG_DIR)
            
            ENCTOOL('cert', self.MANIFEST_JSON, self.MANIFEST_JSON, self.cert, 0, 'app')
            if os.path.exists(ca32_image):
                CATFILE('', app_ca32, app, ca32_image)
                ENCTOOL('manifest', self.MANIFEST_JSON, self.MANIFEST_JSON, app_ca32, self.manifest, 'app')
            else:
                ENCTOOL('manifest', self.MANIFEST_JSON, self.MANIFEST_JSON, app, self.manifest, 'app')
            ENCTOOL('rsip', km0_image2, km0_image2_en, '0x0C000000', self.MANIFEST_JSON, 'app')
            ENCTOOL('rsip', km4_image2, km4_image2_en, '0x0D000000', self.MANIFEST_JSON, 'app')

            if os.path.exists(self.manifest) == False:
                sys.exit()

            if os.path.exists(km4_image3_en):
                CATFILE('', app_ns, self.cert, self.manifest, km0_image2, km4_image2, km4_image3)
                CATFILE('', app, self.cert, self.manifest, km0_image2_en, km4_image2_en, km4_image3_en)
            else:
                CATFILE('', app_ns, self.cert, self.manifest, km0_image2, km4_image2)
                CATFILE('', app, self.cert, self.manifest, km0_image2_en, km4_image2_en)
            
            if os.path.exists(ca32_image_en):
                CATFILE('', app_ca32_ns, app_ns, ca32_image)
                CATFILE('', app_ca32, app, ca32_image_en)

            self.image2_posthandle(KM4_IMG_DIR, app, app_ns, app_ca32, app_ca32_ns)

        if self.IMAGE_FILENAME == 'ap_image_all.bin':
            if os.path.exists(ca32_image):
                ENCTOOL('rsip', ca32_image, ca32_image_en, '0x0E000000', self.MANIFEST_JSON, 'app')
    
    def amebagreen2(self, AP_IMG_DIR):
        if self.BUILD_TYPE == 'MFG':
            NP_IMG_DIR = os.path.join(self.pwd, 'project_np', 'asdk', 'image_mp')
        else:
            NP_IMG_DIR = os.path.join(self.pwd, 'project_np', 'asdk', 'image')
        np_image2 = os.path.join(NP_IMG_DIR, 'np_image2_all.bin')
        ap_image2 = os.path.join(AP_IMG_DIR, 'ap_image2_all.bin')
        ap_image3 = os.path.join(AP_IMG_DIR, 'ap_image3_all.bin')
        np_image2_en = os.path.join(AP_IMG_DIR, 'np_image2_all_en.bin')
        ap_image2_en = os.path.join(AP_IMG_DIR, 'ap_image2_all_en.bin')
        ap_image3_en = os.path.join(AP_IMG_DIR, 'ap_image3_all_en.bin')
        app = os.path.join(AP_IMG_DIR, 'amebagreen2_app.bin')
        app_ns = os.path.join(AP_IMG_DIR, 'amebagreen2_app_ns.bin')

        if self.IMAGE_FILENAME == 'np_image2_all.bin' or self.IMAGE_FILENAME == 'ap_image2_all.bin':
            np_image2 = self.image2_prehandle(np_image2, ap_image2, ap_image3, app, AP_IMG_DIR)

            ENCTOOL('cert', self.MANIFEST_JSON, self.MANIFEST_JSON, self.cert, 0, 'app')
            ENCTOOL('manifest', self.MANIFEST_JSON, self.MANIFEST_JSON, app, self.manifest, 'app')
            ENCTOOL('rsip', np_image2, np_image2_en, '0x06000000', self.MANIFEST_JSON, 'app')
            ENCTOOL('rsip', ap_image2, ap_image2_en, '0x04000000', self.MANIFEST_JSON, 'app')

            if os.path.exists(self.manifest) == False:
                sys.exit()

            if os.path.exists(ap_image3_en):
                CATFILE('', app_ns, self.cert, self.manifest, np_image2, ap_image2, ap_image3)
                CATFILE('', app, self.cert, self.manifest, np_image2_en, ap_image2_en, ap_image3_en)
            else:
                CATFILE('', app_ns, self.cert, self.manifest, np_image2, ap_image2)
                CATFILE('', app, self.cert, self.manifest, np_image2_en, ap_image2_en)

            self.image2_posthandle(AP_IMG_DIR, app, app_ns)
    
        if self.IMAGE_FILENAME == 'ram_all_prepend.bin':
            ENCTOOL('manifest', self.MANIFEST_JSON, self.MANIFEST_JSON, os.path.join(AP_IMG_DIR, 'ram_all_prepend.bin'), self.manifest, 'boot')
            CATFILE('', os.path.join(AP_IMG_DIR, 'ap_fullmac.bin'), os.path.join(AP_IMG_DIR, 'ram_all_prepend.bin'), self.manifest)
    
    def get_handler(self, project):
        return {
            'amebasmart_gcc_project': {'handler': self.amebasmart, 'image_core': 'project_hp', 'boot_addr': '0x0A000000', 'boot_image': 'km4_boot_all'},
            'amebalite_gcc_project': {'handler': self.amebalite, 'image_core': 'project_km4', 'boot_addr': '0x0F800000', 'boot_image': 'km4_boot_all'},
            'amebadplus_gcc_project': {'handler': self.amebadplus, 'image_core': 'project_km4', 'boot_addr': '0x0F800000', 'boot_image': 'km4_boot_all'},
            'amebagreen2_gcc_project': {'handler': self.amebagreen2, 'image_core': 'project_ap', 'boot_addr': '0x02000000', 'boot_image': 'amebagreen2_boot'},
        }.get(project, None)

    def execute(self):
        lproject = re.findall(r"ameba[A-Za-z0-9]+_gcc_project", os.getcwd())
        if lproject == None or len(lproject) == 0:
            sys.exit()

        handler = self.get_handler(lproject[0])
        if handler == None:
            sys.exit()
        
        index = os.getcwd().find(lproject[0])
        self.pwd = os.path.join(os.getcwd()[0: index], lproject[0])

        self.IMAGE_FULLNAME = self.argv[2]
        self.IMAGE_FILENAME = os.path.basename(self.IMAGE_FULLNAME)
        self.BUILD_TYPE = self.argv[3]
        self.MANIFEST_JSON = os.path.join(self.pwd, 'manifest.json')

        if self.BUILD_TYPE == 'MFG':
            IMG_DIR = os.path.join(self.pwd, handler['image_core'], 'asdk', 'image_mp')
        else:
            IMG_DIR = os.path.join(self.pwd, handler['image_core'], 'asdk', 'image')
        if os.path.exists(IMG_DIR) == False:
            os.mkdir(IMG_DIR)

        self.cert = os.path.join(IMG_DIR, 'cert.bin')
        self.manifest = os.path.join(IMG_DIR, 'manifest.bin')

        handler['handler'](IMG_DIR)

        if self.IMAGE_FILENAME.find('boot') > 0:
            boot = self.IMAGE_FULLNAME
            boot_en = os.path.join(IMG_DIR, os.path.splitext(self.IMAGE_FILENAME)[0] + '_en.bin')
            boot_all = os.path.join(IMG_DIR, handler['boot_image'] + '.bin')
            boot_all_ns = os.path.join(IMG_DIR, handler['boot_image'] + '_ns.bin')
            ENCTOOL('manifest', self.MANIFEST_JSON, self.MANIFEST_JSON, boot, self.manifest, 'boot')
            ENCTOOL('rsip', boot, boot_en, handler['boot_addr'], self.MANIFEST_JSON, 'boot')
            CATFILE('', boot_all_ns, self.manifest, boot)
            CATFILE('', boot_all, self.manifest, boot_en)
            os.remove(boot_en)
        
        if self.IMAGE_FILENAME == 'ram_1_prepend.bin':
            ENCTOOL('manifest', self.MANIFEST_JSON, self.MANIFEST_JSON, os.path.join(IMG_DIR, 'ram_1_prepend.bin'), self.manifest, 'boot')
            CATFILE('', os.path.join(IMG_DIR, 'imgtool_flashloader.bin'), os.path.join(IMG_DIR, 'ram_1_prepend.bin'), self.manifest)
        
        if self.IMAGE_FILENAME.find('image3') > 0:
            image3_en = os.path.join(IMG_DIR, os.path.splitext(self.IMAGE_FILENAME)[0] + '_en.bin')
            ENCTOOL('rdp', 'enc', os.path.join(IMG_DIR, self.IMAGE_FILENAME), image3_en, self.MANIFEST_JSON)

if len(sys.argv) <= 1:
    print_help()
    sys.exit()

if sys.argv[1] == "pad":
    if len(sys.argv) < 4:
        print_help()
        sys.exit()
    padtool = PADTOOL(len(sys.argv), sys.argv)
    padtool.pad()
elif sys.argv[1] == 'prepend_header':
    if len(sys.argv) != 5:
        print_help()
        sys.exit()
    prependtool = PREPEND_TOOL(len(sys.argv), sys.argv)
    prependtool.prepend_header()
elif sys.argv[1] == 'ota_prepend_header':
    if len(sys.argv) < 3 or len(sys.argv) > 5:
        print_help()
        sys.exit()
    otaprependtool = OTA_PREPEND_TOOL(len(sys.argv), sys.argv)
    otaprependtool.ota_prepend_header()
elif sys.argv[1] == 'imagetool':
    if len(sys.argv) < 4:
        print_help()
        sys.exit()
    imagetool = IMAGETOOL(len(sys.argv), sys.argv)
    imagetool.execute()
elif sys.argv[1] == 'binary_pading':
    if len(sys.argv) < 5:
        print_help()
        sys.exit()
    imagetool = IMAGETOOL(len(sys.argv), sys.argv)
    imagetool.binary_pading(sys.argv[2], sys.argv[3], int(sys.argv[4]))
else:
    tmp = tuple(sys.argv[2:])
    ENCTOOL(sys.argv[1], *tmp)