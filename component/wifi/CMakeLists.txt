
ameba_add_subdirectory(driver)

if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if(${c_MCU_PROJECT_NAME} STREQUAL "lp")
        #NOTE: For lp, just only add head dirs
        return()
    endif()
endif()

ameba_add_subdirectory(api)

if(NOT CONFIG_AMEBALITE)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E echo
        OUTPUT_FILE wifi_feature_disable/wifi_intf_drv_to_app_ext_noused.c
    )
endif()
ameba_add_subdirectory(whc)
ameba_add_subdirectory(common)

if (CONFIG_WLAN)
    ameba_add_subdirectory(rtk_app)
    if(CONFIG_AS_INIC_AP)
        ameba_add_subdirectory(wpa_supplicant/wpa_lite)
        if(NOT CONFIG_MP_SHRINK)
            ameba_add_subdirectory(wpa_supplicant/src/wps)
            ameba_add_subdirectory(wpa_supplicant/src/eap_peer)
        endif()
    elseif(CONFIG_AS_INIC_NP)
        ameba_add_subdirectory(wpa_supplicant/wpa_lite)
        if(CONFIG_WPA_LOCATION_DEV)
            ameba_add_subdirectory(wpa_supplicant/src/wps)
            ameba_add_subdirectory(wpa_supplicant/src/eap_peer)
        endif()
        if(CONFIG_WPA_STD)
            add_subdirectory(wpa_supplicant/wpa_supplicant_std)
        endif()
    elseif(CONFIG_SINGLE_CORE_WIFI)
        if(CONFIG_WHC_BRIDGE AND CONFIG_WPA_STD)
            add_subdirectory(wpa_supplicant/wpa_supplicant_std)
        else()
            ameba_add_subdirectory(wpa_supplicant/wpa_lite)
            if(NOT CONFIG_MP_SHRINK)
                ameba_add_subdirectory(wpa_supplicant/src/wps)
                ameba_add_subdirectory(wpa_supplicant/src/eap_peer)
            endif()
        endif()
    endif()
endif()

