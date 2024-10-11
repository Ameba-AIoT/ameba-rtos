
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
FL_DIR = $(BASEDIR)/component/soc/common/flash_download/imgtool_floader
USB_COM_DIR = $(BASEDIR)/component/usb/common
RAM_COM_DIR = $(TARGETDIR)/fwlib/ram_common
LIB_RAM_COM_DIR = $(TARGETDIR)/lib/ram_common
RAM_HP_DIR = $(TARGETDIR)/fwlib/ram_hp
HAL_DIR = $(TARGETDIR)/hal
HAL_SRC_DIR = $(TARGETDIR)/hal/src
FTL_DIR = $(BASEDIR)/component/soc/common/flash_wrapper/src
SWLIB_DIR = $(TARGETDIR)/swlib

MODULE_IFLAGS += -I$(TARGETDIR)/app/xmodem \
		 -I$(TARGETDIR)/fwlib/include \
		 -I$(BASEDIR)/component/mbed/hal \
		 -I$(HAL_DIR)/include \
		 -I$(HAL_DIR)/src \
		 -I$(SWLIB_DIR) \
		 -I$(USB_COM_DIR) \
		 -I$(FL_DIR) \
		 -I$(FTL_DIR)/../inc

GLOBAL_CFLAGS += -DCONFIG_USB_DOWNLOAD_EN -DCONFIG_DOWNLOAD_IND_EN -DCONFIG_NAND_DOWNLOAD_EN -DCONFIG_FLOADER_USBD_EN

vpath %.c $(FL_DIR) $(USB_COM_DIR) $(RAM_COM_DIR) $(LIB_RAM_COM_DIR) $(HAL_SRC_DIR) $(RAM_HP_DIR) $(SWLIB_DIR) $(FTL_DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = 	$(RAM_COM_DIR)/ameba_reset.c \
	$(RAM_COM_DIR)/ameba_otp.c \
	$(RAM_COM_DIR)/ameba_ipc_api.c \
	$(RAM_COM_DIR)/ameba_tim.c \
	$(LIB_RAM_COM_DIR)/ameba_rom_patch.c \
	$(RAM_COM_DIR)/ameba_usb.c \
	$(RAM_HP_DIR)/ameba_codec.c \
	$(SWLIB_DIR)/log.c \
	$(USB_COM_DIR)/usb_hal.c \
	$(USB_COM_DIR)/usb_os.c \
	$(FL_DIR)/usbd_cdc.c \
	$(FL_DIR)/usbd_core.c \
	$(FL_DIR)/usbd_hal.c \
	$(FL_DIR)/usbd_intf.c \
	$(FL_DIR)/usbd_pcd.c \
	$(FL_DIR)/loguart_intf.c \
	$(FL_DIR)/floader_debug.c \
	$(FL_DIR)/floader_fb.c \
	$(FL_DIR)/floader_intf.c \
	$(FL_DIR)/floader.c \
	$(FTL_DIR)/ameba_nand_ftl.c \
	$(FTL_DIR)/ameba_nand_mfg_ftl.c \
	$(FTL_DIR)/ameba_nor_ftl.c

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
