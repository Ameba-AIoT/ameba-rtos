#include ../auto_build/config/.config_ap
include ../menuconfig/.config_ap

IMAGE2_CFLAGS = NONE
all:IMAGE2_CFLAGS = YES
export IMAGE2_CFLAGS
.PHONY: all clean bootloader debug setup clean_all update_wlan_makefiles
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS =

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS boot lib_img2
	make -C asdk image2

update_wlan_makefiles:
	mkdir -p ./asdk/make/wlan/
	cp -vrf ../../component/wifi/wifi_make/* ./asdk/make/wlan/

bootloader: CORE_TARGETS
	make -C asdk bootloader

image2: CORE_TARGETS
	make -C asdk image2

setup:
	make -C asdk $(MAKECMDGOALS)

debug:
	make -C asdk debug

debug_core0:
	make -C asdk debug_core0

debug_core1:
	make -C asdk debug_core1

boot: CORE_TARGETS
	make -C asdk bootloader

lib_img2: CORE_TARGETS

clean: CORE_TARGETS
	rm -rf ./asdk/make/wlan/
	make -C asdk clean

clean_all: CORE_TARGETS
	make -C asdk clean
	make -C asdk clean_all

menuconfig: CORE_TARGETS
	@echo " "
	#error: please use make menuconfig at amebasmart_gcc_project/
	#error: any problem please contact eric_gao
	@echo " "
	@#make -C asdk menuconfig

swdpwd: CORE_TARGETS
	make -C asdk swdpwd

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: update_wlan_makefiles $(OBJS)

