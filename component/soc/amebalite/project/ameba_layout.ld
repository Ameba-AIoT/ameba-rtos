/* auto gen, please change AmebaLite_memory_layout.xlsx and exec gen_memory_layout.py */
#include "project_km4/platform_autoconf.h"
/* constant macro */
#define SECURE_ADDR_OFFSET 			(0x10000000)
#define KBYTES(x)  					((x) * 1024)
#define IMAGE_HEADER_LEN			(0x20)
#define KR4_KM4_RAM_SIZE			KBYTES(40)
#define SRAM_BASE					(0x20000000)
#define SRAM_END					(0x20080000)
#define PSRAM_BASE					(0x60000000)
/* PSRAM_END is only the address-window ceiling (largest internal PSRAM = 16MB). The real capacity comes from BD_num */
#define PSRAM_END					(0x61000000)

/* layout configuration */
/* TZ region sized by CONFIG_TZ_S_SIZE, default 64K. */
#if defined(CONFIG_IMG3_SRAM)
#define KM4_RAM_TZ_SIZE				KBYTES(CONFIG_TZ_S_SIZE)
#define KM4_PSRAM_TZ_SIZE			KBYTES(0)
#elif defined(CONFIG_IMG3_PSRAM)
#define KM4_RAM_TZ_SIZE				KBYTES(0)
#define KM4_PSRAM_TZ_SIZE			KBYTES(CONFIG_TZ_S_SIZE)
#else /* undefine trustzone */
#define KM4_RAM_TZ_SIZE				KBYTES(0)
#define KM4_PSRAM_TZ_SIZE			KBYTES(0)
#endif

/* IMG1 Size */
#if defined(CONFIG_IMG1_FLASH)
#define KM4_IMG1_SIZE 				KBYTES(4)
#else
#define KM4_IMG1_SIZE 				KBYTES(28)
#endif

/*
                    AmebaLite SRAM Layout (512KB: 0x20000000 ~ 0x20080000)
  Tags: (cfg)=Kconfig-sized   (fixed)=hardcoded constant   (elastic)=absorbs remainder

  Common prefix (all modes):
  0x20000000  +----------------------------+
              | KR4/KM4 RAM       (fixed)  |  KR4_KM4_RAM_SIZE = KBYTES(40); RealTek fixed region
              |                            |  (ROM BSS / MSP / IPC / PMC / KR4 boot-entry backup)
  0x2000A000  +----------------------------+  KM4_IMG1_RAM_SATRT
              | KM4 Bootloader(IMG1)(cfg)  |  KM4_IMG1_SIZE = 4K (CONFIG_IMG1_FLASH) / 28K (CONFIG_IMG1_SRAM)
              +----------------------------+  KM4_RAM_TZ_START
              | KM4 TrustZone (IMG3)(cfg)  |  KM4_RAM_TZ_SIZE = KBYTES(CONFIG_TZ_S_SIZE) if IMG3_SRAM, else 0
              +----------------------------+  KM4_IMG2_RAM_START
              | IMG2 region                |  one of the three variants below
  0x20080000  +----------------------------+  SRAM_END

  Variant A -- DSP in SRAM  (CONFIG_DSP_EN && CONFIG_DSP_RUN_IN_SRAM; no-PSRAM SoC)
  KM4_IMG2_RAM_START -> +------------------------+
                        | KM4 IMG2 RAM    (cfg)  |  KBYTES(CONFIG_SRAM_KM4_IMG2_KB)
                        +------------------------+  KR4_IMG2_RAM_START
                        | KR4 IMG2 RAM    (cfg)  |  KBYTES(CONFIG_SRAM_KR4_IMG2_KB)
  DSP_BD_SRAM_START --> +------------------------+  (KR4_BOOT_RAM 0x20040020 may be overlapped by DSP)
                        | DSP BD SRAM  (elastic) |  fills to SRAM_END (link-time SRAM_END-start, no runtime clamp)
  0x20080000            +------------------------+  SRAM_END

  Variant B1 -- DSP off / DSP in PSRAM, WHC_HOST || WHC_NONE  (DSP_BD_SRAM collapses to 0)
  KM4_IMG2_RAM_START -> +------------------------+
                        | KM4 IMG2 RAM  (elastic)|  KR4_IMG2_RAM_START - KM4_IMG2_RAM_START
  SRAM_END - 96K -----> +------------------------+  KR4_IMG2_RAM_START (pinned at top)
                        | KR4 IMG2 RAM    (fixed)|  RAM_KR4_IMG2_SIZE = KBYTES(96), hardcoded (not Kconfig)
  0x20080000            +------------------------+  SRAM_END

  Variant B2 -- DSP off / DSP in PSRAM, WHC_DEV  (DSP_BD_SRAM collapses to 0)
  KM4_IMG2_RAM_START -> +------------------------+
                        | KM4 IMG2 RAM    (fixed)|  RAM_KM4_IMG2_SIZE = KBYTES(96), hardcoded (not Kconfig)
                        +------------------------+  KR4_IMG2_RAM_START
                        | KR4 IMG2 RAM  (elastic)|  SRAM_END - KR4_IMG2_RAM_START
  0x20080000            +------------------------+  SRAM_END

  KR4 ROM anchors (address-pinned in KR4 ROM; NOT drawn as boxes above because they fall inside whichever variant segment spans them):
    0x20004C20  KR4_BOOT_RAM_BACKUP (64B) -- inside the 40K fixed region; backup of KR4 BootEntry.bin (32B header + 32B entry)
    0x20040020  KR4_BOOT_RAM        (32B) -- KR4 boot-entry, fixed in KR4 ROM. Old layout put KR4 IMG2 exactly here; now it may land in
                                             KM4/KR4 IMG2 or DSP depending on variant/sizes. Safe to overlap ("can be ignored when DSP uses it").
 */

/* BOOT(IMG1)/IMG3/IMG2 layout*/													/* Trustzone on			   | Trustzone off	*/
#define KM4_IMG1_RAM_SATRT   		(SRAM_BASE + KR4_KM4_RAM_SIZE)					/* IMG1 On SRAM: 0x2000A000 ~ 0x20011400 | IMG1 On FLASH: 0x2000A000 ~ 0x2000D000*/
#define KM4_RAM_TZ_START			(KM4_IMG1_RAM_SATRT + KM4_IMG1_SIZE )			/* TZ region start: 0x20011400 ~ 0x20021400 | unavailable */

/* IMG2 Size*/
#if defined(CONFIG_DSP_EN) && defined(CONFIG_DSP_RUN_IN_SRAM)
/* DSP-in-SRAM (no-PSRAM SoC): TZ -> KM4(fixed) -> KR4(fixed) -> DSP(tail).
 * KM4/KR4 take fixed Kconfig budgets; DSP fills the SRAM remainder to SRAM_END.
 * This branch is independent of WHC role (the WHC layout below is untouched). */
#define KM4_IMG2_RAM_START			(KM4_RAM_TZ_START + KM4_RAM_TZ_SIZE)
#define RAM_KM4_IMG2_SIZE			KBYTES(CONFIG_SRAM_KM4_IMG2_KB)
#define KR4_IMG2_RAM_START			(KM4_IMG2_RAM_START + RAM_KM4_IMG2_SIZE)
#define RAM_KR4_IMG2_SIZE			KBYTES(CONFIG_SRAM_KR4_IMG2_KB)
#define DSP_BD_SRAM_START			(KR4_IMG2_RAM_START + RAM_KR4_IMG2_SIZE)
#define DSP_BD_SRAM_ORIGIN			(DSP_BD_SRAM_START + IMAGE_HEADER_LEN)
#define DSP_BD_SRAM_LENGTH			(SRAM_END - DSP_BD_SRAM_START - IMAGE_HEADER_LEN)
ASSERT(DSP_BD_SRAM_START + IMAGE_HEADER_LEN <= SRAM_END, "SRAM over-committed: 40K+IMG1+TZ+KM4+KR4 leaves no room for DSP; shrink CONFIG_SRAM_K*_IMG2_KB.")
#else
#if defined(CONFIG_WHC_HOST) || defined(CONFIG_WHC_NONE)
#define RAM_KR4_IMG2_SIZE			KBYTES(96)
#define KM4_IMG2_RAM_START			(KM4_RAM_TZ_START + KM4_RAM_TZ_SIZE)			/* 0x20021400 ~ 0x20068000 | 0x2000D000 ~ 0x20068000*/
#define KR4_IMG2_RAM_START			(SRAM_END - RAM_KR4_IMG2_SIZE) 					/* 0x20068000 ~ 0x20078000 | 0x20068000 ~ 0x20078000*/
#define RAM_KM4_IMG2_SIZE			(KR4_IMG2_RAM_START - KM4_IMG2_RAM_START)
ASSERT(KM4_IMG2_RAM_START + IMAGE_HEADER_LEN <= KR4_IMG2_RAM_START, "SRAM over-committed: 40K+IMG1+TZ leaves no room for KM4 IMG2 (KR4 fixed 96K pinned at top).")
#else
#define RAM_KM4_IMG2_SIZE			KBYTES(96)
#define KM4_IMG2_RAM_START			(KM4_RAM_TZ_START + KM4_RAM_TZ_SIZE)			/* 0x20021400 ~ 0x20039400 | 0x2000D000 ~ 0x20025000*/
#define KR4_IMG2_RAM_START			(KM4_IMG2_RAM_START + RAM_KM4_IMG2_SIZE) 		/* 0x20039400 ~ 0x20078000 | 0x20025000 ~ 0x20078000*/
#define RAM_KR4_IMG2_SIZE			(SRAM_END - KR4_IMG2_RAM_START)
ASSERT(KR4_IMG2_RAM_START + IMAGE_HEADER_LEN <= SRAM_END, "SRAM over-committed: 40K+IMG1+TZ+KM4(fixed 96K) leaves no room for KR4 IMG2.")
#endif
/* DSP not in SRAM (DSP off or DSP in PSRAM): DSP_BD_SRAM collapses to 0; SRAM
 * keeps the existing WHC layout above unchanged. */
#define DSP_BD_SRAM_START			(SRAM_END)
#define DSP_BD_SRAM_ORIGIN			(SRAM_END)
#define DSP_BD_SRAM_LENGTH			(0)
#endif

/*
                 AmebaLite PSRAM Layout (window 16MB: 0x60000000 ~ 0x61000000)
  Tags: (cfg)=Kconfig-sized   (elastic)=absorbs remainder.

  Variant A -- DSP in PSRAM  (CONFIG_DSP_EN && !CONFIG_DSP_RUN_IN_SRAM)
  PSRAM_BASE 0x60000000  +------------------------+
                         | KM4 BD PSRAM TZ  (cfg) |  KM4_PSRAM_TZ_SIZE = KBYTES(CONFIG_TZ_S_SIZE) if IMG3_PSRAM, else 0
  __non_secure_       -> +------------------------+  KR4_IMG2_PSRAM_START (MPC entry0 non-secure base)
  psram_start__          | KR4 BD PSRAM     (cfg) |  KBYTES(CONFIG_PSRAM_KR4_IMG2_KB)
  __km4_bd_           -> +------------------------+  KM4_IMG2_PSRAM_START
  psram_start__          | KM4 BD PSRAM     (cfg) |  KBYTES(CONFIG_PSRAM_KM4_IMG2_KB) - TZ (KM4 absorbs TZ)
  __dsp_psram_start__ -> +------------------------+  PSRAM_DSP_START (0x60300000 at defaults, TZ-invariant)
    (e.g. 0x60300000)    | DSP BD PSRAM  (elastic)|  fills to PSRAM_END; DSP-side heap top clamped to real capacity via RRAM DSP_PSRAM_HEAP_TOP
  PSRAM_END  0x61000000  +------------------------+  window ceiling

  Variant B -- DSP off / DSP in SRAM  (DSP_BD_PSRAM collapses to 0)
  PSRAM_BASE 0x60000000  +------------------------+
                         | KM4 BD PSRAM TZ  (cfg) |  KBYTES(CONFIG_TZ_S_SIZE) if IMG3_PSRAM, else 0
  __non_secure_       -> +------------------------+  KR4_IMG2_PSRAM_START
  psram_start__          | KR4 BD PSRAM     (cfg) |  KBYTES(CONFIG_PSRAM_KR4_IMG2_KB) (0 if IMG2/data not in PSRAM)
  __km4_bd_           -> +------------------------+  KM4_IMG2_PSRAM_START
  psram_start__          | KM4 BD PSRAM  (elastic)|  absorbs remainder up to PSRAM_END; heap clamped to ChipInfo_PsramHeapTop() at runtime
  PSRAM_END  0x61000000  +------------------------+  (DSP_BD_PSRAM length = 0)
 */

/* PSRAM layout, Kconfig driven (order per ASCII above). KR4 first so KM4/DSP resizes never move its base;
 * KM4 absorbs the TZ slice from its own budget so PSRAM_DSP_START is TZ-invariant; DSP takes the remainder up to PSRAM_END. */
#define KM4_PSRAM_TZ_START			(PSRAM_BASE)									/* TZ region start: 0x60000000 ~ 0x60010000 | unavailable */
#define KR4_IMG2_PSRAM_START		(KM4_PSRAM_TZ_START + KM4_PSRAM_TZ_SIZE)		/* 0x60010000 ~ 0x60190000 | 0x60000000 ~ 0x60180000*/
/* KR4 PSRAM IMG2 only when IMG2/data goes to PSRAM; else collapses to 0 and KM4
 * absorbs the whole non-secure PSRAM (mirrors CONFIG_PSRAM_KR4_IMG2_KB deps). */
#if defined(CONFIG_IMG2_PSRAM) || defined(CONFIG_DATA_HEAP_PSRAM)
#define PSRAM_KR4_IMG2_SIZE			KBYTES(CONFIG_PSRAM_KR4_IMG2_KB)
#else
#define PSRAM_KR4_IMG2_SIZE			0
#endif
#define KM4_IMG2_PSRAM_START		(KR4_IMG2_PSRAM_START + PSRAM_KR4_IMG2_SIZE)	/* 0x60190000 ~ 0x60300000 | 0x60180000 ~ 0x60300000*/

#if defined(CONFIG_DSP_EN) && !defined(CONFIG_DSP_RUN_IN_SRAM)
#define PSRAM_KM4_IMG2_SIZE			(KBYTES(CONFIG_PSRAM_KM4_IMG2_KB) - KM4_PSRAM_TZ_SIZE)
#define PSRAM_DSP_START				(KM4_IMG2_PSRAM_START + PSRAM_KM4_IMG2_SIZE)	/* 0x60300000 (TZ cancels, does not move) */
#define DSP_BD_PSRAM_ORIGIN			(PSRAM_DSP_START + IMAGE_HEADER_LEN)
#define DSP_BD_PSRAM_LENGTH			(PSRAM_END - PSRAM_DSP_START - IMAGE_HEADER_LEN)
ASSERT(KBYTES(CONFIG_PSRAM_KM4_IMG2_KB) > KM4_PSRAM_TZ_SIZE + IMAGE_HEADER_LEN, "CONFIG_PSRAM_KM4_IMG2_KB must exceed TZ size by at least the image header; KM4 PSRAM length would underflow.")
ASSERT(PSRAM_DSP_START + IMAGE_HEADER_LEN <= PSRAM_END, "PSRAM over-committed: TZ+KR4+KM4+DSP exceeds the 16MB window, shrink CONFIG_PSRAM_K*_IMG2_KB.")
#else	/* DSP off, or DSP in SRAM: PSRAM DSP segment collapses, KM4 absorbs up to PSRAM_END */
#define PSRAM_KM4_IMG2_SIZE			(PSRAM_END - KM4_IMG2_PSRAM_START)
#define PSRAM_DSP_START				(PSRAM_END)
#define DSP_BD_PSRAM_ORIGIN			(PSRAM_END)
#define DSP_BD_PSRAM_LENGTH			(0)
ASSERT(KM4_IMG2_PSRAM_START + IMAGE_HEADER_LEN <= PSRAM_END, "PSRAM over-committed: TZ+KR4 exceeds the 16MB window, shrink CONFIG_PSRAM_KR4_IMG2_KB.")
#endif

/* Collapse-safe ORIGIN/LENGTH: for a segment that can shrink to 0,
 * a bare "SIZE - IMAGE_HEADER_LEN" underflows to ~4GB (-> bogus heap, overlap, aliasing). Force LENGTH 0 instead. */
#if KM4_RAM_TZ_SIZE > 0
#define KM4_BD_RAM_TZ_ORIGIN		(KM4_RAM_TZ_START + IMAGE_HEADER_LEN)
#define KM4_BD_RAM_TZ_LENGTH		(KM4_RAM_TZ_SIZE - IMAGE_HEADER_LEN)
#else
#define KM4_BD_RAM_TZ_ORIGIN		(KM4_RAM_TZ_START)
#define KM4_BD_RAM_TZ_LENGTH		0
#endif

#if KM4_PSRAM_TZ_SIZE > 0
#define KM4_BD_PSRAM_TZ_ORIGIN		(KM4_PSRAM_TZ_START + IMAGE_HEADER_LEN)
#define KM4_BD_PSRAM_TZ_LENGTH		(KM4_PSRAM_TZ_SIZE - IMAGE_HEADER_LEN)
#else
#define KM4_BD_PSRAM_TZ_ORIGIN		(KM4_PSRAM_TZ_START)
#define KM4_BD_PSRAM_TZ_LENGTH		0
#endif

#if PSRAM_KR4_IMG2_SIZE > 0
#define KR4_BD_PSRAM_ORIGIN			(KR4_IMG2_PSRAM_START + IMAGE_HEADER_LEN)
#define KR4_BD_PSRAM_LENGTH			(PSRAM_KR4_IMG2_SIZE - IMAGE_HEADER_LEN)
#else
#define KR4_BD_PSRAM_ORIGIN			(KR4_IMG2_PSRAM_START)
#define KR4_BD_PSRAM_LENGTH			0
#endif

MEMORY
{
	/* KM4 ROM */
	KM4_IROM (rx) :                         ORIGIN = 0x00000000, LENGTH = 0x0001E000 - 0x00000000	/* KM4 IROM_S: 120k */
	KM4_IROM_NS (rx) :                      ORIGIN = 0x0001E000, LENGTH = 0x00034000 - 0x0001E000	/* KM4 IROM_NS: 88k */
	KM4_IROM_STDLIB_NS (rx) :               ORIGIN = 0x00034000, LENGTH = 0x00041000 - 0x00034000	/* KM4 STDLIB ROM: 52k */
	KM4_DROM_NS (rx) :                      ORIGIN = 0x00041000, LENGTH = 0x00045000 - 0x00041000	/* KM4 DROM_NS: 16k */
	KM4_DROM (rx) :                         ORIGIN = 0x00045000, LENGTH = 0x00048000 - 0x00045000	/* KM4 DROM_S: 12k */

	/* KM4 Secure RAM, BIT(28)=1 */
	KM4_ROMBSS_RAM_S (rw) :                 ORIGIN = 0x30007000, LENGTH = 0x30008000 - 0x30007000	/* KM4 ROM BSS RAM S: 4k */
	KM4_MSP_RAM_S (rw) :                    ORIGIN = 0x30008000, LENGTH = 0x3000A000 - 0x30008000	/* KM4 MSP_S RAM: 8k */
	KM4_BOOTLOADER_RAM_S (rwx) :            ORIGIN = KM4_IMG1_RAM_SATRT + SECURE_ADDR_OFFSET + IMAGE_HEADER_LEN, LENGTH = KM4_IMG1_SIZE  - IMAGE_HEADER_LEN		/* KM4 BOOT Loader RAM: 4K or 24k */
	KM4_FLOADER_RAM_S (rwx) :               ORIGIN = 0x3000A020, LENGTH = 0x30020000 - 0x3000A020	/* KM4 ImgTool Flash Loader RAM: 88k */

	/* KM4 Non-secure RAM */
	KM4_MSP_RAM_NS (rw) :                   ORIGIN = 0x20003000, LENGTH = 0x20004000 - 0x20003000	/* KM4  MSP_NS RAM: 4k */
	KM4_ROMBSS_RAM_COM (rw) :               ORIGIN = 0x20004000, LENGTH = 0x20005000 - 0x20004000	/* KM4  ROM BSS COMMON(S & NS both used) RAM: 4k */
	KM4_ROMBSS_RAM_NS (rw) :                ORIGIN = 0x20005000, LENGTH = 0x20005FBF - 0x20005000	/* KM4 ROM BSS NS RAM: 4k */
	KM4_BOOT_RAM (rwx) :                    ORIGIN = 0x20005FE0, LENGTH = 0x20006000 - 0x20005FE0	/* KM4 BOOT RAM: 64B */
	KM4_TIMER_IDLE_TASK_STACK (rw) :        ORIGIN = 0x20006000, LENGTH = 0x20007000 - 0x20006000	/* KM4 TIMER and IDLE task stack: 4k */

	/* PMC RAM, fixed in memory layout, do not changed */
	KR4_IPC_RAM (rwx) :                     ORIGIN = 0x20004900, LENGTH = 0x20004C00 - 0x20004900	/* KR4 IPC RAM: 768B */
	KR4_BOOT_RAM_BACKUP (rwx) :             ORIGIN = 0x20004C20, LENGTH = 0x20004C40 - 0x20004C20	/* KR4 BOOT RAM: 64B, Backup KR4 BootEntry.bin(32B Header+32B Entry)*/
	KR4_RAM_ONLY (rwx) :                    ORIGIN = 0x20005520, LENGTH = 0x20005900 - 0x20005520	/* KR4 PMC RAM: 1K */
	KM4_RAM_ONLY (rwx) :                    ORIGIN = 0x20005920, LENGTH = 0x20005D20 - 0x20005920	/* KM4 PMC RAM: 1K */
	KR4_BOOT_RAM (rwx) :                    ORIGIN = 0x20040020, LENGTH = 0x20040040 - 0x20040020	/* KR4 BOOT RAM: 32B, Can be ignore When DSP use this addr */

	/* IMG2 RAM */
	KM4_BD_RAM_TZ (rwx) :                   ORIGIN = KM4_BD_RAM_TZ_ORIGIN, 		LENGTH = KM4_BD_RAM_TZ_LENGTH		/* KM4 BD RAM TZ: TZ Off,0;TZ On,64K (NS-alias, SAU marks secure) */

	KM4_BD_RAM (rwx) :                      ORIGIN = KM4_IMG2_RAM_START + IMAGE_HEADER_LEN,   	LENGTH = RAM_KM4_IMG2_SIZE - IMAGE_HEADER_LEN	/* KM4 BD RAM */
	KR4_BD_RAM (rwx) :                      ORIGIN = KR4_IMG2_RAM_START + IMAGE_HEADER_LEN,   	LENGTH = RAM_KR4_IMG2_SIZE - IMAGE_HEADER_LEN	/* KR4 BD RAM */
	DSP_BD_SRAM (rwx) :                     ORIGIN = DSP_BD_SRAM_ORIGIN,   	LENGTH = DSP_BD_SRAM_LENGTH	/* DSP BD SRAM: fills SRAM tail when DSP_RUN_IN_SRAM; collapses to 0 otherwise */

	/* KM4 Non-secure PSRAM. Order: TZ -> KR4 -> KM4 -> DSP */
	KM4_BD_PSRAM_TZ (rwx) :                 ORIGIN = KM4_BD_PSRAM_TZ_ORIGIN, 	LENGTH = KM4_BD_PSRAM_TZ_LENGTH	/* KM4 BD PSRAM TZ: 64k (NS-alias, SAU marks secure) */

	KR4_BD_PSRAM (rwx) :                    ORIGIN = KR4_BD_PSRAM_ORIGIN, 	LENGTH = KR4_BD_PSRAM_LENGTH	/* KR4 BD PSRAM: CONFIG_PSRAM_KR4_IMG2_KB, collapses to 0 when KR4 has no PSRAM IMG2 */
	KM4_BD_PSRAM (rwx) :                    ORIGIN = KM4_IMG2_PSRAM_START + IMAGE_HEADER_LEN, 	LENGTH = PSRAM_KM4_IMG2_SIZE - IMAGE_HEADER_LEN	/* KM4 MAIN RAM NS: DSP on, CONFIG_PSRAM_KM4_IMG2_KB - TZ; DSP off, up to PSRAM_END */
	DSP_BD_PSRAM (rwx) :                    ORIGIN = DSP_BD_PSRAM_ORIGIN, 	  	LENGTH = DSP_BD_PSRAM_LENGTH	/* DSP BD PSRAM: fills to PSRAM_END; collapses to 0 when DSP off */

	/* Retention RAM */
	RETENTION_RAM (rwx) :                   ORIGIN = 0x4100CE00, LENGTH = 0x4100D000 - 0x4100CE00	/* KR4 Retention SRAM: 512B: 0k */

	/* KR4 ROM */
	KR4_IROM (rx) :                         ORIGIN = 0x00000000, LENGTH = 0x0001C000 - 0x00000000	/* KR4 IROM: 112k */
	KR4_IROM_STDLIB (rx) :                  ORIGIN = 0x0001C000, LENGTH = 0x0002D000 - 0x0001C000	/* KR4 STDLIB ROM: 68k */
	KR4_DROM (rx) :                         ORIGIN = 0x0002D000 , LENGTH = 0x00030000 - 0x0002D000 	/* KR4 DROM: 12k */

	/* KR4 RAM */
	KR4_ROMBSS_RAM (rw) :                   ORIGIN = 0x20000000, LENGTH = 0x20001000 - 0x20000000	/* KR4 ROM BSS RAM: 4k */
	KR4_MSP_RAM (rw) :                      ORIGIN = 0x20001000, LENGTH = 0x20002000 - 0x20001000	/* KR4 MSP RAM: 4k */
	KR4_TIMER_IDLE_TASK_STACK (rw) :        ORIGIN = 0x20002000, LENGTH = 0x20003000 - 0x20002000	/* KR4 TIMER and IDLE task stack: 4k */

	/* Flash */
	KR4_IMG2_XIP (rx) :                     ORIGIN = 0x0C000020, LENGTH = 0x0D000000 - 0x0C000020	/* KR4 IMG2 Virtual address: 32768k */
	DSP_IMG2_XIP (rx) :                     ORIGIN = 0x0D000020, LENGTH = 0x0E000000 - 0x0D000020	/* DSP IMG2 Virtual address: 24576k */
	KM4_IMG2_XIP (rx) :                     ORIGIN = 0x0E000020, LENGTH = 0x0F800000 - 0x0E000020	/* KM4 IMG2 Virtual address: 24576k */
	KM4_BOOT_XIP (rx) :                     ORIGIN = 0x0F800020, LENGTH = 0x10000000 - 0x0F800020	/* KM4 Bootloader Virtual address: 8192k */
	BTRACE (rx) :                           ORIGIN = 0xC8800000, LENGTH = 0xC9000000 - 0xC8800000	/* BT LOG Virtual address: 8192k */
	CTRACE (rx) :                           ORIGIN = 0xCA000000, LENGTH = 0xCA7FFFFF - 0xCA000000	/* COEX LOG Virtual address: 8192k */

	/* DSP ROM */
	DSP_IROM (rx) :                         ORIGIN = 0x00000000, LENGTH = 0x00000400 - 0x00000000	/* KM0 IROM: 1k */

	/* mp use */
	KM4_FLOADER_RAM_MP (rwx) :               ORIGIN = 0x3000A020, LENGTH = 0x30012000 - 0x3000A020	/* KM4 ImgTool Flash Loader RAM: 32k */

	KM4_BOOTLOADER_ENTRY (rwx) :             ORIGIN = 0x3000A020, LENGTH = 0x3000A400 - 0x3000A020	/* KM4 IMG1 Entry: 1024B aligned for flashloader */
	KM4_BOOTLOADER_RAM_MP (rwx) :            ORIGIN = 0x30013400 + IMAGE_HEADER_LEN, LENGTH = 0x3001A000 - 0x30013400 - IMAGE_HEADER_LEN	/* KM4 BootLoader RAM: 32k */

#if defined(CONFIG_WHC_HOST) || defined(CONFIG_WHC_NONE)
	KR4_BD_RAM_MP (rwx) :                   ORIGIN = 0x2001A060, LENGTH = 0x20061F80 - 0x2001A060 /* KR4 BD RAM: 288k */
	KR4_BD_RAM_MP_END (rwx) :               ORIGIN = 0x20061F80 - 32, LENGTH = 32                 /* Only for KR4 shrink MP image2 */
	KM4_BD_RAM_MP (rwx) :                   ORIGIN = 0x20062060, LENGTH = 0x2007FFA0 - 0x20062060 /* KM4 BD RAM: TZ off,120k */
#else
	KR4_BD_RAM_MP (rwx) :                   ORIGIN = 0x2001A060, LENGTH = 0x20041F80 - 0x2001A060 /* KR4 BD RAM: TZ off,160K */
	KM4_BD_RAM_MP (rwx) :                   ORIGIN = 0x20042060, LENGTH = 0x2007FFA0 - 0x20042060 /* KM4 BD RAM: 248k */
#endif
}

__kr4_image2_entry_func__ = ORIGIN(KR4_BD_RAM);
__kr4_ipc_memory_start__ = ORIGIN(KR4_IPC_RAM);
__kr4_flash_text_start__ = ORIGIN(KR4_IMG2_XIP);
__km4_flash_text_start__ = ORIGIN(KM4_IMG2_XIP);
__dsp_flash_text_start__ = ORIGIN(DSP_IMG2_XIP);
__kr4_psram_text_start__ = ORIGIN(KR4_BD_PSRAM);
__km4_boot_text_start__ = ORIGIN(KM4_BOOT_XIP);

__kr4_start_table_backup_addr__ = ORIGIN(KR4_BOOT_RAM_BACKUP);
__kr4_boot_entry_addr__ = ORIGIN(KR4_BOOT_RAM);

/*SRAM Layout boundary*/
/* TZ region symbols (__image3_ram_start__/end, __ram_image3_nsc_start__/end) are defined in img3 link script */
__km4_bd_ram_start__ = ORIGIN(KM4_BD_RAM) - IMAGE_HEADER_LEN;

/*PSRAM Layout boundary*/
__km4_bd_psram_start__ = ORIGIN(KM4_BD_PSRAM) - IMAGE_HEADER_LEN;
__non_secure_psram_start__ = KR4_IMG2_PSRAM_START;	/* first non-secure PSRAM segment (after TZ): MPC entry0 NS window base */
__dsp_psram_start__ = PSRAM_DSP_START;				/* DSP PSRAM segment base, for boot/pack checks */
__non_secure_psram_end__ = PSRAM_END;
__dsp_sram_start__ = DSP_BD_SRAM_START;				/* DSP SRAM segment base, mirrors RTK_LSP_SRAM (dsp_layout_sync.py) */
__dsp_sram_end__ = SRAM_END;						/* DSP SRAM segment ceiling */

__ap_sram_heap_start = SRAM_END;
__ap_sram_heap_size = KBYTES(256);

/* Region 0: KM4 IROM*/
__rom_text_region0_start__ = ORIGIN(KM4_IROM);
__rom_text_region0_end__   = ORIGIN(KM4_IROM_STDLIB_NS) + LENGTH(KM4_IROM_STDLIB_NS);

/* Region 1: KR4 IROM*/
__rom_text_region1_start__ = ORIGIN(KR4_IROM);
__rom_text_region1_end__   = ORIGIN(KR4_IROM_STDLIB) + LENGTH(KR4_IROM_STDLIB);


/* mp used */
__km4_bd_boot_download_addr__ = 0x30012000;
__kr4_bd_ram_mp_start__ = ORIGIN(KR4_BD_RAM_MP);
__km4_bd_ram_mp_start__ = ORIGIN(KM4_BD_RAM_MP);
__kr4_boot_entry2_addr__ = ORIGIN(KR4_RAM_ONLY);

/* code analyse used */
__rtk_fixed_addr__ = SRAM_BASE + KR4_KM4_RAM_SIZE;
__img1_sram_size__ = KM4_IMG1_SIZE;
__img3_sram_size__ = KM4_RAM_TZ_SIZE;

/* for sleep backup */
__km4_msp_ram_ns_limit__ = ORIGIN(KM4_MSP_RAM_NS);
