if (CONFIG_WLAN)

    #common lib
    if(CONFIG_AMEBADPLUS)
        if(CONFIG_AS_INIC_NP)
            if(CONFIG_KM4_AS_NP)
                list(
                    APPEND LINK_APP_LIB
                    ${APP_LIB_DIR}/lib_wifi_com_sec.a
                )
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
                    ${APP_LIB_DIR}/lib_wifi_common.a
                    ${APP_LIB_DIR}/lib_wpa_lite.a
                    ${APP_LIB_DIR}/lib_wps.a
                )
            else()
                list(
                    APPEND LINK_APP_LIB
                    ${APP_LIB_DIR}/lib_wifi_common.a
                )
            endif()
        else()
            list(
                APPEND LINK_APP_LIB
                ${APP_LIB_DIR}/lib_wifi_com_sec.a
            )
        endif()
    endif()

    if(CONFIG_AMEBAD)
        list(
            APPEND LINK_APP_LIB
            ${APP_LIB_DIR}/lib_wifi_com_sec.a
        )
    endif()

    if(CONFIG_AMEBASMART)
        list(
            APPEND LINK_APP_LIB
            ${APP_LIB_DIR}/lib_wifi_com_sec.a
        )
    endif()

    if(CONFIG_AMEBASMARTPLUS)
        list(
            APPEND LINK_APP_LIB
            ${APP_LIB_DIR}/lib_wifi_com_sec.a
        )
    endif()

    if(CONFIG_AMEBALITE)
        list(
            APPEND LINK_APP_LIB
            ${APP_LIB_DIR}/lib_wifi_com_sec.a
        )
    endif()

    if(CONFIG_AMEBAGREEN2)
        list(
            APPEND LINK_APP_LIB
            ${APP_LIB_DIR}/lib_wifi_common.a
        )
    endif()

    if(CONFIG_AMEBAPRO3)
        list(
            APPEND LINK_APP_LIB
            ${APP_LIB_DIR}/lib_wifi_common.a
        )
    endif()

    if(CONFIG_AMEBAL2)
        list(
            APPEND LINK_APP_LIB
            ${APP_LIB_DIR}/lib_wifi_com_sec.a
        )
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
    # NP Link Library
    elseif(CONFIG_AS_INIC_NP)
        if(CONFIG_MP_INCLUDED)
            if(CONFIG_MP_SHRINK)
                if(CONFIG_WHC_INTF_SDIO)
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wifi_whc_sdio_np_mp_shrink.a
                    )
                elseif(CONFIG_WHC_INTF_SPI)
                    list(
                        APPEND LINK_APP_LIB 
                        ${APP_LIB_DIR}/lib_wifi_whc_spi_np_mp_shrink.a
                    )
                elseif(CONFIG_WHC_INTF_USB)
                    list(
                        APPEND LINK_APP_LIB 
                        ${APP_LIB_DIR}/lib_wifi_whc_usb_np_mp_shrink.a
                    )
                else()
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wifi_whc_np_mp_shrink.a
                    )
                endif()
            else()
                if(CONFIG_WHC_INTF_SDIO)
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wifi_whc_sdio_np_mp.a
                    )
                elseif(CONFIG_WHC_INTF_SPI)
                    list(
                        APPEND LINK_APP_LIB 
                        ${APP_LIB_DIR}/lib_wifi_whc_spi_np_mp.a
                    )
                elseif(CONFIG_WHC_INTF_USB)
                    list(
                        APPEND LINK_APP_LIB 
                        ${APP_LIB_DIR}/lib_wifi_whc_usb_np_mp.a
                    )
                else()
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wifi_whc_np_mp.a
                    )
                endif()
            endif()
        else()
            if(CONFIG_WHC_INTF_SDIO)
                list(
                    APPEND LINK_APP_LIB
                    ${APP_LIB_DIR}/lib_wifi_whc_sdio_np.a
                )
            elseif(CONFIG_WHC_INTF_SPI)
                list(
                    APPEND LINK_APP_LIB
                    ${APP_LIB_DIR}/lib_wifi_whc_spi_np.a
                )
            elseif(CONFIG_WHC_INTF_USB)
                list(
                    APPEND LINK_APP_LIB 
                    ${APP_LIB_DIR}/lib_wifi_whc_usb_np.a
                )
            else()
                list(
                    APPEND LINK_APP_LIB
                    ${APP_LIB_DIR}/lib_wifi_whc_np.a
                )
            endif()
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
        if(CONFIG_SDIO_BRIDGE)
            if(CONFIG_MP_INCLUDED)
                if(CONFIG_MP_SHRINK)
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wifi_sdio_bridge_mp_shrink.a
                    )
                else()
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wifi_sdio_bridge_mp.a
                    )
                endif()
            else()
                list(
                    APPEND LINK_APP_LIB
                    ${APP_LIB_DIR}/lib_wifi_sdio_bridge.a
                )
            endif()
        elseif(CONFIG_SPI_BRIDGE)
            if(CONFIG_MP_INCLUDED)
                if(CONFIG_MP_SHRINK)
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wifi_spi_bridge_mp_shrink.a
                    )
                else()
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wifi_spi_bridge_mp.a
                    )
                endif()
            else()
                list(
                    APPEND LINK_APP_LIB
                    ${APP_LIB_DIR}/lib_wifi_spi_bridge.a
                )
            endif()
        else()
            if(CONFIG_MP_INCLUDED)
                if(CONFIG_MP_SHRINK)
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wifi_single_core_mp_shrink.a
                    )
                else()
                    list(
                        APPEND LINK_APP_LIB
                        ${APP_LIB_DIR}/lib_wifi_single_core_mp.a
                    )
                endif()
            else()
                list(
                    APPEND LINK_APP_LIB
                    ${APP_LIB_DIR}/lib_wifi_single_core.a
                )
            endif()
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
