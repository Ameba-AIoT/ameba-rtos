#!/bin/sh

#===============================================================================
CURRENT_UTILITY_DIR=$(pwd)
GNU_SCRIPT=$1
BUILD_TYPE=$2

if [ "$BUILD_TYPE" != "MFG" ]; then
	IMAGE_TARGET_FOLDER=$CURRENT_UTILITY_DIR/image
	NP_IMAGE_TARGET_FOLDER=$CURRENT_UTILITY_DIR/../../project_np/asdk/image/
	AP_IMAGE_TARGET_FOLDER=$CURRENT_UTILITY_DIR/../../project_ap/asdk/image/
	GREEN2_APP=amebagreen2_app
else
	IMAGE_TARGET_FOLDER=$CURRENT_UTILITY_DIR/image_mp
	NP_IMAGE_TARGET_FOLDER=$CURRENT_UTILITY_DIR/../../project_np/asdk/image_mp/
	AP_IMAGE_TARGET_FOLDER=$CURRENT_UTILITY_DIR/../../project_ap/asdk/image_mp/
	GREEN2_APP=amebagreen2_app_mp
fi

echo "..."
echo $BUILD_TYPE
echo $CURRENT_UTILITY_DIR
echo $IMAGE_TARGET_FOLDER
echo $NP_IMAGE_TARGET_FOLDER
echo $AP_IMAGE_TARGET_FOLDER

GDBSCPTFILE="$GNU_SCRIPT/rtl_gdb_flash_write.txt"

#===============================================================================
#switch between normal and mp
if [ "$BUILD_TYPE" != "MFG" ]; then
	sed -i "s#/image_mp/#/image/#g"  $GDBSCPTFILE
	sed -i "s#amebagreen2_app_mp.bin#amebagreen2_app.bin#g"  $GDBSCPTFILE
else
	sed -i "s#/image/#/image_mp/#g"  $GDBSCPTFILE
	sed -i "s#amebagreen2_app.bin#amebagreen2_app_mp.bin#g"  $GDBSCPTFILE
fi

#===============================================================================
#set AP image2 code size
FLASHFILENAME="$IMAGE_TARGET_FOLDER/$GREEN2_APP.bin";
if [ -f $FLASHFILENAME ]; then
	FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME);
else
	FLASH_FILE_SIZE=0;
fi
sed -i "s#^set \$FlashFileSize =.*#set \$FlashFileSize =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE

#set boot code size
FLASHFILENAME="$IMAGE_TARGET_FOLDER/amebagreen2_boot.bin";
if [ -f $FLASHFILENAME ]; then
	FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME);
else
	FLASH_FILE_SIZE=0;
fi
sed -i "s#^set \$XIPBootSize =.*#set \$XIPBootSize =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE

#set AP GCM tag bin
FLASHFILENAME="$AP_IMAGE_TARGET_FOLDER/ap_image2_all_en_tag.bin";
if [ -f $FLASHFILENAME ]; then
	FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME);
else
	FLASH_FILE_SIZE=0;
fi
sed -i "s#^set \$AP_GCM_Tag_Size =.*#set \$AP_GCM_Tag_Size =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE

#set NP GCM tag bin
FLASHFILENAME="$AP_IMAGE_TARGET_FOLDER/np_image2_all_en_tag.bin";
if [ -f $FLASHFILENAME ]; then
	FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME);
else
	FLASH_FILE_SIZE=0;
fi
sed -i "s#^set \$NP_GCM_Tag_Size =.*#set \$NP_GCM_Tag_Size =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE

