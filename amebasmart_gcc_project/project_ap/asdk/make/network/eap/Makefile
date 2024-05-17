
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/wifi/wpa_supplicant
TARGET_LIB = lib_eap
vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

CSRC +=	$(DIR)/src/eap_common/eap_peap_common.c\

CSRC +=	$(DIR)/src/eap_peer/eap.c\
		$(DIR)/src/eap_peer/eap_methods.c\
		$(DIR)/src/eap_peer/eap_mschapv2.c\
		$(DIR)/src/eap_peer/eap_peap.c\
		$(DIR)/src/eap_peer/eap_tls.c\
		$(DIR)/src/eap_peer/eap_tls_common.c\
		$(DIR)/src/eap_peer/eap_ttls.c\
		$(DIR)/src/eap_peer/mschapv2.c\

CSRC +=	$(DIR)/src/crypto/des-internal.c\
		$(DIR)/src/crypto/md4-internal.c\
		$(DIR)/src/crypto/ms_funcs.c\
		$(DIR)/src/crypto/rc4.c\
		$(DIR)/src/crypto/sha1-internal.c\

CSRC +=	$(DIR)/src/utils/common.c\

CSRC += $(DIR)/wpa_supplicant/eap_protocol_handler.c\

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
