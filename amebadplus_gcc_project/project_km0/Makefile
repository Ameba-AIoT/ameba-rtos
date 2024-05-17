include ../menuconfig/.config_km0

STDLIB_TYPE = NONE
stdlib:STDLIB_TYPE = DEFAULT
export STDLIB_TYPE

ROM_CFLAG = NONE
rom:ROM_CFLAG = YES
export ROM_CFLAG

IMAGE2_CFLAGS = NONE
all:IMAGE2_CFLAGS = YES
export IMAGE2_CFLAGS

.PHONY: all clean flash debug setup stdlib update_wlan_makefiles
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = 

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS
	make -C asdk image2

update_wlan_makefiles:
	mkdir -p ./asdk/make/wlan/
	cp -vrf ../../component/wifi/wifi_make/* ./asdk/make/wlan/

setup:
	make -C asdk $(MAKECMDGOALS)

flash:
	make -C asdk gdb

debug:
	make -C asdk debug

ifeq ($(CONFIG_FPGA),y)
rom: CORE_TARGETS stdlib
	make -f Makefile.rom -C asdk rom
else
rom: CORE_TARGETS
	@echo "Rom code freezing"
	@echo "do not need make rom"
endif

stdlib: CORE_TARGETS
	make -f Makefile.rom -C asdk std_lib

clean: CORE_TARGETS
	rm -rf ./asdk/make/wlan/
	make -C asdk clean

clean_rom: CORE_TARGETS
	make -f Makefile.rom -C asdk clean_rom

clean_all: CORE_TARGETS
	make -C asdk clean
	make -f Makefile.rom -C asdk clean_rom
	make -C asdk clean_all

menuconfig: CORE_TARGETS
	@echo " "
	#error: please use make menuconfig at AmebaDplus_GCC_example
	#error: any problem please contact eric_gao
	@echo " "
	@#make -C asdk menuconfig

gdb: CORE_TARGETS
	@echo "KM0 No Need to make gdb because KM4 will load KM0 Image!"
	@echo "KM0 Shall make all before KM4 Make gdb!"
	@#make -C asdk gdb #step2: load flash if need
	@#please use tools\AmebaZ\Image Tool!!!!!!!!!!!!!!!!!!
	@#user guide: WS_20160928_WLAN5_AmebaZ_Image_Tool_User_Guide_1v0

gdbloadrom: CORE_TARGETS
	make -f Makefile.rom -C asdk gdbloadrom #step1: load rom if need

swdpwd: CORE_TARGETS
	make -C asdk swdpwd

gdbboot: CORE_TARGETS
	make -C asdk gdbboot #boot from RAM
#
#gdbbootxip: CORE_TARGETS
#	make -C asdk gdbbootxip #step3: boot from flash
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: update_wlan_makefiles $(OBJS)

analysis:
	make -C asdk analysis_extract