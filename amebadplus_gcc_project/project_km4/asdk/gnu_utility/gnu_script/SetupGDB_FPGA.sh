#!/bin/sh

#===============================================================================
CURRENT_UTILITY_DIR=$(pwd)
GNU_SCRIPT=$1
IMAGE_TARGET_FOLDER=$CURRENT_UTILITY_DIR/image
KM0_IMAGE_TARGET_FOLDER=$CURRENT_UTILITY_DIR/../../project_km0/asdk/image/

echo "..."
echo $CURRENT_UTILITY_DIR
echo $IMAGE_TARGET_FOLDER
echo $KM0_IMAGE_TARGET_FOLDER

GDBSCPTFILE="$GNU_SCRIPT/rtl_gdb_flash_write.txt"

#===============================================================================
#set KM4 image2 code size
FLASHFILENAME="$IMAGE_TARGET_FOLDER/km0_km4_app.bin";
if [ -f $FLASHFILENAME ]; then
	FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME);
else
	FLASH_FILE_SIZE=0;
fi
sed -i "s#^set \$FlashFileSize =.*#set \$FlashFileSize =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE

#set boot code size
FLASHFILENAME="$IMAGE_TARGET_FOLDER/km4_boot_all.bin";
if [ -f $FLASHFILENAME ]; then
	FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME);
else
	FLASH_FILE_SIZE=0;
fi
sed -i "s#^set \$XIPBootSize =.*#set \$XIPBootSize =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE

#set KM4 image3 code size
FLASHFILENAME="$IMAGE_TARGET_FOLDER/km4_image3_all.bin";
if [ -f $FLASHFILENAME ]; then
	FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME);
else
	FLASH_FILE_SIZE=0;
fi
sed -i "s#^set \$Img3FileSize =.*#set \$Img3FileSize =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE

#===============================================================================

#set KM0 boot code size
FLASHFILENAME="$KM0_IMAGE_TARGET_FOLDER/km0_boot_all.bin";
if [ -f $FLASHFILENAME ]; then
	FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME);
else
	FLASH_FILE_SIZE=0;
fi
sed -i "s#^set \$KM0XIPBootSize =.*#set \$KM0XIPBootSize =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE
