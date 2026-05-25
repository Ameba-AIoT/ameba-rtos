@echo off
REM Wrapper for `claude mcp add ameba-dev`. Injects prebuilts toolchain PATH.
REM Independent of env.bat (which ends with cmd /k and would hang).

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..\..\..\") do set "SDK_ROOT=%%~fI"
if "%SDK_ROOT:~-1%"=="\" set "SDK_ROOT=%SDK_ROOT:~0,-1%"

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
"%MCP_BIN%" %*
exit /b %ERRORLEVEL%
