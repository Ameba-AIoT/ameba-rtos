echo "Start JLinkGDBServer..."
/opt/SEGGER/JLink/JLinkGDBServer -select USB -device Cortex-M23 -if SWD -scriptfile AP1_KM0.JLinkScript -port 2331

if [ $? -eq 0 ]; then
    echo "JLinkGDBServer connection success"
else
    echo "JLinkGDBServer connection fail"
    exit 1
fi