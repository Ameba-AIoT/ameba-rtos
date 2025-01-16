set(AMEBA_GLOBAL_DEFINED TRUE)
include(${CMAKE_CURRENT_LIST_DIR}/utility.cmake)

############################## Global Definitions Can be Override ################################
# Definition starts with v_ meas overridable variable
# Call ameba_reset_global_define() after reset variable below

ameba_set_if_unset(v_ASDK_VER 10.3.1)
ameba_set_if_unset(v_VSDK_VER 10.3.1)

ameba_set_if_unset(v_FATFS_VER r0.14b)
ameba_set_if_unset(v_FREERTOS_VER v10.4.3)
ameba_set_if_unset(v_LITTLEFS_VER r2.50)
ameba_set_if_unset(v_LWIP_VER v2.1.2)
ameba_set_if_unset(v_MBEDTLS_VER 3.6.2)
ameba_set_if_unset(v_HTTP2_VER 1.31.0)

ameba_set_if_unset(v_GIC_VER 2)

############################## Global Definitions MUST NOT be Override Manually ################################
# Definition starts with c_ meas const

ameba_set_basedir(c_BASEDIR)
ameba_set(c_CMAKE_FILES_DIR ${CMAKE_CURRENT_LIST_DIR})
ameba_set(c_COMPONENT_DIR ${c_BASEDIR}/component)
ameba_set(c_WIFI_CMAKE_DIR ${c_BASEDIR}/component/wifi/wifi_make)
ameba_set(c_EMPTY_C_FILE ${c_COMPONENT_DIR}/build_info/empty_file.c)


ameba_set(c_CMPT_APPLICATION_DIR  ${c_COMPONENT_DIR}/application)
ameba_set(c_CMPT_AT_CMD_DIR       ${c_COMPONENT_DIR}/at_cmd)
ameba_set(c_CMPT_AUDIO_DIR        ${c_COMPONENT_DIR}/audio)
ameba_set(c_CMPT_BLUETOOTH_DIR    ${c_COMPONENT_DIR}/bluetooth)
ameba_set(c_CMPT_ETHERNET_DIR     ${c_COMPONENT_DIR}/ethernet)
ameba_set(c_CMPT_EXAMPLE_DIR      ${c_COMPONENT_DIR}/example)
ameba_set(c_CMPT_FILE_SYSTEM_DIR  ${c_COMPONENT_DIR}/file_system)
ameba_set(c_CMPT_LWIP_DIR         ${c_COMPONENT_DIR}/lwip)
ameba_set(c_CMPT_NETWORK_DIR      ${c_COMPONENT_DIR}/network)
ameba_set(c_CMPT_OS_DIR           ${c_COMPONENT_DIR}/os)
ameba_set(c_CMPT_USB_DIR          ${c_COMPONENT_DIR}/usb)
ameba_set(c_CMPT_UI_DIR           ${c_COMPONENT_DIR}/ui)
ameba_set(c_CMPT_WIFI_DIR         ${c_COMPONENT_DIR}/wifi)
ameba_set(c_CMPT_WPAN_DIR         ${c_COMPONENT_DIR}/wpan)

ameba_set(c_DYNAMIC_APP_DIR       ${c_COMPONENT_DIR}/dynamic_app_load)
ameba_set(c_FREERTOS_POSIX_DIR    ${c_COMPONENT_DIR}/os/freertos/freertos_posix)

ameba_set(c_FREERTOS_DIR_SUFFIX)
ameba_set(c_FREERTOS_DIR ${c_COMPONENT_DIR}/os/freertos/freertos_${v_FREERTOS_VER}${c_FREERTOS_DIR_SUFFIX}/Source)
ameba_set(c_MBEDTLS_DIR ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER})
ameba_set(c_WIFIFW_DIR ${c_COMPONENT_DIR}/wifi/wifi_fw)

############################################################

macro(ameba_reset_global_define)
    ameba_set(c_FREERTOS_DIR ${c_COMPONENT_DIR}/os/freertos/freertos_${v_FREERTOS_VER}${c_FREERTOS_DIR_SUFFIX}/Source)
    ameba_set(c_LWIP_DIR ${c_COMPONENT_DIR}/lwip/lwip_${v_LWIP_VER})
    ameba_set(c_MBEDTLS_DIR ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER})
endmacro()

############################################################

ameba_reset_global_define()

#NOTE: For compatibility with version CMAKE_V0.1, use variable starts with d_ or c_ above
############################################################
ameba_set(CMAKE_FILES_DIR ${c_CMAKE_FILES_DIR})
ameba_set(WIFIFWDIR ${c_WIFIFW_DIR})
ameba_set(BASEDIR ${c_BASEDIR})

############################################################


include(${CMAKE_CURRENT_LIST_DIR}/private_define.cmake)
