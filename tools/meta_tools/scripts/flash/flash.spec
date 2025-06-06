# -*- mode: python ; coding: utf-8 -*-
import sys
from pathlib import Path
sys.path.insert(0, str((Path("__name__").parent)))
from tempfile import  NamedTemporaryFile
from version_info import version

temp_version_file=Path("version.tmp")
temp_version_file.write_text(
f"""# UTF-8 encoding
# UTF-8
#
# Fields to define the version info for a Windows executable
VSVersionInfo(
    ffi=FixedFileInfo(
        filevers=({version.replace(".",",")}),       # 文件版本号
        prodvers=({version.replace(".",",")}),       # 产品版本号
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
            StringStruct(u'FileDescription', u'Flash Tool for Realtek Ameba SoCs'),
            StringStruct(u'FileVersion', u'{version}'),
            StringStruct(u'InternalName', u'flash.exe'),
            StringStruct(u'LegalCopyright', u'Copyright (c) 2025 Realtek Semiconductor Corp.'),
            StringStruct(u'OriginalFilename', u'flash.exe'),
            StringStruct(u'ProductName', u'Ameba Flash Tool'),
            StringStruct(u'ProductVersion', u'{version}')])
        ]),
        VarFileInfo([VarStruct(u'Translation', [1033, 1200])])
    ]
)
""",encoding='utf-8')


a = Analysis(
    ['flash.py'],
    pathex=[],
    binaries=[],
    datas=[],
    hiddenimports=[],
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
    name='flash',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=True,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
    version=temp_version_file.name,
)
os.remove(temp_version_file.name)