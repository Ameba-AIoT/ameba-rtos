if(CONFIG_RTL8720F)
    add_compile_options(-O2)
endif()

if(CMAKE_REFACTOR) # For temporary compatibility.
    ameba_set(WIFIFWDIR ${c_COMPONENT_DIR}/wifi/wifi_fw) #Used in component/wifi/wifi_fw/amebasmart/CMakeLists.txt

    if(CONFIG_AMEBAD)
        ameba_add_subdirectory_if(CONFIG_WIFI_FW_EN amebad)
    elseif(CONFIG_AMEBADPLUS)
        ameba_add_subdirectory_if(CONFIG_WIFI_FW_EN amebadplus)
    elseif(CONFIG_AMEBAGREEN2)
        ameba_add_subdirectory_if(CONFIG_WIFI_FW_EN amebagreen2)
    elseif(CONFIG_AMEBAL2)
        ameba_add_subdirectory_if(CONFIG_WIFI_FW_EN rle1331)
    elseif(CONFIG_RTL8720F)
        ameba_add_subdirectory_if(CONFIG_WIFI_FW_EN rtl8720f)
    elseif(CONFIG_AMEBALITE)
        if(CONFIG_WLAN)
            if(CONFIG_WHC_DEV OR CONFIG_WHC_NONE)
                ameba_add_subdirectory_ifnot(CONFIG_MP_SHRINK amebalite)
            endif()
        endif()
    elseif(CONFIG_AMEBASMART)
        ameba_add_subdirectory(amebasmart)
    else()
        ameba_fatal("not ported for ${c_SOC_TYPE} yet")
    endif()

    ameba_global_include(
        common
    )

    ameba_global_include(
        ${c_SOC_TYPE}/include
    )

else()# For temporary compatibility.
    ameba_fatal("only used in CMAKE_REFACTOR")
endif() #For temporary compatibility.
