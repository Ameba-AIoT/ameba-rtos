ameba_add_subdirectory(drivers)
ameba_add_subdirectory_if_exist(third_party)

if(CONFIG_GUI_EN)
    ameba_add_subdirectories_if(CONFIG_EMWIN_EN emWin)
    ameba_add_subdirectories_if(CONFIG_LVGL_EN LVGL)
endif()
