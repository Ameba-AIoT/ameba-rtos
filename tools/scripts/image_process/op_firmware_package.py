import os
import shutil
from typing import Union, Dict

from context import Context
from op_base import OperationBase
from op_encrypt import Encrypt
from op_rsip import Rsip
from op_rdp import Rdp
from op_pad import Pad
from op_prepend_header import PrependHeader
from ameba_enums import *
from utility import *
from manifest_manager import ManifestManager

@dataclass(frozen=True)
class SocImageConfig:
    image1_section: Union[str, dict]    #Used for rsip: project_name:section_name
    image2_section: dict                #Used for rsip: project_name:section_name
    image3_section: Union[str, None]    #Used for rsip

    def section(self, image_type:Union[ImageType, str]) -> str:
        if isinstance(image_type, str):
            return getattr(self, f'{image_type.lower()}_section')
        else:
            return getattr(self, f'{image_type.name.lower()}_section')

soc_configs:Dict[str, SocImageConfig] = {
    "amebad": SocImageConfig(
        image1_section = {
            "km0": "KM0_BOOT_XIP",
            "km4": "KM4_BOOT_XIP",
        },
        image2_section = {
            "km0": "KM0_IMG2_XIP",
            "km4": "KM4_IMG2_XIP",
        },
        image3_section = None #Not support rsip yet(maybe only support rdp)
    ),
    "amebadplus": SocImageConfig(
        image1_section = "KM4_BOOT_XIP",
        image2_section = {
            "km0": "KM0_IMG2_XIP",
            "km4": "KM4_IMG2_XIP",
        },
        image3_section = None #Not support rsip yet(maybe only support rdp)
    ),
    "amebagreen2": SocImageConfig(
        image1_section = "AP_BOOT_XIP",
        image2_section = {
            "km4ns": "NP_IMG2_XIP",
            "km4tz": "AP_IMG2_XIP",
        },
        image3_section = "AP_IMG3_XIP"
    ),
    "amebaL2": SocImageConfig(
        image1_section = "KM4TZ_BOOT_XIP",
        image2_section = {
            "km4ns": "KM4NS_IMG2_XIP",
            "km4tz": "KM4TZ_IMG2_XIP",
        },
        image3_section = "KM4TZ_IMG3_XIP"
    ),
    "amebalite": SocImageConfig(
        image1_section = "KM4_BOOT_XIP",
        image2_section = {
            "kr4": "KR4_IMG2_XIP",
            "km4": "KM4_IMG2_XIP",
        },
        image3_section = None #Not support rsip yet(maybe only support rdp)
    ),
    "amebasmart": SocImageConfig(
        image1_section = "KM4_BOOT_XIP",
        image2_section = {
            "ap": "CA32_IMG2_XIP",
            "hp": "KM4_IMG2_XIP",
            "lp": "KM0_IMG2_XIP",
        },
        image3_section = None #Not support rsip yet(maybe only support rdp)
    ),
    "amebasmartplus": SocImageConfig(
        image1_section = "KM4_BOOT_XIP",
        image2_section = {
            "ap": "CA32_IMG2_XIP",
            "hp": "KM4_IMG2_XIP",
            "lp": "KM0_IMG2_XIP",
        },
        image3_section = None #Not support rsip yet(maybe only support rdp)
    ),
}

class FirmwarePackage(OperationBase):
    cmd_help_msg = 'Generate final firmware image'
    cmd_promote = "fw_pack"

    def __init__(self, context:Context) -> None:
        super().__init__(context)
        self.config:SocImageConfig = soc_configs[context.soc_project]

        #init in pre_process
        self.output_project = ''
        self.output_image_dir = ''
        self.output_file = ''


        self.manifest_manager = ManifestManager(context)
        self.encrypt_handler = Encrypt(context)

    @staticmethod
    def register_args(parser) -> None:
        #WARNING: The file paths passed as parameters for image1/image2/image3
        #         MUST be the original paths generated during their corresponding project creation.
        #         Paths of files copied to other projects are not allowed. The reasons are as follows:
        #  1. For image2, its files may come from multiple different projects. Therefore, it’s necessary
        #     to obtain the correct section based on their paths for RSIP encryption.
        #  2. For image3, the project information carried in its file path is used to determine
        #     which image2 it should be packaged behind during the packaging process.
        parser.add_argument('--image1', type=str, help='Input image1 file path')
        parser.add_argument('--image2', type=str, nargs='+', help='Input image2 file path, the order and project path is important')
        parser.add_argument('--image3', type=str, help='Input image3 file path')
        parser.add_argument('--imgtool-floader', type=str, help='Input imagetool flashloader file path')
        parser.add_argument('--fullmac-image1', type=str, help='Input fullmac image1 file path')
        parser.add_argument('--fullmac-image2', type=str, help='Input fullmac image2 file path')
        parser.add_argument('--dsp', type=str, help='Input dsp file path')
        parser.add_argument('-o', '--output-file', type=str, help='Output file name', required=True)
        parser.add_argument('--output-project', type=str, help='Output project name') #if not set, output project is decided by output_file

    def pre_process(self) -> Error:
        if not self.context.soc_project:
            self.logger.fatal("soc_project variable is required")
            Error(ErrorType.INVALID_INPUT)

        for img_type in ['image1', 'image2', 'image3', 'dsp', 'imgtool_floader']:
            files = getattr(self.context.args, img_type)
            if files:
                if isinstance(files, str):
                    if not os.path.exists(files):
                        self.logger.fatal(f'File not exist: {files} for type: {img_type}')
                        return Error(ErrorType.FILE_NOT_FOUND, files)
                elif isinstance(files, list):
                    for file in files:
                        if not os.path.exists(file):
                            self.logger.fatal(f'File not exist: {file} for type: {img_type}')
                            return Error(ErrorType.FILE_NOT_FOUND, file)
                else:
                    self.logger.fatal(f'Unknown parameter type: {type(files)} for type: {img_type}')
                    return Error(ErrorType.UNKNOWN_ERROR)
        if self.context.args.image1:
            if self.context.args.image2 or self.context.args.image3:
                self.logger.fatal(f'Only support processing image1 or image2/image3 at once')
                return Error(ErrorType.INVALID_INPUT)

        if self.context.args.output_project:
            self.output_project = self.context.args.output_project
        else:
            info = parse_project_info(self.context.args.output_file)
            self.output_project = info['mcu_project']
            if info['mcu_project'] == "":
                self.logger.fatal(f'Failed determine output project by output file: {self.context.args.output_file}')
                return Error(ErrorType.INVALID_INPUT)

        if self.output_project == "":
            return Error(ErrorType.INVALID_INPUT, "Failed determine output project")

        self.output_image_dir = self.context.get_image_target_dir(self.output_project)
        if file_has_path(self.context.args.output_file):
            self.output_file = self.context.args.output_file
        else:
            self.output_file = os.path.join(self.output_image_dir, self.context.args.output_file)

        return Error.success()

    def process(self) -> Error:
        if self.context.args.image1:
            return self.process_boot()
        elif self.context.args.image2:
            return self.process_app()
        elif self.context.args.imgtool_floader:
            return self.process_imgtool_floader()
        elif self.context.args.fullmac_image1:
            return self.process_fullmac_image1()
        elif self.context.args.fullmac_image2:
            return self.process_fullmac_image2()
        else:
            self.logger.fatal("No image input")
            return Error(ErrorType.INVALID_INPUT, "No image input")

    def post_process(self) -> Error:
        return Error.success()

    def process_boot(self) -> Error:
        #Structure of file used for generating manifest.bin
        # ┌───────────────────────┐
        # │ image1_gcm_prepend.bin│  <- optional(only when image1 gcm mode enabled)
        # ├───────────────────────┤
        # │    image1_all.bin     │
        # └───────────────────────┘

        #Final output file's structure
        # ┌───────────────────────┐
        # │     manifest.bin      │
        # ├───────────────────────┤
        # │ image1_gcm_prepend.bin│  <- optional(only when image1 gcm mode enabled)
        # ├───────────────────────┤
        # │   image1_all_en.bin   │
        # └───────────────────────┘
        input_file = self.context.args.image1
        tmp_manifest_source_file = os.path.join(self.output_image_dir, 'boot_manifest_source.bin') #file for creating manifest
        tmp_en_file_name = modify_file_path(input_file, suffix='_en', new_directory=self.output_image_dir)             #encrypted file
        tmp_gcm_prepend_file_name = modify_file_path(tmp_en_file_name, suffix='_tag_prepend')   #prepend gcm tag file
        manifest_config = self.manifest_manager.image1
        gcm_enable = manifest_config.rsip_en and manifest_config.rsip_mode == 2

        if os.path.exists(tmp_manifest_source_file): os.remove(tmp_manifest_source_file)

        #Step1: create encrypt file and manifest file
        self.encrypt_and_update_manifest_source(input_file, ImageType.IMAGE1, tmp_en_file_name, tmp_manifest_source_file)

        manifest_file_name = os.path.join(self.output_image_dir, 'manifest_boot.bin') #output manifest file
        res = self.manifest_manager.create_manifest(manifest_file_name, tmp_manifest_source_file, ImageType.IMAGE1)
        if res:
            self.logger.fatal("Failed generating manifest file")
            return res

        #Step2: merge final output file
        merge_files(self.output_file, manifest_file_name) # merge_files api will overwrite output_file file
        if gcm_enable:
            append_files(self.output_file, tmp_gcm_prepend_file_name)
        append_files(self.output_file, tmp_en_file_name)

        return Error.success()

    def process_app(self) -> Error:
        #Structure of file used for generating manifest.bin
        # ┌───────────────────────────────┐
        # │ proj1: image2_gcm_prepend.bin │  <- optional(only when image2 gcm mode enabled)
        # ├───────────────────────────────┤
        # │ proj1: image2_all.bin         │
        # ├───────────────────────────────┤
        # │ proj2: image2_gcm_prepend.bin │  <- optional(only when image2 gcm mode enabled)
        # ├───────────────────────────────┤
        # │ proj2: image2_all.bin         │
        # ├───────────────────────────────┤
        # │ proj2: image3_gcm_prepend.bin │  <- optional(only when image3 and image3 gcm mode enabled)
        # ├───────────────────────────────┤
        # │ proj2: image3_all.bin         │  <- optional(only when image3 enabled)
        # ├───────────────────────────────┤
        # │ proj3: image2_gcm_prepend.bin │  <- optional(only when proj3:image2 and image2 gcm mode enabled)
        # ├───────────────────────────────┤
        # │ proj3: image2_all.bin         │  <- optional(only when proj3:image2 enabled)
        # └───────────────────────────────┘


        #Final output secure file's structure
        # ┌───────────────────────────────┐
        # │           cert.bin            │
        # ├───────────────────────────────┤
        # │         manifest.bin          │
        # ├───────────────────────────────┤
        # │ proj1: image2_gcm_prepend.bin │  <- optional(only when image2 gcm mode enabled)
        # ├───────────────────────────────┤
        # │ proj1: image2_all_en.bin      │
        # ├───────────────────────────────┤
        # │ proj2: image2_gcm_prepend.bin │  <- optional(only when image2 gcm mode enabled)
        # ├───────────────────────────────┤
        # │ proj2: image2_all_en.bin      │
        # ├───────────────────────────────┤
        # │ proj2: image3_gcm_prepend.bin │  <- optional(only when image3 and image3 gcm mode enabled)
        # ├───────────────────────────────┤
        # │ proj2: image3_all_en.bin      │  <- optional(only when image3 enabled)
        # ├───────────────────────────────┤
        # │ proj3: image2_gcm_prepend.bin │  <- optional(only when proj3:image2 and image2 gcm mode enabled)
        # ├───────────────────────────────┤
        # │ proj3: image2_all_en.bin      │  <- optional(only when proj3:image2 enabled)
        # └───────────────────────────────┘

        #Final output non-secure file's structure
        # ┌───────────────────────────────┐
        # │           cert.bin            │ <- manifest.bin is same as secure file
        # ├───────────────────────────────┤
        # │         manifest.bin          │ <- manifest.bin is same as secure file
        # ├───────────────────────────────┤
        # │ proj1: image2_all_all.bin     │
        # ├───────────────────────────────┤
        # │ proj2: image2_all_all.bin     │
        # ├───────────────────────────────┤
        # │ proj2: image3_all_all.bin     │  <- optional(only when image3 enabled)
        # ├───────────────────────────────┤
        # │ proj3: image3_all_all.bin     │  <- optional(only when proj3:image2 enabled)
        # └───────────────────────────────┘

        #Step1: create encrypt file and manifest/cert file
        tmp_manifest_source_file = os.path.join(self.output_image_dir, 'app_manifest_source.bin') #file for creating manifest
        if os.path.exists(tmp_manifest_source_file): os.remove(tmp_manifest_source_file)

        for img2 in self.context.args.image2:
            tmp_en_file_name = modify_file_path(img2, suffix='_en')             #encrypted file
            self.encrypt_and_update_manifest_source(img2, ImageType.IMAGE2, tmp_en_file_name, tmp_manifest_source_file)

            if self.context.args.image3:
                #WARNING: Here image3's file path is used to determine which image2 it should be packaged behind during the packaging process
                img2_info = parse_project_info(img2)
                img3_info = parse_project_info(self.context.args.image3)
                if img2_info['mcu_project'] == img3_info['mcu_project']:
                    tmp_en_file_name = modify_file_path(self.context.args.image3, suffix='_en')             #encrypted file
                    self.encrypt_and_update_manifest_source(self.context.args.image3, ImageType.IMAGE3, tmp_en_file_name, tmp_manifest_source_file)

        manifest_file_name = os.path.join(self.output_image_dir, 'manifest_app.bin') #output manifest file
        cert_file_name = os.path.join(self.output_image_dir, 'app_cert.bin') #output cert file
        res = self.manifest_manager.create_manifest(manifest_file_name, tmp_manifest_source_file, ImageType.IMAGE2)
        if res:
            self.logger.fatal("Failed generating manifest file")
            return res
        res = self.manifest_manager.create_cert(cert_file_name, 0, 'image2')
        if res:
            self.logger.fatal("Failed generating cert file")
            return res

        #Step2: merge final output file
        img2_manifest_config = self.manifest_manager.image2
        img3_manifest_config = self.manifest_manager.image3
        img2_gcm_enable = img2_manifest_config.rsip_en and img2_manifest_config.rsip_mode == 2
        if img3_manifest_config == None: #NOTE: manifest maybe not contain image3
            img3_gcm_enable = False
        else:
            img3_gcm_enable = img3_manifest_config.rsip_en and img3_manifest_config.rsip_mode == 2

        tmp_ns_file_name = modify_file_path(self.output_file, suffix='_ns')# non-secure app file
        merge_files(tmp_ns_file_name, cert_file_name, manifest_file_name)  # merge_files api will overwrite output_file file
        merge_files(self.output_file, cert_file_name, manifest_file_name)  # merge_files api will overwrite output_file file

        for img2 in self.context.args.image2:
            tmp_en_file_name = modify_file_path(img2, suffix='_en')             #encrypted file

            if img2_gcm_enable:
                tmp_gcm_prepend_file_name = modify_file_path(tmp_en_file_name, suffix='_tag_prepend')
                append_files(self.output_file, tmp_gcm_prepend_file_name)
            append_files(self.output_file, tmp_en_file_name)
            append_files(tmp_ns_file_name, img2)

            if self.context.args.image3:
                #WARNING: Here image3's file path is used to determine which image2 it should be packaged behind during the packaging process
                img2_info = parse_project_info(img2)
                img3_info = parse_project_info(self.context.args.image3)
                if img2_info['mcu_project'] == img3_info['mcu_project']:
                    tmp_en_file_name = modify_file_path(self.context.args.image3, suffix='_en')             #encrypted file
                    if img3_gcm_enable:
                        tmp_gcm_prepend_file_name = modify_file_path(tmp_en_file_name, suffix='_tag_prepend')
                        append_files(self.output_file, tmp_gcm_prepend_file_name)
                    append_files(self.output_file, tmp_en_file_name)
                    append_files(tmp_ns_file_name, self.context.args.image3)
        return Error.success()

    def process_fullmac_image1(self) -> Error:
        #Final output file's structure
        # ┌───────────────────────────┐
        # │ fullmac_ram_1_prepend.bin │
        # ├───────────────────────────┤
        # │       manifest.bin        │
        # └───────────────────────────┘

        manifest_file_name = os.path.join(self.output_image_dir, 'manifest_fullmac_image1.bin') #output manifest file
        res = self.manifest_manager.create_manifest(manifest_file_name, self.context.args.fullmac_image1, ImageType.IMAGE1)
        if res:
            self.logger.fatal("Failed generating manifest file")
            return res
        #NOTE: manifest file is behind input file
        merge_files(self.output_file, self.context.args.fullmac_image1, manifest_file_name)  # merge_files api will overwrite output_file file
        return Error.success()

    def process_fullmac_image2(self) -> Error:
        #Final output file's structure
        # ┌────────────────────────────┐
        # │ fullmac_sram_2_prepend.bin │
        # ├────────────────────────────┤
        # │        manifest.bin        │
        # └────────────────────────────┘

        manifest_file_name = os.path.join(self.output_image_dir, 'manifest_fullmac_image2.bin') #output manifest file
        res = self.manifest_manager.create_manifest(manifest_file_name, self.context.args.fullmac_image2, ImageType.IMAGE2)
        if res:
            self.logger.fatal("Failed generating manifest file")
            return res
        #NOTE: manifest file is behind input file
        merge_files(self.output_file, self.context.args.fullmac_image2, manifest_file_name)  # merge_files api will overwrite output_file file
        return Error.success()

    def process_imgtool_floader(self) -> Error:
        #Final output file's structure
        # ┌───────────────────┐
        # │ ram_1_prepend.bin │
        # ├───────────────────┤
        # │   manifest.bin    │
        # └───────────────────┘

        manifest_file_name = os.path.join(self.output_image_dir, 'manifest_imgtool_floader.bin') #output manifest file
        res = self.manifest_manager.create_manifest(manifest_file_name, self.context.args.imgtool_floader, ImageType.IMAGE1)
        if res:
            self.logger.fatal("Failed generating manifest file")
            return res
        #NOTE: manifest file is behind input file
        merge_files(self.output_file, self.context.args.imgtool_floader, manifest_file_name)  # merge_files api will overwrite output_file file
        return Error.success()

    def encrypt_and_update_manifest_source(self, input_file:str, image_type, output_encrypt_file:str, manifest_source_file:str) -> Error:
        info = parse_project_info(input_file)
        if info['mcu_project'] != self.output_project:
            # copy input file to output_image_dir if not in
            shutil.copy(input_file, self.output_image_dir)
            input_file = modify_file_path(input_file, new_directory=self.output_image_dir)

        manifest_config = self.manifest_manager.get_image_config(image_type)
        gcm_enable = manifest_config.rsip_en and manifest_config.rsip_mode == 2

        tmp_en_file_name = output_encrypt_file           #encrypted file
        tmp_gcm_file_name = modify_file_path(tmp_en_file_name, suffix='_tag')   #gcm tag file, file name role in security.py
        tmp_gcm_prepend_file_name = modify_file_path(tmp_gcm_file_name, suffix='_prepend')  #prepend gcm tag file

        #Step1: create encrypt file and manifest file
        if manifest_config.rsip_en:
            section = self.config.section(image_type)
            if isinstance(section, dict):
                #WARNING: Here image1/image2's section is get by it's belonging project
                section = section[info["mcu_project"]]
            if section == None:
                self.logger.fatal(f"No section for {input_file} to do rsip")
                return Error(ErrorType.UNKNOWN_ERROR, f"No section for {input_file} to rsip")
            Rsip.execute(self.context, tmp_en_file_name, input_file, section)
            if gcm_enable:
                Pad.execute(self.context, tmp_gcm_file_name, 32)
                PrependHeader.execute(self.context, tmp_gcm_prepend_file_name, tmp_gcm_file_name)
                #NOTE: manifest file should contains gcm info
                append_files(manifest_source_file, tmp_gcm_prepend_file_name)
        elif manifest_config.rdp_en:
            Rdp.execute(self.context, tmp_en_file_name, input_file, 'enc', ImageType.IMAGE1)
        else:
            self.logger.warning(f"Both rsip and rdp are not enabled for {image_type.name.lower()}")
            shutil.copy(input_file, tmp_en_file_name)
        append_files(manifest_source_file, input_file)
        return Error.success()