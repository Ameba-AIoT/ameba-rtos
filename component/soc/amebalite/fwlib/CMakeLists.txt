ameba_global_include(
    include
    include/cmsis
    ${c_COMPONENT_DIR}/soc/common/include
)

ameba_add_subdirectory(ram_common)
ameba_add_subdirectory(${c_COMPONENT_DIR}/soc/common/rtk_assert)

if("${c_MCU_PROJECT_NAME}" STREQUAL "km4")
    ameba_add_subdirectory(ram_km4)
elseif("${c_MCU_PROJECT_NAME}" STREQUAL "kr4")
    ameba_add_subdirectory(ram_kr4)
endif()
