#include "project_km55tz/platform_autoconf.h"
#include "../fwlib/include/hal_platform.h"

#undef KBYTES
#undef SRAM_BASE
#undef PSRAM_BASE

#define SECURE_ADDR_OFFSET           (0x10000000)
#define KBYTES(x)                    ((x) * 1024)
#define IMAGE_HEADER_LEN             (0x20)
#define KM55TZ_KM55NS_RAM_SIZE         KBYTES(8)
#define SRAM_BASE                    (0x20000000)
#define SRAM_END                     (0x20080000)
#define PSRAM_BASE                   (0x60000000)
/* PSRAM_END is the address window top = the largest PSRAM this chip supports
 * (RTL8720F: 8MB). Real heap top is runtime-measured (ChipInfo_PsramHeapTop),
 * so smaller boards shrink automatically and no customer configuration is needed. */
#define PSRAM_END                    (0x60800000)

/* layout configuration */
/* KM55TZ_RAM_TZ_SIZE / KM55TZ_PSRAM_TZ_SIZE: total TrustZone image3 region size. */
#if defined(CONFIG_TZ_S_SIZE)
#define TZ_S_SIZE                    (CONFIG_TZ_S_SIZE)
#else
#define TZ_S_SIZE                    (64)
#endif

#if defined(CONFIG_IMG3_SRAM) || defined(CONFIG_IMG3_FLASH)
#define KM55TZ_RAM_TZ_SIZE            KBYTES(TZ_S_SIZE)
#define KM55TZ_PSRAM_TZ_SIZE          KBYTES(0)
#define KM55TZ_MSP_RAM_NS_SIZE        KBYTES(2)
#elif defined(CONFIG_IMG3_PSRAM)
#define KM55TZ_RAM_TZ_SIZE            KBYTES(4)            /* minimum SRAM-TZ for .data/.bss in PSRAM mode */
#define KM55TZ_PSRAM_TZ_SIZE          KBYTES(TZ_S_SIZE)
#define KM55TZ_MSP_RAM_NS_SIZE        KBYTES(2)
#else /* undefine trustzone */
#define KM55TZ_RAM_TZ_SIZE            KBYTES(0)
#define KM55TZ_PSRAM_TZ_SIZE          KBYTES(0)
#define KM55TZ_MSP_RAM_NS_SIZE        KBYTES(0)
#endif

#if defined (CONFIG_WHC_DEV_HCI_BOOT)
#define KM55TZ_IMG1_SIZE              KBYTES(0)
#elif defined(CONFIG_IMG1_FLASH)
#define KM55TZ_IMG1_SIZE              KBYTES(4)
#elif defined(CONFIG_SOLO)
#define KM55TZ_IMG1_SIZE              KBYTES(28)
#else
#define KM55TZ_IMG1_SIZE              KBYTES(24)
#endif

#define KM55NS_IPC_RAM_SIZE           512              /* 512B */

#if defined(CONFIG_SOLO)
#define KM55NS_SHARE_RAM_SIZE         KBYTES(2)
#else
#define KM55NS_SHARE_RAM_SIZE         0
#endif


#if defined(CONFIG_WIFI_AP_STA_NUM) && (CONFIG_WIFI_AP_STA_NUM > 5)
#define RAM_KM55NS_IMG2_SIZE          KBYTES(CONFIG_SRAM_KM55NS_IMG2_KB + 2 * (CONFIG_WIFI_AP_STA_NUM - 5))
#else
#define RAM_KM55NS_IMG2_SIZE          KBYTES(CONFIG_SRAM_KM55NS_IMG2_KB)
#endif

/*
                  RTL8720F SRAM Layout (512K)
0x20000000 ──────────►┌──────────────────────┐
                      │     KM55TZ/KM55NS RAM  │
                      │       (12K)          │
                      │    <Realtek Fixed>   │
                      │                      │
fix addr              │                      │
0x20002000 ──────────►├──────────────────────┼──┐
                      │KM55TZ Bootloader(IMG1)│  ├───────►  KM55TZ_IMG1_SIZE
                      │                      │  │
                   ┌──┼──────────────────────┼──┴─┐
                   │  │                      │    │
If Trustzone off,  │  │   Trustzone (IMG3)   │    │        If Trustzone on,
              ◄────┤  │                      │    ├─────►  IMG3 = KM55TZ_RAM_TZ_SIZE = CONFIG_TZ_S_SIZE KB
IMG3 size is 0.    │  │                      │    │
                   └──┼──────────────────────┼────┴─┐
                      │                      │      │
                      │                      │      │
                      │    KM55TZ IMG2 RAM    │      │
                      │                      │      ├───►  KM55TZ_IMG2_SIZE = KM55NS_IMG2_RAM_START - KM55TZ_IMG2_RAM_START
                      │                      │      │
                      │                      │      │
                      ├──────────────────────┼──────┴─┐
                      │                      │        │
                      │    KM55NS IMG2 RAM    │        │
                      │                      │        ├─►  RAM_KM55NS_IMG2_SIZE
                      │                      │        │
                      │                      │        │
                      │                      │        │
 0x20080000──────────►└──────────────────────┴────────┘
*/

/* BOOT(IMG1)/IMG3/IMG2 layout*/                                                            /* Trstzone On  (IMG1 On Flash) | Trustzone off (IMG1 On Flash)  */
#define KM55TZ_IMG1_RAM_SATRT          (SRAM_BASE + KM55TZ_KM55NS_RAM_SIZE)                    /* IMG1 On SRAM: 0x20002000 ~ 0x20008000 ; IMG1 On Flash: 0x20002000 ~ 0x20003000*/
#define KM55TZ_RAM_TZ_START            (KM55TZ_IMG1_RAM_SATRT + KM55TZ_IMG1_SIZE)              /* 0x20003000 | unavailable when TZ off */
#define KM55TZ_MSP_RAM_NS_START        (KM55TZ_RAM_TZ_START + KM55TZ_RAM_TZ_SIZE)              /* 0x20013000 ~ 0x20013800 | unavailable */
#define KM55TZ_IMG2_RAM_START          (KM55TZ_MSP_RAM_NS_START + KM55TZ_MSP_RAM_NS_SIZE)      /* 0x20013800 ~ 0x20064000 | 0x20003000 ~ 0x20064000 */
#if defined(CONFIG_WHC_DEV_HCI_BOOT)
/* HCI_BOOT: KM55NS sits at the bottom of SRAM, right after the fixed 8 KB area. KM55TZ takes [KM55NS_end .. KM55TZ_HCI_BD_RAM_END]. */
#define KM55NS_IMG2_RAM_START          (SRAM_BASE + KM55TZ_KM55NS_RAM_SIZE)                    /* 0x20002000 */
#define KM55TZ_HCI_BD_RAM_END          (0x20070000)
#else
#define KM55NS_IMG2_RAM_START          (SRAM_END - RAM_KM55NS_IMG2_SIZE)                      /* 0x20064000 ~ 0x20080000 | 0x20064000 ~ 0x20080000 */
#endif

/*
                  RTL8720F PSRAM Layout (window 8MB, runtime-measured capacity)
PSRAM_BASE      ──────────►┌──────────────────────┐
0x60000000                 │                      │
                        ┌──┤  KM55TZ BD PSRAM TZ   │
If Trustzone off,       │  │  (IMG3 on PSRAM)     ├─────►  KM55TZ_PSRAM_TZ_SIZE = KBYTES(CONFIG_TZ_S_SIZE)
  TZ_SIZE = 0.     ◄────┤  │                      │        0 when TrustZone off or IMG3 not on PSRAM.
                        └──┤                      │
                           ├──────────────────────┤
                           │                      │
                           │   KM55NS BD PSRAM     │
                           │   (NP core image2)   ├─────►  KBYTES(CONFIG_PSRAM_KM55NS_IMG2_KB)
                           │   right after TZ,    │        default 876K; Kconfig-configurable.
                           │   or 0 if NP uses no  │        Present only when NP places code/data
                           │   PSRAM              │        in PSRAM (IMG2_PSRAM/DATA_HEAP_PSRAM).
                           ├──────────────────────┤
                           │                      │
                           │   KM55TZ BD PSRAM     │
                           │   (AP core image2)   ├─────►  PSRAM_END - KM55TZ_IMG2_PSRAM_START
                           │   Fills to PSRAM_END │        (fills to 8MB window top).
                           │   window; heap top   │        Heap top = ChipInfo_PsramHeapTop()
                           │   = runtime-measured │        (runtime-measured capacity, may be
                           │   PSRAM capacity.    │        less than PSRAM_END on small boards).
                           │                      │
PSRAM_END       ──────────►└──────────────────────┘
0x60800000 (8MB)               chip max; real heap top capped to ChipInfo_PsramHeapTop()
*/

#define KM55TZ_PSRAM_TZ_START          (PSRAM_BASE)
#define KM55NS_IMG2_PSRAM_START        (KM55TZ_PSRAM_TZ_START + KM55TZ_PSRAM_TZ_SIZE)
#define KM55TZ_IMG2_PSRAM_START        (KM55NS_IMG2_PSRAM_START + KM55NS_PSRAM_IMG2_SIZE)

/* PSRAM layout: TZ → NP(KM55NS) → AP(KM55TZ), see ASCII diagram above.
 * NP(KM55NS) gets a segment only when it puts code/data in PSRAM;
 * else it collapses to 0 and all non-secure PSRAM folds into AP(KM55TZ). */
#if defined(CONFIG_IMG2_PSRAM) || defined(CONFIG_DATA_HEAP_PSRAM)
#define KM55NS_PSRAM_IMG2_SIZE         KBYTES(CONFIG_PSRAM_KM55NS_IMG2_KB)
#define KM55NS_BD_PSRAM_LENGTH         (KM55NS_PSRAM_IMG2_SIZE - IMAGE_HEADER_LEN)
#else
#define KM55NS_PSRAM_IMG2_SIZE         0
#define KM55NS_BD_PSRAM_LENGTH         0
#endif

MEMORY
{
    EMPTY_SECTION (rwx) :                     ORIGIN = 0x00000000, LENGTH = 0x00000000 - 0x00000000    /* This region shall be empty */

    /* KM55TZ(CPU0) ROM */
    KM4TZ_IROM (rx) :                         ORIGIN = 0x00020000, LENGTH = 0x00033C00 - 0x00020000    /* KM55TZ IROM_S: 79K */
    KM4TZ_DROM (rx) :                         ORIGIN = 0x00033C00, LENGTH = 0x00036B00 - 0x00033C00    /* KM55TZ DROM_S: 11.75k */
    KM4TZ_NS_ROM (rx) :                       ORIGIN = 0x00036B00, LENGTH = 0x00038000 - 0x00036B00    /* KM55TZ NS ROM: 5.25k */

    /* KM55NS ROM */
    KM4NS_ROM (rx) :                          ORIGIN = 0x00020000, LENGTH = 0x00024000 - 0x00020000    /* KM55NS KM4NS_ROM: 16k */

    /* KM55TZ Secure RAM, BIT(28)=1 */
    KM4TZ_ROMBSS_RAM_S (rw) :                 ORIGIN = 0x30000000, LENGTH = 0x30000400 - 0x30000000    /* KM55TZ ROM BSS RAM S: 1k */
    KM55TZ_MSP_RAM_S (rw) :                    ORIGIN = 0x30000400, LENGTH = 0x30001000 - 0x30000400    /* KM55TZ MSP_S RAM: 3k */

    /* KM55TZ Non-secure RAM */
    KM4TZ_ROMBSS_RAM_NS (rw) :                ORIGIN = 0x20001000, LENGTH = 0x20001380 - 0x20001000    /* KM55TZ ROM BSS NS RAM: 1k */
    GLOBAL_BSS_RAM (rw) :                     ORIGIN = 0x20001380, LENGTH = 0x20001400 - 0x20001380    /* IMAGE1/2 GLOBAL BSS RAM: 128B */

    /* KM55NS RAM */
    KM4NS_ROMBSS_RAM (rw) :                   ORIGIN = 0x20001400, LENGTH = 0x20001800 - 0x20001400    /* KM55NS ROM BSS RAM: 1k */
    KM55NS_MSP_RAM (rw) :                      ORIGIN = 0x20001800, LENGTH = 0x20002000 - 0x20001800    /* KM55NS MSP RAM: 2k */

    /* KM55TZ Secure RAM, BIT(28)=1 */
    KM55TZ_BOOTLOADER_RAM_S (rwx) :            ORIGIN = KM55TZ_IMG1_RAM_SATRT + SECURE_ADDR_OFFSET + IMAGE_HEADER_LEN, LENGTH = KM55TZ_IMG1_SIZE  - IMAGE_HEADER_LEN        /* KM55TZ BOOT Loader RAM: 4K or 32k */
    KM55TZ_FLOADER_RAM_S (rwx) :               ORIGIN = 0x30070020, LENGTH = 0x30080000 - 0x30070020    /* Floader / FULLMAC bootloader: 60k, 0x3007F000 ~ 0x30080000 for MANIFEST */

    /* FULLMAC */
    FULLMAC_TCM_RAM (rwx) :                   ORIGIN = 0x000F0000, LENGTH = 0x00100000 - 0x000F0000    /* FULLMAC TCM RAM: TZ+NS=64k */

    /* IMG2 RAM */
    KM55TZ_BD_RAM_TZ (rwx) :                   ORIGIN = KM55TZ_RAM_TZ_START + IMAGE_HEADER_LEN,         LENGTH = KM55TZ_RAM_TZ_SIZE - IMAGE_HEADER_LEN                               /* KM55TZ BD RAM TZ: CONFIG_TZ_S_SIZE KB */
    KM55TZ_MSP_RAM_NS (rw) :                   ORIGIN = KM55TZ_MSP_RAM_NS_START, LENGTH = KM55TZ_MSP_RAM_NS_SIZE                                                                     /* KM55TZ MSP_NS RAM: 2k */
#if defined(CONFIG_WHC_DEV_HCI_BOOT)
    KM55TZ_BD_RAM (rwx) :                      ORIGIN = (KM55NS_IMG2_RAM_START + RAM_KM55NS_IMG2_SIZE) + IMAGE_HEADER_LEN, LENGTH = KM55TZ_HCI_BD_RAM_END - ORIGIN(KM55TZ_BD_RAM)      /* KM55TZ BD RAM: 0x2001E020 ~ 0x2006FFFF */
#else
    KM55TZ_BD_RAM (rwx) :                      ORIGIN = KM55TZ_IMG2_RAM_START + IMAGE_HEADER_LEN, LENGTH = KM55NS_IMG2_RAM_START - KM55TZ_IMG2_RAM_START - IMAGE_HEADER_LEN           /* KM55TZ BD RAM: TZ On, 322KB; Tz off, 388K */
#endif

    KM55NS_IPC_RAM (rwx) :                     ORIGIN = KM55NS_IMG2_RAM_START, LENGTH = KM55NS_IPC_RAM_SIZE                                                                          /* KM55NS IPC RAM: 512B */
    KM55NS_SHARE_RAM (rwx) :                   ORIGIN = KM55NS_IMG2_RAM_START + KM55NS_IPC_RAM_SIZE, LENGTH = KM55NS_SHARE_RAM_SIZE                                                    /* SOLO KM55TZ<->KM55NS shared SRAM: 2K (with the IPC RAM above = 2.5K shared) */
    KM55NS_BD_RAM (rwx) :                      ORIGIN = KM55NS_IMG2_RAM_START + KM55NS_IPC_RAM_SIZE + KM55NS_SHARE_RAM_SIZE + IMAGE_HEADER_LEN,   LENGTH = RAM_KM55NS_IMG2_SIZE - KM55NS_IPC_RAM_SIZE - KM55NS_SHARE_RAM_SIZE - IMAGE_HEADER_LEN    /* KM55NS BD RAM */

    KM55TZ_BD_PSRAM_TZ (rwx) :                 ORIGIN = KM55TZ_PSRAM_TZ_START + IMAGE_HEADER_LEN,   LENGTH = KM55TZ_PSRAM_TZ_SIZE - IMAGE_HEADER_LEN                                   /* KM55TZ BD PSRAM TZ: CONFIG_TZ_S_SIZE KB */
    /* KM55TZ Non-secure PSRAM */
    KM55NS_BD_PSRAM (rwx) :                    ORIGIN = KM55NS_IMG2_PSRAM_START + IMAGE_HEADER_LEN,                    LENGTH = KM55NS_BD_PSRAM_LENGTH                                                    /* KM55NS BD PSRAM: CONFIG_PSRAM_KM55NS_IMG2_KB (default 876K), or 0 when NP uses no PSRAM */
    KM55TZ_BD_PSRAM (rwx) :                    ORIGIN = KM55TZ_IMG2_PSRAM_START + IMAGE_HEADER_LEN, LENGTH = (PSRAM_END - KM55TZ_IMG2_PSRAM_START) - IMAGE_HEADER_LEN                 /* KM55TZ BD PSRAM: last segment, fills to PSRAM_END; heap top capped to measured at runtime */

    /* Retention RAM */
    RETENTION_RAM (rwx) :                     ORIGIN = 0x4080AA00, LENGTH = 0x4080AC00 - 0x4080AA00    /* KM55NS Retention SRAM: 512B */

    /* Flash */
    KM55TZ_BOOT_XIP (rx) :                     ORIGIN = 0x10400020, LENGTH = 0x10C00000 - 0x10400020    /* KM55TZ IMG1 Virtual address: 8M */
    KM55TZ_IMG3_XIP (rx) :                     ORIGIN = 0x00C00020, LENGTH = 0x01000000 - 0x00C00020    /* KM55TZ IMG3 Virtual address: 4M */
    KM55NS_IMG2_XIP (rx) :                     ORIGIN = 0x02000020, LENGTH = 0x04000000 - 0x02000020    /* KM55TZ IMG2 Virtual address: 32M */
    KM55TZ_IMG2_XIP (rx) :                     ORIGIN = 0x04000020, LENGTH = 0x06000000 - 0x04000020    /* KM55NS IMG2 Virtual address: 32M */
    BTRACE (rx) :                             ORIGIN = 0xC8800000, LENGTH = 0xC9000000 - 0xC8800000    /* BT LOG Virtual address: 8192k */
    CTRACE (rx) :                             ORIGIN = 0xCA000000, LENGTH = 0xCA7FFFFF - 0xCA000000    /* COEX LOG Virtual address: 8192k */
    STRACE (rx) :                             ORIGIN = 0xCB000000, LENGTH = 0xCB7FFFFF - 0xCB000000    /* SDN LOG Virtual address: 8192k */
}

__ipc_memory_start__ = ORIGIN(KM55NS_IPC_RAM);
__km4ns_flash_text_start__ = ORIGIN(KM55NS_IMG2_XIP);
__km4ns_flash_text_end__ = ORIGIN(KM55NS_IMG2_XIP) + LENGTH(KM55NS_IMG2_XIP);
#if defined (CONFIG_SOLO)
__km55ns_app_flash_text_start__ = ORIGIN(KM55NS_IMG2_XIP);
__km55ns_app_flash_text_end__ = ORIGIN(KM55NS_IMG2_XIP) + LENGTH(KM55NS_IMG2_XIP);
/* SOLO isolation: the KM55NS SRAM span (IPC mailbox + img2 app/data) the non-secure
 * KM55NS core is allowed to access; consumed by solo_km55ns_sram[] in ameba_solocfg.c. */
__km55ns_sram_start__ = KM55NS_IMG2_RAM_START;
__km55ns_sram_end__ = SRAM_END;
#endif
__km4tz_flash_text_start__ = ORIGIN(KM55TZ_IMG2_XIP);
__km4tz_flash_text_end__ = ORIGIN(KM55TZ_IMG2_XIP) + LENGTH(KM55TZ_IMG2_XIP);
__km4tz_boot_text_start__ = ORIGIN(KM55TZ_BOOT_XIP);
__km4tz_boot_text_end__ = ORIGIN(KM55TZ_BOOT_XIP) + LENGTH(KM55TZ_BOOT_XIP);
__km4tz_img3_text_start__ = ORIGIN(KM55TZ_IMG3_XIP);
__km4tz_img3_text_end__ = ORIGIN(KM55TZ_IMG3_XIP) + LENGTH(KM55TZ_IMG3_XIP);
/* Per-mode start symbols for image3 sub-image headers (used by CMakeLists & SAU config) */
__image3_xip_start__   = ORIGIN(KM55TZ_IMG3_XIP);
__image3_sram_start__  = ORIGIN(KM55TZ_BD_RAM_TZ);
__image3_psram_start__ = ORIGIN(KM55TZ_BD_PSRAM_TZ);

/* Region 0: KM55TZ  ROM */
__rom_text_region0_start__ = ORIGIN(KM4TZ_IROM);                                      /* 0x00020000 */
__rom_text_region0_end__   = ORIGIN(KM4TZ_NS_ROM) + LENGTH(KM4TZ_NS_ROM);             /* 0x00038000 */
/* Region 1: KM55NS  ROM */
__rom_text_region1_start__ = ORIGIN(KM4NS_ROM);                                       /* 0x00020000 */
__rom_text_region1_end__   = ORIGIN(KM4NS_ROM) + LENGTH(KM4NS_ROM);                   /* 0x00024000 */

__global_bss_start__ = ORIGIN(GLOBAL_BSS_RAM);
__global_bss_end__ = ORIGIN(GLOBAL_BSS_RAM) + LENGTH(GLOBAL_BSS_RAM);;
__km4tz_pqc_rom_msp_top__ = ORIGIN(KM55TZ_FLOADER_RAM_S) - IMAGE_HEADER_LEN;
__km4tz_msp_ram_ns__ = ORIGIN(KM55TZ_MSP_RAM_NS) + LENGTH(KM55TZ_MSP_RAM_NS);
__km4tz_msp_ram_ns_limit__ = ORIGIN(KM55TZ_MSP_RAM_NS);

/* for ameba_boot_trustzonecfg.c */
#if defined (CONFIG_TRUSTZONE_EN)
__km4tz_ns_rom_start__ = ORIGIN(KM4TZ_NS_ROM);
__km4tz_ns_rom_end__ = ORIGIN(KM4TZ_NS_ROM) + LENGTH(KM4TZ_NS_ROM);
__km4tz_rombss_ram_s_start__ = ORIGIN(KM4TZ_ROMBSS_RAM_S);
__km4tz_rombss_ram_ns_start__ = ORIGIN(KM4TZ_ROMBSS_RAM_NS);
#endif

__km4tz_bootloader_ram_s_start__ = ORIGIN(KM55TZ_BOOTLOADER_RAM_S) - IMAGE_HEADER_LEN;
/*SRAM Layout boundary*/
__km4tz_bd_ram_start__ = ORIGIN(KM55TZ_BD_RAM) - IMAGE_HEADER_LEN;
__km4tz_bd_ram_end__ = ORIGIN(KM55TZ_BD_RAM) + LENGTH(KM55TZ_BD_RAM);
__km4ns_bd_ram_start__ = ORIGIN(KM55NS_BD_RAM) - IMAGE_HEADER_LEN;
__km4ns_bd_ram_end__ = ORIGIN(KM55NS_BD_RAM) + LENGTH(KM55NS_BD_RAM);
/* SOLO KM55TZ<->KM55NS shared SRAM block: 512B IPC table + 2K share region, one
 * contiguous 2.5K span at the KM55NS SRAM base (consumed by the isolation config). */
__solo_share_ram_start__ = ORIGIN(KM55NS_IPC_RAM);
__solo_share_ram_end__ = ORIGIN(KM55NS_SHARE_RAM) + LENGTH(KM55NS_SHARE_RAM);

/*PSRAM Layout boundary*/
__km4tz_bd_psram_start__ = ORIGIN(KM55TZ_BD_PSRAM) - IMAGE_HEADER_LEN;
__km4tz_bd_psram_end__ = ORIGIN(KM55TZ_BD_PSRAM) + LENGTH(KM55TZ_BD_PSRAM);
__km4ns_bd_psram_start__ = KM55NS_IMG2_PSRAM_START;
__km4ns_bd_psram_end__ = KM55NS_IMG2_PSRAM_START + KM55NS_PSRAM_IMG2_SIZE;
__non_secure_psram_start__ = KM55NS_IMG2_PSRAM_START;
__non_secure_psram_end__ = PSRAM_END;

/* Just for ROM check */
__romchk_sram_end_s__ = SRAM_END + SECURE_ADDR_OFFSET;
__romchk_msp_ram_lp__ = ORIGIN(KM55NS_MSP_RAM) + LENGTH(KM55NS_MSP_RAM);
__romchk_msp_ram_hp__ = ORIGIN(KM55TZ_MSP_RAM_S) + LENGTH(KM55TZ_MSP_RAM_S);
__romchk_bootloader_start__ = ORIGIN(KM55TZ_BOOTLOADER_RAM_S) - IMAGE_HEADER_LEN;
__romchk_bootloader_limit__ = ORIGIN(KM55TZ_BOOTLOADER_RAM_S) - IMAGE_HEADER_LEN + KBYTES(96);
__romchk_loader_download_addr__ = ORIGIN(KM55TZ_FLOADER_RAM_S) - IMAGE_HEADER_LEN;;
__romchk_fullmac_addr_start__ = ORIGIN(KM4NS_ROMBSS_RAM) + SECURE_ADDR_OFFSET;
__romchk_fullmac_addr_end__ = SRAM_END + SECURE_ADDR_OFFSET;
__romchk_xmodem_buffer_limit__ = ORIGIN(KM55NS_MSP_RAM) + LENGTH(KM55NS_MSP_RAM);
ASSERT(__romchk_sram_end_s__ == SRAM_END_ADDRESS_S, "Error: Linker address (__romchk_sram_end_s__) does not match C Macro (SRAM_END_ADDRESS_S)!");
ASSERT(__romchk_msp_ram_lp__ == MSP_RAM_LP, "Error: Linker address (__romchk_msp_ram_lp__) does not match C Macro (MSP_RAM_LP)!");
ASSERT(__romchk_msp_ram_hp__ == MSP_RAM_HP, "Error: Linker address (__romchk_msp_ram_hp__) does not match C Macro (MSP_RAM_HP)!");
ASSERT(__romchk_bootloader_start__ == BOOT_LOADER_START, "Error: Linker address (__romchk_bootloader_start__) does not match C Macro (BOOT_LOADER_START)!");
ASSERT(__romchk_bootloader_limit__ == BOOT_LOADER_LIMIT, "Error: Linker address (__romchk_bootloader_limit__) does not match C Macro (BOOT_LOADER_LIMIT)!");
ASSERT(__romchk_loader_download_addr__ == LOADER_DOWNLOAD_ADDR, "Error: Linker address (__romchk_loader_download_addr__) does not match C Macro (LOADER_DOWNLOAD_ADDR)!");
ASSERT(__romchk_fullmac_addr_start__ == FULLMAC_ADDR_START, "Error: Linker address (__romchk_fullmac_addr_start__) does not match C Macro (FULLMAC_ADDR_START)!");
ASSERT(__romchk_fullmac_addr_end__ == FULLMAC_ADDR_END, "Error: Linker address (__romchk_fullmac_addr_end__) does not match C Macro (FULLMAC_ADDR_END)!");

/* code analyse used */
__rtk_fixed_addr__ = SRAM_BASE + KM55TZ_KM55NS_RAM_SIZE;
__img1_sram_size__ = KM55TZ_IMG1_SIZE;
__img3_sram_size__ = KM55TZ_RAM_TZ_SIZE;
__misc_sram_size__ = KM55NS_IPC_RAM_SIZE;
