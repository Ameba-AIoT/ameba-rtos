set(DIR ${BASEDIR}/component/file_system/fatfs)

target_sources(
    target_img2_${PROJECT_NAME}
    PRIVATE 
    ${DIR}/disk_if/src/usbdisk.c
	${DIR}/r0.14b/src/ff.c
	${DIR}/r0.14b/src/diskio.c
	${DIR}/r0.14b/src/ffsystem.c
	${DIR}/r0.14b/src/ffunicode.c 
)