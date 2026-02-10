#!/bin/bash 

tap_out=$1/tap_out
mkdir $tap_out

rm -fr *rom*.bin
rm -fr *rom*.txt
rm -fr *rom*.hex

#copy binary
cp rom_code_to/ap_rom_all.bin.txt ./
cp rom_code_to/share_rom_all.bin.txt ./

#gen tape out ap rom hex
split -l 8192	ap_rom_all.bin.txt	ap_rom_ -d -a 1
for romfiles in $(ls ap_rom_[0-9])
	do mv $romfiles $tap_out/$romfiles.hex
done

#gen tape out share rom hex
split -l 4096	share_rom_all.bin.txt	share_rom_ -d -a 1
for romfiles in $(ls share_rom_[0-9])
	do mv $romfiles $tap_out/$romfiles.hex
done

###################debug only##############################
#use pacman -S diffutils to install diff command

#debug  compare ap rom again
rm -f ap_debug_rom.hex
for romfiles in $(ls $tap_out/ap_rom_*)
	do cat $romfiles >> ap_debug_rom.hex
done
diff -q ap_debug_rom.hex ap_rom_all.bin.txt
rm ap_debug_rom.hex


#debug  compare share rom again
rm -f share_debug_rom.hex
for romfiles in $(ls $tap_out/share_rom_*)
	do cat $romfiles >> share_debug_rom.hex
done
diff -q share_debug_rom.hex share_rom_all.bin.txt
rm share_debug_rom.hex

##############debug end###################################

rm ap_rom_all.bin.txt
rm share_rom_all.bin.txt
