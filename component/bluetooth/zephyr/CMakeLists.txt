##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    include
    port
)

ameba_list_append(public_libraries
    ${c_SDK_LIB_APPLICATION_DIR}/lib_bt_stack.a
)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

execute_process(
    COMMAND git log -1 --format=%H
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_REPO_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(VERSION_HEADER "${CMAKE_CURRENT_LIST_DIR}/stack/common/stack_version.h")

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/stack/common/stack_version.h.in"
    "${VERSION_HEADER}"
    @ONLY
)

ameba_list_append(private_sources
    hci_driver/buf.c

    stack/common/addr.c
    stack/common/rpa.c
    stack/common/bt_str.c

    stack/host/addr.c
    stack/host/buf.c
    stack/host/hci_core.c
    stack/host/id.c
    stack/host/crypto_tc.c
    stack/host/conn.c
    stack/host/scan.c
    stack/host/adv.c
    stack/host/l2cap.c
    stack/host/smp.c
    stack/host/att.c
    stack/host/gatt.c
    stack/host/ecc.c
    stack/host/keys.c
    stack/host/uuid.c
    stack/host/hci_ecc.c
    stack/host/long_wq.c
    stack/host/settings.c
    stack/host/aes_ccm.c
    stack/host/direction.c
)
ameba_list_append_if(CONFIG_BT_DUAL_MODE private_sources
    stack/host/classic/br.c
    stack/host/classic/sco.c
    stack/host/classic/sdp.c
    stack/host/classic/ssp.c
    stack/host/classic/a2dp.c
    stack/host/classic/avdtp.c
    stack/host/classic/keys_br.c
    stack/host/classic/l2cap_br.c
    stack_ext/avctp.c
    stack_ext/avrcp.c
    stack_ext/spp.c
    stack_ext/goep.c
    stack_ext/opp.c
    stack_ext/pbap.c
    stack/host/classic/rfcomm.c
    stack/host/classic/hfp_hf.c
    stack/host/classic/hfp_ag.c
    stack/host/classic/at.c
)

if (CONFIG_BT_MESH_PROVISIONER_SUPPORT OR CONFIG_BT_MESH_DEVICE_SUPPORT)
    ameba_list_append(private_sources
        stack/mesh/access.c
        stack/mesh/adv.c
        stack/mesh/adv_ext.c
        stack/mesh/adv_legacy.c
        stack/mesh/app_keys.c
        stack/mesh/beacon.c
        stack/mesh/blob_cli.c
        stack/mesh/blob_srv.c
        stack/mesh/cdb.c
        stack/mesh/cfg.c
        stack/mesh/cfg_cli.c
        stack/mesh/cfg_srv.c
        stack/mesh/crypto.c
        stack/mesh/crypto_tc.c
        stack/mesh/delayable_msg.c
        stack/mesh/dfu_cli.c
        stack/mesh/dfu_slot.c
        stack/mesh/dfu_srv.c
        stack/mesh/friend.c
        stack/mesh/gatt_cli.c
        stack/mesh/health_cli.c
        stack/mesh/health_srv.c
        stack/mesh/heartbeat.c
        stack/mesh/main.c
        stack/mesh/msg.c
        stack/mesh/net.c
        stack/mesh/pb_adv.c
        stack/mesh/pb_gatt.c
        stack/mesh/pb_gatt_cli.c
        stack/mesh/pb_gatt_srv.c
        stack/mesh/proxy_cli.c
        stack/mesh/proxy_srv.c
        stack/mesh/proxy_msg.c
        stack/mesh/prov.c
        stack/mesh/provisionee.c
        stack/mesh/provisioner.c
        stack/mesh/rpl.c
        stack/mesh/settings.c
        stack/mesh/subnet.c
        stack/mesh/transport.c
        stack/mesh/va.c
        stack/mesh/rpr_cli.c
        stack/mesh/rpr_srv.c
        stack/mesh/priv_beacon_cli.c
        stack/mesh/priv_beacon_srv.c
    )
endif()
ameba_list_append_if(CONFIG_BT_MESH_DEVICE_SUPPORT private_sources
    stack/mesh/lpn.c
)

ameba_list_append(private_sources
    stack/crypto/bt_crypto.c
    stack/crypto/bt_crypto_tc.c

    tinycrypt/cmac_mode.c
    tinycrypt/aes_encrypt.c
    tinycrypt/utils.c
    tinycrypt/ecc.c
    tinycrypt/ecc_dh.c

#    port/zephyr_log.c
    port/zephyr_queue.c
    port/zephyr_time.c
    port/zephyr_thread.c
    port/zephyr_work.c
    port/zephyr_sema.c
    port/zephyr_mutex.c
    port/zephyr_poll.c
    port/zephyr_mem.c
    port/zephyr_flash.c
    port/zephyr_sys.c

    settings/settings.c
    settings/settings_store.c
    settings/settings_line.c
    settings/settings_init.c
    # settings/settings_nvs.c
    # settings/nvs.c

#    settings/settings_ftl.c
    settings/settings_kv.c
    util/dec.c
    util/hex.c
    util/crc8_sw.c
)

ameba_list_append_if(CONFIG_BT_SDN private_sources hci_driver/hci_h4.c)

ameba_list_append(private_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    include
    include/syscalls
    include/zephyr/sys
    include/zephyr/tracing
    port
    stack
    stack/host
    ${c_CMPT_BLUETOOTH_DIR}/driver/hci
)

ameba_list_append_if(CONFIG_BT_DUAL_MODE private_includes
)

ameba_list_append(private_compile_options
    -Wno-error
#     -O0
)

foreach(src ${private_sources})
    cmake_path(GET src STEM filez)
    cmake_path(GET src FILENAME filename)
    set(MODULE_C_DEFINES __FILE_Z__=${filez} __FILE_Z_STR__=\"${filename}\")
    set_source_files_properties(${src} PROPERTIES COMPILE_DEFINITIONS "${MODULE_C_DEFINES}")
endforeach()

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_external_app_library(bt_stack
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
    p_REMOVE_INCLUDES_RECURSIVE
        ${c_BASEDIR}/component/lwip
)

ameba_add_internal_library(zephyr_log
    p_SOURCES
        zephyr_log.c
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################