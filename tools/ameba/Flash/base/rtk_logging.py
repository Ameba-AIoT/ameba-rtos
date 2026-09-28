#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import re
import sys
import uuid
import stat
import atexit
import logging
import threading
import faulthandler
from datetime import datetime

from colorama import Fore, Style, init

from .rtk_utils import RtkUtils

# init Colorama
init(autoreset=True)

_crash_log_stream = None
_crash_handler_lock = threading.Lock()
_crash_handlers_registered = False
_previous_sys_excepthook = None
_previous_threading_excepthook = None
_previous_unraisablehook = None


def create_default_log_file(serial_ports=None):
    log_dir = os.path.join(RtkUtils.get_executable_root_path(), "log")
    os.makedirs(log_dir, exist_ok=True)
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    port_suffix = ""
    if serial_ports is not None and len(serial_ports) == 1:
        safe_port = re.sub(r"[^A-Za-z0-9._-]+", "_", str(serial_ports[0])).strip("._-")
        port_suffix = f"_{safe_port or 'port'}"
    log_file = os.path.join(
        log_dir, f"{timestamp}_{uuid.uuid4().hex[:8]}{port_suffix}.log")
    with open(log_file, "x", encoding="utf-8"):
        pass
    return log_file


def prepare_output_log_file(output_log_file):
    output_path = os.path.abspath(output_log_file)
    output_dir = os.path.dirname(output_path)
    if output_dir:
        os.makedirs(output_dir, exist_ok=True)

    if os.path.exists(output_path):
        if not os.path.isfile(output_path):
            raise OSError(f"Log file path is not a regular file: {output_path}")
        if not os.stat(output_path).st_mode & stat.S_IWRITE:
            raise PermissionError(f"Log file is read-only: {output_path}")

    with open(output_path, "ab"):
        pass
    return output_path


def register_log_file_append(default_log_file, output_log_file):
    if output_log_file is None:
        return

    output_path = os.path.abspath(output_log_file)

    def append_log_at_exit():
        logging.shutdown()
        try:
            with open(default_log_file, "rb") as source, open(output_path, "ab") as output:
                output.write(source.read())
        except OSError as err:
            print(f"Append default log to {output_path} failed: {err}", file=sys.stderr)

    atexit.register(append_log_at_exit)


def flush_logger(logger, sync=False):
    for handler in logger.handlers:
        try:
            handler.flush()
            if sync and isinstance(handler, logging.FileHandler):
                os.fsync(handler.stream.fileno())
        except (OSError, ValueError):
            pass


def register_crash_handlers(logger, default_log_file):
    global _crash_log_stream
    global _crash_handlers_registered
    global _previous_sys_excepthook
    global _previous_threading_excepthook
    global _previous_unraisablehook

    with _crash_handler_lock:
        if _crash_handlers_registered:
            return

        _previous_sys_excepthook = sys.excepthook
        _previous_threading_excepthook = getattr(threading, "excepthook", None)
        _previous_unraisablehook = getattr(sys, "unraisablehook", None)

        def main_exception_hook(exc_type, exc_value, exc_traceback):
            try:
                if issubclass(exc_type, KeyboardInterrupt):
                    logger.error("Interrupted by user")
                else:
                    logger.critical(
                        "Unhandled exception in main thread",
                        exc_info=(exc_type, exc_value, exc_traceback))
                flush_logger(logger, sync=True)
            except Exception:
                _previous_sys_excepthook(exc_type, exc_value, exc_traceback)

        def thread_exception_hook(args):
            try:
                thread_name = args.thread.name if args.thread is not None else "unknown"
                logger.critical(
                    f"Unhandled exception in thread {thread_name}",
                    exc_info=(args.exc_type, args.exc_value, args.exc_traceback))
                flush_logger(logger, sync=True)
            except Exception:
                if _previous_threading_excepthook is not None:
                    _previous_threading_excepthook(args)

        def unraisable_exception_hook(args):
            try:
                logger.error(
                    f"Unraisable exception in {args.object!r}: {args.err_msg or ''}",
                    exc_info=(args.exc_type, args.exc_value, args.exc_traceback))
                flush_logger(logger, sync=True)
            except Exception:
                if _previous_unraisablehook is not None:
                    _previous_unraisablehook(args)

        sys.excepthook = main_exception_hook
        if _previous_threading_excepthook is not None:
            threading.excepthook = thread_exception_hook
        if _previous_unraisablehook is not None:
            sys.unraisablehook = unraisable_exception_hook

        try:
            _crash_log_stream = open(
                default_log_file, "a", encoding="utf-8", buffering=1)
            faulthandler.enable(file=_crash_log_stream, all_threads=True)
        except (OSError, RuntimeError) as err:
            if _crash_log_stream is not None:
                try:
                    _crash_log_stream.close()
                except OSError:
                    pass
                _crash_log_stream = None
            logger.warning(f"Enable fatal error logging failed: {err}")

        _crash_handlers_registered = True


def dump_all_thread_tracebacks(logger, reason):
    logger.error(f"{reason}; dumping all thread tracebacks")
    flush_logger(logger)

    if _crash_log_stream is None:
        logger.error("Thread traceback dump unavailable: crash log stream is not initialized")
        flush_logger(logger, sync=True)
        return

    try:
        faulthandler.dump_traceback(file=_crash_log_stream, all_threads=True)
        _crash_log_stream.flush()
        os.fsync(_crash_log_stream.fileno())
    except (OSError, RuntimeError, ValueError) as err:
        logger.error(f"Dump all thread tracebacks failed: {err}")
        flush_logger(logger, sync=True)


def close_crash_handlers():
    global _crash_log_stream
    global _crash_handlers_registered

    with _crash_handler_lock:
        if not _crash_handlers_registered:
            return

        sys.excepthook = _previous_sys_excepthook
        if _previous_threading_excepthook is not None:
            threading.excepthook = _previous_threading_excepthook
        if _previous_unraisablehook is not None:
            sys.unraisablehook = _previous_unraisablehook

        if faulthandler.is_enabled():
            faulthandler.disable()
        if _crash_log_stream is not None:
            try:
                _crash_log_stream.close()
            except OSError:
                pass
            _crash_log_stream = None
        _crash_handlers_registered = False


def create_logger(name, log_level="INFO", stream=sys.stdout, file=None):
    log_level = log_level.upper() if log_level else "INFO"
    if log_level == "DEBUG":
        level = logging.DEBUG
    elif log_level == "WARNING":
        level = logging.WARNING
    elif log_level == "ERROR":
        level = logging.ERROR
    elif log_level == "FATAL":
        level = logging.FATAL
    else:
        level = logging.INFO

    logger = logging.getLogger(name)

    formatter = logging.Formatter(
        fmt=f'[%(asctime)s.%(msecs)03d][%(levelname)s] [{name}]%(message)s',
        datefmt='%Y-%m-%d %H:%M:%S')

    # addLevelName is global and idempotent; safe to (re)set on every call.
    logging.addLevelName(logging.DEBUG, f"D")
    logging.addLevelName(logging.INFO, f"I")
    logging.addLevelName(logging.WARNING, f"{Fore.YELLOW}W{Style.RESET_ALL}")
    logging.addLevelName(logging.ERROR, f"{Fore.RED}E{Style.RESET_ALL}")
    logging.addLevelName(logging.FATAL, f"{Fore.RED}{Style.BRIGHT}F{Style.RESET_ALL}")

    # File handler: reconcile with the requested `file` on every call so that
    # toggling "Save Log" at runtime takes effect. Drop stale file handlers
    # (different path or logging now disabled) and add one if needed.
    wanted = os.path.abspath(file) if file else None
    kept = None
    for h in [h for h in logger.handlers if isinstance(h, logging.FileHandler)]:
        if wanted is not None and os.path.abspath(h.baseFilename) == wanted:
            kept = h
        else:
            logger.removeHandler(h)
            h.close()
    if wanted is not None and kept is None:
        fileHandler = logging.FileHandler(file, mode='a')
        fileHandler.setFormatter(formatter)
        logger.addHandler(fileHandler)

    # Console handler: add once per logger (FileHandler is a StreamHandler
    # subclass, so exclude it when checking for an existing console handler).
    has_console = any(
        isinstance(h, logging.StreamHandler) and not isinstance(h, logging.FileHandler)
        for h in logger.handlers
    )
    if not has_console:
        consoleHandler = logging.StreamHandler(stream)
        consoleHandler.setFormatter(formatter)
        logger.addHandler(consoleHandler)

    # Reorder existing handlers as well. This is needed when the GUI enables
    # "Save Log" at runtime: the new file handler would otherwise be appended
    # after the console handler created by an earlier call.
    file_handlers = [h for h in logger.handlers if isinstance(h, logging.FileHandler)]
    console_handlers = [
        h for h in logger.handlers
        if isinstance(h, logging.StreamHandler) and not isinstance(h, logging.FileHandler)
    ]
    other_handlers = [
        h for h in logger.handlers
        if h not in file_handlers and h not in console_handlers
    ]
    ordered_handlers = file_handlers + console_handlers + other_handlers
    if logger.handlers != ordered_handlers:
        for h in logger.handlers[:]:
            logger.removeHandler(h)
        for h in ordered_handlers:
            logger.addHandler(h)

    logger.propagate = False  # Prevent logging from propagating to the root logger
    logger.setLevel(level)
    return logger
