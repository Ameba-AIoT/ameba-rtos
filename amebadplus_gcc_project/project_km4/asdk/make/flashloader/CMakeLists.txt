set(DIR ${FLASHDLDIR})

set(
    CSRC
    ${DIR}/rtl_flash_download.c
    ${DIR}/rtl_flash_download_entry.c
)

ameba_executable(target_flashloader EXCLUDE_FROM_ALL)

add_custom_target(
    flashloader
    COMMENT "build flashloader only"
    DEPENDS target_flashloader
)

target_sources(
    target_flashloader
    PRIVATE
    ${CSRC}
)

target_link_libraries(
    target_flashloader
    PRIVATE ameba_interface_${PROJECT_NAME} ameba_wrap_interface_${PROJECT_NAME}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map
    -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    ${LINK_ROM_LIB}
    ${LINK_LOADER_LIB}
)

add_custom_command(
  TARGET target_flashloader PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c -DGENERATE_FLOADER_LD ${LDDIR}/ameba_img1_all.ld -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMAND ${CMAKE_COMMAND} -E cat ${LINK_ROM_SYMBOL_S} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMENT "build flashloader linkerscript rlx8721d.ld"
)

set(
    LINK_SCRIPTS
    ${LDDIR}/ameba_img1_all.ld
    ${PROJECTDIR}/../amebaDplus_layout.ld
)

set_target_properties(target_flashloader PROPERTIES LINK_DEPENDS "${LINK_SCRIPTS}")

add_custom_command(TARGET target_flashloader POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${IMAGE_TARGET_FOLDER}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy text.map ${IMAGE_TARGET_FOLDER}/text_loader.map
    COMMAND ${CMAKE_COMMAND} -E copy target_flashloader.axf ${IMAGE_TARGET_FOLDER}/target_loader.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_loader.axf | sort > ${IMAGE_TARGET_FOLDER}/target_loader.map
    COMMAND ${CMAKE_OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_loader.axf > ${IMAGE_TARGET_FOLDER}/target_loader.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_loader.axf ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf
    COMMAND ${CMAKE_STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      flash_loader_ram_1.bin"

    COMMAND ${CMAKE_COMMAND}
    -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DBASEDIR=${BASEDIR}
    -DCMAKE_FILES_DIR=${CMAKE_FILES_DIR} -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}
    -DBUILD_TYPE=${BUILD_TYPE}
    -DCONFIG_MP_SHRINK=${CONFIG_MP_SHRINK}
    -DIMGTOOL_LOADER=y
    -P ${CMAKE_CURRENT_SOURCE_DIR}/../loader_postbuild.cmake

    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_loader.axf 	${GNU_SCRIPT}/acut_loader/target_FPGA.axf
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/ram_1.bin           ${GNU_SCRIPT}/acut_loader/flash_loader_ram_1.bin
)
