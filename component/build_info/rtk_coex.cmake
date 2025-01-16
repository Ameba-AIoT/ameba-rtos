if (__INCLUDED_RTK_COEX_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_RTK_COEX_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_rtk_coex INTERFACE)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_rtk_coex p_SCOPE interface
    ${c_COMPONENT_DIR}/os_dep
    ${c_COMPONENT_DIR}/rtk_coex
    ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_wl_chip
    ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_ext_chip
)

if(CONFIG_MP_SHRINK)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_rtk_coex p_SCOPE interface
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_mp_api.c
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_ipc_dev.c 
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_ipc_dev_api.c
    )
else()
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_rtk_coex p_SCOPE interface
        ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_wl_chip/rtw_coex_wl_chip.c
        ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_ext_chip/rtw_coex_ext_chip.c 
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_action.c
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_action_single_ant.c
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_action_dual_ant.c
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_action_dual_chip.c
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_api_wl.c
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_api_bt.c
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_api_ext.c   
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_api_wp.c 
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_dbg.c
        ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_ext_chip/rtw_coex_ext_chip_bt.c
        ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_ext_chip/rtw_coex_ext_chip_wpan.c
        ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_ext_chip/rtw_coex_ext_chip_debug.c
        ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_ext_chip/rtw_coex_ext_chip_btwpan.c
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_ipc_dev.c 
        ${c_COMPONENT_DIR}/rtk_coex/rtw_coex_ipc_dev_api.c 
    )
    if(${d_PLATFORM_TYPE} STREQUAL "amebadplus")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_rtk_coex p_SCOPE interface
            ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_wl_chip/rtw_coex_wl_chip_rtl8721da.c
            ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_ext_chip/rtw_coex_ext_chip_rtl8721da.c
        )
    elseif(CONFIG_AMEBASMART)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_rtk_coex p_SCOPE interface
            ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_wl_chip/rtw_coex_wl_chip_rtl8730e.c
            ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_ext_chip/rtw_coex_ext_chip_rtl8730e.c
        )
    elseif(${d_PLATFORM_TYPE} STREQUAL "amebalite")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_rtk_coex p_SCOPE interface
            ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_wl_chip/rtw_coex_wl_chip_rtl8720e.c
            ${c_COMPONENT_DIR}/rtk_coex/rtk_coex_ext_chip/rtw_coex_ext_chip_rtl8720e.c
        )
    elseif("${d_PLATFORM_TYPE}" STREQUAL "amebagreen2" OR "${d_PLATFORM_TYPE}" STREQUAL "amebapro3")
    else()

    endif()
endif()
#--------------------------#
