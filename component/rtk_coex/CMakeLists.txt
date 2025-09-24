
if (CONFIG_COEXIST_HOST)
    include(rtk_coex_api.cmake)
endif()

if (CONFIG_COEXIST_DEV)
    include(rtk_coex.cmake)
else()
    #ameba_global_include(${CMAKE_CURRENT_SOURCE_DIR})
endif()
