
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
export MENUCONFIG_RELA_PATH MENUCONFIG_NAME IC_NAME

ifeq ($(CONFIG_MP_INCLUDED),y)
app_all	:= $(CURRENT_PATH)/project_km4/asdk/image_mp/km0_km4_app_mp.bin
ifeq ($(CONFIG_MP_SHRINK),y)
km4_boot_all	:= $(CURRENT_PATH)/project_km4/asdk/image_mp/km4_boot_all_mp.bin
else
km4_boot_all	:= $(CURRENT_PATH)/project_km4/asdk/image_mp/km4_boot_all.bin
endif
else
app_all	:= $(CURRENT_PATH)/project_km4/asdk/image/km0_km4_app.bin
km4_boot_all	:= $(CURRENT_PATH)/project_km4/asdk/image/km4_boot_all.bin
endif
ota_all		 := $(CURRENT_PATH)/project_km4/asdk/image/ota_all.bin

# Define the Rules to build the core targets
all: REMOVE_TARGETS  KM4_TARGET_ALL KM0_TARGET_ALL
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

analysis:
	make -C ./project_km0 analysis
	make -C ./project_km4 analysis

clean:
	make -C ./project_km0 clean
	make -C ./project_km4 clean

	@rm ./*.bin -rf

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
	make -C ./project_km4 toolchain

KM0_TARGET_ALL:
	make -C ./project_km0 all EXAMPLE=
KM4_TARGET_ALL:
	make -C ./project_km4 all

gen_imgtool_floader:
	$(MAKE) -C ./project_km4 gen_imgtool_floader
	@mv ./project_km4/asdk/image/imgtool_flashloader.bin 	./floader_$(RTLNAME).bin -f;

REMOVE_TARGETS:
	@rm -rf ./project_km0/asdk/image/*.bin
	@rm -rf ./project_km4/asdk/image/*.bin