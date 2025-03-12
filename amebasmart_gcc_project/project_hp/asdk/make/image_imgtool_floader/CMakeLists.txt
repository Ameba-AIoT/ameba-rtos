ameba_add_image(target_imgtool_floader p_EXCLUDE_FROM_ALL)
# Use variable c_CURRENT_IMAGE to represent full image target name

###############################  ADD COMPONENT  ###################################
add_compile_definitions(
    CONFIG_USB_FS
    CONFIG_FLOADER_USBD_EN
)

ameba_add_subdirectory(${c_COMPONENT_DIR}/soc/common/flash_download/imgtool_floader)
ameba_add_subdirectory(${c_COMPONENT_DIR}/soc/common/flash_wrapper)
ameba_add_subdirectory(${c_CMPT_CRASHDUMP_DIR})
ameba_add_subdirectory(${c_CMPT_CHIPINFO_DIR})
ameba_add_subdirectory(${c_CMPT_SOC_DIR}/lib/bootloader lib_bootloader)

target_sources(${c_CURRENT_IMAGE} PRIVATE
    ${c_CMPT_FWLIB_DIR}/ram_common/ameba_reset.c
    ${c_CMPT_FWLIB_DIR}/ram_common/ameba_otp.c
    ${c_CMPT_FWLIB_DIR}/ram_common/ameba_ipc_api.c
    ${c_CMPT_FWLIB_DIR}/ram_common/ameba_tim.c
    ${c_CMPT_FWLIB_DIR}/ram_common/ameba_usb.c
    ${c_CMPT_FWLIB_DIR}/ram_hp/ameba_codec.c

    ${c_CMPT_SWLIB_DIR}/log.c
    ${c_CMPT_USB_DIR}/common/usb_hal.c
    ${c_CMPT_USB_DIR}/common/usb_os.c
)

###############################  GENERATE IMAGE  ###################################

#---------------- PRE LINK -------------------#
add_custom_command(
    TARGET ${c_CURRENT_IMAGE} PRE_LINK
    COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${c_SDK_LD_DIR}/ameba_floader_s.ld -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld -I ${c_MENUCONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E cat ${c_LINK_ROM_SYMBOL_S} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    COMMENT "build imgtool_flashloader linkerscript rlx8721d.ld"
)

#----------------- LINKING -------------------#
target_link_libraries(${c_CURRENT_IMAGE}
    PRIVATE
    -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    ${c_MCU_PROJ_CONFIG}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map

    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_libraries>
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_whole_archive_libs>
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_no_whole_archive_libs>

    ${c_LINK_ROM_LIB} #WARNING: keep this at last
)

#--------------- POST BUILD ------------------#
add_custom_command(TARGET ${c_CURRENT_IMAGE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${c_SDK_IMAGE_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${c_SDK_IMAGE_TARGET_DIR}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy text.map ${c_SDK_IMAGE_TARGET_DIR}/text_loader.map
    COMMAND ${CMAKE_COMMAND} -E copy ${c_CURRENT_IMAGE}.axf ${c_SDK_IMAGE_TARGET_DIR}/target_loader.axf
    COMMAND ${CMAKE_NM} ${c_SDK_IMAGE_TARGET_DIR}/target_loader.axf | sort > ${c_SDK_IMAGE_TARGET_DIR}/target_loader.map
    COMMAND ${CMAKE_OBJDUMP} -d ${c_SDK_IMAGE_TARGET_DIR}/target_loader.axf > ${c_SDK_IMAGE_TARGET_DIR}/target_loader.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/target_loader.axf ${c_SDK_IMAGE_TARGET_DIR}/target_pure_loader.axf
    COMMAND ${CMAKE_STRIP} ${c_SDK_IMAGE_TARGET_DIR}/target_pure_loader.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km4_boot_all.bin"

    COMMAND ${CMAKE_COMMAND}
        # common variables
        -Dc_BASEDIR=${c_BASEDIR}  # dir of sdk's root
        -Dc_CMAKE_FILES_DIR=${c_CMAKE_FILES_DIR} # dir of cmake scripts
        -Dc_SOC_PROJECT_DIR=${c_SOC_PROJECT_DIR} # dir of soc project
        -Dc_MCU_PROJECT_DIR=${c_MCU_PROJECT_DIR} # dir of current mcu project
        -Dc_MCU_KCONFIG_FILE=${c_MCU_KCONFIG_FILE} # file of current project's kconfig file
        -Dc_SDK_IMAGE_TARGET_DIR=${c_SDK_IMAGE_TARGET_DIR} # dir of image output

        # user's variables
        -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
        -DBUILD_TYPE=${BUILD_TYPE}
        -DANALYZE_MP_IMG=${ANALYZE_MP_IMG}
        -DDAILY_BUILD=${DAILY_BUILD}

        # special variables for current image
        -DIMGTOOL_LOADER=y

        # cmake toolchain variables
        -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
        -DCMAKE_SIZE=${CMAKE_SIZE}
        -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}

        -P ${CMAKE_CURRENT_SOURCE_DIR}/../image1/postbuild.cmake

    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin ${c_SDK_GNU_UTILITY_DIR}/image_tool_flashloader/amebasmart_acut/imgtool_flashloader_raw.bin
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/imgtool_flashloader.bin ${c_SDK_GNU_UTILITY_DIR}/image_tool_flashloader/amebasmart_acut/floader_amebasmart.bin
)
################################  EXTRA PROCESS  ###################################

set(clean_files
    ${c_SDK_IMAGE_TARGET_DIR}
    ${FINAL_IMAGE_DIR}/km4_boot_all.bin
)

set(link_deps
    ${c_SDK_LD_DIR}/ameba_floader_s.ld
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
    imgtool_flashloader
    COMMENT "build imgtool_flashloader only"
    DEPENDS ${c_CURRENT_IMAGE}
)
