include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
BT_DIR = $(BASEDIR)/component/bluetooth

vpath %.c $(BT_DIR)/example/ble_mesh_provisioner_scatternet \
		$(BT_DIR)/example/gatt_service/server \
		$(BT_DIR)/example/gatt_service/client

CSRC += \
	$(BT_DIR)/example/ble_mesh_provisioner_scatternet/mesh_provisioner_scatternet.c \
	$(BT_DIR)/example/gatt_service/client/rtk_bas_client.c \
	$(BT_DIR)/example/gatt_service/client/rtk_gaps_client.c \
	$(BT_DIR)/example/gatt_service/client/rtk_gcs_client.c \
	$(BT_DIR)/example/gatt_service/client/rtk_simple_ble_client.c \
	$(BT_DIR)/example/gatt_service/server/rtk_bas.c \
	$(BT_DIR)/example/gatt_service/server/rtk_dis.c \
	$(BT_DIR)/example/gatt_service/server/rtk_gls.c \
	$(BT_DIR)/example/gatt_service/server/rtk_hids_kb.c \
	$(BT_DIR)/example/gatt_service/server/rtk_hrs.c \
	$(BT_DIR)/example/gatt_service/server/rtk_ias.c \
	$(BT_DIR)/example/gatt_service/server/rtk_long_uuid_service.c \
	$(BT_DIR)/example/gatt_service/server/rtk_simple_ble_service.c

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
