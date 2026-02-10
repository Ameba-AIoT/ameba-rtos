###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed

set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################

ameba_mcu_project_create(lp km0)
ameba_mcu_project_check()

###############################################################################
set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})
# ameba_add_subdirectories(${c_SDK_NAME})

###############################################################################
set(CMAKE_EXECUTABLE_SUFFIX .axf)

add_compile_options(-fdata-sections -ffunction-sections)

# note!!! Keep the latest version(lp:A) at the top for release reason.
ameba_set(c_ROM_LIB_DIR ${c_SDK_LIB_DIR}/${c_SOC_TYPE}_rom_acut)
ameba_set(c_FLOADER_BIN_DIR ${c_SDK_GNU_UTILITY_DIR}/image_tool_flashloader/${c_SOC_TYPE}_acut)

if(CONFIG_LINK_ROM_SYMB)
    ameba_unset(c_LINK_ROM_LIB)
    ameba_set(c_LINK_ROM_SYMBOL    ${c_SDK_LD_DIR}/ameba_rom_symbol_acut.ld)
else()
    ameba_set(c_LINK_ROM_LIB ${ROM_LIB_DIR}/rom.a)
    ameba_set(c_LINK_ROM_SYMBOL ${c_SDK_LD_DIR}/ameba_rom_symbol_empty.ld)
endif()

ameba_add_subdirectory(make/image2)

add_custom_target(
    ${c_MCU_PROJECT_NAME}
    COMMENT "build ${c_MCU_PROJECT_NAME} only"
    DEPENDS $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image_list>
)
