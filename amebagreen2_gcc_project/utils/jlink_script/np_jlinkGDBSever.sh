echo "Start JLinkGDBServer..."
/opt/SEGGER/JLink/JLinkGDBServer -select USB -device Cortex-M33 -if swd -scriptfile AP1_NP.JLinkScript -port 2331

if [ $? -eq 0 ]; then
    echo "JLinkGDBServer success"
else
    echo "JLinkGDBServer fail"
    exit 1
fi
