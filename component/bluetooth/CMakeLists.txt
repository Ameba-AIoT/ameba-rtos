# linux_driver: Linux host BT INIC driver; not built by this cmake project but is part of the open-source release.
# Keep the common files plus the ports of the current SoC only.
if(CONFIG_AMEBADPLUS)
    ameba_install_directory(example/bt_host/linux_driver p_EXCLUDE_REGEX "_green2($|/)")
elseif(CONFIG_AMEBAGREEN2)
    ameba_install_directory(example/bt_host/linux_driver p_EXCLUDE_REGEX "_dplus($|/)")
endif()

if(NOT CONFIG_BT)
    return()
endif()

ameba_add_subdirectory_if(CONFIG_BT_AUDIO_CODEC_SUPPORT bt_audio)
ameba_add_subdirectory_ifnot(CONFIG_BT_INIC_HOST driver)
ameba_add_subdirectory(osif)
ameba_add_subdirectory(bt_mp)
if(NOT CONFIG_BT_INIC)
    ameba_add_subdirectory(api)
    ameba_add_subdirectory(example)
    ameba_add_subdirectory(rtk_coex)
    ameba_add_subdirectory_if(CONFIG_BT_ZEPHYR zephyr)
    ameba_add_subdirectory_ifnot(CONFIG_BT_ZEPHYR rtk_stack)  # refer to ble_mesh_stack
endif()