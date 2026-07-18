if(CONFIG_SOC_SINGLE_CORE)
    set(prebuilt_img2_all ${c_MCU_PROJECT_DIR}/lib/bin/${c_MCU_PROJECT_NAME}_image2_all.bin)
    set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY image2 "target_img2_km4ns")
    set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY image2_all
        "${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_image2_all.bin")
    add_custom_target(target_img2_km4ns_postbuild
        ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${c_SDK_IMAGE_TARGET_DIR}
        COMMAND ${CMAKE_COMMAND} -E touch ${c_SDK_IMAGE_TARGET_DIR}/.gitignore
        COMMAND ${CMAKE_COMMAND} -E copy ${prebuilt_img2_all}
            ${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_image2_all.bin
    )
    return()
endif()

###############################  PREBUILT INTERNAL LIBS DETECTION  ###############
# Detect BEFORE ameba_add_image so early-return doesn't leave an empty link target.

# Step 1: consume sealed internal libs ONLY in a released prebuilt SDK.
# Discriminator is CONFIG_AMEBA_RLS (release.py flips it to =y in the released
# Kconfig; dev trees are always =n) AND the presence of lib/internal/*.a.
# Gating on AMEBA_RLS, not on lib/internal presence alone, keeps a normal
# (non-release) build fully decoupled from the seal artifacts: even if a prior
# seal build left stale lib/internal (or lib/bin) behind in the tree, a dev build
# compiles km4ns from source and never links the prebuilt libs. During release
# build_lib AMEBA_RLS is still n (it is set only after all conf passes), so every
# pass, including the ones after the seal build, also rebuilds from source with
# its own config, so there is no config-mismatch link error and no need to reuse
# a prebuilt bin.
# (Uses AMEBA_RLS, not the seal switch, so this block survives the release strip.)
if(CONFIG_AMEBA_RLS AND EXISTS "${c_MCU_PROJECT_DIR}/lib/internal")
    file(GLOB _psc_int_libs "${c_MCU_PROJECT_DIR}/lib/internal/*.a")
    if(_psc_int_libs)
        set(CONFIG_KM4NS_IMG2_PREBUILT TRUE)
    endif()
endif()

# Step 2: seal build -> always recompile from source.
# Defensive only: AMEBA_RLS is already n during a seal, so Step 1 leaves
# PREBUILT false anyway. This block is removed by the release strip step.

if(CONFIG_SOLO)
ameba_add_image(target_img2 p_IMAGE_ALL ${c_MCU_PROJECT_NAME}_image2_all.bin p_TYPE image2 p_HIDE_FROM_ALL_PATH)
else()
ameba_add_image(target_img2 p_IMAGE_ALL ${c_MCU_PROJECT_NAME}_image2_all.bin)
endif()
# Use variable c_CURRENT_IMAGE to represent full image target name

# Note: sealed internal libs are registered per-lib inside ameba_add_internal_library
# path 2, using the MP suffix derived from menuconfig. No GLOB needed here.
# lib/soc and lib/application are registered by their own component subdirs' public sections.

###############################  ADD COMPONENT  ###################################
if (CONFIG_WHC_HOST OR CONFIG_WHC_NONE OR (CONFIG_WHC_INTF_IPC AND CONFIG_WHC_DEV))
    ameba_add_subdirectory(${c_CMPT_UTILS_DIR})
    ameba_add_subdirectory(${c_CMPT_AT_CMD_DIR})
    ameba_add_subdirectory(${c_CMPT_BLUETOOTH_DIR})
    ameba_add_subdirectory(${c_COMPONENT_DIR}/sdn)
    if(NOT CONFIG_SOLO)
        ameba_add_subdirectory(${c_CMPT_BOOTLOADER_DIR}/bootloader_ns)
    endif()
    ameba_add_subdirectory(${c_CMPT_BOOTLOADER_NS_LIB_DIR} lib_bootloader_ns)
    ameba_add_subdirectory(${c_CMPT_CHIPINFO_DIR})
    ameba_add_subdirectory(${c_CMPT_EXAMPLE_DIR})
    ameba_add_subdirectory(${c_CMPT_FILE_SYSTEM_DIR})
    ameba_add_subdirectory(${c_CMPT_FWLIB_DIR})
    ameba_add_subdirectory(${c_CMPT_HAL_DIR})
    ameba_add_subdirectory(${c_CMPT_LWIP_DIR})
    ameba_add_subdirectory(${c_CMPT_MISC_DIR})
    ameba_add_subdirectory(${c_CMPT_MONITOR_DIR})
    ameba_add_subdirectory(${c_CMPT_NETWORK_DIR})
    ameba_add_subdirectory(${c_CMPT_OS_DIR})
    ameba_add_subdirectory(${c_CMPT_PMC_DIR})
    ameba_add_subdirectory(${c_CMPT_RTK_COEX_DIR})
    ameba_add_subdirectory(${c_CMPT_SSL_DIR})
    ameba_add_subdirectory(${c_CMPT_SWLIB_DIR})
    ameba_add_subdirectory(${c_CMPT_CRASHDUMP_DIR})
    ameba_add_subdirectory(${c_CMPT_USRCFG_DIR} usrcfg)
    ameba_add_subdirectory(${c_CMPT_DIAGNOSE_DIR})
    ameba_add_subdirectory_if_exist(${c_CMPT_VERIFICATION_DIR})
    ameba_add_subdirectory_if_exist(${c_CMPT_PYTEST_DIR})
    ameba_add_subdirectory_if_exist(${c_CMPT_APP_DIR}/benchmark)
    ameba_add_subdirectory_if_exist(${c_CMPT_ROM_DIR})
    ameba_add_subdirectory(${c_CMPT_WIFI_DIR}/wifi_fw)
    ameba_add_subdirectory(${c_CMPT_WIFI_DIR})
    ameba_add_subdirectory_if_exist(${c_CMPT_WPAN_DIR})
    ameba_add_subdirectory(${c_CMPT_SOC_DIR})
    ameba_add_subdirectory(${c_CMPT_SOC_OTA_DIR})
    ameba_add_subdirectory(${c_CMPT_OTA_DIR})
    ameba_add_subdirectory(${c_COMPONENT_DIR}/soc/common/flash_wrapper)
    ameba_add_subdirectory(${c_MCU_SRC_DIR})
endif()
###############################  GENERATE IMAGE  ###################################
#---------------- PRE LINK -------------------#
# TRX code placement remap for ameba_sections_rename.py:
#   IMG2 on FLASH : off -> group all TRX into .flash.only.text (flash);
#                   All -> .sram.only.text (SRAM); Partial -> no remap
#   IMG2 on PSRAM/SRAM: force all TRX back to .text (default region), clearing any
#                   .flash.only/.sram.only baked into the shipped libs (otherwise
#                   functions stay stuck in flash after switching from FLASH mode)
if(CONFIG_IMG2_FLASH)
  set(c_TRX_REMAP
    "$<$<NOT:$<BOOL:${CONFIG_WIFI_MOVE_TRX_CODE_TO_SRAM}>>:--remap=.sram.only.text=.flash.only.text>"
    "$<$<BOOL:${CONFIG_WIFI_TRX_SRAM_ALL}>:--remap=.flash.only.text=.sram.only.text>")
else()
  set(c_TRX_REMAP --remap=.flash.only.text=.text,.sram.only.text=.text)
endif()

add_custom_command(
  TARGET ${c_CURRENT_IMAGE} PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${c_IMG2_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld -I ${c_MENUCONFIG_DIR}
  COMMAND ${CMAKE_COMMAND} -E cat ${c_LINK_ROM_SYMBOL} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMAND ${Python3_EXECUTABLE} ${c_BASEDIR}/tools/image_scripts/ameba_sections_rename.py
          --config ${c_SOC_PROJECT_DIR}/section_placement.json5
          --build-dir ${CMAKE_CURRENT_BINARY_DIR}
          --build-dir ${c_SDK_LIB_APPLICATION_DIR}
          --core ${c_MCU_PROJECT_NAME}
          --cross ${CROSS_COMPILE}
          ${c_TRX_REMAP}
  COMMENT "build img2 linkerscript + apply section placements"
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
    ${c_LINK_ROM_LIB}

    -l:crtbegin.o
    -l:crtend.o
    -lm -lstdc++
)

#--------------- POST BUILD ------------------#

add_custom_command(
    OUTPUT ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf
    COMMAND ${CMAKE_COMMAND} -E make_directory ${c_SDK_IMAGE_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${c_SDK_IMAGE_TARGET_DIR}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${c_SDK_IMAGE_TARGET_DIR}/text_image2.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${c_CURRENT_IMAGE}.axf ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf
    COMMAND ${CMAKE_NM} ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf | sort > ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
    COMMAND ${CMAKE_OBJDUMP} -d ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf > ${c_SDK_IMAGE_TARGET_DIR}/target_img2.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/target_img2.asm ${KM4TZ_BUILD_DIR}/${c_SDK_IMAGE_FOLDER_NAME}/target_img2_otrcore.asm
    COMMAND ${CMAKE_STRIP} ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km4ns_image2_all.bin"

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
        -DEXTERN_DIR=${EXTERN_DIR}
        -DCODE_ANALYZE_RETRY=${CODE_ANALYZE_RETRY}

        # special variables for current image
        -DKM4TZ_PROJECT_DIR=${KM4TZ_PROJECT_DIR}

        # cmake toolchain variables
        -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
        -DCMAKE_SIZE=${CMAKE_SIZE}
        -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}

        -P ${CMAKE_CURRENT_SOURCE_DIR}/postbuild.cmake

    COMMENT "${c_CURRENT_IMAGE} post build"
    DEPENDS ${c_CURRENT_IMAGE}
)

add_custom_target(
    ${c_CURRENT_IMAGE}_postbuild
    ALL
    DEPENDS ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf
)

#------------- EXTRA PROCESS -----------------#
set(clean_files
    ${c_SDK_IMAGE_TARGET_DIR}
    ${FINAL_IMAGE_DIR}/app.bin
    ${FINAL_IMAGE_DIR}/app_mp.bin
    ${FINAL_IMAGE_DIR}/km4tz_fullmac.bin
)

set(link_deps
    ${c_LAYOUT_LD}
    ${c_IMG2_LD}
    ${c_LINK_ROM_SYMBOL}
)

set_target_properties(${c_CURRENT_IMAGE} PROPERTIES
    #WARNING: values for these parameter MUST be placed inside a pair of quotes
    ADDITIONAL_CLEAN_FILES
        "${clean_files}"
    LINK_DEPENDS
        "${link_deps}"
)
