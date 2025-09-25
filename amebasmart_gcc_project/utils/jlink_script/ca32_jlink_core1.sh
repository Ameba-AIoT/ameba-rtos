echo "Start JLink..."
/opt/SEGGER/JLink/JLinkExe -device cortex-a32 -if SWD -jlinkscriptfile AP3_CA32_Core1.JLinkScript

if [ $? -eq 0 ]; then
    echo "JLink success"
else
    echo "JLink fail"
    exit 1
fi