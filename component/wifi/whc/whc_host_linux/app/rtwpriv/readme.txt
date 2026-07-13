===============================================================================

This tool is only for below purpose

(1)The Mass production line which need to calibrate or test the RF performance
(2)Regualtor related purpose which are like FCC/CE/MKK...etc.

Customer must remove the tool when shipping the product to end-customers.

==============================================================================

Change log:
Version			Comments
5.0 ,	Integrate mp_psd_analysis function.
5.1 ,	refine psd func for huawei, add input for adjust 256 512 1024 psd  PTS.
5.2 , 	bugfix HW tx mode switch issue.
5.3 ,	Add XW tx mode add Packet Pattern settng.
5.4 ,	Correct HX tx mode channel support over 171.
5.4.1 , Add input parameters : rtwpriv wlan0 [Channel] [Bandwidth] [ANT_PAH] [RateID] [TxMode] [Packet Interval] [PacketLength] [Packet Count] [Packet Pattern]
5.4.2 , correct the channel to support 177.
5.4.3 , support rfr and rfw ioctrl.
5.4.4 , correct HW Tx packet Pattern for input parameter.
5.4.5 , correct return string format for test tool.
5.5.0 , new feature for RWTK test tool.
5.5.2 , PSD - refine mp_psd_analysis function.
5.6.0 , new function for UDP server. CMD:rtwpriv wlan0 mp_server.
5.6.1 , fixed for Illegal instruction issue on some platform.
5.6.2 , modify default packet pktPeriod 2000 for HX Tx mode.
5.6.3 , Correct Rate index setting for HX Tx mode.
5.6.4 , Revise readme.tx document for statement.
5.7.0 , Support for 8852X AX chipset.
5.7.1 , HW TX CMD compatible for AX ICs.
5.7.3 , Modify UDP server for RTK Robot test.
5.7.4 , bugfix UDP server for Wlanlab tool reg read.
5.8.0 , add for AX suppport RU_tone/PPDU
5.8.4 , HW Tx CMD format to integrate AX (ru_tone, ru_alloc, ppdu type/ + ru_alloc).
	support 160M BW.
	solved issue: vsprintf: prevented 97-byte write into 64-byte buffer " .
5.8.5
5.8.6 , Fixed mp_rate wrong string on embedded system.
	Workaround for AX BW setting.
5.8.7 , UDP Server fixed for RF path of “mp_ther” command , and direct CMD using fork() to get output.
5.9.1 , Unlimited packet length for HW TX CMD format.
5.9.2 , HW TX CMD modify first perform CH/BW, Frey Antenna CMD get power idx correctly.
5.9.3 , modify for RF Autotest return format.
6.0.0 , support kernel's'whc_demo' Generic Netlink family instead of ioctl (SIOCDEVPRIVATE).
6.0.1 , add Makefile/Android.mk "NL_DEFAULT" build option (default off).
	When built with "make NL_DEFAULT=y", Netlink mode is enabled by default so
	the "-n" flag can be omitted: "rtwpriv wlan0 <cmd>" == "rtwpriv -n wlan0 <cmd>".
	add Netlink helper command list (getip/getmac/logon/logoff/init/dbg); bugfix:
	getmac/getip wlan_idx.

===============================================================================
 Netlink mode (Generic Netlink "whc_demo" family) - command list & usage
===============================================================================

Netlink mode sends commands to the driver through the kernel's "whc_demo"
Generic Netlink family instead of ioctl (SIOCDEVPRIVATE).

How to select Netlink mode:
  (a) Explicit flag (default build):
        rtwpriv -n wlan0 <command> [args...]
  (b) Default build option (make NL_DEFAULT=y): "-n" can be omitted:
        rtwpriv wlan0 <command> [args...]

Any command not in the helper list below is forwarded as-is over CMD_WIFI_MP
(so the usual mp_* / config_get / read / write commands keep working), e.g.:
        rtwpriv -n wlan0 mp_start
        rtwpriv -n wlan0 mp_channel 36
        rtwpriv -n wlan0 mp_bandwidth 40M=1,shortGI=0
        rtwpriv -n wlan0 mp_txpower dbm=16.00
        rtwpriv -n wlan0 mp_ant_tx a
        rtwpriv -n wlan0 mp_query
        rtwpriv -n wlan0 config_get rmap,0,256

Helper command list (parsed locally, mapped to dedicated WHC sub-commands):

  Command                       Args                Reply   Description
  ----------------------------- ------------------- ------- -----------------------------
  getmac    <wlan_idx>          0 or 1              yes     Read interface MAC address
  getip     <wlan_idx>          0 or 1              yes     Read IP / gateway
  logon                         -                   no      Enable device log
  logoff                        -                   no      Disable device log
  dbg       <cmd> [args...]     free-form string    yes     Send a CMD_WIFI_DBG string

Examples:
        rtwpriv -n wlan0 getmac 0
        rtwpriv -n wlan0 getip 0
        rtwpriv -n wlan0 logon
        rtwpriv -n wlan0 logoff
        rtwpriv -n wlan0 dbg <driver-dbg-command>

Notes:
  - Reply "yes": the tool sends the request, then waits (up to 10s) for the
    device's WHC_CMD_EVENT reply and prints it.
  - The WiFi driver (whc_demo Netlink family) must be loaded; otherwise the
    tool reports "Failed to resolve Netlink family 'whc_demo'".

A. Feature

The following steps demonstrate Realtek Wireless Adapter Linux RF HW/SW Tx Tool.

This is a simple User guide,
The rtwpriv V5 version is Integrate HW/SW Tx tool.
We use Linux utility “rtwpriv” to I/O control config WLAN driver,

B. Software Package
	1. Readme.txt
	2. Rtw_Config.txt
	3. rtwpriv.cpp/rtwpriv.h
	4. rtwpriv

C. Build the realtek RF Hw Tx tool.
	Q. How to build rtwpriv tool?
    A.[Linux]
        Just "make", and you will get executable file "rtwpriv".
			If you want to use another OS platform, please to modify the Makefile of toolchain.

D. Use Example:
1. insert the wlan.ko driver.
#insmod wlan.ko

2. enable wlan interface
#ifconfig wlan0 up

3. Enter wlan MP mode
#rtwpriv wlan0 mp_start

3. execute the rtwpriv tool to launch RF HW Tx.
 rtwpriv wlan0 [Channel] [Bandwidth] [ANT_PAH] [RateID] [TxMode] [Packet Interval] [PacketLength] [Packet Count] [Packet Pattern]
#rtwpriv wlan0 36 0 a HTMCS0 1   	//Start Tx
#rtwpriv wlan0 stop		       //Stop Tx

4. To adjust the Tx power index ,
   if you want to adjust [CONTINUOUS Tx] power, please frist to stop Tx,then do adjust power index.
#rtwpriv wlan0 mp_txpower patha=30,pathb=30,pathc=30,pathd=30

Instruction Command format:

#####[ SW TX mode ]#####
         Input the parameters like this: rtwpriv wlan0 mp_XXX XXX
        1. mp_start                             ## enter to MP Mode
        2. mp_bandwidth 40M=[Num]               ## input [Num], 0=20M, 1=40M, 2=80M
        3. mp_channel [Num]                     ## input [Num] = 1~167 channel number
        4. mp_rate [Rate Id Num] or [RateID]
        5. mp_ant_tx [Path]                     ## input Antenna [PARH] = a,b,c,d,ab,ac,ad...
        6. mp_txpower patha=[index],pathb=[index],pathc=[index],pathd=[index]   ## input power index range [index] = 1~63
        7. mp_get_txpower [PATH Num]            ## Input Antenna PATH Num 0~3 , A = 0, B = 1 , C = 2 , D = 3
        8. mp_ctx [Tx Mode]                     ##input [ Tx Mode ]
                                [a].Continuous Packet Tx = 'background,pkt'
                                [b].Continuous Tx = 'background'
                                [c].Count Packet Tx = 'count=[num],pkt'
                                [d].Carrier suppression testing = 'background,cs'
                                [e].Single Tone TX testing = 'background,stone'
           mp_ctx stop                          ## Stop Tx Mode
        9. mp_query                             ## Get Tx/Rx Packet Counter.

##### [ HW TX mode ] VHT IC Suppport Only #####
         Input the parameters like this:rtwpriv wlan0 [Channel] [Bandwidth] [ANT_PAH] [RateID] [TxMode] [Packet Interval] [PacketLength] [Packet Count] [Packet Pattern]
        [Channel]: 1~167
        [BW]: 0 = 20M, 1 = 40M, 2 = 80M
        [ANT_PAH]: a: PATH A, b: PATH B, c: PATH C, d: PATH D, ab : PATH AB 2x2 ....
        [RateID]:
1M 2M 5.5M 11M 6M 9M 12M 18M 24M 36M 48M 54M
HTMCS0 HTMCS1 HTMCS2 HTMCS3 HTMCS4 HTMCS5 HTMCS6 HTMCS7 HTMCS8 HTMCS9 HTMCS10
HTMCS11 HTMCS12 HTMCS13 HTMCS14 HTMCS15 HTMCS16 HTMCS17 HTMCS18 HTMCS19 HTMCS20 HTMCS21
HTMCS22 HTMCS23 HTMCS24 HTMCS25 HTMCS26 HTMCS27 HTMCS28 HTMCS29 HTMCS30 HTMCS31 VHT1MCS0
VHT1MCS1 VHT1MCS2 VHT1MCS3 VHT1MCS4 VHT1MCS5 VHT1MCS6 VHT1MCS7 VHT1MCS8 VHT1MCS9 VHT2MCS0 VHT2MCS1
VHT2MCS2 VHT2MCS3 VHT2MCS4 VHT2MCS5 VHT2MCS6 VHT2MCS7 VHT2MCS8 VHT2MCS9 VHT3MCS0 VHT3MCS1 VHT3MCS2
VHT3MCS3 VHT3MCS4 VHT3MCS5 VHT3MCS6 VHT3MCS7 VHT3MCS8 VHT3MCS9 VHT4MCS0 VHT4MCS1 VHT4MCS2 VHT4MCS3
VHT4MCS4 VHT4MCS5 VHT4MCS6 VHT4MCS7 VHT4MCS8 VHT4MCS9
        [ TxMode ]: 1: PACKET Tx 2:CONTINUOUS TX 3:OFDM Single Tone TX
        [PacketLength] (Option): Packet of payload data length, default 1500
        [Packet Count] (Option): count the number of packet to Tx , set 0 for CONTINUOUS Packet TX ,default 0
        [Packet Interval] (Option): 1~65535 us,default 2000
        [Packet Pattern] (Option): 00~ff(hex) ,default random hex


#####[ RX mode ] #####
        1. mp_start                             ## enter to MP Mode
        2. mp_bandwidth 40M=[Num]               ## input [Num], 0=20M, 1=40M, 2=80M
        3. mp_channel [Num]                     ## input [Num] = 1~167 channel number
        5. mp_ant_rx [Path]                     ## input Antenna [PARH] = a,b,c,d,ab,ac,ad...
        6. mp_arx start                         ## Enter Rx packet mode
        7. mp_arx phy                           ## Query Rx Phy Packet Count
        8. mp_reset_stats                       ## reset all Count


#####[ Read/Write RF reg ] #####
        1. rfr [RF_Path] [Reg_Addr]  ,return hex value, # RF Path= 0/1/2/3 , Reg Addr hex = 0xXX.
        2. rfw [RF_Path] [Reg_Addr] [Value] ,# RF Path= 0/1/2/3 , Reg Addr hex = 0xXX , Value Hex = 0x00000 .
        3. read_rf [RF_Path],[reg_Addr] ,return decimal value ,# RF Path= 0/1/2/3 , Reg Addr hex = 0xXX .
        4. write_rf [RF_Path],[Reg_Addr],[Value] ,# RF Path= 0/1/2/3 , Reg Addr hex = 0xXX , Value Hex = 0x00000 .

#####[ Read/Write mac/bb reg ] #####
        1. read [data width],[Reg_Addr]  ,return hex value, # Data Width= 1/2/4 , Reg Addr hex = 0xXX .
        2. write [data width],[Reg_Add],[Value] ,# Data Width= 1/2/4 , Reg Addr hex = 0xXX , Value Hex = 0x00000 .


4. How to get tool version number
#./rtwpriv -v
rtwpriv version:V5.X.X_20180307

5. How to get CMD help
#./rtwpriv
