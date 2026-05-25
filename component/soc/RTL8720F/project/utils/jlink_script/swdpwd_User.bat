@echo off
SET JLinkGDBServer_Path="C:\Program Files (x86)\SEGGER\JLink\JLinkGDBServer.exe"

IF NOT EXIST %JLinkGDBServer_Path% ( 
ECHO %JLinkGDBServer_Path% is NOT EXIST, Please Check your JlinkGDBServer_Path in swdpwd_User.bat
ECHO Jlink version with v7.22 or later is recommanded
PAUSE && EXIT 
)

%JLinkGDBServer_Path% -select USB -device Cortex-M33 -if swd -scriptfile AP3_SWDPWD_User.JLinkScript -port 2335 -log swdpwd_password.log