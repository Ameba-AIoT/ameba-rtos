
#*****************************************************************************#
#                            Basic Configurations                             #
#*****************************************************************************#

ABS_ROOTDIR		= $(shell pwd)
MAKE_INCLUDE_GEN	= $(ABS_ROOTDIR)/../../Makefile.include.gen.app

CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
          else if [ -x /bin/bash ]; then echo /bin/bash; \
          else echo sh; fi ; fi)

include $(MAKE_INCLUDE_GEN)

Q:=@
ECHO:=@echo

export ABS_ROOTDIR MAKE_INCLUDE_GEN HOSTCC Q ECHO

# Target can be anything you want. It will create an elf and bin file
# with this name
TARGET = target_apps_1

###############################################################################
#
# Lets work out what the user wants, and if they have configured us yet
#

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#

APPDIR = $(MODDIR)/apps
APPCOMMONDIR = $(MODDIR)/apps/common
APPTESTDIR = $(ABS_ROOTDIR)
POSIXDIR = $(BASEDIR)/component/os/freertos/freertos_posix
MODULE_IFLAGS += -I$(MODDIR)/sys_module
MODULE_IFLAGS += -I$(APPCOMMONDIR)
MODULE_IFLAGS += -I$(APPTESTDIR)
MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/ARM_CA7
MODULE_IFLAGS += -I$(POSIXDIR)/include
MODULE_IFLAGS += -I$(POSIXDIR)/FreeRTOS-Plus-POSIX/include
MODULE_IFLAGS += -I$(POSIXDIR)/FreeRTOS-Plus-POSIX/include/portable/realtek

vpath %.c $(APPCOMMONDIR) $(APPTESTDIR) $(shell find $(APPTESTDIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

# Don't modify
CSRC +=	$(APPCOMMONDIR)/app_syscall_posix.c
CSRC +=	$(APPCOMMONDIR)/app_syscall_socket.c
CSRC +=	$(APPCOMMONDIR)/app_syscall_vfs.c
CSRC +=	$(APPCOMMONDIR)/app_syscall_stdc.c
CSRC +=	$(APPCOMMONDIR)/app_start.c
# User files
CSRC +=	$(APPTESTDIR)/app_main.c
CSRC +=	$(APPTESTDIR)/app_socket.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)


.PHONY: all clean

LINK_APPS_LIB =
LINK_APPS_LIB += $(TARGET_LIBAPP_FOLDER)/lib_syscall.a

all: make_subdirs_apps linker_apps

make_subdirs_apps:
	@+$(MAKE) -C ../../sys_module all

#*****************************************************************************#
#                      RULES TO LINKER IMAGE                                  #
#*****************************************************************************#
linker_apps: CORE_TARGETS
	@echo "========= linker apps start ========="
	$(ECHO) "  LD      $(TARGET).axf"
	$(LD) $(LD_PIC_ARG) $(LD_ARG) $(TARGET).axf $(OBJS) $(ASM_OBJS) $(LINK_APPS_LIB) \
	$(SDK_TOOLCHAIN)/lib/gcc/arm-none-eabi/$(ASDK_VER)/v7-a+neon-fp16/fpu/crtbegin.o \
	$(SDK_TOOLCHAIN)/lib/gcc/arm-none-eabi/$(ASDK_VER)/v7-a+neon-fp16/fpu/crtend.o -lm -lstdc++

	$(ECHO) "  LD      $(TARGET).axf done"

	$(Q)$(MOVE) text.map $(APPS_IMAGE_TARGET_FOLDER)/text_apps.map
	$(Q)$(MOVE) $(TARGET).axf $(APPS_IMAGE_TARGET_FOLDER)
	$(Q)$(NM) $(APPS_IMAGE_TARGET_FOLDER)/$(TARGET).axf | sort > $(APPS_IMAGE_TARGET_FOLDER)/$(TARGET).map
	$(Q)$(OBJDUMP) -Dz $(APPS_IMAGE_TARGET_FOLDER)/$(TARGET).axf > $(APPS_IMAGE_TARGET_FOLDER)/$(TARGET).asm

	# @echo "========== Image Info HEX =========="
	# $(Q)$(CC_SIZE) -A --radix=16 $(APPS_IMAGE_TARGET_FOLDER)/$(TARGET).axf
	# $(Q)$(CC_SIZE) -t --radix=16 $(APPS_IMAGE_TARGET_FOLDER)/$(TARGET).axf
	# @echo "========== Image Info HEX =========="

	# @echo "========== Image Info DEC =========="
	# $(Q)$(CC_SIZE) -A --radix=10 $(APPS_IMAGE_TARGET_FOLDER)/$(TARGET).axf
	# $(Q)$(CC_SIZE) -t --radix=10 $(APPS_IMAGE_TARGET_FOLDER)/$(TARGET).axf
	# @echo "========== Image Info DEC =========="
	
	$(ELF2TINF) $(APPS_IMAGE_TARGET_FOLDER)/$(TARGET).axf $(TARGET)
	$(COPY) $(APPS_IMAGE_TARGET_FOLDER)/$(TARGET)_tinf.h $(MODDIR)/app_loader/$(TARGET)_tinf.h
	$(COPY) $(APPS_IMAGE_TARGET_FOLDER)/$(TARGET)_tinf.bin $(MODDIR)/tinfbin/$(TARGET)_tinf.bin

	@echo "========== Image manipulating end =========="

#*****************************************************************************#
#                      RULES TO CLEAN                                         #
#*****************************************************************************#

clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
	$(call CLEAN_BY_SEARCH_FUNC, ../../sys_module)
	@if [ -d $(APPS_IMAGE_TARGET_FOLDER) ]; \
		then $(REMOVEDIR) $(APPS_IMAGE_TARGET_FOLDER)/*;\
			echo "====== Remove $(APPS_IMAGE_TARGET_FOLDER)/* Success ======";\
		else echo "$(APPS_IMAGE_TARGET_FOLDER) no exist"; fi

clean_all: clean
