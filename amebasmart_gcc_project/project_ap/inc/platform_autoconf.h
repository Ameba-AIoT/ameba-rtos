
/*
 * < CONFIG BOOT OPTION
 */
#undef  CONFIG_XIP_FLASH

/*
 * < CLINTWOOD Config
 */
#undef  CONFIG_CLINTWOOD

/*
 * < CONFIG Mass Production
 */
#undef  CONFIG_MP_INCLUDED

/*
 * < CONFIG Linux
 */
#undef  CONFIG_LINUX_FW_EN

/*
 * < CONFIG Shell CMD
 */
#undef  CONFIG_NEW_ATCMD
#undef  CONFIG_LONGER_CMD

/*
 * < CONFIG VFS
 */
#define CONFIG_VFS_LITTLEFS_INCLUDED 1
#undef  CONFIG_VFS_FATFS_INCLUDED

/*
 * < CONFIG OTA OPTION
 */
#undef  CONFIG_UPGRADE_BOOTLOADER

/*
 * < CONFIG Application Processor
 */
#define CONFIG_AP_CORE_CA32 1
#undef  CONFIG_AP_CORE_KM4

/*
 * < CONFIG BT
 */
#undef  CONFIG_BT_MENU

/*
 * < CONFIG WIFI
 */
#define CONFIG_WLAN_MENU 1
#define CONFIG_AS_INIC_KM4_NP_CA32_AP 1
#undef  CONFIG_SINGLE_CORE_WIFI_KM4
#define CONFIG_WLAN 1
#define CONFIG_AS_INIC_AP 1
#define CONFIG_LWIP_LAYER 1
#undef  CONFIG_HIGH_TP_TEST
#undef  CONFIG_ENABLE_WPS
#undef  CONFIG_WIFI_CSI_ENABLE
#undef  CONFIG_WIFI_ANTDIV_ENABLE

/*
 * < CONFIG LWIP
 */
#undef  CONFIG_FAST_DHCP


/*
 * < MENUCONFIG FOR CA32 CONFIG
 */

/*
 * < CONFIG CHIP
 */
#define CONFIG_AMEBASMART 1
#define ARM_CORE_CA32 1
#define CONFIG_FPGA 1
#define CONFIG_AMEBASMART_A_CUT 1

/*
 * < CONFIG SMP
 */
#define CONFIG_SMP_DUAL_CORE 1
#undef  CONFIG_SMP_SINGLE_CORE
#define CONFIG_CORE_NUM (2)

/*
 * < CONFIG OS
 */
#define CONFIG_KERNEL 1
#define PLATFORM_FREERTOS 1
#define TASK_SCHEDULER_DISABLED (0)

/*
 * < CONFIG USB
 */
#undef  CONFIG_USB_OTG_EN

/*
 * < CONFIG FUNCTION TEST
 */
#undef  CONFIG_PER_TEST

/*
 * < LWIP Config
 */
#undef  CONFIG_IP_NAT

/*
 * < SSL Config
 */
#define CONFIG_MBED_TLS_ENABLED 1
#undef  MBEDTLS_V_3_4_1_ENABLE
#define MBEDTLS_V_2_16_9_ENABLE 1
#define MBEDTLS_VERSION (0x02100300)

/*
 * < 802154 Config
 */
#undef  CONFIG_802154_EN

/*
 * < Audio Config
 */
#undef  CONFIG_AUDIO_FWK
#undef  CONFIG_MEDIA_PLAYER

/*
 * < IPC Message Queue Config
 */
#undef  CONFIG_IMQ_EN

/*
 * < GUI Config
 */
#undef  CONFIG_GUI_EN

/*
 * < Range Config
 */
#undef  CONFIG_RANGE_ENABLED

/*
 * < Build Option
 */
#define CONFIG_TOOLCHAIN_ASDK 1
#undef  CONFIG_TOOLCHAIN_ARM_GCC
#define CONFIG_LINK_ROM_LIB 1
#undef  CONFIG_LINK_ROM_SYMB

/*
 * < CONFIG IAR PROJECT
 */
#if defined(CONFIG_INIC_IPC_DISABLE) && (CONFIG_INIC_IPC_DISABLE == 1)
#ifdef CONFIG_WLAN
#undef CONFIG_WLAN
#endif
#ifdef CONFIG_HIGH_TP_TEST
#undef CONFIG_HIGH_TP_TEST
#endif
#ifdef CONFIG_AS_INIC_AP
#undef CONFIG_AS_INIC_AP
#endif
#ifdef CONFIG_LWIP_LAYER
#undef CONFIG_LWIP_LAYER
#endif
#endif
