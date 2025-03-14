if(NOT CMAKE_REFACTOR) # For temporary compatibility.
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
    add_subdirectory(make_lib/chipinfo_lib)

    add_subdirectory(make_lib/crashdump_lib)

    add_subdirectory(make_lib/pmc_lib)

    if(CONFIG_WIFI_FW_EN)
        add_subdirectory(${WIFIFWDIR}/amebadplus make/target/wifi_fw)
    endif()

    if(CONFIG_WLAN)
        if(CONFIG_AS_INIC_NP)
            add_subdirectory(make/rtk_coex)
        endif()
    endif()

    if(CONFIG_CORE_AS_AP OR ((NOT CONFIG_CORE_AS_NP) AND CONFIG_FULLMAC_MENU))
        add_subdirectory(make/rtk_coex/rtk_coex_api)
    endif()
endif()

############################################################################
add_custom_target(
    ${PROJECT_NAME}
    DEPENDS target_img2_${PROJECT_NAME} target_img2_${PROJECT_NAME}_post_build
)
else() # For temporary compatibility.NOTE:  script below is new version cmake
set(CMAKE_EXECUTABLE_SUFFIX .axf)
add_compile_options(-fdata-sections -ffunction-sections)

set(c_LAYOUT_LD ${c_SOC_PROJECT_DIR}/amebaDplus_layout.ld)
if(CONFIG_MP_SHRINK)
    set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all_mp.ld)
else()
    set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all.ld)
endif()

if(CONFIG_AMEBADPLUS_A_CUT)
    ameba_set(c_ROM_LIB_DIR ${c_SDK_LIB_DIR}/amebadplus_rom_acut)
endif()

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
    DEPENDS $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image_list>
)
endif()# For temporary compatibility.