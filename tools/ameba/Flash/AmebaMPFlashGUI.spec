# -*- mode: python ; coding: utf-8 -*-
import sys
import os
from pathlib import Path

current_dir = os.path.abspath(SPECPATH if 'SPECPATH' in globals() else '.')
sys.path.insert(0, current_dir)

from version_info import gui_version

# Pad gui_version to 4 components for Windows FILEVERS tuple (e.g. "3.0.0" -> "3,0,0,0")
_parts = gui_version.split('.') + ['0', '0', '0', '0']
_ver_tuple = ','.join(_parts[:4])

temp_version_file = Path("version_mp.tmp")
temp_version_file.write_text(
f"""# UTF-8 encoding
# UTF-8
#
# Fields to define the version info for a Windows executable
VSVersionInfo(
    ffi=FixedFileInfo(
        filevers=({_ver_tuple}),
        prodvers=({_ver_tuple}),
        mask=0x3f,
        flags=0x0,
        OS=0x40004,
        fileType=0x1,
        subtype=0x0,
        date=(0, 0)
        ),
    kids=[
        StringFileInfo(
        [
        StringTable(
            u'040904B0',
            [StringStruct(u'CompanyName', u'Realtek Semiconductor Corp.'),
            StringStruct(u'FileDescription', u'MP Flash Tool for Realtek Ameba SoCs'),
            StringStruct(u'FileVersion', u'{gui_version}'),
            StringStruct(u'InternalName', u'AmebaMPFlashGUI.exe'),
            StringStruct(u'LegalCopyright', u'Copyright (c) 2025 Realtek Semiconductor Corp.'),
            StringStruct(u'OriginalFilename', u'AmebaMPFlashGUI.exe'),
            StringStruct(u'ProductName', u'Ameba MP Flash Tool'),
            StringStruct(u'ProductVersion', u'{gui_version}')])
        ]),
        VarFileInfo([VarStruct(u'Translation', [1033, 1200])])
    ]
)
""", encoding='utf-8')


a = Analysis(
    ['AmebaMPFlashGUI.py'],
    pathex=[],
    binaries=[],
    datas=[
        ('app/icon/mp_flashtool.ico', 'app/icon'),
    ],
    hiddenimports=[
        'serial.tools.list_ports',
        'colorama',
        'base',
        'base.rtk_logging',
        'base.rt_settings',
        'base.image_info',
        'base.memory_info',
        'base.flash_utils',
        'base.errno',
        'base.device_profile',
        'base.json_utils',
        'base.download_handler',
        'base.floader_handler',
        'base.rom_handler',
        'base.device_info',
        'base.rtk_utils',
        'base.config_utils',
        'base.sys_utils',
        'base.version',
        'base.sense_status',
        'base.next_op',
        'base.spic_addr_mode',
        'base.rtk_flash_type',
        'base.efuse_data',
        'app',
        'app.mp_flashtool_main',
        'app.port_progress_widget',
        'app.progress_log_handler',
        'app.download_worker',
    ],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
)

pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.datas,
    [],
    name='AmebaMPFlashGUI',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
    icon='app/icon/mp_flashtool.ico',
    version=temp_version_file.name,
)

os.remove(temp_version_file.name)
