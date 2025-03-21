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

    add_subdirectory(make/audio)

    if(CONFIG_WLAN)
        if(CONFIG_AS_INIC_AP)
            add_subdirectory(make/network/httpc)
            add_subdirectory(make/network/httpd)
            add_subdirectory(make/network/websocket)
            add_subdirectory(make/network/ipnat)
            add_subdirectory(make/network/ipdnrd)
            add_subdirectory(make/network/tftp)
        elseif(CONFIG_AS_INIC_NP)
            add_subdirectory(make/rtk_coex)
        elseif(CONFIG_SINGLE_CORE_WIFI)
            add_subdirectory(make/network/httpc)
            add_subdirectory(make/network/httpd)
            add_subdirectory(make/network/websocket)
            add_subdirectory(make/network/ipnat)
            add_subdirectory(make/network/ipdnrd)
            add_subdirectory(make/network/tftp)
            add_subdirectory(make/rtk_coex)
        endif()
    endif()

    if(CONFIG_CORE_AS_AP OR ((NOT CONFIG_CORE_AS_NP) AND CONFIG_FULLMAC_DEV))
        add_subdirectory(make/rtk_coex/rtk_coex_api)
    endif()

    if(CONFIG_AUDIO_EN)
        add_subdirectory(make/cmsis-dsp)
        add_subdirectory(make/audio/ac3)
        add_subdirectory(make/audio/amr)
        add_subdirectory(make/audio/flac)
        add_subdirectory(make/audio/haac)
        add_subdirectory(make/audio/hmp3)
        add_subdirectory(make/audio/mp3)
        add_subdirectory(make/audio/gsm610)
        add_subdirectory(make/audio/ekho)
        add_subdirectory(make/audio/libogg-1.3.3)
        add_subdirectory(make/audio/opus-1.3.1)
        add_subdirectory(make/audio/opusfile-0.11)
        add_subdirectory(make/audio/libopusenc-0.2.1)
        add_subdirectory(make/audio/m4a_selfparse)
    endif()

    add_subdirectory(make/usb_otg/device/lib)

    if(CONFIG_WFA_SRC)
        add_subdirectory(make/application/wifi-audio-src)
    endif()

    if(CONFIG_WLAN)
        if(CONFIG_WIFI_FW_EN)
            add_subdirectory(${WIFIFWDIR}/amebadplus make/target/wifi_fw)
        endif()
    endif()

    add_subdirectory(make/ui/drivers)
endif()
############################### BUILD TFM ##################################
#TODO USE EXTERNAL_PROJECT
if(CONFIG_RDP_TFM)
    set(TOOLCHAINDIR /opt/rtk-toolchain)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/trusted-firmware-m)
    set(ENV{PATH} /opt/rtk-toolchain/asdk-10.3.1/linux/newlib/bin:$ENV{PATH})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -DTFM_PROFILE=profile_medium -DTFM_PLATFORM=realtek/amebadplus -DBL2=OFF ${BASEDIR}/component/soc/common/trusted-firmware-m
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/trusted-firmware-m
    )

    add_custom_target(
        target_tfm
        ALL
        COMMAND make install
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/trusted-firmware-m
    )

    add_dependencies(target_img2 target_tfm)

    add_custom_command(
        TARGET target_tfm POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/trusted-firmware-m/install/outputs ${IMAGE_TARGET_FOLDER}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/trusted-firmware-m/install/interface/lib/s_veneers.o ${IMAGE_TARGET_FOLDER}
    )
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
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/imgtool_flashloader.bin ${FLOADER_BIN_DIR}/floader_amebadplus.bin
)

add_custom_target(
    ${PROJECT_NAME}
    DEPENDS target_bootloader target_img2_${PROJECT_NAME}
)

if(TARGET target_img3)
    add_dependencies(${PROJECT_NAME} target_img3)
endif()

if(TARGET target_tfm)
    add_dependencies(${PROJECT_NAME} target_tfm)
endif()
else() # For temporary compatibility.NOTE:  script below is new version cmake
set(CMAKE_EXECUTABLE_SUFFIX .axf)

add_compile_options(-fdata-sections -ffunction-sections)

set(c_LAYOUT_LD ${c_SOC_PROJECT_DIR}/amebaDplus_layout.ld)
if(CONFIG_MP_SHRINK)
    set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all_mp.ld)
else()
    set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all.ld)
endif()
set(c_IMG1_LD ${c_SDK_LD_DIR}/ameba_img1_all.ld)

if(CONFIG_AMEBADPLUS_A_CUT)
    set(c_ROM_LIB_DIR ${c_SDK_LIB_DIR}/amebadplus_rom_acut)
elseif(CONFIG_AMEBADPLUS_B_CUT)
    set(c_ROM_LIB_DIR ${c_SDK_LIB_DIR}/amebadplus_rom_bcut)
endif()

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
    COMMAND ${CMAKE_COMMAND} -DFILEPATH=${c_SDK_FLOADER_BIN_DIR}/imgtool_flashloader_raw.bin -P${c_CMAKE_FILES_DIR}/checkexists.cmake
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_FLOADER_BIN_DIR}/imgtool_flashloader_raw.bin ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin
    COMMAND ${IMAGETOOL} ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin ${BUILD_TYPE}
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/imgtool_flashloader.bin ${c_SDK_FLOADER_BIN_DIR}/floader_amebadplus.bin
)
endif()# For temporary compatibility.
