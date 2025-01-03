if(CONFIG_GUI_EN AND CONFIG_LVGL_EN)
    ameba_add_subdirectories(LVGL)
endif()

ameba_add_subdirectories(
    drivers
    # test
)