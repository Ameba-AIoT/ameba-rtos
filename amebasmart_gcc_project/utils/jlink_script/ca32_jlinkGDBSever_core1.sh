echo "Start JLinkGDBServer..."
/opt/SEGGER/JLink/JLinkGDBServer -device cortex-a32 -if SWD -scriptfile AP3_CA32_Core1.JLinkScript -port 2339

if [ $? -eq 0 ]; then
    echo "JLinkGDBServer success"
else
    echo "JLinkGDBServer fail"
    exit 1
fi