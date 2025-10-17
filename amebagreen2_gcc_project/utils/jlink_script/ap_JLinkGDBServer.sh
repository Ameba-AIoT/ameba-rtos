echo "Start JLinkGDBServer..."
/opt/SEGGER/JLink/JLinkGDBServer -select USB -device Cortex-M33 -if swd -scriptfile AP2_AP.JLinkScript -port 2335

if [ $? -eq 0 ]; then
    echo "JLinkGDBServer connection success"
else
    echo "JLinkGDBServer connection fail"
    exit 1
fi

