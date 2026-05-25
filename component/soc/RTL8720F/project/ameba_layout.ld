#include "project_km4tz/platform_autoconf.h"
#include "../../component/soc/RTL8720F/fwlib/include/hal_platform.h"

#undef KBYTES
#undef SRAM_BASE
#undef PSRAM_BASE

#define SECURE_ADDR_OFFSET           (0x10000000)
#define KBYTES(x)                    ((x) * 1024)
#define IMAGE_HEADER_LEN             (0x20)
#define KM4TZ_KM4NS_RAM_SIZE         KBYTES(8)
#define SRAM_BASE                    (0x20000000)
#define SRAM_END                     (0x20080000)
#define PSRAM_BASE                   (0x60000000)
#define PSRAM_END                    (0x60400000)

/* layout configuration */
#define TZ_NSC_SIZE                  (4)
#define TZ_ENTRY_SIZE                (16)
#if defined(CONFIG_TZ_S_SIZE)
#define TZ_S_SIZE                    (CONFIG_TZ_S_SIZE)
#else
#define TZ_S_SIZE                    (44)
#endif
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
#define KM4TZ_MSP_RAM_NS_SIZE        KBYTES(2)
#elif defined(CONFIG_IMG3_PSRAM)
#define RAM_TZ_NSC_SIZE              KBYTES(0)
#define RAM_TZ_ENTRY_SIZE            KBYTES(0)
#define RAM_KM4TZ_IMG3_SIZE          KBYTES(0)
#define PSRAM_TZ_NSC_SIZE            KBYTES(TZ_NSC_SIZE)
#define PSRAM_TZ_ENTRY_SIZE          KBYTES(TZ_ENTRY_SIZE)
#define PSRAM_KM4TZ_IMG3_SIZE        KBYTES(TZ_S_SIZE)
#define KM4TZ_MSP_RAM_NS_SIZE        KBYTES(2)
#else /* undefine trustzone */
#define RAM_TZ_NSC_SIZE              KBYTES(0)
#define RAM_TZ_ENTRY_SIZE            KBYTES(0)
#define RAM_KM4TZ_IMG3_SIZE          KBYTES(0)
#define PSRAM_TZ_NSC_SIZE            KBYTES(0)
#define PSRAM_TZ_ENTRY_SIZE          KBYTES(0)
#define PSRAM_KM4TZ_IMG3_SIZE        KBYTES(0)
#define KM4TZ_MSP_RAM_NS_SIZE        KBYTES(0)
#endif

#if defined(CONFIG_IMG1_FLASH)
#define KM4TZ_IMG1_SIZE              KBYTES(4)
#else
#define KM4TZ_IMG1_SIZE              KBYTES(24)
#endif

#define KM4NS_IPC_RAM_SIZE           512              /* 512B */

#if defined (CONFIG_SOLO)
#define RAM_KM4NS_IMG2_SIZE          KBYTES(184)
#define RAM_KM4NS_BOOT_SIZE          KBYTES(8)
#elif defined (CONFIG_WHC_DEV_FLASH_BOOT)
#define RAM_KM4NS_IMG2_SIZE          KBYTES(0)
#define RAM_KM4NS_BOOT_SIZE          KBYTES(0)
#else
#define RAM_KM4NS_IMG2_SIZE          KBYTES(192)       /* include 512B IPC RAM */
#define RAM_KM4NS_BOOT_SIZE          KBYTES(0)
#endif

/*
                  RTL8720F SRAM Layout (512K)
0x20000000 ──────────►┌──────────────────────┐
                      │     KM4TZ/KM4NS RAM  │
                      │       (12K)          │
                      │    <Realtek Fixed>   │
                      │                      │
fix addr              │                      │
0x20002000 ──────────►├──────────────────────┼──┐
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

/* BOOT(IMG1)/IMG3/IMG2 layout*/                                                            /* Trstzone On  (IMG1 On Flash) | Trustzone off (IMG1 On Flash)  */
#define KM4TZ_IMG1_RAM_SATRT          (SRAM_BASE + KM4TZ_KM4NS_RAM_SIZE)                    /* IMG1 On SRAM: 0x20002000 ~ 0x20008000 ; IMG1 On Flash: 0x20002000 ~ 0x20003000*/
#define KM4TZ_RAM_TZ_NSC_START        (KM4TZ_IMG1_RAM_SATRT + KM4TZ_IMG1_SIZE )             /* 0x20003000 ~ 0x20004000 | unavailable */
#define KM4TZ_RAM_TZ_ENTRY_START      (KM4TZ_RAM_TZ_NSC_START + RAM_TZ_NSC_SIZE )           /* 0x20004000 ~ 0x20008000 | unavailable */
#define KM4TZ_RAM_TZ_SECURE_START     (KM4TZ_RAM_TZ_ENTRY_START + RAM_TZ_ENTRY_SIZE )       /* 0x20008000 ~ 0x20013000 | unavailable */
#define KM4TZ_MSP_RAM_NS_START        (KM4TZ_RAM_TZ_SECURE_START + RAM_KM4TZ_IMG3_SIZE)     /* 0x20013000 ~ 0x20013800 | unavailable */
#define KM4TZ_IMG2_RAM_START          (KM4TZ_MSP_RAM_NS_START + KM4TZ_MSP_RAM_NS_SIZE)      /* 0x20013800 ~ 0x20064000 | 0x20003000 ~ 0x20064000 */
#define KM4NS_BOOT_RAM_START          (SRAM_END - RAM_KM4NS_BOOT_SIZE)                      /* 0x20064000 ~ 0x20064000 | 0x20064000 ~ 0x20064000 */
#define KM4NS_IMG2_RAM_START          (KM4NS_BOOT_RAM_START - RAM_KM4NS_IMG2_SIZE)          /* 0x20064000 ~ 0x20080000 | 0x20064000 ~ 0x20080000 */

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
    EMPTY_SECTION (rwx) :                     ORIGIN = 0x00000000, LENGTH = 0x00000000 - 0x00000000    /* This region shall be empty */

    /* KM4TZ(CPU0) ROM */
    KM4TZ_IROM (rx) :                         ORIGIN = 0x00020000, LENGTH = 0x00033C00 - 0x00020000    /* KM4TZ IROM_S: 79K */
    KM4TZ_DROM (rx) :                         ORIGIN = 0x00033C00, LENGTH = 0x00036B00 - 0x00033C00    /* KM4TZ DROM_S: 11.75k */
    KM4TZ_NS_ROM (rx) :                       ORIGIN = 0x00036B00, LENGTH = 0x00038000 - 0x00036B00    /* KM4TZ NS ROM: 5.25k */

    /* KM4NS ROM */
    KM4NS_ROM (rx) :                          ORIGIN = 0x00020000, LENGTH = 0x00024000 - 0x00020000    /* KM4NS KM4NS_ROM: 16k */

    /* KM4TZ Secure RAM, BIT(28)=1 */
    KM4TZ_ROMBSS_RAM_S (rw) :                 ORIGIN = 0x30000000, LENGTH = 0x30000400 - 0x30000000    /* KM4TZ ROM BSS RAM S: 1k */
    KM4TZ_MSP_RAM_S (rw) :                    ORIGIN = 0x30000400, LENGTH = 0x30001000 - 0x30000400    /* KM4TZ MSP_S RAM: 3k */

    /* KM4TZ Non-secure RAM */
    KM4TZ_ROMBSS_RAM_NS (rw) :                ORIGIN = 0x20001000, LENGTH = 0x20001380 - 0x20001000    /* KM4TZ ROM BSS NS RAM: 1k */
    GLOBAL_BSS_RAM (rw) :                     ORIGIN = 0x20001380, LENGTH = 0x20001400 - 0x20001380    /* IMAGE1/2 GLOBAL BSS RAM: 128B */

    /* KM4NS RAM */
    KM4NS_ROMBSS_RAM (rw) :                   ORIGIN = 0x20001400, LENGTH = 0x20001800 - 0x20001400    /* KM4NS ROM BSS RAM: 1k */
    KM4NS_MSP_RAM (rw) :                      ORIGIN = 0x20001800, LENGTH = 0x20002000 - 0x20001800    /* KM4NS MSP RAM: 2k */

    /* KM4TZ Secure RAM, BIT(28)=1 */
    KM4TZ_BOOTLOADER_RAM_S (rwx) :            ORIGIN = KM4TZ_IMG1_RAM_SATRT + SECURE_ADDR_OFFSET + IMAGE_HEADER_LEN, LENGTH = KM4TZ_IMG1_SIZE  - IMAGE_HEADER_LEN        /* KM4TZ BOOT Loader RAM: 4K or 32k */
    KM4TZ_FLOADER_RAM_S (rwx) :               ORIGIN = 0x30070020, LENGTH = 0x30080000 - 0x30070020    /* Floader / FULLMAC bootloader: 60k, 0x3007F000 ~ 0x30080000 for MANIFEST */

    /* FULLMAC */
    FULLMAC_IMG2 (rwx) :                      ORIGIN = 0x20001420, LENGTH = 0x20070000 - 0x20001420    /* FULLMAC IMAGE2. reuse 0x3007F000 ~ 0x30080000 store MANIFEST */
    FULLMAC_TCM_RAM (rwx) :                   ORIGIN = 0x000F0000, LENGTH = 0x00100000 - 0x000F0000    /* FULLMAC TCM RAM: TZ+NS=64k */

    /* IMG2 RAM */
#if defined (CONFIG_WHC_DEV_SDIO_BOOT)
    KM4TZ_BD_RAM (rwx) :                      ORIGIN = ORIGIN(FULLMAC_IMG2), LENGTH = LENGTH(FULLMAC_IMG2)
#else
    KM4TZ_BD_RAM_TZ_NSC (rwx) :               ORIGIN = KM4TZ_RAM_TZ_NSC_START + IMAGE_HEADER_LEN,     LENGTH = RAM_TZ_NSC_SIZE - IMAGE_HEADER_LEN                                   /* KM4TZ BD RAM NSC: 4k */
    KM4TZ_BD_RAM_TZ_ENTRY (rwx) :             ORIGIN = KM4TZ_RAM_TZ_ENTRY_START,  LENGTH = RAM_TZ_ENTRY_SIZE                                                                        /* KM4TZ BD RAM ENTRY: 16k */
    KM4TZ_BD_RAM_TZ_S (rwx) :                 ORIGIN = KM4TZ_RAM_TZ_SECURE_START + SECURE_ADDR_OFFSET  + IMAGE_HEADER_LEN, LENGTH = RAM_KM4TZ_IMG3_SIZE - IMAGE_HEADER_LEN          /* KM4TZ BD RAM S: 44k */
    KM4TZ_MSP_RAM_NS (rw) :                   ORIGIN = KM4TZ_MSP_RAM_NS_START, LENGTH = KM4TZ_MSP_RAM_NS_SIZE                                                                     /* KM4TZ MSP_NS RAM: 2k */
    KM4TZ_BD_RAM (rwx) :                      ORIGIN = KM4TZ_IMG2_RAM_START + IMAGE_HEADER_LEN, LENGTH = KM4NS_IMG2_RAM_START - KM4TZ_IMG2_RAM_START - IMAGE_HEADER_LEN           /* KM4TZ BD RAM: TZ On, 322KB; Tz off, 388K */
#endif /* CONFIG_TRUSTZONE_EN */

    KM4NS_IPC_RAM (rwx) :                     ORIGIN = KM4NS_IMG2_RAM_START, LENGTH = KM4NS_IPC_RAM_SIZE    /* KM4TZ IPC RAM: 512B */
    KM4NS_BD_RAM (rwx) :                      ORIGIN = KM4NS_IMG2_RAM_START + KM4NS_IPC_RAM_SIZE + IMAGE_HEADER_LEN,   LENGTH = RAM_KM4NS_IMG2_SIZE - KM4NS_IPC_RAM_SIZE - IMAGE_HEADER_LEN    /* KM4NS BD RAM: 112KB */
    KM4NS_BOOTLOADER_RAM (rwx) :              ORIGIN = KM4NS_BOOT_RAM_START + IMAGE_HEADER_LEN,   LENGTH = RAM_KM4NS_BOOT_SIZE - IMAGE_HEADER_LEN                                   /* KM4NS BOOT RAM */

    KM4TZ_BD_PSRAM_TZ_NSC (rwx) :             ORIGIN = KM4TZ_PSRAM_TZ_NSC_START + IMAGE_HEADER_LEN, LENGTH = PSRAM_TZ_NSC_SIZE - IMAGE_HEADER_LEN                                   /* KM4TZ BD PSRAM NSC: 4k */
    KM4TZ_BD_PSRAM_TZ_ENTRY (rwx) :           ORIGIN = KM4TZ_PSRAM_TZ_ENTRY_START, LENGTH = PSRAM_TZ_ENTRY_SIZE                                                                     /* KM4TZ BD PSRAM ENTRY: 16k */
    KM4TZ_BD_PSRAM_TZ_S (rwx) :               ORIGIN = KM4TZ_PSRAM_TZ_SECURE_START + SECURE_ADDR_OFFSET  + IMAGE_HEADER_LEN, LENGTH = PSRAM_KM4TZ_IMG3_SIZE - IMAGE_HEADER_LEN      /* KM4TZ BD PSRAM S:  128k */
    /* KM4TZ Non-secure PSRAM */
    KM4TZ_BD_PSRAM (rwx) :                    ORIGIN = KM4TZ_IMG2_PSRAM_START + IMAGE_HEADER_LEN, LENGTH = PSRAM_KM4TZ_IMG2_SIZE - IMAGE_HEADER_LEN                                 /* KM4TZ BD PSRAM : Tz off,3220K; TZ on, 3072K */
    KM4NS_BD_PSRAM (rwx) :                    ORIGIN = KM4NS_IMG2_PSRAM_START + IMAGE_HEADER_LEN, LENGTH = PSRAM_KM4NS_IMG2_SIZE - IMAGE_HEADER_LEN

    KM4TZ_PSRAM_HEAP_EXT (rwx) :              ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF    /* KM4TZ PSRAM HEKM4TZ EXT */
    KM4NS_PSRAM_HEAP_EXT (rwx) :              ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF    /* KM4NS PSRAM HEKM4TZ EXT */

    /* Retention RAM */
    RETENTION_RAM (rwx) :                     ORIGIN = 0x4080AA00, LENGTH = 0x4080AC00 - 0x4080AA00    /* KM4NS Retention SRAM: 512B */

    /* Flash */
    KM4NS_BOOT_XIP (rx) :                     ORIGIN = 0x00200020, LENGTH = 0x00400000 - 0x00200020    /* KM4NS IMG1 Virtual address: 2M */
    KM4TZ_BOOT_XIP (rx) :                     ORIGIN = 0x10400020, LENGTH = 0x10C00000 - 0x10400020    /* KM4TZ IMG1 Virtual address: 8M */
    KM4TZ_IMG3_XIP (rx) :                     ORIGIN = 0x10C00020, LENGTH = 0x11000000 - 0x10C00020    /* KM4TZ IMG3 Virtual address: 4M */
    KM4NS_IMG2_XIP (rx) :                     ORIGIN = 0x02000020, LENGTH = 0x04000000 - 0x02000020    /* KM4TZ IMG2 Virtual address: 32M */
    KM4TZ_IMG2_XIP (rx) :                     ORIGIN = 0x04000020, LENGTH = 0x06000000 - 0x04000020    /* KM4NS IMG2 Virtual address: 32M */
    BTRACE (rx) :                             ORIGIN = 0xC8800000, LENGTH = 0xC9000000 - 0xC8800000    /* BT LOG Virtual address: 8192k */
    CTRACE (rx) :                             ORIGIN = 0xCA000000, LENGTH = 0xCA7FFFFF - 0xCA000000    /* COEX LOG Virtual address: 8192k */
    STRACE (rx) :                             ORIGIN = 0xCB000000, LENGTH = 0xCB7FFFFF - 0xCB000000    /* SDN LOG Virtual address: 8192k */
}

__ipc_memory_start__ = ORIGIN(KM4NS_IPC_RAM);
#if defined (CONFIG_SOLO)
__km4ns_flash_text_start__ = ORIGIN(KM4NS_BOOT_XIP);
__km4ns_flash_text_end__ = ORIGIN(KM4NS_BOOT_XIP) + LENGTH(KM4NS_BOOT_XIP);
__km4ns_app_flash_text_start__ = ORIGIN(KM4NS_IMG2_XIP);
__km4ns_app_flash_text_end__ = ORIGIN(KM4NS_IMG2_XIP) + LENGTH(KM4NS_IMG2_XIP);
#else
__km4ns_flash_text_start__ = ORIGIN(KM4NS_IMG2_XIP);
__km4ns_flash_text_end__ = ORIGIN(KM4NS_IMG2_XIP) + LENGTH(KM4NS_IMG2_XIP);
#endif
__km4tz_flash_text_start__ = ORIGIN(KM4TZ_IMG2_XIP);
__km4tz_flash_text_end__ = ORIGIN(KM4TZ_IMG2_XIP) + LENGTH(KM4TZ_IMG2_XIP);
__km4tz_boot_text_start__ = ORIGIN(KM4TZ_BOOT_XIP);
__km4tz_boot_text_end__ = ORIGIN(KM4TZ_BOOT_XIP) + LENGTH(KM4TZ_BOOT_XIP);
__km4tz_img3_text_start__ = ORIGIN(KM4TZ_IMG3_XIP);
__km4tz_img3_text_end__ = ORIGIN(KM4TZ_IMG3_XIP) + LENGTH(KM4TZ_IMG3_XIP);

/* Region 0: KM4TZ  ROM */
__rom_text_region0_start__ = ORIGIN(KM4TZ_IROM);                                      /* 0x00020000 */
__rom_text_region0_end__   = ORIGIN(KM4TZ_NS_ROM) + LENGTH(KM4TZ_NS_ROM);             /* 0x00038000 */
/* Region 1: KM4NS  ROM */
__rom_text_region1_start__ = ORIGIN(KM4NS_ROM);                                       /* 0x00020000 */
__rom_text_region1_end__   = ORIGIN(KM4NS_ROM) + LENGTH(KM4NS_ROM);                   /* 0x00024000 */

__global_bss_start__ = ORIGIN(GLOBAL_BSS_RAM);
__global_bss_end__ = ORIGIN(GLOBAL_BSS_RAM) + LENGTH(GLOBAL_BSS_RAM);;
__km4tz_pqc_rom_msp_top__ = ORIGIN(KM4TZ_FLOADER_RAM_S) - IMAGE_HEADER_LEN;
__km4tz_msp_ram_ns__ = ORIGIN(KM4TZ_MSP_RAM_NS) + LENGTH(KM4TZ_MSP_RAM_NS);
__km4tz_msp_ram_ns_limit__ = ORIGIN(KM4TZ_MSP_RAM_NS);

/* for ameba_boot_trustzonecfg.c */
#if defined (CONFIG_TRUSTZONE_EN)

__km4tz_ns_rom_start__ = ORIGIN(KM4TZ_NS_ROM);
__km4tz_ns_rom_end__ = ORIGIN(KM4TZ_NS_ROM) + LENGTH(KM4TZ_NS_ROM);
__km4tz_rombss_ram_s_start__ = ORIGIN(KM4TZ_ROMBSS_RAM_S);
__km4tz_rombss_ram_ns_start__ = ORIGIN(KM4TZ_ROMBSS_RAM_NS);

__km4tz_tz_nsc_start__ = ORIGIN(KM4TZ_BD_RAM_TZ_NSC) - IMAGE_HEADER_LEN;
__km4tz_tz_nsc_end__ = ORIGIN(KM4TZ_BD_RAM_TZ_NSC) + LENGTH(KM4TZ_BD_RAM_TZ_NSC);
__km4tz_tz_entry_start__ = ORIGIN(KM4TZ_BD_RAM_TZ_ENTRY);
__km4tz_tz_entry_end__ = ORIGIN(KM4TZ_BD_RAM_TZ_ENTRY) + LENGTH(KM4TZ_BD_RAM_TZ_ENTRY);

__km4tz_psram_tz_nsc_start__ = ORIGIN(KM4TZ_BD_PSRAM_TZ_NSC) - IMAGE_HEADER_LEN;
__km4tz_psram_tz_nsc_end__ = ORIGIN(KM4TZ_BD_PSRAM_TZ_NSC) + LENGTH(KM4TZ_BD_PSRAM_TZ_NSC);
__km4tz_psram_tz_entry_start__ = ORIGIN(KM4TZ_BD_PSRAM_TZ_ENTRY);
__km4tz_psram_tz_entry_end__ = ORIGIN(KM4TZ_BD_PSRAM_TZ_ENTRY) + LENGTH(KM4TZ_BD_PSRAM_TZ_ENTRY);

#endif
__km4tz_bootloader_ram_s_start__ = ORIGIN(KM4TZ_BOOTLOADER_RAM_S) - IMAGE_HEADER_LEN;
/*SRAM Layout boundary*/
__km4tz_bd_ram_start__ = ORIGIN(KM4TZ_BD_RAM) - IMAGE_HEADER_LEN;
__km4tz_bd_ram_end__ = ORIGIN(KM4TZ_BD_RAM) + LENGTH(KM4TZ_BD_RAM);
__km4ns_bd_ram_start__ = ORIGIN(KM4NS_BD_RAM) - IMAGE_HEADER_LEN;
__km4ns_bd_ram_end__ = ORIGIN(KM4NS_BD_RAM) + LENGTH(KM4NS_BD_RAM);

/*PSRAM Layout boundary*/
__km4tz_bd_psram_start__ = ORIGIN(KM4TZ_BD_PSRAM) - IMAGE_HEADER_LEN;
__km4tz_bd_psram_end__ = ORIGIN(KM4TZ_BD_PSRAM) + LENGTH(KM4TZ_BD_PSRAM);
__km4ns_bd_psram_start__ = ORIGIN(KM4NS_BD_PSRAM) - IMAGE_HEADER_LEN;
__km4ns_bd_psram_end__ = ORIGIN(KM4NS_BD_PSRAM) + LENGTH(KM4NS_BD_PSRAM);
__non_secure_psram_end__ = PSRAM_END;

/* Just for ROM check */
__romchk_sram_end_s__ = SRAM_END + SECURE_ADDR_OFFSET;
__romchk_msp_ram_lp__ = ORIGIN(KM4NS_MSP_RAM) + LENGTH(KM4NS_MSP_RAM);
__romchk_msp_ram_hp__ = ORIGIN(KM4TZ_MSP_RAM_S) + LENGTH(KM4TZ_MSP_RAM_S);
__romchk_bootloader_start__ = ORIGIN(KM4TZ_BOOTLOADER_RAM_S) - IMAGE_HEADER_LEN;
__romchk_bootloader_limit__ = ORIGIN(KM4TZ_BOOTLOADER_RAM_S) - IMAGE_HEADER_LEN + KBYTES(96);
__romchk_loader_download_addr__ = ORIGIN(KM4TZ_FLOADER_RAM_S) - IMAGE_HEADER_LEN;;
__romchk_fullmac_addr_start__ = ORIGIN(KM4NS_ROMBSS_RAM) + SECURE_ADDR_OFFSET;
__romchk_fullmac_addr_end__ = SRAM_END + SECURE_ADDR_OFFSET;
__romchk_xmodem_buffer_limit__ = ORIGIN(KM4NS_MSP_RAM) + LENGTH(KM4NS_MSP_RAM);
ASSERT(__romchk_sram_end_s__ == SRAM_END_ADDRESS_S, "Error: Linker address (__romchk_sram_end_s__) does not match C Macro (SRAM_END_ADDRESS_S)!");
ASSERT(__romchk_msp_ram_lp__ == MSP_RAM_LP, "Error: Linker address (__romchk_msp_ram_lp__) does not match C Macro (MSP_RAM_LP)!");
ASSERT(__romchk_msp_ram_hp__ == MSP_RAM_HP, "Error: Linker address (__romchk_msp_ram_hp__) does not match C Macro (MSP_RAM_HP)!");
ASSERT(__romchk_bootloader_start__ == BOOT_LOADER_START, "Error: Linker address (__romchk_bootloader_start__) does not match C Macro (BOOT_LOADER_START)!");
ASSERT(__romchk_bootloader_limit__ == BOOT_LOADER_LIMIT, "Error: Linker address (__romchk_bootloader_limit__) does not match C Macro (BOOT_LOADER_LIMIT)!");
ASSERT(__romchk_loader_download_addr__ == LOADER_DOWNLOAD_ADDR, "Error: Linker address (__romchk_loader_download_addr__) does not match C Macro (LOADER_DOWNLOAD_ADDR)!");
ASSERT(__romchk_fullmac_addr_start__ == FULLMAC_ADDR_START, "Error: Linker address (__romchk_fullmac_addr_start__) does not match C Macro (FULLMAC_ADDR_START)!");
ASSERT(__romchk_fullmac_addr_end__ == FULLMAC_ADDR_END, "Error: Linker address (__romchk_fullmac_addr_end__) does not match C Macro (FULLMAC_ADDR_END)!");

/* code analyse used */
__rtk_fixed_addr__ = SRAM_BASE + KM4TZ_KM4NS_RAM_SIZE;
__img1_sram_size__ = KM4TZ_IMG1_SIZE;
__img3_sram_size__ = RAM_TZ_NSC_SIZE + RAM_TZ_ENTRY_SIZE + RAM_KM4TZ_IMG3_SIZE;
__misc_sram_size__ = KM4NS_IPC_RAM_SIZE;
