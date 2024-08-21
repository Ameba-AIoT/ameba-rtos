
include $(MAKE_INCLUDE_GEN)
.PHONY: all clean
#*****************************************************************************#
#                               VARIABLES                                     #
#*****************************************************************************#
DIR = $(BASEDIR)/component/wifi/wifi_feature_disable

vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
ifneq ($(CONFIG_AS_INIC_AP), y)
ifneq ($(CONFIG_WIFI_CSI_ENABLE),y)
CSRC += $(DIR)/wifi_feature_dis_csi.c
endif
ifneq ($(CONFIG_WIFI_ANTDIV_ENABLE),y)
CSRC += $(DIR)/wifi_feature_dis_antdiv.c
endif
ifneq ($(CONFIG_WIFI_SPEAKER_ENABLE),y)
CSRC += $(DIR)/wifi_feature_dis_speaker.c
endif
endif

ifneq ($(CONFIG_WIFI_11K_ENABLE)_$(CONFIG_WIFI_11V_ENABLE)_$(CONFIG_WIFI_11R_ENABLE), y_y_y)
CSRC += $(DIR)/wifi_feature_dis_kvr.c
endif
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))
#*****************************************************************************#
#                          RULES TO GENERATE TARGETS                          #
#*****************************************************************************#
# Define the Rules to build the core targets
all: CLEAN_OBJS COPY_NOUSED_API CORE_TARGETS COPY_RAM_OBJS

#*****************************************************************************#
#                             GENERATE OBJECT FILE                            #
#*****************************************************************************#
COPY_NOUSED_API:
ifeq ($(CONFIG_AS_INIC_NP), y)
ifneq ($(CONFIG_AMEBALITE), y)
ifneq ($(CONFIG_INIC_INTF_SDIO), y)
ifneq ($(CONFIG_INIC_INTF_SPI), y)
ifneq ($(CONFIG_INIC_INTF_USB), y)
	python ./gen_noused_c.py -a $(IMAGE_TARGET_FOLDER)/target_img2_otrcore.asm -c $(DIR)/wifi_intf_drv_to_app_ext_strong.c -r ./wifi_intf_drv_to_app_ext_noused.c
CSRC += ./wifi_intf_drv_to_app_ext_noused.c
endif
endif
endif
endif
endif
CORE_TARGETS:	$(OBJS)
#*****************************************************************************#
#                                RULES TO CLEAN TARGETS                       #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
