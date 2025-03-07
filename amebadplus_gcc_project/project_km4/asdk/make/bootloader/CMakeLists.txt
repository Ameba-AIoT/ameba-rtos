set(DIR ${TARGETDIR}/bootloader)
set(CFGDIR ${TARGETDIR}/usrcfg)
set(DRVDIR ${TARGETDIR}/fwlib/ram_km4)
set(DRVCOM ${TARGETDIR}/fwlib/ram_common)
set(LOGDIR ${TARGETDIR}/swlib)
set(LZMADIR ${TARGETDIR}/../common/lzma)

ameba_executable(target_bootloader)

add_custom_target(
    boot
    COMMENT "build bootloader only"
    DEPENDS target_bootloader
)

if (CONFIG_MP_SHRINK)
    target_sources(target_bootloader PRIVATE ${DIR}/bootloader_km4_mp.c)
else()
    target_sources(target_bootloader PRIVATE ${DIR}/bootloader_km4.c)
endif()

target_sources(
    target_bootloader
    PRIVATE
    ${DIR}/boot_ota_km4.c
	${DIR}/boot_security_km4.c
	${CFGDIR}/ameba_boot_trustzonecfg.c
	${CFGDIR}/ameba_bootcfg.c
	${CFGDIR}/ameba_flashcfg.c
	${DRVDIR}/ameba_flashclk.c
	${DRVCOM}/ameba_psram.c
	${DRVCOM}/ameba_clk.c
	${DRVCOM}/ameba_ldo.c
	${DRVCOM}/ameba_reset.c
	${DRVCOM}/ameba_pll.c
	${DRVCOM}/ameba_fault_handle.c
    ${LOGDIR}/log.c
)

if(CONFIG_COMPRESS_OTA_IMG)
    target_sources(
        target_bootloader
        PRIVATE
        ${LZMADIR}/ameba_boot_lzma.c
        ${LZMADIR}/LzmaDec.c
    )
endif()

target_include_directories(
    target_bootloader
    PRIVATE
    ${LOGDIR}
    ${TARGETDIR}/lib/bootloader
    ${BASEDIR}/component/soc/common/crashdump/include
    ${LZMADIR}
)

target_link_libraries(
    target_bootloader
    PRIVATE ameba_interface_${PROJECT_NAME} ameba_wrap_interface_${PROJECT_NAME}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map
    -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    ${LINK_LOADER_LIB}
    ${LINK_ROM_LIB}
)

if(CONFIG_MP_SHRINK)
    set(IMG1_LD ${LDDIR}/ameba_img1_all_mp.ld)
else()
    set(IMG1_LD ${LDDIR}/ameba_img1_all.ld)
endif()

add_custom_command(
  TARGET target_bootloader PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${IMG1_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMAND ${CMAKE_COMMAND} -E cat ${LINK_ROM_SYMBOL_S} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMENT "build bootloader linkerscript rlx8721d.ld"
)

set(
    LINK_SCRIPTS
    ${IMG1_LD}
    ${PROJECTDIR}/../amebaDplus_layout.ld
)

set_target_properties(target_bootloader PROPERTIES LINK_DEPENDS "${LINK_SCRIPTS}")


add_custom_command(TARGET target_bootloader POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${IMAGE_TARGET_FOLDER}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy text.map ${IMAGE_TARGET_FOLDER}/text_loader.map
    COMMAND ${CMAKE_COMMAND} -E copy target_bootloader.axf ${IMAGE_TARGET_FOLDER}/target_loader.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_loader.axf | sort > ${IMAGE_TARGET_FOLDER}/target_loader.map
    COMMAND ${CMAKE_OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_loader.axf > ${IMAGE_TARGET_FOLDER}/target_loader.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_loader.axf ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf
    COMMAND ${CMAKE_STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km4_boot_all.bin"

    COMMAND ${CMAKE_COMMAND}
    -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DBASEDIR=${BASEDIR} -DPROJECTDIR=${PROJECTDIR}
    -DCMAKE_FILES_DIR=${CMAKE_FILES_DIR} -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR} -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}
    -DBUILD_TYPE=${BUILD_TYPE}
    -DCONFIG_MP_SHRINK=${CONFIG_MP_SHRINK}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/../loader_postbuild.cmake
)

set_property(TARGET target_bootloader
    PROPERTY ADDITIONAL_CLEAN_FILES
    ${IMAGE_TARGET_FOLDER}
    ${FINAL_IMAGE_DIR}/km4_boot_all.bin
)
