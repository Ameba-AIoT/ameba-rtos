from abc import ABC
import json5
import json
import importlib
import shutil
from ctypes import *
from enum import Enum, unique
from typing import Union, List
import copy

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
              ('HukEpoch',c_uint32),
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
        if image_type in [ImageType.IMAGE1, ImageType.IMAGE2, ImageType.CERT]:  #image3 is not required
            self.sboot_enable:bool = config.get("sboot_enable", config.get("secure_boot_en", False))
            if self.sboot_enable:
                self.sboot_algorithm:str = config["sboot_algorithm"] if "sboot_algorithm" in config else config["algorithm"]
                self.sboot_hash_alg:str = config["sboot_hash_alg"] if "sboot_hash_alg" in config else config["hash_alg"]
                if "sboot_hmac_key" in config:
                    self.sboot_hmac_key:str = config[config["sboot_hmac_key"]] if config["sboot_hmac_key"] in config else config["sboot_hmac_key"]
                else:
                    self.sboot_hmac_key:str = config["hmac_key"]
                self.sboot_private_key:str = config["sboot_private_key"] if "sboot_private_key" in config else config["private_key"]
                self.sboot_public_key:str = config["sboot_public_key"] if "sboot_public_key" in config else config["public_key"]
            self.sboot_public_key_hash:str = config["sboot_public_key_hash"] if "sboot_public_key_hash" in config else config["public_key_hash"]

class ManifestManager(ABC):
    valid_algorithm = importlib.import_module('security').secure_boot.valid_algorithm

    def __init__(self, context: Context) -> None:
        super().__init__()
        self.context = context

        with open(context.manifest_file, 'r') as f:
            self.origin_json_data = json5.load(f)

        self.lib_security = importlib.import_module('security')
        self.sboot = self.lib_security.secure_boot()

        self.new_json_data = copy.deepcopy(self.origin_json_data)
        # Add key from outside(global config) of image part if key not in image part
        for img in ['image1', 'image2', 'image3', 'cert']:
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
            context.logger.info(f"manifest file does not contains image3, will use image2 config for image3")
            self.image3 = self.image2
        self.dsp = self.image2 #DSP use image2 config
        if 'cert' in self.new_json_data:
            self.cert = ManifestImageConfig(self.new_json_data['cert'], ImageType.CERT)
        else:
            context.logger.info(f"manifest file does not contains cert, will use image2 config for cert")
            self.cert = self.image2
        self.app_all = self.image2 #NOTE: APP_ALL used in compress image

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
                elif key in ["ctr_key", "rdp_key", "sboot_private_key", "sboot_public_key", "sboot_public_key_hash"]:
                    if isinstance(value, list):
                        if any(len(v) != 64 and len(v) != 32 for v in value):
                            self.context.logger.error(f'{key} format error: should be 32/64 bytes')
                            return False
                    elif len(value) != 64 and len(value) != 32 and value not in jdata:
                        self.context.logger.error(f'{key} format error: should be 32/64 bytes: {value}')
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
            if entry_config.sboot_public_key_hash != None:
                hash_bytes = bytes.fromhex(entry_config.sboot_public_key_hash) #REVIEW: check key existance?
                memmove(addressof(cert_entries[i].Hash), hash_bytes, len(hash_bytes))

        if self.image2.sboot_enable:
            # parse signing key json
            pubkey_bytes = bytes.fromhex(self.cert.sboot_public_key)
            memmove(addressof(cert.SBPubKey), pubkey_bytes, len(pubkey_bytes))

            cert.AuthAlg = self.sboot.gen_auth_id(self.cert.sboot_algorithm)
            if cert.AuthAlg == -1:
                return Error(ErrorType.UNKNOWN_ERROR, "self.sboot gen auth id failed")

            if self.sboot.gen_hash_id(cert, self.image2.sboot_hash_alg) != 0:
                return Error(ErrorType.UNKNOWN_ERROR, "self.sboot gen hash id failed")
            for i in range(0, entry_num):
                memmove(byref(cert, cert_size + i * sizeof(cert_entries[i])), addressof(cert_entries[i]), sizeof(cert_entries[i]))

            # gen signature
            self.sboot.HmacKey = self.image2.sboot_hmac_key
            self.sboot.HmacKeyLen = len(self.image2.sboot_hmac_key) // 2
            ret = self.sboot.gen_signature(cert.AuthAlg, self.cert.sboot_private_key, cert.SBPubKey, cert, cert.TableSize, cert.Signature)
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
        valid_type = [ImageType.IMAGE1, ImageType.IMAGE2, ImageType.APP_ALL]
        if image_type not in valid_type: #NOTE: APP_ALL used in compress image
            return Error(ErrorType.INVALID_ARGS, f"create manifest only for {valid_type}: {image_type}")

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
        manifest.HukEpoch = image_config.huk_epoch

        if image_config.rsip_enable:
            for i, img in enumerate([self.image1, self.image2, self.image3], start=1):
                if img == None: continue #NOTE: manifest maybe not contain image3
                rsip_mode = 0xFF if img.rsip_mode == None else img.rsip_mode
                manifest.RsipCfg = manifest.RsipCfg & (~(0x03 << (i * 2))|(rsip_mode << (i * 2)))

            memmove(addressof(manifest.RsipIV), bytes.fromhex(image_config.rsip_iv), 8)
            manifest.RsipCfg = manifest.RsipCfg & (((image_config.rsip_gcm_tag_len // 4) >> 1) | ~0x03)
        if ImgID.IMGID_NSPE.value == manifest.ImgID:
            if image_config.rdp_enable:
                memmove(addressof(manifest.RsipIV), bytes.fromhex(image_config.rsip_iv), 8)
                memmove(byref(manifest.RsipIV, 8), bytes.fromhex(image_config.rdp_iv), 8)

        manifest.ImgSize = os.path.getsize(input_file)

        if image_config.sboot_enable:
            if (ImagePattern[0] == CompressFlag[0]) and (ImagePattern[1] == CompressFlag[1]):
                #NOTE: compress use cert keys
                public_key = self.cert.sboot_public_key
                private_key = self.cert.sboot_private_key
            else:
                public_key = image_config.sboot_public_key
                private_key = image_config.sboot_private_key

            pubkey_bytes = bytes.fromhex(public_key)
            memmove(addressof(manifest.SBPubKey), pubkey_bytes, len(pubkey_bytes))

            manifest.AuthAlg = self.sboot.gen_auth_id(image_config.sboot_algorithm)
            if manifest.AuthAlg == -1:
                return Error(ErrorType.UNKNOWN_ERROR, "self.sboot gen auth id failed")
            if self.sboot.gen_hash_id(manifest, image_config.sboot_hash_alg) != 0:
                return Error(ErrorType.UNKNOWN_ERROR, "self.sboot gen hash id failed")
            self.sboot.HmacKey = image_config.sboot_hmac_key
            self.sboot.HmacKeyLen = len(image_config.sboot_hmac_key) // 2
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
        key_info = {'algorithm':algorithm, 'private_key':'', 'public_key':'', 'public_key_hash':''}

        if auth_alg_id == AuthAlg.AuthID_ED25519.value:
            sboot.ed25519_genkey(key_info)
        else:
            curve = sboot.get_supported_curve(auth_alg_id)
            sboot.ecdsa_genkey(curve, key_info)

        with open(output_file, 'w') as f:
            json.dump(key_info, f, indent=2)
        return Error.success()
