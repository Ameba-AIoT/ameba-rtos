if (__INCLUDED_WPAN_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_WPAN_BUILD_CMAKE__ TRUE)
endif()

if(NOT d_AMEBA_REALSE)
    include(${c_CMPT_WPAN_DIR}/make/platform/sw_radio.cmake)

    include(${c_CMPT_WPAN_DIR}/make/openthread/otbr.cmake)

    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_aps.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_comm_bdb.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_comm_legacy.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_comm_se.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_common.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_common_u.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_mac.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_nwk.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_secur.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_secur_zgpd.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_test_multitest.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_zcl.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_zdo.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_zgp.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_zgp_zgpd.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_zll.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_zse.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zcp_gppb_zc_test.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zcp_gppb_zr_test.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zcp_default_ed_test.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zcp_default_zc_zr_test.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/mac_test.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/regression_test.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_ed.cmake)
    include(${c_CMPT_WPAN_DIR}/make/zigbee/zboss_zc_zr.cmake)
endif()
add_library(src_${d_MCU_PROJECT_NAME}_wpan_platform INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wpan_ot INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wpan_zb INTERFACE)

#------------wpan_platform--------------#
if(CONFIG_802154_THREAD_EN OR CONFIG_802154_ZIGBEE_EN)
ameba_target_set(src_${d_MCU_PROJECT_NAME}_wpan_platform p_SCOPE interface
    p_INCLUDES
        ${d_SOC_PLATFORM_DIR}/fwlib/include
        ${c_CMPT_OS_DIR}/os_dep/
        ${c_CMPT_WPAN_DIR}/platform/timer
        ${c_CMPT_WPAN_DIR}/platform/flash
        ${c_CMPT_WPAN_DIR}/platform/config
    p_SOURCES
        ${c_CMPT_WPAN_DIR}/platform/timer/rtk_802154_timer.c
        ${c_CMPT_WPAN_DIR}/platform/timer/rtk_802154_timer_sched.c
        ${c_CMPT_WPAN_DIR}/platform/flash/rtk_802154_flash.c
)

ameba_target_set_if(CONFIG_802154_RADIO_EXT_RCP_RTK src_${d_MCU_PROJECT_NAME}_wpan_platform p_SCOPE interface
    p_INCLUDES
        ${c_CMPT_WPAN_DIR}/platform/rtk_rcp_cfu
    p_SOURCES
        ${c_CMPT_WPAN_DIR}/platform/rtk_rcp_cfu/rtk_cfu.c
        ${c_CMPT_WPAN_DIR}/platform/rtk_rcp_cfu/rtk_cfu_file_array.c
        ${c_CMPT_WPAN_DIR}/platform/rtk_rcp_cfu/rtk_cfu_file_fs.c
        ${c_CMPT_WPAN_DIR}/platform/rtk_rcp_cfu/rtk_cfu_fw_array.c
        ${c_CMPT_WPAN_DIR}/platform/rtk_rcp_cfu/rtk_cfu_fw_update_process.c
        ${c_CMPT_WPAN_DIR}/platform/rtk_rcp_cfu/rtk_cfu_fw_update_sub_process.c
        ${c_CMPT_WPAN_DIR}/platform/rtk_rcp_cfu/rtk_cfu_hdlc.c
        ${c_CMPT_WPAN_DIR}/platform/rtk_rcp_cfu/rtk_cfu_rcp_config.c
        ${c_CMPT_WPAN_DIR}/platform/rtk_rcp_cfu/rtk_cfu_uart.c
)
endif()
#------------wpan_platform--------------#

#------------wpan_ot--------------#
if(CONFIG_802154_THREAD_EN)
ameba_target_set(src_${d_MCU_PROJECT_NAME}_wpan_ot p_SCOPE interface
    p_COMPILE_DEFINES
        OPENTHREAD_CONFIG_CORE_USER_CONFIG_HEADER_ENABLE=1
    p_INCLUDES
        ${d_SOC_PLATFORM_DIR}/fwlib/include
        ${c_CMPT_OS_DIR}/os_dep/
        ${c_CMPT_WPAN_DIR}/platform/timer
        ${c_CMPT_WPAN_DIR}/platform/flash
        ${c_CMPT_WPAN_DIR}/platform/config
        ${c_CMPT_WPAN_DIR}/openthread/config
        ${c_CMPT_WPAN_DIR}/openthread/openthread/src
        ${c_CMPT_WPAN_DIR}/openthread/openthread/src/core
        ${c_CMPT_WPAN_DIR}/openthread/openthread/include
        ${c_CMPT_WPAN_DIR}/openthread/openthread/examples
        ${c_CMPT_WPAN_DIR}/openthread/openthread/examples/platforms
        ${c_CMPT_WPAN_DIR}/openthread/openthread/examples/platforms/utils
        ${c_CMPT_WPAN_DIR}/openthread/openthread/cli
    p_SOURCES
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/rtk_ot_alarm.c
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/rtk_ot_entropy.c
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/rtk_ot_flash.c
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/rtk_ot_logging.c
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/rtk_ot_misc.c
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/rtk_ot_system.c
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/rtk_ot_crypto.c
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/rtk_ot_uart.c
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/rtk_ot_settings.c
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/rtk_ot_multipan.c
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/rtk_ot_ble.c
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/rtk_ot_test.c
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/rtk_ot_cli_extension.c
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/rtk_ot_cli_uart.c
)

if(CONFIG_802154_RADIO_EXT_RCP_RTK OR CONFIG_802154_RADIO_EXT_RCP_OTHER)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_wpan_ot p_SCOPE interface
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/radio/rcp_radio/rtk_ot_radio.cpp
        ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/radio/rcp_radio/rtk_ot_hdlc_interface.cpp
    )
elseif(CONFIG_802154_RADIO_INT_SW)
    ameba_target_set(src_${d_MCU_PROJECT_NAME}_wpan_ot p_SCOPE interface
        p_INCLUDES
            ${c_CMPT_WPAN_DIR}/platform/sw_radio/mac
        p_SOURCES
            ${c_CMPT_WPAN_DIR}/openthread/rtk_ot_support/radio/rtk_ot_sw_radio.c
)
endif()
ameba_target_sources_if(CONFIG_802154_THREAD_RCP_EN src_${d_MCU_PROJECT_NAME}_wpan_ot p_SCOPE interface
    ${c_CMPT_WPAN_DIR}/openthread/openthread/examples/apps/ncp/ncp.c
)
endif()
#------------wpan_ot--------------#

#------------wpan_zb--------------#
if(CONFIG_802154_ZIGBEE_EN)
ameba_target_set(src_${d_MCU_PROJECT_NAME}_wpan_zb p_SCOPE interface
    p_COMPILE_OPTIONS
        -Wno-error
    p_COMPILE_DEFINES
        ZB_ERROR_PRINT_TO_LOG=1
        CONFIG_ZIGBEE_ERROR_TO_STRING_ENABLED=1
        LIBZBOSS_CONFIG_FILE=\"libzboss_config.h\"
    p_INCLUDES
        ${d_SOC_PLATFORM_DIR}/fwlib/include
        ${c_CMPT_OS_DIR}/os_dep/
        ${c_CMPT_WPAN_DIR}/platform/timer
        ${c_CMPT_WPAN_DIR}/platform/flash
        ${c_CMPT_WPAN_DIR}/platform/config
        ${c_CMPT_WPAN_DIR}/zigbee/include
        ${c_CMPT_WPAN_DIR}/zigbee/rtk_zb_support/config
        ${c_CMPT_WPAN_DIR}/zigbee/rtk_zb_support/include
        ${c_CMPT_WPAN_DIR}/zigbee/zboss_stable/include/	
        ${c_CMPT_WPAN_DIR}/zigbee/zboss_stable/include/ha/	
        ${c_CMPT_WPAN_DIR}/zigbee/zboss_stable/include/se/	
        ${c_CMPT_WPAN_DIR}/zigbee/zboss_stable/include/zcl/	
        ${c_CMPT_WPAN_DIR}/zigbee/zboss_stable/include/zgp/	
        ${c_CMPT_WPAN_DIR}/zigbee/zboss_stable/include/zgpd/	
        ${c_CMPT_WPAN_DIR}/zigbee/zboss_stable/include/zll/
        ${c_CMPT_WPAN_DIR}/platform/sw_radio/mac
    p_SOURCES
        ${c_CMPT_WPAN_DIR}/zigbee/api/zigbee_helpers.c
        ${c_CMPT_WPAN_DIR}/zigbee/api/zb_error_to_string.c
        ${c_CMPT_WPAN_DIR}/zigbee/rtk_zb_support/zb_osif_entry.c
        ${c_CMPT_WPAN_DIR}/zigbee/rtk_zb_support/zb_osif_common.c
        ${c_CMPT_WPAN_DIR}/zigbee/rtk_zb_support/zb_osif_logger.c
        ${c_CMPT_WPAN_DIR}/zigbee/rtk_zb_support/zb_osif_nvram.c
        ${c_CMPT_WPAN_DIR}/zigbee/rtk_zb_support/zb_osif_transceiver.c
        ${c_CMPT_WPAN_DIR}/zigbee/rtk_zb_support/zb_osif_transceiver_mem.c
        ${c_CMPT_WPAN_DIR}/zigbee/rtk_zb_support/zb_osif_timer.c
)
ameba_target_definitions_if(CONFIG_802154_ZIGBEE_ROLE_ZED src_${d_MCU_PROJECT_NAME}_wpan_zb p_SCOPE interface ZB_ED_ROLE=1)

ameba_target_set_if(CONFIG_802154_ZIGBEE_EXAMPLE_LIGHTCTL_B src_${d_MCU_PROJECT_NAME}_wpan_zb p_SCOPE interface
    p_INCLUDES
        ${c_CMPT_WPAN_DIR}/zigbee/example/light_control/light_bulb
    p_SOURCES
        ${c_CMPT_WPAN_DIR}/zigbee/example/light_control/light_bulb/example.c
)
ameba_target_set_if(CONFIG_802154_ZIGBEE_EXAMPLE_LIGHTCTL_S src_${d_MCU_PROJECT_NAME}_wpan_zb p_SCOPE interface
    p_INCLUDES
        ${c_CMPT_WPAN_DIR}/zigbee/example/light_control/light_switch
    p_SOURCES
        ${c_CMPT_WPAN_DIR}/zigbee/example/light_control/light_switch/example.c
)
ameba_target_set_if(CONFIG_802154_ZIGBEE_EXAMPLE_LIGHTCTL_C src_${d_MCU_PROJECT_NAME}_wpan_zb p_SCOPE interface
    p_INCLUDES
        ${c_CMPT_WPAN_DIR}/zigbee/example/light_control/light_coordinator
    p_SOURCES
        ${c_CMPT_WPAN_DIR}/zigbee/example/light_control/light_coordinator/example.c
)
ameba_target_set_if(CONFIG_802154_ZIGBEE_EXAMPLE_ONOFFLIGHT_L src_${d_MCU_PROJECT_NAME}_wpan_zb p_SCOPE interface
    p_INCLUDES
        ${c_CMPT_WPAN_DIR}/zigbee/example/ha_on_off_light_n_switch/on_off_light
        ${c_CMPT_WPAN_DIR}/zigbee/example/common/light_driver/
        ${c_CMPT_WPAN_DIR}/zigbee/example/common/switch_driver/
    p_SOURCES
        ${c_CMPT_WPAN_DIR}/zigbee/example/ha_on_off_light_n_switch/on_off_light/example.c
        ${c_CMPT_WPAN_DIR}/zigbee/example/common/light_driver/light_driver.c
        ${c_CMPT_WPAN_DIR}/zigbee/example/common/switch_driver/switch_driver.c
)
ameba_target_set_if(CONFIG_802154_ZIGBEE_EXAMPLE_ONOFFLIGHT_S src_${d_MCU_PROJECT_NAME}_wpan_zb p_SCOPE interface
    p_INCLUDES
        ${c_CMPT_WPAN_DIR}/zigbee/example/ha_on_off_light_n_switch/on_off_light_switch
        ${c_CMPT_WPAN_DIR}/zigbee/example/common/light_driver/
        ${c_CMPT_WPAN_DIR}/zigbee/example/common/switch_driver/
    p_SOURCES
        ${c_CMPT_WPAN_DIR}/zigbee/example/ha_on_off_light_n_switch/on_off_light_switch/example.c
        ${c_CMPT_WPAN_DIR}/zigbee/example/common/light_driver/light_driver.c
        ${c_CMPT_WPAN_DIR}/zigbee/example/common/switch_driver/switch_driver.c
)
ameba_target_set_if(CONFIG_802154_ZIGBEE_EXAMPLE_CUSTOM_CLUSTER_ZC src_${d_MCU_PROJECT_NAME}_wpan_zb p_SCOPE interface
    p_INCLUDES
        ${c_CMPT_WPAN_DIR}/zigbee/example/ha_custom_cluster/custom_cluster_zc
    p_SOURCES
        ${c_CMPT_WPAN_DIR}/zigbee/example/ha_custom_cluster/custom_cluster_zc/example.c
)
ameba_target_set_if(CONFIG_802154_ZIGBEE_EXAMPLE_CUSTOM_CLUSTER_ZR src_${d_MCU_PROJECT_NAME}_wpan_zb p_SCOPE interface
    p_INCLUDES
        ${c_CMPT_WPAN_DIR}/zigbee/example/ha_custom_cluster/custom_cluster_zr
    p_SOURCES
        ${c_CMPT_WPAN_DIR}/zigbee/example/ha_custom_cluster/custom_cluster_zr/example.c
)
endif()
#------------wpan_zb--------------#
