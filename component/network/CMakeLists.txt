if(CONFIG_CORE_AS_AP)
    ameba_add_subdirectory(mqtt)
    ameba_add_subdirectory(httpc)
    ameba_add_subdirectory(httpd)
    ameba_add_subdirectory(websocket)
    ameba_add_subdirectory(sntp)
    ameba_add_subdirectory(dhcp)

    if(CONFIG_AMEBAD OR CONFIG_AMEBADPLUS)
        ameba_add_subdirectory(dnrd/src)
    endif()

    if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
        if("${c_MCU_PROJECT_NAME}" STREQUAL "ap")
            ameba_add_subdirectory(dnrd/src)
            ameba_add_subdirectory(iperf3)
        elseif("${c_MCU_PROJECT_NAME}" STREQUAL "hp")
            ameba_add_subdirectory(coap)
        endif()
    else()
        ameba_add_subdirectory(iperf3)

    endif()

    ameba_add_subdirectory(httplite)
    ameba_add_subdirectory(iperf)
    ameba_add_subdirectory(ping)
    ameba_add_subdirectory(xml)
endif()

#OPTIMIZE: cJSON.h is always included in atcmd
ameba_add_subdirectory(cJSON)