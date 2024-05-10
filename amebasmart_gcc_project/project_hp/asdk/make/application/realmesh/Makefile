
include $(MAKE_INCLUDE_GEN)


.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/wifi/realmesh
MODULE_IFLAGS               += -I$(BASEDIR)/component/wifi/driver/mesh
MODULE_IFLAGS               += -I$(BASEDIR)/component/wifi/realmesh/include

vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC +=		$(DIR)/src/rmesh.c \
		$(DIR)/src/rmesh_frame.c \
		$(DIR)/src/rmesh_route.c \
		$(DIR)/src/rmesh_scan.c \
		$(DIR)/src/rmesh_connect.c \
		$(DIR)/src/rmesh_data_forward.c \
		$(DIR)/src/rmesh_event_cb.c \
		$(DIR)/src/rmesh_topology.c \
		$(DIR)/src/rmesh_keep_alive.c \
		$(DIR)/src/rmesh_light_application.c \
		$(DIR)/src/rmesh_root_socket.c \
		$(DIR)/test/rmesh_test.c \
		$(DIR)/src/rmesh_config.c  \
		$(DIR)/rmesh_bt_config/rmesh_bt_config_app_main.c  \
		$(DIR)/rmesh_bt_config/rmesh_bt_config_app_task.c  \
		$(DIR)/rmesh_bt_config/rmesh_bt_config_peripheral_app.c  \
		$(DIR)/rmesh_bt_config/rmesh_bt_config_service.c  \
		$(DIR)/rmesh_bt_config/rmesh_bt_config_wifi.c  \
		$(DIR)/rmesh_bt_config/rmesh_bt_config_wifi_lib.c  \
		$(DIR)/src/rmesh_broadcast.c \

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
