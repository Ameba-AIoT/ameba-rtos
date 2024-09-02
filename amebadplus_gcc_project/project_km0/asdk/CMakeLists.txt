set(CMAKE_EXECUTABLE_SUFFIX .axf)

############################ COMPILE FLAGS ##############################
add_compile_options( 
	"$<$<COMPILE_LANGUAGE:C>:${GLOBAL_C_OPTIONS}>"
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
endif()

if(CONFIG_LINK_ROM_SYMB) 
    set(LINK_ROM_LIB)
    set(LINK_ROM_SYMBOL ${LDDIR}/ameba_rom_symbol_acut.ld)
else()
    set(LINK_ROM_LIB ${ROM_LIB_DIR}/rom.a)
    set(LINK_ROM_SYMBOL ${LDDIR}/ameba_rom_symbol_empty.ld)
endif()


list(APPEND LINK_ROM_SYMBOL ${LDDIR}/ameba_rom_symbol_stdlib.ld)

################################# BUILD ROM   ##############################

#TODO

############################################################################

add_compile_options(-fdata-sections  -ffunction-sections)

########################### BUILD APPLICATION ##############################

add_subdirectory(make)

########################### BUILD SOC LIBS #################################

add_subdirectory(make_lib/chipinfo_lib)

add_subdirectory(make_lib/crashdump_lib)

add_subdirectory(make_lib/pmc_lib)

############################################################################
add_custom_target(
    ${PROJECT_NAME} 
    DEPENDS target_img2_${PROJECT_NAME}
)
