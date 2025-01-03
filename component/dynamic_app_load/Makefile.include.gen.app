
#*****************************************************************************#
#                            Basic Configurations                             #
#*****************************************************************************#

CURRENT_PATH	:= $(shell pwd)
ROOTDIR		:= $(shell realpath --relative-to=$(CURRENT_PATH) $(ABS_ROOTDIR))
BASEDIR =  $(ROOTDIR)/../../../..
MODDIR = $(BASEDIR)/component/dynamic_app_load
PROJECTDIR = $(BASEDIR)/amebasmart_gcc_project/project_ap

CONFIGDIR		= $(PROJECTDIR)/../menuconfig
MENU_CONFIG		= $(CONFIGDIR)/.config_ap
include $(MENU_CONFIG)

SRCDIR		=$(PROJECTDIR)/src
INCDIR		=$(PROJECTDIR)/inc
OSDIR		= $(BASEDIR)/component/os
FREERTOSDIR	= $(BASEDIR)/component/os/freertos/freertos_v10.2.1_smp/Source/
TARGETDIR	=$(BASEDIR)/component/soc/amebasmart
BOOTLOADER	=$(BASEDIR)/component/soc/amebasmart/bootloader

APPS_IMAGE_TARGET_FOLDER = $(MODDIR)/image
LDDIR = $(MODDIR)/ld
TARGET_LIBAPP_FOLDER = $(MODDIR)/lib

ELF2TINFDIR = $(MODDIR)/elf2tinf
ELF2TINF = python $(ELF2TINFDIR)/elf2tinf.py --major 1 --minor 0

GIC_VERSION=2
ADD_DEFINE=-DARM_ARCH_MAJOR=7 -DLOG_LEVEL=40 -DARMV7_SUPPORTS_LARGE_PAGE_ADDRESSING=1 -DPLAT_RO_XLAT_TABLES=0 -DPLAT_XLAT_TABLES_DYNAMIC=0 -DENABLE_BTI=0 -DHW_ASSISTED_COHERENCY=0 -DENABLE_ASSERTIONS=1 -DWARMBOOT_ENABLE_DCACHE_EARLY=0

ASDK_VER=10.3.1
TOOLCHAINDIR	= /opt/rtk-toolchain

COMPILEOS		= $(shell uname -o)
COMPILEOS_CYGWIN	= $(shell uname -o | cut -c 1-6)
LINUX_OS		= GNU/Linux
HOSTCC			= cc
MAKE			= make

ifeq ($(COMPILEOS),$(LINUX_OS))
ASDK_TOOLCHAIN = $(TOOLCHAINDIR)/asdk-$(ASDK_VER)/linux/newlib
else
ASDK_TOOLCHAIN = $(TOOLCHAINDIR)/asdk-$(ASDK_VER)/mingw32/newlib
endif

tool_dir = $(ASDK_TOOLCHAIN)/bin

CONFIG_TOOLCHAIN_LD_GCC=y

TOOL_PREFIX = arm-none-eabi-

CC = $(tool_dir)/$(TOOL_PREFIX)gcc
AS = $(tool_dir)/$(TOOL_PREFIX)as
AR = $(tool_dir)/$(TOOL_PREFIX)ar
#LD = $(tool_dir)/$(TOOL_PREFIX)ld
LD = $(tool_dir)/$(TOOL_PREFIX)gcc
LD_ROM = $(tool_dir)/$(TOOL_PREFIX)ld

CC_SIZE = $(tool_dir)/$(TOOL_PREFIX)size
OBJDUMP = $(tool_dir)/$(TOOL_PREFIX)objdump
FROMELF = $(tool_dir)/$(TOOL_PREFIX)objcopy
STRIP   = $(tool_dir)/$(TOOL_PREFIX)strip
NM	= $(tool_dir)/$(TOOL_PREFIX)nm

GDB = $(tool_dir)/arm-none-eabi-gdb
READELF = $(tool_dir)/arm-none-eabi-readelf

REMOVE    = -rm -f
REMOVEDIR = rm -rf
MKDIR     = mkdir
MAKE      = make
CD        = cd
COPY      = cp
MOVE      = mv

#*****************************************************************************#
#                              Include Directories
#*****************************************************************************#

IFLAGS               = -I.
IFLAGS               += -I$(OSDIR)/
IFLAGS               += -I$(OSDIR)/freertos
IFLAGS               += -I$(OSDIR)/os_wrapper/include

IFLAGS               += -I$(TARGETDIR)/cmsis/
IFLAGS               += -I$(TARGETDIR)/cmsis/device
IFLAGS               += -I$(TARGETDIR)/fwlib/include
IFLAGS               += -I$(TARGETDIR)/fwlib/ap_core
IFLAGS               += -I$(TARGETDIR)/wifi_fw/include
IFLAGS               += -I$(TARGETDIR)/app/monitor/include
IFLAGS               += -I$(TARGETDIR)/app/xmodem
IFLAGS               += -I$(TARGETDIR)/swlib
IFLAGS               += -I$(TARGETDIR)/misc
IFLAGS               += -I$(TARGETDIR)/hal/include
IFLAGS               += -I$(TARGETDIR)/hal/src
IFLAGS               += -I$(TARGETDIR)/usrcfg/include
IFLAGS               += -I$(BASEDIR)/component/soc/common/include

IFLAGS               += -I$(INCDIR)
IFLAGS               += -I$(BASEDIR)/component/lwip/api
IFLAGS               += -I$(BASEDIR)/component/stdlib

IFLAGS               += -I$(BASEDIR)/component/example

IFLAGS               += -I$(BASEDIR)/component/file_system/fatfs/r0.14b/include \
			-I$(BASEDIR)/component/file_system/fatfs \
			-I$(BASEDIR)/component/file_system/ftl \
			-I$(BASEDIR)/component/file_system/littlefs \
			-I$(BASEDIR)/component/file_system/littlefs/r2.50 \
			-I$(BASEDIR)/component/file_system/kv \
			-I$(BASEDIR)/component/file_system/vfs


IFLAGS               += -I$(BASEDIR)/component/network
IFLAGS               += -I$(BASEDIR)/component/lwip/lwip_v2.1.2/src/include
IFLAGS               += -I$(BASEDIR)/component/lwip/lwip_v2.1.2/src/include/lwip/apps
IFLAGS               += -I$(BASEDIR)/component/lwip/lwip_v2.1.2/src/include/netif
IFLAGS               += -I$(BASEDIR)/component/lwip/lwip_v2.1.2/port/realtek
IFLAGS               += -I$(BASEDIR)/component/lwip/lwip_v2.1.2/port/realtek/freertos

IFLAGS               += -I$(BASEDIR)/component/os_dep

#SSL
IFLAGS               += -I$(BASEDIR)/component/ssl/ssl_ram_map/rom

#MBEDTLS
IFLAGS               += -I$(BASEDIR)/component/ssl/mbedtls-3.6.0/include
IFLAGS               += -I$(BASEDIR)/component/ssl/mbedtls_ram_map/rom


#*****************************************************************************#
#                             ARM ASSEMBLER FLAGS
#*****************************************************************************#
ASM_FLAGS = -g -mcpu=cortex-a32 -mfpu=vfpv4 -mfloat-abi=hard -c
ASM_FLAGS += -DCONFIG_GIC_VER=${GIC_VERSION} -DconfigINTERRUPT_CONTROLLER_VERSION=${GIC_VERSION} -DCONFIG_CPUS_NUM=${CONFIG_CORE_NUM} -DDEBUG
ASM_FLAGS += $(ADD_DEFINE)
ASM_FLAGS += -MMD -MP
ASM_FLAGS += $(IFLAGS)
#ifeq ($(CONFIG_TOOLCHAIN_ARM_GCC), y)
ASM_FLAGS += $(MODULE_IFLAGS)
#endif
ASM_FLAGS += -fpic -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative -mlong-calls


#*****************************************************************************#
#                           ARM COMPILATION FLAGS
#*****************************************************************************#
GLOBAL_CFLAGS += -O2 -g 
#avoid compile optimize based on undefined behavior
GLOBAL_CFLAGS += -fno-strict-aliasing -fno-strict-overflow -fno-delete-null-pointer-checks
GLOBAL_CFLAGS += -mcpu=cortex-a32 -mfpu=neon -mfloat-abi=hard 
GLOBAL_CFLAGS += -marm 
GLOBAL_CFLAGS += -save-temps

GLOBAL_CFLAGS += -gdwarf-3
GLOBAL_CFLAGS += -fstack-usage
GLOBAL_CFLAGS += -fdata-sections
GLOBAL_CFLAGS += -ffunction-sections
GLOBAL_CFLAGS += -nostartfiles
GLOBAL_CFLAGS += -nostdlib
GLOBAL_CFLAGS += -fno-builtin-printf
GLOBAL_CFLAGS += -Wall
GLOBAL_CFLAGS += -Wpointer-arith

#When the memory length is 4 bytes, memset/memcpy will be optimized for direct 32-bit reading and writing.
#If the source address is not aligned, an error will result because the hardware does not support unaligned accesses.
GLOBAL_CFLAGS += -fno-builtin-memset
GLOBAL_CFLAGS += -fno-builtin-memcpy

GLOBAL_CFLAGS += -Wundef
GLOBAL_CFLAGS += -Wno-write-strings
GLOBAL_CFLAGS += -Wno-maybe-uninitialized
GLOBAL_CFLAGS += --save-temps
GLOBAL_CFLAGS += -c
GLOBAL_CFLAGS += -MMD -MP
GLOBAL_CFLAGS += -fno-jump-tables #For link _thumb1_case_uqi issue

GLOBAL_CFLAGS += -Wextra
GLOBAL_CFLAGS += $(IFLAGS)
GLOBAL_CFLAGS += -DCONFIG_PLATFORM_AMEBASMART
GLOBAL_CFLAGS += -DCONFIG_GIC_VER=${GIC_VERSION} -DconfigINTERRUPT_CONTROLLER_VERSION=${GIC_VERSION} -DCONFIG_CPUS_NUM=${CONFIG_CORE_NUM} -DDEBUG
GLOBAL_CFLAGS += $(ADD_DEFINE)
GLOBAL_CFLAGS += -fpic -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative -mlong-calls

#COPY GLOBAL_CFLAGS to CPP_FLAGS except -Wstrict-prototypes
CPPFLAGS = $(filter-out -Wstrict-prototypes, $(GLOBAL_CFLAGS))
CPPFLAGS += -std=c++11 -fno-use-cxa-atexit

GLOBAL_CFLAGS += -Wstrict-prototypes

ifeq (${IMAGE2_CFLAGS},YES)
GLOBAL_CFLAGS += -DIMAGE2_BUILD
endif

LD_ARG = -L$(ASDK_TOOLCHAIN)/arm-none-eabi/lib/v7-a+neon-fp16/fpu/
LD_ARG += -L$(ASDK_TOOLCHAIN)/lib/gcc/arm-none-eabi/$(ASDK_VER)/v7-a+neon-fp16/fpu
#LD_ARG += -L$(ROOTDIR)/lib/
LD_ARG += -L$(TARGET_LIBAPP_FOLDER)/

LD_ARG += -T$(LDDIR)/app_base.ld

LD_ARG += -O0
LD_ARG += -mcpu=cortex-a32 -mfpu=vfpv4 -mfloat-abi=hard 
LD_ARG += -save-temps 


LD_ARG += -nostartfiles
LD_ARG += -specs nosys.specs
LD_ARG += -Wl,--gc-sections
LD_ARG += -Wl,-Map=text.map
LD_ARG += -Wl,--cref
LD_ARG += -Wl,--build-id=none

#LD_ARG += -Wl,-wrap,strtoul -Wl,-wrap,strupr

LD_ARG += -Wl,-wrap,strlen -Wl,-wrap,strnlen -Wl,-wrap,strcpy -Wl,-wrap,strncpy -Wl,-wrap,strcat -Wl,-wrap,strncat
LD_ARG += -Wl,-wrap,strcmp -Wl,-wrap,stricmp -Wl,-wrap,strncmp -Wl,-wrap,strnicmp
LD_ARG += -Wl,-wrap,strchr -Wl,-wrap,strnchr -Wl,-wrap,strrchr -Wl,-wrap,strstr
LD_ARG += -Wl,-wrap,strtod -Wl,-wrap,strtol
LD_ARG += -Wl,-wrap,atoi -Wl,-wrap,atof
LD_ARG += -Wl,-wrap,memcmp -Wl,-wrap,memmove -Wl,-wrap,memset -Wl,-wrap,memcpy -Wl,-wrap,memclr -Wl,-wrap,memchr

#LD_ARG += -Wl,-wrap,snprintf -Wl,-wrap,scnprintf
LD_ARG += -Wl,-wrap,vsnprintf -Wl,-wrap,vscnprintf -Wl,-wrap,vsprintf -Wl,-wrap,sprintf
LD_ARG += -Wl,-wrap,printf -Wl,-wrap,vprintf
LD_ARG += -Wl,-wrap,malloc  -Wl,-wrap,realloc  -Wl,-wrap,free  -Wl,-wrap,calloc
LD_ARG += -Wl,-wrap,_malloc_r  -Wl,-wrap,_realloc_r  -Wl,-wrap,_free_r  -Wl,-wrap,_calloc_r
LD_ARG += -Wl,-wrap,rand
LD_ARG += -Wl,-wrap,fopen -Wl,-wrap,fclose -Wl,-wrap,fread -Wl,-wrap,fwrite -Wl,-wrap,fseek -Wl,-wrap,fsetpos -Wl,-wrap,fgetpos
LD_ARG += -Wl,-wrap,rewind -Wl,-wrap,fflush -Wl,-wrap,remove -Wl,-wrap,rename -Wl,-wrap,feof -Wl,-wrap,ferror -Wl,-wrap,ftell
LD_ARG += -Wl,-wrap,ftruncate -Wl,-wrap,fputc -Wl,-wrap,fputs -Wl,-wrap,fgets -Wl,-wrap,stat -Wl,-wrap,mkdir -Wl,-wrap,scandir
LD_ARG += -Wl,-wrap,readdir -Wl,-wrap,opendir -Wl,-wrap,access -Wl,-wrap,rmdir -Wl,-wrap,closedir

LD_ARG += -Wl,--no-enum-size-warning
LD_ARG += -Wl,--warn-common -o

LD_PIC_ARG += -fpic -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative -mlong-calls
LD_PIC_ARG += -g
LD_PIC_ARG += -Wl,--undefined=app_main
LD_PIC_ARG += -Wl,--emit-relocs

#*****************************************************************************#
#              CLEAN OBJECT FILES FOR MAKE CLEAN                    	      #
#*****************************************************************************#
CLEAN_OBJS:
	$(Q)$(REMOVE) $(OBJS)
	$(Q)$(REMOVE) $(OBJS:.o=.d)
	$(Q)$(REMOVE) $(OBJS:.o=.i)
	$(Q)$(REMOVE) $(OBJS:.o=.s)
	$(Q)$(REMOVE) $(OBJS:.o=.su)

define CLEAN_FUNC
	-$(Q) $(REMOVE) -rf *.o
	-$(Q) $(REMOVE) -rf *.i
	-$(Q) $(REMOVE) -rf *.ii
	-$(Q) $(REMOVE) -rf *.s
	-$(Q) $(REMOVE) -rf *.su
	-$(Q) $(REMOVE) -rf *.d
endef

define CLEAN_BY_SEARCH_FUNC
	-$(Q) find $(1) -name "*.o" -delete
	-$(Q) find $(1) -name "*.i" -delete
	-$(Q) find $(1) -name "*.ii" -delete
	-$(Q) find $(1) -name "*.s" -delete
	-$(Q) find $(1) -name "*.su" -delete
	-$(Q) find $(1) -name "*.d" -delete
endef

define GEN_AND_MOVE_TO_LIB_FOLDER
	$(AR) rvs $(1).a $(OBJS) $(ASM_OBJS)
	$(FROMELF) -g -D $(1).a
	$(MOVE) -f $(1).a $(2)
endef

#*****************************************************************************#
#              RULES TO GENERATE OBJECT FILE FROM .C FILE                     #
#*****************************************************************************#

%.o:%.c
	$(ECHO) "  CC      $<"
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#              RULES TO GENERATE OBJECT FILE FROM .CPP FILE                   #
#*****************************************************************************#
%.o:%.cpp
	$(ECHO) "  CPP      $<"
	$(Q)$(CC) -ffunction-sections $(CPPFLAGS) $(MODULE_IFLAGS) $< -o $@

#*****************************************************************************#
#              RULES TO GENERATE OBJECT FILE FROM .S FILE                     #
#*****************************************************************************#

%.o:%.S
	$(ECHO) "  AS      $<"
	$(Q)$(CC) $(ASM_FLAGS) $< -o $@