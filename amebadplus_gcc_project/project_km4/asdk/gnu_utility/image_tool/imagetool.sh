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

cd ../../project_km4/asdk/gnu_utility/image_tool

# Get Parameters
COMPILEOS=$(uname -o)

if [ "$COMPILEOS" == "GNU/Linux" ]; then
	IMAGE_FULLNAME=$1
	BUILD_TYPE=$2
else
	IMAGE_FULLNAME=$(realpath --relative-to=$(pwd) $1)
	BUILD_TYPE=$(realpath --relative-to=$(pwd) $2)
fi

IMAGE_FILENAME=$(basename $1)

if [ "$COMPILEOS" == "GNU/Linux" ]; then
	ELF2BIN=./elf2bin
else
	ELF2BIN=./elf2bin.exe
fi


if [ $BUILD_TYPE == "MFG" ]; then
	KM4_IMG_DIR=../../../../project_km4/asdk/image_mp
	KM0_IMG_DIR=../../../../project_km0/asdk/image_mp
else
	KM4_IMG_DIR=../../../../project_km4/asdk/image
	KM0_IMG_DIR=../../../../project_km0/asdk/image
fi

MANIFEST_JSON=../../../../manifest.json

if [ "$IMAGE_FILENAME" == "km0_image2_all.bin" ] || [ "$IMAGE_FILENAME" == "km4_image2_all.bin" ]; then
	if [ -f $KM0_IMG_DIR/km0_image2_all.bin ]; then
		if [ ! -d $KM4_IMG_DIR ]; then
			mkdir -p $KM4_IMG_DIR
		fi
		cp $KM0_IMG_DIR/km0_image2_all.bin $KM4_IMG_DIR
	fi

	if [ ! -f $KM4_IMG_DIR/km4_image2_all.bin ]; then
		exit
	fi
	
	if [ ! -f $KM4_IMG_DIR/km0_image2_all.bin ]; then
		exit
	fi

	if [ -f $KM4_IMG_DIR/km0_km4_app.bin ]; then
		rm -rf $KM4_IMG_DIR/km0_km4_app.bin
	fi

	if [ -f $KM4_IMG_DIR/km4_image3_all.bin ]; then
		cat $KM4_IMG_DIR/km0_image2_all.bin $KM4_IMG_DIR/km4_image2_all.bin $KM4_IMG_DIR/km4_image3_all.bin > $KM4_IMG_DIR/km0_km4_app_tmp.bin
	else
		cat $KM4_IMG_DIR/km0_image2_all.bin $KM4_IMG_DIR/km4_image2_all.bin > $KM4_IMG_DIR/km0_km4_app_tmp.bin
	fi

	$ELF2BIN cert $MANIFEST_JSON $MANIFEST_JSON $KM4_IMG_DIR/cert.bin 0 app
	$ELF2BIN manifest $MANIFEST_JSON $MANIFEST_JSON $KM4_IMG_DIR/km0_km4_app_tmp.bin $KM4_IMG_DIR/manifest.bin app

	if [ ! -f $KM4_IMG_DIR/manifest.bin ]; then
		exit
	fi
	$ELF2BIN rsip $KM4_IMG_DIR/km0_image2_all.bin $KM4_IMG_DIR/km0_image2_all_en.bin 0x0c000000 $MANIFEST_JSON app
	$ELF2BIN rsip $KM4_IMG_DIR/km4_image2_all.bin $KM4_IMG_DIR/km4_image2_all_en.bin 0x0e000000 $MANIFEST_JSON app

	if [ -f $KM4_IMG_DIR/km4_image3_all_en.bin ]; then
		cat $KM4_IMG_DIR/cert.bin $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km0_image2_all.bin $KM4_IMG_DIR/km4_image2_all.bin $KM4_IMG_DIR/km4_image3_all.bin > $KM4_IMG_DIR/km0_km4_app_ns.bin
		cat $KM4_IMG_DIR/cert.bin $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km0_image2_all_en.bin $KM4_IMG_DIR/km4_image2_all_en.bin $KM4_IMG_DIR/km4_image3_all_en.bin > $KM4_IMG_DIR/km0_km4_app.bin
	else
		cat $KM4_IMG_DIR/cert.bin $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km0_image2_all.bin $KM4_IMG_DIR/km4_image2_all.bin > $KM4_IMG_DIR/km0_km4_app_ns.bin
		cat $KM4_IMG_DIR/cert.bin $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km0_image2_all_en.bin $KM4_IMG_DIR/km4_image2_all_en.bin > $KM4_IMG_DIR/km0_km4_app.bin
	fi

	if [ $BUILD_TYPE == "MFG" ]; then
		if [ -f $KM4_IMG_DIR/km0_km4_app.bin ]; then
			mv $KM4_IMG_DIR/km0_km4_app.bin $KM4_IMG_DIR/km0_km4_app_mp.bin
		fi
	fi
	rm -rf $KM4_IMG_DIR/manifest.bin

fi

if [ "$IMAGE_FILENAME" == "km4_boot.bin" ]; then
	if [ -f $KM4_IMG_DIR/km4_boot_all.bin ]; then
		rm -rf $KM4_IMG_DIR/km4_boot_all.bin
	fi
	$ELF2BIN manifest $MANIFEST_JSON $MANIFEST_JSON $KM4_IMG_DIR/km4_boot.bin $KM4_IMG_DIR/manifest.bin boot
	if [ ! -f $KM4_IMG_DIR/manifest.bin ]; then
		exit
	fi
	$ELF2BIN rsip $KM4_IMG_DIR/km4_boot.bin $KM4_IMG_DIR/km4_boot_en.bin 0x0F800000 $MANIFEST_JSON boot

	cat $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km4_boot.bin > $KM4_IMG_DIR/km4_boot_all_ns.bin
	cat $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km4_boot_en.bin > $KM4_IMG_DIR/km4_boot_all.bin
	rm -rf $KM4_IMG_DIR/km4_boot_en.bin
	rm -rf $KM4_IMG_DIR/manifest.bin

fi

if [ "$IMAGE_FILENAME" == "ram_1_prepend.bin" ]; then
	$ELF2BIN manifest $MANIFEST_JSON $MANIFEST_JSON $KM4_IMG_DIR/ram_1_prepend.bin $KM4_IMG_DIR/manifest.bin boot
	cat $KM4_IMG_DIR/ram_1_prepend.bin $KM4_IMG_DIR/manifest.bin > $KM4_IMG_DIR/imgtool_flashloader.bin
fi

if [ "$IMAGE_FILENAME" == "km4_image3_all.bin" ]; then
	$ELF2BIN rdp enc $KM4_IMG_DIR/km4_image3_all.bin $KM4_IMG_DIR/km4_image3_all_en.bin $MANIFEST_JSON
fi
