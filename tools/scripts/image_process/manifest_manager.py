from abc import ABC
import json5
import json
import importlib
import shutil
from ctypes import *
from enum import Enum, unique
from typing import Union, List
import copy
from cryptography.hazmat.primitives.asymmetric import ed25519
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives import serialization

from ameba_enums import *
from context import Context
from utility import *


SIGN_MAX_LEN = 144
PKEY_MAX_LEN = 144
HASH_MAX_LEN = 64

# Extended manifest sizes for PQC ml-dsa-87 support
SIGN_MAX_LEN_EXT = 4640
PKEY_MAX_LEN_EXT = 2592

# Manifest size constants
MANIFEST_SIZE_4K = 4096
MANIFEST_SIZE_8K = 8192

# Certificate size constants
CERT_SIZE_4K_ALIGN = 0x1000
CERT_SIZE_8K_ALIGN = 0x2000
CERT_MAX_KEYINFO = 5

# Manifest version flags
MANIFEST_VERSION_4KB = 1
MANIFEST_VERSION_8KB = 2

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
              ('HukEpoch',c_uint32),
              ('RsipIV',c_uint8 * 16),
              ('ImgHash',c_uint8 * HASH_MAX_LEN),
              ('SBPubKey',c_uint8 * PKEY_MAX_LEN),
              ('Signature',c_uint8 * SIGN_MAX_LEN)]

# Extended manifest structure for 8KB support and PQC algorithms
class Manifest_TypeDef_Ext(Structure):
    _fields_=[
        # 嵌套基础结构体
        ('basic_manifest', Manifest_TypeDef),

        # Extension area for PQC algorithms
        ('PQC_AuthAlg',c_uint8),                # PQC authentication algorithm
        ('Rsvd2',c_uint8 * 3),
        ('PQC_KeySize',c_uint16),               # PQC public key size
        ('PQC_SigSize',c_uint16),               # PQC signature size
        ('Rsvd3',c_uint8 * 8),
        ('PQCPubKey',c_uint8 * PKEY_MAX_LEN_EXT),    # PQC public key (up to 4096 bytes)
        ('PQCSignature',c_uint8 * SIGN_MAX_LEN_EXT)] # PQC signature (up to 4096 bytes)

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
              ('PKInfo',c_uint8 * sizeof(CertEntry_TypeDef) * CERT_MAX_KEYINFO),
              ('Signature',c_uint8 * SIGN_MAX_LEN)]

# Extended certificate structure for 8KB support and PQC algorithms
class Certificate_TypeDef_Ext(Structure):
    _fields_=[
        # 嵌套基础结构体
        ('basic_cert', Certificate_TypeDef),

        # Extension area for PQC algorithms
        ('PQC_AuthAlg',c_uint8),                # PQC authentication algorithm
        ('Rsvd2',c_uint8 * 3),
        ('PQC_KeySize',c_uint16),               # PQC public key size
        ('PQC_SigSize',c_uint16),               # PQC signature size
        ('Rsvd3',c_uint8 * 8),
        ('PQCPubKey',c_uint8 * PKEY_MAX_LEN_EXT),    # PQC public key (up to 4096 bytes)
        ('PQCSignature',c_uint8 * SIGN_MAX_LEN_EXT)] # PQC signature (up to 4096 bytes)

class SB_HEADER(Structure):
    _fields_=[('reserved', c_uint32*12),
              ('sb_sig', c_uint8*64)]

class ManifestImageConfig:
    def __init__(self, config: dict, image_type:ImageType) -> None:
        self.version:int = config["version"]
        self.img_id:Union[int, None] = config.get("img_id", None)
        self.img_ver_major:Union[int, None] = config.get("img_ver_major", None)
        self.img_ver_minor:Union[int, None] = config.get("img_ver_minor", None)
        self.huk_epoch:Union[int, None] = config.get("huk_epoch", config.get("sec_epoch", None))

        if image_type != ImageType.CERT:
            #RSIP
            # IMG3 enc: SoCs before amebagreen2 use RDP, not RSIP; new SoCs use RSIP, not RDP
            # Therefore, the rdp_ string is used to determine whether image3's RSIP is enabled.
            if image_type == ImageType.IMAGE3 and any(k.startswith("rdp_") for k in config):
                self.rsip_enable:bool = False
            else:
                self.rsip_enable:bool = config.get("rsip_enable", config.get("rsip_en", False))

            if self.rsip_enable:
                self.rsip_mode:int = config["rsip_mode"] #0 is CTR, 1 is XTS(CTR+ECB), 2 is GCM
                self.rsip_gcm_tag_len:int = config.get("rsip_gcm_tag_len", 0xFF)
                self.rsip_iv:str = config["rsip_iv"]
                self.rsip_key:List[str] = []
                if "rsip_key_group" in config:
                    self.rsip_key = [config[v] for v in config[config["rsip_key_group"]]]
                else:
                    if self.rsip_mode == 0:
                        self.rsip_key = [config["ctr_key"] if isinstance(config["ctr_key"], str) else config["ctr_key"][config["rsip_key_id"]]]
                    elif self.rsip_mode == 1:
                        self.rsip_key = [
                            config["ecb_key"] if isinstance(config["ecb_key"], str) else config["ecb_key"][config["rsip_key_id"]],
                            config["ctr_key"] if isinstance(config["ctr_key"], str) else config["ctr_key"][config["rsip_key_id"]],
                        ]
                    elif self.rsip_mode == 1:
                        self.rsip_key = [config["ctr_key"] if isinstance(config["ctr_key"], str) else config["ctr_key"][config["rsip_key_id"]]]

            #RDP
            self.rdp_enable:bool = config.get("rdp_enable", config.get("rdp_en", False))
            if self.rdp_enable:
                self.rdp_iv:str = config.get("rdp_iv", "") #NOTE: amebaD dont need rdp_iv
                self.rdp_key:str = ""
                if "rdp_key" in config:
                    if len(config["rdp_key"]) == 64 or len(config["rdp_key"]) == 32:
                        self.rdp_key = config["rdp_key"]
                    else:
                        self.rdp_key = config[config["rdp_key"]]

        #SBOOT:
        if image_type in [ImageType.IMAGE1, ImageType.IMAGE2, ImageType.CERT, ImageType.VBMETA]:  #image3 is not required
            self.sboot_enable:bool = config.get("sboot_enable", config.get("secure_boot_en", False))
            self.sboot_algorithm:str = config["sboot_algorithm"] if "sboot_algorithm" in config else config.get("algorithm", "")
            self.sboot_hash_alg:str = config["sboot_hash_alg"] if "sboot_hash_alg" in config else config.get("hash_alg", "")
            if "sboot_hmac_key" in config:
                self.sboot_hmac_key:str = config[config["sboot_hmac_key"]] if config["sboot_hmac_key"] in config else config["sboot_hmac_key"]
            else:
                self.sboot_hmac_key:str = config.get("hmac_key", "")
            self.sboot_private_key:str = config["sboot_private_key"] if "sboot_private_key" in config else config.get("private_key", "")
            self.sboot_public_key:str = config["sboot_public_key"] if "sboot_public_key" in config else config.get("public_key", "")
            self.sboot_public_key_hash:str = config["sboot_public_key_hash"] if "sboot_public_key_hash" in config else config.get("public_key_hash", "")

        #PQC Support (for image2 when version >= 2)
        if "sboot_pqc_public_key_hash" in config:
            self.sboot_pqc_public_key_hash = config["sboot_pqc_public_key_hash"]
        if "sboot_pqc_public_key" in config:
            self.sboot_pqc_public_key = config["sboot_pqc_public_key"]
        if "sboot_pqc_private_key" in config:
            self.sboot_pqc_private_key = config["sboot_pqc_private_key"]

class ManifestManager(ABC):
    valid_algorithm = importlib.import_module('security').secure_boot.valid_algorithm

    # PQC algorithm mapping shared between create_manifest and create_cert
    pqc_alg_mapping = {
        'ml_dsa_44': 240,   # AUTHID_PQC_ML_DSA_44
        'ml_dsa_65': 241,   # AUTHID_PQC_ML_DSA_65
        'ml_dsa_87': 242    # AUTHID_PQC_ML_DSA_87
    }
    pqc_sig_size_mapping = {
        'ml_dsa_44': 2420,
        'ml_dsa_65': 3309,
        'ml_dsa_87': 4627
    }

    def __init__(self, context: Context) -> None:
        super().__init__()
        self.context = context

        with open(context.manifest_file, 'r') as f:
            self.origin_json_data = json5.load(f)

        self.lib_security = importlib.import_module('security')
        self.sboot = self.lib_security.secure_boot()

        self.new_json_data = copy.deepcopy(self.origin_json_data)
        # Add key from outside(global config) of image part if key not in image part
        for img in ['image1', 'image2', 'image3', 'cert', 'vbmeta']:
            if img not in self.new_json_data:
                context.logger.info(f"manifest file does not contains {img}")
                continue
            #优先级: image内部直接定义>inherit_from>外部的全局定义
            if "inherit_from" in self.new_json_data[img]:
                for key, value in self.new_json_data[self.new_json_data[img]["inherit_from"]].items():
                    if key not in self.new_json_data[img]:
                        self.new_json_data[img][key] = value
            for key, value in self.origin_json_data.items():
                if isinstance(value, dict): continue

                if key not in self.new_json_data[img]:
                    self.new_json_data[img][key] = value

        if not self.validate_config(self.new_json_data):
            raise ValueError(f"Invalid JSON format")

        self.image1 = ManifestImageConfig(self.new_json_data['image1'], ImageType.IMAGE1)
        self.image2 = ManifestImageConfig(self.new_json_data['image2'], ImageType.IMAGE2)
        if 'image3' in self.new_json_data:
            self.image3 = ManifestImageConfig(self.new_json_data['image3'], ImageType.IMAGE3)
        else:
            context.logger.info(f"manifest file does not contains image3, will use hybrid config for image3")
            # Create a hybrid config for image3
            self.image3 = ManifestImageConfig(self.new_json_data['image1'], ImageType.IMAGE3)
            # Override with image2's IV
            if hasattr(self.image2, 'rsip_iv') and self.image2.rsip_enable:
                self.image3.rsip_iv = self.image2.rsip_iv
        self.dsp = self.image2 #DSP use image2 config
        if 'cert' in self.new_json_data:
            self.cert = ManifestImageConfig(self.new_json_data['cert'], ImageType.CERT)
        else:
            context.logger.info(f"manifest file does not contains cert, will use image2 config for cert")
            self.cert = self.image2
        self.app_all = self.image2 #NOTE: APP_ALL used in compress image
        if 'vbmeta' in self.new_json_data:
            self.vbmeta = ManifestImageConfig(self.new_json_data['vbmeta'], ImageType.VBMETA)

    def validate_pqc_config(self, data: dict) -> bool:
        """Validate PQC configuration based on manifest version"""
        version = data.get("version", 1)

        # List of all PQC fields to check
        pqc_fields = [
            "sboot_pqc_algorithm",
            "sboot_pqc_private_key",
            "sboot_pqc_public_key",
            "sboot_pqc_public_key_hash"
        ]

        # Collect all PQC fields from global and image configs
        found_pqc_fields = []

        # Check global config
        for field in pqc_fields:
            if field in data:
                found_pqc_fields.append(field)

        # Check image-specific configs, no image3
        for img_key in ["image1", "image2", "cert"]:
            if img_key in data and isinstance(data[img_key], dict):
                for field in pqc_fields:
                    if field in data[img_key]:
                        found_pqc_fields.append(f"{img_key}.{field}")

        # Version 1: PQC not supported
        if version == 1 and found_pqc_fields:
            self.context.logger.error(
                f"Manifest version 1 does not support PQC. "
                f"Found PQC fields: {', '.join(found_pqc_fields)}. "
                f"Please comment out these fields or upgrade to version 2."
            )
            return False

        # Version 2 with secure boot: All PQC fields required
        elif version == 2 and data.get("sboot_enable", False):
            # Check if all required PQC fields are present (either globally or in images)
            missing_fields = []
            for field in pqc_fields:
                # Check global config first
                if field not in data:
                    # Then check if field exists in any image config
                    found_in_image = False
                    for img_key in ["image1", "image2", "image3", "cert"]:
                        if (img_key in data and
                            isinstance(data[img_key], dict) and
                            field in data[img_key]):
                            found_in_image = True
                            break

                    if not found_in_image:
                        missing_fields.append(field)

            if missing_fields:
                self.context.logger.error(
                    f"Manifest version 2 with secure boot requires all PQC fields. "
                    f"Missing fields: {', '.join(missing_fields)}"
                )
                return False

        return True

    def validate_config(self, data:Union[str, dict]) -> bool:
        if isinstance(data, str):
            with open(data, 'r') as f:
                jdata = json5.load(f)
        else:
            jdata = data

        # First validate PQC configuration if this is the root config
        if isinstance(data, dict):
            # Validate PQC configuration based on version
            if not self.validate_pqc_config(jdata):
                return False

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
                    if len(value) != 16 and len(value) != 32:
                        self.context.logger.error(f'{key} format error: should be 16/32 bytes: {len(value)}')
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
                elif key in ["ctr_key", "rdp_key", "sboot_public_key_hash"]:
                    if isinstance(value, list):
                        if any(len(v) != 64 and len(v) != 32 for v in value):
                            self.context.logger.error(f'{key} format error: should be 32/64 bytes')
                            return False
                    elif len(value) != 64 and len(value) != 32 and value not in jdata:
                        self.context.logger.error(f'{key} format error: should be 32/64 bytes: {value}, current len: {len(value)}')
                        return False
                elif key.startswith("ecb_key"):
                    if isinstance(value, list):
                        if any(len(v) != 64 for v in value):
                            self.context.logger.error(f'{key} format error: should be 64 bytes')
                            return False
                    elif len(value) != 64:
                        self.context.logger.error(f'{key} format error: should be 64 bytes')
                        return False
                elif key == "algorithm" or key == "sboot_algorithm":
                    if value not in self.valid_algorithm:
                        self.context.logger.error(f'{key} format error: should be one of {self.valid_algorithm}')
                        return False
                elif key == "rsip_key_group":
                    if value not in jdata:
                        self.context.logger.error(f'rsip_key_group: {value} not exist')
                        return False
                    for k in jdata[value]:
                        if k not in jdata:
                            self.context.logger.error(f'{value}: {k} not exist')
                            return False

                    # 验证rsip_key_group中的算法名与rsip_mode匹配
                    rsip_mode = jdata.get("rsip_mode", None)
                    if rsip_mode is not None:
                        # 从rsip_key_group名称中提取算法名
                        key_group_lower = value.lower()
                        if "xts" in key_group_lower and rsip_mode != 1:
                            self.context.logger.error(f'rsip_key_group "{value}" indicates XTS mode but rsip_mode is {rsip_mode} (expected 1 for XTS)')
                            return False
                        elif "gcm" in key_group_lower and rsip_mode != 2:
                            self.context.logger.error(f'rsip_key_group "{value}" indicates GCM mode but rsip_mode is {rsip_mode} (expected 2 for GCM)')
                            return False
                        elif "ctr" in key_group_lower and rsip_mode != 0:
                            self.context.logger.error(f'rsip_key_group "{value}" indicates CTR mode but rsip_mode is {rsip_mode} (expected 0 for CTR)')
                            return False
        return True

    def get_image_config(self, image_type:Union[ImageType, str]) -> ManifestImageConfig:
        if isinstance(image_type, str):
            return getattr(self, image_type.lower())
        else:
            return getattr(self, image_type.name.lower())

    def create_cert(self, output_file:str, *entry_pairs) -> Error:
        # Check if using extended certificate
        use_extended_cert = self.image2.version >= 2

        if use_extended_cert:
            cert = Certificate_TypeDef_Ext()
            memset(addressof(cert), 0xFF, sizeof(cert))
            # 使用 basic_cert_part 指向嵌套的基础结构
            basic_cert_part = cert.basic_cert
        else:
            cert = Certificate_TypeDef()
            memset(addressof(cert), 0xFF, sizeof(cert))
            # basic_cert_part 直接指向 cert 本身
            basic_cert_part = cert

        basic_cert_part.Ver = self.image2.version
        basic_cert_part.ImgID = self.image2.img_id
        ImagePattern_lsb = [htonl_to_ntohl(i) for i in ImagePattern]
        ImagePattern_bytes = bytes.fromhex(list_to_hex_str(ImagePattern_lsb))
        memmove(addressof(basic_cert_part.Pattern), ImagePattern_bytes, sizeof(basic_cert_part.Pattern))
        basic_cert_part.MajorKeyVer = self.image2.img_ver_major
        basic_cert_part.MinorKeyVer = self.image2.img_ver_minor

        print(f"Certificate set: Ver={basic_cert_part.Ver}, ImgID={basic_cert_part.ImgID}, MajorKeyVer={basic_cert_part.MajorKeyVer}, MinorKeyVer={basic_cert_part.MinorKeyVer}")

        # Build entry list dynamically
        entries = []

        # step-1: Add provided entry pairs (backward compatibility)
        for i in range(0, len(entry_pairs), 2):
            entries.append((int(entry_pairs[i]), entry_pairs[i+1]))
            print(f"Added entry pair: KeyID={entry_pairs[i]}, name={entry_pairs[i+1]}")

        # step-2: Add PQC entry if version >= 2 and not already added
        if self.image2.version >= 2:
            has_pqc = any(e[1] == 'image2_pqc' for e in entries)
            if not has_pqc and hasattr(self.image2, 'sboot_pqc_public_key_hash'):
                entries.append((1, 'image2_pqc'))
                print(f"Auto-added PQC entry: KeyID=1, name=image2_pqc")

        # step-3: Add user-defined keys from global manifest configuration
        for key, value in self.new_json_data.items():
            if key.startswith('user_defined_key_') and isinstance(value, str) and len(value) == 64:  # 32 bytes = 64 hex chars
                try:
                    key_id = int(key.split('_')[-1])
                    entries.append((key_id, key))
                    print(f"Added user-defined key: KeyID={key_id}, name={key}")
                except (ValueError, IndexError):
                    self.context.logger.warning(f"Invalid user_defined_key format: {key}")

        print(f"Total entries: {len(entries)}")
        for i, (key_id, name) in enumerate(entries):
            print(f"  Entry[{i}]: KeyID={key_id}, name={name}")

        # Check entry limit (Certificate structure supports max CERT_MAX_KEYINFO entries)
        if len(entries) > CERT_MAX_KEYINFO:
            self.context.logger.warning(f"Too many certificate entries ({len(entries)}), only first CERT_MAX_KEYINFO will be used")
            entries = entries[:CERT_MAX_KEYINFO]

        entry_num = len(entries)
        entry_size = entry_num * sizeof(CertEntry_TypeDef)
        cert_entries = []
        for _ in range(0, entry_num):
            entry = CertEntry_TypeDef()
            memset(addressof(entry), 0xFF, sizeof(entry))
            cert_entries.append(entry)

        cert_size = sizeof(Certificate_TypeDef) - sizeof(CertEntry_TypeDef) * CERT_MAX_KEYINFO - SIGN_MAX_LEN
        basic_cert_part.TableSize = cert_size + entry_size
        basic_cert_part.EntryNum = entry_num

        # Process certificate entries
        for i, (key_id, entry_name) in enumerate(entries):
            cert_entries[i].KeyID = key_id

            # Handle special case for PQC key hash
            if entry_name == 'image2_pqc':
                if hasattr(self.image2, 'sboot_pqc_public_key_hash'):
                    hash_bytes = bytes.fromhex(self.image2.sboot_pqc_public_key_hash)
                    memmove(addressof(cert_entries[i].Hash), hash_bytes, min(len(hash_bytes), 32))
            # Handle user-defined keys
            elif entry_name.startswith('user_defined_key_'):
                hash_value = self.new_json_data[entry_name]
                hash_bytes = bytes.fromhex(hash_value)
                memmove(addressof(cert_entries[i].Hash), hash_bytes, min(len(hash_bytes), 32))
            # Handle image-based keys (original behavior)
            else:
                entry_config = self.get_image_config(entry_name)
                if hasattr(entry_config, 'sboot_public_key_hash') and entry_config.sboot_public_key_hash is not None:
                    hash_bytes = bytes.fromhex(entry_config.sboot_public_key_hash)
                    memmove(addressof(cert_entries[i].Hash), hash_bytes, min(len(hash_bytes), 32))

        if self.image2.sboot_enable:
            # parse signing key json
            pubkey_bytes = bytes.fromhex(self.cert.sboot_public_key)
            memmove(addressof(basic_cert_part.SBPubKey), pubkey_bytes, len(pubkey_bytes))

            basic_cert_part.AuthAlg = self.sboot.gen_auth_id(self.cert.sboot_algorithm)
            if basic_cert_part.AuthAlg == -1:
                return Error(ErrorType.UNKNOWN_ERROR, "self.sboot gen auth id failed")

            if self.sboot.gen_hash_id(basic_cert_part, self.image2.sboot_hash_alg) != 0:
                return Error(ErrorType.UNKNOWN_ERROR, "self.sboot gen hash id failed")

            # Copy certificate entries to PKInfo field
            pkinfo_offset = addressof(basic_cert_part) + cert_size  # After SBPubKey field
            for i in range(0, entry_num):
                entry_offset = pkinfo_offset + i * sizeof(cert_entries[i])
                memmove(entry_offset, addressof(cert_entries[i]), sizeof(cert_entries[i]))

            # gen ECC signature
            self.sboot.HmacKey = self.image2.sboot_hmac_key
            self.sboot.HmacKeyLen = len(self.image2.sboot_hmac_key) // 2

            ret = self.sboot.gen_signature(basic_cert_part.AuthAlg, self.cert.sboot_private_key, basic_cert_part.SBPubKey, basic_cert_part, basic_cert_part.TableSize, basic_cert_part.Signature)
            if ret != 0:
                return Error(ErrorType.UNKNOWN_ERROR, f"self.sboot gen signature failed: {ret}")

            if use_extended_cert:
                # Set PQC fields
                pqc_algorithm = self.new_json_data.get('sboot_pqc_algorithm', 'ml_dsa_65')

                if pqc_algorithm in self.pqc_alg_mapping:
                    cert.PQC_AuthAlg = self.pqc_alg_mapping[pqc_algorithm]
                    cert.PQC_SigSize = self.pqc_sig_size_mapping[pqc_algorithm]
                    print(f"PQC AuthAlg set to: {cert.PQC_AuthAlg}, SigSize: {cert.PQC_SigSize}")
                else:
                    return Error(ErrorType.UNKNOWN_ERROR, f"Unsupported PQC algorithm: {pqc_algorithm}")

                # Get PQC keys from image1
                pqc_pubkey_bytes = bytes.fromhex(self.image1.sboot_pqc_public_key)
                cert.PQC_KeySize = len(pqc_pubkey_bytes)
                pqc_private_key = self.image1.sboot_pqc_private_key

                # Copy PQC public key
                memmove(addressof(cert.PQCPubKey), pqc_pubkey_bytes, cert.PQC_KeySize)

                # Generate PQC signature for the same message as ECC
                msg_buffer = (c_uint8 * basic_cert_part.TableSize)()
                memmove(msg_buffer, addressof(cert), basic_cert_part.TableSize)

                # Message should be passed as bytes
                msg_bytes = bytes(msg_buffer)
                ret = self.sboot.ml_dsa_65_sign(pqc_private_key, msg_bytes, basic_cert_part.TableSize, cert.PQCSignature)

                if ret != 0:
                    print(f"ERROR: PQC certificate signature generation failed, ret={ret}")
                    return Error(ErrorType.UNKNOWN_ERROR, f"PQC certificate signature generation failed: {ret}")

        # Write certificate to file
        with open(output_file, 'wb') as f:
            total_cert_size = 0
            # write cert part (include public key)
            f.write(string_at(addressof(cert), cert_size))
            total_cert_size += cert_size
            # write key entries
            for i in range(0, entry_num):
                f.write(string_at(addressof(cert_entries[i]), sizeof(cert_entries[i])))
                total_cert_size += sizeof(cert_entries[i])
            # write ecc signature
            f.write(string_at(addressof(basic_cert_part.Signature), SIGN_MAX_LEN))
            total_cert_size += SIGN_MAX_LEN
            print(f"PQC certificate basic done, total_cert_size: {total_cert_size}, entry_num:{entry_num}")

            if use_extended_cert:
                pqc_ext_size = sizeof(Certificate_TypeDef_Ext) - sizeof(Certificate_TypeDef)
                f.write(string_at(addressof(cert) + sizeof(Certificate_TypeDef), pqc_ext_size))
                total_cert_size += pqc_ext_size

                # Pad to CERT_SIZE_8K_ALIGN
                pad_count = CERT_SIZE_8K_ALIGN - total_cert_size
                f.write(b'\xFF' * pad_count)
            else:
                # Pad to CERT_SIZE_4K_ALIGN
                pad_count = CERT_SIZE_4K_ALIGN - basic_cert_part.TableSize - SIGN_MAX_LEN
                f.write(b'\xFF' * pad_count)

        return Error.success()

    def create_manifest(self, output_file:str, input_file:str, img_type = ImageType.UNKNOWN, compress = False) -> Error:
        image_type = img_type if img_type != ImageType.UNKNOWN else parse_image_type(input_file)
        valid_type = [ImageType.IMAGE1, ImageType.IMAGE2, ImageType.APP_ALL]
        if image_type not in valid_type: #NOTE: APP_ALL used in compress image
            return Error(ErrorType.INVALID_ARGS, f"create manifest only for {valid_type}: {image_type}")
        image_config = self.get_image_config(image_type)

        if compress:
            global ImagePattern
            ImagePattern[0] = CompressFlag[0]
            ImagePattern[1] = CompressFlag[1]

        # Choose manifest structure based on version
        use_extended_manifest = image_config.version >= 2

        if use_extended_manifest:
            manifest = Manifest_TypeDef_Ext()
            memset(addressof(manifest), 0xFF, sizeof(manifest))
            # 使用 basic_manifest_part 指向嵌套的基础结构
            basic_manifest_part = manifest.basic_manifest
            manifest_size = MANIFEST_SIZE_8K
        else:
            manifest = Manifest_TypeDef()
            memset(addressof(manifest), 0xFF, sizeof(manifest))
            # basic_manifest_part 直接指向 manifest 本身
            basic_manifest_part = manifest
            manifest_size = MANIFEST_SIZE_4K

        basic_manifest_part.ImgID = image_config.img_id
        basic_manifest_part.Ver = image_config.version
        if basic_manifest_part.ImgID == ImgID.IMGID_BOOT.value:
            FlashCalibPattern_lsb = [htonl_to_ntohl(i) for i in FlashCalibPattern]
            FlashCalibPattern_bytes = bytes.fromhex(list_to_hex_str(FlashCalibPattern_lsb))
            memmove(addressof(basic_manifest_part.Pattern), FlashCalibPattern_bytes, sizeof(basic_manifest_part.Pattern))
        else:
            ImagePattern_lsb = [htonl_to_ntohl(i) for i in ImagePattern]
            ImagePattern_bytes = bytes.fromhex(list_to_hex_str(ImagePattern_lsb))
            memmove(addressof(basic_manifest_part.Pattern), ImagePattern_bytes, sizeof(basic_manifest_part.Pattern))

        basic_manifest_part.MajorImgVer = image_config.img_ver_major
        basic_manifest_part.MinorImgVer = image_config.img_ver_minor
        basic_manifest_part.HukEpoch = image_config.huk_epoch

        print(f"Manifest set: Ver={basic_manifest_part.Ver}, ImgID={basic_manifest_part.ImgID}, MajorImgVer={basic_manifest_part.MajorImgVer}, MinorImgVer={basic_manifest_part.MinorImgVer}")

        if image_config.rsip_enable:
            for i, img in enumerate([self.image1, self.image2, self.image3], start=1):
                if img is None: continue #NOTE: manifest maybe not contain image3
                # Only process rsip_mode for images that have rsip_enable and rsip_mode attribute
                if not hasattr(img, 'rsip_enable') or not img.rsip_enable:
                    continue
                rsip_mode = getattr(img, 'rsip_mode', None)
                if rsip_mode is None:
                    continue
                basic_manifest_part.RsipCfg = basic_manifest_part.RsipCfg & (~(0x03 << (i * 2))|(rsip_mode << (i * 2)))

            memmove(addressof(basic_manifest_part.RsipIV), bytes.fromhex(image_config.rsip_iv), 8)
            basic_manifest_part.RsipCfg = basic_manifest_part.RsipCfg & (((image_config.rsip_gcm_tag_len // 4) >> 1) | ~0x03)
        if ImgID.IMGID_NSPE.value == basic_manifest_part.ImgID:
            if image_config.rdp_enable:
                memmove(addressof(basic_manifest_part.RsipIV), bytes.fromhex(image_config.rsip_iv), 8)
                memmove(byref(basic_manifest_part.RsipIV, 8), bytes.fromhex(image_config.rdp_iv), 8)

        basic_manifest_part.ImgSize = os.path.getsize(input_file)

        if image_config.sboot_enable:
            if (ImagePattern[0] == CompressFlag[0]) and (ImagePattern[1] == CompressFlag[1]):
                #NOTE: compress use cert keys
                public_key = self.cert.sboot_public_key
                private_key = self.cert.sboot_private_key
            else:
                public_key = image_config.sboot_public_key
                private_key = image_config.sboot_private_key

            pubkey_bytes = bytes.fromhex(public_key)
            memmove(addressof(basic_manifest_part.SBPubKey), pubkey_bytes, len(pubkey_bytes))

            basic_manifest_part.AuthAlg = self.sboot.gen_auth_id(image_config.sboot_algorithm)
            if basic_manifest_part.AuthAlg == -1:
                return Error(ErrorType.UNKNOWN_ERROR, "self.sboot gen auth id failed")
            if self.sboot.gen_hash_id(basic_manifest_part, image_config.sboot_hash_alg) != 0:
                return Error(ErrorType.UNKNOWN_ERROR, "self.sboot gen hash id failed")
            self.sboot.HmacKey = image_config.sboot_hmac_key
            self.sboot.HmacKeyLen = len(image_config.sboot_hmac_key) // 2
            ret = self.sboot.gen_image_hash(input_file, basic_manifest_part.ImgHash)
            if ret != 0:
                return Error(ErrorType.UNKNOWN_ERROR, f"self.sboot gen image hash failed: {ret}")

            ret = self.sboot.gen_signature(basic_manifest_part.AuthAlg, private_key, basic_manifest_part.SBPubKey, basic_manifest_part, sizeof(Manifest_TypeDef) - SIGN_MAX_LEN, basic_manifest_part.Signature)
            if ret != 0:
                return Error(ErrorType.UNKNOWN_ERROR, f"self.sboot gen signature failed: {ret}")

        # Handle PQC fields for extended manifest
        if use_extended_manifest:
            # Check if PQC is configured globally
            pqc_algorithm = self.new_json_data.get('sboot_pqc_algorithm', None)
            if pqc_algorithm:
                print(f"PQC algorithm detected: {pqc_algorithm}")

                if pqc_algorithm in self.pqc_alg_mapping:
                    # Set PQC algorithm ID and SigSize
                    manifest.PQC_AuthAlg = self.pqc_alg_mapping[pqc_algorithm]
                    manifest.PQC_SigSize = self.pqc_sig_size_mapping[pqc_algorithm]

                    # Get PQC keys
                    pqc_private_key = image_config.sboot_pqc_private_key
                    pqc_public_key = image_config.sboot_pqc_public_key

                    if pqc_public_key:
                        # Convert hex string to bytes and copy to manifest
                        pqc_pubkey_bytes = bytes.fromhex(pqc_public_key)
                        pqc_key_size = len(pqc_pubkey_bytes)

                        if pqc_key_size <= PKEY_MAX_LEN_EXT:
                            manifest.PQC_KeySize = pqc_key_size
                            memmove(addressof(manifest.PQCPubKey), pqc_pubkey_bytes, pqc_key_size)
                            print(f"PQC public key size: {pqc_key_size}")
                        else:
                            return Error(ErrorType.UNKNOWN_ERROR, f"PQC public key too large: {pqc_key_size} > {PKEY_MAX_LEN_EXT}")

                    # Generate PQC signature if private key is available
                    if pqc_private_key:
                        # PQC signature should cover same content as ECC signature (base manifest only)
                        # Exclude PQCSignature and PQC metadata from signature
                        pqc_msg_length = sizeof(Manifest_TypeDef) - SIGN_MAX_LEN  # Same as ECC signature

                        # Create message buffer for PQC signature
                        msg_buffer = (c_uint8 * pqc_msg_length)()
                        memmove(msg_buffer, addressof(manifest), pqc_msg_length)

                        # Generate PQC signature
                        if pqc_algorithm == 'ml_dsa_65':
                            # Convert ctypes array to bytes for PQC library
                            msg_bytes = bytes(msg_buffer)
                            ret = self.sboot.ml_dsa_65_sign(pqc_private_key, msg_bytes, pqc_msg_length, manifest.PQCSignature)
                            if ret == 0:
                                # Get signature size from ML-DSA-65
                                print(f"PQC signature generated successfully, size: {manifest.PQC_SigSize}")
                            else:
                                return Error(ErrorType.UNKNOWN_ERROR, f"Failed to generate PQC signature: {ret}")
                        else:
                            return Error(ErrorType.UNKNOWN_ERROR, f"Unsupported PQC algorithm: {pqc_algorithm}")

                    print(f"PQC fields populated - AuthAlg: {manifest.PQC_AuthAlg}, KeySize: {manifest.PQC_KeySize}, SigSize: {manifest.PQC_SigSize}")

        with open(output_file, 'wb') as f:
            # Write the correct manifest structure
            if use_extended_manifest:
                f.write(string_at(addressof(manifest), sizeof(Manifest_TypeDef_Ext)))
                pad_count = manifest_size - sizeof(Manifest_TypeDef_Ext)
            else:
                f.write(string_at(addressof(manifest), sizeof(Manifest_TypeDef)))
                pad_count = manifest_size - sizeof(Manifest_TypeDef)
            f.write(b'\xFF' * pad_count)
        return Error.success()

    def create_sboot(self, output_file:str, input_file:str, image_type = ImageType.UNKNOWN) -> Error:
        if image_type == ImageType.UNKNOWN:
            #Detect image type by image name
            image_type = parse_image_type(input_file)
        image_config = self.get_image_config(image_type)
        layout_file = self.context.layout_file
        if image_config.sboot_enable:
            lib_security = importlib.import_module('security')
            sboot = lib_security.secure_boot()

            with open(input_file, 'rb') as f:
                content = f.read()
            origin_size = len(content)
            padded_size = ((origin_size - 1) // 4 + 1) * 4
            content.ljust(padded_size, b'\xFF')

            if image_type == ImageType.IMAGE1:
                section_addr = get_layout_address(layout_file, "KM4_BOOT_XIP", "ORIGIN")
                if section_addr == '':
                    self.context.logger.fatal(f"Failed to parse addr for KM4_BOOT_XIP in {layout_file}")
                addr = (int(section_addr, 16) + padded_size).to_bytes(4, 'little')
            else: # image_config.secure_image2_en
                if image_type == ImageType.IMAGE2:
                    section_addr = get_layout_address(layout_file, "KM4_IMG2_XIP", "ORIGIN")
                    if section_addr == '':
                        self.context.logger.fatal(f"Failed to parse addr for KM4_IMG2_XIP in {layout_file}")
                    addr = (int(section_addr, 16) + padded_size).to_bytes(4, 'little')
                else:
                    addr = (0xFFFFFFFF).to_bytes(4, 'little')
            new_content = content[:0x10] + addr + content[0x14:]

            sbHdr = SB_HEADER()
            memset(addressof(sbHdr), 0xFF, sizeof(sbHdr))
            sboot.gen_signature(AuthAlg.AuthID_ED25519.value, image_config.sboot_private_key, image_config.sboot_public_key, new_content[0x20:], padded_size - 0x20, sbHdr.sb_sig)

            with open(output_file, 'wb') as f:
                f.write(new_content)

                f.write(string_at(addressof(sbHdr), sizeof(sbHdr)))
                if image_type == ImageType.IMAGE2:
                    new_size = ((padded_size + sizeof(sbHdr) - 1) // 4096 + 1) * 4096
                    pad_count = new_size - sizeof(sbHdr) - padded_size
                    f.write(b'\xFF' * pad_count)
        else:
            shutil.copy(input_file, output_file)
        return Error.success()

    @staticmethod
    def create_keypair(context:Context, output_file:str, algorithm:str) -> Error:
        context.logger.info(f"create keypair file for {algorithm}")
        lib_security = importlib.import_module('security')
        sboot = lib_security.secure_boot()
        auth_alg_id = sboot.gen_auth_id(algorithm)
        if auth_alg_id == -1:
            print("Fail to create keypair, ret: %d"%auth_alg_id)
            return Error(ErrorType.UNKNOWN_ERROR, f"Fail to create keypair for {algorithm}, ret: {auth_alg_id}")

        # Use PQC field names for ml_dsa_65 algorithm
        if algorithm == "ml_dsa_65":
            key_info = {'sboot_pqc_algorithm': algorithm}
        else:
            key_info = {'sboot_algorithm': algorithm}

        if algorithm == "ml_dsa_65":
            ret = sboot.ml_dsa_65_genkey(key_info)
            if ret != 0:
                return Error(ErrorType.UNKNOWN_ERROR, f"Failed to generate ML-DSA-65 keypair, ret: {ret}")
        elif auth_alg_id == AuthAlg.AuthID_ED25519.value:
            sboot.ed25519_genkey(key_info)
        else:
            curve = sboot.get_supported_curve(auth_alg_id)
            sboot.ecdsa_genkey(curve, key_info)

        with open(output_file, 'w') as f:
            json5.dump(key_info, f, indent=2)
        return Error.success()

    def transform_to_pem(self, output_file:str, algorithm:str, image_type) -> Error:
        supportted_alg = ['ed25519', 'secp256r1']
        if algorithm not in supportted_alg:
            self.context.logger.error(f"not support algorithm: {algorithm}, support: {supportted_alg}")
            return Error(ErrorType.INVALID_ARGS, "not support algorithm")
        image_config = self.get_image_config(image_type)
        private_key = bytes.fromhex(image_config.sboot_private_key)
        public_key = bytes.fromhex(image_config.sboot_public_key)
        public_key_hash_hex = image_config.sboot_public_key_hash
        final_priv = None
        if algorithm == 'ed25519':
            seed = private_key[:32]
            expected_pk = public_key
            priv = ed25519.Ed25519PrivateKey.from_private_bytes(seed)
            pub = priv.public_key()
            pub_raw = pub.public_bytes(
                encoding=serialization.Encoding.Raw,
                format=serialization.PublicFormat.Raw
            )
            if pub_raw == expected_pk:
                final_priv = priv
            calc_hash = hashlib.sha256(pub_raw).hexdigest().upper()
            json_hash_upper = public_key_hash_hex.upper()
            if calc_hash != json_hash_upper:
                self.context.logger.warning(f"hash not match: {json_hash_upper} vs {calc_hash}")
        elif algorithm == 'secp256r1':
            private_value = int.from_bytes(private_key, byteorder='big')
            final_priv = ec.derive_private_key(private_value, ec.SECP256R1())
            pub = final_priv.public_key()
            pub_raw = pub.public_bytes(
                encoding=serialization.Encoding.X962,
                format=serialization.PublicFormat.UncompressedPoint
            )
            expected_pk = public_key
            pub_to_compare = pub_raw
            if len(expected_pk) == 64 and len(pub_raw) == 65:
                pub_to_compare = pub_raw[1:]

            if pub_to_compare != expected_pk:
                self.context.logger.warning(
                    f"Public key mismatch! Config: {expected_pk.hex()[:10]}... Derived: {pub_to_compare.hex()[:10]}..."
                )
            calc_hash = hashlib.sha256(pub_raw).hexdigest().upper()
            json_hash_upper = public_key_hash_hex.upper()

            if calc_hash != json_hash_upper:
                calc_hash_no_header = hashlib.sha256(pub_raw[1:]).hexdigest().upper()
                if calc_hash_no_header == json_hash_upper:
                    pass
                else:
                    self.context.logger.warning(f"hash not match: {json_hash_upper} vs {calc_hash}")

        enc = serialization.NoEncryption()
        pem_pkcs8 = final_priv.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.PKCS8,
            encryption_algorithm=enc
        )
        with open(output_file, "wb") as f:
            f.write(pem_pkcs8)
        return Error.success()