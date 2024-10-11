include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
BT_DIR = $(BASEDIR)/component/bluetooth

vpath %.c $(BT_DIR)/rtk_stack/src/mesh/cmd \
		$(BT_DIR)/rtk_stack/src/mesh/model \
		$(BT_DIR)/rtk_stack/src/mesh/model/realtek \
		$(BT_DIR)/rtk_stack/src/mesh/utility

CSRC += \
	$(BT_DIR)/rtk_stack/src/mesh/cmd/mesh_cmd.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/delay_msg_rsp.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/realtek/datatrans_model.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/realtek/blob_transfer_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/realtek/firmware_distribution_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/realtek/firmware_update_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/utility/mesh_data_dump.c

ifeq ($(CONFIG_BT_MESH_PROVISIONER_SUPPORT), y)
CSRC += \
	$(BT_DIR)/rtk_stack/src/mesh/model/configuration_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_battery_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_default_transition_time_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_level_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_location_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_on_off_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_power_level_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_power_on_off_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_property_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_ctl_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_hsl_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_lc_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_lightness_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_xyl_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/scene_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/scheduler_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/time_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/sensor_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/health_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/directed_forwarding_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/private_beacon_client.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/subnet_bridge_client.c
endif

ifeq ($(CONFIG_BT_MESH_DEVICE_SUPPORT), y)
CSRC += \
	$(BT_DIR)/rtk_stack/src/mesh/model/delay_execution.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_battery_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_default_transition_time_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_level_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_location_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_location_setup_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_on_off_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_power_level_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_power_level_setup_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_power_on_off_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_power_on_off_setup_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_transition_time.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_user_property_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_admin_property_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_manufacturer_property_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/generic_client_property_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/health_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_ctl_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_ctl_setup_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_ctl_temperature_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_hsl_hue_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_hsl_saturation_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_hsl_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_hsl_setup_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_lc_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_lc_setup_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_lightness_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_lightness_setup_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_xyl_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/light_xyl_setup_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/scene_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/scene_setup_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/scheduler_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/scheduler_setup_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/time_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/time_setup_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/sensor_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/model/sensor_setup_server.c \
	$(BT_DIR)/rtk_stack/src/mesh/utility/mesh_data_dump.c
endif

OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                          RULES TO GENERATE TARGETS                          #
#*****************************************************************************#
# LIBBLUETOOTH_OBJS_PATH = $(shell pwd)/objs
# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS
	# mkdir -p $(LIBBLUETOOTH_OBJS_PATH)
	# for i in $(OBJS); do \
	# 	 $(COPY)  $$i $(LIBBLUETOOTH_OBJS_PATH) -f;\
	# 	done
	# $(REMOVE) *.i
	# $(REMOVE) *.s
	# $(REMOVE) *.d

#*****************************************************************************#
#                            GENERATE OBJECT FILE                             #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                           RULES TO CLEAN TARGETS                            #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
