include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                              OBJECT FILE LIST                               #
#*****************************************************************************#
OBJS =

#*****************************************************************************#
#                             INCLUDE DEPENDENCY                              #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                          RULES TO GENERATE TARGETS                          #
#*****************************************************************************#
all: CORE_TARGETS
	make -C amebasmart_bsp all
	make -C bt_coex all

ifeq ($(CONFIG_BT_INIC), y)
	make -C bt_inic all
else
	make -C bt_api all
	make -C bt_atcmd all
endif

ifeq ($(CONFIG_BT_ZEPHYR), y)
	make -C bt_zephyr all
endif

ifeq ($(findstring y, $(CONFIG_BT_MESH_PROVISIONER_SUPPORT) $(CONFIG_BT_MESH_DEVICE_SUPPORT)), y)
ifneq ($(CONFIG_BT_ZEPHYR), y)
	make -C ble_mesh_stack all
endif
endif

ifeq ($(CONFIG_BT_PERIPHERAL), y)
	make -C ble_peripheral all
endif
ifeq ($(CONFIG_BT_CENTRAL), y)
	make -C ble_central all
endif
ifeq ($(CONFIG_BT_SCATTERNET), y)
	make -C ble_scatternet all
endif
ifeq ($(CONFIG_BT_THROUGHPUT), y)
	make -C ble_throughput all
endif
ifeq ($(CONFIG_BT_MESH_PROVISIONER), y)
	make -C ble_mesh_provisioner all
endif
ifeq ($(CONFIG_BT_MESH_DEVICE), y)
	make -C ble_mesh_device all
endif
ifeq ($(CONFIG_BT_MESH_PROVISIONER_SCATTERNET), y)
	make -C ble_mesh_provisioner_scatternet all
endif
ifeq ($(CONFIG_BT_MESH_DEVICE_SCATTERNET), y)
	make -C ble_mesh_device_scatternet all
endif
ifeq ($(CONFIG_BT_MESH_PROVISIONER_TEST), y)
	make -C ble_mesh_provisioner_test all
endif
ifeq ($(CONFIG_BT_MESH_DEVICE_TEST), y)
	make -C ble_mesh_device_test all
endif
ifeq ($(CONFIG_BT_LE_AUDIO), y)
	make -C ble_audio_common all
	make -C ble_audio all
	make -C bt_audio all
endif
ifeq ($(CONFIG_BT_ISO_TEST), y)
	make -C ble_iso all
endif
ifeq ($(CONFIG_BT_APP_AUDIO_DATA), y)
	make -C bt_app_audio_data all
endif
ifeq ($(CONFIG_BT_A2DP), y)
	make -C bt_a2dp all
	make -C bt_audio all
endif
ifeq ($(CONFIG_BT_A2DP_HFP), y)
	make -C bt_a2dp_hfp all
	make -C bt_audio all
endif
ifeq ($(CONFIG_BT_SPP), y)
	make -C bt_spp all
endif
ifeq ($(CONFIG_BT_HID), y)
	make -C bt_hid all
endif
ifeq ($(CONFIG_BT_HFP), y)
	make -C bt_hfp all
	make -C bt_audio all
endif
ifeq ($(CONFIG_BT_A2DP_SCATTERNET), y)
	make -C bt_a2dp_scatternet all
	make -C bt_audio all
endif
ifeq ($(CONFIG_BT_AUDIO_MP_TEST), y)
	make -C bt_audio_mp_test all
	make -C bt_audio all
endif
ifeq ($(CONFIG_BT_A2DP_LE_AUDIO_PBP), y)
	make -C ble_audio_common all
	make -C bt_a2dp_pbp all
	make -C bt_audio all
endif
ifeq ($(CONFIG_BT_A2DP_LE_AUDIO_TMAP), y)
	make -C ble_audio_common all
	make -C bt_a2dp_tmap all
	make -C bt_audio all
endif

#*****************************************************************************#
#                            GENERATE OBJECT FILE                             #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                            CLEAN GENERATED FILES                            #
#*****************************************************************************#
clean:
	make -C amebasmart_bsp clean
	make -C bt_coex clean

ifeq ($(CONFIG_BT_INIC), y)
	make -C bt_inic clean
else
	make -C bt_api clean
	make -C bt_atcmd clean
endif

ifeq ($(CONFIG_BT_ZEPHYR), y)
	make -C bt_zephyr clean
endif

ifeq ($(findstring y, $(CONFIG_BT_MESH_PROVISIONER_SUPPORT) $(CONFIG_BT_MESH_DEVICE_SUPPORT)), y)
ifneq ($(CONFIG_BT_ZEPHYR), y)
	make -C ble_mesh_stack clean
endif
endif

ifeq ($(CONFIG_BT_PERIPHERAL), y)
	make -C ble_peripheral clean
endif
ifeq ($(CONFIG_BT_CENTRAL), y)
	make -C ble_central clean
endif
ifeq ($(CONFIG_BT_SCATTERNET), y)
	make -C ble_scatternet clean
endif
ifeq ($(CONFIG_BT_THROUGHPUT), y)
	make -C ble_throughput clean
endif
ifeq ($(CONFIG_BT_MESH_PROVISIONER), y)
	make -C ble_mesh_provisioner clean
endif
ifeq ($(CONFIG_BT_MESH_DEVICE), y)
	make -C ble_mesh_device clean
endif
ifeq ($(CONFIG_BT_MESH_PROVISIONER_SCATTERNET), y)
	make -C ble_mesh_provisioner_scatternet clean
endif
ifeq ($(CONFIG_BT_MESH_DEVICE_SCATTERNET), y)
	make -C ble_mesh_device_scatternet clean
endif
ifeq ($(CONFIG_BT_MESH_PROVISIONER_TEST), y)
	make -C ble_mesh_provisioner_test clean
endif
ifeq ($(CONFIG_BT_MESH_DEVICE_TEST), y)
	make -C ble_mesh_device_test clean
endif
ifeq ($(CONFIG_BT_LE_AUDIO), y)
	make -C ble_audio_common clean
	make -C ble_audio clean
	make -C bt_audio clean
	make -C bt_audio/bt_codec clean
	make -C bt_audio/audio_sync clean
endif
ifeq ($(CONFIG_BT_ISO_TEST), y)
	make -C ble_iso clean
endif
ifeq ($(CONFIG_BT_APP_AUDIO_DATA), y)
	make -C bt_app_audio_data clean
endif
ifeq ($(CONFIG_BT_A2DP), y)
	make -C bt_a2dp clean
	make -C bt_audio clean
	make -C bt_audio/bt_codec clean
	make -C bt_audio/audio_sync clean
endif
ifeq ($(CONFIG_BT_A2DP_HFP), y)
	make -C bt_a2dp_hfp clean
	make -C bt_audio clean
	make -C bt_audio/bt_codec clean
	make -C bt_audio/audio_sync clean
endif
ifeq ($(CONFIG_BT_SPP), y)
	make -C bt_spp clean
endif
ifeq ($(CONFIG_BT_HID), y)
	make -C bt_hid clean
endif
ifeq ($(CONFIG_BT_HFP), y)
	make -C bt_hfp clean
	make -C bt_audio clean
	make -C bt_audio/bt_codec clean
	make -C bt_audio/audio_sync clean
endif
ifeq ($(CONFIG_BT_A2DP_SCATTERNET), y)
	make -C bt_a2dp_scatternet clean
	make -C bt_audio clean
	make -C bt_audio/bt_codec clean
	make -C bt_audio/audio_sync clean
endif
ifeq ($(CONFIG_BT_AUDIO_MP_TEST), y)
	make -C bt_audio_mp_test clean
	make -C bt_audio clean
	make -C bt_audio/bt_codec clean
	make -C bt_audio/audio_sync clean
endif
ifeq ($(CONFIG_BT_A2DP_LE_AUDIO_PBP), y)
	make -C ble_audio_common clean
	make -C bt_a2dp_pbp clean
	make -C bt_audio clean
	make -C bt_audio/bt_codec clean
	make -C bt_audio/audio_sync clean
endif
ifeq ($(CONFIG_BT_A2DP_LE_AUDIO_TMAP), y)
	make -C ble_audio_common clean
	make -C bt_a2dp_tmap clean
	make -C bt_audio clean
	make -C bt_audio/bt_codec clean
	make -C bt_audio/audio_sync clean
endif

-include $(DEPS)
