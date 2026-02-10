#!/bin/bash

rom_folder=rom_code_to

ap_debug_folder=rom_code_to/ap_debug
np_debug_folder=rom_code_to/np_debug

rm -rf $rom_folder/*
mkdir -p $ap_debug_folder
mkdir -p $np_debug_folder

#copy ap rom & flash
#copy ap rom & flash
if [ `grep -c "CONFIG_AMEBAGREEN2_A_CUT=y" ../../menuconfig/.config_km4tz` -ne '0' ]; then
	rom_cut_folder=amebagreen2_rom_acut
elif [ `grep -c "CONFIG_AMEBAGREEN2_B_CUT=y" ../../menuconfig/.config_km4tz` -ne '0' ]; then
	rom_cut_folder=amebagreen2_rom_bcut
else
	echo "error rom cut!!!"
	exit 1
fi

cp ../asdk/lib/$rom_cut_folder/ap_rom.bin $rom_folder/
cp ../asdk/lib/$rom_cut_folder/share_rom.bin $rom_folder/

cp ../asdk/image/amebagreen2_boot.bin $rom_folder/
cp ../asdk/image/amebagreen2_app.bin $rom_folder/
# if [ -f ../../project_km4tz/asdk/image/ap_image3_all.bin ]; then
# 	cp ../../project_km4tz/asdk/image/ap_image3_all.bin $rom_folder
# fi


#copy ap debug related file
cp ../asdk/image/*.axf $ap_debug_folder/
cp ../asdk/image/*.map $ap_debug_folder/
cp ../asdk/image/*.asm $ap_debug_folder/

#copy np debug related file
cp ../../project_km4ns/asdk/image/*.axf $np_debug_folder/
cp ../../project_km4ns/asdk/image/*.map $np_debug_folder/
cp ../../project_km4ns/asdk/image/*.asm $np_debug_folder/
