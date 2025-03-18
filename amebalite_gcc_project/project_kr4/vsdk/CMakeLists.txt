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

########################### BUILD APPLICATION ##############################

add_subdirectory(make)

############################# BUILD LIBS ###################################
if(NOT CONFIG_AMEBA_RLS)
    ## build soc libs
    add_subdirectory(make_lib/chipinfo_lib)

    add_subdirectory(make_lib/crashdump_lib)

    add_subdirectory(make_lib/pmc_lib)

    #build app libs
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
            if(NOT CONFIG_MP_SHRINK)
	            add_subdirectory(make/target/wifi_fw)
            endif()
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

# add_custom_target(
#     gdb
#     COMMAND ${CMAKE_COMMAND} -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DGNU_SCRIPT=${GNU_SCRIPT} -DPROJECTDIR=${PROJECTDIR} -P ${GNU_SCRIPT}/gdbsetup.cmake
#     COMMAND ${CMAKE_GDB} -x ${GNU_SCRIPT}/rtl_gdb_flash_write.txt
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
# )

add_custom_target(
    ${PROJECT_NAME}
    DEPENDS target_bootloader target_img2_${PROJECT_NAME} target_img2_${PROJECT_NAME}_postbuild
)
else() # For temporary compatibility.NOTE:  script below is new version cmake
set(CMAKE_EXECUTABLE_SUFFIX .axf)

add_compile_options(-fdata-sections -ffunction-sections)

set(c_LAYOUT_LD ${c_SOC_PROJECT_DIR}/amebalite_layout.ld)

if(CONFIG_MP_SHRINK)
    set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all_mp.ld)
else()
    set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all.ld)
endif()

set(c_ROM_LIB_DIR ${c_SDK_LIB_DIR}/amebalite_rom_acut)
if(CONFIG_LINK_ROM_SYMB)
    set(c_LINK_ROM_LIB)
    set(c_LINK_ROM_SYMBOL ${c_SDK_LD_DIR}/ameba_rom_symbol_acut.ld)
else()
    set(c_LINK_ROM_LIB ${c_ROM_LIB_DIR}/rom.a)
    set(c_LINK_ROM_SYMBOL ${c_SDK_LD_DIR}/ameba_rom_symbol_empty.ld)
endif()

list(APPEND c_LINK_ROM_SYMBOL ${c_SDK_LD_DIR}/ameba_rom_symbol_stdlib.ld)

ameba_add_subdirectory(make/image2)

add_custom_target(
    ${c_MCU_PROJECT_NAME}
    COMMENT "build ${c_MCU_PROJECT_NAME} only"
    DEPENDS
        $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image1>
        $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image2>
        $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image2>_postbuild
)

endif()# For temporary compatibility.
