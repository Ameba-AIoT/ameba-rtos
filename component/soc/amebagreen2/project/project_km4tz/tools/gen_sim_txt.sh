#!/bin/bash

rom_folder=rom_code_to
out_folder=rom_code_to

ap_debug_folder=rom_code_to/ap_debug
np_debug_folder=rom_code_to/np_debug

function binary_pading()
{
	form_file=$1
	to_file=$2
	size=$3

	dd if=$form_file ibs=1k count=$size >  temp.bin
	dd if=/dev/zero bs=1 count=$((size*1024)) | tr '\000' '\377' >> temp.bin
	# dd if=/dev/zero ibs=1k count=$size >> temp.bin
	dd if=temp.bin ibs=1k count=$size > $to_file

	rm temp.bin
}

function binary_to_txt_x16()
{
	form_file=$1
	to_file=$2

	od -t x8 -w16 -v $form_file| cut -c9- | awk '{print $2,$1}' | sed s/[[:space:]]//g > $to_file

# Remove unnecessary last line generate by od command.
	sed '$ d' $to_file > temp.txt && cp temp.txt $to_file

	rm temp.txt
}

function binary_to_txt_x8()
{
	form_file=$1
	to_file=$2

	# Use "od" tool to convert binary file to hexadecimal text format
	od -t x8 -w8 -v $form_file | cut -d " " -f 2 > $to_file

	# Remove unnecessary last line generate by od command.
	sed '$ d' $to_file > temp.txt && cp temp.txt $to_file

	rm temp.txt
}

function binary_to_txt_x4()
{
	form_file=$1
	to_file=$2

	# Use "od" tool to convert binary file to hexadecimal text format
	od -t x4 -w4 -v $form_file | cut -d " " -f 2 > $to_file

	# Remove unnecessary last line generate by od command.
	sed '$ d' $to_file > temp.txt && cp temp.txt $to_file

	rm temp.txt
}

function binary_to_txt_x1()
{
	form_file=$1
	to_file=$2

	# Use "od" tool to convert binary file to hexadecimal text format
	od -t x1 -w1 -v $form_file | cut -d " " -f 2 > $to_file

	# Remove unnecessary last line generate by od command.
	sed '$ d' $to_file > temp.txt && cp temp.txt $to_file

	rm temp.txt
}

#gen AP Secure ROM
binary_pading	$rom_folder/ap_rom.bin			$rom_folder/ap_rom_pad.bin			128
binary_to_txt_x4	$rom_folder/ap_rom_pad.bin 	$out_folder/ap_rom_all.bin.txt

#gen share rom
binary_pading	$rom_folder/share_rom.bin			$rom_folder/share_rom_pad.bin			288
binary_to_txt_x8	$rom_folder/share_rom_pad.bin	$out_folder/share_rom_all.bin.txt

#gen Flash
binary_pading	$rom_folder/amebagreen2_boot.bin	$rom_folder/amebagreen2_boot_pad.bin	256
binary_pading	$rom_folder/amebagreen2_app.bin		$rom_folder/amebagreen2_app_pad.bin		1792
cat		$rom_folder/amebagreen2_boot_pad.bin > 		$rom_folder/flash_all.bin
cat		$rom_folder/amebagreen2_app_pad.bin >>		$rom_folder/flash_all.bin
binary_to_txt_x1	$rom_folder/flash_all.bin	$out_folder/flash_all.bin.txt

str="AmebaGreen2-"`date +"%Y%m%d-%H%M%S"`
time=`date +"%Y%m%d-%H%M%S"`
echo "simulation files folder:"$str
mkdir $str
cp		$out_folder/ap_rom_all.bin.txt		$str/ap_rom.hex
cp		$out_folder/share_rom_all.bin.txt	$str/share_rom.hex
cp		$out_folder/flash_all.bin.txt		$str/flash.hex
# add for combo SPIC Test, extra_option in image_sim.xlsx shall add option -flash_b
cp		$out_folder/flash_all.bin.txt		$str/flash_b.hex

#echo "split rom code for tapeout !!!!!!!!"
sh split_rom.sh $str

cp -r $ap_debug_folder $str
cp -r $np_debug_folder $str

if [ -f image_sim.xlsx ]; then
	mv image_sim.xlsx $str
	tar -zcf xxx_imagesimulation_$time.tar.gz $str
fi

rm $rom_folder/*_pad.bin $rom_folder/*_all.bin.txt

# for PXP upload
cp -rf $str/tap_out	  $out_folder/
cp -rf $str/flash.hex $out_folder/tap_out/
cd $out_folder && zip -rq tap_out.zip tap_out/ && rm -rf tap_out/
