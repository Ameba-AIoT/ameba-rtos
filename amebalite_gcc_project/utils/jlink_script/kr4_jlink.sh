/opt/SEGGER/JLink/JLinkExe -device Cortex-M33 -if swd -autoconnect 1 -speed 1000 -JLinkScriptFile KM4_SEL.JLinkScript << EOF
exit
EOF
/opt/SEGGER/JLink/JLinkExe -device RV32 -if cjtag -speed 1000
