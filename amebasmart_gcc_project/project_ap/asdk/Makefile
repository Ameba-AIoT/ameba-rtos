
#*****************************************************************************#
#                            Basic Configurations                             #
#*****************************************************************************#

ABS_ROOTDIR		= $(shell pwd)
SCRIPT_DIR		= $(ROOTDIR)/../../../../tools/image_scripts/internal
MAKE_INCLUDE_GEN	= $(ABS_ROOTDIR)/Makefile.include.gen

WIFI_MAKE_DIR = make/wlan
APP_LIB_DIR = $(ROOTDIR)/lib/application

CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
          else if [ -x /bin/bash ]; then echo /bin/bash; \
          else echo sh; fi ; fi)

CODE_ANALYZE_PYTHON=python $(SCRIPT_DIR)/code_analyze.py
STATIC_ANALYZE_PYTHON=python $(SCRIPT_DIR)/static_code_analyze.py
ifeq ($(CONFIG_MP_INCLUDED), y)
    ANALYZE_MP_IMG = 1
else
    ANALYZE_MP_IMG = 0
endif
#ROM_SYMBOL_GEN_PYTHON=$(shell $(ABS_ROOTDIR)/img_utility/export_rom_symbol.py)
#ROM_SYMBOL_GEN_PYTHON= 
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
DAILY_BUILD = ${V}
export ABS_ROOTDIR MAKE_INCLUDE_GEN HOSTCC Q ECHO


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

ifeq ($(CONFIG_AMEBASMART_A_CUT),y)
ROM_LIB_DIR = $(ROOTDIR)/lib/amebasmart_rom_acut
endif

RAM_OBJS_LIST = ./build/ram/*.o
ROM_OBJS_LIST = ./build/rom/*.o
LIB_OBJS_LIST = ./build/lib/*.o

LINK_ROM_LIB = 
LINK_ROM_SYMBOL = 
LINK_APP_LIB =
LINK_THIRD_APP_LIB =
.PHONY: all build_target_folder menuconfig clean build_libraries make_wifi_lib

APP_LIB = $(APP_LIB_DIR)/lib_httpc.a \
	$(APP_LIB_DIR)/lib_httpd.a \
	$(APP_LIB_DIR)/lib_websocket.a \
	$(APP_LIB_DIR)/lib_ipnat.a \
	$(APP_LIB_DIR)/lib_ipdnrd.a \
	#$(APP_LIB_DIR)/lib_mdns.a \
	#$(APP_LIB_DIR)/lib_coap.a \
	#$(APP_LIB_DIR)/lib_eap.a \

ifeq ($(CONFIG_WLAN),y)
# AP Link Library
ifeq ($(CONFIG_AS_INIC_AP), y)
LINK_APP_LIB += $(APP_LIB)

# NP Link Library
else ifeq ($(CONFIG_AS_INIC_NP),y)
ifeq ($(CONFIG_MP_INCLUDED), y)
LINK_APP_LIB += $(APP_LIB_DIR)/lib_coex_mp.a
else
LINK_APP_LIB += $(APP_LIB_DIR)/lib_coex.a
endif
endif
endif

ifeq ($(findstring y, $(CONFIG_USB_DEVICE_EN)), y)
LINK_APP_LIB += $(APP_LIB_DIR)/lib_usbd.a
endif
ifeq ($(findstring y, $(CONFIG_USB_HOST_EN)), y)
LINK_APP_LIB += $(APP_LIB_DIR)/lib_usbh.a
endif
ifeq ($(findstring y, $(CONFIG_USB_DRD_EN)), y)
LINK_APP_LIB += $(APP_LIB_DIR)/lib_usb.a
endif
ifeq ($(CONFIG_MP_SHRINK), y)
ifeq ($(CONFIG_CORE_NUM),1)
LINK_APP_LIB += $(ROOTDIR)/lib/soc/lib_freertos_smp_sc_mp.a
else
LINK_APP_LIB += $(ROOTDIR)/lib/soc/lib_freertos_smp_mp.a
endif
else
ifeq ($(CONFIG_CORE_NUM),1)
LINK_APP_LIB += $(ROOTDIR)/lib/soc/lib_freertos_smp_sc.a
else
LINK_APP_LIB += $(ROOTDIR)/lib/soc/lib_freertos_smp.a
endif
endif
LINK_APP_LIB += $(ROOTDIR)/lib/soc/lib_chipinfo.a
LINK_APP_LIB += $(ROOTDIR)/lib/soc/lib_pmc.a
#LINK_APP_LIB += $(ROOTDIR)/lib/soc/lib_Vmin_Test.a
ifeq ($(CONFIG_802154_THREAD_EN), y)
LINK_THIRD_APP_LIB += $(APP_LIB_DIR)/lib_openthread.a
endif

ifeq ($(CONFIG_WIFI_AUDIO),y)
LINK_THIRD_APP_LIB += $(APP_LIB_DIR)/lib_wifi_audio.a
LINK_THIRD_APP_LIB += $(APP_LIB_DIR)/lib_flac.a
endif

ifneq ($(CONFIG_MEDIA_PLAYER)$(CONFIG_MEDIA_LITE_PLAYER),)
LINK_APP_LIB += $(APP_LIB_DIR)/lib_playback.a
ifneq ($(CONFIG_MEDIA_CODEC_AAC)$(CONFIG_MEDIA_LITE_CODEC_AAC),)
LINK_THIRD_APP_LIB += $(APP_LIB_DIR)/lib_fdkaac.a
endif
ifneq ($(CONFIG_MEDIA_CODEC_HAAC)$(CONFIG_MEDIA_LITE_CODEC_HAAC),)
LINK_THIRD_APP_LIB += $(APP_LIB_DIR)/lib_haac.a
endif
ifeq ($(CONFIG_MEDIA_CODEC_OPUS),y)
LINK_THIRD_APP_LIB += $(APP_LIB_DIR)/lib_opus.a
endif
ifneq ($(CONFIG_MEDIA_DEMUX_FLAC)$(CONFIG_MEDIA_DEC_FLAC)$(CONFIG_MEDIA_ENC_FLAC),)
LINK_THIRD_APP_LIB += $(APP_LIB_DIR)/lib_flac.a
endif
ifeq ($(CONFIG_MEDIA_DEMUX_OGG),y)
LINK_THIRD_APP_LIB += $(APP_LIB_DIR)/lib_vorbisdec.a
endif
endif

ifeq ($(CONFIG_SPEEX_LIB),y)
LINK_THIRD_APP_LIB += $(APP_LIB_DIR)/lib_speexdsp.a
endif
ifeq ($(CONFIG_OPUS_LIB),y)
LINK_THIRD_APP_LIB += $(APP_LIB_DIR)/lib_opus.a
endif

ifeq ($(CONFIG_AUDIO_MIXER),y)
LINK_APP_LIB += $(APP_LIB_DIR)/lib_audio_mixer.a
else ifeq ($(CONFIG_AUDIO_PASSTHROUGH),y)
LINK_APP_LIB += $(APP_LIB_DIR)/lib_audio_passthrough.a
endif

ifeq ($(CONFIG_GUI_EN),y)
ifeq ($(CONFIG_DEMO_UI_EN),y)
LINK_APP_LIB += $(APP_LIB_DIR)/lib_ui_utils.a
endif
LINK_APP_LIB += $(APP_LIB_DIR)/lib_ui_drivers.a
endif

ifeq ($(CONFIG_AUDIO_FWK),y)
LINK_APP_LIB += $(APP_LIB_DIR)/lib_base.a
endif

ifeq ($(CONFIG_IMQ_EN),y)
LINK_APP_LIB += $(APP_LIB_DIR)/lib_rpc.a
endif

ifeq ($(CONFIG_BT), y)
ifneq ($(CONFIG_BT_INIC), y)
ifeq ($(CONFIG_BT_AUDIO_CODEC_SUPPORT), y)
LINK_THIRD_APP_LIB += $(APP_LIB_DIR)/lib_btcodec.a
LINK_THIRD_APP_LIB += $(APP_LIB_DIR)/lib_btaudiosync.a
endif
ifneq ($(CONFIG_BT_ZEPHYR), y)
ifeq ($(CONFIG_BT_BLE_ONLY), y)
LINK_THIRD_APP_LIB += $(BASEDIR)/component/bluetooth/rtk_stack/platform/amebasmart/lib/ca32/ble_only/btgap.a
else ifeq ($(CONFIG_BT_DUAL_MODE), y)
LINK_THIRD_APP_LIB += $(BASEDIR)/component/bluetooth/rtk_stack/platform/amebasmart/lib/ca32/dual_mode/btgap.a
LINK_THIRD_APP_LIB += $(BASEDIR)/component/bluetooth/rtk_stack/platform/amebasmart/lib/ca32/bt_mgr.a
endif
LINK_THIRD_APP_LIB += $(BASEDIR)/component/bluetooth/rtk_stack/platform/amebasmart/lib/ca32/leaudio.a
LINK_THIRD_APP_LIB += $(BASEDIR)/component/bluetooth/rtk_stack/platform/amebasmart/lib/ca32/ble_mgr.a
LINK_THIRD_APP_LIB += $(BASEDIR)/component/bluetooth/rtk_stack/platform/amebasmart/lib/ca32/sysm.a
ifeq ($(findstring y, $(CONFIG_BT_MESH_PROVISIONER_SUPPORT) $(CONFIG_BT_MESH_DEVICE_SUPPORT)), y)
LINK_THIRD_APP_LIB += $(BASEDIR)/component/bluetooth/rtk_stack/src/mesh/lib/amebasmart/ca32/btmesh.a
endif
endif
endif
endif

ifneq ($(wildcard ./$(WIFI_MAKE_DIR)/Makefile.lib.wifi),)
include ./$(WIFI_MAKE_DIR)/Makefile.lib.wifi
make_subdirs_lib_img2: make_wifi_lib
endif

prebuild: build_target_folder check_toolchain build_info.h
#all: check_toolchain copy_ld_sheipa make_atf make_rtos linker_rtos gen_image
bootloader: prebuild make_atf
image2: prebuild copy_ld_img2 make_subdirs linker_image2 fip_image code_analyze
lib_image2: prebuild copy_ld_img2 make_subdirs_lib_img2
#*****************************************************************************#
#                      RULES TO CREATE DIRECTORY                              #
#*****************************************************************************#

build_target_folder:
	@if [ -d $(BUILD_TARGET_FOLDER) ]; then echo "$(BUILD_TARGET_FOLDER) exist"; else $(MKDIR) -p $(BUILD_TARGET_FOLDER); fi
	@if [ -d $(IMAGE_TARGET_FOLDER) ]; then echo "$(IMAGE_TARGET_FOLDER) exist"; else $(MKDIR) -p $(IMAGE_TARGET_FOLDER); fi
	@if [ -d $(BUILD_TARGET_LIB_FOLDER) ]; then echo "$(BUILD_TARGET_LIB_FOLDER) exist"; else $(MKDIR) $(BUILD_TARGET_LIB_FOLDER); fi
	@if [ -d $(BUILD_TARGET_RAM_FOLDER) ]; then echo "$(BUILD_TARGET_RAM_FOLDER) exist"; else $(MKDIR) $(BUILD_TARGET_RAM_FOLDER); fi
	@if [ -d $(BUILD_TARGET_ROM_FOLDER) ]; then echo "$(BUILD_TARGET_ROM_FOLDER) exist"; else $(MKDIR) $(BUILD_TARGET_ROM_FOLDER); fi
	@if [ -d $(ROM_LIB_DIR) ]; then echo "$(ROM_LIB_DIR) exist"; else $(MKDIR) $(ROM_LIB_DIR); fi
	@if [ -d $(TARGET_LIBAPP_FOLDER) ]; then echo "$(TARGET_LIBAPP_FOLDER) exist"; else $(MKDIR) -p $(TARGET_LIBAPP_FOLDER); fi
	@if [ -d $(TARGET_LIBSOC_FOLDER) ]; then echo "$(TARGET_LIBSOC_FOLDER) exist"; else $(MKDIR) -p $(TARGET_LIBSOC_FOLDER); fi
#*****************************************************************************#
#                      RULES TO MAKE SUB DIRS                                 #
#*****************************************************************************#
make_subdirs:
	@rm -f $(RAM_OBJS_LIST)
	@echo > $(ROOTDIR)/ram_size.txt
	@echo > $(ROOTDIR)/obj_list.txt
	@make -C make all
	@$(MOVE) $(ROOTDIR)/ram_size.txt $(IMAGE_TARGET_FOLDER)
	@$(MOVE) $(ROOTDIR)/obj_list.txt $(IMAGE_TARGET_FOLDER)

make_subdirs_lib_img2:
	@ rm -f $(RAM_OBJS_LIST)
	#@make -C make/target/wifi_rcu_fw all
	@ rm -f $(RAM_OBJS_LIST)

	@+$(MAKE) -C make/usb_otg/device/lib all
	@+$(MAKE) -C make/usb_otg/host/lib all
	@+$(MAKE) -C make/usb_otg/drd/lib all


ifeq ($(CONFIG_WLAN),y)
# build AP libs
ifeq ($(CONFIG_AS_INIC_AP), y)
	@make -C make/network/httpc all
	@make -C make/network/httpd all
	@make -C make/network/websocket all
	@+$(MAKE) -C make/network/ipnat all
	@+$(MAKE) -C make/network/ipdnrd all
	#@+$(MAKE) -C make/network/eap all
	#@+$(MAKE) -C make/network/http2 all
	#@+$(MAKE) -C make/network/tftp all
endif
endif

# build audio libs
	@+$(MAKE) -C make/audio lib
# build wifi-audio lib
ifeq ($(CONFIG_WIFI_AUDIO),y)
	@+$(MAKE) -C make/application/wifi-audio lib
endif
# build utils libs
	@+$(MAKE) -C make/utils lib

# build ui libs
	@+$(MAKE) -C make/ui lib
	@+$(MAKE) -C make/application/demoui lib

# build bt audio codec libs
ifeq ($(CONFIG_BT), y)
ifeq ($(CONFIG_BT_AUDIO_CODEC_SUPPORT), y)
	@+$(MAKE) -C make/bluetooth/bt_audio/bt_codec all
	@+$(MAKE) -C make/bluetooth/bt_audio/audio_sync all
endif
endif

make_atf:
	@make -C make/atf all

#*****************************************************************************#
#                      RULES TO LINKER IMAGE                                  #
#*****************************************************************************#
linker_image2_raw:
	@echo "========= linker img2 start ========="
	$(ECHO) "  LD      target_img2.axf"
	$(Q)$(CD) $(BUILD_TARGET_LIB_FOLDER); for libname in $(LINK_APP_LIB); do $(AR) x $$libname ; done
	$(Q)$(LD) $(LD_ARG) target_img2.axf $(RAM_OBJS_LIST) $(LINK_ROM_LIB) $(LIB_OBJS_LIST) $(LINK_THIRD_APP_LIB) \
	$(ASDK_TOOLCHAIN)/lib/gcc/arm-none-eabi/$(ASDK_VER)/v7-a+neon-fp16/fpu/crtbegin.o \
	$(ASDK_TOOLCHAIN)/lib/gcc/arm-none-eabi/$(ASDK_VER)/v7-a+neon-fp16/fpu/crtend.o -lm -lstdc++

	$(Q)$(MOVE) text.map $(IMAGE_TARGET_FOLDER)/text_image2.map
	$(Q)$(MOVE) target_img2.axf $(IMAGE_TARGET_FOLDER)
	$(Q)$(NM) $(IMAGE_TARGET_FOLDER)/target_img2.axf | sort > $(IMAGE_TARGET_FOLDER)/target_img2.map
	$(Q)$(OBJDUMP) -d $(IMAGE_TARGET_FOLDER)/target_img2.axf > $(IMAGE_TARGET_FOLDER)/target_img2.asm
	@#$(OBJDUMP) -h -S --disassemble-all $(IMAGE_TARGET_FOLDER)/target_img2.axf > $(IMAGE_TARGET_FOLDER)/target_img2.txt
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/target_img2.axf $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf
	$(Q)$(COPY) $(IMAGE_TARGET_FOLDER)/target_img2.asm $(KM4_BUILDDIR)/asdk/image/target_img2_otrcore.asm
	$(Q)$(STRIP) $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf

linker_image2: linker_image2_raw
	$(ECHO) "  BIN      ca32_image2_all.bin"

	@#let .ARM.extab/.ARM.exidx +- 1GB can jump to *(.text*)
	$(Q)ARMExAddr=0x$(shell grep __exidx_end $(IMAGE_TARGET_FOLDER)/target_img2.map | awk '{print $$1}'); \
	if [[ $$ARMExAddr -gt 0x60000000 ]] ; then \
		$(FROMELF) -j .code -j .ARM.extab -j .ARM.exidx -j .ctors -j .dtors -j .preinit_array -j .init_array -j .fini_array -j .data -j .mmu_tbl -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf $(IMAGE_TARGET_FOLDER)/ram_2.bin; \
		$(FROMELF) -j .xip_image2.text -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf $(IMAGE_TARGET_FOLDER)/xip_image2.bin; \
	else \
		$(FROMELF) -j .code -j .data -j .mmu_tbl -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf $(IMAGE_TARGET_FOLDER)/ram_2.bin; \
		$(FROMELF) -j .xip_image2.text -j .ARM.extab -j .ARM.exidx -j .ctors -j .dtors -j .preinit_array -j .init_array -j .fini_array -Obinary $(IMAGE_TARGET_FOLDER)/target_pure_img2.axf $(IMAGE_TARGET_FOLDER)/xip_image2.bin; \
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
	@echo "========== linker img2 end =========="

	@#$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/ram_2.bin  __ram_image2_text_start__  $(IMAGE_TARGET_FOLDER)/target_img2.map


	$(Q)cat $(IMAGE_TARGET_FOLDER)/ram_2.bin > $(IMAGE_TARGET_FOLDER)/ca32_image2_all.bin

fip_image:
	@echo "========== Image manipulating start =========="
	@#link ca32_image2_all.bin to gen fip.bin
	@make -C make/atf image
	$(Q) sh $(KM4_BUILDDIR)/asdk/gnu_utility/pad.sh $(IMAGE_TARGET_FOLDER)/bl1_sram.bin 32
	$(Q) sh $(KM4_BUILDDIR)/asdk/gnu_utility/pad.sh $(IMAGE_TARGET_FOLDER)/bl1.bin 32
	$(Q) sh $(KM4_BUILDDIR)/asdk/gnu_utility/pad.sh $(IMAGE_TARGET_FOLDER)/fip.bin 32
	$(Q) sh $(KM4_BUILDDIR)/asdk/gnu_utility/pad.sh $(IMAGE_TARGET_FOLDER)/xip_image2.bin 32

	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/xip_image2.bin  __flash_text_start__  $(IMAGE_TARGET_FOLDER)/target_img2.map
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/bl1_sram.bin  __ca32_bl1_sram_start__  $(IMAGE_TARGET_FOLDER)/target_img2.map
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/bl1.bin  __ca32_bl1_dram_start__  $(IMAGE_TARGET_FOLDER)/target_img2.map
	$(Q)$(PREPENDTOOL) $(IMAGE_TARGET_FOLDER)/fip.bin  __ca32_fip_dram_start__  $(IMAGE_TARGET_FOLDER)/target_img2.map
	@cat $(IMAGE_TARGET_FOLDER)/xip_image2_prepend.bin $(IMAGE_TARGET_FOLDER)/bl1_sram_prepend.bin $(IMAGE_TARGET_FOLDER)/bl1_prepend.bin $(IMAGE_TARGET_FOLDER)/fip_prepend.bin > $(IMAGE_TARGET_FOLDER)/ap_image_all.bin

	$(Q)$(KM4_BUILDDIR)/asdk/gnu_utility/image_tool/imagetool.sh $(IMAGE_TARGET_FOLDER)/ap_image_all.bin $(BUILD_TYPE)

ifeq ($(CONFIG_FATFS_WITHIN_APP_IMG), y)
	@if [ -f $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app.bin ] && [ -f $(BASEDIR)/amebasmart_gcc_project/fatfs.bin ]; then \
		mv $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app.bin $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app_tmp.bin; \
		mv $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app_ns.bin $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app_ns_tmp.bin; \
		sh $(UTILITYDIR)/pad.sh $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app_tmp.bin 4096; \
		sh $(UTILITYDIR)/pad.sh $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app_ns_tmp.bin 4096; \
		$(PREPENDTOOL) $(BASEDIR)/amebasmart_gcc_project/fatfs.bin  VFS1_FLASH_BASE_ADDR  $(AP_IMAGEDIR)/target_img2.map; \
		cat $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app_tmp.bin $(BASEDIR)/amebasmart_gcc_project/fatfs_prepend.bin > $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app.bin; \
		cat $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app_ns_tmp.bin $(BASEDIR)/amebasmart_gcc_project/fatfs_prepend.bin > $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app_ns.bin; \
	fi
endif

ifeq ($(CONFIG_COMPRESS_OTA_IMG), y)
	@if [ -f $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app.bin ]; then \
		$(COMPRESSTOOL) $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app.bin; \
		$(COMPRESSTOOL) $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app_ns.bin; \
		$(COPY) $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app_compress.bin $(KM4_BUILDDIR)/asdk/image/tmp_app.bin; \
		$(COPY) $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app_ns_compress.bin $(KM4_BUILDDIR)/asdk/image/tmp_app_ns.bin; \
	fi
else
	@if [ -f $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app.bin ]; then \
		$(COPY) $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app.bin $(KM4_BUILDDIR)/asdk/image/tmp_app.bin; \
		$(COPY) $(KM4_BUILDDIR)/asdk/image/km0_km4_ca32_app_ns.bin $(KM4_BUILDDIR)/asdk/image/tmp_app_ns.bin; \
	fi
endif
ifeq ($(CONFIG_UPGRADE_BOOTLOADER), y)
	@if [ -f $(KM4_BUILDDIR)/asdk/image/tmp_app.bin ]; then \
		$(OTAPREPENDTOOL) $(KM4_BUILDDIR)/asdk/image/tmp_app.bin $(KM4_BUILDDIR)/asdk/image/km4_boot_all.bin; \
		$(OTAPREPENDTOOL) $(KM4_BUILDDIR)/asdk/image/tmp_app_ns.bin $(KM4_BUILDDIR)/asdk/image/km4_boot_all_ns.bin; \
	fi
else
	@if [ -f $(KM4_BUILDDIR)/asdk/image/tmp_app.bin ]; then \
		$(OTAPREPENDTOOL) $(KM4_BUILDDIR)/asdk/image/tmp_app.bin; \
		$(OTAPREPENDTOOL) $(KM4_BUILDDIR)/asdk/image/tmp_app_ns.bin; \
	fi
endif
	@rm -f $(KM4_BUILDDIR)/asdk/image/tmp_app.bin
	@rm -f $(KM4_BUILDDIR)/asdk/image/tmp_app_ns.bin

	@rm -f $(IMAGE_TARGET_FOLDER)/*prepend.bin
	@$(COPY) $(IMAGE_TARGET_FOLDER)/*rom* $(ROM_LIB_DIR)
	@$(COPY) ./build/atf/bl1/{*.dump,*.elf,*.ld,*.map} $(ROM_LIB_DIR)

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

gdb: gdb_prepare
	$(COPY) $(FLASHDLIMGDIR_KM4)/*  $(FLASHDLIMGDIR)/
	$(COPY) $(GNU_SCRIPT)/acut_loader/*  $(GNU_SCRIPT)/

	$(GNU_SCRIPT)/SetupGDB_FPGA.sh $(GNU_SCRIPT)
	$(GDB) -x $(GNU_SCRIPT)/rtl_gdb_flash_write.txt
	

gdbboot: gdb_prepare
	$(GDB) -x $(GNU_SCRIPT)/rtl_gdb_jtag_boot.txt

gdbbootxip: gdb_prepare
	$(GDB) -x $(GNU_SCRIPT)/rtl_gdb_jtag_boot_xip.txt

gdbloadrom: gdb_prepare
	$(GDB) -x $(GNU_SCRIPT)/rtl_gdb_jtag_load_rom.txt

swdpwd:
	$(GDB) -x $(GNU_SCRIPT)/rtl_gdb_jtag_pwd.txt

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

debug_core0:
	$(GDB) -x $(GNU_SCRIPT)/rtl_gdb_debug_core0.txt

debug_core1:
	$(GDB) -x $(GNU_SCRIPT)/rtl_gdb_debug_core1.txt

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
	@#use CLEAN_BY_SEARCH_FUNC replace clean target
	@# make -C make clean
	# make -C config clean

	# atf/tools cannot use CLEAN_BY_SEARCH_FUNC because its objects are not in project directory.
	make -C make/atf clean

clean_all: clean

#*****************************************************************************#
#              PACKAGE COPY link script file-ld                               #
#*****************************************************************************#
copy_ld_img2:
ifeq ($(CONFIG_XIP_FLASH), y)
	$(Q)$(COPY) $(ROOTDIR)/ld/rlx8721d_img2_xip.ld  $(BUILD_TARGET_FOLDER)/rlx8721d.ld
else
	$(Q)$(COPY) $(ROOTDIR)/ld/rlx8721d_img2_dram.ld  $(BUILD_TARGET_FOLDER)/rlx8721d.ld
endif
