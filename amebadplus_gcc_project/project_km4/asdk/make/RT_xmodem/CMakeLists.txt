set(XM_DIR ${BASEDIR}/component/soc/common/flash_download/rt_xmodem)
set(USBD_DIR ${BASEDIR}/component/soc/common/flash_download/usb_download)
set(USB_COM_DIR ${BASEDIR}/component/usb/common)
set(RAM_COM_DIR ${TARGETDIR}/fwlib/ram_common)
set(RAM_HP_DIR ${TARGETDIR}/fwlib/ram_km4)
set(HAL_DIR ${TARGETDIR}/hal)
set(HAL_SRC_DIR ${TARGETDIR}/hal/src)
set(TAR_DIR ${BASEDIR}/component/soc/common/imgtool_floader/src)
set(SWLIB_DIR ${TARGETDIR}/swlib)

set(
    MODULE_IFLAGS
    ${TARGETDIR}/app/xmodem
    ${TARGETDIR}/fwlib/include
    ${BASEDIR}/component/mbed/hal
    ${HAL_DIR}/include
    ${HAL_DIR}/src
    ${SWLIB_DIR}
    ${XM_DIR}
    ${USB_COM_DIR}
    ${USBD_DIR}
    ${TAR_DIR}/../inc
)

set(
    MODULE_C_DEFINES
    CONFIG_USB_FS
    CONFIG_USB_DOWNLOAD_EN
    CONFIG_USB_DOWNLOAD_IND_EN
    CONFIG_FLOADER_USBD_EN
    CONFIG_USB_NONOS
)

set(
    CSRC
    ${XM_DIR}/floader_fw_update.c
	${XM_DIR}/floader_xmodem.c
	${SWLIB_DIR}/log.c
	${RAM_COM_DIR}/ameba_reset.c
	${RAM_COM_DIR}/ameba_tim.c
	${TAR_DIR}/ameba_floader_rom_parse.c
	${TAR_DIR}/floader_nor_ftl.c
	${RAM_HP_DIR}/ameba_usb.c
	${HAL_SRC_DIR}/pinmap_common.c
	${HAL_SRC_DIR}/pinmap.c
	${HAL_SRC_DIR}/gpio_api.c
	${HAL_SRC_DIR}/pwmout_api.c
	${USB_COM_DIR}/usb_os.c
	${USB_COM_DIR}/usb_hal.c
	${USBD_DIR}/usbd_cdc.c
	${USBD_DIR}/usbd_core.c
	${USBD_DIR}/usbd_dbg.c
	${USBD_DIR}/usbd_hal.c
	${USBD_DIR}/usbd_intf.c
	${USBD_DIR}/usbd_pcd.c
)

ameba_executable(target_imgtool_flashloader EXCLUDE_FROM_ALL)

add_custom_target(
    imgtool_flashloader
    COMMENT "build imgtool_flashloader only"
    DEPENDS target_imgtool_flashloader
)

target_sources(
    target_imgtool_flashloader
    PRIVATE
    ${CSRC}
)

target_include_directories(
    target_imgtool_flashloader
    PRIVATE
    ${MODULE_IFLAGS}
)

target_compile_definitions(
    target_imgtool_flashloader
    PRIVATE
    ${MODULE_C_DEFINES}
)

target_link_libraries(
    target_imgtool_flashloader
    PRIVATE ameba_interface_${PROJECT_NAME} ameba_wrap_interface_${PROJECT_NAME}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map
    -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    ${LINK_ROM_LIB}
    ${LINK_LOADER_LIB}
)

add_custom_command(
  TARGET target_imgtool_flashloader PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c -DGENERATE_FLOADER_LD ${LDDIR}/ameba_img1_all.ld -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMAND ${CMAKE_COMMAND} -E cat ${LINK_ROM_SYMBOL_S} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMENT "build imgtool_flashloader linkerscript rlx8721d.ld"
)

set(
    LINK_SCRIPTS
    ${LDDIR}/ameba_img1_all.ld
    ${PROJECTDIR}/../amebaDplus_layout.ld
)

set_target_properties(target_imgtool_flashloader PROPERTIES LINK_DEPENDS "${LINK_SCRIPTS}")

add_custom_command(TARGET target_imgtool_flashloader POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${IMAGE_TARGET_FOLDER}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy text.map ${IMAGE_TARGET_FOLDER}/text_loader.map
    COMMAND ${CMAKE_COMMAND} -E copy target_imgtool_flashloader.axf ${IMAGE_TARGET_FOLDER}/target_loader.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_loader.axf | sort > ${IMAGE_TARGET_FOLDER}/target_loader.map
    COMMAND ${CMAKE_OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_loader.axf > ${IMAGE_TARGET_FOLDER}/target_loader.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_loader.axf ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf
    COMMAND ${CMAKE_STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      imgtool_flashloader.bin"

    COMMAND ${CMAKE_COMMAND}
    -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DBASEDIR=${BASEDIR}
    -DCMAKE_FILES_DIR=${CMAKE_FILES_DIR} -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}
    -DBUILD_TYPE=${BUILD_TYPE}
    -DCONFIG_MP_SHRINK=${CONFIG_MP_SHRINK}
    -DIMGTOOL_LOADER=y
    -P ${CMAKE_CURRENT_SOURCE_DIR}/../loader_postbuild.cmake

    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_loader.axf 	        ${FLOADER_BIN_DIR}/target_FPGA.axf
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/ram_1_prepend.bin           ${FLOADER_BIN_DIR}/imgtool_flashloader_raw.bin
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/imgtool_flashloader.bin     ${FLOADER_BIN_DIR}/floader_amebadplus.bin
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/imgtool_flashloader.bin     ${FLOADER_BIN_DIR}/imgtool_flashloader.bin
    COMMAND ${CMAKE_COMMAND} -E remove -f ${IMAGE_TARGET_FOLDER}/imgtool_flashloader.bin
)
