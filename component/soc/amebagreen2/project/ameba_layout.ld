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
/* Minimum SRAM-TZ reserved even when the secure image runs from PSRAM, so the
 * SRAM-resident sub-image (bin B: .data/.bss and .sram.only low-power code that
 * must run after flash/psram are powered down) always has a home. */
#define KM4TZ_RAM_TZ_MIN_SIZE        KBYTES(4)
#if defined(CONFIG_IMG3_SRAM) || defined(CONFIG_IMG3_FLASH)
#define KM4TZ_RAM_TZ_SIZE            KBYTES(CONFIG_TZ_S_SIZE)
#define KM4TZ_PSRAM_TZ_SIZE          KBYTES(0)
#elif defined(CONFIG_IMG3_PSRAM)
#define KM4TZ_RAM_TZ_SIZE            KM4TZ_RAM_TZ_MIN_SIZE
#define KM4TZ_PSRAM_TZ_SIZE          KBYTES(CONFIG_TZ_S_SIZE)
#else /* undefine trustzone */
#define KM4TZ_RAM_TZ_SIZE            KBYTES(0)
#define KM4TZ_PSRAM_TZ_SIZE          KBYTES(0)
#endif

#if defined(CONFIG_IMG1_FLASH)
#define KM4TZ_IMG1_SIZE              KBYTES(4)
#else
#define KM4TZ_IMG1_SIZE              KBYTES(32)
#endif

#define RAM_KM4NS_IMG2_SIZE          KBYTES(96)
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
If Trustzone off,  │  │                      │    │        If Trustzone on,
              ◄────┤  │      RAM_TZ          │    ├─────►  IMG3 = KM4TZ_RAM_TZ_SIZE = KBYTES(CONFIG_TZ_S_SIZE)
IMG3 size is 0.    │  │                      │    │
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
#define KM4TZ_RAM_TZ_START            (KM4TZ_IMG1_RAM_SATRT + KM4TZ_IMG1_SIZE )             /* 0x20006000 ~ 0x20007000 | unavailable */
#define KM4TZ_IMG2_RAM_START          (KM4TZ_RAM_TZ_START + KM4TZ_RAM_TZ_SIZE)              /* 0x20016000 ~ 0x20068000 | 0x20006000 ~ 0x20068000*/
#define KM4NS_IMG2_RAM_START          (SRAM_END - RAM_KM4NS_IMG2_SIZE)                      /* 0x20068000 ~ 0x20080000 | 0x20068000 ~ 0x20080000*/

/* IMG3 On PSRAM */
#define KM4TZ_PSRAM_TZ_START          (PSRAM_BASE)                                          /* 0x60000000 ~ 0x60001000 | unavailable */

#define PSRAM_KM4TZ_IMG2_SIZE         (KBYTES(3220) - KM4TZ_PSRAM_TZ_SIZE)
#define KM4TZ_IMG2_PSRAM_START        (KM4TZ_PSRAM_TZ_START + KM4TZ_PSRAM_TZ_SIZE)             /* 0x60025000 ~ 0x60325000 | 0x60000000 ~ 0x60325000*/
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
    FULLMAC_IMG2 (rwx) :                   ORIGIN = 0x20003020, LENGTH = 0x20070000 - 0x20003020    /* FULLMAC IMAGE2: 436k, reuse 0x3007F000 ~ 0x30080000 store MANIFEST */
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
    KM4TZ_BD_RAM_TZ (rwx) :                ORIGIN = KM4TZ_RAM_TZ_START + IMAGE_HEADER_LEN,     LENGTH = KM4TZ_RAM_TZ_SIZE - IMAGE_HEADER_LEN                                   /* KM4TZ BD RAM TZ: ENTRY + SECURE + NSC */
    KM4TZ_BD_RAM (rwx) :                   ORIGIN = KM4TZ_IMG2_RAM_START + IMAGE_HEADER_LEN,   LENGTH = KM4NS_IMG2_RAM_START - KM4TZ_IMG2_RAM_START - IMAGE_HEADER_LEN           /* KM4TZ BD RAM: TZ On, 308KB; Tz off, 384K */
    KM4NS_BD_RAM (rwx) :                   ORIGIN = KM4NS_IMG2_RAM_START + IMAGE_HEADER_LEN,   LENGTH = RAM_KM4NS_IMG2_SIZE - IMAGE_HEADER_LEN                                   /* KM4NS BD RAM: 96KB */
#endif /* CONFIG_TRUSTZONE_EN */


    KM4TZ_BD_PSRAM_TZ (rwx) :              ORIGIN = KM4TZ_PSRAM_TZ_START + IMAGE_HEADER_LEN,   LENGTH = KM4TZ_PSRAM_TZ_SIZE - IMAGE_HEADER_LEN                                   /* KM4TZ BD PSRAM TZ: 64k */
    /* KM4TZ Non-secure PSRAM */
    KM4TZ_BD_PSRAM (rwx) :                 ORIGIN = KM4TZ_IMG2_PSRAM_START + IMAGE_HEADER_LEN, LENGTH = PSRAM_KM4TZ_IMG2_SIZE - IMAGE_HEADER_LEN                                 /* KM4TZ BD PSRAM : Tz off,3220K; TZ on, 3072K */
    KM4NS_BD_PSRAM (rwx) :                 ORIGIN = KM4NS_IMG2_PSRAM_START + IMAGE_HEADER_LEN, LENGTH = PSRAM_KM4NS_IMG2_SIZE - IMAGE_HEADER_LEN                                 /* KM4NS BD PSRAM: 876K */

    KM4TZ_PSRAM_HEAP_EXT (rwx) :           ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF    /* KM4TZ PSRAM HEAP EXT */
    KM4NS_PSRAM_HEAP_EXT (rwx) :           ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF    /* KM4NS PSRAM HEAP EXT */

    /* Retention RAM */
    RETENTION_RAM (rwx) :                  ORIGIN = 0x4080AA00, LENGTH = 0x4080AC00 - 0x4080AA00    /* KM4NS Retention SRAM: 512B */

    /* Flash */
    KM4TZ_BOOT_XIP (rx) :                  ORIGIN = 0x10400020, LENGTH = 0x10C00000 - 0x10400020    /* KM4TZ IMG1 Virtual address: 8M */
    KM4TZ_IMG3_XIP (rx) :                  ORIGIN = 0x00C00020, LENGTH = 0x01000000 - 0x00C00020    /* KM4TZ IMG3 Virtual address: 4M */
    KM4NS_IMG2_XIP (rx) :                  ORIGIN = 0x02000020, LENGTH = 0x04000000 - 0x02000020    /* KM4TZ IMG2 Virtual address: 32M */
    KM4TZ_IMG2_XIP (rx) :                  ORIGIN = 0x04000020, LENGTH = 0x06000000 - 0x04000020    /* KM4NS IMG2 Virtual address: 32M */
    BTRACE (rx) :                          ORIGIN = 0xC8800000, LENGTH = 0xC9000000 - 0xC8800000    /* BT LOG Virtual address: 8192k */
    CTRACE (rx) :                          ORIGIN = 0xCA000000, LENGTH = 0xCA7FFFFF - 0xCA000000    /* COEX LOG Virtual address: 8192k */

    /* dummy header for xip part */
    KM4TZ_BD_RAM_MP (rwx) :                ORIGIN = 0x2000D000 + IMAGE_HEADER_LEN,   LENGTH = 0x20034F80 - 0x2000D040
    KM4TZ_BD_RAM_MP_END(rwx) :             ORIGIN = 0x20034F80 - 32, LENGTH = 32
    KM4NS_BD_RAM_MP (rwx) :                ORIGIN = 0x20035000 + IMAGE_HEADER_LEN + IMAGE_HEADER_LEN,   LENGTH = 0x2007FFA0 - 0x2003F040

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

/*ROM code boundary for backtrace: two non-contiguous regions*/
__rom_text_region0_start__  = ORIGIN(KM4TZ_IROM);                              /* 0x00000000 */
__rom_text_region0_end__    = ORIGIN(KM4TZ_ONLY_ROM) + LENGTH(KM4TZ_ONLY_ROM); /* 0x00020000 */
__rom_text_region1_start__  = ORIGIN(KM4NS_SYS_ROM);                           /* 0x00100000 */
__rom_text_region1_end__    = ORIGIN(DELAY_ROM) + LENGTH(DELAY_ROM);           /* 0x00148000 */

/*SRAM Layout boundary*/
__km4tz_bd_ram_start__ = ORIGIN(KM4TZ_BD_RAM) - IMAGE_HEADER_LEN;

__km4tz_msp_ram_ns_limit__ = ORIGIN(KM4TZ_MSP_RAM_NS);
__km4tz_msp_ram_ns__ = ORIGIN(KM4TZ_MSP_RAM_NS) + LENGTH(KM4TZ_MSP_RAM_NS);

__km4ns_msp_ram_limit__ = ORIGIN(KM4NS_MSP_RAM);
__km4ns_msp_ram__ = ORIGIN(KM4NS_MSP_RAM) + LENGTH(KM4NS_MSP_RAM);

/*PSRAM Layout boundary*/
__km4tz_bd_psram_start__ = ORIGIN(KM4TZ_BD_PSRAM) - IMAGE_HEADER_LEN;
__non_secure_psram_end__ = PSRAM_END;

/* here for km4tz entry addr for floader */
__km4tz_bd_ram_mp_start__ = ORIGIN(KM4TZ_BD_RAM_MP) - IMAGE_HEADER_LEN - 1024 + IMAGE_HEADER_LEN;
__km4ns_bd_ram_mp_start__ = ORIGIN(KM4NS_BD_RAM_MP);

/* code analyse used */
__rtk_fixed_addr__ = SRAM_BASE + KM4TZ_KM4NS_RAM_SIZE;
__img1_sram_size__ = KM4TZ_IMG1_SIZE;
__img3_sram_size__ = KM4TZ_RAM_TZ_SIZE;
