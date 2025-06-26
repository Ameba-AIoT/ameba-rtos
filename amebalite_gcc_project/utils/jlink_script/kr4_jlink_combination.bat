@echo off

SET JLinkGDBServer_Path="C:\Program Files (x86)\SEGGER\JLink\JLinkGDBServer.exe"
SET JLink_Path="C:\Program Files (x86)\SEGGER\JLink\JLink.exe"

IF NOT EXIST %JLinkGDBServer_Path% ( 
ECHO %JLinkGDBServer_Path% is NOT EXIST, Please Check your JlinkGDBServer_Path in kr4_jlink.bat
ECHO Jlink version with v7.22 or later is recommanded
PAUSE && EXIT 
)

start "J-Link Commander" %JLink_Path% -device CORTEX-M33 -if SWD -autoconnect 1 -speed 1000 -JLinkScriptFile KM4_SEL.JLinkScript
choice /t 3 /d y /n >nul

:cmd_quit
tasklist /nh|find /i "JLink.exe"
if errorlevel 1 (echo Ready to go) else (taskkill /im JLink.exe /f)

start "J-Link Commander" %JLink_Path% -device RV32 -if cjtag
choice /t 1 /d y /n >nul
