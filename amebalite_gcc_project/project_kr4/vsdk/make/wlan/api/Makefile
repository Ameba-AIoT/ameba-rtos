
include $(MAKE_INCLUDE_GEN)


.PHONY: all clean

#*****************************************************************************#
#                                     VARIABLES                               #
#*****************************************************************************#
DIR = $(BASEDIR)/component/wifi
USRCFG_DIR = $(TARGETDIR)/usrcfg

vpath %.c $(DIR) $(USRCFG_DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC +=	$(DIR)/api/wifi_ind.c \
	$(DIR)/api/wifi_init.c \
	$(USRCFG_DIR)/ameba_wificfg.c

# AP
ifeq ($(CONFIG_AS_INIC_AP), y)
CSRC +=	$(DIR)/wpa_supplicant/wpa_supplicant/wifi_eap_config.c \
		$(DIR)/wpa_supplicant/wpa_supplicant/wifi_p2p_config.c \
		$(DIR)/wpa_supplicant/wpa_supplicant/wifi_wps_config.c \
		$(DIR)/wpa_supplicant/src/crypto/tls_polarssl.c \
		$(DIR)/wifi_fast_connect/wifi_fast_connect.c \
		$(DIR)/wifi_auto_reconnect/wifi_auto_reconnect.c

# Single core
else ifeq ($(CONFIG_SINGLE_CORE_WIFI), y)
CSRC +=	$(DIR)/wpa_supplicant/wpa_supplicant/wifi_eap_config.c \
		$(DIR)/wpa_supplicant/wpa_supplicant/wifi_p2p_config.c \
		$(DIR)/wpa_supplicant/wpa_supplicant/wifi_wps_config.c \
		$(DIR)/wpa_supplicant/src/crypto/tls_polarssl.c \
		$(DIR)/wifi_fast_connect/wifi_fast_connect.c \
		$(DIR)/wifi_auto_reconnect/wifi_auto_reconnect.c
endif

# NP and Single Core
ifneq ($(CONFIG_AS_INIC_AP), y)
CSRC += $(USRCFG_DIR)/ameba_wifi_power_table_usrcfg.c
CSRC += $(USRCFG_DIR)/ameba_wifi_country_code_table_usrcfg.c

ifeq ($(CONFIG_AMEBADPLUS), y)
CSRC += $(USRCFG_DIR)/ameba_wifi_power_table_usrcfg_1v8.c \
	   $(USRCFG_DIR)/ameba_wifi_power_table_usrcfg_3v3_1v25.c
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
all: COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)

-include $(DEPS)
