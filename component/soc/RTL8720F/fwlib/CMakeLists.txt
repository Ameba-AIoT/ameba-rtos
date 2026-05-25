ameba_global_include(
    include
    include/rom
    ${c_COMPONENT_DIR}/soc/common/include
    ${c_COMPONENT_DIR}/soc/common/include/cmsis
)

ameba_add_subdirectory(ram_common)
ameba_add_subdirectory(${c_COMPONENT_DIR}/soc/common/rtk_assert)

if("${c_MCU_PROJECT_NAME}" STREQUAL "km4tz")
    ameba_add_subdirectory(ram_km4tz)
elseif("${c_MCU_PROJECT_NAME}" STREQUAL "km4ns")
    ameba_add_subdirectory(ram_km4ns)
endif()
