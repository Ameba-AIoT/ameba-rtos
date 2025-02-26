ameba_add_image(target_img2_${PROJECT_NAME})
# variable c_CURRENT_IMAGE value is target_img2_${PROJECT_NAME}

ameba_add_subdirectory(${c_CMPT_AT_CMD_DIR})
ameba_add_subdirectory(${c_CMPT_FWLIB_DIR})
ameba_add_subdirectory(${c_CMPT_HAL_DIR})
ameba_add_subdirectory(${c_CMPT_SWLIB_DIR})
ameba_add_subdirectory(${c_CMPT_FILE_SYSTEM_DIR})
ameba_add_subdirectory(${c_CMPT_EXAMPLE_DIR})
ameba_add_subdirectory(${c_CMPT_AUDIO_DIR})
ameba_add_subdirectory(${c_CMPT_UTILS_DIR})
ameba_add_subdirectory(${c_CMPT_USB_DIR})
ameba_add_subdirectory(${c_CMPT_AUDIO_DIR})
ameba_add_subdirectory(${c_CMPT_WPAN_DIR})
ameba_add_subdirectory(${c_CMPT_BLUETOOTH_DIR})
ameba_add_subdirectory(${c_CMPT_NETWORK_DIR})
ameba_add_subdirectory(${c_CMPT_SSL_DIR})
ameba_add_subdirectory(${c_CMPT_UI_DIR})
ameba_add_subdirectory(${c_CMPT_DYN_APP_DIR})
ameba_add_subdirectory(${c_CMPT_OS_DIR})
ameba_add_subdirectory(${c_CMPT_TFLITE_DIR})
ameba_add_subdirectory(${c_CMPT_WIFI_DIR}/wifi_make)
ameba_add_subdirectory(${c_CMPT_APP_DIR})
ameba_add_subdirectory(${c_CMPT_RTK_COEX_DIR})
ameba_add_subdirectory(${c_CMPT_SOC_DIR}/cmake/chipinfo)
ameba_add_subdirectory(${c_CMPT_SOC_DIR}/cmake/pmc)
ameba_add_subdirectory(${c_CMPT_SOC_DIR}/verification)

ameba_add_subdirectory(${c_MCU_PROJECT_DIR}/src)

if(NOT d_AMEBA_RELEASE)
    ameba_add_subdirectories(
        make_lib/freertos_lib
        make_lib/chipinfo_lib
        make_lib/pmc_lib
        # make_lib/Vmin_Test_lib
    )
endif()

############################make_subdirs####################################

add_custom_target(atf ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Building atf project"
    COMMAND make -j CROSS_COMPILE=${CROSS_COMPILE} PROJECT_DIR=${d_PLATFORM_PROJECT_DIR}/project_ap -C ${d_SOC_PLATFORM_DIR}/atf all
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/make/atf
    COMMENT "Building atf project using make"
)

ameba_target_depend(${c_CURRENT_IMAGE} atf)
############################################################################

target_link_libraries(
    ${c_CURRENT_IMAGE}
    PRIVATE
    -T${CMAKE_CURRENT_BINARY_DIR}/make/rlx8721d.ld
    ${c_MCU_PROJ_CONFIG}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map

    -Wl,--whole-archive
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_libraries>
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_whole_achive_libs>
    -Wl,--no-whole-archive

    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_no_whole_achive_libs>

    ${d_TOOLCHAIN_DIR}/lib/gcc/arm-none-eabi/${d_SDK_VERSION}/v7-a+neon-fp16/fpu/crtbegin.o
    ${d_TOOLCHAIN_DIR}/lib/gcc/arm-none-eabi/${d_SDK_VERSION}/v7-a+neon-fp16/fpu/crtend.o
    -lm -lstdc++
)

add_custom_command(
    OUTPUT ${IMAGE_TARGET_FOLDER}/target_img2.axf ${HP_PROJECT_DIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${IMAGE_TARGET_FOLDER}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${IMAGE_TARGET_FOLDER}/text_image2.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/target_img2_${PROJECT_NAME}.axf ${IMAGE_TARGET_FOLDER}/target_img2.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_img2.axf | sort > ${IMAGE_TARGET_FOLDER}/target_img2.map
    COMMAND ${CMAKE_OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_img2.axf > ${IMAGE_TARGET_FOLDER}/target_img2.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.axf ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.asm ${HP_PROJECT_DIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      ca32_image2_all.bin"

    COMMAND ${CMAKE_COMMAND}
    -Dd_PLATFORM_PROJECT_DIR=${d_PLATFORM_PROJECT_DIR}
    -Dc_SOC_TYPE=${c_SOC_TYPE}
    -DCROSS_COMPILE=${CROSS_COMPILE}
    -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
    -DTOOLCHAIN_DIR=${TOOLCHAIN_DIR}
    -DASDK_VER=${ASDK_VER}
    -DTOOLCHAINVER=${TOOLCHAINVER}
    -DCONFIG_AMEBA_RLS=${CONFIG_AMEBA_RLS}
    -DCONFIG_LINUX_FW_EN=${CONFIG_LINUX_FW_EN}
    -DATF_TARGET_DIR=${c_MCU_PROJECT_DIR}/asdk/build/atf
    -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DBASEDIR=${c_BASEDIR} -DKM4_PROJECT_DIR=${KM4_PROJECT_DIR} -DPROJECTDIR=${PROJECTDIR}
    -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR} -DCMAKE_FILES_DIR=${CMAKE_FILES_DIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}
    -DBUILD_TYPE=${BUILD_TYPE} -DANALYZE_MP_IMG=${ANALYZE_MP_IMG} -DDAILY_BUILD=${DAILY_BUILD}
    -DCONFIG_BT=${CONFIG_BT} -DCONFIG_MP_SHRINK=${CONFIG_MP_SHRINK} -DCONFIG_MP_INCLUDED=${CONFIG_MP_INCLUDED}
    -DCONFIG_FATFS_WITHIN_APP_IMG=${CONFIG_FATFS_WITHIN_APP_IMG} -DCONFIG_COMPRESS_OTA_IMG=${CONFIG_COMPRESS_OTA_IMG} -DCONFIG_UPGRADE_BOOTLOADER=${CONFIG_UPGRADE_BOOTLOADER}
    -DCONFIG_DYNAMIC_APP_LOAD_EN=${CONFIG_DYNAMIC_APP_LOAD_EN}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/img2_postbuild.cmake

    DEPENDS target_img2_${PROJECT_NAME}
    COMMENT "target_img2_${PROJECT_NAME} post handle"
)

add_custom_target(
    target_img2_${PROJECT_NAME}_postbuild
    ALL
    DEPENDS target_wifi_nouse
)

add_custom_target(
    target_wifi_nouse
    DEPENDS ${IMAGE_TARGET_FOLDER}/target_img2.axf ${HP_PROJECT_DIR}/asdk/image/target_img2_otrcore.asm
)

if(CONFIG_XIP_FLASH)
    set(IMG2_LD ${d_SDK_LD_DIR}/ameba_img2_xip.ld)
else()
    set(IMG2_LD ${d_SDK_LD_DIR}/ameba_img2_dram.ld)
endif()

add_custom_command(
  TARGET target_img2_${PROJECT_NAME} PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${IMG2_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/make/rlx8721d.ld
  COMMENT "build img2 linkerscript rlx8721d.ld"
)

set(
    LINK_SCRIPTS
    ${IMG2_LD}
    ${d_PLATFORM_PROJECT_DIR}/${c_SOC_TYPE}_layout.ld
)

set_target_properties(target_img2_${PROJECT_NAME} PROPERTIES LINK_DEPENDS "${LINK_SCRIPTS}")


set_property(TARGET target_img2_${PROJECT_NAME}
    PROPERTY ADDITIONAL_CLEAN_FILES
    ${IMAGE_TARGET_FOLDER}
    ${c_MCU_PROJECT_DIR}/asdk/build/atf
)
##########################  CUSTOM TARGET  ###################################

add_custom_target(
    ${PROJECT_NAME}
    DEPENDS target_img2_${PROJECT_NAME}
)
