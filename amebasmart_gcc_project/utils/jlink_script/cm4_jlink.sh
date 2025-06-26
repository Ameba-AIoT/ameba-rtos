echo "Start JLink..."
/opt/SEGGER/JLink/JLinkExe -device cortex-m33 -if SWD -jlinkscriptfile AP1_KM4.JLinkScript

if [ $? -eq 0 ]; then
    echo "JLink success"
else
    echo "JLink fail"
    exit 1
fi