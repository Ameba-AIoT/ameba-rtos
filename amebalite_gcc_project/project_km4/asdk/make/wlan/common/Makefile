include $(MAKE_INCLUDE_GEN)
IFLAGS               += -I$(BASEDIR)/component/wifi/common
IFLAGS               += -I$(BASEDIR)/component/wifi/common/secure

.PHONY: lib clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
COMDIR = $(BASEDIR)/component/wifi/common
SECDIR = $(BASEDIR)/component/wifi/common/secure
HALCOMDIR = $(BASEDIR)/component/wifi/driver/halcommon_rom
vpath %.c $(COMDIR) $(SECDIR) $(HALCOMDIR) $(shell find $(SECDIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
# The following files are needed by all cores except dplus km0
# no need in green2 ap and np (share rom)
CSRC_SEC += $(SECDIR)/rom_sha1.c \
	$(SECDIR)/rom_md5.c \
	$(SECDIR)/rom_aes.c \
	$(SECDIR)/rom_rtw_sha256.c \
	$(SECDIR)/rom_rtw_dh_groups.c \
	$(SECDIR)/rom_arc4.c \
	$(SECDIR)/rom_rc4.c \
	$(SECDIR)/rom_rtw_mbedtls_bignum.c \
	$(SECDIR)/rom_rtw_mbedtls_ecp.c \
	$(SECDIR)/rom_rtw_mbedtls_ecp_curves.c \
	$(SECDIR)/rom_rtw_mbedtls_platform.c \
	$(SECDIR)/rom_rtw_mbedtls_func_map.c \
	$(SECDIR)/rom_rtw_security.c \
	$(TARGET_LIB)_git_version.c

CSRC_ROM = 	$(COMDIR)/rom_rtw_queue.c
CSRC_ROM += $(HALCOMDIR)/rom_hal_rom_to_flash.c

# The following files are needed by network cores/single core
# only CSRC_COM in green2
CSRC_COM += $(COMDIR)/rtw_timer.c \
	$(COMDIR)/rtw_wakelock.c \
	$(COMDIR)/rtw_atomic.c

CSRC += $(CSRC_COM)
TARGET_LIB = lib_wifi_common

ifeq ($(CONFIG_AMEBAGREEN2),y)
TARGET_LIB = lib_wifi_common
else
CSRC += $(CSRC_ROM)
ifeq ($(CONFIG_AS_INIC_AP), y)
CSRC += $(CSRC_SEC)
TARGET_LIB = lib_wifi_com_sec
else
ifneq ($(CONFIG_AMEBADPLUS), y)
CSRC += $(CSRC_SEC)
TARGET_LIB = lib_wifi_com_sec
endif
endif
endif
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
lib: COPY_RAM_OBJS
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))
	$(REMOVE) $(TARGET_LIB)_git_version.*
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              GENERATE GIT VERSION FILE                                      #
#*****************************************************************************#
$(TARGET_LIB)_git_version.c:
	$(call GEN_GIT_VER, $(TARGET_LIB))

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)

-include $(DEPS)
