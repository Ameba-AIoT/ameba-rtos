set(DIR ${BASEDIR}/component/wifi)
set(USRCFG_DIR ${TARGETDIR}/usrcfg)

if(CONFIG_ZEPHYR_SDK AND CONFIG_SINGLE_CORE_WIFI)
set(TARGET_LIB wifi_api)
endif()

set(
    CSRC 
    ${DIR}/api/wifi_ind.c 
	${DIR}/api/wifi_init.c 
	${USRCFG_DIR}/ameba_wificfg.c
)

if(CONFIG_AS_INIC_AP)
    list(
        APPEND CSRC 
        ${DIR}/wpa_supplicant/wpa_supplicant/wifi_eap_config.c 
		${DIR}/wpa_supplicant/wpa_supplicant/wifi_p2p_config.c 
		${DIR}/wpa_supplicant/wpa_supplicant/wifi_wps_config.c 
		${DIR}/wpa_supplicant/src/crypto/tls_polarssl.c 
		${DIR}/wifi_fast_connect/wifi_fast_connect.c
        ${DIR}/wifi_auto_reconnect/wifi_auto_reconnect.c
    )
elseif(CONFIG_SINGLE_CORE_WIFI)
    list(
        APPEND CSRC 
        ${DIR}/wpa_supplicant/wpa_supplicant/wifi_eap_config.c 
		${DIR}/wpa_supplicant/wpa_supplicant/wifi_p2p_config.c 
		${DIR}/wpa_supplicant/wpa_supplicant/wifi_wps_config.c 
		${DIR}/wpa_supplicant/src/crypto/tls_polarssl.c 
		${DIR}/wifi_fast_connect/wifi_fast_connect.c
        ${DIR}/wifi_auto_reconnect/wifi_auto_reconnect.c
    )
endif()

if(NOT CONFIG_AS_INIC_AP)
    list(
        APPEND CSRC 
        ${USRCFG_DIR}/ameba_wifi_power_table_usrcfg.c
        ${USRCFG_DIR}/ameba_wifi_country_code_table_usrcfg.c
    )
    if(CONFIG_AMEBADPLUS)
        list(
            APPEND CSRC 
            ${USRCFG_DIR}/ameba_wifi_power_table_usrcfg_1v8.c 
	        ${USRCFG_DIR}/ameba_wifi_power_table_usrcfg_3v3_1v25.c 
        )
    endif()

    if(CONFIG_AMEBAD)
        list(
            APPEND CSRC 
            ${USRCFG_DIR}/ameba_wifi_power_table_usrcfg_1v8.c 
        )
    endif()
endif()

if(CONFIG_ZEPHYR_SDK AND CONFIG_SINGLE_CORE_WIFI)
ameba_app_library_with_gitver(${TARGET_LIB})
else()
ameba_internal_library(wifi_api)
endif()

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
