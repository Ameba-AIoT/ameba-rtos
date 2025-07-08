ameba_add_image(target_img2 p_IMAGE_ALL ${c_MCU_PROJECT_NAME}_image2_all.bin)
# Use variable c_CURRENT_IMAGE to represent full image target name

###############################  ADD COMPONENT  ###################################

ameba_add_subdirectory(${c_CMPT_AT_CMD_DIR})
ameba_add_subdirectory(${c_CMPT_AUDIO_DIR})
ameba_add_subdirectory(${c_CMPT_BLUETOOTH_DIR})
ameba_add_subdirectory(${c_CMPT_CHIPINFO_DIR})
ameba_add_subdirectory(${c_CMPT_EXAMPLE_DIR})
ameba_add_subdirectory(${c_CMPT_ETHERNET_DIR})
ameba_add_subdirectory(${c_CMPT_FILE_SYSTEM_DIR})
ameba_add_subdirectory(${c_CMPT_FWLIB_DIR})
ameba_add_subdirectory(${c_CMPT_HAL_DIR})
ameba_add_subdirectory(${c_CMPT_MISC_DIR})
ameba_add_subdirectory(${c_CMPT_LWIP_DIR})
ameba_add_subdirectory(${c_CMPT_MONITOR_DIR})
ameba_add_subdirectory(${c_CMPT_NETWORK_DIR})
ameba_add_subdirectory(${c_CMPT_OS_DIR})
ameba_add_subdirectory(${c_CMPT_PMC_DIR})
ameba_add_subdirectory(${c_CMPT_RTK_COEX_DIR})
ameba_add_subdirectory(${c_CMPT_SSL_DIR})
ameba_add_subdirectory(${c_CMPT_SWLIB_DIR})
ameba_add_subdirectory(${c_CMPT_UI_DIR})
ameba_add_subdirectory(${c_CMPT_UTILS_DIR})
ameba_add_subdirectory(${c_CMPT_USB_DIR})
ameba_add_subdirectory(${c_CMPT_USRCFG_DIR} usrcfg)
ameba_add_subdirectory(${c_CMPT_DIAGNOSE_DIR})
ameba_add_subdirectory_if_exist(${c_CMPT_VERIFICATION_DIR})
ameba_add_subdirectory_if_exist(${c_CMPT_APP_DIR}/benchmark)
ameba_add_subdirectory(${c_CMPT_WIFI_DIR}/wifi_fw)
ameba_add_subdirectory(${c_CMPT_WIFI_DIR})
ameba_add_subdirectory_if_exist(${c_CMPT_WPAN_DIR})
ameba_add_subdirectory(${c_CMPT_SOC_DIR})

ameba_add_subdirectory(${c_MCU_PROJECT_DIR}/src)

###############################  GENERATE IMAGE  ###################################
#---------------- PRE LINK -------------------#
add_custom_command(
  TARGET ${c_CURRENT_IMAGE} PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${c_IMG2_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld -I ${c_MENUCONFIG_DIR}
  COMMAND ${CMAKE_COMMAND} -E cat ${c_LINK_ROM_SYMBOL} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMAND ${CMAKE_COMMAND} -E cat ${c_LINK_ROM_SYMBOL_WIFI} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMAND ${CMAKE_COMMAND} -E cat ${c_LINK_ROM_SYMBOL_OS} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMENT "build img2 linkerscript rlx8721d.ld"
)

#----------------- LINKING -------------------#

ameba_set_if(CONFIG_TRUSTZONE c_CMSE_IMPLIB
    ${c_SDK_IMAGE_TARGET_DIR}/cmse_implib.lib
)

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
    ${c_LINK_ROM_LIB}
    ${c_CMSE_IMPLIB}

    ${SDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${ASDK_VER}/thumb/v8-m.main/crtbegin.o
    ${SDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${ASDK_VER}/thumb/v8-m.main/crtend.o
    -lm -lstdc++
)

#--------------- POST BUILD ------------------#
if(CONFIG_WIFI_HOST_CONTROL)
    ameba_layout_extract(FULLMAC_IMG2 ${c_LAYOUT_LD} FULLMAC_IMG2_ORIGIN FULLMAC_IMG2_END FULLMAC_IMG2_LENGTH)
endif()

add_custom_command(
    OUTPUT ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf ${KM4NS_PROJECT_DIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_COMMAND} -E make_directory ${c_SDK_IMAGE_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${c_SDK_IMAGE_TARGET_DIR}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${c_SDK_IMAGE_TARGET_DIR}/text_image2.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${c_CURRENT_IMAGE}.axf ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf
    COMMAND ${CMAKE_NM} ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf | sort > ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
    COMMAND ${CMAKE_OBJDUMP} -d ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf > ${c_SDK_IMAGE_TARGET_DIR}/target_img2.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/target_img2.asm ${KM4NS_PROJECT_DIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_STRIP} ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km4tz_image2_all.bin"

    COMMAND ${CMAKE_COMMAND}
        # common variables
        -Dc_BASEDIR=${c_BASEDIR}  # dir of sdk's root
        -Dc_CMAKE_FILES_DIR=${c_CMAKE_FILES_DIR} # dir of cmake scripts
        -Dc_SOC_PROJECT_DIR=${c_SOC_PROJECT_DIR} # dir of soc project
        -Dc_MCU_PROJECT_DIR=${c_MCU_PROJECT_DIR} # dir of current mcu project
        -Dc_MCU_PROJECT_NAME=${c_MCU_PROJECT_NAME} # name of current mcu project
        -Dc_MCU_KCONFIG_FILE=${c_MCU_KCONFIG_FILE} # file of current project's kconfig file
        -Dc_SDK_IMAGE_TARGET_DIR=${c_SDK_IMAGE_TARGET_DIR} # dir of image output

        # user's variables
        -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
        -DBUILD_TYPE=${BUILD_TYPE}
        -DANALYZE_MP_IMG=${ANALYZE_MP_IMG}
        -DDAILY_BUILD=${DAILY_BUILD}

        # special variables for current image
        -DKM4TZ_PROJECT_DIR=${KM4TZ_PROJECT_DIR}
        -DFULLMAC_IMG2_LENGTH=${FULLMAC_IMG2_LENGTH}

        # cmake toolchain variables
        -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
        -DCMAKE_SIZE=${CMAKE_SIZE}
        -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}

        -P ${CMAKE_CURRENT_SOURCE_DIR}/${c_POST_BUILD_SCRIPT}

    COMMENT "${c_CURRENT_IMAGE} post build"
    DEPENDS ${c_CURRENT_IMAGE} ${c_SDK_IMAGE_TARGET_DIR}/fullmac_ram_1_prepend.bin
)

add_custom_target(
    ${c_CURRENT_IMAGE}_postbuild
    ALL
    DEPENDS target_wifi_nouse
)
add_custom_target(
    target_wifi_nouse
    DEPENDS ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf ${KM4NS_PROJECT_DIR}/asdk/image/target_img2_otrcore.asm
)

#------------- EXTRA PROCESS -----------------#
set(clean_files
    ${c_SDK_IMAGE_TARGET_DIR}
    ${FINAL_IMAGE_DIR}/amebagreen2_app.bin
    ${FINAL_IMAGE_DIR}/amebagreen2_app_mp.bin
    ${FINAL_IMAGE_DIR}/ap_fullmac.bin
    ${FINAL_IMAGE_DIR}/ota_all.bin
)

set(link_deps
    ${c_LAYOUT_LD}
    ${c_IMG2_LD}
    ${c_LINK_ROM_SYMBOL}
    ${c_LINK_ROM_SYMBOL_WIFI}
    ${c_LINK_ROM_SYMBOL_OS}
)

set_target_properties(${c_CURRENT_IMAGE} PROPERTIES
    #WARNING: values for these parameter MUST be placed inside a pair of quotes
    ADDITIONAL_CLEAN_FILES
        "${clean_files}"
    LINK_DEPENDS
        "${link_deps}"
)
