if(CMAKE_REFACTOR) # For temporary compatibility.
    ameba_set(WIFIFWDIR ${c_COMPONENT_DIR}/wifi/wifi_fw) #Used in component/wifi/wifi_fw/amebasmart/CMakeLists.txt

    if(CONFIG_AMEBAD)
        ameba_fatal("not ported for ${c_SOC_TYPE} yet")
    elseif(CONFIG_AMEBADPLUS)
        ameba_add_subdirectory(amebadplus)
    elseif(CONFIG_AMEBAGREEN2)
        ameba_add_subdirectory(amebagreen2)
    elseif(CONFIG_AMEBAL2)
        ameba_fatal("not ported for ${c_SOC_TYPE} yet")
    elseif(CONFIG_AMEBALITE)
        if(CONFIG_WLAN)
            if(CONFIG_AS_INIC_NP OR CONFIG_SINGLE_CORE_WIFI)
                ameba_add_subdirectory_ifnot(CONFIG_MP_SHRINK amebalite)
            endif()
        endif()
    elseif(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
        ameba_add_subdirectory(amebasmart)
    else()
        ameba_fatal("not ported for ${c_SOC_TYPE} yet")
    endif()
    ameba_global_include(
        common
        ${c_SOC_TYPE}/include
    )

else()# For temporary compatibility.
    ameba_fatal("only used in CMAKE_REFACTOR")
endif() #For temporary compatibility.