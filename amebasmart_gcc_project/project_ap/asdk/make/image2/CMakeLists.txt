ameba_add_image(target_img2 p_IMAGE_ALL ap_image_all.bin)
# Use variable c_CURRENT_IMAGE to represent full image target name
# set(c_CURRENT_IMAGE_TYPE "image2") #TODO: remove this line if ameba_add_image() above is uncommented

###############################  ADD COMPONENT  ###################################

ameba_add_subdirectory_if_exist(${c_CMPT_AIVOICE_DIR})
ameba_add_subdirectory(${c_CMPT_AT_CMD_DIR})
ameba_add_subdirectory(${c_CMPT_MONITOR_DIR})
ameba_add_subdirectory(${c_CMPT_SOC_DIR})
ameba_add_subdirectory(${c_CMPT_FWLIB_DIR})
ameba_add_subdirectory(${c_CMPT_USRCFG_DIR} usrcfg)
ameba_add_subdirectory(${c_CMPT_HAL_DIR})
ameba_add_subdirectory(${c_CMPT_SWLIB_DIR})
ameba_add_subdirectory(${c_CMPT_FILE_SYSTEM_DIR})
ameba_add_subdirectory(${c_CMPT_EXAMPLE_DIR})
ameba_add_subdirectory_if_exist(${c_CMPT_AUDIO_DIR})
ameba_add_subdirectory(${c_CMPT_UTILS_DIR})
ameba_add_subdirectory(${c_CMPT_USB_DIR})
ameba_add_subdirectory(${c_COMPONENT_DIR}/soc/common/cmsis-dsp)
ameba_add_subdirectory_if_exist(${c_CMPT_WPAN_DIR})
ameba_add_subdirectory(${c_CMPT_BLUETOOTH_DIR})
ameba_add_subdirectory(${c_CMPT_NETWORK_DIR})
ameba_add_subdirectory(${c_CMPT_LWIP_DIR})
ameba_add_subdirectory(${c_CMPT_SSL_DIR})
ameba_add_subdirectory_if_exist(${c_CMPT_UI_DIR})
ameba_add_subdirectory(${c_CMPT_DYN_APP_DIR})
ameba_add_subdirectory(${c_CMPT_OS_DIR})
ameba_add_subdirectory(${c_CMPT_DIAGNOSE_DIR})
ameba_add_subdirectory_if_exist(${c_CMPT_TFLITE_DIR}/cmake)
ameba_add_subdirectory(${c_CMPT_WIFI_DIR})
ameba_add_subdirectory_if_exist(${c_CMPT_APP_DIR})
ameba_add_subdirectory(${c_CMPT_RTK_COEX_DIR})
ameba_add_subdirectory_if_exist(${c_CMPT_SOC_DIR}/verification)
ameba_add_subdirectory_if_exist(${c_CMPT_SOC_DIR}/rom)
ameba_add_subdirectory(${c_CMPT_MISC_DIR})

ameba_add_subdirectory(${c_CMPT_CHIPINFO_DIR})
ameba_add_subdirectory(${c_CMPT_PMC_DIR})

ameba_add_subdirectory(${c_MCU_PROJECT_DIR}/src)

ameba_add_subdirectory_if(CONFIG_MATTER_EN
    ${c_CMPT_MATTER_DIR}/project/cmake lib_matter
)

# TODO: need to catch the error
add_custom_target(atf ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Building atf project"
    COMMAND make -j CROSS_COMPILE=${CROSS_COMPILE} PROJECT_DIR=${c_SOC_PROJECT_DIR}/project_ap BUILD_BASE=${CMAKE_BINARY_DIR} -C ${c_CMPT_SOC_DIR}/atf all
    WORKING_DIRECTORY ${c_CMPT_SOC_DIR}/atf
    COMMENT "Building atf project using make"
)

set_target_properties(atf PROPERTIES
    ADDITIONAL_CLEAN_FILES
        ${CMAKE_BINARY_DIR}/project_ap/asdk/make/atf
)

ameba_target_depend_if(CONFIG_MATTER_SECURE_EN atf lib_atf_mbedtls_secure.a)

ameba_target_depend(${c_CURRENT_IMAGE} atf)

###############################  GENERATE IMAGE  ###################################
#---------------- PRE LINK -------------------#'
if(CONFIG_IMG2_FLASH)
    set(IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_xip.ld)
else()
    set(IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_dram.ld)
endif()

add_custom_command(
  TARGET ${c_CURRENT_IMAGE} PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${IMG2_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld -I ${c_MENUCONFIG_DIR}
  COMMENT "build img2 linkerscript rlx8721d.ld"
)
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

    ${SDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${c_SDK_VERSION}/v7-a+neon-fp16/fpu/crtbegin.o
    ${SDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${c_SDK_VERSION}/v7-a+neon-fp16/fpu/crtend.o
    -lm -lstdc++
    ${c_LINK_ROM_LIB} #WARNING: keep this at last
)

#--------------- POST BUILD ------------------#

add_custom_command(
    OUTPUT ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf ${HP_PROJECT_DIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_COMMAND} -E make_directory ${c_SDK_IMAGE_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${c_SDK_IMAGE_TARGET_DIR}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${c_SDK_IMAGE_TARGET_DIR}/text_image2.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${c_CURRENT_IMAGE}.axf ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf
    COMMAND ${CMAKE_NM} ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf | sort > ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
    COMMAND ${CMAKE_OBJDUMP} -d ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf > ${c_SDK_IMAGE_TARGET_DIR}/target_img2.asm
    COMMAND ${CMAKE_OBJDUMP} -d -j .data ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf >> ${c_SDK_IMAGE_TARGET_DIR}/target_img2.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/target_img2.asm ${HP_PROJECT_DIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_STRIP} ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      ca32_image2_all.bin"

    COMMAND ${CMAKE_COMMAND}
        # common variables
        -Dc_BASEDIR=${c_BASEDIR}  # dir of sdk's root
        -Dc_CMAKE_FILES_DIR=${c_CMAKE_FILES_DIR} # dir of cmake scripts
        -Dc_SOC_PROJECT_DIR=${c_SOC_PROJECT_DIR} # dir of soc project
        -Dc_MCU_PROJECT_DIR=${c_MCU_PROJECT_DIR} # dir of current mcu project
        -Dc_MCU_PROJECT_NAME=${c_MCU_PROJECT_NAME} # name of current mcu project
        -Dc_SDK_IMAGE_TARGET_DIR=${c_SDK_IMAGE_TARGET_DIR} # dir of image output
        -DBUILD_BASE=${CMAKE_BINARY_DIR}
        -Dc_MCU_KCONFIG_FILE=${c_MCU_KCONFIG_FILE}
        -Dc_SOC_TYPE=${c_SOC_TYPE}

        # user's variables
        -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
        -DBUILD_TYPE=${BUILD_TYPE}
        -DANALYZE_MP_IMG=${ANALYZE_MP_IMG}
        -DDAILY_BUILD=${DAILY_BUILD}
        -DEXTERN_DIR=${EXTERN_DIR}

        # special variables for current image

        # cmake toolchain variables
        -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
        -DCMAKE_SIZE=${CMAKE_SIZE}
        -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}
        -DCROSS_COMPILE=${CROSS_COMPILE}

        -P ${CMAKE_CURRENT_SOURCE_DIR}/${c_POST_BUILD_SCRIPT}
    DEPENDS ${c_CURRENT_IMAGE}
    COMMENT "${c_CURRENT_IMAGE} post handle"
)

add_custom_target(
    ${c_CURRENT_IMAGE}_postbuild
    ALL
    DEPENDS target_wifi_nouse
)

add_custom_target(
    target_wifi_nouse
    DEPENDS ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf ${HP_PROJECT_DIR}/asdk/image/target_img2_otrcore.asm
)

################################  EXTRA PROCESS  ###################################

set(clean_files
    ${c_SDK_IMAGE_TARGET_DIR}
)

set(link_deps
    ${IMG2_LD}
    ${c_SOC_PROJECT_DIR}/ameba_layout.ld
)

set_target_properties(${c_CURRENT_IMAGE} PROPERTIES
    #WARNING: values for these parameter MUST be placed inside a pair of quotes
    ADDITIONAL_CLEAN_FILES
        "${clean_files}"
    LINK_DEPENDS
        "${link_deps}"
)
