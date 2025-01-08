

############################## Global Definitions for Private use ################################
set(p_dependency_list_full
    # Targets defined in make/dependency_common.cmake
    audio at_cmd clintwood example emwin file_system flac lwip network os rt_xmodem  soc_common ssl stdlib ui usb utilities utilities_example wifi_audio wpan png jpeg zlib aivoice tflite_micro

    # Targets defined in make/dependency_platform.cmake
    app_monitor app_xmodem cmsis cmsis_dsp hal misc swlib test usrcfg utils wifi img3 rtk_coex rtk_coex_api

    # Targets defined in make/dependency_mcu.cmake
    bluetooth chipinfo bootloader crashdump freertos fwlib pmc user_project vmin_test
)

set(p_dependency_list_non_loader
    # Targets defined in make/dependency_common.cmake
    audio at_cmd clintwood example emwin file_system flac lwip network os rt_xmodem soc_common ssl stdlib ui usb utilities utilities_example wifi_audio wpan png jpeg zlib aivoice tflite_micro

    # Targets defined in make/dependency_platform.cmake
    app_monitor app_xmodem cmsis cmsis_dsp hal misc swlib test usrcfg utils wifi rtk_coex rtk_coex_api

    # Targets defined in make/dependency_mcu.cmake
    bluetooth chipinfo crashdump freertos fwlib pmc user_project vmin_test
)

set(p_dependency_list_loader
    chipinfo bootloader crashdump
)

set(p_dependency_list_secure
    chipinfo
)
