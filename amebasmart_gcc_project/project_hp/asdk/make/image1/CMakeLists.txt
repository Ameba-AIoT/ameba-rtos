ameba_add_image(target_loader_${PROJECT_NAME})
# variable c_CURRENT_IMAGE value is target_loader_${PROJECT_NAME}

###############################  ADD COMPONENT  ###################################

ameba_add_subdirectory(${c_CMPT_BOOTLOADER_DIR})
ameba_add_subdirectory(${c_CMPT_CRASHDUMP_DIR})
ameba_add_subdirectory(${c_CMPT_SOC_DIR}/lib/bootloader lib_bootloader)
ameba_add_subdirectory(${c_CMPT_CHIPINFO_DIR})

###############################  GENERATE IMAGE  ###################################

#---------------- PRE LINK -------------------#
add_custom_command(
    TARGET ${c_CURRENT_IMAGE} PRE_LINK
    COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${c_SDK_LD_DIR}/ameba_img1_s.ld -o ${CMAKE_CURRENT_BINARY_DIR}/make/rlx8721d.ld
    COMMAND ${CMAKE_COMMAND} -E cat ${c_LINK_ROM_SYMBOL_S} >> ${CMAKE_CURRENT_BINARY_DIR}/make/rlx8721d.ld
    COMMENT "build bootloader linkerscript rlx8721d.ld"
)

#----------------- LINKING -------------------#
target_link_libraries(${c_CURRENT_IMAGE}
    PRIVATE
    -T${CMAKE_CURRENT_BINARY_DIR}/make/rlx8721d.ld
    ${c_MCU_PROJ_CONFIG}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map

    -Wl,--whole-archive
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_libraries>
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_whole_archive_libs>
    -Wl,--no-whole-archive

    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_no_whole_archive_libs>

    ${c_LINK_ROM_LIB} #WARNING: keep this at last
)

#--------------- POST BUILD ------------------#

add_custom_command(TARGET ${c_CURRENT_IMAGE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${c_SDK_IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${c_SDK_IMAGE_TARGET_FOLDER}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy text.map ${c_SDK_IMAGE_TARGET_FOLDER}/text_loader.map
    COMMAND ${CMAKE_COMMAND} -E copy ${c_CURRENT_IMAGE}.axf ${c_SDK_IMAGE_TARGET_FOLDER}/target_loader.axf
    COMMAND ${CMAKE_NM} ${c_SDK_IMAGE_TARGET_FOLDER}/target_loader.axf | sort > ${c_SDK_IMAGE_TARGET_FOLDER}/target_loader.map
    COMMAND ${CMAKE_OBJDUMP} -d ${c_SDK_IMAGE_TARGET_FOLDER}/target_loader.axf > ${c_SDK_IMAGE_TARGET_FOLDER}/target_loader.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_FOLDER}/target_loader.axf ${c_SDK_IMAGE_TARGET_FOLDER}/target_pure_loader.axf
    COMMAND ${CMAKE_STRIP} ${c_SDK_IMAGE_TARGET_FOLDER}/target_pure_loader.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km4_boot_all.bin"

    COMMAND ${CMAKE_COMMAND}
    -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
    -Dc_SDK_IMAGE_TARGET_FOLDER=${c_SDK_IMAGE_TARGET_FOLDER} -Dc_BASEDIR=${c_BASEDIR} -Dc_MCU_PROJECT_DIR=${c_MCU_PROJECT_DIR}
    -Dc_CMAKE_FILES_DIR=${c_CMAKE_FILES_DIR} -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}
    -DBUILD_TYPE=${BUILD_TYPE}
    -DCONFIG_MP_SHRINK=${CONFIG_MP_SHRINK}
    -P ${c_MCU_SDK_DIR}/loader_postbuild.cmake
)


#------------- EXTRA PROCESS ------------ -----#

set(clean_files
    ${c_SDK_IMAGE_TARGET_FOLDER}
    ${FINAL_IMAGE_DIR}/km4_boot_all.bin
)

set(link_deps
    ${c_SDK_LD_DIR}/ameba_img1_s.ld
    ${c_LINK_ROM_SYMBOL_S}
    ${c_LINK_ROM_LIB}
)

set_target_properties(${c_CURRENT_IMAGE} PROPERTIES
    #WARNING: values for these parameter MUST be placed inside a pair of quotes
    ADDITIONAL_CLEAN_FILES
        "${clean_files}"
    LINK_DEPENDS
        "${link_deps}"
)

################################  CUSTOM TARGET  ###################################

add_custom_target(
    boot
    COMMENT "build bootloader only"
    DEPENDS ${c_CURRENT_IMAGE}
)
