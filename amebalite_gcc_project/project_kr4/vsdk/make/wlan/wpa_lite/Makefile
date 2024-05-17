include $(MAKE_INCLUDE_GEN)
IFLAGS               += -I$(BASEDIR)/component/wifi/wpa_supplicant/wpa_lite
IFLAGS               += -I$(BASEDIR)/component/wifi/wpa_supplicant/wpa_lite/rom
.PHONY: lib clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/wifi/wpa_supplicant/wpa_lite
vpath %.c $(DIR) $(shell find $(DIR) -type d)

TARGET_LIB = lib_wpa_lite
TARGET_DIR = $(ROOTDIR)/lib/application/

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(TARGET_LIB)_git_version.c \

CSRC += $(DIR)/rom/rom_wpa_lite_psk.c \
	$(DIR)/wpa_lite_psk.c \
	$(DIR)/rom/rom_wpa_lite_part2.c \
	$(DIR)/rom/rom_wpa_lite_sae.c \
	$(DIR)/rom/rom_wpa_lite_sae_crypto.c \
	$(DIR)/wpa_lite_sae.c \
	$(DIR)/wpa_lite_owe.c \
	$(DIR)/wpa_lite_wnm.c \
	$(DIR)/wpa_lite_ft.c \

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
