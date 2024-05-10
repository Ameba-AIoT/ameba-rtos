
include $(MAKE_INCLUDE_GEN)

GLOBAL_CFLAGS += -DCONFIG_WPS2

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/wifi/wpa_supplicant

vpath %.c $(DIR) $(shell find $(DIR) -type d)

TARGET_LIB = lib_wps
TARGET_DIR = $(ROOTDIR)/lib/application/

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(TARGET_LIB)_git_version.c \

CSRC +=	$(DIR)/src/crypto/crypto_internal-modexp.c \
		$(DIR)/src/crypto/dh_group5.c \
		$(DIR)/src/crypto/dh_groups.c \
		$(DIR)/src/crypto/random.c

CSRC +=	$(DIR)/src/eap_common/eap_common.c \
		$(DIR)/src/eap_common/eap_wsc_common.c		

CSRC +=	$(DIR)/src/eap_peer/eap_wsc.c

CSRC +=	$(DIR)/src/utils/os_freertos.c \
		$(DIR)/src/utils/wpabuf.c \
		$(DIR)/src/utils/wpa_debug.c

CSRC +=	$(DIR)/src/wps/wps.c \
		$(DIR)/src/wps/wps_attr_build.c \
		$(DIR)/src/wps/wps_attr_parse.c \
		$(DIR)/src/wps/wps_attr_process.c \
		$(DIR)/src/wps/wps_common.c \
		$(DIR)/src/wps/wps_dev_attr.c \
		$(DIR)/src/wps/wps_enrollee.c \
		$(DIR)/src/wps/wps_validate.c
		
CSRC +=	$(DIR)/wpa_supplicant/wps_protocol_handler.c

CSRC +=	$(DIR)/src/crypto/rom/rom_wps_aes-cbc.c \
		$(DIR)/src/crypto/rom/rom_wps_aes-internal.c \
		$(DIR)/src/crypto/rom/rom_wps_aes-internal-dec.c \
		$(DIR)/src/crypto/rom/rom_wps_aes-internal-enc.c \
		$(DIR)/src/crypto/rom/rom_wps_sha_256.c
CSRC += $(DIR)/src/tls/rom/rom_wps_big_num.c
CSRC += $(DIR)/src/utils/rom/rom_wps_os_freertos.c \

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
