@echo off
REM Wrapper for `claude mcp add ameba-dev`. Injects prebuilts toolchain PATH.
REM Independent of env.bat (which ends with cmd /k and would hang).

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..\..\..\") do set "SDK_ROOT=%%~fI"
if "%SDK_ROOT:~-1%"=="\" set "SDK_ROOT=%SDK_ROOT:~0,-1%"

REM Parse (and CONSUME) --project-root so it is NOT forwarded to ameba-mcp.
REM When present, export AMEBA_PROJECT_ROOT (absolute); _paths.py reads it to
REM point the build/flash/config layer at an out-of-tree project.
set "REST_ARGS="
:parse_args
if "%~1"=="" goto after_args
if /I "%~1"=="--project-root" (
    if "%~2"=="" (
        echo [ameba-mcp launcher] --project-root requires a value 1>&2
        exit /b 1
    )
    REM %~f2 always yields a fully-qualified path resolved against cwd.
    set "AMEBA_PROJECT_ROOT=%~f2"
    shift
    shift
    goto parse_args
)
set "REST_ARGS=!REST_ARGS! %1"
shift
goto parse_args
:after_args
if defined AMEBA_PROJECT_ROOT echo [ameba-mcp launcher] AMEBA_PROJECT_ROOT=%AMEBA_PROJECT_ROOT% 1>&2

if not defined RTK_TOOLCHAIN_DIR set "RTK_TOOLCHAIN_DIR=C:\rtk-toolchain"
set "PREBUILTS_VERSION=1.0.3"
set "PREBUILTS_DIR=%RTK_TOOLCHAIN_DIR%\prebuilts-win-%PREBUILTS_VERSION%"
set "MCP_BIN=%SDK_ROOT%\.venv\Scripts\ameba-mcp.exe"

if not exist "%PREBUILTS_DIR%\setenv.bat" (
    echo [ameba-mcp launcher] prebuilts not found at %PREBUILTS_DIR% 1>&2
    echo [ameba-mcp launcher] run "%SDK_ROOT%\env.bat" once to install it 1>&2
    exit /b 1
)
if not exist "%MCP_BIN%" (
    echo [ameba-mcp launcher] %MCP_BIN% missing - run "%SDK_ROOT%\env.bat" to set up venv 1>&2
    exit /b 1
)

call "%PREBUILTS_DIR%\setenv.bat" 1>&2
call "%SDK_ROOT%\.venv\Scripts\activate.bat" 1>&2
"%MCP_BIN%" !REST_ARGS!
exit /b %ERRORLEVEL%
