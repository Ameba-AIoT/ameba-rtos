#include ../auto_build/config/.config_kr4
include ../menuconfig/.config_kr4

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
	make -C vsdk image2

update_wlan_makefiles:
	mkdir -p ./vsdk/make/wlan/
	cp -vrf ../../component/wifi/wifi_make/* ./vsdk/make/wlan/

setup:
	make -C vsdk $(MAKECMDGOALS)

flash:
	make -C vsdk gdb

debug:
	make -C vsdk debug

rom:
	@echo "Rom code freezing"
	@echo "do not need make rom"

#rom: CORE_TARGETS stdlib
#	make -f Makefile.rom -C vsdk rom
#	make -f Makefile.rom -C vsdk copy_rom

stdlib: CORE_TARGETS
	make -f Makefile.rom -C vsdk std_lib
	
clean: CORE_TARGETS
	rm -rf ./vsdk/make/wlan/
	make -C vsdk clean

clean_rom: CORE_TARGETS
	make -f Makefile.rom -C vsdk clean_rom
	
clean_all: CORE_TARGETS
	make -C vsdk clean_all
	

menuconfig: CORE_TARGETS
	@echo " "
	#error: please use make menuconfig at amebalite_gcc_project/
	#error: any problem please contact eric_gao
	@echo " "
	#make -C vsdk menuconfig

gdbloadrom: CORE_TARGETS
	make -f Makefile.rom -C vsdk gdbloadrom #step1: load rom if need

swdpwd: CORE_TARGETS
	make -C vsdk swdpwd

#image2: CORE_TARGETS
#	make -C vsdk image2

gdbboot: CORE_TARGETS
	make -C vsdk gdbboot #boot from RAM
#
#gdbbootxip: CORE_TARGETS
#	make -C vsdk gdbbootxip #step3: boot from flash
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: update_wlan_makefiles $(OBJS)

analysis:
	make -C vsdk analysis_extract

toolchain:
	make -C vsdk toolchain
