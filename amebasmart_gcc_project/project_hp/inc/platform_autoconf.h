
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
#define CONFIG_AS_INIC_NP 1
#undef  CONFIG_HIGH_TP_TEST
#undef  CONFIG_WIFI_CSI_ENABLE
#undef  CONFIG_WIFI_ANTDIV_ENABLE

/*
 * < CONFIG LWIP
 */


/*
 * < MENUCONFIG FOR KM4 CONFIG
 */

/*
 * < CONFIG CHIP
 */
#define CONFIG_AMEBASMART 1
#define ARM_CORE_CM4 1
#define CONFIG_FPGA 1
#define CONFIG_AMEBASMART_B_CUT 1

/*
 * < CONFIG TrustZone
 */
#undef  CONFIG_TRUSTZONE

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
 * < CONFIG SECURE TEST
 */
#undef  CONFIG_SEC_VERIFY

/*
 * < SSL Config
 */
#define CONFIG_USE_MBEDTLS_ROM 1
#define CONFIG_MBED_TLS_ENABLED 1
#undef  CONFIG_SSL_ROM_TEST

/*
 * < GUI Config
 */
#undef  CONFIG_GUI_EN

/*
 * < Audio Config
 */
#undef  CONFIG_AUDIO_FWK

/*
 * < IPC Message Queue Config
 */
#undef  CONFIG_IMQ_EN

/*
 * < Build Option
 */
#define CONFIG_TOOLCHAIN_ASDK 1
#undef  CONFIG_TOOLCHAIN_ARM_GCC
#undef  CONFIG_LINK_ROM_LIB
#define CONFIG_LINK_ROM_SYMB 1

/*
 * < CONFIG IAR PROJECT
 */
#if defined(CONFIG_INIC_IPC_DISABLE) && (CONFIG_INIC_IPC_DISABLE == 1)
#ifdef CONFIG_INIC_EN
#undef CONFIG_INIC_EN
#endif
#ifdef CONFIG_HIGH_TP_TEST
#undef CONFIG_HIGH_TP_TEST
#endif
#ifdef CONFIG_INIC_IPC
#undef CONFIG_INIC_IPC
#endif
#ifdef CONFIG_INIC_IPC_HIGH_TP
#undef CONFIG_INIC_IPC_HIGH_TP
#endif
#ifdef CONFIG_AS_INIC_NP
#undef CONFIG_AS_INIC_NP
#endif
#define CONFIG_SINGLE_CORE_WIFI 1
#define CONFIG_LWIP_LAYER 1
#endif
