if(NOT CONFIG_AMEBAD)
    #FIXME: amebad not not support rtk_coex_api?
    if (CONFIG_COEXIST_HOST)
        include(rtk_coex_api.cmake)
    endif()
endif()

if (CONFIG_COEXIST_DEV)
    include(rtk_coex.cmake)
else()
    #ameba_global_include(${CMAKE_CURRENT_SOURCE_DIR})
endif()
