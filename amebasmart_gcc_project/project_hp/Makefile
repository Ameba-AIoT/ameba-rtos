include ../menuconfig/.config_hp

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

.PHONY: all clean flash debug setup stdlib image3 update_wlan_makefiles

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

image2: CORE_TARGETS lib_img2
	$(MAKE) -C asdk image2_ns

image3: CORE_TARGETS
	$(MAKE) -C asdk image3_s

setup:
	$(MAKE) -C asdk $(MAKECMDGOALS)

flash:
	$(MAKE) -C asdk gdb

debug:
	$(MAKE) -C asdk debug

boot: CORE_TARGETS lib_boot
	$(MAKE) -C asdk bootloader

lib_boot:

lib_img2: update_wlan_makefiles CORE_TARGETS image3

flash_loader:
	$(MAKE) -C asdk flashloader

rom:
	@echo "Rom code freezing"
	@echo "do not need make rom"
#	@echo "use make rom_debug_if_need if really need make rom"

#rom: CORE_TARGETS stdlib
#ifeq ($(CONFIG_RLE1080_A_CUT),y)
	#@echo "RLE1080A ROM code freezed. Make rom is not allowed"
#else
#	$(MAKE) -f Makefile.rom -C asdk rom_ns
#	$(MAKE) -f Makefile.rom -C asdk rom
#	$(MAKE) -f Makefile.rom -C asdk copy_rom
#endif

stdlib: CORE_TARGETS
ifeq ($(CONFIG_RLE1080_A_CUT),y)
	@echo "RLE1080A ROM code freezed. Make rom is not allowed"
else
	$(MAKE) -f Makefile.rom -C asdk std_lib
endif

clean: CORE_TARGETS
	rm -rf ./asdk/make/wlan/
	$(MAKE) -C asdk clean

clean_rom: CORE_TARGETS
	$(MAKE) -f Makefile.rom -C asdk clean_rom

clean_all: clean_rom CORE_TARGETS
	make -C asdk clean_all

menuconfig: CORE_TARGETS
	@echo " "
	#error: please use make menuconfig at amebasmart_gcc_project
	#error: any problem please contact eric_gao
	@echo " "
	@#$(MAKE) -C asdk menuconfig

gdb: CORE_TARGETS
	$(MAKE) -C asdk gdb #step2: load flash if need
	@#please use tools\AmebaZ\Image Tool!!!!!!!!!!!!!!!!!!
	@#user guide: WS_20160928_WLAN5_AmebaZ_Image_Tool_User_Guide_1v0

gdbloadrom: CORE_TARGETS
	$(MAKE) -f Makefile.rom -C asdk gdbloadrom #step1: load rom if need

imgtool_flashloader: lib_boot
	make -C asdk imgtool_flashloader

gen_imgtool_floader:
	$(MAKE) -C asdk gen_imgtool_floader

#not use now
#bootloader: CORE_TARGETS
#	$(MAKE) -C asdk bootloader
#img2: CORE_TARGETS
#	$(MAKE) -C asdk image2_ns
#img3: CORE_TARGETS
#	$(MAKE) -C asdk image3_s
#clean_xip: CORE_TARGETS
#	$(MAKE) -C asdk clean_xip
#
gdbboot: CORE_TARGETS
	$(MAKE) -C asdk gdbboot #boot from RAM
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
