include ../menuconfig/.config_lp

IMAGE2_CFLAGS = NONE
all:IMAGE2_CFLAGS = YES
export IMAGE2_CFLAGS
.PHONY: all clean flash debug setup
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

setup:
	make -C asdk $(MAKECMDGOALS)

debug:
	make -C asdk debug

rom:
	@echo "Rom code freezing"
	@echo "do not need make rom"

#rom: CORE_TARGETS
#ifeq ($(CONFIG_RLE1080_A_CUT),y)
	#@echo "RLE1080A ROM code freezed. Make rom is not allowed"
#else
#	make -f Makefile.rom -C asdk rom
#	make -f Makefile.rom -C asdk copy_rom
#endif

clean: CORE_TARGETS
	make -C asdk clean

clean_rom: CORE_TARGETS
	make -f Makefile.rom -C asdk clean_rom

clean_all: clean_rom CORE_TARGETS
	make -C asdk clean_all

menuconfig: CORE_TARGETS
	@echo " "
	#error: please use make menuconfig at amebasmart_gcc_project
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

#image2: CORE_TARGETS
#	make -C asdk image2
#
#bootloader: CORE_TARGETS
#	make -C asdk bootloader
#
gdbboot: CORE_TARGETS
	make -C asdk gdbboot #boot from RAM
#
#gdbbootxip: CORE_TARGETS
#	make -C asdk gdbbootxip #step3: boot from flash
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

