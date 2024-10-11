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

function binary_pading()
{
	form_file=$1
	to_file=$2
	size=$3

	dd if=$form_file ibs=1k count=$size >  temp.bin
	dd if=/dev/zero ibs=1k count=$size >> temp.bin
	dd if=temp.bin ibs=1k count=$size > $to_file

	rm temp.bin
}

if [ "$COMPILEOS" == "GNU/Linux" ]; then
	IMAGE_FULLNAME=$1
	BUILD_TYPE=$2
	DSP_IMG_DIR=$3
else
	IMAGE_FULLNAME=$(realpath --relative-to=$(pwd) $1)
	BUILD_TYPE=$(realpath --relative-to=$(pwd) $2)
	DSP_IMG_DIR=$(realpath --relative-to=$(pwd) $3)
fi

IMAGE_FILENAME=$(basename $1)

if [ "$COMPILEOS" == "GNU/Linux" ]; then
	ELF2BIN=./elf2bin
else
	ELF2BIN=./elf2bin.exe
fi

if [ $BUILD_TYPE == "MFG" ]; then
	KM4_IMG_DIR=../../../../project_km4/asdk/image_mp
	KR4_IMG_DIR=../../../../project_kr4/vsdk/image_mp
else
	KM4_IMG_DIR=../../../../project_km4/asdk/image
	KR4_IMG_DIR=../../../../project_kr4/vsdk/image
fi

MANIFEST_JSON=../../../../manifest.json

if [ "$IMAGE_FILENAME" == "kr4_image2_all.bin" ] || [ "$IMAGE_FILENAME" == "km4_image2_all.bin" ]; then
	if [ -f $KR4_IMG_DIR/kr4_image2_all.bin ]; then
		if [ ! -d $KM4_IMG_DIR ]; then
			mkdir -p $KM4_IMG_DIR
		fi
		cp $KR4_IMG_DIR/kr4_image2_all.bin $KM4_IMG_DIR
	fi

	if [ ! -f $KM4_IMG_DIR/km4_image2_all.bin ]; then
		exit
	fi
	
	if [ ! -f $KM4_IMG_DIR/kr4_image2_all.bin ]; then
		exit
	fi

	if [ -f $KM4_IMG_DIR/km4_image3_all.bin ]; then
		cat $KM4_IMG_DIR/kr4_image2_all.bin $KM4_IMG_DIR/km4_image2_all.bin $KM4_IMG_DIR/km4_image3_all.bin > $KM4_IMG_DIR/kr4_km4_app.bin
	else
		cat $KM4_IMG_DIR/kr4_image2_all.bin $KM4_IMG_DIR/km4_image2_all.bin > $KM4_IMG_DIR/kr4_km4_app.bin
	fi

	$ELF2BIN cert $MANIFEST_JSON $MANIFEST_JSON $KM4_IMG_DIR/cert.bin 0 app
	if [ -f $DSP_IMG_DIR/dsp.bin ]; then
		cat $KM4_IMG_DIR/kr4_km4_app.bin $DSP_IMG_DIR/dsp.bin > $KM4_IMG_DIR/kr4_km4_dsp_app.bin
		$ELF2BIN manifest $MANIFEST_JSON $MANIFEST_JSON $KM4_IMG_DIR/kr4_km4_dsp_app.bin $KM4_IMG_DIR/manifest.bin app
		$ELF2BIN rsip $DSP_IMG_DIR/dsp.bin $DSP_IMG_DIR/dsp_en.bin 0x0D000000 $MANIFEST_JSON app
	else
		$ELF2BIN manifest $MANIFEST_JSON $MANIFEST_JSON  $KM4_IMG_DIR/kr4_km4_app.bin $KM4_IMG_DIR/manifest.bin app
	fi
	$ELF2BIN rsip $KM4_IMG_DIR/kr4_image2_all.bin $KM4_IMG_DIR/kr4_image2_all_en.bin 0x0c000000 $MANIFEST_JSON app
	$ELF2BIN rsip $KM4_IMG_DIR/km4_image2_all.bin $KM4_IMG_DIR/km4_image2_all_en.bin 0x0e000000 $MANIFEST_JSON app

	if [ -f $KM4_IMG_DIR/km4_image3_all_en.bin ]; then
		cat $KM4_IMG_DIR/cert.bin $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/kr4_image2_all.bin $KM4_IMG_DIR/km4_image2_all.bin $KM4_IMG_DIR/km4_image3_all.bin > $KM4_IMG_DIR/kr4_km4_app_ns.bin
		cat $KM4_IMG_DIR/cert.bin $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/kr4_image2_all_en.bin $KM4_IMG_DIR/km4_image2_all_en.bin $KM4_IMG_DIR/km4_image3_all_en.bin > $KM4_IMG_DIR/kr4_km4_app.bin
	else
		cat $KM4_IMG_DIR/cert.bin $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/kr4_image2_all.bin $KM4_IMG_DIR/km4_image2_all.bin > $KM4_IMG_DIR/kr4_km4_app_ns.bin
		cat $KM4_IMG_DIR/cert.bin $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/kr4_image2_all_en.bin $KM4_IMG_DIR/km4_image2_all_en.bin > $KM4_IMG_DIR/kr4_km4_app.bin
	fi

	if [ -f $DSP_IMG_DIR/dsp_en.bin ]; then
		cat $KM4_IMG_DIR/kr4_km4_app_ns.bin $DSP_IMG_DIR/dsp.bin > $KM4_IMG_DIR/kr4_km4_dsp_app_ns.bin
		cat $KM4_IMG_DIR/kr4_km4_app.bin $DSP_IMG_DIR/dsp_en.bin > $KM4_IMG_DIR/kr4_km4_dsp_app.bin
		rm -rf $KM4_IMG_DIR/kr4_km4_app.bin $KM4_IMG_DIR/kr4_km4_app_ns.bin
	else
		rm -rf $KM4_IMG_DIR/kr4_km4_dsp_app.bin $KM4_IMG_DIR/kr4_km4_dsp_app_ns.bin
	fi

	if [ $BUILD_TYPE == "MFG" ]; then
		if [ -f $KM4_IMG_DIR/kr4_km4_app.bin ]; then
			mv $KM4_IMG_DIR/kr4_km4_app_ns.bin $KM4_IMG_DIR/kr4_km4_app_ns_mp.bin
			mv $KM4_IMG_DIR/kr4_km4_app.bin $KM4_IMG_DIR/kr4_km4_app_mp.bin
		fi
		if [ -f $KM4_IMG_DIR/kr4_km4_dsp_app.bin ]; then
			mv $KM4_IMG_DIR/kr4_km4_dsp_app_ns.bin $KM4_IMG_DIR/kr4_km4_dsp_app_ns_mp.bin
			mv $KM4_IMG_DIR/kr4_km4_dsp_app.bin $KM4_IMG_DIR/kr4_km4_dsp_app_mp.bin
		fi
	fi
fi

if [ "$IMAGE_FILENAME" == "kr4_image2_all_shrink.bin" ] || [ "$IMAGE_FILENAME" == "km4_image2_all_shrink.bin" ]; then
	if [ -f $KR4_IMG_DIR/kr4_image2_all_shrink.bin ]; then
		if [ ! -d $KM4_IMG_DIR ]; then
			mkdir -p $KM4_IMG_DIR
		fi
		cp $KR4_IMG_DIR/kr4_image2_all_shrink.bin $KM4_IMG_DIR
	fi

	if [ ! -f $KM4_IMG_DIR/km4_image2_all_shrink.bin ]; then
		exit
	fi
	
	if [ ! -f $KM4_IMG_DIR/km4_image2_all_shrink.bin ]; then
		exit
	fi

	cat $KM4_IMG_DIR/km4_image2_all_shrink.bin $KM4_IMG_DIR/kr4_image2_all_shrink.bin > $KM4_IMG_DIR/kr4_km4_app.bin


	if [ $BUILD_TYPE == "MFG" ]; then
		if [ -f $KM4_IMG_DIR/kr4_km4_app.bin ]; then
			mv $KM4_IMG_DIR/kr4_km4_app.bin $KM4_IMG_DIR/kr4_km4_app_mp.bin
		fi
	fi
fi


if [ "$IMAGE_FILENAME" == "km4_boot_all.bin" ]; then
	$ELF2BIN manifest $MANIFEST_JSON $MANIFEST_JSON $KM4_IMG_DIR/km4_boot_all.bin $KM4_IMG_DIR/manifest.bin boot
	$ELF2BIN rsip $KM4_IMG_DIR/km4_boot_all.bin $KM4_IMG_DIR/km4_boot_all_en.bin 0x0F800000 $MANIFEST_JSON boot

	cat $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km4_boot_all.bin > $KM4_IMG_DIR/km4_boot_all_ns.bin	
	cat $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km4_boot_all_en.bin > $KM4_IMG_DIR/km4_boot_all.bin
	rm -rf $KM4_IMG_DIR/km4_boot_all_en.bin
fi

if [ "$IMAGE_FILENAME" == "km4_boot_all_mp.bin" ]; then
	$ELF2BIN manifest $MANIFEST_JSON $MANIFEST_JSON $KM4_IMG_DIR/km4_boot_all_mp.bin $KM4_IMG_DIR/manifest.bin boot
	$ELF2BIN rsip $KM4_IMG_DIR/km4_boot_all_mp.bin $KM4_IMG_DIR/km4_boot_all_mp_en.bin 0x0F800000 $MANIFEST_JSON boot

	cat $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km4_boot_all_mp.bin > $KM4_IMG_DIR/km4_boot_all_mp_ns.bin	
	cat $KM4_IMG_DIR/manifest.bin $KM4_IMG_DIR/km4_boot_all_mp_en.bin > $KM4_IMG_DIR/km4_boot_all_mp.bin
	rm -rf $KM4_IMG_DIR/km4_boot_all_mp_en.bin
fi

if [ "$IMAGE_FILENAME" == "ram_1_prepend.bin" ]; then
	$ELF2BIN manifest $MANIFEST_JSON $MANIFEST_JSON $KM4_IMG_DIR/ram_1_prepend.bin $KM4_IMG_DIR/manifest.bin boot
	cat $KM4_IMG_DIR/ram_1_prepend.bin $KM4_IMG_DIR/manifest.bin > $KM4_IMG_DIR/imgtool_flashloader.bin
fi

if [ "$IMAGE_FILENAME" == "km4_image3_all.bin" ]; then
	$ELF2BIN rdp enc $KM4_IMG_DIR/km4_image3_all.bin $KM4_IMG_DIR/km4_image3_all_en.bin $MANIFEST_JSON
fi
