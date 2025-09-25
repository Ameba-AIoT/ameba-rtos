echo "Start JLinkGDBServer..."
/opt/SEGGER/JLink/JLinkGDBServer -device cortex-m33 -if SWD -scriptfile AP1_KM4.JLinkScript -port 2335

if [ $? -eq 0 ]; then
    echo "JLinkGDBServer success"
else
    echo "JLinkGDBServer fail"
    exit 1
fi