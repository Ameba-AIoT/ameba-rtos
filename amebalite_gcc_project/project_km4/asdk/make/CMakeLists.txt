ameba_executable(target_img2_${PROJECT_NAME} empty_file.c)
target_compile_definitions(target_img2_${PROJECT_NAME} PRIVATE IMAGE2_BUILD)

add_custom_target(
    image2_${PROJECT_NAME}
    COMMENT "build img2 only"
    DEPENDS target_img2_${PROJECT_NAME}
)


define_property(GLOBAL PROPERTY IMG2_LIBS_${PROJECT_NAME}
BRIEF_DOCS "internal library lists which will be linked to img2"
FULL_DOCS "internal library lists which will be linked to img2"
)
set_property(GLOBAL PROPERTY IMG2_LIBS_${PROJECT_NAME} "")

add_subdirectory(at_cmd)
add_subdirectory(cmsis)
add_subdirectory(cmsis-dsp)
#audio
if(CONFIG_AUDIO_FWK)
    add_subdirectory(audio/audio_config)
    add_subdirectory(audio/audio_hal)
    # add_subdirectory(audio/audio_test)
    # add_subdirectory(audio/audio_hal/test)
    # add_subdirectory(audio/base/cutils/cmd)
    # add_subdirectory(audio/audio_cmds)
endif()

if(CONFIG_MEDIA_PLAYER OR CONFIG_MEDIA_LITE_PLAYER)
    add_subdirectory(audio/media_config)
    # add_subdirectory(audio/media_test)
endif()

add_subdirectory(file_system)
add_subdirectory(libnosys)
add_subdirectory(os)
add_subdirectory(project)
add_subdirectory(target)
add_subdirectory(test)
add_subdirectory(utils)

if(CONFIG_BT)
    add_subdirectory(bluetooth)
endif()

if(CONFIG_802154_EN)
    add_subdirectory(wpan)
endif()

if(CONFIG_WLAN)
    add_subdirectory(${WIFIMAKEDIR} ${CMAKE_CURRENT_BINARY_DIR}/wlan)
    #AP
    if(CONFIG_AS_INIC_AP)
        if(NOT CONFIG_MP_SHRINK)
            add_subdirectory(network)
        endif()
        add_subdirectory(application)
        add_subdirectory(utilities)
        add_subdirectory(utilities_example)
    #NP
    elseif(CONFIG_AS_INIC_NP)
    #Single Core
    elseif(CONFIG_SINGLE_CORE_WIFI)
        if(NOT CONFIG_MP_SHRINK)
            add_subdirectory(network)
        endif()
        add_subdirectory(application)
        add_subdirectory(utilities)
        add_subdirectory(utilities_example)
    endif()
endif()

if(CONFIG_MBEDTLS_ENABLED)
    add_subdirectory(mbedtls)
endif()

if(CONFIG_GUI_EN)
    add_subdirectory(ui)
endif()

if(CONFIG_LINKKIT_AWSS)
    add_subdirectory(linkkit)
endif()

get_property(IMG2_LIBS_PROPERTY GLOBAL PROPERTY IMG2_LIBS_${PROJECT_NAME})

target_link_libraries(
    target_img2_${PROJECT_NAME}
    PRIVATE ameba_interface_${PROJECT_NAME} ameba_wrap_interface_${PROJECT_NAME}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map
    -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    -Wl,--whole-archive
    ${IMG2_LIBS_PROPERTY}
    ${LINK_APP_LIB}
    -Wl,--no-whole-archive
    ${LINK_ROM_LIB_NS}
    ${LINK_THIRD_APP_LIB}
    ${ASDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${ASDK_VER}/thumb/v8-m.main+fp/fpu/crtbegin.o
    ${ASDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${ASDK_VER}/thumb/v8-m.main+fp/fpu/crtend.o
    -lm -lstdc++
)

add_custom_command(
    OUTPUT ${IMAGE_TARGET_FOLDER}/target_img2.axf ${KR4_BUILDDIR}/vsdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${IMAGE_TARGET_FOLDER}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${IMAGE_TARGET_FOLDER}/text_image2_ns.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/target_img2_${PROJECT_NAME}.axf ${IMAGE_TARGET_FOLDER}/target_img2.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_img2.axf | sort > ${IMAGE_TARGET_FOLDER}/target_img2.map
    COMMAND ${CMAKE_OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_img2.axf > ${IMAGE_TARGET_FOLDER}/target_img2.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.axf ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.asm ${KR4_BUILDDIR}/vsdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km4_image2_all.bin"

    COMMAND ${CMAKE_COMMAND}
    -DAMEBA_RLS=${AMEBA_RLS}
    -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DBASEDIR=${BASEDIR} -DPROJECTDIR=${PROJECTDIR} -DDSP_IMAGE_TARGET_DIR=${DSP_IMAGE_TARGET_DIR}
    -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR} -DCMAKE_FILES_DIR=${CMAKE_FILES_DIR} -DAP_IMAGEDIR=${AP_IMAGEDIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}
    -DBUILD_TYPE=${BUILD_TYPE} -DANALYZE_MP_IMG=${ANALYZE_MP_IMG} -DDAILY_BUILD=${DAILY_BUILD}
    -DCONFIG_BT=${CONFIG_BT} -DCONFIG_MP_SHRINK=${CONFIG_MP_SHRINK} -DCONFIG_MP_INCLUDED=${CONFIG_MP_INCLUDED}
    -DCONFIG_FATFS_WITHIN_APP_IMG=${CONFIG_FATFS_WITHIN_APP_IMG} -DCONFIG_COMPRESS_OTA_IMG=${CONFIG_COMPRESS_OTA_IMG}
    -DCONFIG_UPGRADE_BOOTLOADER=${CONFIG_UPGRADE_BOOTLOADER} -DCONFIG_DSP_WITHIN_APP_IMG=${CONFIG_DSP_WITHIN_APP_IMG}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/img2_postbuild.cmake

    DEPENDS target_img2_${PROJECT_NAME}
)

add_custom_target(
    target_img2_${PROJECT_NAME}_postbuild
    ALL
    DEPENDS target_wifi_nouse
)

add_custom_target(
    target_wifi_nouse
    DEPENDS ${IMAGE_TARGET_FOLDER}/target_img2.axf ${KR4_BUILDDIR}/vsdk/image/target_img2_otrcore.asm
)

if(CONFIG_MP_SHRINK)
    set(IMG2_LD ${LDDIR}/ameba_img2_all_mp.ld)
else()
    set(IMG2_LD ${LDDIR}/ameba_img2_all.ld)
endif()

if(CONFIG_TRUSTZONE)
    set(LINK_ROM_SYMBOL ${LINK_ROM_SYMBOL_NS})
else()
    set(LINK_ROM_SYMBOL ${LINK_ROM_SYMBOL_S})
endif()


add_custom_command(
  TARGET target_img2_${PROJECT_NAME} PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${IMG2_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMAND ${CMAKE_COMMAND} -E cat ${LINK_ROM_SYMBOL} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMAND ${CMAKE_COMMAND} -E cat ${LINK_ROM_SYMBOL_STDLIB} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMENT "build img2 linkerscript rlx8721d.ld"
)

set(
    LINK_SCRIPTS
    ${IMG2_LD}
    ${PROJECTDIR}/../amebalite_layout.ld
)

set_target_properties(target_img2_${PROJECT_NAME} PROPERTIES LINK_DEPENDS "${LINK_SCRIPTS}")


set_property(TARGET target_img2_${PROJECT_NAME}
    PROPERTY ADDITIONAL_CLEAN_FILES
    ${IMAGE_TARGET_FOLDER}
    ${IMAGE_TARGET_FOLDER}/../../../kr4_km4_app.bin
    ${IMAGE_TARGET_FOLDER}/../../../kr4_km4_dsp_app.bin
    ${IMAGE_TARGET_FOLDER}/../../../kr4_km4_app_mp.bin
    ${IMAGE_TARGET_FOLDER}/../../../ota_all.bin
)
