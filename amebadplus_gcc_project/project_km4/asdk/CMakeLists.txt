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
set(FLOADER_BIN_DIR ${UTILITYDIR}/image_tool_flashloader/amebadplus_acut)

if(CONFIG_AMEBADPLUS_A_CUT)
    set(ROM_LIB_DIR  ${PROJECTDIR}/asdk/lib/amebadplus_rom_acut)
elseif(CONFIG_AMEBADPLUS_B_CUT)
    set(ROM_LIB_DIR ${PROJECTDIR}/asdk/lib/amebadplus_rom_bcut)
else()

endif()

if(CONFIG_LINK_ROM_SYMB) 
    set(LINK_ROM_LIB)
    set(LINK_ROM_LIB_NS)
    set(LINK_ROM_SYMBOL_NS ${LDDIR}/ameba_rom_symbol_acut.ld)
    set(LINK_ROM_SYMBOL_S  ${LDDIR}/ameba_rom_symbol_acut_s.ld)
else()
    set(LINK_ROM_LIB ${ROM_LIB_DIR}/rom.a)
    set(LINK_ROM_LIB_NS ${ROM_LIB_DIR}/rom_ns.a)
    set(LINK_ROM_SYMBOL_NS ${LDDIR}/ameba_rom_symbol_empty.ld)
    set(LINK_ROM_SYMBOL_S ${LDDIR}/ameba_rom_symbol_empty.ld)
endif()

list(
    APPEND LINK_ROM_LIB
    ${TARGET_LIBSOC_FOLDER}/lib_chipinfo.a
)

list(
    APPEND LINK_ROM_LIB_NS
    ${TARGET_LIBSOC_FOLDER}/lib_chipinfo.a
)

set(LINK_ROM_SYMBOL_STDLIB ${LDDIR}/ameba_rom_symbol_stdlib.ld)

################################# BUILD ROM   ##############################

#TODO

############################################################################

add_compile_options(-fdata-sections  -ffunction-sections)

############################  BUILD BOOTLOADER #############################

add_subdirectory(make/bootloader)

########################### BUILD APPLICATION ##############################

add_subdirectory(make)

add_subdirectory(make_secure)
########################### BUILD SOC LIBS #################################

add_subdirectory(make_lib/bootloader_lib)

add_subdirectory(make_lib/chipinfo_lib)

add_subdirectory(make_lib/crashdump_lib)

add_subdirectory(make_lib/pmc_lib)

############################################################################

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
############################################################################
add_custom_target(
    ${PROJECT_NAME} 
    DEPENDS target_loader_${PROJECT_NAME} target_img2_${PROJECT_NAME}
)
if(TARGET target_img3)
    add_dependencies(${PROJECT_NAME} target_img3)
endif()

if(TARGET target_tfm)
    add_dependencies(${PROJECT_NAME} target_tfm)
endif()