@echo off
SET JLinkGDBServer_Path=C:\"Program Files (x86)"\SEGGER\JLink\JLinkGDBServer.exe

IF NOT EXIST %JLinkGDBServer_Path% ( 
ECHO %JLinkGDBServer_Path% is NOT EXIST, Please Check your JlinkGDBServer_Path in kr4_jlink.bat
ECHO Jlink version with v7.22 or later is recommanded
PAUSE && EXIT 
)

%JLinkGDBServer_Path% -select USB -device RV32 -if cjtag -port 2331 -nolocalhostonly
