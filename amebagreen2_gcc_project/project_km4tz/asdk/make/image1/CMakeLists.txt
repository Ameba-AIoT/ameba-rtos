ameba_add_image(target_loader p_IMAGE_ALL ${c_MCU_PROJECT_NAME}_boot_all.bin)
# Use variable c_CURRENT_IMAGE to represent full image target name

###############################  ADD COMPONENT  ###################################

add_compile_definitions(
    CONFIG_NON_OS
)

ameba_add_subdirectory(${c_CMPT_BOOTLOADER_DIR})
ameba_add_subdirectory(${c_CMPT_CHIPINFO_DIR})
ameba_add_subdirectory(${c_CMPT_FWLIB_DIR})
ameba_add_subdirectory(${c_CMPT_LZMA_DIR})
ameba_add_subdirectory(${c_CMPT_USRCFG_DIR} usrcfg)
ameba_add_subdirectory(${c_CMPT_SOC_DIR}/lib/bootloader lib_bootloader)

if ((NOT CONFIG_WHC_INTF_IPC) AND CONFIG_WHC_DEV)
    ameba_add_subdirectory(${c_COMPONENT_DIR}/soc/common/flash_wrapper)
endif()

if(CONFIG_WHC_INTF_USB)
    target_sources(${c_CURRENT_IMAGE} PRIVATE
        ${c_CMPT_FWLIB_DIR}/ram_common/ameba_usb.c

        ${c_CMPT_USB_DIR}/common/usb_os.c
        ${c_CMPT_USB_DIR}/common/usb_hal.c
        ${c_CMPT_USB_DIR}/common/usb_phy.c
    )
    target_include_directories(${c_CURRENT_IMAGE} PRIVATE
	${c_CMPT_USB_DIR}/common
    )
endif()

###############################  GENERATE IMAGE  ###################################

#---------------- PRE LINK -------------------#
add_custom_command(
    TARGET ${c_CURRENT_IMAGE} PRE_LINK
    COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${c_IMG1_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld -I ${c_MENUCONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E cat ${c_LINK_ROM_SYMBOL_S} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    COMMENT "build bootloader linkerscript rlx8721d.ld"
)

if ((NOT CONFIG_WHC_INTF_IPC) AND CONFIG_WHC_DEV)
    target_sources(${c_CURRENT_IMAGE} PRIVATE ${c_SDK_IMAGE_TARGET_DIR}/embedded_bin.o)

    if(TARGET target_pg_loader_${c_MCU_PROJECT_NAME})
        set(DEP_pg_loader target_pg_loader_${c_MCU_PROJECT_NAME})
    else()
        message(FATAL_ERROR "target_pg_loader_${c_MCU_PROJECT_NAME} not found, please check your project configuration")
    endif()

    add_dependencies(${c_CURRENT_IMAGE} ${DEP_pg_loader})
endif()

#----------------- LINKING -------------------#
target_link_libraries(${c_CURRENT_IMAGE}
    PRIVATE
    -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
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
ameba_get_image_target_name(gdb_floader floader_target)
if(TARGET ${floader_target}_postbuild)
    set(DEP_GDB_FLOADER ${floader_target}_postbuild)
endif()

add_custom_command(OUTPUT ${c_SDK_IMAGE_TARGET_DIR}/fullmac_ram_1_prepend.bin
    COMMAND ${CMAKE_COMMAND} -E make_directory ${c_SDK_IMAGE_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${c_SDK_IMAGE_TARGET_DIR}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy text.map ${c_SDK_IMAGE_TARGET_DIR}/text_loader.map
    COMMAND ${CMAKE_COMMAND} -E copy ${c_CURRENT_IMAGE}.axf ${c_SDK_IMAGE_TARGET_DIR}/target_loader.axf
    COMMAND ${CMAKE_NM} ${c_SDK_IMAGE_TARGET_DIR}/target_loader.axf | sort > ${c_SDK_IMAGE_TARGET_DIR}/target_loader.map
    COMMAND ${CMAKE_OBJDUMP} -d ${c_SDK_IMAGE_TARGET_DIR}/target_loader.axf > ${c_SDK_IMAGE_TARGET_DIR}/target_loader.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/target_loader.axf ${c_SDK_IMAGE_TARGET_DIR}/target_pure_loader.axf
    COMMAND ${CMAKE_STRIP} ${c_SDK_IMAGE_TARGET_DIR}/target_pure_loader.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      ${c_MCU_PROJECT_NAME}_boot_all.bin"

    COMMAND ${CMAKE_COMMAND}
        # common variables
        -Dc_BASEDIR=${c_BASEDIR}  # dir of sdk's root
        -Dc_CMAKE_FILES_DIR=${c_CMAKE_FILES_DIR} # dir of cmake scripts
        -Dc_SOC_PROJECT_DIR=${c_SOC_PROJECT_DIR} # dir of soc project
        -Dc_MCU_PROJECT_DIR=${c_MCU_PROJECT_DIR} # dir of current mcu project
        -Dc_MCU_PROJECT_NAME=${c_MCU_PROJECT_NAME} # name of current mcu project
        -Dc_MCU_KCONFIG_FILE=${c_MCU_KCONFIG_FILE} # file of current project's kconfig file
        -Dc_SDK_IMAGE_TARGET_DIR=${c_SDK_IMAGE_TARGET_DIR} # dir of image output
        -Dc_BOOT_BINARY_NAME=${boot_name} # name of boot image

        # user's variables
        -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
        -DBUILD_TYPE=${BUILD_TYPE}
        -DANALYZE_MP_IMG=${ANALYZE_MP_IMG}
        -DDAILY_BUILD=${DAILY_BUILD}
        -DEXTERN_DIR=${EXTERN_DIR}

        # cmake toolchain variables
        -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
        -DCMAKE_SIZE=${CMAKE_SIZE}
        -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}

        -P ${CMAKE_CURRENT_SOURCE_DIR}/${c_POST_BUILD_SCRIPT}
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/fullmac_ram_1_prepend.bin
    DEPENDS ${c_CURRENT_IMAGE} ${DEP_GDB_FLOADER}
)

add_custom_target(
    ${c_CURRENT_IMAGE}_postbuild
    ALL
    DEPENDS ${c_SDK_IMAGE_TARGET_DIR}/fullmac_ram_1_prepend.bin
)

################################  EXTRA PROCESS  ###################################

set(clean_files
    ${c_SDK_IMAGE_TARGET_DIR}
    ${FINAL_IMAGE_DIR}/${boot_name}
)

set(link_deps
    ${c_LAYOUT_LD}
    ${c_IMG1_LD}
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
    DEPENDS ${c_CURRENT_IMAGE}_postbuild
)
