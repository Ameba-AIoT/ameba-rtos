ameba_internal_library(bluetooth)

add_subdirectory(amebadplus_bsp)
add_subdirectory(bt_coex)

if(CONFIG_BT_INIC)
    add_subdirectory(bt_inic)
endif()