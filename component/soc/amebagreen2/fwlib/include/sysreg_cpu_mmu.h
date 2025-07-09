#ifndef _RL7005_CPU_MMU_H_
#define _RL7005_CPU_MMU_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup CPU_MMU
  * @brief CPU_MMU driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup CPU_MMU_Register_Definitions CPU_MMU Register Definitions
  * @{
  */

/** @defgroup REG_CMMU_REMAP0BAR_CPU0
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_BAR0_CPU0   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu0 mmu base addr for entry0 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_BAR0_CPU0(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_BAR0_CPU0(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP0EAR_CPU0
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_EAR0_CPU0   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu0 mmu end addr for entry0 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_EAR0_CPU0(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_EAR0_CPU0(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP0OR_CPU0
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_OFS0_CPU0   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu0 mmu offset addr when hit entry0. MMU unit in 1KB, so [9:0] is no used. HW always add this value to acture addr when hit , if need to sub, set this value to 2-comp minus value. Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_OFS0_CPU0(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_OFS0_CPU0(x) ((u32)(((x >> 10) & 0x003FFFFF)))
#define CMMU_BIT_REMAP_EN0_CPU0     ((u32)0x00000001 << 0)            /*!< R/W 0x0  cpu0 mmu enable for entry0*/
/** @} */

/** @defgroup REG_CMMU_REMAP1BAR_CPU0
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_BAR1_CPU0   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu0 mmu base addr for entry1 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_BAR1_CPU0(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_BAR1_CPU0(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP1EAR_CPU0
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_EAR1_CPU0   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu0 mmu end addr for entry1 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_EAR1_CPU0(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_EAR1_CPU0(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP1OR_CPU0
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_OFS1_CPU0   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu0 mmu offset addr when hit entry1. MMU unit in 1KB, so [9:0] is no used. HW always add this value to acture addr when hit , if need to sub, set this value to 2-comp minus value. Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_OFS1_CPU0(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_OFS1_CPU0(x) ((u32)(((x >> 10) & 0x003FFFFF)))
#define CMMU_BIT_REMAP_EN1_CPU0     ((u32)0x00000001 << 0)            /*!< R/W 0x0  cpu0 mmu enable for entry1*/
/** @} */

/** @defgroup REG_CMMU_REMAP2BAR_CPU0
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_BAR2_CPU0   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu0 mmu base addr for entry2 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_BAR2_CPU0(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_BAR2_CPU0(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP2EAR_CPU0
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_EAR2_CPU0   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu0 mmu end addr for entry2 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_EAR2_CPU0(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_EAR2_CPU0(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP2OR_CPU0
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_OFS2_CPU0   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu0 mmu offset addr when hit entry2. MMU unit in 1KB, so [9:0] is no used. HW always add this value to acture addr when hit , if need to sub, set this value to 2-comp minus value. Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_OFS2_CPU0(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_OFS2_CPU0(x) ((u32)(((x >> 10) & 0x003FFFFF)))
#define CMMU_BIT_REMAP_EN2_CPU0     ((u32)0x00000001 << 0)            /*!< R/W 0x0  cpu0 mmu enable for entry2*/
/** @} */

/** @defgroup REG_CMMU_REMAP3BAR_CPU0
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_BAR3_CPU0   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu0 mmu base addr for entry3 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_BAR3_CPU0(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_BAR3_CPU0(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP3EAR_CPU0
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_EAR3_CPU0   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu0 mmu end addr for entry3 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_EAR3_CPU0(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_EAR3_CPU0(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP3OR_CPU0
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_OFS3_CPU0   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu0 mmu offset addr when hit entry3. MMU unit in 1KB, so [9:0] is no used. HW always add this value to acture addr when hit , if need to sub, set this value to 2-comp minus value. Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_OFS3_CPU0(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_OFS3_CPU0(x) ((u32)(((x >> 10) & 0x003FFFFF)))
#define CMMU_BIT_REMAP_EN3_CPU0     ((u32)0x00000001 << 0)            /*!< R/W 0x0  cpu0 mmu enable for entry3*/
/** @} */

/** @defgroup REG_CMMU_REMAP0BAR_CPU1
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_BAR0_CPU1   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu1 mmu base addr for entry0 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_BAR0_CPU1(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_BAR0_CPU1(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP0EAR_CPU1
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_EAR0_CPU1   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu1 mmu end addr for entry0 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_EAR0_CPU1(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_EAR0_CPU1(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP0OR_CPU1
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_OFS0_CPU1   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu1 mmu offset addr when hit entry0. MMU unit in 1KB, so [9:0] is no used. HW always add this value to acture addr when hit , if need to sub, set this value to 2-comp minus value. Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_OFS0_CPU1(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_OFS0_CPU1(x) ((u32)(((x >> 10) & 0x003FFFFF)))
#define CMMU_BIT_REMAP_EN0_CPU1     ((u32)0x00000001 << 0)            /*!< R/W 0x0  cpu1 mmu enable for entry0*/
/** @} */

/** @defgroup REG_CMMU_REMAP1BAR_CPU1
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_BAR1_CPU1   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu1 mmu base addr for entry1 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_BAR1_CPU1(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_BAR1_CPU1(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP1EAR_CPU1
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_EAR1_CPU1   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu1 mmu end addr for entry1 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_EAR1_CPU1(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_EAR1_CPU1(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP1OR_CPU1
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_OFS1_CPU1   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu1 mmu offset addr when hit entry1. MMU unit in 1KB, so [9:0] is no used. HW always add this value to acture addr when hit , if need to sub, set this value to 2-comp minus value. Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_OFS1_CPU1(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_OFS1_CPU1(x) ((u32)(((x >> 10) & 0x003FFFFF)))
#define CMMU_BIT_REMAP_EN1_CPU1     ((u32)0x00000001 << 0)            /*!< R/W 0x0  cpu1 mmu enable for entry1*/
/** @} */

/** @defgroup REG_CMMU_REMAP2BAR_CPU1
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_BAR2_CPU1   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu1 mmu base addr for entry2 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_BAR2_CPU1(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_BAR2_CPU1(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP2EAR_CPU1
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_EAR2_CPU1   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu1 mmu end addr for entry2 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_EAR2_CPU1(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_EAR2_CPU1(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP2OR_CPU1
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_OFS2_CPU1   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu1 mmu offset addr when hit entry2. MMU unit in 1KB, so [9:0] is no used. HW always add this value to acture addr when hit , if need to sub, set this value to 2-comp minus value. Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_OFS2_CPU1(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_OFS2_CPU1(x) ((u32)(((x >> 10) & 0x003FFFFF)))
#define CMMU_BIT_REMAP_EN2_CPU1     ((u32)0x00000001 << 0)            /*!< R/W 0x0  cpu1 mmu enable for entry2*/
/** @} */

/** @defgroup REG_CMMU_REMAP3BAR_CPU1
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_BAR3_CPU1   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu1 mmu base addr for entry3 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_BAR3_CPU1(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_BAR3_CPU1(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP3EAR_CPU1
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_EAR3_CPU1   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu1 mmu end addr for entry3 Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_EAR3_CPU1(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_EAR3_CPU1(x) ((u32)(((x >> 10) & 0x003FFFFF)))
/** @} */

/** @defgroup REG_CMMU_REMAP3OR_CPU1
  * @brief
  * @{
  */
#define CMMU_MASK_REMAP_OFS3_CPU1   ((u32)0x003FFFFF << 10)           /*!< R/W 0x0  cpu1 mmu offset addr when hit entry3. MMU unit in 1KB, so [9:0] is no used. HW always add this value to acture addr when hit , if need to sub, set this value to 2-comp minus value. Note:[31:20] not used for mmu only for sram and rom*/
#define CMMU_REMAP_OFS3_CPU1(x)     (((u32)((x) & 0x003FFFFF) << 10))
#define CMMU_GET_REMAP_OFS3_CPU1(x) ((u32)(((x >> 10) & 0x003FFFFF)))
#define CMMU_BIT_REMAP_EN3_CPU1     ((u32)0x00000001 << 0)            /*!< R/W 0x0  cpu1 mmu enable for entry3*/
/** @} */

/*==========CPU_MMU Register Address Definition==========*/
#define REG_CMMU_REMAP0BAR_CPU0          0x000
#define REG_CMMU_REMAP0EAR_CPU0          0x004
#define REG_CMMU_REMAP0OR_CPU0           0x008
#define REG_CMMU_REMAP1BAR_CPU0          0x00C
#define REG_CMMU_REMAP1EAR_CPU0          0x010
#define REG_CMMU_REMAP1OR_CPU0           0x014
#define REG_CMMU_REMAP2BAR_CPU0          0x018
#define REG_CMMU_REMAP2EAR_CPU0          0x01C
#define REG_CMMU_REMAP2OR_CPU0           0x020
#define REG_CMMU_REMAP3BAR_CPU0          0x024
#define REG_CMMU_REMAP3EAR_CPU0          0x028
#define REG_CMMU_REMAP3OR_CPU0           0x02C
#define REG_CMMU_REMAP0BAR_CPU1          0x040
#define REG_CMMU_REMAP0EAR_CPU1          0x044
#define REG_CMMU_REMAP0OR_CPU1           0x048
#define REG_CMMU_REMAP1BAR_CPU1          0x04C
#define REG_CMMU_REMAP1EAR_CPU1          0x050
#define REG_CMMU_REMAP1OR_CPU1           0x054
#define REG_CMMU_REMAP2BAR_CPU1          0x058
#define REG_CMMU_REMAP2EAR_CPU1          0x05C
#define REG_CMMU_REMAP2OR_CPU1           0x060
#define REG_CMMU_REMAP3BAR_CPU1          0x064
#define REG_CMMU_REMAP3EAR_CPU1          0x068
#define REG_CMMU_REMAP3OR_CPU1           0x06C

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your definition here

/* MANUAL_GEN_END */

/** @} */

/** @} */

#endif