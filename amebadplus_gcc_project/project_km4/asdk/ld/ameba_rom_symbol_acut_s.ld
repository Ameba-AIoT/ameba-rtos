SECTIONS
{
    PROVIDE(__vectors_table = 0x0);
    PROVIDE(__rom_xo_start__ = 0x140);
    PROVIDE(CRYPTO_MemDump = 0x141);
    PROVIDE(CRYPTO_SetSrcDesc = 0x269);
    PROVIDE(CRYPTO_SetDstDesc = 0x2f9);
    PROVIDE(CRYPTO_CleanCmdOk = 0x389);
    PROVIDE(CRYPTO_ClearAllINT = 0x3d5);
    PROVIDE(CRYPTO_SetCmdBuf = 0x41d);
    PROVIDE(CRYPTO_SrcDescKeyIv = 0x6b1);
    PROVIDE(CRYPTO_Reset = 0xb41);
    PROVIDE(CRYPTO_SHA_Reset = 0xb8d);
    PROVIDE(CRYPTO_SetSecurityModeAD = 0xbdd);
    PROVIDE(CRYPTO_Init = 0xdad);
    PROVIDE(CRYPTO_SHA_Init = 0xdd5);
    PROVIDE(CRYPTO_OTPKey_Init = 0xdfd);
    PROVIDE(CRYPTO_OTPKey_SHA_Init = 0xe61);
    PROVIDE(CRYPTO_ProcessAD = 0xee1);
    PROVIDE(CRYPTO_SendSeqBuf = 0x1739);
    PROVIDE(CRYPTO_CipherInit = 0x1849);
    PROVIDE(CRYPTO_CipherEncryptAD = 0x186d);
    PROVIDE(CRYPTO_CipherDecryptAD = 0x18a1);
    PROVIDE(SBOOT_SwapData = 0x18d9);
    PROVIDE(SBOOT_GetMdType = 0x18f5);
    PROVIDE(SBOOT_EDDSA_Verify_SW = 0x1981);
    PROVIDE(SBOOT_Validate_Algorithm = 0x199d);
    PROVIDE(SBOOT_Validate_Signature = 0x19e1);
    PROVIDE(SBOOT_Validate_PubKey = 0x1a05);
    PROVIDE(SBOOT_Validate_ImgHash = 0x1a75);
    PROVIDE(SBOOT_SignatureVerify = 0x1bf1);
    PROVIDE(rtl_crypto_aes_cbc_init = 0x1c95);
    PROVIDE(rtl_crypto_aes_cbc_encrypt = 0x1cf5);
    PROVIDE(rtl_crypto_aes_cbc_decrypt = 0x1d49);
    PROVIDE(rtl_crypto_aes_ecb_init = 0x1d9d);
    PROVIDE(rtl_crypto_aes_ecb_encrypt = 0x1dfd);
    PROVIDE(rtl_crypto_aes_ecb_decrypt = 0x1e3d);
    PROVIDE(rtl_crypto_aes_ctr_init = 0x1e7d);
    PROVIDE(rtl_crypto_aes_ctr_encrypt = 0x1edd);
    PROVIDE(rtl_crypto_aes_ctr_decrypt = 0x1f29);
    PROVIDE(rtl_crypto_aes_cfb_init = 0x1f75);
    PROVIDE(rtl_crypto_aes_cfb_encrypt = 0x1fd5);
    PROVIDE(rtl_crypto_aes_cfb_decrypt = 0x2021);
    PROVIDE(rtl_crypto_aes_ofb_init = 0x206d);
    PROVIDE(rtl_crypto_aes_ofb_encrypt = 0x20cd);
    PROVIDE(rtl_crypto_aes_ofb_decrypt = 0x2119);
    PROVIDE(rtl_crypto_aes_gcm_init = 0x2165);
    PROVIDE(rtl_crypto_aes_gcm_encrypt = 0x21c5);
    PROVIDE(rtl_crypto_aes_gcm_decrypt = 0x2259);
    PROVIDE(hash_save = 0x22ed);
    PROVIDE(hash_restore = 0x2385);
    PROVIDE(rtl_crypto_sha1_init = 0x2431);
    PROVIDE(rtl_crypto_sha1_process = 0x24c9);
    PROVIDE(rtl_crypto_sha1_update_rom = 0x2509);
    PROVIDE(rtl_crypto_sha1_update = 0x2589);
    PROVIDE(rtl_crypto_sha1_final = 0x25f9);
    PROVIDE(rtl_crypto_sha1 = 0x2675);
    PROVIDE(rtl_crypto_sha2_init = 0x26a1);
    PROVIDE(rtl_crypto_sha2_process = 0x2969);
    PROVIDE(rtl_crypto_sha2_update_rom = 0x29ad);
    PROVIDE(rtl_crypto_sha2_update = 0x2a35);
    PROVIDE(rtl_crypto_sha2_final = 0x2aa5);
    PROVIDE(rtl_crypto_sha2 = 0x2b25);
    PROVIDE(rtl_crypto_hmac_sha1_init = 0x2b51);
    PROVIDE(rtl_crypto_hmac_sha1_process = 0x2c39);
    PROVIDE(rtl_crypto_hmac_sha1_update_rom = 0x2c7d);
    PROVIDE(rtl_crypto_hmac_sha1_update = 0x2d05);
    PROVIDE(rtl_crypto_hmac_sha1_final = 0x2d75);
    PROVIDE(rtl_crypto_hmac_sha1 = 0x2df5);
    PROVIDE(rtl_crypto_hmac_sha2_init = 0x2e25);
    PROVIDE(rtl_crypto_hmac_sha2_process = 0x3189);
    PROVIDE(rtl_crypto_hmac_sha2_update_rom = 0x31d1);
    PROVIDE(rtl_crypto_hmac_sha2_update = 0x325d);
    PROVIDE(rtl_crypto_hmac_sha2_final = 0x32cd);
    PROVIDE(rtl_crypto_hmac_sha2 = 0x3351);
    PROVIDE(md_hash_save = 0x3381);
    PROVIDE(md_hash_restore = 0x3419);
    PROVIDE(rtl_crypto_md5_init = 0x34c1);
    PROVIDE(rtl_crypto_md5_process = 0x3549);
    PROVIDE(rtl_crypto_md5_update_rom = 0x35ad);
    PROVIDE(rtl_crypto_md5_update = 0x362d);
    PROVIDE(rtl_crypto_md5_final = 0x369d);
    PROVIDE(rtl_crypto_md5 = 0x371d);
    PROVIDE(rtl_crypto_hmac_md5_init = 0x3749);
    PROVIDE(rtl_crypto_hmac_md5_process = 0x3821);
    PROVIDE(rtl_crypto_hmac_md5_update_rom = 0x3869);
    PROVIDE(rtl_crypto_hmac_md5_update = 0x38f5);
    PROVIDE(rtl_crypto_hmac_md5_final = 0x3965);
    PROVIDE(rtl_crypto_hmac_md5 = 0x39e9);
    PROVIDE(RSIP_OTF_Load_Key = 0x3a19);
    PROVIDE(RSIP_OTF_init = 0x3a31);
    PROVIDE(RSIP_IV_Set = 0x3a45);
    PROVIDE(RSIP_OTF_Cmd = 0x3a61);
    PROVIDE(RSIP_OTF_Enable = 0x3a7d);
    PROVIDE(RSIP_MMU_Config = 0x3b25);
    PROVIDE(RSIP_MMU_Cmd = 0x3b5d);
    PROVIDE(RSIP_MMU_Cache_Clean = 0x3b89);
    PROVIDE(boot_rom_ed25519_ge_double_scalarmult_vartime = 0x4e5b);
    PROVIDE(boot_rom_ed25519_ge_frombytes_negate_vartime = 0x522d);
    PROVIDE(boot_rom_ed25519_ge_tobytes = 0x56c7);
    PROVIDE(boot_rom_ed25519_verify_signature = 0x584b);
    PROVIDE(boot_rom_ed25519_crypto_sign_verify_detached = 0x584f);
    PROVIDE(boot_rom_ed25519_sc_reduce = 0x5a67);
    PROVIDE(rom_sha512_starts = 0x69d7);
    PROVIDE(rom_sha512_update = 0x69db);
    PROVIDE(rom_sha512_finish = 0x69df);
    PROVIDE(rom_sha512 = 0x69e3);
    PROVIDE(rom_sha512_hmac_starts = 0x69e7);
    PROVIDE(rom_sha512_hmac_update = 0x6a71);
    PROVIDE(rom_sha512_hmac_finish = 0x6a75);
    PROVIDE(rom_sha512_hmac_reset = 0x6ac9);
    PROVIDE(rom_sha512_hmac = 0x6ae5);
    PROVIDE(rom_sha512_hkdf = 0x6b21);
    PROVIDE(BOOT_ROM_OTFCheck = 0x6bf1);
    PROVIDE(BOOT_ROM_OTPCheck = 0x6cc1);
    PROVIDE(BOOT_ROM_SPUFlash = 0x6e31);
    PROVIDE(BOOT_ROM_InitSpic = 0x6e85);
    PROVIDE(BOOT_ROM_LoadImage = 0x6fa1);
    PROVIDE(BOOT_ROM_SignatureCheck = 0x7111);
    PROVIDE(BOOT_ROM_LoaderEntry = 0x71e1);
    PROVIDE(BOOT_ROM_KeyDeriveFunc = 0x7265);
    PROVIDE(BOOT_ROM_HUKDerive = 0x7459);
    PROVIDE(BOOT_ROM_SecurityConfig = 0x752d);
    PROVIDE(BOOT_ROM_BootldrVersion = 0x7715);
    PROVIDE(BOOT_ROM_BootldrSelect = 0x7765);
    PROVIDE(BOOT_ROM_InitFlash = 0x79e9);
    PROVIDE(BOOT_ROM_FromFlash = 0x7b0d);
    PROVIDE(BOOT_ROM_InitDebugFlg = 0x7bc9);
    PROVIDE(BOOT_ROM_OSC131_Enable = 0x7c09);
    PROVIDE(BOOT_ROM_ResetVsr = 0x7c21);
    PROVIDE(__rom_xo_end__ = 0x7e44);
    PROVIDE(Reset_Handler = 0x9001);
    PROVIDE(NMI_Handler = 0x9011);
    PROVIDE(HardFault_Handler = 0x9015);
    PROVIDE(MemManage_Handler = 0x9019);
    PROVIDE(BusFault_Handler = 0x901d);
    PROVIDE(UsageFault_Handler = 0x9021);
    PROVIDE(SecureFault_Handler = 0x9025);
    PROVIDE(io_assert_failed = 0x902d);
    PROVIDE(BKUP_Write = 0x904d);
    PROVIDE(BKUP_Read = 0x9079);
    PROVIDE(BKUP_Set = 0x90a1);
    PROVIDE(BKUP_Clear = 0x90c9);
    PROVIDE(EFUSE_GetPackageInfo = 0x90f5);
    PROVIDE(EFUSE_GetBDNum = 0x9119);
    PROVIDE(EFUSE_GetChipVersion = 0x9139);
    PROVIDE(EFUSE_GetESFlag = 0x915d);
    PROVIDE(EFUSE_GetUUID = 0x9181);
    PROVIDE(EFUSE_Get_Info = 0x91c5);
    PROVIDE(XTAL_ClkGet = 0x9255);
    PROVIDE(PLL_ClkGet = 0x9271);
    PROVIDE(_CPU_ClkSet = 0x92b1);
    PROVIDE(CPU_ClkGet = 0x93b5);
    PROVIDE(_CPU_ClkSrcSet = 0x9441);
    PROVIDE(DelayNop = 0x9499);
    PROVIDE(DelayUs = 0x94a5);
    PROVIDE(DelayMs = 0x951d);
    PROVIDE(DelayClkUpdate = 0x9531);
    PROVIDE(FLASH_Calibration_PLL_Open = 0x9549);
    PROVIDE(FLASH_Calibration_PLL_Close = 0x9575);
    PROVIDE(FLASH_CalibrationPLLPSCmd = 0x958d);
    PROVIDE(FLASH_CalibrationPLLPS_Shift = 0x95a5);
    PROVIDE(FLASH_PLLInit_ClockDiv = 0x95bd);
    PROVIDE(FLASH_CalibrationPhase = 0x9631);
    PROVIDE(FLASH_CalibrationPhaseIdx = 0x96c1);
    PROVIDE(FLASH_CalibrationNewCmd = 0x96cd);
    PROVIDE(FLASH_Read_HandShake_Cmd = 0x96f5);
    PROVIDE(FLASH_Read_DataIsRight = 0x9749);
    PROVIDE(FLASH_CalibrationNew = 0x97e1);
    PROVIDE(FLASH_Read_HandShake = 0x996d);
    PROVIDE(FLASH_UserMode_Enter = 0x99e5);
    PROVIDE(FLASH_UserMode_Exit = 0x99fd);
    PROVIDE(FLASH_Config = 0x9a0d);
    PROVIDE(FLASH_SetSpiMode = 0x9aa1);
    PROVIDE(FLASH_RxCmd_InUserMode = 0x9ba9);
    PROVIDE(FLASH_RxCmd = 0x9c25);
    PROVIDE(FLASH_WaitBusy_InUserMode = 0x9c41);
    PROVIDE(FLASH_RxData = 0x9cc9);
    PROVIDE(FLASH_TxCmd_InUserMode = 0x9df1);
    PROVIDE(FLASH_WriteEn_InUserMode = 0x9e49);
    PROVIDE(FLASH_TxData = 0x9e6d);
    PROVIDE(FLASH_SetStatus = 0x9fb9);
    PROVIDE(FLASH_Erase = 0x9fe5);
    PROVIDE(FLASH_Addr4ByteEn = 0xa081);
    PROVIDE(FLASH_Addr4ByteDis = 0xa0a5);
    PROVIDE(FLASH_DeepPowerDown = 0xa0c9);
    PROVIDE(FLASH_SetStatusBits = 0xa129);
    PROVIDE(FLASH_StructInit_Micron = 0xa201);
    PROVIDE(FLASH_StructInit_MXIC = 0xa26d);
    PROVIDE(FLASH_StructInit_GD = 0xa2d9);
    PROVIDE(FLASH_StructInit = 0xa34d);
    PROVIDE(FLASH_Init = 0xa3bd);
    PROVIDE(FLASH_Wait_Boot_Finish = 0xa3dd);
    PROVIDE(GPIO_INTMode = 0xa401);
    PROVIDE(GPIO_INTConfig = 0xa485);
    PROVIDE(GPIO_INTHandler = 0xa4c9);
    PROVIDE(GPIO_Direction = 0xa581);
    PROVIDE(GPIO_Init = 0xa5cd);
    PROVIDE(GPIO_DeInit = 0xa65d);
    PROVIDE(GPIO_ReadDataBit = 0xa6d1);
    PROVIDE(GPIO_WriteBit = 0xa709);
    PROVIDE(GPIO_PortDirection = 0xa755);
    PROVIDE(GPIO_PortRead = 0xa7a9);
    PROVIDE(GPIO_PortWrite = 0xa7ed);
    PROVIDE(GPIO_DebounceClock = 0xa839);
    PROVIDE(GPIO_LevelSync = 0xa889);
    PROVIDE(GPIO_UserRegIrq = 0xa8d5);
    PROVIDE(GPIO_ODInit = 0xa901);
    PROVIDE(GPIO_ODWriteBit = 0xa931);
    PROVIDE(LOGUART_StructInit = 0xa949);
    PROVIDE(LOGUART_SetBaud = 0xa981);
    PROVIDE(LOGUART_RxCmd = 0xaa2d);
    PROVIDE(LOGUART_INTCoreConfig = 0xaa41);
    PROVIDE(LOGUART_AGGCmd = 0xaa55);
    PROVIDE(LOGUART_AGGPathCmd = 0xaa69);
    PROVIDE(LOGUART_AGGSetTimeOut = 0xaae9);
    PROVIDE(LOGUART_AGGSetThreshold = 0xab05);
    PROVIDE(LOGUART_AGGSetSyncPattern = 0xab31);
    PROVIDE(LOGUART_AGGPathAllCmd = 0xab41);
    PROVIDE(LOGUART_WaitTx = 0xab65);
    PROVIDE(LOGUART_PutChar = 0xaba1);
    PROVIDE(LOGUART_GetChar = 0xabed);
    PROVIDE(LOGUART_Readable = 0xac09);
    PROVIDE(LOGUART_GetRxCount = 0xac19);
    PROVIDE(LOGUART_GetIMR = 0xac25);
    PROVIDE(LOGUART_SetIMR = 0xac31);
    PROVIDE(LOGUART_INTConfig = 0xac3d);
    PROVIDE(LOGUART_INTClear = 0xac4d);
    PROVIDE(LOGUART_GetStatus = 0xac55);
    PROVIDE(LOGUART_ClearRxFifo = 0xac59);
    PROVIDE(_LOGUART_Init = 0xac79);
    PROVIDE(LOGUART_Init = 0xadcd);
    PROVIDE(LOGUART_WaitBusy = 0xae31);
    PROVIDE(LOGUART_WaitTxComplete = 0xae71);
    PROVIDE(LOGUART_BT_SendData = 0xaea1);
    PROVIDE(LOGUART_Relay_StructInit = 0xaed9);
    PROVIDE(LOGUART_Relay_SetFormat = 0xaeed);
    PROVIDE(LOGUART_Relay_SetBaud = 0xaf3d);
    PROVIDE(LOGUART_Relay_ClearRxFifo = 0xaf95);
    PROVIDE(LOGUART_Relay_RxCmd = 0xafb9);
    PROVIDE(LOGUART_MonitorParaConfig = 0xafcd);
    PROVIDE(LOGUART_LPBaudSet = 0xb015);
    PROVIDE(LOGUART_Relay_LPSetBaud = 0xb0c1);
    PROVIDE(LOGUART_RxMonitorCmd = 0xb121);
    PROVIDE(LOGUART_RxMonBaudCtrlRegGet = 0xb135);
    PROVIDE(LOGUART_RxMonitorSatusGet = 0xb139);
    PROVIDE(LOGUART_DiagInit = 0xb13d);
    PROVIDE(OTP_PowerCmd = 0xb1cd);
    PROVIDE(OTP_GetPowerState = 0xb1e9);
    PROVIDE(OTP_AccessCmd = 0xb1f9);
    PROVIDE(OTPPowerSwitch = 0xb211);
    PROVIDE(OTPGetCRC = 0xb235);
    PROVIDE(OTP_Read8_Rom = 0xb275);
    PROVIDE(OTP_Write8_Rom = 0xb485);
    PROVIDE(OTP_Read32 = 0xb52d);
    PROVIDE(OTP_PG_Packet_Byte = 0xb565);
    PROVIDE(OTP_PG_Packet_Word = 0xb635);
    PROVIDE(OTP_LogicalMap_Read = 0xb769);
    PROVIDE(OTP_LogicalMap_Write = 0xb891);
    PROVIDE(otp_logical_remain = 0xbaa9);
    PROVIDE(PAD_DrvStrength = 0xbaf5);
    PROVIDE(PAD_SchmitCtrl = 0xbb19);
    PROVIDE(PAD_SlewRateCtrl = 0xbb39);
    PROVIDE(PAD_PullCtrl = 0xbb59);
    PROVIDE(PAD_SleepPullCtrl = 0xbc21);
    PROVIDE(PAD_ResistorCtrl = 0xbce9);
    PROVIDE(PAD_InputCtrl = 0xbd0d);
    PROVIDE(PAD_CMD = 0xbd2d);
    PROVIDE(Pinmux_Config = 0xbd4d);
    PROVIDE(Pinmux_ConfigGet = 0xbd65);
    PROVIDE(Pinmux_UartLogCtrl = 0xbd75);
    PROVIDE(PAD_SpicCtrl = 0xbdbd);
    PROVIDE(Pinmux_BootSpicCtrl = 0xbe91);
    PROVIDE(Pinmux_ComboSpicCtrl = 0xbe9d);
    PROVIDE(Pinmux_Swdoff = 0xbea9);
    PROVIDE(RCC_PeriphClockCmd = 0xbebd);
    PROVIDE(RCC_PeriphClockSource_UART_ROM = 0xbf5d);
    PROVIDE(RCC_PeriphClockSource_SPIC = 0xbfe5);
    PROVIDE(RCC_PeriphClockSource_PSRAM = 0xc001);
    PROVIDE(RCC_PeriphClockSource_CTC = 0xc01d);
    PROVIDE(RCC_PeriphClockSource_ADC = 0xc039);
    PROVIDE(RCC_PeriphClockSource_GPIO = 0xc055);
    PROVIDE(RCC_PeriphClockSource_SPORT = 0xc071);
    PROVIDE(RCC_PeriphClockSource_LOGUART_ROM = 0xc0b1);
    PROVIDE(IPC_SEMTake = 0xc109);
    PROVIDE(IPC_SEMFree = 0xc135);
    PROVIDE(SYSCFG_RLVersion = 0xc15d);
    PROVIDE(SYSCFG_GetRLNum = 0xc181);
    PROVIDE(SYSCFG_TRP_UARTImage = 0xc1a5);
    PROVIDE(SYSCFG_TRP_ICFG = 0xc1b5);
    PROVIDE(SYSCFG_TRP_TestMode = 0xc1cd);
    PROVIDE(SYSCFG_TRP_OTPBYP = 0xc1dd);
    PROVIDE(SYSCFG_ROMINFO_Get = 0xc1ed);
    PROVIDE(SYSCFG_ROMINFO_Set = 0xc1fd);
    PROVIDE(SYSCFG_CHIPType_Get = 0xc219);
    PROVIDE(SYSCFG_OTP_RDPEn = 0xc229);
    PROVIDE(SYSCFG_OTP_RSIPEn = 0xc249);
    PROVIDE(SYSCFG_OTP_SBootEn = 0xc269);
    PROVIDE(SYSCFG_OTP_LowBaudLog = 0xc289);
    PROVIDE(SYSCFG_OTP_DisBootLog = 0xc299);
    PROVIDE(SYSCFG_OTP_BootMCMFlash = 0xc2a9);
    PROVIDE(SYSCFG_OTP_BOOT_CNT_LMT_EN = 0xc2d1);
    PROVIDE(SYSCFG_OTP_BootFlashBaud = 0xc2e1);
    PROVIDE(SYSCFG_OTP_SPICBootSpeedUpDis = 0xc2f1);
    PROVIDE(SYSCFG_OTP_SPICAddr4ByteEn = 0xc301);
    PROVIDE(SYSCFG_OTP_FlashDSleepEn = 0xc311);
    PROVIDE(SYSCFG_OTP_UsbClockCalEn = 0xc321);
    PROVIDE(SYSCFG_OTP_UsbDownloadEn = 0xc331);
    PROVIDE(SYSCFG_OTP_UartDownloadDis = 0xc33d);
    PROVIDE(SYSCFG_OTP_AntiRollback = 0xc34d);
    PROVIDE(SYSCFG_OTP_FaultLogDis = 0xc361);
    PROVIDE(SYSCFG_OTP_RSIPMode = 0xc375);
    PROVIDE(SYSCFG_OTP_HUKDerivEn = 0xc38d);
    PROVIDE(SYSCFG_OTP_RomPatchEn = 0xc3a1);
    PROVIDE(SYSCFG_OTP_RandDelayEn = 0xc3c1);
    PROVIDE(SYSCFG_OTP_RMAMode = 0xc3d1);
    PROVIDE(SYSCFG_HWTrigDownload = 0xc3f5);
    PROVIDE(SYSCFG_SWTrigDownload = 0xc409);
    PROVIDE(SYSTIMER_Init = 0xc42d);
    PROVIDE(SYSTIMER_TickGet = 0xc479);
    PROVIDE(SYSTIMER_GetPassTick = 0xc489);
    PROVIDE(SYSTIMER_GetPassTime = 0xc4a9);
    PROVIDE(RTIM_TimeBaseStructInit = 0xc4dd);
    PROVIDE(RTIM_Cmd = 0xc4fd);
    PROVIDE(RTIM_GetCount = 0xc521);
    PROVIDE(RTIM_INTConfig = 0xc531);
    PROVIDE(RTIM_INTClear = 0xc575);
    PROVIDE(RTIM_TimeBaseInit = 0xc58d);
    PROVIDE(RTIM_DeInit = 0xc68d);
    PROVIDE(RTIM_INTClearPendingBit = 0xc6ad);
    PROVIDE(RTIM_GetFlagStatus = 0xc6ed);
    PROVIDE(RTIM_GetINTStatus = 0xc765);
    PROVIDE(TRNG_Random_Delay = 0xc7e9);
    PROVIDE(TRNG_INTConfig = 0xc8a9);
    PROVIDE(TRNG_ClearINT = 0xc8dd);
    PROVIDE(TRNG_GetINT = 0xc905);
    PROVIDE(TRNG_Init = 0xc929);
    PROVIDE(UART_DeInit = 0xc92d);
    PROVIDE(UART_StructInit = 0xc935);
    PROVIDE(UART_BaudParaGetFull = 0xc951);
    PROVIDE(UART_BaudParaGet = 0xc991);
    PROVIDE(UART_SetBaud = 0xc9b9);
    PROVIDE(UART_SetBaudExt = 0xca69);
    PROVIDE(UART_SetRxLevel = 0xcaf9);
    PROVIDE(UART_RxCmd = 0xcb09);
    PROVIDE(UART_Writable = 0xcb1d);
    PROVIDE(UART_Readable = 0xcb25);
    PROVIDE(UART_CharPut = 0xcb2d);
    PROVIDE(UART_CharGet = 0xcb31);
    PROVIDE(UART_ReceiveData = 0xcb39);
    PROVIDE(UART_SendData = 0xcb61);
    PROVIDE(UART_ReceiveDataTO = 0xcb89);
    PROVIDE(UART_SendDataTO = 0xcbc5);
    PROVIDE(UART_RxByteCntClear = 0xcc01);
    PROVIDE(UART_RxByteCntGet = 0xcc0d);
    PROVIDE(UART_BreakCtl = 0xcc15);
    PROVIDE(UART_ClearRxFifo = 0xcc29);
    PROVIDE(UART_Init = 0xcc49);
    PROVIDE(UART_ClearTxFifo = 0xcce5);
    PROVIDE(UART_INTConfig = 0xccf1);
    PROVIDE(UART_INT_Clear = 0xcd01);
    PROVIDE(UART_IntStatus = 0xcd09);
    PROVIDE(UART_ModemStatusGet = 0xcd0d);
    PROVIDE(UART_LineStatusGet = 0xcd11);
    PROVIDE(UART_WaitBusy = 0xcd15);
    PROVIDE(INT_HardFault = 0xcd39);
    PROVIDE(INT_MemManage = 0xcd5d);
    PROVIDE(INT_BusFault = 0xcd81);
    PROVIDE(INT_UsageFault = 0xcda5);
    PROVIDE(INT_SecureFault = 0xcdc9);
    PROVIDE(INT_HardFault_C = 0xcded);
    PROVIDE(INT_NMI = 0xd23d);
    PROVIDE(irq_table_init = 0xd3c9);
    PROVIDE(irq_enable = 0xd641);
    PROVIDE(irq_disable = 0xd65d);
    PROVIDE(irq_set_priority = 0xd685);
    PROVIDE(irq_get_priority = 0xd6e5);
    PROVIDE(irq_set_pending = 0xd705);
    PROVIDE(irq_get_pending = 0xd725);
    PROVIDE(irq_clear_pending = 0xd749);
    PROVIDE(irq_register = 0xd769);
    PROVIDE(irq_unregister = 0xd795);
    PROVIDE(FaultPatch_register = 0xd7b9);
    PROVIDE(WDG_Wait_Busy = 0xd7c9);
    PROVIDE(WDG_StructInit = 0xd7ed);
    PROVIDE(WDG_Init = 0xd7fd);
    PROVIDE(WDG_Enable = 0xd879);
    PROVIDE(WDG_Timeout = 0xd8d1);
    PROVIDE(WDG_Refresh = 0xd939);
    PROVIDE(WDG_INTConfig = 0xd991);
    PROVIDE(WDG_ClearINT = 0xda01);
    PROVIDE(IWDG_LP_Enable = 0xda69);
    PROVIDE(DiagVSprintf = 0xdaa9);
    PROVIDE(DiagPrintf = 0xdd55);
    PROVIDE(DiagPrintfD = 0xdd89);
    PROVIDE(DiagSPrintf = 0xddbd);
    PROVIDE(DiagSnPrintf = 0xddf1);
    PROVIDE(cmd_rom_table = 0xe1ad);
    PROVIDE(_rand = 0xe1b9);
    PROVIDE(__wrap_rand = 0xe215);
    PROVIDE(RandBytes_Get = 0xe219);
    PROVIDE(shell_array_init = 0xe305);
    PROVIDE(shell_get_argc = 0xe313);
    PROVIDE(shell_get_argv = 0xe33f);
    PROVIDE(shell_cmd_chk = 0xe385);
    PROVIDE(shell_uart_irq_rom = 0xe441);
    PROVIDE(shell_init_rom = 0xe505);
    PROVIDE(shell_task_rom = 0xe575);
    PROVIDE(shell_rom = 0xe60d);
    PROVIDE(USBD_CDC_SetTxBuffer = 0xe8fd);
    PROVIDE(USBD_CDC_SetRxBuffer = 0xe915);
    PROVIDE(USBD_CDC_TransmitPacket = 0xe929);
    PROVIDE(USBD_CDC_ReceivePacket = 0xe961);
    PROVIDE(USBD_Core_Init = 0xe985);
    PROVIDE(USBD_Core_DeInit = 0xea01);
    PROVIDE(USBD_IRQHandler = 0xea39);
    PROVIDE(USBD_OpenEP = 0xea5d);
    PROVIDE(USBD_CloseEP = 0xea71);
    PROVIDE(USBD_Transmit = 0xea81);
    PROVIDE(USBD_PrepareReceive = 0xea91);
    PROVIDE(USBD_GetRxDataSize = 0xeaa1);
    PROVIDE(USBD_DataOutStage = 0xeaa9);
    PROVIDE(USBD_DataInStage = 0xeb2d);
    PROVIDE(USBD_Reset = 0xebf1);
    PROVIDE(USBD_SetSpeed = 0xec3d);
    PROVIDE(USBD_Suspend = 0xec45);
    PROVIDE(USBD_Resume = 0xec59);
    PROVIDE(USBD_SOF = 0xec69);
    PROVIDE(USBD_DevConnected = 0xec85);
    PROVIDE(USBD_DevDisconnected = 0xec89);
    PROVIDE(USBD_CtlSendData = 0xeca1);
    PROVIDE(USBD_CtlPrepareRx = 0xecbd);
    PROVIDE(USBD_CtlError = 0xecd9);
    PROVIDE(USBD_SetupStage = 0xecf5);
    PROVIDE(USBD_GetString = 0xf1d1);
    PROVIDE(USBD_DebugInit = 0xf225);
    PROVIDE(USBD_Debug = 0xf271);
    PROVIDE(USBD_Debug_Config = 0xf2b5);
    PROVIDE(USB_HalInit = 0xf2ed);
    PROVIDE(USB_SetTurnaroundTime = 0xf3bd);
    PROVIDE(USB_EnableGlobalInt = 0xf3e5);
    PROVIDE(USB_DisableGlobalInt = 0xf3f9);
    PROVIDE(USB_SetDeviceMode = 0xf40d);
    PROVIDE(USB_EPPredictNextEp = 0xf435);
    PROVIDE(USB_ResetLearnQueue = 0xf439);
    PROVIDE(USB_FlushTxFifo = 0xf461);
    PROVIDE(USB_FlushRxFifo = 0xf49d);
    PROVIDE(USB_SetDevSpeed = 0xf4d5);
    PROVIDE(USB_DevInit = 0xf4e5);
    PROVIDE(USB_GetDevSpeed = 0xf625);
    PROVIDE(USB_GetTxFifoNum = 0xf641);
    PROVIDE(USB_ActivateEndpoint = 0xf651);
    PROVIDE(USB_DeactivateEndpoint = 0xf6dd);
    PROVIDE(USB_EP0StartXfer = 0xf745);
    PROVIDE(USB_WritePacket = 0xf845);
    PROVIDE(USB_EPStartXfer = 0xf86d);
    PROVIDE(USB_ReadPacket = 0xfa01);
    PROVIDE(USB_EPSetStall = 0xfa21);
    PROVIDE(USB_EPClearStall = 0xfa5d);
    PROVIDE(USB_StopDevice = 0xfa99);
    PROVIDE(USB_SetDevAddress = 0xfae5);
    PROVIDE(USB_DevConnect = 0xfb05);
    PROVIDE(USB_DevDisconnect = 0xfb25);
    PROVIDE(USB_ReadInterrupts = 0xfb45);
    PROVIDE(USB_ReadDevAllOutEpInterrupt = 0xfb55);
    PROVIDE(USB_ReadDevAllInEpInterrupt = 0xfb65);
    PROVIDE(USB_ReadDevOutEPInterrupt = 0xfb75);
    PROVIDE(USB_ReadDevInEPInterrupt = 0xfb8d);
    PROVIDE(USB_ClearInterrupts = 0xfba5);
    PROVIDE(USB_GetMode = 0xfbb5);
    PROVIDE(USB_ActivateSetup = 0xfbc5);
    PROVIDE(USB_EP0_OutStart = 0xfbfd);
    PROVIDE(USBD_IsAttached = 0xff31);
    PROVIDE(USBD_Process = 0xff3d);
    PROVIDE(USBD_Init = 0xffb1);
    PROVIDE(USBD_DeInit = 0xffed);
    PROVIDE(USB_PCD_Init = 0xfff5);
    PROVIDE(USB_PCD_Start = 0x101ed);
    PROVIDE(USB_PCD_Stop = 0x10215);
    PROVIDE(USB_PCD_DeInit = 0x10251);
    PROVIDE(USB_PCD_DevConnect = 0x10291);
    PROVIDE(USB_PCD_DevDisconnect = 0x102b5);
    PROVIDE(USB_PCD_SetAddress = 0x102d9);
    PROVIDE(USB_PCD_EP_Open = 0x10301);
    PROVIDE(USB_PCD_EP_Close = 0x10379);
    PROVIDE(USB_PCD_EP_Receive = 0x103cd);
    PROVIDE(USB_PCD_EP_GetRxCount = 0x1041d);
    PROVIDE(USB_PCD_EP_Transmit = 0x1042d);
    PROVIDE(USB_PCD_EP_SetStall = 0x10481);
    PROVIDE(USB_PCD_EP_ClrStall = 0x104e1);
    PROVIDE(USB_PCD_EP_Flush = 0x1053d);
    PROVIDE(USB_PCD_GetState = 0x10579);
    PROVIDE(USB_PCD_SetTxFiFo = 0x10581);
    PROVIDE(USB_PCD_SetRxFiFo = 0x105d9);
    PROVIDE(USB_PCD_IRQHandler = 0x105e5);
    PROVIDE(xmodem_img_rxbuffer = 0x10d79);
    PROVIDE(xmodem_uart_init = 0x11535);
    PROVIDE(xmodem_uart_port_init = 0x115ad);
    PROVIDE(xmodem_uart_port_deinit = 0x115c5);
    PROVIDE(xmodem_uart_readable = 0x115dd);
    PROVIDE(xmodem_uart_writable = 0x115e5);
    PROVIDE(xmodem_uart_getc = 0x115f5);
    PROVIDE(xmodem_uart_putc = 0x11601);
    PROVIDE(xmodem_uart_putdata = 0x1160d);
    PROVIDE(xmodem_uart_getc_to = 0x11629);
    PROVIDE(xmodem_uart_clean_rx = 0x11655);
    PROVIDE(xmodem_uart_waitbusy = 0x11671);
    PROVIDE(xmodem_uart_setbaud = 0x11689);
    PROVIDE(xmodem_img_write = 0x116bd);
    PROVIDE(xmodem_vefiry_floader = 0x11749);
    PROVIDE(xmodem_pll_clkset = 0x11891);
    PROVIDE(xmodem_soc_clkset = 0x118e5);
    PROVIDE(xmodem_img_download = 0x11925);
    PROVIDE(_char2num = 0x11a19);
    PROVIDE(_2char2dec = 0x11a49);
    PROVIDE(_2char2hex = 0x11abd);
    PROVIDE(_memchr = 0x11b3d);
    PROVIDE(_memcmp = 0x11bbd);
    PROVIDE(_memcpy = 0x11c1d);
    PROVIDE(_memmove = 0x11ced);
    PROVIDE(_memset = 0x11de5);
    PROVIDE(_strcmp = 0x11e89);
    PROVIDE(_stricmp = 0x11f15);
    PROVIDE(_strlen = 0x11f69);
    PROVIDE(_strtoul = 0x11fc5);
    PROVIDE(_strupr = 0x120f1);
    PROVIDE(__wrap_memset = 0x1210d);
    PROVIDE(__wrap_memcpy = 0x12115);
    PROVIDE(__wrap_memcmp = 0x1211d);
    PROVIDE(__wrap_memchr = 0x12125);
    PROVIDE(__wrap_memmove = 0x1212d);
    PROVIDE(__wrap_strlen = 0x12135);
    PROVIDE(__wrap_strcmp = 0x1213d);
    PROVIDE(__wrap_stricmp = 0x12145);
    PROVIDE(__wrap_strupr = 0x1214d);
    PROVIDE(__wrap_strtoul = 0x12155);
    PROVIDE(__rom_entry_ns_start__ = 0x42000);
    PROVIDE(SPIC_CALIB_PATTERN = 0x4484c);
    PROVIDE(GPIO_PORTx_S = 0x44854);
    PROVIDE(GPIO_PORTx = 0x4485c);
    PROVIDE(LOG_UART_IDX_FLAG = 0x44864);
    PROVIDE(USBD_CDC = 0x44a6c);
    PROVIDE(BAUDRATE_TABLE = 0x44ac8);
    PROVIDE(__nspe_phy_addr__ = 0x8014000);
    PROVIDE(__km4_image3_flash_start__ = 0x81e0000);
    PROVIDE(__km0_flash_text_start__ = 0xc000020);
    PROVIDE(__km4_flash_text_start__ = 0xe000020);
    PROVIDE(__km4_boot_text_start__ = 0xf800020);
    PROVIDE(__rom_bss_start__ = 0x20004000);
    PROVIDE(ConfigDebugClose = 0x20004000);
    PROVIDE(ConfigDebug = 0x20004004);
    PROVIDE(flash_init_para = 0x20004014);
    PROVIDE(shell_argv_array = 0x2000406c);
    PROVIDE(shell_rxbuf = 0x200040ac);
    PROVIDE(shell_ctl = 0x2000412c);
    PROVIDE(shell_buf = 0x20004144);
    PROVIDE(SHA_OTPKEY = 0x200041e0);
    PROVIDE(crypto_engine = 0x20004200);
    PROVIDE(crypto_sha_engine = 0x20004440);
    PROVIDE(RBSS_UDELAY_CLK = 0x200046dc);
    PROVIDE(PortA_IrqHandlerTable = 0x200046e0);
    PROVIDE(PortA_IrqDataTable = 0x20004760);
    PROVIDE(PortB_IrqHandlerTable = 0x200047e0);
    PROVIDE(PortB_IrqDataTable = 0x20004860);
    PROVIDE(__rom_bss_chk__ = 0x200048e0);
    PROVIDE(__rom_bss_end__ = 0x200048e0);
    PROVIDE(__km0_ipc_memory_start__ = 0x20004e00);
    PROVIDE(__km0_image2_entry_func__ = 0x20050020);
    PROVIDE(__rom_bss_start_s__ = 0x30007000);
    PROVIDE(NewVectorTable = 0x30007000);
    PROVIDE(FaultPatchHandler = 0x30007180);
    PROVIDE(UserIrqFunTable = 0x30007184);
    PROVIDE(UserIrqDataTable = 0x300072c4);
    PROVIDE(DerivedKey = 0x30007404);
    PROVIDE(USBD_Device = 0x30007898);
    PROVIDE(xmodem_ctrl = 0x30007984);
    PROVIDE(usbd_enabled = 0x30007990);
    PROVIDE(p_rom_ssl_ram_map = 0x30007998);
    PROVIDE(rom_ssl_ram_map = 0x3000799c);
    PROVIDE(__rom_bss_end_s__ = 0x300079a8);
    PROVIDE(STACK_TOP = 0x30009ffc);
    PROVIDE(__ram_start_table_start__ = 0x3000a020);
    PROVIDE(__retention_entry_func__ = 0x41008e00);
    PROVIDE(__km0_psram_text_start__ = 0x60180020);
}
