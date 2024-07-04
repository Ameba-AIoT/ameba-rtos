include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/bluetooth
BT_API_DIR = $(DIR)/api

vpath %.c $(DIR)/api

ifeq ($(CONFIG_BT_ZEPHYR), y)
vpath %.c $(BT_API_DIR)/zephyr_stack
else
vpath %.c $(BT_API_DIR)/rtk_stack
endif

CSRC += \
	$(BT_API_DIR)/rtk_bt_common.c \
	$(BT_API_DIR)/rtk_bt_device.c \
	$(BT_API_DIR)/rtk_bt_le_gap.c \
	$(BT_API_DIR)/rtk_bt_gap.c \
	$(BT_API_DIR)/rtk_bt_gatt.c \
	$(BT_API_DIR)/rtk_bt_vendor.c \
	$(BT_API_DIR)/rtk_bt_mp_api.c \
	$(BT_API_DIR)/rtk_bt_power_control.c \
	$(BT_API_DIR)/rtk_bt_mesh_common.c \
	$(BT_API_DIR)/rtk_bt_mesh_config_client_model.c \
	$(BT_API_DIR)/rtk_bt_mesh_datatrans_model.c \
	$(BT_API_DIR)/rtk_bt_mesh_generic_model.c \
	$(BT_API_DIR)/rtk_bt_mesh_generic_onoff_model.c \
	$(BT_API_DIR)/rtk_bt_mesh_generic_default_transition_time.c \
	$(BT_API_DIR)/rtk_bt_mesh_light_model.c \
	$(BT_API_DIR)/rtk_bt_mesh_time_model.c \
	$(BT_API_DIR)/rtk_bt_mesh_scheduler_model.c \
	$(BT_API_DIR)/rtk_bt_mesh_scene_model.c \
	$(BT_API_DIR)/rtk_bt_mesh_remote_prov_model.c \
	$(BT_API_DIR)/rtk_bt_mesh_health_model.c \
	$(BT_API_DIR)/rtk_bt_mesh_sensor_model.c

ifeq ($(CONFIG_BT_ZEPHYR), y)
CSRC += \
	$(BT_API_DIR)/zephyr_stack/zephyr_stack_api.c \
	$(BT_API_DIR)/zephyr_stack/zephyr_stack_resource.c \
	$(BT_API_DIR)/zephyr_stack/zephyr_stack_le_gap.c \
	$(BT_API_DIR)/zephyr_stack/zephyr_stack_gattc.c \
	$(BT_API_DIR)/zephyr_stack/zephyr_stack_gatts.c
else
CSRC += \
	$(BT_API_DIR)/rtk_stack/rtk_stack_api.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_le_gap.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_gap.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_gattc.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_gatts.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_vendor.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_mesh_common.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_mesh_config_client_model.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_mesh_datatrans_model.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_mesh_remote_prov_model.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_mesh_generic_onoff_model.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_mesh_light_model.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_mesh_time_model.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_mesh_scheduler_model.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_mesh_scene_model.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_mesh_generic_default_transition_time.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_mesh_generic_model.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_mesh_health_model.c \
	$(BT_API_DIR)/rtk_stack/rtk_stack_mesh_sensor_model.c
endif

OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
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
#                         GENERATE OBJECT FILE                                #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)