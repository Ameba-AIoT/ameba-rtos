.PHONY: all clean menuconfig
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS =

CONFIG_DISPLAY ?= 1
#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
CURRENT_PATH	:= $(shell pwd)

MENUCONFIG_ABS_PATH = $(CURRENT_PATH)/menuconfig
MENUCONFIG_RELA_PATH = $(shell realpath --relative-base=$(CURRENT_PATH) $(MENUCONFIG_ABS_PATH))
MENUCONFIG_NAME = $(shell basename $(MENUCONFIG_ABS_PATH))

#include ./menuconfig/.config
include $(MENUCONFIG_RELA_PATH)/.config
export MENUCONFIG_RELA_PATH MENUCONFIG_NAME

ifeq ($(CONFIG_MP_INCLUDED),y)

km4_boot_all	:= $(CURRENT_PATH)/project_hp/asdk/image_mp/km4_boot_all.bin
ifneq ($(CONFIG_LINUX_FW_EN),y)
app_all	:= $(CURRENT_PATH)/project_hp/asdk/image_mp/km0_km4_ca32_app_mp.bin
else
app_all	:= $(CURRENT_PATH)/project_hp/asdk/image_mp/km0_km4_app_mp.bin
endif

else

km4_boot_all	:= $(CURRENT_PATH)/project_hp/asdk/image/km4_boot_all.bin
km4_boot_all_sec	:= $(CURRENT_PATH)/project_hp/asdk/image/km4_boot_all_sec.bin
ifneq ($(CONFIG_LINUX_FW_EN),y)
app_all	:= $(CURRENT_PATH)/project_hp/asdk/image/km0_km4_ca32_app.bin
km0_image2_all_sec	:= $(CURRENT_PATH)/project_hp/asdk/image/km0_km4_ca32_app_sec.bin
else
app_all	:= $(CURRENT_PATH)/project_hp/asdk/image/km0_km4_app.bin
km0_image2_all_sec	:= $(CURRENT_PATH)/project_hp/asdk/image/km0_km4_app_sec.bin
endif
endif

ota_all			:= $(CURRENT_PATH)/project_hp/asdk/image/ota_all.bin

# Define the Rules to build the core targets
all: REMOVE_TARGETS AP_TARGET_ALL LP_TARGET_ALL HP_TARGET_ALL 
	@if [ -f  $(km4_boot_all) ]; \
		then cp -f $(km4_boot_all) ./; \
	else \
		echo "$(km4_boot_all) Build Fail" ; \
	fi

	@if [ -f  $(app_all) ]; \
		then cp -f $(app_all) ./; \
		echo "$(app_all) Build Success" ; \
	else \
		echo "$(app_all) Build Fail" ; \
	fi

	@if [ -f  $(ota_all) ]; \
		then cp -f $(ota_all) ./; \
	fi

ifneq ($(CONFIG_MP_INCLUDED),y)
	@if [ -f  $(km4_boot_all_sec) ]; \
		then cp -f $(km4_boot_all_sec) ./; \
	fi

	@if [ -f  $(km0_image2_all_sec) ]; \
		then cp -f $(km0_image2_all_sec) ./; \
	fi
endif

	@if [ -f  $(ota_all) ]; \
		then cp -f $(ota_all) ./; \
	fi

analysis:
	make -C ./project_hp analysis

clean: 
	make -C ./project_ap clean
	make -C ./project_hp clean
	make -C ./project_lp clean

	@rm -f ./*.bin

menuconfig:
	make -C ./menuconfig/scripts/lxdialog clean all
	$(CONFIG_SHELL) ./menuconfig/scripts/Menuconfig ./menuconfig/config.in $(CONFIG_DISPLAY)
	@if [ ! -f  ./menuconfig/.config ]; then \
		echo; \
		echo "You have not saved your config, please re-run make config"; \
		echo; \
		exit 1; \
	fi

toolchain:
	make -C ./project_hp toolchain

AP_TARGET_ALL:
ifneq ($(CONFIG_LINUX_FW_EN),y)
	make -C ./project_ap all
endif

HP_TARGET_ALL:
	make -C ./project_hp all EXAMPLE=

LP_TARGET_ALL:
	make -C ./project_lp all EXAMPLE=

gen_imgtool_floader:
	$(MAKE) -C ./project_hp gen_imgtool_floader
	@mv ./project_hp/asdk/image/imgtool_flashloader.bin 	./floader_$(RTLNAME).bin -f;

REMOVE_TARGETS:
	@rm -rf ./project_ap/asdk/image/*.bin
	@rm -rf ./project_lp/asdk/image/*.bin
	@rm -rf ./project_hp/asdk/image/*.bin
