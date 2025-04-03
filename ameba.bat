@echo off

setlocal enabledelayedexpansion
title AMEBA CMD

if defined RTK_TOOLCHAIN_DIR (
  set "RTK_TOOLCHAIN_DIR=%RTK_TOOLCHAIN_DIR%"
) else (
  set "RTK_TOOLCHAIN_DIR=C:\rtk-toolchain"
)

set "PREBUILTS_VERSION=1.0.1"
set "PREBUILTS_DIR=%RTK_TOOLCHAIN_DIR%\prebuilts-win-%PREBUILTS_VERSION%"
set "PREBUILTS_WIN_URL=https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/prebuilts-v1.0.1/prebuilts-win-1.0.1.zip"
set "PREBUILTS_WIN_URL_ALIYUN=https://rs-wn.oss-cn-shanghai.aliyuncs.com/prebuilts-win-1.0.1.zip"

set "BASE_DIR=%~dp0"
if "%BASE_DIR:~-1%"=="\" set "BASE_DIR=%BASE_DIR:~0,-1%"

if exist "%PREBUILTS_DIR%\setenv.bat" (
	call "%PREBUILTS_DIR%\setenv.bat"
) else (

	if exist "%BASE_DIR%\.venv" rmdir /s /q "%BASE_DIR%\.venv"

	set "lastMatchedFolder="

	for /d %%D in ("%RTK_TOOLCHAIN_DIR%\prebuilts-win*") do (
		set "currentFolder=%%~nxD"
		if "!lastMatchedFolder!" lss "!currentFolder!" set "lastMatchedFolder=!currentFolder!"
	)

	if defined lastMatchedFolder (
		echo use !lastMatchedFolder! to update prebuilts
		call "%RTK_TOOLCHAIN_DIR%\!lastMatchedFolder!\setenv.bat"
		if not exist "%PREBUILTS_DIR%.zip" (
			echo download....
			wget -P %RTK_TOOLCHAIN_DIR% %PREBUILTS_WIN_URL% || (
				echo Try to download from %PREBUILTS_WIN_URL_ALIYUN%
				wget -P %RTK_TOOLCHAIN_DIR% %PREBUILTS_WIN_URL_ALIYUN% || (
					echo Download failed. Please check your network or download manually
					goto end
				)
			)
		)
		echo unzip....
		7z x "%PREBUILTS_DIR%.zip" -o%RTK_TOOLCHAIN_DIR%

		call "%PREBUILTS_DIR%\setenv.bat"

	) else (
		echo RTK software suite not exist or outdated, please download newest version from %PREBUILTS_WIN_URL%  or %PREBUILTS_WIN_URL_ALIYUN% and unzip it at %RTK_TOOLCHAIN_DIR%
		goto end
	)

)

if exist "%BASE_DIR%\.venv\Scripts\activate.bat" (
	call "%BASE_DIR%\.venv\Scripts\activate.bat"
) else (
	echo python virtual environment is not detected, it will be installed automatically...
	python -m virtualenv "%BASE_DIR%\.venv"
	call "%BASE_DIR%\.venv\Scripts\activate.bat"
	pip install -r "%BASE_DIR%\tools\requirements.txt"
	echo These python modules are installed:
	pip list
)

python "%BASE_DIR%\tools\scripts\check_requirements.py"

DOSKEY build.py=python build.py $*
DOSKEY menuconfig.py=python menuconfig.py $*


set "info1=First choose IC platform : cd [IC]_gcc_project"
set "info2=Configure command:  menuconfig.py"
set "info3=Build command:  build.py"

echo ================================================================================
echo.
echo ^|  %info1%
echo ^|  %info2%
echo ^|  %info3%
echo.
echo ================================================================================


:end
cmd.exe /k

