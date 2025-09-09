@echo off

setlocal enabledelayedexpansion
title AMEBA CMD

if defined RTK_TOOLCHAIN_DIR (
  set "RTK_TOOLCHAIN_DIR=%RTK_TOOLCHAIN_DIR%"
) else (
  set "RTK_TOOLCHAIN_DIR=C:\rtk-toolchain"
)

set "PREBUILTS_VERSION=1.0.3"
set "PREBUILTS_DIR=%RTK_TOOLCHAIN_DIR%\prebuilts-win-%PREBUILTS_VERSION%"
set "PREBUILTS_WIN_URL=https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/prebuilts-v1.0.3/prebuilts-win-1.0.3.zip"
set "PREBUILTS_WIN_URL_ALIYUN=https://rs-wn.oss-cn-shanghai.aliyuncs.com/prebuilts-win-1.0.3.zip"

set "BASE_DIR=%~dp0"
if "%BASE_DIR:~-1%"=="\" set "BASE_DIR=%BASE_DIR:~0,-1%"

set "EXPECTED_VENV=%BASE_DIR%\.venv"
if defined VIRTUAL_ENV (
	set "clean_venv=!VIRTUAL_ENV!"
	if "!clean_venv:~-1!"=="\" set "clean_venv=!clean_venv:~0,-1!"

	if /i "!clean_venv!"=="!EXPECTED_VENV!" (
		goto skip_venv_setup
	) else (
		echo Detected different virtual environment: !EXPECTED_VENV! !clean_venv!
		if exist "!VIRTUAL_ENV!\Scripts\deactivate.bat" (
			call "!VIRTUAL_ENV!\Scripts\deactivate.bat"
		)
	)
) else (
	echo not in virtual
)

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
		7z x "%PREBUILTS_DIR%.zip" -o%RTK_TOOLCHAIN_DIR% || (
			echo unzip failed. Please unzip %PREBUILTS_DIR%.zip manually
			goto end
		)
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

:skip_venv_setup
python "%BASE_DIR%\tools\scripts\check_requirements.py"

DOSKEY build.py=python build.py $*
DOSKEY menuconfig.py=python menuconfig.py $*
DOSKEY flash.py=python flash.py $*
DOSKEY monitor.py=python monitor.py $*

set "info1=First choose IC platform : cd [IC]_gcc_project"
set "info2=Configure command:  menuconfig.py"
set "info3=Build command:  build.py"
set "info4=Flash command:  flash.py -p COMx"
set "info5=Monitor command:  monitor.py -p COMx"

echo ================================================================================
echo ^|  %info1%
echo ^|  %info2%
echo ^|  %info3%
echo ^|  %info4%
echo ^|  %info5%
echo ================================================================================

:end
cmd.exe /k