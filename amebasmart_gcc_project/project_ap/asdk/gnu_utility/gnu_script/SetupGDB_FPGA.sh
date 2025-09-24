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
#set ap image code size
if [ -f $IMAGE_TARGET_FOLDER/ap_image_all.bin ]; then 
	FLASHFILENAME="$IMAGE_TARGET_FOLDER/ap_image_all.bin";
	FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME); 
else 
	FLASH_FILE_SIZE=0; 
fi

sed -i "s#^set \$APIMGFileSize =.*#set \$APIMGFileSize =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE

FLASHFILENAME="$IMAGE_TARGET_FOLDER/cortex-a7.dtb";
if [ -f $FLASHFILENAME ]; then
	FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME);
else
	FLASH_FILE_SIZE=0;
fi
sed -i "s#^set \$DTBFileSize =.*#set \$DTBFileSize =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE

FLASHFILENAME="$IMAGE_TARGET_FOLDER/uImage";
if [ -f $FLASHFILENAME ]; then
	FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME);
else
	FLASH_FILE_SIZE=0;
fi
sed -i "s#^set \$UIMAGEFileSize =.*#set \$UIMAGEFileSize =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE

FLASHFILENAME="$IMAGE_TARGET_FOLDER/initsquashfs.bin";
if [ -f $FLASHFILENAME ]; then
	FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME);
else
	FLASH_FILE_SIZE=0;
fi
sed -i "s#^set \$CA32_FILE_SQ_SIZE =.*#set \$CA32_FILE_SQ_SIZE =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE

FLASHFILENAME="$IMAGE_TARGET_FOLDER/jffs2.img";
if [ -f $FLASHFILENAME ]; then
	FLASH_FILE_SIZE=$(stat -c %s $FLASHFILENAME);
else
	FLASH_FILE_SIZE=0;
fi
sed -i "s#^set \$CA32_FILE_JF_SIZE =.*#set \$CA32_FILE_JF_SIZE =$FLASH_FILE_SIZE#g"  $GDBSCPTFILE
