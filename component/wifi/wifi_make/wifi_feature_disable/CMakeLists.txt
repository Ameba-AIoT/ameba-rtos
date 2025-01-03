set(DIR ${BASEDIR}/component/wifi/wifi_feature_disable)

set(CSRC)

if(NOT CONFIG_AS_INIC_AP)
    if(NOT CONFIG_WIFI_CSI_ENABLE)
        list(APPEND CSRC ${DIR}/wifi_feature_dis_csi.c)
    endif()
    if(NOT CONFIG_WIFI_ANTDIV_ENABLE)
        list(APPEND CSRC ${DIR}/wifi_feature_dis_antdiv.c)
    endif()
    if(NOT CONFIG_WIFI_SPEAKER_ENABLE)
        list(APPEND CSRC ${DIR}/wifi_feature_dis_speaker.c)
    endif()
    if(NOT CONFIG_WIFI_P2P_ENABLE)
        list(APPEND CSRC ${DIR}/wifi_feature_dis_p2p.c)
    endif()
endif()

if(NOT (CONFIG_WIFI_11K_ENABLE AND CONFIG_WIFI_11V_ENABLE AND CONFIG_WIFI_11R_ENABLE))
    list(APPEND CSRC  ${DIR}/wifi_feature_dis_kvr.c)
endif()

if(CONFIG_AS_INIC_NP)
    if(NOT CONFIG_AMEBALITE)
        if(NOT CONFIG_INIC_INTF_SDIO)
            if(NOT CONFIG_INIC_INTF_SPI)
                if(NOT CONFIG_INIC_INTF_USB)
					if(NOT CONFIG_ZEPHYR_SDK)
						if(TARGET target_wifi_nouse)
							add_custom_command(
								OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/wifi_intf_drv_to_app_ext_noused.c
								COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/gen_noused_c.py -a ${IMAGE_TARGET_FOLDER}/target_img2_otrcore.asm -c ${DIR}/wifi_intf_drv_to_app_ext_strong.c -r ${CMAKE_CURRENT_SOURCE_DIR}/wifi_intf_drv_to_app_ext_noused.c
								# COMMAND ${CMAKE_COMMAND} -E remove ${IMAGE_TARGET_FOLDER}/target_img2_otrcore.asm
								DEPENDS target_wifi_nouse ${PROJECTDIR}/asdk/image/target_img2_otrcore.asm
							)
							list(APPEND CSRC ${CMAKE_CURRENT_SOURCE_DIR}/wifi_intf_drv_to_app_ext_noused.c)
						endif()
					endif()
                endif()
            endif()
        endif()
    endif()
endif()

if (CSRC)
ameba_internal_library(wifi_feature_disable)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
endif()