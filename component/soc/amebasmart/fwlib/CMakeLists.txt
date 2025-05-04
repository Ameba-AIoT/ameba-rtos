
#REVIEW: how to add fwlib_secure.cmake(by check c_CURRENT_IMAGE is image3?)

ameba_global_include(
    include
    include/rom
    include/cmsis
    ../rom/include
    ${c_CMPT_SWLIB_DIR}
    ${c_COMPONENT_DIR}/soc/common/include
)

ameba_add_subdirectory(ram_common)
ameba_add_subdirectory(${c_COMPONENT_DIR}/soc/common/rtk_assert)

if("${c_MCU_PROJECT_NAME}" STREQUAL "lp")
    ameba_add_subdirectory(ram_lp)
elseif("${c_MCU_PROJECT_NAME}" STREQUAL "hp")
    ameba_add_subdirectory(ram_hp)
elseif("${c_MCU_PROJECT_NAME}" STREQUAL "ap")
    ameba_add_subdirectory(ram_hp)
    ameba_add_subdirectory(ap_core)
    ameba_add_subdirectory(ap_peripheral)
endif()
