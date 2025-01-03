if(CONFIG_802154_ZIGBEE_APPLICATION_EN)

list(
	APPEND MODULE_IFLAGS
    ${ZB_EXAMPLE_DIR}/	
)

# example: light_control
if(CONFIG_802154_ZIGBEE_EXAMPLE_LIGHTCTL_B)
add_subdirectory(light_control/light_bulb)
endif()
if(CONFIG_802154_ZIGBEE_EXAMPLE_LIGHTCTL_S)
add_subdirectory(light_control/light_switch)
endif()
if(CONFIG_802154_ZIGBEE_EXAMPLE_LIGHTCTL_C)
add_subdirectory(light_control/light_coordinator)
endif()

# example: ha_on_off_light_n_switch
if(CONFIG_802154_ZIGBEE_EXAMPLE_ONOFFLIGHT_L)
add_subdirectory(ha_on_off_light_n_switch/on_off_light)
endif()
if(CONFIG_802154_ZIGBEE_EXAMPLE_ONOFFLIGHT_S)
add_subdirectory(ha_on_off_light_n_switch/on_off_light_switch)
endif()

# example: ha_custom_cluster
if(CONFIG_802154_ZIGBEE_EXAMPLE_CUSTOM_CLUSTER_ZC)
add_subdirectory(ha_custom_cluster/custom_cluster_zc)
endif()
if(CONFIG_802154_ZIGBEE_EXAMPLE_CUSTOM_CLUSTER_ZR)
add_subdirectory(ha_custom_cluster/custom_cluster_zr)
endif()

endif()