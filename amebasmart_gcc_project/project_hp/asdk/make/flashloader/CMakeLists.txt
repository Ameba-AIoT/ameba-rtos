ameba_set_if(d_VERBOSE_MAKEFILE CMAKE_VERBOSE_MAKEFILE ON p_ELSE OFF)

add_executable(target_flashloader EXCLUDE_FROM_ALL)
add_custom_target(
    flashloader
    COMMENT "build flashloader only"
    DEPENDS target_flashloader
)

ameba_info("target_flashloader: d_LIST_${d_MCU_PROJECT_NAME_UPPER}_LOADER_LIBRARIES: ${d_LIST_${d_MCU_PROJECT_NAME_UPPER}_LOADER_LIBRARIES}")


target_link_libraries(
    target_flashloader
    PRIVATE
    -Wl,--gc-sections
    -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    ${d_LIST_${d_MCU_PROJECT_NAME_UPPER}_LOADER_LIBRARIES}
    d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES
    d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
    src_${d_MCU_PROJECT_NAME}_flashloader
    ${d_LINK_ROM_LIB}
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map
)

foreach(dep IN LISTS d_LIST_${d_MCU_PROJECT_NAME_UPPER}_LOADER_DEPS)
    ameba_target_depend(target_flashloader ${dep})
endforeach()


add_custom_command(
    TARGET target_flashloader PRE_LINK
    COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${LDDIR}/ameba_img1_s.ld -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld -I ${c_MENUCONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E cat ${d_LINK_ROM_SYMBOL_S} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    COMMENT "build flashloader linkerscript rlx8721d.ld"
)

set_target_properties(target_flashloader PROPERTIES LINK_DEPENDS ${LDDIR}/ameba_img1_s.ld)


add_custom_command(TARGET target_flashloader POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${IMAGE_TARGET_FOLDER}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy text.map ${IMAGE_TARGET_FOLDER}/text_loader.map
    COMMAND ${CMAKE_COMMAND} -E copy target_flashloader.axf ${IMAGE_TARGET_FOLDER}/target_loader.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_loader.axf | sort > ${IMAGE_TARGET_FOLDER}/target_loader.map
    COMMAND ${CMAKE_OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_loader.axf > ${IMAGE_TARGET_FOLDER}/target_loader.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_loader.axf ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf
    COMMAND ${CMAKE_STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km4_boot_all.bin"

    COMMAND ${CMAKE_COMMAND}
    -Dd_MCU_PROJECT_NAME_UPPER=${d_MCU_PROJECT_NAME_UPPER}
    -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
    -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DBASEDIR=${BASEDIR} -DPROJECTDIR=${PROJECTDIR}
    -DCMAKE_FILES_DIR=${CMAKE_FILES_DIR} -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}
    -DBUILD_TYPE=${BUILD_TYPE}
    -DCONFIG_MP_SHRINK=${CONFIG_MP_SHRINK}
    -P ${d_MCU_SDK_DIR}/loader_postbuild.cmake
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_loader.axf   ${d_SDK_GNU_UTILITY_DIR}/gnu_script/acut_loader/target_FPGA.axf
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/ram_1.bin           ${d_SDK_GNU_UTILITY_DIR}/gnu_script/acut_loader/flash_loader_ram_1.bin
)

set_property(TARGET target_flashloader
    PROPERTY ADDITIONAL_CLEAN_FILES
    ${IMAGE_TARGET_FOLDER}
    ${FINAL_IMAGE_DIR}/km4_boot_all.bin
)
