#include "project_km4tz/platform_autoconf.h"

#define SECURE_ADDR_OFFSET           (0x10000000)
#define KBYTES(x)                    ((x) * 1024)
#define IMAGE_HEADER_LEN             (0x20)
#define KM4TZ_KM4NS_RAM_SIZE         KBYTES(20)
#define SRAM_BASE                    (0x20000000)
#define SRAM_END                     (0x20080000)
#define PSRAM_BASE                   (0x60000000)
#define PSRAM_END                    (0x60400000)

/* layout configuration */
#define TZ_NSC_SIZE                  (4)
#define TZ_ENTRY_SIZE                (16)
#define TZ_S_SIZE                    (44)
#if defined(CONFIG_IMG3_FLASH)
#define TZ_S_BSS_SIZE                (16)
#endif

#if defined(CONFIG_IMG3_SRAM)
#define RAM_TZ_NSC_SIZE              KBYTES(TZ_NSC_SIZE)
#define RAM_TZ_ENTRY_SIZE            KBYTES(TZ_ENTRY_SIZE)
#define RAM_KM4TZ_IMG3_SIZE          KBYTES(TZ_S_SIZE)
#define PSRAM_TZ_NSC_SIZE            KBYTES(0)
#define PSRAM_TZ_ENTRY_SIZE          KBYTES(0)
#define PSRAM_KM4TZ_IMG3_SIZE        KBYTES(0)
#elif defined(CONFIG_IMG3_PSRAM)
#define RAM_TZ_NSC_SIZE              KBYTES(0)
#define RAM_TZ_ENTRY_SIZE            KBYTES(0)
#define RAM_KM4TZ_IMG3_SIZE          KBYTES(0)
#define PSRAM_TZ_NSC_SIZE            KBYTES(TZ_NSC_SIZE)
#define PSRAM_TZ_ENTRY_SIZE          KBYTES(TZ_ENTRY_SIZE)
#define PSRAM_KM4TZ_IMG3_SIZE        KBYTES(TZ_S_SIZE)
#elif defined(CONFIG_IMG3_FLASH)
#define RAM_TZ_NSC_SIZE              KBYTES(TZ_NSC_SIZE)
#define RAM_TZ_ENTRY_SIZE            KBYTES(TZ_ENTRY_SIZE)
#define RAM_KM4TZ_IMG3_SIZE          KBYTES(TZ_S_BSS_SIZE)
#define PSRAM_TZ_NSC_SIZE            KBYTES(0)
#define PSRAM_TZ_ENTRY_SIZE          KBYTES(0)
#define PSRAM_KM4TZ_IMG3_SIZE        KBYTES(0)
#else /* undefine trustzone */
#define RAM_TZ_NSC_SIZE              KBYTES(0)
#define RAM_TZ_ENTRY_SIZE            KBYTES(0)
#define RAM_KM4TZ_IMG3_SIZE          KBYTES(0)
#define PSRAM_TZ_NSC_SIZE            KBYTES(0)
#define PSRAM_TZ_ENTRY_SIZE          KBYTES(0)
#define PSRAM_KM4TZ_IMG3_SIZE        KBYTES(0)
#endif

#if defined(CONFIG_IMG1_FLASH)
#define KM4TZ_IMG1_SIZE              KBYTES(4)
#else
#define KM4TZ_IMG1_SIZE              KBYTES(32)
#endif

#define RAM_KM4NS_IMG2_SIZE          KBYTES(192)
/*
                  amebagreen2 SRAM Layout (512K)
0x20000000 ──────────►┌──────────────────────┐
                      │     KM4TZ/KM4NS RAM  │
                      │       (20K)          │
                      │    <Realtek Fixed>   │
                      │                      │
fix addr              │                      │
0x20005000 ──────────►├──────────────────────┼──┐
                      │KM4TZ Bootloader(IMG1)│  ├───────►  KM4TZ_IMG1_SIZE
                      │                      │  │
                   ┌──┼──────────────────────┼──┴─┐
                   │  │      Trustzone(IMG3) │    │
If Trustzone off,  │  │      TZ_NSC(4K)      │    │        If Trustzone on,
              ◄────┤  │     TZ_ENTRY(16K)    │    ├─────►  IMG3 = RAM_TZ_NSC_SIZE + RAM_TZ_ENTRY_SIZE + RAM_KM4TZ_IMG3_SIZE
IMG3 size is 0.    │  │   KM4TZ_IMG3(44K)    │    │
                   └──┼──────────────────────┼────┴─┐
                      │                      │      │
                      │                      │      │
                      │    KM4TZ IMG2 RAM    │      │
                      │                      │      ├───►  KM4TZ_IMG2_SIZE = KM4NS_IMG2_RAM_START - KM4TZ_IMG2_RAM_START
                      │                      │      │
                      │                      │      │
                      ├──────────────────────┼──────┴─┐
                      │                      │        │
                      │    KM4NS IMG2 RAM    │        │
                      │                      │        ├─►  RAM_KM4NS_IMG2_SIZE
                      │                      │        │
                      │                      │        │
                      │                      │        │
 0x20080000──────────►└──────────────────────┴────────┘
*/

/* BOOT(IMG1)/IMG3/IMG2 layout*/                                                            /* Trstzone On (IMG1 On Flash) | Trustzone off (IMG1 On Flash) */
#define KM4TZ_IMG1_RAM_SATRT          (SRAM_BASE + KM4TZ_KM4NS_RAM_SIZE)                    /* IMG1 On SRAM: 0x20005000 ~ 0x2000D000 ; IMG1 On Flash: 0x20005000 ~ 0x20006000*/
#define KM4TZ_RAM_TZ_NSC_START        (KM4TZ_IMG1_RAM_SATRT + KM4TZ_IMG1_SIZE )             /* 0x20006000 ~ 0x20007000 | unavailable */
#define KM4TZ_RAM_TZ_ENTRY_START      (KM4TZ_RAM_TZ_NSC_START + RAM_TZ_NSC_SIZE )           /* 0x20007000 ~ 0x2000B000 | unavailable */
#define KM4TZ_RAM_TZ_SECURE_START     (KM4TZ_RAM_TZ_ENTRY_START + RAM_TZ_ENTRY_SIZE )       /* 0x2000B000 ~ 0x20016000 | unavailable */
#define KM4TZ_IMG2_RAM_START          (KM4TZ_RAM_TZ_SECURE_START + RAM_KM4TZ_IMG3_SIZE)     /* 0x20016000 ~ 0x20050000 | 0x20006000 ~ 0x20050000*/
#define KM4NS_IMG2_RAM_START          (SRAM_END - RAM_KM4NS_IMG2_SIZE)                      /* 0x20050000 ~ 0x20080000 | 0x20050000 ~ 0x20080000*/

/* IMG3 On PSRAM */
#define KM4TZ_PSRAM_TZ_NSC_START      (PSRAM_BASE)                                          /* 0x60000000 ~ 0x60001000 | unavailable */
#define KM4TZ_PSRAM_TZ_ENTRY_START    (KM4TZ_PSRAM_TZ_NSC_START + PSRAM_TZ_NSC_SIZE )       /* 0x60001000 ~ 0x60005000 | unavailable */
#define KM4TZ_PSRAM_TZ_SECURE_START   (KM4TZ_PSRAM_TZ_ENTRY_START + PSRAM_TZ_ENTRY_SIZE)    /* 0x60005000 ~ 0x60025000 | unavailable */

#define PSRAM_KM4TZ_IMG2_SIZE         (KBYTES(3220) - (PSRAM_TZ_NSC_SIZE + PSRAM_TZ_ENTRY_SIZE + PSRAM_KM4TZ_IMG3_SIZE))
#define KM4TZ_IMG2_PSRAM_START        (KM4TZ_PSRAM_TZ_SECURE_START + PSRAM_KM4TZ_IMG3_SIZE)    /* 0x60025000 ~ 0x60325000 | 0x60000000 ~ 0x60325000*/
#define KM4NS_IMG2_PSRAM_START        (KM4TZ_IMG2_PSRAM_START + PSRAM_KM4TZ_IMG2_SIZE)         /* 0x60325000 ~ 0x60400000 | 0x60325000 ~ 0x60400000*/
#define PSRAM_KM4NS_IMG2_SIZE         (PSRAM_END - KM4NS_IMG2_PSRAM_START)

MEMORY
{
    EMPTY_SECTION (rwx) :                  ORIGIN = 0x00000000, LENGTH = 0x00000000 - 0x00000000    /* This region shall be empty */

    /* KM4TZ(CPU0) ROM */
    KM4TZ_IROM (rx) :                      ORIGIN = 0x00000000, LENGTH = 0x0001D000 - 0x00000000    /* KM4TZ IROM_S: 116K */
    KM4TZ_DROM (rx) :                      ORIGIN = 0x0001D000, LENGTH = 0x0001F000 - 0x0001D000    /* KM4TZ DROM_S: 8k */
    KM4TZ_ONLY_ROM (rx) :                  ORIGIN = 0x0001F000, LENGTH = 0x00020000 - 0x0001F000    /* KM4TZ_ONLY_ROM: 4k */

    /* SHARE ROM */
    KM4NS_SYS_ROM (rx) :                   ORIGIN = 0x00100000, LENGTH = 0x00101000 - 0x00100000    /* KM4NS NP_SYS_ROM: 4k */
    SHARE_IROM (rx) :                      ORIGIN = 0x00101000, LENGTH = 0x0010C000 - 0x00101000    /* SHARE IROM: 44k */
    SHARE_DROM (rx) :                      ORIGIN = 0x0010C000, LENGTH = 0x0010F000 - 0x0010C000    /* SHARE DROM: 12k */
    OS_ROM (rx) :                          ORIGIN = 0x0010F000, LENGTH = 0x00114000 - 0x0010F000    /* OS ROM: 20k */
    WIFI_ROM (rx) :                        ORIGIN = 0x00114000, LENGTH = 0x00147800 - 0x00114000    /* WIFI ROM: 206k */
    DELAY_ROM (rx) :                       ORIGIN = 0x00147800, LENGTH = 0x00148000 - 0x00147800    /* DELAY ROM: 2k */

    /* KM4TZ Secure RAM, BIT(28)=1 */
    KM4TZ_ROMBSS_RAM_S (rw) :              ORIGIN = 0x30000000, LENGTH = 0x30000600 - 0x30000000    /* KM4TZ ROM BSS RAM S: 1.5k */
    KM4TZ_MSP_RAM_S (rw) :                 ORIGIN = 0x30000600, LENGTH = 0x30001000 - 0x30000600    /* KM4TZ MSP_S RAM: 2.5k */

    /* KM4TZ Non-secure RAM */
    KM4TZ_ROMBSS_RAM_NS (rw) :             ORIGIN = 0x20001000, LENGTH = 0x20002400 - 0x20001000    /* KM4TZ ROM BSS NS RAM: 5k */
    KM4TZ_MSP_RAM_NS (rw) :                ORIGIN = 0x20002400, LENGTH = 0x20003000 - 0x20002400    /* KM4TZ MSP_NS RAM: 3k */

    /* KM4NS RAM */
    KM4NS_ROMBSS_MMU (rw) :                ORIGIN = 0x20001000, LENGTH = 0x20002400 - 0x20001000    /* Remap to AP_ROMBSS_RAM_NS */
    KM4NS_ROMBSS_RAM (rw) :                ORIGIN = 0x20003000, LENGTH = 0x20004400 - 0x20003000    /* KM4NS ROM BSS RAM: 5k */
    KM4NS_MSP_RAM (rw) :                   ORIGIN = 0x20004600, LENGTH = 0x20005000 - 0x20004600    /* KM4NS MSP RAM: 3k */

    /* KM4NS_IPC_RAM need to be delete */
    KM4NS_IPC_RAM (rwx) :                  ORIGIN = 0x20004400, LENGTH = 0x20004600 - 0x20004400    /* KM4TZ PMC RAM: 512B */

    /* FULLMAC */
    FULLMAC_IMG2 (rwx) :                   ORIGIN = 0x20003020, LENGTH = 0x2006F000 - 0x20003020    /* FULLMAC IMAGE2: 432k, 0x2006F000 ~ 0x20070000 for MANIFEST */
    FULLMAC_TCM_RAM (rwx) :                ORIGIN = 0x000F0000, LENGTH = 0x00100000 - 0x000F0000    /* FULLMAC TCM RAM: TZ+NS=64k */
    FULLMAC_PG_LOADER_RAM (rwx) :          ORIGIN = 0x30000040, LENGTH = 0x300004C0 - 0x30000040    /* FULLMAC PG Loader RAM: AP_ROMBSS_RAM_S, shall after _fih_cfi_ctr */

    /* KM4TZ Secure RAM, BIT(28)=1 */
    KM4TZ_BOOTLOADER_RAM_S (rwx) :         ORIGIN = KM4TZ_IMG1_RAM_SATRT + SECURE_ADDR_OFFSET + IMAGE_HEADER_LEN, LENGTH = KM4TZ_IMG1_SIZE  - IMAGE_HEADER_LEN        /* KM4TZ BOOT Loader RAM: 4K or 32k */
#ifndef CONFIG_FULLMAC_PG_LOADER
    KM4TZ_FLOADER_RAM_S (rwx) :            ORIGIN = 0x30070020, LENGTH = 0x3007F000 - 0x30070020    /* Floader / FULLMAC bootloader: 60k, 0x3007F000 ~ 0x30080000 for MANIFEST */
#else
    KM4TZ_FLOADER_RAM_S (rwx) :            ORIGIN = ORIGIN(FULLMAC_PG_LOADER_RAM), LENGTH = LENGTH(FULLMAC_PG_LOADER_RAM)
#endif

    /* IMG2 RAM */
#if (!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
    KM4TZ_BD_RAM (rwx) :                   ORIGIN = ORIGIN(FULLMAC_IMG2), LENGTH = LENGTH(FULLMAC_IMG2)
#else
    KM4TZ_BD_RAM_TZ_NSC (rwx) :            ORIGIN = KM4TZ_RAM_TZ_NSC_START + IMAGE_HEADER_LEN,     LENGTH = RAM_TZ_NSC_SIZE - IMAGE_HEADER_LEN                                   /* KM4TZ BD RAM NSC: 4k */
    KM4TZ_BD_RAM_TZ_ENTRY (rwx) :          ORIGIN = KM4TZ_RAM_TZ_ENTRY_START,  LENGTH = RAM_TZ_ENTRY_SIZE                                                                        /* KM4TZ BD RAM ENTRY: 16k */
    KM4TZ_BD_RAM_TZ_S (rwx) :              ORIGIN = KM4TZ_RAM_TZ_SECURE_START + SECURE_ADDR_OFFSET  + IMAGE_HEADER_LEN, LENGTH = RAM_KM4TZ_IMG3_SIZE - IMAGE_HEADER_LEN          /* KM4TZ BD RAM S: 44k */
    KM4TZ_BD_RAM (rwx) :                   ORIGIN = KM4TZ_IMG2_RAM_START + IMAGE_HEADER_LEN,   LENGTH = KM4NS_IMG2_RAM_START - KM4TZ_IMG2_RAM_START - IMAGE_HEADER_LEN           /* KM4TZ BD RAM: TZ On, 212KB; Tz off, 288K */
    KM4NS_BD_RAM (rwx) :                   ORIGIN = KM4NS_IMG2_RAM_START + IMAGE_HEADER_LEN,   LENGTH = RAM_KM4NS_IMG2_SIZE - IMAGE_HEADER_LEN                                   /* KM4NS BD RAM: 192KB */
#endif /* CONFIG_TRUSTZONE_EN */


    KM4TZ_BD_PSRAM_TZ_NSC (rwx) :          ORIGIN = KM4TZ_PSRAM_TZ_NSC_START + IMAGE_HEADER_LEN, LENGTH = PSRAM_TZ_NSC_SIZE - IMAGE_HEADER_LEN                                   /* KM4TZ BD PSRAM NSC: 4k */
    KM4TZ_BD_PSRAM_TZ_ENTRY (rwx) :        ORIGIN = KM4TZ_PSRAM_TZ_ENTRY_START, LENGTH = PSRAM_TZ_ENTRY_SIZE                                                                     /* KM4TZ BD PSRAM ENTRY: 16k */
    KM4TZ_BD_PSRAM_TZ_S (rwx) :            ORIGIN = KM4TZ_PSRAM_TZ_SECURE_START + SECURE_ADDR_OFFSET  + IMAGE_HEADER_LEN, LENGTH = PSRAM_KM4TZ_IMG3_SIZE - IMAGE_HEADER_LEN      /* KM4TZ BD PSRAM S:  128k */
    /* KM4TZ Non-secure PSRAM */
    KM4TZ_BD_PSRAM (rwx) :                 ORIGIN = KM4TZ_IMG2_PSRAM_START + IMAGE_HEADER_LEN, LENGTH = PSRAM_KM4TZ_IMG2_SIZE - IMAGE_HEADER_LEN                                 /* KM4TZ BD PSRAM : Tz off,3220K; TZ on, 3072K */
    KM4NS_BD_PSRAM (rwx) :                 ORIGIN = KM4NS_IMG2_PSRAM_START + IMAGE_HEADER_LEN, LENGTH = PSRAM_KM4NS_IMG2_SIZE - IMAGE_HEADER_LEN                                 /* KM4NS BD PSRAM: 876K */

    KM4TZ_PSRAM_HEAP_EXT (rwx) :           ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF    /* KM4TZ PSRAM HEAP EXT */
    KM4NS_PSRAM_HEAP_EXT (rwx) :           ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF    /* KM4NS PSRAM HEAP EXT */

    /* Retention RAM */
    RETENTION_RAM (rwx) :                  ORIGIN = 0x4080AA00, LENGTH = 0x4080AC00 - 0x4080AA00    /* KM4NS Retention SRAM: 512B */

    /* Flash */
    KM4TZ_BOOT_XIP (rx) :                  ORIGIN = 0x10400020, LENGTH = 0x10C00000 - 0x10400020    /* KM4TZ IMG1 Virtual address: 8M */
    KM4TZ_IMG3_XIP (rx) :                  ORIGIN = 0x10C00020, LENGTH = 0x11000000 - 0x10C00020    /* KM4TZ IMG3 Virtual address: 4M */
    KM4NS_IMG2_XIP (rx) :                  ORIGIN = 0x02000020, LENGTH = 0x04000000 - 0x02000020    /* KM4TZ IMG2 Virtual address: 32M */
    KM4TZ_IMG2_XIP (rx) :                  ORIGIN = 0x04000020, LENGTH = 0x06000000 - 0x04000020    /* KM4NS IMG2 Virtual address: 32M */
    BTRACE (rx) :                          ORIGIN = 0xC8800000, LENGTH = 0xC9000000 - 0xC8800000    /* BT LOG Virtual address: 8192k */
    CTRACE (rx) :                          ORIGIN = 0xCA000000, LENGTH = 0xCA7FFFFF - 0xCA000000    /* COEX LOG Virtual address: 8192k */
}

__ipc_memory_start__ = ORIGIN(KM4NS_IPC_RAM);
__km4ns_flash_text_start__ = ORIGIN(KM4NS_IMG2_XIP);
__km4ns_flash_text_end__ = ORIGIN(KM4NS_IMG2_XIP) + LENGTH(KM4NS_IMG2_XIP);
__km4tz_flash_text_start__ = ORIGIN(KM4TZ_IMG2_XIP);
__km4tz_flash_text_end__ = ORIGIN(KM4TZ_IMG2_XIP) + LENGTH(KM4TZ_IMG2_XIP);
__km4tz_boot_text_start__ = ORIGIN(KM4TZ_BOOT_XIP);
__km4tz_boot_text_end__ = ORIGIN(KM4TZ_BOOT_XIP) + LENGTH(KM4TZ_BOOT_XIP);
__km4tz_img3_text_start__ = ORIGIN(KM4TZ_IMG3_XIP);
__km4tz_img3_text_end__ = ORIGIN(KM4TZ_IMG3_XIP) + LENGTH(KM4TZ_IMG3_XIP);


#if defined (CONFIG_TRUSTZONE_EN)
__km4tz_tz_nsc_start__ = ORIGIN(KM4TZ_BD_RAM_TZ_NSC) - 0x20;
__km4tz_tz_entry_start__ = ORIGIN(KM4TZ_BD_RAM_TZ_ENTRY);
__km4tz_tz_entry_end__ = ORIGIN(KM4TZ_BD_RAM_TZ_ENTRY) + LENGTH(KM4TZ_BD_RAM_TZ_ENTRY);

__km4tz_psram_tz_nsc_start__ = ORIGIN(KM4TZ_BD_PSRAM_TZ_NSC) - 0x20;
__km4tz_psram_tz_entry_start__ = ORIGIN(KM4TZ_BD_PSRAM_TZ_ENTRY) ;
__km4tz_psram_tz_entry_end__ = ORIGIN(KM4TZ_BD_PSRAM_TZ_ENTRY) + LENGTH(KM4TZ_BD_PSRAM_TZ_ENTRY);
#endif
/*SRAM Layout boundary*/
__km4tz_bd_ram_start__ = ORIGIN(KM4TZ_BD_RAM) - 0x20;

/*PSRAM Layout boundary*/
__km4tz_bd_psram_start__ = ORIGIN(KM4TZ_BD_PSRAM) - 0x20;
__non_secure_psram_end__ = PSRAM_END;
