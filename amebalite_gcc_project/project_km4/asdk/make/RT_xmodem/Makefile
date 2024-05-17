
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
XM_DIR = $(BASEDIR)/component/soc/common/flash_download/rt_xmodem
RAM_COM_DIR = $(TARGETDIR)/fwlib/ram_common
DRVDIR =  $(TARGETDIR)/fwlib/ram_km4
TAR_DIR = $(TARGETDIR)/imgtool_floader/src
SWLIB_DIR = $(TARGETDIR)/swlib

MODULE_IFLAGS += -I$(TARGETDIR)/app/xmodem \
		 -I$(TARGETDIR)/fwlib/include \
		 -I$(SWLIB_DIR) \
		 -I$(XM_DIR) \
		 -I$(TAR_DIR)/../include \
		 -I$(FREERTOSDIR)/include \
		 -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/non_secure \
		 -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/secure

vpath %.c $(XM_DIR) $(RAM_COM_DIR) $(SWLIB_DIR) $(TAR_DIR) $(DRVDIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(XM_DIR)/floader_fw_update.c \
	$(XM_DIR)/floader_xmodem.c \
	$(RAM_COM_DIR)/ameba_otp.c \
	$(RAM_COM_DIR)/ameba_ipc_ram.c \
	$(DRVDIR)/ameba_arch.c \
	$(RAM_COM_DIR)/ameba_reset.c \
	$(SWLIB_DIR)/log.c \
	$(TAR_DIR)/floader_nor_ftl.c \
	$(TAR_DIR)/ameba_floader.c \
	$(TAR_DIR)/ameba_floader_rom_parse.c

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
