
/home/auto_jenkins_wlan5/IOT_ATS/repos/release-master/source/amebadplus_gcc_project/project_km4/asdk/image_mp/target_img2.axf:     file format elf32-littlearm


Disassembly of section .xip_image2.text:

0e000020 <ADC_StructInit>:
 e000020:	2203      	movs	r2, #3
 e000022:	2300      	movs	r3, #0
 e000024:	f44f 6c20 	mov.w	ip, #2560	; 0xa00
 e000028:	21ff      	movs	r1, #255	; 0xff
 e00002a:	8242      	strh	r2, [r0, #18]
 e00002c:	1c42      	adds	r2, r0, #1
 e00002e:	f8a0 c000 	strh.w	ip, [r0]
 e000032:	6183      	str	r3, [r0, #24]
 e000034:	7501      	strb	r1, [r0, #20]
 e000036:	f802 3f01 	strb.w	r3, [r2, #1]!
 e00003a:	3301      	adds	r3, #1
 e00003c:	2b0b      	cmp	r3, #11
 e00003e:	d1fa      	bne.n	e000036 <ADC_StructInit+0x16>
 e000040:	4770      	bx	lr
 e000042:	bf00      	nop

0e000044 <ADC_Init>:
 e000044:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
 e000048:	2307      	movs	r3, #7
 e00004a:	b083      	sub	sp, #12
 e00004c:	4f7a      	ldr	r7, [pc, #488]	; (e000238 <ADC_Init+0x1f4>)
 e00004e:	4604      	mov	r4, r0
 e000050:	4a7a      	ldr	r2, [pc, #488]	; (e00023c <ADC_Init+0x1f8>)
 e000052:	f88d 3007 	strb.w	r3, [sp, #7]
 e000056:	e847 f300 	tt	r3, r7
 e00005a:	f413 0f80 	tst.w	r3, #4194304	; 0x400000
 e00005e:	7803      	ldrb	r3, [r0, #0]
 e000060:	4e77      	ldr	r6, [pc, #476]	; (e000240 <ADC_Init+0x1fc>)
 e000062:	bf18      	it	ne
 e000064:	4616      	movne	r6, r2
 e000066:	2b03      	cmp	r3, #3
 e000068:	f200 80b6 	bhi.w	e0001d8 <ADC_Init+0x194>
 e00006c:	7ca3      	ldrb	r3, [r4, #18]
 e00006e:	2b06      	cmp	r3, #6
 e000070:	f200 80ba 	bhi.w	e0001e8 <ADC_Init+0x1a4>
 e000074:	7863      	ldrb	r3, [r4, #1]
 e000076:	2b0f      	cmp	r3, #15
 e000078:	f200 80be 	bhi.w	e0001f8 <ADC_Init+0x1b4>
 e00007c:	7823      	ldrb	r3, [r4, #0]
 e00007e:	2b01      	cmp	r3, #1
 e000080:	d017      	beq.n	e0000b2 <ADC_Init+0x6e>
 e000082:	f104 0801 	add.w	r8, r4, #1
 e000086:	f8df 91c4 	ldr.w	r9, [pc, #452]	; e00024c <ADC_Init+0x208>
 e00008a:	e004      	b.n	e000096 <ADC_Init+0x52>
 e00008c:	7863      	ldrb	r3, [r4, #1]
 e00008e:	1b2d      	subs	r5, r5, r4
 e000090:	3301      	adds	r3, #1
 e000092:	42ab      	cmp	r3, r5
 e000094:	dd0d      	ble.n	e0000b2 <ADC_Init+0x6e>
 e000096:	4645      	mov	r5, r8
 e000098:	f818 3f01 	ldrb.w	r3, [r8, #1]!
 e00009c:	2b0f      	cmp	r3, #15
 e00009e:	d1f5      	bne.n	e00008c <ADC_Init+0x48>
 e0000a0:	2149      	movs	r1, #73	; 0x49
 e0000a2:	4648      	mov	r0, r9
 e0000a4:	f007 f964 	bl	e007370 <io_assert_failed>
 e0000a8:	7863      	ldrb	r3, [r4, #1]
 e0000aa:	1b2d      	subs	r5, r5, r4
 e0000ac:	3301      	adds	r3, #1
 e0000ae:	42ab      	cmp	r3, r5
 e0000b0:	dcf1      	bgt.n	e000096 <ADC_Init+0x52>
 e0000b2:	e847 f300 	tt	r3, r7
 e0000b6:	4a62      	ldr	r2, [pc, #392]	; (e000240 <ADC_Init+0x1fc>)
 e0000b8:	f413 0f80 	tst.w	r3, #4194304	; 0x400000
 e0000bc:	495f      	ldr	r1, [pc, #380]	; (e00023c <ADC_Init+0x1f8>)
 e0000be:	f04f 0501 	mov.w	r5, #1
 e0000c2:	bf0c      	ite	eq
 e0000c4:	4610      	moveq	r0, r2
 e0000c6:	4608      	movne	r0, r1
 e0000c8:	6803      	ldr	r3, [r0, #0]
 e0000ca:	f423 7300 	bic.w	r3, r3, #512	; 0x200
 e0000ce:	6003      	str	r3, [r0, #0]
 e0000d0:	e847 f300 	tt	r3, r7
 e0000d4:	f413 0f80 	tst.w	r3, #4194304	; 0x400000
 e0000d8:	bf18      	it	ne
 e0000da:	460a      	movne	r2, r1
 e0000dc:	f8c2 50a4 	str.w	r5, [r2, #164]	; 0xa4
 e0000e0:	f8d2 30a0 	ldr.w	r3, [r2, #160]	; 0xa0
 e0000e4:	2b00      	cmp	r3, #0
 e0000e6:	d1fb      	bne.n	e0000e0 <ADC_Init+0x9c>
 e0000e8:	4d56      	ldr	r5, [pc, #344]	; (e000244 <ADC_Init+0x200>)
 e0000ea:	f8c2 30a4 	str.w	r3, [r2, #164]	; 0xa4
 e0000ee:	782b      	ldrb	r3, [r5, #0]
 e0000f0:	2b00      	cmp	r3, #0
 e0000f2:	f000 8086 	beq.w	e000202 <ADC_Init+0x1be>
 e0000f6:	2200      	movs	r2, #0
 e0000f8:	4b51      	ldr	r3, [pc, #324]	; (e000240 <ADC_Init+0x1fc>)
 e0000fa:	4d53      	ldr	r5, [pc, #332]	; (e000248 <ADC_Init+0x204>)
 e0000fc:	65b2      	str	r2, [r6, #88]	; 0x58
 e0000fe:	e847 f100 	tt	r1, r7
 e000102:	4a4e      	ldr	r2, [pc, #312]	; (e00023c <ADC_Init+0x1f8>)
 e000104:	f411 0f80 	tst.w	r1, #4194304	; 0x400000
 e000108:	f04f 0101 	mov.w	r1, #1
 e00010c:	bf0c      	ite	eq
 e00010e:	4618      	moveq	r0, r3
 e000110:	4610      	movne	r0, r2
 e000112:	6605      	str	r5, [r0, #96]	; 0x60
 e000114:	e847 f000 	tt	r0, r7
 e000118:	f410 0f80 	tst.w	r0, #4194304	; 0x400000
 e00011c:	bf18      	it	ne
 e00011e:	4613      	movne	r3, r2
 e000120:	f8c3 10a4 	str.w	r1, [r3, #164]	; 0xa4
 e000124:	f8d3 10a0 	ldr.w	r1, [r3, #160]	; 0xa0
 e000128:	2900      	cmp	r1, #0
 e00012a:	d1fb      	bne.n	e000124 <ADC_Init+0xe0>
 e00012c:	f8c3 10a4 	str.w	r1, [r3, #164]	; 0xa4
 e000130:	1ca5      	adds	r5, r4, #2
 e000132:	7ca3      	ldrb	r3, [r4, #18]
 e000134:	f003 0307 	and.w	r3, r3, #7
 e000138:	f8c6 30a8 	str.w	r3, [r6, #168]	; 0xa8
 e00013c:	7823      	ldrb	r3, [r4, #0]
 e00013e:	7862      	ldrb	r2, [r4, #1]
 e000140:	005b      	lsls	r3, r3, #1
 e000142:	6830      	ldr	r0, [r6, #0]
 e000144:	0112      	lsls	r2, r2, #4
 e000146:	f003 030e 	and.w	r3, r3, #14
 e00014a:	f020 00fe 	bic.w	r0, r0, #254	; 0xfe
 e00014e:	b2d2      	uxtb	r2, r2
 e000150:	4313      	orrs	r3, r2
 e000152:	4303      	orrs	r3, r0
 e000154:	4608      	mov	r0, r1
 e000156:	6033      	str	r3, [r6, #0]
 e000158:	69a3      	ldr	r3, [r4, #24]
 e00015a:	6073      	str	r3, [r6, #4]
 e00015c:	460b      	mov	r3, r1
 e00015e:	f894 c001 	ldrb.w	ip, [r4, #1]
 e000162:	f10c 0c01 	add.w	ip, ip, #1
 e000166:	f1bc 0f08 	cmp.w	ip, #8
 e00016a:	bfa8      	it	ge
 e00016c:	f04f 0c08 	movge.w	ip, #8
 e000170:	ea4f 0c8c 	mov.w	ip, ip, lsl #2
 e000174:	f815 2b01 	ldrb.w	r2, [r5], #1
 e000178:	409a      	lsls	r2, r3
 e00017a:	3304      	adds	r3, #4
 e00017c:	459c      	cmp	ip, r3
 e00017e:	ea40 0002 	orr.w	r0, r0, r2
 e000182:	d1f7      	bne.n	e000174 <ADC_Init+0x130>
 e000184:	63f0      	str	r0, [r6, #60]	; 0x3c
 e000186:	7863      	ldrb	r3, [r4, #1]
 e000188:	1c5a      	adds	r2, r3, #1
 e00018a:	2a08      	cmp	r2, #8
 e00018c:	dd0d      	ble.n	e0001aa <ADC_Init+0x166>
 e00018e:	1ce5      	adds	r5, r4, #3
 e000190:	f104 000a 	add.w	r0, r4, #10
 e000194:	2200      	movs	r2, #0
 e000196:	441d      	add	r5, r3
 e000198:	f810 3b01 	ldrb.w	r3, [r0], #1
 e00019c:	4093      	lsls	r3, r2
 e00019e:	4285      	cmp	r5, r0
 e0001a0:	f102 0204 	add.w	r2, r2, #4
 e0001a4:	ea41 0103 	orr.w	r1, r1, r3
 e0001a8:	d1f6      	bne.n	e000198 <ADC_Init+0x154>
 e0001aa:	6431      	str	r1, [r6, #64]	; 0x40
 e0001ac:	7d23      	ldrb	r3, [r4, #20]
 e0001ae:	2b0a      	cmp	r3, #10
 e0001b0:	d80d      	bhi.n	e0001ce <ADC_Init+0x18a>
 e0001b2:	e847 f700 	tt	r7, r7
 e0001b6:	4a21      	ldr	r2, [pc, #132]	; (e00023c <ADC_Init+0x1f8>)
 e0001b8:	f417 0f80 	tst.w	r7, #4194304	; 0x400000
 e0001bc:	4b20      	ldr	r3, [pc, #128]	; (e000240 <ADC_Init+0x1fc>)
 e0001be:	bf18      	it	ne
 e0001c0:	4613      	movne	r3, r2
 e0001c2:	6d9a      	ldr	r2, [r3, #88]	; 0x58
 e0001c4:	f042 0204 	orr.w	r2, r2, #4
 e0001c8:	659a      	str	r2, [r3, #88]	; 0x58
 e0001ca:	7d23      	ldrb	r3, [r4, #20]
 e0001cc:	6673      	str	r3, [r6, #100]	; 0x64
 e0001ce:	7ce3      	ldrb	r3, [r4, #19]
 e0001d0:	66b3      	str	r3, [r6, #104]	; 0x68
 e0001d2:	b003      	add	sp, #12
 e0001d4:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 e0001d8:	2142      	movs	r1, #66	; 0x42
 e0001da:	481c      	ldr	r0, [pc, #112]	; (e00024c <ADC_Init+0x208>)
 e0001dc:	f007 f8c8 	bl	e007370 <io_assert_failed>
 e0001e0:	7ca3      	ldrb	r3, [r4, #18]
 e0001e2:	2b06      	cmp	r3, #6
 e0001e4:	f67f af46 	bls.w	e000074 <ADC_Init+0x30>
 e0001e8:	2143      	movs	r1, #67	; 0x43
 e0001ea:	4818      	ldr	r0, [pc, #96]	; (e00024c <ADC_Init+0x208>)
 e0001ec:	f007 f8c0 	bl	e007370 <io_assert_failed>
 e0001f0:	7863      	ldrb	r3, [r4, #1]
 e0001f2:	2b0f      	cmp	r3, #15
 e0001f4:	f67f af42 	bls.w	e00007c <ADC_Init+0x38>
 e0001f8:	2144      	movs	r1, #68	; 0x44
 e0001fa:	4814      	ldr	r0, [pc, #80]	; (e00024c <ADC_Init+0x208>)
 e0001fc:	f007 f8b8 	bl	e007370 <io_assert_failed>
 e000200:	e73c      	b.n	e00007c <ADC_Init+0x38>
 e000202:	4b13      	ldr	r3, [pc, #76]	; (e000250 <ADC_Init+0x20c>)
 e000204:	f10d 0107 	add.w	r1, sp, #7
 e000208:	f240 70eb 	movw	r0, #2027	; 0x7eb
 e00020c:	4798      	blx	r3
 e00020e:	f89d 2007 	ldrb.w	r2, [sp, #7]
 e000212:	4b10      	ldr	r3, [pc, #64]	; (e000254 <ADC_Init+0x210>)
 e000214:	f002 0207 	and.w	r2, r2, #7
 e000218:	f88d 2007 	strb.w	r2, [sp, #7]
 e00021c:	2a03      	cmp	r2, #3
 e00021e:	6e5b      	ldr	r3, [r3, #100]	; 0x64
 e000220:	4a0c      	ldr	r2, [pc, #48]	; (e000254 <ADC_Init+0x210>)
 e000222:	f423 63e0 	bic.w	r3, r3, #1792	; 0x700
 e000226:	bf0c      	ite	eq
 e000228:	f443 7340 	orreq.w	r3, r3, #768	; 0x300
 e00022c:	f443 7300 	orrne.w	r3, r3, #512	; 0x200
 e000230:	6653      	str	r3, [r2, #100]	; 0x64
 e000232:	2301      	movs	r3, #1
 e000234:	702b      	strb	r3, [r5, #0]
 e000236:	e75e      	b.n	e0000f6 <ADC_Init+0xb2>
 e000238:	0e007009 	.word	0x0e007009
 e00023c:	51011000 	.word	0x51011000
 e000240:	41011000 	.word	0x41011000
 e000244:	2000b180 	.word	0x2000b180
 e000248:	0007ffff 	.word	0x0007ffff
 e00024c:	0e007a20 	.word	0x0e007a20
 e000250:	0e006a29 	.word	0x0e006a29
 e000254:	41008400 	.word	0x41008400

0e000258 <ADC_Cmd>:
 e000258:	b410      	push	{r4}
 e00025a:	4914      	ldr	r1, [pc, #80]	; (e0002ac <ADC_Cmd+0x54>)
 e00025c:	4a14      	ldr	r2, [pc, #80]	; (e0002b0 <ADC_Cmd+0x58>)
 e00025e:	e841 f300 	tt	r3, r1
 e000262:	4c14      	ldr	r4, [pc, #80]	; (e0002b4 <ADC_Cmd+0x5c>)
 e000264:	f413 0f80 	tst.w	r3, #4194304	; 0x400000
 e000268:	bf0c      	ite	eq
 e00026a:	4613      	moveq	r3, r2
 e00026c:	4623      	movne	r3, r4
 e00026e:	b130      	cbz	r0, e00027e <ADC_Cmd+0x26>
 e000270:	681a      	ldr	r2, [r3, #0]
 e000272:	f442 7200 	orr.w	r2, r2, #512	; 0x200
 e000276:	601a      	str	r2, [r3, #0]
 e000278:	f85d 4b04 	ldr.w	r4, [sp], #4
 e00027c:	4770      	bx	lr
 e00027e:	6818      	ldr	r0, [r3, #0]
 e000280:	f420 7000 	bic.w	r0, r0, #512	; 0x200
 e000284:	6018      	str	r0, [r3, #0]
 e000286:	2301      	movs	r3, #1
 e000288:	e841 f100 	tt	r1, r1
 e00028c:	f411 0f80 	tst.w	r1, #4194304	; 0x400000
 e000290:	bf18      	it	ne
 e000292:	4622      	movne	r2, r4
 e000294:	f8c2 30a4 	str.w	r3, [r2, #164]	; 0xa4
 e000298:	f8d2 30a0 	ldr.w	r3, [r2, #160]	; 0xa0
 e00029c:	2b00      	cmp	r3, #0
 e00029e:	d1fb      	bne.n	e000298 <ADC_Cmd+0x40>
 e0002a0:	f8c2 30a4 	str.w	r3, [r2, #164]	; 0xa4
 e0002a4:	f85d 4b04 	ldr.w	r4, [sp], #4
 e0002a8:	4770      	bx	lr
 e0002aa:	bf00      	nop
 e0002ac:	0e007009 	.word	0x0e007009
 e0002b0:	41011000 	.word	0x41011000
 e0002b4:	51011000 	.word	0x51011000

0e0002b8 <ADC_ReceiveBuf>:
 e0002b8:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e0002bc:	4c35      	ldr	r4, [pc, #212]	; (e000394 <ADC_ReceiveBuf+0xdc>)
 e0002be:	460f      	mov	r7, r1
 e0002c0:	f8df 80d8 	ldr.w	r8, [pc, #216]	; e00039c <ADC_ReceiveBuf+0xe4>
 e0002c4:	2201      	movs	r2, #1
 e0002c6:	e848 f100 	tt	r1, r8
 e0002ca:	f411 0f80 	tst.w	r1, #4194304	; 0x400000
 e0002ce:	4b32      	ldr	r3, [pc, #200]	; (e000398 <ADC_ReceiveBuf+0xe0>)
 e0002d0:	b083      	sub	sp, #12
 e0002d2:	4683      	mov	fp, r0
 e0002d4:	bf18      	it	ne
 e0002d6:	4623      	movne	r3, r4
 e0002d8:	f8c3 20a4 	str.w	r2, [r3, #164]	; 0xa4
 e0002dc:	f8d3 60a0 	ldr.w	r6, [r3, #160]	; 0xa0
 e0002e0:	2e00      	cmp	r6, #0
 e0002e2:	d1fb      	bne.n	e0002dc <ADC_ReceiveBuf+0x24>
 e0002e4:	f8c3 60a4 	str.w	r6, [r3, #164]	; 0xa4
 e0002e8:	2101      	movs	r1, #1
 e0002ea:	e848 f300 	tt	r3, r8
 e0002ee:	4c2a      	ldr	r4, [pc, #168]	; (e000398 <ADC_ReceiveBuf+0xe0>)
 e0002f0:	f413 0f80 	tst.w	r3, #4194304	; 0x400000
 e0002f4:	4d27      	ldr	r5, [pc, #156]	; (e000394 <ADC_ReceiveBuf+0xdc>)
 e0002f6:	bf08      	it	eq
 e0002f8:	4622      	moveq	r2, r4
 e0002fa:	f8df 90a4 	ldr.w	r9, [pc, #164]	; e0003a0 <ADC_ReceiveBuf+0xe8>
 e0002fe:	bf18      	it	ne
 e000300:	462a      	movne	r2, r5
 e000302:	f8df a0a0 	ldr.w	sl, [pc, #160]	; e0003a4 <ADC_ReceiveBuf+0xec>
 e000306:	f8d2 30a8 	ldr.w	r3, [r2, #168]	; 0xa8
 e00030a:	6491      	str	r1, [r2, #72]	; 0x48
 e00030c:	aa02      	add	r2, sp, #8
 e00030e:	e899 0003 	ldmia.w	r9, {r0, r1}
 e000312:	fa52 f383 	uxtab	r3, r2, r3
 e000316:	f8ad 1004 	strh.w	r1, [sp, #4]
 e00031a:	0c09      	lsrs	r1, r1, #16
 e00031c:	9000      	str	r0, [sp, #0]
 e00031e:	f88d 1006 	strb.w	r1, [sp, #6]
 e000322:	f813 0c08 	ldrb.w	r0, [r3, #-8]
 e000326:	47d0      	blx	sl
 e000328:	b1af      	cbz	r7, e000356 <ADC_ReceiveBuf+0x9e>
 e00032a:	e848 f200 	tt	r2, r8
 e00032e:	f1ab 0104 	sub.w	r1, fp, #4
 e000332:	f3c2 5280 	ubfx	r2, r2, #22, #1
 e000336:	2a00      	cmp	r2, #0
 e000338:	bf0c      	ite	eq
 e00033a:	4623      	moveq	r3, r4
 e00033c:	462b      	movne	r3, r5
 e00033e:	6d5b      	ldr	r3, [r3, #84]	; 0x54
 e000340:	075b      	lsls	r3, r3, #29
 e000342:	d4f8      	bmi.n	e000336 <ADC_ReceiveBuf+0x7e>
 e000344:	f8d4 309c 	ldr.w	r3, [r4, #156]	; 0x9c
 e000348:	3601      	adds	r6, #1
 e00034a:	f3c3 0313 	ubfx	r3, r3, #0, #20
 e00034e:	42b7      	cmp	r7, r6
 e000350:	f841 3f04 	str.w	r3, [r1, #4]!
 e000354:	d1ef      	bne.n	e000336 <ADC_ReceiveBuf+0x7e>
 e000356:	e848 f800 	tt	r8, r8
 e00035a:	4a0e      	ldr	r2, [pc, #56]	; (e000394 <ADC_ReceiveBuf+0xdc>)
 e00035c:	f418 0f80 	tst.w	r8, #4194304	; 0x400000
 e000360:	4b0d      	ldr	r3, [pc, #52]	; (e000398 <ADC_ReceiveBuf+0xe0>)
 e000362:	f04f 0400 	mov.w	r4, #0
 e000366:	bf18      	it	ne
 e000368:	4613      	movne	r3, r2
 e00036a:	e899 0003 	ldmia.w	r9, {r0, r1}
 e00036e:	f8d3 20a8 	ldr.w	r2, [r3, #168]	; 0xa8
 e000372:	649c      	str	r4, [r3, #72]	; 0x48
 e000374:	ab02      	add	r3, sp, #8
 e000376:	9000      	str	r0, [sp, #0]
 e000378:	0c08      	lsrs	r0, r1, #16
 e00037a:	fa53 f282 	uxtab	r2, r3, r2
 e00037e:	f8ad 1004 	strh.w	r1, [sp, #4]
 e000382:	f88d 0006 	strb.w	r0, [sp, #6]
 e000386:	4653      	mov	r3, sl
 e000388:	f812 0c08 	ldrb.w	r0, [r2, #-8]
 e00038c:	b003      	add	sp, #12
 e00038e:	e8bd 4ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e000392:	4718      	bx	r3
 e000394:	51011000 	.word	0x51011000
 e000398:	41011000 	.word	0x41011000
 e00039c:	0e007009 	.word	0x0e007009
 e0003a0:	0e007a58 	.word	0x0e007a58
 e0003a4:	000094a5 	.word	0x000094a5

0e0003a8 <app_mpu_nocache_init>:
 e0003a8:	b510      	push	{r4, lr}
 e0003aa:	b086      	sub	sp, #24
 e0003ac:	f001 fc98 	bl	e001ce0 <mpu_entry_alloc>
 e0003b0:	4a10      	ldr	r2, [pc, #64]	; (e0003f4 <app_mpu_nocache_init+0x4c>)
 e0003b2:	4b11      	ldr	r3, [pc, #68]	; (e0003f8 <app_mpu_nocache_init+0x50>)
 e0003b4:	f44f 7100 	mov.w	r1, #512	; 0x200
 e0003b8:	9203      	str	r2, [sp, #12]
 e0003ba:	1a9b      	subs	r3, r3, r2
 e0003bc:	2b1f      	cmp	r3, #31
 e0003be:	e9cd 3104 	strd	r3, r1, [sp, #16]
 e0003c2:	a903      	add	r1, sp, #12
 e0003c4:	d810      	bhi.n	e0003e8 <app_mpu_nocache_init+0x40>
 e0003c6:	2400      	movs	r4, #0
 e0003c8:	9101      	str	r1, [sp, #4]
 e0003ca:	f001 fc89 	bl	e001ce0 <mpu_entry_alloc>
 e0003ce:	f44f 2200 	mov.w	r2, #524288	; 0x80000
 e0003d2:	f44f 7300 	mov.w	r3, #512	; 0x200
 e0003d6:	9901      	ldr	r1, [sp, #4]
 e0003d8:	e9cd 4203 	strd	r4, r2, [sp, #12]
 e0003dc:	9305      	str	r3, [sp, #20]
 e0003de:	f001 fc0f 	bl	e001c00 <mpu_region_cfg>
 e0003e2:	4620      	mov	r0, r4
 e0003e4:	b006      	add	sp, #24
 e0003e6:	bd10      	pop	{r4, pc}
 e0003e8:	9101      	str	r1, [sp, #4]
 e0003ea:	f001 fc09 	bl	e001c00 <mpu_region_cfg>
 e0003ee:	9901      	ldr	r1, [sp, #4]
 e0003f0:	e7e9      	b.n	e0003c6 <app_mpu_nocache_init+0x1e>
 e0003f2:	bf00      	nop
 e0003f4:	2000c100 	.word	0x2000c100
 e0003f8:	2000c100 	.word	0x2000c100

0e0003fc <_init>:
 e0003fc:	4770      	bx	lr
 e0003fe:	bf00      	nop

0e000400 <app_testmode_status>:
 e000400:	b510      	push	{r4, lr}
 e000402:	4b0f      	ldr	r3, [pc, #60]	; (e000440 <app_testmode_status+0x40>)
 e000404:	b082      	sub	sp, #8
 e000406:	4798      	blx	r3
 e000408:	b908      	cbnz	r0, e00040e <app_testmode_status+0xe>
 e00040a:	b002      	add	sp, #8
 e00040c:	bd10      	pop	{r4, pc}
 e00040e:	4b0d      	ldr	r3, [pc, #52]	; (e000444 <app_testmode_status+0x44>)
 e000410:	4798      	blx	r3
 e000412:	b140      	cbz	r0, e000426 <app_testmode_status+0x26>
 e000414:	4b0c      	ldr	r3, [pc, #48]	; (e000448 <app_testmode_status+0x48>)
 e000416:	2249      	movs	r2, #73	; 0x49
 e000418:	490c      	ldr	r1, [pc, #48]	; (e00044c <app_testmode_status+0x4c>)
 e00041a:	2004      	movs	r0, #4
 e00041c:	b002      	add	sp, #8
 e00041e:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
 e000422:	f002 b80b 	b.w	e00243c <rtk_log_write>
 e000426:	4b0a      	ldr	r3, [pc, #40]	; (e000450 <app_testmode_status+0x50>)
 e000428:	4798      	blx	r3
 e00042a:	4604      	mov	r4, r0
 e00042c:	4b09      	ldr	r3, [pc, #36]	; (e000454 <app_testmode_status+0x54>)
 e00042e:	2249      	movs	r2, #73	; 0x49
 e000430:	4906      	ldr	r1, [pc, #24]	; (e00044c <app_testmode_status+0x4c>)
 e000432:	2004      	movs	r0, #4
 e000434:	9400      	str	r4, [sp, #0]
 e000436:	f002 f801 	bl	e00243c <rtk_log_write>
 e00043a:	b002      	add	sp, #8
 e00043c:	bd10      	pop	{r4, pc}
 e00043e:	bf00      	nop
 e000440:	0000c1cd 	.word	0x0000c1cd
 e000444:	0000c1dd 	.word	0x0000c1dd
 e000448:	0e007a60 	.word	0x0e007a60
 e00044c:	0e007a78 	.word	0x0e007a78
 e000450:	0000c1b5 	.word	0x0000c1b5
 e000454:	0e007a7c 	.word	0x0e007a7c

0e000458 <app_vdd1833_detect>:
 e000458:	f04f 4181 	mov.w	r1, #1082130432	; 0x40800000
 e00045c:	2201      	movs	r2, #1
 e00045e:	4b1e      	ldr	r3, [pc, #120]	; (e0004d8 <app_vdd1833_detect+0x80>)
 e000460:	4608      	mov	r0, r1
 e000462:	b530      	push	{r4, r5, lr}
 e000464:	b099      	sub	sp, #100	; 0x64
 e000466:	2501      	movs	r5, #1
 e000468:	4798      	blx	r3
 e00046a:	4b1c      	ldr	r3, [pc, #112]	; (e0004dc <app_vdd1833_detect+0x84>)
 e00046c:	a801      	add	r0, sp, #4
 e00046e:	4c1c      	ldr	r4, [pc, #112]	; (e0004e0 <app_vdd1833_detect+0x88>)
 e000470:	4798      	blx	r3
 e000472:	2209      	movs	r2, #9
 e000474:	4b1b      	ldr	r3, [pc, #108]	; (e0004e4 <app_vdd1833_detect+0x8c>)
 e000476:	a801      	add	r0, sp, #4
 e000478:	f88d 2006 	strb.w	r2, [sp, #6]
 e00047c:	f8ad 5004 	strh.w	r5, [sp, #4]
 e000480:	4798      	blx	r3
 e000482:	4628      	mov	r0, r5
 e000484:	47a0      	blx	r4
 e000486:	4b18      	ldr	r3, [pc, #96]	; (e0004e8 <app_vdd1833_detect+0x90>)
 e000488:	2110      	movs	r1, #16
 e00048a:	a808      	add	r0, sp, #32
 e00048c:	4798      	blx	r3
 e00048e:	2000      	movs	r0, #0
 e000490:	47a0      	blx	r4
 e000492:	ab07      	add	r3, sp, #28
 e000494:	a817      	add	r0, sp, #92	; 0x5c
 e000496:	2200      	movs	r2, #0
 e000498:	f853 1f04 	ldr.w	r1, [r3, #4]!
 e00049c:	4283      	cmp	r3, r0
 e00049e:	fa12 f281 	uxtah	r2, r2, r1
 e0004a2:	d1f9      	bne.n	e000498 <app_vdd1833_detect+0x40>
 e0004a4:	f640 33b8 	movw	r3, #3000	; 0xbb8
 e0004a8:	4910      	ldr	r1, [pc, #64]	; (e0004ec <app_vdd1833_detect+0x94>)
 e0004aa:	ebb3 1f12 	cmp.w	r3, r2, lsr #4
 e0004ae:	6f4c      	ldr	r4, [r1, #116]	; 0x74
 e0004b0:	d205      	bcs.n	e0004be <app_vdd1833_detect+0x66>
 e0004b2:	f044 4480 	orr.w	r4, r4, #1073741824	; 0x40000000
 e0004b6:	4b0d      	ldr	r3, [pc, #52]	; (e0004ec <app_vdd1833_detect+0x94>)
 e0004b8:	675c      	str	r4, [r3, #116]	; 0x74
 e0004ba:	b019      	add	sp, #100	; 0x64
 e0004bc:	bd30      	pop	{r4, r5, pc}
 e0004be:	4b0c      	ldr	r3, [pc, #48]	; (e0004f0 <app_vdd1833_detect+0x98>)
 e0004c0:	2249      	movs	r2, #73	; 0x49
 e0004c2:	490c      	ldr	r1, [pc, #48]	; (e0004f4 <app_vdd1833_detect+0x9c>)
 e0004c4:	2004      	movs	r0, #4
 e0004c6:	f001 ffb9 	bl	e00243c <rtk_log_write>
 e0004ca:	f024 4480 	bic.w	r4, r4, #1073741824	; 0x40000000
 e0004ce:	4b07      	ldr	r3, [pc, #28]	; (e0004ec <app_vdd1833_detect+0x94>)
 e0004d0:	675c      	str	r4, [r3, #116]	; 0x74
 e0004d2:	b019      	add	sp, #100	; 0x64
 e0004d4:	bd30      	pop	{r4, r5, pc}
 e0004d6:	bf00      	nop
 e0004d8:	0000bebd 	.word	0x0000bebd
 e0004dc:	0e000021 	.word	0x0e000021
 e0004e0:	0e000259 	.word	0x0e000259
 e0004e4:	0e000045 	.word	0x0e000045
 e0004e8:	0e0002b9 	.word	0x0e0002b9
 e0004ec:	41008000 	.word	0x41008000
 e0004f0:	0e007a94 	.word	0x0e007a94
 e0004f4:	0e007a78 	.word	0x0e007a78

0e0004f8 <app_start>:
 e0004f8:	b530      	push	{r4, r5, lr}
 e0004fa:	4b67      	ldr	r3, [pc, #412]	; (e000698 <app_start+0x1a0>)
 e0004fc:	b083      	sub	sp, #12
 e0004fe:	2249      	movs	r2, #73	; 0x49
 e000500:	4966      	ldr	r1, [pc, #408]	; (e00069c <app_start+0x1a4>)
 e000502:	2004      	movs	r0, #4
 e000504:	f001 ff9a 	bl	e00243c <rtk_log_write>
 e000508:	f3bf 8f4f 	dsb	sy
 e00050c:	f3bf 8f6f 	isb	sy
 e000510:	4963      	ldr	r1, [pc, #396]	; (e0006a0 <app_start+0x1a8>)
 e000512:	2300      	movs	r3, #0
 e000514:	f8c1 3250 	str.w	r3, [r1, #592]	; 0x250
 e000518:	f3bf 8f4f 	dsb	sy
 e00051c:	f3bf 8f6f 	isb	sy
 e000520:	694b      	ldr	r3, [r1, #20]
 e000522:	f443 3300 	orr.w	r3, r3, #131072	; 0x20000
 e000526:	614b      	str	r3, [r1, #20]
 e000528:	f3bf 8f4f 	dsb	sy
 e00052c:	f3bf 8f6f 	isb	sy
 e000530:	694b      	ldr	r3, [r1, #20]
 e000532:	f413 3380 	ands.w	r3, r3, #65536	; 0x10000
 e000536:	d126      	bne.n	e000586 <app_start+0x8e>
 e000538:	f8c1 3084 	str.w	r3, [r1, #132]	; 0x84
 e00053c:	f3bf 8f4f 	dsb	sy
 e000540:	f8d1 3080 	ldr.w	r3, [r1, #128]	; 0x80
 e000544:	f643 74e0 	movw	r4, #16352	; 0x3fe0
 e000548:	f3c3 3c4e 	ubfx	ip, r3, #13, #15
 e00054c:	f3c3 0ec9 	ubfx	lr, r3, #3, #10
 e000550:	ea4f 1c4c 	mov.w	ip, ip, lsl #5
 e000554:	ea0c 0004 	and.w	r0, ip, r4
 e000558:	4673      	mov	r3, lr
 e00055a:	ea40 7283 	orr.w	r2, r0, r3, lsl #30
 e00055e:	3b01      	subs	r3, #1
 e000560:	f8c1 2260 	str.w	r2, [r1, #608]	; 0x260
 e000564:	1c5a      	adds	r2, r3, #1
 e000566:	d1f8      	bne.n	e00055a <app_start+0x62>
 e000568:	f1ac 0c20 	sub.w	ip, ip, #32
 e00056c:	f11c 0f20 	cmn.w	ip, #32
 e000570:	d1f0      	bne.n	e000554 <app_start+0x5c>
 e000572:	f3bf 8f4f 	dsb	sy
 e000576:	694b      	ldr	r3, [r1, #20]
 e000578:	f443 3380 	orr.w	r3, r3, #65536	; 0x10000
 e00057c:	614b      	str	r3, [r1, #20]
 e00057e:	f3bf 8f4f 	dsb	sy
 e000582:	f3bf 8f6f 	isb	sy
 e000586:	4847      	ldr	r0, [pc, #284]	; (e0006a4 <app_start+0x1ac>)
 e000588:	2100      	movs	r1, #0
 e00058a:	4a47      	ldr	r2, [pc, #284]	; (e0006a8 <app_start+0x1b0>)
 e00058c:	4b47      	ldr	r3, [pc, #284]	; (e0006ac <app_start+0x1b4>)
 e00058e:	1a12      	subs	r2, r2, r0
 e000590:	4d43      	ldr	r5, [pc, #268]	; (e0006a0 <app_start+0x1a8>)
 e000592:	4798      	blx	r3
 e000594:	4c46      	ldr	r4, [pc, #280]	; (e0006b0 <app_start+0x1b8>)
 e000596:	4b47      	ldr	r3, [pc, #284]	; (e0006b4 <app_start+0x1bc>)
 e000598:	4847      	ldr	r0, [pc, #284]	; (e0006b8 <app_start+0x1c0>)
 e00059a:	4798      	blx	r3
 e00059c:	4847      	ldr	r0, [pc, #284]	; (e0006bc <app_start+0x1c4>)
 e00059e:	f000 ff3b 	bl	e001418 <Fault_Hanlder_Redirect>
 e0005a2:	68a9      	ldr	r1, [r5, #8]
 e0005a4:	68a2      	ldr	r2, [r4, #8]
 e0005a6:	2004      	movs	r0, #4
 e0005a8:	4b45      	ldr	r3, [pc, #276]	; (e0006c0 <app_start+0x1c8>)
 e0005aa:	e9cd 1200 	strd	r1, r2, [sp]
 e0005ae:	2249      	movs	r2, #73	; 0x49
 e0005b0:	493a      	ldr	r1, [pc, #232]	; (e00069c <app_start+0x1a4>)
 e0005b2:	f001 ff43 	bl	e00243c <rtk_log_write>
 e0005b6:	f002 fa95 	bl	e002ae4 <rtos_mem_init>
 e0005ba:	68a8      	ldr	r0, [r5, #8]
 e0005bc:	68a4      	ldr	r4, [r4, #8]
 e0005be:	2249      	movs	r2, #73	; 0x49
 e0005c0:	4b3f      	ldr	r3, [pc, #252]	; (e0006c0 <app_start+0x1c8>)
 e0005c2:	4936      	ldr	r1, [pc, #216]	; (e00069c <app_start+0x1a4>)
 e0005c4:	e9cd 0400 	strd	r0, r4, [sp]
 e0005c8:	2004      	movs	r0, #4
 e0005ca:	4c3e      	ldr	r4, [pc, #248]	; (e0006c4 <app_start+0x1cc>)
 e0005cc:	f001 ff36 	bl	e00243c <rtk_log_write>
 e0005d0:	f7ff ff16 	bl	e000400 <app_testmode_status>
 e0005d4:	47a0      	blx	r4
 e0005d6:	f410 3f40 	tst.w	r0, #196608	; 0x30000
 e0005da:	d137      	bne.n	e00064c <app_start+0x154>
 e0005dc:	47a0      	blx	r4
 e0005de:	47a0      	blx	r4
 e0005e0:	f410 2f40 	tst.w	r0, #786432	; 0xc0000
 e0005e4:	bf14      	ite	ne
 e0005e6:	2501      	movne	r5, #1
 e0005e8:	2500      	moveq	r5, #0
 e0005ea:	47a0      	blx	r4
 e0005ec:	f410 3f40 	tst.w	r0, #196608	; 0x30000
 e0005f0:	d103      	bne.n	e0005fa <app_start+0x102>
 e0005f2:	47a0      	blx	r4
 e0005f4:	f410 2f40 	tst.w	r0, #786432	; 0xc0000
 e0005f8:	d12f      	bne.n	e00065a <app_start+0x162>
 e0005fa:	47a0      	blx	r4
 e0005fc:	2d00      	cmp	r5, #0
 e0005fe:	d135      	bne.n	e00066c <app_start+0x174>
 e000600:	4c31      	ldr	r4, [pc, #196]	; (e0006c8 <app_start+0x1d0>)
 e000602:	2249      	movs	r2, #73	; 0x49
 e000604:	e844 f400 	tt	r4, r4
 e000608:	f3c4 5480 	ubfx	r4, r4, #22, #1
 e00060c:	4b2f      	ldr	r3, [pc, #188]	; (e0006cc <app_start+0x1d4>)
 e00060e:	4923      	ldr	r1, [pc, #140]	; (e00069c <app_start+0x1a4>)
 e000610:	2004      	movs	r0, #4
 e000612:	9400      	str	r4, [sp, #0]
 e000614:	f001 ff12 	bl	e00243c <rtk_log_write>
 e000618:	f000 f860 	bl	e0006dc <SystemCoreClockUpdate>
 e00061c:	f000 fa4e 	bl	e000abc <XTAL_INIT>
 e000620:	4b2b      	ldr	r3, [pc, #172]	; (e0006d0 <app_start+0x1d8>)
 e000622:	4798      	blx	r3
 e000624:	b328      	cbz	r0, e000672 <app_start+0x17a>
 e000626:	f007 f897 	bl	e007758 <__libc_init_array>
 e00062a:	f001 fac3 	bl	e001bb4 <mpu_init>
 e00062e:	f7ff febb 	bl	e0003a8 <app_mpu_nocache_init>
 e000632:	f7ff ff11 	bl	e000458 <app_vdd1833_detect>
 e000636:	f06f 017f 	mvn.w	r1, #127	; 0x7f
 e00063a:	4668      	mov	r0, sp
 e00063c:	ea00 0001 	and.w	r0, r0, r1
 e000640:	4685      	mov	sp, r0
 e000642:	b003      	add	sp, #12
 e000644:	e8bd 4030 	ldmia.w	sp!, {r4, r5, lr}
 e000648:	f002 b872 	b.w	e002730 <main>
 e00064c:	47a0      	blx	r4
 e00064e:	f410 2f40 	tst.w	r0, #786432	; 0xc0000
 e000652:	bf14      	ite	ne
 e000654:	2501      	movne	r5, #1
 e000656:	2500      	moveq	r5, #0
 e000658:	e7c7      	b.n	e0005ea <app_start+0xf2>
 e00065a:	47a0      	blx	r4
 e00065c:	f410 1f40 	tst.w	r0, #3145728	; 0x300000
 e000660:	f005 0501 	and.w	r5, r5, #1
 e000664:	bf08      	it	eq
 e000666:	2500      	moveq	r5, #0
 e000668:	2d00      	cmp	r5, #0
 e00066a:	d0c9      	beq.n	e000600 <app_start+0x108>
 e00066c:	4b19      	ldr	r3, [pc, #100]	; (e0006d4 <app_start+0x1dc>)
 e00066e:	4798      	blx	r3
 e000670:	e7c6      	b.n	e000600 <app_start+0x108>
 e000672:	f000 fa21 	bl	e000ab8 <OSC4M_Init>
 e000676:	f247 5030 	movw	r0, #30000	; 0x7530
 e00067a:	f000 f91f 	bl	e0008bc <OSC4M_Calibration>
 e00067e:	f001 fbef 	bl	e001e60 <BOOT_Reason>
 e000682:	0783      	lsls	r3, r0, #30
 e000684:	d4cf      	bmi.n	e000626 <app_start+0x12e>
 e000686:	4b14      	ldr	r3, [pc, #80]	; (e0006d8 <app_start+0x1e0>)
 e000688:	4798      	blx	r3
 e00068a:	2800      	cmp	r0, #0
 e00068c:	d1cb      	bne.n	e000626 <app_start+0x12e>
 e00068e:	f247 5030 	movw	r0, #30000	; 0x7530
 e000692:	f000 f84b 	bl	e00072c <OSC131K_Calibration>
 e000696:	e7c6      	b.n	e000626 <app_start+0x12e>
 e000698:	0e007aa4 	.word	0x0e007aa4
 e00069c:	0e007a78 	.word	0x0e007a78
 e0006a0:	e000ed00 	.word	0xe000ed00
 e0006a4:	2000b180 	.word	0x2000b180
 e0006a8:	2000c0e8 	.word	0x2000c0e8
 e0006ac:	00011de5 	.word	0x00011de5
 e0006b0:	e002ed00 	.word	0xe002ed00
 e0006b4:	0000d3c9 	.word	0x0000d3c9
 e0006b8:	20004000 	.word	0x20004000
 e0006bc:	0e0053e9 	.word	0x0e0053e9
 e0006c0:	0e007ab4 	.word	0x0e007ab4
 e0006c4:	0e006815 	.word	0x0e006815
 e0006c8:	0e0004f9 	.word	0x0e0004f9
 e0006cc:	0e007acc 	.word	0x0e007acc
 e0006d0:	0000c219 	.word	0x0000c219
 e0006d4:	0e000d75 	.word	0x0e000d75
 e0006d8:	0e001ea5 	.word	0x0e001ea5

0e0006dc <SystemCoreClockUpdate>:
 e0006dc:	b508      	push	{r3, lr}
 e0006de:	4b02      	ldr	r3, [pc, #8]	; (e0006e8 <SystemCoreClockUpdate+0xc>)
 e0006e0:	4798      	blx	r3
 e0006e2:	4b02      	ldr	r3, [pc, #8]	; (e0006ec <SystemCoreClockUpdate+0x10>)
 e0006e4:	6018      	str	r0, [r3, #0]
 e0006e6:	bd08      	pop	{r3, pc}
 e0006e8:	000093b5 	.word	0x000093b5
 e0006ec:	2000b020 	.word	0x2000b020

0e0006f0 <Systick_Cmd>:
 e0006f0:	f04f 22e0 	mov.w	r2, #3758153728	; 0xe000e000
 e0006f4:	6913      	ldr	r3, [r2, #16]
 e0006f6:	b128      	cbz	r0, e000704 <Systick_Cmd+0x14>
 e0006f8:	f043 0301 	orr.w	r3, r3, #1
 e0006fc:	6113      	str	r3, [r2, #16]
 e0006fe:	f3bf 8f4f 	dsb	sy
 e000702:	4770      	bx	lr
 e000704:	f023 0301 	bic.w	r3, r3, #1
 e000708:	6113      	str	r3, [r2, #16]
 e00070a:	f3bf 8f4f 	dsb	sy
 e00070e:	4770      	bx	lr

0e000710 <CPU_InInterrupt>:
 e000710:	f3ef 8005 	mrs	r0, IPSR
 e000714:	3800      	subs	r0, #0
 e000716:	bf18      	it	ne
 e000718:	2001      	movne	r0, #1
 e00071a:	4770      	bx	lr

0e00071c <irq_disable_save>:
 e00071c:	f3ef 8010 	mrs	r0, PRIMASK
 e000720:	b672      	cpsid	i
 e000722:	4770      	bx	lr

0e000724 <irq_enable_restore>:
 e000724:	f380 8810 	msr	PRIMASK, r0
 e000728:	4770      	bx	lr
 e00072a:	bf00      	nop

0e00072c <OSC131K_Calibration>:
 e00072c:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e000730:	4957      	ldr	r1, [pc, #348]	; (e000890 <OSC131K_Calibration+0x164>)
 e000732:	b087      	sub	sp, #28
 e000734:	9005      	str	r0, [sp, #20]
 e000736:	6f0b      	ldr	r3, [r1, #112]	; 0x70
 e000738:	055a      	lsls	r2, r3, #21
 e00073a:	d5fc      	bpl.n	e000736 <OSC131K_Calibration+0xa>
 e00073c:	694b      	ldr	r3, [r1, #20]
 e00073e:	4a54      	ldr	r2, [pc, #336]	; (e000890 <OSC131K_Calibration+0x164>)
 e000740:	f023 033f 	bic.w	r3, r3, #63	; 0x3f
 e000744:	614b      	str	r3, [r1, #20]
 e000746:	6913      	ldr	r3, [r2, #16]
 e000748:	07db      	lsls	r3, r3, #31
 e00074a:	d5fc      	bpl.n	e000746 <OSC131K_Calibration+0x1a>
 e00074c:	6953      	ldr	r3, [r2, #20]
 e00074e:	f04f 0b04 	mov.w	fp, #4
 e000752:	2600      	movs	r6, #0
 e000754:	f04f 35ff 	mov.w	r5, #4294967295	; 0xffffffff
 e000758:	f043 0320 	orr.w	r3, r3, #32
 e00075c:	f04f 0a20 	mov.w	sl, #32
 e000760:	4f4c      	ldr	r7, [pc, #304]	; (e000894 <OSC131K_Calibration+0x168>)
 e000762:	4c4d      	ldr	r4, [pc, #308]	; (e000898 <OSC131K_Calibration+0x16c>)
 e000764:	f8df 8144 	ldr.w	r8, [pc, #324]	; e0008ac <OSC131K_Calibration+0x180>
 e000768:	f8df 9124 	ldr.w	r9, [pc, #292]	; e000890 <OSC131K_Calibration+0x164>
 e00076c:	6153      	str	r3, [r2, #20]
 e00076e:	2001      	movs	r0, #1
 e000770:	47b8      	blx	r7
 e000772:	f8d4 3288 	ldr.w	r3, [r4, #648]	; 0x288
 e000776:	f423 3340 	bic.w	r3, r3, #196608	; 0x30000
 e00077a:	ea43 0308 	orr.w	r3, r3, r8
 e00077e:	f8c4 3288 	str.w	r3, [r4, #648]	; 0x288
 e000782:	f8d4 3288 	ldr.w	r3, [r4, #648]	; 0x288
 e000786:	2b00      	cmp	r3, #0
 e000788:	dbfb      	blt.n	e000782 <OSC131K_Calibration+0x56>
 e00078a:	f8d4 3288 	ldr.w	r3, [r4, #648]	; 0x288
 e00078e:	f640 1188 	movw	r1, #2440	; 0x988
 e000792:	b29b      	uxth	r3, r3
 e000794:	428b      	cmp	r3, r1
 e000796:	d821      	bhi.n	e0007dc <OSC131K_Calibration+0xb0>
 e000798:	f640 1189 	movw	r1, #2441	; 0x989
 e00079c:	4650      	mov	r0, sl
 e00079e:	1ac9      	subs	r1, r1, r3
 e0007a0:	42a9      	cmp	r1, r5
 e0007a2:	d204      	bcs.n	e0007ae <OSC131K_Calibration+0x82>
 e0007a4:	f8d9 6014 	ldr.w	r6, [r9, #20]
 e0007a8:	460d      	mov	r5, r1
 e0007aa:	f006 063f 	and.w	r6, r6, #63	; 0x3f
 e0007ae:	f1bb 3fff 	cmp.w	fp, #4294967295	; 0xffffffff
 e0007b2:	d017      	beq.n	e0007e4 <OSC131K_Calibration+0xb8>
 e0007b4:	f04f 0a01 	mov.w	sl, #1
 e0007b8:	f8d9 3014 	ldr.w	r3, [r9, #20]
 e0007bc:	fa0a fa0b 	lsl.w	sl, sl, fp
 e0007c0:	ea4a 0303 	orr.w	r3, sl, r3
 e0007c4:	f8c9 3014 	str.w	r3, [r9, #20]
 e0007c8:	b128      	cbz	r0, e0007d6 <OSC131K_Calibration+0xaa>
 e0007ca:	f8d9 3014 	ldr.w	r3, [r9, #20]
 e0007ce:	ea23 0000 	bic.w	r0, r3, r0
 e0007d2:	f8c9 0014 	str.w	r0, [r9, #20]
 e0007d6:	f10b 3bff 	add.w	fp, fp, #4294967295	; 0xffffffff
 e0007da:	e7c8      	b.n	e00076e <OSC131K_Calibration+0x42>
 e0007dc:	f6a3 1189 	subw	r1, r3, #2441	; 0x989
 e0007e0:	2000      	movs	r0, #0
 e0007e2:	e7dd      	b.n	e0007a0 <OSC131K_Calibration+0x74>
 e0007e4:	42a9      	cmp	r1, r5
 e0007e6:	d819      	bhi.n	e00081c <OSC131K_Calibration+0xf0>
 e0007e8:	4c2c      	ldr	r4, [pc, #176]	; (e00089c <OSC131K_Calibration+0x170>)
 e0007ea:	f640 1089 	movw	r0, #2441	; 0x989
 e0007ee:	4a2c      	ldr	r2, [pc, #176]	; (e0008a0 <OSC131K_Calibration+0x174>)
 e0007f0:	fb01 f404 	mul.w	r4, r1, r4
 e0007f4:	9d05      	ldr	r5, [sp, #20]
 e0007f6:	9100      	str	r1, [sp, #0]
 e0007f8:	fba2 2404 	umull	r2, r4, r2, r4
 e0007fc:	4b29      	ldr	r3, [pc, #164]	; (e0008a4 <OSC131K_Calibration+0x178>)
 e0007fe:	9001      	str	r0, [sp, #4]
 e000800:	2249      	movs	r2, #73	; 0x49
 e000802:	0ae4      	lsrs	r4, r4, #11
 e000804:	4928      	ldr	r1, [pc, #160]	; (e0008a8 <OSC131K_Calibration+0x17c>)
 e000806:	2004      	movs	r0, #4
 e000808:	9503      	str	r5, [sp, #12]
 e00080a:	9402      	str	r4, [sp, #8]
 e00080c:	f001 fe16 	bl	e00243c <rtk_log_write>
 e000810:	42ac      	cmp	r4, r5
 e000812:	d225      	bcs.n	e000860 <OSC131K_Calibration+0x134>
 e000814:	2001      	movs	r0, #1
 e000816:	b007      	add	sp, #28
 e000818:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 e00081c:	4b1c      	ldr	r3, [pc, #112]	; (e000890 <OSC131K_Calibration+0x164>)
 e00081e:	2001      	movs	r0, #1
 e000820:	695a      	ldr	r2, [r3, #20]
 e000822:	f022 023f 	bic.w	r2, r2, #63	; 0x3f
 e000826:	615a      	str	r2, [r3, #20]
 e000828:	695a      	ldr	r2, [r3, #20]
 e00082a:	4316      	orrs	r6, r2
 e00082c:	615e      	str	r6, [r3, #20]
 e00082e:	47b8      	blx	r7
 e000830:	4a19      	ldr	r2, [pc, #100]	; (e000898 <OSC131K_Calibration+0x16c>)
 e000832:	4b1e      	ldr	r3, [pc, #120]	; (e0008ac <OSC131K_Calibration+0x180>)
 e000834:	f8d2 1288 	ldr.w	r1, [r2, #648]	; 0x288
 e000838:	f421 3140 	bic.w	r1, r1, #196608	; 0x30000
 e00083c:	430b      	orrs	r3, r1
 e00083e:	f8c2 3288 	str.w	r3, [r2, #648]	; 0x288
 e000842:	f8d2 3288 	ldr.w	r3, [r2, #648]	; 0x288
 e000846:	2b00      	cmp	r3, #0
 e000848:	dbfb      	blt.n	e000842 <OSC131K_Calibration+0x116>
 e00084a:	f8d2 3288 	ldr.w	r3, [r2, #648]	; 0x288
 e00084e:	f640 1288 	movw	r2, #2440	; 0x988
 e000852:	b29b      	uxth	r3, r3
 e000854:	4293      	cmp	r3, r2
 e000856:	d818      	bhi.n	e00088a <OSC131K_Calibration+0x15e>
 e000858:	f640 1189 	movw	r1, #2441	; 0x989
 e00085c:	1ac9      	subs	r1, r1, r3
 e00085e:	e7c3      	b.n	e0007e8 <OSC131K_Calibration+0xbc>
 e000860:	9805      	ldr	r0, [sp, #20]
 e000862:	2245      	movs	r2, #69	; 0x45
 e000864:	4b12      	ldr	r3, [pc, #72]	; (e0008b0 <OSC131K_Calibration+0x184>)
 e000866:	4910      	ldr	r1, [pc, #64]	; (e0008a8 <OSC131K_Calibration+0x17c>)
 e000868:	e9cd 4000 	strd	r4, r0, [sp]
 e00086c:	2002      	movs	r0, #2
 e00086e:	f001 fde5 	bl	e00243c <rtk_log_write>
 e000872:	4b10      	ldr	r3, [pc, #64]	; (e0008b4 <OSC131K_Calibration+0x188>)
 e000874:	4798      	blx	r3
 e000876:	2800      	cmp	r0, #0
 e000878:	d0cc      	beq.n	e000814 <OSC131K_Calibration+0xe8>
 e00087a:	21b7      	movs	r1, #183	; 0xb7
 e00087c:	480e      	ldr	r0, [pc, #56]	; (e0008b8 <OSC131K_Calibration+0x18c>)
 e00087e:	f006 fd77 	bl	e007370 <io_assert_failed>
 e000882:	2001      	movs	r0, #1
 e000884:	b007      	add	sp, #28
 e000886:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 e00088a:	f6a3 1189 	subw	r1, r3, #2441	; 0x989
 e00088e:	e7ab      	b.n	e0007e8 <OSC131K_Calibration+0xbc>
 e000890:	41008100 	.word	0x41008100
 e000894:	0000951d 	.word	0x0000951d
 e000898:	41008000 	.word	0x41008000
 e00089c:	000f4240 	.word	0x000f4240
 e0008a0:	d6c8bb19 	.word	0xd6c8bb19
 e0008a4:	0e007ae4 	.word	0x0e007ae4
 e0008a8:	0e007b20 	.word	0x0e007b20
 e0008ac:	80020000 	.word	0x80020000
 e0008b0:	0e007b24 	.word	0x0e007b24
 e0008b4:	0000c15d 	.word	0x0000c15d
 e0008b8:	0e007b5c 	.word	0x0e007b5c

0e0008bc <OSC4M_Calibration>:
 e0008bc:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e0008c0:	f8df a1e8 	ldr.w	sl, [pc, #488]	; e000aac <OSC4M_Calibration+0x1f0>
 e0008c4:	b085      	sub	sp, #20
 e0008c6:	f44f 69a0 	mov.w	r9, #1280	; 0x500
 e0008ca:	2600      	movs	r6, #0
 e0008cc:	f8da 3004 	ldr.w	r3, [sl, #4]
 e0008d0:	4680      	mov	r8, r0
 e0008d2:	f04f 35ff 	mov.w	r5, #4294967295	; 0xffffffff
 e0008d6:	4f6f      	ldr	r7, [pc, #444]	; (e000a94 <OSC4M_Calibration+0x1d8>)
 e0008d8:	f043 0301 	orr.w	r3, r3, #1
 e0008dc:	4c6e      	ldr	r4, [pc, #440]	; (e000a98 <OSC4M_Calibration+0x1dc>)
 e0008de:	f8ca 3004 	str.w	r3, [sl, #4]
 e0008e2:	f8da 300c 	ldr.w	r3, [sl, #12]
 e0008e6:	b2db      	uxtb	r3, r3
 e0008e8:	f8ca 300c 	str.w	r3, [sl, #12]
 e0008ec:	f8da 300c 	ldr.w	r3, [sl, #12]
 e0008f0:	2002      	movs	r0, #2
 e0008f2:	f423 437f 	bic.w	r3, r3, #65280	; 0xff00
 e0008f6:	ea43 0309 	orr.w	r3, r3, r9
 e0008fa:	ea43 1309 	orr.w	r3, r3, r9, lsl #4
 e0008fe:	f8ca 300c 	str.w	r3, [sl, #12]
 e000902:	47b8      	blx	r7
 e000904:	f8d4 3288 	ldr.w	r3, [r4, #648]	; 0x288
 e000908:	f423 3340 	bic.w	r3, r3, #196608	; 0x30000
 e00090c:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
 e000910:	f8c4 3288 	str.w	r3, [r4, #648]	; 0x288
 e000914:	f8d4 3288 	ldr.w	r3, [r4, #648]	; 0x288
 e000918:	2b00      	cmp	r3, #0
 e00091a:	dbfb      	blt.n	e000914 <OSC4M_Calibration+0x58>
 e00091c:	f8d4 3288 	ldr.w	r3, [r4, #648]	; 0x288
 e000920:	b29b      	uxth	r3, r3
 e000922:	f5b3 7fa0 	cmp.w	r3, #320	; 0x140
 e000926:	d25a      	bcs.n	e0009de <OSC4M_Calibration+0x122>
 e000928:	f5c3 73a0 	rsb	r3, r3, #320	; 0x140
 e00092c:	42ab      	cmp	r3, r5
 e00092e:	d203      	bcs.n	e000938 <OSC4M_Calibration+0x7c>
 e000930:	f8da 600c 	ldr.w	r6, [sl, #12]
 e000934:	461d      	mov	r5, r3
 e000936:	b2b6      	uxth	r6, r6
 e000938:	f509 7980 	add.w	r9, r9, #256	; 0x100
 e00093c:	f5b9 6f20 	cmp.w	r9, #2560	; 0xa00
 e000940:	d1d4      	bne.n	e0008ec <OSC4M_Calibration+0x30>
 e000942:	42ab      	cmp	r3, r5
 e000944:	d86a      	bhi.n	e000a1c <OSC4M_Calibration+0x160>
 e000946:	f8df a164 	ldr.w	sl, [pc, #356]	; e000aac <OSC4M_Calibration+0x1f0>
 e00094a:	f04f 0980 	mov.w	r9, #128	; 0x80
 e00094e:	f04f 0b06 	mov.w	fp, #6
 e000952:	4c51      	ldr	r4, [pc, #324]	; (e000a98 <OSC4M_Calibration+0x1dc>)
 e000954:	f8da 300c 	ldr.w	r3, [sl, #12]
 e000958:	f403 437f 	and.w	r3, r3, #65280	; 0xff00
 e00095c:	f8ca 300c 	str.w	r3, [sl, #12]
 e000960:	f8da 300c 	ldr.w	r3, [sl, #12]
 e000964:	ea43 0309 	orr.w	r3, r3, r9
 e000968:	f8ca 300c 	str.w	r3, [sl, #12]
 e00096c:	2002      	movs	r0, #2
 e00096e:	47b8      	blx	r7
 e000970:	f8d4 3288 	ldr.w	r3, [r4, #648]	; 0x288
 e000974:	f423 3340 	bic.w	r3, r3, #196608	; 0x30000
 e000978:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
 e00097c:	f8c4 3288 	str.w	r3, [r4, #648]	; 0x288
 e000980:	f8d4 3288 	ldr.w	r3, [r4, #648]	; 0x288
 e000984:	2b00      	cmp	r3, #0
 e000986:	dbfb      	blt.n	e000980 <OSC4M_Calibration+0xc4>
 e000988:	f8d4 3288 	ldr.w	r3, [r4, #648]	; 0x288
 e00098c:	b29b      	uxth	r3, r3
 e00098e:	f5b3 7fa0 	cmp.w	r3, #320	; 0x140
 e000992:	d21f      	bcs.n	e0009d4 <OSC4M_Calibration+0x118>
 e000994:	f5c3 72a0 	rsb	r2, r3, #320	; 0x140
 e000998:	2100      	movs	r1, #0
 e00099a:	42aa      	cmp	r2, r5
 e00099c:	d203      	bcs.n	e0009a6 <OSC4M_Calibration+0xea>
 e00099e:	f8da 600c 	ldr.w	r6, [sl, #12]
 e0009a2:	4615      	mov	r5, r2
 e0009a4:	b2f6      	uxtb	r6, r6
 e0009a6:	f1bb 3fff 	cmp.w	fp, #4294967295	; 0xffffffff
 e0009aa:	d01c      	beq.n	e0009e6 <OSC4M_Calibration+0x12a>
 e0009ac:	f04f 0901 	mov.w	r9, #1
 e0009b0:	f8da 300c 	ldr.w	r3, [sl, #12]
 e0009b4:	fa09 f90b 	lsl.w	r9, r9, fp
 e0009b8:	ea49 0303 	orr.w	r3, r9, r3
 e0009bc:	f8ca 300c 	str.w	r3, [sl, #12]
 e0009c0:	b129      	cbz	r1, e0009ce <OSC4M_Calibration+0x112>
 e0009c2:	f8da 300c 	ldr.w	r3, [sl, #12]
 e0009c6:	ea23 0101 	bic.w	r1, r3, r1
 e0009ca:	f8ca 100c 	str.w	r1, [sl, #12]
 e0009ce:	f10b 3bff 	add.w	fp, fp, #4294967295	; 0xffffffff
 e0009d2:	e7cb      	b.n	e00096c <OSC4M_Calibration+0xb0>
 e0009d4:	d05b      	beq.n	e000a8e <OSC4M_Calibration+0x1d2>
 e0009d6:	f5a3 72a0 	sub.w	r2, r3, #320	; 0x140
 e0009da:	4649      	mov	r1, r9
 e0009dc:	e7dd      	b.n	e00099a <OSC4M_Calibration+0xde>
 e0009de:	d056      	beq.n	e000a8e <OSC4M_Calibration+0x1d2>
 e0009e0:	f5a3 73a0 	sub.w	r3, r3, #320	; 0x140
 e0009e4:	e7a2      	b.n	e00092c <OSC4M_Calibration+0x70>
 e0009e6:	42aa      	cmp	r2, r5
 e0009e8:	d81f      	bhi.n	e000a2a <OSC4M_Calibration+0x16e>
 e0009ea:	4c2c      	ldr	r4, [pc, #176]	; (e000a9c <OSC4M_Calibration+0x1e0>)
 e0009ec:	4b2c      	ldr	r3, [pc, #176]	; (e000aa0 <OSC4M_Calibration+0x1e4>)
 e0009ee:	fb02 f404 	mul.w	r4, r2, r4
 e0009f2:	fba3 3404 	umull	r3, r4, r3, r4
 e0009f6:	0a24      	lsrs	r4, r4, #8
 e0009f8:	f44f 71a0 	mov.w	r1, #320	; 0x140
 e0009fc:	4b29      	ldr	r3, [pc, #164]	; (e000aa4 <OSC4M_Calibration+0x1e8>)
 e0009fe:	2004      	movs	r0, #4
 e000a00:	e9cd 2100 	strd	r2, r1, [sp]
 e000a04:	e9cd 4802 	strd	r4, r8, [sp, #8]
 e000a08:	2249      	movs	r2, #73	; 0x49
 e000a0a:	4927      	ldr	r1, [pc, #156]	; (e000aa8 <OSC4M_Calibration+0x1ec>)
 e000a0c:	f001 fd16 	bl	e00243c <rtk_log_write>
 e000a10:	4544      	cmp	r4, r8
 e000a12:	d22a      	bcs.n	e000a6a <OSC4M_Calibration+0x1ae>
 e000a14:	2001      	movs	r0, #1
 e000a16:	b005      	add	sp, #20
 e000a18:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 e000a1c:	4a23      	ldr	r2, [pc, #140]	; (e000aac <OSC4M_Calibration+0x1f0>)
 e000a1e:	68d3      	ldr	r3, [r2, #12]
 e000a20:	f423 437f 	bic.w	r3, r3, #65280	; 0xff00
 e000a24:	4333      	orrs	r3, r6
 e000a26:	60d3      	str	r3, [r2, #12]
 e000a28:	e78d      	b.n	e000946 <OSC4M_Calibration+0x8a>
 e000a2a:	4a20      	ldr	r2, [pc, #128]	; (e000aac <OSC4M_Calibration+0x1f0>)
 e000a2c:	2002      	movs	r0, #2
 e000a2e:	68d3      	ldr	r3, [r2, #12]
 e000a30:	f023 03ff 	bic.w	r3, r3, #255	; 0xff
 e000a34:	431e      	orrs	r6, r3
 e000a36:	60d6      	str	r6, [r2, #12]
 e000a38:	47b8      	blx	r7
 e000a3a:	4a17      	ldr	r2, [pc, #92]	; (e000a98 <OSC4M_Calibration+0x1dc>)
 e000a3c:	f8d2 3288 	ldr.w	r3, [r2, #648]	; 0x288
 e000a40:	f423 3340 	bic.w	r3, r3, #196608	; 0x30000
 e000a44:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
 e000a48:	f8c2 3288 	str.w	r3, [r2, #648]	; 0x288
 e000a4c:	f8d2 3288 	ldr.w	r3, [r2, #648]	; 0x288
 e000a50:	2b00      	cmp	r3, #0
 e000a52:	dbfb      	blt.n	e000a4c <OSC4M_Calibration+0x190>
 e000a54:	f8d2 3288 	ldr.w	r3, [r2, #648]	; 0x288
 e000a58:	b29b      	uxth	r3, r3
 e000a5a:	f5b3 7fa0 	cmp.w	r3, #320	; 0x140
 e000a5e:	bf34      	ite	cc
 e000a60:	f5c3 72a0 	rsbcc	r2, r3, #320	; 0x140
 e000a64:	f5a3 72a0 	subcs.w	r2, r3, #320	; 0x140
 e000a68:	e7bf      	b.n	e0009ea <OSC4M_Calibration+0x12e>
 e000a6a:	4b11      	ldr	r3, [pc, #68]	; (e000ab0 <OSC4M_Calibration+0x1f4>)
 e000a6c:	2257      	movs	r2, #87	; 0x57
 e000a6e:	490e      	ldr	r1, [pc, #56]	; (e000aa8 <OSC4M_Calibration+0x1ec>)
 e000a70:	2003      	movs	r0, #3
 e000a72:	f8cd 8004 	str.w	r8, [sp, #4]
 e000a76:	9400      	str	r4, [sp, #0]
 e000a78:	f001 fce0 	bl	e00243c <rtk_log_write>
 e000a7c:	f44f 71b5 	mov.w	r1, #362	; 0x16a
 e000a80:	480c      	ldr	r0, [pc, #48]	; (e000ab4 <OSC4M_Calibration+0x1f8>)
 e000a82:	f006 fc75 	bl	e007370 <io_assert_failed>
 e000a86:	2001      	movs	r0, #1
 e000a88:	b005      	add	sp, #20
 e000a8a:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 e000a8e:	2400      	movs	r4, #0
 e000a90:	4622      	mov	r2, r4
 e000a92:	e7b1      	b.n	e0009f8 <OSC4M_Calibration+0x13c>
 e000a94:	000094a5 	.word	0x000094a5
 e000a98:	41008000 	.word	0x41008000
 e000a9c:	000f4240 	.word	0x000f4240
 e000aa0:	cccccccd 	.word	0xcccccccd
 e000aa4:	0e007b94 	.word	0x0e007b94
 e000aa8:	0e007b20 	.word	0x0e007b20
 e000aac:	41008100 	.word	0x41008100
 e000ab0:	0e007bcc 	.word	0x0e007bcc
 e000ab4:	0e007b5c 	.word	0x0e007b5c

0e000ab8 <OSC4M_Init>:
 e000ab8:	4770      	bx	lr
 e000aba:	bf00      	nop

0e000abc <XTAL_INIT>:
 e000abc:	4b08      	ldr	r3, [pc, #32]	; (e000ae0 <XTAL_INIT+0x24>)
 e000abe:	4909      	ldr	r1, [pc, #36]	; (e000ae4 <XTAL_INIT+0x28>)
 e000ac0:	4a09      	ldr	r2, [pc, #36]	; (e000ae8 <XTAL_INIT+0x2c>)
 e000ac2:	480a      	ldr	r0, [pc, #40]	; (e000aec <XTAL_INIT+0x30>)
 e000ac4:	b410      	push	{r4}
 e000ac6:	6059      	str	r1, [r3, #4]
 e000ac8:	f640 34e9 	movw	r4, #3049	; 0xbe9
 e000acc:	609a      	str	r2, [r3, #8]
 e000ace:	2168      	movs	r1, #104	; 0x68
 e000ad0:	2206      	movs	r2, #6
 e000ad2:	60dc      	str	r4, [r3, #12]
 e000ad4:	6118      	str	r0, [r3, #16]
 e000ad6:	6159      	str	r1, [r3, #20]
 e000ad8:	619a      	str	r2, [r3, #24]
 e000ada:	f85d 4b04 	ldr.w	r4, [sp], #4
 e000ade:	4770      	bx	lr
 e000ae0:	41008400 	.word	0x41008400
 e000ae4:	008103ff 	.word	0x008103ff
 e000ae8:	fc3b9e80 	.word	0xfc3b9e80
 e000aec:	cc0e0190 	.word	0xcc0e0190

0e000af0 <DATA_FLASH_PLLInit_ClockDiv>:
 e000af0:	2200      	movs	r2, #0
 e000af2:	f44f 5100 	mov.w	r1, #8192	; 0x2000
 e000af6:	b570      	push	{r4, r5, r6, lr}
 e000af8:	4c16      	ldr	r4, [pc, #88]	; (e000b54 <DATA_FLASH_PLLInit_ClockDiv+0x64>)
 e000afa:	4605      	mov	r5, r0
 e000afc:	4e16      	ldr	r6, [pc, #88]	; (e000b58 <DATA_FLASH_PLLInit_ClockDiv+0x68>)
 e000afe:	4610      	mov	r0, r2
 e000b00:	47b0      	blx	r6
 e000b02:	f8d4 324c 	ldr.w	r3, [r4, #588]	; 0x24c
 e000b06:	2001      	movs	r0, #1
 e000b08:	022d      	lsls	r5, r5, #8
 e000b0a:	f423 5300 	bic.w	r3, r3, #8192	; 0x2000
 e000b0e:	f405 6570 	and.w	r5, r5, #3840	; 0xf00
 e000b12:	f8c4 324c 	str.w	r3, [r4, #588]	; 0x24c
 e000b16:	4b11      	ldr	r3, [pc, #68]	; (e000b5c <DATA_FLASH_PLLInit_ClockDiv+0x6c>)
 e000b18:	4798      	blx	r3
 e000b1a:	f8d4 224c 	ldr.w	r2, [r4, #588]	; 0x24c
 e000b1e:	230a      	movs	r3, #10
 e000b20:	f422 6270 	bic.w	r2, r2, #3840	; 0xf00
 e000b24:	4315      	orrs	r5, r2
 e000b26:	f8c4 524c 	str.w	r5, [r4, #588]	; 0x24c
 e000b2a:	f8d4 224c 	ldr.w	r2, [r4, #588]	; 0x24c
 e000b2e:	f442 5200 	orr.w	r2, r2, #8192	; 0x2000
 e000b32:	f8c4 224c 	str.w	r2, [r4, #588]	; 0x24c
 e000b36:	e001      	b.n	e000b3c <DATA_FLASH_PLLInit_ClockDiv+0x4c>
 e000b38:	07d2      	lsls	r2, r2, #31
 e000b3a:	d403      	bmi.n	e000b44 <DATA_FLASH_PLLInit_ClockDiv+0x54>
 e000b3c:	3b01      	subs	r3, #1
 e000b3e:	f8d4 224c 	ldr.w	r2, [r4, #588]	; 0x24c
 e000b42:	d1f9      	bne.n	e000b38 <DATA_FLASH_PLLInit_ClockDiv+0x48>
 e000b44:	2000      	movs	r0, #0
 e000b46:	2201      	movs	r2, #1
 e000b48:	f44f 5100 	mov.w	r1, #8192	; 0x2000
 e000b4c:	47b0      	blx	r6
 e000b4e:	2001      	movs	r0, #1
 e000b50:	bd70      	pop	{r4, r5, r6, pc}
 e000b52:	bf00      	nop
 e000b54:	41008000 	.word	0x41008000
 e000b58:	0000bebd 	.word	0x0000bebd
 e000b5c:	0000c001 	.word	0x0000c001

0e000b60 <DATA_FLASH_Read_DataIsRight>:
 e000b60:	4b16      	ldr	r3, [pc, #88]	; (e000bbc <DATA_FLASH_Read_DataIsRight+0x5c>)
 e000b62:	b510      	push	{r4, lr}
 e000b64:	695a      	ldr	r2, [r3, #20]
 e000b66:	b084      	sub	sp, #16
 e000b68:	03d2      	lsls	r2, r2, #15
 e000b6a:	d509      	bpl.n	e000b80 <DATA_FLASH_Read_DataIsRight+0x20>
 e000b6c:	f3bf 8f4f 	dsb	sy
 e000b70:	f04f 42c0 	mov.w	r2, #1610612736	; 0x60000000
 e000b74:	f8c3 225c 	str.w	r2, [r3, #604]	; 0x25c
 e000b78:	f3bf 8f4f 	dsb	sy
 e000b7c:	f3bf 8f6f 	isb	sy
 e000b80:	f04f 43c0 	mov.w	r3, #1610612736	; 0x60000000
 e000b84:	4a0e      	ldr	r2, [pc, #56]	; (e000bc0 <DATA_FLASH_Read_DataIsRight+0x60>)
 e000b86:	6818      	ldr	r0, [r3, #0]
 e000b88:	79d2      	ldrb	r2, [r2, #7]
 e000b8a:	9002      	str	r0, [sp, #8]
 e000b8c:	685c      	ldr	r4, [r3, #4]
 e000b8e:	9403      	str	r4, [sp, #12]
 e000b90:	b952      	cbnz	r2, e000ba8 <DATA_FLASH_Read_DataIsRight+0x48>
 e000b92:	2208      	movs	r2, #8
 e000b94:	490b      	ldr	r1, [pc, #44]	; (e000bc4 <DATA_FLASH_Read_DataIsRight+0x64>)
 e000b96:	4b0c      	ldr	r3, [pc, #48]	; (e000bc8 <DATA_FLASH_Read_DataIsRight+0x68>)
 e000b98:	eb0d 0002 	add.w	r0, sp, r2
 e000b9c:	4798      	blx	r3
 e000b9e:	fab0 f080 	clz	r0, r0
 e000ba2:	0940      	lsrs	r0, r0, #5
 e000ba4:	b004      	add	sp, #16
 e000ba6:	bd10      	pop	{r4, pc}
 e000ba8:	e9cd 0400 	strd	r0, r4, [sp]
 e000bac:	4b07      	ldr	r3, [pc, #28]	; (e000bcc <DATA_FLASH_Read_DataIsRight+0x6c>)
 e000bae:	2249      	movs	r2, #73	; 0x49
 e000bb0:	4907      	ldr	r1, [pc, #28]	; (e000bd0 <DATA_FLASH_Read_DataIsRight+0x70>)
 e000bb2:	2004      	movs	r0, #4
 e000bb4:	f001 fc42 	bl	e00243c <rtk_log_write>
 e000bb8:	e7eb      	b.n	e000b92 <DATA_FLASH_Read_DataIsRight+0x32>
 e000bba:	bf00      	nop
 e000bbc:	e000ed00 	.word	0xe000ed00
 e000bc0:	2000b188 	.word	0x2000b188
 e000bc4:	0004484c 	.word	0x0004484c
 e000bc8:	00011bbd 	.word	0x00011bbd
 e000bcc:	0e007c04 	.word	0x0e007c04
 e000bd0:	0e007c24 	.word	0x0e007c24

0e000bd4 <data_flash_handshake_highspeed>:
 e000bd4:	b5f0      	push	{r4, r5, r6, r7, lr}
 e000bd6:	4c25      	ldr	r4, [pc, #148]	; (e000c6c <data_flash_handshake_highspeed+0x98>)
 e000bd8:	b083      	sub	sp, #12
 e000bda:	4b25      	ldr	r3, [pc, #148]	; (e000c70 <data_flash_handshake_highspeed+0x9c>)
 e000bdc:	4607      	mov	r7, r0
 e000bde:	4798      	blx	r3
 e000be0:	f8d4 324c 	ldr.w	r3, [r4, #588]	; 0x24c
 e000be4:	4e23      	ldr	r6, [pc, #140]	; (e000c74 <data_flash_handshake_highspeed+0xa0>)
 e000be6:	f423 13f0 	bic.w	r3, r3, #1966080	; 0x1e0000
 e000bea:	4a23      	ldr	r2, [pc, #140]	; (e000c78 <data_flash_handshake_highspeed+0xa4>)
 e000bec:	f443 23c0 	orr.w	r3, r3, #393216	; 0x60000
 e000bf0:	f8c4 324c 	str.w	r3, [r4, #588]	; 0x24c
 e000bf4:	f8d4 324c 	ldr.w	r3, [r4, #588]	; 0x24c
 e000bf8:	f443 3380 	orr.w	r3, r3, #65536	; 0x10000
 e000bfc:	f8c4 324c 	str.w	r3, [r4, #588]	; 0x24c
 e000c00:	f8d6 3110 	ldr.w	r3, [r6, #272]	; 0x110
 e000c04:	f443 3380 	orr.w	r3, r3, #65536	; 0x10000
 e000c08:	f8c6 3110 	str.w	r3, [r6, #272]	; 0x110
 e000c0c:	4790      	blx	r2
 e000c0e:	b160      	cbz	r0, e000c2a <data_flash_handshake_highspeed+0x56>
 e000c10:	2501      	movs	r5, #1
 e000c12:	491a      	ldr	r1, [pc, #104]	; (e000c7c <data_flash_handshake_highspeed+0xa8>)
 e000c14:	4b1a      	ldr	r3, [pc, #104]	; (e000c80 <data_flash_handshake_highspeed+0xac>)
 e000c16:	2249      	movs	r2, #73	; 0x49
 e000c18:	2004      	movs	r0, #4
 e000c1a:	e9cd 7100 	strd	r7, r1, [sp]
 e000c1e:	4919      	ldr	r1, [pc, #100]	; (e000c84 <data_flash_handshake_highspeed+0xb0>)
 e000c20:	f001 fc0c 	bl	e00243c <rtk_log_write>
 e000c24:	4628      	mov	r0, r5
 e000c26:	b003      	add	sp, #12
 e000c28:	bdf0      	pop	{r4, r5, r6, r7, pc}
 e000c2a:	f8d4 324c 	ldr.w	r3, [r4, #588]	; 0x24c
 e000c2e:	4605      	mov	r5, r0
 e000c30:	f423 13f0 	bic.w	r3, r3, #1966080	; 0x1e0000
 e000c34:	f443 23c0 	orr.w	r3, r3, #393216	; 0x60000
 e000c38:	f8c4 324c 	str.w	r3, [r4, #588]	; 0x24c
 e000c3c:	f8d4 324c 	ldr.w	r3, [r4, #588]	; 0x24c
 e000c40:	f423 3380 	bic.w	r3, r3, #65536	; 0x10000
 e000c44:	f8c4 324c 	str.w	r3, [r4, #588]	; 0x24c
 e000c48:	f8d6 3110 	ldr.w	r3, [r6, #272]	; 0x110
 e000c4c:	f423 3380 	bic.w	r3, r3, #65536	; 0x10000
 e000c50:	f8c6 3110 	str.w	r3, [r6, #272]	; 0x110
 e000c54:	4b0c      	ldr	r3, [pc, #48]	; (e000c88 <data_flash_handshake_highspeed+0xb4>)
 e000c56:	4798      	blx	r3
 e000c58:	f8d6 3140 	ldr.w	r3, [r6, #320]	; 0x140
 e000c5c:	490b      	ldr	r1, [pc, #44]	; (e000c8c <data_flash_handshake_highspeed+0xb8>)
 e000c5e:	f023 030f 	bic.w	r3, r3, #15
 e000c62:	f043 0301 	orr.w	r3, r3, #1
 e000c66:	f8c6 3140 	str.w	r3, [r6, #320]	; 0x140
 e000c6a:	e7d3      	b.n	e000c14 <data_flash_handshake_highspeed+0x40>
 e000c6c:	41008000 	.word	0x41008000
 e000c70:	0e000af1 	.word	0x0e000af1
 e000c74:	40127000 	.word	0x40127000
 e000c78:	0e000b61 	.word	0x0e000b61
 e000c7c:	0e007c38 	.word	0x0e007c38
 e000c80:	0e007c3c 	.word	0x0e007c3c
 e000c84:	0e007c24 	.word	0x0e007c24
 e000c88:	0000c001 	.word	0x0000c001
 e000c8c:	0e007c30 	.word	0x0e007c30

0e000c90 <data_flash_rx_mode_switch>:
 e000c90:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
 e000c94:	2300      	movs	r3, #0
 e000c96:	b089      	sub	sp, #36	; 0x24
 e000c98:	f8df e0d0 	ldr.w	lr, [pc, #208]	; e000d6c <data_flash_rx_mode_switch+0xdc>
 e000c9c:	4604      	mov	r4, r0
 e000c9e:	f10d 0c0c 	add.w	ip, sp, #12
 e000ca2:	f88d 300b 	strb.w	r3, [sp, #11]
 e000ca6:	4d2a      	ldr	r5, [pc, #168]	; (e000d50 <data_flash_rx_mode_switch+0xc0>)
 e000ca8:	4e2a      	ldr	r6, [pc, #168]	; (e000d54 <data_flash_rx_mode_switch+0xc4>)
 e000caa:	f8df 90c4 	ldr.w	r9, [pc, #196]	; e000d70 <data_flash_rx_mode_switch+0xe0>
 e000cae:	f105 084e 	add.w	r8, r5, #78	; 0x4e
 e000cb2:	4f29      	ldr	r7, [pc, #164]	; (e000d58 <data_flash_rx_mode_switch+0xc8>)
 e000cb4:	e8be 000f 	ldmia.w	lr!, {r0, r1, r2, r3}
 e000cb8:	e8ac 000f 	stmia.w	ip!, {r0, r1, r2, r3}
 e000cbc:	f8de 3000 	ldr.w	r3, [lr]
 e000cc0:	f8cc 3000 	str.w	r3, [ip]
 e000cc4:	e006      	b.n	e000cd4 <data_flash_rx_mode_switch+0x44>
 e000cc6:	b2e0      	uxtb	r0, r4
 e000cc8:	47b0      	blx	r6
 e000cca:	4b24      	ldr	r3, [pc, #144]	; (e000d5c <data_flash_rx_mode_switch+0xcc>)
 e000ccc:	b334      	cbz	r4, e000d1c <data_flash_rx_mode_switch+0x8c>
 e000cce:	4798      	blx	r3
 e000cd0:	bb70      	cbnz	r0, e000d30 <data_flash_rx_mode_switch+0xa0>
 e000cd2:	3c01      	subs	r4, #1
 e000cd4:	2c02      	cmp	r4, #2
 e000cd6:	bf18      	it	ne
 e000cd8:	2c04      	cmpne	r4, #4
 e000cda:	682b      	ldr	r3, [r5, #0]
 e000cdc:	bf0c      	ite	eq
 e000cde:	2201      	moveq	r2, #1
 e000ce0:	2200      	movne	r2, #0
 e000ce2:	2b03      	cmp	r3, #3
 e000ce4:	f885 2055 	strb.w	r2, [r5, #85]	; 0x55
 e000ce8:	d1ed      	bne.n	e000cc6 <data_flash_rx_mode_switch+0x36>
 e000cea:	2101      	movs	r1, #1
 e000cec:	2085      	movs	r0, #133	; 0x85
 e000cee:	f10d 020b 	add.w	r2, sp, #11
 e000cf2:	47c8      	blx	r9
 e000cf4:	f89d 300b 	ldrb.w	r3, [sp, #11]
 e000cf8:	f814 0008 	ldrb.w	r0, [r4, r8]
 e000cfc:	f10d 020b 	add.w	r2, sp, #11
 e000d00:	f003 030f 	and.w	r3, r3, #15
 e000d04:	2101      	movs	r1, #1
 e000d06:	ea43 1300 	orr.w	r3, r3, r0, lsl #4
 e000d0a:	2081      	movs	r0, #129	; 0x81
 e000d0c:	f88d 300b 	strb.w	r3, [sp, #11]
 e000d10:	47b8      	blx	r7
 e000d12:	b2e0      	uxtb	r0, r4
 e000d14:	47b0      	blx	r6
 e000d16:	4b11      	ldr	r3, [pc, #68]	; (e000d5c <data_flash_rx_mode_switch+0xcc>)
 e000d18:	2c00      	cmp	r4, #0
 e000d1a:	d1d8      	bne.n	e000cce <data_flash_rx_mode_switch+0x3e>
 e000d1c:	4b10      	ldr	r3, [pc, #64]	; (e000d60 <data_flash_rx_mode_switch+0xd0>)
 e000d1e:	2245      	movs	r2, #69	; 0x45
 e000d20:	4910      	ldr	r1, [pc, #64]	; (e000d64 <data_flash_rx_mode_switch+0xd4>)
 e000d22:	2002      	movs	r0, #2
 e000d24:	f001 fb8a 	bl	e00243c <rtk_log_write>
 e000d28:	4620      	mov	r0, r4
 e000d2a:	b009      	add	sp, #36	; 0x24
 e000d2c:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 e000d30:	ab08      	add	r3, sp, #32
 e000d32:	2249      	movs	r2, #73	; 0x49
 e000d34:	490b      	ldr	r1, [pc, #44]	; (e000d64 <data_flash_rx_mode_switch+0xd4>)
 e000d36:	2004      	movs	r0, #4
 e000d38:	eb03 0484 	add.w	r4, r3, r4, lsl #2
 e000d3c:	4b0a      	ldr	r3, [pc, #40]	; (e000d68 <data_flash_rx_mode_switch+0xd8>)
 e000d3e:	f854 4c14 	ldr.w	r4, [r4, #-20]
 e000d42:	9400      	str	r4, [sp, #0]
 e000d44:	f001 fb7a 	bl	e00243c <rtk_log_write>
 e000d48:	2001      	movs	r0, #1
 e000d4a:	b009      	add	sp, #36	; 0x24
 e000d4c:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 e000d50:	2000b188 	.word	0x2000b188
 e000d54:	0e001241 	.word	0x0e001241
 e000d58:	0e001155 	.word	0x0e001155
 e000d5c:	0e000b61 	.word	0x0e000b61
 e000d60:	0e007c5c 	.word	0x0e007c5c
 e000d64:	0e007c24 	.word	0x0e007c24
 e000d68:	0e007c80 	.word	0x0e007c80
 e000d6c:	0e007d0c 	.word	0x0e007d0c
 e000d70:	0e001051 	.word	0x0e001051

0e000d74 <data_flash_highspeed_setup>:
 e000d74:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
 e000d78:	4d7f      	ldr	r5, [pc, #508]	; (e000f78 <data_flash_highspeed_setup+0x204>)
 e000d7a:	b087      	sub	sp, #28
 e000d7c:	4b7f      	ldr	r3, [pc, #508]	; (e000f7c <data_flash_highspeed_setup+0x208>)
 e000d7e:	4798      	blx	r3
 e000d80:	4f7f      	ldr	r7, [pc, #508]	; (e000f80 <data_flash_highspeed_setup+0x20c>)
 e000d82:	4604      	mov	r4, r0
 e000d84:	4628      	mov	r0, r5
 e000d86:	47b8      	blx	r7
 e000d88:	4a7e      	ldr	r2, [pc, #504]	; (e000f84 <data_flash_highspeed_setup+0x210>)
 e000d8a:	2301      	movs	r3, #1
 e000d8c:	8810      	ldrh	r0, [r2, #0]
 e000d8e:	716b      	strb	r3, [r5, #5]
 e000d90:	71ab      	strb	r3, [r5, #6]
 e000d92:	f000 fb8f 	bl	e0014b4 <flash_get_readmode>
 e000d96:	4b7c      	ldr	r3, [pc, #496]	; (e000f88 <data_flash_highspeed_setup+0x214>)
 e000d98:	4606      	mov	r6, r0
 e000d9a:	4a7c      	ldr	r2, [pc, #496]	; (e000f8c <data_flash_highspeed_setup+0x218>)
 e000d9c:	4423      	add	r3, r4
 e000d9e:	fba2 2303 	umull	r2, r3, r2, r3
 e000da2:	4a7b      	ldr	r2, [pc, #492]	; (e000f90 <data_flash_highspeed_setup+0x21c>)
 e000da4:	8814      	ldrh	r4, [r2, #0]
 e000da6:	0edb      	lsrs	r3, r3, #27
 e000da8:	3c01      	subs	r4, #1
 e000daa:	3b01      	subs	r3, #1
 e000dac:	b2e4      	uxtb	r4, r4
 e000dae:	429c      	cmp	r4, r3
 e000db0:	bf38      	it	cc
 e000db2:	461c      	movcc	r4, r3
 e000db4:	b672      	cpsid	i
 e000db6:	b2e4      	uxtb	r4, r4
 e000db8:	429c      	cmp	r4, r3
 e000dba:	d108      	bne.n	e000dce <data_flash_highspeed_setup+0x5a>
 e000dbc:	4a75      	ldr	r2, [pc, #468]	; (e000f94 <data_flash_highspeed_setup+0x220>)
 e000dbe:	f8d2 3140 	ldr.w	r3, [r2, #320]	; 0x140
 e000dc2:	f023 030f 	bic.w	r3, r3, #15
 e000dc6:	f043 0302 	orr.w	r3, r3, #2
 e000dca:	f8c2 3140 	str.w	r3, [r2, #320]	; 0x140
 e000dce:	2103      	movs	r1, #3
 e000dd0:	f895 0041 	ldrb.w	r0, [r5, #65]	; 0x41
 e000dd4:	aa05      	add	r2, sp, #20
 e000dd6:	f8df 81ec 	ldr.w	r8, [pc, #492]	; e000fc4 <data_flash_highspeed_setup+0x250>
 e000dda:	47c0      	blx	r8
 e000ddc:	f89d 3016 	ldrb.w	r3, [sp, #22]
 e000de0:	2249      	movs	r2, #73	; 0x49
 e000de2:	496d      	ldr	r1, [pc, #436]	; (e000f98 <data_flash_highspeed_setup+0x224>)
 e000de4:	9302      	str	r3, [sp, #8]
 e000de6:	2004      	movs	r0, #4
 e000de8:	f89d 3015 	ldrb.w	r3, [sp, #21]
 e000dec:	f8df 91d8 	ldr.w	r9, [pc, #472]	; e000fc8 <data_flash_highspeed_setup+0x254>
 e000df0:	9301      	str	r3, [sp, #4]
 e000df2:	f89d 3014 	ldrb.w	r3, [sp, #20]
 e000df6:	9300      	str	r3, [sp, #0]
 e000df8:	4b68      	ldr	r3, [pc, #416]	; (e000f9c <data_flash_highspeed_setup+0x228>)
 e000dfa:	f001 fb1f 	bl	e00243c <rtk_log_write>
 e000dfe:	f89d 3015 	ldrb.w	r3, [sp, #21]
 e000e02:	f89d 0016 	ldrb.w	r0, [sp, #22]
 e000e06:	021b      	lsls	r3, r3, #8
 e000e08:	ea43 4300 	orr.w	r3, r3, r0, lsl #16
 e000e0c:	f89d 0014 	ldrb.w	r0, [sp, #20]
 e000e10:	4318      	orrs	r0, r3
 e000e12:	f000 fb6d 	bl	e0014f0 <flash_get_chip_info>
 e000e16:	f8c9 0000 	str.w	r0, [r9]
 e000e1a:	2800      	cmp	r0, #0
 e000e1c:	f000 8090 	beq.w	e000f40 <data_flash_highspeed_setup+0x1cc>
 e000e20:	7a03      	ldrb	r3, [r0, #8]
 e000e22:	2b05      	cmp	r3, #5
 e000e24:	d809      	bhi.n	e000e3a <data_flash_highspeed_setup+0xc6>
 e000e26:	3b01      	subs	r3, #1
 e000e28:	4853      	ldr	r0, [pc, #332]	; (e000f78 <data_flash_highspeed_setup+0x204>)
 e000e2a:	2b04      	cmp	r3, #4
 e000e2c:	f200 809f 	bhi.w	e000f6e <data_flash_highspeed_setup+0x1fa>
 e000e30:	e8df f003 	tbb	[pc, r3]
 e000e34:	5a7c7669 	.word	0x5a7c7669
 e000e38:	60          	.byte	0x60
 e000e39:	00          	.byte	0x00
 e000e3a:	2bfe      	cmp	r3, #254	; 0xfe
 e000e3c:	d106      	bne.n	e000e4c <data_flash_highspeed_setup+0xd8>
 e000e3e:	6903      	ldr	r3, [r0, #16]
 e000e40:	2b00      	cmp	r3, #0
 e000e42:	f000 808b 	beq.w	e000f5c <data_flash_highspeed_setup+0x1e8>
 e000e46:	4798      	blx	r3
 e000e48:	f8d9 0000 	ldr.w	r0, [r9]
 e000e4c:	68eb      	ldr	r3, [r5, #12]
 e000e4e:	f8d0 900c 	ldr.w	r9, [r0, #12]
 e000e52:	9304      	str	r3, [sp, #16]
 e000e54:	aa05      	add	r2, sp, #20
 e000e56:	2101      	movs	r1, #1
 e000e58:	f895 0042 	ldrb.w	r0, [r5, #66]	; 0x42
 e000e5c:	47c0      	blx	r8
 e000e5e:	69ab      	ldr	r3, [r5, #24]
 e000e60:	bb2b      	cbnz	r3, e000eae <data_flash_highspeed_setup+0x13a>
 e000e62:	4b4f      	ldr	r3, [pc, #316]	; (e000fa0 <data_flash_highspeed_setup+0x22c>)
 e000e64:	781b      	ldrb	r3, [r3, #0]
 e000e66:	2b00      	cmp	r3, #0
 e000e68:	d133      	bne.n	e000ed2 <data_flash_highspeed_setup+0x15e>
 e000e6a:	2701      	movs	r7, #1
 e000e6c:	9b05      	ldr	r3, [sp, #20]
 e000e6e:	a805      	add	r0, sp, #20
 e000e70:	463a      	mov	r2, r7
 e000e72:	a904      	add	r1, sp, #16
 e000e74:	ea03 0309 	and.w	r3, r3, r9
 e000e78:	9305      	str	r3, [sp, #20]
 e000e7a:	4b4a      	ldr	r3, [pc, #296]	; (e000fa4 <data_flash_highspeed_setup+0x230>)
 e000e7c:	4798      	blx	r3
 e000e7e:	b140      	cbz	r0, e000e92 <data_flash_highspeed_setup+0x11e>
 e000e80:	f895 3045 	ldrb.w	r3, [r5, #69]	; 0x45
 e000e84:	aa04      	add	r2, sp, #16
 e000e86:	f895 0044 	ldrb.w	r0, [r5, #68]	; 0x44
 e000e8a:	b9c3      	cbnz	r3, e000ebe <data_flash_highspeed_setup+0x14a>
 e000e8c:	4639      	mov	r1, r7
 e000e8e:	4b46      	ldr	r3, [pc, #280]	; (e000fa8 <data_flash_highspeed_setup+0x234>)
 e000e90:	4798      	blx	r3
 e000e92:	b2f0      	uxtb	r0, r6
 e000e94:	f7ff fefc 	bl	e000c90 <data_flash_rx_mode_switch>
 e000e98:	4b44      	ldr	r3, [pc, #272]	; (e000fac <data_flash_highspeed_setup+0x238>)
 e000e9a:	4798      	blx	r3
 e000e9c:	2801      	cmp	r0, #1
 e000e9e:	d002      	beq.n	e000ea6 <data_flash_highspeed_setup+0x132>
 e000ea0:	4620      	mov	r0, r4
 e000ea2:	f7ff fe97 	bl	e000bd4 <data_flash_handshake_highspeed>
 e000ea6:	b662      	cpsie	i
 e000ea8:	b007      	add	sp, #28
 e000eaa:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 e000eae:	f10d 0215 	add.w	r2, sp, #21
 e000eb2:	2101      	movs	r1, #1
 e000eb4:	f895 0043 	ldrb.w	r0, [r5, #67]	; 0x43
 e000eb8:	2702      	movs	r7, #2
 e000eba:	47c0      	blx	r8
 e000ebc:	e7d6      	b.n	e000e6c <data_flash_highspeed_setup+0xf8>
 e000ebe:	2101      	movs	r1, #1
 e000ec0:	4f39      	ldr	r7, [pc, #228]	; (e000fa8 <data_flash_highspeed_setup+0x234>)
 e000ec2:	47b8      	blx	r7
 e000ec4:	f10d 0211 	add.w	r2, sp, #17
 e000ec8:	f895 0045 	ldrb.w	r0, [r5, #69]	; 0x45
 e000ecc:	2101      	movs	r1, #1
 e000ece:	47b8      	blx	r7
 e000ed0:	e7df      	b.n	e000e92 <data_flash_highspeed_setup+0x11e>
 e000ed2:	f10d 0215 	add.w	r2, sp, #21
 e000ed6:	2102      	movs	r1, #2
 e000ed8:	2015      	movs	r0, #21
 e000eda:	2703      	movs	r7, #3
 e000edc:	47c0      	blx	r8
 e000ede:	9b04      	ldr	r3, [sp, #16]
 e000ee0:	f443 3300 	orr.w	r3, r3, #131072	; 0x20000
 e000ee4:	9304      	str	r3, [sp, #16]
 e000ee6:	e7c1      	b.n	e000e6c <data_flash_highspeed_setup+0xf8>
 e000ee8:	4b31      	ldr	r3, [pc, #196]	; (e000fb0 <data_flash_highspeed_setup+0x23c>)
 e000eea:	4798      	blx	r3
 e000eec:	f8d9 0000 	ldr.w	r0, [r9]
 e000ef0:	68eb      	ldr	r3, [r5, #12]
 e000ef2:	e7ac      	b.n	e000e4e <data_flash_highspeed_setup+0xda>
 e000ef4:	4b2f      	ldr	r3, [pc, #188]	; (e000fb4 <data_flash_highspeed_setup+0x240>)
 e000ef6:	4798      	blx	r3
 e000ef8:	4b29      	ldr	r3, [pc, #164]	; (e000fa0 <data_flash_highspeed_setup+0x22c>)
 e000efa:	2201      	movs	r2, #1
 e000efc:	f8d9 0000 	ldr.w	r0, [r9]
 e000f00:	701a      	strb	r2, [r3, #0]
 e000f02:	68eb      	ldr	r3, [r5, #12]
 e000f04:	e7a3      	b.n	e000e4e <data_flash_highspeed_setup+0xda>
 e000f06:	4b2c      	ldr	r3, [pc, #176]	; (e000fb8 <data_flash_highspeed_setup+0x244>)
 e000f08:	4798      	blx	r3
 e000f0a:	f89d 3016 	ldrb.w	r3, [sp, #22]
 e000f0e:	f8d9 0000 	ldr.w	r0, [r9]
 e000f12:	2b15      	cmp	r3, #21
 e000f14:	d99a      	bls.n	e000e4c <data_flash_highspeed_setup+0xd8>
 e000f16:	2331      	movs	r3, #49	; 0x31
 e000f18:	f885 3045 	strb.w	r3, [r5, #69]	; 0x45
 e000f1c:	68eb      	ldr	r3, [r5, #12]
 e000f1e:	e796      	b.n	e000e4e <data_flash_highspeed_setup+0xda>
 e000f20:	4b24      	ldr	r3, [pc, #144]	; (e000fb4 <data_flash_highspeed_setup+0x240>)
 e000f22:	4798      	blx	r3
 e000f24:	f8d9 0000 	ldr.w	r0, [r9]
 e000f28:	68eb      	ldr	r3, [r5, #12]
 e000f2a:	e790      	b.n	e000e4e <data_flash_highspeed_setup+0xda>
 e000f2c:	4b21      	ldr	r3, [pc, #132]	; (e000fb4 <data_flash_highspeed_setup+0x240>)
 e000f2e:	4798      	blx	r3
 e000f30:	2300      	movs	r3, #0
 e000f32:	f8d9 2000 	ldr.w	r2, [r9]
 e000f36:	60eb      	str	r3, [r5, #12]
 e000f38:	9304      	str	r3, [sp, #16]
 e000f3a:	f8d2 900c 	ldr.w	r9, [r2, #12]
 e000f3e:	e789      	b.n	e000e54 <data_flash_highspeed_setup+0xe0>
 e000f40:	4b1e      	ldr	r3, [pc, #120]	; (e000fbc <data_flash_highspeed_setup+0x248>)
 e000f42:	2257      	movs	r2, #87	; 0x57
 e000f44:	4914      	ldr	r1, [pc, #80]	; (e000f98 <data_flash_highspeed_setup+0x224>)
 e000f46:	2003      	movs	r0, #3
 e000f48:	f001 fa78 	bl	e00243c <rtk_log_write>
 e000f4c:	481c      	ldr	r0, [pc, #112]	; (e000fc0 <data_flash_highspeed_setup+0x24c>)
 e000f4e:	f240 11fb 	movw	r1, #507	; 0x1fb
 e000f52:	f006 fa0d 	bl	e007370 <io_assert_failed>
 e000f56:	f8d9 0000 	ldr.w	r0, [r9]
 e000f5a:	e761      	b.n	e000e20 <data_flash_highspeed_setup+0xac>
 e000f5c:	f44f 7106 	mov.w	r1, #536	; 0x218
 e000f60:	4817      	ldr	r0, [pc, #92]	; (e000fc0 <data_flash_highspeed_setup+0x24c>)
 e000f62:	f006 fa05 	bl	e007370 <io_assert_failed>
 e000f66:	f8d9 3000 	ldr.w	r3, [r9]
 e000f6a:	691b      	ldr	r3, [r3, #16]
 e000f6c:	e76b      	b.n	e000e46 <data_flash_highspeed_setup+0xd2>
 e000f6e:	47b8      	blx	r7
 e000f70:	f8d9 0000 	ldr.w	r0, [r9]
 e000f74:	68eb      	ldr	r3, [r5, #12]
 e000f76:	e76a      	b.n	e000e4e <data_flash_highspeed_setup+0xda>
 e000f78:	2000b188 	.word	0x2000b188
 e000f7c:	00009271 	.word	0x00009271
 e000f80:	0e001239 	.word	0x0e001239
 e000f84:	0e007df0 	.word	0x0e007df0
 e000f88:	0c65d3ff 	.word	0x0c65d3ff
 e000f8c:	a530e8a5 	.word	0xa530e8a5
 e000f90:	0e007df2 	.word	0x0e007df2
 e000f94:	40127000 	.word	0x40127000
 e000f98:	0e007c24 	.word	0x0e007c24
 e000f9c:	0e007c94 	.word	0x0e007c94
 e000fa0:	2000b181 	.word	0x2000b181
 e000fa4:	00011bbd 	.word	0x00011bbd
 e000fa8:	0e001155 	.word	0x0e001155
 e000fac:	0000c219 	.word	0x0000c219
 e000fb0:	0e001221 	.word	0x0e001221
 e000fb4:	0e001229 	.word	0x0e001229
 e000fb8:	0e001231 	.word	0x0e001231
 e000fbc:	0e007ca8 	.word	0x0e007ca8
 e000fc0:	0e007ccc 	.word	0x0e007ccc
 e000fc4:	0e001051 	.word	0x0e001051
 e000fc8:	2000b184 	.word	0x2000b184

0e000fcc <DATA_FLASH_RxCmd_InUserMode>:
 e000fcc:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 e000fd0:	4c1b      	ldr	r4, [pc, #108]	; (e001040 <DATA_FLASH_RxCmd_InUserMode+0x74>)
 e000fd2:	4688      	mov	r8, r1
 e000fd4:	4617      	mov	r7, r2
 e000fd6:	4606      	mov	r6, r0
 e000fd8:	2200      	movs	r2, #0
 e000fda:	491a      	ldr	r1, [pc, #104]	; (e001044 <DATA_FLASH_RxCmd_InUserMode+0x78>)
 e000fdc:	4620      	mov	r0, r4
 e000fde:	4d1a      	ldr	r5, [pc, #104]	; (e001048 <DATA_FLASH_RxCmd_InUserMode+0x7c>)
 e000fe0:	47a8      	blx	r5
 e000fe2:	f8d4 3118 	ldr.w	r3, [r4, #280]	; 0x118
 e000fe6:	1e7a      	subs	r2, r7, #1
 e000fe8:	f423 2370 	bic.w	r3, r3, #983040	; 0xf0000
 e000fec:	eb02 0108 	add.w	r1, r2, r8
 e000ff0:	f8c4 3118 	str.w	r3, [r4, #280]	; 0x118
 e000ff4:	f028 437f 	bic.w	r3, r8, #4278190080	; 0xff000000
 e000ff8:	6063      	str	r3, [r4, #4]
 e000ffa:	2300      	movs	r3, #0
 e000ffc:	f8c4 3130 	str.w	r3, [r4, #304]	; 0x130
 e001000:	4b12      	ldr	r3, [pc, #72]	; (e00104c <DATA_FLASH_RxCmd_InUserMode+0x80>)
 e001002:	6820      	ldr	r0, [r4, #0]
 e001004:	4003      	ands	r3, r0
 e001006:	f443 7340 	orr.w	r3, r3, #768	; 0x300
 e00100a:	6023      	str	r3, [r4, #0]
 e00100c:	2301      	movs	r3, #1
 e00100e:	f884 6060 	strb.w	r6, [r4, #96]	; 0x60
 e001012:	60a3      	str	r3, [r4, #8]
 e001014:	e002      	b.n	e00101c <DATA_FLASH_RxCmd_InUserMode+0x50>
 e001016:	6aa3      	ldr	r3, [r4, #40]	; 0x28
 e001018:	0718      	lsls	r0, r3, #28
 e00101a:	d40c      	bmi.n	e001036 <DATA_FLASH_RxCmd_InUserMode+0x6a>
 e00101c:	428a      	cmp	r2, r1
 e00101e:	d1fa      	bne.n	e001016 <DATA_FLASH_RxCmd_InUserMode+0x4a>
 e001020:	4a07      	ldr	r2, [pc, #28]	; (e001040 <DATA_FLASH_RxCmd_InUserMode+0x74>)
 e001022:	6893      	ldr	r3, [r2, #8]
 e001024:	07db      	lsls	r3, r3, #31
 e001026:	d4fc      	bmi.n	e001022 <DATA_FLASH_RxCmd_InUserMode+0x56>
 e001028:	4906      	ldr	r1, [pc, #24]	; (e001044 <DATA_FLASH_RxCmd_InUserMode+0x78>)
 e00102a:	462b      	mov	r3, r5
 e00102c:	4804      	ldr	r0, [pc, #16]	; (e001040 <DATA_FLASH_RxCmd_InUserMode+0x74>)
 e00102e:	790a      	ldrb	r2, [r1, #4]
 e001030:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
 e001034:	4718      	bx	r3
 e001036:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
 e00103a:	f802 3f01 	strb.w	r3, [r2, #1]!
 e00103e:	e7ed      	b.n	e00101c <DATA_FLASH_RxCmd_InUserMode+0x50>
 e001040:	40127000 	.word	0x40127000
 e001044:	2000b188 	.word	0x2000b188
 e001048:	00009aa1 	.word	0x00009aa1
 e00104c:	fff0fcff 	.word	0xfff0fcff

0e001050 <DATA_FLASH_RxCmd>:
 e001050:	b510      	push	{r4, lr}
 e001052:	4c07      	ldr	r4, [pc, #28]	; (e001070 <DATA_FLASH_RxCmd+0x20>)
 e001054:	6823      	ldr	r3, [r4, #0]
 e001056:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
 e00105a:	6023      	str	r3, [r4, #0]
 e00105c:	6823      	ldr	r3, [r4, #0]
 e00105e:	2b00      	cmp	r3, #0
 e001060:	dafc      	bge.n	e00105c <DATA_FLASH_RxCmd+0xc>
 e001062:	f7ff ffb3 	bl	e000fcc <DATA_FLASH_RxCmd_InUserMode>
 e001066:	6823      	ldr	r3, [r4, #0]
 e001068:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
 e00106c:	6023      	str	r3, [r4, #0]
 e00106e:	bd10      	pop	{r4, pc}
 e001070:	40127000 	.word	0x40127000

0e001074 <DATA_FLASH_WaitBusy_InUserMode>:
 e001074:	2200      	movs	r2, #0
 e001076:	b570      	push	{r4, r5, r6, lr}
 e001078:	4613      	mov	r3, r2
 e00107a:	b082      	sub	sp, #8
 e00107c:	4e1d      	ldr	r6, [pc, #116]	; (e0010f4 <DATA_FLASH_WaitBusy_InUserMode+0x80>)
 e00107e:	4604      	mov	r4, r0
 e001080:	4d1d      	ldr	r5, [pc, #116]	; (e0010f8 <DATA_FLASH_WaitBusy_InUserMode+0x84>)
 e001082:	f88d 2007 	strb.w	r2, [sp, #7]
 e001086:	b934      	cbnz	r4, e001096 <DATA_FLASH_WaitBusy_InUserMode+0x22>
 e001088:	6aab      	ldr	r3, [r5, #40]	; 0x28
 e00108a:	f003 0301 	and.w	r3, r3, #1
 e00108e:	2b00      	cmp	r3, #0
 e001090:	d1f9      	bne.n	e001086 <DATA_FLASH_WaitBusy_InUserMode+0x12>
 e001092:	b002      	add	sp, #8
 e001094:	bd70      	pop	{r4, r5, r6, pc}
 e001096:	2c04      	cmp	r4, #4
 e001098:	d017      	beq.n	e0010ca <DATA_FLASH_WaitBusy_InUserMode+0x56>
 e00109a:	2c01      	cmp	r4, #1
 e00109c:	d01b      	beq.n	e0010d6 <DATA_FLASH_WaitBusy_InUserMode+0x62>
 e00109e:	2c03      	cmp	r4, #3
 e0010a0:	d1f5      	bne.n	e00108e <DATA_FLASH_WaitBusy_InUserMode+0x1a>
 e0010a2:	f10d 0207 	add.w	r2, sp, #7
 e0010a6:	2101      	movs	r1, #1
 e0010a8:	f896 0042 	ldrb.w	r0, [r6, #66]	; 0x42
 e0010ac:	f7ff ff8e 	bl	e000fcc <DATA_FLASH_RxCmd_InUserMode>
 e0010b0:	f89d 3007 	ldrb.w	r3, [sp, #7]
 e0010b4:	6972      	ldr	r2, [r6, #20]
 e0010b6:	4213      	tst	r3, r2
 e0010b8:	d0f3      	beq.n	e0010a2 <DATA_FLASH_WaitBusy_InUserMode+0x2e>
 e0010ba:	6932      	ldr	r2, [r6, #16]
 e0010bc:	4213      	tst	r3, r2
 e0010be:	bf14      	ite	ne
 e0010c0:	2301      	movne	r3, #1
 e0010c2:	2300      	moveq	r3, #0
 e0010c4:	2b00      	cmp	r3, #0
 e0010c6:	d1de      	bne.n	e001086 <DATA_FLASH_WaitBusy_InUserMode+0x12>
 e0010c8:	e7e3      	b.n	e001092 <DATA_FLASH_WaitBusy_InUserMode+0x1e>
 e0010ca:	68ab      	ldr	r3, [r5, #8]
 e0010cc:	f003 0301 	and.w	r3, r3, #1
 e0010d0:	2b00      	cmp	r3, #0
 e0010d2:	d1d8      	bne.n	e001086 <DATA_FLASH_WaitBusy_InUserMode+0x12>
 e0010d4:	e7dd      	b.n	e001092 <DATA_FLASH_WaitBusy_InUserMode+0x1e>
 e0010d6:	f10d 0207 	add.w	r2, sp, #7
 e0010da:	4621      	mov	r1, r4
 e0010dc:	f896 0042 	ldrb.w	r0, [r6, #66]	; 0x42
 e0010e0:	f7ff ff74 	bl	e000fcc <DATA_FLASH_RxCmd_InUserMode>
 e0010e4:	f89d 3007 	ldrb.w	r3, [sp, #7]
 e0010e8:	6932      	ldr	r2, [r6, #16]
 e0010ea:	4013      	ands	r3, r2
 e0010ec:	2b00      	cmp	r3, #0
 e0010ee:	d1ca      	bne.n	e001086 <DATA_FLASH_WaitBusy_InUserMode+0x12>
 e0010f0:	e7cf      	b.n	e001092 <DATA_FLASH_WaitBusy_InUserMode+0x1e>
 e0010f2:	bf00      	nop
 e0010f4:	2000b188 	.word	0x2000b188
 e0010f8:	40127000 	.word	0x40127000

0e0010fc <DATA_FLASH_TxCmd_InUserMode>:
 e0010fc:	4b13      	ldr	r3, [pc, #76]	; (e00114c <DATA_FLASH_TxCmd_InUserMode+0x50>)
 e0010fe:	b4f0      	push	{r4, r5, r6, r7}
 e001100:	4c13      	ldr	r4, [pc, #76]	; (e001150 <DATA_FLASH_TxCmd_InUserMode+0x54>)
 e001102:	f8d3 5118 	ldr.w	r5, [r3, #280]	; 0x118
 e001106:	681e      	ldr	r6, [r3, #0]
 e001108:	681f      	ldr	r7, [r3, #0]
 e00110a:	403c      	ands	r4, r7
 e00110c:	601c      	str	r4, [r3, #0]
 e00110e:	f8d3 4118 	ldr.w	r4, [r3, #280]	; 0x118
 e001112:	f424 2470 	bic.w	r4, r4, #983040	; 0xf0000
 e001116:	f8c3 4118 	str.w	r4, [r3, #280]	; 0x118
 e00111a:	2400      	movs	r4, #0
 e00111c:	605c      	str	r4, [r3, #4]
 e00111e:	f8c3 1130 	str.w	r1, [r3, #304]	; 0x130
 e001122:	f883 0060 	strb.w	r0, [r3, #96]	; 0x60
 e001126:	b131      	cbz	r1, e001136 <DATA_FLASH_TxCmd_InUserMode+0x3a>
 e001128:	1850      	adds	r0, r2, r1
 e00112a:	f812 1b01 	ldrb.w	r1, [r2], #1
 e00112e:	4282      	cmp	r2, r0
 e001130:	f883 1060 	strb.w	r1, [r3, #96]	; 0x60
 e001134:	d1f9      	bne.n	e00112a <DATA_FLASH_TxCmd_InUserMode+0x2e>
 e001136:	4a05      	ldr	r2, [pc, #20]	; (e00114c <DATA_FLASH_TxCmd_InUserMode+0x50>)
 e001138:	2301      	movs	r3, #1
 e00113a:	6093      	str	r3, [r2, #8]
 e00113c:	6893      	ldr	r3, [r2, #8]
 e00113e:	07db      	lsls	r3, r3, #31
 e001140:	d4fc      	bmi.n	e00113c <DATA_FLASH_TxCmd_InUserMode+0x40>
 e001142:	f8c2 5118 	str.w	r5, [r2, #280]	; 0x118
 e001146:	6016      	str	r6, [r2, #0]
 e001148:	bcf0      	pop	{r4, r5, r6, r7}
 e00114a:	4770      	bx	lr
 e00114c:	40127000 	.word	0x40127000
 e001150:	ffc0fcff 	.word	0xffc0fcff

0e001154 <DATA_FLASH_SetStatus>:
 e001154:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
 e001158:	4c2c      	ldr	r4, [pc, #176]	; (e00120c <DATA_FLASH_SetStatus+0xb8>)
 e00115a:	b083      	sub	sp, #12
 e00115c:	4606      	mov	r6, r0
 e00115e:	460f      	mov	r7, r1
 e001160:	6823      	ldr	r3, [r4, #0]
 e001162:	4690      	mov	r8, r2
 e001164:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
 e001168:	6023      	str	r3, [r4, #0]
 e00116a:	6823      	ldr	r3, [r4, #0]
 e00116c:	2b00      	cmp	r3, #0
 e00116e:	dafc      	bge.n	e00116a <DATA_FLASH_SetStatus+0x16>
 e001170:	2300      	movs	r3, #0
 e001172:	4c27      	ldr	r4, [pc, #156]	; (e001210 <DATA_FLASH_SetStatus+0xbc>)
 e001174:	f88d 3007 	strb.w	r3, [sp, #7]
 e001178:	f10d 0207 	add.w	r2, sp, #7
 e00117c:	2101      	movs	r1, #1
 e00117e:	f894 0042 	ldrb.w	r0, [r4, #66]	; 0x42
 e001182:	f7ff ff23 	bl	e000fcc <DATA_FLASH_RxCmd_InUserMode>
 e001186:	f89d 3007 	ldrb.w	r3, [sp, #7]
 e00118a:	6922      	ldr	r2, [r4, #16]
 e00118c:	4013      	ands	r3, r2
 e00118e:	d1f3      	bne.n	e001178 <DATA_FLASH_SetStatus+0x24>
 e001190:	4a1e      	ldr	r2, [pc, #120]	; (e00120c <DATA_FLASH_SetStatus+0xb8>)
 e001192:	2501      	movs	r5, #1
 e001194:	491f      	ldr	r1, [pc, #124]	; (e001214 <DATA_FLASH_SetStatus+0xc0>)
 e001196:	f8d2 c118 	ldr.w	ip, [r2, #280]	; 0x118
 e00119a:	6810      	ldr	r0, [r2, #0]
 e00119c:	f8d2 9000 	ldr.w	r9, [r2]
 e0011a0:	f894 e040 	ldrb.w	lr, [r4, #64]	; 0x40
 e0011a4:	ea09 0101 	and.w	r1, r9, r1
 e0011a8:	6011      	str	r1, [r2, #0]
 e0011aa:	f8d2 1118 	ldr.w	r1, [r2, #280]	; 0x118
 e0011ae:	f421 2170 	bic.w	r1, r1, #983040	; 0xf0000
 e0011b2:	f8c2 1118 	str.w	r1, [r2, #280]	; 0x118
 e0011b6:	6053      	str	r3, [r2, #4]
 e0011b8:	f8c2 3130 	str.w	r3, [r2, #304]	; 0x130
 e0011bc:	f882 e060 	strb.w	lr, [r2, #96]	; 0x60
 e0011c0:	6095      	str	r5, [r2, #8]
 e0011c2:	6895      	ldr	r5, [r2, #8]
 e0011c4:	f015 0501 	ands.w	r5, r5, #1
 e0011c8:	d1fb      	bne.n	e0011c2 <DATA_FLASH_SetStatus+0x6e>
 e0011ca:	f8c2 c118 	str.w	ip, [r2, #280]	; 0x118
 e0011ce:	4b12      	ldr	r3, [pc, #72]	; (e001218 <DATA_FLASH_SetStatus+0xc4>)
 e0011d0:	6010      	str	r0, [r2, #0]
 e0011d2:	2003      	movs	r0, #3
 e0011d4:	4798      	blx	r3
 e0011d6:	4642      	mov	r2, r8
 e0011d8:	b2f9      	uxtb	r1, r7
 e0011da:	4630      	mov	r0, r6
 e0011dc:	4b0f      	ldr	r3, [pc, #60]	; (e00121c <DATA_FLASH_SetStatus+0xc8>)
 e0011de:	4798      	blx	r3
 e0011e0:	f88d 5007 	strb.w	r5, [sp, #7]
 e0011e4:	f10d 0207 	add.w	r2, sp, #7
 e0011e8:	2101      	movs	r1, #1
 e0011ea:	f894 0042 	ldrb.w	r0, [r4, #66]	; 0x42
 e0011ee:	f7ff feed 	bl	e000fcc <DATA_FLASH_RxCmd_InUserMode>
 e0011f2:	f89d 2007 	ldrb.w	r2, [sp, #7]
 e0011f6:	6923      	ldr	r3, [r4, #16]
 e0011f8:	421a      	tst	r2, r3
 e0011fa:	d1f3      	bne.n	e0011e4 <DATA_FLASH_SetStatus+0x90>
 e0011fc:	4a03      	ldr	r2, [pc, #12]	; (e00120c <DATA_FLASH_SetStatus+0xb8>)
 e0011fe:	6813      	ldr	r3, [r2, #0]
 e001200:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
 e001204:	6013      	str	r3, [r2, #0]
 e001206:	b003      	add	sp, #12
 e001208:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 e00120c:	40127000 	.word	0x40127000
 e001210:	2000b188 	.word	0x2000b188
 e001214:	ffc0fcff 	.word	0xffc0fcff
 e001218:	0e001075 	.word	0x0e001075
 e00121c:	0e0010fd 	.word	0x0e0010fd

0e001220 <DATA_FLASH_StructInit_Micron>:
 e001220:	4b00      	ldr	r3, [pc, #0]	; (e001224 <DATA_FLASH_StructInit_Micron+0x4>)
 e001222:	4718      	bx	r3
 e001224:	0000a201 	.word	0x0000a201

0e001228 <DATA_FLASH_StructInit_MXIC>:
 e001228:	4b00      	ldr	r3, [pc, #0]	; (e00122c <DATA_FLASH_StructInit_MXIC+0x4>)
 e00122a:	4718      	bx	r3
 e00122c:	0000a26d 	.word	0x0000a26d

0e001230 <DATA_FLASH_StructInit_GD>:
 e001230:	4b00      	ldr	r3, [pc, #0]	; (e001234 <DATA_FLASH_StructInit_GD+0x4>)
 e001232:	4718      	bx	r3
 e001234:	0000a2d9 	.word	0x0000a2d9

0e001238 <DATA_FLASH_StructInit>:
 e001238:	4b00      	ldr	r3, [pc, #0]	; (e00123c <DATA_FLASH_StructInit+0x4>)
 e00123a:	4718      	bx	r3
 e00123c:	0000a34d 	.word	0x0000a34d

0e001240 <DATA_FLASH_Init>:
 e001240:	b510      	push	{r4, lr}
 e001242:	4906      	ldr	r1, [pc, #24]	; (e00125c <DATA_FLASH_Init+0x1c>)
 e001244:	4604      	mov	r4, r0
 e001246:	4b06      	ldr	r3, [pc, #24]	; (e001260 <DATA_FLASH_Init+0x20>)
 e001248:	4806      	ldr	r0, [pc, #24]	; (e001264 <DATA_FLASH_Init+0x24>)
 e00124a:	4798      	blx	r3
 e00124c:	4903      	ldr	r1, [pc, #12]	; (e00125c <DATA_FLASH_Init+0x1c>)
 e00124e:	4805      	ldr	r0, [pc, #20]	; (e001264 <DATA_FLASH_Init+0x24>)
 e001250:	4622      	mov	r2, r4
 e001252:	4b05      	ldr	r3, [pc, #20]	; (e001268 <DATA_FLASH_Init+0x28>)
 e001254:	710c      	strb	r4, [r1, #4]
 e001256:	4798      	blx	r3
 e001258:	2001      	movs	r0, #1
 e00125a:	bd10      	pop	{r4, pc}
 e00125c:	2000b188 	.word	0x2000b188
 e001260:	00009a0d 	.word	0x00009a0d
 e001264:	40127000 	.word	0x40127000
 e001268:	00009aa1 	.word	0x00009aa1

0e00126c <HANDLER_MemFault>:
 e00126c:	f3ef 8008 	mrs	r0, MSP
 e001270:	f3ef 8109 	mrs	r1, PSP
 e001274:	4672      	mov	r2, lr
 e001276:	f04f 0303 	mov.w	r3, #3
 e00127a:	e92d 0ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
 e00127e:	f000 b825 	b.w	e0012cc <Fault_Handler>
 e001282:	4770      	bx	lr

0e001284 <HANDLER_BusFault>:
 e001284:	f3ef 8008 	mrs	r0, MSP
 e001288:	f3ef 8109 	mrs	r1, PSP
 e00128c:	4672      	mov	r2, lr
 e00128e:	f04f 0302 	mov.w	r3, #2
 e001292:	e92d 0ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
 e001296:	f000 b819 	b.w	e0012cc <Fault_Handler>
 e00129a:	4770      	bx	lr

0e00129c <HANDLER_UsageFault>:
 e00129c:	f3ef 8008 	mrs	r0, MSP
 e0012a0:	f3ef 8109 	mrs	r1, PSP
 e0012a4:	4672      	mov	r2, lr
 e0012a6:	f04f 0301 	mov.w	r3, #1
 e0012aa:	e92d 0ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
 e0012ae:	f000 b80d 	b.w	e0012cc <Fault_Handler>
 e0012b2:	4770      	bx	lr

0e0012b4 <HANDLER_HardFault>:
 e0012b4:	f3ef 8008 	mrs	r0, MSP
 e0012b8:	f3ef 8109 	mrs	r1, PSP
 e0012bc:	4672      	mov	r2, lr
 e0012be:	f04f 0300 	mov.w	r3, #0
 e0012c2:	e92d 0ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
 e0012c6:	f000 b801 	b.w	e0012cc <Fault_Handler>
 e0012ca:	4770      	bx	lr

0e0012cc <Fault_Handler>:
 e0012cc:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
 e0012d0:	b095      	sub	sp, #84	; 0x54
 e0012d2:	4614      	mov	r4, r2
 e0012d4:	4605      	mov	r5, r0
 e0012d6:	2244      	movs	r2, #68	; 0x44
 e0012d8:	460e      	mov	r6, r1
 e0012da:	a803      	add	r0, sp, #12
 e0012dc:	2100      	movs	r1, #0
 e0012de:	461f      	mov	r7, r3
 e0012e0:	f006 fb92 	bl	e007a08 <____wrap_memset_veneer>
 e0012e4:	07e2      	lsls	r2, r4, #31
 e0012e6:	f004 0340 	and.w	r3, r4, #64	; 0x40
 e0012ea:	d554      	bpl.n	e001396 <Fault_Handler+0xca>
 e0012ec:	2b00      	cmp	r3, #0
 e0012ee:	d05b      	beq.n	e0013a8 <Fault_Handler+0xdc>
 e0012f0:	f004 000c 	and.w	r0, r4, #12
 e0012f4:	f04f 0800 	mov.w	r8, #0
 e0012f8:	f1a0 000c 	sub.w	r0, r0, #12
 e0012fc:	fab0 f080 	clz	r0, r0
 e001300:	0940      	lsrs	r0, r0, #5
 e001302:	f1a5 0c24 	sub.w	ip, r5, #36	; 0x24
 e001306:	ab04      	add	r3, sp, #16
 e001308:	1f29      	subs	r1, r5, #4
 e00130a:	9403      	str	r4, [sp, #12]
 e00130c:	f85c 2f04 	ldr.w	r2, [ip, #4]!
 e001310:	458c      	cmp	ip, r1
 e001312:	f843 2b04 	str.w	r2, [r3], #4
 e001316:	d1f9      	bne.n	e00130c <Fault_Handler+0x40>
 e001318:	2800      	cmp	r0, #0
 e00131a:	bf14      	ite	ne
 e00131c:	4631      	movne	r1, r6
 e00131e:	4629      	moveq	r1, r5
 e001320:	f10d 0e30 	add.w	lr, sp, #48	; 0x30
 e001324:	f1a1 0c04 	sub.w	ip, r1, #4
 e001328:	f101 021c 	add.w	r2, r1, #28
 e00132c:	f85c 3f04 	ldr.w	r3, [ip, #4]!
 e001330:	4594      	cmp	ip, r2
 e001332:	f84e 3b04 	str.w	r3, [lr], #4
 e001336:	d1f9      	bne.n	e00132c <Fault_Handler+0x60>
 e001338:	aa03      	add	r2, sp, #12
 e00133a:	6bc8      	ldr	r0, [r1, #60]	; 0x3c
 e00133c:	f004 f8ee 	bl	e00551c <crash_dump>
 e001340:	2f04      	cmp	r7, #4
 e001342:	d042      	beq.n	e0013ca <Fault_Handler+0xfe>
 e001344:	f1b8 0f00 	cmp.w	r8, #0
 e001348:	d136      	bne.n	e0013b8 <Fault_Handler+0xec>
 e00134a:	f8df 80c8 	ldr.w	r8, [pc, #200]	; e001414 <Fault_Handler+0x148>
 e00134e:	2241      	movs	r2, #65	; 0x41
 e001350:	4b28      	ldr	r3, [pc, #160]	; (e0013f4 <Fault_Handler+0x128>)
 e001352:	f8d8 0024 	ldr.w	r0, [r8, #36]	; 0x24
 e001356:	4928      	ldr	r1, [pc, #160]	; (e0013f8 <Fault_Handler+0x12c>)
 e001358:	9000      	str	r0, [sp, #0]
 e00135a:	2001      	movs	r0, #1
 e00135c:	f001 f86e 	bl	e00243c <rtk_log_write>
 e001360:	f8d8 300c 	ldr.w	r3, [r8, #12]
 e001364:	2241      	movs	r2, #65	; 0x41
 e001366:	4924      	ldr	r1, [pc, #144]	; (e0013f8 <Fault_Handler+0x12c>)
 e001368:	9300      	str	r3, [sp, #0]
 e00136a:	2001      	movs	r0, #1
 e00136c:	4b23      	ldr	r3, [pc, #140]	; (e0013fc <Fault_Handler+0x130>)
 e00136e:	f001 f865 	bl	e00243c <rtk_log_write>
 e001372:	f3ef 8314 	mrs	r3, CONTROL
 e001376:	2241      	movs	r2, #65	; 0x41
 e001378:	9300      	str	r3, [sp, #0]
 e00137a:	491f      	ldr	r1, [pc, #124]	; (e0013f8 <Fault_Handler+0x12c>)
 e00137c:	2001      	movs	r0, #1
 e00137e:	4b20      	ldr	r3, [pc, #128]	; (e001400 <Fault_Handler+0x134>)
 e001380:	f001 f85c 	bl	e00243c <rtk_log_write>
 e001384:	463b      	mov	r3, r7
 e001386:	4622      	mov	r2, r4
 e001388:	4631      	mov	r1, r6
 e00138a:	4628      	mov	r0, r5
 e00138c:	f006 fb38 	bl	e007a00 <__INT_HardFault_C_veneer>
 e001390:	b015      	add	sp, #84	; 0x54
 e001392:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 e001396:	2b00      	cmp	r3, #0
 e001398:	d0aa      	beq.n	e0012f0 <Fault_Handler+0x24>
 e00139a:	4b1a      	ldr	r3, [pc, #104]	; (e001404 <Fault_Handler+0x138>)
 e00139c:	2241      	movs	r2, #65	; 0x41
 e00139e:	4916      	ldr	r1, [pc, #88]	; (e0013f8 <Fault_Handler+0x12c>)
 e0013a0:	2001      	movs	r0, #1
 e0013a2:	f001 f84b 	bl	e00243c <rtk_log_write>
 e0013a6:	e7fe      	b.n	e0013a6 <Fault_Handler+0xda>
 e0013a8:	f3ef 8894 	mrs	r8, CONTROL_NS
 e0013ac:	f014 0008 	ands.w	r0, r4, #8
 e0013b0:	d0a7      	beq.n	e001302 <Fault_Handler+0x36>
 e0013b2:	f3c8 0040 	ubfx	r0, r8, #1, #1
 e0013b6:	e7a4      	b.n	e001302 <Fault_Handler+0x36>
 e0013b8:	4b13      	ldr	r3, [pc, #76]	; (e001408 <Fault_Handler+0x13c>)
 e0013ba:	2241      	movs	r2, #65	; 0x41
 e0013bc:	490e      	ldr	r1, [pc, #56]	; (e0013f8 <Fault_Handler+0x12c>)
 e0013be:	2001      	movs	r0, #1
 e0013c0:	f8cd 8000 	str.w	r8, [sp]
 e0013c4:	f001 f83a 	bl	e00243c <rtk_log_write>
 e0013c8:	e7bf      	b.n	e00134a <Fault_Handler+0x7e>
 e0013ca:	f8df 9048 	ldr.w	r9, [pc, #72]	; e001414 <Fault_Handler+0x148>
 e0013ce:	2241      	movs	r2, #65	; 0x41
 e0013d0:	4b0e      	ldr	r3, [pc, #56]	; (e00140c <Fault_Handler+0x140>)
 e0013d2:	f8d9 00e4 	ldr.w	r0, [r9, #228]	; 0xe4
 e0013d6:	4908      	ldr	r1, [pc, #32]	; (e0013f8 <Fault_Handler+0x12c>)
 e0013d8:	9000      	str	r0, [sp, #0]
 e0013da:	2001      	movs	r0, #1
 e0013dc:	f001 f82e 	bl	e00243c <rtk_log_write>
 e0013e0:	f8d9 30e8 	ldr.w	r3, [r9, #232]	; 0xe8
 e0013e4:	2241      	movs	r2, #65	; 0x41
 e0013e6:	4904      	ldr	r1, [pc, #16]	; (e0013f8 <Fault_Handler+0x12c>)
 e0013e8:	9300      	str	r3, [sp, #0]
 e0013ea:	2001      	movs	r0, #1
 e0013ec:	4b08      	ldr	r3, [pc, #32]	; (e001410 <Fault_Handler+0x144>)
 e0013ee:	f001 f825 	bl	e00243c <rtk_log_write>
 e0013f2:	e7a7      	b.n	e001344 <Fault_Handler+0x78>
 e0013f4:	0e007db4 	.word	0x0e007db4
 e0013f8:	0e007d6c 	.word	0x0e007d6c
 e0013fc:	0e007dc8 	.word	0x0e007dc8
 e001400:	0e007ddc 	.word	0x0e007ddc
 e001404:	0e007d20 	.word	0x0e007d20
 e001408:	0e007d9c 	.word	0x0e007d9c
 e00140c:	0e007d74 	.word	0x0e007d74
 e001410:	0e007d88 	.word	0x0e007d88
 e001414:	e000ed00 	.word	0xe000ed00

0e001418 <Fault_Hanlder_Redirect>:
 e001418:	4b07      	ldr	r3, [pc, #28]	; (e001438 <Fault_Hanlder_Redirect+0x20>)
 e00141a:	4908      	ldr	r1, [pc, #32]	; (e00143c <Fault_Hanlder_Redirect+0x24>)
 e00141c:	4a08      	ldr	r2, [pc, #32]	; (e001440 <Fault_Hanlder_Redirect+0x28>)
 e00141e:	b410      	push	{r4}
 e001420:	e9c3 1203 	strd	r1, r2, [r3, #12]
 e001424:	4c07      	ldr	r4, [pc, #28]	; (e001444 <Fault_Hanlder_Redirect+0x2c>)
 e001426:	4908      	ldr	r1, [pc, #32]	; (e001448 <Fault_Hanlder_Redirect+0x30>)
 e001428:	4a08      	ldr	r2, [pc, #32]	; (e00144c <Fault_Hanlder_Redirect+0x34>)
 e00142a:	e9c3 4105 	strd	r4, r1, [r3, #20]
 e00142e:	6010      	str	r0, [r2, #0]
 e001430:	f85d 4b04 	ldr.w	r4, [sp], #4
 e001434:	4770      	bx	lr
 e001436:	bf00      	nop
 e001438:	30007000 	.word	0x30007000
 e00143c:	0e0012b5 	.word	0x0e0012b5
 e001440:	0e00126d 	.word	0x0e00126d
 e001444:	0e001285 	.word	0x0e001285
 e001448:	0e00129d 	.word	0x0e00129d
 e00144c:	2000bd90 	.word	0x2000bd90

0e001450 <flash_init_userdef>:
 e001450:	4b15      	ldr	r3, [pc, #84]	; (e0014a8 <flash_init_userdef+0x58>)
 e001452:	2201      	movs	r2, #1
 e001454:	2140      	movs	r1, #64	; 0x40
 e001456:	f44f 6000 	mov.w	r0, #2048	; 0x800
 e00145a:	b430      	push	{r4, r5}
 e00145c:	e9c3 1203 	strd	r1, r2, [r3, #12]
 e001460:	2502      	movs	r5, #2
 e001462:	2400      	movs	r4, #0
 e001464:	f640 0104 	movw	r1, #2052	; 0x804
 e001468:	601a      	str	r2, [r3, #0]
 e00146a:	22bb      	movs	r2, #187	; 0xbb
 e00146c:	f8a3 004e 	strh.w	r0, [r3, #78]	; 0x4e
 e001470:	f8a3 1050 	strh.w	r1, [r3, #80]	; 0x50
 e001474:	203b      	movs	r0, #59	; 0x3b
 e001476:	21eb      	movs	r1, #235	; 0xeb
 e001478:	621a      	str	r2, [r3, #32]
 e00147a:	4a0c      	ldr	r2, [pc, #48]	; (e0014ac <flash_init_userdef+0x5c>)
 e00147c:	61d8      	str	r0, [r3, #28]
 e00147e:	6299      	str	r1, [r3, #40]	; 0x28
 e001480:	f64a 3120 	movw	r1, #43808	; 0xab20
 e001484:	641a      	str	r2, [r3, #64]	; 0x40
 e001486:	22b9      	movs	r2, #185	; 0xb9
 e001488:	4809      	ldr	r0, [pc, #36]	; (e0014b0 <flash_init_userdef+0x60>)
 e00148a:	f8a3 1048 	strh.w	r1, [r3, #72]	; 0x48
 e00148e:	6458      	str	r0, [r3, #68]	; 0x44
 e001490:	f883 204a 	strb.w	r2, [r3, #74]	; 0x4a
 e001494:	e9c3 5405 	strd	r5, r4, [r3, #20]
 e001498:	2406      	movs	r4, #6
 e00149a:	f883 4052 	strb.w	r4, [r3, #82]	; 0x52
 e00149e:	246b      	movs	r4, #107	; 0x6b
 e0014a0:	625c      	str	r4, [r3, #36]	; 0x24
 e0014a2:	bc30      	pop	{r4, r5}
 e0014a4:	4770      	bx	lr
 e0014a6:	bf00      	nop
 e0014a8:	20004014 	.word	0x20004014
 e0014ac:	00059f06 	.word	0x00059f06
 e0014b0:	d8600001 	.word	0xd8600001

0e0014b4 <flash_get_readmode>:
 e0014b4:	f647 73ff 	movw	r3, #32767	; 0x7fff
 e0014b8:	4298      	cmp	r0, r3
 e0014ba:	d016      	beq.n	e0014ea <flash_get_readmode+0x36>
 e0014bc:	f5b0 4f00 	cmp.w	r0, #32768	; 0x8000
 e0014c0:	d20a      	bcs.n	e0014d8 <flash_get_readmode+0x24>
 e0014c2:	f641 73ff 	movw	r3, #8191	; 0x1fff
 e0014c6:	4298      	cmp	r0, r3
 e0014c8:	d00d      	beq.n	e0014e6 <flash_get_readmode+0x32>
 e0014ca:	f643 73ff 	movw	r3, #16383	; 0x3fff
 e0014ce:	4298      	cmp	r0, r3
 e0014d0:	bf0c      	ite	eq
 e0014d2:	2002      	moveq	r0, #2
 e0014d4:	2000      	movne	r0, #0
 e0014d6:	4770      	bx	lr
 e0014d8:	f64f 73ff 	movw	r3, #65535	; 0xffff
 e0014dc:	4298      	cmp	r0, r3
 e0014de:	bf0c      	ite	eq
 e0014e0:	2004      	moveq	r0, #4
 e0014e2:	2000      	movne	r0, #0
 e0014e4:	4770      	bx	lr
 e0014e6:	2001      	movs	r0, #1
 e0014e8:	4770      	bx	lr
 e0014ea:	2003      	movs	r0, #3
 e0014ec:	4770      	bx	lr
 e0014ee:	bf00      	nop

0e0014f0 <flash_get_chip_info>:
 e0014f0:	4b09      	ldr	r3, [pc, #36]	; (e001518 <flash_get_chip_info+0x28>)
 e0014f2:	21ef      	movs	r1, #239	; 0xef
 e0014f4:	22ff      	movs	r2, #255	; 0xff
 e0014f6:	e005      	b.n	e001504 <flash_get_chip_info+0x14>
 e0014f8:	7f1a      	ldrb	r2, [r3, #28]
 e0014fa:	2aff      	cmp	r2, #255	; 0xff
 e0014fc:	d008      	beq.n	e001510 <flash_get_chip_info+0x20>
 e0014fe:	699a      	ldr	r2, [r3, #24]
 e001500:	f853 1f14 	ldr.w	r1, [r3, #20]!
 e001504:	4002      	ands	r2, r0
 e001506:	469c      	mov	ip, r3
 e001508:	428a      	cmp	r2, r1
 e00150a:	d1f5      	bne.n	e0014f8 <flash_get_chip_info+0x8>
 e00150c:	4660      	mov	r0, ip
 e00150e:	4770      	bx	lr
 e001510:	f04f 0c00 	mov.w	ip, #0
 e001514:	4660      	mov	r0, ip
 e001516:	4770      	bx	lr
 e001518:	0e007df4 	.word	0x0e007df4

0e00151c <flash_get_layout_info>:
 e00151c:	b510      	push	{r4, lr}
 e00151e:	f04f 0c00 	mov.w	ip, #0
 e001522:	4c0e      	ldr	r4, [pc, #56]	; (e00155c <flash_get_layout_info+0x40>)
 e001524:	4663      	mov	r3, ip
 e001526:	46a6      	mov	lr, r4
 e001528:	e005      	b.n	e001536 <flash_get_layout_info+0x1a>
 e00152a:	f85e 3f0c 	ldr.w	r3, [lr, #12]!
 e00152e:	f10c 0c01 	add.w	ip, ip, #1
 e001532:	2bff      	cmp	r3, #255	; 0xff
 e001534:	d010      	beq.n	e001558 <flash_get_layout_info+0x3c>
 e001536:	4298      	cmp	r0, r3
 e001538:	d1f7      	bne.n	e00152a <flash_get_layout_info+0xe>
 e00153a:	b129      	cbz	r1, e001548 <flash_get_layout_info+0x2c>
 e00153c:	eb0c 034c 	add.w	r3, ip, ip, lsl #1
 e001540:	eb04 0383 	add.w	r3, r4, r3, lsl #2
 e001544:	685b      	ldr	r3, [r3, #4]
 e001546:	600b      	str	r3, [r1, #0]
 e001548:	b132      	cbz	r2, e001558 <flash_get_layout_info+0x3c>
 e00154a:	eb0c 0c4c 	add.w	ip, ip, ip, lsl #1
 e00154e:	eb04 0c8c 	add.w	ip, r4, ip, lsl #2
 e001552:	f8dc 3008 	ldr.w	r3, [ip, #8]
 e001556:	6013      	str	r3, [r2, #0]
 e001558:	bd10      	pop	{r4, pc}
 e00155a:	bf00      	nop
 e00155c:	0e007f20 	.word	0x0e007f20

0e001560 <Flash_Write_Lock_IPC>:
 e001560:	b510      	push	{r4, lr}
 e001562:	4b15      	ldr	r3, [pc, #84]	; (e0015b8 <Flash_Write_Lock_IPC+0x58>)
 e001564:	b084      	sub	sp, #16
 e001566:	4c15      	ldr	r4, [pc, #84]	; (e0015bc <Flash_Write_Lock_IPC+0x5c>)
 e001568:	7020      	strb	r0, [r4, #0]
 e00156a:	695a      	ldr	r2, [r3, #20]
 e00156c:	03d2      	lsls	r2, r2, #15
 e00156e:	d507      	bpl.n	e001580 <Flash_Write_Lock_IPC+0x20>
 e001570:	f3bf 8f4f 	dsb	sy
 e001574:	f8c3 4268 	str.w	r4, [r3, #616]	; 0x268
 e001578:	f3bf 8f4f 	dsb	sy
 e00157c:	f3bf 8f6f 	isb	sy
 e001580:	2300      	movs	r3, #0
 e001582:	2001      	movs	r0, #1
 e001584:	466a      	mov	r2, sp
 e001586:	2104      	movs	r1, #4
 e001588:	e9cd 3400 	strd	r3, r4, [sp]
 e00158c:	e9cd 0302 	strd	r0, r3, [sp, #8]
 e001590:	f000 fa10 	bl	e0019b4 <ipc_send_message>
 e001594:	4a08      	ldr	r2, [pc, #32]	; (e0015b8 <Flash_Write_Lock_IPC+0x58>)
 e001596:	6953      	ldr	r3, [r2, #20]
 e001598:	03db      	lsls	r3, r3, #15
 e00159a:	d507      	bpl.n	e0015ac <Flash_Write_Lock_IPC+0x4c>
 e00159c:	f3bf 8f4f 	dsb	sy
 e0015a0:	f8c2 425c 	str.w	r4, [r2, #604]	; 0x25c
 e0015a4:	f3bf 8f4f 	dsb	sy
 e0015a8:	f3bf 8f6f 	isb	sy
 e0015ac:	7823      	ldrb	r3, [r4, #0]
 e0015ae:	2b00      	cmp	r3, #0
 e0015b0:	d1f1      	bne.n	e001596 <Flash_Write_Lock_IPC+0x36>
 e0015b2:	b004      	add	sp, #16
 e0015b4:	bd10      	pop	{r4, pc}
 e0015b6:	bf00      	nop
 e0015b8:	e000ed00 	.word	0xe000ed00
 e0015bc:	2000b1e0 	.word	0x2000b1e0

0e0015c0 <FLASH_Write_Lock>:
 e0015c0:	b538      	push	{r3, r4, r5, lr}
 e0015c2:	4d0d      	ldr	r5, [pc, #52]	; (e0015f8 <FLASH_Write_Lock+0x38>)
 e0015c4:	4c0d      	ldr	r4, [pc, #52]	; (e0015fc <FLASH_Write_Lock+0x3c>)
 e0015c6:	f001 fc29 	bl	e002e1c <rtos_sched_suspend>
 e0015ca:	e002      	b.n	e0015d2 <FLASH_Write_Lock+0x12>
 e0015cc:	2002      	movs	r0, #2
 e0015ce:	f000 ff35 	bl	e00243c <rtk_log_write>
 e0015d2:	f44f 717a 	mov.w	r1, #1000	; 0x3e8
 e0015d6:	2002      	movs	r0, #2
 e0015d8:	f005 fed4 	bl	e007384 <IPC_SEMTake>
 e0015dc:	2801      	cmp	r0, #1
 e0015de:	462b      	mov	r3, r5
 e0015e0:	f04f 0245 	mov.w	r2, #69	; 0x45
 e0015e4:	4621      	mov	r1, r4
 e0015e6:	d1f1      	bne.n	e0015cc <FLASH_Write_Lock+0xc>
 e0015e8:	f7ff ffba 	bl	e001560 <Flash_Write_Lock_IPC>
 e0015ec:	f7ff f896 	bl	e00071c <irq_disable_save>
 e0015f0:	4b03      	ldr	r3, [pc, #12]	; (e001600 <FLASH_Write_Lock+0x40>)
 e0015f2:	6018      	str	r0, [r3, #0]
 e0015f4:	bd38      	pop	{r3, r4, r5, pc}
 e0015f6:	bf00      	nop
 e0015f8:	0e007f80 	.word	0x0e007f80
 e0015fc:	0e007fa4 	.word	0x0e007fa4
 e001600:	2000b200 	.word	0x2000b200

0e001604 <FLASH_EraseXIP>:
 e001604:	b538      	push	{r3, r4, r5, lr}
 e001606:	4605      	mov	r5, r0
 e001608:	460c      	mov	r4, r1
 e00160a:	f7ff ffd9 	bl	e0015c0 <FLASH_Write_Lock>
 e00160e:	4b2f      	ldr	r3, [pc, #188]	; (e0016cc <FLASH_EraseXIP+0xc8>)
 e001610:	4621      	mov	r1, r4
 e001612:	4628      	mov	r0, r5
 e001614:	4798      	blx	r3
 e001616:	2d02      	cmp	r5, #2
 e001618:	d039      	beq.n	e00168e <FLASH_EraseXIP+0x8a>
 e00161a:	2d01      	cmp	r5, #1
 e00161c:	d01f      	beq.n	e00165e <FLASH_EraseXIP+0x5a>
 e00161e:	4a2c      	ldr	r2, [pc, #176]	; (e0016d0 <FLASH_EraseXIP+0xcc>)
 e001620:	6953      	ldr	r3, [r2, #20]
 e001622:	03db      	lsls	r3, r3, #15
 e001624:	d50d      	bpl.n	e001642 <FLASH_EraseXIP+0x3e>
 e001626:	f3bf 8f4f 	dsb	sy
 e00162a:	f04f 6300 	mov.w	r3, #134217728	; 0x8000000
 e00162e:	f8c2 325c 	str.w	r3, [r2, #604]	; 0x25c
 e001632:	3320      	adds	r3, #32
 e001634:	f1b3 5f80 	cmp.w	r3, #268435456	; 0x10000000
 e001638:	d1f9      	bne.n	e00162e <FLASH_EraseXIP+0x2a>
 e00163a:	f3bf 8f4f 	dsb	sy
 e00163e:	f3bf 8f6f 	isb	sy
 e001642:	2002      	movs	r0, #2
 e001644:	f7ff ff8c 	bl	e001560 <Flash_Write_Lock_IPC>
 e001648:	4b22      	ldr	r3, [pc, #136]	; (e0016d4 <FLASH_EraseXIP+0xd0>)
 e00164a:	6818      	ldr	r0, [r3, #0]
 e00164c:	f7ff f86a 	bl	e000724 <irq_enable_restore>
 e001650:	2002      	movs	r0, #2
 e001652:	f006 f9dd 	bl	e007a10 <__IPC_SEMFree_veneer>
 e001656:	e8bd 4038 	ldmia.w	sp!, {r3, r4, r5, lr}
 e00165a:	f001 bbe5 	b.w	e002e28 <rtos_sched_resume>
 e00165e:	4b1c      	ldr	r3, [pc, #112]	; (e0016d0 <FLASH_EraseXIP+0xcc>)
 e001660:	695b      	ldr	r3, [r3, #20]
 e001662:	03da      	lsls	r2, r3, #15
 e001664:	d5ed      	bpl.n	e001642 <FLASH_EraseXIP+0x3e>
 e001666:	f104 6300 	add.w	r3, r4, #134217728	; 0x8000000
 e00166a:	f014 041f 	ands.w	r4, r4, #31
 e00166e:	d029      	beq.n	e0016c4 <FLASH_EraseXIP+0xc0>
 e001670:	f023 031f 	bic.w	r3, r3, #31
 e001674:	f504 3280 	add.w	r2, r4, #65536	; 0x10000
 e001678:	f3bf 8f4f 	dsb	sy
 e00167c:	4814      	ldr	r0, [pc, #80]	; (e0016d0 <FLASH_EraseXIP+0xcc>)
 e00167e:	441a      	add	r2, r3
 e001680:	f8c0 325c 	str.w	r3, [r0, #604]	; 0x25c
 e001684:	3320      	adds	r3, #32
 e001686:	1ad1      	subs	r1, r2, r3
 e001688:	2900      	cmp	r1, #0
 e00168a:	dcf9      	bgt.n	e001680 <FLASH_EraseXIP+0x7c>
 e00168c:	e7d5      	b.n	e00163a <FLASH_EraseXIP+0x36>
 e00168e:	4b10      	ldr	r3, [pc, #64]	; (e0016d0 <FLASH_EraseXIP+0xcc>)
 e001690:	695b      	ldr	r3, [r3, #20]
 e001692:	03d9      	lsls	r1, r3, #15
 e001694:	d5d5      	bpl.n	e001642 <FLASH_EraseXIP+0x3e>
 e001696:	f104 6300 	add.w	r3, r4, #134217728	; 0x8000000
 e00169a:	f014 041f 	ands.w	r4, r4, #31
 e00169e:	d10c      	bne.n	e0016ba <FLASH_EraseXIP+0xb6>
 e0016a0:	f44f 5480 	mov.w	r4, #4096	; 0x1000
 e0016a4:	f3bf 8f4f 	dsb	sy
 e0016a8:	4909      	ldr	r1, [pc, #36]	; (e0016d0 <FLASH_EraseXIP+0xcc>)
 e0016aa:	441c      	add	r4, r3
 e0016ac:	f8c1 325c 	str.w	r3, [r1, #604]	; 0x25c
 e0016b0:	3320      	adds	r3, #32
 e0016b2:	1ae2      	subs	r2, r4, r3
 e0016b4:	2a00      	cmp	r2, #0
 e0016b6:	dcf9      	bgt.n	e0016ac <FLASH_EraseXIP+0xa8>
 e0016b8:	e7bf      	b.n	e00163a <FLASH_EraseXIP+0x36>
 e0016ba:	f023 031f 	bic.w	r3, r3, #31
 e0016be:	f504 5480 	add.w	r4, r4, #4096	; 0x1000
 e0016c2:	e7ef      	b.n	e0016a4 <FLASH_EraseXIP+0xa0>
 e0016c4:	f44f 3280 	mov.w	r2, #65536	; 0x10000
 e0016c8:	e7d6      	b.n	e001678 <FLASH_EraseXIP+0x74>
 e0016ca:	bf00      	nop
 e0016cc:	00009fe5 	.word	0x00009fe5
 e0016d0:	e000ed00 	.word	0xe000ed00
 e0016d4:	2000b200 	.word	0x2000b200

0e0016d8 <FLASH_ReadStream>:
 e0016d8:	b530      	push	{r4, r5, lr}
 e0016da:	4614      	mov	r4, r2
 e0016dc:	b083      	sub	sp, #12
 e0016de:	4605      	mov	r5, r0
 e0016e0:	460a      	mov	r2, r1
 e0016e2:	b13c      	cbz	r4, e0016f4 <FLASH_ReadStream+0x1c>
 e0016e4:	f105 6100 	add.w	r1, r5, #134217728	; 0x8000000
 e0016e8:	4620      	mov	r0, r4
 e0016ea:	4b06      	ldr	r3, [pc, #24]	; (e001704 <FLASH_ReadStream+0x2c>)
 e0016ec:	4798      	blx	r3
 e0016ee:	2001      	movs	r0, #1
 e0016f0:	b003      	add	sp, #12
 e0016f2:	bd30      	pop	{r4, r5, pc}
 e0016f4:	9101      	str	r1, [sp, #4]
 e0016f6:	f240 1119 	movw	r1, #281	; 0x119
 e0016fa:	4803      	ldr	r0, [pc, #12]	; (e001708 <FLASH_ReadStream+0x30>)
 e0016fc:	f005 fe38 	bl	e007370 <io_assert_failed>
 e001700:	9a01      	ldr	r2, [sp, #4]
 e001702:	e7ef      	b.n	e0016e4 <FLASH_ReadStream+0xc>
 e001704:	00011c1d 	.word	0x00011c1d
 e001708:	0e007fac 	.word	0x0e007fac

0e00170c <FLASH_WriteStream>:
 e00170c:	e92d 4ff8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e001710:	eb00 0b01 	add.w	fp, r0, r1
 e001714:	f020 05ff 	bic.w	r5, r0, #255	; 0xff
 e001718:	4680      	mov	r8, r0
 e00171a:	4689      	mov	r9, r1
 e00171c:	f10b 33ff 	add.w	r3, fp, #4294967295	; 0xffffffff
 e001720:	4616      	mov	r6, r2
 e001722:	f023 03ff 	bic.w	r3, r3, #255	; 0xff
 e001726:	1b5b      	subs	r3, r3, r5
 e001728:	0a1b      	lsrs	r3, r3, #8
 e00172a:	1c5c      	adds	r4, r3, #1
 e00172c:	2b00      	cmp	r3, #0
 e00172e:	d071      	beq.n	e001814 <FLASH_WriteStream+0x108>
 e001730:	f505 7580 	add.w	r5, r5, #256	; 0x100
 e001734:	1a2f      	subs	r7, r5, r0
 e001736:	f7ff ff43 	bl	e0015c0 <FLASH_Write_Lock>
 e00173a:	f8df a0e8 	ldr.w	sl, [pc, #232]	; e001824 <FLASH_WriteStream+0x118>
 e00173e:	4640      	mov	r0, r8
 e001740:	e002      	b.n	e001748 <FLASH_WriteStream+0x3c>
 e001742:	b184      	cbz	r4, e001766 <FLASH_WriteStream+0x5a>
 e001744:	4628      	mov	r0, r5
 e001746:	461d      	mov	r5, r3
 e001748:	3c01      	subs	r4, #1
 e00174a:	4639      	mov	r1, r7
 e00174c:	4632      	mov	r2, r6
 e00174e:	443e      	add	r6, r7
 e001750:	47d0      	blx	sl
 e001752:	f44f 7780 	mov.w	r7, #256	; 0x100
 e001756:	2c01      	cmp	r4, #1
 e001758:	eb05 0307 	add.w	r3, r5, r7
 e00175c:	d1f1      	bne.n	e001742 <FLASH_WriteStream+0x36>
 e00175e:	ebab 0705 	sub.w	r7, fp, r5
 e001762:	465b      	mov	r3, fp
 e001764:	e7ee      	b.n	e001744 <FLASH_WriteStream+0x38>
 e001766:	4d2d      	ldr	r5, [pc, #180]	; (e00181c <FLASH_WriteStream+0x110>)
 e001768:	696b      	ldr	r3, [r5, #20]
 e00176a:	03d9      	lsls	r1, r3, #15
 e00176c:	d523      	bpl.n	e0017b6 <FLASH_WriteStream+0xaa>
 e00176e:	f108 6000 	add.w	r0, r8, #134217728	; 0x8000000
 e001772:	ea00 0309 	and.w	r3, r0, r9
 e001776:	3301      	adds	r3, #1
 e001778:	d02c      	beq.n	e0017d4 <FLASH_WriteStream+0xc8>
 e00177a:	f010 031f 	ands.w	r3, r0, #31
 e00177e:	d002      	beq.n	e001786 <FLASH_WriteStream+0x7a>
 e001780:	f020 001f 	bic.w	r0, r0, #31
 e001784:	4499      	add	r9, r3
 e001786:	f3bf 8f4f 	dsb	sy
 e00178a:	f1b9 0f00 	cmp.w	r9, #0
 e00178e:	dd0e      	ble.n	e0017ae <FLASH_WriteStream+0xa2>
 e001790:	f109 32ff 	add.w	r2, r9, #4294967295	; 0xffffffff
 e001794:	f100 0320 	add.w	r3, r0, #32
 e001798:	4920      	ldr	r1, [pc, #128]	; (e00181c <FLASH_WriteStream+0x110>)
 e00179a:	f022 021f 	bic.w	r2, r2, #31
 e00179e:	441a      	add	r2, r3
 e0017a0:	4293      	cmp	r3, r2
 e0017a2:	f8c1 025c 	str.w	r0, [r1, #604]	; 0x25c
 e0017a6:	4618      	mov	r0, r3
 e0017a8:	f103 0320 	add.w	r3, r3, #32
 e0017ac:	d1f8      	bne.n	e0017a0 <FLASH_WriteStream+0x94>
 e0017ae:	f3bf 8f4f 	dsb	sy
 e0017b2:	f3bf 8f6f 	isb	sy
 e0017b6:	2002      	movs	r0, #2
 e0017b8:	f7ff fed2 	bl	e001560 <Flash_Write_Lock_IPC>
 e0017bc:	4b18      	ldr	r3, [pc, #96]	; (e001820 <FLASH_WriteStream+0x114>)
 e0017be:	6818      	ldr	r0, [r3, #0]
 e0017c0:	f7fe ffb0 	bl	e000724 <irq_enable_restore>
 e0017c4:	2002      	movs	r0, #2
 e0017c6:	f006 f923 	bl	e007a10 <__IPC_SEMFree_veneer>
 e0017ca:	f001 fb2d 	bl	e002e28 <rtos_sched_resume>
 e0017ce:	2001      	movs	r0, #1
 e0017d0:	e8bd 8ff8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, pc}
 e0017d4:	f8c5 4084 	str.w	r4, [r5, #132]	; 0x84
 e0017d8:	f3bf 8f4f 	dsb	sy
 e0017dc:	f8d5 4080 	ldr.w	r4, [r5, #128]	; 0x80
 e0017e0:	f643 76e0 	movw	r6, #16352	; 0x3fe0
 e0017e4:	f3c4 304e 	ubfx	r0, r4, #13, #15
 e0017e8:	f3c4 04c9 	ubfx	r4, r4, #3, #10
 e0017ec:	0140      	lsls	r0, r0, #5
 e0017ee:	ea00 0106 	and.w	r1, r0, r6
 e0017f2:	4623      	mov	r3, r4
 e0017f4:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
 e0017f8:	3b01      	subs	r3, #1
 e0017fa:	f8c5 2260 	str.w	r2, [r5, #608]	; 0x260
 e0017fe:	1c5a      	adds	r2, r3, #1
 e001800:	d1f8      	bne.n	e0017f4 <FLASH_WriteStream+0xe8>
 e001802:	3820      	subs	r0, #32
 e001804:	f110 0f20 	cmn.w	r0, #32
 e001808:	d1f1      	bne.n	e0017ee <FLASH_WriteStream+0xe2>
 e00180a:	f3bf 8f4f 	dsb	sy
 e00180e:	f3bf 8f6f 	isb	sy
 e001812:	e7d0      	b.n	e0017b6 <FLASH_WriteStream+0xaa>
 e001814:	460f      	mov	r7, r1
 e001816:	465d      	mov	r5, fp
 e001818:	e78d      	b.n	e001736 <FLASH_WriteStream+0x2a>
 e00181a:	bf00      	nop
 e00181c:	e000ed00 	.word	0xe000ed00
 e001820:	2000b200 	.word	0x2000b200
 e001824:	00009e6d 	.word	0x00009e6d

0e001828 <ipc_table_init>:
 e001828:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e00182c:	4b27      	ldr	r3, [pc, #156]	; (e0018cc <ipc_table_init+0xa4>)
 e00182e:	b083      	sub	sp, #12
 e001830:	4c27      	ldr	r4, [pc, #156]	; (e0018d0 <ipc_table_init+0xa8>)
 e001832:	4a28      	ldr	r2, [pc, #160]	; (e0018d4 <ipc_table_init+0xac>)
 e001834:	1b1b      	subs	r3, r3, r4
 e001836:	ea4f 0893 	mov.w	r8, r3, lsr #2
 e00183a:	2b1b      	cmp	r3, #27
 e00183c:	fba2 2808 	umull	r2, r8, r2, r8
 e001840:	d934      	bls.n	e0018ac <ipc_table_init+0x84>
 e001842:	4606      	mov	r6, r0
 e001844:	2700      	movs	r7, #0
 e001846:	f8df b094 	ldr.w	fp, [pc, #148]	; e0018dc <ipc_table_init+0xb4>
 e00184a:	f8df a094 	ldr.w	sl, [pc, #148]	; e0018e0 <ipc_table_init+0xb8>
 e00184e:	f8df 9094 	ldr.w	r9, [pc, #148]	; e0018e4 <ipc_table_init+0xbc>
 e001852:	e013      	b.n	e00187c <ipc_table_init+0x54>
 e001854:	454e      	cmp	r6, r9
 e001856:	d017      	beq.n	e001888 <ipc_table_init+0x60>
 e001858:	4630      	mov	r0, r6
 e00185a:	3510      	adds	r5, #16
 e00185c:	f000 f93e 	bl	e001adc <IPC_IERGet>
 e001860:	2301      	movs	r3, #1
 e001862:	40ab      	lsls	r3, r5
 e001864:	4203      	tst	r3, r0
 e001866:	d124      	bne.n	e0018b2 <ipc_table_init+0x8a>
 e001868:	b2e9      	uxtb	r1, r5
 e00186a:	4630      	mov	r0, r6
 e00186c:	e9d4 2303 	ldrd	r2, r3, [r4, #12]
 e001870:	f000 f960 	bl	e001b34 <IPC_INTUserHandler>
 e001874:	3701      	adds	r7, #1
 e001876:	341c      	adds	r4, #28
 e001878:	45b8      	cmp	r8, r7
 e00187a:	d917      	bls.n	e0018ac <ipc_table_init+0x84>
 e00187c:	e9d4 3505 	ldrd	r3, r5, [r4, #20]
 e001880:	2b01      	cmp	r3, #1
 e001882:	d1e7      	bne.n	e001854 <ipc_table_init+0x2c>
 e001884:	454e      	cmp	r6, r9
 e001886:	d0e7      	beq.n	e001858 <ipc_table_init+0x30>
 e001888:	4630      	mov	r0, r6
 e00188a:	f000 f927 	bl	e001adc <IPC_IERGet>
 e00188e:	2301      	movs	r3, #1
 e001890:	40ab      	lsls	r3, r5
 e001892:	4203      	tst	r3, r0
 e001894:	d10d      	bne.n	e0018b2 <ipc_table_init+0x8a>
 e001896:	3701      	adds	r7, #1
 e001898:	b2e9      	uxtb	r1, r5
 e00189a:	4630      	mov	r0, r6
 e00189c:	e9d4 2301 	ldrd	r2, r3, [r4, #4]
 e0018a0:	f000 f948 	bl	e001b34 <IPC_INTUserHandler>
 e0018a4:	45b8      	cmp	r8, r7
 e0018a6:	f104 041c 	add.w	r4, r4, #28
 e0018aa:	d8e7      	bhi.n	e00187c <ipc_table_init+0x54>
 e0018ac:	b003      	add	sp, #12
 e0018ae:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 e0018b2:	f8db 3228 	ldr.w	r3, [fp, #552]	; 0x228
 e0018b6:	2249      	movs	r2, #73	; 0x49
 e0018b8:	4907      	ldr	r1, [pc, #28]	; (e0018d8 <ipc_table_init+0xb0>)
 e0018ba:	2004      	movs	r0, #4
 e0018bc:	f3c3 1381 	ubfx	r3, r3, #6, #2
 e0018c0:	9501      	str	r5, [sp, #4]
 e0018c2:	9300      	str	r3, [sp, #0]
 e0018c4:	4653      	mov	r3, sl
 e0018c6:	f000 fdb9 	bl	e00243c <rtk_log_write>
 e0018ca:	e7d3      	b.n	e001874 <ipc_table_init+0x4c>
 e0018cc:	0e009a00 	.word	0x0e009a00
 e0018d0:	0e009990 	.word	0x0e009990
 e0018d4:	24924925 	.word	0x24924925
 e0018d8:	0e008034 	.word	0x0e008034
 e0018dc:	41008000 	.word	0x41008000
 e0018e0:	0e007fec 	.word	0x0e007fec
 e0018e4:	41014080 	.word	0x41014080

0e0018e8 <IPC_TXHandler>:
 e0018e8:	4b0b      	ldr	r3, [pc, #44]	; (e001918 <IPC_TXHandler+0x30>)
 e0018ea:	b510      	push	{r4, lr}
 e0018ec:	f8d3 0228 	ldr.w	r0, [r3, #552]	; 0x228
 e0018f0:	4614      	mov	r4, r2
 e0018f2:	f3c0 1081 	ubfx	r0, r0, #6, #2
 e0018f6:	f000 f951 	bl	e001b9c <IPC_GetDevById>
 e0018fa:	2200      	movs	r2, #0
 e0018fc:	b2e1      	uxtb	r1, r4
 e0018fe:	3c10      	subs	r4, #16
 e001900:	f000 f8e0 	bl	e001ac4 <IPC_INTConfig>
 e001904:	4b05      	ldr	r3, [pc, #20]	; (e00191c <IPC_TXHandler+0x34>)
 e001906:	f853 0024 	ldr.w	r0, [r3, r4, lsl #2]
 e00190a:	b118      	cbz	r0, e001914 <IPC_TXHandler+0x2c>
 e00190c:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
 e001910:	f001 b974 	b.w	e002bfc <rtos_sema_give>
 e001914:	bd10      	pop	{r4, pc}
 e001916:	bf00      	nop
 e001918:	41008000 	.word	0x41008000
 e00191c:	2000b204 	.word	0x2000b204

0e001920 <IPC_wait_idle>:
 e001920:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 e001922:	460d      	mov	r5, r1
 e001924:	4604      	mov	r4, r0
 e001926:	f7fe fef3 	bl	e000710 <CPU_InInterrupt>
 e00192a:	f105 0610 	add.w	r6, r5, #16
 e00192e:	b9b0      	cbnz	r0, e00195e <IPC_wait_idle+0x3e>
 e001930:	4b1b      	ldr	r3, [pc, #108]	; (e0019a0 <IPC_wait_idle+0x80>)
 e001932:	f853 3026 	ldr.w	r3, [r3, r6, lsl #2]
 e001936:	b193      	cbz	r3, e00195e <IPC_wait_idle+0x3e>
 e001938:	4f1a      	ldr	r7, [pc, #104]	; (e0019a4 <IPC_wait_idle+0x84>)
 e00193a:	f857 3025 	ldr.w	r3, [r7, r5, lsl #2]
 e00193e:	b1fb      	cbz	r3, e001980 <IPC_wait_idle+0x60>
 e001940:	b2f6      	uxtb	r6, r6
 e001942:	4620      	mov	r0, r4
 e001944:	2201      	movs	r2, #1
 e001946:	4631      	mov	r1, r6
 e001948:	f000 f8bc 	bl	e001ac4 <IPC_INTConfig>
 e00194c:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
 e001950:	f857 0025 	ldr.w	r0, [r7, r5, lsl #2]
 e001954:	f001 f92a 	bl	e002bac <rtos_sema_take>
 e001958:	b9b8      	cbnz	r0, e00198a <IPC_wait_idle+0x6a>
 e00195a:	2000      	movs	r0, #0
 e00195c:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e00195e:	2101      	movs	r1, #1
 e001960:	4b11      	ldr	r3, [pc, #68]	; (e0019a8 <IPC_wait_idle+0x88>)
 e001962:	40b1      	lsls	r1, r6
 e001964:	e001      	b.n	e00196a <IPC_wait_idle+0x4a>
 e001966:	3b01      	subs	r3, #1
 e001968:	d004      	beq.n	e001974 <IPC_wait_idle+0x54>
 e00196a:	6822      	ldr	r2, [r4, #0]
 e00196c:	420a      	tst	r2, r1
 e00196e:	d1fa      	bne.n	e001966 <IPC_wait_idle+0x46>
 e001970:	2000      	movs	r0, #0
 e001972:	e7f3      	b.n	e00195c <IPC_wait_idle+0x3c>
 e001974:	480d      	ldr	r0, [pc, #52]	; (e0019ac <IPC_wait_idle+0x8c>)
 e001976:	f005 fb55 	bl	e007024 <DiagPrintf_minimal>
 e00197a:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
 e00197e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e001980:	eb07 0085 	add.w	r0, r7, r5, lsl #2
 e001984:	f001 f900 	bl	e002b88 <rtos_sema_create_binary>
 e001988:	e7da      	b.n	e001940 <IPC_wait_idle+0x20>
 e00198a:	4809      	ldr	r0, [pc, #36]	; (e0019b0 <IPC_wait_idle+0x90>)
 e00198c:	f005 fb4a 	bl	e007024 <DiagPrintf_minimal>
 e001990:	4620      	mov	r0, r4
 e001992:	2200      	movs	r2, #0
 e001994:	4631      	mov	r1, r6
 e001996:	f000 f895 	bl	e001ac4 <IPC_INTConfig>
 e00199a:	f06f 0001 	mvn.w	r0, #1
 e00199e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e0019a0:	2000b2c4 	.word	0x2000b2c4
 e0019a4:	2000b204 	.word	0x2000b204
 e0019a8:	00989680 	.word	0x00989680
 e0019ac:	0e008038 	.word	0x0e008038
 e0019b0:	0e008050 	.word	0x0e008050

0e0019b4 <ipc_send_message>:
 e0019b4:	2801      	cmp	r0, #1
 e0019b6:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
 e0019ba:	4605      	mov	r5, r0
 e0019bc:	460c      	mov	r4, r1
 e0019be:	4690      	mov	r8, r2
 e0019c0:	d903      	bls.n	e0019ca <ipc_send_message+0x16>
 e0019c2:	2191      	movs	r1, #145	; 0x91
 e0019c4:	4826      	ldr	r0, [pc, #152]	; (e001a60 <ipc_send_message+0xac>)
 e0019c6:	f005 fcd3 	bl	e007370 <io_assert_failed>
 e0019ca:	2c0f      	cmp	r4, #15
 e0019cc:	d83f      	bhi.n	e001a4e <ipc_send_message+0x9a>
 e0019ce:	f104 0710 	add.w	r7, r4, #16
 e0019d2:	f04f 0901 	mov.w	r9, #1
 e0019d6:	2100      	movs	r1, #0
 e0019d8:	4628      	mov	r0, r5
 e0019da:	f000 f8c3 	bl	e001b64 <IPC_GetDev>
 e0019de:	fa09 f707 	lsl.w	r7, r9, r7
 e0019e2:	6803      	ldr	r3, [r0, #0]
 e0019e4:	4606      	mov	r6, r0
 e0019e6:	423b      	tst	r3, r7
 e0019e8:	d129      	bne.n	e001a3e <ipc_send_message+0x8a>
 e0019ea:	f1b8 0f00 	cmp.w	r8, #0
 e0019ee:	d022      	beq.n	e001a36 <ipc_send_message+0x82>
 e0019f0:	f005 050f 	and.w	r5, r5, #15
 e0019f4:	4b1b      	ldr	r3, [pc, #108]	; (e001a64 <ipc_send_message+0xb0>)
 e0019f6:	4641      	mov	r1, r8
 e0019f8:	2210      	movs	r2, #16
 e0019fa:	eb04 1405 	add.w	r4, r4, r5, lsl #4
 e0019fe:	eb03 1404 	add.w	r4, r3, r4, lsl #4
 e001a02:	4620      	mov	r0, r4
 e001a04:	f005 fff8 	bl	e0079f8 <____wrap_memcpy_veneer>
 e001a08:	4b17      	ldr	r3, [pc, #92]	; (e001a68 <ipc_send_message+0xb4>)
 e001a0a:	695b      	ldr	r3, [r3, #20]
 e001a0c:	03db      	lsls	r3, r3, #15
 e001a0e:	d512      	bpl.n	e001a36 <ipc_send_message+0x82>
 e001a10:	f014 011f 	ands.w	r1, r4, #31
 e001a14:	4623      	mov	r3, r4
 e001a16:	d11f      	bne.n	e001a58 <ipc_send_message+0xa4>
 e001a18:	2110      	movs	r1, #16
 e001a1a:	f3bf 8f4f 	dsb	sy
 e001a1e:	4812      	ldr	r0, [pc, #72]	; (e001a68 <ipc_send_message+0xb4>)
 e001a20:	4419      	add	r1, r3
 e001a22:	f8c0 3268 	str.w	r3, [r0, #616]	; 0x268
 e001a26:	3320      	adds	r3, #32
 e001a28:	1aca      	subs	r2, r1, r3
 e001a2a:	2a00      	cmp	r2, #0
 e001a2c:	dcf9      	bgt.n	e001a22 <ipc_send_message+0x6e>
 e001a2e:	f3bf 8f4f 	dsb	sy
 e001a32:	f3bf 8f6f 	isb	sy
 e001a36:	2000      	movs	r0, #0
 e001a38:	6037      	str	r7, [r6, #0]
 e001a3a:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
 e001a3e:	4621      	mov	r1, r4
 e001a40:	f7ff ff6e 	bl	e001920 <IPC_wait_idle>
 e001a44:	2800      	cmp	r0, #0
 e001a46:	d0d0      	beq.n	e0019ea <ipc_send_message+0x36>
 e001a48:	4648      	mov	r0, r9
 e001a4a:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
 e001a4e:	2192      	movs	r1, #146	; 0x92
 e001a50:	4803      	ldr	r0, [pc, #12]	; (e001a60 <ipc_send_message+0xac>)
 e001a52:	f005 fc8d 	bl	e007370 <io_assert_failed>
 e001a56:	e7ba      	b.n	e0019ce <ipc_send_message+0x1a>
 e001a58:	f024 031f 	bic.w	r3, r4, #31
 e001a5c:	3110      	adds	r1, #16
 e001a5e:	e7dc      	b.n	e001a1a <ipc_send_message+0x66>
 e001a60:	0e008070 	.word	0x0e008070
 e001a64:	20004e00 	.word	0x20004e00
 e001a68:	e000ed00 	.word	0xe000ed00

0e001a6c <ipc_get_message>:
 e001a6c:	4a13      	ldr	r2, [pc, #76]	; (e001abc <ipc_get_message+0x50>)
 e001a6e:	f000 000f 	and.w	r0, r0, #15
 e001a72:	4b13      	ldr	r3, [pc, #76]	; (e001ac0 <ipc_get_message+0x54>)
 e001a74:	6952      	ldr	r2, [r2, #20]
 e001a76:	eb01 1000 	add.w	r0, r1, r0, lsl #4
 e001a7a:	eb03 1000 	add.w	r0, r3, r0, lsl #4
 e001a7e:	03d3      	lsls	r3, r2, #15
 e001a80:	d51a      	bpl.n	e001ab8 <ipc_get_message+0x4c>
 e001a82:	f010 011f 	ands.w	r1, r0, #31
 e001a86:	4603      	mov	r3, r0
 e001a88:	b410      	push	{r4}
 e001a8a:	d111      	bne.n	e001ab0 <ipc_get_message+0x44>
 e001a8c:	2110      	movs	r1, #16
 e001a8e:	f3bf 8f4f 	dsb	sy
 e001a92:	4c0a      	ldr	r4, [pc, #40]	; (e001abc <ipc_get_message+0x50>)
 e001a94:	4419      	add	r1, r3
 e001a96:	f8c4 325c 	str.w	r3, [r4, #604]	; 0x25c
 e001a9a:	3320      	adds	r3, #32
 e001a9c:	1aca      	subs	r2, r1, r3
 e001a9e:	2a00      	cmp	r2, #0
 e001aa0:	dcf9      	bgt.n	e001a96 <ipc_get_message+0x2a>
 e001aa2:	f3bf 8f4f 	dsb	sy
 e001aa6:	f3bf 8f6f 	isb	sy
 e001aaa:	f85d 4b04 	ldr.w	r4, [sp], #4
 e001aae:	4770      	bx	lr
 e001ab0:	f020 031f 	bic.w	r3, r0, #31
 e001ab4:	3110      	adds	r1, #16
 e001ab6:	e7ea      	b.n	e001a8e <ipc_get_message+0x22>
 e001ab8:	4770      	bx	lr
 e001aba:	bf00      	nop
 e001abc:	e000ed00 	.word	0xe000ed00
 e001ac0:	20004e00 	.word	0x20004e00

0e001ac4 <IPC_INTConfig>:
 e001ac4:	2301      	movs	r3, #1
 e001ac6:	fa03 f101 	lsl.w	r1, r3, r1
 e001aca:	429a      	cmp	r2, r3
 e001acc:	6883      	ldr	r3, [r0, #8]
 e001ace:	bf0c      	ite	eq
 e001ad0:	4319      	orreq	r1, r3
 e001ad2:	ea23 0101 	bicne.w	r1, r3, r1
 e001ad6:	6081      	str	r1, [r0, #8]
 e001ad8:	4770      	bx	lr
 e001ada:	bf00      	nop

0e001adc <IPC_IERGet>:
 e001adc:	6880      	ldr	r0, [r0, #8]
 e001ade:	4770      	bx	lr

0e001ae0 <IPC_INTHandler>:
 e001ae0:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 e001ae4:	2400      	movs	r4, #0
 e001ae6:	2701      	movs	r7, #1
 e001ae8:	f8df 9040 	ldr.w	r9, [pc, #64]	; e001b2c <IPC_INTHandler+0x4c>
 e001aec:	4680      	mov	r8, r0
 e001aee:	f8df a040 	ldr.w	sl, [pc, #64]	; e001b30 <IPC_INTHandler+0x50>
 e001af2:	6846      	ldr	r6, [r0, #4]
 e001af4:	e007      	b.n	e001b06 <IPC_INTHandler+0x26>
 e001af6:	f85a 0024 	ldr.w	r0, [sl, r4, lsl #2]
 e001afa:	4798      	blx	r3
 e001afc:	f8c8 5004 	str.w	r5, [r8, #4]
 e001b00:	3401      	adds	r4, #1
 e001b02:	2c20      	cmp	r4, #32
 e001b04:	d00e      	beq.n	e001b24 <IPC_INTHandler+0x44>
 e001b06:	fa07 f504 	lsl.w	r5, r7, r4
 e001b0a:	4235      	tst	r5, r6
 e001b0c:	d0f8      	beq.n	e001b00 <IPC_INTHandler+0x20>
 e001b0e:	f859 3024 	ldr.w	r3, [r9, r4, lsl #2]
 e001b12:	4622      	mov	r2, r4
 e001b14:	4631      	mov	r1, r6
 e001b16:	2b00      	cmp	r3, #0
 e001b18:	d1ed      	bne.n	e001af6 <IPC_INTHandler+0x16>
 e001b1a:	3401      	adds	r4, #1
 e001b1c:	f8c8 5004 	str.w	r5, [r8, #4]
 e001b20:	2c20      	cmp	r4, #32
 e001b22:	d1f0      	bne.n	e001b06 <IPC_INTHandler+0x26>
 e001b24:	2000      	movs	r0, #0
 e001b26:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
 e001b2a:	bf00      	nop
 e001b2c:	2000b2c4 	.word	0x2000b2c4
 e001b30:	2000b244 	.word	0x2000b244

0e001b34 <IPC_INTUserHandler>:
 e001b34:	b18a      	cbz	r2, e001b5a <IPC_INTUserHandler+0x26>
 e001b36:	290f      	cmp	r1, #15
 e001b38:	b410      	push	{r4}
 e001b3a:	4c08      	ldr	r4, [pc, #32]	; (e001b5c <IPC_INTUserHandler+0x28>)
 e001b3c:	f844 2021 	str.w	r2, [r4, r1, lsl #2]
 e001b40:	4a07      	ldr	r2, [pc, #28]	; (e001b60 <IPC_INTUserHandler+0x2c>)
 e001b42:	f842 3021 	str.w	r3, [r2, r1, lsl #2]
 e001b46:	d805      	bhi.n	e001b54 <IPC_INTUserHandler+0x20>
 e001b48:	2301      	movs	r3, #1
 e001b4a:	6882      	ldr	r2, [r0, #8]
 e001b4c:	fa03 f101 	lsl.w	r1, r3, r1
 e001b50:	4311      	orrs	r1, r2
 e001b52:	6081      	str	r1, [r0, #8]
 e001b54:	f85d 4b04 	ldr.w	r4, [sp], #4
 e001b58:	4770      	bx	lr
 e001b5a:	4770      	bx	lr
 e001b5c:	2000b2c4 	.word	0x2000b2c4
 e001b60:	2000b244 	.word	0x2000b244

0e001b64 <IPC_GetDev>:
 e001b64:	2801      	cmp	r0, #1
 e001b66:	b510      	push	{r4, lr}
 e001b68:	460c      	mov	r4, r1
 e001b6a:	d808      	bhi.n	e001b7e <IPC_GetDev+0x1a>
 e001b6c:	b119      	cbz	r1, e001b76 <IPC_GetDev+0x12>
 e001b6e:	2801      	cmp	r0, #1
 e001b70:	d103      	bne.n	e001b7a <IPC_GetDev+0x16>
 e001b72:	4807      	ldr	r0, [pc, #28]	; (e001b90 <IPC_GetDev+0x2c>)
 e001b74:	bd10      	pop	{r4, pc}
 e001b76:	2801      	cmp	r0, #1
 e001b78:	d1fb      	bne.n	e001b72 <IPC_GetDev+0xe>
 e001b7a:	4806      	ldr	r0, [pc, #24]	; (e001b94 <IPC_GetDev+0x30>)
 e001b7c:	bd10      	pop	{r4, pc}
 e001b7e:	21a6      	movs	r1, #166	; 0xa6
 e001b80:	4805      	ldr	r0, [pc, #20]	; (e001b98 <IPC_GetDev+0x34>)
 e001b82:	f005 fbf5 	bl	e007370 <io_assert_failed>
 e001b86:	2c00      	cmp	r4, #0
 e001b88:	d0f3      	beq.n	e001b72 <IPC_GetDev+0xe>
 e001b8a:	4802      	ldr	r0, [pc, #8]	; (e001b94 <IPC_GetDev+0x30>)
 e001b8c:	e7f6      	b.n	e001b7c <IPC_GetDev+0x18>
 e001b8e:	bf00      	nop
 e001b90:	41014000 	.word	0x41014000
 e001b94:	41014080 	.word	0x41014080
 e001b98:	0e0080ac 	.word	0x0e0080ac

0e001b9c <IPC_GetDevById>:
 e001b9c:	4a03      	ldr	r2, [pc, #12]	; (e001bac <IPC_GetDevById+0x10>)
 e001b9e:	4b04      	ldr	r3, [pc, #16]	; (e001bb0 <IPC_GetDevById+0x14>)
 e001ba0:	2800      	cmp	r0, #0
 e001ba2:	bf14      	ite	ne
 e001ba4:	4610      	movne	r0, r2
 e001ba6:	4618      	moveq	r0, r3
 e001ba8:	4770      	bx	lr
 e001baa:	bf00      	nop
 e001bac:	41014080 	.word	0x41014080
 e001bb0:	41014000 	.word	0x41014000

0e001bb4 <mpu_init>:
 e001bb4:	f3bf 8f5f 	dmb	sy
 e001bb8:	4b0f      	ldr	r3, [pc, #60]	; (e001bf8 <mpu_init+0x44>)
 e001bba:	2100      	movs	r1, #0
 e001bbc:	480f      	ldr	r0, [pc, #60]	; (e001bfc <mpu_init+0x48>)
 e001bbe:	f8d3 2094 	ldr.w	r2, [r3, #148]	; 0x94
 e001bc2:	f022 0201 	bic.w	r2, r2, #1
 e001bc6:	f8c3 2094 	str.w	r2, [r3, #148]	; 0x94
 e001bca:	f8c3 1094 	str.w	r1, [r3, #148]	; 0x94
 e001bce:	f8c3 00c0 	str.w	r0, [r3, #192]	; 0xc0
 e001bd2:	f8c3 10c4 	str.w	r1, [r3, #196]	; 0xc4
 e001bd6:	f8d3 2094 	ldr.w	r2, [r3, #148]	; 0x94
 e001bda:	f042 0204 	orr.w	r2, r2, #4
 e001bde:	f8c3 2094 	str.w	r2, [r3, #148]	; 0x94
 e001be2:	f8d3 2094 	ldr.w	r2, [r3, #148]	; 0x94
 e001be6:	f042 0201 	orr.w	r2, r2, #1
 e001bea:	f8c3 2094 	str.w	r2, [r3, #148]	; 0x94
 e001bee:	f3bf 8f4f 	dsb	sy
 e001bf2:	f3bf 8f6f 	isb	sy
 e001bf6:	4770      	bx	lr
 e001bf8:	e000ed00 	.word	0xe000ed00
 e001bfc:	00772244 	.word	0x00772244

0e001c00 <mpu_region_cfg>:
 e001c00:	2807      	cmp	r0, #7
 e001c02:	b530      	push	{r4, r5, lr}
 e001c04:	4605      	mov	r5, r0
 e001c06:	b083      	sub	sp, #12
 e001c08:	460c      	mov	r4, r1
 e001c0a:	d83c      	bhi.n	e001c86 <mpu_region_cfg+0x86>
 e001c0c:	6822      	ldr	r2, [r4, #0]
 e001c0e:	06d1      	lsls	r1, r2, #27
 e001c10:	d140      	bne.n	e001c94 <mpu_region_cfg+0x94>
 e001c12:	6861      	ldr	r1, [r4, #4]
 e001c14:	06cb      	lsls	r3, r1, #27
 e001c16:	d14b      	bne.n	e001cb0 <mpu_region_cfg+0xb0>
 e001c18:	6822      	ldr	r2, [r4, #0]
 e001c1a:	3a01      	subs	r2, #1
 e001c1c:	440a      	add	r2, r1
 e001c1e:	f3bf 8f5f 	dmb	sy
 e001c22:	4b2a      	ldr	r3, [pc, #168]	; (e001ccc <mpu_region_cfg+0xcc>)
 e001c24:	f022 0c1f 	bic.w	ip, r2, #31
 e001c28:	f8d3 2094 	ldr.w	r2, [r3, #148]	; 0x94
 e001c2c:	f022 0201 	bic.w	r2, r2, #1
 e001c30:	f8c3 2094 	str.w	r2, [r3, #148]	; 0x94
 e001c34:	f8c3 5098 	str.w	r5, [r3, #152]	; 0x98
 e001c38:	6821      	ldr	r1, [r4, #0]
 e001c3a:	7aa2      	ldrb	r2, [r4, #10]
 e001c3c:	f021 011f 	bic.w	r1, r1, #31
 e001c40:	4311      	orrs	r1, r2
 e001c42:	7a62      	ldrb	r2, [r4, #9]
 e001c44:	4311      	orrs	r1, r2
 e001c46:	7a22      	ldrb	r2, [r4, #8]
 e001c48:	4311      	orrs	r1, r2
 e001c4a:	f8c3 109c 	str.w	r1, [r3, #156]	; 0x9c
 e001c4e:	7ae2      	ldrb	r2, [r4, #11]
 e001c50:	0052      	lsls	r2, r2, #1
 e001c52:	f002 020e 	and.w	r2, r2, #14
 e001c56:	ea42 020c 	orr.w	r2, r2, ip
 e001c5a:	f042 0201 	orr.w	r2, r2, #1
 e001c5e:	f8c3 20a0 	str.w	r2, [r3, #160]	; 0xa0
 e001c62:	f8d3 2094 	ldr.w	r2, [r3, #148]	; 0x94
 e001c66:	f042 0201 	orr.w	r2, r2, #1
 e001c6a:	f8c3 2094 	str.w	r2, [r3, #148]	; 0x94
 e001c6e:	f8d3 2094 	ldr.w	r2, [r3, #148]	; 0x94
 e001c72:	f042 0201 	orr.w	r2, r2, #1
 e001c76:	f8c3 2094 	str.w	r2, [r3, #148]	; 0x94
 e001c7a:	f3bf 8f4f 	dsb	sy
 e001c7e:	f3bf 8f6f 	isb	sy
 e001c82:	b003      	add	sp, #12
 e001c84:	bd30      	pop	{r4, r5, pc}
 e001c86:	216a      	movs	r1, #106	; 0x6a
 e001c88:	4811      	ldr	r0, [pc, #68]	; (e001cd0 <mpu_region_cfg+0xd0>)
 e001c8a:	f005 fb71 	bl	e007370 <io_assert_failed>
 e001c8e:	6822      	ldr	r2, [r4, #0]
 e001c90:	06d1      	lsls	r1, r2, #27
 e001c92:	d0be      	beq.n	e001c12 <mpu_region_cfg+0x12>
 e001c94:	4b0f      	ldr	r3, [pc, #60]	; (e001cd4 <mpu_region_cfg+0xd4>)
 e001c96:	2003      	movs	r0, #3
 e001c98:	490f      	ldr	r1, [pc, #60]	; (e001cd8 <mpu_region_cfg+0xd8>)
 e001c9a:	9200      	str	r2, [sp, #0]
 e001c9c:	2257      	movs	r2, #87	; 0x57
 e001c9e:	f000 fbcd 	bl	e00243c <rtk_log_write>
 e001ca2:	216f      	movs	r1, #111	; 0x6f
 e001ca4:	480a      	ldr	r0, [pc, #40]	; (e001cd0 <mpu_region_cfg+0xd0>)
 e001ca6:	f005 fb63 	bl	e007370 <io_assert_failed>
 e001caa:	6861      	ldr	r1, [r4, #4]
 e001cac:	06cb      	lsls	r3, r1, #27
 e001cae:	d0b3      	beq.n	e001c18 <mpu_region_cfg+0x18>
 e001cb0:	4b0a      	ldr	r3, [pc, #40]	; (e001cdc <mpu_region_cfg+0xdc>)
 e001cb2:	2257      	movs	r2, #87	; 0x57
 e001cb4:	2003      	movs	r0, #3
 e001cb6:	9100      	str	r1, [sp, #0]
 e001cb8:	4907      	ldr	r1, [pc, #28]	; (e001cd8 <mpu_region_cfg+0xd8>)
 e001cba:	f000 fbbf 	bl	e00243c <rtk_log_write>
 e001cbe:	2174      	movs	r1, #116	; 0x74
 e001cc0:	4803      	ldr	r0, [pc, #12]	; (e001cd0 <mpu_region_cfg+0xd0>)
 e001cc2:	f005 fb55 	bl	e007370 <io_assert_failed>
 e001cc6:	6861      	ldr	r1, [r4, #4]
 e001cc8:	e7a6      	b.n	e001c18 <mpu_region_cfg+0x18>
 e001cca:	bf00      	nop
 e001ccc:	e000ed00 	.word	0xe000ed00
 e001cd0:	0e0080e8 	.word	0x0e0080e8
 e001cd4:	0e008124 	.word	0x0e008124
 e001cd8:	0e008158 	.word	0x0e008158
 e001cdc:	0e00815c 	.word	0x0e00815c

0e001ce0 <mpu_entry_alloc>:
 e001ce0:	4a07      	ldr	r2, [pc, #28]	; (e001d00 <mpu_entry_alloc+0x20>)
 e001ce2:	2300      	movs	r3, #0
 e001ce4:	f812 1f01 	ldrb.w	r1, [r2, #1]!
 e001ce8:	b121      	cbz	r1, e001cf4 <mpu_entry_alloc+0x14>
 e001cea:	3301      	adds	r3, #1
 e001cec:	2b08      	cmp	r3, #8
 e001cee:	d1f9      	bne.n	e001ce4 <mpu_entry_alloc+0x4>
 e001cf0:	20ff      	movs	r0, #255	; 0xff
 e001cf2:	4770      	bx	lr
 e001cf4:	4a03      	ldr	r2, [pc, #12]	; (e001d04 <mpu_entry_alloc+0x24>)
 e001cf6:	2101      	movs	r1, #1
 e001cf8:	b2d8      	uxtb	r0, r3
 e001cfa:	54d1      	strb	r1, [r2, r3]
 e001cfc:	4770      	bx	lr
 e001cfe:	bf00      	nop
 e001d00:	2000b343 	.word	0x2000b343
 e001d04:	2000b344 	.word	0x2000b344

0e001d08 <ota_get_cur_index>:
 e001d08:	eb00 0140 	add.w	r1, r0, r0, lsl #1
 e001d0c:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 e001d0e:	0089      	lsls	r1, r1, #2
 e001d10:	4b1b      	ldr	r3, [pc, #108]	; (e001d80 <ota_get_cur_index+0x78>)
 e001d12:	4e1c      	ldr	r6, [pc, #112]	; (e001d84 <ota_get_cur_index+0x7c>)
 e001d14:	4604      	mov	r4, r0
 e001d16:	440b      	add	r3, r1
 e001d18:	f856 7030 	ldr.w	r7, [r6, r0, lsl #3]
 e001d1c:	f8d3 5108 	ldr.w	r5, [r3, #264]	; 0x108
 e001d20:	b17f      	cbz	r7, e001d42 <ota_get_cur_index+0x3a>
 e001d22:	f015 0001 	ands.w	r0, r5, #1
 e001d26:	d00b      	beq.n	e001d40 <ota_get_cur_index+0x38>
 e001d28:	0a6d      	lsrs	r5, r5, #9
 e001d2a:	2c01      	cmp	r4, #1
 e001d2c:	ea4f 1545 	mov.w	r5, r5, lsl #5
 e001d30:	d01e      	beq.n	e001d70 <ota_get_cur_index+0x68>
 e001d32:	f5a5 5580 	sub.w	r5, r5, #4096	; 0x1000
 e001d36:	f856 0034 	ldr.w	r0, [r6, r4, lsl #3]
 e001d3a:	1b40      	subs	r0, r0, r5
 e001d3c:	bf18      	it	ne
 e001d3e:	2001      	movne	r0, #1
 e001d40:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e001d42:	463a      	mov	r2, r7
 e001d44:	4631      	mov	r1, r6
 e001d46:	4638      	mov	r0, r7
 e001d48:	f7ff fbe8 	bl	e00151c <flash_get_layout_info>
 e001d4c:	463a      	mov	r2, r7
 e001d4e:	1d31      	adds	r1, r6, #4
 e001d50:	2001      	movs	r0, #1
 e001d52:	f7ff fbe3 	bl	e00151c <flash_get_layout_info>
 e001d56:	463a      	mov	r2, r7
 e001d58:	f106 0108 	add.w	r1, r6, #8
 e001d5c:	2002      	movs	r0, #2
 e001d5e:	f7ff fbdd 	bl	e00151c <flash_get_layout_info>
 e001d62:	463a      	mov	r2, r7
 e001d64:	f106 010c 	add.w	r1, r6, #12
 e001d68:	2003      	movs	r0, #3
 e001d6a:	f7ff fbd7 	bl	e00151c <flash_get_layout_info>
 e001d6e:	e7d8      	b.n	e001d22 <ota_get_cur_index+0x1a>
 e001d70:	f5a5 5500 	sub.w	r5, r5, #8192	; 0x2000
 e001d74:	68b0      	ldr	r0, [r6, #8]
 e001d76:	1b40      	subs	r0, r0, r5
 e001d78:	bf18      	it	ne
 e001d7a:	2001      	movne	r0, #1
 e001d7c:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e001d7e:	bf00      	nop
 e001d80:	41104000 	.word	0x41104000
 e001d84:	2000b34c 	.word	0x2000b34c

0e001d88 <pmc_wakeuptimer_int_hdl>:
 e001d88:	4a03      	ldr	r2, [pc, #12]	; (e001d98 <pmc_wakeuptimer_int_hdl+0x10>)
 e001d8a:	2001      	movs	r0, #1
 e001d8c:	6893      	ldr	r3, [r2, #8]
 e001d8e:	f043 0304 	orr.w	r3, r3, #4
 e001d92:	6093      	str	r3, [r2, #8]
 e001d94:	4770      	bx	lr
 e001d96:	bf00      	nop
 e001d98:	41016000 	.word	0x41016000

0e001d9c <pmu_register_sleep_callback>:
 e001d9c:	2805      	cmp	r0, #5
 e001d9e:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 e001da2:	4605      	mov	r5, r0
 e001da4:	460f      	mov	r7, r1
 e001da6:	4690      	mov	r8, r2
 e001da8:	461e      	mov	r6, r3
 e001daa:	d810      	bhi.n	e001dce <pmu_register_sleep_callback+0x32>
 e001dac:	ea57 0306 	orrs.w	r3, r7, r6
 e001db0:	d014      	beq.n	e001ddc <pmu_register_sleep_callback+0x40>
 e001db2:	4c0d      	ldr	r4, [pc, #52]	; (e001de8 <pmu_register_sleep_callback+0x4c>)
 e001db4:	eb05 0085 	add.w	r0, r5, r5, lsl #2
 e001db8:	9b06      	ldr	r3, [sp, #24]
 e001dba:	f844 5020 	str.w	r5, [r4, r0, lsl #2]
 e001dbe:	eb04 0080 	add.w	r0, r4, r0, lsl #2
 e001dc2:	e9c0 7801 	strd	r7, r8, [r0, #4]
 e001dc6:	60c6      	str	r6, [r0, #12]
 e001dc8:	6103      	str	r3, [r0, #16]
 e001dca:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e001dce:	2158      	movs	r1, #88	; 0x58
 e001dd0:	4806      	ldr	r0, [pc, #24]	; (e001dec <pmu_register_sleep_callback+0x50>)
 e001dd2:	f005 facd 	bl	e007370 <io_assert_failed>
 e001dd6:	ea57 0306 	orrs.w	r3, r7, r6
 e001dda:	d1ea      	bne.n	e001db2 <pmu_register_sleep_callback+0x16>
 e001ddc:	2159      	movs	r1, #89	; 0x59
 e001dde:	4803      	ldr	r0, [pc, #12]	; (e001dec <pmu_register_sleep_callback+0x50>)
 e001de0:	f005 fac6 	bl	e007370 <io_assert_failed>
 e001de4:	e7e5      	b.n	e001db2 <pmu_register_sleep_callback+0x16>
 e001de6:	bf00      	nop
 e001de8:	2000b35c 	.word	0x2000b35c
 e001dec:	0e008190 	.word	0x0e008190

0e001df0 <pmu_yield_os_check>:
 e001df0:	4b01      	ldr	r3, [pc, #4]	; (e001df8 <pmu_yield_os_check+0x8>)
 e001df2:	6818      	ldr	r0, [r3, #0]
 e001df4:	4770      	bx	lr
 e001df6:	bf00      	nop
 e001df8:	2000b024 	.word	0x2000b024

0e001dfc <pmu_ready_to_sleep>:
 e001dfc:	4b06      	ldr	r3, [pc, #24]	; (e001e18 <pmu_ready_to_sleep+0x1c>)
 e001dfe:	6818      	ldr	r0, [r3, #0]
 e001e00:	f410 7080 	ands.w	r0, r0, #256	; 0x100
 e001e04:	d006      	beq.n	e001e14 <pmu_ready_to_sleep+0x18>
 e001e06:	4905      	ldr	r1, [pc, #20]	; (e001e1c <pmu_ready_to_sleep+0x20>)
 e001e08:	68da      	ldr	r2, [r3, #12]
 e001e0a:	680b      	ldr	r3, [r1, #0]
 e001e0c:	4313      	orrs	r3, r2
 e001e0e:	bf0c      	ite	eq
 e001e10:	2001      	moveq	r0, #1
 e001e12:	2000      	movne	r0, #0
 e001e14:	4770      	bx	lr
 e001e16:	bf00      	nop
 e001e18:	41016000 	.word	0x41016000
 e001e1c:	2000b028 	.word	0x2000b028

0e001e20 <pmu_set_sleep_type>:
 e001e20:	4602      	mov	r2, r0
 e001e22:	4b02      	ldr	r3, [pc, #8]	; (e001e2c <pmu_set_sleep_type+0xc>)
 e001e24:	2000      	movs	r0, #0
 e001e26:	601a      	str	r2, [r3, #0]
 e001e28:	4770      	bx	lr
 e001e2a:	bf00      	nop
 e001e2c:	2000b3d4 	.word	0x2000b3d4

0e001e30 <pmu_init_wakeup_timer>:
 e001e30:	2200      	movs	r2, #0
 e001e32:	2306      	movs	r3, #6
 e001e34:	2116      	movs	r1, #22
 e001e36:	4806      	ldr	r0, [pc, #24]	; (e001e50 <pmu_init_wakeup_timer+0x20>)
 e001e38:	b510      	push	{r4, lr}
 e001e3a:	4c06      	ldr	r4, [pc, #24]	; (e001e54 <pmu_init_wakeup_timer+0x24>)
 e001e3c:	47a0      	blx	r4
 e001e3e:	4b06      	ldr	r3, [pc, #24]	; (e001e58 <pmu_init_wakeup_timer+0x28>)
 e001e40:	2016      	movs	r0, #22
 e001e42:	4798      	blx	r3
 e001e44:	4a05      	ldr	r2, [pc, #20]	; (e001e5c <pmu_init_wakeup_timer+0x2c>)
 e001e46:	6853      	ldr	r3, [r2, #4]
 e001e48:	f043 0304 	orr.w	r3, r3, #4
 e001e4c:	6053      	str	r3, [r2, #4]
 e001e4e:	bd10      	pop	{r4, pc}
 e001e50:	0e001d89 	.word	0x0e001d89
 e001e54:	0000d769 	.word	0x0000d769
 e001e58:	0000d641 	.word	0x0000d641
 e001e5c:	41016000 	.word	0x41016000

0e001e60 <BOOT_Reason>:
 e001e60:	4b01      	ldr	r3, [pc, #4]	; (e001e68 <BOOT_Reason+0x8>)
 e001e62:	f8d3 0268 	ldr.w	r0, [r3, #616]	; 0x268
 e001e66:	4770      	bx	lr
 e001e68:	41008000 	.word	0x41008000

0e001e6c <System_Reset>:
 e001e6c:	4b0c      	ldr	r3, [pc, #48]	; (e001ea0 <System_Reset+0x34>)
 e001e6e:	f8d3 3228 	ldr.w	r3, [r3, #552]	; 0x228
 e001e72:	f3c3 1381 	ubfx	r3, r3, #6, #2
 e001e76:	2b01      	cmp	r3, #1
 e001e78:	d00e      	beq.n	e001e98 <System_Reset+0x2c>
 e001e7a:	b963      	cbnz	r3, e001e96 <System_Reset+0x2a>
 e001e7c:	f04f 4180 	mov.w	r1, #1073741824	; 0x40000000
 e001e80:	4b07      	ldr	r3, [pc, #28]	; (e001ea0 <System_Reset+0x34>)
 e001e82:	f04f 3096 	mov.w	r0, #2526451350	; 0x96969696
 e001e86:	f04f 3269 	mov.w	r2, #1768515945	; 0x69696969
 e001e8a:	f8c3 0264 	str.w	r0, [r3, #612]	; 0x264
 e001e8e:	f8c3 1260 	str.w	r1, [r3, #608]	; 0x260
 e001e92:	f8c3 2264 	str.w	r2, [r3, #612]	; 0x264
 e001e96:	4770      	bx	lr
 e001e98:	f04f 4100 	mov.w	r1, #2147483648	; 0x80000000
 e001e9c:	e7f0      	b.n	e001e80 <System_Reset+0x14>
 e001e9e:	bf00      	nop
 e001ea0:	41008000 	.word	0x41008000

0e001ea4 <RTCIO_IsEnabled>:
 e001ea4:	4b02      	ldr	r3, [pc, #8]	; (e001eb0 <RTCIO_IsEnabled+0xc>)
 e001ea6:	6c98      	ldr	r0, [r3, #72]	; 0x48
 e001ea8:	f3c0 1000 	ubfx	r0, r0, #4, #1
 e001eac:	4770      	bx	lr
 e001eae:	bf00      	nop
 e001eb0:	41008000 	.word	0x41008000

0e001eb4 <wifi_set_user_config>:
 e001eb4:	b538      	push	{r3, r4, r5, lr}
 e001eb6:	4c17      	ldr	r4, [pc, #92]	; (e001f14 <wifi_set_user_config+0x60>)
 e001eb8:	223c      	movs	r2, #60	; 0x3c
 e001eba:	2100      	movs	r1, #0
 e001ebc:	4d16      	ldr	r5, [pc, #88]	; (e001f18 <wifi_set_user_config+0x64>)
 e001ebe:	4620      	mov	r0, r4
 e001ec0:	f005 fda2 	bl	e007a08 <____wrap_memset_veneer>
 e001ec4:	f240 1301 	movw	r3, #257	; 0x101
 e001ec8:	2101      	movs	r1, #1
 e001eca:	2202      	movs	r2, #2
 e001ecc:	8263      	strh	r3, [r4, #18]
 e001ece:	2000      	movs	r0, #0
 e001ed0:	8563      	strh	r3, [r4, #42]	; 0x2a
 e001ed2:	230a      	movs	r3, #10
 e001ed4:	81a1      	strh	r1, [r4, #12]
 e001ed6:	7461      	strb	r1, [r4, #17]
 e001ed8:	210b      	movs	r1, #11
 e001eda:	73a2      	strb	r2, [r4, #14]
 e001edc:	61a2      	str	r2, [r4, #24]
 e001ede:	f44f 52a0 	mov.w	r2, #5120	; 0x1400
 e001ee2:	6223      	str	r3, [r4, #32]
 e001ee4:	2304      	movs	r3, #4
 e001ee6:	6025      	str	r5, [r4, #0]
 e001ee8:	f04f 65a1 	mov.w	r5, #84410368	; 0x5080000
 e001eec:	7120      	strb	r0, [r4, #4]
 e001eee:	f884 0029 	strb.w	r0, [r4, #41]	; 0x29
 e001ef2:	6161      	str	r1, [r4, #20]
 e001ef4:	f241 3188 	movw	r1, #5000	; 0x1388
 e001ef8:	4808      	ldr	r0, [pc, #32]	; (e001f1c <wifi_set_user_config+0x68>)
 e001efa:	8722      	strh	r2, [r4, #56]	; 0x38
 e001efc:	f240 720f 	movw	r2, #1807	; 0x70f
 e001f00:	6263      	str	r3, [r4, #36]	; 0x24
 e001f02:	f884 3030 	strb.w	r3, [r4, #48]	; 0x30
 e001f06:	4b06      	ldr	r3, [pc, #24]	; (e001f20 <wifi_set_user_config+0x6c>)
 e001f08:	61e5      	str	r5, [r4, #28]
 e001f0a:	62e0      	str	r0, [r4, #44]	; 0x2c
 e001f0c:	6361      	str	r1, [r4, #52]	; 0x34
 e001f0e:	80e2      	strh	r2, [r4, #6]
 e001f10:	60a3      	str	r3, [r4, #8]
 e001f12:	bd38      	pop	{r3, r4, r5, pc}
 e001f14:	2000b400 	.word	0x2000b400
 e001f18:	00020200 	.word	0x00020200
 e001f1c:	00040101 	.word	0x00040101
 e001f20:	01000001 	.word	0x01000001

0e001f24 <fATSC>:
 e001f24:	b508      	push	{r3, lr}
 e001f26:	4b0a      	ldr	r3, [pc, #40]	; (e001f50 <fATSC+0x2c>)
 e001f28:	681b      	ldr	r3, [r3, #0]
 e001f2a:	06db      	lsls	r3, r3, #27
 e001f2c:	d502      	bpl.n	e001f34 <fATSC+0x10>
 e001f2e:	4b09      	ldr	r3, [pc, #36]	; (e001f54 <fATSC+0x30>)
 e001f30:	781b      	ldrb	r3, [r3, #0]
 e001f32:	b91b      	cbnz	r3, e001f3c <fATSC+0x18>
 e001f34:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
 e001f38:	f001 bf6c 	b.w	e003e14 <sys_clear_ota_signature>
 e001f3c:	4806      	ldr	r0, [pc, #24]	; (e001f58 <fATSC+0x34>)
 e001f3e:	f005 f863 	bl	e007008 <DiagPrintf>
 e001f42:	4806      	ldr	r0, [pc, #24]	; (e001f5c <fATSC+0x38>)
 e001f44:	f005 f860 	bl	e007008 <DiagPrintf>
 e001f48:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
 e001f4c:	f001 bf62 	b.w	e003e14 <sys_clear_ota_signature>
 e001f50:	2000b050 	.word	0x2000b050
 e001f54:	2000b054 	.word	0x2000b054
 e001f58:	0e0081bc 	.word	0x0e0081bc
 e001f5c:	0e0082dc 	.word	0x0e0082dc

0e001f60 <fATSR>:
 e001f60:	b508      	push	{r3, lr}
 e001f62:	4b0a      	ldr	r3, [pc, #40]	; (e001f8c <fATSR+0x2c>)
 e001f64:	681b      	ldr	r3, [r3, #0]
 e001f66:	06db      	lsls	r3, r3, #27
 e001f68:	d502      	bpl.n	e001f70 <fATSR+0x10>
 e001f6a:	4b09      	ldr	r3, [pc, #36]	; (e001f90 <fATSR+0x30>)
 e001f6c:	781b      	ldrb	r3, [r3, #0]
 e001f6e:	b91b      	cbnz	r3, e001f78 <fATSR+0x18>
 e001f70:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
 e001f74:	f001 bfd0 	b.w	e003f18 <sys_recover_ota_signature>
 e001f78:	4806      	ldr	r0, [pc, #24]	; (e001f94 <fATSR+0x34>)
 e001f7a:	f005 f845 	bl	e007008 <DiagPrintf>
 e001f7e:	4806      	ldr	r0, [pc, #24]	; (e001f98 <fATSR+0x38>)
 e001f80:	f005 f842 	bl	e007008 <DiagPrintf>
 e001f84:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
 e001f88:	f001 bfc6 	b.w	e003f18 <sys_recover_ota_signature>
 e001f8c:	2000b050 	.word	0x2000b050
 e001f90:	2000b054 	.word	0x2000b054
 e001f94:	0e0081e4 	.word	0x0e0081e4
 e001f98:	0e0082dc 	.word	0x0e0082dc

0e001f9c <at_sys_init>:
 e001f9c:	2102      	movs	r1, #2
 e001f9e:	4801      	ldr	r0, [pc, #4]	; (e001fa4 <at_sys_init+0x8>)
 e001fa0:	f000 bab0 	b.w	e002504 <log_service_add_table>
 e001fa4:	2000b02c 	.word	0x2000b02c

0e001fa8 <flash_read_word>:
 e001fa8:	b508      	push	{r3, lr}
 e001faa:	4608      	mov	r0, r1
 e001fac:	4b02      	ldr	r3, [pc, #8]	; (e001fb8 <flash_read_word+0x10>)
 e001fae:	2104      	movs	r1, #4
 e001fb0:	4798      	blx	r3
 e001fb2:	2001      	movs	r0, #1
 e001fb4:	bd08      	pop	{r3, pc}
 e001fb6:	bf00      	nop
 e001fb8:	0e0016d9 	.word	0x0e0016d9

0e001fbc <os_heap_init>:
 e001fbc:	b580      	push	{r7, lr}
 e001fbe:	af00      	add	r7, sp, #0
 e001fc0:	4b0c      	ldr	r3, [pc, #48]	; (e001ff4 <os_heap_init+0x38>)
 e001fc2:	4619      	mov	r1, r3
 e001fc4:	480c      	ldr	r0, [pc, #48]	; (e001ff8 <os_heap_init+0x3c>)
 e001fc6:	f000 f84d 	bl	e002064 <os_heap_add>
 e001fca:	4b0c      	ldr	r3, [pc, #48]	; (e001ffc <os_heap_init+0x40>)
 e001fcc:	4619      	mov	r1, r3
 e001fce:	480c      	ldr	r0, [pc, #48]	; (e002000 <os_heap_init+0x44>)
 e001fd0:	f000 f848 	bl	e002064 <os_heap_add>
 e001fd4:	4b0b      	ldr	r3, [pc, #44]	; (e002004 <os_heap_init+0x48>)
 e001fd6:	4619      	mov	r1, r3
 e001fd8:	480b      	ldr	r0, [pc, #44]	; (e002008 <os_heap_init+0x4c>)
 e001fda:	f000 f843 	bl	e002064 <os_heap_add>
 e001fde:	4b0b      	ldr	r3, [pc, #44]	; (e00200c <os_heap_init+0x50>)
 e001fe0:	781b      	ldrb	r3, [r3, #0]
 e001fe2:	2b00      	cmp	r3, #0
 e001fe4:	d001      	beq.n	e001fea <os_heap_init+0x2e>
 e001fe6:	f000 f815 	bl	e002014 <os_heap_sort>
 e001fea:	4809      	ldr	r0, [pc, #36]	; (e002010 <os_heap_init+0x54>)
 e001fec:	f000 f92a 	bl	e002244 <vPortDefineHeapRegions>
 e001ff0:	bf00      	nop
 e001ff2:	bd80      	pop	{r7, pc}
 e001ff4:	0005bf00 	.word	0x0005bf00
 e001ff8:	2000c100 	.word	0x2000c100
 e001ffc:	00000000 	.word	0x00000000
 e002000:	c8800000 	.word	0xc8800000
 e002004:	00000000 	.word	0x00000000
 e002008:	c8800000 	.word	0xc8800000
 e00200c:	2000b46c 	.word	0x2000b46c
 e002010:	2000b43c 	.word	0x2000b43c

0e002014 <os_heap_sort>:
 e002014:	4b11      	ldr	r3, [pc, #68]	; (e00205c <os_heap_sort+0x48>)
 e002016:	b570      	push	{r4, r5, r6, lr}
 e002018:	f893 e000 	ldrb.w	lr, [r3]
 e00201c:	f10e 3eff 	add.w	lr, lr, #4294967295	; 0xffffffff
 e002020:	f1be 0f00 	cmp.w	lr, #0
 e002024:	dd18      	ble.n	e002058 <os_heap_sort+0x44>
 e002026:	4e0e      	ldr	r6, [pc, #56]	; (e002060 <os_heap_sort+0x4c>)
 e002028:	eb06 0cce 	add.w	ip, r6, lr, lsl #3
 e00202c:	4633      	mov	r3, r6
 e00202e:	681c      	ldr	r4, [r3, #0]
 e002030:	f103 0208 	add.w	r2, r3, #8
 e002034:	6899      	ldr	r1, [r3, #8]
 e002036:	428c      	cmp	r4, r1
 e002038:	d906      	bls.n	e002048 <os_heap_sort+0x34>
 e00203a:	685d      	ldr	r5, [r3, #4]
 e00203c:	e892 0003 	ldmia.w	r2, {r0, r1}
 e002040:	609c      	str	r4, [r3, #8]
 e002042:	60dd      	str	r5, [r3, #12]
 e002044:	e883 0003 	stmia.w	r3, {r0, r1}
 e002048:	4562      	cmp	r2, ip
 e00204a:	4613      	mov	r3, r2
 e00204c:	d1ef      	bne.n	e00202e <os_heap_sort+0x1a>
 e00204e:	f1be 0e01 	subs.w	lr, lr, #1
 e002052:	f1ac 0c08 	sub.w	ip, ip, #8
 e002056:	d1e9      	bne.n	e00202c <os_heap_sort+0x18>
 e002058:	bd70      	pop	{r4, r5, r6, pc}
 e00205a:	bf00      	nop
 e00205c:	2000b46c 	.word	0x2000b46c
 e002060:	2000b43c 	.word	0x2000b43c

0e002064 <os_heap_add>:
 e002064:	b570      	push	{r4, r5, r6, lr}
 e002066:	4e0c      	ldr	r6, [pc, #48]	; (e002098 <os_heap_add+0x34>)
 e002068:	4605      	mov	r5, r0
 e00206a:	460c      	mov	r4, r1
 e00206c:	7833      	ldrb	r3, [r6, #0]
 e00206e:	2b05      	cmp	r3, #5
 e002070:	d80c      	bhi.n	e00208c <os_heap_add+0x28>
 e002072:	4a0a      	ldr	r2, [pc, #40]	; (e00209c <os_heap_add+0x38>)
 e002074:	eb02 01c3 	add.w	r1, r2, r3, lsl #3
 e002078:	f842 5033 	str.w	r5, [r2, r3, lsl #3]
 e00207c:	604c      	str	r4, [r1, #4]
 e00207e:	b90c      	cbnz	r4, e002084 <os_heap_add+0x20>
 e002080:	4620      	mov	r0, r4
 e002082:	bd70      	pop	{r4, r5, r6, pc}
 e002084:	3301      	adds	r3, #1
 e002086:	2001      	movs	r0, #1
 e002088:	7033      	strb	r3, [r6, #0]
 e00208a:	bd70      	pop	{r4, r5, r6, pc}
 e00208c:	2125      	movs	r1, #37	; 0x25
 e00208e:	4804      	ldr	r0, [pc, #16]	; (e0020a0 <os_heap_add+0x3c>)
 e002090:	f005 f96e 	bl	e007370 <io_assert_failed>
 e002094:	7833      	ldrb	r3, [r6, #0]
 e002096:	e7ec      	b.n	e002072 <os_heap_add+0xe>
 e002098:	2000b46c 	.word	0x2000b46c
 e00209c:	2000b43c 	.word	0x2000b43c
 e0020a0:	0e008220 	.word	0x0e008220

0e0020a4 <__wrap_malloc>:
 e0020a4:	f000 bd20 	b.w	e002ae8 <rtos_mem_malloc>

0e0020a8 <__wrap_free>:
 e0020a8:	f000 bd2c 	b.w	e002b04 <rtos_mem_free>

0e0020ac <__wrap__free_r>:
 e0020ac:	4608      	mov	r0, r1
 e0020ae:	f000 bd29 	b.w	e002b04 <rtos_mem_free>
 e0020b2:	bf00      	nop

0e0020b4 <prvInsertBlockIntoFreeList>:
 e0020b4:	4b15      	ldr	r3, [pc, #84]	; (e00210c <prvInsertBlockIntoFreeList+0x58>)
 e0020b6:	b410      	push	{r4}
 e0020b8:	461a      	mov	r2, r3
 e0020ba:	681b      	ldr	r3, [r3, #0]
 e0020bc:	4283      	cmp	r3, r0
 e0020be:	d3fb      	bcc.n	e0020b8 <prvInsertBlockIntoFreeList+0x4>
 e0020c0:	6854      	ldr	r4, [r2, #4]
 e0020c2:	6841      	ldr	r1, [r0, #4]
 e0020c4:	eb02 0c04 	add.w	ip, r2, r4
 e0020c8:	4560      	cmp	r0, ip
 e0020ca:	d009      	beq.n	e0020e0 <prvInsertBlockIntoFreeList+0x2c>
 e0020cc:	1844      	adds	r4, r0, r1
 e0020ce:	42a3      	cmp	r3, r4
 e0020d0:	d00c      	beq.n	e0020ec <prvInsertBlockIntoFreeList+0x38>
 e0020d2:	4290      	cmp	r0, r2
 e0020d4:	6003      	str	r3, [r0, #0]
 e0020d6:	bf18      	it	ne
 e0020d8:	6010      	strne	r0, [r2, #0]
 e0020da:	f85d 4b04 	ldr.w	r4, [sp], #4
 e0020de:	4770      	bx	lr
 e0020e0:	4421      	add	r1, r4
 e0020e2:	4610      	mov	r0, r2
 e0020e4:	1844      	adds	r4, r0, r1
 e0020e6:	6051      	str	r1, [r2, #4]
 e0020e8:	42a3      	cmp	r3, r4
 e0020ea:	d1f2      	bne.n	e0020d2 <prvInsertBlockIntoFreeList+0x1e>
 e0020ec:	4c08      	ldr	r4, [pc, #32]	; (e002110 <prvInsertBlockIntoFreeList+0x5c>)
 e0020ee:	6824      	ldr	r4, [r4, #0]
 e0020f0:	42a3      	cmp	r3, r4
 e0020f2:	d0ee      	beq.n	e0020d2 <prvInsertBlockIntoFreeList+0x1e>
 e0020f4:	685b      	ldr	r3, [r3, #4]
 e0020f6:	4290      	cmp	r0, r2
 e0020f8:	4419      	add	r1, r3
 e0020fa:	6041      	str	r1, [r0, #4]
 e0020fc:	6813      	ldr	r3, [r2, #0]
 e0020fe:	681b      	ldr	r3, [r3, #0]
 e002100:	6003      	str	r3, [r0, #0]
 e002102:	bf18      	it	ne
 e002104:	6010      	strne	r0, [r2, #0]
 e002106:	f85d 4b04 	ldr.w	r4, [sp], #4
 e00210a:	4770      	bx	lr
 e00210c:	2000b488 	.word	0x2000b488
 e002110:	2000b470 	.word	0x2000b470

0e002114 <pvPortMalloc>:
 e002114:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 e002118:	4604      	mov	r4, r0
 e00211a:	f002 faf7 	bl	e00470c <vTaskSuspendAll>
 e00211e:	4b29      	ldr	r3, [pc, #164]	; (e0021c4 <pvPortMalloc+0xb0>)
 e002120:	681e      	ldr	r6, [r3, #0]
 e002122:	4226      	tst	r6, r4
 e002124:	d13c      	bne.n	e0021a0 <pvPortMalloc+0x8c>
 e002126:	1e63      	subs	r3, r4, #1
 e002128:	3322      	adds	r3, #34	; 0x22
 e00212a:	d839      	bhi.n	e0021a0 <pvPortMalloc+0x8c>
 e00212c:	06e3      	lsls	r3, r4, #27
 e00212e:	f104 0220 	add.w	r2, r4, #32
 e002132:	d130      	bne.n	e002196 <pvPortMalloc+0x82>
 e002134:	f8df 80a0 	ldr.w	r8, [pc, #160]	; e0021d8 <pvPortMalloc+0xc4>
 e002138:	f8d8 7000 	ldr.w	r7, [r8]
 e00213c:	4297      	cmp	r7, r2
 e00213e:	d32f      	bcc.n	e0021a0 <pvPortMalloc+0x8c>
 e002140:	4921      	ldr	r1, [pc, #132]	; (e0021c8 <pvPortMalloc+0xb4>)
 e002142:	680c      	ldr	r4, [r1, #0]
 e002144:	e003      	b.n	e00214e <pvPortMalloc+0x3a>
 e002146:	6823      	ldr	r3, [r4, #0]
 e002148:	b123      	cbz	r3, e002154 <pvPortMalloc+0x40>
 e00214a:	4621      	mov	r1, r4
 e00214c:	461c      	mov	r4, r3
 e00214e:	6863      	ldr	r3, [r4, #4]
 e002150:	4293      	cmp	r3, r2
 e002152:	d3f8      	bcc.n	e002146 <pvPortMalloc+0x32>
 e002154:	4b1d      	ldr	r3, [pc, #116]	; (e0021cc <pvPortMalloc+0xb8>)
 e002156:	681b      	ldr	r3, [r3, #0]
 e002158:	42a3      	cmp	r3, r4
 e00215a:	d021      	beq.n	e0021a0 <pvPortMalloc+0x8c>
 e00215c:	6823      	ldr	r3, [r4, #0]
 e00215e:	680d      	ldr	r5, [r1, #0]
 e002160:	600b      	str	r3, [r1, #0]
 e002162:	6863      	ldr	r3, [r4, #4]
 e002164:	3520      	adds	r5, #32
 e002166:	1a99      	subs	r1, r3, r2
 e002168:	2940      	cmp	r1, #64	; 0x40
 e00216a:	d823      	bhi.n	e0021b4 <pvPortMalloc+0xa0>
 e00216c:	4a18      	ldr	r2, [pc, #96]	; (e0021d0 <pvPortMalloc+0xbc>)
 e00216e:	1aff      	subs	r7, r7, r3
 e002170:	4333      	orrs	r3, r6
 e002172:	6811      	ldr	r1, [r2, #0]
 e002174:	f8c8 7000 	str.w	r7, [r8]
 e002178:	428f      	cmp	r7, r1
 e00217a:	f04f 0100 	mov.w	r1, #0
 e00217e:	bf38      	it	cc
 e002180:	6017      	strcc	r7, [r2, #0]
 e002182:	4a14      	ldr	r2, [pc, #80]	; (e0021d4 <pvPortMalloc+0xc0>)
 e002184:	e9c4 1300 	strd	r1, r3, [r4]
 e002188:	6813      	ldr	r3, [r2, #0]
 e00218a:	3301      	adds	r3, #1
 e00218c:	6013      	str	r3, [r2, #0]
 e00218e:	f002 fac5 	bl	e00471c <xTaskResumeAll>
 e002192:	b955      	cbnz	r5, e0021aa <pvPortMalloc+0x96>
 e002194:	e006      	b.n	e0021a4 <pvPortMalloc+0x90>
 e002196:	f022 031f 	bic.w	r3, r2, #31
 e00219a:	3320      	adds	r3, #32
 e00219c:	4293      	cmp	r3, r2
 e00219e:	d807      	bhi.n	e0021b0 <pvPortMalloc+0x9c>
 e0021a0:	f002 fabc 	bl	e00471c <xTaskResumeAll>
 e0021a4:	2500      	movs	r5, #0
 e0021a6:	f001 f827 	bl	e0031f8 <vApplicationMallocFailedHook>
 e0021aa:	4628      	mov	r0, r5
 e0021ac:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e0021b0:	461a      	mov	r2, r3
 e0021b2:	e7bf      	b.n	e002134 <pvPortMalloc+0x20>
 e0021b4:	18a0      	adds	r0, r4, r2
 e0021b6:	6041      	str	r1, [r0, #4]
 e0021b8:	6062      	str	r2, [r4, #4]
 e0021ba:	f7ff ff7b 	bl	e0020b4 <prvInsertBlockIntoFreeList>
 e0021be:	6863      	ldr	r3, [r4, #4]
 e0021c0:	e7d4      	b.n	e00216c <pvPortMalloc+0x58>
 e0021c2:	bf00      	nop
 e0021c4:	2000b474 	.word	0x2000b474
 e0021c8:	2000b488 	.word	0x2000b488
 e0021cc:	2000b470 	.word	0x2000b470
 e0021d0:	2000b47c 	.word	0x2000b47c
 e0021d4:	2000b480 	.word	0x2000b480
 e0021d8:	2000b478 	.word	0x2000b478

0e0021dc <vPortFree>:
 e0021dc:	b140      	cbz	r0, e0021f0 <vPortFree+0x14>
 e0021de:	4a13      	ldr	r2, [pc, #76]	; (e00222c <vPortFree+0x50>)
 e0021e0:	f850 3c1c 	ldr.w	r3, [r0, #-28]
 e0021e4:	6812      	ldr	r2, [r2, #0]
 e0021e6:	4213      	tst	r3, r2
 e0021e8:	d002      	beq.n	e0021f0 <vPortFree+0x14>
 e0021ea:	f850 1c20 	ldr.w	r1, [r0, #-32]
 e0021ee:	b101      	cbz	r1, e0021f2 <vPortFree+0x16>
 e0021f0:	4770      	bx	lr
 e0021f2:	ea23 0302 	bic.w	r3, r3, r2
 e0021f6:	b500      	push	{lr}
 e0021f8:	b083      	sub	sp, #12
 e0021fa:	f840 3c1c 	str.w	r3, [r0, #-28]
 e0021fe:	9001      	str	r0, [sp, #4]
 e002200:	f002 fa84 	bl	e00470c <vTaskSuspendAll>
 e002204:	4a0a      	ldr	r2, [pc, #40]	; (e002230 <vPortFree+0x54>)
 e002206:	9801      	ldr	r0, [sp, #4]
 e002208:	6811      	ldr	r1, [r2, #0]
 e00220a:	f850 3c1c 	ldr.w	r3, [r0, #-28]
 e00220e:	3820      	subs	r0, #32
 e002210:	440b      	add	r3, r1
 e002212:	6013      	str	r3, [r2, #0]
 e002214:	f7ff ff4e 	bl	e0020b4 <prvInsertBlockIntoFreeList>
 e002218:	4a06      	ldr	r2, [pc, #24]	; (e002234 <vPortFree+0x58>)
 e00221a:	6813      	ldr	r3, [r2, #0]
 e00221c:	3301      	adds	r3, #1
 e00221e:	6013      	str	r3, [r2, #0]
 e002220:	b003      	add	sp, #12
 e002222:	f85d eb04 	ldr.w	lr, [sp], #4
 e002226:	f002 ba79 	b.w	e00471c <xTaskResumeAll>
 e00222a:	bf00      	nop
 e00222c:	2000b474 	.word	0x2000b474
 e002230:	2000b478 	.word	0x2000b478
 e002234:	2000b484 	.word	0x2000b484

0e002238 <xPortGetFreeHeapSize>:
 e002238:	4b01      	ldr	r3, [pc, #4]	; (e002240 <xPortGetFreeHeapSize+0x8>)
 e00223a:	6818      	ldr	r0, [r3, #0]
 e00223c:	4770      	bx	lr
 e00223e:	bf00      	nop
 e002240:	2000b478 	.word	0x2000b478

0e002244 <vPortDefineHeapRegions>:
 e002244:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 e002248:	6844      	ldr	r4, [r0, #4]
 e00224a:	2c00      	cmp	r4, #0
 e00224c:	d03e      	beq.n	e0022cc <vPortDefineHeapRegions+0x88>
 e00224e:	2100      	movs	r1, #0
 e002250:	f8df 9088 	ldr.w	r9, [pc, #136]	; e0022dc <vPortDefineHeapRegions+0x98>
 e002254:	f100 0804 	add.w	r8, r0, #4
 e002258:	f8df e084 	ldr.w	lr, [pc, #132]	; e0022e0 <vPortDefineHeapRegions+0x9c>
 e00225c:	f8d9 5000 	ldr.w	r5, [r9]
 e002260:	460e      	mov	r6, r1
 e002262:	460f      	mov	r7, r1
 e002264:	f850 3031 	ldr.w	r3, [r0, r1, lsl #3]
 e002268:	f103 021f 	add.w	r2, r3, #31
 e00226c:	f013 0f1f 	tst.w	r3, #31
 e002270:	eb03 0a04 	add.w	sl, r3, r4
 e002274:	469c      	mov	ip, r3
 e002276:	f022 021f 	bic.w	r2, r2, #31
 e00227a:	d003      	beq.n	e002284 <vPortDefineHeapRegions+0x40>
 e00227c:	4613      	mov	r3, r2
 e00227e:	ebaa 0402 	sub.w	r4, sl, r2
 e002282:	4694      	mov	ip, r2
 e002284:	b909      	cbnz	r1, e00228a <vPortDefineHeapRegions+0x46>
 e002286:	e9ce 3100 	strd	r3, r1, [lr]
 e00228a:	eb04 020c 	add.w	r2, r4, ip
 e00228e:	3101      	adds	r1, #1
 e002290:	3a20      	subs	r2, #32
 e002292:	f022 021f 	bic.w	r2, r2, #31
 e002296:	eba2 040c 	sub.w	r4, r2, ip
 e00229a:	e9c2 7700 	strd	r7, r7, [r2]
 e00229e:	e9c3 2400 	strd	r2, r4, [r3]
 e0022a2:	b105      	cbz	r5, e0022a6 <vPortDefineHeapRegions+0x62>
 e0022a4:	602b      	str	r3, [r5, #0]
 e0022a6:	685b      	ldr	r3, [r3, #4]
 e0022a8:	4615      	mov	r5, r2
 e0022aa:	f858 4031 	ldr.w	r4, [r8, r1, lsl #3]
 e0022ae:	441e      	add	r6, r3
 e0022b0:	2c00      	cmp	r4, #0
 e0022b2:	d1d7      	bne.n	e002264 <vPortDefineHeapRegions+0x20>
 e0022b4:	f8c9 2000 	str.w	r2, [r9]
 e0022b8:	4805      	ldr	r0, [pc, #20]	; (e0022d0 <vPortDefineHeapRegions+0x8c>)
 e0022ba:	f04f 4200 	mov.w	r2, #2147483648	; 0x80000000
 e0022be:	4905      	ldr	r1, [pc, #20]	; (e0022d4 <vPortDefineHeapRegions+0x90>)
 e0022c0:	4b05      	ldr	r3, [pc, #20]	; (e0022d8 <vPortDefineHeapRegions+0x94>)
 e0022c2:	6006      	str	r6, [r0, #0]
 e0022c4:	600e      	str	r6, [r1, #0]
 e0022c6:	601a      	str	r2, [r3, #0]
 e0022c8:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
 e0022cc:	4626      	mov	r6, r4
 e0022ce:	e7f3      	b.n	e0022b8 <vPortDefineHeapRegions+0x74>
 e0022d0:	2000b47c 	.word	0x2000b47c
 e0022d4:	2000b478 	.word	0x2000b478
 e0022d8:	2000b474 	.word	0x2000b474
 e0022dc:	2000b470 	.word	0x2000b470
 e0022e0:	2000b488 	.word	0x2000b488

0e0022e4 <vListInitialise>:
 e0022e4:	f100 0308 	add.w	r3, r0, #8
 e0022e8:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
 e0022ec:	2200      	movs	r2, #0
 e0022ee:	6081      	str	r1, [r0, #8]
 e0022f0:	6002      	str	r2, [r0, #0]
 e0022f2:	6043      	str	r3, [r0, #4]
 e0022f4:	e9c0 3303 	strd	r3, r3, [r0, #12]
 e0022f8:	4770      	bx	lr
 e0022fa:	bf00      	nop

0e0022fc <vListInitialiseItem>:
 e0022fc:	2300      	movs	r3, #0
 e0022fe:	6103      	str	r3, [r0, #16]
 e002300:	4770      	bx	lr
 e002302:	bf00      	nop

0e002304 <vListInsertEnd>:
 e002304:	6843      	ldr	r3, [r0, #4]
 e002306:	604b      	str	r3, [r1, #4]
 e002308:	689a      	ldr	r2, [r3, #8]
 e00230a:	608a      	str	r2, [r1, #8]
 e00230c:	6051      	str	r1, [r2, #4]
 e00230e:	6099      	str	r1, [r3, #8]
 e002310:	6108      	str	r0, [r1, #16]
 e002312:	6803      	ldr	r3, [r0, #0]
 e002314:	3301      	adds	r3, #1
 e002316:	6003      	str	r3, [r0, #0]
 e002318:	4770      	bx	lr
 e00231a:	bf00      	nop

0e00231c <vListInsert>:
 e00231c:	b430      	push	{r4, r5}
 e00231e:	680d      	ldr	r5, [r1, #0]
 e002320:	1c6b      	adds	r3, r5, #1
 e002322:	d010      	beq.n	e002346 <vListInsert+0x2a>
 e002324:	f100 0308 	add.w	r3, r0, #8
 e002328:	461c      	mov	r4, r3
 e00232a:	685b      	ldr	r3, [r3, #4]
 e00232c:	681a      	ldr	r2, [r3, #0]
 e00232e:	42aa      	cmp	r2, r5
 e002330:	d9fa      	bls.n	e002328 <vListInsert+0xc>
 e002332:	604b      	str	r3, [r1, #4]
 e002334:	6099      	str	r1, [r3, #8]
 e002336:	608c      	str	r4, [r1, #8]
 e002338:	6061      	str	r1, [r4, #4]
 e00233a:	6108      	str	r0, [r1, #16]
 e00233c:	6803      	ldr	r3, [r0, #0]
 e00233e:	3301      	adds	r3, #1
 e002340:	bc30      	pop	{r4, r5}
 e002342:	6003      	str	r3, [r0, #0]
 e002344:	4770      	bx	lr
 e002346:	6904      	ldr	r4, [r0, #16]
 e002348:	6863      	ldr	r3, [r4, #4]
 e00234a:	e7f2      	b.n	e002332 <vListInsert+0x16>

0e00234c <uxListRemove>:
 e00234c:	e9d0 1201 	ldrd	r1, r2, [r0, #4]
 e002350:	6903      	ldr	r3, [r0, #16]
 e002352:	608a      	str	r2, [r1, #8]
 e002354:	6841      	ldr	r1, [r0, #4]
 e002356:	6051      	str	r1, [r2, #4]
 e002358:	685a      	ldr	r2, [r3, #4]
 e00235a:	4282      	cmp	r2, r0
 e00235c:	d006      	beq.n	e00236c <uxListRemove+0x20>
 e00235e:	2200      	movs	r2, #0
 e002360:	6102      	str	r2, [r0, #16]
 e002362:	681a      	ldr	r2, [r3, #0]
 e002364:	3a01      	subs	r2, #1
 e002366:	601a      	str	r2, [r3, #0]
 e002368:	6818      	ldr	r0, [r3, #0]
 e00236a:	4770      	bx	lr
 e00236c:	6882      	ldr	r2, [r0, #8]
 e00236e:	605a      	str	r2, [r3, #4]
 e002370:	e7f5      	b.n	e00235e <uxListRemove+0x12>
 e002372:	bf00      	nop

0e002374 <rtk_log_memory_dump_word>:
 e002374:	b321      	cbz	r1, e0023c0 <rtk_log_memory_dump_word+0x4c>
 e002376:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 e00237a:	4680      	mov	r8, r0
 e00237c:	460e      	mov	r6, r1
 e00237e:	4605      	mov	r5, r0
 e002380:	2400      	movs	r4, #0
 e002382:	f8df 904c 	ldr.w	r9, [pc, #76]	; e0023d0 <rtk_log_memory_dump_word+0x5c>
 e002386:	f8df a04c 	ldr.w	sl, [pc, #76]	; e0023d4 <rtk_log_memory_dump_word+0x60>
 e00238a:	4f0f      	ldr	r7, [pc, #60]	; (e0023c8 <rtk_log_memory_dump_word+0x54>)
 e00238c:	b19c      	cbz	r4, e0023b6 <rtk_log_memory_dump_word+0x42>
 e00238e:	0763      	lsls	r3, r4, #29
 e002390:	d00c      	beq.n	e0023ac <rtk_log_memory_dump_word+0x38>
 e002392:	3401      	adds	r4, #1
 e002394:	f855 1b04 	ldr.w	r1, [r5], #4
 e002398:	4638      	mov	r0, r7
 e00239a:	f004 fe43 	bl	e007024 <DiagPrintf_minimal>
 e00239e:	42a6      	cmp	r6, r4
 e0023a0:	d1f4      	bne.n	e00238c <rtk_log_memory_dump_word+0x18>
 e0023a2:	480a      	ldr	r0, [pc, #40]	; (e0023cc <rtk_log_memory_dump_word+0x58>)
 e0023a4:	e8bd 47f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 e0023a8:	f004 be3c 	b.w	e007024 <DiagPrintf_minimal>
 e0023ac:	4629      	mov	r1, r5
 e0023ae:	4648      	mov	r0, r9
 e0023b0:	f004 fe38 	bl	e007024 <DiagPrintf_minimal>
 e0023b4:	e7ed      	b.n	e002392 <rtk_log_memory_dump_word+0x1e>
 e0023b6:	4641      	mov	r1, r8
 e0023b8:	4650      	mov	r0, sl
 e0023ba:	f004 fe33 	bl	e007024 <DiagPrintf_minimal>
 e0023be:	e7e8      	b.n	e002392 <rtk_log_memory_dump_word+0x1e>
 e0023c0:	4802      	ldr	r0, [pc, #8]	; (e0023cc <rtk_log_memory_dump_word+0x58>)
 e0023c2:	f004 be2f 	b.w	e007024 <DiagPrintf_minimal>
 e0023c6:	bf00      	nop
 e0023c8:	0e008264 	.word	0x0e008264
 e0023cc:	0e0088e0 	.word	0x0e0088e0
 e0023d0:	0e008258 	.word	0x0e008258
 e0023d4:	0e008250 	.word	0x0e008250

0e0023d8 <rtk_log_memory_dump_byte>:
 e0023d8:	b321      	cbz	r1, e002424 <rtk_log_memory_dump_byte+0x4c>
 e0023da:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 e0023de:	4680      	mov	r8, r0
 e0023e0:	460e      	mov	r6, r1
 e0023e2:	4605      	mov	r5, r0
 e0023e4:	2400      	movs	r4, #0
 e0023e6:	f8df 904c 	ldr.w	r9, [pc, #76]	; e002434 <rtk_log_memory_dump_byte+0x5c>
 e0023ea:	f8df a04c 	ldr.w	sl, [pc, #76]	; e002438 <rtk_log_memory_dump_byte+0x60>
 e0023ee:	4f0f      	ldr	r7, [pc, #60]	; (e00242c <rtk_log_memory_dump_byte+0x54>)
 e0023f0:	b19c      	cbz	r4, e00241a <rtk_log_memory_dump_byte+0x42>
 e0023f2:	0763      	lsls	r3, r4, #29
 e0023f4:	d00c      	beq.n	e002410 <rtk_log_memory_dump_byte+0x38>
 e0023f6:	3401      	adds	r4, #1
 e0023f8:	f815 1b01 	ldrb.w	r1, [r5], #1
 e0023fc:	4638      	mov	r0, r7
 e0023fe:	f004 fe11 	bl	e007024 <DiagPrintf_minimal>
 e002402:	42a6      	cmp	r6, r4
 e002404:	d1f4      	bne.n	e0023f0 <rtk_log_memory_dump_byte+0x18>
 e002406:	480a      	ldr	r0, [pc, #40]	; (e002430 <rtk_log_memory_dump_byte+0x58>)
 e002408:	e8bd 47f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 e00240c:	f004 be0a 	b.w	e007024 <DiagPrintf_minimal>
 e002410:	4629      	mov	r1, r5
 e002412:	4648      	mov	r0, r9
 e002414:	f004 fe06 	bl	e007024 <DiagPrintf_minimal>
 e002418:	e7ed      	b.n	e0023f6 <rtk_log_memory_dump_byte+0x1e>
 e00241a:	4641      	mov	r1, r8
 e00241c:	4650      	mov	r0, sl
 e00241e:	f004 fe01 	bl	e007024 <DiagPrintf_minimal>
 e002422:	e7e8      	b.n	e0023f6 <rtk_log_memory_dump_byte+0x1e>
 e002424:	4802      	ldr	r0, [pc, #8]	; (e002430 <rtk_log_memory_dump_byte+0x58>)
 e002426:	f004 bdfd 	b.w	e007024 <DiagPrintf_minimal>
 e00242a:	bf00      	nop
 e00242c:	0e00826c 	.word	0x0e00826c
 e002430:	0e0088e0 	.word	0x0e0088e0
 e002434:	0e008258 	.word	0x0e008258
 e002438:	0e008250 	.word	0x0e008250

0e00243c <rtk_log_write>:
 e00243c:	b408      	push	{r3}
 e00243e:	4b1d      	ldr	r3, [pc, #116]	; (e0024b4 <rtk_log_write+0x78>)
 e002440:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 e002444:	681c      	ldr	r4, [r3, #0]
 e002446:	b083      	sub	sp, #12
 e002448:	4680      	mov	r8, r0
 e00244a:	460e      	mov	r6, r1
 e00244c:	2c03      	cmp	r4, #3
 e00244e:	4691      	mov	r9, r2
 e002450:	d827      	bhi.n	e0024a2 <rtk_log_write+0x66>
 e002452:	681f      	ldr	r7, [r3, #0]
 e002454:	b1a7      	cbz	r7, e002480 <rtk_log_write+0x44>
 e002456:	4d18      	ldr	r5, [pc, #96]	; (e0024b8 <rtk_log_write+0x7c>)
 e002458:	2400      	movs	r4, #0
 e00245a:	f8df a06c 	ldr.w	sl, [pc, #108]	; e0024c8 <rtk_log_write+0x8c>
 e00245e:	e002      	b.n	e002466 <rtk_log_write+0x2a>
 e002460:	3401      	adds	r4, #1
 e002462:	42a7      	cmp	r7, r4
 e002464:	d90c      	bls.n	e002480 <rtk_log_write+0x44>
 e002466:	4628      	mov	r0, r5
 e002468:	4631      	mov	r1, r6
 e00246a:	350b      	adds	r5, #11
 e00246c:	47d0      	blx	sl
 e00246e:	2800      	cmp	r0, #0
 e002470:	d1f6      	bne.n	e002460 <rtk_log_write+0x24>
 e002472:	eb04 0284 	add.w	r2, r4, r4, lsl #2
 e002476:	4b11      	ldr	r3, [pc, #68]	; (e0024bc <rtk_log_write+0x80>)
 e002478:	eb04 0442 	add.w	r4, r4, r2, lsl #1
 e00247c:	5d1b      	ldrb	r3, [r3, r4]
 e00247e:	e001      	b.n	e002484 <rtk_log_write+0x48>
 e002480:	4b0f      	ldr	r3, [pc, #60]	; (e0024c0 <rtk_log_write+0x84>)
 e002482:	781b      	ldrb	r3, [r3, #0]
 e002484:	4598      	cmp	r8, r3
 e002486:	d807      	bhi.n	e002498 <rtk_log_write+0x5c>
 e002488:	7833      	ldrb	r3, [r6, #0]
 e00248a:	2b23      	cmp	r3, #35	; 0x23
 e00248c:	d10b      	bne.n	e0024a6 <rtk_log_write+0x6a>
 e00248e:	a90c      	add	r1, sp, #48	; 0x30
 e002490:	980b      	ldr	r0, [sp, #44]	; 0x2c
 e002492:	9101      	str	r1, [sp, #4]
 e002494:	f004 faf8 	bl	e006a88 <DiagVprintf>
 e002498:	b003      	add	sp, #12
 e00249a:	e8bd 47f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 e00249e:	b001      	add	sp, #4
 e0024a0:	4770      	bx	lr
 e0024a2:	2704      	movs	r7, #4
 e0024a4:	e7d7      	b.n	e002456 <rtk_log_write+0x1a>
 e0024a6:	464a      	mov	r2, r9
 e0024a8:	4631      	mov	r1, r6
 e0024aa:	4806      	ldr	r0, [pc, #24]	; (e0024c4 <rtk_log_write+0x88>)
 e0024ac:	f004 fdac 	bl	e007008 <DiagPrintf>
 e0024b0:	e7ed      	b.n	e00248e <rtk_log_write+0x52>
 e0024b2:	bf00      	nop
 e0024b4:	2000b490 	.word	0x2000b490
 e0024b8:	2000b495 	.word	0x2000b495
 e0024bc:	2000b494 	.word	0x2000b494
 e0024c0:	2000b04c 	.word	0x2000b04c
 e0024c4:	0e008274 	.word	0x0e008274
 e0024c8:	00011e89 	.word	0x00011e89

0e0024cc <log_service_init>:
 e0024cc:	4909      	ldr	r1, [pc, #36]	; (e0024f4 <log_service_init+0x28>)
 e0024ce:	4a0a      	ldr	r2, [pc, #40]	; (e0024f8 <log_service_init+0x2c>)
 e0024d0:	4b0a      	ldr	r3, [pc, #40]	; (e0024fc <log_service_init+0x30>)
 e0024d2:	1d08      	adds	r0, r1, #4
 e0024d4:	b410      	push	{r4}
 e0024d6:	4c0a      	ldr	r4, [pc, #40]	; (e002500 <log_service_init+0x34>)
 e0024d8:	6010      	str	r0, [r2, #0]
 e0024da:	f503 7280 	add.w	r2, r3, #256	; 0x100
 e0024de:	6021      	str	r1, [r4, #0]
 e0024e0:	e9c3 3300 	strd	r3, r3, [r3]
 e0024e4:	3308      	adds	r3, #8
 e0024e6:	429a      	cmp	r2, r3
 e0024e8:	d1fa      	bne.n	e0024e0 <log_service_init+0x14>
 e0024ea:	680b      	ldr	r3, [r1, #0]
 e0024ec:	f85d 4b04 	ldr.w	r4, [sp], #4
 e0024f0:	4718      	bx	r3
 e0024f2:	bf00      	nop
 e0024f4:	2000b058 	.word	0x2000b058
 e0024f8:	2000b4c4 	.word	0x2000b4c4
 e0024fc:	2000b4c8 	.word	0x2000b4c8
 e002500:	2000b4c0 	.word	0x2000b4c0

0e002504 <log_service_add_table>:
 e002504:	2900      	cmp	r1, #0
 e002506:	dd26      	ble.n	e002556 <log_service_add_table+0x52>
 e002508:	b430      	push	{r4, r5}
 e00250a:	f100 0408 	add.w	r4, r0, #8
 e00250e:	4d12      	ldr	r5, [pc, #72]	; (e002558 <log_service_add_table+0x54>)
 e002510:	eb04 1c01 	add.w	ip, r4, r1, lsl #4
 e002514:	f854 1c08 	ldr.w	r1, [r4, #-8]
 e002518:	780a      	ldrb	r2, [r1, #0]
 e00251a:	b1ca      	cbz	r2, e002550 <log_service_add_table+0x4c>
 e00251c:	2300      	movs	r3, #0
 e00251e:	eb03 1083 	add.w	r0, r3, r3, lsl #6
 e002522:	eb03 0340 	add.w	r3, r3, r0, lsl #1
 e002526:	4413      	add	r3, r2
 e002528:	f811 2f01 	ldrb.w	r2, [r1, #1]!
 e00252c:	2a00      	cmp	r2, #0
 e00252e:	d1f6      	bne.n	e00251e <log_service_add_table+0x1a>
 e002530:	f003 031f 	and.w	r3, r3, #31
 e002534:	eb05 01c3 	add.w	r1, r5, r3, lsl #3
 e002538:	f855 2033 	ldr.w	r2, [r5, r3, lsl #3]
 e00253c:	6054      	str	r4, [r2, #4]
 e00253e:	e9c4 2100 	strd	r2, r1, [r4]
 e002542:	f845 4033 	str.w	r4, [r5, r3, lsl #3]
 e002546:	3410      	adds	r4, #16
 e002548:	45a4      	cmp	ip, r4
 e00254a:	d1e3      	bne.n	e002514 <log_service_add_table+0x10>
 e00254c:	bc30      	pop	{r4, r5}
 e00254e:	4770      	bx	lr
 e002550:	4629      	mov	r1, r5
 e002552:	4613      	mov	r3, r2
 e002554:	e7f0      	b.n	e002538 <log_service_add_table+0x34>
 e002556:	4770      	bx	lr
 e002558:	2000b4c8 	.word	0x2000b4c8

0e00255c <log_action>:
 e00255c:	b570      	push	{r4, r5, r6, lr}
 e00255e:	7802      	ldrb	r2, [r0, #0]
 e002560:	4605      	mov	r5, r0
 e002562:	b31a      	cbz	r2, e0025ac <log_action+0x50>
 e002564:	4604      	mov	r4, r0
 e002566:	2300      	movs	r3, #0
 e002568:	eb03 1c83 	add.w	ip, r3, r3, lsl #6
 e00256c:	eb03 034c 	add.w	r3, r3, ip, lsl #1
 e002570:	4413      	add	r3, r2
 e002572:	f814 2f01 	ldrb.w	r2, [r4, #1]!
 e002576:	2a00      	cmp	r2, #0
 e002578:	d1f6      	bne.n	e002568 <log_action+0xc>
 e00257a:	f003 031f 	and.w	r3, r3, #31
 e00257e:	490d      	ldr	r1, [pc, #52]	; (e0025b4 <log_action+0x58>)
 e002580:	eb01 06c3 	add.w	r6, r1, r3, lsl #3
 e002584:	f851 4033 	ldr.w	r4, [r1, r3, lsl #3]
 e002588:	42b4      	cmp	r4, r6
 e00258a:	d103      	bne.n	e002594 <log_action+0x38>
 e00258c:	e00c      	b.n	e0025a8 <log_action+0x4c>
 e00258e:	6824      	ldr	r4, [r4, #0]
 e002590:	42b4      	cmp	r4, r6
 e002592:	d009      	beq.n	e0025a8 <log_action+0x4c>
 e002594:	f854 0c08 	ldr.w	r0, [r4, #-8]
 e002598:	4629      	mov	r1, r5
 e00259a:	f005 fa3d 	bl	e007a18 <____wrap_strcmp_veneer>
 e00259e:	2800      	cmp	r0, #0
 e0025a0:	d1f5      	bne.n	e00258e <log_action+0x32>
 e0025a2:	f854 0c04 	ldr.w	r0, [r4, #-4]
 e0025a6:	bd70      	pop	{r4, r5, r6, pc}
 e0025a8:	2000      	movs	r0, #0
 e0025aa:	bd70      	pop	{r4, r5, r6, pc}
 e0025ac:	4901      	ldr	r1, [pc, #4]	; (e0025b4 <log_action+0x58>)
 e0025ae:	4613      	mov	r3, r2
 e0025b0:	460e      	mov	r6, r1
 e0025b2:	e7e7      	b.n	e002584 <log_action+0x28>
 e0025b4:	2000b4c8 	.word	0x2000b4c8

0e0025b8 <log_handler>:
 e0025b8:	b530      	push	{r4, r5, lr}
 e0025ba:	b0a5      	sub	sp, #148	; 0x94
 e0025bc:	4605      	mov	r5, r0
 e0025be:	2264      	movs	r2, #100	; 0x64
 e0025c0:	2100      	movs	r1, #0
 e0025c2:	ac0b      	add	r4, sp, #44	; 0x2c
 e0025c4:	4620      	mov	r0, r4
 e0025c6:	f005 fa1f 	bl	e007a08 <____wrap_memset_veneer>
 e0025ca:	2100      	movs	r1, #0
 e0025cc:	221d      	movs	r2, #29
 e0025ce:	a803      	add	r0, sp, #12
 e0025d0:	e9cd 4101 	strd	r4, r1, [sp, #4]
 e0025d4:	f005 fa18 	bl	e007a08 <____wrap_memset_veneer>
 e0025d8:	2263      	movs	r2, #99	; 0x63
 e0025da:	4629      	mov	r1, r5
 e0025dc:	4620      	mov	r0, r4
 e0025de:	f005 f9b1 	bl	e007944 <strncpy>
 e0025e2:	4910      	ldr	r1, [pc, #64]	; (e002624 <log_handler+0x6c>)
 e0025e4:	a801      	add	r0, sp, #4
 e0025e6:	f005 f9d7 	bl	e007998 <strsep>
 e0025ea:	4604      	mov	r4, r0
 e0025ec:	490e      	ldr	r1, [pc, #56]	; (e002628 <log_handler+0x70>)
 e0025ee:	a801      	add	r0, sp, #4
 e0025f0:	f005 f9d2 	bl	e007998 <strsep>
 e0025f4:	b17c      	cbz	r4, e002616 <log_handler+0x5e>
 e0025f6:	7823      	ldrb	r3, [r4, #0]
 e0025f8:	b183      	cbz	r3, e00261c <log_handler+0x64>
 e0025fa:	2220      	movs	r2, #32
 e0025fc:	4621      	mov	r1, r4
 e0025fe:	4605      	mov	r5, r0
 e002600:	a802      	add	r0, sp, #8
 e002602:	f005 f99f 	bl	e007944 <strncpy>
 e002606:	a802      	add	r0, sp, #8
 e002608:	f7ff ffa8 	bl	e00255c <log_action>
 e00260c:	4603      	mov	r3, r0
 e00260e:	b128      	cbz	r0, e00261c <log_handler+0x64>
 e002610:	4628      	mov	r0, r5
 e002612:	461c      	mov	r4, r3
 e002614:	4798      	blx	r3
 e002616:	4620      	mov	r0, r4
 e002618:	b025      	add	sp, #148	; 0x94
 e00261a:	bd30      	pop	{r4, r5, pc}
 e00261c:	461c      	mov	r4, r3
 e00261e:	4620      	mov	r0, r4
 e002620:	b025      	add	sp, #148	; 0x94
 e002622:	bd30      	pop	{r4, r5, pc}
 e002624:	0e008280 	.word	0x0e008280
 e002628:	0e0082e0 	.word	0x0e0082e0

0e00262c <mp_commnad_handler>:
 e00262c:	b530      	push	{r4, r5, lr}
 e00262e:	b0a3      	sub	sp, #140	; 0x8c
 e002630:	4605      	mov	r5, r0
 e002632:	2280      	movs	r2, #128	; 0x80
 e002634:	2100      	movs	r1, #0
 e002636:	ac02      	add	r4, sp, #8
 e002638:	4620      	mov	r0, r4
 e00263a:	9401      	str	r4, [sp, #4]
 e00263c:	f005 f9e4 	bl	e007a08 <____wrap_memset_veneer>
 e002640:	4629      	mov	r1, r5
 e002642:	227f      	movs	r2, #127	; 0x7f
 e002644:	4620      	mov	r0, r4
 e002646:	f005 f97d 	bl	e007944 <strncpy>
 e00264a:	4910      	ldr	r1, [pc, #64]	; (e00268c <mp_commnad_handler+0x60>)
 e00264c:	a801      	add	r0, sp, #4
 e00264e:	f005 f9a3 	bl	e007998 <strsep>
 e002652:	b118      	cbz	r0, e00265c <mp_commnad_handler+0x30>
 e002654:	4b0e      	ldr	r3, [pc, #56]	; (e002690 <mp_commnad_handler+0x64>)
 e002656:	9a02      	ldr	r2, [sp, #8]
 e002658:	429a      	cmp	r2, r3
 e00265a:	d004      	beq.n	e002666 <mp_commnad_handler+0x3a>
 e00265c:	f04f 34ff 	mov.w	r4, #4294967295	; 0xffffffff
 e002660:	4620      	mov	r0, r4
 e002662:	b023      	add	sp, #140	; 0x8c
 e002664:	bd30      	pop	{r4, r5, pc}
 e002666:	f8bd 200c 	ldrh.w	r2, [sp, #12]
 e00266a:	f247 6369 	movw	r3, #30313	; 0x7669
 e00266e:	429a      	cmp	r2, r3
 e002670:	d1f4      	bne.n	e00265c <mp_commnad_handler+0x30>
 e002672:	f89d 400e 	ldrb.w	r4, [sp, #14]
 e002676:	2c00      	cmp	r4, #0
 e002678:	d1f0      	bne.n	e00265c <mp_commnad_handler+0x30>
 e00267a:	4906      	ldr	r1, [pc, #24]	; (e002694 <mp_commnad_handler+0x68>)
 e00267c:	a801      	add	r0, sp, #4
 e00267e:	f005 f98b 	bl	e007998 <strsep>
 e002682:	2201      	movs	r2, #1
 e002684:	2180      	movs	r1, #128	; 0x80
 e002686:	f003 fd7b 	bl	e006180 <inic_mp_command>
 e00268a:	e7e9      	b.n	e002660 <mp_commnad_handler+0x34>
 e00268c:	0e008260 	.word	0x0e008260
 e002690:	72707769 	.word	0x72707769
 e002694:	0e008bf4 	.word	0x0e008bf4

0e002698 <log_service>:
 e002698:	b510      	push	{r4, lr}
 e00269a:	4604      	mov	r4, r0
 e00269c:	f7ff ff8c 	bl	e0025b8 <log_handler>
 e0026a0:	b150      	cbz	r0, e0026b8 <log_service+0x20>
 e0026a2:	f000 fa33 	bl	e002b0c <rtos_mem_get_free_heap_size>
 e0026a6:	4601      	mov	r1, r0
 e0026a8:	480b      	ldr	r0, [pc, #44]	; (e0026d8 <log_service+0x40>)
 e0026aa:	f004 fcbb 	bl	e007024 <DiagPrintf_minimal>
 e0026ae:	480b      	ldr	r0, [pc, #44]	; (e0026dc <log_service+0x44>)
 e0026b0:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
 e0026b4:	f004 bcb6 	b.w	e007024 <DiagPrintf_minimal>
 e0026b8:	4620      	mov	r0, r4
 e0026ba:	f7ff ffb7 	bl	e00262c <mp_commnad_handler>
 e0026be:	2800      	cmp	r0, #0
 e0026c0:	daef      	bge.n	e0026a2 <log_service+0xa>
 e0026c2:	4907      	ldr	r1, [pc, #28]	; (e0026e0 <log_service+0x48>)
 e0026c4:	4620      	mov	r0, r4
 e0026c6:	f005 f9a7 	bl	e007a18 <____wrap_strcmp_veneer>
 e0026ca:	2800      	cmp	r0, #0
 e0026cc:	d0e9      	beq.n	e0026a2 <log_service+0xa>
 e0026ce:	4621      	mov	r1, r4
 e0026d0:	4804      	ldr	r0, [pc, #16]	; (e0026e4 <log_service+0x4c>)
 e0026d2:	f004 fca7 	bl	e007024 <DiagPrintf_minimal>
 e0026d6:	e7e4      	b.n	e0026a2 <log_service+0xa>
 e0026d8:	0e0082ac 	.word	0x0e0082ac
 e0026dc:	0e0082d8 	.word	0x0e0082d8
 e0026e0:	0e00828c 	.word	0x0e00828c
 e0026e4:	0e008294 	.word	0x0e008294

0e0026e8 <app_mbedtls_free_func>:
 e0026e8:	f000 ba0c 	b.w	e002b04 <rtos_mem_free>

0e0026ec <app_mbedtls_calloc_func>:
 e0026ec:	b538      	push	{r3, r4, r5, lr}
 e0026ee:	fb01 f500 	mul.w	r5, r1, r0
 e0026f2:	4628      	mov	r0, r5
 e0026f4:	f000 f9f8 	bl	e002ae8 <rtos_mem_malloc>
 e0026f8:	4604      	mov	r4, r0
 e0026fa:	b118      	cbz	r0, e002704 <app_mbedtls_calloc_func+0x18>
 e0026fc:	462a      	mov	r2, r5
 e0026fe:	2100      	movs	r1, #0
 e002700:	f005 f982 	bl	e007a08 <____wrap_memset_veneer>
 e002704:	4620      	mov	r0, r4
 e002706:	bd38      	pop	{r3, r4, r5, pc}

0e002708 <app_pmu_init>:
 e002708:	4a06      	ldr	r2, [pc, #24]	; (e002724 <app_pmu_init+0x1c>)
 e00270a:	2000      	movs	r0, #0
 e00270c:	b508      	push	{r3, lr}
 e00270e:	6893      	ldr	r3, [r2, #8]
 e002710:	f043 5300 	orr.w	r3, r3, #536870912	; 0x20000000
 e002714:	6093      	str	r3, [r2, #8]
 e002716:	f7ff fb83 	bl	e001e20 <pmu_set_sleep_type>
 e00271a:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
 e00271e:	f7ff bb87 	b.w	e001e30 <pmu_init_wakeup_timer>
 e002722:	bf00      	nop
 e002724:	20004004 	.word	0x20004004

0e002728 <app_pre_example>:
 e002728:	4770      	bx	lr
 e00272a:	bf00      	nop

0e00272c <app_example>:
 e00272c:	4770      	bx	lr
 e00272e:	bf00      	nop

0e002730 <main>:
 e002730:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 e002732:	4d1e      	ldr	r5, [pc, #120]	; (e0027ac <main+0x7c>)
 e002734:	2249      	movs	r2, #73	; 0x49
 e002736:	4b1e      	ldr	r3, [pc, #120]	; (e0027b0 <main+0x80>)
 e002738:	2004      	movs	r0, #4
 e00273a:	4629      	mov	r1, r5
 e00273c:	2400      	movs	r4, #0
 e00273e:	f7ff fe7d 	bl	e00243c <rtk_log_write>
 e002742:	4b1c      	ldr	r3, [pc, #112]	; (e0027b4 <main+0x84>)
 e002744:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
 e002748:	4e1b      	ldr	r6, [pc, #108]	; (e0027b8 <main+0x88>)
 e00274a:	4621      	mov	r1, r4
 e00274c:	4620      	mov	r0, r4
 e00274e:	601a      	str	r2, [r3, #0]
 e002750:	60dc      	str	r4, [r3, #12]
 e002752:	4f1a      	ldr	r7, [pc, #104]	; (e0027bc <main+0x8c>)
 e002754:	e9c3 4401 	strd	r4, r4, [r3, #4]
 e002758:	4b19      	ldr	r3, [pc, #100]	; (e0027c0 <main+0x90>)
 e00275a:	4798      	blx	r3
 e00275c:	f001 fb1c 	bl	e003d98 <shell_init_ram>
 e002760:	4632      	mov	r2, r6
 e002762:	2108      	movs	r1, #8
 e002764:	2305      	movs	r3, #5
 e002766:	4817      	ldr	r0, [pc, #92]	; (e0027c4 <main+0x94>)
 e002768:	47b8      	blx	r7
 e00276a:	4b17      	ldr	r3, [pc, #92]	; (e0027c8 <main+0x98>)
 e00276c:	2008      	movs	r0, #8
 e00276e:	4798      	blx	r3
 e002770:	4630      	mov	r0, r6
 e002772:	f7ff f859 	bl	e001828 <ipc_table_init>
 e002776:	4915      	ldr	r1, [pc, #84]	; (e0027cc <main+0x9c>)
 e002778:	4815      	ldr	r0, [pc, #84]	; (e0027d0 <main+0xa0>)
 e00277a:	f000 fba7 	bl	e002ecc <mbedtls_platform_set_calloc_free>
 e00277e:	4b15      	ldr	r3, [pc, #84]	; (e0027d4 <main+0xa4>)
 e002780:	601c      	str	r4, [r3, #0]
 e002782:	f7ff ffd1 	bl	e002728 <app_pre_example>
 e002786:	f002 fda9 	bl	e0052dc <wlan_initialize>
 e00278a:	f7ff ffbd 	bl	e002708 <app_pmu_init>
 e00278e:	f7ff ffcd 	bl	e00272c <app_example>
 e002792:	4811      	ldr	r0, [pc, #68]	; (e0027d8 <main+0xa8>)
 e002794:	f004 f942 	bl	e006a1c <IPC_SEMDelay>
 e002798:	4b10      	ldr	r3, [pc, #64]	; (e0027dc <main+0xac>)
 e00279a:	2249      	movs	r2, #73	; 0x49
 e00279c:	4629      	mov	r1, r5
 e00279e:	2004      	movs	r0, #4
 e0027a0:	f7ff fe4c 	bl	e00243c <rtk_log_write>
 e0027a4:	f000 fb34 	bl	e002e10 <rtos_sched_start>
 e0027a8:	4620      	mov	r0, r4
 e0027aa:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e0027ac:	0e008304 	.word	0x0e008304
 e0027b0:	0e008324 	.word	0x0e008324
 e0027b4:	20004004 	.word	0x20004004
 e0027b8:	41014080 	.word	0x41014080
 e0027bc:	0000d769 	.word	0x0000d769
 e0027c0:	0000e505 	.word	0x0000e505
 e0027c4:	0e001ae1 	.word	0x0e001ae1
 e0027c8:	0000d641 	.word	0x0000d641
 e0027cc:	0e0026e9 	.word	0x0e0026e9
 e0027d0:	0e0026ed 	.word	0x0e0026ed
 e0027d4:	2000b5c8 	.word	0x2000b5c8
 e0027d8:	0e002e91 	.word	0x0e002e91
 e0027dc:	0e008330 	.word	0x0e008330

0e0027e0 <CmdRamHelp>:
 e0027e0:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 e0027e2:	4f0d      	ldr	r7, [pc, #52]	; (e002818 <CmdRamHelp+0x38>)
 e0027e4:	4c0d      	ldr	r4, [pc, #52]	; (e00281c <CmdRamHelp+0x3c>)
 e0027e6:	480e      	ldr	r0, [pc, #56]	; (e002820 <CmdRamHelp+0x40>)
 e0027e8:	1b3f      	subs	r7, r7, r4
 e0027ea:	093f      	lsrs	r7, r7, #4
 e0027ec:	4639      	mov	r1, r7
 e0027ee:	f004 fc19 	bl	e007024 <DiagPrintf_minimal>
 e0027f2:	b157      	cbz	r7, e00280a <CmdRamHelp+0x2a>
 e0027f4:	4e0b      	ldr	r6, [pc, #44]	; (e002824 <CmdRamHelp+0x44>)
 e0027f6:	eb04 1507 	add.w	r5, r4, r7, lsl #4
 e0027fa:	68e1      	ldr	r1, [r4, #12]
 e0027fc:	4630      	mov	r0, r6
 e0027fe:	3410      	adds	r4, #16
 e002800:	b109      	cbz	r1, e002806 <CmdRamHelp+0x26>
 e002802:	f004 fc0f 	bl	e007024 <DiagPrintf_minimal>
 e002806:	42ac      	cmp	r4, r5
 e002808:	d1f7      	bne.n	e0027fa <CmdRamHelp+0x1a>
 e00280a:	4639      	mov	r1, r7
 e00280c:	4806      	ldr	r0, [pc, #24]	; (e002828 <CmdRamHelp+0x48>)
 e00280e:	f004 fc09 	bl	e007024 <DiagPrintf_minimal>
 e002812:	2001      	movs	r0, #1
 e002814:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e002816:	bf00      	nop
 e002818:	0e009990 	.word	0x0e009990
 e00281c:	0e009950 	.word	0x0e009950
 e002820:	0e008348 	.word	0x0e008348
 e002824:	0e008ed8 	.word	0x0e008ed8
 e002828:	0e008388 	.word	0x0e008388

0e00282c <cmd_reboot>:
 e00282c:	b538      	push	{r3, r4, r5, lr}
 e00282e:	4604      	mov	r4, r0
 e002830:	480b      	ldr	r0, [pc, #44]	; (e002860 <cmd_reboot+0x34>)
 e002832:	460d      	mov	r5, r1
 e002834:	f004 fbf6 	bl	e007024 <DiagPrintf_minimal>
 e002838:	b91c      	cbnz	r4, e002842 <cmd_reboot+0x16>
 e00283a:	f7ff fb17 	bl	e001e6c <System_Reset>
 e00283e:	2001      	movs	r0, #1
 e002840:	bd38      	pop	{r3, r4, r5, pc}
 e002842:	4908      	ldr	r1, [pc, #32]	; (e002864 <cmd_reboot+0x38>)
 e002844:	6828      	ldr	r0, [r5, #0]
 e002846:	4b08      	ldr	r3, [pc, #32]	; (e002868 <cmd_reboot+0x3c>)
 e002848:	4798      	blx	r3
 e00284a:	2800      	cmp	r0, #0
 e00284c:	d1f5      	bne.n	e00283a <cmd_reboot+0xe>
 e00284e:	f44f 7100 	mov.w	r1, #512	; 0x200
 e002852:	4b06      	ldr	r3, [pc, #24]	; (e00286c <cmd_reboot+0x40>)
 e002854:	4798      	blx	r3
 e002856:	f7ff fb09 	bl	e001e6c <System_Reset>
 e00285a:	2001      	movs	r0, #1
 e00285c:	bd38      	pop	{r3, r4, r5, pc}
 e00285e:	bf00      	nop
 e002860:	0e0083c8 	.word	0x0e0083c8
 e002864:	0e0083d8 	.word	0x0e0083d8
 e002868:	00011e89 	.word	0x00011e89
 e00286c:	000090a1 	.word	0x000090a1

0e002870 <cmd_write_word>:
 e002870:	b570      	push	{r4, r5, r6, lr}
 e002872:	460c      	mov	r4, r1
 e002874:	4e13      	ldr	r6, [pc, #76]	; (e0028c4 <cmd_write_word+0x54>)
 e002876:	2210      	movs	r2, #16
 e002878:	2100      	movs	r1, #0
 e00287a:	6820      	ldr	r0, [r4, #0]
 e00287c:	47b0      	blx	r6
 e00287e:	4605      	mov	r5, r0
 e002880:	2210      	movs	r2, #16
 e002882:	2100      	movs	r1, #0
 e002884:	6860      	ldr	r0, [r4, #4]
 e002886:	47b0      	blx	r6
 e002888:	f025 0603 	bic.w	r6, r5, #3
 e00288c:	4604      	mov	r4, r0
 e00288e:	480e      	ldr	r0, [pc, #56]	; (e0028c8 <cmd_write_word+0x58>)
 e002890:	4631      	mov	r1, r6
 e002892:	4622      	mov	r2, r4
 e002894:	f004 fbc6 	bl	e007024 <DiagPrintf_minimal>
 e002898:	4b0c      	ldr	r3, [pc, #48]	; (e0028cc <cmd_write_word+0x5c>)
 e00289a:	6034      	str	r4, [r6, #0]
 e00289c:	695b      	ldr	r3, [r3, #20]
 e00289e:	03db      	lsls	r3, r3, #15
 e0028a0:	d50d      	bpl.n	e0028be <cmd_write_word+0x4e>
 e0028a2:	f015 0f1c 	tst.w	r5, #28
 e0028a6:	bf18      	it	ne
 e0028a8:	f025 061f 	bicne.w	r6, r5, #31
 e0028ac:	f3bf 8f4f 	dsb	sy
 e0028b0:	4b06      	ldr	r3, [pc, #24]	; (e0028cc <cmd_write_word+0x5c>)
 e0028b2:	f8c3 6268 	str.w	r6, [r3, #616]	; 0x268
 e0028b6:	f3bf 8f4f 	dsb	sy
 e0028ba:	f3bf 8f6f 	isb	sy
 e0028be:	2000      	movs	r0, #0
 e0028c0:	bd70      	pop	{r4, r5, r6, pc}
 e0028c2:	bf00      	nop
 e0028c4:	00011fc5 	.word	0x00011fc5
 e0028c8:	0e0083e4 	.word	0x0e0083e4
 e0028cc:	e000ed00 	.word	0xe000ed00

0e0028d0 <cmd_dump_word>:
 e0028d0:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 e0028d4:	4604      	mov	r4, r0
 e0028d6:	4b61      	ldr	r3, [pc, #388]	; (e002a5c <cmd_dump_word+0x18c>)
 e0028d8:	460f      	mov	r7, r1
 e0028da:	4798      	blx	r3
 e0028dc:	1e63      	subs	r3, r4, #1
 e0028de:	2b02      	cmp	r3, #2
 e0028e0:	f200 808a 	bhi.w	e0029f8 <cmd_dump_word+0x128>
 e0028e4:	2c03      	cmp	r4, #3
 e0028e6:	4606      	mov	r6, r0
 e0028e8:	d03c      	beq.n	e002964 <cmd_dump_word+0x94>
 e0028ea:	2c01      	cmp	r4, #1
 e0028ec:	f040 8091 	bne.w	e002a12 <cmd_dump_word+0x142>
 e0028f0:	2210      	movs	r2, #16
 e0028f2:	4b5b      	ldr	r3, [pc, #364]	; (e002a60 <cmd_dump_word+0x190>)
 e0028f4:	2100      	movs	r1, #0
 e0028f6:	6838      	ldr	r0, [r7, #0]
 e0028f8:	4798      	blx	r3
 e0028fa:	f020 0c03 	bic.w	ip, r0, #3
 e0028fe:	4602      	mov	r2, r0
 e002900:	f10c 4378 	add.w	r3, ip, #4160749568	; 0xf8000000
 e002904:	f1b3 6f00 	cmp.w	r3, #134217728	; 0x8000000
 e002908:	d201      	bcs.n	e00290e <cmd_dump_word+0x3e>
 e00290a:	2e00      	cmp	r6, #0
 e00290c:	d17c      	bne.n	e002a08 <cmd_dump_word+0x138>
 e00290e:	2501      	movs	r5, #1
 e002910:	4b54      	ldr	r3, [pc, #336]	; (e002a64 <cmd_dump_word+0x194>)
 e002912:	695b      	ldr	r3, [r3, #20]
 e002914:	03db      	lsls	r3, r3, #15
 e002916:	d51e      	bpl.n	e002956 <cmd_dump_word+0x86>
 e002918:	f012 031c 	ands.w	r3, r2, #28
 e00291c:	ea4f 0185 	mov.w	r1, r5, lsl #2
 e002920:	f000 809a 	beq.w	e002a58 <cmd_dump_word+0x188>
 e002924:	f022 021f 	bic.w	r2, r2, #31
 e002928:	4419      	add	r1, r3
 e00292a:	f3bf 8f4f 	dsb	sy
 e00292e:	2900      	cmp	r1, #0
 e002930:	dd0d      	ble.n	e00294e <cmd_dump_word+0x7e>
 e002932:	3901      	subs	r1, #1
 e002934:	f102 0320 	add.w	r3, r2, #32
 e002938:	484a      	ldr	r0, [pc, #296]	; (e002a64 <cmd_dump_word+0x194>)
 e00293a:	f021 011f 	bic.w	r1, r1, #31
 e00293e:	4419      	add	r1, r3
 e002940:	428b      	cmp	r3, r1
 e002942:	f8c0 2270 	str.w	r2, [r0, #624]	; 0x270
 e002946:	461a      	mov	r2, r3
 e002948:	f103 0320 	add.w	r3, r3, #32
 e00294c:	d1f8      	bne.n	e002940 <cmd_dump_word+0x70>
 e00294e:	f3bf 8f4f 	dsb	sy
 e002952:	f3bf 8f6f 	isb	sy
 e002956:	4660      	mov	r0, ip
 e002958:	4629      	mov	r1, r5
 e00295a:	f7ff fd0b 	bl	e002374 <rtk_log_memory_dump_word>
 e00295e:	2001      	movs	r0, #1
 e002960:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e002964:	68bb      	ldr	r3, [r7, #8]
 e002966:	220a      	movs	r2, #10
 e002968:	2100      	movs	r1, #0
 e00296a:	4c3d      	ldr	r4, [pc, #244]	; (e002a60 <cmd_dump_word+0x190>)
 e00296c:	781b      	ldrb	r3, [r3, #0]
 e00296e:	6878      	ldr	r0, [r7, #4]
 e002970:	f003 03df 	and.w	r3, r3, #223	; 0xdf
 e002974:	f1a3 0842 	sub.w	r8, r3, #66	; 0x42
 e002978:	47a0      	blx	r4
 e00297a:	2210      	movs	r2, #16
 e00297c:	2100      	movs	r1, #0
 e00297e:	4605      	mov	r5, r0
 e002980:	6838      	ldr	r0, [r7, #0]
 e002982:	47a0      	blx	r4
 e002984:	f020 0c03 	bic.w	ip, r0, #3
 e002988:	fab8 f888 	clz	r8, r8
 e00298c:	4602      	mov	r2, r0
 e00298e:	f10c 4178 	add.w	r1, ip, #4160749568	; 0xf8000000
 e002992:	ea4f 1858 	mov.w	r8, r8, lsr #5
 e002996:	f1b1 6f00 	cmp.w	r1, #134217728	; 0x8000000
 e00299a:	d333      	bcc.n	e002a04 <cmd_dump_word+0x134>
 e00299c:	1e2b      	subs	r3, r5, #0
 e00299e:	bf18      	it	ne
 e0029a0:	2301      	movne	r3, #1
 e0029a2:	2d00      	cmp	r5, #0
 e0029a4:	d04f      	beq.n	e002a46 <cmd_dump_word+0x176>
 e0029a6:	f1b8 0f00 	cmp.w	r8, #0
 e0029aa:	d049      	beq.n	e002a40 <cmd_dump_word+0x170>
 e0029ac:	4b2d      	ldr	r3, [pc, #180]	; (e002a64 <cmd_dump_word+0x194>)
 e0029ae:	695b      	ldr	r3, [r3, #20]
 e0029b0:	03d9      	lsls	r1, r3, #15
 e0029b2:	d51a      	bpl.n	e0029ea <cmd_dump_word+0x11a>
 e0029b4:	f012 041c 	ands.w	r4, r2, #28
 e0029b8:	d14a      	bne.n	e002a50 <cmd_dump_word+0x180>
 e0029ba:	462c      	mov	r4, r5
 e0029bc:	4662      	mov	r2, ip
 e0029be:	f3bf 8f4f 	dsb	sy
 e0029c2:	2c00      	cmp	r4, #0
 e0029c4:	dd0d      	ble.n	e0029e2 <cmd_dump_word+0x112>
 e0029c6:	3c01      	subs	r4, #1
 e0029c8:	f102 0320 	add.w	r3, r2, #32
 e0029cc:	4925      	ldr	r1, [pc, #148]	; (e002a64 <cmd_dump_word+0x194>)
 e0029ce:	f024 041f 	bic.w	r4, r4, #31
 e0029d2:	441c      	add	r4, r3
 e0029d4:	42a3      	cmp	r3, r4
 e0029d6:	f8c1 2270 	str.w	r2, [r1, #624]	; 0x270
 e0029da:	461a      	mov	r2, r3
 e0029dc:	f103 0320 	add.w	r3, r3, #32
 e0029e0:	d1f8      	bne.n	e0029d4 <cmd_dump_word+0x104>
 e0029e2:	f3bf 8f4f 	dsb	sy
 e0029e6:	f3bf 8f6f 	isb	sy
 e0029ea:	4660      	mov	r0, ip
 e0029ec:	4629      	mov	r1, r5
 e0029ee:	f7ff fcf3 	bl	e0023d8 <rtk_log_memory_dump_byte>
 e0029f2:	2001      	movs	r0, #1
 e0029f4:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e0029f8:	481b      	ldr	r0, [pc, #108]	; (e002a68 <cmd_dump_word+0x198>)
 e0029fa:	f004 fb13 	bl	e007024 <DiagPrintf_minimal>
 e0029fe:	2000      	movs	r0, #0
 e002a00:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e002a04:	2e00      	cmp	r6, #0
 e002a06:	d0c9      	beq.n	e00299c <cmd_dump_word+0xcc>
 e002a08:	4818      	ldr	r0, [pc, #96]	; (e002a6c <cmd_dump_word+0x19c>)
 e002a0a:	f004 fb0b 	bl	e007024 <DiagPrintf_minimal>
 e002a0e:	2001      	movs	r0, #1
 e002a10:	e7a6      	b.n	e002960 <cmd_dump_word+0x90>
 e002a12:	220a      	movs	r2, #10
 e002a14:	2100      	movs	r1, #0
 e002a16:	4c12      	ldr	r4, [pc, #72]	; (e002a60 <cmd_dump_word+0x190>)
 e002a18:	6878      	ldr	r0, [r7, #4]
 e002a1a:	47a0      	blx	r4
 e002a1c:	2210      	movs	r2, #16
 e002a1e:	4605      	mov	r5, r0
 e002a20:	2100      	movs	r1, #0
 e002a22:	6838      	ldr	r0, [r7, #0]
 e002a24:	47a0      	blx	r4
 e002a26:	f020 0c03 	bic.w	ip, r0, #3
 e002a2a:	4602      	mov	r2, r0
 e002a2c:	f10c 4378 	add.w	r3, ip, #4160749568	; 0xf8000000
 e002a30:	f1b3 6f00 	cmp.w	r3, #134217728	; 0x8000000
 e002a34:	d201      	bcs.n	e002a3a <cmd_dump_word+0x16a>
 e002a36:	2e00      	cmp	r6, #0
 e002a38:	d1e6      	bne.n	e002a08 <cmd_dump_word+0x138>
 e002a3a:	1e2b      	subs	r3, r5, #0
 e002a3c:	bf18      	it	ne
 e002a3e:	2301      	movne	r3, #1
 e002a40:	2b00      	cmp	r3, #0
 e002a42:	f47f af65 	bne.w	e002910 <cmd_dump_word+0x40>
 e002a46:	480a      	ldr	r0, [pc, #40]	; (e002a70 <cmd_dump_word+0x1a0>)
 e002a48:	f004 faec 	bl	e007024 <DiagPrintf_minimal>
 e002a4c:	2000      	movs	r0, #0
 e002a4e:	e787      	b.n	e002960 <cmd_dump_word+0x90>
 e002a50:	f022 021f 	bic.w	r2, r2, #31
 e002a54:	442c      	add	r4, r5
 e002a56:	e7b2      	b.n	e0029be <cmd_dump_word+0xee>
 e002a58:	4662      	mov	r2, ip
 e002a5a:	e766      	b.n	e00292a <cmd_dump_word+0x5a>
 e002a5c:	0000c249 	.word	0x0000c249
 e002a60:	00011fc5 	.word	0x00011fc5
 e002a64:	e000ed00 	.word	0xe000ed00
 e002a68:	0e0083f4 	.word	0x0e0083f4
 e002a6c:	0e008410 	.word	0x0e008410
 e002a70:	0e008434 	.word	0x0e008434

0e002a74 <rtos_critical_is_in_interrupt>:
 e002a74:	f3ef 8003 	mrs	r0, PSR
 e002a78:	f3c0 0008 	ubfx	r0, r0, #0, #9
 e002a7c:	3800      	subs	r0, #0
 e002a7e:	bf18      	it	ne
 e002a80:	2001      	movne	r0, #1
 e002a82:	4770      	bx	lr

0e002a84 <rtos_critical_enter>:
 e002a84:	f3ef 8303 	mrs	r3, PSR
 e002a88:	f3c3 0308 	ubfx	r3, r3, #0, #9
 e002a8c:	b173      	cbz	r3, e002aac <rtos_critical_enter+0x28>
 e002a8e:	b510      	push	{r4, lr}
 e002a90:	4c07      	ldr	r4, [pc, #28]	; (e002ab0 <rtos_critical_enter+0x2c>)
 e002a92:	8823      	ldrh	r3, [r4, #0]
 e002a94:	b113      	cbz	r3, e002a9c <rtos_critical_enter+0x18>
 e002a96:	3301      	adds	r3, #1
 e002a98:	8023      	strh	r3, [r4, #0]
 e002a9a:	bd10      	pop	{r4, pc}
 e002a9c:	f000 fa8a 	bl	e002fb4 <ulSetInterruptMaskFromISR>
 e002aa0:	8823      	ldrh	r3, [r4, #0]
 e002aa2:	4a04      	ldr	r2, [pc, #16]	; (e002ab4 <rtos_critical_enter+0x30>)
 e002aa4:	3301      	adds	r3, #1
 e002aa6:	6010      	str	r0, [r2, #0]
 e002aa8:	8023      	strh	r3, [r4, #0]
 e002aaa:	bd10      	pop	{r4, pc}
 e002aac:	f000 bac8 	b.w	e003040 <vPortEnterCritical>
 e002ab0:	2000b5cc 	.word	0x2000b5cc
 e002ab4:	2000b5d0 	.word	0x2000b5d0

0e002ab8 <rtos_critical_exit>:
 e002ab8:	f3ef 8303 	mrs	r3, PSR
 e002abc:	f3c3 0308 	ubfx	r3, r3, #0, #9
 e002ac0:	b153      	cbz	r3, e002ad8 <rtos_critical_exit+0x20>
 e002ac2:	4a06      	ldr	r2, [pc, #24]	; (e002adc <rtos_critical_exit+0x24>)
 e002ac4:	8813      	ldrh	r3, [r2, #0]
 e002ac6:	3b01      	subs	r3, #1
 e002ac8:	b29b      	uxth	r3, r3
 e002aca:	8013      	strh	r3, [r2, #0]
 e002acc:	b103      	cbz	r3, e002ad0 <rtos_critical_exit+0x18>
 e002ace:	4770      	bx	lr
 e002ad0:	4b03      	ldr	r3, [pc, #12]	; (e002ae0 <rtos_critical_exit+0x28>)
 e002ad2:	6818      	ldr	r0, [r3, #0]
 e002ad4:	f000 ba74 	b.w	e002fc0 <vClearInterruptMaskFromISR>
 e002ad8:	f000 bad0 	b.w	e00307c <vPortExitCritical>
 e002adc:	2000b5cc 	.word	0x2000b5cc
 e002ae0:	2000b5d0 	.word	0x2000b5d0

0e002ae4 <rtos_mem_init>:
 e002ae4:	f7ff ba6a 	b.w	e001fbc <os_heap_init>

0e002ae8 <rtos_mem_malloc>:
 e002ae8:	f7ff bb14 	b.w	e002114 <pvPortMalloc>

0e002aec <rtos_mem_zmalloc>:
 e002aec:	b538      	push	{r3, r4, r5, lr}
 e002aee:	4605      	mov	r5, r0
 e002af0:	f7ff fb10 	bl	e002114 <pvPortMalloc>
 e002af4:	4604      	mov	r4, r0
 e002af6:	b118      	cbz	r0, e002b00 <rtos_mem_zmalloc+0x14>
 e002af8:	462a      	mov	r2, r5
 e002afa:	2100      	movs	r1, #0
 e002afc:	f004 ff84 	bl	e007a08 <____wrap_memset_veneer>
 e002b00:	4620      	mov	r0, r4
 e002b02:	bd38      	pop	{r3, r4, r5, pc}

0e002b04 <rtos_mem_free>:
 e002b04:	b108      	cbz	r0, e002b0a <rtos_mem_free+0x6>
 e002b06:	f7ff bb69 	b.w	e0021dc <vPortFree>
 e002b0a:	4770      	bx	lr

0e002b0c <rtos_mem_get_free_heap_size>:
 e002b0c:	f7ff bb94 	b.w	e002238 <xPortGetFreeHeapSize>

0e002b10 <rtos_mutex_create_static>:
 e002b10:	b510      	push	{r4, lr}
 e002b12:	4604      	mov	r4, r0
 e002b14:	f000 f94c 	bl	e002db0 <__reserved_get_mutex_from_poll>
 e002b18:	b148      	cbz	r0, e002b2e <rtos_mutex_create_static+0x1e>
 e002b1a:	4601      	mov	r1, r0
 e002b1c:	2001      	movs	r0, #1
 e002b1e:	f000 fded 	bl	e0036fc <xQueueCreateMutexStatic>
 e002b22:	fab0 f380 	clz	r3, r0
 e002b26:	6020      	str	r0, [r4, #0]
 e002b28:	095b      	lsrs	r3, r3, #5
 e002b2a:	4258      	negs	r0, r3
 e002b2c:	bd10      	pop	{r4, pc}
 e002b2e:	b144      	cbz	r4, e002b42 <rtos_mutex_create_static+0x32>
 e002b30:	2001      	movs	r0, #1
 e002b32:	f000 fe19 	bl	e003768 <xQueueCreateMutex>
 e002b36:	fab0 f380 	clz	r3, r0
 e002b3a:	6020      	str	r0, [r4, #0]
 e002b3c:	095b      	lsrs	r3, r3, #5
 e002b3e:	4258      	negs	r0, r3
 e002b40:	bd10      	pop	{r4, pc}
 e002b42:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
 e002b46:	bd10      	pop	{r4, pc}

0e002b48 <rtos_sema_create_static>:
 e002b48:	b570      	push	{r4, r5, r6, lr}
 e002b4a:	4604      	mov	r4, r0
 e002b4c:	460e      	mov	r6, r1
 e002b4e:	4615      	mov	r5, r2
 e002b50:	f000 f946 	bl	e002de0 <__reserved_get_sema_from_poll>
 e002b54:	b150      	cbz	r0, e002b6c <rtos_sema_create_static+0x24>
 e002b56:	4602      	mov	r2, r0
 e002b58:	4631      	mov	r1, r6
 e002b5a:	4628      	mov	r0, r5
 e002b5c:	f000 fce4 	bl	e003528 <xQueueCreateCountingSemaphoreStatic>
 e002b60:	fab0 f380 	clz	r3, r0
 e002b64:	6020      	str	r0, [r4, #0]
 e002b66:	095b      	lsrs	r3, r3, #5
 e002b68:	4258      	negs	r0, r3
 e002b6a:	bd70      	pop	{r4, r5, r6, pc}
 e002b6c:	b14c      	cbz	r4, e002b82 <rtos_sema_create_static+0x3a>
 e002b6e:	4631      	mov	r1, r6
 e002b70:	4628      	mov	r0, r5
 e002b72:	f000 fd09 	bl	e003588 <xQueueCreateCountingSemaphore>
 e002b76:	fab0 f380 	clz	r3, r0
 e002b7a:	6020      	str	r0, [r4, #0]
 e002b7c:	095b      	lsrs	r3, r3, #5
 e002b7e:	4258      	negs	r0, r3
 e002b80:	bd70      	pop	{r4, r5, r6, pc}
 e002b82:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
 e002b86:	bd70      	pop	{r4, r5, r6, pc}

0e002b88 <rtos_sema_create_binary>:
 e002b88:	b160      	cbz	r0, e002ba4 <rtos_sema_create_binary+0x1c>
 e002b8a:	2203      	movs	r2, #3
 e002b8c:	2100      	movs	r1, #0
 e002b8e:	b510      	push	{r4, lr}
 e002b90:	4604      	mov	r4, r0
 e002b92:	2001      	movs	r0, #1
 e002b94:	f000 fc8e 	bl	e0034b4 <xQueueGenericCreate>
 e002b98:	fab0 f380 	clz	r3, r0
 e002b9c:	6020      	str	r0, [r4, #0]
 e002b9e:	095b      	lsrs	r3, r3, #5
 e002ba0:	4258      	negs	r0, r3
 e002ba2:	bd10      	pop	{r4, pc}
 e002ba4:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
 e002ba8:	4770      	bx	lr
 e002baa:	bf00      	nop

0e002bac <rtos_sema_take>:
 e002bac:	b570      	push	{r4, r5, r6, lr}
 e002bae:	2600      	movs	r6, #0
 e002bb0:	b082      	sub	sp, #8
 e002bb2:	4604      	mov	r4, r0
 e002bb4:	460d      	mov	r5, r1
 e002bb6:	9601      	str	r6, [sp, #4]
 e002bb8:	f7ff ff5c 	bl	e002a74 <rtos_critical_is_in_interrupt>
 e002bbc:	b188      	cbz	r0, e002be2 <rtos_sema_take+0x36>
 e002bbe:	aa01      	add	r2, sp, #4
 e002bc0:	4631      	mov	r1, r6
 e002bc2:	4620      	mov	r0, r4
 e002bc4:	f000 ffc2 	bl	e003b4c <xQueueReceiveFromISR>
 e002bc8:	2801      	cmp	r0, #1
 e002bca:	d114      	bne.n	e002bf6 <rtos_sema_take+0x4a>
 e002bcc:	9801      	ldr	r0, [sp, #4]
 e002bce:	b180      	cbz	r0, e002bf2 <rtos_sema_take+0x46>
 e002bd0:	f04f 23e0 	mov.w	r3, #3758153728	; 0xe000e000
 e002bd4:	f04f 5280 	mov.w	r2, #268435456	; 0x10000000
 e002bd8:	4630      	mov	r0, r6
 e002bda:	f8c3 2d04 	str.w	r2, [r3, #3332]	; 0xd04
 e002bde:	b002      	add	sp, #8
 e002be0:	bd70      	pop	{r4, r5, r6, pc}
 e002be2:	4629      	mov	r1, r5
 e002be4:	4620      	mov	r0, r4
 e002be6:	f000 ff0f 	bl	e003a08 <xQueueSemaphoreTake>
 e002bea:	3801      	subs	r0, #1
 e002bec:	bf18      	it	ne
 e002bee:	f04f 30ff 	movne.w	r0, #4294967295	; 0xffffffff
 e002bf2:	b002      	add	sp, #8
 e002bf4:	bd70      	pop	{r4, r5, r6, pc}
 e002bf6:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
 e002bfa:	e7fa      	b.n	e002bf2 <rtos_sema_take+0x46>

0e002bfc <rtos_sema_give>:
 e002bfc:	b510      	push	{r4, lr}
 e002bfe:	2300      	movs	r3, #0
 e002c00:	b082      	sub	sp, #8
 e002c02:	4604      	mov	r4, r0
 e002c04:	9301      	str	r3, [sp, #4]
 e002c06:	f7ff ff35 	bl	e002a74 <rtos_critical_is_in_interrupt>
 e002c0a:	b188      	cbz	r0, e002c30 <rtos_sema_give+0x34>
 e002c0c:	a901      	add	r1, sp, #4
 e002c0e:	4620      	mov	r0, r4
 e002c10:	f000 fe3a 	bl	e003888 <xQueueGiveFromISR>
 e002c14:	9b01      	ldr	r3, [sp, #4]
 e002c16:	b12b      	cbz	r3, e002c24 <rtos_sema_give+0x28>
 e002c18:	f04f 23e0 	mov.w	r3, #3758153728	; 0xe000e000
 e002c1c:	f04f 5280 	mov.w	r2, #268435456	; 0x10000000
 e002c20:	f8c3 2d04 	str.w	r2, [r3, #3332]	; 0xd04
 e002c24:	3801      	subs	r0, #1
 e002c26:	bf18      	it	ne
 e002c28:	f04f 30ff 	movne.w	r0, #4294967295	; 0xffffffff
 e002c2c:	b002      	add	sp, #8
 e002c2e:	bd10      	pop	{r4, pc}
 e002c30:	4603      	mov	r3, r0
 e002c32:	4602      	mov	r2, r0
 e002c34:	4601      	mov	r1, r0
 e002c36:	4620      	mov	r0, r4
 e002c38:	f000 fcd8 	bl	e0035ec <xQueueGenericSend>
 e002c3c:	3801      	subs	r0, #1
 e002c3e:	bf18      	it	ne
 e002c40:	f04f 30ff 	movne.w	r0, #4294967295	; 0xffffffff
 e002c44:	b002      	add	sp, #8
 e002c46:	bd10      	pop	{r4, pc}

0e002c48 <__reserved_get_from_poll>:
 e002c48:	e92d 4ff8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e002c4c:	6814      	ldr	r4, [r2, #0]
 e002c4e:	4617      	mov	r7, r2
 e002c50:	4606      	mov	r6, r0
 e002c52:	460d      	mov	r5, r1
 e002c54:	4698      	mov	r8, r3
 e002c56:	f8dd a028 	ldr.w	sl, [sp, #40]	; 0x28
 e002c5a:	f8dd 902c 	ldr.w	r9, [sp, #44]	; 0x2c
 e002c5e:	b9d4      	cbnz	r4, e002c96 <__reserved_get_from_poll+0x4e>
 e002c60:	2801      	cmp	r0, #1
 e002c62:	d051      	beq.n	e002d08 <__reserved_get_from_poll+0xc0>
 e002c64:	2802      	cmp	r0, #2
 e002c66:	d066      	beq.n	e002d36 <__reserved_get_from_poll+0xee>
 e002c68:	f8df b138 	ldr.w	fp, [pc, #312]	; e002da4 <__reserved_get_from_poll+0x15c>
 e002c6c:	f8db 3000 	ldr.w	r3, [fp]
 e002c70:	2b01      	cmp	r3, #1
 e002c72:	d010      	beq.n	e002c96 <__reserved_get_from_poll+0x4e>
 e002c74:	4622      	mov	r2, r4
 e002c76:	4621      	mov	r1, r4
 e002c78:	483b      	ldr	r0, [pc, #236]	; (e002d68 <__reserved_get_from_poll+0x120>)
 e002c7a:	f004 fec5 	bl	e007a08 <____wrap_memset_veneer>
 e002c7e:	4b3b      	ldr	r3, [pc, #236]	; (e002d6c <__reserved_get_from_poll+0x124>)
 e002c80:	e9c3 3300 	strd	r3, r3, [r3]
 e002c84:	4b3a      	ldr	r3, [pc, #232]	; (e002d70 <__reserved_get_from_poll+0x128>)
 e002c86:	601c      	str	r4, [r3, #0]
 e002c88:	4b3a      	ldr	r3, [pc, #232]	; (e002d74 <__reserved_get_from_poll+0x12c>)
 e002c8a:	601c      	str	r4, [r3, #0]
 e002c8c:	4b3a      	ldr	r3, [pc, #232]	; (e002d78 <__reserved_get_from_poll+0x130>)
 e002c8e:	601c      	str	r4, [r3, #0]
 e002c90:	2301      	movs	r3, #1
 e002c92:	f8cb 3000 	str.w	r3, [fp]
 e002c96:	f7ff fef5 	bl	e002a84 <rtos_critical_enter>
 e002c9a:	682c      	ldr	r4, [r5, #0]
 e002c9c:	42a5      	cmp	r5, r4
 e002c9e:	d027      	beq.n	e002cf0 <__reserved_get_from_poll+0xa8>
 e002ca0:	f104 0508 	add.w	r5, r4, #8
 e002ca4:	e9d4 2300 	ldrd	r2, r3, [r4]
 e002ca8:	6053      	str	r3, [r2, #4]
 e002caa:	601a      	str	r2, [r3, #0]
 e002cac:	e9c4 4400 	strd	r4, r4, [r4]
 e002cb0:	f8da 3000 	ldr.w	r3, [sl]
 e002cb4:	3301      	adds	r3, #1
 e002cb6:	f8ca 3000 	str.w	r3, [sl]
 e002cba:	f7ff fefd 	bl	e002ab8 <rtos_critical_exit>
 e002cbe:	3408      	adds	r4, #8
 e002cc0:	d019      	beq.n	e002cf6 <__reserved_get_from_poll+0xae>
 e002cc2:	2e03      	cmp	r6, #3
 e002cc4:	f04f 0100 	mov.w	r1, #0
 e002cc8:	4628      	mov	r0, r5
 e002cca:	bf14      	ite	ne
 e002ccc:	2254      	movne	r2, #84	; 0x54
 e002cce:	222c      	moveq	r2, #44	; 0x2c
 e002cd0:	f004 fe9a 	bl	e007a08 <____wrap_memset_veneer>
 e002cd4:	f8d8 2000 	ldr.w	r2, [r8]
 e002cd8:	f8da 3000 	ldr.w	r3, [sl]
 e002cdc:	4628      	mov	r0, r5
 e002cde:	f8d9 1000 	ldr.w	r1, [r9]
 e002ce2:	4413      	add	r3, r2
 e002ce4:	428b      	cmp	r3, r1
 e002ce6:	bf88      	it	hi
 e002ce8:	f8c9 3000 	strhi.w	r3, [r9]
 e002cec:	e8bd 8ff8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, pc}
 e002cf0:	2500      	movs	r5, #0
 e002cf2:	f7ff fee1 	bl	e002ab8 <rtos_critical_exit>
 e002cf6:	683b      	ldr	r3, [r7, #0]
 e002cf8:	f8d8 2000 	ldr.w	r2, [r8]
 e002cfc:	2b00      	cmp	r3, #0
 e002cfe:	d0eb      	beq.n	e002cd8 <__reserved_get_from_poll+0x90>
 e002d00:	3201      	adds	r2, #1
 e002d02:	f8c8 2000 	str.w	r2, [r8]
 e002d06:	e7e7      	b.n	e002cd8 <__reserved_get_from_poll+0x90>
 e002d08:	f8df b09c 	ldr.w	fp, [pc, #156]	; e002da8 <__reserved_get_from_poll+0x160>
 e002d0c:	f8db 3000 	ldr.w	r3, [fp]
 e002d10:	2b01      	cmp	r3, #1
 e002d12:	d0c0      	beq.n	e002c96 <__reserved_get_from_poll+0x4e>
 e002d14:	4622      	mov	r2, r4
 e002d16:	4621      	mov	r1, r4
 e002d18:	4818      	ldr	r0, [pc, #96]	; (e002d7c <__reserved_get_from_poll+0x134>)
 e002d1a:	f004 fe75 	bl	e007a08 <____wrap_memset_veneer>
 e002d1e:	4b18      	ldr	r3, [pc, #96]	; (e002d80 <__reserved_get_from_poll+0x138>)
 e002d20:	f8cb 6000 	str.w	r6, [fp]
 e002d24:	e9c3 3300 	strd	r3, r3, [r3]
 e002d28:	4b16      	ldr	r3, [pc, #88]	; (e002d84 <__reserved_get_from_poll+0x13c>)
 e002d2a:	601c      	str	r4, [r3, #0]
 e002d2c:	4b16      	ldr	r3, [pc, #88]	; (e002d88 <__reserved_get_from_poll+0x140>)
 e002d2e:	601c      	str	r4, [r3, #0]
 e002d30:	4b16      	ldr	r3, [pc, #88]	; (e002d8c <__reserved_get_from_poll+0x144>)
 e002d32:	601c      	str	r4, [r3, #0]
 e002d34:	e7af      	b.n	e002c96 <__reserved_get_from_poll+0x4e>
 e002d36:	f8df b074 	ldr.w	fp, [pc, #116]	; e002dac <__reserved_get_from_poll+0x164>
 e002d3a:	f8db 3000 	ldr.w	r3, [fp]
 e002d3e:	2b01      	cmp	r3, #1
 e002d40:	d0a9      	beq.n	e002c96 <__reserved_get_from_poll+0x4e>
 e002d42:	4622      	mov	r2, r4
 e002d44:	4621      	mov	r1, r4
 e002d46:	4812      	ldr	r0, [pc, #72]	; (e002d90 <__reserved_get_from_poll+0x148>)
 e002d48:	f004 fe5e 	bl	e007a08 <____wrap_memset_veneer>
 e002d4c:	4b11      	ldr	r3, [pc, #68]	; (e002d94 <__reserved_get_from_poll+0x14c>)
 e002d4e:	e9c3 3300 	strd	r3, r3, [r3]
 e002d52:	4b11      	ldr	r3, [pc, #68]	; (e002d98 <__reserved_get_from_poll+0x150>)
 e002d54:	601c      	str	r4, [r3, #0]
 e002d56:	4b11      	ldr	r3, [pc, #68]	; (e002d9c <__reserved_get_from_poll+0x154>)
 e002d58:	601c      	str	r4, [r3, #0]
 e002d5a:	4b11      	ldr	r3, [pc, #68]	; (e002da0 <__reserved_get_from_poll+0x158>)
 e002d5c:	601c      	str	r4, [r3, #0]
 e002d5e:	2301      	movs	r3, #1
 e002d60:	f8cb 3000 	str.w	r3, [fp]
 e002d64:	e797      	b.n	e002c96 <__reserved_get_from_poll+0x4e>
 e002d66:	bf00      	nop
 e002d68:	2000b600 	.word	0x2000b600
 e002d6c:	2000b614 	.word	0x2000b614
 e002d70:	2000b5f4 	.word	0x2000b5f4
 e002d74:	2000b5fc 	.word	0x2000b5fc
 e002d78:	2000b5f8 	.word	0x2000b5f8
 e002d7c:	2000b5e0 	.word	0x2000b5e0
 e002d80:	2000b604 	.word	0x2000b604
 e002d84:	2000b5d4 	.word	0x2000b5d4
 e002d88:	2000b5dc 	.word	0x2000b5dc
 e002d8c:	2000b5d8 	.word	0x2000b5d8
 e002d90:	2000b5f0 	.word	0x2000b5f0
 e002d94:	2000b60c 	.word	0x2000b60c
 e002d98:	2000b5e4 	.word	0x2000b5e4
 e002d9c:	2000b5ec 	.word	0x2000b5ec
 e002da0:	2000b5e8 	.word	0x2000b5e8
 e002da4:	2000b600 	.word	0x2000b600
 e002da8:	2000b5e0 	.word	0x2000b5e0
 e002dac:	2000b5f0 	.word	0x2000b5f0

0e002db0 <__reserved_get_mutex_from_poll>:
 e002db0:	b510      	push	{r4, lr}
 e002db2:	4806      	ldr	r0, [pc, #24]	; (e002dcc <__reserved_get_mutex_from_poll+0x1c>)
 e002db4:	b082      	sub	sp, #8
 e002db6:	4c06      	ldr	r4, [pc, #24]	; (e002dd0 <__reserved_get_mutex_from_poll+0x20>)
 e002db8:	4b06      	ldr	r3, [pc, #24]	; (e002dd4 <__reserved_get_mutex_from_poll+0x24>)
 e002dba:	4a07      	ldr	r2, [pc, #28]	; (e002dd8 <__reserved_get_mutex_from_poll+0x28>)
 e002dbc:	4907      	ldr	r1, [pc, #28]	; (e002ddc <__reserved_get_mutex_from_poll+0x2c>)
 e002dbe:	e9cd 0400 	strd	r0, r4, [sp]
 e002dc2:	2001      	movs	r0, #1
 e002dc4:	f7ff ff40 	bl	e002c48 <__reserved_get_from_poll>
 e002dc8:	b002      	add	sp, #8
 e002dca:	bd10      	pop	{r4, pc}
 e002dcc:	2000b5d4 	.word	0x2000b5d4
 e002dd0:	2000b5dc 	.word	0x2000b5dc
 e002dd4:	2000b5d8 	.word	0x2000b5d8
 e002dd8:	2000b5e0 	.word	0x2000b5e0
 e002ddc:	2000b604 	.word	0x2000b604

0e002de0 <__reserved_get_sema_from_poll>:
 e002de0:	b510      	push	{r4, lr}
 e002de2:	4806      	ldr	r0, [pc, #24]	; (e002dfc <__reserved_get_sema_from_poll+0x1c>)
 e002de4:	b082      	sub	sp, #8
 e002de6:	4c06      	ldr	r4, [pc, #24]	; (e002e00 <__reserved_get_sema_from_poll+0x20>)
 e002de8:	4b06      	ldr	r3, [pc, #24]	; (e002e04 <__reserved_get_sema_from_poll+0x24>)
 e002dea:	4a07      	ldr	r2, [pc, #28]	; (e002e08 <__reserved_get_sema_from_poll+0x28>)
 e002dec:	4907      	ldr	r1, [pc, #28]	; (e002e0c <__reserved_get_sema_from_poll+0x2c>)
 e002dee:	e9cd 0400 	strd	r0, r4, [sp]
 e002df2:	2002      	movs	r0, #2
 e002df4:	f7ff ff28 	bl	e002c48 <__reserved_get_from_poll>
 e002df8:	b002      	add	sp, #8
 e002dfa:	bd10      	pop	{r4, pc}
 e002dfc:	2000b5e4 	.word	0x2000b5e4
 e002e00:	2000b5ec 	.word	0x2000b5ec
 e002e04:	2000b5e8 	.word	0x2000b5e8
 e002e08:	2000b5f0 	.word	0x2000b5f0
 e002e0c:	2000b60c 	.word	0x2000b60c

0e002e10 <rtos_sched_start>:
 e002e10:	b508      	push	{r3, lr}
 e002e12:	f001 fc2b 	bl	e00466c <vTaskStartScheduler>
 e002e16:	2000      	movs	r0, #0
 e002e18:	bd08      	pop	{r3, pc}
 e002e1a:	bf00      	nop

0e002e1c <rtos_sched_suspend>:
 e002e1c:	b508      	push	{r3, lr}
 e002e1e:	f001 fc75 	bl	e00470c <vTaskSuspendAll>
 e002e22:	2000      	movs	r0, #0
 e002e24:	bd08      	pop	{r3, pc}
 e002e26:	bf00      	nop

0e002e28 <rtos_sched_resume>:
 e002e28:	b508      	push	{r3, lr}
 e002e2a:	f001 fc77 	bl	e00471c <xTaskResumeAll>
 e002e2e:	2000      	movs	r0, #0
 e002e30:	bd08      	pop	{r3, pc}
 e002e32:	bf00      	nop

0e002e34 <rtos_sched_get_state>:
 e002e34:	b508      	push	{r3, lr}
 e002e36:	f001 fef7 	bl	e004c28 <xTaskGetSchedulerState>
 e002e3a:	2801      	cmp	r0, #1
 e002e3c:	d100      	bne.n	e002e40 <rtos_sched_get_state+0xc>
 e002e3e:	bd08      	pop	{r3, pc}
 e002e40:	f001 fef2 	bl	e004c28 <xTaskGetSchedulerState>
 e002e44:	2800      	cmp	r0, #0
 e002e46:	d0fa      	beq.n	e002e3e <rtos_sched_get_state+0xa>
 e002e48:	f001 feee 	bl	e004c28 <xTaskGetSchedulerState>
 e002e4c:	2802      	cmp	r0, #2
 e002e4e:	bf18      	it	ne
 e002e50:	f04f 30ff 	movne.w	r0, #4294967295	; 0xffffffff
 e002e54:	bd08      	pop	{r3, pc}
 e002e56:	bf00      	nop

0e002e58 <rtos_task_create>:
 e002e58:	b510      	push	{r4, lr}
 e002e5a:	b082      	sub	sp, #8
 e002e5c:	f8bd c010 	ldrh.w	ip, [sp, #16]
 e002e60:	f8bd 4014 	ldrh.w	r4, [sp, #20]
 e002e64:	9001      	str	r0, [sp, #4]
 e002e66:	4610      	mov	r0, r2
 e002e68:	9400      	str	r4, [sp, #0]
 e002e6a:	ea4f 029c 	mov.w	r2, ip, lsr #2
 e002e6e:	f001 fb29 	bl	e0044c4 <xTaskCreate>
 e002e72:	3801      	subs	r0, #1
 e002e74:	bf18      	it	ne
 e002e76:	f04f 30ff 	movne.w	r0, #4294967295	; 0xffffffff
 e002e7a:	b002      	add	sp, #8
 e002e7c:	bd10      	pop	{r4, pc}
 e002e7e:	bf00      	nop

0e002e80 <rtos_task_delete>:
 e002e80:	b508      	push	{r3, lr}
 e002e82:	f001 fb53 	bl	e00452c <vTaskDelete>
 e002e86:	2000      	movs	r0, #0
 e002e88:	bd08      	pop	{r3, pc}
 e002e8a:	bf00      	nop

0e002e8c <rtos_create_secure_context>:
 e002e8c:	4770      	bx	lr
 e002e8e:	bf00      	nop

0e002e90 <rtos_time_delay_ms>:
 e002e90:	b510      	push	{r4, lr}
 e002e92:	4604      	mov	r4, r0
 e002e94:	f7fe ffac 	bl	e001df0 <pmu_yield_os_check>
 e002e98:	b118      	cbz	r0, e002ea2 <rtos_time_delay_ms+0x12>
 e002e9a:	f001 fec5 	bl	e004c28 <xTaskGetSchedulerState>
 e002e9e:	2802      	cmp	r0, #2
 e002ea0:	d004      	beq.n	e002eac <rtos_time_delay_ms+0x1c>
 e002ea2:	4620      	mov	r0, r4
 e002ea4:	4b06      	ldr	r3, [pc, #24]	; (e002ec0 <rtos_time_delay_ms+0x30>)
 e002ea6:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
 e002eaa:	4718      	bx	r3
 e002eac:	f7ff fde2 	bl	e002a74 <rtos_critical_is_in_interrupt>
 e002eb0:	2800      	cmp	r0, #0
 e002eb2:	d1f6      	bne.n	e002ea2 <rtos_time_delay_ms+0x12>
 e002eb4:	4620      	mov	r0, r4
 e002eb6:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
 e002eba:	f001 bcbb 	b.w	e004834 <vTaskDelay>
 e002ebe:	bf00      	nop
 e002ec0:	0000951d 	.word	0x0000951d

0e002ec4 <platform_calloc_uninit>:
 e002ec4:	2000      	movs	r0, #0
 e002ec6:	4770      	bx	lr

0e002ec8 <platform_free_uninit>:
 e002ec8:	4770      	bx	lr
 e002eca:	bf00      	nop

0e002ecc <mbedtls_platform_set_calloc_free>:
 e002ecc:	4a06      	ldr	r2, [pc, #24]	; (e002ee8 <mbedtls_platform_set_calloc_free+0x1c>)
 e002ece:	4b07      	ldr	r3, [pc, #28]	; (e002eec <mbedtls_platform_set_calloc_free+0x20>)
 e002ed0:	b410      	push	{r4}
 e002ed2:	6011      	str	r1, [r2, #0]
 e002ed4:	4a06      	ldr	r2, [pc, #24]	; (e002ef0 <mbedtls_platform_set_calloc_free+0x24>)
 e002ed6:	4c07      	ldr	r4, [pc, #28]	; (e002ef4 <mbedtls_platform_set_calloc_free+0x28>)
 e002ed8:	6018      	str	r0, [r3, #0]
 e002eda:	6059      	str	r1, [r3, #4]
 e002edc:	6013      	str	r3, [r2, #0]
 e002ede:	6020      	str	r0, [r4, #0]
 e002ee0:	2000      	movs	r0, #0
 e002ee2:	f85d 4b04 	ldr.w	r4, [sp], #4
 e002ee6:	4770      	bx	lr
 e002ee8:	2000b060 	.word	0x2000b060
 e002eec:	3000799c 	.word	0x3000799c
 e002ef0:	30007998 	.word	0x30007998
 e002ef4:	2000b05c 	.word	0x2000b05c
	...

0e002f00 <vRestoreContextOfFirstTask>:
 e002f00:	4a07      	ldr	r2, [pc, #28]	; (e002f20 <pxCurrentTCBConst2>)
 e002f02:	6811      	ldr	r1, [r2, #0]
 e002f04:	6808      	ldr	r0, [r1, #0]
 e002f06:	c806      	ldmia	r0!, {r1, r2}
 e002f08:	f381 880b 	msr	PSPLIM, r1
 e002f0c:	2102      	movs	r1, #2
 e002f0e:	f381 8814 	msr	CONTROL, r1
 e002f12:	3020      	adds	r0, #32
 e002f14:	f380 8809 	msr	PSP, r0
 e002f18:	f3bf 8f6f 	isb	sy
 e002f1c:	4710      	bx	r2
 e002f1e:	bf00      	nop

0e002f20 <pxCurrentTCBConst2>:
 e002f20:	2000b970 	.word	0x2000b970
	...

0e002f30 <PendSV_Handler>:
 e002f30:	f3ef 8009 	mrs	r0, PSP
 e002f34:	f01e 0f10 	tst.w	lr, #16
 e002f38:	bf08      	it	eq
 e002f3a:	ed20 8a10 	vstmdbeq	r0!, {s16-s31}
 e002f3e:	f3ef 820b 	mrs	r2, PSPLIM
 e002f42:	4673      	mov	r3, lr
 e002f44:	e920 0ffc 	stmdb	r0!, {r2, r3, r4, r5, r6, r7, r8, r9, sl, fp}
 e002f48:	4a0d      	ldr	r2, [pc, #52]	; (e002f80 <pxCurrentTCBConst>)
 e002f4a:	6811      	ldr	r1, [r2, #0]
 e002f4c:	6008      	str	r0, [r1, #0]
 e002f4e:	b672      	cpsid	i
 e002f50:	f001 fd2e 	bl	e0049b0 <vTaskSwitchContext>
 e002f54:	b662      	cpsie	i
 e002f56:	4a0a      	ldr	r2, [pc, #40]	; (e002f80 <pxCurrentTCBConst>)
 e002f58:	6811      	ldr	r1, [r2, #0]
 e002f5a:	6808      	ldr	r0, [r1, #0]
 e002f5c:	e8b0 0ffc 	ldmia.w	r0!, {r2, r3, r4, r5, r6, r7, r8, r9, sl, fp}
 e002f60:	f013 0f10 	tst.w	r3, #16
 e002f64:	bf08      	it	eq
 e002f66:	ecb0 8a10 	vldmiaeq	r0!, {s16-s31}
 e002f6a:	f382 880b 	msr	PSPLIM, r2
 e002f6e:	f380 8809 	msr	PSP, r0
 e002f72:	4718      	bx	r3
 e002f74:	f3af 8000 	nop.w
 e002f78:	f3af 8000 	nop.w
 e002f7c:	f3af 8000 	nop.w

0e002f80 <pxCurrentTCBConst>:
 e002f80:	2000b970 	.word	0x2000b970

0e002f84 <xMAIR0Const>:
 e002f84:	e000edc0 	.word	0xe000edc0

0e002f88 <xRNRConst>:
 e002f88:	e000ed98 	.word	0xe000ed98

0e002f8c <xRBARConst>:
 e002f8c:	e000ed9c 	.word	0xe000ed9c

0e002f90 <vStartFirstTask>:
 e002f90:	4807      	ldr	r0, [pc, #28]	; (e002fb0 <xVTORConst>)
 e002f92:	6800      	ldr	r0, [r0, #0]
 e002f94:	6800      	ldr	r0, [r0, #0]
 e002f96:	f380 8808 	msr	MSP, r0
 e002f9a:	b662      	cpsie	i
 e002f9c:	b661      	cpsie	f
 e002f9e:	f3bf 8f4f 	dsb	sy
 e002fa2:	f3bf 8f6f 	isb	sy
 e002fa6:	df02      	svc	2
 e002fa8:	bf00      	nop
 e002faa:	bf00      	nop
 e002fac:	f3af 8000 	nop.w

0e002fb0 <xVTORConst>:
 e002fb0:	e000ed08 	.word	0xe000ed08

0e002fb4 <ulSetInterruptMaskFromISR>:
 e002fb4:	f3ef 8010 	mrs	r0, PRIMASK
 e002fb8:	b672      	cpsid	i
 e002fba:	4770      	bx	lr
 e002fbc:	2000      	movs	r0, #0
 e002fbe:	bf00      	nop

0e002fc0 <vClearInterruptMaskFromISR>:
 e002fc0:	f380 8810 	msr	PRIMASK, r0
 e002fc4:	4770      	bx	lr
 e002fc6:	bf00      	nop
	...

0e002fd0 <SVC_Handler>:
 e002fd0:	f01e 0f04 	tst.w	lr, #4
 e002fd4:	bf0c      	ite	eq
 e002fd6:	f3ef 8008 	mrseq	r0, MSP
 e002fda:	f3ef 8009 	mrsne	r0, PSP
 e002fde:	4904      	ldr	r1, [pc, #16]	; (e002ff0 <svchandler_address_const>)
 e002fe0:	4708      	bx	r1
 e002fe2:	bf00      	nop
 e002fe4:	f3af 8000 	nop.w
 e002fe8:	f3af 8000 	nop.w
 e002fec:	f3af 8000 	nop.w

0e002ff0 <svchandler_address_const>:
 e002ff0:	0e003095 	.word	0x0e003095

0e002ff4 <SysTick_Handler>:
 e002ff4:	b510      	push	{r4, lr}
 e002ff6:	f7ff ffdd 	bl	e002fb4 <ulSetInterruptMaskFromISR>
 e002ffa:	4604      	mov	r4, r0
 e002ffc:	f001 fcc8 	bl	e004990 <xTaskIncrementTick>
 e003000:	b128      	cbz	r0, e00300e <SysTick_Handler+0x1a>
 e003002:	f04f 23e0 	mov.w	r3, #3758153728	; 0xe000e000
 e003006:	f04f 5280 	mov.w	r2, #268435456	; 0x10000000
 e00300a:	f8c3 2d04 	str.w	r2, [r3, #3332]	; 0xd04
 e00300e:	4620      	mov	r0, r4
 e003010:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
 e003014:	f7ff bfd4 	b.w	e002fc0 <vClearInterruptMaskFromISR>

0e003018 <vPortYield>:
 e003018:	f04f 23e0 	mov.w	r3, #3758153728	; 0xe000e000
 e00301c:	f04f 5280 	mov.w	r2, #268435456	; 0x10000000
 e003020:	f8c3 2d04 	str.w	r2, [r3, #3332]	; 0xd04
 e003024:	f3bf 8f4f 	dsb	sy
 e003028:	f3bf 8f6f 	isb	sy
 e00302c:	4770      	bx	lr
 e00302e:	bf00      	nop

0e003030 <ulPortSetInterruptMask>:
 e003030:	f3ef 8011 	mrs	r0, BASEPRI
 e003034:	f04f 0140 	mov.w	r1, #64	; 0x40
 e003038:	f381 8811 	msr	BASEPRI, r1
 e00303c:	4770      	bx	lr
 e00303e:	2000      	movs	r0, #0

0e003040 <vPortEnterCritical>:
 e003040:	b508      	push	{r3, lr}
 e003042:	f7ff fff5 	bl	e003030 <ulPortSetInterruptMask>
 e003046:	4a04      	ldr	r2, [pc, #16]	; (e003058 <vPortEnterCritical+0x18>)
 e003048:	6813      	ldr	r3, [r2, #0]
 e00304a:	3301      	adds	r3, #1
 e00304c:	6013      	str	r3, [r2, #0]
 e00304e:	f3bf 8f4f 	dsb	sy
 e003052:	f3bf 8f6f 	isb	sy
 e003056:	bd08      	pop	{r3, pc}
 e003058:	2000b61c 	.word	0x2000b61c

0e00305c <prvTaskExitError>:
 e00305c:	b500      	push	{lr}
 e00305e:	2300      	movs	r3, #0
 e003060:	b083      	sub	sp, #12
 e003062:	9301      	str	r3, [sp, #4]
 e003064:	f7ff ffe4 	bl	e003030 <ulPortSetInterruptMask>
 e003068:	9b01      	ldr	r3, [sp, #4]
 e00306a:	2b00      	cmp	r3, #0
 e00306c:	d0fc      	beq.n	e003068 <prvTaskExitError+0xc>
 e00306e:	b003      	add	sp, #12
 e003070:	f85d fb04 	ldr.w	pc, [sp], #4

0e003074 <vPortClearInterruptMask>:
 e003074:	f380 8811 	msr	BASEPRI, r0
 e003078:	4770      	bx	lr
 e00307a:	bf00      	nop

0e00307c <vPortExitCritical>:
 e00307c:	4b04      	ldr	r3, [pc, #16]	; (e003090 <vPortExitCritical+0x14>)
 e00307e:	681a      	ldr	r2, [r3, #0]
 e003080:	3a01      	subs	r2, #1
 e003082:	601a      	str	r2, [r3, #0]
 e003084:	6818      	ldr	r0, [r3, #0]
 e003086:	b100      	cbz	r0, e00308a <vPortExitCritical+0xe>
 e003088:	4770      	bx	lr
 e00308a:	f7ff bff3 	b.w	e003074 <vPortClearInterruptMask>
 e00308e:	bf00      	nop
 e003090:	2000b61c 	.word	0x2000b61c

0e003094 <vPortSVCHandler_C>:
 e003094:	6983      	ldr	r3, [r0, #24]
 e003096:	f813 3c02 	ldrb.w	r3, [r3, #-2]
 e00309a:	2b02      	cmp	r3, #2
 e00309c:	d000      	beq.n	e0030a0 <vPortSVCHandler_C+0xc>
 e00309e:	4770      	bx	lr
 e0030a0:	f04f 23e0 	mov.w	r3, #3758153728	; 0xe000e000
 e0030a4:	f8d3 2d88 	ldr.w	r2, [r3, #3464]	; 0xd88
 e0030a8:	f442 0270 	orr.w	r2, r2, #15728640	; 0xf00000
 e0030ac:	f8c3 2d88 	str.w	r2, [r3, #3464]	; 0xd88
 e0030b0:	f8d3 2f34 	ldr.w	r2, [r3, #3892]	; 0xf34
 e0030b4:	f042 4240 	orr.w	r2, r2, #3221225472	; 0xc0000000
 e0030b8:	f8c3 2f34 	str.w	r2, [r3, #3892]	; 0xf34
 e0030bc:	f7ff bf20 	b.w	e002f00 <vRestoreContextOfFirstTask>

0e0030c0 <pxPortInitialiseStack>:
 e0030c0:	b430      	push	{r4, r5}
 e0030c2:	f840 2c08 	str.w	r2, [r0, #-8]
 e0030c6:	f04f 3211 	mov.w	r2, #286331153	; 0x11111111
 e0030ca:	4c1e      	ldr	r4, [pc, #120]	; (e003144 <pxPortInitialiseStack+0x84>)
 e0030cc:	f04f 7580 	mov.w	r5, #16777216	; 0x1000000
 e0030d0:	f840 2c24 	str.w	r2, [r0, #-36]
 e0030d4:	f04f 3210 	mov.w	r2, #269488144	; 0x10101010
 e0030d8:	f840 4c0c 	str.w	r4, [r0, #-12]
 e0030dc:	f04f 3412 	mov.w	r4, #303174162	; 0x12121212
 e0030e0:	f840 2c28 	str.w	r2, [r0, #-40]
 e0030e4:	f04f 3209 	mov.w	r2, #151587081	; 0x9090909
 e0030e8:	f840 4c10 	str.w	r4, [r0, #-16]
 e0030ec:	f04f 3403 	mov.w	r4, #50529027	; 0x3030303
 e0030f0:	f840 2c2c 	str.w	r2, [r0, #-44]
 e0030f4:	f04f 3208 	mov.w	r2, #134744072	; 0x8080808
 e0030f8:	f840 4c14 	str.w	r4, [r0, #-20]
 e0030fc:	f04f 3402 	mov.w	r4, #33686018	; 0x2020202
 e003100:	f840 2c30 	str.w	r2, [r0, #-48]
 e003104:	f04f 3207 	mov.w	r2, #117901063	; 0x7070707
 e003108:	f840 4c18 	str.w	r4, [r0, #-24]
 e00310c:	f04f 3401 	mov.w	r4, #16843009	; 0x1010101
 e003110:	f840 2c34 	str.w	r2, [r0, #-52]
 e003114:	f04f 3205 	mov.w	r2, #84215045	; 0x5050505
 e003118:	f840 4c1c 	str.w	r4, [r0, #-28]
 e00311c:	f04f 3406 	mov.w	r4, #101058054	; 0x6060606
 e003120:	f840 3c20 	str.w	r3, [r0, #-32]
 e003124:	4603      	mov	r3, r0
 e003126:	f840 5c04 	str.w	r5, [r0, #-4]
 e00312a:	e940 240f 	strd	r2, r4, [r0, #-60]	; 0x3c
 e00312e:	f04f 3404 	mov.w	r4, #67372036	; 0x4040404
 e003132:	f06f 0202 	mvn.w	r2, #2
 e003136:	e940 2411 	strd	r2, r4, [r0, #-68]	; 0x44
 e00313a:	f843 1d48 	str.w	r1, [r3, #-72]!
 e00313e:	4618      	mov	r0, r3
 e003140:	bc30      	pop	{r4, r5}
 e003142:	4770      	bx	lr
 e003144:	0e00305d 	.word	0x0e00305d

0e003148 <xPortStartScheduler>:
 e003148:	f04f 23e0 	mov.w	r3, #3758153728	; 0xe000e000
 e00314c:	4a1c      	ldr	r2, [pc, #112]	; (e0031c0 <xPortStartScheduler+0x78>)
 e00314e:	481d      	ldr	r0, [pc, #116]	; (e0031c4 <xPortStartScheduler+0x7c>)
 e003150:	b510      	push	{r4, lr}
 e003152:	f8d3 1d20 	ldr.w	r1, [r3, #3360]	; 0xd20
 e003156:	b082      	sub	sp, #8
 e003158:	f441 017f 	orr.w	r1, r1, #16711680	; 0xff0000
 e00315c:	f8c3 1d20 	str.w	r1, [r3, #3360]	; 0xd20
 e003160:	f8d3 1d20 	ldr.w	r1, [r3, #3360]	; 0xd20
 e003164:	f041 417f 	orr.w	r1, r1, #4278190080	; 0xff000000
 e003168:	f8c3 1d20 	str.w	r1, [r3, #3360]	; 0xd20
 e00316c:	6891      	ldr	r1, [r2, #8]
 e00316e:	62c8      	str	r0, [r1, #44]	; 0x2c
 e003170:	f3bf 8f4f 	dsb	sy
 e003174:	4814      	ldr	r0, [pc, #80]	; (e0031c8 <xPortStartScheduler+0x80>)
 e003176:	6891      	ldr	r1, [r2, #8]
 e003178:	6388      	str	r0, [r1, #56]	; 0x38
 e00317a:	f3bf 8f4f 	dsb	sy
 e00317e:	4913      	ldr	r1, [pc, #76]	; (e0031cc <xPortStartScheduler+0x84>)
 e003180:	6892      	ldr	r2, [r2, #8]
 e003182:	63d1      	str	r1, [r2, #60]	; 0x3c
 e003184:	f3bf 8f4f 	dsb	sy
 e003188:	2400      	movs	r4, #0
 e00318a:	4a11      	ldr	r2, [pc, #68]	; (e0031d0 <xPortStartScheduler+0x88>)
 e00318c:	4811      	ldr	r0, [pc, #68]	; (e0031d4 <xPortStartScheduler+0x8c>)
 e00318e:	2107      	movs	r1, #7
 e003190:	611c      	str	r4, [r3, #16]
 e003192:	619c      	str	r4, [r3, #24]
 e003194:	6812      	ldr	r2, [r2, #0]
 e003196:	fba0 0202 	umull	r0, r2, r0, r2
 e00319a:	0992      	lsrs	r2, r2, #6
 e00319c:	3a01      	subs	r2, #1
 e00319e:	615a      	str	r2, [r3, #20]
 e0031a0:	4a0d      	ldr	r2, [pc, #52]	; (e0031d8 <xPortStartScheduler+0x90>)
 e0031a2:	6119      	str	r1, [r3, #16]
 e0031a4:	6014      	str	r4, [r2, #0]
 e0031a6:	f7ff fef3 	bl	e002f90 <vStartFirstTask>
 e0031aa:	f001 fc01 	bl	e0049b0 <vTaskSwitchContext>
 e0031ae:	9401      	str	r4, [sp, #4]
 e0031b0:	f7ff ff3e 	bl	e003030 <ulPortSetInterruptMask>
 e0031b4:	9b01      	ldr	r3, [sp, #4]
 e0031b6:	2b00      	cmp	r3, #0
 e0031b8:	d0fc      	beq.n	e0031b4 <xPortStartScheduler+0x6c>
 e0031ba:	2000      	movs	r0, #0
 e0031bc:	b002      	add	sp, #8
 e0031be:	bd10      	pop	{r4, pc}
 e0031c0:	e000ed00 	.word	0xe000ed00
 e0031c4:	0e002fd1 	.word	0x0e002fd1
 e0031c8:	0e002f31 	.word	0x0e002f31
 e0031cc:	0e002ff5 	.word	0x0e002ff5
 e0031d0:	2000b020 	.word	0x2000b020
 e0031d4:	10624dd3 	.word	0x10624dd3
 e0031d8:	2000b61c 	.word	0x2000b61c

0e0031dc <vApplicationIdleHook>:
 e0031dc:	4770      	bx	lr
 e0031de:	bf00      	nop

0e0031e0 <vApplicationStackOverflowHook>:
 e0031e0:	460a      	mov	r2, r1
 e0031e2:	4803      	ldr	r0, [pc, #12]	; (e0031f0 <vApplicationStackOverflowHook+0x10>)
 e0031e4:	4903      	ldr	r1, [pc, #12]	; (e0031f4 <vApplicationStackOverflowHook+0x14>)
 e0031e6:	b508      	push	{r3, lr}
 e0031e8:	f003 ff1c 	bl	e007024 <DiagPrintf_minimal>
 e0031ec:	e7fe      	b.n	e0031ec <vApplicationStackOverflowHook+0xc>
 e0031ee:	bf00      	nop
 e0031f0:	0e008654 	.word	0x0e008654
 e0031f4:	0e0086c4 	.word	0x0e0086c4

0e0031f8 <vApplicationMallocFailedHook>:
 e0031f8:	b500      	push	{lr}
 e0031fa:	b083      	sub	sp, #12
 e0031fc:	f001 fd14 	bl	e004c28 <xTaskGetSchedulerState>
 e003200:	2801      	cmp	r0, #1
 e003202:	d10c      	bne.n	e00321e <vApplicationMallocFailedHook+0x26>
 e003204:	4a08      	ldr	r2, [pc, #32]	; (e003228 <vApplicationMallocFailedHook+0x30>)
 e003206:	9201      	str	r2, [sp, #4]
 e003208:	f7ff f816 	bl	e002238 <xPortGetFreeHeapSize>
 e00320c:	9a01      	ldr	r2, [sp, #4]
 e00320e:	4603      	mov	r3, r0
 e003210:	4906      	ldr	r1, [pc, #24]	; (e00322c <vApplicationMallocFailedHook+0x34>)
 e003212:	4807      	ldr	r0, [pc, #28]	; (e003230 <vApplicationMallocFailedHook+0x38>)
 e003214:	f003 ff06 	bl	e007024 <DiagPrintf_minimal>
 e003218:	f7ff ff0a 	bl	e003030 <ulPortSetInterruptMask>
 e00321c:	e7fe      	b.n	e00321c <vApplicationMallocFailedHook+0x24>
 e00321e:	2000      	movs	r0, #0
 e003220:	f001 fbac 	bl	e00497c <pcTaskGetName>
 e003224:	4602      	mov	r2, r0
 e003226:	e7ee      	b.n	e003206 <vApplicationMallocFailedHook+0xe>
 e003228:	0e00867c 	.word	0x0e00867c
 e00322c:	0e008684 	.word	0x0e008684
 e003230:	0e008688 	.word	0x0e008688

0e003234 <vApplicationGetIdleTaskMemory>:
 e003234:	4b05      	ldr	r3, [pc, #20]	; (e00324c <vApplicationGetIdleTaskMemory+0x18>)
 e003236:	b410      	push	{r4}
 e003238:	6003      	str	r3, [r0, #0]
 e00323a:	f44f 7300 	mov.w	r3, #512	; 0x200
 e00323e:	4c04      	ldr	r4, [pc, #16]	; (e003250 <vApplicationGetIdleTaskMemory+0x1c>)
 e003240:	600c      	str	r4, [r1, #0]
 e003242:	f85d 4b04 	ldr.w	r4, [sp], #4
 e003246:	6013      	str	r3, [r2, #0]
 e003248:	4770      	bx	lr
 e00324a:	bf00      	nop
 e00324c:	2000b620 	.word	0x2000b620
 e003250:	20006000 	.word	0x20006000

0e003254 <vApplicationGetTimerTaskMemory>:
 e003254:	4b05      	ldr	r3, [pc, #20]	; (e00326c <vApplicationGetTimerTaskMemory+0x18>)
 e003256:	b410      	push	{r4}
 e003258:	6003      	str	r3, [r0, #0]
 e00325a:	f44f 7300 	mov.w	r3, #512	; 0x200
 e00325e:	4c04      	ldr	r4, [pc, #16]	; (e003270 <vApplicationGetTimerTaskMemory+0x1c>)
 e003260:	600c      	str	r4, [r1, #0]
 e003262:	f85d 4b04 	ldr.w	r4, [sp], #4
 e003266:	6013      	str	r3, [r2, #0]
 e003268:	4770      	bx	lr
 e00326a:	bf00      	nop
 e00326c:	2000b784 	.word	0x2000b784
 e003270:	20006800 	.word	0x20006800

0e003274 <vPortSuppressTicksAndSleep>:
 e003274:	b510      	push	{r4, lr}
 e003276:	b672      	cpsid	i
 e003278:	2000      	movs	r0, #0
 e00327a:	4c0e      	ldr	r4, [pc, #56]	; (e0032b4 <vPortSuppressTicksAndSleep+0x40>)
 e00327c:	f7fd fa38 	bl	e0006f0 <Systick_Cmd>
 e003280:	2300      	movs	r3, #0
 e003282:	6023      	str	r3, [r4, #0]
 e003284:	f001 fc5e 	bl	e004b44 <eTaskConfirmSleepModeStatus>
 e003288:	b928      	cbnz	r0, e003296 <vPortSuppressTicksAndSleep+0x22>
 e00328a:	2001      	movs	r0, #1
 e00328c:	6020      	str	r0, [r4, #0]
 e00328e:	f7fd fa2f 	bl	e0006f0 <Systick_Cmd>
 e003292:	b662      	cpsie	i
 e003294:	bd10      	pop	{r4, pc}
 e003296:	f7fe fdb1 	bl	e001dfc <pmu_ready_to_sleep>
 e00329a:	2800      	cmp	r0, #0
 e00329c:	d1f5      	bne.n	e00328a <vPortSuppressTicksAndSleep+0x16>
 e00329e:	2001      	movs	r0, #1
 e0032a0:	f7fd fa26 	bl	e0006f0 <Systick_Cmd>
 e0032a4:	f3bf 8f4f 	dsb	sy
 e0032a8:	bf30      	wfi
 e0032aa:	bf00      	nop
 e0032ac:	f3bf 8f6f 	isb	sy
 e0032b0:	e7eb      	b.n	e00328a <vPortSuppressTicksAndSleep+0x16>
 e0032b2:	bf00      	nop
 e0032b4:	2000b024 	.word	0x2000b024

0e0032b8 <prvCopyDataToQueue>:
 e0032b8:	b570      	push	{r4, r5, r6, lr}
 e0032ba:	4616      	mov	r6, r2
 e0032bc:	6c02      	ldr	r2, [r0, #64]	; 0x40
 e0032be:	4604      	mov	r4, r0
 e0032c0:	6b85      	ldr	r5, [r0, #56]	; 0x38
 e0032c2:	b92a      	cbnz	r2, e0032d0 <prvCopyDataToQueue+0x18>
 e0032c4:	6806      	ldr	r6, [r0, #0]
 e0032c6:	3501      	adds	r5, #1
 e0032c8:	b356      	cbz	r6, e003320 <prvCopyDataToQueue+0x68>
 e0032ca:	2000      	movs	r0, #0
 e0032cc:	63a5      	str	r5, [r4, #56]	; 0x38
 e0032ce:	bd70      	pop	{r4, r5, r6, pc}
 e0032d0:	b97e      	cbnz	r6, e0032f2 <prvCopyDataToQueue+0x3a>
 e0032d2:	6840      	ldr	r0, [r0, #4]
 e0032d4:	3501      	adds	r5, #1
 e0032d6:	f004 fb8f 	bl	e0079f8 <____wrap_memcpy_veneer>
 e0032da:	6863      	ldr	r3, [r4, #4]
 e0032dc:	6c22      	ldr	r2, [r4, #64]	; 0x40
 e0032de:	4413      	add	r3, r2
 e0032e0:	68a2      	ldr	r2, [r4, #8]
 e0032e2:	4293      	cmp	r3, r2
 e0032e4:	6063      	str	r3, [r4, #4]
 e0032e6:	d318      	bcc.n	e00331a <prvCopyDataToQueue+0x62>
 e0032e8:	6823      	ldr	r3, [r4, #0]
 e0032ea:	4630      	mov	r0, r6
 e0032ec:	63a5      	str	r5, [r4, #56]	; 0x38
 e0032ee:	6063      	str	r3, [r4, #4]
 e0032f0:	bd70      	pop	{r4, r5, r6, pc}
 e0032f2:	68c0      	ldr	r0, [r0, #12]
 e0032f4:	f004 fb80 	bl	e0079f8 <____wrap_memcpy_veneer>
 e0032f8:	6c22      	ldr	r2, [r4, #64]	; 0x40
 e0032fa:	68e3      	ldr	r3, [r4, #12]
 e0032fc:	4251      	negs	r1, r2
 e0032fe:	1a9b      	subs	r3, r3, r2
 e003300:	6822      	ldr	r2, [r4, #0]
 e003302:	4293      	cmp	r3, r2
 e003304:	60e3      	str	r3, [r4, #12]
 e003306:	d202      	bcs.n	e00330e <prvCopyDataToQueue+0x56>
 e003308:	68a3      	ldr	r3, [r4, #8]
 e00330a:	440b      	add	r3, r1
 e00330c:	60e3      	str	r3, [r4, #12]
 e00330e:	2e02      	cmp	r6, #2
 e003310:	d00b      	beq.n	e00332a <prvCopyDataToQueue+0x72>
 e003312:	3501      	adds	r5, #1
 e003314:	2000      	movs	r0, #0
 e003316:	63a5      	str	r5, [r4, #56]	; 0x38
 e003318:	bd70      	pop	{r4, r5, r6, pc}
 e00331a:	4630      	mov	r0, r6
 e00331c:	63a5      	str	r5, [r4, #56]	; 0x38
 e00331e:	bd70      	pop	{r4, r5, r6, pc}
 e003320:	6880      	ldr	r0, [r0, #8]
 e003322:	f001 fcd7 	bl	e004cd4 <xTaskPriorityDisinherit>
 e003326:	60a6      	str	r6, [r4, #8]
 e003328:	e7d0      	b.n	e0032cc <prvCopyDataToQueue+0x14>
 e00332a:	2d01      	cmp	r5, #1
 e00332c:	bf38      	it	cc
 e00332e:	2501      	movcc	r5, #1
 e003330:	e7cb      	b.n	e0032ca <prvCopyDataToQueue+0x12>
 e003332:	bf00      	nop

0e003334 <prvNotifyQueueSetContainer>:
 e003334:	b5f0      	push	{r4, r5, r6, r7, lr}
 e003336:	6c84      	ldr	r4, [r0, #72]	; 0x48
 e003338:	b083      	sub	sp, #12
 e00333a:	6ba2      	ldr	r2, [r4, #56]	; 0x38
 e00333c:	6be3      	ldr	r3, [r4, #60]	; 0x3c
 e00333e:	9001      	str	r0, [sp, #4]
 e003340:	429a      	cmp	r2, r3
 e003342:	d303      	bcc.n	e00334c <prvNotifyQueueSetContainer+0x18>
 e003344:	2600      	movs	r6, #0
 e003346:	4630      	mov	r0, r6
 e003348:	b003      	add	sp, #12
 e00334a:	bdf0      	pop	{r4, r5, r6, r7, pc}
 e00334c:	f894 5045 	ldrb.w	r5, [r4, #69]	; 0x45
 e003350:	6c22      	ldr	r2, [r4, #64]	; 0x40
 e003352:	6ba7      	ldr	r7, [r4, #56]	; 0x38
 e003354:	b26d      	sxtb	r5, r5
 e003356:	b1c2      	cbz	r2, e00338a <prvNotifyQueueSetContainer+0x56>
 e003358:	a901      	add	r1, sp, #4
 e00335a:	6860      	ldr	r0, [r4, #4]
 e00335c:	f004 fb4c 	bl	e0079f8 <____wrap_memcpy_veneer>
 e003360:	6862      	ldr	r2, [r4, #4]
 e003362:	6c23      	ldr	r3, [r4, #64]	; 0x40
 e003364:	441a      	add	r2, r3
 e003366:	68a3      	ldr	r3, [r4, #8]
 e003368:	429a      	cmp	r2, r3
 e00336a:	6062      	str	r2, [r4, #4]
 e00336c:	d30f      	bcc.n	e00338e <prvNotifyQueueSetContainer+0x5a>
 e00336e:	6823      	ldr	r3, [r4, #0]
 e003370:	2600      	movs	r6, #0
 e003372:	6063      	str	r3, [r4, #4]
 e003374:	3701      	adds	r7, #1
 e003376:	1c6b      	adds	r3, r5, #1
 e003378:	63a7      	str	r7, [r4, #56]	; 0x38
 e00337a:	d00e      	beq.n	e00339a <prvNotifyQueueSetContainer+0x66>
 e00337c:	3501      	adds	r5, #1
 e00337e:	4630      	mov	r0, r6
 e003380:	b26d      	sxtb	r5, r5
 e003382:	f884 5045 	strb.w	r5, [r4, #69]	; 0x45
 e003386:	b003      	add	sp, #12
 e003388:	bdf0      	pop	{r4, r5, r6, r7, pc}
 e00338a:	6826      	ldr	r6, [r4, #0]
 e00338c:	b186      	cbz	r6, e0033b0 <prvNotifyQueueSetContainer+0x7c>
 e00338e:	3701      	adds	r7, #1
 e003390:	1c6b      	adds	r3, r5, #1
 e003392:	f04f 0600 	mov.w	r6, #0
 e003396:	63a7      	str	r7, [r4, #56]	; 0x38
 e003398:	d1f0      	bne.n	e00337c <prvNotifyQueueSetContainer+0x48>
 e00339a:	6a63      	ldr	r3, [r4, #36]	; 0x24
 e00339c:	2b00      	cmp	r3, #0
 e00339e:	d0d2      	beq.n	e003346 <prvNotifyQueueSetContainer+0x12>
 e0033a0:	f104 0024 	add.w	r0, r4, #36	; 0x24
 e0033a4:	f001 fb38 	bl	e004a18 <xTaskRemoveFromEventList>
 e0033a8:	2800      	cmp	r0, #0
 e0033aa:	bf18      	it	ne
 e0033ac:	2601      	movne	r6, #1
 e0033ae:	e7ca      	b.n	e003346 <prvNotifyQueueSetContainer+0x12>
 e0033b0:	68a0      	ldr	r0, [r4, #8]
 e0033b2:	f001 fc8f 	bl	e004cd4 <xTaskPriorityDisinherit>
 e0033b6:	60a6      	str	r6, [r4, #8]
 e0033b8:	4606      	mov	r6, r0
 e0033ba:	e7db      	b.n	e003374 <prvNotifyQueueSetContainer+0x40>

0e0033bc <prvUnlockQueue>:
 e0033bc:	b570      	push	{r4, r5, r6, lr}
 e0033be:	4605      	mov	r5, r0
 e0033c0:	f7ff fe3e 	bl	e003040 <vPortEnterCritical>
 e0033c4:	f895 4045 	ldrb.w	r4, [r5, #69]	; 0x45
 e0033c8:	b264      	sxtb	r4, r4
 e0033ca:	2c00      	cmp	r4, #0
 e0033cc:	dd19      	ble.n	e003402 <prvUnlockQueue+0x46>
 e0033ce:	f105 0624 	add.w	r6, r5, #36	; 0x24
 e0033d2:	e005      	b.n	e0033e0 <prvUnlockQueue+0x24>
 e0033d4:	f7ff ffae 	bl	e003334 <prvNotifyQueueSetContainer>
 e0033d8:	b968      	cbnz	r0, e0033f6 <prvUnlockQueue+0x3a>
 e0033da:	3c01      	subs	r4, #1
 e0033dc:	b264      	sxtb	r4, r4
 e0033de:	b184      	cbz	r4, e003402 <prvUnlockQueue+0x46>
 e0033e0:	6cab      	ldr	r3, [r5, #72]	; 0x48
 e0033e2:	4628      	mov	r0, r5
 e0033e4:	2b00      	cmp	r3, #0
 e0033e6:	d1f5      	bne.n	e0033d4 <prvUnlockQueue+0x18>
 e0033e8:	6a6b      	ldr	r3, [r5, #36]	; 0x24
 e0033ea:	4630      	mov	r0, r6
 e0033ec:	b14b      	cbz	r3, e003402 <prvUnlockQueue+0x46>
 e0033ee:	f001 fb13 	bl	e004a18 <xTaskRemoveFromEventList>
 e0033f2:	2800      	cmp	r0, #0
 e0033f4:	d0f1      	beq.n	e0033da <prvUnlockQueue+0x1e>
 e0033f6:	3c01      	subs	r4, #1
 e0033f8:	f001 fb9e 	bl	e004b38 <vTaskMissedYield>
 e0033fc:	b264      	sxtb	r4, r4
 e0033fe:	2c00      	cmp	r4, #0
 e003400:	d1ee      	bne.n	e0033e0 <prvUnlockQueue+0x24>
 e003402:	23ff      	movs	r3, #255	; 0xff
 e003404:	f885 3045 	strb.w	r3, [r5, #69]	; 0x45
 e003408:	f7ff fe38 	bl	e00307c <vPortExitCritical>
 e00340c:	f7ff fe18 	bl	e003040 <vPortEnterCritical>
 e003410:	f895 4044 	ldrb.w	r4, [r5, #68]	; 0x44
 e003414:	b264      	sxtb	r4, r4
 e003416:	2c00      	cmp	r4, #0
 e003418:	dd12      	ble.n	e003440 <prvUnlockQueue+0x84>
 e00341a:	f105 0610 	add.w	r6, r5, #16
 e00341e:	e002      	b.n	e003426 <prvUnlockQueue+0x6a>
 e003420:	3c01      	subs	r4, #1
 e003422:	b264      	sxtb	r4, r4
 e003424:	b164      	cbz	r4, e003440 <prvUnlockQueue+0x84>
 e003426:	692b      	ldr	r3, [r5, #16]
 e003428:	4630      	mov	r0, r6
 e00342a:	b14b      	cbz	r3, e003440 <prvUnlockQueue+0x84>
 e00342c:	f001 faf4 	bl	e004a18 <xTaskRemoveFromEventList>
 e003430:	2800      	cmp	r0, #0
 e003432:	d0f5      	beq.n	e003420 <prvUnlockQueue+0x64>
 e003434:	3c01      	subs	r4, #1
 e003436:	f001 fb7f 	bl	e004b38 <vTaskMissedYield>
 e00343a:	b264      	sxtb	r4, r4
 e00343c:	2c00      	cmp	r4, #0
 e00343e:	d1f2      	bne.n	e003426 <prvUnlockQueue+0x6a>
 e003440:	23ff      	movs	r3, #255	; 0xff
 e003442:	f885 3044 	strb.w	r3, [r5, #68]	; 0x44
 e003446:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
 e00344a:	f7ff be17 	b.w	e00307c <vPortExitCritical>
 e00344e:	bf00      	nop

0e003450 <xQueueGenericCreateStatic>:
 e003450:	b570      	push	{r4, r5, r6, lr}
 e003452:	461c      	mov	r4, r3
 e003454:	f89d 5010 	ldrb.w	r5, [sp, #16]
 e003458:	b34b      	cbz	r3, e0034ae <xQueueGenericCreateStatic+0x5e>
 e00345a:	2900      	cmp	r1, #0
 e00345c:	bf08      	it	eq
 e00345e:	461a      	moveq	r2, r3
 e003460:	63d8      	str	r0, [r3, #60]	; 0x3c
 e003462:	6419      	str	r1, [r3, #64]	; 0x40
 e003464:	2600      	movs	r6, #0
 e003466:	601a      	str	r2, [r3, #0]
 e003468:	2301      	movs	r3, #1
 e00346a:	f884 3046 	strb.w	r3, [r4, #70]	; 0x46
 e00346e:	f7ff fde7 	bl	e003040 <vPortEnterCritical>
 e003472:	6822      	ldr	r2, [r4, #0]
 e003474:	63a6      	str	r6, [r4, #56]	; 0x38
 e003476:	f104 0010 	add.w	r0, r4, #16
 e00347a:	6062      	str	r2, [r4, #4]
 e00347c:	e9d4 310f 	ldrd	r3, r1, [r4, #60]	; 0x3c
 e003480:	fb01 f303 	mul.w	r3, r1, r3
 e003484:	1a59      	subs	r1, r3, r1
 e003486:	4413      	add	r3, r2
 e003488:	440a      	add	r2, r1
 e00348a:	60a3      	str	r3, [r4, #8]
 e00348c:	23ff      	movs	r3, #255	; 0xff
 e00348e:	60e2      	str	r2, [r4, #12]
 e003490:	f884 3044 	strb.w	r3, [r4, #68]	; 0x44
 e003494:	f884 3045 	strb.w	r3, [r4, #69]	; 0x45
 e003498:	f7fe ff24 	bl	e0022e4 <vListInitialise>
 e00349c:	f104 0024 	add.w	r0, r4, #36	; 0x24
 e0034a0:	f7fe ff20 	bl	e0022e4 <vListInitialise>
 e0034a4:	f7ff fdea 	bl	e00307c <vPortExitCritical>
 e0034a8:	f884 5050 	strb.w	r5, [r4, #80]	; 0x50
 e0034ac:	64a6      	str	r6, [r4, #72]	; 0x48
 e0034ae:	4620      	mov	r0, r4
 e0034b0:	bd70      	pop	{r4, r5, r6, pc}
 e0034b2:	bf00      	nop

0e0034b4 <xQueueGenericCreate>:
 e0034b4:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 e0034b6:	4606      	mov	r6, r0
 e0034b8:	fb01 f000 	mul.w	r0, r1, r0
 e0034bc:	460d      	mov	r5, r1
 e0034be:	4617      	mov	r7, r2
 e0034c0:	3054      	adds	r0, #84	; 0x54
 e0034c2:	f7fe fe27 	bl	e002114 <pvPortMalloc>
 e0034c6:	4604      	mov	r4, r0
 e0034c8:	b348      	cbz	r0, e00351e <xQueueGenericCreate+0x6a>
 e0034ca:	2300      	movs	r3, #0
 e0034cc:	f880 3046 	strb.w	r3, [r0, #70]	; 0x46
 e0034d0:	b33d      	cbz	r5, e003522 <xQueueGenericCreate+0x6e>
 e0034d2:	f100 0354 	add.w	r3, r0, #84	; 0x54
 e0034d6:	6023      	str	r3, [r4, #0]
 e0034d8:	e9c4 650f 	strd	r6, r5, [r4, #60]	; 0x3c
 e0034dc:	f7ff fdb0 	bl	e003040 <vPortEnterCritical>
 e0034e0:	6821      	ldr	r1, [r4, #0]
 e0034e2:	2500      	movs	r5, #0
 e0034e4:	f104 0010 	add.w	r0, r4, #16
 e0034e8:	6061      	str	r1, [r4, #4]
 e0034ea:	63a5      	str	r5, [r4, #56]	; 0x38
 e0034ec:	e9d4 320f 	ldrd	r3, r2, [r4, #60]	; 0x3c
 e0034f0:	fb02 f303 	mul.w	r3, r2, r3
 e0034f4:	1a9a      	subs	r2, r3, r2
 e0034f6:	440b      	add	r3, r1
 e0034f8:	4411      	add	r1, r2
 e0034fa:	60a3      	str	r3, [r4, #8]
 e0034fc:	23ff      	movs	r3, #255	; 0xff
 e0034fe:	60e1      	str	r1, [r4, #12]
 e003500:	f884 3044 	strb.w	r3, [r4, #68]	; 0x44
 e003504:	f884 3045 	strb.w	r3, [r4, #69]	; 0x45
 e003508:	f7fe feec 	bl	e0022e4 <vListInitialise>
 e00350c:	f104 0024 	add.w	r0, r4, #36	; 0x24
 e003510:	f7fe fee8 	bl	e0022e4 <vListInitialise>
 e003514:	f7ff fdb2 	bl	e00307c <vPortExitCritical>
 e003518:	f884 7050 	strb.w	r7, [r4, #80]	; 0x50
 e00351c:	64a5      	str	r5, [r4, #72]	; 0x48
 e00351e:	4620      	mov	r0, r4
 e003520:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e003522:	4603      	mov	r3, r0
 e003524:	e7d7      	b.n	e0034d6 <xQueueGenericCreate+0x22>
 e003526:	bf00      	nop

0e003528 <xQueueCreateCountingSemaphoreStatic>:
 e003528:	b570      	push	{r4, r5, r6, lr}
 e00352a:	4614      	mov	r4, r2
 e00352c:	b34a      	cbz	r2, e003582 <xQueueCreateCountingSemaphoreStatic+0x5a>
 e00352e:	2600      	movs	r6, #0
 e003530:	2301      	movs	r3, #1
 e003532:	6022      	str	r2, [r4, #0]
 e003534:	460d      	mov	r5, r1
 e003536:	63d0      	str	r0, [r2, #60]	; 0x3c
 e003538:	f882 3046 	strb.w	r3, [r2, #70]	; 0x46
 e00353c:	6416      	str	r6, [r2, #64]	; 0x40
 e00353e:	f7ff fd7f 	bl	e003040 <vPortEnterCritical>
 e003542:	6822      	ldr	r2, [r4, #0]
 e003544:	63a6      	str	r6, [r4, #56]	; 0x38
 e003546:	f104 0010 	add.w	r0, r4, #16
 e00354a:	6062      	str	r2, [r4, #4]
 e00354c:	e9d4 310f 	ldrd	r3, r1, [r4, #60]	; 0x3c
 e003550:	fb01 f303 	mul.w	r3, r1, r3
 e003554:	1a59      	subs	r1, r3, r1
 e003556:	4413      	add	r3, r2
 e003558:	440a      	add	r2, r1
 e00355a:	e9c4 3202 	strd	r3, r2, [r4, #8]
 e00355e:	23ff      	movs	r3, #255	; 0xff
 e003560:	f884 3044 	strb.w	r3, [r4, #68]	; 0x44
 e003564:	f884 3045 	strb.w	r3, [r4, #69]	; 0x45
 e003568:	f7fe febc 	bl	e0022e4 <vListInitialise>
 e00356c:	f104 0024 	add.w	r0, r4, #36	; 0x24
 e003570:	f7fe feb8 	bl	e0022e4 <vListInitialise>
 e003574:	f7ff fd82 	bl	e00307c <vPortExitCritical>
 e003578:	2302      	movs	r3, #2
 e00357a:	64a6      	str	r6, [r4, #72]	; 0x48
 e00357c:	63a5      	str	r5, [r4, #56]	; 0x38
 e00357e:	f884 3050 	strb.w	r3, [r4, #80]	; 0x50
 e003582:	4620      	mov	r0, r4
 e003584:	bd70      	pop	{r4, r5, r6, pc}
 e003586:	bf00      	nop

0e003588 <xQueueCreateCountingSemaphore>:
 e003588:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 e00358a:	4606      	mov	r6, r0
 e00358c:	2054      	movs	r0, #84	; 0x54
 e00358e:	460d      	mov	r5, r1
 e003590:	f7fe fdc0 	bl	e002114 <pvPortMalloc>
 e003594:	4604      	mov	r4, r0
 e003596:	b338      	cbz	r0, e0035e8 <xQueueCreateCountingSemaphore+0x60>
 e003598:	2700      	movs	r7, #0
 e00359a:	6020      	str	r0, [r4, #0]
 e00359c:	63c6      	str	r6, [r0, #60]	; 0x3c
 e00359e:	f880 7046 	strb.w	r7, [r0, #70]	; 0x46
 e0035a2:	6407      	str	r7, [r0, #64]	; 0x40
 e0035a4:	f7ff fd4c 	bl	e003040 <vPortEnterCritical>
 e0035a8:	6822      	ldr	r2, [r4, #0]
 e0035aa:	63a7      	str	r7, [r4, #56]	; 0x38
 e0035ac:	f104 0010 	add.w	r0, r4, #16
 e0035b0:	6062      	str	r2, [r4, #4]
 e0035b2:	e9d4 310f 	ldrd	r3, r1, [r4, #60]	; 0x3c
 e0035b6:	fb01 f303 	mul.w	r3, r1, r3
 e0035ba:	1a59      	subs	r1, r3, r1
 e0035bc:	4413      	add	r3, r2
 e0035be:	440a      	add	r2, r1
 e0035c0:	e9c4 3202 	strd	r3, r2, [r4, #8]
 e0035c4:	23ff      	movs	r3, #255	; 0xff
 e0035c6:	f884 3044 	strb.w	r3, [r4, #68]	; 0x44
 e0035ca:	f884 3045 	strb.w	r3, [r4, #69]	; 0x45
 e0035ce:	f7fe fe89 	bl	e0022e4 <vListInitialise>
 e0035d2:	f104 0024 	add.w	r0, r4, #36	; 0x24
 e0035d6:	f7fe fe85 	bl	e0022e4 <vListInitialise>
 e0035da:	f7ff fd4f 	bl	e00307c <vPortExitCritical>
 e0035de:	2302      	movs	r3, #2
 e0035e0:	64a7      	str	r7, [r4, #72]	; 0x48
 e0035e2:	63a5      	str	r5, [r4, #56]	; 0x38
 e0035e4:	f884 3050 	strb.w	r3, [r4, #80]	; 0x50
 e0035e8:	4620      	mov	r0, r4
 e0035ea:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

0e0035ec <xQueueGenericSend>:
 e0035ec:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
 e0035f0:	f1a3 0602 	sub.w	r6, r3, #2
 e0035f4:	b085      	sub	sp, #20
 e0035f6:	2500      	movs	r5, #0
 e0035f8:	4698      	mov	r8, r3
 e0035fa:	fab6 f686 	clz	r6, r6
 e0035fe:	4604      	mov	r4, r0
 e003600:	4689      	mov	r9, r1
 e003602:	462f      	mov	r7, r5
 e003604:	0976      	lsrs	r6, r6, #5
 e003606:	9201      	str	r2, [sp, #4]
 e003608:	e007      	b.n	e00361a <xQueueGenericSend+0x2e>
 e00360a:	f7ff fd37 	bl	e00307c <vPortExitCritical>
 e00360e:	4620      	mov	r0, r4
 e003610:	f7ff fed4 	bl	e0033bc <prvUnlockQueue>
 e003614:	f001 f882 	bl	e00471c <xTaskResumeAll>
 e003618:	2501      	movs	r5, #1
 e00361a:	f7ff fd11 	bl	e003040 <vPortEnterCritical>
 e00361e:	6ba2      	ldr	r2, [r4, #56]	; 0x38
 e003620:	6be3      	ldr	r3, [r4, #60]	; 0x3c
 e003622:	429a      	cmp	r2, r3
 e003624:	d339      	bcc.n	e00369a <xQueueGenericSend+0xae>
 e003626:	2e00      	cmp	r6, #0
 e003628:	d137      	bne.n	e00369a <xQueueGenericSend+0xae>
 e00362a:	9b01      	ldr	r3, [sp, #4]
 e00362c:	2b00      	cmp	r3, #0
 e00362e:	d044      	beq.n	e0036ba <xQueueGenericSend+0xce>
 e003630:	b37d      	cbz	r5, e003692 <xQueueGenericSend+0xa6>
 e003632:	f7ff fd23 	bl	e00307c <vPortExitCritical>
 e003636:	f001 f869 	bl	e00470c <vTaskSuspendAll>
 e00363a:	f7ff fd01 	bl	e003040 <vPortEnterCritical>
 e00363e:	f894 3044 	ldrb.w	r3, [r4, #68]	; 0x44
 e003642:	2bff      	cmp	r3, #255	; 0xff
 e003644:	bf08      	it	eq
 e003646:	f884 7044 	strbeq.w	r7, [r4, #68]	; 0x44
 e00364a:	f894 3045 	ldrb.w	r3, [r4, #69]	; 0x45
 e00364e:	2bff      	cmp	r3, #255	; 0xff
 e003650:	bf08      	it	eq
 e003652:	f884 7045 	strbeq.w	r7, [r4, #69]	; 0x45
 e003656:	f7ff fd11 	bl	e00307c <vPortExitCritical>
 e00365a:	a901      	add	r1, sp, #4
 e00365c:	a802      	add	r0, sp, #8
 e00365e:	f001 fa39 	bl	e004ad4 <xTaskCheckForTimeOut>
 e003662:	bb80      	cbnz	r0, e0036c6 <xQueueGenericSend+0xda>
 e003664:	f7ff fcec 	bl	e003040 <vPortEnterCritical>
 e003668:	6ba2      	ldr	r2, [r4, #56]	; 0x38
 e00366a:	6be3      	ldr	r3, [r4, #60]	; 0x3c
 e00366c:	429a      	cmp	r2, r3
 e00366e:	d1cc      	bne.n	e00360a <xQueueGenericSend+0x1e>
 e003670:	f7ff fd04 	bl	e00307c <vPortExitCritical>
 e003674:	9901      	ldr	r1, [sp, #4]
 e003676:	f104 0010 	add.w	r0, r4, #16
 e00367a:	f001 f9a7 	bl	e0049cc <vTaskPlaceOnEventList>
 e00367e:	4620      	mov	r0, r4
 e003680:	f7ff fe9c 	bl	e0033bc <prvUnlockQueue>
 e003684:	f001 f84a 	bl	e00471c <xTaskResumeAll>
 e003688:	2800      	cmp	r0, #0
 e00368a:	d1c5      	bne.n	e003618 <xQueueGenericSend+0x2c>
 e00368c:	f7ff fcc4 	bl	e003018 <vPortYield>
 e003690:	e7c2      	b.n	e003618 <xQueueGenericSend+0x2c>
 e003692:	a802      	add	r0, sp, #8
 e003694:	f001 fa12 	bl	e004abc <vTaskInternalSetTimeOutState>
 e003698:	e7cb      	b.n	e003632 <xQueueGenericSend+0x46>
 e00369a:	4642      	mov	r2, r8
 e00369c:	4649      	mov	r1, r9
 e00369e:	4620      	mov	r0, r4
 e0036a0:	6ba5      	ldr	r5, [r4, #56]	; 0x38
 e0036a2:	f7ff fe09 	bl	e0032b8 <prvCopyDataToQueue>
 e0036a6:	6ca3      	ldr	r3, [r4, #72]	; 0x48
 e0036a8:	b1b3      	cbz	r3, e0036d8 <xQueueGenericSend+0xec>
 e0036aa:	b31d      	cbz	r5, e0036f4 <xQueueGenericSend+0x108>
 e0036ac:	b316      	cbz	r6, e0036f4 <xQueueGenericSend+0x108>
 e0036ae:	f7ff fce5 	bl	e00307c <vPortExitCritical>
 e0036b2:	2001      	movs	r0, #1
 e0036b4:	b005      	add	sp, #20
 e0036b6:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 e0036ba:	f7ff fcdf 	bl	e00307c <vPortExitCritical>
 e0036be:	4630      	mov	r0, r6
 e0036c0:	b005      	add	sp, #20
 e0036c2:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 e0036c6:	4620      	mov	r0, r4
 e0036c8:	f7ff fe78 	bl	e0033bc <prvUnlockQueue>
 e0036cc:	f001 f826 	bl	e00471c <xTaskResumeAll>
 e0036d0:	2000      	movs	r0, #0
 e0036d2:	b005      	add	sp, #20
 e0036d4:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 e0036d8:	6a63      	ldr	r3, [r4, #36]	; 0x24
 e0036da:	b923      	cbnz	r3, e0036e6 <xQueueGenericSend+0xfa>
 e0036dc:	2800      	cmp	r0, #0
 e0036de:	d0e6      	beq.n	e0036ae <xQueueGenericSend+0xc2>
 e0036e0:	f7ff fc9a 	bl	e003018 <vPortYield>
 e0036e4:	e7e3      	b.n	e0036ae <xQueueGenericSend+0xc2>
 e0036e6:	f104 0024 	add.w	r0, r4, #36	; 0x24
 e0036ea:	f001 f995 	bl	e004a18 <xTaskRemoveFromEventList>
 e0036ee:	2800      	cmp	r0, #0
 e0036f0:	d0dd      	beq.n	e0036ae <xQueueGenericSend+0xc2>
 e0036f2:	e7f5      	b.n	e0036e0 <xQueueGenericSend+0xf4>
 e0036f4:	4620      	mov	r0, r4
 e0036f6:	f7ff fe1d 	bl	e003334 <prvNotifyQueueSetContainer>
 e0036fa:	e7ef      	b.n	e0036dc <xQueueGenericSend+0xf0>

0e0036fc <xQueueCreateMutexStatic>:
 e0036fc:	b570      	push	{r4, r5, r6, lr}
 e0036fe:	460c      	mov	r4, r1
 e003700:	b381      	cbz	r1, e003764 <xQueueCreateMutexStatic+0x68>
 e003702:	2600      	movs	r6, #0
 e003704:	2301      	movs	r3, #1
 e003706:	6021      	str	r1, [r4, #0]
 e003708:	4605      	mov	r5, r0
 e00370a:	f881 3046 	strb.w	r3, [r1, #70]	; 0x46
 e00370e:	e9c1 360f 	strd	r3, r6, [r1, #60]	; 0x3c
 e003712:	f7ff fc95 	bl	e003040 <vPortEnterCritical>
 e003716:	6821      	ldr	r1, [r4, #0]
 e003718:	23ff      	movs	r3, #255	; 0xff
 e00371a:	63a6      	str	r6, [r4, #56]	; 0x38
 e00371c:	6061      	str	r1, [r4, #4]
 e00371e:	f884 3044 	strb.w	r3, [r4, #68]	; 0x44
 e003722:	f884 3045 	strb.w	r3, [r4, #69]	; 0x45
 e003726:	e9d4 200f 	ldrd	r2, r0, [r4, #60]	; 0x3c
 e00372a:	fb00 f202 	mul.w	r2, r0, r2
 e00372e:	1a10      	subs	r0, r2, r0
 e003730:	440a      	add	r2, r1
 e003732:	4401      	add	r1, r0
 e003734:	f104 0010 	add.w	r0, r4, #16
 e003738:	e9c4 2102 	strd	r2, r1, [r4, #8]
 e00373c:	f7fe fdd2 	bl	e0022e4 <vListInitialise>
 e003740:	f104 0024 	add.w	r0, r4, #36	; 0x24
 e003744:	f7fe fdce 	bl	e0022e4 <vListInitialise>
 e003748:	f7ff fc98 	bl	e00307c <vPortExitCritical>
 e00374c:	4633      	mov	r3, r6
 e00374e:	4632      	mov	r2, r6
 e003750:	4631      	mov	r1, r6
 e003752:	4620      	mov	r0, r4
 e003754:	f884 5050 	strb.w	r5, [r4, #80]	; 0x50
 e003758:	64a6      	str	r6, [r4, #72]	; 0x48
 e00375a:	60a6      	str	r6, [r4, #8]
 e00375c:	6026      	str	r6, [r4, #0]
 e00375e:	60e6      	str	r6, [r4, #12]
 e003760:	f7ff ff44 	bl	e0035ec <xQueueGenericSend>
 e003764:	4620      	mov	r0, r4
 e003766:	bd70      	pop	{r4, r5, r6, pc}

0e003768 <xQueueCreateMutex>:
 e003768:	b570      	push	{r4, r5, r6, lr}
 e00376a:	4605      	mov	r5, r0
 e00376c:	2054      	movs	r0, #84	; 0x54
 e00376e:	f7fe fcd1 	bl	e002114 <pvPortMalloc>
 e003772:	4604      	mov	r4, r0
 e003774:	b378      	cbz	r0, e0037d6 <xQueueCreateMutex+0x6e>
 e003776:	2600      	movs	r6, #0
 e003778:	2301      	movs	r3, #1
 e00377a:	6020      	str	r0, [r4, #0]
 e00377c:	f880 6046 	strb.w	r6, [r0, #70]	; 0x46
 e003780:	e9c0 360f 	strd	r3, r6, [r0, #60]	; 0x3c
 e003784:	f7ff fc5c 	bl	e003040 <vPortEnterCritical>
 e003788:	6821      	ldr	r1, [r4, #0]
 e00378a:	63a6      	str	r6, [r4, #56]	; 0x38
 e00378c:	f104 0010 	add.w	r0, r4, #16
 e003790:	6061      	str	r1, [r4, #4]
 e003792:	e9d4 230f 	ldrd	r2, r3, [r4, #60]	; 0x3c
 e003796:	fb03 f202 	mul.w	r2, r3, r2
 e00379a:	1ad3      	subs	r3, r2, r3
 e00379c:	440a      	add	r2, r1
 e00379e:	4419      	add	r1, r3
 e0037a0:	23ff      	movs	r3, #255	; 0xff
 e0037a2:	e9c4 2102 	strd	r2, r1, [r4, #8]
 e0037a6:	f884 3044 	strb.w	r3, [r4, #68]	; 0x44
 e0037aa:	f884 3045 	strb.w	r3, [r4, #69]	; 0x45
 e0037ae:	f7fe fd99 	bl	e0022e4 <vListInitialise>
 e0037b2:	f104 0024 	add.w	r0, r4, #36	; 0x24
 e0037b6:	f7fe fd95 	bl	e0022e4 <vListInitialise>
 e0037ba:	f7ff fc5f 	bl	e00307c <vPortExitCritical>
 e0037be:	4633      	mov	r3, r6
 e0037c0:	4632      	mov	r2, r6
 e0037c2:	4631      	mov	r1, r6
 e0037c4:	4620      	mov	r0, r4
 e0037c6:	f884 5050 	strb.w	r5, [r4, #80]	; 0x50
 e0037ca:	64a6      	str	r6, [r4, #72]	; 0x48
 e0037cc:	60a6      	str	r6, [r4, #8]
 e0037ce:	6026      	str	r6, [r4, #0]
 e0037d0:	60e6      	str	r6, [r4, #12]
 e0037d2:	f7ff ff0b 	bl	e0035ec <xQueueGenericSend>
 e0037d6:	4620      	mov	r0, r4
 e0037d8:	bd70      	pop	{r4, r5, r6, pc}
 e0037da:	bf00      	nop

0e0037dc <xQueueGenericSendFromISR>:
 e0037dc:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
 e0037e0:	4604      	mov	r4, r0
 e0037e2:	461d      	mov	r5, r3
 e0037e4:	4690      	mov	r8, r2
 e0037e6:	4689      	mov	r9, r1
 e0037e8:	f1a5 0602 	sub.w	r6, r5, #2
 e0037ec:	f7ff fbe2 	bl	e002fb4 <ulSetInterruptMaskFromISR>
 e0037f0:	6ba2      	ldr	r2, [r4, #56]	; 0x38
 e0037f2:	4607      	mov	r7, r0
 e0037f4:	6be3      	ldr	r3, [r4, #60]	; 0x3c
 e0037f6:	fab6 f686 	clz	r6, r6
 e0037fa:	429a      	cmp	r2, r3
 e0037fc:	ea4f 1656 	mov.w	r6, r6, lsr #5
 e003800:	d306      	bcc.n	e003810 <xQueueGenericSendFromISR+0x34>
 e003802:	b92e      	cbnz	r6, e003810 <xQueueGenericSendFromISR+0x34>
 e003804:	4638      	mov	r0, r7
 e003806:	f7ff fbdb 	bl	e002fc0 <vClearInterruptMaskFromISR>
 e00380a:	4630      	mov	r0, r6
 e00380c:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
 e003810:	462a      	mov	r2, r5
 e003812:	f894 5045 	ldrb.w	r5, [r4, #69]	; 0x45
 e003816:	4649      	mov	r1, r9
 e003818:	4620      	mov	r0, r4
 e00381a:	b26d      	sxtb	r5, r5
 e00381c:	f8d4 9038 	ldr.w	r9, [r4, #56]	; 0x38
 e003820:	f7ff fd4a 	bl	e0032b8 <prvCopyDataToQueue>
 e003824:	1c6b      	adds	r3, r5, #1
 e003826:	d00a      	beq.n	e00383e <xQueueGenericSendFromISR+0x62>
 e003828:	3501      	adds	r5, #1
 e00382a:	2601      	movs	r6, #1
 e00382c:	4638      	mov	r0, r7
 e00382e:	b26d      	sxtb	r5, r5
 e003830:	f884 5045 	strb.w	r5, [r4, #69]	; 0x45
 e003834:	f7ff fbc4 	bl	e002fc0 <vClearInterruptMaskFromISR>
 e003838:	4630      	mov	r0, r6
 e00383a:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
 e00383e:	6ca3      	ldr	r3, [r4, #72]	; 0x48
 e003840:	b153      	cbz	r3, e003858 <xQueueGenericSendFromISR+0x7c>
 e003842:	f1b9 0f00 	cmp.w	r9, #0
 e003846:	d018      	beq.n	e00387a <xQueueGenericSendFromISR+0x9e>
 e003848:	b1be      	cbz	r6, e00387a <xQueueGenericSendFromISR+0x9e>
 e00384a:	2601      	movs	r6, #1
 e00384c:	4638      	mov	r0, r7
 e00384e:	f7ff fbb7 	bl	e002fc0 <vClearInterruptMaskFromISR>
 e003852:	4630      	mov	r0, r6
 e003854:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
 e003858:	6a63      	ldr	r3, [r4, #36]	; 0x24
 e00385a:	2b00      	cmp	r3, #0
 e00385c:	d0f5      	beq.n	e00384a <xQueueGenericSendFromISR+0x6e>
 e00385e:	f104 0024 	add.w	r0, r4, #36	; 0x24
 e003862:	f001 f8d9 	bl	e004a18 <xTaskRemoveFromEventList>
 e003866:	f1b8 0f00 	cmp.w	r8, #0
 e00386a:	d0ee      	beq.n	e00384a <xQueueGenericSendFromISR+0x6e>
 e00386c:	2800      	cmp	r0, #0
 e00386e:	d0ec      	beq.n	e00384a <xQueueGenericSendFromISR+0x6e>
 e003870:	2301      	movs	r3, #1
 e003872:	461e      	mov	r6, r3
 e003874:	f8c8 3000 	str.w	r3, [r8]
 e003878:	e7c4      	b.n	e003804 <xQueueGenericSendFromISR+0x28>
 e00387a:	4620      	mov	r0, r4
 e00387c:	f7ff fd5a 	bl	e003334 <prvNotifyQueueSetContainer>
 e003880:	f1b8 0f00 	cmp.w	r8, #0
 e003884:	d1f2      	bne.n	e00386c <xQueueGenericSendFromISR+0x90>
 e003886:	e7e0      	b.n	e00384a <xQueueGenericSendFromISR+0x6e>

0e003888 <xQueueGiveFromISR>:
 e003888:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 e00388a:	4604      	mov	r4, r0
 e00388c:	460e      	mov	r6, r1
 e00388e:	f7ff fb91 	bl	e002fb4 <ulSetInterruptMaskFromISR>
 e003892:	4605      	mov	r5, r0
 e003894:	6ba3      	ldr	r3, [r4, #56]	; 0x38
 e003896:	6be2      	ldr	r2, [r4, #60]	; 0x3c
 e003898:	429a      	cmp	r2, r3
 e00389a:	d910      	bls.n	e0038be <xQueueGiveFromISR+0x36>
 e00389c:	f894 2045 	ldrb.w	r2, [r4, #69]	; 0x45
 e0038a0:	3301      	adds	r3, #1
 e0038a2:	2aff      	cmp	r2, #255	; 0xff
 e0038a4:	63a3      	str	r3, [r4, #56]	; 0x38
 e0038a6:	b253      	sxtb	r3, r2
 e0038a8:	d00f      	beq.n	e0038ca <xQueueGiveFromISR+0x42>
 e0038aa:	3301      	adds	r3, #1
 e0038ac:	2701      	movs	r7, #1
 e0038ae:	4628      	mov	r0, r5
 e0038b0:	b25b      	sxtb	r3, r3
 e0038b2:	f884 3045 	strb.w	r3, [r4, #69]	; 0x45
 e0038b6:	f7ff fb83 	bl	e002fc0 <vClearInterruptMaskFromISR>
 e0038ba:	4638      	mov	r0, r7
 e0038bc:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e0038be:	2700      	movs	r7, #0
 e0038c0:	4628      	mov	r0, r5
 e0038c2:	f7ff fb7d 	bl	e002fc0 <vClearInterruptMaskFromISR>
 e0038c6:	4638      	mov	r0, r7
 e0038c8:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e0038ca:	6ca3      	ldr	r3, [r4, #72]	; 0x48
 e0038cc:	b143      	cbz	r3, e0038e0 <xQueueGiveFromISR+0x58>
 e0038ce:	4620      	mov	r0, r4
 e0038d0:	f7ff fd30 	bl	e003334 <prvNotifyQueueSetContainer>
 e0038d4:	b136      	cbz	r6, e0038e4 <xQueueGiveFromISR+0x5c>
 e0038d6:	b128      	cbz	r0, e0038e4 <xQueueGiveFromISR+0x5c>
 e0038d8:	2301      	movs	r3, #1
 e0038da:	461f      	mov	r7, r3
 e0038dc:	6033      	str	r3, [r6, #0]
 e0038de:	e7ef      	b.n	e0038c0 <xQueueGiveFromISR+0x38>
 e0038e0:	6a63      	ldr	r3, [r4, #36]	; 0x24
 e0038e2:	b92b      	cbnz	r3, e0038f0 <xQueueGiveFromISR+0x68>
 e0038e4:	2701      	movs	r7, #1
 e0038e6:	4628      	mov	r0, r5
 e0038e8:	f7ff fb6a 	bl	e002fc0 <vClearInterruptMaskFromISR>
 e0038ec:	4638      	mov	r0, r7
 e0038ee:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e0038f0:	f104 0024 	add.w	r0, r4, #36	; 0x24
 e0038f4:	f001 f890 	bl	e004a18 <xTaskRemoveFromEventList>
 e0038f8:	2e00      	cmp	r6, #0
 e0038fa:	d0f3      	beq.n	e0038e4 <xQueueGiveFromISR+0x5c>
 e0038fc:	2800      	cmp	r0, #0
 e0038fe:	d1eb      	bne.n	e0038d8 <xQueueGiveFromISR+0x50>
 e003900:	e7f0      	b.n	e0038e4 <xQueueGiveFromISR+0x5c>
 e003902:	bf00      	nop

0e003904 <xQueueReceive>:
 e003904:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 e003908:	4604      	mov	r4, r0
 e00390a:	b084      	sub	sp, #16
 e00390c:	460f      	mov	r7, r1
 e00390e:	9201      	str	r2, [sp, #4]
 e003910:	f7ff fb96 	bl	e003040 <vPortEnterCritical>
 e003914:	6ba5      	ldr	r5, [r4, #56]	; 0x38
 e003916:	2d00      	cmp	r5, #0
 e003918:	d155      	bne.n	e0039c6 <xQueueReceive+0xc2>
 e00391a:	9b01      	ldr	r3, [sp, #4]
 e00391c:	b3ab      	cbz	r3, e00398a <xQueueReceive+0x86>
 e00391e:	a802      	add	r0, sp, #8
 e003920:	462e      	mov	r6, r5
 e003922:	f104 0824 	add.w	r8, r4, #36	; 0x24
 e003926:	f001 f8c9 	bl	e004abc <vTaskInternalSetTimeOutState>
 e00392a:	f7ff fba7 	bl	e00307c <vPortExitCritical>
 e00392e:	f000 feed 	bl	e00470c <vTaskSuspendAll>
 e003932:	f7ff fb85 	bl	e003040 <vPortEnterCritical>
 e003936:	f894 3044 	ldrb.w	r3, [r4, #68]	; 0x44
 e00393a:	2bff      	cmp	r3, #255	; 0xff
 e00393c:	bf08      	it	eq
 e00393e:	f884 6044 	strbeq.w	r6, [r4, #68]	; 0x44
 e003942:	f894 3045 	ldrb.w	r3, [r4, #69]	; 0x45
 e003946:	2bff      	cmp	r3, #255	; 0xff
 e003948:	bf08      	it	eq
 e00394a:	f884 6045 	strbeq.w	r6, [r4, #69]	; 0x45
 e00394e:	f7ff fb95 	bl	e00307c <vPortExitCritical>
 e003952:	a901      	add	r1, sp, #4
 e003954:	a802      	add	r0, sp, #8
 e003956:	f001 f8bd 	bl	e004ad4 <xTaskCheckForTimeOut>
 e00395a:	b9e0      	cbnz	r0, e003996 <xQueueReceive+0x92>
 e00395c:	f7ff fb70 	bl	e003040 <vPortEnterCritical>
 e003960:	6ba3      	ldr	r3, [r4, #56]	; 0x38
 e003962:	bb2b      	cbnz	r3, e0039b0 <xQueueReceive+0xac>
 e003964:	f7ff fb8a 	bl	e00307c <vPortExitCritical>
 e003968:	9901      	ldr	r1, [sp, #4]
 e00396a:	4640      	mov	r0, r8
 e00396c:	f001 f82e 	bl	e0049cc <vTaskPlaceOnEventList>
 e003970:	4620      	mov	r0, r4
 e003972:	f7ff fd23 	bl	e0033bc <prvUnlockQueue>
 e003976:	f000 fed1 	bl	e00471c <xTaskResumeAll>
 e00397a:	b308      	cbz	r0, e0039c0 <xQueueReceive+0xbc>
 e00397c:	f7ff fb60 	bl	e003040 <vPortEnterCritical>
 e003980:	6ba5      	ldr	r5, [r4, #56]	; 0x38
 e003982:	bb05      	cbnz	r5, e0039c6 <xQueueReceive+0xc2>
 e003984:	9b01      	ldr	r3, [sp, #4]
 e003986:	2b00      	cmp	r3, #0
 e003988:	d1cf      	bne.n	e00392a <xQueueReceive+0x26>
 e00398a:	f7ff fb77 	bl	e00307c <vPortExitCritical>
 e00398e:	2000      	movs	r0, #0
 e003990:	b004      	add	sp, #16
 e003992:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e003996:	4620      	mov	r0, r4
 e003998:	f7ff fd10 	bl	e0033bc <prvUnlockQueue>
 e00399c:	f000 febe 	bl	e00471c <xTaskResumeAll>
 e0039a0:	f7ff fb4e 	bl	e003040 <vPortEnterCritical>
 e0039a4:	6ba3      	ldr	r3, [r4, #56]	; 0x38
 e0039a6:	2b00      	cmp	r3, #0
 e0039a8:	d0ef      	beq.n	e00398a <xQueueReceive+0x86>
 e0039aa:	f7ff fb67 	bl	e00307c <vPortExitCritical>
 e0039ae:	e7e5      	b.n	e00397c <xQueueReceive+0x78>
 e0039b0:	f7ff fb64 	bl	e00307c <vPortExitCritical>
 e0039b4:	4620      	mov	r0, r4
 e0039b6:	f7ff fd01 	bl	e0033bc <prvUnlockQueue>
 e0039ba:	f000 feaf 	bl	e00471c <xTaskResumeAll>
 e0039be:	e7dd      	b.n	e00397c <xQueueReceive+0x78>
 e0039c0:	f7ff fb2a 	bl	e003018 <vPortYield>
 e0039c4:	e7da      	b.n	e00397c <xQueueReceive+0x78>
 e0039c6:	6c22      	ldr	r2, [r4, #64]	; 0x40
 e0039c8:	b142      	cbz	r2, e0039dc <xQueueReceive+0xd8>
 e0039ca:	68e1      	ldr	r1, [r4, #12]
 e0039cc:	68a3      	ldr	r3, [r4, #8]
 e0039ce:	4411      	add	r1, r2
 e0039d0:	4299      	cmp	r1, r3
 e0039d2:	60e1      	str	r1, [r4, #12]
 e0039d4:	d20c      	bcs.n	e0039f0 <xQueueReceive+0xec>
 e0039d6:	4638      	mov	r0, r7
 e0039d8:	f004 f80e 	bl	e0079f8 <____wrap_memcpy_veneer>
 e0039dc:	3d01      	subs	r5, #1
 e0039de:	63a5      	str	r5, [r4, #56]	; 0x38
 e0039e0:	6923      	ldr	r3, [r4, #16]
 e0039e2:	b943      	cbnz	r3, e0039f6 <xQueueReceive+0xf2>
 e0039e4:	f7ff fb4a 	bl	e00307c <vPortExitCritical>
 e0039e8:	2001      	movs	r0, #1
 e0039ea:	b004      	add	sp, #16
 e0039ec:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e0039f0:	6821      	ldr	r1, [r4, #0]
 e0039f2:	60e1      	str	r1, [r4, #12]
 e0039f4:	e7ef      	b.n	e0039d6 <xQueueReceive+0xd2>
 e0039f6:	f104 0010 	add.w	r0, r4, #16
 e0039fa:	f001 f80d 	bl	e004a18 <xTaskRemoveFromEventList>
 e0039fe:	2800      	cmp	r0, #0
 e003a00:	d0f0      	beq.n	e0039e4 <xQueueReceive+0xe0>
 e003a02:	f7ff fb09 	bl	e003018 <vPortYield>
 e003a06:	e7ed      	b.n	e0039e4 <xQueueReceive+0xe0>

0e003a08 <xQueueSemaphoreTake>:
 e003a08:	b5f0      	push	{r4, r5, r6, r7, lr}
 e003a0a:	4604      	mov	r4, r0
 e003a0c:	b085      	sub	sp, #20
 e003a0e:	2600      	movs	r6, #0
 e003a10:	9101      	str	r1, [sp, #4]
 e003a12:	f7ff fb15 	bl	e003040 <vPortEnterCritical>
 e003a16:	6ba3      	ldr	r3, [r4, #56]	; 0x38
 e003a18:	4635      	mov	r5, r6
 e003a1a:	4637      	mov	r7, r6
 e003a1c:	2b00      	cmp	r3, #0
 e003a1e:	d139      	bne.n	e003a94 <xQueueSemaphoreTake+0x8c>
 e003a20:	9b01      	ldr	r3, [sp, #4]
 e003a22:	2b00      	cmp	r3, #0
 e003a24:	d07f      	beq.n	e003b26 <xQueueSemaphoreTake+0x11e>
 e003a26:	2d00      	cmp	r5, #0
 e003a28:	d04e      	beq.n	e003ac8 <xQueueSemaphoreTake+0xc0>
 e003a2a:	f7ff fb27 	bl	e00307c <vPortExitCritical>
 e003a2e:	f000 fe6d 	bl	e00470c <vTaskSuspendAll>
 e003a32:	f7ff fb05 	bl	e003040 <vPortEnterCritical>
 e003a36:	f894 3044 	ldrb.w	r3, [r4, #68]	; 0x44
 e003a3a:	2bff      	cmp	r3, #255	; 0xff
 e003a3c:	bf08      	it	eq
 e003a3e:	f884 7044 	strbeq.w	r7, [r4, #68]	; 0x44
 e003a42:	f894 3045 	ldrb.w	r3, [r4, #69]	; 0x45
 e003a46:	2bff      	cmp	r3, #255	; 0xff
 e003a48:	bf08      	it	eq
 e003a4a:	f884 7045 	strbeq.w	r7, [r4, #69]	; 0x45
 e003a4e:	f7ff fb15 	bl	e00307c <vPortExitCritical>
 e003a52:	a901      	add	r1, sp, #4
 e003a54:	a802      	add	r0, sp, #8
 e003a56:	f001 f83d 	bl	e004ad4 <xTaskCheckForTimeOut>
 e003a5a:	bb48      	cbnz	r0, e003ab0 <xQueueSemaphoreTake+0xa8>
 e003a5c:	f7ff faf0 	bl	e003040 <vPortEnterCritical>
 e003a60:	6ba3      	ldr	r3, [r4, #56]	; 0x38
 e003a62:	2b00      	cmp	r3, #0
 e003a64:	d134      	bne.n	e003ad0 <xQueueSemaphoreTake+0xc8>
 e003a66:	f7ff fb09 	bl	e00307c <vPortExitCritical>
 e003a6a:	6823      	ldr	r3, [r4, #0]
 e003a6c:	2b00      	cmp	r3, #0
 e003a6e:	d051      	beq.n	e003b14 <xQueueSemaphoreTake+0x10c>
 e003a70:	9901      	ldr	r1, [sp, #4]
 e003a72:	f104 0024 	add.w	r0, r4, #36	; 0x24
 e003a76:	f000 ffa9 	bl	e0049cc <vTaskPlaceOnEventList>
 e003a7a:	4620      	mov	r0, r4
 e003a7c:	f7ff fc9e 	bl	e0033bc <prvUnlockQueue>
 e003a80:	f000 fe4c 	bl	e00471c <xTaskResumeAll>
 e003a84:	2800      	cmp	r0, #0
 e003a86:	d042      	beq.n	e003b0e <xQueueSemaphoreTake+0x106>
 e003a88:	f7ff fada 	bl	e003040 <vPortEnterCritical>
 e003a8c:	6ba3      	ldr	r3, [r4, #56]	; 0x38
 e003a8e:	2501      	movs	r5, #1
 e003a90:	2b00      	cmp	r3, #0
 e003a92:	d0c5      	beq.n	e003a20 <xQueueSemaphoreTake+0x18>
 e003a94:	3b01      	subs	r3, #1
 e003a96:	6822      	ldr	r2, [r4, #0]
 e003a98:	63a3      	str	r3, [r4, #56]	; 0x38
 e003a9a:	2a00      	cmp	r2, #0
 e003a9c:	d052      	beq.n	e003b44 <xQueueSemaphoreTake+0x13c>
 e003a9e:	6923      	ldr	r3, [r4, #16]
 e003aa0:	2b00      	cmp	r3, #0
 e003aa2:	d146      	bne.n	e003b32 <xQueueSemaphoreTake+0x12a>
 e003aa4:	2601      	movs	r6, #1
 e003aa6:	f7ff fae9 	bl	e00307c <vPortExitCritical>
 e003aaa:	4630      	mov	r0, r6
 e003aac:	b005      	add	sp, #20
 e003aae:	bdf0      	pop	{r4, r5, r6, r7, pc}
 e003ab0:	4620      	mov	r0, r4
 e003ab2:	f7ff fc83 	bl	e0033bc <prvUnlockQueue>
 e003ab6:	f000 fe31 	bl	e00471c <xTaskResumeAll>
 e003aba:	f7ff fac1 	bl	e003040 <vPortEnterCritical>
 e003abe:	6ba3      	ldr	r3, [r4, #56]	; 0x38
 e003ac0:	b173      	cbz	r3, e003ae0 <xQueueSemaphoreTake+0xd8>
 e003ac2:	f7ff fadb 	bl	e00307c <vPortExitCritical>
 e003ac6:	e7df      	b.n	e003a88 <xQueueSemaphoreTake+0x80>
 e003ac8:	a802      	add	r0, sp, #8
 e003aca:	f000 fff7 	bl	e004abc <vTaskInternalSetTimeOutState>
 e003ace:	e7ac      	b.n	e003a2a <xQueueSemaphoreTake+0x22>
 e003ad0:	f7ff fad4 	bl	e00307c <vPortExitCritical>
 e003ad4:	4620      	mov	r0, r4
 e003ad6:	f7ff fc71 	bl	e0033bc <prvUnlockQueue>
 e003ada:	f000 fe1f 	bl	e00471c <xTaskResumeAll>
 e003ade:	e7d3      	b.n	e003a88 <xQueueSemaphoreTake+0x80>
 e003ae0:	f7ff facc 	bl	e00307c <vPortExitCritical>
 e003ae4:	b916      	cbnz	r6, e003aec <xQueueSemaphoreTake+0xe4>
 e003ae6:	4630      	mov	r0, r6
 e003ae8:	b005      	add	sp, #20
 e003aea:	bdf0      	pop	{r4, r5, r6, r7, pc}
 e003aec:	f7ff faa8 	bl	e003040 <vPortEnterCritical>
 e003af0:	6a61      	ldr	r1, [r4, #36]	; 0x24
 e003af2:	b119      	cbz	r1, e003afc <xQueueSemaphoreTake+0xf4>
 e003af4:	6b23      	ldr	r3, [r4, #48]	; 0x30
 e003af6:	6819      	ldr	r1, [r3, #0]
 e003af8:	f1c1 010b 	rsb	r1, r1, #11
 e003afc:	2600      	movs	r6, #0
 e003afe:	68a0      	ldr	r0, [r4, #8]
 e003b00:	f001 f918 	bl	e004d34 <vTaskPriorityDisinheritAfterTimeout>
 e003b04:	f7ff faba 	bl	e00307c <vPortExitCritical>
 e003b08:	4630      	mov	r0, r6
 e003b0a:	b005      	add	sp, #20
 e003b0c:	bdf0      	pop	{r4, r5, r6, r7, pc}
 e003b0e:	f7ff fa83 	bl	e003018 <vPortYield>
 e003b12:	e7b9      	b.n	e003a88 <xQueueSemaphoreTake+0x80>
 e003b14:	f7ff fa94 	bl	e003040 <vPortEnterCritical>
 e003b18:	68a0      	ldr	r0, [r4, #8]
 e003b1a:	f001 f895 	bl	e004c48 <xTaskPriorityInherit>
 e003b1e:	4606      	mov	r6, r0
 e003b20:	f7ff faac 	bl	e00307c <vPortExitCritical>
 e003b24:	e7a4      	b.n	e003a70 <xQueueSemaphoreTake+0x68>
 e003b26:	461e      	mov	r6, r3
 e003b28:	f7ff faa8 	bl	e00307c <vPortExitCritical>
 e003b2c:	4630      	mov	r0, r6
 e003b2e:	b005      	add	sp, #20
 e003b30:	bdf0      	pop	{r4, r5, r6, r7, pc}
 e003b32:	f104 0010 	add.w	r0, r4, #16
 e003b36:	f000 ff6f 	bl	e004a18 <xTaskRemoveFromEventList>
 e003b3a:	2800      	cmp	r0, #0
 e003b3c:	d0b2      	beq.n	e003aa4 <xQueueSemaphoreTake+0x9c>
 e003b3e:	f7ff fa6b 	bl	e003018 <vPortYield>
 e003b42:	e7af      	b.n	e003aa4 <xQueueSemaphoreTake+0x9c>
 e003b44:	f001 f930 	bl	e004da8 <pvTaskIncrementMutexHeldCount>
 e003b48:	60a0      	str	r0, [r4, #8]
 e003b4a:	e7a8      	b.n	e003a9e <xQueueSemaphoreTake+0x96>

0e003b4c <xQueueReceiveFromISR>:
 e003b4c:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
 e003b50:	4604      	mov	r4, r0
 e003b52:	4689      	mov	r9, r1
 e003b54:	4690      	mov	r8, r2
 e003b56:	f7ff fa2d 	bl	e002fb4 <ulSetInterruptMaskFromISR>
 e003b5a:	6ba5      	ldr	r5, [r4, #56]	; 0x38
 e003b5c:	4606      	mov	r6, r0
 e003b5e:	b92d      	cbnz	r5, e003b6c <xQueueReceiveFromISR+0x20>
 e003b60:	4630      	mov	r0, r6
 e003b62:	f7ff fa2d 	bl	e002fc0 <vClearInterruptMaskFromISR>
 e003b66:	4628      	mov	r0, r5
 e003b68:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
 e003b6c:	f894 7044 	ldrb.w	r7, [r4, #68]	; 0x44
 e003b70:	6c22      	ldr	r2, [r4, #64]	; 0x40
 e003b72:	b27f      	sxtb	r7, r7
 e003b74:	b152      	cbz	r2, e003b8c <xQueueReceiveFromISR+0x40>
 e003b76:	68e1      	ldr	r1, [r4, #12]
 e003b78:	68a3      	ldr	r3, [r4, #8]
 e003b7a:	4411      	add	r1, r2
 e003b7c:	4299      	cmp	r1, r3
 e003b7e:	60e1      	str	r1, [r4, #12]
 e003b80:	d301      	bcc.n	e003b86 <xQueueReceiveFromISR+0x3a>
 e003b82:	6821      	ldr	r1, [r4, #0]
 e003b84:	60e1      	str	r1, [r4, #12]
 e003b86:	4648      	mov	r0, r9
 e003b88:	f003 ff36 	bl	e0079f8 <____wrap_memcpy_veneer>
 e003b8c:	3d01      	subs	r5, #1
 e003b8e:	1c7b      	adds	r3, r7, #1
 e003b90:	63a5      	str	r5, [r4, #56]	; 0x38
 e003b92:	d00a      	beq.n	e003baa <xQueueReceiveFromISR+0x5e>
 e003b94:	3701      	adds	r7, #1
 e003b96:	2501      	movs	r5, #1
 e003b98:	4630      	mov	r0, r6
 e003b9a:	b27f      	sxtb	r7, r7
 e003b9c:	f884 7044 	strb.w	r7, [r4, #68]	; 0x44
 e003ba0:	f7ff fa0e 	bl	e002fc0 <vClearInterruptMaskFromISR>
 e003ba4:	4628      	mov	r0, r5
 e003ba6:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
 e003baa:	6923      	ldr	r3, [r4, #16]
 e003bac:	b90b      	cbnz	r3, e003bb2 <xQueueReceiveFromISR+0x66>
 e003bae:	2501      	movs	r5, #1
 e003bb0:	e7d6      	b.n	e003b60 <xQueueReceiveFromISR+0x14>
 e003bb2:	f104 0010 	add.w	r0, r4, #16
 e003bb6:	f000 ff2f 	bl	e004a18 <xTaskRemoveFromEventList>
 e003bba:	f1b8 0f00 	cmp.w	r8, #0
 e003bbe:	d0f6      	beq.n	e003bae <xQueueReceiveFromISR+0x62>
 e003bc0:	2800      	cmp	r0, #0
 e003bc2:	d0f4      	beq.n	e003bae <xQueueReceiveFromISR+0x62>
 e003bc4:	2301      	movs	r3, #1
 e003bc6:	461d      	mov	r5, r3
 e003bc8:	f8c8 3000 	str.w	r3, [r8]
 e003bcc:	e7c8      	b.n	e003b60 <xQueueReceiveFromISR+0x14>
 e003bce:	bf00      	nop

0e003bd0 <vQueueWaitForMessageRestricted>:
 e003bd0:	b570      	push	{r4, r5, r6, lr}
 e003bd2:	4604      	mov	r4, r0
 e003bd4:	460e      	mov	r6, r1
 e003bd6:	4615      	mov	r5, r2
 e003bd8:	f7ff fa32 	bl	e003040 <vPortEnterCritical>
 e003bdc:	f894 3044 	ldrb.w	r3, [r4, #68]	; 0x44
 e003be0:	2bff      	cmp	r3, #255	; 0xff
 e003be2:	d102      	bne.n	e003bea <vQueueWaitForMessageRestricted+0x1a>
 e003be4:	2300      	movs	r3, #0
 e003be6:	f884 3044 	strb.w	r3, [r4, #68]	; 0x44
 e003bea:	f894 3045 	ldrb.w	r3, [r4, #69]	; 0x45
 e003bee:	2bff      	cmp	r3, #255	; 0xff
 e003bf0:	d102      	bne.n	e003bf8 <vQueueWaitForMessageRestricted+0x28>
 e003bf2:	2300      	movs	r3, #0
 e003bf4:	f884 3045 	strb.w	r3, [r4, #69]	; 0x45
 e003bf8:	f7ff fa40 	bl	e00307c <vPortExitCritical>
 e003bfc:	6ba3      	ldr	r3, [r4, #56]	; 0x38
 e003bfe:	b123      	cbz	r3, e003c0a <vQueueWaitForMessageRestricted+0x3a>
 e003c00:	4620      	mov	r0, r4
 e003c02:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
 e003c06:	f7ff bbd9 	b.w	e0033bc <prvUnlockQueue>
 e003c0a:	f104 0024 	add.w	r0, r4, #36	; 0x24
 e003c0e:	462a      	mov	r2, r5
 e003c10:	4631      	mov	r1, r6
 e003c12:	f000 feeb 	bl	e0049ec <vTaskPlaceOnEventListRestricted>
 e003c16:	4620      	mov	r0, r4
 e003c18:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
 e003c1c:	f7ff bbce 	b.w	e0033bc <prvUnlockQueue>

0e003c20 <shell_loguratRx_ipc_int>:
 e003c20:	b570      	push	{r4, r5, r6, lr}
 e003c22:	2104      	movs	r1, #4
 e003c24:	2000      	movs	r0, #0
 e003c26:	f7fd ff21 	bl	e001a6c <ipc_get_message>
 e003c2a:	4e1d      	ldr	r6, [pc, #116]	; (e003ca0 <shell_loguratRx_ipc_int+0x80>)
 e003c2c:	4b1d      	ldr	r3, [pc, #116]	; (e003ca4 <shell_loguratRx_ipc_int+0x84>)
 e003c2e:	6875      	ldr	r5, [r6, #4]
 e003c30:	695b      	ldr	r3, [r3, #20]
 e003c32:	6841      	ldr	r1, [r0, #4]
 e003c34:	03db      	lsls	r3, r3, #15
 e003c36:	d512      	bpl.n	e003c5e <shell_loguratRx_ipc_int+0x3e>
 e003c38:	f011 001f 	ands.w	r0, r1, #31
 e003c3c:	d122      	bne.n	e003c84 <shell_loguratRx_ipc_int+0x64>
 e003c3e:	460b      	mov	r3, r1
 e003c40:	2080      	movs	r0, #128	; 0x80
 e003c42:	f3bf 8f4f 	dsb	sy
 e003c46:	4c17      	ldr	r4, [pc, #92]	; (e003ca4 <shell_loguratRx_ipc_int+0x84>)
 e003c48:	4418      	add	r0, r3
 e003c4a:	f8c4 325c 	str.w	r3, [r4, #604]	; 0x25c
 e003c4e:	3320      	adds	r3, #32
 e003c50:	1ac2      	subs	r2, r0, r3
 e003c52:	2a00      	cmp	r2, #0
 e003c54:	dcf9      	bgt.n	e003c4a <shell_loguratRx_ipc_int+0x2a>
 e003c56:	f3bf 8f4f 	dsb	sy
 e003c5a:	f3bf 8f6f 	isb	sy
 e003c5e:	2280      	movs	r2, #128	; 0x80
 e003c60:	4b11      	ldr	r3, [pc, #68]	; (e003ca8 <shell_loguratRx_ipc_int+0x88>)
 e003c62:	4628      	mov	r0, r5
 e003c64:	4798      	blx	r3
 e003c66:	4911      	ldr	r1, [pc, #68]	; (e003cac <shell_loguratRx_ipc_int+0x8c>)
 e003c68:	1c68      	adds	r0, r5, #1
 e003c6a:	4b11      	ldr	r3, [pc, #68]	; (e003cb0 <shell_loguratRx_ipc_int+0x90>)
 e003c6c:	4798      	blx	r3
 e003c6e:	4604      	mov	r4, r0
 e003c70:	b160      	cbz	r0, e003c8c <shell_loguratRx_ipc_int+0x6c>
 e003c72:	2301      	movs	r3, #1
 e003c74:	7073      	strb	r3, [r6, #1]
 e003c76:	78b3      	ldrb	r3, [r6, #2]
 e003c78:	b11b      	cbz	r3, e003c82 <shell_loguratRx_ipc_int+0x62>
 e003c7a:	6973      	ldr	r3, [r6, #20]
 e003c7c:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
 e003c80:	4718      	bx	r3
 e003c82:	bd70      	pop	{r4, r5, r6, pc}
 e003c84:	f021 031f 	bic.w	r3, r1, #31
 e003c88:	3080      	adds	r0, #128	; 0x80
 e003c8a:	e7da      	b.n	e003c42 <shell_loguratRx_ipc_int+0x22>
 e003c8c:	4b09      	ldr	r3, [pc, #36]	; (e003cb4 <shell_loguratRx_ipc_int+0x94>)
 e003c8e:	4798      	blx	r3
 e003c90:	4622      	mov	r2, r4
 e003c92:	4628      	mov	r0, r5
 e003c94:	2180      	movs	r1, #128	; 0x80
 e003c96:	4b08      	ldr	r3, [pc, #32]	; (e003cb8 <shell_loguratRx_ipc_int+0x98>)
 e003c98:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
 e003c9c:	4718      	bx	r3
 e003c9e:	bf00      	nop
 e003ca0:	2000412c 	.word	0x2000412c
 e003ca4:	e000ed00 	.word	0xe000ed00
 e003ca8:	00011c1d 	.word	0x00011c1d
 e003cac:	0e0086e4 	.word	0x0e0086e4
 e003cb0:	00011f15 	.word	0x00011f15
 e003cb4:	0e006875 	.word	0x0e006875
 e003cb8:	0000e305 	.word	0x0000e305

0e003cbc <shell_task_ram>:
 e003cbc:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e003cc0:	4c28      	ldr	r4, [pc, #160]	; (e003d64 <shell_task_ram+0xa8>)
 e003cc2:	2301      	movs	r3, #1
 e003cc4:	4f28      	ldr	r7, [pc, #160]	; (e003d68 <shell_task_ram+0xac>)
 e003cc6:	f44f 6080 	mov.w	r0, #1024	; 0x400
 e003cca:	6862      	ldr	r2, [r4, #4]
 e003ccc:	f8df 80ac 	ldr.w	r8, [pc, #172]	; e003d7c <shell_task_ram+0xc0>
 e003cd0:	4615      	mov	r5, r2
 e003cd2:	70a3      	strb	r3, [r4, #2]
 e003cd4:	f105 0b01 	add.w	fp, r5, #1
 e003cd8:	46da      	mov	sl, fp
 e003cda:	ed2d 8b02 	vpush	{d8}
 e003cde:	b083      	sub	sp, #12
 e003ce0:	ee08 2a10 	vmov	s16, r2
 e003ce4:	f7ff f8d2 	bl	e002e8c <rtos_create_secure_context>
 e003ce8:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
 e003cec:	6838      	ldr	r0, [r7, #0]
 e003cee:	f7fe ff5d 	bl	e002bac <rtos_sema_take>
 e003cf2:	7863      	ldrb	r3, [r4, #1]
 e003cf4:	2b00      	cmp	r3, #0
 e003cf6:	d0f7      	beq.n	e003ce8 <shell_task_ram+0x2c>
 e003cf8:	4651      	mov	r1, sl
 e003cfa:	481c      	ldr	r0, [pc, #112]	; (e003d6c <shell_task_ram+0xb0>)
 e003cfc:	f003 fdc4 	bl	e007888 <strcpy>
 e003d00:	4b1b      	ldr	r3, [pc, #108]	; (e003d70 <shell_task_ram+0xb4>)
 e003d02:	4650      	mov	r0, sl
 e003d04:	4798      	blx	r3
 e003d06:	4603      	mov	r3, r0
 e003d08:	4650      	mov	r0, sl
 e003d0a:	9300      	str	r3, [sp, #0]
 e003d0c:	4b19      	ldr	r3, [pc, #100]	; (e003d74 <shell_task_ram+0xb8>)
 e003d0e:	4798      	blx	r3
 e003d10:	68e5      	ldr	r5, [r4, #12]
 e003d12:	6926      	ldr	r6, [r4, #16]
 e003d14:	f8d0 9000 	ldr.w	r9, [r0]
 e003d18:	9001      	str	r0, [sp, #4]
 e003d1a:	b1fe      	cbz	r6, e003d5c <shell_task_ram+0xa0>
 e003d1c:	f04f 0b00 	mov.w	fp, #0
 e003d20:	e003      	b.n	e003d2a <shell_task_ram+0x6e>
 e003d22:	455e      	cmp	r6, fp
 e003d24:	f105 0510 	add.w	r5, r5, #16
 e003d28:	d018      	beq.n	e003d5c <shell_task_ram+0xa0>
 e003d2a:	6829      	ldr	r1, [r5, #0]
 e003d2c:	4648      	mov	r0, r9
 e003d2e:	f10b 0b01 	add.w	fp, fp, #1
 e003d32:	47c0      	blx	r8
 e003d34:	2800      	cmp	r0, #0
 e003d36:	d1f4      	bne.n	e003d22 <shell_task_ram+0x66>
 e003d38:	68ab      	ldr	r3, [r5, #8]
 e003d3a:	b17b      	cbz	r3, e003d5c <shell_task_ram+0xa0>
 e003d3c:	9a00      	ldr	r2, [sp, #0]
 e003d3e:	9901      	ldr	r1, [sp, #4]
 e003d40:	1e50      	subs	r0, r2, #1
 e003d42:	3104      	adds	r1, #4
 e003d44:	b280      	uxth	r0, r0
 e003d46:	4798      	blx	r3
 e003d48:	4b0b      	ldr	r3, [pc, #44]	; (e003d78 <shell_task_ram+0xbc>)
 e003d4a:	2200      	movs	r2, #0
 e003d4c:	2180      	movs	r1, #128	; 0x80
 e003d4e:	ee18 0a10 	vmov	r0, s16
 e003d52:	4798      	blx	r3
 e003d54:	f04f 0300 	mov.w	r3, #0
 e003d58:	7063      	strb	r3, [r4, #1]
 e003d5a:	e7c5      	b.n	e003ce8 <shell_task_ram+0x2c>
 e003d5c:	4803      	ldr	r0, [pc, #12]	; (e003d6c <shell_task_ram+0xb0>)
 e003d5e:	f7fe fc9b 	bl	e002698 <log_service>
 e003d62:	e7f1      	b.n	e003d48 <shell_task_ram+0x8c>
 e003d64:	2000412c 	.word	0x2000412c
 e003d68:	2000b968 	.word	0x2000b968
 e003d6c:	2000b8e8 	.word	0x2000b8e8
 e003d70:	0000e313 	.word	0x0000e313
 e003d74:	0000e33f 	.word	0x0000e33f
 e003d78:	0000e305 	.word	0x0000e305
 e003d7c:	00011f15 	.word	0x00011f15

0e003d80 <shell_give_sema>:
 e003d80:	4b03      	ldr	r3, [pc, #12]	; (e003d90 <shell_give_sema+0x10>)
 e003d82:	789b      	ldrb	r3, [r3, #2]
 e003d84:	b903      	cbnz	r3, e003d88 <shell_give_sema+0x8>
 e003d86:	4770      	bx	lr
 e003d88:	4b02      	ldr	r3, [pc, #8]	; (e003d94 <shell_give_sema+0x14>)
 e003d8a:	6818      	ldr	r0, [r3, #0]
 e003d8c:	f7fe bf36 	b.w	e002bfc <rtos_sema_give>
 e003d90:	2000412c 	.word	0x2000412c
 e003d94:	2000b968 	.word	0x2000b968

0e003d98 <shell_init_ram>:
 e003d98:	b530      	push	{r4, r5, lr}
 e003d9a:	b083      	sub	sp, #12
 e003d9c:	2400      	movs	r4, #0
 e003d9e:	f7fe fb95 	bl	e0024cc <log_service_init>
 e003da2:	4b13      	ldr	r3, [pc, #76]	; (e003df0 <shell_init_ram+0x58>)
 e003da4:	4913      	ldr	r1, [pc, #76]	; (e003df4 <shell_init_ram+0x5c>)
 e003da6:	2501      	movs	r5, #1
 e003da8:	4a13      	ldr	r2, [pc, #76]	; (e003df8 <shell_init_ram+0x60>)
 e003daa:	1ac9      	subs	r1, r1, r3
 e003dac:	4813      	ldr	r0, [pc, #76]	; (e003dfc <shell_init_ram+0x64>)
 e003dae:	60d3      	str	r3, [r2, #12]
 e003db0:	0909      	lsrs	r1, r1, #4
 e003db2:	4b13      	ldr	r3, [pc, #76]	; (e003e00 <shell_init_ram+0x68>)
 e003db4:	6111      	str	r1, [r2, #16]
 e003db6:	7054      	strb	r4, [r2, #1]
 e003db8:	7015      	strb	r5, [r2, #0]
 e003dba:	2505      	movs	r5, #5
 e003dbc:	6153      	str	r3, [r2, #20]
 e003dbe:	f7fe fee3 	bl	e002b88 <rtos_sema_create_binary>
 e003dc2:	f44f 5180 	mov.w	r1, #4096	; 0x1000
 e003dc6:	4623      	mov	r3, r4
 e003dc8:	4a0e      	ldr	r2, [pc, #56]	; (e003e04 <shell_init_ram+0x6c>)
 e003dca:	4620      	mov	r0, r4
 e003dcc:	e9cd 1500 	strd	r1, r5, [sp]
 e003dd0:	490d      	ldr	r1, [pc, #52]	; (e003e08 <shell_init_ram+0x70>)
 e003dd2:	f7ff f841 	bl	e002e58 <rtos_task_create>
 e003dd6:	b908      	cbnz	r0, e003ddc <shell_init_ram+0x44>
 e003dd8:	b003      	add	sp, #12
 e003dda:	bd30      	pop	{r4, r5, pc}
 e003ddc:	4b0b      	ldr	r3, [pc, #44]	; (e003e0c <shell_init_ram+0x74>)
 e003dde:	2245      	movs	r2, #69	; 0x45
 e003de0:	490b      	ldr	r1, [pc, #44]	; (e003e10 <shell_init_ram+0x78>)
 e003de2:	2002      	movs	r0, #2
 e003de4:	b003      	add	sp, #12
 e003de6:	e8bd 4030 	ldmia.w	sp!, {r4, r5, lr}
 e003dea:	f7fe bb27 	b.w	e00243c <rtk_log_write>
 e003dee:	bf00      	nop
 e003df0:	0e009950 	.word	0x0e009950
 e003df4:	0e009990 	.word	0x0e009990
 e003df8:	2000412c 	.word	0x2000412c
 e003dfc:	2000b968 	.word	0x2000b968
 e003e00:	0e003d81 	.word	0x0e003d81
 e003e04:	0e003cbd 	.word	0x0e003cbd
 e003e08:	0e0086ec 	.word	0x0e0086ec
 e003e0c:	0e0086fc 	.word	0x0e0086fc
 e003e10:	0e008718 	.word	0x0e008718

0e003e14 <sys_clear_ota_signature>:
 e003e14:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 e003e18:	2400      	movs	r4, #0
 e003e1a:	b0a4      	sub	sp, #144	; 0x90
 e003e1c:	2002      	movs	r0, #2
 e003e1e:	a906      	add	r1, sp, #24
 e003e20:	4622      	mov	r2, r4
 e003e22:	e9cd 440c 	strd	r4, r4, [sp, #48]	; 0x30
 e003e26:	f7fd fb79 	bl	e00151c <flash_get_layout_info>
 e003e2a:	4622      	mov	r2, r4
 e003e2c:	a907      	add	r1, sp, #28
 e003e2e:	2003      	movs	r0, #3
 e003e30:	f7fd fb74 	bl	e00151c <flash_get_layout_info>
 e003e34:	2001      	movs	r0, #1
 e003e36:	f7fd ff67 	bl	e001d08 <ota_get_cur_index>
 e003e3a:	b2c0      	uxtb	r0, r0
 e003e3c:	f080 0201 	eor.w	r2, r0, #1
 e003e40:	2800      	cmp	r0, #0
 e003e42:	d144      	bne.n	e003ece <sys_clear_ota_signature+0xba>
 e003e44:	4603      	mov	r3, r0
 e003e46:	e9dd 1506 	ldrd	r1, r5, [sp, #24]
 e003e4a:	f101 4178 	add.w	r1, r1, #4160749568	; 0xf8000000
 e003e4e:	f105 4578 	add.w	r5, r5, #4160749568	; 0xf8000000
 e003e52:	910a      	str	r1, [sp, #40]	; 0x28
 e003e54:	a924      	add	r1, sp, #144	; 0x90
 e003e56:	3390      	adds	r3, #144	; 0x90
 e003e58:	1c44      	adds	r4, r0, #1
 e003e5a:	1c56      	adds	r6, r2, #1
 e003e5c:	eb01 0182 	add.w	r1, r1, r2, lsl #2
 e003e60:	446b      	add	r3, sp
 e003e62:	2001      	movs	r0, #1
 e003e64:	4f26      	ldr	r7, [pc, #152]	; (e003f00 <sys_clear_ota_signature+0xec>)
 e003e66:	f841 5c68 	str.w	r5, [r1, #-104]
 e003e6a:	2241      	movs	r2, #65	; 0x41
 e003e6c:	f853 8c68 	ldr.w	r8, [r3, #-104]
 e003e70:	4924      	ldr	r1, [pc, #144]	; (e003f04 <sys_clear_ota_signature+0xf0>)
 e003e72:	4b25      	ldr	r3, [pc, #148]	; (e003f08 <sys_clear_ota_signature+0xf4>)
 e003e74:	9001      	str	r0, [sp, #4]
 e003e76:	9700      	str	r7, [sp, #0]
 e003e78:	e9cd 6504 	strd	r6, r5, [sp, #16]
 e003e7c:	e9cd 4802 	strd	r4, r8, [sp, #8]
 e003e80:	f7fe fadc 	bl	e00243c <rtk_log_write>
 e003e84:	aa08      	add	r2, sp, #32
 e003e86:	4629      	mov	r1, r5
 e003e88:	a80e      	add	r0, sp, #56	; 0x38
 e003e8a:	f7fe f88d 	bl	e001fa8 <flash_read_word>
 e003e8e:	aa09      	add	r2, sp, #36	; 0x24
 e003e90:	1d29      	adds	r1, r5, #4
 e003e92:	a80e      	add	r0, sp, #56	; 0x38
 e003e94:	f7fe f888 	bl	e001fa8 <flash_read_word>
 e003e98:	4b1c      	ldr	r3, [pc, #112]	; (e003f0c <sys_clear_ota_signature+0xf8>)
 e003e9a:	9a08      	ldr	r2, [sp, #32]
 e003e9c:	429a      	cmp	r2, r3
 e003e9e:	d108      	bne.n	e003eb2 <sys_clear_ota_signature+0x9e>
 e003ea0:	f103 437b 	add.w	r3, r3, #4211081216	; 0xfb000000
 e003ea4:	9a09      	ldr	r2, [sp, #36]	; 0x24
 e003ea6:	f503 0378 	add.w	r3, r3, #16252928	; 0xf80000
 e003eaa:	f503 63c0 	add.w	r3, r3, #1536	; 0x600
 e003eae:	429a      	cmp	r2, r3
 e003eb0:	d01d      	beq.n	e003eee <sys_clear_ota_signature+0xda>
 e003eb2:	2001      	movs	r0, #1
 e003eb4:	4b16      	ldr	r3, [pc, #88]	; (e003f10 <sys_clear_ota_signature+0xfc>)
 e003eb6:	2245      	movs	r2, #69	; 0x45
 e003eb8:	4912      	ldr	r1, [pc, #72]	; (e003f04 <sys_clear_ota_signature+0xf0>)
 e003eba:	e9cd 7000 	strd	r7, r0, [sp]
 e003ebe:	2002      	movs	r0, #2
 e003ec0:	e9cd 4602 	strd	r4, r6, [sp, #8]
 e003ec4:	f7fe faba 	bl	e00243c <rtk_log_write>
 e003ec8:	b024      	add	sp, #144	; 0x90
 e003eca:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e003ece:	9d07      	ldr	r5, [sp, #28]
 e003ed0:	a924      	add	r1, sp, #144	; 0x90
 e003ed2:	2801      	cmp	r0, #1
 e003ed4:	ea4f 0380 	mov.w	r3, r0, lsl #2
 e003ed8:	f105 4578 	add.w	r5, r5, #4160749568	; 0xf8000000
 e003edc:	eb01 0180 	add.w	r1, r1, r0, lsl #2
 e003ee0:	f841 5c68 	str.w	r5, [r1, #-104]
 e003ee4:	d1b6      	bne.n	e003e54 <sys_clear_ota_signature+0x40>
 e003ee6:	9d06      	ldr	r5, [sp, #24]
 e003ee8:	f105 4578 	add.w	r5, r5, #4160749568	; 0xf8000000
 e003eec:	e7b2      	b.n	e003e54 <sys_clear_ota_signature+0x40>
 e003eee:	aa0c      	add	r2, sp, #48	; 0x30
 e003ef0:	2108      	movs	r1, #8
 e003ef2:	4b08      	ldr	r3, [pc, #32]	; (e003f14 <sys_clear_ota_signature+0x100>)
 e003ef4:	4640      	mov	r0, r8
 e003ef6:	4798      	blx	r3
 e003ef8:	b024      	add	sp, #144	; 0x90
 e003efa:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e003efe:	bf00      	nop
 e003f00:	0e0087fc 	.word	0x0e0087fc
 e003f04:	0e008770 	.word	0x0e008770
 e003f08:	0e008720 	.word	0x0e008720
 e003f0c:	35393138 	.word	0x35393138
 e003f10:	0e008774 	.word	0x0e008774
 e003f14:	0e00170d 	.word	0x0e00170d

0e003f18 <sys_recover_ota_signature>:
 e003f18:	b5f0      	push	{r4, r5, r6, r7, lr}
 e003f1a:	4a3c      	ldr	r2, [pc, #240]	; (e00400c <sys_recover_ota_signature+0xf4>)
 e003f1c:	b08d      	sub	sp, #52	; 0x34
 e003f1e:	4b3c      	ldr	r3, [pc, #240]	; (e004010 <sys_recover_ota_signature+0xf8>)
 e003f20:	f44f 5080 	mov.w	r0, #4096	; 0x1000
 e003f24:	e9cd 230a 	strd	r2, r3, [sp, #40]	; 0x28
 e003f28:	f7fe f8bc 	bl	e0020a4 <__wrap_malloc>
 e003f2c:	2800      	cmp	r0, #0
 e003f2e:	d063      	beq.n	e003ff8 <sys_recover_ota_signature+0xe0>
 e003f30:	2200      	movs	r2, #0
 e003f32:	a906      	add	r1, sp, #24
 e003f34:	4605      	mov	r5, r0
 e003f36:	2002      	movs	r0, #2
 e003f38:	f7fd faf0 	bl	e00151c <flash_get_layout_info>
 e003f3c:	2200      	movs	r2, #0
 e003f3e:	a907      	add	r1, sp, #28
 e003f40:	2003      	movs	r0, #3
 e003f42:	f7fd faeb 	bl	e00151c <flash_get_layout_info>
 e003f46:	2001      	movs	r0, #1
 e003f48:	f7fd fede 	bl	e001d08 <ota_get_cur_index>
 e003f4c:	b2c0      	uxtb	r0, r0
 e003f4e:	2801      	cmp	r0, #1
 e003f50:	d048      	beq.n	e003fe4 <sys_recover_ota_signature+0xcc>
 e003f52:	f080 0201 	eor.w	r2, r0, #1
 e003f56:	9b07      	ldr	r3, [sp, #28]
 e003f58:	ac0c      	add	r4, sp, #48	; 0x30
 e003f5a:	f103 4378 	add.w	r3, r3, #4160749568	; 0xf8000000
 e003f5e:	0091      	lsls	r1, r2, #2
 e003f60:	eb04 0482 	add.w	r4, r4, r2, lsl #2
 e003f64:	3201      	adds	r2, #1
 e003f66:	f844 3c10 	str.w	r3, [r4, #-16]
 e003f6a:	b910      	cbnz	r0, e003f72 <sys_recover_ota_signature+0x5a>
 e003f6c:	9b06      	ldr	r3, [sp, #24]
 e003f6e:	f103 4378 	add.w	r3, r3, #4160749568	; 0xf8000000
 e003f72:	9204      	str	r2, [sp, #16]
 e003f74:	ac0c      	add	r4, sp, #48	; 0x30
 e003f76:	f101 0230 	add.w	r2, r1, #48	; 0x30
 e003f7a:	1c41      	adds	r1, r0, #1
 e003f7c:	eb04 0080 	add.w	r0, r4, r0, lsl #2
 e003f80:	4e24      	ldr	r6, [pc, #144]	; (e004014 <sys_recover_ota_signature+0xfc>)
 e003f82:	446a      	add	r2, sp
 e003f84:	f840 3c10 	str.w	r3, [r0, #-16]
 e003f88:	2001      	movs	r0, #1
 e003f8a:	f852 4c10 	ldr.w	r4, [r2, #-16]
 e003f8e:	2241      	movs	r2, #65	; 0x41
 e003f90:	9303      	str	r3, [sp, #12]
 e003f92:	4b21      	ldr	r3, [pc, #132]	; (e004018 <sys_recover_ota_signature+0x100>)
 e003f94:	f504 5780 	add.w	r7, r4, #4096	; 0x1000
 e003f98:	9405      	str	r4, [sp, #20]
 e003f9a:	9300      	str	r3, [sp, #0]
 e003f9c:	4b1f      	ldr	r3, [pc, #124]	; (e00401c <sys_recover_ota_signature+0x104>)
 e003f9e:	e9cd 0101 	strd	r0, r1, [sp, #4]
 e003fa2:	491f      	ldr	r1, [pc, #124]	; (e004020 <sys_recover_ota_signature+0x108>)
 e003fa4:	f7fe fa4a 	bl	e00243c <rtk_log_write>
 e003fa8:	462a      	mov	r2, r5
 e003faa:	4b1e      	ldr	r3, [pc, #120]	; (e004024 <sys_recover_ota_signature+0x10c>)
 e003fac:	f44f 5180 	mov.w	r1, #4096	; 0x1000
 e003fb0:	4620      	mov	r0, r4
 e003fb2:	4798      	blx	r3
 e003fb4:	4621      	mov	r1, r4
 e003fb6:	2002      	movs	r0, #2
 e003fb8:	f7fd fb24 	bl	e001604 <FLASH_EraseXIP>
 e003fbc:	2208      	movs	r2, #8
 e003fbe:	a90a      	add	r1, sp, #40	; 0x28
 e003fc0:	4628      	mov	r0, r5
 e003fc2:	4b19      	ldr	r3, [pc, #100]	; (e004028 <sys_recover_ota_signature+0x110>)
 e003fc4:	4798      	blx	r3
 e003fc6:	4620      	mov	r0, r4
 e003fc8:	f504 7480 	add.w	r4, r4, #256	; 0x100
 e003fcc:	462a      	mov	r2, r5
 e003fce:	f44f 7180 	mov.w	r1, #256	; 0x100
 e003fd2:	47b0      	blx	r6
 e003fd4:	42a7      	cmp	r7, r4
 e003fd6:	d1f6      	bne.n	e003fc6 <sys_recover_ota_signature+0xae>
 e003fd8:	4628      	mov	r0, r5
 e003fda:	b00d      	add	sp, #52	; 0x34
 e003fdc:	e8bd 40f0 	ldmia.w	sp!, {r4, r5, r6, r7, lr}
 e003fe0:	f7fe b862 	b.w	e0020a8 <__wrap_free>
 e003fe4:	e9dd 1306 	ldrd	r1, r3, [sp, #24]
 e003fe8:	4602      	mov	r2, r0
 e003fea:	f101 4178 	add.w	r1, r1, #4160749568	; 0xf8000000
 e003fee:	f103 4378 	add.w	r3, r3, #4160749568	; 0xf8000000
 e003ff2:	9108      	str	r1, [sp, #32]
 e003ff4:	2100      	movs	r1, #0
 e003ff6:	e7bc      	b.n	e003f72 <sys_recover_ota_signature+0x5a>
 e003ff8:	4c07      	ldr	r4, [pc, #28]	; (e004018 <sys_recover_ota_signature+0x100>)
 e003ffa:	2245      	movs	r2, #69	; 0x45
 e003ffc:	4b0b      	ldr	r3, [pc, #44]	; (e00402c <sys_recover_ota_signature+0x114>)
 e003ffe:	2002      	movs	r0, #2
 e004000:	4907      	ldr	r1, [pc, #28]	; (e004020 <sys_recover_ota_signature+0x108>)
 e004002:	9400      	str	r4, [sp, #0]
 e004004:	f7fe fa1a 	bl	e00243c <rtk_log_write>
 e004008:	b00d      	add	sp, #52	; 0x34
 e00400a:	bdf0      	pop	{r4, r5, r6, r7, pc}
 e00400c:	35393138 	.word	0x35393138
 e004010:	31313738 	.word	0x31313738
 e004014:	0e00170d 	.word	0x0e00170d
 e004018:	0e0087e0 	.word	0x0e0087e0
 e00401c:	0e008720 	.word	0x0e008720
 e004020:	0e008770 	.word	0x0e008770
 e004024:	0e0016d9 	.word	0x0e0016d9
 e004028:	00011c1d 	.word	0x00011c1d
 e00402c:	0e0087c4 	.word	0x0e0087c4

0e004030 <prvAddNewTaskToReadyList>:
 e004030:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
 e004034:	4604      	mov	r4, r0
 e004036:	f7ff f803 	bl	e003040 <vPortEnterCritical>
 e00403a:	4a30      	ldr	r2, [pc, #192]	; (e0040fc <prvAddNewTaskToReadyList+0xcc>)
 e00403c:	4d30      	ldr	r5, [pc, #192]	; (e004100 <prvAddNewTaskToReadyList+0xd0>)
 e00403e:	6813      	ldr	r3, [r2, #0]
 e004040:	3301      	adds	r3, #1
 e004042:	6013      	str	r3, [r2, #0]
 e004044:	682b      	ldr	r3, [r5, #0]
 e004046:	b33b      	cbz	r3, e004098 <prvAddNewTaskToReadyList+0x68>
 e004048:	4e2e      	ldr	r6, [pc, #184]	; (e004104 <prvAddNewTaskToReadyList+0xd4>)
 e00404a:	6ae0      	ldr	r0, [r4, #44]	; 0x2c
 e00404c:	6833      	ldr	r3, [r6, #0]
 e00404e:	b1e3      	cbz	r3, e00408a <prvAddNewTaskToReadyList+0x5a>
 e004050:	4f2d      	ldr	r7, [pc, #180]	; (e004108 <prvAddNewTaskToReadyList+0xd8>)
 e004052:	4a2e      	ldr	r2, [pc, #184]	; (e00410c <prvAddNewTaskToReadyList+0xdc>)
 e004054:	492e      	ldr	r1, [pc, #184]	; (e004110 <prvAddNewTaskToReadyList+0xe0>)
 e004056:	6813      	ldr	r3, [r2, #0]
 e004058:	3301      	adds	r3, #1
 e00405a:	6013      	str	r3, [r2, #0]
 e00405c:	6523      	str	r3, [r4, #80]	; 0x50
 e00405e:	680b      	ldr	r3, [r1, #0]
 e004060:	4283      	cmp	r3, r0
 e004062:	bf38      	it	cc
 e004064:	6008      	strcc	r0, [r1, #0]
 e004066:	eb00 0080 	add.w	r0, r0, r0, lsl #2
 e00406a:	1d21      	adds	r1, r4, #4
 e00406c:	eb07 0080 	add.w	r0, r7, r0, lsl #2
 e004070:	f7fe f948 	bl	e002304 <vListInsertEnd>
 e004074:	f7ff f802 	bl	e00307c <vPortExitCritical>
 e004078:	6833      	ldr	r3, [r6, #0]
 e00407a:	b123      	cbz	r3, e004086 <prvAddNewTaskToReadyList+0x56>
 e00407c:	682a      	ldr	r2, [r5, #0]
 e00407e:	6ae3      	ldr	r3, [r4, #44]	; 0x2c
 e004080:	6ad2      	ldr	r2, [r2, #44]	; 0x2c
 e004082:	429a      	cmp	r2, r3
 e004084:	d310      	bcc.n	e0040a8 <prvAddNewTaskToReadyList+0x78>
 e004086:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
 e00408a:	682b      	ldr	r3, [r5, #0]
 e00408c:	4f1e      	ldr	r7, [pc, #120]	; (e004108 <prvAddNewTaskToReadyList+0xd8>)
 e00408e:	6adb      	ldr	r3, [r3, #44]	; 0x2c
 e004090:	4283      	cmp	r3, r0
 e004092:	bf98      	it	ls
 e004094:	602c      	strls	r4, [r5, #0]
 e004096:	e7dc      	b.n	e004052 <prvAddNewTaskToReadyList+0x22>
 e004098:	602c      	str	r4, [r5, #0]
 e00409a:	6813      	ldr	r3, [r2, #0]
 e00409c:	2b01      	cmp	r3, #1
 e00409e:	d007      	beq.n	e0040b0 <prvAddNewTaskToReadyList+0x80>
 e0040a0:	6ae0      	ldr	r0, [r4, #44]	; 0x2c
 e0040a2:	4f19      	ldr	r7, [pc, #100]	; (e004108 <prvAddNewTaskToReadyList+0xd8>)
 e0040a4:	4e17      	ldr	r6, [pc, #92]	; (e004104 <prvAddNewTaskToReadyList+0xd4>)
 e0040a6:	e7d4      	b.n	e004052 <prvAddNewTaskToReadyList+0x22>
 e0040a8:	e8bd 43f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
 e0040ac:	f7fe bfb4 	b.w	e003018 <vPortYield>
 e0040b0:	4f15      	ldr	r7, [pc, #84]	; (e004108 <prvAddNewTaskToReadyList+0xd8>)
 e0040b2:	463e      	mov	r6, r7
 e0040b4:	f107 08dc 	add.w	r8, r7, #220	; 0xdc
 e0040b8:	4630      	mov	r0, r6
 e0040ba:	3614      	adds	r6, #20
 e0040bc:	f7fe f912 	bl	e0022e4 <vListInitialise>
 e0040c0:	45b0      	cmp	r8, r6
 e0040c2:	d1f9      	bne.n	e0040b8 <prvAddNewTaskToReadyList+0x88>
 e0040c4:	f8df 9060 	ldr.w	r9, [pc, #96]	; e004128 <prvAddNewTaskToReadyList+0xf8>
 e0040c8:	f8df 8060 	ldr.w	r8, [pc, #96]	; e00412c <prvAddNewTaskToReadyList+0xfc>
 e0040cc:	4648      	mov	r0, r9
 e0040ce:	4e0d      	ldr	r6, [pc, #52]	; (e004104 <prvAddNewTaskToReadyList+0xd4>)
 e0040d0:	f7fe f908 	bl	e0022e4 <vListInitialise>
 e0040d4:	4640      	mov	r0, r8
 e0040d6:	f7fe f905 	bl	e0022e4 <vListInitialise>
 e0040da:	480e      	ldr	r0, [pc, #56]	; (e004114 <prvAddNewTaskToReadyList+0xe4>)
 e0040dc:	f7fe f902 	bl	e0022e4 <vListInitialise>
 e0040e0:	480d      	ldr	r0, [pc, #52]	; (e004118 <prvAddNewTaskToReadyList+0xe8>)
 e0040e2:	f7fe f8ff 	bl	e0022e4 <vListInitialise>
 e0040e6:	480d      	ldr	r0, [pc, #52]	; (e00411c <prvAddNewTaskToReadyList+0xec>)
 e0040e8:	f7fe f8fc 	bl	e0022e4 <vListInitialise>
 e0040ec:	4b0c      	ldr	r3, [pc, #48]	; (e004120 <prvAddNewTaskToReadyList+0xf0>)
 e0040ee:	6ae0      	ldr	r0, [r4, #44]	; 0x2c
 e0040f0:	f8c3 9000 	str.w	r9, [r3]
 e0040f4:	4b0b      	ldr	r3, [pc, #44]	; (e004124 <prvAddNewTaskToReadyList+0xf4>)
 e0040f6:	f8c3 8000 	str.w	r8, [r3]
 e0040fa:	e7aa      	b.n	e004052 <prvAddNewTaskToReadyList+0x22>
 e0040fc:	2000ba60 	.word	0x2000ba60
 e004100:	2000b970 	.word	0x2000b970
 e004104:	2000babc 	.word	0x2000babc
 e004108:	2000b97c 	.word	0x2000b97c
 e00410c:	2000ba6c 	.word	0x2000ba6c
 e004110:	2000ba70 	.word	0x2000ba70
 e004114:	2000baa8 	.word	0x2000baa8
 e004118:	2000bad4 	.word	0x2000bad4
 e00411c:	2000bac0 	.word	0x2000bac0
 e004120:	2000b974 	.word	0x2000b974
 e004124:	2000b978 	.word	0x2000b978
 e004128:	2000ba74 	.word	0x2000ba74
 e00412c:	2000ba88 	.word	0x2000ba88

0e004130 <prvAddCurrentTaskToDelayedList>:
 e004130:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 e004132:	4b15      	ldr	r3, [pc, #84]	; (e004188 <prvAddCurrentTaskToDelayedList+0x58>)
 e004134:	4604      	mov	r4, r0
 e004136:	4e15      	ldr	r6, [pc, #84]	; (e00418c <prvAddCurrentTaskToDelayedList+0x5c>)
 e004138:	460f      	mov	r7, r1
 e00413a:	681d      	ldr	r5, [r3, #0]
 e00413c:	6833      	ldr	r3, [r6, #0]
 e00413e:	1d18      	adds	r0, r3, #4
 e004140:	f7fe f904 	bl	e00234c <uxListRemove>
 e004144:	1c63      	adds	r3, r4, #1
 e004146:	d100      	bne.n	e00414a <prvAddCurrentTaskToDelayedList+0x1a>
 e004148:	b9bf      	cbnz	r7, e00417a <prvAddCurrentTaskToDelayedList+0x4a>
 e00414a:	192c      	adds	r4, r5, r4
 e00414c:	6833      	ldr	r3, [r6, #0]
 e00414e:	605c      	str	r4, [r3, #4]
 e004150:	d307      	bcc.n	e004162 <prvAddCurrentTaskToDelayedList+0x32>
 e004152:	4b0f      	ldr	r3, [pc, #60]	; (e004190 <prvAddCurrentTaskToDelayedList+0x60>)
 e004154:	6818      	ldr	r0, [r3, #0]
 e004156:	6831      	ldr	r1, [r6, #0]
 e004158:	e8bd 40f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, lr}
 e00415c:	3104      	adds	r1, #4
 e00415e:	f7fe b8dd 	b.w	e00231c <vListInsert>
 e004162:	4b0c      	ldr	r3, [pc, #48]	; (e004194 <prvAddCurrentTaskToDelayedList+0x64>)
 e004164:	6818      	ldr	r0, [r3, #0]
 e004166:	6831      	ldr	r1, [r6, #0]
 e004168:	3104      	adds	r1, #4
 e00416a:	f7fe f8d7 	bl	e00231c <vListInsert>
 e00416e:	4b0a      	ldr	r3, [pc, #40]	; (e004198 <prvAddCurrentTaskToDelayedList+0x68>)
 e004170:	681a      	ldr	r2, [r3, #0]
 e004172:	42a2      	cmp	r2, r4
 e004174:	bf88      	it	hi
 e004176:	601c      	strhi	r4, [r3, #0]
 e004178:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e00417a:	6831      	ldr	r1, [r6, #0]
 e00417c:	4807      	ldr	r0, [pc, #28]	; (e00419c <prvAddCurrentTaskToDelayedList+0x6c>)
 e00417e:	3104      	adds	r1, #4
 e004180:	e8bd 40f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, lr}
 e004184:	f7fe b8be 	b.w	e002304 <vListInsertEnd>
 e004188:	2000bae8 	.word	0x2000bae8
 e00418c:	2000b970 	.word	0x2000b970
 e004190:	2000b978 	.word	0x2000b978
 e004194:	2000b974 	.word	0x2000b974
 e004198:	2000ba9c 	.word	0x2000ba9c
 e00419c:	2000bac0 	.word	0x2000bac0

0e0041a0 <vTaskSwitchContext.part.0>:
 e0041a0:	4a2f      	ldr	r2, [pc, #188]	; (e004260 <vTaskSwitchContext.part.0+0xc0>)
 e0041a2:	2000      	movs	r0, #0
 e0041a4:	492f      	ldr	r1, [pc, #188]	; (e004264 <vTaskSwitchContext.part.0+0xc4>)
 e0041a6:	4b30      	ldr	r3, [pc, #192]	; (e004268 <vTaskSwitchContext.part.0+0xc8>)
 e0041a8:	b570      	push	{r4, r5, r6, lr}
 e0041aa:	6010      	str	r0, [r2, #0]
 e0041ac:	4a2f      	ldr	r2, [pc, #188]	; (e00426c <vTaskSwitchContext.part.0+0xcc>)
 e0041ae:	6949      	ldr	r1, [r1, #20]
 e0041b0:	6810      	ldr	r0, [r2, #0]
 e0041b2:	6019      	str	r1, [r3, #0]
 e0041b4:	6819      	ldr	r1, [r3, #0]
 e0041b6:	4c2e      	ldr	r4, [pc, #184]	; (e004270 <vTaskSwitchContext.part.0+0xd0>)
 e0041b8:	4281      	cmp	r1, r0
 e0041ba:	d905      	bls.n	e0041c8 <vTaskSwitchContext.part.0+0x28>
 e0041bc:	681e      	ldr	r6, [r3, #0]
 e0041be:	6825      	ldr	r5, [r4, #0]
 e0041c0:	6e69      	ldr	r1, [r5, #100]	; 0x64
 e0041c2:	1a09      	subs	r1, r1, r0
 e0041c4:	4431      	add	r1, r6
 e0041c6:	6669      	str	r1, [r5, #100]	; 0x64
 e0041c8:	6819      	ldr	r1, [r3, #0]
 e0041ca:	6823      	ldr	r3, [r4, #0]
 e0041cc:	6011      	str	r1, [r2, #0]
 e0041ce:	6b1b      	ldr	r3, [r3, #48]	; 0x30
 e0041d0:	681a      	ldr	r2, [r3, #0]
 e0041d2:	f1b2 3fa5 	cmp.w	r2, #2779096485	; 0xa5a5a5a5
 e0041d6:	d103      	bne.n	e0041e0 <vTaskSwitchContext.part.0+0x40>
 e0041d8:	685a      	ldr	r2, [r3, #4]
 e0041da:	f1b2 3fa5 	cmp.w	r2, #2779096485	; 0xa5a5a5a5
 e0041de:	d036      	beq.n	e00424e <vTaskSwitchContext.part.0+0xae>
 e0041e0:	6820      	ldr	r0, [r4, #0]
 e0041e2:	6821      	ldr	r1, [r4, #0]
 e0041e4:	3134      	adds	r1, #52	; 0x34
 e0041e6:	f7fe fffb 	bl	e0031e0 <vApplicationStackOverflowHook>
 e0041ea:	4d22      	ldr	r5, [pc, #136]	; (e004274 <vTaskSwitchContext.part.0+0xd4>)
 e0041ec:	6823      	ldr	r3, [r4, #0]
 e0041ee:	682a      	ldr	r2, [r5, #0]
 e0041f0:	4e21      	ldr	r6, [pc, #132]	; (e004278 <vTaskSwitchContext.part.0+0xd8>)
 e0041f2:	f8c3 2160 	str.w	r2, [r3, #352]	; 0x160
 e0041f6:	6833      	ldr	r3, [r6, #0]
 e0041f8:	4920      	ldr	r1, [pc, #128]	; (e00427c <vTaskSwitchContext.part.0+0xdc>)
 e0041fa:	eb03 0283 	add.w	r2, r3, r3, lsl #2
 e0041fe:	0098      	lsls	r0, r3, #2
 e004200:	0092      	lsls	r2, r2, #2
 e004202:	588a      	ldr	r2, [r1, r2]
 e004204:	b93a      	cbnz	r2, e004216 <vTaskSwitchContext.part.0+0x76>
 e004206:	3b01      	subs	r3, #1
 e004208:	eb03 0283 	add.w	r2, r3, r3, lsl #2
 e00420c:	0098      	lsls	r0, r3, #2
 e00420e:	f851 2022 	ldr.w	r2, [r1, r2, lsl #2]
 e004212:	2a00      	cmp	r2, #0
 e004214:	d0f7      	beq.n	e004206 <vTaskSwitchContext.part.0+0x66>
 e004216:	4418      	add	r0, r3
 e004218:	eb01 0c80 	add.w	ip, r1, r0, lsl #2
 e00421c:	f8dc 2004 	ldr.w	r2, [ip, #4]
 e004220:	4661      	mov	r1, ip
 e004222:	6852      	ldr	r2, [r2, #4]
 e004224:	3108      	adds	r1, #8
 e004226:	428a      	cmp	r2, r1
 e004228:	f8cc 2004 	str.w	r2, [ip, #4]
 e00422c:	d00b      	beq.n	e004246 <vTaskSwitchContext.part.0+0xa6>
 e00422e:	68d2      	ldr	r2, [r2, #12]
 e004230:	6022      	str	r2, [r4, #0]
 e004232:	6033      	str	r3, [r6, #0]
 e004234:	6821      	ldr	r1, [r4, #0]
 e004236:	6823      	ldr	r3, [r4, #0]
 e004238:	4a11      	ldr	r2, [pc, #68]	; (e004280 <vTaskSwitchContext.part.0+0xe0>)
 e00423a:	f8d1 1160 	ldr.w	r1, [r1, #352]	; 0x160
 e00423e:	3368      	adds	r3, #104	; 0x68
 e004240:	6029      	str	r1, [r5, #0]
 e004242:	6013      	str	r3, [r2, #0]
 e004244:	bd70      	pop	{r4, r5, r6, pc}
 e004246:	6852      	ldr	r2, [r2, #4]
 e004248:	f8cc 2004 	str.w	r2, [ip, #4]
 e00424c:	e7ef      	b.n	e00422e <vTaskSwitchContext.part.0+0x8e>
 e00424e:	689a      	ldr	r2, [r3, #8]
 e004250:	f1b2 3fa5 	cmp.w	r2, #2779096485	; 0xa5a5a5a5
 e004254:	d1c4      	bne.n	e0041e0 <vTaskSwitchContext.part.0+0x40>
 e004256:	68db      	ldr	r3, [r3, #12]
 e004258:	f1b3 3fa5 	cmp.w	r3, #2779096485	; 0xa5a5a5a5
 e00425c:	d1c0      	bne.n	e0041e0 <vTaskSwitchContext.part.0+0x40>
 e00425e:	e7c4      	b.n	e0041ea <vTaskSwitchContext.part.0+0x4a>
 e004260:	2000baec 	.word	0x2000baec
 e004264:	41017000 	.word	0x41017000
 e004268:	2000ba5c 	.word	0x2000ba5c
 e00426c:	2000ba58 	.word	0x2000ba58
 e004270:	2000b970 	.word	0x2000b970
 e004274:	2000b96c 	.word	0x2000b96c
 e004278:	2000ba70 	.word	0x2000ba70
 e00427c:	2000b97c 	.word	0x2000b97c
 e004280:	2000b068 	.word	0x2000b068

0e004284 <prvInitialiseNewTask.constprop.0>:
 e004284:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 e004288:	9c0a      	ldr	r4, [sp, #40]	; 0x28
 e00428a:	0095      	lsls	r5, r2, #2
 e00428c:	4606      	mov	r6, r0
 e00428e:	460f      	mov	r7, r1
 e004290:	462a      	mov	r2, r5
 e004292:	21a5      	movs	r1, #165	; 0xa5
 e004294:	6b20      	ldr	r0, [r4, #48]	; 0x30
 e004296:	4698      	mov	r8, r3
 e004298:	f8dd a024 	ldr.w	sl, [sp, #36]	; 0x24
 e00429c:	3d04      	subs	r5, #4
 e00429e:	f003 fbb3 	bl	e007a08 <____wrap_memset_veneer>
 e0042a2:	6b23      	ldr	r3, [r4, #48]	; 0x30
 e0042a4:	441d      	add	r5, r3
 e0042a6:	f025 051f 	bic.w	r5, r5, #31
 e0042aa:	64e5      	str	r5, [r4, #76]	; 0x4c
 e0042ac:	2f00      	cmp	r7, #0
 e0042ae:	d04f      	beq.n	e004350 <prvInitialiseNewTask.constprop.0+0xcc>
 e0042b0:	f107 3cff 	add.w	ip, r7, #4294967295	; 0xffffffff
 e0042b4:	f107 0317 	add.w	r3, r7, #23
 e0042b8:	f104 0e33 	add.w	lr, r4, #51	; 0x33
 e0042bc:	e001      	b.n	e0042c2 <prvInitialiseNewTask.constprop.0+0x3e>
 e0042be:	459c      	cmp	ip, r3
 e0042c0:	d005      	beq.n	e0042ce <prvInitialiseNewTask.constprop.0+0x4a>
 e0042c2:	f81c 1f01 	ldrb.w	r1, [ip, #1]!
 e0042c6:	f80e 1f01 	strb.w	r1, [lr, #1]!
 e0042ca:	2900      	cmp	r1, #0
 e0042cc:	d1f7      	bne.n	e0042be <prvInitialiseNewTask.constprop.0+0x3a>
 e0042ce:	2300      	movs	r3, #0
 e0042d0:	f884 304b 	strb.w	r3, [r4, #75]	; 0x4b
 e0042d4:	9b08      	ldr	r3, [sp, #32]
 e0042d6:	2700      	movs	r7, #0
 e0042d8:	1d20      	adds	r0, r4, #4
 e0042da:	2b0a      	cmp	r3, #10
 e0042dc:	65e7      	str	r7, [r4, #92]	; 0x5c
 e0042de:	bf28      	it	cs
 e0042e0:	230a      	movcs	r3, #10
 e0042e2:	4699      	mov	r9, r3
 e0042e4:	62e3      	str	r3, [r4, #44]	; 0x2c
 e0042e6:	65a3      	str	r3, [r4, #88]	; 0x58
 e0042e8:	f7fe f808 	bl	e0022fc <vListInitialiseItem>
 e0042ec:	f104 0018 	add.w	r0, r4, #24
 e0042f0:	f7fe f804 	bl	e0022fc <vListInitialiseItem>
 e0042f4:	f1c9 030b 	rsb	r3, r9, #11
 e0042f8:	2204      	movs	r2, #4
 e0042fa:	4639      	mov	r1, r7
 e0042fc:	61a3      	str	r3, [r4, #24]
 e0042fe:	f504 70ac 	add.w	r0, r4, #344	; 0x158
 e004302:	6124      	str	r4, [r4, #16]
 e004304:	6264      	str	r4, [r4, #36]	; 0x24
 e004306:	e9c4 7718 	strd	r7, r7, [r4, #96]	; 0x60
 e00430a:	f003 fb7d 	bl	e007a08 <____wrap_memset_veneer>
 e00430e:	2201      	movs	r2, #1
 e004310:	4639      	mov	r1, r7
 e004312:	f504 70ae 	add.w	r0, r4, #348	; 0x15c
 e004316:	f003 fb77 	bl	e007a08 <____wrap_memset_veneer>
 e00431a:	4639      	mov	r1, r7
 e00431c:	22f0      	movs	r2, #240	; 0xf0
 e00431e:	f104 0068 	add.w	r0, r4, #104	; 0x68
 e004322:	f003 fb71 	bl	e007a08 <____wrap_memset_veneer>
 e004326:	490c      	ldr	r1, [pc, #48]	; (e004358 <prvInitialiseNewTask.constprop.0+0xd4>)
 e004328:	4628      	mov	r0, r5
 e00432a:	4643      	mov	r3, r8
 e00432c:	f101 0568 	add.w	r5, r1, #104	; 0x68
 e004330:	66e1      	str	r1, [r4, #108]	; 0x6c
 e004332:	31d0      	adds	r1, #208	; 0xd0
 e004334:	4632      	mov	r2, r6
 e004336:	6725      	str	r5, [r4, #112]	; 0x70
 e004338:	6761      	str	r1, [r4, #116]	; 0x74
 e00433a:	6b21      	ldr	r1, [r4, #48]	; 0x30
 e00433c:	f7fe fec0 	bl	e0030c0 <pxPortInitialiseStack>
 e004340:	6020      	str	r0, [r4, #0]
 e004342:	f1ba 0f00 	cmp.w	sl, #0
 e004346:	d001      	beq.n	e00434c <prvInitialiseNewTask.constprop.0+0xc8>
 e004348:	f8ca 4000 	str.w	r4, [sl]
 e00434c:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
 e004350:	f884 7034 	strb.w	r7, [r4, #52]	; 0x34
 e004354:	e7be      	b.n	e0042d4 <prvInitialiseNewTask.constprop.0+0x50>
 e004356:	bf00      	nop
 e004358:	2000bfb0 	.word	0x2000bfb0

0e00435c <xTaskIncrementTick.part.0>:
 e00435c:	4b43      	ldr	r3, [pc, #268]	; (e00446c <xTaskIncrementTick.part.0+0x110>)
 e00435e:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e004362:	681e      	ldr	r6, [r3, #0]
 e004364:	b083      	sub	sp, #12
 e004366:	3601      	adds	r6, #1
 e004368:	601e      	str	r6, [r3, #0]
 e00436a:	2e00      	cmp	r6, #0
 e00436c:	d059      	beq.n	e004422 <xTaskIncrementTick.part.0+0xc6>
 e00436e:	f8df b110 	ldr.w	fp, [pc, #272]	; e004480 <xTaskIncrementTick.part.0+0x124>
 e004372:	f8db 3000 	ldr.w	r3, [fp]
 e004376:	429e      	cmp	r6, r3
 e004378:	d33a      	bcc.n	e0043f0 <xTaskIncrementTick.part.0+0x94>
 e00437a:	4f3d      	ldr	r7, [pc, #244]	; (e004470 <xTaskIncrementTick.part.0+0x114>)
 e00437c:	683b      	ldr	r3, [r7, #0]
 e00437e:	681d      	ldr	r5, [r3, #0]
 e004380:	2d00      	cmp	r5, #0
 e004382:	d062      	beq.n	e00444a <xTaskIncrementTick.part.0+0xee>
 e004384:	2500      	movs	r5, #0
 e004386:	f8df 90fc 	ldr.w	r9, [pc, #252]	; e004484 <xTaskIncrementTick.part.0+0x128>
 e00438a:	f8df a0fc 	ldr.w	sl, [pc, #252]	; e004488 <xTaskIncrementTick.part.0+0x12c>
 e00438e:	f8df 80fc 	ldr.w	r8, [pc, #252]	; e00448c <xTaskIncrementTick.part.0+0x130>
 e004392:	e022      	b.n	e0043da <xTaskIncrementTick.part.0+0x7e>
 e004394:	9101      	str	r1, [sp, #4]
 e004396:	f7fd ffd9 	bl	e00234c <uxListRemove>
 e00439a:	6aa3      	ldr	r3, [r4, #40]	; 0x28
 e00439c:	f104 0018 	add.w	r0, r4, #24
 e0043a0:	9901      	ldr	r1, [sp, #4]
 e0043a2:	b113      	cbz	r3, e0043aa <xTaskIncrementTick.part.0+0x4e>
 e0043a4:	f7fd ffd2 	bl	e00234c <uxListRemove>
 e0043a8:	9901      	ldr	r1, [sp, #4]
 e0043aa:	6ae3      	ldr	r3, [r4, #44]	; 0x2c
 e0043ac:	f8d8 2000 	ldr.w	r2, [r8]
 e0043b0:	eb03 0083 	add.w	r0, r3, r3, lsl #2
 e0043b4:	4293      	cmp	r3, r2
 e0043b6:	eb09 0080 	add.w	r0, r9, r0, lsl #2
 e0043ba:	bf88      	it	hi
 e0043bc:	f8c8 3000 	strhi.w	r3, [r8]
 e0043c0:	f7fd ffa0 	bl	e002304 <vListInsertEnd>
 e0043c4:	f8da 2000 	ldr.w	r2, [sl]
 e0043c8:	683b      	ldr	r3, [r7, #0]
 e0043ca:	6ae1      	ldr	r1, [r4, #44]	; 0x2c
 e0043cc:	6ad2      	ldr	r2, [r2, #44]	; 0x2c
 e0043ce:	681b      	ldr	r3, [r3, #0]
 e0043d0:	4291      	cmp	r1, r2
 e0043d2:	bf28      	it	cs
 e0043d4:	2501      	movcs	r5, #1
 e0043d6:	2b00      	cmp	r3, #0
 e0043d8:	d03b      	beq.n	e004452 <xTaskIncrementTick.part.0+0xf6>
 e0043da:	683b      	ldr	r3, [r7, #0]
 e0043dc:	68db      	ldr	r3, [r3, #12]
 e0043de:	68dc      	ldr	r4, [r3, #12]
 e0043e0:	6863      	ldr	r3, [r4, #4]
 e0043e2:	1d21      	adds	r1, r4, #4
 e0043e4:	429e      	cmp	r6, r3
 e0043e6:	4608      	mov	r0, r1
 e0043e8:	d2d4      	bcs.n	e004394 <xTaskIncrementTick.part.0+0x38>
 e0043ea:	f8cb 3000 	str.w	r3, [fp]
 e0043ee:	e004      	b.n	e0043fa <xTaskIncrementTick.part.0+0x9e>
 e0043f0:	2500      	movs	r5, #0
 e0043f2:	f8df 9090 	ldr.w	r9, [pc, #144]	; e004484 <xTaskIncrementTick.part.0+0x128>
 e0043f6:	f8df a090 	ldr.w	sl, [pc, #144]	; e004488 <xTaskIncrementTick.part.0+0x12c>
 e0043fa:	f8da 3000 	ldr.w	r3, [sl]
 e0043fe:	491d      	ldr	r1, [pc, #116]	; (e004474 <xTaskIncrementTick.part.0+0x118>)
 e004400:	6adb      	ldr	r3, [r3, #44]	; 0x2c
 e004402:	eb03 0383 	add.w	r3, r3, r3, lsl #2
 e004406:	009b      	lsls	r3, r3, #2
 e004408:	f859 2003 	ldr.w	r2, [r9, r3]
 e00440c:	680b      	ldr	r3, [r1, #0]
 e00440e:	2a02      	cmp	r2, #2
 e004410:	bf28      	it	cs
 e004412:	2501      	movcs	r5, #1
 e004414:	2b00      	cmp	r3, #0
 e004416:	bf0c      	ite	eq
 e004418:	4628      	moveq	r0, r5
 e00441a:	2001      	movne	r0, #1
 e00441c:	b003      	add	sp, #12
 e00441e:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 e004422:	4b13      	ldr	r3, [pc, #76]	; (e004470 <xTaskIncrementTick.part.0+0x114>)
 e004424:	4a14      	ldr	r2, [pc, #80]	; (e004478 <xTaskIncrementTick.part.0+0x11c>)
 e004426:	6818      	ldr	r0, [r3, #0]
 e004428:	4914      	ldr	r1, [pc, #80]	; (e00447c <xTaskIncrementTick.part.0+0x120>)
 e00442a:	6814      	ldr	r4, [r2, #0]
 e00442c:	601c      	str	r4, [r3, #0]
 e00442e:	6010      	str	r0, [r2, #0]
 e004430:	680a      	ldr	r2, [r1, #0]
 e004432:	3201      	adds	r2, #1
 e004434:	600a      	str	r2, [r1, #0]
 e004436:	681a      	ldr	r2, [r3, #0]
 e004438:	6812      	ldr	r2, [r2, #0]
 e00443a:	b97a      	cbnz	r2, e00445c <xTaskIncrementTick.part.0+0x100>
 e00443c:	f8df b040 	ldr.w	fp, [pc, #64]	; e004480 <xTaskIncrementTick.part.0+0x124>
 e004440:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
 e004444:	f8cb 3000 	str.w	r3, [fp]
 e004448:	e793      	b.n	e004372 <xTaskIncrementTick.part.0+0x16>
 e00444a:	f8df 9038 	ldr.w	r9, [pc, #56]	; e004484 <xTaskIncrementTick.part.0+0x128>
 e00444e:	f8df a038 	ldr.w	sl, [pc, #56]	; e004488 <xTaskIncrementTick.part.0+0x12c>
 e004452:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
 e004456:	f8cb 3000 	str.w	r3, [fp]
 e00445a:	e7ce      	b.n	e0043fa <xTaskIncrementTick.part.0+0x9e>
 e00445c:	681b      	ldr	r3, [r3, #0]
 e00445e:	f8df b020 	ldr.w	fp, [pc, #32]	; e004480 <xTaskIncrementTick.part.0+0x124>
 e004462:	68db      	ldr	r3, [r3, #12]
 e004464:	681b      	ldr	r3, [r3, #0]
 e004466:	f8cb 3000 	str.w	r3, [fp]
 e00446a:	e782      	b.n	e004372 <xTaskIncrementTick.part.0+0x16>
 e00446c:	2000bae8 	.word	0x2000bae8
 e004470:	2000b974 	.word	0x2000b974
 e004474:	2000baec 	.word	0x2000baec
 e004478:	2000b978 	.word	0x2000b978
 e00447c:	2000baa0 	.word	0x2000baa0
 e004480:	2000ba9c 	.word	0x2000ba9c
 e004484:	2000b97c 	.word	0x2000b97c
 e004488:	2000b970 	.word	0x2000b970
 e00448c:	2000ba70 	.word	0x2000ba70

0e004490 <xTaskCreateStatic>:
 e004490:	b530      	push	{r4, r5, lr}
 e004492:	b087      	sub	sp, #28
 e004494:	9c0c      	ldr	r4, [sp, #48]	; 0x30
 e004496:	b10c      	cbz	r4, e00449c <xTaskCreateStatic+0xc>
 e004498:	9d0b      	ldr	r5, [sp, #44]	; 0x2c
 e00449a:	b915      	cbnz	r5, e0044a2 <xTaskCreateStatic+0x12>
 e00449c:	2000      	movs	r0, #0
 e00449e:	b007      	add	sp, #28
 e0044a0:	bd30      	pop	{r4, r5, pc}
 e0044a2:	6325      	str	r5, [r4, #48]	; 0x30
 e0044a4:	9d0a      	ldr	r5, [sp, #40]	; 0x28
 e0044a6:	9402      	str	r4, [sp, #8]
 e0044a8:	9500      	str	r5, [sp, #0]
 e0044aa:	2502      	movs	r5, #2
 e0044ac:	f884 515d 	strb.w	r5, [r4, #349]	; 0x15d
 e0044b0:	ad05      	add	r5, sp, #20
 e0044b2:	9501      	str	r5, [sp, #4]
 e0044b4:	f7ff fee6 	bl	e004284 <prvInitialiseNewTask.constprop.0>
 e0044b8:	4620      	mov	r0, r4
 e0044ba:	f7ff fdb9 	bl	e004030 <prvAddNewTaskToReadyList>
 e0044be:	9805      	ldr	r0, [sp, #20]
 e0044c0:	b007      	add	sp, #28
 e0044c2:	bd30      	pop	{r4, r5, pc}

0e0044c4 <xTaskCreate>:
 e0044c4:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
 e0044c8:	4607      	mov	r7, r0
 e0044ca:	b085      	sub	sp, #20
 e0044cc:	0090      	lsls	r0, r2, #2
 e0044ce:	4615      	mov	r5, r2
 e0044d0:	4688      	mov	r8, r1
 e0044d2:	4699      	mov	r9, r3
 e0044d4:	f7fd fe1e 	bl	e002114 <pvPortMalloc>
 e0044d8:	b1e0      	cbz	r0, e004514 <xTaskCreate+0x50>
 e0044da:	4604      	mov	r4, r0
 e0044dc:	f44f 70b2 	mov.w	r0, #356	; 0x164
 e0044e0:	f7fd fe18 	bl	e002114 <pvPortMalloc>
 e0044e4:	4606      	mov	r6, r0
 e0044e6:	b1d0      	cbz	r0, e00451e <xTaskCreate+0x5a>
 e0044e8:	6304      	str	r4, [r0, #48]	; 0x30
 e0044ea:	2400      	movs	r4, #0
 e0044ec:	464b      	mov	r3, r9
 e0044ee:	462a      	mov	r2, r5
 e0044f0:	f886 415d 	strb.w	r4, [r6, #349]	; 0x15d
 e0044f4:	4641      	mov	r1, r8
 e0044f6:	9c0d      	ldr	r4, [sp, #52]	; 0x34
 e0044f8:	4638      	mov	r0, r7
 e0044fa:	9602      	str	r6, [sp, #8]
 e0044fc:	9401      	str	r4, [sp, #4]
 e0044fe:	9c0c      	ldr	r4, [sp, #48]	; 0x30
 e004500:	9400      	str	r4, [sp, #0]
 e004502:	f7ff febf 	bl	e004284 <prvInitialiseNewTask.constprop.0>
 e004506:	4630      	mov	r0, r6
 e004508:	f7ff fd92 	bl	e004030 <prvAddNewTaskToReadyList>
 e00450c:	2001      	movs	r0, #1
 e00450e:	b005      	add	sp, #20
 e004510:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 e004514:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
 e004518:	b005      	add	sp, #20
 e00451a:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 e00451e:	4620      	mov	r0, r4
 e004520:	f7fd fe5c 	bl	e0021dc <vPortFree>
 e004524:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
 e004528:	e7f1      	b.n	e00450e <xTaskCreate+0x4a>
 e00452a:	bf00      	nop

0e00452c <vTaskDelete>:
 e00452c:	b570      	push	{r4, r5, r6, lr}
 e00452e:	4604      	mov	r4, r0
 e004530:	4d2b      	ldr	r5, [pc, #172]	; (e0045e0 <vTaskDelete+0xb4>)
 e004532:	f7fe fd85 	bl	e003040 <vPortEnterCritical>
 e004536:	2c00      	cmp	r4, #0
 e004538:	d048      	beq.n	e0045cc <vTaskDelete+0xa0>
 e00453a:	1d26      	adds	r6, r4, #4
 e00453c:	4630      	mov	r0, r6
 e00453e:	f7fd ff05 	bl	e00234c <uxListRemove>
 e004542:	6aa3      	ldr	r3, [r4, #40]	; 0x28
 e004544:	b11b      	cbz	r3, e00454e <vTaskDelete+0x22>
 e004546:	f104 0018 	add.w	r0, r4, #24
 e00454a:	f7fd feff 	bl	e00234c <uxListRemove>
 e00454e:	4a25      	ldr	r2, [pc, #148]	; (e0045e4 <vTaskDelete+0xb8>)
 e004550:	6813      	ldr	r3, [r2, #0]
 e004552:	3301      	adds	r3, #1
 e004554:	6013      	str	r3, [r2, #0]
 e004556:	682b      	ldr	r3, [r5, #0]
 e004558:	42a3      	cmp	r3, r4
 e00455a:	d02e      	beq.n	e0045ba <vTaskDelete+0x8e>
 e00455c:	4a22      	ldr	r2, [pc, #136]	; (e0045e8 <vTaskDelete+0xbc>)
 e00455e:	f104 0068 	add.w	r0, r4, #104	; 0x68
 e004562:	6813      	ldr	r3, [r2, #0]
 e004564:	3b01      	subs	r3, #1
 e004566:	6013      	str	r3, [r2, #0]
 e004568:	f003 f927 	bl	e0077ba <_reclaim_reent>
 e00456c:	f894 315d 	ldrb.w	r3, [r4, #349]	; 0x15d
 e004570:	b193      	cbz	r3, e004598 <vTaskDelete+0x6c>
 e004572:	2b01      	cmp	r3, #1
 e004574:	d030      	beq.n	e0045d8 <vTaskDelete+0xac>
 e004576:	4b1d      	ldr	r3, [pc, #116]	; (e0045ec <vTaskDelete+0xc0>)
 e004578:	681a      	ldr	r2, [r3, #0]
 e00457a:	6812      	ldr	r2, [r2, #0]
 e00457c:	b9ba      	cbnz	r2, e0045ae <vTaskDelete+0x82>
 e00457e:	4b1c      	ldr	r3, [pc, #112]	; (e0045f0 <vTaskDelete+0xc4>)
 e004580:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
 e004584:	601a      	str	r2, [r3, #0]
 e004586:	f7fe fd79 	bl	e00307c <vPortExitCritical>
 e00458a:	4b1a      	ldr	r3, [pc, #104]	; (e0045f4 <vTaskDelete+0xc8>)
 e00458c:	681b      	ldr	r3, [r3, #0]
 e00458e:	b113      	cbz	r3, e004596 <vTaskDelete+0x6a>
 e004590:	682b      	ldr	r3, [r5, #0]
 e004592:	42a3      	cmp	r3, r4
 e004594:	d01c      	beq.n	e0045d0 <vTaskDelete+0xa4>
 e004596:	bd70      	pop	{r4, r5, r6, pc}
 e004598:	6b20      	ldr	r0, [r4, #48]	; 0x30
 e00459a:	f7fd fe1f 	bl	e0021dc <vPortFree>
 e00459e:	4620      	mov	r0, r4
 e0045a0:	f7fd fe1c 	bl	e0021dc <vPortFree>
 e0045a4:	4b11      	ldr	r3, [pc, #68]	; (e0045ec <vTaskDelete+0xc0>)
 e0045a6:	681a      	ldr	r2, [r3, #0]
 e0045a8:	6812      	ldr	r2, [r2, #0]
 e0045aa:	2a00      	cmp	r2, #0
 e0045ac:	d0e7      	beq.n	e00457e <vTaskDelete+0x52>
 e0045ae:	681a      	ldr	r2, [r3, #0]
 e0045b0:	4b0f      	ldr	r3, [pc, #60]	; (e0045f0 <vTaskDelete+0xc4>)
 e0045b2:	68d2      	ldr	r2, [r2, #12]
 e0045b4:	6812      	ldr	r2, [r2, #0]
 e0045b6:	601a      	str	r2, [r3, #0]
 e0045b8:	e7e5      	b.n	e004586 <vTaskDelete+0x5a>
 e0045ba:	4631      	mov	r1, r6
 e0045bc:	480e      	ldr	r0, [pc, #56]	; (e0045f8 <vTaskDelete+0xcc>)
 e0045be:	f7fd fea1 	bl	e002304 <vListInsertEnd>
 e0045c2:	4a0e      	ldr	r2, [pc, #56]	; (e0045fc <vTaskDelete+0xd0>)
 e0045c4:	6813      	ldr	r3, [r2, #0]
 e0045c6:	3301      	adds	r3, #1
 e0045c8:	6013      	str	r3, [r2, #0]
 e0045ca:	e7dc      	b.n	e004586 <vTaskDelete+0x5a>
 e0045cc:	682c      	ldr	r4, [r5, #0]
 e0045ce:	e7b4      	b.n	e00453a <vTaskDelete+0xe>
 e0045d0:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
 e0045d4:	f7fe bd20 	b.w	e003018 <vPortYield>
 e0045d8:	4620      	mov	r0, r4
 e0045da:	f7fd fdff 	bl	e0021dc <vPortFree>
 e0045de:	e7ca      	b.n	e004576 <vTaskDelete+0x4a>
 e0045e0:	2000b970 	.word	0x2000b970
 e0045e4:	2000ba6c 	.word	0x2000ba6c
 e0045e8:	2000ba60 	.word	0x2000ba60
 e0045ec:	2000b974 	.word	0x2000b974
 e0045f0:	2000ba9c 	.word	0x2000ba9c
 e0045f4:	2000babc 	.word	0x2000babc
 e0045f8:	2000bad4 	.word	0x2000bad4
 e0045fc:	2000ba64 	.word	0x2000ba64

0e004600 <eTaskGetState>:
 e004600:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 e004602:	4b15      	ldr	r3, [pc, #84]	; (e004658 <eTaskGetState+0x58>)
 e004604:	681b      	ldr	r3, [r3, #0]
 e004606:	4283      	cmp	r3, r0
 e004608:	d01a      	beq.n	e004640 <eTaskGetState+0x40>
 e00460a:	4604      	mov	r4, r0
 e00460c:	f7fe fd18 	bl	e003040 <vPortEnterCritical>
 e004610:	4a12      	ldr	r2, [pc, #72]	; (e00465c <eTaskGetState+0x5c>)
 e004612:	6965      	ldr	r5, [r4, #20]
 e004614:	6816      	ldr	r6, [r2, #0]
 e004616:	4b12      	ldr	r3, [pc, #72]	; (e004660 <eTaskGetState+0x60>)
 e004618:	681f      	ldr	r7, [r3, #0]
 e00461a:	f7fe fd2f 	bl	e00307c <vPortExitCritical>
 e00461e:	42b5      	cmp	r5, r6
 e004620:	d00c      	beq.n	e00463c <eTaskGetState+0x3c>
 e004622:	42bd      	cmp	r5, r7
 e004624:	d00a      	beq.n	e00463c <eTaskGetState+0x3c>
 e004626:	4b0f      	ldr	r3, [pc, #60]	; (e004664 <eTaskGetState+0x64>)
 e004628:	429d      	cmp	r5, r3
 e00462a:	d005      	beq.n	e004638 <eTaskGetState+0x38>
 e00462c:	4b0e      	ldr	r3, [pc, #56]	; (e004668 <eTaskGetState+0x68>)
 e00462e:	429d      	cmp	r5, r3
 e004630:	d008      	beq.n	e004644 <eTaskGetState+0x44>
 e004632:	b13d      	cbz	r5, e004644 <eTaskGetState+0x44>
 e004634:	2001      	movs	r0, #1
 e004636:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e004638:	6aa3      	ldr	r3, [r4, #40]	; 0x28
 e00463a:	b12b      	cbz	r3, e004648 <eTaskGetState+0x48>
 e00463c:	2002      	movs	r0, #2
 e00463e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e004640:	2000      	movs	r0, #0
 e004642:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e004644:	2004      	movs	r0, #4
 e004646:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e004648:	f894 315c 	ldrb.w	r3, [r4, #348]	; 0x15c
 e00464c:	2b01      	cmp	r3, #1
 e00464e:	bf0c      	ite	eq
 e004650:	2002      	moveq	r0, #2
 e004652:	2003      	movne	r0, #3
 e004654:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e004656:	bf00      	nop
 e004658:	2000b970 	.word	0x2000b970
 e00465c:	2000b974 	.word	0x2000b974
 e004660:	2000b978 	.word	0x2000b978
 e004664:	2000bac0 	.word	0x2000bac0
 e004668:	2000bad4 	.word	0x2000bad4

0e00466c <vTaskStartScheduler>:
 e00466c:	b530      	push	{r4, r5, lr}
 e00466e:	b089      	sub	sp, #36	; 0x24
 e004670:	2400      	movs	r4, #0
 e004672:	aa06      	add	r2, sp, #24
 e004674:	a905      	add	r1, sp, #20
 e004676:	a804      	add	r0, sp, #16
 e004678:	e9cd 4404 	strd	r4, r4, [sp, #16]
 e00467c:	f7fe fdda 	bl	e003234 <vApplicationGetIdleTaskMemory>
 e004680:	e9dd 5304 	ldrd	r5, r3, [sp, #16]
 e004684:	b105      	cbz	r5, e004688 <vTaskStartScheduler+0x1c>
 e004686:	b91b      	cbnz	r3, e004690 <vTaskStartScheduler+0x24>
 e004688:	4b18      	ldr	r3, [pc, #96]	; (e0046ec <vTaskStartScheduler+0x80>)
 e00468a:	681b      	ldr	r3, [r3, #0]
 e00468c:	b009      	add	sp, #36	; 0x24
 e00468e:	bd30      	pop	{r4, r5, pc}
 e004690:	2202      	movs	r2, #2
 e004692:	a907      	add	r1, sp, #28
 e004694:	632b      	str	r3, [r5, #48]	; 0x30
 e004696:	4623      	mov	r3, r4
 e004698:	f885 215d 	strb.w	r2, [r5, #349]	; 0x15d
 e00469c:	4814      	ldr	r0, [pc, #80]	; (e0046f0 <vTaskStartScheduler+0x84>)
 e00469e:	9a06      	ldr	r2, [sp, #24]
 e0046a0:	9400      	str	r4, [sp, #0]
 e0046a2:	e9cd 1501 	strd	r1, r5, [sp, #4]
 e0046a6:	4913      	ldr	r1, [pc, #76]	; (e0046f4 <vTaskStartScheduler+0x88>)
 e0046a8:	f7ff fdec 	bl	e004284 <prvInitialiseNewTask.constprop.0>
 e0046ac:	4628      	mov	r0, r5
 e0046ae:	f7ff fcbf 	bl	e004030 <prvAddNewTaskToReadyList>
 e0046b2:	9b07      	ldr	r3, [sp, #28]
 e0046b4:	2b00      	cmp	r3, #0
 e0046b6:	d0e7      	beq.n	e004688 <vTaskStartScheduler+0x1c>
 e0046b8:	f000 fbd8 	bl	e004e6c <xTimerCreateTimerTask>
 e0046bc:	2801      	cmp	r0, #1
 e0046be:	4605      	mov	r5, r0
 e0046c0:	d1e2      	bne.n	e004688 <vTaskStartScheduler+0x1c>
 e0046c2:	f7fe fcb5 	bl	e003030 <ulPortSetInterruptMask>
 e0046c6:	4b0c      	ldr	r3, [pc, #48]	; (e0046f8 <vTaskStartScheduler+0x8c>)
 e0046c8:	4a0c      	ldr	r2, [pc, #48]	; (e0046fc <vTaskStartScheduler+0x90>)
 e0046ca:	681b      	ldr	r3, [r3, #0]
 e0046cc:	490c      	ldr	r1, [pc, #48]	; (e004700 <vTaskStartScheduler+0x94>)
 e0046ce:	3368      	adds	r3, #104	; 0x68
 e0046d0:	6013      	str	r3, [r2, #0]
 e0046d2:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
 e0046d6:	4a0b      	ldr	r2, [pc, #44]	; (e004704 <vTaskStartScheduler+0x98>)
 e0046d8:	600b      	str	r3, [r1, #0]
 e0046da:	4b0b      	ldr	r3, [pc, #44]	; (e004708 <vTaskStartScheduler+0x9c>)
 e0046dc:	6015      	str	r5, [r2, #0]
 e0046de:	601c      	str	r4, [r3, #0]
 e0046e0:	f7fe fd32 	bl	e003148 <xPortStartScheduler>
 e0046e4:	4b01      	ldr	r3, [pc, #4]	; (e0046ec <vTaskStartScheduler+0x80>)
 e0046e6:	681b      	ldr	r3, [r3, #0]
 e0046e8:	b009      	add	sp, #36	; 0x24
 e0046ea:	bd30      	pop	{r4, r5, pc}
 e0046ec:	2000b064 	.word	0x2000b064
 e0046f0:	0e004861 	.word	0x0e004861
 e0046f4:	0e008814 	.word	0x0e008814
 e0046f8:	2000b970 	.word	0x2000b970
 e0046fc:	2000b068 	.word	0x2000b068
 e004700:	2000ba9c 	.word	0x2000ba9c
 e004704:	2000babc 	.word	0x2000babc
 e004708:	2000bae8 	.word	0x2000bae8

0e00470c <vTaskSuspendAll>:
 e00470c:	4a02      	ldr	r2, [pc, #8]	; (e004718 <vTaskSuspendAll+0xc>)
 e00470e:	6813      	ldr	r3, [r2, #0]
 e004710:	3301      	adds	r3, #1
 e004712:	6013      	str	r3, [r2, #0]
 e004714:	4770      	bx	lr
 e004716:	bf00      	nop
 e004718:	2000ba68 	.word	0x2000ba68

0e00471c <xTaskResumeAll>:
 e00471c:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e004720:	4d3a      	ldr	r5, [pc, #232]	; (e00480c <xTaskResumeAll+0xf0>)
 e004722:	b083      	sub	sp, #12
 e004724:	f7fe fc8c 	bl	e003040 <vPortEnterCritical>
 e004728:	682b      	ldr	r3, [r5, #0]
 e00472a:	3b01      	subs	r3, #1
 e00472c:	602b      	str	r3, [r5, #0]
 e00472e:	682b      	ldr	r3, [r5, #0]
 e004730:	2b00      	cmp	r3, #0
 e004732:	d153      	bne.n	e0047dc <xTaskResumeAll+0xc0>
 e004734:	4b36      	ldr	r3, [pc, #216]	; (e004810 <xTaskResumeAll+0xf4>)
 e004736:	681b      	ldr	r3, [r3, #0]
 e004738:	2b00      	cmp	r3, #0
 e00473a:	d04f      	beq.n	e0047dc <xTaskResumeAll+0xc0>
 e00473c:	4e35      	ldr	r6, [pc, #212]	; (e004814 <xTaskResumeAll+0xf8>)
 e00473e:	6833      	ldr	r3, [r6, #0]
 e004740:	2b00      	cmp	r3, #0
 e004742:	d05f      	beq.n	e004804 <xTaskResumeAll+0xe8>
 e004744:	4f34      	ldr	r7, [pc, #208]	; (e004818 <xTaskResumeAll+0xfc>)
 e004746:	f8df 90e0 	ldr.w	r9, [pc, #224]	; e004828 <xTaskResumeAll+0x10c>
 e00474a:	f8df 80e0 	ldr.w	r8, [pc, #224]	; e00482c <xTaskResumeAll+0x110>
 e00474e:	f8df a0e0 	ldr.w	sl, [pc, #224]	; e004830 <xTaskResumeAll+0x114>
 e004752:	68f3      	ldr	r3, [r6, #12]
 e004754:	68dc      	ldr	r4, [r3, #12]
 e004756:	f104 0b04 	add.w	fp, r4, #4
 e00475a:	f104 0018 	add.w	r0, r4, #24
 e00475e:	f7fd fdf5 	bl	e00234c <uxListRemove>
 e004762:	4658      	mov	r0, fp
 e004764:	f7fd fdf2 	bl	e00234c <uxListRemove>
 e004768:	6ae3      	ldr	r3, [r4, #44]	; 0x2c
 e00476a:	683a      	ldr	r2, [r7, #0]
 e00476c:	4659      	mov	r1, fp
 e00476e:	eb03 0083 	add.w	r0, r3, r3, lsl #2
 e004772:	4293      	cmp	r3, r2
 e004774:	eb09 0080 	add.w	r0, r9, r0, lsl #2
 e004778:	bf88      	it	hi
 e00477a:	603b      	strhi	r3, [r7, #0]
 e00477c:	f7fd fdc2 	bl	e002304 <vListInsertEnd>
 e004780:	f8d8 3000 	ldr.w	r3, [r8]
 e004784:	6ae2      	ldr	r2, [r4, #44]	; 0x2c
 e004786:	6adb      	ldr	r3, [r3, #44]	; 0x2c
 e004788:	429a      	cmp	r2, r3
 e00478a:	d302      	bcc.n	e004792 <xTaskResumeAll+0x76>
 e00478c:	2301      	movs	r3, #1
 e00478e:	f8ca 3000 	str.w	r3, [sl]
 e004792:	6833      	ldr	r3, [r6, #0]
 e004794:	2b00      	cmp	r3, #0
 e004796:	d1dc      	bne.n	e004752 <xTaskResumeAll+0x36>
 e004798:	b13c      	cbz	r4, e0047aa <xTaskResumeAll+0x8e>
 e00479a:	4b20      	ldr	r3, [pc, #128]	; (e00481c <xTaskResumeAll+0x100>)
 e00479c:	681a      	ldr	r2, [r3, #0]
 e00479e:	6812      	ldr	r2, [r2, #0]
 e0047a0:	bb52      	cbnz	r2, e0047f8 <xTaskResumeAll+0xdc>
 e0047a2:	4b1f      	ldr	r3, [pc, #124]	; (e004820 <xTaskResumeAll+0x104>)
 e0047a4:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
 e0047a8:	601a      	str	r2, [r3, #0]
 e0047aa:	4e1e      	ldr	r6, [pc, #120]	; (e004824 <xTaskResumeAll+0x108>)
 e0047ac:	6834      	ldr	r4, [r6, #0]
 e0047ae:	b194      	cbz	r4, e0047d6 <xTaskResumeAll+0xba>
 e0047b0:	2701      	movs	r7, #1
 e0047b2:	e006      	b.n	e0047c2 <xTaskResumeAll+0xa6>
 e0047b4:	f7ff fdd2 	bl	e00435c <xTaskIncrementTick.part.0>
 e0047b8:	b108      	cbz	r0, e0047be <xTaskResumeAll+0xa2>
 e0047ba:	f8ca 7000 	str.w	r7, [sl]
 e0047be:	3c01      	subs	r4, #1
 e0047c0:	d008      	beq.n	e0047d4 <xTaskResumeAll+0xb8>
 e0047c2:	682b      	ldr	r3, [r5, #0]
 e0047c4:	2b00      	cmp	r3, #0
 e0047c6:	d0f5      	beq.n	e0047b4 <xTaskResumeAll+0x98>
 e0047c8:	6833      	ldr	r3, [r6, #0]
 e0047ca:	3c01      	subs	r4, #1
 e0047cc:	f103 0301 	add.w	r3, r3, #1
 e0047d0:	6033      	str	r3, [r6, #0]
 e0047d2:	d1f6      	bne.n	e0047c2 <xTaskResumeAll+0xa6>
 e0047d4:	6034      	str	r4, [r6, #0]
 e0047d6:	f8da 3000 	ldr.w	r3, [sl]
 e0047da:	b93b      	cbnz	r3, e0047ec <xTaskResumeAll+0xd0>
 e0047dc:	2000      	movs	r0, #0
 e0047de:	9001      	str	r0, [sp, #4]
 e0047e0:	f7fe fc4c 	bl	e00307c <vPortExitCritical>
 e0047e4:	9801      	ldr	r0, [sp, #4]
 e0047e6:	b003      	add	sp, #12
 e0047e8:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 e0047ec:	2001      	movs	r0, #1
 e0047ee:	9001      	str	r0, [sp, #4]
 e0047f0:	f7fe fc12 	bl	e003018 <vPortYield>
 e0047f4:	9801      	ldr	r0, [sp, #4]
 e0047f6:	e7f2      	b.n	e0047de <xTaskResumeAll+0xc2>
 e0047f8:	681a      	ldr	r2, [r3, #0]
 e0047fa:	4b09      	ldr	r3, [pc, #36]	; (e004820 <xTaskResumeAll+0x104>)
 e0047fc:	68d2      	ldr	r2, [r2, #12]
 e0047fe:	6812      	ldr	r2, [r2, #0]
 e004800:	601a      	str	r2, [r3, #0]
 e004802:	e7d2      	b.n	e0047aa <xTaskResumeAll+0x8e>
 e004804:	f8df a028 	ldr.w	sl, [pc, #40]	; e004830 <xTaskResumeAll+0x114>
 e004808:	e7cf      	b.n	e0047aa <xTaskResumeAll+0x8e>
 e00480a:	bf00      	nop
 e00480c:	2000ba68 	.word	0x2000ba68
 e004810:	2000ba60 	.word	0x2000ba60
 e004814:	2000baa8 	.word	0x2000baa8
 e004818:	2000ba70 	.word	0x2000ba70
 e00481c:	2000b974 	.word	0x2000b974
 e004820:	2000ba9c 	.word	0x2000ba9c
 e004824:	2000baa4 	.word	0x2000baa4
 e004828:	2000b97c 	.word	0x2000b97c
 e00482c:	2000b970 	.word	0x2000b970
 e004830:	2000baec 	.word	0x2000baec

0e004834 <vTaskDelay>:
 e004834:	b908      	cbnz	r0, e00483a <vTaskDelay+0x6>
 e004836:	f7fe bbef 	b.w	e003018 <vPortYield>
 e00483a:	4a08      	ldr	r2, [pc, #32]	; (e00485c <vTaskDelay+0x28>)
 e00483c:	2100      	movs	r1, #0
 e00483e:	b508      	push	{r3, lr}
 e004840:	6813      	ldr	r3, [r2, #0]
 e004842:	3301      	adds	r3, #1
 e004844:	6013      	str	r3, [r2, #0]
 e004846:	f7ff fc73 	bl	e004130 <prvAddCurrentTaskToDelayedList>
 e00484a:	f7ff ff67 	bl	e00471c <xTaskResumeAll>
 e00484e:	b918      	cbnz	r0, e004858 <vTaskDelay+0x24>
 e004850:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
 e004854:	f7fe bbe0 	b.w	e003018 <vPortYield>
 e004858:	bd08      	pop	{r3, pc}
 e00485a:	bf00      	nop
 e00485c:	2000ba68 	.word	0x2000ba68

0e004860 <prvIdleTask>:
 e004860:	e92d 4ff8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e004864:	4c39      	ldr	r4, [pc, #228]	; (e00494c <prvIdleTask+0xec>)
 e004866:	4e3a      	ldr	r6, [pc, #232]	; (e004950 <prvIdleTask+0xf0>)
 e004868:	4d3a      	ldr	r5, [pc, #232]	; (e004954 <prvIdleTask+0xf4>)
 e00486a:	f8df 80f4 	ldr.w	r8, [pc, #244]	; e004960 <prvIdleTask+0x100>
 e00486e:	f8df 90f4 	ldr.w	r9, [pc, #244]	; e004964 <prvIdleTask+0x104>
 e004872:	f8df b0f4 	ldr.w	fp, [pc, #244]	; e004968 <prvIdleTask+0x108>
 e004876:	f8df a0f4 	ldr.w	sl, [pc, #244]	; e00496c <prvIdleTask+0x10c>
 e00487a:	6823      	ldr	r3, [r4, #0]
 e00487c:	b1eb      	cbz	r3, e0048ba <prvIdleTask+0x5a>
 e00487e:	f7fe fbdf 	bl	e003040 <vPortEnterCritical>
 e004882:	68f3      	ldr	r3, [r6, #12]
 e004884:	68df      	ldr	r7, [r3, #12]
 e004886:	1d38      	adds	r0, r7, #4
 e004888:	f7fd fd60 	bl	e00234c <uxListRemove>
 e00488c:	682a      	ldr	r2, [r5, #0]
 e00488e:	3a01      	subs	r2, #1
 e004890:	602a      	str	r2, [r5, #0]
 e004892:	6822      	ldr	r2, [r4, #0]
 e004894:	3a01      	subs	r2, #1
 e004896:	6022      	str	r2, [r4, #0]
 e004898:	f7fe fbf0 	bl	e00307c <vPortExitCritical>
 e00489c:	f107 0068 	add.w	r0, r7, #104	; 0x68
 e0048a0:	f002 ff8b 	bl	e0077ba <_reclaim_reent>
 e0048a4:	f897 215d 	ldrb.w	r2, [r7, #349]	; 0x15d
 e0048a8:	b36a      	cbz	r2, e004906 <prvIdleTask+0xa6>
 e0048aa:	2a01      	cmp	r2, #1
 e0048ac:	d1e5      	bne.n	e00487a <prvIdleTask+0x1a>
 e0048ae:	4638      	mov	r0, r7
 e0048b0:	f7fd fc94 	bl	e0021dc <vPortFree>
 e0048b4:	6823      	ldr	r3, [r4, #0]
 e0048b6:	2b00      	cmp	r3, #0
 e0048b8:	d1e1      	bne.n	e00487e <prvIdleTask+0x1e>
 e0048ba:	f7fe fc8f 	bl	e0031dc <vApplicationIdleHook>
 e0048be:	f8d8 3000 	ldr.w	r3, [r8]
 e0048c2:	bb3b      	cbnz	r3, e004914 <prvIdleTask+0xb4>
 e0048c4:	4b24      	ldr	r3, [pc, #144]	; (e004958 <prvIdleTask+0xf8>)
 e0048c6:	681b      	ldr	r3, [r3, #0]
 e0048c8:	6adb      	ldr	r3, [r3, #44]	; 0x2c
 e0048ca:	2b00      	cmp	r3, #0
 e0048cc:	d1d5      	bne.n	e00487a <prvIdleTask+0x1a>
 e0048ce:	f8d9 3000 	ldr.w	r3, [r9]
 e0048d2:	2b01      	cmp	r3, #1
 e0048d4:	d8d1      	bhi.n	e00487a <prvIdleTask+0x1a>
 e0048d6:	4a21      	ldr	r2, [pc, #132]	; (e00495c <prvIdleTask+0xfc>)
 e0048d8:	f8db 3000 	ldr.w	r3, [fp]
 e0048dc:	6812      	ldr	r2, [r2, #0]
 e0048de:	1a9b      	subs	r3, r3, r2
 e0048e0:	2b01      	cmp	r3, #1
 e0048e2:	d9ca      	bls.n	e00487a <prvIdleTask+0x1a>
 e0048e4:	f8da 3000 	ldr.w	r3, [sl]
 e0048e8:	3301      	adds	r3, #1
 e0048ea:	f8ca 3000 	str.w	r3, [sl]
 e0048ee:	f8d8 3000 	ldr.w	r3, [r8]
 e0048f2:	b1bb      	cbz	r3, e004924 <prvIdleTask+0xc4>
 e0048f4:	4b18      	ldr	r3, [pc, #96]	; (e004958 <prvIdleTask+0xf8>)
 e0048f6:	681b      	ldr	r3, [r3, #0]
 e0048f8:	6adb      	ldr	r3, [r3, #44]	; 0x2c
 e0048fa:	b90b      	cbnz	r3, e004900 <prvIdleTask+0xa0>
 e0048fc:	f8d9 3000 	ldr.w	r3, [r9]
 e004900:	f7ff ff0c 	bl	e00471c <xTaskResumeAll>
 e004904:	e7b9      	b.n	e00487a <prvIdleTask+0x1a>
 e004906:	6b38      	ldr	r0, [r7, #48]	; 0x30
 e004908:	f7fd fc68 	bl	e0021dc <vPortFree>
 e00490c:	4638      	mov	r0, r7
 e00490e:	f7fd fc65 	bl	e0021dc <vPortFree>
 e004912:	e7b2      	b.n	e00487a <prvIdleTask+0x1a>
 e004914:	4b10      	ldr	r3, [pc, #64]	; (e004958 <prvIdleTask+0xf8>)
 e004916:	681b      	ldr	r3, [r3, #0]
 e004918:	6adb      	ldr	r3, [r3, #44]	; 0x2c
 e00491a:	2b00      	cmp	r3, #0
 e00491c:	d1ad      	bne.n	e00487a <prvIdleTask+0x1a>
 e00491e:	f8d9 3000 	ldr.w	r3, [r9]
 e004922:	e7aa      	b.n	e00487a <prvIdleTask+0x1a>
 e004924:	4b0c      	ldr	r3, [pc, #48]	; (e004958 <prvIdleTask+0xf8>)
 e004926:	681b      	ldr	r3, [r3, #0]
 e004928:	6adb      	ldr	r3, [r3, #44]	; 0x2c
 e00492a:	2b00      	cmp	r3, #0
 e00492c:	d1e8      	bne.n	e004900 <prvIdleTask+0xa0>
 e00492e:	f8d9 3000 	ldr.w	r3, [r9]
 e004932:	2b01      	cmp	r3, #1
 e004934:	d8e4      	bhi.n	e004900 <prvIdleTask+0xa0>
 e004936:	4b09      	ldr	r3, [pc, #36]	; (e00495c <prvIdleTask+0xfc>)
 e004938:	f8db 0000 	ldr.w	r0, [fp]
 e00493c:	681b      	ldr	r3, [r3, #0]
 e00493e:	1ac0      	subs	r0, r0, r3
 e004940:	2801      	cmp	r0, #1
 e004942:	d9dd      	bls.n	e004900 <prvIdleTask+0xa0>
 e004944:	f7fe fc96 	bl	e003274 <vPortSuppressTicksAndSleep>
 e004948:	e7da      	b.n	e004900 <prvIdleTask+0xa0>
 e00494a:	bf00      	nop
 e00494c:	2000ba64 	.word	0x2000ba64
 e004950:	2000bad4 	.word	0x2000bad4
 e004954:	2000ba60 	.word	0x2000ba60
 e004958:	2000b970 	.word	0x2000b970
 e00495c:	2000bae8 	.word	0x2000bae8
 e004960:	2000ba70 	.word	0x2000ba70
 e004964:	2000b97c 	.word	0x2000b97c
 e004968:	2000ba9c 	.word	0x2000ba9c
 e00496c:	2000ba68 	.word	0x2000ba68

0e004970 <xTaskGetTickCount>:
 e004970:	4b01      	ldr	r3, [pc, #4]	; (e004978 <xTaskGetTickCount+0x8>)
 e004972:	6818      	ldr	r0, [r3, #0]
 e004974:	4770      	bx	lr
 e004976:	bf00      	nop
 e004978:	2000bae8 	.word	0x2000bae8

0e00497c <pcTaskGetName>:
 e00497c:	b108      	cbz	r0, e004982 <pcTaskGetName+0x6>
 e00497e:	3034      	adds	r0, #52	; 0x34
 e004980:	4770      	bx	lr
 e004982:	4b02      	ldr	r3, [pc, #8]	; (e00498c <pcTaskGetName+0x10>)
 e004984:	6818      	ldr	r0, [r3, #0]
 e004986:	3034      	adds	r0, #52	; 0x34
 e004988:	4770      	bx	lr
 e00498a:	bf00      	nop
 e00498c:	2000b970 	.word	0x2000b970

0e004990 <xTaskIncrementTick>:
 e004990:	4b05      	ldr	r3, [pc, #20]	; (e0049a8 <xTaskIncrementTick+0x18>)
 e004992:	681b      	ldr	r3, [r3, #0]
 e004994:	b90b      	cbnz	r3, e00499a <xTaskIncrementTick+0xa>
 e004996:	f7ff bce1 	b.w	e00435c <xTaskIncrementTick.part.0>
 e00499a:	4a04      	ldr	r2, [pc, #16]	; (e0049ac <xTaskIncrementTick+0x1c>)
 e00499c:	2000      	movs	r0, #0
 e00499e:	6813      	ldr	r3, [r2, #0]
 e0049a0:	3301      	adds	r3, #1
 e0049a2:	6013      	str	r3, [r2, #0]
 e0049a4:	4770      	bx	lr
 e0049a6:	bf00      	nop
 e0049a8:	2000ba68 	.word	0x2000ba68
 e0049ac:	2000baa4 	.word	0x2000baa4

0e0049b0 <vTaskSwitchContext>:
 e0049b0:	4b04      	ldr	r3, [pc, #16]	; (e0049c4 <vTaskSwitchContext+0x14>)
 e0049b2:	681b      	ldr	r3, [r3, #0]
 e0049b4:	b11b      	cbz	r3, e0049be <vTaskSwitchContext+0xe>
 e0049b6:	4b04      	ldr	r3, [pc, #16]	; (e0049c8 <vTaskSwitchContext+0x18>)
 e0049b8:	2201      	movs	r2, #1
 e0049ba:	601a      	str	r2, [r3, #0]
 e0049bc:	4770      	bx	lr
 e0049be:	f7ff bbef 	b.w	e0041a0 <vTaskSwitchContext.part.0>
 e0049c2:	bf00      	nop
 e0049c4:	2000ba68 	.word	0x2000ba68
 e0049c8:	2000baec 	.word	0x2000baec

0e0049cc <vTaskPlaceOnEventList>:
 e0049cc:	4b06      	ldr	r3, [pc, #24]	; (e0049e8 <vTaskPlaceOnEventList+0x1c>)
 e0049ce:	b510      	push	{r4, lr}
 e0049d0:	681b      	ldr	r3, [r3, #0]
 e0049d2:	460c      	mov	r4, r1
 e0049d4:	f103 0118 	add.w	r1, r3, #24
 e0049d8:	f7fd fca0 	bl	e00231c <vListInsert>
 e0049dc:	4620      	mov	r0, r4
 e0049de:	2101      	movs	r1, #1
 e0049e0:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
 e0049e4:	f7ff bba4 	b.w	e004130 <prvAddCurrentTaskToDelayedList>
 e0049e8:	2000b970 	.word	0x2000b970

0e0049ec <vTaskPlaceOnEventListRestricted>:
 e0049ec:	b538      	push	{r3, r4, r5, lr}
 e0049ee:	4b09      	ldr	r3, [pc, #36]	; (e004a14 <vTaskPlaceOnEventListRestricted+0x28>)
 e0049f0:	4614      	mov	r4, r2
 e0049f2:	460d      	mov	r5, r1
 e0049f4:	681b      	ldr	r3, [r3, #0]
 e0049f6:	f103 0118 	add.w	r1, r3, #24
 e0049fa:	f7fd fc83 	bl	e002304 <vListInsertEnd>
 e0049fe:	2c00      	cmp	r4, #0
 e004a00:	4621      	mov	r1, r4
 e004a02:	bf0c      	ite	eq
 e004a04:	4628      	moveq	r0, r5
 e004a06:	f04f 30ff 	movne.w	r0, #4294967295	; 0xffffffff
 e004a0a:	e8bd 4038 	ldmia.w	sp!, {r3, r4, r5, lr}
 e004a0e:	f7ff bb8f 	b.w	e004130 <prvAddCurrentTaskToDelayedList>
 e004a12:	bf00      	nop
 e004a14:	2000b970 	.word	0x2000b970

0e004a18 <xTaskRemoveFromEventList>:
 e004a18:	b530      	push	{r4, r5, lr}
 e004a1a:	68c3      	ldr	r3, [r0, #12]
 e004a1c:	b083      	sub	sp, #12
 e004a1e:	68dc      	ldr	r4, [r3, #12]
 e004a20:	f104 0518 	add.w	r5, r4, #24
 e004a24:	4628      	mov	r0, r5
 e004a26:	f7fd fc91 	bl	e00234c <uxListRemove>
 e004a2a:	4b1c      	ldr	r3, [pc, #112]	; (e004a9c <xTaskRemoveFromEventList+0x84>)
 e004a2c:	681b      	ldr	r3, [r3, #0]
 e004a2e:	b9db      	cbnz	r3, e004a68 <xTaskRemoveFromEventList+0x50>
 e004a30:	1d21      	adds	r1, r4, #4
 e004a32:	4608      	mov	r0, r1
 e004a34:	9101      	str	r1, [sp, #4]
 e004a36:	f7fd fc89 	bl	e00234c <uxListRemove>
 e004a3a:	4b19      	ldr	r3, [pc, #100]	; (e004aa0 <xTaskRemoveFromEventList+0x88>)
 e004a3c:	6ae0      	ldr	r0, [r4, #44]	; 0x2c
 e004a3e:	681a      	ldr	r2, [r3, #0]
 e004a40:	9901      	ldr	r1, [sp, #4]
 e004a42:	4290      	cmp	r0, r2
 e004a44:	bf88      	it	hi
 e004a46:	6018      	strhi	r0, [r3, #0]
 e004a48:	eb00 0080 	add.w	r0, r0, r0, lsl #2
 e004a4c:	4b15      	ldr	r3, [pc, #84]	; (e004aa4 <xTaskRemoveFromEventList+0x8c>)
 e004a4e:	eb03 0080 	add.w	r0, r3, r0, lsl #2
 e004a52:	f7fd fc57 	bl	e002304 <vListInsertEnd>
 e004a56:	4b14      	ldr	r3, [pc, #80]	; (e004aa8 <xTaskRemoveFromEventList+0x90>)
 e004a58:	681a      	ldr	r2, [r3, #0]
 e004a5a:	6812      	ldr	r2, [r2, #0]
 e004a5c:	b9ba      	cbnz	r2, e004a8e <xTaskRemoveFromEventList+0x76>
 e004a5e:	4b13      	ldr	r3, [pc, #76]	; (e004aac <xTaskRemoveFromEventList+0x94>)
 e004a60:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
 e004a64:	601a      	str	r2, [r3, #0]
 e004a66:	e003      	b.n	e004a70 <xTaskRemoveFromEventList+0x58>
 e004a68:	4629      	mov	r1, r5
 e004a6a:	4811      	ldr	r0, [pc, #68]	; (e004ab0 <xTaskRemoveFromEventList+0x98>)
 e004a6c:	f7fd fc4a 	bl	e002304 <vListInsertEnd>
 e004a70:	4b10      	ldr	r3, [pc, #64]	; (e004ab4 <xTaskRemoveFromEventList+0x9c>)
 e004a72:	6ae2      	ldr	r2, [r4, #44]	; 0x2c
 e004a74:	681b      	ldr	r3, [r3, #0]
 e004a76:	6adb      	ldr	r3, [r3, #44]	; 0x2c
 e004a78:	429a      	cmp	r2, r3
 e004a7a:	d905      	bls.n	e004a88 <xTaskRemoveFromEventList+0x70>
 e004a7c:	2301      	movs	r3, #1
 e004a7e:	4a0e      	ldr	r2, [pc, #56]	; (e004ab8 <xTaskRemoveFromEventList+0xa0>)
 e004a80:	4618      	mov	r0, r3
 e004a82:	6013      	str	r3, [r2, #0]
 e004a84:	b003      	add	sp, #12
 e004a86:	bd30      	pop	{r4, r5, pc}
 e004a88:	2000      	movs	r0, #0
 e004a8a:	b003      	add	sp, #12
 e004a8c:	bd30      	pop	{r4, r5, pc}
 e004a8e:	681a      	ldr	r2, [r3, #0]
 e004a90:	4b06      	ldr	r3, [pc, #24]	; (e004aac <xTaskRemoveFromEventList+0x94>)
 e004a92:	68d2      	ldr	r2, [r2, #12]
 e004a94:	6812      	ldr	r2, [r2, #0]
 e004a96:	601a      	str	r2, [r3, #0]
 e004a98:	e7ea      	b.n	e004a70 <xTaskRemoveFromEventList+0x58>
 e004a9a:	bf00      	nop
 e004a9c:	2000ba68 	.word	0x2000ba68
 e004aa0:	2000ba70 	.word	0x2000ba70
 e004aa4:	2000b97c 	.word	0x2000b97c
 e004aa8:	2000b974 	.word	0x2000b974
 e004aac:	2000ba9c 	.word	0x2000ba9c
 e004ab0:	2000baa8 	.word	0x2000baa8
 e004ab4:	2000b970 	.word	0x2000b970
 e004ab8:	2000baec 	.word	0x2000baec

0e004abc <vTaskInternalSetTimeOutState>:
 e004abc:	4a03      	ldr	r2, [pc, #12]	; (e004acc <vTaskInternalSetTimeOutState+0x10>)
 e004abe:	4b04      	ldr	r3, [pc, #16]	; (e004ad0 <vTaskInternalSetTimeOutState+0x14>)
 e004ac0:	6812      	ldr	r2, [r2, #0]
 e004ac2:	681b      	ldr	r3, [r3, #0]
 e004ac4:	e9c0 2300 	strd	r2, r3, [r0]
 e004ac8:	4770      	bx	lr
 e004aca:	bf00      	nop
 e004acc:	2000baa0 	.word	0x2000baa0
 e004ad0:	2000bae8 	.word	0x2000bae8

0e004ad4 <xTaskCheckForTimeOut>:
 e004ad4:	b5f0      	push	{r4, r5, r6, r7, lr}
 e004ad6:	460d      	mov	r5, r1
 e004ad8:	b083      	sub	sp, #12
 e004ada:	4604      	mov	r4, r0
 e004adc:	f7fe fab0 	bl	e003040 <vPortEnterCritical>
 e004ae0:	682b      	ldr	r3, [r5, #0]
 e004ae2:	4a13      	ldr	r2, [pc, #76]	; (e004b30 <xTaskCheckForTimeOut+0x5c>)
 e004ae4:	1c59      	adds	r1, r3, #1
 e004ae6:	6810      	ldr	r0, [r2, #0]
 e004ae8:	d01f      	beq.n	e004b2a <xTaskCheckForTimeOut+0x56>
 e004aea:	f8df c048 	ldr.w	ip, [pc, #72]	; e004b34 <xTaskCheckForTimeOut+0x60>
 e004aee:	6826      	ldr	r6, [r4, #0]
 e004af0:	f8dc 7000 	ldr.w	r7, [ip]
 e004af4:	6861      	ldr	r1, [r4, #4]
 e004af6:	42be      	cmp	r6, r7
 e004af8:	d00a      	beq.n	e004b10 <xTaskCheckForTimeOut+0x3c>
 e004afa:	4281      	cmp	r1, r0
 e004afc:	d808      	bhi.n	e004b10 <xTaskCheckForTimeOut+0x3c>
 e004afe:	2300      	movs	r3, #0
 e004b00:	2001      	movs	r0, #1
 e004b02:	602b      	str	r3, [r5, #0]
 e004b04:	9001      	str	r0, [sp, #4]
 e004b06:	f7fe fab9 	bl	e00307c <vPortExitCritical>
 e004b0a:	9801      	ldr	r0, [sp, #4]
 e004b0c:	b003      	add	sp, #12
 e004b0e:	bdf0      	pop	{r4, r5, r6, r7, pc}
 e004b10:	1a47      	subs	r7, r0, r1
 e004b12:	42bb      	cmp	r3, r7
 e004b14:	d9f3      	bls.n	e004afe <xTaskCheckForTimeOut+0x2a>
 e004b16:	1a1b      	subs	r3, r3, r0
 e004b18:	2000      	movs	r0, #0
 e004b1a:	440b      	add	r3, r1
 e004b1c:	f8dc 1000 	ldr.w	r1, [ip]
 e004b20:	6812      	ldr	r2, [r2, #0]
 e004b22:	602b      	str	r3, [r5, #0]
 e004b24:	e9c4 1200 	strd	r1, r2, [r4]
 e004b28:	e7ec      	b.n	e004b04 <xTaskCheckForTimeOut+0x30>
 e004b2a:	2000      	movs	r0, #0
 e004b2c:	e7ea      	b.n	e004b04 <xTaskCheckForTimeOut+0x30>
 e004b2e:	bf00      	nop
 e004b30:	2000bae8 	.word	0x2000bae8
 e004b34:	2000baa0 	.word	0x2000baa0

0e004b38 <vTaskMissedYield>:
 e004b38:	4b01      	ldr	r3, [pc, #4]	; (e004b40 <vTaskMissedYield+0x8>)
 e004b3a:	2201      	movs	r2, #1
 e004b3c:	601a      	str	r2, [r3, #0]
 e004b3e:	4770      	bx	lr
 e004b40:	2000baec 	.word	0x2000baec

0e004b44 <eTaskConfirmSleepModeStatus>:
 e004b44:	4b0b      	ldr	r3, [pc, #44]	; (e004b74 <eTaskConfirmSleepModeStatus+0x30>)
 e004b46:	6818      	ldr	r0, [r3, #0]
 e004b48:	b918      	cbnz	r0, e004b52 <eTaskConfirmSleepModeStatus+0xe>
 e004b4a:	4b0b      	ldr	r3, [pc, #44]	; (e004b78 <eTaskConfirmSleepModeStatus+0x34>)
 e004b4c:	681b      	ldr	r3, [r3, #0]
 e004b4e:	b113      	cbz	r3, e004b56 <eTaskConfirmSleepModeStatus+0x12>
 e004b50:	4770      	bx	lr
 e004b52:	2000      	movs	r0, #0
 e004b54:	4770      	bx	lr
 e004b56:	4a09      	ldr	r2, [pc, #36]	; (e004b7c <eTaskConfirmSleepModeStatus+0x38>)
 e004b58:	6812      	ldr	r2, [r2, #0]
 e004b5a:	b10a      	cbz	r2, e004b60 <eTaskConfirmSleepModeStatus+0x1c>
 e004b5c:	4618      	mov	r0, r3
 e004b5e:	4770      	bx	lr
 e004b60:	4a07      	ldr	r2, [pc, #28]	; (e004b80 <eTaskConfirmSleepModeStatus+0x3c>)
 e004b62:	4b08      	ldr	r3, [pc, #32]	; (e004b84 <eTaskConfirmSleepModeStatus+0x40>)
 e004b64:	6810      	ldr	r0, [r2, #0]
 e004b66:	681b      	ldr	r3, [r3, #0]
 e004b68:	3b01      	subs	r3, #1
 e004b6a:	4298      	cmp	r0, r3
 e004b6c:	bf14      	ite	ne
 e004b6e:	2001      	movne	r0, #1
 e004b70:	2002      	moveq	r0, #2
 e004b72:	4770      	bx	lr
 e004b74:	2000baa8 	.word	0x2000baa8
 e004b78:	2000baec 	.word	0x2000baec
 e004b7c:	2000baa4 	.word	0x2000baa4
 e004b80:	2000bac0 	.word	0x2000bac0
 e004b84:	2000ba60 	.word	0x2000ba60

0e004b88 <vTaskGetInfo>:
 e004b88:	b570      	push	{r4, r5, r6, lr}
 e004b8a:	460d      	mov	r5, r1
 e004b8c:	4616      	mov	r6, r2
 e004b8e:	b348      	cbz	r0, e004be4 <vTaskGetInfo+0x5c>
 e004b90:	4604      	mov	r4, r0
 e004b92:	f104 0234 	add.w	r2, r4, #52	; 0x34
 e004b96:	2b05      	cmp	r3, #5
 e004b98:	e9c5 4200 	strd	r4, r2, [r5]
 e004b9c:	6ae2      	ldr	r2, [r4, #44]	; 0x2c
 e004b9e:	612a      	str	r2, [r5, #16]
 e004ba0:	6b22      	ldr	r2, [r4, #48]	; 0x30
 e004ba2:	61ea      	str	r2, [r5, #28]
 e004ba4:	6d22      	ldr	r2, [r4, #80]	; 0x50
 e004ba6:	60aa      	str	r2, [r5, #8]
 e004ba8:	6da2      	ldr	r2, [r4, #88]	; 0x58
 e004baa:	616a      	str	r2, [r5, #20]
 e004bac:	6e62      	ldr	r2, [r4, #100]	; 0x64
 e004bae:	61aa      	str	r2, [r5, #24]
 e004bb0:	d01e      	beq.n	e004bf0 <vTaskGetInfo+0x68>
 e004bb2:	4a18      	ldr	r2, [pc, #96]	; (e004c14 <vTaskGetInfo+0x8c>)
 e004bb4:	6812      	ldr	r2, [r2, #0]
 e004bb6:	42a2      	cmp	r2, r4
 e004bb8:	d017      	beq.n	e004bea <vTaskGetInfo+0x62>
 e004bba:	2b03      	cmp	r3, #3
 e004bbc:	732b      	strb	r3, [r5, #12]
 e004bbe:	d01c      	beq.n	e004bfa <vTaskGetInfo+0x72>
 e004bc0:	b176      	cbz	r6, e004be0 <vTaskGetInfo+0x58>
 e004bc2:	6b23      	ldr	r3, [r4, #48]	; 0x30
 e004bc4:	781a      	ldrb	r2, [r3, #0]
 e004bc6:	2aa5      	cmp	r2, #165	; 0xa5
 e004bc8:	d122      	bne.n	e004c10 <vTaskGetInfo+0x88>
 e004bca:	f1c3 0001 	rsb	r0, r3, #1
 e004bce:	18c1      	adds	r1, r0, r3
 e004bd0:	f813 2f01 	ldrb.w	r2, [r3, #1]!
 e004bd4:	2aa5      	cmp	r2, #165	; 0xa5
 e004bd6:	d0fa      	beq.n	e004bce <vTaskGetInfo+0x46>
 e004bd8:	f3c1 018f 	ubfx	r1, r1, #2, #16
 e004bdc:	8429      	strh	r1, [r5, #32]
 e004bde:	bd70      	pop	{r4, r5, r6, pc}
 e004be0:	842e      	strh	r6, [r5, #32]
 e004be2:	bd70      	pop	{r4, r5, r6, pc}
 e004be4:	4a0b      	ldr	r2, [pc, #44]	; (e004c14 <vTaskGetInfo+0x8c>)
 e004be6:	6814      	ldr	r4, [r2, #0]
 e004be8:	e7d3      	b.n	e004b92 <vTaskGetInfo+0xa>
 e004bea:	2300      	movs	r3, #0
 e004bec:	732b      	strb	r3, [r5, #12]
 e004bee:	e7e7      	b.n	e004bc0 <vTaskGetInfo+0x38>
 e004bf0:	4620      	mov	r0, r4
 e004bf2:	f7ff fd05 	bl	e004600 <eTaskGetState>
 e004bf6:	7328      	strb	r0, [r5, #12]
 e004bf8:	e7e2      	b.n	e004bc0 <vTaskGetInfo+0x38>
 e004bfa:	4a07      	ldr	r2, [pc, #28]	; (e004c18 <vTaskGetInfo+0x90>)
 e004bfc:	6813      	ldr	r3, [r2, #0]
 e004bfe:	3301      	adds	r3, #1
 e004c00:	6013      	str	r3, [r2, #0]
 e004c02:	6aa3      	ldr	r3, [r4, #40]	; 0x28
 e004c04:	b10b      	cbz	r3, e004c0a <vTaskGetInfo+0x82>
 e004c06:	2302      	movs	r3, #2
 e004c08:	732b      	strb	r3, [r5, #12]
 e004c0a:	f7ff fd87 	bl	e00471c <xTaskResumeAll>
 e004c0e:	e7d7      	b.n	e004bc0 <vTaskGetInfo+0x38>
 e004c10:	2100      	movs	r1, #0
 e004c12:	e7e3      	b.n	e004bdc <vTaskGetInfo+0x54>
 e004c14:	2000b970 	.word	0x2000b970
 e004c18:	2000ba68 	.word	0x2000ba68

0e004c1c <xTaskGetCurrentTaskHandle>:
 e004c1c:	4b01      	ldr	r3, [pc, #4]	; (e004c24 <xTaskGetCurrentTaskHandle+0x8>)
 e004c1e:	6818      	ldr	r0, [r3, #0]
 e004c20:	4770      	bx	lr
 e004c22:	bf00      	nop
 e004c24:	2000b970 	.word	0x2000b970

0e004c28 <xTaskGetSchedulerState>:
 e004c28:	4b05      	ldr	r3, [pc, #20]	; (e004c40 <xTaskGetSchedulerState+0x18>)
 e004c2a:	681b      	ldr	r3, [r3, #0]
 e004c2c:	b133      	cbz	r3, e004c3c <xTaskGetSchedulerState+0x14>
 e004c2e:	4b05      	ldr	r3, [pc, #20]	; (e004c44 <xTaskGetSchedulerState+0x1c>)
 e004c30:	681b      	ldr	r3, [r3, #0]
 e004c32:	2b00      	cmp	r3, #0
 e004c34:	bf0c      	ite	eq
 e004c36:	2002      	moveq	r0, #2
 e004c38:	2000      	movne	r0, #0
 e004c3a:	4770      	bx	lr
 e004c3c:	2001      	movs	r0, #1
 e004c3e:	4770      	bx	lr
 e004c40:	2000babc 	.word	0x2000babc
 e004c44:	2000ba68 	.word	0x2000ba68

0e004c48 <xTaskPriorityInherit>:
 e004c48:	b570      	push	{r4, r5, r6, lr}
 e004c4a:	4604      	mov	r4, r0
 e004c4c:	b082      	sub	sp, #8
 e004c4e:	b1c8      	cbz	r0, e004c84 <xTaskPriorityInherit+0x3c>
 e004c50:	4d1d      	ldr	r5, [pc, #116]	; (e004cc8 <xTaskPriorityInherit+0x80>)
 e004c52:	6ac3      	ldr	r3, [r0, #44]	; 0x2c
 e004c54:	682a      	ldr	r2, [r5, #0]
 e004c56:	6ad2      	ldr	r2, [r2, #44]	; 0x2c
 e004c58:	4293      	cmp	r3, r2
 e004c5a:	d215      	bcs.n	e004c88 <xTaskPriorityInherit+0x40>
 e004c5c:	6982      	ldr	r2, [r0, #24]
 e004c5e:	2a00      	cmp	r2, #0
 e004c60:	db04      	blt.n	e004c6c <xTaskPriorityInherit+0x24>
 e004c62:	682a      	ldr	r2, [r5, #0]
 e004c64:	6ad2      	ldr	r2, [r2, #44]	; 0x2c
 e004c66:	f1c2 020b 	rsb	r2, r2, #11
 e004c6a:	6182      	str	r2, [r0, #24]
 e004c6c:	4e17      	ldr	r6, [pc, #92]	; (e004ccc <xTaskPriorityInherit+0x84>)
 e004c6e:	eb03 0383 	add.w	r3, r3, r3, lsl #2
 e004c72:	6962      	ldr	r2, [r4, #20]
 e004c74:	eb06 0383 	add.w	r3, r6, r3, lsl #2
 e004c78:	429a      	cmp	r2, r3
 e004c7a:	d00e      	beq.n	e004c9a <xTaskPriorityInherit+0x52>
 e004c7c:	682b      	ldr	r3, [r5, #0]
 e004c7e:	2001      	movs	r0, #1
 e004c80:	6adb      	ldr	r3, [r3, #44]	; 0x2c
 e004c82:	62e3      	str	r3, [r4, #44]	; 0x2c
 e004c84:	b002      	add	sp, #8
 e004c86:	bd70      	pop	{r4, r5, r6, pc}
 e004c88:	682b      	ldr	r3, [r5, #0]
 e004c8a:	6d80      	ldr	r0, [r0, #88]	; 0x58
 e004c8c:	6adb      	ldr	r3, [r3, #44]	; 0x2c
 e004c8e:	4298      	cmp	r0, r3
 e004c90:	bf2c      	ite	cs
 e004c92:	2000      	movcs	r0, #0
 e004c94:	2001      	movcc	r0, #1
 e004c96:	b002      	add	sp, #8
 e004c98:	bd70      	pop	{r4, r5, r6, pc}
 e004c9a:	1d21      	adds	r1, r4, #4
 e004c9c:	4608      	mov	r0, r1
 e004c9e:	9101      	str	r1, [sp, #4]
 e004ca0:	f7fd fb54 	bl	e00234c <uxListRemove>
 e004ca4:	682a      	ldr	r2, [r5, #0]
 e004ca6:	4b0a      	ldr	r3, [pc, #40]	; (e004cd0 <xTaskPriorityInherit+0x88>)
 e004ca8:	6ad0      	ldr	r0, [r2, #44]	; 0x2c
 e004caa:	681a      	ldr	r2, [r3, #0]
 e004cac:	62e0      	str	r0, [r4, #44]	; 0x2c
 e004cae:	4290      	cmp	r0, r2
 e004cb0:	9901      	ldr	r1, [sp, #4]
 e004cb2:	bf88      	it	hi
 e004cb4:	6018      	strhi	r0, [r3, #0]
 e004cb6:	eb00 0080 	add.w	r0, r0, r0, lsl #2
 e004cba:	eb06 0080 	add.w	r0, r6, r0, lsl #2
 e004cbe:	f7fd fb21 	bl	e002304 <vListInsertEnd>
 e004cc2:	2001      	movs	r0, #1
 e004cc4:	e7de      	b.n	e004c84 <xTaskPriorityInherit+0x3c>
 e004cc6:	bf00      	nop
 e004cc8:	2000b970 	.word	0x2000b970
 e004ccc:	2000b97c 	.word	0x2000b97c
 e004cd0:	2000ba70 	.word	0x2000ba70

0e004cd4 <xTaskPriorityDisinherit>:
 e004cd4:	b168      	cbz	r0, e004cf2 <xTaskPriorityDisinherit+0x1e>
 e004cd6:	b510      	push	{r4, lr}
 e004cd8:	6dc3      	ldr	r3, [r0, #92]	; 0x5c
 e004cda:	b082      	sub	sp, #8
 e004cdc:	6ac1      	ldr	r1, [r0, #44]	; 0x2c
 e004cde:	4604      	mov	r4, r0
 e004ce0:	6d82      	ldr	r2, [r0, #88]	; 0x58
 e004ce2:	3b01      	subs	r3, #1
 e004ce4:	4291      	cmp	r1, r2
 e004ce6:	65c3      	str	r3, [r0, #92]	; 0x5c
 e004ce8:	d000      	beq.n	e004cec <xTaskPriorityDisinherit+0x18>
 e004cea:	b123      	cbz	r3, e004cf6 <xTaskPriorityDisinherit+0x22>
 e004cec:	2000      	movs	r0, #0
 e004cee:	b002      	add	sp, #8
 e004cf0:	bd10      	pop	{r4, pc}
 e004cf2:	2000      	movs	r0, #0
 e004cf4:	4770      	bx	lr
 e004cf6:	1d01      	adds	r1, r0, #4
 e004cf8:	4608      	mov	r0, r1
 e004cfa:	9101      	str	r1, [sp, #4]
 e004cfc:	f7fd fb26 	bl	e00234c <uxListRemove>
 e004d00:	6da0      	ldr	r0, [r4, #88]	; 0x58
 e004d02:	4b0a      	ldr	r3, [pc, #40]	; (e004d2c <xTaskPriorityDisinherit+0x58>)
 e004d04:	f1c0 020b 	rsb	r2, r0, #11
 e004d08:	62e0      	str	r0, [r4, #44]	; 0x2c
 e004d0a:	9901      	ldr	r1, [sp, #4]
 e004d0c:	61a2      	str	r2, [r4, #24]
 e004d0e:	681a      	ldr	r2, [r3, #0]
 e004d10:	4290      	cmp	r0, r2
 e004d12:	bf88      	it	hi
 e004d14:	6018      	strhi	r0, [r3, #0]
 e004d16:	eb00 0080 	add.w	r0, r0, r0, lsl #2
 e004d1a:	4b05      	ldr	r3, [pc, #20]	; (e004d30 <xTaskPriorityDisinherit+0x5c>)
 e004d1c:	eb03 0080 	add.w	r0, r3, r0, lsl #2
 e004d20:	f7fd faf0 	bl	e002304 <vListInsertEnd>
 e004d24:	2001      	movs	r0, #1
 e004d26:	b002      	add	sp, #8
 e004d28:	bd10      	pop	{r4, pc}
 e004d2a:	bf00      	nop
 e004d2c:	2000ba70 	.word	0x2000ba70
 e004d30:	2000b97c 	.word	0x2000b97c

0e004d34 <vTaskPriorityDisinheritAfterTimeout>:
 e004d34:	b170      	cbz	r0, e004d54 <vTaskPriorityDisinheritAfterTimeout+0x20>
 e004d36:	b530      	push	{r4, r5, lr}
 e004d38:	6d82      	ldr	r2, [r0, #88]	; 0x58
 e004d3a:	b083      	sub	sp, #12
 e004d3c:	6ac3      	ldr	r3, [r0, #44]	; 0x2c
 e004d3e:	4604      	mov	r4, r0
 e004d40:	4291      	cmp	r1, r2
 e004d42:	bf38      	it	cc
 e004d44:	4611      	movcc	r1, r2
 e004d46:	428b      	cmp	r3, r1
 e004d48:	d002      	beq.n	e004d50 <vTaskPriorityDisinheritAfterTimeout+0x1c>
 e004d4a:	6dc2      	ldr	r2, [r0, #92]	; 0x5c
 e004d4c:	2a01      	cmp	r2, #1
 e004d4e:	d002      	beq.n	e004d56 <vTaskPriorityDisinheritAfterTimeout+0x22>
 e004d50:	b003      	add	sp, #12
 e004d52:	bd30      	pop	{r4, r5, pc}
 e004d54:	4770      	bx	lr
 e004d56:	6982      	ldr	r2, [r0, #24]
 e004d58:	62c1      	str	r1, [r0, #44]	; 0x2c
 e004d5a:	2a00      	cmp	r2, #0
 e004d5c:	db02      	blt.n	e004d64 <vTaskPriorityDisinheritAfterTimeout+0x30>
 e004d5e:	f1c1 010b 	rsb	r1, r1, #11
 e004d62:	6181      	str	r1, [r0, #24]
 e004d64:	4d0e      	ldr	r5, [pc, #56]	; (e004da0 <vTaskPriorityDisinheritAfterTimeout+0x6c>)
 e004d66:	eb03 0383 	add.w	r3, r3, r3, lsl #2
 e004d6a:	6962      	ldr	r2, [r4, #20]
 e004d6c:	eb05 0383 	add.w	r3, r5, r3, lsl #2
 e004d70:	429a      	cmp	r2, r3
 e004d72:	d1ed      	bne.n	e004d50 <vTaskPriorityDisinheritAfterTimeout+0x1c>
 e004d74:	1d21      	adds	r1, r4, #4
 e004d76:	4608      	mov	r0, r1
 e004d78:	9101      	str	r1, [sp, #4]
 e004d7a:	f7fd fae7 	bl	e00234c <uxListRemove>
 e004d7e:	4b09      	ldr	r3, [pc, #36]	; (e004da4 <vTaskPriorityDisinheritAfterTimeout+0x70>)
 e004d80:	6ae0      	ldr	r0, [r4, #44]	; 0x2c
 e004d82:	681a      	ldr	r2, [r3, #0]
 e004d84:	9901      	ldr	r1, [sp, #4]
 e004d86:	4290      	cmp	r0, r2
 e004d88:	bf88      	it	hi
 e004d8a:	6018      	strhi	r0, [r3, #0]
 e004d8c:	eb00 0080 	add.w	r0, r0, r0, lsl #2
 e004d90:	eb05 0080 	add.w	r0, r5, r0, lsl #2
 e004d94:	b003      	add	sp, #12
 e004d96:	e8bd 4030 	ldmia.w	sp!, {r4, r5, lr}
 e004d9a:	f7fd bab3 	b.w	e002304 <vListInsertEnd>
 e004d9e:	bf00      	nop
 e004da0:	2000b97c 	.word	0x2000b97c
 e004da4:	2000ba70 	.word	0x2000ba70

0e004da8 <pvTaskIncrementMutexHeldCount>:
 e004da8:	4b04      	ldr	r3, [pc, #16]	; (e004dbc <pvTaskIncrementMutexHeldCount+0x14>)
 e004daa:	681a      	ldr	r2, [r3, #0]
 e004dac:	b11a      	cbz	r2, e004db6 <pvTaskIncrementMutexHeldCount+0xe>
 e004dae:	6819      	ldr	r1, [r3, #0]
 e004db0:	6dca      	ldr	r2, [r1, #92]	; 0x5c
 e004db2:	3201      	adds	r2, #1
 e004db4:	65ca      	str	r2, [r1, #92]	; 0x5c
 e004db6:	6818      	ldr	r0, [r3, #0]
 e004db8:	4770      	bx	lr
 e004dba:	bf00      	nop
 e004dbc:	2000b970 	.word	0x2000b970

0e004dc0 <prvCheckForValidListAndQueue>:
 e004dc0:	b5f0      	push	{r4, r5, r6, r7, lr}
 e004dc2:	4c12      	ldr	r4, [pc, #72]	; (e004e0c <prvCheckForValidListAndQueue+0x4c>)
 e004dc4:	b083      	sub	sp, #12
 e004dc6:	f7fe f93b 	bl	e003040 <vPortEnterCritical>
 e004dca:	6825      	ldr	r5, [r4, #0]
 e004dcc:	b125      	cbz	r5, e004dd8 <prvCheckForValidListAndQueue+0x18>
 e004dce:	b003      	add	sp, #12
 e004dd0:	e8bd 40f0 	ldmia.w	sp!, {r4, r5, r6, r7, lr}
 e004dd4:	f7fe b952 	b.w	e00307c <vPortExitCritical>
 e004dd8:	4f0d      	ldr	r7, [pc, #52]	; (e004e10 <prvCheckForValidListAndQueue+0x50>)
 e004dda:	4e0e      	ldr	r6, [pc, #56]	; (e004e14 <prvCheckForValidListAndQueue+0x54>)
 e004ddc:	4638      	mov	r0, r7
 e004dde:	f7fd fa81 	bl	e0022e4 <vListInitialise>
 e004de2:	4630      	mov	r0, r6
 e004de4:	f7fd fa7e 	bl	e0022e4 <vListInitialise>
 e004de8:	4a0b      	ldr	r2, [pc, #44]	; (e004e18 <prvCheckForValidListAndQueue+0x58>)
 e004dea:	9500      	str	r5, [sp, #0]
 e004dec:	2110      	movs	r1, #16
 e004dee:	6017      	str	r7, [r2, #0]
 e004df0:	200a      	movs	r0, #10
 e004df2:	4a0a      	ldr	r2, [pc, #40]	; (e004e1c <prvCheckForValidListAndQueue+0x5c>)
 e004df4:	4b0a      	ldr	r3, [pc, #40]	; (e004e20 <prvCheckForValidListAndQueue+0x60>)
 e004df6:	6016      	str	r6, [r2, #0]
 e004df8:	4a0a      	ldr	r2, [pc, #40]	; (e004e24 <prvCheckForValidListAndQueue+0x64>)
 e004dfa:	f7fe fb29 	bl	e003450 <xQueueGenericCreateStatic>
 e004dfe:	6020      	str	r0, [r4, #0]
 e004e00:	b003      	add	sp, #12
 e004e02:	e8bd 40f0 	ldmia.w	sp!, {r4, r5, r6, r7, lr}
 e004e06:	f7fe b939 	b.w	e00307c <vPortExitCritical>
 e004e0a:	bf00      	nop
 e004e0c:	2000bc18 	.word	0x2000bc18
 e004e10:	2000bb98 	.word	0x2000bb98
 e004e14:	2000bbac 	.word	0x2000bbac
 e004e18:	2000baf0 	.word	0x2000baf0
 e004e1c:	2000baf4 	.word	0x2000baf4
 e004e20:	2000bbc4 	.word	0x2000bbc4
 e004e24:	2000baf8 	.word	0x2000baf8

0e004e28 <prvInsertTimerInActiveList>:
 e004e28:	4291      	cmp	r1, r2
 e004e2a:	b508      	push	{r3, lr}
 e004e2c:	6041      	str	r1, [r0, #4]
 e004e2e:	6100      	str	r0, [r0, #16]
 e004e30:	d805      	bhi.n	e004e3e <prvInsertTimerInActiveList+0x16>
 e004e32:	1ad2      	subs	r2, r2, r3
 e004e34:	6983      	ldr	r3, [r0, #24]
 e004e36:	429a      	cmp	r2, r3
 e004e38:	d30c      	bcc.n	e004e54 <prvInsertTimerInActiveList+0x2c>
 e004e3a:	2001      	movs	r0, #1
 e004e3c:	bd08      	pop	{r3, pc}
 e004e3e:	429a      	cmp	r2, r3
 e004e40:	d201      	bcs.n	e004e46 <prvInsertTimerInActiveList+0x1e>
 e004e42:	4299      	cmp	r1, r3
 e004e44:	d2f9      	bcs.n	e004e3a <prvInsertTimerInActiveList+0x12>
 e004e46:	4b07      	ldr	r3, [pc, #28]	; (e004e64 <prvInsertTimerInActiveList+0x3c>)
 e004e48:	1d01      	adds	r1, r0, #4
 e004e4a:	6818      	ldr	r0, [r3, #0]
 e004e4c:	f7fd fa66 	bl	e00231c <vListInsert>
 e004e50:	2000      	movs	r0, #0
 e004e52:	bd08      	pop	{r3, pc}
 e004e54:	4b04      	ldr	r3, [pc, #16]	; (e004e68 <prvInsertTimerInActiveList+0x40>)
 e004e56:	1d01      	adds	r1, r0, #4
 e004e58:	6818      	ldr	r0, [r3, #0]
 e004e5a:	f7fd fa5f 	bl	e00231c <vListInsert>
 e004e5e:	2000      	movs	r0, #0
 e004e60:	bd08      	pop	{r3, pc}
 e004e62:	bf00      	nop
 e004e64:	2000baf0 	.word	0x2000baf0
 e004e68:	2000baf4 	.word	0x2000baf4

0e004e6c <xTimerCreateTimerTask>:
 e004e6c:	b530      	push	{r4, r5, lr}
 e004e6e:	b089      	sub	sp, #36	; 0x24
 e004e70:	f7ff ffa6 	bl	e004dc0 <prvCheckForValidListAndQueue>
 e004e74:	4b0f      	ldr	r3, [pc, #60]	; (e004eb4 <xTimerCreateTimerTask+0x48>)
 e004e76:	6818      	ldr	r0, [r3, #0]
 e004e78:	b1c8      	cbz	r0, e004eae <xTimerCreateTimerTask+0x42>
 e004e7a:	2400      	movs	r4, #0
 e004e7c:	aa07      	add	r2, sp, #28
 e004e7e:	a906      	add	r1, sp, #24
 e004e80:	a805      	add	r0, sp, #20
 e004e82:	250a      	movs	r5, #10
 e004e84:	e9cd 4405 	strd	r4, r4, [sp, #20]
 e004e88:	f7fe f9e4 	bl	e003254 <vApplicationGetTimerTaskMemory>
 e004e8c:	4623      	mov	r3, r4
 e004e8e:	9a07      	ldr	r2, [sp, #28]
 e004e90:	9500      	str	r5, [sp, #0]
 e004e92:	e9dd 0105 	ldrd	r0, r1, [sp, #20]
 e004e96:	e9cd 1001 	strd	r1, r0, [sp, #4]
 e004e9a:	4907      	ldr	r1, [pc, #28]	; (e004eb8 <xTimerCreateTimerTask+0x4c>)
 e004e9c:	4807      	ldr	r0, [pc, #28]	; (e004ebc <xTimerCreateTimerTask+0x50>)
 e004e9e:	f7ff faf7 	bl	e004490 <xTaskCreateStatic>
 e004ea2:	4603      	mov	r3, r0
 e004ea4:	1b00      	subs	r0, r0, r4
 e004ea6:	4a06      	ldr	r2, [pc, #24]	; (e004ec0 <xTimerCreateTimerTask+0x54>)
 e004ea8:	bf18      	it	ne
 e004eaa:	2001      	movne	r0, #1
 e004eac:	6013      	str	r3, [r2, #0]
 e004eae:	b009      	add	sp, #36	; 0x24
 e004eb0:	bd30      	pop	{r4, r5, pc}
 e004eb2:	bf00      	nop
 e004eb4:	2000bc18 	.word	0x2000bc18
 e004eb8:	0e00881c 	.word	0x0e00881c
 e004ebc:	0e004fe9 	.word	0x0e004fe9
 e004ec0:	2000bc1c 	.word	0x2000bc1c

0e004ec4 <xTimerGenericCommand>:
 e004ec4:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 e004ec8:	b084      	sub	sp, #16
 e004eca:	4698      	mov	r8, r3
 e004ecc:	4606      	mov	r6, r0
 e004ece:	460d      	mov	r5, r1
 e004ed0:	4617      	mov	r7, r2
 e004ed2:	f7ff fea3 	bl	e004c1c <xTaskGetCurrentTaskHandle>
 e004ed6:	4b26      	ldr	r3, [pc, #152]	; (e004f70 <xTimerGenericCommand+0xac>)
 e004ed8:	681b      	ldr	r3, [r3, #0]
 e004eda:	4283      	cmp	r3, r0
 e004edc:	d01d      	beq.n	e004f1a <xTimerGenericCommand+0x56>
 e004ede:	4c25      	ldr	r4, [pc, #148]	; (e004f74 <xTimerGenericCommand+0xb0>)
 e004ee0:	6820      	ldr	r0, [r4, #0]
 e004ee2:	b178      	cbz	r0, e004f04 <xTimerGenericCommand+0x40>
 e004ee4:	2d05      	cmp	r5, #5
 e004ee6:	9602      	str	r6, [sp, #8]
 e004ee8:	e9cd 5700 	strd	r5, r7, [sp]
 e004eec:	dc0d      	bgt.n	e004f0a <xTimerGenericCommand+0x46>
 e004eee:	f7ff fe9b 	bl	e004c28 <xTaskGetSchedulerState>
 e004ef2:	2802      	cmp	r0, #2
 e004ef4:	f04f 0300 	mov.w	r3, #0
 e004ef8:	d024      	beq.n	e004f44 <xTimerGenericCommand+0x80>
 e004efa:	4669      	mov	r1, sp
 e004efc:	6820      	ldr	r0, [r4, #0]
 e004efe:	461a      	mov	r2, r3
 e004f00:	f7fe fb74 	bl	e0035ec <xQueueGenericSend>
 e004f04:	b004      	add	sp, #16
 e004f06:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e004f0a:	2300      	movs	r3, #0
 e004f0c:	4642      	mov	r2, r8
 e004f0e:	4669      	mov	r1, sp
 e004f10:	f7fe fc64 	bl	e0037dc <xQueueGenericSendFromISR>
 e004f14:	b004      	add	sp, #16
 e004f16:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e004f1a:	1eeb      	subs	r3, r5, #3
 e004f1c:	2b01      	cmp	r3, #1
 e004f1e:	d8de      	bhi.n	e004ede <xTimerGenericCommand+0x1a>
 e004f20:	f7ff fd26 	bl	e004970 <xTaskGetTickCount>
 e004f24:	6973      	ldr	r3, [r6, #20]
 e004f26:	4604      	mov	r4, r0
 e004f28:	b113      	cbz	r3, e004f30 <xTimerGenericCommand+0x6c>
 e004f2a:	1d30      	adds	r0, r6, #4
 e004f2c:	f7fd fa0e 	bl	e00234c <uxListRemove>
 e004f30:	2d04      	cmp	r5, #4
 e004f32:	f896 3028 	ldrb.w	r3, [r6, #40]	; 0x28
 e004f36:	d00d      	beq.n	e004f54 <xTimerGenericCommand+0x90>
 e004f38:	f023 0301 	bic.w	r3, r3, #1
 e004f3c:	2001      	movs	r0, #1
 e004f3e:	f886 3028 	strb.w	r3, [r6, #40]	; 0x28
 e004f42:	e7df      	b.n	e004f04 <xTimerGenericCommand+0x40>
 e004f44:	9a0a      	ldr	r2, [sp, #40]	; 0x28
 e004f46:	4669      	mov	r1, sp
 e004f48:	6820      	ldr	r0, [r4, #0]
 e004f4a:	f7fe fb4f 	bl	e0035ec <xQueueGenericSend>
 e004f4e:	b004      	add	sp, #16
 e004f50:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e004f54:	f043 0001 	orr.w	r0, r3, #1
 e004f58:	1939      	adds	r1, r7, r4
 e004f5a:	4623      	mov	r3, r4
 e004f5c:	4622      	mov	r2, r4
 e004f5e:	f886 0028 	strb.w	r0, [r6, #40]	; 0x28
 e004f62:	4630      	mov	r0, r6
 e004f64:	61b7      	str	r7, [r6, #24]
 e004f66:	f7ff ff5f 	bl	e004e28 <prvInsertTimerInActiveList>
 e004f6a:	2001      	movs	r0, #1
 e004f6c:	e7ca      	b.n	e004f04 <xTimerGenericCommand+0x40>
 e004f6e:	bf00      	nop
 e004f70:	2000bc1c 	.word	0x2000bc1c
 e004f74:	2000bc18 	.word	0x2000bc18

0e004f78 <prvSwitchTimerLists>:
 e004f78:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 e004f7c:	4e18      	ldr	r6, [pc, #96]	; (e004fe0 <prvSwitchTimerLists+0x68>)
 e004f7e:	b082      	sub	sp, #8
 e004f80:	f04f 0800 	mov.w	r8, #0
 e004f84:	e00d      	b.n	e004fa2 <prvSwitchTimerLists+0x2a>
 e004f86:	68db      	ldr	r3, [r3, #12]
 e004f88:	68dc      	ldr	r4, [r3, #12]
 e004f8a:	681f      	ldr	r7, [r3, #0]
 e004f8c:	1d25      	adds	r5, r4, #4
 e004f8e:	4628      	mov	r0, r5
 e004f90:	f7fd f9dc 	bl	e00234c <uxListRemove>
 e004f94:	6a23      	ldr	r3, [r4, #32]
 e004f96:	4620      	mov	r0, r4
 e004f98:	4798      	blx	r3
 e004f9a:	f894 3028 	ldrb.w	r3, [r4, #40]	; 0x28
 e004f9e:	075b      	lsls	r3, r3, #29
 e004fa0:	d40a      	bmi.n	e004fb8 <prvSwitchTimerLists+0x40>
 e004fa2:	6833      	ldr	r3, [r6, #0]
 e004fa4:	681a      	ldr	r2, [r3, #0]
 e004fa6:	2a00      	cmp	r2, #0
 e004fa8:	d1ed      	bne.n	e004f86 <prvSwitchTimerLists+0xe>
 e004faa:	4a0e      	ldr	r2, [pc, #56]	; (e004fe4 <prvSwitchTimerLists+0x6c>)
 e004fac:	6811      	ldr	r1, [r2, #0]
 e004fae:	6013      	str	r3, [r2, #0]
 e004fb0:	6031      	str	r1, [r6, #0]
 e004fb2:	b002      	add	sp, #8
 e004fb4:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e004fb8:	69a2      	ldr	r2, [r4, #24]
 e004fba:	4629      	mov	r1, r5
 e004fbc:	2300      	movs	r3, #0
 e004fbe:	4620      	mov	r0, r4
 e004fc0:	18bd      	adds	r5, r7, r2
 e004fc2:	463a      	mov	r2, r7
 e004fc4:	42af      	cmp	r7, r5
 e004fc6:	d205      	bcs.n	e004fd4 <prvSwitchTimerLists+0x5c>
 e004fc8:	6830      	ldr	r0, [r6, #0]
 e004fca:	6065      	str	r5, [r4, #4]
 e004fcc:	6124      	str	r4, [r4, #16]
 e004fce:	f7fd f9a5 	bl	e00231c <vListInsert>
 e004fd2:	e7e6      	b.n	e004fa2 <prvSwitchTimerLists+0x2a>
 e004fd4:	4619      	mov	r1, r3
 e004fd6:	f8cd 8000 	str.w	r8, [sp]
 e004fda:	f7ff ff73 	bl	e004ec4 <xTimerGenericCommand>
 e004fde:	e7e0      	b.n	e004fa2 <prvSwitchTimerLists+0x2a>
 e004fe0:	2000baf0 	.word	0x2000baf0
 e004fe4:	2000baf4 	.word	0x2000baf4

0e004fe8 <prvTimerTask>:
 e004fe8:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e004fec:	4e80      	ldr	r6, [pc, #512]	; (e0051f0 <prvTimerTask+0x208>)
 e004fee:	b087      	sub	sp, #28
 e004ff0:	4d80      	ldr	r5, [pc, #512]	; (e0051f4 <prvTimerTask+0x20c>)
 e004ff2:	f04f 0800 	mov.w	r8, #0
 e004ff6:	4c80      	ldr	r4, [pc, #512]	; (e0051f8 <prvTimerTask+0x210>)
 e004ff8:	6833      	ldr	r3, [r6, #0]
 e004ffa:	681f      	ldr	r7, [r3, #0]
 e004ffc:	2f00      	cmp	r7, #0
 e004ffe:	f000 809a 	beq.w	e005136 <prvTimerTask+0x14e>
 e005002:	68db      	ldr	r3, [r3, #12]
 e005004:	681f      	ldr	r7, [r3, #0]
 e005006:	f7ff fb81 	bl	e00470c <vTaskSuspendAll>
 e00500a:	f7ff fcb1 	bl	e004970 <xTaskGetTickCount>
 e00500e:	682b      	ldr	r3, [r5, #0]
 e005010:	4681      	mov	r9, r0
 e005012:	4298      	cmp	r0, r3
 e005014:	f0c0 8097 	bcc.w	e005146 <prvTimerTask+0x15e>
 e005018:	4287      	cmp	r7, r0
 e00501a:	6028      	str	r0, [r5, #0]
 e00501c:	f200 809d 	bhi.w	e00515a <prvTimerTask+0x172>
 e005020:	f7ff fb7c 	bl	e00471c <xTaskResumeAll>
 e005024:	6833      	ldr	r3, [r6, #0]
 e005026:	68db      	ldr	r3, [r3, #12]
 e005028:	f8d3 b00c 	ldr.w	fp, [r3, #12]
 e00502c:	f10b 0a04 	add.w	sl, fp, #4
 e005030:	4650      	mov	r0, sl
 e005032:	f7fd f98b 	bl	e00234c <uxListRemove>
 e005036:	f89b 3028 	ldrb.w	r3, [fp, #40]	; 0x28
 e00503a:	0758      	lsls	r0, r3, #29
 e00503c:	f100 80ad 	bmi.w	e00519a <prvTimerTask+0x1b2>
 e005040:	f023 0301 	bic.w	r3, r3, #1
 e005044:	f88b 3028 	strb.w	r3, [fp, #40]	; 0x28
 e005048:	f8db 3020 	ldr.w	r3, [fp, #32]
 e00504c:	4658      	mov	r0, fp
 e00504e:	4798      	blx	r3
 e005050:	2200      	movs	r2, #0
 e005052:	a902      	add	r1, sp, #8
 e005054:	6820      	ldr	r0, [r4, #0]
 e005056:	f7fe fc55 	bl	e003904 <xQueueReceive>
 e00505a:	2800      	cmp	r0, #0
 e00505c:	d0cc      	beq.n	e004ff8 <prvTimerTask+0x10>
 e00505e:	9b02      	ldr	r3, [sp, #8]
 e005060:	9804      	ldr	r0, [sp, #16]
 e005062:	2b00      	cmp	r3, #0
 e005064:	db60      	blt.n	e005128 <prvTimerTask+0x140>
 e005066:	9f04      	ldr	r7, [sp, #16]
 e005068:	697b      	ldr	r3, [r7, #20]
 e00506a:	b113      	cbz	r3, e005072 <prvTimerTask+0x8a>
 e00506c:	1d38      	adds	r0, r7, #4
 e00506e:	f7fd f96d 	bl	e00234c <uxListRemove>
 e005072:	f7ff fc7d 	bl	e004970 <xTaskGetTickCount>
 e005076:	682b      	ldr	r3, [r5, #0]
 e005078:	4681      	mov	r9, r0
 e00507a:	4298      	cmp	r0, r3
 e00507c:	d36a      	bcc.n	e005154 <prvTimerTask+0x16c>
 e00507e:	9b02      	ldr	r3, [sp, #8]
 e005080:	f8c5 9000 	str.w	r9, [r5]
 e005084:	2b09      	cmp	r3, #9
 e005086:	d8e3      	bhi.n	e005050 <prvTimerTask+0x68>
 e005088:	e8df f003 	tbb	[pc, r3]
 e00508c:	3e050505 	.word	0x3e050505
 e005090:	0505452c 	.word	0x0505452c
 e005094:	2c3e      	.short	0x2c3e
 e005096:	f897 2028 	ldrb.w	r2, [r7, #40]	; 0x28
 e00509a:	9b03      	ldr	r3, [sp, #12]
 e00509c:	69b9      	ldr	r1, [r7, #24]
 e00509e:	f042 0201 	orr.w	r2, r2, #1
 e0050a2:	613f      	str	r7, [r7, #16]
 e0050a4:	1858      	adds	r0, r3, r1
 e0050a6:	f887 2028 	strb.w	r2, [r7, #40]	; 0x28
 e0050aa:	bf2c      	ite	cs
 e0050ac:	2201      	movcs	r2, #1
 e0050ae:	2200      	movcc	r2, #0
 e0050b0:	4548      	cmp	r0, r9
 e0050b2:	6078      	str	r0, [r7, #4]
 e0050b4:	d868      	bhi.n	e005188 <prvTimerTask+0x1a0>
 e0050b6:	eba9 0303 	sub.w	r3, r9, r3
 e0050ba:	4299      	cmp	r1, r3
 e0050bc:	f200 808c 	bhi.w	e0051d8 <prvTimerTask+0x1f0>
 e0050c0:	6a3b      	ldr	r3, [r7, #32]
 e0050c2:	4638      	mov	r0, r7
 e0050c4:	4798      	blx	r3
 e0050c6:	f897 3028 	ldrb.w	r3, [r7, #40]	; 0x28
 e0050ca:	0759      	lsls	r1, r3, #29
 e0050cc:	d5c0      	bpl.n	e005050 <prvTimerTask+0x68>
 e0050ce:	69ba      	ldr	r2, [r7, #24]
 e0050d0:	2300      	movs	r3, #0
 e0050d2:	9903      	ldr	r1, [sp, #12]
 e0050d4:	4638      	mov	r0, r7
 e0050d6:	f8cd 8000 	str.w	r8, [sp]
 e0050da:	440a      	add	r2, r1
 e0050dc:	4619      	mov	r1, r3
 e0050de:	f7ff fef1 	bl	e004ec4 <xTimerGenericCommand>
 e0050e2:	e7b5      	b.n	e005050 <prvTimerTask+0x68>
 e0050e4:	f897 1028 	ldrb.w	r1, [r7, #40]	; 0x28
 e0050e8:	464b      	mov	r3, r9
 e0050ea:	f8dd c00c 	ldr.w	ip, [sp, #12]
 e0050ee:	464a      	mov	r2, r9
 e0050f0:	f041 0001 	orr.w	r0, r1, #1
 e0050f4:	f8c7 c018 	str.w	ip, [r7, #24]
 e0050f8:	eb0c 0109 	add.w	r1, ip, r9
 e0050fc:	f887 0028 	strb.w	r0, [r7, #40]	; 0x28
 e005100:	4638      	mov	r0, r7
 e005102:	f7ff fe91 	bl	e004e28 <prvInsertTimerInActiveList>
 e005106:	e7a3      	b.n	e005050 <prvTimerTask+0x68>
 e005108:	f897 3028 	ldrb.w	r3, [r7, #40]	; 0x28
 e00510c:	f023 0301 	bic.w	r3, r3, #1
 e005110:	f887 3028 	strb.w	r3, [r7, #40]	; 0x28
 e005114:	e79c      	b.n	e005050 <prvTimerTask+0x68>
 e005116:	f897 3028 	ldrb.w	r3, [r7, #40]	; 0x28
 e00511a:	079a      	lsls	r2, r3, #30
 e00511c:	d553      	bpl.n	e0051c6 <prvTimerTask+0x1de>
 e00511e:	f023 0301 	bic.w	r3, r3, #1
 e005122:	f887 3028 	strb.w	r3, [r7, #40]	; 0x28
 e005126:	e793      	b.n	e005050 <prvTimerTask+0x68>
 e005128:	9b03      	ldr	r3, [sp, #12]
 e00512a:	9905      	ldr	r1, [sp, #20]
 e00512c:	4798      	blx	r3
 e00512e:	9b02      	ldr	r3, [sp, #8]
 e005130:	2b00      	cmp	r3, #0
 e005132:	db8d      	blt.n	e005050 <prvTimerTask+0x68>
 e005134:	e797      	b.n	e005066 <prvTimerTask+0x7e>
 e005136:	f7ff fae9 	bl	e00470c <vTaskSuspendAll>
 e00513a:	f7ff fc19 	bl	e004970 <xTaskGetTickCount>
 e00513e:	682b      	ldr	r3, [r5, #0]
 e005140:	4681      	mov	r9, r0
 e005142:	4298      	cmp	r0, r3
 e005144:	d217      	bcs.n	e005176 <prvTimerTask+0x18e>
 e005146:	f7ff ff17 	bl	e004f78 <prvSwitchTimerLists>
 e00514a:	f8c5 9000 	str.w	r9, [r5]
 e00514e:	f7ff fae5 	bl	e00471c <xTaskResumeAll>
 e005152:	e77d      	b.n	e005050 <prvTimerTask+0x68>
 e005154:	f7ff ff10 	bl	e004f78 <prvSwitchTimerLists>
 e005158:	e791      	b.n	e00507e <prvTimerTask+0x96>
 e00515a:	2200      	movs	r2, #0
 e00515c:	eba7 0109 	sub.w	r1, r7, r9
 e005160:	6820      	ldr	r0, [r4, #0]
 e005162:	f7fe fd35 	bl	e003bd0 <vQueueWaitForMessageRestricted>
 e005166:	f7ff fad9 	bl	e00471c <xTaskResumeAll>
 e00516a:	2800      	cmp	r0, #0
 e00516c:	f47f af70 	bne.w	e005050 <prvTimerTask+0x68>
 e005170:	f7fd ff52 	bl	e003018 <vPortYield>
 e005174:	e76c      	b.n	e005050 <prvTimerTask+0x68>
 e005176:	4b21      	ldr	r3, [pc, #132]	; (e0051fc <prvTimerTask+0x214>)
 e005178:	f8c5 9000 	str.w	r9, [r5]
 e00517c:	681b      	ldr	r3, [r3, #0]
 e00517e:	681a      	ldr	r2, [r3, #0]
 e005180:	fab2 f282 	clz	r2, r2
 e005184:	0952      	lsrs	r2, r2, #5
 e005186:	e7e9      	b.n	e00515c <prvTimerTask+0x174>
 e005188:	454b      	cmp	r3, r9
 e00518a:	d901      	bls.n	e005190 <prvTimerTask+0x1a8>
 e00518c:	2a00      	cmp	r2, #0
 e00518e:	d097      	beq.n	e0050c0 <prvTimerTask+0xd8>
 e005190:	1d39      	adds	r1, r7, #4
 e005192:	6830      	ldr	r0, [r6, #0]
 e005194:	f7fd f8c2 	bl	e00231c <vListInsert>
 e005198:	e75a      	b.n	e005050 <prvTimerTask+0x68>
 e00519a:	f8db 2018 	ldr.w	r2, [fp, #24]
 e00519e:	f8cb b010 	str.w	fp, [fp, #16]
 e0051a2:	18bb      	adds	r3, r7, r2
 e0051a4:	4599      	cmp	r9, r3
 e0051a6:	f8cb 3004 	str.w	r3, [fp, #4]
 e0051aa:	d310      	bcc.n	e0051ce <prvTimerTask+0x1e6>
 e0051ac:	eba9 0907 	sub.w	r9, r9, r7
 e0051b0:	454a      	cmp	r2, r9
 e0051b2:	d817      	bhi.n	e0051e4 <prvTimerTask+0x1fc>
 e0051b4:	2300      	movs	r3, #0
 e0051b6:	463a      	mov	r2, r7
 e0051b8:	4658      	mov	r0, fp
 e0051ba:	f8cd 8000 	str.w	r8, [sp]
 e0051be:	4619      	mov	r1, r3
 e0051c0:	f7ff fe80 	bl	e004ec4 <xTimerGenericCommand>
 e0051c4:	e740      	b.n	e005048 <prvTimerTask+0x60>
 e0051c6:	4638      	mov	r0, r7
 e0051c8:	f7fd f808 	bl	e0021dc <vPortFree>
 e0051cc:	e740      	b.n	e005050 <prvTimerTask+0x68>
 e0051ce:	4651      	mov	r1, sl
 e0051d0:	6830      	ldr	r0, [r6, #0]
 e0051d2:	f7fd f8a3 	bl	e00231c <vListInsert>
 e0051d6:	e737      	b.n	e005048 <prvTimerTask+0x60>
 e0051d8:	4b08      	ldr	r3, [pc, #32]	; (e0051fc <prvTimerTask+0x214>)
 e0051da:	1d39      	adds	r1, r7, #4
 e0051dc:	6818      	ldr	r0, [r3, #0]
 e0051de:	f7fd f89d 	bl	e00231c <vListInsert>
 e0051e2:	e735      	b.n	e005050 <prvTimerTask+0x68>
 e0051e4:	4b05      	ldr	r3, [pc, #20]	; (e0051fc <prvTimerTask+0x214>)
 e0051e6:	4651      	mov	r1, sl
 e0051e8:	6818      	ldr	r0, [r3, #0]
 e0051ea:	f7fd f897 	bl	e00231c <vListInsert>
 e0051ee:	e72b      	b.n	e005048 <prvTimerTask+0x60>
 e0051f0:	2000baf0 	.word	0x2000baf0
 e0051f4:	2000bbc0 	.word	0x2000bbc0
 e0051f8:	2000bc18 	.word	0x2000bc18
 e0051fc:	2000baf4 	.word	0x2000baf4

0e005200 <wifi_fast_connect_enable>:
 e005200:	2300      	movs	r3, #0
 e005202:	4902      	ldr	r1, [pc, #8]	; (e00520c <wifi_fast_connect_enable+0xc>)
 e005204:	4a02      	ldr	r2, [pc, #8]	; (e005210 <wifi_fast_connect_enable+0x10>)
 e005206:	600b      	str	r3, [r1, #0]
 e005208:	6013      	str	r3, [r2, #0]
 e00520a:	4770      	bx	lr
 e00520c:	2000be08 	.word	0x2000be08
 e005210:	2000be04 	.word	0x2000be04

0e005214 <wifi_indication>:
 e005214:	280b      	cmp	r0, #11
 e005216:	4684      	mov	ip, r0
 e005218:	b5f0      	push	{r4, r5, r6, r7, lr}
 e00521a:	461d      	mov	r5, r3
 e00521c:	b083      	sub	sp, #12
 e00521e:	d01c      	beq.n	e00525a <wifi_indication+0x46>
 e005220:	2816      	cmp	r0, #22
 e005222:	dc22      	bgt.n	e00526a <wifi_indication+0x56>
 e005224:	4c12      	ldr	r4, [pc, #72]	; (e005270 <wifi_indication+0x5c>)
 e005226:	ea4f 160c 	mov.w	r6, ip, lsl #4
 e00522a:	4608      	mov	r0, r1
 e00522c:	4611      	mov	r1, r2
 e00522e:	59a7      	ldr	r7, [r4, r6]
 e005230:	eb04 1c0c 	add.w	ip, r4, ip, lsl #4
 e005234:	b13f      	cbz	r7, e005246 <wifi_indication+0x32>
 e005236:	f8dc 3004 	ldr.w	r3, [ip, #4]
 e00523a:	462a      	mov	r2, r5
 e00523c:	e9cd 0100 	strd	r0, r1, [sp]
 e005240:	47b8      	blx	r7
 e005242:	e9dd 0100 	ldrd	r0, r1, [sp]
 e005246:	4434      	add	r4, r6
 e005248:	68a6      	ldr	r6, [r4, #8]
 e00524a:	b176      	cbz	r6, e00526a <wifi_indication+0x56>
 e00524c:	68e3      	ldr	r3, [r4, #12]
 e00524e:	462a      	mov	r2, r5
 e005250:	46b4      	mov	ip, r6
 e005252:	b003      	add	sp, #12
 e005254:	e8bd 40f0 	ldmia.w	sp!, {r4, r5, r6, r7, lr}
 e005258:	4760      	bx	ip
 e00525a:	4b06      	ldr	r3, [pc, #24]	; (e005274 <wifi_indication+0x60>)
 e00525c:	681b      	ldr	r3, [r3, #0]
 e00525e:	b123      	cbz	r3, e00526a <wifi_indication+0x56>
 e005260:	b2e8      	uxtb	r0, r5
 e005262:	b003      	add	sp, #12
 e005264:	e8bd 40f0 	ldmia.w	sp!, {r4, r5, r6, r7, lr}
 e005268:	4718      	bx	r3
 e00526a:	b003      	add	sp, #12
 e00526c:	bdf0      	pop	{r4, r5, r6, r7, pc}
 e00526e:	bf00      	nop
 e005270:	2000bc20 	.word	0x2000bc20
 e005274:	2000be0c 	.word	0x2000be0c

0e005278 <_init_thread>:
 e005278:	b510      	push	{r4, lr}
 e00527a:	4c14      	ldr	r4, [pc, #80]	; (e0052cc <_init_thread+0x54>)
 e00527c:	b084      	sub	sp, #16
 e00527e:	6f63      	ldr	r3, [r4, #116]	; 0x74
 e005280:	2b00      	cmp	r3, #0
 e005282:	db05      	blt.n	e005290 <_init_thread+0x18>
 e005284:	2001      	movs	r0, #1
 e005286:	f7fd fe03 	bl	e002e90 <rtos_time_delay_ms>
 e00528a:	6f63      	ldr	r3, [r4, #116]	; 0x74
 e00528c:	2b00      	cmp	r3, #0
 e00528e:	daf9      	bge.n	e005284 <_init_thread+0xc>
 e005290:	4a0e      	ldr	r2, [pc, #56]	; (e0052cc <_init_thread+0x54>)
 e005292:	2001      	movs	r0, #1
 e005294:	4c0e      	ldr	r4, [pc, #56]	; (e0052d0 <_init_thread+0x58>)
 e005296:	6f53      	ldr	r3, [r2, #116]	; 0x74
 e005298:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
 e00529c:	6753      	str	r3, [r2, #116]	; 0x74
 e00529e:	f000 fc29 	bl	e005af4 <wifi_on>
 e0052a2:	2001      	movs	r0, #1
 e0052a4:	f000 fc7a 	bl	e005b9c <wifi_config_autoreconnect>
 e0052a8:	f7fd fc30 	bl	e002b0c <rtos_mem_get_free_heap_size>
 e0052ac:	2135      	movs	r1, #53	; 0x35
 e0052ae:	4b09      	ldr	r3, [pc, #36]	; (e0052d4 <_init_thread+0x5c>)
 e0052b0:	2249      	movs	r2, #73	; 0x49
 e0052b2:	9400      	str	r4, [sp, #0]
 e0052b4:	e9cd 1001 	strd	r1, r0, [sp, #4]
 e0052b8:	2004      	movs	r0, #4
 e0052ba:	4907      	ldr	r1, [pc, #28]	; (e0052d8 <_init_thread+0x60>)
 e0052bc:	f7fd f8be 	bl	e00243c <rtk_log_write>
 e0052c0:	2000      	movs	r0, #0
 e0052c2:	b004      	add	sp, #16
 e0052c4:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
 e0052c8:	f7fd bdda 	b.w	e002e80 <rtos_task_delete>
 e0052cc:	41008000 	.word	0x41008000
 e0052d0:	0e00888c 	.word	0x0e00888c
 e0052d4:	0e008824 	.word	0x0e008824
 e0052d8:	0e008840 	.word	0x0e008840

0e0052dc <wlan_initialize>:
 e0052dc:	b530      	push	{r4, r5, lr}
 e0052de:	b083      	sub	sp, #12
 e0052e0:	2402      	movs	r4, #2
 e0052e2:	f001 f807 	bl	e0062f4 <inic_host_init>
 e0052e6:	2001      	movs	r0, #1
 e0052e8:	f7ff ff8a 	bl	e005200 <wifi_fast_connect_enable>
 e0052ec:	f44f 50a0 	mov.w	r0, #5120	; 0x1400
 e0052f0:	2300      	movs	r3, #0
 e0052f2:	4a0a      	ldr	r2, [pc, #40]	; (e00531c <wlan_initialize+0x40>)
 e0052f4:	490a      	ldr	r1, [pc, #40]	; (e005320 <wlan_initialize+0x44>)
 e0052f6:	e9cd 0400 	strd	r0, r4, [sp]
 e0052fa:	4618      	mov	r0, r3
 e0052fc:	f7fd fdac 	bl	e002e58 <rtos_task_create>
 e005300:	b908      	cbnz	r0, e005306 <wlan_initialize+0x2a>
 e005302:	b003      	add	sp, #12
 e005304:	bd30      	pop	{r4, r5, pc}
 e005306:	4d07      	ldr	r5, [pc, #28]	; (e005324 <wlan_initialize+0x48>)
 e005308:	2245      	movs	r2, #69	; 0x45
 e00530a:	4b07      	ldr	r3, [pc, #28]	; (e005328 <wlan_initialize+0x4c>)
 e00530c:	4620      	mov	r0, r4
 e00530e:	4907      	ldr	r1, [pc, #28]	; (e00532c <wlan_initialize+0x50>)
 e005310:	9500      	str	r5, [sp, #0]
 e005312:	f7fd f893 	bl	e00243c <rtk_log_write>
 e005316:	b003      	add	sp, #12
 e005318:	bd30      	pop	{r4, r5, pc}
 e00531a:	bf00      	nop
 e00531c:	0e005279 	.word	0x0e005279
 e005320:	0e008848 	.word	0x0e008848
 e005324:	0e00887c 	.word	0x0e00887c
 e005328:	0e008850 	.word	0x0e008850
 e00532c:	0e008840 	.word	0x0e008840

0e005330 <vTaskStatus>:
 e005330:	b530      	push	{r4, r5, lr}
 e005332:	b08d      	sub	sp, #52	; 0x34
 e005334:	4d21      	ldr	r5, [pc, #132]	; (e0053bc <vTaskStatus+0x8c>)
 e005336:	f7fd fe83 	bl	e003040 <vPortEnterCritical>
 e00533a:	f7ff fc6f 	bl	e004c1c <xTaskGetCurrentTaskHandle>
 e00533e:	2305      	movs	r3, #5
 e005340:	2201      	movs	r2, #1
 e005342:	a903      	add	r1, sp, #12
 e005344:	4604      	mov	r4, r0
 e005346:	f7ff fc1f 	bl	e004b88 <vTaskGetInfo>
 e00534a:	4628      	mov	r0, r5
 e00534c:	f001 fe5c 	bl	e007008 <DiagPrintf>
 e005350:	9904      	ldr	r1, [sp, #16]
 e005352:	481b      	ldr	r0, [pc, #108]	; (e0053c0 <vTaskStatus+0x90>)
 e005354:	f001 fe58 	bl	e007008 <DiagPrintf>
 e005358:	9905      	ldr	r1, [sp, #20]
 e00535a:	481a      	ldr	r0, [pc, #104]	; (e0053c4 <vTaskStatus+0x94>)
 e00535c:	f001 fe54 	bl	e007008 <DiagPrintf>
 e005360:	4621      	mov	r1, r4
 e005362:	4819      	ldr	r0, [pc, #100]	; (e0053c8 <vTaskStatus+0x98>)
 e005364:	f001 fe50 	bl	e007008 <DiagPrintf>
 e005368:	4b18      	ldr	r3, [pc, #96]	; (e0053cc <vTaskStatus+0x9c>)
 e00536a:	f89d 1018 	ldrb.w	r1, [sp, #24]
 e00536e:	4818      	ldr	r0, [pc, #96]	; (e0053d0 <vTaskStatus+0xa0>)
 e005370:	f853 2021 	ldr.w	r2, [r3, r1, lsl #2]
 e005374:	f001 fe48 	bl	e007008 <DiagPrintf>
 e005378:	9908      	ldr	r1, [sp, #32]
 e00537a:	4816      	ldr	r0, [pc, #88]	; (e0053d4 <vTaskStatus+0xa4>)
 e00537c:	f001 fe44 	bl	e007008 <DiagPrintf>
 e005380:	9907      	ldr	r1, [sp, #28]
 e005382:	4815      	ldr	r0, [pc, #84]	; (e0053d8 <vTaskStatus+0xa8>)
 e005384:	f001 fe40 	bl	e007008 <DiagPrintf>
 e005388:	9909      	ldr	r1, [sp, #36]	; 0x24
 e00538a:	4814      	ldr	r0, [pc, #80]	; (e0053dc <vTaskStatus+0xac>)
 e00538c:	f001 fe3c 	bl	e007008 <DiagPrintf>
 e005390:	6ce3      	ldr	r3, [r4, #76]	; 0x4c
 e005392:	6b22      	ldr	r2, [r4, #48]	; 0x30
 e005394:	6821      	ldr	r1, [r4, #0]
 e005396:	1a9a      	subs	r2, r3, r2
 e005398:	4811      	ldr	r0, [pc, #68]	; (e0053e0 <vTaskStatus+0xb0>)
 e00539a:	1092      	asrs	r2, r2, #2
 e00539c:	3201      	adds	r2, #1
 e00539e:	9200      	str	r2, [sp, #0]
 e0053a0:	9a0a      	ldr	r2, [sp, #40]	; 0x28
 e0053a2:	f001 fe31 	bl	e007008 <DiagPrintf>
 e0053a6:	f8bd 102c 	ldrh.w	r1, [sp, #44]	; 0x2c
 e0053aa:	480e      	ldr	r0, [pc, #56]	; (e0053e4 <vTaskStatus+0xb4>)
 e0053ac:	f001 fe2c 	bl	e007008 <DiagPrintf>
 e0053b0:	4628      	mov	r0, r5
 e0053b2:	f001 fe29 	bl	e007008 <DiagPrintf>
 e0053b6:	b00d      	add	sp, #52	; 0x34
 e0053b8:	bd30      	pop	{r4, r5, pc}
 e0053ba:	bf00      	nop
 e0053bc:	0e00889c 	.word	0x0e00889c
 e0053c0:	0e0088c0 	.word	0x0e0088c0
 e0053c4:	0e0088d4 	.word	0x0e0088d4
 e0053c8:	0e0088e4 	.word	0x0e0088e4
 e0053cc:	0e008a08 	.word	0x0e008a08
 e0053d0:	0e0088f8 	.word	0x0e0088f8
 e0053d4:	0e008914 	.word	0x0e008914
 e0053d8:	0e008928 	.word	0x0e008928
 e0053dc:	0e008940 	.word	0x0e008940
 e0053e0:	0e008958 	.word	0x0e008958
 e0053e4:	0e0089a8 	.word	0x0e0089a8

0e0053e8 <vTaskCrashCallback>:
 e0053e8:	b510      	push	{r4, lr}
 e0053ea:	4604      	mov	r4, r0
 e0053ec:	f7fd fd22 	bl	e002e34 <rtos_sched_get_state>
 e0053f0:	2801      	cmp	r0, #1
 e0053f2:	d002      	beq.n	e0053fa <vTaskCrashCallback+0x12>
 e0053f4:	6823      	ldr	r3, [r4, #0]
 e0053f6:	075b      	lsls	r3, r3, #29
 e0053f8:	d400      	bmi.n	e0053fc <vTaskCrashCallback+0x14>
 e0053fa:	bd10      	pop	{r4, pc}
 e0053fc:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
 e005400:	f7ff bf96 	b.w	e005330 <vTaskStatus>

0e005404 <SOCPS_KM0WKM4_ipc_int>:
 e005404:	b530      	push	{r4, r5, lr}
 e005406:	2101      	movs	r1, #1
 e005408:	b083      	sub	sp, #12
 e00540a:	2000      	movs	r0, #0
 e00540c:	4d12      	ldr	r5, [pc, #72]	; (e005458 <SOCPS_KM0WKM4_ipc_int+0x54>)
 e00540e:	f7fc fb2d 	bl	e001a6c <ipc_get_message>
 e005412:	4b12      	ldr	r3, [pc, #72]	; (e00545c <SOCPS_KM0WKM4_ipc_int+0x58>)
 e005414:	4604      	mov	r4, r0
 e005416:	2249      	movs	r2, #73	; 0x49
 e005418:	f103 0108 	add.w	r1, r3, #8
 e00541c:	2004      	movs	r0, #4
 e00541e:	9500      	str	r5, [sp, #0]
 e005420:	f7fd f80c 	bl	e00243c <rtk_log_write>
 e005424:	6863      	ldr	r3, [r4, #4]
 e005426:	b11b      	cbz	r3, e005430 <SOCPS_KM0WKM4_ipc_int+0x2c>
 e005428:	2b01      	cmp	r3, #1
 e00542a:	d00a      	beq.n	e005442 <SOCPS_KM0WKM4_ipc_int+0x3e>
 e00542c:	b003      	add	sp, #12
 e00542e:	bd30      	pop	{r4, r5, pc}
 e005430:	4b0b      	ldr	r3, [pc, #44]	; (e005460 <SOCPS_KM0WKM4_ipc_int+0x5c>)
 e005432:	2249      	movs	r2, #73	; 0x49
 e005434:	2004      	movs	r0, #4
 e005436:	1f19      	subs	r1, r3, #4
 e005438:	b003      	add	sp, #12
 e00543a:	e8bd 4030 	ldmia.w	sp!, {r4, r5, lr}
 e00543e:	f7fc bffd 	b.w	e00243c <rtk_log_write>
 e005442:	4b08      	ldr	r3, [pc, #32]	; (e005464 <SOCPS_KM0WKM4_ipc_int+0x60>)
 e005444:	2249      	movs	r2, #73	; 0x49
 e005446:	2004      	movs	r0, #4
 e005448:	f1a3 011c 	sub.w	r1, r3, #28
 e00544c:	b003      	add	sp, #12
 e00544e:	e8bd 4030 	ldmia.w	sp!, {r4, r5, lr}
 e005452:	f7fc bff3 	b.w	e00243c <rtk_log_write>
 e005456:	bf00      	nop
 e005458:	0e008aa4 	.word	0x0e008aa4
 e00545c:	0e008a2c 	.word	0x0e008a2c
 e005460:	0e008a38 	.word	0x0e008a38
 e005464:	0e008a50 	.word	0x0e008a50

0e005468 <crash_dump_memory>:
 e005468:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 e00546c:	460e      	mov	r6, r1
 e00546e:	4680      	mov	r8, r0
 e005470:	4b21      	ldr	r3, [pc, #132]	; (e0054f8 <crash_dump_memory+0x90>)
 e005472:	f1a6 0080 	sub.w	r0, r6, #128	; 0x80
 e005476:	4a21      	ldr	r2, [pc, #132]	; (e0054fc <crash_dump_memory+0x94>)
 e005478:	f8d3 1228 	ldr.w	r1, [r3, #552]	; 0x228
 e00547c:	4644      	mov	r4, r8
 e00547e:	f5b0 5f7e 	cmp.w	r0, #16256	; 0x3f80
 e005482:	4b1f      	ldr	r3, [pc, #124]	; (e005500 <crash_dump_memory+0x98>)
 e005484:	481f      	ldr	r0, [pc, #124]	; (e005504 <crash_dump_memory+0x9c>)
 e005486:	bf88      	it	hi
 e005488:	2680      	movhi	r6, #128	; 0x80
 e00548a:	f011 0fc0 	tst.w	r1, #192	; 0xc0
 e00548e:	4f1e      	ldr	r7, [pc, #120]	; (e005508 <crash_dump_memory+0xa0>)
 e005490:	bf14      	ite	ne
 e005492:	4611      	movne	r1, r2
 e005494:	4619      	moveq	r1, r3
 e005496:	f001 fdb7 	bl	e007008 <DiagPrintf>
 e00549a:	4632      	mov	r2, r6
 e00549c:	4641      	mov	r1, r8
 e00549e:	481b      	ldr	r0, [pc, #108]	; (e00550c <crash_dump_memory+0xa4>)
 e0054a0:	f001 fdb2 	bl	e007008 <DiagPrintf>
 e0054a4:	f8df 906c 	ldr.w	r9, [pc, #108]	; e005514 <crash_dump_memory+0xac>
 e0054a8:	2300      	movs	r3, #0
 e0054aa:	f8df a06c 	ldr.w	sl, [pc, #108]	; e005518 <crash_dump_memory+0xb0>
 e0054ae:	e007      	b.n	e0054c0 <crash_dump_memory+0x58>
 e0054b0:	6821      	ldr	r1, [r4, #0]
 e0054b2:	4638      	mov	r0, r7
 e0054b4:	f001 fda8 	bl	e007008 <DiagPrintf>
 e0054b8:	42ae      	cmp	r6, r5
 e0054ba:	d00f      	beq.n	e0054dc <crash_dump_memory+0x74>
 e0054bc:	3404      	adds	r4, #4
 e0054be:	462b      	mov	r3, r5
 e0054c0:	1c5d      	adds	r5, r3, #1
 e0054c2:	b183      	cbz	r3, e0054e6 <crash_dump_memory+0x7e>
 e0054c4:	079b      	lsls	r3, r3, #30
 e0054c6:	d1f3      	bne.n	e0054b0 <crash_dump_memory+0x48>
 e0054c8:	4621      	mov	r1, r4
 e0054ca:	4648      	mov	r0, r9
 e0054cc:	f001 fd9c 	bl	e007008 <DiagPrintf>
 e0054d0:	6821      	ldr	r1, [r4, #0]
 e0054d2:	4638      	mov	r0, r7
 e0054d4:	f001 fd98 	bl	e007008 <DiagPrintf>
 e0054d8:	42ae      	cmp	r6, r5
 e0054da:	d1ef      	bne.n	e0054bc <crash_dump_memory+0x54>
 e0054dc:	480c      	ldr	r0, [pc, #48]	; (e005510 <crash_dump_memory+0xa8>)
 e0054de:	e8bd 47f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
 e0054e2:	f001 bd91 	b.w	e007008 <DiagPrintf>
 e0054e6:	4641      	mov	r1, r8
 e0054e8:	4650      	mov	r0, sl
 e0054ea:	f001 fd8d 	bl	e007008 <DiagPrintf>
 e0054ee:	6821      	ldr	r1, [r4, #0]
 e0054f0:	4638      	mov	r0, r7
 e0054f2:	f001 fd89 	bl	e007008 <DiagPrintf>
 e0054f6:	e7e1      	b.n	e0054bc <crash_dump_memory+0x54>
 e0054f8:	41008000 	.word	0x41008000
 e0054fc:	0e008684 	.word	0x0e008684
 e005500:	0e008abc 	.word	0x0e008abc
 e005504:	0e008ac0 	.word	0x0e008ac0
 e005508:	0e008264 	.word	0x0e008264
 e00550c:	0e008ae4 	.word	0x0e008ae4
 e005510:	0e0088e0 	.word	0x0e0088e0
 e005514:	0e008258 	.word	0x0e008258
 e005518:	0e008250 	.word	0x0e008250

0e00551c <crash_dump>:
 e00551c:	e92d 4ff8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e005520:	4b51      	ldr	r3, [pc, #324]	; (e005668 <crash_dump+0x14c>)
 e005522:	4614      	mov	r4, r2
 e005524:	4688      	mov	r8, r1
 e005526:	f04f 25e0 	mov.w	r5, #3758153728	; 0xe000e000
 e00552a:	4850      	ldr	r0, [pc, #320]	; (e00566c <crash_dump+0x150>)
 e00552c:	e9d2 2109 	ldrd	r2, r1, [r2, #36]	; 0x24
 e005530:	e9c3 2100 	strd	r2, r1, [r3]
 e005534:	6ae2      	ldr	r2, [r4, #44]	; 0x2c
 e005536:	609a      	str	r2, [r3, #8]
 e005538:	6b22      	ldr	r2, [r4, #48]	; 0x30
 e00553a:	60da      	str	r2, [r3, #12]
 e00553c:	6862      	ldr	r2, [r4, #4]
 e00553e:	611a      	str	r2, [r3, #16]
 e005540:	68a2      	ldr	r2, [r4, #8]
 e005542:	615a      	str	r2, [r3, #20]
 e005544:	68e2      	ldr	r2, [r4, #12]
 e005546:	619a      	str	r2, [r3, #24]
 e005548:	6922      	ldr	r2, [r4, #16]
 e00554a:	61da      	str	r2, [r3, #28]
 e00554c:	6962      	ldr	r2, [r4, #20]
 e00554e:	621a      	str	r2, [r3, #32]
 e005550:	69a2      	ldr	r2, [r4, #24]
 e005552:	625a      	str	r2, [r3, #36]	; 0x24
 e005554:	69e2      	ldr	r2, [r4, #28]
 e005556:	629a      	str	r2, [r3, #40]	; 0x28
 e005558:	6a22      	ldr	r2, [r4, #32]
 e00555a:	62da      	str	r2, [r3, #44]	; 0x2c
 e00555c:	6b62      	ldr	r2, [r4, #52]	; 0x34
 e00555e:	631a      	str	r2, [r3, #48]	; 0x30
 e005560:	6ba2      	ldr	r2, [r4, #56]	; 0x38
 e005562:	635a      	str	r2, [r3, #52]	; 0x34
 e005564:	6be2      	ldr	r2, [r4, #60]	; 0x3c
 e005566:	639a      	str	r2, [r3, #56]	; 0x38
 e005568:	6c22      	ldr	r2, [r4, #64]	; 0x40
 e00556a:	63da      	str	r2, [r3, #60]	; 0x3c
 e00556c:	6822      	ldr	r2, [r4, #0]
 e00556e:	641a      	str	r2, [r3, #64]	; 0x40
 e005570:	f8d5 2d24 	ldr.w	r2, [r5, #3364]	; 0xd24
 e005574:	645a      	str	r2, [r3, #68]	; 0x44
 e005576:	4a3e      	ldr	r2, [pc, #248]	; (e005670 <crash_dump+0x154>)
 e005578:	f892 1028 	ldrb.w	r1, [r2, #40]	; 0x28
 e00557c:	f883 1048 	strb.w	r1, [r3, #72]	; 0x48
 e005580:	f8d5 1d34 	ldr.w	r1, [r5, #3380]	; 0xd34
 e005584:	64d9      	str	r1, [r3, #76]	; 0x4c
 e005586:	f892 1029 	ldrb.w	r1, [r2, #41]	; 0x29
 e00558a:	f883 1050 	strb.w	r1, [r3, #80]	; 0x50
 e00558e:	f8d5 1d38 	ldr.w	r1, [r5, #3384]	; 0xd38
 e005592:	6559      	str	r1, [r3, #84]	; 0x54
 e005594:	8d51      	ldrh	r1, [r2, #42]	; 0x2a
 e005596:	f8a3 1058 	strh.w	r1, [r3, #88]	; 0x58
 e00559a:	f8d5 1d2c 	ldr.w	r1, [r5, #3372]	; 0xd2c
 e00559e:	65d9      	str	r1, [r3, #92]	; 0x5c
 e0055a0:	8e11      	ldrh	r1, [r2, #48]	; 0x30
 e0055a2:	8f92      	ldrh	r2, [r2, #60]	; 0x3c
 e0055a4:	b289      	uxth	r1, r1
 e0055a6:	b292      	uxth	r2, r2
 e0055a8:	6619      	str	r1, [r3, #96]	; 0x60
 e0055aa:	665a      	str	r2, [r3, #100]	; 0x64
 e0055ac:	f8d5 2de4 	ldr.w	r2, [r5, #3556]	; 0xde4
 e0055b0:	669a      	str	r2, [r3, #104]	; 0x68
 e0055b2:	f8d5 2de8 	ldr.w	r2, [r5, #3560]	; 0xde8
 e0055b6:	66da      	str	r2, [r3, #108]	; 0x6c
 e0055b8:	f001 fd26 	bl	e007008 <DiagPrintf>
 e0055bc:	4b2d      	ldr	r3, [pc, #180]	; (e005674 <crash_dump+0x158>)
 e0055be:	681b      	ldr	r3, [r3, #0]
 e0055c0:	b10b      	cbz	r3, e0055c6 <crash_dump+0xaa>
 e0055c2:	4620      	mov	r0, r4
 e0055c4:	4798      	blx	r3
 e0055c6:	4828      	ldr	r0, [pc, #160]	; (e005668 <crash_dump+0x14c>)
 e0055c8:	4627      	mov	r7, r4
 e0055ca:	f000 f86f 	bl	e0056ac <fault_diagnosis>
 e0055ce:	482a      	ldr	r0, [pc, #168]	; (e005678 <crash_dump+0x15c>)
 e0055d0:	f001 fd1a 	bl	e007008 <DiagPrintf>
 e0055d4:	6ba1      	ldr	r1, [r4, #56]	; 0x38
 e0055d6:	4829      	ldr	r0, [pc, #164]	; (e00567c <crash_dump+0x160>)
 e0055d8:	f104 0a30 	add.w	sl, r4, #48	; 0x30
 e0055dc:	f001 fd14 	bl	e007008 <DiagPrintf>
 e0055e0:	6be1      	ldr	r1, [r4, #60]	; 0x3c
 e0055e2:	4827      	ldr	r0, [pc, #156]	; (e005680 <crash_dump+0x164>)
 e0055e4:	4625      	mov	r5, r4
 e0055e6:	f001 fd0f 	bl	e007008 <DiagPrintf>
 e0055ea:	6c21      	ldr	r1, [r4, #64]	; 0x40
 e0055ec:	4825      	ldr	r0, [pc, #148]	; (e005684 <crash_dump+0x168>)
 e0055ee:	f001 fd0b 	bl	e007008 <DiagPrintf>
 e0055f2:	f857 1b20 	ldr.w	r1, [r7], #32
 e0055f6:	4824      	ldr	r0, [pc, #144]	; (e005688 <crash_dump+0x16c>)
 e0055f8:	463e      	mov	r6, r7
 e0055fa:	f8df b0a0 	ldr.w	fp, [pc, #160]	; e00569c <crash_dump+0x180>
 e0055fe:	f001 fd03 	bl	e007008 <DiagPrintf>
 e005602:	4922      	ldr	r1, [pc, #136]	; (e00568c <crash_dump+0x170>)
 e005604:	f856 2f04 	ldr.w	r2, [r6, #4]!
 e005608:	4658      	mov	r0, fp
 e00560a:	f001 fcfd 	bl	e007008 <DiagPrintf>
 e00560e:	f8df 9098 	ldr.w	r9, [pc, #152]	; e0056a8 <crash_dump+0x18c>
 e005612:	4556      	cmp	r6, sl
 e005614:	d008      	beq.n	e005628 <crash_dump+0x10c>
 e005616:	f856 2f04 	ldr.w	r2, [r6, #4]!
 e00561a:	4658      	mov	r0, fp
 e00561c:	f859 1b04 	ldr.w	r1, [r9], #4
 e005620:	f001 fcf2 	bl	e007008 <DiagPrintf>
 e005624:	4556      	cmp	r6, sl
 e005626:	d1f6      	bne.n	e005616 <crash_dump+0xfa>
 e005628:	4e19      	ldr	r6, [pc, #100]	; (e005690 <crash_dump+0x174>)
 e00562a:	491a      	ldr	r1, [pc, #104]	; (e005694 <crash_dump+0x178>)
 e00562c:	f8df 906c 	ldr.w	r9, [pc, #108]	; e00569c <crash_dump+0x180>
 e005630:	e001      	b.n	e005636 <crash_dump+0x11a>
 e005632:	f856 1b04 	ldr.w	r1, [r6], #4
 e005636:	f855 2f04 	ldr.w	r2, [r5, #4]!
 e00563a:	4648      	mov	r0, r9
 e00563c:	f001 fce4 	bl	e007008 <DiagPrintf>
 e005640:	42bd      	cmp	r5, r7
 e005642:	d1f6      	bne.n	e005632 <crash_dump+0x116>
 e005644:	6b62      	ldr	r2, [r4, #52]	; 0x34
 e005646:	4914      	ldr	r1, [pc, #80]	; (e005698 <crash_dump+0x17c>)
 e005648:	4814      	ldr	r0, [pc, #80]	; (e00569c <crash_dump+0x180>)
 e00564a:	f001 fcdd 	bl	e007008 <DiagPrintf>
 e00564e:	2180      	movs	r1, #128	; 0x80
 e005650:	4640      	mov	r0, r8
 e005652:	f7ff ff09 	bl	e005468 <crash_dump_memory>
 e005656:	4812      	ldr	r0, [pc, #72]	; (e0056a0 <crash_dump+0x184>)
 e005658:	f001 fcd6 	bl	e007008 <DiagPrintf>
 e00565c:	4811      	ldr	r0, [pc, #68]	; (e0056a4 <crash_dump+0x188>)
 e00565e:	f001 fcd3 	bl	e007008 <DiagPrintf>
 e005662:	2000      	movs	r0, #0
 e005664:	e8bd 8ff8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, pc}
 e005668:	2000bd94 	.word	0x2000bd94
 e00566c:	0e008b24 	.word	0x0e008b24
 e005670:	e000ed00 	.word	0xe000ed00
 e005674:	2000bd90 	.word	0x2000bd90
 e005678:	0e008b48 	.word	0x0e008b48
 e00567c:	0e008b70 	.word	0x0e008b70
 e005680:	0e008b80 	.word	0x0e008b80
 e005684:	0e008b90 	.word	0x0e008b90
 e005688:	0e008ba0 	.word	0x0e008ba0
 e00568c:	0e008b1c 	.word	0x0e008b1c
 e005690:	0e008c28 	.word	0x0e008c28
 e005694:	0e008b20 	.word	0x0e008b20
 e005698:	0e008bc0 	.word	0x0e008bc0
 e00569c:	0e008bb0 	.word	0x0e008bb0
 e0056a0:	0e008bc4 	.word	0x0e008bc4
 e0056a4:	0e008bf0 	.word	0x0e008bf0
 e0056a8:	0e008c48 	.word	0x0e008c48

0e0056ac <fault_diagnosis>:
 e0056ac:	b510      	push	{r4, lr}
 e0056ae:	4604      	mov	r4, r0
 e0056b0:	4888      	ldr	r0, [pc, #544]	; (e0058d4 <fault_diagnosis+0x228>)
 e0056b2:	6ba1      	ldr	r1, [r4, #56]	; 0x38
 e0056b4:	f001 fca8 	bl	e007008 <DiagPrintf>
 e0056b8:	f894 305c 	ldrb.w	r3, [r4, #92]	; 0x5c
 e0056bc:	079b      	lsls	r3, r3, #30
 e0056be:	f100 808a 	bmi.w	e0057d6 <fault_diagnosis+0x12a>
 e0056c2:	6ea1      	ldr	r1, [r4, #104]	; 0x68
 e0056c4:	b1e1      	cbz	r1, e005700 <fault_diagnosis+0x54>
 e0056c6:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 e0056ca:	07d8      	lsls	r0, r3, #31
 e0056cc:	f100 8094 	bmi.w	e0057f8 <fault_diagnosis+0x14c>
 e0056d0:	079a      	lsls	r2, r3, #30
 e0056d2:	f100 809a 	bmi.w	e00580a <fault_diagnosis+0x15e>
 e0056d6:	0758      	lsls	r0, r3, #29
 e0056d8:	f100 80a1 	bmi.w	e00581e <fault_diagnosis+0x172>
 e0056dc:	0719      	lsls	r1, r3, #28
 e0056de:	f100 80a8 	bmi.w	e005832 <fault_diagnosis+0x186>
 e0056e2:	06da      	lsls	r2, r3, #27
 e0056e4:	f100 80af 	bmi.w	e005846 <fault_diagnosis+0x19a>
 e0056e8:	0698      	lsls	r0, r3, #26
 e0056ea:	f100 80b6 	bmi.w	e00585a <fault_diagnosis+0x1ae>
 e0056ee:	0619      	lsls	r1, r3, #24
 e0056f0:	f100 80bd 	bmi.w	e00586e <fault_diagnosis+0x1c2>
 e0056f4:	065a      	lsls	r2, r3, #25
 e0056f6:	d503      	bpl.n	e005700 <fault_diagnosis+0x54>
 e0056f8:	f013 0f28 	tst.w	r3, #40	; 0x28
 e0056fc:	f040 8186 	bne.w	e005a0c <fault_diagnosis+0x360>
 e005700:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
 e005704:	b199      	cbz	r1, e00572e <fault_diagnosis+0x82>
 e005706:	07cb      	lsls	r3, r1, #31
 e005708:	f100 80b9 	bmi.w	e00587e <fault_diagnosis+0x1d2>
 e00570c:	0788      	lsls	r0, r1, #30
 e00570e:	f100 80bf 	bmi.w	e005890 <fault_diagnosis+0x1e4>
 e005712:	070a      	lsls	r2, r1, #28
 e005714:	f100 80c5 	bmi.w	e0058a2 <fault_diagnosis+0x1f6>
 e005718:	06cb      	lsls	r3, r1, #27
 e00571a:	f100 80cb 	bmi.w	e0058b4 <fault_diagnosis+0x208>
 e00571e:	0688      	lsls	r0, r1, #26
 e005720:	f100 80d1 	bmi.w	e0058c6 <fault_diagnosis+0x21a>
 e005724:	060a      	lsls	r2, r1, #24
 e005726:	d502      	bpl.n	e00572e <fault_diagnosis+0x82>
 e005728:	078b      	lsls	r3, r1, #30
 e00572a:	f040 8194 	bne.w	e005a56 <fault_diagnosis+0x3aa>
 e00572e:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
 e005732:	b1b1      	cbz	r1, e005762 <fault_diagnosis+0xb6>
 e005734:	07c8      	lsls	r0, r1, #31
 e005736:	f100 80f7 	bmi.w	e005928 <fault_diagnosis+0x27c>
 e00573a:	078a      	lsls	r2, r1, #30
 e00573c:	f100 80fd 	bmi.w	e00593a <fault_diagnosis+0x28e>
 e005740:	074b      	lsls	r3, r1, #29
 e005742:	f100 8103 	bmi.w	e00594c <fault_diagnosis+0x2a0>
 e005746:	0708      	lsls	r0, r1, #28
 e005748:	f100 8109 	bmi.w	e00595e <fault_diagnosis+0x2b2>
 e00574c:	06ca      	lsls	r2, r1, #27
 e00574e:	f100 810f 	bmi.w	e005970 <fault_diagnosis+0x2c4>
 e005752:	068b      	lsls	r3, r1, #26
 e005754:	f100 8115 	bmi.w	e005982 <fault_diagnosis+0x2d6>
 e005758:	0608      	lsls	r0, r1, #24
 e00575a:	d502      	bpl.n	e005762 <fault_diagnosis+0xb6>
 e00575c:	0789      	lsls	r1, r1, #30
 e00575e:	f100 8180 	bmi.w	e005a62 <fault_diagnosis+0x3b6>
 e005762:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
 e005766:	b1b9      	cbz	r1, e005798 <fault_diagnosis+0xec>
 e005768:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
 e00576c:	07da      	lsls	r2, r3, #31
 e00576e:	f100 810f 	bmi.w	e005990 <fault_diagnosis+0x2e4>
 e005772:	0798      	lsls	r0, r3, #30
 e005774:	f100 8115 	bmi.w	e0059a2 <fault_diagnosis+0x2f6>
 e005778:	0759      	lsls	r1, r3, #29
 e00577a:	f100 811d 	bmi.w	e0059b8 <fault_diagnosis+0x30c>
 e00577e:	071a      	lsls	r2, r3, #28
 e005780:	f100 8125 	bmi.w	e0059ce <fault_diagnosis+0x322>
 e005784:	06db      	lsls	r3, r3, #27
 e005786:	f100 812d 	bmi.w	e0059e4 <fault_diagnosis+0x338>
 e00578a:	f894 3059 	ldrb.w	r3, [r4, #89]	; 0x59
 e00578e:	07d8      	lsls	r0, r3, #31
 e005790:	f100 8133 	bmi.w	e0059fa <fault_diagnosis+0x34e>
 e005794:	0799      	lsls	r1, r3, #30
 e005796:	d424      	bmi.n	e0057e2 <fault_diagnosis+0x136>
 e005798:	f994 305f 	ldrsb.w	r3, [r4, #95]	; 0x5f
 e00579c:	2b00      	cmp	r3, #0
 e00579e:	db00      	blt.n	e0057a2 <fault_diagnosis+0xf6>
 e0057a0:	bd10      	pop	{r4, pc}
 e0057a2:	6e21      	ldr	r1, [r4, #96]	; 0x60
 e0057a4:	2900      	cmp	r1, #0
 e0057a6:	d0fb      	beq.n	e0057a0 <fault_diagnosis+0xf4>
 e0057a8:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
 e0057ac:	07da      	lsls	r2, r3, #31
 e0057ae:	f100 8143 	bmi.w	e005a38 <fault_diagnosis+0x38c>
 e0057b2:	0798      	lsls	r0, r3, #30
 e0057b4:	f100 8138 	bmi.w	e005a28 <fault_diagnosis+0x37c>
 e0057b8:	0759      	lsls	r1, r3, #29
 e0057ba:	f100 812d 	bmi.w	e005a18 <fault_diagnosis+0x36c>
 e0057be:	071a      	lsls	r2, r3, #28
 e0057c0:	f100 8141 	bmi.w	e005a46 <fault_diagnosis+0x39a>
 e0057c4:	06db      	lsls	r3, r3, #27
 e0057c6:	d5eb      	bpl.n	e0057a0 <fault_diagnosis+0xf4>
 e0057c8:	6e21      	ldr	r1, [r4, #96]	; 0x60
 e0057ca:	4a43      	ldr	r2, [pc, #268]	; (e0058d8 <fault_diagnosis+0x22c>)
 e0057cc:	4843      	ldr	r0, [pc, #268]	; (e0058dc <fault_diagnosis+0x230>)
 e0057ce:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
 e0057d2:	f001 bc19 	b.w	e007008 <DiagPrintf>
 e0057d6:	4a42      	ldr	r2, [pc, #264]	; (e0058e0 <fault_diagnosis+0x234>)
 e0057d8:	6de1      	ldr	r1, [r4, #92]	; 0x5c
 e0057da:	4842      	ldr	r0, [pc, #264]	; (e0058e4 <fault_diagnosis+0x238>)
 e0057dc:	f001 fc14 	bl	e007008 <DiagPrintf>
 e0057e0:	e76f      	b.n	e0056c2 <fault_diagnosis+0x16>
 e0057e2:	4a41      	ldr	r2, [pc, #260]	; (e0058e8 <fault_diagnosis+0x23c>)
 e0057e4:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
 e0057e8:	4840      	ldr	r0, [pc, #256]	; (e0058ec <fault_diagnosis+0x240>)
 e0057ea:	f001 fc0d 	bl	e007008 <DiagPrintf>
 e0057ee:	f994 305f 	ldrsb.w	r3, [r4, #95]	; 0x5f
 e0057f2:	2b00      	cmp	r3, #0
 e0057f4:	dad4      	bge.n	e0057a0 <fault_diagnosis+0xf4>
 e0057f6:	e7d4      	b.n	e0057a2 <fault_diagnosis+0xf6>
 e0057f8:	4a3d      	ldr	r2, [pc, #244]	; (e0058f0 <fault_diagnosis+0x244>)
 e0057fa:	483e      	ldr	r0, [pc, #248]	; (e0058f4 <fault_diagnosis+0x248>)
 e0057fc:	f001 fc04 	bl	e007008 <DiagPrintf>
 e005800:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 e005804:	079a      	lsls	r2, r3, #30
 e005806:	f57f af66 	bpl.w	e0056d6 <fault_diagnosis+0x2a>
 e00580a:	4a3b      	ldr	r2, [pc, #236]	; (e0058f8 <fault_diagnosis+0x24c>)
 e00580c:	6ea1      	ldr	r1, [r4, #104]	; 0x68
 e00580e:	4839      	ldr	r0, [pc, #228]	; (e0058f4 <fault_diagnosis+0x248>)
 e005810:	f001 fbfa 	bl	e007008 <DiagPrintf>
 e005814:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 e005818:	0758      	lsls	r0, r3, #29
 e00581a:	f57f af5f 	bpl.w	e0056dc <fault_diagnosis+0x30>
 e00581e:	6ea1      	ldr	r1, [r4, #104]	; 0x68
 e005820:	4a36      	ldr	r2, [pc, #216]	; (e0058fc <fault_diagnosis+0x250>)
 e005822:	4834      	ldr	r0, [pc, #208]	; (e0058f4 <fault_diagnosis+0x248>)
 e005824:	f001 fbf0 	bl	e007008 <DiagPrintf>
 e005828:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 e00582c:	0719      	lsls	r1, r3, #28
 e00582e:	f57f af58 	bpl.w	e0056e2 <fault_diagnosis+0x36>
 e005832:	4a33      	ldr	r2, [pc, #204]	; (e005900 <fault_diagnosis+0x254>)
 e005834:	6ea1      	ldr	r1, [r4, #104]	; 0x68
 e005836:	482f      	ldr	r0, [pc, #188]	; (e0058f4 <fault_diagnosis+0x248>)
 e005838:	f001 fbe6 	bl	e007008 <DiagPrintf>
 e00583c:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 e005840:	06da      	lsls	r2, r3, #27
 e005842:	f57f af51 	bpl.w	e0056e8 <fault_diagnosis+0x3c>
 e005846:	4a2f      	ldr	r2, [pc, #188]	; (e005904 <fault_diagnosis+0x258>)
 e005848:	6ea1      	ldr	r1, [r4, #104]	; 0x68
 e00584a:	482a      	ldr	r0, [pc, #168]	; (e0058f4 <fault_diagnosis+0x248>)
 e00584c:	f001 fbdc 	bl	e007008 <DiagPrintf>
 e005850:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 e005854:	0698      	lsls	r0, r3, #26
 e005856:	f57f af4a 	bpl.w	e0056ee <fault_diagnosis+0x42>
 e00585a:	6ea1      	ldr	r1, [r4, #104]	; 0x68
 e00585c:	4a2a      	ldr	r2, [pc, #168]	; (e005908 <fault_diagnosis+0x25c>)
 e00585e:	4825      	ldr	r0, [pc, #148]	; (e0058f4 <fault_diagnosis+0x248>)
 e005860:	f001 fbd2 	bl	e007008 <DiagPrintf>
 e005864:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 e005868:	0619      	lsls	r1, r3, #24
 e00586a:	f57f af43 	bpl.w	e0056f4 <fault_diagnosis+0x48>
 e00586e:	4a27      	ldr	r2, [pc, #156]	; (e00590c <fault_diagnosis+0x260>)
 e005870:	6ea1      	ldr	r1, [r4, #104]	; 0x68
 e005872:	4820      	ldr	r0, [pc, #128]	; (e0058f4 <fault_diagnosis+0x248>)
 e005874:	f001 fbc8 	bl	e007008 <DiagPrintf>
 e005878:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
 e00587c:	e73a      	b.n	e0056f4 <fault_diagnosis+0x48>
 e00587e:	4a24      	ldr	r2, [pc, #144]	; (e005910 <fault_diagnosis+0x264>)
 e005880:	4824      	ldr	r0, [pc, #144]	; (e005914 <fault_diagnosis+0x268>)
 e005882:	f001 fbc1 	bl	e007008 <DiagPrintf>
 e005886:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
 e00588a:	0788      	lsls	r0, r1, #30
 e00588c:	f57f af41 	bpl.w	e005712 <fault_diagnosis+0x66>
 e005890:	4a21      	ldr	r2, [pc, #132]	; (e005918 <fault_diagnosis+0x26c>)
 e005892:	4820      	ldr	r0, [pc, #128]	; (e005914 <fault_diagnosis+0x268>)
 e005894:	f001 fbb8 	bl	e007008 <DiagPrintf>
 e005898:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
 e00589c:	070a      	lsls	r2, r1, #28
 e00589e:	f57f af3b 	bpl.w	e005718 <fault_diagnosis+0x6c>
 e0058a2:	4a1e      	ldr	r2, [pc, #120]	; (e00591c <fault_diagnosis+0x270>)
 e0058a4:	481b      	ldr	r0, [pc, #108]	; (e005914 <fault_diagnosis+0x268>)
 e0058a6:	f001 fbaf 	bl	e007008 <DiagPrintf>
 e0058aa:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
 e0058ae:	06cb      	lsls	r3, r1, #27
 e0058b0:	f57f af35 	bpl.w	e00571e <fault_diagnosis+0x72>
 e0058b4:	4a1a      	ldr	r2, [pc, #104]	; (e005920 <fault_diagnosis+0x274>)
 e0058b6:	4817      	ldr	r0, [pc, #92]	; (e005914 <fault_diagnosis+0x268>)
 e0058b8:	f001 fba6 	bl	e007008 <DiagPrintf>
 e0058bc:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
 e0058c0:	0688      	lsls	r0, r1, #26
 e0058c2:	f57f af2f 	bpl.w	e005724 <fault_diagnosis+0x78>
 e0058c6:	4a17      	ldr	r2, [pc, #92]	; (e005924 <fault_diagnosis+0x278>)
 e0058c8:	4812      	ldr	r0, [pc, #72]	; (e005914 <fault_diagnosis+0x268>)
 e0058ca:	f001 fb9d 	bl	e007008 <DiagPrintf>
 e0058ce:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
 e0058d2:	e727      	b.n	e005724 <fault_diagnosis+0x78>
 e0058d4:	0e008c58 	.word	0x0e008c58
 e0058d8:	0e0094e8 	.word	0x0e0094e8
 e0058dc:	0e009440 	.word	0x0e009440
 e0058e0:	0e008c70 	.word	0x0e008c70
 e0058e4:	0e008c9c 	.word	0x0e008c9c
 e0058e8:	0e0093a4 	.word	0x0e0093a4
 e0058ec:	0e0091e8 	.word	0x0e0091e8
 e0058f0:	0e008cb4 	.word	0x0e008cb4
 e0058f4:	0e008ce4 	.word	0x0e008ce4
 e0058f8:	0e008cfc 	.word	0x0e008cfc
 e0058fc:	0e008d34 	.word	0x0e008d34
 e005900:	0e008d6c 	.word	0x0e008d6c
 e005904:	0e008da4 	.word	0x0e008da4
 e005908:	0e008dd4 	.word	0x0e008dd4
 e00590c:	0e008e10 	.word	0x0e008e10
 e005910:	0e008e80 	.word	0x0e008e80
 e005914:	0e008ec4 	.word	0x0e008ec4
 e005918:	0e008edc 	.word	0x0e008edc
 e00591c:	0e008f18 	.word	0x0e008f18
 e005920:	0e008f50 	.word	0x0e008f50
 e005924:	0e008f84 	.word	0x0e008f84
 e005928:	4a51      	ldr	r2, [pc, #324]	; (e005a70 <fault_diagnosis+0x3c4>)
 e00592a:	4852      	ldr	r0, [pc, #328]	; (e005a74 <fault_diagnosis+0x3c8>)
 e00592c:	f001 fb6c 	bl	e007008 <DiagPrintf>
 e005930:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
 e005934:	078a      	lsls	r2, r1, #30
 e005936:	f57f af03 	bpl.w	e005740 <fault_diagnosis+0x94>
 e00593a:	4a4f      	ldr	r2, [pc, #316]	; (e005a78 <fault_diagnosis+0x3cc>)
 e00593c:	484d      	ldr	r0, [pc, #308]	; (e005a74 <fault_diagnosis+0x3c8>)
 e00593e:	f001 fb63 	bl	e007008 <DiagPrintf>
 e005942:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
 e005946:	074b      	lsls	r3, r1, #29
 e005948:	f57f aefd 	bpl.w	e005746 <fault_diagnosis+0x9a>
 e00594c:	4a4b      	ldr	r2, [pc, #300]	; (e005a7c <fault_diagnosis+0x3d0>)
 e00594e:	4849      	ldr	r0, [pc, #292]	; (e005a74 <fault_diagnosis+0x3c8>)
 e005950:	f001 fb5a 	bl	e007008 <DiagPrintf>
 e005954:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
 e005958:	0708      	lsls	r0, r1, #28
 e00595a:	f57f aef7 	bpl.w	e00574c <fault_diagnosis+0xa0>
 e00595e:	4a48      	ldr	r2, [pc, #288]	; (e005a80 <fault_diagnosis+0x3d4>)
 e005960:	4844      	ldr	r0, [pc, #272]	; (e005a74 <fault_diagnosis+0x3c8>)
 e005962:	f001 fb51 	bl	e007008 <DiagPrintf>
 e005966:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
 e00596a:	06ca      	lsls	r2, r1, #27
 e00596c:	f57f aef1 	bpl.w	e005752 <fault_diagnosis+0xa6>
 e005970:	4a44      	ldr	r2, [pc, #272]	; (e005a84 <fault_diagnosis+0x3d8>)
 e005972:	4840      	ldr	r0, [pc, #256]	; (e005a74 <fault_diagnosis+0x3c8>)
 e005974:	f001 fb48 	bl	e007008 <DiagPrintf>
 e005978:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
 e00597c:	068b      	lsls	r3, r1, #26
 e00597e:	f57f aeeb 	bpl.w	e005758 <fault_diagnosis+0xac>
 e005982:	4a41      	ldr	r2, [pc, #260]	; (e005a88 <fault_diagnosis+0x3dc>)
 e005984:	483b      	ldr	r0, [pc, #236]	; (e005a74 <fault_diagnosis+0x3c8>)
 e005986:	f001 fb3f 	bl	e007008 <DiagPrintf>
 e00598a:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
 e00598e:	e6e3      	b.n	e005758 <fault_diagnosis+0xac>
 e005990:	4a3e      	ldr	r2, [pc, #248]	; (e005a8c <fault_diagnosis+0x3e0>)
 e005992:	483f      	ldr	r0, [pc, #252]	; (e005a90 <fault_diagnosis+0x3e4>)
 e005994:	f001 fb38 	bl	e007008 <DiagPrintf>
 e005998:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
 e00599c:	0798      	lsls	r0, r3, #30
 e00599e:	f57f aeeb 	bpl.w	e005778 <fault_diagnosis+0xcc>
 e0059a2:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
 e0059a6:	4a3b      	ldr	r2, [pc, #236]	; (e005a94 <fault_diagnosis+0x3e8>)
 e0059a8:	4839      	ldr	r0, [pc, #228]	; (e005a90 <fault_diagnosis+0x3e4>)
 e0059aa:	f001 fb2d 	bl	e007008 <DiagPrintf>
 e0059ae:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
 e0059b2:	0759      	lsls	r1, r3, #29
 e0059b4:	f57f aee3 	bpl.w	e00577e <fault_diagnosis+0xd2>
 e0059b8:	4a37      	ldr	r2, [pc, #220]	; (e005a98 <fault_diagnosis+0x3ec>)
 e0059ba:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
 e0059be:	4834      	ldr	r0, [pc, #208]	; (e005a90 <fault_diagnosis+0x3e4>)
 e0059c0:	f001 fb22 	bl	e007008 <DiagPrintf>
 e0059c4:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
 e0059c8:	071a      	lsls	r2, r3, #28
 e0059ca:	f57f aedb 	bpl.w	e005784 <fault_diagnosis+0xd8>
 e0059ce:	4a33      	ldr	r2, [pc, #204]	; (e005a9c <fault_diagnosis+0x3f0>)
 e0059d0:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
 e0059d4:	482e      	ldr	r0, [pc, #184]	; (e005a90 <fault_diagnosis+0x3e4>)
 e0059d6:	f001 fb17 	bl	e007008 <DiagPrintf>
 e0059da:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
 e0059de:	06db      	lsls	r3, r3, #27
 e0059e0:	f57f aed3 	bpl.w	e00578a <fault_diagnosis+0xde>
 e0059e4:	4a2e      	ldr	r2, [pc, #184]	; (e005aa0 <fault_diagnosis+0x3f4>)
 e0059e6:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
 e0059ea:	4829      	ldr	r0, [pc, #164]	; (e005a90 <fault_diagnosis+0x3e4>)
 e0059ec:	f001 fb0c 	bl	e007008 <DiagPrintf>
 e0059f0:	f894 3059 	ldrb.w	r3, [r4, #89]	; 0x59
 e0059f4:	07d8      	lsls	r0, r3, #31
 e0059f6:	f57f aecd 	bpl.w	e005794 <fault_diagnosis+0xe8>
 e0059fa:	4a2a      	ldr	r2, [pc, #168]	; (e005aa4 <fault_diagnosis+0x3f8>)
 e0059fc:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
 e005a00:	4823      	ldr	r0, [pc, #140]	; (e005a90 <fault_diagnosis+0x3e4>)
 e005a02:	f001 fb01 	bl	e007008 <DiagPrintf>
 e005a06:	f894 3059 	ldrb.w	r3, [r4, #89]	; 0x59
 e005a0a:	e6c3      	b.n	e005794 <fault_diagnosis+0xe8>
 e005a0c:	4a26      	ldr	r2, [pc, #152]	; (e005aa8 <fault_diagnosis+0x3fc>)
 e005a0e:	6ee1      	ldr	r1, [r4, #108]	; 0x6c
 e005a10:	4826      	ldr	r0, [pc, #152]	; (e005aac <fault_diagnosis+0x400>)
 e005a12:	f001 faf9 	bl	e007008 <DiagPrintf>
 e005a16:	e673      	b.n	e005700 <fault_diagnosis+0x54>
 e005a18:	4a25      	ldr	r2, [pc, #148]	; (e005ab0 <fault_diagnosis+0x404>)
 e005a1a:	6e21      	ldr	r1, [r4, #96]	; 0x60
 e005a1c:	4825      	ldr	r0, [pc, #148]	; (e005ab4 <fault_diagnosis+0x408>)
 e005a1e:	f001 faf3 	bl	e007008 <DiagPrintf>
 e005a22:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
 e005a26:	e6ca      	b.n	e0057be <fault_diagnosis+0x112>
 e005a28:	4a23      	ldr	r2, [pc, #140]	; (e005ab8 <fault_diagnosis+0x40c>)
 e005a2a:	6e21      	ldr	r1, [r4, #96]	; 0x60
 e005a2c:	4821      	ldr	r0, [pc, #132]	; (e005ab4 <fault_diagnosis+0x408>)
 e005a2e:	f001 faeb 	bl	e007008 <DiagPrintf>
 e005a32:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
 e005a36:	e6bf      	b.n	e0057b8 <fault_diagnosis+0x10c>
 e005a38:	4a20      	ldr	r2, [pc, #128]	; (e005abc <fault_diagnosis+0x410>)
 e005a3a:	481e      	ldr	r0, [pc, #120]	; (e005ab4 <fault_diagnosis+0x408>)
 e005a3c:	f001 fae4 	bl	e007008 <DiagPrintf>
 e005a40:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
 e005a44:	e6b5      	b.n	e0057b2 <fault_diagnosis+0x106>
 e005a46:	4a1e      	ldr	r2, [pc, #120]	; (e005ac0 <fault_diagnosis+0x414>)
 e005a48:	6e21      	ldr	r1, [r4, #96]	; 0x60
 e005a4a:	481a      	ldr	r0, [pc, #104]	; (e005ab4 <fault_diagnosis+0x408>)
 e005a4c:	f001 fadc 	bl	e007008 <DiagPrintf>
 e005a50:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
 e005a54:	e6b6      	b.n	e0057c4 <fault_diagnosis+0x118>
 e005a56:	4a1b      	ldr	r2, [pc, #108]	; (e005ac4 <fault_diagnosis+0x418>)
 e005a58:	6ce1      	ldr	r1, [r4, #76]	; 0x4c
 e005a5a:	481b      	ldr	r0, [pc, #108]	; (e005ac8 <fault_diagnosis+0x41c>)
 e005a5c:	f001 fad4 	bl	e007008 <DiagPrintf>
 e005a60:	e665      	b.n	e00572e <fault_diagnosis+0x82>
 e005a62:	4a1a      	ldr	r2, [pc, #104]	; (e005acc <fault_diagnosis+0x420>)
 e005a64:	6d61      	ldr	r1, [r4, #84]	; 0x54
 e005a66:	481a      	ldr	r0, [pc, #104]	; (e005ad0 <fault_diagnosis+0x424>)
 e005a68:	f001 face 	bl	e007008 <DiagPrintf>
 e005a6c:	e679      	b.n	e005762 <fault_diagnosis+0xb6>
 e005a6e:	bf00      	nop
 e005a70:	0e009018 	.word	0x0e009018
 e005a74:	0e00904c 	.word	0x0e00904c
 e005a78:	0e009064 	.word	0x0e009064
 e005a7c:	0e00909c 	.word	0x0e00909c
 e005a80:	0e0090d4 	.word	0x0e0090d4
 e005a84:	0e0090fc 	.word	0x0e0090fc
 e005a88:	0e009124 	.word	0x0e009124
 e005a8c:	0e0091a0 	.word	0x0e0091a0
 e005a90:	0e0091e8 	.word	0x0e0091e8
 e005a94:	0e009200 	.word	0x0e009200
 e005a98:	0e00924c 	.word	0x0e00924c
 e005a9c:	0e0092ac 	.word	0x0e0092ac
 e005aa0:	0e0092f4 	.word	0x0e0092f4
 e005aa4:	0e009350 	.word	0x0e009350
 e005aa8:	0e008e40 	.word	0x0e008e40
 e005aac:	0e008e68 	.word	0x0e008e68
 e005ab0:	0e00948c 	.word	0x0e00948c
 e005ab4:	0e009440 	.word	0x0e009440
 e005ab8:	0e009458 	.word	0x0e009458
 e005abc:	0e009410 	.word	0x0e009410
 e005ac0:	0e0094b8 	.word	0x0e0094b8
 e005ac4:	0e008fd0 	.word	0x0e008fd0
 e005ac8:	0e009000 	.word	0x0e009000
 e005acc:	0e009164 	.word	0x0e009164
 e005ad0:	0e009188 	.word	0x0e009188

0e005ad4 <_my_calloc>:
 e005ad4:	fb01 f000 	mul.w	r0, r1, r0
 e005ad8:	f7fd b808 	b.w	e002aec <rtos_mem_zmalloc>

0e005adc <_my_free>:
 e005adc:	f7fd b812 	b.w	e002b04 <rtos_mem_free>

0e005ae0 <_my_random>:
 e005ae0:	b508      	push	{r3, lr}
 e005ae2:	4608      	mov	r0, r1
 e005ae4:	4b02      	ldr	r3, [pc, #8]	; (e005af0 <_my_random+0x10>)
 e005ae6:	4611      	mov	r1, r2
 e005ae8:	4798      	blx	r3
 e005aea:	2000      	movs	r0, #0
 e005aec:	bd08      	pop	{r3, pc}
 e005aee:	bf00      	nop
 e005af0:	0e007311 	.word	0x0e007311

0e005af4 <wifi_on>:
 e005af4:	b530      	push	{r4, r5, lr}
 e005af6:	b083      	sub	sp, #12
 e005af8:	4604      	mov	r4, r0
 e005afa:	f7fc f9db 	bl	e001eb4 <wifi_set_user_config>
 e005afe:	f000 fd6f 	bl	e0065e0 <inic_msg_q_init>
 e005b02:	4a1d      	ldr	r2, [pc, #116]	; (e005b78 <wifi_on+0x84>)
 e005b04:	4b1d      	ldr	r3, [pc, #116]	; (e005b7c <wifi_on+0x88>)
 e005b06:	6951      	ldr	r1, [r2, #20]
 e005b08:	03c9      	lsls	r1, r1, #15
 e005b0a:	d50b      	bpl.n	e005b24 <wifi_on+0x30>
 e005b0c:	f3bf 8f4f 	dsb	sy
 e005b10:	f103 0120 	add.w	r1, r3, #32
 e005b14:	f8c2 3268 	str.w	r3, [r2, #616]	; 0x268
 e005b18:	f8c2 1268 	str.w	r1, [r2, #616]	; 0x268
 e005b1c:	f3bf 8f4f 	dsb	sy
 e005b20:	f3bf 8f6f 	isb	sy
 e005b24:	2201      	movs	r2, #1
 e005b26:	a901      	add	r1, sp, #4
 e005b28:	2007      	movs	r0, #7
 e005b2a:	9301      	str	r3, [sp, #4]
 e005b2c:	f000 fa30 	bl	e005f90 <inic_api_host_message_send>
 e005b30:	4b13      	ldr	r3, [pc, #76]	; (e005b80 <wifi_on+0x8c>)
 e005b32:	4914      	ldr	r1, [pc, #80]	; (e005b84 <wifi_on+0x90>)
 e005b34:	4a14      	ldr	r2, [pc, #80]	; (e005b88 <wifi_on+0x94>)
 e005b36:	600b      	str	r3, [r1, #0]
 e005b38:	601a      	str	r2, [r3, #0]
 e005b3a:	4914      	ldr	r1, [pc, #80]	; (e005b8c <wifi_on+0x98>)
 e005b3c:	4a14      	ldr	r2, [pc, #80]	; (e005b90 <wifi_on+0x9c>)
 e005b3e:	e9c3 1201 	strd	r1, r2, [r3, #4]
 e005b42:	f000 fbdf 	bl	e006304 <inic_host_init_skb>
 e005b46:	2201      	movs	r2, #1
 e005b48:	a901      	add	r1, sp, #4
 e005b4a:	2009      	movs	r0, #9
 e005b4c:	9401      	str	r4, [sp, #4]
 e005b4e:	f000 fa1f 	bl	e005f90 <inic_api_host_message_send>
 e005b52:	4b10      	ldr	r3, [pc, #64]	; (e005b94 <wifi_on+0xa0>)
 e005b54:	4605      	mov	r5, r0
 e005b56:	681a      	ldr	r2, [r3, #0]
 e005b58:	b112      	cbz	r2, e005b60 <wifi_on+0x6c>
 e005b5a:	4628      	mov	r0, r5
 e005b5c:	b003      	add	sp, #12
 e005b5e:	bd30      	pop	{r4, r5, pc}
 e005b60:	2201      	movs	r2, #1
 e005b62:	601a      	str	r2, [r3, #0]
 e005b64:	f000 fe08 	bl	e006778 <init_timer_wrapper>
 e005b68:	4b0b      	ldr	r3, [pc, #44]	; (e005b98 <wifi_on+0xa4>)
 e005b6a:	681b      	ldr	r3, [r3, #0]
 e005b6c:	2b00      	cmp	r3, #0
 e005b6e:	d0f4      	beq.n	e005b5a <wifi_on+0x66>
 e005b70:	2c01      	cmp	r4, #1
 e005b72:	d1f2      	bne.n	e005b5a <wifi_on+0x66>
 e005b74:	4798      	blx	r3
 e005b76:	e7f0      	b.n	e005b5a <wifi_on+0x66>
 e005b78:	e000ed00 	.word	0xe000ed00
 e005b7c:	2000b400 	.word	0x2000b400
 e005b80:	2000bf64 	.word	0x2000bf64
 e005b84:	2000bf60 	.word	0x2000bf60
 e005b88:	0e005ad5 	.word	0x0e005ad5
 e005b8c:	0e005add 	.word	0x0e005add
 e005b90:	0e005ae1 	.word	0x0e005ae1
 e005b94:	2000be1c 	.word	0x2000be1c
 e005b98:	2000be08 	.word	0x2000be08

0e005b9c <wifi_config_autoreconnect>:
 e005b9c:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
 e005ba0:	4770      	bx	lr
 e005ba2:	bf00      	nop

0e005ba4 <_inic_ipc_ip_addr_update_in_wowlan>:
 e005ba4:	2001      	movs	r0, #1
 e005ba6:	4770      	bx	lr

0e005ba8 <inic_api_host_task_h>:
 e005ba8:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e005bac:	4fb7      	ldr	r7, [pc, #732]	; (e005e8c <inic_api_host_task_h+0x2e4>)
 e005bae:	b083      	sub	sp, #12
 e005bb0:	f8df a2ec 	ldr.w	sl, [pc, #748]	; e005ea0 <inic_api_host_task_h+0x2f8>
 e005bb4:	4db6      	ldr	r5, [pc, #728]	; (e005e90 <inic_api_host_task_h+0x2e8>)
 e005bb6:	f8df 92ec 	ldr.w	r9, [pc, #748]	; e005ea4 <inic_api_host_task_h+0x2fc>
 e005bba:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
 e005bbe:	6838      	ldr	r0, [r7, #0]
 e005bc0:	f7fc fff4 	bl	e002bac <rtos_sema_take>
 e005bc4:	2107      	movs	r1, #7
 e005bc6:	2000      	movs	r0, #0
 e005bc8:	f7fb ff50 	bl	e001a6c <ipc_get_message>
 e005bcc:	696b      	ldr	r3, [r5, #20]
 e005bce:	6844      	ldr	r4, [r0, #4]
 e005bd0:	03da      	lsls	r2, r3, #15
 e005bd2:	d513      	bpl.n	e005bfc <inic_api_host_task_h+0x54>
 e005bd4:	f014 011f 	ands.w	r1, r4, #31
 e005bd8:	f000 8131 	beq.w	e005e3e <inic_api_host_task_h+0x296>
 e005bdc:	f024 031f 	bic.w	r3, r4, #31
 e005be0:	3140      	adds	r1, #64	; 0x40
 e005be2:	f3bf 8f4f 	dsb	sy
 e005be6:	4419      	add	r1, r3
 e005be8:	f8c5 325c 	str.w	r3, [r5, #604]	; 0x25c
 e005bec:	3320      	adds	r3, #32
 e005bee:	1aca      	subs	r2, r1, r3
 e005bf0:	2a00      	cmp	r2, #0
 e005bf2:	dcf9      	bgt.n	e005be8 <inic_api_host_task_h+0x40>
 e005bf4:	f3bf 8f4f 	dsb	sy
 e005bf8:	f3bf 8f6f 	isb	sy
 e005bfc:	b91c      	cbnz	r4, e005c06 <inic_api_host_task_h+0x5e>
 e005bfe:	4648      	mov	r0, r9
 e005c00:	f001 fa10 	bl	e007024 <DiagPrintf_minimal>
 e005c04:	e7d9      	b.n	e005bba <inic_api_host_task_h+0x12>
 e005c06:	6821      	ldr	r1, [r4, #0]
 e005c08:	1e4b      	subs	r3, r1, #1
 e005c0a:	2b08      	cmp	r3, #8
 e005c0c:	f200 81b3 	bhi.w	e005f76 <inic_api_host_task_h+0x3ce>
 e005c10:	e8df f013 	tbh	[pc, r3, lsl #1]
 e005c14:	00b7010c 	.word	0x00b7010c
 e005c18:	004f006a 	.word	0x004f006a
 e005c1c:	00d60102 	.word	0x00d60102
 e005c20:	004f0009 	.word	0x004f0009
 e005c24:	004f      	.short	0x004f
 e005c26:	f8da 0000 	ldr.w	r0, [sl]
 e005c2a:	2800      	cmp	r0, #0
 e005c2c:	d040      	beq.n	e005cb0 <inic_api_host_task_h+0x108>
 e005c2e:	696b      	ldr	r3, [r5, #20]
 e005c30:	6860      	ldr	r0, [r4, #4]
 e005c32:	03d9      	lsls	r1, r3, #15
 e005c34:	d513      	bpl.n	e005c5e <inic_api_host_task_h+0xb6>
 e005c36:	f010 011f 	ands.w	r1, r0, #31
 e005c3a:	f000 8108 	beq.w	e005e4e <inic_api_host_task_h+0x2a6>
 e005c3e:	3110      	adds	r1, #16
 e005c40:	f020 031f 	bic.w	r3, r0, #31
 e005c44:	f3bf 8f4f 	dsb	sy
 e005c48:	4419      	add	r1, r3
 e005c4a:	f8c5 325c 	str.w	r3, [r5, #604]	; 0x25c
 e005c4e:	3320      	adds	r3, #32
 e005c50:	1aca      	subs	r2, r1, r3
 e005c52:	2a00      	cmp	r2, #0
 e005c54:	dcf9      	bgt.n	e005c4a <inic_api_host_task_h+0xa2>
 e005c56:	f3bf 8f4f 	dsb	sy
 e005c5a:	f3bf 8f6f 	isb	sy
 e005c5e:	696b      	ldr	r3, [r5, #20]
 e005c60:	03da      	lsls	r2, r3, #15
 e005c62:	d521      	bpl.n	e005ca8 <inic_api_host_task_h+0x100>
 e005c64:	e9d0 2302 	ldrd	r2, r3, [r0, #8]
 e005c68:	ea02 0103 	and.w	r1, r2, r3
 e005c6c:	3101      	adds	r1, #1
 e005c6e:	f000 8166 	beq.w	e005f3e <inic_api_host_task_h+0x396>
 e005c72:	f012 011f 	ands.w	r1, r2, #31
 e005c76:	d002      	beq.n	e005c7e <inic_api_host_task_h+0xd6>
 e005c78:	440b      	add	r3, r1
 e005c7a:	f022 021f 	bic.w	r2, r2, #31
 e005c7e:	f3bf 8f4f 	dsb	sy
 e005c82:	2b00      	cmp	r3, #0
 e005c84:	dd0c      	ble.n	e005ca0 <inic_api_host_task_h+0xf8>
 e005c86:	1e59      	subs	r1, r3, #1
 e005c88:	f102 0320 	add.w	r3, r2, #32
 e005c8c:	f021 011f 	bic.w	r1, r1, #31
 e005c90:	4419      	add	r1, r3
 e005c92:	428b      	cmp	r3, r1
 e005c94:	f8c5 225c 	str.w	r2, [r5, #604]	; 0x25c
 e005c98:	461a      	mov	r2, r3
 e005c9a:	f103 0320 	add.w	r3, r3, #32
 e005c9e:	d1f8      	bne.n	e005c92 <inic_api_host_task_h+0xea>
 e005ca0:	f3bf 8f4f 	dsb	sy
 e005ca4:	f3bf 8f6f 	isb	sy
 e005ca8:	f8da 3000 	ldr.w	r3, [sl]
 e005cac:	6860      	ldr	r0, [r4, #4]
 e005cae:	4798      	blx	r3
 e005cb0:	6220      	str	r0, [r4, #32]
 e005cb2:	2300      	movs	r3, #0
 e005cb4:	6023      	str	r3, [r4, #0]
 e005cb6:	696b      	ldr	r3, [r5, #20]
 e005cb8:	03db      	lsls	r3, r3, #15
 e005cba:	f57f af7e 	bpl.w	e005bba <inic_api_host_task_h+0x12>
 e005cbe:	f014 021f 	ands.w	r2, r4, #31
 e005cc2:	f000 80bf 	beq.w	e005e44 <inic_api_host_task_h+0x29c>
 e005cc6:	f024 041f 	bic.w	r4, r4, #31
 e005cca:	3240      	adds	r2, #64	; 0x40
 e005ccc:	f3bf 8f4f 	dsb	sy
 e005cd0:	4422      	add	r2, r4
 e005cd2:	f8c5 4268 	str.w	r4, [r5, #616]	; 0x268
 e005cd6:	3420      	adds	r4, #32
 e005cd8:	1b13      	subs	r3, r2, r4
 e005cda:	2b00      	cmp	r3, #0
 e005cdc:	dcf9      	bgt.n	e005cd2 <inic_api_host_task_h+0x12a>
 e005cde:	f3bf 8f4f 	dsb	sy
 e005ce2:	f3bf 8f6f 	isb	sy
 e005ce6:	e768      	b.n	e005bba <inic_api_host_task_h+0x12>
 e005ce8:	696b      	ldr	r3, [r5, #20]
 e005cea:	03de      	lsls	r6, r3, #15
 e005cec:	e9d4 0c04 	ldrd	r0, ip, [r4, #16]
 e005cf0:	d521      	bpl.n	e005d36 <inic_api_host_task_h+0x18e>
 e005cf2:	e9d4 2302 	ldrd	r2, r3, [r4, #8]
 e005cf6:	ea02 0103 	and.w	r1, r2, r3
 e005cfa:	3101      	adds	r1, #1
 e005cfc:	f000 80aa 	beq.w	e005e54 <inic_api_host_task_h+0x2ac>
 e005d00:	f012 011f 	ands.w	r1, r2, #31
 e005d04:	d002      	beq.n	e005d0c <inic_api_host_task_h+0x164>
 e005d06:	440b      	add	r3, r1
 e005d08:	f022 021f 	bic.w	r2, r2, #31
 e005d0c:	f3bf 8f4f 	dsb	sy
 e005d10:	2b00      	cmp	r3, #0
 e005d12:	dd0c      	ble.n	e005d2e <inic_api_host_task_h+0x186>
 e005d14:	1e59      	subs	r1, r3, #1
 e005d16:	f102 0320 	add.w	r3, r2, #32
 e005d1a:	f021 011f 	bic.w	r1, r1, #31
 e005d1e:	4419      	add	r1, r3
 e005d20:	428b      	cmp	r3, r1
 e005d22:	f8c5 225c 	str.w	r2, [r5, #604]	; 0x25c
 e005d26:	461a      	mov	r2, r3
 e005d28:	f103 0320 	add.w	r3, r3, #32
 e005d2c:	d1f8      	bne.n	e005d20 <inic_api_host_task_h+0x178>
 e005d2e:	f3bf 8f4f 	dsb	sy
 e005d32:	f3bf 8f6f 	isb	sy
 e005d36:	696b      	ldr	r3, [r5, #20]
 e005d38:	03d9      	lsls	r1, r3, #15
 e005d3a:	d5ba      	bpl.n	e005cb2 <inic_api_host_task_h+0x10a>
 e005d3c:	ea00 030c 	and.w	r3, r0, ip
 e005d40:	3301      	adds	r3, #1
 e005d42:	f000 80dc 	beq.w	e005efe <inic_api_host_task_h+0x356>
 e005d46:	f010 031f 	ands.w	r3, r0, #31
 e005d4a:	d002      	beq.n	e005d52 <inic_api_host_task_h+0x1aa>
 e005d4c:	449c      	add	ip, r3
 e005d4e:	f020 001f 	bic.w	r0, r0, #31
 e005d52:	f3bf 8f4f 	dsb	sy
 e005d56:	f1bc 0f00 	cmp.w	ip, #0
 e005d5a:	dd0d      	ble.n	e005d78 <inic_api_host_task_h+0x1d0>
 e005d5c:	f10c 32ff 	add.w	r2, ip, #4294967295	; 0xffffffff
 e005d60:	f100 0320 	add.w	r3, r0, #32
 e005d64:	f022 021f 	bic.w	r2, r2, #31
 e005d68:	441a      	add	r2, r3
 e005d6a:	4293      	cmp	r3, r2
 e005d6c:	f8c5 025c 	str.w	r0, [r5, #604]	; 0x25c
 e005d70:	4618      	mov	r0, r3
 e005d72:	f103 0320 	add.w	r3, r3, #32
 e005d76:	d1f8      	bne.n	e005d6a <inic_api_host_task_h+0x1c2>
 e005d78:	f3bf 8f4f 	dsb	sy
 e005d7c:	f3bf 8f6f 	isb	sy
 e005d80:	e797      	b.n	e005cb2 <inic_api_host_task_h+0x10a>
 e005d82:	696b      	ldr	r3, [r5, #20]
 e005d84:	03db      	lsls	r3, r3, #15
 e005d86:	e9d4 0101 	ldrd	r0, r1, [r4, #4]
 e005d8a:	d512      	bpl.n	e005db2 <inic_api_host_task_h+0x20a>
 e005d8c:	f010 061f 	ands.w	r6, r0, #31
 e005d90:	d05a      	beq.n	e005e48 <inic_api_host_task_h+0x2a0>
 e005d92:	3635      	adds	r6, #53	; 0x35
 e005d94:	f020 031f 	bic.w	r3, r0, #31
 e005d98:	f3bf 8f4f 	dsb	sy
 e005d9c:	441e      	add	r6, r3
 e005d9e:	f8c5 325c 	str.w	r3, [r5, #604]	; 0x25c
 e005da2:	3320      	adds	r3, #32
 e005da4:	1af2      	subs	r2, r6, r3
 e005da6:	2a00      	cmp	r2, #0
 e005da8:	dcf9      	bgt.n	e005d9e <inic_api_host_task_h+0x1f6>
 e005daa:	f3bf 8f4f 	dsb	sy
 e005dae:	f3bf 8f6f 	isb	sy
 e005db2:	4b38      	ldr	r3, [pc, #224]	; (e005e94 <inic_api_host_task_h+0x2ec>)
 e005db4:	681b      	ldr	r3, [r3, #0]
 e005db6:	2b00      	cmp	r3, #0
 e005db8:	f43f af7b 	beq.w	e005cb2 <inic_api_host_task_h+0x10a>
 e005dbc:	4798      	blx	r3
 e005dbe:	e778      	b.n	e005cb2 <inic_api_host_task_h+0x10a>
 e005dc0:	696a      	ldr	r2, [r5, #20]
 e005dc2:	68a1      	ldr	r1, [r4, #8]
 e005dc4:	03d2      	lsls	r2, r2, #15
 e005dc6:	7920      	ldrb	r0, [r4, #4]
 e005dc8:	460e      	mov	r6, r1
 e005dca:	e9d4 8303 	ldrd	r8, r3, [r4, #12]
 e005dce:	d51f      	bpl.n	e005e10 <inic_api_host_task_h+0x268>
 e005dd0:	ea01 0208 	and.w	r2, r1, r8
 e005dd4:	3201      	adds	r2, #1
 e005dd6:	d069      	beq.n	e005eac <inic_api_host_task_h+0x304>
 e005dd8:	f011 021f 	ands.w	r2, r1, #31
 e005ddc:	d064      	beq.n	e005ea8 <inic_api_host_task_h+0x300>
 e005dde:	4442      	add	r2, r8
 e005de0:	f021 061f 	bic.w	r6, r1, #31
 e005de4:	f3bf 8f4f 	dsb	sy
 e005de8:	2a00      	cmp	r2, #0
 e005dea:	dd0d      	ble.n	e005e08 <inic_api_host_task_h+0x260>
 e005dec:	f102 3cff 	add.w	ip, r2, #4294967295	; 0xffffffff
 e005df0:	f106 0220 	add.w	r2, r6, #32
 e005df4:	f02c 0c1f 	bic.w	ip, ip, #31
 e005df8:	4494      	add	ip, r2
 e005dfa:	4594      	cmp	ip, r2
 e005dfc:	f8c5 625c 	str.w	r6, [r5, #604]	; 0x25c
 e005e00:	4616      	mov	r6, r2
 e005e02:	f102 0220 	add.w	r2, r2, #32
 e005e06:	d1f8      	bne.n	e005dfa <inic_api_host_task_h+0x252>
 e005e08:	f3bf 8f4f 	dsb	sy
 e005e0c:	f3bf 8f6f 	isb	sy
 e005e10:	4642      	mov	r2, r8
 e005e12:	f7ff f9ff 	bl	e005214 <wifi_indication>
 e005e16:	e74c      	b.n	e005cb2 <inic_api_host_task_h+0x10a>
 e005e18:	4b1f      	ldr	r3, [pc, #124]	; (e005e98 <inic_api_host_task_h+0x2f0>)
 e005e1a:	681b      	ldr	r3, [r3, #0]
 e005e1c:	2b00      	cmp	r3, #0
 e005e1e:	f43f af48 	beq.w	e005cb2 <inic_api_host_task_h+0x10a>
 e005e22:	f994 1008 	ldrsb.w	r1, [r4, #8]
 e005e26:	7920      	ldrb	r0, [r4, #4]
 e005e28:	4798      	blx	r3
 e005e2a:	e742      	b.n	e005cb2 <inic_api_host_task_h+0x10a>
 e005e2c:	4b1b      	ldr	r3, [pc, #108]	; (e005e9c <inic_api_host_task_h+0x2f4>)
 e005e2e:	681b      	ldr	r3, [r3, #0]
 e005e30:	2b00      	cmp	r3, #0
 e005e32:	f43f af3e 	beq.w	e005cb2 <inic_api_host_task_h+0x10a>
 e005e36:	e9d4 0101 	ldrd	r0, r1, [r4, #4]
 e005e3a:	4798      	blx	r3
 e005e3c:	e739      	b.n	e005cb2 <inic_api_host_task_h+0x10a>
 e005e3e:	4623      	mov	r3, r4
 e005e40:	2140      	movs	r1, #64	; 0x40
 e005e42:	e6ce      	b.n	e005be2 <inic_api_host_task_h+0x3a>
 e005e44:	2240      	movs	r2, #64	; 0x40
 e005e46:	e741      	b.n	e005ccc <inic_api_host_task_h+0x124>
 e005e48:	4603      	mov	r3, r0
 e005e4a:	2635      	movs	r6, #53	; 0x35
 e005e4c:	e7a4      	b.n	e005d98 <inic_api_host_task_h+0x1f0>
 e005e4e:	4603      	mov	r3, r0
 e005e50:	2110      	movs	r1, #16
 e005e52:	e6f7      	b.n	e005c44 <inic_api_host_task_h+0x9c>
 e005e54:	2300      	movs	r3, #0
 e005e56:	f8c5 3084 	str.w	r3, [r5, #132]	; 0x84
 e005e5a:	f3bf 8f4f 	dsb	sy
 e005e5e:	f8d5 3080 	ldr.w	r3, [r5, #128]	; 0x80
 e005e62:	f643 78e0 	movw	r8, #16352	; 0x3fe0
 e005e66:	f3c3 364e 	ubfx	r6, r3, #13, #15
 e005e6a:	f3c3 0ec9 	ubfx	lr, r3, #3, #10
 e005e6e:	0176      	lsls	r6, r6, #5
 e005e70:	ea06 0108 	and.w	r1, r6, r8
 e005e74:	4673      	mov	r3, lr
 e005e76:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
 e005e7a:	3b01      	subs	r3, #1
 e005e7c:	f8c5 2260 	str.w	r2, [r5, #608]	; 0x260
 e005e80:	d2f9      	bcs.n	e005e76 <inic_api_host_task_h+0x2ce>
 e005e82:	3e20      	subs	r6, #32
 e005e84:	f116 0f20 	cmn.w	r6, #32
 e005e88:	d1f2      	bne.n	e005e70 <inic_api_host_task_h+0x2c8>
 e005e8a:	e750      	b.n	e005d2e <inic_api_host_task_h+0x186>
 e005e8c:	2000be28 	.word	0x2000be28
 e005e90:	e000ed00 	.word	0xe000ed00
 e005e94:	2000be14 	.word	0x2000be14
 e005e98:	2000be20 	.word	0x2000be20
 e005e9c:	2000be18 	.word	0x2000be18
 e005ea0:	2000be10 	.word	0x2000be10
 e005ea4:	0e009518 	.word	0x0e009518
 e005ea8:	4642      	mov	r2, r8
 e005eaa:	e79b      	b.n	e005de4 <inic_api_host_task_h+0x23c>
 e005eac:	2200      	movs	r2, #0
 e005eae:	f8c5 2084 	str.w	r2, [r5, #132]	; 0x84
 e005eb2:	f3bf 8f4f 	dsb	sy
 e005eb6:	f8d5 2080 	ldr.w	r2, [r5, #128]	; 0x80
 e005eba:	f643 7be0 	movw	fp, #16352	; 0x3fe0
 e005ebe:	f8cd 8004 	str.w	r8, [sp, #4]
 e005ec2:	4688      	mov	r8, r1
 e005ec4:	f3c2 3e4e 	ubfx	lr, r2, #13, #15
 e005ec8:	f3c2 02c9 	ubfx	r2, r2, #3, #10
 e005ecc:	ea4f 1e4e 	mov.w	lr, lr, lsl #5
 e005ed0:	4611      	mov	r1, r2
 e005ed2:	ea0e 0c0b 	and.w	ip, lr, fp
 e005ed6:	460a      	mov	r2, r1
 e005ed8:	ea4c 7682 	orr.w	r6, ip, r2, lsl #30
 e005edc:	3a01      	subs	r2, #1
 e005ede:	f8c5 6260 	str.w	r6, [r5, #608]	; 0x260
 e005ee2:	d2f9      	bcs.n	e005ed8 <inic_api_host_task_h+0x330>
 e005ee4:	f1ae 0e20 	sub.w	lr, lr, #32
 e005ee8:	f11e 0f20 	cmn.w	lr, #32
 e005eec:	d1f1      	bne.n	e005ed2 <inic_api_host_task_h+0x32a>
 e005eee:	4641      	mov	r1, r8
 e005ef0:	f8dd 8004 	ldr.w	r8, [sp, #4]
 e005ef4:	f3bf 8f4f 	dsb	sy
 e005ef8:	f3bf 8f6f 	isb	sy
 e005efc:	e788      	b.n	e005e10 <inic_api_host_task_h+0x268>
 e005efe:	2300      	movs	r3, #0
 e005f00:	f8c5 3084 	str.w	r3, [r5, #132]	; 0x84
 e005f04:	f3bf 8f4f 	dsb	sy
 e005f08:	f8d5 6080 	ldr.w	r6, [r5, #128]	; 0x80
 e005f0c:	f643 7ce0 	movw	ip, #16352	; 0x3fe0
 e005f10:	f3c6 304e 	ubfx	r0, r6, #13, #15
 e005f14:	f3c6 06c9 	ubfx	r6, r6, #3, #10
 e005f18:	0140      	lsls	r0, r0, #5
 e005f1a:	ea00 010c 	and.w	r1, r0, ip
 e005f1e:	4633      	mov	r3, r6
 e005f20:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
 e005f24:	3b01      	subs	r3, #1
 e005f26:	f8c5 2260 	str.w	r2, [r5, #608]	; 0x260
 e005f2a:	d2f9      	bcs.n	e005f20 <inic_api_host_task_h+0x378>
 e005f2c:	3820      	subs	r0, #32
 e005f2e:	f110 0f20 	cmn.w	r0, #32
 e005f32:	d1f2      	bne.n	e005f1a <inic_api_host_task_h+0x372>
 e005f34:	f3bf 8f4f 	dsb	sy
 e005f38:	f3bf 8f6f 	isb	sy
 e005f3c:	e6b9      	b.n	e005cb2 <inic_api_host_task_h+0x10a>
 e005f3e:	2300      	movs	r3, #0
 e005f40:	f8c5 3084 	str.w	r3, [r5, #132]	; 0x84
 e005f44:	f3bf 8f4f 	dsb	sy
 e005f48:	f8d5 6080 	ldr.w	r6, [r5, #128]	; 0x80
 e005f4c:	f643 7ce0 	movw	ip, #16352	; 0x3fe0
 e005f50:	f3c6 304e 	ubfx	r0, r6, #13, #15
 e005f54:	f3c6 06c9 	ubfx	r6, r6, #3, #10
 e005f58:	0140      	lsls	r0, r0, #5
 e005f5a:	ea00 010c 	and.w	r1, r0, ip
 e005f5e:	4633      	mov	r3, r6
 e005f60:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
 e005f64:	3b01      	subs	r3, #1
 e005f66:	f8c5 2260 	str.w	r2, [r5, #608]	; 0x260
 e005f6a:	d2f9      	bcs.n	e005f60 <inic_api_host_task_h+0x3b8>
 e005f6c:	3820      	subs	r0, #32
 e005f6e:	f110 0f20 	cmn.w	r0, #32
 e005f72:	d1f2      	bne.n	e005f5a <inic_api_host_task_h+0x3b2>
 e005f74:	e694      	b.n	e005ca0 <inic_api_host_task_h+0xf8>
 e005f76:	4802      	ldr	r0, [pc, #8]	; (e005f80 <inic_api_host_task_h+0x3d8>)
 e005f78:	f001 f854 	bl	e007024 <DiagPrintf_minimal>
 e005f7c:	e699      	b.n	e005cb2 <inic_api_host_task_h+0x10a>
 e005f7e:	bf00      	nop
 e005f80:	0e009544 	.word	0x0e009544

0e005f84 <inic_api_host_int_hdl>:
 e005f84:	4b01      	ldr	r3, [pc, #4]	; (e005f8c <inic_api_host_int_hdl+0x8>)
 e005f86:	6818      	ldr	r0, [r3, #0]
 e005f88:	f7fc be38 	b.w	e002bfc <rtos_sema_give>
 e005f8c:	2000be28 	.word	0x2000be28

0e005f90 <inic_api_host_message_send>:
 e005f90:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e005f94:	f8df b168 	ldr.w	fp, [pc, #360]	; e006100 <inic_api_host_message_send+0x170>
 e005f98:	b083      	sub	sp, #12
 e005f9a:	4688      	mov	r8, r1
 e005f9c:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
 e005fa0:	4d4e      	ldr	r5, [pc, #312]	; (e0060dc <inic_api_host_message_send+0x14c>)
 e005fa2:	f8df a144 	ldr.w	sl, [pc, #324]	; e0060e8 <inic_api_host_message_send+0x158>
 e005fa6:	4e4e      	ldr	r6, [pc, #312]	; (e0060e0 <inic_api_host_message_send+0x150>)
 e005fa8:	e9cd 0200 	strd	r0, r2, [sp]
 e005fac:	f8db 0000 	ldr.w	r0, [fp]
 e005fb0:	f7fc fdfc 	bl	e002bac <rtos_sema_take>
 e005fb4:	f44f 797a 	mov.w	r9, #1000	; 0x3e8
 e005fb8:	2400      	movs	r4, #0
 e005fba:	f105 0720 	add.w	r7, r5, #32
 e005fbe:	e00d      	b.n	e005fdc <inic_api_host_message_send+0x4c>
 e005fc0:	f7fc ff66 	bl	e002e90 <rtos_time_delay_ms>
 e005fc4:	1c61      	adds	r1, r4, #1
 e005fc6:	f44f 707a 	mov.w	r0, #1000	; 0x3e8
 e005fca:	f1b9 0901 	subs.w	r9, r9, #1
 e005fce:	fba6 3401 	umull	r3, r4, r6, r1
 e005fd2:	ea4f 1494 	mov.w	r4, r4, lsr #6
 e005fd6:	fb00 1414 	mls	r4, r0, r4, r1
 e005fda:	d075      	beq.n	e0060c8 <inic_api_host_message_send+0x138>
 e005fdc:	f8da 1014 	ldr.w	r1, [sl, #20]
 e005fe0:	03c8      	lsls	r0, r1, #15
 e005fe2:	d509      	bpl.n	e005ff8 <inic_api_host_message_send+0x68>
 e005fe4:	f3bf 8f4f 	dsb	sy
 e005fe8:	f8ca 525c 	str.w	r5, [sl, #604]	; 0x25c
 e005fec:	f8ca 725c 	str.w	r7, [sl, #604]	; 0x25c
 e005ff0:	f3bf 8f4f 	dsb	sy
 e005ff4:	f3bf 8f6f 	isb	sy
 e005ff8:	6829      	ldr	r1, [r5, #0]
 e005ffa:	2001      	movs	r0, #1
 e005ffc:	2900      	cmp	r1, #0
 e005ffe:	d1df      	bne.n	e005fc0 <inic_api_host_message_send+0x30>
 e006000:	2240      	movs	r2, #64	; 0x40
 e006002:	4836      	ldr	r0, [pc, #216]	; (e0060dc <inic_api_host_message_send+0x14c>)
 e006004:	f001 fd00 	bl	e007a08 <____wrap_memset_veneer>
 e006008:	9b00      	ldr	r3, [sp, #0]
 e00600a:	602b      	str	r3, [r5, #0]
 e00600c:	f1b8 0f00 	cmp.w	r8, #0
 e006010:	d005      	beq.n	e00601e <inic_api_host_message_send+0x8e>
 e006012:	9b01      	ldr	r3, [sp, #4]
 e006014:	4641      	mov	r1, r8
 e006016:	4833      	ldr	r0, [pc, #204]	; (e0060e4 <inic_api_host_message_send+0x154>)
 e006018:	009a      	lsls	r2, r3, #2
 e00601a:	f001 fced 	bl	e0079f8 <____wrap_memcpy_veneer>
 e00601e:	4b32      	ldr	r3, [pc, #200]	; (e0060e8 <inic_api_host_message_send+0x158>)
 e006020:	695a      	ldr	r2, [r3, #20]
 e006022:	03d1      	lsls	r1, r2, #15
 e006024:	d50a      	bpl.n	e00603c <inic_api_host_message_send+0xac>
 e006026:	f3bf 8f4f 	dsb	sy
 e00602a:	4a30      	ldr	r2, [pc, #192]	; (e0060ec <inic_api_host_message_send+0x15c>)
 e00602c:	f8c3 5268 	str.w	r5, [r3, #616]	; 0x268
 e006030:	f8c3 2268 	str.w	r2, [r3, #616]	; 0x268
 e006034:	f3bf 8f4f 	dsb	sy
 e006038:	f3bf 8f6f 	isb	sy
 e00603c:	4e2c      	ldr	r6, [pc, #176]	; (e0060f0 <inic_api_host_message_send+0x160>)
 e00603e:	2210      	movs	r2, #16
 e006040:	2100      	movs	r1, #0
 e006042:	4630      	mov	r0, r6
 e006044:	f001 fce0 	bl	e007a08 <____wrap_memset_veneer>
 e006048:	2300      	movs	r3, #0
 e00604a:	e9c6 3500 	strd	r3, r5, [r6]
 e00604e:	2340      	movs	r3, #64	; 0x40
 e006050:	60b3      	str	r3, [r6, #8]
 e006052:	4b25      	ldr	r3, [pc, #148]	; (e0060e8 <inic_api_host_message_send+0x158>)
 e006054:	695a      	ldr	r2, [r3, #20]
 e006056:	03d2      	lsls	r2, r2, #15
 e006058:	d507      	bpl.n	e00606a <inic_api_host_message_send+0xda>
 e00605a:	f3bf 8f4f 	dsb	sy
 e00605e:	f8c3 6268 	str.w	r6, [r3, #616]	; 0x268
 e006062:	f3bf 8f4f 	dsb	sy
 e006066:	f3bf 8f6f 	isb	sy
 e00606a:	4a21      	ldr	r2, [pc, #132]	; (e0060f0 <inic_api_host_message_send+0x160>)
 e00606c:	2107      	movs	r1, #7
 e00606e:	2001      	movs	r0, #1
 e006070:	4e1d      	ldr	r6, [pc, #116]	; (e0060e8 <inic_api_host_message_send+0x158>)
 e006072:	4f1b      	ldr	r7, [pc, #108]	; (e0060e0 <inic_api_host_message_send+0x150>)
 e006074:	f7fb fc9e 	bl	e0019b4 <ipc_send_message>
 e006078:	e019      	b.n	e0060ae <inic_api_host_message_send+0x11e>
 e00607a:	f7fc ff09 	bl	e002e90 <rtos_time_delay_ms>
 e00607e:	6973      	ldr	r3, [r6, #20]
 e006080:	03db      	lsls	r3, r3, #15
 e006082:	d50b      	bpl.n	e00609c <inic_api_host_message_send+0x10c>
 e006084:	f3bf 8f4f 	dsb	sy
 e006088:	f105 0320 	add.w	r3, r5, #32
 e00608c:	f8c6 525c 	str.w	r5, [r6, #604]	; 0x25c
 e006090:	f8c6 325c 	str.w	r3, [r6, #604]	; 0x25c
 e006094:	f3bf 8f4f 	dsb	sy
 e006098:	f3bf 8f6f 	isb	sy
 e00609c:	1c63      	adds	r3, r4, #1
 e00609e:	f44f 727a 	mov.w	r2, #1000	; 0x3e8
 e0060a2:	fba7 1403 	umull	r1, r4, r7, r3
 e0060a6:	09a4      	lsrs	r4, r4, #6
 e0060a8:	fb02 3414 	mls	r4, r2, r4, r3
 e0060ac:	b18c      	cbz	r4, e0060d2 <inic_api_host_message_send+0x142>
 e0060ae:	682b      	ldr	r3, [r5, #0]
 e0060b0:	2001      	movs	r0, #1
 e0060b2:	2b00      	cmp	r3, #0
 e0060b4:	d1e1      	bne.n	e00607a <inic_api_host_message_send+0xea>
 e0060b6:	6aac      	ldr	r4, [r5, #40]	; 0x28
 e0060b8:	f8db 0000 	ldr.w	r0, [fp]
 e0060bc:	f7fc fd9e 	bl	e002bfc <rtos_sema_give>
 e0060c0:	4620      	mov	r0, r4
 e0060c2:	b003      	add	sp, #12
 e0060c4:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 e0060c8:	490a      	ldr	r1, [pc, #40]	; (e0060f4 <inic_api_host_message_send+0x164>)
 e0060ca:	480b      	ldr	r0, [pc, #44]	; (e0060f8 <inic_api_host_message_send+0x168>)
 e0060cc:	f000 ffaa 	bl	e007024 <DiagPrintf_minimal>
 e0060d0:	e770      	b.n	e005fb4 <inic_api_host_message_send+0x24>
 e0060d2:	4908      	ldr	r1, [pc, #32]	; (e0060f4 <inic_api_host_message_send+0x164>)
 e0060d4:	4809      	ldr	r0, [pc, #36]	; (e0060fc <inic_api_host_message_send+0x16c>)
 e0060d6:	f000 ffa5 	bl	e007024 <DiagPrintf_minimal>
 e0060da:	e7e8      	b.n	e0060ae <inic_api_host_message_send+0x11e>
 e0060dc:	2000be80 	.word	0x2000be80
 e0060e0:	10624dd3 	.word	0x10624dd3
 e0060e4:	2000be84 	.word	0x2000be84
 e0060e8:	e000ed00 	.word	0xe000ed00
 e0060ec:	2000bea0 	.word	0x2000bea0
 e0060f0:	2000be40 	.word	0x2000be40
 e0060f4:	0e009618 	.word	0x0e009618
 e0060f8:	0e009564 	.word	0x0e009564
 e0060fc:	0e009584 	.word	0x0e009584
 e006100:	2000be24 	.word	0x2000be24

0e006104 <inic_api_init_host>:
 e006104:	b510      	push	{r4, lr}
 e006106:	4c17      	ldr	r4, [pc, #92]	; (e006164 <inic_api_init_host+0x60>)
 e006108:	b082      	sub	sp, #8
 e00610a:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
 e00610e:	2100      	movs	r1, #0
 e006110:	4815      	ldr	r0, [pc, #84]	; (e006168 <inic_api_init_host+0x64>)
 e006112:	f7fc fd19 	bl	e002b48 <rtos_sema_create_static>
 e006116:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
 e00611a:	2100      	movs	r1, #0
 e00611c:	4620      	mov	r0, r4
 e00611e:	f7fc fd13 	bl	e002b48 <rtos_sema_create_static>
 e006122:	6820      	ldr	r0, [r4, #0]
 e006124:	f7fc fd6a 	bl	e002bfc <rtos_sema_give>
 e006128:	2300      	movs	r3, #0
 e00612a:	4910      	ldr	r1, [pc, #64]	; (e00616c <inic_api_init_host+0x68>)
 e00612c:	2001      	movs	r0, #1
 e00612e:	461a      	mov	r2, r3
 e006130:	9300      	str	r3, [sp, #0]
 e006132:	f7fb fe33 	bl	e001d9c <pmu_register_sleep_callback>
 e006136:	2403      	movs	r4, #3
 e006138:	f44f 6146 	mov.w	r1, #3168	; 0xc60
 e00613c:	2300      	movs	r3, #0
 e00613e:	4a0c      	ldr	r2, [pc, #48]	; (e006170 <inic_api_init_host+0x6c>)
 e006140:	4618      	mov	r0, r3
 e006142:	e9cd 1400 	strd	r1, r4, [sp]
 e006146:	490b      	ldr	r1, [pc, #44]	; (e006174 <inic_api_init_host+0x70>)
 e006148:	f7fc fe86 	bl	e002e58 <rtos_task_create>
 e00614c:	b908      	cbnz	r0, e006152 <inic_api_init_host+0x4e>
 e00614e:	b002      	add	sp, #8
 e006150:	bd10      	pop	{r4, pc}
 e006152:	4b09      	ldr	r3, [pc, #36]	; (e006178 <inic_api_init_host+0x74>)
 e006154:	2245      	movs	r2, #69	; 0x45
 e006156:	4909      	ldr	r1, [pc, #36]	; (e00617c <inic_api_init_host+0x78>)
 e006158:	2002      	movs	r0, #2
 e00615a:	b002      	add	sp, #8
 e00615c:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
 e006160:	f7fc b96c 	b.w	e00243c <rtk_log_write>
 e006164:	2000be24 	.word	0x2000be24
 e006168:	2000be28 	.word	0x2000be28
 e00616c:	0e005ba5 	.word	0x0e005ba5
 e006170:	0e005ba9 	.word	0x0e005ba9
 e006174:	0e0095a0 	.word	0x0e0095a0
 e006178:	0e0095b4 	.word	0x0e0095b4
 e00617c:	0e0095d8 	.word	0x0e0095d8

0e006180 <inic_mp_command>:
 e006180:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
 e006184:	4681      	mov	r9, r0
 e006186:	b087      	sub	sp, #28
 e006188:	f44f 5080 	mov.w	r0, #4096	; 0x1000
 e00618c:	4688      	mov	r8, r1
 e00618e:	4617      	mov	r7, r2
 e006190:	f7fc fcac 	bl	e002aec <rtos_mem_zmalloc>
 e006194:	2800      	cmp	r0, #0
 e006196:	f000 809b 	beq.w	e0062d0 <inic_mp_command+0x150>
 e00619a:	4b52      	ldr	r3, [pc, #328]	; (e0062e4 <inic_mp_command+0x164>)
 e00619c:	4606      	mov	r6, r0
 e00619e:	4649      	mov	r1, r9
 e0061a0:	4605      	mov	r5, r0
 e0061a2:	e9cd 9802 	strd	r9, r8, [sp, #8]
 e0061a6:	e9cd 7004 	strd	r7, r0, [sp, #16]
 e0061aa:	695b      	ldr	r3, [r3, #20]
 e0061ac:	03dc      	lsls	r4, r3, #15
 e0061ae:	d513      	bpl.n	e0061d8 <inic_mp_command+0x58>
 e0061b0:	f010 041f 	ands.w	r4, r0, #31
 e0061b4:	d15d      	bne.n	e006272 <inic_mp_command+0xf2>
 e0061b6:	4603      	mov	r3, r0
 e0061b8:	f44f 5480 	mov.w	r4, #4096	; 0x1000
 e0061bc:	f3bf 8f4f 	dsb	sy
 e0061c0:	4848      	ldr	r0, [pc, #288]	; (e0062e4 <inic_mp_command+0x164>)
 e0061c2:	441c      	add	r4, r3
 e0061c4:	f8c0 3268 	str.w	r3, [r0, #616]	; 0x268
 e0061c8:	3320      	adds	r3, #32
 e0061ca:	1ae2      	subs	r2, r4, r3
 e0061cc:	2a00      	cmp	r2, #0
 e0061ce:	dcf9      	bgt.n	e0061c4 <inic_mp_command+0x44>
 e0061d0:	f3bf 8f4f 	dsb	sy
 e0061d4:	f3bf 8f6f 	isb	sy
 e0061d8:	4c42      	ldr	r4, [pc, #264]	; (e0062e4 <inic_mp_command+0x164>)
 e0061da:	6963      	ldr	r3, [r4, #20]
 e0061dc:	03d8      	lsls	r0, r3, #15
 e0061de:	d51e      	bpl.n	e00621e <inic_mp_command+0x9e>
 e0061e0:	ea09 0308 	and.w	r3, r9, r8
 e0061e4:	3301      	adds	r3, #1
 e0061e6:	d049      	beq.n	e00627c <inic_mp_command+0xfc>
 e0061e8:	f019 031f 	ands.w	r3, r9, #31
 e0061ec:	d167      	bne.n	e0062be <inic_mp_command+0x13e>
 e0061ee:	f3bf 8f4f 	dsb	sy
 e0061f2:	f1b8 0f00 	cmp.w	r8, #0
 e0061f6:	dd0e      	ble.n	e006216 <inic_mp_command+0x96>
 e0061f8:	f108 32ff 	add.w	r2, r8, #4294967295	; 0xffffffff
 e0061fc:	f101 0320 	add.w	r3, r1, #32
 e006200:	4838      	ldr	r0, [pc, #224]	; (e0062e4 <inic_mp_command+0x164>)
 e006202:	f022 021f 	bic.w	r2, r2, #31
 e006206:	441a      	add	r2, r3
 e006208:	4293      	cmp	r3, r2
 e00620a:	f8c0 1268 	str.w	r1, [r0, #616]	; 0x268
 e00620e:	4619      	mov	r1, r3
 e006210:	f103 0320 	add.w	r3, r3, #32
 e006214:	d1f8      	bne.n	e006208 <inic_mp_command+0x88>
 e006216:	f3bf 8f4f 	dsb	sy
 e00621a:	f3bf 8f6f 	isb	sy
 e00621e:	2204      	movs	r2, #4
 e006220:	a902      	add	r1, sp, #8
 e006222:	2050      	movs	r0, #80	; 0x50
 e006224:	f7ff feb4 	bl	e005f90 <inic_api_host_message_send>
 e006228:	b1ef      	cbz	r7, e006266 <inic_mp_command+0xe6>
 e00622a:	4b2e      	ldr	r3, [pc, #184]	; (e0062e4 <inic_mp_command+0x164>)
 e00622c:	695b      	ldr	r3, [r3, #20]
 e00622e:	03db      	lsls	r3, r3, #15
 e006230:	d512      	bpl.n	e006258 <inic_mp_command+0xd8>
 e006232:	f016 021f 	ands.w	r2, r6, #31
 e006236:	d146      	bne.n	e0062c6 <inic_mp_command+0x146>
 e006238:	f44f 5280 	mov.w	r2, #4096	; 0x1000
 e00623c:	f3bf 8f4f 	dsb	sy
 e006240:	4928      	ldr	r1, [pc, #160]	; (e0062e4 <inic_mp_command+0x164>)
 e006242:	442a      	add	r2, r5
 e006244:	f8c1 525c 	str.w	r5, [r1, #604]	; 0x25c
 e006248:	3520      	adds	r5, #32
 e00624a:	1b53      	subs	r3, r2, r5
 e00624c:	2b00      	cmp	r3, #0
 e00624e:	dcf9      	bgt.n	e006244 <inic_mp_command+0xc4>
 e006250:	f3bf 8f4f 	dsb	sy
 e006254:	f3bf 8f6f 	isb	sy
 e006258:	4b23      	ldr	r3, [pc, #140]	; (e0062e8 <inic_mp_command+0x168>)
 e00625a:	2241      	movs	r2, #65	; 0x41
 e00625c:	4923      	ldr	r1, [pc, #140]	; (e0062ec <inic_mp_command+0x16c>)
 e00625e:	2001      	movs	r0, #1
 e006260:	9600      	str	r6, [sp, #0]
 e006262:	f7fc f8eb 	bl	e00243c <rtk_log_write>
 e006266:	4630      	mov	r0, r6
 e006268:	f7fc fc4c 	bl	e002b04 <rtos_mem_free>
 e00626c:	b007      	add	sp, #28
 e00626e:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 e006272:	f504 5480 	add.w	r4, r4, #4096	; 0x1000
 e006276:	f020 031f 	bic.w	r3, r0, #31
 e00627a:	e79f      	b.n	e0061bc <inic_mp_command+0x3c>
 e00627c:	2300      	movs	r3, #0
 e00627e:	f8c4 3084 	str.w	r3, [r4, #132]	; 0x84
 e006282:	f3bf 8f4f 	dsb	sy
 e006286:	f8d4 3080 	ldr.w	r3, [r4, #128]	; 0x80
 e00628a:	f643 7ee0 	movw	lr, #16352	; 0x3fe0
 e00628e:	f3c3 304e 	ubfx	r0, r3, #13, #15
 e006292:	f3c3 0cc9 	ubfx	ip, r3, #3, #10
 e006296:	0140      	lsls	r0, r0, #5
 e006298:	ea00 020e 	and.w	r2, r0, lr
 e00629c:	4663      	mov	r3, ip
 e00629e:	ea42 7183 	orr.w	r1, r2, r3, lsl #30
 e0062a2:	3b01      	subs	r3, #1
 e0062a4:	f8c4 126c 	str.w	r1, [r4, #620]	; 0x26c
 e0062a8:	1c59      	adds	r1, r3, #1
 e0062aa:	d1f8      	bne.n	e00629e <inic_mp_command+0x11e>
 e0062ac:	3820      	subs	r0, #32
 e0062ae:	f110 0f20 	cmn.w	r0, #32
 e0062b2:	d1f1      	bne.n	e006298 <inic_mp_command+0x118>
 e0062b4:	f3bf 8f4f 	dsb	sy
 e0062b8:	f3bf 8f6f 	isb	sy
 e0062bc:	e7af      	b.n	e00621e <inic_mp_command+0x9e>
 e0062be:	4498      	add	r8, r3
 e0062c0:	f029 011f 	bic.w	r1, r9, #31
 e0062c4:	e793      	b.n	e0061ee <inic_mp_command+0x6e>
 e0062c6:	f026 051f 	bic.w	r5, r6, #31
 e0062ca:	f502 5280 	add.w	r2, r2, #4096	; 0x1000
 e0062ce:	e7b5      	b.n	e00623c <inic_mp_command+0xbc>
 e0062d0:	4b07      	ldr	r3, [pc, #28]	; (e0062f0 <inic_mp_command+0x170>)
 e0062d2:	2245      	movs	r2, #69	; 0x45
 e0062d4:	4905      	ldr	r1, [pc, #20]	; (e0062ec <inic_mp_command+0x16c>)
 e0062d6:	2002      	movs	r0, #2
 e0062d8:	b007      	add	sp, #28
 e0062da:	e8bd 43f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, lr}
 e0062de:	f7fc b8ad 	b.w	e00243c <rtk_log_write>
 e0062e2:	bf00      	nop
 e0062e4:	e000ed00 	.word	0xe000ed00
 e0062e8:	0e009600 	.word	0x0e009600
 e0062ec:	0e0095d8 	.word	0x0e0095d8
 e0062f0:	0e0095e0 	.word	0x0e0095e0

0e0062f4 <inic_host_init>:
 e0062f4:	b508      	push	{r3, lr}
 e0062f6:	f000 f8a9 	bl	e00644c <inic_host_init_priv>
 e0062fa:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
 e0062fe:	f7ff bf01 	b.w	e006104 <inic_api_init_host>
 e006302:	bf00      	nop

0e006304 <inic_host_init_skb>:
 e006304:	b5f0      	push	{r4, r5, r6, r7, lr}
 e006306:	4e49      	ldr	r6, [pc, #292]	; (e00642c <inic_host_init_skb+0x128>)
 e006308:	b083      	sub	sp, #12
 e00630a:	6833      	ldr	r3, [r6, #0]
 e00630c:	b913      	cbnz	r3, e006314 <inic_host_init_skb+0x10>
 e00630e:	4c48      	ldr	r4, [pc, #288]	; (e006430 <inic_host_init_skb+0x12c>)
 e006310:	6823      	ldr	r3, [r4, #0]
 e006312:	b143      	cbz	r3, e006326 <inic_host_init_skb+0x22>
 e006314:	4b47      	ldr	r3, [pc, #284]	; (e006434 <inic_host_init_skb+0x130>)
 e006316:	2245      	movs	r2, #69	; 0x45
 e006318:	4947      	ldr	r1, [pc, #284]	; (e006438 <inic_host_init_skb+0x134>)
 e00631a:	2002      	movs	r0, #2
 e00631c:	b003      	add	sp, #12
 e00631e:	e8bd 40f0 	ldmia.w	sp!, {r4, r5, r6, r7, lr}
 e006322:	f7fc b88b 	b.w	e00243c <rtk_log_write>
 e006326:	4d45      	ldr	r5, [pc, #276]	; (e00643c <inic_host_init_skb+0x138>)
 e006328:	6a68      	ldr	r0, [r5, #36]	; 0x24
 e00632a:	0180      	lsls	r0, r0, #6
 e00632c:	f7fc fbde 	bl	e002aec <rtos_mem_zmalloc>
 e006330:	6a6b      	ldr	r3, [r5, #36]	; 0x24
 e006332:	6030      	str	r0, [r6, #0]
 e006334:	f44f 60d8 	mov.w	r0, #1728	; 0x6c0
 e006338:	fb03 f000 	mul.w	r0, r3, r0
 e00633c:	f7fc fbd6 	bl	e002aec <rtos_mem_zmalloc>
 e006340:	6831      	ldr	r1, [r6, #0]
 e006342:	6020      	str	r0, [r4, #0]
 e006344:	2800      	cmp	r0, #0
 e006346:	d066      	beq.n	e006416 <inic_host_init_skb+0x112>
 e006348:	2900      	cmp	r1, #0
 e00634a:	d064      	beq.n	e006416 <inic_host_init_skb+0x112>
 e00634c:	6a68      	ldr	r0, [r5, #36]	; 0x24
 e00634e:	2800      	cmp	r0, #0
 e006350:	dd17      	ble.n	e006382 <inic_host_init_skb+0x7e>
 e006352:	2300      	movs	r3, #0
 e006354:	461a      	mov	r2, r3
 e006356:	eb01 1082 	add.w	r0, r1, r2, lsl #6
 e00635a:	ea4f 1c82 	mov.w	ip, r2, lsl #6
 e00635e:	3201      	adds	r2, #1
 e006360:	f841 000c 	str.w	r0, [r1, ip]
 e006364:	6820      	ldr	r0, [r4, #0]
 e006366:	6831      	ldr	r1, [r6, #0]
 e006368:	18c7      	adds	r7, r0, r3
 e00636a:	4461      	add	r1, ip
 e00636c:	6049      	str	r1, [r1, #4]
 e00636e:	50c7      	str	r7, [r0, r3]
 e006370:	6821      	ldr	r1, [r4, #0]
 e006372:	4419      	add	r1, r3
 e006374:	f503 63d8 	add.w	r3, r3, #1728	; 0x6c0
 e006378:	6049      	str	r1, [r1, #4]
 e00637a:	6a68      	ldr	r0, [r5, #36]	; 0x24
 e00637c:	6831      	ldr	r1, [r6, #0]
 e00637e:	4290      	cmp	r0, r2
 e006380:	dce9      	bgt.n	e006356 <inic_host_init_skb+0x52>
 e006382:	4b2f      	ldr	r3, [pc, #188]	; (e006440 <inic_host_init_skb+0x13c>)
 e006384:	695b      	ldr	r3, [r3, #20]
 e006386:	03da      	lsls	r2, r3, #15
 e006388:	d51d      	bpl.n	e0063c6 <inic_host_init_skb+0xc2>
 e00638a:	f011 031f 	ands.w	r3, r1, #31
 e00638e:	ea4f 1080 	mov.w	r0, r0, lsl #6
 e006392:	d002      	beq.n	e00639a <inic_host_init_skb+0x96>
 e006394:	f021 011f 	bic.w	r1, r1, #31
 e006398:	4418      	add	r0, r3
 e00639a:	f3bf 8f4f 	dsb	sy
 e00639e:	2800      	cmp	r0, #0
 e0063a0:	dd0d      	ble.n	e0063be <inic_host_init_skb+0xba>
 e0063a2:	3801      	subs	r0, #1
 e0063a4:	f101 0220 	add.w	r2, r1, #32
 e0063a8:	f020 031f 	bic.w	r3, r0, #31
 e0063ac:	4824      	ldr	r0, [pc, #144]	; (e006440 <inic_host_init_skb+0x13c>)
 e0063ae:	4413      	add	r3, r2
 e0063b0:	429a      	cmp	r2, r3
 e0063b2:	f8c0 1268 	str.w	r1, [r0, #616]	; 0x268
 e0063b6:	4611      	mov	r1, r2
 e0063b8:	f102 0220 	add.w	r2, r2, #32
 e0063bc:	d1f8      	bne.n	e0063b0 <inic_host_init_skb+0xac>
 e0063be:	f3bf 8f4f 	dsb	sy
 e0063c2:	f3bf 8f6f 	isb	sy
 e0063c6:	4b1e      	ldr	r3, [pc, #120]	; (e006440 <inic_host_init_skb+0x13c>)
 e0063c8:	695b      	ldr	r3, [r3, #20]
 e0063ca:	03db      	lsls	r3, r3, #15
 e0063cc:	d521      	bpl.n	e006412 <inic_host_init_skb+0x10e>
 e0063ce:	f44f 63d8 	mov.w	r3, #1728	; 0x6c0
 e0063d2:	6a69      	ldr	r1, [r5, #36]	; 0x24
 e0063d4:	6822      	ldr	r2, [r4, #0]
 e0063d6:	fb03 f101 	mul.w	r1, r3, r1
 e0063da:	f012 031f 	ands.w	r3, r2, #31
 e0063de:	d002      	beq.n	e0063e6 <inic_host_init_skb+0xe2>
 e0063e0:	f022 021f 	bic.w	r2, r2, #31
 e0063e4:	4419      	add	r1, r3
 e0063e6:	f3bf 8f4f 	dsb	sy
 e0063ea:	2900      	cmp	r1, #0
 e0063ec:	dd0d      	ble.n	e00640a <inic_host_init_skb+0x106>
 e0063ee:	3901      	subs	r1, #1
 e0063f0:	f102 0320 	add.w	r3, r2, #32
 e0063f4:	4812      	ldr	r0, [pc, #72]	; (e006440 <inic_host_init_skb+0x13c>)
 e0063f6:	f021 011f 	bic.w	r1, r1, #31
 e0063fa:	4419      	add	r1, r3
 e0063fc:	428b      	cmp	r3, r1
 e0063fe:	f8c0 2268 	str.w	r2, [r0, #616]	; 0x268
 e006402:	461a      	mov	r2, r3
 e006404:	f103 0320 	add.w	r3, r3, #32
 e006408:	d1f8      	bne.n	e0063fc <inic_host_init_skb+0xf8>
 e00640a:	f3bf 8f4f 	dsb	sy
 e00640e:	f3bf 8f6f 	isb	sy
 e006412:	b003      	add	sp, #12
 e006414:	bdf0      	pop	{r4, r5, r6, r7, pc}
 e006416:	4a0b      	ldr	r2, [pc, #44]	; (e006444 <inic_host_init_skb+0x140>)
 e006418:	2002      	movs	r0, #2
 e00641a:	4907      	ldr	r1, [pc, #28]	; (e006438 <inic_host_init_skb+0x134>)
 e00641c:	9200      	str	r2, [sp, #0]
 e00641e:	2245      	movs	r2, #69	; 0x45
 e006420:	4b09      	ldr	r3, [pc, #36]	; (e006448 <inic_host_init_skb+0x144>)
 e006422:	f7fc f80b 	bl	e00243c <rtk_log_write>
 e006426:	6831      	ldr	r1, [r6, #0]
 e006428:	e790      	b.n	e00634c <inic_host_init_skb+0x48>
 e00642a:	bf00      	nop
 e00642c:	2000beec 	.word	0x2000beec
 e006430:	2000bee8 	.word	0x2000bee8
 e006434:	0e009634 	.word	0x0e009634
 e006438:	0e0095d8 	.word	0x0e0095d8
 e00643c:	2000b400 	.word	0x2000b400
 e006440:	e000ed00 	.word	0xe000ed00
 e006444:	0e009694 	.word	0x0e009694
 e006448:	0e009660 	.word	0x0e009660

0e00644c <inic_host_init_priv>:
 e00644c:	b510      	push	{r4, lr}
 e00644e:	4c13      	ldr	r4, [pc, #76]	; (e00649c <inic_host_init_priv+0x50>)
 e006450:	2228      	movs	r2, #40	; 0x28
 e006452:	2100      	movs	r1, #0
 e006454:	4620      	mov	r0, r4
 e006456:	f001 fad7 	bl	e007a08 <____wrap_memset_veneer>
 e00645a:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
 e00645e:	2100      	movs	r1, #0
 e006460:	4620      	mov	r0, r4
 e006462:	f7fc fb71 	bl	e002b48 <rtos_sema_create_static>
 e006466:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
 e00646a:	2100      	movs	r1, #0
 e00646c:	1d20      	adds	r0, r4, #4
 e00646e:	f7fc fb6b 	bl	e002b48 <rtos_sema_create_static>
 e006472:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
 e006476:	2100      	movs	r1, #0
 e006478:	f104 0008 	add.w	r0, r4, #8
 e00647c:	f7fc fb64 	bl	e002b48 <rtos_sema_create_static>
 e006480:	68a0      	ldr	r0, [r4, #8]
 e006482:	f7fc fbbb 	bl	e002bfc <rtos_sema_give>
 e006486:	f104 000c 	add.w	r0, r4, #12
 e00648a:	f000 f969 	bl	e006760 <rtw_init_queue>
 e00648e:	2300      	movs	r3, #0
 e006490:	e9c4 3306 	strd	r3, r3, [r4, #24]
 e006494:	e9c4 3308 	strd	r3, r3, [r4, #32]
 e006498:	bd10      	pop	{r4, pc}
 e00649a:	bf00      	nop
 e00649c:	2000bec0 	.word	0x2000bec0

0e0064a0 <inic_host_trx_event_hdl>:
 e0064a0:	2804      	cmp	r0, #4
 e0064a2:	d100      	bne.n	e0064a6 <inic_host_trx_event_hdl+0x6>
 e0064a4:	4770      	bx	lr
 e0064a6:	b510      	push	{r4, lr}
 e0064a8:	4604      	mov	r4, r0
 e0064aa:	b082      	sub	sp, #8
 e0064ac:	4b04      	ldr	r3, [pc, #16]	; (e0064c0 <inic_host_trx_event_hdl+0x20>)
 e0064ae:	2245      	movs	r2, #69	; 0x45
 e0064b0:	4904      	ldr	r1, [pc, #16]	; (e0064c4 <inic_host_trx_event_hdl+0x24>)
 e0064b2:	2002      	movs	r0, #2
 e0064b4:	9400      	str	r4, [sp, #0]
 e0064b6:	f7fb ffc1 	bl	e00243c <rtk_log_write>
 e0064ba:	b002      	add	sp, #8
 e0064bc:	bd10      	pop	{r4, pc}
 e0064be:	bf00      	nop
 e0064c0:	0e009678 	.word	0x0e009678
 e0064c4:	0e0095d8 	.word	0x0e0095d8

0e0064c8 <inic_host_trx_int_hdl>:
 e0064c8:	b530      	push	{r4, r5, lr}
 e0064ca:	2106      	movs	r1, #6
 e0064cc:	b083      	sub	sp, #12
 e0064ce:	2000      	movs	r0, #0
 e0064d0:	f7fb facc 	bl	e001a6c <ipc_get_message>
 e0064d4:	4b27      	ldr	r3, [pc, #156]	; (e006574 <inic_host_trx_int_hdl+0xac>)
 e0064d6:	6844      	ldr	r4, [r0, #4]
 e0064d8:	695b      	ldr	r3, [r3, #20]
 e0064da:	03da      	lsls	r2, r3, #15
 e0064dc:	d512      	bpl.n	e006504 <inic_host_trx_int_hdl+0x3c>
 e0064de:	f014 011f 	ands.w	r1, r4, #31
 e0064e2:	d138      	bne.n	e006556 <inic_host_trx_int_hdl+0x8e>
 e0064e4:	4623      	mov	r3, r4
 e0064e6:	2140      	movs	r1, #64	; 0x40
 e0064e8:	f3bf 8f4f 	dsb	sy
 e0064ec:	4821      	ldr	r0, [pc, #132]	; (e006574 <inic_host_trx_int_hdl+0xac>)
 e0064ee:	4419      	add	r1, r3
 e0064f0:	f8c0 325c 	str.w	r3, [r0, #604]	; 0x25c
 e0064f4:	3320      	adds	r3, #32
 e0064f6:	1aca      	subs	r2, r1, r3
 e0064f8:	2a00      	cmp	r2, #0
 e0064fa:	dcf9      	bgt.n	e0064f0 <inic_host_trx_int_hdl+0x28>
 e0064fc:	f3bf 8f4f 	dsb	sy
 e006500:	f3bf 8f6f 	isb	sy
 e006504:	f000 f914 	bl	e006730 <inic_msg_get_queue_status>
 e006508:	4605      	mov	r5, r0
 e00650a:	b340      	cbz	r0, e00655e <inic_host_trx_int_hdl+0x96>
 e00650c:	4620      	mov	r0, r4
 e00650e:	f000 f8c7 	bl	e0066a0 <inic_msg_enqueue>
 e006512:	1e45      	subs	r5, r0, #1
 e006514:	bf18      	it	ne
 e006516:	2501      	movne	r5, #1
 e006518:	2200      	movs	r2, #0
 e00651a:	4b16      	ldr	r3, [pc, #88]	; (e006574 <inic_host_trx_int_hdl+0xac>)
 e00651c:	60a5      	str	r5, [r4, #8]
 e00651e:	6022      	str	r2, [r4, #0]
 e006520:	695b      	ldr	r3, [r3, #20]
 e006522:	03db      	lsls	r3, r3, #15
 e006524:	d511      	bpl.n	e00654a <inic_host_trx_int_hdl+0x82>
 e006526:	f014 021f 	ands.w	r2, r4, #31
 e00652a:	d110      	bne.n	e00654e <inic_host_trx_int_hdl+0x86>
 e00652c:	2240      	movs	r2, #64	; 0x40
 e00652e:	f3bf 8f4f 	dsb	sy
 e006532:	4910      	ldr	r1, [pc, #64]	; (e006574 <inic_host_trx_int_hdl+0xac>)
 e006534:	4422      	add	r2, r4
 e006536:	f8c1 4268 	str.w	r4, [r1, #616]	; 0x268
 e00653a:	3420      	adds	r4, #32
 e00653c:	1b13      	subs	r3, r2, r4
 e00653e:	2b00      	cmp	r3, #0
 e006540:	dcf9      	bgt.n	e006536 <inic_host_trx_int_hdl+0x6e>
 e006542:	f3bf 8f4f 	dsb	sy
 e006546:	f3bf 8f6f 	isb	sy
 e00654a:	b003      	add	sp, #12
 e00654c:	bd30      	pop	{r4, r5, pc}
 e00654e:	f024 041f 	bic.w	r4, r4, #31
 e006552:	3240      	adds	r2, #64	; 0x40
 e006554:	e7eb      	b.n	e00652e <inic_host_trx_int_hdl+0x66>
 e006556:	f024 031f 	bic.w	r3, r4, #31
 e00655a:	3140      	adds	r1, #64	; 0x40
 e00655c:	e7c4      	b.n	e0064e8 <inic_host_trx_int_hdl+0x20>
 e00655e:	7821      	ldrb	r1, [r4, #0]
 e006560:	2904      	cmp	r1, #4
 e006562:	d0d9      	beq.n	e006518 <inic_host_trx_int_hdl+0x50>
 e006564:	9100      	str	r1, [sp, #0]
 e006566:	2245      	movs	r2, #69	; 0x45
 e006568:	4b03      	ldr	r3, [pc, #12]	; (e006578 <inic_host_trx_int_hdl+0xb0>)
 e00656a:	2002      	movs	r0, #2
 e00656c:	4903      	ldr	r1, [pc, #12]	; (e00657c <inic_host_trx_int_hdl+0xb4>)
 e00656e:	f7fb ff65 	bl	e00243c <rtk_log_write>
 e006572:	e7d1      	b.n	e006518 <inic_host_trx_int_hdl+0x50>
 e006574:	e000ed00 	.word	0xe000ed00
 e006578:	0e009678 	.word	0x0e009678
 e00657c:	0e0095d8 	.word	0x0e0095d8

0e006580 <inic_msg_q_task>:
 e006580:	b538      	push	{r3, r4, r5, lr}
 e006582:	4d16      	ldr	r5, [pc, #88]	; (e0065dc <inic_msg_q_task+0x5c>)
 e006584:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
 e006588:	68e8      	ldr	r0, [r5, #12]
 e00658a:	f7fc fb0f 	bl	e002bac <rtos_sema_take>
 e00658e:	e015      	b.n	e0065bc <inic_msg_q_task+0x3c>
 e006590:	682c      	ldr	r4, [r5, #0]
 e006592:	4620      	mov	r0, r4
 e006594:	f000 f8dc 	bl	e006750 <rtw_list_delete>
 e006598:	f7fc fa8e 	bl	e002ab8 <rtos_critical_exit>
 e00659c:	b1bc      	cbz	r4, e0065ce <inic_msg_q_task+0x4e>
 e00659e:	7b20      	ldrb	r0, [r4, #12]
 e0065a0:	68a1      	ldr	r1, [r4, #8]
 e0065a2:	f3c0 0241 	ubfx	r2, r0, #1, #2
 e0065a6:	08c0      	lsrs	r0, r0, #3
 e0065a8:	f7ff ff7a 	bl	e0064a0 <inic_host_trx_event_hdl>
 e0065ac:	f7fc fa6a 	bl	e002a84 <rtos_critical_enter>
 e0065b0:	7b23      	ldrb	r3, [r4, #12]
 e0065b2:	f36f 0300 	bfc	r3, #0, #1
 e0065b6:	7323      	strb	r3, [r4, #12]
 e0065b8:	f7fc fa7e 	bl	e002ab8 <rtos_critical_exit>
 e0065bc:	f7fc fa62 	bl	e002a84 <rtos_critical_enter>
 e0065c0:	4628      	mov	r0, r5
 e0065c2:	f000 f8d3 	bl	e00676c <rtw_queue_empty>
 e0065c6:	2801      	cmp	r0, #1
 e0065c8:	d1e2      	bne.n	e006590 <inic_msg_q_task+0x10>
 e0065ca:	f7fc fa75 	bl	e002ab8 <rtos_critical_exit>
 e0065ce:	7d28      	ldrb	r0, [r5, #20]
 e0065d0:	2800      	cmp	r0, #0
 e0065d2:	d1d7      	bne.n	e006584 <inic_msg_q_task+0x4>
 e0065d4:	e8bd 4038 	ldmia.w	sp!, {r3, r4, r5, lr}
 e0065d8:	f7fc bc52 	b.w	e002e80 <rtos_task_delete>
 e0065dc:	2000bf40 	.word	0x2000bf40

0e0065e0 <inic_msg_q_init>:
 e0065e0:	b510      	push	{r4, lr}
 e0065e2:	4c28      	ldr	r4, [pc, #160]	; (e006684 <inic_msg_q_init+0xa4>)
 e0065e4:	b082      	sub	sp, #8
 e0065e6:	2220      	movs	r2, #32
 e0065e8:	2100      	movs	r1, #0
 e0065ea:	4620      	mov	r0, r4
 e0065ec:	f001 fa0c 	bl	e007a08 <____wrap_memset_veneer>
 e0065f0:	2240      	movs	r2, #64	; 0x40
 e0065f2:	2100      	movs	r1, #0
 e0065f4:	4824      	ldr	r0, [pc, #144]	; (e006688 <inic_msg_q_init+0xa8>)
 e0065f6:	f001 fa07 	bl	e007a08 <____wrap_memset_veneer>
 e0065fa:	4620      	mov	r0, r4
 e0065fc:	f000 f8b0 	bl	e006760 <rtw_init_queue>
 e006600:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
 e006604:	2100      	movs	r1, #0
 e006606:	f104 000c 	add.w	r0, r4, #12
 e00660a:	f7fc fa9d 	bl	e002b48 <rtos_sema_create_static>
 e00660e:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
 e006612:	2100      	movs	r1, #0
 e006614:	f104 0010 	add.w	r0, r4, #16
 e006618:	f7fc fa96 	bl	e002b48 <rtos_sema_create_static>
 e00661c:	6920      	ldr	r0, [r4, #16]
 e00661e:	f7fc faed 	bl	e002bfc <rtos_sema_give>
 e006622:	4b1a      	ldr	r3, [pc, #104]	; (e00668c <inic_msg_q_init+0xac>)
 e006624:	6a1b      	ldr	r3, [r3, #32]
 e006626:	b2d8      	uxtb	r0, r3
 e006628:	7723      	strb	r3, [r4, #28]
 e00662a:	0100      	lsls	r0, r0, #4
 e00662c:	f7fc fa5e 	bl	e002aec <rtos_mem_zmalloc>
 e006630:	7f23      	ldrb	r3, [r4, #28]
 e006632:	61a0      	str	r0, [r4, #24]
 e006634:	b163      	cbz	r3, e006650 <inic_msg_q_init+0x70>
 e006636:	2300      	movs	r3, #0
 e006638:	e000      	b.n	e00663c <inic_msg_q_init+0x5c>
 e00663a:	69a0      	ldr	r0, [r4, #24]
 e00663c:	eb00 1003 	add.w	r0, r0, r3, lsl #4
 e006640:	3301      	adds	r3, #1
 e006642:	7b02      	ldrb	r2, [r0, #12]
 e006644:	f36f 0200 	bfc	r2, #0, #1
 e006648:	7302      	strb	r2, [r0, #12]
 e00664a:	7f22      	ldrb	r2, [r4, #28]
 e00664c:	429a      	cmp	r2, r3
 e00664e:	dcf4      	bgt.n	e00663a <inic_msg_q_init+0x5a>
 e006650:	2006      	movs	r0, #6
 e006652:	f44f 7124 	mov.w	r1, #656	; 0x290
 e006656:	2300      	movs	r3, #0
 e006658:	4a0d      	ldr	r2, [pc, #52]	; (e006690 <inic_msg_q_init+0xb0>)
 e00665a:	e9cd 1000 	strd	r1, r0, [sp]
 e00665e:	490d      	ldr	r1, [pc, #52]	; (e006694 <inic_msg_q_init+0xb4>)
 e006660:	4618      	mov	r0, r3
 e006662:	f7fc fbf9 	bl	e002e58 <rtos_task_create>
 e006666:	b918      	cbnz	r0, e006670 <inic_msg_q_init+0x90>
 e006668:	2301      	movs	r3, #1
 e00666a:	7523      	strb	r3, [r4, #20]
 e00666c:	b002      	add	sp, #8
 e00666e:	bd10      	pop	{r4, pc}
 e006670:	4b09      	ldr	r3, [pc, #36]	; (e006698 <inic_msg_q_init+0xb8>)
 e006672:	2245      	movs	r2, #69	; 0x45
 e006674:	4909      	ldr	r1, [pc, #36]	; (e00669c <inic_msg_q_init+0xbc>)
 e006676:	2002      	movs	r0, #2
 e006678:	f7fb fee0 	bl	e00243c <rtk_log_write>
 e00667c:	2301      	movs	r3, #1
 e00667e:	7523      	strb	r3, [r4, #20]
 e006680:	b002      	add	sp, #8
 e006682:	bd10      	pop	{r4, pc}
 e006684:	2000bf40 	.word	0x2000bf40
 e006688:	2000bf00 	.word	0x2000bf00
 e00668c:	2000b400 	.word	0x2000b400
 e006690:	0e006581 	.word	0x0e006581
 e006694:	0e0096a8 	.word	0x0e0096a8
 e006698:	0e0096b8 	.word	0x0e0096b8
 e00669c:	0e0095d8 	.word	0x0e0095d8

0e0066a0 <inic_msg_enqueue>:
 e0066a0:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 e0066a2:	4f20      	ldr	r7, [pc, #128]	; (e006724 <inic_msg_enqueue+0x84>)
 e0066a4:	4606      	mov	r6, r0
 e0066a6:	f7fc f9ed 	bl	e002a84 <rtos_critical_enter>
 e0066aa:	7f3d      	ldrb	r5, [r7, #28]
 e0066ac:	b35d      	cbz	r5, e006706 <inic_msg_enqueue+0x66>
 e0066ae:	69bc      	ldr	r4, [r7, #24]
 e0066b0:	2300      	movs	r3, #0
 e0066b2:	e003      	b.n	e0066bc <inic_msg_enqueue+0x1c>
 e0066b4:	42ab      	cmp	r3, r5
 e0066b6:	f104 0410 	add.w	r4, r4, #16
 e0066ba:	d024      	beq.n	e006706 <inic_msg_enqueue+0x66>
 e0066bc:	7b22      	ldrb	r2, [r4, #12]
 e0066be:	3301      	adds	r3, #1
 e0066c0:	4611      	mov	r1, r2
 e0066c2:	07d2      	lsls	r2, r2, #31
 e0066c4:	d4f6      	bmi.n	e0066b4 <inic_msg_enqueue+0x14>
 e0066c6:	f041 0101 	orr.w	r1, r1, #1
 e0066ca:	7321      	strb	r1, [r4, #12]
 e0066cc:	f7fc f9f4 	bl	e002ab8 <rtos_critical_exit>
 e0066d0:	b1dc      	cbz	r4, e00670a <inic_msg_enqueue+0x6a>
 e0066d2:	6832      	ldr	r2, [r6, #0]
 e0066d4:	2501      	movs	r5, #1
 e0066d6:	7b23      	ldrb	r3, [r4, #12]
 e0066d8:	f362 03c7 	bfi	r3, r2, #3, #5
 e0066dc:	7323      	strb	r3, [r4, #12]
 e0066de:	b2db      	uxtb	r3, r3
 e0066e0:	6872      	ldr	r2, [r6, #4]
 e0066e2:	60a2      	str	r2, [r4, #8]
 e0066e4:	68f2      	ldr	r2, [r6, #12]
 e0066e6:	f362 0342 	bfi	r3, r2, #1, #2
 e0066ea:	7323      	strb	r3, [r4, #12]
 e0066ec:	f7fc f9ca 	bl	e002a84 <rtos_critical_enter>
 e0066f0:	490c      	ldr	r1, [pc, #48]	; (e006724 <inic_msg_enqueue+0x84>)
 e0066f2:	4620      	mov	r0, r4
 e0066f4:	f000 f826 	bl	e006744 <rtw_list_insert_tail>
 e0066f8:	f7fc f9de 	bl	e002ab8 <rtos_critical_exit>
 e0066fc:	68f8      	ldr	r0, [r7, #12]
 e0066fe:	f7fc fa7d 	bl	e002bfc <rtos_sema_give>
 e006702:	4628      	mov	r0, r5
 e006704:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e006706:	f7fc f9d7 	bl	e002ab8 <rtos_critical_exit>
 e00670a:	4b07      	ldr	r3, [pc, #28]	; (e006728 <inic_msg_enqueue+0x88>)
 e00670c:	2245      	movs	r2, #69	; 0x45
 e00670e:	4907      	ldr	r1, [pc, #28]	; (e00672c <inic_msg_enqueue+0x8c>)
 e006710:	2002      	movs	r0, #2
 e006712:	f04f 35ff 	mov.w	r5, #4294967295	; 0xffffffff
 e006716:	f7fb fe91 	bl	e00243c <rtk_log_write>
 e00671a:	68f8      	ldr	r0, [r7, #12]
 e00671c:	f7fc fa6e 	bl	e002bfc <rtos_sema_give>
 e006720:	4628      	mov	r0, r5
 e006722:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
 e006724:	2000bf40 	.word	0x2000bf40
 e006728:	0e0096d8 	.word	0x0e0096d8
 e00672c:	0e0095d8 	.word	0x0e0095d8

0e006730 <inic_msg_get_queue_status>:
 e006730:	4b01      	ldr	r3, [pc, #4]	; (e006738 <inic_msg_get_queue_status+0x8>)
 e006732:	7d18      	ldrb	r0, [r3, #20]
 e006734:	4770      	bx	lr
 e006736:	bf00      	nop
 e006738:	2000bf40 	.word	0x2000bf40

0e00673c <rtw_init_listhead>:
 e00673c:	e9c0 0000 	strd	r0, r0, [r0]
 e006740:	4770      	bx	lr
 e006742:	bf00      	nop

0e006744 <rtw_list_insert_tail>:
 e006744:	684b      	ldr	r3, [r1, #4]
 e006746:	6048      	str	r0, [r1, #4]
 e006748:	e9c0 1300 	strd	r1, r3, [r0]
 e00674c:	6018      	str	r0, [r3, #0]
 e00674e:	4770      	bx	lr

0e006750 <rtw_list_delete>:
 e006750:	e9d0 2300 	ldrd	r2, r3, [r0]
 e006754:	6053      	str	r3, [r2, #4]
 e006756:	601a      	str	r2, [r3, #0]
 e006758:	e9c0 0000 	strd	r0, r0, [r0]
 e00675c:	4770      	bx	lr
 e00675e:	bf00      	nop

0e006760 <rtw_init_queue>:
 e006760:	4603      	mov	r3, r0
 e006762:	3008      	adds	r0, #8
 e006764:	e9c3 3300 	strd	r3, r3, [r3]
 e006768:	f7fc b9d2 	b.w	e002b10 <rtos_mutex_create_static>

0e00676c <rtw_queue_empty>:
 e00676c:	6803      	ldr	r3, [r0, #0]
 e00676e:	1a18      	subs	r0, r3, r0
 e006770:	fab0 f080 	clz	r0, r0
 e006774:	0940      	lsrs	r0, r0, #5
 e006776:	4770      	bx	lr

0e006778 <init_timer_wrapper>:
 e006778:	b508      	push	{r3, lr}
 e00677a:	4804      	ldr	r0, [pc, #16]	; (e00678c <init_timer_wrapper+0x14>)
 e00677c:	f7ff ffde 	bl	e00673c <rtw_init_listhead>
 e006780:	2300      	movs	r3, #0
 e006782:	4903      	ldr	r1, [pc, #12]	; (e006790 <init_timer_wrapper+0x18>)
 e006784:	4a03      	ldr	r2, [pc, #12]	; (e006794 <init_timer_wrapper+0x1c>)
 e006786:	600b      	str	r3, [r1, #0]
 e006788:	6013      	str	r3, [r2, #0]
 e00678a:	bd08      	pop	{r3, pc}
 e00678c:	2000bf74 	.word	0x2000bf74
 e006790:	2000bf7c 	.word	0x2000bf7c
 e006794:	2000bf70 	.word	0x2000bf70

0e006798 <ChipInfo_BDNum>:
 e006798:	b500      	push	{lr}
 e00679a:	b083      	sub	sp, #12
 e00679c:	f240 70ff 	movw	r0, #2047	; 0x7ff
 e0067a0:	4b18      	ldr	r3, [pc, #96]	; (e006804 <ChipInfo_BDNum+0x6c>)
 e0067a2:	f10d 0107 	add.w	r1, sp, #7
 e0067a6:	4798      	blx	r3
 e0067a8:	f89d e007 	ldrb.w	lr, [sp, #7]
 e0067ac:	f1be 0fff 	cmp.w	lr, #255	; 0xff
 e0067b0:	d022      	beq.n	e0067f8 <ChipInfo_BDNum+0x60>
 e0067b2:	ea4f 115e 	mov.w	r1, lr, lsr #5
 e0067b6:	4b14      	ldr	r3, [pc, #80]	; (e006808 <ChipInfo_BDNum+0x70>)
 e0067b8:	f00e 0e1f 	and.w	lr, lr, #31
 e0067bc:	2200      	movs	r2, #0
 e0067be:	f44f 707d 	mov.w	r0, #1012	; 0x3f4
 e0067c2:	f64f 7cff 	movw	ip, #65535	; 0xffff
 e0067c6:	e004      	b.n	e0067d2 <ChipInfo_BDNum+0x3a>
 e0067c8:	8958      	ldrh	r0, [r3, #10]
 e0067ca:	4560      	cmp	r0, ip
 e0067cc:	d009      	beq.n	e0067e2 <ChipInfo_BDNum+0x4a>
 e0067ce:	f813 2f08 	ldrb.w	r2, [r3, #8]!
 e0067d2:	428a      	cmp	r2, r1
 e0067d4:	d1f8      	bne.n	e0067c8 <ChipInfo_BDNum+0x30>
 e0067d6:	785a      	ldrb	r2, [r3, #1]
 e0067d8:	4572      	cmp	r2, lr
 e0067da:	d1f5      	bne.n	e0067c8 <ChipInfo_BDNum+0x30>
 e0067dc:	b003      	add	sp, #12
 e0067de:	f85d fb04 	ldr.w	pc, [sp], #4
 e0067e2:	4b0a      	ldr	r3, [pc, #40]	; (e00680c <ChipInfo_BDNum+0x74>)
 e0067e4:	2245      	movs	r2, #69	; 0x45
 e0067e6:	490a      	ldr	r1, [pc, #40]	; (e006810 <ChipInfo_BDNum+0x78>)
 e0067e8:	2002      	movs	r0, #2
 e0067ea:	f7fb fe27 	bl	e00243c <rtk_log_write>
 e0067ee:	f240 30f6 	movw	r0, #1014	; 0x3f6
 e0067f2:	b003      	add	sp, #12
 e0067f4:	f85d fb04 	ldr.w	pc, [sp], #4
 e0067f8:	f64f 70ff 	movw	r0, #65535	; 0xffff
 e0067fc:	b003      	add	sp, #12
 e0067fe:	f85d fb04 	ldr.w	pc, [sp], #4
 e006802:	bf00      	nop
 e006804:	0e006a29 	.word	0x0e006a29
 e006808:	0e00980c 	.word	0x0e00980c
 e00680c:	0e0096f4 	.word	0x0e0096f4
 e006810:	0e009708 	.word	0x0e009708

0e006814 <ChipInfo_MCM_Info>:
 e006814:	b570      	push	{r4, r5, r6, lr}
 e006816:	f64f 76ff 	movw	r6, #65535	; 0xffff
 e00681a:	4b14      	ldr	r3, [pc, #80]	; (e00686c <ChipInfo_MCM_Info+0x58>)
 e00681c:	4798      	blx	r3
 e00681e:	42b0      	cmp	r0, r6
 e006820:	d020      	beq.n	e006864 <ChipInfo_MCM_Info+0x50>
 e006822:	f240 34f6 	movw	r4, #1014	; 0x3f6
 e006826:	1b04      	subs	r4, r0, r4
 e006828:	fab4 f484 	clz	r4, r4
 e00682c:	0964      	lsrs	r4, r4, #5
 e00682e:	b9cc      	cbnz	r4, e006864 <ChipInfo_MCM_Info+0x50>
 e006830:	2301      	movs	r3, #1
 e006832:	f240 32f7 	movw	r2, #1015	; 0x3f7
 e006836:	f44f 7c7d 	mov.w	ip, #1012	; 0x3f4
 e00683a:	4621      	mov	r1, r4
 e00683c:	4d0c      	ldr	r5, [pc, #48]	; (e006870 <ChipInfo_MCM_Info+0x5c>)
 e00683e:	e000      	b.n	e006842 <ChipInfo_MCM_Info+0x2e>
 e006840:	8852      	ldrh	r2, [r2, #2]
 e006842:	4560      	cmp	r0, ip
 e006844:	f103 0e01 	add.w	lr, r3, #1
 e006848:	4694      	mov	ip, r2
 e00684a:	d102      	bne.n	e006852 <ChipInfo_MCM_Info+0x3e>
 e00684c:	eb05 01c1 	add.w	r1, r5, r1, lsl #3
 e006850:	684c      	ldr	r4, [r1, #4]
 e006852:	4619      	mov	r1, r3
 e006854:	42b2      	cmp	r2, r6
 e006856:	fa5f f38e 	uxtb.w	r3, lr
 e00685a:	eb05 02c3 	add.w	r2, r5, r3, lsl #3
 e00685e:	d1ef      	bne.n	e006840 <ChipInfo_MCM_Info+0x2c>
 e006860:	4620      	mov	r0, r4
 e006862:	bd70      	pop	{r4, r5, r6, pc}
 e006864:	f44f 0480 	mov.w	r4, #4194304	; 0x400000
 e006868:	4620      	mov	r0, r4
 e00686a:	bd70      	pop	{r4, r5, r6, pc}
 e00686c:	0e006799 	.word	0x0e006799
 e006870:	0e00980c 	.word	0x0e00980c

0e006874 <ChipInfo_GetLibVersion>:
 e006874:	b5f0      	push	{r4, r5, r6, r7, lr}
 e006876:	4c1e      	ldr	r4, [pc, #120]	; (e0068f0 <ChipInfo_GetLibVersion+0x7c>)
 e006878:	b083      	sub	sp, #12
 e00687a:	4b1e      	ldr	r3, [pc, #120]	; (e0068f4 <ChipInfo_GetLibVersion+0x80>)
 e00687c:	2249      	movs	r2, #73	; 0x49
 e00687e:	9400      	str	r4, [sp, #0]
 e006880:	2004      	movs	r0, #4
 e006882:	4c1d      	ldr	r4, [pc, #116]	; (e0068f8 <ChipInfo_GetLibVersion+0x84>)
 e006884:	491d      	ldr	r1, [pc, #116]	; (e0068fc <ChipInfo_GetLibVersion+0x88>)
 e006886:	f7fb fdd9 	bl	e00243c <rtk_log_write>
 e00688a:	4b1d      	ldr	r3, [pc, #116]	; (e006900 <ChipInfo_GetLibVersion+0x8c>)
 e00688c:	2249      	movs	r2, #73	; 0x49
 e00688e:	491b      	ldr	r1, [pc, #108]	; (e0068fc <ChipInfo_GetLibVersion+0x88>)
 e006890:	2004      	movs	r0, #4
 e006892:	9400      	str	r4, [sp, #0]
 e006894:	f7fb fdd2 	bl	e00243c <rtk_log_write>
 e006898:	4b1a      	ldr	r3, [pc, #104]	; (e006904 <ChipInfo_GetLibVersion+0x90>)
 e00689a:	491b      	ldr	r1, [pc, #108]	; (e006908 <ChipInfo_GetLibVersion+0x94>)
 e00689c:	2249      	movs	r2, #73	; 0x49
 e00689e:	f8d3 0228 	ldr.w	r0, [r3, #552]	; 0x228
 e0068a2:	4c1a      	ldr	r4, [pc, #104]	; (e00690c <ChipInfo_GetLibVersion+0x98>)
 e0068a4:	f010 0fc0 	tst.w	r0, #192	; 0xc0
 e0068a8:	4d19      	ldr	r5, [pc, #100]	; (e006910 <ChipInfo_GetLibVersion+0x9c>)
 e0068aa:	4b1a      	ldr	r3, [pc, #104]	; (e006914 <ChipInfo_GetLibVersion+0xa0>)
 e0068ac:	f04f 0004 	mov.w	r0, #4
 e0068b0:	bf08      	it	eq
 e0068b2:	460c      	moveq	r4, r1
 e0068b4:	4911      	ldr	r1, [pc, #68]	; (e0068fc <ChipInfo_GetLibVersion+0x88>)
 e0068b6:	9400      	str	r4, [sp, #0]
 e0068b8:	4c17      	ldr	r4, [pc, #92]	; (e006918 <ChipInfo_GetLibVersion+0xa4>)
 e0068ba:	f7fb fdbf 	bl	e00243c <rtk_log_write>
 e0068be:	42ac      	cmp	r4, r5
 e0068c0:	d214      	bcs.n	e0068ec <ChipInfo_GetLibVersion+0x78>
 e0068c2:	4e16      	ldr	r6, [pc, #88]	; (e00691c <ChipInfo_GetLibVersion+0xa8>)
 e0068c4:	4f16      	ldr	r7, [pc, #88]	; (e006920 <ChipInfo_GetLibVersion+0xac>)
 e0068c6:	e005      	b.n	e0068d4 <ChipInfo_GetLibVersion+0x60>
 e0068c8:	3401      	adds	r4, #1
 e0068ca:	4630      	mov	r0, r6
 e0068cc:	f000 fb9c 	bl	e007008 <DiagPrintf>
 e0068d0:	42ac      	cmp	r4, r5
 e0068d2:	d20b      	bcs.n	e0068ec <ChipInfo_GetLibVersion+0x78>
 e0068d4:	7821      	ldrb	r1, [r4, #0]
 e0068d6:	2900      	cmp	r1, #0
 e0068d8:	d1f6      	bne.n	e0068c8 <ChipInfo_GetLibVersion+0x54>
 e0068da:	f814 3f01 	ldrb.w	r3, [r4, #1]!
 e0068de:	2b00      	cmp	r3, #0
 e0068e0:	d0fb      	beq.n	e0068da <ChipInfo_GetLibVersion+0x66>
 e0068e2:	4638      	mov	r0, r7
 e0068e4:	f000 fb90 	bl	e007008 <DiagPrintf>
 e0068e8:	42ac      	cmp	r4, r5
 e0068ea:	d3f3      	bcc.n	e0068d4 <ChipInfo_GetLibVersion+0x60>
 e0068ec:	b003      	add	sp, #12
 e0068ee:	bdf0      	pop	{r4, r5, r6, r7, pc}
 e0068f0:	0e009790 	.word	0x0e009790
 e0068f4:	0e00977c 	.word	0x0e00977c
 e0068f8:	0e0097b4 	.word	0x0e0097b4
 e0068fc:	0e009708 	.word	0x0e009708
 e006900:	0e0097a4 	.word	0x0e0097a4
 e006904:	41008000 	.word	0x41008000
 e006908:	0e008abc 	.word	0x0e008abc
 e00690c:	0e008684 	.word	0x0e008684
 e006910:	0e009a90 	.word	0x0e009a90
 e006914:	0e0097f4 	.word	0x0e0097f4
 e006918:	0e009a00 	.word	0x0e009a00
 e00691c:	0e009808 	.word	0x0e009808
 e006920:	0e0082dc 	.word	0x0e0082dc

0e006924 <print_unsigned_num>:
 e006924:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e006928:	b089      	sub	sp, #36	; 0x24
 e00692a:	4606      	mov	r6, r0
 e00692c:	460d      	mov	r5, r1
 e00692e:	4690      	mov	r8, r2
 e006930:	f10d 0a0c 	add.w	sl, sp, #12
 e006934:	469b      	mov	fp, r3
 e006936:	2400      	movs	r4, #0
 e006938:	9f13      	ldr	r7, [sp, #76]	; 0x4c
 e00693a:	46d1      	mov	r9, sl
 e00693c:	e00f      	b.n	e00695e <print_unsigned_num+0x3a>
 e00693e:	f10c 0357 	add.w	r3, ip, #87	; 0x57
 e006942:	2f01      	cmp	r7, #1
 e006944:	b2db      	uxtb	r3, r3
 e006946:	d018      	beq.n	e00697a <print_unsigned_num+0x56>
 e006948:	4546      	cmp	r6, r8
 e00694a:	4606      	mov	r6, r0
 e00694c:	f104 0201 	add.w	r2, r4, #1
 e006950:	f809 3b01 	strb.w	r3, [r9], #1
 e006954:	f175 0000 	sbcs.w	r0, r5, #0
 e006958:	460d      	mov	r5, r1
 e00695a:	d312      	bcc.n	e006982 <print_unsigned_num+0x5e>
 e00695c:	4614      	mov	r4, r2
 e00695e:	4630      	mov	r0, r6
 e006960:	4629      	mov	r1, r5
 e006962:	4642      	mov	r2, r8
 e006964:	2300      	movs	r3, #0
 e006966:	f000 fd89 	bl	e00747c <__aeabi_uldivmod>
 e00696a:	2a09      	cmp	r2, #9
 e00696c:	fa5f fc82 	uxtb.w	ip, r2
 e006970:	f10c 0330 	add.w	r3, ip, #48	; 0x30
 e006974:	d8e3      	bhi.n	e00693e <print_unsigned_num+0x1a>
 e006976:	b2db      	uxtb	r3, r3
 e006978:	e7e6      	b.n	e006948 <print_unsigned_num+0x24>
 e00697a:	f10c 0337 	add.w	r3, ip, #55	; 0x37
 e00697e:	b2db      	uxtb	r3, r3
 e006980:	e7e2      	b.n	e006948 <print_unsigned_num+0x24>
 e006982:	4617      	mov	r7, r2
 e006984:	9a14      	ldr	r2, [sp, #80]	; 0x50
 e006986:	b192      	cbz	r2, e0069ae <print_unsigned_num+0x8a>
 e006988:	9a12      	ldr	r2, [sp, #72]	; 0x48
 e00698a:	f1bb 0f30 	cmp.w	fp, #48	; 0x30
 e00698e:	f102 32ff 	add.w	r2, r2, #4294967295	; 0xffffffff
 e006992:	9212      	str	r2, [sp, #72]	; 0x48
 e006994:	d136      	bne.n	e006a04 <print_unsigned_num+0xe0>
 e006996:	202d      	movs	r0, #45	; 0x2d
 e006998:	4d1f      	ldr	r5, [pc, #124]	; (e006a18 <print_unsigned_num+0xf4>)
 e00699a:	9301      	str	r3, [sp, #4]
 e00699c:	47a8      	blx	r5
 e00699e:	9b12      	ldr	r3, [sp, #72]	; 0x48
 e0069a0:	2b00      	cmp	r3, #0
 e0069a2:	9b01      	ldr	r3, [sp, #4]
 e0069a4:	dd1f      	ble.n	e0069e6 <print_unsigned_num+0xc2>
 e0069a6:	9a12      	ldr	r2, [sp, #72]	; 0x48
 e0069a8:	4297      	cmp	r7, r2
 e0069aa:	db08      	blt.n	e0069be <print_unsigned_num+0x9a>
 e0069ac:	e018      	b.n	e0069e0 <print_unsigned_num+0xbc>
 e0069ae:	9a12      	ldr	r2, [sp, #72]	; 0x48
 e0069b0:	2a00      	cmp	r2, #0
 e0069b2:	dd25      	ble.n	e006a00 <print_unsigned_num+0xdc>
 e0069b4:	9912      	ldr	r1, [sp, #72]	; 0x48
 e0069b6:	9a14      	ldr	r2, [sp, #80]	; 0x50
 e0069b8:	42b9      	cmp	r1, r7
 e0069ba:	4d17      	ldr	r5, [pc, #92]	; (e006a18 <print_unsigned_num+0xf4>)
 e0069bc:	dd0d      	ble.n	e0069da <print_unsigned_num+0xb6>
 e0069be:	9e12      	ldr	r6, [sp, #72]	; 0x48
 e0069c0:	4698      	mov	r8, r3
 e0069c2:	3e01      	subs	r6, #1
 e0069c4:	4658      	mov	r0, fp
 e0069c6:	47a8      	blx	r5
 e0069c8:	42b7      	cmp	r7, r6
 e0069ca:	d1fa      	bne.n	e0069c2 <print_unsigned_num+0x9e>
 e0069cc:	9912      	ldr	r1, [sp, #72]	; 0x48
 e0069ce:	43fa      	mvns	r2, r7
 e0069d0:	4643      	mov	r3, r8
 e0069d2:	440a      	add	r2, r1
 e0069d4:	9914      	ldr	r1, [sp, #80]	; 0x50
 e0069d6:	3101      	adds	r1, #1
 e0069d8:	440a      	add	r2, r1
 e0069da:	9914      	ldr	r1, [sp, #80]	; 0x50
 e0069dc:	9214      	str	r2, [sp, #80]	; 0x50
 e0069de:	b111      	cbz	r1, e0069e6 <print_unsigned_num+0xc2>
 e0069e0:	f01b 0fdf 	tst.w	fp, #223	; 0xdf
 e0069e4:	d013      	beq.n	e006a0e <print_unsigned_num+0xea>
 e0069e6:	4454      	add	r4, sl
 e0069e8:	4618      	mov	r0, r3
 e0069ea:	e001      	b.n	e0069f0 <print_unsigned_num+0xcc>
 e0069ec:	f814 0d01 	ldrb.w	r0, [r4, #-1]!
 e0069f0:	47a8      	blx	r5
 e0069f2:	45a2      	cmp	sl, r4
 e0069f4:	d1fa      	bne.n	e0069ec <print_unsigned_num+0xc8>
 e0069f6:	9b14      	ldr	r3, [sp, #80]	; 0x50
 e0069f8:	19d8      	adds	r0, r3, r7
 e0069fa:	b009      	add	sp, #36	; 0x24
 e0069fc:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 e006a00:	4d05      	ldr	r5, [pc, #20]	; (e006a18 <print_unsigned_num+0xf4>)
 e006a02:	e7f0      	b.n	e0069e6 <print_unsigned_num+0xc2>
 e006a04:	9a12      	ldr	r2, [sp, #72]	; 0x48
 e006a06:	2a00      	cmp	r2, #0
 e006a08:	dcd4      	bgt.n	e0069b4 <print_unsigned_num+0x90>
 e006a0a:	4d03      	ldr	r5, [pc, #12]	; (e006a18 <print_unsigned_num+0xf4>)
 e006a0c:	e7e8      	b.n	e0069e0 <print_unsigned_num+0xbc>
 e006a0e:	202d      	movs	r0, #45	; 0x2d
 e006a10:	9301      	str	r3, [sp, #4]
 e006a12:	47a8      	blx	r5
 e006a14:	9b01      	ldr	r3, [sp, #4]
 e006a16:	e7e6      	b.n	e0069e6 <print_unsigned_num+0xc2>
 e006a18:	0000aba1 	.word	0x0000aba1

0e006a1c <IPC_SEMDelay>:
 e006a1c:	4b01      	ldr	r3, [pc, #4]	; (e006a24 <IPC_SEMDelay+0x8>)
 e006a1e:	6018      	str	r0, [r3, #0]
 e006a20:	4770      	bx	lr
 e006a22:	bf00      	nop
 e006a24:	2000bf80 	.word	0x2000bf80

0e006a28 <OTP_Read8>:
 e006a28:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
 e006a2c:	f242 7410 	movw	r4, #10000	; 0x2710
 e006a30:	4d10      	ldr	r5, [pc, #64]	; (e006a74 <OTP_Read8+0x4c>)
 e006a32:	4607      	mov	r7, r0
 e006a34:	4688      	mov	r8, r1
 e006a36:	4e10      	ldr	r6, [pc, #64]	; (e006a78 <OTP_Read8+0x50>)
 e006a38:	f8d5 3204 	ldr.w	r3, [r5, #516]	; 0x204
 e006a3c:	2001      	movs	r0, #1
 e006a3e:	b16b      	cbz	r3, e006a5c <OTP_Read8+0x34>
 e006a40:	6833      	ldr	r3, [r6, #0]
 e006a42:	b103      	cbz	r3, e006a46 <OTP_Read8+0x1e>
 e006a44:	4798      	blx	r3
 e006a46:	3c01      	subs	r4, #1
 e006a48:	d1f6      	bne.n	e006a38 <OTP_Read8+0x10>
 e006a4a:	4b0c      	ldr	r3, [pc, #48]	; (e006a7c <OTP_Read8+0x54>)
 e006a4c:	2245      	movs	r2, #69	; 0x45
 e006a4e:	490c      	ldr	r1, [pc, #48]	; (e006a80 <OTP_Read8+0x58>)
 e006a50:	2002      	movs	r0, #2
 e006a52:	f7fb fcf3 	bl	e00243c <rtk_log_write>
 e006a56:	4620      	mov	r0, r4
 e006a58:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e006a5c:	4641      	mov	r1, r8
 e006a5e:	4b09      	ldr	r3, [pc, #36]	; (e006a84 <OTP_Read8+0x5c>)
 e006a60:	4638      	mov	r0, r7
 e006a62:	4798      	blx	r3
 e006a64:	4604      	mov	r4, r0
 e006a66:	2001      	movs	r0, #1
 e006a68:	f000 ffd2 	bl	e007a10 <__IPC_SEMFree_veneer>
 e006a6c:	4620      	mov	r0, r4
 e006a6e:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
 e006a72:	bf00      	nop
 e006a74:	41014000 	.word	0x41014000
 e006a78:	2000bf80 	.word	0x2000bf80
 e006a7c:	0e0098bc 	.word	0x0e0098bc
 e006a80:	0e0098d4 	.word	0x0e0098d4
 e006a84:	0000b275 	.word	0x0000b275

0e006a88 <DiagVprintf>:
 e006a88:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e006a8c:	4604      	mov	r4, r0
 e006a8e:	7800      	ldrb	r0, [r0, #0]
 e006a90:	b089      	sub	sp, #36	; 0x24
 e006a92:	2800      	cmp	r0, #0
 e006a94:	f000 82ab 	beq.w	e006fee <DiagVprintf+0x566>
 e006a98:	f04f 0a00 	mov.w	sl, #0
 e006a9c:	4688      	mov	r8, r1
 e006a9e:	4f89      	ldr	r7, [pc, #548]	; (e006cc4 <DiagVprintf+0x23c>)
 e006aa0:	4655      	mov	r5, sl
 e006aa2:	f8cd a010 	str.w	sl, [sp, #16]
 e006aa6:	e005      	b.n	e006ab4 <DiagVprintf+0x2c>
 e006aa8:	3501      	adds	r5, #1
 e006aaa:	47b8      	blx	r7
 e006aac:	7820      	ldrb	r0, [r4, #0]
 e006aae:	2800      	cmp	r0, #0
 e006ab0:	f000 808b 	beq.w	e006bca <DiagVprintf+0x142>
 e006ab4:	2825      	cmp	r0, #37	; 0x25
 e006ab6:	f104 0401 	add.w	r4, r4, #1
 e006aba:	d1f5      	bne.n	e006aa8 <DiagVprintf+0x20>
 e006abc:	f04f 0a00 	mov.w	sl, #0
 e006ac0:	4656      	mov	r6, sl
 e006ac2:	46d1      	mov	r9, sl
 e006ac4:	46d4      	mov	ip, sl
 e006ac6:	7823      	ldrb	r3, [r4, #0]
 e006ac8:	2b7a      	cmp	r3, #122	; 0x7a
 e006aca:	d87c      	bhi.n	e006bc6 <DiagVprintf+0x13e>
 e006acc:	e8df f013 	tbh	[pc, r3, lsl #1]
 e006ad0:	007b00af 	.word	0x007b00af
 e006ad4:	007b007b 	.word	0x007b007b
 e006ad8:	007b007b 	.word	0x007b007b
 e006adc:	007b007b 	.word	0x007b007b
 e006ae0:	007b007b 	.word	0x007b007b
 e006ae4:	007b007b 	.word	0x007b007b
 e006ae8:	007b007b 	.word	0x007b007b
 e006aec:	007b007b 	.word	0x007b007b
 e006af0:	007b007b 	.word	0x007b007b
 e006af4:	007b007b 	.word	0x007b007b
 e006af8:	007b007b 	.word	0x007b007b
 e006afc:	007b007b 	.word	0x007b007b
 e006b00:	007b007b 	.word	0x007b007b
 e006b04:	007b007b 	.word	0x007b007b
 e006b08:	007b007b 	.word	0x007b007b
 e006b0c:	007b007b 	.word	0x007b007b
 e006b10:	007b00c4 	.word	0x007b00c4
 e006b14:	007b007b 	.word	0x007b007b
 e006b18:	01af007b 	.word	0x01af007b
 e006b1c:	007b007b 	.word	0x007b007b
 e006b20:	007b007b 	.word	0x007b007b
 e006b24:	007b007b 	.word	0x007b007b
 e006b28:	01ab007b 	.word	0x01ab007b
 e006b2c:	007b007b 	.word	0x007b007b
 e006b30:	00b100c4 	.word	0x00b100c4
 e006b34:	00b100b1 	.word	0x00b100b1
 e006b38:	00b100b1 	.word	0x00b100b1
 e006b3c:	00b100b1 	.word	0x00b100b1
 e006b40:	00b100b1 	.word	0x00b100b1
 e006b44:	007b007b 	.word	0x007b007b
 e006b48:	007b007b 	.word	0x007b007b
 e006b4c:	007b007b 	.word	0x007b007b
 e006b50:	007b007b 	.word	0x007b007b
 e006b54:	012c014f 	.word	0x012c014f
 e006b58:	007b007b 	.word	0x007b007b
 e006b5c:	007b007b 	.word	0x007b007b
 e006b60:	007b007b 	.word	0x007b007b
 e006b64:	007b007b 	.word	0x007b007b
 e006b68:	007b0128 	.word	0x007b0128
 e006b6c:	00fc007b 	.word	0x00fc007b
 e006b70:	007b007b 	.word	0x007b007b
 e006b74:	007b007b 	.word	0x007b007b
 e006b78:	007b007b 	.word	0x007b007b
 e006b7c:	007b007b 	.word	0x007b007b
 e006b80:	007b0081 	.word	0x007b0081
 e006b84:	007b007b 	.word	0x007b007b
 e006b88:	007b007b 	.word	0x007b007b
 e006b8c:	007b007b 	.word	0x007b007b
 e006b90:	007b007b 	.word	0x007b007b
 e006b94:	012c014f 	.word	0x012c014f
 e006b98:	007b00ca 	.word	0x007b00ca
 e006b9c:	007b007b 	.word	0x007b007b
 e006ba0:	00ca007b 	.word	0x00ca007b
 e006ba4:	007b007b 	.word	0x007b007b
 e006ba8:	007b0128 	.word	0x007b0128
 e006bac:	00fc007b 	.word	0x00fc007b
 e006bb0:	007b01e6 	.word	0x007b01e6
 e006bb4:	01b4007b 	.word	0x01b4007b
 e006bb8:	017d007b 	.word	0x017d007b
 e006bbc:	007b007b 	.word	0x007b007b
 e006bc0:	007b0083 	.word	0x007b0083
 e006bc4:	012a      	.short	0x012a
 e006bc6:	f04f 35ff 	mov.w	r5, #4294967295	; 0xffffffff
 e006bca:	4628      	mov	r0, r5
 e006bcc:	b009      	add	sp, #36	; 0x24
 e006bce:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 e006bd2:	2301      	movs	r3, #1
 e006bd4:	9304      	str	r3, [sp, #16]
 e006bd6:	f1bc 0f01 	cmp.w	ip, #1
 e006bda:	f340 81d4 	ble.w	e006f86 <DiagVprintf+0x4fe>
 e006bde:	f108 0807 	add.w	r8, r8, #7
 e006be2:	f028 0807 	bic.w	r8, r8, #7
 e006be6:	f8d8 1004 	ldr.w	r1, [r8, #4]
 e006bea:	f858 0b08 	ldr.w	r0, [r8], #8
 e006bee:	f1ba 0f00 	cmp.w	sl, #0
 e006bf2:	f000 8168 	beq.w	e006ec6 <DiagVprintf+0x43e>
 e006bf6:	2300      	movs	r3, #0
 e006bf8:	2210      	movs	r2, #16
 e006bfa:	9302      	str	r3, [sp, #8]
 e006bfc:	9b04      	ldr	r3, [sp, #16]
 e006bfe:	9301      	str	r3, [sp, #4]
 e006c00:	2300      	movs	r3, #0
 e006c02:	9300      	str	r3, [sp, #0]
 e006c04:	464b      	mov	r3, r9
 e006c06:	f7ff fe8d 	bl	e006924 <print_unsigned_num>
 e006c0a:	4286      	cmp	r6, r0
 e006c0c:	dd0e      	ble.n	e006c2c <DiagVprintf+0x1a4>
 e006c0e:	f1b9 0f20 	cmp.w	r9, #32
 e006c12:	d10b      	bne.n	e006c2c <DiagVprintf+0x1a4>
 e006c14:	eba6 0900 	sub.w	r9, r6, r0
 e006c18:	f1b9 0f00 	cmp.w	r9, #0
 e006c1c:	f340 81e4 	ble.w	e006fe8 <DiagVprintf+0x560>
 e006c20:	2020      	movs	r0, #32
 e006c22:	47b8      	blx	r7
 e006c24:	f1b9 0901 	subs.w	r9, r9, #1
 e006c28:	d1fa      	bne.n	e006c20 <DiagVprintf+0x198>
 e006c2a:	4630      	mov	r0, r6
 e006c2c:	4405      	add	r5, r0
 e006c2e:	3401      	adds	r4, #1
 e006c30:	e73c      	b.n	e006aac <DiagVprintf+0x24>
 e006c32:	f04f 0920 	mov.w	r9, #32
 e006c36:	2600      	movs	r6, #0
 e006c38:	e005      	b.n	e006c46 <DiagVprintf+0x1be>
 e006c3a:	f814 3f01 	ldrb.w	r3, [r4, #1]!
 e006c3e:	f1a2 0630 	sub.w	r6, r2, #48	; 0x30
 e006c42:	2b00      	cmp	r3, #0
 e006c44:	d0c1      	beq.n	e006bca <DiagVprintf+0x142>
 e006c46:	f1a3 0130 	sub.w	r1, r3, #48	; 0x30
 e006c4a:	eb06 0286 	add.w	r2, r6, r6, lsl #2
 e006c4e:	2909      	cmp	r1, #9
 e006c50:	eb03 0242 	add.w	r2, r3, r2, lsl #1
 e006c54:	d9f1      	bls.n	e006c3a <DiagVprintf+0x1b2>
 e006c56:	e736      	b.n	e006ac6 <DiagVprintf+0x3e>
 e006c58:	3401      	adds	r4, #1
 e006c5a:	4699      	mov	r9, r3
 e006c5c:	7823      	ldrb	r3, [r4, #0]
 e006c5e:	2b00      	cmp	r3, #0
 e006c60:	d1e9      	bne.n	e006c36 <DiagVprintf+0x1ae>
 e006c62:	e7b2      	b.n	e006bca <DiagVprintf+0x142>
 e006c64:	f1bc 0f01 	cmp.w	ip, #1
 e006c68:	f340 8186 	ble.w	e006f78 <DiagVprintf+0x4f0>
 e006c6c:	f108 0807 	add.w	r8, r8, #7
 e006c70:	f028 0807 	bic.w	r8, r8, #7
 e006c74:	f8d8 1004 	ldr.w	r1, [r8, #4]
 e006c78:	f858 0b08 	ldr.w	r0, [r8], #8
 e006c7c:	2900      	cmp	r1, #0
 e006c7e:	f2c0 8189 	blt.w	e006f94 <DiagVprintf+0x50c>
 e006c82:	2200      	movs	r2, #0
 e006c84:	9202      	str	r2, [sp, #8]
 e006c86:	f1ba 0f00 	cmp.w	sl, #0
 e006c8a:	f000 816c 	beq.w	e006f66 <DiagVprintf+0x4de>
 e006c8e:	9b04      	ldr	r3, [sp, #16]
 e006c90:	220a      	movs	r2, #10
 e006c92:	9301      	str	r3, [sp, #4]
 e006c94:	2300      	movs	r3, #0
 e006c96:	9300      	str	r3, [sp, #0]
 e006c98:	464b      	mov	r3, r9
 e006c9a:	f7ff fe43 	bl	e006924 <print_unsigned_num>
 e006c9e:	4286      	cmp	r6, r0
 e006ca0:	ddc4      	ble.n	e006c2c <DiagVprintf+0x1a4>
 e006ca2:	f1b9 0f20 	cmp.w	r9, #32
 e006ca6:	d1c1      	bne.n	e006c2c <DiagVprintf+0x1a4>
 e006ca8:	eba6 0900 	sub.w	r9, r6, r0
 e006cac:	f1b9 0f00 	cmp.w	r9, #0
 e006cb0:	f340 819a 	ble.w	e006fe8 <DiagVprintf+0x560>
 e006cb4:	2020      	movs	r0, #32
 e006cb6:	47b8      	blx	r7
 e006cb8:	f1b9 0901 	subs.w	r9, r9, #1
 e006cbc:	d1fa      	bne.n	e006cb4 <DiagVprintf+0x22c>
 e006cbe:	4630      	mov	r0, r6
 e006cc0:	e7b4      	b.n	e006c2c <DiagVprintf+0x1a4>
 e006cc2:	bf00      	nop
 e006cc4:	0000aba1 	.word	0x0000aba1
 e006cc8:	f1bc 0f01 	cmp.w	ip, #1
 e006ccc:	f340 811e 	ble.w	e006f0c <DiagVprintf+0x484>
 e006cd0:	f108 0807 	add.w	r8, r8, #7
 e006cd4:	f028 0807 	bic.w	r8, r8, #7
 e006cd8:	f8d8 1004 	ldr.w	r1, [r8, #4]
 e006cdc:	f858 0b08 	ldr.w	r0, [r8], #8
 e006ce0:	f1ba 0f00 	cmp.w	sl, #0
 e006ce4:	f000 8119 	beq.w	e006f1a <DiagVprintf+0x492>
 e006ce8:	2300      	movs	r3, #0
 e006cea:	2208      	movs	r2, #8
 e006cec:	9302      	str	r3, [sp, #8]
 e006cee:	9b04      	ldr	r3, [sp, #16]
 e006cf0:	9301      	str	r3, [sp, #4]
 e006cf2:	2300      	movs	r3, #0
 e006cf4:	9300      	str	r3, [sp, #0]
 e006cf6:	464b      	mov	r3, r9
 e006cf8:	f7ff fe14 	bl	e006924 <print_unsigned_num>
 e006cfc:	4286      	cmp	r6, r0
 e006cfe:	dd95      	ble.n	e006c2c <DiagVprintf+0x1a4>
 e006d00:	f1b9 0f20 	cmp.w	r9, #32
 e006d04:	d192      	bne.n	e006c2c <DiagVprintf+0x1a4>
 e006d06:	eba6 0900 	sub.w	r9, r6, r0
 e006d0a:	f1b9 0f00 	cmp.w	r9, #0
 e006d0e:	f340 816b 	ble.w	e006fe8 <DiagVprintf+0x560>
 e006d12:	2020      	movs	r0, #32
 e006d14:	47b8      	blx	r7
 e006d16:	f1b9 0901 	subs.w	r9, r9, #1
 e006d1a:	d1fa      	bne.n	e006d12 <DiagVprintf+0x28a>
 e006d1c:	4630      	mov	r0, r6
 e006d1e:	e785      	b.n	e006c2c <DiagVprintf+0x1a4>
 e006d20:	f10c 0c01 	add.w	ip, ip, #1
 e006d24:	3401      	adds	r4, #1
 e006d26:	e6ce      	b.n	e006ac6 <DiagVprintf+0x3e>
 e006d28:	2e01      	cmp	r6, #1
 e006d2a:	f858 1b04 	ldr.w	r1, [r8], #4
 e006d2e:	bfd4      	ite	le
 e006d30:	2200      	movle	r2, #0
 e006d32:	2201      	movgt	r2, #1
 e006d34:	fa5f fb81 	uxtb.w	fp, r1
 e006d38:	f1ba 0f00 	cmp.w	sl, #0
 e006d3c:	d102      	bne.n	e006d44 <DiagVprintf+0x2bc>
 e006d3e:	2a00      	cmp	r2, #0
 e006d40:	f040 812e 	bne.w	e006fa0 <DiagVprintf+0x518>
 e006d44:	4658      	mov	r0, fp
 e006d46:	9205      	str	r2, [sp, #20]
 e006d48:	47b8      	blx	r7
 e006d4a:	1c69      	adds	r1, r5, #1
 e006d4c:	f1ba 0f00 	cmp.w	sl, #0
 e006d50:	f000 811d 	beq.w	e006f8e <DiagVprintf+0x506>
 e006d54:	9a05      	ldr	r2, [sp, #20]
 e006d56:	2a00      	cmp	r2, #0
 e006d58:	f000 8119 	beq.w	e006f8e <DiagVprintf+0x506>
 e006d5c:	f106 3bff 	add.w	fp, r6, #4294967295	; 0xffffffff
 e006d60:	4648      	mov	r0, r9
 e006d62:	47b8      	blx	r7
 e006d64:	f1bb 0b01 	subs.w	fp, fp, #1
 e006d68:	d1fa      	bne.n	e006d60 <DiagVprintf+0x2d8>
 e006d6a:	4435      	add	r5, r6
 e006d6c:	e75f      	b.n	e006c2e <DiagVprintf+0x1a6>
 e006d6e:	f1bc 0f01 	cmp.w	ip, #1
 e006d72:	f340 80dd 	ble.w	e006f30 <DiagVprintf+0x4a8>
 e006d76:	f108 0807 	add.w	r8, r8, #7
 e006d7a:	f028 0807 	bic.w	r8, r8, #7
 e006d7e:	f8d8 1004 	ldr.w	r1, [r8, #4]
 e006d82:	f858 0b08 	ldr.w	r0, [r8], #8
 e006d86:	f1ba 0f00 	cmp.w	sl, #0
 e006d8a:	f000 80d8 	beq.w	e006f3e <DiagVprintf+0x4b6>
 e006d8e:	2300      	movs	r3, #0
 e006d90:	2202      	movs	r2, #2
 e006d92:	9302      	str	r3, [sp, #8]
 e006d94:	9b04      	ldr	r3, [sp, #16]
 e006d96:	9301      	str	r3, [sp, #4]
 e006d98:	2300      	movs	r3, #0
 e006d9a:	9300      	str	r3, [sp, #0]
 e006d9c:	464b      	mov	r3, r9
 e006d9e:	f7ff fdc1 	bl	e006924 <print_unsigned_num>
 e006da2:	4286      	cmp	r6, r0
 e006da4:	f77f af42 	ble.w	e006c2c <DiagVprintf+0x1a4>
 e006da8:	f1b9 0f20 	cmp.w	r9, #32
 e006dac:	f47f af3e 	bne.w	e006c2c <DiagVprintf+0x1a4>
 e006db0:	eba6 0900 	sub.w	r9, r6, r0
 e006db4:	f1b9 0f00 	cmp.w	r9, #0
 e006db8:	f340 8116 	ble.w	e006fe8 <DiagVprintf+0x560>
 e006dbc:	2020      	movs	r0, #32
 e006dbe:	47b8      	blx	r7
 e006dc0:	f1b9 0901 	subs.w	r9, r9, #1
 e006dc4:	d1fa      	bne.n	e006dbc <DiagVprintf+0x334>
 e006dc6:	4630      	mov	r0, r6
 e006dc8:	e730      	b.n	e006c2c <DiagVprintf+0x1a4>
 e006dca:	f1bc 0f01 	cmp.w	ip, #1
 e006dce:	f340 80c1 	ble.w	e006f54 <DiagVprintf+0x4cc>
 e006dd2:	f108 0807 	add.w	r8, r8, #7
 e006dd6:	f028 0807 	bic.w	r8, r8, #7
 e006dda:	f8d8 1004 	ldr.w	r1, [r8, #4]
 e006dde:	f858 0b08 	ldr.w	r0, [r8], #8
 e006de2:	f1ba 0f00 	cmp.w	sl, #0
 e006de6:	f000 80bc 	beq.w	e006f62 <DiagVprintf+0x4da>
 e006dea:	2300      	movs	r3, #0
 e006dec:	220a      	movs	r2, #10
 e006dee:	9302      	str	r3, [sp, #8]
 e006df0:	9b04      	ldr	r3, [sp, #16]
 e006df2:	9301      	str	r3, [sp, #4]
 e006df4:	2300      	movs	r3, #0
 e006df6:	9300      	str	r3, [sp, #0]
 e006df8:	464b      	mov	r3, r9
 e006dfa:	f7ff fd93 	bl	e006924 <print_unsigned_num>
 e006dfe:	4286      	cmp	r6, r0
 e006e00:	f77f af14 	ble.w	e006c2c <DiagVprintf+0x1a4>
 e006e04:	f1b9 0f20 	cmp.w	r9, #32
 e006e08:	f47f af10 	bne.w	e006c2c <DiagVprintf+0x1a4>
 e006e0c:	eba6 0900 	sub.w	r9, r6, r0
 e006e10:	f1b9 0f00 	cmp.w	r9, #0
 e006e14:	f340 80e8 	ble.w	e006fe8 <DiagVprintf+0x560>
 e006e18:	2020      	movs	r0, #32
 e006e1a:	47b8      	blx	r7
 e006e1c:	f1b9 0901 	subs.w	r9, r9, #1
 e006e20:	d1fa      	bne.n	e006e18 <DiagVprintf+0x390>
 e006e22:	4630      	mov	r0, r6
 e006e24:	e702      	b.n	e006c2c <DiagVprintf+0x1a4>
 e006e26:	3401      	adds	r4, #1
 e006e28:	f10a 0a01 	add.w	sl, sl, #1
 e006e2c:	e64b      	b.n	e006ac6 <DiagVprintf+0x3e>
 e006e2e:	2025      	movs	r0, #37	; 0x25
 e006e30:	3501      	adds	r5, #1
 e006e32:	3401      	adds	r4, #1
 e006e34:	47b8      	blx	r7
 e006e36:	e639      	b.n	e006aac <DiagVprintf+0x24>
 e006e38:	4643      	mov	r3, r8
 e006e3a:	f853 2b04 	ldr.w	r2, [r3], #4
 e006e3e:	7810      	ldrb	r0, [r2, #0]
 e006e40:	9305      	str	r3, [sp, #20]
 e006e42:	2800      	cmp	r0, #0
 e006e44:	f000 80d5 	beq.w	e006ff2 <DiagVprintf+0x56a>
 e006e48:	4694      	mov	ip, r2
 e006e4a:	f1c2 0801 	rsb	r8, r2, #1
 e006e4e:	eb08 010c 	add.w	r1, r8, ip
 e006e52:	f81c 3f01 	ldrb.w	r3, [ip, #1]!
 e006e56:	2b00      	cmp	r3, #0
 e006e58:	d1f9      	bne.n	e006e4e <DiagVprintf+0x3c6>
 e006e5a:	9106      	str	r1, [sp, #24]
 e006e5c:	f1ba 0f00 	cmp.w	sl, #0
 e006e60:	d03c      	beq.n	e006edc <DiagVprintf+0x454>
 e006e62:	2a00      	cmp	r2, #0
 e006e64:	f000 80cb 	beq.w	e006ffe <DiagVprintf+0x576>
 e006e68:	b168      	cbz	r0, e006e86 <DiagVprintf+0x3fe>
 e006e6a:	4693      	mov	fp, r2
 e006e6c:	f1c2 0801 	rsb	r8, r2, #1
 e006e70:	47b8      	blx	r7
 e006e72:	eb0b 0c08 	add.w	ip, fp, r8
 e006e76:	f81b 0f01 	ldrb.w	r0, [fp, #1]!
 e006e7a:	2800      	cmp	r0, #0
 e006e7c:	d1f8      	bne.n	e006e70 <DiagVprintf+0x3e8>
 e006e7e:	4465      	add	r5, ip
 e006e80:	f1ba 0f00 	cmp.w	sl, #0
 e006e84:	d006      	beq.n	e006e94 <DiagVprintf+0x40c>
 e006e86:	9b06      	ldr	r3, [sp, #24]
 e006e88:	eba6 0b03 	sub.w	fp, r6, r3
 e006e8c:	f1bb 0f00 	cmp.w	fp, #0
 e006e90:	f300 80a1 	bgt.w	e006fd6 <DiagVprintf+0x54e>
 e006e94:	f8dd 8014 	ldr.w	r8, [sp, #20]
 e006e98:	3401      	adds	r4, #1
 e006e9a:	e607      	b.n	e006aac <DiagVprintf+0x24>
 e006e9c:	f858 0b04 	ldr.w	r0, [r8], #4
 e006ea0:	f1ba 0f00 	cmp.w	sl, #0
 e006ea4:	d127      	bne.n	e006ef6 <DiagVprintf+0x46e>
 e006ea6:	2308      	movs	r3, #8
 e006ea8:	2201      	movs	r2, #1
 e006eaa:	4651      	mov	r1, sl
 e006eac:	f8cd a008 	str.w	sl, [sp, #8]
 e006eb0:	e9cd 3200 	strd	r3, r2, [sp]
 e006eb4:	2330      	movs	r3, #48	; 0x30
 e006eb6:	2210      	movs	r2, #16
 e006eb8:	f7ff fd34 	bl	e006924 <print_unsigned_num>
 e006ebc:	2301      	movs	r3, #1
 e006ebe:	4405      	add	r5, r0
 e006ec0:	3401      	adds	r4, #1
 e006ec2:	9304      	str	r3, [sp, #16]
 e006ec4:	e5f2      	b.n	e006aac <DiagVprintf+0x24>
 e006ec6:	9a04      	ldr	r2, [sp, #16]
 e006ec8:	464b      	mov	r3, r9
 e006eca:	f8cd a008 	str.w	sl, [sp, #8]
 e006ece:	9201      	str	r2, [sp, #4]
 e006ed0:	2210      	movs	r2, #16
 e006ed2:	9600      	str	r6, [sp, #0]
 e006ed4:	f7ff fd26 	bl	e006924 <print_unsigned_num>
 e006ed8:	4405      	add	r5, r0
 e006eda:	e6a8      	b.n	e006c2e <DiagVprintf+0x1a6>
 e006edc:	9b06      	ldr	r3, [sp, #24]
 e006ede:	eba6 0803 	sub.w	r8, r6, r3
 e006ee2:	f1b8 0f00 	cmp.w	r8, #0
 e006ee6:	dc66      	bgt.n	e006fb6 <DiagVprintf+0x52e>
 e006ee8:	2a00      	cmp	r2, #0
 e006eea:	f000 808a 	beq.w	e007002 <DiagVprintf+0x57a>
 e006eee:	7810      	ldrb	r0, [r2, #0]
 e006ef0:	2800      	cmp	r0, #0
 e006ef2:	d1ba      	bne.n	e006e6a <DiagVprintf+0x3e2>
 e006ef4:	e7ce      	b.n	e006e94 <DiagVprintf+0x40c>
 e006ef6:	2300      	movs	r3, #0
 e006ef8:	2210      	movs	r2, #16
 e006efa:	2100      	movs	r1, #0
 e006efc:	9302      	str	r3, [sp, #8]
 e006efe:	9300      	str	r3, [sp, #0]
 e006f00:	2301      	movs	r3, #1
 e006f02:	9301      	str	r3, [sp, #4]
 e006f04:	2330      	movs	r3, #48	; 0x30
 e006f06:	f7ff fd0d 	bl	e006924 <print_unsigned_num>
 e006f0a:	e7d7      	b.n	e006ebc <DiagVprintf+0x434>
 e006f0c:	f858 0b04 	ldr.w	r0, [r8], #4
 e006f10:	2100      	movs	r1, #0
 e006f12:	f1ba 0f00 	cmp.w	sl, #0
 e006f16:	f47f aee7 	bne.w	e006ce8 <DiagVprintf+0x260>
 e006f1a:	9a04      	ldr	r2, [sp, #16]
 e006f1c:	464b      	mov	r3, r9
 e006f1e:	f8cd a008 	str.w	sl, [sp, #8]
 e006f22:	9201      	str	r2, [sp, #4]
 e006f24:	2208      	movs	r2, #8
 e006f26:	9600      	str	r6, [sp, #0]
 e006f28:	f7ff fcfc 	bl	e006924 <print_unsigned_num>
 e006f2c:	4405      	add	r5, r0
 e006f2e:	e67e      	b.n	e006c2e <DiagVprintf+0x1a6>
 e006f30:	f858 0b04 	ldr.w	r0, [r8], #4
 e006f34:	2100      	movs	r1, #0
 e006f36:	f1ba 0f00 	cmp.w	sl, #0
 e006f3a:	f47f af28 	bne.w	e006d8e <DiagVprintf+0x306>
 e006f3e:	9a04      	ldr	r2, [sp, #16]
 e006f40:	464b      	mov	r3, r9
 e006f42:	f8cd a008 	str.w	sl, [sp, #8]
 e006f46:	9201      	str	r2, [sp, #4]
 e006f48:	2202      	movs	r2, #2
 e006f4a:	9600      	str	r6, [sp, #0]
 e006f4c:	f7ff fcea 	bl	e006924 <print_unsigned_num>
 e006f50:	4405      	add	r5, r0
 e006f52:	e66c      	b.n	e006c2e <DiagVprintf+0x1a6>
 e006f54:	f858 0b04 	ldr.w	r0, [r8], #4
 e006f58:	2100      	movs	r1, #0
 e006f5a:	f1ba 0f00 	cmp.w	sl, #0
 e006f5e:	f47f af44 	bne.w	e006dea <DiagVprintf+0x362>
 e006f62:	f8cd a008 	str.w	sl, [sp, #8]
 e006f66:	9a04      	ldr	r2, [sp, #16]
 e006f68:	464b      	mov	r3, r9
 e006f6a:	9600      	str	r6, [sp, #0]
 e006f6c:	9201      	str	r2, [sp, #4]
 e006f6e:	220a      	movs	r2, #10
 e006f70:	f7ff fcd8 	bl	e006924 <print_unsigned_num>
 e006f74:	4405      	add	r5, r0
 e006f76:	e65a      	b.n	e006c2e <DiagVprintf+0x1a6>
 e006f78:	f8d8 0000 	ldr.w	r0, [r8]
 e006f7c:	f108 0204 	add.w	r2, r8, #4
 e006f80:	17c1      	asrs	r1, r0, #31
 e006f82:	4690      	mov	r8, r2
 e006f84:	e67a      	b.n	e006c7c <DiagVprintf+0x1f4>
 e006f86:	f858 0b04 	ldr.w	r0, [r8], #4
 e006f8a:	2100      	movs	r1, #0
 e006f8c:	e62f      	b.n	e006bee <DiagVprintf+0x166>
 e006f8e:	460d      	mov	r5, r1
 e006f90:	3401      	adds	r4, #1
 e006f92:	e58b      	b.n	e006aac <DiagVprintf+0x24>
 e006f94:	4240      	negs	r0, r0
 e006f96:	f04f 0201 	mov.w	r2, #1
 e006f9a:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
 e006f9e:	e671      	b.n	e006c84 <DiagVprintf+0x1fc>
 e006fa0:	f106 3aff 	add.w	sl, r6, #4294967295	; 0xffffffff
 e006fa4:	4648      	mov	r0, r9
 e006fa6:	47b8      	blx	r7
 e006fa8:	f1ba 0a01 	subs.w	sl, sl, #1
 e006fac:	d1fa      	bne.n	e006fa4 <DiagVprintf+0x51c>
 e006fae:	4658      	mov	r0, fp
 e006fb0:	4435      	add	r5, r6
 e006fb2:	47b8      	blx	r7
 e006fb4:	e63b      	b.n	e006c2e <DiagVprintf+0x1a6>
 e006fb6:	4623      	mov	r3, r4
 e006fb8:	46c3      	mov	fp, r8
 e006fba:	464c      	mov	r4, r9
 e006fbc:	9207      	str	r2, [sp, #28]
 e006fbe:	4699      	mov	r9, r3
 e006fc0:	4620      	mov	r0, r4
 e006fc2:	47b8      	blx	r7
 e006fc4:	f1bb 0b01 	subs.w	fp, fp, #1
 e006fc8:	d1fa      	bne.n	e006fc0 <DiagVprintf+0x538>
 e006fca:	464b      	mov	r3, r9
 e006fcc:	9a07      	ldr	r2, [sp, #28]
 e006fce:	46a1      	mov	r9, r4
 e006fd0:	4445      	add	r5, r8
 e006fd2:	461c      	mov	r4, r3
 e006fd4:	e788      	b.n	e006ee8 <DiagVprintf+0x460>
 e006fd6:	465e      	mov	r6, fp
 e006fd8:	4648      	mov	r0, r9
 e006fda:	47b8      	blx	r7
 e006fdc:	3e01      	subs	r6, #1
 e006fde:	d1fb      	bne.n	e006fd8 <DiagVprintf+0x550>
 e006fe0:	445d      	add	r5, fp
 e006fe2:	f8dd 8014 	ldr.w	r8, [sp, #20]
 e006fe6:	e622      	b.n	e006c2e <DiagVprintf+0x1a6>
 e006fe8:	4606      	mov	r6, r0
 e006fea:	4630      	mov	r0, r6
 e006fec:	e61e      	b.n	e006c2c <DiagVprintf+0x1a4>
 e006fee:	4605      	mov	r5, r0
 e006ff0:	e5eb      	b.n	e006bca <DiagVprintf+0x142>
 e006ff2:	9006      	str	r0, [sp, #24]
 e006ff4:	f1ba 0f00 	cmp.w	sl, #0
 e006ff8:	f47f af33 	bne.w	e006e62 <DiagVprintf+0x3da>
 e006ffc:	e76e      	b.n	e006edc <DiagVprintf+0x454>
 e006ffe:	3d01      	subs	r5, #1
 e007000:	e741      	b.n	e006e86 <DiagVprintf+0x3fe>
 e007002:	3d01      	subs	r5, #1
 e007004:	e746      	b.n	e006e94 <DiagVprintf+0x40c>
 e007006:	bf00      	nop

0e007008 <DiagPrintf>:
 e007008:	b40f      	push	{r0, r1, r2, r3}
 e00700a:	b500      	push	{lr}
 e00700c:	b083      	sub	sp, #12
 e00700e:	a904      	add	r1, sp, #16
 e007010:	f851 0b04 	ldr.w	r0, [r1], #4
 e007014:	9101      	str	r1, [sp, #4]
 e007016:	f7ff fd37 	bl	e006a88 <DiagVprintf>
 e00701a:	b003      	add	sp, #12
 e00701c:	f85d eb04 	ldr.w	lr, [sp], #4
 e007020:	b004      	add	sp, #16
 e007022:	4770      	bx	lr

0e007024 <DiagPrintf_minimal>:
 e007024:	b40f      	push	{r0, r1, r2, r3}
 e007026:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e00702a:	b089      	sub	sp, #36	; 0x24
 e00702c:	ab12      	add	r3, sp, #72	; 0x48
 e00702e:	f853 4b04 	ldr.w	r4, [r3], #4
 e007032:	7820      	ldrb	r0, [r4, #0]
 e007034:	9304      	str	r3, [sp, #16]
 e007036:	2800      	cmp	r0, #0
 e007038:	d040      	beq.n	e0070bc <DiagPrintf_minimal+0x98>
 e00703a:	4699      	mov	r9, r3
 e00703c:	2700      	movs	r7, #0
 e00703e:	f8df 82cc 	ldr.w	r8, [pc, #716]	; e00730c <DiagPrintf_minimal+0x2e8>
 e007042:	e004      	b.n	e00704e <DiagPrintf_minimal+0x2a>
 e007044:	3701      	adds	r7, #1
 e007046:	47c0      	blx	r8
 e007048:	7820      	ldrb	r0, [r4, #0]
 e00704a:	2800      	cmp	r0, #0
 e00704c:	d04c      	beq.n	e0070e8 <DiagPrintf_minimal+0xc4>
 e00704e:	2825      	cmp	r0, #37	; 0x25
 e007050:	f104 0401 	add.w	r4, r4, #1
 e007054:	d1f6      	bne.n	e007044 <DiagPrintf_minimal+0x20>
 e007056:	2500      	movs	r5, #0
 e007058:	462e      	mov	r6, r5
 e00705a:	7823      	ldrb	r3, [r4, #0]
 e00705c:	2b43      	cmp	r3, #67	; 0x43
 e00705e:	d850      	bhi.n	e007102 <DiagPrintf_minimal+0xde>
 e007060:	2b1f      	cmp	r3, #31
 e007062:	d929      	bls.n	e0070b8 <DiagPrintf_minimal+0x94>
 e007064:	f1a3 0220 	sub.w	r2, r3, #32
 e007068:	2a23      	cmp	r2, #35	; 0x23
 e00706a:	d825      	bhi.n	e0070b8 <DiagPrintf_minimal+0x94>
 e00706c:	e8df f012 	tbh	[pc, r2, lsl #1]
 e007070:	00240042 	.word	0x00240042
 e007074:	00240024 	.word	0x00240024
 e007078:	012f0024 	.word	0x012f0024
 e00707c:	00240024 	.word	0x00240024
 e007080:	00240024 	.word	0x00240024
 e007084:	00240024 	.word	0x00240024
 e007088:	00240024 	.word	0x00240024
 e00708c:	00240024 	.word	0x00240024
 e007090:	002b0042 	.word	0x002b0042
 e007094:	002b002b 	.word	0x002b002b
 e007098:	002b002b 	.word	0x002b002b
 e00709c:	002b002b 	.word	0x002b002b
 e0070a0:	002b002b 	.word	0x002b002b
 e0070a4:	00240024 	.word	0x00240024
 e0070a8:	00240024 	.word	0x00240024
 e0070ac:	00240024 	.word	0x00240024
 e0070b0:	00240024 	.word	0x00240024
 e0070b4:	00c00024 	.word	0x00c00024
 e0070b8:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
 e0070bc:	b009      	add	sp, #36	; 0x24
 e0070be:	e8bd 4ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e0070c2:	b004      	add	sp, #16
 e0070c4:	4770      	bx	lr
 e0070c6:	2620      	movs	r6, #32
 e0070c8:	2500      	movs	r5, #0
 e0070ca:	f1a3 0130 	sub.w	r1, r3, #48	; 0x30
 e0070ce:	eb05 0285 	add.w	r2, r5, r5, lsl #2
 e0070d2:	b2c9      	uxtb	r1, r1
 e0070d4:	eb03 0242 	add.w	r2, r3, r2, lsl #1
 e0070d8:	2909      	cmp	r1, #9
 e0070da:	d8be      	bhi.n	e00705a <DiagPrintf_minimal+0x36>
 e0070dc:	f814 3f01 	ldrb.w	r3, [r4, #1]!
 e0070e0:	f1a2 0530 	sub.w	r5, r2, #48	; 0x30
 e0070e4:	2b00      	cmp	r3, #0
 e0070e6:	d1f0      	bne.n	e0070ca <DiagPrintf_minimal+0xa6>
 e0070e8:	4638      	mov	r0, r7
 e0070ea:	b009      	add	sp, #36	; 0x24
 e0070ec:	e8bd 4ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e0070f0:	b004      	add	sp, #16
 e0070f2:	4770      	bx	lr
 e0070f4:	7862      	ldrb	r2, [r4, #1]
 e0070f6:	3401      	adds	r4, #1
 e0070f8:	2a00      	cmp	r2, #0
 e0070fa:	d0f5      	beq.n	e0070e8 <DiagPrintf_minimal+0xc4>
 e0070fc:	461e      	mov	r6, r3
 e0070fe:	4613      	mov	r3, r2
 e007100:	e7e2      	b.n	e0070c8 <DiagPrintf_minimal+0xa4>
 e007102:	3b63      	subs	r3, #99	; 0x63
 e007104:	b2da      	uxtb	r2, r3
 e007106:	2a15      	cmp	r2, #21
 e007108:	d8d6      	bhi.n	e0070b8 <DiagPrintf_minimal+0x94>
 e00710a:	2b15      	cmp	r3, #21
 e00710c:	d8d4      	bhi.n	e0070b8 <DiagPrintf_minimal+0x94>
 e00710e:	a201      	add	r2, pc, #4	; (adr r2, e007114 <DiagPrintf_minimal+0xf0>)
 e007110:	f852 f023 	ldr.w	pc, [r2, r3, lsl #2]
 e007114:	0e0071f1 	.word	0x0e0071f1
 e007118:	0e00716d 	.word	0x0e00716d
 e00711c:	0e0070b9 	.word	0x0e0070b9
 e007120:	0e0070b9 	.word	0x0e0070b9
 e007124:	0e0070b9 	.word	0x0e0070b9
 e007128:	0e0070b9 	.word	0x0e0070b9
 e00712c:	0e0070b9 	.word	0x0e0070b9
 e007130:	0e0070b9 	.word	0x0e0070b9
 e007134:	0e0070b9 	.word	0x0e0070b9
 e007138:	0e0070b9 	.word	0x0e0070b9
 e00713c:	0e0070b9 	.word	0x0e0070b9
 e007140:	0e0070b9 	.word	0x0e0070b9
 e007144:	0e0070b9 	.word	0x0e0070b9
 e007148:	0e0070b9 	.word	0x0e0070b9
 e00714c:	0e0070b9 	.word	0x0e0070b9
 e007150:	0e0070b9 	.word	0x0e0070b9
 e007154:	0e007207 	.word	0x0e007207
 e007158:	0e0070b9 	.word	0x0e0070b9
 e00715c:	0e00716d 	.word	0x0e00716d
 e007160:	0e0070b9 	.word	0x0e0070b9
 e007164:	0e0070b9 	.word	0x0e0070b9
 e007168:	0e00724d 	.word	0x0e00724d
 e00716c:	464b      	mov	r3, r9
 e00716e:	f10d 0a14 	add.w	sl, sp, #20
 e007172:	2200      	movs	r2, #0
 e007174:	f853 1b04 	ldr.w	r1, [r3], #4
 e007178:	4650      	mov	r0, sl
 e00717a:	9301      	str	r3, [sp, #4]
 e00717c:	4b62      	ldr	r3, [pc, #392]	; (e007308 <DiagPrintf_minimal+0x2e4>)
 e00717e:	4693      	mov	fp, r2
 e007180:	3201      	adds	r2, #1
 e007182:	fba3 3c01 	umull	r3, ip, r3, r1
 e007186:	460b      	mov	r3, r1
 e007188:	ea4f 0cdc 	mov.w	ip, ip, lsr #3
 e00718c:	2b09      	cmp	r3, #9
 e00718e:	eb0c 098c 	add.w	r9, ip, ip, lsl #2
 e007192:	eba1 0949 	sub.w	r9, r1, r9, lsl #1
 e007196:	4661      	mov	r1, ip
 e007198:	f109 0930 	add.w	r9, r9, #48	; 0x30
 e00719c:	fa5f f989 	uxtb.w	r9, r9
 e0071a0:	f800 9b01 	strb.w	r9, [r0], #1
 e0071a4:	d8ea      	bhi.n	e00717c <DiagPrintf_minimal+0x158>
 e0071a6:	2d00      	cmp	r5, #0
 e0071a8:	f340 8095 	ble.w	e0072d6 <DiagPrintf_minimal+0x2b2>
 e0071ac:	4295      	cmp	r5, r2
 e0071ae:	f340 8092 	ble.w	e0072d6 <DiagPrintf_minimal+0x2b2>
 e0071b2:	f8cd a00c 	str.w	sl, [sp, #12]
 e0071b6:	46a2      	mov	sl, r4
 e0071b8:	4614      	mov	r4, r2
 e0071ba:	9502      	str	r5, [sp, #8]
 e0071bc:	3d01      	subs	r5, #1
 e0071be:	4630      	mov	r0, r6
 e0071c0:	47c0      	blx	r8
 e0071c2:	42ac      	cmp	r4, r5
 e0071c4:	d1fa      	bne.n	e0071bc <DiagPrintf_minimal+0x198>
 e0071c6:	4622      	mov	r2, r4
 e0071c8:	9d02      	ldr	r5, [sp, #8]
 e0071ca:	4654      	mov	r4, sl
 e0071cc:	f8dd a00c 	ldr.w	sl, [sp, #12]
 e0071d0:	1aad      	subs	r5, r5, r2
 e0071d2:	eb0a 060b 	add.w	r6, sl, fp
 e0071d6:	4648      	mov	r0, r9
 e0071d8:	e001      	b.n	e0071de <DiagPrintf_minimal+0x1ba>
 e0071da:	f816 0d01 	ldrb.w	r0, [r6, #-1]!
 e0071de:	47c0      	blx	r8
 e0071e0:	45b2      	cmp	sl, r6
 e0071e2:	d1fa      	bne.n	e0071da <DiagPrintf_minimal+0x1b6>
 e0071e4:	3701      	adds	r7, #1
 e0071e6:	f8dd 9004 	ldr.w	r9, [sp, #4]
 e0071ea:	445f      	add	r7, fp
 e0071ec:	442f      	add	r7, r5
 e0071ee:	e008      	b.n	e007202 <DiagPrintf_minimal+0x1de>
 e0071f0:	f859 3b04 	ldr.w	r3, [r9], #4
 e0071f4:	2d01      	cmp	r5, #1
 e0071f6:	fa5f fb83 	uxtb.w	fp, r3
 e0071fa:	dc78      	bgt.n	e0072ee <DiagPrintf_minimal+0x2ca>
 e0071fc:	3701      	adds	r7, #1
 e0071fe:	4658      	mov	r0, fp
 e007200:	47c0      	blx	r8
 e007202:	3401      	adds	r4, #1
 e007204:	e720      	b.n	e007048 <DiagPrintf_minimal+0x24>
 e007206:	f859 ab04 	ldr.w	sl, [r9], #4
 e00720a:	f89a 3000 	ldrb.w	r3, [sl]
 e00720e:	b14b      	cbz	r3, e007224 <DiagPrintf_minimal+0x200>
 e007210:	4653      	mov	r3, sl
 e007212:	f1ca 0b01 	rsb	fp, sl, #1
 e007216:	eb0b 0103 	add.w	r1, fp, r3
 e00721a:	f813 2f01 	ldrb.w	r2, [r3, #1]!
 e00721e:	2a00      	cmp	r2, #0
 e007220:	d1f9      	bne.n	e007216 <DiagPrintf_minimal+0x1f2>
 e007222:	1a6d      	subs	r5, r5, r1
 e007224:	2d00      	cmp	r5, #0
 e007226:	dc5a      	bgt.n	e0072de <DiagPrintf_minimal+0x2ba>
 e007228:	f1ba 0f00 	cmp.w	sl, #0
 e00722c:	d069      	beq.n	e007302 <DiagPrintf_minimal+0x2de>
 e00722e:	f89a 0000 	ldrb.w	r0, [sl]
 e007232:	2800      	cmp	r0, #0
 e007234:	d0e5      	beq.n	e007202 <DiagPrintf_minimal+0x1de>
 e007236:	f1ca 0b01 	rsb	fp, sl, #1
 e00723a:	47c0      	blx	r8
 e00723c:	eb0a 030b 	add.w	r3, sl, fp
 e007240:	f81a 0f01 	ldrb.w	r0, [sl, #1]!
 e007244:	2800      	cmp	r0, #0
 e007246:	d1f8      	bne.n	e00723a <DiagPrintf_minimal+0x216>
 e007248:	441f      	add	r7, r3
 e00724a:	e7da      	b.n	e007202 <DiagPrintf_minimal+0x1de>
 e00724c:	464a      	mov	r2, r9
 e00724e:	f10d 0a14 	add.w	sl, sp, #20
 e007252:	f04f 0b00 	mov.w	fp, #0
 e007256:	f852 3b04 	ldr.w	r3, [r2], #4
 e00725a:	4650      	mov	r0, sl
 e00725c:	9201      	str	r2, [sp, #4]
 e00725e:	e000      	b.n	e007262 <DiagPrintf_minimal+0x23e>
 e007260:	468b      	mov	fp, r1
 e007262:	f003 020f 	and.w	r2, r3, #15
 e007266:	f10b 0101 	add.w	r1, fp, #1
 e00726a:	2a09      	cmp	r2, #9
 e00726c:	f102 0957 	add.w	r9, r2, #87	; 0x57
 e007270:	bf98      	it	ls
 e007272:	f102 0930 	addls.w	r9, r2, #48	; 0x30
 e007276:	091a      	lsrs	r2, r3, #4
 e007278:	2b0f      	cmp	r3, #15
 e00727a:	f800 9b01 	strb.w	r9, [r0], #1
 e00727e:	4613      	mov	r3, r2
 e007280:	d8ee      	bhi.n	e007260 <DiagPrintf_minimal+0x23c>
 e007282:	2d00      	cmp	r5, #0
 e007284:	dd29      	ble.n	e0072da <DiagPrintf_minimal+0x2b6>
 e007286:	428d      	cmp	r5, r1
 e007288:	dd27      	ble.n	e0072da <DiagPrintf_minimal+0x2b6>
 e00728a:	462b      	mov	r3, r5
 e00728c:	9502      	str	r5, [sp, #8]
 e00728e:	f8cd a00c 	str.w	sl, [sp, #12]
 e007292:	4635      	mov	r5, r6
 e007294:	46a2      	mov	sl, r4
 e007296:	461e      	mov	r6, r3
 e007298:	460c      	mov	r4, r1
 e00729a:	3e01      	subs	r6, #1
 e00729c:	4628      	mov	r0, r5
 e00729e:	47c0      	blx	r8
 e0072a0:	42b4      	cmp	r4, r6
 e0072a2:	d1fa      	bne.n	e00729a <DiagPrintf_minimal+0x276>
 e0072a4:	4621      	mov	r1, r4
 e0072a6:	9d02      	ldr	r5, [sp, #8]
 e0072a8:	4654      	mov	r4, sl
 e0072aa:	f8dd a00c 	ldr.w	sl, [sp, #12]
 e0072ae:	1a6d      	subs	r5, r5, r1
 e0072b0:	eb0a 060b 	add.w	r6, sl, fp
 e0072b4:	4648      	mov	r0, r9
 e0072b6:	e001      	b.n	e0072bc <DiagPrintf_minimal+0x298>
 e0072b8:	f816 0d01 	ldrb.w	r0, [r6, #-1]!
 e0072bc:	47c0      	blx	r8
 e0072be:	4556      	cmp	r6, sl
 e0072c0:	d1fa      	bne.n	e0072b8 <DiagPrintf_minimal+0x294>
 e0072c2:	3701      	adds	r7, #1
 e0072c4:	f8dd 9004 	ldr.w	r9, [sp, #4]
 e0072c8:	445f      	add	r7, fp
 e0072ca:	442f      	add	r7, r5
 e0072cc:	e799      	b.n	e007202 <DiagPrintf_minimal+0x1de>
 e0072ce:	2025      	movs	r0, #37	; 0x25
 e0072d0:	3701      	adds	r7, #1
 e0072d2:	47c0      	blx	r8
 e0072d4:	e795      	b.n	e007202 <DiagPrintf_minimal+0x1de>
 e0072d6:	2500      	movs	r5, #0
 e0072d8:	e77b      	b.n	e0071d2 <DiagPrintf_minimal+0x1ae>
 e0072da:	2500      	movs	r5, #0
 e0072dc:	e7e8      	b.n	e0072b0 <DiagPrintf_minimal+0x28c>
 e0072de:	46ab      	mov	fp, r5
 e0072e0:	4630      	mov	r0, r6
 e0072e2:	47c0      	blx	r8
 e0072e4:	f1bb 0b01 	subs.w	fp, fp, #1
 e0072e8:	d1fa      	bne.n	e0072e0 <DiagPrintf_minimal+0x2bc>
 e0072ea:	442f      	add	r7, r5
 e0072ec:	e79c      	b.n	e007228 <DiagPrintf_minimal+0x204>
 e0072ee:	f105 3aff 	add.w	sl, r5, #4294967295	; 0xffffffff
 e0072f2:	4630      	mov	r0, r6
 e0072f4:	47c0      	blx	r8
 e0072f6:	f1ba 0a01 	subs.w	sl, sl, #1
 e0072fa:	d1fa      	bne.n	e0072f2 <DiagPrintf_minimal+0x2ce>
 e0072fc:	3f01      	subs	r7, #1
 e0072fe:	442f      	add	r7, r5
 e007300:	e77c      	b.n	e0071fc <DiagPrintf_minimal+0x1d8>
 e007302:	3f01      	subs	r7, #1
 e007304:	e77d      	b.n	e007202 <DiagPrintf_minimal+0x1de>
 e007306:	bf00      	nop
 e007308:	cccccccd 	.word	0xcccccccd
 e00730c:	0000aba1 	.word	0x0000aba1

0e007310 <TRNG_get_random_bytes>:
 e007310:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
 e007314:	088e      	lsrs	r6, r1, #2
 e007316:	b083      	sub	sp, #12
 e007318:	4688      	mov	r8, r1
 e00731a:	4681      	mov	r9, r0
 e00731c:	d021      	beq.n	e007362 <TRNG_get_random_bytes+0x52>
 e00731e:	1f05      	subs	r5, r0, #4
 e007320:	2400      	movs	r4, #0
 e007322:	4f11      	ldr	r7, [pc, #68]	; (e007368 <TRNG_get_random_bytes+0x58>)
 e007324:	3401      	adds	r4, #1
 e007326:	47b8      	blx	r7
 e007328:	f845 0f04 	str.w	r0, [r5, #4]!
 e00732c:	42a6      	cmp	r6, r4
 e00732e:	d1f9      	bne.n	e007324 <TRNG_get_random_bytes+0x14>
 e007330:	ebc4 7384 	rsb	r3, r4, r4, lsl #30
 e007334:	eb08 0883 	add.w	r8, r8, r3, lsl #2
 e007338:	f1b8 0f00 	cmp.w	r8, #0
 e00733c:	d103      	bne.n	e007346 <TRNG_get_random_bytes+0x36>
 e00733e:	2000      	movs	r0, #0
 e007340:	b003      	add	sp, #12
 e007342:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 e007346:	4b08      	ldr	r3, [pc, #32]	; (e007368 <TRNG_get_random_bytes+0x58>)
 e007348:	4798      	blx	r3
 e00734a:	4605      	mov	r5, r0
 e00734c:	4642      	mov	r2, r8
 e00734e:	a901      	add	r1, sp, #4
 e007350:	eb09 0084 	add.w	r0, r9, r4, lsl #2
 e007354:	4b05      	ldr	r3, [pc, #20]	; (e00736c <TRNG_get_random_bytes+0x5c>)
 e007356:	9501      	str	r5, [sp, #4]
 e007358:	4798      	blx	r3
 e00735a:	2000      	movs	r0, #0
 e00735c:	b003      	add	sp, #12
 e00735e:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
 e007362:	4634      	mov	r4, r6
 e007364:	e7e8      	b.n	e007338 <TRNG_get_random_bytes+0x28>
 e007366:	bf00      	nop
 e007368:	0000e1b9 	.word	0x0000e1b9
 e00736c:	00011c1d 	.word	0x00011c1d

0e007370 <io_assert_failed>:
 e007370:	b508      	push	{r3, lr}
 e007372:	4603      	mov	r3, r0
 e007374:	460a      	mov	r2, r1
 e007376:	4802      	ldr	r0, [pc, #8]	; (e007380 <io_assert_failed+0x10>)
 e007378:	4619      	mov	r1, r3
 e00737a:	f7ff fe53 	bl	e007024 <DiagPrintf_minimal>
 e00737e:	e7fe      	b.n	e00737e <io_assert_failed+0xe>
 e007380:	0e0098e0 	.word	0x0e0098e0

0e007384 <IPC_SEMTake>:
 e007384:	283f      	cmp	r0, #63	; 0x3f
 e007386:	b570      	push	{r4, r5, r6, lr}
 e007388:	d816      	bhi.n	e0073b8 <IPC_SEMTake+0x34>
 e00738a:	4d0d      	ldr	r5, [pc, #52]	; (e0073c0 <IPC_SEMTake+0x3c>)
 e00738c:	4405      	add	r5, r0
 e00738e:	00ad      	lsls	r5, r5, #2
 e007390:	682b      	ldr	r3, [r5, #0]
 e007392:	b17b      	cbz	r3, e0073b4 <IPC_SEMTake+0x30>
 e007394:	460c      	mov	r4, r1
 e007396:	b159      	cbz	r1, e0073b0 <IPC_SEMTake+0x2c>
 e007398:	4e0a      	ldr	r6, [pc, #40]	; (e0073c4 <IPC_SEMTake+0x40>)
 e00739a:	e001      	b.n	e0073a0 <IPC_SEMTake+0x1c>
 e00739c:	682b      	ldr	r3, [r5, #0]
 e00739e:	b14b      	cbz	r3, e0073b4 <IPC_SEMTake+0x30>
 e0073a0:	6833      	ldr	r3, [r6, #0]
 e0073a2:	b10b      	cbz	r3, e0073a8 <IPC_SEMTake+0x24>
 e0073a4:	2001      	movs	r0, #1
 e0073a6:	4798      	blx	r3
 e0073a8:	1c63      	adds	r3, r4, #1
 e0073aa:	d0f7      	beq.n	e00739c <IPC_SEMTake+0x18>
 e0073ac:	3c01      	subs	r4, #1
 e0073ae:	d1f5      	bne.n	e00739c <IPC_SEMTake+0x18>
 e0073b0:	2000      	movs	r0, #0
 e0073b2:	bd70      	pop	{r4, r5, r6, pc}
 e0073b4:	2001      	movs	r0, #1
 e0073b6:	bd70      	pop	{r4, r5, r6, pc}
 e0073b8:	21f8      	movs	r1, #248	; 0xf8
 e0073ba:	4803      	ldr	r0, [pc, #12]	; (e0073c8 <IPC_SEMTake+0x44>)
 e0073bc:	f7ff ffd8 	bl	e007370 <io_assert_failed>
 e0073c0:	10405080 	.word	0x10405080
 e0073c4:	2000bf80 	.word	0x2000bf80
 e0073c8:	0e009914 	.word	0x0e009914

0e0073cc <deregister_tm_clones>:
 e0073cc:	4803      	ldr	r0, [pc, #12]	; (e0073dc <deregister_tm_clones+0x10>)
 e0073ce:	4b04      	ldr	r3, [pc, #16]	; (e0073e0 <deregister_tm_clones+0x14>)
 e0073d0:	4283      	cmp	r3, r0
 e0073d2:	d002      	beq.n	e0073da <deregister_tm_clones+0xe>
 e0073d4:	4b03      	ldr	r3, [pc, #12]	; (e0073e4 <deregister_tm_clones+0x18>)
 e0073d6:	b103      	cbz	r3, e0073da <deregister_tm_clones+0xe>
 e0073d8:	4718      	bx	r3
 e0073da:	4770      	bx	lr
 e0073dc:	2000b020 	.word	0x2000b020
 e0073e0:	2000b020 	.word	0x2000b020
 e0073e4:	00000000 	.word	0x00000000

0e0073e8 <register_tm_clones>:
 e0073e8:	4805      	ldr	r0, [pc, #20]	; (e007400 <register_tm_clones+0x18>)
 e0073ea:	4b06      	ldr	r3, [pc, #24]	; (e007404 <register_tm_clones+0x1c>)
 e0073ec:	1a1b      	subs	r3, r3, r0
 e0073ee:	0fd9      	lsrs	r1, r3, #31
 e0073f0:	eb01 01a3 	add.w	r1, r1, r3, asr #2
 e0073f4:	1049      	asrs	r1, r1, #1
 e0073f6:	d002      	beq.n	e0073fe <register_tm_clones+0x16>
 e0073f8:	4b03      	ldr	r3, [pc, #12]	; (e007408 <register_tm_clones+0x20>)
 e0073fa:	b103      	cbz	r3, e0073fe <register_tm_clones+0x16>
 e0073fc:	4718      	bx	r3
 e0073fe:	4770      	bx	lr
 e007400:	2000b020 	.word	0x2000b020
 e007404:	2000b020 	.word	0x2000b020
 e007408:	00000000 	.word	0x00000000

0e00740c <__do_global_dtors_aux>:
 e00740c:	b510      	push	{r4, lr}
 e00740e:	4c06      	ldr	r4, [pc, #24]	; (e007428 <__do_global_dtors_aux+0x1c>)
 e007410:	7823      	ldrb	r3, [r4, #0]
 e007412:	b943      	cbnz	r3, e007426 <__do_global_dtors_aux+0x1a>
 e007414:	f7ff ffda 	bl	e0073cc <deregister_tm_clones>
 e007418:	4b04      	ldr	r3, [pc, #16]	; (e00742c <__do_global_dtors_aux+0x20>)
 e00741a:	b113      	cbz	r3, e007422 <__do_global_dtors_aux+0x16>
 e00741c:	4804      	ldr	r0, [pc, #16]	; (e007430 <__do_global_dtors_aux+0x24>)
 e00741e:	f3af 8000 	nop.w
 e007422:	2301      	movs	r3, #1
 e007424:	7023      	strb	r3, [r4, #0]
 e007426:	bd10      	pop	{r4, pc}
 e007428:	2000bf84 	.word	0x2000bf84
 e00742c:	00000000 	.word	0x00000000
 e007430:	0e009aa8 	.word	0x0e009aa8

0e007434 <frame_dummy>:
 e007434:	b508      	push	{r3, lr}
 e007436:	4b04      	ldr	r3, [pc, #16]	; (e007448 <frame_dummy+0x14>)
 e007438:	b11b      	cbz	r3, e007442 <frame_dummy+0xe>
 e00743a:	4904      	ldr	r1, [pc, #16]	; (e00744c <frame_dummy+0x18>)
 e00743c:	4804      	ldr	r0, [pc, #16]	; (e007450 <frame_dummy+0x1c>)
 e00743e:	f3af 8000 	nop.w
 e007442:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
 e007446:	e7cf      	b.n	e0073e8 <register_tm_clones>
 e007448:	00000000 	.word	0x00000000
 e00744c:	2000bf88 	.word	0x2000bf88
 e007450:	0e009aa8 	.word	0x0e009aa8

0e007454 <_GLOBAL__sub_I__ZN9__gnu_cxx9__freeresEv>:
 e007454:	b538      	push	{r3, r4, r5, lr}
 e007456:	f44f 651e 	mov.w	r5, #2528	; 0x9e0
 e00745a:	4c07      	ldr	r4, [pc, #28]	; (e007478 <_GLOBAL__sub_I__ZN9__gnu_cxx9__freeresEv+0x24>)
 e00745c:	4628      	mov	r0, r5
 e00745e:	60e5      	str	r5, [r4, #12]
 e007460:	f7fa fe20 	bl	e0020a4 <__wrap_malloc>
 e007464:	60a0      	str	r0, [r4, #8]
 e007466:	b910      	cbnz	r0, e00746e <_GLOBAL__sub_I__ZN9__gnu_cxx9__freeresEv+0x1a>
 e007468:	60e0      	str	r0, [r4, #12]
 e00746a:	6060      	str	r0, [r4, #4]
 e00746c:	bd38      	pop	{r3, r4, r5, pc}
 e00746e:	2300      	movs	r3, #0
 e007470:	6060      	str	r0, [r4, #4]
 e007472:	6005      	str	r5, [r0, #0]
 e007474:	6043      	str	r3, [r0, #4]
 e007476:	e7f9      	b.n	e00746c <_GLOBAL__sub_I__ZN9__gnu_cxx9__freeresEv+0x18>
 e007478:	2000bfa0 	.word	0x2000bfa0

0e00747c <__aeabi_uldivmod>:
 e00747c:	b953      	cbnz	r3, e007494 <__aeabi_uldivmod+0x18>
 e00747e:	b94a      	cbnz	r2, e007494 <__aeabi_uldivmod+0x18>
 e007480:	2900      	cmp	r1, #0
 e007482:	bf08      	it	eq
 e007484:	2800      	cmpeq	r0, #0
 e007486:	bf1c      	itt	ne
 e007488:	f04f 31ff 	movne.w	r1, #4294967295	; 0xffffffff
 e00748c:	f04f 30ff 	movne.w	r0, #4294967295	; 0xffffffff
 e007490:	f000 b960 	b.w	e007754 <__aeabi_idiv0>
 e007494:	f1ad 0c08 	sub.w	ip, sp, #8
 e007498:	e96d ce04 	strd	ip, lr, [sp, #-16]!
 e00749c:	f000 f806 	bl	e0074ac <__udivmoddi4>
 e0074a0:	f8dd e004 	ldr.w	lr, [sp, #4]
 e0074a4:	e9dd 2302 	ldrd	r2, r3, [sp, #8]
 e0074a8:	b004      	add	sp, #16
 e0074aa:	4770      	bx	lr

0e0074ac <__udivmoddi4>:
 e0074ac:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
 e0074b0:	9e09      	ldr	r6, [sp, #36]	; 0x24
 e0074b2:	4688      	mov	r8, r1
 e0074b4:	468e      	mov	lr, r1
 e0074b6:	4615      	mov	r5, r2
 e0074b8:	4604      	mov	r4, r0
 e0074ba:	4619      	mov	r1, r3
 e0074bc:	2b00      	cmp	r3, #0
 e0074be:	f040 80c6 	bne.w	e00764e <__udivmoddi4+0x1a2>
 e0074c2:	4542      	cmp	r2, r8
 e0074c4:	fab2 f782 	clz	r7, r2
 e0074c8:	d946      	bls.n	e007558 <__udivmoddi4+0xac>
 e0074ca:	b14f      	cbz	r7, e0074e0 <__udivmoddi4+0x34>
 e0074cc:	f1c7 0c20 	rsb	ip, r7, #32
 e0074d0:	fa08 f307 	lsl.w	r3, r8, r7
 e0074d4:	40bd      	lsls	r5, r7
 e0074d6:	40bc      	lsls	r4, r7
 e0074d8:	fa20 fc0c 	lsr.w	ip, r0, ip
 e0074dc:	ea4c 0e03 	orr.w	lr, ip, r3
 e0074e0:	ea4f 4815 	mov.w	r8, r5, lsr #16
 e0074e4:	fa1f fc85 	uxth.w	ip, r5
 e0074e8:	0c22      	lsrs	r2, r4, #16
 e0074ea:	fbbe f9f8 	udiv	r9, lr, r8
 e0074ee:	fb08 e319 	mls	r3, r8, r9, lr
 e0074f2:	fb09 fa0c 	mul.w	sl, r9, ip
 e0074f6:	ea42 4303 	orr.w	r3, r2, r3, lsl #16
 e0074fa:	459a      	cmp	sl, r3
 e0074fc:	d928      	bls.n	e007550 <__udivmoddi4+0xa4>
 e0074fe:	18eb      	adds	r3, r5, r3
 e007500:	f109 30ff 	add.w	r0, r9, #4294967295	; 0xffffffff
 e007504:	d204      	bcs.n	e007510 <__udivmoddi4+0x64>
 e007506:	459a      	cmp	sl, r3
 e007508:	d902      	bls.n	e007510 <__udivmoddi4+0x64>
 e00750a:	f1a9 0002 	sub.w	r0, r9, #2
 e00750e:	442b      	add	r3, r5
 e007510:	eba3 030a 	sub.w	r3, r3, sl
 e007514:	b2a4      	uxth	r4, r4
 e007516:	fbb3 f2f8 	udiv	r2, r3, r8
 e00751a:	fb08 3312 	mls	r3, r8, r2, r3
 e00751e:	fb02 fc0c 	mul.w	ip, r2, ip
 e007522:	ea44 4403 	orr.w	r4, r4, r3, lsl #16
 e007526:	45a4      	cmp	ip, r4
 e007528:	d914      	bls.n	e007554 <__udivmoddi4+0xa8>
 e00752a:	192c      	adds	r4, r5, r4
 e00752c:	f102 33ff 	add.w	r3, r2, #4294967295	; 0xffffffff
 e007530:	d203      	bcs.n	e00753a <__udivmoddi4+0x8e>
 e007532:	45a4      	cmp	ip, r4
 e007534:	d901      	bls.n	e00753a <__udivmoddi4+0x8e>
 e007536:	1e93      	subs	r3, r2, #2
 e007538:	442c      	add	r4, r5
 e00753a:	eba4 040c 	sub.w	r4, r4, ip
 e00753e:	ea43 4000 	orr.w	r0, r3, r0, lsl #16
 e007542:	b11e      	cbz	r6, e00754c <__udivmoddi4+0xa0>
 e007544:	40fc      	lsrs	r4, r7
 e007546:	2300      	movs	r3, #0
 e007548:	6034      	str	r4, [r6, #0]
 e00754a:	6073      	str	r3, [r6, #4]
 e00754c:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
 e007550:	4648      	mov	r0, r9
 e007552:	e7dd      	b.n	e007510 <__udivmoddi4+0x64>
 e007554:	4613      	mov	r3, r2
 e007556:	e7f0      	b.n	e00753a <__udivmoddi4+0x8e>
 e007558:	b902      	cbnz	r2, e00755c <__udivmoddi4+0xb0>
 e00755a:	deff      	udf	#255	; 0xff
 e00755c:	bb8f      	cbnz	r7, e0075c2 <__udivmoddi4+0x116>
 e00755e:	eba8 0302 	sub.w	r3, r8, r2
 e007562:	2101      	movs	r1, #1
 e007564:	ea4f 4e15 	mov.w	lr, r5, lsr #16
 e007568:	b2aa      	uxth	r2, r5
 e00756a:	0c20      	lsrs	r0, r4, #16
 e00756c:	fbb3 fcfe 	udiv	ip, r3, lr
 e007570:	fb0e 331c 	mls	r3, lr, ip, r3
 e007574:	fb0c f802 	mul.w	r8, ip, r2
 e007578:	ea40 4303 	orr.w	r3, r0, r3, lsl #16
 e00757c:	4598      	cmp	r8, r3
 e00757e:	d962      	bls.n	e007646 <__udivmoddi4+0x19a>
 e007580:	18eb      	adds	r3, r5, r3
 e007582:	f10c 30ff 	add.w	r0, ip, #4294967295	; 0xffffffff
 e007586:	d204      	bcs.n	e007592 <__udivmoddi4+0xe6>
 e007588:	4598      	cmp	r8, r3
 e00758a:	d902      	bls.n	e007592 <__udivmoddi4+0xe6>
 e00758c:	f1ac 0002 	sub.w	r0, ip, #2
 e007590:	442b      	add	r3, r5
 e007592:	eba3 0308 	sub.w	r3, r3, r8
 e007596:	b2a4      	uxth	r4, r4
 e007598:	fbb3 fcfe 	udiv	ip, r3, lr
 e00759c:	fb0e 331c 	mls	r3, lr, ip, r3
 e0075a0:	fb0c f202 	mul.w	r2, ip, r2
 e0075a4:	ea44 4403 	orr.w	r4, r4, r3, lsl #16
 e0075a8:	42a2      	cmp	r2, r4
 e0075aa:	d94e      	bls.n	e00764a <__udivmoddi4+0x19e>
 e0075ac:	192c      	adds	r4, r5, r4
 e0075ae:	f10c 33ff 	add.w	r3, ip, #4294967295	; 0xffffffff
 e0075b2:	d204      	bcs.n	e0075be <__udivmoddi4+0x112>
 e0075b4:	42a2      	cmp	r2, r4
 e0075b6:	d902      	bls.n	e0075be <__udivmoddi4+0x112>
 e0075b8:	f1ac 0302 	sub.w	r3, ip, #2
 e0075bc:	442c      	add	r4, r5
 e0075be:	1aa4      	subs	r4, r4, r2
 e0075c0:	e7bd      	b.n	e00753e <__udivmoddi4+0x92>
 e0075c2:	f1c7 0c20 	rsb	ip, r7, #32
 e0075c6:	40bd      	lsls	r5, r7
 e0075c8:	fa08 f307 	lsl.w	r3, r8, r7
 e0075cc:	40bc      	lsls	r4, r7
 e0075ce:	fa28 f90c 	lsr.w	r9, r8, ip
 e0075d2:	ea4f 4e15 	mov.w	lr, r5, lsr #16
 e0075d6:	fa20 fc0c 	lsr.w	ip, r0, ip
 e0075da:	ea4c 0203 	orr.w	r2, ip, r3
 e0075de:	b2ab      	uxth	r3, r5
 e0075e0:	fbb9 fcfe 	udiv	ip, r9, lr
 e0075e4:	0c11      	lsrs	r1, r2, #16
 e0075e6:	fb0e 901c 	mls	r0, lr, ip, r9
 e0075ea:	fb0c f803 	mul.w	r8, ip, r3
 e0075ee:	ea41 4000 	orr.w	r0, r1, r0, lsl #16
 e0075f2:	4580      	cmp	r8, r0
 e0075f4:	d923      	bls.n	e00763e <__udivmoddi4+0x192>
 e0075f6:	1828      	adds	r0, r5, r0
 e0075f8:	f10c 31ff 	add.w	r1, ip, #4294967295	; 0xffffffff
 e0075fc:	d204      	bcs.n	e007608 <__udivmoddi4+0x15c>
 e0075fe:	4580      	cmp	r8, r0
 e007600:	d902      	bls.n	e007608 <__udivmoddi4+0x15c>
 e007602:	f1ac 0102 	sub.w	r1, ip, #2
 e007606:	4428      	add	r0, r5
 e007608:	eba0 0008 	sub.w	r0, r0, r8
 e00760c:	b292      	uxth	r2, r2
 e00760e:	fbb0 fcfe 	udiv	ip, r0, lr
 e007612:	fb0e 001c 	mls	r0, lr, ip, r0
 e007616:	fb0c f803 	mul.w	r8, ip, r3
 e00761a:	ea42 4300 	orr.w	r3, r2, r0, lsl #16
 e00761e:	4598      	cmp	r8, r3
 e007620:	d90f      	bls.n	e007642 <__udivmoddi4+0x196>
 e007622:	18eb      	adds	r3, r5, r3
 e007624:	f10c 32ff 	add.w	r2, ip, #4294967295	; 0xffffffff
 e007628:	d204      	bcs.n	e007634 <__udivmoddi4+0x188>
 e00762a:	4598      	cmp	r8, r3
 e00762c:	d902      	bls.n	e007634 <__udivmoddi4+0x188>
 e00762e:	f1ac 0202 	sub.w	r2, ip, #2
 e007632:	442b      	add	r3, r5
 e007634:	eba3 0308 	sub.w	r3, r3, r8
 e007638:	ea42 4101 	orr.w	r1, r2, r1, lsl #16
 e00763c:	e792      	b.n	e007564 <__udivmoddi4+0xb8>
 e00763e:	4661      	mov	r1, ip
 e007640:	e7e2      	b.n	e007608 <__udivmoddi4+0x15c>
 e007642:	4662      	mov	r2, ip
 e007644:	e7f6      	b.n	e007634 <__udivmoddi4+0x188>
 e007646:	4660      	mov	r0, ip
 e007648:	e7a3      	b.n	e007592 <__udivmoddi4+0xe6>
 e00764a:	4663      	mov	r3, ip
 e00764c:	e7b7      	b.n	e0075be <__udivmoddi4+0x112>
 e00764e:	4543      	cmp	r3, r8
 e007650:	d905      	bls.n	e00765e <__udivmoddi4+0x1b2>
 e007652:	b10e      	cbz	r6, e007658 <__udivmoddi4+0x1ac>
 e007654:	e9c6 0800 	strd	r0, r8, [r6]
 e007658:	2100      	movs	r1, #0
 e00765a:	4608      	mov	r0, r1
 e00765c:	e776      	b.n	e00754c <__udivmoddi4+0xa0>
 e00765e:	fab3 fc83 	clz	ip, r3
 e007662:	f1bc 0f00 	cmp.w	ip, #0
 e007666:	d110      	bne.n	e00768a <__udivmoddi4+0x1de>
 e007668:	4543      	cmp	r3, r8
 e00766a:	d301      	bcc.n	e007670 <__udivmoddi4+0x1c4>
 e00766c:	4282      	cmp	r2, r0
 e00766e:	d80a      	bhi.n	e007686 <__udivmoddi4+0x1da>
 e007670:	1a84      	subs	r4, r0, r2
 e007672:	eb68 0303 	sbc.w	r3, r8, r3
 e007676:	2001      	movs	r0, #1
 e007678:	469e      	mov	lr, r3
 e00767a:	2e00      	cmp	r6, #0
 e00767c:	d068      	beq.n	e007750 <__udivmoddi4+0x2a4>
 e00767e:	e9c6 4e00 	strd	r4, lr, [r6]
 e007682:	2100      	movs	r1, #0
 e007684:	e762      	b.n	e00754c <__udivmoddi4+0xa0>
 e007686:	4660      	mov	r0, ip
 e007688:	e7f7      	b.n	e00767a <__udivmoddi4+0x1ce>
 e00768a:	f1cc 0e20 	rsb	lr, ip, #32
 e00768e:	fa03 f30c 	lsl.w	r3, r3, ip
 e007692:	fa02 f50c 	lsl.w	r5, r2, ip
 e007696:	fa00 f70c 	lsl.w	r7, r0, ip
 e00769a:	fa22 f40e 	lsr.w	r4, r2, lr
 e00769e:	fa28 fb0e 	lsr.w	fp, r8, lr
 e0076a2:	fa20 f10e 	lsr.w	r1, r0, lr
 e0076a6:	431c      	orrs	r4, r3
 e0076a8:	fa08 f30c 	lsl.w	r3, r8, ip
 e0076ac:	ea4f 4914 	mov.w	r9, r4, lsr #16
 e0076b0:	4319      	orrs	r1, r3
 e0076b2:	b2a3      	uxth	r3, r4
 e0076b4:	0c0a      	lsrs	r2, r1, #16
 e0076b6:	fbbb f8f9 	udiv	r8, fp, r9
 e0076ba:	fb09 bb18 	mls	fp, r9, r8, fp
 e0076be:	fb08 fa03 	mul.w	sl, r8, r3
 e0076c2:	ea42 420b 	orr.w	r2, r2, fp, lsl #16
 e0076c6:	4592      	cmp	sl, r2
 e0076c8:	d93e      	bls.n	e007748 <__udivmoddi4+0x29c>
 e0076ca:	18a2      	adds	r2, r4, r2
 e0076cc:	f108 30ff 	add.w	r0, r8, #4294967295	; 0xffffffff
 e0076d0:	d204      	bcs.n	e0076dc <__udivmoddi4+0x230>
 e0076d2:	4592      	cmp	sl, r2
 e0076d4:	d902      	bls.n	e0076dc <__udivmoddi4+0x230>
 e0076d6:	f1a8 0002 	sub.w	r0, r8, #2
 e0076da:	4422      	add	r2, r4
 e0076dc:	eba2 020a 	sub.w	r2, r2, sl
 e0076e0:	b289      	uxth	r1, r1
 e0076e2:	fbb2 f8f9 	udiv	r8, r2, r9
 e0076e6:	fb09 2218 	mls	r2, r9, r8, r2
 e0076ea:	fb08 f303 	mul.w	r3, r8, r3
 e0076ee:	ea41 4202 	orr.w	r2, r1, r2, lsl #16
 e0076f2:	4293      	cmp	r3, r2
 e0076f4:	d92a      	bls.n	e00774c <__udivmoddi4+0x2a0>
 e0076f6:	18a2      	adds	r2, r4, r2
 e0076f8:	f108 31ff 	add.w	r1, r8, #4294967295	; 0xffffffff
 e0076fc:	d204      	bcs.n	e007708 <__udivmoddi4+0x25c>
 e0076fe:	4293      	cmp	r3, r2
 e007700:	d902      	bls.n	e007708 <__udivmoddi4+0x25c>
 e007702:	f1a8 0102 	sub.w	r1, r8, #2
 e007706:	4422      	add	r2, r4
 e007708:	ea41 4000 	orr.w	r0, r1, r0, lsl #16
 e00770c:	1ad2      	subs	r2, r2, r3
 e00770e:	fba0 9805 	umull	r9, r8, r0, r5
 e007712:	4542      	cmp	r2, r8
 e007714:	464b      	mov	r3, r9
 e007716:	4641      	mov	r1, r8
 e007718:	d302      	bcc.n	e007720 <__udivmoddi4+0x274>
 e00771a:	d106      	bne.n	e00772a <__udivmoddi4+0x27e>
 e00771c:	454f      	cmp	r7, r9
 e00771e:	d204      	bcs.n	e00772a <__udivmoddi4+0x27e>
 e007720:	3801      	subs	r0, #1
 e007722:	ebb9 0305 	subs.w	r3, r9, r5
 e007726:	eb68 0104 	sbc.w	r1, r8, r4
 e00772a:	b18e      	cbz	r6, e007750 <__udivmoddi4+0x2a4>
 e00772c:	1afc      	subs	r4, r7, r3
 e00772e:	eb62 0701 	sbc.w	r7, r2, r1
 e007732:	fa07 fe0e 	lsl.w	lr, r7, lr
 e007736:	fa24 f30c 	lsr.w	r3, r4, ip
 e00773a:	fa27 f70c 	lsr.w	r7, r7, ip
 e00773e:	ea4e 0303 	orr.w	r3, lr, r3
 e007742:	e9c6 3700 	strd	r3, r7, [r6]
 e007746:	e79c      	b.n	e007682 <__udivmoddi4+0x1d6>
 e007748:	4640      	mov	r0, r8
 e00774a:	e7c7      	b.n	e0076dc <__udivmoddi4+0x230>
 e00774c:	4641      	mov	r1, r8
 e00774e:	e7db      	b.n	e007708 <__udivmoddi4+0x25c>
 e007750:	4631      	mov	r1, r6
 e007752:	e6fb      	b.n	e00754c <__udivmoddi4+0xa0>

0e007754 <__aeabi_idiv0>:
 e007754:	4770      	bx	lr
 e007756:	bf00      	nop

0e007758 <__libc_init_array>:
 e007758:	b570      	push	{r4, r5, r6, lr}
 e00775a:	4d0d      	ldr	r5, [pc, #52]	; (e007790 <__libc_init_array+0x38>)
 e00775c:	2600      	movs	r6, #0
 e00775e:	4c0d      	ldr	r4, [pc, #52]	; (e007794 <__libc_init_array+0x3c>)
 e007760:	1b64      	subs	r4, r4, r5
 e007762:	10a4      	asrs	r4, r4, #2
 e007764:	42a6      	cmp	r6, r4
 e007766:	d109      	bne.n	e00777c <__libc_init_array+0x24>
 e007768:	4d0b      	ldr	r5, [pc, #44]	; (e007798 <__libc_init_array+0x40>)
 e00776a:	2600      	movs	r6, #0
 e00776c:	4c0b      	ldr	r4, [pc, #44]	; (e00779c <__libc_init_array+0x44>)
 e00776e:	f7f8 fe45 	bl	e0003fc <_init>
 e007772:	1b64      	subs	r4, r4, r5
 e007774:	10a4      	asrs	r4, r4, #2
 e007776:	42a6      	cmp	r6, r4
 e007778:	d105      	bne.n	e007786 <__libc_init_array+0x2e>
 e00777a:	bd70      	pop	{r4, r5, r6, pc}
 e00777c:	f855 3b04 	ldr.w	r3, [r5], #4
 e007780:	3601      	adds	r6, #1
 e007782:	4798      	blx	r3
 e007784:	e7ee      	b.n	e007764 <__libc_init_array+0xc>
 e007786:	f855 3b04 	ldr.w	r3, [r5], #4
 e00778a:	3601      	adds	r6, #1
 e00778c:	4798      	blx	r3
 e00778e:	e7f2      	b.n	e007776 <__libc_init_array+0x1e>
 e007790:	0e009a90 	.word	0x0e009a90
 e007794:	0e009a90 	.word	0x0e009a90
 e007798:	0e009a90 	.word	0x0e009a90
 e00779c:	0e009a98 	.word	0x0e009a98

0e0077a0 <cleanup_glue>:
 e0077a0:	b538      	push	{r3, r4, r5, lr}
 e0077a2:	460c      	mov	r4, r1
 e0077a4:	6809      	ldr	r1, [r1, #0]
 e0077a6:	4605      	mov	r5, r0
 e0077a8:	b109      	cbz	r1, e0077ae <cleanup_glue+0xe>
 e0077aa:	f7ff fff9 	bl	e0077a0 <cleanup_glue>
 e0077ae:	4621      	mov	r1, r4
 e0077b0:	4628      	mov	r0, r5
 e0077b2:	e8bd 4038 	ldmia.w	sp!, {r3, r4, r5, lr}
 e0077b6:	f7fa bc79 	b.w	e0020ac <__wrap__free_r>

0e0077ba <_reclaim_reent>:
 e0077ba:	4b32      	ldr	r3, [pc, #200]	; (e007884 <_reclaim_reent+0xca>)
 e0077bc:	681b      	ldr	r3, [r3, #0]
 e0077be:	4283      	cmp	r3, r0
 e0077c0:	b570      	push	{r4, r5, r6, lr}
 e0077c2:	4604      	mov	r4, r0
 e0077c4:	d05c      	beq.n	e007880 <_reclaim_reent+0xc6>
 e0077c6:	6a43      	ldr	r3, [r0, #36]	; 0x24
 e0077c8:	b143      	cbz	r3, e0077dc <_reclaim_reent+0x22>
 e0077ca:	68db      	ldr	r3, [r3, #12]
 e0077cc:	2b00      	cmp	r3, #0
 e0077ce:	d155      	bne.n	e00787c <_reclaim_reent+0xc2>
 e0077d0:	6a63      	ldr	r3, [r4, #36]	; 0x24
 e0077d2:	6819      	ldr	r1, [r3, #0]
 e0077d4:	b111      	cbz	r1, e0077dc <_reclaim_reent+0x22>
 e0077d6:	4620      	mov	r0, r4
 e0077d8:	f7fa fc68 	bl	e0020ac <__wrap__free_r>
 e0077dc:	6961      	ldr	r1, [r4, #20]
 e0077de:	b111      	cbz	r1, e0077e6 <_reclaim_reent+0x2c>
 e0077e0:	4620      	mov	r0, r4
 e0077e2:	f7fa fc63 	bl	e0020ac <__wrap__free_r>
 e0077e6:	6a61      	ldr	r1, [r4, #36]	; 0x24
 e0077e8:	b111      	cbz	r1, e0077f0 <_reclaim_reent+0x36>
 e0077ea:	4620      	mov	r0, r4
 e0077ec:	f7fa fc5e 	bl	e0020ac <__wrap__free_r>
 e0077f0:	6ba1      	ldr	r1, [r4, #56]	; 0x38
 e0077f2:	b111      	cbz	r1, e0077fa <_reclaim_reent+0x40>
 e0077f4:	4620      	mov	r0, r4
 e0077f6:	f7fa fc59 	bl	e0020ac <__wrap__free_r>
 e0077fa:	6be1      	ldr	r1, [r4, #60]	; 0x3c
 e0077fc:	b111      	cbz	r1, e007804 <_reclaim_reent+0x4a>
 e0077fe:	4620      	mov	r0, r4
 e007800:	f7fa fc54 	bl	e0020ac <__wrap__free_r>
 e007804:	6c21      	ldr	r1, [r4, #64]	; 0x40
 e007806:	b111      	cbz	r1, e00780e <_reclaim_reent+0x54>
 e007808:	4620      	mov	r0, r4
 e00780a:	f7fa fc4f 	bl	e0020ac <__wrap__free_r>
 e00780e:	f8d4 10ec 	ldr.w	r1, [r4, #236]	; 0xec
 e007812:	b111      	cbz	r1, e00781a <_reclaim_reent+0x60>
 e007814:	4620      	mov	r0, r4
 e007816:	f7fa fc49 	bl	e0020ac <__wrap__free_r>
 e00781a:	f8d4 10e8 	ldr.w	r1, [r4, #232]	; 0xe8
 e00781e:	b111      	cbz	r1, e007826 <_reclaim_reent+0x6c>
 e007820:	4620      	mov	r0, r4
 e007822:	f7fa fc43 	bl	e0020ac <__wrap__free_r>
 e007826:	6ca3      	ldr	r3, [r4, #72]	; 0x48
 e007828:	b12b      	cbz	r3, e007836 <_reclaim_reent+0x7c>
 e00782a:	f8d3 1088 	ldr.w	r1, [r3, #136]	; 0x88
 e00782e:	b111      	cbz	r1, e007836 <_reclaim_reent+0x7c>
 e007830:	4620      	mov	r0, r4
 e007832:	f7fa fc3b 	bl	e0020ac <__wrap__free_r>
 e007836:	6b61      	ldr	r1, [r4, #52]	; 0x34
 e007838:	b111      	cbz	r1, e007840 <_reclaim_reent+0x86>
 e00783a:	4620      	mov	r0, r4
 e00783c:	f7fa fc36 	bl	e0020ac <__wrap__free_r>
 e007840:	69a3      	ldr	r3, [r4, #24]
 e007842:	b1eb      	cbz	r3, e007880 <_reclaim_reent+0xc6>
 e007844:	6aa3      	ldr	r3, [r4, #40]	; 0x28
 e007846:	4620      	mov	r0, r4
 e007848:	4798      	blx	r3
 e00784a:	f8d4 10d8 	ldr.w	r1, [r4, #216]	; 0xd8
 e00784e:	b1b9      	cbz	r1, e007880 <_reclaim_reent+0xc6>
 e007850:	4620      	mov	r0, r4
 e007852:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
 e007856:	f7ff bfa3 	b.w	e0077a0 <cleanup_glue>
 e00785a:	5949      	ldr	r1, [r1, r5]
 e00785c:	b941      	cbnz	r1, e007870 <_reclaim_reent+0xb6>
 e00785e:	3504      	adds	r5, #4
 e007860:	6a63      	ldr	r3, [r4, #36]	; 0x24
 e007862:	2d80      	cmp	r5, #128	; 0x80
 e007864:	68d9      	ldr	r1, [r3, #12]
 e007866:	d1f8      	bne.n	e00785a <_reclaim_reent+0xa0>
 e007868:	4620      	mov	r0, r4
 e00786a:	f7fa fc1f 	bl	e0020ac <__wrap__free_r>
 e00786e:	e7af      	b.n	e0077d0 <_reclaim_reent+0x16>
 e007870:	680e      	ldr	r6, [r1, #0]
 e007872:	4620      	mov	r0, r4
 e007874:	f7fa fc1a 	bl	e0020ac <__wrap__free_r>
 e007878:	4631      	mov	r1, r6
 e00787a:	e7ef      	b.n	e00785c <_reclaim_reent+0xa2>
 e00787c:	2500      	movs	r5, #0
 e00787e:	e7ef      	b.n	e007860 <_reclaim_reent+0xa6>
 e007880:	bd70      	pop	{r4, r5, r6, pc}
 e007882:	bf00      	nop
 e007884:	2000b068 	.word	0x2000b068

0e007888 <strcpy>:
 e007888:	ea80 0201 	eor.w	r2, r0, r1
 e00788c:	4684      	mov	ip, r0
 e00788e:	f012 0f03 	tst.w	r2, #3
 e007892:	d14f      	bne.n	e007934 <strcpy+0xac>
 e007894:	f011 0f03 	tst.w	r1, #3
 e007898:	d132      	bne.n	e007900 <strcpy+0x78>
 e00789a:	f84d 4d04 	str.w	r4, [sp, #-4]!
 e00789e:	f011 0f04 	tst.w	r1, #4
 e0078a2:	f851 3b04 	ldr.w	r3, [r1], #4
 e0078a6:	d00b      	beq.n	e0078c0 <strcpy+0x38>
 e0078a8:	f1a3 3201 	sub.w	r2, r3, #16843009	; 0x1010101
 e0078ac:	439a      	bics	r2, r3
 e0078ae:	f012 3f80 	tst.w	r2, #2155905152	; 0x80808080
 e0078b2:	bf04      	itt	eq
 e0078b4:	f84c 3b04 	streq.w	r3, [ip], #4
 e0078b8:	f851 3b04 	ldreq.w	r3, [r1], #4
 e0078bc:	d116      	bne.n	e0078ec <strcpy+0x64>
 e0078be:	bf00      	nop
 e0078c0:	f851 4b04 	ldr.w	r4, [r1], #4
 e0078c4:	f1a3 3201 	sub.w	r2, r3, #16843009	; 0x1010101
 e0078c8:	439a      	bics	r2, r3
 e0078ca:	f012 3f80 	tst.w	r2, #2155905152	; 0x80808080
 e0078ce:	f1a4 3201 	sub.w	r2, r4, #16843009	; 0x1010101
 e0078d2:	d10b      	bne.n	e0078ec <strcpy+0x64>
 e0078d4:	f84c 3b04 	str.w	r3, [ip], #4
 e0078d8:	43a2      	bics	r2, r4
 e0078da:	f012 3f80 	tst.w	r2, #2155905152	; 0x80808080
 e0078de:	bf04      	itt	eq
 e0078e0:	f851 3b04 	ldreq.w	r3, [r1], #4
 e0078e4:	f84c 4b04 	streq.w	r4, [ip], #4
 e0078e8:	d0ea      	beq.n	e0078c0 <strcpy+0x38>
 e0078ea:	4623      	mov	r3, r4
 e0078ec:	f80c 3b01 	strb.w	r3, [ip], #1
 e0078f0:	f013 0fff 	tst.w	r3, #255	; 0xff
 e0078f4:	ea4f 2333 	mov.w	r3, r3, ror #8
 e0078f8:	d1f8      	bne.n	e0078ec <strcpy+0x64>
 e0078fa:	f85d 4b04 	ldr.w	r4, [sp], #4
 e0078fe:	4770      	bx	lr
 e007900:	f011 0f01 	tst.w	r1, #1
 e007904:	d006      	beq.n	e007914 <strcpy+0x8c>
 e007906:	f811 2b01 	ldrb.w	r2, [r1], #1
 e00790a:	f80c 2b01 	strb.w	r2, [ip], #1
 e00790e:	2a00      	cmp	r2, #0
 e007910:	bf08      	it	eq
 e007912:	4770      	bxeq	lr
 e007914:	f011 0f02 	tst.w	r1, #2
 e007918:	d0bf      	beq.n	e00789a <strcpy+0x12>
 e00791a:	f831 2b02 	ldrh.w	r2, [r1], #2
 e00791e:	f012 0fff 	tst.w	r2, #255	; 0xff
 e007922:	bf16      	itet	ne
 e007924:	f82c 2b02 	strhne.w	r2, [ip], #2
 e007928:	f88c 2000 	strbeq.w	r2, [ip]
 e00792c:	f412 4f7f 	tstne.w	r2, #65280	; 0xff00
 e007930:	d1b3      	bne.n	e00789a <strcpy+0x12>
 e007932:	4770      	bx	lr
 e007934:	f811 2b01 	ldrb.w	r2, [r1], #1
 e007938:	f80c 2b01 	strb.w	r2, [ip], #1
 e00793c:	2a00      	cmp	r2, #0
 e00793e:	d1f9      	bne.n	e007934 <strcpy+0xac>
 e007940:	4770      	bx	lr
 e007942:	bf00      	nop

0e007944 <strncpy>:
 e007944:	ea40 0301 	orr.w	r3, r0, r1
 e007948:	f013 0f03 	tst.w	r3, #3
 e00794c:	4603      	mov	r3, r0
 e00794e:	b570      	push	{r4, r5, r6, lr}
 e007950:	460c      	mov	r4, r1
 e007952:	d101      	bne.n	e007958 <strncpy+0x14>
 e007954:	2a03      	cmp	r2, #3
 e007956:	d80d      	bhi.n	e007974 <strncpy+0x30>
 e007958:	3901      	subs	r1, #1
 e00795a:	b132      	cbz	r2, e00796a <strncpy+0x26>
 e00795c:	f811 4f01 	ldrb.w	r4, [r1, #1]!
 e007960:	3a01      	subs	r2, #1
 e007962:	f803 4b01 	strb.w	r4, [r3], #1
 e007966:	2c00      	cmp	r4, #0
 e007968:	d1f7      	bne.n	e00795a <strncpy+0x16>
 e00796a:	441a      	add	r2, r3
 e00796c:	2100      	movs	r1, #0
 e00796e:	429a      	cmp	r2, r3
 e007970:	d10f      	bne.n	e007992 <strncpy+0x4e>
 e007972:	bd70      	pop	{r4, r5, r6, pc}
 e007974:	4621      	mov	r1, r4
 e007976:	f854 6b04 	ldr.w	r6, [r4], #4
 e00797a:	f1a6 3501 	sub.w	r5, r6, #16843009	; 0x1010101
 e00797e:	ea25 0506 	bic.w	r5, r5, r6
 e007982:	f015 3f80 	tst.w	r5, #2155905152	; 0x80808080
 e007986:	d1e7      	bne.n	e007958 <strncpy+0x14>
 e007988:	3a04      	subs	r2, #4
 e00798a:	4621      	mov	r1, r4
 e00798c:	f843 6b04 	str.w	r6, [r3], #4
 e007990:	e7e0      	b.n	e007954 <strncpy+0x10>
 e007992:	f803 1b01 	strb.w	r1, [r3], #1
 e007996:	e7ea      	b.n	e00796e <strncpy+0x2a>

0e007998 <strsep>:
 e007998:	4602      	mov	r2, r0
 e00799a:	2300      	movs	r3, #0
 e00799c:	6800      	ldr	r0, [r0, #0]
 e00799e:	f000 b800 	b.w	e0079a2 <__strtok_r>

0e0079a2 <__strtok_r>:
 e0079a2:	b5f0      	push	{r4, r5, r6, r7, lr}
 e0079a4:	b908      	cbnz	r0, e0079aa <__strtok_r+0x8>
 e0079a6:	6810      	ldr	r0, [r2, #0]
 e0079a8:	b188      	cbz	r0, e0079ce <__strtok_r+0x2c>
 e0079aa:	4604      	mov	r4, r0
 e0079ac:	4620      	mov	r0, r4
 e0079ae:	460f      	mov	r7, r1
 e0079b0:	f814 5b01 	ldrb.w	r5, [r4], #1
 e0079b4:	f817 6b01 	ldrb.w	r6, [r7], #1
 e0079b8:	b91e      	cbnz	r6, e0079c2 <__strtok_r+0x20>
 e0079ba:	b965      	cbnz	r5, e0079d6 <__strtok_r+0x34>
 e0079bc:	4628      	mov	r0, r5
 e0079be:	6015      	str	r5, [r2, #0]
 e0079c0:	e005      	b.n	e0079ce <__strtok_r+0x2c>
 e0079c2:	42b5      	cmp	r5, r6
 e0079c4:	d1f6      	bne.n	e0079b4 <__strtok_r+0x12>
 e0079c6:	2b00      	cmp	r3, #0
 e0079c8:	d1f0      	bne.n	e0079ac <__strtok_r+0xa>
 e0079ca:	6014      	str	r4, [r2, #0]
 e0079cc:	7003      	strb	r3, [r0, #0]
 e0079ce:	bdf0      	pop	{r4, r5, r6, r7, pc}
 e0079d0:	461c      	mov	r4, r3
 e0079d2:	e00c      	b.n	e0079ee <__strtok_r+0x4c>
 e0079d4:	b915      	cbnz	r5, e0079dc <__strtok_r+0x3a>
 e0079d6:	f814 3b01 	ldrb.w	r3, [r4], #1
 e0079da:	460e      	mov	r6, r1
 e0079dc:	f816 5b01 	ldrb.w	r5, [r6], #1
 e0079e0:	42ab      	cmp	r3, r5
 e0079e2:	d1f7      	bne.n	e0079d4 <__strtok_r+0x32>
 e0079e4:	2b00      	cmp	r3, #0
 e0079e6:	d0f3      	beq.n	e0079d0 <__strtok_r+0x2e>
 e0079e8:	2300      	movs	r3, #0
 e0079ea:	f804 3c01 	strb.w	r3, [r4, #-1]
 e0079ee:	6014      	str	r4, [r2, #0]
 e0079f0:	e7ed      	b.n	e0079ce <__strtok_r+0x2c>

0e0079f2 <strtok_r>:
 e0079f2:	2301      	movs	r3, #1
 e0079f4:	f7ff bfd5 	b.w	e0079a2 <__strtok_r>

0e0079f8 <____wrap_memcpy_veneer>:
 e0079f8:	f85f f000 	ldr.w	pc, [pc]	; e0079fc <____wrap_memcpy_veneer+0x4>
 e0079fc:	00012115 	.word	0x00012115

0e007a00 <__INT_HardFault_C_veneer>:
 e007a00:	f85f f000 	ldr.w	pc, [pc]	; e007a04 <__INT_HardFault_C_veneer+0x4>
 e007a04:	0000cded 	.word	0x0000cded

0e007a08 <____wrap_memset_veneer>:
 e007a08:	f85f f000 	ldr.w	pc, [pc]	; e007a0c <____wrap_memset_veneer+0x4>
 e007a0c:	0001210d 	.word	0x0001210d

0e007a10 <__IPC_SEMFree_veneer>:
 e007a10:	f85f f000 	ldr.w	pc, [pc]	; e007a14 <__IPC_SEMFree_veneer+0x4>
 e007a14:	0000c135 	.word	0x0000c135

0e007a18 <____wrap_strcmp_veneer>:
 e007a18:	f85f f000 	ldr.w	pc, [pc]	; e007a1c <____wrap_strcmp_veneer+0x4>
 e007a1c:	0001213d 	.word	0x0001213d
 e007a20:	6f632f2e 	.word	0x6f632f2e
 e007a24:	6e6f706d 	.word	0x6e6f706d
 e007a28:	2f746e65 	.word	0x2f746e65
 e007a2c:	2f636f73 	.word	0x2f636f73
 e007a30:	62656d61 	.word	0x62656d61
 e007a34:	6c706461 	.word	0x6c706461
 e007a38:	662f7375 	.word	0x662f7375
 e007a3c:	62696c77 	.word	0x62696c77
 e007a40:	6d61722f 	.word	0x6d61722f
 e007a44:	6d6f635f 	.word	0x6d6f635f
 e007a48:	2f6e6f6d 	.word	0x2f6e6f6d
 e007a4c:	62656d61 	.word	0x62656d61
 e007a50:	64615f61 	.word	0x64615f61
 e007a54:	00632e63 	.word	0x00632e63
 e007a58:	0c080402 	.word	0x0c080402
 e007a5c:	00402010 	.word	0x00402010
 e007a60:	61707942 	.word	0x61707942
 e007a64:	4f207373 	.word	0x4f207373
 e007a68:	61205054 	.word	0x61205054
 e007a6c:	6c6f7475 	.word	0x6c6f7475
 e007a70:	0d64616f 	.word	0x0d64616f
 e007a74:	0000000a 	.word	0x0000000a
 e007a78:	00505041 	.word	0x00505041
 e007a7c:	54206e49 	.word	0x54206e49
 e007a80:	20747365 	.word	0x20747365
 e007a84:	65646f6d 	.word	0x65646f6d
 e007a88:	7830203a 	.word	0x7830203a
 e007a8c:	0d786c25 	.word	0x0d786c25
 e007a90:	0000000a 	.word	0x0000000a
 e007a94:	50204f49 	.word	0x50204f49
 e007a98:	7265776f 	.word	0x7265776f
 e007a9c:	382e3120 	.word	0x382e3120
 e007aa0:	00000a56 	.word	0x00000a56
 e007aa4:	20344d4b 	.word	0x20344d4b
 e007aa8:	20505041 	.word	0x20505041
 e007aac:	52415453 	.word	0x52415453
 e007ab0:	000a2054 	.word	0x000a2054
 e007ab4:	524f5456 	.word	0x524f5456
 e007ab8:	6c25203a 	.word	0x6c25203a
 e007abc:	56202c78 	.word	0x56202c78
 e007ac0:	5f524f54 	.word	0x5f524f54
 e007ac4:	253a534e 	.word	0x253a534e
 e007ac8:	000a786c 	.word	0x000a786c
 e007acc:	32474d49 	.word	0x32474d49
 e007ad0:	43455320 	.word	0x43455320
 e007ad4:	20455255 	.word	0x20455255
 e007ad8:	54415453 	.word	0x54415453
 e007adc:	25203a45 	.word	0x25203a45
 e007ae0:	00000a64 	.word	0x00000a64
 e007ae4:	4c41435b 	.word	0x4c41435b
 e007ae8:	4b313331 	.word	0x4b313331
 e007aec:	64203a5d 	.word	0x64203a5d
 e007af0:	61746c65 	.word	0x61746c65
 e007af4:	756c253a 	.word	0x756c253a
 e007af8:	72617420 	.word	0x72617420
 e007afc:	3a746567 	.word	0x3a746567
 e007b00:	20756c25 	.word	0x20756c25
 e007b04:	3a4d5050 	.word	0x3a4d5050
 e007b08:	756c2520 	.word	0x756c2520
 e007b0c:	4d505020 	.word	0x4d505020
 e007b10:	6d694c5f 	.word	0x6d694c5f
 e007b14:	253a7469 	.word	0x253a7469
 e007b18:	0a20756c 	.word	0x0a20756c
 e007b1c:	00000000 	.word	0x00000000
 e007b20:	004b4c43 	.word	0x004b4c43
 e007b24:	4c41435b 	.word	0x4c41435b
 e007b28:	4b313331 	.word	0x4b313331
 e007b2c:	21203a5d 	.word	0x21203a5d
 e007b30:	63202121 	.word	0x63202121
 e007b34:	66206c61 	.word	0x66206c61
 e007b38:	206c6961 	.word	0x206c6961
 e007b3c:	20212121 	.word	0x20212121
 e007b40:	3a4d5050 	.word	0x3a4d5050
 e007b44:	756c2520 	.word	0x756c2520
 e007b48:	4d505020 	.word	0x4d505020
 e007b4c:	6d694c5f 	.word	0x6d694c5f
 e007b50:	253a7469 	.word	0x253a7469
 e007b54:	0a20756c 	.word	0x0a20756c
 e007b58:	00000000 	.word	0x00000000
 e007b5c:	6f632f2e 	.word	0x6f632f2e
 e007b60:	6e6f706d 	.word	0x6e6f706d
 e007b64:	2f746e65 	.word	0x2f746e65
 e007b68:	2f636f73 	.word	0x2f636f73
 e007b6c:	62656d61 	.word	0x62656d61
 e007b70:	6c706461 	.word	0x6c706461
 e007b74:	662f7375 	.word	0x662f7375
 e007b78:	62696c77 	.word	0x62696c77
 e007b7c:	6d61722f 	.word	0x6d61722f
 e007b80:	6d6f635f 	.word	0x6d6f635f
 e007b84:	2f6e6f6d 	.word	0x2f6e6f6d
 e007b88:	62656d61 	.word	0x62656d61
 e007b8c:	6c635f61 	.word	0x6c635f61
 e007b90:	00632e6b 	.word	0x00632e6b
 e007b94:	4c41435b 	.word	0x4c41435b
 e007b98:	3a5d4d34 	.word	0x3a5d4d34
 e007b9c:	6c656420 	.word	0x6c656420
 e007ba0:	253a6174 	.word	0x253a6174
 e007ba4:	7420756c 	.word	0x7420756c
 e007ba8:	65677261 	.word	0x65677261
 e007bac:	6c253a74 	.word	0x6c253a74
 e007bb0:	50502075 	.word	0x50502075
 e007bb4:	25203a4d 	.word	0x25203a4d
 e007bb8:	5020756c 	.word	0x5020756c
 e007bbc:	4c5f4d50 	.word	0x4c5f4d50
 e007bc0:	74696d69 	.word	0x74696d69
 e007bc4:	756c253a 	.word	0x756c253a
 e007bc8:	00000a20 	.word	0x00000a20
 e007bcc:	4c41435b 	.word	0x4c41435b
 e007bd0:	3a5d4d34 	.word	0x3a5d4d34
 e007bd4:	4d505020 	.word	0x4d505020
 e007bd8:	6c25203a 	.word	0x6c25203a
 e007bdc:	50502075 	.word	0x50502075
 e007be0:	694c5f4d 	.word	0x694c5f4d
 e007be4:	3a74696d 	.word	0x3a74696d
 e007be8:	20756c25 	.word	0x20756c25
 e007bec:	0000000a 	.word	0x0000000a
 e007bf0:	004f4931 	.word	0x004f4931
 e007bf4:	00004f32 	.word	0x00004f32
 e007bf8:	004f4932 	.word	0x004f4932
 e007bfc:	00004f34 	.word	0x00004f34
 e007c00:	004f4934 	.word	0x004f4934
 e007c04:	41544144 	.word	0x41544144
 e007c08:	414c4620 	.word	0x414c4620
 e007c0c:	525f4853 	.word	0x525f4853
 e007c10:	20646165 	.word	0x20646165
 e007c14:	6c257830 	.word	0x6c257830
 e007c18:	78303a78 	.word	0x78303a78
 e007c1c:	0a786c25 	.word	0x0a786c25
 e007c20:	00000000 	.word	0x00000000
 e007c24:	414c4644 	.word	0x414c4644
 e007c28:	4c434853 	.word	0x4c434853
 e007c2c:	0000004b 	.word	0x0000004b
 e007c30:	4c494146 	.word	0x4c494146
 e007c34:	00000000 	.word	0x00000000
 e007c38:	00004b4f 	.word	0x00004b4f
 e007c3c:	41544144 	.word	0x41544144
 e007c40:	414c4620 	.word	0x414c4620
 e007c44:	48204853 	.word	0x48204853
 e007c48:	53646e61 	.word	0x53646e61
 e007c4c:	656b6168 	.word	0x656b6168
 e007c50:	2578305b 	.word	0x2578305b
 e007c54:	73252078 	.word	0x73252078
 e007c58:	00000a5d 	.word	0x00000a5d
 e007c5c:	41544144 	.word	0x41544144
 e007c60:	616c4620 	.word	0x616c4620
 e007c64:	53206873 	.word	0x53206873
 e007c68:	63746977 	.word	0x63746977
 e007c6c:	65522068 	.word	0x65522068
 e007c70:	4d206461 	.word	0x4d206461
 e007c74:	2065646f 	.word	0x2065646f
 e007c78:	4c494146 	.word	0x4c494146
 e007c7c:	0000000a 	.word	0x0000000a
 e007c80:	41544144 	.word	0x41544144
 e007c84:	616c4620 	.word	0x616c4620
 e007c88:	52206873 	.word	0x52206873
 e007c8c:	20646165 	.word	0x20646165
 e007c90:	000a7325 	.word	0x000a7325
 e007c94:	73616c46 	.word	0x73616c46
 e007c98:	44492068 	.word	0x44492068
 e007c9c:	7825203a 	.word	0x7825203a
 e007ca0:	2d78252d 	.word	0x2d78252d
 e007ca4:	000a7825 	.word	0x000a7825
 e007ca8:	73696854 	.word	0x73696854
 e007cac:	616c6620 	.word	0x616c6620
 e007cb0:	74206873 	.word	0x74206873
 e007cb4:	20657079 	.word	0x20657079
 e007cb8:	6e207369 	.word	0x6e207369
 e007cbc:	7320746f 	.word	0x7320746f
 e007cc0:	6f707075 	.word	0x6f707075
 e007cc4:	64657472 	.word	0x64657472
 e007cc8:	00000a21 	.word	0x00000a21
 e007ccc:	6f632f2e 	.word	0x6f632f2e
 e007cd0:	6e6f706d 	.word	0x6e6f706d
 e007cd4:	2f746e65 	.word	0x2f746e65
 e007cd8:	2f636f73 	.word	0x2f636f73
 e007cdc:	62656d61 	.word	0x62656d61
 e007ce0:	6c706461 	.word	0x6c706461
 e007ce4:	662f7375 	.word	0x662f7375
 e007ce8:	62696c77 	.word	0x62696c77
 e007cec:	6d61722f 	.word	0x6d61722f
 e007cf0:	346d6b5f 	.word	0x346d6b5f
 e007cf4:	656d612f 	.word	0x656d612f
 e007cf8:	645f6162 	.word	0x645f6162
 e007cfc:	5f617461 	.word	0x5f617461
 e007d00:	73616c66 	.word	0x73616c66
 e007d04:	6b6c6368 	.word	0x6b6c6368
 e007d08:	0000632e 	.word	0x0000632e
 e007d0c:	0e007bf0 	.word	0x0e007bf0
 e007d10:	0e007bf4 	.word	0x0e007bf4
 e007d14:	0e007bf8 	.word	0x0e007bf8
 e007d18:	0e007bfc 	.word	0x0e007bfc
 e007d1c:	0e007c00 	.word	0x0e007c00
 e007d20:	6e6f4e0a 	.word	0x6e6f4e0a
 e007d24:	6365532d 	.word	0x6365532d
 e007d28:	20657275 	.word	0x20657275
 e007d2c:	6c726f77 	.word	0x6c726f77
 e007d30:	61632064 	.word	0x61632064
 e007d34:	746f6e6e 	.word	0x746f6e6e
 e007d38:	6d756420 	.word	0x6d756420
 e007d3c:	65722070 	.word	0x65722070
 e007d40:	74736967 	.word	0x74736967
 e007d44:	20737265 	.word	0x20737265
 e007d48:	756f6261 	.word	0x756f6261
 e007d4c:	78652074 	.word	0x78652074
 e007d50:	74706563 	.word	0x74706563
 e007d54:	206e6f69 	.word	0x206e6f69
 e007d58:	6d6f7266 	.word	0x6d6f7266
 e007d5c:	63657320 	.word	0x63657320
 e007d60:	20657275 	.word	0x20657275
 e007d64:	6c726f77 	.word	0x6c726f77
 e007d68:	000a2164 	.word	0x000a2164
 e007d6c:	4c554146 	.word	0x4c554146
 e007d70:	00000054 	.word	0x00000054
 e007d74:	52534653 	.word	0x52534653
 e007d78:	203d2020 	.word	0x203d2020
 e007d7c:	30257830 	.word	0x30257830
 e007d80:	0d786c38 	.word	0x0d786c38
 e007d84:	0000000a 	.word	0x0000000a
 e007d88:	52414653 	.word	0x52414653
 e007d8c:	203d2020 	.word	0x203d2020
 e007d90:	30257830 	.word	0x30257830
 e007d94:	0d786c38 	.word	0x0d786c38
 e007d98:	0000000a 	.word	0x0000000a
 e007d9c:	544e4f43 	.word	0x544e4f43
 e007da0:	5f4c4f52 	.word	0x5f4c4f52
 e007da4:	3d20534e 	.word	0x3d20534e
 e007da8:	25783020 	.word	0x25783020
 e007dac:	586c3830 	.word	0x586c3830
 e007db0:	00000a0d 	.word	0x00000a0d
 e007db4:	53434853 	.word	0x53434853
 e007db8:	203d2052 	.word	0x203d2052
 e007dbc:	30257830 	.word	0x30257830
 e007dc0:	0d786c38 	.word	0x0d786c38
 e007dc4:	0000000a 	.word	0x0000000a
 e007dc8:	43524941 	.word	0x43524941
 e007dcc:	203d2052 	.word	0x203d2052
 e007dd0:	30257830 	.word	0x30257830
 e007dd4:	0d786c38 	.word	0x0d786c38
 e007dd8:	0000000a 	.word	0x0000000a
 e007ddc:	544e4f43 	.word	0x544e4f43
 e007de0:	204c4f52 	.word	0x204c4f52
 e007de4:	7830203d 	.word	0x7830203d
 e007de8:	6c383025 	.word	0x6c383025
 e007dec:	000a0d78 	.word	0x000a0d78

0e007df0 <Data_Flash_ReadMode>:
 e007df0:	                                         ..

0e007df2 <Data_Flash_Speed>:
 e007df2:	                                         ..

0e007df4 <Flash_AVL>:
 e007df4:	000000ef 000000ff 00000000 000043fc     .............C..
 e007e04:	00000000 000000a1 000000ff 00000000     ................
 e007e14:	0000fffc 00000000 0000000b 000000ff     ................
 e007e24:	00000000 000043fc 00000000 0000000e     .....C..........
 e007e34:	000000ff 00000000 000043fc 00000000     .........C......
 e007e44:	000000c8 000000ff 00000001 000043fc     .............C..
 e007e54:	00000000 000028c2 0000ffff 00000005     .....(..........
 e007e64:	000200fc 00000000 000000c2 000000ff     ................
 e007e74:	00000002 000000fc 00000000 00000068     ............h...
 e007e84:	000000ff 00000002 000000fc 00000000     ................
 e007e94:	00000051 000000ff 00000002 000000fc     Q...............
 e007ea4:	00000000 0000001c 000000ff 00000003     ................
 e007eb4:	000000fc 00000000 00000020 000000ff     ........ .......
 e007ec4:	00000000 000043fc 00000000 00000085     .....C..........
 e007ed4:	000000ff 00000000 000043fc 00000000     .........C......
 e007ee4:	0000005e 000000ff 00000000 000043fc     ^............C..
	...
 e007efc:	000000ff 000000fe ffffffff 0e001451     ............Q...
 e007f0c:	000000ff ffffffff 000000ff ffffffff     ................
 e007f1c:	00000000                                ....

0e007f20 <Flash_Layout>:
 e007f20:	00000000 08000000 08013fff 00000002     .........?......
 e007f30:	08014000 081f3fff 00000001 08200000     .@...?........ .
 e007f40:	08213fff 00000003 08214000 083f3fff     .?!......@!..??.
 e007f50:	00000004 08700000 08702fff 00000005     ......p../p.....
 e007f60:	08703000 08722fff 00000007 ffffffff     .0p../r.........
 e007f70:	ffffffff 000000ff ffffffff ffffffff     ................
 e007f80:	53414c46 72575f48 5f657469 6b636f4c     FLASH_Write_Lock
 e007f90:	74656720 20776820 616d6573 69616620      get hw sema fai
 e007fa0:	00000a6c 53414c46 00000048 6f632f2e     l...FLASH..../co
 e007fb0:	6e6f706d 2f746e65 2f636f73 62656d61     mponent/soc/ameb
 e007fc0:	6c706461 662f7375 62696c77 6d61722f     adplus/fwlib/ram
 e007fd0:	6d6f635f 2f6e6f6d 62656d61 6c665f61     _common/ameba_fl
 e007fe0:	5f687361 2e6d6172 00000063 6e616843     ash_ram.c...Chan
 e007ff0:	206c656e 666e6f43 7463696c 726f6620     nel Conflict for
 e008000:	55504320 756c2520 61684320 6c656e6e      CPU %lu Channel
 e008010:	756c2520 49202120 726f6e67 66492065      %lu ! Ignore If
 e008020:	55504320 73614820 73655220 0a0d7465      CPU Has Reset..
 e008030:	00000000 00435049 43504920 71655220     ....IPC. IPC Req
 e008040:	74736575 6d695420 74756f65 00000a0d     uest Timeout....
 e008050:	43504920 74654720 6d655320 6f687061      IPC Get Semapho
 e008060:	54206572 6f656d69 0a0d7475 00000000     re Timeout......
 e008070:	6f632f2e 6e6f706d 2f746e65 2f636f73     ./component/soc/
 e008080:	62656d61 6c706461 662f7375 62696c77     amebadplus/fwlib
 e008090:	6d61722f 6d6f635f 2f6e6f6d 62656d61     /ram_common/ameb
 e0080a0:	70695f61 70615f63 00632e69 6f632f2e     a_ipc_api.c../co
 e0080b0:	6e6f706d 2f746e65 2f636f73 62656d61     mponent/soc/ameb
 e0080c0:	6c706461 662f7375 62696c77 6d61722f     adplus/fwlib/ram
 e0080d0:	6d6f635f 2f6e6f6d 62656d61 70695f61     _common/ameba_ip
 e0080e0:	61725f63 00632e6d 6f632f2e 6e6f706d     c_ram.c../compon
 e0080f0:	2f746e65 2f636f73 62656d61 6c706461     ent/soc/amebadpl
 e008100:	662f7375 62696c77 6d61722f 6d6f635f     us/fwlib/ram_com
 e008110:	2f6e6f6d 62656d61 706d5f61 61725f75     mon/ameba_mpu_ra
 e008120:	00632e6d 3a55504d 67657220 5f6e6f69     m.c.MPU: region_
 e008130:	65736162 6f687320 20646c75 33206562     base should be 3
 e008140:	79622032 20736574 67696c61 2064656e     2 bytes aligned 
 e008150:	6c383025 00000a58 0055504d 3a55504d     %08lX...MPU.MPU:
 e008160:	67657220 5f6e6f69 657a6973 6f687320      region_size sho
 e008170:	20646c75 33206562 79622032 20736574     uld be 32 bytes 
 e008180:	67696c61 2064656e 6c383025 00000a58     aligned %08lX...
 e008190:	6f632f2e 6e6f706d 2f746e65 2f636f73     ./component/soc/
 e0081a0:	62656d61 6c706461 6d2f7375 2f637369     amebadplus/misc/
 e0081b0:	62656d61 6d705f61 00632e75 5354415b     ameba_pmu.c.[ATS
 e0081c0:	203a5d43 5f54415f 54535953 435f4d45     C]: _AT_SYSTEM_C
 e0081d0:	5241454c 41544f5f 4749535f 5554414e     LEAR_OTA_SIGNATU
 e0081e0:	005f4552 5354415b 203a5d52 5f54415f     RE_.[ATSR]: _AT_
 e0081f0:	54535953 525f4d45 564f4345 4f5f5245     SYSTEM_RECOVER_O
 e008200:	535f4154 414e4749 45525554 0000005f     TA_SIGNATURE_...
 e008210:	43535441 00000000 52535441 00000000     ATSC....ATSR....
 e008220:	6f632f2e 6e6f706d 2f746e65 662f736f     ./component/os/f
 e008230:	72656572 2f736f74 65657266 736f7472     reertos/freertos
 e008240:	6165685f 635f3570 69666e6f 00632e67     _heap5_config.c.
 e008250:	3830255b 00205d78 255b0a0d 5d783830     [%08x] ...[%08x]
 e008260:	00000020 78383025 00000020 78323025      ...%08x ...%02x
 e008270:	00000020 2d73255b 205d6325 00000000      ...[%s-%c] ....
 e008280:	0000003d 72707769 00007669 706c6568     =...iwpriv..help
 e008290:	00000000 6e750a0d 776f6e6b 6f63206e     ......unknown co
 e0082a0:	6e616d6d 25272064 00002773 4d5b0d0a     mmand '%s'....[M
 e0082b0:	205d4d45 65746641 6f642072 646d6320     EM] After do cmd
 e0082c0:	7661202c 616c6961 20656c62 70616568     , available heap
 e0082d0:	0a642520 0000000d 230a0a0d 00000a0d      %d........#....
 e0082e0:	00000000 0073666c 656c6946 73795320     ....lfs.File Sys
 e0082f0:	206d6574 74696e49 63755320 73736563     tem Init Success
 e008300:	0000000a 4e49414d 00000000 656c6946     ....MAIN....File
 e008310:	73795320 206d6574 74696e49 69614620      System Init Fai
 e008320:	000a206c 20344d4b 4e49414d 00000a20     l ..KM4 MAIN ...
 e008330:	20344d4b 52415453 43532054 55444548     KM4 START SCHEDU
 e008340:	2052454c 0000000a 2d2d2d2d 2d2d2d2d     LER ....--------
 e008350:	2d2d2d2d 2d2d2d2d 4554202d 43205453     --------- TEST C
 e008360:	414d4d4f 4d20444e 2045444f 504c4548     OMMAND MODE HELP
 e008370:	20782520 2d2d2d2d 2d2d2d2d 2d2d2d2d      %x ------------
 e008380:	2d2d2d2d 000a2d2d 2d2d2d2d 2d2d2d2d     ------..--------
 e008390:	2d2d2d2d 2d2d2d2d 4554202d 43205453     --------- TEST C
 e0083a0:	414d4d4f 4d20444e 2045444f 20444e45     OMMAND MODE END 
 e0083b0:	20782520 2d2d2d2d 2d2d2d2d 2d2d2d2d      %x ------------
 e0083c0:	2d2d2d2d 000a2d2d 6f626552 6e69746f     ------..Rebootin
 e0083d0:	2e2e2067 000d0a2e 74726175 6e727562     g ......uartburn
 e0083e0:	00000000 3830255b 205d586c 6c383025     ....[%08lX] %08l
 e0083f0:	000a2058 6e6f7257 72612067 656d7567     X ..Wrong argume
 e008400:	6e20746e 65626d75 0a0d2172 00000000     nt number!......
 e008410:	50495352 616e6520 64656c62 6143202c     RSIP enabled, Ca
 e008420:	2074276e 64616552 616c4620 0a2e6873     n't Read Flash..
 e008430:	00000000 6e6f7257 6d632067 0a0d2164     ....Wrong cmd!..
 e008440:	00000000 0000003f 4c454809 3f282050     ....?....HELP (?
 e008450:	203a2029 2009090a 6e697250 65682074     ) : ... Print he
 e008460:	6d20706c 61737365 00000a67 00005744     lp messag...DW..
 e008470:	20574409 6464413c 73736572 654c202c     .DW <Address, Le
 e008480:	6874676e 09090a3e 6d754420 656d2070     ngth>... Dump me
 e008490:	79726f6d 726f7720 726f2064 61655220     mory word or Rea
 e0084a0:	77482064 726f7720 65722064 74736967     d Hw word regist
 e0084b0:	203b7265 2009090a 3c205744 72646461     er; ... DW <addr
 e0084c0:	0909203e 6d754420 6e6f2070 6f20796c     > .. Dump only o
 e0084d0:	7720656e 2064726f 74207461 73206568     ne word at the s
 e0084e0:	20656d61 656d6974 6e75202c 203a7469     ame time, unit: 
 e0084f0:	74794234 090a7365 57442009 64613c20     4Bytes... DW <ad
 e008500:	203e7264 6e656c3c 2009203e 706d7544     dr> <len> . Dump
 e008510:	65687420 65707320 69666963 6c206465      the specified l
 e008520:	74676e65 666f2068 65687420 726f7720     ength of the wor
 e008530:	75202c64 3a74696e 79423420 0a736574     d, unit: 4Bytes.
 e008540:	44200909 613c2057 3e726464 656c3c20     .. DW <addr> <le
 e008550:	62203e6e 75442020 7420706d 73206568     n> b  Dump the s
 e008560:	69636570 64656966 6e656c20 20687467     pecified length 
 e008570:	7420666f 62206568 2c657479 696e7520     of the byte, uni
 e008580:	31203a74 65747942 0000000a 00005745     t: 1Byte....EW..
 e008590:	20574509 6464413c 73736572 6548202c     .EW <Address, He
 e0085a0:	090a3e78 72572009 20657469 6f6d656d     x>... Write memo
 e0085b0:	64207972 64726f77 20726f20 74697257     ry dword or Writ
 e0085c0:	77482065 6f776420 72206472 73696765     e Hw dword regis
 e0085d0:	20726574 2009090a 206e6143 74697277     ter ... Can writ
 e0085e0:	6e6f2065 6f20796c 6420656e 64726f77     e only one dword
 e0085f0:	20746120 20656874 656d6173 6d697420      at the same tim
 e008600:	090a2065 78452009 5745203a 64644120     e ... Ex: EW Add
 e008610:	73736572 6c615620 0a206575 00000000     ress Value .....
 e008620:	4f424552 0000544f 42455209 20544f4f     REBOOT...REBOOT 
 e008630:	2009090a 6f626572 0a20746f 72200909     ... reboot ... r
 e008640:	6f6f6265 61752074 75627472 0a206e72     eboot uartburn .
 e008650:	00000000 255b0d0a 53205d73 4b434154     ......[%s] STACK
 e008660:	45564f20 4f4c4652 202d2057 6b736154      OVERFLOW - Task
 e008670:	656d614e 29732528 00000d0a 73546f4e     Name(%s)....NoTs
 e008680:	0000006b 00344d4b 6c6c614d 6620636f     k...KM4.Malloc f
 e008690:	656c6961 43202e64 3a65726f 5d73255b     ailed. Core:[%s]
 e0086a0:	6154202c 5b3a6b73 2c5d7325 72665b20     , Task:[%s], [fr
 e0086b0:	68206565 20706165 657a6973 6425203a     ee heap size: %d
 e0086c0:	000a0d5d                                ]...

0e0086c4 <__FUNCTION__.5>:
 e0086c4:	70704176 6163696c 6e6f6974 63617453     vApplicationStac
 e0086d4:	65764f6b 6f6c6672 6f6f4877 0000006b     kOverflowHook...
 e0086e4:	3f535441 00000000 55474f4c 5f545241     ATS?....LOGUART_
 e0086f4:	4b534154 00000000 61657243 4c206574     TASK....Create L
 e008704:	5520676f 20545241 6b736154 72724520     og UART Task Err
 e008714:	000a2121 4c454853 0000004c 5d73255b     !!..SHELL...[%s]
 e008724:	474d4920 203a4449 202c6425 72727563      IMGID: %d, curr
 e008734:	20746e65 2541544f 64412064 73657264     ent OTA%d Addres
 e008744:	30203a73 38302578 202c786c 67726174     s: 0x%08lx, targ
 e008754:	4f207465 64254154 64644120 73736572     et OTA%d Address
 e008764:	7830203a 6c383025 00000a78 00535953     : 0x%08lx...SYS.
 e008774:	5d73255b 474d4920 203a4449 202c6425     [%s] IMGID: %d, 
 e008784:	72727563 20746e65 6d726966 65726177     current firmware
 e008794:	20736920 2541544f 74202c64 65677261      is OTA%d, targe
 e0087a4:	69662074 61776d72 4f206572 64254154     t firmware OTA%d
 e0087b4:	20736920 61766e69 0a64696c 00000000      is invalid.....
 e0087c4:	5d73255b 63616220 2070756b 6c6c616d     [%s] backup mall
 e0087d4:	6620636f 646c6961 000a6465              oc failded..

0e0087e0 <__func__.0>:
 e0087e0:	5f737973 6f636572 5f726576 5f61746f     sys_recover_ota_
 e0087f0:	6e676973 72757461 00000065              signature...

0e0087fc <__func__.1>:
 e0087fc:	5f737973 61656c63 746f5f72 69735f61     sys_clear_ota_si
 e00880c:	74616e67 00657275 454c4449 00000000     gnature.IDLE....
 e00881c:	20726d54 00637653 25287325 202c2964     Tmr Svc.%s(%d), 
 e00882c:	69617641 6c62616c 65682065 25207061     Available heap %
 e00883c:	00000a64 4e414c57 00000000 74696e69     d...WLAN....init
 e00884c:	00000000 72207325 5f736f74 6b736174     ....%s rtos_task
 e00885c:	6572635f 28657461 74696e69 7268745f     _create(init_thr
 e00886c:	29646165 69616620 0a64656c 00000000     ead) failed.....

0e00887c <__FUNCTION__.0>:
 e00887c:	6e616c77 696e695f 6c616974 00657a69     wlan_initialize.

0e00888c <__FUNCTION__.1>:
 e00888c:	696e695f 68745f74 64616572 00000000     _init_thread....
 e00889c:	2d2d2d2d 2d2d2d2d 2d2d2d2d 6b736154     ------------Task
 e0088ac:	666e4920 2d2d2d6f 2d2d2d2d 2d2d2d2d      Info-----------
 e0088bc:	000a0d2d 6c756146 6e6f2074 73617420     -...Fault on tas
 e0088cc:	253c206b 000a3e73 6b736154 3a444920     k <%s>..Task ID:
 e0088dc:	0d642520 0000000a 6b736154 42435420      %d.....Task TCB
 e0088ec:	2578303a 786c3830 00000a0d 72727543     :0x%08lx....Curr
 e0088fc:	20746e65 74617453 25203a65 25282064     ent State: %d (%
 e00890c:	0a0d2973 00000000 65736142 69725020     s)......Base Pri
 e00891c:	7469726f 25203a79 000a0d64 72727543     ority: %d...Curr
 e00892c:	20746e65 6f697250 79746972 6425203a     ent Priority: %d
 e00893c:	00000a0d 206e7552 656d6954 756f4320     ....Run Time Cou
 e00894c:	7265746e 6c25203a 000a0d75 63617453     nter: %lu...Stac
 e00895c:	706f546b 7830203a 6c383025 53202c78     kTop: 0x%08lx, S
 e00896c:	6b636174 65736142 7830203a 6c383025     tackBase: 0x%08l
 e00897c:	53202c78 6b636174 3a646e45 25783020     x, StackEnd: 0x%
 e00898c:	786c3830 7453202c 536b6361 3d657a69     08lx, StackSize=
 e00899c:	28756c25 64726f77 000a0d29 63617453     %lu(word)...Stac
 e0089ac:	6948206b 57206867 72657461 6b72614d     k High WaterMark
 e0089bc:	6425203a 726f7728 0a0d2964 00000000     : %d(word)......
 e0089cc:	6e6e7552 00676e69 64616552 202f2079     Running.Ready / 
 e0089dc:	646e6550 00676e69 636f6c42 0064656b     Pending.Blocked.
 e0089ec:	70737553 65646e65 00000064 656c6544     Suspended...Dele
 e0089fc:	00646574 61766e49 0064696c              ted.Invalid.

0e008a08 <TaskStateString.0>:
 e008a08:	0e0089cc 0e0089d4 0e0089e4 0e0089ec     ................
 e008a18:	0e0089f8 0e008a00 5747344d 0a504120     ........M4GW AP.
 e008a28:	00000000 0a207325 00000000 00434d50     ....%s .....PMC.
 e008a38:	77205746 75656b61 4d4b2070 69762034     FW wakeup KM4 vi
 e008a48:	50492061 000a2043 454d4954 61772052     a IPC ..TIMER wa
 e008a58:	7075656b 344d4b20 61697620 43504920     keup KM4 via IPC
 e008a68:	00000a20 3a474244 656c5320 62207065      ...DBG: Sleep b
 e008a78:	6b636f6c 62206465 75616365 44206573     locked because D
 e008a88:	25207665 2020786c 79737562 0000000a     ev %lx  busy....
 e008a98:	5743344d 0a50412d 00000000              M4CW-AP.....

0e008aa4 <__func__.0>:
 e008aa4:	50434f53 4d4b5f53 4d4b5730 70695f34     SOCPS_KM0WKM4_ip
 e008ab4:	6e695f63 00000074 00304d4b 3d3d3d3d     c_int...KM0.====
 e008ac4:	3d3d3d3d 73253d3d 61745320 44206b63     ======%s Stack D
 e008ad4:	20706d75 3d3d3d3d 3d3d3d3d 000a3d3d     ump ==========..
 e008ae4:	72727543 20746e65 63617453 6f50206b     Current Stack Po
 e008af4:	65746e69 203d2072 202c7025 20646e61     inter = %p, and 
 e008b04:	706d7564 61747320 64206b63 68747065     dump stack depth
 e008b14:	25203d20 000a0d64 00003052 00003452      = %d...R0..R4..
 e008b24:	3d3d3d3d 3d3d3d3d 43203d3d 68736172     ========== Crash
 e008b34:	6d754420 3d3d2070 3d3d3d3d 3d3d3d3d      Dump ==========
 e008b44:	0000000a 3d3d3d3d 3d3d3d3d 52203d3d     ....========== R
 e008b54:	73696765 20726574 706d7544 3d3d3d20     egister Dump ===
 e008b64:	3d3d3d3d 0a3d3d3d 00000000 4c20205b     =======.....[  L
 e008b74:	30205d52 38302578 000a786c 5020205b     R] 0x%08lx..[  P
 e008b84:	30205d43 38302578 000a786c 5350785b     C] 0x%08lx..[xPS
 e008b94:	30205d52 38302578 000a786c 4358455b     R] 0x%08lx..[EXC
 e008ba4:	30205d52 38302578 000a786c 7325205b     R] 0x%08lx..[ %s
 e008bb4:	7830205d 6c383025 00000a78 00323152     ] 0x%08lx...R12.
 e008bc4:	3d3d3d3d 3d3d3d3d 45203d3d 6f20646e     ========== End o
 e008bd4:	72432066 20687361 706d7544 3d3d3d20     f Crash Dump ===
 e008be4:	3d3d3d3d 0a3d3d3d 00000000 0a0d0a0d     =======.........
 e008bf4:	00000000 00003552 00003652 00003752     ....R5..R6..R7..
 e008c04:	00003852 00003952 00303152 00313152     R8..R9..R10.R11.
 e008c14:	00003152 00003252 00003352              R1..R2..R3..

0e008c20 <register_string>:
 e008c20:	00000000 0e008b20 0e008bf8 0e008bfc     .... ...........
 e008c30:	0e008c00 0e008c04 0e008c08 0e008c0c     ................
 e008c40:	0e008c10 0e008b1c 0e008c14 0e008c18     ................
 e008c50:	0e008c1c 0e008bc0 65637845 6f697470     ........Exceptio
 e008c60:	6163206e 74686775 206e6f20 000a7025     n caught on %p..
 e008c70:	64726148 75616620 6920746c 61632073     Hard fault is ca
 e008c80:	64657375 20796220 6c696166 76206465     used by failed v
 e008c90:	6f746365 65662072 00686374 52534648     ector fetch.HFSR
 e008ca0:	305b203a 38302578 205d786c 25203e2d     : [0x%08lx] -> %
 e008cb0:	00000a73 75636553 79746972 75616620     s...Security fau
 e008cc0:	6920746c 61632073 64657375 20796220     lt is caused by 
 e008cd0:	61766e49 2064696c 72746e65 6f702079     Invalid entry po
 e008ce0:	00746e69 52534653 305b203a 38302578     int.SFSR: [0x%08
 e008cf0:	205d786c 25203e2d 00000a73 75636553     lx] -> %s...Secu
 e008d00:	79746972 75616620 6920746c 61632073     rity fault is ca
 e008d10:	64657375 20796220 61766e49 2064696c     used by Invalid 
 e008d20:	65746e69 74697267 69732079 74616e67     integrity signat
 e008d30:	00657275 75636553 79746972 75616620     ure.Security fau
 e008d40:	6920746c 61632073 64657375 20796220     lt is caused by 
 e008d50:	61766e49 2064696c 65637865 6f697470     Invalid exceptio
 e008d60:	6572206e 6e727574 00000000 75636553     n return....Secu
 e008d70:	79746972 75616620 6920746c 61632073     rity fault is ca
 e008d80:	64657375 20796220 72747441 74756269     used by Attribut
 e008d90:	206e6f69 74696e75 6f697620 6974616c     ion unit violati
 e008da0:	00006e6f 75636553 79746972 75616620     on..Security fau
 e008db0:	6920746c 61632073 64657375 20796220     lt is caused by 
 e008dc0:	61766e49 2064696c 6e617274 69746973     Invalid transiti
 e008dd0:	00006e6f 75636553 79746972 75616620     on..Security fau
 e008de0:	6920746c 61632073 64657375 20796220     lt is caused by 
 e008df0:	797a614c 61747320 70206574 65736572     Lazy state prese
 e008e00:	74617672 206e6f69 6f727265 00000072     rvation error...
 e008e10:	75636553 79746972 75616620 6920746c     Security fault i
 e008e20:	61632073 64657375 20796220 797a614c     s caused by Lazy
 e008e30:	61747320 65206574 726f7272 00000000      state error....
 e008e40:	20656854 75636573 79746972 75616620     The security fau
 e008e50:	6f20746c 72756363 20646572 72646461     lt occurred addr
 e008e60:	20737365 00007369 52414653 305b203a     ess is..SFAR: [0
 e008e70:	38302578 205d786c 25203e2d 00000a73     x%08lx] -> %s...
 e008e80:	6f6d654d 6d207972 67616e61 6e656d65     Memory managemen
 e008e90:	61662074 20746c75 63207369 65737561     t fault is cause
 e008ea0:	79622064 736e6920 63757274 6e6f6974     d by instruction
 e008eb0:	63636120 20737365 6c6f6976 6f697461      access violatio
 e008ec0:	0000006e 53464d4d 5b203a52 30257830     n...MMFSR: [0x%0
 e008ed0:	5d786c38 203e2d20 000a7325 6f6d654d     8lx] -> %s..Memo
 e008ee0:	6d207972 67616e61 6e656d65 61662074     ry management fa
 e008ef0:	20746c75 63207369 65737561 79622064     ult is caused by
 e008f00:	74616420 63612061 73736563 6f697620      data access vio
 e008f10:	6974616c 00006e6f 6f6d654d 6d207972     lation..Memory m
 e008f20:	67616e61 6e656d65 61662074 20746c75     anagement fault 
 e008f30:	63207369 65737561 79622064 736e7520     is caused by uns
 e008f40:	6b636174 20676e69 6f727265 00000072     tacking error...
 e008f50:	6f6d654d 6d207972 67616e61 6e656d65     Memory managemen
 e008f60:	61662074 20746c75 63207369 65737561     t fault is cause
 e008f70:	79622064 61747320 6e696b63 72652067     d by stacking er
 e008f80:	00726f72 6f6d654d 6d207972 67616e61     ror.Memory manag
 e008f90:	6e656d65 61662074 20746c75 63207369     ement fault is c
 e008fa0:	65737561 79622064 6f6c6620 6e697461     aused by floatin
 e008fb0:	6f702d67 20746e69 797a616c 61747320     g-point lazy sta
 e008fc0:	70206574 65736572 74617672 006e6f69     te preservation.
 e008fd0:	20656854 6f6d656d 6d207972 67616e61     The memory manag
 e008fe0:	6e656d65 61662074 20746c75 7563636f     ement fault occu
 e008ff0:	64657272 64646120 73736572 00736920     rred address is.
 e009000:	52414d4d 305b203a 38302578 205d786c     MMAR: [0x%08lx] 
 e009010:	25203e2d 00000a73 20737542 6c756166     -> %s...Bus faul
 e009020:	73692074 75616320 20646573 69207962     t is caused by i
 e009030:	7274736e 69746375 61206e6f 73656363     nstruction acces
 e009040:	69762073 74616c6f 006e6f69 52534642     s violation.BFSR
 e009050:	305b203a 38302578 205d786c 25203e2d     : [0x%08lx] -> %
 e009060:	00000a73 20737542 6c756166 73692074     s...Bus fault is
 e009070:	75616320 20646573 70207962 69636572      caused by preci
 e009080:	64206573 20617461 65636361 76207373     se data access v
 e009090:	616c6f69 6e6f6974 00000000 20737542     iolation....Bus 
 e0090a0:	6c756166 73692074 75616320 20646573     fault is caused 
 e0090b0:	69207962 6572706d 65736963 74616420     by imprecise dat
 e0090c0:	63612061 73736563 6f697620 6974616c     a access violati
 e0090d0:	00006e6f 20737542 6c756166 73692074     on..Bus fault is
 e0090e0:	75616320 20646573 75207962 6174736e      caused by unsta
 e0090f0:	6e696b63 72652067 00726f72 20737542     cking error.Bus 
 e009100:	6c756166 73692074 75616320 20646573     fault is caused 
 e009110:	73207962 6b636174 20676e69 6f727265     by stacking erro
 e009120:	00000072 20737542 6c756166 73692074     r...Bus fault is
 e009130:	75616320 20646573 66207962 74616f6c      caused by float
 e009140:	2d676e69 6e696f70 616c2074 7320797a     ing-point lazy s
 e009150:	65746174 65727020 76726573 6f697461     tate preservatio
 e009160:	0000006e 20656854 20737562 6c756166     n...The bus faul
 e009170:	636f2074 72727563 61206465 65726464     t occurred addre
 e009180:	69207373 00000073 52414642 305b203a     ss is...BFAR: [0
 e009190:	38302578 205d786c 25203e2d 00000a73     x%08lx] -> %s...
 e0091a0:	67617355 61662065 20746c75 63207369     Usage fault is c
 e0091b0:	65737561 79622064 74746120 74706d65     aused by attempt
 e0091c0:	6f742073 65786520 65747563 206e6120     s to execute an 
 e0091d0:	65646e75 656e6966 6e692064 75727473     undefined instru
 e0091e0:	6f697463 0000006e 52534655 305b203a     ction...UFSR: [0
 e0091f0:	38302578 205d786c 25203e2d 00000a73     x%08lx] -> %s...
 e009200:	67617355 61662065 20746c75 63207369     Usage fault is c
 e009210:	65737561 79622064 74746120 74706d65     aused by attempt
 e009220:	6f742073 69777320 20686374 61206f74     s to switch to a
 e009230:	6e69206e 696c6176 74732064 20657461     n invalid state 
 e009240:	672e6528 41202c2e 00294d52 67617355     (e.g., ARM).Usag
 e009250:	61662065 20746c75 63207369 65737561     e fault is cause
 e009260:	79622064 74746120 74706d65 6f742073     d by attempts to
 e009270:	206f6420 65206e61 70656378 6e6f6974      do an exception
 e009280:	74697720 20612068 20646162 756c6176      with a bad valu
 e009290:	6e692065 65687420 43584520 5445525f     e in the EXC_RET
 e0092a0:	204e5255 626d756e 00007265 67617355     URN number..Usag
 e0092b0:	61662065 20746c75 63207369 65737561     e fault is cause
 e0092c0:	79622064 74746120 74706d65 6f742073     d by attempts to
 e0092d0:	65786520 65747563 63206120 6f72706f      execute a copro
 e0092e0:	73736563 6920726f 7274736e 69746375     cessor instructi
 e0092f0:	00006e6f 67617355 61662065 20746c75     on..Usage fault 
 e009300:	63207369 65737561 79622064 646e6920     is caused by ind
 e009310:	74616369 74207365 20746168 74732061     icates that a st
 e009320:	206b6361 7265766f 776f6c66 61682820     ack overflow (ha
 e009330:	61776472 63206572 6b636568 61682029     rdware check) ha
 e009340:	61742073 206e656b 63616c70 00000065     s taken place...
 e009350:	67617355 61662065 20746c75 63207369     Usage fault is c
 e009360:	65737561 79622064 646e6920 74616369     aused by indicat
 e009370:	74207365 20746168 75206e61 696c616e     es that an unali
 e009380:	64656e67 63636120 20737365 6c756166     gned access faul
 e009390:	61682074 61742073 206e656b 63616c70     t has taken plac
 e0093a0:	00000065 67617355 61662065 20746c75     e...Usage fault 
 e0093b0:	63207369 65737561 79622064 646e4920     is caused by Ind
 e0093c0:	74616369 61207365 76696420 20656469     icates a divide 
 e0093d0:	7a207962 206f7265 20736168 656b6174     by zero has take
 e0093e0:	6c70206e 20656361 6e616328 20656220     n place (can be 
 e0093f0:	20746573 796c6e6f 20666920 5f564944     set only if DIV_
 e009400:	52545f30 73692050 74657320 00000029     0_TRP is set)...
 e009410:	75626544 61662067 20746c75 63207369     Debug fault is c
 e009420:	65737561 79622064 6c616820 65722074     aused by halt re
 e009430:	73657571 20646574 4e206e69 00434956     quested in NVIC.
 e009440:	52534644 305b203a 38302578 205d786c     DFSR: [0x%08lx] 
 e009450:	25203e2d 00000a73 75626544 61662067     -> %s...Debug fa
 e009460:	20746c75 63207369 65737561 79622064     ult is caused by
 e009470:	504b4220 6e692054 75727473 6f697463      BKPT instructio
 e009480:	7865206e 74756365 00006465 75626544     n executed..Debu
 e009490:	61662067 20746c75 63207369 65737561     g fault is cause
 e0094a0:	79622064 54574420 74616d20 6f206863     d by DWT match o
 e0094b0:	72756363 00646572 75626544 61662067     ccurred.Debug fa
 e0094c0:	20746c75 63207369 65737561 79622064     ult is caused by
 e0094d0:	63655620 20726f74 63746566 636f2068      Vector fetch oc
 e0094e0:	72727563 00006465 75626544 61662067     curred..Debug fa
 e0094f0:	20746c75 63207369 65737561 79622064     ult is caused by
 e009500:	42444520 20515247 6e676973 61206c61      EDBGRQ signal a
 e009510:	72657373 00646574 69766544 49206563     sserted.Device I
 e009520:	41204350 6d204950 61737365 69206567     PC API message i
 e009530:	554e2073 202c4c4c 61766e69 2164696c     s NULL, invalid!
 e009540:	00000d0a 74736f48 49504120 6b6e5520     ....Host API Unk
 e009550:	6e776f6e 65766520 2528746e 0a212978     nown event(%x)!.
 e009560:	0000000d 6c207325 20747361 63696e69     ....%s last inic
 e009570:	63706920 746f6e20 6e616820 64656c64      ipc not handled
 e009580:	00000a20 77207325 69746961 6920676e      ...%s waiting i
 e009590:	2063696e 20637069 656e6f64 00000a20     nic ipc done ...
 e0095a0:	63696e69 6970615f 736f685f 61745f74     inic_api_host_ta
 e0095b0:	00006b73 61657243 69206574 5f63696e     sk..Create inic_
 e0095c0:	5f697061 74736f68 7361745f 7245206b     api_host_task Er
 e0095d0:	0a212172 00000000 43494e49 00000000     r!!.....INIC....
 e0095e0:	72657375 66756220 726f6620 20706d20     user buf for mp 
 e0095f0:	6c6c616d 6620636f 656c6961 00000a64     malloc failed...
 e009600:	76697250 20657461 7373654d 3a656761     Private Message:
 e009610:	0a732520 00000000                        %s.....

0e009618 <__func__.0>:
 e009618:	63696e69 6970615f 736f685f 656d5f74     inic_api_host_me
 e009628:	67617373 65735f65 0000646e 74736f68     ssage_send..host
 e009638:	626b735f 666e695f 726f206f 736f6820     _skb_info or hos
 e009648:	6b735f74 61645f62 6e206174 6d20746f     t_skb_data not m
 e009658:	65657266 00000a7c 3e3d7325 20626b73     free|...%s=>skb 
 e009668:	6c6c616d 6620636f 216c6961 00000d0a     malloc fail!....
 e009678:	74736f48 6b6e5520 6e776f6e 65766520     Host Unknown eve
 e009688:	2528746e 2129786c 00000d0a              nt(%lx)!....

0e009694 <__func__.2>:
 e009694:	63696e69 736f685f 6e695f74 735f7469     inic_host_init_s
 e0096a4:	0000626b 63696e69 67736d5f 745f715f     kb..inic_msg_q_t
 e0096b4:	006b7361 61657243 69206574 5f63696e     ask.Create inic_
 e0096c4:	5f67736d 61745f71 45206b73 21217272     msg_q_task Err!!
 e0096d4:	0000000a 62204f4e 65666675 6f662072     ....NO buffer fo
 e0096e4:	656e2072 6f6e2077 21736564 00000d0a     r new nodes!....
 e0096f4:	61766e49 2064696c 6e204442 65626d75     Invalid BD numbe
 e009704:	000a2172 50494843 4f464e49 00000000     r!..CHIPINFO....
 e009714:	6f632f2e 6e6f706d 2f746e65 2f636f73     ./component/soc/
 e009724:	62656d61 6c706461 6c2f7375 722f6269     amebadplus/lib/r
 e009734:	635f6d61 6f6d6d6f 6d612f6e 5f616265     am_common/ameba_
 e009744:	70696863 6f666e69 62696c5f 0000632e     chipinfo_lib.c..
 e009754:	203a7325 00000000 78323025 00000000     %s: ....%02x....
 e009764:	3a525245 74654720 636f5320 6d614e20     ERR: Get Soc Nam
 e009774:	61462065 000a6c69 504d4f43 20454c49     e Fail..COMPILE 
 e009784:	454d4954 7325203a 0000000a 34323032     TIME: %s....2024
 e009794:	2f35302f 312d3731 34343a31 0035353a     /05/17-11:44:55.
 e0097a4:	504d4f43 52454c49 7325203a 0000000a     COMPILER: %s....
 e0097b4:	20636367 73726576 206e6f69 332e3031     gcc version 10.3
 e0097c4:	3220312e 30323230 20373131 61655228     .1 20220117 (Rea
 e0097d4:	6b65746c 44534120 30312d4b 312e332e     ltek ASDK-10.3.1
 e0097e4:	69754220 3420646c 2b323730 00002029      Build 4072+) ..
 e0097f4:	4c207325 56206269 69737265 0a3a6e6f     %s Lib Version:.
 e009804:	00000000 00006325                       ....%c..

0e00980c <Chip_Info>:
 e00980c:	03f40100 00020107 03f70200 00040000     ................
 e00981c:	03f80300 00040000 04030400 00080000     ................
 e00982c:	04010500 00400000 04000600 00040000     ......@.........
 e00983c:	04090700 00020107 04020800 00040000     ................
 e00984c:	040a0900 00020107 04110a00 00020107     ................
 e00985c:	04120b00 00040000 04130c00 00040000     ................
 e00986c:	04160d00 00080000 04170e00 00080000     ................
 e00987c:	04190f00 0001020a 041a1000 00080000     ................
 e00988c:	04201100 00400000 041c1200 00020107     .. ...@.........
 e00989c:	041d0e01 00080000 041f0901 00020107     ................
 e0098ac:	040f0501 00400000 ffffffff 00400000     ......@.......@.
 e0098bc:	2050544f 20746567 73207768 20616d65     OTP get hw sema 
 e0098cc:	6c696166 0000000a 504d4f52 48435441     fail....ROMPATCH
 e0098dc:	00000000 64206f69 65766972 61702072     ....io driver pa
 e0098ec:	656d6172 73726574 72726520 2021726f     rameters error! 
 e0098fc:	656c6966 6d616e5f 25203a65 6c202c73     file_name: %s, l
 e00990c:	3a656e69 00642520 6f632f2e 6e6f706d     ine: %d../compon
 e00991c:	2f746e65 2f636f73 62656d61 6c706461     ent/soc/amebadpl
 e00992c:	6c2f7375 722f6269 635f6d61 6f6d6d6f     us/lib/ram_commo
 e00993c:	6d612f6e 5f616265 5f6d6f72 63746170     n/ameba_rom_patc
 e00994c:	00632e68                                h.c.

0e009950 <shell_cmd_table>:
 e009950:	0e008444 00000000 0e0027e1 0e008448     D........'..H...
 e009960:	0e00846c 00000004 0e0028d1 0e008470     l........(..p...
 e009970:	0e00858c 00000002 0e002871 0e008590     ........q(......
 e009980:	0e008620 00000004 0e00282d 0e008628      .......-(..(...

0e009990 <ipc_shell_table>:
 e009990:	00000001 0e003c21 00000000 0e0018e9     ....!<..........
	...
 e0099a8:	00000004                                ....

0e0099ac <ipc_tickless_table>:
 e0099ac:	00000001 0e005405 00000000 0e0018e9     .....T..........
	...
 e0099c4:	00000001                                ....

0e0099c8 <ipc_api_host_table>:
 e0099c8:	00000000 0e005f85 00000000 0e0018e9     ....._..........
	...
 e0099e0:	00000007                                ....

0e0099e4 <ipc_host_event_table>:
 e0099e4:	00000000 0e0064c9 00000000 0e0018e9     .....d..........
	...
 e0099fc:	00000006                                ....

0e009a00 <lib_pmc_git_rev>:
 e009a00:	62696c20 636d705f 7265765f 3830305f      lib_pmc_ver_008
 e009a10:	36343133 5f613635 34323032 2f35302f     314656a_2024/05/
 e009a20:	312d3731 35343a31 0033313a              17-11:45:13.

0e009a2c <lib_wifi_com_sec_git_rev>:
 e009a2c:	62696c20 6669775f 6f635f69 65735f6d      lib_wifi_com_se
 e009a3c:	65765f63 30305f72 34313338 61363536     c_ver_008314656a
 e009a4c:	3230325f 35302f34 2d37312f 343a3131     _2024/05/17-11:4
 e009a5c:	30313a35 00000000                       5:10....

0e009a64 <lib_wps_git_rev>:
 e009a64:	62696c20 7370775f 7265765f 3830305f      lib_wps_ver_008
 e009a74:	36343133 5f613635 34323032 2f35302f     314656a_2024/05/
 e009a84:	312d3731 35343a31 0033313a              17-11:45:13.

0e009a90 <__frame_dummy_init_array_entry>:
 e009a90:	0e007435 0e007455                       5t..Ut..

0e009a98 <__do_global_dtors_aux_fini_array_entry>:
 e009a98:	0e00740d                                .t..

0e009a9c <__fini_array_end>:
 e009a9c:	00000000 	.word	0x00000000
