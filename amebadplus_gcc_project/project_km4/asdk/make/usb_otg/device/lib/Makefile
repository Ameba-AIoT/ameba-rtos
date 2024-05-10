
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
HAL_DIR = $(TARGETDIR)/fwlib/ram_km4
DRV_DIR = $(BASEDIR)/component/usb
COM_DIR = $(DRV_DIR)/common
LIB_DIR = $(DRV_DIR)/device/core
MODULE_IFLAGS += -I$(COM_DIR)
MODULE_IFLAGS += -I$(LIB_DIR)
GLOBAL_CFLAGS += -DCONFIG_USB_FS
TARGET_LIB = lib_usbd

vpath %.c $(HAL_DIR) $(COM_DIR) $(LIB_DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC += $(HAL_DIR)/ameba_usb.c \
	$(COM_DIR)/usb_hal.c \
	$(COM_DIR)/usb_os.c \
	$(LIB_DIR)/usbd.c \
	$(LIB_DIR)/usbd_core.c \
	$(LIB_DIR)/usbd_hal.c \
	$(LIB_DIR)/usbd_pcd.c \
	
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))
	
#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
# Define the Rules to build the core targets
all: CORE_TARGETS
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))
	
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
