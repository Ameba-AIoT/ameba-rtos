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
	goto :prebuilts_done
)

if exist "%BASE_DIR%\.venv" rmdir /s /q "%BASE_DIR%\.venv"

set "lastMatchedFolder="

for /d %%D in ("%RTK_TOOLCHAIN_DIR%\prebuilts-win*") do (
	set "currentFolder=%%~nxD"
	if "!lastMatchedFolder!" lss "!currentFolder!" set "lastMatchedFolder=!currentFolder!"
)

if defined lastMatchedFolder (
	if "!lastMatchedFolder!" neq "prebuilts-win-%PREBUILTS_VERSION%" (
		echo !lastMatchedFolder! is out of date, updating prebuilts...
	) else (
		echo !lastMatchedFolder! is incomplete, reinstalling...
	)
	rmdir /s /q "%RTK_TOOLCHAIN_DIR%\!lastMatchedFolder!" >nul 2>&1
) else (
	echo RTK software suite not found, downloading for first-time setup...
)

if exist "%PREBUILTS_DIR%.zip" (
	tar -tf "%PREBUILTS_DIR%.zip" >nul 2>&1
	if !errorlevel! neq 0 (
		echo Incomplete download detected, re-downloading...
		del /f /q "%PREBUILTS_DIR%.zip"
	)
)
if not exist "%PREBUILTS_DIR%.zip" (
	where curl.exe >nul 2>&1
	if !errorlevel! neq 0 (
		echo curl.exe not found. Please download manually from:
		echo   %PREBUILTS_WIN_URL%
		echo   %PREBUILTS_WIN_URL_ALIYUN%
		echo and unzip it at %RTK_TOOLCHAIN_DIR%
		goto end
	)
	echo download....
	curl.exe -fL# -o "%PREBUILTS_DIR%.zip" "%PREBUILTS_WIN_URL%"
	if !errorlevel! neq 0 (
		if exist "%PREBUILTS_DIR%.zip" del /f /q "%PREBUILTS_DIR%.zip"
		echo Try to download from %PREBUILTS_WIN_URL_ALIYUN%
		curl.exe -fL# -o "%PREBUILTS_DIR%.zip" "%PREBUILTS_WIN_URL_ALIYUN%"
		if !errorlevel! neq 0 (
			if exist "%PREBUILTS_DIR%.zip" del /f /q "%PREBUILTS_DIR%.zip"
			echo Download failed. Please download manually from:
			echo   %PREBUILTS_WIN_URL%
			echo   %PREBUILTS_WIN_URL_ALIYUN%
			echo and unzip it at %RTK_TOOLCHAIN_DIR%
			goto end
		)
	)
)
where tar.exe >nul 2>&1
if !errorlevel! neq 0 (
	echo tar.exe not found. Please unzip %PREBUILTS_DIR%.zip manually to %RTK_TOOLCHAIN_DIR%
	goto end
)
if exist "%PREBUILTS_DIR%" (
	rmdir /s /q "%PREBUILTS_DIR%" >nul 2>&1
	if exist "%PREBUILTS_DIR%" (
		echo Error: Cannot remove "%PREBUILTS_DIR%". Please close any program locking it and retry.
		goto end
	)
)
echo unzip....
tar -xf "%PREBUILTS_DIR%.zip" -C "%RTK_TOOLCHAIN_DIR%" || (
	echo unzip failed. Please unzip %PREBUILTS_DIR%.zip manually to %RTK_TOOLCHAIN_DIR%
	goto end
)
if not exist "%PREBUILTS_DIR%\setenv.bat" (
	echo Error: setenv.bat not found after extraction. Prebuilts may be corrupted.
	goto end
)
call "%PREBUILTS_DIR%\setenv.bat"

:prebuilts_done
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

set "AMEBA_SDK=%BASE_DIR%"

:skip_venv_setup
python "%BASE_DIR%\tools\scripts\check_requirements.py"

DOSKEY build.py=python build.py $*
DOSKEY menuconfig.py=python menuconfig.py $*
DOSKEY flash.py=python flash.py $*
DOSKEY monitor.py=python monitor.py $*
DOSKEY ameba.py=python %BASE_DIR%\ameba.py $*

set "info1=Setup complete. The Ameba SDK is now ready for use."
set "info2=Go to the project root directory and run ameba.py"
set "info3=Usage:      ameba.py [COMMAND] [ARGS]"
set "info4=Build:      ameba.py build"
set "info5=Select SoC: ameba.py soc"
set "info6=Help:       ameba.py help"

echo ================================================================================
echo ^|  %info1%
echo ^|  %info2%
echo ^|  %info3%
echo ^|  %info4%
echo ^|  %info5%
echo ^|  %info6%
echo ================================================================================

:end
cmd.exe /k