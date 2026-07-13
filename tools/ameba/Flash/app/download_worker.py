import logging
import os
from copy import deepcopy

from PySide6.QtCore import QThread, Signal

from base import Ameba, create_logger
from base.errno import ErrType
from base.flash_utils import FlashBPS
from base.image_info import ImageInfo
from base.memory_info import MemoryInfo
from app.progress_log_handler import ProgressLogHandler
from version_info import gui_version


class DownloadWorker(QThread):
    progress_updated = Signal(int)   # 0-100
    finished = Signal(bool, str)     # success, message

    def __init__(self, port, baudrate, profile_info, settings,
                 image_path, start_address, chip_erase,
                 split_by_layout=True,
                 log_file=None, log_level="info", parent=None):
        super().__init__(parent)
        self.port = port
        self.baudrate = baudrate
        # profile_info already carries the user's Layout edits (image name,
        # addresses, memory type, mandatory / full-erase); the GUI syncs the
        # table into it before starting each run.
        self.profile_info = profile_info
        self.settings = settings
        self.image_path = image_path
        self.start_address = start_address
        self.chip_erase = chip_erase
        self.split_by_layout = split_by_layout
        self.log_file = log_file
        self.log_level = log_level

    def run(self):
        logger = create_logger(self.port, log_level=self.log_level.upper(), file=self.log_file)
        progress_handler = ProgressLogHandler(
            lambda pct: self.progress_updated.emit(pct)
        )
        logger.addHandler(progress_handler)

        # Record the GUI tool version at the top of the (optionally saved) log so a
        # saved log file always identifies which GUI build produced it (mirrors the
        # CLI's "AmebaFlash Version: ..." line).
        logger.info(f"AmebaMPFlashGUI Version: {gui_version}")

        # Capture the last error-level log message so failures surface a
        # meaningful reason (e.g. serial port already in use) instead of a
        # bare "Exit(1)" when the download flow aborts via sys.exit().
        last_error = {"msg": None}

        class _ErrorCaptureHandler(logging.Handler):
            def emit(self, record):
                if record.levelno >= logging.ERROR:
                    last_error["msg"] = record.getMessage()

        error_handler = _ErrorCaptureHandler()
        logger.addHandler(error_handler)

        try:
            # Memory type is derived from the device profile. The GUI has no
            # explicit memory-type selector, so when a single image is downloaded
            # to a RAM address, force RAM to override the profile's memory type
            # (mirrors the CLI's '-m ram').
            memory_type = getattr(self.profile_info, 'memory_type', MemoryInfo.MEMORY_TYPE_NOR)

            image_dir = None
            images_info = None
            if os.path.isfile(self.image_path):
                if self.profile_info.is_ram_address(self.start_address):
                    memory_type = MemoryInfo.MEMORY_TYPE_RAM
                    logger.info(
                        f"Start address 0x{self.start_address:08X} is a RAM address, "
                        f"memory type overridden to RAM")
                img = ImageInfo()
                img.image_name = self.image_path
                img.start_address = self.start_address
                img.end_address = self.start_address + os.path.getsize(self.image_path)
                img.memory_type = memory_type
                img.mandatory = True
                # A single image may actually be a merged image (e.g. image_all.bin)
                # that spans several partitions of the device profile layout. When
                # the user selected Layout regions, split it by layout and skip the
                # all-FF padding between/after partitions instead of programming it
                # (mirrors the CLI single-image path). When the image does not line
                # up with any layout partition, expand_merged_image falls back to
                # trimming the trailing padding, so this is safe for plain
                # single-partition writes. When no Layout region is selected,
                # split_by_layout is False and the whole image is programmed as-is.
                img.split_by_layout = self.split_by_layout
                images_info = [img]

                # Work on a private copy of the profile so layout-based splitting
                # for this run cannot affect other ports sharing the same object.
                # The Mandatory / Full Erase choices are already baked into
                # profile_info by the GUI, so expand_merged_image emits only the
                # selected partitions.
                if self.split_by_layout:
                    self.profile_info = deepcopy(self.profile_info)
            elif os.path.isdir(self.image_path):
                image_dir = self.image_path
            else:
                self.finished.emit(False, "Invalid image path")
                return

            def make_ameba():
                return Ameba(
                    self.profile_info, self.port, self.baudrate,
                    image_dir, self.settings, logger,
                    download_img_info=deepcopy(images_info),
                    chip_erase=self.chip_erase,
                    memory_type=memory_type,
                    erase_info=None,
                )

            ameba = make_ameba()

            if not ameba.check_protocol_for_download():
                ameba.clean_up()
                self.finished.emit(False, "Protocol check failed")
                return

            if memory_type == MemoryInfo.MEMORY_TYPE_NOR:
                ret, is_reburn = ameba.check_supported_flash_size()
                if ret != ErrType.OK:
                    ameba.clean_up()
                    self.finished.emit(False, f"Flash size check failed: {ret}")
                    return
                if is_reburn:
                    ameba.clean_up()
                    ameba = make_ameba()
                    ret = ameba.prepare()
                else:
                    ret = ameba.show_device_info()
            else:
                ret = ameba.prepare()

            if ret != ErrType.OK:
                ameba.clean_up()
                self.finished.emit(False, f"Prepare failed: {ret}")
                return

            ret = ameba.verify_images()
            if ret != ErrType.OK:
                ameba.clean_up()
                self.finished.emit(False, f"Image verify failed: {ret}")
                return

            if not ameba.is_all_ram:
                ret = ameba.post_verify_images()
                if ret != ErrType.OK:
                    ameba.clean_up()
                    self.finished.emit(False, f"Post verify failed: {ret}")
                    return

            flash_status = FlashBPS()
            if not ameba.is_all_ram:
                ret = ameba.check_and_process_flash_lock(flash_status)
                if ret != ErrType.OK:
                    ameba.clean_up()
                    self.finished.emit(False, f"Flash lock check failed: {ret}")
                    return

            ret = ameba.download_images()
            if ret != ErrType.OK:
                ameba.clean_up()
                self.finished.emit(False, f"Download failed: {ret}")
                return

            if not ameba.is_all_ram and flash_status.need_unlock:
                ret = ameba.lock_flash(flash_status.protection)
                if ret != ErrType.OK:
                    ameba.clean_up()
                    self.finished.emit(False, f"Restore flash protection failed: {ret}")
                    return

            ret = ameba.post_process()
            if ret != ErrType.OK:
                ameba.clean_up()
                self.finished.emit(False, f"Post process failed: {ret}")
                return

            ameba.clean_up()
            self.finished.emit(True, "PASS")

        except SystemExit as e:
            if e.code == 0:
                self.finished.emit(True, "PASS")
            else:
                self.finished.emit(False, last_error["msg"] or f"Exit({e.code})")
        except Exception as e:
            self.finished.emit(False, last_error["msg"] or str(e))
        finally:
            try:
                logger.removeHandler(progress_handler)
                logger.removeHandler(error_handler)
            except Exception:
                pass
