if(NOT CMAKE_REFACTOR) # For temporary compatibility.
set(CMAKE_EXECUTABLE_SUFFIX .axf)

############################ COMPILE FLAGS ##############################
add_compile_options(
	"$<$<COMPILE_LANGUAGE:C>:${GLOBAL_C_OPTIONS}>"
	"$<$<COMPILE_LANGUAGE:CXX>:${GLOBAL_CPP_OPTIONS}>"
    "$<$<COMPILE_LANGUAGE:ASM>:${GLOBAL_ASM_OPTIONS}>"
)

add_compile_definitions(${GLOBAL_C_DEFINES})

############################ interface library ##############################
add_library(ameba_interface_${PROJECT_NAME} INTERFACE)
add_library(ameba_wrap_interface_${PROJECT_NAME} INTERFACE)

############################ LINK FLAGS ##############################

target_link_options(ameba_interface_${PROJECT_NAME} INTERFACE ${LINK_OPTIONS})
target_link_options(ameba_wrap_interface_${PROJECT_NAME} INTERFACE ${LINK_OPTIONS_WRAP})

############################ INCLUDE DIRECTORIES ##############################
include(includeflags.cmake)
target_include_directories(ameba_interface_${PROJECT_NAME} INTERFACE ${GLOBAL_IFLAGS})

############################ GEN BUILD_INFO_H ##############################
add_custom_target(
    build_info_${PROJECT_NAME}
    ALL
    COMMENT "generate build_info.h"
    COMMAND ${CMAKE_COMMAND} -DPROJECTDIR=${PROJECTDIR} -DCMAKE_FILES_DIR=${CMAKE_FILES_DIR} -DCONFIG_TOOLCHAIN_ARM_GCC=${CONFIG_TOOLCHAIN_ARM_GCC} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -P ${CMAKE_FILES_DIR}/buildinfo.cmake
)
set_property(TARGET build_info_${PROJECT_NAME}
    PROPERTY ADDITIONAL_CLEAN_FILES
    ${PROJECTDIR}/inc/build_info.h
)

############################################################################

include(libs.cmake)

define_property(GLOBAL PROPERTY ALL_EXE_TARGETS_${PROJECT_NAME}
BRIEF_DOCS "all executable targets"
FULL_DOCS "all executable targets"
)

################################# BUILD ROM   ##############################

#TODO

############################################################################

add_compile_options(-fdata-sections  -ffunction-sections)

############################  BUILD LOADER #################################

add_subdirectory(make/bootloader)

add_subdirectory(make/flashloader)

if(NOT CONFIG_AMEBA_RLS)
    add_subdirectory(make/imgtool_floader)
endif()

########################### BUILD APPLICATION ##############################

add_subdirectory(make)

add_subdirectory(make_secure)
############################# BUILD LIBS ###################################
if(NOT CONFIG_AMEBA_RLS)
    ## build soc libs
    add_subdirectory(make_lib/bootloader_lib)

    add_subdirectory(make_lib/chipinfo_lib)

    add_subdirectory(make_lib/crashdump_lib)

    add_subdirectory(make_lib/pmc_lib)

    ##build app libs
    if(CONFIG_WLAN)
        if(CONFIG_AS_INIC_AP)
            add_subdirectory(make/network/httpc)
            add_subdirectory(make/network/httpd)
            add_subdirectory(make/network/websocket)
            add_subdirectory(make/network/tftp)
            add_subdirectory(make/rtk_coex/rtk_coex_api)
        elseif(CONFIG_AS_INIC_NP)
            if(NOT CONFIG_MP_SHRINK)
                add_subdirectory(make/target/wifi_fw)
            endif()
            add_subdirectory(make/rtk_coex)
        elseif(CONFIG_SINGLE_CORE_WIFI)
            add_subdirectory(make/network/httpc)
            add_subdirectory(make/network/httpd)
            add_subdirectory(make/network/websocket)
            add_subdirectory(make/network/tftp)
            add_subdirectory(make/rtk_coex)
            add_subdirectory(make/rtk_coex/rtk_coex_api)
        endif()
    endif()

    if(CONFIG_BT)
        if(CONFIG_BT_AUDIO_CODEC_SUPPORT)
            add_subdirectory(make/bluetooth/bt_audio/bt_codec)
            add_subdirectory(make/bluetooth/bt_audio/audio_sync)
        endif()
    endif()

    add_subdirectory(make/audio)
    add_subdirectory(make/utils/ipc/rpc/framework/libs)
endif()
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
    COMMAND ${CMAKE_COMMAND} -DFILEPATH=${FLOADER_BIN_DIR}/imgtool_flashloader_raw.bin -P${CMAKE_FILES_DIR}/checkexists.cmake
    COMMAND ${CMAKE_COMMAND} -E copy ${FLOADER_BIN_DIR}/imgtool_flashloader_raw.bin ${IMAGE_TARGET_FOLDER}/ram_1_prepend.bin
    COMMAND ${IMAGETOOL} ${IMAGE_TARGET_FOLDER}/ram_1_prepend.bin ${BUILD_TYPE}
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/imgtool_flashloader.bin ${FLOADER_BIN_DIR}/floader_amebalite.bin
)

add_custom_target(
    ${PROJECT_NAME}
    DEPENDS target_bootloader target_img2_${PROJECT_NAME} target_img2_${PROJECT_NAME}_postbuild
)

if(TARGET target_img3)
    add_dependencies(${PROJECT_NAME} target_img3)
endif()
else() # For temporary compatibility.NOTE:  script below is new version cmake
set(CMAKE_EXECUTABLE_SUFFIX .axf)

add_compile_options(-fdata-sections -ffunction-sections)

set(c_LAYOUT_LD ${c_SOC_PROJECT_DIR}/amebalite_layout.ld)
set(c_IMG1_LD ${c_SDK_LD_DIR}/ameba_img1_all.ld)
if(CONFIG_MP_SHRINK)
    set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all_mp.ld)
else()
    set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all.ld)
endif()
set(c_IMG3_LD  ${c_SDK_LD_DIR}/ameba_img3_all.ld)

set(c_ROM_LIB_DIR ${c_SDK_LIB_DIR}/amebalite_rom_acut)
if(CONFIG_LINK_ROM_SYMB)
    set(c_LINK_ROM_LIB)
    set(c_LINK_ROM_LIB_NS)
    set(c_LINK_ROM_SYMBOL_NS ${c_SDK_LD_DIR}/ameba_rom_symbol_acut.ld)
    set(c_LINK_ROM_SYMBOL_S  ${c_SDK_LD_DIR}/ameba_rom_symbol_acut_s.ld)
else()
    set(c_LINK_ROM_LIB ${c_ROM_LIB_DIR}/rom.a)
    set(c_LINK_ROM_LIB_NS ${c_ROM_LIB_DIR}/rom_ns.a)
    set(c_LINK_ROM_SYMBOL_NS ${c_SDK_LD_DIR}/ameba_rom_symbol_empty.ld)
    set(c_LINK_ROM_SYMBOL_S ${c_SDK_LD_DIR}/ameba_rom_symbol_empty.ld)
endif()

if(CONFIG_TRUSTZONE)
    set(c_LINK_ROM_SYMBOL ${c_LINK_ROM_SYMBOL_NS})
else()
    set(c_LINK_ROM_SYMBOL ${c_LINK_ROM_SYMBOL_S})
endif()

list(APPEND c_LINK_ROM_SYMBOL ${c_SDK_LD_DIR}/ameba_rom_symbol_stdlib.ld)


ameba_add_subdirectory(make/image1)
ameba_add_subdirectory(make/image2)
ameba_add_subdirectory(make/image_floader)
ameba_add_subdirectory_ifnot(CONFIG_AMEBA_RLS make/image_imgtool_floader)

add_custom_target(
    ${c_MCU_PROJECT_NAME}
    COMMENT "build ${c_MCU_PROJECT_NAME} only"
    DEPENDS
        $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image1>
        $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image2>
        $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image2>_postbuild
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
    COMMAND ${CMAKE_COMMAND} -DFILEPATH=${c_SDK_FLOADER_BIN_DIR}/imgtool_flashloader_raw.bin -P${c_CMAKE_FILES_DIR}/checkexists.cmake
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_FLOADER_BIN_DIR}/imgtool_flashloader_raw.bin ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin
    COMMAND ${IMAGETOOL} ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin ${BUILD_TYPE}
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/imgtool_flashloader.bin ${c_SDK_FLOADER_BIN_DIR}/floader_amebalite.bin
)

endif()# For temporary compatibility.
