
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
flashloader:IMGTOOL_LOADER = YES
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

FLOADER_BIN_DIR = $(UTILITYDIR)/image_tool_flashloader/amebadplus_acut
ifeq ($(CONFIG_AMEBADPLUS_A_CUT),y)
ROM_LIB_DIR = $(ROOTDIR)/lib/amebadplus_rom_acut
else ifeq ($(CONFIG_AMEBADPLUS_B_CUT),y)
ROM_LIB_DIR = $(ROOTDIR)/lib/amebadplus_rom_bcut
else
exit 1
endif

ifeq ($(CONFIG_LINK_ROM_SYMB),y)
LINK_ROM_LIB =
LINK_ROM_LIB_NS =
LINK_ROM_SYMBOL_NS = ./ld/ameba_rom_symbol_acut.ld
LINK_ROM_SYMBOL_S = ./ld/ameba_rom_symbol_acut_s.ld
else
LINK_ROM_LIB = $(ROM_LIB_DIR)/rom.a
LINK_ROM_LIB_NS = $(ROM_LIB_DIR)/rom_ns.a
LINK_ROM_SYMBOL_NS = ./ld/ameba_rom_symbol_empty.ld
LINK_ROM_SYMBOL_S = ./ld/ameba_rom_symbol_empty.ld
endif

LINK_ROM_LIB += $(ROOTDIR)/lib/soc/lib_chipinfo.a
LINK_ROM_LIB_NS += $(ROOTDIR)/lib/soc/lib_chipinfo.a

LINK_ROM_SYMBOL_STDLIB = ./ld/ameba_rom_symbol_stdlib.ld

LINK_APP_LIB =
# Third libs include ARM libs and BT libs.
LINK_THIRD_APP_LIB =

ifeq ($(CONFIG_RDP_BASIC), y)
LINK_THIRD_APP_LIB += $(IMAGE_TARGET_FOLDER)/cmse_implib.lib
endif

ifeq ($(CONFIG_RDP_TFM), y)
LINK_THIRD_APP_LIB += $(IMAGE_TARGET_FOLDER)/s_veneers.o
LINK_THIRD_APP_LIB += $(IMAGE_TARGET_FOLDER)/lib_tfm_ns.a

TFM_SRC_DIR = $(TARGETDIR)/../common/trusted-firmware-m/
TFM_LD_DIR = $(TFM_SRC_DIR)/platform/ext/target/realtek/amebadplus/ld/

ifeq ($(CONFIG_TFM_TEST_ENABLE), y)
CMAKE_ARGU += -DNS=ON
else
CMAKE_ARGU += -DNS=OFF
endif

ifeq ($(CONFIG_TFM_TEST_S), y)
CMAKE_ARGU += -DTEST_S=ON
CMAKE_ARGU += -DTEST_S_ATTESTATION=ON
CMAKE_ARGU += -DTEST_S_CRYPTO=ON
CMAKE_ARGU += -DTEST_S_ITS=ON
CMAKE_ARGU += -DTEST_S_PS=ON
CMAKE_ARGU += -DTEST_S_PLATFORM=ON
CMAKE_ARGU += -DTEST_S_IPC=ON
else
CMAKE_ARGU += -DTEST_S=OFF -DTEST_S_ATTESTATION=OFF -DTEST_S_CRYPTO=OFF -DTEST_S_ITS=OFF -DTEST_S_PS=OFF -DTEST_S_PLATFORM=OFF -DTEST_S_IPC=OFF
endif

ifeq ($(CONFIG_TFM_TEST_NS), y)
CMAKE_ARGU += -DTEST_NS=ON
CMAKE_ARGU += -DTEST_NS_ATTESTATION=ON
CMAKE_ARGU += -DTEST_NS_T_COSE=ON
CMAKE_ARGU += -DTEST_NS_CRYPTO=ON
CMAKE_ARGU += -DTEST_NS_ITS=ON
CMAKE_ARGU += -DTEST_NS_PS=ON
CMAKE_ARGU += -DTEST_NS_PLATFORM=ON
CMAKE_ARGU += -DTEST_NS_IPC=ON
else
CMAKE_ARGU += -DTEST_NS=OFF -DTEST_NS_ATTESTATION=OFF -DTEST_NS_CRYPTO=OFF -DTEST_NS_ITS=OFF -DTEST_NS_PS=OFF -DTEST_NS_T_COSE=OFF -DTEST_NS_PLATFORM=OFF -DTEST_NS_IPC=OFF
endif

ifeq ($(CONFIG_TFM_TEST_PSA_API), y)
# CMAKE_ARGU += -DTESTS_COVERAGE=PASS
ifeq ($(CONFIG_TEST_PSA_API_STORAGE), y)
CMAKE_ARGU += -DTEST_PSA_API=STORAGE
endif
ifeq ($(CONFIG_TEST_PSA_API_CRYPTO), y)
CMAKE_ARGU += -DTEST_PSA_API=CRYPTO
endif
ifeq ($(CONFIG_TEST_PSA_API_INITIAL_ATTESTATION), y)
CMAKE_ARGU += -DTEST_PSA_API=INITIAL_ATTESTATION
endif
endif

export CMAKE_ARGU
endif

ifeq ($(CONFIG_802154_THREAD_EN), y)
LINK_THIRD_APP_LIB += $(ROOTDIR)/lib/application/lib_openthread.a
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

linker_tfm: prebuild copy_ld_tfm make_subdirs_tfm
tfm_s: linker_tfm
#*****************************************************************************#
#                      RULES TO CREATE DIRECTORY                              #
#*****************************************************************************#

build_target_folder:
	@if [ -d $(BUILD_TARGET_FOLDER) ]; then echo "$(BUILD_TARGET_FOLDER) exist"; else $(MKDIR) -p $(BUILD_TARGET_FOLDER); fi
	@if [ -d $(IMAGE_TARGET_FOLDER) ]; then echo "$(IMAGE_TARGET_FOLDER) exist"; else $(MKDIR) -p $(IMAGE_TARGET_FOLDER); fi
	@if [ -d $(BUILD_TARGET_RAM_FOLDER) ]; then echo "$(BUILD_TARGET_RAM_FOLDER) exist"; else $(MKDIR) $(BUILD_TARGET_RAM_FOLDER); fi
	@if [ -d $(BUILD_TARGET_LIB_FOLDER) ]; then echo "$(BUILD_TARGET_LIB_FOLDER) exist"; else $(MKDIR) $(BUILD_TARGET_LIB_FOLDER); fi
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

make_subdirs_tfm:
ifeq ($(CONFIG_RDP_TFM), y)
	@#delete build_tfm folder for msys2, otherwise rebuild will fail in windows
	@rm -rf build_tfm

	$(Q)if [ ! -d build_tfm ]; then \
		export PATH=$(tool_dir):$$PATH; \
		$(MKDIR) -p build_tfm; \
		cd build_tfm; \
		cmake -DTFM_PROFILE=profile_medium -DTFM_PLATFORM=realtek/amebadplus -DBL2=OFF $(TFM_SRC_DIR); \
		# cmake -DTFM_SPM_LOG_LEVEL=TFM_SPM_LOG_LEVEL_INFO -DTFM_PARTITION_LOG_LEVEL=TFM_PARTITION_LOG_LEVEL_INFO $(TFM_SRC_DIR); \
	fi

ifeq ($(CONFIG_TFM_TEST_ENABLE), y)
	@cd build_tfm && cmake $(CMAKE_ARGU) $(TFM_SRC_DIR)
	@if [ $(shell echo $$PATH | grep -q "asdk-10.3.1" && echo 1 || echo 0) -eq 0 ]; then \
		echo "-- Error: please add 'export PATH=$(tool_dir):\$$PATH' to ~/.bashrc "; exit 1;\
	fi
endif

	$(Q)make -C build_tfm install
	$(Q)$(COPY) build_tfm/install/outputs/tfm_s.* $(IMAGE_TARGET_FOLDER)
	$(Q)$(COPY) build_tfm/install/interface/lib/s_veneers.o $(IMAGE_TARGET_FOLDER)

	@if [ ! -d $(TFM_NS_DIR)/interface/ ]; then $(MKDIR) -p $(TFM_NS_DIR)/interface/; fi
	$(Q)$(COPY) -r build_tfm/install/interface/include/		$(TFM_NS_DIR)/interface/
	$(Q)$(COPY) -r build_tfm/install/interface/src/			$(TFM_NS_DIR)/interface/
	$(Q)$(COPY) -r $(TFM_SRC_DIR)/config/profile/config_profile_medium.h	$(TFM_NS_DIR)/interface/

ifeq ($(CONFIG_TFM_TEST_ENABLE), y)
	$(COPY) -r build_tfm/lib/ext/tfm_test_repo-src/ns_interface/os_wrapper			$(TFM_NS_DIR)/interface/include/;
endif
endif

#*****************************************************************************#
#                      RULES TO LINKER IMAGE                                  #
#*****************************************************************************#
linker_tfm:
ifeq ($(CONFIG_RDP_TFM), y)
	@echo "========= linker tfm secure start ========="
	$(ECHO) "  LD      tfm_s.axf"

	$(Q)$(NM) $(IMAGE_TARGET_FOLDER)/tfm_s.axf | sort > $(IMAGE_TARGET_FOLDER)/tfm_s.map
	$(Q)$(OBJDUMP) -d $(IMAGE_TARGET_FOLDER)/tfm_s.axf > $(IMAGE_TARGET_FOLDER)/tfm_s.asm
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/tfm_s.axf $(IMAGE_TARGET_FOLDER)/target_pure_tfm.axf
	$(Q)$(STRIP) $(IMAGE_TARGET_FOLDER)/target_pure_tfm.axf
	$(Q)$(NM) $(IMAGE_TARGET_FOLDER)/target_pure_tfm.axf | sort > $(IMAGE_TARGET_FOLDER)/target_pure_tfm.map

	##secure
	$(Q)$(READELF) -s $(IMAGE_TARGET_FOLDER)/s_veneers.o > $(IMAGE_TARGET_FOLDER)/cmse_implib.txt
	$(Q)$(OBJDUMP) -j .gnu.sgstubs -d $(IMAGE_TARGET_FOLDER)/tfm_s.axf > $(IMAGE_TARGET_FOLDER)/cmse_implib.asm

	$(ECHO) "  BIN      km4_image3_all.bin"
	$(Q)$(FROMELF) -R .gnu.sgstubs -R .ram_image3.entry \
	-Obinary $(IMAGE_TARGET_FOLDER)/target_pure_tfm.axf $(IMAGE_TARGET_FOLDER)/ram_3_s.bin

	$(Q)$(FROMELF) -j .gnu.sgstubs -j .ram_image3.entry \
	-Obinary $(IMAGE_TARGET_FOLDER)/target_pure_tfm.axf $(IMAGE_TARGET_FOLDER)/ram_3_nsc.bin

	@echo "========== Image Info HEX =========="
	$(Q)$(CC_SIZE) -A --radix=16 $(IMAGE_TARGET_FOLDER)/tfm_s.axf
	$(Q)$(CC_SIZE) -t --radix=16 $(IMAGE_TARGET_FOLDER)/tfm_s.axf
	@echo "========== Image Info HEX =========="

	@echo "========== Image Info DEC =========="
	$(Q)$(CC_SIZE) -A --radix=10 $(IMAGE_TARGET_FOLDER)/tfm_s.axf
	$(Q)$(CC_SIZE) -t --radix=10 $(IMAGE_TARGET_FOLDER)/tfm_s.axf
	@echo "========== Image Info DEC =========="

	$(Q)$(RM) -f $(RAM_OBJS_LIST)
	@echo "========== linker img3 secure end =========="

	@echo "========== Image manipulating start =========="
	$(Q) sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/ram_3_s.bin 32
	$(Q) sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/ram_3_nsc.bin 32

	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/ram_3_s.bin  __ram_image3_text_start__  $(IMAGE_TARGET_FOLDER)/tfm_s.map
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/ram_3_nsc.bin  __ram_image3_nsc_start__  $(IMAGE_TARGET_FOLDER)/tfm_s.map

	$(Q)cat $(IMAGE_TARGET_FOLDER)/ram_3_s_prepend.bin $(IMAGE_TARGET_FOLDER)/ram_3_nsc_prepend.bin > $(IMAGE_TARGET_FOLDER)/km4_image3_all.bin

	sh $(UTILITYDIR)/image_tool/imagetool.sh $(IMAGE_TARGET_FOLDER)/km4_image3_all.bin $(BUILD_TYPE)

	@echo "========== Image manipulating end =========="
endif

linker_image3:
ifeq ($(CONFIG_TRUSTZONE), y)
	@echo "========= linker img3 secure start ========="
	$(ECHO) "  LD      target_img3.axf"
	$(Q)$(LD) $(LD_SEC_OUT_ARG) $(LD_ARG) target_img3.axf $(RAM_OBJS_LIST) $(LINK_ROM_LIB)

	$(Q)$(MOVE) cmse_implib.lib $(IMAGE_TARGET_FOLDER)/cmse_implib.lib
	$(Q)$(MOVE) text.map $(IMAGE_TARGET_FOLDER)/text_image3.map
	$(Q)$(MOVE) target_img3.axf $(IMAGE_TARGET_FOLDER)
	$(Q)$(NM) $(IMAGE_TARGET_FOLDER)/target_img3.axf | sort > $(IMAGE_TARGET_FOLDER)/target_img3.map
	$(Q)$(OBJDUMP) -d $(IMAGE_TARGET_FOLDER)/target_img3.axf > $(IMAGE_TARGET_FOLDER)/target_img3.asm
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/target_img3.axf $(IMAGE_TARGET_FOLDER)/target_pure_img3.axf
	$(Q)$(STRIP) $(IMAGE_TARGET_FOLDER)/target_pure_img3.axf
	$(Q)$(NM) $(IMAGE_TARGET_FOLDER)/target_pure_img3.axf | sort > $(IMAGE_TARGET_FOLDER)/target_pure_img3.map

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

	$(Q)sh $(UTILITYDIR)/image_tool/imagetool.sh $(IMAGE_TARGET_FOLDER)/km4_image3_all.bin $(BUILD_TYPE)
	@echo "========== Image manipulating end =========="
endif

linker_image2_raw:
	@echo "========= linker img2_ns start ========="
	$(ECHO) "  LD      target_img2.axf"
ifeq (${CODE_ANA},1)
	$(Q)$(LD) $(LD_ARG) target_img2.axf $(RAM_OBJS_LIST) $(LINK_APP_LIB) $(LINK_ROM_LIB_NS) $(LINK_THIRD_APP_LIB) \
	$(ASDK_TOOLCHAIN)/lib/gcc/arm-none-eabi/$(ASDK_VER)/thumb/v8-m.main+fp/fpu/crtbegin.o \
	$(ASDK_TOOLCHAIN)/lib/gcc/arm-none-eabi/$(ASDK_VER)/thumb/v8-m.main+fp/fpu/crtend.o -lm -lstdc++
else
	$(Q)$(CD) $(BUILD_TARGET_LIB_FOLDER); for libname in $(LINK_APP_LIB); do $(AR) x $$libname ; done
	$(Q)$(LD) $(LD_ARG) target_img2.axf $(RAM_OBJS_LIST) $(LIB_OBJS_LIST) $(LINK_ROM_LIB_NS) $(LINK_THIRD_APP_LIB) \
	$(ASDK_TOOLCHAIN)/lib/gcc/arm-none-eabi/$(ASDK_VER)/thumb/v8-m.main+fp/fpu/crtbegin.o \
	$(ASDK_TOOLCHAIN)/lib/gcc/arm-none-eabi/$(ASDK_VER)/thumb/v8-m.main+fp/fpu/crtend.o -lm -lstdc++
endif
	$(Q)$(MOVE) text.map $(IMAGE_TARGET_FOLDER)/text_image2_ns.map
	$(Q)$(MOVE) target_img2.axf $(IMAGE_TARGET_FOLDER)
	$(Q)$(NM) $(IMAGE_TARGET_FOLDER)/target_img2.axf | sort > $(IMAGE_TARGET_FOLDER)/target_img2.map
	$(Q)$(OBJDUMP) -d $(IMAGE_TARGET_FOLDER)/target_img2.axf > $(IMAGE_TARGET_FOLDER)/target_img2.asm
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/target_img2.axf $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/target_img2.asm $(KM0_BUILDDIR)/asdk/image/target_img2_otrcore.asm
	$(Q)$(STRIP) $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf

linker_image2_ns: linker_image2_raw
	$(ECHO) "  BIN      km4_image2_all.bin"
ifeq ($(CONFIG_MP_SHRINK), y)
	$(Q)$(FROMELF) -j .sram_image2.text.data -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf $(IMAGE_TARGET_FOLDER)/sram_2.bin
else
	$(Q)$(FROMELF) -j .ram_image2.entry -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf $(IMAGE_TARGET_FOLDER)/entry.bin
	$(Q)$(FROMELF) -j .sram_image2.text.data -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf $(IMAGE_TARGET_FOLDER)/sram_2.bin

endif
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

	@# 32Bytes aligned for rsip #
	$(Q) sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/xip_image2.bin 32
	$(Q) sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/sram_2.bin 32
	$(Q) sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/psram_2.bin 32

ifeq ($(CONFIG_MP_SHRINK), y)
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/sram_2.bin  __image2_entry_func__  $(IMAGE_TARGET_FOLDER)/target_img2.map
	$(Q)cat $(IMAGE_TARGET_FOLDER)/sram_2_prepend.bin > $(IMAGE_TARGET_FOLDER)/km4_image2_all_shrink.bin
	$(Q)sh $(UTILITYDIR)/image_tool/imagetool.sh $(IMAGE_TARGET_FOLDER)/km4_image2_all_shrink.bin $(BUILD_TYPE);

else
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/entry.bin  __image2_entry_func__  $(IMAGE_TARGET_FOLDER)/target_img2.map
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/sram_2.bin  __sram_image2_start__  $(IMAGE_TARGET_FOLDER)/target_img2.map
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/psram_2.bin  __psram_image2_start__  $(IMAGE_TARGET_FOLDER)/target_img2.map
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/xip_image2.bin  __flash_text_start__  $(IMAGE_TARGET_FOLDER)/target_img2.map
	$(Q)cat $(IMAGE_TARGET_FOLDER)/xip_image2_prepend.bin $(IMAGE_TARGET_FOLDER)/sram_2_prepend.bin $(IMAGE_TARGET_FOLDER)/psram_2_prepend.bin $(IMAGE_TARGET_FOLDER)/entry_prepend.bin > $(IMAGE_TARGET_FOLDER)/km4_image2_all.bin
	$(Q)sh $(UTILITYDIR)/image_tool/imagetool.sh $(IMAGE_TARGET_FOLDER)/km4_image2_all.bin $(BUILD_TYPE);

endif

ifeq ($(CONFIG_FATFS_WITHIN_APP_IMG), y)
	@if [ -f $(IMAGE_TARGET_FOLDER)/km0_km4_app.bin ] && [ -f $(BASEDIR)/amebadplus_gcc_project/fatfs.bin ]; then \
		mv $(IMAGE_TARGET_FOLDER)/km0_km4_app.bin $(IMAGE_TARGET_FOLDER)/km0_km4_app_tmp.bin; \
		sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/km0_km4_app_tmp.bin 4096; \
		$(PREPENDTOOL) $(BASEDIR)/amebadplus_gcc_project/fatfs.bin  VFS1_FLASH_BASE_ADDR  $(IMAGE_TARGET_FOLDER)/target_img2.map; \
		cat $(IMAGE_TARGET_FOLDER)/km0_km4_app_tmp.bin $(BASEDIR)/amebadplus_gcc_project/fatfs_prepend.bin > $(IMAGE_TARGET_FOLDER)/km0_km4_app.bin; \
	fi
endif
	
ifeq ($(CONFIG_UPGRADE_BOOTLOADER), y)
	@if [ -f $(IMAGE_TARGET_FOLDER)/km0_km4_app.bin ]; then \
		$(OTAPREPENDTOOL) $(IMAGE_TARGET_FOLDER)/km0_km4_app.bin $(IMAGE_TARGET_FOLDER)/km4_boot_all.bin; \
		$(OTAPREPENDTOOL) $(IMAGE_TARGET_FOLDER)/km0_km4_app_ns.bin $(IMAGE_TARGET_FOLDER)/km4_boot_all_ns.bin; \
	fi
else
	@if [ -f $(IMAGE_TARGET_FOLDER)/km0_km4_app.bin ]; then \
		$(OTAPREPENDTOOL) $(IMAGE_TARGET_FOLDER)/km0_km4_app.bin; \
		$(OTAPREPENDTOOL) $(IMAGE_TARGET_FOLDER)/km0_km4_app_ns.bin; \
	fi
endif


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
ifeq ($(CONFIG_MP_SHRINK), y)
	$(Q)$(FROMELF) -j .ram_image1.entry -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_loader.axf $(IMAGE_TARGET_FOLDER)/entry_1.bin
	$(Q)$(FROMELF) -j .ram_image1.text -j .ram_image1.data -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_loader.axf $(IMAGE_TARGET_FOLDER)/ram_1.bin
else
	$(Q)$(FROMELF) -j .ram_image1.entry  -j .ram_image1.text -j .ram_image1.data \
	-Obinary $(IMAGE_TARGET_FOLDER)/target_pure_loader.axf $(IMAGE_TARGET_FOLDER)/ram_1.bin
endif

	$(Q)$(FROMELF) -j .xip_image1.text \
	-Obinary $(IMAGE_TARGET_FOLDER)/target_pure_loader.axf $(IMAGE_TARGET_FOLDER)/xip_boot.bin

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
ifeq ($(CONFIG_MP_SHRINK), y)
	##entry 1k for flashloader
	$(Q) sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/entry_1.bin 992
endif
	$(Q) sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/ram_1.bin 32
	$(Q) sh $(UTILITYDIR)/pad.sh $(IMAGE_TARGET_FOLDER)/xip_boot.bin 32

ifeq ($(CONFIG_MP_SHRINK), y)
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/entry_1.bin  __ram_start_table_start__  $(IMAGE_TARGET_FOLDER)/target_loader.map
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/ram_1.bin  __ram_image1_text_start__  $(IMAGE_TARGET_FOLDER)/target_loader.map

	$(Q)cat $(IMAGE_TARGET_FOLDER)/entry_1_prepend.bin $(IMAGE_TARGET_FOLDER)/ram_1_prepend.bin > $(IMAGE_TARGET_FOLDER)/km4_boot.bin

	sh $(UTILITYDIR)/image_tool/imagetool.sh $(IMAGE_TARGET_FOLDER)/km4_boot.bin $(BUILD_TYPE); \

else
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/ram_1.bin  __ram_start_table_start__  $(IMAGE_TARGET_FOLDER)/target_loader.map
	@if [ ${IMGTOOL_LOADER} == "YES" ]; then \
		sh $(UTILITYDIR)/image_tool/imagetool.sh $(IMAGE_TARGET_FOLDER)/ram_1_prepend.bin $(BUILD_TYPE); \
	else \
		$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/xip_boot.bin  __km4_boot_text_start__  $(IMAGE_TARGET_FOLDER)/target_loader.map; \
		cat $(IMAGE_TARGET_FOLDER)/xip_boot_prepend.bin $(IMAGE_TARGET_FOLDER)/ram_1_prepend.bin > $(IMAGE_TARGET_FOLDER)/km4_boot.bin; \
		sh $(UTILITYDIR)/image_tool/imagetool.sh $(IMAGE_TARGET_FOLDER)/km4_boot.bin $(BUILD_TYPE); \
	fi
endif
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
ifeq ($(CONFIG_RDP_TFM), y)
	$(call CLEAN_BY_SEARCH_FUNC, $(TFM_NS_DIR))
	@rm $(TFM_NS_DIR)/interface/ -rf
	@rm build_tfm/ -rf
endif
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
ifeq ($(CONFIG_MP_SHRINK), y)
	$(Q) $(CC) -E -P -xc -c ./ld/ameba_img1_all_mp.ld -o ./ld/ameba_img1.ld
else
	$(Q) $(CC) -E -P -xc -c ./ld/ameba_img1_all.ld -o ./ld/ameba_img1.ld
endif
	$(Q)$(MOVE) ./ld/ameba_img1.ld  $(BUILD_TARGET_FOLDER)/rlx8721d.ld
	cat $(LINK_ROM_SYMBOL_S) >> $(BUILD_TARGET_FOLDER)/rlx8721d.ld
copy_ld_floader:
	$(Q) $(CC) -E -P -xc -c -DGENERATE_FLOADER_LD ./ld/ameba_img1_all.ld -o ./ld/ameba_floader.ld
	$(Q)$(MOVE) ./ld/ameba_floader.ld  $(BUILD_TARGET_FOLDER)/rlx8721d.ld
	cat $(LINK_ROM_SYMBOL_S) >> $(BUILD_TARGET_FOLDER)/rlx8721d.ld
copy_ld_img2:
ifeq ($(CONFIG_MP_SHRINK), y)
	$(Q)$(CC) -E -P -xc -c $(ROOTDIR)/ld/ameba_img2_all_mp.ld  -o $(ROOTDIR)/ld/ameba_img2.ld
else
	$(Q) $(CC) -E -P -xc -c $(ROOTDIR)/ld/ameba_img2_all.ld  -o $(ROOTDIR)/ld/ameba_img2.ld
endif
	$(Q)$(MOVE) $(ROOTDIR)/ld/ameba_img2.ld  $(BUILD_TARGET_FOLDER)/rlx8721d.ld
ifeq ($(CONFIG_TRUSTZONE), y)
	$(Q)cat $(LINK_ROM_SYMBOL_NS) >> $(BUILD_TARGET_FOLDER)/rlx8721d.ld
else
	$(Q)cat $(LINK_ROM_SYMBOL_S) >> $(BUILD_TARGET_FOLDER)/rlx8721d.ld
endif
	$(Q)cat $(LINK_ROM_SYMBOL_STDLIB) >> $(BUILD_TARGET_FOLDER)/rlx8721d.ld
copy_ld_img3:
	$(Q) $(CC) -E -P -xc -c ./ld/ameba_img3_all.ld  -o ./ld/ameba_img3.ld
	$(Q)$(MOVE) ./ld/ameba_img3.ld  $(BUILD_TARGET_FOLDER)/rlx8721d.ld
	$(Q)cat $(LINK_ROM_SYMBOL_S) >> $(BUILD_TARGET_FOLDER)/rlx8721d.ld

copy_ld_tfm:
ifeq ($(CONFIG_RDP_TFM), y)
	$(Q) $(CC) -E -P -xc -c ../../amebaDplus_layout.ld  -o $(TFM_LD_DIR)/tfm_amebadplus_s.ld
	$(Q)cat ./ld/ameba_tfm.ld >> $(TFM_LD_DIR)/tfm_amebadplus_s.ld
	$(Q)cat $(LINK_ROM_SYMBOL_S) >> $(TFM_LD_DIR)/tfm_amebadplus_s.ld
endif
#*****************************************************************************#
#              COPY flashloader into flashdownload folder                     #
#*****************************************************************************#
copy_flashloader:
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/target_loader.axf 	$(GNU_SCRIPT)/acut_loader/target_FPGA.axf -f
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/ram_1.bin 			$(GNU_SCRIPT)/acut_loader/flash_loader_ram_1.bin -f

copy_imgtool_flashloader:
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/target_loader.axf 		$(FLOADER_BIN_DIR)/target_FPGA.axf -f
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/imgtool_flashloader.bin 	$(FLOADER_BIN_DIR)/imgtool_flashloader.bin -f
	$(Q)$(MOVE) $(IMAGE_TARGET_FOLDER)/imgtool_flashloader.bin 	$(FLOADER_BIN_DIR)/floader_amebadplus.bin -f

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
