
#*****************************************************************************#
#                            Basic Configurations                             #
#*****************************************************************************#

ABS_ROOTDIR		= $(shell pwd)
SCRIPT_DIR		= $(ROOTDIR)/../../../../tools/image_scripts/internal
MAKE_INCLUDE_GEN	= $(ABS_ROOTDIR)/Makefile.include.gen
MAKE_INCLUDE_LIB	= $(ABS_ROOTDIR)/Makefile.lib

CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
          else if [ -x /bin/bash ]; then echo /bin/bash; \
          else echo sh; fi ; fi)

IMGTOOL_LOADER = NONE
imgtool_flashloader:IMGTOOL_LOADER = YES
export IMGTOOL_LOADER

include $(MAKE_INCLUDE_GEN)

# Verbose flag
V ?= 0
ifeq (${V},1)
        Q:=
        ECHO:=@\#
else
        Q:=@
	ECHO:=@echo
endif
# Code analyze link lib flag
ifeq (${V},2)
CODE_ANA ?= 1
endif
DAILY_BUILD = ${V}
export ABS_ROOTDIR MAKE_INCLUDE_GEN HOSTCC ECHO Q


ifeq ($(COMPILEOS_CYGWIN),Cygwin)
HOSTNAME_APP = /usr/bin/hostname
else
HOSTNAME_APP = $(which hostname)
endif


#.PHONY: all build_target_folder menuconfig clean

###############################################################################
#
# Lets work out what the user wants, and if they have configured us yet
#

RAM_OBJS_LIST = ./build/ram/*.o
LIB_OBJS_LIST = ./build/lib/*.o

ifeq ($(CONFIG_AMEBASMART_B_CUT),y)
ROM_LIB_DIR = $(ROOTDIR)/lib/amebasmart_rom_bcut
FLOADER_BIN_DIR = $(UTILITYDIR)/image_tool_flashloader/amebasmart_acut
endif

LINK_ROM_LIB = $(ROM_LIB_DIR)/rom.a
LINK_ROM_LIB_NS = $(ROM_LIB_DIR)/rom_ns.a

LINK_ROM_SYMBOL_NS = ./ld/rlx8721d_rom_symbol_empty.ld
LINK_ROM_SYMBOL_S = ./ld/rlx8721d_rom_symbol_empty.ld
ifeq ($(CONFIG_LINK_ROM_SYMB),y) 
LINK_ROM_LIB =
LINK_ROM_LIB_NS =
ifeq ($(CONFIG_AMEBASMART_B_CUT),y)
LINK_ROM_SYMBOL_NS = ./ld/rlx8721d_rom_symbol_acut.ld
LINK_ROM_SYMBOL_S = ./ld/rlx8721d_rom_symbol_bcut_s.ld
endif
endif
#LINK_ROM_SYMBOL_S += ./ld/rlx8721d_rom_symbol_stdlib.ld
LINK_ROM_SYMBOL_NS += ./ld/rlx8721d_rom_symbol_stdlib.ld

LINK_APP_LIB =
# Third libs include ARM libs and BT libs.
LINK_THIRD_APP_LIB =
LINK_ROM_LIB += $(ROOTDIR)/lib/soc/lib_chipinfo.a
LINK_ROM_LIB_NS += $(ROOTDIR)/lib/soc/lib_chipinfo.a
ifeq ($(CONFIG_TRUSTZONE), y)
LINK_THIRD_APP_LIB += $(IMAGE_TARGET_FOLDER)/cmse_implib.lib
endif

ifeq ($(CONFIG_SPEEX_LIB),y)
LINK_THIRD_APP_LIB += $(APP_LIB_DIR)/lib_speexdsp.a
endif

#add all app lib in this file
include $(MAKE_INCLUDE_LIB)

CODE_ANALYZE_PYTHON=python $(SCRIPT_DIR)/code_analyze.py
STATIC_ANALYZE_PYTHON=python $(SCRIPT_DIR)/static_code_analyze.py
EXTRACT_CODE_SIZE_PYTHON=python $(SCRIPT_DIR)/extract_code_size.py
ifeq ($(CONFIG_MP_INCLUDED), y)
    ANALYZE_MP_IMG = 1
else
    ANALYZE_MP_IMG = 0
endif

.PHONY: all build_target_folder menuconfig clean build_libraries

prebuild: build_target_folder check_toolchain build_info.h
linker_image3: prebuild copy_ld_img3 make_subdirs_secure

image2_ns: prebuild copy_ld_img2 make_subdirs linker_image2_ns code_analyze
image3_s: linker_image3
bootloader: prebuild copy_ld_img1 make_subdirs_bootloader linker_loader
flashloader: prebuild copy_ld_img1 make_subdirs_flashloader linker_loader copy_flashloader
imgtool_flashloader:  prebuild copy_ld_floader make_subdirs_imgtool_flashloader linker_loader copy_imgtool_flashloader
analysis_extract: code_extract
toolchain: download_toolchain
#*****************************************************************************#
#                      RULES TO CREATE DIRECTORY                              #
#*****************************************************************************#

build_target_folder:
	@if [ -d $(BUILD_TARGET_FOLDER) ]; then echo "$(BUILD_TARGET_FOLDER) exist"; else $(MKDIR) -p $(BUILD_TARGET_FOLDER); fi
	@if [ -d $(IMAGE_TARGET_FOLDER) ]; then echo "$(IMAGE_TARGET_FOLDER) exist"; else $(MKDIR) -p $(IMAGE_TARGET_FOLDER); fi
	@if [ -d $(BUILD_TARGET_LIB_FOLDER) ]; then echo "$(BUILD_TARGET_LIB_FOLDER) exist"; else $(MKDIR) $(BUILD_TARGET_LIB_FOLDER); fi
	@if [ -d $(BUILD_TARGET_RAM_FOLDER) ]; then echo "$(BUILD_TARGET_RAM_FOLDER) exist"; else $(MKDIR) $(BUILD_TARGET_RAM_FOLDER); fi
	@if [ -d $(ROM_LIB_DIR) ]; then echo "$(ROM_LIB_DIR) exist"; else $(MKDIR) $(ROM_LIB_DIR); fi
	@if [ -d $(TARGET_LIBAPP_FOLDER) ]; then echo "$(TARGET_LIBAPP_FOLDER) exist"; else $(MKDIR) -p $(TARGET_LIBAPP_FOLDER); fi
	@if [ -d $(TARGET_LIBSOC_FOLDER) ]; then echo "$(TARGET_LIBSOC_FOLDER) exist"; else $(MKDIR) -p $(TARGET_LIBSOC_FOLDER); fi
#*****************************************************************************#
#                      RULES TO MAKE SUB DIRS                                 #
#*****************************************************************************#
make_subdirs:
	@ rm -f $(RAM_OBJS_LIST)
	@echo > $(ROOTDIR)/ram_size.txt
	@echo > $(ROOTDIR)/obj_list.txt
	@+$(MAKE) -C make all
	@$(MOVE) $(ROOTDIR)/ram_size.txt $(IMAGE_TARGET_FOLDER)
	@$(MOVE) $(ROOTDIR)/obj_list.txt $(IMAGE_TARGET_FOLDER)

make_subdirs_secure:
	@ rm -f $(RAM_OBJS_LIST)
	@echo > $(ROOTDIR)/ram_size.txt
	@echo > $(ROOTDIR)/obj_list.txt
	@+$(MAKE) -C make_secure all
	@$(MOVE) $(ROOTDIR)/ram_size.txt $(IMAGE_TARGET_FOLDER)
	@$(MOVE) $(ROOTDIR)/obj_list.txt $(IMAGE_TARGET_FOLDER)

make_subdirs_bootloader:
	@ rm -f $(RAM_OBJS_LIST)
	@+$(MAKE) -C make/bootloader all

make_subdirs_flashloader:
	@ rm -f $(RAM_OBJS_LIST)
	@make -C make/flashloader all

make_subdirs_imgtool_flashloader:
	@ rm -f $(RAM_OBJS_LIST)
	@make -C $(ROOTDIR)/make/RT_xmodem all

#*****************************************************************************#
#                      RULES TO LINKER IMAGE                                  #
#*****************************************************************************#

linker_image3:
ifeq ($(CONFIG_TRUSTZONE), y)
	@echo "========= linker img3 secure start ========="
	$(ECHO) "  LD      target_img3.axf"
	$(Q)$(LD) $(LD_SEC_OUT_ARG) $(LD_ARG) target_img3.axf $(RAM_OBJS_LIST)  $(LINK_ROM_LIB)

	$(Q)$(MOVE) cmse_implib.lib $(IMAGE_TARGET_FOLDER)/cmse_implib.lib
	$(Q)$(MOVE) text.map $(IMAGE_TARGET_FOLDER)/text_image3.map
	$(Q)$(MOVE) target_img3.axf $(IMAGE_TARGET_FOLDER)
	$(Q)$(NM) $(IMAGE_TARGET_FOLDER)/target_img3.axf | sort > $(IMAGE_TARGET_FOLDER)/target_img3.map
	$(Q)$(OBJDUMP) -d $(IMAGE_TARGET_FOLDER)/target_img3.axf > $(IMAGE_TARGET_FOLDER)/target_img3.asm
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/target_img3.axf $(IMAGE_TARGET_FOLDER)/target_pure_img3.axf
	$(Q)$(STRIP) $(IMAGE_TARGET_FOLDER)/target_pure_img3.axf
	$(Q)$(NM) $(IMAGE_TARGET_FOLDER)/target_pure.axf | sort > $(IMAGE_TARGET_FOLDER)/target_pure_img3.map

	##secure
	$(Q)$(READELF) -s $(IMAGE_TARGET_FOLDER)/cmse_implib.lib > $(IMAGE_TARGET_FOLDER)/cmse_implib.txt
	$(Q)$(OBJDUMP) -j .gnu.sgstubs -d $(IMAGE_TARGET_FOLDER)/target_img3.axf > $(IMAGE_TARGET_FOLDER)/cmse_implib.asm

	$(ECHO) "  BIN      km4_image3_all.bin"
	$(Q)$(FROMELF) -j .ram_image3.text -j .ram_image3.data \
	-Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img3.axf $(IMAGE_TARGET_FOLDER)/ram_3_s.bin

	$(Q)$(FROMELF) -j .gnu.sgstubs -j .ram_image3.entry \
	-Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img3.axf $(IMAGE_TARGET_FOLDER)/ram_3_nsc.bin

	@echo "========== Image Info HEX =========="
	$(Q)$(CC_SIZE) -A --radix=16 $(IMAGE_TARGET_FOLDER)/target_img3.axf
	$(Q)$(CC_SIZE) -t --radix=16 $(IMAGE_TARGET_FOLDER)/target_img3.axf
	@echo "========== Image Info HEX =========="

	@echo "========== Image Info DEC =========="
	$(Q)$(CC_SIZE) -A --radix=10 $(IMAGE_TARGET_FOLDER)/target_img3.axf
	$(Q)$(CC_SIZE) -t --radix=10 $(IMAGE_TARGET_FOLDER)/target_img3.axf
	@echo "========== Image Info DEC =========="

	$(Q)$(RM) -f $(RAM_OBJS_LIST)
	@echo "========== linker img3 secure end =========="

	@echo "========== Image manipulating start =========="
	$(Q) sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/ram_3_s.bin 32
	$(Q) sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/ram_3_nsc.bin 32

	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/ram_3_s.bin  __ram_image3_text_start__  $(IMAGE_TARGET_FOLDER)/target_img3.map
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/ram_3_nsc.bin  __ram_image3_nsc_start__  $(IMAGE_TARGET_FOLDER)/target_img3.map

	$(Q)cat $(IMAGE_TARGET_FOLDER)/ram_3_s_prepend.bin $(IMAGE_TARGET_FOLDER)/ram_3_nsc_prepend.bin > $(IMAGE_TARGET_FOLDER)/km4_image3_all.bin

	$(Q)$(UTILITYDIR)/image_tool/imagetool.sh $(IMAGE_TARGET_FOLDER)/km4_image3_all.bin $(BUILD_TYPE)

	@echo "========== Image manipulating end =========="
endif

linker_image2_raw:
	@echo "========= linker img2_ns start ========="
	$(ECHO) "  LD      target_img2.axf"
ifeq (${CODE_ANA},1)
	$(Q)$(LD) $(LD_ARG) target_img2.axf $(RAM_OBJS_LIST) $(LINK_ROM_LIB_NS) $(LINK_APP_LIB) $(LINK_THIRD_APP_LIB) \
	$(ASDK_TOOLCHAIN)/lib/gcc/arm-none-eabi/$(ASDK_VER)/thumb/v8-m.main+fp/fpu/crtbegin.o \
	$(ASDK_TOOLCHAIN)/lib/gcc/arm-none-eabi/$(ASDK_VER)/thumb/v8-m.main+fp/fpu/crtend.o -lm -lstdc++
else
	$(Q)$(CD) $(BUILD_TARGET_LIB_FOLDER); for libname in $(LINK_APP_LIB); do $(AR) x $$libname ; done
	$(Q)$(LD) $(LD_ARG) target_img2.axf $(RAM_OBJS_LIST) $(LINK_ROM_LIB_NS) $(LIB_OBJS_LIST) $(LINK_THIRD_APP_LIB) \
	$(ASDK_TOOLCHAIN)/lib/gcc/arm-none-eabi/$(ASDK_VER)/thumb/v8-m.main+fp/fpu/crtbegin.o \
	$(ASDK_TOOLCHAIN)/lib/gcc/arm-none-eabi/$(ASDK_VER)/thumb/v8-m.main+fp/fpu/crtend.o -lm -lstdc++
endif
	$(Q)$(MOVE) text.map $(IMAGE_TARGET_FOLDER)/text_image2_ns.map
	$(Q)$(MOVE) target_img2.axf $(IMAGE_TARGET_FOLDER)
	$(Q)$(NM) $(IMAGE_TARGET_FOLDER)/target_img2.axf | sort > $(IMAGE_TARGET_FOLDER)/target_img2.map
	$(Q)$(OBJDUMP) -d $(IMAGE_TARGET_FOLDER)/target_img2.axf > $(IMAGE_TARGET_FOLDER)/target_img2.asm
	@if [ -d $(AP_BUILDDIR)/asdk/image/ ]; then \
		$(COPY) $(IMAGE_TARGET_FOLDER)/target_img2.asm $(AP_BUILDDIR)/asdk/image/target_img2_otrcore.asm; fi
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/target_img2.axf $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf
	$(Q)$(STRIP) $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf

linker_image2_ns: linker_image2_raw
	$(ECHO) "  BIN      km4_image2_all.bin"
	$(Q)$(FROMELF) -j .sram_image2.text.data -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf $(IMAGE_TARGET_FOLDER)/sram_2.bin

	@#let .ARM.extab/.ARM.exidx +- 1GB can jump to *(.text*)
	$(Q)ARMExAddr=0x$(shell grep __exidx_end $(IMAGE_TARGET_FOLDER)/target_img2.map | awk '{print $$1}'); \
	if [[ $$ARMExAddr -gt 0x60000000 ]] ; then \
		$(FROMELF) -j .psram_image2.text.data -j .ARM.extab -j .ARM.exidx -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf $(IMAGE_TARGET_FOLDER)/psram_2.bin; \
		$(FROMELF) -j .xip_image2.text -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf $(IMAGE_TARGET_FOLDER)/xip_image2.bin; \
	else \
		$(FROMELF) -j .psram_image2.text.data -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf $(IMAGE_TARGET_FOLDER)/psram_2.bin; \
		$(FROMELF) -j .xip_image2.text -j .ARM.extab -j .ARM.exidx -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf $(IMAGE_TARGET_FOLDER)/xip_image2.bin; \
	fi

	@# for Bluetooth trace
ifeq ($(CONFIG_BT), y)
	$(Q)$(FROMELF) -j .bluetooth_trace.text -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf $(IMAGE_TARGET_FOLDER)/APP.trace
endif

	@# for Coexistence trace
	$(Q)$(FROMELF) -j .coex_trace.text -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf $(IMAGE_TARGET_FOLDER)/COEX.trace

	@echo "========== Image Info HEX =========="
	$(Q)$(CC_SIZE) -A --radix=16 $(IMAGE_TARGET_FOLDER)/target_img2.axf
	$(Q)$(CC_SIZE) -t --radix=16 $(IMAGE_TARGET_FOLDER)/target_img2.axf
	@echo "========== Image Info HEX =========="

	@echo "========== Image Info DEC =========="
	$(Q)$(CC_SIZE) -A --radix=10 $(IMAGE_TARGET_FOLDER)/target_img2.axf
	$(Q)$(CC_SIZE) -t --radix=10 $(IMAGE_TARGET_FOLDER)/target_img2.axf
	@echo "========== Image Info DEC =========="
	
	$(Q)$(RM) -f $(RAM_OBJS_LIST) $(LIB_OBJS_LIST)
	@echo "========== linker img2_ns end =========="

	@echo "========== Image manipulating start =========="

	# 32Bytes aligned for otf en #
	$(Q) sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/sram_2.bin 32
	$(Q) sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/psram_2.bin 32
	$(Q) sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/xip_image2.bin 32

	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/sram_2.bin  __sram_image2_start__  $(IMAGE_TARGET_FOLDER)/target_img2.map
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/psram_2.bin  __psram_image2_start__  $(IMAGE_TARGET_FOLDER)/target_img2.map
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/xip_image2.bin  __flash_text_start__  $(IMAGE_TARGET_FOLDER)/target_img2.map

	$(Q)cat $(IMAGE_TARGET_FOLDER)/xip_image2_prepend.bin $(IMAGE_TARGET_FOLDER)/sram_2_prepend.bin $(IMAGE_TARGET_FOLDER)/psram_2_prepend.bin  > $(IMAGE_TARGET_FOLDER)/km4_image2_all.bin
	@#$(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/km4_image2_all.bin 4096
	$(Q)$(UTILITYDIR)/image_tool/imagetool.sh $(IMAGE_TARGET_FOLDER)/km4_image2_all.bin $(BUILD_TYPE);

ifeq ($(CONFIG_FATFS_WITHIN_APP_IMG), y)
	@if [ -f $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app.bin ] && [ -f $(BASEDIR)/amebasmart_gcc_project/fatfs.bin ]; then \
		mv $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app.bin $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app_tmp.bin; \
		mv $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app_ns.bin $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app_ns_tmp.bin; \
		sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app_tmp.bin 4096; \
		sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app_ns_tmp.bin 4096; \
		$(PREPENDTOOL) $(BASEDIR)/amebasmart_gcc_project/fatfs.bin  VFS1_FLASH_BASE_ADDR  $(AP_IMAGEDIR)/target_img2.map; \
		cat $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app_tmp.bin $(BASEDIR)/amebasmart_gcc_project/fatfs_prepend.bin > $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app.bin; \
		cat $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app_ns_tmp.bin $(BASEDIR)/amebasmart_gcc_project/fatfs_prepend.bin > $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app_ns.bin; \
	else \
		if [ -f $(IMAGE_TARGET_FOLDER)/km0_km4_app.bin ] && [ -f $(BASEDIR)/amebasmart_gcc_project/fatfs.bin ]; then \
			mv $(IMAGE_TARGET_FOLDER)/km0_km4_app.bin $(IMAGE_TARGET_FOLDER)/km0_km4_app_tmp.bin; \
			mv $(IMAGE_TARGET_FOLDER)/km0_km4_app_ns.bin $(IMAGE_TARGET_FOLDER)/km0_km4_app_ns_tmp.bin; \
			sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/km0_km4_app_tmp.bin 4096; \
			sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/km0_km4_app_ns_tmp.bin 4096; \
			$(PREPENDTOOL) $(BASEDIR)/amebasmart_gcc_project/fatfs.bin  VFS1_FLASH_BASE_ADDR  $(AP_IMAGEDIR)/target_img2.map; \
			cat $(IMAGE_TARGET_FOLDER)/km0_km4_app_tmp.bin $(BASEDIR)/amebasmart_gcc_project/fatfs_prepend.bin > $(IMAGE_TARGET_FOLDER)/km0_km4_app.bin; \
			cat $(IMAGE_TARGET_FOLDER)/km0_km4_app_ns_tmp.bin $(BASEDIR)/amebasmart_gcc_project/fatfs_prepend.bin > $(IMAGE_TARGET_FOLDER)/km0_km4_app_ns.bin; \
		fi \
	fi
endif

ifeq ($(CONFIG_COMPRESS_OTA_IMG), y)
	@if [ -f $(IMAGE_TARGET_FOLDER)/km0_km4_app.bin ]; then \
		$(COMPRESSTOOL) $(IMAGE_TARGET_FOLDER)/km0_km4_app.bin; \
		$(COMPRESSTOOL) $(IMAGE_TARGET_FOLDER)/km0_km4_app_ns.bin; \
		$(COPY) $(IMAGE_TARGET_FOLDER)/km0_km4_app_compress.bin $(IMAGE_TARGET_FOLDER)/tmp_app.bin; \
		$(COPY) $(IMAGE_TARGET_FOLDER)/km0_km4_app_ns_compress.bin $(IMAGE_TARGET_FOLDER)/tmp_app_ns.bin; \
	else \
		if [ -f $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app.bin ]; then \
			$(COMPRESSTOOL) $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app.bin; \
			$(COMPRESSTOOL) $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app_ns.bin; \
			$(COPY) $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app_compress.bin $(IMAGE_TARGET_FOLDER)/tmp_app.bin; \
			$(COPY) $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app_ns_compress.bin $(IMAGE_TARGET_FOLDER)/tmp_app_ns.bin; \
		fi \
	fi
else
	@if [ -f $(IMAGE_TARGET_FOLDER)/km0_km4_app.bin ]; then \
		$(COPY) $(IMAGE_TARGET_FOLDER)/km0_km4_app.bin $(IMAGE_TARGET_FOLDER)/tmp_app.bin; \
		$(COPY) $(IMAGE_TARGET_FOLDER)/km0_km4_app_ns.bin $(IMAGE_TARGET_FOLDER)/tmp_app_ns.bin; \
	else \
		if [ -f $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app.bin ]; then \
			$(COPY) $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app.bin $(IMAGE_TARGET_FOLDER)/tmp_app.bin; \
			$(COPY) $(IMAGE_TARGET_FOLDER)/km0_km4_ca32_app_ns.bin $(IMAGE_TARGET_FOLDER)/tmp_app_ns.bin; \
		fi \
	fi
endif
ifeq ($(CONFIG_UPGRADE_BOOTLOADER), y)
	@if [ -f $(IMAGE_TARGET_FOLDER)/tmp_app.bin ]; then \
		$(OTAPREPENDTOOL) $(IMAGE_TARGET_FOLDER)/tmp_app.bin $(IMAGE_TARGET_FOLDER)/km4_boot_all.bin; \
		$(OTAPREPENDTOOL) $(IMAGE_TARGET_FOLDER)/tmp_app_ns.bin $(IMAGE_TARGET_FOLDER)/km4_boot_all_ns.bin; \
	fi
else
	@if [ -f $(IMAGE_TARGET_FOLDER)/tmp_app.bin ]; then \
		$(OTAPREPENDTOOL) $(IMAGE_TARGET_FOLDER)/tmp_app.bin; \
		$(OTAPREPENDTOOL) $(IMAGE_TARGET_FOLDER)/tmp_app_ns.bin; \
	fi
endif
	@rm -f $(IMAGE_TARGET_FOLDER)/tmp_app.bin
	@rm -f $(IMAGE_TARGET_FOLDER)/tmp_app_ns.bin
	@echo "========== Image manipulating end =========="

linker_loader:
	@echo "========= linker loader start ========="
	$(ECHO) "  LD      target_loader.axf"
	$(Q)$(LD) $(LD_ARG) target_loader.axf $(RAM_OBJS_LIST) $(LINK_ROM_LIB) $(LINK_LOADER_LIB)

	$(Q)$(MOVE) text.map $(IMAGE_TARGET_FOLDER)/text_loader.map
	$(Q)$(MOVE) target_loader.axf $(IMAGE_TARGET_FOLDER)
	$(Q)$(NM) $(IMAGE_TARGET_FOLDER)/target_loader.axf | sort > $(IMAGE_TARGET_FOLDER)/target_loader.map
	$(Q)$(OBJDUMP) -d $(IMAGE_TARGET_FOLDER)/target_loader.axf > $(IMAGE_TARGET_FOLDER)/target_loader.asm
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/target_loader.axf $(IMAGE_TARGET_FOLDER)/target_pure_loader.axf
	$(Q)$(STRIP) $(IMAGE_TARGET_FOLDER)/target_pure_loader.axf

	$(ECHO) "  BIN      km4_boot_all.bin"
	$(Q)$(FROMELF) -j .ram_image1.entry  -j .ram_image1.text -j .ram_image1.data \
	-Obinary $(IMAGE_TARGET_FOLDER)/target_pure_loader.axf $(IMAGE_TARGET_FOLDER)/ram_1.bin

	@echo "========== Image Info HEX =========="
	$(Q)$(CC_SIZE) -A --radix=16 $(IMAGE_TARGET_FOLDER)/target_loader.axf
	$(Q)$(CC_SIZE) -t --radix=16 $(IMAGE_TARGET_FOLDER)/target_loader.axf
	@echo "========== Image Info HEX =========="

	@echo "========== Image Info DEC =========="
	$(Q)$(CC_SIZE) -A --radix=10 $(IMAGE_TARGET_FOLDER)/target_loader.axf
	$(Q)$(CC_SIZE) -t --radix=10 $(IMAGE_TARGET_FOLDER)/target_loader.axf
	@echo "========== Image Info DEC =========="

	$(Q)$(RM) -f $(RAM_OBJS_LIST)
	@echo "========== linker end =========="

	@echo "========== Image manipulating start =========="

	$(Q) sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/ram_1.bin 32

	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/ram_1.bin  __ram_start_table_start__  $(IMAGE_TARGET_FOLDER)/target_loader.map
	@if [ ${IMGTOOL_LOADER} == "YES" ]; then \
		$(UTILITYDIR)/image_tool/imagetool.sh $(IMAGE_TARGET_FOLDER)/ram_1_prepend.bin $(BUILD_TYPE); \
	else \
		cat $(IMAGE_TARGET_FOLDER)/ram_1_prepend.bin > $(IMAGE_TARGET_FOLDER)/km4_boot_all.bin; \
		$(UTILITYDIR)/image_tool/imagetool.sh $(IMAGE_TARGET_FOLDER)/km4_boot_all.bin $(BUILD_TYPE); \
	fi

	@echo "========== Image manipulating end =========="
	
code_analyze:
	@echo "========== Image analyze start =========="
	@if [ -f $(SCRIPT_DIR)/code_analyze.py ]; then \
		$(CODE_ANALYZE_PYTHON) $(ANALYZE_MP_IMG) $(DAILY_BUILD); \
	fi
	@if [ -f $(SCRIPT_DIR)/static_code_analyze.py ]; then \
		$(STATIC_ANALYZE_PYTHON) $(DAILY_BUILD); \
	fi
	@echo "========== Image analyze end =========="

code_extract:
	@echo "========== Image extract start =========="
	@if [ -f $(SCRIPT_DIR)/extract_code_size.py ]; then \
		$(EXTRACT_CODE_SIZE_PYTHON); \
	fi
	@echo "========== Image extract end =========="

download_toolchain:
ifeq ($(URL), aliyun)
	@echo "Download toolchain from $(TOOLCHAINURL_ALIYUN)"
	$(call download_toolchain_func,$(TOOLCHAINURL_ALIYUN))
else
	@echo "Download toolchain from $(TOOLCHAINURL)"
	$(call download_toolchain_func,$(TOOLCHAINURL))
endif
#*****************************************************************************#
#                      RULES TO BUILD COMPILER TIME                           #
#*****************************************************************************#

build_info.h:
	@echo \#define UTS_VERSION \"`date +%Y/%m/%d-%T`\" > .ver
	@echo \#define RTL_FW_COMPILE_TIME \"`date +%Y/%m/%d-%T`\" >> .ver
	@echo \#define RTL_FW_COMPILE_DATE \"`date +%Y/%m/%d`\" >> .ver
	@echo \#define RTL_FW_COMPILE_BY \"`id -u -n`\" >> .ver
	@echo \#define RTL_FW_COMPILE_HOST \"`$(HOSTNAME_APP)`\" >> .ver
	@if [ -x /bin/dnsdomainname ]; then \
		echo \#define RTL_FW_COMPILE_DOMAIN \"`dnsdomainname`\"; \
	elif [ -x /bin/domainname ]; then \
		echo \#define RTL_FW_COMPILE_DOMAIN \"`domainname`\"; \
	else \
		echo \#define RTL_FW_COMPILE_DOMAIN ; \
	fi >> .ver
ifeq ($(CONFIG_TOOLCHAIN_ARM_GCC), y)
	@echo \#define RTL_FW_COMPILER \"ARM_GCC\" >> .ver
else
	@echo \#define RTL_FW_COMPILER \"`$(CC) $(CFLAGS) -v 2>&1 | tail -1`\" >> .ver
endif
	@mv -f .ver $(INCDIR)/$@

#*****************************************************************************#
#                      RULES TO ENTER GDB                                     #
#*****************************************************************************#

gdb_prepare:
ifeq ($(COMPILEOS_CYGWIN),Cygwin)
	$(GNU_SCRIPT)/Check_Jtag.sh $(GNU_SCRIPT)
endif
	@if [ ! -d $(IMAGE_TARGET_FOLDER) ]; \
		then echo "$(IMAGE_TARGET_FOLDER) no exist"; fi
	@if [ -f  $(GNU_SCRIPT)/.bin ]; \
		then	$(REMOVE) $(GNU_SCRIPT)/*.bin; fi
	@if [ -d ./lib/rom_acut ]; then echo "./lib/rom_acut exist"; else $(MKDIR) ./lib/rom_acut; fi

gdb: gdb_prepare
	$(COPY) $(GNU_SCRIPT)/acut_loader/*  $(GNU_SCRIPT)/
	$(GNU_SCRIPT)/SetupGDB_FPGA.sh $(GNU_SCRIPT)
	$(GDB) -x $(GNU_SCRIPT)/rtl_gdb_flash_write.txt
	
gdbboot: gdb_prepare
	$(GDB) -x $(GNU_SCRIPT)/rtl_gdb_jtag_boot.txt

gdbbootxip: gdb_prepare
	$(GDB) -x $(GNU_SCRIPT)/rtl_gdb_jtag_boot_xip.txt

#*****************************************************************************#
#                      RULES TO SET DOWNLOAD AND DEBUG                        #
#*****************************************************************************#
.PHONY: debug setup
setup:
	@echo "----------------"
	@echo Setup $(GDB_SERVER)
	@echo "----------------"
ifeq ($(GDB_SERVER), probe)
	cp -p $(GNU_SCRIPT)/rtl_gdb_debug_probe.txt $(GNU_SCRIPT)/rtl_gdb_debug.txt
	cp -p $(GNU_SCRIPT)/rtl_gdb_flash_write_probe.txt $(GNU_SCRIPT)/rtl_gdb_flash_write.txt
	cp -p $(GNU_SCRIPT)/rtl_gdb_jtag_boot_com_probe.txt $(GNU_SCRIPT)/rtl_gdb_jtag_boot_com.txt
else
	cp -p $(GNU_SCRIPT)/rtl_gdb_debug_jlink.txt $(GNU_SCRIPT)/rtl_gdb_debug.txt
	cp -p $(GNU_SCRIPT)/rtl_gdb_flash_write_jlink.txt $(GNU_SCRIPT)/rtl_gdb_flash_write.txt
	cp -p $(GNU_SCRIPT)/rtl_gdb_jtag_boot_com_jlink.txt $(GNU_SCRIPT)/rtl_gdb_jtag_boot_com.txt
endif

debug:
	@if [ ! -f $(GNU_SCRIPT)/rtl_gdb_debug.txt ] ; then echo Please do \"make setup GDB_SERVER=[jlink or openocd]\" first; echo && false ; fi
ifeq ($(COMPILEOS_CYGWIN),Cygwin)
	$(GNU_SCRIPT)/Check_Jtag.sh
	cmd /c start $(GDBR) -x $(GNU_SCRIPT)/rtl_gdb_debug.txt
else
	$(GDB) -x $(GNU_SCRIPT)/rtl_gdb_debug.txt
endif

#*****************************************************************************#
#                      RULES TO CLEAN                                         #
#*****************************************************************************#

clean:
	@if [ -d $(BUILD_TARGET_FOLDER) ]; \
		then $(REMOVEDIR) $(BUILD_TARGET_FOLDER)/*;\
			echo "====== Remove $(BUILD_TARGET_FOLDER)/* Success ======";\
		else echo "$(BUILD_TARGET_FOLDER) no exist"; fi
	@if [ -d $(IMAGE_TARGET_FOLDER) ]; \
		then $(REMOVEDIR) $(IMAGE_TARGET_FOLDER)/*; \
			echo "====== Remove $(IMAGE_TARGET_FOLDER) Success ======";\
		else echo "$(IMAGE_TARGET_FOLDER) no exist"; fi

	$(call CLEAN_BY_SEARCH_FUNC, make)
	$(call CLEAN_BY_SEARCH_FUNC, make_secure)
	@#use CLEAN_BY_SEARCH_FUNC replace clean target
	@# +$(MAKE) -C make clean
	@# +$(MAKE) -C make_secure clean

	@# #flashloader code start
	@# @+$(MAKE) -C make/flashloader clean
	@#+$(MAKE) -C $(TARGETDIR)/imgtool_floader clean
	
clean_xip:
	@make -C make clean
	@make -C make_ns clean

clean_all: clean

#*****************************************************************************#
#              PACKAGE COPY link script file-ld                               #
#*****************************************************************************#
copy_ld_img1:
ifeq ($(CONFIG_XIP_FLASH), y)
	$(Q)$(COPY) ./ld/rlx8721d_img1_s_xip.ld  $(BUILD_TARGET_FOLDER)/rlx8721d.ld
else
	$(Q)$(COPY) ./ld/rlx8721d_img1_s.ld  $(BUILD_TARGET_FOLDER)/rlx8721d.ld
endif
	cat $(LINK_ROM_SYMBOL_S) >> $(BUILD_TARGET_FOLDER)/rlx8721d.ld
copy_ld_floader:
	$(Q)$(COPY) ./ld/rlx8721d_floader_s.ld  $(BUILD_TARGET_FOLDER)/rlx8721d.ld
	cat $(LINK_ROM_SYMBOL_S) >> $(BUILD_TARGET_FOLDER)/rlx8721d.ld
copy_ld_img2:
ifeq ($(CONFIG_XIP_FLASH), y)
	$(Q)$(COPY) ./ld/rlx8721d_img2_non_trustzone_xip.ld  $(BUILD_TARGET_FOLDER)/rlx8721d.ld
else
	$(Q)$(COPY) ./ld/rlx8721d_img2_non_trustzone_dram.ld  $(BUILD_TARGET_FOLDER)/rlx8721d.ld
endif
	$(Q)cat $(LINK_ROM_SYMBOL_NS) >> $(BUILD_TARGET_FOLDER)/rlx8721d.ld
copy_ld_img3:
	$(Q)$(COPY) ./ld/rlx8721d_img3_s.ld  $(BUILD_TARGET_FOLDER)/rlx8721d.ld
	$(Q)cat $(LINK_ROM_SYMBOL_S) >> $(BUILD_TARGET_FOLDER)/rlx8721d.ld

#*****************************************************************************#
#              COPY flashloader into flashdownload folder                     #
#*****************************************************************************#
copy_flashloader:
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/target_loader.axf $(GNU_SCRIPT)/acut_loader/target_FPGA.axf -f
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/ram_1.bin $(GNU_SCRIPT)/acut_loader/flash_loader_ram_1.bin -f

copy_imgtool_flashloader:
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/target_loader.axf $(FLOADER_BIN_DIR)/target_FPGA.axf -f
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/imgtool_flashloader.bin 	$(FLOADER_BIN_DIR)/imgtool_flashloader.bin -f
	$(Q)$(MOVE) $(IMAGE_TARGET_FOLDER)/imgtool_flashloader.bin $(FLOADER_BIN_DIR)/floader_amebasmart.bin -f

gen_imgtool_floader: 
	@if [ ! -f $(FLOADER_BIN_DIR)/imgtool_flashloader.bin ]; then \
		echo Error: No imgtool_flashloader.bin in $(FLOADER_BIN_DIR); exit 1;\
	fi
	$(Q)head -zc -4096 $(FLOADER_BIN_DIR)/imgtool_flashloader.bin > $(IMAGE_TARGET_FOLDER)/ram_1_prepend.bin
	$(Q)sh $(UTILITYDIR)/image_tool/imagetool.sh $(IMAGE_TARGET_FOLDER)/ram_1_prepend.bin $(BUILD_TYPE);
ifneq ($(RTLNAME),)
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/imgtool_flashloader.bin 	$(FLOADER_BIN_DIR)/floader_$(RTLNAME).bin -f;
else
	$(Q)echo Error: use \"make gen_imgtool_floader RTLNAME=xxx\" to build a floader.bin, xxx is rltxxx; exit 1;
endif
