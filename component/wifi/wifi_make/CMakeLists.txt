add_subdirectory(api)
if(NOT CONFIG_AMEBALITE)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E echo
        OUTPUT_FILE wifi_feature_disable/wifi_intf_drv_to_app_ext_noused.c
    )
endif()
add_subdirectory(inic)
add_subdirectory(wifi_feature_disable)

# make wifi lib
if(NOT CONFIG_AMEBA_RLS)
    if(CONFIG_WLAN)
        add_subdirectory(common)
        #build AP libs
        if(CONFIG_AS_INIC_AP)
            if(NOT CONFIG_MP_SHRINK)
                add_subdirectory(wpa_supplicant/wpa_lite)
                add_subdirectory(wpa_supplicant/wps)
                add_subdirectory(wifi_cast)
                add_subdirectory(wifi_tunnel_app)
            endif()
        #build NP libs
        elseif(CONFIG_AS_INIC_NP)
            add_subdirectory(driver)
            if(CONFIG_ZEPHYR_SDK)
                add_subdirectory(wpa_supplicant/wpa_lite)
                add_subdirectory(wpa_supplicant/wps)
            endif()
        # build single core wifi libs
        elseif(CONFIG_SINGLE_CORE_WIFI)
            add_subdirectory(driver)
            if(NOT CONFIG_MP_SHRINK)
                add_subdirectory(wpa_supplicant/wpa_lite)
                add_subdirectory(wpa_supplicant/wps)
            endif()
        endif()
    endif()
endif()
