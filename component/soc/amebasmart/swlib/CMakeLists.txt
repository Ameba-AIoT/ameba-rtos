if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if(${c_MCU_PROJECT_NAME} STREQUAL "ap")

    elseif(${c_MCU_PROJECT_NAME} STREQUAL "hp")

    elseif(${c_MCU_PROJECT_NAME} STREQUAL "lp")
        ameba_add_subdirectory(cmake/wrapper)
    endif()
endif()