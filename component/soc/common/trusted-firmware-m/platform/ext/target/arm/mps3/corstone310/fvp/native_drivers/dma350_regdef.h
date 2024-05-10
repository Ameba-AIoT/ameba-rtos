/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __DMA350_REGDEF_H
#define __DMA350_REGDEF_H

#include <stdint.h>
#define __IM      volatile const
#define __OM      volatile
#define __IOM     volatile
/******************************************************************************/
/*                Type definitions for DMA350 register blocks                */
/******************************************************************************/
typedef struct
{
  __IOM uint32_t CH_CMD;                             /*!< Address offset: 0x000, Channel DMA Command Register */
  __IOM uint32_t CH_STATUS;                          /*!< Address offset: 0x004, Channel Status Register */
  __IOM uint32_t CH_INTREN;                          /*!< Address offset: 0x008, Channel Interrupt Enable Register */
  __IOM uint32_t CH_CTRL;                            /*!< Address offset: 0x00C, Channel Control Register */
  __IOM uint32_t CH_SRCADDR;                         /*!< Address offset: 0x010, Channel Source Address Register */
  __IOM uint32_t CH_SRCADDRHI;                       /*!< Address offset: 0x014, Channel Source Address Register High Bits [63:32] */
  __IOM uint32_t CH_DESADDR;                         /*!< Address offset: 0x018, Channel Destination Address Register */
  __IOM uint32_t CH_DESADDRHI;                       /*!< Address offset: 0x01C, Channel Destination Address Register, High Bits [63:32] */
  __IOM uint32_t CH_XSIZE;                           /*!< Address offset: 0x020, Channel X Dimension Size Register, Lower Bits [15:0] */
  __IOM uint32_t CH_XSIZEHI;                         /*!< Address offset: 0x024, Channel X Dimension Size Register, High Bits [31:16] */
  __IOM uint32_t CH_SRCTRANSCFG;                     /*!< Address offset: 0x028, Channel Source Transfer Configuration Register */
  __IOM uint32_t CH_DESTRANSCFG;                     /*!< Address offset: 0x02C, Channel Destination Transfer Configuration Register */
  __IOM uint32_t CH_XADDRINC;                        /*!< Address offset: 0x030, Channel X Dimension Address Increment Register */
  __IOM uint32_t CH_YADDRSTRIDE;                     /*!< Address offset: 0x034, Channel Y Dimension Address Stride Register */
  __IOM uint32_t CH_FILLVAL;                         /*!< Address offset: 0x038, Channel Fill Pattern Value Register */
  __IOM uint32_t CH_YSIZE;                           /*!< Address offset: 0x03C, Channel Y Dimensions Size Register */
  __IOM uint32_t CH_TMPLTCFG;                        /*!< Address offset: 0x040, Channel Template Configuration Register */
  __IOM uint32_t CH_SRCTMPLT;                        /*!< Address offset: 0x044, Channel Source Template Pattern Register */
  __IOM uint32_t CH_DESTMPLT;                        /*!< Address offset: 0x048, Channel Destination Template Pattern Register */
  __IOM uint32_t CH_SRCTRIGINCFG;                    /*!< Address offset: 0x04C, Channel Source Trigger In Configuration Register */
  __IOM uint32_t CH_DESTRIGINCFG;                    /*!< Address offset: 0x050, Channel Destination Trigger In Configuration Register */
  __IOM uint32_t CH_TRIGOUTCFG;                      /*!< Address offset: 0x054, Channel Trigger Out Configuration Register */
  __IOM uint32_t CH_GPOEN0;                          /*!< Address offset: 0x058, Channel GPO  Driving Enable Register 0 */
  uint32_t       RESERVED0[1];                       /*!< Address offset: 0x5C - 0x5C, Reserved */
  __IOM uint32_t CH_GPOVAL0;                         /*!< Address offset: 0x060, Channel GPO Value Register 0 */
  uint32_t       RESERVED1[1];                       /*!< Address offset: 0x64 - 0x64, Reserved */
  __IOM uint32_t CH_STREAMINTCFG;                    /*!< Address offset: 0x068, Channel Stream Interface Configuration Register */
  uint32_t       RESERVED2[1];                       /*!< Address offset: 0x6C - 0x6C, Reserved */
  __IOM uint32_t CH_LINKATTR;                        /*!< Address offset: 0x070, Channel Link Address Memory Attributes Register */
  __IOM uint32_t CH_AUTOCFG;                         /*!< Address offset: 0x074, Channel Automatic Command Restart Configuration Register */
  __IOM uint32_t CH_LINKADDR;                        /*!< Address offset: 0x078, Channel Link Address Register */
  __IOM uint32_t CH_LINKADDRHI;                      /*!< Address offset: 0x07C, Channel Link Address Register, High Bits [63:32] */
  __IOM uint32_t CH_GPOREAD0;                        /*!< Address offset: 0x080, Channel GPO Read Value Register 0 */
  uint32_t       RESERVED3[1];                       /*!< Address offset: 0x84 - 0x84, Reserved */
  __IOM uint32_t CH_WRKREGPTR;                       /*!< Address offset: 0x088, Channel Working Register Pointer Register */
  __IOM uint32_t CH_WRKREGVAL;                       /*!< Address offset: 0x08C, Channel - Working Register Value Register */
  __IOM uint32_t CH_ERRINFO;                         /*!< Address offset: 0x090, Channel Error Information Register */
  uint32_t       RESERVED4[13];                      /*!< Address offset: 0x94 - 0xC4, Reserved */
  __IOM uint32_t CH_IIDR;                            /*!< Address offset: 0x0C8, Channel Implementation Identification Register */
  __IOM uint32_t CH_AIDR;                            /*!< Address offset: 0x0CC, Channel Architecture Identification Register */
  uint32_t       RESERVED5[6];                       /*!< Address offset: 0xD0 - 0xE4, Reserved */
  __IOM uint32_t CH_ISSUECAP;                        /*!< Address offset: 0x0E8, Used for setting issuing capability threshold. */
  uint32_t       RESERVED6[3];                       /*!< Address offset: 0xEC - 0xF4, Reserved */
  __IOM uint32_t CH_BUILDCFG0;                       /*!< Address offset: 0x0F8, Channel Build Configuration and Capability Register 0 */
  __IOM uint32_t CH_BUILDCFG1;                       /*!< Address offset: 0x0FC, Channel Build Configuration and Capability Register 1 */
} DMACH_TypeDef;
typedef struct
{
  __IOM uint32_t NSEC_CHINTRSTATUS0;                 /*!< Address offset: 0x000, Non-Secure Channel Interrupt Status Register 0 */
  uint32_t       RESERVED0[1];                       /*!< Address offset: 0x4 - 0x4, Reserved */
  __IOM uint32_t NSEC_STATUS;                        /*!< Address offset: 0x008, Non-Secure Status Register */
  __IOM uint32_t NSEC_CTRL;                          /*!< Address offset: 0x00C, Non-Secure Control Register */
  uint32_t       RESERVED1[1];                       /*!< Address offset: 0x10 - 0x10, Reserved */
  __IOM uint32_t NSEC_CHPTR;                         /*!< Address offset: 0x014, Non-Secure Channel Pointer */
  __IOM uint32_t NSEC_CHCFG;                         /*!< Address offset: 0x018, Non-Secure Channel Configuration Register */
  uint32_t       RESERVED2[53];                      /*!< Address offset: 0x1C - 0xEC, Reserved */
  __IOM uint32_t NSEC_STATUSPTR;                     /*!< Address offset: 0x0F0, Non-Secure Unit Status Pointer Register */
  __IOM uint32_t NSEC_STATUSVAL;                     /*!< Address offset: 0x0F4, Non-Secure Unit Status Value Register */
  __IOM uint32_t NSEC_SIGNALPTR;                     /*!< Address offset: 0x0F8, Non-Secure Unit Signal Pointer */
  __IOM uint32_t NSEC_SIGNALVAL;                     /*!< Address offset: 0x0FC, Non-Secure Unit Signal Value Register */
} DMANSECCTRL_TypeDef;
typedef struct
{
  __IOM uint32_t SEC_CHINTRSTATUS0;                  /*!< Address offset: 0x000, Secure Channel Interrupt Status Register 0 */
  uint32_t       RESERVED0[1];                       /*!< Address offset: 0x4 - 0x4, Reserved */
  __IOM uint32_t SEC_STATUS;                         /*!< Address offset: 0x008, Secure Status Register */
  __IOM uint32_t SEC_CTRL;                           /*!< Address offset: 0x00C, Secure Control Register */
  uint32_t       RESERVED1[1];                       /*!< Address offset: 0x10 - 0x10, Reserved */
  __IOM uint32_t SEC_CHPTR;                          /*!< Address offset: 0x014, Secure Channel Pointer */
  __IOM uint32_t SEC_CHCFG;                          /*!< Address offset: 0x018, Secure Channel Configuration Register */
  uint32_t       RESERVED2[53];                      /*!< Address offset: 0x1C - 0xEC, Reserved */
  __IOM uint32_t SEC_STATUSPTR;                      /*!< Address offset: 0x0F0, Secure Unit Status Pointer Register */
  __IOM uint32_t SEC_STATUSVAL;                      /*!< Address offset: 0x0F4, Secure Unit Status Value Register */
  __IOM uint32_t SEC_SIGNALPTR;                      /*!< Address offset: 0x0F8, Secure Unit Signal Pointer */
  __IOM uint32_t SEC_SIGNALVAL;                      /*!< Address offset: 0x0FC, Secure Unit Signal Value Register */
} DMASECCTRL_TypeDef;
typedef struct
{
  __IOM uint32_t SCFG_CHSEC0;                        /*!< Address offset: 0x000, Secure Configuration Channel Security Mapping Register 0 */
  uint32_t       RESERVED0[1];                       /*!< Address offset: 0x4 - 0x4, Reserved */
  __IOM uint32_t SCFG_TRIGINSEC0;                    /*!< Address offset: 0x008, Secure Configuration Trigger Input Security Mapping Register 0 */
  uint32_t       RESERVED1[7];                       /*!< Address offset: 0xC - 0x24, Reserved */
  __IOM uint32_t SCFG_TRIGOUTSEC0;                   /*!< Address offset: 0x028, Secure Configuration Trigger Output Security Mapping Register 0 */
  uint32_t       RESERVED2[5];                       /*!< Address offset: 0x2C - 0x3C, Reserved */
  __IOM uint32_t SCFG_CTRL;                          /*!< Address offset: 0x040, Secure Configuration Control */
  __IOM uint32_t SCFG_INTRSTATUS;                    /*!< Address offset: 0x044, Secure Configuration Interrupt Status */
} DMASECCFG_TypeDef;
typedef struct
{
  uint32_t       RESERVED0[44];                      /*!< Address offset: 0x0 - 0xAC, Reserved */
  __IOM uint32_t DMA_BUILDCFG0;                      /*!< Address offset: 0x0B0, DMA Build Configuration Register 0 */
  __IOM uint32_t DMA_BUILDCFG1;                      /*!< Address offset: 0x0B4, DMA Build Configuration Register 1 */
  __IOM uint32_t DMA_BUILDCFG2;                      /*!< Address offset: 0x0B8, DMA Build Configuration Register 2 */
  uint32_t       RESERVED1[3];                       /*!< Address offset: 0xBC - 0xC4, Reserved */
  __IOM uint32_t IIDR;                               /*!< Address offset: 0x0C8, Implementation Identification Register */
  __IOM uint32_t AIDR;                               /*!< Address offset: 0x0CC, Architecture Identification Register */
  __IOM uint32_t PIDR4;                              /*!< Address offset: 0x0D0, Peripheral ID4 Register */
  uint32_t       RESERVED2[3];                       /*!< Address offset: 0xD4 - 0xDC, Reserved */
  __IOM uint32_t PIDR0;                              /*!< Address offset: 0x0E0, Peripheral ID0 Register */
  __IOM uint32_t PIDR1;                              /*!< Address offset: 0x0E4, Peripheral ID1 Register */
  __IOM uint32_t PIDR2;                              /*!< Address offset: 0x0E8, Peripheral ID2 Register */
  __IOM uint32_t PIDR3;                              /*!< Address offset: 0x0EC, Peripheral ID 3 Register */
  __IOM uint32_t CIDR0;                              /*!< Address offset: 0x0F0, Component ID0 Register */
  __IOM uint32_t CIDR1;                              /*!< Address offset: 0x0F4, Component ID1 Register */
  __IOM uint32_t CIDR2;                              /*!< Address offset: 0x0F8, Component ID2 Register */
  __IOM uint32_t CIDR3;                              /*!< Address offset: 0x0FC, Component ID3 Register */
} DMAINFO_TypeDef;

/******************************************************************************/
/*                       Field Definitions of Registers                       */
/******************************************************************************/
/******************************************************************************/
/*                                   DMACH                                    */
/******************************************************************************/
/******************  Field definitions for CH_CMD register  *******************/
#define DMA_CH_CMD_ENABLECMD_Pos                    (0U)
#define DMA_CH_CMD_ENABLECMD_Msk                    (0x1UL << DMA_CH_CMD_ENABLECMD_Pos)                       /*!< 0x00000001UL*/
#define DMA_CH_CMD_ENABLECMD                        DMA_CH_CMD_ENABLECMD_Msk                                  /*!< ENABLECMD bit Channel Enable. When set to '1', enables the channel to run its programmed task. When set to '1', it cannot be set back to zero, and this field will automatically clear to zero when a DMA process is completed. To force the DMA to stop prematurely, you must use CH_CMD.STOPCMD instead.*/
#define DMA_CH_CMD_CLEARCMD_Pos                     (1U)
#define DMA_CH_CMD_CLEARCMD_Msk                     (0x1UL << DMA_CH_CMD_CLEARCMD_Pos)                        /*!< 0x00000002UL*/
#define DMA_CH_CMD_CLEARCMD                         DMA_CH_CMD_CLEARCMD_Msk                                   /*!< CLEARCMD bit DMA Clear command. When set to '1', it will remain high until all DMA channel registers and any internal queues and buffers are cleared, before returning to '0'. When set at the same time as ENABLECMD or while the DMA channel is already enabled, the clear will only occur after any ongoing DMA operation is either completed, stopped or disabled and the ENABLECMD bit is deasserted by the DMA.*/
#define DMA_CH_CMD_DISABLECMD_Pos                   (2U)
#define DMA_CH_CMD_DISABLECMD_Msk                   (0x1UL << DMA_CH_CMD_DISABLECMD_Pos)                      /*!< 0x00000004UL*/
#define DMA_CH_CMD_DISABLECMD                       DMA_CH_CMD_DISABLECMD_Msk                                 /*!< DISABLECMD bit Disable DMA Operation at the end of the current DMA command operation. Once set to '1', this field will stay high and the current DMA command will be allowed to complete, but the DMA will not fetch the next linked command or will it auto-restart the DMA command even if they are set. Once the DMA has stopped, it will return to '0' and ENABLECMD is also cleared. When set at the same time as ENABLECMD or when the channel is not enabled then write to this register is ignored.*/
#define DMA_CH_CMD_STOPCMD_Pos                      (3U)
#define DMA_CH_CMD_STOPCMD_Msk                      (0x1UL << DMA_CH_CMD_STOPCMD_Pos)                         /*!< 0x00000008UL*/
#define DMA_CH_CMD_STOPCMD                          DMA_CH_CMD_STOPCMD_Msk                                    /*!< STOPCMD bit Stop Current DMA Operation. Once set to '1', his will remain high until the DMA channel is stopped cleanly. Then this will return to '0' and ENABLECMD is also cleared. When set at the same time as ENABLECMD or when the channel is not enabled then write to this register is ignored. Note that each DMA channel can have other sources of a stop request and this field will not reflect the state of the other sources.*/
#define DMA_CH_CMD_PAUSECMD_Pos                     (4U)
#define DMA_CH_CMD_PAUSECMD_Msk                     (0x1UL << DMA_CH_CMD_PAUSECMD_Pos)                        /*!< 0x00000010UL*/
#define DMA_CH_CMD_PAUSECMD                         DMA_CH_CMD_PAUSECMD_Msk                                   /*!< PAUSECMD bit Pause Current DMA Operation. Once set to '1' the status cannot change until the DMA operation reached the paused state indicated by the STAT_PAUSED and STAT_RESUMEWAIT bits. The bit can be set by SW by writing it to '1', the current active DMA operation will be paused as soon as possible, but the ENABLECMD bit will remain HIGH to show that the operation is still active.  Cleared automatically when STAT_RESUMEWAIT is set and the RESUMECMD bit is written to '1', meaning that the SW continues the operation of the channel. Note that each DMA channel can have other sources of a pause request and this field will not reflect the state of the other sources. When set at the same time as ENABLECMD or when the channel is not enabled then write to this register is ignored.*/
#define DMA_CH_CMD_RESUMECMD_Pos                    (5U)
#define DMA_CH_CMD_RESUMECMD_Msk                    (0x1UL << DMA_CH_CMD_RESUMECMD_Pos)                       /*!< 0x00000020UL*/
#define DMA_CH_CMD_RESUMECMD                        DMA_CH_CMD_RESUMECMD_Msk                                  /*!< RESUMECMD bit Resume Current DMA Operation. Writing this bit to '1' means that the DMAC can continue the operation of a paused channel. Can be set to '1' when the PAUSECMD or a STAT_DONE assertion with DONEPAUSEEN set HIGH results in pausing the current DMA channel operation indicated by the STAT_PAUSED and STAT_RESUMEWAIT bits. Otherwise, writes to this bit are ignored.*/
#define DMA_CH_CMD_SRCSWTRIGINREQ_Pos               (16U)
#define DMA_CH_CMD_SRCSWTRIGINREQ_Msk               (0x1UL << DMA_CH_CMD_SRCSWTRIGINREQ_Pos)                  /*!< 0x00010000UL*/
#define DMA_CH_CMD_SRCSWTRIGINREQ                   DMA_CH_CMD_SRCSWTRIGINREQ_Msk                             /*!< SRCSWTRIGINREQ bit Software Generated Source Trigger Input Request. Write to '1' to create a SW trigger request to the DMA with the specified type in the SRCSWTRIGINTYPE register. Once set to '1', this will remain high until the DMA accepted the trigger and will return this to '0'. It will also be cleared automatically if the current command is completed without expecting another trigger event. When the channel is not enabled, write to this register is ignored.*/
#define DMA_CH_CMD_SRCSWTRIGINTYPE_Pos              (17U)
#define DMA_CH_CMD_SRCSWTRIGINTYPE_Msk              (0x3UL << DMA_CH_CMD_SRCSWTRIGINTYPE_Pos)                 /*!< 0x00060000UL*/
#define DMA_CH_CMD_SRCSWTRIGINTYPE                  DMA_CH_CMD_SRCSWTRIGINTYPE_Msk                            /*!< SRCSWTRIGINTYPE[ 1:0] bits Software Generated Source Trigger Input Request Type. Selects the trigger request type for the source trigger input when the SW triggers the SRCSWTRIGINREQ bit.
 - 00: Single request
 - 01: Last single request
 - 10: Block request
 - 11: Last block request
This field cannot be changed while the SRCSWTRIGINREQ bit is set.*/
#define DMA_CH_CMD_SRCSWTRIGINTYPE_0                (0x1UL << DMA_CH_CMD_SRCSWTRIGINTYPE_Pos)                 /*!< 0x00020000UL*/
#define DMA_CH_CMD_SRCSWTRIGINTYPE_1                (0x2UL << DMA_CH_CMD_SRCSWTRIGINTYPE_Pos)                 /*!< 0x00040000UL*/
#define DMA_CH_CMD_DESSWTRIGINREQ_Pos               (20U)
#define DMA_CH_CMD_DESSWTRIGINREQ_Msk               (0x1UL << DMA_CH_CMD_DESSWTRIGINREQ_Pos)                  /*!< 0x00100000UL*/
#define DMA_CH_CMD_DESSWTRIGINREQ                   DMA_CH_CMD_DESSWTRIGINREQ_Msk                             /*!< DESSWTRIGINREQ bit Software Generated Destination Trigger Input Request. Write to '1' to create a SW trigger request to the DMA with the specified type in the DESSWTRIGINTYPE register. Once set to '1', this will remain high until the DMA is accepted the trigger and will return this to '0'. It will also be cleared automatically if the current command is completed without expecting another trigger event. When the channel is not enabled, write to this register is ignored.*/
#define DMA_CH_CMD_DESSWTRIGINTYPE_Pos              (21U)
#define DMA_CH_CMD_DESSWTRIGINTYPE_Msk              (0x3UL << DMA_CH_CMD_DESSWTRIGINTYPE_Pos)                 /*!< 0x00600000UL*/
#define DMA_CH_CMD_DESSWTRIGINTYPE                  DMA_CH_CMD_DESSWTRIGINTYPE_Msk                            /*!< DESSWTRIGINTYPE[ 1:0] bits Software Generated Destination Trigger Input Request Type. Selects the trigger request type for the destination trigger input when the SW triggers the DESSWTRIGINREQ bit.
 - 00: Single request
 - 01: Last single request
 - 10: Block request
 - 11: Last block request
This field cannot be changed while the DESSWTRIGINREQ bit is set.*/
#define DMA_CH_CMD_DESSWTRIGINTYPE_0                (0x1UL << DMA_CH_CMD_DESSWTRIGINTYPE_Pos)                 /*!< 0x00200000UL*/
#define DMA_CH_CMD_DESSWTRIGINTYPE_1                (0x2UL << DMA_CH_CMD_DESSWTRIGINTYPE_Pos)                 /*!< 0x00400000UL*/
#define DMA_CH_CMD_SWTRIGOUTACK_Pos                 (24U)
#define DMA_CH_CMD_SWTRIGOUTACK_Msk                 (0x1UL << DMA_CH_CMD_SWTRIGOUTACK_Pos)                    /*!< 0x01000000UL*/
#define DMA_CH_CMD_SWTRIGOUTACK                     DMA_CH_CMD_SWTRIGOUTACK_Msk                               /*!< SWTRIGOUTACK bit Software Generated Trigger Output Acknowledge. Write '1' to acknowledge a Trigger Output request from the DMA. Once set to '1', this will remain high until the DMA Trigger Output is raised (either on the trigger output signal or as an interrupt) and the acknowledge is accepted. When the channel is not enabled, write to this register is ignored.*/
/*****************  Field definitions for CH_STATUS register  *****************/
#define DMA_CH_STATUS_INTR_DONE_Pos                 (0U)
#define DMA_CH_STATUS_INTR_DONE_Msk                 (0x1UL << DMA_CH_STATUS_INTR_DONE_Pos)                    /*!< 0x00000001UL*/
#define DMA_CH_STATUS_INTR_DONE                     DMA_CH_STATUS_INTR_DONE_Msk                               /*!< INTR_DONE bit Done Interrupt Flag. This interrupt will be set to HIGH if the INTREN_DONE is set and the STAT_DONE status flag gets raised. Automatically cleared when STAT_DONE is cleared.*/
#define DMA_CH_STATUS_INTR_ERR_Pos                  (1U)
#define DMA_CH_STATUS_INTR_ERR_Msk                  (0x1UL << DMA_CH_STATUS_INTR_ERR_Pos)                     /*!< 0x00000002UL*/
#define DMA_CH_STATUS_INTR_ERR                      DMA_CH_STATUS_INTR_ERR_Msk                                /*!< INTR_ERR bit Error Interrupt Flag. This interrupt will be set to HIGH if the INTREN_ERR is set and the STAT_ERR status flag gets raised. Automatically cleared when STAT_ERR is cleared.*/
#define DMA_CH_STATUS_INTR_DISABLED_Pos             (2U)
#define DMA_CH_STATUS_INTR_DISABLED_Msk             (0x1UL << DMA_CH_STATUS_INTR_DISABLED_Pos)                /*!< 0x00000004UL*/
#define DMA_CH_STATUS_INTR_DISABLED                 DMA_CH_STATUS_INTR_DISABLED_Msk                           /*!< INTR_DISABLED bit Disabled Interrupt Flag. This interrupt will be set to HIGH if the INTREN_DISABLED is set and the STAT_DISABLED flag gets raised. Automatically cleared when STAT_DISABLED is cleared.*/
#define DMA_CH_STATUS_INTR_STOPPED_Pos              (3U)
#define DMA_CH_STATUS_INTR_STOPPED_Msk              (0x1UL << DMA_CH_STATUS_INTR_STOPPED_Pos)                 /*!< 0x00000008UL*/
#define DMA_CH_STATUS_INTR_STOPPED                  DMA_CH_STATUS_INTR_STOPPED_Msk                            /*!< INTR_STOPPED bit Stopped Interrupt Flag. This interrupt will be set to HIGH if the INTREN_STOPPED is set and the STAT_STOPPED flag gets raised. Automatically cleared when STAT_STOPPED is cleared.*/
#define DMA_CH_STATUS_INTR_SRCTRIGINWAIT_Pos        (8U)
#define DMA_CH_STATUS_INTR_SRCTRIGINWAIT_Msk        (0x1UL << DMA_CH_STATUS_INTR_SRCTRIGINWAIT_Pos)           /*!< 0x00000100UL*/
#define DMA_CH_STATUS_INTR_SRCTRIGINWAIT            DMA_CH_STATUS_INTR_SRCTRIGINWAIT_Msk                      /*!< INTR_SRCTRIGINWAIT bit Channel is waiting for Source Trigger Interrupt Flag. This interrupt will be set to HIGH if the INTREN_SRCTRIGINWAIT is set and the STAT_SRCTRIGINWAIT status flag is asserted. Automatically cleared when STAT_SRCTRIGINWAIT is cleared.*/
#define DMA_CH_STATUS_INTR_DESTRIGINWAIT_Pos        (9U)
#define DMA_CH_STATUS_INTR_DESTRIGINWAIT_Msk        (0x1UL << DMA_CH_STATUS_INTR_DESTRIGINWAIT_Pos)           /*!< 0x00000200UL*/
#define DMA_CH_STATUS_INTR_DESTRIGINWAIT            DMA_CH_STATUS_INTR_DESTRIGINWAIT_Msk                      /*!< INTR_DESTRIGINWAIT bit Channel is waiting for Destination Trigger Interrupt Flag. This interrupt will be set to HIGH if the INTREN_DESTRIGINWAIT is set and the STAT_DESTRIGINWAIT status flag is asserted. Automatically cleared when STAT_DESTRIGINWAIT is cleared.*/
#define DMA_CH_STATUS_INTR_TRIGOUTACKWAIT_Pos       (10U)
#define DMA_CH_STATUS_INTR_TRIGOUTACKWAIT_Msk       (0x1UL << DMA_CH_STATUS_INTR_TRIGOUTACKWAIT_Pos)          /*!< 0x00000400UL*/
#define DMA_CH_STATUS_INTR_TRIGOUTACKWAIT           DMA_CH_STATUS_INTR_TRIGOUTACKWAIT_Msk                     /*!< INTR_TRIGOUTACKWAIT bit Channel is waiting for output Trigger Acknowledgement Interrupt Flag.  This interrupt will be set to HIGH if the INTREN_TRIGOUTACKWAIT is set and the STAT_TRIGOUTACKWAIT status flag is asserted. Automatically cleared when STAT_TRIGOUTACKWAIT is cleared.*/
#define DMA_CH_STATUS_STAT_DONE_Pos                 (16U)
#define DMA_CH_STATUS_STAT_DONE_Msk                 (0x1UL << DMA_CH_STATUS_STAT_DONE_Pos)                    /*!< 0x00010000UL*/
#define DMA_CH_STATUS_STAT_DONE                     DMA_CH_STATUS_STAT_DONE_Msk                               /*!< STAT_DONE bit Done Status Flag. This flag will be set to HIGH when the DMA command reaches the state defined by the DONETYPE settings. When DONEPAUSEEN is set the DMA command operation is paused when this flag is asserted. Write '1' to this bit to clear it. Automatically cleared when the ENABLECMD is set.*/
#define DMA_CH_STATUS_STAT_ERR_Pos                  (17U)
#define DMA_CH_STATUS_STAT_ERR_Msk                  (0x1UL << DMA_CH_STATUS_STAT_ERR_Pos)                     /*!< 0x00020000UL*/
#define DMA_CH_STATUS_STAT_ERR                      DMA_CH_STATUS_STAT_ERR_Msk                                /*!< STAT_ERR bit Error Status Flag. This flag will be set to HIGH if the DMA encounters an error during its operation. The details about the error event can be found in the ERRINFO register. Write '1' to this bit to clear it. When cleared, it also clears the ERRINFO register. Automatically cleared when the ENABLECMD is set.*/
#define DMA_CH_STATUS_STAT_DISABLED_Pos             (18U)
#define DMA_CH_STATUS_STAT_DISABLED_Msk             (0x1UL << DMA_CH_STATUS_STAT_DISABLED_Pos)                /*!< 0x00040000UL*/
#define DMA_CH_STATUS_STAT_DISABLED                 DMA_CH_STATUS_STAT_DISABLED_Msk                           /*!< STAT_DISABLED bit Disabled Status Flag. This flag will be set to HIGH if the DMA channel is successfully disabled using the DISABLECMD command. Write '1' to this bit to clear it. Automatically cleared when the ENABLECMD is set.*/
#define DMA_CH_STATUS_STAT_STOPPED_Pos              (19U)
#define DMA_CH_STATUS_STAT_STOPPED_Msk              (0x1UL << DMA_CH_STATUS_STAT_STOPPED_Pos)                 /*!< 0x00080000UL*/
#define DMA_CH_STATUS_STAT_STOPPED                  DMA_CH_STATUS_STAT_STOPPED_Msk                            /*!< STAT_STOPPED bit Stopped Status Flag. This flag will be set to HIGH if the DMA channel successfully reached the stopped state. The stop request can come from many internal or external sources. Write '1' to this bit to clear it. Automatically cleared when the ENABLECMD is set.*/
#define DMA_CH_STATUS_STAT_PAUSED_Pos               (20U)
#define DMA_CH_STATUS_STAT_PAUSED_Msk               (0x1UL << DMA_CH_STATUS_STAT_PAUSED_Pos)                  /*!< 0x00100000UL*/
#define DMA_CH_STATUS_STAT_PAUSED                   DMA_CH_STATUS_STAT_PAUSED_Msk                             /*!< STAT_PAUSED bit Paused Status Flag. This flag will be set to HIGH if the DMA channel successfully paused the operation of the command. The pause request can come from many internal or external sources. When the request to pause is not asserted anymore the bit will be cleared automatically and the command operation can continue.*/
#define DMA_CH_STATUS_STAT_RESUMEWAIT_Pos           (21U)
#define DMA_CH_STATUS_STAT_RESUMEWAIT_Msk           (0x1UL << DMA_CH_STATUS_STAT_RESUMEWAIT_Pos)              /*!< 0x00200000UL*/
#define DMA_CH_STATUS_STAT_RESUMEWAIT               DMA_CH_STATUS_STAT_RESUMEWAIT_Msk                         /*!< STAT_RESUMEWAIT bit Waiting for resume from software Flag. This flag indicates that the DMA channel successfully paused the operation of the command and needs SW acknowledgment to resume the operation. Will be set to HIGH if STAT_PAUSED is asserted and the PAUSECMD bit set in the command register or when the STAT_DONE is asserted and the DONEPAUSEEN bit is set. Cleared when the RESUMECMD bit is set in the command register.*/
#define DMA_CH_STATUS_STAT_SRCTRIGINWAIT_Pos        (24U)
#define DMA_CH_STATUS_STAT_SRCTRIGINWAIT_Msk        (0x1UL << DMA_CH_STATUS_STAT_SRCTRIGINWAIT_Pos)           /*!< 0x01000000UL*/
#define DMA_CH_STATUS_STAT_SRCTRIGINWAIT            DMA_CH_STATUS_STAT_SRCTRIGINWAIT_Msk                      /*!< STAT_SRCTRIGINWAIT bit Channel is waiting for Source Trigger Status. This bit is set to HIGH when DMA channel starts waiting for source input trigger request. Automatically cleared when the source trigger request is received either from HW or SW source. */
#define DMA_CH_STATUS_STAT_DESTRIGINWAIT_Pos        (25U)
#define DMA_CH_STATUS_STAT_DESTRIGINWAIT_Msk        (0x1UL << DMA_CH_STATUS_STAT_DESTRIGINWAIT_Pos)           /*!< 0x02000000UL*/
#define DMA_CH_STATUS_STAT_DESTRIGINWAIT            DMA_CH_STATUS_STAT_DESTRIGINWAIT_Msk                      /*!< STAT_DESTRIGINWAIT bit Channel is waiting for Destination Trigger Status. This bit is set to HIGH when DMA channel starts waiting for destination input trigger request. Automatically cleared when the destination trigger request is received either from HW or SW source.*/
#define DMA_CH_STATUS_STAT_TRIGOUTACKWAIT_Pos       (26U)
#define DMA_CH_STATUS_STAT_TRIGOUTACKWAIT_Msk       (0x1UL << DMA_CH_STATUS_STAT_TRIGOUTACKWAIT_Pos)          /*!< 0x04000000UL*/
#define DMA_CH_STATUS_STAT_TRIGOUTACKWAIT           DMA_CH_STATUS_STAT_TRIGOUTACKWAIT_Msk                     /*!< STAT_TRIGOUTACKWAIT bit Channel is waiting for output Trigger Acknowledgement Status. This bit is set to HIGH when DMA channel starts waiting for output trigger acknowledgement. Automatically cleared when the output trigger acknowledgement is received either from HW or SW source.*/
/*****************  Field definitions for CH_INTREN register  *****************/
#define DMA_CH_INTREN_INTREN_DONE_Pos               (0U)
#define DMA_CH_INTREN_INTREN_DONE_Msk               (0x1UL << DMA_CH_INTREN_INTREN_DONE_Pos)                  /*!< 0x00000001UL*/
#define DMA_CH_INTREN_INTREN_DONE                   DMA_CH_INTREN_INTREN_DONE_Msk                             /*!< INTREN_DONE bit Done Interrupt Enable. When set to HIGH, enables the INTR_DONE to be set and raise an interrupt when the STAT_DONE status flag is asserted. When set to LOW, it prevents INTR_DONE to be asserted. Currently pending interrupts are not affected by clearing this bit.*/
#define DMA_CH_INTREN_INTREN_ERR_Pos                (1U)
#define DMA_CH_INTREN_INTREN_ERR_Msk                (0x1UL << DMA_CH_INTREN_INTREN_ERR_Pos)                   /*!< 0x00000002UL*/
#define DMA_CH_INTREN_INTREN_ERR                    DMA_CH_INTREN_INTREN_ERR_Msk                              /*!< INTREN_ERR bit Error Interrupt Enable. When set to HIGH, enables INTR_ERROR to be set and raise an interrupt when the STAT_ERR status flag is asserted. When set to LOW, it prevents INTR_ERR to be asserted. Currently pending interrupts are not affected by clearing this bit.*/
#define DMA_CH_INTREN_INTREN_DISABLED_Pos           (2U)
#define DMA_CH_INTREN_INTREN_DISABLED_Msk           (0x1UL << DMA_CH_INTREN_INTREN_DISABLED_Pos)              /*!< 0x00000004UL*/
#define DMA_CH_INTREN_INTREN_DISABLED               DMA_CH_INTREN_INTREN_DISABLED_Msk                         /*!< INTREN_DISABLED bit Disabled Interrupt Enable. When set to HIGH, enables INTR_DISABLED to be set and raise an interrupt when STAT_DISABLED status flag is asserted. When set to LOW, it prevents INTR_DISABLED to be asserted. Currently pending interrupts are not affected by clearing this bit.*/
#define DMA_CH_INTREN_INTREN_STOPPED_Pos            (3U)
#define DMA_CH_INTREN_INTREN_STOPPED_Msk            (0x1UL << DMA_CH_INTREN_INTREN_STOPPED_Pos)               /*!< 0x00000008UL*/
#define DMA_CH_INTREN_INTREN_STOPPED                DMA_CH_INTREN_INTREN_STOPPED_Msk                          /*!< INTREN_STOPPED bit Stopped Interrupt Enable. When set to HIGH, enables INTR_STOPPED to be set and raise an interrupt when STAT_STOPPED status flag is asserted. When set to LOW, it prevents INTR_STOPPED to be asserted. Currently pending interrupts are not affected by clearing this bit.*/
#define DMA_CH_INTREN_INTREN_SRCTRIGINWAIT_Pos      (8U)
#define DMA_CH_INTREN_INTREN_SRCTRIGINWAIT_Msk      (0x1UL << DMA_CH_INTREN_INTREN_SRCTRIGINWAIT_Pos)         /*!< 0x00000100UL*/
#define DMA_CH_INTREN_INTREN_SRCTRIGINWAIT          DMA_CH_INTREN_INTREN_SRCTRIGINWAIT_Msk                    /*!< INTREN_SRCTRIGINWAIT bit Channel is waiting for Source Trigger Interrupt Enable. When set to HIGH, enables INTR_SRCTRIGINWAIT to be set and raise an interrupt when STAT_SRCTRIGINWAIT status flag is asserted. When set to LOW, it prevents INTR_SRCTRIGINWAIT to be asserted. Currently pending interrupts are not affected by clearing this bit.*/
#define DMA_CH_INTREN_INTREN_DESTRIGINWAIT_Pos      (9U)
#define DMA_CH_INTREN_INTREN_DESTRIGINWAIT_Msk      (0x1UL << DMA_CH_INTREN_INTREN_DESTRIGINWAIT_Pos)         /*!< 0x00000200UL*/
#define DMA_CH_INTREN_INTREN_DESTRIGINWAIT          DMA_CH_INTREN_INTREN_DESTRIGINWAIT_Msk                    /*!< INTREN_DESTRIGINWAIT bit Channel is waiting for destination Trigger Interrupt Enable. When set to HIGH, enables INTR_DESTRIGINWAIT to be set and raise an interrupt when STAT_DESTRIGINWAIT status flag is asserted. When set to LOW, it prevents INTR_DESTRIGINWAIT to be asserted. Currently pending interrupts are not affected by clearing this bit.*/
#define DMA_CH_INTREN_INTREN_TRIGOUTACKWAIT_Pos     (10U)
#define DMA_CH_INTREN_INTREN_TRIGOUTACKWAIT_Msk     (0x1UL << DMA_CH_INTREN_INTREN_TRIGOUTACKWAIT_Pos)        /*!< 0x00000400UL*/
#define DMA_CH_INTREN_INTREN_TRIGOUTACKWAIT         DMA_CH_INTREN_INTREN_TRIGOUTACKWAIT_Msk                   /*!< INTREN_TRIGOUTACKWAIT bit Channel is waiting for output Trigger Acknowledgement Interrupt Enable. When set to HIGH, enables INTR_TRIGOUTACKWAIT to be set and raise an interrupt when STAT_TRIGOUTACKWAIT status flag is asserted. When set to LOW, it prevents INTR_TRIGOUTACKWAIT to be asserted. Currently pending interrupts are not affected by clearing this bit.*/
/******************  Field definitions for CH_CTRL register  ******************/
#define DMA_CH_CTRL_TRANSIZE_Pos                    (0U)
#define DMA_CH_CTRL_TRANSIZE_Msk                    (0x7UL << DMA_CH_CTRL_TRANSIZE_Pos)                       /*!< 0x00000007UL*/
#define DMA_CH_CTRL_TRANSIZE                        DMA_CH_CTRL_TRANSIZE_Msk                                  /*!< TRANSIZE[ 2:0] bits Transfer Entity Size. Size in bytes = 2^TRANSIZE.
 - 000: Byte
 - 001: Halfworld
 - 010: Word
 - 011: Doubleword
 - 100: 128bits
 - 101: 256bits
 - 110: 512bits
 - 111: 1024bits
Note that DATA_WIDTH limits this field. Address will be aligned to TRANSIZE by the DMAC by ignoring the lower bits.*/
#define DMA_CH_CTRL_TRANSIZE_0                      (0x1UL << DMA_CH_CTRL_TRANSIZE_Pos)                       /*!< 0x00000001UL*/
#define DMA_CH_CTRL_TRANSIZE_1                      (0x2UL << DMA_CH_CTRL_TRANSIZE_Pos)                       /*!< 0x00000002UL*/
#define DMA_CH_CTRL_TRANSIZE_2                      (0x4UL << DMA_CH_CTRL_TRANSIZE_Pos)                       /*!< 0x00000004UL*/
#define DMA_CH_CTRL_CHPRIO_Pos                      (4U)
#define DMA_CH_CTRL_CHPRIO_Msk                      (0xFUL << DMA_CH_CTRL_CHPRIO_Pos)                         /*!< 0x000000F0UL*/
#define DMA_CH_CTRL_CHPRIO                          DMA_CH_CTRL_CHPRIO_Msk                                    /*!< CHPRIO[ 3:0] bits Channel Priority.
 - 0: Lowest Priority
 - 15: Highest Priority*/
#define DMA_CH_CTRL_CHPRIO_0                        (0x1UL << DMA_CH_CTRL_CHPRIO_Pos)                         /*!< 0x00000010UL*/
#define DMA_CH_CTRL_CHPRIO_1                        (0x2UL << DMA_CH_CTRL_CHPRIO_Pos)                         /*!< 0x00000020UL*/
#define DMA_CH_CTRL_CHPRIO_2                        (0x4UL << DMA_CH_CTRL_CHPRIO_Pos)                         /*!< 0x00000040UL*/
#define DMA_CH_CTRL_CHPRIO_3                        (0x8UL << DMA_CH_CTRL_CHPRIO_Pos)                         /*!< 0x00000080UL*/
#define DMA_CH_CTRL_XTYPE_Pos                       (9U)
#define DMA_CH_CTRL_XTYPE_Msk                       (0x7UL << DMA_CH_CTRL_XTYPE_Pos)                          /*!< 0x00000E00UL*/
#define DMA_CH_CTRL_XTYPE                           DMA_CH_CTRL_XTYPE_Msk                                     /*!< XTYPE[ 2:0] bits Operation type for X direction:
 - 000: "disable" - No data transfer will take place for this command. This mode can be used to create empty commands that wait for an event or set GPOs.
 - 001: "continue" - Copy data in a continuous manner from source to the destination. For 1D operations it is expected that SRCXSIZE is equal to DESXSIZE, other combinations result in UNPREDICTABLE behavior. For 2D operations, this mode can be used for simple 2D to 2D copy but it also allows the reshaping of the data like 1D to 2D or 2D to 1D conversions. If the DESXSIZE is smaller than SRCXSIZE then the read data from the current source line goes to the next destination line. If SRCXSIZE is smaller than DESXSIZE then the reads start on the next line and data is written to the remainder of the current destination line. Note: For 1D to 2D the SRCYSIZE for 2D to 1D conversion the DESYSIZE needs to be set to 1 when using this mode.
 - 010: "wrap" - Wrap source data within a destination line when the end of the source line is reached. Read starts again from the beginning of the source line and copied to the remainder of the destination line. If the DESXSIZE is smaller than SRCXSIZE then the behavior is UNPREDICTABLE. Not supported when HAS_WRAP is 0 in HW
 - 011: "fill" - Fill the remainder of the destination line with FILLVAL when the end of the source line is reached. If the DESXSIZE is smaller than SRCXSIZE then the behavior is UNPREDICTABLE. Not supported when HAS_WRAP is 0 in HW.
 - Others: Reserved.*/
#define DMA_CH_CTRL_XTYPE_0                         (0x1UL << DMA_CH_CTRL_XTYPE_Pos)                          /*!< 0x00000200UL*/
#define DMA_CH_CTRL_XTYPE_1                         (0x2UL << DMA_CH_CTRL_XTYPE_Pos)                          /*!< 0x00000400UL*/
#define DMA_CH_CTRL_XTYPE_2                         (0x4UL << DMA_CH_CTRL_XTYPE_Pos)                          /*!< 0x00000800UL*/
#define DMA_CH_CTRL_YTYPE_Pos                       (12U)
#define DMA_CH_CTRL_YTYPE_Msk                       (0x7UL << DMA_CH_CTRL_YTYPE_Pos)                          /*!< 0x00007000UL*/
#define DMA_CH_CTRL_YTYPE                           DMA_CH_CTRL_YTYPE_Msk                                     /*!< YTYPE[ 2:0] bits Operation type for Y direction:
 - 000: "disable" - Only do 1D transfers. When HAS_2D is 0, meaning 2D capability is not supported in HW, the YTYPE is always "000".
 - 001: "continue" - Copy 2D data in a continuous manner from source area to the destination area by using the YSIZE registers. The copy stops when the source runs out of data or the destination runs out of space. Not supported when HAS_2D is 0 in HW.
 - 010: "wrap" - Wrap the 2D source area within the destination 2D area by starting to copy data from the beginning of the first source line to the remaining space in the destination area. If the destination area is smaller than the source area then the behavior is UNPREDICTABLE. Not supported when HAS_WRAP or HAS_2D is 0 in HW.
 - 011:  Fill the remainder of the destination area with FILLVAL when the source area runs out of data. If the destination area is smaller than the source area then the behavior is UNPREDICTABLE. Not supported when HAS_WRAP or HAS_2D is 0 in HW
 - Others: Reserved*/
#define DMA_CH_CTRL_YTYPE_0                         (0x1UL << DMA_CH_CTRL_YTYPE_Pos)                          /*!< 0x00001000UL*/
#define DMA_CH_CTRL_YTYPE_1                         (0x2UL << DMA_CH_CTRL_YTYPE_Pos)                          /*!< 0x00002000UL*/
#define DMA_CH_CTRL_YTYPE_2                         (0x4UL << DMA_CH_CTRL_YTYPE_Pos)                          /*!< 0x00004000UL*/
#define DMA_CH_CTRL_REGRELOADTYPE_Pos               (18U)
#define DMA_CH_CTRL_REGRELOADTYPE_Msk               (0x7UL << DMA_CH_CTRL_REGRELOADTYPE_Pos)                  /*!< 0x001C0000UL*/
#define DMA_CH_CTRL_REGRELOADTYPE                   DMA_CH_CTRL_REGRELOADTYPE_Msk                             /*!< REGRELOADTYPE[ 2:0] bits Automatic register reload type. Defines how the DMA command reloads initial values at the end of a DMA command before autorestarting, ending or linking to a new DMA command:
 - 000: Reload Disabled.
 - 001: Reload source and destination size registers only.
 - 011: Reload source address only and all source and destination size registers.
 - 101: Reload destination address only and all source and destination size registers.
 - 111: Reload source and destination address and all source and destination size registers.
 - Others: Reserved.
NOTE: When CLEARCMD is set, the reloaded registers will also be cleared.*/
#define DMA_CH_CTRL_REGRELOADTYPE_0                 (0x1UL << DMA_CH_CTRL_REGRELOADTYPE_Pos)                  /*!< 0x00040000UL*/
#define DMA_CH_CTRL_REGRELOADTYPE_1                 (0x2UL << DMA_CH_CTRL_REGRELOADTYPE_Pos)                  /*!< 0x00080000UL*/
#define DMA_CH_CTRL_REGRELOADTYPE_2                 (0x4UL << DMA_CH_CTRL_REGRELOADTYPE_Pos)                  /*!< 0x00100000UL*/
#define DMA_CH_CTRL_DONETYPE_Pos                    (21U)
#define DMA_CH_CTRL_DONETYPE_Msk                    (0x7UL << DMA_CH_CTRL_DONETYPE_Pos)                       /*!< 0x00E00000UL*/
#define DMA_CH_CTRL_DONETYPE                        DMA_CH_CTRL_DONETYPE_Msk                                  /*!< DONETYPE[ 2:0] bits Done type selection. This field defines when the STAT_DONE status flag is asserted during the command operation.
 - 000: STAT_DONE flag is not asserted for this command.
 - 001: End of a command, before jumping to the next linked command. (default)
 - 011: End of an autorestart cycle, before starting the next cycle.
 - Others : Reserved.*/
#define DMA_CH_CTRL_DONETYPE_0                      (0x1UL << DMA_CH_CTRL_DONETYPE_Pos)                       /*!< 0x00200000UL*/
#define DMA_CH_CTRL_DONETYPE_1                      (0x2UL << DMA_CH_CTRL_DONETYPE_Pos)                       /*!< 0x00400000UL*/
#define DMA_CH_CTRL_DONETYPE_2                      (0x4UL << DMA_CH_CTRL_DONETYPE_Pos)                       /*!< 0x00800000UL*/
#define DMA_CH_CTRL_DONEPAUSEEN_Pos                 (24U)
#define DMA_CH_CTRL_DONEPAUSEEN_Msk                 (0x1UL << DMA_CH_CTRL_DONEPAUSEEN_Pos)                    /*!< 0x01000000UL*/
#define DMA_CH_CTRL_DONEPAUSEEN                     DMA_CH_CTRL_DONEPAUSEEN_Msk                               /*!< DONEPAUSEEN bit Done pause enable. When set to HIGH the assertion of the STAT_DONE flag results in an automatic pause request for the current DMA operation. When the paused state is reached the STAT_RESUMEWAIT flag is also set. When set to LOW the assertion of the STAT_DONE does not pause the progress of the command and the next operation of the channel will be started immediately after the STAT_DONE flag is set.*/
#define DMA_CH_CTRL_USESRCTRIGIN_Pos                (25U)
#define DMA_CH_CTRL_USESRCTRIGIN_Msk                (0x1UL << DMA_CH_CTRL_USESRCTRIGIN_Pos)                   /*!< 0x02000000UL*/
#define DMA_CH_CTRL_USESRCTRIGIN                    DMA_CH_CTRL_USESRCTRIGIN_Msk                              /*!< USESRCTRIGIN bit Enable Source Trigger Input use for this command.
 - 0: disable
 - 1: enable*/
#define DMA_CH_CTRL_USEDESTRIGIN_Pos                (26U)
#define DMA_CH_CTRL_USEDESTRIGIN_Msk                (0x1UL << DMA_CH_CTRL_USEDESTRIGIN_Pos)                   /*!< 0x04000000UL*/
#define DMA_CH_CTRL_USEDESTRIGIN                    DMA_CH_CTRL_USEDESTRIGIN_Msk                              /*!< USEDESTRIGIN bit Enable Destination Trigger Input use for this command.
 - 0: disable
 - 1: enable*/
#define DMA_CH_CTRL_USETRIGOUT_Pos                  (27U)
#define DMA_CH_CTRL_USETRIGOUT_Msk                  (0x1UL << DMA_CH_CTRL_USETRIGOUT_Pos)                     /*!< 0x08000000UL*/
#define DMA_CH_CTRL_USETRIGOUT                      DMA_CH_CTRL_USETRIGOUT_Msk                                /*!< USETRIGOUT bit Enable Trigger Output use for this command.
 - 0: disable
 - 1: enable*/
#define DMA_CH_CTRL_USEGPO_Pos                      (28U)
#define DMA_CH_CTRL_USEGPO_Msk                      (0x1UL << DMA_CH_CTRL_USEGPO_Pos)                         /*!< 0x10000000UL*/
#define DMA_CH_CTRL_USEGPO                          DMA_CH_CTRL_USEGPO_Msk                                    /*!< USEGPO bit Enable GPO use for this command.
 - 0: disable
 - 1: enable*/
#define DMA_CH_CTRL_USESTREAM_Pos                   (29U)
#define DMA_CH_CTRL_USESTREAM_Msk                   (0x1UL << DMA_CH_CTRL_USESTREAM_Pos)                      /*!< 0x20000000UL*/
#define DMA_CH_CTRL_USESTREAM                       DMA_CH_CTRL_USESTREAM_Msk                                 /*!< USESTREAM bit Enable Stream Interface use for this command.
 - 0: disable
 - 1: enable*/
/****************  Field definitions for CH_SRCADDR register  *****************/
#define DMA_CH_SRCADDR_SRCADDR_Pos                  (0U)
#define DMA_CH_SRCADDR_SRCADDR_Msk                  (0xFFFFFFFFUL << DMA_CH_SRCADDR_SRCADDR_Pos)              /*!< 0xFFFFFFFFUL*/
#define DMA_CH_SRCADDR_SRCADDR                      DMA_CH_SRCADDR_SRCADDR_Msk                                /*!< SRCADDR[31:0] bits Source Address [31:0].*/
/***************  Field definitions for CH_SRCADDRHI register  ****************/
#define DMA_CH_SRCADDRHI_SRCADDRHI_Pos              (0U)
#define DMA_CH_SRCADDRHI_SRCADDRHI_Msk              (0xFFFFFFFFUL << DMA_CH_SRCADDRHI_SRCADDRHI_Pos)          /*!< 0xFFFFFFFFUL*/
#define DMA_CH_SRCADDRHI_SRCADDRHI                  DMA_CH_SRCADDRHI_SRCADDRHI_Msk                            /*!< SRCADDRHI[31:0] bits Source Address [63:32]. Allows 64-bit addressing but the system might need less address bits defined by ADDR_WIDTH. The not implemented bits remain reserved.*/
/****************  Field definitions for CH_DESADDR register  *****************/
#define DMA_CH_DESADDR_DESADDR_Pos                  (0U)
#define DMA_CH_DESADDR_DESADDR_Msk                  (0xFFFFFFFFUL << DMA_CH_DESADDR_DESADDR_Pos)              /*!< 0xFFFFFFFFUL*/
#define DMA_CH_DESADDR_DESADDR                      DMA_CH_DESADDR_DESADDR_Msk                                /*!< DESADDR[31:0] bits Destination Address[31:0]*/
/***************  Field definitions for CH_DESADDRHI register  ****************/
#define DMA_CH_DESADDRHI_DESADDRHI_Pos              (0U)
#define DMA_CH_DESADDRHI_DESADDRHI_Msk              (0xFFFFFFFFUL << DMA_CH_DESADDRHI_DESADDRHI_Pos)          /*!< 0xFFFFFFFFUL*/
#define DMA_CH_DESADDRHI_DESADDRHI                  DMA_CH_DESADDRHI_DESADDRHI_Msk                            /*!< DESADDRHI[31:0] bits Destination Address[63:32]. Allows 64-bit addressing but the system might need less address bits defined by ADDR_WIDTH. The not implemented bits remain reserved.*/
/*****************  Field definitions for CH_XSIZE register  ******************/
#define DMA_CH_XSIZE_SRCXSIZE_Pos                   (0U)
#define DMA_CH_XSIZE_SRCXSIZE_Msk                   (0xFFFFUL << DMA_CH_XSIZE_SRCXSIZE_Pos)                   /*!< 0x0000FFFFUL*/
#define DMA_CH_XSIZE_SRCXSIZE                       DMA_CH_XSIZE_SRCXSIZE_Msk                                 /*!< SRCXSIZE[15:0] bits Source Number of Transfers in the X Dimension lower bits [15:0]. This register along with SRCXSIZEHI defines the source data block size of the DMA operation for any 1D operation, and defines the X dimension of the 2D source block for 2D operation.*/
#define DMA_CH_XSIZE_DESXSIZE_Pos                   (16U)
#define DMA_CH_XSIZE_DESXSIZE_Msk                   (0xFFFFUL << DMA_CH_XSIZE_DESXSIZE_Pos)                   /*!< 0xFFFF0000UL*/
#define DMA_CH_XSIZE_DESXSIZE                       DMA_CH_XSIZE_DESXSIZE_Msk                                 /*!< DESXSIZE[15:0] bits Destination Number of Transfers in the X Dimension lower bits [15:0]. This register along with DESXSIZEHI defines the destination data block size of the DMA operation for or any 1D operation, and defines the X dimension of the 2D destination block for a 2D operation. HAS_WRAP or HAS_STREAM configuration needs to be set to allow writes to this register, otherwise it is read-only and writing to SRCXSIZE will also update the value of this register.*/
/****************  Field definitions for CH_XSIZEHI register  *****************/
#define DMA_CH_XSIZEHI_SRCXSIZEHI_Pos               (0U)
#define DMA_CH_XSIZEHI_SRCXSIZEHI_Msk               (0xFFFFUL << DMA_CH_XSIZEHI_SRCXSIZEHI_Pos)               /*!< 0x0000FFFFUL*/
#define DMA_CH_XSIZEHI_SRCXSIZEHI                   DMA_CH_XSIZEHI_SRCXSIZEHI_Msk                             /*!< SRCXSIZEHI[15:0] bits Source Number of Transfers in the X Dimension high bits [31:16]. This register along with SRCXSIZE defines the source data block size of the DMA operation for any 1D operation, and defines the X dimension of the 2D source block for 2D operation.*/
#define DMA_CH_XSIZEHI_DESXSIZEHI_Pos               (16U)
#define DMA_CH_XSIZEHI_DESXSIZEHI_Msk               (0xFFFFUL << DMA_CH_XSIZEHI_DESXSIZEHI_Pos)               /*!< 0xFFFF0000UL*/
#define DMA_CH_XSIZEHI_DESXSIZEHI                   DMA_CH_XSIZEHI_DESXSIZEHI_Msk                             /*!< DESXSIZEHI[15:0] bits Destination Number of Transfers in the X Dimension high bits [31:16]. This register along with DESXSIZE defines the destination data block size of the DMA operation for or any 1D operation, and defines the X dimension of the 2D destination block for a 2D operation. HAS_WRAP or HAS_STREAM configuration needs to be set to allow writes to this register, otherwise it is read-only and writing to SRCXSIZEHI will also update the value of this register.*/
/**************  Field definitions for CH_SRCTRANSCFG register  ***************/
#define DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Pos         (0U)
#define DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Msk         (0xFUL << DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Pos)            /*!< 0x0000000FUL*/
#define DMA_CH_SRCTRANSCFG_SRCMEMATTRLO             DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Msk                       /*!< SRCMEMATTRLO[ 3:0] bits Source Transfer Memory Attribute field [3:0].
When SRCMEMATTRHI is Device type (0000) then this field means:
 - 0000: Device-nGnRnE
 - 0100: Device-nGnRE
 - 1000: Device-nGRE
 - 1100: Device-GRE
 - Others: Invalid resulting in UNPREDICTABLE behavior
When SRCMEMATTRHI is Normal memory type (other than 0000) then this field means:
 - 0000: Reserved
 - 0001: Normal memory, Inner Write allocate, Inner Write-through transient
 - 0010: Normal memory, Inner Read allocate, Inner Write-through transient
 - 0011: Normal memory, Inner Read/Write allocate, Inner Write-through transient
 - 0100: Normal memory, Inner non-cacheable
 - 0101: Normal memory, Inner Write allocate, Inner Write-back transient
 - 0110: Normal memory, Inner Read allocate, Inner Write-back transient
 - 0111: Normal memory, Inner Read/Write allocate, Inner Write-back transient
 - 1000: Normal memory, Inner Write-through non-transient
 - 1001: Normal memory, Inner Write allocate, Inner Write-through non-transient
 - 1010: Normal memory, Inner Read allocate, Inner Write-through non-transient
 - 1011: Normal memory, Inner Read/Write allocate, Inner Write-through non-transient
 - 1100: Normal memory, Inner Write-back non-transient
 - 1101: Normal memory, Inner Write allocate, Inner Write-back non-transient
 - 1110: Normal memory, Inner Read allocate, Inner Write-back non-transient
 - 1111: Normal memory, Inner Read/Write allocate, Inner Write-back non-transient*/
#define DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_0           (0x1UL << DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Pos)            /*!< 0x00000001UL*/
#define DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_1           (0x2UL << DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Pos)            /*!< 0x00000002UL*/
#define DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_2           (0x4UL << DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Pos)            /*!< 0x00000004UL*/
#define DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_3           (0x8UL << DMA_CH_SRCTRANSCFG_SRCMEMATTRLO_Pos)            /*!< 0x00000008UL*/
#define DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_Pos         (4U)
#define DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_Msk         (0xFUL << DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_Pos)            /*!< 0x000000F0UL*/
#define DMA_CH_SRCTRANSCFG_SRCMEMATTRHI             DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_Msk                       /*!< SRCMEMATTRHI[ 3:0] bits Source Transfer Memory Attribute field [7:4].
 - 0000: Device memory
 - 0001: Normal memory, Outer Write allocate, Outer Write-through transient
 - 0010: Normal memory, Outer Read allocate, Outer Write-through transient
 - 0011: Normal memory, Outer Read/Write allocate, Outer Write-through transient
 - 0100: Normal memory, Outer non-cacheable
 - 0101: Normal memory, Outer Write allocate, Outer Write-back transient
 - 0110: Normal memory, Outer Read allocate, Outer Write-back transient
 - 0111: Normal memory, Outer Read/Write allocate, Outer Write-back transient
 - 1000: Normal memory, Outer Write-through non-transient
 - 1001: Normal memory, Outer Write allocate, Outer Write-through non-transient
 - 1010: Normal memory, Outer Read allocate, Outer Write-through non-transient
 - 1011: Normal memory, Outer Read/Write allocate, Outer Write-through non-transient
 - 1100: Normal memory, Outer Write-back non-transient
 - 1101: Normal memory, Outer Write allocate, Outer Write-back non-transient
 - 1110: Normal memory, Outer Read allocate, Outer Write-back non-transient
 - 1111: Normal memory, Outer Read/Write allocate, Outer Write-back non-transient*/
#define DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_0           (0x1UL << DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_Pos)            /*!< 0x00000010UL*/
#define DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_1           (0x2UL << DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_Pos)            /*!< 0x00000020UL*/
#define DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_2           (0x4UL << DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_Pos)            /*!< 0x00000040UL*/
#define DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_3           (0x8UL << DMA_CH_SRCTRANSCFG_SRCMEMATTRHI_Pos)            /*!< 0x00000080UL*/
#define DMA_CH_SRCTRANSCFG_SRCSHAREATTR_Pos         (8U)
#define DMA_CH_SRCTRANSCFG_SRCSHAREATTR_Msk         (0x3UL << DMA_CH_SRCTRANSCFG_SRCSHAREATTR_Pos)            /*!< 0x00000300UL*/
#define DMA_CH_SRCTRANSCFG_SRCSHAREATTR             DMA_CH_SRCTRANSCFG_SRCSHAREATTR_Msk                       /*!< SRCSHAREATTR[ 1:0] bits Source Transfer Shareability Attribute.
 - 00: Non-shareable
 - 01: Reserved
 - 10: Outer shareable
 - 11: Inner shareable */
#define DMA_CH_SRCTRANSCFG_SRCSHAREATTR_0           (0x1UL << DMA_CH_SRCTRANSCFG_SRCSHAREATTR_Pos)            /*!< 0x00000100UL*/
#define DMA_CH_SRCTRANSCFG_SRCSHAREATTR_1           (0x2UL << DMA_CH_SRCTRANSCFG_SRCSHAREATTR_Pos)            /*!< 0x00000200UL*/
#define DMA_CH_SRCTRANSCFG_SRCNONSECATTR_Pos        (10U)
#define DMA_CH_SRCTRANSCFG_SRCNONSECATTR_Msk        (0x1UL << DMA_CH_SRCTRANSCFG_SRCNONSECATTR_Pos)           /*!< 0x00000400UL*/
#define DMA_CH_SRCTRANSCFG_SRCNONSECATTR            DMA_CH_SRCTRANSCFG_SRCNONSECATTR_Msk                      /*!< SRCNONSECATTR bit Source Transfer Non-secure Attribute.
 - 0: Secure
 - 1: Non-secure
 When a channel is Non-secure this bit is tied to 1. */
#define DMA_CH_SRCTRANSCFG_SRCPRIVATTR_Pos          (11U)
#define DMA_CH_SRCTRANSCFG_SRCPRIVATTR_Msk          (0x1UL << DMA_CH_SRCTRANSCFG_SRCPRIVATTR_Pos)             /*!< 0x00000800UL*/
#define DMA_CH_SRCTRANSCFG_SRCPRIVATTR              DMA_CH_SRCTRANSCFG_SRCPRIVATTR_Msk                        /*!< SRCPRIVATTR bit Source Transfer Privilege Attribute.
 - 0: Unprivileged
 - 1: Privileged
 When a channel is unprivileged this bit is tied to 0.*/
#define DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_Pos       (16U)
#define DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_Msk       (0xFUL << DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_Pos)          /*!< 0x000F0000UL*/
#define DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN           DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_Msk                     /*!< SRCMAXBURSTLEN[ 3:0] bits Source Max Burst Length. Hint for the DMA on what is the maximum allowed burst size it can use for read transfers. The maximum number of beats sent by the DMA for a read burst is equal to SRCMAXBURSTLEN + 1. Default value is 16 beats, which allows the DMA to set all burst sizes. Note: Limited by the DATA_BUFF_SIZE so larger settings may not always result in larger bursts.*/
#define DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_0         (0x1UL << DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_Pos)          /*!< 0x00010000UL*/
#define DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_1         (0x2UL << DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_Pos)          /*!< 0x00020000UL*/
#define DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_2         (0x4UL << DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_Pos)          /*!< 0x00040000UL*/
#define DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_3         (0x8UL << DMA_CH_SRCTRANSCFG_SRCMAXBURSTLEN_Pos)          /*!< 0x00080000UL*/
/**************  Field definitions for CH_DESTRANSCFG register  ***************/
#define DMA_CH_DESTRANSCFG_DESMEMATTRLO_Pos         (0U)
#define DMA_CH_DESTRANSCFG_DESMEMATTRLO_Msk         (0xFUL << DMA_CH_DESTRANSCFG_DESMEMATTRLO_Pos)            /*!< 0x0000000FUL*/
#define DMA_CH_DESTRANSCFG_DESMEMATTRLO             DMA_CH_DESTRANSCFG_DESMEMATTRLO_Msk                       /*!< DESMEMATTRLO[ 3:0] bits Destination Transfer Memory Attribute field [3:0].
When DESMEMATTRHI is Device type (0000) then this field means:
 - 0000: Device-nGnRnE
 - 0100: Device-nGnRE
 - 1000: Device-nGRE
 - 1100: Device-GRE
 - Others: Invalid resulting in UNPREDICTABLE behavior
When DESMEMATTRHI is Normal Memory type (other than 0000) then this field means:
 - 0000: Reserved
 - 0001: Normal memory, Inner Write allocate, Inner Write-through transient
 - 0010: Normal memory, Inner Read allocate, Inner Write-through transient
 - 0011: Normal memory, Inner Read/Write allocate, Inner Write-through transient
 - 0100: Normal memory, Inner non-cacheable
 - 0101: Normal memory, Inner Write allocate, Inner Write-back transient
 - 0110: Normal memory, Inner Read allocate, Inner Write-back transient
 - 0111: Normal memory, Inner Read/Write allocate, Inner Write-back transient
 - 1000: Normal memory, Inner Write-through non-transient
 - 1001: Normal memory, Inner Write allocate, Inner Write-through non-transient
 - 1010: Normal memory, Inner Read allocate, Inner Write-through non-transient
 - 1011: Normal memory, Inner Read/Write allocate, Inner Write-through non-transient
 - 1100: Normal memory, Inner Write-back non-transient
 - 1101: Normal memory, Inner Write allocate, Inner Write-back non-transient
 - 1110: Normal memory, Inner Read allocate, Inner Write-back non-transient
 - 1111: Normal memory, Inner Read/Write allocate, Inner Write-back non-transient*/
#define DMA_CH_DESTRANSCFG_DESMEMATTRLO_0           (0x1UL << DMA_CH_DESTRANSCFG_DESMEMATTRLO_Pos)            /*!< 0x00000001UL*/
#define DMA_CH_DESTRANSCFG_DESMEMATTRLO_1           (0x2UL << DMA_CH_DESTRANSCFG_DESMEMATTRLO_Pos)            /*!< 0x00000002UL*/
#define DMA_CH_DESTRANSCFG_DESMEMATTRLO_2           (0x4UL << DMA_CH_DESTRANSCFG_DESMEMATTRLO_Pos)            /*!< 0x00000004UL*/
#define DMA_CH_DESTRANSCFG_DESMEMATTRLO_3           (0x8UL << DMA_CH_DESTRANSCFG_DESMEMATTRLO_Pos)            /*!< 0x00000008UL*/
#define DMA_CH_DESTRANSCFG_DESMEMATTRHI_Pos         (4U)
#define DMA_CH_DESTRANSCFG_DESMEMATTRHI_Msk         (0xFUL << DMA_CH_DESTRANSCFG_DESMEMATTRHI_Pos)            /*!< 0x000000F0UL*/
#define DMA_CH_DESTRANSCFG_DESMEMATTRHI             DMA_CH_DESTRANSCFG_DESMEMATTRHI_Msk                       /*!< DESMEMATTRHI[ 3:0] bits Destination Transfer Memory Attribute field [7:4].
 - 0000: Device memory
 - 0001: Normal memory, Outer Write allocate, Outer Write-through transient
 - 0010: Normal memory, Outer Read allocate, Outer Write-through transient
 - 0011: Normal memory, Outer Read/Write allocate, Outer Write-through transient
 - 0100: Normal memory, Outer non-cacheable
 - 0101: Normal memory, Outer Write allocate, Outer Write-back transient
 - 0110: Normal memory, Outer Read allocate, Outer Write-back transient
 - 0111: Normal memory, Outer Read/Write allocate, Outer Write-back transient
 - 1000: Normal memory, Outer Write-through non-transient
 - 1001: Normal memory, Outer Write allocate, Outer Write-through non-transient
 - 1010: Normal memory, Outer Read allocate, Outer Write-through non-transient
 - 1011: Normal memory, Outer Read/Write allocate, Outer Write-through non-transient
 - 1100: Normal memory, Outer Write-back non-transient
 - 1101: Normal memory, Outer Write allocate, Outer Write-back non-transient
 - 1110: Normal memory, Outer Read allocate, Outer Write-back non-transient
 - 1111: Normal memory, Outer Read/Write allocate, Outer Write-back non-transient*/
#define DMA_CH_DESTRANSCFG_DESMEMATTRHI_0           (0x1UL << DMA_CH_DESTRANSCFG_DESMEMATTRHI_Pos)            /*!< 0x00000010UL*/
#define DMA_CH_DESTRANSCFG_DESMEMATTRHI_1           (0x2UL << DMA_CH_DESTRANSCFG_DESMEMATTRHI_Pos)            /*!< 0x00000020UL*/
#define DMA_CH_DESTRANSCFG_DESMEMATTRHI_2           (0x4UL << DMA_CH_DESTRANSCFG_DESMEMATTRHI_Pos)            /*!< 0x00000040UL*/
#define DMA_CH_DESTRANSCFG_DESMEMATTRHI_3           (0x8UL << DMA_CH_DESTRANSCFG_DESMEMATTRHI_Pos)            /*!< 0x00000080UL*/
#define DMA_CH_DESTRANSCFG_DESSHAREATTR_Pos         (8U)
#define DMA_CH_DESTRANSCFG_DESSHAREATTR_Msk         (0x3UL << DMA_CH_DESTRANSCFG_DESSHAREATTR_Pos)            /*!< 0x00000300UL*/
#define DMA_CH_DESTRANSCFG_DESSHAREATTR             DMA_CH_DESTRANSCFG_DESSHAREATTR_Msk                       /*!< DESSHAREATTR[ 1:0] bits Destination Transfer Shareability Attribute.
 - 00: Non-shareable
 - 01: Reserved
 - 10: Outer shareable
 - 11: Inner shareable*/
#define DMA_CH_DESTRANSCFG_DESSHAREATTR_0           (0x1UL << DMA_CH_DESTRANSCFG_DESSHAREATTR_Pos)            /*!< 0x00000100UL*/
#define DMA_CH_DESTRANSCFG_DESSHAREATTR_1           (0x2UL << DMA_CH_DESTRANSCFG_DESSHAREATTR_Pos)            /*!< 0x00000200UL*/
#define DMA_CH_DESTRANSCFG_DESNONSECATTR_Pos        (10U)
#define DMA_CH_DESTRANSCFG_DESNONSECATTR_Msk        (0x1UL << DMA_CH_DESTRANSCFG_DESNONSECATTR_Pos)           /*!< 0x00000400UL*/
#define DMA_CH_DESTRANSCFG_DESNONSECATTR            DMA_CH_DESTRANSCFG_DESNONSECATTR_Msk                      /*!< DESNONSECATTR bit Destination Transfer Non-secure Attribute.
 - 0: Secure
 - 1: Non-secure
 When a channel is Non-secure this bit is tied to 1. */
#define DMA_CH_DESTRANSCFG_DESPRIVATTR_Pos          (11U)
#define DMA_CH_DESTRANSCFG_DESPRIVATTR_Msk          (0x1UL << DMA_CH_DESTRANSCFG_DESPRIVATTR_Pos)             /*!< 0x00000800UL*/
#define DMA_CH_DESTRANSCFG_DESPRIVATTR              DMA_CH_DESTRANSCFG_DESPRIVATTR_Msk                        /*!< DESPRIVATTR bit Destination Transfer Privilege Attribute.
 - 0: Unprivileged
 - 1: Privileged
 When a channel is unprivileged this bit is tied to 0.*/
#define DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_Pos       (16U)
#define DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_Msk       (0xFUL << DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_Pos)          /*!< 0x000F0000UL*/
#define DMA_CH_DESTRANSCFG_DESMAXBURSTLEN           DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_Msk                     /*!< DESMAXBURSTLEN[ 3:0] bits Destination Max Burst Length. Hint for the DMA on what is the maximum allowed burst size it can use for write transfers. The maximum number of beats sent by the DMA for a write burst is equal to DESMAXBURSTLEN + 1. Default value is 16 beats, which allows the DMA to set all burst sizes. Note: Limited by the DATA_BUFF_SIZE so larger settings may not always result in larger bursts.*/
#define DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_0         (0x1UL << DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_Pos)          /*!< 0x00010000UL*/
#define DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_1         (0x2UL << DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_Pos)          /*!< 0x00020000UL*/
#define DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_2         (0x4UL << DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_Pos)          /*!< 0x00040000UL*/
#define DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_3         (0x8UL << DMA_CH_DESTRANSCFG_DESMAXBURSTLEN_Pos)          /*!< 0x00080000UL*/
/****************  Field definitions for CH_XADDRINC register  ****************/
#define DMA_CH_XADDRINC_SRCXADDRINC_Pos             (0U)
#define DMA_CH_XADDRINC_SRCXADDRINC_Msk             (0xFFFFUL << DMA_CH_XADDRINC_SRCXADDRINC_Pos)             /*!< 0x0000FFFFUL*/
#define DMA_CH_XADDRINC_SRCXADDRINC                 DMA_CH_XADDRINC_SRCXADDRINC_Msk                           /*!< SRCXADDRINC[15:0] bits Source X dimension Address Increment. This value is used as the increment between each TRANSIZE transfer. When a single bit is used then only 0 and 1 can be set. For wider increment registers, two's complement used with a range between -32768 to 32767 when the counter is 16-bits wide. The width of the register is indicated by the INC_WIDTH parameter. SRCADDR_next = SRCADDR + 2^TRANSIZE * SRCXADDRINC*/
#define DMA_CH_XADDRINC_DESXADDRINC_Pos             (16U)
#define DMA_CH_XADDRINC_DESXADDRINC_Msk             (0xFFFFUL << DMA_CH_XADDRINC_DESXADDRINC_Pos)             /*!< 0xFFFF0000UL*/
#define DMA_CH_XADDRINC_DESXADDRINC                 DMA_CH_XADDRINC_DESXADDRINC_Msk                           /*!< DESXADDRINC[15:0] bits Destination X dimension Address Increment. This value is used as the increment between each TRANSIZE transfer. When a single bit is used then only 0 and 1 can be set. For wider increment registers, two's complement used with a range between -32768 to 32767 when the counter is 16-bits wide. The width of the register is indicated by the INC_WIDTH parameter. DESADDR_next = DESADDR + 2^TRANSIZE * DESXADDRINC*/
/**************  Field definitions for CH_YADDRSTRIDE register  ***************/
#define DMA_CH_YADDRSTRIDE_SRCYADDRSTRIDE_Pos       (0U)
#define DMA_CH_YADDRSTRIDE_SRCYADDRSTRIDE_Msk       (0xFFFFUL << DMA_CH_YADDRSTRIDE_SRCYADDRSTRIDE_Pos)       /*!< 0x0000FFFFUL*/
#define DMA_CH_YADDRSTRIDE_SRCYADDRSTRIDE           DMA_CH_YADDRSTRIDE_SRCYADDRSTRIDE_Msk                     /*!< SRCYADDRSTRIDE[15:0] bits Source Address Stride between lines. Calculated in TRANSIZE aligned steps. This value is used to increment the SRCADDR after completing the transfer of a source line. SRCADDR_next_line_base = SRCADDR_line_base + 2^TRANSIZE * SRCYADDRSTRIDE. Two's complement used with a range between -32768 to 32767. When set to 0 the SRCADDR is not incremented after completing one line. Not present when HAS_2D is 0.*/
#define DMA_CH_YADDRSTRIDE_DESYADDRSTRIDE_Pos       (16U)
#define DMA_CH_YADDRSTRIDE_DESYADDRSTRIDE_Msk       (0xFFFFUL << DMA_CH_YADDRSTRIDE_DESYADDRSTRIDE_Pos)       /*!< 0xFFFF0000UL*/
#define DMA_CH_YADDRSTRIDE_DESYADDRSTRIDE           DMA_CH_YADDRSTRIDE_DESYADDRSTRIDE_Msk                     /*!< DESYADDRSTRIDE[15:0] bits Destination Address Stride between lines. Calculated in TRANSIZE aligned steps. This value is used to increment the DESADDR after completing the transfer of a destination line. DESADDR_next_line_base = DESADDR_line_base + 2^TRANSIZE * DESYADDRSTRIDE. Two's complement used with a range between -32768 to 32767. When set to 0 the DESADDR is not incremented after completing one line. Not present when HAS_2D is 0.*/
/****************  Field definitions for CH_FILLVAL register  *****************/
#define DMA_CH_FILLVAL_FILLVAL_Pos                  (0U)
#define DMA_CH_FILLVAL_FILLVAL_Msk                  (0xFFFFFFFFUL << DMA_CH_FILLVAL_FILLVAL_Pos)              /*!< 0xFFFFFFFFUL*/
#define DMA_CH_FILLVAL_FILLVAL                      DMA_CH_FILLVAL_FILLVAL_Msk                                /*!< FILLVAL[31:0] bits Fill pattern value. When XTYPE or YTYPE is set to fill mode, then this register value is used on the write data bus when the command starts to fill the memory area. The TRANSIZE defines the width of the FILLVAL used for the command. For byte transfers the FILLVAL[7:0] is used, other bits are ignored. For halfword transfers the FILLVAL[15:0] is used, other bits are ignored. For 64-bit and wider transfers the FILLVAL[31:0] pattern is repeated on the full width of the data bus. Not present when HAS_WRAP is 0.*/
/*****************  Field definitions for CH_YSIZE register  ******************/
#define DMA_CH_YSIZE_SRCYSIZE_Pos                   (0U)
#define DMA_CH_YSIZE_SRCYSIZE_Msk                   (0xFFFFUL << DMA_CH_YSIZE_SRCYSIZE_Pos)                   /*!< 0x0000FFFFUL*/
#define DMA_CH_YSIZE_SRCYSIZE                       DMA_CH_YSIZE_SRCYSIZE_Msk                                 /*!< SRCYSIZE[15:0] bits Source Y dimension or number of lines. Not present when HAS_2D is 0.*/
#define DMA_CH_YSIZE_DESYSIZE_Pos                   (16U)
#define DMA_CH_YSIZE_DESYSIZE_Msk                   (0xFFFFUL << DMA_CH_YSIZE_DESYSIZE_Pos)                   /*!< 0xFFFF0000UL*/
#define DMA_CH_YSIZE_DESYSIZE                       DMA_CH_YSIZE_DESYSIZE_Msk                                 /*!< DESYSIZE[15:0] bits Destination Y dimension or number of lines. Not present when HAS_2D is 0. HAS_WRAP or HAS_STREAM configuration needs to be set to allow writes to this register, otherwise it is read-only.*/
/****************  Field definitions for CH_TMPLTCFG register  ****************/
#define DMA_CH_TMPLTCFG_SRCTMPLTSIZE_Pos            (8U)
#define DMA_CH_TMPLTCFG_SRCTMPLTSIZE_Msk            (0x1FUL << DMA_CH_TMPLTCFG_SRCTMPLTSIZE_Pos)              /*!< 0x00001F00UL*/
#define DMA_CH_TMPLTCFG_SRCTMPLTSIZE                DMA_CH_TMPLTCFG_SRCTMPLTSIZE_Msk                          /*!< SRCTMPLTSIZE[ 4:0] bits Source Template Size in number of transfers plus one.
 - 0: Source template is disabled.
 - 1 to 31: Bits SRCTMPLT[SRCTMPLTSIZE:0] is used as the source template. Not present when HAS_TMPLT is 0.*/
#define DMA_CH_TMPLTCFG_SRCTMPLTSIZE_0              (0x1UL << DMA_CH_TMPLTCFG_SRCTMPLTSIZE_Pos)               /*!< 0x00000100UL*/
#define DMA_CH_TMPLTCFG_SRCTMPLTSIZE_1              (0x2UL << DMA_CH_TMPLTCFG_SRCTMPLTSIZE_Pos)               /*!< 0x00000200UL*/
#define DMA_CH_TMPLTCFG_SRCTMPLTSIZE_2              (0x4UL << DMA_CH_TMPLTCFG_SRCTMPLTSIZE_Pos)               /*!< 0x00000400UL*/
#define DMA_CH_TMPLTCFG_SRCTMPLTSIZE_3              (0x8UL << DMA_CH_TMPLTCFG_SRCTMPLTSIZE_Pos)               /*!< 0x00000800UL*/
#define DMA_CH_TMPLTCFG_SRCTMPLTSIZE_4              (0x10UL << DMA_CH_TMPLTCFG_SRCTMPLTSIZE_Pos)              /*!< 0x00001000UL*/
#define DMA_CH_TMPLTCFG_DESTMPLTSIZE_Pos            (16U)
#define DMA_CH_TMPLTCFG_DESTMPLTSIZE_Msk            (0x1FUL << DMA_CH_TMPLTCFG_DESTMPLTSIZE_Pos)              /*!< 0x001F0000UL*/
#define DMA_CH_TMPLTCFG_DESTMPLTSIZE                DMA_CH_TMPLTCFG_DESTMPLTSIZE_Msk                          /*!< DESTMPLTSIZE[ 4:0] bits Destination Template Size in number of transfers plus one.
 - 0: Destination template is disabled.
 - 1 to 31: DESTMPLT[DESTMPLTSIZE:0] is used as the destination template. Not present when HAS_TMPLT is 0.*/
#define DMA_CH_TMPLTCFG_DESTMPLTSIZE_0              (0x1UL << DMA_CH_TMPLTCFG_DESTMPLTSIZE_Pos)               /*!< 0x00010000UL*/
#define DMA_CH_TMPLTCFG_DESTMPLTSIZE_1              (0x2UL << DMA_CH_TMPLTCFG_DESTMPLTSIZE_Pos)               /*!< 0x00020000UL*/
#define DMA_CH_TMPLTCFG_DESTMPLTSIZE_2              (0x4UL << DMA_CH_TMPLTCFG_DESTMPLTSIZE_Pos)               /*!< 0x00040000UL*/
#define DMA_CH_TMPLTCFG_DESTMPLTSIZE_3              (0x8UL << DMA_CH_TMPLTCFG_DESTMPLTSIZE_Pos)               /*!< 0x00080000UL*/
#define DMA_CH_TMPLTCFG_DESTMPLTSIZE_4              (0x10UL << DMA_CH_TMPLTCFG_DESTMPLTSIZE_Pos)              /*!< 0x00100000UL*/
/****************  Field definitions for CH_SRCTMPLT register  ****************/
#define DMA_CH_SRCTMPLT_SRCTMPLTLSB_Pos             (0U)
#define DMA_CH_SRCTMPLT_SRCTMPLTLSB_Msk             (0x1UL << DMA_CH_SRCTMPLT_SRCTMPLTLSB_Pos)                /*!< 0x00000001UL*/
#define DMA_CH_SRCTMPLT_SRCTMPLTLSB                 DMA_CH_SRCTMPLT_SRCTMPLTLSB_Msk                           /*!< SRCTMPLTLSB bit Source Packing Template Least Significant Bit. This bit of the template is read only and always set to 1 as template patterns can only start from the base address of the transfer. Not present when HAS_TMPLT is 0.*/
#define DMA_CH_SRCTMPLT_SRCTMPLT_Pos                (1U)
#define DMA_CH_SRCTMPLT_SRCTMPLT_Msk                (0x7FFFFFFFUL << DMA_CH_SRCTMPLT_SRCTMPLT_Pos)            /*!< 0xFFFFFFFEUL*/
#define DMA_CH_SRCTMPLT_SRCTMPLT                    DMA_CH_SRCTMPLT_SRCTMPLT_Msk                              /*!< SRCTMPLT[30:0] bits Source Packing Template. Bit[0] is read only and always set to 1 as template patterns can only start from the base address of the transfer. Not present when HAS_TMPLT is 0.*/
/****************  Field definitions for CH_DESTMPLT register  ****************/
#define DMA_CH_DESTMPLT_DESTMPLTLSB_Pos             (0U)
#define DMA_CH_DESTMPLT_DESTMPLTLSB_Msk             (0x1UL << DMA_CH_DESTMPLT_DESTMPLTLSB_Pos)                /*!< 0x00000001UL*/
#define DMA_CH_DESTMPLT_DESTMPLTLSB                 DMA_CH_DESTMPLT_DESTMPLTLSB_Msk                           /*!< DESTMPLTLSB bit Destination Packing Template Least Significant Bit. This bit of the template is read only and always set to 1 as template patterns can only start from the base address of the transfer. Not present when HAS_TMPLT is 0.*/
#define DMA_CH_DESTMPLT_DESTMPLT_Pos                (1U)
#define DMA_CH_DESTMPLT_DESTMPLT_Msk                (0x7FFFFFFFUL << DMA_CH_DESTMPLT_DESTMPLT_Pos)            /*!< 0xFFFFFFFEUL*/
#define DMA_CH_DESTMPLT_DESTMPLT                    DMA_CH_DESTMPLT_DESTMPLT_Msk                              /*!< DESTMPLT[30:0] bits Destination Packing Template.  Bit[0] is read only and always set to 1 as template patterns can only start from the base address of the transfer. Not present when HAS_TMPLT is 0.*/
/**************  Field definitions for CH_SRCTRIGINCFG register  **************/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Pos        (0U)
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Msk        (0xFFUL << DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Pos)          /*!< 0x000000FFUL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL            DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Msk                      /*!< SRCTRIGINSEL[ 7:0] bits Source Trigger Input Select*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_0          (0x1UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Pos)           /*!< 0x00000001UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_1          (0x2UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Pos)           /*!< 0x00000002UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_2          (0x4UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Pos)           /*!< 0x00000004UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_3          (0x8UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Pos)           /*!< 0x00000008UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_4          (0x10UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Pos)          /*!< 0x00000010UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_5          (0x20UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Pos)          /*!< 0x00000020UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_6          (0x40UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Pos)          /*!< 0x00000040UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_7          (0x80UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINSEL_Pos)          /*!< 0x00000080UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_Pos       (8U)
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_Msk       (0x3UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_Pos)          /*!< 0x00000300UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE           DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_Msk                     /*!< SRCTRIGINTYPE[ 1:0] bits Source Trigger Input Type:
 - 00: Software only Trigger Request. SRCTRIGINSEL is ignored.
 - 01: Reserved
 - 10: HW Trigger Request. Only allowed when HAS_TRIGIN is enabled. SRCTRIGINSEL selects between external trigger inputs if HAS_TRIGSEL is enabled.
 - 11: Internal Trigger Request. Only allowed when HAS_TRIGSEL is enabled and the DMAC has multiple channels, otherwise treated as HW Trigger Request. SRCTRIGINSEL selects between DMA channels.
Note: SW triggers are also available when HW or Internal types are selected, but is is not recommended and caution must be taken when the these modes are combined.*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_0         (0x1UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_Pos)          /*!< 0x00000100UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_1         (0x2UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINTYPE_Pos)          /*!< 0x00000200UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_Pos       (10U)
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_Msk       (0x3UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_Pos)          /*!< 0x00000C00UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE           DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_Msk                     /*!< SRCTRIGINMODE[ 1:0] bits Source Trigger Input Mode:
 - 00: Command
 - 01: Reserved
 - 10: DMA driven Flow control. Only allowed when HAS_TRIGIN is enabled.
 - 11: Peripheral driven Flow control. Only allowed when HAS_TRIGIN is enabled.
Note: This field is ignored for Internal triggers as they only support Command triggers.*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_0         (0x1UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_Pos)          /*!< 0x00000400UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_1         (0x2UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINMODE_Pos)          /*!< 0x00000800UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Pos    (16U)
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Msk    (0xFFUL << DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Pos)      /*!< 0x00FF0000UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE        DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Msk                  /*!< SRCTRIGINBLKSIZE[ 7:0] bits Source Trigger Input Default Transfer Size. Defined transfer size per trigger + 1.*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_0      (0x1UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Pos)       /*!< 0x00010000UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_1      (0x2UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Pos)       /*!< 0x00020000UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_2      (0x4UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Pos)       /*!< 0x00040000UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_3      (0x8UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Pos)       /*!< 0x00080000UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_4      (0x10UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Pos)      /*!< 0x00100000UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_5      (0x20UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Pos)      /*!< 0x00200000UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_6      (0x40UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Pos)      /*!< 0x00400000UL*/
#define DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_7      (0x80UL << DMA_CH_SRCTRIGINCFG_SRCTRIGINBLKSIZE_Pos)      /*!< 0x00800000UL*/
/**************  Field definitions for CH_DESTRIGINCFG register  **************/
#define DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Pos        (0U)
#define DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Msk        (0xFFUL << DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Pos)          /*!< 0x000000FFUL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINSEL            DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Msk                      /*!< DESTRIGINSEL[ 7:0] bits Destination Trigger Input Select*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINSEL_0          (0x1UL << DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Pos)           /*!< 0x00000001UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINSEL_1          (0x2UL << DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Pos)           /*!< 0x00000002UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINSEL_2          (0x4UL << DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Pos)           /*!< 0x00000004UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINSEL_3          (0x8UL << DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Pos)           /*!< 0x00000008UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINSEL_4          (0x10UL << DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Pos)          /*!< 0x00000010UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINSEL_5          (0x20UL << DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Pos)          /*!< 0x00000020UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINSEL_6          (0x40UL << DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Pos)          /*!< 0x00000040UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINSEL_7          (0x80UL << DMA_CH_DESTRIGINCFG_DESTRIGINSEL_Pos)          /*!< 0x00000080UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_Pos       (8U)
#define DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_Msk       (0x3UL << DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_Pos)          /*!< 0x00000300UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINTYPE           DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_Msk                     /*!< DESTRIGINTYPE[ 1:0] bits Destination Trigger Input Type:
 - 00: Software only Trigger Request. DESTRIGINSEL is ignored.
 - 01: Reserved
 - 10: HW Trigger Request. Only allowed when HAS_TRIGIN is enabled. DESTRIGINSEL selects between external trigger inputs if HAS_TRIGSEL is enabled.
 - 11: Internal Trigger Request. Only allowed when HAS_TRIGSEL is enabled and the DMAC has multiple channels, otherwise treated as HW Trigger Request. DESTRIGINSEL selects between DMA channels.
Note: SW triggers are also available when HW or Internal types are selected, but is is not recommended and caution must be taken when the these modes are combined.*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_0         (0x1UL << DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_Pos)          /*!< 0x00000100UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_1         (0x2UL << DMA_CH_DESTRIGINCFG_DESTRIGINTYPE_Pos)          /*!< 0x00000200UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINMODE_Pos       (10U)
#define DMA_CH_DESTRIGINCFG_DESTRIGINMODE_Msk       (0x3UL << DMA_CH_DESTRIGINCFG_DESTRIGINMODE_Pos)          /*!< 0x00000C00UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINMODE           DMA_CH_DESTRIGINCFG_DESTRIGINMODE_Msk                     /*!< DESTRIGINMODE[ 1:0] bits Destination Trigger Input Mode:
 - 00: Command
 - 01: Reserved
 - 10: DMA driven Flow control. Only allowed when HAS_TRIGIN is enabled.
 - 11: Peripheral driven Flow control. Only allowed when HAS_TRIGIN is enabled.
Note: This field is ignored for Internal triggers as they only support Command triggers.*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINMODE_0         (0x1UL << DMA_CH_DESTRIGINCFG_DESTRIGINMODE_Pos)          /*!< 0x00000400UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINMODE_1         (0x2UL << DMA_CH_DESTRIGINCFG_DESTRIGINMODE_Pos)          /*!< 0x00000800UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Pos    (16U)
#define DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Msk    (0xFFUL << DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Pos)      /*!< 0x00FF0000UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE        DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Msk                  /*!< DESTRIGINBLKSIZE[ 7:0] bits Destination Trigger Input Default Transfer Size. Defined transfer size per trigger + 1.*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_0      (0x1UL << DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Pos)       /*!< 0x00010000UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_1      (0x2UL << DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Pos)       /*!< 0x00020000UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_2      (0x4UL << DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Pos)       /*!< 0x00040000UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_3      (0x8UL << DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Pos)       /*!< 0x00080000UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_4      (0x10UL << DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Pos)      /*!< 0x00100000UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_5      (0x20UL << DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Pos)      /*!< 0x00200000UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_6      (0x40UL << DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Pos)      /*!< 0x00400000UL*/
#define DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_7      (0x80UL << DMA_CH_DESTRIGINCFG_DESTRIGINBLKSIZE_Pos)      /*!< 0x00800000UL*/
/***************  Field definitions for CH_TRIGOUTCFG register  ***************/
#define DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Pos            (0U)
#define DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Msk            (0x3FUL << DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Pos)              /*!< 0x0000003FUL*/
#define DMA_CH_TRIGOUTCFG_TRIGOUTSEL                DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Msk                          /*!< TRIGOUTSEL[ 5:0] bits Trigger Output Select*/
#define DMA_CH_TRIGOUTCFG_TRIGOUTSEL_0              (0x1UL << DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Pos)               /*!< 0x00000001UL*/
#define DMA_CH_TRIGOUTCFG_TRIGOUTSEL_1              (0x2UL << DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Pos)               /*!< 0x00000002UL*/
#define DMA_CH_TRIGOUTCFG_TRIGOUTSEL_2              (0x4UL << DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Pos)               /*!< 0x00000004UL*/
#define DMA_CH_TRIGOUTCFG_TRIGOUTSEL_3              (0x8UL << DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Pos)               /*!< 0x00000008UL*/
#define DMA_CH_TRIGOUTCFG_TRIGOUTSEL_4              (0x10UL << DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Pos)              /*!< 0x00000010UL*/
#define DMA_CH_TRIGOUTCFG_TRIGOUTSEL_5              (0x20UL << DMA_CH_TRIGOUTCFG_TRIGOUTSEL_Pos)              /*!< 0x00000020UL*/
#define DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_Pos           (8U)
#define DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_Msk           (0x3UL << DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_Pos)              /*!< 0x00000300UL*/
#define DMA_CH_TRIGOUTCFG_TRIGOUTTYPE               DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_Msk                         /*!< TRIGOUTTYPE[ 1:0] bits Trigger Output Type
 - 00: Software only Trigger Acknowledgement.
 - 01: Reserved
 - 10: HW Trigger Acknowledgement. Only allowed when HAS_TRIGOUT is enabled.
 - 11: Internal Trigger Acknowledgement. Only allowed when HAS_TRIGSEL is enabled and the DMAC has multiple channels, otherwise treated as HW Trigger Acknowledgement.*/
#define DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_0             (0x1UL << DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_Pos)              /*!< 0x00000100UL*/
#define DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_1             (0x2UL << DMA_CH_TRIGOUTCFG_TRIGOUTTYPE_Pos)              /*!< 0x00000200UL*/
/*****************  Field definitions for CH_GPOEN0 register  *****************/
#define DMA_CH_GPOEN0_GPOEN0_Pos                    (0U)
#define DMA_CH_GPOEN0_GPOEN0_Msk                    (0xFFFFFFFFUL << DMA_CH_GPOEN0_GPOEN0_Pos)                /*!< 0xFFFFFFFFUL*/
#define DMA_CH_GPOEN0_GPOEN0                        DMA_CH_GPOEN0_GPOEN0_Msk                                  /*!< GPOEN0[31:0] bits Channel General Purpose Output (GPO) bit 0 to 31 enable mask. If bit n is '1', then GPO[n] is selected for driving by GPOVAL0[n]. If bit 'n' is '0', then GPO[n] keeps its previous value. Only [GPO_WIDTH-1:0] are implemented. All unimplemented bits are RAZWI.*/
/****************  Field definitions for CH_GPOVAL0 register  *****************/
#define DMA_CH_GPOVAL0_GPOVAL0_Pos                  (0U)
#define DMA_CH_GPOVAL0_GPOVAL0_Msk                  (0xFFFFFFFFUL << DMA_CH_GPOVAL0_GPOVAL0_Pos)              /*!< 0xFFFFFFFFUL*/
#define DMA_CH_GPOVAL0_GPOVAL0                      DMA_CH_GPOVAL0_GPOVAL0_Msk                                /*!< GPOVAL0[31:0] bits General Purpose Output Value GPO[31:0]. Write to set output value. The actual value on the GPO port will become active when the command is enabled. Read returns the register value which might be different from the actual GPO port status. Only [GPO_WIDTH-1:0] are implemented. All unimplemented bits are RAZWI.*/
/**************  Field definitions for CH_STREAMINTCFG register  **************/
#define DMA_CH_STREAMINTCFG_STREAMTYPE_Pos          (9U)
#define DMA_CH_STREAMINTCFG_STREAMTYPE_Msk          (0x3UL << DMA_CH_STREAMINTCFG_STREAMTYPE_Pos)             /*!< 0x00000600UL*/
#define DMA_CH_STREAMINTCFG_STREAMTYPE              DMA_CH_STREAMINTCFG_STREAMTYPE_Msk                        /*!< STREAMTYPE[ 1:0] bits Stream Interface operation Type
 - 00: Stream in and out used.
 - 01: Stream out only
 - 10: Stream in only
 - 11: Reserved*/
#define DMA_CH_STREAMINTCFG_STREAMTYPE_0            (0x1UL << DMA_CH_STREAMINTCFG_STREAMTYPE_Pos)             /*!< 0x00000200UL*/
#define DMA_CH_STREAMINTCFG_STREAMTYPE_1            (0x2UL << DMA_CH_STREAMINTCFG_STREAMTYPE_Pos)             /*!< 0x00000400UL*/
/****************  Field definitions for CH_LINKATTR register  ****************/
#define DMA_CH_LINKATTR_LINKMEMATTRLO_Pos           (0U)
#define DMA_CH_LINKATTR_LINKMEMATTRLO_Msk           (0xFUL << DMA_CH_LINKATTR_LINKMEMATTRLO_Pos)              /*!< 0x0000000FUL*/
#define DMA_CH_LINKATTR_LINKMEMATTRLO               DMA_CH_LINKATTR_LINKMEMATTRLO_Msk                         /*!< LINKMEMATTRLO[ 3:0] bits Link Address Read Transfer Memory Attribute field [3:0].
When LINKMEMATTRHI is Device type (0000) then this field means:
 - 0000: Device-nGnRnE
 - 0100: Device-nGnRE
 - 1000: Device-nGRE
 - 1100: Device-GRE
 - Others: Invalid resulting in UNPREDICTABLE behavior
When LINKMEMATTRHI is Normal memory type (other than 0000) then this field means:
 - 0000: Reserved
 - 0001: Normal memory, Inner Write allocate, Inner Write-through transient
 - 0010: Normal memory, Inner Read allocate, Inner Write-through transient
 - 0011: Normal memory, Inner Read/Write allocate, Inner Write-through transient
 - 0100: Normal memory, Inner non-cacheable
 - 0101: Normal memory, Inner Write allocate, Inner Write-back transient
 - 0110: Normal memory, Inner Read allocate, Inner Write-back transient
 - 0111: Normal memory, Inner Read/Write allocate, Inner Write-back transient
 - 1000: Normal memory, Inner Write-through non-transient
 - 1001: Normal memory, Inner Write allocate, Inner Write-through non-transient
 - 1010: Normal memory, Inner Read allocate, Inner Write-through non-transient
 - 1011: Normal memory, Inner Read/Write allocate, Inner Write-through non-transient
 - 1100: Normal memory, Inner Write-back non-transient
 - 1101: Normal memory, Inner Write allocate, Inner Write-back non-transient
 - 1110: Normal memory, Inner Read allocate, Inner Write-back non-transient
 - 1111: Normal memory, Inner Read/Write allocate, Inner Write-back non-transient*/
#define DMA_CH_LINKATTR_LINKMEMATTRLO_0             (0x1UL << DMA_CH_LINKATTR_LINKMEMATTRLO_Pos)              /*!< 0x00000001UL*/
#define DMA_CH_LINKATTR_LINKMEMATTRLO_1             (0x2UL << DMA_CH_LINKATTR_LINKMEMATTRLO_Pos)              /*!< 0x00000002UL*/
#define DMA_CH_LINKATTR_LINKMEMATTRLO_2             (0x4UL << DMA_CH_LINKATTR_LINKMEMATTRLO_Pos)              /*!< 0x00000004UL*/
#define DMA_CH_LINKATTR_LINKMEMATTRLO_3             (0x8UL << DMA_CH_LINKATTR_LINKMEMATTRLO_Pos)              /*!< 0x00000008UL*/
#define DMA_CH_LINKATTR_LINKMEMATTRHI_Pos           (4U)
#define DMA_CH_LINKATTR_LINKMEMATTRHI_Msk           (0xFUL << DMA_CH_LINKATTR_LINKMEMATTRHI_Pos)              /*!< 0x000000F0UL*/
#define DMA_CH_LINKATTR_LINKMEMATTRHI               DMA_CH_LINKATTR_LINKMEMATTRHI_Msk                         /*!< LINKMEMATTRHI[ 3:0] bits Link Address Read Transfer Memory Attribute field [7:4].
 - 0000: Device memory
 - 0001: Normal memory, Outer Write allocate, Outer Write-through transient
 - 0010: Normal memory, Outer Read allocate, Outer Write-through transient
 - 0011: Normal memory, Outer Read/Write allocate, Outer Write-through transient
 - 0100: Normal memory, Outer non-cacheable
 - 0101: Normal memory, Outer Write allocate, Outer Write-back transient
 - 0110: Normal memory, Outer Read allocate, Outer Write-back transient
 - 0111: Normal memory, Outer Read/Write allocate, Outer Write-back transient
 - 1000: Normal memory, Outer Write-through non-transient
 - 1001: Normal memory, Outer Write allocate, Outer Write-through non-transient
 - 1010: Normal memory, Outer Read allocate, Outer Write-through non-transient
 - 1011: Normal memory, Outer Read/Write allocate, Outer Write-through non-transient
 - 1100: Normal memory, Outer Write-back non-transient
 - 1101: Normal memory, Outer Write allocate, Outer Write-back non-transient
 - 1110: Normal memory, Outer Read allocate, Outer Write-back non-transient
 - 1111: Normal memory, Outer Read/Write allocate, Outer Write-back non-transient*/
#define DMA_CH_LINKATTR_LINKMEMATTRHI_0             (0x1UL << DMA_CH_LINKATTR_LINKMEMATTRHI_Pos)              /*!< 0x00000010UL*/
#define DMA_CH_LINKATTR_LINKMEMATTRHI_1             (0x2UL << DMA_CH_LINKATTR_LINKMEMATTRHI_Pos)              /*!< 0x00000020UL*/
#define DMA_CH_LINKATTR_LINKMEMATTRHI_2             (0x4UL << DMA_CH_LINKATTR_LINKMEMATTRHI_Pos)              /*!< 0x00000040UL*/
#define DMA_CH_LINKATTR_LINKMEMATTRHI_3             (0x8UL << DMA_CH_LINKATTR_LINKMEMATTRHI_Pos)              /*!< 0x00000080UL*/
#define DMA_CH_LINKATTR_LINKSHAREATTR_Pos           (8U)
#define DMA_CH_LINKATTR_LINKSHAREATTR_Msk           (0x3UL << DMA_CH_LINKATTR_LINKSHAREATTR_Pos)              /*!< 0x00000300UL*/
#define DMA_CH_LINKATTR_LINKSHAREATTR               DMA_CH_LINKATTR_LINKSHAREATTR_Msk                         /*!< LINKSHAREATTR[ 1:0] bits Link Address Transfer Shareability Attribute.
 - 00: Non-shareable
 - 01: Reserved
 - 10: Outer shareable
 - 11: Inner shareable */
#define DMA_CH_LINKATTR_LINKSHAREATTR_0             (0x1UL << DMA_CH_LINKATTR_LINKSHAREATTR_Pos)              /*!< 0x00000100UL*/
#define DMA_CH_LINKATTR_LINKSHAREATTR_1             (0x2UL << DMA_CH_LINKATTR_LINKSHAREATTR_Pos)              /*!< 0x00000200UL*/
/****************  Field definitions for CH_AUTOCFG register  *****************/
#define DMA_CH_AUTOCFG_CMDRESTARTCNT_Pos            (0U)
#define DMA_CH_AUTOCFG_CMDRESTARTCNT_Msk            (0xFFFFUL << DMA_CH_AUTOCFG_CMDRESTARTCNT_Pos)            /*!< 0x0000FFFFUL*/
#define DMA_CH_AUTOCFG_CMDRESTARTCNT                DMA_CH_AUTOCFG_CMDRESTARTCNT_Msk                          /*!< CMDRESTARTCNT[15:0] bits Automatic Command Restart Counter. Defines the number of times automatic restarting will occur at end of DMA command. Auto restarting will occur after the command is completed, including output triggering if enabled and autoreloading the registers, but it will only perfrom a link to the next command when CMDRESTARTCNT == 0. When CMDRESTARTCNT and CMDRESTARTINF are both set to '0', autorestart is disabled.*/
#define DMA_CH_AUTOCFG_CMDRESTARTINFEN_Pos          (16U)
#define DMA_CH_AUTOCFG_CMDRESTARTINFEN_Msk          (0x1UL << DMA_CH_AUTOCFG_CMDRESTARTINFEN_Pos)             /*!< 0x00010000UL*/
#define DMA_CH_AUTOCFG_CMDRESTARTINFEN              DMA_CH_AUTOCFG_CMDRESTARTINFEN_Msk                        /*!< CMDRESTARTINFEN bit Enable Infinite Automatic Command Restart. When set, CMDRESTARTCNT is ignored and the command is always restarted after it is completed, including output triggering if enabled and autoreloading the registers but it will not perform a link to the next command. This means that the infinite loop of automatic restarts can only be broken by DISABLECMD or STOPCMD. When CMDRESTARTINFEN is set to '0', then the autorestarting of a command depends on CMDRESTARTCNT and when that counter is set to 0 the autorestarting is finished. In this case the next linked command is read or the command is complete.*/
/****************  Field definitions for CH_LINKADDR register  ****************/
#define DMA_CH_LINKADDR_LINKADDREN_Pos              (0U)
#define DMA_CH_LINKADDR_LINKADDREN_Msk              (0x1UL << DMA_CH_LINKADDR_LINKADDREN_Pos)                 /*!< 0x00000001UL*/
#define DMA_CH_LINKADDR_LINKADDREN                  DMA_CH_LINKADDR_LINKADDREN_Msk                            /*!< LINKADDREN bit Enable Link Address. When set to '1', the DMAC fetches the next command defined by LINKADDR. When set to '0' the DMAC will return to idle at the end of the current command.
NOTE: the linked command fetched by the DMAC needs to clear this field to mark the end of the command chain. Otherwise it may result in an infinite loop of the same command.
*/
#define DMA_CH_LINKADDR_LINKADDR_Pos                (2U)
#define DMA_CH_LINKADDR_LINKADDR_Msk                (0x3FFFFFFFUL << DMA_CH_LINKADDR_LINKADDR_Pos)            /*!< 0xFFFFFFFCUL*/
#define DMA_CH_LINKADDR_LINKADDR                    DMA_CH_LINKADDR_LINKADDR_Msk                              /*!< LINKADDR[29:0] bits Link Address Pointer [31:2]. The DMAC fetches the next command from this address if LINKADDREN is set.
NOTE: Commands are fetched with the security and privilege attribute of the channel and cannot be adjusted for the command link reads.
*/
/***************  Field definitions for CH_LINKADDRHI register  ***************/
#define DMA_CH_LINKADDRHI_LINKADDRHI_Pos            (0U)
#define DMA_CH_LINKADDRHI_LINKADDRHI_Msk            (0xFFFFFFFFUL << DMA_CH_LINKADDRHI_LINKADDRHI_Pos)        /*!< 0xFFFFFFFFUL*/
#define DMA_CH_LINKADDRHI_LINKADDRHI                DMA_CH_LINKADDRHI_LINKADDRHI_Msk                          /*!< LINKADDRHI[31:0] bits Link Address Pointer [63:32]. Allows 64-bit addressing but the system might need less address bits. Limited by ADDR_WIDTH and the not implemented bits remain reserved.*/
/****************  Field definitions for CH_GPOREAD0 register  ****************/
#define DMA_CH_GPOREAD0_GPOREAD0_Pos                (0U)
#define DMA_CH_GPOREAD0_GPOREAD0_Msk                (0xFFFFFFFFUL << DMA_CH_GPOREAD0_GPOREAD0_Pos)            /*!< 0xFFFFFFFFUL*/
#define DMA_CH_GPOREAD0_GPOREAD0                    DMA_CH_GPOREAD0_GPOREAD0_Msk                              /*!< GPOREAD0[31:0] bits General Purpose Output Read Value for GPO[31:0]. Read returns the actual value of the GPO ports. Only [GPO_WIDTH-1:0] are implemented. All unimplemented bits are set to 0.*/
/***************  Field definitions for CH_WRKREGPTR register  ****************/
#define DMA_CH_WRKREGPTR_WRKREGPTR_Pos              (0U)
#define DMA_CH_WRKREGPTR_WRKREGPTR_Msk              (0xFUL << DMA_CH_WRKREGPTR_WRKREGPTR_Pos)                 /*!< 0x0000000FUL*/
#define DMA_CH_WRKREGPTR_WRKREGPTR                  DMA_CH_WRKREGPTR_WRKREGPTR_Msk                            /*!< WRKREGPTR[ 3:0] bits Internal Working Register Pointer. These pointer refers to the following:
 - 0: Reserved
 - 1: SRCADDR_INITIAL
 - 2: SRCADDRHI_INITIAL
 - 3: DESADDR_INITIAL
 - 4: DESADDRHI_INITIAL
 - 5: SRCXSIZEHI_INITIAL, SRCXSIZE_INITIAL
 - 6: DESXSIZEHI_INITIAL, DESXSIZE_INITIAL
 - 7: SRCADDR_LINEINITIAL
 - 8: SRCADDRHI_LINEINITIAL
 - 9: DESADDR_LINEINITIAL
 - 10: DESADDRHI_LINEINITIAL
 - 11: SRCYSIZE_INITIAL (HAS_2D only)
 - 12: DESYSIZE_INITIAL (HAS_2D only)
 - Others: Reserved.
NOTE: When the selected register is not supported by the DMA then it is read as 0 in the CH_WRKREGVAL register. For 1D modes the INITIAL and LINEINITIAL registers contain the same values.*/
#define DMA_CH_WRKREGPTR_WRKREGPTR_0                (0x1UL << DMA_CH_WRKREGPTR_WRKREGPTR_Pos)                 /*!< 0x00000001UL*/
#define DMA_CH_WRKREGPTR_WRKREGPTR_1                (0x2UL << DMA_CH_WRKREGPTR_WRKREGPTR_Pos)                 /*!< 0x00000002UL*/
#define DMA_CH_WRKREGPTR_WRKREGPTR_2                (0x4UL << DMA_CH_WRKREGPTR_WRKREGPTR_Pos)                 /*!< 0x00000004UL*/
#define DMA_CH_WRKREGPTR_WRKREGPTR_3                (0x8UL << DMA_CH_WRKREGPTR_WRKREGPTR_Pos)                 /*!< 0x00000008UL*/
/***************  Field definitions for CH_WRKREGVAL register  ****************/
#define DMA_CH_WRKREGVAL_WRKREGVAL_Pos              (0U)
#define DMA_CH_WRKREGVAL_WRKREGVAL_Msk              (0xFFFFFFFFUL << DMA_CH_WRKREGVAL_WRKREGVAL_Pos)          /*!< 0xFFFFFFFFUL*/
#define DMA_CH_WRKREGVAL_WRKREGVAL                  DMA_CH_WRKREGVAL_WRKREGVAL_Msk                            /*!< WRKREGVAL[31:0] bits Internal Working Register Values. WRKREGPTR points to the register that is visible here. The values are only guaranteed to be stable for the current command when STAT_PAUSED is asserted, otherwise they depend on the current status of the channel which makes them UNPREDICTABLE.*/
/****************  Field definitions for CH_ERRINFO register  *****************/
#define DMA_CH_ERRINFO_BUSERR_Pos                   (0U)
#define DMA_CH_ERRINFO_BUSERR_Msk                   (0x1UL << DMA_CH_ERRINFO_BUSERR_Pos)                      /*!< 0x00000001UL*/
#define DMA_CH_ERRINFO_BUSERR                       DMA_CH_ERRINFO_BUSERR_Msk                                 /*!< BUSERR bit Bus Error Flag. Set when the DMA encounters a bus error during data or command read transfers.*/
#define DMA_CH_ERRINFO_CFGERR_Pos                   (1U)
#define DMA_CH_ERRINFO_CFGERR_Msk                   (0x1UL << DMA_CH_ERRINFO_CFGERR_Pos)                      /*!< 0x00000002UL*/
#define DMA_CH_ERRINFO_CFGERR                       DMA_CH_ERRINFO_CFGERR_Msk                                 /*!< CFGERR bit Configuration Error Flag. Set when the DMA command is enabled or a linked command is read but it is configured in a mode that is not supported by the implementation.*/
#define DMA_CH_ERRINFO_SRCTRIGINSELERR_Pos          (2U)
#define DMA_CH_ERRINFO_SRCTRIGINSELERR_Msk          (0x1UL << DMA_CH_ERRINFO_SRCTRIGINSELERR_Pos)             /*!< 0x00000004UL*/
#define DMA_CH_ERRINFO_SRCTRIGINSELERR              DMA_CH_ERRINFO_SRCTRIGINSELERR_Msk                        /*!< SRCTRIGINSELERR bit Source Trigger Input Selection Error Flag. Set when the command selects a source trigger input that is not allowed for this channel.*/
#define DMA_CH_ERRINFO_DESTRIGINSELERR_Pos          (3U)
#define DMA_CH_ERRINFO_DESTRIGINSELERR_Msk          (0x1UL << DMA_CH_ERRINFO_DESTRIGINSELERR_Pos)             /*!< 0x00000008UL*/
#define DMA_CH_ERRINFO_DESTRIGINSELERR              DMA_CH_ERRINFO_DESTRIGINSELERR_Msk                        /*!< DESTRIGINSELERR bit Destination Trigger Input Selection Error Flag. Set when the command selects a destination trigger input that is not allowed for this channel.*/
#define DMA_CH_ERRINFO_TRIGOUTSELERR_Pos            (4U)
#define DMA_CH_ERRINFO_TRIGOUTSELERR_Msk            (0x1UL << DMA_CH_ERRINFO_TRIGOUTSELERR_Pos)               /*!< 0x00000010UL*/
#define DMA_CH_ERRINFO_TRIGOUTSELERR                DMA_CH_ERRINFO_TRIGOUTSELERR_Msk                          /*!< TRIGOUTSELERR bit Trigger Output Selection Error Flag. Set when the command selects a trigger output that is not allowed for this channel.*/
#define DMA_CH_ERRINFO_STREAMERR_Pos                (7U)
#define DMA_CH_ERRINFO_STREAMERR_Msk                (0x1UL << DMA_CH_ERRINFO_STREAMERR_Pos)                   /*!< 0x00000080UL*/
#define DMA_CH_ERRINFO_STREAMERR                    DMA_CH_ERRINFO_STREAMERR_Msk                              /*!< STREAMERR bit Stream Interface Error Flag. Set when the stream interface encountered an error.*/
#define DMA_CH_ERRINFO_ERRINFO_Pos                  (16U)
#define DMA_CH_ERRINFO_ERRINFO_Msk                  (0xFFFFUL << DMA_CH_ERRINFO_ERRINFO_Pos)                  /*!< 0xFFFF0000UL*/
#define DMA_CH_ERRINFO_ERRINFO                      DMA_CH_ERRINFO_ERRINFO_Msk                                /*!< ERRINFO[15:0] bits Error information Register. Additional information for the error that is encountered by the DMA. The meaning of the bits are detailed in the Error Handling section.*/
/******************  Field definitions for CH_IIDR register  ******************/
#define DMA_CH_IIDR_IMPLEMENTER_Pos                 (0U)
#define DMA_CH_IIDR_IMPLEMENTER_Msk                 (0xFFFUL << DMA_CH_IIDR_IMPLEMENTER_Pos)                  /*!< 0x00000FFFUL*/
#define DMA_CH_IIDR_IMPLEMENTER                     DMA_CH_IIDR_IMPLEMENTER_Msk                               /*!< IMPLEMENTER[11:0] bits Contains the JEP106 code of the company that implemented the IP:
 - [11:8]: JEP106 continuation code of implementer.
 - [7]: Always 0.
 - [6:0]: JEP106 identity code of implementer.
For Arm this field reads as 0x43B.*/
#define DMA_CH_IIDR_IMPLEMENTER_0                   (0x1UL << DMA_CH_IIDR_IMPLEMENTER_Pos)                    /*!< 0x00000001UL*/
#define DMA_CH_IIDR_IMPLEMENTER_1                   (0x2UL << DMA_CH_IIDR_IMPLEMENTER_Pos)                    /*!< 0x00000002UL*/
#define DMA_CH_IIDR_IMPLEMENTER_2                   (0x4UL << DMA_CH_IIDR_IMPLEMENTER_Pos)                    /*!< 0x00000004UL*/
#define DMA_CH_IIDR_IMPLEMENTER_3                   (0x8UL << DMA_CH_IIDR_IMPLEMENTER_Pos)                    /*!< 0x00000008UL*/
#define DMA_CH_IIDR_IMPLEMENTER_4                   (0x10UL << DMA_CH_IIDR_IMPLEMENTER_Pos)                   /*!< 0x00000010UL*/
#define DMA_CH_IIDR_IMPLEMENTER_5                   (0x20UL << DMA_CH_IIDR_IMPLEMENTER_Pos)                   /*!< 0x00000020UL*/
#define DMA_CH_IIDR_IMPLEMENTER_6                   (0x40UL << DMA_CH_IIDR_IMPLEMENTER_Pos)                   /*!< 0x00000040UL*/
#define DMA_CH_IIDR_IMPLEMENTER_7                   (0x80UL << DMA_CH_IIDR_IMPLEMENTER_Pos)                   /*!< 0x00000080UL*/
#define DMA_CH_IIDR_IMPLEMENTER_8                   (0x100UL << DMA_CH_IIDR_IMPLEMENTER_Pos)                  /*!< 0x00000100UL*/
#define DMA_CH_IIDR_IMPLEMENTER_9                   (0x200UL << DMA_CH_IIDR_IMPLEMENTER_Pos)                  /*!< 0x00000200UL*/
#define DMA_CH_IIDR_IMPLEMENTER_10                  (0x400UL << DMA_CH_IIDR_IMPLEMENTER_Pos)                  /*!< 0x00000400UL*/
#define DMA_CH_IIDR_IMPLEMENTER_11                  (0x800UL << DMA_CH_IIDR_IMPLEMENTER_Pos)                  /*!< 0x00000800UL*/
#define DMA_CH_IIDR_REVISION_Pos                    (12U)
#define DMA_CH_IIDR_REVISION_Msk                    (0xFUL << DMA_CH_IIDR_REVISION_Pos)                       /*!< 0x0000F000UL*/
#define DMA_CH_IIDR_REVISION                        DMA_CH_IIDR_REVISION_Msk                                  /*!< REVISION[ 3:0] bits Indicates the minor revision of the product rxpy identifier*/
#define DMA_CH_IIDR_REVISION_0                      (0x1UL << DMA_CH_IIDR_REVISION_Pos)                       /*!< 0x00001000UL*/
#define DMA_CH_IIDR_REVISION_1                      (0x2UL << DMA_CH_IIDR_REVISION_Pos)                       /*!< 0x00002000UL*/
#define DMA_CH_IIDR_REVISION_2                      (0x4UL << DMA_CH_IIDR_REVISION_Pos)                       /*!< 0x00004000UL*/
#define DMA_CH_IIDR_REVISION_3                      (0x8UL << DMA_CH_IIDR_REVISION_Pos)                       /*!< 0x00008000UL*/
#define DMA_CH_IIDR_VARIANT_Pos                     (16U)
#define DMA_CH_IIDR_VARIANT_Msk                     (0xFUL << DMA_CH_IIDR_VARIANT_Pos)                        /*!< 0x000F0000UL*/
#define DMA_CH_IIDR_VARIANT                         DMA_CH_IIDR_VARIANT_Msk                                   /*!< VARIANT[ 3:0] bits Indicates the major revision, or variant, of the product rxpy identifier*/
#define DMA_CH_IIDR_VARIANT_0                       (0x1UL << DMA_CH_IIDR_VARIANT_Pos)                        /*!< 0x00010000UL*/
#define DMA_CH_IIDR_VARIANT_1                       (0x2UL << DMA_CH_IIDR_VARIANT_Pos)                        /*!< 0x00020000UL*/
#define DMA_CH_IIDR_VARIANT_2                       (0x4UL << DMA_CH_IIDR_VARIANT_Pos)                        /*!< 0x00040000UL*/
#define DMA_CH_IIDR_VARIANT_3                       (0x8UL << DMA_CH_IIDR_VARIANT_Pos)                        /*!< 0x00080000UL*/
#define DMA_CH_IIDR_PRODUCTID_Pos                   (20U)
#define DMA_CH_IIDR_PRODUCTID_Msk                   (0xFFFUL << DMA_CH_IIDR_PRODUCTID_Pos)                    /*!< 0xFFF00000UL*/
#define DMA_CH_IIDR_PRODUCTID                       DMA_CH_IIDR_PRODUCTID_Msk                                 /*!< PRODUCTID[11:0] bits Indicates the product ID*/
#define DMA_CH_IIDR_PRODUCTID_0                     (0x1UL << DMA_CH_IIDR_PRODUCTID_Pos)                      /*!< 0x00100000UL*/
#define DMA_CH_IIDR_PRODUCTID_1                     (0x2UL << DMA_CH_IIDR_PRODUCTID_Pos)                      /*!< 0x00200000UL*/
#define DMA_CH_IIDR_PRODUCTID_2                     (0x4UL << DMA_CH_IIDR_PRODUCTID_Pos)                      /*!< 0x00400000UL*/
#define DMA_CH_IIDR_PRODUCTID_3                     (0x8UL << DMA_CH_IIDR_PRODUCTID_Pos)                      /*!< 0x00800000UL*/
#define DMA_CH_IIDR_PRODUCTID_4                     (0x10UL << DMA_CH_IIDR_PRODUCTID_Pos)                     /*!< 0x01000000UL*/
#define DMA_CH_IIDR_PRODUCTID_5                     (0x20UL << DMA_CH_IIDR_PRODUCTID_Pos)                     /*!< 0x02000000UL*/
#define DMA_CH_IIDR_PRODUCTID_6                     (0x40UL << DMA_CH_IIDR_PRODUCTID_Pos)                     /*!< 0x04000000UL*/
#define DMA_CH_IIDR_PRODUCTID_7                     (0x80UL << DMA_CH_IIDR_PRODUCTID_Pos)                     /*!< 0x08000000UL*/
#define DMA_CH_IIDR_PRODUCTID_8                     (0x100UL << DMA_CH_IIDR_PRODUCTID_Pos)                    /*!< 0x10000000UL*/
#define DMA_CH_IIDR_PRODUCTID_9                     (0x200UL << DMA_CH_IIDR_PRODUCTID_Pos)                    /*!< 0x20000000UL*/
#define DMA_CH_IIDR_PRODUCTID_10                    (0x400UL << DMA_CH_IIDR_PRODUCTID_Pos)                    /*!< 0x40000000UL*/
#define DMA_CH_IIDR_PRODUCTID_11                    (0x800UL << DMA_CH_IIDR_PRODUCTID_Pos)                    /*!< 0x80000000UL*/
/******************  Field definitions for CH_AIDR register  ******************/
#define DMA_CH_AIDR_ARCH_MINOR_REV_Pos              (0U)
#define DMA_CH_AIDR_ARCH_MINOR_REV_Msk              (0xFUL << DMA_CH_AIDR_ARCH_MINOR_REV_Pos)                 /*!< 0x0000000FUL*/
#define DMA_CH_AIDR_ARCH_MINOR_REV                  DMA_CH_AIDR_ARCH_MINOR_REV_Msk                            /*!< ARCH_MINOR_REV[ 3:0] bits Architecture Minor Revision.*/
#define DMA_CH_AIDR_ARCH_MINOR_REV_0                (0x1UL << DMA_CH_AIDR_ARCH_MINOR_REV_Pos)                 /*!< 0x00000001UL*/
#define DMA_CH_AIDR_ARCH_MINOR_REV_1                (0x2UL << DMA_CH_AIDR_ARCH_MINOR_REV_Pos)                 /*!< 0x00000002UL*/
#define DMA_CH_AIDR_ARCH_MINOR_REV_2                (0x4UL << DMA_CH_AIDR_ARCH_MINOR_REV_Pos)                 /*!< 0x00000004UL*/
#define DMA_CH_AIDR_ARCH_MINOR_REV_3                (0x8UL << DMA_CH_AIDR_ARCH_MINOR_REV_Pos)                 /*!< 0x00000008UL*/
#define DMA_CH_AIDR_ARCH_MAJOR_REV_Pos              (4U)
#define DMA_CH_AIDR_ARCH_MAJOR_REV_Msk              (0xFUL << DMA_CH_AIDR_ARCH_MAJOR_REV_Pos)                 /*!< 0x000000F0UL*/
#define DMA_CH_AIDR_ARCH_MAJOR_REV                  DMA_CH_AIDR_ARCH_MAJOR_REV_Msk                            /*!< ARCH_MAJOR_REV[ 3:0] bits Architecture Major Revision.*/
#define DMA_CH_AIDR_ARCH_MAJOR_REV_0                (0x1UL << DMA_CH_AIDR_ARCH_MAJOR_REV_Pos)                 /*!< 0x00000010UL*/
#define DMA_CH_AIDR_ARCH_MAJOR_REV_1                (0x2UL << DMA_CH_AIDR_ARCH_MAJOR_REV_Pos)                 /*!< 0x00000020UL*/
#define DMA_CH_AIDR_ARCH_MAJOR_REV_2                (0x4UL << DMA_CH_AIDR_ARCH_MAJOR_REV_Pos)                 /*!< 0x00000040UL*/
#define DMA_CH_AIDR_ARCH_MAJOR_REV_3                (0x8UL << DMA_CH_AIDR_ARCH_MAJOR_REV_Pos)                 /*!< 0x00000080UL*/
/****************  Field definitions for CH_ISSUECAP register  ****************/
#define DMA_CH_ISSUECAP_ISSUECAP_Pos                (0U)
#define DMA_CH_ISSUECAP_ISSUECAP_Msk                (0x7UL << DMA_CH_ISSUECAP_ISSUECAP_Pos)                   /*!< 0x00000007UL*/
#define DMA_CH_ISSUECAP_ISSUECAP                    DMA_CH_ISSUECAP_ISSUECAP_Msk                              /*!< ISSUECAP[ 2:0] bits ISSUECAP can be used by software to place a constraint on allowed issuing capability in both read and write direction.*/
#define DMA_CH_ISSUECAP_ISSUECAP_0                  (0x1UL << DMA_CH_ISSUECAP_ISSUECAP_Pos)                   /*!< 0x00000001UL*/
#define DMA_CH_ISSUECAP_ISSUECAP_1                  (0x2UL << DMA_CH_ISSUECAP_ISSUECAP_Pos)                   /*!< 0x00000002UL*/
#define DMA_CH_ISSUECAP_ISSUECAP_2                  (0x4UL << DMA_CH_ISSUECAP_ISSUECAP_Pos)                   /*!< 0x00000004UL*/
/***************  Field definitions for CH_BUILDCFG0 register  ****************/
#define DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_Pos         (0U)
#define DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_Msk         (0xFFUL << DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_Pos)           /*!< 0x000000FFUL*/
#define DMA_CH_BUILDCFG0_DATA_BUFF_SIZE             DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_Msk                       /*!< DATA_BUFF_SIZE[ 7:0] bits Data Buffer Size in entries - 1. Total Size = ((DATA_BUFF_SIZE + 1) * DATA_WIDTH /8) bytes.*/
#define DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_0           (0x1UL << DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_Pos)            /*!< 0x00000001UL*/
#define DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_1           (0x2UL << DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_Pos)            /*!< 0x00000002UL*/
#define DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_2           (0x4UL << DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_Pos)            /*!< 0x00000004UL*/
#define DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_3           (0x8UL << DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_Pos)            /*!< 0x00000008UL*/
#define DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_4           (0x10UL << DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_Pos)           /*!< 0x00000010UL*/
#define DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_5           (0x20UL << DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_Pos)           /*!< 0x00000020UL*/
#define DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_6           (0x40UL << DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_Pos)           /*!< 0x00000040UL*/
#define DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_7           (0x80UL << DMA_CH_BUILDCFG0_DATA_BUFF_SIZE_Pos)           /*!< 0x00000080UL*/
#define DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_Pos          (8U)
#define DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_Msk          (0xFFUL << DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_Pos)            /*!< 0x0000FF00UL*/
#define DMA_CH_BUILDCFG0_CMD_BUFF_SIZE              DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_Msk                        /*!< CMD_BUFF_SIZE[ 7:0] bits Command Buffer Size in command words - 1. Total Size = (CMD_BUFF_SIZE + 1) * 4 bytes.*/
#define DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_0            (0x1UL << DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_Pos)             /*!< 0x00000100UL*/
#define DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_1            (0x2UL << DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_Pos)             /*!< 0x00000200UL*/
#define DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_2            (0x4UL << DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_Pos)             /*!< 0x00000400UL*/
#define DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_3            (0x8UL << DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_Pos)             /*!< 0x00000800UL*/
#define DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_4            (0x10UL << DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_Pos)            /*!< 0x00001000UL*/
#define DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_5            (0x20UL << DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_Pos)            /*!< 0x00002000UL*/
#define DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_6            (0x40UL << DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_Pos)            /*!< 0x00004000UL*/
#define DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_7            (0x80UL << DMA_CH_BUILDCFG0_CMD_BUFF_SIZE_Pos)            /*!< 0x00008000UL*/
#define DMA_CH_BUILDCFG0_ADDR_WIDTH_Pos             (16U)
#define DMA_CH_BUILDCFG0_ADDR_WIDTH_Msk             (0x3FUL << DMA_CH_BUILDCFG0_ADDR_WIDTH_Pos)               /*!< 0x003F0000UL*/
#define DMA_CH_BUILDCFG0_ADDR_WIDTH                 DMA_CH_BUILDCFG0_ADDR_WIDTH_Msk                           /*!< ADDR_WIDTH[ 5:0] bits Address Width in bits = ADDR_WIDTH + 1*/
#define DMA_CH_BUILDCFG0_ADDR_WIDTH_0               (0x1UL << DMA_CH_BUILDCFG0_ADDR_WIDTH_Pos)                /*!< 0x00010000UL*/
#define DMA_CH_BUILDCFG0_ADDR_WIDTH_1               (0x2UL << DMA_CH_BUILDCFG0_ADDR_WIDTH_Pos)                /*!< 0x00020000UL*/
#define DMA_CH_BUILDCFG0_ADDR_WIDTH_2               (0x4UL << DMA_CH_BUILDCFG0_ADDR_WIDTH_Pos)                /*!< 0x00040000UL*/
#define DMA_CH_BUILDCFG0_ADDR_WIDTH_3               (0x8UL << DMA_CH_BUILDCFG0_ADDR_WIDTH_Pos)                /*!< 0x00080000UL*/
#define DMA_CH_BUILDCFG0_ADDR_WIDTH_4               (0x10UL << DMA_CH_BUILDCFG0_ADDR_WIDTH_Pos)               /*!< 0x00100000UL*/
#define DMA_CH_BUILDCFG0_ADDR_WIDTH_5               (0x20UL << DMA_CH_BUILDCFG0_ADDR_WIDTH_Pos)               /*!< 0x00200000UL*/
#define DMA_CH_BUILDCFG0_DATA_WIDTH_Pos             (22U)
#define DMA_CH_BUILDCFG0_DATA_WIDTH_Msk             (0x7UL << DMA_CH_BUILDCFG0_DATA_WIDTH_Pos)                /*!< 0x01C00000UL*/
#define DMA_CH_BUILDCFG0_DATA_WIDTH                 DMA_CH_BUILDCFG0_DATA_WIDTH_Msk                           /*!< DATA_WIDTH[ 2:0] bits Data Width.
 - 000: 8-bit
 - 001: 16-bit
 - 010: 32-bit
 - 011: 64-bit
 - 100: 128-bit
 - 101: 256-bit
 - 110: 512-bit
 - 111: 1024-bit*/
#define DMA_CH_BUILDCFG0_DATA_WIDTH_0               (0x1UL << DMA_CH_BUILDCFG0_DATA_WIDTH_Pos)                /*!< 0x00400000UL*/
#define DMA_CH_BUILDCFG0_DATA_WIDTH_1               (0x2UL << DMA_CH_BUILDCFG0_DATA_WIDTH_Pos)                /*!< 0x00800000UL*/
#define DMA_CH_BUILDCFG0_DATA_WIDTH_2               (0x4UL << DMA_CH_BUILDCFG0_DATA_WIDTH_Pos)                /*!< 0x01000000UL*/
#define DMA_CH_BUILDCFG0_INC_WIDTH_Pos              (26U)
#define DMA_CH_BUILDCFG0_INC_WIDTH_Msk              (0xFUL << DMA_CH_BUILDCFG0_INC_WIDTH_Pos)                 /*!< 0x3C000000UL*/
#define DMA_CH_BUILDCFG0_INC_WIDTH                  DMA_CH_BUILDCFG0_INC_WIDTH_Msk                            /*!< INC_WIDTH[ 3:0] bits Width of the increment register = INC_WIDTH + 1.
When set to 0, then only 0 and 1 can be set as an increment. When larger values are used, then negative increments can be set using 2's complement:
INC_WIDTH = 0: 0..1
INC_WIDTH = 1: -2..1
INC_WIDTH = 2: -4..3
INC_WIDTH = 3: -8..7
…
INC_WIDTH = 15: -32768..32767*/
#define DMA_CH_BUILDCFG0_INC_WIDTH_0                (0x1UL << DMA_CH_BUILDCFG0_INC_WIDTH_Pos)                 /*!< 0x04000000UL*/
#define DMA_CH_BUILDCFG0_INC_WIDTH_1                (0x2UL << DMA_CH_BUILDCFG0_INC_WIDTH_Pos)                 /*!< 0x08000000UL*/
#define DMA_CH_BUILDCFG0_INC_WIDTH_2                (0x4UL << DMA_CH_BUILDCFG0_INC_WIDTH_Pos)                 /*!< 0x10000000UL*/
#define DMA_CH_BUILDCFG0_INC_WIDTH_3                (0x8UL << DMA_CH_BUILDCFG0_INC_WIDTH_Pos)                 /*!< 0x20000000UL*/
/***************  Field definitions for CH_BUILDCFG1 register  ****************/
#define DMA_CH_BUILDCFG1_HAS_XSIZEHI_Pos            (0U)
#define DMA_CH_BUILDCFG1_HAS_XSIZEHI_Msk            (0x1UL << DMA_CH_BUILDCFG1_HAS_XSIZEHI_Pos)               /*!< 0x00000001UL*/
#define DMA_CH_BUILDCFG1_HAS_XSIZEHI                DMA_CH_BUILDCFG1_HAS_XSIZEHI_Msk                          /*!< HAS_XSIZEHI bit Has 32-bit XSIZE counters.*/
#define DMA_CH_BUILDCFG1_HAS_WRAP_Pos               (1U)
#define DMA_CH_BUILDCFG1_HAS_WRAP_Msk               (0x1UL << DMA_CH_BUILDCFG1_HAS_WRAP_Pos)                  /*!< 0x00000002UL*/
#define DMA_CH_BUILDCFG1_HAS_WRAP                   DMA_CH_BUILDCFG1_HAS_WRAP_Msk                             /*!< HAS_WRAP bit Has wrap capability.*/
#define DMA_CH_BUILDCFG1_HAS_2D_Pos                 (2U)
#define DMA_CH_BUILDCFG1_HAS_2D_Msk                 (0x1UL << DMA_CH_BUILDCFG1_HAS_2D_Pos)                    /*!< 0x00000004UL*/
#define DMA_CH_BUILDCFG1_HAS_2D                     DMA_CH_BUILDCFG1_HAS_2D_Msk                               /*!< HAS_2D bit Has 2D capability.*/
#define DMA_CH_BUILDCFG1_HAS_TMPLT_Pos              (3U)
#define DMA_CH_BUILDCFG1_HAS_TMPLT_Msk              (0x1UL << DMA_CH_BUILDCFG1_HAS_TMPLT_Pos)                 /*!< 0x00000008UL*/
#define DMA_CH_BUILDCFG1_HAS_TMPLT                  DMA_CH_BUILDCFG1_HAS_TMPLT_Msk                            /*!< HAS_TMPLT bit Has template based pack and unpack capability.*/
#define DMA_CH_BUILDCFG1_HAS_TRIG_Pos               (4U)
#define DMA_CH_BUILDCFG1_HAS_TRIG_Msk               (0x1UL << DMA_CH_BUILDCFG1_HAS_TRIG_Pos)                  /*!< 0x00000010UL*/
#define DMA_CH_BUILDCFG1_HAS_TRIG                   DMA_CH_BUILDCFG1_HAS_TRIG_Msk                             /*!< HAS_TRIG bit Has trigger capability.*/
#define DMA_CH_BUILDCFG1_HAS_TRIGIN_Pos             (5U)
#define DMA_CH_BUILDCFG1_HAS_TRIGIN_Msk             (0x1UL << DMA_CH_BUILDCFG1_HAS_TRIGIN_Pos)                /*!< 0x00000020UL*/
#define DMA_CH_BUILDCFG1_HAS_TRIGIN                 DMA_CH_BUILDCFG1_HAS_TRIGIN_Msk                           /*!< HAS_TRIGIN bit Has hardware trigger input ports.*/
#define DMA_CH_BUILDCFG1_HAS_TRIGOUT_Pos            (6U)
#define DMA_CH_BUILDCFG1_HAS_TRIGOUT_Msk            (0x1UL << DMA_CH_BUILDCFG1_HAS_TRIGOUT_Pos)               /*!< 0x00000040UL*/
#define DMA_CH_BUILDCFG1_HAS_TRIGOUT                DMA_CH_BUILDCFG1_HAS_TRIGOUT_Msk                          /*!< HAS_TRIGOUT bit Has hardware trigger output port.*/
#define DMA_CH_BUILDCFG1_HAS_TRIGSEL_Pos            (7U)
#define DMA_CH_BUILDCFG1_HAS_TRIGSEL_Msk            (0x1UL << DMA_CH_BUILDCFG1_HAS_TRIGSEL_Pos)               /*!< 0x00000080UL*/
#define DMA_CH_BUILDCFG1_HAS_TRIGSEL                DMA_CH_BUILDCFG1_HAS_TRIGSEL_Msk                          /*!< HAS_TRIGSEL bit Has selectable triggers.*/
#define DMA_CH_BUILDCFG1_HAS_CMDLINK_Pos            (8U)
#define DMA_CH_BUILDCFG1_HAS_CMDLINK_Msk            (0x1UL << DMA_CH_BUILDCFG1_HAS_CMDLINK_Pos)               /*!< 0x00000100UL*/
#define DMA_CH_BUILDCFG1_HAS_CMDLINK                DMA_CH_BUILDCFG1_HAS_CMDLINK_Msk                          /*!< HAS_CMDLINK bit Has command link list capability.*/
#define DMA_CH_BUILDCFG1_HAS_AUTO_Pos               (9U)
#define DMA_CH_BUILDCFG1_HAS_AUTO_Msk               (0x1UL << DMA_CH_BUILDCFG1_HAS_AUTO_Pos)                  /*!< 0x00000200UL*/
#define DMA_CH_BUILDCFG1_HAS_AUTO                   DMA_CH_BUILDCFG1_HAS_AUTO_Msk                             /*!< HAS_AUTO bit Has automatic reload and restart capability.*/
#define DMA_CH_BUILDCFG1_HAS_WRKREG_Pos             (10U)
#define DMA_CH_BUILDCFG1_HAS_WRKREG_Msk             (0x1UL << DMA_CH_BUILDCFG1_HAS_WRKREG_Pos)                /*!< 0x00000400UL*/
#define DMA_CH_BUILDCFG1_HAS_WRKREG                 DMA_CH_BUILDCFG1_HAS_WRKREG_Msk                           /*!< HAS_WRKREG bit Has internal register view capability.*/
#define DMA_CH_BUILDCFG1_HAS_STREAM_Pos             (11U)
#define DMA_CH_BUILDCFG1_HAS_STREAM_Msk             (0x1UL << DMA_CH_BUILDCFG1_HAS_STREAM_Pos)                /*!< 0x00000800UL*/
#define DMA_CH_BUILDCFG1_HAS_STREAM                 DMA_CH_BUILDCFG1_HAS_STREAM_Msk                           /*!< HAS_STREAM bit Has stream interface support.*/
#define DMA_CH_BUILDCFG1_HAS_STREAMSEL_Pos          (12U)
#define DMA_CH_BUILDCFG1_HAS_STREAMSEL_Msk          (0x1UL << DMA_CH_BUILDCFG1_HAS_STREAMSEL_Pos)             /*!< 0x00001000UL*/
#define DMA_CH_BUILDCFG1_HAS_STREAMSEL              DMA_CH_BUILDCFG1_HAS_STREAMSEL_Msk                        /*!< HAS_STREAMSEL bit Has selectable stream interfaces.*/
#define DMA_CH_BUILDCFG1_HAS_GPOSEL_Pos             (18U)
#define DMA_CH_BUILDCFG1_HAS_GPOSEL_Msk             (0x1UL << DMA_CH_BUILDCFG1_HAS_GPOSEL_Pos)                /*!< 0x00040000UL*/
#define DMA_CH_BUILDCFG1_HAS_GPOSEL                 DMA_CH_BUILDCFG1_HAS_GPOSEL_Msk                           /*!< HAS_GPOSEL bit Has shared GPO ports between channels.*/
#define DMA_CH_BUILDCFG1_GPO_WIDTH_Pos              (19U)
#define DMA_CH_BUILDCFG1_GPO_WIDTH_Msk              (0x7FUL << DMA_CH_BUILDCFG1_GPO_WIDTH_Pos)                /*!< 0x03F80000UL*/
#define DMA_CH_BUILDCFG1_GPO_WIDTH                  DMA_CH_BUILDCFG1_GPO_WIDTH_Msk                            /*!< GPO_WIDTH[ 6:0] bits General Purpose Output Width. 0 to 64.*/
#define DMA_CH_BUILDCFG1_GPO_WIDTH_0                (0x1UL << DMA_CH_BUILDCFG1_GPO_WIDTH_Pos)                 /*!< 0x00080000UL*/
#define DMA_CH_BUILDCFG1_GPO_WIDTH_1                (0x2UL << DMA_CH_BUILDCFG1_GPO_WIDTH_Pos)                 /*!< 0x00100000UL*/
#define DMA_CH_BUILDCFG1_GPO_WIDTH_2                (0x4UL << DMA_CH_BUILDCFG1_GPO_WIDTH_Pos)                 /*!< 0x00200000UL*/
#define DMA_CH_BUILDCFG1_GPO_WIDTH_3                (0x8UL << DMA_CH_BUILDCFG1_GPO_WIDTH_Pos)                 /*!< 0x00400000UL*/
#define DMA_CH_BUILDCFG1_GPO_WIDTH_4                (0x10UL << DMA_CH_BUILDCFG1_GPO_WIDTH_Pos)                /*!< 0x00800000UL*/
#define DMA_CH_BUILDCFG1_GPO_WIDTH_5                (0x20UL << DMA_CH_BUILDCFG1_GPO_WIDTH_Pos)                /*!< 0x01000000UL*/
#define DMA_CH_BUILDCFG1_GPO_WIDTH_6                (0x40UL << DMA_CH_BUILDCFG1_GPO_WIDTH_Pos)                /*!< 0x02000000UL*/
/******************************************************************************/
/*                                DMANSECCTRL                                 */
/******************************************************************************/
/************  Field definitions for NSEC_CHINTRSTATUS0 register  *************/
#define DMA_NSEC_CHINTRSTATUS0_CHINTRSTATUS0_Pos    (0U)
#define DMA_NSEC_CHINTRSTATUS0_CHINTRSTATUS0_Msk    (0xFFFFFFFFUL << DMA_NSEC_CHINTRSTATUS0_CHINTRSTATUS0_Pos)/*!< 0xFFFFFFFFUL*/
#define DMA_NSEC_CHINTRSTATUS0_CHINTRSTATUS0        DMA_NSEC_CHINTRSTATUS0_CHINTRSTATUS0_Msk                  /*!< CHINTRSTATUS0[31:0] bits Collated Non-Secure Channel Interrupt flags for channel 0 to channel 31. Limited by NUM_CHANNELS, all unimplemented bits are reserved.*/
/****************  Field definitions for NSEC_STATUS register  ****************/
#define DMA_NSEC_STATUS_INTR_ANYCHINTR_Pos          (0U)
#define DMA_NSEC_STATUS_INTR_ANYCHINTR_Msk          (0x1UL << DMA_NSEC_STATUS_INTR_ANYCHINTR_Pos)             /*!< 0x00000001UL*/
#define DMA_NSEC_STATUS_INTR_ANYCHINTR              DMA_NSEC_STATUS_INTR_ANYCHINTR_Msk                        /*!< INTR_ANYCHINTR bit Combined Non-Secure Channel Interrupt Flag. Set to '1' when any Non-secure channel has an interrupt request in CHINTRSTATUS0 and NSECCTRL.INTREN_ANYCHINTR = 1. Cleared automatically when the source of the channel interrupt is cleared.*/
#define DMA_NSEC_STATUS_INTR_ALLCHIDLE_Pos          (1U)
#define DMA_NSEC_STATUS_INTR_ALLCHIDLE_Msk          (0x1UL << DMA_NSEC_STATUS_INTR_ALLCHIDLE_Pos)             /*!< 0x00000002UL*/
#define DMA_NSEC_STATUS_INTR_ALLCHIDLE              DMA_NSEC_STATUS_INTR_ALLCHIDLE_Msk                        /*!< INTR_ALLCHIDLE bit All Non-Secure Channel Idle Interrupt Status. Set to '1' when NSECCTRL.STAT_ALLCHIDLE is asserted and NSECCTRL.INTREN_ALLCHIDLE = 1. Cleared automatically when STAT_ALLCHIDLE is cleared.*/
#define DMA_NSEC_STATUS_INTR_ALLCHSTOPPED_Pos       (2U)
#define DMA_NSEC_STATUS_INTR_ALLCHSTOPPED_Msk       (0x1UL << DMA_NSEC_STATUS_INTR_ALLCHSTOPPED_Pos)          /*!< 0x00000004UL*/
#define DMA_NSEC_STATUS_INTR_ALLCHSTOPPED           DMA_NSEC_STATUS_INTR_ALLCHSTOPPED_Msk                     /*!< INTR_ALLCHSTOPPED bit All Non-Secure Channel Stopped Interrupt Status. Set to '1' when NSECCTRL.STAT_ALLCHSTOPPED is asserted and NSECCTRL.INTREN_ALLCHSTOPPED = 1. Cleared automatically when STAT_ALLCHSTOPPED is cleared.*/
#define DMA_NSEC_STATUS_INTR_ALLCHPAUSED_Pos        (3U)
#define DMA_NSEC_STATUS_INTR_ALLCHPAUSED_Msk        (0x1UL << DMA_NSEC_STATUS_INTR_ALLCHPAUSED_Pos)           /*!< 0x00000008UL*/
#define DMA_NSEC_STATUS_INTR_ALLCHPAUSED            DMA_NSEC_STATUS_INTR_ALLCHPAUSED_Msk                      /*!< INTR_ALLCHPAUSED bit All Non-Secure Channel Paused Interrupt Status. Set to '1' when NSECCTRL.STAT_ALLCHPAUSED is asserted and NSECCTRL.INTREN_ALLCHPAUSED = 1. Cleared automatically when STAT_ALLCHPAUSED is cleared.*/
#define DMA_NSEC_STATUS_STAT_ALLCHIDLE_Pos          (17U)
#define DMA_NSEC_STATUS_STAT_ALLCHIDLE_Msk          (0x1UL << DMA_NSEC_STATUS_STAT_ALLCHIDLE_Pos)             /*!< 0x00020000UL*/
#define DMA_NSEC_STATUS_STAT_ALLCHIDLE              DMA_NSEC_STATUS_STAT_ALLCHIDLE_Msk                        /*!< STAT_ALLCHIDLE bit All Non-Secure Channel Idle Status. Set to '1' whenever all Non-secure DMA channel is in idle state after at least one channel was running. Cleared by writing '1' to this bit.*/
#define DMA_NSEC_STATUS_STAT_ALLCHSTOPPED_Pos       (18U)
#define DMA_NSEC_STATUS_STAT_ALLCHSTOPPED_Msk       (0x1UL << DMA_NSEC_STATUS_STAT_ALLCHSTOPPED_Pos)          /*!< 0x00040000UL*/
#define DMA_NSEC_STATUS_STAT_ALLCHSTOPPED           DMA_NSEC_STATUS_STAT_ALLCHSTOPPED_Msk                     /*!< STAT_ALLCHSTOPPED bit All Non-Secure Channel Stopped Status. Set to '1' whenever all Non-secure DMA channels are in stopped or inactive state and the ALLCHSTOP request is active. Not set when channels reach the stopped state for other reasons. All Non-secure channels are forced to an immediate stopped state until ALLCHSTOP request is asserted even if they were enabled after the request. Cleared by writing '1' to this bit. The ALLCHSTOP request is also cleared when this bit is cleared.*/
#define DMA_NSEC_STATUS_STAT_ALLCHPAUSED_Pos        (19U)
#define DMA_NSEC_STATUS_STAT_ALLCHPAUSED_Msk        (0x1UL << DMA_NSEC_STATUS_STAT_ALLCHPAUSED_Pos)           /*!< 0x00080000UL*/
#define DMA_NSEC_STATUS_STAT_ALLCHPAUSED            DMA_NSEC_STATUS_STAT_ALLCHPAUSED_Msk                      /*!< STAT_ALLCHPAUSED bit All Non-Secure Channel Paused Status. Set to '1' whenever all Non-secure DMA channel is in paused or inactive state and the ALLCHPAUSE request is active. Not set when channels reach the paused state for other reasons. All Non-secure channels are forced to an immediate pause until ALLCHPAUSE request is asserted even if they were enabled after the request. Cleared by writing '1' to this bit which results in all Non-secure channels to resume their operation. The ALLCHPAUSE request is also cleared when this bit is cleared.*/
/*****************  Field definitions for NSEC_CTRL register  *****************/
#define DMA_NSEC_CTRL_INTREN_ANYCHINTR_Pos          (0U)
#define DMA_NSEC_CTRL_INTREN_ANYCHINTR_Msk          (0x1UL << DMA_NSEC_CTRL_INTREN_ANYCHINTR_Pos)             /*!< 0x00000001UL*/
#define DMA_NSEC_CTRL_INTREN_ANYCHINTR              DMA_NSEC_CTRL_INTREN_ANYCHINTR_Msk                        /*!< INTREN_ANYCHINTR bit Combined Non-Secure Channel Interrupt Enable*/
#define DMA_NSEC_CTRL_INTREN_ALLCHIDLE_Pos          (1U)
#define DMA_NSEC_CTRL_INTREN_ALLCHIDLE_Msk          (0x1UL << DMA_NSEC_CTRL_INTREN_ALLCHIDLE_Pos)             /*!< 0x00000002UL*/
#define DMA_NSEC_CTRL_INTREN_ALLCHIDLE              DMA_NSEC_CTRL_INTREN_ALLCHIDLE_Msk                        /*!< INTREN_ALLCHIDLE bit All Non-Secure Channel Idle Interrupt Enable*/
#define DMA_NSEC_CTRL_INTREN_ALLCHSTOPPED_Pos       (2U)
#define DMA_NSEC_CTRL_INTREN_ALLCHSTOPPED_Msk       (0x1UL << DMA_NSEC_CTRL_INTREN_ALLCHSTOPPED_Pos)          /*!< 0x00000004UL*/
#define DMA_NSEC_CTRL_INTREN_ALLCHSTOPPED           DMA_NSEC_CTRL_INTREN_ALLCHSTOPPED_Msk                     /*!< INTREN_ALLCHSTOPPED bit All Non-Secure Channels Stopped Interrupt Enable*/
#define DMA_NSEC_CTRL_INTREN_ALLCHPAUSED_Pos        (3U)
#define DMA_NSEC_CTRL_INTREN_ALLCHPAUSED_Msk        (0x1UL << DMA_NSEC_CTRL_INTREN_ALLCHPAUSED_Pos)           /*!< 0x00000008UL*/
#define DMA_NSEC_CTRL_INTREN_ALLCHPAUSED            DMA_NSEC_CTRL_INTREN_ALLCHPAUSED_Msk                      /*!< INTREN_ALLCHPAUSED bit All Non-Secure Channel Paused Interrupt Enable*/
#define DMA_NSEC_CTRL_ALLCHSTOP_Pos                 (8U)
#define DMA_NSEC_CTRL_ALLCHSTOP_Msk                 (0x1UL << DMA_NSEC_CTRL_ALLCHSTOP_Pos)                    /*!< 0x00000100UL*/
#define DMA_NSEC_CTRL_ALLCHSTOP                     DMA_NSEC_CTRL_ALLCHSTOP_Msk                               /*!< ALLCHSTOP bit Non-Secure All Channel Stop Request. When set to '1', all Non-secure channels get a stop request. Stays asserted until the STAT_ALLCHSTOPPED status flag is set. Cleared automatically when the STAT_ALLCHSTOPPED status flag is cleared.*/
#define DMA_NSEC_CTRL_ALLCHPAUSE_Pos                (9U)
#define DMA_NSEC_CTRL_ALLCHPAUSE_Msk                (0x1UL << DMA_NSEC_CTRL_ALLCHPAUSE_Pos)                   /*!< 0x00000200UL*/
#define DMA_NSEC_CTRL_ALLCHPAUSE                    DMA_NSEC_CTRL_ALLCHPAUSE_Msk                              /*!< ALLCHPAUSE bit Non-Secure All Channel Pause Request. When set to '1', all Non-secure channels get a pause request. Stays asserted until the channels are paused and the STAT_ALLCHPAUSED status flag is set. Cleared automatically when the STAT_ALLCHPAUSED status flag is cleared.*/
#define DMA_NSEC_CTRL_DBGHALTNSRO_Pos               (27U)
#define DMA_NSEC_CTRL_DBGHALTNSRO_Msk               (0x1UL << DMA_NSEC_CTRL_DBGHALTNSRO_Pos)                  /*!< 0x08000000UL*/
#define DMA_NSEC_CTRL_DBGHALTNSRO                   DMA_NSEC_CTRL_DBGHALTNSRO_Msk                             /*!< DBGHALTNSRO bit Debug Halt Enable Non-Secure Read Only. When set to '1', the NSEC_CTRL.DBGHALTEN register becomes read-only. When set to '0', NSEC_CTRL.DBGHALTEN has read-write access. This register is read-only for the non-secure SW.*/
#define DMA_NSEC_CTRL_DBGHALTEN_Pos                 (28U)
#define DMA_NSEC_CTRL_DBGHALTEN_Msk                 (0x1UL << DMA_NSEC_CTRL_DBGHALTEN_Pos)                    /*!< 0x10000000UL*/
#define DMA_NSEC_CTRL_DBGHALTEN                     DMA_NSEC_CTRL_DBGHALTEN_Msk                               /*!< DBGHALTEN bit Debug Halt Enabled. When set to '0', the DMA ignores the halt request from an external debugger. Clearing this bit while halt is ongoing results in continuing the operation. When set to '1', the debugger request to halt the DMA is allowed. This field is common for non-secure and secure side of the DMA, but access to this register is limited by the DBGHALTNSRO register field.*/
#define DMA_NSEC_CTRL_IDLERETEN_Pos                 (29U)
#define DMA_NSEC_CTRL_IDLERETEN_Msk                 (0x1UL << DMA_NSEC_CTRL_IDLERETEN_Pos)                    /*!< 0x20000000UL*/
#define DMA_NSEC_CTRL_IDLERETEN                     DMA_NSEC_CTRL_IDLERETEN_Msk                               /*!< IDLERETEN bit Idle Channel Retention Enable. Allows retention for non-secure channels that are enabled and waiting for an event in IDLE state.
 - 0: disabled
 - 1: enabled*/
#define DMA_NSEC_CTRL_DISMINPWR_Pos                 (30U)
#define DMA_NSEC_CTRL_DISMINPWR_Msk                 (0x3UL << DMA_NSEC_CTRL_DISMINPWR_Pos)                    /*!< 0xC0000000UL*/
#define DMA_NSEC_CTRL_DISMINPWR                     DMA_NSEC_CTRL_DISMINPWR_Msk                               /*!< DISMINPWR[ 1:0] bits Minimum Power state of the DMAC when at least one non-secure channel is present.
 - 00: OFF
 - 01: Retention
 - 10: ON
 - Others: Reserved*/
#define DMA_NSEC_CTRL_DISMINPWR_0                   (0x1UL << DMA_NSEC_CTRL_DISMINPWR_Pos)                    /*!< 0x40000000UL*/
#define DMA_NSEC_CTRL_DISMINPWR_1                   (0x2UL << DMA_NSEC_CTRL_DISMINPWR_Pos)                    /*!< 0x80000000UL*/
/****************  Field definitions for NSEC_CHPTR register  *****************/
#define DMA_NSEC_CHPTR_CHPTR_Pos                    (0U)
#define DMA_NSEC_CHPTR_CHPTR_Msk                    (0x3FUL << DMA_NSEC_CHPTR_CHPTR_Pos)                      /*!< 0x0000003FUL*/
#define DMA_NSEC_CHPTR_CHPTR                        DMA_NSEC_CHPTR_CHPTR_Msk                                  /*!< CHPTR[ 5:0] bits Non-Secure Channel Pointer. Selects which channel settings can be adjusted by the following registers.*/
#define DMA_NSEC_CHPTR_CHPTR_0                      (0x1UL << DMA_NSEC_CHPTR_CHPTR_Pos)                       /*!< 0x00000001UL*/
#define DMA_NSEC_CHPTR_CHPTR_1                      (0x2UL << DMA_NSEC_CHPTR_CHPTR_Pos)                       /*!< 0x00000002UL*/
#define DMA_NSEC_CHPTR_CHPTR_2                      (0x4UL << DMA_NSEC_CHPTR_CHPTR_Pos)                       /*!< 0x00000004UL*/
#define DMA_NSEC_CHPTR_CHPTR_3                      (0x8UL << DMA_NSEC_CHPTR_CHPTR_Pos)                       /*!< 0x00000008UL*/
#define DMA_NSEC_CHPTR_CHPTR_4                      (0x10UL << DMA_NSEC_CHPTR_CHPTR_Pos)                      /*!< 0x00000010UL*/
#define DMA_NSEC_CHPTR_CHPTR_5                      (0x20UL << DMA_NSEC_CHPTR_CHPTR_Pos)                      /*!< 0x00000020UL*/
/****************  Field definitions for NSEC_CHCFG register  *****************/
#define DMA_NSEC_CHCFG_CHID_Pos                     (0U)
#define DMA_NSEC_CHCFG_CHID_Msk                     (0xFFFFUL << DMA_NSEC_CHCFG_CHID_Pos)                     /*!< 0x0000FFFFUL*/
#define DMA_NSEC_CHCFG_CHID                         DMA_NSEC_CHCFG_CHID_Msk                                   /*!< CHID[15:0] bits Non-Secure Channel ID value. NSECCHPTR selects the channel ID value to be read or written via this register. CHID_WIDTH limits this field, unused bits are RAZWI.*/
#define DMA_NSEC_CHCFG_CHIDVLD_Pos                  (16U)
#define DMA_NSEC_CHCFG_CHIDVLD_Msk                  (0x1UL << DMA_NSEC_CHCFG_CHIDVLD_Pos)                     /*!< 0x00010000UL*/
#define DMA_NSEC_CHCFG_CHIDVLD                      DMA_NSEC_CHCFG_CHIDVLD_Msk                                /*!< CHIDVLD bit Non-Secure Channel ID valid. NSECCHPTR selects the channel. Set to '1' to drive the channel ID value in CHID for all the transfers by the selected channel.*/
#define DMA_NSEC_CHCFG_CHPRIV_Pos                   (17U)
#define DMA_NSEC_CHCFG_CHPRIV_Msk                   (0x1UL << DMA_NSEC_CHCFG_CHPRIV_Pos)                      /*!< 0x00020000UL*/
#define DMA_NSEC_CHCFG_CHPRIV                       DMA_NSEC_CHCFG_CHPRIV_Msk                                 /*!< CHPRIV bit Non-Secure Channel Privilege Enable. NSECCHPTR selects the channel. When set to '1' it allows the channel to send transfers marked as Privileged only. The configuration registers of the selected channel are also given privileged only access rights. When set to '0' the channel is only allowed to send unprivileged transfers and the channel registers can be accessed by both privileged and unprivileged register accesses.*/
/**************  Field definitions for NSEC_STATUSPTR register  ***************/
#define DMA_NSEC_STATUSPTR_NSECSTATUSPTR_Pos        (0U)
#define DMA_NSEC_STATUSPTR_NSECSTATUSPTR_Msk        (0xFUL << DMA_NSEC_STATUSPTR_NSECSTATUSPTR_Pos)           /*!< 0x0000000FUL*/
#define DMA_NSEC_STATUSPTR_NSECSTATUSPTR            DMA_NSEC_STATUSPTR_NSECSTATUSPTR_Msk                      /*!< NSECSTATUSPTR[ 3:0] bits Non-Secure DMA Unit status pointer used to select which status value to view using STATUSVALUE register. Pointer values are:
 - 0: Channel Enabled Status for channel numbers [31 : 0].
 - 1: Reserved
 - 2: Channel Stopped Status for channel numbers [31 : 0].
 - 3: Reserved
 - 4: Channel Paused Status for channel numbers [31 : 0].
 - Others: Reserved.*/
#define DMA_NSEC_STATUSPTR_NSECSTATUSPTR_0          (0x1UL << DMA_NSEC_STATUSPTR_NSECSTATUSPTR_Pos)           /*!< 0x00000001UL*/
#define DMA_NSEC_STATUSPTR_NSECSTATUSPTR_1          (0x2UL << DMA_NSEC_STATUSPTR_NSECSTATUSPTR_Pos)           /*!< 0x00000002UL*/
#define DMA_NSEC_STATUSPTR_NSECSTATUSPTR_2          (0x4UL << DMA_NSEC_STATUSPTR_NSECSTATUSPTR_Pos)           /*!< 0x00000004UL*/
#define DMA_NSEC_STATUSPTR_NSECSTATUSPTR_3          (0x8UL << DMA_NSEC_STATUSPTR_NSECSTATUSPTR_Pos)           /*!< 0x00000008UL*/
/**************  Field definitions for NSEC_STATUSVAL register  ***************/
#define DMA_NSEC_STATUSVAL_NSECSTATUSVAL_Pos        (0U)
#define DMA_NSEC_STATUSVAL_NSECSTATUSVAL_Msk        (0xFFFFFFFFUL << DMA_NSEC_STATUSVAL_NSECSTATUSVAL_Pos)    /*!< 0xFFFFFFFFUL*/
#define DMA_NSEC_STATUSVAL_NSECSTATUSVAL            DMA_NSEC_STATUSVAL_NSECSTATUSVAL_Msk                      /*!< NSECSTATUSVAL[31:0] bits Non-Secure DMA Unit status value. Can be used for reading internal status values of the DMA Unit for debug purposes. Values shown here are dependent on STATUSPTR. Note that inputs are masked by security mapping before being presented here. This means that only status values mapped to Non-secure world are visible as non-Zero values.
All unimplemented bits are RAZWI. */
/**************  Field definitions for NSEC_SIGNALPTR register  ***************/
#define DMA_NSEC_SIGNALPTR_NSECSIGNALPTR_Pos        (0U)
#define DMA_NSEC_SIGNALPTR_NSECSIGNALPTR_Msk        (0xFUL << DMA_NSEC_SIGNALPTR_NSECSIGNALPTR_Pos)           /*!< 0x0000000FUL*/
#define DMA_NSEC_SIGNALPTR_NSECSIGNALPTR            DMA_NSEC_SIGNALPTR_NSECSIGNALPTR_Msk                      /*!< NSECSIGNALPTR[ 3:0] bits Non-Secure DMA Unit signal pointer used to select which inputs or outputs to view using SIGNALVAL register. Pointer values, x,  are:
 - 0 to 7: Trigger Input Requests [31+32*x : 32*x]
 - 8 to 9: Trigger output Acknowledges [31+32*(x-8) :  32*(x-8)]
 - 10 to 11: GPO output value [31+32*(x-10) : 32*(x-10)], only present when HAS_GPOSEL is set
 - Others: Reserved*/
#define DMA_NSEC_SIGNALPTR_NSECSIGNALPTR_0          (0x1UL << DMA_NSEC_SIGNALPTR_NSECSIGNALPTR_Pos)           /*!< 0x00000001UL*/
#define DMA_NSEC_SIGNALPTR_NSECSIGNALPTR_1          (0x2UL << DMA_NSEC_SIGNALPTR_NSECSIGNALPTR_Pos)           /*!< 0x00000002UL*/
#define DMA_NSEC_SIGNALPTR_NSECSIGNALPTR_2          (0x4UL << DMA_NSEC_SIGNALPTR_NSECSIGNALPTR_Pos)           /*!< 0x00000004UL*/
#define DMA_NSEC_SIGNALPTR_NSECSIGNALPTR_3          (0x8UL << DMA_NSEC_SIGNALPTR_NSECSIGNALPTR_Pos)           /*!< 0x00000008UL*/
/**************  Field definitions for NSEC_SIGNALVAL register  ***************/
#define DMA_NSEC_SIGNALVAL_NSECSIGNALVAL_Pos        (0U)
#define DMA_NSEC_SIGNALVAL_NSECSIGNALVAL_Msk        (0xFFFFFFFFUL << DMA_NSEC_SIGNALVAL_NSECSIGNALVAL_Pos)    /*!< 0xFFFFFFFFUL*/
#define DMA_NSEC_SIGNALVAL_NSECSIGNALVAL            DMA_NSEC_SIGNALVAL_NSECSIGNALVAL_Msk                      /*!< NSECSIGNALVAL[31:0] bits Non-Secure DMA Unit signal status. Can be used for reading signal values of triggers and GPOs for debug. Values shown here are dependent on SIGNALPTR. Note that inputs are masked by security mapping before being presented here. This means that only signals mapped to Non-secure world are visible as non-zero values. Writing to this register has the following effect:
 - Writing '1' to a Non-secure Trigger Input Request that are not selected by any DMA channel will cause a deny response for it. This can attempt to clear an unwanted trigger input.
 - Writing '1' to any Trigger Output Acknowledges status, GPO status and all Trigger Input Request already selected by any DMA channel is ignored.
All unimplemented bits are RAZWI. */
/******************************************************************************/
/*                                 DMASECCTRL                                 */
/******************************************************************************/
/*************  Field definitions for SEC_CHINTRSTATUS0 register  *************/
#define DMA_SEC_CHINTRSTATUS0_CHINTRSTATUS0_Pos     (0U)
#define DMA_SEC_CHINTRSTATUS0_CHINTRSTATUS0_Msk     (0xFFFFFFFFUL << DMA_SEC_CHINTRSTATUS0_CHINTRSTATUS0_Pos) /*!< 0xFFFFFFFFUL*/
#define DMA_SEC_CHINTRSTATUS0_CHINTRSTATUS0         DMA_SEC_CHINTRSTATUS0_CHINTRSTATUS0_Msk                   /*!< CHINTRSTATUS0[31:0] bits Collated Secure Channel Interrupt flags for channel 0 to Channel 31. Limited by NUM_CHANNELS, all unimplemented bits are reserved.*/
/****************  Field definitions for SEC_STATUS register  *****************/
#define DMA_SEC_STATUS_INTR_ANYCHINTR_Pos           (0U)
#define DMA_SEC_STATUS_INTR_ANYCHINTR_Msk           (0x1UL << DMA_SEC_STATUS_INTR_ANYCHINTR_Pos)              /*!< 0x00000001UL*/
#define DMA_SEC_STATUS_INTR_ANYCHINTR               DMA_SEC_STATUS_INTR_ANYCHINTR_Msk                         /*!< INTR_ANYCHINTR bit Combined Secure Channel Interrupt Flag.  Set to '1' when any Secure channel has an interrupt request in CHINTRSTATUS0 and SECCTRL.INTREN_ANYCHINTR = 1. Cleared automatically when the source of the channel interrupt is cleared.*/
#define DMA_SEC_STATUS_INTR_ALLCHIDLE_Pos           (1U)
#define DMA_SEC_STATUS_INTR_ALLCHIDLE_Msk           (0x1UL << DMA_SEC_STATUS_INTR_ALLCHIDLE_Pos)              /*!< 0x00000002UL*/
#define DMA_SEC_STATUS_INTR_ALLCHIDLE               DMA_SEC_STATUS_INTR_ALLCHIDLE_Msk                         /*!< INTR_ALLCHIDLE bit All Secure Channel Idle Interrupt Status. Set to '1' when SECCTRL.STAT_ALLCHIDLE is asserted and SECCTRL.INTREN_ALLCHIDLE = 1. Cleared automatically when STAT_ALLCHIDLE is cleared.*/
#define DMA_SEC_STATUS_INTR_ALLCHSTOPPED_Pos        (2U)
#define DMA_SEC_STATUS_INTR_ALLCHSTOPPED_Msk        (0x1UL << DMA_SEC_STATUS_INTR_ALLCHSTOPPED_Pos)           /*!< 0x00000004UL*/
#define DMA_SEC_STATUS_INTR_ALLCHSTOPPED            DMA_SEC_STATUS_INTR_ALLCHSTOPPED_Msk                      /*!< INTR_ALLCHSTOPPED bit All Secure Channel Stopped Interrupt Status. Set to '1' when SECCTRL.STAT_ALLCHSTOPPED is asserted and SECCTRL.INTREN_ALLCHSTOPPED = 1. Cleared automatically when STAT_ALLCHSTOPPED is cleared.*/
#define DMA_SEC_STATUS_INTR_ALLCHPAUSED_Pos         (3U)
#define DMA_SEC_STATUS_INTR_ALLCHPAUSED_Msk         (0x1UL << DMA_SEC_STATUS_INTR_ALLCHPAUSED_Pos)            /*!< 0x00000008UL*/
#define DMA_SEC_STATUS_INTR_ALLCHPAUSED             DMA_SEC_STATUS_INTR_ALLCHPAUSED_Msk                       /*!< INTR_ALLCHPAUSED bit All Secure Channel Paused Interrupt Status. Set to '1' when SECCTRL.STAT_ALLCHPAUSED is asserted and SECCTRL.INTREN_ALLCHPAUSED = 1. Cleared automatically when STAT_ALLCHPAUSED is cleared.*/
#define DMA_SEC_STATUS_STAT_ALLCHIDLE_Pos           (17U)
#define DMA_SEC_STATUS_STAT_ALLCHIDLE_Msk           (0x1UL << DMA_SEC_STATUS_STAT_ALLCHIDLE_Pos)              /*!< 0x00020000UL*/
#define DMA_SEC_STATUS_STAT_ALLCHIDLE               DMA_SEC_STATUS_STAT_ALLCHIDLE_Msk                         /*!< STAT_ALLCHIDLE bit All Secure Channel Idle Status. Set to '1' whenever all Secure DMA channel is in idle state after at least one channel was running. Cleared by writing '1' to this bit.*/
#define DMA_SEC_STATUS_STAT_ALLCHSTOPPED_Pos        (18U)
#define DMA_SEC_STATUS_STAT_ALLCHSTOPPED_Msk        (0x1UL << DMA_SEC_STATUS_STAT_ALLCHSTOPPED_Pos)           /*!< 0x00040000UL*/
#define DMA_SEC_STATUS_STAT_ALLCHSTOPPED            DMA_SEC_STATUS_STAT_ALLCHSTOPPED_Msk                      /*!< STAT_ALLCHSTOPPED bit All Secure Channel Stopped Status. Set to '1' whenever all Secure DMA channels are in stopped or inactive state and the ALLCHSTOP request is active. Not set when channels reach the stopped state for other reasons. All Secure channels are forced to an immediate stopped state until ALLCHSTOP request is asserted even if they were enabled after the request. Cleared by writing '1' to this bit. The ALLCHSTOP request is also cleared when this bit is cleared.*/
#define DMA_SEC_STATUS_STAT_ALLCHPAUSED_Pos         (19U)
#define DMA_SEC_STATUS_STAT_ALLCHPAUSED_Msk         (0x1UL << DMA_SEC_STATUS_STAT_ALLCHPAUSED_Pos)            /*!< 0x00080000UL*/
#define DMA_SEC_STATUS_STAT_ALLCHPAUSED             DMA_SEC_STATUS_STAT_ALLCHPAUSED_Msk                       /*!< STAT_ALLCHPAUSED bit All Secure Channel Paused Status. Set to '1' whenever all Secure DMA channel is in paused or inactive state and the ALLCHPAUSE request is active. Not set when channels reach the paused state for other reasons. All Secure channels are forced to an immediate pause until ALLCHPAUSE request is asserted even if they were enabled after the request. Cleared by writing '1' to this bit which results in all secure channels to resume their operation. The ALLCHPAUSE request is also cleared when this bit is cleared.*/
/*****************  Field definitions for SEC_CTRL register  ******************/
#define DMA_SEC_CTRL_INTREN_ANYCHINTR_Pos           (0U)
#define DMA_SEC_CTRL_INTREN_ANYCHINTR_Msk           (0x1UL << DMA_SEC_CTRL_INTREN_ANYCHINTR_Pos)              /*!< 0x00000001UL*/
#define DMA_SEC_CTRL_INTREN_ANYCHINTR               DMA_SEC_CTRL_INTREN_ANYCHINTR_Msk                         /*!< INTREN_ANYCHINTR bit Combined Secure Channel Interrupt Enable*/
#define DMA_SEC_CTRL_INTREN_ALLCHIDLE_Pos           (1U)
#define DMA_SEC_CTRL_INTREN_ALLCHIDLE_Msk           (0x1UL << DMA_SEC_CTRL_INTREN_ALLCHIDLE_Pos)              /*!< 0x00000002UL*/
#define DMA_SEC_CTRL_INTREN_ALLCHIDLE               DMA_SEC_CTRL_INTREN_ALLCHIDLE_Msk                         /*!< INTREN_ALLCHIDLE bit All Secure Channel Idle Interrupt Enable*/
#define DMA_SEC_CTRL_INTREN_ALLCHSTOPPED_Pos        (2U)
#define DMA_SEC_CTRL_INTREN_ALLCHSTOPPED_Msk        (0x1UL << DMA_SEC_CTRL_INTREN_ALLCHSTOPPED_Pos)           /*!< 0x00000004UL*/
#define DMA_SEC_CTRL_INTREN_ALLCHSTOPPED            DMA_SEC_CTRL_INTREN_ALLCHSTOPPED_Msk                      /*!< INTREN_ALLCHSTOPPED bit All Secure Channels Stopped Interrupt Enable*/
#define DMA_SEC_CTRL_INTREN_ALLCHPAUSED_Pos         (3U)
#define DMA_SEC_CTRL_INTREN_ALLCHPAUSED_Msk         (0x1UL << DMA_SEC_CTRL_INTREN_ALLCHPAUSED_Pos)            /*!< 0x00000008UL*/
#define DMA_SEC_CTRL_INTREN_ALLCHPAUSED             DMA_SEC_CTRL_INTREN_ALLCHPAUSED_Msk                       /*!< INTREN_ALLCHPAUSED bit All Secure Channel Paused Interrupt Enable*/
#define DMA_SEC_CTRL_ALLCHSTOP_Pos                  (8U)
#define DMA_SEC_CTRL_ALLCHSTOP_Msk                  (0x1UL << DMA_SEC_CTRL_ALLCHSTOP_Pos)                     /*!< 0x00000100UL*/
#define DMA_SEC_CTRL_ALLCHSTOP                      DMA_SEC_CTRL_ALLCHSTOP_Msk                                /*!< ALLCHSTOP bit Secure All Channel Stop Request. When set to '1', all Secure channels get a stop request. Stays asserted until the STAT_ALLCHSTOPPED status flag is set. Cleared automatically when the STAT_ALLCHSTOPPED status flag is cleared.*/
#define DMA_SEC_CTRL_ALLCHPAUSE_Pos                 (9U)
#define DMA_SEC_CTRL_ALLCHPAUSE_Msk                 (0x1UL << DMA_SEC_CTRL_ALLCHPAUSE_Pos)                    /*!< 0x00000200UL*/
#define DMA_SEC_CTRL_ALLCHPAUSE                     DMA_SEC_CTRL_ALLCHPAUSE_Msk                               /*!< ALLCHPAUSE bit Secure All Channel Pause Request. When set to '1', all Secure channels get a pause request. Stays asserted until the channels are paused and the STAT_ALLCHPAUSED status flag is set. Cleared automatically when the STAT_ALLCHPAUSED status flag is cleared.*/
#define DMA_SEC_CTRL_DBGHALTNSRO_Pos                (27U)
#define DMA_SEC_CTRL_DBGHALTNSRO_Msk                (0x1UL << DMA_SEC_CTRL_DBGHALTNSRO_Pos)                   /*!< 0x08000000UL*/
#define DMA_SEC_CTRL_DBGHALTNSRO                    DMA_SEC_CTRL_DBGHALTNSRO_Msk                              /*!< DBGHALTNSRO bit Debug Halt Enable Non-Secure Read Only. When set to '1', the NSEC_CTRL.DBGHALTEN register becomes read-only. When set to '0', NSEC_CTRL.DBGHALTEN has read-write access. This register allows the secure SW to limit the non-secure SW adjusting the common DBGHALTEN register bit.*/
#define DMA_SEC_CTRL_DBGHALTEN_Pos                  (28U)
#define DMA_SEC_CTRL_DBGHALTEN_Msk                  (0x1UL << DMA_SEC_CTRL_DBGHALTEN_Pos)                     /*!< 0x10000000UL*/
#define DMA_SEC_CTRL_DBGHALTEN                      DMA_SEC_CTRL_DBGHALTEN_Msk                                /*!< DBGHALTEN bit Debug Halt Enabled. When set to '0', the DMA ignores the halt request from an external debugger. Clearing this bit while halt is ongoing results in continuing the operation. When set to '1', the debugger request to halt the DMA is allowed for all channels. This field is common for non-secure and secure side of the DMA, but control can be limited by the SEC_CTRL.DBGHALTNSRO register field.*/
#define DMA_SEC_CTRL_IDLERETEN_Pos                  (29U)
#define DMA_SEC_CTRL_IDLERETEN_Msk                  (0x1UL << DMA_SEC_CTRL_IDLERETEN_Pos)                     /*!< 0x20000000UL*/
#define DMA_SEC_CTRL_IDLERETEN                      DMA_SEC_CTRL_IDLERETEN_Msk                                /*!< IDLERETEN bit Idle Channel Retention Enable. Allows retention for Secure channels that are enabled and waiting for an event in IDLE state.
 - 0: disabled
 - 1: enabled*/
#define DMA_SEC_CTRL_DISMINPWR_Pos                  (30U)
#define DMA_SEC_CTRL_DISMINPWR_Msk                  (0x3UL << DMA_SEC_CTRL_DISMINPWR_Pos)                     /*!< 0xC0000000UL*/
#define DMA_SEC_CTRL_DISMINPWR                      DMA_SEC_CTRL_DISMINPWR_Msk                                /*!< DISMINPWR[ 1:0] bits Minimum Power state of the DMAC when at least one secure channel is present.
 - 00: OFF
 - 01: Retention
 - 10: ON
 - Others: Reserved*/
#define DMA_SEC_CTRL_DISMINPWR_0                    (0x1UL << DMA_SEC_CTRL_DISMINPWR_Pos)                     /*!< 0x40000000UL*/
#define DMA_SEC_CTRL_DISMINPWR_1                    (0x2UL << DMA_SEC_CTRL_DISMINPWR_Pos)                     /*!< 0x80000000UL*/
/*****************  Field definitions for SEC_CHPTR register  *****************/
#define DMA_SEC_CHPTR_CHPTR_Pos                     (0U)
#define DMA_SEC_CHPTR_CHPTR_Msk                     (0x3FUL << DMA_SEC_CHPTR_CHPTR_Pos)                       /*!< 0x0000003FUL*/
#define DMA_SEC_CHPTR_CHPTR                         DMA_SEC_CHPTR_CHPTR_Msk                                   /*!< CHPTR[ 5:0] bits Secure Channel Pointer. Selects which channel settings can be adjusted by the following registers.*/
#define DMA_SEC_CHPTR_CHPTR_0                       (0x1UL << DMA_SEC_CHPTR_CHPTR_Pos)                        /*!< 0x00000001UL*/
#define DMA_SEC_CHPTR_CHPTR_1                       (0x2UL << DMA_SEC_CHPTR_CHPTR_Pos)                        /*!< 0x00000002UL*/
#define DMA_SEC_CHPTR_CHPTR_2                       (0x4UL << DMA_SEC_CHPTR_CHPTR_Pos)                        /*!< 0x00000004UL*/
#define DMA_SEC_CHPTR_CHPTR_3                       (0x8UL << DMA_SEC_CHPTR_CHPTR_Pos)                        /*!< 0x00000008UL*/
#define DMA_SEC_CHPTR_CHPTR_4                       (0x10UL << DMA_SEC_CHPTR_CHPTR_Pos)                       /*!< 0x00000010UL*/
#define DMA_SEC_CHPTR_CHPTR_5                       (0x20UL << DMA_SEC_CHPTR_CHPTR_Pos)                       /*!< 0x00000020UL*/
/*****************  Field definitions for SEC_CHCFG register  *****************/
#define DMA_SEC_CHCFG_CHID_Pos                      (0U)
#define DMA_SEC_CHCFG_CHID_Msk                      (0xFFFFUL << DMA_SEC_CHCFG_CHID_Pos)                      /*!< 0x0000FFFFUL*/
#define DMA_SEC_CHCFG_CHID                          DMA_SEC_CHCFG_CHID_Msk                                    /*!< CHID[15:0] bits Secure Channel ID value. SECCHPTR selects the channel ID value to be read or written via this register. CHID_WIDTH limits this field, unused bits are RAZWI.*/
#define DMA_SEC_CHCFG_CHIDVLD_Pos                   (16U)
#define DMA_SEC_CHCFG_CHIDVLD_Msk                   (0x1UL << DMA_SEC_CHCFG_CHIDVLD_Pos)                      /*!< 0x00010000UL*/
#define DMA_SEC_CHCFG_CHIDVLD                       DMA_SEC_CHCFG_CHIDVLD_Msk                                 /*!< CHIDVLD bit Secure Channel ID valid. SECCHPTR selects the channel. Set to '1' to drive the channel ID value in CHID for all the transfers by the selected channel.*/
#define DMA_SEC_CHCFG_CHPRIV_Pos                    (17U)
#define DMA_SEC_CHCFG_CHPRIV_Msk                    (0x1UL << DMA_SEC_CHCFG_CHPRIV_Pos)                       /*!< 0x00020000UL*/
#define DMA_SEC_CHCFG_CHPRIV                        DMA_SEC_CHCFG_CHPRIV_Msk                                  /*!< CHPRIV bit Secure Channel Privilege Enable. SECCHPTR selects the channel. When set to '1' it allows the channel to send transfers marked as Privileged only. The configuration registers of the selected channel are also given privileged only access rights. When set to '0' the channel is only allowed to send unprivileged transfers and the channel registers can be accessed by both privileged and unprivileged register accesses.*/
/***************  Field definitions for SEC_STATUSPTR register  ***************/
#define DMA_SEC_STATUSPTR_SECSTATUSPTR_Pos          (0U)
#define DMA_SEC_STATUSPTR_SECSTATUSPTR_Msk          (0xFUL << DMA_SEC_STATUSPTR_SECSTATUSPTR_Pos)             /*!< 0x0000000FUL*/
#define DMA_SEC_STATUSPTR_SECSTATUSPTR              DMA_SEC_STATUSPTR_SECSTATUSPTR_Msk                        /*!< SECSTATUSPTR[ 3:0] bits Secure DMA Unit status pointer used to select which status value to view using STATUSVALUE register. Pointer values are:
 - 0: Channel Enabled Status for channel numbers [31 : 0].
 - 1: Reserved
 - 2: Channel Stopped Status for channel numbers [31 : 0].
 - 3: Reserved
 - 4: Channel Paused Status for channel numbers [31 : 0].
 - Others: Reserved.*/
#define DMA_SEC_STATUSPTR_SECSTATUSPTR_0            (0x1UL << DMA_SEC_STATUSPTR_SECSTATUSPTR_Pos)             /*!< 0x00000001UL*/
#define DMA_SEC_STATUSPTR_SECSTATUSPTR_1            (0x2UL << DMA_SEC_STATUSPTR_SECSTATUSPTR_Pos)             /*!< 0x00000002UL*/
#define DMA_SEC_STATUSPTR_SECSTATUSPTR_2            (0x4UL << DMA_SEC_STATUSPTR_SECSTATUSPTR_Pos)             /*!< 0x00000004UL*/
#define DMA_SEC_STATUSPTR_SECSTATUSPTR_3            (0x8UL << DMA_SEC_STATUSPTR_SECSTATUSPTR_Pos)             /*!< 0x00000008UL*/
/***************  Field definitions for SEC_STATUSVAL register  ***************/
#define DMA_SEC_STATUSVAL_SECSTATUSVAL_Pos          (0U)
#define DMA_SEC_STATUSVAL_SECSTATUSVAL_Msk          (0xFFFFFFFFUL << DMA_SEC_STATUSVAL_SECSTATUSVAL_Pos)      /*!< 0xFFFFFFFFUL*/
#define DMA_SEC_STATUSVAL_SECSTATUSVAL              DMA_SEC_STATUSVAL_SECSTATUSVAL_Msk                        /*!< SECSTATUSVAL[31:0] bits Secure DMA Unit status value. Can be used for reading internal status values of the DMA Unit for debug purposes. Values shown here are dependent on STATUSPTR. Note that inputs are masked by security mapping before being presented here. This means that only status values mapped to Secure world are visible as non-Zero values.
All unimplemented bits are RAZWI. */
/***************  Field definitions for SEC_SIGNALPTR register  ***************/
#define DMA_SEC_SIGNALPTR_SECSIGNALPTR_Pos          (0U)
#define DMA_SEC_SIGNALPTR_SECSIGNALPTR_Msk          (0xFUL << DMA_SEC_SIGNALPTR_SECSIGNALPTR_Pos)             /*!< 0x0000000FUL*/
#define DMA_SEC_SIGNALPTR_SECSIGNALPTR              DMA_SEC_SIGNALPTR_SECSIGNALPTR_Msk                        /*!< SECSIGNALPTR[ 3:0] bits Secure DMA Unit signal pointer used to select which inputs to view using SIGNALVAL register. Pointer values, x,  are:
 - 0 to 7: Trigger Input Requests [31+32*x : 32*x]
 - 8 to 9: Trigger output Acknowledges [31+32*(x-8) :  32*(x-8)]
 - 10 to 11: GPO output value [31+32*(x-10) : 32*(x-10)], only present when HAS_GPOSEL is set.
 - Others: Reserved*/
#define DMA_SEC_SIGNALPTR_SECSIGNALPTR_0            (0x1UL << DMA_SEC_SIGNALPTR_SECSIGNALPTR_Pos)             /*!< 0x00000001UL*/
#define DMA_SEC_SIGNALPTR_SECSIGNALPTR_1            (0x2UL << DMA_SEC_SIGNALPTR_SECSIGNALPTR_Pos)             /*!< 0x00000002UL*/
#define DMA_SEC_SIGNALPTR_SECSIGNALPTR_2            (0x4UL << DMA_SEC_SIGNALPTR_SECSIGNALPTR_Pos)             /*!< 0x00000004UL*/
#define DMA_SEC_SIGNALPTR_SECSIGNALPTR_3            (0x8UL << DMA_SEC_SIGNALPTR_SECSIGNALPTR_Pos)             /*!< 0x00000008UL*/
/***************  Field definitions for SEC_SIGNALVAL register  ***************/
#define DMA_SEC_SIGNALVAL_SECSIGNALVAL_Pos          (0U)
#define DMA_SEC_SIGNALVAL_SECSIGNALVAL_Msk          (0xFFFFFFFFUL << DMA_SEC_SIGNALVAL_SECSIGNALVAL_Pos)      /*!< 0xFFFFFFFFUL*/
#define DMA_SEC_SIGNALVAL_SECSIGNALVAL              DMA_SEC_SIGNALVAL_SECSIGNALVAL_Msk                        /*!< SECSIGNALVAL[31:0] bits Secure DMA Unit signal status. Can be used for reading signal values of triggers and GPOs for debug. Values shown here are dependent on SIGNALPTR. Note that inputs are masked by security mapping before being presented here. This means that only signal values mapped to Secure world are visible as non-zero values. Writing to this register has the following effect:
 - Writing '1' to a Trigger Input Request that are not selected by any DMA channel will cause a deny response for it. This can attempt to clear an unwanted trigger input.
 - Writing '1' to any Trigger Output Acknowledges status, GPO value and all Trigger Input Request selected by any DMA channel is ignored.
All unimplemented bits are RAZWI. */
/******************************************************************************/
/*                                 DMASECCFG                                  */
/******************************************************************************/
/****************  Field definitions for SCFG_CHSEC0 register  ****************/
#define DMA_SCFG_CHSEC0_SCFGCHSEC0_Pos              (0U)
#define DMA_SCFG_CHSEC0_SCFGCHSEC0_Msk              (0xFFFFFFFFUL << DMA_SCFG_CHSEC0_SCFGCHSEC0_Pos)          /*!< 0xFFFFFFFFUL*/
#define DMA_SCFG_CHSEC0_SCFGCHSEC0                  DMA_SCFG_CHSEC0_SCFGCHSEC0_Msk                            /*!< SCFGCHSEC0[31:0] bits Secure Configuration Channel Security Mapping for Channel 0 to 31. When [i] set to '1', CH<i> is Non-secure world, else Secure world. The NUM_CHANNELS parameter limits this field, unused bits are RAZWI. Value for bit[i] can only be changed if the selected channel is not enabled, so reading back the register content is needed to check the success of the change.*/
/**************  Field definitions for SCFG_TRIGINSEC0 register  **************/
#define DMA_SCFG_TRIGINSEC0_SCFGTRIGINSEC0_Pos      (0U)
#define DMA_SCFG_TRIGINSEC0_SCFGTRIGINSEC0_Msk      (0xFFFFFFFFUL << DMA_SCFG_TRIGINSEC0_SCFGTRIGINSEC0_Pos)  /*!< 0xFFFFFFFFUL*/
#define DMA_SCFG_TRIGINSEC0_SCFGTRIGINSEC0          DMA_SCFG_TRIGINSEC0_SCFGTRIGINSEC0_Msk                    /*!< SCFGTRIGINSEC0[31:0] bits Secure Configuration Tigger Input Security Mapping. When [i] set to '1', Trigger Input <i> is Non-secure world, else Secure world. The NUM_TRIGGER_IN parameter limits this field, unused bits are RAZWI. Value for bit[i] can only be changed if the trigger input port is not in use, so reading back the register content is needed to check the success of the change.*/
/*************  Field definitions for SCFG_TRIGOUTSEC0 register  **************/
#define DMA_SCFG_TRIGOUTSEC0_SCFGTRIGOUTSEC0_Pos    (0U)
#define DMA_SCFG_TRIGOUTSEC0_SCFGTRIGOUTSEC0_Msk    (0xFFFFFFFFUL << DMA_SCFG_TRIGOUTSEC0_SCFGTRIGOUTSEC0_Pos)/*!< 0xFFFFFFFFUL*/
#define DMA_SCFG_TRIGOUTSEC0_SCFGTRIGOUTSEC0        DMA_SCFG_TRIGOUTSEC0_SCFGTRIGOUTSEC0_Msk                  /*!< SCFGTRIGOUTSEC0[31:0] bits Secure Configuration Tigger Output Security Mapping. When [i] set to '1', Trigger Output <i> is Non-secure world, else Secure world. The NUM_TRIGGER_OUT parameter limits this field, unused bits are RAZWI. Value for bit[i] can only be changed if the trigger output port is not in use, so reading back the register content is needed to check the success of the change.*/
/*****************  Field definitions for SCFG_CTRL register  *****************/
#define DMA_SCFG_CTRL_INTREN_SECACCVIO_Pos          (0U)
#define DMA_SCFG_CTRL_INTREN_SECACCVIO_Msk          (0x1UL << DMA_SCFG_CTRL_INTREN_SECACCVIO_Pos)             /*!< 0x00000001UL*/
#define DMA_SCFG_CTRL_INTREN_SECACCVIO              DMA_SCFG_CTRL_INTREN_SECACCVIO_Msk                        /*!< INTREN_SECACCVIO bit Secure Access Violation Interrupt Enable.
 - 0: no interrupt
 - 1: interrupt raised for security violation.*/
#define DMA_SCFG_CTRL_RSPTYPE_SECACCVIO_Pos         (1U)
#define DMA_SCFG_CTRL_RSPTYPE_SECACCVIO_Msk         (0x1UL << DMA_SCFG_CTRL_RSPTYPE_SECACCVIO_Pos)            /*!< 0x00000002UL*/
#define DMA_SCFG_CTRL_RSPTYPE_SECACCVIO             DMA_SCFG_CTRL_RSPTYPE_SECACCVIO_Msk                       /*!< RSPTYPE_SECACCVIO bit Secure Access Violation response type configuration.
 - 0: RAZWI
 - 1: bus error.*/
#define DMA_SCFG_CTRL_SEC_CFG_LCK_Pos               (31U)
#define DMA_SCFG_CTRL_SEC_CFG_LCK_Msk               (0x1UL << DMA_SCFG_CTRL_SEC_CFG_LCK_Pos)                  /*!< 0x80000000UL*/
#define DMA_SCFG_CTRL_SEC_CFG_LCK                   DMA_SCFG_CTRL_SEC_CFG_LCK_Msk                             /*!< SEC_CFG_LCK bit Security Configuration Lock. When set to '1', only SCFG.STAT_SECACCVIO can be cleared, all other register fields in the this block become read-only. Once set to '1', this field can only be set back to '0' by reset.*/
/**************  Field definitions for SCFG_INTRSTATUS register  **************/
#define DMA_SCFG_INTRSTATUS_INTR_SECACCVIO_Pos      (0U)
#define DMA_SCFG_INTRSTATUS_INTR_SECACCVIO_Msk      (0x1UL << DMA_SCFG_INTRSTATUS_INTR_SECACCVIO_Pos)         /*!< 0x00000001UL*/
#define DMA_SCFG_INTRSTATUS_INTR_SECACCVIO          DMA_SCFG_INTRSTATUS_INTR_SECACCVIO_Msk                    /*!< INTR_SECACCVIO bit Secure Access Violation Interrupt Status. Set to '1' when SCFG.STAT_SECACCVIO is asserted and SCFG.INTREN_SECACCVIO = 1. Cleared automatically when STAT_SECACCVIO is cleared.*/
#define DMA_SCFG_INTRSTATUS_STAT_SECACCVIO_Pos      (16U)
#define DMA_SCFG_INTRSTATUS_STAT_SECACCVIO_Msk      (0x1UL << DMA_SCFG_INTRSTATUS_STAT_SECACCVIO_Pos)         /*!< 0x00010000UL*/
#define DMA_SCFG_INTRSTATUS_STAT_SECACCVIO          DMA_SCFG_INTRSTATUS_STAT_SECACCVIO_Msk                    /*!< STAT_SECACCVIO bit Secure Access Violation Status. Set to '1' when a security violation occurred. Write '1' to clear.*/
/******************************************************************************/
/*                                  DMAINFO                                   */
/******************************************************************************/
/***************  Field definitions for DMA_BUILDCFG0 register  ***************/
#define DMA_DMA_BUILDCFG0_FRAMETYPE_Pos             (0U)
#define DMA_DMA_BUILDCFG0_FRAMETYPE_Msk             (0x7UL << DMA_DMA_BUILDCFG0_FRAMETYPE_Pos)                /*!< 0x00000007UL*/
#define DMA_DMA_BUILDCFG0_FRAMETYPE                 DMA_DMA_BUILDCFG0_FRAMETYPE_Msk                           /*!< FRAMETYPE[ 2:0] bits Register Frame Type.
 - 000: Combined Frame,
 - 001: Security Configuration Frame,
 - 010: Secure Control Frame, or if TrustZone not implemented, Control Frame.
 - 011: Non-Secure Control Frame,
 - 100: DMA Channel Frame.
Note that each frame replicates the Information fields.*/
#define DMA_DMA_BUILDCFG0_FRAMETYPE_0               (0x1UL << DMA_DMA_BUILDCFG0_FRAMETYPE_Pos)                /*!< 0x00000001UL*/
#define DMA_DMA_BUILDCFG0_FRAMETYPE_1               (0x2UL << DMA_DMA_BUILDCFG0_FRAMETYPE_Pos)                /*!< 0x00000002UL*/
#define DMA_DMA_BUILDCFG0_FRAMETYPE_2               (0x4UL << DMA_DMA_BUILDCFG0_FRAMETYPE_Pos)                /*!< 0x00000004UL*/
#define DMA_DMA_BUILDCFG0_NUM_CHANNELS_Pos          (4U)
#define DMA_DMA_BUILDCFG0_NUM_CHANNELS_Msk          (0x3FUL << DMA_DMA_BUILDCFG0_NUM_CHANNELS_Pos)            /*!< 0x000003F0UL*/
#define DMA_DMA_BUILDCFG0_NUM_CHANNELS              DMA_DMA_BUILDCFG0_NUM_CHANNELS_Msk                        /*!< NUM_CHANNELS[ 5:0] bits Number of Channels + 1 supported by the DMAC*/
#define DMA_DMA_BUILDCFG0_NUM_CHANNELS_0            (0x1UL << DMA_DMA_BUILDCFG0_NUM_CHANNELS_Pos)             /*!< 0x00000010UL*/
#define DMA_DMA_BUILDCFG0_NUM_CHANNELS_1            (0x2UL << DMA_DMA_BUILDCFG0_NUM_CHANNELS_Pos)             /*!< 0x00000020UL*/
#define DMA_DMA_BUILDCFG0_NUM_CHANNELS_2            (0x4UL << DMA_DMA_BUILDCFG0_NUM_CHANNELS_Pos)             /*!< 0x00000040UL*/
#define DMA_DMA_BUILDCFG0_NUM_CHANNELS_3            (0x8UL << DMA_DMA_BUILDCFG0_NUM_CHANNELS_Pos)             /*!< 0x00000080UL*/
#define DMA_DMA_BUILDCFG0_NUM_CHANNELS_4            (0x10UL << DMA_DMA_BUILDCFG0_NUM_CHANNELS_Pos)            /*!< 0x00000100UL*/
#define DMA_DMA_BUILDCFG0_NUM_CHANNELS_5            (0x20UL << DMA_DMA_BUILDCFG0_NUM_CHANNELS_Pos)            /*!< 0x00000200UL*/
#define DMA_DMA_BUILDCFG0_ADDR_WIDTH_Pos            (10U)
#define DMA_DMA_BUILDCFG0_ADDR_WIDTH_Msk            (0x3FUL << DMA_DMA_BUILDCFG0_ADDR_WIDTH_Pos)              /*!< 0x0000FC00UL*/
#define DMA_DMA_BUILDCFG0_ADDR_WIDTH                DMA_DMA_BUILDCFG0_ADDR_WIDTH_Msk                          /*!< ADDR_WIDTH[ 5:0] bits Address Width in bits = ADDR_WIDTH + 1*/
#define DMA_DMA_BUILDCFG0_ADDR_WIDTH_0              (0x1UL << DMA_DMA_BUILDCFG0_ADDR_WIDTH_Pos)               /*!< 0x00000400UL*/
#define DMA_DMA_BUILDCFG0_ADDR_WIDTH_1              (0x2UL << DMA_DMA_BUILDCFG0_ADDR_WIDTH_Pos)               /*!< 0x00000800UL*/
#define DMA_DMA_BUILDCFG0_ADDR_WIDTH_2              (0x4UL << DMA_DMA_BUILDCFG0_ADDR_WIDTH_Pos)               /*!< 0x00001000UL*/
#define DMA_DMA_BUILDCFG0_ADDR_WIDTH_3              (0x8UL << DMA_DMA_BUILDCFG0_ADDR_WIDTH_Pos)               /*!< 0x00002000UL*/
#define DMA_DMA_BUILDCFG0_ADDR_WIDTH_4              (0x10UL << DMA_DMA_BUILDCFG0_ADDR_WIDTH_Pos)              /*!< 0x00004000UL*/
#define DMA_DMA_BUILDCFG0_ADDR_WIDTH_5              (0x20UL << DMA_DMA_BUILDCFG0_ADDR_WIDTH_Pos)              /*!< 0x00008000UL*/
#define DMA_DMA_BUILDCFG0_DATA_WIDTH_Pos            (16U)
#define DMA_DMA_BUILDCFG0_DATA_WIDTH_Msk            (0x7UL << DMA_DMA_BUILDCFG0_DATA_WIDTH_Pos)               /*!< 0x00070000UL*/
#define DMA_DMA_BUILDCFG0_DATA_WIDTH                DMA_DMA_BUILDCFG0_DATA_WIDTH_Msk                          /*!< DATA_WIDTH[ 2:0] bits Data Width.
 - 000: 8-bit
 - 001: 16-bit
 - 010: 32-bit
 - 011: 64-bit
 - 100: 128-bit
 - 101: 256-bit
 - 110: 512-bit
 - 111: 1024-bit*/
#define DMA_DMA_BUILDCFG0_DATA_WIDTH_0              (0x1UL << DMA_DMA_BUILDCFG0_DATA_WIDTH_Pos)               /*!< 0x00010000UL*/
#define DMA_DMA_BUILDCFG0_DATA_WIDTH_1              (0x2UL << DMA_DMA_BUILDCFG0_DATA_WIDTH_Pos)               /*!< 0x00020000UL*/
#define DMA_DMA_BUILDCFG0_DATA_WIDTH_2              (0x4UL << DMA_DMA_BUILDCFG0_DATA_WIDTH_Pos)               /*!< 0x00040000UL*/
#define DMA_DMA_BUILDCFG0_CHID_WIDTH_Pos            (20U)
#define DMA_DMA_BUILDCFG0_CHID_WIDTH_Msk            (0x1FUL << DMA_DMA_BUILDCFG0_CHID_WIDTH_Pos)              /*!< 0x01F00000UL*/
#define DMA_DMA_BUILDCFG0_CHID_WIDTH                DMA_DMA_BUILDCFG0_CHID_WIDTH_Msk                          /*!< CHID_WIDTH[ 4:0] bits Channel ID Width. '0' means CHID is not present.*/
#define DMA_DMA_BUILDCFG0_CHID_WIDTH_0              (0x1UL << DMA_DMA_BUILDCFG0_CHID_WIDTH_Pos)               /*!< 0x00100000UL*/
#define DMA_DMA_BUILDCFG0_CHID_WIDTH_1              (0x2UL << DMA_DMA_BUILDCFG0_CHID_WIDTH_Pos)               /*!< 0x00200000UL*/
#define DMA_DMA_BUILDCFG0_CHID_WIDTH_2              (0x4UL << DMA_DMA_BUILDCFG0_CHID_WIDTH_Pos)               /*!< 0x00400000UL*/
#define DMA_DMA_BUILDCFG0_CHID_WIDTH_3              (0x8UL << DMA_DMA_BUILDCFG0_CHID_WIDTH_Pos)               /*!< 0x00800000UL*/
#define DMA_DMA_BUILDCFG0_CHID_WIDTH_4              (0x10UL << DMA_DMA_BUILDCFG0_CHID_WIDTH_Pos)              /*!< 0x01000000UL*/
/***************  Field definitions for DMA_BUILDCFG1 register  ***************/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_Pos        (0U)
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_Msk        (0x1FFUL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_Pos)         /*!< 0x000001FFUL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN            DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_Msk                      /*!< NUM_TRIGGER_IN[ 8:0] bits Number of  Triggers Inputs. '0' means that no input triggers are present.*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_0          (0x1UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_Pos)           /*!< 0x00000001UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_1          (0x2UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_Pos)           /*!< 0x00000002UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_2          (0x4UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_Pos)           /*!< 0x00000004UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_3          (0x8UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_Pos)           /*!< 0x00000008UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_4          (0x10UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_Pos)          /*!< 0x00000010UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_5          (0x20UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_Pos)          /*!< 0x00000020UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_6          (0x40UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_Pos)          /*!< 0x00000040UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_7          (0x80UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_Pos)          /*!< 0x00000080UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_8          (0x100UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_IN_Pos)         /*!< 0x00000100UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_Pos       (9U)
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_Msk       (0x7FUL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_Pos)         /*!< 0x0000FE00UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT           DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_Msk                     /*!< NUM_TRIGGER_OUT[ 6:0] bits Number of  Triggers Outputs. '0' means that no output triggers are present.*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_0         (0x1UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_Pos)          /*!< 0x00000200UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_1         (0x2UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_Pos)          /*!< 0x00000400UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_2         (0x4UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_Pos)          /*!< 0x00000800UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_3         (0x8UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_Pos)          /*!< 0x00001000UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_4         (0x10UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_Pos)         /*!< 0x00002000UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_5         (0x20UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_Pos)         /*!< 0x00004000UL*/
#define DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_6         (0x40UL << DMA_DMA_BUILDCFG1_NUM_TRIGGER_OUT_Pos)         /*!< 0x00008000UL*/
#define DMA_DMA_BUILDCFG1_HAS_TRIGSEL_Pos           (16U)
#define DMA_DMA_BUILDCFG1_HAS_TRIGSEL_Msk           (0x1UL << DMA_DMA_BUILDCFG1_HAS_TRIGSEL_Pos)              /*!< 0x00010000UL*/
#define DMA_DMA_BUILDCFG1_HAS_TRIGSEL               DMA_DMA_BUILDCFG1_HAS_TRIGSEL_Msk                         /*!< HAS_TRIGSEL bit Has Selectable Trigger Support*/
/***************  Field definitions for DMA_BUILDCFG2 register  ***************/
#define DMA_DMA_BUILDCFG2_HAS_GPOSEL_Pos            (7U)
#define DMA_DMA_BUILDCFG2_HAS_GPOSEL_Msk            (0x1UL << DMA_DMA_BUILDCFG2_HAS_GPOSEL_Pos)               /*!< 0x00000080UL*/
#define DMA_DMA_BUILDCFG2_HAS_GPOSEL                DMA_DMA_BUILDCFG2_HAS_GPOSEL_Msk                          /*!< HAS_GPOSEL bit Has Shared and hence Selectable GPO Support.*/
#define DMA_DMA_BUILDCFG2_HAS_TZ_Pos                (8U)
#define DMA_DMA_BUILDCFG2_HAS_TZ_Msk                (0x1UL << DMA_DMA_BUILDCFG2_HAS_TZ_Pos)                   /*!< 0x00000100UL*/
#define DMA_DMA_BUILDCFG2_HAS_TZ                    DMA_DMA_BUILDCFG2_HAS_TZ_Msk                              /*!< HAS_TZ bit Has TrustZone Support*/
#define DMA_DMA_BUILDCFG2_HAS_RET_Pos               (9U)
#define DMA_DMA_BUILDCFG2_HAS_RET_Msk               (0x1UL << DMA_DMA_BUILDCFG2_HAS_RET_Pos)                  /*!< 0x00000200UL*/
#define DMA_DMA_BUILDCFG2_HAS_RET                   DMA_DMA_BUILDCFG2_HAS_RET_Msk                             /*!< HAS_RET bit Has Retention Support*/
/*******************  Field definitions for IIDR register  ********************/
#define DMA_IIDR_IMPLEMENTER_Pos                    (0U)
#define DMA_IIDR_IMPLEMENTER_Msk                    (0xFFFUL << DMA_IIDR_IMPLEMENTER_Pos)                     /*!< 0x00000FFFUL*/
#define DMA_IIDR_IMPLEMENTER                        DMA_IIDR_IMPLEMENTER_Msk                                  /*!< IMPLEMENTER[11:0] bits Contains the JEP106 code of the company that implemented the IP:
 - [11:8]: JEP106 continuation code of implementer.
 - [7]: Always 0.
 - [6:0]: JEP106 identity code of implementer.
For Arm this field reads as 0x43B.*/
#define DMA_IIDR_IMPLEMENTER_0                      (0x1UL << DMA_IIDR_IMPLEMENTER_Pos)                       /*!< 0x00000001UL*/
#define DMA_IIDR_IMPLEMENTER_1                      (0x2UL << DMA_IIDR_IMPLEMENTER_Pos)                       /*!< 0x00000002UL*/
#define DMA_IIDR_IMPLEMENTER_2                      (0x4UL << DMA_IIDR_IMPLEMENTER_Pos)                       /*!< 0x00000004UL*/
#define DMA_IIDR_IMPLEMENTER_3                      (0x8UL << DMA_IIDR_IMPLEMENTER_Pos)                       /*!< 0x00000008UL*/
#define DMA_IIDR_IMPLEMENTER_4                      (0x10UL << DMA_IIDR_IMPLEMENTER_Pos)                      /*!< 0x00000010UL*/
#define DMA_IIDR_IMPLEMENTER_5                      (0x20UL << DMA_IIDR_IMPLEMENTER_Pos)                      /*!< 0x00000020UL*/
#define DMA_IIDR_IMPLEMENTER_6                      (0x40UL << DMA_IIDR_IMPLEMENTER_Pos)                      /*!< 0x00000040UL*/
#define DMA_IIDR_IMPLEMENTER_7                      (0x80UL << DMA_IIDR_IMPLEMENTER_Pos)                      /*!< 0x00000080UL*/
#define DMA_IIDR_IMPLEMENTER_8                      (0x100UL << DMA_IIDR_IMPLEMENTER_Pos)                     /*!< 0x00000100UL*/
#define DMA_IIDR_IMPLEMENTER_9                      (0x200UL << DMA_IIDR_IMPLEMENTER_Pos)                     /*!< 0x00000200UL*/
#define DMA_IIDR_IMPLEMENTER_10                     (0x400UL << DMA_IIDR_IMPLEMENTER_Pos)                     /*!< 0x00000400UL*/
#define DMA_IIDR_IMPLEMENTER_11                     (0x800UL << DMA_IIDR_IMPLEMENTER_Pos)                     /*!< 0x00000800UL*/
#define DMA_IIDR_REVISION_Pos                       (12U)
#define DMA_IIDR_REVISION_Msk                       (0xFUL << DMA_IIDR_REVISION_Pos)                          /*!< 0x0000F000UL*/
#define DMA_IIDR_REVISION                           DMA_IIDR_REVISION_Msk                                     /*!< REVISION[ 3:0] bits Indicates the minor revision of the product rxpy identifier*/
#define DMA_IIDR_REVISION_0                         (0x1UL << DMA_IIDR_REVISION_Pos)                          /*!< 0x00001000UL*/
#define DMA_IIDR_REVISION_1                         (0x2UL << DMA_IIDR_REVISION_Pos)                          /*!< 0x00002000UL*/
#define DMA_IIDR_REVISION_2                         (0x4UL << DMA_IIDR_REVISION_Pos)                          /*!< 0x00004000UL*/
#define DMA_IIDR_REVISION_3                         (0x8UL << DMA_IIDR_REVISION_Pos)                          /*!< 0x00008000UL*/
#define DMA_IIDR_VARIANT_Pos                        (16U)
#define DMA_IIDR_VARIANT_Msk                        (0xFUL << DMA_IIDR_VARIANT_Pos)                           /*!< 0x000F0000UL*/
#define DMA_IIDR_VARIANT                            DMA_IIDR_VARIANT_Msk                                      /*!< VARIANT[ 3:0] bits Indicates the major revision, or variant, of the product rxpy identifier*/
#define DMA_IIDR_VARIANT_0                          (0x1UL << DMA_IIDR_VARIANT_Pos)                           /*!< 0x00010000UL*/
#define DMA_IIDR_VARIANT_1                          (0x2UL << DMA_IIDR_VARIANT_Pos)                           /*!< 0x00020000UL*/
#define DMA_IIDR_VARIANT_2                          (0x4UL << DMA_IIDR_VARIANT_Pos)                           /*!< 0x00040000UL*/
#define DMA_IIDR_VARIANT_3                          (0x8UL << DMA_IIDR_VARIANT_Pos)                           /*!< 0x00080000UL*/
#define DMA_IIDR_PRODUCTID_Pos                      (20U)
#define DMA_IIDR_PRODUCTID_Msk                      (0xFFFUL << DMA_IIDR_PRODUCTID_Pos)                       /*!< 0xFFF00000UL*/
#define DMA_IIDR_PRODUCTID                          DMA_IIDR_PRODUCTID_Msk                                    /*!< PRODUCTID[11:0] bits Indicates the product ID*/
#define DMA_IIDR_PRODUCTID_0                        (0x1UL << DMA_IIDR_PRODUCTID_Pos)                         /*!< 0x00100000UL*/
#define DMA_IIDR_PRODUCTID_1                        (0x2UL << DMA_IIDR_PRODUCTID_Pos)                         /*!< 0x00200000UL*/
#define DMA_IIDR_PRODUCTID_2                        (0x4UL << DMA_IIDR_PRODUCTID_Pos)                         /*!< 0x00400000UL*/
#define DMA_IIDR_PRODUCTID_3                        (0x8UL << DMA_IIDR_PRODUCTID_Pos)                         /*!< 0x00800000UL*/
#define DMA_IIDR_PRODUCTID_4                        (0x10UL << DMA_IIDR_PRODUCTID_Pos)                        /*!< 0x01000000UL*/
#define DMA_IIDR_PRODUCTID_5                        (0x20UL << DMA_IIDR_PRODUCTID_Pos)                        /*!< 0x02000000UL*/
#define DMA_IIDR_PRODUCTID_6                        (0x40UL << DMA_IIDR_PRODUCTID_Pos)                        /*!< 0x04000000UL*/
#define DMA_IIDR_PRODUCTID_7                        (0x80UL << DMA_IIDR_PRODUCTID_Pos)                        /*!< 0x08000000UL*/
#define DMA_IIDR_PRODUCTID_8                        (0x100UL << DMA_IIDR_PRODUCTID_Pos)                       /*!< 0x10000000UL*/
#define DMA_IIDR_PRODUCTID_9                        (0x200UL << DMA_IIDR_PRODUCTID_Pos)                       /*!< 0x20000000UL*/
#define DMA_IIDR_PRODUCTID_10                       (0x400UL << DMA_IIDR_PRODUCTID_Pos)                       /*!< 0x40000000UL*/
#define DMA_IIDR_PRODUCTID_11                       (0x800UL << DMA_IIDR_PRODUCTID_Pos)                       /*!< 0x80000000UL*/
/*******************  Field definitions for AIDR register  ********************/
#define DMA_AIDR_ARCH_MINOR_REV_Pos                 (0U)
#define DMA_AIDR_ARCH_MINOR_REV_Msk                 (0xFUL << DMA_AIDR_ARCH_MINOR_REV_Pos)                    /*!< 0x0000000FUL*/
#define DMA_AIDR_ARCH_MINOR_REV                     DMA_AIDR_ARCH_MINOR_REV_Msk                               /*!< ARCH_MINOR_REV[ 3:0] bits Architecture Minor Revision.*/
#define DMA_AIDR_ARCH_MINOR_REV_0                   (0x1UL << DMA_AIDR_ARCH_MINOR_REV_Pos)                    /*!< 0x00000001UL*/
#define DMA_AIDR_ARCH_MINOR_REV_1                   (0x2UL << DMA_AIDR_ARCH_MINOR_REV_Pos)                    /*!< 0x00000002UL*/
#define DMA_AIDR_ARCH_MINOR_REV_2                   (0x4UL << DMA_AIDR_ARCH_MINOR_REV_Pos)                    /*!< 0x00000004UL*/
#define DMA_AIDR_ARCH_MINOR_REV_3                   (0x8UL << DMA_AIDR_ARCH_MINOR_REV_Pos)                    /*!< 0x00000008UL*/
#define DMA_AIDR_ARCH_MAJOR_REV_Pos                 (4U)
#define DMA_AIDR_ARCH_MAJOR_REV_Msk                 (0xFUL << DMA_AIDR_ARCH_MAJOR_REV_Pos)                    /*!< 0x000000F0UL*/
#define DMA_AIDR_ARCH_MAJOR_REV                     DMA_AIDR_ARCH_MAJOR_REV_Msk                               /*!< ARCH_MAJOR_REV[ 3:0] bits Architecture Major Revision.*/
#define DMA_AIDR_ARCH_MAJOR_REV_0                   (0x1UL << DMA_AIDR_ARCH_MAJOR_REV_Pos)                    /*!< 0x00000010UL*/
#define DMA_AIDR_ARCH_MAJOR_REV_1                   (0x2UL << DMA_AIDR_ARCH_MAJOR_REV_Pos)                    /*!< 0x00000020UL*/
#define DMA_AIDR_ARCH_MAJOR_REV_2                   (0x4UL << DMA_AIDR_ARCH_MAJOR_REV_Pos)                    /*!< 0x00000040UL*/
#define DMA_AIDR_ARCH_MAJOR_REV_3                   (0x8UL << DMA_AIDR_ARCH_MAJOR_REV_Pos)                    /*!< 0x00000080UL*/
/*******************  Field definitions for PIDR4 register  *******************/
#define DMA_PIDR4_DES_2_Pos                         (0U)
#define DMA_PIDR4_DES_2_Msk                         (0xFUL << DMA_PIDR4_DES_2_Pos)                            /*!< 0x0000000FUL*/
#define DMA_PIDR4_DES_2                             DMA_PIDR4_DES_2_Msk                                       /*!< DES_2[ 3:0] bits JEP106 Continuation Code*/
#define DMA_PIDR4_DES_2_0                           (0x1UL << DMA_PIDR4_DES_2_Pos)                            /*!< 0x00000001UL*/
#define DMA_PIDR4_DES_2_1                           (0x2UL << DMA_PIDR4_DES_2_Pos)                            /*!< 0x00000002UL*/
#define DMA_PIDR4_DES_2_2                           (0x4UL << DMA_PIDR4_DES_2_Pos)                            /*!< 0x00000004UL*/
#define DMA_PIDR4_DES_2_3                           (0x8UL << DMA_PIDR4_DES_2_Pos)                            /*!< 0x00000008UL*/
#define DMA_PIDR4_SIZE_Pos                          (4U)
#define DMA_PIDR4_SIZE_Msk                          (0xFUL << DMA_PIDR4_SIZE_Pos)                             /*!< 0x000000F0UL*/
#define DMA_PIDR4_SIZE                              DMA_PIDR4_SIZE_Msk                                        /*!< SIZE[ 3:0] bits 4KB Count - the number of 4K pages used.
 - 0x00: 4K
 - 0x01: 8K
 - 0x02: 16K
 - 0x03: 32K*/
#define DMA_PIDR4_SIZE_0                            (0x1UL << DMA_PIDR4_SIZE_Pos)                             /*!< 0x00000010UL*/
#define DMA_PIDR4_SIZE_1                            (0x2UL << DMA_PIDR4_SIZE_Pos)                             /*!< 0x00000020UL*/
#define DMA_PIDR4_SIZE_2                            (0x4UL << DMA_PIDR4_SIZE_Pos)                             /*!< 0x00000040UL*/
#define DMA_PIDR4_SIZE_3                            (0x8UL << DMA_PIDR4_SIZE_Pos)                             /*!< 0x00000080UL*/
/*******************  Field definitions for PIDR0 register  *******************/
#define DMA_PIDR0_PART_0_Pos                        (0U)
#define DMA_PIDR0_PART_0_Msk                        (0xFFUL << DMA_PIDR0_PART_0_Pos)                          /*!< 0x000000FFUL*/
#define DMA_PIDR0_PART_0                            DMA_PIDR0_PART_0_Msk                                      /*!< PART_0[ 7:0] bits Part Number [7:0]*/
#define DMA_PIDR0_PART_0_0                          (0x1UL << DMA_PIDR0_PART_0_Pos)                           /*!< 0x00000001UL*/
#define DMA_PIDR0_PART_0_1                          (0x2UL << DMA_PIDR0_PART_0_Pos)                           /*!< 0x00000002UL*/
#define DMA_PIDR0_PART_0_2                          (0x4UL << DMA_PIDR0_PART_0_Pos)                           /*!< 0x00000004UL*/
#define DMA_PIDR0_PART_0_3                          (0x8UL << DMA_PIDR0_PART_0_Pos)                           /*!< 0x00000008UL*/
#define DMA_PIDR0_PART_0_4                          (0x10UL << DMA_PIDR0_PART_0_Pos)                          /*!< 0x00000010UL*/
#define DMA_PIDR0_PART_0_5                          (0x20UL << DMA_PIDR0_PART_0_Pos)                          /*!< 0x00000020UL*/
#define DMA_PIDR0_PART_0_6                          (0x40UL << DMA_PIDR0_PART_0_Pos)                          /*!< 0x00000040UL*/
#define DMA_PIDR0_PART_0_7                          (0x80UL << DMA_PIDR0_PART_0_Pos)                          /*!< 0x00000080UL*/
/*******************  Field definitions for PIDR1 register  *******************/
#define DMA_PIDR1_PART_1_Pos                        (0U)
#define DMA_PIDR1_PART_1_Msk                        (0xFUL << DMA_PIDR1_PART_1_Pos)                           /*!< 0x0000000FUL*/
#define DMA_PIDR1_PART_1                            DMA_PIDR1_PART_1_Msk                                      /*!< PART_1[ 3:0] bits Part Number [11:8].*/
#define DMA_PIDR1_PART_1_0                          (0x1UL << DMA_PIDR1_PART_1_Pos)                           /*!< 0x00000001UL*/
#define DMA_PIDR1_PART_1_1                          (0x2UL << DMA_PIDR1_PART_1_Pos)                           /*!< 0x00000002UL*/
#define DMA_PIDR1_PART_1_2                          (0x4UL << DMA_PIDR1_PART_1_Pos)                           /*!< 0x00000004UL*/
#define DMA_PIDR1_PART_1_3                          (0x8UL << DMA_PIDR1_PART_1_Pos)                           /*!< 0x00000008UL*/
#define DMA_PIDR1_DES_0_Pos                         (4U)
#define DMA_PIDR1_DES_0_Msk                         (0xFUL << DMA_PIDR1_DES_0_Pos)                            /*!< 0x000000F0UL*/
#define DMA_PIDR1_DES_0                             DMA_PIDR1_DES_0_Msk                                       /*!< DES_0[ 3:0] bits JEP106 Identity Code [3:0]*/
#define DMA_PIDR1_DES_0_0                           (0x1UL << DMA_PIDR1_DES_0_Pos)                            /*!< 0x00000010UL*/
#define DMA_PIDR1_DES_0_1                           (0x2UL << DMA_PIDR1_DES_0_Pos)                            /*!< 0x00000020UL*/
#define DMA_PIDR1_DES_0_2                           (0x4UL << DMA_PIDR1_DES_0_Pos)                            /*!< 0x00000040UL*/
#define DMA_PIDR1_DES_0_3                           (0x8UL << DMA_PIDR1_DES_0_Pos)                            /*!< 0x00000080UL*/
/*******************  Field definitions for PIDR2 register  *******************/
#define DMA_PIDR2_DES_1_Pos                         (0U)
#define DMA_PIDR2_DES_1_Msk                         (0x7UL << DMA_PIDR2_DES_1_Pos)                            /*!< 0x00000007UL*/
#define DMA_PIDR2_DES_1                             DMA_PIDR2_DES_1_Msk                                       /*!< DES_1[ 2:0] bits JEP106 Identity Code [6:4]*/
#define DMA_PIDR2_DES_1_0                           (0x1UL << DMA_PIDR2_DES_1_Pos)                            /*!< 0x00000001UL*/
#define DMA_PIDR2_DES_1_1                           (0x2UL << DMA_PIDR2_DES_1_Pos)                            /*!< 0x00000002UL*/
#define DMA_PIDR2_DES_1_2                           (0x4UL << DMA_PIDR2_DES_1_Pos)                            /*!< 0x00000004UL*/
#define DMA_PIDR2_JEDEC_Pos                         (3U)
#define DMA_PIDR2_JEDEC_Msk                         (0x1UL << DMA_PIDR2_JEDEC_Pos)                            /*!< 0x00000008UL*/
#define DMA_PIDR2_JEDEC                             DMA_PIDR2_JEDEC_Msk                                       /*!< JEDEC bit JEDEC*/
#define DMA_PIDR2_REVISION_Pos                      (4U)
#define DMA_PIDR2_REVISION_Msk                      (0xFUL << DMA_PIDR2_REVISION_Pos)                         /*!< 0x000000F0UL*/
#define DMA_PIDR2_REVISION                          DMA_PIDR2_REVISION_Msk                                    /*!< REVISION[ 3:0] bits Revision Code*/
#define DMA_PIDR2_REVISION_0                        (0x1UL << DMA_PIDR2_REVISION_Pos)                         /*!< 0x00000010UL*/
#define DMA_PIDR2_REVISION_1                        (0x2UL << DMA_PIDR2_REVISION_Pos)                         /*!< 0x00000020UL*/
#define DMA_PIDR2_REVISION_2                        (0x4UL << DMA_PIDR2_REVISION_Pos)                         /*!< 0x00000040UL*/
#define DMA_PIDR2_REVISION_3                        (0x8UL << DMA_PIDR2_REVISION_Pos)                         /*!< 0x00000080UL*/
/*******************  Field definitions for PIDR3 register  *******************/
#define DMA_PIDR3_CMOD_Pos                          (0U)
#define DMA_PIDR3_CMOD_Msk                          (0xFUL << DMA_PIDR3_CMOD_Pos)                             /*!< 0x0000000FUL*/
#define DMA_PIDR3_CMOD                              DMA_PIDR3_CMOD_Msk                                        /*!< CMOD[ 3:0] bits Customer Modified*/
#define DMA_PIDR3_CMOD_0                            (0x1UL << DMA_PIDR3_CMOD_Pos)                             /*!< 0x00000001UL*/
#define DMA_PIDR3_CMOD_1                            (0x2UL << DMA_PIDR3_CMOD_Pos)                             /*!< 0x00000002UL*/
#define DMA_PIDR3_CMOD_2                            (0x4UL << DMA_PIDR3_CMOD_Pos)                             /*!< 0x00000004UL*/
#define DMA_PIDR3_CMOD_3                            (0x8UL << DMA_PIDR3_CMOD_Pos)                             /*!< 0x00000008UL*/
#define DMA_PIDR3_REVAND_Pos                        (4U)
#define DMA_PIDR3_REVAND_Msk                        (0xFUL << DMA_PIDR3_REVAND_Pos)                           /*!< 0x000000F0UL*/
#define DMA_PIDR3_REVAND                            DMA_PIDR3_REVAND_Msk                                      /*!< REVAND[ 3:0] bits Manufacturer revision number*/
#define DMA_PIDR3_REVAND_0                          (0x1UL << DMA_PIDR3_REVAND_Pos)                           /*!< 0x00000010UL*/
#define DMA_PIDR3_REVAND_1                          (0x2UL << DMA_PIDR3_REVAND_Pos)                           /*!< 0x00000020UL*/
#define DMA_PIDR3_REVAND_2                          (0x4UL << DMA_PIDR3_REVAND_Pos)                           /*!< 0x00000040UL*/
#define DMA_PIDR3_REVAND_3                          (0x8UL << DMA_PIDR3_REVAND_Pos)                           /*!< 0x00000080UL*/
/*******************  Field definitions for CIDR0 register  *******************/
#define DMA_CIDR0_PRMBL_0_Pos                       (0U)
#define DMA_CIDR0_PRMBL_0_Msk                       (0xFFUL << DMA_CIDR0_PRMBL_0_Pos)                         /*!< 0x000000FFUL*/
#define DMA_CIDR0_PRMBL_0                           DMA_CIDR0_PRMBL_0_Msk                                     /*!< PRMBL_0[ 7:0] bits Preamble*/
#define DMA_CIDR0_PRMBL_0_0                         (0x1UL << DMA_CIDR0_PRMBL_0_Pos)                          /*!< 0x00000001UL*/
#define DMA_CIDR0_PRMBL_0_1                         (0x2UL << DMA_CIDR0_PRMBL_0_Pos)                          /*!< 0x00000002UL*/
#define DMA_CIDR0_PRMBL_0_2                         (0x4UL << DMA_CIDR0_PRMBL_0_Pos)                          /*!< 0x00000004UL*/
#define DMA_CIDR0_PRMBL_0_3                         (0x8UL << DMA_CIDR0_PRMBL_0_Pos)                          /*!< 0x00000008UL*/
#define DMA_CIDR0_PRMBL_0_4                         (0x10UL << DMA_CIDR0_PRMBL_0_Pos)                         /*!< 0x00000010UL*/
#define DMA_CIDR0_PRMBL_0_5                         (0x20UL << DMA_CIDR0_PRMBL_0_Pos)                         /*!< 0x00000020UL*/
#define DMA_CIDR0_PRMBL_0_6                         (0x40UL << DMA_CIDR0_PRMBL_0_Pos)                         /*!< 0x00000040UL*/
#define DMA_CIDR0_PRMBL_0_7                         (0x80UL << DMA_CIDR0_PRMBL_0_Pos)                         /*!< 0x00000080UL*/
/*******************  Field definitions for CIDR1 register  *******************/
#define DMA_CIDR1_PRMBL_1_Pos                       (0U)
#define DMA_CIDR1_PRMBL_1_Msk                       (0xFUL << DMA_CIDR1_PRMBL_1_Pos)                          /*!< 0x0000000FUL*/
#define DMA_CIDR1_PRMBL_1                           DMA_CIDR1_PRMBL_1_Msk                                     /*!< PRMBL_1[ 3:0] bits Preamble*/
#define DMA_CIDR1_PRMBL_1_0                         (0x1UL << DMA_CIDR1_PRMBL_1_Pos)                          /*!< 0x00000001UL*/
#define DMA_CIDR1_PRMBL_1_1                         (0x2UL << DMA_CIDR1_PRMBL_1_Pos)                          /*!< 0x00000002UL*/
#define DMA_CIDR1_PRMBL_1_2                         (0x4UL << DMA_CIDR1_PRMBL_1_Pos)                          /*!< 0x00000004UL*/
#define DMA_CIDR1_PRMBL_1_3                         (0x8UL << DMA_CIDR1_PRMBL_1_Pos)                          /*!< 0x00000008UL*/
#define DMA_CIDR1_CLASS_Pos                         (4U)
#define DMA_CIDR1_CLASS_Msk                         (0xFUL << DMA_CIDR1_CLASS_Pos)                            /*!< 0x000000F0UL*/
#define DMA_CIDR1_CLASS                             DMA_CIDR1_CLASS_Msk                                       /*!< CLASS[ 3:0] bits Component class*/
#define DMA_CIDR1_CLASS_0                           (0x1UL << DMA_CIDR1_CLASS_Pos)                            /*!< 0x00000010UL*/
#define DMA_CIDR1_CLASS_1                           (0x2UL << DMA_CIDR1_CLASS_Pos)                            /*!< 0x00000020UL*/
#define DMA_CIDR1_CLASS_2                           (0x4UL << DMA_CIDR1_CLASS_Pos)                            /*!< 0x00000040UL*/
#define DMA_CIDR1_CLASS_3                           (0x8UL << DMA_CIDR1_CLASS_Pos)                            /*!< 0x00000080UL*/
/*******************  Field definitions for CIDR2 register  *******************/
#define DMA_CIDR2_PRMBL_2_Pos                       (0U)
#define DMA_CIDR2_PRMBL_2_Msk                       (0xFFUL << DMA_CIDR2_PRMBL_2_Pos)                         /*!< 0x000000FFUL*/
#define DMA_CIDR2_PRMBL_2                           DMA_CIDR2_PRMBL_2_Msk                                     /*!< PRMBL_2[ 7:0] bits Preamble*/
#define DMA_CIDR2_PRMBL_2_0                         (0x1UL << DMA_CIDR2_PRMBL_2_Pos)                          /*!< 0x00000001UL*/
#define DMA_CIDR2_PRMBL_2_1                         (0x2UL << DMA_CIDR2_PRMBL_2_Pos)                          /*!< 0x00000002UL*/
#define DMA_CIDR2_PRMBL_2_2                         (0x4UL << DMA_CIDR2_PRMBL_2_Pos)                          /*!< 0x00000004UL*/
#define DMA_CIDR2_PRMBL_2_3                         (0x8UL << DMA_CIDR2_PRMBL_2_Pos)                          /*!< 0x00000008UL*/
#define DMA_CIDR2_PRMBL_2_4                         (0x10UL << DMA_CIDR2_PRMBL_2_Pos)                         /*!< 0x00000010UL*/
#define DMA_CIDR2_PRMBL_2_5                         (0x20UL << DMA_CIDR2_PRMBL_2_Pos)                         /*!< 0x00000020UL*/
#define DMA_CIDR2_PRMBL_2_6                         (0x40UL << DMA_CIDR2_PRMBL_2_Pos)                         /*!< 0x00000040UL*/
#define DMA_CIDR2_PRMBL_2_7                         (0x80UL << DMA_CIDR2_PRMBL_2_Pos)                         /*!< 0x00000080UL*/
/*******************  Field definitions for CIDR3 register  *******************/
#define DMA_CIDR3_PRMBL_3_Pos                       (0U)
#define DMA_CIDR3_PRMBL_3_Msk                       (0xFFUL << DMA_CIDR3_PRMBL_3_Pos)                         /*!< 0x000000FFUL*/
#define DMA_CIDR3_PRMBL_3                           DMA_CIDR3_PRMBL_3_Msk                                     /*!< PRMBL_3[ 7:0] bits Preamble*/
#define DMA_CIDR3_PRMBL_3_0                         (0x1UL << DMA_CIDR3_PRMBL_3_Pos)                          /*!< 0x00000001UL*/
#define DMA_CIDR3_PRMBL_3_1                         (0x2UL << DMA_CIDR3_PRMBL_3_Pos)                          /*!< 0x00000002UL*/
#define DMA_CIDR3_PRMBL_3_2                         (0x4UL << DMA_CIDR3_PRMBL_3_Pos)                          /*!< 0x00000004UL*/
#define DMA_CIDR3_PRMBL_3_3                         (0x8UL << DMA_CIDR3_PRMBL_3_Pos)                          /*!< 0x00000008UL*/
#define DMA_CIDR3_PRMBL_3_4                         (0x10UL << DMA_CIDR3_PRMBL_3_Pos)                         /*!< 0x00000010UL*/
#define DMA_CIDR3_PRMBL_3_5                         (0x20UL << DMA_CIDR3_PRMBL_3_Pos)                         /*!< 0x00000020UL*/
#define DMA_CIDR3_PRMBL_3_6                         (0x40UL << DMA_CIDR3_PRMBL_3_Pos)                         /*!< 0x00000040UL*/
#define DMA_CIDR3_PRMBL_3_7                         (0x80UL << DMA_CIDR3_PRMBL_3_Pos)                         /*!< 0x00000080UL*/

#endif /* __DMA350_REGDEF_H */
