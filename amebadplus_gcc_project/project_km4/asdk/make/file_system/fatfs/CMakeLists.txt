set(DIR ${BASEDIR}/component/file_system/fatfs)

set(
	CSRC
    ${DIR}/disk_if/src/usbdisk.c
	${DIR}/${FATFS_VER}/src/ff.c
	${DIR}/${FATFS_VER}/src/diskio.c
	${DIR}/${FATFS_VER}/src/ffsystem.c
	${DIR}/${FATFS_VER}/src/ffunicode.c 
)

ameba_internal_library(fatfs)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
