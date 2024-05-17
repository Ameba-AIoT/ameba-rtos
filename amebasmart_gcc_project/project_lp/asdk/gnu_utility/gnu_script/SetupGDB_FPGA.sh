#!/bin/sh

#===============================================================================
CURRENT_UTILITY_DIR=$(pwd)
GNU_SCRIPT=$1
IMAGE_TARGET_FOLDER=$CURRENT_UTILITY_DIR/image
echo "..."
echo $CURRENT_UTILITY_DIR
echo $IMAGE_TARGET_FOLDER
GDBSCPTFILE="$GNU_SCRIPT/rtl_gdb_flash_write.txt" 

#===============================================================================
#set image2 code size
if [ -f $IMAGE_TARGET_FOLDER/km0_image2_all.bin ]; then 
	FLASHFILENAME="$IMAGE_TARGET_FOLDER/km0_image2_all.bin";
	FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME); 
else 
	FLASH_FILE_SIZE=0; 
fi

sed -i "s#^set \$FlashFileSize =.*#set \$FlashFileSize =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE

#set boot code size
FLASHFILENAME="$IMAGE_TARGET_FOLDER/km0_boot_all.bin" 
FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME)
sed -i "s#^set \$XIPBootSize =.*#set \$XIPBootSize =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE
