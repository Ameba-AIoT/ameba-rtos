include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/bluetooth/example
MODULE_IFLAGS += -I$(BASEDIR)/component/at_cmd

vpath %.c $(DIR)/bt_atcmd

MODULE_IFLAGS += \
	-I$(BASEDIR)/component/at_cmd

CSRC += \
	$(DIR)/bt_atcmd/bt_utils.c \
	$(DIR)/bt_atcmd/atcmd_bt_common.c \
	$(DIR)/bt_atcmd/atcmd_bt_device.c \
	$(DIR)/bt_atcmd/atcmd_bt_example.c \
	$(DIR)/bt_atcmd/atcmd_bt_gap.c \
	$(DIR)/bt_atcmd/atcmd_bt_le_gap.c \
	$(DIR)/bt_atcmd/atcmd_bt_gatts.c \
	$(DIR)/bt_atcmd/atcmd_bt_gattc.c \
	$(DIR)/bt_atcmd/atcmd_bt_vendor.c \
	$(DIR)/bt_atcmd/atcmd_bt_mesh_common.c \
	$(DIR)/bt_atcmd/atcmd_bt_mesh_config_model.c \
	$(DIR)/bt_atcmd/atcmd_bt_mesh_datatrans_model.c \
	$(DIR)/bt_atcmd/atcmd_bt_mesh_goo_model.c \
	$(DIR)/bt_atcmd/atcmd_bt_mesh_remote_prov_model.c \
	$(DIR)/bt_atcmd/atcmd_bt_mesh_light_model.c \
	$(DIR)/bt_atcmd/atcmd_bt_mesh_time_model.c \
	$(DIR)/bt_atcmd/atcmd_bt_mesh_scheduler_model.c \
	$(DIR)/bt_atcmd/atcmd_bt_mesh_scene_model.c \
	$(DIR)/bt_atcmd/atcmd_bt_mesh_generic_default_transition_time.c \
	$(DIR)/bt_atcmd/atcmd_bt_mesh_generic_model.c \
	$(DIR)/bt_atcmd/atcmd_bt_mesh_sensor_model.c \
	$(DIR)/bt_atcmd/atcmd_bt_mesh_health_model.c

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
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)