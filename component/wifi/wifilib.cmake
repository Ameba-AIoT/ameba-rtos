if (CONFIG_WLAN)

    #common lib
    if(CONFIG_AMEBADPLUS)
        if(CONFIG_AS_INIC_NP)
            if(CONFIG_KM4_AS_NP)
                if(CONFIG_WPA_STD)
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wpa_std.a
                    )
                endif()
            endif()
        endif()
    endif()


endif()
