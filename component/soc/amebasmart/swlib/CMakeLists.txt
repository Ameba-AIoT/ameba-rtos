if(CONFIG_AMEBASMART)
    if(${c_MCU_PROJECT_NAME} STREQUAL "ap")
        include(swlib.cmake)
    else()
        if(CONFIG_KERNEL)
            include(swlib_wrap.cmake)
        endif()
    endif()
endif()