
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
XM_DIR = $(BASEDIR)/component/soc/common/flash_download/rt_xmodem
USBD_DIR = $(BASEDIR)/component/soc/common/flash_download/usb_download
USB_COM_DIR = $(BASEDIR)/component/usb/common
RAM_COM_DIR = $(TARGETDIR)/fwlib/ram_common
LIB_RAM_COM_DIR = $(TARGETDIR)/lib/ram_common
RAM_HP_DIR = $(TARGETDIR)/fwlib/ram_hp
HAL_DIR = $(TARGETDIR)/hal
HAL_SRC_DIR = $(TARGETDIR)/hal/src
TAR_DIR = $(BASEDIR)/component/soc/common/imgtool_floader/src
SWLIB_DIR = $(TARGETDIR)/swlib

MODULE_IFLAGS += -I$(TARGETDIR)/app/xmodem \
		 -I$(TARGETDIR)/fwlib/include \
		 -I$(BASEDIR)/component/mbed/hal \
		 -I$(HAL_DIR)/include \
		 -I$(HAL_DIR)/src \
		 -I$(XM_DIR) \
		 -I$(SWLIB_DIR) \
		 -I$(USB_COM_DIR) \
		 -I$(USBD_DIR) \
		 -I$(TAR_DIR)/../inc

GLOBAL_CFLAGS += -DCONFIG_USB_DOWNLOAD_EN -DCONFIG_USB_DOWNLOAD_IND_EN -DCONFIG_NAND_DOWNLOAD_EN -DCONFIG_FLOADER_USBD_EN -DCONFIG_USB_NONOS

vpath %.c $(XM_DIR) $(USBD_DIR) $(USB_COM_DIR) $(RAM_COM_DIR) $(LIB_RAM_COM_DIR) $(HAL_SRC_DIR) $(RAM_HP_DIR) $(SWLIB_DIR) $(TAR_DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(XM_DIR)/floader_fw_update.c \
	$(XM_DIR)/floader_xmodem.c \
	$(RAM_COM_DIR)/ameba_reset.c \
	$(RAM_COM_DIR)/ameba_otp.c \
	$(RAM_COM_DIR)/ameba_ipc_api.c \
	$(RAM_COM_DIR)/ameba_tim.c \
	$(LIB_RAM_COM_DIR)/ameba_rom_patch.c \
	$(RAM_COM_DIR)/ameba_usb.c \
	$(RAM_HP_DIR)/ram_hp/ameba_codec.c \
	$(SWLIB_DIR)/log.c \
	$(TAR_DIR)/ameba_floader_rom_parse.c \
	$(HAL_SRC_DIR)/pinmap_common.c \
	$(HAL_SRC_DIR)/pinmap.c \
	$(HAL_SRC_DIR)/gpio_api.c \
	$(HAL_SRC_DIR)/pwmout_api.c \
	$(USBD_DIR)/usbd_cdc.c \
	$(USBD_DIR)/usbd_core.c \
	$(USBD_DIR)/usbd_dbg.c \
	$(USBD_DIR)/usbd_hal.c \
	$(USBD_DIR)/usbd_intf.c \
	$(USBD_DIR)/usbd_pcd.c \
	$(USB_COM_DIR)/usb_hal.c \
	$(USB_COM_DIR)/usb_os.c \
	$(TAR_DIR)/floader_nand_ftl.c \
	$(TAR_DIR)/floader_nand_ftl_mfg.c \
	$(TAR_DIR)/floader_nor_ftl.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS

	
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
