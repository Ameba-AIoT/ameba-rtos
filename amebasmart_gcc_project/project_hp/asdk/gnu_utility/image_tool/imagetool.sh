#!/bin/bash 

################
# Library
################
Usage() {
    echo "Usage: $0 [Image Name]"
}

################
# Main
################
#if [ "$#" -lt 2 ]; then
#    Usage
#    exit 1
#fi

# Get Parameters
COMPILEOS=$(uname -o)

if [ "$COMPILEOS" == "GNU/Linux" ]; then
	TOOL_PATH=$0
	IMAGE_FULLPATH=$1
	ELF2BIN=./elf2bin
else
	TOOL_PATH=$(realpath --relative-to=$(pwd) $0)
	IMAGE_FULLPATH=$(realpath --relative-to=$(pwd) $1)
	ELF2BIN=./elf2bin.exe
fi
IMAGE_FILENAME=$(basename $1)
TOOL_PATH=$(dirname "$TOOL_PATH")
cd $TOOL_PATH

################
# Splice binary file function: Place the files that need to be spliced in ./image_join, use ./imagetoool.sh "join" cmd for splicing and the results is Under the ./image_join path.
################
if [ $IMAGE_FULLPATH == "join" ]; then
	KM4_IMG_DIR=./image_join
	CA32_IMG_DIR=./image_join
	KM0_IMG_DIR=./image_join
	BUILD_TYPE="NONE"
else
	if [ "$COMPILEOS" == "GNU/Linux" ]; then
	BUILD_TYPE=$2
	BUILD_CMD=$3
	else
	BUILD_TYPE=$(realpath --relative-to=$(pwd) $2)
	BUILD_CMD=$(realpath --relative-to=$(pwd) $3)
	fi
fi


if [ $BUILD_TYPE == "MFG" ]; then
	KM4_IMG_DIR=../../../../project_hp/asdk/image_mp
	CA32_IMG_DIR=../../../../project_ap/asdk/image_mp
else
	KM4_IMG_DIR=../../../../project_hp/asdk/image
	CA32_IMG_DIR=../../../../project_ap/asdk/image
fi
KM0_IMG_DIR=../../../../project_lp/asdk/image


MANIFEST_JSON=../../../../manifest.json


function combin_bin_nec(){

	if [ -f $KM4_IMG_DIR/km4_image3_all.bin ]; then
		cat $KM4_IMG_DIR/km0_image2_all.bin $KM4_IMG_DIR/km4_image2_all.bin $KM4_IMG_DIR/km4_image3_all.bin > $KM4_IMG_DIR/km0_km4_app.bin
	else
		cat $KM4_IMG_DIR/km0_image2_all.bin $KM4_IMG_DIR/km4_image2_all.bin > $KM4_IMG_DIR/km0_km4_app.bin
	fi
	
	if [ -f $CA32_IMG_DIR/ap_image_all.bin ]; then
		cat $KM4_IMG_DIR/km0_km4_app.bin $CA32_IMG_DIR/ap_image_all.bin > $KM4_IMG_DIR/km0_km4_ca32_app.bin
		$ELF2BIN manifest $MANIFEST_JSON $MANIFEST_JSON $KM4_IMG_DIR/km0_km4_ca32_app.bin $KM4_IMG_DIR/manifest.bin app
	else
		$ELF2BIN manifest $MANIFEST_JSON $MANIFEST_JSON $KM4_IMG_DIR/km0_km4_app.bin $KM4_IMG_DIR/manifest.bin app
	fi
}

function combin_bin_enc(){

	$ELF2BIN cert $MANIFEST_JSON $MANIFEST_JSON $KM4_IMG_DIR/cert.bin 0 app
	if [ -f $KM4_IMG_DIR/km4_image3_all_en.bin ]; then
		cat $KM4_IMG_DIR/cert.bin $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km0_image2_all.bin $KM4_IMG_DIR/km4_image2_all.bin $KM4_IMG_DIR/km4_image3_all.bin > $KM4_IMG_DIR/km0_km4_app_ns.bin
		cat $KM4_IMG_DIR/cert.bin $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km0_image2_all_en.bin $KM4_IMG_DIR/km4_image2_all_en.bin $KM4_IMG_DIR/km4_image3_all_en.bin > $KM4_IMG_DIR/km0_km4_app.bin
	else
		cat $KM4_IMG_DIR/cert.bin $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km0_image2_all.bin $KM4_IMG_DIR/km4_image2_all.bin > $KM4_IMG_DIR/km0_km4_app_ns.bin
		cat $KM4_IMG_DIR/cert.bin $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km0_image2_all_en.bin $KM4_IMG_DIR/km4_image2_all_en.bin > $KM4_IMG_DIR/km0_km4_app.bin
	fi
	
	if [ -f $CA32_IMG_DIR/ap_image_all_en.bin ]; then
		cat $KM4_IMG_DIR/km0_km4_app_ns.bin $CA32_IMG_DIR/ap_image_all.bin > $KM4_IMG_DIR/km0_km4_ca32_app_ns.bin
		cat $KM4_IMG_DIR/km0_km4_app.bin $CA32_IMG_DIR/ap_image_all_en.bin > $KM4_IMG_DIR/km0_km4_ca32_app.bin
	else
		rm -rf $KM4_IMG_DIR/km0_km4_ca32_app.bin $KM4_IMG_DIR/km0_km4_ca32_app_ns.bin
	fi
}

function copy_to_mp_image(){
	if [ -f $KM4_IMG_DIR/km0_km4_app.bin ]; then
		mv $KM4_IMG_DIR/km0_km4_app.bin $KM4_IMG_DIR/km0_km4_app_mp.bin
		mv $KM4_IMG_DIR/km0_km4_app_ns.bin $KM4_IMG_DIR/km0_km4_app_ns_mp.bin
	fi

	if [ -f $KM4_IMG_DIR/km0_km4_ca32_app.bin ]; then
		mv $KM4_IMG_DIR/km0_km4_ca32_app.bin $KM4_IMG_DIR/km0_km4_ca32_app_mp.bin
		mv $KM4_IMG_DIR/km0_km4_ca32_app_ns.bin $KM4_IMG_DIR/km0_km4_ca32_app_ns_mp.bin
	fi
}

function main_join(){

	if [ -f $KM0_IMG_DIR/km0_image2_all.bin ]; then
		$ELF2BIN rsip $KM0_IMG_DIR/km0_image2_all.bin $KM0_IMG_DIR/km0_image2_all_en.bin 0x0c000000 $MANIFEST_JSON app
	fi
	
	if [ -f $KM0_IMG_DIR/km4_image2_all.bin ]; then
		$ELF2BIN rsip $KM4_IMG_DIR/km4_image2_all.bin $KM4_IMG_DIR/km4_image2_all_en.bin 0x0d000000 $MANIFEST_JSON app
	fi
	
	if [ -f $KM4_IMG_DIR/km4_image3_all.bin ]; then
		$ELF2BIN rdp enc $KM4_IMG_DIR/km4_image3_all.bin $KM4_IMG_DIR/km4_image3_all_en.bin $MANIFEST_JSON
	fi
	
	if [ -f $CA32_IMG_DIR/ap_image_all.bin ]; then
		$ELF2BIN rsip $CA32_IMG_DIR/ap_image_all.bin $CA32_IMG_DIR/ap_image_all_en.bin 0x0e000000 $MANIFEST_JSON app
	fi
	combin_bin_nec
	combin_bin_enc
	echo "Splicing completed."
}

function main_bin(){

	if [ "$IMAGE_FILENAME" == "km0_image2_all.bin" ]; then
		if [ ! -d $KM4_IMG_DIR ]; then
			mkdir -p $KM4_IMG_DIR
		fi
		if [ -f $KM0_IMG_DIR/km0_image2_all.bin ]; then
			cp $KM0_IMG_DIR/km0_image2_all.bin $KM4_IMG_DIR
			$ELF2BIN rsip $KM0_IMG_DIR/km0_image2_all.bin $KM0_IMG_DIR/km0_image2_all_en.bin 0x0c000000 $MANIFEST_JSON app
			cp $KM0_IMG_DIR/km0_image2_all_en.bin $KM4_IMG_DIR
		fi
	fi

	if [ "$IMAGE_FILENAME" == "km4_boot_all.bin" ]; then
		$ELF2BIN manifest $MANIFEST_JSON $MANIFEST_JSON $KM4_IMG_DIR/km4_boot_all.bin $KM4_IMG_DIR/manifest.bin boot
		$ELF2BIN rsip $KM4_IMG_DIR/km4_boot_all.bin $KM4_IMG_DIR/km4_boot_all_en.bin 0x0a000000 $MANIFEST_JSON boot
	
		cat $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km4_boot_all.bin > $KM4_IMG_DIR/km4_boot_all_ns.bin
		cat $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km4_boot_all_en.bin > $KM4_IMG_DIR/km4_boot_all.bin
		rm -rf $KM4_IMG_DIR/km4_boot_all_en.bin
		exit
	fi

	if [ "$IMAGE_FILENAME" == "ram_1_prepend.bin" ]; then
		$ELF2BIN manifest $MANIFEST_JSON $MANIFEST_JSON $KM4_IMG_DIR/ram_1_prepend.bin $KM4_IMG_DIR/manifest.bin boot
		cat $KM4_IMG_DIR/ram_1_prepend.bin $KM4_IMG_DIR/manifest.bin > $KM4_IMG_DIR/imgtool_flashloader.bin
		exit
	fi

	if [ "$IMAGE_FILENAME" == "km4_image2_all.bin" ]; then
		$ELF2BIN rsip $KM4_IMG_DIR/km4_image2_all.bin $KM4_IMG_DIR/km4_image2_all_en.bin 0x0d000000 $MANIFEST_JSON app
	fi
	
	if [ "$IMAGE_FILENAME" == "km4_image3_all.bin" ]; then
		$ELF2BIN rdp enc $KM4_IMG_DIR/km4_image3_all.bin $KM4_IMG_DIR/km4_image3_all_en.bin $MANIFEST_JSON
		exit
	fi

	if [ "$IMAGE_FILENAME" == "ap_image_all.bin" ]; then
		if [ -f $CA32_IMG_DIR/ap_image_all.bin ]; then
			$ELF2BIN rsip $CA32_IMG_DIR/ap_image_all.bin $CA32_IMG_DIR/ap_image_all_en.bin 0x0e000000 $MANIFEST_JSON app
		fi
		exit
	fi
	
	if [ ! -f $KM4_IMG_DIR/km4_image2_all.bin ]; then
		exit
	fi
	
	if [ ! -f $KM4_IMG_DIR/km0_image2_all.bin ]; then
		exit
	fi

	combin_bin_nec
	combin_bin_enc
	
	if [ $BUILD_TYPE == "MFG" ]; then
		copy_to_mp_image
	fi
}

if [ $IMAGE_FULLPATH == "join" ]; then
	main_join
else
	main_bin
fi