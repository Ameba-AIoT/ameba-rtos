from abc import ABC
import json5
import json
import importlib
from ctypes import *
from enum import Enum, unique
from dataclasses import fields
from typing import Union, List

from ameba_enums import *
from context import Context
from utility import *


SIGN_MAX_LEN = 144
PKEY_MAX_LEN = 144
HASH_MAX_LEN = 64

FlashCalibPattern = [0x96969999, 0xFC66CC3F]
ImagePattern = [0x35393138, 0x31313738]
CompressFlag = [0x504D4F43, 0x53534552]

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
              ('Rsvd1',c_uint8 * 7),
              ('RsipCfg',c_uint8),
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

@dataclass(frozen=True)
class ManifestImageConfig:
    version:Union[int, None] = None

    img_id:Union[int, None] = None
    img_ver_major:Union[int, None] = None
    img_ver_minor:Union[int, None] = None
    sec_epoch:Union[int, None] = None

    secure_boot_en:Union[bool, None] = None
    hmac_key:Union[str, None] = None
    ecb_key:Union[str, List, None] = None
    ctr_key:Union[str, List, None] = None

    hash_alg:Union[str, None] = None

    rdp_en:Union[bool, None] = None
    rdp_iv:Union[bool, None] = None
    rdp_key:Union[bool, None] = None


    rsip_en:Union[bool, None] = None
    rsip_mode:Union[int, None] = None           # rsip_mode: 0 is CTR, 1 is XTS(CTR+ECB), 2 is GCM

    gcm_tag_len:Union[int, None] = None         # gcm_tag_len: use in GCM, should be 4, 8, 16
    rsip_key_id:Union[int, None] = None         # 0 use (ecb_key, ctr_key), 1 use (ecb_key1, ctr_key1),
    rsip_iv:Union[str, None] = None

    algorithm:Union[str, None] = None
    private_key:Union[str, None] = None
    public_key:Union[str, None] = None
    public_key_hash:Union[str, None] = None

    @classmethod
    def create(cls, config: dict) -> 'ManifestImageConfig':
        class_fields = {f.name for f in fields(cls)}
        invalid_fields = set(config.keys()) - class_fields
        if invalid_fields:
            raise ValueError(f"Invalid fields in config: {', '.join(invalid_fields)}")
        return cls(**config)

class ManifestManager(ABC):
    valid_algorithm = importlib.import_module('security').secure_boot.valid_algorithm

    def __init__(self, context: Context) -> None:
        super().__init__()
        self.context = context

        with open(context.manifest_file, 'r') as f:
            self.origin_json_data = json5.load(f)

        self.lib_security = importlib.import_module('security')
        self.sboot = self.lib_security.secure_boot()

        if not self.validate_config(self.origin_json_data):
            raise ValueError(f"Invalid JSON format")

        self.new_json_data = self.origin_json_data

        # Add key from outside(global config) of image part if key not in image part
        for img in ['image1', 'image2', 'image3']:
            if img not in self.new_json_data.keys():
                context.logger.info(f"manifest file does not contains {img}")
                continue
            for key, value in self.origin_json_data.items():
                if isinstance(value, dict): continue

                if key not in self.new_json_data[img].keys():
                    self.new_json_data[img][key] = value

        self.image1 = ManifestImageConfig.create(self.new_json_data['image1'])
        self.image2 = ManifestImageConfig.create(self.new_json_data['image2'])
        if 'image3' in self.new_json_data.keys():
            self.image3 = ManifestImageConfig.create(self.new_json_data['image3'])
        else:
            context.logger.info(f"manifest file does not contains image3, will use image2 config for image3")
            self.image3 = self.image2
        self.cert = ManifestImageConfig.create(self.new_json_data['cert'])

    def validate_config(self, data:Union[str, dict]) -> bool:
        if isinstance(data, str):
            with open(data, 'r') as f:
                jdata = json5.load(f)
        else:
            jdata = data

        for key, value in jdata.items():
            if isinstance(value, dict):
                res = self.validate_config(value)
                if res == False:
                    return False
                if value.get('rsip_en', False) and value.get('rdp_en', False):
                    self.context.logger.error(f'rsip_en and rdp_en cannot be enabled at same time')
                    return False
            else:
                if key in ["img_ver_major", "img_ver_minor"]:
                    img_id = jdata.get('img_id', None)
                    if img_id == ImgID.IMGID_BOOT.value:
                        if value > 0x7FFF:
                            self.context.logger.error(f'{key} should in range 0~32767!!!')
                            return False
                    else:
                        if value > 0xFFFF:
                            self.context.logger.error(f'{key} should in range 0~65535!!!')
                            return False
                elif key == "hmac_key":
                    if len(value) != 64:
                        self.context.logger.error(f'{key} format error: should be 64 bytes')
                        return False
                elif key in ["rsip_iv", "rdp_iv"]:
                    if len(value) != 16:
                        self.context.logger.error(f'{key} format error: should be 16 bytes')
                        return False
                elif key == "gcm_tag_len":
                    choices = [4, 8, 16]
                    if value not in choices:
                        self.context.logger.error(f'{key} format error: should be one of {choices}')
                        return False
                elif key == "rsip_mode":
                    #NOTE: amebad only support mode 0
                    choices = [0] if self.context.soc_project == "amebad" else [0, 1, 2]
                    if value not in choices:
                        self.context.logger.error(f'{key} format error: should be one of {choices}')
                        return False
                elif key == "rsip_key_id":
                    choices = [0, 1]
                    if value not in choices:
                        self.context.logger.error(f'{key} format error: should be one of {choices}')
                        return False
                elif key in ["ctr_key", "rdp_key"]:
                    if isinstance(value, list):
                        if any(len(v) != 64 and len(v) != 32 for v in value):
                            self.context.logger.error(f'{key} format error: should be 32/64 bytes')
                            return False
                    elif len(value) != 64 and len(value) != 32:
                        self.context.logger.error(f'{key} format error: should be 32/64 bytes')
                        return False
                elif key.startswith("ecb_key"):
                    if isinstance(value, list):
                        if any(len(v) != 64 for v in value):
                            self.context.logger.error(f'{key} format error: should be 64 bytes')
                            return False
                    elif len(value) != 64:
                        self.context.logger.error(f'{key} format error: should be 64 bytes')
                        return False
                elif key == "algorithm":
                    if value not in self.valid_algorithm:
                        self.context.logger.error(f'{key} format error: should be one of {self.valid_algorithm}')
                        return False
        return True

    def get_image_config(self, image_type:Union[ImageType, str]) -> ManifestImageConfig:
        if isinstance(image_type, str):
            return getattr(self, image_type.lower())
        else:
            return getattr(self, image_type.name.lower())

    def create_cert(self, output_file:str, *entry_pairs) -> Error:
        cert = Certificate_TypeDef()
        memset(addressof(cert), 0xFF, sizeof(cert))

        cert.Ver = self.image2.version
        cert.ImgID = self.image2.img_id
        ImagePattern_lsb = [htonl_to_ntohl(i) for i in ImagePattern]
        ImagePattern_bytes = bytes.fromhex(list_to_hex_str(ImagePattern_lsb))
        memmove(addressof(cert.Pattern), ImagePattern_bytes, sizeof(cert.Pattern))
        cert.MajorKeyVer = self.image2.img_ver_major
        cert.MinorKeyVer = self.image2.img_ver_minor

        entry_num = len(entry_pairs) // 2
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
            cert_entries[i].KeyID = int(entry_pairs[i * 2])
            entry_config = self.get_image_config(entry_pairs[i * 2 + 1])
            if entry_config.public_key_hash != None:
                hash_bytes = bytes.fromhex(entry_config.public_key_hash) #REVIEW: check key existance?
                memmove(addressof(cert_entries[i].Hash), hash_bytes, len(hash_bytes))

        if self.image2.secure_boot_en:
            # parse signing key json
            pubkey_bytes = bytes.fromhex(self.cert.public_key)
            memmove(addressof(cert.SBPubKey), pubkey_bytes, len(pubkey_bytes))

            cert.AuthAlg = self.sboot.gen_auth_id(self.cert.algorithm)
            if cert.AuthAlg == -1:
                return Error(ErrorType.UNKNOWN_ERROR, "self.sboot gen auth id failed")

            if self.sboot.gen_hash_id(cert, self.image2.hash_alg) != 0:
                return Error(ErrorType.UNKNOWN_ERROR, "self.sboot gen hash id failed")
            for i in range(0, entry_num):
                memmove(byref(cert, cert_size + i * sizeof(cert_entries[i])), addressof(cert_entries[i]), sizeof(cert_entries[i]))

            # gen signature
            self.sboot.HmacKey = self.image2.hmac_key
            self.sboot.HmacKeyLen = len(self.image2.hmac_key) // 2
            ret = self.sboot.gen_signature(cert.AuthAlg, self.cert.private_key, cert.SBPubKey, cert, cert.TableSize, cert.Signature)
            if ret != 0:
                return Error(ErrorType.UNKNOWN_ERROR, f"self.sboot gen signature failed: {ret}")

        with open(output_file, 'wb') as f:
            f.write(string_at(addressof(cert), cert_size))
            for i in range(0, entry_num):
                f.write(string_at(addressof(cert_entries[i]), sizeof(cert_entries[i])))
            f.write(string_at(addressof(cert.Signature), SIGN_MAX_LEN))

            pad_count = 4096 - cert.TableSize - SIGN_MAX_LEN
            f.write(b'\xFF' * pad_count)

        return Error.success()

    def create_manifest(self, output_file:str, input_file:str, img_type = ImageType.UNKNOWN, compress = False) -> Error:
        image_type = img_type if img_type != ImageType.UNKNOWN else parse_image_type(input_file)
        if compress:
            global ImagePattern
            ImagePattern[0] = CompressFlag[0]
            ImagePattern[1] = CompressFlag[1]

        manifest = Manifest_TypeDef()
        memset(addressof(manifest), 0xFF, sizeof(manifest))

        image_config = self.get_image_config(image_type)


        manifest.ImgID = image_config.img_id
        manifest.Ver = image_config.version
        if manifest.ImgID == ImgID.IMGID_BOOT.value:
            FlashCalibPattern_lsb = [htonl_to_ntohl(i) for i in FlashCalibPattern]
            FlashCalibPattern_bytes = bytes.fromhex(list_to_hex_str(FlashCalibPattern_lsb))
            memmove(addressof(manifest.Pattern), FlashCalibPattern_bytes, sizeof(manifest.Pattern))
        else:
            ImagePattern_lsb = [htonl_to_ntohl(i) for i in ImagePattern]
            ImagePattern_bytes = bytes.fromhex(list_to_hex_str(ImagePattern_lsb))
            memmove(addressof(manifest.Pattern), ImagePattern_bytes, sizeof(manifest.Pattern))

        manifest.MajorImgVer = image_config.img_ver_major
        manifest.MinorImgVer = image_config.img_ver_minor
        manifest.SecEpoch = image_config.sec_epoch

        if image_config.rsip_en:
            for i, img in enumerate([self.image1, self.image2, self.image3], start=1):
                if img == None: continue #NOTE: manifest maybe not contain image3
                rsip_mode = 0xFF if img.rsip_mode == None else img.rsip_mode
                manifest.RsipCfg = manifest.RsipCfg & (~(0x03 << (i * 2))|(rsip_mode << (i * 2)))

            memmove(addressof(manifest.RsipIV), bytes.fromhex(image_config.rsip_iv), 8)
            manifest.RsipCfg = manifest.RsipCfg & (((image_config.gcm_tag_len // 4) >> 1) | ~0x03)
        if ImgID.IMGID_NSPE.value == manifest.ImgID:
            if image_config.rdp_en:
                memmove(addressof(manifest.RsipIV), bytes.fromhex(image_config.rsip_iv), 8)
                memmove(byref(manifest.RsipIV, 8), bytes.fromhex(image_config.rdp_iv), 8)

        manifest.ImgSize = os.path.getsize(input_file)

        if image_config.secure_boot_en:
            if (ImagePattern[0] == CompressFlag[0]) and (ImagePattern[1] == CompressFlag[1]):
                #NOTE: compress use cert keys
                public_key = self.cert.public_key
                private_key = self.cert.private_key
            else:
                public_key = image_config.public_key
                private_key = image_config.private_key

            pubkey_bytes = bytes.fromhex(public_key)
            memmove(addressof(manifest.SBPubKey), pubkey_bytes, len(pubkey_bytes))

            manifest.AuthAlg = self.sboot.gen_auth_id(image_config.algorithm)
            if manifest.AuthAlg == -1:
                return Error(ErrorType.UNKNOWN_ERROR, "self.sboot gen auth id failed")
            if self.sboot.gen_hash_id(manifest, image_config.hash_alg) != 0:
                return Error(ErrorType.UNKNOWN_ERROR, "self.sboot gen hash id failed")
            self.sboot.HmacKey = image_config.hmac_key
            self.sboot.HmacKeyLen = len(image_config.hmac_key) // 2
            ret = self.sboot.gen_image_hash(input_file, manifest.ImgHash)
            if ret != 0:
                return Error(ErrorType.UNKNOWN_ERROR, f"self.sboot gen image hash failed: {ret}")

            ret = self.sboot.gen_signature(manifest.AuthAlg, private_key, manifest.SBPubKey, manifest, sizeof(Manifest_TypeDef) - SIGN_MAX_LEN, manifest.Signature)
            if ret != 0:
                return Error(ErrorType.UNKNOWN_ERROR, f"self.sboot gen signature failed: {ret}")

        with open(output_file, 'wb') as f:
            f.write(string_at(addressof(manifest), sizeof(Manifest_TypeDef)))

            new_size = ((sizeof(Manifest_TypeDef) - 1) // 4096 + 1) * 4096
            pad_count = new_size - sizeof(Manifest_TypeDef)
            f.write(b'\xFF' * pad_count)
        return Error.success()

    def create_keypair(self, output_file:str, algorithm:str) -> Error:
        self.context.logger.info(f"create keypair file for {algorithm}")
        auth_alg_id = self.sboot.gen_auth_id(algorithm)
        if auth_alg_id == -1:
            print("Fail to create keypair, ret: %d"%auth_alg_id)
            return Error(ErrorType.UNKNOWN_ERROR, f"Fail to create keypair for {algorithm}, ret: {auth_alg_id}")
        key_info = {'algorithm':algorithm, 'private_key':'', 'public_key':'', 'public_key_hash':''}

        if auth_alg_id == AuthAlg.AuthID_ED25519.value:
            self.sboot.ed25519_genkey(key_info)
        else:
            curve = self.sboot.get_supported_curve(auth_alg_id)
            self.sboot.ecdsa_genkey(curve, key_info)

        with open(output_file, 'w') as f:
            json.dump(key_info, f, indent=2)
        return Error.success()