echo "Start JLinkGDBServer..."
/opt/SEGGER/JLink/JLinkGDBServer -select USB -device Cortex-M33 -if SWD -scriptfile AP0_KM4.JLinkScript -port 2335

if [ $? -eq 0 ]; then
    echo "JLinkGDBServer connection success"
else
    echo "JLinkGDBServer connection fail"
    exit 1
fi
