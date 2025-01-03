if (__INCLUDED_FLASHLOADER_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_FLASHLOADER_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_flashloader INTERFACE)

#--------------------------#
if(${d_PLATFORM_TYPE} STREQUAL "amebasmart")
    if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_flashloader p_SCOPE interface
            ${d_MCU_SDK_DIR}/flashloader/rtl_flash_download.c
            ${d_MCU_SDK_DIR}/flashloader/rtl_flash_download_entry.c
            ${d_MCU_SDK_DIR}/flashloader/rtl_spinand_download.c
        )
    endif()
endif()
#--------------------------#