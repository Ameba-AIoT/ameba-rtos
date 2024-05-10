
.PHONY: all clean menuconfig
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS =

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
km0_km4_app	:= $(CURRENT_PATH)/project_km4/asdk/image_mp/km0_km4_app_mp.bin
km4_boot_all	:= $(CURRENT_PATH)/project_km4/asdk/image_mp/km4_boot_all.bin
else
km0_km4_app	:= $(CURRENT_PATH)/project_km4/asdk/image/km0_km4_app.bin
km4_boot_all	:= $(CURRENT_PATH)/project_km4/asdk/image/km4_boot_all.bin
endif
ota_all		 := $(CURRENT_PATH)/project_km4/asdk/image/OTA_All.bin

# Define the Rules to build the core targets
all: REMOVE_TARGETS  KM4_TARGET_ALL KM0_TARGET_ALL
	@if [ -f  $(km4_boot_all) ]; \
		then cp -f $(km4_boot_all) ./; \
	else \
		echo "$(km4_boot_all) Build Fail" ; \
	fi

	@if [ -f  $(km0_km4_app) ]; \
		then cp -f $(km0_km4_app) ./; \
	else \
		echo "$(km0_km4_app) Build Fail" ; \
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
	$(CONFIG_SHELL) ./menuconfig/scripts/Menuconfig ./menuconfig/config.in
	@if [ ! -f  ./menuconfig/.config ]; then \
		echo; \
		echo "You have not saved your config, please re-run make config"; \
		echo; \
		exit 1; \
	fi

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