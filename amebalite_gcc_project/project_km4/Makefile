#include ../auto_build/config/.config_km4
include ../menuconfig/.config_km4

STDLIB_TYPE = NONE
stdlib:STDLIB_TYPE = DEFAULT
export STDLIB_TYPE

IMG3_TYPE = NONE
image3:IMG3_TYPE = DEFAULT
export IMG3_TYPE

ROM_CFLAG = NONE
rom_debug_if_need:ROM_CFLAG = YES
rom:ROM_CFLAG = YES
export ROM_CFLAG

IMAGE2_CFLAGS = NONE
image2:IMAGE2_CFLAGS = YES
lib_img2:IMAGE2_CFLAGS = YES
export IMAGE2_CFLAGS

.PHONY: all clean image2 flash debug setup stdlib image3 update_wlan_makefiles

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = 

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets

all: CORE_TARGETS boot image3 image2

update_wlan_makefiles:
	mkdir -p ./asdk/make/wlan/
	cp -vrf ../../component/wifi/wifi_make/* ./asdk/make/wlan/

boot: CORE_TARGETS lib_boot
	$(MAKE) -C asdk bootloader

image2: CORE_TARGETS lib_img2
	$(MAKE) -C asdk image2_ns

setup:
	$(MAKE) -C asdk $(MAKECMDGOALS)

flash:
	$(MAKE) -C asdk gdb

debug:
	$(MAKE) -C asdk debug

lib_img2: update_wlan_makefiles CORE_TARGETS image3

lib_boot: CORE_TARGETS

flash_loader:
	$(MAKE) -C asdk flashloader

image3: CORE_TARGETS
	$(MAKE) -C asdk image3_s

#rom: CORE_TARGETS stdlib
#	$(MAKE) -f Makefile.rom -C asdk rom_ns
#	$(MAKE) -f Makefile.rom -C asdk rom
#	$(MAKE) -f Makefile.rom -C asdk copy_rom

#stdlib: CORE_TARGETS
#	$(MAKE) -f Makefile.rom -C asdk std_lib
rom:
	@echo "Rom code freezing"
	@echo "do not need make rom"
#	@echo "use make rom_debug_if_need if really need make rom"

clean_rom: CORE_TARGETS
	$(MAKE) -f Makefile.rom -C asdk clean_rom
	
clean: CORE_TARGETS
	rm -rf ./asdk/make/wlan/
	$(MAKE) -C asdk clean

clean_all: CORE_TARGETS
	$(MAKE) -C asdk clean_all

menuconfig: CORE_TARGETS
	@echo " "
	#error: please use make menuconfig at amebalite_gcc_project/
	#error: any problem please contact eric_gao
	@echo " "
	#$(MAKE) -C asdk menuconfig

gdb: CORE_TARGETS
	$(MAKE) -C asdk gdb #step2: load flash if need
	@#please use tools\AmebaZ\Image Tool!!!!!!!!!!!!!!!!!!
	@#user guide: WS_20160928_WLAN5_AmebaZ_Image_Tool_User_Guide_1v0

gdbloadrom: CORE_TARGETS
	$(MAKE) -f Makefile.rom -C asdk gdbloadrom #step1: load rom if need

imgtool_flashloader: lib_boot
	make -C asdk imgtool_flashloader

gdbboot: CORE_TARGETS
	$(MAKE) -C asdk gdbboot #boot from RAM

gen_imgtool_floader: 
	$(MAKE) -C asdk gen_imgtool_floader
#gdbbootxip: CORE_TARGETS
#	$(MAKE) -C asdk gdbbootxip #step3: boot from flash
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

analysis:
	$(MAKE) -C asdk analysis_extract

toolchain:
	$(MAKE) -C asdk toolchain
