import os
import sys

TEMPLATE_CORE = """/*********************************************************************
*  Auto Generated J-Link Script for %CORE%
*********************************************************************/

void InitTarget(void) {
  Report("******************************************************");
  Report("Auto Generated J-Link Script: %CORE%");
  Report("******************************************************");

  // Set AP info for all APs in the device
%AP_CONFIG%

  CORESIGHT_IndexAHBAPToUse = %AP_INDEX%;
  JTAG_WriteClocks(1);                // Make sure that JTAG buffers are empty
  JTAG_Speed = 1000;

  JLINK_MemRegion("0xEFFF0000-0xFFFFFFFF X");
}
"""

TEMPLATE_CA32 = """/*********************************************************************
*  Auto Generated J-Link Script for %CORE% (Cortex-A32)
*********************************************************************/

/*********************************************************************
*
*             Constants (similar to defines)
*
**********************************************************************
*/
__constant U32 _APB_ADDR_DBGREGS_A32_0                     = 0x80030000;
__constant U32 _APB_ADDR_DBGREGS_A32_1                     = 0x80032000;

__constant U32 _APB_ADDR_CTIREGS_A32_0                     = 0x80038000;
__constant U32 _APB_ADDR_CTIREGS_A32_1                     = 0x80039000;

//
// USER CONFIGURABLE
//
U32 _Index_APCA32 = %AP_INDEX%;
U32 _Index_CORE = %CORE_INDEX%;
U32 _Addr_DBGREGS = %DBG_REG%;
U32 _Addr_CTIREGS = %CTI_REG%;
const char* _sExecCTIBaseAddr = "CORESIGHT_SetCSCTICoreBaseAddr=%CTI_BASE%";

/*CPU Debug Architecture*/
const U32 DBGDTRRX_EL0_OFFSET           = 0x080;
const U32 EDITR_OFFSET                  = 0x084;
const U32 EDSCR_OFFSET                  = 0x088;
const U32 DBGDTRTX_EL0_OFFSET           = 0x08C;
const U32 EDRCR_OFFSET                  = 0x090;
const U32 OSLAR_EL1_OFFSET              = 0x300;
const U32 EDPRCR_OFFSET                 = 0x310;
const U32 EDPRSR_OFFSET                 = 0x314;
const U32 EDLAR_OFFSET                  = 0xFB0;

const U32 CTICONTROL_OFFSET             = 0x000;
const U32 CTIINTACK_OFFSET              = 0x010;
const U32 CTIAPPPULSE_OFFSET            = 0x01C;
const U32 CTIINEN0_OFFSET               = 0x020;
const U32 CTIOUTEN0_OFFSET              = 0x0A0;
const U32 CTIGATE_OFFSET                = 0x140;
const U32 CTILAR_OFFSET                 = 0xFB0;


/*********************************************************************
*
*             _WriteAPReg
*
*    Function description
*        Writes an AP register. Depends on the previous command (DPACC / APACC)
*/
void _WriteAPReg(unsigned int Address, unsigned int Data) {
    JLINK_CORESIGHT_WriteAP(1, Address);
    JLINK_CORESIGHT_WriteAP(3, Data);
}

/*********************************************************************
*
*             _ReadAPReg
*
*    Function description
*        Read an AP register. Depends on the previous command (DPACC / APACC)
*/
int _ReadAPReg(unsigned int Address) {
    int v;
    JLINK_CORESIGHT_WriteAP(1, Address);
    JLINK_CORESIGHT_ReadAP(3);

    v = JLINK_CORESIGHT_ReadDP(3);
    return v;
}

void HaltCPUAPBAP(void) {
    int v, timeout;
    timeout = 50;
    /*if Cortex-A CPU access target memory, then CPU should be in debug mode!!!*/
    _WriteAPReg(_Addr_DBGREGS + OSLAR_EL1_OFFSET, 0x0);
    _WriteAPReg(_Addr_DBGREGS + EDLAR_OFFSET, 0xC5ACCE55);

    v = _ReadAPReg(_Addr_DBGREGS + EDPRSR_OFFSET);
    if(v & (1 << 4)) {
        JLINK_SYS_Report1("    Target is in Debug mode: ", _Index_CORE);
        return;
    }

    v = _ReadAPReg(_Addr_DBGREGS + EDSCR_OFFSET);
    v = v | (1 << 14) & 0xFFFFFFE0;
    _WriteAPReg(_Addr_DBGREGS + EDSCR_OFFSET, v);

    _WriteAPReg(_Addr_CTIREGS + CTILAR_OFFSET, 0xC5ACCE55);
    _WriteAPReg(_Addr_CTIREGS + CTICONTROL_OFFSET, 0x1);

    v = _ReadAPReg(_Addr_CTIREGS + CTIGATE_OFFSET);
    v = (v & 0xC) | 0x0;
    _WriteAPReg(_Addr_CTIREGS + CTIGATE_OFFSET, v);

    _WriteAPReg(_Addr_CTIREGS + CTIOUTEN0_OFFSET, 1);
    _WriteAPReg(_Addr_CTIREGS + CTIAPPPULSE_OFFSET, 1);
    _WriteAPReg(_Addr_CTIREGS + CTIINTACK_OFFSET, 1);
    _WriteAPReg(_Addr_CTIREGS + CTIINTACK_OFFSET, 0);

    do {
        if(timeout == 0) {
            JLINK_SYS_Report1("    Target Failed to enter Debug mode: ", _Index_CORE);
            return;
        }
        timeout = timeout - 1;

        v = _ReadAPReg(_Addr_DBGREGS + EDSCR_OFFSET);
    } while((v & (1 << 14)) == 0);

    JLINK_SYS_Report1("    Target Succed to enter Debug mode: ", _Index_CORE);
}

void WriteInstruction(U32 Dbg_Addr, U32 instruction)
{
    int v;
    /*write instruction*/
    _WriteAPReg(Dbg_Addr + EDITR_OFFSET, instruction);

    /*poll until the instruction has been executed*/
    do {
        v = _ReadAPReg(Dbg_Addr + EDSCR_OFFSET);
    } while((v & 0x01000000) != 0x01000000);
}

int ReadApbAPMem_ARMV7AR(U32 Address)
{
    int v;

    /*if Cortex-A CPU access target memory, then CPU should be in debug mode!!!*/
    /*Enable DSCR.ITRen*/
    v = _ReadAPReg(_Addr_DBGREGS + EDSCR_OFFSET);
    _WriteAPReg(_Addr_DBGREGS + EDSCR_OFFSET, (v | (1 << 13)));

    /*write Address to DTRRX*/
    _WriteAPReg(_Addr_DBGREGS + DBGDTRRX_EL0_OFFSET, Address);
    /*MRC p14, #0x0, r1, c0, c5, #0*/
    WriteInstruction(_Addr_DBGREGS, 0xee101e15);

    /*ldr r0, [r1]*/
    WriteInstruction(_Addr_DBGREGS, 0xe5910000);

    /*MCR p14, #0x0, r0, c0, c5, #0*/
    WriteInstruction(_Addr_DBGREGS, 0xee000e15);

    do {
        v = _ReadAPReg(_Addr_DBGREGS + EDSCR_OFFSET);
        v = v >> 29;
    } while (v == 0);
    v = _ReadAPReg(_Addr_DBGREGS + DBGDTRTX_EL0_OFFSET);
    JLINK_SYS_Report1(" Read Target Memory Address: ", Address);
    JLINK_SYS_Report1(" = ", v);
    return v;
}

int ReadApbAPMem_AARCH64(U32 Address)
{
    int v;

    /*if Cortex-A CPU access target memory, then CPU should be in debug mode!!!*/
    /*write Addr[63:32] to DTRTX*/
    _WriteAPReg(_Addr_DBGREGS + DBGDTRTX_EL0_OFFSET, Address >> 32);
    /*write Addr[31:0] to DTRRX*/
    _WriteAPReg(_Addr_DBGREGS + DBGDTRRX_EL0_OFFSET, Address & 0xFFFFFFFF);

    /*MRS X0, DTR*/
    WriteInstruction(_Addr_DBGREGS, 0xd5330400);
    /*LDR W1, [X0]*/
    WriteInstruction(_Addr_DBGREGS, 0xb8400001);
    /*MSR W1, DTRRX*/
    WriteInstruction(_Addr_DBGREGS, 0xd5130501);

    do {
        v = _ReadAPReg(_Addr_DBGREGS + EDSCR_OFFSET);
        v = v >> 29;
    } while (v == 0);
    v = _ReadAPReg(_Addr_DBGREGS + DBGDTRTX_EL0_OFFSET);
    JLINK_SYS_Report1(" Read Target Memory Address: ", Address);
    JLINK_SYS_Report1(" = ", v);
    return v;
}

int ReadApbAPMem(U32 Address)
{
    int v;

    /*if Cortex-A CPU access target memory, then CPU should be in debug mode!!!*/
    /*write Address to DTRRX*/
    _WriteAPReg(_Addr_DBGREGS + DBGDTRRX_EL0_OFFSET, Address);

    /*MRC p14, #0x0, r1, c0, c5, #0*/
    WriteInstruction(_Addr_DBGREGS, 0x1e15ee10);

    /*ldr r0, [r1]*/
    //WriteInstruction(_Addr_DBGREGS, 0x1b00f840);
    WriteInstruction(_Addr_DBGREGS, 0x0b00f851);
    //WriteInstruction(_Addr_DBGREGS, 0x0000e591);

    /*MCR p14, #0x0, r0, c0, c5, #0*/
    WriteInstruction(_Addr_DBGREGS, 0x0e15ee00);

    do {
        v = _ReadAPReg(_Addr_DBGREGS + EDSCR_OFFSET);
        v = v >> 29;
    } while (v == 0);
    v = _ReadAPReg(_Addr_DBGREGS + DBGDTRTX_EL0_OFFSET);
    JLINK_SYS_Report1(" Read Target Memory Address: ", Address);
    JLINK_SYS_Report1(" = ", v);
    return v;
}

void WriteApbAPMem_ARMV7AR(U32 Address, U32 Data)
{
    int v;

    JLINK_SYS_Report1(" Write Target Memory Address: ", Address);
    JLINK_SYS_Report1(" = ", Data);

    /*if Cortex-A CPU access target memory, then CPU should be in debug mode!!!*/
    /*Enable DSCR.ITRen*/
    v = _ReadAPReg(_Addr_DBGREGS + EDSCR_OFFSET);
    _WriteAPReg(_Addr_DBGREGS + EDSCR_OFFSET, (v | (1 << 13)));

    /*Write Address to DTRRX*/
    _WriteAPReg(_Addr_DBGREGS + DBGDTRRX_EL0_OFFSET, Address);
    /*MRC p14, #0x0, r1, c0, c5, #0*/
    WriteInstruction(_Addr_DBGREGS, 0xee101e15);
    /*Write Data to DTRRX*/
    _WriteAPReg(_Addr_DBGREGS + DBGDTRRX_EL0_OFFSET, Data);
    /*MRC p14, #0x0, r1, c0, c5, #0*/
    WriteInstruction(_Addr_DBGREGS, 0xee1000e15);

    /*STR r0, [r1]*/
    WriteInstruction(_Addr_DBGREGS, 0xe5810000);
}

void WriteApbAPMem_AARCH64(U32 Address, U32 Data)
{
    JLINK_SYS_Report1(" Write Target Memory Address: ", Address);
    JLINK_SYS_Report1(" = ", Data);

    /*if Cortex-A CPU access target memory, then CPU should be in debug mode!!!*/
    /*write Addr[63:32] to DTRTX*/
    _WriteAPReg(_Addr_DBGREGS + DBGDTRTX_EL0_OFFSET, Address >> 32);
    /*write Addr[31:0] to DTRRX*/
    _WriteAPReg(_Addr_DBGREGS + DBGDTRRX_EL0_OFFSET, Address & 0xFFFFFFFF);

    /*MRS X0, DTR*/
    WriteInstruction(_Addr_DBGREGS, 0xd5330400);
    /*Write Data to DTRRX*/
    _WriteAPReg(_Addr_DBGREGS + DBGDTRRX_EL0_OFFSET, Data);
    /*MRS W1, DTRRX*/
    WriteInstruction(_Addr_DBGREGS, 0xd5330501);

    /*STR W1, [X0]*/
    WriteInstruction(_Addr_DBGREGS, 0xb8000001);
}

void WriteApbAPMem(U32 Address, U32 Data)
{
    JLINK_SYS_Report1(" Write Target Memory Address: ", Address);
    JLINK_SYS_Report1(" = ", Data);

    /*if Cortex-A CPU access target memory, then CPU should be in debug mode!!!*/
    /*write Address to DTRRX*/
    _WriteAPReg(_Addr_DBGREGS + DBGDTRRX_EL0_OFFSET, Address);

    /*MRC p14, #0x0, r1, c0, c5, #0*/
    WriteInstruction(_Addr_DBGREGS, 0x1e15ee10);
    /*Write Data to DTRRX*/
    _WriteAPReg(_Addr_DBGREGS + DBGDTRRX_EL0_OFFSET, Data);
    /*MRC p14, #0x0, r0, c0, c5, #0*/
    WriteInstruction(_Addr_DBGREGS, 0x0e15ee10);

    /*STR r0, [r1]*/
    //WriteInstruction(_Addr_DBGREGS, 0x1b00f840);
    WriteInstruction(_Addr_DBGREGS, 0x0b00f841);
    //WriteInstruction(_Addr_DBGREGS, 0x0000e581);
}

void CTIIRQTest(void) {
    Report("******************************************************");
    Report("J-Link script: CTIIRQTest()");
    Report("******************************************************");

    _WriteAPReg(_APB_ADDR_CTIREGS_A32_0 + 0xFB0, 0xC5ACCE55); //LAR unlock 0xC5ACCE55 Clear the software lock.
    _WriteAPReg(_APB_ADDR_CTIREGS_A32_0 + 0x000, 0x1);        //CTICONTROL.CTIEN
    _WriteAPReg(_APB_ADDR_CTIREGS_A32_0 + 0x140, 0xF);        //CTIGATE.CTIGATEEN

    _WriteAPReg(_APB_ADDR_CTIREGS_A32_1 + 0xFB0, 0xC5ACCE55); //LAR unlock 0xC5ACCE55 Clear the software lock.
    _WriteAPReg(_APB_ADDR_CTIREGS_A32_1 + 0x000, 0x1);        //CTICONTROL.CTIEN
    _WriteAPReg(_APB_ADDR_CTIREGS_A32_1 + 0x140, 0xF);        //CTIGATE.CTIGATEEN

    /*ch2 ==== core0/1 CTIIRQ Test*/
    _WriteAPReg(_APB_ADDR_CTIREGS_A32_0 + 0x0A8, 4);         //CTIOUTEN2 enable Channel 2 to Trigger output 2(CTIIRQ) @ CTI 0
    _WriteAPReg(_APB_ADDR_CTIREGS_A32_1 + 0x0A8, 4);         //CTIOUTEN2 enable Channel 2 to Trigger output 2(CTIIRQ) @ CTI 1
    _WriteAPReg(_APB_ADDR_CTIREGS_A32_0 + 0x01C, 4);         //CTIAPPPULSE Pulse channel 2 event for one clk cycl
}

void APWarmResetTest(void) {
    int v;
    Report("******************************************************");
    Report("J-Link script: APWarmResetTest()");
    Report("******************************************************");

    v = _ReadAPReg(_Addr_DBGREGS + EDPRSR_OFFSET);
    v = _ReadAPReg(_Addr_DBGREGS + EDPRSR_OFFSET);
    JLINK_SYS_Report1("EDPRSR_OFFSET: ", v);
    if(v == 0x0D) {
        return;
    }

    HaltCPUAPBAP();
    WriteApbAPMem(0x4200823C, 0x96969696);     //Write 0x96969696 to enable the write of reset control
    WriteApbAPMem(0x42008238, 0x1);            //Write Bit[0] to enable reset other cpu's core and local cpu
    // WriteApbAPMem(0x42008238, 0xC0000001);     //Write Bit[31] to trigger apsys_rst
    // WriteApbAPMem(0x4200823C, 0x69696969);     //Write 0x69696969 will trigger the system if REG_SW_RST_CTRL set

    _WriteAPReg(_Addr_DBGREGS + OSLAR_EL1_OFFSET, 0x0);
    _WriteAPReg(_Addr_DBGREGS + EDLAR_OFFSET, 0xC5ACCE55);
    _WriteAPReg(_Addr_DBGREGS + EDPRCR_OFFSET, 0x1);        //EDPRCR.CORENPDRQ

    //The PE ignores writes to this bit if any of the following are true:
    //* ExternalSecureInvasiveDebugEnabled() == FALSE
    //* The Core power domain is either off or in a low-power state where the Core power domain registers cannot be accessed.
    //* DoubleLockStatus() == TRUE (DBGOSDLR.DLK == '1' && DBGPRCR.CORENPDRQ == '0' && !Halted())
    //* OSLSR.OSLK == 1 (OS lock is locked).
    _WriteAPReg(_Addr_DBGREGS + EDPRCR_OFFSET, 0x2);        //EDPRCR.CWRR

    JLINK_SYS_Sleep(200);                                    //wait 200ms to check CA32 status.
}

/*********************************************************************
*
*             ResetTarget
*/
void ResetTarget(void) {
    int v;
    int Speed;
    int Ctrl;

    Report("******************************************************");
    Report("J-Link script: ResetTarget()");
    Report("******************************************************");
    Speed = JTAG_Speed;
    JTAG_Speed = 100;
    JTAG_WriteClocks(1);

    //
    // Select APB-AP and prepare control register
    //
    JLINK_CORESIGHT_WriteDP(2, (_Index_APCA32 << 24) | (0 << 4));    // Select AP[x], bank 0
    Ctrl =    0
             | (2 << 0)        // AP-access size. Fixed to 2: 32-bit
             | (1 << 4)        // Auto increment TAR after read/write access. Increment is NOT performed on access to banked data registers 0-3.
             | (1 << 31)       // Enable software access to the Debug APB bus.
             ;
    JLINK_CORESIGHT_WriteAP(0, Ctrl);
    //
    // Perform some other init steps which are required to get full control of the debug logic
    //
    _WriteAPReg(_Addr_DBGREGS + OSLAR_EL1_OFFSET, 0x0);
    _WriteAPReg(_Addr_DBGREGS + EDLAR_OFFSET, 0xC5ACCE55);
    _WriteAPReg(_Addr_DBGREGS + EDPRCR_OFFSET, 0x1);        //EDPRCR.CORENPDRQ
    v = _ReadAPReg(_Addr_DBGREGS + EDPRSR_OFFSET);
    JLINK_SYS_Report1("EDPRSR: ", v);

    //
    // Read & modify DSCR in order to enable debug halt mode
    //
    v = _ReadAPReg(_Addr_DBGREGS + EDSCR_OFFSET);
    v |= (1 << 14);
    _WriteAPReg(_Addr_DBGREGS + EDSCR_OFFSET, v);           // Enable debug halt mode by writing the EDSCR

    //
    // Halt CPU by writing the halt request bit in the DRCR
    //
    //_WriteAPReg(_Addr_DBGREGS + EDRCR_OFFSET, 1);

    JTAG_WriteClocks(1);
    JTAG_Speed = Speed;
}

/*********************************************************************
*
*             InitTarget
*
*    Notes
*        (1) High-level functions like MEM_xxx functions must not be used here.
*                They may only be used in SetupTarget() which is called later in the connect process.
*/
void InitTarget(void) {
    Report("******************************************************");
    Report("J-Link script: Auto Generated for %CORE% (Cortex-A32)");
    Report("******************************************************");
    if (MAIN_ActiveTIF == JLINK_TIF_JTAG) {
        JLINK_CORESIGHT_Configure("IRPre=0;DRPre=0;IRPost=0;DRPost=0;IRLenDevice=4");
        JTAG_AllowTAPReset = 1;                     // J-Link is allowed to use a TAP reset for JTAG-chain auto-detection
        JTAG_SetDeviceId(0, 0x4BA00477);    // 4-bits IRLen
    } else {
        JLINK_CORESIGHT_Configure("");        // Perform SWD init sequence
    }
    //
    // For the AmebaSmart there is 1 JTAG/SWD device
    // For Cortex-A devices: Each core has its own set of debug registers in the APB-AP memory map
    // To connect to a specific core, just tell J-Link which debug registers to use
    // NOTE: The selected core *must* be powered and enabled when leaving this function (InitTarget())
    //
    // For AmebaSmart:
    // 0x80030000: Debug register Cortex-A32 core0
    // 0x80032000: Debug register Cortex-A32 core1
    //

    // Set AP info for all APs in the device
    //
%AP_CONFIG%

    JLINK_CPU                                             = CORTEX_A32;
    JLINK_CORESIGHT_IndexAPBAPToUse = _Index_APCA32;        // AP used as debug interface between J-Link and core to be debugged
    JLINK_CORESIGHT_CoreBaseAddr        = _Addr_DBGREGS;        // Specify Coresight Core Debug Register address to use.
    JLINK_ExecCommand(_sExecCTIBaseAddr);     // Specify CoreSight CTI address to use.

    JTAG_WriteClocks(1);                                // Make sure that JTAG buffers are empty
    JTAG_Speed = 1000;

    ResetTarget();    // Reset and Halt Core

    //CTIIRQTest();
    //APWarmResetTest();
}
"""

def gen_jlink_script(core, config, output_file):
    # Get AP configurations
    ap_add = config.get('AP_ADD', {})
    ap_use = config.get('AP_USE', {})

    if core not in ap_use:
         print(f"Error: AP config for '{core}' not found")
         sys.exit(1)

    ap_index = ap_use[core]

    add_ap_lines = []
    for idx_str, type_str in ap_add.items():
        ap_type = "CORESIGHT_AHB_AP" if type_str == "AHB" else "CORESIGHT_APB_AP"
        add_ap_lines.append(f"\tJLINK_CORESIGHT_AddAP({idx_str}, {ap_type});")
    add_ap_block = "\n".join(add_ap_lines)

    # Check if core is ca32 based
    if core.startswith("ca32"):
        core_idx = 0 if "0" in core else 1

        dbg_reg = f"_APB_ADDR_DBGREGS_A32_{core_idx}"
        cti_reg = f"_APB_ADDR_CTIREGS_A32_{core_idx}"
        cti_base_val = "0x80038000" if core_idx == 0 else "0x80039000"

        content = TEMPLATE_CA32.replace("%CORE%", core.upper())
        content = content.replace("%AP_CONFIG%", add_ap_block)
        content = content.replace("%AP_INDEX%", str(ap_index))
        content = content.replace("%CORE_INDEX%", str(core_idx))
        content = content.replace("%DBG_REG%", dbg_reg)
        content = content.replace("%CTI_REG%", cti_reg)
        content = content.replace("%CTI_BASE%", cti_base_val)
    else:
        # Fill standard template
        content = TEMPLATE_CORE.replace("%CORE%", core.upper())
        content = content.replace("%AP_CONFIG%", add_ap_block)
        content = content.replace("%AP_INDEX%", str(ap_index))

    # Write file
    try:
        with open(output_file, 'w') as f:
            f.write(content)
        print(f"\nGenerate '{output_file}' successfully.")
    except IOError as e:
        print(f"Error writing JLinkScript: {e}")
        sys.exit(1)



TEMPLATE_SWD_USER = """/*********************************************************************
*  Auto Generated J-Link Script for SWDPWD
*********************************************************************/

U32 PASSWORD_0 = 0x11223344;
U32 PASSWORD_1 = 0x55667788;
U32 PASSWORD_2 = 0x99aabbcc;
U32 PASSWORD_3 = 0xddeeff00;

/*********************************************************************
*
*       ConfigTargetSettings
*/
int ConfigTargetSettings(void) {
    CORESIGHT_AHBAPCSWDefaultSettings = 0x03000002; // default secure if SPIDEN = 1
    //CORESIGHT_AHBAPCSWDefaultSettings = 0x43000002; // default nonsecure if SPIDEN = 0
}

/*********************************************************************
*
*       SetupTarget
*/
void SetupTarget(void) {
    Report("J-Link script: SetupTarget()");
    //MEM_WriteU32(0xE6151010, 0x00000002);
	//MEM_WriteU32(0xE6160044, 0x5A5A0003);
}

/*********************************************************************
*       SWDPWD_Read_ID
*/
void SWDPWD_READ_ID(void) {
    Report("J-Link script: SWD_READ_ID");

    int tmp;
    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_ADDR, 0x0020);
    tmp=JLINK_CORESIGHT_ReadAP(JLINK_CORESIGHT_AP_REG_DATA);
    Report1("SWD ID:",tmp);
}

/*********************************************************************
*       SWDPWD_Password_Write
*/
void SWDPWD_Password_Write(void) {
    Report("J-Link script: SWDPWD_Password_Write()");

    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_ADDR, 0x0004); // target address of PASSWORD
    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_DATA, PASSWORD_0);
    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_ADDR, 0x0008); // target address of PASSWORD
    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_DATA, PASSWORD_1);
    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_ADDR, 0x000C); // target address of PASSWORD
    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_DATA, PASSWORD_2);
    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_ADDR, 0x0010); // target address of PASSWORD
    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_DATA, PASSWORD_3);
}

/*********************************************************************
*       SWDPWD_Compare_Enable
*/
int SWDPWD_Compare_Enable(void) {
    int tmp;
    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_ADDR, 0x0000); // PASSWORD compare enable
    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_DATA, (1<<1));

    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_ADDR, 0x0000);
    tmp=JLINK_CORESIGHT_ReadAP(JLINK_CORESIGHT_AP_REG_DATA); //automatically read twice, user will always see valid data.
    Report1("SWD_PRTET_CTRL:",tmp);

    while(tmp&(1<<1)){
        Report("Waiting HW Loading and Compare...");
        JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_ADDR, 0x0000);
        tmp=JLINK_CORESIGHT_ReadAP(JLINK_CORESIGHT_AP_REG_DATA);
    }

    if (tmp&(1<<3)) {
        Report("SWD Password right!");
        return 0;
    } else {
		Report("SWD Password wrong!");

		/* try to set ctrl reg RO bit3 */
		JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_ADDR, 0x0000);
		JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_DATA, (1<<3));

		JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_ADDR, 0x0000);
		tmp=JLINK_CORESIGHT_ReadAP(JLINK_CORESIGHT_AP_REG_DATA);
		if (tmp&(1<<3)) {
			Report1("SWD_PRTET_CTRL :",tmp);
			Report("--------------compare result is changed, error!!!--------------");
		}
	}

	if (tmp&(1<<4)){
		Report("SWD Password locked!");
		return 2;
	}
    return 1;
}

/*********************************************************************
*
*       ResetTarget
*/
void ResetTarget(void) {
    int v;
    int Speed;
    int Ctrl;
    int result;

    Report("*******************************************************");
    Report("J-Link script: ResetTarget()");
    Report("*******************************************************");
    Speed = JTAG_Speed;
    JTAG_Speed = 3000;
    JTAG_WriteClocks(1);

    // ----------------------------------------------------
    // STEP 1: Select CTRL_AP(AP%SECURE_AP_IDX%) and set authentication interface
    // Set SYSPWRUPREQ and DBGPWRUPREQ and clear STICKYORUN
    // ----------------------------------------------------
    JLINK_CORESIGHT_WriteDP(JLINK_CORESIGHT_DP_REG_SELECT, (%SECURE_AP_IDX% << 24) | (0 << 4));
    Ctrl = 0x23800052;  // AHB-AP.CSW  //NO PWD compare enable
    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_CTRL, Ctrl);

    Report("****************** SWD ID ******************");
    SWDPWD_READ_ID();

    Report("****************** SWDPWD_Write_Password ******************");
    SWDPWD_Password_Write();
    result = 1;
    result = SWDPWD_Compare_Enable();

    if (result != 0) {
		Report("NOTE: Wrong Password, please reset PASSWORD_0/1/2/3 and try again");
		if (result == 2) {
			Report("WARNNING: Wrong Password tried 4 times, SWDPWD locked! please reset PASSWORD_0/1/2/3 and try again");
			Report("WARNNING: please reset the system and input new password to PASSWORD_0/1/2/3 and try again!");
		}
		return;
	}

    // ----------------------------------------------------
    // STEP 2: Select CPU AP(AP%CPU_AP_IDX%) to Check Connection
     // ----------------------------------------------------
    JLINK_CORESIGHT_WriteDP(JLINK_CORESIGHT_DP_REG_SELECT, (%CPU_AP_IDX% << 24) | (0 << 4));
    Ctrl = 0x23800052;
    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_CTRL, Ctrl);
    JLINK_CORESIGHT_WriteAP(JLINK_CORESIGHT_AP_REG_ADDR, 0xE000ED00);
    v = JLINK_CORESIGHT_ReadAP(JLINK_CORESIGHT_AP_REG_DATA);
    Report1("CPUID is: ",v);
    if(v == 0xFFFFFFFF) {
		Report("ERROR: Right Password Shall Access CPUID!");
	}
}

/*********************************************************************
*
*       InitTarget
*
*  Notes
*    (1) High-level functions like MEM_xxx functions must not be used here.
*        They may only be used in SetupTarget() which is called later in the connect process.
*/
void InitTarget(void) {
    Report("J-Link script");

    // Manually configured AP map
%ADD_AP_BLOCK%

    CORESIGHT_IndexAHBAPToUse = %INIT_AP_IDX%; // <= where the target CPU locates

    JTAG_WriteClocks(1);                       // Make sure that JTAG buffers are empty
    JTAG_Speed = 1000;
    ResetTarget();                             // Reset and Halt Core
}
"""
def gen_swd_script(config, output_file):
    # get AP config
    ap_add = config['AP_ADD']
    ap_swd_cfg = config['AP_USE']['swd']
    # swd: [2, 3, 2] -> [init_idx, secure_idx, cpu_idx]
    init_ap_idx = ap_swd_cfg[0]
    secure_ap_idx = ap_swd_cfg[1]
    cpu_ap_idx = ap_swd_cfg[2]

    add_ap_lines = []
    for idx_str, type_str in ap_add.items():
        ap_type = "CORESIGHT_AHB_AP" if type_str == "AHB" else "CORESIGHT_APB_AP"
        add_ap_lines.append(f"\tCORESIGHT_AddAP({idx_str}, {ap_type});")
    add_ap_block = "\n".join(add_ap_lines)

    # Fill template
    content = TEMPLATE_SWD_USER.replace("%ADD_AP_BLOCK%", add_ap_block)
    content = content.replace("%INIT_AP_IDX%", str(init_ap_idx))
    content = content.replace("%SECURE_AP_IDX%", str(secure_ap_idx))
    content = content.replace("%CPU_AP_IDX%", str(cpu_ap_idx))

    # 4. Write file
    try:
        with open(output_file, 'w') as f:
            f.write(content)
        print(f"\nGenerate '{output_file}' successfully.")
    except IOError as e:
        print(f"Error writing JLinkScript: {e}")
        sys.exit(1)


OZONE_TEMPLATE = """/*********************************************************************
*  Auto Generated Ozone Debug J-Link Script for %CORE%
*********************************************************************/

void ConfigTargetSettings(void) {
%AP_CONFIG%

  JLINK_CORESIGHT_IndexAHBAPToUse = %AP_INDEX%;

  JTAG_WriteClocks(1);
  JTAG_Speed = 1000;
}
"""

def gen_ozone_script(core, config, output_file):
    # Get AP configurations
    ap_add = config.get('AP_ADD', {})
    ap_use = config.get('AP_USE', {})

    if core not in ap_use:
         print(f"Error: AP config for '{core}' not found")
         sys.exit(1)

    ap_index = ap_use[core]

    add_ap_lines = []
    for idx_str, type_str in ap_add.items():
        ap_type = "CORESIGHT_AHB_AP" if type_str == "AHB" else "CORESIGHT_APB_AP"
        add_ap_lines.append(f"  JLINK_CORESIGHT_AddAP({idx_str}, {ap_type});")
    add_ap_block = "\n".join(add_ap_lines)

    # Fill template
    content = OZONE_TEMPLATE.replace("%CORE%", core.upper())
    content = content.replace("%AP_CONFIG%", add_ap_block)
    content = content.replace("%AP_INDEX%", str(ap_index))

    # Write file
    try:
        with open(output_file, 'w') as f:
            f.write(content)
        print(f"\nGenerate '{output_file}' successfully.")
    except IOError as e:
        print(f"Error writing JLinkScript: {e}")
        sys.exit(1)

