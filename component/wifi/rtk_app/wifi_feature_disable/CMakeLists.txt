##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
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
if(NOT CONFIG_AS_INIC_AP)
    ameba_list_append_ifnot(CONFIG_WIFI_CSI_ENABLE private_sources
        wifi_feature_dis_csi.c)

    ameba_list_append_ifnot(CONFIG_WIFI_RADAR_ENABLE private_sources
        wifi_feature_dis_radar.c)

    ameba_list_append_ifnot(CONFIG_WIFI_MCC_ENABLE private_sources
        wifi_feature_dis_mcc.c)

    ameba_list_append_ifnot(CONFIG_WIFI_ANTDIV_ENABLE private_sources
        wifi_feature_dis_antdiv.c)

    ameba_list_append_ifnot(CONFIG_WIFI_SPEAKER_ENABLE private_sources
        wifi_feature_dis_speaker.c)

    ameba_list_append_ifnot(CONFIG_WIFI_P2P_ENABLE private_sources
        wifi_feature_dis_p2p.c)

endif()

ameba_list_append_ifnot(CONFIG_WIFI_EAP_ENABLE private_sources
    wifi_feature_dis_eap.c)

if(NOT (CONFIG_WIFI_11K_ENABLE AND CONFIG_WIFI_11V_ENABLE AND CONFIG_WIFI_11R_ENABLE))
    ameba_list_append(private_sources
        wifi_feature_dis_kvr.c
    )
endif()

if(CONFIG_AS_INIC_NP)
    if(NOT CONFIG_AMEBALITE)
        if(NOT CONFIG_WHC_INTF_SDIO)
            if(NOT CONFIG_WHC_INTF_SPI)
                if(NOT CONFIG_WHC_INTF_USB)
                    if(NOT CONFIG_ZEPHYR_SDK)
                        if(TARGET target_wifi_nouse)
                            add_custom_command(
                                OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/wifi_intf_drv_to_app_ext_noused.c
                                COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/gen_noused_c.py -a ${c_SDK_IMAGE_TARGET_DIR}/target_img2_otrcore.asm -c ${CMAKE_CURRENT_SOURCE_DIR}/wifi_intf_drv_to_app_ext_strong.c -r ${CMAKE_CURRENT_BINARY_DIR}/wifi_intf_drv_to_app_ext_noused.c
                                # COMMAND ${CMAKE_COMMAND} -E remove ${c_SDK_IMAGE_TARGET_DIR}/target_img2_otrcore.asm
                                DEPENDS target_wifi_nouse ${c_MCU_PROJECT_DIR}/asdk/image/target_img2_otrcore.asm
                            )
                            ameba_list_append(private_sources
                                ${CMAKE_CURRENT_BINARY_DIR}/wifi_intf_drv_to_app_ext_noused.c
                            )
                        endif()
                    endif()
                endif()
            endif()
        endif()
    endif()
endif()

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
if(private_sources)
    ameba_add_internal_library(wifi_feature_disable
        p_SOURCES
            ${private_sources}
        p_INCLUDES
            ${private_includes}
        p_DEFINITIONS
            ${private_definitions}
        p_COMPILE_OPTIONS
            ${private_compile_options}
    )
endif()
##########################################################################################