echo "Start JLink..."
/opt/SEGGER/JLink/JLinkExe -device cortex-m23 -if SWD -jlinkscriptfile AP0_KM0.JLinkScript

if [ $? -eq 0 ]; then
    echo "JLink success"
else
    echo "JLink fail"
    exit 1
fi