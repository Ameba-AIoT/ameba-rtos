#!/bin/bash
RESET() {
    killall rtw_aware
    killall dmesg
}
trap RESET EXIT

echo "=========================="
echo "     start log capture    "
echo "=========================="
dmesg -C
dmesg -w > aware_drv.log &

service network-manager stop

#################################################################################
# Usage: ./rtw_aware [-r publish|subscribe] [-f config_file] [option] 
# 
# General : 
#   -b 	 enable bluetooth trigger, default is disabled.
# 
# Publish : 
#   -s 	 open,pmk (Deprecated, the security type setting has been moved to the config file)
#   -t 	 timeout value
#   -n 	 number of service
# subscribe : 
#   -s 	 open,pmk (Deprecated, the security type setting has been moved to the config file)
#   -t 	 timeout value
#################################################################################

sudo env LD_LIBRARY_PATH=./ ./rtw_aware -r publish -n 1 -f aware.conf
#sudo env LD_LIBRARY_PATH=./ ./rtw_aware -r subscribe -f aware.conf
echo "=========================="
echo "     stop log capture     "
echo "=========================="
killall dmesg

sleep 3

if [ ! -d log_folder ]; then
  mkdir log_folder
fi
mv -f *.log log_folder/
chmod 777 -R log_folder
