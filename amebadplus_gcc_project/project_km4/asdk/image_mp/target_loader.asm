
/home/auto_jenkins_wlan5/IOT_ATS/repos/release-master/source/amebadplus_gcc_project/project_km4/asdk/image_mp/target_loader.axf:     file format elf32-littlearm


Disassembly of section .ram_image1.text:

3000a050 <flash_handshake_highspeed>:
3000a050:	b570      	push	{r4, r5, r6, lr}
3000a052:	4b15      	ldr	r3, [pc, #84]	; (3000a0a8 <flash_handshake_highspeed+0x58>)
3000a054:	b082      	sub	sp, #8
3000a056:	4606      	mov	r6, r0
3000a058:	b2c0      	uxtb	r0, r0
3000a05a:	4798      	blx	r3
3000a05c:	4d13      	ldr	r5, [pc, #76]	; (3000a0ac <flash_handshake_highspeed+0x5c>)
3000a05e:	2101      	movs	r1, #1
3000a060:	2003      	movs	r0, #3
3000a062:	47a8      	blx	r5
3000a064:	4b12      	ldr	r3, [pc, #72]	; (3000a0b0 <flash_handshake_highspeed+0x60>)
3000a066:	4798      	blx	r3
3000a068:	b160      	cbz	r0, 3000a084 <flash_handshake_highspeed+0x34>
3000a06a:	2401      	movs	r4, #1
3000a06c:	4911      	ldr	r1, [pc, #68]	; (3000a0b4 <flash_handshake_highspeed+0x64>)
3000a06e:	4b12      	ldr	r3, [pc, #72]	; (3000a0b8 <flash_handshake_highspeed+0x68>)
3000a070:	2249      	movs	r2, #73	; 0x49
3000a072:	2004      	movs	r0, #4
3000a074:	e9cd 6100 	strd	r6, r1, [sp]
3000a078:	4910      	ldr	r1, [pc, #64]	; (3000a0bc <flash_handshake_highspeed+0x6c>)
3000a07a:	f000 f899 	bl	3000a1b0 <__rtk_log_write_veneer>
3000a07e:	4620      	mov	r0, r4
3000a080:	b002      	add	sp, #8
3000a082:	bd70      	pop	{r4, r5, r6, pc}
3000a084:	4604      	mov	r4, r0
3000a086:	4601      	mov	r1, r0
3000a088:	2003      	movs	r0, #3
3000a08a:	47a8      	blx	r5
3000a08c:	4b0c      	ldr	r3, [pc, #48]	; (3000a0c0 <flash_handshake_highspeed+0x70>)
3000a08e:	4620      	mov	r0, r4
3000a090:	4798      	blx	r3
3000a092:	4a0c      	ldr	r2, [pc, #48]	; (3000a0c4 <flash_handshake_highspeed+0x74>)
3000a094:	490c      	ldr	r1, [pc, #48]	; (3000a0c8 <flash_handshake_highspeed+0x78>)
3000a096:	f8d2 3140 	ldr.w	r3, [r2, #320]	; 0x140
3000a09a:	f023 030f 	bic.w	r3, r3, #15
3000a09e:	f043 0301 	orr.w	r3, r3, #1
3000a0a2:	f8c2 3140 	str.w	r3, [r2, #320]	; 0x140
3000a0a6:	e7e2      	b.n	3000a06e <flash_handshake_highspeed+0x1e>
3000a0a8:	000095bd 	.word	0x000095bd
3000a0ac:	000096f5 	.word	0x000096f5
3000a0b0:	00009749 	.word	0x00009749
3000a0b4:	0f804864 	.word	0x0f804864
3000a0b8:	0f804868 	.word	0x0f804868
3000a0bc:	0f804884 	.word	0x0f804884
3000a0c0:	0000bfe5 	.word	0x0000bfe5
3000a0c4:	40128000 	.word	0x40128000
3000a0c8:	0f80485c 	.word	0x0f80485c

3000a0cc <flash_rx_mode_switch>:
3000a0cc:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
3000a0d0:	2300      	movs	r3, #0
3000a0d2:	b089      	sub	sp, #36	; 0x24
3000a0d4:	f8df e0d0 	ldr.w	lr, [pc, #208]	; 3000a1a8 <flash_rx_mode_switch+0xdc>
3000a0d8:	4604      	mov	r4, r0
3000a0da:	f10d 0c0c 	add.w	ip, sp, #12
3000a0de:	f88d 300b 	strb.w	r3, [sp, #11]
3000a0e2:	4d2a      	ldr	r5, [pc, #168]	; (3000a18c <flash_rx_mode_switch+0xc0>)
3000a0e4:	4e2a      	ldr	r6, [pc, #168]	; (3000a190 <flash_rx_mode_switch+0xc4>)
3000a0e6:	f8df 90c4 	ldr.w	r9, [pc, #196]	; 3000a1ac <flash_rx_mode_switch+0xe0>
3000a0ea:	f105 084e 	add.w	r8, r5, #78	; 0x4e
3000a0ee:	4f29      	ldr	r7, [pc, #164]	; (3000a194 <flash_rx_mode_switch+0xc8>)
3000a0f0:	e8be 000f 	ldmia.w	lr!, {r0, r1, r2, r3}
3000a0f4:	e8ac 000f 	stmia.w	ip!, {r0, r1, r2, r3}
3000a0f8:	f8de 3000 	ldr.w	r3, [lr]
3000a0fc:	f8cc 3000 	str.w	r3, [ip]
3000a100:	e006      	b.n	3000a110 <flash_rx_mode_switch+0x44>
3000a102:	b2e0      	uxtb	r0, r4
3000a104:	47b0      	blx	r6
3000a106:	4b24      	ldr	r3, [pc, #144]	; (3000a198 <flash_rx_mode_switch+0xcc>)
3000a108:	b334      	cbz	r4, 3000a158 <flash_rx_mode_switch+0x8c>
3000a10a:	4798      	blx	r3
3000a10c:	bb70      	cbnz	r0, 3000a16c <flash_rx_mode_switch+0xa0>
3000a10e:	3c01      	subs	r4, #1
3000a110:	2c02      	cmp	r4, #2
3000a112:	bf18      	it	ne
3000a114:	2c04      	cmpne	r4, #4
3000a116:	682b      	ldr	r3, [r5, #0]
3000a118:	bf0c      	ite	eq
3000a11a:	2201      	moveq	r2, #1
3000a11c:	2200      	movne	r2, #0
3000a11e:	2b03      	cmp	r3, #3
3000a120:	f885 2055 	strb.w	r2, [r5, #85]	; 0x55
3000a124:	d1ed      	bne.n	3000a102 <flash_rx_mode_switch+0x36>
3000a126:	2101      	movs	r1, #1
3000a128:	2085      	movs	r0, #133	; 0x85
3000a12a:	f10d 020b 	add.w	r2, sp, #11
3000a12e:	47c8      	blx	r9
3000a130:	f89d 300b 	ldrb.w	r3, [sp, #11]
3000a134:	f814 0008 	ldrb.w	r0, [r4, r8]
3000a138:	f10d 020b 	add.w	r2, sp, #11
3000a13c:	f003 030f 	and.w	r3, r3, #15
3000a140:	2101      	movs	r1, #1
3000a142:	ea43 1300 	orr.w	r3, r3, r0, lsl #4
3000a146:	2081      	movs	r0, #129	; 0x81
3000a148:	f88d 300b 	strb.w	r3, [sp, #11]
3000a14c:	47b8      	blx	r7
3000a14e:	b2e0      	uxtb	r0, r4
3000a150:	47b0      	blx	r6
3000a152:	4b11      	ldr	r3, [pc, #68]	; (3000a198 <flash_rx_mode_switch+0xcc>)
3000a154:	2c00      	cmp	r4, #0
3000a156:	d1d8      	bne.n	3000a10a <flash_rx_mode_switch+0x3e>
3000a158:	4b10      	ldr	r3, [pc, #64]	; (3000a19c <flash_rx_mode_switch+0xd0>)
3000a15a:	2245      	movs	r2, #69	; 0x45
3000a15c:	4910      	ldr	r1, [pc, #64]	; (3000a1a0 <flash_rx_mode_switch+0xd4>)
3000a15e:	2002      	movs	r0, #2
3000a160:	f000 f826 	bl	3000a1b0 <__rtk_log_write_veneer>
3000a164:	4620      	mov	r0, r4
3000a166:	b009      	add	sp, #36	; 0x24
3000a168:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
3000a16c:	ab08      	add	r3, sp, #32
3000a16e:	2249      	movs	r2, #73	; 0x49
3000a170:	490b      	ldr	r1, [pc, #44]	; (3000a1a0 <flash_rx_mode_switch+0xd4>)
3000a172:	2004      	movs	r0, #4
3000a174:	eb03 0484 	add.w	r4, r3, r4, lsl #2
3000a178:	4b0a      	ldr	r3, [pc, #40]	; (3000a1a4 <flash_rx_mode_switch+0xd8>)
3000a17a:	f854 4c14 	ldr.w	r4, [r4, #-20]
3000a17e:	9400      	str	r4, [sp, #0]
3000a180:	f000 f816 	bl	3000a1b0 <__rtk_log_write_veneer>
3000a184:	2001      	movs	r0, #1
3000a186:	b009      	add	sp, #36	; 0x24
3000a188:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
3000a18c:	20004014 	.word	0x20004014
3000a190:	0000a3bd 	.word	0x0000a3bd
3000a194:	00009fb9 	.word	0x00009fb9
3000a198:	00009749 	.word	0x00009749
3000a19c:	0f80488c 	.word	0x0f80488c
3000a1a0:	0f804884 	.word	0x0f804884
3000a1a4:	0f8048ac 	.word	0x0f8048ac
3000a1a8:	0f804938 	.word	0x0f804938
3000a1ac:	00009c25 	.word	0x00009c25

3000a1b0 <__rtk_log_write_veneer>:
3000a1b0:	f85f f000 	ldr.w	pc, [pc]	; 3000a1b4 <__rtk_log_write_veneer+0x4>
3000a1b4:	0f802519 	.word	0x0f802519
	...

Disassembly of section .xip_image1.text:

0f800020 <SDM32K_Enable>:
 f800020:	4a02      	ldr	r2, [pc, #8]	; (f80002c <SDM32K_Enable+0xc>)
 f800022:	4b03      	ldr	r3, [pc, #12]	; (f800030 <SDM32K_Enable+0x10>)
 f800024:	6811      	ldr	r1, [r2, #0]
 f800026:	430b      	orrs	r3, r1
 f800028:	6013      	str	r3, [r2, #0]
 f80002a:	4770      	bx	lr
 f80002c:	41009000 	.word	0x41009000
 f800030:	c0060000 	.word	0xc0060000

0f800034 <OSC131K_Reset>:
 f800034:	4b04      	ldr	r3, [pc, #16]	; (f800048 <OSC131K_Reset+0x14>)
 f800036:	691a      	ldr	r2, [r3, #16]
 f800038:	f022 0201 	bic.w	r2, r2, #1
 f80003c:	611a      	str	r2, [r3, #16]
 f80003e:	691a      	ldr	r2, [r3, #16]
 f800040:	f042 0201 	orr.w	r2, r2, #1
 f800044:	611a      	str	r2, [r3, #16]
 f800046:	4770      	bx	lr
 f800048:	41008100 	.word	0x41008100

0f80004c <HANDLER_SecureFault>:
 f80004c:	f3ef 8008 	mrs	r0, MSP
 f800050:	f3ef 8109 	mrs	r1, PSP
 f800054:	4672      	mov	r2, lr
 f800056:	f04f 0304 	mov.w	r3, #4
 f80005a:	e92d 0ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
 f80005e:	f000 b801 	b.w	f800064 <Fault_Handler>
 f800062:	4770      	bx	lr

0f800064 <Fault_Handler>:
 f800064:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
 f800068:	b095      	sub	sp, #84	; 0x54
 f80006a:	4614      	mov	r4, r2
 f80006c:	4605      	mov	r5, r0
 f80006e:	2244      	movs	r2, #68	; 0x44
 f800070:	460e      	mov	r6, r1
 f800072:	a803      	add	r0, sp, #12
 f800074:	2100      	movs	r1, #0
 f800076:	461f      	mov	r7, r3
 f800078:	f004 fa56 	bl	f804528 <____wrap_memset_veneer>
 f80007c:	07e2      	lsls	r2, r4, #31
 f80007e:	f004 0340 	and.w	r3, r4, #64	; 0x40
 f800082:	d554      	bpl.n	f80012e <Fault_Handler+0xca>
 f800084:	2b00      	cmp	r3, #0
 f800086:	d05b      	beq.n	f800140 <Fault_Handler+0xdc>
 f800088:	f004 000c 	and.w	r0, r4, #12
 f80008c:	f04f 0800 	mov.w	r8, #0
 f800090:	f1a0 000c 	sub.w	r0, r0, #12
 f800094:	fab0 f080 	clz	r0, r0
 f800098:	0940      	lsrs	r0, r0, #5
 f80009a:	f1a5 0c24 	sub.w	ip, r5, #36	; 0x24
 f80009e:	ab04      	add	r3, sp, #16
 f8000a0:	1f29      	subs	r1, r5, #4
 f8000a2:	9403      	str	r4, [sp, #12]
 f8000a4:	f85c 2f04 	ldr.w	r2, [ip, #4]!
 f8000a8:	458c      	cmp	ip, r1
 f8000aa:	f843 2b04 	str.w	r2, [r3], #4
 f8000ae:	d1f9      	bne.n	f8000a4 <Fault_Handler+0x40>
 f8000b0:	2800      	cmp	r0, #0
 f8000b2:	bf14      	ite	ne
 f8000b4:	4631      	movne	r1, r6
 f8000b6:	4629      	moveq	r1, r5
 f8000b8:	f10d 0e30 	add.w	lr, sp, #48	; 0x30
 f8000bc:	f1a1 0c04 	sub.w	ip, r1, #4
 f8000c0:	f101 021c 	add.w	r2, r1, #28
 f8000c4:	f85c 3f04 	ldr.w	r3, [ip, #4]!
 f8000c8:	4594      	cmp	ip, r2
 f8000ca:	f84e 3b04 	str.w	r3, [lr], #4
 f8000ce:	d1f9      	bne.n	f8000c4 <Fault_Handler+0x60>
 f8000d0:	aa03      	add	r2, sp, #12
 f8000d2:	6bc8      	ldr	r0, [r1, #60]	; 0x3c
 f8000d4:	f003 fae4 	bl	f8036a0 <crash_dump>
 f8000d8:	2f04      	cmp	r7, #4
 f8000da:	d042      	beq.n	f800162 <Fault_Handler+0xfe>
 f8000dc:	f1b8 0f00 	cmp.w	r8, #0
 f8000e0:	d136      	bne.n	f800150 <Fault_Handler+0xec>
 f8000e2:	f8df 80c8 	ldr.w	r8, [pc, #200]	; f8001ac <Fault_Handler+0x148>
 f8000e6:	2241      	movs	r2, #65	; 0x41
 f8000e8:	4b28      	ldr	r3, [pc, #160]	; (f80018c <Fault_Handler+0x128>)
 f8000ea:	f8d8 0024 	ldr.w	r0, [r8, #36]	; 0x24
 f8000ee:	4928      	ldr	r1, [pc, #160]	; (f800190 <Fault_Handler+0x12c>)
 f8000f0:	9000      	str	r0, [sp, #0]
 f8000f2:	2001      	movs	r0, #1
 f8000f4:	f002 fa10 	bl	f802518 <rtk_log_write>
 f8000f8:	f8d8 300c 	ldr.w	r3, [r8, #12]
 f8000fc:	2241      	movs	r2, #65	; 0x41
 f8000fe:	4924      	ldr	r1, [pc, #144]	; (f800190 <Fault_Handler+0x12c>)
 f800100:	9300      	str	r3, [sp, #0]
 f800102:	2001      	movs	r0, #1
 f800104:	4b23      	ldr	r3, [pc, #140]	; (f800194 <Fault_Handler+0x130>)
 f800106:	f002 fa07 	bl	f802518 <rtk_log_write>
 f80010a:	f3ef 8314 	mrs	r3, CONTROL
 f80010e:	2241      	movs	r2, #65	; 0x41
 f800110:	9300      	str	r3, [sp, #0]
 f800112:	491f      	ldr	r1, [pc, #124]	; (f800190 <Fault_Handler+0x12c>)
 f800114:	2001      	movs	r0, #1
 f800116:	4b20      	ldr	r3, [pc, #128]	; (f800198 <Fault_Handler+0x134>)
 f800118:	f002 f9fe 	bl	f802518 <rtk_log_write>
 f80011c:	463b      	mov	r3, r7
 f80011e:	4622      	mov	r2, r4
 f800120:	4631      	mov	r1, r6
 f800122:	4628      	mov	r0, r5
 f800124:	f004 f9e8 	bl	f8044f8 <__INT_HardFault_C_veneer>
 f800128:	b015      	add	sp, #84	; 0x54
 f80012a:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 f80012e:	2b00      	cmp	r3, #0
 f800130:	d0aa      	beq.n	f800088 <Fault_Handler+0x24>
 f800132:	4b1a      	ldr	r3, [pc, #104]	; (f80019c <Fault_Handler+0x138>)
 f800134:	2241      	movs	r2, #65	; 0x41
 f800136:	4916      	ldr	r1, [pc, #88]	; (f800190 <Fault_Handler+0x12c>)
 f800138:	2001      	movs	r0, #1
 f80013a:	f002 f9ed 	bl	f802518 <rtk_log_write>
 f80013e:	e7fe      	b.n	f80013e <Fault_Handler+0xda>
 f800140:	f3ef 8894 	mrs	r8, CONTROL_NS
 f800144:	f014 0008 	ands.w	r0, r4, #8
 f800148:	d0a7      	beq.n	f80009a <Fault_Handler+0x36>
 f80014a:	f3c8 0040 	ubfx	r0, r8, #1, #1
 f80014e:	e7a4      	b.n	f80009a <Fault_Handler+0x36>
 f800150:	4b13      	ldr	r3, [pc, #76]	; (f8001a0 <Fault_Handler+0x13c>)
 f800152:	2241      	movs	r2, #65	; 0x41
 f800154:	490e      	ldr	r1, [pc, #56]	; (f800190 <Fault_Handler+0x12c>)
 f800156:	2001      	movs	r0, #1
 f800158:	f8cd 8000 	str.w	r8, [sp]
 f80015c:	f002 f9dc 	bl	f802518 <rtk_log_write>
 f800160:	e7bf      	b.n	f8000e2 <Fault_Handler+0x7e>
 f800162:	f8df 9048 	ldr.w	r9, [pc, #72]	; f8001ac <Fault_Handler+0x148>
 f800166:	2241      	movs	r2, #65	; 0x41
 f800168:	4b0e      	ldr	r3, [pc, #56]	; (f8001a4 <Fault_Handler+0x140>)
 f80016a:	f8d9 00e4 	ldr.w	r0, [r9, #228]	; 0xe4
 f80016e:	4908      	ldr	r1, [pc, #32]	; (f800190 <Fault_Handler+0x12c>)
 f800170:	9000      	str	r0, [sp, #0]
 f800172:	2001      	movs	r0, #1
 f800174:	f002 f9d0 	bl	f802518 <rtk_log_write>
 f800178:	f8d9 30e8 	ldr.w	r3, [r9, #232]	; 0xe8
 f80017c:	2241      	movs	r2, #65	; 0x41
 f80017e:	4904      	ldr	r1, [pc, #16]	; (f800190 <Fault_Handler+0x12c>)
 f800180:	9300      	str	r3, [sp, #0]
 f800182:	2001      	movs	r0, #1
 f800184:	4b08      	ldr	r3, [pc, #32]	; (f8001a8 <Fault_Handler+0x144>)
 f800186:	f002 f9c7 	bl	f802518 <rtk_log_write>
 f80018a:	e7a7      	b.n	f8000dc <Fault_Handler+0x78>
 f80018c:	0f80467c 	.word	0x0f80467c
 f800190:	0f804634 	.word	0x0f804634
 f800194:	0f804690 	.word	0x0f804690
 f800198:	0f8046a4 	.word	0x0f8046a4
 f80019c:	0f8045e8 	.word	0x0f8045e8
 f8001a0:	0f804664 	.word	0x0f804664
 f8001a4:	0f80463c 	.word	0x0f80463c
 f8001a8:	0f804650 	.word	0x0f804650
 f8001ac:	e000ed00 	.word	0xe000ed00

0f8001b0 <Fault_Hanlder_Redirect>:
 f8001b0:	4b01      	ldr	r3, [pc, #4]	; (f8001b8 <Fault_Hanlder_Redirect+0x8>)
 f8001b2:	4a02      	ldr	r2, [pc, #8]	; (f8001bc <Fault_Hanlder_Redirect+0xc>)
 f8001b4:	61da      	str	r2, [r3, #28]
 f8001b6:	4770      	bx	lr
 f8001b8:	30007000 	.word	0x30007000
 f8001bc:	0f80004d 	.word	0x0f80004d

0f8001c0 <flash_init_userdef>:
 f8001c0:	4b15      	ldr	r3, [pc, #84]	; (f800218 <flash_init_userdef+0x58>)
 f8001c2:	2201      	movs	r2, #1
 f8001c4:	2140      	movs	r1, #64	; 0x40
 f8001c6:	f44f 6000 	mov.w	r0, #2048	; 0x800
 f8001ca:	b430      	push	{r4, r5}
 f8001cc:	e9c3 1203 	strd	r1, r2, [r3, #12]
 f8001d0:	2502      	movs	r5, #2
 f8001d2:	2400      	movs	r4, #0
 f8001d4:	f640 0104 	movw	r1, #2052	; 0x804
 f8001d8:	601a      	str	r2, [r3, #0]
 f8001da:	22bb      	movs	r2, #187	; 0xbb
 f8001dc:	f8a3 004e 	strh.w	r0, [r3, #78]	; 0x4e
 f8001e0:	f8a3 1050 	strh.w	r1, [r3, #80]	; 0x50
 f8001e4:	203b      	movs	r0, #59	; 0x3b
 f8001e6:	21eb      	movs	r1, #235	; 0xeb
 f8001e8:	621a      	str	r2, [r3, #32]
 f8001ea:	4a0c      	ldr	r2, [pc, #48]	; (f80021c <flash_init_userdef+0x5c>)
 f8001ec:	61d8      	str	r0, [r3, #28]
 f8001ee:	6299      	str	r1, [r3, #40]	; 0x28
 f8001f0:	f64a 3120 	movw	r1, #43808	; 0xab20
 f8001f4:	641a      	str	r2, [r3, #64]	; 0x40
 f8001f6:	22b9      	movs	r2, #185	; 0xb9
 f8001f8:	4809      	ldr	r0, [pc, #36]	; (f800220 <flash_init_userdef+0x60>)
 f8001fa:	f8a3 1048 	strh.w	r1, [r3, #72]	; 0x48
 f8001fe:	6458      	str	r0, [r3, #68]	; 0x44
 f800200:	f883 204a 	strb.w	r2, [r3, #74]	; 0x4a
 f800204:	e9c3 5405 	strd	r5, r4, [r3, #20]
 f800208:	2406      	movs	r4, #6
 f80020a:	f883 4052 	strb.w	r4, [r3, #82]	; 0x52
 f80020e:	246b      	movs	r4, #107	; 0x6b
 f800210:	625c      	str	r4, [r3, #36]	; 0x24
 f800212:	bc30      	pop	{r4, r5}
 f800214:	4770      	bx	lr
 f800216:	bf00      	nop
 f800218:	20004014 	.word	0x20004014
 f80021c:	00059f06 	.word	0x00059f06
 f800220:	d8600001 	.word	0xd8600001

0f800224 <flash_get_readmode>:
 f800224:	f647 73ff 	movw	r3, #32767	; 0x7fff
 f800228:	4298      	cmp	r0, r3
 f80022a:	d016      	beq.n	f80025a <flash_get_readmode+0x36>
 f80022c:	f5b0 4f00 	cmp.w	r0, #32768	; 0x8000
 f800230:	d20a      	bcs.n	f800248 <flash_get_readmode+0x24>
 f800232:	f641 73ff 	movw	r3, #8191	; 0x1fff
 f800236:	4298      	cmp	r0, r3
 f800238:	d00d      	beq.n	f800256 <flash_get_readmode+0x32>
 f80023a:	f643 73ff 	movw	r3, #16383	; 0x3fff
 f80023e:	4298      	cmp	r0, r3
 f800240:	bf0c      	ite	eq
 f800242:	2002      	moveq	r0, #2
 f800244:	2000      	movne	r0, #0
 f800246:	4770      	bx	lr
 f800248:	f64f 73ff 	movw	r3, #65535	; 0xffff
 f80024c:	4298      	cmp	r0, r3
 f80024e:	bf0c      	ite	eq
 f800250:	2004      	moveq	r0, #4
 f800252:	2000      	movne	r0, #0
 f800254:	4770      	bx	lr
 f800256:	2001      	movs	r0, #1
 f800258:	4770      	bx	lr
 f80025a:	2003      	movs	r0, #3
 f80025c:	4770      	bx	lr
 f80025e:	bf00      	nop

0f800260 <flash_get_chip_info>:
 f800260:	4b09      	ldr	r3, [pc, #36]	; (f800288 <flash_get_chip_info+0x28>)
 f800262:	21ef      	movs	r1, #239	; 0xef
 f800264:	22ff      	movs	r2, #255	; 0xff
 f800266:	e005      	b.n	f800274 <flash_get_chip_info+0x14>
 f800268:	7f1a      	ldrb	r2, [r3, #28]
 f80026a:	2aff      	cmp	r2, #255	; 0xff
 f80026c:	d008      	beq.n	f800280 <flash_get_chip_info+0x20>
 f80026e:	699a      	ldr	r2, [r3, #24]
 f800270:	f853 1f14 	ldr.w	r1, [r3, #20]!
 f800274:	4002      	ands	r2, r0
 f800276:	469c      	mov	ip, r3
 f800278:	428a      	cmp	r2, r1
 f80027a:	d1f5      	bne.n	f800268 <flash_get_chip_info+0x8>
 f80027c:	4660      	mov	r0, ip
 f80027e:	4770      	bx	lr
 f800280:	f04f 0c00 	mov.w	ip, #0
 f800284:	4660      	mov	r0, ip
 f800286:	4770      	bx	lr
 f800288:	0f8046b8 	.word	0x0f8046b8

0f80028c <flash_get_layout_info>:
 f80028c:	b510      	push	{r4, lr}
 f80028e:	f04f 0c00 	mov.w	ip, #0
 f800292:	4c0e      	ldr	r4, [pc, #56]	; (f8002cc <flash_get_layout_info+0x40>)
 f800294:	4663      	mov	r3, ip
 f800296:	46a6      	mov	lr, r4
 f800298:	e005      	b.n	f8002a6 <flash_get_layout_info+0x1a>
 f80029a:	f85e 3f0c 	ldr.w	r3, [lr, #12]!
 f80029e:	f10c 0c01 	add.w	ip, ip, #1
 f8002a2:	2bff      	cmp	r3, #255	; 0xff
 f8002a4:	d010      	beq.n	f8002c8 <flash_get_layout_info+0x3c>
 f8002a6:	4298      	cmp	r0, r3
 f8002a8:	d1f7      	bne.n	f80029a <flash_get_layout_info+0xe>
 f8002aa:	b129      	cbz	r1, f8002b8 <flash_get_layout_info+0x2c>
 f8002ac:	eb0c 034c 	add.w	r3, ip, ip, lsl #1
 f8002b0:	eb04 0383 	add.w	r3, r4, r3, lsl #2
 f8002b4:	685b      	ldr	r3, [r3, #4]
 f8002b6:	600b      	str	r3, [r1, #0]
 f8002b8:	b132      	cbz	r2, f8002c8 <flash_get_layout_info+0x3c>
 f8002ba:	eb0c 0c4c 	add.w	ip, ip, ip, lsl #1
 f8002be:	eb04 0c8c 	add.w	ip, r4, ip, lsl #2
 f8002c2:	f8dc 3008 	ldr.w	r3, [ip, #8]
 f8002c6:	6013      	str	r3, [r2, #0]
 f8002c8:	bd10      	pop	{r4, pc}
 f8002ca:	bf00      	nop
 f8002cc:	0f8047e4 	.word	0x0f8047e4

0f8002d0 <flash_highspeed_setup>:
 f8002d0:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 f8002d4:	4b90      	ldr	r3, [pc, #576]	; (f800518 <flash_highspeed_setup+0x248>)
 f8002d6:	b088      	sub	sp, #32
 f8002d8:	4798      	blx	r3
 f8002da:	4b90      	ldr	r3, [pc, #576]	; (f80051c <flash_highspeed_setup+0x24c>)
 f8002dc:	4604      	mov	r4, r0
 f8002de:	8818      	ldrh	r0, [r3, #0]
 f8002e0:	f7ff ffa0 	bl	f800224 <flash_get_readmode>
 f8002e4:	4b8e      	ldr	r3, [pc, #568]	; (f800520 <flash_highspeed_setup+0x250>)
 f8002e6:	4a8f      	ldr	r2, [pc, #572]	; (f800524 <flash_highspeed_setup+0x254>)
 f8002e8:	4606      	mov	r6, r0
 f8002ea:	498f      	ldr	r1, [pc, #572]	; (f800528 <flash_highspeed_setup+0x258>)
 f8002ec:	4423      	add	r3, r4
 f8002ee:	880d      	ldrh	r5, [r1, #0]
 f8002f0:	fba2 2303 	umull	r2, r3, r2, r3
 f8002f4:	3d01      	subs	r5, #1
 f8002f6:	0edb      	lsrs	r3, r3, #27
 f8002f8:	b2ed      	uxtb	r5, r5
 f8002fa:	3b01      	subs	r3, #1
 f8002fc:	429d      	cmp	r5, r3
 f8002fe:	bf38      	it	cc
 f800300:	461d      	movcc	r5, r3
 f800302:	b672      	cpsid	i
 f800304:	4989      	ldr	r1, [pc, #548]	; (f80052c <flash_highspeed_setup+0x25c>)
 f800306:	b2ed      	uxtb	r5, r5
 f800308:	680a      	ldr	r2, [r1, #0]
 f80030a:	429d      	cmp	r5, r3
 f80030c:	f442 6200 	orr.w	r2, r2, #2048	; 0x800
 f800310:	600a      	str	r2, [r1, #0]
 f800312:	d107      	bne.n	f800324 <flash_highspeed_setup+0x54>
 f800314:	f8d1 3140 	ldr.w	r3, [r1, #320]	; 0x140
 f800318:	f023 030f 	bic.w	r3, r3, #15
 f80031c:	f043 0302 	orr.w	r3, r3, #2
 f800320:	f8c1 3140 	str.w	r3, [r1, #320]	; 0x140
 f800324:	4c82      	ldr	r4, [pc, #520]	; (f800530 <flash_highspeed_setup+0x260>)
 f800326:	aa06      	add	r2, sp, #24
 f800328:	2103      	movs	r1, #3
 f80032a:	4f82      	ldr	r7, [pc, #520]	; (f800534 <flash_highspeed_setup+0x264>)
 f80032c:	f894 0041 	ldrb.w	r0, [r4, #65]	; 0x41
 f800330:	47b8      	blx	r7
 f800332:	f89d 301a 	ldrb.w	r3, [sp, #26]
 f800336:	2249      	movs	r2, #73	; 0x49
 f800338:	497f      	ldr	r1, [pc, #508]	; (f800538 <flash_highspeed_setup+0x268>)
 f80033a:	9302      	str	r3, [sp, #8]
 f80033c:	2004      	movs	r0, #4
 f80033e:	f89d 3019 	ldrb.w	r3, [sp, #25]
 f800342:	f8df 8238 	ldr.w	r8, [pc, #568]	; f80057c <flash_highspeed_setup+0x2ac>
 f800346:	9301      	str	r3, [sp, #4]
 f800348:	f89d 3018 	ldrb.w	r3, [sp, #24]
 f80034c:	9300      	str	r3, [sp, #0]
 f80034e:	4b7b      	ldr	r3, [pc, #492]	; (f80053c <flash_highspeed_setup+0x26c>)
 f800350:	f002 f8e2 	bl	f802518 <rtk_log_write>
 f800354:	f89d 3019 	ldrb.w	r3, [sp, #25]
 f800358:	f89d 001a 	ldrb.w	r0, [sp, #26]
 f80035c:	021b      	lsls	r3, r3, #8
 f80035e:	ea43 4300 	orr.w	r3, r3, r0, lsl #16
 f800362:	f89d 0018 	ldrb.w	r0, [sp, #24]
 f800366:	4318      	orrs	r0, r3
 f800368:	f7ff ff7a 	bl	f800260 <flash_get_chip_info>
 f80036c:	f8c8 0000 	str.w	r0, [r8]
 f800370:	2800      	cmp	r0, #0
 f800372:	f000 80b9 	beq.w	f8004e8 <flash_highspeed_setup+0x218>
 f800376:	7a03      	ldrb	r3, [r0, #8]
 f800378:	2b05      	cmp	r3, #5
 f80037a:	d809      	bhi.n	f800390 <flash_highspeed_setup+0xc0>
 f80037c:	3b01      	subs	r3, #1
 f80037e:	486c      	ldr	r0, [pc, #432]	; (f800530 <flash_highspeed_setup+0x260>)
 f800380:	2b04      	cmp	r3, #4
 f800382:	f200 80c6 	bhi.w	f800512 <flash_highspeed_setup+0x242>
 f800386:	e8df f003 	tbb	[pc, r3]
 f80038a:	a79c      	.short	0xa79c
 f80038c:	93aa      	.short	0x93aa
 f80038e:	96          	.byte	0x96
 f80038f:	00          	.byte	0x00
 f800390:	2bfe      	cmp	r3, #254	; 0xfe
 f800392:	d104      	bne.n	f80039e <flash_highspeed_setup+0xce>
 f800394:	6903      	ldr	r3, [r0, #16]
 f800396:	2b00      	cmp	r3, #0
 f800398:	f000 80b3 	beq.w	f800502 <flash_highspeed_setup+0x232>
 f80039c:	4798      	blx	r3
 f80039e:	4b68      	ldr	r3, [pc, #416]	; (f800540 <flash_highspeed_setup+0x270>)
 f8003a0:	4798      	blx	r3
 f8003a2:	b110      	cbz	r0, f8003aa <flash_highspeed_setup+0xda>
 f8003a4:	2304      	movs	r3, #4
 f8003a6:	f884 3054 	strb.w	r3, [r4, #84]	; 0x54
 f8003aa:	f8d8 3000 	ldr.w	r3, [r8]
 f8003ae:	aa06      	add	r2, sp, #24
 f8003b0:	2101      	movs	r1, #1
 f8003b2:	f894 0042 	ldrb.w	r0, [r4, #66]	; 0x42
 f8003b6:	f8d3 800c 	ldr.w	r8, [r3, #12]
 f8003ba:	68e3      	ldr	r3, [r4, #12]
 f8003bc:	9305      	str	r3, [sp, #20]
 f8003be:	47b8      	blx	r7
 f8003c0:	69a3      	ldr	r3, [r4, #24]
 f8003c2:	2b00      	cmp	r3, #0
 f8003c4:	d162      	bne.n	f80048c <flash_highspeed_setup+0x1bc>
 f8003c6:	4b5f      	ldr	r3, [pc, #380]	; (f800544 <flash_highspeed_setup+0x274>)
 f8003c8:	781b      	ldrb	r3, [r3, #0]
 f8003ca:	2b00      	cmp	r3, #0
 f8003cc:	d13d      	bne.n	f80044a <flash_highspeed_setup+0x17a>
 f8003ce:	2701      	movs	r7, #1
 f8003d0:	9b06      	ldr	r3, [sp, #24]
 f8003d2:	463a      	mov	r2, r7
 f8003d4:	a806      	add	r0, sp, #24
 f8003d6:	a905      	add	r1, sp, #20
 f8003d8:	ea03 0308 	and.w	r3, r3, r8
 f8003dc:	9306      	str	r3, [sp, #24]
 f8003de:	4b5a      	ldr	r3, [pc, #360]	; (f800548 <flash_highspeed_setup+0x278>)
 f8003e0:	4798      	blx	r3
 f8003e2:	b148      	cbz	r0, f8003f8 <flash_highspeed_setup+0x128>
 f8003e4:	f894 3045 	ldrb.w	r3, [r4, #69]	; 0x45
 f8003e8:	aa05      	add	r2, sp, #20
 f8003ea:	f894 0044 	ldrb.w	r0, [r4, #68]	; 0x44
 f8003ee:	2b00      	cmp	r3, #0
 f8003f0:	d154      	bne.n	f80049c <flash_highspeed_setup+0x1cc>
 f8003f2:	4639      	mov	r1, r7
 f8003f4:	4b55      	ldr	r3, [pc, #340]	; (f80054c <flash_highspeed_setup+0x27c>)
 f8003f6:	4798      	blx	r3
 f8003f8:	b2f0      	uxtb	r0, r6
 f8003fa:	f004 f88d 	bl	f804518 <__flash_rx_mode_switch_veneer>
 f8003fe:	4a54      	ldr	r2, [pc, #336]	; (f800550 <flash_highspeed_setup+0x280>)
 f800400:	f894 1056 	ldrb.w	r1, [r4, #86]	; 0x56
 f800404:	f8d2 324c 	ldr.w	r3, [r2, #588]	; 0x24c
 f800408:	bb51      	cbnz	r1, f800460 <flash_highspeed_setup+0x190>
 f80040a:	4952      	ldr	r1, [pc, #328]	; (f800554 <flash_highspeed_setup+0x284>)
 f80040c:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
 f800410:	ac06      	add	r4, sp, #24
 f800412:	f10d 071d 	add.w	r7, sp, #29
 f800416:	f8c2 324c 	str.w	r3, [r2, #588]	; 0x24c
 f80041a:	4e4f      	ldr	r6, [pc, #316]	; (f800558 <flash_highspeed_setup+0x288>)
 f80041c:	c903      	ldmia	r1, {r0, r1}
 f80041e:	9006      	str	r0, [sp, #24]
 f800420:	200d      	movs	r0, #13
 f800422:	f8ad 101c 	strh.w	r1, [sp, #28]
 f800426:	e001      	b.n	f80042c <flash_highspeed_setup+0x15c>
 f800428:	f814 0f01 	ldrb.w	r0, [r4, #1]!
 f80042c:	2100      	movs	r1, #0
 f80042e:	47b0      	blx	r6
 f800430:	42a7      	cmp	r7, r4
 f800432:	d1f9      	bne.n	f800428 <flash_highspeed_setup+0x158>
 f800434:	4b49      	ldr	r3, [pc, #292]	; (f80055c <flash_highspeed_setup+0x28c>)
 f800436:	4798      	blx	r3
 f800438:	2801      	cmp	r0, #1
 f80043a:	d002      	beq.n	f800442 <flash_highspeed_setup+0x172>
 f80043c:	4628      	mov	r0, r5
 f80043e:	f004 f867 	bl	f804510 <__flash_handshake_highspeed_veneer>
 f800442:	b662      	cpsie	i
 f800444:	b008      	add	sp, #32
 f800446:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 f80044a:	f10d 0219 	add.w	r2, sp, #25
 f80044e:	2102      	movs	r1, #2
 f800450:	2015      	movs	r0, #21
 f800452:	47b8      	blx	r7
 f800454:	9b05      	ldr	r3, [sp, #20]
 f800456:	2703      	movs	r7, #3
 f800458:	f443 3300 	orr.w	r3, r3, #131072	; 0x20000
 f80045c:	9305      	str	r3, [sp, #20]
 f80045e:	e7b7      	b.n	f8003d0 <flash_highspeed_setup+0x100>
 f800460:	493f      	ldr	r1, [pc, #252]	; (f800560 <flash_highspeed_setup+0x290>)
 f800462:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
 f800466:	ac06      	add	r4, sp, #24
 f800468:	f10d 071d 	add.w	r7, sp, #29
 f80046c:	f8c2 324c 	str.w	r3, [r2, #588]	; 0x24c
 f800470:	4e39      	ldr	r6, [pc, #228]	; (f800558 <flash_highspeed_setup+0x288>)
 f800472:	c903      	ldmia	r1, {r0, r1}
 f800474:	9006      	str	r0, [sp, #24]
 f800476:	2000      	movs	r0, #0
 f800478:	f8ad 101c 	strh.w	r1, [sp, #28]
 f80047c:	e001      	b.n	f800482 <flash_highspeed_setup+0x1b2>
 f80047e:	f814 0f01 	ldrb.w	r0, [r4, #1]!
 f800482:	2100      	movs	r1, #0
 f800484:	47b0      	blx	r6
 f800486:	42bc      	cmp	r4, r7
 f800488:	d1f9      	bne.n	f80047e <flash_highspeed_setup+0x1ae>
 f80048a:	e7d3      	b.n	f800434 <flash_highspeed_setup+0x164>
 f80048c:	f10d 0219 	add.w	r2, sp, #25
 f800490:	2101      	movs	r1, #1
 f800492:	f894 0043 	ldrb.w	r0, [r4, #67]	; 0x43
 f800496:	47b8      	blx	r7
 f800498:	2702      	movs	r7, #2
 f80049a:	e799      	b.n	f8003d0 <flash_highspeed_setup+0x100>
 f80049c:	2101      	movs	r1, #1
 f80049e:	4f2b      	ldr	r7, [pc, #172]	; (f80054c <flash_highspeed_setup+0x27c>)
 f8004a0:	47b8      	blx	r7
 f8004a2:	f10d 0215 	add.w	r2, sp, #21
 f8004a6:	2101      	movs	r1, #1
 f8004a8:	f894 0045 	ldrb.w	r0, [r4, #69]	; 0x45
 f8004ac:	47b8      	blx	r7
 f8004ae:	e7a3      	b.n	f8003f8 <flash_highspeed_setup+0x128>
 f8004b0:	4b2c      	ldr	r3, [pc, #176]	; (f800564 <flash_highspeed_setup+0x294>)
 f8004b2:	4798      	blx	r3
 f8004b4:	e773      	b.n	f80039e <flash_highspeed_setup+0xce>
 f8004b6:	4b2c      	ldr	r3, [pc, #176]	; (f800568 <flash_highspeed_setup+0x298>)
 f8004b8:	4798      	blx	r3
 f8004ba:	4b22      	ldr	r3, [pc, #136]	; (f800544 <flash_highspeed_setup+0x274>)
 f8004bc:	2201      	movs	r2, #1
 f8004be:	701a      	strb	r2, [r3, #0]
 f8004c0:	e76d      	b.n	f80039e <flash_highspeed_setup+0xce>
 f8004c2:	4b2a      	ldr	r3, [pc, #168]	; (f80056c <flash_highspeed_setup+0x29c>)
 f8004c4:	4798      	blx	r3
 f8004c6:	f89d 301a 	ldrb.w	r3, [sp, #26]
 f8004ca:	2b15      	cmp	r3, #21
 f8004cc:	f67f af67 	bls.w	f80039e <flash_highspeed_setup+0xce>
 f8004d0:	2331      	movs	r3, #49	; 0x31
 f8004d2:	f884 3045 	strb.w	r3, [r4, #69]	; 0x45
 f8004d6:	e762      	b.n	f80039e <flash_highspeed_setup+0xce>
 f8004d8:	4b23      	ldr	r3, [pc, #140]	; (f800568 <flash_highspeed_setup+0x298>)
 f8004da:	4798      	blx	r3
 f8004dc:	e75f      	b.n	f80039e <flash_highspeed_setup+0xce>
 f8004de:	4b22      	ldr	r3, [pc, #136]	; (f800568 <flash_highspeed_setup+0x298>)
 f8004e0:	4798      	blx	r3
 f8004e2:	2300      	movs	r3, #0
 f8004e4:	60e3      	str	r3, [r4, #12]
 f8004e6:	e75a      	b.n	f80039e <flash_highspeed_setup+0xce>
 f8004e8:	4b21      	ldr	r3, [pc, #132]	; (f800570 <flash_highspeed_setup+0x2a0>)
 f8004ea:	2257      	movs	r2, #87	; 0x57
 f8004ec:	4912      	ldr	r1, [pc, #72]	; (f800538 <flash_highspeed_setup+0x268>)
 f8004ee:	2003      	movs	r0, #3
 f8004f0:	f002 f812 	bl	f802518 <rtk_log_write>
 f8004f4:	481f      	ldr	r0, [pc, #124]	; (f800574 <flash_highspeed_setup+0x2a4>)
 f8004f6:	2137      	movs	r1, #55	; 0x37
 f8004f8:	f002 fe2a 	bl	f803150 <io_assert_failed>
 f8004fc:	f8d8 0000 	ldr.w	r0, [r8]
 f800500:	e739      	b.n	f800376 <flash_highspeed_setup+0xa6>
 f800502:	2154      	movs	r1, #84	; 0x54
 f800504:	481b      	ldr	r0, [pc, #108]	; (f800574 <flash_highspeed_setup+0x2a4>)
 f800506:	f002 fe23 	bl	f803150 <io_assert_failed>
 f80050a:	f8d8 3000 	ldr.w	r3, [r8]
 f80050e:	691b      	ldr	r3, [r3, #16]
 f800510:	e744      	b.n	f80039c <flash_highspeed_setup+0xcc>
 f800512:	4b19      	ldr	r3, [pc, #100]	; (f800578 <flash_highspeed_setup+0x2a8>)
 f800514:	4798      	blx	r3
 f800516:	e742      	b.n	f80039e <flash_highspeed_setup+0xce>
 f800518:	00009271 	.word	0x00009271
 f80051c:	0f804844 	.word	0x0f804844
 f800520:	0c65d3ff 	.word	0x0c65d3ff
 f800524:	a530e8a5 	.word	0xa530e8a5
 f800528:	0f804846 	.word	0x0f804846
 f80052c:	40128000 	.word	0x40128000
 f800530:	20004014 	.word	0x20004014
 f800534:	00009c25 	.word	0x00009c25
 f800538:	0f804884 	.word	0x0f804884
 f80053c:	0f8048c4 	.word	0x0f8048c4
 f800540:	0000c301 	.word	0x0000c301
 f800544:	3000a1e2 	.word	0x3000a1e2
 f800548:	00011bbd 	.word	0x00011bbd
 f80054c:	00009fb9 	.word	0x00009fb9
 f800550:	41008000 	.word	0x41008000
 f800554:	0f8048bc 	.word	0x0f8048bc
 f800558:	0000baf5 	.word	0x0000baf5
 f80055c:	0000c219 	.word	0x0000c219
 f800560:	0f80494c 	.word	0x0f80494c
 f800564:	0000a201 	.word	0x0000a201
 f800568:	0000a26d 	.word	0x0000a26d
 f80056c:	0000a2d9 	.word	0x0000a2d9
 f800570:	0f8048d8 	.word	0x0f8048d8
 f800574:	0f8048fc 	.word	0x0f8048fc
 f800578:	0000a34d 	.word	0x0000a34d
 f80057c:	3000a1e4 	.word	0x3000a1e4

0f800580 <Combo_SPIC_Init>:
 f800580:	f44f 5100 	mov.w	r1, #8192	; 0x2000
 f800584:	2201      	movs	r2, #1
 f800586:	4b28      	ldr	r3, [pc, #160]	; (f800628 <Combo_SPIC_Init+0xa8>)
 f800588:	4608      	mov	r0, r1
 f80058a:	b5f0      	push	{r4, r5, r6, r7, lr}
 f80058c:	b083      	sub	sp, #12
 f80058e:	4798      	blx	r3
 f800590:	4b26      	ldr	r3, [pc, #152]	; (f80062c <Combo_SPIC_Init+0xac>)
 f800592:	2000      	movs	r0, #0
 f800594:	4798      	blx	r3
 f800596:	4a26      	ldr	r2, [pc, #152]	; (f800630 <Combo_SPIC_Init+0xb0>)
 f800598:	4926      	ldr	r1, [pc, #152]	; (f800634 <Combo_SPIC_Init+0xb4>)
 f80059a:	f8d2 324c 	ldr.w	r3, [r2, #588]	; 0x24c
 f80059e:	f891 5056 	ldrb.w	r5, [r1, #86]	; 0x56
 f8005a2:	2101      	movs	r1, #1
 f8005a4:	f043 4380 	orr.w	r3, r3, #1073741824	; 0x40000000
 f8005a8:	f8c2 324c 	str.w	r3, [r2, #588]	; 0x24c
 f8005ac:	bb25      	cbnz	r5, f8005f8 <Combo_SPIC_Init+0x78>
 f8005ae:	4b22      	ldr	r3, [pc, #136]	; (f800638 <Combo_SPIC_Init+0xb8>)
 f8005b0:	4608      	mov	r0, r1
 f8005b2:	4798      	blx	r3
 f8005b4:	4b21      	ldr	r3, [pc, #132]	; (f80063c <Combo_SPIC_Init+0xbc>)
 f8005b6:	466c      	mov	r4, sp
 f8005b8:	f10d 0705 	add.w	r7, sp, #5
 f8005bc:	4e20      	ldr	r6, [pc, #128]	; (f800640 <Combo_SPIC_Init+0xc0>)
 f8005be:	e893 0003 	ldmia.w	r3, {r0, r1}
 f8005c2:	9000      	str	r0, [sp, #0]
 f8005c4:	f8ad 1004 	strh.w	r1, [sp, #4]
 f8005c8:	e001      	b.n	f8005ce <Combo_SPIC_Init+0x4e>
 f8005ca:	f814 5f01 	ldrb.w	r5, [r4, #1]!
 f8005ce:	4628      	mov	r0, r5
 f8005d0:	2100      	movs	r1, #0
 f8005d2:	47b0      	blx	r6
 f8005d4:	42a7      	cmp	r7, r4
 f8005d6:	d1f8      	bne.n	f8005ca <Combo_SPIC_Init+0x4a>
 f8005d8:	4c1a      	ldr	r4, [pc, #104]	; (f800644 <Combo_SPIC_Init+0xc4>)
 f8005da:	4916      	ldr	r1, [pc, #88]	; (f800634 <Combo_SPIC_Init+0xb4>)
 f8005dc:	4620      	mov	r0, r4
 f8005de:	4b1a      	ldr	r3, [pc, #104]	; (f800648 <Combo_SPIC_Init+0xc8>)
 f8005e0:	4798      	blx	r3
 f8005e2:	2200      	movs	r2, #0
 f8005e4:	4913      	ldr	r1, [pc, #76]	; (f800634 <Combo_SPIC_Init+0xb4>)
 f8005e6:	4620      	mov	r0, r4
 f8005e8:	4b18      	ldr	r3, [pc, #96]	; (f80064c <Combo_SPIC_Init+0xcc>)
 f8005ea:	4798      	blx	r3
 f8005ec:	6823      	ldr	r3, [r4, #0]
 f8005ee:	f443 6300 	orr.w	r3, r3, #2048	; 0x800
 f8005f2:	6023      	str	r3, [r4, #0]
 f8005f4:	b003      	add	sp, #12
 f8005f6:	bdf0      	pop	{r4, r5, r6, r7, pc}
 f8005f8:	2000      	movs	r0, #0
 f8005fa:	4b0f      	ldr	r3, [pc, #60]	; (f800638 <Combo_SPIC_Init+0xb8>)
 f8005fc:	4798      	blx	r3
 f8005fe:	4a14      	ldr	r2, [pc, #80]	; (f800650 <Combo_SPIC_Init+0xd0>)
 f800600:	466c      	mov	r4, sp
 f800602:	f10d 0505 	add.w	r5, sp, #5
 f800606:	230d      	movs	r3, #13
 f800608:	4e0d      	ldr	r6, [pc, #52]	; (f800640 <Combo_SPIC_Init+0xc0>)
 f80060a:	e892 0003 	ldmia.w	r2, {r0, r1}
 f80060e:	9000      	str	r0, [sp, #0]
 f800610:	f8ad 1004 	strh.w	r1, [sp, #4]
 f800614:	e001      	b.n	f80061a <Combo_SPIC_Init+0x9a>
 f800616:	f814 3f01 	ldrb.w	r3, [r4, #1]!
 f80061a:	4618      	mov	r0, r3
 f80061c:	2100      	movs	r1, #0
 f80061e:	47b0      	blx	r6
 f800620:	42ac      	cmp	r4, r5
 f800622:	d1f8      	bne.n	f800616 <Combo_SPIC_Init+0x96>
 f800624:	e7d8      	b.n	f8005d8 <Combo_SPIC_Init+0x58>
 f800626:	bf00      	nop
 f800628:	0000bebd 	.word	0x0000bebd
 f80062c:	0000c001 	.word	0x0000c001
 f800630:	41008000 	.word	0x41008000
 f800634:	20004014 	.word	0x20004014
 f800638:	0000be9d 	.word	0x0000be9d
 f80063c:	0f80494c 	.word	0x0f80494c
 f800640:	0000baf5 	.word	0x0000baf5
 f800644:	40127000 	.word	0x40127000
 f800648:	00009a0d 	.word	0x00009a0d
 f80064c:	00009aa1 	.word	0x00009aa1
 f800650:	0f8048bc 	.word	0x0f8048bc

0f800654 <Combo_SPIC_Output_Div_Clk>:
 f800654:	4a12      	ldr	r2, [pc, #72]	; (f8006a0 <Combo_SPIC_Output_Div_Clk+0x4c>)
 f800656:	0201      	lsls	r1, r0, #8
 f800658:	230a      	movs	r3, #10
 f80065a:	f401 6170 	and.w	r1, r1, #3840	; 0xf00
 f80065e:	b410      	push	{r4}
 f800660:	f8d2 424c 	ldr.w	r4, [r2, #588]	; 0x24c
 f800664:	f424 5400 	bic.w	r4, r4, #8192	; 0x2000
 f800668:	f8c2 424c 	str.w	r4, [r2, #588]	; 0x24c
 f80066c:	f8d2 024c 	ldr.w	r0, [r2, #588]	; 0x24c
 f800670:	f420 6070 	bic.w	r0, r0, #3840	; 0xf00
 f800674:	4301      	orrs	r1, r0
 f800676:	f8c2 124c 	str.w	r1, [r2, #588]	; 0x24c
 f80067a:	f8d2 124c 	ldr.w	r1, [r2, #588]	; 0x24c
 f80067e:	f441 5100 	orr.w	r1, r1, #8192	; 0x2000
 f800682:	f8c2 124c 	str.w	r1, [r2, #588]	; 0x24c
 f800686:	e001      	b.n	f80068c <Combo_SPIC_Output_Div_Clk+0x38>
 f800688:	07c9      	lsls	r1, r1, #31
 f80068a:	d403      	bmi.n	f800694 <Combo_SPIC_Output_Div_Clk+0x40>
 f80068c:	3b01      	subs	r3, #1
 f80068e:	f8d2 124c 	ldr.w	r1, [r2, #588]	; 0x24c
 f800692:	d1f9      	bne.n	f800688 <Combo_SPIC_Output_Div_Clk+0x34>
 f800694:	2001      	movs	r0, #1
 f800696:	4b03      	ldr	r3, [pc, #12]	; (f8006a4 <Combo_SPIC_Output_Div_Clk+0x50>)
 f800698:	f85d 4b04 	ldr.w	r4, [sp], #4
 f80069c:	4718      	bx	r3
 f80069e:	bf00      	nop
 f8006a0:	41008000 	.word	0x41008000
 f8006a4:	0000c001 	.word	0x0000c001

0f8006a8 <CORE_LDO_Vol_Set>:
 f8006a8:	f04f 4282 	mov.w	r2, #1090519040	; 0x41000000
 f8006ac:	2801      	cmp	r0, #1
 f8006ae:	f8d2 311c 	ldr.w	r3, [r2, #284]	; 0x11c
 f8006b2:	d009      	beq.n	f8006c8 <CORE_LDO_Vol_Set+0x20>
 f8006b4:	b938      	cbnz	r0, f8006c6 <CORE_LDO_Vol_Set+0x1e>
 f8006b6:	2b00      	cmp	r3, #0
 f8006b8:	da05      	bge.n	f8006c6 <CORE_LDO_Vol_Set+0x1e>
 f8006ba:	4b0a      	ldr	r3, [pc, #40]	; (f8006e4 <CORE_LDO_Vol_Set+0x3c>)
 f8006bc:	2245      	movs	r2, #69	; 0x45
 f8006be:	490a      	ldr	r1, [pc, #40]	; (f8006e8 <CORE_LDO_Vol_Set+0x40>)
 f8006c0:	2002      	movs	r0, #2
 f8006c2:	f001 bf29 	b.w	f802518 <rtk_log_write>
 f8006c6:	4770      	bx	lr
 f8006c8:	2b00      	cmp	r3, #0
 f8006ca:	dbfc      	blt.n	f8006c6 <CORE_LDO_Vol_Set+0x1e>
 f8006cc:	4907      	ldr	r1, [pc, #28]	; (f8006ec <CORE_LDO_Vol_Set+0x44>)
 f8006ce:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
 f8006d2:	f8c2 311c 	str.w	r3, [r2, #284]	; 0x11c
 f8006d6:	f8d1 3370 	ldr.w	r3, [r1, #880]	; 0x370
 f8006da:	f043 0301 	orr.w	r3, r3, #1
 f8006de:	f8c1 3370 	str.w	r3, [r1, #880]	; 0x370
 f8006e2:	4770      	bx	lr
 f8006e4:	0f804954 	.word	0x0f804954
 f8006e8:	0f804988 	.word	0x0f804988
 f8006ec:	41008000 	.word	0x41008000

0f8006f0 <LDO_Mem_Dummy_Ctrl>:
 f8006f0:	4b05      	ldr	r3, [pc, #20]	; (f800708 <LDO_Mem_Dummy_Ctrl+0x18>)
 f8006f2:	0280      	lsls	r0, r0, #10
 f8006f4:	6a1a      	ldr	r2, [r3, #32]
 f8006f6:	f400 6040 	and.w	r0, r0, #3072	; 0xc00
 f8006fa:	f422 6240 	bic.w	r2, r2, #3072	; 0xc00
 f8006fe:	621a      	str	r2, [r3, #32]
 f800700:	6a1a      	ldr	r2, [r3, #32]
 f800702:	4310      	orrs	r0, r2
 f800704:	6218      	str	r0, [r3, #32]
 f800706:	4770      	bx	lr
 f800708:	41008100 	.word	0x41008100

0f80070c <PLL_Cmd.part.0>:
 f80070c:	b510      	push	{r4, lr}
 f80070e:	4c10      	ldr	r4, [pc, #64]	; (f800750 <PLL_Cmd.part.0+0x44>)
 f800710:	2001      	movs	r0, #1
 f800712:	4a10      	ldr	r2, [pc, #64]	; (f800754 <PLL_Cmd.part.0+0x48>)
 f800714:	6c23      	ldr	r3, [r4, #64]	; 0x40
 f800716:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
 f80071a:	6423      	str	r3, [r4, #64]	; 0x40
 f80071c:	4790      	blx	r2
 f80071e:	6d63      	ldr	r3, [r4, #84]	; 0x54
 f800720:	4a0d      	ldr	r2, [pc, #52]	; (f800758 <PLL_Cmd.part.0+0x4c>)
 f800722:	f043 0318 	orr.w	r3, r3, #24
 f800726:	6563      	str	r3, [r4, #84]	; 0x54
 f800728:	6d63      	ldr	r3, [r4, #84]	; 0x54
 f80072a:	f443 4380 	orr.w	r3, r3, #16384	; 0x4000
 f80072e:	6563      	str	r3, [r4, #84]	; 0x54
 f800730:	6c23      	ldr	r3, [r4, #64]	; 0x40
 f800732:	f043 5300 	orr.w	r3, r3, #536870912	; 0x20000000
 f800736:	6423      	str	r3, [r4, #64]	; 0x40
 f800738:	6c23      	ldr	r3, [r4, #64]	; 0x40
 f80073a:	f443 5380 	orr.w	r3, r3, #4096	; 0x1000
 f80073e:	6423      	str	r3, [r4, #64]	; 0x40
 f800740:	4790      	blx	r2
 f800742:	2801      	cmp	r0, #1
 f800744:	d002      	beq.n	f80074c <PLL_Cmd.part.0+0x40>
 f800746:	6f63      	ldr	r3, [r4, #116]	; 0x74
 f800748:	07db      	lsls	r3, r3, #31
 f80074a:	d5fc      	bpl.n	f800746 <PLL_Cmd.part.0+0x3a>
 f80074c:	bd10      	pop	{r4, pc}
 f80074e:	bf00      	nop
 f800750:	41008400 	.word	0x41008400
 f800754:	000094a5 	.word	0x000094a5
 f800758:	0000c219 	.word	0x0000c219

0f80075c <PLL_ClkSet>:
 f80075c:	b570      	push	{r4, r5, r6, lr}
 f80075e:	4b30      	ldr	r3, [pc, #192]	; (f800820 <PLL_ClkSet+0xc4>)
 f800760:	4605      	mov	r5, r0
 f800762:	4798      	blx	r3
 f800764:	4b2f      	ldr	r3, [pc, #188]	; (f800824 <PLL_ClkSet+0xc8>)
 f800766:	4c30      	ldr	r4, [pc, #192]	; (f800828 <PLL_ClkSet+0xcc>)
 f800768:	4606      	mov	r6, r0
 f80076a:	6c1a      	ldr	r2, [r3, #64]	; 0x40
 f80076c:	2001      	movs	r0, #1
 f80076e:	42a5      	cmp	r5, r4
 f800770:	4c2c      	ldr	r4, [pc, #176]	; (f800824 <PLL_ClkSet+0xc8>)
 f800772:	f422 5280 	bic.w	r2, r2, #4096	; 0x1000
 f800776:	641a      	str	r2, [r3, #64]	; 0x40
 f800778:	6c1a      	ldr	r2, [r3, #64]	; 0x40
 f80077a:	f022 5200 	bic.w	r2, r2, #536870912	; 0x20000000
 f80077e:	641a      	str	r2, [r3, #64]	; 0x40
 f800780:	6d5a      	ldr	r2, [r3, #84]	; 0x54
 f800782:	f022 0218 	bic.w	r2, r2, #24
 f800786:	655a      	str	r2, [r3, #84]	; 0x54
 f800788:	6d5a      	ldr	r2, [r3, #84]	; 0x54
 f80078a:	f422 4280 	bic.w	r2, r2, #16384	; 0x4000
 f80078e:	655a      	str	r2, [r3, #84]	; 0x54
 f800790:	6c1a      	ldr	r2, [r3, #64]	; 0x40
 f800792:	f022 4200 	bic.w	r2, r2, #2147483648	; 0x80000000
 f800796:	641a      	str	r2, [r3, #64]	; 0x40
 f800798:	6c1a      	ldr	r2, [r3, #64]	; 0x40
 f80079a:	f422 22e0 	bic.w	r2, r2, #458752	; 0x70000
 f80079e:	641a      	str	r2, [r3, #64]	; 0x40
 f8007a0:	6c1a      	ldr	r2, [r3, #64]	; 0x40
 f8007a2:	bf8c      	ite	hi
 f8007a4:	f442 22a0 	orrhi.w	r2, r2, #327680	; 0x50000
 f8007a8:	f442 22c0 	orrls.w	r2, r2, #393216	; 0x60000
 f8007ac:	641a      	str	r2, [r3, #64]	; 0x40
 f8007ae:	4b1f      	ldr	r3, [pc, #124]	; (f80082c <PLL_ClkSet+0xd0>)
 f8007b0:	4798      	blx	r3
 f8007b2:	08f2      	lsrs	r2, r6, #3
 f8007b4:	fbb5 f3f6 	udiv	r3, r5, r6
 f8007b8:	fb06 5113 	mls	r1, r6, r3, r5
 f8007bc:	1e9e      	subs	r6, r3, #2
 f8007be:	6c23      	ldr	r3, [r4, #64]	; 0x40
 f8007c0:	0176      	lsls	r6, r6, #5
 f8007c2:	4d1b      	ldr	r5, [pc, #108]	; (f800830 <PLL_ClkSet+0xd4>)
 f8007c4:	f423 5380 	bic.w	r3, r3, #4096	; 0x1000
 f8007c8:	f406 66fc 	and.w	r6, r6, #2016	; 0x7e0
 f8007cc:	6423      	str	r3, [r4, #64]	; 0x40
 f8007ce:	6c63      	ldr	r3, [r4, #68]	; 0x44
 f8007d0:	f423 63fc 	bic.w	r3, r3, #2016	; 0x7e0
 f8007d4:	431e      	orrs	r6, r3
 f8007d6:	fbb1 f3f2 	udiv	r3, r1, r2
 f8007da:	6466      	str	r6, [r4, #68]	; 0x44
 f8007dc:	fb02 1113 	mls	r1, r2, r3, r1
 f8007e0:	6ce6      	ldr	r6, [r4, #76]	; 0x4c
 f8007e2:	041b      	lsls	r3, r3, #16
 f8007e4:	b2b6      	uxth	r6, r6
 f8007e6:	f403 23e0 	and.w	r3, r3, #458752	; 0x70000
 f8007ea:	0348      	lsls	r0, r1, #13
 f8007ec:	0cc9      	lsrs	r1, r1, #19
 f8007ee:	431e      	orrs	r6, r3
 f8007f0:	2300      	movs	r3, #0
 f8007f2:	f003 fa81 	bl	f803cf8 <__aeabi_uldivmod>
 f8007f6:	4603      	mov	r3, r0
 f8007f8:	2001      	movs	r0, #1
 f8007fa:	ea46 46c3 	orr.w	r6, r6, r3, lsl #19
 f8007fe:	64e6      	str	r6, [r4, #76]	; 0x4c
 f800800:	47a8      	blx	r5
 f800802:	6c63      	ldr	r3, [r4, #68]	; 0x44
 f800804:	203c      	movs	r0, #60	; 0x3c
 f800806:	f443 4300 	orr.w	r3, r3, #32768	; 0x8000
 f80080a:	6463      	str	r3, [r4, #68]	; 0x44
 f80080c:	6c63      	ldr	r3, [r4, #68]	; 0x44
 f80080e:	f423 4300 	bic.w	r3, r3, #32768	; 0x8000
 f800812:	6463      	str	r3, [r4, #68]	; 0x44
 f800814:	47a8      	blx	r5
 f800816:	6c23      	ldr	r3, [r4, #64]	; 0x40
 f800818:	f443 5380 	orr.w	r3, r3, #4096	; 0x1000
 f80081c:	6423      	str	r3, [r4, #64]	; 0x40
 f80081e:	bd70      	pop	{r4, r5, r6, pc}
 f800820:	00009255 	.word	0x00009255
 f800824:	41008400 	.word	0x41008400
 f800828:	23c34600 	.word	0x23c34600
 f80082c:	0f80070d 	.word	0x0f80070d
 f800830:	000094a5 	.word	0x000094a5
 f800834:	00000000 	.word	0x00000000

0f800838 <PSRAM_CLK_Update>:
 f800838:	b570      	push	{r4, r5, r6, lr}
 f80083a:	4c4b      	ldr	r4, [pc, #300]	; (f800968 <PSRAM_CLK_Update+0x130>)
 f80083c:	47a0      	blx	r4
 f80083e:	f410 3f40 	tst.w	r0, #196608	; 0x30000
 f800842:	d103      	bne.n	f80084c <PSRAM_CLK_Update+0x14>
 f800844:	47a0      	blx	r4
 f800846:	f410 2f40 	tst.w	r0, #786432	; 0xc0000
 f80084a:	d136      	bne.n	f8008ba <PSRAM_CLK_Update+0x82>
 f80084c:	47a0      	blx	r4
 f80084e:	f440 1080 	orr.w	r0, r0, #1048576	; 0x100000
 f800852:	4b46      	ldr	r3, [pc, #280]	; (f80096c <PSRAM_CLK_Update+0x134>)
 f800854:	f400 01fe 	and.w	r1, r0, #8323072	; 0x7f0000
 f800858:	4c45      	ldr	r4, [pc, #276]	; (f800970 <PSRAM_CLK_Update+0x138>)
 f80085a:	b2c0      	uxtb	r0, r0
 f80085c:	f8d3 2218 	ldr.w	r2, [r3, #536]	; 0x218
 f800860:	60e0      	str	r0, [r4, #12]
 f800862:	0650      	lsls	r0, r2, #25
 f800864:	61e1      	str	r1, [r4, #28]
 f800866:	d42a      	bmi.n	f8008be <PSRAM_CLK_Update+0x86>
 f800868:	4b42      	ldr	r3, [pc, #264]	; (f800974 <PSRAM_CLK_Update+0x13c>)
 f80086a:	4798      	blx	r3
 f80086c:	4602      	mov	r2, r0
 f80086e:	2a00      	cmp	r2, #0
 f800870:	d030      	beq.n	f8008d4 <PSRAM_CLK_Update+0x9c>
 f800872:	0855      	lsrs	r5, r2, #1
 f800874:	2600      	movs	r6, #0
 f800876:	a13a      	add	r1, pc, #232	; (adr r1, f800960 <PSRAM_CLK_Update+0x128>)
 f800878:	e9d1 0100 	ldrd	r0, r1, [r1]
 f80087c:	4633      	mov	r3, r6
 f80087e:	6025      	str	r5, [r4, #0]
 f800880:	f003 f9ea 	bl	f803c58 <__aeabi_ldivmod>
 f800884:	69e2      	ldr	r2, [r4, #28]
 f800886:	6060      	str	r0, [r4, #4]
 f800888:	0391      	lsls	r1, r2, #14
 f80088a:	d50c      	bpl.n	f8008a6 <PSRAM_CLK_Update+0x6e>
 f80088c:	4b3a      	ldr	r3, [pc, #232]	; (f800978 <PSRAM_CLK_Update+0x140>)
 f80088e:	429d      	cmp	r5, r3
 f800890:	d929      	bls.n	f8008e6 <PSRAM_CLK_Update+0xae>
 f800892:	4b3a      	ldr	r3, [pc, #232]	; (f80097c <PSRAM_CLK_Update+0x144>)
 f800894:	429d      	cmp	r5, r3
 f800896:	d82d      	bhi.n	f8008f4 <PSRAM_CLK_Update+0xbc>
 f800898:	2104      	movs	r1, #4
 f80089a:	2226      	movs	r2, #38	; 0x26
 f80089c:	230f      	movs	r3, #15
 f80089e:	60a1      	str	r1, [r4, #8]
 f8008a0:	e9c4 2305 	strd	r2, r3, [r4, #20]
 f8008a4:	bd70      	pop	{r4, r5, r6, pc}
 f8008a6:	03d3      	lsls	r3, r2, #15
 f8008a8:	d5fc      	bpl.n	f8008a4 <PSRAM_CLK_Update+0x6c>
 f8008aa:	4b34      	ldr	r3, [pc, #208]	; (f80097c <PSRAM_CLK_Update+0x144>)
 f8008ac:	429d      	cmp	r5, r3
 f8008ae:	d82e      	bhi.n	f80090e <PSRAM_CLK_Update+0xd6>
 f8008b0:	2204      	movs	r2, #4
 f8008b2:	230f      	movs	r3, #15
 f8008b4:	60a2      	str	r2, [r4, #8]
 f8008b6:	6123      	str	r3, [r4, #16]
 f8008b8:	bd70      	pop	{r4, r5, r6, pc}
 f8008ba:	47a0      	blx	r4
 f8008bc:	e7c9      	b.n	f800852 <PSRAM_CLK_Update+0x1a>
 f8008be:	f8d3 524c 	ldr.w	r5, [r3, #588]	; 0x24c
 f8008c2:	4b2f      	ldr	r3, [pc, #188]	; (f800980 <PSRAM_CLK_Update+0x148>)
 f8008c4:	f3c5 2503 	ubfx	r5, r5, #8, #4
 f8008c8:	4798      	blx	r3
 f8008ca:	1c6a      	adds	r2, r5, #1
 f8008cc:	fbb0 f2f2 	udiv	r2, r0, r2
 f8008d0:	2a00      	cmp	r2, #0
 f8008d2:	d1ce      	bne.n	f800872 <PSRAM_CLK_Update+0x3a>
 f8008d4:	4b2b      	ldr	r3, [pc, #172]	; (f800984 <PSRAM_CLK_Update+0x14c>)
 f8008d6:	2245      	movs	r2, #69	; 0x45
 f8008d8:	2002      	movs	r0, #2
 f8008da:	f103 0114 	add.w	r1, r3, #20
 f8008de:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
 f8008e2:	f001 be19 	b.w	f802518 <rtk_log_write>
 f8008e6:	2103      	movs	r1, #3
 f8008e8:	2226      	movs	r2, #38	; 0x26
 f8008ea:	230e      	movs	r3, #14
 f8008ec:	60a1      	str	r1, [r4, #8]
 f8008ee:	e9c4 2305 	strd	r2, r3, [r4, #20]
 f8008f2:	bd70      	pop	{r4, r5, r6, pc}
 f8008f4:	4b24      	ldr	r3, [pc, #144]	; (f800988 <PSRAM_CLK_Update+0x150>)
 f8008f6:	429d      	cmp	r5, r3
 f8008f8:	d919      	bls.n	f80092e <PSRAM_CLK_Update+0xf6>
 f8008fa:	4b24      	ldr	r3, [pc, #144]	; (f80098c <PSRAM_CLK_Update+0x154>)
 f8008fc:	429d      	cmp	r5, r3
 f8008fe:	d8d1      	bhi.n	f8008a4 <PSRAM_CLK_Update+0x6c>
 f800900:	2106      	movs	r1, #6
 f800902:	2224      	movs	r2, #36	; 0x24
 f800904:	2301      	movs	r3, #1
 f800906:	60a1      	str	r1, [r4, #8]
 f800908:	e9c4 2305 	strd	r2, r3, [r4, #20]
 f80090c:	bd70      	pop	{r4, r5, r6, pc}
 f80090e:	4b1e      	ldr	r3, [pc, #120]	; (f800988 <PSRAM_CLK_Update+0x150>)
 f800910:	429d      	cmp	r5, r3
 f800912:	d907      	bls.n	f800924 <PSRAM_CLK_Update+0xec>
 f800914:	4b1e      	ldr	r3, [pc, #120]	; (f800990 <PSRAM_CLK_Update+0x158>)
 f800916:	429d      	cmp	r5, r3
 f800918:	d80f      	bhi.n	f80093a <PSRAM_CLK_Update+0x102>
 f80091a:	2206      	movs	r2, #6
 f80091c:	2312      	movs	r3, #18
 f80091e:	60a2      	str	r2, [r4, #8]
 f800920:	6123      	str	r3, [r4, #16]
 f800922:	bd70      	pop	{r4, r5, r6, pc}
 f800924:	2205      	movs	r2, #5
 f800926:	230f      	movs	r3, #15
 f800928:	60a2      	str	r2, [r4, #8]
 f80092a:	6123      	str	r3, [r4, #16]
 f80092c:	bd70      	pop	{r4, r5, r6, pc}
 f80092e:	2205      	movs	r2, #5
 f800930:	2326      	movs	r3, #38	; 0x26
 f800932:	61a6      	str	r6, [r4, #24]
 f800934:	60a2      	str	r2, [r4, #8]
 f800936:	6163      	str	r3, [r4, #20]
 f800938:	bd70      	pop	{r4, r5, r6, pc}
 f80093a:	4b14      	ldr	r3, [pc, #80]	; (f80098c <PSRAM_CLK_Update+0x154>)
 f80093c:	429d      	cmp	r5, r3
 f80093e:	d804      	bhi.n	f80094a <PSRAM_CLK_Update+0x112>
 f800940:	2207      	movs	r2, #7
 f800942:	2314      	movs	r3, #20
 f800944:	60a2      	str	r2, [r4, #8]
 f800946:	6123      	str	r3, [r4, #16]
 f800948:	bd70      	pop	{r4, r5, r6, pc}
 f80094a:	4b12      	ldr	r3, [pc, #72]	; (f800994 <PSRAM_CLK_Update+0x15c>)
 f80094c:	429d      	cmp	r5, r3
 f80094e:	d8a9      	bhi.n	f8008a4 <PSRAM_CLK_Update+0x6c>
 f800950:	2208      	movs	r2, #8
 f800952:	231c      	movs	r3, #28
 f800954:	60a2      	str	r2, [r4, #8]
 f800956:	6123      	str	r3, [r4, #16]
 f800958:	bd70      	pop	{r4, r5, r6, pc}
 f80095a:	bf00      	nop
 f80095c:	f3af 8000 	nop.w
 f800960:	d4a51000 	.word	0xd4a51000
 f800964:	000000e8 	.word	0x000000e8
 f800968:	0f802625 	.word	0x0f802625
 f80096c:	41008000 	.word	0x41008000
 f800970:	3000a1e8 	.word	0x3000a1e8
 f800974:	00009255 	.word	0x00009255
 f800978:	04f27ac0 	.word	0x04f27ac0
 f80097c:	0632ea00 	.word	0x0632ea00
 f800980:	00009271 	.word	0x00009271
 f800984:	0f804990 	.word	0x0f804990
 f800988:	07ed6b40 	.word	0x07ed6b40
 f80098c:	0bebc200 	.word	0x0bebc200
 f800990:	09e4f580 	.word	0x09e4f580
 f800994:	0ee6b280 	.word	0x0ee6b280

0f800998 <PSRAM_PHY_StructInit>:
 f800998:	2300      	movs	r3, #0
 f80099a:	2201      	movs	r2, #1
 f80099c:	2103      	movs	r1, #3
 f80099e:	6203      	str	r3, [r0, #32]
 f8009a0:	b430      	push	{r4, r5}
 f8009a2:	2502      	movs	r5, #2
 f8009a4:	240a      	movs	r4, #10
 f8009a6:	e9c0 3500 	strd	r3, r5, [r0]
 f8009aa:	e9c0 4204 	strd	r4, r2, [r0, #16]
 f8009ae:	e9c0 1102 	strd	r1, r1, [r0, #8]
 f8009b2:	bc30      	pop	{r4, r5}
 f8009b4:	e9c0 2306 	strd	r2, r3, [r0, #24]
 f8009b8:	e9c0 2309 	strd	r2, r3, [r0, #36]	; 0x24
 f8009bc:	4770      	bx	lr
 f8009be:	bf00      	nop

0f8009c0 <PSRAM_PHY_Init>:
 f8009c0:	b430      	push	{r4, r5}
 f8009c2:	6845      	ldr	r5, [r0, #4]
 f8009c4:	6903      	ldr	r3, [r0, #16]
 f8009c6:	6881      	ldr	r1, [r0, #8]
 f8009c8:	ea4f 6c05 	mov.w	ip, r5, lsl #24
 f8009cc:	6804      	ldr	r4, [r0, #0]
 f8009ce:	f003 031f 	and.w	r3, r3, #31
 f8009d2:	0409      	lsls	r1, r1, #16
 f8009d4:	f00c 6ce0 	and.w	ip, ip, #117440512	; 0x7000000
 f8009d8:	ea43 7304 	orr.w	r3, r3, r4, lsl #28
 f8009dc:	4a14      	ldr	r2, [pc, #80]	; (f800a30 <PSRAM_PHY_Init+0x70>)
 f8009de:	f401 2470 	and.w	r4, r1, #983040	; 0xf0000
 f8009e2:	68c1      	ldr	r1, [r0, #12]
 f8009e4:	ea43 030c 	orr.w	r3, r3, ip
 f8009e8:	0209      	lsls	r1, r1, #8
 f8009ea:	4323      	orrs	r3, r4
 f8009ec:	f401 6170 	and.w	r1, r1, #3840	; 0xf00
 f8009f0:	430b      	orrs	r3, r1
 f8009f2:	6053      	str	r3, [r2, #4]
 f8009f4:	6a83      	ldr	r3, [r0, #40]	; 0x28
 f8009f6:	6945      	ldr	r5, [r0, #20]
 f8009f8:	f003 030f 	and.w	r3, r3, #15
 f8009fc:	e9d0 4106 	ldrd	r4, r1, [r0, #24]
 f800a00:	ea43 73c5 	orr.w	r3, r3, r5, lsl #31
 f800a04:	03a4      	lsls	r4, r4, #14
 f800a06:	0349      	lsls	r1, r1, #13
 f800a08:	f404 4480 	and.w	r4, r4, #16384	; 0x4000
 f800a0c:	f401 5c00 	and.w	ip, r1, #8192	; 0x2000
 f800a10:	4323      	orrs	r3, r4
 f800a12:	e9d0 4108 	ldrd	r4, r1, [r0, #32]
 f800a16:	ea43 030c 	orr.w	r3, r3, ip
 f800a1a:	0320      	lsls	r0, r4, #12
 f800a1c:	0109      	lsls	r1, r1, #4
 f800a1e:	f400 5080 	and.w	r0, r0, #4096	; 0x1000
 f800a22:	b2c9      	uxtb	r1, r1
 f800a24:	4303      	orrs	r3, r0
 f800a26:	430b      	orrs	r3, r1
 f800a28:	6193      	str	r3, [r2, #24]
 f800a2a:	bc30      	pop	{r4, r5}
 f800a2c:	4770      	bx	lr
 f800a2e:	bf00      	nop
 f800a30:	40126000 	.word	0x40126000

0f800a34 <PSRAM_CTRL_Init>:
 f800a34:	b470      	push	{r4, r5, r6}
 f800a36:	4b49      	ldr	r3, [pc, #292]	; (f800b5c <PSRAM_CTRL_Init+0x128>)
 f800a38:	2500      	movs	r5, #0
 f800a3a:	4949      	ldr	r1, [pc, #292]	; (f800b60 <PSRAM_CTRL_Init+0x12c>)
 f800a3c:	2401      	movs	r4, #1
 f800a3e:	4a49      	ldr	r2, [pc, #292]	; (f800b64 <PSRAM_CTRL_Init+0x130>)
 f800a40:	f24d 0001 	movw	r0, #53249	; 0xd001
 f800a44:	609d      	str	r5, [r3, #8]
 f800a46:	6019      	str	r1, [r3, #0]
 f800a48:	69d1      	ldr	r1, [r2, #28]
 f800a4a:	615c      	str	r4, [r3, #20]
 f800a4c:	0389      	lsls	r1, r1, #14
 f800a4e:	f8c3 0120 	str.w	r0, [r3, #288]	; 0x120
 f800a52:	d54f      	bpl.n	f800af4 <PSRAM_CTRL_Init+0xc0>
 f800a54:	6855      	ldr	r5, [r2, #4]
 f800a56:	f44f 767a 	mov.w	r6, #1000	; 0x3e8
 f800a5a:	4943      	ldr	r1, [pc, #268]	; (f800b68 <PSRAM_CTRL_Init+0x134>)
 f800a5c:	6950      	ldr	r0, [r2, #20]
 f800a5e:	68d4      	ldr	r4, [r2, #12]
 f800a60:	f8c3 1110 	str.w	r1, [r3, #272]	; 0x110
 f800a64:	1e69      	subs	r1, r5, #1
 f800a66:	f004 040f 	and.w	r4, r4, #15
 f800a6a:	fb06 1100 	mla	r1, r6, r0, r1
 f800a6e:	483f      	ldr	r0, [pc, #252]	; (f800b6c <PSRAM_CTRL_Init+0x138>)
 f800a70:	26a0      	movs	r6, #160	; 0xa0
 f800a72:	fbb1 f1f5 	udiv	r1, r1, r5
 f800a76:	3906      	subs	r1, #6
 f800a78:	fbb0 f5f5 	udiv	r5, r0, r5
 f800a7c:	0188      	lsls	r0, r1, #6
 f800a7e:	f001 013f 	and.w	r1, r1, #63	; 0x3f
 f800a82:	f400 607c 	and.w	r0, r0, #4032	; 0xfc0
 f800a86:	ea40 6005 	orr.w	r0, r0, r5, lsl #24
 f800a8a:	f641 4570 	movw	r5, #7280	; 0x1c70
 f800a8e:	4325      	orrs	r5, r4
 f800a90:	4301      	orrs	r1, r0
 f800a92:	4837      	ldr	r0, [pc, #220]	; (f800b70 <PSRAM_CTRL_Init+0x13c>)
 f800a94:	f8c3 5134 	str.w	r5, [r3, #308]	; 0x134
 f800a98:	f441 1180 	orr.w	r1, r1, #1048576	; 0x100000
 f800a9c:	f8d3 4140 	ldr.w	r4, [r3, #320]	; 0x140
 f800aa0:	4020      	ands	r0, r4
 f800aa2:	2420      	movs	r4, #32
 f800aa4:	f8c3 0140 	str.w	r0, [r3, #320]	; 0x140
 f800aa8:	f44f 20a0 	mov.w	r0, #327680	; 0x50000
 f800aac:	f8c3 1138 	str.w	r1, [r3, #312]	; 0x138
 f800ab0:	f44f 21a2 	mov.w	r1, #331776	; 0x51000
 f800ab4:	f8c3 60e0 	str.w	r6, [r3, #224]	; 0xe0
 f800ab8:	f8c3 40f4 	str.w	r4, [r3, #244]	; 0xf4
 f800abc:	f8c3 011c 	str.w	r0, [r3, #284]	; 0x11c
 f800ac0:	f8c3 1118 	str.w	r1, [r3, #280]	; 0x118
 f800ac4:	6893      	ldr	r3, [r2, #8]
 f800ac6:	f640 74fe 	movw	r4, #4094	; 0xffe
 f800aca:	4924      	ldr	r1, [pc, #144]	; (f800b5c <PSRAM_CTRL_Init+0x128>)
 f800acc:	005b      	lsls	r3, r3, #1
 f800ace:	f8d1 011c 	ldr.w	r0, [r1, #284]	; 0x11c
 f800ad2:	1e5a      	subs	r2, r3, #1
 f800ad4:	3b02      	subs	r3, #2
 f800ad6:	f3c2 020b 	ubfx	r2, r2, #0, #12
 f800ada:	4023      	ands	r3, r4
 f800adc:	4310      	orrs	r0, r2
 f800ade:	f8c1 011c 	str.w	r0, [r1, #284]	; 0x11c
 f800ae2:	f8c1 313c 	str.w	r3, [r1, #316]	; 0x13c
 f800ae6:	f8d1 3118 	ldr.w	r3, [r1, #280]	; 0x118
 f800aea:	431a      	orrs	r2, r3
 f800aec:	f8c1 2118 	str.w	r2, [r1, #280]	; 0x118
 f800af0:	bc70      	pop	{r4, r5, r6}
 f800af2:	4770      	bx	lr
 f800af4:	6910      	ldr	r0, [r2, #16]
 f800af6:	f44f 717a 	mov.w	r1, #1000	; 0x3e8
 f800afa:	6854      	ldr	r4, [r2, #4]
 f800afc:	f647 2c12 	movw	ip, #31250	; 0x7a12
 f800b00:	fb00 f101 	mul.w	r1, r0, r1
 f800b04:	68d0      	ldr	r0, [r2, #12]
 f800b06:	f242 0620 	movw	r6, #8224	; 0x2020
 f800b0a:	f24a 05a0 	movw	r5, #41120	; 0xa0a0
 f800b0e:	fbb1 f1f4 	udiv	r1, r1, r4
 f800b12:	fbbc fcf4 	udiv	ip, ip, r4
 f800b16:	f000 040f 	and.w	r4, r0, #15
 f800b1a:	4816      	ldr	r0, [pc, #88]	; (f800b74 <PSRAM_CTRL_Init+0x140>)
 f800b1c:	f8c3 0110 	str.w	r0, [r3, #272]	; 0x110
 f800b20:	0188      	lsls	r0, r1, #6
 f800b22:	f444 648e 	orr.w	r4, r4, #1136	; 0x470
 f800b26:	f001 013f 	and.w	r1, r1, #63	; 0x3f
 f800b2a:	f400 607c 	and.w	r0, r0, #4032	; 0xfc0
 f800b2e:	f8c3 4134 	str.w	r4, [r3, #308]	; 0x134
 f800b32:	ea40 600c 	orr.w	r0, r0, ip, lsl #24
 f800b36:	4c10      	ldr	r4, [pc, #64]	; (f800b78 <PSRAM_CTRL_Init+0x144>)
 f800b38:	4301      	orrs	r1, r0
 f800b3a:	f44f 2080 	mov.w	r0, #262144	; 0x40000
 f800b3e:	430c      	orrs	r4, r1
 f800b40:	f44f 2184 	mov.w	r1, #270336	; 0x42000
 f800b44:	f8c3 4138 	str.w	r4, [r3, #312]	; 0x138
 f800b48:	f8c3 60e0 	str.w	r6, [r3, #224]	; 0xe0
 f800b4c:	f8c3 50f4 	str.w	r5, [r3, #244]	; 0xf4
 f800b50:	f8c3 011c 	str.w	r0, [r3, #284]	; 0x11c
 f800b54:	f8c3 1118 	str.w	r1, [r3, #280]	; 0x118
 f800b58:	e7b4      	b.n	f800ac4 <PSRAM_CTRL_Init+0x90>
 f800b5a:	bf00      	nop
 f800b5c:	40127000 	.word	0x40127000
 f800b60:	00bfe000 	.word	0x00bfe000
 f800b64:	3000a1e8 	.word	0x3000a1e8
 f800b68:	00135551 	.word	0x00135551
 f800b6c:	0001e848 	.word	0x0001e848
 f800b70:	ff0000ff 	.word	0xff0000ff
 f800b74:	00131551 	.word	0x00131551
 f800b78:	00101000 	.word	0x00101000

0f800b7c <PSRAM_InfoDump>:
 f800b7c:	e92d 4370 	stmdb	sp!, {r4, r5, r6, r8, r9, lr}
 f800b80:	4c93      	ldr	r4, [pc, #588]	; (f800dd0 <PSRAM_InfoDump+0x254>)
 f800b82:	b086      	sub	sp, #24
 f800b84:	f04f 0800 	mov.w	r8, #0
 f800b88:	4e92      	ldr	r6, [pc, #584]	; (f800dd4 <PSRAM_InfoDump+0x258>)
 f800b8a:	f104 0514 	add.w	r5, r4, #20
 f800b8e:	4b92      	ldr	r3, [pc, #584]	; (f800dd8 <PSRAM_InfoDump+0x25c>)
 f800b90:	f8ad 800c 	strh.w	r8, [sp, #12]
 f800b94:	f8ad 8010 	strh.w	r8, [sp, #16]
 f800b98:	f8ad 8014 	strh.w	r8, [sp, #20]
 f800b9c:	4798      	blx	r3
 f800b9e:	f104 031c 	add.w	r3, r4, #28
 f800ba2:	4629      	mov	r1, r5
 f800ba4:	9000      	str	r0, [sp, #0]
 f800ba6:	2249      	movs	r2, #73	; 0x49
 f800ba8:	2004      	movs	r0, #4
 f800baa:	f8df 926c 	ldr.w	r9, [pc, #620]	; f800e18 <PSRAM_InfoDump+0x29c>
 f800bae:	f001 fcb3 	bl	f802518 <rtk_log_write>
 f800bb2:	68b2      	ldr	r2, [r6, #8]
 f800bb4:	4b89      	ldr	r3, [pc, #548]	; (f800ddc <PSRAM_InfoDump+0x260>)
 f800bb6:	4629      	mov	r1, r5
 f800bb8:	2004      	movs	r0, #4
 f800bba:	e9cd 3200 	strd	r3, r2, [sp]
 f800bbe:	f104 032c 	add.w	r3, r4, #44	; 0x2c
 f800bc2:	2249      	movs	r2, #73	; 0x49
 f800bc4:	f001 fca8 	bl	f802518 <rtk_log_write>
 f800bc8:	6832      	ldr	r2, [r6, #0]
 f800bca:	f104 0340 	add.w	r3, r4, #64	; 0x40
 f800bce:	4629      	mov	r1, r5
 f800bd0:	2004      	movs	r0, #4
 f800bd2:	e9cd 6200 	strd	r6, r2, [sp]
 f800bd6:	2249      	movs	r2, #73	; 0x49
 f800bd8:	f001 fc9e 	bl	f802518 <rtk_log_write>
 f800bdc:	6972      	ldr	r2, [r6, #20]
 f800bde:	4b80      	ldr	r3, [pc, #512]	; (f800de0 <PSRAM_InfoDump+0x264>)
 f800be0:	4629      	mov	r1, r5
 f800be2:	2004      	movs	r0, #4
 f800be4:	e9cd 3200 	strd	r3, r2, [sp]
 f800be8:	f104 0354 	add.w	r3, r4, #84	; 0x54
 f800bec:	2249      	movs	r2, #73	; 0x49
 f800bee:	f001 fc93 	bl	f802518 <rtk_log_write>
 f800bf2:	f8d6 2120 	ldr.w	r2, [r6, #288]	; 0x120
 f800bf6:	4b7b      	ldr	r3, [pc, #492]	; (f800de4 <PSRAM_InfoDump+0x268>)
 f800bf8:	4629      	mov	r1, r5
 f800bfa:	2004      	movs	r0, #4
 f800bfc:	e9cd 3200 	strd	r3, r2, [sp]
 f800c00:	f104 0368 	add.w	r3, r4, #104	; 0x68
 f800c04:	2249      	movs	r2, #73	; 0x49
 f800c06:	f001 fc87 	bl	f802518 <rtk_log_write>
 f800c0a:	f8d6 2110 	ldr.w	r2, [r6, #272]	; 0x110
 f800c0e:	4b76      	ldr	r3, [pc, #472]	; (f800de8 <PSRAM_InfoDump+0x26c>)
 f800c10:	4629      	mov	r1, r5
 f800c12:	2004      	movs	r0, #4
 f800c14:	e9cd 3200 	strd	r3, r2, [sp]
 f800c18:	f104 0380 	add.w	r3, r4, #128	; 0x80
 f800c1c:	2249      	movs	r2, #73	; 0x49
 f800c1e:	f001 fc7b 	bl	f802518 <rtk_log_write>
 f800c22:	f8d6 2134 	ldr.w	r2, [r6, #308]	; 0x134
 f800c26:	4b71      	ldr	r3, [pc, #452]	; (f800dec <PSRAM_InfoDump+0x270>)
 f800c28:	4629      	mov	r1, r5
 f800c2a:	2004      	movs	r0, #4
 f800c2c:	e9cd 3200 	strd	r3, r2, [sp]
 f800c30:	f104 0394 	add.w	r3, r4, #148	; 0x94
 f800c34:	2249      	movs	r2, #73	; 0x49
 f800c36:	f001 fc6f 	bl	f802518 <rtk_log_write>
 f800c3a:	f8d6 2138 	ldr.w	r2, [r6, #312]	; 0x138
 f800c3e:	4b6c      	ldr	r3, [pc, #432]	; (f800df0 <PSRAM_InfoDump+0x274>)
 f800c40:	4629      	mov	r1, r5
 f800c42:	2004      	movs	r0, #4
 f800c44:	e9cd 3200 	strd	r3, r2, [sp]
 f800c48:	f104 03ac 	add.w	r3, r4, #172	; 0xac
 f800c4c:	2249      	movs	r2, #73	; 0x49
 f800c4e:	f001 fc63 	bl	f802518 <rtk_log_write>
 f800c52:	f8d6 20e0 	ldr.w	r2, [r6, #224]	; 0xe0
 f800c56:	4b67      	ldr	r3, [pc, #412]	; (f800df4 <PSRAM_InfoDump+0x278>)
 f800c58:	4629      	mov	r1, r5
 f800c5a:	2004      	movs	r0, #4
 f800c5c:	e9cd 3200 	strd	r3, r2, [sp]
 f800c60:	f104 03bc 	add.w	r3, r4, #188	; 0xbc
 f800c64:	2249      	movs	r2, #73	; 0x49
 f800c66:	f001 fc57 	bl	f802518 <rtk_log_write>
 f800c6a:	f8d6 20f4 	ldr.w	r2, [r6, #244]	; 0xf4
 f800c6e:	4b62      	ldr	r3, [pc, #392]	; (f800df8 <PSRAM_InfoDump+0x27c>)
 f800c70:	4629      	mov	r1, r5
 f800c72:	2004      	movs	r0, #4
 f800c74:	e9cd 3200 	strd	r3, r2, [sp]
 f800c78:	f104 03d8 	add.w	r3, r4, #216	; 0xd8
 f800c7c:	2249      	movs	r2, #73	; 0x49
 f800c7e:	f001 fc4b 	bl	f802518 <rtk_log_write>
 f800c82:	f8d6 211c 	ldr.w	r2, [r6, #284]	; 0x11c
 f800c86:	4b5d      	ldr	r3, [pc, #372]	; (f800dfc <PSRAM_InfoDump+0x280>)
 f800c88:	4629      	mov	r1, r5
 f800c8a:	2004      	movs	r0, #4
 f800c8c:	e9cd 3200 	strd	r3, r2, [sp]
 f800c90:	f104 03f0 	add.w	r3, r4, #240	; 0xf0
 f800c94:	2249      	movs	r2, #73	; 0x49
 f800c96:	f001 fc3f 	bl	f802518 <rtk_log_write>
 f800c9a:	f8d6 2118 	ldr.w	r2, [r6, #280]	; 0x118
 f800c9e:	4b58      	ldr	r3, [pc, #352]	; (f800e00 <PSRAM_InfoDump+0x284>)
 f800ca0:	4629      	mov	r1, r5
 f800ca2:	2004      	movs	r0, #4
 f800ca4:	e9cd 3200 	strd	r3, r2, [sp]
 f800ca8:	f504 7384 	add.w	r3, r4, #264	; 0x108
 f800cac:	2249      	movs	r2, #73	; 0x49
 f800cae:	f001 fc33 	bl	f802518 <rtk_log_write>
 f800cb2:	f8d6 213c 	ldr.w	r2, [r6, #316]	; 0x13c
 f800cb6:	4b53      	ldr	r3, [pc, #332]	; (f800e04 <PSRAM_InfoDump+0x288>)
 f800cb8:	4629      	mov	r1, r5
 f800cba:	2004      	movs	r0, #4
 f800cbc:	e9cd 3200 	strd	r3, r2, [sp]
 f800cc0:	f504 7390 	add.w	r3, r4, #288	; 0x120
 f800cc4:	2249      	movs	r2, #73	; 0x49
 f800cc6:	f001 fc27 	bl	f802518 <rtk_log_write>
 f800cca:	6ab1      	ldr	r1, [r6, #40]	; 0x28
 f800ccc:	4a4e      	ldr	r2, [pc, #312]	; (f800e08 <PSRAM_InfoDump+0x28c>)
 f800cce:	f5a6 5680 	sub.w	r6, r6, #4096	; 0x1000
 f800cd2:	f504 739c 	add.w	r3, r4, #312	; 0x138
 f800cd6:	2004      	movs	r0, #4
 f800cd8:	e9cd 2100 	strd	r2, r1, [sp]
 f800cdc:	4629      	mov	r1, r5
 f800cde:	2249      	movs	r2, #73	; 0x49
 f800ce0:	f001 fc1a 	bl	f802518 <rtk_log_write>
 f800ce4:	6872      	ldr	r2, [r6, #4]
 f800ce6:	4b49      	ldr	r3, [pc, #292]	; (f800e0c <PSRAM_InfoDump+0x290>)
 f800ce8:	4629      	mov	r1, r5
 f800cea:	2004      	movs	r0, #4
 f800cec:	e9cd 3200 	strd	r3, r2, [sp]
 f800cf0:	f504 73a4 	add.w	r3, r4, #328	; 0x148
 f800cf4:	2249      	movs	r2, #73	; 0x49
 f800cf6:	f001 fc0f 	bl	f802518 <rtk_log_write>
 f800cfa:	69b2      	ldr	r2, [r6, #24]
 f800cfc:	4b44      	ldr	r3, [pc, #272]	; (f800e10 <PSRAM_InfoDump+0x294>)
 f800cfe:	4629      	mov	r1, r5
 f800d00:	2004      	movs	r0, #4
 f800d02:	e9cd 3200 	strd	r3, r2, [sp]
 f800d06:	f504 73b2 	add.w	r3, r4, #356	; 0x164
 f800d0a:	2249      	movs	r2, #73	; 0x49
 f800d0c:	f001 fc04 	bl	f802518 <rtk_log_write>
 f800d10:	f8d6 10f0 	ldr.w	r1, [r6, #240]	; 0xf0
 f800d14:	4a3f      	ldr	r2, [pc, #252]	; (f800e14 <PSRAM_InfoDump+0x298>)
 f800d16:	f504 73c0 	add.w	r3, r4, #384	; 0x180
 f800d1a:	2004      	movs	r0, #4
 f800d1c:	2601      	movs	r6, #1
 f800d1e:	e9cd 2100 	strd	r2, r1, [sp]
 f800d22:	4629      	mov	r1, r5
 f800d24:	2249      	movs	r2, #73	; 0x49
 f800d26:	f001 fbf7 	bl	f802518 <rtk_log_write>
 f800d2a:	ab03      	add	r3, sp, #12
 f800d2c:	4641      	mov	r1, r8
 f800d2e:	2202      	movs	r2, #2
 f800d30:	2001      	movs	r0, #1
 f800d32:	f8cd 8000 	str.w	r8, [sp]
 f800d36:	47c8      	blx	r9
 f800d38:	f89d 200d 	ldrb.w	r2, [sp, #13]
 f800d3c:	f504 73ce 	add.w	r3, r4, #412	; 0x19c
 f800d40:	4629      	mov	r1, r5
 f800d42:	9200      	str	r2, [sp, #0]
 f800d44:	2004      	movs	r0, #4
 f800d46:	2249      	movs	r2, #73	; 0x49
 f800d48:	f001 fbe6 	bl	f802518 <rtk_log_write>
 f800d4c:	f89d 200c 	ldrb.w	r2, [sp, #12]
 f800d50:	f504 73d6 	add.w	r3, r4, #428	; 0x1ac
 f800d54:	4629      	mov	r1, r5
 f800d56:	9200      	str	r2, [sp, #0]
 f800d58:	2004      	movs	r0, #4
 f800d5a:	2249      	movs	r2, #73	; 0x49
 f800d5c:	f001 fbdc 	bl	f802518 <rtk_log_write>
 f800d60:	ab04      	add	r3, sp, #16
 f800d62:	4641      	mov	r1, r8
 f800d64:	4630      	mov	r0, r6
 f800d66:	9600      	str	r6, [sp, #0]
 f800d68:	2202      	movs	r2, #2
 f800d6a:	47c8      	blx	r9
 f800d6c:	f89d 2011 	ldrb.w	r2, [sp, #17]
 f800d70:	f504 73de 	add.w	r3, r4, #444	; 0x1bc
 f800d74:	4629      	mov	r1, r5
 f800d76:	9200      	str	r2, [sp, #0]
 f800d78:	2004      	movs	r0, #4
 f800d7a:	2249      	movs	r2, #73	; 0x49
 f800d7c:	f001 fbcc 	bl	f802518 <rtk_log_write>
 f800d80:	f89d 2010 	ldrb.w	r2, [sp, #16]
 f800d84:	f504 73e6 	add.w	r3, r4, #460	; 0x1cc
 f800d88:	4629      	mov	r1, r5
 f800d8a:	9200      	str	r2, [sp, #0]
 f800d8c:	2004      	movs	r0, #4
 f800d8e:	2249      	movs	r2, #73	; 0x49
 f800d90:	f001 fbc2 	bl	f802518 <rtk_log_write>
 f800d94:	ab05      	add	r3, sp, #20
 f800d96:	4631      	mov	r1, r6
 f800d98:	4630      	mov	r0, r6
 f800d9a:	9600      	str	r6, [sp, #0]
 f800d9c:	2202      	movs	r2, #2
 f800d9e:	47c8      	blx	r9
 f800da0:	f89d 0015 	ldrb.w	r0, [sp, #21]
 f800da4:	f504 73ee 	add.w	r3, r4, #476	; 0x1dc
 f800da8:	4629      	mov	r1, r5
 f800daa:	9000      	str	r0, [sp, #0]
 f800dac:	2249      	movs	r2, #73	; 0x49
 f800dae:	2004      	movs	r0, #4
 f800db0:	f001 fbb2 	bl	f802518 <rtk_log_write>
 f800db4:	f89d 6014 	ldrb.w	r6, [sp, #20]
 f800db8:	f504 73f6 	add.w	r3, r4, #492	; 0x1ec
 f800dbc:	2249      	movs	r2, #73	; 0x49
 f800dbe:	4629      	mov	r1, r5
 f800dc0:	2004      	movs	r0, #4
 f800dc2:	9600      	str	r6, [sp, #0]
 f800dc4:	f001 fba8 	bl	f802518 <rtk_log_write>
 f800dc8:	b006      	add	sp, #24
 f800dca:	e8bd 8370 	ldmia.w	sp!, {r4, r5, r6, r8, r9, pc}
 f800dce:	bf00      	nop
 f800dd0:	0f804990 	.word	0x0f804990
 f800dd4:	40127000 	.word	0x40127000
 f800dd8:	0f8025a9 	.word	0x0f8025a9
 f800ddc:	40127008 	.word	0x40127008
 f800de0:	40127014 	.word	0x40127014
 f800de4:	40127120 	.word	0x40127120
 f800de8:	40127110 	.word	0x40127110
 f800dec:	40127134 	.word	0x40127134
 f800df0:	40127138 	.word	0x40127138
 f800df4:	401270e0 	.word	0x401270e0
 f800df8:	401270f4 	.word	0x401270f4
 f800dfc:	4012711c 	.word	0x4012711c
 f800e00:	40127118 	.word	0x40127118
 f800e04:	4012713c 	.word	0x4012713c
 f800e08:	40127028 	.word	0x40127028
 f800e0c:	40126004 	.word	0x40126004
 f800e10:	40126018 	.word	0x40126018
 f800e14:	401260f0 	.word	0x401260f0
 f800e18:	0f8042f9 	.word	0x0f8042f9

0f800e1c <PSRAM_APM_DEVIC_Init>:
 f800e1c:	4a14      	ldr	r2, [pc, #80]	; (f800e70 <PSRAM_APM_DEVIC_Init+0x54>)
 f800e1e:	2100      	movs	r1, #0
 f800e20:	b530      	push	{r4, r5, lr}
 f800e22:	4c14      	ldr	r4, [pc, #80]	; (f800e74 <PSRAM_APM_DEVIC_Init+0x58>)
 f800e24:	b083      	sub	sp, #12
 f800e26:	6895      	ldr	r5, [r2, #8]
 f800e28:	2202      	movs	r2, #2
 f800e2a:	466b      	mov	r3, sp
 f800e2c:	4608      	mov	r0, r1
 f800e2e:	eb04 0e05 	add.w	lr, r4, r5
 f800e32:	f1a5 0c03 	sub.w	ip, r5, #3
 f800e36:	4c10      	ldr	r4, [pc, #64]	; (f800e78 <PSRAM_APM_DEVIC_Init+0x5c>)
 f800e38:	fa0c fc02 	lsl.w	ip, ip, r2
 f800e3c:	f81e ec03 	ldrb.w	lr, [lr, #-3]
 f800e40:	f00c 0c1c 	and.w	ip, ip, #28
 f800e44:	ea4f 1e4e 	mov.w	lr, lr, lsl #5
 f800e48:	ea4c 0c02 	orr.w	ip, ip, r2
 f800e4c:	fa5f fe8e 	uxtb.w	lr, lr
 f800e50:	f88d c000 	strb.w	ip, [sp]
 f800e54:	f88d e004 	strb.w	lr, [sp, #4]
 f800e58:	f88d c001 	strb.w	ip, [sp, #1]
 f800e5c:	f88d e005 	strb.w	lr, [sp, #5]
 f800e60:	47a0      	blx	r4
 f800e62:	ab01      	add	r3, sp, #4
 f800e64:	2202      	movs	r2, #2
 f800e66:	2104      	movs	r1, #4
 f800e68:	2000      	movs	r0, #0
 f800e6a:	47a0      	blx	r4
 f800e6c:	b003      	add	sp, #12
 f800e6e:	bd30      	pop	{r4, r5, pc}
 f800e70:	3000a1e8 	.word	0x3000a1e8
 f800e74:	3000a1c4 	.word	0x3000a1c4
 f800e78:	0f804399 	.word	0x0f804399

0f800e7c <PSRAM_WB_DEVIC_Init>:
 f800e7c:	490a      	ldr	r1, [pc, #40]	; (f800ea8 <PSRAM_WB_DEVIC_Init+0x2c>)
 f800e7e:	f04f 0caf 	mov.w	ip, #175	; 0xaf
 f800e82:	2202      	movs	r2, #2
 f800e84:	2001      	movs	r0, #1
 f800e86:	b510      	push	{r4, lr}
 f800e88:	b082      	sub	sp, #8
 f800e8a:	698c      	ldr	r4, [r1, #24]
 f800e8c:	2100      	movs	r1, #0
 f800e8e:	f88d c004 	strb.w	ip, [sp, #4]
 f800e92:	ea4f 1c04 	mov.w	ip, r4, lsl #4
 f800e96:	ab01      	add	r3, sp, #4
 f800e98:	4c04      	ldr	r4, [pc, #16]	; (f800eac <PSRAM_WB_DEVIC_Init+0x30>)
 f800e9a:	f04c 0c04 	orr.w	ip, ip, #4
 f800e9e:	f88d c005 	strb.w	ip, [sp, #5]
 f800ea2:	47a0      	blx	r4
 f800ea4:	b002      	add	sp, #8
 f800ea6:	bd10      	pop	{r4, pc}
 f800ea8:	3000a1e8 	.word	0x3000a1e8
 f800eac:	0f804399 	.word	0x0f804399

0f800eb0 <PSRAM_calibration>:
 f800eb0:	4aa9      	ldr	r2, [pc, #676]	; (f801158 <PSRAM_calibration+0x2a8>)
 f800eb2:	2300      	movs	r3, #0
 f800eb4:	49a9      	ldr	r1, [pc, #676]	; (f80115c <PSRAM_calibration+0x2ac>)
 f800eb6:	e92d 4f70 	stmdb	sp!, {r4, r5, r6, r8, r9, sl, fp, lr}
 f800eba:	6852      	ldr	r2, [r2, #4]
 f800ebc:	b098      	sub	sp, #96	; 0x60
 f800ebe:	4ca8      	ldr	r4, [pc, #672]	; (f801160 <PSRAM_calibration+0x2b0>)
 f800ec0:	9211      	str	r2, [sp, #68]	; 0x44
 f800ec2:	400a      	ands	r2, r1
 f800ec4:	9008      	str	r0, [sp, #32]
 f800ec6:	9210      	str	r2, [sp, #64]	; 0x40
 f800ec8:	e9cd 3312 	strd	r3, r3, [sp, #72]	; 0x48
 f800ecc:	e9cd 3314 	strd	r3, r3, [sp, #80]	; 0x50
 f800ed0:	e9cd 3316 	strd	r3, r3, [sp, #88]	; 0x58
 f800ed4:	f8c4 3084 	str.w	r3, [r4, #132]	; 0x84
 f800ed8:	f3bf 8f4f 	dsb	sy
 f800edc:	6963      	ldr	r3, [r4, #20]
 f800ede:	f423 3380 	bic.w	r3, r3, #65536	; 0x10000
 f800ee2:	6163      	str	r3, [r4, #20]
 f800ee4:	f3bf 8f4f 	dsb	sy
 f800ee8:	f8d4 5080 	ldr.w	r5, [r4, #128]	; 0x80
 f800eec:	f643 76e0 	movw	r6, #16352	; 0x3fe0
 f800ef0:	f3c5 304e 	ubfx	r0, r5, #13, #15
 f800ef4:	f3c5 05c9 	ubfx	r5, r5, #3, #10
 f800ef8:	0140      	lsls	r0, r0, #5
 f800efa:	ea00 0106 	and.w	r1, r0, r6
 f800efe:	462b      	mov	r3, r5
 f800f00:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
 f800f04:	3b01      	subs	r3, #1
 f800f06:	f8c4 2274 	str.w	r2, [r4, #628]	; 0x274
 f800f0a:	1c5a      	adds	r2, r3, #1
 f800f0c:	d1f8      	bne.n	f800f00 <PSRAM_calibration+0x50>
 f800f0e:	3820      	subs	r0, #32
 f800f10:	f110 0f20 	cmn.w	r0, #32
 f800f14:	d1f1      	bne.n	f800efa <PSRAM_calibration+0x4a>
 f800f16:	f3bf 8f4f 	dsb	sy
 f800f1a:	f3bf 8f6f 	isb	sy
 f800f1e:	4a8e      	ldr	r2, [pc, #568]	; (f801158 <PSRAM_calibration+0x2a8>)
 f800f20:	461d      	mov	r5, r3
 f800f22:	4e90      	ldr	r6, [pc, #576]	; (f801164 <PSRAM_calibration+0x2b4>)
 f800f24:	f04f 0a00 	mov.w	sl, #0
 f800f28:	930d      	str	r3, [sp, #52]	; 0x34
 f800f2a:	6813      	ldr	r3, [r2, #0]
 f800f2c:	f506 1bd8 	add.w	fp, r6, #1769472	; 0x1b0000
 f800f30:	f506 1800 	add.w	r8, r6, #2097152	; 0x200000
 f800f34:	f5a6 29a0 	sub.w	r9, r6, #327680	; 0x50000
 f800f38:	f023 0301 	bic.w	r3, r3, #1
 f800f3c:	6013      	str	r3, [r2, #0]
 f800f3e:	950a      	str	r5, [sp, #40]	; 0x28
 f800f40:	f8cd a030 	str.w	sl, [sp, #48]	; 0x30
 f800f44:	9b08      	ldr	r3, [sp, #32]
 f800f46:	2b00      	cmp	r3, #0
 f800f48:	f040 812a 	bne.w	f8011a0 <PSRAM_calibration+0x2f0>
 f800f4c:	9b0a      	ldr	r3, [sp, #40]	; 0x28
 f800f4e:	9a0c      	ldr	r2, [sp, #48]	; 0x30
 f800f50:	f8cd a02c 	str.w	sl, [sp, #44]	; 0x2c
 f800f54:	f04f 0a00 	mov.w	sl, #0
 f800f58:	e9cd 530e 	strd	r5, r3, [sp, #56]	; 0x38
 f800f5c:	9b10      	ldr	r3, [sp, #64]	; 0x40
 f800f5e:	f04f 35ff 	mov.w	r5, #4294967295	; 0xffffffff
 f800f62:	4654      	mov	r4, sl
 f800f64:	ea43 7302 	orr.w	r3, r3, r2, lsl #28
 f800f68:	950a      	str	r5, [sp, #40]	; 0x28
 f800f6a:	9309      	str	r3, [sp, #36]	; 0x24
 f800f6c:	e013      	b.n	f800f96 <PSRAM_calibration+0xe6>
 f800f6e:	2b00      	cmp	r3, #0
 f800f70:	d156      	bne.n	f801020 <PSRAM_calibration+0x170>
 f800f72:	f1b5 3fff 	cmp.w	r5, #4294967295	; 0xffffffff
 f800f76:	bf08      	it	eq
 f800f78:	4625      	moveq	r5, r4
 f800f7a:	eb05 030a 	add.w	r3, r5, sl
 f800f7e:	f10a 0a01 	add.w	sl, sl, #1
 f800f82:	930a      	str	r3, [sp, #40]	; 0x28
 f800f84:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
 f800f86:	459a      	cmp	sl, r3
 f800f88:	dd01      	ble.n	f800f8e <PSRAM_calibration+0xde>
 f800f8a:	2c1f      	cmp	r4, #31
 f800f8c:	d079      	beq.n	f801082 <PSRAM_calibration+0x1d2>
 f800f8e:	3401      	adds	r4, #1
 f800f90:	2c20      	cmp	r4, #32
 f800f92:	f000 8113 	beq.w	f8011bc <PSRAM_calibration+0x30c>
 f800f96:	9b09      	ldr	r3, [sp, #36]	; 0x24
 f800f98:	a812      	add	r0, sp, #72	; 0x48
 f800f9a:	4a6f      	ldr	r2, [pc, #444]	; (f801158 <PSRAM_calibration+0x2a8>)
 f800f9c:	4323      	orrs	r3, r4
 f800f9e:	6053      	str	r3, [r2, #4]
 f800fa0:	4b71      	ldr	r3, [pc, #452]	; (f801168 <PSRAM_calibration+0x2b8>)
 f800fa2:	4a72      	ldr	r2, [pc, #456]	; (f80116c <PSRAM_calibration+0x2bc>)
 f800fa4:	f8c9 3000 	str.w	r3, [r9]
 f800fa8:	4b71      	ldr	r3, [pc, #452]	; (f801170 <PSRAM_calibration+0x2c0>)
 f800faa:	f502 21a0 	add.w	r1, r2, #327680	; 0x50000
 f800fae:	6033      	str	r3, [r6, #0]
 f800fb0:	4b70      	ldr	r3, [pc, #448]	; (f801174 <PSRAM_calibration+0x2c4>)
 f800fb2:	6013      	str	r3, [r2, #0]
 f800fb4:	4b70      	ldr	r3, [pc, #448]	; (f801178 <PSRAM_calibration+0x2c8>)
 f800fb6:	600b      	str	r3, [r1, #0]
 f800fb8:	f103 43a4 	add.w	r3, r3, #1375731712	; 0x52000000
 f800fbc:	f503 1359 	add.w	r3, r3, #3555328	; 0x364000
 f800fc0:	f203 7385 	addw	r3, r3, #1925	; 0x785
 f800fc4:	f8cb 3000 	str.w	r3, [fp]
 f800fc8:	4b6c      	ldr	r3, [pc, #432]	; (f80117c <PSRAM_calibration+0x2cc>)
 f800fca:	f8c8 3000 	str.w	r3, [r8]
 f800fce:	f8d9 3000 	ldr.w	r3, [r9]
 f800fd2:	9312      	str	r3, [sp, #72]	; 0x48
 f800fd4:	6833      	ldr	r3, [r6, #0]
 f800fd6:	9313      	str	r3, [sp, #76]	; 0x4c
 f800fd8:	6813      	ldr	r3, [r2, #0]
 f800fda:	2218      	movs	r2, #24
 f800fdc:	9314      	str	r3, [sp, #80]	; 0x50
 f800fde:	680b      	ldr	r3, [r1, #0]
 f800fe0:	4967      	ldr	r1, [pc, #412]	; (f801180 <PSRAM_calibration+0x2d0>)
 f800fe2:	9315      	str	r3, [sp, #84]	; 0x54
 f800fe4:	f8db 3000 	ldr.w	r3, [fp]
 f800fe8:	9316      	str	r3, [sp, #88]	; 0x58
 f800fea:	f8d8 3000 	ldr.w	r3, [r8]
 f800fee:	9317      	str	r3, [sp, #92]	; 0x5c
 f800ff0:	4b64      	ldr	r3, [pc, #400]	; (f801184 <PSRAM_calibration+0x2d4>)
 f800ff2:	4798      	blx	r3
 f800ff4:	9b08      	ldr	r3, [sp, #32]
 f800ff6:	2800      	cmp	r0, #0
 f800ff8:	d0b9      	beq.n	f800f6e <PSRAM_calibration+0xbe>
 f800ffa:	bb33      	cbnz	r3, f80104a <PSRAM_calibration+0x19a>
 f800ffc:	1c69      	adds	r1, r5, #1
 f800ffe:	d0c6      	beq.n	f800f8e <PSRAM_calibration+0xde>
 f801000:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
 f801002:	4553      	cmp	r3, sl
 f801004:	da36      	bge.n	f801074 <PSRAM_calibration+0x1c4>
 f801006:	9b0a      	ldr	r3, [sp, #40]	; 0x28
 f801008:	950e      	str	r5, [sp, #56]	; 0x38
 f80100a:	f04f 35ff 	mov.w	r5, #4294967295	; 0xffffffff
 f80100e:	930f      	str	r3, [sp, #60]	; 0x3c
 f801010:	9b0c      	ldr	r3, [sp, #48]	; 0x30
 f801012:	f8cd a02c 	str.w	sl, [sp, #44]	; 0x2c
 f801016:	f04f 0a00 	mov.w	sl, #0
 f80101a:	950a      	str	r5, [sp, #40]	; 0x28
 f80101c:	930d      	str	r3, [sp, #52]	; 0x34
 f80101e:	e7b6      	b.n	f800f8e <PSRAM_calibration+0xde>
 f801020:	9b17      	ldr	r3, [sp, #92]	; 0x5c
 f801022:	2249      	movs	r2, #73	; 0x49
 f801024:	2004      	movs	r0, #4
 f801026:	9400      	str	r4, [sp, #0]
 f801028:	9306      	str	r3, [sp, #24]
 f80102a:	9b16      	ldr	r3, [sp, #88]	; 0x58
 f80102c:	9305      	str	r3, [sp, #20]
 f80102e:	9b15      	ldr	r3, [sp, #84]	; 0x54
 f801030:	9304      	str	r3, [sp, #16]
 f801032:	9b14      	ldr	r3, [sp, #80]	; 0x50
 f801034:	9303      	str	r3, [sp, #12]
 f801036:	9b13      	ldr	r3, [sp, #76]	; 0x4c
 f801038:	9302      	str	r3, [sp, #8]
 f80103a:	9b12      	ldr	r3, [sp, #72]	; 0x48
 f80103c:	9301      	str	r3, [sp, #4]
 f80103e:	4b52      	ldr	r3, [pc, #328]	; (f801188 <PSRAM_calibration+0x2d8>)
 f801040:	f5a3 7104 	sub.w	r1, r3, #528	; 0x210
 f801044:	f001 fa68 	bl	f802518 <rtk_log_write>
 f801048:	e793      	b.n	f800f72 <PSRAM_calibration+0xc2>
 f80104a:	9b17      	ldr	r3, [sp, #92]	; 0x5c
 f80104c:	2249      	movs	r2, #73	; 0x49
 f80104e:	2004      	movs	r0, #4
 f801050:	9400      	str	r4, [sp, #0]
 f801052:	9306      	str	r3, [sp, #24]
 f801054:	9b16      	ldr	r3, [sp, #88]	; 0x58
 f801056:	9305      	str	r3, [sp, #20]
 f801058:	9b15      	ldr	r3, [sp, #84]	; 0x54
 f80105a:	9304      	str	r3, [sp, #16]
 f80105c:	9b14      	ldr	r3, [sp, #80]	; 0x50
 f80105e:	9303      	str	r3, [sp, #12]
 f801060:	9b13      	ldr	r3, [sp, #76]	; 0x4c
 f801062:	9302      	str	r3, [sp, #8]
 f801064:	9b12      	ldr	r3, [sp, #72]	; 0x48
 f801066:	9301      	str	r3, [sp, #4]
 f801068:	4b48      	ldr	r3, [pc, #288]	; (f80118c <PSRAM_calibration+0x2dc>)
 f80106a:	f5a3 710c 	sub.w	r1, r3, #560	; 0x230
 f80106e:	f001 fa53 	bl	f802518 <rtk_log_write>
 f801072:	e7c3      	b.n	f800ffc <PSRAM_calibration+0x14c>
 f801074:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
 f801078:	f04f 0a00 	mov.w	sl, #0
 f80107c:	461d      	mov	r5, r3
 f80107e:	930a      	str	r3, [sp, #40]	; 0x28
 f801080:	e785      	b.n	f800f8e <PSRAM_calibration+0xde>
 f801082:	9b0c      	ldr	r3, [sp, #48]	; 0x30
 f801084:	930d      	str	r3, [sp, #52]	; 0x34
 f801086:	9b0c      	ldr	r3, [sp, #48]	; 0x30
 f801088:	2b00      	cmp	r3, #0
 f80108a:	f000 8083 	beq.w	f801194 <PSRAM_calibration+0x2e4>
 f80108e:	005b      	lsls	r3, r3, #1
 f801090:	2b08      	cmp	r3, #8
 f801092:	930c      	str	r3, [sp, #48]	; 0x30
 f801094:	f67f af56 	bls.w	f800f44 <PSRAM_calibration+0x94>
 f801098:	9a0d      	ldr	r2, [sp, #52]	; 0x34
 f80109a:	2004      	movs	r0, #4
 f80109c:	4b3c      	ldr	r3, [pc, #240]	; (f801190 <PSRAM_calibration+0x2e0>)
 f80109e:	e9cd a202 	strd	sl, r2, [sp, #8]
 f8010a2:	9a0a      	ldr	r2, [sp, #40]	; 0x28
 f8010a4:	f5a3 7115 	sub.w	r1, r3, #596	; 0x254
 f8010a8:	e9cd 5200 	strd	r5, r2, [sp]
 f8010ac:	2249      	movs	r2, #73	; 0x49
 f8010ae:	f001 fa33 	bl	f802518 <rtk_log_write>
 f8010b2:	482b      	ldr	r0, [pc, #172]	; (f801160 <PSRAM_calibration+0x2b0>)
 f8010b4:	6943      	ldr	r3, [r0, #20]
 f8010b6:	f413 3380 	ands.w	r3, r3, #65536	; 0x10000
 f8010ba:	d124      	bne.n	f801106 <PSRAM_calibration+0x256>
 f8010bc:	f8c0 3084 	str.w	r3, [r0, #132]	; 0x84
 f8010c0:	f3bf 8f4f 	dsb	sy
 f8010c4:	f8d0 6080 	ldr.w	r6, [r0, #128]	; 0x80
 f8010c8:	f643 7ce0 	movw	ip, #16352	; 0x3fe0
 f8010cc:	f3c6 344e 	ubfx	r4, r6, #13, #15
 f8010d0:	f3c6 06c9 	ubfx	r6, r6, #3, #10
 f8010d4:	0164      	lsls	r4, r4, #5
 f8010d6:	ea04 010c 	and.w	r1, r4, ip
 f8010da:	4633      	mov	r3, r6
 f8010dc:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
 f8010e0:	3b01      	subs	r3, #1
 f8010e2:	f8c0 2260 	str.w	r2, [r0, #608]	; 0x260
 f8010e6:	1c5a      	adds	r2, r3, #1
 f8010e8:	d1f8      	bne.n	f8010dc <PSRAM_calibration+0x22c>
 f8010ea:	3c20      	subs	r4, #32
 f8010ec:	f114 0f20 	cmn.w	r4, #32
 f8010f0:	d1f1      	bne.n	f8010d6 <PSRAM_calibration+0x226>
 f8010f2:	f3bf 8f4f 	dsb	sy
 f8010f6:	6943      	ldr	r3, [r0, #20]
 f8010f8:	f443 3380 	orr.w	r3, r3, #65536	; 0x10000
 f8010fc:	6143      	str	r3, [r0, #20]
 f8010fe:	f3bf 8f4f 	dsb	sy
 f801102:	f3bf 8f6f 	isb	sy
 f801106:	f1ba 0f08 	cmp.w	sl, #8
 f80110a:	dd53      	ble.n	f8011b4 <PSRAM_calibration+0x304>
 f80110c:	9b0a      	ldr	r3, [sp, #40]	; 0x28
 f80110e:	980d      	ldr	r0, [sp, #52]	; 0x34
 f801110:	1b5b      	subs	r3, r3, r5
 f801112:	bf48      	it	mi
 f801114:	3301      	addmi	r3, #1
 f801116:	105b      	asrs	r3, r3, #1
 f801118:	1e9a      	subs	r2, r3, #2
 f80111a:	9b0a      	ldr	r3, [sp, #40]	; 0x28
 f80111c:	195d      	adds	r5, r3, r5
 f80111e:	9b11      	ldr	r3, [sp, #68]	; 0x44
 f801120:	ea4f 4402 	mov.w	r4, r2, lsl #16
 f801124:	bf48      	it	mi
 f801126:	3501      	addmi	r5, #1
 f801128:	f003 617f 	and.w	r1, r3, #267386880	; 0xff00000
 f80112c:	f404 2470 	and.w	r4, r4, #983040	; 0xf0000
 f801130:	0212      	lsls	r2, r2, #8
 f801132:	ea41 7100 	orr.w	r1, r1, r0, lsl #28
 f801136:	f3c5 0344 	ubfx	r3, r5, #1, #5
 f80113a:	f402 6270 	and.w	r2, r2, #3840	; 0xf00
 f80113e:	2001      	movs	r0, #1
 f801140:	430b      	orrs	r3, r1
 f801142:	4905      	ldr	r1, [pc, #20]	; (f801158 <PSRAM_calibration+0x2a8>)
 f801144:	4323      	orrs	r3, r4
 f801146:	4313      	orrs	r3, r2
 f801148:	604b      	str	r3, [r1, #4]
 f80114a:	680b      	ldr	r3, [r1, #0]
 f80114c:	4303      	orrs	r3, r0
 f80114e:	600b      	str	r3, [r1, #0]
 f801150:	b018      	add	sp, #96	; 0x60
 f801152:	e8bd 8f70 	ldmia.w	sp!, {r4, r5, r6, r8, r9, sl, fp, pc}
 f801156:	bf00      	nop
 f801158:	40126000 	.word	0x40126000
 f80115c:	0fffffe0 	.word	0x0fffffe0
 f801160:	e000ed00 	.word	0xe000ed00
 f801164:	60050000 	.word	0x60050000
 f801168:	11223344 	.word	0x11223344
 f80116c:	60100000 	.word	0x60100000
 f801170:	a55aa55a 	.word	0xa55aa55a
 f801174:	5aa55aa5 	.word	0x5aa55aa5
 f801178:	44332211 	.word	0x44332211
 f80117c:	69969669 	.word	0x69969669
 f801180:	0f804c30 	.word	0x0f804c30
 f801184:	00011bbd 	.word	0x00011bbd
 f801188:	0f804bb4 	.word	0x0f804bb4
 f80118c:	0f804bd4 	.word	0x0f804bd4
 f801190:	0f804bf8 	.word	0x0f804bf8
 f801194:	2301      	movs	r3, #1
 f801196:	930c      	str	r3, [sp, #48]	; 0x30
 f801198:	9b08      	ldr	r3, [sp, #32]
 f80119a:	2b00      	cmp	r3, #0
 f80119c:	f43f aed6 	beq.w	f800f4c <PSRAM_calibration+0x9c>
 f8011a0:	9b0c      	ldr	r3, [sp, #48]	; 0x30
 f8011a2:	2249      	movs	r2, #73	; 0x49
 f8011a4:	2004      	movs	r0, #4
 f8011a6:	9300      	str	r3, [sp, #0]
 f8011a8:	4b07      	ldr	r3, [pc, #28]	; (f8011c8 <PSRAM_calibration+0x318>)
 f8011aa:	f5a3 71fe 	sub.w	r1, r3, #508	; 0x1fc
 f8011ae:	f001 f9b3 	bl	f802518 <rtk_log_write>
 f8011b2:	e6cb      	b.n	f800f4c <PSRAM_calibration+0x9c>
 f8011b4:	2000      	movs	r0, #0
 f8011b6:	b018      	add	sp, #96	; 0x60
 f8011b8:	e8bd 8f70 	ldmia.w	sp!, {r4, r5, r6, r8, r9, sl, fp, pc}
 f8011bc:	9b0f      	ldr	r3, [sp, #60]	; 0x3c
 f8011be:	f8dd a02c 	ldr.w	sl, [sp, #44]	; 0x2c
 f8011c2:	9d0e      	ldr	r5, [sp, #56]	; 0x38
 f8011c4:	930a      	str	r3, [sp, #40]	; 0x28
 f8011c6:	e75e      	b.n	f801086 <PSRAM_calibration+0x1d6>
 f8011c8:	0f804ba0 	.word	0x0f804ba0

0f8011cc <BOOT_Reason>:
 f8011cc:	4b01      	ldr	r3, [pc, #4]	; (f8011d4 <BOOT_Reason+0x8>)
 f8011ce:	f8d3 0268 	ldr.w	r0, [r3, #616]	; 0x268
 f8011d2:	4770      	bx	lr
 f8011d4:	41008000 	.word	0x41008000

0f8011d8 <BOOT_NsStart>:
 f8011d8:	b538      	push	{r3, r4, r5, lr}
 f8011da:	4b1b      	ldr	r3, [pc, #108]	; (f801248 <BOOT_NsStart+0x70>)
 f8011dc:	4604      	mov	r4, r0
 f8011de:	f383 8808 	msr	MSP, r3
 f8011e2:	f240 72fc 	movw	r2, #2044	; 0x7fc
 f8011e6:	2100      	movs	r1, #0
 f8011e8:	4618      	mov	r0, r3
 f8011ea:	4d18      	ldr	r5, [pc, #96]	; (f80124c <BOOT_NsStart+0x74>)
 f8011ec:	47a8      	blx	r5
 f8011ee:	4b18      	ldr	r3, [pc, #96]	; (f801250 <BOOT_NsStart+0x78>)
 f8011f0:	f383 8808 	msr	MSP, r3
 f8011f4:	f44f 6200 	mov.w	r2, #2048	; 0x800
 f8011f8:	2100      	movs	r1, #0
 f8011fa:	4816      	ldr	r0, [pc, #88]	; (f801254 <BOOT_NsStart+0x7c>)
 f8011fc:	47a8      	blx	r5
 f8011fe:	4a16      	ldr	r2, [pc, #88]	; (f801258 <BOOT_NsStart+0x80>)
 f801200:	6953      	ldr	r3, [r2, #20]
 f801202:	03db      	lsls	r3, r3, #15
 f801204:	d50c      	bpl.n	f801220 <BOOT_NsStart+0x48>
 f801206:	f3bf 8f4f 	dsb	sy
 f80120a:	4b12      	ldr	r3, [pc, #72]	; (f801254 <BOOT_NsStart+0x7c>)
 f80120c:	4913      	ldr	r1, [pc, #76]	; (f80125c <BOOT_NsStart+0x84>)
 f80120e:	f8c2 3270 	str.w	r3, [r2, #624]	; 0x270
 f801212:	3320      	adds	r3, #32
 f801214:	428b      	cmp	r3, r1
 f801216:	d1fa      	bne.n	f80120e <BOOT_NsStart+0x36>
 f801218:	f3bf 8f4f 	dsb	sy
 f80121c:	f3bf 8f6f 	isb	sy
 f801220:	47a0      	blx	r4
 f801222:	f024 0401 	bic.w	r4, r4, #1
 f801226:	0864      	lsrs	r4, r4, #1
 f801228:	0064      	lsls	r4, r4, #1
 f80122a:	e92d 0ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
 f80122e:	ed2d 8b10 	vpush	{d8-d15}
 f801232:	ec9f 0a20 	vscclrm	{s0-s31, VPR}
 f801236:	e89f 9fef 	clrm	{r0, r1, r2, r3, r5, r6, r7, r8, r9, sl, fp, ip, APSR}
 f80123a:	47a4      	blxns	r4
 f80123c:	ecbd 8b10 	vpop	{d8-d15}
 f801240:	e8bd 0ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
 f801244:	e7fe      	b.n	f801244 <BOOT_NsStart+0x6c>
 f801246:	bf00      	nop
 f801248:	30009800 	.word	0x30009800
 f80124c:	00011de5 	.word	0x00011de5
 f801250:	30009ffc 	.word	0x30009ffc
 f801254:	30009000 	.word	0x30009000
 f801258:	e000ed00 	.word	0xe000ed00
 f80125c:	3000a000 	.word	0x3000a000

0f801260 <BOOT_RccConfig>:
 f801260:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 f801264:	4b34      	ldr	r3, [pc, #208]	; (f801338 <BOOT_RccConfig+0xd8>)
 f801266:	b090      	sub	sp, #64	; 0x40
 f801268:	2400      	movs	r4, #0
 f80126a:	4e34      	ldr	r6, [pc, #208]	; (f80133c <BOOT_RccConfig+0xdc>)
 f80126c:	f103 0710 	add.w	r7, r3, #16
 f801270:	466d      	mov	r5, sp
 f801272:	f10d 0e20 	add.w	lr, sp, #32
 f801276:	9404      	str	r4, [sp, #16]
 f801278:	940c      	str	r4, [sp, #48]	; 0x30
 f80127a:	f10d 0c30 	add.w	ip, sp, #48	; 0x30
 f80127e:	9407      	str	r4, [sp, #28]
 f801280:	940f      	str	r4, [sp, #60]	; 0x3c
 f801282:	f8df 80c4 	ldr.w	r8, [pc, #196]	; f801348 <BOOT_RccConfig+0xe8>
 f801286:	cb0f      	ldmia	r3, {r0, r1, r2, r3}
 f801288:	e885 000f 	stmia.w	r5, {r0, r1, r2, r3}
 f80128c:	e897 000f 	ldmia.w	r7, {r0, r1, r2, r3}
 f801290:	e9cd 4405 	strd	r4, r4, [sp, #20]
 f801294:	e9cd 440d 	strd	r4, r4, [sp, #52]	; 0x34
 f801298:	e88e 000f 	stmia.w	lr, {r0, r1, r2, r3}
 f80129c:	f8d6 3204 	ldr.w	r3, [r6, #516]	; 0x204
 f8012a0:	f8d8 4000 	ldr.w	r4, [r8]
 f8012a4:	f043 0340 	orr.w	r3, r3, #64	; 0x40
 f8012a8:	1c62      	adds	r2, r4, #1
 f8012aa:	f8c6 3204 	str.w	r3, [r6, #516]	; 0x204
 f8012ae:	f8d6 320c 	ldr.w	r3, [r6, #524]	; 0x20c
 f8012b2:	f043 0340 	orr.w	r3, r3, #64	; 0x40
 f8012b6:	f8c6 320c 	str.w	r3, [r6, #524]	; 0x20c
 f8012ba:	d01a      	beq.n	f8012f2 <BOOT_RccConfig+0x92>
 f8012bc:	4642      	mov	r2, r8
 f8012be:	6891      	ldr	r1, [r2, #8]
 f8012c0:	0fa3      	lsrs	r3, r4, #30
 f8012c2:	a810      	add	r0, sp, #64	; 0x40
 f8012c4:	2901      	cmp	r1, #1
 f8012c6:	eb00 0383 	add.w	r3, r0, r3, lsl #2
 f8012ca:	d10e      	bne.n	f8012ea <BOOT_RccConfig+0x8a>
 f8012cc:	f853 0c30 	ldr.w	r0, [r3, #-48]
 f8012d0:	6856      	ldr	r6, [r2, #4]
 f8012d2:	4320      	orrs	r0, r4
 f8012d4:	ac10      	add	r4, sp, #64	; 0x40
 f8012d6:	0fb1      	lsrs	r1, r6, #30
 f8012d8:	f843 0c30 	str.w	r0, [r3, #-48]
 f8012dc:	eb04 0181 	add.w	r1, r4, r1, lsl #2
 f8012e0:	f851 3c10 	ldr.w	r3, [r1, #-16]
 f8012e4:	4333      	orrs	r3, r6
 f8012e6:	f841 3c10 	str.w	r3, [r1, #-16]
 f8012ea:	f852 4f0c 	ldr.w	r4, [r2, #12]!
 f8012ee:	1c63      	adds	r3, r4, #1
 f8012f0:	d1e5      	bne.n	f8012be <BOOT_RccConfig+0x5e>
 f8012f2:	4667      	mov	r7, ip
 f8012f4:	ae04      	add	r6, sp, #16
 f8012f6:	4911      	ldr	r1, [pc, #68]	; (f80133c <BOOT_RccConfig+0xdc>)
 f8012f8:	f85e 4b04 	ldr.w	r4, [lr], #4
 f8012fc:	f857 2b04 	ldr.w	r2, [r7], #4
 f801300:	f854 8001 	ldr.w	r8, [r4, r1]
 f801304:	45f4      	cmp	ip, lr
 f801306:	f022 4240 	bic.w	r2, r2, #3221225472	; 0xc0000000
 f80130a:	f855 0b04 	ldr.w	r0, [r5], #4
 f80130e:	f856 3b04 	ldr.w	r3, [r6], #4
 f801312:	ea42 0208 	orr.w	r2, r2, r8
 f801316:	f023 4340 	bic.w	r3, r3, #3221225472	; 0xc0000000
 f80131a:	5062      	str	r2, [r4, r1]
 f80131c:	5842      	ldr	r2, [r0, r1]
 f80131e:	ea43 0302 	orr.w	r3, r3, r2
 f801322:	5043      	str	r3, [r0, r1]
 f801324:	d1e8      	bne.n	f8012f8 <BOOT_RccConfig+0x98>
 f801326:	200a      	movs	r0, #10
 f801328:	4b05      	ldr	r3, [pc, #20]	; (f801340 <BOOT_RccConfig+0xe0>)
 f80132a:	4798      	blx	r3
 f80132c:	4b05      	ldr	r3, [pc, #20]	; (f801344 <BOOT_RccConfig+0xe4>)
 f80132e:	b010      	add	sp, #64	; 0x40
 f801330:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
 f801334:	4718      	bx	r3
 f801336:	bf00      	nop
 f801338:	0f804d80 	.word	0x0f804d80
 f80133c:	41008000 	.word	0x41008000
 f801340:	000094a5 	.word	0x000094a5
 f801344:	0f800021 	.word	0x0f800021
 f801348:	0f804534 	.word	0x0f804534

0f80134c <BOOT_ComboSpic_PLL_Open>:
 f80134c:	4a04      	ldr	r2, [pc, #16]	; (f801360 <BOOT_ComboSpic_PLL_Open+0x14>)
 f80134e:	4b05      	ldr	r3, [pc, #20]	; (f801364 <BOOT_ComboSpic_PLL_Open+0x18>)
 f801350:	7812      	ldrb	r2, [r2, #0]
 f801352:	eb03 03c2 	add.w	r3, r3, r2, lsl #3
 f801356:	79d8      	ldrb	r0, [r3, #7]
 f801358:	3801      	subs	r0, #1
 f80135a:	f7ff b97b 	b.w	f800654 <Combo_SPIC_Output_Div_Clk>
 f80135e:	bf00      	nop
 f801360:	3000a1e1 	.word	0x3000a1e1
 f801364:	0f8045b8 	.word	0x0f8045b8

0f801368 <BOOT_PSRAM_Init>:
 f801368:	f44f 5100 	mov.w	r1, #8192	; 0x2000
 f80136c:	2200      	movs	r2, #0
 f80136e:	b570      	push	{r4, r5, r6, lr}
 f801370:	2401      	movs	r4, #1
 f801372:	4608      	mov	r0, r1
 f801374:	b08e      	sub	sp, #56	; 0x38
 f801376:	4e31      	ldr	r6, [pc, #196]	; (f80143c <BOOT_PSRAM_Init+0xd4>)
 f801378:	47b0      	blx	r6
 f80137a:	4620      	mov	r0, r4
 f80137c:	4b30      	ldr	r3, [pc, #192]	; (f801440 <BOOT_PSRAM_Init+0xd8>)
 f80137e:	4798      	blx	r3
 f801380:	f44f 5100 	mov.w	r1, #8192	; 0x2000
 f801384:	4622      	mov	r2, r4
 f801386:	4d2f      	ldr	r5, [pc, #188]	; (f801444 <BOOT_PSRAM_Init+0xdc>)
 f801388:	4608      	mov	r0, r1
 f80138a:	47b0      	blx	r6
 f80138c:	4620      	mov	r0, r4
 f80138e:	3401      	adds	r4, #1
 f801390:	2101      	movs	r1, #1
 f801392:	b2e4      	uxtb	r4, r4
 f801394:	47a8      	blx	r5
 f801396:	2c05      	cmp	r4, #5
 f801398:	d1f8      	bne.n	f80138c <BOOT_PSRAM_Init+0x24>
 f80139a:	2407      	movs	r4, #7
 f80139c:	4620      	mov	r0, r4
 f80139e:	3401      	adds	r4, #1
 f8013a0:	2101      	movs	r1, #1
 f8013a2:	b2e4      	uxtb	r4, r4
 f8013a4:	47a8      	blx	r5
 f8013a6:	2c0b      	cmp	r4, #11
 f8013a8:	d1f8      	bne.n	f80139c <BOOT_PSRAM_Init+0x34>
 f8013aa:	4e27      	ldr	r6, [pc, #156]	; (f801448 <BOOT_PSRAM_Init+0xe0>)
 f8013ac:	47b0      	blx	r6
 f8013ae:	2803      	cmp	r0, #3
 f8013b0:	d03b      	beq.n	f80142a <BOOT_PSRAM_Init+0xc2>
 f8013b2:	2400      	movs	r4, #0
 f8013b4:	4d25      	ldr	r5, [pc, #148]	; (f80144c <BOOT_PSRAM_Init+0xe4>)
 f8013b6:	b2e0      	uxtb	r0, r4
 f8013b8:	3401      	adds	r4, #1
 f8013ba:	2104      	movs	r1, #4
 f8013bc:	47a8      	blx	r5
 f8013be:	2c0c      	cmp	r4, #12
 f8013c0:	d1f9      	bne.n	f8013b6 <BOOT_PSRAM_Init+0x4e>
 f8013c2:	4b23      	ldr	r3, [pc, #140]	; (f801450 <BOOT_PSRAM_Init+0xe8>)
 f8013c4:	4798      	blx	r3
 f8013c6:	4b23      	ldr	r3, [pc, #140]	; (f801454 <BOOT_PSRAM_Init+0xec>)
 f8013c8:	a801      	add	r0, sp, #4
 f8013ca:	4798      	blx	r3
 f8013cc:	4b22      	ldr	r3, [pc, #136]	; (f801458 <BOOT_PSRAM_Init+0xf0>)
 f8013ce:	a801      	add	r0, sp, #4
 f8013d0:	4798      	blx	r3
 f8013d2:	4b22      	ldr	r3, [pc, #136]	; (f80145c <BOOT_PSRAM_Init+0xf4>)
 f8013d4:	f8b3 3106 	ldrh.w	r3, [r3, #262]	; 0x106
 f8013d8:	07db      	lsls	r3, r3, #31
 f8013da:	d51b      	bpl.n	f801414 <BOOT_PSRAM_Init+0xac>
 f8013dc:	4b20      	ldr	r3, [pc, #128]	; (f801460 <BOOT_PSRAM_Init+0xf8>)
 f8013de:	2249      	movs	r2, #73	; 0x49
 f8013e0:	4920      	ldr	r1, [pc, #128]	; (f801464 <BOOT_PSRAM_Init+0xfc>)
 f8013e2:	2004      	movs	r0, #4
 f8013e4:	f001 f898 	bl	f802518 <rtk_log_write>
 f8013e8:	4b1f      	ldr	r3, [pc, #124]	; (f801468 <BOOT_PSRAM_Init+0x100>)
 f8013ea:	4798      	blx	r3
 f8013ec:	4b1f      	ldr	r3, [pc, #124]	; (f80146c <BOOT_PSRAM_Init+0x104>)
 f8013ee:	4798      	blx	r3
 f8013f0:	47b0      	blx	r6
 f8013f2:	b120      	cbz	r0, f8013fe <BOOT_PSRAM_Init+0x96>
 f8013f4:	47b0      	blx	r6
 f8013f6:	2801      	cmp	r0, #1
 f8013f8:	d001      	beq.n	f8013fe <BOOT_PSRAM_Init+0x96>
 f8013fa:	b00e      	add	sp, #56	; 0x38
 f8013fc:	bd70      	pop	{r4, r5, r6, pc}
 f8013fe:	2000      	movs	r0, #0
 f801400:	4c1b      	ldr	r4, [pc, #108]	; (f801470 <BOOT_PSRAM_Init+0x108>)
 f801402:	47a0      	blx	r4
 f801404:	2800      	cmp	r0, #0
 f801406:	d1f8      	bne.n	f8013fa <BOOT_PSRAM_Init+0x92>
 f801408:	4b1a      	ldr	r3, [pc, #104]	; (f801474 <BOOT_PSRAM_Init+0x10c>)
 f80140a:	4798      	blx	r3
 f80140c:	2001      	movs	r0, #1
 f80140e:	47a0      	blx	r4
 f801410:	b00e      	add	sp, #56	; 0x38
 f801412:	bd70      	pop	{r4, r5, r6, pc}
 f801414:	2249      	movs	r2, #73	; 0x49
 f801416:	4913      	ldr	r1, [pc, #76]	; (f801464 <BOOT_PSRAM_Init+0xfc>)
 f801418:	2004      	movs	r0, #4
 f80141a:	4b17      	ldr	r3, [pc, #92]	; (f801478 <BOOT_PSRAM_Init+0x110>)
 f80141c:	f001 f87c 	bl	f802518 <rtk_log_write>
 f801420:	4b11      	ldr	r3, [pc, #68]	; (f801468 <BOOT_PSRAM_Init+0x100>)
 f801422:	4798      	blx	r3
 f801424:	4b15      	ldr	r3, [pc, #84]	; (f80147c <BOOT_PSRAM_Init+0x114>)
 f801426:	4798      	blx	r3
 f801428:	e7e2      	b.n	f8013f0 <BOOT_PSRAM_Init+0x88>
 f80142a:	2400      	movs	r4, #0
 f80142c:	b2e0      	uxtb	r0, r4
 f80142e:	3401      	adds	r4, #1
 f801430:	2100      	movs	r1, #0
 f801432:	47a8      	blx	r5
 f801434:	2c0c      	cmp	r4, #12
 f801436:	d1f9      	bne.n	f80142c <BOOT_PSRAM_Init+0xc4>
 f801438:	e7bb      	b.n	f8013b2 <BOOT_PSRAM_Init+0x4a>
 f80143a:	bf00      	nop
 f80143c:	0000bebd 	.word	0x0000bebd
 f801440:	000094a5 	.word	0x000094a5
 f801444:	0000bb59 	.word	0x0000bb59
 f801448:	0000c219 	.word	0x0000c219
 f80144c:	0000bd4d 	.word	0x0000bd4d
 f801450:	0f800839 	.word	0x0f800839
 f801454:	0f800999 	.word	0x0f800999
 f801458:	0f8009c1 	.word	0x0f8009c1
 f80145c:	41008e00 	.word	0x41008e00
 f801460:	0f804c48 	.word	0x0f804c48
 f801464:	0f804988 	.word	0x0f804988
 f801468:	0f800a35 	.word	0x0f800a35
 f80146c:	0f800e1d 	.word	0x0f800e1d
 f801470:	0f800eb1 	.word	0x0f800eb1
 f801474:	0f800b7d 	.word	0x0f800b7d
 f801478:	0f804c5c 	.word	0x0f804c5c
 f80147c:	0f800e7d 	.word	0x0f800e7d

0f801480 <BOOT_SCBConfig_HP>:
 f801480:	4b0e      	ldr	r3, [pc, #56]	; (f8014bc <BOOT_SCBConfig_HP+0x3c>)
 f801482:	4a0f      	ldr	r2, [pc, #60]	; (f8014c0 <BOOT_SCBConfig_HP+0x40>)
 f801484:	6a59      	ldr	r1, [r3, #36]	; 0x24
 f801486:	f441 2170 	orr.w	r1, r1, #983040	; 0xf0000
 f80148a:	6259      	str	r1, [r3, #36]	; 0x24
 f80148c:	6a51      	ldr	r1, [r2, #36]	; 0x24
 f80148e:	f441 21e0 	orr.w	r1, r1, #458752	; 0x70000
 f801492:	6251      	str	r1, [r2, #36]	; 0x24
 f801494:	f8d3 1088 	ldr.w	r1, [r3, #136]	; 0x88
 f801498:	f441 0170 	orr.w	r1, r1, #15728640	; 0xf00000
 f80149c:	f8c3 1088 	str.w	r1, [r3, #136]	; 0x88
 f8014a0:	f8d2 1088 	ldr.w	r1, [r2, #136]	; 0x88
 f8014a4:	f441 0170 	orr.w	r1, r1, #15728640	; 0xf00000
 f8014a8:	f8c2 1088 	str.w	r1, [r2, #136]	; 0x88
 f8014ac:	f8d3 208c 	ldr.w	r2, [r3, #140]	; 0x8c
 f8014b0:	f442 6240 	orr.w	r2, r2, #3072	; 0xc00
 f8014b4:	f8c3 208c 	str.w	r2, [r3, #140]	; 0x8c
 f8014b8:	4770      	bx	lr
 f8014ba:	bf00      	nop
 f8014bc:	e000ed00 	.word	0xe000ed00
 f8014c0:	e002ed00 	.word	0xe002ed00

0f8014c4 <BOOT_WakeFromPG>:
 f8014c4:	b510      	push	{r4, lr}
 f8014c6:	f001 fe4d 	bl	f803164 <BOOT_TRNG_ParaSet>
 f8014ca:	f001 fff9 	bl	f8034c0 <BOOT_RAM_TZCfg>
 f8014ce:	4b0a      	ldr	r3, [pc, #40]	; (f8014f8 <BOOT_WakeFromPG+0x34>)
 f8014d0:	4a0a      	ldr	r2, [pc, #40]	; (f8014fc <BOOT_WakeFromPG+0x38>)
 f8014d2:	2000      	movs	r0, #0
 f8014d4:	689c      	ldr	r4, [r3, #8]
 f8014d6:	685b      	ldr	r3, [r3, #4]
 f8014d8:	6063      	str	r3, [r4, #4]
 f8014da:	6094      	str	r4, [r2, #8]
 f8014dc:	f7fe fe68 	bl	f8001b0 <Fault_Hanlder_Redirect>
 f8014e0:	f7ff ffce 	bl	f801480 <BOOT_SCBConfig_HP>
 f8014e4:	4b06      	ldr	r3, [pc, #24]	; (f801500 <BOOT_WakeFromPG+0x3c>)
 f8014e6:	f383 8888 	msr	MSP_NS, r3
 f8014ea:	f5a3 6300 	sub.w	r3, r3, #2048	; 0x800
 f8014ee:	f383 8809 	msr	PSP, r3
 f8014f2:	6860      	ldr	r0, [r4, #4]
 f8014f4:	f7ff fe70 	bl	f8011d8 <BOOT_NsStart>
 f8014f8:	20004da0 	.word	0x20004da0
 f8014fc:	e002ed00 	.word	0xe002ed00
 f801500:	20004000 	.word	0x20004000

0f801504 <BOOT_ReasonSet>:
 f801504:	4807      	ldr	r0, [pc, #28]	; (f801524 <BOOT_ReasonSet+0x20>)
 f801506:	2249      	movs	r2, #73	; 0x49
 f801508:	4b07      	ldr	r3, [pc, #28]	; (f801528 <BOOT_ReasonSet+0x24>)
 f80150a:	4908      	ldr	r1, [pc, #32]	; (f80152c <BOOT_ReasonSet+0x28>)
 f80150c:	b510      	push	{r4, lr}
 f80150e:	6a04      	ldr	r4, [r0, #32]
 f801510:	b082      	sub	sp, #8
 f801512:	6204      	str	r4, [r0, #32]
 f801514:	f8c0 4268 	str.w	r4, [r0, #616]	; 0x268
 f801518:	2004      	movs	r0, #4
 f80151a:	9400      	str	r4, [sp, #0]
 f80151c:	f000 fffc 	bl	f802518 <rtk_log_write>
 f801520:	b002      	add	sp, #8
 f801522:	bd10      	pop	{r4, pc}
 f801524:	41008000 	.word	0x41008000
 f801528:	0f804c6c 	.word	0x0f804c6c
 f80152c:	0f804988 	.word	0x0f804988

0f801530 <BOOT_Enable_KM0>:
 f801530:	b538      	push	{r3, r4, r5, lr}
 f801532:	4c0e      	ldr	r4, [pc, #56]	; (f80156c <BOOT_Enable_KM0+0x3c>)
 f801534:	2200      	movs	r2, #0
 f801536:	2108      	movs	r1, #8
 f801538:	2004      	movs	r0, #4
 f80153a:	f8d4 3234 	ldr.w	r3, [r4, #564]	; 0x234
 f80153e:	4d0c      	ldr	r5, [pc, #48]	; (f801570 <BOOT_Enable_KM0+0x40>)
 f801540:	f443 4300 	orr.w	r3, r3, #32768	; 0x8000
 f801544:	f8c4 3234 	str.w	r3, [r4, #564]	; 0x234
 f801548:	47a8      	blx	r5
 f80154a:	2201      	movs	r2, #1
 f80154c:	2108      	movs	r1, #8
 f80154e:	2004      	movs	r0, #4
 f801550:	47a8      	blx	r5
 f801552:	f8d4 3234 	ldr.w	r3, [r4, #564]	; 0x234
 f801556:	f423 4300 	bic.w	r3, r3, #32768	; 0x8000
 f80155a:	f8c4 3234 	str.w	r3, [r4, #564]	; 0x234
 f80155e:	f8d4 326c 	ldr.w	r3, [r4, #620]	; 0x26c
 f801562:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
 f801566:	f8c4 326c 	str.w	r3, [r4, #620]	; 0x26c
 f80156a:	bd38      	pop	{r3, r4, r5, pc}
 f80156c:	41008000 	.word	0x41008000
 f801570:	0000bebd 	.word	0x0000bebd

0f801574 <BOOT_SOC_ClkChk>:
 f801574:	b5f0      	push	{r4, r5, r6, r7, lr}
 f801576:	4b46      	ldr	r3, [pc, #280]	; (f801690 <BOOT_SOC_ClkChk+0x11c>)
 f801578:	b083      	sub	sp, #12
 f80157a:	6806      	ldr	r6, [r0, #0]
 f80157c:	4604      	mov	r4, r0
 f80157e:	429e      	cmp	r6, r3
 f801580:	d868      	bhi.n	f801654 <BOOT_SOC_ClkChk+0xe0>
 f801582:	4b44      	ldr	r3, [pc, #272]	; (f801694 <BOOT_SOC_ClkChk+0x120>)
 f801584:	429e      	cmp	r6, r3
 f801586:	d946      	bls.n	f801616 <BOOT_SOC_ClkChk+0xa2>
 f801588:	7963      	ldrb	r3, [r4, #5]
 f80158a:	2b00      	cmp	r3, #0
 f80158c:	d04b      	beq.n	f801626 <BOOT_SOC_ClkChk+0xb2>
 f80158e:	79a3      	ldrb	r3, [r4, #6]
 f801590:	2b00      	cmp	r3, #0
 f801592:	d050      	beq.n	f801636 <BOOT_SOC_ClkChk+0xc2>
 f801594:	79e5      	ldrb	r5, [r4, #7]
 f801596:	2d00      	cmp	r5, #0
 f801598:	d055      	beq.n	f801646 <BOOT_SOC_ClkChk+0xd2>
 f80159a:	7967      	ldrb	r7, [r4, #5]
 f80159c:	79a1      	ldrb	r1, [r4, #6]
 f80159e:	7922      	ldrb	r2, [r4, #4]
 f8015a0:	fbb6 f5f5 	udiv	r5, r6, r5
 f8015a4:	fbb6 f7f7 	udiv	r7, r6, r7
 f8015a8:	fbb6 f6f1 	udiv	r6, r6, r1
 f8015ac:	b9fa      	cbnz	r2, f8015ee <BOOT_SOC_ClkChk+0x7a>
 f8015ae:	4b3a      	ldr	r3, [pc, #232]	; (f801698 <BOOT_SOC_ClkChk+0x124>)
 f8015b0:	429f      	cmp	r7, r3
 f8015b2:	d855      	bhi.n	f801660 <BOOT_SOC_ClkChk+0xec>
 f8015b4:	4b39      	ldr	r3, [pc, #228]	; (f80169c <BOOT_SOC_ClkChk+0x128>)
 f8015b6:	429e      	cmp	r6, r3
 f8015b8:	d85a      	bhi.n	f801670 <BOOT_SOC_ClkChk+0xfc>
 f8015ba:	4b39      	ldr	r3, [pc, #228]	; (f8016a0 <BOOT_SOC_ClkChk+0x12c>)
 f8015bc:	429d      	cmp	r5, r3
 f8015be:	d824      	bhi.n	f80160a <BOOT_SOC_ClkChk+0x96>
 f8015c0:	4b38      	ldr	r3, [pc, #224]	; (f8016a4 <BOOT_SOC_ClkChk+0x130>)
 f8015c2:	2249      	movs	r2, #73	; 0x49
 f8015c4:	4938      	ldr	r1, [pc, #224]	; (f8016a8 <BOOT_SOC_ClkChk+0x134>)
 f8015c6:	2004      	movs	r0, #4
 f8015c8:	9700      	str	r7, [sp, #0]
 f8015ca:	f000 ffa5 	bl	f802518 <rtk_log_write>
 f8015ce:	4b37      	ldr	r3, [pc, #220]	; (f8016ac <BOOT_SOC_ClkChk+0x138>)
 f8015d0:	2249      	movs	r2, #73	; 0x49
 f8015d2:	4935      	ldr	r1, [pc, #212]	; (f8016a8 <BOOT_SOC_ClkChk+0x134>)
 f8015d4:	2004      	movs	r0, #4
 f8015d6:	9600      	str	r6, [sp, #0]
 f8015d8:	f000 ff9e 	bl	f802518 <rtk_log_write>
 f8015dc:	4b34      	ldr	r3, [pc, #208]	; (f8016b0 <BOOT_SOC_ClkChk+0x13c>)
 f8015de:	2249      	movs	r2, #73	; 0x49
 f8015e0:	4931      	ldr	r1, [pc, #196]	; (f8016a8 <BOOT_SOC_ClkChk+0x134>)
 f8015e2:	2004      	movs	r0, #4
 f8015e4:	9500      	str	r5, [sp, #0]
 f8015e6:	f000 ff97 	bl	f802518 <rtk_log_write>
 f8015ea:	b003      	add	sp, #12
 f8015ec:	bdf0      	pop	{r4, r5, r6, r7, pc}
 f8015ee:	4b31      	ldr	r3, [pc, #196]	; (f8016b4 <BOOT_SOC_ClkChk+0x140>)
 f8015f0:	429f      	cmp	r7, r3
 f8015f2:	d843      	bhi.n	f80167c <BOOT_SOC_ClkChk+0x108>
 f8015f4:	4b30      	ldr	r3, [pc, #192]	; (f8016b8 <BOOT_SOC_ClkChk+0x144>)
 f8015f6:	429e      	cmp	r6, r3
 f8015f8:	d9df      	bls.n	f8015ba <BOOT_SOC_ClkChk+0x46>
 f8015fa:	f240 1131 	movw	r1, #305	; 0x131
 f8015fe:	482f      	ldr	r0, [pc, #188]	; (f8016bc <BOOT_SOC_ClkChk+0x148>)
 f801600:	f001 fda6 	bl	f803150 <io_assert_failed>
 f801604:	4b26      	ldr	r3, [pc, #152]	; (f8016a0 <BOOT_SOC_ClkChk+0x12c>)
 f801606:	429d      	cmp	r5, r3
 f801608:	d9da      	bls.n	f8015c0 <BOOT_SOC_ClkChk+0x4c>
 f80160a:	f44f 719a 	mov.w	r1, #308	; 0x134
 f80160e:	482b      	ldr	r0, [pc, #172]	; (f8016bc <BOOT_SOC_ClkChk+0x148>)
 f801610:	f001 fd9e 	bl	f803150 <io_assert_failed>
 f801614:	e7d4      	b.n	f8015c0 <BOOT_SOC_ClkChk+0x4c>
 f801616:	f240 1123 	movw	r1, #291	; 0x123
 f80161a:	4828      	ldr	r0, [pc, #160]	; (f8016bc <BOOT_SOC_ClkChk+0x148>)
 f80161c:	f001 fd98 	bl	f803150 <io_assert_failed>
 f801620:	7963      	ldrb	r3, [r4, #5]
 f801622:	2b00      	cmp	r3, #0
 f801624:	d1b3      	bne.n	f80158e <BOOT_SOC_ClkChk+0x1a>
 f801626:	f44f 7192 	mov.w	r1, #292	; 0x124
 f80162a:	4824      	ldr	r0, [pc, #144]	; (f8016bc <BOOT_SOC_ClkChk+0x148>)
 f80162c:	f001 fd90 	bl	f803150 <io_assert_failed>
 f801630:	79a3      	ldrb	r3, [r4, #6]
 f801632:	2b00      	cmp	r3, #0
 f801634:	d1ae      	bne.n	f801594 <BOOT_SOC_ClkChk+0x20>
 f801636:	f240 1125 	movw	r1, #293	; 0x125
 f80163a:	4820      	ldr	r0, [pc, #128]	; (f8016bc <BOOT_SOC_ClkChk+0x148>)
 f80163c:	f001 fd88 	bl	f803150 <io_assert_failed>
 f801640:	79e5      	ldrb	r5, [r4, #7]
 f801642:	2d00      	cmp	r5, #0
 f801644:	d1a9      	bne.n	f80159a <BOOT_SOC_ClkChk+0x26>
 f801646:	f44f 7193 	mov.w	r1, #294	; 0x126
 f80164a:	481c      	ldr	r0, [pc, #112]	; (f8016bc <BOOT_SOC_ClkChk+0x148>)
 f80164c:	f001 fd80 	bl	f803150 <io_assert_failed>
 f801650:	79e5      	ldrb	r5, [r4, #7]
 f801652:	e7a2      	b.n	f80159a <BOOT_SOC_ClkChk+0x26>
 f801654:	f44f 7191 	mov.w	r1, #290	; 0x122
 f801658:	4818      	ldr	r0, [pc, #96]	; (f8016bc <BOOT_SOC_ClkChk+0x148>)
 f80165a:	f001 fd79 	bl	f803150 <io_assert_failed>
 f80165e:	e793      	b.n	f801588 <BOOT_SOC_ClkChk+0x14>
 f801660:	f240 112d 	movw	r1, #301	; 0x12d
 f801664:	4815      	ldr	r0, [pc, #84]	; (f8016bc <BOOT_SOC_ClkChk+0x148>)
 f801666:	f001 fd73 	bl	f803150 <io_assert_failed>
 f80166a:	4b0c      	ldr	r3, [pc, #48]	; (f80169c <BOOT_SOC_ClkChk+0x128>)
 f80166c:	429e      	cmp	r6, r3
 f80166e:	d9a4      	bls.n	f8015ba <BOOT_SOC_ClkChk+0x46>
 f801670:	f44f 7197 	mov.w	r1, #302	; 0x12e
 f801674:	4811      	ldr	r0, [pc, #68]	; (f8016bc <BOOT_SOC_ClkChk+0x148>)
 f801676:	f001 fd6b 	bl	f803150 <io_assert_failed>
 f80167a:	e79e      	b.n	f8015ba <BOOT_SOC_ClkChk+0x46>
 f80167c:	f44f 7198 	mov.w	r1, #304	; 0x130
 f801680:	480e      	ldr	r0, [pc, #56]	; (f8016bc <BOOT_SOC_ClkChk+0x148>)
 f801682:	f001 fd65 	bl	f803150 <io_assert_failed>
 f801686:	4b0c      	ldr	r3, [pc, #48]	; (f8016b8 <BOOT_SOC_ClkChk+0x144>)
 f801688:	429e      	cmp	r6, r3
 f80168a:	d996      	bls.n	f8015ba <BOOT_SOC_ClkChk+0x46>
 f80168c:	e7b5      	b.n	f8015fa <BOOT_SOC_ClkChk+0x86>
 f80168e:	bf00      	nop
 f801690:	29040000 	.word	0x29040000
 f801694:	11e1a2ff 	.word	0x11e1a2ff
 f801698:	0f7f4900 	.word	0x0f7f4900
 f80169c:	0632ea00 	.word	0x0632ea00
 f8016a0:	17d78400 	.word	0x17d78400
 f8016a4:	0f804cbc 	.word	0x0f804cbc
 f8016a8:	0f804988 	.word	0x0f804988
 f8016ac:	0f804cd4 	.word	0x0f804cd4
 f8016b0:	0f804cec 	.word	0x0f804cec
 f8016b4:	14904840 	.word	0x14904840
 f8016b8:	06dac2c0 	.word	0x06dac2c0
 f8016bc:	0f804c84 	.word	0x0f804c84

0f8016c0 <BOOT_SOC_ClkSet>:
 f8016c0:	4a31      	ldr	r2, [pc, #196]	; (f801788 <BOOT_SOC_ClkSet+0xc8>)
 f8016c2:	4b32      	ldr	r3, [pc, #200]	; (f80178c <BOOT_SOC_ClkSet+0xcc>)
 f8016c4:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 f8016c8:	4c31      	ldr	r4, [pc, #196]	; (f801790 <BOOT_SOC_ClkSet+0xd0>)
 f8016ca:	f892 a000 	ldrb.w	sl, [r2]
 f8016ce:	f854 803a 	ldr.w	r8, [r4, sl, lsl #3]
 f8016d2:	4798      	blx	r3
 f8016d4:	2801      	cmp	r0, #1
 f8016d6:	eb04 03ca 	add.w	r3, r4, sl, lsl #3
 f8016da:	d04e      	beq.n	f80177a <BOOT_SOC_ClkSet+0xba>
 f8016dc:	eb04 0aca 	add.w	sl, r4, sl, lsl #3
 f8016e0:	4618      	mov	r0, r3
 f8016e2:	f7ff ff47 	bl	f801574 <BOOT_SOC_ClkChk>
 f8016e6:	4b2b      	ldr	r3, [pc, #172]	; (f801794 <BOOT_SOC_ClkSet+0xd4>)
 f8016e8:	f89a 9005 	ldrb.w	r9, [sl, #5]
 f8016ec:	4d2a      	ldr	r5, [pc, #168]	; (f801798 <BOOT_SOC_ClkSet+0xd8>)
 f8016ee:	f89a 0004 	ldrb.w	r0, [sl, #4]
 f8016f2:	4e2a      	ldr	r6, [pc, #168]	; (f80179c <BOOT_SOC_ClkSet+0xdc>)
 f8016f4:	4c2a      	ldr	r4, [pc, #168]	; (f8017a0 <BOOT_SOC_ClkSet+0xe0>)
 f8016f6:	fbb8 f7f9 	udiv	r7, r8, r9
 f8016fa:	429f      	cmp	r7, r3
 f8016fc:	bf8c      	ite	hi
 f8016fe:	f44f 3780 	movhi.w	r7, #65536	; 0x10000
 f801702:	2700      	movls	r7, #0
 f801704:	4b27      	ldr	r3, [pc, #156]	; (f8017a4 <BOOT_SOC_ClkSet+0xe4>)
 f801706:	4798      	blx	r3
 f801708:	f8d5 3218 	ldr.w	r3, [r5, #536]	; 0x218
 f80170c:	f109 39ff 	add.w	r9, r9, #4294967295	; 0xffffffff
 f801710:	f023 030f 	bic.w	r3, r3, #15
 f801714:	f009 0907 	and.w	r9, r9, #7
 f801718:	f8c5 3218 	str.w	r3, [r5, #536]	; 0x218
 f80171c:	47b0      	blx	r6
 f80171e:	47a0      	blx	r4
 f801720:	4640      	mov	r0, r8
 f801722:	4b21      	ldr	r3, [pc, #132]	; (f8017a8 <BOOT_SOC_ClkSet+0xe8>)
 f801724:	4798      	blx	r3
 f801726:	4b21      	ldr	r3, [pc, #132]	; (f8017ac <BOOT_SOC_ClkSet+0xec>)
 f801728:	4921      	ldr	r1, [pc, #132]	; (f8017b0 <BOOT_SOC_ClkSet+0xf0>)
 f80172a:	4443      	add	r3, r8
 f80172c:	f89a 2006 	ldrb.w	r2, [sl, #6]
 f801730:	f8d5 0220 	ldr.w	r0, [r5, #544]	; 0x220
 f801734:	fba1 1303 	umull	r1, r3, r1, r3
 f801738:	3a01      	subs	r2, #1
 f80173a:	491e      	ldr	r1, [pc, #120]	; (f8017b4 <BOOT_SOC_ClkSet+0xf4>)
 f80173c:	0e5b      	lsrs	r3, r3, #25
 f80173e:	4001      	ands	r1, r0
 f801740:	0112      	lsls	r2, r2, #4
 f801742:	3b01      	subs	r3, #1
 f801744:	ea49 0901 	orr.w	r9, r9, r1
 f801748:	b2d2      	uxtb	r2, r2
 f80174a:	0219      	lsls	r1, r3, #8
 f80174c:	ea42 0209 	orr.w	r2, r2, r9
 f801750:	071b      	lsls	r3, r3, #28
 f801752:	f401 61e0 	and.w	r1, r1, #1792	; 0x700
 f801756:	f003 43e0 	and.w	r3, r3, #1879048192	; 0x70000000
 f80175a:	430a      	orrs	r2, r1
 f80175c:	4313      	orrs	r3, r2
 f80175e:	433b      	orrs	r3, r7
 f801760:	f8c5 3220 	str.w	r3, [r5, #544]	; 0x220
 f801764:	f8d5 3218 	ldr.w	r3, [r5, #536]	; 0x218
 f801768:	f043 030f 	orr.w	r3, r3, #15
 f80176c:	f8c5 3218 	str.w	r3, [r5, #536]	; 0x218
 f801770:	47b0      	blx	r6
 f801772:	4623      	mov	r3, r4
 f801774:	e8bd 47f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 f801778:	4718      	bx	r3
 f80177a:	4b08      	ldr	r3, [pc, #32]	; (f80179c <BOOT_SOC_ClkSet+0xdc>)
 f80177c:	4798      	blx	r3
 f80177e:	4b08      	ldr	r3, [pc, #32]	; (f8017a0 <BOOT_SOC_ClkSet+0xe0>)
 f801780:	e8bd 47f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 f801784:	4718      	bx	r3
 f801786:	bf00      	nop
 f801788:	3000a1e1 	.word	0x3000a1e1
 f80178c:	0000c219 	.word	0x0000c219
 f801790:	0f8045b8 	.word	0x0f8045b8
 f801794:	0bebc200 	.word	0x0bebc200
 f801798:	41008000 	.word	0x41008000
 f80179c:	000093b5 	.word	0x000093b5
 f8017a0:	00009531 	.word	0x00009531
 f8017a4:	0f8006a9 	.word	0x0f8006a9
 f8017a8:	0f80075d 	.word	0x0f80075d
 f8017ac:	05f5e0ff 	.word	0x05f5e0ff
 f8017b0:	55e63b89 	.word	0x55e63b89
 f8017b4:	8ffcf808 	.word	0x8ffcf808

0f8017b8 <BOOT_Log_Init>:
 f8017b8:	4b10      	ldr	r3, [pc, #64]	; (f8017fc <BOOT_Log_Init+0x44>)
 f8017ba:	b510      	push	{r4, lr}
 f8017bc:	781b      	ldrb	r3, [r3, #0]
 f8017be:	b93b      	cbnz	r3, f8017d0 <BOOT_Log_Init+0x18>
 f8017c0:	4c0f      	ldr	r4, [pc, #60]	; (f801800 <BOOT_Log_Init+0x48>)
 f8017c2:	4623      	mov	r3, r4
 f8017c4:	2201      	movs	r2, #1
 f8017c6:	2102      	movs	r1, #2
 f8017c8:	480e      	ldr	r0, [pc, #56]	; (f801804 <BOOT_Log_Init+0x4c>)
 f8017ca:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
 f8017ce:	4718      	bx	r3
 f8017d0:	4b0d      	ldr	r3, [pc, #52]	; (f801808 <BOOT_Log_Init+0x50>)
 f8017d2:	4798      	blx	r3
 f8017d4:	3802      	subs	r0, #2
 f8017d6:	2801      	cmp	r0, #1
 f8017d8:	d9f2      	bls.n	f8017c0 <BOOT_Log_Init+0x8>
 f8017da:	4b0c      	ldr	r3, [pc, #48]	; (f80180c <BOOT_Log_Init+0x54>)
 f8017dc:	4798      	blx	r3
 f8017de:	4c08      	ldr	r4, [pc, #32]	; (f801800 <BOOT_Log_Init+0x48>)
 f8017e0:	2200      	movs	r2, #0
 f8017e2:	2101      	movs	r1, #1
 f8017e4:	4807      	ldr	r0, [pc, #28]	; (f801804 <BOOT_Log_Init+0x4c>)
 f8017e6:	47a0      	blx	r4
 f8017e8:	2101      	movs	r1, #1
 f8017ea:	4806      	ldr	r0, [pc, #24]	; (f801804 <BOOT_Log_Init+0x4c>)
 f8017ec:	4b08      	ldr	r3, [pc, #32]	; (f801810 <BOOT_Log_Init+0x58>)
 f8017ee:	4798      	blx	r3
 f8017f0:	2201      	movs	r2, #1
 f8017f2:	4804      	ldr	r0, [pc, #16]	; (f801804 <BOOT_Log_Init+0x4c>)
 f8017f4:	4611      	mov	r1, r2
 f8017f6:	47a0      	blx	r4
 f8017f8:	e7e3      	b.n	f8017c2 <BOOT_Log_Init+0xa>
 f8017fa:	bf00      	nop
 f8017fc:	3000a1e0 	.word	0x3000a1e0
 f801800:	0000aa69 	.word	0x0000aa69
 f801804:	4100f000 	.word	0x4100f000
 f801808:	0000c219 	.word	0x0000c219
 f80180c:	0000ae71 	.word	0x0000ae71
 f801810:	0000aa55 	.word	0x0000aa55

0f801814 <BOOT_Image1>:
 f801814:	486a      	ldr	r0, [pc, #424]	; (f8019c0 <BOOT_Image1+0x1ac>)
 f801816:	2100      	movs	r1, #0
 f801818:	4a6a      	ldr	r2, [pc, #424]	; (f8019c4 <BOOT_Image1+0x1b0>)
 f80181a:	4b6b      	ldr	r3, [pc, #428]	; (f8019c8 <BOOT_Image1+0x1b4>)
 f80181c:	1a12      	subs	r2, r2, r0
 f80181e:	b530      	push	{r4, r5, lr}
 f801820:	b083      	sub	sp, #12
 f801822:	4798      	blx	r3
 f801824:	f7ff fe6e 	bl	f801504 <BOOT_ReasonSet>
 f801828:	f7ff fcd0 	bl	f8011cc <BOOT_Reason>
 f80182c:	2800      	cmp	r0, #0
 f80182e:	f000 80ba 	beq.w	f8019a6 <BOOT_Image1+0x192>
 f801832:	f7ff ff45 	bl	f8016c0 <BOOT_SOC_ClkSet>
 f801836:	2001      	movs	r0, #1
 f801838:	4b64      	ldr	r3, [pc, #400]	; (f8019cc <BOOT_Image1+0x1b8>)
 f80183a:	4798      	blx	r3
 f80183c:	f001 fc92 	bl	f803164 <BOOT_TRNG_ParaSet>
 f801840:	f7ff ffba 	bl	f8017b8 <BOOT_Log_Init>
 f801844:	f3ef 8408 	mrs	r4, MSP
 f801848:	4b61      	ldr	r3, [pc, #388]	; (f8019d0 <BOOT_Image1+0x1bc>)
 f80184a:	2249      	movs	r2, #73	; 0x49
 f80184c:	4961      	ldr	r1, [pc, #388]	; (f8019d4 <BOOT_Image1+0x1c0>)
 f80184e:	2004      	movs	r0, #4
 f801850:	9400      	str	r4, [sp, #0]
 f801852:	f000 fe61 	bl	f802518 <rtk_log_write>
 f801856:	4c60      	ldr	r4, [pc, #384]	; (f8019d8 <BOOT_Image1+0x1c4>)
 f801858:	4860      	ldr	r0, [pc, #384]	; (f8019dc <BOOT_Image1+0x1c8>)
 f80185a:	2249      	movs	r2, #73	; 0x49
 f80185c:	4b60      	ldr	r3, [pc, #384]	; (f8019e0 <BOOT_Image1+0x1cc>)
 f80185e:	495d      	ldr	r1, [pc, #372]	; (f8019d4 <BOOT_Image1+0x1c0>)
 f801860:	e9cd 0400 	strd	r0, r4, [sp]
 f801864:	2004      	movs	r0, #4
 f801866:	4c5f      	ldr	r4, [pc, #380]	; (f8019e4 <BOOT_Image1+0x1d0>)
 f801868:	f000 fe56 	bl	f802518 <rtk_log_write>
 f80186c:	4b5e      	ldr	r3, [pc, #376]	; (f8019e8 <BOOT_Image1+0x1d4>)
 f80186e:	e844 f400 	tt	r4, r4
 f801872:	f3c4 5480 	ubfx	r4, r4, #22, #1
 f801876:	2249      	movs	r2, #73	; 0x49
 f801878:	4956      	ldr	r1, [pc, #344]	; (f8019d4 <BOOT_Image1+0x1c0>)
 f80187a:	2004      	movs	r0, #4
 f80187c:	9400      	str	r4, [sp, #0]
 f80187e:	f000 fe4b 	bl	f802518 <rtk_log_write>
 f801882:	4c5a      	ldr	r4, [pc, #360]	; (f8019ec <BOOT_Image1+0x1d8>)
 f801884:	f7ff fcec 	bl	f801260 <BOOT_RccConfig>
 f801888:	f001 fc84 	bl	f803194 <BOOT_Disable_PadSlewRate>
 f80188c:	f7fe fd20 	bl	f8002d0 <flash_highspeed_setup>
 f801890:	4b57      	ldr	r3, [pc, #348]	; (f8019f0 <BOOT_Image1+0x1dc>)
 f801892:	695b      	ldr	r3, [r3, #20]
 f801894:	03db      	lsls	r3, r3, #15
 f801896:	d512      	bpl.n	f8018be <BOOT_Image1+0xaa>
 f801898:	f014 011f 	ands.w	r1, r4, #31
 f80189c:	d170      	bne.n	f801980 <BOOT_Image1+0x16c>
 f80189e:	4623      	mov	r3, r4
 f8018a0:	2158      	movs	r1, #88	; 0x58
 f8018a2:	f3bf 8f4f 	dsb	sy
 f8018a6:	4852      	ldr	r0, [pc, #328]	; (f8019f0 <BOOT_Image1+0x1dc>)
 f8018a8:	4419      	add	r1, r3
 f8018aa:	f8c0 3268 	str.w	r3, [r0, #616]	; 0x268
 f8018ae:	3320      	adds	r3, #32
 f8018b0:	1aca      	subs	r2, r1, r3
 f8018b2:	2a00      	cmp	r2, #0
 f8018b4:	dcf9      	bgt.n	f8018aa <BOOT_Image1+0x96>
 f8018b6:	f3bf 8f4f 	dsb	sy
 f8018ba:	f3bf 8f6f 	isb	sy
 f8018be:	4b4d      	ldr	r3, [pc, #308]	; (f8019f4 <BOOT_Image1+0x1e0>)
 f8018c0:	4d4d      	ldr	r5, [pc, #308]	; (f8019f8 <BOOT_Image1+0x1e4>)
 f8018c2:	f8c3 42f0 	str.w	r4, [r3, #752]	; 0x2f0
 f8018c6:	f7ff fd41 	bl	f80134c <BOOT_ComboSpic_PLL_Open>
 f8018ca:	47a8      	blx	r5
 f8018cc:	f410 3f40 	tst.w	r0, #196608	; 0x30000
 f8018d0:	d103      	bne.n	f8018da <BOOT_Image1+0xc6>
 f8018d2:	47a8      	blx	r5
 f8018d4:	f410 2f40 	tst.w	r0, #786432	; 0xc0000
 f8018d8:	d156      	bne.n	f801988 <BOOT_Image1+0x174>
 f8018da:	47a8      	blx	r5
 f8018dc:	f400 00de 	and.w	r0, r0, #7274496	; 0x6f0000
 f8018e0:	f440 1080 	orr.w	r0, r0, #1048576	; 0x100000
 f8018e4:	4b45      	ldr	r3, [pc, #276]	; (f8019fc <BOOT_Image1+0x1e8>)
 f8018e6:	f410 3f40 	tst.w	r0, #196608	; 0x30000
 f8018ea:	f8c3 0104 	str.w	r0, [r3, #260]	; 0x104
 f8018ee:	d14f      	bne.n	f801990 <BOOT_Image1+0x17c>
 f8018f0:	f410 1f70 	tst.w	r0, #3932160	; 0x3c0000
 f8018f4:	d00d      	beq.n	f801912 <BOOT_Image1+0xfe>
 f8018f6:	f410 2f40 	tst.w	r0, #786432	; 0xc0000
 f8018fa:	d002      	beq.n	f801902 <BOOT_Image1+0xee>
 f8018fc:	f410 1f40 	tst.w	r0, #3145728	; 0x300000
 f801900:	d15b      	bne.n	f8019ba <BOOT_Image1+0x1a6>
 f801902:	2101      	movs	r1, #1
 f801904:	2005      	movs	r0, #5
 f801906:	4b3e      	ldr	r3, [pc, #248]	; (f801a00 <BOOT_Image1+0x1ec>)
 f801908:	4798      	blx	r3
 f80190a:	2101      	movs	r1, #1
 f80190c:	2005      	movs	r0, #5
 f80190e:	4b3d      	ldr	r3, [pc, #244]	; (f801a04 <BOOT_Image1+0x1f0>)
 f801910:	4798      	blx	r3
 f801912:	f001 fc4b 	bl	f8031ac <BOOT_Share_Memory_Patch>
 f801916:	2400      	movs	r4, #0
 f801918:	f001 fc2e 	bl	f803178 <BOOT_ResetMask_Config>
 f80191c:	f001 fc74 	bl	f803208 <BOOT_VerCheck>
 f801920:	f001 fccc 	bl	f8032bc <BOOT_OTPCheck>
 f801924:	f000 fb1c 	bl	f801f60 <BOOT_OTA_IMG>
 f801928:	4b32      	ldr	r3, [pc, #200]	; (f8019f4 <BOOT_Image1+0x1e0>)
 f80192a:	f8c3 4270 	str.w	r4, [r3, #624]	; 0x270
 f80192e:	f001 fdc7 	bl	f8034c0 <BOOT_RAM_TZCfg>
 f801932:	f7ff fdfd 	bl	f801530 <BOOT_Enable_KM0>
 f801936:	2102      	movs	r1, #2
 f801938:	4622      	mov	r2, r4
 f80193a:	4833      	ldr	r0, [pc, #204]	; (f801a08 <BOOT_Image1+0x1f4>)
 f80193c:	4b33      	ldr	r3, [pc, #204]	; (f801a0c <BOOT_Image1+0x1f8>)
 f80193e:	4798      	blx	r3
 f801940:	201b      	movs	r0, #27
 f801942:	4b33      	ldr	r3, [pc, #204]	; (f801a10 <BOOT_Image1+0x1fc>)
 f801944:	4798      	blx	r3
 f801946:	4b33      	ldr	r3, [pc, #204]	; (f801a14 <BOOT_Image1+0x200>)
 f801948:	4a33      	ldr	r2, [pc, #204]	; (f801a18 <BOOT_Image1+0x204>)
 f80194a:	4620      	mov	r0, r4
 f80194c:	689c      	ldr	r4, [r3, #8]
 f80194e:	681b      	ldr	r3, [r3, #0]
 f801950:	6063      	str	r3, [r4, #4]
 f801952:	6094      	str	r4, [r2, #8]
 f801954:	f7fe fc2c 	bl	f8001b0 <Fault_Hanlder_Redirect>
 f801958:	f7ff fd92 	bl	f801480 <BOOT_SCBConfig_HP>
 f80195c:	4b2f      	ldr	r3, [pc, #188]	; (f801a1c <BOOT_Image1+0x208>)
 f80195e:	f383 8888 	msr	MSP_NS, r3
 f801962:	f5a3 6300 	sub.w	r3, r3, #2048	; 0x800
 f801966:	f383 8809 	msr	PSP, r3
 f80196a:	6861      	ldr	r1, [r4, #4]
 f80196c:	2004      	movs	r0, #4
 f80196e:	4b2c      	ldr	r3, [pc, #176]	; (f801a20 <BOOT_Image1+0x20c>)
 f801970:	2249      	movs	r2, #73	; 0x49
 f801972:	9100      	str	r1, [sp, #0]
 f801974:	4917      	ldr	r1, [pc, #92]	; (f8019d4 <BOOT_Image1+0x1c0>)
 f801976:	f000 fdcf 	bl	f802518 <rtk_log_write>
 f80197a:	6860      	ldr	r0, [r4, #4]
 f80197c:	f7ff fc2c 	bl	f8011d8 <BOOT_NsStart>
 f801980:	f024 031f 	bic.w	r3, r4, #31
 f801984:	3158      	adds	r1, #88	; 0x58
 f801986:	e78c      	b.n	f8018a2 <BOOT_Image1+0x8e>
 f801988:	47a8      	blx	r5
 f80198a:	f400 00fe 	and.w	r0, r0, #8323072	; 0x7f0000
 f80198e:	e7a9      	b.n	f8018e4 <BOOT_Image1+0xd0>
 f801990:	f7ff fcea 	bl	f801368 <BOOT_PSRAM_Init>
 f801994:	2102      	movs	r1, #2
 f801996:	2005      	movs	r0, #5
 f801998:	4b19      	ldr	r3, [pc, #100]	; (f801a00 <BOOT_Image1+0x1ec>)
 f80199a:	4798      	blx	r3
 f80199c:	2102      	movs	r1, #2
 f80199e:	2005      	movs	r0, #5
 f8019a0:	4b18      	ldr	r3, [pc, #96]	; (f801a04 <BOOT_Image1+0x1f0>)
 f8019a2:	4798      	blx	r3
 f8019a4:	e7b5      	b.n	f801912 <BOOT_Image1+0xfe>
 f8019a6:	4604      	mov	r4, r0
 f8019a8:	4b1e      	ldr	r3, [pc, #120]	; (f801a24 <BOOT_Image1+0x210>)
 f8019aa:	4798      	blx	r3
 f8019ac:	f44f 7202 	mov.w	r2, #520	; 0x208
 f8019b0:	4621      	mov	r1, r4
 f8019b2:	4812      	ldr	r0, [pc, #72]	; (f8019fc <BOOT_Image1+0x1e8>)
 f8019b4:	f002 fdb8 	bl	f804528 <____wrap_memset_veneer>
 f8019b8:	e73b      	b.n	f801832 <BOOT_Image1+0x1e>
 f8019ba:	f7fe fde1 	bl	f800580 <Combo_SPIC_Init>
 f8019be:	e7a0      	b.n	f801902 <BOOT_Image1+0xee>
 f8019c0:	3000a1e0 	.word	0x3000a1e0
 f8019c4:	3000a9e4 	.word	0x3000a9e4
 f8019c8:	00011de5 	.word	0x00011de5
 f8019cc:	0f8006f1 	.word	0x0f8006f1
 f8019d0:	0f804d08 	.word	0x0f804d08
 f8019d4:	0f804988 	.word	0x0f804988
 f8019d8:	0f804d34 	.word	0x0f804d34
 f8019dc:	0f804d40 	.word	0x0f804d40
 f8019e0:	0f804d20 	.word	0x0f804d20
 f8019e4:	0f801815 	.word	0x0f801815
 f8019e8:	0f804d4c 	.word	0x0f804d4c
 f8019ec:	20004014 	.word	0x20004014
 f8019f0:	e000ed00 	.word	0xe000ed00
 f8019f4:	41008000 	.word	0x41008000
 f8019f8:	0f802625 	.word	0x0f802625
 f8019fc:	41008e00 	.word	0x41008e00
 f801a00:	0000bb59 	.word	0x0000bb59
 f801a04:	0000bc21 	.word	0x0000bc21
 f801a08:	4100f000 	.word	0x4100f000
 f801a0c:	0000aa41 	.word	0x0000aa41
 f801a10:	0000d65d 	.word	0x0000d65d
 f801a14:	20004da0 	.word	0x20004da0
 f801a18:	e002ed00 	.word	0xe002ed00
 f801a1c:	20004000 	.word	0x20004000
 f801a20:	0f804d64 	.word	0x0f804d64
 f801a24:	0f800035 	.word	0x0f800035

0f801a28 <BOOT_OTA_SlotSelect>:
 f801a28:	e92d 4ff8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
 f801a2c:	f8df 8118 	ldr.w	r8, [pc, #280]	; f801b48 <BOOT_OTA_SlotSelect+0x120>
 f801a30:	4d3d      	ldr	r5, [pc, #244]	; (f801b28 <BOOT_OTA_SlotSelect+0x100>)
 f801a32:	4c3e      	ldr	r4, [pc, #248]	; (f801b2c <BOOT_OTA_SlotSelect+0x104>)
 f801a34:	4647      	mov	r7, r8
 f801a36:	f8df a114 	ldr.w	sl, [pc, #276]	; f801b4c <BOOT_OTA_SlotSelect+0x124>
 f801a3a:	f8df 9114 	ldr.w	r9, [pc, #276]	; f801b50 <BOOT_OTA_SlotSelect+0x128>
 f801a3e:	f8df b114 	ldr.w	fp, [pc, #276]	; f801b54 <BOOT_OTA_SlotSelect+0x12c>
 f801a42:	ed2d 8b02 	vpush	{d8}
 f801a46:	ed9f 8b36 	vldr	d8, [pc, #216]	; f801b20 <BOOT_OTA_SlotSelect+0xf8>
 f801a4a:	f855 1b04 	ldr.w	r1, [r5], #4
 f801a4e:	f44f 72b2 	mov.w	r2, #356	; 0x164
 f801a52:	4b37      	ldr	r3, [pc, #220]	; (f801b30 <BOOT_OTA_SlotSelect+0x108>)
 f801a54:	4620      	mov	r0, r4
 f801a56:	4798      	blx	r3
 f801a58:	4659      	mov	r1, fp
 f801a5a:	4620      	mov	r0, r4
 f801a5c:	2208      	movs	r2, #8
 f801a5e:	4b35      	ldr	r3, [pc, #212]	; (f801b34 <BOOT_OTA_SlotSelect+0x10c>)
 f801a60:	4798      	blx	r3
 f801a62:	4606      	mov	r6, r0
 f801a64:	2800      	cmp	r0, #0
 f801a66:	d037      	beq.n	f801ad8 <BOOT_OTA_SlotSelect+0xb0>
 f801a68:	ed87 8b00 	vstr	d8, [r7]
 f801a6c:	4b32      	ldr	r3, [pc, #200]	; (f801b38 <BOOT_OTA_SlotSelect+0x110>)
 f801a6e:	f504 74b2 	add.w	r4, r4, #356	; 0x164
 f801a72:	f10a 0a90 	add.w	sl, sl, #144	; 0x90
 f801a76:	f509 79c8 	add.w	r9, r9, #400	; 0x190
 f801a7a:	429d      	cmp	r5, r3
 f801a7c:	f107 0708 	add.w	r7, r7, #8
 f801a80:	d1e3      	bne.n	f801a4a <BOOT_OTA_SlotSelect+0x22>
 f801a82:	4b2e      	ldr	r3, [pc, #184]	; (f801b3c <BOOT_OTA_SlotSelect+0x114>)
 f801a84:	4798      	blx	r3
 f801a86:	b178      	cbz	r0, f801aa8 <BOOT_OTA_SlotSelect+0x80>
 f801a88:	4b2d      	ldr	r3, [pc, #180]	; (f801b40 <BOOT_OTA_SlotSelect+0x118>)
 f801a8a:	2100      	movs	r1, #0
 f801a8c:	2401      	movs	r4, #1
 f801a8e:	f8b3 006e 	ldrh.w	r0, [r3, #110]	; 0x6e
 f801a92:	460b      	mov	r3, r1
 f801a94:	b280      	uxth	r0, r0
 f801a96:	fa04 f203 	lsl.w	r2, r4, r3
 f801a9a:	3301      	adds	r3, #1
 f801a9c:	4202      	tst	r2, r0
 f801a9e:	bf08      	it	eq
 f801aa0:	3101      	addeq	r1, #1
 f801aa2:	2b10      	cmp	r3, #16
 f801aa4:	d1f7      	bne.n	f801a96 <BOOT_OTA_SlotSelect+0x6e>
 f801aa6:	0408      	lsls	r0, r1, #16
 f801aa8:	e9d8 2300 	ldrd	r2, r3, [r8]
 f801aac:	e9d8 4102 	ldrd	r4, r1, [r8, #8]
 f801ab0:	4282      	cmp	r2, r0
 f801ab2:	f173 0500 	sbcs.w	r5, r3, #0
 f801ab6:	db24      	blt.n	f801b02 <BOOT_OTA_SlotSelect+0xda>
 f801ab8:	4284      	cmp	r4, r0
 f801aba:	f171 0000 	sbcs.w	r0, r1, #0
 f801abe:	da28      	bge.n	f801b12 <BOOT_OTA_SlotSelect+0xea>
 f801ac0:	4820      	ldr	r0, [pc, #128]	; (f801b44 <BOOT_OTA_SlotSelect+0x11c>)
 f801ac2:	2501      	movs	r5, #1
 f801ac4:	7005      	strb	r5, [r0, #0]
 f801ac6:	42a2      	cmp	r2, r4
 f801ac8:	ecbd 8b02 	vpop	{d8}
 f801acc:	418b      	sbcs	r3, r1
 f801ace:	bfb4      	ite	lt
 f801ad0:	2001      	movlt	r0, #1
 f801ad2:	2000      	movge	r0, #0
 f801ad4:	e8bd 8ff8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, pc}
 f801ad8:	69a3      	ldr	r3, [r4, #24]
 f801ada:	2290      	movs	r2, #144	; 0x90
 f801adc:	f855 1c04 	ldr.w	r1, [r5, #-4]
 f801ae0:	4650      	mov	r0, sl
 f801ae2:	4419      	add	r1, r3
 f801ae4:	4b12      	ldr	r3, [pc, #72]	; (f801b30 <BOOT_OTA_SlotSelect+0x108>)
 f801ae6:	4798      	blx	r3
 f801ae8:	f44f 72c8 	mov.w	r2, #400	; 0x190
 f801aec:	4b10      	ldr	r3, [pc, #64]	; (f801b30 <BOOT_OTA_SlotSelect+0x108>)
 f801aee:	4648      	mov	r0, r9
 f801af0:	6869      	ldr	r1, [r5, #4]
 f801af2:	4798      	blx	r3
 f801af4:	8aa2      	ldrh	r2, [r4, #20]
 f801af6:	8ae3      	ldrh	r3, [r4, #22]
 f801af8:	607e      	str	r6, [r7, #4]
 f801afa:	ea43 4302 	orr.w	r3, r3, r2, lsl #16
 f801afe:	603b      	str	r3, [r7, #0]
 f801b00:	e7b4      	b.n	f801a6c <BOOT_OTA_SlotSelect+0x44>
 f801b02:	4284      	cmp	r4, r0
 f801b04:	f171 0000 	sbcs.w	r0, r1, #0
 f801b08:	dada      	bge.n	f801ac0 <BOOT_OTA_SlotSelect+0x98>
 f801b0a:	480e      	ldr	r0, [pc, #56]	; (f801b44 <BOOT_OTA_SlotSelect+0x11c>)
 f801b0c:	2500      	movs	r5, #0
 f801b0e:	7005      	strb	r5, [r0, #0]
 f801b10:	e7d9      	b.n	f801ac6 <BOOT_OTA_SlotSelect+0x9e>
 f801b12:	480c      	ldr	r0, [pc, #48]	; (f801b44 <BOOT_OTA_SlotSelect+0x11c>)
 f801b14:	2502      	movs	r5, #2
 f801b16:	7005      	strb	r5, [r0, #0]
 f801b18:	e7d5      	b.n	f801ac6 <BOOT_OTA_SlotSelect+0x9e>
 f801b1a:	bf00      	nop
 f801b1c:	f3af 8000 	nop.w
 f801b20:	ffffffff 	.word	0xffffffff
 f801b24:	ffffffff 	.word	0xffffffff
 f801b28:	3000a7f0 	.word	0x3000a7f0
 f801b2c:	3000a208 	.word	0x3000a208
 f801b30:	00011c1d 	.word	0x00011c1d
 f801b34:	00011bbd 	.word	0x00011bbd
 f801b38:	3000a7f8 	.word	0x3000a7f8
 f801b3c:	0000c34d 	.word	0x0000c34d
 f801b40:	41000300 	.word	0x41000300
 f801b44:	3000a920 	.word	0x3000a920
 f801b48:	3000a928 	.word	0x3000a928
 f801b4c:	3000a800 	.word	0x3000a800
 f801b50:	3000a4d0 	.word	0x3000a4d0
 f801b54:	0f804ef4 	.word	0x0f804ef4

0f801b58 <BOOT_LoadSubImage>:
 f801b58:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 f801b5c:	b08f      	sub	sp, #60	; 0x3c
 f801b5e:	469a      	mov	sl, r3
 f801b60:	f89d 3060 	ldrb.w	r3, [sp, #96]	; 0x60
 f801b64:	9305      	str	r3, [sp, #20]
 f801b66:	2a00      	cmp	r2, #0
 f801b68:	d03a      	beq.n	f801be0 <BOOT_LoadSubImage+0x88>
 f801b6a:	460f      	mov	r7, r1
 f801b6c:	4691      	mov	r9, r2
 f801b6e:	2600      	movs	r6, #0
 f801b70:	4605      	mov	r5, r0
 f801b72:	2220      	movs	r2, #32
 f801b74:	4b4e      	ldr	r3, [pc, #312]	; (f801cb0 <BOOT_LoadSubImage+0x158>)
 f801b76:	4639      	mov	r1, r7
 f801b78:	a806      	add	r0, sp, #24
 f801b7a:	4798      	blx	r3
 f801b7c:	9b06      	ldr	r3, [sp, #24]
 f801b7e:	4a4d      	ldr	r2, [pc, #308]	; (f801cb4 <BOOT_LoadSubImage+0x15c>)
 f801b80:	4293      	cmp	r3, r2
 f801b82:	d15a      	bne.n	f801c3a <BOOT_LoadSubImage+0xe2>
 f801b84:	f102 427b 	add.w	r2, r2, #4211081216	; 0xfb000000
 f801b88:	9b07      	ldr	r3, [sp, #28]
 f801b8a:	f502 0278 	add.w	r2, r2, #16252928	; 0xf80000
 f801b8e:	f502 62c0 	add.w	r2, r2, #1536	; 0x600
 f801b92:	4293      	cmp	r3, r2
 f801b94:	d151      	bne.n	f801c3a <BOOT_LoadSubImage+0xe2>
 f801b96:	9b09      	ldr	r3, [sp, #36]	; 0x24
 f801b98:	9c08      	ldr	r4, [sp, #32]
 f801b9a:	f1a3 0820 	sub.w	r8, r3, #32
 f801b9e:	f103 4378 	add.w	r3, r3, #4160749568	; 0xf8000000
 f801ba2:	3420      	adds	r4, #32
 f801ba4:	3b20      	subs	r3, #32
 f801ba6:	f1b3 6f00 	cmp.w	r3, #134217728	; 0x8000000
 f801baa:	d301      	bcc.n	f801bb0 <BOOT_LoadSubImage+0x58>
 f801bac:	2c20      	cmp	r4, #32
 f801bae:	d81b      	bhi.n	f801be8 <BOOT_LoadSubImage+0x90>
 f801bb0:	2c20      	cmp	r4, #32
 f801bb2:	bf08      	it	eq
 f801bb4:	46b8      	moveq	r8, r7
 f801bb6:	b17d      	cbz	r5, f801bd8 <BOOT_LoadSubImage+0x80>
 f801bb8:	1d2b      	adds	r3, r5, #4
 f801bba:	f845 8036 	str.w	r8, [r5, r6, lsl #3]
 f801bbe:	2249      	movs	r2, #73	; 0x49
 f801bc0:	493d      	ldr	r1, [pc, #244]	; (f801cb8 <BOOT_LoadSubImage+0x160>)
 f801bc2:	2004      	movs	r0, #4
 f801bc4:	e9cd 8401 	strd	r8, r4, [sp, #4]
 f801bc8:	f843 4036 	str.w	r4, [r3, r6, lsl #3]
 f801bcc:	f85a 3026 	ldr.w	r3, [sl, r6, lsl #2]
 f801bd0:	9300      	str	r3, [sp, #0]
 f801bd2:	4b3a      	ldr	r3, [pc, #232]	; (f801cbc <BOOT_LoadSubImage+0x164>)
 f801bd4:	f000 fca0 	bl	f802518 <rtk_log_write>
 f801bd8:	3601      	adds	r6, #1
 f801bda:	4427      	add	r7, r4
 f801bdc:	454e      	cmp	r6, r9
 f801bde:	d1c8      	bne.n	f801b72 <BOOT_LoadSubImage+0x1a>
 f801be0:	2001      	movs	r0, #1
 f801be2:	b00f      	add	sp, #60	; 0x3c
 f801be4:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 f801be8:	4639      	mov	r1, r7
 f801bea:	4b31      	ldr	r3, [pc, #196]	; (f801cb0 <BOOT_LoadSubImage+0x158>)
 f801bec:	4622      	mov	r2, r4
 f801bee:	4640      	mov	r0, r8
 f801bf0:	4798      	blx	r3
 f801bf2:	4933      	ldr	r1, [pc, #204]	; (f801cc0 <BOOT_LoadSubImage+0x168>)
 f801bf4:	694b      	ldr	r3, [r1, #20]
 f801bf6:	03db      	lsls	r3, r3, #15
 f801bf8:	d5dd      	bpl.n	f801bb6 <BOOT_LoadSubImage+0x5e>
 f801bfa:	ea08 0304 	and.w	r3, r8, r4
 f801bfe:	3301      	adds	r3, #1
 f801c00:	d026      	beq.n	f801c50 <BOOT_LoadSubImage+0xf8>
 f801c02:	f018 031f 	ands.w	r3, r8, #31
 f801c06:	d11f      	bne.n	f801c48 <BOOT_LoadSubImage+0xf0>
 f801c08:	4623      	mov	r3, r4
 f801c0a:	4642      	mov	r2, r8
 f801c0c:	f3bf 8f4f 	dsb	sy
 f801c10:	2b00      	cmp	r3, #0
 f801c12:	dd0d      	ble.n	f801c30 <BOOT_LoadSubImage+0xd8>
 f801c14:	1e59      	subs	r1, r3, #1
 f801c16:	f102 0320 	add.w	r3, r2, #32
 f801c1a:	4829      	ldr	r0, [pc, #164]	; (f801cc0 <BOOT_LoadSubImage+0x168>)
 f801c1c:	f021 011f 	bic.w	r1, r1, #31
 f801c20:	4419      	add	r1, r3
 f801c22:	428b      	cmp	r3, r1
 f801c24:	f8c0 2270 	str.w	r2, [r0, #624]	; 0x270
 f801c28:	461a      	mov	r2, r3
 f801c2a:	f103 0320 	add.w	r3, r3, #32
 f801c2e:	d1f8      	bne.n	f801c22 <BOOT_LoadSubImage+0xca>
 f801c30:	f3bf 8f4f 	dsb	sy
 f801c34:	f3bf 8f6f 	isb	sy
 f801c38:	e7bd      	b.n	f801bb6 <BOOT_LoadSubImage+0x5e>
 f801c3a:	9b05      	ldr	r3, [sp, #20]
 f801c3c:	2b01      	cmp	r3, #1
 f801c3e:	d029      	beq.n	f801c94 <BOOT_LoadSubImage+0x13c>
 f801c40:	2000      	movs	r0, #0
 f801c42:	b00f      	add	sp, #60	; 0x3c
 f801c44:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 f801c48:	4423      	add	r3, r4
 f801c4a:	f028 021f 	bic.w	r2, r8, #31
 f801c4e:	e7dd      	b.n	f801c0c <BOOT_LoadSubImage+0xb4>
 f801c50:	2300      	movs	r3, #0
 f801c52:	f8c1 3084 	str.w	r3, [r1, #132]	; 0x84
 f801c56:	f3bf 8f4f 	dsb	sy
 f801c5a:	f8d1 3080 	ldr.w	r3, [r1, #128]	; 0x80
 f801c5e:	f643 7be0 	movw	fp, #16352	; 0x3fe0
 f801c62:	f3c3 3c4e 	ubfx	ip, r3, #13, #15
 f801c66:	f3c3 0ec9 	ubfx	lr, r3, #3, #10
 f801c6a:	ea4f 1c4c 	mov.w	ip, ip, lsl #5
 f801c6e:	ea0c 000b 	and.w	r0, ip, fp
 f801c72:	4673      	mov	r3, lr
 f801c74:	ea40 7283 	orr.w	r2, r0, r3, lsl #30
 f801c78:	3b01      	subs	r3, #1
 f801c7a:	f8c1 2274 	str.w	r2, [r1, #628]	; 0x274
 f801c7e:	d2f9      	bcs.n	f801c74 <BOOT_LoadSubImage+0x11c>
 f801c80:	f1ac 0c20 	sub.w	ip, ip, #32
 f801c84:	f11c 0f20 	cmn.w	ip, #32
 f801c88:	d1f1      	bne.n	f801c6e <BOOT_LoadSubImage+0x116>
 f801c8a:	f3bf 8f4f 	dsb	sy
 f801c8e:	f3bf 8f6f 	isb	sy
 f801c92:	e790      	b.n	f801bb6 <BOOT_LoadSubImage+0x5e>
 f801c94:	f85a 4026 	ldr.w	r4, [sl, r6, lsl #2]
 f801c98:	2249      	movs	r2, #73	; 0x49
 f801c9a:	4b0a      	ldr	r3, [pc, #40]	; (f801cc4 <BOOT_LoadSubImage+0x16c>)
 f801c9c:	2004      	movs	r0, #4
 f801c9e:	4906      	ldr	r1, [pc, #24]	; (f801cb8 <BOOT_LoadSubImage+0x160>)
 f801ca0:	9400      	str	r4, [sp, #0]
 f801ca2:	f000 fc39 	bl	f802518 <rtk_log_write>
 f801ca6:	2000      	movs	r0, #0
 f801ca8:	b00f      	add	sp, #60	; 0x3c
 f801caa:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 f801cae:	bf00      	nop
 f801cb0:	00011c1d 	.word	0x00011c1d
 f801cb4:	35393138 	.word	0x35393138
 f801cb8:	0f804988 	.word	0x0f804988
 f801cbc:	0f804e0c 	.word	0x0f804e0c
 f801cc0:	e000ed00 	.word	0xe000ed00
 f801cc4:	0f804e00 	.word	0x0f804e00

0f801cc8 <BOOT_OTFCheck>:
 f801cc8:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 f801ccc:	4d1c      	ldr	r5, [pc, #112]	; (f801d40 <BOOT_OTFCheck+0x78>)
 f801cce:	b082      	sub	sp, #8
 f801cd0:	4604      	mov	r4, r0
 f801cd2:	4688      	mov	r8, r1
 f801cd4:	4617      	mov	r7, r2
 f801cd6:	461e      	mov	r6, r3
 f801cd8:	47a8      	blx	r5
 f801cda:	b148      	cbz	r0, f801cf0 <BOOT_OTFCheck+0x28>
 f801cdc:	f104 4378 	add.w	r3, r4, #4160749568	; 0xf8000000
 f801ce0:	f1b3 6f00 	cmp.w	r3, #134217728	; 0x8000000
 f801ce4:	d204      	bcs.n	f801cf0 <BOOT_OTFCheck+0x28>
 f801ce6:	f108 4378 	add.w	r3, r8, #4160749568	; 0xf8000000
 f801cea:	f1b3 6f00 	cmp.w	r3, #134217728	; 0x8000000
 f801cee:	d302      	bcc.n	f801cf6 <BOOT_OTFCheck+0x2e>
 f801cf0:	b002      	add	sp, #8
 f801cf2:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 f801cf6:	4b13      	ldr	r3, [pc, #76]	; (f801d44 <BOOT_OTFCheck+0x7c>)
 f801cf8:	2249      	movs	r2, #73	; 0x49
 f801cfa:	4913      	ldr	r1, [pc, #76]	; (f801d48 <BOOT_OTFCheck+0x80>)
 f801cfc:	2004      	movs	r0, #4
 f801cfe:	f000 fc0b 	bl	f802518 <rtk_log_write>
 f801d02:	4b12      	ldr	r3, [pc, #72]	; (f801d4c <BOOT_OTFCheck+0x84>)
 f801d04:	4798      	blx	r3
 f801d06:	4603      	mov	r3, r0
 f801d08:	b150      	cbz	r0, f801d20 <BOOT_OTFCheck+0x58>
 f801d0a:	2801      	cmp	r0, #1
 f801d0c:	d009      	beq.n	f801d22 <BOOT_OTFCheck+0x5a>
 f801d0e:	4b10      	ldr	r3, [pc, #64]	; (f801d50 <BOOT_OTFCheck+0x88>)
 f801d10:	2245      	movs	r2, #69	; 0x45
 f801d12:	490d      	ldr	r1, [pc, #52]	; (f801d48 <BOOT_OTFCheck+0x80>)
 f801d14:	2002      	movs	r0, #2
 f801d16:	b002      	add	sp, #8
 f801d18:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
 f801d1c:	f000 bbfc 	b.w	f802518 <rtk_log_write>
 f801d20:	2302      	movs	r3, #2
 f801d22:	4630      	mov	r0, r6
 f801d24:	4621      	mov	r1, r4
 f801d26:	9301      	str	r3, [sp, #4]
 f801d28:	4642      	mov	r2, r8
 f801d2a:	2301      	movs	r3, #1
 f801d2c:	4c09      	ldr	r4, [pc, #36]	; (f801d54 <BOOT_OTFCheck+0x8c>)
 f801d2e:	9700      	str	r7, [sp, #0]
 f801d30:	47a0      	blx	r4
 f801d32:	2001      	movs	r0, #1
 f801d34:	4b08      	ldr	r3, [pc, #32]	; (f801d58 <BOOT_OTFCheck+0x90>)
 f801d36:	b002      	add	sp, #8
 f801d38:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
 f801d3c:	4718      	bx	r3
 f801d3e:	bf00      	nop
 f801d40:	0000c249 	.word	0x0000c249
 f801d44:	0f804e1c 	.word	0x0f804e1c
 f801d48:	0f804988 	.word	0x0f804988
 f801d4c:	0000c375 	.word	0x0000c375
 f801d50:	0f804e2c 	.word	0x0f804e2c
 f801d54:	00003a7d 	.word	0x00003a7d
 f801d58:	00003a61 	.word	0x00003a61

0f801d5c <BOOT_OTA_LoadIMGAll>:
 f801d5c:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 f801d60:	4b6d      	ldr	r3, [pc, #436]	; (f801f18 <BOOT_OTA_LoadIMGAll+0x1bc>)
 f801d62:	b0a5      	sub	sp, #148	; 0x94
 f801d64:	4680      	mov	r8, r0
 f801d66:	f8df 91f0 	ldr.w	r9, [pc, #496]	; f801f58 <BOOT_OTA_LoadIMGAll+0x1fc>
 f801d6a:	f103 0610 	add.w	r6, r3, #16
 f801d6e:	ad06      	add	r5, sp, #24
 f801d70:	af0a      	add	r7, sp, #40	; 0x28
 f801d72:	cb0f      	ldmia	r3, {r0, r1, r2, r3}
 f801d74:	e885 000f 	stmia.w	r5, {r0, r1, r2, r3}
 f801d78:	2300      	movs	r3, #0
 f801d7a:	f108 0202 	add.w	r2, r8, #2
 f801d7e:	e9cd 330e 	strd	r3, r3, [sp, #56]	; 0x38
 f801d82:	4b66      	ldr	r3, [pc, #408]	; (f801f1c <BOOT_OTA_LoadIMGAll+0x1c0>)
 f801d84:	f853 4022 	ldr.w	r4, [r3, r2, lsl #2]
 f801d88:	f44f 73c8 	mov.w	r3, #400	; 0x190
 f801d8c:	fb03 f308 	mul.w	r3, r3, r8
 f801d90:	9303      	str	r3, [sp, #12]
 f801d92:	e896 000f 	ldmia.w	r6, {r0, r1, r2, r3}
 f801d96:	e887 000f 	stmia.w	r7, {r0, r1, r2, r3}
 f801d9a:	47c8      	blx	r9
 f801d9c:	f10d 0938 	add.w	r9, sp, #56	; 0x38
 f801da0:	b150      	cbz	r0, f801db8 <BOOT_OTA_LoadIMGAll+0x5c>
 f801da2:	9b03      	ldr	r3, [sp, #12]
 f801da4:	2208      	movs	r2, #8
 f801da6:	495e      	ldr	r1, [pc, #376]	; (f801f20 <BOOT_OTA_LoadIMGAll+0x1c4>)
 f801da8:	4648      	mov	r0, r9
 f801daa:	4419      	add	r1, r3
 f801dac:	4b5d      	ldr	r3, [pc, #372]	; (f801f24 <BOOT_OTA_LoadIMGAll+0x1c8>)
 f801dae:	4798      	blx	r3
 f801db0:	4649      	mov	r1, r9
 f801db2:	2001      	movs	r0, #1
 f801db4:	4b5c      	ldr	r3, [pc, #368]	; (f801f28 <BOOT_OTA_LoadIMGAll+0x1cc>)
 f801db6:	4798      	blx	r3
 f801db8:	4e5c      	ldr	r6, [pc, #368]	; (f801f2c <BOOT_OTA_LoadIMGAll+0x1d0>)
 f801dba:	f504 5480 	add.w	r4, r4, #4096	; 0x1000
 f801dbe:	4a5c      	ldr	r2, [pc, #368]	; (f801f30 <BOOT_OTA_LoadIMGAll+0x1d4>)
 f801dc0:	2001      	movs	r0, #1
 f801dc2:	4623      	mov	r3, r4
 f801dc4:	4631      	mov	r1, r6
 f801dc6:	f8df a194 	ldr.w	sl, [pc, #404]	; f801f5c <BOOT_OTA_LoadIMGAll+0x200>
 f801dca:	47d0      	blx	sl
 f801dcc:	2101      	movs	r1, #1
 f801dce:	4b59      	ldr	r3, [pc, #356]	; (f801f34 <BOOT_OTA_LoadIMGAll+0x1d8>)
 f801dd0:	4608      	mov	r0, r1
 f801dd2:	4798      	blx	r3
 f801dd4:	4b58      	ldr	r3, [pc, #352]	; (f801f38 <BOOT_OTA_LoadIMGAll+0x1dc>)
 f801dd6:	4798      	blx	r3
 f801dd8:	2301      	movs	r3, #1
 f801dda:	4955      	ldr	r1, [pc, #340]	; (f801f30 <BOOT_OTA_LoadIMGAll+0x1d4>)
 f801ddc:	4630      	mov	r0, r6
 f801dde:	469b      	mov	fp, r3
 f801de0:	461a      	mov	r2, r3
 f801de2:	f7ff ff71 	bl	f801cc8 <BOOT_OTFCheck>
 f801de6:	2204      	movs	r2, #4
 f801de8:	462b      	mov	r3, r5
 f801dea:	4631      	mov	r1, r6
 f801dec:	4648      	mov	r0, r9
 f801dee:	f8cd b000 	str.w	fp, [sp]
 f801df2:	f88d 2017 	strb.w	r2, [sp, #23]
 f801df6:	f7ff feaf 	bl	f801b58 <BOOT_LoadSubImage>
 f801dfa:	2800      	cmp	r0, #0
 f801dfc:	d06b      	beq.n	f801ed6 <BOOT_OTA_LoadIMGAll+0x17a>
 f801dfe:	f89d c017 	ldrb.w	ip, [sp, #23]
 f801e02:	f1bc 0f00 	cmp.w	ip, #0
 f801e06:	f000 8082 	beq.w	f801f0e <BOOT_OTA_LoadIMGAll+0x1b2>
 f801e0a:	f10c 30ff 	add.w	r0, ip, #4294967295	; 0xffffffff
 f801e0e:	ab10      	add	r3, sp, #64	; 0x40
 f801e10:	2200      	movs	r2, #0
 f801e12:	b2c0      	uxtb	r0, r0
 f801e14:	eb03 00c0 	add.w	r0, r3, r0, lsl #3
 f801e18:	464b      	mov	r3, r9
 f801e1a:	6859      	ldr	r1, [r3, #4]
 f801e1c:	3308      	adds	r3, #8
 f801e1e:	4298      	cmp	r0, r3
 f801e20:	440a      	add	r2, r1
 f801e22:	d1fa      	bne.n	f801e1a <BOOT_OTA_LoadIMGAll+0xbe>
 f801e24:	1e51      	subs	r1, r2, #1
 f801e26:	4b45      	ldr	r3, [pc, #276]	; (f801f3c <BOOT_OTA_LoadIMGAll+0x1e0>)
 f801e28:	f10c 0501 	add.w	r5, ip, #1
 f801e2c:	4416      	add	r6, r2
 f801e2e:	400b      	ands	r3, r1
 f801e30:	ea4f 0bc5 	mov.w	fp, r5, lsl #3
 f801e34:	f503 5380 	add.w	r3, r3, #4096	; 0x1000
 f801e38:	1a9a      	subs	r2, r3, r2
 f801e3a:	441c      	add	r4, r3
 f801e3c:	ab24      	add	r3, sp, #144	; 0x90
 f801e3e:	4940      	ldr	r1, [pc, #256]	; (f801f40 <BOOT_OTA_LoadIMGAll+0x1e4>)
 f801e40:	2002      	movs	r0, #2
 f801e42:	eb03 0ccc 	add.w	ip, r3, ip, lsl #3
 f801e46:	4623      	mov	r3, r4
 f801e48:	e94c 6216 	strd	r6, r2, [ip, #-88]	; 0x58
 f801e4c:	4a3d      	ldr	r2, [pc, #244]	; (f801f44 <BOOT_OTA_LoadIMGAll+0x1e8>)
 f801e4e:	47d0      	blx	sl
 f801e50:	2101      	movs	r1, #1
 f801e52:	2002      	movs	r0, #2
 f801e54:	4b37      	ldr	r3, [pc, #220]	; (f801f34 <BOOT_OTA_LoadIMGAll+0x1d8>)
 f801e56:	4798      	blx	r3
 f801e58:	4b37      	ldr	r3, [pc, #220]	; (f801f38 <BOOT_OTA_LoadIMGAll+0x1dc>)
 f801e5a:	4798      	blx	r3
 f801e5c:	2302      	movs	r3, #2
 f801e5e:	2201      	movs	r2, #1
 f801e60:	4938      	ldr	r1, [pc, #224]	; (f801f44 <BOOT_OTA_LoadIMGAll+0x1e8>)
 f801e62:	4837      	ldr	r0, [pc, #220]	; (f801f40 <BOOT_OTA_LoadIMGAll+0x1e4>)
 f801e64:	f7ff ff30 	bl	f801cc8 <BOOT_OTFCheck>
 f801e68:	2101      	movs	r1, #1
 f801e6a:	2204      	movs	r2, #4
 f801e6c:	463b      	mov	r3, r7
 f801e6e:	9100      	str	r1, [sp, #0]
 f801e70:	eb09 000b 	add.w	r0, r9, fp
 f801e74:	4932      	ldr	r1, [pc, #200]	; (f801f40 <BOOT_OTA_LoadIMGAll+0x1e4>)
 f801e76:	f88d 2017 	strb.w	r2, [sp, #23]
 f801e7a:	f7ff fe6d 	bl	f801b58 <BOOT_LoadSubImage>
 f801e7e:	b350      	cbz	r0, f801ed6 <BOOT_OTA_LoadIMGAll+0x17a>
 f801e80:	f89d 0017 	ldrb.w	r0, [sp, #23]
 f801e84:	b168      	cbz	r0, f801ea2 <BOOT_OTA_LoadIMGAll+0x146>
 f801e86:	1e41      	subs	r1, r0, #1
 f801e88:	ab10      	add	r3, sp, #64	; 0x40
 f801e8a:	fa55 f181 	uxtab	r1, r5, r1
 f801e8e:	eb03 01c1 	add.w	r1, r3, r1, lsl #3
 f801e92:	eb09 03c5 	add.w	r3, r9, r5, lsl #3
 f801e96:	685a      	ldr	r2, [r3, #4]
 f801e98:	3308      	adds	r3, #8
 f801e9a:	4299      	cmp	r1, r3
 f801e9c:	4414      	add	r4, r2
 f801e9e:	d1fa      	bne.n	f801e96 <BOOT_OTA_LoadIMGAll+0x13a>
 f801ea0:	4405      	add	r5, r0
 f801ea2:	4b29      	ldr	r3, [pc, #164]	; (f801f48 <BOOT_OTA_LoadIMGAll+0x1ec>)
 f801ea4:	4798      	blx	r3
 f801ea6:	2801      	cmp	r0, #1
 f801ea8:	d01f      	beq.n	f801eea <BOOT_OTA_LoadIMGAll+0x18e>
 f801eaa:	4c28      	ldr	r4, [pc, #160]	; (f801f4c <BOOT_OTA_LoadIMGAll+0x1f0>)
 f801eac:	2d0b      	cmp	r5, #11
 f801eae:	d816      	bhi.n	f801ede <BOOT_OTA_LoadIMGAll+0x182>
 f801eb0:	9803      	ldr	r0, [sp, #12]
 f801eb2:	b2ea      	uxtb	r2, r5
 f801eb4:	4b26      	ldr	r3, [pc, #152]	; (f801f50 <BOOT_OTA_LoadIMGAll+0x1f4>)
 f801eb6:	4649      	mov	r1, r9
 f801eb8:	4420      	add	r0, r4
 f801eba:	f44f 74b2 	mov.w	r4, #356	; 0x164
 f801ebe:	fb04 3308 	mla	r3, r4, r8, r3
 f801ec2:	2400      	movs	r4, #0
 f801ec4:	9400      	str	r4, [sp, #0]
 f801ec6:	f000 f991 	bl	f8021ec <BOOT_SignatureCheck>
 f801eca:	fab0 f080 	clz	r0, r0
 f801ece:	0940      	lsrs	r0, r0, #5
 f801ed0:	b025      	add	sp, #148	; 0x94
 f801ed2:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 f801ed6:	2000      	movs	r0, #0
 f801ed8:	b025      	add	sp, #148	; 0x94
 f801eda:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 f801ede:	f240 1117 	movw	r1, #279	; 0x117
 f801ee2:	481c      	ldr	r0, [pc, #112]	; (f801f54 <BOOT_OTA_LoadIMGAll+0x1f8>)
 f801ee4:	f001 f934 	bl	f803150 <io_assert_failed>
 f801ee8:	e7e2      	b.n	f801eb0 <BOOT_OTA_LoadIMGAll+0x154>
 f801eea:	9b03      	ldr	r3, [sp, #12]
 f801eec:	4620      	mov	r0, r4
 f801eee:	4c17      	ldr	r4, [pc, #92]	; (f801f4c <BOOT_OTA_LoadIMGAll+0x1f0>)
 f801ef0:	eb09 02c5 	add.w	r2, r9, r5, lsl #3
 f801ef4:	f103 0120 	add.w	r1, r3, #32
 f801ef8:	f10d 0317 	add.w	r3, sp, #23
 f801efc:	4421      	add	r1, r4
 f801efe:	f000 f89d 	bl	f80203c <BOOT_DecRDPImg>
 f801f02:	2800      	cmp	r0, #0
 f801f04:	d0e7      	beq.n	f801ed6 <BOOT_OTA_LoadIMGAll+0x17a>
 f801f06:	f89d 3017 	ldrb.w	r3, [sp, #23]
 f801f0a:	441d      	add	r5, r3
 f801f0c:	e7ce      	b.n	f801eac <BOOT_OTA_LoadIMGAll+0x150>
 f801f0e:	465d      	mov	r5, fp
 f801f10:	4662      	mov	r2, ip
 f801f12:	f04f 0b08 	mov.w	fp, #8
 f801f16:	e791      	b.n	f801e3c <BOOT_OTA_LoadIMGAll+0xe0>
 f801f18:	0f804ed4 	.word	0x0f804ed4
 f801f1c:	3000a7f0 	.word	0x3000a7f0
 f801f20:	3000a4f0 	.word	0x3000a4f0
 f801f24:	00011c1d 	.word	0x00011c1d
 f801f28:	00003a45 	.word	0x00003a45
 f801f2c:	0c000000 	.word	0x0c000000
 f801f30:	0cffffe0 	.word	0x0cffffe0
 f801f34:	00003b5d 	.word	0x00003b5d
 f801f38:	00003b89 	.word	0x00003b89
 f801f3c:	fffff000 	.word	0xfffff000
 f801f40:	0e000000 	.word	0x0e000000
 f801f44:	0effffe0 	.word	0x0effffe0
 f801f48:	0000c229 	.word	0x0000c229
 f801f4c:	3000a4d0 	.word	0x3000a4d0
 f801f50:	3000a208 	.word	0x3000a208
 f801f54:	0f804e40 	.word	0x0f804e40
 f801f58:	0000c249 	.word	0x0000c249
 f801f5c:	00003b25 	.word	0x00003b25

0f801f60 <BOOT_OTA_IMG>:
 f801f60:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 f801f64:	4c2e      	ldr	r4, [pc, #184]	; (f802020 <BOOT_OTA_IMG+0xc0>)
 f801f66:	b084      	sub	sp, #16
 f801f68:	2200      	movs	r2, #0
 f801f6a:	2002      	movs	r0, #2
 f801f6c:	4621      	mov	r1, r4
 f801f6e:	4e2d      	ldr	r6, [pc, #180]	; (f802024 <BOOT_OTA_IMG+0xc4>)
 f801f70:	f7fe f98c 	bl	f80028c <flash_get_layout_info>
 f801f74:	2200      	movs	r2, #0
 f801f76:	1d21      	adds	r1, r4, #4
 f801f78:	2003      	movs	r0, #3
 f801f7a:	f7fe f987 	bl	f80028c <flash_get_layout_info>
 f801f7e:	e9d4 2300 	ldrd	r2, r3, [r4]
 f801f82:	f503 5380 	add.w	r3, r3, #4096	; 0x1000
 f801f86:	f502 5280 	add.w	r2, r2, #4096	; 0x1000
 f801f8a:	e9c4 2302 	strd	r2, r3, [r4, #8]
 f801f8e:	f7ff fd4b 	bl	f801a28 <BOOT_OTA_SlotSelect>
 f801f92:	7833      	ldrb	r3, [r6, #0]
 f801f94:	b353      	cbz	r3, f801fec <BOOT_OTA_IMG+0x8c>
 f801f96:	4604      	mov	r4, r0
 f801f98:	2500      	movs	r5, #0
 f801f9a:	f8df 809c 	ldr.w	r8, [pc, #156]	; f802038 <BOOT_OTA_IMG+0xd8>
 f801f9e:	f44f 77b2 	mov.w	r7, #356	; 0x164
 f801fa2:	4621      	mov	r1, r4
 f801fa4:	fb07 8004 	mla	r0, r7, r4, r8
 f801fa8:	f000 fa20 	bl	f8023ec <BOOT_CertificateCheck>
 f801fac:	4603      	mov	r3, r0
 f801fae:	1c62      	adds	r2, r4, #1
 f801fb0:	4620      	mov	r0, r4
 f801fb2:	b343      	cbz	r3, f802006 <BOOT_OTA_IMG+0xa6>
 f801fb4:	f002 0401 	and.w	r4, r2, #1
 f801fb8:	3501      	adds	r5, #1
 f801fba:	7833      	ldrb	r3, [r6, #0]
 f801fbc:	b2ed      	uxtb	r5, r5
 f801fbe:	42ab      	cmp	r3, r5
 f801fc0:	d8ef      	bhi.n	f801fa2 <BOOT_OTA_IMG+0x42>
 f801fc2:	429d      	cmp	r5, r3
 f801fc4:	d012      	beq.n	f801fec <BOOT_OTA_IMG+0x8c>
 f801fc6:	4918      	ldr	r1, [pc, #96]	; (f802028 <BOOT_OTA_IMG+0xc8>)
 f801fc8:	1c60      	adds	r0, r4, #1
 f801fca:	4b18      	ldr	r3, [pc, #96]	; (f80202c <BOOT_OTA_IMG+0xcc>)
 f801fcc:	2249      	movs	r2, #73	; 0x49
 f801fce:	f851 1034 	ldr.w	r1, [r1, r4, lsl #3]
 f801fd2:	9000      	str	r0, [sp, #0]
 f801fd4:	b28d      	uxth	r5, r1
 f801fd6:	0c08      	lsrs	r0, r1, #16
 f801fd8:	4915      	ldr	r1, [pc, #84]	; (f802030 <BOOT_OTA_IMG+0xd0>)
 f801fda:	9502      	str	r5, [sp, #8]
 f801fdc:	9001      	str	r0, [sp, #4]
 f801fde:	2004      	movs	r0, #4
 f801fe0:	f000 fa9a 	bl	f802518 <rtk_log_write>
 f801fe4:	4620      	mov	r0, r4
 f801fe6:	b004      	add	sp, #16
 f801fe8:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 f801fec:	2400      	movs	r4, #0
 f801fee:	4b11      	ldr	r3, [pc, #68]	; (f802034 <BOOT_OTA_IMG+0xd4>)
 f801ff0:	2245      	movs	r2, #69	; 0x45
 f801ff2:	490f      	ldr	r1, [pc, #60]	; (f802030 <BOOT_OTA_IMG+0xd0>)
 f801ff4:	2002      	movs	r0, #2
 f801ff6:	f000 fa8f 	bl	f802518 <rtk_log_write>
 f801ffa:	f000 f8cd 	bl	f802198 <BOOT_Validate_Fail_Stuck>
 f801ffe:	4620      	mov	r0, r4
 f802000:	b004      	add	sp, #16
 f802002:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 f802006:	f7ff fea9 	bl	f801d5c <BOOT_OTA_LoadIMGAll>
 f80200a:	2801      	cmp	r0, #1
 f80200c:	f104 0301 	add.w	r3, r4, #1
 f802010:	d002      	beq.n	f802018 <BOOT_OTA_IMG+0xb8>
 f802012:	f003 0401 	and.w	r4, r3, #1
 f802016:	e7cf      	b.n	f801fb8 <BOOT_OTA_IMG+0x58>
 f802018:	7833      	ldrb	r3, [r6, #0]
 f80201a:	429d      	cmp	r5, r3
 f80201c:	d1d3      	bne.n	f801fc6 <BOOT_OTA_IMG+0x66>
 f80201e:	e7e5      	b.n	f801fec <BOOT_OTA_IMG+0x8c>
 f802020:	3000a7f0 	.word	0x3000a7f0
 f802024:	3000a920 	.word	0x3000a920
 f802028:	3000a928 	.word	0x3000a928
 f80202c:	0f804e78 	.word	0x0f804e78
 f802030:	0f804988 	.word	0x0f804988
 f802034:	0f804ea4 	.word	0x0f804ea4
 f802038:	3000a208 	.word	0x3000a208

0f80203c <BOOT_DecRDPImg>:
 f80203c:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 f802040:	460c      	mov	r4, r1
 f802042:	b0e5      	sub	sp, #404	; 0x194
 f802044:	494a      	ldr	r1, [pc, #296]	; (f802170 <BOOT_DecRDPImg+0x134>)
 f802046:	4606      	mov	r6, r0
 f802048:	4690      	mov	r8, r2
 f80204a:	af08      	add	r7, sp, #32
 f80204c:	9206      	str	r2, [sp, #24]
 f80204e:	2500      	movs	r5, #0
 f802050:	2202      	movs	r2, #2
 f802052:	c903      	ldmia	r1, {r0, r1}
 f802054:	701a      	strb	r2, [r3, #0]
 f802056:	463b      	mov	r3, r7
 f802058:	e887 0003 	stmia.w	r7, {r0, r1}
 f80205c:	4631      	mov	r1, r6
 f80205e:	4640      	mov	r0, r8
 f802060:	9500      	str	r5, [sp, #0]
 f802062:	f7ff fd79 	bl	f801b58 <BOOT_LoadSubImage>
 f802066:	2801      	cmp	r0, #1
 f802068:	d072      	beq.n	f802150 <BOOT_DecRDPImg+0x114>
 f80206a:	4b42      	ldr	r3, [pc, #264]	; (f802174 <BOOT_DecRDPImg+0x138>)
 f80206c:	4628      	mov	r0, r5
 f80206e:	9705      	str	r7, [sp, #20]
 f802070:	4798      	blx	r3
 f802072:	2101      	movs	r1, #1
 f802074:	4b40      	ldr	r3, [pc, #256]	; (f802178 <BOOT_DecRDPImg+0x13c>)
 f802076:	4628      	mov	r0, r5
 f802078:	f20d 1a77 	addw	sl, sp, #375	; 0x177
 f80207c:	4798      	blx	r3
 f80207e:	2120      	movs	r1, #32
 f802080:	4b3e      	ldr	r3, [pc, #248]	; (f80217c <BOOT_DecRDPImg+0x140>)
 f802082:	a80e      	add	r0, sp, #56	; 0x38
 f802084:	4798      	blx	r3
 f802086:	4643      	mov	r3, r8
 f802088:	46c3      	mov	fp, r8
 f80208a:	4621      	mov	r1, r4
 f80208c:	3310      	adds	r3, #16
 f80208e:	2210      	movs	r2, #16
 f802090:	a80a      	add	r0, sp, #40	; 0x28
 f802092:	f8df 80fc 	ldr.w	r8, [pc, #252]	; f802190 <BOOT_DecRDPImg+0x154>
 f802096:	f02a 0a1f 	bic.w	sl, sl, #31
 f80209a:	f8df 90f8 	ldr.w	r9, [pc, #248]	; f802194 <BOOT_DecRDPImg+0x158>
 f80209e:	9307      	str	r3, [sp, #28]
 f8020a0:	47c0      	blx	r8
 f8020a2:	2210      	movs	r2, #16
 f8020a4:	4631      	mov	r1, r6
 f8020a6:	a816      	add	r0, sp, #88	; 0x58
 f8020a8:	47c0      	blx	r8
 f8020aa:	2310      	movs	r3, #16
 f8020ac:	aa0a      	add	r2, sp, #40	; 0x28
 f8020ae:	a816      	add	r0, sp, #88	; 0x58
 f8020b0:	4619      	mov	r1, r3
 f8020b2:	f8cd a000 	str.w	sl, [sp]
 f8020b6:	47c8      	blx	r9
 f8020b8:	f8da 3000 	ldr.w	r3, [sl]
 f8020bc:	4a30      	ldr	r2, [pc, #192]	; (f802180 <BOOT_DecRDPImg+0x144>)
 f8020be:	4293      	cmp	r3, r2
 f8020c0:	d149      	bne.n	f802156 <BOOT_DecRDPImg+0x11a>
 f8020c2:	f102 427b 	add.w	r2, r2, #4211081216	; 0xfb000000
 f8020c6:	f8da 3004 	ldr.w	r3, [sl, #4]
 f8020ca:	f502 0278 	add.w	r2, r2, #16252928	; 0xf80000
 f8020ce:	f502 62c0 	add.w	r2, r2, #1536	; 0x600
 f8020d2:	4293      	cmp	r3, r2
 f8020d4:	d13f      	bne.n	f802156 <BOOT_DecRDPImg+0x11a>
 f8020d6:	f8da 700c 	ldr.w	r7, [sl, #12]
 f8020da:	2004      	movs	r0, #4
 f8020dc:	9a05      	ldr	r2, [sp, #20]
 f8020de:	3f20      	subs	r7, #32
 f8020e0:	4928      	ldr	r1, [pc, #160]	; (f802184 <BOOT_DecRDPImg+0x148>)
 f8020e2:	9701      	str	r7, [sp, #4]
 f8020e4:	f8da 5008 	ldr.w	r5, [sl, #8]
 f8020e8:	3520      	adds	r5, #32
 f8020ea:	9502      	str	r5, [sp, #8]
 f8020ec:	f852 3b04 	ldr.w	r3, [r2], #4
 f8020f0:	9205      	str	r2, [sp, #20]
 f8020f2:	2249      	movs	r2, #73	; 0x49
 f8020f4:	9300      	str	r3, [sp, #0]
 f8020f6:	4b24      	ldr	r3, [pc, #144]	; (f802188 <BOOT_DecRDPImg+0x14c>)
 f8020f8:	f000 fa0e 	bl	f802518 <rtk_log_write>
 f8020fc:	9b06      	ldr	r3, [sp, #24]
 f8020fe:	b10b      	cbz	r3, f802104 <BOOT_DecRDPImg+0xc8>
 f802100:	e9cb 7500 	strd	r7, r5, [fp]
 f802104:	b1d5      	cbz	r5, f80213c <BOOT_DecRDPImg+0x100>
 f802106:	f5b5 7f80 	cmp.w	r5, #256	; 0x100
 f80210a:	462c      	mov	r4, r5
 f80210c:	4631      	mov	r1, r6
 f80210e:	a816      	add	r0, sp, #88	; 0x58
 f802110:	bf28      	it	cs
 f802112:	f44f 7480 	movcs.w	r4, #256	; 0x100
 f802116:	4622      	mov	r2, r4
 f802118:	4426      	add	r6, r4
 f80211a:	47c0      	blx	r8
 f80211c:	2310      	movs	r3, #16
 f80211e:	aa0a      	add	r2, sp, #40	; 0x28
 f802120:	4621      	mov	r1, r4
 f802122:	9700      	str	r7, [sp, #0]
 f802124:	a816      	add	r0, sp, #88	; 0x58
 f802126:	47c8      	blx	r9
 f802128:	ab16      	add	r3, sp, #88	; 0x58
 f80212a:	f1a4 0110 	sub.w	r1, r4, #16
 f80212e:	2210      	movs	r2, #16
 f802130:	a80a      	add	r0, sp, #40	; 0x28
 f802132:	4427      	add	r7, r4
 f802134:	4419      	add	r1, r3
 f802136:	47c0      	blx	r8
 f802138:	1b2d      	subs	r5, r5, r4
 f80213a:	d1e4      	bne.n	f802106 <BOOT_DecRDPImg+0xca>
 f80213c:	f10b 0b08 	add.w	fp, fp, #8
 f802140:	9b07      	ldr	r3, [sp, #28]
 f802142:	455b      	cmp	r3, fp
 f802144:	d1ad      	bne.n	f8020a2 <BOOT_DecRDPImg+0x66>
 f802146:	2100      	movs	r1, #0
 f802148:	4b0b      	ldr	r3, [pc, #44]	; (f802178 <BOOT_DecRDPImg+0x13c>)
 f80214a:	4608      	mov	r0, r1
 f80214c:	4798      	blx	r3
 f80214e:	2001      	movs	r0, #1
 f802150:	b065      	add	sp, #404	; 0x194
 f802152:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 f802156:	4b0d      	ldr	r3, [pc, #52]	; (f80218c <BOOT_DecRDPImg+0x150>)
 f802158:	2245      	movs	r2, #69	; 0x45
 f80215a:	490a      	ldr	r1, [pc, #40]	; (f802184 <BOOT_DecRDPImg+0x148>)
 f80215c:	2002      	movs	r0, #2
 f80215e:	f000 f9db 	bl	f802518 <rtk_log_write>
 f802162:	2100      	movs	r1, #0
 f802164:	4b04      	ldr	r3, [pc, #16]	; (f802178 <BOOT_DecRDPImg+0x13c>)
 f802166:	4608      	mov	r0, r1
 f802168:	4798      	blx	r3
 f80216a:	2000      	movs	r0, #0
 f80216c:	e7f0      	b.n	f802150 <BOOT_DecRDPImg+0x114>
 f80216e:	bf00      	nop
 f802170:	0f804fa0 	.word	0x0f804fa0
 f802174:	00000dad 	.word	0x00000dad
 f802178:	00000dfd 	.word	0x00000dfd
 f80217c:	00001c95 	.word	0x00001c95
 f802180:	35393138 	.word	0x35393138
 f802184:	0f804988 	.word	0x0f804988
 f802188:	0f804f3c 	.word	0x0f804f3c
 f80218c:	0f804f20 	.word	0x0f804f20
 f802190:	00011c1d 	.word	0x00011c1d
 f802194:	00001d49 	.word	0x00001d49

0f802198 <BOOT_Validate_Fail_Stuck>:
 f802198:	b510      	push	{r4, lr}
 f80219a:	4a0f      	ldr	r2, [pc, #60]	; (f8021d8 <BOOT_Validate_Fail_Stuck+0x40>)
 f80219c:	f382 8808 	msr	MSP, r2
 f8021a0:	4b0e      	ldr	r3, [pc, #56]	; (f8021dc <BOOT_Validate_Fail_Stuck+0x44>)
 f8021a2:	2100      	movs	r1, #0
 f8021a4:	f843 1b04 	str.w	r1, [r3], #4
 f8021a8:	4293      	cmp	r3, r2
 f8021aa:	d1fb      	bne.n	f8021a4 <BOOT_Validate_Fail_Stuck+0xc>
 f8021ac:	4a0c      	ldr	r2, [pc, #48]	; (f8021e0 <BOOT_Validate_Fail_Stuck+0x48>)
 f8021ae:	6953      	ldr	r3, [r2, #20]
 f8021b0:	03db      	lsls	r3, r3, #15
 f8021b2:	d50c      	bpl.n	f8021ce <BOOT_Validate_Fail_Stuck+0x36>
 f8021b4:	f3bf 8f4f 	dsb	sy
 f8021b8:	4b08      	ldr	r3, [pc, #32]	; (f8021dc <BOOT_Validate_Fail_Stuck+0x44>)
 f8021ba:	490a      	ldr	r1, [pc, #40]	; (f8021e4 <BOOT_Validate_Fail_Stuck+0x4c>)
 f8021bc:	f8c2 3270 	str.w	r3, [r2, #624]	; 0x270
 f8021c0:	3320      	adds	r3, #32
 f8021c2:	428b      	cmp	r3, r1
 f8021c4:	d1fa      	bne.n	f8021bc <BOOT_Validate_Fail_Stuck+0x24>
 f8021c6:	f3bf 8f4f 	dsb	sy
 f8021ca:	f3bf 8f6f 	isb	sy
 f8021ce:	4c06      	ldr	r4, [pc, #24]	; (f8021e8 <BOOT_Validate_Fail_Stuck+0x50>)
 f8021d0:	f44f 707a 	mov.w	r0, #1000	; 0x3e8
 f8021d4:	47a0      	blx	r4
 f8021d6:	e7fb      	b.n	f8021d0 <BOOT_Validate_Fail_Stuck+0x38>
 f8021d8:	30009ffc 	.word	0x30009ffc
 f8021dc:	30009000 	.word	0x30009000
 f8021e0:	e000ed00 	.word	0xe000ed00
 f8021e4:	3000a000 	.word	0x3000a000
 f8021e8:	0000951d 	.word	0x0000951d

0f8021ec <BOOT_SignatureCheck>:
 f8021ec:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 f8021f0:	4690      	mov	r8, r2
 f8021f2:	b086      	sub	sp, #24
 f8021f4:	4a75      	ldr	r2, [pc, #468]	; (f8023cc <BOOT_SignatureCheck+0x1e0>)
 f8021f6:	461f      	mov	r7, r3
 f8021f8:	4b75      	ldr	r3, [pc, #468]	; (f8023d0 <BOOT_SignatureCheck+0x1e4>)
 f8021fa:	4681      	mov	r9, r0
 f8021fc:	468a      	mov	sl, r1
 f8021fe:	7815      	ldrb	r5, [r2, #0]
 f802200:	9e0e      	ldr	r6, [sp, #56]	; 0x38
 f802202:	e893 0007 	ldmia.w	r3, {r0, r1, r2}
 f802206:	ab06      	add	r3, sp, #24
 f802208:	e903 0007 	stmdb	r3, {r0, r1, r2}
 f80220c:	b91d      	cbnz	r5, f802216 <BOOT_SignatureCheck+0x2a>
 f80220e:	4628      	mov	r0, r5
 f802210:	b006      	add	sp, #24
 f802212:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
 f802216:	4b6f      	ldr	r3, [pc, #444]	; (f8023d4 <BOOT_SignatureCheck+0x1e8>)
 f802218:	2000      	movs	r0, #0
 f80221a:	4798      	blx	r3
 f80221c:	f899 3013 	ldrb.w	r3, [r9, #19]
 f802220:	f899 2012 	ldrb.w	r2, [r9, #18]
 f802224:	f10d 010b 	add.w	r1, sp, #11
 f802228:	f10d 000a 	add.w	r0, sp, #10
 f80222c:	f002 f96c 	bl	f804508 <__SBOOT_Validate_Algorithm_veneer>
 f802230:	4605      	mov	r5, r0
 f802232:	b9e8      	cbnz	r0, f802270 <BOOT_SignatureCheck+0x84>
 f802234:	69f9      	ldr	r1, [r7, #28]
 f802236:	b919      	cbnz	r1, f802240 <BOOT_SignatureCheck+0x54>
 f802238:	e0c5      	b.n	f8023c6 <BOOT_SignatureCheck+0x1da>
 f80223a:	b2d0      	uxtb	r0, r2
 f80223c:	4288      	cmp	r0, r1
 f80223e:	d208      	bcs.n	f802252 <BOOT_SignatureCheck+0x66>
 f802240:	eb00 04c0 	add.w	r4, r0, r0, lsl #3
 f802244:	1c42      	adds	r2, r0, #1
 f802246:	eb07 0484 	add.w	r4, r7, r4, lsl #2
 f80224a:	f8d4 30b0 	ldr.w	r3, [r4, #176]	; 0xb0
 f80224e:	42b3      	cmp	r3, r6
 f802250:	d1f3      	bne.n	f80223a <BOOT_SignatureCheck+0x4e>
 f802252:	2324      	movs	r3, #36	; 0x24
 f802254:	25b4      	movs	r5, #180	; 0xb4
 f802256:	fb13 5500 	smlabb	r5, r3, r0, r5
 f80225a:	f109 0470 	add.w	r4, r9, #112	; 0x70
 f80225e:	197a      	adds	r2, r7, r5
 f802260:	f89d 000a 	ldrb.w	r0, [sp, #10]
 f802264:	4621      	mov	r1, r4
 f802266:	f002 f94b 	bl	f804500 <__SBOOT_Validate_PubKey_veneer>
 f80226a:	4605      	mov	r5, r0
 f80226c:	2800      	cmp	r0, #0
 f80226e:	d07d      	beq.n	f80236c <BOOT_SignatureCheck+0x180>
 f802270:	f899 2011 	ldrb.w	r2, [r9, #17]
 f802274:	a906      	add	r1, sp, #24
 f802276:	9501      	str	r5, [sp, #4]
 f802278:	2002      	movs	r0, #2
 f80227a:	eb01 0282 	add.w	r2, r1, r2, lsl #2
 f80227e:	4b56      	ldr	r3, [pc, #344]	; (f8023d8 <BOOT_SignatureCheck+0x1ec>)
 f802280:	4956      	ldr	r1, [pc, #344]	; (f8023dc <BOOT_SignatureCheck+0x1f0>)
 f802282:	f852 2c0c 	ldr.w	r2, [r2, #-12]
 f802286:	9200      	str	r2, [sp, #0]
 f802288:	2245      	movs	r2, #69	; 0x45
 f80228a:	f000 f945 	bl	f802518 <rtk_log_write>
 f80228e:	f1b8 0f00 	cmp.w	r8, #0
 f802292:	d046      	beq.n	f802322 <BOOT_SignatureCheck+0x136>
 f802294:	f108 38ff 	add.w	r8, r8, #4294967295	; 0xffffffff
 f802298:	4656      	mov	r6, sl
 f80229a:	f10a 0a08 	add.w	sl, sl, #8
 f80229e:	f04f 0900 	mov.w	r9, #0
 f8022a2:	fa5f f888 	uxtb.w	r8, r8
 f8022a6:	1d37      	adds	r7, r6, #4
 f8022a8:	4c4d      	ldr	r4, [pc, #308]	; (f8023e0 <BOOT_SignatureCheck+0x1f4>)
 f8022aa:	eb0a 0ac8 	add.w	sl, sl, r8, lsl #3
 f8022ae:	f8df 8138 	ldr.w	r8, [pc, #312]	; f8023e8 <BOOT_SignatureCheck+0x1fc>
 f8022b2:	e003      	b.n	f8022bc <BOOT_SignatureCheck+0xd0>
 f8022b4:	3608      	adds	r6, #8
 f8022b6:	3708      	adds	r7, #8
 f8022b8:	45b2      	cmp	sl, r6
 f8022ba:	d032      	beq.n	f802322 <BOOT_SignatureCheck+0x136>
 f8022bc:	f857 0c04 	ldr.w	r0, [r7, #-4]
 f8022c0:	f100 4378 	add.w	r3, r0, #4160749568	; 0xf8000000
 f8022c4:	f1b3 6f00 	cmp.w	r3, #134217728	; 0x8000000
 f8022c8:	d3f4      	bcc.n	f8022b4 <BOOT_SignatureCheck+0xc8>
 f8022ca:	2100      	movs	r1, #0
 f8022cc:	6872      	ldr	r2, [r6, #4]
 f8022ce:	47c0      	blx	r8
 f8022d0:	6963      	ldr	r3, [r4, #20]
 f8022d2:	03d9      	lsls	r1, r3, #15
 f8022d4:	d5ee      	bpl.n	f8022b4 <BOOT_SignatureCheck+0xc8>
 f8022d6:	f857 2c04 	ldr.w	r2, [r7, #-4]
 f8022da:	6873      	ldr	r3, [r6, #4]
 f8022dc:	ea02 0103 	and.w	r1, r2, r3
 f8022e0:	3101      	adds	r1, #1
 f8022e2:	d023      	beq.n	f80232c <BOOT_SignatureCheck+0x140>
 f8022e4:	f012 011f 	ands.w	r1, r2, #31
 f8022e8:	d002      	beq.n	f8022f0 <BOOT_SignatureCheck+0x104>
 f8022ea:	440b      	add	r3, r1
 f8022ec:	f022 021f 	bic.w	r2, r2, #31
 f8022f0:	f3bf 8f4f 	dsb	sy
 f8022f4:	2b00      	cmp	r3, #0
 f8022f6:	dd0c      	ble.n	f802312 <BOOT_SignatureCheck+0x126>
 f8022f8:	1e59      	subs	r1, r3, #1
 f8022fa:	f102 0320 	add.w	r3, r2, #32
 f8022fe:	f021 011f 	bic.w	r1, r1, #31
 f802302:	4419      	add	r1, r3
 f802304:	428b      	cmp	r3, r1
 f802306:	f8c4 2270 	str.w	r2, [r4, #624]	; 0x270
 f80230a:	461a      	mov	r2, r3
 f80230c:	f103 0320 	add.w	r3, r3, #32
 f802310:	d1f8      	bne.n	f802304 <BOOT_SignatureCheck+0x118>
 f802312:	f3bf 8f4f 	dsb	sy
 f802316:	f3bf 8f6f 	isb	sy
 f80231a:	3608      	adds	r6, #8
 f80231c:	3708      	adds	r7, #8
 f80231e:	45b2      	cmp	sl, r6
 f802320:	d1cc      	bne.n	f8022bc <BOOT_SignatureCheck+0xd0>
 f802322:	b2ed      	uxtb	r5, r5
 f802324:	4628      	mov	r0, r5
 f802326:	b006      	add	sp, #24
 f802328:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
 f80232c:	f8c4 9084 	str.w	r9, [r4, #132]	; 0x84
 f802330:	f3bf 8f4f 	dsb	sy
 f802334:	f8d4 3080 	ldr.w	r3, [r4, #128]	; 0x80
 f802338:	f643 7ee0 	movw	lr, #16352	; 0x3fe0
 f80233c:	f3c3 304e 	ubfx	r0, r3, #13, #15
 f802340:	f3c3 0cc9 	ubfx	ip, r3, #3, #10
 f802344:	0140      	lsls	r0, r0, #5
 f802346:	ea00 010e 	and.w	r1, r0, lr
 f80234a:	4663      	mov	r3, ip
 f80234c:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
 f802350:	3b01      	subs	r3, #1
 f802352:	f8c4 2274 	str.w	r2, [r4, #628]	; 0x274
 f802356:	1c5a      	adds	r2, r3, #1
 f802358:	d1f8      	bne.n	f80234c <BOOT_SignatureCheck+0x160>
 f80235a:	3820      	subs	r0, #32
 f80235c:	f110 0f20 	cmn.w	r0, #32
 f802360:	d1f1      	bne.n	f802346 <BOOT_SignatureCheck+0x15a>
 f802362:	f3bf 8f4f 	dsb	sy
 f802366:	f3bf 8f6f 	isb	sy
 f80236a:	e7d6      	b.n	f80231a <BOOT_SignatureCheck+0x12e>
 f80236c:	f509 7380 	add.w	r3, r9, #256	; 0x100
 f802370:	4622      	mov	r2, r4
 f802372:	f89d 100b 	ldrb.w	r1, [sp, #11]
 f802376:	9301      	str	r3, [sp, #4]
 f802378:	f44f 7380 	mov.w	r3, #256	; 0x100
 f80237c:	f89d 000a 	ldrb.w	r0, [sp, #10]
 f802380:	9300      	str	r3, [sp, #0]
 f802382:	464b      	mov	r3, r9
 f802384:	f002 f8cc 	bl	f804520 <__SBOOT_Validate_Signature_veneer>
 f802388:	4605      	mov	r5, r0
 f80238a:	2800      	cmp	r0, #0
 f80238c:	f47f af70 	bne.w	f802270 <BOOT_SignatureCheck+0x84>
 f802390:	4643      	mov	r3, r8
 f802392:	4652      	mov	r2, sl
 f802394:	f109 0130 	add.w	r1, r9, #48	; 0x30
 f802398:	f89d 000b 	ldrb.w	r0, [sp, #11]
 f80239c:	f002 f8a8 	bl	f8044f0 <__SBOOT_Validate_ImgHash_veneer>
 f8023a0:	4605      	mov	r5, r0
 f8023a2:	2800      	cmp	r0, #0
 f8023a4:	f47f af64 	bne.w	f802270 <BOOT_SignatureCheck+0x84>
 f8023a8:	f899 0011 	ldrb.w	r0, [r9, #17]
 f8023ac:	ac06      	add	r4, sp, #24
 f8023ae:	4b0d      	ldr	r3, [pc, #52]	; (f8023e4 <BOOT_SignatureCheck+0x1f8>)
 f8023b0:	2249      	movs	r2, #73	; 0x49
 f8023b2:	eb04 0080 	add.w	r0, r4, r0, lsl #2
 f8023b6:	4909      	ldr	r1, [pc, #36]	; (f8023dc <BOOT_SignatureCheck+0x1f0>)
 f8023b8:	f850 0c0c 	ldr.w	r0, [r0, #-12]
 f8023bc:	9000      	str	r0, [sp, #0]
 f8023be:	2004      	movs	r0, #4
 f8023c0:	f000 f8aa 	bl	f802518 <rtk_log_write>
 f8023c4:	e723      	b.n	f80220e <BOOT_SignatureCheck+0x22>
 f8023c6:	25b4      	movs	r5, #180	; 0xb4
 f8023c8:	e747      	b.n	f80225a <BOOT_SignatureCheck+0x6e>
 f8023ca:	bf00      	nop
 f8023cc:	3000a938 	.word	0x3000a938
 f8023d0:	0f804fa8 	.word	0x0f804fa8
 f8023d4:	00000dd5 	.word	0x00000dd5
 f8023d8:	0f804f60 	.word	0x0f804f60
 f8023dc:	0f804988 	.word	0x0f804988
 f8023e0:	e000ed00 	.word	0xe000ed00
 f8023e4:	0f804f50 	.word	0x0f804f50
 f8023e8:	00011de5 	.word	0x00011de5

0f8023ec <BOOT_CertificateCheck>:
 f8023ec:	b5f0      	push	{r4, r5, r6, r7, lr}
 f8023ee:	4b40      	ldr	r3, [pc, #256]	; (f8024f0 <BOOT_CertificateCheck+0x104>)
 f8023f0:	b08d      	sub	sp, #52	; 0x34
 f8023f2:	4604      	mov	r4, r0
 f8023f4:	460d      	mov	r5, r1
 f8023f6:	4798      	blx	r3
 f8023f8:	b310      	cbz	r0, f802440 <BOOT_CertificateCheck+0x54>
 f8023fa:	4b3e      	ldr	r3, [pc, #248]	; (f8024f4 <BOOT_CertificateCheck+0x108>)
 f8023fc:	4798      	blx	r3
 f8023fe:	4e3e      	ldr	r6, [pc, #248]	; (f8024f8 <BOOT_CertificateCheck+0x10c>)
 f802400:	f000 031f 	and.w	r3, r0, #31
 f802404:	2220      	movs	r2, #32
 f802406:	483d      	ldr	r0, [pc, #244]	; (f8024fc <BOOT_CertificateCheck+0x110>)
 f802408:	f04f 0e01 	mov.w	lr, #1
 f80240c:	f813 c000 	ldrb.w	ip, [r3, r0]
 f802410:	f103 0130 	add.w	r1, r3, #48	; 0x30
 f802414:	3a01      	subs	r2, #1
 f802416:	3301      	adds	r3, #1
 f802418:	fa5f fc8c 	uxtb.w	ip, ip
 f80241c:	4469      	add	r1, sp
 f80241e:	f003 031f 	and.w	r3, r3, #31
 f802422:	f1bc 0fff 	cmp.w	ip, #255	; 0xff
 f802426:	f801 cc20 	strb.w	ip, [r1, #-32]
 f80242a:	bf18      	it	ne
 f80242c:	f886 e000 	strbne.w	lr, [r6]
 f802430:	f012 02ff 	ands.w	r2, r2, #255	; 0xff
 f802434:	d1ea      	bne.n	f80240c <BOOT_CertificateCheck+0x20>
 f802436:	7833      	ldrb	r3, [r6, #0]
 f802438:	b94b      	cbnz	r3, f80244e <BOOT_CertificateCheck+0x62>
 f80243a:	2000      	movs	r0, #0
 f80243c:	b00d      	add	sp, #52	; 0x34
 f80243e:	bdf0      	pop	{r4, r5, r6, r7, pc}
 f802440:	4b2f      	ldr	r3, [pc, #188]	; (f802500 <BOOT_CertificateCheck+0x114>)
 f802442:	4798      	blx	r3
 f802444:	bb20      	cbnz	r0, f802490 <BOOT_CertificateCheck+0xa4>
 f802446:	4b2c      	ldr	r3, [pc, #176]	; (f8024f8 <BOOT_CertificateCheck+0x10c>)
 f802448:	781b      	ldrb	r3, [r3, #0]
 f80244a:	2b00      	cmp	r3, #0
 f80244c:	d0f5      	beq.n	f80243a <BOOT_CertificateCheck+0x4e>
 f80244e:	4b2d      	ldr	r3, [pc, #180]	; (f802504 <BOOT_CertificateCheck+0x118>)
 f802450:	2000      	movs	r0, #0
 f802452:	4798      	blx	r3
 f802454:	7ce3      	ldrb	r3, [r4, #19]
 f802456:	7ca2      	ldrb	r2, [r4, #18]
 f802458:	f10d 010f 	add.w	r1, sp, #15
 f80245c:	f10d 000e 	add.w	r0, sp, #14
 f802460:	f002 f852 	bl	f804508 <__SBOOT_Validate_Algorithm_veneer>
 f802464:	4606      	mov	r6, r0
 f802466:	b948      	cbnz	r0, f80247c <BOOT_CertificateCheck+0x90>
 f802468:	f104 0720 	add.w	r7, r4, #32
 f80246c:	aa04      	add	r2, sp, #16
 f80246e:	f89d 000e 	ldrb.w	r0, [sp, #14]
 f802472:	4639      	mov	r1, r7
 f802474:	f002 f844 	bl	f804500 <__SBOOT_Validate_PubKey_veneer>
 f802478:	4606      	mov	r6, r0
 f80247a:	b328      	cbz	r0, f8024c8 <BOOT_CertificateCheck+0xdc>
 f80247c:	4b22      	ldr	r3, [pc, #136]	; (f802508 <BOOT_CertificateCheck+0x11c>)
 f80247e:	2245      	movs	r2, #69	; 0x45
 f802480:	4922      	ldr	r1, [pc, #136]	; (f80250c <BOOT_CertificateCheck+0x120>)
 f802482:	2002      	movs	r0, #2
 f802484:	9600      	str	r6, [sp, #0]
 f802486:	f000 f847 	bl	f802518 <rtk_log_write>
 f80248a:	b2f0      	uxtb	r0, r6
 f80248c:	b00d      	add	sp, #52	; 0x34
 f80248e:	bdf0      	pop	{r4, r5, r6, r7, pc}
 f802490:	2201      	movs	r2, #1
 f802492:	4e19      	ldr	r6, [pc, #100]	; (f8024f8 <BOOT_CertificateCheck+0x10c>)
 f802494:	4b17      	ldr	r3, [pc, #92]	; (f8024f4 <BOOT_CertificateCheck+0x108>)
 f802496:	7032      	strb	r2, [r6, #0]
 f802498:	4798      	blx	r3
 f80249a:	4b1d      	ldr	r3, [pc, #116]	; (f802510 <BOOT_CertificateCheck+0x124>)
 f80249c:	f000 001f 	and.w	r0, r0, #31
 f8024a0:	681b      	ldr	r3, [r3, #0]
 f8024a2:	f103 4c82 	add.w	ip, r3, #1090519040	; 0x41000000
 f8024a6:	2320      	movs	r3, #32
 f8024a8:	f100 0230 	add.w	r2, r0, #48	; 0x30
 f8024ac:	3b01      	subs	r3, #1
 f8024ae:	f810 100c 	ldrb.w	r1, [r0, ip]
 f8024b2:	3001      	adds	r0, #1
 f8024b4:	446a      	add	r2, sp
 f8024b6:	f013 03ff 	ands.w	r3, r3, #255	; 0xff
 f8024ba:	f000 001f 	and.w	r0, r0, #31
 f8024be:	f802 1c20 	strb.w	r1, [r2, #-32]
 f8024c2:	d1f1      	bne.n	f8024a8 <BOOT_CertificateCheck+0xbc>
 f8024c4:	7833      	ldrb	r3, [r6, #0]
 f8024c6:	e7b7      	b.n	f802438 <BOOT_CertificateCheck+0x4c>
 f8024c8:	4a12      	ldr	r2, [pc, #72]	; (f802514 <BOOT_CertificateCheck+0x128>)
 f8024ca:	eb05 05c5 	add.w	r5, r5, r5, lsl #3
 f8024ce:	69a1      	ldr	r1, [r4, #24]
 f8024d0:	4623      	mov	r3, r4
 f8024d2:	eb02 1505 	add.w	r5, r2, r5, lsl #4
 f8024d6:	f89d 000e 	ldrb.w	r0, [sp, #14]
 f8024da:	9100      	str	r1, [sp, #0]
 f8024dc:	463a      	mov	r2, r7
 f8024de:	f89d 100f 	ldrb.w	r1, [sp, #15]
 f8024e2:	9501      	str	r5, [sp, #4]
 f8024e4:	f002 f81c 	bl	f804520 <__SBOOT_Validate_Signature_veneer>
 f8024e8:	4606      	mov	r6, r0
 f8024ea:	2800      	cmp	r0, #0
 f8024ec:	d0a5      	beq.n	f80243a <BOOT_CertificateCheck+0x4e>
 f8024ee:	e7c5      	b.n	f80247c <BOOT_CertificateCheck+0x90>
 f8024f0:	0000c3d1 	.word	0x0000c3d1
 f8024f4:	0000e1b9 	.word	0x0000e1b9
 f8024f8:	3000a938 	.word	0x3000a938
 f8024fc:	41000720 	.word	0x41000720
 f802500:	0000c269 	.word	0x0000c269
 f802504:	00000dd5 	.word	0x00000dd5
 f802508:	0f804f7c 	.word	0x0f804f7c
 f80250c:	0f804988 	.word	0x0f804988
 f802510:	3000a1c0 	.word	0x3000a1c0
 f802514:	3000a800 	.word	0x3000a800

0f802518 <rtk_log_write>:
 f802518:	b408      	push	{r3}
 f80251a:	4b1d      	ldr	r3, [pc, #116]	; (f802590 <rtk_log_write+0x78>)
 f80251c:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 f802520:	681c      	ldr	r4, [r3, #0]
 f802522:	b083      	sub	sp, #12
 f802524:	4680      	mov	r8, r0
 f802526:	460e      	mov	r6, r1
 f802528:	2c03      	cmp	r4, #3
 f80252a:	4691      	mov	r9, r2
 f80252c:	d827      	bhi.n	f80257e <rtk_log_write+0x66>
 f80252e:	681f      	ldr	r7, [r3, #0]
 f802530:	b1a7      	cbz	r7, f80255c <rtk_log_write+0x44>
 f802532:	4d18      	ldr	r5, [pc, #96]	; (f802594 <rtk_log_write+0x7c>)
 f802534:	2400      	movs	r4, #0
 f802536:	f8df a06c 	ldr.w	sl, [pc, #108]	; f8025a4 <rtk_log_write+0x8c>
 f80253a:	e002      	b.n	f802542 <rtk_log_write+0x2a>
 f80253c:	3401      	adds	r4, #1
 f80253e:	42a7      	cmp	r7, r4
 f802540:	d90c      	bls.n	f80255c <rtk_log_write+0x44>
 f802542:	4628      	mov	r0, r5
 f802544:	4631      	mov	r1, r6
 f802546:	350b      	adds	r5, #11
 f802548:	47d0      	blx	sl
 f80254a:	2800      	cmp	r0, #0
 f80254c:	d1f6      	bne.n	f80253c <rtk_log_write+0x24>
 f80254e:	eb04 0284 	add.w	r2, r4, r4, lsl #2
 f802552:	4b11      	ldr	r3, [pc, #68]	; (f802598 <rtk_log_write+0x80>)
 f802554:	eb04 0442 	add.w	r4, r4, r2, lsl #1
 f802558:	5d1b      	ldrb	r3, [r3, r4]
 f80255a:	e001      	b.n	f802560 <rtk_log_write+0x48>
 f80255c:	4b0f      	ldr	r3, [pc, #60]	; (f80259c <rtk_log_write+0x84>)
 f80255e:	781b      	ldrb	r3, [r3, #0]
 f802560:	4598      	cmp	r8, r3
 f802562:	d807      	bhi.n	f802574 <rtk_log_write+0x5c>
 f802564:	7833      	ldrb	r3, [r6, #0]
 f802566:	2b23      	cmp	r3, #35	; 0x23
 f802568:	d10b      	bne.n	f802582 <rtk_log_write+0x6a>
 f80256a:	a90c      	add	r1, sp, #48	; 0x30
 f80256c:	980b      	ldr	r0, [sp, #44]	; 0x2c
 f80256e:	9101      	str	r1, [sp, #4]
 f802570:	f000 f9aa 	bl	f8028c8 <DiagVprintf>
 f802574:	b003      	add	sp, #12
 f802576:	e8bd 47f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 f80257a:	b001      	add	sp, #4
 f80257c:	4770      	bx	lr
 f80257e:	2704      	movs	r7, #4
 f802580:	e7d7      	b.n	f802532 <rtk_log_write+0x1a>
 f802582:	464a      	mov	r2, r9
 f802584:	4631      	mov	r1, r6
 f802586:	4806      	ldr	r0, [pc, #24]	; (f8025a0 <rtk_log_write+0x88>)
 f802588:	f000 fc5e 	bl	f802e48 <DiagPrintf>
 f80258c:	e7ed      	b.n	f80256a <rtk_log_write+0x52>
 f80258e:	bf00      	nop
 f802590:	3000a93c 	.word	0x3000a93c
 f802594:	3000a941 	.word	0x3000a941
 f802598:	3000a940 	.word	0x3000a940
 f80259c:	3000a1ca 	.word	0x3000a1ca
 f8025a0:	0f804fb4 	.word	0x0f804fb4
 f8025a4:	00011e89 	.word	0x00011e89

0f8025a8 <ChipInfo_BDNum>:
 f8025a8:	b500      	push	{lr}
 f8025aa:	b083      	sub	sp, #12
 f8025ac:	f240 70ff 	movw	r0, #2047	; 0x7ff
 f8025b0:	4b18      	ldr	r3, [pc, #96]	; (f802614 <ChipInfo_BDNum+0x6c>)
 f8025b2:	f10d 0107 	add.w	r1, sp, #7
 f8025b6:	4798      	blx	r3
 f8025b8:	f89d e007 	ldrb.w	lr, [sp, #7]
 f8025bc:	f1be 0fff 	cmp.w	lr, #255	; 0xff
 f8025c0:	d022      	beq.n	f802608 <ChipInfo_BDNum+0x60>
 f8025c2:	ea4f 115e 	mov.w	r1, lr, lsr #5
 f8025c6:	4b14      	ldr	r3, [pc, #80]	; (f802618 <ChipInfo_BDNum+0x70>)
 f8025c8:	f00e 0e1f 	and.w	lr, lr, #31
 f8025cc:	2200      	movs	r2, #0
 f8025ce:	f44f 707d 	mov.w	r0, #1012	; 0x3f4
 f8025d2:	f64f 7cff 	movw	ip, #65535	; 0xffff
 f8025d6:	e004      	b.n	f8025e2 <ChipInfo_BDNum+0x3a>
 f8025d8:	8958      	ldrh	r0, [r3, #10]
 f8025da:	4560      	cmp	r0, ip
 f8025dc:	d009      	beq.n	f8025f2 <ChipInfo_BDNum+0x4a>
 f8025de:	f813 2f08 	ldrb.w	r2, [r3, #8]!
 f8025e2:	428a      	cmp	r2, r1
 f8025e4:	d1f8      	bne.n	f8025d8 <ChipInfo_BDNum+0x30>
 f8025e6:	785a      	ldrb	r2, [r3, #1]
 f8025e8:	4572      	cmp	r2, lr
 f8025ea:	d1f5      	bne.n	f8025d8 <ChipInfo_BDNum+0x30>
 f8025ec:	b003      	add	sp, #12
 f8025ee:	f85d fb04 	ldr.w	pc, [sp], #4
 f8025f2:	4b0a      	ldr	r3, [pc, #40]	; (f80261c <ChipInfo_BDNum+0x74>)
 f8025f4:	2245      	movs	r2, #69	; 0x45
 f8025f6:	490a      	ldr	r1, [pc, #40]	; (f802620 <ChipInfo_BDNum+0x78>)
 f8025f8:	2002      	movs	r0, #2
 f8025fa:	f7ff ff8d 	bl	f802518 <rtk_log_write>
 f8025fe:	f240 30f6 	movw	r0, #1014	; 0x3f6
 f802602:	b003      	add	sp, #12
 f802604:	f85d fb04 	ldr.w	pc, [sp], #4
 f802608:	f64f 70ff 	movw	r0, #65535	; 0xffff
 f80260c:	b003      	add	sp, #12
 f80260e:	f85d fb04 	ldr.w	pc, [sp], #4
 f802612:	bf00      	nop
 f802614:	0f80277d 	.word	0x0f80277d
 f802618:	0f804fe0 	.word	0x0f804fe0
 f80261c:	0f804fc0 	.word	0x0f804fc0
 f802620:	0f804fd4 	.word	0x0f804fd4

0f802624 <ChipInfo_MCM_Info>:
 f802624:	b570      	push	{r4, r5, r6, lr}
 f802626:	f64f 76ff 	movw	r6, #65535	; 0xffff
 f80262a:	4b14      	ldr	r3, [pc, #80]	; (f80267c <ChipInfo_MCM_Info+0x58>)
 f80262c:	4798      	blx	r3
 f80262e:	42b0      	cmp	r0, r6
 f802630:	d020      	beq.n	f802674 <ChipInfo_MCM_Info+0x50>
 f802632:	f240 34f6 	movw	r4, #1014	; 0x3f6
 f802636:	1b04      	subs	r4, r0, r4
 f802638:	fab4 f484 	clz	r4, r4
 f80263c:	0964      	lsrs	r4, r4, #5
 f80263e:	b9cc      	cbnz	r4, f802674 <ChipInfo_MCM_Info+0x50>
 f802640:	2301      	movs	r3, #1
 f802642:	f240 32f7 	movw	r2, #1015	; 0x3f7
 f802646:	f44f 7c7d 	mov.w	ip, #1012	; 0x3f4
 f80264a:	4621      	mov	r1, r4
 f80264c:	4d0c      	ldr	r5, [pc, #48]	; (f802680 <ChipInfo_MCM_Info+0x5c>)
 f80264e:	e000      	b.n	f802652 <ChipInfo_MCM_Info+0x2e>
 f802650:	8852      	ldrh	r2, [r2, #2]
 f802652:	4560      	cmp	r0, ip
 f802654:	f103 0e01 	add.w	lr, r3, #1
 f802658:	4694      	mov	ip, r2
 f80265a:	d102      	bne.n	f802662 <ChipInfo_MCM_Info+0x3e>
 f80265c:	eb05 01c1 	add.w	r1, r5, r1, lsl #3
 f802660:	684c      	ldr	r4, [r1, #4]
 f802662:	4619      	mov	r1, r3
 f802664:	42b2      	cmp	r2, r6
 f802666:	fa5f f38e 	uxtb.w	r3, lr
 f80266a:	eb05 02c3 	add.w	r2, r5, r3, lsl #3
 f80266e:	d1ef      	bne.n	f802650 <ChipInfo_MCM_Info+0x2c>
 f802670:	4620      	mov	r0, r4
 f802672:	bd70      	pop	{r4, r5, r6, pc}
 f802674:	f44f 0480 	mov.w	r4, #4194304	; 0x400000
 f802678:	4620      	mov	r0, r4
 f80267a:	bd70      	pop	{r4, r5, r6, pc}
 f80267c:	0f8025a9 	.word	0x0f8025a9
 f802680:	0f804fe0 	.word	0x0f804fe0

0f802684 <print_unsigned_num>:
 f802684:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 f802688:	b089      	sub	sp, #36	; 0x24
 f80268a:	4606      	mov	r6, r0
 f80268c:	460d      	mov	r5, r1
 f80268e:	4690      	mov	r8, r2
 f802690:	f10d 0a0c 	add.w	sl, sp, #12
 f802694:	469b      	mov	fp, r3
 f802696:	2400      	movs	r4, #0
 f802698:	9f13      	ldr	r7, [sp, #76]	; 0x4c
 f80269a:	46d1      	mov	r9, sl
 f80269c:	e00f      	b.n	f8026be <print_unsigned_num+0x3a>
 f80269e:	f10c 0357 	add.w	r3, ip, #87	; 0x57
 f8026a2:	2f01      	cmp	r7, #1
 f8026a4:	b2db      	uxtb	r3, r3
 f8026a6:	d018      	beq.n	f8026da <print_unsigned_num+0x56>
 f8026a8:	4546      	cmp	r6, r8
 f8026aa:	4606      	mov	r6, r0
 f8026ac:	f104 0201 	add.w	r2, r4, #1
 f8026b0:	f809 3b01 	strb.w	r3, [r9], #1
 f8026b4:	f175 0000 	sbcs.w	r0, r5, #0
 f8026b8:	460d      	mov	r5, r1
 f8026ba:	d312      	bcc.n	f8026e2 <print_unsigned_num+0x5e>
 f8026bc:	4614      	mov	r4, r2
 f8026be:	4630      	mov	r0, r6
 f8026c0:	4629      	mov	r1, r5
 f8026c2:	4642      	mov	r2, r8
 f8026c4:	2300      	movs	r3, #0
 f8026c6:	f001 fb17 	bl	f803cf8 <__aeabi_uldivmod>
 f8026ca:	2a09      	cmp	r2, #9
 f8026cc:	fa5f fc82 	uxtb.w	ip, r2
 f8026d0:	f10c 0330 	add.w	r3, ip, #48	; 0x30
 f8026d4:	d8e3      	bhi.n	f80269e <print_unsigned_num+0x1a>
 f8026d6:	b2db      	uxtb	r3, r3
 f8026d8:	e7e6      	b.n	f8026a8 <print_unsigned_num+0x24>
 f8026da:	f10c 0337 	add.w	r3, ip, #55	; 0x37
 f8026de:	b2db      	uxtb	r3, r3
 f8026e0:	e7e2      	b.n	f8026a8 <print_unsigned_num+0x24>
 f8026e2:	4617      	mov	r7, r2
 f8026e4:	9a14      	ldr	r2, [sp, #80]	; 0x50
 f8026e6:	b192      	cbz	r2, f80270e <print_unsigned_num+0x8a>
 f8026e8:	9a12      	ldr	r2, [sp, #72]	; 0x48
 f8026ea:	f1bb 0f30 	cmp.w	fp, #48	; 0x30
 f8026ee:	f102 32ff 	add.w	r2, r2, #4294967295	; 0xffffffff
 f8026f2:	9212      	str	r2, [sp, #72]	; 0x48
 f8026f4:	d136      	bne.n	f802764 <print_unsigned_num+0xe0>
 f8026f6:	202d      	movs	r0, #45	; 0x2d
 f8026f8:	4d1f      	ldr	r5, [pc, #124]	; (f802778 <print_unsigned_num+0xf4>)
 f8026fa:	9301      	str	r3, [sp, #4]
 f8026fc:	47a8      	blx	r5
 f8026fe:	9b12      	ldr	r3, [sp, #72]	; 0x48
 f802700:	2b00      	cmp	r3, #0
 f802702:	9b01      	ldr	r3, [sp, #4]
 f802704:	dd1f      	ble.n	f802746 <print_unsigned_num+0xc2>
 f802706:	9a12      	ldr	r2, [sp, #72]	; 0x48
 f802708:	4297      	cmp	r7, r2
 f80270a:	db08      	blt.n	f80271e <print_unsigned_num+0x9a>
 f80270c:	e018      	b.n	f802740 <print_unsigned_num+0xbc>
 f80270e:	9a12      	ldr	r2, [sp, #72]	; 0x48
 f802710:	2a00      	cmp	r2, #0
 f802712:	dd25      	ble.n	f802760 <print_unsigned_num+0xdc>
 f802714:	9912      	ldr	r1, [sp, #72]	; 0x48
 f802716:	9a14      	ldr	r2, [sp, #80]	; 0x50
 f802718:	42b9      	cmp	r1, r7
 f80271a:	4d17      	ldr	r5, [pc, #92]	; (f802778 <print_unsigned_num+0xf4>)
 f80271c:	dd0d      	ble.n	f80273a <print_unsigned_num+0xb6>
 f80271e:	9e12      	ldr	r6, [sp, #72]	; 0x48
 f802720:	4698      	mov	r8, r3
 f802722:	3e01      	subs	r6, #1
 f802724:	4658      	mov	r0, fp
 f802726:	47a8      	blx	r5
 f802728:	42b7      	cmp	r7, r6
 f80272a:	d1fa      	bne.n	f802722 <print_unsigned_num+0x9e>
 f80272c:	9912      	ldr	r1, [sp, #72]	; 0x48
 f80272e:	43fa      	mvns	r2, r7
 f802730:	4643      	mov	r3, r8
 f802732:	440a      	add	r2, r1
 f802734:	9914      	ldr	r1, [sp, #80]	; 0x50
 f802736:	3101      	adds	r1, #1
 f802738:	440a      	add	r2, r1
 f80273a:	9914      	ldr	r1, [sp, #80]	; 0x50
 f80273c:	9214      	str	r2, [sp, #80]	; 0x50
 f80273e:	b111      	cbz	r1, f802746 <print_unsigned_num+0xc2>
 f802740:	f01b 0fdf 	tst.w	fp, #223	; 0xdf
 f802744:	d013      	beq.n	f80276e <print_unsigned_num+0xea>
 f802746:	4454      	add	r4, sl
 f802748:	4618      	mov	r0, r3
 f80274a:	e001      	b.n	f802750 <print_unsigned_num+0xcc>
 f80274c:	f814 0d01 	ldrb.w	r0, [r4, #-1]!
 f802750:	47a8      	blx	r5
 f802752:	45a2      	cmp	sl, r4
 f802754:	d1fa      	bne.n	f80274c <print_unsigned_num+0xc8>
 f802756:	9b14      	ldr	r3, [sp, #80]	; 0x50
 f802758:	19d8      	adds	r0, r3, r7
 f80275a:	b009      	add	sp, #36	; 0x24
 f80275c:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 f802760:	4d05      	ldr	r5, [pc, #20]	; (f802778 <print_unsigned_num+0xf4>)
 f802762:	e7f0      	b.n	f802746 <print_unsigned_num+0xc2>
 f802764:	9a12      	ldr	r2, [sp, #72]	; 0x48
 f802766:	2a00      	cmp	r2, #0
 f802768:	dcd4      	bgt.n	f802714 <print_unsigned_num+0x90>
 f80276a:	4d03      	ldr	r5, [pc, #12]	; (f802778 <print_unsigned_num+0xf4>)
 f80276c:	e7e8      	b.n	f802740 <print_unsigned_num+0xbc>
 f80276e:	202d      	movs	r0, #45	; 0x2d
 f802770:	9301      	str	r3, [sp, #4]
 f802772:	47a8      	blx	r5
 f802774:	9b01      	ldr	r3, [sp, #4]
 f802776:	e7e6      	b.n	f802746 <print_unsigned_num+0xc2>
 f802778:	0000aba1 	.word	0x0000aba1

0f80277c <OTP_Read8>:
 f80277c:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 f802780:	f242 7410 	movw	r4, #10000	; 0x2710
 f802784:	4d10      	ldr	r5, [pc, #64]	; (f8027c8 <OTP_Read8+0x4c>)
 f802786:	4607      	mov	r7, r0
 f802788:	4688      	mov	r8, r1
 f80278a:	4e10      	ldr	r6, [pc, #64]	; (f8027cc <OTP_Read8+0x50>)
 f80278c:	f8d5 3204 	ldr.w	r3, [r5, #516]	; 0x204
 f802790:	2001      	movs	r0, #1
 f802792:	b16b      	cbz	r3, f8027b0 <OTP_Read8+0x34>
 f802794:	6833      	ldr	r3, [r6, #0]
 f802796:	b103      	cbz	r3, f80279a <OTP_Read8+0x1e>
 f802798:	4798      	blx	r3
 f80279a:	3c01      	subs	r4, #1
 f80279c:	d1f6      	bne.n	f80278c <OTP_Read8+0x10>
 f80279e:	4b0c      	ldr	r3, [pc, #48]	; (f8027d0 <OTP_Read8+0x54>)
 f8027a0:	2245      	movs	r2, #69	; 0x45
 f8027a2:	490c      	ldr	r1, [pc, #48]	; (f8027d4 <OTP_Read8+0x58>)
 f8027a4:	2002      	movs	r0, #2
 f8027a6:	f7ff feb7 	bl	f802518 <rtk_log_write>
 f8027aa:	4620      	mov	r0, r4
 f8027ac:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 f8027b0:	4641      	mov	r1, r8
 f8027b2:	4b09      	ldr	r3, [pc, #36]	; (f8027d8 <OTP_Read8+0x5c>)
 f8027b4:	4638      	mov	r0, r7
 f8027b6:	4798      	blx	r3
 f8027b8:	4604      	mov	r4, r0
 f8027ba:	2001      	movs	r0, #1
 f8027bc:	f001 fe94 	bl	f8044e8 <__IPC_SEMFree_veneer>
 f8027c0:	4620      	mov	r0, r4
 f8027c2:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 f8027c6:	bf00      	nop
 f8027c8:	41014000 	.word	0x41014000
 f8027cc:	3000a96c 	.word	0x3000a96c
 f8027d0:	0f805090 	.word	0x0f805090
 f8027d4:	0f8050a8 	.word	0x0f8050a8
 f8027d8:	0000b275 	.word	0x0000b275

0f8027dc <OTP_Read32>:
 f8027dc:	2300      	movs	r3, #0
 f8027de:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 f8027e2:	4d1d      	ldr	r5, [pc, #116]	; (f802858 <OTP_Read32+0x7c>)
 f8027e4:	b082      	sub	sp, #8
 f8027e6:	4680      	mov	r8, r0
 f8027e8:	461f      	mov	r7, r3
 f8027ea:	4689      	mov	r9, r1
 f8027ec:	4e1b      	ldr	r6, [pc, #108]	; (f80285c <OTP_Read32+0x80>)
 f8027ee:	600b      	str	r3, [r1, #0]
 f8027f0:	46c2      	mov	sl, r8
 f8027f2:	f242 7410 	movw	r4, #10000	; 0x2710
 f8027f6:	f108 0801 	add.w	r8, r8, #1
 f8027fa:	f8d5 3204 	ldr.w	r3, [r5, #516]	; 0x204
 f8027fe:	2001      	movs	r0, #1
 f802800:	b173      	cbz	r3, f802820 <OTP_Read32+0x44>
 f802802:	6833      	ldr	r3, [r6, #0]
 f802804:	b103      	cbz	r3, f802808 <OTP_Read32+0x2c>
 f802806:	4798      	blx	r3
 f802808:	3c01      	subs	r4, #1
 f80280a:	d1f6      	bne.n	f8027fa <OTP_Read32+0x1e>
 f80280c:	4b14      	ldr	r3, [pc, #80]	; (f802860 <OTP_Read32+0x84>)
 f80280e:	2245      	movs	r2, #69	; 0x45
 f802810:	4914      	ldr	r1, [pc, #80]	; (f802864 <OTP_Read32+0x88>)
 f802812:	2002      	movs	r0, #2
 f802814:	f7ff fe80 	bl	f802518 <rtk_log_write>
 f802818:	4620      	mov	r0, r4
 f80281a:	b002      	add	sp, #8
 f80281c:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
 f802820:	4650      	mov	r0, sl
 f802822:	f10d 0107 	add.w	r1, sp, #7
 f802826:	4b10      	ldr	r3, [pc, #64]	; (f802868 <OTP_Read32+0x8c>)
 f802828:	4798      	blx	r3
 f80282a:	4604      	mov	r4, r0
 f80282c:	2001      	movs	r0, #1
 f80282e:	f001 fe5b 	bl	f8044e8 <__IPC_SEMFree_veneer>
 f802832:	2c00      	cmp	r4, #0
 f802834:	d0f0      	beq.n	f802818 <OTP_Read32+0x3c>
 f802836:	f89d 2007 	ldrb.w	r2, [sp, #7]
 f80283a:	f8d9 3000 	ldr.w	r3, [r9]
 f80283e:	40ba      	lsls	r2, r7
 f802840:	3708      	adds	r7, #8
 f802842:	4313      	orrs	r3, r2
 f802844:	2f20      	cmp	r7, #32
 f802846:	f8c9 3000 	str.w	r3, [r9]
 f80284a:	d1d1      	bne.n	f8027f0 <OTP_Read32+0x14>
 f80284c:	2401      	movs	r4, #1
 f80284e:	4620      	mov	r0, r4
 f802850:	b002      	add	sp, #8
 f802852:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
 f802856:	bf00      	nop
 f802858:	41014000 	.word	0x41014000
 f80285c:	3000a96c 	.word	0x3000a96c
 f802860:	0f805090 	.word	0x0f805090
 f802864:	0f8050a8 	.word	0x0f8050a8
 f802868:	0000b275 	.word	0x0000b275

0f80286c <OTP_LogicalMap_CHK>:
 f80286c:	b530      	push	{r4, r5, lr}
 f80286e:	2400      	movs	r4, #0
 f802870:	b083      	sub	sp, #12
 f802872:	4d14      	ldr	r5, [pc, #80]	; (f8028c4 <OTP_LogicalMap_CHK+0x58>)
 f802874:	e00e      	b.n	f802894 <OTP_LogicalMap_CHK+0x28>
 f802876:	f3c3 6203 	ubfx	r2, r3, #24, #4
 f80287a:	2802      	cmp	r0, #2
 f80287c:	f3c3 3303 	ubfx	r3, r3, #12, #4
 f802880:	ea4f 0282 	mov.w	r2, r2, lsl #2
 f802884:	d103      	bne.n	f80288e <OTP_LogicalMap_CHK+0x22>
 f802886:	3204      	adds	r2, #4
 f802888:	2b05      	cmp	r3, #5
 f80288a:	4414      	add	r4, r2
 f80288c:	d016      	beq.n	f8028bc <OTP_LogicalMap_CHK+0x50>
 f80288e:	f5b4 7ffe 	cmp.w	r4, #508	; 0x1fc
 f802892:	d810      	bhi.n	f8028b6 <OTP_LogicalMap_CHK+0x4a>
 f802894:	4620      	mov	r0, r4
 f802896:	a901      	add	r1, sp, #4
 f802898:	47a8      	blx	r5
 f80289a:	9b01      	ldr	r3, [sp, #4]
 f80289c:	3404      	adds	r4, #4
 f80289e:	1c5a      	adds	r2, r3, #1
 f8028a0:	ea4f 7013 	mov.w	r0, r3, lsr #28
 f8028a4:	d007      	beq.n	f8028b6 <OTP_LogicalMap_CHK+0x4a>
 f8028a6:	2801      	cmp	r0, #1
 f8028a8:	d1e5      	bne.n	f802876 <OTP_LogicalMap_CHK+0xa>
 f8028aa:	f3c3 3303 	ubfx	r3, r3, #12, #4
 f8028ae:	2b05      	cmp	r3, #5
 f8028b0:	d1ed      	bne.n	f80288e <OTP_LogicalMap_CHK+0x22>
 f8028b2:	b003      	add	sp, #12
 f8028b4:	bd30      	pop	{r4, r5, pc}
 f8028b6:	2000      	movs	r0, #0
 f8028b8:	b003      	add	sp, #12
 f8028ba:	bd30      	pop	{r4, r5, pc}
 f8028bc:	2001      	movs	r0, #1
 f8028be:	b003      	add	sp, #12
 f8028c0:	bd30      	pop	{r4, r5, pc}
 f8028c2:	bf00      	nop
 f8028c4:	0f8027dd 	.word	0x0f8027dd

0f8028c8 <DiagVprintf>:
 f8028c8:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 f8028cc:	4604      	mov	r4, r0
 f8028ce:	7800      	ldrb	r0, [r0, #0]
 f8028d0:	b089      	sub	sp, #36	; 0x24
 f8028d2:	2800      	cmp	r0, #0
 f8028d4:	f000 82ab 	beq.w	f802e2e <DiagVprintf+0x566>
 f8028d8:	f04f 0a00 	mov.w	sl, #0
 f8028dc:	4688      	mov	r8, r1
 f8028de:	4f89      	ldr	r7, [pc, #548]	; (f802b04 <DiagVprintf+0x23c>)
 f8028e0:	4655      	mov	r5, sl
 f8028e2:	f8cd a010 	str.w	sl, [sp, #16]
 f8028e6:	e005      	b.n	f8028f4 <DiagVprintf+0x2c>
 f8028e8:	3501      	adds	r5, #1
 f8028ea:	47b8      	blx	r7
 f8028ec:	7820      	ldrb	r0, [r4, #0]
 f8028ee:	2800      	cmp	r0, #0
 f8028f0:	f000 808b 	beq.w	f802a0a <DiagVprintf+0x142>
 f8028f4:	2825      	cmp	r0, #37	; 0x25
 f8028f6:	f104 0401 	add.w	r4, r4, #1
 f8028fa:	d1f5      	bne.n	f8028e8 <DiagVprintf+0x20>
 f8028fc:	f04f 0a00 	mov.w	sl, #0
 f802900:	4656      	mov	r6, sl
 f802902:	46d1      	mov	r9, sl
 f802904:	46d4      	mov	ip, sl
 f802906:	7823      	ldrb	r3, [r4, #0]
 f802908:	2b7a      	cmp	r3, #122	; 0x7a
 f80290a:	d87c      	bhi.n	f802a06 <DiagVprintf+0x13e>
 f80290c:	e8df f013 	tbh	[pc, r3, lsl #1]
 f802910:	007b00af 	.word	0x007b00af
 f802914:	007b007b 	.word	0x007b007b
 f802918:	007b007b 	.word	0x007b007b
 f80291c:	007b007b 	.word	0x007b007b
 f802920:	007b007b 	.word	0x007b007b
 f802924:	007b007b 	.word	0x007b007b
 f802928:	007b007b 	.word	0x007b007b
 f80292c:	007b007b 	.word	0x007b007b
 f802930:	007b007b 	.word	0x007b007b
 f802934:	007b007b 	.word	0x007b007b
 f802938:	007b007b 	.word	0x007b007b
 f80293c:	007b007b 	.word	0x007b007b
 f802940:	007b007b 	.word	0x007b007b
 f802944:	007b007b 	.word	0x007b007b
 f802948:	007b007b 	.word	0x007b007b
 f80294c:	007b007b 	.word	0x007b007b
 f802950:	007b00c4 	.word	0x007b00c4
 f802954:	007b007b 	.word	0x007b007b
 f802958:	01af007b 	.word	0x01af007b
 f80295c:	007b007b 	.word	0x007b007b
 f802960:	007b007b 	.word	0x007b007b
 f802964:	007b007b 	.word	0x007b007b
 f802968:	01ab007b 	.word	0x01ab007b
 f80296c:	007b007b 	.word	0x007b007b
 f802970:	00b100c4 	.word	0x00b100c4
 f802974:	00b100b1 	.word	0x00b100b1
 f802978:	00b100b1 	.word	0x00b100b1
 f80297c:	00b100b1 	.word	0x00b100b1
 f802980:	00b100b1 	.word	0x00b100b1
 f802984:	007b007b 	.word	0x007b007b
 f802988:	007b007b 	.word	0x007b007b
 f80298c:	007b007b 	.word	0x007b007b
 f802990:	007b007b 	.word	0x007b007b
 f802994:	012c014f 	.word	0x012c014f
 f802998:	007b007b 	.word	0x007b007b
 f80299c:	007b007b 	.word	0x007b007b
 f8029a0:	007b007b 	.word	0x007b007b
 f8029a4:	007b007b 	.word	0x007b007b
 f8029a8:	007b0128 	.word	0x007b0128
 f8029ac:	00fc007b 	.word	0x00fc007b
 f8029b0:	007b007b 	.word	0x007b007b
 f8029b4:	007b007b 	.word	0x007b007b
 f8029b8:	007b007b 	.word	0x007b007b
 f8029bc:	007b007b 	.word	0x007b007b
 f8029c0:	007b0081 	.word	0x007b0081
 f8029c4:	007b007b 	.word	0x007b007b
 f8029c8:	007b007b 	.word	0x007b007b
 f8029cc:	007b007b 	.word	0x007b007b
 f8029d0:	007b007b 	.word	0x007b007b
 f8029d4:	012c014f 	.word	0x012c014f
 f8029d8:	007b00ca 	.word	0x007b00ca
 f8029dc:	007b007b 	.word	0x007b007b
 f8029e0:	00ca007b 	.word	0x00ca007b
 f8029e4:	007b007b 	.word	0x007b007b
 f8029e8:	007b0128 	.word	0x007b0128
 f8029ec:	00fc007b 	.word	0x00fc007b
 f8029f0:	007b01e6 	.word	0x007b01e6
 f8029f4:	01b4007b 	.word	0x01b4007b
 f8029f8:	017d007b 	.word	0x017d007b
 f8029fc:	007b007b 	.word	0x007b007b
 f802a00:	007b0083 	.word	0x007b0083
 f802a04:	012a      	.short	0x012a
 f802a06:	f04f 35ff 	mov.w	r5, #4294967295	; 0xffffffff
 f802a0a:	4628      	mov	r0, r5
 f802a0c:	b009      	add	sp, #36	; 0x24
 f802a0e:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 f802a12:	2301      	movs	r3, #1
 f802a14:	9304      	str	r3, [sp, #16]
 f802a16:	f1bc 0f01 	cmp.w	ip, #1
 f802a1a:	f340 81d4 	ble.w	f802dc6 <DiagVprintf+0x4fe>
 f802a1e:	f108 0807 	add.w	r8, r8, #7
 f802a22:	f028 0807 	bic.w	r8, r8, #7
 f802a26:	f8d8 1004 	ldr.w	r1, [r8, #4]
 f802a2a:	f858 0b08 	ldr.w	r0, [r8], #8
 f802a2e:	f1ba 0f00 	cmp.w	sl, #0
 f802a32:	f000 8168 	beq.w	f802d06 <DiagVprintf+0x43e>
 f802a36:	2300      	movs	r3, #0
 f802a38:	2210      	movs	r2, #16
 f802a3a:	9302      	str	r3, [sp, #8]
 f802a3c:	9b04      	ldr	r3, [sp, #16]
 f802a3e:	9301      	str	r3, [sp, #4]
 f802a40:	2300      	movs	r3, #0
 f802a42:	9300      	str	r3, [sp, #0]
 f802a44:	464b      	mov	r3, r9
 f802a46:	f7ff fe1d 	bl	f802684 <print_unsigned_num>
 f802a4a:	4286      	cmp	r6, r0
 f802a4c:	dd0e      	ble.n	f802a6c <DiagVprintf+0x1a4>
 f802a4e:	f1b9 0f20 	cmp.w	r9, #32
 f802a52:	d10b      	bne.n	f802a6c <DiagVprintf+0x1a4>
 f802a54:	eba6 0900 	sub.w	r9, r6, r0
 f802a58:	f1b9 0f00 	cmp.w	r9, #0
 f802a5c:	f340 81e4 	ble.w	f802e28 <DiagVprintf+0x560>
 f802a60:	2020      	movs	r0, #32
 f802a62:	47b8      	blx	r7
 f802a64:	f1b9 0901 	subs.w	r9, r9, #1
 f802a68:	d1fa      	bne.n	f802a60 <DiagVprintf+0x198>
 f802a6a:	4630      	mov	r0, r6
 f802a6c:	4405      	add	r5, r0
 f802a6e:	3401      	adds	r4, #1
 f802a70:	e73c      	b.n	f8028ec <DiagVprintf+0x24>
 f802a72:	f04f 0920 	mov.w	r9, #32
 f802a76:	2600      	movs	r6, #0
 f802a78:	e005      	b.n	f802a86 <DiagVprintf+0x1be>
 f802a7a:	f814 3f01 	ldrb.w	r3, [r4, #1]!
 f802a7e:	f1a2 0630 	sub.w	r6, r2, #48	; 0x30
 f802a82:	2b00      	cmp	r3, #0
 f802a84:	d0c1      	beq.n	f802a0a <DiagVprintf+0x142>
 f802a86:	f1a3 0130 	sub.w	r1, r3, #48	; 0x30
 f802a8a:	eb06 0286 	add.w	r2, r6, r6, lsl #2
 f802a8e:	2909      	cmp	r1, #9
 f802a90:	eb03 0242 	add.w	r2, r3, r2, lsl #1
 f802a94:	d9f1      	bls.n	f802a7a <DiagVprintf+0x1b2>
 f802a96:	e736      	b.n	f802906 <DiagVprintf+0x3e>
 f802a98:	3401      	adds	r4, #1
 f802a9a:	4699      	mov	r9, r3
 f802a9c:	7823      	ldrb	r3, [r4, #0]
 f802a9e:	2b00      	cmp	r3, #0
 f802aa0:	d1e9      	bne.n	f802a76 <DiagVprintf+0x1ae>
 f802aa2:	e7b2      	b.n	f802a0a <DiagVprintf+0x142>
 f802aa4:	f1bc 0f01 	cmp.w	ip, #1
 f802aa8:	f340 8186 	ble.w	f802db8 <DiagVprintf+0x4f0>
 f802aac:	f108 0807 	add.w	r8, r8, #7
 f802ab0:	f028 0807 	bic.w	r8, r8, #7
 f802ab4:	f8d8 1004 	ldr.w	r1, [r8, #4]
 f802ab8:	f858 0b08 	ldr.w	r0, [r8], #8
 f802abc:	2900      	cmp	r1, #0
 f802abe:	f2c0 8189 	blt.w	f802dd4 <DiagVprintf+0x50c>
 f802ac2:	2200      	movs	r2, #0
 f802ac4:	9202      	str	r2, [sp, #8]
 f802ac6:	f1ba 0f00 	cmp.w	sl, #0
 f802aca:	f000 816c 	beq.w	f802da6 <DiagVprintf+0x4de>
 f802ace:	9b04      	ldr	r3, [sp, #16]
 f802ad0:	220a      	movs	r2, #10
 f802ad2:	9301      	str	r3, [sp, #4]
 f802ad4:	2300      	movs	r3, #0
 f802ad6:	9300      	str	r3, [sp, #0]
 f802ad8:	464b      	mov	r3, r9
 f802ada:	f7ff fdd3 	bl	f802684 <print_unsigned_num>
 f802ade:	4286      	cmp	r6, r0
 f802ae0:	ddc4      	ble.n	f802a6c <DiagVprintf+0x1a4>
 f802ae2:	f1b9 0f20 	cmp.w	r9, #32
 f802ae6:	d1c1      	bne.n	f802a6c <DiagVprintf+0x1a4>
 f802ae8:	eba6 0900 	sub.w	r9, r6, r0
 f802aec:	f1b9 0f00 	cmp.w	r9, #0
 f802af0:	f340 819a 	ble.w	f802e28 <DiagVprintf+0x560>
 f802af4:	2020      	movs	r0, #32
 f802af6:	47b8      	blx	r7
 f802af8:	f1b9 0901 	subs.w	r9, r9, #1
 f802afc:	d1fa      	bne.n	f802af4 <DiagVprintf+0x22c>
 f802afe:	4630      	mov	r0, r6
 f802b00:	e7b4      	b.n	f802a6c <DiagVprintf+0x1a4>
 f802b02:	bf00      	nop
 f802b04:	0000aba1 	.word	0x0000aba1
 f802b08:	f1bc 0f01 	cmp.w	ip, #1
 f802b0c:	f340 811e 	ble.w	f802d4c <DiagVprintf+0x484>
 f802b10:	f108 0807 	add.w	r8, r8, #7
 f802b14:	f028 0807 	bic.w	r8, r8, #7
 f802b18:	f8d8 1004 	ldr.w	r1, [r8, #4]
 f802b1c:	f858 0b08 	ldr.w	r0, [r8], #8
 f802b20:	f1ba 0f00 	cmp.w	sl, #0
 f802b24:	f000 8119 	beq.w	f802d5a <DiagVprintf+0x492>
 f802b28:	2300      	movs	r3, #0
 f802b2a:	2208      	movs	r2, #8
 f802b2c:	9302      	str	r3, [sp, #8]
 f802b2e:	9b04      	ldr	r3, [sp, #16]
 f802b30:	9301      	str	r3, [sp, #4]
 f802b32:	2300      	movs	r3, #0
 f802b34:	9300      	str	r3, [sp, #0]
 f802b36:	464b      	mov	r3, r9
 f802b38:	f7ff fda4 	bl	f802684 <print_unsigned_num>
 f802b3c:	4286      	cmp	r6, r0
 f802b3e:	dd95      	ble.n	f802a6c <DiagVprintf+0x1a4>
 f802b40:	f1b9 0f20 	cmp.w	r9, #32
 f802b44:	d192      	bne.n	f802a6c <DiagVprintf+0x1a4>
 f802b46:	eba6 0900 	sub.w	r9, r6, r0
 f802b4a:	f1b9 0f00 	cmp.w	r9, #0
 f802b4e:	f340 816b 	ble.w	f802e28 <DiagVprintf+0x560>
 f802b52:	2020      	movs	r0, #32
 f802b54:	47b8      	blx	r7
 f802b56:	f1b9 0901 	subs.w	r9, r9, #1
 f802b5a:	d1fa      	bne.n	f802b52 <DiagVprintf+0x28a>
 f802b5c:	4630      	mov	r0, r6
 f802b5e:	e785      	b.n	f802a6c <DiagVprintf+0x1a4>
 f802b60:	f10c 0c01 	add.w	ip, ip, #1
 f802b64:	3401      	adds	r4, #1
 f802b66:	e6ce      	b.n	f802906 <DiagVprintf+0x3e>
 f802b68:	2e01      	cmp	r6, #1
 f802b6a:	f858 1b04 	ldr.w	r1, [r8], #4
 f802b6e:	bfd4      	ite	le
 f802b70:	2200      	movle	r2, #0
 f802b72:	2201      	movgt	r2, #1
 f802b74:	fa5f fb81 	uxtb.w	fp, r1
 f802b78:	f1ba 0f00 	cmp.w	sl, #0
 f802b7c:	d102      	bne.n	f802b84 <DiagVprintf+0x2bc>
 f802b7e:	2a00      	cmp	r2, #0
 f802b80:	f040 812e 	bne.w	f802de0 <DiagVprintf+0x518>
 f802b84:	4658      	mov	r0, fp
 f802b86:	9205      	str	r2, [sp, #20]
 f802b88:	47b8      	blx	r7
 f802b8a:	1c69      	adds	r1, r5, #1
 f802b8c:	f1ba 0f00 	cmp.w	sl, #0
 f802b90:	f000 811d 	beq.w	f802dce <DiagVprintf+0x506>
 f802b94:	9a05      	ldr	r2, [sp, #20]
 f802b96:	2a00      	cmp	r2, #0
 f802b98:	f000 8119 	beq.w	f802dce <DiagVprintf+0x506>
 f802b9c:	f106 3bff 	add.w	fp, r6, #4294967295	; 0xffffffff
 f802ba0:	4648      	mov	r0, r9
 f802ba2:	47b8      	blx	r7
 f802ba4:	f1bb 0b01 	subs.w	fp, fp, #1
 f802ba8:	d1fa      	bne.n	f802ba0 <DiagVprintf+0x2d8>
 f802baa:	4435      	add	r5, r6
 f802bac:	e75f      	b.n	f802a6e <DiagVprintf+0x1a6>
 f802bae:	f1bc 0f01 	cmp.w	ip, #1
 f802bb2:	f340 80dd 	ble.w	f802d70 <DiagVprintf+0x4a8>
 f802bb6:	f108 0807 	add.w	r8, r8, #7
 f802bba:	f028 0807 	bic.w	r8, r8, #7
 f802bbe:	f8d8 1004 	ldr.w	r1, [r8, #4]
 f802bc2:	f858 0b08 	ldr.w	r0, [r8], #8
 f802bc6:	f1ba 0f00 	cmp.w	sl, #0
 f802bca:	f000 80d8 	beq.w	f802d7e <DiagVprintf+0x4b6>
 f802bce:	2300      	movs	r3, #0
 f802bd0:	2202      	movs	r2, #2
 f802bd2:	9302      	str	r3, [sp, #8]
 f802bd4:	9b04      	ldr	r3, [sp, #16]
 f802bd6:	9301      	str	r3, [sp, #4]
 f802bd8:	2300      	movs	r3, #0
 f802bda:	9300      	str	r3, [sp, #0]
 f802bdc:	464b      	mov	r3, r9
 f802bde:	f7ff fd51 	bl	f802684 <print_unsigned_num>
 f802be2:	4286      	cmp	r6, r0
 f802be4:	f77f af42 	ble.w	f802a6c <DiagVprintf+0x1a4>
 f802be8:	f1b9 0f20 	cmp.w	r9, #32
 f802bec:	f47f af3e 	bne.w	f802a6c <DiagVprintf+0x1a4>
 f802bf0:	eba6 0900 	sub.w	r9, r6, r0
 f802bf4:	f1b9 0f00 	cmp.w	r9, #0
 f802bf8:	f340 8116 	ble.w	f802e28 <DiagVprintf+0x560>
 f802bfc:	2020      	movs	r0, #32
 f802bfe:	47b8      	blx	r7
 f802c00:	f1b9 0901 	subs.w	r9, r9, #1
 f802c04:	d1fa      	bne.n	f802bfc <DiagVprintf+0x334>
 f802c06:	4630      	mov	r0, r6
 f802c08:	e730      	b.n	f802a6c <DiagVprintf+0x1a4>
 f802c0a:	f1bc 0f01 	cmp.w	ip, #1
 f802c0e:	f340 80c1 	ble.w	f802d94 <DiagVprintf+0x4cc>
 f802c12:	f108 0807 	add.w	r8, r8, #7
 f802c16:	f028 0807 	bic.w	r8, r8, #7
 f802c1a:	f8d8 1004 	ldr.w	r1, [r8, #4]
 f802c1e:	f858 0b08 	ldr.w	r0, [r8], #8
 f802c22:	f1ba 0f00 	cmp.w	sl, #0
 f802c26:	f000 80bc 	beq.w	f802da2 <DiagVprintf+0x4da>
 f802c2a:	2300      	movs	r3, #0
 f802c2c:	220a      	movs	r2, #10
 f802c2e:	9302      	str	r3, [sp, #8]
 f802c30:	9b04      	ldr	r3, [sp, #16]
 f802c32:	9301      	str	r3, [sp, #4]
 f802c34:	2300      	movs	r3, #0
 f802c36:	9300      	str	r3, [sp, #0]
 f802c38:	464b      	mov	r3, r9
 f802c3a:	f7ff fd23 	bl	f802684 <print_unsigned_num>
 f802c3e:	4286      	cmp	r6, r0
 f802c40:	f77f af14 	ble.w	f802a6c <DiagVprintf+0x1a4>
 f802c44:	f1b9 0f20 	cmp.w	r9, #32
 f802c48:	f47f af10 	bne.w	f802a6c <DiagVprintf+0x1a4>
 f802c4c:	eba6 0900 	sub.w	r9, r6, r0
 f802c50:	f1b9 0f00 	cmp.w	r9, #0
 f802c54:	f340 80e8 	ble.w	f802e28 <DiagVprintf+0x560>
 f802c58:	2020      	movs	r0, #32
 f802c5a:	47b8      	blx	r7
 f802c5c:	f1b9 0901 	subs.w	r9, r9, #1
 f802c60:	d1fa      	bne.n	f802c58 <DiagVprintf+0x390>
 f802c62:	4630      	mov	r0, r6
 f802c64:	e702      	b.n	f802a6c <DiagVprintf+0x1a4>
 f802c66:	3401      	adds	r4, #1
 f802c68:	f10a 0a01 	add.w	sl, sl, #1
 f802c6c:	e64b      	b.n	f802906 <DiagVprintf+0x3e>
 f802c6e:	2025      	movs	r0, #37	; 0x25
 f802c70:	3501      	adds	r5, #1
 f802c72:	3401      	adds	r4, #1
 f802c74:	47b8      	blx	r7
 f802c76:	e639      	b.n	f8028ec <DiagVprintf+0x24>
 f802c78:	4643      	mov	r3, r8
 f802c7a:	f853 2b04 	ldr.w	r2, [r3], #4
 f802c7e:	7810      	ldrb	r0, [r2, #0]
 f802c80:	9305      	str	r3, [sp, #20]
 f802c82:	2800      	cmp	r0, #0
 f802c84:	f000 80d5 	beq.w	f802e32 <DiagVprintf+0x56a>
 f802c88:	4694      	mov	ip, r2
 f802c8a:	f1c2 0801 	rsb	r8, r2, #1
 f802c8e:	eb08 010c 	add.w	r1, r8, ip
 f802c92:	f81c 3f01 	ldrb.w	r3, [ip, #1]!
 f802c96:	2b00      	cmp	r3, #0
 f802c98:	d1f9      	bne.n	f802c8e <DiagVprintf+0x3c6>
 f802c9a:	9106      	str	r1, [sp, #24]
 f802c9c:	f1ba 0f00 	cmp.w	sl, #0
 f802ca0:	d03c      	beq.n	f802d1c <DiagVprintf+0x454>
 f802ca2:	2a00      	cmp	r2, #0
 f802ca4:	f000 80cb 	beq.w	f802e3e <DiagVprintf+0x576>
 f802ca8:	b168      	cbz	r0, f802cc6 <DiagVprintf+0x3fe>
 f802caa:	4693      	mov	fp, r2
 f802cac:	f1c2 0801 	rsb	r8, r2, #1
 f802cb0:	47b8      	blx	r7
 f802cb2:	eb0b 0c08 	add.w	ip, fp, r8
 f802cb6:	f81b 0f01 	ldrb.w	r0, [fp, #1]!
 f802cba:	2800      	cmp	r0, #0
 f802cbc:	d1f8      	bne.n	f802cb0 <DiagVprintf+0x3e8>
 f802cbe:	4465      	add	r5, ip
 f802cc0:	f1ba 0f00 	cmp.w	sl, #0
 f802cc4:	d006      	beq.n	f802cd4 <DiagVprintf+0x40c>
 f802cc6:	9b06      	ldr	r3, [sp, #24]
 f802cc8:	eba6 0b03 	sub.w	fp, r6, r3
 f802ccc:	f1bb 0f00 	cmp.w	fp, #0
 f802cd0:	f300 80a1 	bgt.w	f802e16 <DiagVprintf+0x54e>
 f802cd4:	f8dd 8014 	ldr.w	r8, [sp, #20]
 f802cd8:	3401      	adds	r4, #1
 f802cda:	e607      	b.n	f8028ec <DiagVprintf+0x24>
 f802cdc:	f858 0b04 	ldr.w	r0, [r8], #4
 f802ce0:	f1ba 0f00 	cmp.w	sl, #0
 f802ce4:	d127      	bne.n	f802d36 <DiagVprintf+0x46e>
 f802ce6:	2308      	movs	r3, #8
 f802ce8:	2201      	movs	r2, #1
 f802cea:	4651      	mov	r1, sl
 f802cec:	f8cd a008 	str.w	sl, [sp, #8]
 f802cf0:	e9cd 3200 	strd	r3, r2, [sp]
 f802cf4:	2330      	movs	r3, #48	; 0x30
 f802cf6:	2210      	movs	r2, #16
 f802cf8:	f7ff fcc4 	bl	f802684 <print_unsigned_num>
 f802cfc:	2301      	movs	r3, #1
 f802cfe:	4405      	add	r5, r0
 f802d00:	3401      	adds	r4, #1
 f802d02:	9304      	str	r3, [sp, #16]
 f802d04:	e5f2      	b.n	f8028ec <DiagVprintf+0x24>
 f802d06:	9a04      	ldr	r2, [sp, #16]
 f802d08:	464b      	mov	r3, r9
 f802d0a:	f8cd a008 	str.w	sl, [sp, #8]
 f802d0e:	9201      	str	r2, [sp, #4]
 f802d10:	2210      	movs	r2, #16
 f802d12:	9600      	str	r6, [sp, #0]
 f802d14:	f7ff fcb6 	bl	f802684 <print_unsigned_num>
 f802d18:	4405      	add	r5, r0
 f802d1a:	e6a8      	b.n	f802a6e <DiagVprintf+0x1a6>
 f802d1c:	9b06      	ldr	r3, [sp, #24]
 f802d1e:	eba6 0803 	sub.w	r8, r6, r3
 f802d22:	f1b8 0f00 	cmp.w	r8, #0
 f802d26:	dc66      	bgt.n	f802df6 <DiagVprintf+0x52e>
 f802d28:	2a00      	cmp	r2, #0
 f802d2a:	f000 808a 	beq.w	f802e42 <DiagVprintf+0x57a>
 f802d2e:	7810      	ldrb	r0, [r2, #0]
 f802d30:	2800      	cmp	r0, #0
 f802d32:	d1ba      	bne.n	f802caa <DiagVprintf+0x3e2>
 f802d34:	e7ce      	b.n	f802cd4 <DiagVprintf+0x40c>
 f802d36:	2300      	movs	r3, #0
 f802d38:	2210      	movs	r2, #16
 f802d3a:	2100      	movs	r1, #0
 f802d3c:	9302      	str	r3, [sp, #8]
 f802d3e:	9300      	str	r3, [sp, #0]
 f802d40:	2301      	movs	r3, #1
 f802d42:	9301      	str	r3, [sp, #4]
 f802d44:	2330      	movs	r3, #48	; 0x30
 f802d46:	f7ff fc9d 	bl	f802684 <print_unsigned_num>
 f802d4a:	e7d7      	b.n	f802cfc <DiagVprintf+0x434>
 f802d4c:	f858 0b04 	ldr.w	r0, [r8], #4
 f802d50:	2100      	movs	r1, #0
 f802d52:	f1ba 0f00 	cmp.w	sl, #0
 f802d56:	f47f aee7 	bne.w	f802b28 <DiagVprintf+0x260>
 f802d5a:	9a04      	ldr	r2, [sp, #16]
 f802d5c:	464b      	mov	r3, r9
 f802d5e:	f8cd a008 	str.w	sl, [sp, #8]
 f802d62:	9201      	str	r2, [sp, #4]
 f802d64:	2208      	movs	r2, #8
 f802d66:	9600      	str	r6, [sp, #0]
 f802d68:	f7ff fc8c 	bl	f802684 <print_unsigned_num>
 f802d6c:	4405      	add	r5, r0
 f802d6e:	e67e      	b.n	f802a6e <DiagVprintf+0x1a6>
 f802d70:	f858 0b04 	ldr.w	r0, [r8], #4
 f802d74:	2100      	movs	r1, #0
 f802d76:	f1ba 0f00 	cmp.w	sl, #0
 f802d7a:	f47f af28 	bne.w	f802bce <DiagVprintf+0x306>
 f802d7e:	9a04      	ldr	r2, [sp, #16]
 f802d80:	464b      	mov	r3, r9
 f802d82:	f8cd a008 	str.w	sl, [sp, #8]
 f802d86:	9201      	str	r2, [sp, #4]
 f802d88:	2202      	movs	r2, #2
 f802d8a:	9600      	str	r6, [sp, #0]
 f802d8c:	f7ff fc7a 	bl	f802684 <print_unsigned_num>
 f802d90:	4405      	add	r5, r0
 f802d92:	e66c      	b.n	f802a6e <DiagVprintf+0x1a6>
 f802d94:	f858 0b04 	ldr.w	r0, [r8], #4
 f802d98:	2100      	movs	r1, #0
 f802d9a:	f1ba 0f00 	cmp.w	sl, #0
 f802d9e:	f47f af44 	bne.w	f802c2a <DiagVprintf+0x362>
 f802da2:	f8cd a008 	str.w	sl, [sp, #8]
 f802da6:	9a04      	ldr	r2, [sp, #16]
 f802da8:	464b      	mov	r3, r9
 f802daa:	9600      	str	r6, [sp, #0]
 f802dac:	9201      	str	r2, [sp, #4]
 f802dae:	220a      	movs	r2, #10
 f802db0:	f7ff fc68 	bl	f802684 <print_unsigned_num>
 f802db4:	4405      	add	r5, r0
 f802db6:	e65a      	b.n	f802a6e <DiagVprintf+0x1a6>
 f802db8:	f8d8 0000 	ldr.w	r0, [r8]
 f802dbc:	f108 0204 	add.w	r2, r8, #4
 f802dc0:	17c1      	asrs	r1, r0, #31
 f802dc2:	4690      	mov	r8, r2
 f802dc4:	e67a      	b.n	f802abc <DiagVprintf+0x1f4>
 f802dc6:	f858 0b04 	ldr.w	r0, [r8], #4
 f802dca:	2100      	movs	r1, #0
 f802dcc:	e62f      	b.n	f802a2e <DiagVprintf+0x166>
 f802dce:	460d      	mov	r5, r1
 f802dd0:	3401      	adds	r4, #1
 f802dd2:	e58b      	b.n	f8028ec <DiagVprintf+0x24>
 f802dd4:	4240      	negs	r0, r0
 f802dd6:	f04f 0201 	mov.w	r2, #1
 f802dda:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
 f802dde:	e671      	b.n	f802ac4 <DiagVprintf+0x1fc>
 f802de0:	f106 3aff 	add.w	sl, r6, #4294967295	; 0xffffffff
 f802de4:	4648      	mov	r0, r9
 f802de6:	47b8      	blx	r7
 f802de8:	f1ba 0a01 	subs.w	sl, sl, #1
 f802dec:	d1fa      	bne.n	f802de4 <DiagVprintf+0x51c>
 f802dee:	4658      	mov	r0, fp
 f802df0:	4435      	add	r5, r6
 f802df2:	47b8      	blx	r7
 f802df4:	e63b      	b.n	f802a6e <DiagVprintf+0x1a6>
 f802df6:	4623      	mov	r3, r4
 f802df8:	46c3      	mov	fp, r8
 f802dfa:	464c      	mov	r4, r9
 f802dfc:	9207      	str	r2, [sp, #28]
 f802dfe:	4699      	mov	r9, r3
 f802e00:	4620      	mov	r0, r4
 f802e02:	47b8      	blx	r7
 f802e04:	f1bb 0b01 	subs.w	fp, fp, #1
 f802e08:	d1fa      	bne.n	f802e00 <DiagVprintf+0x538>
 f802e0a:	464b      	mov	r3, r9
 f802e0c:	9a07      	ldr	r2, [sp, #28]
 f802e0e:	46a1      	mov	r9, r4
 f802e10:	4445      	add	r5, r8
 f802e12:	461c      	mov	r4, r3
 f802e14:	e788      	b.n	f802d28 <DiagVprintf+0x460>
 f802e16:	465e      	mov	r6, fp
 f802e18:	4648      	mov	r0, r9
 f802e1a:	47b8      	blx	r7
 f802e1c:	3e01      	subs	r6, #1
 f802e1e:	d1fb      	bne.n	f802e18 <DiagVprintf+0x550>
 f802e20:	445d      	add	r5, fp
 f802e22:	f8dd 8014 	ldr.w	r8, [sp, #20]
 f802e26:	e622      	b.n	f802a6e <DiagVprintf+0x1a6>
 f802e28:	4606      	mov	r6, r0
 f802e2a:	4630      	mov	r0, r6
 f802e2c:	e61e      	b.n	f802a6c <DiagVprintf+0x1a4>
 f802e2e:	4605      	mov	r5, r0
 f802e30:	e5eb      	b.n	f802a0a <DiagVprintf+0x142>
 f802e32:	9006      	str	r0, [sp, #24]
 f802e34:	f1ba 0f00 	cmp.w	sl, #0
 f802e38:	f47f af33 	bne.w	f802ca2 <DiagVprintf+0x3da>
 f802e3c:	e76e      	b.n	f802d1c <DiagVprintf+0x454>
 f802e3e:	3d01      	subs	r5, #1
 f802e40:	e741      	b.n	f802cc6 <DiagVprintf+0x3fe>
 f802e42:	3d01      	subs	r5, #1
 f802e44:	e746      	b.n	f802cd4 <DiagVprintf+0x40c>
 f802e46:	bf00      	nop

0f802e48 <DiagPrintf>:
 f802e48:	b40f      	push	{r0, r1, r2, r3}
 f802e4a:	b500      	push	{lr}
 f802e4c:	b083      	sub	sp, #12
 f802e4e:	a904      	add	r1, sp, #16
 f802e50:	f851 0b04 	ldr.w	r0, [r1], #4
 f802e54:	9101      	str	r1, [sp, #4]
 f802e56:	f7ff fd37 	bl	f8028c8 <DiagVprintf>
 f802e5a:	b003      	add	sp, #12
 f802e5c:	f85d eb04 	ldr.w	lr, [sp], #4
 f802e60:	b004      	add	sp, #16
 f802e62:	4770      	bx	lr

0f802e64 <DiagPrintf_minimal>:
 f802e64:	b40f      	push	{r0, r1, r2, r3}
 f802e66:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 f802e6a:	b089      	sub	sp, #36	; 0x24
 f802e6c:	ab12      	add	r3, sp, #72	; 0x48
 f802e6e:	f853 4b04 	ldr.w	r4, [r3], #4
 f802e72:	7820      	ldrb	r0, [r4, #0]
 f802e74:	9304      	str	r3, [sp, #16]
 f802e76:	2800      	cmp	r0, #0
 f802e78:	d040      	beq.n	f802efc <DiagPrintf_minimal+0x98>
 f802e7a:	4699      	mov	r9, r3
 f802e7c:	2700      	movs	r7, #0
 f802e7e:	f8df 82cc 	ldr.w	r8, [pc, #716]	; f80314c <DiagPrintf_minimal+0x2e8>
 f802e82:	e004      	b.n	f802e8e <DiagPrintf_minimal+0x2a>
 f802e84:	3701      	adds	r7, #1
 f802e86:	47c0      	blx	r8
 f802e88:	7820      	ldrb	r0, [r4, #0]
 f802e8a:	2800      	cmp	r0, #0
 f802e8c:	d04c      	beq.n	f802f28 <DiagPrintf_minimal+0xc4>
 f802e8e:	2825      	cmp	r0, #37	; 0x25
 f802e90:	f104 0401 	add.w	r4, r4, #1
 f802e94:	d1f6      	bne.n	f802e84 <DiagPrintf_minimal+0x20>
 f802e96:	2500      	movs	r5, #0
 f802e98:	462e      	mov	r6, r5
 f802e9a:	7823      	ldrb	r3, [r4, #0]
 f802e9c:	2b43      	cmp	r3, #67	; 0x43
 f802e9e:	d850      	bhi.n	f802f42 <DiagPrintf_minimal+0xde>
 f802ea0:	2b1f      	cmp	r3, #31
 f802ea2:	d929      	bls.n	f802ef8 <DiagPrintf_minimal+0x94>
 f802ea4:	f1a3 0220 	sub.w	r2, r3, #32
 f802ea8:	2a23      	cmp	r2, #35	; 0x23
 f802eaa:	d825      	bhi.n	f802ef8 <DiagPrintf_minimal+0x94>
 f802eac:	e8df f012 	tbh	[pc, r2, lsl #1]
 f802eb0:	00240042 	.word	0x00240042
 f802eb4:	00240024 	.word	0x00240024
 f802eb8:	012f0024 	.word	0x012f0024
 f802ebc:	00240024 	.word	0x00240024
 f802ec0:	00240024 	.word	0x00240024
 f802ec4:	00240024 	.word	0x00240024
 f802ec8:	00240024 	.word	0x00240024
 f802ecc:	00240024 	.word	0x00240024
 f802ed0:	002b0042 	.word	0x002b0042
 f802ed4:	002b002b 	.word	0x002b002b
 f802ed8:	002b002b 	.word	0x002b002b
 f802edc:	002b002b 	.word	0x002b002b
 f802ee0:	002b002b 	.word	0x002b002b
 f802ee4:	00240024 	.word	0x00240024
 f802ee8:	00240024 	.word	0x00240024
 f802eec:	00240024 	.word	0x00240024
 f802ef0:	00240024 	.word	0x00240024
 f802ef4:	00c00024 	.word	0x00c00024
 f802ef8:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
 f802efc:	b009      	add	sp, #36	; 0x24
 f802efe:	e8bd 4ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 f802f02:	b004      	add	sp, #16
 f802f04:	4770      	bx	lr
 f802f06:	2620      	movs	r6, #32
 f802f08:	2500      	movs	r5, #0
 f802f0a:	f1a3 0130 	sub.w	r1, r3, #48	; 0x30
 f802f0e:	eb05 0285 	add.w	r2, r5, r5, lsl #2
 f802f12:	b2c9      	uxtb	r1, r1
 f802f14:	eb03 0242 	add.w	r2, r3, r2, lsl #1
 f802f18:	2909      	cmp	r1, #9
 f802f1a:	d8be      	bhi.n	f802e9a <DiagPrintf_minimal+0x36>
 f802f1c:	f814 3f01 	ldrb.w	r3, [r4, #1]!
 f802f20:	f1a2 0530 	sub.w	r5, r2, #48	; 0x30
 f802f24:	2b00      	cmp	r3, #0
 f802f26:	d1f0      	bne.n	f802f0a <DiagPrintf_minimal+0xa6>
 f802f28:	4638      	mov	r0, r7
 f802f2a:	b009      	add	sp, #36	; 0x24
 f802f2c:	e8bd 4ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 f802f30:	b004      	add	sp, #16
 f802f32:	4770      	bx	lr
 f802f34:	7862      	ldrb	r2, [r4, #1]
 f802f36:	3401      	adds	r4, #1
 f802f38:	2a00      	cmp	r2, #0
 f802f3a:	d0f5      	beq.n	f802f28 <DiagPrintf_minimal+0xc4>
 f802f3c:	461e      	mov	r6, r3
 f802f3e:	4613      	mov	r3, r2
 f802f40:	e7e2      	b.n	f802f08 <DiagPrintf_minimal+0xa4>
 f802f42:	3b63      	subs	r3, #99	; 0x63
 f802f44:	b2da      	uxtb	r2, r3
 f802f46:	2a15      	cmp	r2, #21
 f802f48:	d8d6      	bhi.n	f802ef8 <DiagPrintf_minimal+0x94>
 f802f4a:	2b15      	cmp	r3, #21
 f802f4c:	d8d4      	bhi.n	f802ef8 <DiagPrintf_minimal+0x94>
 f802f4e:	a201      	add	r2, pc, #4	; (adr r2, f802f54 <DiagPrintf_minimal+0xf0>)
 f802f50:	f852 f023 	ldr.w	pc, [r2, r3, lsl #2]
 f802f54:	0f803031 	.word	0x0f803031
 f802f58:	0f802fad 	.word	0x0f802fad
 f802f5c:	0f802ef9 	.word	0x0f802ef9
 f802f60:	0f802ef9 	.word	0x0f802ef9
 f802f64:	0f802ef9 	.word	0x0f802ef9
 f802f68:	0f802ef9 	.word	0x0f802ef9
 f802f6c:	0f802ef9 	.word	0x0f802ef9
 f802f70:	0f802ef9 	.word	0x0f802ef9
 f802f74:	0f802ef9 	.word	0x0f802ef9
 f802f78:	0f802ef9 	.word	0x0f802ef9
 f802f7c:	0f802ef9 	.word	0x0f802ef9
 f802f80:	0f802ef9 	.word	0x0f802ef9
 f802f84:	0f802ef9 	.word	0x0f802ef9
 f802f88:	0f802ef9 	.word	0x0f802ef9
 f802f8c:	0f802ef9 	.word	0x0f802ef9
 f802f90:	0f802ef9 	.word	0x0f802ef9
 f802f94:	0f803047 	.word	0x0f803047
 f802f98:	0f802ef9 	.word	0x0f802ef9
 f802f9c:	0f802fad 	.word	0x0f802fad
 f802fa0:	0f802ef9 	.word	0x0f802ef9
 f802fa4:	0f802ef9 	.word	0x0f802ef9
 f802fa8:	0f80308d 	.word	0x0f80308d
 f802fac:	464b      	mov	r3, r9
 f802fae:	f10d 0a14 	add.w	sl, sp, #20
 f802fb2:	2200      	movs	r2, #0
 f802fb4:	f853 1b04 	ldr.w	r1, [r3], #4
 f802fb8:	4650      	mov	r0, sl
 f802fba:	9301      	str	r3, [sp, #4]
 f802fbc:	4b62      	ldr	r3, [pc, #392]	; (f803148 <DiagPrintf_minimal+0x2e4>)
 f802fbe:	4693      	mov	fp, r2
 f802fc0:	3201      	adds	r2, #1
 f802fc2:	fba3 3c01 	umull	r3, ip, r3, r1
 f802fc6:	460b      	mov	r3, r1
 f802fc8:	ea4f 0cdc 	mov.w	ip, ip, lsr #3
 f802fcc:	2b09      	cmp	r3, #9
 f802fce:	eb0c 098c 	add.w	r9, ip, ip, lsl #2
 f802fd2:	eba1 0949 	sub.w	r9, r1, r9, lsl #1
 f802fd6:	4661      	mov	r1, ip
 f802fd8:	f109 0930 	add.w	r9, r9, #48	; 0x30
 f802fdc:	fa5f f989 	uxtb.w	r9, r9
 f802fe0:	f800 9b01 	strb.w	r9, [r0], #1
 f802fe4:	d8ea      	bhi.n	f802fbc <DiagPrintf_minimal+0x158>
 f802fe6:	2d00      	cmp	r5, #0
 f802fe8:	f340 8095 	ble.w	f803116 <DiagPrintf_minimal+0x2b2>
 f802fec:	4295      	cmp	r5, r2
 f802fee:	f340 8092 	ble.w	f803116 <DiagPrintf_minimal+0x2b2>
 f802ff2:	f8cd a00c 	str.w	sl, [sp, #12]
 f802ff6:	46a2      	mov	sl, r4
 f802ff8:	4614      	mov	r4, r2
 f802ffa:	9502      	str	r5, [sp, #8]
 f802ffc:	3d01      	subs	r5, #1
 f802ffe:	4630      	mov	r0, r6
 f803000:	47c0      	blx	r8
 f803002:	42ac      	cmp	r4, r5
 f803004:	d1fa      	bne.n	f802ffc <DiagPrintf_minimal+0x198>
 f803006:	4622      	mov	r2, r4
 f803008:	9d02      	ldr	r5, [sp, #8]
 f80300a:	4654      	mov	r4, sl
 f80300c:	f8dd a00c 	ldr.w	sl, [sp, #12]
 f803010:	1aad      	subs	r5, r5, r2
 f803012:	eb0a 060b 	add.w	r6, sl, fp
 f803016:	4648      	mov	r0, r9
 f803018:	e001      	b.n	f80301e <DiagPrintf_minimal+0x1ba>
 f80301a:	f816 0d01 	ldrb.w	r0, [r6, #-1]!
 f80301e:	47c0      	blx	r8
 f803020:	45b2      	cmp	sl, r6
 f803022:	d1fa      	bne.n	f80301a <DiagPrintf_minimal+0x1b6>
 f803024:	3701      	adds	r7, #1
 f803026:	f8dd 9004 	ldr.w	r9, [sp, #4]
 f80302a:	445f      	add	r7, fp
 f80302c:	442f      	add	r7, r5
 f80302e:	e008      	b.n	f803042 <DiagPrintf_minimal+0x1de>
 f803030:	f859 3b04 	ldr.w	r3, [r9], #4
 f803034:	2d01      	cmp	r5, #1
 f803036:	fa5f fb83 	uxtb.w	fp, r3
 f80303a:	dc78      	bgt.n	f80312e <DiagPrintf_minimal+0x2ca>
 f80303c:	3701      	adds	r7, #1
 f80303e:	4658      	mov	r0, fp
 f803040:	47c0      	blx	r8
 f803042:	3401      	adds	r4, #1
 f803044:	e720      	b.n	f802e88 <DiagPrintf_minimal+0x24>
 f803046:	f859 ab04 	ldr.w	sl, [r9], #4
 f80304a:	f89a 3000 	ldrb.w	r3, [sl]
 f80304e:	b14b      	cbz	r3, f803064 <DiagPrintf_minimal+0x200>
 f803050:	4653      	mov	r3, sl
 f803052:	f1ca 0b01 	rsb	fp, sl, #1
 f803056:	eb0b 0103 	add.w	r1, fp, r3
 f80305a:	f813 2f01 	ldrb.w	r2, [r3, #1]!
 f80305e:	2a00      	cmp	r2, #0
 f803060:	d1f9      	bne.n	f803056 <DiagPrintf_minimal+0x1f2>
 f803062:	1a6d      	subs	r5, r5, r1
 f803064:	2d00      	cmp	r5, #0
 f803066:	dc5a      	bgt.n	f80311e <DiagPrintf_minimal+0x2ba>
 f803068:	f1ba 0f00 	cmp.w	sl, #0
 f80306c:	d069      	beq.n	f803142 <DiagPrintf_minimal+0x2de>
 f80306e:	f89a 0000 	ldrb.w	r0, [sl]
 f803072:	2800      	cmp	r0, #0
 f803074:	d0e5      	beq.n	f803042 <DiagPrintf_minimal+0x1de>
 f803076:	f1ca 0b01 	rsb	fp, sl, #1
 f80307a:	47c0      	blx	r8
 f80307c:	eb0a 030b 	add.w	r3, sl, fp
 f803080:	f81a 0f01 	ldrb.w	r0, [sl, #1]!
 f803084:	2800      	cmp	r0, #0
 f803086:	d1f8      	bne.n	f80307a <DiagPrintf_minimal+0x216>
 f803088:	441f      	add	r7, r3
 f80308a:	e7da      	b.n	f803042 <DiagPrintf_minimal+0x1de>
 f80308c:	464a      	mov	r2, r9
 f80308e:	f10d 0a14 	add.w	sl, sp, #20
 f803092:	f04f 0b00 	mov.w	fp, #0
 f803096:	f852 3b04 	ldr.w	r3, [r2], #4
 f80309a:	4650      	mov	r0, sl
 f80309c:	9201      	str	r2, [sp, #4]
 f80309e:	e000      	b.n	f8030a2 <DiagPrintf_minimal+0x23e>
 f8030a0:	468b      	mov	fp, r1
 f8030a2:	f003 020f 	and.w	r2, r3, #15
 f8030a6:	f10b 0101 	add.w	r1, fp, #1
 f8030aa:	2a09      	cmp	r2, #9
 f8030ac:	f102 0957 	add.w	r9, r2, #87	; 0x57
 f8030b0:	bf98      	it	ls
 f8030b2:	f102 0930 	addls.w	r9, r2, #48	; 0x30
 f8030b6:	091a      	lsrs	r2, r3, #4
 f8030b8:	2b0f      	cmp	r3, #15
 f8030ba:	f800 9b01 	strb.w	r9, [r0], #1
 f8030be:	4613      	mov	r3, r2
 f8030c0:	d8ee      	bhi.n	f8030a0 <DiagPrintf_minimal+0x23c>
 f8030c2:	2d00      	cmp	r5, #0
 f8030c4:	dd29      	ble.n	f80311a <DiagPrintf_minimal+0x2b6>
 f8030c6:	428d      	cmp	r5, r1
 f8030c8:	dd27      	ble.n	f80311a <DiagPrintf_minimal+0x2b6>
 f8030ca:	462b      	mov	r3, r5
 f8030cc:	9502      	str	r5, [sp, #8]
 f8030ce:	f8cd a00c 	str.w	sl, [sp, #12]
 f8030d2:	4635      	mov	r5, r6
 f8030d4:	46a2      	mov	sl, r4
 f8030d6:	461e      	mov	r6, r3
 f8030d8:	460c      	mov	r4, r1
 f8030da:	3e01      	subs	r6, #1
 f8030dc:	4628      	mov	r0, r5
 f8030de:	47c0      	blx	r8
 f8030e0:	42b4      	cmp	r4, r6
 f8030e2:	d1fa      	bne.n	f8030da <DiagPrintf_minimal+0x276>
 f8030e4:	4621      	mov	r1, r4
 f8030e6:	9d02      	ldr	r5, [sp, #8]
 f8030e8:	4654      	mov	r4, sl
 f8030ea:	f8dd a00c 	ldr.w	sl, [sp, #12]
 f8030ee:	1a6d      	subs	r5, r5, r1
 f8030f0:	eb0a 060b 	add.w	r6, sl, fp
 f8030f4:	4648      	mov	r0, r9
 f8030f6:	e001      	b.n	f8030fc <DiagPrintf_minimal+0x298>
 f8030f8:	f816 0d01 	ldrb.w	r0, [r6, #-1]!
 f8030fc:	47c0      	blx	r8
 f8030fe:	4556      	cmp	r6, sl
 f803100:	d1fa      	bne.n	f8030f8 <DiagPrintf_minimal+0x294>
 f803102:	3701      	adds	r7, #1
 f803104:	f8dd 9004 	ldr.w	r9, [sp, #4]
 f803108:	445f      	add	r7, fp
 f80310a:	442f      	add	r7, r5
 f80310c:	e799      	b.n	f803042 <DiagPrintf_minimal+0x1de>
 f80310e:	2025      	movs	r0, #37	; 0x25
 f803110:	3701      	adds	r7, #1
 f803112:	47c0      	blx	r8
 f803114:	e795      	b.n	f803042 <DiagPrintf_minimal+0x1de>
 f803116:	2500      	movs	r5, #0
 f803118:	e77b      	b.n	f803012 <DiagPrintf_minimal+0x1ae>
 f80311a:	2500      	movs	r5, #0
 f80311c:	e7e8      	b.n	f8030f0 <DiagPrintf_minimal+0x28c>
 f80311e:	46ab      	mov	fp, r5
 f803120:	4630      	mov	r0, r6
 f803122:	47c0      	blx	r8
 f803124:	f1bb 0b01 	subs.w	fp, fp, #1
 f803128:	d1fa      	bne.n	f803120 <DiagPrintf_minimal+0x2bc>
 f80312a:	442f      	add	r7, r5
 f80312c:	e79c      	b.n	f803068 <DiagPrintf_minimal+0x204>
 f80312e:	f105 3aff 	add.w	sl, r5, #4294967295	; 0xffffffff
 f803132:	4630      	mov	r0, r6
 f803134:	47c0      	blx	r8
 f803136:	f1ba 0a01 	subs.w	sl, sl, #1
 f80313a:	d1fa      	bne.n	f803132 <DiagPrintf_minimal+0x2ce>
 f80313c:	3f01      	subs	r7, #1
 f80313e:	442f      	add	r7, r5
 f803140:	e77c      	b.n	f80303c <DiagPrintf_minimal+0x1d8>
 f803142:	3f01      	subs	r7, #1
 f803144:	e77d      	b.n	f803042 <DiagPrintf_minimal+0x1de>
 f803146:	bf00      	nop
 f803148:	cccccccd 	.word	0xcccccccd
 f80314c:	0000aba1 	.word	0x0000aba1

0f803150 <io_assert_failed>:
 f803150:	b508      	push	{r3, lr}
 f803152:	4603      	mov	r3, r0
 f803154:	460a      	mov	r2, r1
 f803156:	4802      	ldr	r0, [pc, #8]	; (f803160 <io_assert_failed+0x10>)
 f803158:	4619      	mov	r1, r3
 f80315a:	f7ff fe83 	bl	f802e64 <DiagPrintf_minimal>
 f80315e:	e7fe      	b.n	f80315e <io_assert_failed+0xe>
 f803160:	0f8050b4 	.word	0x0f8050b4

0f803164 <BOOT_TRNG_ParaSet>:
 f803164:	4b03      	ldr	r3, [pc, #12]	; (f803174 <BOOT_TRNG_ParaSet+0x10>)
 f803166:	2142      	movs	r1, #66	; 0x42
 f803168:	f240 320f 	movw	r2, #783	; 0x30f
 f80316c:	6359      	str	r1, [r3, #52]	; 0x34
 f80316e:	f8c3 2114 	str.w	r2, [r3, #276]	; 0x114
 f803172:	4770      	bx	lr
 f803174:	51101000 	.word	0x51101000

0f803178 <BOOT_ResetMask_Config>:
 f803178:	4b05      	ldr	r3, [pc, #20]	; (f803190 <BOOT_ResetMask_Config+0x18>)
 f80317a:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
 f80317e:	2104      	movs	r1, #4
 f803180:	2200      	movs	r2, #0
 f803182:	f8c3 02d0 	str.w	r0, [r3, #720]	; 0x2d0
 f803186:	f8c3 12d4 	str.w	r1, [r3, #724]	; 0x2d4
 f80318a:	f8c3 20d0 	str.w	r2, [r3, #208]	; 0xd0
 f80318e:	4770      	bx	lr
 f803190:	41008000 	.word	0x41008000

0f803194 <BOOT_Disable_PadSlewRate>:
 f803194:	b538      	push	{r3, r4, r5, lr}
 f803196:	2400      	movs	r4, #0
 f803198:	4d03      	ldr	r5, [pc, #12]	; (f8031a8 <BOOT_Disable_PadSlewRate+0x14>)
 f80319a:	b2e0      	uxtb	r0, r4
 f80319c:	3401      	adds	r4, #1
 f80319e:	2100      	movs	r1, #0
 f8031a0:	47a8      	blx	r5
 f8031a2:	2c40      	cmp	r4, #64	; 0x40
 f8031a4:	d1f9      	bne.n	f80319a <BOOT_Disable_PadSlewRate+0x6>
 f8031a6:	bd38      	pop	{r3, r4, r5, pc}
 f8031a8:	0000bb39 	.word	0x0000bb39

0f8031ac <BOOT_Share_Memory_Patch>:
 f8031ac:	4a11      	ldr	r2, [pc, #68]	; (f8031f4 <BOOT_Share_Memory_Patch+0x48>)
 f8031ae:	4912      	ldr	r1, [pc, #72]	; (f8031f8 <BOOT_Share_Memory_Patch+0x4c>)
 f8031b0:	b430      	push	{r4, r5}
 f8031b2:	f8d2 3230 	ldr.w	r3, [r2, #560]	; 0x230
 f8031b6:	4c11      	ldr	r4, [pc, #68]	; (f8031fc <BOOT_Share_Memory_Patch+0x50>)
 f8031b8:	f443 2340 	orr.w	r3, r3, #786432	; 0xc0000
 f8031bc:	4d10      	ldr	r5, [pc, #64]	; (f803200 <BOOT_Share_Memory_Patch+0x54>)
 f8031be:	f8c2 3230 	str.w	r3, [r2, #560]	; 0x230
 f8031c2:	6808      	ldr	r0, [r1, #0]
 f8031c4:	6824      	ldr	r4, [r4, #0]
 f8031c6:	682b      	ldr	r3, [r5, #0]
 f8031c8:	4420      	add	r0, r4
 f8031ca:	4c0e      	ldr	r4, [pc, #56]	; (f803204 <BOOT_Share_Memory_Patch+0x58>)
 f8031cc:	4418      	add	r0, r3
 f8031ce:	6825      	ldr	r5, [r4, #0]
 f8031d0:	f8d2 4230 	ldr.w	r4, [r2, #560]	; 0x230
 f8031d4:	f04f 5300 	mov.w	r3, #536870912	; 0x20000000
 f8031d8:	4428      	add	r0, r5
 f8031da:	f424 2440 	bic.w	r4, r4, #786432	; 0xc0000
 f8031de:	f8c2 4230 	str.w	r4, [r2, #560]	; 0x230
 f8031e2:	681a      	ldr	r2, [r3, #0]
 f8031e4:	f503 3380 	add.w	r3, r3, #65536	; 0x10000
 f8031e8:	428b      	cmp	r3, r1
 f8031ea:	4410      	add	r0, r2
 f8031ec:	d1f9      	bne.n	f8031e2 <BOOT_Share_Memory_Patch+0x36>
 f8031ee:	bc30      	pop	{r4, r5}
 f8031f0:	4770      	bx	lr
 f8031f2:	bf00      	nop
 f8031f4:	41008000 	.word	0x41008000
 f8031f8:	20080000 	.word	0x20080000
 f8031fc:	20090000 	.word	0x20090000
 f803200:	200c0000 	.word	0x200c0000
 f803204:	200c6000 	.word	0x200c6000

0f803208 <BOOT_VerCheck>:
 f803208:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 f80320c:	4b21      	ldr	r3, [pc, #132]	; (f803294 <BOOT_VerCheck+0x8c>)
 f80320e:	4798      	blx	r3
 f803210:	b108      	cbz	r0, f803216 <BOOT_VerCheck+0xe>
 f803212:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 f803216:	2503      	movs	r5, #3
 f803218:	4c1f      	ldr	r4, [pc, #124]	; (f803298 <BOOT_VerCheck+0x90>)
 f80321a:	4b20      	ldr	r3, [pc, #128]	; (f80329c <BOOT_VerCheck+0x94>)
 f80321c:	2257      	movs	r2, #87	; 0x57
 f80321e:	4621      	mov	r1, r4
 f803220:	4628      	mov	r0, r5
 f803222:	4e1f      	ldr	r6, [pc, #124]	; (f8032a0 <BOOT_VerCheck+0x98>)
 f803224:	f8df 8090 	ldr.w	r8, [pc, #144]	; f8032b8 <BOOT_VerCheck+0xb0>
 f803228:	4f1e      	ldr	r7, [pc, #120]	; (f8032a4 <BOOT_VerCheck+0x9c>)
 f80322a:	f7ff f975 	bl	f802518 <rtk_log_write>
 f80322e:	2257      	movs	r2, #87	; 0x57
 f803230:	4621      	mov	r1, r4
 f803232:	2003      	movs	r0, #3
 f803234:	4633      	mov	r3, r6
 f803236:	f7ff f96f 	bl	f802518 <rtk_log_write>
 f80323a:	4643      	mov	r3, r8
 f80323c:	2257      	movs	r2, #87	; 0x57
 f80323e:	4621      	mov	r1, r4
 f803240:	2003      	movs	r0, #3
 f803242:	f7ff f969 	bl	f802518 <rtk_log_write>
 f803246:	463b      	mov	r3, r7
 f803248:	2257      	movs	r2, #87	; 0x57
 f80324a:	4621      	mov	r1, r4
 f80324c:	2003      	movs	r0, #3
 f80324e:	f7ff f963 	bl	f802518 <rtk_log_write>
 f803252:	2257      	movs	r2, #87	; 0x57
 f803254:	4621      	mov	r1, r4
 f803256:	2003      	movs	r0, #3
 f803258:	4b13      	ldr	r3, [pc, #76]	; (f8032a8 <BOOT_VerCheck+0xa0>)
 f80325a:	f7ff f95d 	bl	f802518 <rtk_log_write>
 f80325e:	4633      	mov	r3, r6
 f803260:	2257      	movs	r2, #87	; 0x57
 f803262:	4621      	mov	r1, r4
 f803264:	2003      	movs	r0, #3
 f803266:	f7ff f957 	bl	f802518 <rtk_log_write>
 f80326a:	4b10      	ldr	r3, [pc, #64]	; (f8032ac <BOOT_VerCheck+0xa4>)
 f80326c:	2257      	movs	r2, #87	; 0x57
 f80326e:	4621      	mov	r1, r4
 f803270:	2003      	movs	r0, #3
 f803272:	f7ff f951 	bl	f802518 <rtk_log_write>
 f803276:	3d01      	subs	r5, #1
 f803278:	d1d9      	bne.n	f80322e <BOOT_VerCheck+0x26>
 f80327a:	4907      	ldr	r1, [pc, #28]	; (f803298 <BOOT_VerCheck+0x90>)
 f80327c:	2003      	movs	r0, #3
 f80327e:	4b0c      	ldr	r3, [pc, #48]	; (f8032b0 <BOOT_VerCheck+0xa8>)
 f803280:	2257      	movs	r2, #87	; 0x57
 f803282:	f7ff f949 	bl	f802518 <rtk_log_write>
 f803286:	2160      	movs	r1, #96	; 0x60
 f803288:	480a      	ldr	r0, [pc, #40]	; (f8032b4 <BOOT_VerCheck+0xac>)
 f80328a:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
 f80328e:	f7ff bf5f 	b.w	f803150 <io_assert_failed>
 f803292:	bf00      	nop
 f803294:	0000c15d 	.word	0x0000c15d
 f803298:	0f804988 	.word	0x0f804988
 f80329c:	0f8050e8 	.word	0x0f8050e8
 f8032a0:	0f805124 	.word	0x0f805124
 f8032a4:	0f80519c 	.word	0x0f80519c
 f8032a8:	0f8051d8 	.word	0x0f8051d8
 f8032ac:	0f805398 	.word	0x0f805398
 f8032b0:	0f804d7c 	.word	0x0f804d7c
 f8032b4:	0f805214 	.word	0x0f805214
 f8032b8:	0f805160 	.word	0x0f805160

0f8032bc <BOOT_OTPCheck>:
 f8032bc:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 f8032c0:	4b22      	ldr	r3, [pc, #136]	; (f80334c <BOOT_OTPCheck+0x90>)
 f8032c2:	4798      	blx	r3
 f8032c4:	b908      	cbnz	r0, f8032ca <BOOT_OTPCheck+0xe>
 f8032c6:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 f8032ca:	4b21      	ldr	r3, [pc, #132]	; (f803350 <BOOT_OTPCheck+0x94>)
 f8032cc:	4798      	blx	r3
 f8032ce:	2800      	cmp	r0, #0
 f8032d0:	d0f9      	beq.n	f8032c6 <BOOT_OTPCheck+0xa>
 f8032d2:	2503      	movs	r5, #3
 f8032d4:	4c1f      	ldr	r4, [pc, #124]	; (f803354 <BOOT_OTPCheck+0x98>)
 f8032d6:	4b20      	ldr	r3, [pc, #128]	; (f803358 <BOOT_OTPCheck+0x9c>)
 f8032d8:	2257      	movs	r2, #87	; 0x57
 f8032da:	4621      	mov	r1, r4
 f8032dc:	4628      	mov	r0, r5
 f8032de:	4e1f      	ldr	r6, [pc, #124]	; (f80335c <BOOT_OTPCheck+0xa0>)
 f8032e0:	f8df 8090 	ldr.w	r8, [pc, #144]	; f803374 <BOOT_OTPCheck+0xb8>
 f8032e4:	4f1e      	ldr	r7, [pc, #120]	; (f803360 <BOOT_OTPCheck+0xa4>)
 f8032e6:	f7ff f917 	bl	f802518 <rtk_log_write>
 f8032ea:	2257      	movs	r2, #87	; 0x57
 f8032ec:	4621      	mov	r1, r4
 f8032ee:	2003      	movs	r0, #3
 f8032f0:	4633      	mov	r3, r6
 f8032f2:	f7ff f911 	bl	f802518 <rtk_log_write>
 f8032f6:	4643      	mov	r3, r8
 f8032f8:	2257      	movs	r2, #87	; 0x57
 f8032fa:	4621      	mov	r1, r4
 f8032fc:	2003      	movs	r0, #3
 f8032fe:	f7ff f90b 	bl	f802518 <rtk_log_write>
 f803302:	463b      	mov	r3, r7
 f803304:	2257      	movs	r2, #87	; 0x57
 f803306:	4621      	mov	r1, r4
 f803308:	2003      	movs	r0, #3
 f80330a:	f7ff f905 	bl	f802518 <rtk_log_write>
 f80330e:	2257      	movs	r2, #87	; 0x57
 f803310:	4621      	mov	r1, r4
 f803312:	2003      	movs	r0, #3
 f803314:	4b13      	ldr	r3, [pc, #76]	; (f803364 <BOOT_OTPCheck+0xa8>)
 f803316:	f7ff f8ff 	bl	f802518 <rtk_log_write>
 f80331a:	4633      	mov	r3, r6
 f80331c:	2257      	movs	r2, #87	; 0x57
 f80331e:	4621      	mov	r1, r4
 f803320:	2003      	movs	r0, #3
 f803322:	f7ff f8f9 	bl	f802518 <rtk_log_write>
 f803326:	4b10      	ldr	r3, [pc, #64]	; (f803368 <BOOT_OTPCheck+0xac>)
 f803328:	2257      	movs	r2, #87	; 0x57
 f80332a:	4621      	mov	r1, r4
 f80332c:	2003      	movs	r0, #3
 f80332e:	f7ff f8f3 	bl	f802518 <rtk_log_write>
 f803332:	3d01      	subs	r5, #1
 f803334:	d1d9      	bne.n	f8032ea <BOOT_OTPCheck+0x2e>
 f803336:	4b0d      	ldr	r3, [pc, #52]	; (f80336c <BOOT_OTPCheck+0xb0>)
 f803338:	2257      	movs	r2, #87	; 0x57
 f80333a:	4906      	ldr	r1, [pc, #24]	; (f803354 <BOOT_OTPCheck+0x98>)
 f80333c:	2003      	movs	r0, #3
 f80333e:	4c0c      	ldr	r4, [pc, #48]	; (f803370 <BOOT_OTPCheck+0xb4>)
 f803340:	f7ff f8ea 	bl	f802518 <rtk_log_write>
 f803344:	f242 7010 	movw	r0, #10000	; 0x2710
 f803348:	47a0      	blx	r4
 f80334a:	e7fb      	b.n	f803344 <BOOT_OTPCheck+0x88>
 f80334c:	0000c15d 	.word	0x0000c15d
 f803350:	0f80286d 	.word	0x0f80286d
 f803354:	0f804988 	.word	0x0f804988
 f803358:	0f805254 	.word	0x0f805254
 f80335c:	0f805294 	.word	0x0f805294
 f803360:	0f805314 	.word	0x0f805314
 f803364:	0f805354 	.word	0x0f805354
 f803368:	0f805394 	.word	0x0f805394
 f80336c:	0f804d7c 	.word	0x0f804d7c
 f803370:	0000951d 	.word	0x0000951d
 f803374:	0f8052d4 	.word	0x0f8052d4

0f803378 <BOOT_TZCfg_IsSecChip>:
 f803378:	4b11      	ldr	r3, [pc, #68]	; (f8033c0 <BOOT_TZCfg_IsSecChip+0x48>)
 f80337a:	7818      	ldrb	r0, [r3, #0]
 f80337c:	b140      	cbz	r0, f803390 <BOOT_TZCfg_IsSecChip+0x18>
 f80337e:	b570      	push	{r4, r5, r6, lr}
 f803380:	4c10      	ldr	r4, [pc, #64]	; (f8033c4 <BOOT_TZCfg_IsSecChip+0x4c>)
 f803382:	7820      	ldrb	r0, [r4, #0]
 f803384:	28ff      	cmp	r0, #255	; 0xff
 f803386:	d004      	beq.n	f803392 <BOOT_TZCfg_IsSecChip+0x1a>
 f803388:	3800      	subs	r0, #0
 f80338a:	bf18      	it	ne
 f80338c:	2001      	movne	r0, #1
 f80338e:	bd70      	pop	{r4, r5, r6, pc}
 f803390:	4770      	bx	lr
 f803392:	4e0d      	ldr	r6, [pc, #52]	; (f8033c8 <BOOT_TZCfg_IsSecChip+0x50>)
 f803394:	2001      	movs	r0, #1
 f803396:	47b0      	blx	r6
 f803398:	4605      	mov	r5, r0
 f80339a:	2001      	movs	r0, #1
 f80339c:	47b0      	blx	r6
 f80339e:	2d03      	cmp	r5, #3
 f8033a0:	d009      	beq.n	f8033b6 <BOOT_TZCfg_IsSecChip+0x3e>
 f8033a2:	b145      	cbz	r5, f8033b6 <BOOT_TZCfg_IsSecChip+0x3e>
 f8033a4:	2803      	cmp	r0, #3
 f8033a6:	d006      	beq.n	f8033b6 <BOOT_TZCfg_IsSecChip+0x3e>
 f8033a8:	fab0 f380 	clz	r3, r0
 f8033ac:	095b      	lsrs	r3, r3, #5
 f8033ae:	b110      	cbz	r0, f8033b6 <BOOT_TZCfg_IsSecChip+0x3e>
 f8033b0:	4618      	mov	r0, r3
 f8033b2:	7023      	strb	r3, [r4, #0]
 f8033b4:	bd70      	pop	{r4, r5, r6, pc}
 f8033b6:	2301      	movs	r3, #1
 f8033b8:	4618      	mov	r0, r3
 f8033ba:	7023      	strb	r3, [r4, #0]
 f8033bc:	bd70      	pop	{r4, r5, r6, pc}
 f8033be:	bf00      	nop
 f8033c0:	0f804530 	.word	0x0f804530
 f8033c4:	3000a1cb 	.word	0x3000a1cb
 f8033c8:	000091c5 	.word	0x000091c5

0f8033cc <BOOT_CPU_TZCfg>:
 f8033cc:	2200      	movs	r2, #0
 f8033ce:	b570      	push	{r4, r5, r6, lr}
 f8033d0:	4611      	mov	r1, r2
 f8033d2:	4d36      	ldr	r5, [pc, #216]	; (f8034ac <BOOT_CPU_TZCfg+0xe0>)
 f8033d4:	f100 0608 	add.w	r6, r0, #8
 f8033d8:	f100 0e04 	add.w	lr, r0, #4
 f8033dc:	f850 3022 	ldr.w	r3, [r0, r2, lsl #2]
 f8033e0:	3301      	adds	r3, #1
 f8033e2:	d01c      	beq.n	f80341e <BOOT_CPU_TZCfg+0x52>
 f8033e4:	f8c5 10d8 	str.w	r1, [r5, #216]	; 0xd8
 f8033e8:	3101      	adds	r1, #1
 f8033ea:	f850 3022 	ldr.w	r3, [r0, r2, lsl #2]
 f8033ee:	2908      	cmp	r1, #8
 f8033f0:	f023 031f 	bic.w	r3, r3, #31
 f8033f4:	f8c5 30dc 	str.w	r3, [r5, #220]	; 0xdc
 f8033f8:	f856 3022 	ldr.w	r3, [r6, r2, lsl #2]
 f8033fc:	f85e 4022 	ldr.w	r4, [lr, r2, lsl #2]
 f803400:	f102 0203 	add.w	r2, r2, #3
 f803404:	ea4f 0343 	mov.w	r3, r3, lsl #1
 f803408:	f024 0c1f 	bic.w	ip, r4, #31
 f80340c:	f003 0302 	and.w	r3, r3, #2
 f803410:	ea43 030c 	orr.w	r3, r3, ip
 f803414:	f043 0301 	orr.w	r3, r3, #1
 f803418:	f8c5 30e0 	str.w	r3, [r5, #224]	; 0xe0
 f80341c:	d1de      	bne.n	f8033dc <BOOT_CPU_TZCfg+0x10>
 f80341e:	4823      	ldr	r0, [pc, #140]	; (f8034ac <BOOT_CPU_TZCfg+0xe0>)
 f803420:	2201      	movs	r2, #1
 f803422:	4b23      	ldr	r3, [pc, #140]	; (f8034b0 <BOOT_CPU_TZCfg+0xe4>)
 f803424:	f8c0 20d0 	str.w	r2, [r0, #208]	; 0xd0
 f803428:	f8d3 326c 	ldr.w	r3, [r3, #620]	; 0x26c
 f80342c:	f013 5300 	ands.w	r3, r3, #536870912	; 0x20000000
 f803430:	d11e      	bne.n	f803470 <BOOT_CPU_TZCfg+0xa4>
 f803432:	f8c0 3084 	str.w	r3, [r0, #132]	; 0x84
 f803436:	f3bf 8f4f 	dsb	sy
 f80343a:	f8d0 5080 	ldr.w	r5, [r0, #128]	; 0x80
 f80343e:	f643 76e0 	movw	r6, #16352	; 0x3fe0
 f803442:	f3c5 344e 	ubfx	r4, r5, #13, #15
 f803446:	f3c5 05c9 	ubfx	r5, r5, #3, #10
 f80344a:	0164      	lsls	r4, r4, #5
 f80344c:	ea04 0106 	and.w	r1, r4, r6
 f803450:	462b      	mov	r3, r5
 f803452:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
 f803456:	3b01      	subs	r3, #1
 f803458:	f8c0 2274 	str.w	r2, [r0, #628]	; 0x274
 f80345c:	1c5a      	adds	r2, r3, #1
 f80345e:	d1f8      	bne.n	f803452 <BOOT_CPU_TZCfg+0x86>
 f803460:	3c20      	subs	r4, #32
 f803462:	f114 0f20 	cmn.w	r4, #32
 f803466:	d1f1      	bne.n	f80344c <BOOT_CPU_TZCfg+0x80>
 f803468:	f3bf 8f4f 	dsb	sy
 f80346c:	f3bf 8f6f 	isb	sy
 f803470:	4c0e      	ldr	r4, [pc, #56]	; (f8034ac <BOOT_CPU_TZCfg+0xe0>)
 f803472:	f64b 70f7 	movw	r0, #49143	; 0xbff7
 f803476:	4b0f      	ldr	r3, [pc, #60]	; (f8034b4 <BOOT_CPU_TZCfg+0xe8>)
 f803478:	6921      	ldr	r1, [r4, #16]
 f80347a:	f041 0108 	orr.w	r1, r1, #8
 f80347e:	6121      	str	r1, [r4, #16]
 f803480:	68e2      	ldr	r2, [r4, #12]
 f803482:	4002      	ands	r2, r0
 f803484:	4313      	orrs	r3, r2
 f803486:	60e3      	str	r3, [r4, #12]
 f803488:	f7ff ff76 	bl	f803378 <BOOT_TZCfg_IsSecChip>
 f80348c:	b168      	cbz	r0, f8034aa <BOOT_CPU_TZCfg+0xde>
 f80348e:	4a0a      	ldr	r2, [pc, #40]	; (f8034b8 <BOOT_CPU_TZCfg+0xec>)
 f803490:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
 f803494:	4b09      	ldr	r3, [pc, #36]	; (f8034bc <BOOT_CPU_TZCfg+0xf0>)
 f803496:	f8c2 1280 	str.w	r1, [r2, #640]	; 0x280
 f80349a:	f8c2 1284 	str.w	r1, [r2, #644]	; 0x284
 f80349e:	f8c2 1288 	str.w	r1, [r2, #648]	; 0x288
 f8034a2:	68e2      	ldr	r2, [r4, #12]
 f8034a4:	b292      	uxth	r2, r2
 f8034a6:	4313      	orrs	r3, r2
 f8034a8:	60e3      	str	r3, [r4, #12]
 f8034aa:	bd70      	pop	{r4, r5, r6, pc}
 f8034ac:	e000ed00 	.word	0xe000ed00
 f8034b0:	41008000 	.word	0x41008000
 f8034b4:	05fa4000 	.word	0x05fa4000
 f8034b8:	e000e100 	.word	0xe000e100
 f8034bc:	05fa2000 	.word	0x05fa2000

0f8034c0 <BOOT_RAM_TZCfg>:
 f8034c0:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 f8034c4:	b084      	sub	sp, #16
 f8034c6:	f7ff ff57 	bl	f803378 <BOOT_TZCfg_IsSecChip>
 f8034ca:	2800      	cmp	r0, #0
 f8034cc:	d033      	beq.n	f803536 <BOOT_RAM_TZCfg+0x76>
 f8034ce:	4a3b      	ldr	r2, [pc, #236]	; (f8035bc <BOOT_RAM_TZCfg+0xfc>)
 f8034d0:	f10d 0c04 	add.w	ip, sp, #4
 f8034d4:	4b3a      	ldr	r3, [pc, #232]	; (f8035c0 <BOOT_RAM_TZCfg+0x100>)
 f8034d6:	af03      	add	r7, sp, #12
 f8034d8:	9201      	str	r2, [sp, #4]
 f8034da:	f10d 0e08 	add.w	lr, sp, #8
 f8034de:	4a39      	ldr	r2, [pc, #228]	; (f8035c4 <BOOT_RAM_TZCfg+0x104>)
 f8034e0:	2501      	movs	r5, #1
 f8034e2:	4939      	ldr	r1, [pc, #228]	; (f8035c8 <BOOT_RAM_TZCfg+0x108>)
 f8034e4:	9303      	str	r3, [sp, #12]
 f8034e6:	2300      	movs	r3, #0
 f8034e8:	6810      	ldr	r0, [r2, #0]
 f8034ea:	fa05 f803 	lsl.w	r8, r5, r3
 f8034ee:	eb01 06c3 	add.w	r6, r1, r3, lsl #3
 f8034f2:	f1b0 3fff 	cmp.w	r0, #4294967295	; 0xffffffff
 f8034f6:	4644      	mov	r4, r8
 f8034f8:	d015      	beq.n	f803526 <BOOT_RAM_TZCfg+0x66>
 f8034fa:	f841 0033 	str.w	r0, [r1, r3, lsl #3]
 f8034fe:	3301      	adds	r3, #1
 f803500:	6850      	ldr	r0, [r2, #4]
 f803502:	6070      	str	r0, [r6, #4]
 f803504:	6890      	ldr	r0, [r2, #8]
 f803506:	03c6      	lsls	r6, r0, #15
 f803508:	d504      	bpl.n	f803514 <BOOT_RAM_TZCfg+0x54>
 f80350a:	6c08      	ldr	r0, [r1, #64]	; 0x40
 f80350c:	ea40 4008 	orr.w	r0, r0, r8, lsl #16
 f803510:	6408      	str	r0, [r1, #64]	; 0x40
 f803512:	6890      	ldr	r0, [r2, #8]
 f803514:	07c0      	lsls	r0, r0, #31
 f803516:	f102 020c 	add.w	r2, r2, #12
 f80351a:	d502      	bpl.n	f803522 <BOOT_RAM_TZCfg+0x62>
 f80351c:	6c08      	ldr	r0, [r1, #64]	; 0x40
 f80351e:	4304      	orrs	r4, r0
 f803520:	640c      	str	r4, [r1, #64]	; 0x40
 f803522:	2b08      	cmp	r3, #8
 f803524:	d1e0      	bne.n	f8034e8 <BOOT_RAM_TZCfg+0x28>
 f803526:	45f4      	cmp	ip, lr
 f803528:	644d      	str	r5, [r1, #68]	; 0x44
 f80352a:	d033      	beq.n	f803594 <BOOT_RAM_TZCfg+0xd4>
 f80352c:	f85c 1b04 	ldr.w	r1, [ip], #4
 f803530:	f857 2b04 	ldr.w	r2, [r7], #4
 f803534:	e7d7      	b.n	f8034e6 <BOOT_RAM_TZCfg+0x26>
 f803536:	4924      	ldr	r1, [pc, #144]	; (f8035c8 <BOOT_RAM_TZCfg+0x108>)
 f803538:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
 f80353c:	2401      	movs	r4, #1
 f80353e:	4a1f      	ldr	r2, [pc, #124]	; (f8035bc <BOOT_RAM_TZCfg+0xfc>)
 f803540:	6008      	str	r0, [r1, #0]
 f803542:	604b      	str	r3, [r1, #4]
 f803544:	6c0d      	ldr	r5, [r1, #64]	; 0x40
 f803546:	4325      	orrs	r5, r4
 f803548:	640d      	str	r5, [r1, #64]	; 0x40
 f80354a:	644c      	str	r4, [r1, #68]	; 0x44
 f80354c:	6010      	str	r0, [r2, #0]
 f80354e:	6053      	str	r3, [r2, #4]
 f803550:	6c10      	ldr	r0, [r2, #64]	; 0x40
 f803552:	4320      	orrs	r0, r4
 f803554:	6410      	str	r0, [r2, #64]	; 0x40
 f803556:	6454      	str	r4, [r2, #68]	; 0x44
 f803558:	4a1c      	ldr	r2, [pc, #112]	; (f8035cc <BOOT_RAM_TZCfg+0x10c>)
 f80355a:	f8c1 3200 	str.w	r3, [r1, #512]	; 0x200
 f80355e:	481c      	ldr	r0, [pc, #112]	; (f8035d0 <BOOT_RAM_TZCfg+0x110>)
 f803560:	f8c1 3204 	str.w	r3, [r1, #516]	; 0x204
 f803564:	f8c1 3208 	str.w	r3, [r1, #520]	; 0x208
 f803568:	f8c1 420c 	str.w	r4, [r1, #524]	; 0x20c
 f80356c:	f8c2 4398 	str.w	r4, [r2, #920]	; 0x398
 f803570:	f7ff ff2c 	bl	f8033cc <BOOT_CPU_TZCfg>
 f803574:	4b17      	ldr	r3, [pc, #92]	; (f8035d4 <BOOT_RAM_TZCfg+0x114>)
 f803576:	4798      	blx	r3
 f803578:	b1e8      	cbz	r0, f8035b6 <BOOT_RAM_TZCfg+0xf6>
 f80357a:	4e17      	ldr	r6, [pc, #92]	; (f8035d8 <BOOT_RAM_TZCfg+0x118>)
 f80357c:	4d17      	ldr	r5, [pc, #92]	; (f8035dc <BOOT_RAM_TZCfg+0x11c>)
 f80357e:	4c18      	ldr	r4, [pc, #96]	; (f8035e0 <BOOT_RAM_TZCfg+0x120>)
 f803580:	462b      	mov	r3, r5
 f803582:	2245      	movs	r2, #69	; 0x45
 f803584:	4621      	mov	r1, r4
 f803586:	2002      	movs	r0, #2
 f803588:	f7fe ffc6 	bl	f802518 <rtk_log_write>
 f80358c:	f44f 707a 	mov.w	r0, #1000	; 0x3e8
 f803590:	47b0      	blx	r6
 f803592:	e7f5      	b.n	f803580 <BOOT_RAM_TZCfg+0xc0>
 f803594:	4b13      	ldr	r3, [pc, #76]	; (f8035e4 <BOOT_RAM_TZCfg+0x124>)
 f803596:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
 f80359a:	f06f 0004 	mvn.w	r0, #4
 f80359e:	490b      	ldr	r1, [pc, #44]	; (f8035cc <BOOT_RAM_TZCfg+0x10c>)
 f8035a0:	601a      	str	r2, [r3, #0]
 f8035a2:	605a      	str	r2, [r3, #4]
 f8035a4:	6098      	str	r0, [r3, #8]
 f8035a6:	4810      	ldr	r0, [pc, #64]	; (f8035e8 <BOOT_RAM_TZCfg+0x128>)
 f8035a8:	f8c1 5398 	str.w	r5, [r1, #920]	; 0x398
 f8035ac:	b004      	add	sp, #16
 f8035ae:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
 f8035b2:	f7ff bf0b 	b.w	f8033cc <BOOT_CPU_TZCfg>
 f8035b6:	b004      	add	sp, #16
 f8035b8:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 f8035bc:	51102300 	.word	0x51102300
 f8035c0:	0f805428 	.word	0x0f805428
 f8035c4:	0f80541c 	.word	0x0f80541c
 f8035c8:	51102200 	.word	0x51102200
 f8035cc:	50110000 	.word	0x50110000
 f8035d0:	0f805404 	.word	0x0f805404
 f8035d4:	0000c229 	.word	0x0000c229
 f8035d8:	0000951d 	.word	0x0000951d
 f8035dc:	0f8053d4 	.word	0x0f8053d4
 f8035e0:	0f804988 	.word	0x0f804988
 f8035e4:	51102400 	.word	0x51102400
 f8035e8:	0f805434 	.word	0x0f805434

0f8035ec <crash_dump_memory>:
 f8035ec:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 f8035f0:	460e      	mov	r6, r1
 f8035f2:	4680      	mov	r8, r0
 f8035f4:	4b21      	ldr	r3, [pc, #132]	; (f80367c <crash_dump_memory+0x90>)
 f8035f6:	f1a6 0080 	sub.w	r0, r6, #128	; 0x80
 f8035fa:	4a21      	ldr	r2, [pc, #132]	; (f803680 <crash_dump_memory+0x94>)
 f8035fc:	f8d3 1228 	ldr.w	r1, [r3, #552]	; 0x228
 f803600:	4644      	mov	r4, r8
 f803602:	f5b0 5f7e 	cmp.w	r0, #16256	; 0x3f80
 f803606:	4b1f      	ldr	r3, [pc, #124]	; (f803684 <crash_dump_memory+0x98>)
 f803608:	481f      	ldr	r0, [pc, #124]	; (f803688 <crash_dump_memory+0x9c>)
 f80360a:	bf88      	it	hi
 f80360c:	2680      	movhi	r6, #128	; 0x80
 f80360e:	f011 0fc0 	tst.w	r1, #192	; 0xc0
 f803612:	4f1e      	ldr	r7, [pc, #120]	; (f80368c <crash_dump_memory+0xa0>)
 f803614:	bf14      	ite	ne
 f803616:	4611      	movne	r1, r2
 f803618:	4619      	moveq	r1, r3
 f80361a:	f7ff fc15 	bl	f802e48 <DiagPrintf>
 f80361e:	4632      	mov	r2, r6
 f803620:	4641      	mov	r1, r8
 f803622:	481b      	ldr	r0, [pc, #108]	; (f803690 <crash_dump_memory+0xa4>)
 f803624:	f7ff fc10 	bl	f802e48 <DiagPrintf>
 f803628:	f8df 906c 	ldr.w	r9, [pc, #108]	; f803698 <crash_dump_memory+0xac>
 f80362c:	2300      	movs	r3, #0
 f80362e:	f8df a06c 	ldr.w	sl, [pc, #108]	; f80369c <crash_dump_memory+0xb0>
 f803632:	e007      	b.n	f803644 <crash_dump_memory+0x58>
 f803634:	6821      	ldr	r1, [r4, #0]
 f803636:	4638      	mov	r0, r7
 f803638:	f7ff fc06 	bl	f802e48 <DiagPrintf>
 f80363c:	42ae      	cmp	r6, r5
 f80363e:	d00f      	beq.n	f803660 <crash_dump_memory+0x74>
 f803640:	3404      	adds	r4, #4
 f803642:	462b      	mov	r3, r5
 f803644:	1c5d      	adds	r5, r3, #1
 f803646:	b183      	cbz	r3, f80366a <crash_dump_memory+0x7e>
 f803648:	079b      	lsls	r3, r3, #30
 f80364a:	d1f3      	bne.n	f803634 <crash_dump_memory+0x48>
 f80364c:	4621      	mov	r1, r4
 f80364e:	4648      	mov	r0, r9
 f803650:	f7ff fbfa 	bl	f802e48 <DiagPrintf>
 f803654:	6821      	ldr	r1, [r4, #0]
 f803656:	4638      	mov	r0, r7
 f803658:	f7ff fbf6 	bl	f802e48 <DiagPrintf>
 f80365c:	42ae      	cmp	r6, r5
 f80365e:	d1ef      	bne.n	f803640 <crash_dump_memory+0x54>
 f803660:	480c      	ldr	r0, [pc, #48]	; (f803694 <crash_dump_memory+0xa8>)
 f803662:	e8bd 47f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 f803666:	f7ff bbef 	b.w	f802e48 <DiagPrintf>
 f80366a:	4641      	mov	r1, r8
 f80366c:	4650      	mov	r0, sl
 f80366e:	f7ff fbeb 	bl	f802e48 <DiagPrintf>
 f803672:	6821      	ldr	r1, [r4, #0]
 f803674:	4638      	mov	r0, r7
 f803676:	f7ff fbe7 	bl	f802e48 <DiagPrintf>
 f80367a:	e7e1      	b.n	f803640 <crash_dump_memory+0x54>
 f80367c:	41008000 	.word	0x41008000
 f803680:	0f805440 	.word	0x0f805440
 f803684:	0f805444 	.word	0x0f805444
 f803688:	0f805448 	.word	0x0f805448
 f80368c:	0f8054ac 	.word	0x0f8054ac
 f803690:	0f80546c 	.word	0x0f80546c
 f803694:	0f804d7c 	.word	0x0f804d7c
 f803698:	0f8054b4 	.word	0x0f8054b4
 f80369c:	0f8054a4 	.word	0x0f8054a4

0f8036a0 <crash_dump>:
 f8036a0:	e92d 4ff8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
 f8036a4:	4b51      	ldr	r3, [pc, #324]	; (f8037ec <crash_dump+0x14c>)
 f8036a6:	4614      	mov	r4, r2
 f8036a8:	4688      	mov	r8, r1
 f8036aa:	f04f 25e0 	mov.w	r5, #3758153728	; 0xe000e000
 f8036ae:	4850      	ldr	r0, [pc, #320]	; (f8037f0 <crash_dump+0x150>)
 f8036b0:	e9d2 2109 	ldrd	r2, r1, [r2, #36]	; 0x24
 f8036b4:	e9c3 2100 	strd	r2, r1, [r3]
 f8036b8:	6ae2      	ldr	r2, [r4, #44]	; 0x2c
 f8036ba:	609a      	str	r2, [r3, #8]
 f8036bc:	6b22      	ldr	r2, [r4, #48]	; 0x30
 f8036be:	60da      	str	r2, [r3, #12]
 f8036c0:	6862      	ldr	r2, [r4, #4]
 f8036c2:	611a      	str	r2, [r3, #16]
 f8036c4:	68a2      	ldr	r2, [r4, #8]
 f8036c6:	615a      	str	r2, [r3, #20]
 f8036c8:	68e2      	ldr	r2, [r4, #12]
 f8036ca:	619a      	str	r2, [r3, #24]
 f8036cc:	6922      	ldr	r2, [r4, #16]
 f8036ce:	61da      	str	r2, [r3, #28]
 f8036d0:	6962      	ldr	r2, [r4, #20]
 f8036d2:	621a      	str	r2, [r3, #32]
 f8036d4:	69a2      	ldr	r2, [r4, #24]
 f8036d6:	625a      	str	r2, [r3, #36]	; 0x24
 f8036d8:	69e2      	ldr	r2, [r4, #28]
 f8036da:	629a      	str	r2, [r3, #40]	; 0x28
 f8036dc:	6a22      	ldr	r2, [r4, #32]
 f8036de:	62da      	str	r2, [r3, #44]	; 0x2c
 f8036e0:	6b62      	ldr	r2, [r4, #52]	; 0x34
 f8036e2:	631a      	str	r2, [r3, #48]	; 0x30
 f8036e4:	6ba2      	ldr	r2, [r4, #56]	; 0x38
 f8036e6:	635a      	str	r2, [r3, #52]	; 0x34
 f8036e8:	6be2      	ldr	r2, [r4, #60]	; 0x3c
 f8036ea:	639a      	str	r2, [r3, #56]	; 0x38
 f8036ec:	6c22      	ldr	r2, [r4, #64]	; 0x40
 f8036ee:	63da      	str	r2, [r3, #60]	; 0x3c
 f8036f0:	6822      	ldr	r2, [r4, #0]
 f8036f2:	641a      	str	r2, [r3, #64]	; 0x40
 f8036f4:	f8d5 2d24 	ldr.w	r2, [r5, #3364]	; 0xd24
 f8036f8:	645a      	str	r2, [r3, #68]	; 0x44
 f8036fa:	4a3e      	ldr	r2, [pc, #248]	; (f8037f4 <crash_dump+0x154>)
 f8036fc:	f892 1028 	ldrb.w	r1, [r2, #40]	; 0x28
 f803700:	f883 1048 	strb.w	r1, [r3, #72]	; 0x48
 f803704:	f8d5 1d34 	ldr.w	r1, [r5, #3380]	; 0xd34
 f803708:	64d9      	str	r1, [r3, #76]	; 0x4c
 f80370a:	f892 1029 	ldrb.w	r1, [r2, #41]	; 0x29
 f80370e:	f883 1050 	strb.w	r1, [r3, #80]	; 0x50
 f803712:	f8d5 1d38 	ldr.w	r1, [r5, #3384]	; 0xd38
 f803716:	6559      	str	r1, [r3, #84]	; 0x54
 f803718:	8d51      	ldrh	r1, [r2, #42]	; 0x2a
 f80371a:	f8a3 1058 	strh.w	r1, [r3, #88]	; 0x58
 f80371e:	f8d5 1d2c 	ldr.w	r1, [r5, #3372]	; 0xd2c
 f803722:	65d9      	str	r1, [r3, #92]	; 0x5c
 f803724:	8e11      	ldrh	r1, [r2, #48]	; 0x30
 f803726:	8f92      	ldrh	r2, [r2, #60]	; 0x3c
 f803728:	b289      	uxth	r1, r1
 f80372a:	b292      	uxth	r2, r2
 f80372c:	6619      	str	r1, [r3, #96]	; 0x60
 f80372e:	665a      	str	r2, [r3, #100]	; 0x64
 f803730:	f8d5 2de4 	ldr.w	r2, [r5, #3556]	; 0xde4
 f803734:	669a      	str	r2, [r3, #104]	; 0x68
 f803736:	f8d5 2de8 	ldr.w	r2, [r5, #3560]	; 0xde8
 f80373a:	66da      	str	r2, [r3, #108]	; 0x6c
 f80373c:	f7ff fb84 	bl	f802e48 <DiagPrintf>
 f803740:	4b2d      	ldr	r3, [pc, #180]	; (f8037f8 <crash_dump+0x158>)
 f803742:	681b      	ldr	r3, [r3, #0]
 f803744:	b10b      	cbz	r3, f80374a <crash_dump+0xaa>
 f803746:	4620      	mov	r0, r4
 f803748:	4798      	blx	r3
 f80374a:	4828      	ldr	r0, [pc, #160]	; (f8037ec <crash_dump+0x14c>)
 f80374c:	4627      	mov	r7, r4
 f80374e:	f000 f86f 	bl	f803830 <fault_diagnosis>
 f803752:	482a      	ldr	r0, [pc, #168]	; (f8037fc <crash_dump+0x15c>)
 f803754:	f7ff fb78 	bl	f802e48 <DiagPrintf>
 f803758:	6ba1      	ldr	r1, [r4, #56]	; 0x38
 f80375a:	4829      	ldr	r0, [pc, #164]	; (f803800 <crash_dump+0x160>)
 f80375c:	f104 0a30 	add.w	sl, r4, #48	; 0x30
 f803760:	f7ff fb72 	bl	f802e48 <DiagPrintf>
 f803764:	6be1      	ldr	r1, [r4, #60]	; 0x3c
 f803766:	4827      	ldr	r0, [pc, #156]	; (f803804 <crash_dump+0x164>)
 f803768:	4625      	mov	r5, r4
 f80376a:	f7ff fb6d 	bl	f802e48 <DiagPrintf>
 f80376e:	6c21      	ldr	r1, [r4, #64]	; 0x40
 f803770:	4825      	ldr	r0, [pc, #148]	; (f803808 <crash_dump+0x168>)
 f803772:	f7ff fb69 	bl	f802e48 <DiagPrintf>
 f803776:	f857 1b20 	ldr.w	r1, [r7], #32
 f80377a:	4824      	ldr	r0, [pc, #144]	; (f80380c <crash_dump+0x16c>)
 f80377c:	463e      	mov	r6, r7
 f80377e:	f8df b0a0 	ldr.w	fp, [pc, #160]	; f803820 <crash_dump+0x180>
 f803782:	f7ff fb61 	bl	f802e48 <DiagPrintf>
 f803786:	4922      	ldr	r1, [pc, #136]	; (f803810 <crash_dump+0x170>)
 f803788:	f856 2f04 	ldr.w	r2, [r6, #4]!
 f80378c:	4658      	mov	r0, fp
 f80378e:	f7ff fb5b 	bl	f802e48 <DiagPrintf>
 f803792:	f8df 9098 	ldr.w	r9, [pc, #152]	; f80382c <crash_dump+0x18c>
 f803796:	4556      	cmp	r6, sl
 f803798:	d008      	beq.n	f8037ac <crash_dump+0x10c>
 f80379a:	f856 2f04 	ldr.w	r2, [r6, #4]!
 f80379e:	4658      	mov	r0, fp
 f8037a0:	f859 1b04 	ldr.w	r1, [r9], #4
 f8037a4:	f7ff fb50 	bl	f802e48 <DiagPrintf>
 f8037a8:	4556      	cmp	r6, sl
 f8037aa:	d1f6      	bne.n	f80379a <crash_dump+0xfa>
 f8037ac:	4e19      	ldr	r6, [pc, #100]	; (f803814 <crash_dump+0x174>)
 f8037ae:	491a      	ldr	r1, [pc, #104]	; (f803818 <crash_dump+0x178>)
 f8037b0:	f8df 906c 	ldr.w	r9, [pc, #108]	; f803820 <crash_dump+0x180>
 f8037b4:	e001      	b.n	f8037ba <crash_dump+0x11a>
 f8037b6:	f856 1b04 	ldr.w	r1, [r6], #4
 f8037ba:	f855 2f04 	ldr.w	r2, [r5, #4]!
 f8037be:	4648      	mov	r0, r9
 f8037c0:	f7ff fb42 	bl	f802e48 <DiagPrintf>
 f8037c4:	42bd      	cmp	r5, r7
 f8037c6:	d1f6      	bne.n	f8037b6 <crash_dump+0x116>
 f8037c8:	6b62      	ldr	r2, [r4, #52]	; 0x34
 f8037ca:	4914      	ldr	r1, [pc, #80]	; (f80381c <crash_dump+0x17c>)
 f8037cc:	4814      	ldr	r0, [pc, #80]	; (f803820 <crash_dump+0x180>)
 f8037ce:	f7ff fb3b 	bl	f802e48 <DiagPrintf>
 f8037d2:	2180      	movs	r1, #128	; 0x80
 f8037d4:	4640      	mov	r0, r8
 f8037d6:	f7ff ff09 	bl	f8035ec <crash_dump_memory>
 f8037da:	4812      	ldr	r0, [pc, #72]	; (f803824 <crash_dump+0x184>)
 f8037dc:	f7ff fb34 	bl	f802e48 <DiagPrintf>
 f8037e0:	4811      	ldr	r0, [pc, #68]	; (f803828 <crash_dump+0x188>)
 f8037e2:	f7ff fb31 	bl	f802e48 <DiagPrintf>
 f8037e6:	2000      	movs	r0, #0
 f8037e8:	e8bd 8ff8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, pc}
 f8037ec:	3000a974 	.word	0x3000a974
 f8037f0:	0f8054c8 	.word	0x0f8054c8
 f8037f4:	e000ed00 	.word	0xe000ed00
 f8037f8:	3000a970 	.word	0x3000a970
 f8037fc:	0f8054ec 	.word	0x0f8054ec
 f803800:	0f805514 	.word	0x0f805514
 f803804:	0f805524 	.word	0x0f805524
 f803808:	0f805534 	.word	0x0f805534
 f80380c:	0f805544 	.word	0x0f805544
 f803810:	0f8054c0 	.word	0x0f8054c0
 f803814:	0f8055cc 	.word	0x0f8055cc
 f803818:	0f8054c4 	.word	0x0f8054c4
 f80381c:	0f805564 	.word	0x0f805564
 f803820:	0f805554 	.word	0x0f805554
 f803824:	0f805568 	.word	0x0f805568
 f803828:	0f805594 	.word	0x0f805594
 f80382c:	0f8055ec 	.word	0x0f8055ec

0f803830 <fault_diagnosis>:
 f803830:	b510      	push	{r4, lr}
 f803832:	4604      	mov	r4, r0
 f803834:	4888      	ldr	r0, [pc, #544]	; (f803a58 <fault_diagnosis+0x228>)
 f803836:	6ba1      	ldr	r1, [r4, #56]	; 0x38
 f803838:	f7ff fb06 	bl	f802e48 <DiagPrintf>
 f80383c:	f894 305c 	ldrb.w	r3, [r4, #92]	; 0x5c
 f803840:	079b      	lsls	r3, r3, #30
 f803842:	f100 808a 	bmi.w	f80395a <fault_diagnosis+0x12a>
 f803846:	6ea1      	ldr	r1, [r4, #104]	; 0x68
 f803848:	b1e1      	cbz	r1, f803884 <fault_diagnosis+0x54>
 f80384a:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 f80384e:	07d8      	lsls	r0, r3, #31
 f803850:	f100 8094 	bmi.w	f80397c <fault_diagnosis+0x14c>
 f803854:	079a      	lsls	r2, r3, #30
 f803856:	f100 809a 	bmi.w	f80398e <fault_diagnosis+0x15e>
 f80385a:	0758      	lsls	r0, r3, #29
 f80385c:	f100 80a1 	bmi.w	f8039a2 <fault_diagnosis+0x172>
 f803860:	0719      	lsls	r1, r3, #28
 f803862:	f100 80a8 	bmi.w	f8039b6 <fault_diagnosis+0x186>
 f803866:	06da      	lsls	r2, r3, #27
 f803868:	f100 80af 	bmi.w	f8039ca <fault_diagnosis+0x19a>
 f80386c:	0698      	lsls	r0, r3, #26
 f80386e:	f100 80b6 	bmi.w	f8039de <fault_diagnosis+0x1ae>
 f803872:	0619      	lsls	r1, r3, #24
 f803874:	f100 80bd 	bmi.w	f8039f2 <fault_diagnosis+0x1c2>
 f803878:	065a      	lsls	r2, r3, #25
 f80387a:	d503      	bpl.n	f803884 <fault_diagnosis+0x54>
 f80387c:	f013 0f28 	tst.w	r3, #40	; 0x28
 f803880:	f040 8186 	bne.w	f803b90 <fault_diagnosis+0x360>
 f803884:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
 f803888:	b199      	cbz	r1, f8038b2 <fault_diagnosis+0x82>
 f80388a:	07cb      	lsls	r3, r1, #31
 f80388c:	f100 80b9 	bmi.w	f803a02 <fault_diagnosis+0x1d2>
 f803890:	0788      	lsls	r0, r1, #30
 f803892:	f100 80bf 	bmi.w	f803a14 <fault_diagnosis+0x1e4>
 f803896:	070a      	lsls	r2, r1, #28
 f803898:	f100 80c5 	bmi.w	f803a26 <fault_diagnosis+0x1f6>
 f80389c:	06cb      	lsls	r3, r1, #27
 f80389e:	f100 80cb 	bmi.w	f803a38 <fault_diagnosis+0x208>
 f8038a2:	0688      	lsls	r0, r1, #26
 f8038a4:	f100 80d1 	bmi.w	f803a4a <fault_diagnosis+0x21a>
 f8038a8:	060a      	lsls	r2, r1, #24
 f8038aa:	d502      	bpl.n	f8038b2 <fault_diagnosis+0x82>
 f8038ac:	078b      	lsls	r3, r1, #30
 f8038ae:	f040 8194 	bne.w	f803bda <fault_diagnosis+0x3aa>
 f8038b2:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
 f8038b6:	b1b1      	cbz	r1, f8038e6 <fault_diagnosis+0xb6>
 f8038b8:	07c8      	lsls	r0, r1, #31
 f8038ba:	f100 80f7 	bmi.w	f803aac <fault_diagnosis+0x27c>
 f8038be:	078a      	lsls	r2, r1, #30
 f8038c0:	f100 80fd 	bmi.w	f803abe <fault_diagnosis+0x28e>
 f8038c4:	074b      	lsls	r3, r1, #29
 f8038c6:	f100 8103 	bmi.w	f803ad0 <fault_diagnosis+0x2a0>
 f8038ca:	0708      	lsls	r0, r1, #28
 f8038cc:	f100 8109 	bmi.w	f803ae2 <fault_diagnosis+0x2b2>
 f8038d0:	06ca      	lsls	r2, r1, #27
 f8038d2:	f100 810f 	bmi.w	f803af4 <fault_diagnosis+0x2c4>
 f8038d6:	068b      	lsls	r3, r1, #26
 f8038d8:	f100 8115 	bmi.w	f803b06 <fault_diagnosis+0x2d6>
 f8038dc:	0608      	lsls	r0, r1, #24
 f8038de:	d502      	bpl.n	f8038e6 <fault_diagnosis+0xb6>
 f8038e0:	0789      	lsls	r1, r1, #30
 f8038e2:	f100 8180 	bmi.w	f803be6 <fault_diagnosis+0x3b6>
 f8038e6:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
 f8038ea:	b1b9      	cbz	r1, f80391c <fault_diagnosis+0xec>
 f8038ec:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
 f8038f0:	07da      	lsls	r2, r3, #31
 f8038f2:	f100 810f 	bmi.w	f803b14 <fault_diagnosis+0x2e4>
 f8038f6:	0798      	lsls	r0, r3, #30
 f8038f8:	f100 8115 	bmi.w	f803b26 <fault_diagnosis+0x2f6>
 f8038fc:	0759      	lsls	r1, r3, #29
 f8038fe:	f100 811d 	bmi.w	f803b3c <fault_diagnosis+0x30c>
 f803902:	071a      	lsls	r2, r3, #28
 f803904:	f100 8125 	bmi.w	f803b52 <fault_diagnosis+0x322>
 f803908:	06db      	lsls	r3, r3, #27
 f80390a:	f100 812d 	bmi.w	f803b68 <fault_diagnosis+0x338>
 f80390e:	f894 3059 	ldrb.w	r3, [r4, #89]	; 0x59
 f803912:	07d8      	lsls	r0, r3, #31
 f803914:	f100 8133 	bmi.w	f803b7e <fault_diagnosis+0x34e>
 f803918:	0799      	lsls	r1, r3, #30
 f80391a:	d424      	bmi.n	f803966 <fault_diagnosis+0x136>
 f80391c:	f994 305f 	ldrsb.w	r3, [r4, #95]	; 0x5f
 f803920:	2b00      	cmp	r3, #0
 f803922:	db00      	blt.n	f803926 <fault_diagnosis+0xf6>
 f803924:	bd10      	pop	{r4, pc}
 f803926:	6e21      	ldr	r1, [r4, #96]	; 0x60
 f803928:	2900      	cmp	r1, #0
 f80392a:	d0fb      	beq.n	f803924 <fault_diagnosis+0xf4>
 f80392c:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
 f803930:	07da      	lsls	r2, r3, #31
 f803932:	f100 8143 	bmi.w	f803bbc <fault_diagnosis+0x38c>
 f803936:	0798      	lsls	r0, r3, #30
 f803938:	f100 8138 	bmi.w	f803bac <fault_diagnosis+0x37c>
 f80393c:	0759      	lsls	r1, r3, #29
 f80393e:	f100 812d 	bmi.w	f803b9c <fault_diagnosis+0x36c>
 f803942:	071a      	lsls	r2, r3, #28
 f803944:	f100 8141 	bmi.w	f803bca <fault_diagnosis+0x39a>
 f803948:	06db      	lsls	r3, r3, #27
 f80394a:	d5eb      	bpl.n	f803924 <fault_diagnosis+0xf4>
 f80394c:	6e21      	ldr	r1, [r4, #96]	; 0x60
 f80394e:	4a43      	ldr	r2, [pc, #268]	; (f803a5c <fault_diagnosis+0x22c>)
 f803950:	4843      	ldr	r0, [pc, #268]	; (f803a60 <fault_diagnosis+0x230>)
 f803952:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
 f803956:	f7ff ba77 	b.w	f802e48 <DiagPrintf>
 f80395a:	4a42      	ldr	r2, [pc, #264]	; (f803a64 <fault_diagnosis+0x234>)
 f80395c:	6de1      	ldr	r1, [r4, #92]	; 0x5c
 f80395e:	4842      	ldr	r0, [pc, #264]	; (f803a68 <fault_diagnosis+0x238>)
 f803960:	f7ff fa72 	bl	f802e48 <DiagPrintf>
 f803964:	e76f      	b.n	f803846 <fault_diagnosis+0x16>
 f803966:	4a41      	ldr	r2, [pc, #260]	; (f803a6c <fault_diagnosis+0x23c>)
 f803968:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
 f80396c:	4840      	ldr	r0, [pc, #256]	; (f803a70 <fault_diagnosis+0x240>)
 f80396e:	f7ff fa6b 	bl	f802e48 <DiagPrintf>
 f803972:	f994 305f 	ldrsb.w	r3, [r4, #95]	; 0x5f
 f803976:	2b00      	cmp	r3, #0
 f803978:	dad4      	bge.n	f803924 <fault_diagnosis+0xf4>
 f80397a:	e7d4      	b.n	f803926 <fault_diagnosis+0xf6>
 f80397c:	4a3d      	ldr	r2, [pc, #244]	; (f803a74 <fault_diagnosis+0x244>)
 f80397e:	483e      	ldr	r0, [pc, #248]	; (f803a78 <fault_diagnosis+0x248>)
 f803980:	f7ff fa62 	bl	f802e48 <DiagPrintf>
 f803984:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 f803988:	079a      	lsls	r2, r3, #30
 f80398a:	f57f af66 	bpl.w	f80385a <fault_diagnosis+0x2a>
 f80398e:	4a3b      	ldr	r2, [pc, #236]	; (f803a7c <fault_diagnosis+0x24c>)
 f803990:	6ea1      	ldr	r1, [r4, #104]	; 0x68
 f803992:	4839      	ldr	r0, [pc, #228]	; (f803a78 <fault_diagnosis+0x248>)
 f803994:	f7ff fa58 	bl	f802e48 <DiagPrintf>
 f803998:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 f80399c:	0758      	lsls	r0, r3, #29
 f80399e:	f57f af5f 	bpl.w	f803860 <fault_diagnosis+0x30>
 f8039a2:	6ea1      	ldr	r1, [r4, #104]	; 0x68
 f8039a4:	4a36      	ldr	r2, [pc, #216]	; (f803a80 <fault_diagnosis+0x250>)
 f8039a6:	4834      	ldr	r0, [pc, #208]	; (f803a78 <fault_diagnosis+0x248>)
 f8039a8:	f7ff fa4e 	bl	f802e48 <DiagPrintf>
 f8039ac:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 f8039b0:	0719      	lsls	r1, r3, #28
 f8039b2:	f57f af58 	bpl.w	f803866 <fault_diagnosis+0x36>
 f8039b6:	4a33      	ldr	r2, [pc, #204]	; (f803a84 <fault_diagnosis+0x254>)
 f8039b8:	6ea1      	ldr	r1, [r4, #104]	; 0x68
 f8039ba:	482f      	ldr	r0, [pc, #188]	; (f803a78 <fault_diagnosis+0x248>)
 f8039bc:	f7ff fa44 	bl	f802e48 <DiagPrintf>
 f8039c0:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 f8039c4:	06da      	lsls	r2, r3, #27
 f8039c6:	f57f af51 	bpl.w	f80386c <fault_diagnosis+0x3c>
 f8039ca:	4a2f      	ldr	r2, [pc, #188]	; (f803a88 <fault_diagnosis+0x258>)
 f8039cc:	6ea1      	ldr	r1, [r4, #104]	; 0x68
 f8039ce:	482a      	ldr	r0, [pc, #168]	; (f803a78 <fault_diagnosis+0x248>)
 f8039d0:	f7ff fa3a 	bl	f802e48 <DiagPrintf>
 f8039d4:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 f8039d8:	0698      	lsls	r0, r3, #26
 f8039da:	f57f af4a 	bpl.w	f803872 <fault_diagnosis+0x42>
 f8039de:	6ea1      	ldr	r1, [r4, #104]	; 0x68
 f8039e0:	4a2a      	ldr	r2, [pc, #168]	; (f803a8c <fault_diagnosis+0x25c>)
 f8039e2:	4825      	ldr	r0, [pc, #148]	; (f803a78 <fault_diagnosis+0x248>)
 f8039e4:	f7ff fa30 	bl	f802e48 <DiagPrintf>
 f8039e8:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 f8039ec:	0619      	lsls	r1, r3, #24
 f8039ee:	f57f af43 	bpl.w	f803878 <fault_diagnosis+0x48>
 f8039f2:	4a27      	ldr	r2, [pc, #156]	; (f803a90 <fault_diagnosis+0x260>)
 f8039f4:	6ea1      	ldr	r1, [r4, #104]	; 0x68
 f8039f6:	4820      	ldr	r0, [pc, #128]	; (f803a78 <fault_diagnosis+0x248>)
 f8039f8:	f7ff fa26 	bl	f802e48 <DiagPrintf>
 f8039fc:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 f803a00:	e73a      	b.n	f803878 <fault_diagnosis+0x48>
 f803a02:	4a24      	ldr	r2, [pc, #144]	; (f803a94 <fault_diagnosis+0x264>)
 f803a04:	4824      	ldr	r0, [pc, #144]	; (f803a98 <fault_diagnosis+0x268>)
 f803a06:	f7ff fa1f 	bl	f802e48 <DiagPrintf>
 f803a0a:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
 f803a0e:	0788      	lsls	r0, r1, #30
 f803a10:	f57f af41 	bpl.w	f803896 <fault_diagnosis+0x66>
 f803a14:	4a21      	ldr	r2, [pc, #132]	; (f803a9c <fault_diagnosis+0x26c>)
 f803a16:	4820      	ldr	r0, [pc, #128]	; (f803a98 <fault_diagnosis+0x268>)
 f803a18:	f7ff fa16 	bl	f802e48 <DiagPrintf>
 f803a1c:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
 f803a20:	070a      	lsls	r2, r1, #28
 f803a22:	f57f af3b 	bpl.w	f80389c <fault_diagnosis+0x6c>
 f803a26:	4a1e      	ldr	r2, [pc, #120]	; (f803aa0 <fault_diagnosis+0x270>)
 f803a28:	481b      	ldr	r0, [pc, #108]	; (f803a98 <fault_diagnosis+0x268>)
 f803a2a:	f7ff fa0d 	bl	f802e48 <DiagPrintf>
 f803a2e:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
 f803a32:	06cb      	lsls	r3, r1, #27
 f803a34:	f57f af35 	bpl.w	f8038a2 <fault_diagnosis+0x72>
 f803a38:	4a1a      	ldr	r2, [pc, #104]	; (f803aa4 <fault_diagnosis+0x274>)
 f803a3a:	4817      	ldr	r0, [pc, #92]	; (f803a98 <fault_diagnosis+0x268>)
 f803a3c:	f7ff fa04 	bl	f802e48 <DiagPrintf>
 f803a40:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
 f803a44:	0688      	lsls	r0, r1, #26
 f803a46:	f57f af2f 	bpl.w	f8038a8 <fault_diagnosis+0x78>
 f803a4a:	4a17      	ldr	r2, [pc, #92]	; (f803aa8 <fault_diagnosis+0x278>)
 f803a4c:	4812      	ldr	r0, [pc, #72]	; (f803a98 <fault_diagnosis+0x268>)
 f803a4e:	f7ff f9fb 	bl	f802e48 <DiagPrintf>
 f803a52:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
 f803a56:	e727      	b.n	f8038a8 <fault_diagnosis+0x78>
 f803a58:	0f8055fc 	.word	0x0f8055fc
 f803a5c:	0f805e8c 	.word	0x0f805e8c
 f803a60:	0f805de4 	.word	0x0f805de4
 f803a64:	0f805614 	.word	0x0f805614
 f803a68:	0f805640 	.word	0x0f805640
 f803a6c:	0f805d48 	.word	0x0f805d48
 f803a70:	0f805b8c 	.word	0x0f805b8c
 f803a74:	0f805658 	.word	0x0f805658
 f803a78:	0f805688 	.word	0x0f805688
 f803a7c:	0f8056a0 	.word	0x0f8056a0
 f803a80:	0f8056d8 	.word	0x0f8056d8
 f803a84:	0f805710 	.word	0x0f805710
 f803a88:	0f805748 	.word	0x0f805748
 f803a8c:	0f805778 	.word	0x0f805778
 f803a90:	0f8057b4 	.word	0x0f8057b4
 f803a94:	0f805824 	.word	0x0f805824
 f803a98:	0f805868 	.word	0x0f805868
 f803a9c:	0f805880 	.word	0x0f805880
 f803aa0:	0f8058bc 	.word	0x0f8058bc
 f803aa4:	0f8058f4 	.word	0x0f8058f4
 f803aa8:	0f805928 	.word	0x0f805928
 f803aac:	4a51      	ldr	r2, [pc, #324]	; (f803bf4 <fault_diagnosis+0x3c4>)
 f803aae:	4852      	ldr	r0, [pc, #328]	; (f803bf8 <fault_diagnosis+0x3c8>)
 f803ab0:	f7ff f9ca 	bl	f802e48 <DiagPrintf>
 f803ab4:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
 f803ab8:	078a      	lsls	r2, r1, #30
 f803aba:	f57f af03 	bpl.w	f8038c4 <fault_diagnosis+0x94>
 f803abe:	4a4f      	ldr	r2, [pc, #316]	; (f803bfc <fault_diagnosis+0x3cc>)
 f803ac0:	484d      	ldr	r0, [pc, #308]	; (f803bf8 <fault_diagnosis+0x3c8>)
 f803ac2:	f7ff f9c1 	bl	f802e48 <DiagPrintf>
 f803ac6:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
 f803aca:	074b      	lsls	r3, r1, #29
 f803acc:	f57f aefd 	bpl.w	f8038ca <fault_diagnosis+0x9a>
 f803ad0:	4a4b      	ldr	r2, [pc, #300]	; (f803c00 <fault_diagnosis+0x3d0>)
 f803ad2:	4849      	ldr	r0, [pc, #292]	; (f803bf8 <fault_diagnosis+0x3c8>)
 f803ad4:	f7ff f9b8 	bl	f802e48 <DiagPrintf>
 f803ad8:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
 f803adc:	0708      	lsls	r0, r1, #28
 f803ade:	f57f aef7 	bpl.w	f8038d0 <fault_diagnosis+0xa0>
 f803ae2:	4a48      	ldr	r2, [pc, #288]	; (f803c04 <fault_diagnosis+0x3d4>)
 f803ae4:	4844      	ldr	r0, [pc, #272]	; (f803bf8 <fault_diagnosis+0x3c8>)
 f803ae6:	f7ff f9af 	bl	f802e48 <DiagPrintf>
 f803aea:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
 f803aee:	06ca      	lsls	r2, r1, #27
 f803af0:	f57f aef1 	bpl.w	f8038d6 <fault_diagnosis+0xa6>
 f803af4:	4a44      	ldr	r2, [pc, #272]	; (f803c08 <fault_diagnosis+0x3d8>)
 f803af6:	4840      	ldr	r0, [pc, #256]	; (f803bf8 <fault_diagnosis+0x3c8>)
 f803af8:	f7ff f9a6 	bl	f802e48 <DiagPrintf>
 f803afc:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
 f803b00:	068b      	lsls	r3, r1, #26
 f803b02:	f57f aeeb 	bpl.w	f8038dc <fault_diagnosis+0xac>
 f803b06:	4a41      	ldr	r2, [pc, #260]	; (f803c0c <fault_diagnosis+0x3dc>)
 f803b08:	483b      	ldr	r0, [pc, #236]	; (f803bf8 <fault_diagnosis+0x3c8>)
 f803b0a:	f7ff f99d 	bl	f802e48 <DiagPrintf>
 f803b0e:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
 f803b12:	e6e3      	b.n	f8038dc <fault_diagnosis+0xac>
 f803b14:	4a3e      	ldr	r2, [pc, #248]	; (f803c10 <fault_diagnosis+0x3e0>)
 f803b16:	483f      	ldr	r0, [pc, #252]	; (f803c14 <fault_diagnosis+0x3e4>)
 f803b18:	f7ff f996 	bl	f802e48 <DiagPrintf>
 f803b1c:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
 f803b20:	0798      	lsls	r0, r3, #30
 f803b22:	f57f aeeb 	bpl.w	f8038fc <fault_diagnosis+0xcc>
 f803b26:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
 f803b2a:	4a3b      	ldr	r2, [pc, #236]	; (f803c18 <fault_diagnosis+0x3e8>)
 f803b2c:	4839      	ldr	r0, [pc, #228]	; (f803c14 <fault_diagnosis+0x3e4>)
 f803b2e:	f7ff f98b 	bl	f802e48 <DiagPrintf>
 f803b32:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
 f803b36:	0759      	lsls	r1, r3, #29
 f803b38:	f57f aee3 	bpl.w	f803902 <fault_diagnosis+0xd2>
 f803b3c:	4a37      	ldr	r2, [pc, #220]	; (f803c1c <fault_diagnosis+0x3ec>)
 f803b3e:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
 f803b42:	4834      	ldr	r0, [pc, #208]	; (f803c14 <fault_diagnosis+0x3e4>)
 f803b44:	f7ff f980 	bl	f802e48 <DiagPrintf>
 f803b48:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
 f803b4c:	071a      	lsls	r2, r3, #28
 f803b4e:	f57f aedb 	bpl.w	f803908 <fault_diagnosis+0xd8>
 f803b52:	4a33      	ldr	r2, [pc, #204]	; (f803c20 <fault_diagnosis+0x3f0>)
 f803b54:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
 f803b58:	482e      	ldr	r0, [pc, #184]	; (f803c14 <fault_diagnosis+0x3e4>)
 f803b5a:	f7ff f975 	bl	f802e48 <DiagPrintf>
 f803b5e:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
 f803b62:	06db      	lsls	r3, r3, #27
 f803b64:	f57f aed3 	bpl.w	f80390e <fault_diagnosis+0xde>
 f803b68:	4a2e      	ldr	r2, [pc, #184]	; (f803c24 <fault_diagnosis+0x3f4>)
 f803b6a:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
 f803b6e:	4829      	ldr	r0, [pc, #164]	; (f803c14 <fault_diagnosis+0x3e4>)
 f803b70:	f7ff f96a 	bl	f802e48 <DiagPrintf>
 f803b74:	f894 3059 	ldrb.w	r3, [r4, #89]	; 0x59
 f803b78:	07d8      	lsls	r0, r3, #31
 f803b7a:	f57f aecd 	bpl.w	f803918 <fault_diagnosis+0xe8>
 f803b7e:	4a2a      	ldr	r2, [pc, #168]	; (f803c28 <fault_diagnosis+0x3f8>)
 f803b80:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
 f803b84:	4823      	ldr	r0, [pc, #140]	; (f803c14 <fault_diagnosis+0x3e4>)
 f803b86:	f7ff f95f 	bl	f802e48 <DiagPrintf>
 f803b8a:	f894 3059 	ldrb.w	r3, [r4, #89]	; 0x59
 f803b8e:	e6c3      	b.n	f803918 <fault_diagnosis+0xe8>
 f803b90:	4a26      	ldr	r2, [pc, #152]	; (f803c2c <fault_diagnosis+0x3fc>)
 f803b92:	6ee1      	ldr	r1, [r4, #108]	; 0x6c
 f803b94:	4826      	ldr	r0, [pc, #152]	; (f803c30 <fault_diagnosis+0x400>)
 f803b96:	f7ff f957 	bl	f802e48 <DiagPrintf>
 f803b9a:	e673      	b.n	f803884 <fault_diagnosis+0x54>
 f803b9c:	4a25      	ldr	r2, [pc, #148]	; (f803c34 <fault_diagnosis+0x404>)
 f803b9e:	6e21      	ldr	r1, [r4, #96]	; 0x60
 f803ba0:	4825      	ldr	r0, [pc, #148]	; (f803c38 <fault_diagnosis+0x408>)
 f803ba2:	f7ff f951 	bl	f802e48 <DiagPrintf>
 f803ba6:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
 f803baa:	e6ca      	b.n	f803942 <fault_diagnosis+0x112>
 f803bac:	4a23      	ldr	r2, [pc, #140]	; (f803c3c <fault_diagnosis+0x40c>)
 f803bae:	6e21      	ldr	r1, [r4, #96]	; 0x60
 f803bb0:	4821      	ldr	r0, [pc, #132]	; (f803c38 <fault_diagnosis+0x408>)
 f803bb2:	f7ff f949 	bl	f802e48 <DiagPrintf>
 f803bb6:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
 f803bba:	e6bf      	b.n	f80393c <fault_diagnosis+0x10c>
 f803bbc:	4a20      	ldr	r2, [pc, #128]	; (f803c40 <fault_diagnosis+0x410>)
 f803bbe:	481e      	ldr	r0, [pc, #120]	; (f803c38 <fault_diagnosis+0x408>)
 f803bc0:	f7ff f942 	bl	f802e48 <DiagPrintf>
 f803bc4:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
 f803bc8:	e6b5      	b.n	f803936 <fault_diagnosis+0x106>
 f803bca:	4a1e      	ldr	r2, [pc, #120]	; (f803c44 <fault_diagnosis+0x414>)
 f803bcc:	6e21      	ldr	r1, [r4, #96]	; 0x60
 f803bce:	481a      	ldr	r0, [pc, #104]	; (f803c38 <fault_diagnosis+0x408>)
 f803bd0:	f7ff f93a 	bl	f802e48 <DiagPrintf>
 f803bd4:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
 f803bd8:	e6b6      	b.n	f803948 <fault_diagnosis+0x118>
 f803bda:	4a1b      	ldr	r2, [pc, #108]	; (f803c48 <fault_diagnosis+0x418>)
 f803bdc:	6ce1      	ldr	r1, [r4, #76]	; 0x4c
 f803bde:	481b      	ldr	r0, [pc, #108]	; (f803c4c <fault_diagnosis+0x41c>)
 f803be0:	f7ff f932 	bl	f802e48 <DiagPrintf>
 f803be4:	e665      	b.n	f8038b2 <fault_diagnosis+0x82>
 f803be6:	4a1a      	ldr	r2, [pc, #104]	; (f803c50 <fault_diagnosis+0x420>)
 f803be8:	6d61      	ldr	r1, [r4, #84]	; 0x54
 f803bea:	481a      	ldr	r0, [pc, #104]	; (f803c54 <fault_diagnosis+0x424>)
 f803bec:	f7ff f92c 	bl	f802e48 <DiagPrintf>
 f803bf0:	e679      	b.n	f8038e6 <fault_diagnosis+0xb6>
 f803bf2:	bf00      	nop
 f803bf4:	0f8059bc 	.word	0x0f8059bc
 f803bf8:	0f8059f0 	.word	0x0f8059f0
 f803bfc:	0f805a08 	.word	0x0f805a08
 f803c00:	0f805a40 	.word	0x0f805a40
 f803c04:	0f805a78 	.word	0x0f805a78
 f803c08:	0f805aa0 	.word	0x0f805aa0
 f803c0c:	0f805ac8 	.word	0x0f805ac8
 f803c10:	0f805b44 	.word	0x0f805b44
 f803c14:	0f805b8c 	.word	0x0f805b8c
 f803c18:	0f805ba4 	.word	0x0f805ba4
 f803c1c:	0f805bf0 	.word	0x0f805bf0
 f803c20:	0f805c50 	.word	0x0f805c50
 f803c24:	0f805c98 	.word	0x0f805c98
 f803c28:	0f805cf4 	.word	0x0f805cf4
 f803c2c:	0f8057e4 	.word	0x0f8057e4
 f803c30:	0f80580c 	.word	0x0f80580c
 f803c34:	0f805e30 	.word	0x0f805e30
 f803c38:	0f805de4 	.word	0x0f805de4
 f803c3c:	0f805dfc 	.word	0x0f805dfc
 f803c40:	0f805db4 	.word	0x0f805db4
 f803c44:	0f805e5c 	.word	0x0f805e5c
 f803c48:	0f805974 	.word	0x0f805974
 f803c4c:	0f8059a4 	.word	0x0f8059a4
 f803c50:	0f805b08 	.word	0x0f805b08
 f803c54:	0f805b2c 	.word	0x0f805b2c

0f803c58 <__aeabi_ldivmod>:
 f803c58:	b97b      	cbnz	r3, f803c7a <__aeabi_ldivmod+0x22>
 f803c5a:	b972      	cbnz	r2, f803c7a <__aeabi_ldivmod+0x22>
 f803c5c:	2900      	cmp	r1, #0
 f803c5e:	bfbe      	ittt	lt
 f803c60:	2000      	movlt	r0, #0
 f803c62:	f04f 4100 	movlt.w	r1, #2147483648	; 0x80000000
 f803c66:	e006      	blt.n	f803c76 <__aeabi_ldivmod+0x1e>
 f803c68:	bf08      	it	eq
 f803c6a:	2800      	cmpeq	r0, #0
 f803c6c:	bf1c      	itt	ne
 f803c6e:	f06f 4100 	mvnne.w	r1, #2147483648	; 0x80000000
 f803c72:	f04f 30ff 	movne.w	r0, #4294967295	; 0xffffffff
 f803c76:	f000 b9ab 	b.w	f803fd0 <__aeabi_idiv0>
 f803c7a:	f1ad 0c08 	sub.w	ip, sp, #8
 f803c7e:	e96d ce04 	strd	ip, lr, [sp, #-16]!
 f803c82:	2900      	cmp	r1, #0
 f803c84:	db09      	blt.n	f803c9a <__aeabi_ldivmod+0x42>
 f803c86:	2b00      	cmp	r3, #0
 f803c88:	db1a      	blt.n	f803cc0 <__aeabi_ldivmod+0x68>
 f803c8a:	f000 f84d 	bl	f803d28 <__udivmoddi4>
 f803c8e:	f8dd e004 	ldr.w	lr, [sp, #4]
 f803c92:	e9dd 2302 	ldrd	r2, r3, [sp, #8]
 f803c96:	b004      	add	sp, #16
 f803c98:	4770      	bx	lr
 f803c9a:	4240      	negs	r0, r0
 f803c9c:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
 f803ca0:	2b00      	cmp	r3, #0
 f803ca2:	db1b      	blt.n	f803cdc <__aeabi_ldivmod+0x84>
 f803ca4:	f000 f840 	bl	f803d28 <__udivmoddi4>
 f803ca8:	f8dd e004 	ldr.w	lr, [sp, #4]
 f803cac:	e9dd 2302 	ldrd	r2, r3, [sp, #8]
 f803cb0:	b004      	add	sp, #16
 f803cb2:	4240      	negs	r0, r0
 f803cb4:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
 f803cb8:	4252      	negs	r2, r2
 f803cba:	eb63 0343 	sbc.w	r3, r3, r3, lsl #1
 f803cbe:	4770      	bx	lr
 f803cc0:	4252      	negs	r2, r2
 f803cc2:	eb63 0343 	sbc.w	r3, r3, r3, lsl #1
 f803cc6:	f000 f82f 	bl	f803d28 <__udivmoddi4>
 f803cca:	f8dd e004 	ldr.w	lr, [sp, #4]
 f803cce:	e9dd 2302 	ldrd	r2, r3, [sp, #8]
 f803cd2:	b004      	add	sp, #16
 f803cd4:	4240      	negs	r0, r0
 f803cd6:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
 f803cda:	4770      	bx	lr
 f803cdc:	4252      	negs	r2, r2
 f803cde:	eb63 0343 	sbc.w	r3, r3, r3, lsl #1
 f803ce2:	f000 f821 	bl	f803d28 <__udivmoddi4>
 f803ce6:	f8dd e004 	ldr.w	lr, [sp, #4]
 f803cea:	e9dd 2302 	ldrd	r2, r3, [sp, #8]
 f803cee:	b004      	add	sp, #16
 f803cf0:	4252      	negs	r2, r2
 f803cf2:	eb63 0343 	sbc.w	r3, r3, r3, lsl #1
 f803cf6:	4770      	bx	lr

0f803cf8 <__aeabi_uldivmod>:
 f803cf8:	b953      	cbnz	r3, f803d10 <__aeabi_uldivmod+0x18>
 f803cfa:	b94a      	cbnz	r2, f803d10 <__aeabi_uldivmod+0x18>
 f803cfc:	2900      	cmp	r1, #0
 f803cfe:	bf08      	it	eq
 f803d00:	2800      	cmpeq	r0, #0
 f803d02:	bf1c      	itt	ne
 f803d04:	f04f 31ff 	movne.w	r1, #4294967295	; 0xffffffff
 f803d08:	f04f 30ff 	movne.w	r0, #4294967295	; 0xffffffff
 f803d0c:	f000 b960 	b.w	f803fd0 <__aeabi_idiv0>
 f803d10:	f1ad 0c08 	sub.w	ip, sp, #8
 f803d14:	e96d ce04 	strd	ip, lr, [sp, #-16]!
 f803d18:	f000 f806 	bl	f803d28 <__udivmoddi4>
 f803d1c:	f8dd e004 	ldr.w	lr, [sp, #4]
 f803d20:	e9dd 2302 	ldrd	r2, r3, [sp, #8]
 f803d24:	b004      	add	sp, #16
 f803d26:	4770      	bx	lr

0f803d28 <__udivmoddi4>:
 f803d28:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 f803d2c:	9e09      	ldr	r6, [sp, #36]	; 0x24
 f803d2e:	4688      	mov	r8, r1
 f803d30:	468e      	mov	lr, r1
 f803d32:	4615      	mov	r5, r2
 f803d34:	4604      	mov	r4, r0
 f803d36:	4619      	mov	r1, r3
 f803d38:	2b00      	cmp	r3, #0
 f803d3a:	f040 80c6 	bne.w	f803eca <__udivmoddi4+0x1a2>
 f803d3e:	4542      	cmp	r2, r8
 f803d40:	fab2 f782 	clz	r7, r2
 f803d44:	d946      	bls.n	f803dd4 <__udivmoddi4+0xac>
 f803d46:	b14f      	cbz	r7, f803d5c <__udivmoddi4+0x34>
 f803d48:	f1c7 0c20 	rsb	ip, r7, #32
 f803d4c:	fa08 f307 	lsl.w	r3, r8, r7
 f803d50:	40bd      	lsls	r5, r7
 f803d52:	40bc      	lsls	r4, r7
 f803d54:	fa20 fc0c 	lsr.w	ip, r0, ip
 f803d58:	ea4c 0e03 	orr.w	lr, ip, r3
 f803d5c:	ea4f 4815 	mov.w	r8, r5, lsr #16
 f803d60:	fa1f fc85 	uxth.w	ip, r5
 f803d64:	0c22      	lsrs	r2, r4, #16
 f803d66:	fbbe f9f8 	udiv	r9, lr, r8
 f803d6a:	fb08 e319 	mls	r3, r8, r9, lr
 f803d6e:	fb09 fa0c 	mul.w	sl, r9, ip
 f803d72:	ea42 4303 	orr.w	r3, r2, r3, lsl #16
 f803d76:	459a      	cmp	sl, r3
 f803d78:	d928      	bls.n	f803dcc <__udivmoddi4+0xa4>
 f803d7a:	18eb      	adds	r3, r5, r3
 f803d7c:	f109 30ff 	add.w	r0, r9, #4294967295	; 0xffffffff
 f803d80:	d204      	bcs.n	f803d8c <__udivmoddi4+0x64>
 f803d82:	459a      	cmp	sl, r3
 f803d84:	d902      	bls.n	f803d8c <__udivmoddi4+0x64>
 f803d86:	f1a9 0002 	sub.w	r0, r9, #2
 f803d8a:	442b      	add	r3, r5
 f803d8c:	eba3 030a 	sub.w	r3, r3, sl
 f803d90:	b2a4      	uxth	r4, r4
 f803d92:	fbb3 f2f8 	udiv	r2, r3, r8
 f803d96:	fb08 3312 	mls	r3, r8, r2, r3
 f803d9a:	fb02 fc0c 	mul.w	ip, r2, ip
 f803d9e:	ea44 4403 	orr.w	r4, r4, r3, lsl #16
 f803da2:	45a4      	cmp	ip, r4
 f803da4:	d914      	bls.n	f803dd0 <__udivmoddi4+0xa8>
 f803da6:	192c      	adds	r4, r5, r4
 f803da8:	f102 33ff 	add.w	r3, r2, #4294967295	; 0xffffffff
 f803dac:	d203      	bcs.n	f803db6 <__udivmoddi4+0x8e>
 f803dae:	45a4      	cmp	ip, r4
 f803db0:	d901      	bls.n	f803db6 <__udivmoddi4+0x8e>
 f803db2:	1e93      	subs	r3, r2, #2
 f803db4:	442c      	add	r4, r5
 f803db6:	eba4 040c 	sub.w	r4, r4, ip
 f803dba:	ea43 4000 	orr.w	r0, r3, r0, lsl #16
 f803dbe:	b11e      	cbz	r6, f803dc8 <__udivmoddi4+0xa0>
 f803dc0:	40fc      	lsrs	r4, r7
 f803dc2:	2300      	movs	r3, #0
 f803dc4:	6034      	str	r4, [r6, #0]
 f803dc6:	6073      	str	r3, [r6, #4]
 f803dc8:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 f803dcc:	4648      	mov	r0, r9
 f803dce:	e7dd      	b.n	f803d8c <__udivmoddi4+0x64>
 f803dd0:	4613      	mov	r3, r2
 f803dd2:	e7f0      	b.n	f803db6 <__udivmoddi4+0x8e>
 f803dd4:	b902      	cbnz	r2, f803dd8 <__udivmoddi4+0xb0>
 f803dd6:	deff      	udf	#255	; 0xff
 f803dd8:	bb8f      	cbnz	r7, f803e3e <__udivmoddi4+0x116>
 f803dda:	eba8 0302 	sub.w	r3, r8, r2
 f803dde:	2101      	movs	r1, #1
 f803de0:	ea4f 4e15 	mov.w	lr, r5, lsr #16
 f803de4:	b2aa      	uxth	r2, r5
 f803de6:	0c20      	lsrs	r0, r4, #16
 f803de8:	fbb3 fcfe 	udiv	ip, r3, lr
 f803dec:	fb0e 331c 	mls	r3, lr, ip, r3
 f803df0:	fb0c f802 	mul.w	r8, ip, r2
 f803df4:	ea40 4303 	orr.w	r3, r0, r3, lsl #16
 f803df8:	4598      	cmp	r8, r3
 f803dfa:	d962      	bls.n	f803ec2 <__udivmoddi4+0x19a>
 f803dfc:	18eb      	adds	r3, r5, r3
 f803dfe:	f10c 30ff 	add.w	r0, ip, #4294967295	; 0xffffffff
 f803e02:	d204      	bcs.n	f803e0e <__udivmoddi4+0xe6>
 f803e04:	4598      	cmp	r8, r3
 f803e06:	d902      	bls.n	f803e0e <__udivmoddi4+0xe6>
 f803e08:	f1ac 0002 	sub.w	r0, ip, #2
 f803e0c:	442b      	add	r3, r5
 f803e0e:	eba3 0308 	sub.w	r3, r3, r8
 f803e12:	b2a4      	uxth	r4, r4
 f803e14:	fbb3 fcfe 	udiv	ip, r3, lr
 f803e18:	fb0e 331c 	mls	r3, lr, ip, r3
 f803e1c:	fb0c f202 	mul.w	r2, ip, r2
 f803e20:	ea44 4403 	orr.w	r4, r4, r3, lsl #16
 f803e24:	42a2      	cmp	r2, r4
 f803e26:	d94e      	bls.n	f803ec6 <__udivmoddi4+0x19e>
 f803e28:	192c      	adds	r4, r5, r4
 f803e2a:	f10c 33ff 	add.w	r3, ip, #4294967295	; 0xffffffff
 f803e2e:	d204      	bcs.n	f803e3a <__udivmoddi4+0x112>
 f803e30:	42a2      	cmp	r2, r4
 f803e32:	d902      	bls.n	f803e3a <__udivmoddi4+0x112>
 f803e34:	f1ac 0302 	sub.w	r3, ip, #2
 f803e38:	442c      	add	r4, r5
 f803e3a:	1aa4      	subs	r4, r4, r2
 f803e3c:	e7bd      	b.n	f803dba <__udivmoddi4+0x92>
 f803e3e:	f1c7 0c20 	rsb	ip, r7, #32
 f803e42:	40bd      	lsls	r5, r7
 f803e44:	fa08 f307 	lsl.w	r3, r8, r7
 f803e48:	40bc      	lsls	r4, r7
 f803e4a:	fa28 f90c 	lsr.w	r9, r8, ip
 f803e4e:	ea4f 4e15 	mov.w	lr, r5, lsr #16
 f803e52:	fa20 fc0c 	lsr.w	ip, r0, ip
 f803e56:	ea4c 0203 	orr.w	r2, ip, r3
 f803e5a:	b2ab      	uxth	r3, r5
 f803e5c:	fbb9 fcfe 	udiv	ip, r9, lr
 f803e60:	0c11      	lsrs	r1, r2, #16
 f803e62:	fb0e 901c 	mls	r0, lr, ip, r9
 f803e66:	fb0c f803 	mul.w	r8, ip, r3
 f803e6a:	ea41 4000 	orr.w	r0, r1, r0, lsl #16
 f803e6e:	4580      	cmp	r8, r0
 f803e70:	d923      	bls.n	f803eba <__udivmoddi4+0x192>
 f803e72:	1828      	adds	r0, r5, r0
 f803e74:	f10c 31ff 	add.w	r1, ip, #4294967295	; 0xffffffff
 f803e78:	d204      	bcs.n	f803e84 <__udivmoddi4+0x15c>
 f803e7a:	4580      	cmp	r8, r0
 f803e7c:	d902      	bls.n	f803e84 <__udivmoddi4+0x15c>
 f803e7e:	f1ac 0102 	sub.w	r1, ip, #2
 f803e82:	4428      	add	r0, r5
 f803e84:	eba0 0008 	sub.w	r0, r0, r8
 f803e88:	b292      	uxth	r2, r2
 f803e8a:	fbb0 fcfe 	udiv	ip, r0, lr
 f803e8e:	fb0e 001c 	mls	r0, lr, ip, r0
 f803e92:	fb0c f803 	mul.w	r8, ip, r3
 f803e96:	ea42 4300 	orr.w	r3, r2, r0, lsl #16
 f803e9a:	4598      	cmp	r8, r3
 f803e9c:	d90f      	bls.n	f803ebe <__udivmoddi4+0x196>
 f803e9e:	18eb      	adds	r3, r5, r3
 f803ea0:	f10c 32ff 	add.w	r2, ip, #4294967295	; 0xffffffff
 f803ea4:	d204      	bcs.n	f803eb0 <__udivmoddi4+0x188>
 f803ea6:	4598      	cmp	r8, r3
 f803ea8:	d902      	bls.n	f803eb0 <__udivmoddi4+0x188>
 f803eaa:	f1ac 0202 	sub.w	r2, ip, #2
 f803eae:	442b      	add	r3, r5
 f803eb0:	eba3 0308 	sub.w	r3, r3, r8
 f803eb4:	ea42 4101 	orr.w	r1, r2, r1, lsl #16
 f803eb8:	e792      	b.n	f803de0 <__udivmoddi4+0xb8>
 f803eba:	4661      	mov	r1, ip
 f803ebc:	e7e2      	b.n	f803e84 <__udivmoddi4+0x15c>
 f803ebe:	4662      	mov	r2, ip
 f803ec0:	e7f6      	b.n	f803eb0 <__udivmoddi4+0x188>
 f803ec2:	4660      	mov	r0, ip
 f803ec4:	e7a3      	b.n	f803e0e <__udivmoddi4+0xe6>
 f803ec6:	4663      	mov	r3, ip
 f803ec8:	e7b7      	b.n	f803e3a <__udivmoddi4+0x112>
 f803eca:	4543      	cmp	r3, r8
 f803ecc:	d905      	bls.n	f803eda <__udivmoddi4+0x1b2>
 f803ece:	b10e      	cbz	r6, f803ed4 <__udivmoddi4+0x1ac>
 f803ed0:	e9c6 0800 	strd	r0, r8, [r6]
 f803ed4:	2100      	movs	r1, #0
 f803ed6:	4608      	mov	r0, r1
 f803ed8:	e776      	b.n	f803dc8 <__udivmoddi4+0xa0>
 f803eda:	fab3 fc83 	clz	ip, r3
 f803ede:	f1bc 0f00 	cmp.w	ip, #0
 f803ee2:	d110      	bne.n	f803f06 <__udivmoddi4+0x1de>
 f803ee4:	4543      	cmp	r3, r8
 f803ee6:	d301      	bcc.n	f803eec <__udivmoddi4+0x1c4>
 f803ee8:	4282      	cmp	r2, r0
 f803eea:	d80a      	bhi.n	f803f02 <__udivmoddi4+0x1da>
 f803eec:	1a84      	subs	r4, r0, r2
 f803eee:	eb68 0303 	sbc.w	r3, r8, r3
 f803ef2:	2001      	movs	r0, #1
 f803ef4:	469e      	mov	lr, r3
 f803ef6:	2e00      	cmp	r6, #0
 f803ef8:	d068      	beq.n	f803fcc <__udivmoddi4+0x2a4>
 f803efa:	e9c6 4e00 	strd	r4, lr, [r6]
 f803efe:	2100      	movs	r1, #0
 f803f00:	e762      	b.n	f803dc8 <__udivmoddi4+0xa0>
 f803f02:	4660      	mov	r0, ip
 f803f04:	e7f7      	b.n	f803ef6 <__udivmoddi4+0x1ce>
 f803f06:	f1cc 0e20 	rsb	lr, ip, #32
 f803f0a:	fa03 f30c 	lsl.w	r3, r3, ip
 f803f0e:	fa02 f50c 	lsl.w	r5, r2, ip
 f803f12:	fa00 f70c 	lsl.w	r7, r0, ip
 f803f16:	fa22 f40e 	lsr.w	r4, r2, lr
 f803f1a:	fa28 fb0e 	lsr.w	fp, r8, lr
 f803f1e:	fa20 f10e 	lsr.w	r1, r0, lr
 f803f22:	431c      	orrs	r4, r3
 f803f24:	fa08 f30c 	lsl.w	r3, r8, ip
 f803f28:	ea4f 4914 	mov.w	r9, r4, lsr #16
 f803f2c:	4319      	orrs	r1, r3
 f803f2e:	b2a3      	uxth	r3, r4
 f803f30:	0c0a      	lsrs	r2, r1, #16
 f803f32:	fbbb f8f9 	udiv	r8, fp, r9
 f803f36:	fb09 bb18 	mls	fp, r9, r8, fp
 f803f3a:	fb08 fa03 	mul.w	sl, r8, r3
 f803f3e:	ea42 420b 	orr.w	r2, r2, fp, lsl #16
 f803f42:	4592      	cmp	sl, r2
 f803f44:	d93e      	bls.n	f803fc4 <__udivmoddi4+0x29c>
 f803f46:	18a2      	adds	r2, r4, r2
 f803f48:	f108 30ff 	add.w	r0, r8, #4294967295	; 0xffffffff
 f803f4c:	d204      	bcs.n	f803f58 <__udivmoddi4+0x230>
 f803f4e:	4592      	cmp	sl, r2
 f803f50:	d902      	bls.n	f803f58 <__udivmoddi4+0x230>
 f803f52:	f1a8 0002 	sub.w	r0, r8, #2
 f803f56:	4422      	add	r2, r4
 f803f58:	eba2 020a 	sub.w	r2, r2, sl
 f803f5c:	b289      	uxth	r1, r1
 f803f5e:	fbb2 f8f9 	udiv	r8, r2, r9
 f803f62:	fb09 2218 	mls	r2, r9, r8, r2
 f803f66:	fb08 f303 	mul.w	r3, r8, r3
 f803f6a:	ea41 4202 	orr.w	r2, r1, r2, lsl #16
 f803f6e:	4293      	cmp	r3, r2
 f803f70:	d92a      	bls.n	f803fc8 <__udivmoddi4+0x2a0>
 f803f72:	18a2      	adds	r2, r4, r2
 f803f74:	f108 31ff 	add.w	r1, r8, #4294967295	; 0xffffffff
 f803f78:	d204      	bcs.n	f803f84 <__udivmoddi4+0x25c>
 f803f7a:	4293      	cmp	r3, r2
 f803f7c:	d902      	bls.n	f803f84 <__udivmoddi4+0x25c>
 f803f7e:	f1a8 0102 	sub.w	r1, r8, #2
 f803f82:	4422      	add	r2, r4
 f803f84:	ea41 4000 	orr.w	r0, r1, r0, lsl #16
 f803f88:	1ad2      	subs	r2, r2, r3
 f803f8a:	fba0 9805 	umull	r9, r8, r0, r5
 f803f8e:	4542      	cmp	r2, r8
 f803f90:	464b      	mov	r3, r9
 f803f92:	4641      	mov	r1, r8
 f803f94:	d302      	bcc.n	f803f9c <__udivmoddi4+0x274>
 f803f96:	d106      	bne.n	f803fa6 <__udivmoddi4+0x27e>
 f803f98:	454f      	cmp	r7, r9
 f803f9a:	d204      	bcs.n	f803fa6 <__udivmoddi4+0x27e>
 f803f9c:	3801      	subs	r0, #1
 f803f9e:	ebb9 0305 	subs.w	r3, r9, r5
 f803fa2:	eb68 0104 	sbc.w	r1, r8, r4
 f803fa6:	b18e      	cbz	r6, f803fcc <__udivmoddi4+0x2a4>
 f803fa8:	1afc      	subs	r4, r7, r3
 f803faa:	eb62 0701 	sbc.w	r7, r2, r1
 f803fae:	fa07 fe0e 	lsl.w	lr, r7, lr
 f803fb2:	fa24 f30c 	lsr.w	r3, r4, ip
 f803fb6:	fa27 f70c 	lsr.w	r7, r7, ip
 f803fba:	ea4e 0303 	orr.w	r3, lr, r3
 f803fbe:	e9c6 3700 	strd	r3, r7, [r6]
 f803fc2:	e79c      	b.n	f803efe <__udivmoddi4+0x1d6>
 f803fc4:	4640      	mov	r0, r8
 f803fc6:	e7c7      	b.n	f803f58 <__udivmoddi4+0x230>
 f803fc8:	4641      	mov	r1, r8
 f803fca:	e7db      	b.n	f803f84 <__udivmoddi4+0x25c>
 f803fcc:	4631      	mov	r1, r6
 f803fce:	e6fb      	b.n	f803dc8 <__udivmoddi4+0xa0>

0f803fd0 <__aeabi_idiv0>:
 f803fd0:	4770      	bx	lr
 f803fd2:	bf00      	nop

0f803fd4 <set_psram_wakeup_mode>:
 f803fd4:	b580      	push	{r7, lr}
 f803fd6:	b09e      	sub	sp, #120	; 0x78
 f803fd8:	af00      	add	r7, sp, #0
 f803fda:	6078      	str	r0, [r7, #4]
 f803fdc:	4b73      	ldr	r3, [pc, #460]	; (f8041ac <set_psram_wakeup_mode+0x1d8>)
 f803fde:	677b      	str	r3, [r7, #116]	; 0x74
 f803fe0:	4b73      	ldr	r3, [pc, #460]	; (f8041b0 <set_psram_wakeup_mode+0x1dc>)
 f803fe2:	673b      	str	r3, [r7, #112]	; 0x70
 f803fe4:	4b73      	ldr	r3, [pc, #460]	; (f8041b4 <set_psram_wakeup_mode+0x1e0>)
 f803fe6:	66fb      	str	r3, [r7, #108]	; 0x6c
 f803fe8:	4b73      	ldr	r3, [pc, #460]	; (f8041b8 <set_psram_wakeup_mode+0x1e4>)
 f803fea:	663b      	str	r3, [r7, #96]	; 0x60
 f803fec:	2320      	movs	r3, #32
 f803fee:	65fb      	str	r3, [r7, #92]	; 0x5c
 f803ff0:	4b72      	ldr	r3, [pc, #456]	; (f8041bc <set_psram_wakeup_mode+0x1e8>)
 f803ff2:	695b      	ldr	r3, [r3, #20]
 f803ff4:	f403 3380 	and.w	r3, r3, #65536	; 0x10000
 f803ff8:	2b00      	cmp	r3, #0
 f803ffa:	d064      	beq.n	f8040c6 <set_psram_wakeup_mode+0xf2>
 f803ffc:	6e3a      	ldr	r2, [r7, #96]	; 0x60
 f803ffe:	6dfb      	ldr	r3, [r7, #92]	; 0x5c
 f804000:	4013      	ands	r3, r2
 f804002:	f1b3 3fff 	cmp.w	r3, #4294967295	; 0xffffffff
 f804006:	d134      	bne.n	f804072 <set_psram_wakeup_mode+0x9e>
 f804008:	4b6c      	ldr	r3, [pc, #432]	; (f8041bc <set_psram_wakeup_mode+0x1e8>)
 f80400a:	2200      	movs	r2, #0
 f80400c:	f8c3 2084 	str.w	r2, [r3, #132]	; 0x84
 f804010:	f3bf 8f4f 	dsb	sy
 f804014:	4b69      	ldr	r3, [pc, #420]	; (f8041bc <set_psram_wakeup_mode+0x1e8>)
 f804016:	f8d3 3080 	ldr.w	r3, [r3, #128]	; 0x80
 f80401a:	65bb      	str	r3, [r7, #88]	; 0x58
 f80401c:	6dbb      	ldr	r3, [r7, #88]	; 0x58
 f80401e:	0b5b      	lsrs	r3, r3, #13
 f804020:	f3c3 030e 	ubfx	r3, r3, #0, #15
 f804024:	657b      	str	r3, [r7, #84]	; 0x54
 f804026:	e001      	b.n	f80402c <set_psram_wakeup_mode+0x58>
 f804028:	6cfb      	ldr	r3, [r7, #76]	; 0x4c
 f80402a:	657b      	str	r3, [r7, #84]	; 0x54
 f80402c:	6dbb      	ldr	r3, [r7, #88]	; 0x58
 f80402e:	08db      	lsrs	r3, r3, #3
 f804030:	f3c3 0309 	ubfx	r3, r3, #0, #10
 f804034:	653b      	str	r3, [r7, #80]	; 0x50
 f804036:	6d7b      	ldr	r3, [r7, #84]	; 0x54
 f804038:	015a      	lsls	r2, r3, #5
 f80403a:	f643 73e0 	movw	r3, #16352	; 0x3fe0
 f80403e:	4013      	ands	r3, r2
 f804040:	6d3a      	ldr	r2, [r7, #80]	; 0x50
 f804042:	0792      	lsls	r2, r2, #30
 f804044:	4313      	orrs	r3, r2
 f804046:	4a5d      	ldr	r2, [pc, #372]	; (f8041bc <set_psram_wakeup_mode+0x1e8>)
 f804048:	f8c2 3260 	str.w	r3, [r2, #608]	; 0x260
 f80404c:	6d3b      	ldr	r3, [r7, #80]	; 0x50
 f80404e:	613b      	str	r3, [r7, #16]
 f804050:	6d3b      	ldr	r3, [r7, #80]	; 0x50
 f804052:	3b01      	subs	r3, #1
 f804054:	653b      	str	r3, [r7, #80]	; 0x50
 f804056:	693b      	ldr	r3, [r7, #16]
 f804058:	2b00      	cmp	r3, #0
 f80405a:	d1ec      	bne.n	f804036 <set_psram_wakeup_mode+0x62>
 f80405c:	6d7b      	ldr	r3, [r7, #84]	; 0x54
 f80405e:	3b01      	subs	r3, #1
 f804060:	64fb      	str	r3, [r7, #76]	; 0x4c
 f804062:	6d7b      	ldr	r3, [r7, #84]	; 0x54
 f804064:	2b00      	cmp	r3, #0
 f804066:	d1df      	bne.n	f804028 <set_psram_wakeup_mode+0x54>
 f804068:	f3bf 8f4f 	dsb	sy
 f80406c:	f3bf 8f6f 	isb	sy
 f804070:	e029      	b.n	f8040c6 <set_psram_wakeup_mode+0xf2>
 f804072:	6e3b      	ldr	r3, [r7, #96]	; 0x60
 f804074:	f003 031f 	and.w	r3, r3, #31
 f804078:	2b00      	cmp	r3, #0
 f80407a:	d007      	beq.n	f80408c <set_psram_wakeup_mode+0xb8>
 f80407c:	6e3a      	ldr	r2, [r7, #96]	; 0x60
 f80407e:	f022 021f 	bic.w	r2, r2, #31
 f804082:	64ba      	str	r2, [r7, #72]	; 0x48
 f804084:	6dfa      	ldr	r2, [r7, #92]	; 0x5c
 f804086:	4413      	add	r3, r2
 f804088:	647b      	str	r3, [r7, #68]	; 0x44
 f80408a:	e003      	b.n	f804094 <set_psram_wakeup_mode+0xc0>
 f80408c:	6dfb      	ldr	r3, [r7, #92]	; 0x5c
 f80408e:	647b      	str	r3, [r7, #68]	; 0x44
 f804090:	6e3b      	ldr	r3, [r7, #96]	; 0x60
 f804092:	64bb      	str	r3, [r7, #72]	; 0x48
 f804094:	6c7b      	ldr	r3, [r7, #68]	; 0x44
 f804096:	f3bf 8f4f 	dsb	sy
 f80409a:	6cba      	ldr	r2, [r7, #72]	; 0x48
 f80409c:	643a      	str	r2, [r7, #64]	; 0x40
 f80409e:	63fb      	str	r3, [r7, #60]	; 0x3c
 f8040a0:	e009      	b.n	f8040b6 <set_psram_wakeup_mode+0xe2>
 f8040a2:	4a46      	ldr	r2, [pc, #280]	; (f8041bc <set_psram_wakeup_mode+0x1e8>)
 f8040a4:	6c3b      	ldr	r3, [r7, #64]	; 0x40
 f8040a6:	f8c2 325c 	str.w	r3, [r2, #604]	; 0x25c
 f8040aa:	6c3b      	ldr	r3, [r7, #64]	; 0x40
 f8040ac:	3320      	adds	r3, #32
 f8040ae:	643b      	str	r3, [r7, #64]	; 0x40
 f8040b0:	6bfb      	ldr	r3, [r7, #60]	; 0x3c
 f8040b2:	3b20      	subs	r3, #32
 f8040b4:	63fb      	str	r3, [r7, #60]	; 0x3c
 f8040b6:	6bfb      	ldr	r3, [r7, #60]	; 0x3c
 f8040b8:	2b00      	cmp	r3, #0
 f8040ba:	dcf2      	bgt.n	f8040a2 <set_psram_wakeup_mode+0xce>
 f8040bc:	f3bf 8f4f 	dsb	sy
 f8040c0:	f3bf 8f6f 	isb	sy
 f8040c4:	bf00      	nop
 f8040c6:	4b3c      	ldr	r3, [pc, #240]	; (f8041b8 <set_psram_wakeup_mode+0x1e4>)
 f8040c8:	681b      	ldr	r3, [r3, #0]
 f8040ca:	66bb      	str	r3, [r7, #104]	; 0x68
 f8040cc:	6f3b      	ldr	r3, [r7, #112]	; 0x70
 f8040ce:	691b      	ldr	r3, [r3, #16]
 f8040d0:	667b      	str	r3, [r7, #100]	; 0x64
 f8040d2:	6f3b      	ldr	r3, [r7, #112]	; 0x70
 f8040d4:	6e7a      	ldr	r2, [r7, #100]	; 0x64
 f8040d6:	611a      	str	r2, [r3, #16]
 f8040d8:	687b      	ldr	r3, [r7, #4]
 f8040da:	2b00      	cmp	r3, #0
 f8040dc:	d103      	bne.n	f8040e6 <set_psram_wakeup_mode+0x112>
 f8040de:	2096      	movs	r0, #150	; 0x96
 f8040e0:	4b37      	ldr	r3, [pc, #220]	; (f8041c0 <set_psram_wakeup_mode+0x1ec>)
 f8040e2:	4798      	blx	r3
 f8040e4:	e005      	b.n	f8040f2 <set_psram_wakeup_mode+0x11e>
 f8040e6:	687b      	ldr	r3, [r7, #4]
 f8040e8:	2b01      	cmp	r3, #1
 f8040ea:	d102      	bne.n	f8040f2 <set_psram_wakeup_mode+0x11e>
 f8040ec:	2064      	movs	r0, #100	; 0x64
 f8040ee:	4b34      	ldr	r3, [pc, #208]	; (f8041c0 <set_psram_wakeup_mode+0x1ec>)
 f8040f0:	4798      	blx	r3
 f8040f2:	4b31      	ldr	r3, [pc, #196]	; (f8041b8 <set_psram_wakeup_mode+0x1e4>)
 f8040f4:	63bb      	str	r3, [r7, #56]	; 0x38
 f8040f6:	2320      	movs	r3, #32
 f8040f8:	637b      	str	r3, [r7, #52]	; 0x34
 f8040fa:	4b30      	ldr	r3, [pc, #192]	; (f8041bc <set_psram_wakeup_mode+0x1e8>)
 f8040fc:	695b      	ldr	r3, [r3, #20]
 f8040fe:	f403 3380 	and.w	r3, r3, #65536	; 0x10000
 f804102:	2b00      	cmp	r3, #0
 f804104:	d070      	beq.n	f8041e8 <set_psram_wakeup_mode+0x214>
 f804106:	6bba      	ldr	r2, [r7, #56]	; 0x38
 f804108:	6b7b      	ldr	r3, [r7, #52]	; 0x34
 f80410a:	4013      	ands	r3, r2
 f80410c:	f1b3 3fff 	cmp.w	r3, #4294967295	; 0xffffffff
 f804110:	d134      	bne.n	f80417c <set_psram_wakeup_mode+0x1a8>
 f804112:	4b2a      	ldr	r3, [pc, #168]	; (f8041bc <set_psram_wakeup_mode+0x1e8>)
 f804114:	2200      	movs	r2, #0
 f804116:	f8c3 2084 	str.w	r2, [r3, #132]	; 0x84
 f80411a:	f3bf 8f4f 	dsb	sy
 f80411e:	4b27      	ldr	r3, [pc, #156]	; (f8041bc <set_psram_wakeup_mode+0x1e8>)
 f804120:	f8d3 3080 	ldr.w	r3, [r3, #128]	; 0x80
 f804124:	633b      	str	r3, [r7, #48]	; 0x30
 f804126:	6b3b      	ldr	r3, [r7, #48]	; 0x30
 f804128:	0b5b      	lsrs	r3, r3, #13
 f80412a:	f3c3 030e 	ubfx	r3, r3, #0, #15
 f80412e:	62fb      	str	r3, [r7, #44]	; 0x2c
 f804130:	e001      	b.n	f804136 <set_psram_wakeup_mode+0x162>
 f804132:	6a7b      	ldr	r3, [r7, #36]	; 0x24
 f804134:	62fb      	str	r3, [r7, #44]	; 0x2c
 f804136:	6b3b      	ldr	r3, [r7, #48]	; 0x30
 f804138:	08db      	lsrs	r3, r3, #3
 f80413a:	f3c3 0309 	ubfx	r3, r3, #0, #10
 f80413e:	62bb      	str	r3, [r7, #40]	; 0x28
 f804140:	6afb      	ldr	r3, [r7, #44]	; 0x2c
 f804142:	015a      	lsls	r2, r3, #5
 f804144:	f643 73e0 	movw	r3, #16352	; 0x3fe0
 f804148:	4013      	ands	r3, r2
 f80414a:	6aba      	ldr	r2, [r7, #40]	; 0x28
 f80414c:	0792      	lsls	r2, r2, #30
 f80414e:	4313      	orrs	r3, r2
 f804150:	4a1a      	ldr	r2, [pc, #104]	; (f8041bc <set_psram_wakeup_mode+0x1e8>)
 f804152:	f8c2 3260 	str.w	r3, [r2, #608]	; 0x260
 f804156:	6abb      	ldr	r3, [r7, #40]	; 0x28
 f804158:	60fb      	str	r3, [r7, #12]
 f80415a:	6abb      	ldr	r3, [r7, #40]	; 0x28
 f80415c:	3b01      	subs	r3, #1
 f80415e:	62bb      	str	r3, [r7, #40]	; 0x28
 f804160:	68fb      	ldr	r3, [r7, #12]
 f804162:	2b00      	cmp	r3, #0
 f804164:	d1ec      	bne.n	f804140 <set_psram_wakeup_mode+0x16c>
 f804166:	6afb      	ldr	r3, [r7, #44]	; 0x2c
 f804168:	3b01      	subs	r3, #1
 f80416a:	627b      	str	r3, [r7, #36]	; 0x24
 f80416c:	6afb      	ldr	r3, [r7, #44]	; 0x2c
 f80416e:	2b00      	cmp	r3, #0
 f804170:	d1df      	bne.n	f804132 <set_psram_wakeup_mode+0x15e>
 f804172:	f3bf 8f4f 	dsb	sy
 f804176:	f3bf 8f6f 	isb	sy
 f80417a:	e035      	b.n	f8041e8 <set_psram_wakeup_mode+0x214>
 f80417c:	6bbb      	ldr	r3, [r7, #56]	; 0x38
 f80417e:	f003 031f 	and.w	r3, r3, #31
 f804182:	2b00      	cmp	r3, #0
 f804184:	d007      	beq.n	f804196 <set_psram_wakeup_mode+0x1c2>
 f804186:	6bba      	ldr	r2, [r7, #56]	; 0x38
 f804188:	f022 021f 	bic.w	r2, r2, #31
 f80418c:	623a      	str	r2, [r7, #32]
 f80418e:	6b7a      	ldr	r2, [r7, #52]	; 0x34
 f804190:	4413      	add	r3, r2
 f804192:	61fb      	str	r3, [r7, #28]
 f804194:	e003      	b.n	f80419e <set_psram_wakeup_mode+0x1ca>
 f804196:	6b7b      	ldr	r3, [r7, #52]	; 0x34
 f804198:	61fb      	str	r3, [r7, #28]
 f80419a:	6bbb      	ldr	r3, [r7, #56]	; 0x38
 f80419c:	623b      	str	r3, [r7, #32]
 f80419e:	69fb      	ldr	r3, [r7, #28]
 f8041a0:	f3bf 8f4f 	dsb	sy
 f8041a4:	6a3a      	ldr	r2, [r7, #32]
 f8041a6:	61ba      	str	r2, [r7, #24]
 f8041a8:	617b      	str	r3, [r7, #20]
 f8041aa:	e015      	b.n	f8041d8 <set_psram_wakeup_mode+0x204>
 f8041ac:	41008e00 	.word	0x41008e00
 f8041b0:	40126000 	.word	0x40126000
 f8041b4:	40127000 	.word	0x40127000
 f8041b8:	60000000 	.word	0x60000000
 f8041bc:	e000ed00 	.word	0xe000ed00
 f8041c0:	000094a5 	.word	0x000094a5
 f8041c4:	4a46      	ldr	r2, [pc, #280]	; (f8042e0 <set_psram_wakeup_mode+0x30c>)
 f8041c6:	69bb      	ldr	r3, [r7, #24]
 f8041c8:	f8c2 325c 	str.w	r3, [r2, #604]	; 0x25c
 f8041cc:	69bb      	ldr	r3, [r7, #24]
 f8041ce:	3320      	adds	r3, #32
 f8041d0:	61bb      	str	r3, [r7, #24]
 f8041d2:	697b      	ldr	r3, [r7, #20]
 f8041d4:	3b20      	subs	r3, #32
 f8041d6:	617b      	str	r3, [r7, #20]
 f8041d8:	697b      	ldr	r3, [r7, #20]
 f8041da:	2b00      	cmp	r3, #0
 f8041dc:	dcf2      	bgt.n	f8041c4 <set_psram_wakeup_mode+0x1f0>
 f8041de:	f3bf 8f4f 	dsb	sy
 f8041e2:	f3bf 8f6f 	isb	sy
 f8041e6:	bf00      	nop
 f8041e8:	4b3e      	ldr	r3, [pc, #248]	; (f8042e4 <set_psram_wakeup_mode+0x310>)
 f8041ea:	681b      	ldr	r3, [r3, #0]
 f8041ec:	66bb      	str	r3, [r7, #104]	; 0x68
 f8041ee:	6f3b      	ldr	r3, [r7, #112]	; 0x70
 f8041f0:	681b      	ldr	r3, [r3, #0]
 f8041f2:	0c1b      	lsrs	r3, r3, #16
 f8041f4:	f003 0301 	and.w	r3, r3, #1
 f8041f8:	2b00      	cmp	r3, #0
 f8041fa:	d11b      	bne.n	f804234 <set_psram_wakeup_mode+0x260>
 f8041fc:	6f3b      	ldr	r3, [r7, #112]	; 0x70
 f8041fe:	681b      	ldr	r3, [r3, #0]
 f804200:	0bdb      	lsrs	r3, r3, #15
 f804202:	f003 0301 	and.w	r3, r3, #1
 f804206:	2b00      	cmp	r3, #0
 f804208:	d114      	bne.n	f804234 <set_psram_wakeup_mode+0x260>
 f80420a:	6f3b      	ldr	r3, [r7, #112]	; 0x70
 f80420c:	681b      	ldr	r3, [r3, #0]
 f80420e:	0b9b      	lsrs	r3, r3, #14
 f804210:	f003 0301 	and.w	r3, r3, #1
 f804214:	2b00      	cmp	r3, #0
 f804216:	d10d      	bne.n	f804234 <set_psram_wakeup_mode+0x260>
 f804218:	6f3b      	ldr	r3, [r7, #112]	; 0x70
 f80421a:	681b      	ldr	r3, [r3, #0]
 f80421c:	0b5b      	lsrs	r3, r3, #13
 f80421e:	f003 0301 	and.w	r3, r3, #1
 f804222:	2b00      	cmp	r3, #0
 f804224:	d106      	bne.n	f804234 <set_psram_wakeup_mode+0x260>
 f804226:	6f3b      	ldr	r3, [r7, #112]	; 0x70
 f804228:	681b      	ldr	r3, [r3, #0]
 f80422a:	0b1b      	lsrs	r3, r3, #12
 f80422c:	f003 0301 	and.w	r3, r3, #1
 f804230:	2b00      	cmp	r3, #0
 f804232:	d035      	beq.n	f8042a0 <set_psram_wakeup_mode+0x2cc>
 f804234:	6f3b      	ldr	r3, [r7, #112]	; 0x70
 f804236:	681b      	ldr	r3, [r3, #0]
 f804238:	66bb      	str	r3, [r7, #104]	; 0x68
 f80423a:	6f3b      	ldr	r3, [r7, #112]	; 0x70
 f80423c:	6eba      	ldr	r2, [r7, #104]	; 0x68
 f80423e:	601a      	str	r2, [r3, #0]
 f804240:	2000      	movs	r0, #0
 f804242:	4b29      	ldr	r3, [pc, #164]	; (f8042e8 <set_psram_wakeup_mode+0x314>)
 f804244:	4798      	blx	r3
 f804246:	4a27      	ldr	r2, [pc, #156]	; (f8042e4 <set_psram_wakeup_mode+0x310>)
 f804248:	6f7b      	ldr	r3, [r7, #116]	; 0x74
 f80424a:	f8d3 3110 	ldr.w	r3, [r3, #272]	; 0x110
 f80424e:	6013      	str	r3, [r2, #0]
 f804250:	4b24      	ldr	r3, [pc, #144]	; (f8042e4 <set_psram_wakeup_mode+0x310>)
 f804252:	f503 23a0 	add.w	r3, r3, #327680	; 0x50000
 f804256:	461a      	mov	r2, r3
 f804258:	6f7b      	ldr	r3, [r7, #116]	; 0x74
 f80425a:	f8d3 3114 	ldr.w	r3, [r3, #276]	; 0x114
 f80425e:	6013      	str	r3, [r2, #0]
 f804260:	4b20      	ldr	r3, [pc, #128]	; (f8042e4 <set_psram_wakeup_mode+0x310>)
 f804262:	f503 1380 	add.w	r3, r3, #1048576	; 0x100000
 f804266:	461a      	mov	r2, r3
 f804268:	6f7b      	ldr	r3, [r7, #116]	; 0x74
 f80426a:	f8d3 3118 	ldr.w	r3, [r3, #280]	; 0x118
 f80426e:	6013      	str	r3, [r2, #0]
 f804270:	4b1c      	ldr	r3, [pc, #112]	; (f8042e4 <set_psram_wakeup_mode+0x310>)
 f804272:	f503 13a8 	add.w	r3, r3, #1376256	; 0x150000
 f804276:	461a      	mov	r2, r3
 f804278:	6f7b      	ldr	r3, [r7, #116]	; 0x74
 f80427a:	f8d3 311c 	ldr.w	r3, [r3, #284]	; 0x11c
 f80427e:	6013      	str	r3, [r2, #0]
 f804280:	4b18      	ldr	r3, [pc, #96]	; (f8042e4 <set_psram_wakeup_mode+0x310>)
 f804282:	f503 1300 	add.w	r3, r3, #2097152	; 0x200000
 f804286:	461a      	mov	r2, r3
 f804288:	6f7b      	ldr	r3, [r7, #116]	; 0x74
 f80428a:	f8d3 3120 	ldr.w	r3, [r3, #288]	; 0x120
 f80428e:	6013      	str	r3, [r2, #0]
 f804290:	4b14      	ldr	r3, [pc, #80]	; (f8042e4 <set_psram_wakeup_mode+0x310>)
 f804292:	f503 1314 	add.w	r3, r3, #2424832	; 0x250000
 f804296:	461a      	mov	r2, r3
 f804298:	6f7b      	ldr	r3, [r7, #116]	; 0x74
 f80429a:	f8d3 3124 	ldr.w	r3, [r3, #292]	; 0x124
 f80429e:	6013      	str	r3, [r2, #0]
 f8042a0:	6efb      	ldr	r3, [r7, #108]	; 0x6c
 f8042a2:	f8d3 2140 	ldr.w	r2, [r3, #320]	; 0x140
 f8042a6:	4b11      	ldr	r3, [pc, #68]	; (f8042ec <set_psram_wakeup_mode+0x318>)
 f8042a8:	4013      	ands	r3, r2
 f8042aa:	6efa      	ldr	r2, [r7, #108]	; 0x6c
 f8042ac:	f8c2 3140 	str.w	r3, [r2, #320]	; 0x140
 f8042b0:	687b      	ldr	r3, [r7, #4]
 f8042b2:	2b00      	cmp	r3, #0
 f8042b4:	d110      	bne.n	f8042d8 <set_psram_wakeup_mode+0x304>
 f8042b6:	4b0e      	ldr	r3, [pc, #56]	; (f8042f0 <set_psram_wakeup_mode+0x31c>)
 f8042b8:	689b      	ldr	r3, [r3, #8]
 f8042ba:	3b03      	subs	r3, #3
 f8042bc:	4a0d      	ldr	r2, [pc, #52]	; (f8042f4 <set_psram_wakeup_mode+0x320>)
 f8042be:	5cd3      	ldrb	r3, [r2, r3]
 f8042c0:	015b      	lsls	r3, r3, #5
 f8042c2:	b2db      	uxtb	r3, r3
 f8042c4:	723b      	strb	r3, [r7, #8]
 f8042c6:	7a3b      	ldrb	r3, [r7, #8]
 f8042c8:	727b      	strb	r3, [r7, #9]
 f8042ca:	f107 0308 	add.w	r3, r7, #8
 f8042ce:	2202      	movs	r2, #2
 f8042d0:	2104      	movs	r1, #4
 f8042d2:	2000      	movs	r0, #0
 f8042d4:	f000 f860 	bl	f804398 <PSRAM_REG_Write>
 f8042d8:	bf00      	nop
 f8042da:	3778      	adds	r7, #120	; 0x78
 f8042dc:	46bd      	mov	sp, r7
 f8042de:	bd80      	pop	{r7, pc}
 f8042e0:	e000ed00 	.word	0xe000ed00
 f8042e4:	60000000 	.word	0x60000000
 f8042e8:	0f800eb1 	.word	0x0f800eb1
 f8042ec:	ff0000ff 	.word	0xff0000ff
 f8042f0:	3000a1e8 	.word	0x3000a1e8
 f8042f4:	3000a1c4 	.word	0x3000a1c4

0f8042f8 <PSRAM_REG_Read>:
 f8042f8:	b470      	push	{r4, r5, r6}
 f8042fa:	468c      	mov	ip, r1
 f8042fc:	9e03      	ldr	r6, [sp, #12]
 f8042fe:	4d25      	ldr	r5, [pc, #148]	; (f804394 <PSRAM_REG_Read+0x9c>)
 f804300:	6aac      	ldr	r4, [r5, #40]	; 0x28
 f804302:	f014 0401 	ands.w	r4, r4, #1
 f804306:	d1fb      	bne.n	f804300 <PSRAM_REG_Read+0x8>
 f804308:	6829      	ldr	r1, [r5, #0]
 f80430a:	f041 4100 	orr.w	r1, r1, #2147483648	; 0x80000000
 f80430e:	6029      	str	r1, [r5, #0]
 f804310:	f022 417f 	bic.w	r1, r2, #4278190080	; 0xff000000
 f804314:	60ac      	str	r4, [r5, #8]
 f804316:	6069      	str	r1, [r5, #4]
 f804318:	f8c5 4130 	str.w	r4, [r5, #304]	; 0x130
 f80431c:	6829      	ldr	r1, [r5, #0]
 f80431e:	f441 7140 	orr.w	r1, r1, #768	; 0x300
 f804322:	6029      	str	r1, [r5, #0]
 f804324:	bb78      	cbnz	r0, f804386 <PSRAM_REG_Read+0x8e>
 f804326:	2140      	movs	r1, #64	; 0x40
 f804328:	f885 1060 	strb.w	r1, [r5, #96]	; 0x60
 f80432c:	f885 1060 	strb.w	r1, [r5, #96]	; 0x60
 f804330:	4918      	ldr	r1, [pc, #96]	; (f804394 <PSRAM_REG_Read+0x9c>)
 f804332:	b32e      	cbz	r6, f804380 <PSRAM_REG_Read+0x88>
 f804334:	2001      	movs	r0, #1
 f804336:	f881 0060 	strb.w	r0, [r1, #96]	; 0x60
 f80433a:	4816      	ldr	r0, [pc, #88]	; (f804394 <PSRAM_REG_Read+0x9c>)
 f80433c:	2400      	movs	r4, #0
 f80433e:	fa5f f18c 	uxtb.w	r1, ip
 f804342:	2501      	movs	r5, #1
 f804344:	f880 4060 	strb.w	r4, [r0, #96]	; 0x60
 f804348:	f880 4060 	strb.w	r4, [r0, #96]	; 0x60
 f80434c:	f880 1060 	strb.w	r1, [r0, #96]	; 0x60
 f804350:	6085      	str	r5, [r0, #8]
 f804352:	6881      	ldr	r1, [r0, #8]
 f804354:	07c9      	lsls	r1, r1, #31
 f804356:	d4fc      	bmi.n	f804352 <PSRAM_REG_Read+0x5a>
 f804358:	b142      	cbz	r2, f80436c <PSRAM_REG_Read+0x74>
 f80435a:	3b01      	subs	r3, #1
 f80435c:	480d      	ldr	r0, [pc, #52]	; (f804394 <PSRAM_REG_Read+0x9c>)
 f80435e:	1899      	adds	r1, r3, r2
 f804360:	f890 2060 	ldrb.w	r2, [r0, #96]	; 0x60
 f804364:	f803 2f01 	strb.w	r2, [r3, #1]!
 f804368:	428b      	cmp	r3, r1
 f80436a:	d1f9      	bne.n	f804360 <PSRAM_REG_Read+0x68>
 f80436c:	4a09      	ldr	r2, [pc, #36]	; (f804394 <PSRAM_REG_Read+0x9c>)
 f80436e:	6a93      	ldr	r3, [r2, #40]	; 0x28
 f804370:	07db      	lsls	r3, r3, #31
 f804372:	d4fc      	bmi.n	f80436e <PSRAM_REG_Read+0x76>
 f804374:	6813      	ldr	r3, [r2, #0]
 f804376:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
 f80437a:	6013      	str	r3, [r2, #0]
 f80437c:	bc70      	pop	{r4, r5, r6}
 f80437e:	4770      	bx	lr
 f804380:	f881 6060 	strb.w	r6, [r1, #96]	; 0x60
 f804384:	e7d9      	b.n	f80433a <PSRAM_REG_Read+0x42>
 f804386:	21e0      	movs	r1, #224	; 0xe0
 f804388:	f885 1060 	strb.w	r1, [r5, #96]	; 0x60
 f80438c:	f885 4060 	strb.w	r4, [r5, #96]	; 0x60
 f804390:	e7ce      	b.n	f804330 <PSRAM_REG_Read+0x38>
 f804392:	bf00      	nop
 f804394:	40127000 	.word	0x40127000

0f804398 <PSRAM_REG_Write>:
 f804398:	b430      	push	{r4, r5}
 f80439a:	468c      	mov	ip, r1
 f80439c:	4d22      	ldr	r5, [pc, #136]	; (f804428 <PSRAM_REG_Write+0x90>)
 f80439e:	6aac      	ldr	r4, [r5, #40]	; 0x28
 f8043a0:	f014 0401 	ands.w	r4, r4, #1
 f8043a4:	d1fb      	bne.n	f80439e <PSRAM_REG_Write+0x6>
 f8043a6:	6829      	ldr	r1, [r5, #0]
 f8043a8:	f041 4100 	orr.w	r1, r1, #2147483648	; 0x80000000
 f8043ac:	6029      	str	r1, [r5, #0]
 f8043ae:	60ac      	str	r4, [r5, #8]
 f8043b0:	6829      	ldr	r1, [r5, #0]
 f8043b2:	f421 7140 	bic.w	r1, r1, #768	; 0x300
 f8043b6:	6029      	str	r1, [r5, #0]
 f8043b8:	f022 417f 	bic.w	r1, r2, #4278190080	; 0xff000000
 f8043bc:	606c      	str	r4, [r5, #4]
 f8043be:	f8c5 1130 	str.w	r1, [r5, #304]	; 0x130
 f8043c2:	bb50      	cbnz	r0, f80441a <PSRAM_REG_Write+0x82>
 f8043c4:	21c0      	movs	r1, #192	; 0xc0
 f8043c6:	f885 1060 	strb.w	r1, [r5, #96]	; 0x60
 f8043ca:	f885 1060 	strb.w	r1, [r5, #96]	; 0x60
 f8043ce:	4816      	ldr	r0, [pc, #88]	; (f804428 <PSRAM_REG_Write+0x90>)
 f8043d0:	2400      	movs	r4, #0
 f8043d2:	2501      	movs	r5, #1
 f8043d4:	fa5f f18c 	uxtb.w	r1, ip
 f8043d8:	f880 5060 	strb.w	r5, [r0, #96]	; 0x60
 f8043dc:	f880 4060 	strb.w	r4, [r0, #96]	; 0x60
 f8043e0:	f880 4060 	strb.w	r4, [r0, #96]	; 0x60
 f8043e4:	f880 1060 	strb.w	r1, [r0, #96]	; 0x60
 f8043e8:	b13a      	cbz	r2, f8043fa <PSRAM_REG_Write+0x62>
 f8043ea:	3b01      	subs	r3, #1
 f8043ec:	1899      	adds	r1, r3, r2
 f8043ee:	f813 2f01 	ldrb.w	r2, [r3, #1]!
 f8043f2:	428b      	cmp	r3, r1
 f8043f4:	f880 2060 	strb.w	r2, [r0, #96]	; 0x60
 f8043f8:	d1f9      	bne.n	f8043ee <PSRAM_REG_Write+0x56>
 f8043fa:	4a0b      	ldr	r2, [pc, #44]	; (f804428 <PSRAM_REG_Write+0x90>)
 f8043fc:	2301      	movs	r3, #1
 f8043fe:	6093      	str	r3, [r2, #8]
 f804400:	6893      	ldr	r3, [r2, #8]
 f804402:	07d9      	lsls	r1, r3, #31
 f804404:	d4fc      	bmi.n	f804400 <PSRAM_REG_Write+0x68>
 f804406:	4a08      	ldr	r2, [pc, #32]	; (f804428 <PSRAM_REG_Write+0x90>)
 f804408:	6a93      	ldr	r3, [r2, #40]	; 0x28
 f80440a:	07db      	lsls	r3, r3, #31
 f80440c:	d4fc      	bmi.n	f804408 <PSRAM_REG_Write+0x70>
 f80440e:	6813      	ldr	r3, [r2, #0]
 f804410:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
 f804414:	6013      	str	r3, [r2, #0]
 f804416:	bc30      	pop	{r4, r5}
 f804418:	4770      	bx	lr
 f80441a:	2160      	movs	r1, #96	; 0x60
 f80441c:	f885 1060 	strb.w	r1, [r5, #96]	; 0x60
 f804420:	f885 4060 	strb.w	r4, [r5, #96]	; 0x60
 f804424:	e7d3      	b.n	f8043ce <PSRAM_REG_Write+0x36>
 f804426:	bf00      	nop
 f804428:	40127000 	.word	0x40127000

0f80442c <set_psram_sleep_mode>:
 f80442c:	b530      	push	{r4, r5, lr}
 f80442e:	4b29      	ldr	r3, [pc, #164]	; (f8044d4 <set_psram_sleep_mode+0xa8>)
 f804430:	f240 15f1 	movw	r5, #497	; 0x1f1
 f804434:	4928      	ldr	r1, [pc, #160]	; (f8044d8 <set_psram_sleep_mode+0xac>)
 f804436:	b083      	sub	sp, #12
 f804438:	4a28      	ldr	r2, [pc, #160]	; (f8044dc <set_psram_sleep_mode+0xb0>)
 f80443a:	f503 24a0 	add.w	r4, r3, #327680	; 0x50000
 f80443e:	600d      	str	r5, [r1, #0]
 f804440:	6819      	ldr	r1, [r3, #0]
 f804442:	f8c2 1110 	str.w	r1, [r2, #272]	; 0x110
 f804446:	f503 1180 	add.w	r1, r3, #1048576	; 0x100000
 f80444a:	6824      	ldr	r4, [r4, #0]
 f80444c:	f8c2 4114 	str.w	r4, [r2, #276]	; 0x114
 f804450:	f503 14a8 	add.w	r4, r3, #1376256	; 0x150000
 f804454:	6809      	ldr	r1, [r1, #0]
 f804456:	f8c2 1118 	str.w	r1, [r2, #280]	; 0x118
 f80445a:	f503 1100 	add.w	r1, r3, #2097152	; 0x200000
 f80445e:	6824      	ldr	r4, [r4, #0]
 f804460:	f503 1314 	add.w	r3, r3, #2424832	; 0x250000
 f804464:	f8c2 411c 	str.w	r4, [r2, #284]	; 0x11c
 f804468:	6809      	ldr	r1, [r1, #0]
 f80446a:	f8c2 1120 	str.w	r1, [r2, #288]	; 0x120
 f80446e:	681b      	ldr	r3, [r3, #0]
 f804470:	f8c2 3124 	str.w	r3, [r2, #292]	; 0x124
 f804474:	bb10      	cbnz	r0, f8044bc <set_psram_sleep_mode+0x90>
 f804476:	491a      	ldr	r1, [pc, #104]	; (f8044e0 <set_psram_sleep_mode+0xb4>)
 f804478:	4604      	mov	r4, r0
 f80447a:	4a1a      	ldr	r2, [pc, #104]	; (f8044e4 <set_psram_sleep_mode+0xb8>)
 f80447c:	f24f 05f0 	movw	r5, #61680	; 0xf0f0
 f804480:	6889      	ldr	r1, [r1, #8]
 f804482:	ab01      	add	r3, sp, #4
 f804484:	f8ad 5000 	strh.w	r5, [sp]
 f804488:	eb02 0c01 	add.w	ip, r2, r1
 f80448c:	2202      	movs	r2, #2
 f80448e:	2104      	movs	r1, #4
 f804490:	f81c cc03 	ldrb.w	ip, [ip, #-3]
 f804494:	ea4f 1c4c 	mov.w	ip, ip, lsl #5
 f804498:	f04c 0c08 	orr.w	ip, ip, #8
 f80449c:	fa5f fc8c 	uxtb.w	ip, ip
 f8044a0:	f88d c004 	strb.w	ip, [sp, #4]
 f8044a4:	f88d c005 	strb.w	ip, [sp, #5]
 f8044a8:	f7ff ff76 	bl	f804398 <PSRAM_REG_Write>
 f8044ac:	466b      	mov	r3, sp
 f8044ae:	2202      	movs	r2, #2
 f8044b0:	2106      	movs	r1, #6
 f8044b2:	4620      	mov	r0, r4
 f8044b4:	f7ff ff70 	bl	f804398 <PSRAM_REG_Write>
 f8044b8:	b003      	add	sp, #12
 f8044ba:	bd30      	pop	{r4, r5, pc}
 f8044bc:	2101      	movs	r1, #1
 f8044be:	f242 04ff 	movw	r4, #8447	; 0x20ff
 f8044c2:	466b      	mov	r3, sp
 f8044c4:	2202      	movs	r2, #2
 f8044c6:	4608      	mov	r0, r1
 f8044c8:	f8ad 4000 	strh.w	r4, [sp]
 f8044cc:	f7ff ff64 	bl	f804398 <PSRAM_REG_Write>
 f8044d0:	b003      	add	sp, #12
 f8044d2:	bd30      	pop	{r4, r5, pc}
 f8044d4:	60000000 	.word	0x60000000
 f8044d8:	40126000 	.word	0x40126000
 f8044dc:	41008e00 	.word	0x41008e00
 f8044e0:	3000a1e8 	.word	0x3000a1e8
 f8044e4:	3000a1c4 	.word	0x3000a1c4

0f8044e8 <__IPC_SEMFree_veneer>:
 f8044e8:	f85f f000 	ldr.w	pc, [pc]	; f8044ec <__IPC_SEMFree_veneer+0x4>
 f8044ec:	0000c135 	.word	0x0000c135

0f8044f0 <__SBOOT_Validate_ImgHash_veneer>:
 f8044f0:	f85f f000 	ldr.w	pc, [pc]	; f8044f4 <__SBOOT_Validate_ImgHash_veneer+0x4>
 f8044f4:	00001a75 	.word	0x00001a75

0f8044f8 <__INT_HardFault_C_veneer>:
 f8044f8:	f85f f000 	ldr.w	pc, [pc]	; f8044fc <__INT_HardFault_C_veneer+0x4>
 f8044fc:	0000cded 	.word	0x0000cded

0f804500 <__SBOOT_Validate_PubKey_veneer>:
 f804500:	f85f f000 	ldr.w	pc, [pc]	; f804504 <__SBOOT_Validate_PubKey_veneer+0x4>
 f804504:	00001a05 	.word	0x00001a05

0f804508 <__SBOOT_Validate_Algorithm_veneer>:
 f804508:	f85f f000 	ldr.w	pc, [pc]	; f80450c <__SBOOT_Validate_Algorithm_veneer+0x4>
 f80450c:	0000199d 	.word	0x0000199d

0f804510 <__flash_handshake_highspeed_veneer>:
 f804510:	f85f f000 	ldr.w	pc, [pc]	; f804514 <__flash_handshake_highspeed_veneer+0x4>
 f804514:	3000a051 	.word	0x3000a051

0f804518 <__flash_rx_mode_switch_veneer>:
 f804518:	f85f f000 	ldr.w	pc, [pc]	; f80451c <__flash_rx_mode_switch_veneer+0x4>
 f80451c:	3000a0cd 	.word	0x3000a0cd

0f804520 <__SBOOT_Validate_Signature_veneer>:
 f804520:	f85f f000 	ldr.w	pc, [pc]	; f804524 <__SBOOT_Validate_Signature_veneer+0x4>
 f804524:	000019e1 	.word	0x000019e1

0f804528 <____wrap_memset_veneer>:
 f804528:	f85f f000 	ldr.w	pc, [pc]	; f80452c <____wrap_memset_veneer+0x4>
 f80452c:	0001210d 	.word	0x0001210d

0f804530 <Boot_Tzcfg_En>:
 f804530:	00000000                                ....

0f804534 <RCC_Config>:
 f804534:	c0000010 c0000010 00000001 c0000004     ................
 f804544:	c0000004 00000001 40000010 40000010     ...........@...@
 f804554:	00000001 00000020 00000020 00000001     .... ... .......
 f804564:	00000200 00000200 00000001 00000100     ................
 f804574:	00000100 00000001 00000080 00000080     ................
 f804584:	00000001 40001000 40001000 00000001     .......@...@....
 f804594:	42000000 42000000 00000001 44000000     ...B...B.......D
 f8045a4:	44000000 00000001 ffffffff 00000000     ...D............
 f8045b4:	00000001                                ....

0f8045b8 <SocClk_Info>:
 f8045b8:	1efe9200 02050200 13baa8c0 01030101     ................
 f8045c8:	17d78400 01040200 1c9c3800 02050200     .........8......
 f8045d8:	285ff000 02060201 29040000 02060201     .._(.......)....
 f8045e8:	6e6f4e0a 6365532d 20657275 6c726f77     .Non-Secure worl
 f8045f8:	61632064 746f6e6e 6d756420 65722070     d cannot dump re
 f804608:	74736967 20737265 756f6261 78652074     gisters about ex
 f804618:	74706563 206e6f69 6d6f7266 63657320     ception from sec
 f804628:	20657275 6c726f77 000a2164 4c554146     ure world!..FAUL
 f804638:	00000054 52534653 203d2020 30257830     T...SFSR  = 0x%0
 f804648:	0d786c38 0000000a 52414653 203d2020     8lx.....SFAR  = 
 f804658:	30257830 0d786c38 0000000a 544e4f43     0x%08lx.....CONT
 f804668:	5f4c4f52 3d20534e 25783020 586c3830     ROL_NS = 0x%08lX
 f804678:	00000a0d 53434853 203d2052 30257830     ....SHCSR = 0x%0
 f804688:	0d786c38 0000000a 43524941 203d2052     8lx.....AIRCR = 
 f804698:	30257830 0d786c38 0000000a 544e4f43     0x%08lx.....CONT
 f8046a8:	204c4f52 7830203d 6c383025 000a0d78     ROL = 0x%08lx...

0f8046b8 <Flash_AVL>:
 f8046b8:	000000ef 000000ff 00000000 000043fc     .............C..
 f8046c8:	00000000 000000a1 000000ff 00000000     ................
 f8046d8:	0000fffc 00000000 0000000b 000000ff     ................
 f8046e8:	00000000 000043fc 00000000 0000000e     .....C..........
 f8046f8:	000000ff 00000000 000043fc 00000000     .........C......
 f804708:	000000c8 000000ff 00000001 000043fc     .............C..
 f804718:	00000000 000028c2 0000ffff 00000005     .....(..........
 f804728:	000200fc 00000000 000000c2 000000ff     ................
 f804738:	00000002 000000fc 00000000 00000068     ............h...
 f804748:	000000ff 00000002 000000fc 00000000     ................
 f804758:	00000051 000000ff 00000002 000000fc     Q...............
 f804768:	00000000 0000001c 000000ff 00000003     ................
 f804778:	000000fc 00000000 00000020 000000ff     ........ .......
 f804788:	00000000 000043fc 00000000 00000085     .....C..........
 f804798:	000000ff 00000000 000043fc 00000000     .........C......
 f8047a8:	0000005e 000000ff 00000000 000043fc     ^............C..
	...
 f8047c0:	000000ff 000000fe ffffffff 0f8001c1     ................
 f8047d0:	000000ff ffffffff 000000ff ffffffff     ................
 f8047e0:	00000000                                ....

0f8047e4 <Flash_Layout>:
 f8047e4:	00000000 08000000 08013fff 00000002     .........?......
 f8047f4:	08014000 081f3fff 00000001 08200000     .@...?........ .
 f804804:	08213fff 00000003 08214000 083f3fff     .?!......@!..??.
 f804814:	00000004 08700000 08702fff 00000005     ......p../p.....
 f804824:	08703000 08722fff 00000007 ffffffff     .0p../r.........
 f804834:	ffffffff 000000ff ffffffff ffffffff     ................

0f804844 <Flash_ReadMode>:
 f804844:	                                         ..

0f804846 <Flash_Speed>:
 f804846:	49310002 4f32004f 49320000 4f34004f     ..1IO.2O..2IO.4O
 f804856:	49340000 4146004f 00004c49 4b4f0000     ..4IO.FAIL....OK
 f804866:	4c460000 20485341 646e6148 6b616853     ..FLASH HandShak
 f804876:	78305b65 25207825 000a5d73 4c460000     e[0x%x %s]....FL
 f804886:	00485341 6c460000 20687361 74697753     ASH...Flash Swit
 f804896:	52206863 20646165 65646f4d 49414620     ch Read Mode FAI
 f8048a6:	00000a4c 6c460000 20687361 64616552     L.....Flash Read
 f8048b6:	0a732520 0e0d0000 1211100f 6c460000      %s...........Fl
 f8048c6:	20687361 203a4449 252d7825 78252d78     ash ID: %x-%x-%x
 f8048d6:	6854000a 66207369 6873616c 70797420     ..This flash typ
 f8048e6:	73692065 746f6e20 70757320 74726f70     e is not support
 f8048f6:	0a216465 2f2e0000 706d6f63 6e656e6f     ed!..../componen
 f804906:	6f732f74 6d612f63 64616265 73756c70     t/soc/amebadplus
 f804916:	6c77662f 722f6269 6b5f6d61 612f346d     /fwlib/ram_km4/a
 f804926:	6162656d 616c665f 6c636873 00632e6b     meba_flashclk.c.
 f804936:	48480000 484c0f80 48500f80 48540f80     ..HH..LH..PH..TH
 f804946:	48580f80 01000f80 05040302 544f0000     ..XH..........OT
 f804956:	4f422050 5620544f 63204c4f 736f6f68     P BOOT VOL choos
 f804966:	2e312065 62205630 75207475 66637273     e 1.0V but usrcf
 f804976:	68632067 65736f6f 392e3020 000a2156     g choose 0.9V!..
 f804986:	4f420000 0000544f 6e490000 696c6176     ..BOOT....Invali
 f804996:	73502064 436d6172 000a6b6c 53500000     d PsramClk....PS
 f8049a6:	004d4152 44420000 206d756e 30207369     RAM...BDnum is 0
 f8049b6:	0a782578 53530000 524e4549 25783009     x%x...SSIENR.0x%
 f8049c6:	78303d78 000a7825 54430000 30524c52     x=0x%x....CTRLR0
 f8049d6:	25783009 78303d78 000a7825 41420000     .0x%x=0x%x....BA
 f8049e6:	09524455 78257830 2578303d 00000a78     UDR.0x%x=0x%x...
 f8049f6:	41560000 5f44494c 09444d43 78257830     ..VALID_CMD.0x%x
 f804a06:	2578303d 00000a78 54430000 32524c52     =0x%x.....CTRLR2
 f804a16:	25783009 78303d78 000a7825 45440000     .0x%x=0x%x....DE
 f804a26:	45434956 464e495f 7830094f 303d7825     VICE_INFO.0x%x=0
 f804a36:	0a782578 50540000 30093052 3d782578     x%x...TPR0.0x%x=
 f804a46:	78257830 4552000a 465f4441 5f545341     0x%x..READ_FAST_
 f804a56:	474e4953 3009454c 3d782578 78257830     SINGLE.0x%x=0x%x
 f804a66:	5257000a 5f455449 4e474953 3009454c     ..WRITE_SIGNLE.0
 f804a76:	3d782578 78257830 5541000a 4c5f4f54     x%x=0x%x..AUTO_L
 f804a86:	54474e45 78300948 303d7825 0a782578     ENGTH.0x%x=0x%x.
 f804a96:	53550000 4c5f5245 54474e45 78300948     ..USER_LENGTH.0x
 f804aa6:	303d7825 0a782578 55410000 4c5f4f54     %x=0x%x...AUTO_L
 f804ab6:	54474e45 30093248 3d782578 78257830     ENGTH2.0x%x=0x%x
 f804ac6:	5253000a 25783009 78303d78 000a7825     ..SR.0x%x=0x%x..
 f804ad6:	53500000 5f594850 5f4c4143 09524150     ..PSPHY_CAL_PAR.
 f804ae6:	78257830 2578303d 00000a78 53500000     0x%x=0x%x.....PS
 f804af6:	5f594850 5f594850 4c525443 25783009     PHY_PHY_CTRL.0x%
 f804b06:	78303d78 000a7825 53500000 5f594850     x=0x%x....PSPHY_
 f804b16:	5f474244 094c4553 78257830 2578303d     DBG_SEL.0x%x=0x%
 f804b26:	00000a78 52490000 5d315b30 2578303d     x.....IR0[1]=0x%
 f804b36:	00000a78 52490000 5d305b30 2578303d     x.....IR0[0]=0x%
 f804b46:	00000a78 52430000 5d315b30 2578303d     x.....CR0[1]=0x%
 f804b56:	00000a78 52430000 5d305b30 2578303d     x.....CR0[0]=0x%
 f804b66:	00000a78 52430000 5d315b31 2578303d     x.....CR1[1]=0x%
 f804b76:	00000a78 52430000 5d305b31 2578303d     x.....CR1[0]=0x%
 f804b86:	00000a78 54430000 30524c52 25203d20     x.....CTRLR0 = %
 f804b96:	6c252070 00000a78 3d3d0000 6168703d     p %lx.....===pha
 f804ba6:	25206573 3d20786c 3d3d3d3d 6b6f000a     se %lx =====..ok
 f804bb6:	786c2520 786c2520 786c2520 786c2520      %lx %lx %lx %lx
 f804bc6:	786c2520 786c2520 786c2520 6166000a      %lx %lx %lx..fa
 f804bd6:	25206c69 2520786c 2520786c 2520786c     il %lx %lx %lx %
 f804be6:	2520786c 2520786c 2520786c 000a786c     lx %lx %lx %lx..
 f804bf6:	61430000 696d4e6c 203d206e 43207825     ..CalNmin = %x C
 f804c06:	6d4e6c61 3d207861 20782520 646e6957     alNmax = %x Wind
 f804c16:	6953776f 3d20657a 20782520 73616870     owSize = %x phas
 f804c26:	25203a65 000a2078                        e: %x ....

0f804c30 <PSRAM_CALIB_PATTERN>:
 f804c30:	11223344 a55aa55a 5aa55aa5 44332211     D3".Z.Z..Z.Z."3D
 f804c40:	96696996 69969669 74696e49 4d504120     .ii.i..iInit APM
 f804c50:	52535020 0a0d4d41 00000000 74696e49      PSRAM......Init
 f804c60:	20425720 41525350 000a0d4d 20344d4b      WB PSRAM...KM4 
 f804c70:	544f4f42 41455220 3a4e4f53 786c2520     BOOT REASON: %lx
 f804c80:	00000a20 6f632f2e 6e6f706d 2f746e65      ..../component/
 f804c90:	2f636f73 62656d61 6c706461 622f7375     soc/amebadplus/b
 f804ca0:	6c746f6f 6564616f 6f622f72 6f6c746f     ootloader/bootlo
 f804cb0:	72656461 346d6b5f 0000632e 20344d4b     ader_km4.c..KM4 
 f804cc0:	20555043 3a4b4c43 756c2520 0a7a4820     CPU CLK: %lu Hz.
 f804cd0:	00000000 20304d4b 20555043 3a4b4c43     ....KM0 CPU CLK:
 f804ce0:	756c2520 0a7a4820 00000000 41525350      %lu Hz.....PSRA
 f804cf0:	7443204d 43206c72 203a4b4c 20756c25     M Ctrl CLK: %lu 
 f804d00:	0a207a48 00000000 31474d49 544e4520     Hz .....IMG1 ENT
 f804d10:	4d205245 5b3a5053 6c383025 000a5d58     ER MSP:[%08lX]..
 f804d20:	6c697542 69542064 203a656d 25207325     Build Time: %s %
 f804d30:	00000a73 343a3131 37353a34 00000000     s...11:44:57....
 f804d40:	2079614d 32203731 00343230 31474d49     May 17 2024.IMG1
 f804d50:	43455320 20455255 54415453 25203a45      SECURE STATE: %
 f804d60:	00000a64 67616d49 6e453265 20797274     d...Image2Entry 
 f804d70:	78302040 20786c25 0d2e2e2e 0000000a     @ 0x%lx ........
 f804d80:	00000204 00000208 00000000 00000008     ................
 f804d90:	0000020c 00000210 00000000 0000000c     ................
 f804da0:	20304d4b 20504958 00474d49 20304d4b     KM0 XIP IMG.KM0 
 f804db0:	4d415253 00000000 20304d4b 41525350     SRAM....KM0 PSRA
 f804dc0:	0000004d 20304d4b 52544e45 00000059     M...KM0 ENTRY...
 f804dd0:	20344d4b 20504958 00474d49 20344d4b     KM4 XIP IMG.KM4 
 f804de0:	4d415253 00000000 20344d4b 41525350     SRAM....KM4 PSRA
 f804df0:	0000004d 20344d4b 52544e45 00000059     M...KM4 ENTRY...
 f804e00:	49207325 6c61766e 000a6469 255b7325     %s Invalid..%s[%
 f804e10:	786c3830 786c253a 00000a5d 32474d49     08lx:%lx]...IMG2
 f804e20:	46544f20 0a4e4520 00000000 2046544f      OTF EN.....OTF 
 f804e30:	65646f4d 72726520 0a0d726f 00000000     Mode error......
 f804e40:	6f632f2e 6e6f706d 2f746e65 2f636f73     ./component/soc/
 f804e50:	62656d61 6c706461 622f7375 6c746f6f     amebadplus/bootl
 f804e60:	6564616f 6f622f72 6f5f746f 6b5f6174     oader/boot_ota_k
 f804e70:	632e346d 00000000 32474d49 4f4f4220     m4.c....IMG2 BOO
 f804e80:	72662054 4f206d6f 25204154 56202c64     T from OTA %d, V
 f804e90:	69737265 203a6e6f 2e786c25 20786c25     ersion: %lx.%lx 
 f804ea0:	0000000a 2041544f 74726543 63696669     ....OTA Certific
 f804eb0:	20657461 4d492026 69203247 6c61766e     ate & IMG2 inval
 f804ec0:	202c6469 544f4f42 49414620 0a21214c     id, BOOT FAIL!!.
 f804ed0:	00000000 0f804da0 0f804dac 0f804db8     .....M...M...M..
 f804ee0:	0f804dc4 0f804dd0 0f804ddc 0f804de8     .M...M...M...M..
 f804ef0:	0f804df4                                .M..

0f804ef4 <ImagePattern>:
 f804ef4:	35393138 31313738 20344d4b 33474d49     81958711KM4 IMG3
 f804f04:	00000000 20344d4b 0043534e 31474d49     ....KM4 NSC.IMG1
 f804f14:	00000000 32474d49 00000000 20504452     ....IMG2....RDP 
 f804f24:	67616d69 65442065 70797263 61462074     image Decrypt Fa
 f804f34:	0a216c69 00000000 20434544 255b7325     il!.....DEC %s[%
 f804f44:	586c3830 786c253a 00000a5d 56207325     08lX:%lx]...%s V
 f804f54:	46495245 41502059 000a5353 56207325     ERIFY PASS..%s V
 f804f64:	46495245 41462059 202c4c49 20746572     ERIFY FAIL, ret 
 f804f74:	6425203d 0000000a 74726543 63696669     = %d....Certific
 f804f84:	20657461 49524556 46205946 2c4c4941     ate VERIFY FAIL,
 f804f94:	74657220 25203d20 00000a64 0f804efc      ret = %d....N..
 f804fa4:	0f804f08 0f804f10 0f804f18 0f804f00     .O...O...O...O..
 f804fb4:	2d73255b 205d6325 00000000 61766e49     [%s-%c] ....Inva
 f804fc4:	2064696c 6e204442 65626d75 000a2172     lid BD number!..
 f804fd4:	50494843 4f464e49 00000000              CHIPINFO....

0f804fe0 <Chip_Info>:
 f804fe0:	03f40100 00020107 03f70200 00040000     ................
 f804ff0:	03f80300 00040000 04030400 00080000     ................
 f805000:	04010500 00400000 04000600 00040000     ......@.........
 f805010:	04090700 00020107 04020800 00040000     ................
 f805020:	040a0900 00020107 04110a00 00020107     ................
 f805030:	04120b00 00040000 04130c00 00040000     ................
 f805040:	04160d00 00080000 04170e00 00080000     ................
 f805050:	04190f00 0001020a 041a1000 00080000     ................
 f805060:	04201100 00400000 041c1200 00020107     .. ...@.........
 f805070:	041d0e01 00080000 041f0901 00020107     ................
 f805080:	040f0501 00400000 ffffffff 00400000     ......@.......@.
 f805090:	2050544f 20746567 73207768 20616d65     OTP get hw sema 
 f8050a0:	6c696166 0000000a 504d4f52 48435441     fail....ROMPATCH
 f8050b0:	00000000 64206f69 65766972 61702072     ....io driver pa
 f8050c0:	656d6172 73726574 72726520 2021726f     rameters error! 
 f8050d0:	656c6966 6d616e5f 25203a65 6c202c73     file_name: %s, l
 f8050e0:	3a656e69 00642520 2a2a2a2a 2a2a2a2a     ine: %d.********
 f8050f0:	2a2a2a2a 2a2a2a2a 2a2a2a2a 2a2a2a2a     ****************
 f805100:	57202a2a 494e5241 2a20474e 2a2a2a2a     ** WARNING *****
 f805110:	2a2a2a2a 2a2a2a2a 2a2a2a2a 2a2a2a2a     ****************
 f805120:	000a2a2a 2020202a 20202020 20202020     **..*           
 f805130:	20202020 20202020 20202020 20202020                     
 f805140:	20202020 20202020 20202020 20202020                     
 f805150:	20202020 20202020 20202020 000a2a20                  *..
 f805160:	2020202a 20202020 20202020 75432020     *             Cu
 f805170:	6e657272 68632074 76207069 69737265     rrent chip versi
 f805180:	69206e6f 43412073 21217475 20202021     on is ACut!!!   
 f805190:	20202020 20202020 000a2a20 2020202a              *..*   
 f8051a0:	204b5452 204b4453 73656f64 746f6e20     RTK SDK does not
 f8051b0:	70757320 74726f70 72756320 746e6572      support current
 f8051c0:	72657620 6e6f6973 796e6120 726f6d20      version any mor
 f8051d0:	20202e65 000a2a20 2020202a 50202020     e.   *..*      P
 f8051e0:	7361656c 6f632065 6361746e 54522074     lease contact RT
 f8051f0:	4d50204b 726f6620 726f6d20 72702065     K PM for more pr
 f805200:	6375646f 6e692074 20216f66 20202020     oduct info!     
 f805210:	000a2a20 6f632f2e 6e6f706d 2f746e65      *.../component/
 f805220:	2f636f73 62656d61 6c706461 6c2f7375     soc/amebadplus/l
 f805230:	622f6269 6c746f6f 6564616f 6f622f72     ib/bootloader/bo
 f805240:	6f6c746f 72656461 346d6b5f 62696c5f     otloader_km4_lib
 f805250:	0000632e 2a2a2a2a 2a2a2a2a 2a2a2a2a     .c..************
 f805260:	2a2a2a2a 2a2a2a2a 2a2a2a2a 45202a2a     ************** E
 f805270:	524f5252 2a2a2a20 2a2a2a2a 2a2a2a2a     RROR ***********
 f805280:	2a2a2a2a 2a2a2a2a 2a2a2a2a 2a2a2a2a     ****************
 f805290:	000a2a2a 2020202a 20202020 20202020     **..*           
 f8052a0:	20202020 20202020 20202020 20202020                     
 f8052b0:	20202020 20202020 20202020 20202020                     
 f8052c0:	20202020 20202020 20202020 20202020                     
 f8052d0:	000a2a20 2020202a 20202020 72754320      *..*        Cur
 f8052e0:	746e6572 69686320 61682070 55442073     rent chip has DU
 f8052f0:	20594d4d 45534142 204e4920 2150544f     MMY BASE IN OTP!
 f805300:	20202121 20202020 20202020 20202020     !!              
 f805310:	000a2a20 2020202a 50202020 7361656c      *..*      Pleas
 f805320:	6f632065 6361746e 656b2074 5f796c6c     e contact kelly_
 f805330:	206e6170 20726f66 706c6568 20666920     pan for help if 
 f805340:	53206e69 4f485a55 20202055 20202020     in SUZHOU       
 f805350:	000a2a20 7220202a 72656665 206f7420      *..*  refer to 
 f805360:	70747468 2f2f3a73 6172696a 6165722e     https://jira.rea
 f805370:	6b65746c 6d6f632e 6f72622f 2f657377     ltek.com/browse/
 f805380:	4c575352 49444e41 352d544f 2e303638     RSWLANDIOT-5860.
 f805390:	000a2a20 2a2a2a2a 2a2a2a2a 2a2a2a2a      *..************
 f8053a0:	2a2a2a2a 2a2a2a2a 2a2a2a2a 2a2a2a2a     ****************
 f8053b0:	2a2a2a2a 2a2a2a2a 2a2a2a2a 2a2a2a2a     ****************
 f8053c0:	2a2a2a2a 2a2a2a2a 2a2a2a2a 2a2a2a2a     ****************
 f8053d0:	000a2a2a 2050544f 77207369 676e6f72     **..OTP is wrong
 f8053e0:	6874202c 63207369 20706968 73656f64     , this chip does
 f8053f0:	746f6e20 70757320 74726f70 70647220      not support rdp
 f805400:	0000000a                                ....

0f805404 <NsSau_config>:
 f805404:	40000000 4fffffff 00000000 ffffffff     ...@...O........
 f805414:	ffffffff 00000000                       ........

0f80541c <mpc1_config>:
 f80541c:	ffffffff ffffffff 00000000              ............

0f805428 <mpc2_config>:
 f805428:	ffffffff ffffffff 00000000              ............

0f805434 <sau_config>:
 f805434:	ffffffff ffffffff 00000000 00344d4b     ............KM4.
 f805444:	00304d4b 3d3d3d3d 3d3d3d3d 73253d3d     KM0.==========%s
 f805454:	61745320 44206b63 20706d75 3d3d3d3d      Stack Dump ====
 f805464:	3d3d3d3d 000a3d3d 72727543 20746e65     ======..Current 
 f805474:	63617453 6f50206b 65746e69 203d2072     Stack Pointer = 
 f805484:	202c7025 20646e61 706d7564 61747320     %p, and dump sta
 f805494:	64206b63 68747065 25203d20 000a0d64     ck depth = %d...
 f8054a4:	3830255b 00205d78 78383025 00000020     [%08x] .%08x ...
 f8054b4:	255b0a0d 5d783830 00000020 00003052     ..[%08x] ...R0..
 f8054c4:	00003452 3d3d3d3d 3d3d3d3d 43203d3d     R4..========== C
 f8054d4:	68736172 6d754420 3d3d2070 3d3d3d3d     rash Dump ======
 f8054e4:	3d3d3d3d 0000000a 3d3d3d3d 3d3d3d3d     ====....========
 f8054f4:	52203d3d 73696765 20726574 706d7544     == Register Dump
 f805504:	3d3d3d20 3d3d3d3d 0a3d3d3d 00000000      ==========.....
 f805514:	4c20205b 30205d52 38302578 000a786c     [  LR] 0x%08lx..
 f805524:	5020205b 30205d43 38302578 000a786c     [  PC] 0x%08lx..
 f805534:	5350785b 30205d52 38302578 000a786c     [xPSR] 0x%08lx..
 f805544:	4358455b 30205d52 38302578 000a786c     [EXCR] 0x%08lx..
 f805554:	7325205b 7830205d 6c383025 00000a78     [ %s] 0x%08lx...
 f805564:	00323152 3d3d3d3d 3d3d3d3d 45203d3d     R12.========== E
 f805574:	6f20646e 72432066 20687361 706d7544     nd of Crash Dump
 f805584:	3d3d3d20 3d3d3d3d 0a3d3d3d 00000000      ==========.....
 f805594:	0a0d0a0d 00000000 00003552 00003652     ........R5..R6..
 f8055a4:	00003752 00003852 00003952 00303152     R7..R8..R9..R10.
 f8055b4:	00313152 00003152 00003252 00003352     R11.R1..R2..R3..

0f8055c4 <register_string>:
 f8055c4:	00000000 0f8054c4 0f80559c 0f8055a0     .....T...U...U..
 f8055d4:	0f8055a4 0f8055a8 0f8055ac 0f8055b0     .U...U...U...U..
 f8055e4:	0f8055b4 0f8054c0 0f8055b8 0f8055bc     .U...T...U...U..
 f8055f4:	0f8055c0 0f805564 65637845 6f697470     .U..dU..Exceptio
 f805604:	6163206e 74686775 206e6f20 000a7025     n caught on %p..
 f805614:	64726148 75616620 6920746c 61632073     Hard fault is ca
 f805624:	64657375 20796220 6c696166 76206465     used by failed v
 f805634:	6f746365 65662072 00686374 52534648     ector fetch.HFSR
 f805644:	305b203a 38302578 205d786c 25203e2d     : [0x%08lx] -> %
 f805654:	00000a73 75636553 79746972 75616620     s...Security fau
 f805664:	6920746c 61632073 64657375 20796220     lt is caused by 
 f805674:	61766e49 2064696c 72746e65 6f702079     Invalid entry po
 f805684:	00746e69 52534653 305b203a 38302578     int.SFSR: [0x%08
 f805694:	205d786c 25203e2d 00000a73 75636553     lx] -> %s...Secu
 f8056a4:	79746972 75616620 6920746c 61632073     rity fault is ca
 f8056b4:	64657375 20796220 61766e49 2064696c     used by Invalid 
 f8056c4:	65746e69 74697267 69732079 74616e67     integrity signat
 f8056d4:	00657275 75636553 79746972 75616620     ure.Security fau
 f8056e4:	6920746c 61632073 64657375 20796220     lt is caused by 
 f8056f4:	61766e49 2064696c 65637865 6f697470     Invalid exceptio
 f805704:	6572206e 6e727574 00000000 75636553     n return....Secu
 f805714:	79746972 75616620 6920746c 61632073     rity fault is ca
 f805724:	64657375 20796220 72747441 74756269     used by Attribut
 f805734:	206e6f69 74696e75 6f697620 6974616c     ion unit violati
 f805744:	00006e6f 75636553 79746972 75616620     on..Security fau
 f805754:	6920746c 61632073 64657375 20796220     lt is caused by 
 f805764:	61766e49 2064696c 6e617274 69746973     Invalid transiti
 f805774:	00006e6f 75636553 79746972 75616620     on..Security fau
 f805784:	6920746c 61632073 64657375 20796220     lt is caused by 
 f805794:	797a614c 61747320 70206574 65736572     Lazy state prese
 f8057a4:	74617672 206e6f69 6f727265 00000072     rvation error...
 f8057b4:	75636553 79746972 75616620 6920746c     Security fault i
 f8057c4:	61632073 64657375 20796220 797a614c     s caused by Lazy
 f8057d4:	61747320 65206574 726f7272 00000000      state error....
 f8057e4:	20656854 75636573 79746972 75616620     The security fau
 f8057f4:	6f20746c 72756363 20646572 72646461     lt occurred addr
 f805804:	20737365 00007369 52414653 305b203a     ess is..SFAR: [0
 f805814:	38302578 205d786c 25203e2d 00000a73     x%08lx] -> %s...
 f805824:	6f6d654d 6d207972 67616e61 6e656d65     Memory managemen
 f805834:	61662074 20746c75 63207369 65737561     t fault is cause
 f805844:	79622064 736e6920 63757274 6e6f6974     d by instruction
 f805854:	63636120 20737365 6c6f6976 6f697461      access violatio
 f805864:	0000006e 53464d4d 5b203a52 30257830     n...MMFSR: [0x%0
 f805874:	5d786c38 203e2d20 000a7325 6f6d654d     8lx] -> %s..Memo
 f805884:	6d207972 67616e61 6e656d65 61662074     ry management fa
 f805894:	20746c75 63207369 65737561 79622064     ult is caused by
 f8058a4:	74616420 63612061 73736563 6f697620      data access vio
 f8058b4:	6974616c 00006e6f 6f6d654d 6d207972     lation..Memory m
 f8058c4:	67616e61 6e656d65 61662074 20746c75     anagement fault 
 f8058d4:	63207369 65737561 79622064 736e7520     is caused by uns
 f8058e4:	6b636174 20676e69 6f727265 00000072     tacking error...
 f8058f4:	6f6d654d 6d207972 67616e61 6e656d65     Memory managemen
 f805904:	61662074 20746c75 63207369 65737561     t fault is cause
 f805914:	79622064 61747320 6e696b63 72652067     d by stacking er
 f805924:	00726f72 6f6d654d 6d207972 67616e61     ror.Memory manag
 f805934:	6e656d65 61662074 20746c75 63207369     ement fault is c
 f805944:	65737561 79622064 6f6c6620 6e697461     aused by floatin
 f805954:	6f702d67 20746e69 797a616c 61747320     g-point lazy sta
 f805964:	70206574 65736572 74617672 006e6f69     te preservation.
 f805974:	20656854 6f6d656d 6d207972 67616e61     The memory manag
 f805984:	6e656d65 61662074 20746c75 7563636f     ement fault occu
 f805994:	64657272 64646120 73736572 00736920     rred address is.
 f8059a4:	52414d4d 305b203a 38302578 205d786c     MMAR: [0x%08lx] 
 f8059b4:	25203e2d 00000a73 20737542 6c756166     -> %s...Bus faul
 f8059c4:	73692074 75616320 20646573 69207962     t is caused by i
 f8059d4:	7274736e 69746375 61206e6f 73656363     nstruction acces
 f8059e4:	69762073 74616c6f 006e6f69 52534642     s violation.BFSR
 f8059f4:	305b203a 38302578 205d786c 25203e2d     : [0x%08lx] -> %
 f805a04:	00000a73 20737542 6c756166 73692074     s...Bus fault is
 f805a14:	75616320 20646573 70207962 69636572      caused by preci
 f805a24:	64206573 20617461 65636361 76207373     se data access v
 f805a34:	616c6f69 6e6f6974 00000000 20737542     iolation....Bus 
 f805a44:	6c756166 73692074 75616320 20646573     fault is caused 
 f805a54:	69207962 6572706d 65736963 74616420     by imprecise dat
 f805a64:	63612061 73736563 6f697620 6974616c     a access violati
 f805a74:	00006e6f 20737542 6c756166 73692074     on..Bus fault is
 f805a84:	75616320 20646573 75207962 6174736e      caused by unsta
 f805a94:	6e696b63 72652067 00726f72 20737542     cking error.Bus 
 f805aa4:	6c756166 73692074 75616320 20646573     fault is caused 
 f805ab4:	73207962 6b636174 20676e69 6f727265     by stacking erro
 f805ac4:	00000072 20737542 6c756166 73692074     r...Bus fault is
 f805ad4:	75616320 20646573 66207962 74616f6c      caused by float
 f805ae4:	2d676e69 6e696f70 616c2074 7320797a     ing-point lazy s
 f805af4:	65746174 65727020 76726573 6f697461     tate preservatio
 f805b04:	0000006e 20656854 20737562 6c756166     n...The bus faul
 f805b14:	636f2074 72727563 61206465 65726464     t occurred addre
 f805b24:	69207373 00000073 52414642 305b203a     ss is...BFAR: [0
 f805b34:	38302578 205d786c 25203e2d 00000a73     x%08lx] -> %s...
 f805b44:	67617355 61662065 20746c75 63207369     Usage fault is c
 f805b54:	65737561 79622064 74746120 74706d65     aused by attempt
 f805b64:	6f742073 65786520 65747563 206e6120     s to execute an 
 f805b74:	65646e75 656e6966 6e692064 75727473     undefined instru
 f805b84:	6f697463 0000006e 52534655 305b203a     ction...UFSR: [0
 f805b94:	38302578 205d786c 25203e2d 00000a73     x%08lx] -> %s...
 f805ba4:	67617355 61662065 20746c75 63207369     Usage fault is c
 f805bb4:	65737561 79622064 74746120 74706d65     aused by attempt
 f805bc4:	6f742073 69777320 20686374 61206f74     s to switch to a
 f805bd4:	6e69206e 696c6176 74732064 20657461     n invalid state 
 f805be4:	672e6528 41202c2e 00294d52 67617355     (e.g., ARM).Usag
 f805bf4:	61662065 20746c75 63207369 65737561     e fault is cause
 f805c04:	79622064 74746120 74706d65 6f742073     d by attempts to
 f805c14:	206f6420 65206e61 70656378 6e6f6974      do an exception
 f805c24:	74697720 20612068 20646162 756c6176      with a bad valu
 f805c34:	6e692065 65687420 43584520 5445525f     e in the EXC_RET
 f805c44:	204e5255 626d756e 00007265 67617355     URN number..Usag
 f805c54:	61662065 20746c75 63207369 65737561     e fault is cause
 f805c64:	79622064 74746120 74706d65 6f742073     d by attempts to
 f805c74:	65786520 65747563 63206120 6f72706f      execute a copro
 f805c84:	73736563 6920726f 7274736e 69746375     cessor instructi
 f805c94:	00006e6f 67617355 61662065 20746c75     on..Usage fault 
 f805ca4:	63207369 65737561 79622064 646e6920     is caused by ind
 f805cb4:	74616369 74207365 20746168 74732061     icates that a st
 f805cc4:	206b6361 7265766f 776f6c66 61682820     ack overflow (ha
 f805cd4:	61776472 63206572 6b636568 61682029     rdware check) ha
 f805ce4:	61742073 206e656b 63616c70 00000065     s taken place...
 f805cf4:	67617355 61662065 20746c75 63207369     Usage fault is c
 f805d04:	65737561 79622064 646e6920 74616369     aused by indicat
 f805d14:	74207365 20746168 75206e61 696c616e     es that an unali
 f805d24:	64656e67 63636120 20737365 6c756166     gned access faul
 f805d34:	61682074 61742073 206e656b 63616c70     t has taken plac
 f805d44:	00000065 67617355 61662065 20746c75     e...Usage fault 
 f805d54:	63207369 65737561 79622064 646e4920     is caused by Ind
 f805d64:	74616369 61207365 76696420 20656469     icates a divide 
 f805d74:	7a207962 206f7265 20736168 656b6174     by zero has take
 f805d84:	6c70206e 20656361 6e616328 20656220     n place (can be 
 f805d94:	20746573 796c6e6f 20666920 5f564944     set only if DIV_
 f805da4:	52545f30 73692050 74657320 00000029     0_TRP is set)...
 f805db4:	75626544 61662067 20746c75 63207369     Debug fault is c
 f805dc4:	65737561 79622064 6c616820 65722074     aused by halt re
 f805dd4:	73657571 20646574 4e206e69 00434956     quested in NVIC.
 f805de4:	52534644 305b203a 38302578 205d786c     DFSR: [0x%08lx] 
 f805df4:	25203e2d 00000a73 75626544 61662067     -> %s...Debug fa
 f805e04:	20746c75 63207369 65737561 79622064     ult is caused by
 f805e14:	504b4220 6e692054 75727473 6f697463      BKPT instructio
 f805e24:	7865206e 74756365 00006465 75626544     n executed..Debu
 f805e34:	61662067 20746c75 63207369 65737561     g fault is cause
 f805e44:	79622064 54574420 74616d20 6f206863     d by DWT match o
 f805e54:	72756363 00646572 75626544 61662067     ccurred.Debug fa
 f805e64:	20746c75 63207369 65737561 79622064     ult is caused by
 f805e74:	63655620 20726f74 63746566 636f2068      Vector fetch oc
 f805e84:	72727563 00006465 75626544 61662067     curred..Debug fa
 f805e94:	20746c75 63207369 65737561 79622064     ult is caused by
 f805ea4:	42444520 20515247 6e676973 61206c61      EDBGRQ signal a
 f805eb4:	72657373 00646574 00000000              sserted.....
