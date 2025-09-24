echo "Start JLink..."
/opt/SEGGER/JLink/JLinkExe -device Cortex-M23 -if SWD -jlinkscriptfile AP1_KM0.JLinkScript

if [ $? -eq 0 ]; then
    echo "JLink success"
else
    echo "JLink fail"
    exit 1
fi