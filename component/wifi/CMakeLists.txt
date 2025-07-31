ameba_add_subdirectory(driver)

# D/SMART/SMARTPLUS km0 Need driver to export global include path, but no CONFIG_WLAN
if(NOT CONFIG_WLAN)
    return()
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

ameba_add_subdirectory(rtk_app)
if(CONFIG_WHC_HOST)
    ameba_add_subdirectory(wpa_supplicant/wpa_lite)
    if(NOT CONFIG_MP_SHRINK)
        ameba_add_subdirectory(wpa_supplicant/src/wps)
        ameba_add_subdirectory(wpa_supplicant/src/eap_peer)
    endif()
elseif(CONFIG_WHC_DEV)
    if(CONFIG_WPA_STD)
        add_subdirectory(wpa_supplicant/wpa_supplicant_std)
    else()
        ameba_add_subdirectory(wpa_supplicant/wpa_lite)
        if(CONFIG_WPA_LOCATION_DEV OR CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)
            ameba_add_subdirectory(wpa_supplicant/src/wps)
            ameba_add_subdirectory(wpa_supplicant/src/eap_peer)
        endif()
    endif()
elseif(CONFIG_WHC_NONE)
    if(CONFIG_WPA_STD)
        add_subdirectory(wpa_supplicant/wpa_supplicant_std)
    else()
        ameba_add_subdirectory(wpa_supplicant/wpa_lite)
        if(NOT CONFIG_MP_SHRINK)
            ameba_add_subdirectory(wpa_supplicant/src/wps)
            ameba_add_subdirectory(wpa_supplicant/src/eap_peer)
        endif()
    endif()
endif()

