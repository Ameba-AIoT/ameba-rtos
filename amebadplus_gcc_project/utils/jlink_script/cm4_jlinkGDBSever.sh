echo "Start JLinkGDBServer..."
/opt/SEGGER/JLink/JLinkGDBServer -select USB -device Cortex-M33 -if SWD -scriptfile AP2_KM4.JLinkScript -port 2335

if [ $? -eq 0 ]; then
    echo "JLinkGDBServer success"
else
    echo "JLinkGDBServer fail"
    exit 1
fi