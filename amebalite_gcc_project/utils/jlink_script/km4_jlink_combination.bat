@echo off

SET JLinkGDBServer_Path="C:\Program Files (x86)\SEGGER\JLink\JLink.exe"
SET JLink_Path="C:\Program Files (x86)\SEGGER\JLink\JLink.exe"

IF NOT EXIST %JLinkGDBServer_Path% ( 
ECHO %JLinkGDBServer_Path% is NOT EXIST, Please Check your JlinkGDBServer_Path in cm4_jlink.bat
ECHO Jlink version with v7.22 or later is recommanded
PAUSE && EXIT 
)

%JLinkGDBServer_Path% -device Cortex-M33 -if swd -jlinkscriptfile AP0_KM4.JLinkScript

if %errorlevel% == 0 (
ECHO Close GDBServer Window manually, %errorlevel%
pause
goto cmd_quit

) else (
ECHO GDBServer Window KM4 Connection Failed, %errorlevel%

tasklist /nh|find /i "JLinkGDBServer.exe"
if errorlevel 1 (echo Ready to go) else (taskkill /im JLinkGDBServer.exe /f)

start "J-Link Commander" %JLink_Path% -device RV32 -if cjtag -autoconnect 1 -speed 4000 -JTAGConf -1,-1 -JLinkScriptFile KR4_DMI.JLinkScript
ECHO KR4_DMI.JLinkScript start... %ERRORLEVEL%

choice /t 3 /d y /n >nul

start "J-Link GDB Commander" %JLinkGDBServer_Path% -device Cortex-M33 -if swd -jlinkscriptfile AP0_KM4.JLinkScript
choice /t 1 /d y /n >nul
)

:cmd_quit
tasklist /nh|find /i "JLink.exe"
if errorlevel 1 (echo Ready to go) else (taskkill /im JLink.exe /f)

