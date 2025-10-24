ameba_global_include(
    include
    include/rom
    ${c_COMPONENT_DIR}/soc/common/include
    ${c_COMPONENT_DIR}/soc/common/include/cmsis
)

ameba_add_subdirectory(ram_common)
ameba_add_subdirectory(${c_COMPONENT_DIR}/soc/common/rtk_assert)

if("${c_MCU_PROJECT_NAME}" STREQUAL "km4ns")
    ameba_add_subdirectory(ram_km4ns)
elseif("${c_MCU_PROJECT_NAME}" STREQUAL "km4tz")
    ameba_add_subdirectory(ram_km4tz)

    if(${c_CURRENT_IMAGE_TYPE} STREQUAL "image2")
        ameba_add_subdirectory(jpeg_decoder)
    endif()
endif()
