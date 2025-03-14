if(NOT CMAKE_REFACTOR) # For temporary compatibility.
set(CMAKE_EXECUTABLE_SUFFIX .axf)
ameba_set_if(d_VERBOSE_MAKEFILE CMAKE_VERBOSE_MAKEFILE ON p_ELSE OFF)

add_compile_options(-fdata-sections -ffunction-sections)

ameba_executable(target_img2_${PROJECT_NAME} ${c_EMPTY_C_FILE})

############################make_loaders####################################
ameba_add_subdirectories(
    make/bootloader
    make/flashloader
)
if(NOT CONFIG_AMEBA_RLS)
    ameba_add_subdirectories(make/imgtool_floader)
endif()
############################################################################

############################make_subdirs####################################

ameba_add_subdirectories(
    make/at_cmd
    make/target
    # make/cmsis
    make/audio
    # make/media
    make/utilities_example
    make/file_system
    make/project
    make/application
    make/cmsis-dsp
    make/test
    make/os
    make/utils
)

ameba_add_subdirectories_if(CONFIG_BT make/bluetooth)

if(CONFIG_WLAN)
    if(CONFIG_AS_INIC_AP) # AP
        ameba_add_subdirectories(make/utilities)
        ameba_add_subdirectories_ifnot(CONFIG_MP_SHRINK make/network)
    elseif(CONFIG_AS_INIC_NP) # NP
        add_subdirectory(${c_WIFI_CMAKE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/wlan)
    elseif(CONFIG_SINGLE_CORE_WIFI) # Single core
        add_subdirectory(${c_WIFI_CMAKE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/wlan)
        ameba_add_subdirectories(make/utilities)
        ameba_add_subdirectories_ifnot(CONFIG_MP_SHRINK make/network)
    endif()
endif()

ameba_add_subdirectories_if(CONFIG_MBEDTLS_ENABLED make/mbedtls)
ameba_add_subdirectories_if(CONFIG_GUI_EN make/ui)
ameba_add_subdirectories_if(CONFIG_LINKKIT_AWSS make/linkkit)
ameba_add_subdirectories_if(CONFIG_USB_OTG_EN make/usb_otg)

if(CONFIG_TRUSTZONE)
    ameba_add_subdirectories(make_secure)
    ameba_list_append(d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_LIBRARIES
        ${IMAGE_TARGET_FOLDER}/cmse_implib.lib
    )
    ameba_target_depend(target_img2_${PROJECT_NAME} cmse_implib_gen)
endif()
############################################################################

#######################make_subdirs_lib_img2################################
if(NOT d_AMEBA_RELEASE)
    ameba_add_subdirectories(
        # make/RT_xmodem
        make_lib/bootloader_lib
        make_lib/chipinfo_lib
        make_lib/crashdump_lib
        make_lib/pmc_lib
    )

    if(CONFIG_WLAN)
        if(CONFIG_AS_INIC_AP) # AP
            ameba_add_subdirectories(
                make/network/httpc
                make/network/httpd
                make/network/websocket
                # make/network/eap
                # make/network/http2
                # make/network/tftp
            )
        elseif(CONFIG_AS_INIC_NP) # NP
            ameba_add_subdirectories(make/rtk_coex)
        elseif(CONFIG_SINGLE_CORE_WIFI) # Single core
            ameba_add_subdirectories(
                make/network/httpc
                make/network/httpd
                make/network/websocket
                # make/network/eap
                # make/network/http2
                # make/network/tftp
                make/rtk_coex
                make/rtk_coex/rtk_coex_api
            )
        endif()
    endif()

    # ameba_add_subdirectories(make/file_system/dct)

    ameba_add_subdirectories_if(CONFIG_AUDIO_EN
        make/cmsis-dsp
        #MMF LIBRARY
        # make/audio/g711
        # make/audio/faac
        # make/audio/haac
        # make/network/rtsp
        make/audio/ac3
        make/audio/amr
        make/audio/flac
        make/audio/haac
        make/audio/hmp3
        make/audio/mp3
        make/audio/gsm610
        make/audio/ekho
        make/audio/libogg-1.3.3
        make/audio/opus-1.3.1
        make/audio/opusfile-0.11
        make/audio/libopusenc-0.2.1
        make/audio/m4a_selfparse
    )

    ameba_add_subdirectories(
        make/usb_otg/device/lib
        make/usb_otg/host/lib
        make/usb_otg/drd/lib
    )

    if(CONFIG_BT AND CONFIG_BT_AUDIO_CODEC_SUPPORT)
        ameba_add_subdirectories(
            make/bluetooth/bt_audio/bt_codec
            make/bluetooth/bt_audio/audio_sync
        )
    endif()

    # make -C make/audio lib
    ameba_add_subdirectories_if(CONFIG_AUDIO_FWK
        make/audio/audio_algorithm     # make -C audio_algorithm lib
        make/audio/audio_effect        # make -C audio_effect lib
        make/audio/audio_control       # make -C audio_control lib
        make/audio/audio_stream        # make -C audio_stream lib
        make/audio/audio_xip           # make -C audio_xip lib
        make/audio/base                # make -C base lib
    )
    ameba_add_subdirectories(
        make/audio/third_party         # make -C third_party lib
    )

    # make -C make/utils lib
    ameba_add_subdirectories(
        make/utils/ipc/rpc/framework/libs
        make/utils/ipc/rpc/framework/xip
    )
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
    ${d_LINK_ROM_LIB}

    ${d_TOOLCHAIN_DIR}/lib/gcc/arm-none-eabi/${d_SDK_VERSION}/thumb/v8-m.main+fp/fpu/crtbegin.o
    ${d_TOOLCHAIN_DIR}/lib/gcc/arm-none-eabi/${d_SDK_VERSION}/thumb/v8-m.main+fp/fpu/crtend.o
    -lm -lstdc++
)

foreach(dep IN LISTS d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_DEPS)
    ameba_target_depend(target_img2_${PROJECT_NAME} ${dep})
endforeach()

add_custom_command(
    TARGET target_img2_${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${IMAGE_TARGET_FOLDER}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${IMAGE_TARGET_FOLDER}/text_image2_ns.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/target_img2_${PROJECT_NAME}.axf ${IMAGE_TARGET_FOLDER}/target_img2.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_img2.axf | sort > ${IMAGE_TARGET_FOLDER}/target_img2.map
    COMMAND ${CMAKE_OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_img2.axf > ${IMAGE_TARGET_FOLDER}/target_img2.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.axf ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.asm ${AP_PROJECT_DIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km4_image2_all.bin"

    COMMAND ${CMAKE_COMMAND}
    -Dd_PLATFORM_PROJECT_DIR=${d_PLATFORM_PROJECT_DIR}
    -Dd_MCU_PROJECT_DIR=${d_MCU_PROJECT_DIR}
    -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
    -DCONFIG_AMEBA_RLS=${CONFIG_AMEBA_RLS}
    -DCONFIG_LINUX_FW_EN=${CONFIG_LINUX_FW_EN}
    -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DBASEDIR=${BASEDIR} -DKM4_PROJECT_DIR=${KM4_PROJECT_DIR} -DPROJECTDIR=${PROJECTDIR}
    -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR} -DCMAKE_FILES_DIR=${CMAKE_FILES_DIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}
    -DBUILD_TYPE=${BUILD_TYPE} -DANALYZE_MP_IMG=${ANALYZE_MP_IMG} -DDAILY_BUILD=${DAILY_BUILD}
    -DCONFIG_BT=${CONFIG_BT} -DCONFIG_MP_SHRINK=${CONFIG_MP_SHRINK} -DCONFIG_MP_INCLUDED=${CONFIG_MP_INCLUDED}
    -DCONFIG_FATFS_WITHIN_APP_IMG=${CONFIG_FATFS_WITHIN_APP_IMG} -DCONFIG_COMPRESS_OTA_IMG=${CONFIG_COMPRESS_OTA_IMG} -DCONFIG_UPGRADE_BOOTLOADER=${CONFIG_UPGRADE_BOOTLOADER}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/img2_postbuild.cmake

    DEPENDS target_img2_${PROJECT_NAME}
    COMMENT "target_img2_${PROJECT_NAME} post build"
)

if(CONFIG_XIP_FLASH)
    set(IMG2_LD ${d_SDK_LD_DIR}/ameba_img2_non_trustzone_xip.ld)
else()
    set(IMG2_LD ${d_SDK_LD_DIR}/ameba_img2_non_trustzone_dram.ld)
endif()

add_custom_command(
  TARGET target_img2_${PROJECT_NAME} PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${IMG2_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/make/rlx8721d.ld -I ${c_MENUCONFIG_DIR}
  COMMAND ${CMAKE_COMMAND} -E cat ${d_LINK_ROM_SYMBOL_NS} >> ${CMAKE_CURRENT_BINARY_DIR}/make/rlx8721d.ld
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
    ${FINAL_IMAGE_DIR}/km0_km4_app.bin
    ${FINAL_IMAGE_DIR}/km0_km4_ca32_app.bin
    ${FINAL_IMAGE_DIR}/ota_all.bin
    ${FINAL_IMAGE_DIR}/km4_boot_all.bin
    ${FINAL_IMAGE_DIR}/km0_km4_app_mp.bin
    ${FINAL_IMAGE_DIR}/km0_km4_ca32_app_mp.bin
)
##########################  CUSTOM TARGET  ###################################

add_custom_target(
    gdb
    COMMAND ${CMAKE_COMMAND} -E echo "Note: please use command :./build.py -gdb"
    COMMAND ${CMAKE_COMMAND} -E echo " -------------   HELP   -----------------"
    COMMAND ${CMAKE_COMMAND} -E echo " ./build.py -gdb for loading image with gdb"
    COMMAND ${CMAKE_COMMAND} -E echo " ./build.py -debug for debugging with gdb"
)

add_custom_target(
    gen_imgtool_floader
    COMMAND ${CMAKE_COMMAND}
        -DFILEPATH=${d_SDK_IMGTOOL_FLOADER_DIR}/${d_PLATFORM_TYPE}_acut/imgtool_flashloader_raw.bin
        -P${CMAKE_FILES_DIR}/checkexists.cmake
    COMMAND ${CMAKE_COMMAND} -E copy ${d_SDK_IMGTOOL_FLOADER_DIR}/${d_PLATFORM_TYPE}_acut/imgtool_flashloader_raw.bin ${IMAGE_TARGET_FOLDER}/ram_1_prepend.bin
    COMMAND ${IMAGETOOL} ${IMAGE_TARGET_FOLDER}/ram_1_prepend.bin ${BUILD_TYPE}
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/imgtool_flashloader.bin ${d_SDK_IMGTOOL_FLOADER_DIR}/${d_PLATFORM_TYPE}_acut/floader_${d_PLATFORM_TYPE}.bin
)

add_custom_target(
    ${PROJECT_NAME}
    DEPENDS target_bootloader target_img2_${PROJECT_NAME}
)
else() # For temporary compatibility.NOTE:  script below is new version cmake
set(CMAKE_EXECUTABLE_SUFFIX .axf)

add_compile_options(-fdata-sections -ffunction-sections)

if(CONFIG_AMEBASMART_B_CUT)
    ameba_set(c_ROM_LIB_DIR ${c_SDK_LIB_DIR}/amebasmart_rom_bcut)
    ameba_set(c_FLOADER_BIN_DIR ${c_SDK_GNU_UTILITY_DIR}/image_tool_flashloader/${c_SOC_TYPE}_acut)
else()
    ameba_fatal("CONFIG_AMEBASMART_B_CUT not defined as TRUE")
endif()

if(CONFIG_LINK_ROM_SYMB)
    ameba_unset(c_LINK_ROM_LIB)
    ameba_unset(c_LINK_ROM_LIB_NS)
    if(CONFIG_AMEBASMART_B_CUT)
        ameba_set(c_LINK_ROM_SYMBOL_NS    ${c_SDK_LD_DIR}/ameba_rom_symbol_acut.ld)
        ameba_set(c_LINK_ROM_SYMBOL_S     ${c_SDK_LD_DIR}/ameba_rom_symbol_bcut_s.ld)
    endif()
else()
    ameba_set(c_LINK_ROM_LIB ${c_ROM_LIB_DIR}/rom.a)
    ameba_set(c_LINK_ROM_LIB_NS ${c_ROM_LIB_DIR}/rom_ns.a)
endif()
ameba_list_append(c_LINK_ROM_SYMBOL_NS    ${c_SDK_LD_DIR}/ameba_rom_symbol_stdlib.ld)
ameba_add_subdirectory(make/image1)
ameba_add_subdirectory(make/image2)
# ameba_add_subdirectory(make/image3)
ameba_add_subdirectory(make/image_floader)
ameba_add_subdirectory_ifnot(CONFIG_AMEBA_RLS make/image_imgtool_floader)

add_custom_target(
    ${c_MCU_PROJECT_NAME}
    COMMENT "build ${c_MCU_PROJECT_NAME} only"
    DEPENDS $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image_list>
)

add_custom_target(
    gdb
    COMMAND ${CMAKE_COMMAND} -E echo "Note: please use command :./build.py -gdb"
    COMMAND ${CMAKE_COMMAND} -E echo " -------------   HELP   -----------------"
    COMMAND ${CMAKE_COMMAND} -E echo " ./build.py -gdb for loading image with gdb"
    COMMAND ${CMAKE_COMMAND} -E echo " ./build.py -debug for debugging with gdb"
)

add_custom_target(
    gen_imgtool_floader
    COMMAND ${CMAKE_COMMAND}
        -DFILEPATH=${c_SDK_IMGTOOL_FLOADER_DIR}/${c_SOC_TYPE}_acut/imgtool_flashloader_raw.bin
        -P${c_CMAKE_FILES_DIR}/checkexists.cmake
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMGTOOL_FLOADER_DIR}/${c_SOC_TYPE}_acut/imgtool_flashloader_raw.bin ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin
    COMMAND ${IMAGETOOL} ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin ${BUILD_TYPE}
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/imgtool_flashloader.bin ${c_SDK_IMGTOOL_FLOADER_DIR}/${c_SOC_TYPE}_acut/floader_${c_SOC_TYPE}.bin
)

endif()# For temporary compatibility.
