include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES                                     #
#*****************************************************************************#

#*****************************************************************************#
#                               OBJECT FILE LIST                              #
#*****************************************************************************#
OBJS = 

#*****************************************************************************#
#                               INCLUDE DEPENDENCY                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
all: CORE_TARGETS
	make -C amebadplus_bsp all
	make -C bt_coex all
	make -C bt_api all
	make -C bt_atcmd all

	#make -C bt_demo all
ifeq ($(CONFIG_BT_ZEPHYR), y)
	make -C bt_zephyr all
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
ifeq ($(findstring y, $(CONFIG_BT_MESH_PROVISIONER_SUPPORT) $(CONFIG_BT_MESH_DEVICE_SUPPORT)), y)
ifneq ($(CONFIG_BT_ZEPHYR), y)
	make -C ble_mesh_stack all
endif
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

#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean:
	make -C amebadplus_bsp clean
	make -C bt_coex clean
	make -C bt_api clean
	make -C bt_atcmd clean

	#make -C bt_demo clean
ifeq ($(CONFIG_BT_ZEPHYR), y)
	make -C bt_zephyr clean
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
ifeq ($(findstring y, $(CONFIG_BT_MESH_PROVISIONER_SUPPORT) $(CONFIG_BT_MESH_DEVICE_SUPPORT)), y)
	make -C ble_mesh_stack clean
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

-include $(DEPS)
