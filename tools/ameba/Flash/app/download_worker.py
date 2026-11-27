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


class DownloadWorker(QThread):
    progress_updated = Signal(int)   # 0-100
    result_ready = Signal(bool, str)  # success, message

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
        logger = None
        progress_handler = None
        error_handler = None
        ameba = None
        flash_status = None
        stage_errors = []

        def run_stage(action, *args, **kwargs):
            stage_errors.clear()
            return action(*args, **kwargs)

        def failure_message(default):
            return "\n".join(stage_errors) if stage_errors else default

        try:
            logger = create_logger(
                self.port, log_level=self.log_level.upper(), file=self.log_file
            )
            # The progress is parsed from the INFO "Programming progress" log, which a WARNING
            # or higher logger level drops before any handler. Apply the selected level to the
            # console/file handlers instead, and let the logger pass INFO to the progress handler
            selected_level = logger.level
            if selected_level > logging.INFO:
                for handler in logger.handlers:
                    handler.setLevel(selected_level)
                logger.setLevel(logging.INFO)
            else:
                for handler in logger.handlers:
                    handler.setLevel(logging.NOTSET)
            progress_handler = ProgressLogHandler(
                lambda pct: self.progress_updated.emit(pct)
            )
            logger.addHandler(progress_handler)

            # Capture all error lines from one stage so multi-line diagnostics
            # (for example device/profile mismatches) remain visible in the GUI.
            class _ErrorCaptureHandler(logging.Handler):
                def emit(self, record):
                    if record.levelno >= logging.ERROR:
                        stage_errors.append(record.getMessage())

            error_handler = _ErrorCaptureHandler()
            logger.addHandler(error_handler)

            # Flash session by default: the NOR/NAND flash type is reported by the device,
            # the device profile does not tell it. The GUI has no memory-type selector, so
            # a single image downloaded to a RAM address is a RAM session (mirrors the
            # CLI's '-m ram').
            memory_type = MemoryInfo.PARTITION_TYPE_FLASH

            image_dir = None
            images_info = None
            if os.path.isfile(self.image_path):
                if self.profile_info.is_ram_address(self.start_address):
                    memory_type = MemoryInfo.PARTITION_TYPE_RAM
                    logger.info(
                        f"Start address 0x{self.start_address:08X} is a RAM address, "
                        f"memory type overridden to RAM")
                img = ImageInfo()
                img.image_name = self.image_path
                img.start_address = self.start_address
                img.end_address = self.start_address + os.path.getsize(self.image_path)
                # On NAND the end is extended to the covered layout partitions after
                # QUERY, see Ameba.fit_single_image_to_layout()
                img.end_from_image_size = True
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
                self.result_ready.emit(False, "Invalid image path")
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

            ameba = run_stage(make_ameba)

            if not run_stage(ameba.check_protocol_for_download):
                ameba.clean_up()
                self.result_ready.emit(
                    False, failure_message("Protocol check failed")
                )
                return

            # Flash: SPIC address mode check, NOR only and skipped inside on NAND, as the
            # flash type is only known after QUERY
            if memory_type != MemoryInfo.PARTITION_TYPE_RAM:
                ret, is_reburn = run_stage(ameba.check_supported_flash_size)
                if ret != ErrType.OK:
                    ameba.clean_up()
                    self.result_ready.emit(
                        False,
                        failure_message(f"Flash size check failed: {ret}"),
                    )
                    return
                if is_reburn:
                    ameba.clean_up()
                    ameba = run_stage(make_ameba)
                    ret = run_stage(ameba.prepare)
                else:
                    ret = run_stage(ameba.show_device_info)
            else:
                ret = run_stage(ameba.prepare)

            if ret != ErrType.OK:
                ameba.clean_up()
                self.result_ready.emit(
                    False, failure_message(f"Prepare failed: {ret}")
                )
                return

            ret = run_stage(ameba.verify_images)
            if ret != ErrType.OK:
                ameba.clean_up()
                self.result_ready.emit(
                    False, failure_message(f"Image verify failed: {ret}")
                )
                return

            if not ameba.is_all_ram:
                ret = run_stage(ameba.post_verify_images)
                if ret != ErrType.OK:
                    ameba.clean_up()
                    self.result_ready.emit(
                        False, failure_message(f"Post verify failed: {ret}")
                    )
                    return

            flash_status = FlashBPS()
            if not ameba.is_all_ram:
                ret = run_stage(
                    ameba.check_and_process_flash_lock, flash_status
                )
                if ret != ErrType.OK:
                    # A partly applied unlock is restored as well
                    ameba.restore_flash_lock(flash_status)
                    ameba.clean_up()
                    self.result_ready.emit(
                        False,
                        failure_message(f"Flash lock check failed: {ret}"),
                    )
                    return

            ret = run_stage(ameba.download_images)
            if ret != ErrType.OK:
                # Restore the block protection on failure too: a retry would find the
                # flash unlocked, so nothing would restore it any more
                ameba.restore_flash_lock(flash_status)
                ameba.clean_up()
                self.result_ready.emit(
                    False, failure_message(f"Download failed: {ret}")
                )
                return

            if flash_status.need_unlock:
                ret = run_stage(ameba.restore_flash_lock, flash_status)
                if ret != ErrType.OK:
                    ameba.clean_up()
                    self.result_ready.emit(
                        False,
                        failure_message(
                            f"Restore flash protection failed: {ret}"
                        ),
                    )
                    return

            ret = run_stage(ameba.post_process)
            if ret != ErrType.OK:
                ameba.clean_up()
                self.result_ready.emit(
                    False, failure_message(f"Post process failed: {ret}")
                )
                return

            ameba.clean_up()
            self.result_ready.emit(True, "PASS")

        except SystemExit as e:
            if e.code == 0:
                self.result_ready.emit(True, "PASS")
            else:
                self.result_ready.emit(False, failure_message(f"Exit({e.code})"))
        except Exception as e:
            self.result_ready.emit(False, failure_message(str(e)))
        finally:
            if ameba is not None:
                # Exception raised after the unlock
                if flash_status is not None:
                    try:
                        ameba.restore_flash_lock(flash_status)
                    except Exception as error:
                        if logger is not None:
                            logger.error(f"Restore flash protection failed: {error}")
                try:
                    ameba.clean_up()
                except Exception as error:
                    if logger is not None:
                        logger.error(f"Serial port cleanup failed: {error}")
            if logger is not None:
                try:
                    if progress_handler is not None:
                        logger.removeHandler(progress_handler)
                    if error_handler is not None:
                        logger.removeHandler(error_handler)
                except Exception:
                    pass
