if (__INCLUDED_RTK_COEX_API_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_RTK_COEX_API_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_rtk_coex_api INTERFACE)

#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_rtk_coex_api p_SCOPE interface
    p_INCLUDES
        ${c_COMPONENT_DIR}/os_dep
        ${c_COMPONENT_DIR}/rtk_coex
        ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_wl_chip
    p_SOURCES
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_ipc_host.c 
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_host_api_com.c 
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_host_api_bt.c 
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_host_api_ext.c 
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_host_api_wp.c
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_ipc_host_api.c 
)

#--------------------------#