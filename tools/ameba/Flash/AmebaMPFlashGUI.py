#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import sys
from PySide6.QtGui import QIcon
from PySide6.QtWidgets import QApplication
from app.mp_flashtool_main import MPFlashToolMain

if __name__ == "__main__":
    # In frozen (onefile) mode __file__ == exe path; bundled resources live in _MEIPASS
    _root = getattr(sys, '_MEIPASS', os.path.dirname(os.path.abspath(__file__)))
    icon_path = os.path.join(_root, "app", "icon", "mp_flashtool.ico")

    if sys.platform == "win32":
        import ctypes
        ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(
            "Realtek.AmebaMPFlashTool"
        )

    app = QApplication(sys.argv)
    app.setStyle("Fusion")

    if os.path.exists(icon_path):
        app.setWindowIcon(QIcon(icon_path))

    window = MPFlashToolMain()
    window.show()

    # Force taskbar icon on Windows via Win32 WM_SETICON
    if sys.platform == "win32" and os.path.exists(icon_path):
        try:
            LR_LOADFROMFILE = 0x10
            LR_DEFAULTSIZE = 0x40
            IMAGE_ICON = 1
            hicon = ctypes.windll.user32.LoadImageW(
                None, icon_path, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE
            )
            if hicon:
                hwnd = int(window.winId())
                WM_SETICON = 0x0080
                ctypes.windll.user32.SendMessageW(hwnd, WM_SETICON, 1, hicon)  # ICON_BIG
                ctypes.windll.user32.SendMessageW(hwnd, WM_SETICON, 0, hicon)  # ICON_SMALL
        except Exception:
            pass

    sys.exit(app.exec())
