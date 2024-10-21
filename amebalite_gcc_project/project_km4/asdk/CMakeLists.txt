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

if(NOT AMEBA_RLS)
    add_subdirectory(make/RT_xmodem)
endif()

########################### BUILD APPLICATION ##############################

add_subdirectory(make)

add_subdirectory(make_secure)
############################# BUILD LIBS ###################################
if(NOT AMEBA_RLS)
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
        endif()
    endif()

    if(CONFIG_BT)
        if(CONFIG_BT_AUDIO_CODEC_SUPPORT)
            add_subdirectory(make/bluetooth/bt_audio/bt_codec)
            add_subdirectory(make/bluetooth/bt_audio/audio_sync)
        endif()
    endif()

    if(CONFIG_802154_ZIGBEE_EN)
        add_subdirectory(make/wpan/zigbee)
    endif()

    add_subdirectory(make/audio)
    add_subdirectory(make/utils/ipc/rpc/framework/libs)
endif()
##########################  CUSTOM TARGET  ###################################

add_custom_target(
    gdb
    COMMAND ${CMAKE_COMMAND} -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DGNU_SCRIPT=${GNU_SCRIPT} -DPROJECTDIR=${PROJECTDIR} -P ${GNU_SCRIPT}/gdbsetup.cmake
    COMMAND ${CMAKE_GDB} -x ${GNU_SCRIPT}/rtl_gdb_flash_write.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
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
    DEPENDS target_bootloader target_img2_${PROJECT_NAME}
)

if(TARGET target_img3)
    add_dependencies(${PROJECT_NAME} target_img3)
endif()
