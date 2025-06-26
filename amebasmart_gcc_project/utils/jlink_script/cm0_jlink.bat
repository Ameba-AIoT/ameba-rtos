@echo off
SET JLinkGDBServer_Path=C:\"Program Files (x86)"\SEGGER\JLink\JLink.exe

IF NOT EXIST %JLinkGDBServer_Path% (
ECHO %JLinkGDBServer_Path% is NOT EXIST, Please Check your JlinkGDBServer_Path in cm0_jlink.bat
ECHO Jlink version with v7.22 or later is recommended
PAUSE && EXIT
)

%JLinkGDBServer_Path% -device Cortex-M23 -if swd -jlinkscriptfile AP0_KM0.JLinkScript
