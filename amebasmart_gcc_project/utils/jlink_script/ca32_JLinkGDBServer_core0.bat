@echo off
SET JLinkGDBServer_Path=C:\"Program Files (x86)"\SEGGER\JLink\JLinkGDBServer.exe

IF NOT EXIST %JLinkGDBServer_Path% (
ECHO %JLinkGDBServer_Path% is NOT EXIST, Please Check your JlinkGDBServer_Path in ca32_jlinkGDBServer_core0.bat
ECHO Jlink version with v7.22 or later is recommanded
PAUSE && EXIT
)
TITLE=CORE0
%JLinkGDBServer_Path% -select USB -device Cortex-A32 -if swd -scriptfile AP3_CA32_Core0.JLinkScript -port 2337 -nolocalhostonly
