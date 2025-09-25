echo "Start JLink..."
/opt/SEGGER/JLink/JLinkExe -device Cortex-M33 -if SWD -jlinkscriptfile AP0_KM4.JLinkScript

if [ $? -eq 0 ]; then
    echo "JLink connection success"
else
    echo "JLink connection fail"
    exit 1
fi