if (CONFIG_WLAN)

    #common lib
    if(CONFIG_AMEBADPLUS)
        if(CONFIG_AS_INIC_NP)
            if(CONFIG_KM4_AS_NP)
                if(CONFIG_WPA_LOCATION_DEV)
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wpa_lite.a
                        ${APP_LIB_DIR}/lib_wps.a
                    )
                endif()
                if(CONFIG_WPA_STD)
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wpa_std.a
                    )
                endif()
            elseif(CONFIG_ZEPHYR_SDK)
                list(
                    APPEND LINK_APP_LIB
                    ${APP_LIB_DIR}/lib_wpa_lite.a
                    ${APP_LIB_DIR}/lib_wps.a
                )
            endif()
        endif()
    endif()

    # AP Link Library
    if(CONFIG_AS_INIC_AP)
        if(NOT CONFIG_WHC_INTF_SPI)
            if(CONFIG_MP_INCLUDED)
                if(CONFIG_MP_SHRINK)
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wifi_whc_ap_mp_shrink.a
                    )
                else()
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wifi_whc_ap_mp.a
                    )
                endif()
            else()
                list(
                    APPEND LINK_APP_LIB
                    ${APP_LIB_DIR}/lib_wifi_whc_ap.a
                )
            endif()
        endif()
        if(NOT CONFIG_MP_SHRINK)
            list(
                APPEND LINK_APP_LIB
                ${APP_LIB_DIR}/lib_wpa_lite.a
                ${APP_LIB_DIR}/lib_wps.a
            )
        endif()

    # Single Core Link Library
    elseif(CONFIG_SINGLE_CORE_WIFI)
        if(NOT CONFIG_MP_SHRINK)
            list(
                APPEND LINK_APP_LIB
                ${APP_LIB_DIR}/lib_wpa_lite.a
                ${APP_LIB_DIR}/lib_wps.a
            )
        endif()
    endif()

    if(CONFIG_AS_INIC_AP)
        if(NOT CONFIG_MP_SHRINK)
            list(
                APPEND LINK_APP_LIB
                ${APP_LIB_DIR}/lib_wifi_cast.a
            )
        endif()
    endif()

endif()
