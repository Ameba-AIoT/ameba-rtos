set(DIR ${WIFIFWDIR}/amebadplus/ram)

if(CONFIG_FW_DRIVER_COEXIST)
    if(CONFIG_MP_INCLUDED AND CONFIG_MP_EXPAND)
        set(TARGET_LIB wifi_fw_mp)
    else()
        set(TARGET_LIB wifi_fw)
    endif()
else()
    set(TARGET_LIB wifi_fw_singlecore)
endif()

set(
    CSRC
    ${DIR}/wififw_init.c
    ${DIR}/wififw_bcnmode.c
    ${DIR}/wififw_pwrctrl.c
    ${DIR}/wififw_csi.c
    ${DIR}/wififw_h2c_c2h.c
    ${DIR}/wififw_interrupt_ctrl.c
    ${DIR}/wififw_interrupt_handle.c
    ${DIR}/wififw_ps_bcnhdl.c
    ${DIR}/wififw_wowlan.c
    ${DIR}/wififw_ra_new.c
    ${DIR}/wififw_phy_reg_init.c
    ${DIR}/wififw_periodcca.c
    ${DIR}/wififw_deltatsft_patch.c
    ${DIR}/wififw_multi_rx_bcn.c
    ${DIR}/wififw_lpsofld.c
    ${DIR}/wififw_gtimer.c
    ${DIR}/wififw_btcoex.c
    ${DIR}/wififw_twt_timer.c
    ${DIR}/wififw_txrpt.c
    ${DIR}/wififw_mp_debug.c
)

include(${WIFIFWDIR}/common/wififw_common.cmake)

if(CONFIG_WIFI_FW_VERIFY)
    include(${WIFIFWDIR}/amebadplus/test/amebadplus_test.cmake)
endif()


##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above


ameba_list_append_if(CONFIG_WIFI_FW_EN public_libraries
    ${c_SDK_LIB_APPLICATION_DIR}/lib_${TARGET_LIB}.a
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

ameba_list_append(private_sources ${CSRC})

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
ameba_add_external_app_library(${TARGET_LIB} #target name, will be part of output library name: lib_cmsis_dsp.a
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################
