set(CMAKE_EXECUTABLE_SUFFIX .axf)
ameba_set_if(d_VERBOSE_MAKEFILE CMAKE_VERBOSE_MAKEFILE ON p_ELSE OFF)

add_compile_options(-fdata-sections -ffunction-sections)

ameba_executable(target_img2_${PROJECT_NAME} ${c_EMPTY_C_FILE})

############################make_subdirs####################################
ameba_add_subdirectories(
    make/os                  # make -C os all
    make/fwlib               # make -C fwlib all
    make/hal                 # make -C hal all
    make/swlib               # make -C swlib all
    make/project             # make -C project all
    make/at_cmd              # make -C at_cmd all
    make/test                # make -C test all
    make/file_system         # make -C file_system all
    make/audio               # make -C audio all
    make/utilities_example   # make -C utilities_example all
    make/xlat_table          # make -C xlat_table all
    make/utils               # make -C utils all
)

ameba_add_subdirectories_if(CONFIG_DYNAMIC_APP_LOAD_EN
    make/dynamic_app_load
)

ameba_add_subdirectories_if(CONFIG_USB_OTG_EN
    make/usb_otg             # make -C usb_otg all
)

ameba_add_subdirectories_if(CONFIG_TFLITE_MICRO_EN
    make/tflite_micro
)

if(CONFIG_WLAN)
    add_subdirectory(${c_WIFI_CMAKE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/wlan)
    if(CONFIG_AS_INIC_AP) # AP
        ameba_add_subdirectories_ifnot(CONFIG_MP_SHRINK make/network)
        ameba_add_subdirectories(make/utilities)
    elseif(CONFIG_AS_INIC_NP) # NP
        add_subdirectory(make/api)
    endif()
endif()


ameba_add_subdirectories_if(CONFIG_MBEDTLS_ENABLED
    make/mbedtls             # make -C mbedtls all
)
ameba_add_subdirectories_if(CONFIG_BT
    make/bluetooth           # make -C bluetooth all
)
ameba_add_subdirectories_if(CONFIG_802154_EN
    make/wpan               # make -C wpan all
)
ameba_add_subdirectories(
    make/application        # make -C application all
    make/ui                 # make -C ui all
)
ameba_add_subdirectories_if(CONFIG_RANGE_ENABLED
    make/range              # make -C range all
)

add_custom_target(atf ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Building atf project"
    COMMAND make -j CROSS_COMPILE=${CROSS_COMPILE} PROJECT_DIR=${d_PLATFORM_PROJECT_DIR}/project_ap -C ${d_SOC_PLATFORM_DIR}/atf all
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/make/atf
    COMMENT "Building atf project using make"
)

ameba_target_depend(target_img2_${PROJECT_NAME} atf)
############################################################################

#######################make_subdirs_lib_img2################################
if(NOT d_AMEBA_RELEASE)
    ameba_add_subdirectories(
        make/usb_otg/device/lib
        make/usb_otg/host/lib
        make/usb_otg/drd/lib

        make_lib/freertos_lib
        make_lib/chipinfo_lib
        make_lib/pmc_lib
        # make_lib/Vmin_Test_lib
    )

    if(CONFIG_WLAN AND CONFIG_AS_INIC_AP)
        ameba_add_subdirectories(
            make/network/httpc
            make/network/httpd
            make/network/websocket
            make/network/ipnat
            make/network/ipdnrd
            # make/network/eap
            # make/network/http2
            # make/network/tftp

            make/rtk_coex/rtk_coex_api
        )
    endif()

    ameba_add_subdirectories_if(CONFIG_AUDIO_FWK
        make/audio/audio_algorithm     # make -C audio_algorithm lib
        make/audio/audio_effect        # make -C audio_effect lib
        make/audio/audio_control       # make -C audio_control lib
        make/audio/audio_stream        # make -C audio_stream lib
        make/audio/audio_xip           # make -C audio_xip lib
    )

    if(CONFIG_AUDIO_FWK OR CONFIG_MEDIA_PLAYER OR CONFIG_MEDIA_LITE_PLAYER)
        # make -C base lib
        ameba_add_subdirectories(
            make/audio/base/libs        # make -C libs lib
            make/audio/base/log         # make -C log lib
            make/audio/base/osal        # make -C osal lib
            make/audio/base/cutils/src  # make -C cutils lib
            make/audio/base/audio_utils # make -C audio_utils lib
            make/audio/base/xip         # make -C xip lib
        )
    endif()

    if(CONFIG_MEDIA_PLAYER OR CONFIG_MEDIA_LITE_PLAYER)
        ameba_add_subdirectories_if(CONFIG_MEDIA_PLAYER
            make/audio/media_codec/codec-omx       # make -C media_codec codec-omx
            make/audio/media_codec/common          # make -C media_codec codec-omx
            make/audio/media_core/standard         # make -C media_core standard
            make/audio/media_libmedia/standard     # make -C media_libmedia standard
        )
        ameba_add_subdirectories_if(CONFIG_MEDIA_LITE_PLAYER
            make/audio/media_codec/codec-simple    # make -C media_codec codec-simple
            make/audio/media_codec/common          # make -C media_codec codec-simple
            make/audio/media_core/lite             # make -C media_core lite
            make/audio/media_libmedia/lite         # make -C media_libmedia lite
        )
        ameba_add_subdirectories(
            make/audio/media_utils     # make -C media_utils lib
            make/audio/media_common    # make -C media_common lib
            make/audio/media_source    # make -C media_source lib
            make/audio/media_demux     # make -C media_demux lib
            # make/audio/media_libmedia  # make -C media_libmedia component

            #NOTE: media_xip must be added after its linking subdirectory like make/audio/media_libmedia/standard
            #FIXME: better way to solve the problem?
            make/audio/media_xip       # make -C media_xip lib
        )
    endif()
    ameba_add_subdirectories_if(CONFIG_MEDIA_LITE_PLAYER
        make/audio/third_party/flac        # make -C third_party wfa
    )
    ameba_add_subdirectories(
        make/audio/third_party             # make -C third_party lib
    )

    # make -C make/application/wifi-audio lib
    ameba_add_subdirectories_if(CONFIG_WIFI_AUDIO
        make/application/wifi-audio/libs
        make/application/wifi-audio/xip
    )

    # make -C make/utils lib
    ameba_add_subdirectories(
        make/utils/ipc/rpc/framework/libs
        make/utils/ipc/rpc/framework/xip
    )

    # make -C make/ui lib
    ameba_add_subdirectories(
        make/ui/third_party
    )

    if(CONFIG_BT AND CONFIG_BT_AUDIO_CODEC_SUPPORT)
        ameba_add_subdirectories(
            make/bluetooth/bt_audio/bt_codec    # make -C make/bluetooth/bt_audio/bt_codec all
            make/bluetooth/bt_audio/audio_sync  # make -C make/bluetooth/bt_audio/audio_sync all
        )
    endif()
endif()
############################################################################

get_property(IMG2_LIBS_PROPERTY GLOBAL PROPERTY g_${d_MCU_PROJECT_NAME}_IMAGE2_LIBS)

ameba_info("g_${d_MCU_PROJECT_NAME}_IMAGE2_LIBS: ${IMG2_LIBS_PROPERTY}")
ameba_info("d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_LIBRARIES: ${d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_LIBRARIES}")

target_link_libraries(
    target_img2_${PROJECT_NAME}
    PRIVATE
    -T${CMAKE_CURRENT_BINARY_DIR}/make/rlx8721d.ld
    d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map

    -Wl,--whole-archive
    ${IMG2_LIBS_PROPERTY}
    -Wl,--no-whole-archive

    #NOTE: DONOT add -Wl,--whole-archive for this, for they are added in component/build_info/dependency_*.cmake
    ${d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_LIBRARIES}

    ${d_TOOLCHAIN_DIR}/lib/gcc/arm-none-eabi/${d_SDK_VERSION}/v7-a+neon-fp16/fpu/crtbegin.o
    ${d_TOOLCHAIN_DIR}/lib/gcc/arm-none-eabi/${d_SDK_VERSION}/v7-a+neon-fp16/fpu/crtend.o
    -lm -lstdc++
)

foreach(dep IN LISTS d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_DEPS)
    ameba_target_depend(target_img2_${PROJECT_NAME} ${dep})
endforeach()


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
    -Dd_PLATFORM_TYPE=${d_PLATFORM_TYPE}
    -DCROSS_COMPILE=${CROSS_COMPILE}
    -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
    -DTOOLCHAIN_DIR=${TOOLCHAIN_DIR}
    -DASDK_VER=${ASDK_VER}
    -DTOOLCHAINVER=${TOOLCHAINVER}
    -DCONFIG_AMEBA_RLS=${CONFIG_AMEBA_RLS}
    -DCONFIG_LINUX_FW_EN=${CONFIG_LINUX_FW_EN}
    -DATF_TARGET_DIR=${d_MCU_PROJECT_DIR}/asdk/build/atf
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
    ${d_PLATFORM_PROJECT_DIR}/${d_PLATFORM_TYPE}_layout.ld
)

set_target_properties(target_img2_${PROJECT_NAME} PROPERTIES LINK_DEPENDS "${LINK_SCRIPTS}")


set_property(TARGET target_img2_${PROJECT_NAME}
    PROPERTY ADDITIONAL_CLEAN_FILES
    ${IMAGE_TARGET_FOLDER}
    ${d_MCU_PROJECT_DIR}/asdk/build/atf
)
##########################  CUSTOM TARGET  ###################################

add_custom_target(
    ${PROJECT_NAME}
    DEPENDS target_img2_${PROJECT_NAME}
)
