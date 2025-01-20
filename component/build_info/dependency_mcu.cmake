if (__INCLUDED_DEPENDENCY_MCU_CMAKE__)
    return()
else()
    set(__INCLUDED_DEPENDENCY_MCU_CMAKE__ TRUE)
endif()

if (NOT d_PLATFORM_TYPE OR NOT d_MCU_TYPE)
    ameba_fatal("Make sure ameba_platform_project_create and ameba_mcu_project_create are called before include this cmake file")
endif()

add_library(dep_${d_MCU_PROJECT_NAME}_bluetooth INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_bootloader INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_chipinfo INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_crashdump INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_freertos INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_fwlib INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_pmc INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_user_project INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_vmin_test INTERFACE)

include(${CMAKE_CURRENT_LIST_DIR}/chipinfo.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/crashdump.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/bootloader.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/bluetooth.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/freertos.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/fwlib.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/flashloader.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/pmc.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/vmin_test.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dynamic_app_load.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/freertos_posix.cmake)

if(NOT d_AMEBA_RELEASE)
    include(${CMAKE_CURRENT_LIST_DIR}/imgtool_floader.cmake)
endif()

############################ dep_bluetooth define #################################

ameba_target_include(dep_${d_MCU_PROJECT_NAME}_bluetooth p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/rtk_coex
    ${c_CMPT_BLUETOOTH_DIR}/driver
    ${c_CMPT_BLUETOOTH_DIR}/driver/hci
    ${c_CMPT_BLUETOOTH_DIR}/driver/inc
    ${c_CMPT_BLUETOOTH_DIR}/driver/inc/hci
    ${c_CMPT_BLUETOOTH_DIR}/driver/platform/${d_PLATFORM_TYPE}/inc
    ${c_CMPT_BLUETOOTH_DIR}/osif
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec
    ${c_CMPT_BLUETOOTH_DIR}/bt_mp
)
if (CONFIG_BT_ZEPHYR)
    ameba_target_include(dep_${d_MCU_PROJECT_NAME}_bluetooth p_SCOPE interface
        ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack
        ${c_CMPT_BLUETOOTH_DIR}/zephyr
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/hci_driver
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/include
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/settings
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/port
    )
else()
    ameba_target_include(dep_${d_MCU_PROJECT_NAME}_bluetooth p_SCOPE interface
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/example
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/app
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/os
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/platform
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/stack
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/gap
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/profile
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/profile/client
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/profile/server
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/leaudio
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/ble_mgr
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/framework/bt_mgr_inc
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/framework/sysm_inc
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/common
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/ble/gatt_client
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/ble/vector
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/cmd
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/common
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/gap
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/inc
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/inc/${d_PLATFORM_TYPE}
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/ali
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/realtek
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/platform
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/profile
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/utility
    )
    ameba_target_include_if(CONFIG_BT_BLE_ONLY dep_${d_MCU_PROJECT_NAME}_bluetooth p_SCOPE interface
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${d_PLATFORM_TYPE}/lib/${d_MCU_TYPE}/ble_only
    )

    ameba_target_include_if(CONFIG_BT_DUAL_MODE dep_${d_MCU_PROJECT_NAME}_bluetooth p_SCOPE interface
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${d_PLATFORM_TYPE}/lib/${d_MCU_TYPE}/dual_mode
    )
endif()

ameba_target_include(dep_${d_MCU_PROJECT_NAME}_bluetooth p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/api
    ${c_CMPT_BLUETOOTH_DIR}/api/include
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_mesh_ota_common
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client
)
if (NOT CONFIG_BT_INIC)
    ameba_target_link_if(CONFIG_BT_AUDIO_CODEC_SUPPORT dep_${d_MCU_PROJECT_NAME}_bluetooth p_SCOPE interface
        "${d_SDK_LIB_APPLICATION_DIR}/lib_btcodec.a"
        "${d_SDK_LIB_APPLICATION_DIR}/lib_btaudiosync.a"
    )
    if (NOT CONFIG_BT_ZEPHYR)
        ameba_target_link_if(CONFIG_BT_BLE_ONLY dep_${d_MCU_PROJECT_NAME}_bluetooth p_SCOPE interface
            ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${d_PLATFORM_TYPE}/lib/${d_MCU_TYPE}/ble_only/btgap.a
        )

        ameba_target_link_if(CONFIG_BT_DUAL_MODE dep_${d_MCU_PROJECT_NAME}_bluetooth p_SCOPE interface
            ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${d_PLATFORM_TYPE}/lib/${d_MCU_TYPE}/dual_mode/btgap.a
            ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${d_PLATFORM_TYPE}/lib/${d_MCU_TYPE}/bt_mgr.a
        )

        ameba_target_link(dep_${d_MCU_PROJECT_NAME}_bluetooth p_SCOPE interface
            ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${d_PLATFORM_TYPE}/lib/${d_MCU_TYPE}/leaudio.a
            ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${d_PLATFORM_TYPE}/lib/${d_MCU_TYPE}/ble_mgr.a
            ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${d_PLATFORM_TYPE}/lib/${d_MCU_TYPE}/sysm.a
        )
        if(CONFIG_BT_MESH_PROVISIONER_SUPPORT OR CONFIG_BT_MESH_DEVICE_SUPPORT)
            ameba_target_link(dep_${d_MCU_PROJECT_NAME}_bluetooth p_SCOPE interface
                ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/lib/${d_PLATFORM_TYPE}/${d_MCU_TYPE}/btmesh.a
            )
        endif()
    endif()
endif()
#--------------------------#

########################## dep_bluetooth define end #############################

########################### dep_bootloader define ###############################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_bootloader p_SCOPE interface ${d_SOC_PLATFORM_DIR}/bootloader)
ameba_target_link(dep_${d_MCU_PROJECT_NAME}_bootloader p_SCOPE interface ${d_SDK_LIB_SOC_DIR}/lib_bootloader.a)
######################## dep_bootloader define end ##############################

############################# dep_chipinfo define ###############################
ameba_target_link(dep_${d_MCU_PROJECT_NAME}_chipinfo p_SCOPE interface p_WHOLE_ARCHIVE ${d_SDK_LIB_SOC_DIR}/lib_chipinfo.a)
######################### dep_chipinfo define end ##############################

############################ dep_crashdump define ###############################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_crashdump p_SCOPE interface ${c_COMPONENT_DIR}/soc/common/crashdump/include)
ameba_target_link(dep_${d_MCU_PROJECT_NAME}_crashdump p_SCOPE interface p_WHOLE_ARCHIVE ${d_SDK_LIB_SOC_DIR}/lib_crashdump.a)
######################## dep_crashdump define end ##############################

###################################### dep_freertos begin #############################################

ameba_target_include(dep_${d_MCU_PROJECT_NAME}_freertos p_SCOPE interface
    ${c_FREERTOS_DIR}/include
    ${c_CMPT_OS_DIR}/freertos/os_wrapper/include
)
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        if(CONFIG_MP_SHRINK)
            if("${CONFIG_CORE_NUM}" EQUAL 1)
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_freertos p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_SOC_DIR}/lib_freertos_smp_sc_mp.a
                )
            else()
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_freertos p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_SOC_DIR}/lib_freertos_smp_mp.a
                )
            endif()
        else()
            if("${CONFIG_CORE_NUM}" EQUAL 1)
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_freertos p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_SOC_DIR}/lib_freertos_smp_sc.a
                )
            else()
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_freertos p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_SOC_DIR}/lib_freertos_smp.a
                )
            endif()
        endif()
    endif()
endif()
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_include(dep_${d_MCU_PROJECT_NAME}_freertos p_SCOPE interface
            ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/non_secure
            ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/secure
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "lp")
        ameba_target_include(dep_${d_MCU_PROJECT_NAME}_freertos p_SCOPE interface
            ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_LP
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_include(dep_${d_MCU_PROJECT_NAME}_freertos p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core
            ${c_FREERTOS_DIR}/portable/GCC/ARM_CA7
        )
    else()
    endif()
endif()
####################################### dep_freertos end ##############################################

###################################### dep_fwlib begin ############################################
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    ameba_target_include(dep_${d_MCU_PROJECT_NAME}_fwlib p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/fwlib/include
        ${d_SOC_PLATFORM_DIR}/swlib
        ${c_COMPONENT_DIR}/soc/common/include
    )

    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_include(dep_${d_MCU_PROJECT_NAME}_fwlib p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core
        )
    endif()
endif()
####################################### dep_fwlib end ##############################################

############################## dep_pmc define ###################################
ameba_target_link(dep_${d_MCU_PROJECT_NAME}_pmc p_SCOPE interface p_WHOLE_ARCHIVE ${d_SDK_LIB_SOC_DIR}/lib_pmc.a)
############################ dep_pmc define end #################################

############################## dep_vmin_test define ###################################
ameba_target_link(dep_${d_MCU_PROJECT_NAME}_vmin_test p_SCOPE interface p_WHOLE_ARCHIVE ${d_SDK_LIB_SOC_DIR}/lib_Vmin_Test.a)
############################ dep_vmin_test define end #################################
