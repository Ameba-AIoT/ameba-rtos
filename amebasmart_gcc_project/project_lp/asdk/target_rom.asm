
D:/0code/v03slave/1202/project/realtek_amebaD2_va0_example/GCC-RELEASE/project_lp/asdk/image/target_rom.axf:     file format elf32-littlearm


Disassembly of section .rom.text:

00000000 <__vectors_table>:
       0:	2301fffc 	.word	0x2301fffc
       4:	00000101 	.word	0x00000101
       8:	00000105 	.word	0x00000105
       c:	00000109 	.word	0x00000109
      10:	0000010d 	.word	0x0000010d
      14:	00000111 	.word	0x00000111
	...

00000100 <Reset_Handler>:
     100:	f003 b9f8 	b.w	34f4 <BOOT_ROM_ResetVsr>

00000104 <NMI_Handler>:
     104:	f003 bb90 	b.w	3828 <INT_NMI>

00000108 <HardFault_Handler>:
     108:	f003 bb9c 	b.w	3844 <INT_HardFault>

0000010c <MemManage_Handler>:
     10c:	f003 bba6 	b.w	385c <INT_MemManage>

00000110 <BusFault_Handler>:
     110:	f003 bbb2 	b.w	3878 <INT_BusFault>

00000114 <io_assert_failed>:
     114:	4b05      	ldr	r3, [pc, #20]	; (12c <io_assert_failed+0x18>)
     116:	000a      	movs	r2, r1
     118:	681b      	ldr	r3, [r3, #0]
     11a:	b510      	push	{r4, lr}
     11c:	039b      	lsls	r3, r3, #14
     11e:	d503      	bpl.n	128 <io_assert_failed+0x14>
     120:	0001      	movs	r1, r0
     122:	4803      	ldr	r0, [pc, #12]	; (130 <io_assert_failed+0x1c>)
     124:	f003 ff28 	bl	3f78 <DiagPrintf>
     128:	e7fe      	b.n	128 <io_assert_failed+0x14>
     12a:	46c0      	nop			; (mov r8, r8)
     12c:	230005d8 	.word	0x230005d8
     130:	0000c0d0 	.word	0x0000c0d0

00000134 <BKUP_Write>:
     134:	b570      	push	{r4, r5, r6, lr}
     136:	0004      	movs	r4, r0
     138:	000d      	movs	r5, r1
     13a:	2803      	cmp	r0, #3
     13c:	d903      	bls.n	146 <BKUP_Write+0x12>
     13e:	211f      	movs	r1, #31
     140:	4803      	ldr	r0, [pc, #12]	; (150 <BKUP_Write+0x1c>)
     142:	f7ff ffe7 	bl	114 <io_assert_failed>
     146:	4b03      	ldr	r3, [pc, #12]	; (154 <BKUP_Write+0x20>)
     148:	00a4      	lsls	r4, r4, #2
     14a:	50e5      	str	r5, [r4, r3]
     14c:	bd70      	pop	{r4, r5, r6, pc}
     14e:	46c0      	nop			; (mov r8, r8)
     150:	0000c37a 	.word	0x0000c37a
     154:	420080e0 	.word	0x420080e0

00000158 <BKUP_Read>:
     158:	b510      	push	{r4, lr}
     15a:	0004      	movs	r4, r0
     15c:	2803      	cmp	r0, #3
     15e:	d903      	bls.n	168 <BKUP_Read+0x10>
     160:	212e      	movs	r1, #46	; 0x2e
     162:	4803      	ldr	r0, [pc, #12]	; (170 <BKUP_Read+0x18>)
     164:	f7ff ffd6 	bl	114 <io_assert_failed>
     168:	4b02      	ldr	r3, [pc, #8]	; (174 <BKUP_Read+0x1c>)
     16a:	00a4      	lsls	r4, r4, #2
     16c:	58e0      	ldr	r0, [r4, r3]
     16e:	bd10      	pop	{r4, pc}
     170:	0000c370 	.word	0x0000c370
     174:	420080e0 	.word	0x420080e0

00000178 <BKUP_Set>:
     178:	b570      	push	{r4, r5, r6, lr}
     17a:	0004      	movs	r4, r0
     17c:	000d      	movs	r5, r1
     17e:	2803      	cmp	r0, #3
     180:	d903      	bls.n	18a <BKUP_Set+0x12>
     182:	213e      	movs	r1, #62	; 0x3e
     184:	4804      	ldr	r0, [pc, #16]	; (198 <BKUP_Set+0x20>)
     186:	f7ff ffc5 	bl	114 <io_assert_failed>
     18a:	4b04      	ldr	r3, [pc, #16]	; (19c <BKUP_Set+0x24>)
     18c:	00a4      	lsls	r4, r4, #2
     18e:	18e4      	adds	r4, r4, r3
     190:	6821      	ldr	r1, [r4, #0]
     192:	4329      	orrs	r1, r5
     194:	6021      	str	r1, [r4, #0]
     196:	bd70      	pop	{r4, r5, r6, pc}
     198:	0000c367 	.word	0x0000c367
     19c:	420080e0 	.word	0x420080e0

000001a0 <BKUP_Clear>:
     1a0:	b570      	push	{r4, r5, r6, lr}
     1a2:	0004      	movs	r4, r0
     1a4:	000d      	movs	r5, r1
     1a6:	2803      	cmp	r0, #3
     1a8:	d903      	bls.n	1b2 <BKUP_Clear+0x12>
     1aa:	214e      	movs	r1, #78	; 0x4e
     1ac:	4804      	ldr	r0, [pc, #16]	; (1c0 <BKUP_Clear+0x20>)
     1ae:	f7ff ffb1 	bl	114 <io_assert_failed>
     1b2:	4b04      	ldr	r3, [pc, #16]	; (1c4 <BKUP_Clear+0x24>)
     1b4:	00a4      	lsls	r4, r4, #2
     1b6:	18e4      	adds	r4, r4, r3
     1b8:	6823      	ldr	r3, [r4, #0]
     1ba:	43ab      	bics	r3, r5
     1bc:	6023      	str	r3, [r4, #0]
     1be:	bd70      	pop	{r4, r5, r6, pc}
     1c0:	0000c35c 	.word	0x0000c35c
     1c4:	420080e0 	.word	0x420080e0

000001c8 <XTAL_ClkGet>:
     1c8:	4b03      	ldr	r3, [pc, #12]	; (1d8 <XTAL_ClkGet+0x10>)
     1ca:	4a04      	ldr	r2, [pc, #16]	; (1dc <XTAL_ClkGet+0x14>)
     1cc:	681b      	ldr	r3, [r3, #0]
     1ce:	021b      	lsls	r3, r3, #8
     1d0:	0f1b      	lsrs	r3, r3, #28
     1d2:	009b      	lsls	r3, r3, #2
     1d4:	5898      	ldr	r0, [r3, r2]
     1d6:	4770      	bx	lr
     1d8:	42008224 	.word	0x42008224
     1dc:	0000c000 	.word	0x0000c000

000001e0 <CPU_ClkSet>:
     1e0:	b530      	push	{r4, r5, lr}
     1e2:	4b14      	ldr	r3, [pc, #80]	; (234 <CPU_ClkSet+0x54>)
     1e4:	4c14      	ldr	r4, [pc, #80]	; (238 <CPU_ClkSet+0x58>)
     1e6:	6822      	ldr	r2, [r4, #0]
     1e8:	6819      	ldr	r1, [r3, #0]
     1ea:	2303      	movs	r3, #3
     1ec:	0989      	lsrs	r1, r1, #6
     1ee:	000d      	movs	r5, r1
     1f0:	401d      	ands	r5, r3
     1f2:	4219      	tst	r1, r3
     1f4:	d10a      	bne.n	20c <CPU_ClkSet+0x2c>
     1f6:	439a      	bics	r2, r3
     1f8:	4003      	ands	r3, r0
     1fa:	4313      	orrs	r3, r2
     1fc:	b120      	cbz	r0, 208 <CPU_ClkSet+0x28>
     1fe:	4a0f      	ldr	r2, [pc, #60]	; (23c <CPU_ClkSet+0x5c>)
     200:	0900      	lsrs	r0, r0, #4
     202:	4013      	ands	r3, r2
     204:	0500      	lsls	r0, r0, #20
     206:	4303      	orrs	r3, r0
     208:	6023      	str	r3, [r4, #0]
     20a:	bd30      	pop	{r4, r5, pc}
     20c:	2d01      	cmp	r5, #1
     20e:	d1fc      	bne.n	20a <CPU_ClkSet+0x2a>
     210:	b960      	cbnz	r0, 22c <CPU_ClkSet+0x4c>
     212:	4b0b      	ldr	r3, [pc, #44]	; (240 <CPU_ClkSet+0x60>)
     214:	401a      	ands	r2, r3
     216:	6022      	str	r2, [r4, #0]
     218:	2800      	cmp	r0, #0
     21a:	d0f6      	beq.n	20a <CPU_ClkSet+0x2a>
     21c:	2107      	movs	r1, #7
     21e:	4a09      	ldr	r2, [pc, #36]	; (244 <CPU_ClkSet+0x64>)
     220:	4008      	ands	r0, r1
     222:	6813      	ldr	r3, [r2, #0]
     224:	438b      	bics	r3, r1
     226:	4318      	orrs	r0, r3
     228:	6010      	str	r0, [r2, #0]
     22a:	e7ee      	b.n	20a <CPU_ClkSet+0x2a>
     22c:	f240 1300 	movw	r3, #256	; 0x100
     230:	431a      	orrs	r2, r3
     232:	e7f0      	b.n	216 <CPU_ClkSet+0x36>
     234:	4200827c 	.word	0x4200827c
     238:	42008224 	.word	0x42008224
     23c:	ff0fffff 	.word	0xff0fffff
     240:	fffffeff 	.word	0xfffffeff
     244:	42008228 	.word	0x42008228

00000248 <CPU_ClkGet>:
     248:	4b19      	ldr	r3, [pc, #100]	; (2b0 <CPU_ClkGet+0x68>)
     24a:	b570      	push	{r4, r5, r6, lr}
     24c:	681d      	ldr	r5, [r3, #0]
     24e:	4b19      	ldr	r3, [pc, #100]	; (2b4 <CPU_ClkGet+0x6c>)
     250:	681c      	ldr	r4, [r3, #0]
     252:	4b19      	ldr	r3, [pc, #100]	; (2b8 <CPU_ClkGet+0x70>)
     254:	4798      	blx	r3
     256:	0624      	lsls	r4, r4, #24
     258:	0fa4      	lsrs	r4, r4, #30
     25a:	0003      	movs	r3, r0
     25c:	b934      	cbnz	r4, 26c <CPU_ClkGet+0x24>
     25e:	2801      	cmp	r0, #1
     260:	d01f      	beq.n	2a2 <CPU_ClkGet+0x5a>
     262:	07ab      	lsls	r3, r5, #30
     264:	d51f      	bpl.n	2a6 <CPU_ClkGet+0x5e>
     266:	4b15      	ldr	r3, [pc, #84]	; (2bc <CPU_ClkGet+0x74>)
     268:	4798      	blx	r3
     26a:	bd70      	pop	{r4, r5, r6, pc}
     26c:	2000      	movs	r0, #0
     26e:	2c01      	cmp	r4, #1
     270:	d1fb      	bne.n	26a <CPU_ClkGet+0x22>
     272:	2b01      	cmp	r3, #1
     274:	d107      	bne.n	286 <CPU_ClkGet+0x3e>
     276:	4b12      	ldr	r3, [pc, #72]	; (2c0 <CPU_ClkGet+0x78>)
     278:	6818      	ldr	r0, [r3, #0]
     27a:	0c00      	lsrs	r0, r0, #16
     27c:	b2c3      	uxtb	r3, r0
     27e:	b1a3      	cbz	r3, 2aa <CPU_ClkGet+0x62>
     280:	4810      	ldr	r0, [pc, #64]	; (2c4 <CPU_ClkGet+0x7c>)
     282:	4358      	muls	r0, r3
     284:	e7f1      	b.n	26a <CPU_ClkGet+0x22>
     286:	0a2d      	lsrs	r5, r5, #8
     288:	002b      	movs	r3, r5
     28a:	4023      	ands	r3, r4
     28c:	4225      	tst	r5, r4
     28e:	d003      	beq.n	298 <CPU_ClkGet+0x50>
     290:	2207      	movs	r2, #7
     292:	4b0d      	ldr	r3, [pc, #52]	; (2c8 <CPU_ClkGet+0x80>)
     294:	681b      	ldr	r3, [r3, #0]
     296:	4013      	ands	r3, r2
     298:	4a0c      	ldr	r2, [pc, #48]	; (2cc <CPU_ClkGet+0x84>)
     29a:	009b      	lsls	r3, r3, #2
     29c:	189b      	adds	r3, r3, r2
     29e:	6c18      	ldr	r0, [r3, #64]	; 0x40
     2a0:	e7e3      	b.n	26a <CPU_ClkGet+0x22>
     2a2:	480b      	ldr	r0, [pc, #44]	; (2d0 <CPU_ClkGet+0x88>)
     2a4:	e7e1      	b.n	26a <CPU_ClkGet+0x22>
     2a6:	480b      	ldr	r0, [pc, #44]	; (2d4 <CPU_ClkGet+0x8c>)
     2a8:	e7df      	b.n	26a <CPU_ClkGet+0x22>
     2aa:	480b      	ldr	r0, [pc, #44]	; (2d8 <CPU_ClkGet+0x90>)
     2ac:	e7dd      	b.n	26a <CPU_ClkGet+0x22>
     2ae:	46c0      	nop			; (mov r8, r8)
     2b0:	42008224 	.word	0x42008224
     2b4:	4200827c 	.word	0x4200827c
     2b8:	00001dbd 	.word	0x00001dbd
     2bc:	000001c9 	.word	0x000001c9
     2c0:	4200826c 	.word	0x4200826c
     2c4:	000f4240 	.word	0x000f4240
     2c8:	42008228 	.word	0x42008228
     2cc:	0000c000 	.word	0x0000c000
     2d0:	01312d00 	.word	0x01312d00
     2d4:	003d0900 	.word	0x003d0900
     2d8:	02faf080 	.word	0x02faf080

000002dc <HBUS_ClkSet>:
     2dc:	4a05      	ldr	r2, [pc, #20]	; (2f4 <HBUS_ClkSet+0x18>)
     2de:	4906      	ldr	r1, [pc, #24]	; (2f8 <HBUS_ClkSet+0x1c>)
     2e0:	6813      	ldr	r3, [r2, #0]
     2e2:	0280      	lsls	r0, r0, #10
     2e4:	400b      	ands	r3, r1
     2e6:	f240 4100 	movw	r1, #1024	; 0x400
     2ea:	4008      	ands	r0, r1
     2ec:	4318      	orrs	r0, r3
     2ee:	6010      	str	r0, [r2, #0]
     2f0:	4770      	bx	lr
     2f2:	46c0      	nop			; (mov r8, r8)
     2f4:	42008224 	.word	0x42008224
     2f8:	fffffbff 	.word	0xfffffbff
     2fc:	00000000 	.word	0x00000000

00000300 <DelayNop>:
     300:	3801      	subs	r0, #1
     302:	2800      	cmp	r0, #0
     304:	46c0      	nop			; (mov r8, r8)
     306:	d1fb      	bne.n	300 <DelayNop>
     308:	4770      	bx	lr

0000030a <DelayUs>:
     30a:	b51f      	push	{r0, r1, r2, r3, r4, lr}
     30c:	4b17      	ldr	r3, [pc, #92]	; (36c <DelayUs+0x62>)
     30e:	0004      	movs	r4, r0
     310:	4798      	blx	r3
     312:	4a17      	ldr	r2, [pc, #92]	; (370 <DelayUs+0x66>)
     314:	4b17      	ldr	r3, [pc, #92]	; (374 <DelayUs+0x6a>)
     316:	6812      	ldr	r2, [r2, #0]
     318:	fbb0 f3f3 	udiv	r3, r0, r3
     31c:	0612      	lsls	r2, r2, #24
     31e:	0f92      	lsrs	r2, r2, #30
     320:	b9ea      	cbnz	r2, 35e <DelayUs+0x54>
     322:	4363      	muls	r3, r4
     324:	0018      	movs	r0, r3
     326:	2305      	movs	r3, #5
     328:	fbb0 f0f3 	udiv	r0, r0, r3
     32c:	4912      	ldr	r1, [pc, #72]	; (378 <DelayUs+0x6e>)
     32e:	680b      	ldr	r3, [r1, #0]
     330:	b98b      	cbnz	r3, 356 <DelayUs+0x4c>
     332:	2005      	movs	r0, #5
     334:	331a      	adds	r3, #26
     336:	9300      	str	r3, [sp, #0]
     338:	334a      	adds	r3, #74	; 0x4a
     33a:	9301      	str	r3, [sp, #4]
     33c:	9002      	str	r0, [sp, #8]
     33e:	ab02      	add	r3, sp, #8
     340:	3002      	adds	r0, #2
     342:	6058      	str	r0, [r3, #4]
     344:	6848      	ldr	r0, [r1, #4]
     346:	0092      	lsls	r2, r2, #2
     348:	b908      	cbnz	r0, 34e <DelayUs+0x44>
     34a:	4669      	mov	r1, sp
     34c:	5888      	ldr	r0, [r1, r2]
     34e:	4344      	muls	r4, r0
     350:	5898      	ldr	r0, [r3, r2]
     352:	fbb4 f0f0 	udiv	r0, r4, r0
     356:	b108      	cbz	r0, 35c <DelayUs+0x52>
     358:	4b08      	ldr	r3, [pc, #32]	; (37c <DelayUs+0x72>)
     35a:	4798      	blx	r3
     35c:	bd1f      	pop	{r0, r1, r2, r3, r4, pc}
     35e:	2000      	movs	r0, #0
     360:	2a01      	cmp	r2, #1
     362:	d1e3      	bne.n	32c <DelayUs+0x22>
     364:	4363      	muls	r3, r4
     366:	0018      	movs	r0, r3
     368:	2307      	movs	r3, #7
     36a:	e7dd      	b.n	328 <DelayUs+0x1e>
     36c:	00000249 	.word	0x00000249
     370:	4200827c 	.word	0x4200827c
     374:	000f4240 	.word	0x000f4240
     378:	23000250 	.word	0x23000250
     37c:	00000301 	.word	0x00000301

00000380 <DelayMs>:
     380:	f240 33e8 	movw	r3, #1000	; 0x3e8
     384:	b510      	push	{r4, lr}
     386:	4358      	muls	r0, r3
     388:	4b01      	ldr	r3, [pc, #4]	; (390 <DelayMs+0x10>)
     38a:	4798      	blx	r3
     38c:	bd10      	pop	{r4, pc}
     38e:	46c0      	nop			; (mov r8, r8)
     390:	0000030b 	.word	0x0000030b

00000394 <FLASH_SetSpiMode>:
     394:	b5f0      	push	{r4, r5, r6, r7, lr}
     396:	2588      	movs	r5, #136	; 0x88
     398:	2200      	movs	r2, #0
     39a:	1843      	adds	r3, r0, r1
     39c:	7f9c      	ldrb	r4, [r3, #30]
     39e:	7946      	ldrb	r6, [r0, #5]
     3a0:	05ed      	lsls	r5, r5, #23
     3a2:	60aa      	str	r2, [r5, #8]
     3a4:	4374      	muls	r4, r6
     3a6:	4a2d      	ldr	r2, [pc, #180]	; (45c <FLASH_SetSpiMode+0xc8>)
     3a8:	7f45      	ldrb	r5, [r0, #29]
     3aa:	6a17      	ldr	r7, [r2, #32]
     3ac:	0064      	lsls	r4, r4, #1
     3ae:	072d      	lsls	r5, r5, #28
     3b0:	0c3f      	lsrs	r7, r7, #16
     3b2:	b2a3      	uxth	r3, r4
     3b4:	0c2d      	lsrs	r5, r5, #16
     3b6:	043f      	lsls	r7, r7, #16
     3b8:	0524      	lsls	r4, r4, #20
     3ba:	432f      	orrs	r7, r5
     3bc:	0d24      	lsrs	r4, r4, #20
     3be:	433c      	orrs	r4, r7
     3c0:	6214      	str	r4, [r2, #32]
     3c2:	0004      	movs	r4, r0
     3c4:	3463      	adds	r4, #99	; 0x63
     3c6:	7824      	ldrb	r4, [r4, #0]
     3c8:	b124      	cbz	r4, 3d4 <FLASH_SetSpiMode+0x40>
     3ca:	2904      	cmp	r1, #4
     3cc:	d11a      	bne.n	404 <FLASH_SetSpiMode+0x70>
     3ce:	00b6      	lsls	r6, r6, #2
     3d0:	1b9b      	subs	r3, r3, r6
     3d2:	b29b      	uxth	r3, r3
     3d4:	69d6      	ldr	r6, [r2, #28]
     3d6:	051b      	lsls	r3, r3, #20
     3d8:	0b36      	lsrs	r6, r6, #12
     3da:	0d1b      	lsrs	r3, r3, #20
     3dc:	0336      	lsls	r6, r6, #12
     3de:	4333      	orrs	r3, r6
     3e0:	61d3      	str	r3, [r2, #28]
     3e2:	2388      	movs	r3, #136	; 0x88
     3e4:	05db      	lsls	r3, r3, #23
     3e6:	681d      	ldr	r5, [r3, #0]
     3e8:	4b1d      	ldr	r3, [pc, #116]	; (460 <FLASH_SetSpiMode+0xcc>)
     3ea:	401d      	ands	r5, r3
     3ec:	6a53      	ldr	r3, [r2, #36]	; 0x24
     3ee:	0c1b      	lsrs	r3, r3, #16
     3f0:	041b      	lsls	r3, r3, #16
     3f2:	2903      	cmp	r1, #3
     3f4:	d02a      	beq.n	44c <FLASH_SetSpiMode+0xb8>
     3f6:	d809      	bhi.n	40c <FLASH_SetSpiMode+0x78>
     3f8:	2901      	cmp	r1, #1
     3fa:	d010      	beq.n	41e <FLASH_SetSpiMode+0x8a>
     3fc:	2902      	cmp	r1, #2
     3fe:	d01e      	beq.n	43e <FLASH_SetSpiMode+0xaa>
     400:	2103      	movs	r1, #3
     402:	e012      	b.n	42a <FLASH_SetSpiMode+0x96>
     404:	2902      	cmp	r1, #2
     406:	d1e5      	bne.n	3d4 <FLASH_SetSpiMode+0x40>
     408:	00f6      	lsls	r6, r6, #3
     40a:	e7e1      	b.n	3d0 <FLASH_SetSpiMode+0x3c>
     40c:	2904      	cmp	r1, #4
     40e:	d1f7      	bne.n	400 <FLASH_SetSpiMode+0x6c>
     410:	6d01      	ldr	r1, [r0, #80]	; 0x50
     412:	430b      	orrs	r3, r1
     414:	21a0      	movs	r1, #160	; 0xa0
     416:	0309      	lsls	r1, r1, #12
     418:	430d      	orrs	r5, r1
     41a:	21eb      	movs	r1, #235	; 0xeb
     41c:	e005      	b.n	42a <FLASH_SetSpiMode+0x96>
     41e:	6c41      	ldr	r1, [r0, #68]	; 0x44
     420:	430b      	orrs	r3, r1
     422:	2180      	movs	r1, #128	; 0x80
     424:	02c9      	lsls	r1, r1, #11
     426:	430d      	orrs	r5, r1
     428:	213b      	movs	r1, #59	; 0x3b
     42a:	6081      	str	r1, [r0, #8]
     42c:	b114      	cbz	r4, 434 <FLASH_SetSpiMode+0xa0>
     42e:	f242 0100 	movw	r1, #8192	; 0x2000
     432:	430b      	orrs	r3, r1
     434:	6253      	str	r3, [r2, #36]	; 0x24
     436:	2388      	movs	r3, #136	; 0x88
     438:	05db      	lsls	r3, r3, #23
     43a:	601d      	str	r5, [r3, #0]
     43c:	bdf0      	pop	{r4, r5, r6, r7, pc}
     43e:	6c81      	ldr	r1, [r0, #72]	; 0x48
     440:	430b      	orrs	r3, r1
     442:	21a0      	movs	r1, #160	; 0xa0
     444:	02c9      	lsls	r1, r1, #11
     446:	430d      	orrs	r5, r1
     448:	21bb      	movs	r1, #187	; 0xbb
     44a:	e7ee      	b.n	42a <FLASH_SetSpiMode+0x96>
     44c:	6cc1      	ldr	r1, [r0, #76]	; 0x4c
     44e:	430b      	orrs	r3, r1
     450:	2180      	movs	r1, #128	; 0x80
     452:	0309      	lsls	r1, r1, #12
     454:	430d      	orrs	r5, r1
     456:	216b      	movs	r1, #107	; 0x6b
     458:	e7e7      	b.n	42a <FLASH_SetSpiMode+0x96>
     45a:	46c0      	nop			; (mov r8, r8)
     45c:	440000fc 	.word	0x440000fc
     460:	ffc0ffff 	.word	0xffc0ffff

00000464 <FLASH_RxCmd>:
     464:	b570      	push	{r4, r5, r6, lr}
     466:	0006      	movs	r6, r0
     468:	2001      	movs	r0, #1
     46a:	0014      	movs	r4, r2
     46c:	000d      	movs	r5, r1
     46e:	f000 f873 	bl	558 <FLASH_UserModeEn>
     472:	2100      	movs	r1, #0
     474:	4818      	ldr	r0, [pc, #96]	; (4d8 <FLASH_RxCmd+0x74>)
     476:	f7ff ff8d 	bl	394 <FLASH_SetSpiMode>
     47a:	2388      	movs	r3, #136	; 0x88
     47c:	4a17      	ldr	r2, [pc, #92]	; (4dc <FLASH_RxCmd+0x78>)
     47e:	4818      	ldr	r0, [pc, #96]	; (4e0 <FLASH_RxCmd+0x7c>)
     480:	69d1      	ldr	r1, [r2, #28]
     482:	05db      	lsls	r3, r3, #23
     484:	4001      	ands	r1, r0
     486:	61d1      	str	r1, [r2, #28]
     488:	0229      	lsls	r1, r5, #8
     48a:	0a09      	lsrs	r1, r1, #8
     48c:	6059      	str	r1, [r3, #4]
     48e:	2100      	movs	r1, #0
     490:	6351      	str	r1, [r2, #52]	; 0x34
     492:	681a      	ldr	r2, [r3, #0]
     494:	4913      	ldr	r1, [pc, #76]	; (4e4 <FLASH_RxCmd+0x80>)
     496:	2008      	movs	r0, #8
     498:	4011      	ands	r1, r2
     49a:	f240 3200 	movw	r2, #768	; 0x300
     49e:	430a      	orrs	r2, r1
     4a0:	601a      	str	r2, [r3, #0]
     4a2:	2201      	movs	r2, #1
     4a4:	4910      	ldr	r1, [pc, #64]	; (4e8 <FLASH_RxCmd+0x84>)
     4a6:	1965      	adds	r5, r4, r5
     4a8:	700e      	strb	r6, [r1, #0]
     4aa:	609a      	str	r2, [r3, #8]
     4ac:	0022      	movs	r2, r4
     4ae:	42aa      	cmp	r2, r5
     4b0:	d10a      	bne.n	4c8 <FLASH_RxCmd+0x64>
     4b2:	2004      	movs	r0, #4
     4b4:	f000 f81a 	bl	4ec <FLASH_WaitBusy>
     4b8:	4807      	ldr	r0, [pc, #28]	; (4d8 <FLASH_RxCmd+0x74>)
     4ba:	7901      	ldrb	r1, [r0, #4]
     4bc:	f7ff ff6a 	bl	394 <FLASH_SetSpiMode>
     4c0:	2000      	movs	r0, #0
     4c2:	f000 f849 	bl	558 <FLASH_UserModeEn>
     4c6:	bd70      	pop	{r4, r5, r6, pc}
     4c8:	6a9c      	ldr	r4, [r3, #40]	; 0x28
     4ca:	4204      	tst	r4, r0
     4cc:	d0ef      	beq.n	4ae <FLASH_RxCmd+0x4a>
     4ce:	780c      	ldrb	r4, [r1, #0]
     4d0:	7014      	strb	r4, [r2, #0]
     4d2:	3201      	adds	r2, #1
     4d4:	e7eb      	b.n	4ae <FLASH_RxCmd+0x4a>
     4d6:	46c0      	nop			; (mov r8, r8)
     4d8:	23000258 	.word	0x23000258
     4dc:	440000fc 	.word	0x440000fc
     4e0:	fff0ffff 	.word	0xfff0ffff
     4e4:	fff0fcff 	.word	0xfff0fcff
     4e8:	44000060 	.word	0x44000060

000004ec <FLASH_WaitBusy>:
     4ec:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
     4ee:	466b      	mov	r3, sp
     4f0:	2688      	movs	r6, #136	; 0x88
     4f2:	1ddd      	adds	r5, r3, #7
     4f4:	2300      	movs	r3, #0
     4f6:	0004      	movs	r4, r0
     4f8:	702b      	strb	r3, [r5, #0]
     4fa:	05f6      	lsls	r6, r6, #23
     4fc:	b92c      	cbnz	r4, 50a <FLASH_WaitBusy+0x1e>
     4fe:	6ab3      	ldr	r3, [r6, #40]	; 0x28
     500:	2201      	movs	r2, #1
     502:	4013      	ands	r3, r2
     504:	2b00      	cmp	r3, #0
     506:	d1f9      	bne.n	4fc <FLASH_WaitBusy+0x10>
     508:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}
     50a:	2c04      	cmp	r4, #4
     50c:	d101      	bne.n	512 <FLASH_WaitBusy+0x26>
     50e:	68b3      	ldr	r3, [r6, #8]
     510:	e7f6      	b.n	500 <FLASH_WaitBusy+0x14>
     512:	2c01      	cmp	r4, #1
     514:	d10a      	bne.n	52c <FLASH_WaitBusy+0x40>
     516:	4f0f      	ldr	r7, [pc, #60]	; (554 <FLASH_WaitBusy+0x68>)
     518:	002a      	movs	r2, r5
     51a:	003b      	movs	r3, r7
     51c:	3356      	adds	r3, #86	; 0x56
     51e:	7818      	ldrb	r0, [r3, #0]
     520:	0021      	movs	r1, r4
     522:	f7ff ff9f 	bl	464 <FLASH_RxCmd>
     526:	782a      	ldrb	r2, [r5, #0]
     528:	693b      	ldr	r3, [r7, #16]
     52a:	e7ea      	b.n	502 <FLASH_WaitBusy+0x16>
     52c:	2c03      	cmp	r4, #3
     52e:	d1e9      	bne.n	504 <FLASH_WaitBusy+0x18>
     530:	4f08      	ldr	r7, [pc, #32]	; (554 <FLASH_WaitBusy+0x68>)
     532:	002a      	movs	r2, r5
     534:	003b      	movs	r3, r7
     536:	3356      	adds	r3, #86	; 0x56
     538:	2101      	movs	r1, #1
     53a:	7818      	ldrb	r0, [r3, #0]
     53c:	f7ff ff92 	bl	464 <FLASH_RxCmd>
     540:	782a      	ldrb	r2, [r5, #0]
     542:	6979      	ldr	r1, [r7, #20]
     544:	2301      	movs	r3, #1
     546:	4211      	tst	r1, r2
     548:	d0dc      	beq.n	504 <FLASH_WaitBusy+0x18>
     54a:	693b      	ldr	r3, [r7, #16]
     54c:	4013      	ands	r3, r2
     54e:	1e5a      	subs	r2, r3, #1
     550:	4193      	sbcs	r3, r2
     552:	e7d7      	b.n	504 <FLASH_WaitBusy+0x18>
     554:	23000258 	.word	0x23000258

00000558 <FLASH_UserModeEn>:
     558:	b510      	push	{r4, lr}
     55a:	0004      	movs	r4, r0
     55c:	2000      	movs	r0, #0
     55e:	f7ff ffc5 	bl	4ec <FLASH_WaitBusy>
     562:	2288      	movs	r2, #136	; 0x88
     564:	05d2      	lsls	r2, r2, #23
     566:	b12c      	cbz	r4, 574 <FLASH_UserModeEn+0x1c>
     568:	2380      	movs	r3, #128	; 0x80
     56a:	6811      	ldr	r1, [r2, #0]
     56c:	061b      	lsls	r3, r3, #24
     56e:	430b      	orrs	r3, r1
     570:	6013      	str	r3, [r2, #0]
     572:	bd10      	pop	{r4, pc}
     574:	6813      	ldr	r3, [r2, #0]
     576:	005b      	lsls	r3, r3, #1
     578:	085b      	lsrs	r3, r3, #1
     57a:	e7f9      	b.n	570 <FLASH_UserModeEn+0x18>

0000057c <FLASH_RxData>:
     57c:	b5f0      	push	{r4, r5, r6, r7, lr}
     57e:	b085      	sub	sp, #20
     580:	0005      	movs	r5, r0
     582:	000c      	movs	r4, r1
     584:	4e2d      	ldr	r6, [pc, #180]	; (63c <FLASH_RxData+0xc0>)
     586:	9202      	str	r2, [sp, #8]
     588:	9303      	str	r3, [sp, #12]
     58a:	b908      	cbnz	r0, 590 <FLASH_RxData+0x14>
     58c:	68b3      	ldr	r3, [r6, #8]
     58e:	b2dd      	uxtb	r5, r3
     590:	2001      	movs	r0, #1
     592:	f7ff ffe1 	bl	558 <FLASH_UserModeEn>
     596:	2188      	movs	r1, #136	; 0x88
     598:	f240 3300 	movw	r3, #768	; 0x300
     59c:	05c9      	lsls	r1, r1, #23
     59e:	680a      	ldr	r2, [r1, #0]
     5a0:	4827      	ldr	r0, [pc, #156]	; (640 <FLASH_RxData+0xc4>)
     5a2:	4313      	orrs	r3, r2
     5a4:	600b      	str	r3, [r1, #0]
     5a6:	9b02      	ldr	r3, [sp, #8]
     5a8:	021b      	lsls	r3, r3, #8
     5aa:	0a1b      	lsrs	r3, r3, #8
     5ac:	604b      	str	r3, [r1, #4]
     5ae:	2300      	movs	r3, #0
     5b0:	6343      	str	r3, [r0, #52]	; 0x34
     5b2:	69c2      	ldr	r2, [r0, #28]
     5b4:	4b23      	ldr	r3, [pc, #140]	; (644 <FLASH_RxData+0xc8>)
     5b6:	4013      	ands	r3, r2
     5b8:	0032      	movs	r2, r6
     5ba:	9301      	str	r3, [sp, #4]
     5bc:	3263      	adds	r2, #99	; 0x63
     5be:	7817      	ldrb	r7, [r2, #0]
     5c0:	0032      	movs	r2, r6
     5c2:	23f0      	movs	r3, #240	; 0xf0
     5c4:	3262      	adds	r2, #98	; 0x62
     5c6:	7812      	ldrb	r2, [r2, #0]
     5c8:	031b      	lsls	r3, r3, #12
     5ca:	2f00      	cmp	r7, #0
     5cc:	d02c      	beq.n	628 <FLASH_RxData+0xac>
     5ce:	3201      	adds	r2, #1
     5d0:	0412      	lsls	r2, r2, #16
     5d2:	401a      	ands	r2, r3
     5d4:	9b01      	ldr	r3, [sp, #4]
     5d6:	3662      	adds	r6, #98	; 0x62
     5d8:	4313      	orrs	r3, r2
     5da:	61c3      	str	r3, [r0, #28]
     5dc:	2301      	movs	r3, #1
     5de:	481a      	ldr	r0, [pc, #104]	; (648 <FLASH_RxData+0xcc>)
     5e0:	610b      	str	r3, [r1, #16]
     5e2:	7005      	strb	r5, [r0, #0]
     5e4:	7833      	ldrb	r3, [r6, #0]
     5e6:	2b04      	cmp	r3, #4
     5e8:	d101      	bne.n	5ee <FLASH_RxData+0x72>
     5ea:	0e23      	lsrs	r3, r4, #24
     5ec:	7003      	strb	r3, [r0, #0]
     5ee:	0c23      	lsrs	r3, r4, #16
     5f0:	b2db      	uxtb	r3, r3
     5f2:	7003      	strb	r3, [r0, #0]
     5f4:	0a23      	lsrs	r3, r4, #8
     5f6:	b2db      	uxtb	r3, r3
     5f8:	b2e4      	uxtb	r4, r4
     5fa:	7003      	strb	r3, [r0, #0]
     5fc:	7004      	strb	r4, [r0, #0]
     5fe:	b10f      	cbz	r7, 604 <FLASH_RxData+0x88>
     600:	2300      	movs	r3, #0
     602:	7003      	strb	r3, [r0, #0]
     604:	2188      	movs	r1, #136	; 0x88
     606:	2301      	movs	r3, #1
     608:	2408      	movs	r4, #8
     60a:	05c9      	lsls	r1, r1, #23
     60c:	9a02      	ldr	r2, [sp, #8]
     60e:	608b      	str	r3, [r1, #8]
     610:	9b03      	ldr	r3, [sp, #12]
     612:	189a      	adds	r2, r3, r2
     614:	4293      	cmp	r3, r2
     616:	d109      	bne.n	62c <FLASH_RxData+0xb0>
     618:	2004      	movs	r0, #4
     61a:	f7ff ff67 	bl	4ec <FLASH_WaitBusy>
     61e:	2000      	movs	r0, #0
     620:	f7ff ff9a 	bl	558 <FLASH_UserModeEn>
     624:	b005      	add	sp, #20
     626:	bdf0      	pop	{r4, r5, r6, r7, pc}
     628:	0412      	lsls	r2, r2, #16
     62a:	e7d2      	b.n	5d2 <FLASH_RxData+0x56>
     62c:	6a8d      	ldr	r5, [r1, #40]	; 0x28
     62e:	4225      	tst	r5, r4
     630:	d0f0      	beq.n	614 <FLASH_RxData+0x98>
     632:	7805      	ldrb	r5, [r0, #0]
     634:	701d      	strb	r5, [r3, #0]
     636:	3301      	adds	r3, #1
     638:	e7ec      	b.n	614 <FLASH_RxData+0x98>
     63a:	46c0      	nop			; (mov r8, r8)
     63c:	23000258 	.word	0x23000258
     640:	440000fc 	.word	0x440000fc
     644:	fff0ffff 	.word	0xfff0ffff
     648:	44000060 	.word	0x44000060

0000064c <FLASH_TxCmd>:
     64c:	b5f0      	push	{r4, r5, r6, r7, lr}
     64e:	2488      	movs	r4, #136	; 0x88
     650:	4e16      	ldr	r6, [pc, #88]	; (6ac <FLASH_TxCmd+0x60>)
     652:	b085      	sub	sp, #20
     654:	0007      	movs	r7, r0
     656:	2001      	movs	r0, #1
     658:	000d      	movs	r5, r1
     65a:	9203      	str	r2, [sp, #12]
     65c:	f7ff ff7c 	bl	558 <FLASH_UserModeEn>
     660:	69f3      	ldr	r3, [r6, #28]
     662:	05e4      	lsls	r4, r4, #23
     664:	9301      	str	r3, [sp, #4]
     666:	6823      	ldr	r3, [r4, #0]
     668:	4a11      	ldr	r2, [pc, #68]	; (6b0 <FLASH_TxCmd+0x64>)
     66a:	9302      	str	r3, [sp, #8]
     66c:	6823      	ldr	r3, [r4, #0]
     66e:	4013      	ands	r3, r2
     670:	6023      	str	r3, [r4, #0]
     672:	69f3      	ldr	r3, [r6, #28]
     674:	4a0f      	ldr	r2, [pc, #60]	; (6b4 <FLASH_TxCmd+0x68>)
     676:	4013      	ands	r3, r2
     678:	61f3      	str	r3, [r6, #28]
     67a:	2300      	movs	r3, #0
     67c:	9a03      	ldr	r2, [sp, #12]
     67e:	6063      	str	r3, [r4, #4]
     680:	4b0d      	ldr	r3, [pc, #52]	; (6b8 <FLASH_TxCmd+0x6c>)
     682:	6375      	str	r5, [r6, #52]	; 0x34
     684:	701f      	strb	r7, [r3, #0]
     686:	1955      	adds	r5, r2, r5
     688:	42aa      	cmp	r2, r5
     68a:	d10a      	bne.n	6a2 <FLASH_TxCmd+0x56>
     68c:	2301      	movs	r3, #1
     68e:	2004      	movs	r0, #4
     690:	60a3      	str	r3, [r4, #8]
     692:	f7ff ff2b 	bl	4ec <FLASH_WaitBusy>
     696:	9b01      	ldr	r3, [sp, #4]
     698:	61f3      	str	r3, [r6, #28]
     69a:	9b02      	ldr	r3, [sp, #8]
     69c:	6023      	str	r3, [r4, #0]
     69e:	b005      	add	sp, #20
     6a0:	bdf0      	pop	{r4, r5, r6, r7, pc}
     6a2:	7811      	ldrb	r1, [r2, #0]
     6a4:	3201      	adds	r2, #1
     6a6:	7019      	strb	r1, [r3, #0]
     6a8:	e7ee      	b.n	688 <FLASH_TxCmd+0x3c>
     6aa:	46c0      	nop			; (mov r8, r8)
     6ac:	440000fc 	.word	0x440000fc
     6b0:	ffc0fcff 	.word	0xffc0fcff
     6b4:	fff0ffff 	.word	0xfff0ffff
     6b8:	44000060 	.word	0x44000060

000006bc <FLASH_WriteEn>:
     6bc:	b510      	push	{r4, lr}
     6be:	2001      	movs	r0, #1
     6c0:	f7ff ff14 	bl	4ec <FLASH_WaitBusy>
     6c4:	2200      	movs	r2, #0
     6c6:	4b05      	ldr	r3, [pc, #20]	; (6dc <FLASH_WriteEn+0x20>)
     6c8:	0011      	movs	r1, r2
     6ca:	3354      	adds	r3, #84	; 0x54
     6cc:	7818      	ldrb	r0, [r3, #0]
     6ce:	f7ff ffbd 	bl	64c <FLASH_TxCmd>
     6d2:	2003      	movs	r0, #3
     6d4:	f7ff ff0a 	bl	4ec <FLASH_WaitBusy>
     6d8:	bd10      	pop	{r4, pc}
     6da:	46c0      	nop			; (mov r8, r8)
     6dc:	23000258 	.word	0x23000258

000006e0 <FLASH_TxData>:
     6e0:	b5f0      	push	{r4, r5, r6, r7, lr}
     6e2:	0016      	movs	r6, r2
     6e4:	f240 1200 	movw	r2, #256	; 0x100
     6e8:	b2c3      	uxtb	r3, r0
     6ea:	185b      	adds	r3, r3, r1
     6ec:	0004      	movs	r4, r0
     6ee:	000d      	movs	r5, r1
     6f0:	b085      	sub	sp, #20
     6f2:	4293      	cmp	r3, r2
     6f4:	d904      	bls.n	700 <FLASH_TxData+0x20>
     6f6:	f240 1159 	movw	r1, #345	; 0x159
     6fa:	482e      	ldr	r0, [pc, #184]	; (7b4 <FLASH_TxData+0xd4>)
     6fc:	f7ff fd0a 	bl	114 <io_assert_failed>
     700:	f7ff ffdc 	bl	6bc <FLASH_WriteEn>
     704:	2001      	movs	r0, #1
     706:	f7ff ff27 	bl	558 <FLASH_UserModeEn>
     70a:	2288      	movs	r2, #136	; 0x88
     70c:	05d2      	lsls	r2, r2, #23
     70e:	6813      	ldr	r3, [r2, #0]
     710:	4929      	ldr	r1, [pc, #164]	; (7b8 <FLASH_TxData+0xd8>)
     712:	9302      	str	r3, [sp, #8]
     714:	6813      	ldr	r3, [r2, #0]
     716:	400b      	ands	r3, r1
     718:	4928      	ldr	r1, [pc, #160]	; (7bc <FLASH_TxData+0xdc>)
     71a:	6013      	str	r3, [r2, #0]
     71c:	69cb      	ldr	r3, [r1, #28]
     71e:	9301      	str	r3, [sp, #4]
     720:	4b27      	ldr	r3, [pc, #156]	; (7c0 <FLASH_TxData+0xe0>)
     722:	3362      	adds	r3, #98	; 0x62
     724:	781f      	ldrb	r7, [r3, #0]
     726:	043b      	lsls	r3, r7, #16
     728:	469c      	mov	ip, r3
     72a:	23f0      	movs	r3, #240	; 0xf0
     72c:	4660      	mov	r0, ip
     72e:	031b      	lsls	r3, r3, #12
     730:	4003      	ands	r3, r0
     732:	9303      	str	r3, [sp, #12]
     734:	4823      	ldr	r0, [pc, #140]	; (7c4 <FLASH_TxData+0xe4>)
     736:	9b01      	ldr	r3, [sp, #4]
     738:	4003      	ands	r3, r0
     73a:	0018      	movs	r0, r3
     73c:	9b03      	ldr	r3, [sp, #12]
     73e:	4303      	orrs	r3, r0
     740:	61cb      	str	r3, [r1, #28]
     742:	2300      	movs	r3, #0
     744:	6053      	str	r3, [r2, #4]
     746:	022b      	lsls	r3, r5, #8
     748:	0a1b      	lsrs	r3, r3, #8
     74a:	634b      	str	r3, [r1, #52]	; 0x34
     74c:	2302      	movs	r3, #2
     74e:	4a1e      	ldr	r2, [pc, #120]	; (7c8 <FLASH_TxData+0xe8>)
     750:	7013      	strb	r3, [r2, #0]
     752:	2f04      	cmp	r7, #4
     754:	d101      	bne.n	75a <FLASH_TxData+0x7a>
     756:	0e23      	lsrs	r3, r4, #24
     758:	7013      	strb	r3, [r2, #0]
     75a:	0c23      	lsrs	r3, r4, #16
     75c:	b2db      	uxtb	r3, r3
     75e:	7013      	strb	r3, [r2, #0]
     760:	0a23      	lsrs	r3, r4, #8
     762:	b2db      	uxtb	r3, r3
     764:	2188      	movs	r1, #136	; 0x88
     766:	7013      	strb	r3, [r2, #0]
     768:	2002      	movs	r0, #2
     76a:	2300      	movs	r3, #0
     76c:	b2e4      	uxtb	r4, r4
     76e:	7014      	strb	r4, [r2, #0]
     770:	05c9      	lsls	r1, r1, #23
     772:	42ab      	cmp	r3, r5
     774:	d002      	beq.n	77c <FLASH_TxData+0x9c>
     776:	6a8c      	ldr	r4, [r1, #40]	; 0x28
     778:	4204      	tst	r4, r0
     77a:	d110      	bne.n	79e <FLASH_TxData+0xbe>
     77c:	2001      	movs	r0, #1
     77e:	2488      	movs	r4, #136	; 0x88
     780:	6088      	str	r0, [r1, #8]
     782:	2102      	movs	r1, #2
     784:	05e4      	lsls	r4, r4, #23
     786:	42ab      	cmp	r3, r5
     788:	d30d      	bcc.n	7a6 <FLASH_TxData+0xc6>
     78a:	2004      	movs	r0, #4
     78c:	f7ff feae 	bl	4ec <FLASH_WaitBusy>
     790:	9b02      	ldr	r3, [sp, #8]
     792:	2001      	movs	r0, #1
     794:	6023      	str	r3, [r4, #0]
     796:	f7ff fea9 	bl	4ec <FLASH_WaitBusy>
     79a:	b005      	add	sp, #20
     79c:	bdf0      	pop	{r4, r5, r6, r7, pc}
     79e:	5cf4      	ldrb	r4, [r6, r3]
     7a0:	3301      	adds	r3, #1
     7a2:	7014      	strb	r4, [r2, #0]
     7a4:	e7e5      	b.n	772 <FLASH_TxData+0x92>
     7a6:	6aa0      	ldr	r0, [r4, #40]	; 0x28
     7a8:	4208      	tst	r0, r1
     7aa:	d0ec      	beq.n	786 <FLASH_TxData+0xa6>
     7ac:	5cf0      	ldrb	r0, [r6, r3]
     7ae:	3301      	adds	r3, #1
     7b0:	7010      	strb	r0, [r2, #0]
     7b2:	e7e8      	b.n	786 <FLASH_TxData+0xa6>
     7b4:	0000c391 	.word	0x0000c391
     7b8:	ffc0fcff 	.word	0xffc0fcff
     7bc:	440000fc 	.word	0x440000fc
     7c0:	23000258 	.word	0x23000258
     7c4:	fff0ffff 	.word	0xfff0ffff
     7c8:	44000060 	.word	0x44000060

000007cc <FLASH_SetStatus>:
     7cc:	b570      	push	{r4, r5, r6, lr}
     7ce:	0005      	movs	r5, r0
     7d0:	0016      	movs	r6, r2
     7d2:	000c      	movs	r4, r1
     7d4:	f7ff ff72 	bl	6bc <FLASH_WriteEn>
     7d8:	0032      	movs	r2, r6
     7da:	0028      	movs	r0, r5
     7dc:	b2e1      	uxtb	r1, r4
     7de:	f7ff ff35 	bl	64c <FLASH_TxCmd>
     7e2:	2001      	movs	r0, #1
     7e4:	f7ff fe82 	bl	4ec <FLASH_WaitBusy>
     7e8:	bd70      	pop	{r4, r5, r6, pc}

000007ea <FLASH_Erase>:
     7ea:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
     7ec:	0005      	movs	r5, r0
     7ee:	000e      	movs	r6, r1
     7f0:	2802      	cmp	r0, #2
     7f2:	d904      	bls.n	7fe <FLASH_Erase+0x14>
     7f4:	f240 11eb 	movw	r1, #491	; 0x1eb
     7f8:	4819      	ldr	r0, [pc, #100]	; (860 <FLASH_Erase+0x76>)
     7fa:	f7ff fc8b 	bl	114 <io_assert_failed>
     7fe:	4b19      	ldr	r3, [pc, #100]	; (864 <FLASH_Erase+0x7a>)
     800:	0c32      	lsrs	r2, r6, #16
     802:	3362      	adds	r3, #98	; 0x62
     804:	781f      	ldrb	r7, [r3, #0]
     806:	0a33      	lsrs	r3, r6, #8
     808:	b2d2      	uxtb	r2, r2
     80a:	b2db      	uxtb	r3, r3
     80c:	b2f1      	uxtb	r1, r6
     80e:	ac01      	add	r4, sp, #4
     810:	2f04      	cmp	r7, #4
     812:	d112      	bne.n	83a <FLASH_Erase+0x50>
     814:	0e36      	lsrs	r6, r6, #24
     816:	7026      	strb	r6, [r4, #0]
     818:	7062      	strb	r2, [r4, #1]
     81a:	70a3      	strb	r3, [r4, #2]
     81c:	70e1      	strb	r1, [r4, #3]
     81e:	f7ff ff4d 	bl	6bc <FLASH_WriteEn>
     822:	b97d      	cbnz	r5, 844 <FLASH_Erase+0x5a>
     824:	002a      	movs	r2, r5
     826:	0029      	movs	r1, r5
     828:	4b0e      	ldr	r3, [pc, #56]	; (864 <FLASH_Erase+0x7a>)
     82a:	335a      	adds	r3, #90	; 0x5a
     82c:	7818      	ldrb	r0, [r3, #0]
     82e:	f7ff ff0d 	bl	64c <FLASH_TxCmd>
     832:	2001      	movs	r0, #1
     834:	f7ff fe5a 	bl	4ec <FLASH_WaitBusy>
     838:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}
     83a:	2703      	movs	r7, #3
     83c:	7022      	strb	r2, [r4, #0]
     83e:	7063      	strb	r3, [r4, #1]
     840:	70a1      	strb	r1, [r4, #2]
     842:	e7ec      	b.n	81e <FLASH_Erase+0x34>
     844:	2d01      	cmp	r5, #1
     846:	d105      	bne.n	854 <FLASH_Erase+0x6a>
     848:	4b06      	ldr	r3, [pc, #24]	; (864 <FLASH_Erase+0x7a>)
     84a:	335b      	adds	r3, #91	; 0x5b
     84c:	0022      	movs	r2, r4
     84e:	0039      	movs	r1, r7
     850:	7818      	ldrb	r0, [r3, #0]
     852:	e7ec      	b.n	82e <FLASH_Erase+0x44>
     854:	2d02      	cmp	r5, #2
     856:	d1ec      	bne.n	832 <FLASH_Erase+0x48>
     858:	4b02      	ldr	r3, [pc, #8]	; (864 <FLASH_Erase+0x7a>)
     85a:	335c      	adds	r3, #92	; 0x5c
     85c:	e7f6      	b.n	84c <FLASH_Erase+0x62>
     85e:	46c0      	nop			; (mov r8, r8)
     860:	0000c385 	.word	0x0000c385
     864:	23000258 	.word	0x23000258

00000868 <FLASH_DeepPowerDown>:
     868:	b510      	push	{r4, lr}
     86a:	2801      	cmp	r0, #1
     86c:	d10f      	bne.n	88e <FLASH_DeepPowerDown+0x26>
     86e:	4b0f      	ldr	r3, [pc, #60]	; (8ac <FLASH_DeepPowerDown+0x44>)
     870:	4798      	blx	r3
     872:	b158      	cbz	r0, 88c <FLASH_DeepPowerDown+0x24>
     874:	f7ff ff22 	bl	6bc <FLASH_WriteEn>
     878:	2200      	movs	r2, #0
     87a:	4b0d      	ldr	r3, [pc, #52]	; (8b0 <FLASH_DeepPowerDown+0x48>)
     87c:	0011      	movs	r1, r2
     87e:	335e      	adds	r3, #94	; 0x5e
     880:	7818      	ldrb	r0, [r3, #0]
     882:	f7ff fee3 	bl	64c <FLASH_TxCmd>
     886:	2000      	movs	r0, #0
     888:	f7ff fe66 	bl	558 <FLASH_UserModeEn>
     88c:	bd10      	pop	{r4, pc}
     88e:	2200      	movs	r2, #0
     890:	4b07      	ldr	r3, [pc, #28]	; (8b0 <FLASH_DeepPowerDown+0x48>)
     892:	0011      	movs	r1, r2
     894:	335d      	adds	r3, #93	; 0x5d
     896:	7818      	ldrb	r0, [r3, #0]
     898:	f7ff fed8 	bl	64c <FLASH_TxCmd>
     89c:	2064      	movs	r0, #100	; 0x64
     89e:	4b05      	ldr	r3, [pc, #20]	; (8b4 <FLASH_DeepPowerDown+0x4c>)
     8a0:	4798      	blx	r3
     8a2:	2001      	movs	r0, #1
     8a4:	f7ff fe22 	bl	4ec <FLASH_WaitBusy>
     8a8:	e7f0      	b.n	88c <FLASH_DeepPowerDown+0x24>
     8aa:	46c0      	nop			; (mov r8, r8)
     8ac:	00001ead 	.word	0x00001ead
     8b0:	23000258 	.word	0x23000258
     8b4:	0000030b 	.word	0x0000030b

000008b8 <FLASH_SetStatusBits>:
     8b8:	b5f0      	push	{r4, r5, r6, r7, lr}
     8ba:	4d26      	ldr	r5, [pc, #152]	; (954 <FLASH_SetStatusBits+0x9c>)
     8bc:	b085      	sub	sp, #20
     8be:	002b      	movs	r3, r5
     8c0:	9101      	str	r1, [sp, #4]
     8c2:	3356      	adds	r3, #86	; 0x56
     8c4:	ac03      	add	r4, sp, #12
     8c6:	0006      	movs	r6, r0
     8c8:	0022      	movs	r2, r4
     8ca:	2101      	movs	r1, #1
     8cc:	7818      	ldrb	r0, [r3, #0]
     8ce:	f7ff fdc9 	bl	464 <FLASH_RxCmd>
     8d2:	7823      	ldrb	r3, [r4, #0]
     8d4:	9901      	ldr	r1, [sp, #4]
     8d6:	001a      	movs	r2, r3
     8d8:	43b2      	bics	r2, r6
     8da:	2901      	cmp	r1, #1
     8dc:	d101      	bne.n	8e2 <FLASH_SetStatusBits+0x2a>
     8de:	4333      	orrs	r3, r6
     8e0:	b2da      	uxtb	r2, r3
     8e2:	69ab      	ldr	r3, [r5, #24]
     8e4:	2701      	movs	r7, #1
     8e6:	7022      	strb	r2, [r4, #0]
     8e8:	b17b      	cbz	r3, 90a <FLASH_SetStatusBits+0x52>
     8ea:	002b      	movs	r3, r5
     8ec:	3357      	adds	r3, #87	; 0x57
     8ee:	7818      	ldrb	r0, [r3, #0]
     8f0:	ab02      	add	r3, sp, #8
     8f2:	1d5a      	adds	r2, r3, #5
     8f4:	0039      	movs	r1, r7
     8f6:	f7ff fdb5 	bl	464 <FLASH_RxCmd>
     8fa:	9b01      	ldr	r3, [sp, #4]
     8fc:	7860      	ldrb	r0, [r4, #1]
     8fe:	0a36      	lsrs	r6, r6, #8
     900:	42bb      	cmp	r3, r7
     902:	d113      	bne.n	92c <FLASH_SetStatusBits+0x74>
     904:	4306      	orrs	r6, r0
     906:	7066      	strb	r6, [r4, #1]
     908:	2702      	movs	r7, #2
     90a:	002e      	movs	r6, r5
     90c:	3659      	adds	r6, #89	; 0x59
     90e:	7833      	ldrb	r3, [r6, #0]
     910:	3558      	adds	r5, #88	; 0x58
     912:	b973      	cbnz	r3, 932 <FLASH_SetStatusBits+0x7a>
     914:	f7ff fed2 	bl	6bc <FLASH_WriteEn>
     918:	0022      	movs	r2, r4
     91a:	0039      	movs	r1, r7
     91c:	7828      	ldrb	r0, [r5, #0]
     91e:	f7ff fe95 	bl	64c <FLASH_TxCmd>
     922:	2001      	movs	r0, #1
     924:	f7ff fde2 	bl	4ec <FLASH_WaitBusy>
     928:	b005      	add	sp, #20
     92a:	bdf0      	pop	{r4, r5, r6, r7, pc}
     92c:	43b0      	bics	r0, r6
     92e:	7060      	strb	r0, [r4, #1]
     930:	e7ea      	b.n	908 <FLASH_SetStatusBits+0x50>
     932:	f7ff fec3 	bl	6bc <FLASH_WriteEn>
     936:	0022      	movs	r2, r4
     938:	2101      	movs	r1, #1
     93a:	7828      	ldrb	r0, [r5, #0]
     93c:	f7ff fe86 	bl	64c <FLASH_TxCmd>
     940:	2001      	movs	r0, #1
     942:	f7ff fdd3 	bl	4ec <FLASH_WaitBusy>
     946:	f7ff feb9 	bl	6bc <FLASH_WriteEn>
     94a:	ab02      	add	r3, sp, #8
     94c:	2101      	movs	r1, #1
     94e:	7830      	ldrb	r0, [r6, #0]
     950:	1d5a      	adds	r2, r3, #5
     952:	e7e4      	b.n	91e <FLASH_SetStatusBits+0x66>
     954:	23000258 	.word	0x23000258

00000958 <FLASH_StructInit_Micron>:
     958:	f240 5100 	movw	r1, #1280	; 0x500
     95c:	b530      	push	{r4, r5, lr}
     95e:	83c1      	strh	r1, [r0, #30]
     960:	2403      	movs	r4, #3
     962:	2201      	movs	r2, #1
     964:	f640 1105 	movw	r1, #2309	; 0x905
     968:	2300      	movs	r3, #0
     96a:	2509      	movs	r5, #9
     96c:	7142      	strb	r2, [r0, #5]
     96e:	6102      	str	r2, [r0, #16]
     970:	8401      	strh	r1, [r0, #32]
     972:	1892      	adds	r2, r2, r2
     974:	1901      	adds	r1, r0, r4
     976:	60c3      	str	r3, [r0, #12]
     978:	6183      	str	r3, [r0, #24]
     97a:	7743      	strb	r3, [r0, #29]
     97c:	6004      	str	r4, [r0, #0]
     97e:	6142      	str	r2, [r0, #20]
     980:	77cd      	strb	r5, [r1, #31]
     982:	0005      	movs	r5, r0
     984:	21bb      	movs	r1, #187	; 0xbb
     986:	3561      	adds	r5, #97	; 0x61
     988:	6681      	str	r1, [r0, #104]	; 0x68
     98a:	702b      	strb	r3, [r5, #0]
     98c:	0003      	movs	r3, r0
     98e:	3362      	adds	r3, #98	; 0x62
     990:	801c      	strh	r4, [r3, #0]
     992:	233b      	movs	r3, #59	; 0x3b
     994:	6243      	str	r3, [r0, #36]	; 0x24
     996:	3330      	adds	r3, #48	; 0x30
     998:	62c3      	str	r3, [r0, #44]	; 0x2c
     99a:	3380      	adds	r3, #128	; 0x80
     99c:	6303      	str	r3, [r0, #48]	; 0x30
     99e:	3b49      	subs	r3, #73	; 0x49
     9a0:	6343      	str	r3, [r0, #52]	; 0x34
     9a2:	3330      	adds	r3, #48	; 0x30
     9a4:	6383      	str	r3, [r0, #56]	; 0x38
     9a6:	3ba0      	subs	r3, #160	; 0xa0
     9a8:	63c3      	str	r3, [r0, #60]	; 0x3c
     9aa:	3b20      	subs	r3, #32
     9ac:	6403      	str	r3, [r0, #64]	; 0x40
     9ae:	3b0e      	subs	r3, #14
     9b0:	6483      	str	r3, [r0, #72]	; 0x48
     9b2:	3304      	adds	r3, #4
     9b4:	64c3      	str	r3, [r0, #76]	; 0x4c
     9b6:	3308      	adds	r3, #8
     9b8:	6503      	str	r3, [r0, #80]	; 0x50
     9ba:	4b04      	ldr	r3, [pc, #16]	; (9cc <FLASH_StructInit_Micron+0x74>)
     9bc:	6281      	str	r1, [r0, #40]	; 0x28
     9be:	6543      	str	r3, [r0, #84]	; 0x54
     9c0:	4b03      	ldr	r3, [pc, #12]	; (9d0 <FLASH_StructInit_Micron+0x78>)
     9c2:	6442      	str	r2, [r0, #68]	; 0x44
     9c4:	6583      	str	r3, [r0, #88]	; 0x58
     9c6:	4b03      	ldr	r3, [pc, #12]	; (9d4 <FLASH_StructInit_Micron+0x7c>)
     9c8:	65c3      	str	r3, [r0, #92]	; 0x5c
     9ca:	bd30      	pop	{r4, r5, pc}
     9cc:	00059f06 	.word	0x00059f06
     9d0:	d8c70001 	.word	0xd8c70001
     9d4:	00b9ab20 	.word	0x00b9ab20

000009d8 <FLASH_StructInit_MXIC>:
     9d8:	2301      	movs	r3, #1
     9da:	f640 0100 	movw	r1, #2048	; 0x800
     9de:	2240      	movs	r2, #64	; 0x40
     9e0:	b530      	push	{r4, r5, lr}
     9e2:	6003      	str	r3, [r0, #0]
     9e4:	7143      	strb	r3, [r0, #5]
     9e6:	6103      	str	r3, [r0, #16]
     9e8:	2406      	movs	r4, #6
     9ea:	2300      	movs	r3, #0
     9ec:	83c1      	strh	r1, [r0, #30]
     9ee:	3104      	adds	r1, #4
     9f0:	60c2      	str	r2, [r0, #12]
     9f2:	8401      	strh	r1, [r0, #32]
     9f4:	3a3e      	subs	r2, #62	; 0x3e
     9f6:	1cc1      	adds	r1, r0, #3
     9f8:	6183      	str	r3, [r0, #24]
     9fa:	7743      	strb	r3, [r0, #29]
     9fc:	6142      	str	r2, [r0, #20]
     9fe:	77cc      	strb	r4, [r1, #31]
     a00:	0004      	movs	r4, r0
     a02:	21bb      	movs	r1, #187	; 0xbb
     a04:	2503      	movs	r5, #3
     a06:	3461      	adds	r4, #97	; 0x61
     a08:	6681      	str	r1, [r0, #104]	; 0x68
     a0a:	7023      	strb	r3, [r4, #0]
     a0c:	3401      	adds	r4, #1
     a0e:	8025      	strh	r5, [r4, #0]
     a10:	243b      	movs	r4, #59	; 0x3b
     a12:	6343      	str	r3, [r0, #52]	; 0x34
     a14:	6383      	str	r3, [r0, #56]	; 0x38
     a16:	63c3      	str	r3, [r0, #60]	; 0x3c
     a18:	3338      	adds	r3, #56	; 0x38
     a1a:	6403      	str	r3, [r0, #64]	; 0x40
     a1c:	3b34      	subs	r3, #52	; 0x34
     a1e:	6483      	str	r3, [r0, #72]	; 0x48
     a20:	3304      	adds	r3, #4
     a22:	64c3      	str	r3, [r0, #76]	; 0x4c
     a24:	3308      	adds	r3, #8
     a26:	6503      	str	r3, [r0, #80]	; 0x50
     a28:	4b06      	ldr	r3, [pc, #24]	; (a44 <FLASH_StructInit_MXIC+0x6c>)
     a2a:	6281      	str	r1, [r0, #40]	; 0x28
     a2c:	6543      	str	r3, [r0, #84]	; 0x54
     a2e:	4b06      	ldr	r3, [pc, #24]	; (a48 <FLASH_StructInit_MXIC+0x70>)
     a30:	3950      	subs	r1, #80	; 0x50
     a32:	6583      	str	r3, [r0, #88]	; 0x58
     a34:	4b05      	ldr	r3, [pc, #20]	; (a4c <FLASH_StructInit_MXIC+0x74>)
     a36:	62c1      	str	r1, [r0, #44]	; 0x2c
     a38:	3180      	adds	r1, #128	; 0x80
     a3a:	6244      	str	r4, [r0, #36]	; 0x24
     a3c:	6301      	str	r1, [r0, #48]	; 0x30
     a3e:	6442      	str	r2, [r0, #68]	; 0x44
     a40:	65c3      	str	r3, [r0, #92]	; 0x5c
     a42:	bd30      	pop	{r4, r5, pc}
     a44:	00059f06 	.word	0x00059f06
     a48:	d8600001 	.word	0xd8600001
     a4c:	00b9ab20 	.word	0x00b9ab20

00000a50 <FLASH_StructInit_GD>:
     a50:	2305      	movs	r3, #5
     a52:	f240 2200 	movw	r2, #512	; 0x200
     a56:	f640 0100 	movw	r1, #2048	; 0x800
     a5a:	6003      	str	r3, [r0, #0]
     a5c:	3b04      	subs	r3, #4
     a5e:	b530      	push	{r4, r5, lr}
     a60:	7143      	strb	r3, [r0, #5]
     a62:	6103      	str	r3, [r0, #16]
     a64:	6183      	str	r3, [r0, #24]
     a66:	2406      	movs	r4, #6
     a68:	2300      	movs	r3, #0
     a6a:	60c2      	str	r2, [r0, #12]
     a6c:	83c1      	strh	r1, [r0, #30]
     a6e:	3aff      	subs	r2, #255	; 0xff
     a70:	3104      	adds	r1, #4
     a72:	8401      	strh	r1, [r0, #32]
     a74:	3aff      	subs	r2, #255	; 0xff
     a76:	1cc1      	adds	r1, r0, #3
     a78:	7743      	strb	r3, [r0, #29]
     a7a:	6142      	str	r2, [r0, #20]
     a7c:	77cc      	strb	r4, [r1, #31]
     a7e:	0004      	movs	r4, r0
     a80:	21bb      	movs	r1, #187	; 0xbb
     a82:	2503      	movs	r5, #3
     a84:	3461      	adds	r4, #97	; 0x61
     a86:	6681      	str	r1, [r0, #104]	; 0x68
     a88:	7023      	strb	r3, [r4, #0]
     a8a:	3401      	adds	r4, #1
     a8c:	8025      	strh	r5, [r4, #0]
     a8e:	243b      	movs	r4, #59	; 0x3b
     a90:	6343      	str	r3, [r0, #52]	; 0x34
     a92:	6383      	str	r3, [r0, #56]	; 0x38
     a94:	63c3      	str	r3, [r0, #60]	; 0x3c
     a96:	3332      	adds	r3, #50	; 0x32
     a98:	6403      	str	r3, [r0, #64]	; 0x40
     a9a:	3b2e      	subs	r3, #46	; 0x2e
     a9c:	6483      	str	r3, [r0, #72]	; 0x48
     a9e:	3304      	adds	r3, #4
     aa0:	64c3      	str	r3, [r0, #76]	; 0x4c
     aa2:	3308      	adds	r3, #8
     aa4:	6503      	str	r3, [r0, #80]	; 0x50
     aa6:	4b07      	ldr	r3, [pc, #28]	; (ac4 <FLASH_StructInit_GD+0x74>)
     aa8:	6281      	str	r1, [r0, #40]	; 0x28
     aaa:	6543      	str	r3, [r0, #84]	; 0x54
     aac:	4b06      	ldr	r3, [pc, #24]	; (ac8 <FLASH_StructInit_GD+0x78>)
     aae:	3950      	subs	r1, #80	; 0x50
     ab0:	6583      	str	r3, [r0, #88]	; 0x58
     ab2:	4b06      	ldr	r3, [pc, #24]	; (acc <FLASH_StructInit_GD+0x7c>)
     ab4:	62c1      	str	r1, [r0, #44]	; 0x2c
     ab6:	3180      	adds	r1, #128	; 0x80
     ab8:	6244      	str	r4, [r0, #36]	; 0x24
     aba:	6301      	str	r1, [r0, #48]	; 0x30
     abc:	6442      	str	r2, [r0, #68]	; 0x44
     abe:	65c3      	str	r3, [r0, #92]	; 0x5c
     ac0:	bd30      	pop	{r4, r5, pc}
     ac2:	46c0      	nop			; (mov r8, r8)
     ac4:	35059f06 	.word	0x35059f06
     ac8:	d8600001 	.word	0xd8600001
     acc:	00b9ab20 	.word	0x00b9ab20

00000ad0 <FLASH_StructInit>:
     ad0:	f240 2100 	movw	r1, #512	; 0x200
     ad4:	2301      	movs	r3, #1
     ad6:	60c1      	str	r1, [r0, #12]
     ad8:	f640 0100 	movw	r1, #2048	; 0x800
     adc:	b530      	push	{r4, r5, lr}
     ade:	7143      	strb	r3, [r0, #5]
     ae0:	6103      	str	r3, [r0, #16]
     ae2:	6183      	str	r3, [r0, #24]
     ae4:	2202      	movs	r2, #2
     ae6:	2300      	movs	r3, #0
     ae8:	2406      	movs	r4, #6
     aea:	83c1      	strh	r1, [r0, #30]
     aec:	3104      	adds	r1, #4
     aee:	8401      	strh	r1, [r0, #32]
     af0:	1cc1      	adds	r1, r0, #3
     af2:	7743      	strb	r3, [r0, #29]
     af4:	6002      	str	r2, [r0, #0]
     af6:	6142      	str	r2, [r0, #20]
     af8:	77cc      	strb	r4, [r1, #31]
     afa:	0004      	movs	r4, r0
     afc:	21bb      	movs	r1, #187	; 0xbb
     afe:	2503      	movs	r5, #3
     b00:	3461      	adds	r4, #97	; 0x61
     b02:	6681      	str	r1, [r0, #104]	; 0x68
     b04:	7023      	strb	r3, [r4, #0]
     b06:	3401      	adds	r4, #1
     b08:	8025      	strh	r5, [r4, #0]
     b0a:	243b      	movs	r4, #59	; 0x3b
     b0c:	6343      	str	r3, [r0, #52]	; 0x34
     b0e:	6383      	str	r3, [r0, #56]	; 0x38
     b10:	63c3      	str	r3, [r0, #60]	; 0x3c
     b12:	3332      	adds	r3, #50	; 0x32
     b14:	6403      	str	r3, [r0, #64]	; 0x40
     b16:	3b2e      	subs	r3, #46	; 0x2e
     b18:	6483      	str	r3, [r0, #72]	; 0x48
     b1a:	3304      	adds	r3, #4
     b1c:	64c3      	str	r3, [r0, #76]	; 0x4c
     b1e:	3308      	adds	r3, #8
     b20:	6503      	str	r3, [r0, #80]	; 0x50
     b22:	4b07      	ldr	r3, [pc, #28]	; (b40 <FLASH_StructInit+0x70>)
     b24:	6281      	str	r1, [r0, #40]	; 0x28
     b26:	6543      	str	r3, [r0, #84]	; 0x54
     b28:	4b06      	ldr	r3, [pc, #24]	; (b44 <FLASH_StructInit+0x74>)
     b2a:	3950      	subs	r1, #80	; 0x50
     b2c:	6583      	str	r3, [r0, #88]	; 0x58
     b2e:	4b06      	ldr	r3, [pc, #24]	; (b48 <FLASH_StructInit+0x78>)
     b30:	62c1      	str	r1, [r0, #44]	; 0x2c
     b32:	3180      	adds	r1, #128	; 0x80
     b34:	6244      	str	r4, [r0, #36]	; 0x24
     b36:	6301      	str	r1, [r0, #48]	; 0x30
     b38:	6442      	str	r2, [r0, #68]	; 0x44
     b3a:	65c3      	str	r3, [r0, #92]	; 0x5c
     b3c:	bd30      	pop	{r4, r5, pc}
     b3e:	46c0      	nop			; (mov r8, r8)
     b40:	35059f06 	.word	0x35059f06
     b44:	d8600001 	.word	0xd8600001
     b48:	00b9ab20 	.word	0x00b9ab20

00000b4c <FLASH_Init>:
     b4c:	2288      	movs	r2, #136	; 0x88
     b4e:	b570      	push	{r4, r5, r6, lr}
     b50:	2500      	movs	r5, #0
     b52:	05d2      	lsls	r2, r2, #23
     b54:	0001      	movs	r1, r0
     b56:	481f      	ldr	r0, [pc, #124]	; (bd4 <FLASH_Init+0x88>)
     b58:	6095      	str	r5, [r2, #8]
     b5a:	7943      	ldrb	r3, [r0, #5]
     b5c:	2401      	movs	r4, #1
     b5e:	6153      	str	r3, [r2, #20]
     b60:	0013      	movs	r3, r2
     b62:	6114      	str	r4, [r2, #16]
     b64:	6e86      	ldr	r6, [r0, #104]	; 0x68
     b66:	33e0      	adds	r3, #224	; 0xe0
     b68:	601e      	str	r6, [r3, #0]
     b6a:	6a86      	ldr	r6, [r0, #40]	; 0x28
     b6c:	3304      	adds	r3, #4
     b6e:	605e      	str	r6, [r3, #4]
     b70:	6a46      	ldr	r6, [r0, #36]	; 0x24
     b72:	32f8      	adds	r2, #248	; 0xf8
     b74:	601e      	str	r6, [r3, #0]
     b76:	6b06      	ldr	r6, [r0, #48]	; 0x30
     b78:	60de      	str	r6, [r3, #12]
     b7a:	6ac6      	ldr	r6, [r0, #44]	; 0x2c
     b7c:	3308      	adds	r3, #8
     b7e:	601e      	str	r6, [r3, #0]
     b80:	609d      	str	r5, [r3, #8]
     b82:	61dd      	str	r5, [r3, #28]
     b84:	6015      	str	r5, [r2, #0]
     b86:	611d      	str	r5, [r3, #16]
     b88:	615d      	str	r5, [r3, #20]
     b8a:	619d      	str	r5, [r3, #24]
     b8c:	6a5a      	ldr	r2, [r3, #36]	; 0x24
     b8e:	3506      	adds	r5, #6
     b90:	4322      	orrs	r2, r4
     b92:	625a      	str	r2, [r3, #36]	; 0x24
     b94:	6a5a      	ldr	r2, [r3, #36]	; 0x24
     b96:	26f0      	movs	r6, #240	; 0xf0
     b98:	43aa      	bics	r2, r5
     b9a:	625a      	str	r2, [r3, #36]	; 0x24
     b9c:	0002      	movs	r2, r0
     b9e:	3262      	adds	r2, #98	; 0x62
     ba0:	7812      	ldrb	r2, [r2, #0]
     ba2:	0336      	lsls	r6, r6, #12
     ba4:	0412      	lsls	r2, r2, #16
     ba6:	6b1d      	ldr	r5, [r3, #48]	; 0x30
     ba8:	4032      	ands	r2, r6
     baa:	4e0b      	ldr	r6, [pc, #44]	; (bd8 <FLASH_Init+0x8c>)
     bac:	4035      	ands	r5, r6
     bae:	4315      	orrs	r5, r2
     bb0:	631d      	str	r5, [r3, #48]	; 0x30
     bb2:	6add      	ldr	r5, [r3, #44]	; 0x2c
     bb4:	4e09      	ldr	r6, [pc, #36]	; (bdc <FLASH_Init+0x90>)
     bb6:	4035      	ands	r5, r6
     bb8:	432a      	orrs	r2, r5
     bba:	f241 0500 	movw	r5, #4096	; 0x1000
     bbe:	432a      	orrs	r2, r5
     bc0:	62da      	str	r2, [r3, #44]	; 0x2c
     bc2:	0002      	movs	r2, r0
     bc4:	325f      	adds	r2, #95	; 0x5f
     bc6:	7812      	ldrb	r2, [r2, #0]
     bc8:	641a      	str	r2, [r3, #64]	; 0x40
     bca:	7101      	strb	r1, [r0, #4]
     bcc:	f7ff fbe2 	bl	394 <FLASH_SetSpiMode>
     bd0:	0020      	movs	r0, r4
     bd2:	bd70      	pop	{r4, r5, r6, pc}
     bd4:	23000258 	.word	0x23000258
     bd8:	fff0ffff 	.word	0xfff0ffff
     bdc:	fff0cfff 	.word	0xfff0cfff

00000be0 <GPIO_INTMode>:
     be0:	b5f0      	push	{r4, r5, r6, r7, lr}
     be2:	0644      	lsls	r4, r0, #25
     be4:	4d16      	ldr	r5, [pc, #88]	; (c40 <GPIO_INTMode+0x60>)
     be6:	0fa4      	lsrs	r4, r4, #30
     be8:	261f      	movs	r6, #31
     bea:	00a4      	lsls	r4, r4, #2
     bec:	5964      	ldr	r4, [r4, r5]
     bee:	2501      	movs	r5, #1
     bf0:	4006      	ands	r6, r0
     bf2:	40b5      	lsls	r5, r6
     bf4:	2901      	cmp	r1, #1
     bf6:	d116      	bne.n	c26 <GPIO_INTMode+0x46>
     bf8:	43e8      	mvns	r0, r5
     bfa:	2a02      	cmp	r2, #2
     bfc:	d01c      	beq.n	c38 <GPIO_INTMode+0x58>
     bfe:	40b2      	lsls	r2, r6
     c00:	40b3      	lsls	r3, r6
     c02:	6ea7      	ldr	r7, [r4, #104]	; 0x68
     c04:	4007      	ands	r7, r0
     c06:	66a7      	str	r7, [r4, #104]	; 0x68
     c08:	6ba7      	ldr	r7, [r4, #56]	; 0x38
     c0a:	4007      	ands	r7, r0
     c0c:	433a      	orrs	r2, r7
     c0e:	63a2      	str	r2, [r4, #56]	; 0x38
     c10:	6be2      	ldr	r2, [r4, #60]	; 0x3c
     c12:	4002      	ands	r2, r0
     c14:	4313      	orrs	r3, r2
     c16:	63e3      	str	r3, [r4, #60]	; 0x3c
     c18:	6ca3      	ldr	r3, [r4, #72]	; 0x48
     c1a:	4018      	ands	r0, r3
     c1c:	9b05      	ldr	r3, [sp, #20]
     c1e:	2b01      	cmp	r3, #1
     c20:	d100      	bne.n	c24 <GPIO_INTMode+0x44>
     c22:	4328      	orrs	r0, r5
     c24:	64a0      	str	r0, [r4, #72]	; 0x48
     c26:	6b22      	ldr	r2, [r4, #48]	; 0x30
     c28:	0013      	movs	r3, r2
     c2a:	43ab      	bics	r3, r5
     c2c:	2901      	cmp	r1, #1
     c2e:	d101      	bne.n	c34 <GPIO_INTMode+0x54>
     c30:	002b      	movs	r3, r5
     c32:	4313      	orrs	r3, r2
     c34:	6323      	str	r3, [r4, #48]	; 0x30
     c36:	bdf0      	pop	{r4, r5, r6, r7, pc}
     c38:	6ea3      	ldr	r3, [r4, #104]	; 0x68
     c3a:	432b      	orrs	r3, r5
     c3c:	66a3      	str	r3, [r4, #104]	; 0x68
     c3e:	e7eb      	b.n	c18 <GPIO_INTMode+0x38>
     c40:	0000c068 	.word	0x0000c068

00000c44 <GPIO_INTConfig>:
     c44:	0643      	lsls	r3, r0, #25
     c46:	4a09      	ldr	r2, [pc, #36]	; (c6c <GPIO_INTConfig+0x28>)
     c48:	0f9b      	lsrs	r3, r3, #30
     c4a:	009b      	lsls	r3, r3, #2
     c4c:	589a      	ldr	r2, [r3, r2]
     c4e:	231f      	movs	r3, #31
     c50:	4018      	ands	r0, r3
     c52:	3b1e      	subs	r3, #30
     c54:	4083      	lsls	r3, r0
     c56:	b510      	push	{r4, lr}
     c58:	64d3      	str	r3, [r2, #76]	; 0x4c
     c5a:	6b54      	ldr	r4, [r2, #52]	; 0x34
     c5c:	0020      	movs	r0, r4
     c5e:	4398      	bics	r0, r3
     c60:	b909      	cbnz	r1, c66 <GPIO_INTConfig+0x22>
     c62:	4323      	orrs	r3, r4
     c64:	0018      	movs	r0, r3
     c66:	6350      	str	r0, [r2, #52]	; 0x34
     c68:	bd10      	pop	{r4, pc}
     c6a:	46c0      	nop			; (mov r8, r8)
     c6c:	0000c068 	.word	0x0000c068

00000c70 <GPIO_INTHandler>:
     c70:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
     c72:	6d06      	ldr	r6, [r0, #80]	; 0x50
     c74:	4b2e      	ldr	r3, [pc, #184]	; (d30 <GPIO_INTHandler+0xc0>)
     c76:	6c05      	ldr	r5, [r0, #64]	; 0x40
     c78:	64c5      	str	r5, [r0, #76]	; 0x4c
     c7a:	4298      	cmp	r0, r3
     c7c:	d11e      	bne.n	cbc <GPIO_INTHandler+0x4c>
     c7e:	2400      	movs	r4, #0
     c80:	2701      	movs	r7, #1
     c82:	002b      	movs	r3, r5
     c84:	40e3      	lsrs	r3, r4
     c86:	423b      	tst	r3, r7
     c88:	d013      	beq.n	cb2 <GPIO_INTHandler+0x42>
     c8a:	4a2a      	ldr	r2, [pc, #168]	; (d34 <GPIO_INTHandler+0xc4>)
     c8c:	00a3      	lsls	r3, r4, #2
     c8e:	189a      	adds	r2, r3, r2
     c90:	6812      	ldr	r2, [r2, #0]
     c92:	b172      	cbz	r2, cb2 <GPIO_INTHandler+0x42>
     c94:	0421      	lsls	r1, r4, #16
     c96:	468c      	mov	ip, r1
     c98:	2102      	movs	r1, #2
     c9a:	4660      	mov	r0, ip
     c9c:	4301      	orrs	r1, r0
     c9e:	0030      	movs	r0, r6
     ca0:	40e0      	lsrs	r0, r4
     ca2:	4238      	tst	r0, r7
     ca4:	d001      	beq.n	caa <GPIO_INTHandler+0x3a>
     ca6:	4661      	mov	r1, ip
     ca8:	4339      	orrs	r1, r7
     caa:	4823      	ldr	r0, [pc, #140]	; (d38 <GPIO_INTHandler+0xc8>)
     cac:	181b      	adds	r3, r3, r0
     cae:	6818      	ldr	r0, [r3, #0]
     cb0:	4790      	blx	r2
     cb2:	3401      	adds	r4, #1
     cb4:	2c20      	cmp	r4, #32
     cb6:	d1e4      	bne.n	c82 <GPIO_INTHandler+0x12>
     cb8:	2000      	movs	r0, #0
     cba:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
     cbc:	4b1f      	ldr	r3, [pc, #124]	; (d3c <GPIO_INTHandler+0xcc>)
     cbe:	2400      	movs	r4, #0
     cc0:	2701      	movs	r7, #1
     cc2:	4298      	cmp	r0, r3
     cc4:	d119      	bne.n	cfa <GPIO_INTHandler+0x8a>
     cc6:	002b      	movs	r3, r5
     cc8:	40e3      	lsrs	r3, r4
     cca:	423b      	tst	r3, r7
     ccc:	d00f      	beq.n	cee <GPIO_INTHandler+0x7e>
     cce:	4b1c      	ldr	r3, [pc, #112]	; (d40 <GPIO_INTHandler+0xd0>)
     cd0:	00a2      	lsls	r2, r4, #2
     cd2:	18d3      	adds	r3, r2, r3
     cd4:	681b      	ldr	r3, [r3, #0]
     cd6:	b153      	cbz	r3, cee <GPIO_INTHandler+0x7e>
     cd8:	0030      	movs	r0, r6
     cda:	40e0      	lsrs	r0, r4
     cdc:	0421      	lsls	r1, r4, #16
     cde:	4238      	tst	r0, r7
     ce0:	d009      	beq.n	cf6 <GPIO_INTHandler+0x86>
     ce2:	4818      	ldr	r0, [pc, #96]	; (d44 <GPIO_INTHandler+0xd4>)
     ce4:	4301      	orrs	r1, r0
     ce6:	4818      	ldr	r0, [pc, #96]	; (d48 <GPIO_INTHandler+0xd8>)
     ce8:	1812      	adds	r2, r2, r0
     cea:	6810      	ldr	r0, [r2, #0]
     cec:	4798      	blx	r3
     cee:	3401      	adds	r4, #1
     cf0:	2c20      	cmp	r4, #32
     cf2:	d1e8      	bne.n	cc6 <GPIO_INTHandler+0x56>
     cf4:	e7e0      	b.n	cb8 <GPIO_INTHandler+0x48>
     cf6:	4815      	ldr	r0, [pc, #84]	; (d4c <GPIO_INTHandler+0xdc>)
     cf8:	e7f4      	b.n	ce4 <GPIO_INTHandler+0x74>
     cfa:	002b      	movs	r3, r5
     cfc:	40e3      	lsrs	r3, r4
     cfe:	423b      	tst	r3, r7
     d00:	d00f      	beq.n	d22 <GPIO_INTHandler+0xb2>
     d02:	4b13      	ldr	r3, [pc, #76]	; (d50 <GPIO_INTHandler+0xe0>)
     d04:	00a2      	lsls	r2, r4, #2
     d06:	18d3      	adds	r3, r2, r3
     d08:	681b      	ldr	r3, [r3, #0]
     d0a:	b153      	cbz	r3, d22 <GPIO_INTHandler+0xb2>
     d0c:	0030      	movs	r0, r6
     d0e:	40e0      	lsrs	r0, r4
     d10:	0421      	lsls	r1, r4, #16
     d12:	4238      	tst	r0, r7
     d14:	d009      	beq.n	d2a <GPIO_INTHandler+0xba>
     d16:	480f      	ldr	r0, [pc, #60]	; (d54 <GPIO_INTHandler+0xe4>)
     d18:	4301      	orrs	r1, r0
     d1a:	480f      	ldr	r0, [pc, #60]	; (d58 <GPIO_INTHandler+0xe8>)
     d1c:	1812      	adds	r2, r2, r0
     d1e:	6810      	ldr	r0, [r2, #0]
     d20:	4798      	blx	r3
     d22:	3401      	adds	r4, #1
     d24:	2c20      	cmp	r4, #32
     d26:	d1e8      	bne.n	cfa <GPIO_INTHandler+0x8a>
     d28:	e7c6      	b.n	cb8 <GPIO_INTHandler+0x48>
     d2a:	480c      	ldr	r0, [pc, #48]	; (d5c <GPIO_INTHandler+0xec>)
     d2c:	e7f4      	b.n	d18 <GPIO_INTHandler+0xa8>
     d2e:	46c0      	nop			; (mov r8, r8)
     d30:	4200d000 	.word	0x4200d000
     d34:	2300054c 	.word	0x2300054c
     d38:	230003cc 	.word	0x230003cc
     d3c:	4200d400 	.word	0x4200d400
     d40:	230004cc 	.word	0x230004cc
     d44:	00200001 	.word	0x00200001
     d48:	2300034c 	.word	0x2300034c
     d4c:	00200002 	.word	0x00200002
     d50:	2300044c 	.word	0x2300044c
     d54:	00400001 	.word	0x00400001
     d58:	230002cc 	.word	0x230002cc
     d5c:	00400002 	.word	0x00400002

00000d60 <GPIO_Direction>:
     d60:	0643      	lsls	r3, r0, #25
     d62:	4a08      	ldr	r2, [pc, #32]	; (d84 <GPIO_Direction+0x24>)
     d64:	0f9b      	lsrs	r3, r3, #30
     d66:	009b      	lsls	r3, r3, #2
     d68:	589b      	ldr	r3, [r3, r2]
     d6a:	221f      	movs	r2, #31
     d6c:	4010      	ands	r0, r2
     d6e:	3a1e      	subs	r2, #30
     d70:	4082      	lsls	r2, r0
     d72:	b919      	cbnz	r1, d7c <GPIO_Direction+0x1c>
     d74:	6859      	ldr	r1, [r3, #4]
     d76:	4391      	bics	r1, r2
     d78:	6059      	str	r1, [r3, #4]
     d7a:	4770      	bx	lr
     d7c:	6859      	ldr	r1, [r3, #4]
     d7e:	430a      	orrs	r2, r1
     d80:	605a      	str	r2, [r3, #4]
     d82:	e7fa      	b.n	d7a <GPIO_Direction+0x1a>
     d84:	0000c068 	.word	0x0000c068

00000d88 <GPIO_Init>:
     d88:	6803      	ldr	r3, [r0, #0]
     d8a:	b537      	push	{r0, r1, r2, r4, r5, lr}
     d8c:	0004      	movs	r4, r0
     d8e:	2b02      	cmp	r3, #2
     d90:	d904      	bls.n	d9c <GPIO_Init+0x14>
     d92:	f240 1102 	movw	r1, #258	; 0x102
     d96:	4818      	ldr	r0, [pc, #96]	; (df8 <GPIO_Init+0x70>)
     d98:	f7ff f9bc 	bl	114 <io_assert_failed>
     d9c:	6963      	ldr	r3, [r4, #20]
     d9e:	2100      	movs	r1, #0
     da0:	b2d8      	uxtb	r0, r3
     da2:	4b16      	ldr	r3, [pc, #88]	; (dfc <GPIO_Init+0x74>)
     da4:	4798      	blx	r3
     da6:	6823      	ldr	r3, [r4, #0]
     da8:	2100      	movs	r1, #0
     daa:	4d15      	ldr	r5, [pc, #84]	; (e00 <GPIO_Init+0x78>)
     dac:	2b02      	cmp	r3, #2
     dae:	d10a      	bne.n	dc6 <GPIO_Init+0x3e>
     db0:	6960      	ldr	r0, [r4, #20]
     db2:	4b14      	ldr	r3, [pc, #80]	; (e04 <GPIO_Init+0x7c>)
     db4:	4798      	blx	r3
     db6:	6923      	ldr	r3, [r4, #16]
     db8:	2101      	movs	r1, #1
     dba:	9300      	str	r3, [sp, #0]
     dbc:	68a2      	ldr	r2, [r4, #8]
     dbe:	68e3      	ldr	r3, [r4, #12]
     dc0:	6960      	ldr	r0, [r4, #20]
     dc2:	47a8      	blx	r5
     dc4:	bd37      	pop	{r0, r1, r2, r4, r5, pc}
     dc6:	000b      	movs	r3, r1
     dc8:	000a      	movs	r2, r1
     dca:	9100      	str	r1, [sp, #0]
     dcc:	6960      	ldr	r0, [r4, #20]
     dce:	47a8      	blx	r5
     dd0:	6821      	ldr	r1, [r4, #0]
     dd2:	2901      	cmp	r1, #1
     dd4:	d103      	bne.n	dde <GPIO_Init+0x56>
     dd6:	6960      	ldr	r0, [r4, #20]
     dd8:	4b0a      	ldr	r3, [pc, #40]	; (e04 <GPIO_Init+0x7c>)
     dda:	4798      	blx	r3
     ddc:	e7f2      	b.n	dc4 <GPIO_Init+0x3c>
     dde:	2900      	cmp	r1, #0
     de0:	d1f0      	bne.n	dc4 <GPIO_Init+0x3c>
     de2:	6960      	ldr	r0, [r4, #20]
     de4:	4b07      	ldr	r3, [pc, #28]	; (e04 <GPIO_Init+0x7c>)
     de6:	4798      	blx	r3
     de8:	6863      	ldr	r3, [r4, #4]
     dea:	6960      	ldr	r0, [r4, #20]
     dec:	b2d9      	uxtb	r1, r3
     dee:	b2c0      	uxtb	r0, r0
     df0:	4b05      	ldr	r3, [pc, #20]	; (e08 <GPIO_Init+0x80>)
     df2:	4798      	blx	r3
     df4:	e7e6      	b.n	dc4 <GPIO_Init+0x3c>
     df6:	46c0      	nop			; (mov r8, r8)
     df8:	0000c3f0 	.word	0x0000c3f0
     dfc:	00001909 	.word	0x00001909
     e00:	00000be1 	.word	0x00000be1
     e04:	00000d61 	.word	0x00000d61
     e08:	00001869 	.word	0x00001869

00000e0c <GPIO_DeInit>:
     e0c:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
     e0e:	0645      	lsls	r5, r0, #25
     e10:	4b13      	ldr	r3, [pc, #76]	; (e60 <GPIO_DeInit+0x54>)
     e12:	0fad      	lsrs	r5, r5, #30
     e14:	00aa      	lsls	r2, r5, #2
     e16:	58d6      	ldr	r6, [r2, r3]
     e18:	0004      	movs	r4, r0
     e1a:	221f      	movs	r2, #31
     e1c:	2701      	movs	r7, #1
     e1e:	4014      	ands	r4, r2
     e20:	40a7      	lsls	r7, r4
     e22:	6873      	ldr	r3, [r6, #4]
     e24:	2100      	movs	r1, #0
     e26:	43bb      	bics	r3, r7
     e28:	6073      	str	r3, [r6, #4]
     e2a:	b2c0      	uxtb	r0, r0
     e2c:	4b0d      	ldr	r3, [pc, #52]	; (e64 <GPIO_DeInit+0x58>)
     e2e:	4798      	blx	r3
     e30:	43fa      	mvns	r2, r7
     e32:	6b33      	ldr	r3, [r6, #48]	; 0x30
     e34:	00a4      	lsls	r4, r4, #2
     e36:	4013      	ands	r3, r2
     e38:	6333      	str	r3, [r6, #48]	; 0x30
     e3a:	64f7      	str	r7, [r6, #76]	; 0x4c
     e3c:	b925      	cbnz	r5, e48 <GPIO_DeInit+0x3c>
     e3e:	4b0a      	ldr	r3, [pc, #40]	; (e68 <GPIO_DeInit+0x5c>)
     e40:	511d      	str	r5, [r3, r4]
     e42:	4b0a      	ldr	r3, [pc, #40]	; (e6c <GPIO_DeInit+0x60>)
     e44:	511d      	str	r5, [r3, r4]
     e46:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
     e48:	2300      	movs	r3, #0
     e4a:	2d01      	cmp	r5, #1
     e4c:	d104      	bne.n	e58 <GPIO_DeInit+0x4c>
     e4e:	4a08      	ldr	r2, [pc, #32]	; (e70 <GPIO_DeInit+0x64>)
     e50:	5113      	str	r3, [r2, r4]
     e52:	4a08      	ldr	r2, [pc, #32]	; (e74 <GPIO_DeInit+0x68>)
     e54:	5113      	str	r3, [r2, r4]
     e56:	e7f6      	b.n	e46 <GPIO_DeInit+0x3a>
     e58:	4a07      	ldr	r2, [pc, #28]	; (e78 <GPIO_DeInit+0x6c>)
     e5a:	5113      	str	r3, [r2, r4]
     e5c:	4a07      	ldr	r2, [pc, #28]	; (e7c <GPIO_DeInit+0x70>)
     e5e:	e7f9      	b.n	e54 <GPIO_DeInit+0x48>
     e60:	0000c068 	.word	0x0000c068
     e64:	00001869 	.word	0x00001869
     e68:	2300054c 	.word	0x2300054c
     e6c:	230003cc 	.word	0x230003cc
     e70:	230004cc 	.word	0x230004cc
     e74:	2300034c 	.word	0x2300034c
     e78:	2300044c 	.word	0x2300044c
     e7c:	230002cc 	.word	0x230002cc

00000e80 <GPIO_ReadDataBit>:
     e80:	0643      	lsls	r3, r0, #25
     e82:	4a07      	ldr	r2, [pc, #28]	; (ea0 <GPIO_ReadDataBit+0x20>)
     e84:	0f9b      	lsrs	r3, r3, #30
     e86:	009b      	lsls	r3, r3, #2
     e88:	589b      	ldr	r3, [r3, r2]
     e8a:	221f      	movs	r2, #31
     e8c:	4010      	ands	r0, r2
     e8e:	3a1e      	subs	r2, #30
     e90:	4082      	lsls	r2, r0
     e92:	0010      	movs	r0, r2
     e94:	6d1b      	ldr	r3, [r3, #80]	; 0x50
     e96:	4018      	ands	r0, r3
     e98:	1e43      	subs	r3, r0, #1
     e9a:	4198      	sbcs	r0, r3
     e9c:	4770      	bx	lr
     e9e:	46c0      	nop			; (mov r8, r8)
     ea0:	0000c068 	.word	0x0000c068

00000ea4 <GPIO_WriteBit>:
     ea4:	0643      	lsls	r3, r0, #25
     ea6:	4a08      	ldr	r2, [pc, #32]	; (ec8 <GPIO_WriteBit+0x24>)
     ea8:	0f9b      	lsrs	r3, r3, #30
     eaa:	009b      	lsls	r3, r3, #2
     eac:	589a      	ldr	r2, [r3, r2]
     eae:	231f      	movs	r3, #31
     eb0:	b510      	push	{r4, lr}
     eb2:	6814      	ldr	r4, [r2, #0]
     eb4:	4018      	ands	r0, r3
     eb6:	3b1e      	subs	r3, #30
     eb8:	4083      	lsls	r3, r0
     eba:	0020      	movs	r0, r4
     ebc:	4318      	orrs	r0, r3
     ebe:	b909      	cbnz	r1, ec4 <GPIO_WriteBit+0x20>
     ec0:	0020      	movs	r0, r4
     ec2:	4398      	bics	r0, r3
     ec4:	6010      	str	r0, [r2, #0]
     ec6:	bd10      	pop	{r4, pc}
     ec8:	0000c068 	.word	0x0000c068

00000ecc <GPIO_PortDirection>:
     ecc:	b570      	push	{r4, r5, r6, lr}
     ece:	0004      	movs	r4, r0
     ed0:	000d      	movs	r5, r1
     ed2:	0016      	movs	r6, r2
     ed4:	2802      	cmp	r0, #2
     ed6:	d904      	bls.n	ee2 <GPIO_PortDirection+0x16>
     ed8:	f240 1195 	movw	r1, #405	; 0x195
     edc:	4806      	ldr	r0, [pc, #24]	; (ef8 <GPIO_PortDirection+0x2c>)
     ede:	f7ff f919 	bl	114 <io_assert_failed>
     ee2:	4b06      	ldr	r3, [pc, #24]	; (efc <GPIO_PortDirection+0x30>)
     ee4:	00a4      	lsls	r4, r4, #2
     ee6:	58e3      	ldr	r3, [r4, r3]
     ee8:	6859      	ldr	r1, [r3, #4]
     eea:	000a      	movs	r2, r1
     eec:	432a      	orrs	r2, r5
     eee:	b90e      	cbnz	r6, ef4 <GPIO_PortDirection+0x28>
     ef0:	000a      	movs	r2, r1
     ef2:	43aa      	bics	r2, r5
     ef4:	605a      	str	r2, [r3, #4]
     ef6:	bd70      	pop	{r4, r5, r6, pc}
     ef8:	0000c3dd 	.word	0x0000c3dd
     efc:	0000c068 	.word	0x0000c068

00000f00 <GPIO_PortRead>:
     f00:	b570      	push	{r4, r5, r6, lr}
     f02:	0004      	movs	r4, r0
     f04:	000d      	movs	r5, r1
     f06:	2802      	cmp	r0, #2
     f08:	d904      	bls.n	f14 <GPIO_PortRead+0x14>
     f0a:	f240 11b5 	movw	r1, #437	; 0x1b5
     f0e:	4804      	ldr	r0, [pc, #16]	; (f20 <GPIO_PortRead+0x20>)
     f10:	f7ff f900 	bl	114 <io_assert_failed>
     f14:	4b03      	ldr	r3, [pc, #12]	; (f24 <GPIO_PortRead+0x24>)
     f16:	00a4      	lsls	r4, r4, #2
     f18:	58e3      	ldr	r3, [r4, r3]
     f1a:	6d18      	ldr	r0, [r3, #80]	; 0x50
     f1c:	4028      	ands	r0, r5
     f1e:	bd70      	pop	{r4, r5, r6, pc}
     f20:	0000c3cf 	.word	0x0000c3cf
     f24:	0000c068 	.word	0x0000c068

00000f28 <GPIO_PortWrite>:
     f28:	b570      	push	{r4, r5, r6, lr}
     f2a:	0004      	movs	r4, r0
     f2c:	000d      	movs	r5, r1
     f2e:	0016      	movs	r6, r2
     f30:	2802      	cmp	r0, #2
     f32:	d904      	bls.n	f3e <GPIO_PortWrite+0x16>
     f34:	f240 11d0 	movw	r1, #464	; 0x1d0
     f38:	4805      	ldr	r0, [pc, #20]	; (f50 <GPIO_PortWrite+0x28>)
     f3a:	f7ff f8eb 	bl	114 <io_assert_failed>
     f3e:	4b05      	ldr	r3, [pc, #20]	; (f54 <GPIO_PortWrite+0x2c>)
     f40:	00a4      	lsls	r4, r4, #2
     f42:	58e2      	ldr	r2, [r4, r3]
     f44:	6813      	ldr	r3, [r2, #0]
     f46:	405e      	eors	r6, r3
     f48:	4035      	ands	r5, r6
     f4a:	406b      	eors	r3, r5
     f4c:	6013      	str	r3, [r2, #0]
     f4e:	bd70      	pop	{r4, r5, r6, pc}
     f50:	0000c3c0 	.word	0x0000c3c0
     f54:	0000c068 	.word	0x0000c068

00000f58 <GPIO_DebounceClock>:
     f58:	b570      	push	{r4, r5, r6, lr}
     f5a:	0004      	movs	r4, r0
     f5c:	000d      	movs	r5, r1
     f5e:	2802      	cmp	r0, #2
     f60:	d904      	bls.n	f6c <GPIO_DebounceClock+0x14>
     f62:	f240 11ef 	movw	r1, #495	; 0x1ef
     f66:	4805      	ldr	r0, [pc, #20]	; (f7c <GPIO_DebounceClock+0x24>)
     f68:	f7ff f8d4 	bl	114 <io_assert_failed>
     f6c:	217f      	movs	r1, #127	; 0x7f
     f6e:	4b04      	ldr	r3, [pc, #16]	; (f80 <GPIO_DebounceClock+0x28>)
     f70:	00a4      	lsls	r4, r4, #2
     f72:	58e3      	ldr	r3, [r4, r3]
     f74:	400d      	ands	r5, r1
     f76:	679d      	str	r5, [r3, #120]	; 0x78
     f78:	bd70      	pop	{r4, r5, r6, pc}
     f7a:	46c0      	nop			; (mov r8, r8)
     f7c:	0000c3ad 	.word	0x0000c3ad
     f80:	0000c068 	.word	0x0000c068

00000f84 <GPIO_LevelSync>:
     f84:	b570      	push	{r4, r5, r6, lr}
     f86:	0004      	movs	r4, r0
     f88:	000d      	movs	r5, r1
     f8a:	2802      	cmp	r0, #2
     f8c:	d904      	bls.n	f98 <GPIO_LevelSync+0x14>
     f8e:	f240 2108 	movw	r1, #520	; 0x208
     f92:	4806      	ldr	r0, [pc, #24]	; (fac <GPIO_LevelSync+0x28>)
     f94:	f7ff f8be 	bl	114 <io_assert_failed>
     f98:	4b05      	ldr	r3, [pc, #20]	; (fb0 <GPIO_LevelSync+0x2c>)
     f9a:	00a4      	lsls	r4, r4, #2
     f9c:	58e3      	ldr	r3, [r4, r3]
     f9e:	2d01      	cmp	r5, #1
     fa0:	d101      	bne.n	fa6 <GPIO_LevelSync+0x22>
     fa2:	661d      	str	r5, [r3, #96]	; 0x60
     fa4:	bd70      	pop	{r4, r5, r6, pc}
     fa6:	2200      	movs	r2, #0
     fa8:	661a      	str	r2, [r3, #96]	; 0x60
     faa:	e7fb      	b.n	fa4 <GPIO_LevelSync+0x20>
     fac:	0000c39e 	.word	0x0000c39e
     fb0:	0000c068 	.word	0x0000c068

00000fb4 <GPIO_UserRegIrq>:
     fb4:	b510      	push	{r4, lr}
     fb6:	241f      	movs	r4, #31
     fb8:	0643      	lsls	r3, r0, #25
     fba:	0f9b      	lsrs	r3, r3, #30
     fbc:	4020      	ands	r0, r4
     fbe:	0080      	lsls	r0, r0, #2
     fc0:	b923      	cbnz	r3, fcc <GPIO_UserRegIrq+0x18>
     fc2:	4b07      	ldr	r3, [pc, #28]	; (fe0 <GPIO_UserRegIrq+0x2c>)
     fc4:	5019      	str	r1, [r3, r0]
     fc6:	4b07      	ldr	r3, [pc, #28]	; (fe4 <GPIO_UserRegIrq+0x30>)
     fc8:	501a      	str	r2, [r3, r0]
     fca:	bd10      	pop	{r4, pc}
     fcc:	2b01      	cmp	r3, #1
     fce:	d103      	bne.n	fd8 <GPIO_UserRegIrq+0x24>
     fd0:	4b05      	ldr	r3, [pc, #20]	; (fe8 <GPIO_UserRegIrq+0x34>)
     fd2:	5019      	str	r1, [r3, r0]
     fd4:	4b05      	ldr	r3, [pc, #20]	; (fec <GPIO_UserRegIrq+0x38>)
     fd6:	e7f7      	b.n	fc8 <GPIO_UserRegIrq+0x14>
     fd8:	4b05      	ldr	r3, [pc, #20]	; (ff0 <GPIO_UserRegIrq+0x3c>)
     fda:	5019      	str	r1, [r3, r0]
     fdc:	4b05      	ldr	r3, [pc, #20]	; (ff4 <GPIO_UserRegIrq+0x40>)
     fde:	e7f3      	b.n	fc8 <GPIO_UserRegIrq+0x14>
     fe0:	2300054c 	.word	0x2300054c
     fe4:	230003cc 	.word	0x230003cc
     fe8:	230004cc 	.word	0x230004cc
     fec:	2300034c 	.word	0x2300034c
     ff0:	2300044c 	.word	0x2300044c
     ff4:	230002cc 	.word	0x230002cc

00000ff8 <LOGUART_StructInit>:
     ff8:	2300      	movs	r3, #0
     ffa:	2201      	movs	r2, #1
     ffc:	2140      	movs	r1, #64	; 0x40
     ffe:	6003      	str	r3, [r0, #0]
    1000:	6083      	str	r3, [r0, #8]
    1002:	60c3      	str	r3, [r0, #12]
    1004:	6103      	str	r3, [r0, #16]
    1006:	6143      	str	r3, [r0, #20]
    1008:	6183      	str	r3, [r0, #24]
    100a:	61c3      	str	r3, [r0, #28]
    100c:	6283      	str	r3, [r0, #40]	; 0x28
    100e:	6303      	str	r3, [r0, #48]	; 0x30
    1010:	6343      	str	r3, [r0, #52]	; 0x34
    1012:	6383      	str	r3, [r0, #56]	; 0x38
    1014:	63c3      	str	r3, [r0, #60]	; 0x3c
    1016:	3307      	adds	r3, #7
    1018:	6403      	str	r3, [r0, #64]	; 0x40
    101a:	33f8      	adds	r3, #248	; 0xf8
    101c:	6042      	str	r2, [r0, #4]
    101e:	6241      	str	r1, [r0, #36]	; 0x24
    1020:	6202      	str	r2, [r0, #32]
    1022:	62c2      	str	r2, [r0, #44]	; 0x2c
    1024:	6442      	str	r2, [r0, #68]	; 0x44
    1026:	6483      	str	r3, [r0, #72]	; 0x48
    1028:	4770      	bx	lr

0000102a <LOGUART_SetBaud>:
    102a:	b573      	push	{r0, r1, r4, r5, r6, lr}
    102c:	000d      	movs	r5, r1
    102e:	0004      	movs	r4, r0
    1030:	4b1c      	ldr	r3, [pc, #112]	; (10a4 <LOGUART_SetBaud+0x7a>)
    1032:	4798      	blx	r3
    1034:	466a      	mov	r2, sp
    1036:	0029      	movs	r1, r5
    1038:	ab01      	add	r3, sp, #4
    103a:	4d1b      	ldr	r5, [pc, #108]	; (10a8 <LOGUART_SetBaud+0x7e>)
    103c:	47a8      	blx	r5
    103e:	2080      	movs	r0, #128	; 0x80
    1040:	68e3      	ldr	r3, [r4, #12]
    1042:	491a      	ldr	r1, [pc, #104]	; (10ac <LOGUART_SetBaud+0x82>)
    1044:	4303      	orrs	r3, r0
    1046:	60e3      	str	r3, [r4, #12]
    1048:	69e3      	ldr	r3, [r4, #28]
    104a:	4d19      	ldr	r5, [pc, #100]	; (10b0 <LOGUART_SetBaud+0x86>)
    104c:	4303      	orrs	r3, r0
    104e:	61e3      	str	r3, [r4, #28]
    1050:	6a22      	ldr	r2, [r4, #32]
    1052:	9b00      	ldr	r3, [sp, #0]
    1054:	4011      	ands	r1, r2
    1056:	011a      	lsls	r2, r3, #4
    1058:	402a      	ands	r2, r5
    105a:	430a      	orrs	r2, r1
    105c:	6222      	str	r2, [r4, #32]
    105e:	69e1      	ldr	r1, [r4, #28]
    1060:	9a01      	ldr	r2, [sp, #4]
    1062:	4d14      	ldr	r5, [pc, #80]	; (10b4 <LOGUART_SetBaud+0x8a>)
    1064:	4e14      	ldr	r6, [pc, #80]	; (10b8 <LOGUART_SetBaud+0x8e>)
    1066:	400d      	ands	r5, r1
    1068:	0411      	lsls	r1, r2, #16
    106a:	4031      	ands	r1, r6
    106c:	4329      	orrs	r1, r5
    106e:	61e1      	str	r1, [r4, #28]
    1070:	68e1      	ldr	r1, [r4, #12]
    1072:	00d2      	lsls	r2, r2, #3
    1074:	4381      	bics	r1, r0
    1076:	60e1      	str	r1, [r4, #12]
    1078:	6ca1      	ldr	r1, [r4, #72]	; 0x48
    107a:	4810      	ldr	r0, [pc, #64]	; (10bc <LOGUART_SetBaud+0x92>)
    107c:	0d09      	lsrs	r1, r1, #20
    107e:	0509      	lsls	r1, r1, #20
    1080:	4319      	orrs	r1, r3
    1082:	64a1      	str	r1, [r4, #72]	; 0x48
    1084:	6c61      	ldr	r1, [r4, #68]	; 0x44
    1086:	025b      	lsls	r3, r3, #9
    1088:	4001      	ands	r1, r0
    108a:	480d      	ldr	r0, [pc, #52]	; (10c0 <LOGUART_SetBaud+0x96>)
    108c:	4003      	ands	r3, r0
    108e:	430b      	orrs	r3, r1
    1090:	6463      	str	r3, [r4, #68]	; 0x44
    1092:	6c23      	ldr	r3, [r4, #64]	; 0x40
    1094:	490b      	ldr	r1, [pc, #44]	; (10c4 <LOGUART_SetBaud+0x9a>)
    1096:	400b      	ands	r3, r1
    1098:	f643 71f8 	movw	r1, #16376	; 0x3ff8
    109c:	400a      	ands	r2, r1
    109e:	431a      	orrs	r2, r3
    10a0:	6422      	str	r2, [r4, #64]	; 0x40
    10a2:	bd73      	pop	{r0, r1, r4, r5, r6, pc}
    10a4:	000001c9 	.word	0x000001c9
    10a8:	00002e4b 	.word	0x00002e4b
    10ac:	ff00000f 	.word	0xff00000f
    10b0:	00fffff0 	.word	0x00fffff0
    10b4:	f800ffff 	.word	0xf800ffff
    10b8:	07ff0000 	.word	0x07ff0000
    10bc:	e00001ff 	.word	0xe00001ff
    10c0:	1ffffe00 	.word	0x1ffffe00
    10c4:	ffffc007 	.word	0xffffc007

000010c8 <LOGUART_RxCmd>:
    10c8:	2304      	movs	r3, #4
    10ca:	6c02      	ldr	r2, [r0, #64]	; 0x40
    10cc:	b111      	cbz	r1, 10d4 <LOGUART_RxCmd+0xc>
    10ce:	4313      	orrs	r3, r2
    10d0:	6403      	str	r3, [r0, #64]	; 0x40
    10d2:	4770      	bx	lr
    10d4:	439a      	bics	r2, r3
    10d6:	6402      	str	r2, [r0, #64]	; 0x40
    10d8:	e7fb      	b.n	10d2 <LOGUART_RxCmd+0xa>

000010da <LOGUART_AGGCmd>:
    10da:	b129      	cbz	r1, 10e8 <LOGUART_AGGCmd+0xe>
    10dc:	2380      	movs	r3, #128	; 0x80
    10de:	6d82      	ldr	r2, [r0, #88]	; 0x58
    10e0:	04db      	lsls	r3, r3, #19
    10e2:	4313      	orrs	r3, r2
    10e4:	6583      	str	r3, [r0, #88]	; 0x58
    10e6:	4770      	bx	lr
    10e8:	6d83      	ldr	r3, [r0, #88]	; 0x58
    10ea:	4a01      	ldr	r2, [pc, #4]	; (10f0 <LOGUART_AGGCmd+0x16>)
    10ec:	4013      	ands	r3, r2
    10ee:	e7f9      	b.n	10e4 <LOGUART_AGGCmd+0xa>
    10f0:	fbffffff 	.word	0xfbffffff

000010f4 <LOGUART_AGGPathCmd>:
    10f4:	1e4b      	subs	r3, r1, #1
    10f6:	b570      	push	{r4, r5, r6, lr}
    10f8:	0006      	movs	r6, r0
    10fa:	000c      	movs	r4, r1
    10fc:	0015      	movs	r5, r2
    10fe:	2b04      	cmp	r3, #4
    1100:	d904      	bls.n	110c <LOGUART_AGGPathCmd+0x18>
    1102:	f240 1147 	movw	r1, #327	; 0x147
    1106:	481a      	ldr	r0, [pc, #104]	; (1170 <LOGUART_AGGPathCmd+0x7c>)
    1108:	f7ff f804 	bl	114 <io_assert_failed>
    110c:	6db3      	ldr	r3, [r6, #88]	; 0x58
    110e:	2c03      	cmp	r4, #3
    1110:	d01f      	beq.n	1152 <LOGUART_AGGPathCmd+0x5e>
    1112:	d808      	bhi.n	1126 <LOGUART_AGGPathCmd+0x32>
    1114:	2c01      	cmp	r4, #1
    1116:	d00e      	beq.n	1136 <LOGUART_AGGPathCmd+0x42>
    1118:	2c02      	cmp	r4, #2
    111a:	d014      	beq.n	1146 <LOGUART_AGGPathCmd+0x52>
    111c:	65b3      	str	r3, [r6, #88]	; 0x58
    111e:	2001      	movs	r0, #1
    1120:	4b14      	ldr	r3, [pc, #80]	; (1174 <LOGUART_AGGPathCmd+0x80>)
    1122:	4798      	blx	r3
    1124:	bd70      	pop	{r4, r5, r6, pc}
    1126:	2c04      	cmp	r4, #4
    1128:	d019      	beq.n	115e <LOGUART_AGGPathCmd+0x6a>
    112a:	2c05      	cmp	r4, #5
    112c:	d1f6      	bne.n	111c <LOGUART_AGGPathCmd+0x28>
    112e:	b1e5      	cbz	r5, 116a <LOGUART_AGGPathCmd+0x76>
    1130:	2280      	movs	r2, #128	; 0x80
    1132:	0612      	lsls	r2, r2, #24
    1134:	e002      	b.n	113c <LOGUART_AGGPathCmd+0x48>
    1136:	b11d      	cbz	r5, 1140 <LOGUART_AGGPathCmd+0x4c>
    1138:	2280      	movs	r2, #128	; 0x80
    113a:	0512      	lsls	r2, r2, #20
    113c:	4313      	orrs	r3, r2
    113e:	e7ed      	b.n	111c <LOGUART_AGGPathCmd+0x28>
    1140:	4a0d      	ldr	r2, [pc, #52]	; (1178 <LOGUART_AGGPathCmd+0x84>)
    1142:	4013      	ands	r3, r2
    1144:	e7ea      	b.n	111c <LOGUART_AGGPathCmd+0x28>
    1146:	b115      	cbz	r5, 114e <LOGUART_AGGPathCmd+0x5a>
    1148:	2280      	movs	r2, #128	; 0x80
    114a:	0552      	lsls	r2, r2, #21
    114c:	e7f6      	b.n	113c <LOGUART_AGGPathCmd+0x48>
    114e:	4a0b      	ldr	r2, [pc, #44]	; (117c <LOGUART_AGGPathCmd+0x88>)
    1150:	e7f7      	b.n	1142 <LOGUART_AGGPathCmd+0x4e>
    1152:	b115      	cbz	r5, 115a <LOGUART_AGGPathCmd+0x66>
    1154:	2280      	movs	r2, #128	; 0x80
    1156:	0592      	lsls	r2, r2, #22
    1158:	e7f0      	b.n	113c <LOGUART_AGGPathCmd+0x48>
    115a:	4a09      	ldr	r2, [pc, #36]	; (1180 <LOGUART_AGGPathCmd+0x8c>)
    115c:	e7f1      	b.n	1142 <LOGUART_AGGPathCmd+0x4e>
    115e:	b115      	cbz	r5, 1166 <LOGUART_AGGPathCmd+0x72>
    1160:	2280      	movs	r2, #128	; 0x80
    1162:	05d2      	lsls	r2, r2, #23
    1164:	e7ea      	b.n	113c <LOGUART_AGGPathCmd+0x48>
    1166:	4a07      	ldr	r2, [pc, #28]	; (1184 <LOGUART_AGGPathCmd+0x90>)
    1168:	e7eb      	b.n	1142 <LOGUART_AGGPathCmd+0x4e>
    116a:	005b      	lsls	r3, r3, #1
    116c:	085b      	lsrs	r3, r3, #1
    116e:	e7d5      	b.n	111c <LOGUART_AGGPathCmd+0x28>
    1170:	0000c456 	.word	0x0000c456
    1174:	0000030b 	.word	0x0000030b
    1178:	f7ffffff 	.word	0xf7ffffff
    117c:	efffffff 	.word	0xefffffff
    1180:	dfffffff 	.word	0xdfffffff
    1184:	bfffffff 	.word	0xbfffffff

00001188 <LOGUART_AGGSetTimeOut>:
    1188:	6d83      	ldr	r3, [r0, #88]	; 0x58
    118a:	4a04      	ldr	r2, [pc, #16]	; (119c <LOGUART_AGGSetTimeOut+0x14>)
    118c:	0309      	lsls	r1, r1, #12
    118e:	4013      	ands	r3, r2
    1190:	4a03      	ldr	r2, [pc, #12]	; (11a0 <LOGUART_AGGSetTimeOut+0x18>)
    1192:	4011      	ands	r1, r2
    1194:	4319      	orrs	r1, r3
    1196:	6581      	str	r1, [r0, #88]	; 0x58
    1198:	4770      	bx	lr
    119a:	46c0      	nop			; (mov r8, r8)
    119c:	fc000fff 	.word	0xfc000fff
    11a0:	03fff000 	.word	0x03fff000

000011a4 <LOGUART_AGGSetThreshold>:
    11a4:	b570      	push	{r4, r5, r6, lr}
    11a6:	0005      	movs	r5, r0
    11a8:	000c      	movs	r4, r1
    11aa:	2907      	cmp	r1, #7
    11ac:	d904      	bls.n	11b8 <LOGUART_AGGSetThreshold+0x14>
    11ae:	f240 119d 	movw	r1, #413	; 0x19d
    11b2:	4806      	ldr	r0, [pc, #24]	; (11cc <LOGUART_AGGSetThreshold+0x28>)
    11b4:	f7fe ffae 	bl	114 <io_assert_failed>
    11b8:	6dab      	ldr	r3, [r5, #88]	; 0x58
    11ba:	4905      	ldr	r1, [pc, #20]	; (11d0 <LOGUART_AGGSetThreshold+0x2c>)
    11bc:	0224      	lsls	r4, r4, #8
    11be:	400b      	ands	r3, r1
    11c0:	f640 7100 	movw	r1, #3840	; 0xf00
    11c4:	400c      	ands	r4, r1
    11c6:	431c      	orrs	r4, r3
    11c8:	65ac      	str	r4, [r5, #88]	; 0x58
    11ca:	bd70      	pop	{r4, r5, r6, pc}
    11cc:	0000c43e 	.word	0x0000c43e
    11d0:	fffff0ff 	.word	0xfffff0ff

000011d4 <LOGUART_AGGSetSyncPattern>:
    11d4:	22ff      	movs	r2, #255	; 0xff
    11d6:	6d83      	ldr	r3, [r0, #88]	; 0x58
    11d8:	4011      	ands	r1, r2
    11da:	4393      	bics	r3, r2
    11dc:	4319      	orrs	r1, r3
    11de:	6581      	str	r1, [r0, #88]	; 0x58
    11e0:	4770      	bx	lr

000011e2 <LOGUART_AGGPathAllCmd>:
    11e2:	b510      	push	{r4, lr}
    11e4:	6d82      	ldr	r2, [r0, #88]	; 0x58
    11e6:	b139      	cbz	r1, 11f8 <LOGUART_AGGPathAllCmd+0x16>
    11e8:	23f8      	movs	r3, #248	; 0xf8
    11ea:	061b      	lsls	r3, r3, #24
    11ec:	4313      	orrs	r3, r2
    11ee:	6583      	str	r3, [r0, #88]	; 0x58
    11f0:	2001      	movs	r0, #1
    11f2:	4b03      	ldr	r3, [pc, #12]	; (1200 <LOGUART_AGGPathAllCmd+0x1e>)
    11f4:	4798      	blx	r3
    11f6:	bd10      	pop	{r4, pc}
    11f8:	0153      	lsls	r3, r2, #5
    11fa:	095b      	lsrs	r3, r3, #5
    11fc:	e7f7      	b.n	11ee <LOGUART_AGGPathAllCmd+0xc>
    11fe:	46c0      	nop			; (mov r8, r8)
    1200:	0000030b 	.word	0x0000030b

00001204 <LOGUART_PutChar>:
    1204:	4b11      	ldr	r3, [pc, #68]	; (124c <LOGUART_PutChar+0x48>)
    1206:	4a12      	ldr	r2, [pc, #72]	; (1250 <LOGUART_PutChar+0x4c>)
    1208:	681b      	ldr	r3, [r3, #0]
    120a:	6812      	ldr	r2, [r2, #0]
    120c:	061b      	lsls	r3, r3, #24
    120e:	b5f0      	push	{r4, r5, r6, r7, lr}
    1210:	0f9b      	lsrs	r3, r3, #30
    1212:	2a01      	cmp	r2, #1
    1214:	d019      	beq.n	124a <LOGUART_PutChar+0x46>
    1216:	2214      	movs	r2, #20
    1218:	435a      	muls	r2, r3
    121a:	490e      	ldr	r1, [pc, #56]	; (1254 <LOGUART_PutChar+0x50>)
    121c:	4d0e      	ldr	r5, [pc, #56]	; (1258 <LOGUART_PutChar+0x54>)
    121e:	1852      	adds	r2, r2, r1
    1220:	2100      	movs	r1, #0
    1222:	68d6      	ldr	r6, [r2, #12]
    1224:	3101      	adds	r1, #1
    1226:	428e      	cmp	r6, r1
    1228:	d903      	bls.n	1232 <LOGUART_PutChar+0x2e>
    122a:	696c      	ldr	r4, [r5, #20]
    122c:	6857      	ldr	r7, [r2, #4]
    122e:	4227      	tst	r7, r4
    1230:	d0f8      	beq.n	1224 <LOGUART_PutChar+0x20>
    1232:	2214      	movs	r2, #20
    1234:	4353      	muls	r3, r2
    1236:	4a07      	ldr	r2, [pc, #28]	; (1254 <LOGUART_PutChar+0x50>)
    1238:	589b      	ldr	r3, [r3, r2]
    123a:	4a07      	ldr	r2, [pc, #28]	; (1258 <LOGUART_PutChar+0x54>)
    123c:	009b      	lsls	r3, r3, #2
    123e:	189b      	adds	r3, r3, r2
    1240:	65d8      	str	r0, [r3, #92]	; 0x5c
    1242:	280a      	cmp	r0, #10
    1244:	d101      	bne.n	124a <LOGUART_PutChar+0x46>
    1246:	220d      	movs	r2, #13
    1248:	65da      	str	r2, [r3, #92]	; 0x5c
    124a:	bdf0      	pop	{r4, r5, r6, r7, pc}
    124c:	4200827c 	.word	0x4200827c
    1250:	230005cc 	.word	0x230005cc
    1254:	0000c074 	.word	0x0000c074
    1258:	4200c000 	.word	0x4200c000

0000125c <LOGUART_GetChar>:
    125c:	4b04      	ldr	r3, [pc, #16]	; (1270 <LOGUART_GetChar+0x14>)
    125e:	b118      	cbz	r0, 1268 <LOGUART_GetChar+0xc>
    1260:	2201      	movs	r2, #1
    1262:	6959      	ldr	r1, [r3, #20]
    1264:	4211      	tst	r1, r2
    1266:	d0fc      	beq.n	1262 <LOGUART_GetChar+0x6>
    1268:	6a58      	ldr	r0, [r3, #36]	; 0x24
    126a:	b2c0      	uxtb	r0, r0
    126c:	4770      	bx	lr
    126e:	46c0      	nop			; (mov r8, r8)
    1270:	4200c000 	.word	0x4200c000

00001274 <LOGUART_Readable>:
    1274:	4b02      	ldr	r3, [pc, #8]	; (1280 <LOGUART_Readable+0xc>)
    1276:	6958      	ldr	r0, [r3, #20]
    1278:	2301      	movs	r3, #1
    127a:	4018      	ands	r0, r3
    127c:	4770      	bx	lr
    127e:	46c0      	nop			; (mov r8, r8)
    1280:	4200c000 	.word	0x4200c000

00001284 <LOGUART_GetRxCount>:
    1284:	4b01      	ldr	r3, [pc, #4]	; (128c <LOGUART_GetRxCount+0x8>)
    1286:	6958      	ldr	r0, [r3, #20]
    1288:	0e00      	lsrs	r0, r0, #24
    128a:	4770      	bx	lr
    128c:	4200c000 	.word	0x4200c000

00001290 <LOGUART_GetIMR>:
    1290:	4b01      	ldr	r3, [pc, #4]	; (1298 <LOGUART_GetIMR+0x8>)
    1292:	6858      	ldr	r0, [r3, #4]
    1294:	4770      	bx	lr
    1296:	46c0      	nop			; (mov r8, r8)
    1298:	4200c000 	.word	0x4200c000

0000129c <LOGUART_SetIMR>:
    129c:	4b01      	ldr	r3, [pc, #4]	; (12a4 <LOGUART_SetIMR+0x8>)
    129e:	6058      	str	r0, [r3, #4]
    12a0:	4770      	bx	lr
    12a2:	46c0      	nop			; (mov r8, r8)
    12a4:	4200c000 	.word	0x4200c000

000012a8 <LOGUART_INTConfig>:
    12a8:	6843      	ldr	r3, [r0, #4]
    12aa:	2a01      	cmp	r2, #1
    12ac:	d102      	bne.n	12b4 <LOGUART_INTConfig+0xc>
    12ae:	4319      	orrs	r1, r3
    12b0:	6041      	str	r1, [r0, #4]
    12b2:	4770      	bx	lr
    12b4:	438b      	bics	r3, r1
    12b6:	6043      	str	r3, [r0, #4]
    12b8:	e7fb      	b.n	12b2 <LOGUART_INTConfig+0xa>

000012ba <LOGUART_INTClear>:
    12ba:	6f43      	ldr	r3, [r0, #116]	; 0x74
    12bc:	430b      	orrs	r3, r1
    12be:	6743      	str	r3, [r0, #116]	; 0x74
    12c0:	4770      	bx	lr

000012c2 <LOGUART_GetStatus>:
    12c2:	6940      	ldr	r0, [r0, #20]
    12c4:	4770      	bx	lr

000012c6 <LOGUART_ClearRxFifo>:
    12c6:	2202      	movs	r2, #2
    12c8:	6d41      	ldr	r1, [r0, #84]	; 0x54
    12ca:	0003      	movs	r3, r0
    12cc:	430a      	orrs	r2, r1
    12ce:	6542      	str	r2, [r0, #84]	; 0x54
    12d0:	2005      	movs	r0, #5
    12d2:	2201      	movs	r2, #1
    12d4:	6959      	ldr	r1, [r3, #20]
    12d6:	4211      	tst	r1, r2
    12d8:	d003      	beq.n	12e2 <LOGUART_ClearRxFifo+0x1c>
    12da:	3801      	subs	r0, #1
    12dc:	2800      	cmp	r0, #0
    12de:	d1f9      	bne.n	12d4 <LOGUART_ClearRxFifo+0xe>
    12e0:	4770      	bx	lr
    12e2:	2001      	movs	r0, #1
    12e4:	e7fc      	b.n	12e0 <LOGUART_ClearRxFifo+0x1a>

000012e6 <_LOGUART_Init>:
    12e6:	23c0      	movs	r3, #192	; 0xc0
    12e8:	69ca      	ldr	r2, [r1, #28]
    12ea:	b570      	push	{r4, r5, r6, lr}
    12ec:	439a      	bics	r2, r3
    12ee:	0005      	movs	r5, r0
    12f0:	000c      	movs	r4, r1
    12f2:	b11a      	cbz	r2, 12fc <_LOGUART_Init+0x16>
    12f4:	214c      	movs	r1, #76	; 0x4c
    12f6:	4851      	ldr	r0, [pc, #324]	; (143c <_LOGUART_Init+0x156>)
    12f8:	f7fe ff0c 	bl	114 <io_assert_failed>
    12fc:	6a23      	ldr	r3, [r4, #32]
    12fe:	2b01      	cmp	r3, #1
    1300:	d903      	bls.n	130a <_LOGUART_Init+0x24>
    1302:	214d      	movs	r1, #77	; 0x4d
    1304:	484d      	ldr	r0, [pc, #308]	; (143c <_LOGUART_Init+0x156>)
    1306:	f7fe ff05 	bl	114 <io_assert_failed>
    130a:	6c23      	ldr	r3, [r4, #64]	; 0x40
    130c:	2b07      	cmp	r3, #7
    130e:	d903      	bls.n	1318 <_LOGUART_Init+0x32>
    1310:	214e      	movs	r1, #78	; 0x4e
    1312:	484a      	ldr	r0, [pc, #296]	; (143c <_LOGUART_Init+0x156>)
    1314:	f7fe fefe 	bl	114 <io_assert_failed>
    1318:	2204      	movs	r2, #4
    131a:	6c2b      	ldr	r3, [r5, #64]	; 0x40
    131c:	0028      	movs	r0, r5
    131e:	4393      	bics	r3, r2
    1320:	642b      	str	r3, [r5, #64]	; 0x40
    1322:	4b47      	ldr	r3, [pc, #284]	; (1440 <_LOGUART_Init+0x15a>)
    1324:	4798      	blx	r3
    1326:	2300      	movs	r3, #0
    1328:	606b      	str	r3, [r5, #4]
    132a:	69ea      	ldr	r2, [r5, #28]
    132c:	3380      	adds	r3, #128	; 0x80
    132e:	4313      	orrs	r3, r2
    1330:	22c1      	movs	r2, #193	; 0xc1
    1332:	61eb      	str	r3, [r5, #28]
    1334:	6d6b      	ldr	r3, [r5, #84]	; 0x54
    1336:	4393      	bics	r3, r2
    1338:	69e2      	ldr	r2, [r4, #28]
    133a:	4313      	orrs	r3, r2
    133c:	6a22      	ldr	r2, [r4, #32]
    133e:	4313      	orrs	r3, r2
    1340:	69a2      	ldr	r2, [r4, #24]
    1342:	656b      	str	r3, [r5, #84]	; 0x54
    1344:	2321      	movs	r3, #33	; 0x21
    1346:	2a01      	cmp	r2, #1
    1348:	d15e      	bne.n	1408 <_LOGUART_Init+0x122>
    134a:	692a      	ldr	r2, [r5, #16]
    134c:	4313      	orrs	r3, r2
    134e:	612b      	str	r3, [r5, #16]
    1350:	2204      	movs	r2, #4
    1352:	2108      	movs	r1, #8
    1354:	2010      	movs	r0, #16
    1356:	68a3      	ldr	r3, [r4, #8]
    1358:	009b      	lsls	r3, r3, #2
    135a:	4013      	ands	r3, r2
    135c:	68e2      	ldr	r2, [r4, #12]
    135e:	00d2      	lsls	r2, r2, #3
    1360:	400a      	ands	r2, r1
    1362:	4313      	orrs	r3, r2
    1364:	6862      	ldr	r2, [r4, #4]
    1366:	4313      	orrs	r3, r2
    1368:	6922      	ldr	r2, [r4, #16]
    136a:	0112      	lsls	r2, r2, #4
    136c:	4002      	ands	r2, r0
    136e:	4313      	orrs	r3, r2
    1370:	6962      	ldr	r2, [r4, #20]
    1372:	1800      	adds	r0, r0, r0
    1374:	0152      	lsls	r2, r2, #5
    1376:	4002      	ands	r2, r0
    1378:	4313      	orrs	r3, r2
    137a:	60eb      	str	r3, [r5, #12]
    137c:	6823      	ldr	r3, [r4, #0]
    137e:	2b00      	cmp	r3, #0
    1380:	d046      	beq.n	1410 <_LOGUART_Init+0x12a>
    1382:	6d6b      	ldr	r3, [r5, #84]	; 0x54
    1384:	4319      	orrs	r1, r3
    1386:	6569      	str	r1, [r5, #84]	; 0x54
    1388:	6c2a      	ldr	r2, [r5, #64]	; 0x40
    138a:	6a63      	ldr	r3, [r4, #36]	; 0x24
    138c:	b292      	uxth	r2, r2
    138e:	041b      	lsls	r3, r3, #16
    1390:	4313      	orrs	r3, r2
    1392:	6aa2      	ldr	r2, [r4, #40]	; 0x28
    1394:	642b      	str	r3, [r5, #64]	; 0x40
    1396:	6dab      	ldr	r3, [r5, #88]	; 0x58
    1398:	2a00      	cmp	r2, #0
    139a:	d03d      	beq.n	1418 <_LOGUART_Init+0x132>
    139c:	2280      	movs	r2, #128	; 0x80
    139e:	04d2      	lsls	r2, r2, #19
    13a0:	4313      	orrs	r3, r2
    13a2:	6ae2      	ldr	r2, [r4, #44]	; 0x2c
    13a4:	2a00      	cmp	r2, #0
    13a6:	d03a      	beq.n	141e <_LOGUART_Init+0x138>
    13a8:	2280      	movs	r2, #128	; 0x80
    13aa:	0512      	lsls	r2, r2, #20
    13ac:	4313      	orrs	r3, r2
    13ae:	6b22      	ldr	r2, [r4, #48]	; 0x30
    13b0:	2a00      	cmp	r2, #0
    13b2:	d037      	beq.n	1424 <_LOGUART_Init+0x13e>
    13b4:	2280      	movs	r2, #128	; 0x80
    13b6:	0552      	lsls	r2, r2, #21
    13b8:	4313      	orrs	r3, r2
    13ba:	6b62      	ldr	r2, [r4, #52]	; 0x34
    13bc:	2a00      	cmp	r2, #0
    13be:	d034      	beq.n	142a <_LOGUART_Init+0x144>
    13c0:	2280      	movs	r2, #128	; 0x80
    13c2:	0592      	lsls	r2, r2, #22
    13c4:	4313      	orrs	r3, r2
    13c6:	6ba2      	ldr	r2, [r4, #56]	; 0x38
    13c8:	2a00      	cmp	r2, #0
    13ca:	d031      	beq.n	1430 <_LOGUART_Init+0x14a>
    13cc:	2280      	movs	r2, #128	; 0x80
    13ce:	05d2      	lsls	r2, r2, #23
    13d0:	4313      	orrs	r3, r2
    13d2:	6be2      	ldr	r2, [r4, #60]	; 0x3c
    13d4:	2a00      	cmp	r2, #0
    13d6:	d02e      	beq.n	1436 <_LOGUART_Init+0x150>
    13d8:	2280      	movs	r2, #128	; 0x80
    13da:	0612      	lsls	r2, r2, #24
    13dc:	4313      	orrs	r3, r2
    13de:	4a19      	ldr	r2, [pc, #100]	; (1444 <_LOGUART_Init+0x15e>)
    13e0:	4919      	ldr	r1, [pc, #100]	; (1448 <_LOGUART_Init+0x162>)
    13e2:	4013      	ands	r3, r2
    13e4:	6c62      	ldr	r2, [r4, #68]	; 0x44
    13e6:	0312      	lsls	r2, r2, #12
    13e8:	400a      	ands	r2, r1
    13ea:	f640 7100 	movw	r1, #3840	; 0xf00
    13ee:	4313      	orrs	r3, r2
    13f0:	4a16      	ldr	r2, [pc, #88]	; (144c <_LOGUART_Init+0x166>)
    13f2:	401a      	ands	r2, r3
    13f4:	6c23      	ldr	r3, [r4, #64]	; 0x40
    13f6:	021b      	lsls	r3, r3, #8
    13f8:	400b      	ands	r3, r1
    13fa:	4313      	orrs	r3, r2
    13fc:	22ff      	movs	r2, #255	; 0xff
    13fe:	4393      	bics	r3, r2
    1400:	6ca2      	ldr	r2, [r4, #72]	; 0x48
    1402:	4313      	orrs	r3, r2
    1404:	65ab      	str	r3, [r5, #88]	; 0x58
    1406:	bd70      	pop	{r4, r5, r6, pc}
    1408:	692a      	ldr	r2, [r5, #16]
    140a:	439a      	bics	r2, r3
    140c:	612a      	str	r2, [r5, #16]
    140e:	e79f      	b.n	1350 <_LOGUART_Init+0x6a>
    1410:	6d6b      	ldr	r3, [r5, #84]	; 0x54
    1412:	438b      	bics	r3, r1
    1414:	656b      	str	r3, [r5, #84]	; 0x54
    1416:	e7b7      	b.n	1388 <_LOGUART_Init+0xa2>
    1418:	4a0d      	ldr	r2, [pc, #52]	; (1450 <_LOGUART_Init+0x16a>)
    141a:	4013      	ands	r3, r2
    141c:	e7c1      	b.n	13a2 <_LOGUART_Init+0xbc>
    141e:	4a0d      	ldr	r2, [pc, #52]	; (1454 <_LOGUART_Init+0x16e>)
    1420:	4013      	ands	r3, r2
    1422:	e7c4      	b.n	13ae <_LOGUART_Init+0xc8>
    1424:	4a0c      	ldr	r2, [pc, #48]	; (1458 <_LOGUART_Init+0x172>)
    1426:	4013      	ands	r3, r2
    1428:	e7c7      	b.n	13ba <_LOGUART_Init+0xd4>
    142a:	4a0c      	ldr	r2, [pc, #48]	; (145c <_LOGUART_Init+0x176>)
    142c:	4013      	ands	r3, r2
    142e:	e7ca      	b.n	13c6 <_LOGUART_Init+0xe0>
    1430:	4a0b      	ldr	r2, [pc, #44]	; (1460 <_LOGUART_Init+0x17a>)
    1432:	4013      	ands	r3, r2
    1434:	e7cd      	b.n	13d2 <_LOGUART_Init+0xec>
    1436:	005b      	lsls	r3, r3, #1
    1438:	085b      	lsrs	r3, r3, #1
    143a:	e7d0      	b.n	13de <_LOGUART_Init+0xf8>
    143c:	0000c469 	.word	0x0000c469
    1440:	000012c7 	.word	0x000012c7
    1444:	fc000fff 	.word	0xfc000fff
    1448:	03fff000 	.word	0x03fff000
    144c:	fffff0ff 	.word	0xfffff0ff
    1450:	fbffffff 	.word	0xfbffffff
    1454:	f7ffffff 	.word	0xf7ffffff
    1458:	efffffff 	.word	0xefffffff
    145c:	dfffffff 	.word	0xdfffffff
    1460:	bfffffff 	.word	0xbfffffff

00001464 <LOGUART_Init>:
    1464:	0001      	movs	r1, r0
    1466:	b510      	push	{r4, lr}
    1468:	480e      	ldr	r0, [pc, #56]	; (14a4 <LOGUART_Init+0x40>)
    146a:	f7ff ff3c 	bl	12e6 <_LOGUART_Init>
    146e:	4b0e      	ldr	r3, [pc, #56]	; (14a8 <LOGUART_Init+0x44>)
    1470:	4798      	blx	r3
    1472:	4c0e      	ldr	r4, [pc, #56]	; (14ac <LOGUART_Init+0x48>)
    1474:	2802      	cmp	r0, #2
    1476:	d10c      	bne.n	1492 <LOGUART_Init+0x2e>
    1478:	490d      	ldr	r1, [pc, #52]	; (14b0 <LOGUART_Init+0x4c>)
    147a:	480a      	ldr	r0, [pc, #40]	; (14a4 <LOGUART_Init+0x40>)
    147c:	47a0      	blx	r4
    147e:	2201      	movs	r2, #1
    1480:	4808      	ldr	r0, [pc, #32]	; (14a4 <LOGUART_Init+0x40>)
    1482:	0011      	movs	r1, r2
    1484:	4b0b      	ldr	r3, [pc, #44]	; (14b4 <LOGUART_Init+0x50>)
    1486:	4798      	blx	r3
    1488:	2101      	movs	r1, #1
    148a:	4806      	ldr	r0, [pc, #24]	; (14a4 <LOGUART_Init+0x40>)
    148c:	4b0a      	ldr	r3, [pc, #40]	; (14b8 <LOGUART_Init+0x54>)
    148e:	4798      	blx	r3
    1490:	bd10      	pop	{r4, pc}
    1492:	4b0a      	ldr	r3, [pc, #40]	; (14bc <LOGUART_Init+0x58>)
    1494:	4798      	blx	r3
    1496:	2801      	cmp	r0, #1
    1498:	d102      	bne.n	14a0 <LOGUART_Init+0x3c>
    149a:	21e1      	movs	r1, #225	; 0xe1
    149c:	0249      	lsls	r1, r1, #9
    149e:	e7ec      	b.n	147a <LOGUART_Init+0x16>
    14a0:	4907      	ldr	r1, [pc, #28]	; (14c0 <LOGUART_Init+0x5c>)
    14a2:	e7ea      	b.n	147a <LOGUART_Init+0x16>
    14a4:	4200c000 	.word	0x4200c000
    14a8:	00001dbd 	.word	0x00001dbd
    14ac:	0000102b 	.word	0x0000102b
    14b0:	001e8480 	.word	0x001e8480
    14b4:	000012a9 	.word	0x000012a9
    14b8:	000010c9 	.word	0x000010c9
    14bc:	00001e2d 	.word	0x00001e2d
    14c0:	0016e360 	.word	0x0016e360

000014c4 <LOGUART_WaitBusy>:
    14c4:	4b09      	ldr	r3, [pc, #36]	; (14ec <LOGUART_WaitBusy+0x28>)
    14c6:	b510      	push	{r4, lr}
    14c8:	681c      	ldr	r4, [r3, #0]
    14ca:	0624      	lsls	r4, r4, #24
    14cc:	0fa3      	lsrs	r3, r4, #30
    14ce:	2414      	movs	r4, #20
    14d0:	435c      	muls	r4, r3
    14d2:	4b07      	ldr	r3, [pc, #28]	; (14f0 <LOGUART_WaitBusy+0x2c>)
    14d4:	18e4      	adds	r4, r4, r3
    14d6:	4b07      	ldr	r3, [pc, #28]	; (14f4 <LOGUART_WaitBusy+0x30>)
    14d8:	68a2      	ldr	r2, [r4, #8]
    14da:	695b      	ldr	r3, [r3, #20]
    14dc:	421a      	tst	r2, r3
    14de:	d103      	bne.n	14e8 <LOGUART_WaitBusy+0x24>
    14e0:	2064      	movs	r0, #100	; 0x64
    14e2:	4b05      	ldr	r3, [pc, #20]	; (14f8 <LOGUART_WaitBusy+0x34>)
    14e4:	4798      	blx	r3
    14e6:	e7f6      	b.n	14d6 <LOGUART_WaitBusy+0x12>
    14e8:	bd10      	pop	{r4, pc}
    14ea:	46c0      	nop			; (mov r8, r8)
    14ec:	4200827c 	.word	0x4200827c
    14f0:	0000c074 	.word	0x0000c074
    14f4:	4200c000 	.word	0x4200c000
    14f8:	0000030b 	.word	0x0000030b

000014fc <LOGUART_WaitTxComplete>:
    14fc:	b510      	push	{r4, lr}
    14fe:	4b07      	ldr	r3, [pc, #28]	; (151c <LOGUART_WaitTxComplete+0x20>)
    1500:	4a07      	ldr	r2, [pc, #28]	; (1520 <LOGUART_WaitTxComplete+0x24>)
    1502:	695b      	ldr	r3, [r3, #20]
    1504:	4013      	ands	r3, r2
    1506:	4a07      	ldr	r2, [pc, #28]	; (1524 <LOGUART_WaitTxComplete+0x28>)
    1508:	4293      	cmp	r3, r2
    150a:	d003      	beq.n	1514 <LOGUART_WaitTxComplete+0x18>
    150c:	2064      	movs	r0, #100	; 0x64
    150e:	4b06      	ldr	r3, [pc, #24]	; (1528 <LOGUART_WaitTxComplete+0x2c>)
    1510:	4798      	blx	r3
    1512:	e7f4      	b.n	14fe <LOGUART_WaitTxComplete+0x2>
    1514:	2003      	movs	r0, #3
    1516:	4b05      	ldr	r3, [pc, #20]	; (152c <LOGUART_WaitTxComplete+0x30>)
    1518:	4798      	blx	r3
    151a:	bd10      	pop	{r4, pc}
    151c:	4200c000 	.word	0x4200c000
    1520:	000f0140 	.word	0x000f0140
    1524:	000f0040 	.word	0x000f0040
    1528:	0000030b 	.word	0x0000030b
    152c:	00000381 	.word	0x00000381

00001530 <LOGUART_BT_SendData>:
    1530:	b570      	push	{r4, r5, r6, lr}
    1532:	2580      	movs	r5, #128	; 0x80
    1534:	2300      	movs	r3, #0
    1536:	4c08      	ldr	r4, [pc, #32]	; (1558 <LOGUART_BT_SendData+0x28>)
    1538:	4a08      	ldr	r2, [pc, #32]	; (155c <LOGUART_BT_SendData+0x2c>)
    153a:	1841      	adds	r1, r0, r1
    153c:	03ed      	lsls	r5, r5, #15
    153e:	4288      	cmp	r0, r1
    1540:	d100      	bne.n	1544 <LOGUART_BT_SendData+0x14>
    1542:	bd70      	pop	{r4, r5, r6, pc}
    1544:	3301      	adds	r3, #1
    1546:	42a3      	cmp	r3, r4
    1548:	d802      	bhi.n	1550 <LOGUART_BT_SendData+0x20>
    154a:	6956      	ldr	r6, [r2, #20]
    154c:	422e      	tst	r6, r5
    154e:	d0f9      	beq.n	1544 <LOGUART_BT_SendData+0x14>
    1550:	7806      	ldrb	r6, [r0, #0]
    1552:	3001      	adds	r0, #1
    1554:	6656      	str	r6, [r2, #100]	; 0x64
    1556:	e7f2      	b.n	153e <LOGUART_BT_SendData+0xe>
    1558:	000bee32 	.word	0x000bee32
    155c:	4200c000 	.word	0x4200c000

00001560 <LOGUART_Relay_StructInit>:
    1560:	2300      	movs	r3, #0
    1562:	2201      	movs	r2, #1
    1564:	6043      	str	r3, [r0, #4]
    1566:	6002      	str	r2, [r0, #0]
    1568:	6083      	str	r3, [r0, #8]
    156a:	60c3      	str	r3, [r0, #12]
    156c:	6103      	str	r3, [r0, #16]
    156e:	6142      	str	r2, [r0, #20]
    1570:	4770      	bx	lr

00001572 <LOGUART_Relay_SetFormat>:
    1572:	2204      	movs	r2, #4
    1574:	b510      	push	{r4, lr}
    1576:	2408      	movs	r4, #8
    1578:	684b      	ldr	r3, [r1, #4]
    157a:	009b      	lsls	r3, r3, #2
    157c:	4013      	ands	r3, r2
    157e:	688a      	ldr	r2, [r1, #8]
    1580:	00d2      	lsls	r2, r2, #3
    1582:	4022      	ands	r2, r4
    1584:	4313      	orrs	r3, r2
    1586:	680a      	ldr	r2, [r1, #0]
    1588:	1924      	adds	r4, r4, r4
    158a:	4313      	orrs	r3, r2
    158c:	68ca      	ldr	r2, [r1, #12]
    158e:	0112      	lsls	r2, r2, #4
    1590:	4022      	ands	r2, r4
    1592:	4313      	orrs	r3, r2
    1594:	690a      	ldr	r2, [r1, #16]
    1596:	3410      	adds	r4, #16
    1598:	0152      	lsls	r2, r2, #5
    159a:	4022      	ands	r2, r4
    159c:	4313      	orrs	r3, r2
    159e:	f241 0200 	movw	r2, #4096	; 0x1000
    15a2:	66c3      	str	r3, [r0, #108]	; 0x6c
    15a4:	6d43      	ldr	r3, [r0, #84]	; 0x54
    15a6:	4c05      	ldr	r4, [pc, #20]	; (15bc <LOGUART_Relay_SetFormat+0x4a>)
    15a8:	401c      	ands	r4, r3
    15aa:	694b      	ldr	r3, [r1, #20]
    15ac:	031b      	lsls	r3, r3, #12
    15ae:	4013      	ands	r3, r2
    15b0:	2202      	movs	r2, #2
    15b2:	4323      	orrs	r3, r4
    15b4:	4393      	bics	r3, r2
    15b6:	6543      	str	r3, [r0, #84]	; 0x54
    15b8:	bd10      	pop	{r4, pc}
    15ba:	46c0      	nop			; (mov r8, r8)
    15bc:	ffffefff 	.word	0xffffefff

000015c0 <LOGUART_Relay_SetBaud>:
    15c0:	b537      	push	{r0, r1, r2, r4, r5, lr}
    15c2:	000d      	movs	r5, r1
    15c4:	0004      	movs	r4, r0
    15c6:	4b0f      	ldr	r3, [pc, #60]	; (1604 <LOGUART_Relay_SetBaud+0x44>)
    15c8:	4798      	blx	r3
    15ca:	466a      	mov	r2, sp
    15cc:	0029      	movs	r1, r5
    15ce:	ab01      	add	r3, sp, #4
    15d0:	4d0d      	ldr	r5, [pc, #52]	; (1608 <LOGUART_Relay_SetBaud+0x48>)
    15d2:	47a8      	blx	r5
    15d4:	2180      	movs	r1, #128	; 0x80
    15d6:	6ee3      	ldr	r3, [r4, #108]	; 0x6c
    15d8:	430b      	orrs	r3, r1
    15da:	66e3      	str	r3, [r4, #108]	; 0x6c
    15dc:	6ee2      	ldr	r2, [r4, #108]	; 0x6c
    15de:	9b00      	ldr	r3, [sp, #0]
    15e0:	0512      	lsls	r2, r2, #20
    15e2:	0318      	lsls	r0, r3, #12
    15e4:	0d12      	lsrs	r2, r2, #20
    15e6:	4302      	orrs	r2, r0
    15e8:	66e2      	str	r2, [r4, #108]	; 0x6c
    15ea:	6f22      	ldr	r2, [r4, #112]	; 0x70
    15ec:	4807      	ldr	r0, [pc, #28]	; (160c <LOGUART_Relay_SetBaud+0x4c>)
    15ee:	00db      	lsls	r3, r3, #3
    15f0:	4002      	ands	r2, r0
    15f2:	f643 70f8 	movw	r0, #16376	; 0x3ff8
    15f6:	4003      	ands	r3, r0
    15f8:	4313      	orrs	r3, r2
    15fa:	6723      	str	r3, [r4, #112]	; 0x70
    15fc:	6ee3      	ldr	r3, [r4, #108]	; 0x6c
    15fe:	438b      	bics	r3, r1
    1600:	66e3      	str	r3, [r4, #108]	; 0x6c
    1602:	bd37      	pop	{r0, r1, r2, r4, r5, pc}
    1604:	000001c9 	.word	0x000001c9
    1608:	00002e4b 	.word	0x00002e4b
    160c:	ffffc007 	.word	0xffffc007

00001610 <LOGUART_Relay_ClearRxFifo>:
    1610:	2102      	movs	r1, #2
    1612:	6d42      	ldr	r2, [r0, #84]	; 0x54
    1614:	0003      	movs	r3, r0
    1616:	438a      	bics	r2, r1
    1618:	0011      	movs	r1, r2
    161a:	f242 0200 	movw	r2, #8192	; 0x2000
    161e:	430a      	orrs	r2, r1
    1620:	6542      	str	r2, [r0, #84]	; 0x54
    1622:	2005      	movs	r0, #5
    1624:	f240 1200 	movw	r2, #256	; 0x100
    1628:	6959      	ldr	r1, [r3, #20]
    162a:	4211      	tst	r1, r2
    162c:	d003      	beq.n	1636 <LOGUART_Relay_ClearRxFifo+0x26>
    162e:	3801      	subs	r0, #1
    1630:	2800      	cmp	r0, #0
    1632:	d1f9      	bne.n	1628 <LOGUART_Relay_ClearRxFifo+0x18>
    1634:	4770      	bx	lr
    1636:	2001      	movs	r0, #1
    1638:	e7fc      	b.n	1634 <LOGUART_Relay_ClearRxFifo+0x24>

0000163a <LOGUART_Relay_RxCmd>:
    163a:	2304      	movs	r3, #4
    163c:	6f02      	ldr	r2, [r0, #112]	; 0x70
    163e:	b111      	cbz	r1, 1646 <LOGUART_Relay_RxCmd+0xc>
    1640:	4313      	orrs	r3, r2
    1642:	6703      	str	r3, [r0, #112]	; 0x70
    1644:	4770      	bx	lr
    1646:	439a      	bics	r2, r3
    1648:	6702      	str	r2, [r0, #112]	; 0x70
    164a:	e7fb      	b.n	1644 <LOGUART_Relay_RxCmd+0xa>

0000164c <LOGUART_MonitorParaConfig>:
    164c:	1e4b      	subs	r3, r1, #1
    164e:	b570      	push	{r4, r5, r6, lr}
    1650:	0005      	movs	r5, r0
    1652:	000c      	movs	r4, r1
    1654:	0016      	movs	r6, r2
    1656:	2b7e      	cmp	r3, #126	; 0x7e
    1658:	d904      	bls.n	1664 <LOGUART_MonitorParaConfig+0x18>
    165a:	f240 317e 	movw	r1, #894	; 0x37e
    165e:	480c      	ldr	r0, [pc, #48]	; (1690 <LOGUART_MonitorParaConfig+0x44>)
    1660:	f7fe fd58 	bl	114 <io_assert_failed>
    1664:	2204      	movs	r2, #4
    1666:	6c2b      	ldr	r3, [r5, #64]	; 0x40
    1668:	490a      	ldr	r1, [pc, #40]	; (1694 <LOGUART_MonitorParaConfig+0x48>)
    166a:	4393      	bics	r3, r2
    166c:	642b      	str	r3, [r5, #64]	; 0x40
    166e:	6c6b      	ldr	r3, [r5, #68]	; 0x44
    1670:	0064      	lsls	r4, r4, #1
    1672:	400b      	ands	r3, r1
    1674:	f240 11fe 	movw	r1, #510	; 0x1fe
    1678:	400c      	ands	r4, r1
    167a:	431c      	orrs	r4, r3
    167c:	b126      	cbz	r6, 1688 <LOGUART_MonitorParaConfig+0x3c>
    167e:	2180      	movs	r1, #128	; 0x80
    1680:	0589      	lsls	r1, r1, #22
    1682:	430c      	orrs	r4, r1
    1684:	646c      	str	r4, [r5, #68]	; 0x44
    1686:	bd70      	pop	{r4, r5, r6, pc}
    1688:	4903      	ldr	r1, [pc, #12]	; (1698 <LOGUART_MonitorParaConfig+0x4c>)
    168a:	400c      	ands	r4, r1
    168c:	e7fa      	b.n	1684 <LOGUART_MonitorParaConfig+0x38>
    168e:	46c0      	nop			; (mov r8, r8)
    1690:	0000c424 	.word	0x0000c424
    1694:	fffffe01 	.word	0xfffffe01
    1698:	dfffffff 	.word	0xdfffffff

0000169c <LOGUART_LPBaudSet>:
    169c:	4b20      	ldr	r3, [pc, #128]	; (1720 <LOGUART_LPBaudSet+0x84>)
    169e:	b570      	push	{r4, r5, r6, lr}
    16a0:	0016      	movs	r6, r2
    16a2:	1e4a      	subs	r2, r1, #1
    16a4:	0004      	movs	r4, r0
    16a6:	000d      	movs	r5, r1
    16a8:	429a      	cmp	r2, r3
    16aa:	d904      	bls.n	16b6 <LOGUART_LPBaudSet+0x1a>
    16ac:	f240 31a6 	movw	r1, #934	; 0x3a6
    16b0:	481c      	ldr	r0, [pc, #112]	; (1724 <LOGUART_LPBaudSet+0x88>)
    16b2:	f7fe fd2f 	bl	114 <io_assert_failed>
    16b6:	fbb6 f3f5 	udiv	r3, r6, r5
    16ba:	002a      	movs	r2, r5
    16bc:	2100      	movs	r1, #0
    16be:	435a      	muls	r2, r3
    16c0:	3501      	adds	r5, #1
    16c2:	1ab6      	subs	r6, r6, r2
    16c4:	086d      	lsrs	r5, r5, #1
    16c6:	42ae      	cmp	r6, r5
    16c8:	4149      	adcs	r1, r1
    16ca:	185b      	adds	r3, r3, r1
    16cc:	2180      	movs	r1, #128	; 0x80
    16ce:	68e2      	ldr	r2, [r4, #12]
    16d0:	4815      	ldr	r0, [pc, #84]	; (1728 <LOGUART_LPBaudSet+0x8c>)
    16d2:	430a      	orrs	r2, r1
    16d4:	60e2      	str	r2, [r4, #12]
    16d6:	69e2      	ldr	r2, [r4, #28]
    16d8:	4d14      	ldr	r5, [pc, #80]	; (172c <LOGUART_LPBaudSet+0x90>)
    16da:	430a      	orrs	r2, r1
    16dc:	61e2      	str	r2, [r4, #28]
    16de:	6a22      	ldr	r2, [r4, #32]
    16e0:	4010      	ands	r0, r2
    16e2:	011a      	lsls	r2, r3, #4
    16e4:	402a      	ands	r2, r5
    16e6:	4302      	orrs	r2, r0
    16e8:	6222      	str	r2, [r4, #32]
    16ea:	69e2      	ldr	r2, [r4, #28]
    16ec:	4810      	ldr	r0, [pc, #64]	; (1730 <LOGUART_LPBaudSet+0x94>)
    16ee:	4002      	ands	r2, r0
    16f0:	61e2      	str	r2, [r4, #28]
    16f2:	68e2      	ldr	r2, [r4, #12]
    16f4:	438a      	bics	r2, r1
    16f6:	60e2      	str	r2, [r4, #12]
    16f8:	6ca2      	ldr	r2, [r4, #72]	; 0x48
    16fa:	0d12      	lsrs	r2, r2, #20
    16fc:	0511      	lsls	r1, r2, #20
    16fe:	031a      	lsls	r2, r3, #12
    1700:	0b12      	lsrs	r2, r2, #12
    1702:	430a      	orrs	r2, r1
    1704:	64a2      	str	r2, [r4, #72]	; 0x48
    1706:	6c62      	ldr	r2, [r4, #68]	; 0x44
    1708:	490a      	ldr	r1, [pc, #40]	; (1734 <LOGUART_LPBaudSet+0x98>)
    170a:	025b      	lsls	r3, r3, #9
    170c:	400a      	ands	r2, r1
    170e:	490a      	ldr	r1, [pc, #40]	; (1738 <LOGUART_LPBaudSet+0x9c>)
    1710:	400b      	ands	r3, r1
    1712:	4313      	orrs	r3, r2
    1714:	6463      	str	r3, [r4, #68]	; 0x44
    1716:	6c23      	ldr	r3, [r4, #64]	; 0x40
    1718:	4a08      	ldr	r2, [pc, #32]	; (173c <LOGUART_LPBaudSet+0xa0>)
    171a:	4013      	ands	r3, r2
    171c:	6423      	str	r3, [r4, #64]	; 0x40
    171e:	bd70      	pop	{r4, r5, r6, pc}
    1720:	0007a11f 	.word	0x0007a11f
    1724:	0000c412 	.word	0x0000c412
    1728:	ff00000f 	.word	0xff00000f
    172c:	00fffff0 	.word	0x00fffff0
    1730:	f800ffff 	.word	0xf800ffff
    1734:	e00001ff 	.word	0xe00001ff
    1738:	1ffffe00 	.word	0x1ffffe00
    173c:	ffffc007 	.word	0xffffc007

00001740 <LOGUART_Relay_LPSetBaud>:
    1740:	4b14      	ldr	r3, [pc, #80]	; (1794 <LOGUART_Relay_LPSetBaud+0x54>)
    1742:	b570      	push	{r4, r5, r6, lr}
    1744:	0016      	movs	r6, r2
    1746:	1e4a      	subs	r2, r1, #1
    1748:	0004      	movs	r4, r0
    174a:	000d      	movs	r5, r1
    174c:	429a      	cmp	r2, r3
    174e:	d904      	bls.n	175a <LOGUART_Relay_LPSetBaud+0x1a>
    1750:	f240 31e5 	movw	r1, #997	; 0x3e5
    1754:	4810      	ldr	r0, [pc, #64]	; (1798 <LOGUART_Relay_LPSetBaud+0x58>)
    1756:	f7fe fcdd 	bl	114 <io_assert_failed>
    175a:	fbb6 f1f5 	udiv	r1, r6, r5
    175e:	002a      	movs	r2, r5
    1760:	2300      	movs	r3, #0
    1762:	434a      	muls	r2, r1
    1764:	3501      	adds	r5, #1
    1766:	1ab6      	subs	r6, r6, r2
    1768:	086d      	lsrs	r5, r5, #1
    176a:	42ae      	cmp	r6, r5
    176c:	415b      	adcs	r3, r3
    176e:	2080      	movs	r0, #128	; 0x80
    1770:	18c9      	adds	r1, r1, r3
    1772:	6ee3      	ldr	r3, [r4, #108]	; 0x6c
    1774:	4303      	orrs	r3, r0
    1776:	66e3      	str	r3, [r4, #108]	; 0x6c
    1778:	6ee3      	ldr	r3, [r4, #108]	; 0x6c
    177a:	051b      	lsls	r3, r3, #20
    177c:	0d1a      	lsrs	r2, r3, #20
    177e:	030b      	lsls	r3, r1, #12
    1780:	4313      	orrs	r3, r2
    1782:	66e3      	str	r3, [r4, #108]	; 0x6c
    1784:	6f23      	ldr	r3, [r4, #112]	; 0x70
    1786:	4a05      	ldr	r2, [pc, #20]	; (179c <LOGUART_Relay_LPSetBaud+0x5c>)
    1788:	4013      	ands	r3, r2
    178a:	6723      	str	r3, [r4, #112]	; 0x70
    178c:	6ee3      	ldr	r3, [r4, #108]	; 0x6c
    178e:	4383      	bics	r3, r0
    1790:	66e3      	str	r3, [r4, #108]	; 0x6c
    1792:	bd70      	pop	{r4, r5, r6, pc}
    1794:	0007a11f 	.word	0x0007a11f
    1798:	0000c3fa 	.word	0x0000c3fa
    179c:	ffffc007 	.word	0xffffc007

000017a0 <LOGUART_RxMonitorCmd>:
    17a0:	2301      	movs	r3, #1
    17a2:	6c42      	ldr	r2, [r0, #68]	; 0x44
    17a4:	b111      	cbz	r1, 17ac <LOGUART_RxMonitorCmd+0xc>
    17a6:	4313      	orrs	r3, r2
    17a8:	6443      	str	r3, [r0, #68]	; 0x44
    17aa:	4770      	bx	lr
    17ac:	439a      	bics	r2, r3
    17ae:	6442      	str	r2, [r0, #68]	; 0x44
    17b0:	e7fb      	b.n	17aa <LOGUART_RxMonitorCmd+0xa>

000017b2 <LOGUART_RxMonBaudCtrlRegGet>:
    17b2:	6c40      	ldr	r0, [r0, #68]	; 0x44
    17b4:	4770      	bx	lr

000017b6 <LOGUART_RxMonitorSatusGet>:
    17b6:	6c80      	ldr	r0, [r0, #72]	; 0x48
    17b8:	4770      	bx	lr

000017ba <LOGUART_DiagInit>:
    17ba:	b530      	push	{r4, r5, lr}
    17bc:	0005      	movs	r5, r0
    17be:	20c0      	movs	r0, #192	; 0xc0
    17c0:	4b16      	ldr	r3, [pc, #88]	; (181c <LOGUART_DiagInit+0x62>)
    17c2:	b095      	sub	sp, #84	; 0x54
    17c4:	2201      	movs	r2, #1
    17c6:	f242 0100 	movw	r1, #8192	; 0x2000
    17ca:	681c      	ldr	r4, [r3, #0]
    17cc:	0300      	lsls	r0, r0, #12
    17ce:	4b14      	ldr	r3, [pc, #80]	; (1820 <LOGUART_DiagInit+0x66>)
    17d0:	4798      	blx	r3
    17d2:	2101      	movs	r1, #1
    17d4:	2000      	movs	r0, #0
    17d6:	4b13      	ldr	r3, [pc, #76]	; (1824 <LOGUART_DiagInit+0x6a>)
    17d8:	4798      	blx	r3
    17da:	a801      	add	r0, sp, #4
    17dc:	4b12      	ldr	r3, [pc, #72]	; (1828 <LOGUART_DiagInit+0x6e>)
    17de:	4798      	blx	r3
    17e0:	a801      	add	r0, sp, #4
    17e2:	f7ff fe3f 	bl	1464 <LOGUART_Init>
    17e6:	2d01      	cmp	r5, #1
    17e8:	d115      	bne.n	1816 <LOGUART_DiagInit+0x5c>
    17ea:	2303      	movs	r3, #3
    17ec:	09a1      	lsrs	r1, r4, #6
    17ee:	2414      	movs	r4, #20
    17f0:	2200      	movs	r2, #0
    17f2:	4019      	ands	r1, r3
    17f4:	434c      	muls	r4, r1
    17f6:	490d      	ldr	r1, [pc, #52]	; (182c <LOGUART_DiagInit+0x72>)
    17f8:	480d      	ldr	r0, [pc, #52]	; (1830 <LOGUART_DiagInit+0x76>)
    17fa:	1864      	adds	r4, r4, r1
    17fc:	6921      	ldr	r1, [r4, #16]
    17fe:	4d0d      	ldr	r5, [pc, #52]	; (1834 <LOGUART_DiagInit+0x7a>)
    1800:	9200      	str	r2, [sp, #0]
    1802:	47a8      	blx	r5
    1804:	6920      	ldr	r0, [r4, #16]
    1806:	4b0c      	ldr	r3, [pc, #48]	; (1838 <LOGUART_DiagInit+0x7e>)
    1808:	4798      	blx	r3
    180a:	4b0c      	ldr	r3, [pc, #48]	; (183c <LOGUART_DiagInit+0x82>)
    180c:	4668      	mov	r0, sp
    180e:	4798      	blx	r3
    1810:	9900      	ldr	r1, [sp, #0]
    1812:	4b0b      	ldr	r3, [pc, #44]	; (1840 <LOGUART_DiagInit+0x86>)
    1814:	4798      	blx	r3
    1816:	b015      	add	sp, #84	; 0x54
    1818:	bd30      	pop	{r4, r5, pc}
    181a:	46c0      	nop			; (mov r8, r8)
    181c:	4200827c 	.word	0x4200827c
    1820:	00001a89 	.word	0x00001a89
    1824:	00001931 	.word	0x00001931
    1828:	00000ff9 	.word	0x00000ff9
    182c:	0000c074 	.word	0x0000c074
    1830:	00004771 	.word	0x00004771
    1834:	00003bb5 	.word	0x00003bb5
    1838:	00003a8d 	.word	0x00003a8d
    183c:	0000443d 	.word	0x0000443d
    1840:	00004889 	.word	0x00004889

00001844 <PAD_DrvStrength>:
    1844:	4b06      	ldr	r3, [pc, #24]	; (1860 <PAD_DrvStrength+0x1c>)
    1846:	0080      	lsls	r0, r0, #2
    1848:	18c0      	adds	r0, r0, r3
    184a:	6803      	ldr	r3, [r0, #0]
    184c:	b919      	cbnz	r1, 1856 <PAD_DrvStrength+0x12>
    184e:	4a05      	ldr	r2, [pc, #20]	; (1864 <PAD_DrvStrength+0x20>)
    1850:	4013      	ands	r3, r2
    1852:	6003      	str	r3, [r0, #0]
    1854:	4770      	bx	lr
    1856:	f640 0200 	movw	r2, #2048	; 0x800
    185a:	4313      	orrs	r3, r2
    185c:	e7f9      	b.n	1852 <PAD_DrvStrength+0xe>
    185e:	46c0      	nop			; (mov r8, r8)
    1860:	42008a00 	.word	0x42008a00
    1864:	fffff7ff 	.word	0xfffff7ff

00001868 <PAD_PullCtrl>:
    1868:	4b08      	ldr	r3, [pc, #32]	; (188c <PAD_PullCtrl+0x24>)
    186a:	0080      	lsls	r0, r0, #2
    186c:	18c0      	adds	r0, r0, r3
    186e:	6803      	ldr	r3, [r0, #0]
    1870:	4a07      	ldr	r2, [pc, #28]	; (1890 <PAD_PullCtrl+0x28>)
    1872:	4013      	ands	r3, r2
    1874:	2901      	cmp	r1, #1
    1876:	d104      	bne.n	1882 <PAD_PullCtrl+0x1a>
    1878:	f240 2200 	movw	r2, #512	; 0x200
    187c:	4313      	orrs	r3, r2
    187e:	6003      	str	r3, [r0, #0]
    1880:	4770      	bx	lr
    1882:	2902      	cmp	r1, #2
    1884:	d1fb      	bne.n	187e <PAD_PullCtrl+0x16>
    1886:	f240 1200 	movw	r2, #256	; 0x100
    188a:	e7f7      	b.n	187c <PAD_PullCtrl+0x14>
    188c:	42008a00 	.word	0x42008a00
    1890:	fffffcff 	.word	0xfffffcff

00001894 <PAD_SleepPullCtrl>:
    1894:	4b08      	ldr	r3, [pc, #32]	; (18b8 <PAD_SleepPullCtrl+0x24>)
    1896:	0080      	lsls	r0, r0, #2
    1898:	18c0      	adds	r0, r0, r3
    189a:	6803      	ldr	r3, [r0, #0]
    189c:	4a07      	ldr	r2, [pc, #28]	; (18bc <PAD_SleepPullCtrl+0x28>)
    189e:	4013      	ands	r3, r2
    18a0:	2901      	cmp	r1, #1
    18a2:	d104      	bne.n	18ae <PAD_SleepPullCtrl+0x1a>
    18a4:	2280      	movs	r2, #128	; 0x80
    18a6:	0292      	lsls	r2, r2, #10
    18a8:	4313      	orrs	r3, r2
    18aa:	6003      	str	r3, [r0, #0]
    18ac:	4770      	bx	lr
    18ae:	2902      	cmp	r1, #2
    18b0:	d1fb      	bne.n	18aa <PAD_SleepPullCtrl+0x16>
    18b2:	2280      	movs	r2, #128	; 0x80
    18b4:	0252      	lsls	r2, r2, #9
    18b6:	e7f7      	b.n	18a8 <PAD_SleepPullCtrl+0x14>
    18b8:	42008a00 	.word	0x42008a00
    18bc:	fffcffff 	.word	0xfffcffff

000018c0 <PAD_ResistorCtrl>:
    18c0:	4b06      	ldr	r3, [pc, #24]	; (18dc <PAD_ResistorCtrl+0x1c>)
    18c2:	0080      	lsls	r0, r0, #2
    18c4:	18c0      	adds	r0, r0, r3
    18c6:	6803      	ldr	r3, [r0, #0]
    18c8:	b121      	cbz	r1, 18d4 <PAD_ResistorCtrl+0x14>
    18ca:	f240 4200 	movw	r2, #1024	; 0x400
    18ce:	4313      	orrs	r3, r2
    18d0:	6003      	str	r3, [r0, #0]
    18d2:	4770      	bx	lr
    18d4:	4a02      	ldr	r2, [pc, #8]	; (18e0 <PAD_ResistorCtrl+0x20>)
    18d6:	4013      	ands	r3, r2
    18d8:	e7fa      	b.n	18d0 <PAD_ResistorCtrl+0x10>
    18da:	46c0      	nop			; (mov r8, r8)
    18dc:	42008a00 	.word	0x42008a00
    18e0:	fffffbff 	.word	0xfffffbff

000018e4 <PAD_CMD>:
    18e4:	4b06      	ldr	r3, [pc, #24]	; (1900 <PAD_CMD+0x1c>)
    18e6:	0080      	lsls	r0, r0, #2
    18e8:	18c0      	adds	r0, r0, r3
    18ea:	6803      	ldr	r3, [r0, #0]
    18ec:	2901      	cmp	r1, #1
    18ee:	d103      	bne.n	18f8 <PAD_CMD+0x14>
    18f0:	4a04      	ldr	r2, [pc, #16]	; (1904 <PAD_CMD+0x20>)
    18f2:	4013      	ands	r3, r2
    18f4:	6003      	str	r3, [r0, #0]
    18f6:	4770      	bx	lr
    18f8:	f248 0200 	movw	r2, #32768	; 0x8000
    18fc:	4313      	orrs	r3, r2
    18fe:	e7f9      	b.n	18f4 <PAD_CMD+0x10>
    1900:	42008a00 	.word	0x42008a00
    1904:	ffff7fff 	.word	0xffff7fff

00001908 <Pinmux_Config>:
    1908:	221f      	movs	r2, #31
    190a:	4b04      	ldr	r3, [pc, #16]	; (191c <Pinmux_Config+0x14>)
    190c:	0080      	lsls	r0, r0, #2
    190e:	18c0      	adds	r0, r0, r3
    1910:	6803      	ldr	r3, [r0, #0]
    1912:	4011      	ands	r1, r2
    1914:	4393      	bics	r3, r2
    1916:	4319      	orrs	r1, r3
    1918:	6001      	str	r1, [r0, #0]
    191a:	4770      	bx	lr
    191c:	42008a00 	.word	0x42008a00

00001920 <Pinmux_ConfigGet>:
    1920:	4b02      	ldr	r3, [pc, #8]	; (192c <Pinmux_ConfigGet+0xc>)
    1922:	0080      	lsls	r0, r0, #2
    1924:	58c0      	ldr	r0, [r0, r3]
    1926:	231f      	movs	r3, #31
    1928:	4018      	ands	r0, r3
    192a:	4770      	bx	lr
    192c:	42008a00 	.word	0x42008a00

00001930 <Pinmux_UartLogCtrl>:
    1930:	b570      	push	{r4, r5, r6, lr}
    1932:	000c      	movs	r4, r1
    1934:	b118      	cbz	r0, 193e <Pinmux_UartLogCtrl+0xe>
    1936:	21e7      	movs	r1, #231	; 0xe7
    1938:	480a      	ldr	r0, [pc, #40]	; (1964 <Pinmux_UartLogCtrl+0x34>)
    193a:	f7fe fbeb 	bl	114 <io_assert_failed>
    193e:	1e63      	subs	r3, r4, #1
    1940:	419c      	sbcs	r4, r3
    1942:	4d09      	ldr	r5, [pc, #36]	; (1968 <Pinmux_UartLogCtrl+0x38>)
    1944:	2102      	movs	r1, #2
    1946:	2037      	movs	r0, #55	; 0x37
    1948:	47a8      	blx	r5
    194a:	0064      	lsls	r4, r4, #1
    194c:	2102      	movs	r1, #2
    194e:	2038      	movs	r0, #56	; 0x38
    1950:	47a8      	blx	r5
    1952:	0021      	movs	r1, r4
    1954:	2037      	movs	r0, #55	; 0x37
    1956:	f7ff ffd7 	bl	1908 <Pinmux_Config>
    195a:	0021      	movs	r1, r4
    195c:	2038      	movs	r0, #56	; 0x38
    195e:	f7ff ffd3 	bl	1908 <Pinmux_Config>
    1962:	bd70      	pop	{r4, r5, r6, pc}
    1964:	0000c477 	.word	0x0000c477
    1968:	00001869 	.word	0x00001869

0000196c <PAD_SpicCtrl>:
    196c:	b510      	push	{r4, lr}
    196e:	2102      	movs	r1, #2
    1970:	4c12      	ldr	r4, [pc, #72]	; (19bc <PAD_SpicCtrl+0x50>)
    1972:	2801      	cmp	r0, #1
    1974:	d111      	bne.n	199a <PAD_SpicCtrl+0x2e>
    1976:	3045      	adds	r0, #69	; 0x45
    1978:	47a0      	blx	r4
    197a:	2102      	movs	r1, #2
    197c:	2041      	movs	r0, #65	; 0x41
    197e:	47a0      	blx	r4
    1980:	2102      	movs	r1, #2
    1982:	2042      	movs	r0, #66	; 0x42
    1984:	47a0      	blx	r4
    1986:	2102      	movs	r1, #2
    1988:	2043      	movs	r0, #67	; 0x43
    198a:	47a0      	blx	r4
    198c:	2102      	movs	r1, #2
    198e:	2044      	movs	r0, #68	; 0x44
    1990:	47a0      	blx	r4
    1992:	2102      	movs	r1, #2
    1994:	2045      	movs	r0, #69	; 0x45
    1996:	47a0      	blx	r4
    1998:	bd10      	pop	{r4, pc}
    199a:	2040      	movs	r0, #64	; 0x40
    199c:	47a0      	blx	r4
    199e:	2102      	movs	r1, #2
    19a0:	203b      	movs	r0, #59	; 0x3b
    19a2:	47a0      	blx	r4
    19a4:	2102      	movs	r1, #2
    19a6:	203c      	movs	r0, #60	; 0x3c
    19a8:	47a0      	blx	r4
    19aa:	2102      	movs	r1, #2
    19ac:	203d      	movs	r0, #61	; 0x3d
    19ae:	47a0      	blx	r4
    19b0:	2102      	movs	r1, #2
    19b2:	203e      	movs	r0, #62	; 0x3e
    19b4:	47a0      	blx	r4
    19b6:	2102      	movs	r1, #2
    19b8:	203f      	movs	r0, #63	; 0x3f
    19ba:	e7ec      	b.n	1996 <PAD_SpicCtrl+0x2a>
    19bc:	00001869 	.word	0x00001869

000019c0 <Pinmux_SpicCtrl>:
    19c0:	b570      	push	{r4, r5, r6, lr}
    19c2:	2400      	movs	r4, #0
    19c4:	b101      	cbz	r1, 19c8 <Pinmux_SpicCtrl+0x8>
    19c6:	3406      	adds	r4, #6
    19c8:	2102      	movs	r1, #2
    19ca:	4d2a      	ldr	r5, [pc, #168]	; (1a74 <Pinmux_SpicCtrl+0xb4>)
    19cc:	2801      	cmp	r0, #1
    19ce:	d129      	bne.n	1a24 <Pinmux_SpicCtrl+0x64>
    19d0:	3045      	adds	r0, #69	; 0x45
    19d2:	47a8      	blx	r5
    19d4:	2102      	movs	r1, #2
    19d6:	2041      	movs	r0, #65	; 0x41
    19d8:	47a8      	blx	r5
    19da:	2102      	movs	r1, #2
    19dc:	2042      	movs	r0, #66	; 0x42
    19de:	47a8      	blx	r5
    19e0:	2102      	movs	r1, #2
    19e2:	2043      	movs	r0, #67	; 0x43
    19e4:	47a8      	blx	r5
    19e6:	2102      	movs	r1, #2
    19e8:	2044      	movs	r0, #68	; 0x44
    19ea:	47a8      	blx	r5
    19ec:	2102      	movs	r1, #2
    19ee:	2045      	movs	r0, #69	; 0x45
    19f0:	47a8      	blx	r5
    19f2:	0021      	movs	r1, r4
    19f4:	2041      	movs	r0, #65	; 0x41
    19f6:	f7ff ff87 	bl	1908 <Pinmux_Config>
    19fa:	0021      	movs	r1, r4
    19fc:	2042      	movs	r0, #66	; 0x42
    19fe:	f7ff ff83 	bl	1908 <Pinmux_Config>
    1a02:	0021      	movs	r1, r4
    1a04:	2043      	movs	r0, #67	; 0x43
    1a06:	f7ff ff7f 	bl	1908 <Pinmux_Config>
    1a0a:	0021      	movs	r1, r4
    1a0c:	2044      	movs	r0, #68	; 0x44
    1a0e:	f7ff ff7b 	bl	1908 <Pinmux_Config>
    1a12:	0021      	movs	r1, r4
    1a14:	2045      	movs	r0, #69	; 0x45
    1a16:	f7ff ff77 	bl	1908 <Pinmux_Config>
    1a1a:	0021      	movs	r1, r4
    1a1c:	2046      	movs	r0, #70	; 0x46
    1a1e:	f7ff ff73 	bl	1908 <Pinmux_Config>
    1a22:	bd70      	pop	{r4, r5, r6, pc}
    1a24:	2040      	movs	r0, #64	; 0x40
    1a26:	47a8      	blx	r5
    1a28:	2102      	movs	r1, #2
    1a2a:	203b      	movs	r0, #59	; 0x3b
    1a2c:	47a8      	blx	r5
    1a2e:	2102      	movs	r1, #2
    1a30:	203c      	movs	r0, #60	; 0x3c
    1a32:	47a8      	blx	r5
    1a34:	2102      	movs	r1, #2
    1a36:	203d      	movs	r0, #61	; 0x3d
    1a38:	47a8      	blx	r5
    1a3a:	2102      	movs	r1, #2
    1a3c:	203e      	movs	r0, #62	; 0x3e
    1a3e:	47a8      	blx	r5
    1a40:	2102      	movs	r1, #2
    1a42:	203f      	movs	r0, #63	; 0x3f
    1a44:	47a8      	blx	r5
    1a46:	0021      	movs	r1, r4
    1a48:	203b      	movs	r0, #59	; 0x3b
    1a4a:	f7ff ff5d 	bl	1908 <Pinmux_Config>
    1a4e:	0021      	movs	r1, r4
    1a50:	203c      	movs	r0, #60	; 0x3c
    1a52:	f7ff ff59 	bl	1908 <Pinmux_Config>
    1a56:	0021      	movs	r1, r4
    1a58:	203d      	movs	r0, #61	; 0x3d
    1a5a:	f7ff ff55 	bl	1908 <Pinmux_Config>
    1a5e:	0021      	movs	r1, r4
    1a60:	203e      	movs	r0, #62	; 0x3e
    1a62:	f7ff ff51 	bl	1908 <Pinmux_Config>
    1a66:	0021      	movs	r1, r4
    1a68:	203f      	movs	r0, #63	; 0x3f
    1a6a:	f7ff ff4d 	bl	1908 <Pinmux_Config>
    1a6e:	0021      	movs	r1, r4
    1a70:	2040      	movs	r0, #64	; 0x40
    1a72:	e7d4      	b.n	1a1e <Pinmux_SpicCtrl+0x5e>
    1a74:	00001869 	.word	0x00001869

00001a78 <Pinmux_Swdoff>:
    1a78:	2101      	movs	r1, #1
    1a7a:	4a02      	ldr	r2, [pc, #8]	; (1a84 <Pinmux_Swdoff+0xc>)
    1a7c:	6813      	ldr	r3, [r2, #0]
    1a7e:	438b      	bics	r3, r1
    1a80:	6013      	str	r3, [r2, #0]
    1a82:	4770      	bx	lr
    1a84:	42008bf8 	.word	0x42008bf8

00001a88 <RCC_PeriphClockCmd>:
    1a88:	0f8b      	lsrs	r3, r1, #30
    1a8a:	3b01      	subs	r3, #1
    1a8c:	b570      	push	{r4, r5, r6, lr}
    1a8e:	0f84      	lsrs	r4, r0, #30
    1a90:	2b02      	cmp	r3, #2
    1a92:	d820      	bhi.n	1ad6 <RCC_PeriphClockCmd+0x4e>
    1a94:	4d16      	ldr	r5, [pc, #88]	; (1af0 <RCC_PeriphClockCmd+0x68>)
    1a96:	005b      	lsls	r3, r3, #1
    1a98:	5b5b      	ldrh	r3, [r3, r5]
    1a9a:	b141      	cbz	r1, 1aae <RCC_PeriphClockCmd+0x26>
    1a9c:	4d15      	ldr	r5, [pc, #84]	; (1af4 <RCC_PeriphClockCmd+0x6c>)
    1a9e:	0089      	lsls	r1, r1, #2
    1aa0:	195b      	adds	r3, r3, r5
    1aa2:	681d      	ldr	r5, [r3, #0]
    1aa4:	0889      	lsrs	r1, r1, #2
    1aa6:	2a01      	cmp	r2, #1
    1aa8:	d118      	bne.n	1adc <RCC_PeriphClockCmd+0x54>
    1aaa:	4329      	orrs	r1, r5
    1aac:	6019      	str	r1, [r3, #0]
    1aae:	3c01      	subs	r4, #1
    1ab0:	2c02      	cmp	r4, #2
    1ab2:	d816      	bhi.n	1ae2 <RCC_PeriphClockCmd+0x5a>
    1ab4:	4910      	ldr	r1, [pc, #64]	; (1af8 <RCC_PeriphClockCmd+0x70>)
    1ab6:	0063      	lsls	r3, r4, #1
    1ab8:	5a5b      	ldrh	r3, [r3, r1]
    1aba:	b140      	cbz	r0, 1ace <RCC_PeriphClockCmd+0x46>
    1abc:	490d      	ldr	r1, [pc, #52]	; (1af4 <RCC_PeriphClockCmd+0x6c>)
    1abe:	0080      	lsls	r0, r0, #2
    1ac0:	0880      	lsrs	r0, r0, #2
    1ac2:	185b      	adds	r3, r3, r1
    1ac4:	2a01      	cmp	r2, #1
    1ac6:	d10f      	bne.n	1ae8 <RCC_PeriphClockCmd+0x60>
    1ac8:	681a      	ldr	r2, [r3, #0]
    1aca:	4310      	orrs	r0, r2
    1acc:	6018      	str	r0, [r3, #0]
    1ace:	200a      	movs	r0, #10
    1ad0:	4b0a      	ldr	r3, [pc, #40]	; (1afc <RCC_PeriphClockCmd+0x74>)
    1ad2:	4798      	blx	r3
    1ad4:	bd70      	pop	{r4, r5, r6, pc}
    1ad6:	f240 2314 	movw	r3, #532	; 0x214
    1ada:	e7de      	b.n	1a9a <RCC_PeriphClockCmd+0x12>
    1adc:	438d      	bics	r5, r1
    1ade:	601d      	str	r5, [r3, #0]
    1ae0:	e7e5      	b.n	1aae <RCC_PeriphClockCmd+0x26>
    1ae2:	f240 2308 	movw	r3, #520	; 0x208
    1ae6:	e7e8      	b.n	1aba <RCC_PeriphClockCmd+0x32>
    1ae8:	681a      	ldr	r2, [r3, #0]
    1aea:	4382      	bics	r2, r0
    1aec:	601a      	str	r2, [r3, #0]
    1aee:	e7ee      	b.n	1ace <RCC_PeriphClockCmd+0x46>
    1af0:	0000c48a 	.word	0x0000c48a
    1af4:	42008000 	.word	0x42008000
    1af8:	0000c490 	.word	0x0000c490
    1afc:	0000030b 	.word	0x0000030b

00001b00 <RCC_PeriphClockSource_UART>:
    1b00:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    1b02:	4e19      	ldr	r6, [pc, #100]	; (1b68 <RCC_PeriphClockSource_UART+0x68>)
    1b04:	4c19      	ldr	r4, [pc, #100]	; (1b6c <RCC_PeriphClockSource_UART+0x6c>)
    1b06:	6832      	ldr	r2, [r6, #0]
    1b08:	4f19      	ldr	r7, [pc, #100]	; (1b70 <RCC_PeriphClockSource_UART+0x70>)
    1b0a:	0353      	lsls	r3, r2, #13
    1b0c:	f240 0c00 	movw	ip, #0
    1b10:	6824      	ldr	r4, [r4, #0]
    1b12:	0f5b      	lsrs	r3, r3, #29
    1b14:	42b8      	cmp	r0, r7
    1b16:	d00a      	beq.n	1b2e <RCC_PeriphClockSource_UART+0x2e>
    1b18:	4f16      	ldr	r7, [pc, #88]	; (1b74 <RCC_PeriphClockSource_UART+0x74>)
    1b1a:	f240 0c01 	movw	ip, #1
    1b1e:	42b8      	cmp	r0, r7
    1b20:	d005      	beq.n	1b2e <RCC_PeriphClockSource_UART+0x2e>
    1b22:	4f15      	ldr	r7, [pc, #84]	; (1b78 <RCC_PeriphClockSource_UART+0x78>)
    1b24:	19c0      	adds	r0, r0, r7
    1b26:	4247      	negs	r7, r0
    1b28:	4178      	adcs	r0, r7
    1b2a:	0040      	lsls	r0, r0, #1
    1b2c:	4684      	mov	ip, r0
    1b2e:	2701      	movs	r7, #1
    1b30:	4665      	mov	r5, ip
    1b32:	0038      	movs	r0, r7
    1b34:	40a8      	lsls	r0, r5
    1b36:	b959      	cbnz	r1, 1b50 <RCC_PeriphClockSource_UART+0x50>
    1b38:	4383      	bics	r3, r0
    1b3a:	4910      	ldr	r1, [pc, #64]	; (1b7c <RCC_PeriphClockSource_UART+0x7c>)
    1b3c:	041b      	lsls	r3, r3, #16
    1b3e:	400a      	ands	r2, r1
    1b40:	431a      	orrs	r2, r3
    1b42:	6032      	str	r2, [r6, #0]
    1b44:	2003      	movs	r0, #3
    1b46:	4b0e      	ldr	r3, [pc, #56]	; (1b80 <RCC_PeriphClockSource_UART+0x80>)
    1b48:	4798      	blx	r3
    1b4a:	4b08      	ldr	r3, [pc, #32]	; (1b6c <RCC_PeriphClockSource_UART+0x6c>)
    1b4c:	601c      	str	r4, [r3, #0]
    1b4e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    1b50:	4303      	orrs	r3, r0
    1b52:	2902      	cmp	r1, #2
    1b54:	d104      	bne.n	1b60 <RCC_PeriphClockSource_UART+0x60>
    1b56:	2180      	movs	r1, #128	; 0x80
    1b58:	0309      	lsls	r1, r1, #12
    1b5a:	430a      	orrs	r2, r1
    1b5c:	433c      	orrs	r4, r7
    1b5e:	e7ec      	b.n	1b3a <RCC_PeriphClockSource_UART+0x3a>
    1b60:	4908      	ldr	r1, [pc, #32]	; (1b84 <RCC_PeriphClockSource_UART+0x84>)
    1b62:	43bc      	bics	r4, r7
    1b64:	400a      	ands	r2, r1
    1b66:	e7e8      	b.n	1b3a <RCC_PeriphClockSource_UART+0x3a>
    1b68:	42008224 	.word	0x42008224
    1b6c:	4200822c 	.word	0x4200822c
    1b70:	41004000 	.word	0x41004000
    1b74:	41005000 	.word	0x41005000
    1b78:	beffa000 	.word	0xbeffa000
    1b7c:	fff8ffff 	.word	0xfff8ffff
    1b80:	0000030b 	.word	0x0000030b
    1b84:	fff7ffff 	.word	0xfff7ffff

00001b88 <RCC_PeriphClockSource_SPIC>:
    1b88:	210c      	movs	r1, #12
    1b8a:	4a04      	ldr	r2, [pc, #16]	; (1b9c <RCC_PeriphClockSource_SPIC+0x14>)
    1b8c:	0080      	lsls	r0, r0, #2
    1b8e:	6813      	ldr	r3, [r2, #0]
    1b90:	4008      	ands	r0, r1
    1b92:	438b      	bics	r3, r1
    1b94:	4318      	orrs	r0, r3
    1b96:	6010      	str	r0, [r2, #0]
    1b98:	4770      	bx	lr
    1b9a:	46c0      	nop			; (mov r8, r8)
    1b9c:	42008224 	.word	0x42008224

00001ba0 <RCC_PeriphClockSource_PSRAM>:
    1ba0:	4a03      	ldr	r2, [pc, #12]	; (1bb0 <RCC_PeriphClockSource_PSRAM+0x10>)
    1ba2:	07c0      	lsls	r0, r0, #31
    1ba4:	6813      	ldr	r3, [r2, #0]
    1ba6:	005b      	lsls	r3, r3, #1
    1ba8:	085b      	lsrs	r3, r3, #1
    1baa:	4318      	orrs	r0, r3
    1bac:	6010      	str	r0, [r2, #0]
    1bae:	4770      	bx	lr
    1bb0:	42008224 	.word	0x42008224

00001bb4 <RCC_PeriphClockSource_SDM>:
    1bb4:	4a04      	ldr	r2, [pc, #16]	; (1bc8 <RCC_PeriphClockSource_SDM+0x14>)
    1bb6:	4905      	ldr	r1, [pc, #20]	; (1bcc <RCC_PeriphClockSource_SDM+0x18>)
    1bb8:	6813      	ldr	r3, [r2, #0]
    1bba:	07c0      	lsls	r0, r0, #31
    1bbc:	400b      	ands	r3, r1
    1bbe:	0c00      	lsrs	r0, r0, #16
    1bc0:	4318      	orrs	r0, r3
    1bc2:	6010      	str	r0, [r2, #0]
    1bc4:	4770      	bx	lr
    1bc6:	46c0      	nop			; (mov r8, r8)
    1bc8:	42008224 	.word	0x42008224
    1bcc:	ffff7fff 	.word	0xffff7fff

00001bd0 <RCC_PeriphClockSource_VADMEM>:
    1bd0:	4a05      	ldr	r2, [pc, #20]	; (1be8 <RCC_PeriphClockSource_VADMEM+0x18>)
    1bd2:	4906      	ldr	r1, [pc, #24]	; (1bec <RCC_PeriphClockSource_VADMEM+0x1c>)
    1bd4:	6813      	ldr	r3, [r2, #0]
    1bd6:	0380      	lsls	r0, r0, #14
    1bd8:	400b      	ands	r3, r1
    1bda:	f244 0100 	movw	r1, #16384	; 0x4000
    1bde:	4008      	ands	r0, r1
    1be0:	4318      	orrs	r0, r3
    1be2:	6010      	str	r0, [r2, #0]
    1be4:	4770      	bx	lr
    1be6:	46c0      	nop			; (mov r8, r8)
    1be8:	42008224 	.word	0x42008224
    1bec:	ffffbfff 	.word	0xffffbfff

00001bf0 <RCC_PeriphClockSource_AUDIOCODEC>:
    1bf0:	4a05      	ldr	r2, [pc, #20]	; (1c08 <RCC_PeriphClockSource_AUDIOCODEC+0x18>)
    1bf2:	4906      	ldr	r1, [pc, #24]	; (1c0c <RCC_PeriphClockSource_AUDIOCODEC+0x1c>)
    1bf4:	6813      	ldr	r3, [r2, #0]
    1bf6:	0340      	lsls	r0, r0, #13
    1bf8:	400b      	ands	r3, r1
    1bfa:	f242 0100 	movw	r1, #8192	; 0x2000
    1bfe:	4008      	ands	r0, r1
    1c00:	4318      	orrs	r0, r3
    1c02:	6010      	str	r0, [r2, #0]
    1c04:	4770      	bx	lr
    1c06:	46c0      	nop			; (mov r8, r8)
    1c08:	42008224 	.word	0x42008224
    1c0c:	ffffdfff 	.word	0xffffdfff

00001c10 <RCC_PeriphClockSource_CTC>:
    1c10:	2140      	movs	r1, #64	; 0x40
    1c12:	4a04      	ldr	r2, [pc, #16]	; (1c24 <RCC_PeriphClockSource_CTC+0x14>)
    1c14:	0180      	lsls	r0, r0, #6
    1c16:	6813      	ldr	r3, [r2, #0]
    1c18:	4008      	ands	r0, r1
    1c1a:	438b      	bics	r3, r1
    1c1c:	4318      	orrs	r0, r3
    1c1e:	6010      	str	r0, [r2, #0]
    1c20:	4770      	bx	lr
    1c22:	46c0      	nop			; (mov r8, r8)
    1c24:	42008224 	.word	0x42008224

00001c28 <RCC_PeriphClockSource_ADC>:
    1c28:	2120      	movs	r1, #32
    1c2a:	4a04      	ldr	r2, [pc, #16]	; (1c3c <RCC_PeriphClockSource_ADC+0x14>)
    1c2c:	0140      	lsls	r0, r0, #5
    1c2e:	6813      	ldr	r3, [r2, #0]
    1c30:	4008      	ands	r0, r1
    1c32:	438b      	bics	r3, r1
    1c34:	4318      	orrs	r0, r3
    1c36:	6010      	str	r0, [r2, #0]
    1c38:	4770      	bx	lr
    1c3a:	46c0      	nop			; (mov r8, r8)
    1c3c:	42008224 	.word	0x42008224

00001c40 <RCC_PeriphClockSource_GPIO>:
    1c40:	2110      	movs	r1, #16
    1c42:	4a04      	ldr	r2, [pc, #16]	; (1c54 <RCC_PeriphClockSource_GPIO+0x14>)
    1c44:	0100      	lsls	r0, r0, #4
    1c46:	6813      	ldr	r3, [r2, #0]
    1c48:	4008      	ands	r0, r1
    1c4a:	438b      	bics	r3, r1
    1c4c:	4318      	orrs	r0, r3
    1c4e:	6010      	str	r0, [r2, #0]
    1c50:	4770      	bx	lr
    1c52:	46c0      	nop			; (mov r8, r8)
    1c54:	42008224 	.word	0x42008224

00001c58 <RCC_PeriphClockSource_SPORT>:
    1c58:	b5f0      	push	{r4, r5, r6, r7, lr}
    1c5a:	2701      	movs	r7, #1
    1c5c:	4d18      	ldr	r5, [pc, #96]	; (1cc0 <RCC_PeriphClockSource_SPORT+0x68>)
    1c5e:	4c19      	ldr	r4, [pc, #100]	; (1cc4 <RCC_PeriphClockSource_SPORT+0x6c>)
    1c60:	682e      	ldr	r6, [r5, #0]
    1c62:	4087      	lsls	r7, r0
    1c64:	0132      	lsls	r2, r6, #4
    1c66:	6823      	ldr	r3, [r4, #0]
    1c68:	0f12      	lsrs	r2, r2, #28
    1c6a:	2904      	cmp	r1, #4
    1c6c:	d10a      	bne.n	1c84 <RCC_PeriphClockSource_SPORT+0x2c>
    1c6e:	43ba      	bics	r2, r7
    1c70:	4915      	ldr	r1, [pc, #84]	; (1cc8 <RCC_PeriphClockSource_SPORT+0x70>)
    1c72:	0612      	lsls	r2, r2, #24
    1c74:	400e      	ands	r6, r1
    1c76:	21f0      	movs	r1, #240	; 0xf0
    1c78:	0509      	lsls	r1, r1, #20
    1c7a:	400a      	ands	r2, r1
    1c7c:	4332      	orrs	r2, r6
    1c7e:	602a      	str	r2, [r5, #0]
    1c80:	6023      	str	r3, [r4, #0]
    1c82:	bdf0      	pop	{r4, r5, r6, r7, pc}
    1c84:	433a      	orrs	r2, r7
    1c86:	2802      	cmp	r0, #2
    1c88:	d016      	beq.n	1cb8 <RCC_PeriphClockSource_SPORT+0x60>
    1c8a:	dc07      	bgt.n	1c9c <RCC_PeriphClockSource_SPORT+0x44>
    1c8c:	b178      	cbz	r0, 1cae <RCC_PeriphClockSource_SPORT+0x56>
    1c8e:	2801      	cmp	r0, #1
    1c90:	d1ee      	bne.n	1c70 <RCC_PeriphClockSource_SPORT+0x18>
    1c92:	270c      	movs	r7, #12
    1c94:	0088      	lsls	r0, r1, #2
    1c96:	43bb      	bics	r3, r7
    1c98:	4038      	ands	r0, r7
    1c9a:	e00b      	b.n	1cb4 <RCC_PeriphClockSource_SPORT+0x5c>
    1c9c:	2803      	cmp	r0, #3
    1c9e:	d1e7      	bne.n	1c70 <RCC_PeriphClockSource_SPORT+0x18>
    1ca0:	20c0      	movs	r0, #192	; 0xc0
    1ca2:	0189      	lsls	r1, r1, #6
    1ca4:	4383      	bics	r3, r0
    1ca6:	303f      	adds	r0, #63	; 0x3f
    1ca8:	4001      	ands	r1, r0
    1caa:	430b      	orrs	r3, r1
    1cac:	e7e0      	b.n	1c70 <RCC_PeriphClockSource_SPORT+0x18>
    1cae:	2003      	movs	r0, #3
    1cb0:	4383      	bics	r3, r0
    1cb2:	4008      	ands	r0, r1
    1cb4:	4303      	orrs	r3, r0
    1cb6:	e7db      	b.n	1c70 <RCC_PeriphClockSource_SPORT+0x18>
    1cb8:	2730      	movs	r7, #48	; 0x30
    1cba:	0108      	lsls	r0, r1, #4
    1cbc:	43bb      	bics	r3, r7
    1cbe:	e7eb      	b.n	1c98 <RCC_PeriphClockSource_SPORT+0x40>
    1cc0:	42008224 	.word	0x42008224
    1cc4:	4200822c 	.word	0x4200822c
    1cc8:	f0ffffff 	.word	0xf0ffffff

00001ccc <RCC_PeriphClockSource_32K>:
    1ccc:	2130      	movs	r1, #48	; 0x30
    1cce:	4a04      	ldr	r2, [pc, #16]	; (1ce0 <RCC_PeriphClockSource_32K+0x14>)
    1cd0:	0100      	lsls	r0, r0, #4
    1cd2:	6813      	ldr	r3, [r2, #0]
    1cd4:	4008      	ands	r0, r1
    1cd6:	438b      	bics	r3, r1
    1cd8:	4318      	orrs	r0, r3
    1cda:	6010      	str	r0, [r2, #0]
    1cdc:	4770      	bx	lr
    1cde:	46c0      	nop			; (mov r8, r8)
    1ce0:	42008010 	.word	0x42008010

00001ce4 <RCC_PeriphClockSource_LOGUART>:
    1ce4:	b570      	push	{r4, r5, r6, lr}
    1ce6:	4a0e      	ldr	r2, [pc, #56]	; (1d20 <RCC_PeriphClockSource_LOGUART+0x3c>)
    1ce8:	4d0e      	ldr	r5, [pc, #56]	; (1d24 <RCC_PeriphClockSource_LOGUART+0x40>)
    1cea:	6813      	ldr	r3, [r2, #0]
    1cec:	682c      	ldr	r4, [r5, #0]
    1cee:	b938      	cbnz	r0, 1d00 <RCC_PeriphClockSource_LOGUART+0x1c>
    1cf0:	490d      	ldr	r1, [pc, #52]	; (1d28 <RCC_PeriphClockSource_LOGUART+0x44>)
    1cf2:	400b      	ands	r3, r1
    1cf4:	6013      	str	r3, [r2, #0]
    1cf6:	2003      	movs	r0, #3
    1cf8:	4b0c      	ldr	r3, [pc, #48]	; (1d2c <RCC_PeriphClockSource_LOGUART+0x48>)
    1cfa:	4798      	blx	r3
    1cfc:	602c      	str	r4, [r5, #0]
    1cfe:	bd70      	pop	{r4, r5, r6, pc}
    1d00:	2802      	cmp	r0, #2
    1d02:	d105      	bne.n	1d10 <RCC_PeriphClockSource_LOGUART+0x2c>
    1d04:	f641 0100 	movw	r1, #6144	; 0x1800
    1d08:	430b      	orrs	r3, r1
    1d0a:	2101      	movs	r1, #1
    1d0c:	430c      	orrs	r4, r1
    1d0e:	e7f1      	b.n	1cf4 <RCC_PeriphClockSource_LOGUART+0x10>
    1d10:	4907      	ldr	r1, [pc, #28]	; (1d30 <RCC_PeriphClockSource_LOGUART+0x4c>)
    1d12:	400b      	ands	r3, r1
    1d14:	f640 0100 	movw	r1, #2048	; 0x800
    1d18:	430b      	orrs	r3, r1
    1d1a:	2101      	movs	r1, #1
    1d1c:	438c      	bics	r4, r1
    1d1e:	e7e9      	b.n	1cf4 <RCC_PeriphClockSource_LOGUART+0x10>
    1d20:	42008224 	.word	0x42008224
    1d24:	4200822c 	.word	0x4200822c
    1d28:	fffff7ff 	.word	0xfffff7ff
    1d2c:	0000030b 	.word	0x0000030b
    1d30:	ffffefff 	.word	0xffffefff

00001d34 <SYSCFG_GetChipInfo>:
    1d34:	4b01      	ldr	r3, [pc, #4]	; (1d3c <SYSCFG_GetChipInfo+0x8>)
    1d36:	6818      	ldr	r0, [r3, #0]
    1d38:	4770      	bx	lr
    1d3a:	46c0      	nop			; (mov r8, r8)
    1d3c:	4200827c 	.word	0x4200827c

00001d40 <SYSCFG_CUTVersion>:
    1d40:	4b02      	ldr	r3, [pc, #8]	; (1d4c <SYSCFG_CUTVersion+0xc>)
    1d42:	6818      	ldr	r0, [r3, #0]
    1d44:	230f      	movs	r3, #15
    1d46:	4018      	ands	r0, r3
    1d48:	4770      	bx	lr
    1d4a:	46c0      	nop			; (mov r8, r8)
    1d4c:	4200827c 	.word	0x4200827c

00001d50 <SYSCFG_TRP_UARTImage>:
    1d50:	4b02      	ldr	r3, [pc, #8]	; (1d5c <SYSCFG_TRP_UARTImage+0xc>)
    1d52:	6818      	ldr	r0, [r3, #0]
    1d54:	0300      	lsls	r0, r0, #12
    1d56:	0fc0      	lsrs	r0, r0, #31
    1d58:	4770      	bx	lr
    1d5a:	46c0      	nop			; (mov r8, r8)
    1d5c:	4200827c 	.word	0x4200827c

00001d60 <SYSCFG_TRP_ICFG>:
    1d60:	4b01      	ldr	r3, [pc, #4]	; (1d68 <SYSCFG_TRP_ICFG+0x8>)
    1d62:	6818      	ldr	r0, [r3, #0]
    1d64:	0f00      	lsrs	r0, r0, #28
    1d66:	4770      	bx	lr
    1d68:	42008280 	.word	0x42008280

00001d6c <SYSCFG_TRP_TestMode>:
    1d6c:	4b02      	ldr	r3, [pc, #8]	; (1d78 <SYSCFG_TRP_TestMode+0xc>)
    1d6e:	6818      	ldr	r0, [r3, #0]
    1d70:	0380      	lsls	r0, r0, #14
    1d72:	0fc0      	lsrs	r0, r0, #31
    1d74:	4770      	bx	lr
    1d76:	46c0      	nop			; (mov r8, r8)
    1d78:	4200827c 	.word	0x4200827c

00001d7c <SYSCFG_TRP_OTPBYP>:
    1d7c:	4b02      	ldr	r3, [pc, #8]	; (1d88 <SYSCFG_TRP_OTPBYP+0xc>)
    1d7e:	6818      	ldr	r0, [r3, #0]
    1d80:	03c0      	lsls	r0, r0, #15
    1d82:	0fc0      	lsrs	r0, r0, #31
    1d84:	4770      	bx	lr
    1d86:	46c0      	nop			; (mov r8, r8)
    1d88:	4200827c 	.word	0x4200827c

00001d8c <SYSCFG_TRP_BootSel>:
    1d8c:	4b02      	ldr	r3, [pc, #8]	; (1d98 <SYSCFG_TRP_BootSel+0xc>)
    1d8e:	6818      	ldr	r0, [r3, #0]
    1d90:	02c0      	lsls	r0, r0, #11
    1d92:	0fc0      	lsrs	r0, r0, #31
    1d94:	4770      	bx	lr
    1d96:	46c0      	nop			; (mov r8, r8)
    1d98:	4200827c 	.word	0x4200827c

00001d9c <SYSCFG_ROMINFO_Get>:
    1d9c:	4b01      	ldr	r3, [pc, #4]	; (1da4 <SYSCFG_ROMINFO_Get+0x8>)
    1d9e:	6818      	ldr	r0, [r3, #0]
    1da0:	b280      	uxth	r0, r0
    1da2:	4770      	bx	lr
    1da4:	42008268 	.word	0x42008268

00001da8 <SYSCFG_ROMINFO_Set>:
    1da8:	2201      	movs	r2, #1
    1daa:	4903      	ldr	r1, [pc, #12]	; (1db8 <SYSCFG_ROMINFO_Set+0x10>)
    1dac:	680b      	ldr	r3, [r1, #0]
    1dae:	0c1b      	lsrs	r3, r3, #16
    1db0:	041b      	lsls	r3, r3, #16
    1db2:	4313      	orrs	r3, r2
    1db4:	600b      	str	r3, [r1, #0]
    1db6:	4770      	bx	lr
    1db8:	42008268 	.word	0x42008268

00001dbc <SYSCFG_CHIPType_Get>:
    1dbc:	4b02      	ldr	r3, [pc, #8]	; (1dc8 <SYSCFG_CHIPType_Get+0xc>)
    1dbe:	6818      	ldr	r0, [r3, #0]
    1dc0:	0580      	lsls	r0, r0, #22
    1dc2:	0f80      	lsrs	r0, r0, #30
    1dc4:	4770      	bx	lr
    1dc6:	46c0      	nop			; (mov r8, r8)
    1dc8:	4200827c 	.word	0x4200827c

00001dcc <SYSCFG_OTP_RDPEn>:
    1dcc:	2201      	movs	r2, #1
    1dce:	4b05      	ldr	r3, [pc, #20]	; (1de4 <SYSCFG_OTP_RDPEn+0x18>)
    1dd0:	0010      	movs	r0, r2
    1dd2:	6819      	ldr	r1, [r3, #0]
    1dd4:	4b04      	ldr	r3, [pc, #16]	; (1de8 <SYSCFG_OTP_RDPEn+0x1c>)
    1dd6:	681b      	ldr	r3, [r3, #0]
    1dd8:	00c9      	lsls	r1, r1, #3
    1dda:	d402      	bmi.n	1de2 <SYSCFG_OTP_RDPEn+0x16>
    1ddc:	095b      	lsrs	r3, r3, #5
    1dde:	439a      	bics	r2, r3
    1de0:	0010      	movs	r0, r2
    1de2:	4770      	bx	lr
    1de4:	42000100 	.word	0x42000100
    1de8:	42000368 	.word	0x42000368

00001dec <SYSCFG_OTP_RSIPEn>:
    1dec:	2201      	movs	r2, #1
    1dee:	4b05      	ldr	r3, [pc, #20]	; (1e04 <SYSCFG_OTP_RSIPEn+0x18>)
    1df0:	0010      	movs	r0, r2
    1df2:	6819      	ldr	r1, [r3, #0]
    1df4:	4b04      	ldr	r3, [pc, #16]	; (1e08 <SYSCFG_OTP_RSIPEn+0x1c>)
    1df6:	681b      	ldr	r3, [r3, #0]
    1df8:	0109      	lsls	r1, r1, #4
    1dfa:	d402      	bmi.n	1e02 <SYSCFG_OTP_RSIPEn+0x16>
    1dfc:	089b      	lsrs	r3, r3, #2
    1dfe:	439a      	bics	r2, r3
    1e00:	0010      	movs	r0, r2
    1e02:	4770      	bx	lr
    1e04:	42000100 	.word	0x42000100
    1e08:	42000368 	.word	0x42000368

00001e0c <SYSCFG_OTP_SBootEn>:
    1e0c:	2201      	movs	r2, #1
    1e0e:	4b05      	ldr	r3, [pc, #20]	; (1e24 <SYSCFG_OTP_SBootEn+0x18>)
    1e10:	0010      	movs	r0, r2
    1e12:	6819      	ldr	r1, [r3, #0]
    1e14:	4b04      	ldr	r3, [pc, #16]	; (1e28 <SYSCFG_OTP_SBootEn+0x1c>)
    1e16:	681b      	ldr	r3, [r3, #0]
    1e18:	0149      	lsls	r1, r1, #5
    1e1a:	d402      	bmi.n	1e22 <SYSCFG_OTP_SBootEn+0x16>
    1e1c:	08db      	lsrs	r3, r3, #3
    1e1e:	439a      	bics	r2, r3
    1e20:	0010      	movs	r0, r2
    1e22:	4770      	bx	lr
    1e24:	42000100 	.word	0x42000100
    1e28:	42000368 	.word	0x42000368

00001e2c <SYSCFG_OTP_LowBaudLog>:
    1e2c:	4b02      	ldr	r3, [pc, #8]	; (1e38 <SYSCFG_OTP_LowBaudLog+0xc>)
    1e2e:	6818      	ldr	r0, [r3, #0]
    1e30:	0180      	lsls	r0, r0, #6
    1e32:	0fc0      	lsrs	r0, r0, #31
    1e34:	4770      	bx	lr
    1e36:	46c0      	nop			; (mov r8, r8)
    1e38:	42000100 	.word	0x42000100

00001e3c <SYSCFG_OTP_DisBootLog>:
    1e3c:	4b02      	ldr	r3, [pc, #8]	; (1e48 <SYSCFG_OTP_DisBootLog+0xc>)
    1e3e:	6818      	ldr	r0, [r3, #0]
    1e40:	01c0      	lsls	r0, r0, #7
    1e42:	0fc0      	lsrs	r0, r0, #31
    1e44:	4770      	bx	lr
    1e46:	46c0      	nop			; (mov r8, r8)
    1e48:	42000100 	.word	0x42000100

00001e4c <SYSCFG_OTP_BigPageNand>:
    1e4c:	4b02      	ldr	r3, [pc, #8]	; (1e58 <SYSCFG_OTP_BigPageNand+0xc>)
    1e4e:	6818      	ldr	r0, [r3, #0]
    1e50:	0200      	lsls	r0, r0, #8
    1e52:	0fc0      	lsrs	r0, r0, #31
    1e54:	4770      	bx	lr
    1e56:	46c0      	nop			; (mov r8, r8)
    1e58:	42000100 	.word	0x42000100

00001e5c <SYSCFG_OTP_USBLoadFullSpeed>:
    1e5c:	4b02      	ldr	r3, [pc, #8]	; (1e68 <SYSCFG_OTP_USBLoadFullSpeed+0xc>)
    1e5e:	6818      	ldr	r0, [r3, #0]
    1e60:	0240      	lsls	r0, r0, #9
    1e62:	0fc0      	lsrs	r0, r0, #31
    1e64:	4770      	bx	lr
    1e66:	46c0      	nop			; (mov r8, r8)
    1e68:	42000100 	.word	0x42000100

00001e6c <SYSCFG_OTP_BootSelect>:
    1e6c:	4b02      	ldr	r3, [pc, #8]	; (1e78 <SYSCFG_OTP_BootSelect+0xc>)
    1e6e:	6818      	ldr	r0, [r3, #0]
    1e70:	0280      	lsls	r0, r0, #10
    1e72:	0f80      	lsrs	r0, r0, #30
    1e74:	4770      	bx	lr
    1e76:	46c0      	nop			; (mov r8, r8)
    1e78:	42000100 	.word	0x42000100

00001e7c <SYSCFG_OTP_BootFlashBaud>:
    1e7c:	4b02      	ldr	r3, [pc, #8]	; (1e88 <SYSCFG_OTP_BootFlashBaud+0xc>)
    1e7e:	6818      	ldr	r0, [r3, #0]
    1e80:	0300      	lsls	r0, r0, #12
    1e82:	0fc0      	lsrs	r0, r0, #31
    1e84:	4770      	bx	lr
    1e86:	46c0      	nop			; (mov r8, r8)
    1e88:	42000100 	.word	0x42000100

00001e8c <SYSCFG_OTP_SPICBootSpeedUpDis>:
    1e8c:	4b02      	ldr	r3, [pc, #8]	; (1e98 <SYSCFG_OTP_SPICBootSpeedUpDis+0xc>)
    1e8e:	6818      	ldr	r0, [r3, #0]
    1e90:	0340      	lsls	r0, r0, #13
    1e92:	0fc0      	lsrs	r0, r0, #31
    1e94:	4770      	bx	lr
    1e96:	46c0      	nop			; (mov r8, r8)
    1e98:	42000100 	.word	0x42000100

00001e9c <SYSCFG_OTP_SPICAddr4ByteEn>:
    1e9c:	4b02      	ldr	r3, [pc, #8]	; (1ea8 <SYSCFG_OTP_SPICAddr4ByteEn+0xc>)
    1e9e:	6818      	ldr	r0, [r3, #0]
    1ea0:	0380      	lsls	r0, r0, #14
    1ea2:	0fc0      	lsrs	r0, r0, #31
    1ea4:	4770      	bx	lr
    1ea6:	46c0      	nop			; (mov r8, r8)
    1ea8:	42000100 	.word	0x42000100

00001eac <SYSCFG_OTP_FlashDSleepEn>:
    1eac:	4b02      	ldr	r3, [pc, #8]	; (1eb8 <SYSCFG_OTP_FlashDSleepEn+0xc>)
    1eae:	6818      	ldr	r0, [r3, #0]
    1eb0:	03c0      	lsls	r0, r0, #15
    1eb2:	0fc0      	lsrs	r0, r0, #31
    1eb4:	4770      	bx	lr
    1eb6:	46c0      	nop			; (mov r8, r8)
    1eb8:	42000100 	.word	0x42000100

00001ebc <SYSCFG_OTP_UartDownloadDis>:
    1ebc:	2001      	movs	r0, #1
    1ebe:	4b02      	ldr	r3, [pc, #8]	; (1ec8 <SYSCFG_OTP_UartDownloadDis+0xc>)
    1ec0:	681b      	ldr	r3, [r3, #0]
    1ec2:	4398      	bics	r0, r3
    1ec4:	4770      	bx	lr
    1ec6:	46c0      	nop			; (mov r8, r8)
    1ec8:	42000368 	.word	0x42000368

00001ecc <SYSCFG_OTP_SBoot_HW_DIS>:
    1ecc:	2001      	movs	r0, #1
    1ece:	4b02      	ldr	r3, [pc, #8]	; (1ed8 <SYSCFG_OTP_SBoot_HW_DIS+0xc>)
    1ed0:	681b      	ldr	r3, [r3, #0]
    1ed2:	091b      	lsrs	r3, r3, #4
    1ed4:	4398      	bics	r0, r3
    1ed6:	4770      	bx	lr
    1ed8:	42000368 	.word	0x42000368

00001edc <SYSCFG_OTP_AntiRollback>:
    1edc:	2001      	movs	r0, #1
    1ede:	4b02      	ldr	r3, [pc, #8]	; (1ee8 <SYSCFG_OTP_AntiRollback+0xc>)
    1ee0:	681b      	ldr	r3, [r3, #0]
    1ee2:	099b      	lsrs	r3, r3, #6
    1ee4:	4398      	bics	r0, r3
    1ee6:	4770      	bx	lr
    1ee8:	42000368 	.word	0x42000368

00001eec <SYSCFG_OTP_FaultLogDis>:
    1eec:	2001      	movs	r0, #1
    1eee:	4b02      	ldr	r3, [pc, #8]	; (1ef8 <SYSCFG_OTP_FaultLogDis+0xc>)
    1ef0:	681b      	ldr	r3, [r3, #0]
    1ef2:	09db      	lsrs	r3, r3, #7
    1ef4:	4398      	bics	r0, r3
    1ef6:	4770      	bx	lr
    1ef8:	42000368 	.word	0x42000368

00001efc <SYSCFG_OTP_RSIPMode>:
    1efc:	2201      	movs	r2, #1
    1efe:	2000      	movs	r0, #0
    1f00:	4b03      	ldr	r3, [pc, #12]	; (1f10 <SYSCFG_OTP_RSIPMode+0x14>)
    1f02:	681b      	ldr	r3, [r3, #0]
    1f04:	059b      	lsls	r3, r3, #22
    1f06:	0f9b      	lsrs	r3, r3, #30
    1f08:	3b01      	subs	r3, #1
    1f0a:	429a      	cmp	r2, r3
    1f0c:	4140      	adcs	r0, r0
    1f0e:	4770      	bx	lr
    1f10:	42000368 	.word	0x42000368

00001f14 <SYSCFG_OTP_HUKDerivEn>:
    1f14:	2001      	movs	r0, #1
    1f16:	4b02      	ldr	r3, [pc, #8]	; (1f20 <SYSCFG_OTP_HUKDerivEn+0xc>)
    1f18:	681b      	ldr	r3, [r3, #0]
    1f1a:	0a9b      	lsrs	r3, r3, #10
    1f1c:	4398      	bics	r0, r3
    1f1e:	4770      	bx	lr
    1f20:	42000368 	.word	0x42000368

00001f24 <SYSCFG_OTP_RomPatchEn>:
    1f24:	2203      	movs	r2, #3
    1f26:	4b05      	ldr	r3, [pc, #20]	; (1f3c <SYSCFG_OTP_RomPatchEn+0x18>)
    1f28:	2000      	movs	r0, #0
    1f2a:	781b      	ldrb	r3, [r3, #0]
    1f2c:	4013      	ands	r3, r2
    1f2e:	4293      	cmp	r3, r2
    1f30:	d002      	beq.n	1f38 <SYSCFG_OTP_RomPatchEn+0x14>
    1f32:	0018      	movs	r0, r3
    1f34:	1e43      	subs	r3, r0, #1
    1f36:	4198      	sbcs	r0, r3
    1f38:	4770      	bx	lr
    1f3a:	46c0      	nop			; (mov r8, r8)
    1f3c:	42000701 	.word	0x42000701

00001f40 <SYSCFG_OTP_RandDelayEn>:
    1f40:	4b02      	ldr	r3, [pc, #8]	; (1f4c <SYSCFG_OTP_RandDelayEn+0xc>)
    1f42:	6818      	ldr	r0, [r3, #0]
    1f44:	0080      	lsls	r0, r0, #2
    1f46:	0fc0      	lsrs	r0, r0, #31
    1f48:	4770      	bx	lr
    1f4a:	46c0      	nop			; (mov r8, r8)
    1f4c:	42000100 	.word	0x42000100

00001f50 <SYSCFG_OTP_RMAMode>:
    1f50:	b510      	push	{r4, lr}
    1f52:	2208      	movs	r2, #8
    1f54:	2000      	movs	r0, #0
    1f56:	2401      	movs	r4, #1
    1f58:	4b06      	ldr	r3, [pc, #24]	; (1f74 <SYSCFG_OTP_RMAMode+0x24>)
    1f5a:	781b      	ldrb	r3, [r3, #0]
    1f5c:	b2db      	uxtb	r3, r3
    1f5e:	0019      	movs	r1, r3
    1f60:	3a01      	subs	r2, #1
    1f62:	4021      	ands	r1, r4
    1f64:	1840      	adds	r0, r0, r1
    1f66:	085b      	lsrs	r3, r3, #1
    1f68:	2a00      	cmp	r2, #0
    1f6a:	d1f8      	bne.n	1f5e <SYSCFG_OTP_RMAMode+0xe>
    1f6c:	2301      	movs	r3, #1
    1f6e:	4018      	ands	r0, r3
    1f70:	bd10      	pop	{r4, pc}
    1f72:	46c0      	nop			; (mov r8, r8)
    1f74:	42000700 	.word	0x42000700

00001f78 <SYSCFG_OTP_GetUsbPhyCalData>:
    1f78:	4b01      	ldr	r3, [pc, #4]	; (1f80 <SYSCFG_OTP_GetUsbPhyCalData+0x8>)
    1f7a:	6818      	ldr	r0, [r3, #0]
    1f7c:	4770      	bx	lr
    1f7e:	46c0      	nop			; (mov r8, r8)
    1f80:	42000108 	.word	0x42000108

00001f84 <SYSCFG_OTP_GetUsbPhyCalData2>:
    1f84:	4b01      	ldr	r3, [pc, #4]	; (1f8c <SYSCFG_OTP_GetUsbPhyCalData2+0x8>)
    1f86:	6818      	ldr	r0, [r3, #0]
    1f88:	4770      	bx	lr
    1f8a:	46c0      	nop			; (mov r8, r8)
    1f8c:	4200010c 	.word	0x4200010c

00001f90 <SYSCFG_OTP_UsbDownloadEnable>:
    1f90:	4b01      	ldr	r3, [pc, #4]	; (1f98 <SYSCFG_OTP_UsbDownloadEnable+0x8>)
    1f92:	6818      	ldr	r0, [r3, #0]
    1f94:	0fc0      	lsrs	r0, r0, #31
    1f96:	4770      	bx	lr
    1f98:	42000100 	.word	0x42000100

00001f9c <SYSCFG_BootFromNor>:
    1f9c:	b510      	push	{r4, lr}
    1f9e:	f7ff ff65 	bl	1e6c <SYSCFG_OTP_BootSelect>
    1fa2:	2301      	movs	r3, #1
    1fa4:	2802      	cmp	r0, #2
    1fa6:	d009      	beq.n	1fbc <SYSCFG_BootFromNor+0x20>
    1fa8:	f7ff ff60 	bl	1e6c <SYSCFG_OTP_BootSelect>
    1fac:	2300      	movs	r3, #0
    1fae:	2801      	cmp	r0, #1
    1fb0:	d004      	beq.n	1fbc <SYSCFG_BootFromNor+0x20>
    1fb2:	f7ff feeb 	bl	1d8c <SYSCFG_TRP_BootSel>
    1fb6:	0003      	movs	r3, r0
    1fb8:	1e5a      	subs	r2, r3, #1
    1fba:	4193      	sbcs	r3, r2
    1fbc:	0018      	movs	r0, r3
    1fbe:	bd10      	pop	{r4, pc}

00001fc0 <SYSCFG_HWTrigDownload>:
    1fc0:	b510      	push	{r4, lr}
    1fc2:	f7ff ff7b 	bl	1ebc <SYSCFG_OTP_UartDownloadDis>
    1fc6:	0003      	movs	r3, r0
    1fc8:	2000      	movs	r0, #0
    1fca:	b90b      	cbnz	r3, 1fd0 <SYSCFG_HWTrigDownload+0x10>
    1fcc:	f7ff fec0 	bl	1d50 <SYSCFG_TRP_UARTImage>
    1fd0:	bd10      	pop	{r4, pc}

00001fd2 <SYS_M0CtrlSel>:
    1fd2:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    1fd4:	2500      	movs	r5, #0
    1fd6:	2701      	movs	r7, #1
    1fd8:	f644 6620 	movw	r6, #20000	; 0x4e20
    1fdc:	9001      	str	r0, [sp, #4]
    1fde:	4b0e      	ldr	r3, [pc, #56]	; (2018 <SYS_M0CtrlSel+0x46>)
    1fe0:	2001      	movs	r0, #1
    1fe2:	681c      	ldr	r4, [r3, #0]
    1fe4:	4b0d      	ldr	r3, [pc, #52]	; (201c <SYS_M0CtrlSel+0x4a>)
    1fe6:	403c      	ands	r4, r7
    1fe8:	4798      	blx	r3
    1fea:	3501      	adds	r5, #1
    1fec:	b91c      	cbnz	r4, 1ff6 <SYS_M0CtrlSel+0x24>
    1fee:	42b5      	cmp	r5, r6
    1ff0:	d1f5      	bne.n	1fde <SYS_M0CtrlSel+0xc>
    1ff2:	0020      	movs	r0, r4
    1ff4:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    1ff6:	42b5      	cmp	r5, r6
    1ff8:	d00b      	beq.n	2012 <SYS_M0CtrlSel+0x40>
    1ffa:	4a09      	ldr	r2, [pc, #36]	; (2020 <SYS_M0CtrlSel+0x4e>)
    1ffc:	9901      	ldr	r1, [sp, #4]
    1ffe:	6810      	ldr	r0, [r2, #0]
    2000:	4b08      	ldr	r3, [pc, #32]	; (2024 <SYS_M0CtrlSel+0x52>)
    2002:	4003      	ands	r3, r0
    2004:	0508      	lsls	r0, r1, #20
    2006:	21c0      	movs	r1, #192	; 0xc0
    2008:	0389      	lsls	r1, r1, #14
    200a:	4008      	ands	r0, r1
    200c:	4318      	orrs	r0, r3
    200e:	6010      	str	r0, [r2, #0]
    2010:	e7ef      	b.n	1ff2 <SYS_M0CtrlSel+0x20>
    2012:	2400      	movs	r4, #0
    2014:	e7ed      	b.n	1ff2 <SYS_M0CtrlSel+0x20>
    2016:	46c0      	nop			; (mov r8, r8)
    2018:	42008244 	.word	0x42008244
    201c:	0000030b 	.word	0x0000030b
    2020:	42008240 	.word	0x42008240
    2024:	ffcfffff 	.word	0xffcfffff

00002028 <SYSCFG_SWTrigDownload>:
    2028:	4b07      	ldr	r3, [pc, #28]	; (2048 <SYSCFG_SWTrigDownload+0x20>)
    202a:	b510      	push	{r4, lr}
    202c:	681a      	ldr	r2, [r3, #0]
    202e:	f640 0300 	movw	r3, #2048	; 0x800
    2032:	0010      	movs	r0, r2
    2034:	4018      	ands	r0, r3
    2036:	421a      	tst	r2, r3
    2038:	d004      	beq.n	2044 <SYSCFG_SWTrigDownload+0x1c>
    203a:	2000      	movs	r0, #0
    203c:	4b03      	ldr	r3, [pc, #12]	; (204c <SYSCFG_SWTrigDownload+0x24>)
    203e:	4798      	blx	r3
    2040:	0580      	lsls	r0, r0, #22
    2042:	0fc0      	lsrs	r0, r0, #31
    2044:	bd10      	pop	{r4, pc}
    2046:	46c0      	nop			; (mov r8, r8)
    2048:	42000368 	.word	0x42000368
    204c:	00000159 	.word	0x00000159

00002050 <SYSTIMER_Init>:
    2050:	b570      	push	{r4, r5, r6, lr}
    2052:	b08a      	sub	sp, #40	; 0x28
    2054:	ac03      	add	r4, sp, #12
    2056:	0020      	movs	r0, r4
    2058:	4b0c      	ldr	r3, [pc, #48]	; (208c <SYSTIMER_Init+0x3c>)
    205a:	4798      	blx	r3
    205c:	2301      	movs	r3, #1
    205e:	2200      	movs	r2, #0
    2060:	2501      	movs	r5, #1
    2062:	4e0b      	ldr	r6, [pc, #44]	; (2090 <SYSTIMER_Init+0x40>)
    2064:	425b      	negs	r3, r3
    2066:	9304      	str	r3, [sp, #16]
    2068:	3305      	adds	r3, #5
    206a:	7522      	strb	r2, [r4, #20]
    206c:	0021      	movs	r1, r4
    206e:	0030      	movs	r0, r6
    2070:	9306      	str	r3, [sp, #24]
    2072:	9200      	str	r2, [sp, #0]
    2074:	0013      	movs	r3, r2
    2076:	4c07      	ldr	r4, [pc, #28]	; (2094 <SYSTIMER_Init+0x44>)
    2078:	9203      	str	r2, [sp, #12]
    207a:	9505      	str	r5, [sp, #20]
    207c:	9507      	str	r5, [sp, #28]
    207e:	47a0      	blx	r4
    2080:	0029      	movs	r1, r5
    2082:	0030      	movs	r0, r6
    2084:	4b04      	ldr	r3, [pc, #16]	; (2098 <SYSTIMER_Init+0x48>)
    2086:	4798      	blx	r3
    2088:	b00a      	add	sp, #40	; 0x28
    208a:	bd70      	pop	{r4, r5, r6, pc}
    208c:	00002105 	.word	0x00002105
    2090:	4200b000 	.word	0x4200b000
    2094:	00002645 	.word	0x00002645
    2098:	0000211d 	.word	0x0000211d

0000209c <SYSTIMER_TickGet>:
    209c:	b510      	push	{r4, lr}
    209e:	4802      	ldr	r0, [pc, #8]	; (20a8 <SYSTIMER_TickGet+0xc>)
    20a0:	4b02      	ldr	r3, [pc, #8]	; (20ac <SYSTIMER_TickGet+0x10>)
    20a2:	4798      	blx	r3
    20a4:	bd10      	pop	{r4, pc}
    20a6:	46c0      	nop			; (mov r8, r8)
    20a8:	4200b000 	.word	0x4200b000
    20ac:	00002271 	.word	0x00002271

000020b0 <SYSTIMER_GetPassTick>:
    20b0:	b510      	push	{r4, lr}
    20b2:	4b05      	ldr	r3, [pc, #20]	; (20c8 <SYSTIMER_GetPassTick+0x18>)
    20b4:	0004      	movs	r4, r0
    20b6:	4805      	ldr	r0, [pc, #20]	; (20cc <SYSTIMER_GetPassTick+0x1c>)
    20b8:	4798      	blx	r3
    20ba:	42a0      	cmp	r0, r4
    20bc:	d301      	bcc.n	20c2 <SYSTIMER_GetPassTick+0x12>
    20be:	1b00      	subs	r0, r0, r4
    20c0:	bd10      	pop	{r4, pc}
    20c2:	1a20      	subs	r0, r4, r0
    20c4:	43c0      	mvns	r0, r0
    20c6:	e7fb      	b.n	20c0 <SYSTIMER_GetPassTick+0x10>
    20c8:	00002271 	.word	0x00002271
    20cc:	4200b000 	.word	0x4200b000

000020d0 <SYSTIMER_GetPassTime>:
    20d0:	b510      	push	{r4, lr}
    20d2:	4b0a      	ldr	r3, [pc, #40]	; (20fc <SYSTIMER_GetPassTime+0x2c>)
    20d4:	0004      	movs	r4, r0
    20d6:	480a      	ldr	r0, [pc, #40]	; (2100 <SYSTIMER_GetPassTime+0x30>)
    20d8:	4798      	blx	r3
    20da:	42a0      	cmp	r0, r4
    20dc:	d30a      	bcc.n	20f4 <SYSTIMER_GetPassTime+0x24>
    20de:	1b04      	subs	r4, r0, r4
    20e0:	f240 33e8 	movw	r3, #1000	; 0x3e8
    20e4:	0be2      	lsrs	r2, r4, #15
    20e6:	0464      	lsls	r4, r4, #17
    20e8:	0c64      	lsrs	r4, r4, #17
    20ea:	435c      	muls	r4, r3
    20ec:	435a      	muls	r2, r3
    20ee:	0be0      	lsrs	r0, r4, #15
    20f0:	1810      	adds	r0, r2, r0
    20f2:	bd10      	pop	{r4, pc}
    20f4:	1a24      	subs	r4, r4, r0
    20f6:	43e4      	mvns	r4, r4
    20f8:	e7f2      	b.n	20e0 <SYSTIMER_GetPassTime+0x10>
    20fa:	46c0      	nop			; (mov r8, r8)
    20fc:	00002271 	.word	0x00002271
    2100:	4200b000 	.word	0x4200b000

00002104 <RTIM_TimeBaseStructInit>:
    2104:	f64f 72ff 	movw	r2, #65535	; 0xffff
    2108:	2300      	movs	r3, #0
    210a:	2104      	movs	r1, #4
    210c:	6042      	str	r2, [r0, #4]
    210e:	2201      	movs	r2, #1
    2110:	6003      	str	r3, [r0, #0]
    2112:	6082      	str	r2, [r0, #8]
    2114:	60c1      	str	r1, [r0, #12]
    2116:	6102      	str	r2, [r0, #16]
    2118:	6183      	str	r3, [r0, #24]
    211a:	4770      	bx	lr

0000211c <RTIM_Cmd>:
    211c:	4b37      	ldr	r3, [pc, #220]	; (21fc <RTIM_Cmd+0xe0>)
    211e:	b570      	push	{r4, r5, r6, lr}
    2120:	0004      	movs	r4, r0
    2122:	000d      	movs	r5, r1
    2124:	4298      	cmp	r0, r3
    2126:	d054      	beq.n	21d2 <RTIM_Cmd+0xb6>
    2128:	4b35      	ldr	r3, [pc, #212]	; (2200 <RTIM_Cmd+0xe4>)
    212a:	4298      	cmp	r0, r3
    212c:	d051      	beq.n	21d2 <RTIM_Cmd+0xb6>
    212e:	4b35      	ldr	r3, [pc, #212]	; (2204 <RTIM_Cmd+0xe8>)
    2130:	4298      	cmp	r0, r3
    2132:	d04e      	beq.n	21d2 <RTIM_Cmd+0xb6>
    2134:	4b34      	ldr	r3, [pc, #208]	; (2208 <RTIM_Cmd+0xec>)
    2136:	4298      	cmp	r0, r3
    2138:	d04b      	beq.n	21d2 <RTIM_Cmd+0xb6>
    213a:	4b34      	ldr	r3, [pc, #208]	; (220c <RTIM_Cmd+0xf0>)
    213c:	4298      	cmp	r0, r3
    213e:	d048      	beq.n	21d2 <RTIM_Cmd+0xb6>
    2140:	4b33      	ldr	r3, [pc, #204]	; (2210 <RTIM_Cmd+0xf4>)
    2142:	4298      	cmp	r0, r3
    2144:	d045      	beq.n	21d2 <RTIM_Cmd+0xb6>
    2146:	4b33      	ldr	r3, [pc, #204]	; (2214 <RTIM_Cmd+0xf8>)
    2148:	4298      	cmp	r0, r3
    214a:	d042      	beq.n	21d2 <RTIM_Cmd+0xb6>
    214c:	4b32      	ldr	r3, [pc, #200]	; (2218 <RTIM_Cmd+0xfc>)
    214e:	4298      	cmp	r0, r3
    2150:	d03f      	beq.n	21d2 <RTIM_Cmd+0xb6>
    2152:	4b32      	ldr	r3, [pc, #200]	; (221c <RTIM_Cmd+0x100>)
    2154:	4298      	cmp	r0, r3
    2156:	d03c      	beq.n	21d2 <RTIM_Cmd+0xb6>
    2158:	4b31      	ldr	r3, [pc, #196]	; (2220 <RTIM_Cmd+0x104>)
    215a:	4298      	cmp	r0, r3
    215c:	d039      	beq.n	21d2 <RTIM_Cmd+0xb6>
    215e:	4b31      	ldr	r3, [pc, #196]	; (2224 <RTIM_Cmd+0x108>)
    2160:	4298      	cmp	r0, r3
    2162:	d036      	beq.n	21d2 <RTIM_Cmd+0xb6>
    2164:	4b30      	ldr	r3, [pc, #192]	; (2228 <RTIM_Cmd+0x10c>)
    2166:	4298      	cmp	r0, r3
    2168:	d033      	beq.n	21d2 <RTIM_Cmd+0xb6>
    216a:	4b30      	ldr	r3, [pc, #192]	; (222c <RTIM_Cmd+0x110>)
    216c:	4298      	cmp	r0, r3
    216e:	d030      	beq.n	21d2 <RTIM_Cmd+0xb6>
    2170:	4b2f      	ldr	r3, [pc, #188]	; (2230 <RTIM_Cmd+0x114>)
    2172:	4298      	cmp	r0, r3
    2174:	d02d      	beq.n	21d2 <RTIM_Cmd+0xb6>
    2176:	4b2f      	ldr	r3, [pc, #188]	; (2234 <RTIM_Cmd+0x118>)
    2178:	4298      	cmp	r0, r3
    217a:	d02a      	beq.n	21d2 <RTIM_Cmd+0xb6>
    217c:	4b2e      	ldr	r3, [pc, #184]	; (2238 <RTIM_Cmd+0x11c>)
    217e:	4298      	cmp	r0, r3
    2180:	d027      	beq.n	21d2 <RTIM_Cmd+0xb6>
    2182:	4b2e      	ldr	r3, [pc, #184]	; (223c <RTIM_Cmd+0x120>)
    2184:	4298      	cmp	r0, r3
    2186:	d024      	beq.n	21d2 <RTIM_Cmd+0xb6>
    2188:	4b2d      	ldr	r3, [pc, #180]	; (2240 <RTIM_Cmd+0x124>)
    218a:	4298      	cmp	r0, r3
    218c:	d021      	beq.n	21d2 <RTIM_Cmd+0xb6>
    218e:	4b2d      	ldr	r3, [pc, #180]	; (2244 <RTIM_Cmd+0x128>)
    2190:	4298      	cmp	r0, r3
    2192:	d01e      	beq.n	21d2 <RTIM_Cmd+0xb6>
    2194:	4b2c      	ldr	r3, [pc, #176]	; (2248 <RTIM_Cmd+0x12c>)
    2196:	4298      	cmp	r0, r3
    2198:	d01b      	beq.n	21d2 <RTIM_Cmd+0xb6>
    219a:	4b2c      	ldr	r3, [pc, #176]	; (224c <RTIM_Cmd+0x130>)
    219c:	4298      	cmp	r0, r3
    219e:	d018      	beq.n	21d2 <RTIM_Cmd+0xb6>
    21a0:	4b2b      	ldr	r3, [pc, #172]	; (2250 <RTIM_Cmd+0x134>)
    21a2:	4298      	cmp	r0, r3
    21a4:	d015      	beq.n	21d2 <RTIM_Cmd+0xb6>
    21a6:	4b2b      	ldr	r3, [pc, #172]	; (2254 <RTIM_Cmd+0x138>)
    21a8:	4298      	cmp	r0, r3
    21aa:	d012      	beq.n	21d2 <RTIM_Cmd+0xb6>
    21ac:	4b2a      	ldr	r3, [pc, #168]	; (2258 <RTIM_Cmd+0x13c>)
    21ae:	4298      	cmp	r0, r3
    21b0:	d00f      	beq.n	21d2 <RTIM_Cmd+0xb6>
    21b2:	4b2a      	ldr	r3, [pc, #168]	; (225c <RTIM_Cmd+0x140>)
    21b4:	4298      	cmp	r0, r3
    21b6:	d00c      	beq.n	21d2 <RTIM_Cmd+0xb6>
    21b8:	4b29      	ldr	r3, [pc, #164]	; (2260 <RTIM_Cmd+0x144>)
    21ba:	4298      	cmp	r0, r3
    21bc:	d009      	beq.n	21d2 <RTIM_Cmd+0xb6>
    21be:	4b29      	ldr	r3, [pc, #164]	; (2264 <RTIM_Cmd+0x148>)
    21c0:	4298      	cmp	r0, r3
    21c2:	d006      	beq.n	21d2 <RTIM_Cmd+0xb6>
    21c4:	4b28      	ldr	r3, [pc, #160]	; (2268 <RTIM_Cmd+0x14c>)
    21c6:	4298      	cmp	r0, r3
    21c8:	d003      	beq.n	21d2 <RTIM_Cmd+0xb6>
    21ca:	21a1      	movs	r1, #161	; 0xa1
    21cc:	4827      	ldr	r0, [pc, #156]	; (226c <RTIM_Cmd+0x150>)
    21ce:	f7fd ffa1 	bl	114 <io_assert_failed>
    21d2:	f240 1300 	movw	r3, #256	; 0x100
    21d6:	6822      	ldr	r2, [r4, #0]
    21d8:	b13d      	cbz	r5, 21ea <RTIM_Cmd+0xce>
    21da:	421a      	tst	r2, r3
    21dc:	d101      	bne.n	21e2 <RTIM_Cmd+0xc6>
    21de:	2201      	movs	r2, #1
    21e0:	6022      	str	r2, [r4, #0]
    21e2:	6822      	ldr	r2, [r4, #0]
    21e4:	421a      	tst	r2, r3
    21e6:	d0fc      	beq.n	21e2 <RTIM_Cmd+0xc6>
    21e8:	bd70      	pop	{r4, r5, r6, pc}
    21ea:	421a      	tst	r2, r3
    21ec:	d001      	beq.n	21f2 <RTIM_Cmd+0xd6>
    21ee:	2202      	movs	r2, #2
    21f0:	6022      	str	r2, [r4, #0]
    21f2:	6822      	ldr	r2, [r4, #0]
    21f4:	421a      	tst	r2, r3
    21f6:	d1fc      	bne.n	21f2 <RTIM_Cmd+0xd6>
    21f8:	e7f6      	b.n	21e8 <RTIM_Cmd+0xcc>
    21fa:	46c0      	nop			; (mov r8, r8)
    21fc:	4200b000 	.word	0x4200b000
    2200:	4200b200 	.word	0x4200b200
    2204:	4200b400 	.word	0x4200b400
    2208:	4200b600 	.word	0x4200b600
    220c:	4200b800 	.word	0x4200b800
    2210:	4200ba00 	.word	0x4200ba00
    2214:	4200bc00 	.word	0x4200bc00
    2218:	4200be00 	.word	0x4200be00
    221c:	4100a000 	.word	0x4100a000
    2220:	4100a200 	.word	0x4100a200
    2224:	4100a400 	.word	0x4100a400
    2228:	4100a600 	.word	0x4100a600
    222c:	4100a800 	.word	0x4100a800
    2230:	4100aa00 	.word	0x4100aa00
    2234:	5200b000 	.word	0x5200b000
    2238:	5200b200 	.word	0x5200b200
    223c:	5200b400 	.word	0x5200b400
    2240:	5200b600 	.word	0x5200b600
    2244:	5200b800 	.word	0x5200b800
    2248:	5200ba00 	.word	0x5200ba00
    224c:	5200bc00 	.word	0x5200bc00
    2250:	5200be00 	.word	0x5200be00
    2254:	5100a000 	.word	0x5100a000
    2258:	5100a200 	.word	0x5100a200
    225c:	5100a400 	.word	0x5100a400
    2260:	5100a600 	.word	0x5100a600
    2264:	5100a800 	.word	0x5100a800
    2268:	5100aa00 	.word	0x5100aa00
    226c:	0000c50a 	.word	0x0000c50a

00002270 <RTIM_GetCount>:
    2270:	4b2f      	ldr	r3, [pc, #188]	; (2330 <RTIM_GetCount+0xc0>)
    2272:	b510      	push	{r4, lr}
    2274:	0004      	movs	r4, r0
    2276:	4298      	cmp	r0, r3
    2278:	d054      	beq.n	2324 <RTIM_GetCount+0xb4>
    227a:	4b2e      	ldr	r3, [pc, #184]	; (2334 <RTIM_GetCount+0xc4>)
    227c:	4298      	cmp	r0, r3
    227e:	d051      	beq.n	2324 <RTIM_GetCount+0xb4>
    2280:	4b2d      	ldr	r3, [pc, #180]	; (2338 <RTIM_GetCount+0xc8>)
    2282:	4298      	cmp	r0, r3
    2284:	d04e      	beq.n	2324 <RTIM_GetCount+0xb4>
    2286:	4b2d      	ldr	r3, [pc, #180]	; (233c <RTIM_GetCount+0xcc>)
    2288:	4298      	cmp	r0, r3
    228a:	d04b      	beq.n	2324 <RTIM_GetCount+0xb4>
    228c:	4b2c      	ldr	r3, [pc, #176]	; (2340 <RTIM_GetCount+0xd0>)
    228e:	4298      	cmp	r0, r3
    2290:	d048      	beq.n	2324 <RTIM_GetCount+0xb4>
    2292:	4b2c      	ldr	r3, [pc, #176]	; (2344 <RTIM_GetCount+0xd4>)
    2294:	4298      	cmp	r0, r3
    2296:	d045      	beq.n	2324 <RTIM_GetCount+0xb4>
    2298:	4b2b      	ldr	r3, [pc, #172]	; (2348 <RTIM_GetCount+0xd8>)
    229a:	4298      	cmp	r0, r3
    229c:	d042      	beq.n	2324 <RTIM_GetCount+0xb4>
    229e:	4b2b      	ldr	r3, [pc, #172]	; (234c <RTIM_GetCount+0xdc>)
    22a0:	4298      	cmp	r0, r3
    22a2:	d03f      	beq.n	2324 <RTIM_GetCount+0xb4>
    22a4:	4b2a      	ldr	r3, [pc, #168]	; (2350 <RTIM_GetCount+0xe0>)
    22a6:	4298      	cmp	r0, r3
    22a8:	d03c      	beq.n	2324 <RTIM_GetCount+0xb4>
    22aa:	4b2a      	ldr	r3, [pc, #168]	; (2354 <RTIM_GetCount+0xe4>)
    22ac:	4298      	cmp	r0, r3
    22ae:	d039      	beq.n	2324 <RTIM_GetCount+0xb4>
    22b0:	4b29      	ldr	r3, [pc, #164]	; (2358 <RTIM_GetCount+0xe8>)
    22b2:	4298      	cmp	r0, r3
    22b4:	d036      	beq.n	2324 <RTIM_GetCount+0xb4>
    22b6:	4b29      	ldr	r3, [pc, #164]	; (235c <RTIM_GetCount+0xec>)
    22b8:	4298      	cmp	r0, r3
    22ba:	d033      	beq.n	2324 <RTIM_GetCount+0xb4>
    22bc:	4b28      	ldr	r3, [pc, #160]	; (2360 <RTIM_GetCount+0xf0>)
    22be:	4298      	cmp	r0, r3
    22c0:	d030      	beq.n	2324 <RTIM_GetCount+0xb4>
    22c2:	4b28      	ldr	r3, [pc, #160]	; (2364 <RTIM_GetCount+0xf4>)
    22c4:	4298      	cmp	r0, r3
    22c6:	d02d      	beq.n	2324 <RTIM_GetCount+0xb4>
    22c8:	4b27      	ldr	r3, [pc, #156]	; (2368 <RTIM_GetCount+0xf8>)
    22ca:	4298      	cmp	r0, r3
    22cc:	d02a      	beq.n	2324 <RTIM_GetCount+0xb4>
    22ce:	4b27      	ldr	r3, [pc, #156]	; (236c <RTIM_GetCount+0xfc>)
    22d0:	4298      	cmp	r0, r3
    22d2:	d027      	beq.n	2324 <RTIM_GetCount+0xb4>
    22d4:	4b26      	ldr	r3, [pc, #152]	; (2370 <RTIM_GetCount+0x100>)
    22d6:	4298      	cmp	r0, r3
    22d8:	d024      	beq.n	2324 <RTIM_GetCount+0xb4>
    22da:	4b26      	ldr	r3, [pc, #152]	; (2374 <RTIM_GetCount+0x104>)
    22dc:	4298      	cmp	r0, r3
    22de:	d021      	beq.n	2324 <RTIM_GetCount+0xb4>
    22e0:	4b25      	ldr	r3, [pc, #148]	; (2378 <RTIM_GetCount+0x108>)
    22e2:	4298      	cmp	r0, r3
    22e4:	d01e      	beq.n	2324 <RTIM_GetCount+0xb4>
    22e6:	4b25      	ldr	r3, [pc, #148]	; (237c <RTIM_GetCount+0x10c>)
    22e8:	4298      	cmp	r0, r3
    22ea:	d01b      	beq.n	2324 <RTIM_GetCount+0xb4>
    22ec:	4b24      	ldr	r3, [pc, #144]	; (2380 <RTIM_GetCount+0x110>)
    22ee:	4298      	cmp	r0, r3
    22f0:	d018      	beq.n	2324 <RTIM_GetCount+0xb4>
    22f2:	4b24      	ldr	r3, [pc, #144]	; (2384 <RTIM_GetCount+0x114>)
    22f4:	4298      	cmp	r0, r3
    22f6:	d015      	beq.n	2324 <RTIM_GetCount+0xb4>
    22f8:	4b23      	ldr	r3, [pc, #140]	; (2388 <RTIM_GetCount+0x118>)
    22fa:	4298      	cmp	r0, r3
    22fc:	d012      	beq.n	2324 <RTIM_GetCount+0xb4>
    22fe:	4b23      	ldr	r3, [pc, #140]	; (238c <RTIM_GetCount+0x11c>)
    2300:	4298      	cmp	r0, r3
    2302:	d00f      	beq.n	2324 <RTIM_GetCount+0xb4>
    2304:	4b22      	ldr	r3, [pc, #136]	; (2390 <RTIM_GetCount+0x120>)
    2306:	4298      	cmp	r0, r3
    2308:	d00c      	beq.n	2324 <RTIM_GetCount+0xb4>
    230a:	4b22      	ldr	r3, [pc, #136]	; (2394 <RTIM_GetCount+0x124>)
    230c:	4298      	cmp	r0, r3
    230e:	d009      	beq.n	2324 <RTIM_GetCount+0xb4>
    2310:	4b21      	ldr	r3, [pc, #132]	; (2398 <RTIM_GetCount+0x128>)
    2312:	4298      	cmp	r0, r3
    2314:	d006      	beq.n	2324 <RTIM_GetCount+0xb4>
    2316:	4b21      	ldr	r3, [pc, #132]	; (239c <RTIM_GetCount+0x12c>)
    2318:	4298      	cmp	r0, r3
    231a:	d003      	beq.n	2324 <RTIM_GetCount+0xb4>
    231c:	21e0      	movs	r1, #224	; 0xe0
    231e:	4820      	ldr	r0, [pc, #128]	; (23a0 <RTIM_GetCount+0x130>)
    2320:	f7fd fef8 	bl	114 <io_assert_failed>
    2324:	6963      	ldr	r3, [r4, #20]
    2326:	6960      	ldr	r0, [r4, #20]
    2328:	4283      	cmp	r3, r0
    232a:	d000      	beq.n	232e <RTIM_GetCount+0xbe>
    232c:	6960      	ldr	r0, [r4, #20]
    232e:	bd10      	pop	{r4, pc}
    2330:	4200b000 	.word	0x4200b000
    2334:	4200b200 	.word	0x4200b200
    2338:	4200b400 	.word	0x4200b400
    233c:	4200b600 	.word	0x4200b600
    2340:	4200b800 	.word	0x4200b800
    2344:	4200ba00 	.word	0x4200ba00
    2348:	4200bc00 	.word	0x4200bc00
    234c:	4200be00 	.word	0x4200be00
    2350:	4100a000 	.word	0x4100a000
    2354:	4100a200 	.word	0x4100a200
    2358:	4100a400 	.word	0x4100a400
    235c:	4100a600 	.word	0x4100a600
    2360:	4100a800 	.word	0x4100a800
    2364:	4100aa00 	.word	0x4100aa00
    2368:	5200b000 	.word	0x5200b000
    236c:	5200b200 	.word	0x5200b200
    2370:	5200b400 	.word	0x5200b400
    2374:	5200b600 	.word	0x5200b600
    2378:	5200b800 	.word	0x5200b800
    237c:	5200ba00 	.word	0x5200ba00
    2380:	5200bc00 	.word	0x5200bc00
    2384:	5200be00 	.word	0x5200be00
    2388:	5100a000 	.word	0x5100a000
    238c:	5100a200 	.word	0x5100a200
    2390:	5100a400 	.word	0x5100a400
    2394:	5100a600 	.word	0x5100a600
    2398:	5100a800 	.word	0x5100a800
    239c:	5100aa00 	.word	0x5100aa00
    23a0:	0000c4f0 	.word	0x0000c4f0

000023a4 <RTIM_INTConfig>:
    23a4:	4b38      	ldr	r3, [pc, #224]	; (2488 <RTIM_INTConfig+0xe4>)
    23a6:	b570      	push	{r4, r5, r6, lr}
    23a8:	0004      	movs	r4, r0
    23aa:	000d      	movs	r5, r1
    23ac:	0016      	movs	r6, r2
    23ae:	4298      	cmp	r0, r3
    23b0:	d055      	beq.n	245e <RTIM_INTConfig+0xba>
    23b2:	4b36      	ldr	r3, [pc, #216]	; (248c <RTIM_INTConfig+0xe8>)
    23b4:	4298      	cmp	r0, r3
    23b6:	d052      	beq.n	245e <RTIM_INTConfig+0xba>
    23b8:	4b35      	ldr	r3, [pc, #212]	; (2490 <RTIM_INTConfig+0xec>)
    23ba:	4298      	cmp	r0, r3
    23bc:	d04f      	beq.n	245e <RTIM_INTConfig+0xba>
    23be:	4b35      	ldr	r3, [pc, #212]	; (2494 <RTIM_INTConfig+0xf0>)
    23c0:	4298      	cmp	r0, r3
    23c2:	d04c      	beq.n	245e <RTIM_INTConfig+0xba>
    23c4:	4b34      	ldr	r3, [pc, #208]	; (2498 <RTIM_INTConfig+0xf4>)
    23c6:	4298      	cmp	r0, r3
    23c8:	d049      	beq.n	245e <RTIM_INTConfig+0xba>
    23ca:	4b34      	ldr	r3, [pc, #208]	; (249c <RTIM_INTConfig+0xf8>)
    23cc:	4298      	cmp	r0, r3
    23ce:	d046      	beq.n	245e <RTIM_INTConfig+0xba>
    23d0:	4b33      	ldr	r3, [pc, #204]	; (24a0 <RTIM_INTConfig+0xfc>)
    23d2:	4298      	cmp	r0, r3
    23d4:	d043      	beq.n	245e <RTIM_INTConfig+0xba>
    23d6:	4b33      	ldr	r3, [pc, #204]	; (24a4 <RTIM_INTConfig+0x100>)
    23d8:	4298      	cmp	r0, r3
    23da:	d040      	beq.n	245e <RTIM_INTConfig+0xba>
    23dc:	4b32      	ldr	r3, [pc, #200]	; (24a8 <RTIM_INTConfig+0x104>)
    23de:	4298      	cmp	r0, r3
    23e0:	d03d      	beq.n	245e <RTIM_INTConfig+0xba>
    23e2:	4b32      	ldr	r3, [pc, #200]	; (24ac <RTIM_INTConfig+0x108>)
    23e4:	4298      	cmp	r0, r3
    23e6:	d03a      	beq.n	245e <RTIM_INTConfig+0xba>
    23e8:	4b31      	ldr	r3, [pc, #196]	; (24b0 <RTIM_INTConfig+0x10c>)
    23ea:	4298      	cmp	r0, r3
    23ec:	d037      	beq.n	245e <RTIM_INTConfig+0xba>
    23ee:	4b31      	ldr	r3, [pc, #196]	; (24b4 <RTIM_INTConfig+0x110>)
    23f0:	4298      	cmp	r0, r3
    23f2:	d034      	beq.n	245e <RTIM_INTConfig+0xba>
    23f4:	4b30      	ldr	r3, [pc, #192]	; (24b8 <RTIM_INTConfig+0x114>)
    23f6:	4298      	cmp	r0, r3
    23f8:	d031      	beq.n	245e <RTIM_INTConfig+0xba>
    23fa:	4b30      	ldr	r3, [pc, #192]	; (24bc <RTIM_INTConfig+0x118>)
    23fc:	4298      	cmp	r0, r3
    23fe:	d02e      	beq.n	245e <RTIM_INTConfig+0xba>
    2400:	4b2f      	ldr	r3, [pc, #188]	; (24c0 <RTIM_INTConfig+0x11c>)
    2402:	4298      	cmp	r0, r3
    2404:	d02b      	beq.n	245e <RTIM_INTConfig+0xba>
    2406:	4b2f      	ldr	r3, [pc, #188]	; (24c4 <RTIM_INTConfig+0x120>)
    2408:	4298      	cmp	r0, r3
    240a:	d028      	beq.n	245e <RTIM_INTConfig+0xba>
    240c:	4b2e      	ldr	r3, [pc, #184]	; (24c8 <RTIM_INTConfig+0x124>)
    240e:	4298      	cmp	r0, r3
    2410:	d025      	beq.n	245e <RTIM_INTConfig+0xba>
    2412:	4b2e      	ldr	r3, [pc, #184]	; (24cc <RTIM_INTConfig+0x128>)
    2414:	4298      	cmp	r0, r3
    2416:	d022      	beq.n	245e <RTIM_INTConfig+0xba>
    2418:	4b2d      	ldr	r3, [pc, #180]	; (24d0 <RTIM_INTConfig+0x12c>)
    241a:	4298      	cmp	r0, r3
    241c:	d01f      	beq.n	245e <RTIM_INTConfig+0xba>
    241e:	4b2d      	ldr	r3, [pc, #180]	; (24d4 <RTIM_INTConfig+0x130>)
    2420:	4298      	cmp	r0, r3
    2422:	d01c      	beq.n	245e <RTIM_INTConfig+0xba>
    2424:	4b2c      	ldr	r3, [pc, #176]	; (24d8 <RTIM_INTConfig+0x134>)
    2426:	4298      	cmp	r0, r3
    2428:	d019      	beq.n	245e <RTIM_INTConfig+0xba>
    242a:	4b2c      	ldr	r3, [pc, #176]	; (24dc <RTIM_INTConfig+0x138>)
    242c:	4298      	cmp	r0, r3
    242e:	d016      	beq.n	245e <RTIM_INTConfig+0xba>
    2430:	4b2b      	ldr	r3, [pc, #172]	; (24e0 <RTIM_INTConfig+0x13c>)
    2432:	4298      	cmp	r0, r3
    2434:	d013      	beq.n	245e <RTIM_INTConfig+0xba>
    2436:	4b2b      	ldr	r3, [pc, #172]	; (24e4 <RTIM_INTConfig+0x140>)
    2438:	4298      	cmp	r0, r3
    243a:	d010      	beq.n	245e <RTIM_INTConfig+0xba>
    243c:	4b2a      	ldr	r3, [pc, #168]	; (24e8 <RTIM_INTConfig+0x144>)
    243e:	4298      	cmp	r0, r3
    2440:	d00d      	beq.n	245e <RTIM_INTConfig+0xba>
    2442:	4b2a      	ldr	r3, [pc, #168]	; (24ec <RTIM_INTConfig+0x148>)
    2444:	4298      	cmp	r0, r3
    2446:	d00a      	beq.n	245e <RTIM_INTConfig+0xba>
    2448:	4b29      	ldr	r3, [pc, #164]	; (24f0 <RTIM_INTConfig+0x14c>)
    244a:	4298      	cmp	r0, r3
    244c:	d007      	beq.n	245e <RTIM_INTConfig+0xba>
    244e:	4b29      	ldr	r3, [pc, #164]	; (24f4 <RTIM_INTConfig+0x150>)
    2450:	4298      	cmp	r0, r3
    2452:	d004      	beq.n	245e <RTIM_INTConfig+0xba>
    2454:	f240 1111 	movw	r1, #273	; 0x111
    2458:	4827      	ldr	r0, [pc, #156]	; (24f8 <RTIM_INTConfig+0x154>)
    245a:	f7fd fe5b 	bl	114 <io_assert_failed>
    245e:	4b27      	ldr	r3, [pc, #156]	; (24fc <RTIM_INTConfig+0x158>)
    2460:	421d      	tst	r5, r3
    2462:	d102      	bne.n	246a <RTIM_INTConfig+0xc6>
    2464:	4b26      	ldr	r3, [pc, #152]	; (2500 <RTIM_INTConfig+0x15c>)
    2466:	421d      	tst	r5, r3
    2468:	d104      	bne.n	2474 <RTIM_INTConfig+0xd0>
    246a:	f240 1112 	movw	r1, #274	; 0x112
    246e:	4822      	ldr	r0, [pc, #136]	; (24f8 <RTIM_INTConfig+0x154>)
    2470:	f7fd fe50 	bl	114 <io_assert_failed>
    2474:	b11e      	cbz	r6, 247e <RTIM_INTConfig+0xda>
    2476:	68a1      	ldr	r1, [r4, #8]
    2478:	430d      	orrs	r5, r1
    247a:	60a5      	str	r5, [r4, #8]
    247c:	bd70      	pop	{r4, r5, r6, pc}
    247e:	68a3      	ldr	r3, [r4, #8]
    2480:	43ab      	bics	r3, r5
    2482:	60a3      	str	r3, [r4, #8]
    2484:	e7fa      	b.n	247c <RTIM_INTConfig+0xd8>
    2486:	46c0      	nop			; (mov r8, r8)
    2488:	4200b000 	.word	0x4200b000
    248c:	4200b200 	.word	0x4200b200
    2490:	4200b400 	.word	0x4200b400
    2494:	4200b600 	.word	0x4200b600
    2498:	4200b800 	.word	0x4200b800
    249c:	4200ba00 	.word	0x4200ba00
    24a0:	4200bc00 	.word	0x4200bc00
    24a4:	4200be00 	.word	0x4200be00
    24a8:	4100a000 	.word	0x4100a000
    24ac:	4100a200 	.word	0x4100a200
    24b0:	4100a400 	.word	0x4100a400
    24b4:	4100a600 	.word	0x4100a600
    24b8:	4100a800 	.word	0x4100a800
    24bc:	4100aa00 	.word	0x4100aa00
    24c0:	5200b000 	.word	0x5200b000
    24c4:	5200b200 	.word	0x5200b200
    24c8:	5200b400 	.word	0x5200b400
    24cc:	5200b600 	.word	0x5200b600
    24d0:	5200b800 	.word	0x5200b800
    24d4:	5200ba00 	.word	0x5200ba00
    24d8:	5200bc00 	.word	0x5200bc00
    24dc:	5200be00 	.word	0x5200be00
    24e0:	5100a000 	.word	0x5100a000
    24e4:	5100a200 	.word	0x5100a200
    24e8:	5100a400 	.word	0x5100a400
    24ec:	5100a600 	.word	0x5100a600
    24f0:	5100a800 	.word	0x5100a800
    24f4:	5100aa00 	.word	0x5100aa00
    24f8:	0000c4e1 	.word	0x0000c4e1
    24fc:	ffc0ff80 	.word	0xffc0ff80
    2500:	003f007f 	.word	0x003f007f

00002504 <RTIM_INTClear>:
    2504:	4b32      	ldr	r3, [pc, #200]	; (25d0 <RTIM_INTClear+0xcc>)
    2506:	b570      	push	{r4, r5, r6, lr}
    2508:	0004      	movs	r4, r0
    250a:	68c5      	ldr	r5, [r0, #12]
    250c:	4298      	cmp	r0, r3
    250e:	d055      	beq.n	25bc <RTIM_INTClear+0xb8>
    2510:	4b30      	ldr	r3, [pc, #192]	; (25d4 <RTIM_INTClear+0xd0>)
    2512:	4298      	cmp	r0, r3
    2514:	d052      	beq.n	25bc <RTIM_INTClear+0xb8>
    2516:	4b30      	ldr	r3, [pc, #192]	; (25d8 <RTIM_INTClear+0xd4>)
    2518:	4298      	cmp	r0, r3
    251a:	d04f      	beq.n	25bc <RTIM_INTClear+0xb8>
    251c:	4b2f      	ldr	r3, [pc, #188]	; (25dc <RTIM_INTClear+0xd8>)
    251e:	4298      	cmp	r0, r3
    2520:	d04c      	beq.n	25bc <RTIM_INTClear+0xb8>
    2522:	4b2f      	ldr	r3, [pc, #188]	; (25e0 <RTIM_INTClear+0xdc>)
    2524:	4298      	cmp	r0, r3
    2526:	d049      	beq.n	25bc <RTIM_INTClear+0xb8>
    2528:	4b2e      	ldr	r3, [pc, #184]	; (25e4 <RTIM_INTClear+0xe0>)
    252a:	4298      	cmp	r0, r3
    252c:	d046      	beq.n	25bc <RTIM_INTClear+0xb8>
    252e:	4b2e      	ldr	r3, [pc, #184]	; (25e8 <RTIM_INTClear+0xe4>)
    2530:	4298      	cmp	r0, r3
    2532:	d043      	beq.n	25bc <RTIM_INTClear+0xb8>
    2534:	4b2d      	ldr	r3, [pc, #180]	; (25ec <RTIM_INTClear+0xe8>)
    2536:	4298      	cmp	r0, r3
    2538:	d040      	beq.n	25bc <RTIM_INTClear+0xb8>
    253a:	4b2d      	ldr	r3, [pc, #180]	; (25f0 <RTIM_INTClear+0xec>)
    253c:	4298      	cmp	r0, r3
    253e:	d03d      	beq.n	25bc <RTIM_INTClear+0xb8>
    2540:	4b2c      	ldr	r3, [pc, #176]	; (25f4 <RTIM_INTClear+0xf0>)
    2542:	4298      	cmp	r0, r3
    2544:	d03a      	beq.n	25bc <RTIM_INTClear+0xb8>
    2546:	4b2c      	ldr	r3, [pc, #176]	; (25f8 <RTIM_INTClear+0xf4>)
    2548:	4298      	cmp	r0, r3
    254a:	d037      	beq.n	25bc <RTIM_INTClear+0xb8>
    254c:	4b2b      	ldr	r3, [pc, #172]	; (25fc <RTIM_INTClear+0xf8>)
    254e:	4298      	cmp	r0, r3
    2550:	d034      	beq.n	25bc <RTIM_INTClear+0xb8>
    2552:	4b2b      	ldr	r3, [pc, #172]	; (2600 <RTIM_INTClear+0xfc>)
    2554:	4298      	cmp	r0, r3
    2556:	d031      	beq.n	25bc <RTIM_INTClear+0xb8>
    2558:	4b2a      	ldr	r3, [pc, #168]	; (2604 <RTIM_INTClear+0x100>)
    255a:	4298      	cmp	r0, r3
    255c:	d02e      	beq.n	25bc <RTIM_INTClear+0xb8>
    255e:	4b2a      	ldr	r3, [pc, #168]	; (2608 <RTIM_INTClear+0x104>)
    2560:	4298      	cmp	r0, r3
    2562:	d02b      	beq.n	25bc <RTIM_INTClear+0xb8>
    2564:	4b29      	ldr	r3, [pc, #164]	; (260c <RTIM_INTClear+0x108>)
    2566:	4298      	cmp	r0, r3
    2568:	d028      	beq.n	25bc <RTIM_INTClear+0xb8>
    256a:	4b29      	ldr	r3, [pc, #164]	; (2610 <RTIM_INTClear+0x10c>)
    256c:	4298      	cmp	r0, r3
    256e:	d025      	beq.n	25bc <RTIM_INTClear+0xb8>
    2570:	4b28      	ldr	r3, [pc, #160]	; (2614 <RTIM_INTClear+0x110>)
    2572:	4298      	cmp	r0, r3
    2574:	d022      	beq.n	25bc <RTIM_INTClear+0xb8>
    2576:	4b28      	ldr	r3, [pc, #160]	; (2618 <RTIM_INTClear+0x114>)
    2578:	4298      	cmp	r0, r3
    257a:	d01f      	beq.n	25bc <RTIM_INTClear+0xb8>
    257c:	4b27      	ldr	r3, [pc, #156]	; (261c <RTIM_INTClear+0x118>)
    257e:	4298      	cmp	r0, r3
    2580:	d01c      	beq.n	25bc <RTIM_INTClear+0xb8>
    2582:	4b27      	ldr	r3, [pc, #156]	; (2620 <RTIM_INTClear+0x11c>)
    2584:	4298      	cmp	r0, r3
    2586:	d019      	beq.n	25bc <RTIM_INTClear+0xb8>
    2588:	4b26      	ldr	r3, [pc, #152]	; (2624 <RTIM_INTClear+0x120>)
    258a:	4298      	cmp	r0, r3
    258c:	d016      	beq.n	25bc <RTIM_INTClear+0xb8>
    258e:	4b26      	ldr	r3, [pc, #152]	; (2628 <RTIM_INTClear+0x124>)
    2590:	4298      	cmp	r0, r3
    2592:	d013      	beq.n	25bc <RTIM_INTClear+0xb8>
    2594:	4b25      	ldr	r3, [pc, #148]	; (262c <RTIM_INTClear+0x128>)
    2596:	4298      	cmp	r0, r3
    2598:	d010      	beq.n	25bc <RTIM_INTClear+0xb8>
    259a:	4b25      	ldr	r3, [pc, #148]	; (2630 <RTIM_INTClear+0x12c>)
    259c:	4298      	cmp	r0, r3
    259e:	d00d      	beq.n	25bc <RTIM_INTClear+0xb8>
    25a0:	4b24      	ldr	r3, [pc, #144]	; (2634 <RTIM_INTClear+0x130>)
    25a2:	4298      	cmp	r0, r3
    25a4:	d00a      	beq.n	25bc <RTIM_INTClear+0xb8>
    25a6:	4b24      	ldr	r3, [pc, #144]	; (2638 <RTIM_INTClear+0x134>)
    25a8:	4298      	cmp	r0, r3
    25aa:	d007      	beq.n	25bc <RTIM_INTClear+0xb8>
    25ac:	4b23      	ldr	r3, [pc, #140]	; (263c <RTIM_INTClear+0x138>)
    25ae:	4298      	cmp	r0, r3
    25b0:	d004      	beq.n	25bc <RTIM_INTClear+0xb8>
    25b2:	f240 1129 	movw	r1, #297	; 0x129
    25b6:	4822      	ldr	r0, [pc, #136]	; (2640 <RTIM_INTClear+0x13c>)
    25b8:	f7fd fdac 	bl	114 <io_assert_failed>
    25bc:	f240 122c 	movw	r2, #300	; 0x12c
    25c0:	60e5      	str	r5, [r4, #12]
    25c2:	3a01      	subs	r2, #1
    25c4:	b11a      	cbz	r2, 25ce <RTIM_INTClear+0xca>
    25c6:	68e3      	ldr	r3, [r4, #12]
    25c8:	021b      	lsls	r3, r3, #8
    25ca:	2b00      	cmp	r3, #0
    25cc:	d1f9      	bne.n	25c2 <RTIM_INTClear+0xbe>
    25ce:	bd70      	pop	{r4, r5, r6, pc}
    25d0:	4200b000 	.word	0x4200b000
    25d4:	4200b200 	.word	0x4200b200
    25d8:	4200b400 	.word	0x4200b400
    25dc:	4200b600 	.word	0x4200b600
    25e0:	4200b800 	.word	0x4200b800
    25e4:	4200ba00 	.word	0x4200ba00
    25e8:	4200bc00 	.word	0x4200bc00
    25ec:	4200be00 	.word	0x4200be00
    25f0:	4100a000 	.word	0x4100a000
    25f4:	4100a200 	.word	0x4100a200
    25f8:	4100a400 	.word	0x4100a400
    25fc:	4100a600 	.word	0x4100a600
    2600:	4100a800 	.word	0x4100a800
    2604:	4100aa00 	.word	0x4100aa00
    2608:	5200b000 	.word	0x5200b000
    260c:	5200b200 	.word	0x5200b200
    2610:	5200b400 	.word	0x5200b400
    2614:	5200b600 	.word	0x5200b600
    2618:	5200b800 	.word	0x5200b800
    261c:	5200ba00 	.word	0x5200ba00
    2620:	5200bc00 	.word	0x5200bc00
    2624:	5200be00 	.word	0x5200be00
    2628:	5100a000 	.word	0x5100a000
    262c:	5100a200 	.word	0x5100a200
    2630:	5100a400 	.word	0x5100a400
    2634:	5100a600 	.word	0x5100a600
    2638:	5100a800 	.word	0x5100a800
    263c:	5100aa00 	.word	0x5100aa00
    2640:	0000c4d3 	.word	0x0000c4d3

00002644 <RTIM_TimeBaseInit>:
    2644:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    2646:	9301      	str	r3, [sp, #4]
    2648:	4b61      	ldr	r3, [pc, #388]	; (27d0 <RTIM_TimeBaseInit+0x18c>)
    264a:	0004      	movs	r4, r0
    264c:	000d      	movs	r5, r1
    264e:	0016      	movs	r6, r2
    2650:	4f60      	ldr	r7, [pc, #384]	; (27d4 <RTIM_TimeBaseInit+0x190>)
    2652:	4298      	cmp	r0, r3
    2654:	d053      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    2656:	4b60      	ldr	r3, [pc, #384]	; (27d8 <RTIM_TimeBaseInit+0x194>)
    2658:	4298      	cmp	r0, r3
    265a:	d050      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    265c:	4b5f      	ldr	r3, [pc, #380]	; (27dc <RTIM_TimeBaseInit+0x198>)
    265e:	4298      	cmp	r0, r3
    2660:	d04d      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    2662:	4b5f      	ldr	r3, [pc, #380]	; (27e0 <RTIM_TimeBaseInit+0x19c>)
    2664:	4298      	cmp	r0, r3
    2666:	d04a      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    2668:	4b5e      	ldr	r3, [pc, #376]	; (27e4 <RTIM_TimeBaseInit+0x1a0>)
    266a:	4298      	cmp	r0, r3
    266c:	d047      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    266e:	4b5e      	ldr	r3, [pc, #376]	; (27e8 <RTIM_TimeBaseInit+0x1a4>)
    2670:	4298      	cmp	r0, r3
    2672:	d044      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    2674:	4b5d      	ldr	r3, [pc, #372]	; (27ec <RTIM_TimeBaseInit+0x1a8>)
    2676:	4298      	cmp	r0, r3
    2678:	d041      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    267a:	4b5d      	ldr	r3, [pc, #372]	; (27f0 <RTIM_TimeBaseInit+0x1ac>)
    267c:	4298      	cmp	r0, r3
    267e:	d03e      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    2680:	42b8      	cmp	r0, r7
    2682:	d03c      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    2684:	4b5b      	ldr	r3, [pc, #364]	; (27f4 <RTIM_TimeBaseInit+0x1b0>)
    2686:	4298      	cmp	r0, r3
    2688:	d039      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    268a:	4b5b      	ldr	r3, [pc, #364]	; (27f8 <RTIM_TimeBaseInit+0x1b4>)
    268c:	4298      	cmp	r0, r3
    268e:	d036      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    2690:	4b5a      	ldr	r3, [pc, #360]	; (27fc <RTIM_TimeBaseInit+0x1b8>)
    2692:	4298      	cmp	r0, r3
    2694:	d033      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    2696:	4b5a      	ldr	r3, [pc, #360]	; (2800 <RTIM_TimeBaseInit+0x1bc>)
    2698:	4298      	cmp	r0, r3
    269a:	d030      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    269c:	4b59      	ldr	r3, [pc, #356]	; (2804 <RTIM_TimeBaseInit+0x1c0>)
    269e:	4298      	cmp	r0, r3
    26a0:	d02d      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26a2:	4b59      	ldr	r3, [pc, #356]	; (2808 <RTIM_TimeBaseInit+0x1c4>)
    26a4:	4298      	cmp	r0, r3
    26a6:	d02a      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26a8:	4b58      	ldr	r3, [pc, #352]	; (280c <RTIM_TimeBaseInit+0x1c8>)
    26aa:	4298      	cmp	r0, r3
    26ac:	d027      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26ae:	4b58      	ldr	r3, [pc, #352]	; (2810 <RTIM_TimeBaseInit+0x1cc>)
    26b0:	4298      	cmp	r0, r3
    26b2:	d024      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26b4:	4b57      	ldr	r3, [pc, #348]	; (2814 <RTIM_TimeBaseInit+0x1d0>)
    26b6:	4298      	cmp	r0, r3
    26b8:	d021      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26ba:	4b57      	ldr	r3, [pc, #348]	; (2818 <RTIM_TimeBaseInit+0x1d4>)
    26bc:	4298      	cmp	r0, r3
    26be:	d01e      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26c0:	4b56      	ldr	r3, [pc, #344]	; (281c <RTIM_TimeBaseInit+0x1d8>)
    26c2:	4298      	cmp	r0, r3
    26c4:	d01b      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26c6:	4b56      	ldr	r3, [pc, #344]	; (2820 <RTIM_TimeBaseInit+0x1dc>)
    26c8:	4298      	cmp	r0, r3
    26ca:	d018      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26cc:	4b55      	ldr	r3, [pc, #340]	; (2824 <RTIM_TimeBaseInit+0x1e0>)
    26ce:	4298      	cmp	r0, r3
    26d0:	d015      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26d2:	4b55      	ldr	r3, [pc, #340]	; (2828 <RTIM_TimeBaseInit+0x1e4>)
    26d4:	4298      	cmp	r0, r3
    26d6:	d012      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26d8:	4b54      	ldr	r3, [pc, #336]	; (282c <RTIM_TimeBaseInit+0x1e8>)
    26da:	4298      	cmp	r0, r3
    26dc:	d00f      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26de:	4b54      	ldr	r3, [pc, #336]	; (2830 <RTIM_TimeBaseInit+0x1ec>)
    26e0:	4298      	cmp	r0, r3
    26e2:	d00c      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26e4:	4b53      	ldr	r3, [pc, #332]	; (2834 <RTIM_TimeBaseInit+0x1f0>)
    26e6:	4298      	cmp	r0, r3
    26e8:	d009      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26ea:	4b53      	ldr	r3, [pc, #332]	; (2838 <RTIM_TimeBaseInit+0x1f4>)
    26ec:	4298      	cmp	r0, r3
    26ee:	d006      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26f0:	4b52      	ldr	r3, [pc, #328]	; (283c <RTIM_TimeBaseInit+0x1f8>)
    26f2:	4298      	cmp	r0, r3
    26f4:	d003      	beq.n	26fe <RTIM_TimeBaseInit+0xba>
    26f6:	2141      	movs	r1, #65	; 0x41
    26f8:	4851      	ldr	r0, [pc, #324]	; (2840 <RTIM_TimeBaseInit+0x1fc>)
    26fa:	f7fd fd0b 	bl	114 <io_assert_failed>
    26fe:	2304      	movs	r3, #4
    2700:	68ea      	ldr	r2, [r5, #12]
    2702:	439a      	bics	r2, r3
    2704:	b11a      	cbz	r2, 270e <RTIM_TimeBaseInit+0xca>
    2706:	2142      	movs	r1, #66	; 0x42
    2708:	484d      	ldr	r0, [pc, #308]	; (2840 <RTIM_TimeBaseInit+0x1fc>)
    270a:	f7fd fd03 	bl	114 <io_assert_failed>
    270e:	f64f 73ff 	movw	r3, #65535	; 0xffff
    2712:	682a      	ldr	r2, [r5, #0]
    2714:	429a      	cmp	r2, r3
    2716:	d903      	bls.n	2720 <RTIM_TimeBaseInit+0xdc>
    2718:	2143      	movs	r1, #67	; 0x43
    271a:	4849      	ldr	r0, [pc, #292]	; (2840 <RTIM_TimeBaseInit+0x1fc>)
    271c:	f7fd fcfa 	bl	114 <io_assert_failed>
    2720:	69ab      	ldr	r3, [r5, #24]
    2722:	b11b      	cbz	r3, 272c <RTIM_TimeBaseInit+0xe8>
    2724:	2301      	movs	r3, #1
    2726:	6a22      	ldr	r2, [r4, #32]
    2728:	4313      	orrs	r3, r2
    272a:	6223      	str	r3, [r4, #32]
    272c:	2100      	movs	r1, #0
    272e:	0020      	movs	r0, r4
    2730:	4b44      	ldr	r3, [pc, #272]	; (2844 <RTIM_TimeBaseInit+0x200>)
    2732:	4798      	blx	r3
    2734:	2300      	movs	r3, #0
    2736:	0020      	movs	r0, r4
    2738:	60a3      	str	r3, [r4, #8]
    273a:	4b43      	ldr	r3, [pc, #268]	; (2848 <RTIM_TimeBaseInit+0x204>)
    273c:	4798      	blx	r3
    273e:	686b      	ldr	r3, [r5, #4]
    2740:	42bc      	cmp	r4, r7
    2742:	d008      	beq.n	2756 <RTIM_TimeBaseInit+0x112>
    2744:	4a2b      	ldr	r2, [pc, #172]	; (27f4 <RTIM_TimeBaseInit+0x1b0>)
    2746:	4294      	cmp	r4, r2
    2748:	d005      	beq.n	2756 <RTIM_TimeBaseInit+0x112>
    274a:	4a37      	ldr	r2, [pc, #220]	; (2828 <RTIM_TimeBaseInit+0x1e4>)
    274c:	4294      	cmp	r4, r2
    274e:	d002      	beq.n	2756 <RTIM_TimeBaseInit+0x112>
    2750:	4a36      	ldr	r2, [pc, #216]	; (282c <RTIM_TimeBaseInit+0x1e8>)
    2752:	4294      	cmp	r4, r2
    2754:	d12c      	bne.n	27b0 <RTIM_TimeBaseInit+0x16c>
    2756:	b29b      	uxth	r3, r3
    2758:	61e3      	str	r3, [r4, #28]
    275a:	42bc      	cmp	r4, r7
    275c:	d12b      	bne.n	27b6 <RTIM_TimeBaseInit+0x172>
    275e:	682b      	ldr	r3, [r5, #0]
    2760:	61a3      	str	r3, [r4, #24]
    2762:	6862      	ldr	r2, [r4, #4]
    2764:	2110      	movs	r1, #16
    2766:	0013      	movs	r3, r2
    2768:	6928      	ldr	r0, [r5, #16]
    276a:	438b      	bics	r3, r1
    276c:	b108      	cbz	r0, 2772 <RTIM_TimeBaseInit+0x12e>
    276e:	000b      	movs	r3, r1
    2770:	4313      	orrs	r3, r2
    2772:	68ea      	ldr	r2, [r5, #12]
    2774:	2a04      	cmp	r2, #4
    2776:	d128      	bne.n	27ca <RTIM_TimeBaseInit+0x186>
    2778:	4313      	orrs	r3, r2
    277a:	2102      	movs	r1, #2
    277c:	001a      	movs	r2, r3
    277e:	68a8      	ldr	r0, [r5, #8]
    2780:	438a      	bics	r2, r1
    2782:	b908      	cbnz	r0, 2788 <RTIM_TimeBaseInit+0x144>
    2784:	000a      	movs	r2, r1
    2786:	431a      	orrs	r2, r3
    2788:	9b01      	ldr	r3, [sp, #4]
    278a:	6062      	str	r2, [r4, #4]
    278c:	b143      	cbz	r3, 27a0 <RTIM_TimeBaseInit+0x15c>
    278e:	2303      	movs	r3, #3
    2790:	0031      	movs	r1, r6
    2792:	9a08      	ldr	r2, [sp, #32]
    2794:	9801      	ldr	r0, [sp, #4]
    2796:	4d2d      	ldr	r5, [pc, #180]	; (284c <RTIM_TimeBaseInit+0x208>)
    2798:	47a8      	blx	r5
    279a:	0030      	movs	r0, r6
    279c:	4b2c      	ldr	r3, [pc, #176]	; (2850 <RTIM_TimeBaseInit+0x20c>)
    279e:	4798      	blx	r3
    27a0:	2301      	movs	r3, #1
    27a2:	6123      	str	r3, [r4, #16]
    27a4:	68e3      	ldr	r3, [r4, #12]
    27a6:	2b00      	cmp	r3, #0
    27a8:	dafc      	bge.n	27a4 <RTIM_TimeBaseInit+0x160>
    27aa:	68e3      	ldr	r3, [r4, #12]
    27ac:	60e3      	str	r3, [r4, #12]
    27ae:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}
    27b0:	61e3      	str	r3, [r4, #28]
    27b2:	2300      	movs	r3, #0
    27b4:	e7d4      	b.n	2760 <RTIM_TimeBaseInit+0x11c>
    27b6:	4b0f      	ldr	r3, [pc, #60]	; (27f4 <RTIM_TimeBaseInit+0x1b0>)
    27b8:	429c      	cmp	r4, r3
    27ba:	d0d0      	beq.n	275e <RTIM_TimeBaseInit+0x11a>
    27bc:	4b1a      	ldr	r3, [pc, #104]	; (2828 <RTIM_TimeBaseInit+0x1e4>)
    27be:	429c      	cmp	r4, r3
    27c0:	d0cd      	beq.n	275e <RTIM_TimeBaseInit+0x11a>
    27c2:	4b1a      	ldr	r3, [pc, #104]	; (282c <RTIM_TimeBaseInit+0x1e8>)
    27c4:	429c      	cmp	r4, r3
    27c6:	d0ca      	beq.n	275e <RTIM_TimeBaseInit+0x11a>
    27c8:	e7f3      	b.n	27b2 <RTIM_TimeBaseInit+0x16e>
    27ca:	2204      	movs	r2, #4
    27cc:	4393      	bics	r3, r2
    27ce:	e7d4      	b.n	277a <RTIM_TimeBaseInit+0x136>
    27d0:	4200b000 	.word	0x4200b000
    27d4:	4100a000 	.word	0x4100a000
    27d8:	4200b200 	.word	0x4200b200
    27dc:	4200b400 	.word	0x4200b400
    27e0:	4200b600 	.word	0x4200b600
    27e4:	4200b800 	.word	0x4200b800
    27e8:	4200ba00 	.word	0x4200ba00
    27ec:	4200bc00 	.word	0x4200bc00
    27f0:	4200be00 	.word	0x4200be00
    27f4:	4100a200 	.word	0x4100a200
    27f8:	4100a400 	.word	0x4100a400
    27fc:	4100a600 	.word	0x4100a600
    2800:	4100a800 	.word	0x4100a800
    2804:	4100aa00 	.word	0x4100aa00
    2808:	5200b000 	.word	0x5200b000
    280c:	5200b200 	.word	0x5200b200
    2810:	5200b400 	.word	0x5200b400
    2814:	5200b600 	.word	0x5200b600
    2818:	5200b800 	.word	0x5200b800
    281c:	5200ba00 	.word	0x5200ba00
    2820:	5200bc00 	.word	0x5200bc00
    2824:	5200be00 	.word	0x5200be00
    2828:	5100a000 	.word	0x5100a000
    282c:	5100a200 	.word	0x5100a200
    2830:	5100a400 	.word	0x5100a400
    2834:	5100a600 	.word	0x5100a600
    2838:	5100a800 	.word	0x5100a800
    283c:	5100aa00 	.word	0x5100aa00
    2840:	0000c513 	.word	0x0000c513
    2844:	0000211d 	.word	0x0000211d
    2848:	00002505 	.word	0x00002505
    284c:	00003bb5 	.word	0x00003bb5
    2850:	00003a8d 	.word	0x00003a8d

00002854 <RTIM_DeInit>:
    2854:	4b31      	ldr	r3, [pc, #196]	; (291c <RTIM_DeInit+0xc8>)
    2856:	b510      	push	{r4, lr}
    2858:	0004      	movs	r4, r0
    285a:	4298      	cmp	r0, r3
    285c:	d054      	beq.n	2908 <RTIM_DeInit+0xb4>
    285e:	4b30      	ldr	r3, [pc, #192]	; (2920 <RTIM_DeInit+0xcc>)
    2860:	4298      	cmp	r0, r3
    2862:	d051      	beq.n	2908 <RTIM_DeInit+0xb4>
    2864:	4b2f      	ldr	r3, [pc, #188]	; (2924 <RTIM_DeInit+0xd0>)
    2866:	4298      	cmp	r0, r3
    2868:	d04e      	beq.n	2908 <RTIM_DeInit+0xb4>
    286a:	4b2f      	ldr	r3, [pc, #188]	; (2928 <RTIM_DeInit+0xd4>)
    286c:	4298      	cmp	r0, r3
    286e:	d04b      	beq.n	2908 <RTIM_DeInit+0xb4>
    2870:	4b2e      	ldr	r3, [pc, #184]	; (292c <RTIM_DeInit+0xd8>)
    2872:	4298      	cmp	r0, r3
    2874:	d048      	beq.n	2908 <RTIM_DeInit+0xb4>
    2876:	4b2e      	ldr	r3, [pc, #184]	; (2930 <RTIM_DeInit+0xdc>)
    2878:	4298      	cmp	r0, r3
    287a:	d045      	beq.n	2908 <RTIM_DeInit+0xb4>
    287c:	4b2d      	ldr	r3, [pc, #180]	; (2934 <RTIM_DeInit+0xe0>)
    287e:	4298      	cmp	r0, r3
    2880:	d042      	beq.n	2908 <RTIM_DeInit+0xb4>
    2882:	4b2d      	ldr	r3, [pc, #180]	; (2938 <RTIM_DeInit+0xe4>)
    2884:	4298      	cmp	r0, r3
    2886:	d03f      	beq.n	2908 <RTIM_DeInit+0xb4>
    2888:	4b2c      	ldr	r3, [pc, #176]	; (293c <RTIM_DeInit+0xe8>)
    288a:	4298      	cmp	r0, r3
    288c:	d03c      	beq.n	2908 <RTIM_DeInit+0xb4>
    288e:	4b2c      	ldr	r3, [pc, #176]	; (2940 <RTIM_DeInit+0xec>)
    2890:	4298      	cmp	r0, r3
    2892:	d039      	beq.n	2908 <RTIM_DeInit+0xb4>
    2894:	4b2b      	ldr	r3, [pc, #172]	; (2944 <RTIM_DeInit+0xf0>)
    2896:	4298      	cmp	r0, r3
    2898:	d036      	beq.n	2908 <RTIM_DeInit+0xb4>
    289a:	4b2b      	ldr	r3, [pc, #172]	; (2948 <RTIM_DeInit+0xf4>)
    289c:	4298      	cmp	r0, r3
    289e:	d033      	beq.n	2908 <RTIM_DeInit+0xb4>
    28a0:	4b2a      	ldr	r3, [pc, #168]	; (294c <RTIM_DeInit+0xf8>)
    28a2:	4298      	cmp	r0, r3
    28a4:	d030      	beq.n	2908 <RTIM_DeInit+0xb4>
    28a6:	4b2a      	ldr	r3, [pc, #168]	; (2950 <RTIM_DeInit+0xfc>)
    28a8:	4298      	cmp	r0, r3
    28aa:	d02d      	beq.n	2908 <RTIM_DeInit+0xb4>
    28ac:	4b29      	ldr	r3, [pc, #164]	; (2954 <RTIM_DeInit+0x100>)
    28ae:	4298      	cmp	r0, r3
    28b0:	d02a      	beq.n	2908 <RTIM_DeInit+0xb4>
    28b2:	4b29      	ldr	r3, [pc, #164]	; (2958 <RTIM_DeInit+0x104>)
    28b4:	4298      	cmp	r0, r3
    28b6:	d027      	beq.n	2908 <RTIM_DeInit+0xb4>
    28b8:	4b28      	ldr	r3, [pc, #160]	; (295c <RTIM_DeInit+0x108>)
    28ba:	4298      	cmp	r0, r3
    28bc:	d024      	beq.n	2908 <RTIM_DeInit+0xb4>
    28be:	4b28      	ldr	r3, [pc, #160]	; (2960 <RTIM_DeInit+0x10c>)
    28c0:	4298      	cmp	r0, r3
    28c2:	d021      	beq.n	2908 <RTIM_DeInit+0xb4>
    28c4:	4b27      	ldr	r3, [pc, #156]	; (2964 <RTIM_DeInit+0x110>)
    28c6:	4298      	cmp	r0, r3
    28c8:	d01e      	beq.n	2908 <RTIM_DeInit+0xb4>
    28ca:	4b27      	ldr	r3, [pc, #156]	; (2968 <RTIM_DeInit+0x114>)
    28cc:	4298      	cmp	r0, r3
    28ce:	d01b      	beq.n	2908 <RTIM_DeInit+0xb4>
    28d0:	4b26      	ldr	r3, [pc, #152]	; (296c <RTIM_DeInit+0x118>)
    28d2:	4298      	cmp	r0, r3
    28d4:	d018      	beq.n	2908 <RTIM_DeInit+0xb4>
    28d6:	4b26      	ldr	r3, [pc, #152]	; (2970 <RTIM_DeInit+0x11c>)
    28d8:	4298      	cmp	r0, r3
    28da:	d015      	beq.n	2908 <RTIM_DeInit+0xb4>
    28dc:	4b25      	ldr	r3, [pc, #148]	; (2974 <RTIM_DeInit+0x120>)
    28de:	4298      	cmp	r0, r3
    28e0:	d012      	beq.n	2908 <RTIM_DeInit+0xb4>
    28e2:	4b25      	ldr	r3, [pc, #148]	; (2978 <RTIM_DeInit+0x124>)
    28e4:	4298      	cmp	r0, r3
    28e6:	d00f      	beq.n	2908 <RTIM_DeInit+0xb4>
    28e8:	4b24      	ldr	r3, [pc, #144]	; (297c <RTIM_DeInit+0x128>)
    28ea:	4298      	cmp	r0, r3
    28ec:	d00c      	beq.n	2908 <RTIM_DeInit+0xb4>
    28ee:	4b24      	ldr	r3, [pc, #144]	; (2980 <RTIM_DeInit+0x12c>)
    28f0:	4298      	cmp	r0, r3
    28f2:	d009      	beq.n	2908 <RTIM_DeInit+0xb4>
    28f4:	4b23      	ldr	r3, [pc, #140]	; (2984 <RTIM_DeInit+0x130>)
    28f6:	4298      	cmp	r0, r3
    28f8:	d006      	beq.n	2908 <RTIM_DeInit+0xb4>
    28fa:	4b23      	ldr	r3, [pc, #140]	; (2988 <RTIM_DeInit+0x134>)
    28fc:	4298      	cmp	r0, r3
    28fe:	d003      	beq.n	2908 <RTIM_DeInit+0xb4>
    2900:	21c9      	movs	r1, #201	; 0xc9
    2902:	4822      	ldr	r0, [pc, #136]	; (298c <RTIM_DeInit+0x138>)
    2904:	f7fd fc06 	bl	114 <io_assert_failed>
    2908:	2100      	movs	r1, #0
    290a:	0020      	movs	r0, r4
    290c:	4b20      	ldr	r3, [pc, #128]	; (2990 <RTIM_DeInit+0x13c>)
    290e:	4798      	blx	r3
    2910:	0020      	movs	r0, r4
    2912:	4b20      	ldr	r3, [pc, #128]	; (2994 <RTIM_DeInit+0x140>)
    2914:	4798      	blx	r3
    2916:	2300      	movs	r3, #0
    2918:	60a3      	str	r3, [r4, #8]
    291a:	bd10      	pop	{r4, pc}
    291c:	4200b000 	.word	0x4200b000
    2920:	4200b200 	.word	0x4200b200
    2924:	4200b400 	.word	0x4200b400
    2928:	4200b600 	.word	0x4200b600
    292c:	4200b800 	.word	0x4200b800
    2930:	4200ba00 	.word	0x4200ba00
    2934:	4200bc00 	.word	0x4200bc00
    2938:	4200be00 	.word	0x4200be00
    293c:	4100a000 	.word	0x4100a000
    2940:	4100a200 	.word	0x4100a200
    2944:	4100a400 	.word	0x4100a400
    2948:	4100a600 	.word	0x4100a600
    294c:	4100a800 	.word	0x4100a800
    2950:	4100aa00 	.word	0x4100aa00
    2954:	5200b000 	.word	0x5200b000
    2958:	5200b200 	.word	0x5200b200
    295c:	5200b400 	.word	0x5200b400
    2960:	5200b600 	.word	0x5200b600
    2964:	5200b800 	.word	0x5200b800
    2968:	5200ba00 	.word	0x5200ba00
    296c:	5200bc00 	.word	0x5200bc00
    2970:	5200be00 	.word	0x5200be00
    2974:	5100a000 	.word	0x5100a000
    2978:	5100a200 	.word	0x5100a200
    297c:	5100a400 	.word	0x5100a400
    2980:	5100a600 	.word	0x5100a600
    2984:	5100a800 	.word	0x5100a800
    2988:	5100aa00 	.word	0x5100aa00
    298c:	0000c4fe 	.word	0x0000c4fe
    2990:	0000211d 	.word	0x0000211d
    2994:	00002505 	.word	0x00002505

00002998 <RTIM_INTClearPendingBit>:
    2998:	4b37      	ldr	r3, [pc, #220]	; (2a78 <RTIM_INTClearPendingBit+0xe0>)
    299a:	b570      	push	{r4, r5, r6, lr}
    299c:	0004      	movs	r4, r0
    299e:	000d      	movs	r5, r1
    29a0:	4298      	cmp	r0, r3
    29a2:	d055      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29a4:	4b35      	ldr	r3, [pc, #212]	; (2a7c <RTIM_INTClearPendingBit+0xe4>)
    29a6:	4298      	cmp	r0, r3
    29a8:	d052      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29aa:	4b35      	ldr	r3, [pc, #212]	; (2a80 <RTIM_INTClearPendingBit+0xe8>)
    29ac:	4298      	cmp	r0, r3
    29ae:	d04f      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29b0:	4b34      	ldr	r3, [pc, #208]	; (2a84 <RTIM_INTClearPendingBit+0xec>)
    29b2:	4298      	cmp	r0, r3
    29b4:	d04c      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29b6:	4b34      	ldr	r3, [pc, #208]	; (2a88 <RTIM_INTClearPendingBit+0xf0>)
    29b8:	4298      	cmp	r0, r3
    29ba:	d049      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29bc:	4b33      	ldr	r3, [pc, #204]	; (2a8c <RTIM_INTClearPendingBit+0xf4>)
    29be:	4298      	cmp	r0, r3
    29c0:	d046      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29c2:	4b33      	ldr	r3, [pc, #204]	; (2a90 <RTIM_INTClearPendingBit+0xf8>)
    29c4:	4298      	cmp	r0, r3
    29c6:	d043      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29c8:	4b32      	ldr	r3, [pc, #200]	; (2a94 <RTIM_INTClearPendingBit+0xfc>)
    29ca:	4298      	cmp	r0, r3
    29cc:	d040      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29ce:	4b32      	ldr	r3, [pc, #200]	; (2a98 <RTIM_INTClearPendingBit+0x100>)
    29d0:	4298      	cmp	r0, r3
    29d2:	d03d      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29d4:	4b31      	ldr	r3, [pc, #196]	; (2a9c <RTIM_INTClearPendingBit+0x104>)
    29d6:	4298      	cmp	r0, r3
    29d8:	d03a      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29da:	4b31      	ldr	r3, [pc, #196]	; (2aa0 <RTIM_INTClearPendingBit+0x108>)
    29dc:	4298      	cmp	r0, r3
    29de:	d037      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29e0:	4b30      	ldr	r3, [pc, #192]	; (2aa4 <RTIM_INTClearPendingBit+0x10c>)
    29e2:	4298      	cmp	r0, r3
    29e4:	d034      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29e6:	4b30      	ldr	r3, [pc, #192]	; (2aa8 <RTIM_INTClearPendingBit+0x110>)
    29e8:	4298      	cmp	r0, r3
    29ea:	d031      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29ec:	4b2f      	ldr	r3, [pc, #188]	; (2aac <RTIM_INTClearPendingBit+0x114>)
    29ee:	4298      	cmp	r0, r3
    29f0:	d02e      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29f2:	4b2f      	ldr	r3, [pc, #188]	; (2ab0 <RTIM_INTClearPendingBit+0x118>)
    29f4:	4298      	cmp	r0, r3
    29f6:	d02b      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29f8:	4b2e      	ldr	r3, [pc, #184]	; (2ab4 <RTIM_INTClearPendingBit+0x11c>)
    29fa:	4298      	cmp	r0, r3
    29fc:	d028      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    29fe:	4b2e      	ldr	r3, [pc, #184]	; (2ab8 <RTIM_INTClearPendingBit+0x120>)
    2a00:	4298      	cmp	r0, r3
    2a02:	d025      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    2a04:	4b2d      	ldr	r3, [pc, #180]	; (2abc <RTIM_INTClearPendingBit+0x124>)
    2a06:	4298      	cmp	r0, r3
    2a08:	d022      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    2a0a:	4b2d      	ldr	r3, [pc, #180]	; (2ac0 <RTIM_INTClearPendingBit+0x128>)
    2a0c:	4298      	cmp	r0, r3
    2a0e:	d01f      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    2a10:	4b2c      	ldr	r3, [pc, #176]	; (2ac4 <RTIM_INTClearPendingBit+0x12c>)
    2a12:	4298      	cmp	r0, r3
    2a14:	d01c      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    2a16:	4b2c      	ldr	r3, [pc, #176]	; (2ac8 <RTIM_INTClearPendingBit+0x130>)
    2a18:	4298      	cmp	r0, r3
    2a1a:	d019      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    2a1c:	4b2b      	ldr	r3, [pc, #172]	; (2acc <RTIM_INTClearPendingBit+0x134>)
    2a1e:	4298      	cmp	r0, r3
    2a20:	d016      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    2a22:	4b2b      	ldr	r3, [pc, #172]	; (2ad0 <RTIM_INTClearPendingBit+0x138>)
    2a24:	4298      	cmp	r0, r3
    2a26:	d013      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    2a28:	4b2a      	ldr	r3, [pc, #168]	; (2ad4 <RTIM_INTClearPendingBit+0x13c>)
    2a2a:	4298      	cmp	r0, r3
    2a2c:	d010      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    2a2e:	4b2a      	ldr	r3, [pc, #168]	; (2ad8 <RTIM_INTClearPendingBit+0x140>)
    2a30:	4298      	cmp	r0, r3
    2a32:	d00d      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    2a34:	4b29      	ldr	r3, [pc, #164]	; (2adc <RTIM_INTClearPendingBit+0x144>)
    2a36:	4298      	cmp	r0, r3
    2a38:	d00a      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    2a3a:	4b29      	ldr	r3, [pc, #164]	; (2ae0 <RTIM_INTClearPendingBit+0x148>)
    2a3c:	4298      	cmp	r0, r3
    2a3e:	d007      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    2a40:	4b28      	ldr	r3, [pc, #160]	; (2ae4 <RTIM_INTClearPendingBit+0x14c>)
    2a42:	4298      	cmp	r0, r3
    2a44:	d004      	beq.n	2a50 <RTIM_INTClearPendingBit+0xb8>
    2a46:	f240 1159 	movw	r1, #345	; 0x159
    2a4a:	4827      	ldr	r0, [pc, #156]	; (2ae8 <RTIM_INTClearPendingBit+0x150>)
    2a4c:	f7fd fb62 	bl	114 <io_assert_failed>
    2a50:	4b26      	ldr	r3, [pc, #152]	; (2aec <RTIM_INTClearPendingBit+0x154>)
    2a52:	421d      	tst	r5, r3
    2a54:	d102      	bne.n	2a5c <RTIM_INTClearPendingBit+0xc4>
    2a56:	4b26      	ldr	r3, [pc, #152]	; (2af0 <RTIM_INTClearPendingBit+0x158>)
    2a58:	421d      	tst	r5, r3
    2a5a:	d104      	bne.n	2a66 <RTIM_INTClearPendingBit+0xce>
    2a5c:	f240 115a 	movw	r1, #346	; 0x15a
    2a60:	4821      	ldr	r0, [pc, #132]	; (2ae8 <RTIM_INTClearPendingBit+0x150>)
    2a62:	f7fd fb57 	bl	114 <io_assert_failed>
    2a66:	f240 132c 	movw	r3, #300	; 0x12c
    2a6a:	60e5      	str	r5, [r4, #12]
    2a6c:	3b01      	subs	r3, #1
    2a6e:	b113      	cbz	r3, 2a76 <RTIM_INTClearPendingBit+0xde>
    2a70:	68e2      	ldr	r2, [r4, #12]
    2a72:	4215      	tst	r5, r2
    2a74:	d1fa      	bne.n	2a6c <RTIM_INTClearPendingBit+0xd4>
    2a76:	bd70      	pop	{r4, r5, r6, pc}
    2a78:	4200b000 	.word	0x4200b000
    2a7c:	4200b200 	.word	0x4200b200
    2a80:	4200b400 	.word	0x4200b400
    2a84:	4200b600 	.word	0x4200b600
    2a88:	4200b800 	.word	0x4200b800
    2a8c:	4200ba00 	.word	0x4200ba00
    2a90:	4200bc00 	.word	0x4200bc00
    2a94:	4200be00 	.word	0x4200be00
    2a98:	4100a000 	.word	0x4100a000
    2a9c:	4100a200 	.word	0x4100a200
    2aa0:	4100a400 	.word	0x4100a400
    2aa4:	4100a600 	.word	0x4100a600
    2aa8:	4100a800 	.word	0x4100a800
    2aac:	4100aa00 	.word	0x4100aa00
    2ab0:	5200b000 	.word	0x5200b000
    2ab4:	5200b200 	.word	0x5200b200
    2ab8:	5200b400 	.word	0x5200b400
    2abc:	5200b600 	.word	0x5200b600
    2ac0:	5200b800 	.word	0x5200b800
    2ac4:	5200ba00 	.word	0x5200ba00
    2ac8:	5200bc00 	.word	0x5200bc00
    2acc:	5200be00 	.word	0x5200be00
    2ad0:	5100a000 	.word	0x5100a000
    2ad4:	5100a200 	.word	0x5100a200
    2ad8:	5100a400 	.word	0x5100a400
    2adc:	5100a600 	.word	0x5100a600
    2ae0:	5100a800 	.word	0x5100a800
    2ae4:	5100aa00 	.word	0x5100aa00
    2ae8:	0000c4bb 	.word	0x0000c4bb
    2aec:	ffc0ff80 	.word	0xffc0ff80
    2af0:	003f007f 	.word	0x003f007f

00002af4 <RTIM_GetFlagStatus>:
    2af4:	4b41      	ldr	r3, [pc, #260]	; (2bfc <RTIM_GetFlagStatus+0x108>)
    2af6:	b570      	push	{r4, r5, r6, lr}
    2af8:	0004      	movs	r4, r0
    2afa:	000d      	movs	r5, r1
    2afc:	4298      	cmp	r0, r3
    2afe:	d055      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b00:	4b3f      	ldr	r3, [pc, #252]	; (2c00 <RTIM_GetFlagStatus+0x10c>)
    2b02:	4298      	cmp	r0, r3
    2b04:	d052      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b06:	4b3f      	ldr	r3, [pc, #252]	; (2c04 <RTIM_GetFlagStatus+0x110>)
    2b08:	4298      	cmp	r0, r3
    2b0a:	d04f      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b0c:	4b3e      	ldr	r3, [pc, #248]	; (2c08 <RTIM_GetFlagStatus+0x114>)
    2b0e:	4298      	cmp	r0, r3
    2b10:	d04c      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b12:	4b3e      	ldr	r3, [pc, #248]	; (2c0c <RTIM_GetFlagStatus+0x118>)
    2b14:	4298      	cmp	r0, r3
    2b16:	d049      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b18:	4b3d      	ldr	r3, [pc, #244]	; (2c10 <RTIM_GetFlagStatus+0x11c>)
    2b1a:	4298      	cmp	r0, r3
    2b1c:	d046      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b1e:	4b3d      	ldr	r3, [pc, #244]	; (2c14 <RTIM_GetFlagStatus+0x120>)
    2b20:	4298      	cmp	r0, r3
    2b22:	d043      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b24:	4b3c      	ldr	r3, [pc, #240]	; (2c18 <RTIM_GetFlagStatus+0x124>)
    2b26:	4298      	cmp	r0, r3
    2b28:	d040      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b2a:	4b3c      	ldr	r3, [pc, #240]	; (2c1c <RTIM_GetFlagStatus+0x128>)
    2b2c:	4298      	cmp	r0, r3
    2b2e:	d03d      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b30:	4b3b      	ldr	r3, [pc, #236]	; (2c20 <RTIM_GetFlagStatus+0x12c>)
    2b32:	4298      	cmp	r0, r3
    2b34:	d03a      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b36:	4b3b      	ldr	r3, [pc, #236]	; (2c24 <RTIM_GetFlagStatus+0x130>)
    2b38:	4298      	cmp	r0, r3
    2b3a:	d037      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b3c:	4b3a      	ldr	r3, [pc, #232]	; (2c28 <RTIM_GetFlagStatus+0x134>)
    2b3e:	4298      	cmp	r0, r3
    2b40:	d034      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b42:	4b3a      	ldr	r3, [pc, #232]	; (2c2c <RTIM_GetFlagStatus+0x138>)
    2b44:	4298      	cmp	r0, r3
    2b46:	d031      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b48:	4b39      	ldr	r3, [pc, #228]	; (2c30 <RTIM_GetFlagStatus+0x13c>)
    2b4a:	4298      	cmp	r0, r3
    2b4c:	d02e      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b4e:	4b39      	ldr	r3, [pc, #228]	; (2c34 <RTIM_GetFlagStatus+0x140>)
    2b50:	4298      	cmp	r0, r3
    2b52:	d02b      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b54:	4b38      	ldr	r3, [pc, #224]	; (2c38 <RTIM_GetFlagStatus+0x144>)
    2b56:	4298      	cmp	r0, r3
    2b58:	d028      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b5a:	4b38      	ldr	r3, [pc, #224]	; (2c3c <RTIM_GetFlagStatus+0x148>)
    2b5c:	4298      	cmp	r0, r3
    2b5e:	d025      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b60:	4b37      	ldr	r3, [pc, #220]	; (2c40 <RTIM_GetFlagStatus+0x14c>)
    2b62:	4298      	cmp	r0, r3
    2b64:	d022      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b66:	4b37      	ldr	r3, [pc, #220]	; (2c44 <RTIM_GetFlagStatus+0x150>)
    2b68:	4298      	cmp	r0, r3
    2b6a:	d01f      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b6c:	4b36      	ldr	r3, [pc, #216]	; (2c48 <RTIM_GetFlagStatus+0x154>)
    2b6e:	4298      	cmp	r0, r3
    2b70:	d01c      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b72:	4b36      	ldr	r3, [pc, #216]	; (2c4c <RTIM_GetFlagStatus+0x158>)
    2b74:	4298      	cmp	r0, r3
    2b76:	d019      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b78:	4b35      	ldr	r3, [pc, #212]	; (2c50 <RTIM_GetFlagStatus+0x15c>)
    2b7a:	4298      	cmp	r0, r3
    2b7c:	d016      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b7e:	4b35      	ldr	r3, [pc, #212]	; (2c54 <RTIM_GetFlagStatus+0x160>)
    2b80:	4298      	cmp	r0, r3
    2b82:	d013      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b84:	4b34      	ldr	r3, [pc, #208]	; (2c58 <RTIM_GetFlagStatus+0x164>)
    2b86:	4298      	cmp	r0, r3
    2b88:	d010      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b8a:	4b34      	ldr	r3, [pc, #208]	; (2c5c <RTIM_GetFlagStatus+0x168>)
    2b8c:	4298      	cmp	r0, r3
    2b8e:	d00d      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b90:	4b33      	ldr	r3, [pc, #204]	; (2c60 <RTIM_GetFlagStatus+0x16c>)
    2b92:	4298      	cmp	r0, r3
    2b94:	d00a      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b96:	4b33      	ldr	r3, [pc, #204]	; (2c64 <RTIM_GetFlagStatus+0x170>)
    2b98:	4298      	cmp	r0, r3
    2b9a:	d007      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2b9c:	4b32      	ldr	r3, [pc, #200]	; (2c68 <RTIM_GetFlagStatus+0x174>)
    2b9e:	4298      	cmp	r0, r3
    2ba0:	d004      	beq.n	2bac <RTIM_GetFlagStatus+0xb8>
    2ba2:	f240 118b 	movw	r1, #395	; 0x18b
    2ba6:	4831      	ldr	r0, [pc, #196]	; (2c6c <RTIM_GetFlagStatus+0x178>)
    2ba8:	f7fd fab4 	bl	114 <io_assert_failed>
    2bac:	1e6b      	subs	r3, r5, #1
    2bae:	2b01      	cmp	r3, #1
    2bb0:	d91e      	bls.n	2bf0 <RTIM_GetFlagStatus+0xfc>
    2bb2:	2204      	movs	r2, #4
    2bb4:	1f2b      	subs	r3, r5, #4
    2bb6:	4393      	bics	r3, r2
    2bb8:	b1d3      	cbz	r3, 2bf0 <RTIM_GetFlagStatus+0xfc>
    2bba:	002b      	movs	r3, r5
    2bbc:	320c      	adds	r2, #12
    2bbe:	3b10      	subs	r3, #16
    2bc0:	4393      	bics	r3, r2
    2bc2:	b1ab      	cbz	r3, 2bf0 <RTIM_GetFlagStatus+0xfc>
    2bc4:	2d40      	cmp	r5, #64	; 0x40
    2bc6:	d013      	beq.n	2bf0 <RTIM_GetFlagStatus+0xfc>
    2bc8:	4b29      	ldr	r3, [pc, #164]	; (2c70 <RTIM_GetFlagStatus+0x17c>)
    2bca:	4a2a      	ldr	r2, [pc, #168]	; (2c74 <RTIM_GetFlagStatus+0x180>)
    2bcc:	18eb      	adds	r3, r5, r3
    2bce:	4213      	tst	r3, r2
    2bd0:	d00e      	beq.n	2bf0 <RTIM_GetFlagStatus+0xfc>
    2bd2:	4b29      	ldr	r3, [pc, #164]	; (2c78 <RTIM_GetFlagStatus+0x184>)
    2bd4:	4a29      	ldr	r2, [pc, #164]	; (2c7c <RTIM_GetFlagStatus+0x188>)
    2bd6:	18eb      	adds	r3, r5, r3
    2bd8:	4213      	tst	r3, r2
    2bda:	d009      	beq.n	2bf0 <RTIM_GetFlagStatus+0xfc>
    2bdc:	4b28      	ldr	r3, [pc, #160]	; (2c80 <RTIM_GetFlagStatus+0x18c>)
    2bde:	4a29      	ldr	r2, [pc, #164]	; (2c84 <RTIM_GetFlagStatus+0x190>)
    2be0:	18eb      	adds	r3, r5, r3
    2be2:	4213      	tst	r3, r2
    2be4:	d004      	beq.n	2bf0 <RTIM_GetFlagStatus+0xfc>
    2be6:	f240 118c 	movw	r1, #396	; 0x18c
    2bea:	4820      	ldr	r0, [pc, #128]	; (2c6c <RTIM_GetFlagStatus+0x178>)
    2bec:	f7fd fa92 	bl	114 <io_assert_failed>
    2bf0:	68e0      	ldr	r0, [r4, #12]
    2bf2:	4028      	ands	r0, r5
    2bf4:	1e43      	subs	r3, r0, #1
    2bf6:	4198      	sbcs	r0, r3
    2bf8:	bd70      	pop	{r4, r5, r6, pc}
    2bfa:	46c0      	nop			; (mov r8, r8)
    2bfc:	4200b000 	.word	0x4200b000
    2c00:	4200b200 	.word	0x4200b200
    2c04:	4200b400 	.word	0x4200b400
    2c08:	4200b600 	.word	0x4200b600
    2c0c:	4200b800 	.word	0x4200b800
    2c10:	4200ba00 	.word	0x4200ba00
    2c14:	4200bc00 	.word	0x4200bc00
    2c18:	4200be00 	.word	0x4200be00
    2c1c:	4100a000 	.word	0x4100a000
    2c20:	4100a200 	.word	0x4100a200
    2c24:	4100a400 	.word	0x4100a400
    2c28:	4100a600 	.word	0x4100a600
    2c2c:	4100a800 	.word	0x4100a800
    2c30:	4100aa00 	.word	0x4100aa00
    2c34:	5200b000 	.word	0x5200b000
    2c38:	5200b200 	.word	0x5200b200
    2c3c:	5200b400 	.word	0x5200b400
    2c40:	5200b600 	.word	0x5200b600
    2c44:	5200b800 	.word	0x5200b800
    2c48:	5200ba00 	.word	0x5200ba00
    2c4c:	5200bc00 	.word	0x5200bc00
    2c50:	5200be00 	.word	0x5200be00
    2c54:	5100a000 	.word	0x5100a000
    2c58:	5100a200 	.word	0x5100a200
    2c5c:	5100a400 	.word	0x5100a400
    2c60:	5100a600 	.word	0x5100a600
    2c64:	5100a800 	.word	0x5100a800
    2c68:	5100aa00 	.word	0x5100aa00
    2c6c:	0000c4a8 	.word	0x0000c4a8
    2c70:	ffff0000 	.word	0xffff0000
    2c74:	fffeffff 	.word	0xfffeffff
    2c78:	fffc0000 	.word	0xfffc0000
    2c7c:	fffbffff 	.word	0xfffbffff
    2c80:	fff00000 	.word	0xfff00000
    2c84:	ffefffff 	.word	0xffefffff

00002c88 <RTIM_GetINTStatus>:
    2c88:	4b44      	ldr	r3, [pc, #272]	; (2d9c <RTIM_GetINTStatus+0x114>)
    2c8a:	b570      	push	{r4, r5, r6, lr}
    2c8c:	0004      	movs	r4, r0
    2c8e:	000d      	movs	r5, r1
    2c90:	4298      	cmp	r0, r3
    2c92:	d055      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2c94:	4b42      	ldr	r3, [pc, #264]	; (2da0 <RTIM_GetINTStatus+0x118>)
    2c96:	4298      	cmp	r0, r3
    2c98:	d052      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2c9a:	4b42      	ldr	r3, [pc, #264]	; (2da4 <RTIM_GetINTStatus+0x11c>)
    2c9c:	4298      	cmp	r0, r3
    2c9e:	d04f      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2ca0:	4b41      	ldr	r3, [pc, #260]	; (2da8 <RTIM_GetINTStatus+0x120>)
    2ca2:	4298      	cmp	r0, r3
    2ca4:	d04c      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2ca6:	4b41      	ldr	r3, [pc, #260]	; (2dac <RTIM_GetINTStatus+0x124>)
    2ca8:	4298      	cmp	r0, r3
    2caa:	d049      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2cac:	4b40      	ldr	r3, [pc, #256]	; (2db0 <RTIM_GetINTStatus+0x128>)
    2cae:	4298      	cmp	r0, r3
    2cb0:	d046      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2cb2:	4b40      	ldr	r3, [pc, #256]	; (2db4 <RTIM_GetINTStatus+0x12c>)
    2cb4:	4298      	cmp	r0, r3
    2cb6:	d043      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2cb8:	4b3f      	ldr	r3, [pc, #252]	; (2db8 <RTIM_GetINTStatus+0x130>)
    2cba:	4298      	cmp	r0, r3
    2cbc:	d040      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2cbe:	4b3f      	ldr	r3, [pc, #252]	; (2dbc <RTIM_GetINTStatus+0x134>)
    2cc0:	4298      	cmp	r0, r3
    2cc2:	d03d      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2cc4:	4b3e      	ldr	r3, [pc, #248]	; (2dc0 <RTIM_GetINTStatus+0x138>)
    2cc6:	4298      	cmp	r0, r3
    2cc8:	d03a      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2cca:	4b3e      	ldr	r3, [pc, #248]	; (2dc4 <RTIM_GetINTStatus+0x13c>)
    2ccc:	4298      	cmp	r0, r3
    2cce:	d037      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2cd0:	4b3d      	ldr	r3, [pc, #244]	; (2dc8 <RTIM_GetINTStatus+0x140>)
    2cd2:	4298      	cmp	r0, r3
    2cd4:	d034      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2cd6:	4b3d      	ldr	r3, [pc, #244]	; (2dcc <RTIM_GetINTStatus+0x144>)
    2cd8:	4298      	cmp	r0, r3
    2cda:	d031      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2cdc:	4b3c      	ldr	r3, [pc, #240]	; (2dd0 <RTIM_GetINTStatus+0x148>)
    2cde:	4298      	cmp	r0, r3
    2ce0:	d02e      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2ce2:	4b3c      	ldr	r3, [pc, #240]	; (2dd4 <RTIM_GetINTStatus+0x14c>)
    2ce4:	4298      	cmp	r0, r3
    2ce6:	d02b      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2ce8:	4b3b      	ldr	r3, [pc, #236]	; (2dd8 <RTIM_GetINTStatus+0x150>)
    2cea:	4298      	cmp	r0, r3
    2cec:	d028      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2cee:	4b3b      	ldr	r3, [pc, #236]	; (2ddc <RTIM_GetINTStatus+0x154>)
    2cf0:	4298      	cmp	r0, r3
    2cf2:	d025      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2cf4:	4b3a      	ldr	r3, [pc, #232]	; (2de0 <RTIM_GetINTStatus+0x158>)
    2cf6:	4298      	cmp	r0, r3
    2cf8:	d022      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2cfa:	4b3a      	ldr	r3, [pc, #232]	; (2de4 <RTIM_GetINTStatus+0x15c>)
    2cfc:	4298      	cmp	r0, r3
    2cfe:	d01f      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2d00:	4b39      	ldr	r3, [pc, #228]	; (2de8 <RTIM_GetINTStatus+0x160>)
    2d02:	4298      	cmp	r0, r3
    2d04:	d01c      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2d06:	4b39      	ldr	r3, [pc, #228]	; (2dec <RTIM_GetINTStatus+0x164>)
    2d08:	4298      	cmp	r0, r3
    2d0a:	d019      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2d0c:	4b38      	ldr	r3, [pc, #224]	; (2df0 <RTIM_GetINTStatus+0x168>)
    2d0e:	4298      	cmp	r0, r3
    2d10:	d016      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2d12:	4b38      	ldr	r3, [pc, #224]	; (2df4 <RTIM_GetINTStatus+0x16c>)
    2d14:	4298      	cmp	r0, r3
    2d16:	d013      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2d18:	4b37      	ldr	r3, [pc, #220]	; (2df8 <RTIM_GetINTStatus+0x170>)
    2d1a:	4298      	cmp	r0, r3
    2d1c:	d010      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2d1e:	4b37      	ldr	r3, [pc, #220]	; (2dfc <RTIM_GetINTStatus+0x174>)
    2d20:	4298      	cmp	r0, r3
    2d22:	d00d      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2d24:	4b36      	ldr	r3, [pc, #216]	; (2e00 <RTIM_GetINTStatus+0x178>)
    2d26:	4298      	cmp	r0, r3
    2d28:	d00a      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2d2a:	4b36      	ldr	r3, [pc, #216]	; (2e04 <RTIM_GetINTStatus+0x17c>)
    2d2c:	4298      	cmp	r0, r3
    2d2e:	d007      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2d30:	4b35      	ldr	r3, [pc, #212]	; (2e08 <RTIM_GetINTStatus+0x180>)
    2d32:	4298      	cmp	r0, r3
    2d34:	d004      	beq.n	2d40 <RTIM_GetINTStatus+0xb8>
    2d36:	f240 11b9 	movw	r1, #441	; 0x1b9
    2d3a:	4834      	ldr	r0, [pc, #208]	; (2e0c <RTIM_GetINTStatus+0x184>)
    2d3c:	f7fd f9ea 	bl	114 <io_assert_failed>
    2d40:	1e6b      	subs	r3, r5, #1
    2d42:	2b01      	cmp	r3, #1
    2d44:	d91e      	bls.n	2d84 <RTIM_GetINTStatus+0xfc>
    2d46:	2204      	movs	r2, #4
    2d48:	1f2b      	subs	r3, r5, #4
    2d4a:	4393      	bics	r3, r2
    2d4c:	b1d3      	cbz	r3, 2d84 <RTIM_GetINTStatus+0xfc>
    2d4e:	002b      	movs	r3, r5
    2d50:	320c      	adds	r2, #12
    2d52:	3b10      	subs	r3, #16
    2d54:	4393      	bics	r3, r2
    2d56:	b1ab      	cbz	r3, 2d84 <RTIM_GetINTStatus+0xfc>
    2d58:	2d40      	cmp	r5, #64	; 0x40
    2d5a:	d013      	beq.n	2d84 <RTIM_GetINTStatus+0xfc>
    2d5c:	4b2c      	ldr	r3, [pc, #176]	; (2e10 <RTIM_GetINTStatus+0x188>)
    2d5e:	4a2d      	ldr	r2, [pc, #180]	; (2e14 <RTIM_GetINTStatus+0x18c>)
    2d60:	18eb      	adds	r3, r5, r3
    2d62:	4213      	tst	r3, r2
    2d64:	d00e      	beq.n	2d84 <RTIM_GetINTStatus+0xfc>
    2d66:	4b2c      	ldr	r3, [pc, #176]	; (2e18 <RTIM_GetINTStatus+0x190>)
    2d68:	4a2c      	ldr	r2, [pc, #176]	; (2e1c <RTIM_GetINTStatus+0x194>)
    2d6a:	18eb      	adds	r3, r5, r3
    2d6c:	4213      	tst	r3, r2
    2d6e:	d009      	beq.n	2d84 <RTIM_GetINTStatus+0xfc>
    2d70:	4b2b      	ldr	r3, [pc, #172]	; (2e20 <RTIM_GetINTStatus+0x198>)
    2d72:	4a2c      	ldr	r2, [pc, #176]	; (2e24 <RTIM_GetINTStatus+0x19c>)
    2d74:	18eb      	adds	r3, r5, r3
    2d76:	4213      	tst	r3, r2
    2d78:	d004      	beq.n	2d84 <RTIM_GetINTStatus+0xfc>
    2d7a:	f240 11ba 	movw	r1, #442	; 0x1ba
    2d7e:	4823      	ldr	r0, [pc, #140]	; (2e0c <RTIM_GetINTStatus+0x184>)
    2d80:	f7fd f9c8 	bl	114 <io_assert_failed>
    2d84:	0028      	movs	r0, r5
    2d86:	68e3      	ldr	r3, [r4, #12]
    2d88:	68a1      	ldr	r1, [r4, #8]
    2d8a:	4018      	ands	r0, r3
    2d8c:	421d      	tst	r5, r3
    2d8e:	d003      	beq.n	2d98 <RTIM_GetINTStatus+0x110>
    2d90:	400d      	ands	r5, r1
    2d92:	0028      	movs	r0, r5
    2d94:	1e43      	subs	r3, r0, #1
    2d96:	4198      	sbcs	r0, r3
    2d98:	bd70      	pop	{r4, r5, r6, pc}
    2d9a:	46c0      	nop			; (mov r8, r8)
    2d9c:	4200b000 	.word	0x4200b000
    2da0:	4200b200 	.word	0x4200b200
    2da4:	4200b400 	.word	0x4200b400
    2da8:	4200b600 	.word	0x4200b600
    2dac:	4200b800 	.word	0x4200b800
    2db0:	4200ba00 	.word	0x4200ba00
    2db4:	4200bc00 	.word	0x4200bc00
    2db8:	4200be00 	.word	0x4200be00
    2dbc:	4100a000 	.word	0x4100a000
    2dc0:	4100a200 	.word	0x4100a200
    2dc4:	4100a400 	.word	0x4100a400
    2dc8:	4100a600 	.word	0x4100a600
    2dcc:	4100a800 	.word	0x4100a800
    2dd0:	4100aa00 	.word	0x4100aa00
    2dd4:	5200b000 	.word	0x5200b000
    2dd8:	5200b200 	.word	0x5200b200
    2ddc:	5200b400 	.word	0x5200b400
    2de0:	5200b600 	.word	0x5200b600
    2de4:	5200b800 	.word	0x5200b800
    2de8:	5200ba00 	.word	0x5200ba00
    2dec:	5200bc00 	.word	0x5200bc00
    2df0:	5200be00 	.word	0x5200be00
    2df4:	5100a000 	.word	0x5100a000
    2df8:	5100a200 	.word	0x5100a200
    2dfc:	5100a400 	.word	0x5100a400
    2e00:	5100a600 	.word	0x5100a600
    2e04:	5100a800 	.word	0x5100a800
    2e08:	5100aa00 	.word	0x5100aa00
    2e0c:	0000c496 	.word	0x0000c496
    2e10:	ffff0000 	.word	0xffff0000
    2e14:	fffeffff 	.word	0xfffeffff
    2e18:	fffc0000 	.word	0xfffc0000
    2e1c:	fffbffff 	.word	0xfffbffff
    2e20:	fff00000 	.word	0xfff00000
    2e24:	ffefffff 	.word	0xffefffff

00002e28 <UART_DeInit>:
    2e28:	2300      	movs	r3, #0
    2e2a:	6043      	str	r3, [r0, #4]
    2e2c:	4770      	bx	lr

00002e2e <UART_StructInit>:
    2e2e:	2300      	movs	r3, #0
    2e30:	2201      	movs	r2, #1
    2e32:	6103      	str	r3, [r0, #16]
    2e34:	6143      	str	r3, [r0, #20]
    2e36:	6083      	str	r3, [r0, #8]
    2e38:	61c3      	str	r3, [r0, #28]
    2e3a:	6183      	str	r3, [r0, #24]
    2e3c:	3340      	adds	r3, #64	; 0x40
    2e3e:	60c2      	str	r2, [r0, #12]
    2e40:	6042      	str	r2, [r0, #4]
    2e42:	6202      	str	r2, [r0, #32]
    2e44:	6002      	str	r2, [r0, #0]
    2e46:	6243      	str	r3, [r0, #36]	; 0x24
    2e48:	4770      	bx	lr

00002e4a <UART_BaudParaGetFull>:
    2e4a:	b5f0      	push	{r4, r5, r6, r7, lr}
    2e4c:	fbb0 f4f1 	udiv	r4, r0, r1
    2e50:	6014      	str	r4, [r2, #0]
    2e52:	434c      	muls	r4, r1
    2e54:	1b00      	subs	r0, r0, r4
    2e56:	240b      	movs	r4, #11
    2e58:	0026      	movs	r6, r4
    2e5a:	2500      	movs	r5, #0
    2e5c:	4346      	muls	r6, r0
    2e5e:	1982      	adds	r2, r0, r6
    2e60:	fbb6 f7f1 	udiv	r7, r6, r1
    2e64:	fbb2 f2f1 	udiv	r2, r2, r1
    2e68:	1bd2      	subs	r2, r2, r7
    2e6a:	1e57      	subs	r7, r2, #1
    2e6c:	41ba      	sbcs	r2, r7
    2e6e:	006d      	lsls	r5, r5, #1
    2e70:	3c01      	subs	r4, #1
    2e72:	4315      	orrs	r5, r2
    2e74:	1a36      	subs	r6, r6, r0
    2e76:	2c00      	cmp	r4, #0
    2e78:	d1f1      	bne.n	2e5e <UART_BaudParaGetFull+0x14>
    2e7a:	601d      	str	r5, [r3, #0]
    2e7c:	bdf0      	pop	{r4, r5, r6, r7, pc}

00002e7e <UART_BaudParaGet>:
    2e7e:	b570      	push	{r4, r5, r6, lr}
    2e80:	000d      	movs	r5, r1
    2e82:	0016      	movs	r6, r2
    2e84:	0004      	movs	r4, r0
    2e86:	4b04      	ldr	r3, [pc, #16]	; (2e98 <UART_BaudParaGet+0x1a>)
    2e88:	4798      	blx	r3
    2e8a:	0033      	movs	r3, r6
    2e8c:	002a      	movs	r2, r5
    2e8e:	0021      	movs	r1, r4
    2e90:	4c02      	ldr	r4, [pc, #8]	; (2e9c <UART_BaudParaGet+0x1e>)
    2e92:	47a0      	blx	r4
    2e94:	2001      	movs	r0, #1
    2e96:	bd70      	pop	{r4, r5, r6, pc}
    2e98:	000001c9 	.word	0x000001c9
    2e9c:	00002e4b 	.word	0x00002e4b

00002ea0 <UART_SetBaud>:
    2ea0:	b573      	push	{r0, r1, r4, r5, r6, lr}
    2ea2:	000d      	movs	r5, r1
    2ea4:	0004      	movs	r4, r0
    2ea6:	4b1a      	ldr	r3, [pc, #104]	; (2f10 <UART_SetBaud+0x70>)
    2ea8:	4798      	blx	r3
    2eaa:	466a      	mov	r2, sp
    2eac:	0029      	movs	r1, r5
    2eae:	ab01      	add	r3, sp, #4
    2eb0:	4d18      	ldr	r5, [pc, #96]	; (2f14 <UART_SetBaud+0x74>)
    2eb2:	47a8      	blx	r5
    2eb4:	2080      	movs	r0, #128	; 0x80
    2eb6:	68e3      	ldr	r3, [r4, #12]
    2eb8:	4917      	ldr	r1, [pc, #92]	; (2f18 <UART_SetBaud+0x78>)
    2eba:	4303      	orrs	r3, r0
    2ebc:	60e3      	str	r3, [r4, #12]
    2ebe:	69e3      	ldr	r3, [r4, #28]
    2ec0:	4d16      	ldr	r5, [pc, #88]	; (2f1c <UART_SetBaud+0x7c>)
    2ec2:	4303      	orrs	r3, r0
    2ec4:	61e3      	str	r3, [r4, #28]
    2ec6:	6a22      	ldr	r2, [r4, #32]
    2ec8:	9b00      	ldr	r3, [sp, #0]
    2eca:	4011      	ands	r1, r2
    2ecc:	011a      	lsls	r2, r3, #4
    2ece:	402a      	ands	r2, r5
    2ed0:	430a      	orrs	r2, r1
    2ed2:	6222      	str	r2, [r4, #32]
    2ed4:	69e2      	ldr	r2, [r4, #28]
    2ed6:	9901      	ldr	r1, [sp, #4]
    2ed8:	4d11      	ldr	r5, [pc, #68]	; (2f20 <UART_SetBaud+0x80>)
    2eda:	4e12      	ldr	r6, [pc, #72]	; (2f24 <UART_SetBaud+0x84>)
    2edc:	4015      	ands	r5, r2
    2ede:	040a      	lsls	r2, r1, #16
    2ee0:	4032      	ands	r2, r6
    2ee2:	432a      	orrs	r2, r5
    2ee4:	61e2      	str	r2, [r4, #28]
    2ee6:	68e2      	ldr	r2, [r4, #12]
    2ee8:	00c9      	lsls	r1, r1, #3
    2eea:	4382      	bics	r2, r0
    2eec:	60e2      	str	r2, [r4, #12]
    2eee:	6ca2      	ldr	r2, [r4, #72]	; 0x48
    2ef0:	480d      	ldr	r0, [pc, #52]	; (2f28 <UART_SetBaud+0x88>)
    2ef2:	0d12      	lsrs	r2, r2, #20
    2ef4:	0512      	lsls	r2, r2, #20
    2ef6:	431a      	orrs	r2, r3
    2ef8:	64a2      	str	r2, [r4, #72]	; 0x48
    2efa:	6c62      	ldr	r2, [r4, #68]	; 0x44
    2efc:	025b      	lsls	r3, r3, #9
    2efe:	4002      	ands	r2, r0
    2f00:	4313      	orrs	r3, r2
    2f02:	6463      	str	r3, [r4, #68]	; 0x44
    2f04:	6c23      	ldr	r3, [r4, #64]	; 0x40
    2f06:	4a09      	ldr	r2, [pc, #36]	; (2f2c <UART_SetBaud+0x8c>)
    2f08:	4013      	ands	r3, r2
    2f0a:	4319      	orrs	r1, r3
    2f0c:	6421      	str	r1, [r4, #64]	; 0x40
    2f0e:	bd73      	pop	{r0, r1, r4, r5, r6, pc}
    2f10:	000001c9 	.word	0x000001c9
    2f14:	00002e4b 	.word	0x00002e4b
    2f18:	ff00000f 	.word	0xff00000f
    2f1c:	00fffff0 	.word	0x00fffff0
    2f20:	f800ffff 	.word	0xf800ffff
    2f24:	07ff0000 	.word	0x07ff0000
    2f28:	e00001ff 	.word	0xe00001ff
    2f2c:	ffffc007 	.word	0xffffc007

00002f30 <UART_SetBaudExt>:
    2f30:	b570      	push	{r4, r5, r6, lr}
    2f32:	2480      	movs	r4, #128	; 0x80
    2f34:	68c3      	ldr	r3, [r0, #12]
    2f36:	4d15      	ldr	r5, [pc, #84]	; (2f8c <UART_SetBaudExt+0x5c>)
    2f38:	4323      	orrs	r3, r4
    2f3a:	60c3      	str	r3, [r0, #12]
    2f3c:	69c3      	ldr	r3, [r0, #28]
    2f3e:	4e14      	ldr	r6, [pc, #80]	; (2f90 <UART_SetBaudExt+0x60>)
    2f40:	4323      	orrs	r3, r4
    2f42:	61c3      	str	r3, [r0, #28]
    2f44:	6a03      	ldr	r3, [r0, #32]
    2f46:	401d      	ands	r5, r3
    2f48:	010b      	lsls	r3, r1, #4
    2f4a:	4033      	ands	r3, r6
    2f4c:	432b      	orrs	r3, r5
    2f4e:	6203      	str	r3, [r0, #32]
    2f50:	69c3      	ldr	r3, [r0, #28]
    2f52:	4d10      	ldr	r5, [pc, #64]	; (2f94 <UART_SetBaudExt+0x64>)
    2f54:	4e10      	ldr	r6, [pc, #64]	; (2f98 <UART_SetBaudExt+0x68>)
    2f56:	401d      	ands	r5, r3
    2f58:	0413      	lsls	r3, r2, #16
    2f5a:	4033      	ands	r3, r6
    2f5c:	432b      	orrs	r3, r5
    2f5e:	61c3      	str	r3, [r0, #28]
    2f60:	68c3      	ldr	r3, [r0, #12]
    2f62:	00d2      	lsls	r2, r2, #3
    2f64:	43a3      	bics	r3, r4
    2f66:	60c3      	str	r3, [r0, #12]
    2f68:	6c83      	ldr	r3, [r0, #72]	; 0x48
    2f6a:	4c0c      	ldr	r4, [pc, #48]	; (2f9c <UART_SetBaudExt+0x6c>)
    2f6c:	0d1b      	lsrs	r3, r3, #20
    2f6e:	051b      	lsls	r3, r3, #20
    2f70:	430b      	orrs	r3, r1
    2f72:	6483      	str	r3, [r0, #72]	; 0x48
    2f74:	6c43      	ldr	r3, [r0, #68]	; 0x44
    2f76:	0249      	lsls	r1, r1, #9
    2f78:	4023      	ands	r3, r4
    2f7a:	4319      	orrs	r1, r3
    2f7c:	6441      	str	r1, [r0, #68]	; 0x44
    2f7e:	6c03      	ldr	r3, [r0, #64]	; 0x40
    2f80:	4907      	ldr	r1, [pc, #28]	; (2fa0 <UART_SetBaudExt+0x70>)
    2f82:	400b      	ands	r3, r1
    2f84:	431a      	orrs	r2, r3
    2f86:	6402      	str	r2, [r0, #64]	; 0x40
    2f88:	bd70      	pop	{r4, r5, r6, pc}
    2f8a:	46c0      	nop			; (mov r8, r8)
    2f8c:	ff00000f 	.word	0xff00000f
    2f90:	00fffff0 	.word	0x00fffff0
    2f94:	f800ffff 	.word	0xf800ffff
    2f98:	07ff0000 	.word	0x07ff0000
    2f9c:	e00001ff 	.word	0xe00001ff
    2fa0:	ffffc007 	.word	0xffffc007

00002fa4 <UART_SetRxLevel>:
    2fa4:	23c0      	movs	r3, #192	; 0xc0
    2fa6:	000a      	movs	r2, r1
    2fa8:	439a      	bics	r2, r3
    2faa:	b570      	push	{r4, r5, r6, lr}
    2fac:	0005      	movs	r5, r0
    2fae:	000c      	movs	r4, r1
    2fb0:	b122      	cbz	r2, 2fbc <UART_SetRxLevel+0x18>
    2fb2:	f240 116a 	movw	r1, #362	; 0x16a
    2fb6:	4804      	ldr	r0, [pc, #16]	; (2fc8 <UART_SetRxLevel+0x24>)
    2fb8:	f7fd f8ac 	bl	114 <io_assert_failed>
    2fbc:	23c0      	movs	r3, #192	; 0xc0
    2fbe:	6d69      	ldr	r1, [r5, #84]	; 0x54
    2fc0:	4399      	bics	r1, r3
    2fc2:	430c      	orrs	r4, r1
    2fc4:	656c      	str	r4, [r5, #84]	; 0x54
    2fc6:	bd70      	pop	{r4, r5, r6, pc}
    2fc8:	0000c525 	.word	0x0000c525

00002fcc <UART_RxCmd>:
    2fcc:	2304      	movs	r3, #4
    2fce:	6c02      	ldr	r2, [r0, #64]	; 0x40
    2fd0:	b111      	cbz	r1, 2fd8 <UART_RxCmd+0xc>
    2fd2:	4313      	orrs	r3, r2
    2fd4:	6403      	str	r3, [r0, #64]	; 0x40
    2fd6:	4770      	bx	lr
    2fd8:	439a      	bics	r2, r3
    2fda:	6402      	str	r2, [r0, #64]	; 0x40
    2fdc:	e7fb      	b.n	2fd6 <UART_RxCmd+0xa>

00002fde <UART_Writable>:
    2fde:	6940      	ldr	r0, [r0, #20]
    2fe0:	0680      	lsls	r0, r0, #26
    2fe2:	0fc0      	lsrs	r0, r0, #31
    2fe4:	4770      	bx	lr

00002fe6 <UART_Readable>:
    2fe6:	2301      	movs	r3, #1
    2fe8:	6940      	ldr	r0, [r0, #20]
    2fea:	4018      	ands	r0, r3
    2fec:	4770      	bx	lr

00002fee <UART_CharPut>:
    2fee:	6241      	str	r1, [r0, #36]	; 0x24
    2ff0:	4770      	bx	lr

00002ff2 <UART_CharGet>:
    2ff2:	6a43      	ldr	r3, [r0, #36]	; 0x24
    2ff4:	700b      	strb	r3, [r1, #0]
    2ff6:	4770      	bx	lr

00002ff8 <UART_ReceiveData>:
    2ff8:	b570      	push	{r4, r5, r6, lr}
    2ffa:	0005      	movs	r5, r0
    2ffc:	000c      	movs	r4, r1
    2ffe:	188e      	adds	r6, r1, r2
    3000:	42b4      	cmp	r4, r6
    3002:	d100      	bne.n	3006 <UART_ReceiveData+0xe>
    3004:	bd70      	pop	{r4, r5, r6, pc}
    3006:	0028      	movs	r0, r5
    3008:	4b03      	ldr	r3, [pc, #12]	; (3018 <UART_ReceiveData+0x20>)
    300a:	4798      	blx	r3
    300c:	2800      	cmp	r0, #0
    300e:	d0fa      	beq.n	3006 <UART_ReceiveData+0xe>
    3010:	6a6b      	ldr	r3, [r5, #36]	; 0x24
    3012:	7023      	strb	r3, [r4, #0]
    3014:	3401      	adds	r4, #1
    3016:	e7f3      	b.n	3000 <UART_ReceiveData+0x8>
    3018:	00002fe7 	.word	0x00002fe7

0000301c <UART_SendData>:
    301c:	b570      	push	{r4, r5, r6, lr}
    301e:	0005      	movs	r5, r0
    3020:	000c      	movs	r4, r1
    3022:	188e      	adds	r6, r1, r2
    3024:	42b4      	cmp	r4, r6
    3026:	d100      	bne.n	302a <UART_SendData+0xe>
    3028:	bd70      	pop	{r4, r5, r6, pc}
    302a:	0028      	movs	r0, r5
    302c:	4b03      	ldr	r3, [pc, #12]	; (303c <UART_SendData+0x20>)
    302e:	4798      	blx	r3
    3030:	2800      	cmp	r0, #0
    3032:	d0fa      	beq.n	302a <UART_SendData+0xe>
    3034:	7823      	ldrb	r3, [r4, #0]
    3036:	3401      	adds	r4, #1
    3038:	626b      	str	r3, [r5, #36]	; 0x24
    303a:	e7f3      	b.n	3024 <UART_SendData+0x8>
    303c:	00002fdf 	.word	0x00002fdf

00003040 <UART_ReceiveDataTO>:
    3040:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    3042:	2500      	movs	r5, #0
    3044:	0006      	movs	r6, r0
    3046:	0017      	movs	r7, r2
    3048:	002c      	movs	r4, r5
    304a:	9100      	str	r1, [sp, #0]
    304c:	9301      	str	r3, [sp, #4]
    304e:	0030      	movs	r0, r6
    3050:	4b08      	ldr	r3, [pc, #32]	; (3074 <UART_ReceiveDataTO+0x34>)
    3052:	4798      	blx	r3
    3054:	b138      	cbz	r0, 3066 <UART_ReceiveDataTO+0x26>
    3056:	6a73      	ldr	r3, [r6, #36]	; 0x24
    3058:	9a00      	ldr	r2, [sp, #0]
    305a:	5513      	strb	r3, [r2, r4]
    305c:	3401      	adds	r4, #1
    305e:	42bc      	cmp	r4, r7
    3060:	d205      	bcs.n	306e <UART_ReceiveDataTO+0x2e>
    3062:	2500      	movs	r5, #0
    3064:	e7f3      	b.n	304e <UART_ReceiveDataTO+0xe>
    3066:	9b01      	ldr	r3, [sp, #4]
    3068:	3501      	adds	r5, #1
    306a:	429d      	cmp	r5, r3
    306c:	d9ef      	bls.n	304e <UART_ReceiveDataTO+0xe>
    306e:	0020      	movs	r0, r4
    3070:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    3072:	46c0      	nop			; (mov r8, r8)
    3074:	00002fe7 	.word	0x00002fe7

00003078 <UART_SendDataTO>:
    3078:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    307a:	2500      	movs	r5, #0
    307c:	0006      	movs	r6, r0
    307e:	0017      	movs	r7, r2
    3080:	002c      	movs	r4, r5
    3082:	9100      	str	r1, [sp, #0]
    3084:	9301      	str	r3, [sp, #4]
    3086:	0030      	movs	r0, r6
    3088:	4b08      	ldr	r3, [pc, #32]	; (30ac <UART_SendDataTO+0x34>)
    308a:	4798      	blx	r3
    308c:	b138      	cbz	r0, 309e <UART_SendDataTO+0x26>
    308e:	9b00      	ldr	r3, [sp, #0]
    3090:	5d1b      	ldrb	r3, [r3, r4]
    3092:	3401      	adds	r4, #1
    3094:	6273      	str	r3, [r6, #36]	; 0x24
    3096:	42bc      	cmp	r4, r7
    3098:	d205      	bcs.n	30a6 <UART_SendDataTO+0x2e>
    309a:	2500      	movs	r5, #0
    309c:	e7f3      	b.n	3086 <UART_SendDataTO+0xe>
    309e:	9b01      	ldr	r3, [sp, #4]
    30a0:	3501      	adds	r5, #1
    30a2:	429d      	cmp	r5, r3
    30a4:	d9ef      	bls.n	3086 <UART_SendDataTO+0xe>
    30a6:	0020      	movs	r0, r4
    30a8:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    30aa:	46c0      	nop			; (mov r8, r8)
    30ac:	00002fdf 	.word	0x00002fdf

000030b0 <UART_RxByteCntClear>:
    30b0:	2380      	movs	r3, #128	; 0x80
    30b2:	6d02      	ldr	r2, [r0, #80]	; 0x50
    30b4:	025b      	lsls	r3, r3, #9
    30b6:	4313      	orrs	r3, r2
    30b8:	6503      	str	r3, [r0, #80]	; 0x50
    30ba:	4770      	bx	lr

000030bc <UART_RxByteCntGet>:
    30bc:	6d00      	ldr	r0, [r0, #80]	; 0x50
    30be:	b280      	uxth	r0, r0
    30c0:	4770      	bx	lr

000030c2 <UART_BreakCtl>:
    30c2:	68c2      	ldr	r2, [r0, #12]
    30c4:	b510      	push	{r4, lr}
    30c6:	0013      	movs	r3, r2
    30c8:	2440      	movs	r4, #64	; 0x40
    30ca:	43a3      	bics	r3, r4
    30cc:	b109      	cbz	r1, 30d2 <UART_BreakCtl+0x10>
    30ce:	0023      	movs	r3, r4
    30d0:	4313      	orrs	r3, r2
    30d2:	60c3      	str	r3, [r0, #12]
    30d4:	bd10      	pop	{r4, pc}

000030d6 <UART_ClearRxFifo>:
    30d6:	2202      	movs	r2, #2
    30d8:	6d41      	ldr	r1, [r0, #84]	; 0x54
    30da:	0003      	movs	r3, r0
    30dc:	430a      	orrs	r2, r1
    30de:	6542      	str	r2, [r0, #84]	; 0x54
    30e0:	2005      	movs	r0, #5
    30e2:	2201      	movs	r2, #1
    30e4:	6959      	ldr	r1, [r3, #20]
    30e6:	4211      	tst	r1, r2
    30e8:	d003      	beq.n	30f2 <UART_ClearRxFifo+0x1c>
    30ea:	3801      	subs	r0, #1
    30ec:	2800      	cmp	r0, #0
    30ee:	d1f9      	bne.n	30e4 <UART_ClearRxFifo+0xe>
    30f0:	4770      	bx	lr
    30f2:	2001      	movs	r0, #1
    30f4:	e7fc      	b.n	30f0 <UART_ClearRxFifo+0x1a>

000030f6 <UART_Init>:
    30f6:	23c0      	movs	r3, #192	; 0xc0
    30f8:	69ca      	ldr	r2, [r1, #28]
    30fa:	b570      	push	{r4, r5, r6, lr}
    30fc:	439a      	bics	r2, r3
    30fe:	0004      	movs	r4, r0
    3100:	000d      	movs	r5, r1
    3102:	b11a      	cbz	r2, 310c <UART_Init+0x16>
    3104:	2183      	movs	r1, #131	; 0x83
    3106:	4824      	ldr	r0, [pc, #144]	; (3198 <UART_Init+0xa2>)
    3108:	f7fd f804 	bl	114 <io_assert_failed>
    310c:	6a2b      	ldr	r3, [r5, #32]
    310e:	2b01      	cmp	r3, #1
    3110:	d903      	bls.n	311a <UART_Init+0x24>
    3112:	2184      	movs	r1, #132	; 0x84
    3114:	4820      	ldr	r0, [pc, #128]	; (3198 <UART_Init+0xa2>)
    3116:	f7fc fffd 	bl	114 <io_assert_failed>
    311a:	2204      	movs	r2, #4
    311c:	6c23      	ldr	r3, [r4, #64]	; 0x40
    311e:	0020      	movs	r0, r4
    3120:	4393      	bics	r3, r2
    3122:	6423      	str	r3, [r4, #64]	; 0x40
    3124:	4b1d      	ldr	r3, [pc, #116]	; (319c <UART_Init+0xa6>)
    3126:	4798      	blx	r3
    3128:	2300      	movs	r3, #0
    312a:	6063      	str	r3, [r4, #4]
    312c:	69e2      	ldr	r2, [r4, #28]
    312e:	3380      	adds	r3, #128	; 0x80
    3130:	4313      	orrs	r3, r2
    3132:	22c1      	movs	r2, #193	; 0xc1
    3134:	61e3      	str	r3, [r4, #28]
    3136:	6d63      	ldr	r3, [r4, #84]	; 0x54
    3138:	4393      	bics	r3, r2
    313a:	69ea      	ldr	r2, [r5, #28]
    313c:	4313      	orrs	r3, r2
    313e:	6a2a      	ldr	r2, [r5, #32]
    3140:	4313      	orrs	r3, r2
    3142:	69aa      	ldr	r2, [r5, #24]
    3144:	6563      	str	r3, [r4, #84]	; 0x54
    3146:	2322      	movs	r3, #34	; 0x22
    3148:	2a01      	cmp	r2, #1
    314a:	d11d      	bne.n	3188 <UART_Init+0x92>
    314c:	6922      	ldr	r2, [r4, #16]
    314e:	4313      	orrs	r3, r2
    3150:	6123      	str	r3, [r4, #16]
    3152:	68ab      	ldr	r3, [r5, #8]
    3154:	68ea      	ldr	r2, [r5, #12]
    3156:	009b      	lsls	r3, r3, #2
    3158:	00d2      	lsls	r2, r2, #3
    315a:	4313      	orrs	r3, r2
    315c:	686a      	ldr	r2, [r5, #4]
    315e:	4313      	orrs	r3, r2
    3160:	692a      	ldr	r2, [r5, #16]
    3162:	0112      	lsls	r2, r2, #4
    3164:	4313      	orrs	r3, r2
    3166:	696a      	ldr	r2, [r5, #20]
    3168:	0152      	lsls	r2, r2, #5
    316a:	4313      	orrs	r3, r2
    316c:	682a      	ldr	r2, [r5, #0]
    316e:	60e3      	str	r3, [r4, #12]
    3170:	2308      	movs	r3, #8
    3172:	b16a      	cbz	r2, 3190 <UART_Init+0x9a>
    3174:	6d62      	ldr	r2, [r4, #84]	; 0x54
    3176:	4313      	orrs	r3, r2
    3178:	6563      	str	r3, [r4, #84]	; 0x54
    317a:	6a6b      	ldr	r3, [r5, #36]	; 0x24
    317c:	6c22      	ldr	r2, [r4, #64]	; 0x40
    317e:	041b      	lsls	r3, r3, #16
    3180:	b292      	uxth	r2, r2
    3182:	4313      	orrs	r3, r2
    3184:	6423      	str	r3, [r4, #64]	; 0x40
    3186:	bd70      	pop	{r4, r5, r6, pc}
    3188:	6922      	ldr	r2, [r4, #16]
    318a:	439a      	bics	r2, r3
    318c:	6122      	str	r2, [r4, #16]
    318e:	e7e0      	b.n	3152 <UART_Init+0x5c>
    3190:	6d62      	ldr	r2, [r4, #84]	; 0x54
    3192:	439a      	bics	r2, r3
    3194:	6562      	str	r2, [r4, #84]	; 0x54
    3196:	e7f0      	b.n	317a <UART_Init+0x84>
    3198:	0000c535 	.word	0x0000c535
    319c:	000030d7 	.word	0x000030d7

000031a0 <UART_ClearTxFifo>:
    31a0:	2304      	movs	r3, #4
    31a2:	6d42      	ldr	r2, [r0, #84]	; 0x54
    31a4:	4313      	orrs	r3, r2
    31a6:	6543      	str	r3, [r0, #84]	; 0x54
    31a8:	4770      	bx	lr

000031aa <UART_INTConfig>:
    31aa:	6843      	ldr	r3, [r0, #4]
    31ac:	2a01      	cmp	r2, #1
    31ae:	d102      	bne.n	31b6 <UART_INTConfig+0xc>
    31b0:	4319      	orrs	r1, r3
    31b2:	6041      	str	r1, [r0, #4]
    31b4:	4770      	bx	lr
    31b6:	438b      	bics	r3, r1
    31b8:	6043      	str	r3, [r0, #4]
    31ba:	e7fb      	b.n	31b4 <UART_INTConfig+0xa>

000031bc <UART_INT_Clear>:
    31bc:	6d83      	ldr	r3, [r0, #88]	; 0x58
    31be:	430b      	orrs	r3, r1
    31c0:	6583      	str	r3, [r0, #88]	; 0x58
    31c2:	4770      	bx	lr

000031c4 <UART_IntStatus>:
    31c4:	6880      	ldr	r0, [r0, #8]
    31c6:	4770      	bx	lr

000031c8 <UART_ModemStatusGet>:
    31c8:	6980      	ldr	r0, [r0, #24]
    31ca:	4770      	bx	lr

000031cc <UART_LineStatusGet>:
    31cc:	6940      	ldr	r0, [r0, #20]
    31ce:	4770      	bx	lr

000031d0 <UART_WaitBusy>:
    31d0:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    31d2:	0006      	movs	r6, r0
    31d4:	000c      	movs	r4, r1
    31d6:	2500      	movs	r5, #0
    31d8:	2720      	movs	r7, #32
    31da:	6973      	ldr	r3, [r6, #20]
    31dc:	423b      	tst	r3, r7
    31de:	d105      	bne.n	31ec <UART_WaitBusy+0x1c>
    31e0:	2064      	movs	r0, #100	; 0x64
    31e2:	4b03      	ldr	r3, [pc, #12]	; (31f0 <UART_WaitBusy+0x20>)
    31e4:	3501      	adds	r5, #1
    31e6:	4798      	blx	r3
    31e8:	42a5      	cmp	r5, r4
    31ea:	d9f6      	bls.n	31da <UART_WaitBusy+0xa>
    31ec:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    31ee:	46c0      	nop			; (mov r8, r8)
    31f0:	0000030b 	.word	0x0000030b

000031f4 <WDG_Wait_Busy>:
    31f4:	b570      	push	{r4, r5, r6, lr}
    31f6:	0004      	movs	r4, r0
    31f8:	f240 35e9 	movw	r5, #1001	; 0x3e9
    31fc:	6863      	ldr	r3, [r4, #4]
    31fe:	0f9b      	lsrs	r3, r3, #30
    3200:	b12b      	cbz	r3, 320e <WDG_Wait_Busy+0x1a>
    3202:	2001      	movs	r0, #1
    3204:	4b02      	ldr	r3, [pc, #8]	; (3210 <WDG_Wait_Busy+0x1c>)
    3206:	3d01      	subs	r5, #1
    3208:	4798      	blx	r3
    320a:	2d00      	cmp	r5, #0
    320c:	d1f6      	bne.n	31fc <WDG_Wait_Busy+0x8>
    320e:	bd70      	pop	{r4, r5, r6, pc}
    3210:	0000030b 	.word	0x0000030b

00003214 <WDG_StructInit>:
    3214:	2301      	movs	r3, #1
    3216:	425b      	negs	r3, r3
    3218:	8003      	strh	r3, [r0, #0]
    321a:	8043      	strh	r3, [r0, #2]
    321c:	2300      	movs	r3, #0
    321e:	8083      	strh	r3, [r0, #4]
    3220:	80c3      	strh	r3, [r0, #6]
    3222:	4770      	bx	lr

00003224 <WDG_Init>:
    3224:	4b17      	ldr	r3, [pc, #92]	; (3284 <WDG_Init+0x60>)
    3226:	b570      	push	{r4, r5, r6, lr}
    3228:	0004      	movs	r4, r0
    322a:	000e      	movs	r6, r1
    322c:	2563      	movs	r5, #99	; 0x63
    322e:	4298      	cmp	r0, r3
    3230:	d010      	beq.n	3254 <WDG_Init+0x30>
    3232:	4b15      	ldr	r3, [pc, #84]	; (3288 <WDG_Init+0x64>)
    3234:	3d44      	subs	r5, #68	; 0x44
    3236:	4298      	cmp	r0, r3
    3238:	d00c      	beq.n	3254 <WDG_Init+0x30>
    323a:	4b14      	ldr	r3, [pc, #80]	; (328c <WDG_Init+0x68>)
    323c:	4298      	cmp	r0, r3
    323e:	d009      	beq.n	3254 <WDG_Init+0x30>
    3240:	4b13      	ldr	r3, [pc, #76]	; (3290 <WDG_Init+0x6c>)
    3242:	4298      	cmp	r0, r3
    3244:	d006      	beq.n	3254 <WDG_Init+0x30>
    3246:	4b13      	ldr	r3, [pc, #76]	; (3294 <WDG_Init+0x70>)
    3248:	4298      	cmp	r0, r3
    324a:	d003      	beq.n	3254 <WDG_Init+0x30>
    324c:	2148      	movs	r1, #72	; 0x48
    324e:	4812      	ldr	r0, [pc, #72]	; (3298 <WDG_Init+0x74>)
    3250:	f7fc ff60 	bl	114 <io_assert_failed>
    3254:	0020      	movs	r0, r4
    3256:	f7ff ffcd 	bl	31f4 <WDG_Wait_Busy>
    325a:	f646 1369 	movw	r3, #26985	; 0x6969
    325e:	6023      	str	r3, [r4, #0]
    3260:	88f3      	ldrh	r3, [r6, #6]
    3262:	b11b      	cbz	r3, 326c <WDG_Init+0x48>
    3264:	2380      	movs	r3, #128	; 0x80
    3266:	88b2      	ldrh	r2, [r6, #4]
    3268:	025b      	lsls	r3, r3, #9
    326a:	4313      	orrs	r3, r2
    326c:	6063      	str	r3, [r4, #4]
    326e:	8873      	ldrh	r3, [r6, #2]
    3270:	042d      	lsls	r5, r5, #16
    3272:	431d      	orrs	r5, r3
    3274:	8833      	ldrh	r3, [r6, #0]
    3276:	60a5      	str	r5, [r4, #8]
    3278:	60e3      	str	r3, [r4, #12]
    327a:	f64f 73ff 	movw	r3, #65535	; 0xffff
    327e:	6023      	str	r3, [r4, #0]
    3280:	bd70      	pop	{r4, r5, r6, pc}
    3282:	46c0      	nop			; (mov r8, r8)
    3284:	42008400 	.word	0x42008400
    3288:	51000400 	.word	0x51000400
    328c:	41000440 	.word	0x41000440
    3290:	51000480 	.word	0x51000480
    3294:	410004c0 	.word	0x410004c0
    3298:	0000c58c 	.word	0x0000c58c

0000329c <WDG_Enable>:
    329c:	4b0d      	ldr	r3, [pc, #52]	; (32d4 <WDG_Enable+0x38>)
    329e:	b510      	push	{r4, lr}
    32a0:	0004      	movs	r4, r0
    32a2:	4298      	cmp	r0, r3
    32a4:	d00f      	beq.n	32c6 <WDG_Enable+0x2a>
    32a6:	4b0c      	ldr	r3, [pc, #48]	; (32d8 <WDG_Enable+0x3c>)
    32a8:	4298      	cmp	r0, r3
    32aa:	d00c      	beq.n	32c6 <WDG_Enable+0x2a>
    32ac:	4b0b      	ldr	r3, [pc, #44]	; (32dc <WDG_Enable+0x40>)
    32ae:	4298      	cmp	r0, r3
    32b0:	d009      	beq.n	32c6 <WDG_Enable+0x2a>
    32b2:	4b0b      	ldr	r3, [pc, #44]	; (32e0 <WDG_Enable+0x44>)
    32b4:	4298      	cmp	r0, r3
    32b6:	d006      	beq.n	32c6 <WDG_Enable+0x2a>
    32b8:	4b0a      	ldr	r3, [pc, #40]	; (32e4 <WDG_Enable+0x48>)
    32ba:	4298      	cmp	r0, r3
    32bc:	d003      	beq.n	32c6 <WDG_Enable+0x2a>
    32be:	216c      	movs	r1, #108	; 0x6c
    32c0:	4809      	ldr	r0, [pc, #36]	; (32e8 <WDG_Enable+0x4c>)
    32c2:	f7fc ff27 	bl	114 <io_assert_failed>
    32c6:	0020      	movs	r0, r4
    32c8:	f7ff ff94 	bl	31f4 <WDG_Wait_Busy>
    32cc:	f643 433c 	movw	r3, #15420	; 0x3c3c
    32d0:	6023      	str	r3, [r4, #0]
    32d2:	bd10      	pop	{r4, pc}
    32d4:	42008400 	.word	0x42008400
    32d8:	51000400 	.word	0x51000400
    32dc:	41000440 	.word	0x41000440
    32e0:	51000480 	.word	0x51000480
    32e4:	410004c0 	.word	0x410004c0
    32e8:	0000c581 	.word	0x0000c581

000032ec <WDG_Timeout>:
    32ec:	4b12      	ldr	r3, [pc, #72]	; (3338 <WDG_Timeout+0x4c>)
    32ee:	b570      	push	{r4, r5, r6, lr}
    32f0:	0004      	movs	r4, r0
    32f2:	000e      	movs	r6, r1
    32f4:	2563      	movs	r5, #99	; 0x63
    32f6:	4298      	cmp	r0, r3
    32f8:	d010      	beq.n	331c <WDG_Timeout+0x30>
    32fa:	4b10      	ldr	r3, [pc, #64]	; (333c <WDG_Timeout+0x50>)
    32fc:	3d44      	subs	r5, #68	; 0x44
    32fe:	4298      	cmp	r0, r3
    3300:	d00c      	beq.n	331c <WDG_Timeout+0x30>
    3302:	4b0f      	ldr	r3, [pc, #60]	; (3340 <WDG_Timeout+0x54>)
    3304:	4298      	cmp	r0, r3
    3306:	d009      	beq.n	331c <WDG_Timeout+0x30>
    3308:	4b0e      	ldr	r3, [pc, #56]	; (3344 <WDG_Timeout+0x58>)
    330a:	4298      	cmp	r0, r3
    330c:	d006      	beq.n	331c <WDG_Timeout+0x30>
    330e:	4b0e      	ldr	r3, [pc, #56]	; (3348 <WDG_Timeout+0x5c>)
    3310:	4298      	cmp	r0, r3
    3312:	d003      	beq.n	331c <WDG_Timeout+0x30>
    3314:	2180      	movs	r1, #128	; 0x80
    3316:	480d      	ldr	r0, [pc, #52]	; (334c <WDG_Timeout+0x60>)
    3318:	f7fc fefc 	bl	114 <io_assert_failed>
    331c:	0020      	movs	r0, r4
    331e:	f7ff ff69 	bl	31f4 <WDG_Wait_Busy>
    3322:	f646 1369 	movw	r3, #26985	; 0x6969
    3326:	6023      	str	r3, [r4, #0]
    3328:	f64f 73ff 	movw	r3, #65535	; 0xffff
    332c:	b2b6      	uxth	r6, r6
    332e:	042d      	lsls	r5, r5, #16
    3330:	4335      	orrs	r5, r6
    3332:	60a5      	str	r5, [r4, #8]
    3334:	6023      	str	r3, [r4, #0]
    3336:	bd70      	pop	{r4, r5, r6, pc}
    3338:	42008400 	.word	0x42008400
    333c:	51000400 	.word	0x51000400
    3340:	41000440 	.word	0x41000440
    3344:	51000480 	.word	0x51000480
    3348:	410004c0 	.word	0x410004c0
    334c:	0000c575 	.word	0x0000c575

00003350 <WDG_Refresh>:
    3350:	4b0d      	ldr	r3, [pc, #52]	; (3388 <WDG_Refresh+0x38>)
    3352:	b510      	push	{r4, lr}
    3354:	0004      	movs	r4, r0
    3356:	4298      	cmp	r0, r3
    3358:	d00f      	beq.n	337a <WDG_Refresh+0x2a>
    335a:	4b0c      	ldr	r3, [pc, #48]	; (338c <WDG_Refresh+0x3c>)
    335c:	4298      	cmp	r0, r3
    335e:	d00c      	beq.n	337a <WDG_Refresh+0x2a>
    3360:	4b0b      	ldr	r3, [pc, #44]	; (3390 <WDG_Refresh+0x40>)
    3362:	4298      	cmp	r0, r3
    3364:	d009      	beq.n	337a <WDG_Refresh+0x2a>
    3366:	4b0b      	ldr	r3, [pc, #44]	; (3394 <WDG_Refresh+0x44>)
    3368:	4298      	cmp	r0, r3
    336a:	d006      	beq.n	337a <WDG_Refresh+0x2a>
    336c:	4b0a      	ldr	r3, [pc, #40]	; (3398 <WDG_Refresh+0x48>)
    336e:	4298      	cmp	r0, r3
    3370:	d003      	beq.n	337a <WDG_Refresh+0x2a>
    3372:	219f      	movs	r1, #159	; 0x9f
    3374:	4809      	ldr	r0, [pc, #36]	; (339c <WDG_Refresh+0x4c>)
    3376:	f7fc fecd 	bl	114 <io_assert_failed>
    337a:	0020      	movs	r0, r4
    337c:	f7ff ff3a 	bl	31f4 <WDG_Wait_Busy>
    3380:	f645 235a 	movw	r3, #23130	; 0x5a5a
    3384:	6023      	str	r3, [r4, #0]
    3386:	bd10      	pop	{r4, pc}
    3388:	42008400 	.word	0x42008400
    338c:	51000400 	.word	0x51000400
    3390:	41000440 	.word	0x41000440
    3394:	51000480 	.word	0x51000480
    3398:	410004c0 	.word	0x410004c0
    339c:	0000c569 	.word	0x0000c569

000033a0 <WDG_INTConfig>:
    33a0:	4b18      	ldr	r3, [pc, #96]	; (3404 <WDG_INTConfig+0x64>)
    33a2:	b570      	push	{r4, r5, r6, lr}
    33a4:	0004      	movs	r4, r0
    33a6:	000d      	movs	r5, r1
    33a8:	0016      	movs	r6, r2
    33aa:	4298      	cmp	r0, r3
    33ac:	d00f      	beq.n	33ce <WDG_INTConfig+0x2e>
    33ae:	4b16      	ldr	r3, [pc, #88]	; (3408 <WDG_INTConfig+0x68>)
    33b0:	4298      	cmp	r0, r3
    33b2:	d00c      	beq.n	33ce <WDG_INTConfig+0x2e>
    33b4:	4b15      	ldr	r3, [pc, #84]	; (340c <WDG_INTConfig+0x6c>)
    33b6:	4298      	cmp	r0, r3
    33b8:	d009      	beq.n	33ce <WDG_INTConfig+0x2e>
    33ba:	4b15      	ldr	r3, [pc, #84]	; (3410 <WDG_INTConfig+0x70>)
    33bc:	4298      	cmp	r0, r3
    33be:	d006      	beq.n	33ce <WDG_INTConfig+0x2e>
    33c0:	4b14      	ldr	r3, [pc, #80]	; (3414 <WDG_INTConfig+0x74>)
    33c2:	4298      	cmp	r0, r3
    33c4:	d003      	beq.n	33ce <WDG_INTConfig+0x2e>
    33c6:	21b1      	movs	r1, #177	; 0xb1
    33c8:	4813      	ldr	r0, [pc, #76]	; (3418 <WDG_INTConfig+0x78>)
    33ca:	f7fc fea3 	bl	114 <io_assert_failed>
    33ce:	2380      	movs	r3, #128	; 0x80
    33d0:	025b      	lsls	r3, r3, #9
    33d2:	429d      	cmp	r5, r3
    33d4:	d003      	beq.n	33de <WDG_INTConfig+0x3e>
    33d6:	21b2      	movs	r1, #178	; 0xb2
    33d8:	480f      	ldr	r0, [pc, #60]	; (3418 <WDG_INTConfig+0x78>)
    33da:	f7fc fe9b 	bl	114 <io_assert_failed>
    33de:	0020      	movs	r0, r4
    33e0:	f7ff ff08 	bl	31f4 <WDG_Wait_Busy>
    33e4:	f646 1369 	movw	r3, #26985	; 0x6969
    33e8:	6023      	str	r3, [r4, #0]
    33ea:	2e01      	cmp	r6, #1
    33ec:	d106      	bne.n	33fc <WDG_INTConfig+0x5c>
    33ee:	6861      	ldr	r1, [r4, #4]
    33f0:	430d      	orrs	r5, r1
    33f2:	6065      	str	r5, [r4, #4]
    33f4:	f64f 73ff 	movw	r3, #65535	; 0xffff
    33f8:	6023      	str	r3, [r4, #0]
    33fa:	bd70      	pop	{r4, r5, r6, pc}
    33fc:	6863      	ldr	r3, [r4, #4]
    33fe:	43ab      	bics	r3, r5
    3400:	6063      	str	r3, [r4, #4]
    3402:	e7f7      	b.n	33f4 <WDG_INTConfig+0x54>
    3404:	42008400 	.word	0x42008400
    3408:	51000400 	.word	0x51000400
    340c:	41000440 	.word	0x41000440
    3410:	51000480 	.word	0x51000480
    3414:	410004c0 	.word	0x410004c0
    3418:	0000c55b 	.word	0x0000c55b

0000341c <WDG_ClearINT>:
    341c:	4b15      	ldr	r3, [pc, #84]	; (3474 <WDG_ClearINT+0x58>)
    341e:	b570      	push	{r4, r5, r6, lr}
    3420:	0004      	movs	r4, r0
    3422:	000d      	movs	r5, r1
    3424:	4298      	cmp	r0, r3
    3426:	d00f      	beq.n	3448 <WDG_ClearINT+0x2c>
    3428:	4b13      	ldr	r3, [pc, #76]	; (3478 <WDG_ClearINT+0x5c>)
    342a:	4298      	cmp	r0, r3
    342c:	d00c      	beq.n	3448 <WDG_ClearINT+0x2c>
    342e:	4b13      	ldr	r3, [pc, #76]	; (347c <WDG_ClearINT+0x60>)
    3430:	4298      	cmp	r0, r3
    3432:	d009      	beq.n	3448 <WDG_ClearINT+0x2c>
    3434:	4b12      	ldr	r3, [pc, #72]	; (3480 <WDG_ClearINT+0x64>)
    3436:	4298      	cmp	r0, r3
    3438:	d006      	beq.n	3448 <WDG_ClearINT+0x2c>
    343a:	4b12      	ldr	r3, [pc, #72]	; (3484 <WDG_ClearINT+0x68>)
    343c:	4298      	cmp	r0, r3
    343e:	d003      	beq.n	3448 <WDG_ClearINT+0x2c>
    3440:	21cc      	movs	r1, #204	; 0xcc
    3442:	4811      	ldr	r0, [pc, #68]	; (3488 <WDG_ClearINT+0x6c>)
    3444:	f7fc fe66 	bl	114 <io_assert_failed>
    3448:	2380      	movs	r3, #128	; 0x80
    344a:	029b      	lsls	r3, r3, #10
    344c:	429d      	cmp	r5, r3
    344e:	d003      	beq.n	3458 <WDG_ClearINT+0x3c>
    3450:	21cd      	movs	r1, #205	; 0xcd
    3452:	480d      	ldr	r0, [pc, #52]	; (3488 <WDG_ClearINT+0x6c>)
    3454:	f7fc fe5e 	bl	114 <io_assert_failed>
    3458:	0020      	movs	r0, r4
    345a:	f7ff fecb 	bl	31f4 <WDG_Wait_Busy>
    345e:	f646 1369 	movw	r3, #26985	; 0x6969
    3462:	6023      	str	r3, [r4, #0]
    3464:	f64f 73ff 	movw	r3, #65535	; 0xffff
    3468:	6861      	ldr	r1, [r4, #4]
    346a:	430d      	orrs	r5, r1
    346c:	6065      	str	r5, [r4, #4]
    346e:	6023      	str	r3, [r4, #0]
    3470:	bd70      	pop	{r4, r5, r6, pc}
    3472:	46c0      	nop			; (mov r8, r8)
    3474:	42008400 	.word	0x42008400
    3478:	51000400 	.word	0x51000400
    347c:	41000440 	.word	0x41000440
    3480:	51000480 	.word	0x51000480
    3484:	410004c0 	.word	0x410004c0
    3488:	0000c54e 	.word	0x0000c54e

0000348c <IWDG_LP_Enable>:
    348c:	4b0f      	ldr	r3, [pc, #60]	; (34cc <IWDG_LP_Enable+0x40>)
    348e:	b570      	push	{r4, r5, r6, lr}
    3490:	0004      	movs	r4, r0
    3492:	000d      	movs	r5, r1
    3494:	4298      	cmp	r0, r3
    3496:	d003      	beq.n	34a0 <IWDG_LP_Enable+0x14>
    3498:	21e5      	movs	r1, #229	; 0xe5
    349a:	480d      	ldr	r0, [pc, #52]	; (34d0 <IWDG_LP_Enable+0x44>)
    349c:	f7fc fe3a 	bl	114 <io_assert_failed>
    34a0:	0020      	movs	r0, r4
    34a2:	f7ff fea7 	bl	31f4 <WDG_Wait_Busy>
    34a6:	f646 1369 	movw	r3, #26985	; 0x6969
    34aa:	6023      	str	r3, [r4, #0]
    34ac:	2d01      	cmp	r5, #1
    34ae:	d108      	bne.n	34c2 <IWDG_LP_Enable+0x36>
    34b0:	2380      	movs	r3, #128	; 0x80
    34b2:	6862      	ldr	r2, [r4, #4]
    34b4:	045b      	lsls	r3, r3, #17
    34b6:	4313      	orrs	r3, r2
    34b8:	6063      	str	r3, [r4, #4]
    34ba:	f64f 73ff 	movw	r3, #65535	; 0xffff
    34be:	6023      	str	r3, [r4, #0]
    34c0:	bd70      	pop	{r4, r5, r6, pc}
    34c2:	6863      	ldr	r3, [r4, #4]
    34c4:	4a03      	ldr	r2, [pc, #12]	; (34d4 <IWDG_LP_Enable+0x48>)
    34c6:	4013      	ands	r3, r2
    34c8:	e7f6      	b.n	34b8 <IWDG_LP_Enable+0x2c>
    34ca:	46c0      	nop			; (mov r8, r8)
    34cc:	42008400 	.word	0x42008400
    34d0:	0000c53f 	.word	0x0000c53f
    34d4:	feffffff 	.word	0xfeffffff

000034d8 <BOOT_ROM_InitDebugFlg>:
    34d8:	4b04      	ldr	r3, [pc, #16]	; (34ec <BOOT_ROM_InitDebugFlg+0x14>)
    34da:	681b      	ldr	r3, [r3, #0]
    34dc:	01db      	lsls	r3, r3, #7
    34de:	d403      	bmi.n	34e8 <BOOT_ROM_InitDebugFlg+0x10>
    34e0:	2201      	movs	r2, #1
    34e2:	4b03      	ldr	r3, [pc, #12]	; (34f0 <BOOT_ROM_InitDebugFlg+0x18>)
    34e4:	4252      	negs	r2, r2
    34e6:	601a      	str	r2, [r3, #0]
    34e8:	4770      	bx	lr
    34ea:	46c0      	nop			; (mov r8, r8)
    34ec:	42000100 	.word	0x42000100
    34f0:	230005d8 	.word	0x230005d8

000034f4 <BOOT_ROM_ResetVsr>:
    34f4:	2340      	movs	r3, #64	; 0x40
    34f6:	4a19      	ldr	r2, [pc, #100]	; (355c <BOOT_ROM_ResetVsr+0x68>)
    34f8:	b510      	push	{r4, lr}
    34fa:	6891      	ldr	r1, [r2, #8]
    34fc:	430b      	orrs	r3, r1
    34fe:	6093      	str	r3, [r2, #8]
    3500:	f3bf 8f4f 	dsb	sy
    3504:	4c16      	ldr	r4, [pc, #88]	; (3560 <BOOT_ROM_ResetVsr+0x6c>)
    3506:	6823      	ldr	r3, [r4, #0]
    3508:	01db      	lsls	r3, r3, #7
    350a:	d50b      	bpl.n	3524 <BOOT_ROM_ResetVsr+0x30>
    350c:	4815      	ldr	r0, [pc, #84]	; (3564 <BOOT_ROM_ResetVsr+0x70>)
    350e:	7941      	ldrb	r1, [r0, #5]
    3510:	7903      	ldrb	r3, [r0, #4]
    3512:	7982      	ldrb	r2, [r0, #6]
    3514:	0209      	lsls	r1, r1, #8
    3516:	4319      	orrs	r1, r3
    3518:	79c3      	ldrb	r3, [r0, #7]
    351a:	0412      	lsls	r2, r2, #16
    351c:	4311      	orrs	r1, r2
    351e:	061b      	lsls	r3, r3, #24
    3520:	430b      	orrs	r3, r1
    3522:	4798      	blx	r3
    3524:	2101      	movs	r1, #1
    3526:	4810      	ldr	r0, [pc, #64]	; (3568 <BOOT_ROM_ResetVsr+0x74>)
    3528:	4b10      	ldr	r3, [pc, #64]	; (356c <BOOT_ROM_ResetVsr+0x78>)
    352a:	4798      	blx	r3
    352c:	bf20      	wfe
    352e:	bf20      	wfe
    3530:	2280      	movs	r2, #128	; 0x80
    3532:	0492      	lsls	r2, r2, #18
    3534:	6823      	ldr	r3, [r4, #0]
    3536:	4213      	tst	r3, r2
    3538:	d0fc      	beq.n	3534 <BOOT_ROM_ResetVsr+0x40>
    353a:	4a0d      	ldr	r2, [pc, #52]	; (3570 <BOOT_ROM_ResetVsr+0x7c>)
    353c:	4809      	ldr	r0, [pc, #36]	; (3564 <BOOT_ROM_ResetVsr+0x70>)
    353e:	4013      	ands	r3, r2
    3540:	6023      	str	r3, [r4, #0]
    3542:	7d41      	ldrb	r1, [r0, #21]
    3544:	7d03      	ldrb	r3, [r0, #20]
    3546:	7d82      	ldrb	r2, [r0, #22]
    3548:	0209      	lsls	r1, r1, #8
    354a:	4319      	orrs	r1, r3
    354c:	7dc3      	ldrb	r3, [r0, #23]
    354e:	0412      	lsls	r2, r2, #16
    3550:	4311      	orrs	r1, r2
    3552:	061b      	lsls	r3, r3, #24
    3554:	430b      	orrs	r3, r1
    3556:	4798      	blx	r3
    3558:	bd10      	pop	{r4, pc}
    355a:	46c0      	nop			; (mov r8, r8)
    355c:	e000e000 	.word	0xe000e000
    3560:	42008268 	.word	0x42008268
    3564:	23002020 	.word	0x23002020
    3568:	42008400 	.word	0x42008400
    356c:	0000348d 	.word	0x0000348d
    3570:	fdffffff 	.word	0xfdffffff

00003574 <INT_Entry>:
    3574:	0001      	movs	r1, r0
    3576:	0943      	lsrs	r3, r0, #5
    3578:	201f      	movs	r0, #31
    357a:	2201      	movs	r2, #1
    357c:	4008      	ands	r0, r1
    357e:	4082      	lsls	r2, r0
    3580:	3360      	adds	r3, #96	; 0x60
    3582:	480a      	ldr	r0, [pc, #40]	; (35ac <INT_Entry+0x38>)
    3584:	009b      	lsls	r3, r3, #2
    3586:	b510      	push	{r4, lr}
    3588:	501a      	str	r2, [r3, r0]
    358a:	4b09      	ldr	r3, [pc, #36]	; (35b0 <INT_Entry+0x3c>)
    358c:	008a      	lsls	r2, r1, #2
    358e:	58d3      	ldr	r3, [r2, r3]
    3590:	b11b      	cbz	r3, 359a <INT_Entry+0x26>
    3592:	4908      	ldr	r1, [pc, #32]	; (35b4 <INT_Entry+0x40>)
    3594:	5850      	ldr	r0, [r2, r1]
    3596:	4798      	blx	r3
    3598:	bd10      	pop	{r4, pc}
    359a:	4b07      	ldr	r3, [pc, #28]	; (35b8 <INT_Entry+0x44>)
    359c:	681b      	ldr	r3, [r3, #0]
    359e:	039b      	lsls	r3, r3, #14
    35a0:	d5fa      	bpl.n	3598 <INT_Entry+0x24>
    35a2:	4806      	ldr	r0, [pc, #24]	; (35bc <INT_Entry+0x48>)
    35a4:	f000 fce8 	bl	3f78 <DiagPrintf>
    35a8:	e7f6      	b.n	3598 <INT_Entry+0x24>
    35aa:	46c0      	nop			; (mov r8, r8)
    35ac:	e000e100 	.word	0xe000e100
    35b0:	230000f0 	.word	0x230000f0
    35b4:	230001a0 	.word	0x230001a0
    35b8:	230005d8 	.word	0x230005d8
    35bc:	0000c105 	.word	0x0000c105

000035c0 <INT_TIMER13>:
    35c0:	b510      	push	{r4, lr}
    35c2:	2029      	movs	r0, #41	; 0x29
    35c4:	f7ff ffd6 	bl	3574 <INT_Entry>
    35c8:	bd10      	pop	{r4, pc}

000035ca <INT_TIMER12>:
    35ca:	b510      	push	{r4, lr}
    35cc:	2028      	movs	r0, #40	; 0x28
    35ce:	f7ff ffd1 	bl	3574 <INT_Entry>
    35d2:	bd10      	pop	{r4, pc}

000035d4 <INT_TIMER11>:
    35d4:	b510      	push	{r4, lr}
    35d6:	2027      	movs	r0, #39	; 0x27
    35d8:	f7ff ffcc 	bl	3574 <INT_Entry>
    35dc:	bd10      	pop	{r4, pc}

000035de <INT_TIMER10>:
    35de:	b510      	push	{r4, lr}
    35e0:	2026      	movs	r0, #38	; 0x26
    35e2:	f7ff ffc7 	bl	3574 <INT_Entry>
    35e6:	bd10      	pop	{r4, pc}

000035e8 <INT_WL_PROTOCOL>:
    35e8:	b510      	push	{r4, lr}
    35ea:	2025      	movs	r0, #37	; 0x25
    35ec:	f7ff ffc2 	bl	3574 <INT_Entry>
    35f0:	bd10      	pop	{r4, pc}

000035f2 <INT_WL_DMA>:
    35f2:	b510      	push	{r4, lr}
    35f4:	2024      	movs	r0, #36	; 0x24
    35f6:	f7ff ffbd 	bl	3574 <INT_Entry>
    35fa:	bd10      	pop	{r4, pc}

000035fc <INT_NFIQOUT1_OR_NIRQOUT1>:
    35fc:	b510      	push	{r4, lr}
    35fe:	2023      	movs	r0, #35	; 0x23
    3600:	f7ff ffb8 	bl	3574 <INT_Entry>
    3604:	bd10      	pop	{r4, pc}

00003606 <INT_NFIQOUT0_OR_NIRQOUT0>:
    3606:	b510      	push	{r4, lr}
    3608:	2022      	movs	r0, #34	; 0x22
    360a:	f7ff ffb3 	bl	3574 <INT_Entry>
    360e:	bd10      	pop	{r4, pc}

00003610 <INT_BT_WAKE_HOST>:
    3610:	b510      	push	{r4, lr}
    3612:	2021      	movs	r0, #33	; 0x21
    3614:	f7ff ffae 	bl	3574 <INT_Entry>
    3618:	bd10      	pop	{r4, pc}

0000361a <INT_AON_WAKEPIN>:
    361a:	b510      	push	{r4, lr}
    361c:	2020      	movs	r0, #32
    361e:	f7ff ffa9 	bl	3574 <INT_Entry>
    3622:	bd10      	pop	{r4, pc}

00003624 <INT_RSIP>:
    3624:	b510      	push	{r4, lr}
    3626:	201f      	movs	r0, #31
    3628:	f7ff ffa4 	bl	3574 <INT_Entry>
    362c:	bd10      	pop	{r4, pc}

0000362e <INT_I2C0>:
    362e:	b510      	push	{r4, lr}
    3630:	201e      	movs	r0, #30
    3632:	f7ff ff9f 	bl	3574 <INT_Entry>
    3636:	bd10      	pop	{r4, pc}

00003638 <INT_THERMAL>:
    3638:	b510      	push	{r4, lr}
    363a:	201d      	movs	r0, #29
    363c:	f7ff ff9a 	bl	3574 <INT_Entry>
    3640:	bd10      	pop	{r4, pc}

00003642 <INT_DEBUG_TIMER>:
    3642:	b510      	push	{r4, lr}
    3644:	201c      	movs	r0, #28
    3646:	f7ff ff95 	bl	3574 <INT_Entry>
    364a:	bd10      	pop	{r4, pc}

0000364c <INT_SPI_FLASH>:
    364c:	b510      	push	{r4, lr}
    364e:	201b      	movs	r0, #27
    3650:	f7ff ff90 	bl	3574 <INT_Entry>
    3654:	bd10      	pop	{r4, pc}

00003656 <INT_VADBT_OR_VADPC>:
    3656:	b510      	push	{r4, lr}
    3658:	201a      	movs	r0, #26
    365a:	f7ff ff8b 	bl	3574 <INT_Entry>
    365e:	bd10      	pop	{r4, pc}

00003660 <INT_PWR_DOWN>:
    3660:	b510      	push	{r4, lr}
    3662:	2019      	movs	r0, #25
    3664:	f7ff ff86 	bl	3574 <INT_Entry>
    3668:	bd10      	pop	{r4, pc}

0000366a <INT_BOR>:
    366a:	b510      	push	{r4, lr}
    366c:	2018      	movs	r0, #24
    366e:	f7ff ff81 	bl	3574 <INT_Entry>
    3672:	bd10      	pop	{r4, pc}

00003674 <INT_ADC_COMP>:
    3674:	b510      	push	{r4, lr}
    3676:	2017      	movs	r0, #23
    3678:	f7ff ff7c 	bl	3574 <INT_Entry>
    367c:	bd10      	pop	{r4, pc}

0000367e <INT_ADC>:
    367e:	b510      	push	{r4, lr}
    3680:	2016      	movs	r0, #22
    3682:	f7ff ff77 	bl	3574 <INT_Entry>
    3686:	bd10      	pop	{r4, pc}

00003688 <INT_CTOUCH>:
    3688:	b510      	push	{r4, lr}
    368a:	2015      	movs	r0, #21
    368c:	f7ff ff72 	bl	3574 <INT_Entry>
    3690:	bd10      	pop	{r4, pc}

00003692 <INT_RTC>:
    3692:	b510      	push	{r4, lr}
    3694:	2014      	movs	r0, #20
    3696:	f7ff ff6d 	bl	3574 <INT_Entry>
    369a:	bd10      	pop	{r4, pc}

0000369c <INT_GPIOC>:
    369c:	b510      	push	{r4, lr}
    369e:	2013      	movs	r0, #19
    36a0:	f7ff ff68 	bl	3574 <INT_Entry>
    36a4:	bd10      	pop	{r4, pc}

000036a6 <INT_GPIOB>:
    36a6:	b510      	push	{r4, lr}
    36a8:	2012      	movs	r0, #18
    36aa:	f7ff ff63 	bl	3574 <INT_Entry>
    36ae:	bd10      	pop	{r4, pc}

000036b0 <INT_GPIOA>:
    36b0:	b510      	push	{r4, lr}
    36b2:	2011      	movs	r0, #17
    36b4:	f7ff ff5e 	bl	3574 <INT_Entry>
    36b8:	bd10      	pop	{r4, pc}

000036ba <INT_UART_LOG>:
    36ba:	b510      	push	{r4, lr}
    36bc:	2010      	movs	r0, #16
    36be:	f7ff ff59 	bl	3574 <INT_Entry>
    36c2:	bd10      	pop	{r4, pc}

000036c4 <INT_TIMER7>:
    36c4:	b510      	push	{r4, lr}
    36c6:	200f      	movs	r0, #15
    36c8:	f7ff ff54 	bl	3574 <INT_Entry>
    36cc:	bd10      	pop	{r4, pc}

000036ce <INT_TIMER6>:
    36ce:	b510      	push	{r4, lr}
    36d0:	200e      	movs	r0, #14
    36d2:	f7ff ff4f 	bl	3574 <INT_Entry>
    36d6:	bd10      	pop	{r4, pc}

000036d8 <INT_TIMER5>:
    36d8:	b510      	push	{r4, lr}
    36da:	200d      	movs	r0, #13
    36dc:	f7ff ff4a 	bl	3574 <INT_Entry>
    36e0:	bd10      	pop	{r4, pc}

000036e2 <INT_TIMER4>:
    36e2:	b510      	push	{r4, lr}
    36e4:	200c      	movs	r0, #12
    36e6:	f7ff ff45 	bl	3574 <INT_Entry>
    36ea:	bd10      	pop	{r4, pc}

000036ec <INT_TIMER3>:
    36ec:	b510      	push	{r4, lr}
    36ee:	200b      	movs	r0, #11
    36f0:	f7ff ff40 	bl	3574 <INT_Entry>
    36f4:	bd10      	pop	{r4, pc}

000036f6 <INT_TIMER2>:
    36f6:	b510      	push	{r4, lr}
    36f8:	200a      	movs	r0, #10
    36fa:	f7ff ff3b 	bl	3574 <INT_Entry>
    36fe:	bd10      	pop	{r4, pc}

00003700 <INT_TIMER1>:
    3700:	b510      	push	{r4, lr}
    3702:	2009      	movs	r0, #9
    3704:	f7ff ff36 	bl	3574 <INT_Entry>
    3708:	bd10      	pop	{r4, pc}

0000370a <INT_TIMER0>:
    370a:	b510      	push	{r4, lr}
    370c:	2008      	movs	r0, #8
    370e:	f7ff ff31 	bl	3574 <INT_Entry>
    3712:	bd10      	pop	{r4, pc}

00003714 <INT_WDG0>:
    3714:	b510      	push	{r4, lr}
    3716:	2007      	movs	r0, #7
    3718:	f7ff ff2c 	bl	3574 <INT_Entry>
    371c:	bd10      	pop	{r4, pc}

0000371e <INT_AP_WAKE>:
    371e:	b510      	push	{r4, lr}
    3720:	2006      	movs	r0, #6
    3722:	f7ff ff27 	bl	3574 <INT_Entry>
    3726:	bd10      	pop	{r4, pc}

00003728 <INT_NP_WAKE>:
    3728:	b510      	push	{r4, lr}
    372a:	2005      	movs	r0, #5
    372c:	f7ff ff22 	bl	3574 <INT_Entry>
    3730:	bd10      	pop	{r4, pc}

00003732 <INT_AON_TIM>:
    3732:	b510      	push	{r4, lr}
    3734:	2004      	movs	r0, #4
    3736:	f7ff ff1d 	bl	3574 <INT_Entry>
    373a:	bd10      	pop	{r4, pc}

0000373c <INT_WIFI_FTSR_MAILBOX>:
    373c:	b510      	push	{r4, lr}
    373e:	2003      	movs	r0, #3
    3740:	f7ff ff18 	bl	3574 <INT_Entry>
    3744:	bd10      	pop	{r4, pc}

00003746 <INT_WIFI_FISR_FESR>:
    3746:	b510      	push	{r4, lr}
    3748:	2002      	movs	r0, #2
    374a:	f7ff ff13 	bl	3574 <INT_Entry>
    374e:	bd10      	pop	{r4, pc}

00003750 <INT_RXI300>:
    3750:	b510      	push	{r4, lr}
    3752:	2001      	movs	r0, #1
    3754:	f7ff ff0e 	bl	3574 <INT_Entry>
    3758:	bd10      	pop	{r4, pc}

0000375a <INT_IPC>:
    375a:	b510      	push	{r4, lr}
    375c:	2000      	movs	r0, #0
    375e:	f7ff ff09 	bl	3574 <INT_Entry>
    3762:	bd10      	pop	{r4, pc}

00003764 <INT_HardFault_C>:
    3764:	b570      	push	{r4, r5, r6, lr}
    3766:	2680      	movs	r6, #128	; 0x80
    3768:	4c24      	ldr	r4, [pc, #144]	; (37fc <INT_HardFault_C+0x98>)
    376a:	02b6      	lsls	r6, r6, #10
    376c:	6823      	ldr	r3, [r4, #0]
    376e:	0005      	movs	r5, r0
    3770:	4233      	tst	r3, r6
    3772:	d03a      	beq.n	37ea <INT_HardFault_C+0x86>
    3774:	4822      	ldr	r0, [pc, #136]	; (3800 <INT_HardFault_C+0x9c>)
    3776:	f000 fbff 	bl	3f78 <DiagPrintf>
    377a:	6823      	ldr	r3, [r4, #0]
    377c:	4233      	tst	r3, r6
    377e:	d034      	beq.n	37ea <INT_HardFault_C+0x86>
    3780:	6829      	ldr	r1, [r5, #0]
    3782:	4820      	ldr	r0, [pc, #128]	; (3804 <INT_HardFault_C+0xa0>)
    3784:	f000 fbf8 	bl	3f78 <DiagPrintf>
    3788:	6823      	ldr	r3, [r4, #0]
    378a:	4233      	tst	r3, r6
    378c:	d02d      	beq.n	37ea <INT_HardFault_C+0x86>
    378e:	6869      	ldr	r1, [r5, #4]
    3790:	481d      	ldr	r0, [pc, #116]	; (3808 <INT_HardFault_C+0xa4>)
    3792:	f000 fbf1 	bl	3f78 <DiagPrintf>
    3796:	6823      	ldr	r3, [r4, #0]
    3798:	4233      	tst	r3, r6
    379a:	d026      	beq.n	37ea <INT_HardFault_C+0x86>
    379c:	68a9      	ldr	r1, [r5, #8]
    379e:	481b      	ldr	r0, [pc, #108]	; (380c <INT_HardFault_C+0xa8>)
    37a0:	f000 fbea 	bl	3f78 <DiagPrintf>
    37a4:	6823      	ldr	r3, [r4, #0]
    37a6:	4233      	tst	r3, r6
    37a8:	d01f      	beq.n	37ea <INT_HardFault_C+0x86>
    37aa:	68e9      	ldr	r1, [r5, #12]
    37ac:	4818      	ldr	r0, [pc, #96]	; (3810 <INT_HardFault_C+0xac>)
    37ae:	f000 fbe3 	bl	3f78 <DiagPrintf>
    37b2:	6823      	ldr	r3, [r4, #0]
    37b4:	4233      	tst	r3, r6
    37b6:	d018      	beq.n	37ea <INT_HardFault_C+0x86>
    37b8:	6929      	ldr	r1, [r5, #16]
    37ba:	4816      	ldr	r0, [pc, #88]	; (3814 <INT_HardFault_C+0xb0>)
    37bc:	f000 fbdc 	bl	3f78 <DiagPrintf>
    37c0:	6823      	ldr	r3, [r4, #0]
    37c2:	4233      	tst	r3, r6
    37c4:	d011      	beq.n	37ea <INT_HardFault_C+0x86>
    37c6:	6969      	ldr	r1, [r5, #20]
    37c8:	4813      	ldr	r0, [pc, #76]	; (3818 <INT_HardFault_C+0xb4>)
    37ca:	f000 fbd5 	bl	3f78 <DiagPrintf>
    37ce:	6823      	ldr	r3, [r4, #0]
    37d0:	4233      	tst	r3, r6
    37d2:	d00a      	beq.n	37ea <INT_HardFault_C+0x86>
    37d4:	69a9      	ldr	r1, [r5, #24]
    37d6:	4811      	ldr	r0, [pc, #68]	; (381c <INT_HardFault_C+0xb8>)
    37d8:	f000 fbce 	bl	3f78 <DiagPrintf>
    37dc:	6823      	ldr	r3, [r4, #0]
    37de:	4233      	tst	r3, r6
    37e0:	d003      	beq.n	37ea <INT_HardFault_C+0x86>
    37e2:	69e9      	ldr	r1, [r5, #28]
    37e4:	480e      	ldr	r0, [pc, #56]	; (3820 <INT_HardFault_C+0xbc>)
    37e6:	f000 fbc7 	bl	3f78 <DiagPrintf>
    37ea:	6823      	ldr	r3, [r4, #0]
    37ec:	039b      	lsls	r3, r3, #14
    37ee:	d504      	bpl.n	37fa <INT_HardFault_C+0x96>
    37f0:	f3ef 8110 	mrs	r1, PRIMASK
    37f4:	480b      	ldr	r0, [pc, #44]	; (3824 <INT_HardFault_C+0xc0>)
    37f6:	f000 fbbf 	bl	3f78 <DiagPrintf>
    37fa:	e7fe      	b.n	37fa <INT_HardFault_C+0x96>
    37fc:	230005d8 	.word	0x230005d8
    3800:	0000c12b 	.word	0x0000c12b
    3804:	0000c142 	.word	0x0000c142
    3808:	0000c14e 	.word	0x0000c14e
    380c:	0000c15a 	.word	0x0000c15a
    3810:	0000c166 	.word	0x0000c166
    3814:	0000c172 	.word	0x0000c172
    3818:	0000c17f 	.word	0x0000c17f
    381c:	0000c18b 	.word	0x0000c18b
    3820:	0000c197 	.word	0x0000c197
    3824:	0000c1a5 	.word	0x0000c1a5

00003828 <INT_NMI>:
    3828:	4b04      	ldr	r3, [pc, #16]	; (383c <INT_NMI+0x14>)
    382a:	b510      	push	{r4, lr}
    382c:	681b      	ldr	r3, [r3, #0]
    382e:	039b      	lsls	r3, r3, #14
    3830:	d502      	bpl.n	3838 <INT_NMI+0x10>
    3832:	4803      	ldr	r0, [pc, #12]	; (3840 <INT_NMI+0x18>)
    3834:	f000 fba0 	bl	3f78 <DiagPrintf>
    3838:	bd10      	pop	{r4, pc}
    383a:	46c0      	nop			; (mov r8, r8)
    383c:	230005d8 	.word	0x230005d8
    3840:	0000c1b5 	.word	0x0000c1b5

00003844 <INT_HardFault>:
    3844:	2004      	movs	r0, #4
    3846:	4671      	mov	r1, lr
    3848:	4208      	tst	r0, r1
    384a:	d002      	beq.n	3852 <stacking_used_MSP>
    384c:	f3ef 8009 	mrs	r0, PSP
    3850:	e001      	b.n	3856 <branch>

00003852 <stacking_used_MSP>:
    3852:	f3ef 8008 	mrs	r0, MSP

00003856 <branch>:
    3856:	4af8      	ldr	r2, [pc, #992]	; (3c38 <irq_unregister+0x18>)
    3858:	4710      	bx	r2
    385a:	4770      	bx	lr

0000385c <INT_MemManage>:
    385c:	4b04      	ldr	r3, [pc, #16]	; (3870 <INT_MemManage+0x14>)
    385e:	b510      	push	{r4, lr}
    3860:	681b      	ldr	r3, [r3, #0]
    3862:	039b      	lsls	r3, r3, #14
    3864:	d502      	bpl.n	386c <INT_MemManage+0x10>
    3866:	4803      	ldr	r0, [pc, #12]	; (3874 <INT_MemManage+0x18>)
    3868:	f000 fb86 	bl	3f78 <DiagPrintf>
    386c:	e7fe      	b.n	386c <INT_MemManage+0x10>
    386e:	46c0      	nop			; (mov r8, r8)
    3870:	230005d8 	.word	0x230005d8
    3874:	0000c1cd 	.word	0x0000c1cd

00003878 <INT_BusFault>:
    3878:	4b04      	ldr	r3, [pc, #16]	; (388c <INT_BusFault+0x14>)
    387a:	b510      	push	{r4, lr}
    387c:	681b      	ldr	r3, [r3, #0]
    387e:	039b      	lsls	r3, r3, #14
    3880:	d502      	bpl.n	3888 <INT_BusFault+0x10>
    3882:	4803      	ldr	r0, [pc, #12]	; (3890 <INT_BusFault+0x18>)
    3884:	f000 fb78 	bl	3f78 <DiagPrintf>
    3888:	e7fe      	b.n	3888 <INT_BusFault+0x10>
    388a:	46c0      	nop			; (mov r8, r8)
    388c:	230005d8 	.word	0x230005d8
    3890:	0000c1e7 	.word	0x0000c1e7

00003894 <INT_UsageFault>:
    3894:	4b04      	ldr	r3, [pc, #16]	; (38a8 <INT_UsageFault+0x14>)
    3896:	b510      	push	{r4, lr}
    3898:	681b      	ldr	r3, [r3, #0]
    389a:	039b      	lsls	r3, r3, #14
    389c:	d502      	bpl.n	38a4 <INT_UsageFault+0x10>
    389e:	4803      	ldr	r0, [pc, #12]	; (38ac <INT_UsageFault+0x18>)
    38a0:	f000 fb6a 	bl	3f78 <DiagPrintf>
    38a4:	e7fe      	b.n	38a4 <INT_UsageFault+0x10>
    38a6:	46c0      	nop			; (mov r8, r8)
    38a8:	230005d8 	.word	0x230005d8
    38ac:	0000c1fd 	.word	0x0000c1fd

000038b0 <irq_table_init>:
    38b0:	2204      	movs	r2, #4
    38b2:	4b4a      	ldr	r3, [pc, #296]	; (39dc <irq_table_init+0x12c>)
    38b4:	6812      	ldr	r2, [r2, #0]
    38b6:	494a      	ldr	r1, [pc, #296]	; (39e0 <irq_table_init+0x130>)
    38b8:	605a      	str	r2, [r3, #4]
    38ba:	2208      	movs	r2, #8
    38bc:	6812      	ldr	r2, [r2, #0]
    38be:	6018      	str	r0, [r3, #0]
    38c0:	609a      	str	r2, [r3, #8]
    38c2:	220c      	movs	r2, #12
    38c4:	6812      	ldr	r2, [r2, #0]
    38c6:	60da      	str	r2, [r3, #12]
    38c8:	4a46      	ldr	r2, [pc, #280]	; (39e4 <irq_table_init+0x134>)
    38ca:	641a      	str	r2, [r3, #64]	; 0x40
    38cc:	4a46      	ldr	r2, [pc, #280]	; (39e8 <irq_table_init+0x138>)
    38ce:	645a      	str	r2, [r3, #68]	; 0x44
    38d0:	4a46      	ldr	r2, [pc, #280]	; (39ec <irq_table_init+0x13c>)
    38d2:	649a      	str	r2, [r3, #72]	; 0x48
    38d4:	4a46      	ldr	r2, [pc, #280]	; (39f0 <irq_table_init+0x140>)
    38d6:	64da      	str	r2, [r3, #76]	; 0x4c
    38d8:	4a46      	ldr	r2, [pc, #280]	; (39f4 <irq_table_init+0x144>)
    38da:	651a      	str	r2, [r3, #80]	; 0x50
    38dc:	4a46      	ldr	r2, [pc, #280]	; (39f8 <irq_table_init+0x148>)
    38de:	655a      	str	r2, [r3, #84]	; 0x54
    38e0:	4a46      	ldr	r2, [pc, #280]	; (39fc <irq_table_init+0x14c>)
    38e2:	659a      	str	r2, [r3, #88]	; 0x58
    38e4:	4a46      	ldr	r2, [pc, #280]	; (3a00 <irq_table_init+0x150>)
    38e6:	65da      	str	r2, [r3, #92]	; 0x5c
    38e8:	4a46      	ldr	r2, [pc, #280]	; (3a04 <irq_table_init+0x154>)
    38ea:	661a      	str	r2, [r3, #96]	; 0x60
    38ec:	4a46      	ldr	r2, [pc, #280]	; (3a08 <irq_table_init+0x158>)
    38ee:	665a      	str	r2, [r3, #100]	; 0x64
    38f0:	4a46      	ldr	r2, [pc, #280]	; (3a0c <irq_table_init+0x15c>)
    38f2:	669a      	str	r2, [r3, #104]	; 0x68
    38f4:	4a46      	ldr	r2, [pc, #280]	; (3a10 <irq_table_init+0x160>)
    38f6:	66da      	str	r2, [r3, #108]	; 0x6c
    38f8:	4a46      	ldr	r2, [pc, #280]	; (3a14 <irq_table_init+0x164>)
    38fa:	671a      	str	r2, [r3, #112]	; 0x70
    38fc:	4a46      	ldr	r2, [pc, #280]	; (3a18 <irq_table_init+0x168>)
    38fe:	675a      	str	r2, [r3, #116]	; 0x74
    3900:	4a46      	ldr	r2, [pc, #280]	; (3a1c <irq_table_init+0x16c>)
    3902:	679a      	str	r2, [r3, #120]	; 0x78
    3904:	4a46      	ldr	r2, [pc, #280]	; (3a20 <irq_table_init+0x170>)
    3906:	67da      	str	r2, [r3, #124]	; 0x7c
    3908:	1d1a      	adds	r2, r3, #4
    390a:	67d1      	str	r1, [r2, #124]	; 0x7c
    390c:	001a      	movs	r2, r3
    390e:	4945      	ldr	r1, [pc, #276]	; (3a24 <irq_table_init+0x174>)
    3910:	3208      	adds	r2, #8
    3912:	67d1      	str	r1, [r2, #124]	; 0x7c
    3914:	001a      	movs	r2, r3
    3916:	4944      	ldr	r1, [pc, #272]	; (3a28 <irq_table_init+0x178>)
    3918:	3288      	adds	r2, #136	; 0x88
    391a:	6011      	str	r1, [r2, #0]
    391c:	001a      	movs	r2, r3
    391e:	4943      	ldr	r1, [pc, #268]	; (3a2c <irq_table_init+0x17c>)
    3920:	328c      	adds	r2, #140	; 0x8c
    3922:	6011      	str	r1, [r2, #0]
    3924:	001a      	movs	r2, r3
    3926:	4942      	ldr	r1, [pc, #264]	; (3a30 <irq_table_init+0x180>)
    3928:	3290      	adds	r2, #144	; 0x90
    392a:	6011      	str	r1, [r2, #0]
    392c:	001a      	movs	r2, r3
    392e:	4941      	ldr	r1, [pc, #260]	; (3a34 <irq_table_init+0x184>)
    3930:	3294      	adds	r2, #148	; 0x94
    3932:	6011      	str	r1, [r2, #0]
    3934:	001a      	movs	r2, r3
    3936:	4940      	ldr	r1, [pc, #256]	; (3a38 <irq_table_init+0x188>)
    3938:	3298      	adds	r2, #152	; 0x98
    393a:	6011      	str	r1, [r2, #0]
    393c:	001a      	movs	r2, r3
    393e:	493f      	ldr	r1, [pc, #252]	; (3a3c <irq_table_init+0x18c>)
    3940:	329c      	adds	r2, #156	; 0x9c
    3942:	6011      	str	r1, [r2, #0]
    3944:	001a      	movs	r2, r3
    3946:	493e      	ldr	r1, [pc, #248]	; (3a40 <irq_table_init+0x190>)
    3948:	32a0      	adds	r2, #160	; 0xa0
    394a:	6011      	str	r1, [r2, #0]
    394c:	001a      	movs	r2, r3
    394e:	493d      	ldr	r1, [pc, #244]	; (3a44 <irq_table_init+0x194>)
    3950:	32a4      	adds	r2, #164	; 0xa4
    3952:	6011      	str	r1, [r2, #0]
    3954:	001a      	movs	r2, r3
    3956:	493c      	ldr	r1, [pc, #240]	; (3a48 <irq_table_init+0x198>)
    3958:	32a8      	adds	r2, #168	; 0xa8
    395a:	6011      	str	r1, [r2, #0]
    395c:	001a      	movs	r2, r3
    395e:	493b      	ldr	r1, [pc, #236]	; (3a4c <irq_table_init+0x19c>)
    3960:	32ac      	adds	r2, #172	; 0xac
    3962:	6011      	str	r1, [r2, #0]
    3964:	001a      	movs	r2, r3
    3966:	493a      	ldr	r1, [pc, #232]	; (3a50 <irq_table_init+0x1a0>)
    3968:	32b0      	adds	r2, #176	; 0xb0
    396a:	6011      	str	r1, [r2, #0]
    396c:	001a      	movs	r2, r3
    396e:	4939      	ldr	r1, [pc, #228]	; (3a54 <irq_table_init+0x1a4>)
    3970:	32b4      	adds	r2, #180	; 0xb4
    3972:	6011      	str	r1, [r2, #0]
    3974:	001a      	movs	r2, r3
    3976:	4938      	ldr	r1, [pc, #224]	; (3a58 <irq_table_init+0x1a8>)
    3978:	32b8      	adds	r2, #184	; 0xb8
    397a:	6011      	str	r1, [r2, #0]
    397c:	001a      	movs	r2, r3
    397e:	4937      	ldr	r1, [pc, #220]	; (3a5c <irq_table_init+0x1ac>)
    3980:	32bc      	adds	r2, #188	; 0xbc
    3982:	6011      	str	r1, [r2, #0]
    3984:	001a      	movs	r2, r3
    3986:	4936      	ldr	r1, [pc, #216]	; (3a60 <irq_table_init+0x1b0>)
    3988:	32c0      	adds	r2, #192	; 0xc0
    398a:	6011      	str	r1, [r2, #0]
    398c:	001a      	movs	r2, r3
    398e:	4935      	ldr	r1, [pc, #212]	; (3a64 <irq_table_init+0x1b4>)
    3990:	32c4      	adds	r2, #196	; 0xc4
    3992:	6011      	str	r1, [r2, #0]
    3994:	001a      	movs	r2, r3
    3996:	4934      	ldr	r1, [pc, #208]	; (3a68 <irq_table_init+0x1b8>)
    3998:	32c8      	adds	r2, #200	; 0xc8
    399a:	6011      	str	r1, [r2, #0]
    399c:	001a      	movs	r2, r3
    399e:	4933      	ldr	r1, [pc, #204]	; (3a6c <irq_table_init+0x1bc>)
    39a0:	32cc      	adds	r2, #204	; 0xcc
    39a2:	6011      	str	r1, [r2, #0]
    39a4:	001a      	movs	r2, r3
    39a6:	4932      	ldr	r1, [pc, #200]	; (3a70 <irq_table_init+0x1c0>)
    39a8:	32d0      	adds	r2, #208	; 0xd0
    39aa:	6011      	str	r1, [r2, #0]
    39ac:	001a      	movs	r2, r3
    39ae:	4931      	ldr	r1, [pc, #196]	; (3a74 <irq_table_init+0x1c4>)
    39b0:	32d4      	adds	r2, #212	; 0xd4
    39b2:	6011      	str	r1, [r2, #0]
    39b4:	001a      	movs	r2, r3
    39b6:	4930      	ldr	r1, [pc, #192]	; (3a78 <irq_table_init+0x1c8>)
    39b8:	32d8      	adds	r2, #216	; 0xd8
    39ba:	6011      	str	r1, [r2, #0]
    39bc:	001a      	movs	r2, r3
    39be:	492f      	ldr	r1, [pc, #188]	; (3a7c <irq_table_init+0x1cc>)
    39c0:	32dc      	adds	r2, #220	; 0xdc
    39c2:	6011      	str	r1, [r2, #0]
    39c4:	001a      	movs	r2, r3
    39c6:	492e      	ldr	r1, [pc, #184]	; (3a80 <irq_table_init+0x1d0>)
    39c8:	32e0      	adds	r2, #224	; 0xe0
    39ca:	6011      	str	r1, [r2, #0]
    39cc:	001a      	movs	r2, r3
    39ce:	492d      	ldr	r1, [pc, #180]	; (3a84 <irq_table_init+0x1d4>)
    39d0:	32e4      	adds	r2, #228	; 0xe4
    39d2:	6011      	str	r1, [r2, #0]
    39d4:	4a2c      	ldr	r2, [pc, #176]	; (3a88 <irq_table_init+0x1d8>)
    39d6:	6093      	str	r3, [r2, #8]
    39d8:	4770      	bx	lr
    39da:	46c0      	nop			; (mov r8, r8)
    39dc:	23000000 	.word	0x23000000
    39e0:	000036bb 	.word	0x000036bb
    39e4:	0000375b 	.word	0x0000375b
    39e8:	00003751 	.word	0x00003751
    39ec:	00003747 	.word	0x00003747
    39f0:	0000373d 	.word	0x0000373d
    39f4:	00003733 	.word	0x00003733
    39f8:	00003729 	.word	0x00003729
    39fc:	0000371f 	.word	0x0000371f
    3a00:	00003715 	.word	0x00003715
    3a04:	0000370b 	.word	0x0000370b
    3a08:	00003701 	.word	0x00003701
    3a0c:	000036f7 	.word	0x000036f7
    3a10:	000036ed 	.word	0x000036ed
    3a14:	000036e3 	.word	0x000036e3
    3a18:	000036d9 	.word	0x000036d9
    3a1c:	000036cf 	.word	0x000036cf
    3a20:	000036c5 	.word	0x000036c5
    3a24:	000036b1 	.word	0x000036b1
    3a28:	000036a7 	.word	0x000036a7
    3a2c:	0000369d 	.word	0x0000369d
    3a30:	00003693 	.word	0x00003693
    3a34:	00003689 	.word	0x00003689
    3a38:	0000367f 	.word	0x0000367f
    3a3c:	00003675 	.word	0x00003675
    3a40:	0000366b 	.word	0x0000366b
    3a44:	00003661 	.word	0x00003661
    3a48:	00003657 	.word	0x00003657
    3a4c:	0000364d 	.word	0x0000364d
    3a50:	00003643 	.word	0x00003643
    3a54:	00003639 	.word	0x00003639
    3a58:	0000362f 	.word	0x0000362f
    3a5c:	00003625 	.word	0x00003625
    3a60:	0000361b 	.word	0x0000361b
    3a64:	00003611 	.word	0x00003611
    3a68:	00003607 	.word	0x00003607
    3a6c:	000035fd 	.word	0x000035fd
    3a70:	000035f3 	.word	0x000035f3
    3a74:	000035e9 	.word	0x000035e9
    3a78:	000035df 	.word	0x000035df
    3a7c:	000035d5 	.word	0x000035d5
    3a80:	000035cb 	.word	0x000035cb
    3a84:	000035c1 	.word	0x000035c1
    3a88:	e000ed00 	.word	0xe000ed00

00003a8c <irq_enable>:
    3a8c:	2800      	cmp	r0, #0
    3a8e:	db07      	blt.n	3aa0 <irq_enable+0x14>
    3a90:	221f      	movs	r2, #31
    3a92:	0943      	lsrs	r3, r0, #5
    3a94:	4010      	ands	r0, r2
    3a96:	3a1e      	subs	r2, #30
    3a98:	4082      	lsls	r2, r0
    3a9a:	4902      	ldr	r1, [pc, #8]	; (3aa4 <irq_enable+0x18>)
    3a9c:	009b      	lsls	r3, r3, #2
    3a9e:	505a      	str	r2, [r3, r1]
    3aa0:	4770      	bx	lr
    3aa2:	46c0      	nop			; (mov r8, r8)
    3aa4:	e000e100 	.word	0xe000e100

00003aa8 <irq_disable>:
    3aa8:	2800      	cmp	r0, #0
    3aaa:	db0c      	blt.n	3ac6 <irq_disable+0x1e>
    3aac:	221f      	movs	r2, #31
    3aae:	0943      	lsrs	r3, r0, #5
    3ab0:	4010      	ands	r0, r2
    3ab2:	3a1e      	subs	r2, #30
    3ab4:	4082      	lsls	r2, r0
    3ab6:	4904      	ldr	r1, [pc, #16]	; (3ac8 <irq_disable+0x20>)
    3ab8:	3320      	adds	r3, #32
    3aba:	009b      	lsls	r3, r3, #2
    3abc:	505a      	str	r2, [r3, r1]
    3abe:	f3bf 8f4f 	dsb	sy
    3ac2:	f3bf 8f6f 	isb	sy
    3ac6:	4770      	bx	lr
    3ac8:	e000e100 	.word	0xe000e100

00003acc <irq_set_priority>:
    3acc:	0002      	movs	r2, r0
    3ace:	b510      	push	{r4, lr}
    3ad0:	2403      	movs	r4, #3
    3ad2:	4022      	ands	r2, r4
    3ad4:	40a2      	lsls	r2, r4
    3ad6:	23ff      	movs	r3, #255	; 0xff
    3ad8:	4093      	lsls	r3, r2
    3ada:	43db      	mvns	r3, r3
    3adc:	42a1      	cmp	r1, r4
    3ade:	d900      	bls.n	3ae2 <irq_set_priority+0x16>
    3ae0:	0021      	movs	r1, r4
    3ae2:	0189      	lsls	r1, r1, #6
    3ae4:	4091      	lsls	r1, r2
    3ae6:	2800      	cmp	r0, #0
    3ae8:	db0a      	blt.n	3b00 <irq_set_priority+0x34>
    3aea:	f240 3400 	movw	r4, #768	; 0x300
    3aee:	4a0a      	ldr	r2, [pc, #40]	; (3b18 <irq_set_priority+0x4c>)
    3af0:	0880      	lsrs	r0, r0, #2
    3af2:	0080      	lsls	r0, r0, #2
    3af4:	1880      	adds	r0, r0, r2
    3af6:	5902      	ldr	r2, [r0, r4]
    3af8:	4013      	ands	r3, r2
    3afa:	4319      	orrs	r1, r3
    3afc:	5101      	str	r1, [r0, r4]
    3afe:	bd10      	pop	{r4, pc}
    3b00:	220f      	movs	r2, #15
    3b02:	4010      	ands	r0, r2
    3b04:	3808      	subs	r0, #8
    3b06:	4a05      	ldr	r2, [pc, #20]	; (3b1c <irq_set_priority+0x50>)
    3b08:	0880      	lsrs	r0, r0, #2
    3b0a:	0080      	lsls	r0, r0, #2
    3b0c:	1880      	adds	r0, r0, r2
    3b0e:	69c2      	ldr	r2, [r0, #28]
    3b10:	4013      	ands	r3, r2
    3b12:	4319      	orrs	r1, r3
    3b14:	61c1      	str	r1, [r0, #28]
    3b16:	e7f2      	b.n	3afe <irq_set_priority+0x32>
    3b18:	e000e100 	.word	0xe000e100
    3b1c:	e000ed00 	.word	0xe000ed00

00003b20 <irq_get_priority>:
    3b20:	2203      	movs	r2, #3
    3b22:	0001      	movs	r1, r0
    3b24:	4011      	ands	r1, r2
    3b26:	4091      	lsls	r1, r2
    3b28:	2800      	cmp	r0, #0
    3b2a:	db08      	blt.n	3b3e <irq_get_priority+0x1e>
    3b2c:	0883      	lsrs	r3, r0, #2
    3b2e:	33c0      	adds	r3, #192	; 0xc0
    3b30:	4807      	ldr	r0, [pc, #28]	; (3b50 <irq_get_priority+0x30>)
    3b32:	009b      	lsls	r3, r3, #2
    3b34:	5818      	ldr	r0, [r3, r0]
    3b36:	40c8      	lsrs	r0, r1
    3b38:	0980      	lsrs	r0, r0, #6
    3b3a:	4010      	ands	r0, r2
    3b3c:	4770      	bx	lr
    3b3e:	230f      	movs	r3, #15
    3b40:	4003      	ands	r3, r0
    3b42:	3b08      	subs	r3, #8
    3b44:	4803      	ldr	r0, [pc, #12]	; (3b54 <irq_get_priority+0x34>)
    3b46:	089b      	lsrs	r3, r3, #2
    3b48:	009b      	lsls	r3, r3, #2
    3b4a:	181b      	adds	r3, r3, r0
    3b4c:	6858      	ldr	r0, [r3, #4]
    3b4e:	e7f2      	b.n	3b36 <irq_get_priority+0x16>
    3b50:	e000e100 	.word	0xe000e100
    3b54:	e000ed18 	.word	0xe000ed18

00003b58 <irq_set_pending>:
    3b58:	2800      	cmp	r0, #0
    3b5a:	db08      	blt.n	3b6e <irq_set_pending+0x16>
    3b5c:	221f      	movs	r2, #31
    3b5e:	0943      	lsrs	r3, r0, #5
    3b60:	4010      	ands	r0, r2
    3b62:	3a1e      	subs	r2, #30
    3b64:	4082      	lsls	r2, r0
    3b66:	4902      	ldr	r1, [pc, #8]	; (3b70 <irq_set_pending+0x18>)
    3b68:	3340      	adds	r3, #64	; 0x40
    3b6a:	009b      	lsls	r3, r3, #2
    3b6c:	505a      	str	r2, [r3, r1]
    3b6e:	4770      	bx	lr
    3b70:	e000e100 	.word	0xe000e100

00003b74 <irq_get_pending>:
    3b74:	0003      	movs	r3, r0
    3b76:	2000      	movs	r0, #0
    3b78:	4283      	cmp	r3, r0
    3b7a:	db09      	blt.n	3b90 <irq_get_pending+0x1c>
    3b7c:	095a      	lsrs	r2, r3, #5
    3b7e:	4905      	ldr	r1, [pc, #20]	; (3b94 <irq_get_pending+0x20>)
    3b80:	3240      	adds	r2, #64	; 0x40
    3b82:	0092      	lsls	r2, r2, #2
    3b84:	5850      	ldr	r0, [r2, r1]
    3b86:	221f      	movs	r2, #31
    3b88:	4013      	ands	r3, r2
    3b8a:	40d8      	lsrs	r0, r3
    3b8c:	2301      	movs	r3, #1
    3b8e:	4018      	ands	r0, r3
    3b90:	4770      	bx	lr
    3b92:	46c0      	nop			; (mov r8, r8)
    3b94:	e000e100 	.word	0xe000e100

00003b98 <irq_clear_pending>:
    3b98:	2800      	cmp	r0, #0
    3b9a:	db08      	blt.n	3bae <irq_clear_pending+0x16>
    3b9c:	221f      	movs	r2, #31
    3b9e:	0943      	lsrs	r3, r0, #5
    3ba0:	4010      	ands	r0, r2
    3ba2:	3a1e      	subs	r2, #30
    3ba4:	4082      	lsls	r2, r0
    3ba6:	4902      	ldr	r1, [pc, #8]	; (3bb0 <irq_clear_pending+0x18>)
    3ba8:	3360      	adds	r3, #96	; 0x60
    3baa:	009b      	lsls	r3, r3, #2
    3bac:	505a      	str	r2, [r3, r1]
    3bae:	4770      	bx	lr
    3bb0:	e000e100 	.word	0xe000e100

00003bb4 <irq_register>:
    3bb4:	b530      	push	{r4, r5, lr}
    3bb6:	2401      	movs	r4, #1
    3bb8:	4304      	orrs	r4, r0
    3bba:	4815      	ldr	r0, [pc, #84]	; (3c10 <irq_register+0x5c>)
    3bbc:	008d      	lsls	r5, r1, #2
    3bbe:	502c      	str	r4, [r5, r0]
    3bc0:	4814      	ldr	r0, [pc, #80]	; (3c14 <irq_register+0x60>)
    3bc2:	2403      	movs	r4, #3
    3bc4:	5142      	str	r2, [r0, r5]
    3bc6:	0008      	movs	r0, r1
    3bc8:	4020      	ands	r0, r4
    3bca:	40a0      	lsls	r0, r4
    3bcc:	22ff      	movs	r2, #255	; 0xff
    3bce:	4082      	lsls	r2, r0
    3bd0:	43d2      	mvns	r2, r2
    3bd2:	42a3      	cmp	r3, r4
    3bd4:	d900      	bls.n	3bd8 <irq_register+0x24>
    3bd6:	0023      	movs	r3, r4
    3bd8:	019b      	lsls	r3, r3, #6
    3bda:	4083      	lsls	r3, r0
    3bdc:	2900      	cmp	r1, #0
    3bde:	db0b      	blt.n	3bf8 <irq_register+0x44>
    3be0:	f240 3400 	movw	r4, #768	; 0x300
    3be4:	480c      	ldr	r0, [pc, #48]	; (3c18 <irq_register+0x64>)
    3be6:	0889      	lsrs	r1, r1, #2
    3be8:	0089      	lsls	r1, r1, #2
    3bea:	1809      	adds	r1, r1, r0
    3bec:	5908      	ldr	r0, [r1, r4]
    3bee:	4002      	ands	r2, r0
    3bf0:	4313      	orrs	r3, r2
    3bf2:	510b      	str	r3, [r1, r4]
    3bf4:	2001      	movs	r0, #1
    3bf6:	bd30      	pop	{r4, r5, pc}
    3bf8:	200f      	movs	r0, #15
    3bfa:	4001      	ands	r1, r0
    3bfc:	3908      	subs	r1, #8
    3bfe:	4807      	ldr	r0, [pc, #28]	; (3c1c <irq_register+0x68>)
    3c00:	0889      	lsrs	r1, r1, #2
    3c02:	0089      	lsls	r1, r1, #2
    3c04:	1809      	adds	r1, r1, r0
    3c06:	69c8      	ldr	r0, [r1, #28]
    3c08:	4002      	ands	r2, r0
    3c0a:	4313      	orrs	r3, r2
    3c0c:	61cb      	str	r3, [r1, #28]
    3c0e:	e7f1      	b.n	3bf4 <irq_register+0x40>
    3c10:	230000f0 	.word	0x230000f0
    3c14:	230001a0 	.word	0x230001a0
    3c18:	e000e100 	.word	0xe000e100
    3c1c:	e000ed00 	.word	0xe000ed00

00003c20 <irq_unregister>:
    3c20:	2300      	movs	r3, #0
    3c22:	4a03      	ldr	r2, [pc, #12]	; (3c30 <irq_unregister+0x10>)
    3c24:	0080      	lsls	r0, r0, #2
    3c26:	5083      	str	r3, [r0, r2]
    3c28:	4a02      	ldr	r2, [pc, #8]	; (3c34 <irq_unregister+0x14>)
    3c2a:	5083      	str	r3, [r0, r2]
    3c2c:	2001      	movs	r0, #1
    3c2e:	4770      	bx	lr
    3c30:	230000f0 	.word	0x230000f0
    3c34:	230001a0 	.word	0x230001a0
    3c38:	00003765 	.word	0x00003765

00003c3c <DiagVSprintf>:
    3c3c:	b5f0      	push	{r4, r5, r6, r7, lr}
    3c3e:	4657      	mov	r7, sl
    3c40:	464e      	mov	r6, r9
    3c42:	46de      	mov	lr, fp
    3c44:	4645      	mov	r5, r8
    3c46:	4682      	mov	sl, r0
    3c48:	b5e0      	push	{r5, r6, r7, lr}
    3c4a:	7808      	ldrb	r0, [r1, #0]
    3c4c:	b08d      	sub	sp, #52	; 0x34
    3c4e:	4689      	mov	r9, r1
    3c50:	4654      	mov	r4, sl
    3c52:	9201      	str	r2, [sp, #4]
    3c54:	2800      	cmp	r0, #0
    3c56:	d100      	bne.n	3c5a <DiagVSprintf+0x1e>
    3c58:	e187      	b.n	3f6a <DiagVSprintf+0x32e>
    3c5a:	f241 2305 	movw	r3, #4613	; 0x1205
    3c5e:	f2c0 0300 	movt	r3, #0
    3c62:	9300      	str	r3, [sp, #0]
    3c64:	e00b      	b.n	3c7e <DiagVSprintf+0x42>
    3c66:	4653      	mov	r3, sl
    3c68:	2b00      	cmp	r3, #0
    3c6a:	d100      	bne.n	3c6e <DiagVSprintf+0x32>
    3c6c:	e0d0      	b.n	3e10 <DiagVSprintf+0x1d4>
    3c6e:	0033      	movs	r3, r6
    3c70:	464e      	mov	r6, r9
    3c72:	4699      	mov	r9, r3
    3c74:	7020      	strb	r0, [r4, #0]
    3c76:	3401      	adds	r4, #1
    3c78:	7870      	ldrb	r0, [r6, #1]
    3c7a:	2800      	cmp	r0, #0
    3c7c:	d050      	beq.n	3d20 <DiagVSprintf+0xe4>
    3c7e:	464b      	mov	r3, r9
    3c80:	1c5e      	adds	r6, r3, #1
    3c82:	2825      	cmp	r0, #37	; 0x25
    3c84:	d1ef      	bne.n	3c66 <DiagVSprintf+0x2a>
    3c86:	f240 0c02 	movw	ip, #2
    3c8a:	785d      	ldrb	r5, [r3, #1]
    3c8c:	9b01      	ldr	r3, [sp, #4]
    3c8e:	44e1      	add	r9, ip
    3c90:	681b      	ldr	r3, [r3, #0]
    3c92:	2d73      	cmp	r5, #115	; 0x73
    3c94:	d055      	beq.n	3d42 <DiagVSprintf+0x106>
    3c96:	2200      	movs	r2, #0
    3c98:	2b0f      	cmp	r3, #15
    3c9a:	dd0c      	ble.n	3cb6 <DiagVSprintf+0x7a>
    3c9c:	0019      	movs	r1, r3
    3c9e:	3910      	subs	r1, #16
    3ca0:	3204      	adds	r2, #4
    3ca2:	29ef      	cmp	r1, #239	; 0xef
    3ca4:	d907      	bls.n	3cb6 <DiagVSprintf+0x7a>
    3ca6:	f640 60ff 	movw	r0, #3839	; 0xeff
    3caa:	1e59      	subs	r1, r3, #1
    3cac:	39ff      	subs	r1, #255	; 0xff
    3cae:	3204      	adds	r2, #4
    3cb0:	4281      	cmp	r1, r0
    3cb2:	d900      	bls.n	3cb6 <DiagVSprintf+0x7a>
    3cb4:	e0b2      	b.n	3e1c <DiagVSprintf+0x1e0>
    3cb6:	0029      	movs	r1, r5
    3cb8:	3930      	subs	r1, #48	; 0x30
    3cba:	2909      	cmp	r1, #9
    3cbc:	d955      	bls.n	3d6a <DiagVSprintf+0x12e>
    3cbe:	2d78      	cmp	r5, #120	; 0x78
    3cc0:	d067      	beq.n	3d92 <DiagVSprintf+0x156>
    3cc2:	2d58      	cmp	r5, #88	; 0x58
    3cc4:	d100      	bne.n	3cc8 <DiagVSprintf+0x8c>
    3cc6:	e148      	b.n	3f5a <DiagVSprintf+0x31e>
    3cc8:	2d70      	cmp	r5, #112	; 0x70
    3cca:	d100      	bne.n	3cce <DiagVSprintf+0x92>
    3ccc:	e0d3      	b.n	3e76 <DiagVSprintf+0x23a>
    3cce:	2d50      	cmp	r5, #80	; 0x50
    3cd0:	d100      	bne.n	3cd4 <DiagVSprintf+0x98>
    3cd2:	e0e7      	b.n	3ea4 <DiagVSprintf+0x268>
    3cd4:	2d64      	cmp	r5, #100	; 0x64
    3cd6:	d100      	bne.n	3cda <DiagVSprintf+0x9e>
    3cd8:	e0e9      	b.n	3eae <DiagVSprintf+0x272>
    3cda:	2d63      	cmp	r5, #99	; 0x63
    3cdc:	d100      	bne.n	3ce0 <DiagVSprintf+0xa4>
    3cde:	e0c2      	b.n	3e66 <DiagVSprintf+0x22a>
    3ce0:	ab07      	add	r3, sp, #28
    3ce2:	9303      	str	r3, [sp, #12]
    3ce4:	701d      	strb	r5, [r3, #0]
    3ce6:	aa06      	add	r2, sp, #24
    3ce8:	2305      	movs	r3, #5
    3cea:	4694      	mov	ip, r2
    3cec:	4463      	add	r3, ip
    3cee:	9302      	str	r3, [sp, #8]
    3cf0:	46b0      	mov	r8, r6
    3cf2:	9f03      	ldr	r7, [sp, #12]
    3cf4:	9e02      	ldr	r6, [sp, #8]
    3cf6:	e003      	b.n	3d00 <DiagVSprintf+0xc4>
    3cf8:	3701      	adds	r7, #1
    3cfa:	42be      	cmp	r6, r7
    3cfc:	d90c      	bls.n	3d18 <DiagVSprintf+0xdc>
    3cfe:	783d      	ldrb	r5, [r7, #0]
    3d00:	4653      	mov	r3, sl
    3d02:	b1d3      	cbz	r3, 3d3a <DiagVSprintf+0xfe>
    3d04:	7025      	strb	r5, [r4, #0]
    3d06:	3401      	adds	r4, #1
    3d08:	2d0a      	cmp	r5, #10
    3d0a:	d1f5      	bne.n	3cf8 <DiagVSprintf+0xbc>
    3d0c:	200d      	movs	r0, #13
    3d0e:	9b00      	ldr	r3, [sp, #0]
    3d10:	3701      	adds	r7, #1
    3d12:	4798      	blx	r3
    3d14:	42be      	cmp	r6, r7
    3d16:	d8f2      	bhi.n	3cfe <DiagVSprintf+0xc2>
    3d18:	4646      	mov	r6, r8
    3d1a:	7870      	ldrb	r0, [r6, #1]
    3d1c:	2800      	cmp	r0, #0
    3d1e:	d1ae      	bne.n	3c7e <DiagVSprintf+0x42>
    3d20:	4653      	mov	r3, sl
    3d22:	1ae0      	subs	r0, r4, r3
    3d24:	4653      	mov	r3, sl
    3d26:	b10b      	cbz	r3, 3d2c <DiagVSprintf+0xf0>
    3d28:	2300      	movs	r3, #0
    3d2a:	7023      	strb	r3, [r4, #0]
    3d2c:	b00d      	add	sp, #52	; 0x34
    3d2e:	bcf0      	pop	{r4, r5, r6, r7}
    3d30:	46bb      	mov	fp, r7
    3d32:	46b2      	mov	sl, r6
    3d34:	46a9      	mov	r9, r5
    3d36:	46a0      	mov	r8, r4
    3d38:	bdf0      	pop	{r4, r5, r6, r7, pc}
    3d3a:	0028      	movs	r0, r5
    3d3c:	9b00      	ldr	r3, [sp, #0]
    3d3e:	4798      	blx	r3
    3d40:	e7e2      	b.n	3d08 <DiagVSprintf+0xcc>
    3d42:	9a01      	ldr	r2, [sp, #4]
    3d44:	7818      	ldrb	r0, [r3, #0]
    3d46:	3204      	adds	r2, #4
    3d48:	001d      	movs	r5, r3
    3d4a:	9201      	str	r2, [sp, #4]
    3d4c:	2800      	cmp	r0, #0
    3d4e:	d093      	beq.n	3c78 <DiagVSprintf+0x3c>
    3d50:	4657      	mov	r7, sl
    3d52:	b13f      	cbz	r7, 3d64 <DiagVSprintf+0x128>
    3d54:	7020      	strb	r0, [r4, #0]
    3d56:	3401      	adds	r4, #1
    3d58:	7868      	ldrb	r0, [r5, #1]
    3d5a:	3501      	adds	r5, #1
    3d5c:	2800      	cmp	r0, #0
    3d5e:	d1f8      	bne.n	3d52 <DiagVSprintf+0x116>
    3d60:	46ba      	mov	sl, r7
    3d62:	e789      	b.n	3c78 <DiagVSprintf+0x3c>
    3d64:	9b00      	ldr	r3, [sp, #0]
    3d66:	4798      	blx	r3
    3d68:	e7f6      	b.n	3d58 <DiagVSprintf+0x11c>
    3d6a:	2100      	movs	r1, #0
    3d6c:	008a      	lsls	r2, r1, #2
    3d6e:	1852      	adds	r2, r2, r1
    3d70:	0037      	movs	r7, r6
    3d72:	0052      	lsls	r2, r2, #1
    3d74:	3601      	adds	r6, #1
    3d76:	1952      	adds	r2, r2, r5
    3d78:	7835      	ldrb	r5, [r6, #0]
    3d7a:	0011      	movs	r1, r2
    3d7c:	0028      	movs	r0, r5
    3d7e:	3830      	subs	r0, #48	; 0x30
    3d80:	3930      	subs	r1, #48	; 0x30
    3d82:	2809      	cmp	r0, #9
    3d84:	d9f2      	bls.n	3d6c <DiagVSprintf+0x130>
    3d86:	1cb9      	adds	r1, r7, #2
    3d88:	3a31      	subs	r2, #49	; 0x31
    3d8a:	4689      	mov	r9, r1
    3d8c:	0092      	lsls	r2, r2, #2
    3d8e:	2d78      	cmp	r5, #120	; 0x78
    3d90:	d197      	bne.n	3cc2 <DiagVSprintf+0x86>
    3d92:	2720      	movs	r7, #32
    3d94:	9901      	ldr	r1, [sp, #4]
    3d96:	3104      	adds	r1, #4
    3d98:	9101      	str	r1, [sp, #4]
    3d9a:	a907      	add	r1, sp, #28
    3d9c:	9103      	str	r1, [sp, #12]
    3d9e:	9102      	str	r1, [sp, #8]
    3da0:	0018      	movs	r0, r3
    3da2:	2100      	movs	r1, #0
    3da4:	2b00      	cmp	r3, #0
    3da6:	d07a      	beq.n	3e9e <DiagVSprintf+0x262>
    3da8:	0900      	lsrs	r0, r0, #4
    3daa:	000d      	movs	r5, r1
    3dac:	3101      	adds	r1, #1
    3dae:	2800      	cmp	r0, #0
    3db0:	d1fa      	bne.n	3da8 <DiagVSprintf+0x16c>
    3db2:	00a9      	lsls	r1, r5, #2
    3db4:	428a      	cmp	r2, r1
    3db6:	da00      	bge.n	3dba <DiagVSprintf+0x17e>
    3db8:	000a      	movs	r2, r1
    3dba:	f24c 2818 	movw	r8, #49688	; 0xc218
    3dbe:	b279      	sxtb	r1, r7
    3dc0:	f2c0 0800 	movt	r8, #0
    3dc4:	468c      	mov	ip, r1
    3dc6:	4640      	mov	r0, r8
    3dc8:	0011      	movs	r1, r2
    3dca:	4690      	mov	r8, r2
    3dcc:	270f      	movs	r7, #15
    3dce:	4662      	mov	r2, ip
    3dd0:	46a4      	mov	ip, r4
    3dd2:	0004      	movs	r4, r0
    3dd4:	9d02      	ldr	r5, [sp, #8]
    3dd6:	0018      	movs	r0, r3
    3dd8:	40c8      	lsrs	r0, r1
    3dda:	4038      	ands	r0, r7
    3ddc:	5c20      	ldrb	r0, [r4, r0]
    3dde:	4310      	orrs	r0, r2
    3de0:	7028      	strb	r0, [r5, #0]
    3de2:	3501      	adds	r5, #1
    3de4:	3904      	subs	r1, #4
    3de6:	d5f6      	bpl.n	3dd6 <DiagVSprintf+0x19a>
    3de8:	4642      	mov	r2, r8
    3dea:	4664      	mov	r4, ip
    3dec:	2301      	movs	r3, #1
    3dee:	2a00      	cmp	r2, #0
    3df0:	db01      	blt.n	3df6 <DiagVSprintf+0x1ba>
    3df2:	0893      	lsrs	r3, r2, #2
    3df4:	3301      	adds	r3, #1
    3df6:	9a02      	ldr	r2, [sp, #8]
    3df8:	4694      	mov	ip, r2
    3dfa:	449c      	add	ip, r3
    3dfc:	4663      	mov	r3, ip
    3dfe:	9302      	str	r3, [sp, #8]
    3e00:	9b02      	ldr	r3, [sp, #8]
    3e02:	0019      	movs	r1, r3
    3e04:	9b03      	ldr	r3, [sp, #12]
    3e06:	4299      	cmp	r1, r3
    3e08:	d800      	bhi.n	3e0c <DiagVSprintf+0x1d0>
    3e0a:	e735      	b.n	3c78 <DiagVSprintf+0x3c>
    3e0c:	781d      	ldrb	r5, [r3, #0]
    3e0e:	e76f      	b.n	3cf0 <DiagVSprintf+0xb4>
    3e10:	9b00      	ldr	r3, [sp, #0]
    3e12:	4798      	blx	r3
    3e14:	0033      	movs	r3, r6
    3e16:	464e      	mov	r6, r9
    3e18:	4699      	mov	r9, r3
    3e1a:	e72d      	b.n	3c78 <DiagVSprintf+0x3c>
    3e1c:	f64e 70ff 	movw	r0, #61439	; 0xefff
    3e20:	4953      	ldr	r1, [pc, #332]	; (3f70 <DiagVSprintf+0x334>)
    3e22:	3204      	adds	r2, #4
    3e24:	1859      	adds	r1, r3, r1
    3e26:	4281      	cmp	r1, r0
    3e28:	d800      	bhi.n	3e2c <DiagVSprintf+0x1f0>
    3e2a:	e744      	b.n	3cb6 <DiagVSprintf+0x7a>
    3e2c:	20f0      	movs	r0, #240	; 0xf0
    3e2e:	4951      	ldr	r1, [pc, #324]	; (3f74 <DiagVSprintf+0x338>)
    3e30:	0300      	lsls	r0, r0, #12
    3e32:	1859      	adds	r1, r3, r1
    3e34:	3204      	adds	r2, #4
    3e36:	4281      	cmp	r1, r0
    3e38:	d200      	bcs.n	3e3c <DiagVSprintf+0x200>
    3e3a:	e73c      	b.n	3cb6 <DiagVSprintf+0x7a>
    3e3c:	2100      	movs	r1, #0
    3e3e:	20f0      	movs	r0, #240	; 0xf0
    3e40:	f6cf 71f0 	movt	r1, #65520	; 0xfff0
    3e44:	1859      	adds	r1, r3, r1
    3e46:	0400      	lsls	r0, r0, #16
    3e48:	3204      	adds	r2, #4
    3e4a:	4281      	cmp	r1, r0
    3e4c:	d200      	bcs.n	3e50 <DiagVSprintf+0x214>
    3e4e:	e732      	b.n	3cb6 <DiagVSprintf+0x7a>
    3e50:	22ff      	movs	r2, #255	; 0xff
    3e52:	21f0      	movs	r1, #240	; 0xf0
    3e54:	0612      	lsls	r2, r2, #24
    3e56:	0509      	lsls	r1, r1, #20
    3e58:	189a      	adds	r2, r3, r2
    3e5a:	428a      	cmp	r2, r1
    3e5c:	4192      	sbcs	r2, r2
    3e5e:	2103      	movs	r1, #3
    3e60:	438a      	bics	r2, r1
    3e62:	321c      	adds	r2, #28
    3e64:	e727      	b.n	3cb6 <DiagVSprintf+0x7a>
    3e66:	9a01      	ldr	r2, [sp, #4]
    3e68:	b2dd      	uxtb	r5, r3
    3e6a:	3204      	adds	r2, #4
    3e6c:	ab07      	add	r3, sp, #28
    3e6e:	9201      	str	r2, [sp, #4]
    3e70:	9303      	str	r3, [sp, #12]
    3e72:	701d      	strb	r5, [r3, #0]
    3e74:	e737      	b.n	3ce6 <DiagVSprintf+0xaa>
    3e76:	2720      	movs	r7, #32
    3e78:	9901      	ldr	r1, [sp, #4]
    3e7a:	3104      	adds	r1, #4
    3e7c:	9101      	str	r1, [sp, #4]
    3e7e:	2130      	movs	r1, #48	; 0x30
    3e80:	a807      	add	r0, sp, #28
    3e82:	7001      	strb	r1, [r0, #0]
    3e84:	2158      	movs	r1, #88	; 0x58
    3e86:	4339      	orrs	r1, r7
    3e88:	7041      	strb	r1, [r0, #1]
    3e8a:	9003      	str	r0, [sp, #12]
    3e8c:	a806      	add	r0, sp, #24
    3e8e:	4684      	mov	ip, r0
    3e90:	2106      	movs	r1, #6
    3e92:	4461      	add	r1, ip
    3e94:	9102      	str	r1, [sp, #8]
    3e96:	0018      	movs	r0, r3
    3e98:	2100      	movs	r1, #0
    3e9a:	2b00      	cmp	r3, #0
    3e9c:	d184      	bne.n	3da8 <DiagVSprintf+0x16c>
    3e9e:	2a00      	cmp	r2, #0
    3ea0:	dbae      	blt.n	3e00 <DiagVSprintf+0x1c4>
    3ea2:	e78a      	b.n	3dba <DiagVSprintf+0x17e>
    3ea4:	9901      	ldr	r1, [sp, #4]
    3ea6:	2700      	movs	r7, #0
    3ea8:	3104      	adds	r1, #4
    3eaa:	9101      	str	r1, [sp, #4]
    3eac:	e7e7      	b.n	3e7e <DiagVSprintf+0x242>
    3eae:	9901      	ldr	r1, [sp, #4]
    3eb0:	3104      	adds	r1, #4
    3eb2:	9101      	str	r1, [sp, #4]
    3eb4:	2b00      	cmp	r3, #0
    3eb6:	db43      	blt.n	3f40 <DiagVSprintf+0x304>
    3eb8:	2100      	movs	r1, #0
    3eba:	a807      	add	r0, sp, #28
    3ebc:	9003      	str	r0, [sp, #12]
    3ebe:	9004      	str	r0, [sp, #16]
    3ec0:	9d04      	ldr	r5, [sp, #16]
    3ec2:	9605      	str	r6, [sp, #20]
    3ec4:	f240 0c0a 	movw	ip, #10
    3ec8:	46a0      	mov	r8, r4
    3eca:	002e      	movs	r6, r5
    3ecc:	4693      	mov	fp, r2
    3ece:	1b49      	subs	r1, r1, r5
    3ed0:	9102      	str	r1, [sp, #8]
    3ed2:	fb93 f0fc 	sdiv	r0, r3, ip
    3ed6:	9902      	ldr	r1, [sp, #8]
    3ed8:	0034      	movs	r4, r6
    3eda:	1877      	adds	r7, r6, r1
    3edc:	0081      	lsls	r1, r0, #2
    3ede:	1809      	adds	r1, r1, r0
    3ee0:	0049      	lsls	r1, r1, #1
    3ee2:	3601      	adds	r6, #1
    3ee4:	1a5b      	subs	r3, r3, r1
    3ee6:	3330      	adds	r3, #48	; 0x30
    3ee8:	1e75      	subs	r5, r6, #1
    3eea:	702b      	strb	r3, [r5, #0]
    3eec:	9a04      	ldr	r2, [sp, #16]
    3eee:	0003      	movs	r3, r0
    3ef0:	2800      	cmp	r0, #0
    3ef2:	d1ee      	bne.n	3ed2 <DiagVSprintf+0x296>
    3ef4:	0015      	movs	r5, r2
    3ef6:	465a      	mov	r2, fp
    3ef8:	0031      	movs	r1, r6
    3efa:	0020      	movs	r0, r4
    3efc:	9602      	str	r6, [sp, #8]
    3efe:	4644      	mov	r4, r8
    3f00:	9e05      	ldr	r6, [sp, #20]
    3f02:	2a00      	cmp	r2, #0
    3f04:	db0b      	blt.n	3f1e <DiagVSprintf+0x2e2>
    3f06:	0892      	lsrs	r2, r2, #2
    3f08:	1aba      	subs	r2, r7, r2
    3f0a:	3330      	adds	r3, #48	; 0x30
    3f0c:	42ba      	cmp	r2, r7
    3f0e:	d004      	beq.n	3f1a <DiagVSprintf+0x2de>
    3f10:	2f00      	cmp	r7, #0
    3f12:	dd11      	ble.n	3f38 <DiagVSprintf+0x2fc>
    3f14:	3f01      	subs	r7, #1
    3f16:	42ba      	cmp	r2, r7
    3f18:	d1fa      	bne.n	3f10 <DiagVSprintf+0x2d4>
    3f1a:	9102      	str	r1, [sp, #8]
    3f1c:	1e48      	subs	r0, r1, #1
    3f1e:	9b04      	ldr	r3, [sp, #16]
    3f20:	4283      	cmp	r3, r0
    3f22:	d300      	bcc.n	3f26 <DiagVSprintf+0x2ea>
    3f24:	e76c      	b.n	3e00 <DiagVSprintf+0x1c4>
    3f26:	7803      	ldrb	r3, [r0, #0]
    3f28:	782a      	ldrb	r2, [r5, #0]
    3f2a:	3801      	subs	r0, #1
    3f2c:	7042      	strb	r2, [r0, #1]
    3f2e:	702b      	strb	r3, [r5, #0]
    3f30:	3501      	adds	r5, #1
    3f32:	42a8      	cmp	r0, r5
    3f34:	d8f7      	bhi.n	3f26 <DiagVSprintf+0x2ea>
    3f36:	e763      	b.n	3e00 <DiagVSprintf+0x1c4>
    3f38:	700b      	strb	r3, [r1, #0]
    3f3a:	3f01      	subs	r7, #1
    3f3c:	3101      	adds	r1, #1
    3f3e:	e7ea      	b.n	3f16 <DiagVSprintf+0x2da>
    3f40:	a907      	add	r1, sp, #28
    3f42:	0008      	movs	r0, r1
    3f44:	9103      	str	r1, [sp, #12]
    3f46:	212d      	movs	r1, #45	; 0x2d
    3f48:	ad06      	add	r5, sp, #24
    3f4a:	7001      	strb	r1, [r0, #0]
    3f4c:	46ac      	mov	ip, r5
    3f4e:	2005      	movs	r0, #5
    3f50:	4460      	add	r0, ip
    3f52:	425b      	negs	r3, r3
    3f54:	392c      	subs	r1, #44	; 0x2c
    3f56:	9004      	str	r0, [sp, #16]
    3f58:	e7b2      	b.n	3ec0 <DiagVSprintf+0x284>
    3f5a:	9901      	ldr	r1, [sp, #4]
    3f5c:	2700      	movs	r7, #0
    3f5e:	3104      	adds	r1, #4
    3f60:	9101      	str	r1, [sp, #4]
    3f62:	a907      	add	r1, sp, #28
    3f64:	9103      	str	r1, [sp, #12]
    3f66:	9102      	str	r1, [sp, #8]
    3f68:	e71a      	b.n	3da0 <DiagVSprintf+0x164>
    3f6a:	2000      	movs	r0, #0
    3f6c:	e6da      	b.n	3d24 <DiagVSprintf+0xe8>
    3f6e:	46c0      	nop			; (mov r8, r8)
    3f70:	fffff000 	.word	0xfffff000
    3f74:	ffff0000 	.word	0xffff0000

00003f78 <DiagPrintf>:
    3f78:	b40f      	push	{r0, r1, r2, r3}
    3f7a:	f240 53cc 	movw	r3, #1484	; 0x5cc
    3f7e:	b500      	push	{lr}
    3f80:	f2c2 3300 	movt	r3, #8960	; 0x2300
    3f84:	681a      	ldr	r2, [r3, #0]
    3f86:	b083      	sub	sp, #12
    3f88:	2a01      	cmp	r2, #1
    3f8a:	d01a      	beq.n	3fc2 <DiagPrintf+0x4a>
    3f8c:	685a      	ldr	r2, [r3, #4]
    3f8e:	2a01      	cmp	r2, #1
    3f90:	d00a      	beq.n	3fa8 <DiagPrintf+0x30>
    3f92:	ab05      	add	r3, sp, #20
    3f94:	9301      	str	r3, [sp, #4]
    3f96:	2000      	movs	r0, #0
    3f98:	9a01      	ldr	r2, [sp, #4]
    3f9a:	9904      	ldr	r1, [sp, #16]
    3f9c:	f7ff fe4e 	bl	3c3c <DiagVSprintf>
    3fa0:	b003      	add	sp, #12
    3fa2:	bc08      	pop	{r3}
    3fa4:	b004      	add	sp, #16
    3fa6:	4718      	bx	r3
    3fa8:	689b      	ldr	r3, [r3, #8]
    3faa:	2b00      	cmp	r3, #0
    3fac:	d0f1      	beq.n	3f92 <DiagPrintf+0x1a>
    3fae:	9804      	ldr	r0, [sp, #16]
    3fb0:	4798      	blx	r3
    3fb2:	aa05      	add	r2, sp, #20
    3fb4:	9201      	str	r2, [sp, #4]
    3fb6:	2800      	cmp	r0, #0
    3fb8:	d0ed      	beq.n	3f96 <DiagPrintf+0x1e>
    3fba:	9904      	ldr	r1, [sp, #16]
    3fbc:	f7ff fe3e 	bl	3c3c <DiagVSprintf>
    3fc0:	e7ee      	b.n	3fa0 <DiagPrintf+0x28>
    3fc2:	2000      	movs	r0, #0
    3fc4:	e7ec      	b.n	3fa0 <DiagPrintf+0x28>
    3fc6:	46c0      	nop			; (mov r8, r8)

00003fc8 <DiagPrintfD>:
    3fc8:	b40f      	push	{r0, r1, r2, r3}
    3fca:	f240 53cc 	movw	r3, #1484	; 0x5cc
    3fce:	b500      	push	{lr}
    3fd0:	f2c2 3300 	movt	r3, #8960	; 0x2300
    3fd4:	681b      	ldr	r3, [r3, #0]
    3fd6:	b083      	sub	sp, #12
    3fd8:	2b01      	cmp	r3, #1
    3fda:	d009      	beq.n	3ff0 <DiagPrintfD+0x28>
    3fdc:	aa05      	add	r2, sp, #20
    3fde:	2000      	movs	r0, #0
    3fe0:	9904      	ldr	r1, [sp, #16]
    3fe2:	9201      	str	r2, [sp, #4]
    3fe4:	f7ff fe2a 	bl	3c3c <DiagVSprintf>
    3fe8:	b003      	add	sp, #12
    3fea:	bc08      	pop	{r3}
    3fec:	b004      	add	sp, #16
    3fee:	4718      	bx	r3
    3ff0:	2000      	movs	r0, #0
    3ff2:	e7f9      	b.n	3fe8 <DiagPrintfD+0x20>

00003ff4 <DiagSPrintf>:
    3ff4:	b40e      	push	{r1, r2, r3}
    3ff6:	f240 53cc 	movw	r3, #1484	; 0x5cc
    3ffa:	b500      	push	{lr}
    3ffc:	f2c2 3300 	movt	r3, #8960	; 0x2300
    4000:	681b      	ldr	r3, [r3, #0]
    4002:	b082      	sub	sp, #8
    4004:	2b01      	cmp	r3, #1
    4006:	d008      	beq.n	401a <DiagSPrintf+0x26>
    4008:	aa04      	add	r2, sp, #16
    400a:	9903      	ldr	r1, [sp, #12]
    400c:	9201      	str	r2, [sp, #4]
    400e:	f7ff fe15 	bl	3c3c <DiagVSprintf>
    4012:	b002      	add	sp, #8
    4014:	bc08      	pop	{r3}
    4016:	b003      	add	sp, #12
    4018:	4718      	bx	r3
    401a:	2000      	movs	r0, #0
    401c:	e7f9      	b.n	4012 <DiagSPrintf+0x1e>
    401e:	46c0      	nop			; (mov r8, r8)

00004020 <DiagSnPrintf>:
    4020:	b40c      	push	{r2, r3}
    4022:	b5f0      	push	{r4, r5, r6, r7, lr}
    4024:	4657      	mov	r7, sl
    4026:	46de      	mov	lr, fp
    4028:	464e      	mov	r6, r9
    402a:	4645      	mov	r5, r8
    402c:	b5e0      	push	{r5, r6, r7, lr}
    402e:	4682      	mov	sl, r0
    4030:	b08d      	sub	sp, #52	; 0x34
    4032:	2800      	cmp	r0, #0
    4034:	d05b      	beq.n	40ee <DiagSnPrintf+0xce>
    4036:	ab17      	add	r3, sp, #92	; 0x5c
    4038:	9306      	str	r3, [sp, #24]
    403a:	bb61      	cbnz	r1, 4096 <DiagSnPrintf+0x76>
    403c:	2201      	movs	r2, #1
    403e:	4252      	negs	r2, r2
    4040:	4690      	mov	r8, r2
    4042:	9d16      	ldr	r5, [sp, #88]	; 0x58
    4044:	782a      	ldrb	r2, [r5, #0]
    4046:	2a00      	cmp	r2, #0
    4048:	d100      	bne.n	404c <DiagSnPrintf+0x2c>
    404a:	e176      	b.n	433a <DiagSnPrintf+0x31a>
    404c:	f24c 2b18 	movw	fp, #49688	; 0xc218
    4050:	f240 0c0f 	movw	ip, #15
    4054:	4650      	mov	r0, sl
    4056:	4641      	mov	r1, r8
    4058:	9301      	str	r3, [sp, #4]
    405a:	f2c0 0b00 	movt	fp, #0
    405e:	2a25      	cmp	r2, #37	; 0x25
    4060:	d01c      	beq.n	409c <DiagSnPrintf+0x7c>
    4062:	1c43      	adds	r3, r0, #1
    4064:	7002      	strb	r2, [r0, #0]
    4066:	4299      	cmp	r1, r3
    4068:	d907      	bls.n	407a <DiagSnPrintf+0x5a>
    406a:	0018      	movs	r0, r3
    406c:	9c16      	ldr	r4, [sp, #88]	; 0x58
    406e:	7862      	ldrb	r2, [r4, #1]
    4070:	1c65      	adds	r5, r4, #1
    4072:	9516      	str	r5, [sp, #88]	; 0x58
    4074:	2a00      	cmp	r2, #0
    4076:	d1f2      	bne.n	405e <DiagSnPrintf+0x3e>
    4078:	0003      	movs	r3, r0
    407a:	4652      	mov	r2, sl
    407c:	1a98      	subs	r0, r3, r2
    407e:	2200      	movs	r2, #0
    4080:	701a      	strb	r2, [r3, #0]
    4082:	b00d      	add	sp, #52	; 0x34
    4084:	bcf0      	pop	{r4, r5, r6, r7}
    4086:	46bb      	mov	fp, r7
    4088:	46b2      	mov	sl, r6
    408a:	46a9      	mov	r9, r5
    408c:	46a0      	mov	r8, r4
    408e:	bcf0      	pop	{r4, r5, r6, r7}
    4090:	bc08      	pop	{r3}
    4092:	b002      	add	sp, #8
    4094:	4718      	bx	r3
    4096:	4451      	add	r1, sl
    4098:	4688      	mov	r8, r1
    409a:	e7d2      	b.n	4042 <DiagSnPrintf+0x22>
    409c:	9b01      	ldr	r3, [sp, #4]
    409e:	1c6c      	adds	r4, r5, #1
    40a0:	786f      	ldrb	r7, [r5, #1]
    40a2:	9416      	str	r4, [sp, #88]	; 0x58
    40a4:	681a      	ldr	r2, [r3, #0]
    40a6:	2f73      	cmp	r7, #115	; 0x73
    40a8:	d057      	beq.n	415a <DiagSnPrintf+0x13a>
    40aa:	2300      	movs	r3, #0
    40ac:	9302      	str	r3, [sp, #8]
    40ae:	2a0f      	cmp	r2, #15
    40b0:	dc1f      	bgt.n	40f2 <DiagSnPrintf+0xd2>
    40b2:	003b      	movs	r3, r7
    40b4:	3b30      	subs	r3, #48	; 0x30
    40b6:	2b09      	cmp	r3, #9
    40b8:	d967      	bls.n	418a <DiagSnPrintf+0x16a>
    40ba:	2f78      	cmp	r7, #120	; 0x78
    40bc:	d100      	bne.n	40c0 <DiagSnPrintf+0xa0>
    40be:	e0e1      	b.n	4284 <DiagSnPrintf+0x264>
    40c0:	2f58      	cmp	r7, #88	; 0x58
    40c2:	d100      	bne.n	40c6 <DiagSnPrintf+0xa6>
    40c4:	e13c      	b.n	4340 <DiagSnPrintf+0x320>
    40c6:	2f70      	cmp	r7, #112	; 0x70
    40c8:	d075      	beq.n	41b6 <DiagSnPrintf+0x196>
    40ca:	2f50      	cmp	r7, #80	; 0x50
    40cc:	d100      	bne.n	40d0 <DiagSnPrintf+0xb0>
    40ce:	e0e8      	b.n	42a2 <DiagSnPrintf+0x282>
    40d0:	2f64      	cmp	r7, #100	; 0x64
    40d2:	d100      	bne.n	40d6 <DiagSnPrintf+0xb6>
    40d4:	e0ec      	b.n	42b0 <DiagSnPrintf+0x290>
    40d6:	2f63      	cmp	r7, #99	; 0x63
    40d8:	d100      	bne.n	40dc <DiagSnPrintf+0xbc>
    40da:	e0db      	b.n	4294 <DiagSnPrintf+0x274>
    40dc:	ab06      	add	r3, sp, #24
    40de:	711f      	strb	r7, [r3, #4]
    40e0:	1c43      	adds	r3, r0, #1
    40e2:	7007      	strb	r7, [r0, #0]
    40e4:	428b      	cmp	r3, r1
    40e6:	d300      	bcc.n	40ea <DiagSnPrintf+0xca>
    40e8:	e0ca      	b.n	4280 <DiagSnPrintf+0x260>
    40ea:	0018      	movs	r0, r3
    40ec:	e7bf      	b.n	406e <DiagSnPrintf+0x4e>
    40ee:	2000      	movs	r0, #0
    40f0:	e7c7      	b.n	4082 <DiagSnPrintf+0x62>
    40f2:	3304      	adds	r3, #4
    40f4:	9302      	str	r3, [sp, #8]
    40f6:	0013      	movs	r3, r2
    40f8:	3b10      	subs	r3, #16
    40fa:	2bef      	cmp	r3, #239	; 0xef
    40fc:	d9d9      	bls.n	40b2 <DiagSnPrintf+0x92>
    40fe:	2608      	movs	r6, #8
    4100:	f640 68ff 	movw	r8, #3839	; 0xeff
    4104:	1e53      	subs	r3, r2, #1
    4106:	3bff      	subs	r3, #255	; 0xff
    4108:	9602      	str	r6, [sp, #8]
    410a:	4543      	cmp	r3, r8
    410c:	d9d1      	bls.n	40b2 <DiagSnPrintf+0x92>
    410e:	230c      	movs	r3, #12
    4110:	f64e 76ff 	movw	r6, #61439	; 0xefff
    4114:	9302      	str	r3, [sp, #8]
    4116:	4b96      	ldr	r3, [pc, #600]	; (4370 <DiagSnPrintf+0x350>)
    4118:	18d3      	adds	r3, r2, r3
    411a:	42b3      	cmp	r3, r6
    411c:	d9c9      	bls.n	40b2 <DiagSnPrintf+0x92>
    411e:	2310      	movs	r3, #16
    4120:	26f0      	movs	r6, #240	; 0xf0
    4122:	9302      	str	r3, [sp, #8]
    4124:	4b93      	ldr	r3, [pc, #588]	; (4374 <DiagSnPrintf+0x354>)
    4126:	0336      	lsls	r6, r6, #12
    4128:	18d3      	adds	r3, r2, r3
    412a:	42b3      	cmp	r3, r6
    412c:	d3c1      	bcc.n	40b2 <DiagSnPrintf+0x92>
    412e:	2314      	movs	r3, #20
    4130:	9302      	str	r3, [sp, #8]
    4132:	2300      	movs	r3, #0
    4134:	26f0      	movs	r6, #240	; 0xf0
    4136:	f6cf 73f0 	movt	r3, #65520	; 0xfff0
    413a:	18d3      	adds	r3, r2, r3
    413c:	0436      	lsls	r6, r6, #16
    413e:	42b3      	cmp	r3, r6
    4140:	d3b7      	bcc.n	40b2 <DiagSnPrintf+0x92>
    4142:	23ff      	movs	r3, #255	; 0xff
    4144:	26f0      	movs	r6, #240	; 0xf0
    4146:	061b      	lsls	r3, r3, #24
    4148:	0536      	lsls	r6, r6, #20
    414a:	18d3      	adds	r3, r2, r3
    414c:	42b3      	cmp	r3, r6
    414e:	419b      	sbcs	r3, r3
    4150:	2603      	movs	r6, #3
    4152:	43b3      	bics	r3, r6
    4154:	331c      	adds	r3, #28
    4156:	9302      	str	r3, [sp, #8]
    4158:	e7ab      	b.n	40b2 <DiagSnPrintf+0x92>
    415a:	7815      	ldrb	r5, [r2, #0]
    415c:	3304      	adds	r3, #4
    415e:	9301      	str	r3, [sp, #4]
    4160:	2d00      	cmp	r5, #0
    4162:	d100      	bne.n	4166 <DiagSnPrintf+0x146>
    4164:	e783      	b.n	406e <DiagSnPrintf+0x4e>
    4166:	1c43      	adds	r3, r0, #1
    4168:	7005      	strb	r5, [r0, #0]
    416a:	4299      	cmp	r1, r3
    416c:	d800      	bhi.n	4170 <DiagSnPrintf+0x150>
    416e:	e784      	b.n	407a <DiagSnPrintf+0x5a>
    4170:	1a12      	subs	r2, r2, r0
    4172:	e005      	b.n	4180 <DiagSnPrintf+0x160>
    4174:	3301      	adds	r3, #1
    4176:	1e5d      	subs	r5, r3, #1
    4178:	7028      	strb	r0, [r5, #0]
    417a:	4299      	cmp	r1, r3
    417c:	d100      	bne.n	4180 <DiagSnPrintf+0x160>
    417e:	e77c      	b.n	407a <DiagSnPrintf+0x5a>
    4180:	5c98      	ldrb	r0, [r3, r2]
    4182:	2800      	cmp	r0, #0
    4184:	d1f6      	bne.n	4174 <DiagSnPrintf+0x154>
    4186:	0018      	movs	r0, r3
    4188:	e771      	b.n	406e <DiagSnPrintf+0x4e>
    418a:	2600      	movs	r6, #0
    418c:	4690      	mov	r8, r2
    418e:	3502      	adds	r5, #2
    4190:	00b3      	lsls	r3, r6, #2
    4192:	199b      	adds	r3, r3, r6
    4194:	005b      	lsls	r3, r3, #1
    4196:	19db      	adds	r3, r3, r7
    4198:	782f      	ldrb	r7, [r5, #0]
    419a:	001e      	movs	r6, r3
    419c:	003a      	movs	r2, r7
    419e:	3a30      	subs	r2, #48	; 0x30
    41a0:	002c      	movs	r4, r5
    41a2:	3e30      	subs	r6, #48	; 0x30
    41a4:	3501      	adds	r5, #1
    41a6:	2a09      	cmp	r2, #9
    41a8:	d9f2      	bls.n	4190 <DiagSnPrintf+0x170>
    41aa:	3b31      	subs	r3, #49	; 0x31
    41ac:	009b      	lsls	r3, r3, #2
    41ae:	4642      	mov	r2, r8
    41b0:	9416      	str	r4, [sp, #88]	; 0x58
    41b2:	9302      	str	r3, [sp, #8]
    41b4:	e781      	b.n	40ba <DiagSnPrintf+0x9a>
    41b6:	9b01      	ldr	r3, [sp, #4]
    41b8:	f240 0920 	movw	r9, #32
    41bc:	3304      	adds	r3, #4
    41be:	9301      	str	r3, [sp, #4]
    41c0:	0013      	movs	r3, r2
    41c2:	2630      	movs	r6, #48	; 0x30
    41c4:	ad06      	add	r5, sp, #24
    41c6:	712e      	strb	r6, [r5, #4]
    41c8:	464e      	mov	r6, r9
    41ca:	2558      	movs	r5, #88	; 0x58
    41cc:	4335      	orrs	r5, r6
    41ce:	ae06      	add	r6, sp, #24
    41d0:	7175      	strb	r5, [r6, #5]
    41d2:	0035      	movs	r5, r6
    41d4:	3506      	adds	r5, #6
    41d6:	2700      	movs	r7, #0
    41d8:	4690      	mov	r8, r2
    41da:	2a00      	cmp	r2, #0
    41dc:	d100      	bne.n	41e0 <DiagSnPrintf+0x1c0>
    41de:	e0ba      	b.n	4356 <DiagSnPrintf+0x336>
    41e0:	4666      	mov	r6, ip
    41e2:	401e      	ands	r6, r3
    41e4:	1e72      	subs	r2, r6, #1
    41e6:	4196      	sbcs	r6, r2
    41e8:	091b      	lsrs	r3, r3, #4
    41ea:	19bf      	adds	r7, r7, r6
    41ec:	2b00      	cmp	r3, #0
    41ee:	d1f7      	bne.n	41e0 <DiagSnPrintf+0x1c0>
    41f0:	4642      	mov	r2, r8
    41f2:	3f01      	subs	r7, #1
    41f4:	00bf      	lsls	r7, r7, #2
    41f6:	9b02      	ldr	r3, [sp, #8]
    41f8:	42bb      	cmp	r3, r7
    41fa:	da00      	bge.n	41fe <DiagSnPrintf+0x1de>
    41fc:	003b      	movs	r3, r7
    41fe:	2b00      	cmp	r3, #0
    4200:	db25      	blt.n	424e <DiagSnPrintf+0x22e>
    4202:	464e      	mov	r6, r9
    4204:	b276      	sxtb	r6, r6
    4206:	46b1      	mov	r9, r6
    4208:	2603      	movs	r6, #3
    420a:	002f      	movs	r7, r5
    420c:	9503      	str	r5, [sp, #12]
    420e:	464d      	mov	r5, r9
    4210:	4681      	mov	r9, r0
    4212:	4658      	mov	r0, fp
    4214:	401e      	ands	r6, r3
    4216:	3e04      	subs	r6, #4
    4218:	468b      	mov	fp, r1
    421a:	46a0      	mov	r8, r4
    421c:	0011      	movs	r1, r2
    421e:	0004      	movs	r4, r0
    4220:	0032      	movs	r2, r6
    4222:	0028      	movs	r0, r5
    4224:	9302      	str	r3, [sp, #8]
    4226:	000e      	movs	r6, r1
    4228:	4665      	mov	r5, ip
    422a:	411e      	asrs	r6, r3
    422c:	402e      	ands	r6, r5
    422e:	5da6      	ldrb	r6, [r4, r6]
    4230:	3b04      	subs	r3, #4
    4232:	4306      	orrs	r6, r0
    4234:	703e      	strb	r6, [r7, #0]
    4236:	3701      	adds	r7, #1
    4238:	429a      	cmp	r2, r3
    423a:	d1f4      	bne.n	4226 <DiagSnPrintf+0x206>
    423c:	4659      	mov	r1, fp
    423e:	4648      	mov	r0, r9
    4240:	46a3      	mov	fp, r4
    4242:	4644      	mov	r4, r8
    4244:	9b02      	ldr	r3, [sp, #8]
    4246:	9d03      	ldr	r5, [sp, #12]
    4248:	089b      	lsrs	r3, r3, #2
    424a:	3301      	adds	r3, #1
    424c:	18ed      	adds	r5, r5, r3
    424e:	ab07      	add	r3, sp, #28
    4250:	429d      	cmp	r5, r3
    4252:	d97d      	bls.n	4350 <DiagSnPrintf+0x330>
    4254:	aa06      	add	r2, sp, #24
    4256:	7912      	ldrb	r2, [r2, #4]
    4258:	1c43      	adds	r3, r0, #1
    425a:	7002      	strb	r2, [r0, #0]
    425c:	4299      	cmp	r1, r3
    425e:	d90f      	bls.n	4280 <DiagSnPrintf+0x260>
    4260:	aa07      	add	r2, sp, #28
    4262:	3201      	adds	r2, #1
    4264:	4295      	cmp	r5, r2
    4266:	d800      	bhi.n	426a <DiagSnPrintf+0x24a>
    4268:	e73f      	b.n	40ea <DiagSnPrintf+0xca>
    426a:	7816      	ldrb	r6, [r2, #0]
    426c:	3301      	adds	r3, #1
    426e:	1e58      	subs	r0, r3, #1
    4270:	7006      	strb	r6, [r0, #0]
    4272:	4299      	cmp	r1, r3
    4274:	d1f5      	bne.n	4262 <DiagSnPrintf+0x242>
    4276:	4688      	mov	r8, r1
    4278:	4643      	mov	r3, r8
    427a:	4652      	mov	r2, sl
    427c:	1a98      	subs	r0, r3, r2
    427e:	e6fe      	b.n	407e <DiagSnPrintf+0x5e>
    4280:	4698      	mov	r8, r3
    4282:	e7f9      	b.n	4278 <DiagSnPrintf+0x258>
    4284:	9b01      	ldr	r3, [sp, #4]
    4286:	f240 0920 	movw	r9, #32
    428a:	3304      	adds	r3, #4
    428c:	9301      	str	r3, [sp, #4]
    428e:	ad07      	add	r5, sp, #28
    4290:	0013      	movs	r3, r2
    4292:	e7a0      	b.n	41d6 <DiagSnPrintf+0x1b6>
    4294:	9b01      	ldr	r3, [sp, #4]
    4296:	b2d7      	uxtb	r7, r2
    4298:	3304      	adds	r3, #4
    429a:	9301      	str	r3, [sp, #4]
    429c:	ab06      	add	r3, sp, #24
    429e:	711f      	strb	r7, [r3, #4]
    42a0:	e71e      	b.n	40e0 <DiagSnPrintf+0xc0>
    42a2:	9b01      	ldr	r3, [sp, #4]
    42a4:	f240 0900 	movw	r9, #0
    42a8:	3304      	adds	r3, #4
    42aa:	9301      	str	r3, [sp, #4]
    42ac:	0013      	movs	r3, r2
    42ae:	e788      	b.n	41c2 <DiagSnPrintf+0x1a2>
    42b0:	9b01      	ldr	r3, [sp, #4]
    42b2:	3304      	adds	r3, #4
    42b4:	9301      	str	r3, [sp, #4]
    42b6:	2a00      	cmp	r2, #0
    42b8:	db4f      	blt.n	435a <DiagSnPrintf+0x33a>
    42ba:	2300      	movs	r3, #0
    42bc:	ad07      	add	r5, sp, #28
    42be:	9503      	str	r5, [sp, #12]
    42c0:	9e03      	ldr	r6, [sp, #12]
    42c2:	4680      	mov	r8, r0
    42c4:	0035      	movs	r5, r6
    42c6:	4689      	mov	r9, r1
    42c8:	1b9b      	subs	r3, r3, r6
    42ca:	9304      	str	r3, [sp, #16]
    42cc:	9405      	str	r4, [sp, #20]
    42ce:	9b04      	ldr	r3, [sp, #16]
    42d0:	0028      	movs	r0, r5
    42d2:	18ef      	adds	r7, r5, r3
    42d4:	230a      	movs	r3, #10
    42d6:	fb92 f4f3 	sdiv	r4, r2, r3
    42da:	00a3      	lsls	r3, r4, #2
    42dc:	191b      	adds	r3, r3, r4
    42de:	005b      	lsls	r3, r3, #1
    42e0:	1ad3      	subs	r3, r2, r3
    42e2:	3501      	adds	r5, #1
    42e4:	1e6e      	subs	r6, r5, #1
    42e6:	3330      	adds	r3, #48	; 0x30
    42e8:	0022      	movs	r2, r4
    42ea:	9903      	ldr	r1, [sp, #12]
    42ec:	7033      	strb	r3, [r6, #0]
    42ee:	2c00      	cmp	r4, #0
    42f0:	d1ed      	bne.n	42ce <DiagSnPrintf+0x2ae>
    42f2:	9b02      	ldr	r3, [sp, #8]
    42f4:	0006      	movs	r6, r0
    42f6:	9c05      	ldr	r4, [sp, #20]
    42f8:	4640      	mov	r0, r8
    42fa:	46b8      	mov	r8, r7
    42fc:	000f      	movs	r7, r1
    42fe:	4649      	mov	r1, r9
    4300:	2b00      	cmp	r3, #0
    4302:	db0e      	blt.n	4322 <DiagSnPrintf+0x302>
    4304:	4642      	mov	r2, r8
    4306:	089b      	lsrs	r3, r3, #2
    4308:	1ad3      	subs	r3, r2, r3
    430a:	429a      	cmp	r2, r3
    430c:	d008      	beq.n	4320 <DiagSnPrintf+0x300>
    430e:	2a00      	cmp	r2, #0
    4310:	dd01      	ble.n	4316 <DiagSnPrintf+0x2f6>
    4312:	3a01      	subs	r2, #1
    4314:	e7f9      	b.n	430a <DiagSnPrintf+0x2ea>
    4316:	2630      	movs	r6, #48	; 0x30
    4318:	3a01      	subs	r2, #1
    431a:	702e      	strb	r6, [r5, #0]
    431c:	3501      	adds	r5, #1
    431e:	e7f4      	b.n	430a <DiagSnPrintf+0x2ea>
    4320:	1e6e      	subs	r6, r5, #1
    4322:	9b03      	ldr	r3, [sp, #12]
    4324:	42b3      	cmp	r3, r6
    4326:	d292      	bcs.n	424e <DiagSnPrintf+0x22e>
    4328:	7833      	ldrb	r3, [r6, #0]
    432a:	783a      	ldrb	r2, [r7, #0]
    432c:	3e01      	subs	r6, #1
    432e:	7072      	strb	r2, [r6, #1]
    4330:	703b      	strb	r3, [r7, #0]
    4332:	3701      	adds	r7, #1
    4334:	42be      	cmp	r6, r7
    4336:	d8f7      	bhi.n	4328 <DiagSnPrintf+0x308>
    4338:	e789      	b.n	424e <DiagSnPrintf+0x22e>
    433a:	4653      	mov	r3, sl
    433c:	2000      	movs	r0, #0
    433e:	e69e      	b.n	407e <DiagSnPrintf+0x5e>
    4340:	9b01      	ldr	r3, [sp, #4]
    4342:	f240 0900 	movw	r9, #0
    4346:	3304      	adds	r3, #4
    4348:	9301      	str	r3, [sp, #4]
    434a:	ad07      	add	r5, sp, #28
    434c:	0013      	movs	r3, r2
    434e:	e742      	b.n	41d6 <DiagSnPrintf+0x1b6>
    4350:	0003      	movs	r3, r0
    4352:	0018      	movs	r0, r3
    4354:	e68b      	b.n	406e <DiagSnPrintf+0x4e>
    4356:	3f04      	subs	r7, #4
    4358:	e74d      	b.n	41f6 <DiagSnPrintf+0x1d6>
    435a:	232d      	movs	r3, #45	; 0x2d
    435c:	ad06      	add	r5, sp, #24
    435e:	ae06      	add	r6, sp, #24
    4360:	712b      	strb	r3, [r5, #4]
    4362:	46b0      	mov	r8, r6
    4364:	2505      	movs	r5, #5
    4366:	4445      	add	r5, r8
    4368:	4252      	negs	r2, r2
    436a:	3b2c      	subs	r3, #44	; 0x2c
    436c:	9503      	str	r5, [sp, #12]
    436e:	e7a7      	b.n	42c0 <DiagSnPrintf+0x2a0>
    4370:	fffff000 	.word	0xfffff000
    4374:	ffff0000 	.word	0xffff0000

00004378 <cmd_reboot>:
    4378:	b570      	push	{r4, r5, r6, lr}
    437a:	000c      	movs	r4, r1
    437c:	4d10      	ldr	r5, [pc, #64]	; (43c0 <cmd_reboot+0x48>)
    437e:	4911      	ldr	r1, [pc, #68]	; (43c4 <cmd_reboot+0x4c>)
    4380:	6820      	ldr	r0, [r4, #0]
    4382:	47a8      	blx	r5
    4384:	b998      	cbnz	r0, 43ae <cmd_reboot+0x36>
    4386:	f240 2100 	movw	r1, #512	; 0x200
    438a:	4b0f      	ldr	r3, [pc, #60]	; (43c8 <cmd_reboot+0x50>)
    438c:	4798      	blx	r3
    438e:	4b0f      	ldr	r3, [pc, #60]	; (43cc <cmd_reboot+0x54>)
    4390:	681b      	ldr	r3, [r3, #0]
    4392:	03db      	lsls	r3, r3, #15
    4394:	d502      	bpl.n	439c <cmd_reboot+0x24>
    4396:	480e      	ldr	r0, [pc, #56]	; (43d0 <cmd_reboot+0x58>)
    4398:	f7ff fdee 	bl	3f78 <DiagPrintf>
    439c:	f3bf 8f4f 	dsb	sy
    43a0:	4b0c      	ldr	r3, [pc, #48]	; (43d4 <cmd_reboot+0x5c>)
    43a2:	4a0d      	ldr	r2, [pc, #52]	; (43d8 <cmd_reboot+0x60>)
    43a4:	60da      	str	r2, [r3, #12]
    43a6:	f3bf 8f4f 	dsb	sy
    43aa:	46c0      	nop			; (mov r8, r8)
    43ac:	e7fd      	b.n	43aa <cmd_reboot+0x32>
    43ae:	490b      	ldr	r1, [pc, #44]	; (43dc <cmd_reboot+0x64>)
    43b0:	6820      	ldr	r0, [r4, #0]
    43b2:	47a8      	blx	r5
    43b4:	2800      	cmp	r0, #0
    43b6:	d1ea      	bne.n	438e <cmd_reboot+0x16>
    43b8:	f240 6100 	movw	r1, #1536	; 0x600
    43bc:	e7e5      	b.n	438a <cmd_reboot+0x12>
    43be:	46c0      	nop			; (mov r8, r8)
    43c0:	00004d49 	.word	0x00004d49
    43c4:	0000c229 	.word	0x0000c229
    43c8:	00000179 	.word	0x00000179
    43cc:	230005d8 	.word	0x230005d8
    43d0:	0000c23f 	.word	0x0000c23f
    43d4:	e000ed00 	.word	0xe000ed00
    43d8:	05fa0004 	.word	0x05fa0004
    43dc:	0000c232 	.word	0x0000c232

000043e0 <cmd_rom_help>:
    43e0:	b570      	push	{r4, r5, r6, lr}
    43e2:	2680      	movs	r6, #128	; 0x80
    43e4:	4c10      	ldr	r4, [pc, #64]	; (4428 <cmd_rom_help+0x48>)
    43e6:	0276      	lsls	r6, r6, #9
    43e8:	6823      	ldr	r3, [r4, #0]
    43ea:	4233      	tst	r3, r6
    43ec:	d002      	beq.n	43f4 <cmd_rom_help+0x14>
    43ee:	480f      	ldr	r0, [pc, #60]	; (442c <cmd_rom_help+0x4c>)
    43f0:	f7ff fdc2 	bl	3f78 <DiagPrintf>
    43f4:	4d0e      	ldr	r5, [pc, #56]	; (4430 <cmd_rom_help+0x50>)
    43f6:	68e9      	ldr	r1, [r5, #12]
    43f8:	b129      	cbz	r1, 4406 <cmd_rom_help+0x26>
    43fa:	6823      	ldr	r3, [r4, #0]
    43fc:	4233      	tst	r3, r6
    43fe:	d002      	beq.n	4406 <cmd_rom_help+0x26>
    4400:	480c      	ldr	r0, [pc, #48]	; (4434 <cmd_rom_help+0x54>)
    4402:	f7ff fdb9 	bl	3f78 <DiagPrintf>
    4406:	69e9      	ldr	r1, [r5, #28]
    4408:	b129      	cbz	r1, 4416 <cmd_rom_help+0x36>
    440a:	6823      	ldr	r3, [r4, #0]
    440c:	03db      	lsls	r3, r3, #15
    440e:	d508      	bpl.n	4422 <cmd_rom_help+0x42>
    4410:	4808      	ldr	r0, [pc, #32]	; (4434 <cmd_rom_help+0x54>)
    4412:	f7ff fdb1 	bl	3f78 <DiagPrintf>
    4416:	6823      	ldr	r3, [r4, #0]
    4418:	03db      	lsls	r3, r3, #15
    441a:	d502      	bpl.n	4422 <cmd_rom_help+0x42>
    441c:	4806      	ldr	r0, [pc, #24]	; (4438 <cmd_rom_help+0x58>)
    441e:	f7ff fdab 	bl	3f78 <DiagPrintf>
    4422:	2001      	movs	r0, #1
    4424:	bd70      	pop	{r4, r5, r6, pc}
    4426:	46c0      	nop			; (mov r8, r8)
    4428:	230005d8 	.word	0x230005d8
    442c:	0000c251 	.word	0x0000c251
    4430:	0000c0b0 	.word	0x0000c0b0
    4434:	0000c289 	.word	0x0000c289
    4438:	0000c28d 	.word	0x0000c28d

0000443c <cmd_rom_table>:
    443c:	4b01      	ldr	r3, [pc, #4]	; (4444 <cmd_rom_table+0x8>)
    443e:	6003      	str	r3, [r0, #0]
    4440:	2002      	movs	r0, #2
    4442:	4770      	bx	lr
    4444:	0000c0b0 	.word	0x0000c0b0

00004448 <_rand>:
    4448:	f249 0100 	movw	r1, #36864	; 0x9000
    444c:	2001      	movs	r0, #1
    444e:	f2c4 1100 	movt	r1, #16640	; 0x4100
    4452:	2301      	movs	r3, #1
    4454:	f2c0 0301 	movt	r3, #1
    4458:	e002      	b.n	4460 <_rand+0x18>
    445a:	b952      	cbnz	r2, 4472 <_rand+0x2a>
    445c:	3b01      	subs	r3, #1
    445e:	b133      	cbz	r3, 446e <_rand+0x26>
    4460:	68ca      	ldr	r2, [r1, #12]
    4462:	2a0f      	cmp	r2, #15
    4464:	d1f9      	bne.n	445a <_rand+0x12>
    4466:	3b01      	subs	r3, #1
    4468:	6388      	str	r0, [r1, #56]	; 0x38
    446a:	2b00      	cmp	r3, #0
    446c:	d1f8      	bne.n	4460 <_rand+0x18>
    446e:	6388      	str	r0, [r1, #56]	; 0x38
    4470:	e7ef      	b.n	4452 <_rand+0xa>
    4472:	6908      	ldr	r0, [r1, #16]
    4474:	4770      	bx	lr
    4476:	46c0      	nop			; (mov r8, r8)

00004478 <__wrap_rand>:
    4478:	f249 0100 	movw	r1, #36864	; 0x9000
    447c:	2001      	movs	r0, #1
    447e:	f2c4 1100 	movt	r1, #16640	; 0x4100
    4482:	2301      	movs	r3, #1
    4484:	f2c0 0301 	movt	r3, #1
    4488:	e002      	b.n	4490 <__wrap_rand+0x18>
    448a:	b952      	cbnz	r2, 44a2 <__wrap_rand+0x2a>
    448c:	3b01      	subs	r3, #1
    448e:	b133      	cbz	r3, 449e <__wrap_rand+0x26>
    4490:	68ca      	ldr	r2, [r1, #12]
    4492:	2a0f      	cmp	r2, #15
    4494:	d1f9      	bne.n	448a <__wrap_rand+0x12>
    4496:	3b01      	subs	r3, #1
    4498:	6388      	str	r0, [r1, #56]	; 0x38
    449a:	2b00      	cmp	r3, #0
    449c:	d1f8      	bne.n	4490 <__wrap_rand+0x18>
    449e:	6388      	str	r0, [r1, #56]	; 0x38
    44a0:	e7ef      	b.n	4482 <__wrap_rand+0xa>
    44a2:	6908      	ldr	r0, [r1, #16]
    44a4:	4770      	bx	lr
    44a6:	46c0      	nop			; (mov r8, r8)

000044a8 <RandBytes_Get>:
    44a8:	b5f0      	push	{r4, r5, r6, r7, lr}
    44aa:	0007      	movs	r7, r0
    44ac:	f249 0000 	movw	r0, #36864	; 0x9000
    44b0:	f2c4 1000 	movt	r0, #16640	; 0x4100
    44b4:	68c5      	ldr	r5, [r0, #12]
    44b6:	b083      	sub	sp, #12
    44b8:	088e      	lsrs	r6, r1, #2
    44ba:	2d0f      	cmp	r5, #15
    44bc:	d063      	beq.n	4586 <RandBytes_Get+0xde>
    44be:	2e00      	cmp	r6, #0
    44c0:	d100      	bne.n	44c4 <RandBytes_Get+0x1c>
    44c2:	e072      	b.n	45aa <RandBytes_Get+0x102>
    44c4:	b1a5      	cbz	r5, 44f0 <RandBytes_Get+0x48>
    44c6:	1879      	adds	r1, r7, r1
    44c8:	003b      	movs	r3, r7
    44ca:	2400      	movs	r4, #0
    44cc:	3904      	subs	r1, #4
    44ce:	e002      	b.n	44d6 <RandBytes_Get+0x2e>
    44d0:	3304      	adds	r3, #4
    44d2:	42a5      	cmp	r5, r4
    44d4:	d00e      	beq.n	44f4 <RandBytes_Get+0x4c>
    44d6:	6902      	ldr	r2, [r0, #16]
    44d8:	3401      	adds	r4, #1
    44da:	601a      	str	r2, [r3, #0]
    44dc:	1aca      	subs	r2, r1, r3
    44de:	4694      	mov	ip, r2
    44e0:	42a6      	cmp	r6, r4
    44e2:	d1f5      	bne.n	44d0 <RandBytes_Get+0x28>
    44e4:	4663      	mov	r3, ip
    44e6:	2001      	movs	r0, #1
    44e8:	2b00      	cmp	r3, #0
    44ea:	d12f      	bne.n	454c <RandBytes_Get+0xa4>
    44ec:	b003      	add	sp, #12
    44ee:	bdf0      	pop	{r4, r5, r6, r7, pc}
    44f0:	468c      	mov	ip, r1
    44f2:	2400      	movs	r4, #0
    44f4:	f249 0000 	movw	r0, #36864	; 0x9000
    44f8:	0025      	movs	r5, r4
    44fa:	2201      	movs	r2, #1
    44fc:	f2c4 1000 	movt	r0, #16640	; 0x4100
    4500:	2301      	movs	r3, #1
    4502:	f2c0 0301 	movt	r3, #1
    4506:	e002      	b.n	450e <RandBytes_Get+0x66>
    4508:	b951      	cbnz	r1, 4520 <RandBytes_Get+0x78>
    450a:	3b01      	subs	r3, #1
    450c:	b133      	cbz	r3, 451c <RandBytes_Get+0x74>
    450e:	68c1      	ldr	r1, [r0, #12]
    4510:	290f      	cmp	r1, #15
    4512:	d1f9      	bne.n	4508 <RandBytes_Get+0x60>
    4514:	3b01      	subs	r3, #1
    4516:	6382      	str	r2, [r0, #56]	; 0x38
    4518:	2b00      	cmp	r3, #0
    451a:	d1f8      	bne.n	450e <RandBytes_Get+0x66>
    451c:	6382      	str	r2, [r0, #56]	; 0x38
    451e:	e7ef      	b.n	4500 <RandBytes_Get+0x58>
    4520:	6901      	ldr	r1, [r0, #16]
    4522:	00ab      	lsls	r3, r5, #2
    4524:	3501      	adds	r5, #1
    4526:	50f9      	str	r1, [r7, r3]
    4528:	42ae      	cmp	r6, r5
    452a:	dce9      	bgt.n	4500 <RandBytes_Get+0x58>
    452c:	4661      	mov	r1, ip
    452e:	2200      	movs	r2, #0
    4530:	3904      	subs	r1, #4
    4532:	42a6      	cmp	r6, r4
    4534:	dc2d      	bgt.n	4592 <RandBytes_Get+0xea>
    4536:	4694      	mov	ip, r2
    4538:	2300      	movs	r3, #0
    453a:	448c      	add	ip, r1
    453c:	1c62      	adds	r2, r4, #1
    453e:	42a6      	cmp	r6, r4
    4540:	dc23      	bgt.n	458a <RandBytes_Get+0xe2>
    4542:	18d4      	adds	r4, r2, r3
    4544:	4663      	mov	r3, ip
    4546:	2001      	movs	r0, #1
    4548:	2b00      	cmp	r3, #0
    454a:	d0cf      	beq.n	44ec <RandBytes_Get+0x44>
    454c:	f249 0200 	movw	r2, #36864	; 0x9000
    4550:	f2c4 1200 	movt	r2, #16640	; 0x4100
    4554:	2301      	movs	r3, #1
    4556:	f2c0 0301 	movt	r3, #1
    455a:	e002      	b.n	4562 <RandBytes_Get+0xba>
    455c:	b931      	cbnz	r1, 456c <RandBytes_Get+0xc4>
    455e:	3b01      	subs	r3, #1
    4560:	b30b      	cbz	r3, 45a6 <RandBytes_Get+0xfe>
    4562:	68d1      	ldr	r1, [r2, #12]
    4564:	290f      	cmp	r1, #15
    4566:	d1f9      	bne.n	455c <RandBytes_Get+0xb4>
    4568:	6390      	str	r0, [r2, #56]	; 0x38
    456a:	e7f8      	b.n	455e <RandBytes_Get+0xb6>
    456c:	6913      	ldr	r3, [r2, #16]
    456e:	00a0      	lsls	r0, r4, #2
    4570:	9301      	str	r3, [sp, #4]
    4572:	f644 335d 	movw	r3, #19293	; 0x4b5d
    4576:	4662      	mov	r2, ip
    4578:	1838      	adds	r0, r7, r0
    457a:	a901      	add	r1, sp, #4
    457c:	f2c0 0300 	movt	r3, #0
    4580:	4798      	blx	r3
    4582:	2001      	movs	r0, #1
    4584:	e7b2      	b.n	44ec <RandBytes_Get+0x44>
    4586:	2000      	movs	r0, #0
    4588:	e7b0      	b.n	44ec <RandBytes_Get+0x44>
    458a:	1e73      	subs	r3, r6, #1
    458c:	1b1b      	subs	r3, r3, r4
    458e:	18d4      	adds	r4, r2, r3
    4590:	e7d8      	b.n	4544 <RandBytes_Get+0x9c>
    4592:	1ba2      	subs	r2, r4, r6
    4594:	0092      	lsls	r2, r2, #2
    4596:	3204      	adds	r2, #4
    4598:	4694      	mov	ip, r2
    459a:	2300      	movs	r3, #0
    459c:	448c      	add	ip, r1
    459e:	1c62      	adds	r2, r4, #1
    45a0:	42a6      	cmp	r6, r4
    45a2:	ddce      	ble.n	4542 <RandBytes_Get+0x9a>
    45a4:	e7f1      	b.n	458a <RandBytes_Get+0xe2>
    45a6:	6390      	str	r0, [r2, #56]	; 0x38
    45a8:	e7d4      	b.n	4554 <RandBytes_Get+0xac>
    45aa:	468c      	mov	ip, r1
    45ac:	2400      	movs	r4, #0
    45ae:	e799      	b.n	44e4 <RandBytes_Get+0x3c>

000045b0 <shell_array_init>:
    45b0:	1841      	adds	r1, r0, r1
    45b2:	4288      	cmp	r0, r1
    45b4:	d100      	bne.n	45b8 <shell_array_init+0x8>
    45b6:	4770      	bx	lr
    45b8:	7002      	strb	r2, [r0, #0]
    45ba:	3001      	adds	r0, #1
    45bc:	e7f9      	b.n	45b2 <shell_array_init+0x2>

000045be <shell_get_argc>:
    45be:	2300      	movs	r3, #0
    45c0:	2120      	movs	r1, #32
    45c2:	7802      	ldrb	r2, [r0, #0]
    45c4:	b92a      	cbnz	r2, 45d2 <shell_get_argc+0x14>
    45c6:	1c18      	adds	r0, r3, #0
    45c8:	2b0f      	cmp	r3, #15
    45ca:	d900      	bls.n	45ce <shell_get_argc+0x10>
    45cc:	200f      	movs	r0, #15
    45ce:	b2c0      	uxtb	r0, r0
    45d0:	4770      	bx	lr
    45d2:	2a20      	cmp	r2, #32
    45d4:	d007      	beq.n	45e6 <shell_get_argc+0x28>
    45d6:	7802      	ldrb	r2, [r0, #0]
    45d8:	438a      	bics	r2, r1
    45da:	b912      	cbnz	r2, 45e2 <shell_get_argc+0x24>
    45dc:	3301      	adds	r3, #1
    45de:	b29b      	uxth	r3, r3
    45e0:	e7ef      	b.n	45c2 <shell_get_argc+0x4>
    45e2:	3001      	adds	r0, #1
    45e4:	e7f7      	b.n	45d6 <shell_get_argc+0x18>
    45e6:	3001      	adds	r0, #1
    45e8:	e7eb      	b.n	45c2 <shell_get_argc+0x4>

000045ea <shell_get_argv>:
    45ea:	2140      	movs	r1, #64	; 0x40
    45ec:	b570      	push	{r4, r5, r6, lr}
    45ee:	2200      	movs	r2, #0
    45f0:	0004      	movs	r4, r0
    45f2:	480e      	ldr	r0, [pc, #56]	; (462c <shell_get_argv+0x42>)
    45f4:	f7ff ffdc 	bl	45b0 <shell_array_init>
    45f8:	2310      	movs	r3, #16
    45fa:	2020      	movs	r0, #32
    45fc:	490b      	ldr	r1, [pc, #44]	; (462c <shell_get_argv+0x42>)
    45fe:	7822      	ldrb	r2, [r4, #0]
    4600:	b17a      	cbz	r2, 4622 <shell_get_argv+0x38>
    4602:	600c      	str	r4, [r1, #0]
    4604:	7822      	ldrb	r2, [r4, #0]
    4606:	0025      	movs	r5, r4
    4608:	4382      	bics	r2, r0
    460a:	3401      	adds	r4, #1
    460c:	2a00      	cmp	r2, #0
    460e:	d1f9      	bne.n	4604 <shell_get_argv+0x1a>
    4610:	702a      	strb	r2, [r5, #0]
    4612:	7822      	ldrb	r2, [r4, #0]
    4614:	2a20      	cmp	r2, #32
    4616:	d006      	beq.n	4626 <shell_get_argv+0x3c>
    4618:	3b01      	subs	r3, #1
    461a:	b2db      	uxtb	r3, r3
    461c:	3104      	adds	r1, #4
    461e:	2b00      	cmp	r3, #0
    4620:	d1ed      	bne.n	45fe <shell_get_argv+0x14>
    4622:	4802      	ldr	r0, [pc, #8]	; (462c <shell_get_argv+0x42>)
    4624:	bd70      	pop	{r4, r5, r6, pc}
    4626:	3401      	adds	r4, #1
    4628:	e7f3      	b.n	4612 <shell_get_argv+0x28>
    462a:	46c0      	nop			; (mov r8, r8)
    462c:	230005e8 	.word	0x230005e8

00004630 <shell_cmd_exec_rom.constprop.0>:
    4630:	b5f0      	push	{r4, r5, r6, r7, lr}
    4632:	4d1d      	ldr	r5, [pc, #116]	; (46a8 <shell_cmd_exec_rom.constprop.0+0x78>)
    4634:	b087      	sub	sp, #28
    4636:	6cab      	ldr	r3, [r5, #72]	; 0x48
    4638:	9301      	str	r3, [sp, #4]
    463a:	6d2b      	ldr	r3, [r5, #80]	; 0x50
    463c:	9304      	str	r3, [sp, #16]
    463e:	9b01      	ldr	r3, [sp, #4]
    4640:	1c5f      	adds	r7, r3, #1
    4642:	0038      	movs	r0, r7
    4644:	f7ff ffbb 	bl	45be <shell_get_argc>
    4648:	9003      	str	r0, [sp, #12]
    464a:	0038      	movs	r0, r7
    464c:	f7ff ffcd 	bl	45ea <shell_get_argv>
    4650:	9b03      	ldr	r3, [sp, #12]
    4652:	0004      	movs	r4, r0
    4654:	b1e3      	cbz	r3, 4690 <shell_cmd_exec_rom.constprop.0+0x60>
    4656:	6d6b      	ldr	r3, [r5, #84]	; 0x54
    4658:	6800      	ldr	r0, [r0, #0]
    465a:	9305      	str	r3, [sp, #20]
    465c:	4b13      	ldr	r3, [pc, #76]	; (46ac <shell_cmd_exec_rom.constprop.0+0x7c>)
    465e:	4798      	blx	r3
    4660:	2500      	movs	r5, #0
    4662:	9b05      	ldr	r3, [sp, #20]
    4664:	42ab      	cmp	r3, r5
    4666:	d913      	bls.n	4690 <shell_cmd_exec_rom.constprop.0+0x60>
    4668:	9b04      	ldr	r3, [sp, #16]
    466a:	012e      	lsls	r6, r5, #4
    466c:	199e      	adds	r6, r3, r6
    466e:	6831      	ldr	r1, [r6, #0]
    4670:	6820      	ldr	r0, [r4, #0]
    4672:	4b0f      	ldr	r3, [pc, #60]	; (46b0 <shell_cmd_exec_rom.constprop.0+0x80>)
    4674:	4798      	blx	r3
    4676:	9002      	str	r0, [sp, #8]
    4678:	b998      	cbnz	r0, 46a2 <shell_cmd_exec_rom.constprop.0+0x72>
    467a:	9803      	ldr	r0, [sp, #12]
    467c:	1d21      	adds	r1, r4, #4
    467e:	3801      	subs	r0, #1
    4680:	68b3      	ldr	r3, [r6, #8]
    4682:	b280      	uxth	r0, r0
    4684:	4798      	blx	r3
    4686:	2104      	movs	r1, #4
    4688:	9a02      	ldr	r2, [sp, #8]
    468a:	6820      	ldr	r0, [r4, #0]
    468c:	f7ff ff90 	bl	45b0 <shell_array_init>
    4690:	2200      	movs	r2, #0
    4692:	9b01      	ldr	r3, [sp, #4]
    4694:	217f      	movs	r1, #127	; 0x7f
    4696:	0038      	movs	r0, r7
    4698:	701a      	strb	r2, [r3, #0]
    469a:	f7ff ff89 	bl	45b0 <shell_array_init>
    469e:	b007      	add	sp, #28
    46a0:	bdf0      	pop	{r4, r5, r6, r7, pc}
    46a2:	3501      	adds	r5, #1
    46a4:	b2ed      	uxtb	r5, r5
    46a6:	e7dc      	b.n	4662 <shell_cmd_exec_rom.constprop.0+0x32>
    46a8:	230005e8 	.word	0x230005e8
    46ac:	00004f79 	.word	0x00004f79
    46b0:	00004d49 	.word	0x00004d49

000046b4 <shell_cmd_chk>:
    46b4:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    46b6:	000f      	movs	r7, r1
    46b8:	68ce      	ldr	r6, [r1, #12]
    46ba:	6889      	ldr	r1, [r1, #8]
    46bc:	0004      	movs	r4, r0
    46be:	780b      	ldrb	r3, [r1, #0]
    46c0:	2b7d      	cmp	r3, #125	; 0x7d
    46c2:	d914      	bls.n	46ee <shell_cmd_chk+0x3a>
    46c4:	2500      	movs	r5, #0
    46c6:	2b7e      	cmp	r3, #126	; 0x7e
    46c8:	d121      	bne.n	470e <shell_cmd_chk+0x5a>
    46ca:	280d      	cmp	r0, #13
    46cc:	d016      	beq.n	46fc <shell_cmd_chk+0x48>
    46ce:	b1a8      	cbz	r0, 46fc <shell_cmd_chk+0x48>
    46d0:	2808      	cmp	r0, #8
    46d2:	d001      	beq.n	46d8 <shell_cmd_chk+0x24>
    46d4:	287f      	cmp	r0, #127	; 0x7f
    46d6:	d13a      	bne.n	474e <shell_cmd_chk+0x9a>
    46d8:	3b01      	subs	r3, #1
    46da:	700b      	strb	r3, [r1, #0]
    46dc:	4820      	ldr	r0, [pc, #128]	; (4760 <shell_cmd_chk+0xac>)
    46de:	2108      	movs	r1, #8
    46e0:	47b0      	blx	r6
    46e2:	4820      	ldr	r0, [pc, #128]	; (4764 <shell_cmd_chk+0xb0>)
    46e4:	47b0      	blx	r6
    46e6:	2108      	movs	r1, #8
    46e8:	481d      	ldr	r0, [pc, #116]	; (4760 <shell_cmd_chk+0xac>)
    46ea:	47b0      	blx	r6
    46ec:	e001      	b.n	46f2 <shell_cmd_chk+0x3e>
    46ee:	280a      	cmp	r0, #10
    46f0:	d101      	bne.n	46f6 <shell_cmd_chk+0x42>
    46f2:	2501      	movs	r5, #1
    46f4:	e00b      	b.n	470e <shell_cmd_chk+0x5a>
    46f6:	280d      	cmp	r0, #13
    46f8:	d000      	beq.n	46fc <shell_cmd_chk+0x48>
    46fa:	b950      	cbnz	r0, 4712 <shell_cmd_chk+0x5e>
    46fc:	18cb      	adds	r3, r1, r3
    46fe:	2001      	movs	r0, #1
    4700:	2100      	movs	r1, #0
    4702:	61b8      	str	r0, [r7, #24]
    4704:	2502      	movs	r5, #2
    4706:	7059      	strb	r1, [r3, #1]
    4708:	b10a      	cbz	r2, 470e <shell_cmd_chk+0x5a>
    470a:	4817      	ldr	r0, [pc, #92]	; (4768 <shell_cmd_chk+0xb4>)
    470c:	47b0      	blx	r6
    470e:	0028      	movs	r0, r5
    4710:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    4712:	2808      	cmp	r0, #8
    4714:	d001      	beq.n	471a <shell_cmd_chk+0x66>
    4716:	287f      	cmp	r0, #127	; 0x7f
    4718:	d102      	bne.n	4720 <shell_cmd_chk+0x6c>
    471a:	2b00      	cmp	r3, #0
    471c:	d0e9      	beq.n	46f2 <shell_cmd_chk+0x3e>
    471e:	e7db      	b.n	46d8 <shell_cmd_chk+0x24>
    4720:	2809      	cmp	r0, #9
    4722:	d109      	bne.n	4738 <shell_cmd_chk+0x84>
    4724:	18c8      	adds	r0, r1, r3
    4726:	3417      	adds	r4, #23
    4728:	3301      	adds	r3, #1
    472a:	7044      	strb	r4, [r0, #1]
    472c:	700b      	strb	r3, [r1, #0]
    472e:	2a00      	cmp	r2, #0
    4730:	d0df      	beq.n	46f2 <shell_cmd_chk+0x3e>
    4732:	480c      	ldr	r0, [pc, #48]	; (4764 <shell_cmd_chk+0xb0>)
    4734:	47b0      	blx	r6
    4736:	e7dc      	b.n	46f2 <shell_cmd_chk+0x3e>
    4738:	b112      	cbz	r2, 4740 <shell_cmd_chk+0x8c>
    473a:	0001      	movs	r1, r0
    473c:	4808      	ldr	r0, [pc, #32]	; (4760 <shell_cmd_chk+0xac>)
    473e:	47b0      	blx	r6
    4740:	68ba      	ldr	r2, [r7, #8]
    4742:	7813      	ldrb	r3, [r2, #0]
    4744:	18d1      	adds	r1, r2, r3
    4746:	3301      	adds	r3, #1
    4748:	704c      	strb	r4, [r1, #1]
    474a:	7013      	strb	r3, [r2, #0]
    474c:	e7d1      	b.n	46f2 <shell_cmd_chk+0x3e>
    474e:	4807      	ldr	r0, [pc, #28]	; (476c <shell_cmd_chk+0xb8>)
    4750:	47b0      	blx	r6
    4752:	68b8      	ldr	r0, [r7, #8]
    4754:	7803      	ldrb	r3, [r0, #0]
    4756:	18c3      	adds	r3, r0, r3
    4758:	705d      	strb	r5, [r3, #1]
    475a:	3001      	adds	r0, #1
    475c:	e7ea      	b.n	4734 <shell_cmd_chk+0x80>
    475e:	46c0      	nop			; (mov r8, r8)
    4760:	0000c334 	.word	0x0000c334
    4764:	0000c337 	.word	0x0000c337
    4768:	0000c331 	.word	0x0000c331
    476c:	0000c339 	.word	0x0000c339

00004770 <shell_uart_irq_rom>:
    4770:	b5f0      	push	{r4, r5, r6, r7, lr}
    4772:	4b3d      	ldr	r3, [pc, #244]	; (4868 <shell_uart_irq_rom+0xf8>)
    4774:	b085      	sub	sp, #20
    4776:	483d      	ldr	r0, [pc, #244]	; (486c <shell_uart_irq_rom+0xfc>)
    4778:	4798      	blx	r3
    477a:	07c3      	lsls	r3, r0, #31
    477c:	d545      	bpl.n	480a <shell_uart_irq_rom+0x9a>
    477e:	2400      	movs	r4, #0
    4780:	4d3b      	ldr	r5, [pc, #236]	; (4870 <shell_uart_irq_rom+0x100>)
    4782:	4b3c      	ldr	r3, [pc, #240]	; (4874 <shell_uart_irq_rom+0x104>)
    4784:	702c      	strb	r4, [r5, #0]
    4786:	4798      	blx	r3
    4788:	4e3b      	ldr	r6, [pc, #236]	; (4878 <shell_uart_irq_rom+0x108>)
    478a:	9001      	str	r0, [sp, #4]
    478c:	0020      	movs	r0, r4
    478e:	47b0      	blx	r6
    4790:	9603      	str	r6, [sp, #12]
    4792:	4b3a      	ldr	r3, [pc, #232]	; (487c <shell_uart_irq_rom+0x10c>)
    4794:	4798      	blx	r3
    4796:	2700      	movs	r7, #0
    4798:	9002      	str	r0, [sp, #8]
    479a:	9b02      	ldr	r3, [sp, #8]
    479c:	782e      	ldrb	r6, [r5, #0]
    479e:	429f      	cmp	r7, r3
    47a0:	d312      	bcc.n	47c8 <shell_uart_irq_rom+0x58>
    47a2:	b396      	cbz	r6, 480a <shell_uart_irq_rom+0x9a>
    47a4:	4b32      	ldr	r3, [pc, #200]	; (4870 <shell_uart_irq_rom+0x100>)
    47a6:	4e36      	ldr	r6, [pc, #216]	; (4880 <shell_uart_irq_rom+0x110>)
    47a8:	18e3      	adds	r3, r4, r3
    47aa:	7858      	ldrb	r0, [r3, #1]
    47ac:	281b      	cmp	r0, #27
    47ae:	d12f      	bne.n	4810 <shell_uart_irq_rom+0xa0>
    47b0:	0033      	movs	r3, r6
    47b2:	3340      	adds	r3, #64	; 0x40
    47b4:	795a      	ldrb	r2, [r3, #5]
    47b6:	3401      	adds	r4, #1
    47b8:	b2e4      	uxtb	r4, r4
    47ba:	2a01      	cmp	r2, #1
    47bc:	d012      	beq.n	47e4 <shell_uart_irq_rom+0x74>
    47be:	2201      	movs	r2, #1
    47c0:	715a      	strb	r2, [r3, #5]
    47c2:	2200      	movs	r2, #0
    47c4:	70da      	strb	r2, [r3, #3]
    47c6:	e018      	b.n	47fa <shell_uart_irq_rom+0x8a>
    47c8:	2e7e      	cmp	r6, #126	; 0x7e
    47ca:	d8eb      	bhi.n	47a4 <shell_uart_irq_rom+0x34>
    47cc:	4b2d      	ldr	r3, [pc, #180]	; (4884 <shell_uart_irq_rom+0x114>)
    47ce:	2000      	movs	r0, #0
    47d0:	4798      	blx	r3
    47d2:	4b27      	ldr	r3, [pc, #156]	; (4870 <shell_uart_irq_rom+0x100>)
    47d4:	3701      	adds	r7, #1
    47d6:	18f6      	adds	r6, r6, r3
    47d8:	7070      	strb	r0, [r6, #1]
    47da:	782b      	ldrb	r3, [r5, #0]
    47dc:	b2ff      	uxtb	r7, r7
    47de:	3301      	adds	r3, #1
    47e0:	702b      	strb	r3, [r5, #0]
    47e2:	e7da      	b.n	479a <shell_uart_irq_rom+0x2a>
    47e4:	799a      	ldrb	r2, [r3, #6]
    47e6:	2a01      	cmp	r2, #1
    47e8:	d002      	beq.n	47f0 <shell_uart_irq_rom+0x80>
    47ea:	6e32      	ldr	r2, [r6, #96]	; 0x60
    47ec:	2a01      	cmp	r2, #1
    47ee:	d1e8      	bne.n	47c2 <shell_uart_irq_rom+0x52>
    47f0:	3640      	adds	r6, #64	; 0x40
    47f2:	78f3      	ldrb	r3, [r6, #3]
    47f4:	b90b      	cbnz	r3, 47fa <shell_uart_irq_rom+0x8a>
    47f6:	3301      	adds	r3, #1
    47f8:	70f3      	strb	r3, [r6, #3]
    47fa:	782b      	ldrb	r3, [r5, #0]
    47fc:	42a3      	cmp	r3, r4
    47fe:	d001      	beq.n	4804 <shell_uart_irq_rom+0x94>
    4800:	2b00      	cmp	r3, #0
    4802:	d1c6      	bne.n	4792 <shell_uart_irq_rom+0x22>
    4804:	9801      	ldr	r0, [sp, #4]
    4806:	9b03      	ldr	r3, [sp, #12]
    4808:	4798      	blx	r3
    480a:	2000      	movs	r0, #0
    480c:	b005      	add	sp, #20
    480e:	bdf0      	pop	{r4, r5, r6, r7, pc}
    4810:	0037      	movs	r7, r6
    4812:	3740      	adds	r7, #64	; 0x40
    4814:	78fb      	ldrb	r3, [r7, #3]
    4816:	2b01      	cmp	r3, #1
    4818:	d107      	bne.n	482a <shell_uart_irq_rom+0xba>
    481a:	2300      	movs	r3, #0
    481c:	285b      	cmp	r0, #91	; 0x5b
    481e:	d100      	bne.n	4822 <shell_uart_irq_rom+0xb2>
    4820:	2302      	movs	r3, #2
    4822:	3401      	adds	r4, #1
    4824:	70fb      	strb	r3, [r7, #3]
    4826:	b2e4      	uxtb	r4, r4
    4828:	e7e7      	b.n	47fa <shell_uart_irq_rom+0x8a>
    482a:	78fb      	ldrb	r3, [r7, #3]
    482c:	2b02      	cmp	r3, #2
    482e:	d102      	bne.n	4836 <shell_uart_irq_rom+0xc6>
    4830:	2300      	movs	r3, #0
    4832:	70fb      	strb	r3, [r7, #3]
    4834:	e7e1      	b.n	47fa <shell_uart_irq_rom+0x8a>
    4836:	3401      	adds	r4, #1
    4838:	2201      	movs	r2, #1
    483a:	0039      	movs	r1, r7
    483c:	b2e4      	uxtb	r4, r4
    483e:	f7ff ff39 	bl	46b4 <shell_cmd_chk>
    4842:	2802      	cmp	r0, #2
    4844:	d1d9      	bne.n	47fa <shell_uart_irq_rom+0x8a>
    4846:	6cb2      	ldr	r2, [r6, #72]	; 0x48
    4848:	b13a      	cbz	r2, 485a <shell_uart_irq_rom+0xea>
    484a:	2301      	movs	r3, #1
    484c:	713b      	strb	r3, [r7, #4]
    484e:	6e33      	ldr	r3, [r6, #96]	; 0x60
    4850:	2b00      	cmp	r3, #0
    4852:	d0d2      	beq.n	47fa <shell_uart_irq_rom+0x8a>
    4854:	6df3      	ldr	r3, [r6, #92]	; 0x5c
    4856:	4798      	blx	r3
    4858:	e7cf      	b.n	47fa <shell_uart_irq_rom+0x8a>
    485a:	6cb0      	ldr	r0, [r6, #72]	; 0x48
    485c:	217f      	movs	r1, #127	; 0x7f
    485e:	3001      	adds	r0, #1
    4860:	f7ff fea6 	bl	45b0 <shell_array_init>
    4864:	e7c9      	b.n	47fa <shell_uart_irq_rom+0x8a>
    4866:	46c0      	nop			; (mov r8, r8)
    4868:	000012c3 	.word	0x000012c3
    486c:	4200c000 	.word	0x4200c000
    4870:	2300064c 	.word	0x2300064c
    4874:	00001291 	.word	0x00001291
    4878:	0000129d 	.word	0x0000129d
    487c:	00001285 	.word	0x00001285
    4880:	230005e8 	.word	0x230005e8
    4884:	0000125d 	.word	0x0000125d

00004888 <shell_init_rom>:
    4888:	b5f0      	push	{r4, r5, r6, r7, lr}
    488a:	2500      	movs	r5, #0
    488c:	b085      	sub	sp, #20
    488e:	4f17      	ldr	r7, [pc, #92]	; (48ec <shell_init_rom+0x64>)
    4890:	002a      	movs	r2, r5
    4892:	9101      	str	r1, [sp, #4]
    4894:	0006      	movs	r6, r0
    4896:	217f      	movs	r1, #127	; 0x7f
    4898:	4815      	ldr	r0, [pc, #84]	; (48f0 <shell_init_rom+0x68>)
    489a:	4c16      	ldr	r4, [pc, #88]	; (48f4 <shell_init_rom+0x6c>)
    489c:	703d      	strb	r5, [r7, #0]
    489e:	f7ff fe87 	bl	45b0 <shell_array_init>
    48a2:	4b15      	ldr	r3, [pc, #84]	; (48f8 <shell_init_rom+0x70>)
    48a4:	002a      	movs	r2, r5
    48a6:	217f      	movs	r1, #127	; 0x7f
    48a8:	4814      	ldr	r0, [pc, #80]	; (48fc <shell_init_rom+0x74>)
    48aa:	701d      	strb	r5, [r3, #0]
    48ac:	f7ff fe80 	bl	45b0 <shell_array_init>
    48b0:	0023      	movs	r3, r4
    48b2:	3340      	adds	r3, #64	; 0x40
    48b4:	701d      	strb	r5, [r3, #0]
    48b6:	705d      	strb	r5, [r3, #1]
    48b8:	709d      	strb	r5, [r3, #2]
    48ba:	70dd      	strb	r5, [r3, #3]
    48bc:	719d      	strb	r5, [r3, #6]
    48be:	4b10      	ldr	r3, [pc, #64]	; (4900 <shell_init_rom+0x78>)
    48c0:	64a7      	str	r7, [r4, #72]	; 0x48
    48c2:	64e3      	str	r3, [r4, #76]	; 0x4c
    48c4:	b14e      	cbz	r6, 48da <shell_init_rom+0x52>
    48c6:	9b01      	ldr	r3, [sp, #4]
    48c8:	6523      	str	r3, [r4, #80]	; 0x50
    48ca:	6566      	str	r6, [r4, #84]	; 0x54
    48cc:	2300      	movs	r3, #0
    48ce:	3440      	adds	r4, #64	; 0x40
    48d0:	6223      	str	r3, [r4, #32]
    48d2:	7123      	strb	r3, [r4, #4]
    48d4:	7163      	strb	r3, [r4, #5]
    48d6:	b005      	add	sp, #20
    48d8:	bdf0      	pop	{r4, r5, r6, r7, pc}
    48da:	4b0a      	ldr	r3, [pc, #40]	; (4904 <shell_init_rom+0x7c>)
    48dc:	a803      	add	r0, sp, #12
    48de:	9603      	str	r6, [sp, #12]
    48e0:	4798      	blx	r3
    48e2:	9b03      	ldr	r3, [sp, #12]
    48e4:	6523      	str	r3, [r4, #80]	; 0x50
    48e6:	6560      	str	r0, [r4, #84]	; 0x54
    48e8:	e7f0      	b.n	48cc <shell_init_rom+0x44>
    48ea:	46c0      	nop			; (mov r8, r8)
    48ec:	230006cc 	.word	0x230006cc
    48f0:	230006cd 	.word	0x230006cd
    48f4:	230005e8 	.word	0x230005e8
    48f8:	2300064c 	.word	0x2300064c
    48fc:	2300064d 	.word	0x2300064d
    4900:	00003f79 	.word	0x00003f79
    4904:	0000443d 	.word	0x0000443d

00004908 <shell_task_rom>:
    4908:	b510      	push	{r4, lr}
    490a:	4c07      	ldr	r4, [pc, #28]	; (4928 <shell_task_rom+0x20>)
    490c:	3440      	adds	r4, #64	; 0x40
    490e:	7923      	ldrb	r3, [r4, #4]
    4910:	2b01      	cmp	r3, #1
    4912:	d1fc      	bne.n	490e <shell_task_rom+0x6>
    4914:	0020      	movs	r0, r4
    4916:	f7ff fe8b 	bl	4630 <shell_cmd_exec_rom.constprop.0>
    491a:	4804      	ldr	r0, [pc, #16]	; (492c <shell_task_rom+0x24>)
    491c:	f7ff fb2c 	bl	3f78 <DiagPrintf>
    4920:	2300      	movs	r3, #0
    4922:	7123      	strb	r3, [r4, #4]
    4924:	e7f3      	b.n	490e <shell_task_rom+0x6>
    4926:	46c0      	nop			; (mov r8, r8)
    4928:	230005e8 	.word	0x230005e8
    492c:	0000c359 	.word	0x0000c359

00004930 <shell_rom>:
    4930:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    4932:	0006      	movs	r6, r0
    4934:	2500      	movs	r5, #0
    4936:	4f14      	ldr	r7, [pc, #80]	; (4988 <shell_rom+0x58>)
    4938:	4b14      	ldr	r3, [pc, #80]	; (498c <shell_rom+0x5c>)
    493a:	003c      	movs	r4, r7
    493c:	2064      	movs	r0, #100	; 0x64
    493e:	3440      	adds	r4, #64	; 0x40
    4940:	4798      	blx	r3
    4942:	7963      	ldrb	r3, [r4, #5]
    4944:	3501      	adds	r5, #1
    4946:	b2da      	uxtb	r2, r3
    4948:	9201      	str	r2, [sp, #4]
    494a:	2b01      	cmp	r3, #1
    494c:	d118      	bne.n	4980 <shell_rom+0x50>
    494e:	4810      	ldr	r0, [pc, #64]	; (4990 <shell_rom+0x60>)
    4950:	f7ff fb12 	bl	3f78 <DiagPrintf>
    4954:	2300      	movs	r3, #0
    4956:	2201      	movs	r2, #1
    4958:	70e3      	strb	r3, [r4, #3]
    495a:	4b0e      	ldr	r3, [pc, #56]	; (4994 <shell_rom+0x64>)
    495c:	4252      	negs	r2, r2
    495e:	601a      	str	r2, [r3, #0]
    4960:	9b01      	ldr	r3, [sp, #4]
    4962:	71a3      	strb	r3, [r4, #6]
    4964:	003c      	movs	r4, r7
    4966:	3440      	adds	r4, #64	; 0x40
    4968:	7923      	ldrb	r3, [r4, #4]
    496a:	2b01      	cmp	r3, #1
    496c:	d1fc      	bne.n	4968 <shell_rom+0x38>
    496e:	0020      	movs	r0, r4
    4970:	f7ff fe5e 	bl	4630 <shell_cmd_exec_rom.constprop.0>
    4974:	4806      	ldr	r0, [pc, #24]	; (4990 <shell_rom+0x60>)
    4976:	f7ff faff 	bl	3f78 <DiagPrintf>
    497a:	2300      	movs	r3, #0
    497c:	7123      	strb	r3, [r4, #4]
    497e:	e7f1      	b.n	4964 <shell_rom+0x34>
    4980:	42ae      	cmp	r6, r5
    4982:	d2d8      	bcs.n	4936 <shell_rom+0x6>
    4984:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}
    4986:	46c0      	nop			; (mov r8, r8)
    4988:	230005e8 	.word	0x230005e8
    498c:	0000030b 	.word	0x0000030b
    4990:	0000c359 	.word	0x0000c359
    4994:	230005d8 	.word	0x230005d8

00004998 <_char2num>:
    4998:	0003      	movs	r3, r0
    499a:	3830      	subs	r0, #48	; 0x30
    499c:	b2c0      	uxtb	r0, r0
    499e:	2809      	cmp	r0, #9
    49a0:	d909      	bls.n	49b6 <_char2num+0x1e>
    49a2:	001a      	movs	r2, r3
    49a4:	3a61      	subs	r2, #97	; 0x61
    49a6:	2a05      	cmp	r2, #5
    49a8:	d906      	bls.n	49b8 <_char2num+0x20>
    49aa:	001a      	movs	r2, r3
    49ac:	3a41      	subs	r2, #65	; 0x41
    49ae:	2a05      	cmp	r2, #5
    49b0:	d805      	bhi.n	49be <_char2num+0x26>
    49b2:	3b37      	subs	r3, #55	; 0x37
    49b4:	b2d8      	uxtb	r0, r3
    49b6:	4770      	bx	lr
    49b8:	3b57      	subs	r3, #87	; 0x57
    49ba:	b2d8      	uxtb	r0, r3
    49bc:	e7fb      	b.n	49b6 <_char2num+0x1e>
    49be:	20ff      	movs	r0, #255	; 0xff
    49c0:	e7f9      	b.n	49b6 <_char2num+0x1e>
    49c2:	46c0      	nop			; (mov r8, r8)

000049c4 <_2char2dec>:
    49c4:	0003      	movs	r3, r0
    49c6:	3b30      	subs	r3, #48	; 0x30
    49c8:	b2db      	uxtb	r3, r3
    49ca:	2b09      	cmp	r3, #9
    49cc:	d924      	bls.n	4a18 <_2char2dec+0x54>
    49ce:	0003      	movs	r3, r0
    49d0:	3b61      	subs	r3, #97	; 0x61
    49d2:	2b05      	cmp	r3, #5
    49d4:	d91a      	bls.n	4a0c <_2char2dec+0x48>
    49d6:	0003      	movs	r3, r0
    49d8:	3b41      	subs	r3, #65	; 0x41
    49da:	2b05      	cmp	r3, #5
    49dc:	d824      	bhi.n	4a28 <_2char2dec+0x64>
    49de:	3837      	subs	r0, #55	; 0x37
    49e0:	0083      	lsls	r3, r0, #2
    49e2:	18c0      	adds	r0, r0, r3
    49e4:	0042      	lsls	r2, r0, #1
    49e6:	b2d2      	uxtb	r2, r2
    49e8:	0008      	movs	r0, r1
    49ea:	3830      	subs	r0, #48	; 0x30
    49ec:	b2c0      	uxtb	r0, r0
    49ee:	2809      	cmp	r0, #9
    49f0:	d909      	bls.n	4a06 <_2char2dec+0x42>
    49f2:	000b      	movs	r3, r1
    49f4:	3b61      	subs	r3, #97	; 0x61
    49f6:	2b05      	cmp	r3, #5
    49f8:	d913      	bls.n	4a22 <_2char2dec+0x5e>
    49fa:	000b      	movs	r3, r1
    49fc:	3b41      	subs	r3, #65	; 0x41
    49fe:	2b05      	cmp	r3, #5
    4a00:	d814      	bhi.n	4a2c <_2char2dec+0x68>
    4a02:	3937      	subs	r1, #55	; 0x37
    4a04:	b2c8      	uxtb	r0, r1
    4a06:	1880      	adds	r0, r0, r2
    4a08:	b2c0      	uxtb	r0, r0
    4a0a:	4770      	bx	lr
    4a0c:	3857      	subs	r0, #87	; 0x57
    4a0e:	0083      	lsls	r3, r0, #2
    4a10:	18c0      	adds	r0, r0, r3
    4a12:	0042      	lsls	r2, r0, #1
    4a14:	b2d2      	uxtb	r2, r2
    4a16:	e7e7      	b.n	49e8 <_2char2dec+0x24>
    4a18:	009a      	lsls	r2, r3, #2
    4a1a:	189b      	adds	r3, r3, r2
    4a1c:	005b      	lsls	r3, r3, #1
    4a1e:	b2da      	uxtb	r2, r3
    4a20:	e7e2      	b.n	49e8 <_2char2dec+0x24>
    4a22:	3957      	subs	r1, #87	; 0x57
    4a24:	b2c8      	uxtb	r0, r1
    4a26:	e7ee      	b.n	4a06 <_2char2dec+0x42>
    4a28:	22f6      	movs	r2, #246	; 0xf6
    4a2a:	e7dd      	b.n	49e8 <_2char2dec+0x24>
    4a2c:	20ff      	movs	r0, #255	; 0xff
    4a2e:	e7ea      	b.n	4a06 <_2char2dec+0x42>

00004a30 <_2char2hex>:
    4a30:	0003      	movs	r3, r0
    4a32:	3b30      	subs	r3, #48	; 0x30
    4a34:	b2db      	uxtb	r3, r3
    4a36:	2b09      	cmp	r3, #9
    4a38:	d916      	bls.n	4a68 <_2char2hex+0x38>
    4a3a:	0003      	movs	r3, r0
    4a3c:	3b61      	subs	r3, #97	; 0x61
    4a3e:	2b05      	cmp	r3, #5
    4a40:	d910      	bls.n	4a64 <_2char2hex+0x34>
    4a42:	0003      	movs	r3, r0
    4a44:	3b41      	subs	r3, #65	; 0x41
    4a46:	2b05      	cmp	r3, #5
    4a48:	d827      	bhi.n	4a9a <_2char2hex+0x6a>
    4a4a:	0002      	movs	r2, r0
    4a4c:	3a37      	subs	r2, #55	; 0x37
    4a4e:	b2d3      	uxtb	r3, r2
    4a50:	011b      	lsls	r3, r3, #4
    4a52:	b258      	sxtb	r0, r3
    4a54:	000b      	movs	r3, r1
    4a56:	3b30      	subs	r3, #48	; 0x30
    4a58:	b2db      	uxtb	r3, r3
    4a5a:	2b09      	cmp	r3, #9
    4a5c:	d80b      	bhi.n	4a76 <_2char2hex+0x46>
    4a5e:	4318      	orrs	r0, r3
    4a60:	b2c0      	uxtb	r0, r0
    4a62:	e013      	b.n	4a8c <_2char2hex+0x5c>
    4a64:	3857      	subs	r0, #87	; 0x57
    4a66:	b2c3      	uxtb	r3, r0
    4a68:	011b      	lsls	r3, r3, #4
    4a6a:	b258      	sxtb	r0, r3
    4a6c:	000b      	movs	r3, r1
    4a6e:	3b30      	subs	r3, #48	; 0x30
    4a70:	b2db      	uxtb	r3, r3
    4a72:	2b09      	cmp	r3, #9
    4a74:	d9f3      	bls.n	4a5e <_2char2hex+0x2e>
    4a76:	000b      	movs	r3, r1
    4a78:	3b61      	subs	r3, #97	; 0x61
    4a7a:	2b05      	cmp	r3, #5
    4a7c:	d907      	bls.n	4a8e <_2char2hex+0x5e>
    4a7e:	000b      	movs	r3, r1
    4a80:	3b41      	subs	r3, #65	; 0x41
    4a82:	2b05      	cmp	r3, #5
    4a84:	d807      	bhi.n	4a96 <_2char2hex+0x66>
    4a86:	3937      	subs	r1, #55	; 0x37
    4a88:	4308      	orrs	r0, r1
    4a8a:	b2c0      	uxtb	r0, r0
    4a8c:	4770      	bx	lr
    4a8e:	3957      	subs	r1, #87	; 0x57
    4a90:	4308      	orrs	r0, r1
    4a92:	b2c0      	uxtb	r0, r0
    4a94:	e7fa      	b.n	4a8c <_2char2hex+0x5c>
    4a96:	20ff      	movs	r0, #255	; 0xff
    4a98:	e7f8      	b.n	4a8c <_2char2hex+0x5c>
    4a9a:	2010      	movs	r0, #16
    4a9c:	4240      	negs	r0, r0
    4a9e:	e7e5      	b.n	4a6c <_2char2hex+0x3c>

00004aa0 <_memchr>:
    4aa0:	b570      	push	{r4, r5, r6, lr}
    4aa2:	b2cc      	uxtb	r4, r1
    4aa4:	0783      	lsls	r3, r0, #30
    4aa6:	d00c      	beq.n	4ac2 <_memchr+0x22>
    4aa8:	1e53      	subs	r3, r2, #1
    4aaa:	b172      	cbz	r2, 4aca <_memchr+0x2a>
    4aac:	2503      	movs	r5, #3
    4aae:	e004      	b.n	4aba <_memchr+0x1a>
    4ab0:	3001      	adds	r0, #1
    4ab2:	4228      	tst	r0, r5
    4ab4:	d006      	beq.n	4ac4 <_memchr+0x24>
    4ab6:	3b01      	subs	r3, #1
    4ab8:	d307      	bcc.n	4aca <_memchr+0x2a>
    4aba:	7802      	ldrb	r2, [r0, #0]
    4abc:	42a2      	cmp	r2, r4
    4abe:	d1f7      	bne.n	4ab0 <_memchr+0x10>
    4ac0:	bd70      	pop	{r4, r5, r6, pc}
    4ac2:	0013      	movs	r3, r2
    4ac4:	2b03      	cmp	r3, #3
    4ac6:	d80b      	bhi.n	4ae0 <_memchr+0x40>
    4ac8:	b90b      	cbnz	r3, 4ace <_memchr+0x2e>
    4aca:	2000      	movs	r0, #0
    4acc:	e7f8      	b.n	4ac0 <_memchr+0x20>
    4ace:	18c3      	adds	r3, r0, r3
    4ad0:	e002      	b.n	4ad8 <_memchr+0x38>
    4ad2:	3001      	adds	r0, #1
    4ad4:	4283      	cmp	r3, r0
    4ad6:	d0f8      	beq.n	4aca <_memchr+0x2a>
    4ad8:	7802      	ldrb	r2, [r0, #0]
    4ada:	42a2      	cmp	r2, r4
    4adc:	d1f9      	bne.n	4ad2 <_memchr+0x32>
    4ade:	e7ef      	b.n	4ac0 <_memchr+0x20>
    4ae0:	25ff      	movs	r5, #255	; 0xff
    4ae2:	f248 0680 	movw	r6, #32896	; 0x8080
    4ae6:	4029      	ands	r1, r5
    4ae8:	020d      	lsls	r5, r1, #8
    4aea:	4329      	orrs	r1, r5
    4aec:	040d      	lsls	r5, r1, #16
    4aee:	430d      	orrs	r5, r1
    4af0:	f2c8 0680 	movt	r6, #32896	; 0x8080
    4af4:	f64f 61ff 	movw	r1, #65279	; 0xfeff
    4af8:	6802      	ldr	r2, [r0, #0]
    4afa:	f6cf 61fe 	movt	r1, #65278	; 0xfefe
    4afe:	406a      	eors	r2, r5
    4b00:	1851      	adds	r1, r2, r1
    4b02:	4391      	bics	r1, r2
    4b04:	4231      	tst	r1, r6
    4b06:	d1e2      	bne.n	4ace <_memchr+0x2e>
    4b08:	3b04      	subs	r3, #4
    4b0a:	3004      	adds	r0, #4
    4b0c:	2b03      	cmp	r3, #3
    4b0e:	d8f1      	bhi.n	4af4 <_memchr+0x54>
    4b10:	2b00      	cmp	r3, #0
    4b12:	d1dc      	bne.n	4ace <_memchr+0x2e>
    4b14:	e7d9      	b.n	4aca <_memchr+0x2a>
    4b16:	46c0      	nop			; (mov r8, r8)

00004b18 <_memcmp>:
    4b18:	b530      	push	{r4, r5, lr}
    4b1a:	2a03      	cmp	r2, #3
    4b1c:	d90c      	bls.n	4b38 <_memcmp+0x20>
    4b1e:	0003      	movs	r3, r0
    4b20:	430b      	orrs	r3, r1
    4b22:	079b      	lsls	r3, r3, #30
    4b24:	d118      	bne.n	4b58 <_memcmp+0x40>
    4b26:	6803      	ldr	r3, [r0, #0]
    4b28:	680c      	ldr	r4, [r1, #0]
    4b2a:	42a3      	cmp	r3, r4
    4b2c:	d114      	bne.n	4b58 <_memcmp+0x40>
    4b2e:	3a04      	subs	r2, #4
    4b30:	3004      	adds	r0, #4
    4b32:	3104      	adds	r1, #4
    4b34:	2a03      	cmp	r2, #3
    4b36:	d8f6      	bhi.n	4b26 <_memcmp+0xe>
    4b38:	1e55      	subs	r5, r2, #1
    4b3a:	b15a      	cbz	r2, 4b54 <_memcmp+0x3c>
    4b3c:	2300      	movs	r3, #0
    4b3e:	e003      	b.n	4b48 <_memcmp+0x30>
    4b40:	1c5a      	adds	r2, r3, #1
    4b42:	429d      	cmp	r5, r3
    4b44:	d006      	beq.n	4b54 <_memcmp+0x3c>
    4b46:	0013      	movs	r3, r2
    4b48:	5cc2      	ldrb	r2, [r0, r3]
    4b4a:	5ccc      	ldrb	r4, [r1, r3]
    4b4c:	42a2      	cmp	r2, r4
    4b4e:	d0f7      	beq.n	4b40 <_memcmp+0x28>
    4b50:	1b10      	subs	r0, r2, r4
    4b52:	e000      	b.n	4b56 <_memcmp+0x3e>
    4b54:	2000      	movs	r0, #0
    4b56:	bd30      	pop	{r4, r5, pc}
    4b58:	1e55      	subs	r5, r2, #1
    4b5a:	e7ef      	b.n	4b3c <_memcmp+0x24>

00004b5c <_memcpy>:
    4b5c:	b5f0      	push	{r4, r5, r6, r7, lr}
    4b5e:	46c6      	mov	lr, r8
    4b60:	b500      	push	{lr}
    4b62:	2a0f      	cmp	r2, #15
    4b64:	d940      	bls.n	4be8 <_memcpy+0x8c>
    4b66:	2703      	movs	r7, #3
    4b68:	000e      	movs	r6, r1
    4b6a:	003d      	movs	r5, r7
    4b6c:	4306      	orrs	r6, r0
    4b6e:	000c      	movs	r4, r1
    4b70:	0003      	movs	r3, r0
    4b72:	4035      	ands	r5, r6
    4b74:	4237      	tst	r7, r6
    4b76:	d13c      	bne.n	4bf2 <_memcpy+0x96>
    4b78:	0016      	movs	r6, r2
    4b7a:	3e10      	subs	r6, #16
    4b7c:	0936      	lsrs	r6, r6, #4
    4b7e:	0137      	lsls	r7, r6, #4
    4b80:	46b8      	mov	r8, r7
    4b82:	46b4      	mov	ip, r6
    4b84:	4480      	add	r8, r0
    4b86:	6827      	ldr	r7, [r4, #0]
    4b88:	001e      	movs	r6, r3
    4b8a:	601f      	str	r7, [r3, #0]
    4b8c:	6867      	ldr	r7, [r4, #4]
    4b8e:	605f      	str	r7, [r3, #4]
    4b90:	68a7      	ldr	r7, [r4, #8]
    4b92:	609f      	str	r7, [r3, #8]
    4b94:	68e7      	ldr	r7, [r4, #12]
    4b96:	3410      	adds	r4, #16
    4b98:	60df      	str	r7, [r3, #12]
    4b9a:	3310      	adds	r3, #16
    4b9c:	4546      	cmp	r6, r8
    4b9e:	d1f2      	bne.n	4b86 <_memcpy+0x2a>
    4ba0:	4664      	mov	r4, ip
    4ba2:	230f      	movs	r3, #15
    4ba4:	260c      	movs	r6, #12
    4ba6:	3401      	adds	r4, #1
    4ba8:	0124      	lsls	r4, r4, #4
    4baa:	1909      	adds	r1, r1, r4
    4bac:	4013      	ands	r3, r2
    4bae:	1904      	adds	r4, r0, r4
    4bb0:	4216      	tst	r6, r2
    4bb2:	d021      	beq.n	4bf8 <_memcpy+0x9c>
    4bb4:	594e      	ldr	r6, [r1, r5]
    4bb6:	5166      	str	r6, [r4, r5]
    4bb8:	3504      	adds	r5, #4
    4bba:	1b5e      	subs	r6, r3, r5
    4bbc:	2e03      	cmp	r6, #3
    4bbe:	d8f9      	bhi.n	4bb4 <_memcpy+0x58>
    4bc0:	2503      	movs	r5, #3
    4bc2:	3b04      	subs	r3, #4
    4bc4:	089b      	lsrs	r3, r3, #2
    4bc6:	3301      	adds	r3, #1
    4bc8:	009b      	lsls	r3, r3, #2
    4bca:	402a      	ands	r2, r5
    4bcc:	18e4      	adds	r4, r4, r3
    4bce:	18c9      	adds	r1, r1, r3
    4bd0:	1e55      	subs	r5, r2, #1
    4bd2:	b132      	cbz	r2, 4be2 <_memcpy+0x86>
    4bd4:	2300      	movs	r3, #0
    4bd6:	5cca      	ldrb	r2, [r1, r3]
    4bd8:	54e2      	strb	r2, [r4, r3]
    4bda:	001a      	movs	r2, r3
    4bdc:	3301      	adds	r3, #1
    4bde:	4295      	cmp	r5, r2
    4be0:	d1f9      	bne.n	4bd6 <_memcpy+0x7a>
    4be2:	bc80      	pop	{r7}
    4be4:	46b8      	mov	r8, r7
    4be6:	bdf0      	pop	{r4, r5, r6, r7, pc}
    4be8:	0004      	movs	r4, r0
    4bea:	1e55      	subs	r5, r2, #1
    4bec:	2a00      	cmp	r2, #0
    4bee:	d1f1      	bne.n	4bd4 <_memcpy+0x78>
    4bf0:	e7f7      	b.n	4be2 <_memcpy+0x86>
    4bf2:	0004      	movs	r4, r0
    4bf4:	1e55      	subs	r5, r2, #1
    4bf6:	e7ed      	b.n	4bd4 <_memcpy+0x78>
    4bf8:	001a      	movs	r2, r3
    4bfa:	e7f6      	b.n	4bea <_memcpy+0x8e>

00004bfc <_memmove>:
    4bfc:	b5f0      	push	{r4, r5, r6, r7, lr}
    4bfe:	4288      	cmp	r0, r1
    4c00:	d909      	bls.n	4c16 <_memmove+0x1a>
    4c02:	188b      	adds	r3, r1, r2
    4c04:	4298      	cmp	r0, r3
    4c06:	d206      	bcs.n	4c16 <_memmove+0x1a>
    4c08:	1e53      	subs	r3, r2, #1
    4c0a:	b11a      	cbz	r2, 4c14 <_memmove+0x18>
    4c0c:	5cca      	ldrb	r2, [r1, r3]
    4c0e:	54c2      	strb	r2, [r0, r3]
    4c10:	3b01      	subs	r3, #1
    4c12:	d2fb      	bcs.n	4c0c <_memmove+0x10>
    4c14:	bdf0      	pop	{r4, r5, r6, r7, pc}
    4c16:	2a0f      	cmp	r2, #15
    4c18:	d80b      	bhi.n	4c32 <_memmove+0x36>
    4c1a:	0005      	movs	r5, r0
    4c1c:	1e56      	subs	r6, r2, #1
    4c1e:	2a00      	cmp	r2, #0
    4c20:	d0f8      	beq.n	4c14 <_memmove+0x18>
    4c22:	2300      	movs	r3, #0
    4c24:	5ccc      	ldrb	r4, [r1, r3]
    4c26:	001a      	movs	r2, r3
    4c28:	54ec      	strb	r4, [r5, r3]
    4c2a:	3301      	adds	r3, #1
    4c2c:	4296      	cmp	r6, r2
    4c2e:	d1f9      	bne.n	4c24 <_memmove+0x28>
    4c30:	e7f0      	b.n	4c14 <_memmove+0x18>
    4c32:	2703      	movs	r7, #3
    4c34:	000d      	movs	r5, r1
    4c36:	003e      	movs	r6, r7
    4c38:	4305      	orrs	r5, r0
    4c3a:	000c      	movs	r4, r1
    4c3c:	0003      	movs	r3, r0
    4c3e:	402e      	ands	r6, r5
    4c40:	422f      	tst	r7, r5
    4c42:	d12b      	bne.n	4c9c <_memmove+0xa0>
    4c44:	0015      	movs	r5, r2
    4c46:	3d10      	subs	r5, #16
    4c48:	092d      	lsrs	r5, r5, #4
    4c4a:	46ac      	mov	ip, r5
    4c4c:	012f      	lsls	r7, r5, #4
    4c4e:	183f      	adds	r7, r7, r0
    4c50:	6825      	ldr	r5, [r4, #0]
    4c52:	601d      	str	r5, [r3, #0]
    4c54:	6865      	ldr	r5, [r4, #4]
    4c56:	605d      	str	r5, [r3, #4]
    4c58:	68a5      	ldr	r5, [r4, #8]
    4c5a:	609d      	str	r5, [r3, #8]
    4c5c:	68e5      	ldr	r5, [r4, #12]
    4c5e:	3410      	adds	r4, #16
    4c60:	60dd      	str	r5, [r3, #12]
    4c62:	001d      	movs	r5, r3
    4c64:	3310      	adds	r3, #16
    4c66:	42bd      	cmp	r5, r7
    4c68:	d1f2      	bne.n	4c50 <_memmove+0x54>
    4c6a:	4665      	mov	r5, ip
    4c6c:	230f      	movs	r3, #15
    4c6e:	240c      	movs	r4, #12
    4c70:	3501      	adds	r5, #1
    4c72:	012d      	lsls	r5, r5, #4
    4c74:	1949      	adds	r1, r1, r5
    4c76:	4013      	ands	r3, r2
    4c78:	1945      	adds	r5, r0, r5
    4c7a:	4214      	tst	r4, r2
    4c7c:	d011      	beq.n	4ca2 <_memmove+0xa6>
    4c7e:	598c      	ldr	r4, [r1, r6]
    4c80:	51ac      	str	r4, [r5, r6]
    4c82:	3604      	adds	r6, #4
    4c84:	1b9c      	subs	r4, r3, r6
    4c86:	2c03      	cmp	r4, #3
    4c88:	d8f9      	bhi.n	4c7e <_memmove+0x82>
    4c8a:	3b04      	subs	r3, #4
    4c8c:	089b      	lsrs	r3, r3, #2
    4c8e:	3301      	adds	r3, #1
    4c90:	009b      	lsls	r3, r3, #2
    4c92:	18ed      	adds	r5, r5, r3
    4c94:	18c9      	adds	r1, r1, r3
    4c96:	2303      	movs	r3, #3
    4c98:	401a      	ands	r2, r3
    4c9a:	e7bf      	b.n	4c1c <_memmove+0x20>
    4c9c:	0005      	movs	r5, r0
    4c9e:	1e56      	subs	r6, r2, #1
    4ca0:	e7bf      	b.n	4c22 <_memmove+0x26>
    4ca2:	001a      	movs	r2, r3
    4ca4:	e7ba      	b.n	4c1c <_memmove+0x20>
    4ca6:	46c0      	nop			; (mov r8, r8)

00004ca8 <_memset>:
    4ca8:	b5f0      	push	{r4, r5, r6, r7, lr}
    4caa:	0783      	lsls	r3, r0, #30
    4cac:	d047      	beq.n	4d3e <_memset+0x96>
    4cae:	1e54      	subs	r4, r2, #1
    4cb0:	2a00      	cmp	r2, #0
    4cb2:	d043      	beq.n	4d3c <_memset+0x94>
    4cb4:	0003      	movs	r3, r0
    4cb6:	2503      	movs	r5, #3
    4cb8:	b2ce      	uxtb	r6, r1
    4cba:	e001      	b.n	4cc0 <_memset+0x18>
    4cbc:	3c01      	subs	r4, #1
    4cbe:	d33d      	bcc.n	4d3c <_memset+0x94>
    4cc0:	3301      	adds	r3, #1
    4cc2:	1e5a      	subs	r2, r3, #1
    4cc4:	7016      	strb	r6, [r2, #0]
    4cc6:	422b      	tst	r3, r5
    4cc8:	d1f8      	bne.n	4cbc <_memset+0x14>
    4cca:	2c03      	cmp	r4, #3
    4ccc:	d92f      	bls.n	4d2e <_memset+0x86>
    4cce:	22ff      	movs	r2, #255	; 0xff
    4cd0:	400a      	ands	r2, r1
    4cd2:	0215      	lsls	r5, r2, #8
    4cd4:	4315      	orrs	r5, r2
    4cd6:	042a      	lsls	r2, r5, #16
    4cd8:	4315      	orrs	r5, r2
    4cda:	2c0f      	cmp	r4, #15
    4cdc:	d932      	bls.n	4d44 <_memset+0x9c>
    4cde:	0027      	movs	r7, r4
    4ce0:	3f10      	subs	r7, #16
    4ce2:	093f      	lsrs	r7, r7, #4
    4ce4:	013e      	lsls	r6, r7, #4
    4ce6:	46b4      	mov	ip, r6
    4ce8:	001e      	movs	r6, r3
    4cea:	001a      	movs	r2, r3
    4cec:	3610      	adds	r6, #16
    4cee:	4466      	add	r6, ip
    4cf0:	6015      	str	r5, [r2, #0]
    4cf2:	6055      	str	r5, [r2, #4]
    4cf4:	6095      	str	r5, [r2, #8]
    4cf6:	60d5      	str	r5, [r2, #12]
    4cf8:	3210      	adds	r2, #16
    4cfa:	42b2      	cmp	r2, r6
    4cfc:	d1f8      	bne.n	4cf0 <_memset+0x48>
    4cfe:	3701      	adds	r7, #1
    4d00:	013f      	lsls	r7, r7, #4
    4d02:	220c      	movs	r2, #12
    4d04:	19db      	adds	r3, r3, r7
    4d06:	270f      	movs	r7, #15
    4d08:	4022      	ands	r2, r4
    4d0a:	4027      	ands	r7, r4
    4d0c:	003c      	movs	r4, r7
    4d0e:	b172      	cbz	r2, 4d2e <_memset+0x86>
    4d10:	1f3e      	subs	r6, r7, #4
    4d12:	08b6      	lsrs	r6, r6, #2
    4d14:	00b4      	lsls	r4, r6, #2
    4d16:	46a4      	mov	ip, r4
    4d18:	001a      	movs	r2, r3
    4d1a:	1d1c      	adds	r4, r3, #4
    4d1c:	4464      	add	r4, ip
    4d1e:	c220      	stmia	r2!, {r5}
    4d20:	42a2      	cmp	r2, r4
    4d22:	d1fc      	bne.n	4d1e <_memset+0x76>
    4d24:	2403      	movs	r4, #3
    4d26:	3601      	adds	r6, #1
    4d28:	00b6      	lsls	r6, r6, #2
    4d2a:	199b      	adds	r3, r3, r6
    4d2c:	403c      	ands	r4, r7
    4d2e:	b12c      	cbz	r4, 4d3c <_memset+0x94>
    4d30:	b2c9      	uxtb	r1, r1
    4d32:	191c      	adds	r4, r3, r4
    4d34:	7019      	strb	r1, [r3, #0]
    4d36:	3301      	adds	r3, #1
    4d38:	429c      	cmp	r4, r3
    4d3a:	d1fb      	bne.n	4d34 <_memset+0x8c>
    4d3c:	bdf0      	pop	{r4, r5, r6, r7, pc}
    4d3e:	0003      	movs	r3, r0
    4d40:	0014      	movs	r4, r2
    4d42:	e7c2      	b.n	4cca <_memset+0x22>
    4d44:	0027      	movs	r7, r4
    4d46:	e7e3      	b.n	4d10 <_memset+0x68>

00004d48 <_strcmp>:
    4d48:	0003      	movs	r3, r0
    4d4a:	430b      	orrs	r3, r1
    4d4c:	b510      	push	{r4, lr}
    4d4e:	079b      	lsls	r3, r3, #30
    4d50:	d118      	bne.n	4d84 <_strcmp+0x3c>
    4d52:	6803      	ldr	r3, [r0, #0]
    4d54:	680a      	ldr	r2, [r1, #0]
    4d56:	4293      	cmp	r3, r2
    4d58:	d114      	bne.n	4d84 <_strcmp+0x3c>
    4d5a:	f248 0480 	movw	r4, #32896	; 0x8080
    4d5e:	f2c8 0480 	movt	r4, #32896	; 0x8080
    4d62:	e005      	b.n	4d70 <_strcmp+0x28>
    4d64:	6843      	ldr	r3, [r0, #4]
    4d66:	684a      	ldr	r2, [r1, #4]
    4d68:	3004      	adds	r0, #4
    4d6a:	3104      	adds	r1, #4
    4d6c:	4293      	cmp	r3, r2
    4d6e:	d109      	bne.n	4d84 <_strcmp+0x3c>
    4d70:	f64f 62ff 	movw	r2, #65279	; 0xfeff
    4d74:	f6cf 62fe 	movt	r2, #65278	; 0xfefe
    4d78:	189a      	adds	r2, r3, r2
    4d7a:	439a      	bics	r2, r3
    4d7c:	4222      	tst	r2, r4
    4d7e:	d0f1      	beq.n	4d64 <_strcmp+0x1c>
    4d80:	2000      	movs	r0, #0
    4d82:	e00c      	b.n	4d9e <_strcmp+0x56>
    4d84:	7802      	ldrb	r2, [r0, #0]
    4d86:	780c      	ldrb	r4, [r1, #0]
    4d88:	2301      	movs	r3, #1
    4d8a:	3901      	subs	r1, #1
    4d8c:	b922      	cbnz	r2, 4d98 <_strcmp+0x50>
    4d8e:	e005      	b.n	4d9c <_strcmp+0x54>
    4d90:	5cc2      	ldrb	r2, [r0, r3]
    4d92:	3301      	adds	r3, #1
    4d94:	5ccc      	ldrb	r4, [r1, r3]
    4d96:	b11a      	cbz	r2, 4da0 <_strcmp+0x58>
    4d98:	42a2      	cmp	r2, r4
    4d9a:	d0f9      	beq.n	4d90 <_strcmp+0x48>
    4d9c:	1b10      	subs	r0, r2, r4
    4d9e:	bd10      	pop	{r4, pc}
    4da0:	2200      	movs	r2, #0
    4da2:	e7fb      	b.n	4d9c <_strcmp+0x54>

00004da4 <_stricmp>:
    4da4:	b5f0      	push	{r4, r5, r6, r7, lr}
    4da6:	2200      	movs	r2, #0
    4da8:	2620      	movs	r6, #32
    4daa:	5c84      	ldrb	r4, [r0, r2]
    4dac:	5c8b      	ldrb	r3, [r1, r2]
    4dae:	429c      	cmp	r4, r3
    4db0:	d00a      	beq.n	4dc8 <_stricmp+0x24>
    4db2:	0025      	movs	r5, r4
    4db4:	4335      	orrs	r5, r6
    4db6:	b2ed      	uxtb	r5, r5
    4db8:	002f      	movs	r7, r5
    4dba:	3f61      	subs	r7, #97	; 0x61
    4dbc:	2f19      	cmp	r7, #25
    4dbe:	d808      	bhi.n	4dd2 <_stricmp+0x2e>
    4dc0:	4333      	orrs	r3, r6
    4dc2:	b2db      	uxtb	r3, r3
    4dc4:	429d      	cmp	r5, r3
    4dc6:	d104      	bne.n	4dd2 <_stricmp+0x2e>
    4dc8:	3201      	adds	r2, #1
    4dca:	2c00      	cmp	r4, #0
    4dcc:	d1ed      	bne.n	4daa <_stricmp+0x6>
    4dce:	2000      	movs	r0, #0
    4dd0:	e000      	b.n	4dd4 <_stricmp+0x30>
    4dd2:	2001      	movs	r0, #1
    4dd4:	bdf0      	pop	{r4, r5, r6, r7, pc}
    4dd6:	46c0      	nop			; (mov r8, r8)

00004dd8 <_strlen>:
    4dd8:	b510      	push	{r4, lr}
    4dda:	0783      	lsls	r3, r0, #30
    4ddc:	d00a      	beq.n	4df4 <_strlen+0x1c>
    4dde:	0003      	movs	r3, r0
    4de0:	2103      	movs	r1, #3
    4de2:	e002      	b.n	4dea <_strlen+0x12>
    4de4:	3301      	adds	r3, #1
    4de6:	420b      	tst	r3, r1
    4de8:	d005      	beq.n	4df6 <_strlen+0x1e>
    4dea:	781a      	ldrb	r2, [r3, #0]
    4dec:	2a00      	cmp	r2, #0
    4dee:	d1f9      	bne.n	4de4 <_strlen+0xc>
    4df0:	1a18      	subs	r0, r3, r0
    4df2:	bd10      	pop	{r4, pc}
    4df4:	0003      	movs	r3, r0
    4df6:	f64f 62ff 	movw	r2, #65279	; 0xfeff
    4dfa:	f248 0480 	movw	r4, #32896	; 0x8080
    4dfe:	6819      	ldr	r1, [r3, #0]
    4e00:	f6cf 62fe 	movt	r2, #65278	; 0xfefe
    4e04:	188a      	adds	r2, r1, r2
    4e06:	438a      	bics	r2, r1
    4e08:	f2c8 0480 	movt	r4, #32896	; 0x8080
    4e0c:	4222      	tst	r2, r4
    4e0e:	d10b      	bne.n	4e28 <_strlen+0x50>
    4e10:	f64f 62ff 	movw	r2, #65279	; 0xfeff
    4e14:	6859      	ldr	r1, [r3, #4]
    4e16:	f6cf 62fe 	movt	r2, #65278	; 0xfefe
    4e1a:	188a      	adds	r2, r1, r2
    4e1c:	438a      	bics	r2, r1
    4e1e:	3304      	adds	r3, #4
    4e20:	4222      	tst	r2, r4
    4e22:	d0f5      	beq.n	4e10 <_strlen+0x38>
    4e24:	e000      	b.n	4e28 <_strlen+0x50>
    4e26:	3301      	adds	r3, #1
    4e28:	781a      	ldrb	r2, [r3, #0]
    4e2a:	2a00      	cmp	r2, #0
    4e2c:	d1fb      	bne.n	4e26 <_strlen+0x4e>
    4e2e:	e7df      	b.n	4df0 <_strlen+0x18>

00004e30 <_strtoul>:
    4e30:	b5f0      	push	{r4, r5, r6, r7, lr}
    4e32:	46de      	mov	lr, fp
    4e34:	4657      	mov	r7, sl
    4e36:	4645      	mov	r5, r8
    4e38:	464e      	mov	r6, r9
    4e3a:	4683      	mov	fp, r0
    4e3c:	b5e0      	push	{r5, r6, r7, lr}
    4e3e:	468a      	mov	sl, r1
    4e40:	0015      	movs	r5, r2
    4e42:	0002      	movs	r2, r0
    4e44:	7813      	ldrb	r3, [r2, #0]
    4e46:	1c51      	adds	r1, r2, #1
    4e48:	2b20      	cmp	r3, #32
    4e4a:	d005      	beq.n	4e58 <_strtoul+0x28>
    4e4c:	001c      	movs	r4, r3
    4e4e:	3c09      	subs	r4, #9
    4e50:	2c04      	cmp	r4, #4
    4e52:	d901      	bls.n	4e58 <_strtoul+0x28>
    4e54:	2b2c      	cmp	r3, #44	; 0x2c
    4e56:	d101      	bne.n	4e5c <_strtoul+0x2c>
    4e58:	000a      	movs	r2, r1
    4e5a:	e7f3      	b.n	4e44 <_strtoul+0x14>
    4e5c:	2b2d      	cmp	r3, #45	; 0x2d
    4e5e:	d05d      	beq.n	4f1c <_strtoul+0xec>
    4e60:	f240 0800 	movw	r8, #0
    4e64:	2b2b      	cmp	r3, #43	; 0x2b
    4e66:	d034      	beq.n	4ed2 <_strtoul+0xa2>
    4e68:	b33d      	cbz	r5, 4eba <_strtoul+0x8a>
    4e6a:	2d10      	cmp	r5, #16
    4e6c:	d05d      	beq.n	4f2a <_strtoul+0xfa>
    4e6e:	2601      	movs	r6, #1
    4e70:	4276      	negs	r6, r6
    4e72:	fbb6 f6f5 	udiv	r6, r6, r5
    4e76:	002a      	movs	r2, r5
    4e78:	4372      	muls	r2, r6
    4e7a:	43d2      	mvns	r2, r2
    4e7c:	46ac      	mov	ip, r5
    4e7e:	4691      	mov	r9, r2
    4e80:	2400      	movs	r4, #0
    4e82:	2000      	movs	r0, #0
    4e84:	001a      	movs	r2, r3
    4e86:	3a30      	subs	r2, #48	; 0x30
    4e88:	b2d7      	uxtb	r7, r2
    4e8a:	2f09      	cmp	r7, #9
    4e8c:	d907      	bls.n	4e9e <_strtoul+0x6e>
    4e8e:	3a11      	subs	r2, #17
    4e90:	2a19      	cmp	r2, #25
    4e92:	d928      	bls.n	4ee6 <_strtoul+0xb6>
    4e94:	3a20      	subs	r2, #32
    4e96:	2a19      	cmp	r2, #25
    4e98:	d827      	bhi.n	4eea <_strtoul+0xba>
    4e9a:	2257      	movs	r2, #87	; 0x57
    4e9c:	1a9a      	subs	r2, r3, r2
    4e9e:	4295      	cmp	r5, r2
    4ea0:	dd23      	ble.n	4eea <_strtoul+0xba>
    4ea2:	2c00      	cmp	r4, #0
    4ea4:	db1c      	blt.n	4ee0 <_strtoul+0xb0>
    4ea6:	4286      	cmp	r6, r0
    4ea8:	d31a      	bcc.n	4ee0 <_strtoul+0xb0>
    4eaa:	d017      	beq.n	4edc <_strtoul+0xac>
    4eac:	4663      	mov	r3, ip
    4eae:	2401      	movs	r4, #1
    4eb0:	4358      	muls	r0, r3
    4eb2:	1810      	adds	r0, r2, r0
    4eb4:	780b      	ldrb	r3, [r1, #0]
    4eb6:	3101      	adds	r1, #1
    4eb8:	e7e4      	b.n	4e84 <_strtoul+0x54>
    4eba:	2b30      	cmp	r3, #48	; 0x30
    4ebc:	d04b      	beq.n	4f56 <_strtoul+0x126>
    4ebe:	f649 1699 	movw	r6, #39321	; 0x9999
    4ec2:	f240 0905 	movw	r9, #5
    4ec6:	f240 0c0a 	movw	ip, #10
    4eca:	250a      	movs	r5, #10
    4ecc:	f6c1 1699 	movt	r6, #6553	; 0x1999
    4ed0:	e7d6      	b.n	4e80 <_strtoul+0x50>
    4ed2:	7853      	ldrb	r3, [r2, #1]
    4ed4:	1c91      	adds	r1, r2, #2
    4ed6:	2d00      	cmp	r5, #0
    4ed8:	d0ef      	beq.n	4eba <_strtoul+0x8a>
    4eda:	e7c6      	b.n	4e6a <_strtoul+0x3a>
    4edc:	454a      	cmp	r2, r9
    4ede:	dde5      	ble.n	4eac <_strtoul+0x7c>
    4ee0:	2401      	movs	r4, #1
    4ee2:	4264      	negs	r4, r4
    4ee4:	e7e6      	b.n	4eb4 <_strtoul+0x84>
    4ee6:	2237      	movs	r2, #55	; 0x37
    4ee8:	e7d8      	b.n	4e9c <_strtoul+0x6c>
    4eea:	2c00      	cmp	r4, #0
    4eec:	db0e      	blt.n	4f0c <_strtoul+0xdc>
    4eee:	4643      	mov	r3, r8
    4ef0:	b103      	cbz	r3, 4ef4 <_strtoul+0xc4>
    4ef2:	4240      	negs	r0, r0
    4ef4:	4653      	mov	r3, sl
    4ef6:	b11b      	cbz	r3, 4f00 <_strtoul+0xd0>
    4ef8:	b96c      	cbnz	r4, 4f16 <_strtoul+0xe6>
    4efa:	4653      	mov	r3, sl
    4efc:	465a      	mov	r2, fp
    4efe:	601a      	str	r2, [r3, #0]
    4f00:	bcf0      	pop	{r4, r5, r6, r7}
    4f02:	46bb      	mov	fp, r7
    4f04:	46b2      	mov	sl, r6
    4f06:	46a9      	mov	r9, r5
    4f08:	46a0      	mov	r8, r4
    4f0a:	bdf0      	pop	{r4, r5, r6, r7, pc}
    4f0c:	2001      	movs	r0, #1
    4f0e:	4653      	mov	r3, sl
    4f10:	4240      	negs	r0, r0
    4f12:	2b00      	cmp	r3, #0
    4f14:	d0f4      	beq.n	4f00 <_strtoul+0xd0>
    4f16:	1e4b      	subs	r3, r1, #1
    4f18:	469b      	mov	fp, r3
    4f1a:	e7ee      	b.n	4efa <_strtoul+0xca>
    4f1c:	f240 0801 	movw	r8, #1
    4f20:	7853      	ldrb	r3, [r2, #1]
    4f22:	1c91      	adds	r1, r2, #2
    4f24:	2d00      	cmp	r5, #0
    4f26:	d0c8      	beq.n	4eba <_strtoul+0x8a>
    4f28:	e79f      	b.n	4e6a <_strtoul+0x3a>
    4f2a:	2b30      	cmp	r3, #48	; 0x30
    4f2c:	d104      	bne.n	4f38 <_strtoul+0x108>
    4f2e:	2420      	movs	r4, #32
    4f30:	780a      	ldrb	r2, [r1, #0]
    4f32:	43a2      	bics	r2, r4
    4f34:	2a58      	cmp	r2, #88	; 0x58
    4f36:	d005      	beq.n	4f44 <_strtoul+0x114>
    4f38:	f240 090f 	movw	r9, #15
    4f3c:	f240 0c10 	movw	ip, #16
    4f40:	4e0b      	ldr	r6, [pc, #44]	; (4f70 <_strtoul+0x140>)
    4f42:	e79d      	b.n	4e80 <_strtoul+0x50>
    4f44:	784b      	ldrb	r3, [r1, #1]
    4f46:	f240 090f 	movw	r9, #15
    4f4a:	f240 0c10 	movw	ip, #16
    4f4e:	2510      	movs	r5, #16
    4f50:	4e07      	ldr	r6, [pc, #28]	; (4f70 <_strtoul+0x140>)
    4f52:	3102      	adds	r1, #2
    4f54:	e794      	b.n	4e80 <_strtoul+0x50>
    4f56:	2420      	movs	r4, #32
    4f58:	780a      	ldrb	r2, [r1, #0]
    4f5a:	43a2      	bics	r2, r4
    4f5c:	2a58      	cmp	r2, #88	; 0x58
    4f5e:	d0f1      	beq.n	4f44 <_strtoul+0x114>
    4f60:	f240 0907 	movw	r9, #7
    4f64:	f240 0c08 	movw	ip, #8
    4f68:	2508      	movs	r5, #8
    4f6a:	4e02      	ldr	r6, [pc, #8]	; (4f74 <_strtoul+0x144>)
    4f6c:	e788      	b.n	4e80 <_strtoul+0x50>
    4f6e:	46c0      	nop			; (mov r8, r8)
    4f70:	0fffffff 	.word	0x0fffffff
    4f74:	1fffffff 	.word	0x1fffffff

00004f78 <_strupr>:
    4f78:	7803      	ldrb	r3, [r0, #0]
    4f7a:	b153      	cbz	r3, 4f92 <_strupr+0x1a>
    4f7c:	0002      	movs	r2, r0
    4f7e:	0019      	movs	r1, r3
    4f80:	3961      	subs	r1, #97	; 0x61
    4f82:	2919      	cmp	r1, #25
    4f84:	d801      	bhi.n	4f8a <_strupr+0x12>
    4f86:	3b20      	subs	r3, #32
    4f88:	7013      	strb	r3, [r2, #0]
    4f8a:	7853      	ldrb	r3, [r2, #1]
    4f8c:	3201      	adds	r2, #1
    4f8e:	2b00      	cmp	r3, #0
    4f90:	d1f5      	bne.n	4f7e <_strupr+0x6>
    4f92:	4770      	bx	lr

00004f94 <__wrap_memset>:
    4f94:	f644 43a9 	movw	r3, #19625	; 0x4ca9
    4f98:	b510      	push	{r4, lr}
    4f9a:	f2c0 0300 	movt	r3, #0
    4f9e:	4798      	blx	r3
    4fa0:	bd10      	pop	{r4, pc}
    4fa2:	46c0      	nop			; (mov r8, r8)

00004fa4 <__wrap_memcpy>:
    4fa4:	f644 335d 	movw	r3, #19293	; 0x4b5d
    4fa8:	b510      	push	{r4, lr}
    4faa:	f2c0 0300 	movt	r3, #0
    4fae:	4798      	blx	r3
    4fb0:	bd10      	pop	{r4, pc}
    4fb2:	46c0      	nop			; (mov r8, r8)

00004fb4 <__wrap_memcmp>:
    4fb4:	f644 3319 	movw	r3, #19225	; 0x4b19
    4fb8:	b510      	push	{r4, lr}
    4fba:	f2c0 0300 	movt	r3, #0
    4fbe:	4798      	blx	r3
    4fc0:	bd10      	pop	{r4, pc}
    4fc2:	46c0      	nop			; (mov r8, r8)

00004fc4 <__wrap_memchr>:
    4fc4:	f644 23a1 	movw	r3, #19105	; 0x4aa1
    4fc8:	b510      	push	{r4, lr}
    4fca:	f2c0 0300 	movt	r3, #0
    4fce:	4798      	blx	r3
    4fd0:	bd10      	pop	{r4, pc}
    4fd2:	46c0      	nop			; (mov r8, r8)

00004fd4 <__wrap_memmove>:
    4fd4:	f644 33fd 	movw	r3, #19453	; 0x4bfd
    4fd8:	b510      	push	{r4, lr}
    4fda:	f2c0 0300 	movt	r3, #0
    4fde:	4798      	blx	r3
    4fe0:	bd10      	pop	{r4, pc}
    4fe2:	46c0      	nop			; (mov r8, r8)

00004fe4 <__wrap_strlen>:
    4fe4:	f644 53d9 	movw	r3, #19929	; 0x4dd9
    4fe8:	b510      	push	{r4, lr}
    4fea:	f2c0 0300 	movt	r3, #0
    4fee:	4798      	blx	r3
    4ff0:	bd10      	pop	{r4, pc}
    4ff2:	46c0      	nop			; (mov r8, r8)

00004ff4 <__wrap_strcmp>:
    4ff4:	f644 5349 	movw	r3, #19785	; 0x4d49
    4ff8:	b510      	push	{r4, lr}
    4ffa:	f2c0 0300 	movt	r3, #0
    4ffe:	4798      	blx	r3
    5000:	bd10      	pop	{r4, pc}
    5002:	46c0      	nop			; (mov r8, r8)

00005004 <__wrap_stricmp>:
    5004:	f644 53a5 	movw	r3, #19877	; 0x4da5
    5008:	b510      	push	{r4, lr}
    500a:	f2c0 0300 	movt	r3, #0
    500e:	4798      	blx	r3
    5010:	bd10      	pop	{r4, pc}
    5012:	46c0      	nop			; (mov r8, r8)

00005014 <__wrap_strupr>:
    5014:	f644 7379 	movw	r3, #20345	; 0x4f79
    5018:	b510      	push	{r4, lr}
    501a:	f2c0 0300 	movt	r3, #0
    501e:	4798      	blx	r3
    5020:	bd10      	pop	{r4, pc}
    5022:	46c0      	nop			; (mov r8, r8)

00005024 <__wrap_strtoul>:
    5024:	f644 6331 	movw	r3, #20017	; 0x4e31
    5028:	b510      	push	{r4, lr}
    502a:	f2c0 0300 	movt	r3, #0
    502e:	4798      	blx	r3
    5030:	bd10      	pop	{r4, pc}
    5032:	46c0      	nop			; (mov r8, r8)

00005034 <H2CHDL_BTInfo_8730A_ROM>:
    5034:	b510      	push	{r4, lr}
    5036:	4b03      	ldr	r3, [pc, #12]	; (5044 <H2CHDL_BTInfo_8730A_ROM+0x10>)
    5038:	0002      	movs	r2, r0
    503a:	2101      	movs	r1, #1
    503c:	2023      	movs	r0, #35	; 0x23
    503e:	681b      	ldr	r3, [r3, #0]
    5040:	4798      	blx	r3
    5042:	bd10      	pop	{r4, pc}
    5044:	23000920 	.word	0x23000920

00005048 <H2CHDL_ForceBTTxpwr_8730A_ROM>:
    5048:	b510      	push	{r4, lr}
    504a:	4b03      	ldr	r3, [pc, #12]	; (5058 <H2CHDL_ForceBTTxpwr_8730A_ROM+0x10>)
    504c:	0002      	movs	r2, r0
    504e:	2101      	movs	r1, #1
    5050:	2017      	movs	r0, #23
    5052:	681b      	ldr	r3, [r3, #0]
    5054:	4798      	blx	r3
    5056:	bd10      	pop	{r4, pc}
    5058:	23000920 	.word	0x23000920

0000505c <H2CHDL_BTIgnoreWlanAct_8730A_ROM>:
    505c:	b510      	push	{r4, lr}
    505e:	4b03      	ldr	r3, [pc, #12]	; (506c <H2CHDL_BTIgnoreWlanAct_8730A_ROM+0x10>)
    5060:	0002      	movs	r2, r0
    5062:	2101      	movs	r1, #1
    5064:	201b      	movs	r0, #27
    5066:	681b      	ldr	r3, [r3, #0]
    5068:	4798      	blx	r3
    506a:	bd10      	pop	{r4, pc}
    506c:	23000920 	.word	0x23000920

00005070 <H2CHDL_AntSelReverse_8730A_ROM>:
    5070:	4b03      	ldr	r3, [pc, #12]	; (5080 <H2CHDL_AntSelReverse_8730A_ROM+0x10>)
    5072:	7802      	ldrb	r2, [r0, #0]
    5074:	681b      	ldr	r3, [r3, #0]
    5076:	701a      	strb	r2, [r3, #0]
    5078:	7842      	ldrb	r2, [r0, #1]
    507a:	705a      	strb	r2, [r3, #1]
    507c:	4770      	bx	lr
    507e:	46c0      	nop			; (mov r8, r8)
    5080:	23000a10 	.word	0x23000a10

00005084 <H2CHDL_WLOpmode_8730A_ROM>:
    5084:	b510      	push	{r4, lr}
    5086:	4b07      	ldr	r3, [pc, #28]	; (50a4 <H2CHDL_WLOpmode_8730A_ROM+0x20>)
    5088:	7801      	ldrb	r1, [r0, #0]
    508a:	681b      	ldr	r3, [r3, #0]
    508c:	0002      	movs	r2, r0
    508e:	7019      	strb	r1, [r3, #0]
    5090:	7841      	ldrb	r1, [r0, #1]
    5092:	7059      	strb	r1, [r3, #1]
    5094:	7881      	ldrb	r1, [r0, #2]
    5096:	2011      	movs	r0, #17
    5098:	7099      	strb	r1, [r3, #2]
    509a:	4b03      	ldr	r3, [pc, #12]	; (50a8 <H2CHDL_WLOpmode_8730A_ROM+0x24>)
    509c:	2103      	movs	r1, #3
    509e:	681b      	ldr	r3, [r3, #0]
    50a0:	4798      	blx	r3
    50a2:	bd10      	pop	{r4, pc}
    50a4:	23000a0c 	.word	0x23000a0c
    50a8:	23000920 	.word	0x23000920

000050ac <H2CHDL_BTMpH2C_8730A_ROM>:
    50ac:	b507      	push	{r0, r1, r2, lr}
    50ae:	466a      	mov	r2, sp
    50b0:	7803      	ldrb	r3, [r0, #0]
    50b2:	2107      	movs	r1, #7
    50b4:	7013      	strb	r3, [r2, #0]
    50b6:	7843      	ldrb	r3, [r0, #1]
    50b8:	7053      	strb	r3, [r2, #1]
    50ba:	7883      	ldrb	r3, [r0, #2]
    50bc:	7093      	strb	r3, [r2, #2]
    50be:	78c3      	ldrb	r3, [r0, #3]
    50c0:	70d3      	strb	r3, [r2, #3]
    50c2:	7903      	ldrb	r3, [r0, #4]
    50c4:	2030      	movs	r0, #48	; 0x30
    50c6:	7113      	strb	r3, [r2, #4]
    50c8:	2300      	movs	r3, #0
    50ca:	7153      	strb	r3, [r2, #5]
    50cc:	7193      	strb	r3, [r2, #6]
    50ce:	4b02      	ldr	r3, [pc, #8]	; (50d8 <H2CHDL_BTMpH2C_8730A_ROM+0x2c>)
    50d0:	681b      	ldr	r3, [r3, #0]
    50d2:	4798      	blx	r3
    50d4:	bd07      	pop	{r0, r1, r2, pc}
    50d6:	46c0      	nop			; (mov r8, r8)
    50d8:	23000920 	.word	0x23000920

000050dc <H2CHDL_BTControl_8730A_ROM>:
    50dc:	b510      	push	{r4, lr}
    50de:	4b03      	ldr	r3, [pc, #12]	; (50ec <H2CHDL_BTControl_8730A_ROM+0x10>)
    50e0:	0002      	movs	r2, r0
    50e2:	2101      	movs	r1, #1
    50e4:	2028      	movs	r0, #40	; 0x28
    50e6:	681b      	ldr	r3, [r3, #0]
    50e8:	4798      	blx	r3
    50ea:	bd10      	pop	{r4, pc}
    50ec:	23000920 	.word	0x23000920

000050f0 <H2CHDL_BT_Page_Scan_Interval_8730A_ROM>:
    50f0:	b510      	push	{r4, lr}
    50f2:	4b06      	ldr	r3, [pc, #24]	; (510c <H2CHDL_BT_Page_Scan_Interval_8730A_ROM+0x1c>)
    50f4:	7801      	ldrb	r1, [r0, #0]
    50f6:	681b      	ldr	r3, [r3, #0]
    50f8:	0002      	movs	r2, r0
    50fa:	7019      	strb	r1, [r3, #0]
    50fc:	7841      	ldrb	r1, [r0, #1]
    50fe:	2038      	movs	r0, #56	; 0x38
    5100:	7059      	strb	r1, [r3, #1]
    5102:	4b03      	ldr	r3, [pc, #12]	; (5110 <H2CHDL_BT_Page_Scan_Interval_8730A_ROM+0x20>)
    5104:	2102      	movs	r1, #2
    5106:	681b      	ldr	r3, [r3, #0]
    5108:	4798      	blx	r3
    510a:	bd10      	pop	{r4, pc}
    510c:	23000a08 	.word	0x23000a08
    5110:	23000920 	.word	0x23000920

00005114 <H2CHDL_GNT_BT_Ctrl_8730A_ROM>:
    5114:	7803      	ldrb	r3, [r0, #0]
    5116:	b12b      	cbz	r3, 5124 <H2CHDL_GNT_BT_Ctrl_8730A_ROM+0x10>
    5118:	2318      	movs	r3, #24
    511a:	4a07      	ldr	r2, [pc, #28]	; (5138 <H2CHDL_GNT_BT_Ctrl_8730A_ROM+0x24>)
    511c:	7811      	ldrb	r1, [r2, #0]
    511e:	430b      	orrs	r3, r1
    5120:	7013      	strb	r3, [r2, #0]
    5122:	4770      	bx	lr
    5124:	4b05      	ldr	r3, [pc, #20]	; (513c <H2CHDL_GNT_BT_Ctrl_8730A_ROM+0x28>)
    5126:	681b      	ldr	r3, [r3, #0]
    5128:	781b      	ldrb	r3, [r3, #0]
    512a:	071b      	lsls	r3, r3, #28
    512c:	d4f9      	bmi.n	5122 <H2CHDL_GNT_BT_Ctrl_8730A_ROM+0xe>
    512e:	2118      	movs	r1, #24
    5130:	4a01      	ldr	r2, [pc, #4]	; (5138 <H2CHDL_GNT_BT_Ctrl_8730A_ROM+0x24>)
    5132:	7813      	ldrb	r3, [r2, #0]
    5134:	438b      	bics	r3, r1
    5136:	e7f3      	b.n	5120 <H2CHDL_GNT_BT_Ctrl_8730A_ROM+0xc>
    5138:	43000765 	.word	0x43000765
    513c:	23000a18 	.word	0x23000a18

00005140 <H2CHDL_BT_Only_Test_8730A_ROM>:
    5140:	2201      	movs	r2, #1
    5142:	b510      	push	{r4, lr}
    5144:	4b08      	ldr	r3, [pc, #32]	; (5168 <H2CHDL_BT_Only_Test_8730A_ROM+0x28>)
    5146:	7801      	ldrb	r1, [r0, #0]
    5148:	681c      	ldr	r4, [r3, #0]
    514a:	4011      	ands	r1, r2
    514c:	7823      	ldrb	r3, [r4, #0]
    514e:	4393      	bics	r3, r2
    5150:	430b      	orrs	r3, r1
    5152:	0018      	movs	r0, r3
    5154:	4905      	ldr	r1, [pc, #20]	; (516c <H2CHDL_BT_Only_Test_8730A_ROM+0x2c>)
    5156:	7023      	strb	r3, [r4, #0]
    5158:	4010      	ands	r0, r2
    515a:	4213      	tst	r3, r2
    515c:	d002      	beq.n	5164 <H2CHDL_BT_Only_Test_8730A_ROM+0x24>
    515e:	2318      	movs	r3, #24
    5160:	700b      	strb	r3, [r1, #0]
    5162:	bd10      	pop	{r4, pc}
    5164:	7008      	strb	r0, [r1, #0]
    5166:	e7fc      	b.n	5162 <H2CHDL_BT_Only_Test_8730A_ROM+0x22>
    5168:	23000a00 	.word	0x23000a00
    516c:	43000765 	.word	0x43000765

00005170 <H2CHDL_WL_Port_ID_8730A_ROM>:
    5170:	7803      	ldrb	r3, [r0, #0]
    5172:	2007      	movs	r0, #7
    5174:	4a04      	ldr	r2, [pc, #16]	; (5188 <H2CHDL_WL_Port_ID_8730A_ROM+0x18>)
    5176:	4003      	ands	r3, r0
    5178:	6811      	ldr	r1, [r2, #0]
    517a:	001a      	movs	r2, r3
    517c:	780b      	ldrb	r3, [r1, #0]
    517e:	4383      	bics	r3, r0
    5180:	4313      	orrs	r3, r2
    5182:	700b      	strb	r3, [r1, #0]
    5184:	4770      	bx	lr
    5186:	46c0      	nop			; (mov r8, r8)
    5188:	230009f8 	.word	0x230009f8

0000518c <C2HBTLoopback_8730A_ROM>:
    518c:	220a      	movs	r2, #10
    518e:	b500      	push	{lr}
    5190:	b08b      	sub	sp, #44	; 0x2c
    5192:	0003      	movs	r3, r0
    5194:	a801      	add	r0, sp, #4
    5196:	7002      	strb	r2, [r0, #0]
    5198:	466a      	mov	r2, sp
    519a:	2106      	movs	r1, #6
    519c:	3207      	adds	r2, #7
    519e:	77d1      	strb	r1, [r2, #31]
    51a0:	781a      	ldrb	r2, [r3, #0]
    51a2:	7082      	strb	r2, [r0, #2]
    51a4:	785a      	ldrb	r2, [r3, #1]
    51a6:	70c2      	strb	r2, [r0, #3]
    51a8:	789a      	ldrb	r2, [r3, #2]
    51aa:	7102      	strb	r2, [r0, #4]
    51ac:	78da      	ldrb	r2, [r3, #3]
    51ae:	7142      	strb	r2, [r0, #5]
    51b0:	791a      	ldrb	r2, [r3, #4]
    51b2:	795b      	ldrb	r3, [r3, #5]
    51b4:	7182      	strb	r2, [r0, #6]
    51b6:	71c3      	strb	r3, [r0, #7]
    51b8:	4b04      	ldr	r3, [pc, #16]	; (51cc <C2HBTLoopback_8730A_ROM+0x40>)
    51ba:	681b      	ldr	r3, [r3, #0]
    51bc:	4798      	blx	r3
    51be:	4b04      	ldr	r3, [pc, #16]	; (51d0 <C2HBTLoopback_8730A_ROM+0x44>)
    51c0:	2004      	movs	r0, #4
    51c2:	681b      	ldr	r3, [r3, #0]
    51c4:	4798      	blx	r3
    51c6:	b00b      	add	sp, #44	; 0x2c
    51c8:	bd00      	pop	{pc}
    51ca:	46c0      	nop			; (mov r8, r8)
    51cc:	230007b0 	.word	0x230007b0
    51d0:	230009b0 	.word	0x230009b0

000051d4 <C2HBTMpRpt_8730A_ROM>:
    51d4:	b530      	push	{r4, r5, lr}
    51d6:	240b      	movs	r4, #11
    51d8:	b08b      	sub	sp, #44	; 0x2c
    51da:	0003      	movs	r3, r0
    51dc:	a801      	add	r0, sp, #4
    51de:	7004      	strb	r4, [r0, #0]
    51e0:	466c      	mov	r4, sp
    51e2:	2507      	movs	r5, #7
    51e4:	3407      	adds	r4, #7
    51e6:	77e5      	strb	r5, [r4, #31]
    51e8:	7083      	strb	r3, [r0, #2]
    51ea:	b1b3      	cbz	r3, 521a <C2HBTMpRpt_8730A_ROM+0x46>
    51ec:	4b0e      	ldr	r3, [pc, #56]	; (5228 <C2HBTMpRpt_8730A_ROM+0x54>)
    51ee:	681b      	ldr	r3, [r3, #0]
    51f0:	78da      	ldrb	r2, [r3, #3]
    51f2:	70c2      	strb	r2, [r0, #3]
    51f4:	785a      	ldrb	r2, [r3, #1]
    51f6:	7102      	strb	r2, [r0, #4]
    51f8:	791a      	ldrb	r2, [r3, #4]
    51fa:	7142      	strb	r2, [r0, #5]
    51fc:	795a      	ldrb	r2, [r3, #5]
    51fe:	7182      	strb	r2, [r0, #6]
    5200:	799a      	ldrb	r2, [r3, #6]
    5202:	71c2      	strb	r2, [r0, #7]
    5204:	79db      	ldrb	r3, [r3, #7]
    5206:	7203      	strb	r3, [r0, #8]
    5208:	4b08      	ldr	r3, [pc, #32]	; (522c <C2HBTMpRpt_8730A_ROM+0x58>)
    520a:	681b      	ldr	r3, [r3, #0]
    520c:	4798      	blx	r3
    520e:	4b08      	ldr	r3, [pc, #32]	; (5230 <C2HBTMpRpt_8730A_ROM+0x5c>)
    5210:	2004      	movs	r0, #4
    5212:	681b      	ldr	r3, [r3, #0]
    5214:	4798      	blx	r3
    5216:	b00b      	add	sp, #44	; 0x2c
    5218:	bd30      	pop	{r4, r5, pc}
    521a:	70c1      	strb	r1, [r0, #3]
    521c:	7102      	strb	r2, [r0, #4]
    521e:	7143      	strb	r3, [r0, #5]
    5220:	7183      	strb	r3, [r0, #6]
    5222:	71c3      	strb	r3, [r0, #7]
    5224:	e7ef      	b.n	5206 <C2HBTMpRpt_8730A_ROM+0x32>
    5226:	46c0      	nop			; (mov r8, r8)
    5228:	230009f4 	.word	0x230009f4
    522c:	230007b0 	.word	0x230007b0
    5230:	230009b0 	.word	0x230009b0

00005234 <C2HBTMailBoxStatus_8730A_ROM>:
    5234:	b500      	push	{lr}
    5236:	b08b      	sub	sp, #44	; 0x2c
    5238:	466a      	mov	r2, sp
    523a:	2315      	movs	r3, #21
    523c:	7113      	strb	r3, [r2, #4]
    523e:	466b      	mov	r3, sp
    5240:	2201      	movs	r2, #1
    5242:	3307      	adds	r3, #7
    5244:	77da      	strb	r2, [r3, #31]
    5246:	466b      	mov	r3, sp
    5248:	7198      	strb	r0, [r3, #6]
    524a:	4b05      	ldr	r3, [pc, #20]	; (5260 <C2HBTMailBoxStatus_8730A_ROM+0x2c>)
    524c:	a801      	add	r0, sp, #4
    524e:	681b      	ldr	r3, [r3, #0]
    5250:	4798      	blx	r3
    5252:	4b04      	ldr	r3, [pc, #16]	; (5264 <C2HBTMailBoxStatus_8730A_ROM+0x30>)
    5254:	2004      	movs	r0, #4
    5256:	681b      	ldr	r3, [r3, #0]
    5258:	4798      	blx	r3
    525a:	b00b      	add	sp, #44	; 0x2c
    525c:	bd00      	pop	{pc}
    525e:	46c0      	nop			; (mov r8, r8)
    5260:	230007b0 	.word	0x230007b0
    5264:	230009b0 	.word	0x230009b0

00005268 <InitBTCoexTimer_8730A_ROM>:
    5268:	b513      	push	{r0, r1, r4, lr}
    526a:	4b05      	ldr	r3, [pc, #20]	; (5280 <InitBTCoexTimer_8730A_ROM+0x18>)
    526c:	9000      	str	r0, [sp, #0]
    526e:	681c      	ldr	r4, [r3, #0]
    5270:	2300      	movs	r3, #0
    5272:	001a      	movs	r2, r3
    5274:	0019      	movs	r1, r3
    5276:	f240 106c 	movw	r0, #364	; 0x16c
    527a:	47a0      	blx	r4
    527c:	bd13      	pop	{r0, r1, r4, pc}
    527e:	46c0      	nop			; (mov r8, r8)
    5280:	2300078c 	.word	0x2300078c

00005284 <Write778_8730A_ROM>:
    5284:	4b0b      	ldr	r3, [pc, #44]	; (52b4 <Write778_8730A_ROM+0x30>)
    5286:	4a0c      	ldr	r2, [pc, #48]	; (52b8 <Write778_8730A_ROM+0x34>)
    5288:	681b      	ldr	r3, [r3, #0]
    528a:	7819      	ldrb	r1, [r3, #0]
    528c:	2301      	movs	r3, #1
    528e:	4219      	tst	r1, r3
    5290:	d002      	beq.n	5298 <Write778_8730A_ROM+0x14>
    5292:	3308      	adds	r3, #8
    5294:	7013      	strb	r3, [r2, #0]
    5296:	4770      	bx	lr
    5298:	4908      	ldr	r1, [pc, #32]	; (52bc <Write778_8730A_ROM+0x38>)
    529a:	6809      	ldr	r1, [r1, #0]
    529c:	7809      	ldrb	r1, [r1, #0]
    529e:	4219      	tst	r1, r3
    52a0:	d006      	beq.n	52b0 <Write778_8730A_ROM+0x2c>
    52a2:	3803      	subs	r0, #3
    52a4:	b2c3      	uxtb	r3, r0
    52a6:	2002      	movs	r0, #2
    52a8:	2b0a      	cmp	r3, #10
    52aa:	d801      	bhi.n	52b0 <Write778_8730A_ROM+0x2c>
    52ac:	4904      	ldr	r1, [pc, #16]	; (52c0 <Write778_8730A_ROM+0x3c>)
    52ae:	5cc8      	ldrb	r0, [r1, r3]
    52b0:	7010      	strb	r0, [r2, #0]
    52b2:	e7f0      	b.n	5296 <Write778_8730A_ROM+0x12>
    52b4:	23000a00 	.word	0x23000a00
    52b8:	43000778 	.word	0x43000778
    52bc:	230009fc 	.word	0x230009fc
    52c0:	0000c595 	.word	0x0000c595

000052c4 <TdmaChangeCoexTable_8730A_ROM>:
    52c4:	4b0d      	ldr	r3, [pc, #52]	; (52fc <TdmaChangeCoexTable_8730A_ROM+0x38>)
    52c6:	681b      	ldr	r3, [r3, #0]
    52c8:	2803      	cmp	r0, #3
    52ca:	d10b      	bne.n	52e4 <TdmaChangeCoexTable_8730A_ROM+0x20>
    52cc:	78da      	ldrb	r2, [r3, #3]
    52ce:	4b0c      	ldr	r3, [pc, #48]	; (5300 <TdmaChangeCoexTable_8730A_ROM+0x3c>)
    52d0:	0652      	lsls	r2, r2, #25
    52d2:	d503      	bpl.n	52dc <TdmaChangeCoexTable_8730A_ROM+0x18>
    52d4:	781a      	ldrb	r2, [r3, #0]
    52d6:	4382      	bics	r2, r0
    52d8:	701a      	strb	r2, [r3, #0]
    52da:	e002      	b.n	52e2 <TdmaChangeCoexTable_8730A_ROM+0x1e>
    52dc:	781a      	ldrb	r2, [r3, #0]
    52de:	4310      	orrs	r0, r2
    52e0:	7018      	strb	r0, [r3, #0]
    52e2:	4770      	bx	lr
    52e4:	791b      	ldrb	r3, [r3, #4]
    52e6:	06db      	lsls	r3, r3, #27
    52e8:	d5fb      	bpl.n	52e2 <TdmaChangeCoexTable_8730A_ROM+0x1e>
    52ea:	4b05      	ldr	r3, [pc, #20]	; (5300 <TdmaChangeCoexTable_8730A_ROM+0x3c>)
    52ec:	2203      	movs	r2, #3
    52ee:	7819      	ldrb	r1, [r3, #0]
    52f0:	b910      	cbnz	r0, 52f8 <TdmaChangeCoexTable_8730A_ROM+0x34>
    52f2:	4391      	bics	r1, r2
    52f4:	7019      	strb	r1, [r3, #0]
    52f6:	e7f4      	b.n	52e2 <TdmaChangeCoexTable_8730A_ROM+0x1e>
    52f8:	430a      	orrs	r2, r1
    52fa:	e7ed      	b.n	52d8 <TdmaChangeCoexTable_8730A_ROM+0x14>
    52fc:	23000a14 	.word	0x23000a14
    5300:	430006cc 	.word	0x430006cc

00005304 <BTNullSetting_8730A_ROM>:
    5304:	b570      	push	{r4, r5, r6, lr}
    5306:	000c      	movs	r4, r1
    5308:	001d      	movs	r5, r3
    530a:	4b12      	ldr	r3, [pc, #72]	; (5354 <BTNullSetting_8730A_ROM+0x50>)
    530c:	0016      	movs	r6, r2
    530e:	681b      	ldr	r3, [r3, #0]
    5310:	4798      	blx	r3
    5312:	2180      	movs	r1, #128	; 0x80
    5314:	4b10      	ldr	r3, [pc, #64]	; (5358 <BTNullSetting_8730A_ROM+0x54>)
    5316:	18e3      	adds	r3, r4, r3
    5318:	781a      	ldrb	r2, [r3, #0]
    531a:	430a      	orrs	r2, r1
    531c:	701a      	strb	r2, [r3, #0]
    531e:	781a      	ldrb	r2, [r3, #0]
    5320:	3970      	subs	r1, #112	; 0x70
    5322:	438a      	bics	r2, r1
    5324:	701a      	strb	r2, [r3, #0]
    5326:	4b0d      	ldr	r3, [pc, #52]	; (535c <BTNullSetting_8730A_ROM+0x58>)
    5328:	18e1      	adds	r1, r4, r3
    532a:	2302      	movs	r3, #2
    532c:	780a      	ldrb	r2, [r1, #0]
    532e:	4313      	orrs	r3, r2
    5330:	2203      	movs	r2, #3
    5332:	700b      	strb	r3, [r1, #0]
    5334:	780b      	ldrb	r3, [r1, #0]
    5336:	4013      	ands	r3, r2
    5338:	700b      	strb	r3, [r1, #0]
    533a:	4b09      	ldr	r3, [pc, #36]	; (5360 <BTNullSetting_8730A_ROM+0x5c>)
    533c:	18e3      	adds	r3, r4, r3
    533e:	701e      	strb	r6, [r3, #0]
    5340:	4b08      	ldr	r3, [pc, #32]	; (5364 <BTNullSetting_8730A_ROM+0x60>)
    5342:	18e3      	adds	r3, r4, r3
    5344:	701d      	strb	r5, [r3, #0]
    5346:	4b08      	ldr	r3, [pc, #32]	; (5368 <BTNullSetting_8730A_ROM+0x64>)
    5348:	18e4      	adds	r4, r4, r3
    534a:	7823      	ldrb	r3, [r4, #0]
    534c:	3302      	adds	r3, #2
    534e:	b2db      	uxtb	r3, r3
    5350:	7023      	strb	r3, [r4, #0]
    5352:	bd70      	pop	{r4, r5, r6, pc}
    5354:	230007c4 	.word	0x230007c4
    5358:	4302000d 	.word	0x4302000d
    535c:	43020012 	.word	0x43020012
    5360:	4302002a 	.word	0x4302002a
    5364:	4302002b 	.word	0x4302002b
    5368:	4302002c 	.word	0x4302002c

0000536c <H2CInQueue_8730A_ROM>:
    536c:	b5f0      	push	{r4, r5, r6, r7, lr}
    536e:	4b2f      	ldr	r3, [pc, #188]	; (542c <H2CInQueue_8730A_ROM+0xc0>)
    5370:	2701      	movs	r7, #1
    5372:	7818      	ldrb	r0, [r3, #0]
    5374:	230f      	movs	r3, #15
    5376:	4e2e      	ldr	r6, [pc, #184]	; (5430 <H2CInQueue_8730A_ROM+0xc4>)
    5378:	4a2e      	ldr	r2, [pc, #184]	; (5434 <H2CInQueue_8730A_ROM+0xc8>)
    537a:	4018      	ands	r0, r3
    537c:	b180      	cbz	r0, 53a0 <H2CInQueue_8730A_ROM+0x34>
    537e:	6833      	ldr	r3, [r6, #0]
    5380:	0019      	movs	r1, r3
    5382:	3150      	adds	r1, #80	; 0x50
    5384:	780c      	ldrb	r4, [r1, #0]
    5386:	3101      	adds	r1, #1
    5388:	7809      	ldrb	r1, [r1, #0]
    538a:	b90c      	cbnz	r4, 5390 <H2CInQueue_8730A_ROM+0x24>
    538c:	2909      	cmp	r1, #9
    538e:	d002      	beq.n	5396 <H2CInQueue_8730A_ROM+0x2a>
    5390:	3c01      	subs	r4, #1
    5392:	42a1      	cmp	r1, r4
    5394:	d105      	bne.n	53a2 <H2CInQueue_8730A_ROM+0x36>
    5396:	2301      	movs	r3, #1
    5398:	4a27      	ldr	r2, [pc, #156]	; (5438 <H2CInQueue_8730A_ROM+0xcc>)
    539a:	7811      	ldrb	r1, [r2, #0]
    539c:	430b      	orrs	r3, r1
    539e:	7013      	strb	r3, [r2, #0]
    53a0:	bdf0      	pop	{r4, r5, r6, r7, pc}
    53a2:	0005      	movs	r5, r0
    53a4:	7814      	ldrb	r4, [r2, #0]
    53a6:	40e5      	lsrs	r5, r4
    53a8:	423d      	tst	r5, r7
    53aa:	d02d      	beq.n	5408 <H2CInQueue_8730A_ROM+0x9c>
    53ac:	2400      	movs	r4, #0
    53ae:	00c9      	lsls	r1, r1, #3
    53b0:	185b      	adds	r3, r3, r1
    53b2:	2586      	movs	r5, #134	; 0x86
    53b4:	7811      	ldrb	r1, [r2, #0]
    53b6:	05ed      	lsls	r5, r5, #23
    53b8:	3174      	adds	r1, #116	; 0x74
    53ba:	0089      	lsls	r1, r1, #2
    53bc:	1909      	adds	r1, r1, r4
    53be:	1949      	adds	r1, r1, r5
    53c0:	7809      	ldrb	r1, [r1, #0]
    53c2:	7019      	strb	r1, [r3, #0]
    53c4:	7811      	ldrb	r1, [r2, #0]
    53c6:	3301      	adds	r3, #1
    53c8:	468c      	mov	ip, r1
    53ca:	317c      	adds	r1, #124	; 0x7c
    53cc:	0089      	lsls	r1, r1, #2
    53ce:	1909      	adds	r1, r1, r4
    53d0:	1949      	adds	r1, r1, r5
    53d2:	7809      	ldrb	r1, [r1, #0]
    53d4:	3401      	adds	r4, #1
    53d6:	70d9      	strb	r1, [r3, #3]
    53d8:	2c04      	cmp	r4, #4
    53da:	d1ea      	bne.n	53b2 <H2CInQueue_8730A_ROM+0x46>
    53dc:	4661      	mov	r1, ip
    53de:	003b      	movs	r3, r7
    53e0:	408b      	lsls	r3, r1
    53e2:	4912      	ldr	r1, [pc, #72]	; (542c <H2CInQueue_8730A_ROM+0xc0>)
    53e4:	b2db      	uxtb	r3, r3
    53e6:	700b      	strb	r3, [r1, #0]
    53e8:	2103      	movs	r1, #3
    53ea:	4398      	bics	r0, r3
    53ec:	7813      	ldrb	r3, [r2, #0]
    53ee:	3301      	adds	r3, #1
    53f0:	400b      	ands	r3, r1
    53f2:	6831      	ldr	r1, [r6, #0]
    53f4:	7013      	strb	r3, [r2, #0]
    53f6:	3151      	adds	r1, #81	; 0x51
    53f8:	780b      	ldrb	r3, [r1, #0]
    53fa:	3301      	adds	r3, #1
    53fc:	b2db      	uxtb	r3, r3
    53fe:	2b0a      	cmp	r3, #10
    5400:	d100      	bne.n	5404 <H2CInQueue_8730A_ROM+0x98>
    5402:	2300      	movs	r3, #0
    5404:	700b      	strb	r3, [r1, #0]
    5406:	e7b9      	b.n	537c <H2CInQueue_8730A_ROM+0x10>
    5408:	2302      	movs	r3, #2
    540a:	490c      	ldr	r1, [pc, #48]	; (543c <H2CInQueue_8730A_ROM+0xd0>)
    540c:	7808      	ldrb	r0, [r1, #0]
    540e:	4303      	orrs	r3, r0
    5410:	700b      	strb	r3, [r1, #0]
    5412:	2180      	movs	r1, #128	; 0x80
    5414:	7813      	ldrb	r3, [r2, #0]
    5416:	430b      	orrs	r3, r1
    5418:	4909      	ldr	r1, [pc, #36]	; (5440 <H2CInQueue_8730A_ROM+0xd4>)
    541a:	700b      	strb	r3, [r1, #0]
    541c:	7813      	ldrb	r3, [r2, #0]
    541e:	4a09      	ldr	r2, [pc, #36]	; (5444 <H2CInQueue_8730A_ROM+0xd8>)
    5420:	189b      	adds	r3, r3, r2
    5422:	009b      	lsls	r3, r3, #2
    5424:	781b      	ldrb	r3, [r3, #0]
    5426:	4a08      	ldr	r2, [pc, #32]	; (5448 <H2CInQueue_8730A_ROM+0xdc>)
    5428:	b2db      	uxtb	r3, r3
    542a:	e7b8      	b.n	539e <H2CInQueue_8730A_ROM+0x32>
    542c:	430001cc 	.word	0x430001cc
    5430:	23000a34 	.word	0x23000a34
    5434:	23000a2c 	.word	0x23000a2c
    5438:	430001c1 	.word	0x430001c1
    543c:	430001c0 	.word	0x430001c0
    5440:	4300008a 	.word	0x4300008a
    5444:	10c00074 	.word	0x10c00074
    5448:	430001c2 	.word	0x430001c2

0000544c <H2CDeQueue_8730A_ROM>:
    544c:	b510      	push	{r4, lr}
    544e:	4c15      	ldr	r4, [pc, #84]	; (54a4 <H2CDeQueue_8730A_ROM+0x58>)
    5450:	6822      	ldr	r2, [r4, #0]
    5452:	0013      	movs	r3, r2
    5454:	0011      	movs	r1, r2
    5456:	3350      	adds	r3, #80	; 0x50
    5458:	3151      	adds	r1, #81	; 0x51
    545a:	781b      	ldrb	r3, [r3, #0]
    545c:	7809      	ldrb	r1, [r1, #0]
    545e:	4299      	cmp	r1, r3
    5460:	d01d      	beq.n	549e <H2CDeQueue_8730A_ROM+0x52>
    5462:	00db      	lsls	r3, r3, #3
    5464:	5c98      	ldrb	r0, [r3, r2]
    5466:	1c59      	adds	r1, r3, #1
    5468:	4b0f      	ldr	r3, [pc, #60]	; (54a8 <H2CDeQueue_8730A_ROM+0x5c>)
    546a:	1851      	adds	r1, r2, r1
    546c:	681b      	ldr	r3, [r3, #0]
    546e:	4798      	blx	r3
    5470:	6822      	ldr	r2, [r4, #0]
    5472:	3250      	adds	r2, #80	; 0x50
    5474:	7813      	ldrb	r3, [r2, #0]
    5476:	3301      	adds	r3, #1
    5478:	b2db      	uxtb	r3, r3
    547a:	2b0a      	cmp	r3, #10
    547c:	d010      	beq.n	54a0 <H2CDeQueue_8730A_ROM+0x54>
    547e:	7013      	strb	r3, [r2, #0]
    5480:	4b0a      	ldr	r3, [pc, #40]	; (54ac <H2CDeQueue_8730A_ROM+0x60>)
    5482:	681b      	ldr	r3, [r3, #0]
    5484:	4798      	blx	r3
    5486:	6823      	ldr	r3, [r4, #0]
    5488:	001a      	movs	r2, r3
    548a:	3351      	adds	r3, #81	; 0x51
    548c:	3250      	adds	r2, #80	; 0x50
    548e:	7812      	ldrb	r2, [r2, #0]
    5490:	781b      	ldrb	r3, [r3, #0]
    5492:	429a      	cmp	r2, r3
    5494:	d003      	beq.n	549e <H2CDeQueue_8730A_ROM+0x52>
    5496:	4b06      	ldr	r3, [pc, #24]	; (54b0 <H2CDeQueue_8730A_ROM+0x64>)
    5498:	2002      	movs	r0, #2
    549a:	681b      	ldr	r3, [r3, #0]
    549c:	4798      	blx	r3
    549e:	bd10      	pop	{r4, pc}
    54a0:	2300      	movs	r3, #0
    54a2:	e7ec      	b.n	547e <H2CDeQueue_8730A_ROM+0x32>
    54a4:	23000a34 	.word	0x23000a34
    54a8:	230009a4 	.word	0x230009a4
    54ac:	23000790 	.word	0x23000790
    54b0:	230009b4 	.word	0x230009b4

000054b4 <InitC2H_8730A_ROM>:
    54b4:	4b0a      	ldr	r3, [pc, #40]	; (54e0 <InitC2H_8730A_ROM+0x2c>)
    54b6:	681a      	ldr	r2, [r3, #0]
    54b8:	2300      	movs	r3, #0
    54ba:	32ff      	adds	r2, #255	; 0xff
    54bc:	7653      	strb	r3, [r2, #25]
    54be:	7693      	strb	r3, [r2, #26]
    54c0:	4a08      	ldr	r2, [pc, #32]	; (54e4 <InitC2H_8730A_ROM+0x30>)
    54c2:	6812      	ldr	r2, [r2, #0]
    54c4:	0011      	movs	r1, r2
    54c6:	3251      	adds	r2, #81	; 0x51
    54c8:	3150      	adds	r1, #80	; 0x50
    54ca:	700b      	strb	r3, [r1, #0]
    54cc:	7013      	strb	r3, [r2, #0]
    54ce:	4a06      	ldr	r2, [pc, #24]	; (54e8 <InitC2H_8730A_ROM+0x34>)
    54d0:	7013      	strb	r3, [r2, #0]
    54d2:	4a06      	ldr	r2, [pc, #24]	; (54ec <InitC2H_8730A_ROM+0x38>)
    54d4:	330f      	adds	r3, #15
    54d6:	7811      	ldrb	r1, [r2, #0]
    54d8:	430b      	orrs	r3, r1
    54da:	7013      	strb	r3, [r2, #0]
    54dc:	4770      	bx	lr
    54de:	46c0      	nop			; (mov r8, r8)
    54e0:	23000a30 	.word	0x23000a30
    54e4:	23000a34 	.word	0x23000a34
    54e8:	23000a2c 	.word	0x23000a2c
    54ec:	430001cc 	.word	0x430001cc

000054f0 <WaitC2HOk_8730A_ROM>:
    54f0:	b570      	push	{r4, r5, r6, lr}
    54f2:	0005      	movs	r5, r0
    54f4:	f240 34e9 	movw	r4, #1001	; 0x3e9
    54f8:	2386      	movs	r3, #134	; 0x86
    54fa:	05db      	lsls	r3, r3, #23
    54fc:	18eb      	adds	r3, r5, r3
    54fe:	781b      	ldrb	r3, [r3, #0]
    5500:	b90b      	cbnz	r3, 5506 <WaitC2HOk_8730A_ROM+0x16>
    5502:	2001      	movs	r0, #1
    5504:	e008      	b.n	5518 <WaitC2HOk_8730A_ROM+0x28>
    5506:	3c01      	subs	r4, #1
    5508:	b2a4      	uxth	r4, r4
    550a:	b934      	cbnz	r4, 551a <WaitC2HOk_8730A_ROM+0x2a>
    550c:	2180      	movs	r1, #128	; 0x80
    550e:	0020      	movs	r0, r4
    5510:	4a04      	ldr	r2, [pc, #16]	; (5524 <WaitC2HOk_8730A_ROM+0x34>)
    5512:	7813      	ldrb	r3, [r2, #0]
    5514:	430b      	orrs	r3, r1
    5516:	7013      	strb	r3, [r2, #0]
    5518:	bd70      	pop	{r4, r5, r6, pc}
    551a:	200a      	movs	r0, #10
    551c:	4b02      	ldr	r3, [pc, #8]	; (5528 <WaitC2HOk_8730A_ROM+0x38>)
    551e:	4798      	blx	r3
    5520:	e7ea      	b.n	54f8 <WaitC2HOk_8730A_ROM+0x8>
    5522:	46c0      	nop			; (mov r8, r8)
    5524:	430001c0 	.word	0x430001c0
    5528:	0000030b 	.word	0x0000030b

0000552c <C2HPKTRxDESC_8730A_ROM>:
    552c:	4b0a      	ldr	r3, [pc, #40]	; (5558 <C2HPKTRxDESC_8730A_ROM+0x2c>)
    552e:	b530      	push	{r4, r5, lr}
    5530:	18ca      	adds	r2, r1, r3
    5532:	0013      	movs	r3, r2
    5534:	2500      	movs	r5, #0
    5536:	4c09      	ldr	r4, [pc, #36]	; (555c <C2HPKTRxDESC_8730A_ROM+0x30>)
    5538:	190c      	adds	r4, r1, r4
    553a:	701d      	strb	r5, [r3, #0]
    553c:	3301      	adds	r3, #1
    553e:	42a3      	cmp	r3, r4
    5540:	d1fb      	bne.n	553a <C2HPKTRxDESC_8730A_ROM+0xe>
    5542:	4b07      	ldr	r3, [pc, #28]	; (5560 <C2HPKTRxDESC_8730A_ROM+0x34>)
    5544:	0480      	lsls	r0, r0, #18
    5546:	18c9      	adds	r1, r1, r3
    5548:	2310      	movs	r3, #16
    554a:	0c80      	lsrs	r0, r0, #18
    554c:	8010      	strh	r0, [r2, #0]
    554e:	780a      	ldrb	r2, [r1, #0]
    5550:	4313      	orrs	r3, r2
    5552:	700b      	strb	r3, [r1, #0]
    5554:	bd30      	pop	{r4, r5, pc}
    5556:	46c0      	nop			; (mov r8, r8)
    5558:	43030000 	.word	0x43030000
    555c:	43030018 	.word	0x43030018
    5560:	4303000b 	.word	0x4303000b

00005564 <FillC2HContant_8730A_ROM>:
    5564:	b570      	push	{r4, r5, r6, lr}
    5566:	000c      	movs	r4, r1
    5568:	0005      	movs	r5, r0
    556a:	4a09      	ldr	r2, [pc, #36]	; (5590 <FillC2HContant_8730A_ROM+0x2c>)
    556c:	4e09      	ldr	r6, [pc, #36]	; (5594 <FillC2HContant_8730A_ROM+0x30>)
    556e:	7813      	ldrb	r3, [r2, #0]
    5570:	1c59      	adds	r1, r3, #1
    5572:	7063      	strb	r3, [r4, #1]
    5574:	4b08      	ldr	r3, [pc, #32]	; (5598 <FillC2HContant_8730A_ROM+0x34>)
    5576:	7011      	strb	r1, [r2, #0]
    5578:	18c0      	adds	r0, r0, r3
    557a:	0021      	movs	r1, r4
    557c:	2202      	movs	r2, #2
    557e:	47b0      	blx	r6
    5580:	1ce3      	adds	r3, r4, #3
    5582:	7fda      	ldrb	r2, [r3, #31]
    5584:	4b05      	ldr	r3, [pc, #20]	; (559c <FillC2HContant_8730A_ROM+0x38>)
    5586:	1ca1      	adds	r1, r4, #2
    5588:	18e8      	adds	r0, r5, r3
    558a:	47b0      	blx	r6
    558c:	bd70      	pop	{r4, r5, r6, pc}
    558e:	46c0      	nop			; (mov r8, r8)
    5590:	2300074c 	.word	0x2300074c
    5594:	00004b5d 	.word	0x00004b5d
    5598:	43030000 	.word	0x43030000
    559c:	43030002 	.word	0x43030002

000055a0 <IssueC2HPKT_8730A_ROM>:
    55a0:	4b0f      	ldr	r3, [pc, #60]	; (55e0 <IssueC2HPKT_8730A_ROM+0x40>)
    55a2:	b570      	push	{r4, r5, r6, lr}
    55a4:	681b      	ldr	r3, [r3, #0]
    55a6:	0005      	movs	r5, r0
    55a8:	f240 2096 	movw	r0, #662	; 0x296
    55ac:	4798      	blx	r3
    55ae:	b1a8      	cbz	r0, 55dc <IssueC2HPKT_8730A_ROM+0x3c>
    55b0:	4b0c      	ldr	r3, [pc, #48]	; (55e4 <IssueC2HPKT_8730A_ROM+0x44>)
    55b2:	8819      	ldrh	r1, [r3, #0]
    55b4:	4b0c      	ldr	r3, [pc, #48]	; (55e8 <IssueC2HPKT_8730A_ROM+0x48>)
    55b6:	b28c      	uxth	r4, r1
    55b8:	3101      	adds	r1, #1
    55ba:	b289      	uxth	r1, r1
    55bc:	8019      	strh	r1, [r3, #0]
    55be:	1ceb      	adds	r3, r5, #3
    55c0:	7fd8      	ldrb	r0, [r3, #31]
    55c2:	4b0a      	ldr	r3, [pc, #40]	; (55ec <IssueC2HPKT_8730A_ROM+0x4c>)
    55c4:	3002      	adds	r0, #2
    55c6:	681b      	ldr	r3, [r3, #0]
    55c8:	4798      	blx	r3
    55ca:	4b09      	ldr	r3, [pc, #36]	; (55f0 <IssueC2HPKT_8730A_ROM+0x50>)
    55cc:	3419      	adds	r4, #25
    55ce:	681b      	ldr	r3, [r3, #0]
    55d0:	0029      	movs	r1, r5
    55d2:	b2a0      	uxth	r0, r4
    55d4:	4798      	blx	r3
    55d6:	2201      	movs	r2, #1
    55d8:	4b06      	ldr	r3, [pc, #24]	; (55f4 <IssueC2HPKT_8730A_ROM+0x54>)
    55da:	701a      	strb	r2, [r3, #0]
    55dc:	bd70      	pop	{r4, r5, r6, pc}
    55de:	46c0      	nop			; (mov r8, r8)
    55e0:	2300079c 	.word	0x2300079c
    55e4:	43000116 	.word	0x43000116
    55e8:	43000294 	.word	0x43000294
    55ec:	230007a0 	.word	0x230007a0
    55f0:	230007a4 	.word	0x230007a4
    55f4:	43000296 	.word	0x43000296

000055f8 <C2HDeQueue_8730A_ROM>:
    55f8:	b510      	push	{r4, lr}
    55fa:	4c10      	ldr	r4, [pc, #64]	; (563c <C2HDeQueue_8730A_ROM+0x44>)
    55fc:	6820      	ldr	r0, [r4, #0]
    55fe:	0003      	movs	r3, r0
    5600:	33ff      	adds	r3, #255	; 0xff
    5602:	7e5a      	ldrb	r2, [r3, #25]
    5604:	7e9b      	ldrb	r3, [r3, #26]
    5606:	4293      	cmp	r3, r2
    5608:	d015      	beq.n	5636 <C2HDeQueue_8730A_ROM+0x3e>
    560a:	2323      	movs	r3, #35	; 0x23
    560c:	4353      	muls	r3, r2
    560e:	18c0      	adds	r0, r0, r3
    5610:	4b0b      	ldr	r3, [pc, #44]	; (5640 <C2HDeQueue_8730A_ROM+0x48>)
    5612:	681b      	ldr	r3, [r3, #0]
    5614:	4798      	blx	r3
    5616:	6823      	ldr	r3, [r4, #0]
    5618:	33ff      	adds	r3, #255	; 0xff
    561a:	7e5a      	ldrb	r2, [r3, #25]
    561c:	3201      	adds	r2, #1
    561e:	b2d2      	uxtb	r2, r2
    5620:	2a08      	cmp	r2, #8
    5622:	d009      	beq.n	5638 <C2HDeQueue_8730A_ROM+0x40>
    5624:	765a      	strb	r2, [r3, #25]
    5626:	7e5a      	ldrb	r2, [r3, #25]
    5628:	7e9b      	ldrb	r3, [r3, #26]
    562a:	429a      	cmp	r2, r3
    562c:	d003      	beq.n	5636 <C2HDeQueue_8730A_ROM+0x3e>
    562e:	4b05      	ldr	r3, [pc, #20]	; (5644 <C2HDeQueue_8730A_ROM+0x4c>)
    5630:	2004      	movs	r0, #4
    5632:	681b      	ldr	r3, [r3, #0]
    5634:	4798      	blx	r3
    5636:	bd10      	pop	{r4, pc}
    5638:	2200      	movs	r2, #0
    563a:	e7f3      	b.n	5624 <C2HDeQueue_8730A_ROM+0x2c>
    563c:	23000a30 	.word	0x23000a30
    5640:	230007a8 	.word	0x230007a8
    5644:	230009b4 	.word	0x230009b4

00005648 <C2HInQueue_8730A_ROM>:
    5648:	b510      	push	{r4, lr}
    564a:	4c11      	ldr	r4, [pc, #68]	; (5690 <C2HInQueue_8730A_ROM+0x48>)
    564c:	0001      	movs	r1, r0
    564e:	6820      	ldr	r0, [r4, #0]
    5650:	0003      	movs	r3, r0
    5652:	33ff      	adds	r3, #255	; 0xff
    5654:	7e5a      	ldrb	r2, [r3, #25]
    5656:	7e9b      	ldrb	r3, [r3, #26]
    5658:	b90a      	cbnz	r2, 565e <C2HInQueue_8730A_ROM+0x16>
    565a:	2b07      	cmp	r3, #7
    565c:	d002      	beq.n	5664 <C2HInQueue_8730A_ROM+0x1c>
    565e:	3a01      	subs	r2, #1
    5660:	4293      	cmp	r3, r2
    5662:	d105      	bne.n	5670 <C2HInQueue_8730A_ROM+0x28>
    5664:	2302      	movs	r3, #2
    5666:	4a0b      	ldr	r2, [pc, #44]	; (5694 <C2HInQueue_8730A_ROM+0x4c>)
    5668:	7811      	ldrb	r1, [r2, #0]
    566a:	430b      	orrs	r3, r1
    566c:	7013      	strb	r3, [r2, #0]
    566e:	bd10      	pop	{r4, pc}
    5670:	2223      	movs	r2, #35	; 0x23
    5672:	4353      	muls	r3, r2
    5674:	18c0      	adds	r0, r0, r3
    5676:	4b08      	ldr	r3, [pc, #32]	; (5698 <C2HInQueue_8730A_ROM+0x50>)
    5678:	4798      	blx	r3
    567a:	6822      	ldr	r2, [r4, #0]
    567c:	32ff      	adds	r2, #255	; 0xff
    567e:	7e93      	ldrb	r3, [r2, #26]
    5680:	3301      	adds	r3, #1
    5682:	b2db      	uxtb	r3, r3
    5684:	2b08      	cmp	r3, #8
    5686:	d001      	beq.n	568c <C2HInQueue_8730A_ROM+0x44>
    5688:	7693      	strb	r3, [r2, #26]
    568a:	e7f0      	b.n	566e <C2HInQueue_8730A_ROM+0x26>
    568c:	2300      	movs	r3, #0
    568e:	e7fb      	b.n	5688 <C2HInQueue_8730A_ROM+0x40>
    5690:	23000a30 	.word	0x23000a30
    5694:	430001c1 	.word	0x430001c1
    5698:	00004b5d 	.word	0x00004b5d

0000569c <CheckMaxMacidNum_8730A_ROM>:
    569c:	b530      	push	{r4, r5, lr}
    569e:	4b12      	ldr	r3, [pc, #72]	; (56e8 <CheckMaxMacidNum_8730A_ROM+0x4c>)
    56a0:	6819      	ldr	r1, [r3, #0]
    56a2:	7848      	ldrb	r0, [r1, #1]
    56a4:	b150      	cbz	r0, 56bc <CheckMaxMacidNum_8730A_ROM+0x20>
    56a6:	2307      	movs	r3, #7
    56a8:	2401      	movs	r4, #1
    56aa:	0005      	movs	r5, r0
    56ac:	40dd      	lsrs	r5, r3
    56ae:	b2da      	uxtb	r2, r3
    56b0:	4225      	tst	r5, r4
    56b2:	d001      	beq.n	56b8 <CheckMaxMacidNum_8730A_ROM+0x1c>
    56b4:	2001      	movs	r0, #1
    56b6:	e00d      	b.n	56d4 <CheckMaxMacidNum_8730A_ROM+0x38>
    56b8:	3b01      	subs	r3, #1
    56ba:	d2f6      	bcs.n	56aa <CheckMaxMacidNum_8730A_ROM+0xe>
    56bc:	7808      	ldrb	r0, [r1, #0]
    56be:	b168      	cbz	r0, 56dc <CheckMaxMacidNum_8730A_ROM+0x40>
    56c0:	2307      	movs	r3, #7
    56c2:	2101      	movs	r1, #1
    56c4:	0004      	movs	r4, r0
    56c6:	40dc      	lsrs	r4, r3
    56c8:	0025      	movs	r5, r4
    56ca:	b2da      	uxtb	r2, r3
    56cc:	400d      	ands	r5, r1
    56ce:	420c      	tst	r4, r1
    56d0:	d005      	beq.n	56de <CheckMaxMacidNum_8730A_ROM+0x42>
    56d2:	2000      	movs	r0, #0
    56d4:	00c0      	lsls	r0, r0, #3
    56d6:	3201      	adds	r2, #1
    56d8:	1880      	adds	r0, r0, r2
    56da:	b2c0      	uxtb	r0, r0
    56dc:	bd30      	pop	{r4, r5, pc}
    56de:	3b01      	subs	r3, #1
    56e0:	d2f0      	bcs.n	56c4 <CheckMaxMacidNum_8730A_ROM+0x28>
    56e2:	0028      	movs	r0, r5
    56e4:	e7fa      	b.n	56dc <CheckMaxMacidNum_8730A_ROM+0x40>
    56e6:	46c0      	nop			; (mov r8, r8)
    56e8:	23000a1c 	.word	0x23000a1c

000056ec <SetMediaStatus_8730A_ROM>:
    56ec:	2207      	movs	r2, #7
    56ee:	b570      	push	{r4, r5, r6, lr}
    56f0:	08c5      	lsrs	r5, r0, #3
    56f2:	4010      	ands	r0, r2
    56f4:	3a06      	subs	r2, #6
    56f6:	4082      	lsls	r2, r0
    56f8:	4b06      	ldr	r3, [pc, #24]	; (5714 <SetMediaStatus_8730A_ROM+0x28>)
    56fa:	b2d2      	uxtb	r2, r2
    56fc:	681c      	ldr	r4, [r3, #0]
    56fe:	5d63      	ldrb	r3, [r4, r5]
    5700:	b931      	cbnz	r1, 5710 <SetMediaStatus_8730A_ROM+0x24>
    5702:	4393      	bics	r3, r2
    5704:	5563      	strb	r3, [r4, r5]
    5706:	4b04      	ldr	r3, [pc, #16]	; (5718 <SetMediaStatus_8730A_ROM+0x2c>)
    5708:	681b      	ldr	r3, [r3, #0]
    570a:	4798      	blx	r3
    570c:	70a0      	strb	r0, [r4, #2]
    570e:	bd70      	pop	{r4, r5, r6, pc}
    5710:	4313      	orrs	r3, r2
    5712:	e7f7      	b.n	5704 <SetMediaStatus_8730A_ROM+0x18>
    5714:	23000a1c 	.word	0x23000a1c
    5718:	230007b4 	.word	0x230007b4

0000571c <GetMediaStatus_8730A_ROM>:
    571c:	4b05      	ldr	r3, [pc, #20]	; (5734 <GetMediaStatus_8730A_ROM+0x18>)
    571e:	681a      	ldr	r2, [r3, #0]
    5720:	08c3      	lsrs	r3, r0, #3
    5722:	5cd3      	ldrb	r3, [r2, r3]
    5724:	2207      	movs	r2, #7
    5726:	4010      	ands	r0, r2
    5728:	40c3      	lsrs	r3, r0
    572a:	0018      	movs	r0, r3
    572c:	2301      	movs	r3, #1
    572e:	4018      	ands	r0, r3
    5730:	4770      	bx	lr
    5732:	46c0      	nop			; (mov r8, r8)
    5734:	23000a1c 	.word	0x23000a1c

00005738 <InitDDMA_8730A_ROM>:
    5738:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    573a:	001d      	movs	r5, r3
    573c:	ab08      	add	r3, sp, #32
    573e:	781f      	ldrb	r7, [r3, #0]
    5740:	4b10      	ldr	r3, [pc, #64]	; (5784 <InitDDMA_8730A_ROM+0x4c>)
    5742:	0004      	movs	r4, r0
    5744:	681b      	ldr	r3, [r3, #0]
    5746:	000e      	movs	r6, r1
    5748:	9201      	str	r2, [sp, #4]
    574a:	4798      	blx	r3
    574c:	b1c8      	cbz	r0, 5782 <InitDDMA_8730A_ROM+0x4a>
    574e:	2000      	movs	r0, #0
    5750:	2c05      	cmp	r4, #5
    5752:	d816      	bhi.n	5782 <InitDDMA_8730A_ROM+0x4a>
    5754:	f241 2300 	movw	r3, #4608	; 0x1200
    5758:	0124      	lsls	r4, r4, #4
    575a:	03ad      	lsls	r5, r5, #14
    575c:	18e4      	adds	r4, r4, r3
    575e:	0bad      	lsrs	r5, r5, #14
    5760:	2380      	movs	r3, #128	; 0x80
    5762:	b107      	cbz	r7, 5766 <InitDDMA_8730A_ROM+0x2e>
    5764:	23a0      	movs	r3, #160	; 0xa0
    5766:	061b      	lsls	r3, r3, #24
    5768:	431d      	orrs	r5, r3
    576a:	2386      	movs	r3, #134	; 0x86
    576c:	2001      	movs	r0, #1
    576e:	05db      	lsls	r3, r3, #23
    5770:	18e3      	adds	r3, r4, r3
    5772:	601e      	str	r6, [r3, #0]
    5774:	4b04      	ldr	r3, [pc, #16]	; (5788 <InitDDMA_8730A_ROM+0x50>)
    5776:	9a01      	ldr	r2, [sp, #4]
    5778:	18e3      	adds	r3, r4, r3
    577a:	601a      	str	r2, [r3, #0]
    577c:	4b03      	ldr	r3, [pc, #12]	; (578c <InitDDMA_8730A_ROM+0x54>)
    577e:	18e4      	adds	r4, r4, r3
    5780:	6025      	str	r5, [r4, #0]
    5782:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    5784:	2300077c 	.word	0x2300077c
    5788:	43000004 	.word	0x43000004
    578c:	43000008 	.word	0x43000008

00005790 <WaitDDMARDY_8730A_ROM>:
    5790:	2301      	movs	r3, #1
    5792:	4083      	lsls	r3, r0
    5794:	f241 3088 	movw	r0, #5000	; 0x1388
    5798:	4907      	ldr	r1, [pc, #28]	; (57b8 <WaitDDMARDY_8730A_ROM+0x28>)
    579a:	780a      	ldrb	r2, [r1, #0]
    579c:	4213      	tst	r3, r2
    579e:	d101      	bne.n	57a4 <WaitDDMARDY_8730A_ROM+0x14>
    57a0:	2001      	movs	r0, #1
    57a2:	e008      	b.n	57b6 <WaitDDMARDY_8730A_ROM+0x26>
    57a4:	3801      	subs	r0, #1
    57a6:	b280      	uxth	r0, r0
    57a8:	2800      	cmp	r0, #0
    57aa:	d1f6      	bne.n	579a <WaitDDMARDY_8730A_ROM+0xa>
    57ac:	2308      	movs	r3, #8
    57ae:	4a03      	ldr	r2, [pc, #12]	; (57bc <WaitDDMARDY_8730A_ROM+0x2c>)
    57b0:	7811      	ldrb	r1, [r2, #0]
    57b2:	430b      	orrs	r3, r1
    57b4:	7013      	strb	r3, [r2, #0]
    57b6:	4770      	bx	lr
    57b8:	430012e8 	.word	0x430012e8
    57bc:	430001c2 	.word	0x430001c2

000057c0 <ChkDDMAChksum_8730A_ROM>:
    57c0:	0003      	movs	r3, r0
    57c2:	2000      	movs	r0, #0
    57c4:	2b05      	cmp	r3, #5
    57c6:	d806      	bhi.n	57d6 <ChkDDMAChksum_8730A_ROM+0x16>
    57c8:	4a03      	ldr	r2, [pc, #12]	; (57d8 <ChkDDMAChksum_8730A_ROM+0x18>)
    57ca:	011b      	lsls	r3, r3, #4
    57cc:	189b      	adds	r3, r3, r2
    57ce:	781b      	ldrb	r3, [r3, #0]
    57d0:	3001      	adds	r0, #1
    57d2:	08db      	lsrs	r3, r3, #3
    57d4:	4398      	bics	r0, r3
    57d6:	4770      	bx	lr
    57d8:	43001203 	.word	0x43001203

000057dc <CheckDDMADone_8730A_ROM>:
    57dc:	4b0a      	ldr	r3, [pc, #40]	; (5808 <CheckDDMADone_8730A_ROM+0x2c>)
    57de:	b510      	push	{r4, lr}
    57e0:	681b      	ldr	r3, [r3, #0]
    57e2:	0004      	movs	r4, r0
    57e4:	b169      	cbz	r1, 5802 <CheckDDMADone_8730A_ROM+0x26>
    57e6:	4798      	blx	r3
    57e8:	0003      	movs	r3, r0
    57ea:	2000      	movs	r0, #0
    57ec:	2b01      	cmp	r3, #1
    57ee:	d107      	bne.n	5800 <CheckDDMADone_8730A_ROM+0x24>
    57f0:	4b06      	ldr	r3, [pc, #24]	; (580c <CheckDDMADone_8730A_ROM+0x30>)
    57f2:	0020      	movs	r0, r4
    57f4:	681b      	ldr	r3, [r3, #0]
    57f6:	4798      	blx	r3
    57f8:	3801      	subs	r0, #1
    57fa:	4243      	negs	r3, r0
    57fc:	4158      	adcs	r0, r3
    57fe:	b2c0      	uxtb	r0, r0
    5800:	bd10      	pop	{r4, pc}
    5802:	4798      	blx	r3
    5804:	e7fc      	b.n	5800 <CheckDDMADone_8730A_ROM+0x24>
    5806:	46c0      	nop			; (mov r8, r8)
    5808:	2300077c 	.word	0x2300077c
    580c:	23000780 	.word	0x23000780

00005810 <InitGTimer32us_8730A_ROM>:
    5810:	b530      	push	{r4, r5, lr}
    5812:	ac03      	add	r4, sp, #12
    5814:	7825      	ldrb	r5, [r4, #0]
    5816:	2486      	movs	r4, #134	; 0x86
    5818:	05e4      	lsls	r4, r4, #23
    581a:	1904      	adds	r4, r0, r4
    581c:	7025      	strb	r5, [r4, #0]
    581e:	4c09      	ldr	r4, [pc, #36]	; (5844 <InitGTimer32us_8730A_ROM+0x34>)
    5820:	1904      	adds	r4, r0, r4
    5822:	7023      	strb	r3, [r4, #0]
    5824:	4b08      	ldr	r3, [pc, #32]	; (5848 <InitGTimer32us_8730A_ROM+0x38>)
    5826:	18c3      	adds	r3, r0, r3
    5828:	701a      	strb	r2, [r3, #0]
    582a:	4b08      	ldr	r3, [pc, #32]	; (584c <InitGTimer32us_8730A_ROM+0x3c>)
    582c:	18c0      	adds	r0, r0, r3
    582e:	2303      	movs	r3, #3
    5830:	2901      	cmp	r1, #1
    5832:	d000      	beq.n	5836 <InitGTimer32us_8730A_ROM+0x26>
    5834:	2301      	movs	r3, #1
    5836:	7003      	strb	r3, [r0, #0]
    5838:	2304      	movs	r3, #4
    583a:	7802      	ldrb	r2, [r0, #0]
    583c:	4313      	orrs	r3, r2
    583e:	7003      	strb	r3, [r0, #0]
    5840:	bd30      	pop	{r4, r5, pc}
    5842:	46c0      	nop			; (mov r8, r8)
    5844:	43000001 	.word	0x43000001
    5848:	43000002 	.word	0x43000002
    584c:	43000003 	.word	0x43000003

00005850 <InitGTimer1ms_8730A_ROM>:
    5850:	b570      	push	{r4, r5, r6, lr}
    5852:	ac04      	add	r4, sp, #16
    5854:	7825      	ldrb	r5, [r4, #0]
    5856:	2486      	movs	r4, #134	; 0x86
    5858:	05e4      	lsls	r4, r4, #23
    585a:	1906      	adds	r6, r0, r4
    585c:	016c      	lsls	r4, r5, #5
    585e:	b2e4      	uxtb	r4, r4
    5860:	7034      	strb	r4, [r6, #0]
    5862:	4c0d      	ldr	r4, [pc, #52]	; (5898 <InitGTimer1ms_8730A_ROM+0x48>)
    5864:	08ed      	lsrs	r5, r5, #3
    5866:	1906      	adds	r6, r0, r4
    5868:	015c      	lsls	r4, r3, #5
    586a:	432c      	orrs	r4, r5
    586c:	b2e4      	uxtb	r4, r4
    586e:	7034      	strb	r4, [r6, #0]
    5870:	08db      	lsrs	r3, r3, #3
    5872:	4c0a      	ldr	r4, [pc, #40]	; (589c <InitGTimer1ms_8730A_ROM+0x4c>)
    5874:	0152      	lsls	r2, r2, #5
    5876:	431a      	orrs	r2, r3
    5878:	4b09      	ldr	r3, [pc, #36]	; (58a0 <InitGTimer1ms_8730A_ROM+0x50>)
    587a:	1904      	adds	r4, r0, r4
    587c:	b2d2      	uxtb	r2, r2
    587e:	18c0      	adds	r0, r0, r3
    5880:	7022      	strb	r2, [r4, #0]
    5882:	2303      	movs	r3, #3
    5884:	2901      	cmp	r1, #1
    5886:	d000      	beq.n	588a <InitGTimer1ms_8730A_ROM+0x3a>
    5888:	2301      	movs	r3, #1
    588a:	7003      	strb	r3, [r0, #0]
    588c:	2304      	movs	r3, #4
    588e:	7802      	ldrb	r2, [r0, #0]
    5890:	4313      	orrs	r3, r2
    5892:	7003      	strb	r3, [r0, #0]
    5894:	bd70      	pop	{r4, r5, r6, pc}
    5896:	46c0      	nop			; (mov r8, r8)
    5898:	43000001 	.word	0x43000001
    589c:	43000002 	.word	0x43000002
    58a0:	43000003 	.word	0x43000003

000058a4 <InitFuncPtrTable_8730A_ROM>:
    58a4:	4b76      	ldr	r3, [pc, #472]	; (5a80 <InitFuncPtrTable_8730A_ROM+0x1dc>)
    58a6:	4a77      	ldr	r2, [pc, #476]	; (5a84 <InitFuncPtrTable_8730A_ROM+0x1e0>)
    58a8:	601a      	str	r2, [r3, #0]
    58aa:	4a77      	ldr	r2, [pc, #476]	; (5a88 <InitFuncPtrTable_8730A_ROM+0x1e4>)
    58ac:	605a      	str	r2, [r3, #4]
    58ae:	4a77      	ldr	r2, [pc, #476]	; (5a8c <InitFuncPtrTable_8730A_ROM+0x1e8>)
    58b0:	609a      	str	r2, [r3, #8]
    58b2:	4a77      	ldr	r2, [pc, #476]	; (5a90 <InitFuncPtrTable_8730A_ROM+0x1ec>)
    58b4:	60da      	str	r2, [r3, #12]
    58b6:	4a77      	ldr	r2, [pc, #476]	; (5a94 <InitFuncPtrTable_8730A_ROM+0x1f0>)
    58b8:	611a      	str	r2, [r3, #16]
    58ba:	4a77      	ldr	r2, [pc, #476]	; (5a98 <InitFuncPtrTable_8730A_ROM+0x1f4>)
    58bc:	615a      	str	r2, [r3, #20]
    58be:	4a77      	ldr	r2, [pc, #476]	; (5a9c <InitFuncPtrTable_8730A_ROM+0x1f8>)
    58c0:	619a      	str	r2, [r3, #24]
    58c2:	4a77      	ldr	r2, [pc, #476]	; (5aa0 <InitFuncPtrTable_8730A_ROM+0x1fc>)
    58c4:	61da      	str	r2, [r3, #28]
    58c6:	4a77      	ldr	r2, [pc, #476]	; (5aa4 <InitFuncPtrTable_8730A_ROM+0x200>)
    58c8:	621a      	str	r2, [r3, #32]
    58ca:	4a77      	ldr	r2, [pc, #476]	; (5aa8 <InitFuncPtrTable_8730A_ROM+0x204>)
    58cc:	625a      	str	r2, [r3, #36]	; 0x24
    58ce:	4a77      	ldr	r2, [pc, #476]	; (5aac <InitFuncPtrTable_8730A_ROM+0x208>)
    58d0:	629a      	str	r2, [r3, #40]	; 0x28
    58d2:	4a77      	ldr	r2, [pc, #476]	; (5ab0 <InitFuncPtrTable_8730A_ROM+0x20c>)
    58d4:	62da      	str	r2, [r3, #44]	; 0x2c
    58d6:	4a77      	ldr	r2, [pc, #476]	; (5ab4 <InitFuncPtrTable_8730A_ROM+0x210>)
    58d8:	631a      	str	r2, [r3, #48]	; 0x30
    58da:	4a77      	ldr	r2, [pc, #476]	; (5ab8 <InitFuncPtrTable_8730A_ROM+0x214>)
    58dc:	635a      	str	r2, [r3, #52]	; 0x34
    58de:	4a77      	ldr	r2, [pc, #476]	; (5abc <InitFuncPtrTable_8730A_ROM+0x218>)
    58e0:	639a      	str	r2, [r3, #56]	; 0x38
    58e2:	4a77      	ldr	r2, [pc, #476]	; (5ac0 <InitFuncPtrTable_8730A_ROM+0x21c>)
    58e4:	63da      	str	r2, [r3, #60]	; 0x3c
    58e6:	4a77      	ldr	r2, [pc, #476]	; (5ac4 <InitFuncPtrTable_8730A_ROM+0x220>)
    58e8:	641a      	str	r2, [r3, #64]	; 0x40
    58ea:	4a77      	ldr	r2, [pc, #476]	; (5ac8 <InitFuncPtrTable_8730A_ROM+0x224>)
    58ec:	645a      	str	r2, [r3, #68]	; 0x44
    58ee:	4a77      	ldr	r2, [pc, #476]	; (5acc <InitFuncPtrTable_8730A_ROM+0x228>)
    58f0:	649a      	str	r2, [r3, #72]	; 0x48
    58f2:	4a77      	ldr	r2, [pc, #476]	; (5ad0 <InitFuncPtrTable_8730A_ROM+0x22c>)
    58f4:	64da      	str	r2, [r3, #76]	; 0x4c
    58f6:	4a77      	ldr	r2, [pc, #476]	; (5ad4 <InitFuncPtrTable_8730A_ROM+0x230>)
    58f8:	651a      	str	r2, [r3, #80]	; 0x50
    58fa:	4a77      	ldr	r2, [pc, #476]	; (5ad8 <InitFuncPtrTable_8730A_ROM+0x234>)
    58fc:	655a      	str	r2, [r3, #84]	; 0x54
    58fe:	4a77      	ldr	r2, [pc, #476]	; (5adc <InitFuncPtrTable_8730A_ROM+0x238>)
    5900:	659a      	str	r2, [r3, #88]	; 0x58
    5902:	4a77      	ldr	r2, [pc, #476]	; (5ae0 <InitFuncPtrTable_8730A_ROM+0x23c>)
    5904:	65da      	str	r2, [r3, #92]	; 0x5c
    5906:	4a77      	ldr	r2, [pc, #476]	; (5ae4 <InitFuncPtrTable_8730A_ROM+0x240>)
    5908:	661a      	str	r2, [r3, #96]	; 0x60
    590a:	4a77      	ldr	r2, [pc, #476]	; (5ae8 <InitFuncPtrTable_8730A_ROM+0x244>)
    590c:	665a      	str	r2, [r3, #100]	; 0x64
    590e:	4a77      	ldr	r2, [pc, #476]	; (5aec <InitFuncPtrTable_8730A_ROM+0x248>)
    5910:	669a      	str	r2, [r3, #104]	; 0x68
    5912:	4a77      	ldr	r2, [pc, #476]	; (5af0 <InitFuncPtrTable_8730A_ROM+0x24c>)
    5914:	66da      	str	r2, [r3, #108]	; 0x6c
    5916:	4a77      	ldr	r2, [pc, #476]	; (5af4 <InitFuncPtrTable_8730A_ROM+0x250>)
    5918:	671a      	str	r2, [r3, #112]	; 0x70
    591a:	4a77      	ldr	r2, [pc, #476]	; (5af8 <InitFuncPtrTable_8730A_ROM+0x254>)
    591c:	675a      	str	r2, [r3, #116]	; 0x74
    591e:	4a77      	ldr	r2, [pc, #476]	; (5afc <InitFuncPtrTable_8730A_ROM+0x258>)
    5920:	679a      	str	r2, [r3, #120]	; 0x78
    5922:	4a77      	ldr	r2, [pc, #476]	; (5b00 <InitFuncPtrTable_8730A_ROM+0x25c>)
    5924:	67da      	str	r2, [r3, #124]	; 0x7c
    5926:	4b77      	ldr	r3, [pc, #476]	; (5b04 <InitFuncPtrTable_8730A_ROM+0x260>)
    5928:	4a77      	ldr	r2, [pc, #476]	; (5b08 <InitFuncPtrTable_8730A_ROM+0x264>)
    592a:	601a      	str	r2, [r3, #0]
    592c:	4a77      	ldr	r2, [pc, #476]	; (5b0c <InitFuncPtrTable_8730A_ROM+0x268>)
    592e:	605a      	str	r2, [r3, #4]
    5930:	4a77      	ldr	r2, [pc, #476]	; (5b10 <InitFuncPtrTable_8730A_ROM+0x26c>)
    5932:	609a      	str	r2, [r3, #8]
    5934:	4a77      	ldr	r2, [pc, #476]	; (5b14 <InitFuncPtrTable_8730A_ROM+0x270>)
    5936:	60da      	str	r2, [r3, #12]
    5938:	4a77      	ldr	r2, [pc, #476]	; (5b18 <InitFuncPtrTable_8730A_ROM+0x274>)
    593a:	611a      	str	r2, [r3, #16]
    593c:	4a77      	ldr	r2, [pc, #476]	; (5b1c <InitFuncPtrTable_8730A_ROM+0x278>)
    593e:	615a      	str	r2, [r3, #20]
    5940:	4a77      	ldr	r2, [pc, #476]	; (5b20 <InitFuncPtrTable_8730A_ROM+0x27c>)
    5942:	619a      	str	r2, [r3, #24]
    5944:	4a77      	ldr	r2, [pc, #476]	; (5b24 <InitFuncPtrTable_8730A_ROM+0x280>)
    5946:	61da      	str	r2, [r3, #28]
    5948:	4a77      	ldr	r2, [pc, #476]	; (5b28 <InitFuncPtrTable_8730A_ROM+0x284>)
    594a:	621a      	str	r2, [r3, #32]
    594c:	4a77      	ldr	r2, [pc, #476]	; (5b2c <InitFuncPtrTable_8730A_ROM+0x288>)
    594e:	625a      	str	r2, [r3, #36]	; 0x24
    5950:	4a77      	ldr	r2, [pc, #476]	; (5b30 <InitFuncPtrTable_8730A_ROM+0x28c>)
    5952:	629a      	str	r2, [r3, #40]	; 0x28
    5954:	4a77      	ldr	r2, [pc, #476]	; (5b34 <InitFuncPtrTable_8730A_ROM+0x290>)
    5956:	62da      	str	r2, [r3, #44]	; 0x2c
    5958:	4a77      	ldr	r2, [pc, #476]	; (5b38 <InitFuncPtrTable_8730A_ROM+0x294>)
    595a:	631a      	str	r2, [r3, #48]	; 0x30
    595c:	4a77      	ldr	r2, [pc, #476]	; (5b3c <InitFuncPtrTable_8730A_ROM+0x298>)
    595e:	635a      	str	r2, [r3, #52]	; 0x34
    5960:	4a77      	ldr	r2, [pc, #476]	; (5b40 <InitFuncPtrTable_8730A_ROM+0x29c>)
    5962:	639a      	str	r2, [r3, #56]	; 0x38
    5964:	4a77      	ldr	r2, [pc, #476]	; (5b44 <InitFuncPtrTable_8730A_ROM+0x2a0>)
    5966:	63da      	str	r2, [r3, #60]	; 0x3c
    5968:	4a77      	ldr	r2, [pc, #476]	; (5b48 <InitFuncPtrTable_8730A_ROM+0x2a4>)
    596a:	641a      	str	r2, [r3, #64]	; 0x40
    596c:	4a77      	ldr	r2, [pc, #476]	; (5b4c <InitFuncPtrTable_8730A_ROM+0x2a8>)
    596e:	645a      	str	r2, [r3, #68]	; 0x44
    5970:	4a77      	ldr	r2, [pc, #476]	; (5b50 <InitFuncPtrTable_8730A_ROM+0x2ac>)
    5972:	649a      	str	r2, [r3, #72]	; 0x48
    5974:	4a77      	ldr	r2, [pc, #476]	; (5b54 <InitFuncPtrTable_8730A_ROM+0x2b0>)
    5976:	64da      	str	r2, [r3, #76]	; 0x4c
    5978:	4a77      	ldr	r2, [pc, #476]	; (5b58 <InitFuncPtrTable_8730A_ROM+0x2b4>)
    597a:	651a      	str	r2, [r3, #80]	; 0x50
    597c:	4a77      	ldr	r2, [pc, #476]	; (5b5c <InitFuncPtrTable_8730A_ROM+0x2b8>)
    597e:	655a      	str	r2, [r3, #84]	; 0x54
    5980:	4a77      	ldr	r2, [pc, #476]	; (5b60 <InitFuncPtrTable_8730A_ROM+0x2bc>)
    5982:	659a      	str	r2, [r3, #88]	; 0x58
    5984:	4a77      	ldr	r2, [pc, #476]	; (5b64 <InitFuncPtrTable_8730A_ROM+0x2c0>)
    5986:	65da      	str	r2, [r3, #92]	; 0x5c
    5988:	4a77      	ldr	r2, [pc, #476]	; (5b68 <InitFuncPtrTable_8730A_ROM+0x2c4>)
    598a:	661a      	str	r2, [r3, #96]	; 0x60
    598c:	4a77      	ldr	r2, [pc, #476]	; (5b6c <InitFuncPtrTable_8730A_ROM+0x2c8>)
    598e:	665a      	str	r2, [r3, #100]	; 0x64
    5990:	4a77      	ldr	r2, [pc, #476]	; (5b70 <InitFuncPtrTable_8730A_ROM+0x2cc>)
    5992:	669a      	str	r2, [r3, #104]	; 0x68
    5994:	4a77      	ldr	r2, [pc, #476]	; (5b74 <InitFuncPtrTable_8730A_ROM+0x2d0>)
    5996:	66da      	str	r2, [r3, #108]	; 0x6c
    5998:	4a77      	ldr	r2, [pc, #476]	; (5b78 <InitFuncPtrTable_8730A_ROM+0x2d4>)
    599a:	671a      	str	r2, [r3, #112]	; 0x70
    599c:	4a77      	ldr	r2, [pc, #476]	; (5b7c <InitFuncPtrTable_8730A_ROM+0x2d8>)
    599e:	675a      	str	r2, [r3, #116]	; 0x74
    59a0:	4a77      	ldr	r2, [pc, #476]	; (5b80 <InitFuncPtrTable_8730A_ROM+0x2dc>)
    59a2:	679a      	str	r2, [r3, #120]	; 0x78
    59a4:	4a77      	ldr	r2, [pc, #476]	; (5b84 <InitFuncPtrTable_8730A_ROM+0x2e0>)
    59a6:	67da      	str	r2, [r3, #124]	; 0x7c
    59a8:	4b77      	ldr	r3, [pc, #476]	; (5b88 <InitFuncPtrTable_8730A_ROM+0x2e4>)
    59aa:	4a78      	ldr	r2, [pc, #480]	; (5b8c <InitFuncPtrTable_8730A_ROM+0x2e8>)
    59ac:	601a      	str	r2, [r3, #0]
    59ae:	4a78      	ldr	r2, [pc, #480]	; (5b90 <InitFuncPtrTable_8730A_ROM+0x2ec>)
    59b0:	605a      	str	r2, [r3, #4]
    59b2:	4a78      	ldr	r2, [pc, #480]	; (5b94 <InitFuncPtrTable_8730A_ROM+0x2f0>)
    59b4:	609a      	str	r2, [r3, #8]
    59b6:	4a78      	ldr	r2, [pc, #480]	; (5b98 <InitFuncPtrTable_8730A_ROM+0x2f4>)
    59b8:	60da      	str	r2, [r3, #12]
    59ba:	4a78      	ldr	r2, [pc, #480]	; (5b9c <InitFuncPtrTable_8730A_ROM+0x2f8>)
    59bc:	611a      	str	r2, [r3, #16]
    59be:	4a78      	ldr	r2, [pc, #480]	; (5ba0 <InitFuncPtrTable_8730A_ROM+0x2fc>)
    59c0:	615a      	str	r2, [r3, #20]
    59c2:	4a78      	ldr	r2, [pc, #480]	; (5ba4 <InitFuncPtrTable_8730A_ROM+0x300>)
    59c4:	619a      	str	r2, [r3, #24]
    59c6:	4a78      	ldr	r2, [pc, #480]	; (5ba8 <InitFuncPtrTable_8730A_ROM+0x304>)
    59c8:	61da      	str	r2, [r3, #28]
    59ca:	4a78      	ldr	r2, [pc, #480]	; (5bac <InitFuncPtrTable_8730A_ROM+0x308>)
    59cc:	621a      	str	r2, [r3, #32]
    59ce:	4a78      	ldr	r2, [pc, #480]	; (5bb0 <InitFuncPtrTable_8730A_ROM+0x30c>)
    59d0:	625a      	str	r2, [r3, #36]	; 0x24
    59d2:	4a78      	ldr	r2, [pc, #480]	; (5bb4 <InitFuncPtrTable_8730A_ROM+0x310>)
    59d4:	629a      	str	r2, [r3, #40]	; 0x28
    59d6:	4a78      	ldr	r2, [pc, #480]	; (5bb8 <InitFuncPtrTable_8730A_ROM+0x314>)
    59d8:	62da      	str	r2, [r3, #44]	; 0x2c
    59da:	4a78      	ldr	r2, [pc, #480]	; (5bbc <InitFuncPtrTable_8730A_ROM+0x318>)
    59dc:	631a      	str	r2, [r3, #48]	; 0x30
    59de:	4a78      	ldr	r2, [pc, #480]	; (5bc0 <InitFuncPtrTable_8730A_ROM+0x31c>)
    59e0:	635a      	str	r2, [r3, #52]	; 0x34
    59e2:	4a78      	ldr	r2, [pc, #480]	; (5bc4 <InitFuncPtrTable_8730A_ROM+0x320>)
    59e4:	639a      	str	r2, [r3, #56]	; 0x38
    59e6:	4a78      	ldr	r2, [pc, #480]	; (5bc8 <InitFuncPtrTable_8730A_ROM+0x324>)
    59e8:	63da      	str	r2, [r3, #60]	; 0x3c
    59ea:	4a78      	ldr	r2, [pc, #480]	; (5bcc <InitFuncPtrTable_8730A_ROM+0x328>)
    59ec:	641a      	str	r2, [r3, #64]	; 0x40
    59ee:	4a78      	ldr	r2, [pc, #480]	; (5bd0 <InitFuncPtrTable_8730A_ROM+0x32c>)
    59f0:	645a      	str	r2, [r3, #68]	; 0x44
    59f2:	4a78      	ldr	r2, [pc, #480]	; (5bd4 <InitFuncPtrTable_8730A_ROM+0x330>)
    59f4:	649a      	str	r2, [r3, #72]	; 0x48
    59f6:	4a78      	ldr	r2, [pc, #480]	; (5bd8 <InitFuncPtrTable_8730A_ROM+0x334>)
    59f8:	64da      	str	r2, [r3, #76]	; 0x4c
    59fa:	4a78      	ldr	r2, [pc, #480]	; (5bdc <InitFuncPtrTable_8730A_ROM+0x338>)
    59fc:	651a      	str	r2, [r3, #80]	; 0x50
    59fe:	4a78      	ldr	r2, [pc, #480]	; (5be0 <InitFuncPtrTable_8730A_ROM+0x33c>)
    5a00:	655a      	str	r2, [r3, #84]	; 0x54
    5a02:	4a78      	ldr	r2, [pc, #480]	; (5be4 <InitFuncPtrTable_8730A_ROM+0x340>)
    5a04:	659a      	str	r2, [r3, #88]	; 0x58
    5a06:	4a78      	ldr	r2, [pc, #480]	; (5be8 <InitFuncPtrTable_8730A_ROM+0x344>)
    5a08:	65da      	str	r2, [r3, #92]	; 0x5c
    5a0a:	4a78      	ldr	r2, [pc, #480]	; (5bec <InitFuncPtrTable_8730A_ROM+0x348>)
    5a0c:	661a      	str	r2, [r3, #96]	; 0x60
    5a0e:	4a78      	ldr	r2, [pc, #480]	; (5bf0 <InitFuncPtrTable_8730A_ROM+0x34c>)
    5a10:	665a      	str	r2, [r3, #100]	; 0x64
    5a12:	4a78      	ldr	r2, [pc, #480]	; (5bf4 <InitFuncPtrTable_8730A_ROM+0x350>)
    5a14:	669a      	str	r2, [r3, #104]	; 0x68
    5a16:	4a78      	ldr	r2, [pc, #480]	; (5bf8 <InitFuncPtrTable_8730A_ROM+0x354>)
    5a18:	66da      	str	r2, [r3, #108]	; 0x6c
    5a1a:	4a78      	ldr	r2, [pc, #480]	; (5bfc <InitFuncPtrTable_8730A_ROM+0x358>)
    5a1c:	671a      	str	r2, [r3, #112]	; 0x70
    5a1e:	4a78      	ldr	r2, [pc, #480]	; (5c00 <InitFuncPtrTable_8730A_ROM+0x35c>)
    5a20:	675a      	str	r2, [r3, #116]	; 0x74
    5a22:	4a78      	ldr	r2, [pc, #480]	; (5c04 <InitFuncPtrTable_8730A_ROM+0x360>)
    5a24:	679a      	str	r2, [r3, #120]	; 0x78
    5a26:	4a78      	ldr	r2, [pc, #480]	; (5c08 <InitFuncPtrTable_8730A_ROM+0x364>)
    5a28:	67da      	str	r2, [r3, #124]	; 0x7c
    5a2a:	4b78      	ldr	r3, [pc, #480]	; (5c0c <InitFuncPtrTable_8730A_ROM+0x368>)
    5a2c:	4a78      	ldr	r2, [pc, #480]	; (5c10 <InitFuncPtrTable_8730A_ROM+0x36c>)
    5a2e:	601a      	str	r2, [r3, #0]
    5a30:	4a78      	ldr	r2, [pc, #480]	; (5c14 <InitFuncPtrTable_8730A_ROM+0x370>)
    5a32:	605a      	str	r2, [r3, #4]
    5a34:	4a78      	ldr	r2, [pc, #480]	; (5c18 <InitFuncPtrTable_8730A_ROM+0x374>)
    5a36:	609a      	str	r2, [r3, #8]
    5a38:	4a78      	ldr	r2, [pc, #480]	; (5c1c <InitFuncPtrTable_8730A_ROM+0x378>)
    5a3a:	60da      	str	r2, [r3, #12]
    5a3c:	4a78      	ldr	r2, [pc, #480]	; (5c20 <InitFuncPtrTable_8730A_ROM+0x37c>)
    5a3e:	611a      	str	r2, [r3, #16]
    5a40:	4a78      	ldr	r2, [pc, #480]	; (5c24 <InitFuncPtrTable_8730A_ROM+0x380>)
    5a42:	615a      	str	r2, [r3, #20]
    5a44:	4a78      	ldr	r2, [pc, #480]	; (5c28 <InitFuncPtrTable_8730A_ROM+0x384>)
    5a46:	619a      	str	r2, [r3, #24]
    5a48:	4a78      	ldr	r2, [pc, #480]	; (5c2c <InitFuncPtrTable_8730A_ROM+0x388>)
    5a4a:	61da      	str	r2, [r3, #28]
    5a4c:	4a78      	ldr	r2, [pc, #480]	; (5c30 <InitFuncPtrTable_8730A_ROM+0x38c>)
    5a4e:	621a      	str	r2, [r3, #32]
    5a50:	4a78      	ldr	r2, [pc, #480]	; (5c34 <InitFuncPtrTable_8730A_ROM+0x390>)
    5a52:	625a      	str	r2, [r3, #36]	; 0x24
    5a54:	4a78      	ldr	r2, [pc, #480]	; (5c38 <InitFuncPtrTable_8730A_ROM+0x394>)
    5a56:	629a      	str	r2, [r3, #40]	; 0x28
    5a58:	4a78      	ldr	r2, [pc, #480]	; (5c3c <InitFuncPtrTable_8730A_ROM+0x398>)
    5a5a:	62da      	str	r2, [r3, #44]	; 0x2c
    5a5c:	4a78      	ldr	r2, [pc, #480]	; (5c40 <InitFuncPtrTable_8730A_ROM+0x39c>)
    5a5e:	631a      	str	r2, [r3, #48]	; 0x30
    5a60:	4a78      	ldr	r2, [pc, #480]	; (5c44 <InitFuncPtrTable_8730A_ROM+0x3a0>)
    5a62:	635a      	str	r2, [r3, #52]	; 0x34
    5a64:	4a78      	ldr	r2, [pc, #480]	; (5c48 <InitFuncPtrTable_8730A_ROM+0x3a4>)
    5a66:	639a      	str	r2, [r3, #56]	; 0x38
    5a68:	4a78      	ldr	r2, [pc, #480]	; (5c4c <InitFuncPtrTable_8730A_ROM+0x3a8>)
    5a6a:	63da      	str	r2, [r3, #60]	; 0x3c
    5a6c:	4a78      	ldr	r2, [pc, #480]	; (5c50 <InitFuncPtrTable_8730A_ROM+0x3ac>)
    5a6e:	641a      	str	r2, [r3, #64]	; 0x40
    5a70:	4a78      	ldr	r2, [pc, #480]	; (5c54 <InitFuncPtrTable_8730A_ROM+0x3b0>)
    5a72:	645a      	str	r2, [r3, #68]	; 0x44
    5a74:	4a78      	ldr	r2, [pc, #480]	; (5c58 <InitFuncPtrTable_8730A_ROM+0x3b4>)
    5a76:	649a      	str	r2, [r3, #72]	; 0x48
    5a78:	4a78      	ldr	r2, [pc, #480]	; (5c5c <InitFuncPtrTable_8730A_ROM+0x3b8>)
    5a7a:	64da      	str	r2, [r3, #76]	; 0x4c
    5a7c:	4770      	bx	lr
    5a7e:	46c0      	nop			; (mov r8, r8)
    5a80:	23000750 	.word	0x23000750
    5a84:	00005d1d 	.word	0x00005d1d
    5a88:	00005dc5 	.word	0x00005dc5
    5a8c:	00005c61 	.word	0x00005c61
    5a90:	00005cbd 	.word	0x00005cbd
    5a94:	00005ecd 	.word	0x00005ecd
    5a98:	00005f15 	.word	0x00005f15
    5a9c:	00005e05 	.word	0x00005e05
    5aa0:	00005f59 	.word	0x00005f59
    5aa4:	00006015 	.word	0x00006015
    5aa8:	00006079 	.word	0x00006079
    5aac:	00005739 	.word	0x00005739
    5ab0:	00005791 	.word	0x00005791
    5ab4:	000057c1 	.word	0x000057c1
    5ab8:	000057dd 	.word	0x000057dd
    5abc:	00005811 	.word	0x00005811
    5ac0:	00005851 	.word	0x00005851
    5ac4:	0000536d 	.word	0x0000536d
    5ac8:	0000544d 	.word	0x0000544d
    5acc:	000054b5 	.word	0x000054b5
    5ad0:	000054f1 	.word	0x000054f1
    5ad4:	0000552d 	.word	0x0000552d
    5ad8:	00005565 	.word	0x00005565
    5adc:	000055a1 	.word	0x000055a1
    5ae0:	000055f9 	.word	0x000055f9
    5ae4:	00005649 	.word	0x00005649
    5ae8:	0000569d 	.word	0x0000569d
    5aec:	000056ed 	.word	0x000056ed
    5af0:	0000571d 	.word	0x0000571d
    5af4:	000060d9 	.word	0x000060d9
    5af8:	00006131 	.word	0x00006131
    5afc:	000061a1 	.word	0x000061a1
    5b00:	000061f9 	.word	0x000061f9
    5b04:	230007d0 	.word	0x230007d0
    5b08:	00006291 	.word	0x00006291
    5b0c:	00006325 	.word	0x00006325
    5b10:	00006341 	.word	0x00006341
    5b14:	000063d9 	.word	0x000063d9
    5b18:	000064bd 	.word	0x000064bd
    5b1c:	000064e1 	.word	0x000064e1
    5b20:	00006511 	.word	0x00006511
    5b24:	00006551 	.word	0x00006551
    5b28:	000065c5 	.word	0x000065c5
    5b2c:	0000660d 	.word	0x0000660d
    5b30:	00006631 	.word	0x00006631
    5b34:	000066a5 	.word	0x000066a5
    5b38:	0000675d 	.word	0x0000675d
    5b3c:	0000679d 	.word	0x0000679d
    5b40:	000067ed 	.word	0x000067ed
    5b44:	0000681d 	.word	0x0000681d
    5b48:	00006869 	.word	0x00006869
    5b4c:	000068c5 	.word	0x000068c5
    5b50:	00006919 	.word	0x00006919
    5b54:	0000693d 	.word	0x0000693d
    5b58:	00006961 	.word	0x00006961
    5b5c:	0000699d 	.word	0x0000699d
    5b60:	000069c9 	.word	0x000069c9
    5b64:	000069f5 	.word	0x000069f5
    5b68:	00006a11 	.word	0x00006a11
    5b6c:	00006a4d 	.word	0x00006a4d
    5b70:	00006a61 	.word	0x00006a61
    5b74:	00006aa1 	.word	0x00006aa1
    5b78:	00006ab1 	.word	0x00006ab1
    5b7c:	00006ad1 	.word	0x00006ad1
    5b80:	00006b29 	.word	0x00006b29
    5b84:	00006b65 	.word	0x00006b65
    5b88:	23000850 	.word	0x23000850
    5b8c:	00006c69 	.word	0x00006c69
    5b90:	00006c81 	.word	0x00006c81
    5b94:	00006ca5 	.word	0x00006ca5
    5b98:	00006ce5 	.word	0x00006ce5
    5b9c:	00006d59 	.word	0x00006d59
    5ba0:	00006e31 	.word	0x00006e31
    5ba4:	00006ea9 	.word	0x00006ea9
    5ba8:	00007059 	.word	0x00007059
    5bac:	0000707d 	.word	0x0000707d
    5bb0:	000070b1 	.word	0x000070b1
    5bb4:	0000728d 	.word	0x0000728d
    5bb8:	000072fd 	.word	0x000072fd
    5bbc:	000073b5 	.word	0x000073b5
    5bc0:	00007415 	.word	0x00007415
    5bc4:	0000753d 	.word	0x0000753d
    5bc8:	00007551 	.word	0x00007551
    5bcc:	0000758d 	.word	0x0000758d
    5bd0:	000075c9 	.word	0x000075c9
    5bd4:	000075e9 	.word	0x000075e9
    5bd8:	00007615 	.word	0x00007615
    5bdc:	00007631 	.word	0x00007631
    5be0:	0000764d 	.word	0x0000764d
    5be4:	00007671 	.word	0x00007671
    5be8:	000078a5 	.word	0x000078a5
    5bec:	000078d1 	.word	0x000078d1
    5bf0:	00007915 	.word	0x00007915
    5bf4:	0000793d 	.word	0x0000793d
    5bf8:	000076cd 	.word	0x000076cd
    5bfc:	00007721 	.word	0x00007721
    5c00:	00007769 	.word	0x00007769
    5c04:	00007789 	.word	0x00007789
    5c08:	000077b7 	.word	0x000077b7
    5c0c:	230008d0 	.word	0x230008d0
    5c10:	000077f7 	.word	0x000077f7
    5c14:	00007869 	.word	0x00007869
    5c18:	00005035 	.word	0x00005035
    5c1c:	00005049 	.word	0x00005049
    5c20:	0000505d 	.word	0x0000505d
    5c24:	00005071 	.word	0x00005071
    5c28:	00005085 	.word	0x00005085
    5c2c:	000050ad 	.word	0x000050ad
    5c30:	000050dd 	.word	0x000050dd
    5c34:	000050f1 	.word	0x000050f1
    5c38:	00005115 	.word	0x00005115
    5c3c:	00005141 	.word	0x00005141
    5c40:	00005171 	.word	0x00005171
    5c44:	0000518d 	.word	0x0000518d
    5c48:	000051d5 	.word	0x000051d5
    5c4c:	00005235 	.word	0x00005235
    5c50:	00005269 	.word	0x00005269
    5c54:	00005285 	.word	0x00005285
    5c58:	000052c5 	.word	0x000052c5
    5c5c:	00005305 	.word	0x00005305

00005c60 <ClrAllFWUsedIMR_8730A_ROM>:
    5c60:	2300      	movs	r3, #0
    5c62:	4a0b      	ldr	r2, [pc, #44]	; (5c90 <ClrAllFWUsedIMR_8730A_ROM+0x30>)
    5c64:	6013      	str	r3, [r2, #0]
    5c66:	4a0b      	ldr	r2, [pc, #44]	; (5c94 <ClrAllFWUsedIMR_8730A_ROM+0x34>)
    5c68:	6013      	str	r3, [r2, #0]
    5c6a:	4a0b      	ldr	r2, [pc, #44]	; (5c98 <ClrAllFWUsedIMR_8730A_ROM+0x38>)
    5c6c:	6013      	str	r3, [r2, #0]
    5c6e:	4a0b      	ldr	r2, [pc, #44]	; (5c9c <ClrAllFWUsedIMR_8730A_ROM+0x3c>)
    5c70:	6013      	str	r3, [r2, #0]
    5c72:	4a0b      	ldr	r2, [pc, #44]	; (5ca0 <ClrAllFWUsedIMR_8730A_ROM+0x40>)
    5c74:	6013      	str	r3, [r2, #0]
    5c76:	4a0b      	ldr	r2, [pc, #44]	; (5ca4 <ClrAllFWUsedIMR_8730A_ROM+0x44>)
    5c78:	6013      	str	r3, [r2, #0]
    5c7a:	4a0b      	ldr	r2, [pc, #44]	; (5ca8 <ClrAllFWUsedIMR_8730A_ROM+0x48>)
    5c7c:	6013      	str	r3, [r2, #0]
    5c7e:	4a0b      	ldr	r2, [pc, #44]	; (5cac <ClrAllFWUsedIMR_8730A_ROM+0x4c>)
    5c80:	6013      	str	r3, [r2, #0]
    5c82:	4a0b      	ldr	r2, [pc, #44]	; (5cb0 <ClrAllFWUsedIMR_8730A_ROM+0x50>)
    5c84:	6013      	str	r3, [r2, #0]
    5c86:	4a0b      	ldr	r2, [pc, #44]	; (5cb4 <ClrAllFWUsedIMR_8730A_ROM+0x54>)
    5c88:	6013      	str	r3, [r2, #0]
    5c8a:	4a0b      	ldr	r2, [pc, #44]	; (5cb8 <ClrAllFWUsedIMR_8730A_ROM+0x58>)
    5c8c:	6013      	str	r3, [r2, #0]
    5c8e:	4770      	bx	lr
    5c90:	43000130 	.word	0x43000130
    5c94:	430011c0 	.word	0x430011c0
    5c98:	43000120 	.word	0x43000120
    5c9c:	43001120 	.word	0x43001120
    5ca0:	43001128 	.word	0x43001128
    5ca4:	43001130 	.word	0x43001130
    5ca8:	43001168 	.word	0x43001168
    5cac:	43001170 	.word	0x43001170
    5cb0:	43000138 	.word	0x43000138
    5cb4:	43001138 	.word	0x43001138
    5cb8:	430011e0 	.word	0x430011e0

00005cbc <ClrAllFWUsedISR_8730A_ROM>:
    5cbc:	2301      	movs	r3, #1
    5cbe:	4a0c      	ldr	r2, [pc, #48]	; (5cf0 <ClrAllFWUsedISR_8730A_ROM+0x34>)
    5cc0:	425b      	negs	r3, r3
    5cc2:	6013      	str	r3, [r2, #0]
    5cc4:	4a0b      	ldr	r2, [pc, #44]	; (5cf4 <ClrAllFWUsedISR_8730A_ROM+0x38>)
    5cc6:	6013      	str	r3, [r2, #0]
    5cc8:	4a0b      	ldr	r2, [pc, #44]	; (5cf8 <ClrAllFWUsedISR_8730A_ROM+0x3c>)
    5cca:	6013      	str	r3, [r2, #0]
    5ccc:	4a0b      	ldr	r2, [pc, #44]	; (5cfc <ClrAllFWUsedISR_8730A_ROM+0x40>)
    5cce:	6013      	str	r3, [r2, #0]
    5cd0:	4a0b      	ldr	r2, [pc, #44]	; (5d00 <ClrAllFWUsedISR_8730A_ROM+0x44>)
    5cd2:	6013      	str	r3, [r2, #0]
    5cd4:	4a0b      	ldr	r2, [pc, #44]	; (5d04 <ClrAllFWUsedISR_8730A_ROM+0x48>)
    5cd6:	6013      	str	r3, [r2, #0]
    5cd8:	4a0b      	ldr	r2, [pc, #44]	; (5d08 <ClrAllFWUsedISR_8730A_ROM+0x4c>)
    5cda:	6013      	str	r3, [r2, #0]
    5cdc:	4a0b      	ldr	r2, [pc, #44]	; (5d0c <ClrAllFWUsedISR_8730A_ROM+0x50>)
    5cde:	6013      	str	r3, [r2, #0]
    5ce0:	4a0b      	ldr	r2, [pc, #44]	; (5d10 <ClrAllFWUsedISR_8730A_ROM+0x54>)
    5ce2:	6013      	str	r3, [r2, #0]
    5ce4:	4a0b      	ldr	r2, [pc, #44]	; (5d14 <ClrAllFWUsedISR_8730A_ROM+0x58>)
    5ce6:	6013      	str	r3, [r2, #0]
    5ce8:	4a0b      	ldr	r2, [pc, #44]	; (5d18 <ClrAllFWUsedISR_8730A_ROM+0x5c>)
    5cea:	6013      	str	r3, [r2, #0]
    5cec:	4770      	bx	lr
    5cee:	46c0      	nop			; (mov r8, r8)
    5cf0:	43000134 	.word	0x43000134
    5cf4:	430011c4 	.word	0x430011c4
    5cf8:	43000124 	.word	0x43000124
    5cfc:	43001124 	.word	0x43001124
    5d00:	43001134 	.word	0x43001134
    5d04:	4300116c 	.word	0x4300116c
    5d08:	43001174 	.word	0x43001174
    5d0c:	4300112c 	.word	0x4300112c
    5d10:	4300113c 	.word	0x4300113c
    5d14:	430011e4 	.word	0x430011e4
    5d18:	4300013c 	.word	0x4300013c

00005d1c <UpdateISRWlan_8730A_ROM>:
    5d1c:	4b20      	ldr	r3, [pc, #128]	; (5da0 <UpdateISRWlan_8730A_ROM+0x84>)
    5d1e:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    5d20:	469c      	mov	ip, r3
    5d22:	681a      	ldr	r2, [r3, #0]
    5d24:	4b1f      	ldr	r3, [pc, #124]	; (5da4 <UpdateISRWlan_8730A_ROM+0x88>)
    5d26:	4f20      	ldr	r7, [pc, #128]	; (5da8 <UpdateISRWlan_8730A_ROM+0x8c>)
    5d28:	681b      	ldr	r3, [r3, #0]
    5d2a:	4e20      	ldr	r6, [pc, #128]	; (5dac <UpdateISRWlan_8730A_ROM+0x90>)
    5d2c:	6819      	ldr	r1, [r3, #0]
    5d2e:	4d20      	ldr	r5, [pc, #128]	; (5db0 <UpdateISRWlan_8730A_ROM+0x94>)
    5d30:	4011      	ands	r1, r2
    5d32:	6099      	str	r1, [r3, #8]
    5d34:	683a      	ldr	r2, [r7, #0]
    5d36:	9101      	str	r1, [sp, #4]
    5d38:	6859      	ldr	r1, [r3, #4]
    5d3a:	4c1e      	ldr	r4, [pc, #120]	; (5db4 <UpdateISRWlan_8730A_ROM+0x98>)
    5d3c:	400a      	ands	r2, r1
    5d3e:	60da      	str	r2, [r3, #12]
    5d40:	6832      	ldr	r2, [r6, #0]
    5d42:	6919      	ldr	r1, [r3, #16]
    5d44:	481c      	ldr	r0, [pc, #112]	; (5db8 <UpdateISRWlan_8730A_ROM+0x9c>)
    5d46:	400a      	ands	r2, r1
    5d48:	629a      	str	r2, [r3, #40]	; 0x28
    5d4a:	682a      	ldr	r2, [r5, #0]
    5d4c:	6959      	ldr	r1, [r3, #20]
    5d4e:	400a      	ands	r2, r1
    5d50:	62da      	str	r2, [r3, #44]	; 0x2c
    5d52:	4a1a      	ldr	r2, [pc, #104]	; (5dbc <UpdateISRWlan_8730A_ROM+0xa0>)
    5d54:	6999      	ldr	r1, [r3, #24]
    5d56:	6812      	ldr	r2, [r2, #0]
    5d58:	400a      	ands	r2, r1
    5d5a:	631a      	str	r2, [r3, #48]	; 0x30
    5d5c:	6822      	ldr	r2, [r4, #0]
    5d5e:	69d9      	ldr	r1, [r3, #28]
    5d60:	400a      	ands	r2, r1
    5d62:	635a      	str	r2, [r3, #52]	; 0x34
    5d64:	6802      	ldr	r2, [r0, #0]
    5d66:	6a19      	ldr	r1, [r3, #32]
    5d68:	400a      	ands	r2, r1
    5d6a:	639a      	str	r2, [r3, #56]	; 0x38
    5d6c:	4a14      	ldr	r2, [pc, #80]	; (5dc0 <UpdateISRWlan_8730A_ROM+0xa4>)
    5d6e:	6a59      	ldr	r1, [r3, #36]	; 0x24
    5d70:	6812      	ldr	r2, [r2, #0]
    5d72:	400a      	ands	r2, r1
    5d74:	4661      	mov	r1, ip
    5d76:	63da      	str	r2, [r3, #60]	; 0x3c
    5d78:	9a01      	ldr	r2, [sp, #4]
    5d7a:	600a      	str	r2, [r1, #0]
    5d7c:	68d9      	ldr	r1, [r3, #12]
    5d7e:	4a10      	ldr	r2, [pc, #64]	; (5dc0 <UpdateISRWlan_8730A_ROM+0xa4>)
    5d80:	6039      	str	r1, [r7, #0]
    5d82:	6a99      	ldr	r1, [r3, #40]	; 0x28
    5d84:	6031      	str	r1, [r6, #0]
    5d86:	6ad9      	ldr	r1, [r3, #44]	; 0x2c
    5d88:	6029      	str	r1, [r5, #0]
    5d8a:	6b59      	ldr	r1, [r3, #52]	; 0x34
    5d8c:	6021      	str	r1, [r4, #0]
    5d8e:	6b99      	ldr	r1, [r3, #56]	; 0x38
    5d90:	6001      	str	r1, [r0, #0]
    5d92:	6bd9      	ldr	r1, [r3, #60]	; 0x3c
    5d94:	6011      	str	r1, [r2, #0]
    5d96:	6b1b      	ldr	r3, [r3, #48]	; 0x30
    5d98:	4a08      	ldr	r2, [pc, #32]	; (5dbc <UpdateISRWlan_8730A_ROM+0xa0>)
    5d9a:	6013      	str	r3, [r2, #0]
    5d9c:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}
    5d9e:	46c0      	nop			; (mov r8, r8)
    5da0:	43000134 	.word	0x43000134
    5da4:	23000a3c 	.word	0x23000a3c
    5da8:	430011c4 	.word	0x430011c4
    5dac:	43000124 	.word	0x43000124
    5db0:	43001124 	.word	0x43001124
    5db4:	43001134 	.word	0x43001134
    5db8:	4300116c 	.word	0x4300116c
    5dbc:	4300112c 	.word	0x4300112c
    5dc0:	43001174 	.word	0x43001174

00005dc4 <UpdateISRFT_8730A_ROM>:
    5dc4:	4b0b      	ldr	r3, [pc, #44]	; (5df4 <UpdateISRFT_8730A_ROM+0x30>)
    5dc6:	480c      	ldr	r0, [pc, #48]	; (5df8 <UpdateISRFT_8730A_ROM+0x34>)
    5dc8:	681b      	ldr	r3, [r3, #0]
    5dca:	6802      	ldr	r2, [r0, #0]
    5dcc:	6c19      	ldr	r1, [r3, #64]	; 0x40
    5dce:	b570      	push	{r4, r5, r6, lr}
    5dd0:	400a      	ands	r2, r1
    5dd2:	4d0a      	ldr	r5, [pc, #40]	; (5dfc <UpdateISRFT_8730A_ROM+0x38>)
    5dd4:	64da      	str	r2, [r3, #76]	; 0x4c
    5dd6:	682a      	ldr	r2, [r5, #0]
    5dd8:	6c59      	ldr	r1, [r3, #68]	; 0x44
    5dda:	4c09      	ldr	r4, [pc, #36]	; (5e00 <UpdateISRFT_8730A_ROM+0x3c>)
    5ddc:	400a      	ands	r2, r1
    5dde:	651a      	str	r2, [r3, #80]	; 0x50
    5de0:	6c9e      	ldr	r6, [r3, #72]	; 0x48
    5de2:	6821      	ldr	r1, [r4, #0]
    5de4:	4031      	ands	r1, r6
    5de6:	6559      	str	r1, [r3, #84]	; 0x54
    5de8:	602a      	str	r2, [r5, #0]
    5dea:	6d5a      	ldr	r2, [r3, #84]	; 0x54
    5dec:	6022      	str	r2, [r4, #0]
    5dee:	6cdb      	ldr	r3, [r3, #76]	; 0x4c
    5df0:	6003      	str	r3, [r0, #0]
    5df2:	bd70      	pop	{r4, r5, r6, pc}
    5df4:	23000a3c 	.word	0x23000a3c
    5df8:	4300013c 	.word	0x4300013c
    5dfc:	4300113c 	.word	0x4300113c
    5e00:	430011e4 	.word	0x430011e4

00005e04 <setwlanimr_8730A_ROM>:
    5e04:	2803      	cmp	r0, #3
    5e06:	d033      	beq.n	5e70 <setwlanimr_8730A_ROM+0x6c>
    5e08:	d805      	bhi.n	5e16 <setwlanimr_8730A_ROM+0x12>
    5e0a:	2801      	cmp	r0, #1
    5e0c:	d01c      	beq.n	5e48 <setwlanimr_8730A_ROM+0x44>
    5e0e:	2802      	cmp	r0, #2
    5e10:	d024      	beq.n	5e5c <setwlanimr_8730A_ROM+0x58>
    5e12:	b170      	cbz	r0, 5e32 <setwlanimr_8730A_ROM+0x2e>
    5e14:	4770      	bx	lr
    5e16:	2804      	cmp	r0, #4
    5e18:	d034      	beq.n	5e84 <setwlanimr_8730A_ROM+0x80>
    5e1a:	2805      	cmp	r0, #5
    5e1c:	d1fa      	bne.n	5e14 <setwlanimr_8730A_ROM+0x10>
    5e1e:	4b1e      	ldr	r3, [pc, #120]	; (5e98 <setwlanimr_8730A_ROM+0x94>)
    5e20:	681b      	ldr	r3, [r3, #0]
    5e22:	6a5a      	ldr	r2, [r3, #36]	; 0x24
    5e24:	430a      	orrs	r2, r1
    5e26:	625a      	str	r2, [r3, #36]	; 0x24
    5e28:	4a1c      	ldr	r2, [pc, #112]	; (5e9c <setwlanimr_8730A_ROM+0x98>)
    5e2a:	6011      	str	r1, [r2, #0]
    5e2c:	6a5a      	ldr	r2, [r3, #36]	; 0x24
    5e2e:	4b1c      	ldr	r3, [pc, #112]	; (5ea0 <setwlanimr_8730A_ROM+0x9c>)
    5e30:	e008      	b.n	5e44 <setwlanimr_8730A_ROM+0x40>
    5e32:	4b19      	ldr	r3, [pc, #100]	; (5e98 <setwlanimr_8730A_ROM+0x94>)
    5e34:	681b      	ldr	r3, [r3, #0]
    5e36:	691a      	ldr	r2, [r3, #16]
    5e38:	430a      	orrs	r2, r1
    5e3a:	611a      	str	r2, [r3, #16]
    5e3c:	4a19      	ldr	r2, [pc, #100]	; (5ea4 <setwlanimr_8730A_ROM+0xa0>)
    5e3e:	6011      	str	r1, [r2, #0]
    5e40:	691a      	ldr	r2, [r3, #16]
    5e42:	4b19      	ldr	r3, [pc, #100]	; (5ea8 <setwlanimr_8730A_ROM+0xa4>)
    5e44:	601a      	str	r2, [r3, #0]
    5e46:	e7e5      	b.n	5e14 <setwlanimr_8730A_ROM+0x10>
    5e48:	4b13      	ldr	r3, [pc, #76]	; (5e98 <setwlanimr_8730A_ROM+0x94>)
    5e4a:	681b      	ldr	r3, [r3, #0]
    5e4c:	695a      	ldr	r2, [r3, #20]
    5e4e:	430a      	orrs	r2, r1
    5e50:	615a      	str	r2, [r3, #20]
    5e52:	4a16      	ldr	r2, [pc, #88]	; (5eac <setwlanimr_8730A_ROM+0xa8>)
    5e54:	6011      	str	r1, [r2, #0]
    5e56:	695a      	ldr	r2, [r3, #20]
    5e58:	4b15      	ldr	r3, [pc, #84]	; (5eb0 <setwlanimr_8730A_ROM+0xac>)
    5e5a:	e7f3      	b.n	5e44 <setwlanimr_8730A_ROM+0x40>
    5e5c:	4b0e      	ldr	r3, [pc, #56]	; (5e98 <setwlanimr_8730A_ROM+0x94>)
    5e5e:	681b      	ldr	r3, [r3, #0]
    5e60:	699a      	ldr	r2, [r3, #24]
    5e62:	430a      	orrs	r2, r1
    5e64:	619a      	str	r2, [r3, #24]
    5e66:	4a13      	ldr	r2, [pc, #76]	; (5eb4 <setwlanimr_8730A_ROM+0xb0>)
    5e68:	6011      	str	r1, [r2, #0]
    5e6a:	699a      	ldr	r2, [r3, #24]
    5e6c:	4b12      	ldr	r3, [pc, #72]	; (5eb8 <setwlanimr_8730A_ROM+0xb4>)
    5e6e:	e7e9      	b.n	5e44 <setwlanimr_8730A_ROM+0x40>
    5e70:	4b09      	ldr	r3, [pc, #36]	; (5e98 <setwlanimr_8730A_ROM+0x94>)
    5e72:	681b      	ldr	r3, [r3, #0]
    5e74:	69da      	ldr	r2, [r3, #28]
    5e76:	430a      	orrs	r2, r1
    5e78:	61da      	str	r2, [r3, #28]
    5e7a:	4a10      	ldr	r2, [pc, #64]	; (5ebc <setwlanimr_8730A_ROM+0xb8>)
    5e7c:	6011      	str	r1, [r2, #0]
    5e7e:	69da      	ldr	r2, [r3, #28]
    5e80:	4b0f      	ldr	r3, [pc, #60]	; (5ec0 <setwlanimr_8730A_ROM+0xbc>)
    5e82:	e7df      	b.n	5e44 <setwlanimr_8730A_ROM+0x40>
    5e84:	4b04      	ldr	r3, [pc, #16]	; (5e98 <setwlanimr_8730A_ROM+0x94>)
    5e86:	681b      	ldr	r3, [r3, #0]
    5e88:	6a1a      	ldr	r2, [r3, #32]
    5e8a:	430a      	orrs	r2, r1
    5e8c:	621a      	str	r2, [r3, #32]
    5e8e:	4a0d      	ldr	r2, [pc, #52]	; (5ec4 <setwlanimr_8730A_ROM+0xc0>)
    5e90:	6011      	str	r1, [r2, #0]
    5e92:	6a1a      	ldr	r2, [r3, #32]
    5e94:	4b0c      	ldr	r3, [pc, #48]	; (5ec8 <setwlanimr_8730A_ROM+0xc4>)
    5e96:	e7d5      	b.n	5e44 <setwlanimr_8730A_ROM+0x40>
    5e98:	23000a3c 	.word	0x23000a3c
    5e9c:	43001174 	.word	0x43001174
    5ea0:	43001170 	.word	0x43001170
    5ea4:	43000124 	.word	0x43000124
    5ea8:	43000120 	.word	0x43000120
    5eac:	43001124 	.word	0x43001124
    5eb0:	43001120 	.word	0x43001120
    5eb4:	4300112c 	.word	0x4300112c
    5eb8:	43001128 	.word	0x43001128
    5ebc:	43001134 	.word	0x43001134
    5ec0:	43001130 	.word	0x43001130
    5ec4:	4300116c 	.word	0x4300116c
    5ec8:	43001168 	.word	0x43001168

00005ecc <setfwimr_8730A_ROM>:
    5ecc:	b110      	cbz	r0, 5ed4 <setfwimr_8730A_ROM+0x8>
    5ece:	2801      	cmp	r0, #1
    5ed0:	d00b      	beq.n	5eea <setfwimr_8730A_ROM+0x1e>
    5ed2:	4770      	bx	lr
    5ed4:	4b0a      	ldr	r3, [pc, #40]	; (5f00 <setfwimr_8730A_ROM+0x34>)
    5ed6:	681b      	ldr	r3, [r3, #0]
    5ed8:	681a      	ldr	r2, [r3, #0]
    5eda:	430a      	orrs	r2, r1
    5edc:	601a      	str	r2, [r3, #0]
    5ede:	4a09      	ldr	r2, [pc, #36]	; (5f04 <setfwimr_8730A_ROM+0x38>)
    5ee0:	6011      	str	r1, [r2, #0]
    5ee2:	681a      	ldr	r2, [r3, #0]
    5ee4:	4b08      	ldr	r3, [pc, #32]	; (5f08 <setfwimr_8730A_ROM+0x3c>)
    5ee6:	601a      	str	r2, [r3, #0]
    5ee8:	e7f3      	b.n	5ed2 <setfwimr_8730A_ROM+0x6>
    5eea:	4b05      	ldr	r3, [pc, #20]	; (5f00 <setfwimr_8730A_ROM+0x34>)
    5eec:	681b      	ldr	r3, [r3, #0]
    5eee:	685a      	ldr	r2, [r3, #4]
    5ef0:	430a      	orrs	r2, r1
    5ef2:	605a      	str	r2, [r3, #4]
    5ef4:	4a05      	ldr	r2, [pc, #20]	; (5f0c <setfwimr_8730A_ROM+0x40>)
    5ef6:	6011      	str	r1, [r2, #0]
    5ef8:	685a      	ldr	r2, [r3, #4]
    5efa:	4b05      	ldr	r3, [pc, #20]	; (5f10 <setfwimr_8730A_ROM+0x44>)
    5efc:	e7f3      	b.n	5ee6 <setfwimr_8730A_ROM+0x1a>
    5efe:	46c0      	nop			; (mov r8, r8)
    5f00:	23000a3c 	.word	0x23000a3c
    5f04:	43000134 	.word	0x43000134
    5f08:	43000130 	.word	0x43000130
    5f0c:	430011c4 	.word	0x430011c4
    5f10:	430011c0 	.word	0x430011c0

00005f14 <clrfwimr_8730A_ROM>:
    5f14:	b110      	cbz	r0, 5f1c <clrfwimr_8730A_ROM+0x8>
    5f16:	2801      	cmp	r0, #1
    5f18:	d00a      	beq.n	5f30 <clrfwimr_8730A_ROM+0x1c>
    5f1a:	4770      	bx	lr
    5f1c:	4b09      	ldr	r3, [pc, #36]	; (5f44 <clrfwimr_8730A_ROM+0x30>)
    5f1e:	681a      	ldr	r2, [r3, #0]
    5f20:	6813      	ldr	r3, [r2, #0]
    5f22:	438b      	bics	r3, r1
    5f24:	6013      	str	r3, [r2, #0]
    5f26:	4a08      	ldr	r2, [pc, #32]	; (5f48 <clrfwimr_8730A_ROM+0x34>)
    5f28:	6013      	str	r3, [r2, #0]
    5f2a:	4b08      	ldr	r3, [pc, #32]	; (5f4c <clrfwimr_8730A_ROM+0x38>)
    5f2c:	6019      	str	r1, [r3, #0]
    5f2e:	e7f4      	b.n	5f1a <clrfwimr_8730A_ROM+0x6>
    5f30:	4b04      	ldr	r3, [pc, #16]	; (5f44 <clrfwimr_8730A_ROM+0x30>)
    5f32:	681a      	ldr	r2, [r3, #0]
    5f34:	6853      	ldr	r3, [r2, #4]
    5f36:	438b      	bics	r3, r1
    5f38:	6053      	str	r3, [r2, #4]
    5f3a:	4a05      	ldr	r2, [pc, #20]	; (5f50 <clrfwimr_8730A_ROM+0x3c>)
    5f3c:	6013      	str	r3, [r2, #0]
    5f3e:	4b05      	ldr	r3, [pc, #20]	; (5f54 <clrfwimr_8730A_ROM+0x40>)
    5f40:	e7f4      	b.n	5f2c <clrfwimr_8730A_ROM+0x18>
    5f42:	46c0      	nop			; (mov r8, r8)
    5f44:	23000a3c 	.word	0x23000a3c
    5f48:	43000130 	.word	0x43000130
    5f4c:	43000134 	.word	0x43000134
    5f50:	430011c0 	.word	0x430011c0
    5f54:	430011c4 	.word	0x430011c4

00005f58 <clrwlanimr_8730A_ROM>:
    5f58:	2803      	cmp	r0, #3
    5f5a:	d02f      	beq.n	5fbc <clrwlanimr_8730A_ROM+0x64>
    5f5c:	d805      	bhi.n	5f6a <clrwlanimr_8730A_ROM+0x12>
    5f5e:	2801      	cmp	r0, #1
    5f60:	d01a      	beq.n	5f98 <clrwlanimr_8730A_ROM+0x40>
    5f62:	2802      	cmp	r0, #2
    5f64:	d021      	beq.n	5faa <clrwlanimr_8730A_ROM+0x52>
    5f66:	b168      	cbz	r0, 5f84 <clrwlanimr_8730A_ROM+0x2c>
    5f68:	4770      	bx	lr
    5f6a:	2804      	cmp	r0, #4
    5f6c:	d02f      	beq.n	5fce <clrwlanimr_8730A_ROM+0x76>
    5f6e:	2805      	cmp	r0, #5
    5f70:	d1fa      	bne.n	5f68 <clrwlanimr_8730A_ROM+0x10>
    5f72:	4b1b      	ldr	r3, [pc, #108]	; (5fe0 <clrwlanimr_8730A_ROM+0x88>)
    5f74:	681a      	ldr	r2, [r3, #0]
    5f76:	6a53      	ldr	r3, [r2, #36]	; 0x24
    5f78:	438b      	bics	r3, r1
    5f7a:	6253      	str	r3, [r2, #36]	; 0x24
    5f7c:	4a19      	ldr	r2, [pc, #100]	; (5fe4 <clrwlanimr_8730A_ROM+0x8c>)
    5f7e:	6013      	str	r3, [r2, #0]
    5f80:	4b19      	ldr	r3, [pc, #100]	; (5fe8 <clrwlanimr_8730A_ROM+0x90>)
    5f82:	e007      	b.n	5f94 <clrwlanimr_8730A_ROM+0x3c>
    5f84:	4b16      	ldr	r3, [pc, #88]	; (5fe0 <clrwlanimr_8730A_ROM+0x88>)
    5f86:	681a      	ldr	r2, [r3, #0]
    5f88:	6913      	ldr	r3, [r2, #16]
    5f8a:	438b      	bics	r3, r1
    5f8c:	6113      	str	r3, [r2, #16]
    5f8e:	4a17      	ldr	r2, [pc, #92]	; (5fec <clrwlanimr_8730A_ROM+0x94>)
    5f90:	6013      	str	r3, [r2, #0]
    5f92:	4b17      	ldr	r3, [pc, #92]	; (5ff0 <clrwlanimr_8730A_ROM+0x98>)
    5f94:	6019      	str	r1, [r3, #0]
    5f96:	e7e7      	b.n	5f68 <clrwlanimr_8730A_ROM+0x10>
    5f98:	4b11      	ldr	r3, [pc, #68]	; (5fe0 <clrwlanimr_8730A_ROM+0x88>)
    5f9a:	681a      	ldr	r2, [r3, #0]
    5f9c:	6953      	ldr	r3, [r2, #20]
    5f9e:	438b      	bics	r3, r1
    5fa0:	6153      	str	r3, [r2, #20]
    5fa2:	4a14      	ldr	r2, [pc, #80]	; (5ff4 <clrwlanimr_8730A_ROM+0x9c>)
    5fa4:	6013      	str	r3, [r2, #0]
    5fa6:	4b14      	ldr	r3, [pc, #80]	; (5ff8 <clrwlanimr_8730A_ROM+0xa0>)
    5fa8:	e7f4      	b.n	5f94 <clrwlanimr_8730A_ROM+0x3c>
    5faa:	4b0d      	ldr	r3, [pc, #52]	; (5fe0 <clrwlanimr_8730A_ROM+0x88>)
    5fac:	681a      	ldr	r2, [r3, #0]
    5fae:	6993      	ldr	r3, [r2, #24]
    5fb0:	438b      	bics	r3, r1
    5fb2:	6193      	str	r3, [r2, #24]
    5fb4:	4a11      	ldr	r2, [pc, #68]	; (5ffc <clrwlanimr_8730A_ROM+0xa4>)
    5fb6:	6013      	str	r3, [r2, #0]
    5fb8:	4b11      	ldr	r3, [pc, #68]	; (6000 <clrwlanimr_8730A_ROM+0xa8>)
    5fba:	e7eb      	b.n	5f94 <clrwlanimr_8730A_ROM+0x3c>
    5fbc:	4b08      	ldr	r3, [pc, #32]	; (5fe0 <clrwlanimr_8730A_ROM+0x88>)
    5fbe:	681a      	ldr	r2, [r3, #0]
    5fc0:	69d3      	ldr	r3, [r2, #28]
    5fc2:	438b      	bics	r3, r1
    5fc4:	61d3      	str	r3, [r2, #28]
    5fc6:	4a0f      	ldr	r2, [pc, #60]	; (6004 <clrwlanimr_8730A_ROM+0xac>)
    5fc8:	6013      	str	r3, [r2, #0]
    5fca:	4b0f      	ldr	r3, [pc, #60]	; (6008 <clrwlanimr_8730A_ROM+0xb0>)
    5fcc:	e7e2      	b.n	5f94 <clrwlanimr_8730A_ROM+0x3c>
    5fce:	4b04      	ldr	r3, [pc, #16]	; (5fe0 <clrwlanimr_8730A_ROM+0x88>)
    5fd0:	681a      	ldr	r2, [r3, #0]
    5fd2:	6a13      	ldr	r3, [r2, #32]
    5fd4:	438b      	bics	r3, r1
    5fd6:	6213      	str	r3, [r2, #32]
    5fd8:	4a0c      	ldr	r2, [pc, #48]	; (600c <clrwlanimr_8730A_ROM+0xb4>)
    5fda:	6013      	str	r3, [r2, #0]
    5fdc:	4b0c      	ldr	r3, [pc, #48]	; (6010 <clrwlanimr_8730A_ROM+0xb8>)
    5fde:	e7d9      	b.n	5f94 <clrwlanimr_8730A_ROM+0x3c>
    5fe0:	23000a3c 	.word	0x23000a3c
    5fe4:	43001170 	.word	0x43001170
    5fe8:	43001174 	.word	0x43001174
    5fec:	43000120 	.word	0x43000120
    5ff0:	43000124 	.word	0x43000124
    5ff4:	43001120 	.word	0x43001120
    5ff8:	43001124 	.word	0x43001124
    5ffc:	43001128 	.word	0x43001128
    6000:	4300112c 	.word	0x4300112c
    6004:	43001130 	.word	0x43001130
    6008:	43001134 	.word	0x43001134
    600c:	43001168 	.word	0x43001168
    6010:	4300116c 	.word	0x4300116c

00006014 <setftimr_8730A_ROM>:
    6014:	2801      	cmp	r0, #1
    6016:	d00d      	beq.n	6034 <setftimr_8730A_ROM+0x20>
    6018:	2802      	cmp	r0, #2
    601a:	d015      	beq.n	6048 <setftimr_8730A_ROM+0x34>
    601c:	b948      	cbnz	r0, 6032 <setftimr_8730A_ROM+0x1e>
    601e:	4b0f      	ldr	r3, [pc, #60]	; (605c <setftimr_8730A_ROM+0x48>)
    6020:	681b      	ldr	r3, [r3, #0]
    6022:	6c1a      	ldr	r2, [r3, #64]	; 0x40
    6024:	430a      	orrs	r2, r1
    6026:	641a      	str	r2, [r3, #64]	; 0x40
    6028:	4a0d      	ldr	r2, [pc, #52]	; (6060 <setftimr_8730A_ROM+0x4c>)
    602a:	6011      	str	r1, [r2, #0]
    602c:	6c1a      	ldr	r2, [r3, #64]	; 0x40
    602e:	4b0d      	ldr	r3, [pc, #52]	; (6064 <setftimr_8730A_ROM+0x50>)
    6030:	601a      	str	r2, [r3, #0]
    6032:	4770      	bx	lr
    6034:	4b09      	ldr	r3, [pc, #36]	; (605c <setftimr_8730A_ROM+0x48>)
    6036:	681b      	ldr	r3, [r3, #0]
    6038:	6c5a      	ldr	r2, [r3, #68]	; 0x44
    603a:	430a      	orrs	r2, r1
    603c:	645a      	str	r2, [r3, #68]	; 0x44
    603e:	4a0a      	ldr	r2, [pc, #40]	; (6068 <setftimr_8730A_ROM+0x54>)
    6040:	6011      	str	r1, [r2, #0]
    6042:	6c5a      	ldr	r2, [r3, #68]	; 0x44
    6044:	4b09      	ldr	r3, [pc, #36]	; (606c <setftimr_8730A_ROM+0x58>)
    6046:	e7f3      	b.n	6030 <setftimr_8730A_ROM+0x1c>
    6048:	4b04      	ldr	r3, [pc, #16]	; (605c <setftimr_8730A_ROM+0x48>)
    604a:	681b      	ldr	r3, [r3, #0]
    604c:	6c9a      	ldr	r2, [r3, #72]	; 0x48
    604e:	430a      	orrs	r2, r1
    6050:	649a      	str	r2, [r3, #72]	; 0x48
    6052:	4a07      	ldr	r2, [pc, #28]	; (6070 <setftimr_8730A_ROM+0x5c>)
    6054:	6011      	str	r1, [r2, #0]
    6056:	6c9a      	ldr	r2, [r3, #72]	; 0x48
    6058:	4b06      	ldr	r3, [pc, #24]	; (6074 <setftimr_8730A_ROM+0x60>)
    605a:	e7e9      	b.n	6030 <setftimr_8730A_ROM+0x1c>
    605c:	23000a3c 	.word	0x23000a3c
    6060:	4300013c 	.word	0x4300013c
    6064:	43000138 	.word	0x43000138
    6068:	4300113c 	.word	0x4300113c
    606c:	43001138 	.word	0x43001138
    6070:	430011e4 	.word	0x430011e4
    6074:	430011e0 	.word	0x430011e0

00006078 <clrftimr_8730A_ROM>:
    6078:	2801      	cmp	r0, #1
    607a:	d00c      	beq.n	6096 <clrftimr_8730A_ROM+0x1e>
    607c:	2802      	cmp	r0, #2
    607e:	d013      	beq.n	60a8 <clrftimr_8730A_ROM+0x30>
    6080:	b940      	cbnz	r0, 6094 <clrftimr_8730A_ROM+0x1c>
    6082:	4b0e      	ldr	r3, [pc, #56]	; (60bc <clrftimr_8730A_ROM+0x44>)
    6084:	681a      	ldr	r2, [r3, #0]
    6086:	6c13      	ldr	r3, [r2, #64]	; 0x40
    6088:	438b      	bics	r3, r1
    608a:	6413      	str	r3, [r2, #64]	; 0x40
    608c:	4a0c      	ldr	r2, [pc, #48]	; (60c0 <clrftimr_8730A_ROM+0x48>)
    608e:	6013      	str	r3, [r2, #0]
    6090:	4b0c      	ldr	r3, [pc, #48]	; (60c4 <clrftimr_8730A_ROM+0x4c>)
    6092:	6019      	str	r1, [r3, #0]
    6094:	4770      	bx	lr
    6096:	4b09      	ldr	r3, [pc, #36]	; (60bc <clrftimr_8730A_ROM+0x44>)
    6098:	681a      	ldr	r2, [r3, #0]
    609a:	6c53      	ldr	r3, [r2, #68]	; 0x44
    609c:	438b      	bics	r3, r1
    609e:	6453      	str	r3, [r2, #68]	; 0x44
    60a0:	4a09      	ldr	r2, [pc, #36]	; (60c8 <clrftimr_8730A_ROM+0x50>)
    60a2:	6013      	str	r3, [r2, #0]
    60a4:	4b09      	ldr	r3, [pc, #36]	; (60cc <clrftimr_8730A_ROM+0x54>)
    60a6:	e7f4      	b.n	6092 <clrftimr_8730A_ROM+0x1a>
    60a8:	4b04      	ldr	r3, [pc, #16]	; (60bc <clrftimr_8730A_ROM+0x44>)
    60aa:	681a      	ldr	r2, [r3, #0]
    60ac:	6c93      	ldr	r3, [r2, #72]	; 0x48
    60ae:	438b      	bics	r3, r1
    60b0:	6493      	str	r3, [r2, #72]	; 0x48
    60b2:	4a07      	ldr	r2, [pc, #28]	; (60d0 <clrftimr_8730A_ROM+0x58>)
    60b4:	6013      	str	r3, [r2, #0]
    60b6:	4b07      	ldr	r3, [pc, #28]	; (60d4 <clrftimr_8730A_ROM+0x5c>)
    60b8:	e7eb      	b.n	6092 <clrftimr_8730A_ROM+0x1a>
    60ba:	46c0      	nop			; (mov r8, r8)
    60bc:	23000a3c 	.word	0x23000a3c
    60c0:	43000138 	.word	0x43000138
    60c4:	4300013c 	.word	0x4300013c
    60c8:	43001138 	.word	0x43001138
    60cc:	4300113c 	.word	0x4300113c
    60d0:	430011e0 	.word	0x430011e0
    60d4:	430011e4 	.word	0x430011e4

000060d8 <ChangeTransmiteRate_8730A_ROM>:
    60d8:	2201      	movs	r2, #1
    60da:	b510      	push	{r4, lr}
    60dc:	4b0f      	ldr	r3, [pc, #60]	; (611c <ChangeTransmiteRate_8730A_ROM+0x44>)
    60de:	2403      	movs	r4, #3
    60e0:	18c3      	adds	r3, r0, r3
    60e2:	7819      	ldrb	r1, [r3, #0]
    60e4:	430a      	orrs	r2, r1
    60e6:	2104      	movs	r1, #4
    60e8:	701a      	strb	r2, [r3, #0]
    60ea:	781a      	ldrb	r2, [r3, #0]
    60ec:	438a      	bics	r2, r1
    60ee:	701a      	strb	r2, [r3, #0]
    60f0:	2260      	movs	r2, #96	; 0x60
    60f2:	4b0b      	ldr	r3, [pc, #44]	; (6120 <ChangeTransmiteRate_8730A_ROM+0x48>)
    60f4:	18c3      	adds	r3, r0, r3
    60f6:	701a      	strb	r2, [r3, #0]
    60f8:	781a      	ldrb	r2, [r3, #0]
    60fa:	43a2      	bics	r2, r4
    60fc:	701a      	strb	r2, [r3, #0]
    60fe:	4b09      	ldr	r3, [pc, #36]	; (6124 <ChangeTransmiteRate_8730A_ROM+0x4c>)
    6100:	18c3      	adds	r3, r0, r3
    6102:	781a      	ldrb	r2, [r3, #0]
    6104:	2200      	movs	r2, #0
    6106:	701a      	strb	r2, [r3, #0]
    6108:	4b07      	ldr	r3, [pc, #28]	; (6128 <ChangeTransmiteRate_8730A_ROM+0x50>)
    610a:	18c1      	adds	r1, r0, r3
    610c:	780b      	ldrb	r3, [r1, #0]
    610e:	43a3      	bics	r3, r4
    6110:	700b      	strb	r3, [r1, #0]
    6112:	4b06      	ldr	r3, [pc, #24]	; (612c <ChangeTransmiteRate_8730A_ROM+0x54>)
    6114:	18c0      	adds	r0, r0, r3
    6116:	7803      	ldrb	r3, [r0, #0]
    6118:	7002      	strb	r2, [r0, #0]
    611a:	bd10      	pop	{r4, pc}
    611c:	4302000d 	.word	0x4302000d
    6120:	43020012 	.word	0x43020012
    6124:	43020011 	.word	0x43020011
    6128:	4302001a 	.word	0x4302001a
    612c:	43020019 	.word	0x43020019

00006130 <PowerBitSetting_8730A_ROM>:
    6130:	b510      	push	{r4, lr}
    6132:	2801      	cmp	r0, #1
    6134:	d81f      	bhi.n	6176 <PowerBitSetting_8730A_ROM+0x46>
    6136:	243f      	movs	r4, #63	; 0x3f
    6138:	4b14      	ldr	r3, [pc, #80]	; (618c <PowerBitSetting_8730A_ROM+0x5c>)
    613a:	18cb      	adds	r3, r1, r3
    613c:	781a      	ldrb	r2, [r3, #0]
    613e:	4022      	ands	r2, r4
    6140:	701a      	strb	r2, [r3, #0]
    6142:	4a13      	ldr	r2, [pc, #76]	; (6190 <PowerBitSetting_8730A_ROM+0x60>)
    6144:	3c2f      	subs	r4, #47	; 0x2f
    6146:	188a      	adds	r2, r1, r2
    6148:	b1c0      	cbz	r0, 617c <PowerBitSetting_8730A_ROM+0x4c>
    614a:	7810      	ldrb	r0, [r2, #0]
    614c:	4304      	orrs	r4, r0
    614e:	2080      	movs	r0, #128	; 0x80
    6150:	7014      	strb	r4, [r2, #0]
    6152:	781a      	ldrb	r2, [r3, #0]
    6154:	4302      	orrs	r2, r0
    6156:	701a      	strb	r2, [r3, #0]
    6158:	4b0e      	ldr	r3, [pc, #56]	; (6194 <PowerBitSetting_8730A_ROM+0x64>)
    615a:	681b      	ldr	r3, [r3, #0]
    615c:	791b      	ldrb	r3, [r3, #4]
    615e:	069b      	lsls	r3, r3, #26
    6160:	d509      	bpl.n	6176 <PowerBitSetting_8730A_ROM+0x46>
    6162:	4b0d      	ldr	r3, [pc, #52]	; (6198 <PowerBitSetting_8730A_ROM+0x68>)
    6164:	4a0d      	ldr	r2, [pc, #52]	; (619c <PowerBitSetting_8730A_ROM+0x6c>)
    6166:	681b      	ldr	r3, [r3, #0]
    6168:	1889      	adds	r1, r1, r2
    616a:	7bdb      	ldrb	r3, [r3, #15]
    616c:	2b1f      	cmp	r3, #31
    616e:	d803      	bhi.n	6178 <PowerBitSetting_8730A_ROM+0x48>
    6170:	009b      	lsls	r3, r3, #2
    6172:	b2db      	uxtb	r3, r3
    6174:	700b      	strb	r3, [r1, #0]
    6176:	bd10      	pop	{r4, pc}
    6178:	237f      	movs	r3, #127	; 0x7f
    617a:	e7fb      	b.n	6174 <PowerBitSetting_8730A_ROM+0x44>
    617c:	7811      	ldrb	r1, [r2, #0]
    617e:	43a1      	bics	r1, r4
    6180:	7011      	strb	r1, [r2, #0]
    6182:	2240      	movs	r2, #64	; 0x40
    6184:	7819      	ldrb	r1, [r3, #0]
    6186:	430a      	orrs	r2, r1
    6188:	701a      	strb	r2, [r3, #0]
    618a:	e7f4      	b.n	6176 <PowerBitSetting_8730A_ROM+0x46>
    618c:	43020009 	.word	0x43020009
    6190:	43020029 	.word	0x43020029
    6194:	23000a14 	.word	0x23000a14
    6198:	23000a18 	.word	0x23000a18
    619c:	4302002b 	.word	0x4302002b

000061a0 <IssuePSPoll_8730A_ROM>:
    61a0:	b510      	push	{r4, lr}
    61a2:	4b10      	ldr	r3, [pc, #64]	; (61e4 <IssuePSPoll_8730A_ROM+0x44>)
    61a4:	781b      	ldrb	r3, [r3, #0]
    61a6:	b2d9      	uxtb	r1, r3
    61a8:	b1a3      	cbz	r3, 61d4 <IssuePSPoll_8730A_ROM+0x34>
    61aa:	4b0f      	ldr	r3, [pc, #60]	; (61e8 <IssuePSPoll_8730A_ROM+0x48>)
    61ac:	681b      	ldr	r3, [r3, #0]
    61ae:	4798      	blx	r3
    61b0:	2801      	cmp	r0, #1
    61b2:	d10e      	bne.n	61d2 <IssuePSPoll_8730A_ROM+0x32>
    61b4:	4b0d      	ldr	r3, [pc, #52]	; (61ec <IssuePSPoll_8730A_ROM+0x4c>)
    61b6:	2218      	movs	r2, #24
    61b8:	681b      	ldr	r3, [r3, #0]
    61ba:	2100      	movs	r1, #0
    61bc:	7858      	ldrb	r0, [r3, #1]
    61be:	4b0c      	ldr	r3, [pc, #48]	; (61f0 <IssuePSPoll_8730A_ROM+0x50>)
    61c0:	681c      	ldr	r4, [r3, #0]
    61c2:	2309      	movs	r3, #9
    61c4:	47a0      	blx	r4
    61c6:	2380      	movs	r3, #128	; 0x80
    61c8:	4a0a      	ldr	r2, [pc, #40]	; (61f4 <IssuePSPoll_8730A_ROM+0x54>)
    61ca:	059b      	lsls	r3, r3, #22
    61cc:	6811      	ldr	r1, [r2, #0]
    61ce:	430b      	orrs	r3, r1
    61d0:	6013      	str	r3, [r2, #0]
    61d2:	bd10      	pop	{r4, pc}
    61d4:	4b05      	ldr	r3, [pc, #20]	; (61ec <IssuePSPoll_8730A_ROM+0x4c>)
    61d6:	2218      	movs	r2, #24
    61d8:	681b      	ldr	r3, [r3, #0]
    61da:	7858      	ldrb	r0, [r3, #1]
    61dc:	4b04      	ldr	r3, [pc, #16]	; (61f0 <IssuePSPoll_8730A_ROM+0x50>)
    61de:	681c      	ldr	r4, [r3, #0]
    61e0:	2309      	movs	r3, #9
    61e2:	e7ef      	b.n	61c4 <IssuePSPoll_8730A_ROM+0x24>
    61e4:	4300041d 	.word	0x4300041d
    61e8:	230007e4 	.word	0x230007e4
    61ec:	23000a38 	.word	0x23000a38
    61f0:	230007dc 	.word	0x230007dc
    61f4:	4300041c 	.word	0x4300041c

000061f8 <ChkandChangePS_8730A_ROM>:
    61f8:	b510      	push	{r4, lr}
    61fa:	0004      	movs	r4, r0
    61fc:	2801      	cmp	r0, #1
    61fe:	d820      	bhi.n	6242 <ChkandChangePS_8730A_ROM+0x4a>
    6200:	4a1d      	ldr	r2, [pc, #116]	; (6278 <ChkandChangePS_8730A_ROM+0x80>)
    6202:	4b1e      	ldr	r3, [pc, #120]	; (627c <ChkandChangePS_8730A_ROM+0x84>)
    6204:	6812      	ldr	r2, [r2, #0]
    6206:	781b      	ldrb	r3, [r3, #0]
    6208:	7892      	ldrb	r2, [r2, #2]
    620a:	429a      	cmp	r2, r3
    620c:	d119      	bne.n	6242 <ChkandChangePS_8730A_ROM+0x4a>
    620e:	491c      	ldr	r1, [pc, #112]	; (6280 <ChkandChangePS_8730A_ROM+0x88>)
    6210:	680b      	ldr	r3, [r1, #0]
    6212:	7a1a      	ldrb	r2, [r3, #8]
    6214:	2a0e      	cmp	r2, #14
    6216:	d115      	bne.n	6244 <ChkandChangePS_8730A_ROM+0x4c>
    6218:	b998      	cbnz	r0, 6242 <ChkandChangePS_8730A_ROM+0x4a>
    621a:	2140      	movs	r1, #64	; 0x40
    621c:	781a      	ldrb	r2, [r3, #0]
    621e:	2001      	movs	r0, #1
    6220:	438a      	bics	r2, r1
    6222:	701a      	strb	r2, [r3, #0]
    6224:	4a17      	ldr	r2, [pc, #92]	; (6284 <ChkandChangePS_8730A_ROM+0x8c>)
    6226:	313f      	adds	r1, #63	; 0x3f
    6228:	7813      	ldrb	r3, [r2, #0]
    622a:	400b      	ands	r3, r1
    622c:	7013      	strb	r3, [r2, #0]
    622e:	4b16      	ldr	r3, [pc, #88]	; (6288 <ChkandChangePS_8730A_ROM+0x90>)
    6230:	3973      	subs	r1, #115	; 0x73
    6232:	681b      	ldr	r3, [r3, #0]
    6234:	4798      	blx	r3
    6236:	0022      	movs	r2, r4
    6238:	0021      	movs	r1, r4
    623a:	20ff      	movs	r0, #255	; 0xff
    623c:	4b13      	ldr	r3, [pc, #76]	; (628c <ChkandChangePS_8730A_ROM+0x94>)
    623e:	681b      	ldr	r3, [r3, #0]
    6240:	4798      	blx	r3
    6242:	bd10      	pop	{r4, pc}
    6244:	2a06      	cmp	r2, #6
    6246:	d1fc      	bne.n	6242 <ChkandChangePS_8730A_ROM+0x4a>
    6248:	2800      	cmp	r0, #0
    624a:	d0fa      	beq.n	6242 <ChkandChangePS_8730A_ROM+0x4a>
    624c:	2020      	movs	r0, #32
    624e:	781a      	ldrb	r2, [r3, #0]
    6250:	4382      	bics	r2, r0
    6252:	701a      	strb	r2, [r3, #0]
    6254:	2240      	movs	r2, #64	; 0x40
    6256:	4b0b      	ldr	r3, [pc, #44]	; (6284 <ChkandChangePS_8730A_ROM+0x8c>)
    6258:	7818      	ldrb	r0, [r3, #0]
    625a:	4302      	orrs	r2, r0
    625c:	2080      	movs	r0, #128	; 0x80
    625e:	701a      	strb	r2, [r3, #0]
    6260:	781a      	ldrb	r2, [r3, #0]
    6262:	4302      	orrs	r2, r0
    6264:	701a      	strb	r2, [r3, #0]
    6266:	2204      	movs	r2, #4
    6268:	680b      	ldr	r3, [r1, #0]
    626a:	307f      	adds	r0, #127	; 0x7f
    626c:	721a      	strb	r2, [r3, #8]
    626e:	2200      	movs	r2, #0
    6270:	4b06      	ldr	r3, [pc, #24]	; (628c <ChkandChangePS_8730A_ROM+0x94>)
    6272:	0011      	movs	r1, r2
    6274:	681b      	ldr	r3, [r3, #0]
    6276:	e7e3      	b.n	6240 <ChkandChangePS_8730A_ROM+0x48>
    6278:	23000a38 	.word	0x23000a38
    627c:	4300041c 	.word	0x4300041c
    6280:	23000a28 	.word	0x23000a28
    6284:	43000604 	.word	0x43000604
    6288:	230007ec 	.word	0x230007ec
    628c:	2300094c 	.word	0x2300094c

00006290 <IssueQNull_8730A_ROM>:
    6290:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    6292:	4b1c      	ldr	r3, [pc, #112]	; (6304 <IssueQNull_8730A_ROM+0x74>)
    6294:	0004      	movs	r4, r0
    6296:	781b      	ldrb	r3, [r3, #0]
    6298:	b323      	cbz	r3, 62e4 <IssueQNull_8730A_ROM+0x54>
    629a:	4b1b      	ldr	r3, [pc, #108]	; (6308 <IssueQNull_8730A_ROM+0x78>)
    629c:	4e1b      	ldr	r6, [pc, #108]	; (630c <IssueQNull_8730A_ROM+0x7c>)
    629e:	21ff      	movs	r1, #255	; 0xff
    62a0:	781d      	ldrb	r5, [r3, #0]
    62a2:	2229      	movs	r2, #41	; 0x29
    62a4:	2000      	movs	r0, #0
    62a6:	6833      	ldr	r3, [r6, #0]
    62a8:	4798      	blx	r3
    62aa:	4b19      	ldr	r3, [pc, #100]	; (6310 <IssueQNull_8730A_ROM+0x80>)
    62ac:	b2ed      	uxtb	r5, r5
    62ae:	681b      	ldr	r3, [r3, #0]
    62b0:	4798      	blx	r3
    62b2:	0001      	movs	r1, r0
    62b4:	2801      	cmp	r0, #1
    62b6:	d10c      	bne.n	62d2 <IssueQNull_8730A_ROM+0x42>
    62b8:	4b16      	ldr	r3, [pc, #88]	; (6314 <IssueQNull_8730A_ROM+0x84>)
    62ba:	2208      	movs	r2, #8
    62bc:	681b      	ldr	r3, [r3, #0]
    62be:	78d8      	ldrb	r0, [r3, #3]
    62c0:	4b15      	ldr	r3, [pc, #84]	; (6318 <IssueQNull_8730A_ROM+0x88>)
    62c2:	681f      	ldr	r7, [r3, #0]
    62c4:	230b      	movs	r3, #11
    62c6:	47b8      	blx	r7
    62c8:	4b14      	ldr	r3, [pc, #80]	; (631c <IssueQNull_8730A_ROM+0x8c>)
    62ca:	0001      	movs	r1, r0
    62cc:	681b      	ldr	r3, [r3, #0]
    62ce:	0020      	movs	r0, r4
    62d0:	4798      	blx	r3
    62d2:	222a      	movs	r2, #42	; 0x2a
    62d4:	0029      	movs	r1, r5
    62d6:	2000      	movs	r0, #0
    62d8:	6833      	ldr	r3, [r6, #0]
    62da:	4798      	blx	r3
    62dc:	2220      	movs	r2, #32
    62de:	4b10      	ldr	r3, [pc, #64]	; (6320 <IssueQNull_8730A_ROM+0x90>)
    62e0:	701a      	strb	r2, [r3, #0]
    62e2:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    62e4:	4b0b      	ldr	r3, [pc, #44]	; (6314 <IssueQNull_8730A_ROM+0x84>)
    62e6:	2208      	movs	r2, #8
    62e8:	681b      	ldr	r3, [r3, #0]
    62ea:	2101      	movs	r1, #1
    62ec:	78d8      	ldrb	r0, [r3, #3]
    62ee:	4b0a      	ldr	r3, [pc, #40]	; (6318 <IssueQNull_8730A_ROM+0x88>)
    62f0:	681d      	ldr	r5, [r3, #0]
    62f2:	230b      	movs	r3, #11
    62f4:	47a8      	blx	r5
    62f6:	4b09      	ldr	r3, [pc, #36]	; (631c <IssueQNull_8730A_ROM+0x8c>)
    62f8:	0001      	movs	r1, r0
    62fa:	681b      	ldr	r3, [r3, #0]
    62fc:	0020      	movs	r0, r4
    62fe:	4798      	blx	r3
    6300:	e7ec      	b.n	62dc <IssueQNull_8730A_ROM+0x4c>
    6302:	46c0      	nop			; (mov r8, r8)
    6304:	4300041d 	.word	0x4300041d
    6308:	43000522 	.word	0x43000522
    630c:	2300094c 	.word	0x2300094c
    6310:	23000948 	.word	0x23000948
    6314:	23000a38 	.word	0x23000a38
    6318:	230007dc 	.word	0x230007dc
    631c:	230007c4 	.word	0x230007c4
    6320:	4300041f 	.word	0x4300041f

00006324 <CTS2SelfSetting_8730A_ROM>:
    6324:	2280      	movs	r2, #128	; 0x80
    6326:	4b04      	ldr	r3, [pc, #16]	; (6338 <CTS2SelfSetting_8730A_ROM+0x14>)
    6328:	18c3      	adds	r3, r0, r3
    632a:	8019      	strh	r1, [r3, #0]
    632c:	4b03      	ldr	r3, [pc, #12]	; (633c <CTS2SelfSetting_8730A_ROM+0x18>)
    632e:	18c0      	adds	r0, r0, r3
    6330:	7803      	ldrb	r3, [r0, #0]
    6332:	4313      	orrs	r3, r2
    6334:	7003      	strb	r3, [r0, #0]
    6336:	4770      	bx	lr
    6338:	4302002a 	.word	0x4302002a
    633c:	4302000d 	.word	0x4302000d

00006340 <IssueCTS2Self_8730A_ROM>:
    6340:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    6342:	4b1e      	ldr	r3, [pc, #120]	; (63bc <IssueCTS2Self_8730A_ROM+0x7c>)
    6344:	0005      	movs	r5, r0
    6346:	781b      	ldrb	r3, [r3, #0]
    6348:	000c      	movs	r4, r1
    634a:	4e1d      	ldr	r6, [pc, #116]	; (63c0 <IssueCTS2Self_8730A_ROM+0x80>)
    634c:	b2da      	uxtb	r2, r3
    634e:	b353      	cbz	r3, 63a6 <IssueCTS2Self_8730A_ROM+0x66>
    6350:	4b1c      	ldr	r3, [pc, #112]	; (63c4 <IssueCTS2Self_8730A_ROM+0x84>)
    6352:	21ff      	movs	r1, #255	; 0xff
    6354:	781f      	ldrb	r7, [r3, #0]
    6356:	2236      	movs	r2, #54	; 0x36
    6358:	2000      	movs	r0, #0
    635a:	6833      	ldr	r3, [r6, #0]
    635c:	4798      	blx	r3
    635e:	4b1a      	ldr	r3, [pc, #104]	; (63c8 <IssueCTS2Self_8730A_ROM+0x88>)
    6360:	b2ff      	uxtb	r7, r7
    6362:	681b      	ldr	r3, [r3, #0]
    6364:	4798      	blx	r3
    6366:	0001      	movs	r1, r0
    6368:	2801      	cmp	r0, #1
    636a:	d10b      	bne.n	6384 <IssueCTS2Self_8730A_ROM+0x44>
    636c:	4b17      	ldr	r3, [pc, #92]	; (63cc <IssueCTS2Self_8730A_ROM+0x8c>)
    636e:	0028      	movs	r0, r5
    6370:	681b      	ldr	r3, [r3, #0]
    6372:	2200      	movs	r2, #0
    6374:	469c      	mov	ip, r3
    6376:	230c      	movs	r3, #12
    6378:	4665      	mov	r5, ip
    637a:	47a8      	blx	r5
    637c:	4b14      	ldr	r3, [pc, #80]	; (63d0 <IssueCTS2Self_8730A_ROM+0x90>)
    637e:	0021      	movs	r1, r4
    6380:	681b      	ldr	r3, [r3, #0]
    6382:	4798      	blx	r3
    6384:	2237      	movs	r2, #55	; 0x37
    6386:	0039      	movs	r1, r7
    6388:	2000      	movs	r0, #0
    638a:	6833      	ldr	r3, [r6, #0]
    638c:	4798      	blx	r3
    638e:	4b0d      	ldr	r3, [pc, #52]	; (63c4 <IssueCTS2Self_8730A_ROM+0x84>)
    6390:	2238      	movs	r2, #56	; 0x38
    6392:	7819      	ldrb	r1, [r3, #0]
    6394:	236f      	movs	r3, #111	; 0x6f
    6396:	2090      	movs	r0, #144	; 0x90
    6398:	4019      	ands	r1, r3
    639a:	6833      	ldr	r3, [r6, #0]
    639c:	4798      	blx	r3
    639e:	2220      	movs	r2, #32
    63a0:	4b0c      	ldr	r3, [pc, #48]	; (63d4 <IssueCTS2Self_8730A_ROM+0x94>)
    63a2:	701a      	strb	r2, [r3, #0]
    63a4:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    63a6:	4b09      	ldr	r3, [pc, #36]	; (63cc <IssueCTS2Self_8730A_ROM+0x8c>)
    63a8:	2101      	movs	r1, #1
    63aa:	681f      	ldr	r7, [r3, #0]
    63ac:	230c      	movs	r3, #12
    63ae:	47b8      	blx	r7
    63b0:	4b07      	ldr	r3, [pc, #28]	; (63d0 <IssueCTS2Self_8730A_ROM+0x90>)
    63b2:	0021      	movs	r1, r4
    63b4:	681b      	ldr	r3, [r3, #0]
    63b6:	4798      	blx	r3
    63b8:	e7e9      	b.n	638e <IssueCTS2Self_8730A_ROM+0x4e>
    63ba:	46c0      	nop			; (mov r8, r8)
    63bc:	4300041d 	.word	0x4300041d
    63c0:	2300094c 	.word	0x2300094c
    63c4:	43000522 	.word	0x43000522
    63c8:	23000948 	.word	0x23000948
    63cc:	230007dc 	.word	0x230007dc
    63d0:	230007d4 	.word	0x230007d4
    63d4:	4300041f 	.word	0x4300041f

000063d8 <IssueRsvdPagePacketSetting_8730A_ROM>:
    63d8:	b5f0      	push	{r4, r5, r6, r7, lr}
    63da:	0004      	movs	r4, r0
    63dc:	4d28      	ldr	r5, [pc, #160]	; (6480 <IssueRsvdPagePacketSetting_8730A_ROM+0xa8>)
    63de:	01c0      	lsls	r0, r0, #7
    63e0:	702c      	strb	r4, [r5, #0]
    63e2:	b129      	cbz	r1, 63f0 <IssueRsvdPagePacketSetting_8730A_ROM+0x18>
    63e4:	2580      	movs	r5, #128	; 0x80
    63e6:	4927      	ldr	r1, [pc, #156]	; (6484 <IssueRsvdPagePacketSetting_8730A_ROM+0xac>)
    63e8:	1844      	adds	r4, r0, r1
    63ea:	7821      	ldrb	r1, [r4, #0]
    63ec:	4329      	orrs	r1, r5
    63ee:	7021      	strb	r1, [r4, #0]
    63f0:	4925      	ldr	r1, [pc, #148]	; (6488 <IssueRsvdPagePacketSetting_8730A_ROM+0xb0>)
    63f2:	4d26      	ldr	r5, [pc, #152]	; (648c <IssueRsvdPagePacketSetting_8730A_ROM+0xb4>)
    63f4:	1844      	adds	r4, r0, r1
    63f6:	8821      	ldrh	r1, [r4, #0]
    63f8:	2703      	movs	r7, #3
    63fa:	4029      	ands	r1, r5
    63fc:	8021      	strh	r1, [r4, #0]
    63fe:	001d      	movs	r5, r3
    6400:	217f      	movs	r1, #127	; 0x7f
    6402:	4c23      	ldr	r4, [pc, #140]	; (6490 <IssueRsvdPagePacketSetting_8730A_ROM+0xb8>)
    6404:	400d      	ands	r5, r1
    6406:	1904      	adds	r4, r0, r4
    6408:	7025      	strb	r5, [r4, #0]
    640a:	4c22      	ldr	r4, [pc, #136]	; (6494 <IssueRsvdPagePacketSetting_8730A_ROM+0xbc>)
    640c:	09db      	lsrs	r3, r3, #7
    640e:	1904      	adds	r4, r0, r4
    6410:	7023      	strb	r3, [r4, #0]
    6412:	2400      	movs	r4, #0
    6414:	2508      	movs	r5, #8
    6416:	4b20      	ldr	r3, [pc, #128]	; (6498 <IssueRsvdPagePacketSetting_8730A_ROM+0xc0>)
    6418:	0092      	lsls	r2, r2, #2
    641a:	18c3      	adds	r3, r0, r3
    641c:	701c      	strb	r4, [r3, #0]
    641e:	4b1f      	ldr	r3, [pc, #124]	; (649c <IssueRsvdPagePacketSetting_8730A_ROM+0xc4>)
    6420:	18c6      	adds	r6, r0, r3
    6422:	7833      	ldrb	r3, [r6, #0]
    6424:	432b      	orrs	r3, r5
    6426:	7033      	strb	r3, [r6, #0]
    6428:	4b1d      	ldr	r3, [pc, #116]	; (64a0 <IssueRsvdPagePacketSetting_8730A_ROM+0xc8>)
    642a:	18c3      	adds	r3, r0, r3
    642c:	781e      	ldrb	r6, [r3, #0]
    642e:	403e      	ands	r6, r7
    6430:	4332      	orrs	r2, r6
    6432:	b2d2      	uxtb	r2, r2
    6434:	701a      	strb	r2, [r3, #0]
    6436:	4b1b      	ldr	r3, [pc, #108]	; (64a4 <IssueRsvdPagePacketSetting_8730A_ROM+0xcc>)
    6438:	4e1b      	ldr	r6, [pc, #108]	; (64a8 <IssueRsvdPagePacketSetting_8730A_ROM+0xd0>)
    643a:	18c2      	adds	r2, r0, r3
    643c:	7813      	ldrb	r3, [r2, #0]
    643e:	43ab      	bics	r3, r5
    6440:	7013      	strb	r3, [r2, #0]
    6442:	4a1a      	ldr	r2, [pc, #104]	; (64ac <IssueRsvdPagePacketSetting_8730A_ROM+0xd4>)
    6444:	4b1a      	ldr	r3, [pc, #104]	; (64b0 <IssueRsvdPagePacketSetting_8730A_ROM+0xd8>)
    6446:	6812      	ldr	r2, [r2, #0]
    6448:	4d1a      	ldr	r5, [pc, #104]	; (64b4 <IssueRsvdPagePacketSetting_8730A_ROM+0xdc>)
    644a:	7812      	ldrb	r2, [r2, #0]
    644c:	18c3      	adds	r3, r0, r3
    644e:	07d2      	lsls	r2, r2, #31
    6450:	d50c      	bpl.n	646c <IssueRsvdPagePacketSetting_8730A_ROM+0x94>
    6452:	781a      	ldrb	r2, [r3, #0]
    6454:	3101      	adds	r1, #1
    6456:	430a      	orrs	r2, r1
    6458:	701a      	strb	r2, [r3, #0]
    645a:	7034      	strb	r4, [r6, #0]
    645c:	802c      	strh	r4, [r5, #0]
    645e:	2180      	movs	r1, #128	; 0x80
    6460:	4b15      	ldr	r3, [pc, #84]	; (64b8 <IssueRsvdPagePacketSetting_8730A_ROM+0xe0>)
    6462:	18c2      	adds	r2, r0, r3
    6464:	7813      	ldrb	r3, [r2, #0]
    6466:	430b      	orrs	r3, r1
    6468:	7013      	strb	r3, [r2, #0]
    646a:	bdf0      	pop	{r4, r5, r6, r7, pc}
    646c:	781a      	ldrb	r2, [r3, #0]
    646e:	400a      	ands	r2, r1
    6470:	701a      	strb	r2, [r3, #0]
    6472:	231c      	movs	r3, #28
    6474:	7033      	strb	r3, [r6, #0]
    6476:	f244 4313 	movw	r3, #17427	; 0x4413
    647a:	802b      	strh	r3, [r5, #0]
    647c:	e7ef      	b.n	645e <IssueRsvdPagePacketSetting_8730A_ROM+0x86>
    647e:	46c0      	nop			; (mov r8, r8)
    6480:	43000425 	.word	0x43000425
    6484:	43020021 	.word	0x43020021
    6488:	43020020 	.word	0x43020020
    648c:	ffffc0c0 	.word	0xffffc0c0
    6490:	4302001e 	.word	0x4302001e
    6494:	43020025 	.word	0x43020025
    6498:	43020010 	.word	0x43020010
    649c:	4302001a 	.word	0x4302001a
    64a0:	43020012 	.word	0x43020012
    64a4:	43020029 	.word	0x43020029
    64a8:	43000512 	.word	0x43000512
    64ac:	23000a14 	.word	0x23000a14
    64b0:	43020005 	.word	0x43020005
    64b4:	43000510 	.word	0x43000510
    64b8:	4302000d 	.word	0x4302000d

000064bc <ReadRxBuff_8730A_ROM>:
    64bc:	4b06      	ldr	r3, [pc, #24]	; (64d8 <ReadRxBuff_8730A_ROM+0x1c>)
    64be:	1841      	adds	r1, r0, r1
    64c0:	881b      	ldrh	r3, [r3, #0]
    64c2:	b289      	uxth	r1, r1
    64c4:	428b      	cmp	r3, r1
    64c6:	d202      	bcs.n	64ce <ReadRxBuff_8730A_ROM+0x12>
    64c8:	43db      	mvns	r3, r3
    64ca:	18c9      	adds	r1, r1, r3
    64cc:	b289      	uxth	r1, r1
    64ce:	4b03      	ldr	r3, [pc, #12]	; (64dc <ReadRxBuff_8730A_ROM+0x20>)
    64d0:	18c9      	adds	r1, r1, r3
    64d2:	7808      	ldrb	r0, [r1, #0]
    64d4:	b2c0      	uxtb	r0, r0
    64d6:	4770      	bx	lr
    64d8:	23000a2e 	.word	0x23000a2e
    64dc:	43030000 	.word	0x43030000

000064e0 <WaitCPUMGQEmpty_8730A_ROM>:
    64e0:	b570      	push	{r4, r5, r6, lr}
    64e2:	f240 34ea 	movw	r4, #1002	; 0x3ea
    64e6:	f240 1500 	movw	r5, #256	; 0x100
    64ea:	4b07      	ldr	r3, [pc, #28]	; (6508 <WaitCPUMGQEmpty_8730A_ROM+0x28>)
    64ec:	681b      	ldr	r3, [r3, #0]
    64ee:	422b      	tst	r3, r5
    64f0:	d006      	beq.n	6500 <WaitCPUMGQEmpty_8730A_ROM+0x20>
    64f2:	3c01      	subs	r4, #1
    64f4:	b2a4      	uxth	r4, r4
    64f6:	b12c      	cbz	r4, 6504 <WaitCPUMGQEmpty_8730A_ROM+0x24>
    64f8:	2032      	movs	r0, #50	; 0x32
    64fa:	4b04      	ldr	r3, [pc, #16]	; (650c <WaitCPUMGQEmpty_8730A_ROM+0x2c>)
    64fc:	4798      	blx	r3
    64fe:	e7f4      	b.n	64ea <WaitCPUMGQEmpty_8730A_ROM+0xa>
    6500:	2001      	movs	r0, #1
    6502:	bd70      	pop	{r4, r5, r6, pc}
    6504:	0020      	movs	r0, r4
    6506:	e7fc      	b.n	6502 <WaitCPUMGQEmpty_8730A_ROM+0x22>
    6508:	4300041c 	.word	0x4300041c
    650c:	0000030b 	.word	0x0000030b

00006510 <PollingReg_8730A_ROM>:
    6510:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    6512:	001c      	movs	r4, r3
    6514:	000e      	movs	r6, r1
    6516:	0017      	movs	r7, r2
    6518:	2300      	movs	r3, #0
    651a:	9d08      	ldr	r5, [sp, #32]
    651c:	9001      	str	r0, [sp, #4]
    651e:	2286      	movs	r2, #134	; 0x86
    6520:	802b      	strh	r3, [r5, #0]
    6522:	9b01      	ldr	r3, [sp, #4]
    6524:	05d2      	lsls	r2, r2, #23
    6526:	189b      	adds	r3, r3, r2
    6528:	781b      	ldrb	r3, [r3, #0]
    652a:	8828      	ldrh	r0, [r5, #0]
    652c:	4033      	ands	r3, r6
    652e:	42bb      	cmp	r3, r7
    6530:	d001      	beq.n	6536 <PollingReg_8730A_ROM+0x26>
    6532:	4284      	cmp	r4, r0
    6534:	d803      	bhi.n	653e <PollingReg_8730A_ROM+0x2e>
    6536:	42a0      	cmp	r0, r4
    6538:	4180      	sbcs	r0, r0
    653a:	4240      	negs	r0, r0
    653c:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    653e:	4b03      	ldr	r3, [pc, #12]	; (654c <PollingReg_8730A_ROM+0x3c>)
    6540:	200a      	movs	r0, #10
    6542:	4798      	blx	r3
    6544:	882b      	ldrh	r3, [r5, #0]
    6546:	3301      	adds	r3, #1
    6548:	e7e9      	b.n	651e <PollingReg_8730A_ROM+0xe>
    654a:	46c0      	nop			; (mov r8, r8)
    654c:	0000030b 	.word	0x0000030b

00006550 <SetPwrStateReg_8730A_ROM>:
    6550:	b510      	push	{r4, lr}
    6552:	2801      	cmp	r0, #1
    6554:	d013      	beq.n	657e <SetPwrStateReg_8730A_ROM+0x2e>
    6556:	2802      	cmp	r0, #2
    6558:	d015      	beq.n	6586 <SetPwrStateReg_8730A_ROM+0x36>
    655a:	b938      	cbnz	r0, 656c <SetPwrStateReg_8730A_ROM+0x1c>
    655c:	2401      	movs	r4, #1
    655e:	4b15      	ldr	r3, [pc, #84]	; (65b4 <SetPwrStateReg_8730A_ROM+0x64>)
    6560:	4021      	ands	r1, r4
    6562:	681a      	ldr	r2, [r3, #0]
    6564:	7813      	ldrb	r3, [r2, #0]
    6566:	43a3      	bics	r3, r4
    6568:	4319      	orrs	r1, r3
    656a:	7011      	strb	r1, [r2, #0]
    656c:	4b12      	ldr	r3, [pc, #72]	; (65b8 <SetPwrStateReg_8730A_ROM+0x68>)
    656e:	781b      	ldrb	r3, [r3, #0]
    6570:	06db      	lsls	r3, r3, #27
    6572:	d503      	bpl.n	657c <SetPwrStateReg_8730A_ROM+0x2c>
    6574:	2801      	cmp	r0, #1
    6576:	d90a      	bls.n	658e <SetPwrStateReg_8730A_ROM+0x3e>
    6578:	2802      	cmp	r0, #2
    657a:	d014      	beq.n	65a6 <SetPwrStateReg_8730A_ROM+0x56>
    657c:	bd10      	pop	{r4, pc}
    657e:	4b0d      	ldr	r3, [pc, #52]	; (65b4 <SetPwrStateReg_8730A_ROM+0x64>)
    6580:	681b      	ldr	r3, [r3, #0]
    6582:	7219      	strb	r1, [r3, #8]
    6584:	e7f2      	b.n	656c <SetPwrStateReg_8730A_ROM+0x1c>
    6586:	4b0b      	ldr	r3, [pc, #44]	; (65b4 <SetPwrStateReg_8730A_ROM+0x64>)
    6588:	681b      	ldr	r3, [r3, #0]
    658a:	71d9      	strb	r1, [r3, #7]
    658c:	e7ee      	b.n	656c <SetPwrStateReg_8730A_ROM+0x1c>
    658e:	217f      	movs	r1, #127	; 0x7f
    6590:	4b08      	ldr	r3, [pc, #32]	; (65b4 <SetPwrStateReg_8730A_ROM+0x64>)
    6592:	681a      	ldr	r2, [r3, #0]
    6594:	7a13      	ldrb	r3, [r2, #8]
    6596:	7812      	ldrb	r2, [r2, #0]
    6598:	400b      	ands	r3, r1
    659a:	07d2      	lsls	r2, r2, #31
    659c:	0e12      	lsrs	r2, r2, #24
    659e:	4313      	orrs	r3, r2
    65a0:	4a06      	ldr	r2, [pc, #24]	; (65bc <SetPwrStateReg_8730A_ROM+0x6c>)
    65a2:	7013      	strb	r3, [r2, #0]
    65a4:	e7ea      	b.n	657c <SetPwrStateReg_8730A_ROM+0x2c>
    65a6:	4b03      	ldr	r3, [pc, #12]	; (65b4 <SetPwrStateReg_8730A_ROM+0x64>)
    65a8:	681b      	ldr	r3, [r3, #0]
    65aa:	79da      	ldrb	r2, [r3, #7]
    65ac:	4b04      	ldr	r3, [pc, #16]	; (65c0 <SetPwrStateReg_8730A_ROM+0x70>)
    65ae:	701a      	strb	r2, [r3, #0]
    65b0:	e7e4      	b.n	657c <SetPwrStateReg_8730A_ROM+0x2c>
    65b2:	46c0      	nop			; (mov r8, r8)
    65b4:	23000a28 	.word	0x23000a28
    65b8:	4300008b 	.word	0x4300008b
    65bc:	43000088 	.word	0x43000088
    65c0:	43000089 	.word	0x43000089

000065c4 <ResetPSParm_8730A_ROM>:
    65c4:	2200      	movs	r2, #0
    65c6:	4b0d      	ldr	r3, [pc, #52]	; (65fc <ResetPSParm_8730A_ROM+0x38>)
    65c8:	b510      	push	{r4, lr}
    65ca:	681b      	ldr	r3, [r3, #0]
    65cc:	490c      	ldr	r1, [pc, #48]	; (6600 <ResetPSParm_8730A_ROM+0x3c>)
    65ce:	735a      	strb	r2, [r3, #13]
    65d0:	739a      	strb	r2, [r3, #14]
    65d2:	725a      	strb	r2, [r3, #9]
    65d4:	881a      	ldrh	r2, [r3, #0]
    65d6:	4c0b      	ldr	r4, [pc, #44]	; (6604 <ResetPSParm_8730A_ROM+0x40>)
    65d8:	400a      	ands	r2, r1
    65da:	801a      	strh	r2, [r3, #0]
    65dc:	2101      	movs	r1, #1
    65de:	6823      	ldr	r3, [r4, #0]
    65e0:	2002      	movs	r0, #2
    65e2:	4798      	blx	r3
    65e4:	2101      	movs	r1, #1
    65e6:	6823      	ldr	r3, [r4, #0]
    65e8:	0008      	movs	r0, r1
    65ea:	4798      	blx	r3
    65ec:	4b06      	ldr	r3, [pc, #24]	; (6608 <ResetPSParm_8730A_ROM+0x44>)
    65ee:	f248 0100 	movw	r1, #32768	; 0x8000
    65f2:	2002      	movs	r0, #2
    65f4:	681b      	ldr	r3, [r3, #0]
    65f6:	4798      	blx	r3
    65f8:	bd10      	pop	{r4, pc}
    65fa:	46c0      	nop			; (mov r8, r8)
    65fc:	23000a28 	.word	0x23000a28
    6600:	ffffb7fb 	.word	0xffffb7fb
    6604:	23000768 	.word	0x23000768
    6608:	23000774 	.word	0x23000774

0000660c <ChkTxQueueIsEmpty_8730A_ROM>:
    660c:	4b06      	ldr	r3, [pc, #24]	; (6628 <ChkTxQueueIsEmpty_8730A_ROM+0x1c>)
    660e:	2000      	movs	r0, #0
    6610:	781b      	ldrb	r3, [r3, #0]
    6612:	2bff      	cmp	r3, #255	; 0xff
    6614:	d107      	bne.n	6626 <ChkTxQueueIsEmpty_8730A_ROM+0x1a>
    6616:	4b05      	ldr	r3, [pc, #20]	; (662c <ChkTxQueueIsEmpty_8730A_ROM+0x20>)
    6618:	7818      	ldrb	r0, [r3, #0]
    661a:	2317      	movs	r3, #23
    661c:	4018      	ands	r0, r3
    661e:	3817      	subs	r0, #23
    6620:	4243      	negs	r3, r0
    6622:	4158      	adcs	r0, r3
    6624:	b2c0      	uxtb	r0, r0
    6626:	4770      	bx	lr
    6628:	4300041a 	.word	0x4300041a
    662c:	4300041b 	.word	0x4300041b

00006630 <Legacy_PS_Setting_8730A_ROM>:
    6630:	21f0      	movs	r1, #240	; 0xf0
    6632:	b510      	push	{r4, lr}
    6634:	4c15      	ldr	r4, [pc, #84]	; (668c <Legacy_PS_Setting_8730A_ROM+0x5c>)
    6636:	2000      	movs	r0, #0
    6638:	6823      	ldr	r3, [r4, #0]
    663a:	0349      	lsls	r1, r1, #13
    663c:	4798      	blx	r3
    663e:	2180      	movs	r1, #128	; 0x80
    6640:	6823      	ldr	r3, [r4, #0]
    6642:	2003      	movs	r0, #3
    6644:	0289      	lsls	r1, r1, #10
    6646:	4798      	blx	r3
    6648:	23e0      	movs	r3, #224	; 0xe0
    664a:	4a11      	ldr	r2, [pc, #68]	; (6690 <Legacy_PS_Setting_8730A_ROM+0x60>)
    664c:	4c11      	ldr	r4, [pc, #68]	; (6694 <Legacy_PS_Setting_8730A_ROM+0x64>)
    664e:	6811      	ldr	r1, [r2, #0]
    6650:	02db      	lsls	r3, r3, #11
    6652:	430b      	orrs	r3, r1
    6654:	6013      	str	r3, [r2, #0]
    6656:	6823      	ldr	r3, [r4, #0]
    6658:	4a0f      	ldr	r2, [pc, #60]	; (6698 <Legacy_PS_Setting_8730A_ROM+0x68>)
    665a:	7bdb      	ldrb	r3, [r3, #15]
    665c:	2000      	movs	r0, #0
    665e:	095b      	lsrs	r3, r3, #5
    6660:	7013      	strb	r3, [r2, #0]
    6662:	4b0e      	ldr	r3, [pc, #56]	; (669c <Legacy_PS_Setting_8730A_ROM+0x6c>)
    6664:	681b      	ldr	r3, [r3, #0]
    6666:	4798      	blx	r3
    6668:	6823      	ldr	r3, [r4, #0]
    666a:	0001      	movs	r1, r0
    666c:	785a      	ldrb	r2, [r3, #1]
    666e:	2004      	movs	r0, #4
    6670:	2901      	cmp	r1, #1
    6672:	d108      	bne.n	6686 <Legacy_PS_Setting_8730A_ROM+0x56>
    6674:	4382      	bics	r2, r0
    6676:	705a      	strb	r2, [r3, #1]
    6678:	7a1b      	ldrb	r3, [r3, #8]
    667a:	4203      	tst	r3, r0
    667c:	d102      	bne.n	6684 <Legacy_PS_Setting_8730A_ROM+0x54>
    667e:	4b08      	ldr	r3, [pc, #32]	; (66a0 <Legacy_PS_Setting_8730A_ROM+0x70>)
    6680:	681b      	ldr	r3, [r3, #0]
    6682:	4798      	blx	r3
    6684:	bd10      	pop	{r4, pc}
    6686:	4310      	orrs	r0, r2
    6688:	7058      	strb	r0, [r3, #1]
    668a:	e7fb      	b.n	6684 <Legacy_PS_Setting_8730A_ROM+0x54>
    668c:	23000768 	.word	0x23000768
    6690:	43000608 	.word	0x43000608
    6694:	23000a28 	.word	0x23000a28
    6698:	43000558 	.word	0x43000558
    669c:	230007bc 	.word	0x230007bc
    66a0:	23000958 	.word	0x23000958

000066a4 <PSModeSetting_8730A_ROM>:
    66a4:	b570      	push	{r4, r5, r6, lr}
    66a6:	0004      	movs	r4, r0
    66a8:	2800      	cmp	r0, #0
    66aa:	d138      	bne.n	671e <PSModeSetting_8730A_ROM+0x7a>
    66ac:	21f0      	movs	r1, #240	; 0xf0
    66ae:	4d1e      	ldr	r5, [pc, #120]	; (6728 <PSModeSetting_8730A_ROM+0x84>)
    66b0:	0349      	lsls	r1, r1, #13
    66b2:	682b      	ldr	r3, [r5, #0]
    66b4:	4798      	blx	r3
    66b6:	2180      	movs	r1, #128	; 0x80
    66b8:	2003      	movs	r0, #3
    66ba:	682b      	ldr	r3, [r5, #0]
    66bc:	0289      	lsls	r1, r1, #10
    66be:	4798      	blx	r3
    66c0:	4b1a      	ldr	r3, [pc, #104]	; (672c <PSModeSetting_8730A_ROM+0x88>)
    66c2:	210c      	movs	r1, #12
    66c4:	2001      	movs	r0, #1
    66c6:	681b      	ldr	r3, [r3, #0]
    66c8:	4798      	blx	r3
    66ca:	4b19      	ldr	r3, [pc, #100]	; (6730 <PSModeSetting_8730A_ROM+0x8c>)
    66cc:	2023      	movs	r0, #35	; 0x23
    66ce:	701c      	strb	r4, [r3, #0]
    66d0:	4b18      	ldr	r3, [pc, #96]	; (6734 <PSModeSetting_8730A_ROM+0x90>)
    66d2:	4798      	blx	r3
    66d4:	2202      	movs	r2, #2
    66d6:	4b18      	ldr	r3, [pc, #96]	; (6738 <PSModeSetting_8730A_ROM+0x94>)
    66d8:	0020      	movs	r0, r4
    66da:	701a      	strb	r2, [r3, #0]
    66dc:	4b17      	ldr	r3, [pc, #92]	; (673c <PSModeSetting_8730A_ROM+0x98>)
    66de:	4c18      	ldr	r4, [pc, #96]	; (6740 <PSModeSetting_8730A_ROM+0x9c>)
    66e0:	681b      	ldr	r3, [r3, #0]
    66e2:	4798      	blx	r3
    66e4:	b9a8      	cbnz	r0, 6712 <PSModeSetting_8730A_ROM+0x6e>
    66e6:	4b17      	ldr	r3, [pc, #92]	; (6744 <PSModeSetting_8730A_ROM+0xa0>)
    66e8:	681b      	ldr	r3, [r3, #0]
    66ea:	4798      	blx	r3
    66ec:	4b16      	ldr	r3, [pc, #88]	; (6748 <PSModeSetting_8730A_ROM+0xa4>)
    66ee:	681b      	ldr	r3, [r3, #0]
    66f0:	4798      	blx	r3
    66f2:	2168      	movs	r1, #104	; 0x68
    66f4:	6822      	ldr	r2, [r4, #0]
    66f6:	7813      	ldrb	r3, [r2, #0]
    66f8:	438b      	bics	r3, r1
    66fa:	7013      	strb	r3, [r2, #0]
    66fc:	2108      	movs	r1, #8
    66fe:	6822      	ldr	r2, [r4, #0]
    6700:	7813      	ldrb	r3, [r2, #0]
    6702:	438b      	bics	r3, r1
    6704:	7013      	strb	r3, [r2, #0]
    6706:	4a11      	ldr	r2, [pc, #68]	; (674c <PSModeSetting_8730A_ROM+0xa8>)
    6708:	4911      	ldr	r1, [pc, #68]	; (6750 <PSModeSetting_8730A_ROM+0xac>)
    670a:	6813      	ldr	r3, [r2, #0]
    670c:	400b      	ands	r3, r1
    670e:	6013      	str	r3, [r2, #0]
    6710:	bd70      	pop	{r4, r5, r6, pc}
    6712:	4b10      	ldr	r3, [pc, #64]	; (6754 <PSModeSetting_8730A_ROM+0xb0>)
    6714:	2101      	movs	r1, #1
    6716:	200c      	movs	r0, #12
    6718:	681b      	ldr	r3, [r3, #0]
    671a:	4798      	blx	r3
    671c:	e7ee      	b.n	66fc <PSModeSetting_8730A_ROM+0x58>
    671e:	4b0e      	ldr	r3, [pc, #56]	; (6758 <PSModeSetting_8730A_ROM+0xb4>)
    6720:	681b      	ldr	r3, [r3, #0]
    6722:	4798      	blx	r3
    6724:	e7f4      	b.n	6710 <PSModeSetting_8730A_ROM+0x6c>
    6726:	46c0      	nop			; (mov r8, r8)
    6728:	2300076c 	.word	0x2300076c
    672c:	23000774 	.word	0x23000774
    6730:	43000157 	.word	0x43000157
    6734:	0000030b 	.word	0x0000030b
    6738:	4300013c 	.word	0x4300013c
    673c:	230007bc 	.word	0x230007bc
    6740:	23000a28 	.word	0x23000a28
    6744:	23000940 	.word	0x23000940
    6748:	23000808 	.word	0x23000808
    674c:	43000608 	.word	0x43000608
    6750:	fffcffff 	.word	0xfffcffff
    6754:	23000958 	.word	0x23000958
    6758:	230007f8 	.word	0x230007f8

0000675c <ConfigListenBeaconPeriod_8730A_ROM>:
    675c:	2801      	cmp	r0, #1
    675e:	d002      	beq.n	6766 <ConfigListenBeaconPeriod_8730A_ROM+0xa>
    6760:	2802      	cmp	r0, #2
    6762:	d00a      	beq.n	677a <ConfigListenBeaconPeriod_8730A_ROM+0x1e>
    6764:	4770      	bx	lr
    6766:	f240 2202 	movw	r2, #514	; 0x202
    676a:	4b0a      	ldr	r3, [pc, #40]	; (6794 <ConfigListenBeaconPeriod_8730A_ROM+0x38>)
    676c:	681b      	ldr	r3, [r3, #0]
    676e:	815a      	strh	r2, [r3, #10]
    6770:	2208      	movs	r2, #8
    6772:	7859      	ldrb	r1, [r3, #1]
    6774:	430a      	orrs	r2, r1
    6776:	705a      	strb	r2, [r3, #1]
    6778:	e7f4      	b.n	6764 <ConfigListenBeaconPeriod_8730A_ROM+0x8>
    677a:	4b06      	ldr	r3, [pc, #24]	; (6794 <ConfigListenBeaconPeriod_8730A_ROM+0x38>)
    677c:	681b      	ldr	r3, [r3, #0]
    677e:	b931      	cbnz	r1, 678e <ConfigListenBeaconPeriod_8730A_ROM+0x32>
    6780:	4a05      	ldr	r2, [pc, #20]	; (6798 <ConfigListenBeaconPeriod_8730A_ROM+0x3c>)
    6782:	6812      	ldr	r2, [r2, #0]
    6784:	78d2      	ldrb	r2, [r2, #3]
    6786:	729a      	strb	r2, [r3, #10]
    6788:	7a9a      	ldrb	r2, [r3, #10]
    678a:	72da      	strb	r2, [r3, #11]
    678c:	e7f0      	b.n	6770 <ConfigListenBeaconPeriod_8730A_ROM+0x14>
    678e:	7299      	strb	r1, [r3, #10]
    6790:	e7fa      	b.n	6788 <ConfigListenBeaconPeriod_8730A_ROM+0x2c>
    6792:	46c0      	nop			; (mov r8, r8)
    6794:	23000a28 	.word	0x23000a28
    6798:	23000a24 	.word	0x23000a24

0000679c <PSSetMode_8730A_ROM>:
    679c:	4b0e      	ldr	r3, [pc, #56]	; (67d8 <PSSetMode_8730A_ROM+0x3c>)
    679e:	b570      	push	{r4, r5, r6, lr}
    67a0:	4c0e      	ldr	r4, [pc, #56]	; (67dc <PSSetMode_8730A_ROM+0x40>)
    67a2:	681b      	ldr	r3, [r3, #0]
    67a4:	0005      	movs	r5, r0
    67a6:	4798      	blx	r3
    67a8:	6823      	ldr	r3, [r4, #0]
    67aa:	7958      	ldrb	r0, [r3, #5]
    67ac:	4b0c      	ldr	r3, [pc, #48]	; (67e0 <PSSetMode_8730A_ROM+0x44>)
    67ae:	681b      	ldr	r3, [r3, #0]
    67b0:	4798      	blx	r3
    67b2:	6823      	ldr	r3, [r4, #0]
    67b4:	795b      	ldrb	r3, [r3, #5]
    67b6:	b173      	cbz	r3, 67d6 <PSSetMode_8730A_ROM+0x3a>
    67b8:	7868      	ldrb	r0, [r5, #1]
    67ba:	4b0a      	ldr	r3, [pc, #40]	; (67e4 <PSSetMode_8730A_ROM+0x48>)
    67bc:	0700      	lsls	r0, r0, #28
    67be:	78a9      	ldrb	r1, [r5, #2]
    67c0:	681b      	ldr	r3, [r3, #0]
    67c2:	0f00      	lsrs	r0, r0, #28
    67c4:	4798      	blx	r3
    67c6:	6823      	ldr	r3, [r4, #0]
    67c8:	7dda      	ldrb	r2, [r3, #23]
    67ca:	7c59      	ldrb	r1, [r3, #17]
    67cc:	7c18      	ldrb	r0, [r3, #16]
    67ce:	4b06      	ldr	r3, [pc, #24]	; (67e8 <PSSetMode_8730A_ROM+0x4c>)
    67d0:	681c      	ldr	r4, [r3, #0]
    67d2:	2301      	movs	r3, #1
    67d4:	47a0      	blx	r4
    67d6:	bd70      	pop	{r4, r5, r6, pc}
    67d8:	230007f0 	.word	0x230007f0
    67dc:	23000a28 	.word	0x23000a28
    67e0:	230007fc 	.word	0x230007fc
    67e4:	23000800 	.word	0x23000800
    67e8:	23000960 	.word	0x23000960

000067ec <PS_S2ToS0State_8730A_ROM>:
    67ec:	217f      	movs	r1, #127	; 0x7f
    67ee:	b510      	push	{r4, lr}
    67f0:	4a07      	ldr	r2, [pc, #28]	; (6810 <PS_S2ToS0State_8730A_ROM+0x24>)
    67f2:	20ff      	movs	r0, #255	; 0xff
    67f4:	7813      	ldrb	r3, [r2, #0]
    67f6:	400b      	ands	r3, r1
    67f8:	7013      	strb	r3, [r2, #0]
    67fa:	2200      	movs	r2, #0
    67fc:	4b05      	ldr	r3, [pc, #20]	; (6814 <PS_S2ToS0State_8730A_ROM+0x28>)
    67fe:	0011      	movs	r1, r2
    6800:	681b      	ldr	r3, [r3, #0]
    6802:	4798      	blx	r3
    6804:	4b04      	ldr	r3, [pc, #16]	; (6818 <PS_S2ToS0State_8730A_ROM+0x2c>)
    6806:	210c      	movs	r1, #12
    6808:	2001      	movs	r0, #1
    680a:	681b      	ldr	r3, [r3, #0]
    680c:	4798      	blx	r3
    680e:	bd10      	pop	{r4, pc}
    6810:	43000604 	.word	0x43000604
    6814:	2300094c 	.word	0x2300094c
    6818:	230007ec 	.word	0x230007ec

0000681c <PS_S3ToS2orS0State_8730A_ROM>:
    681c:	2001      	movs	r0, #1
    681e:	4b0e      	ldr	r3, [pc, #56]	; (6858 <PS_S3ToS2orS0State_8730A_ROM+0x3c>)
    6820:	b510      	push	{r4, lr}
    6822:	681b      	ldr	r3, [r3, #0]
    6824:	490d      	ldr	r1, [pc, #52]	; (685c <PS_S3ToS2orS0State_8730A_ROM+0x40>)
    6826:	785a      	ldrb	r2, [r3, #1]
    6828:	4b0d      	ldr	r3, [pc, #52]	; (6860 <PS_S3ToS2orS0State_8730A_ROM+0x44>)
    682a:	4202      	tst	r2, r0
    682c:	d10d      	bne.n	684a <PS_S3ToS2orS0State_8730A_ROM+0x2e>
    682e:	2240      	movs	r2, #64	; 0x40
    6830:	781c      	ldrb	r4, [r3, #0]
    6832:	4322      	orrs	r2, r4
    6834:	701a      	strb	r2, [r3, #0]
    6836:	680b      	ldr	r3, [r1, #0]
    6838:	2104      	movs	r1, #4
    683a:	4798      	blx	r3
    683c:	2200      	movs	r2, #0
    683e:	4b09      	ldr	r3, [pc, #36]	; (6864 <PS_S3ToS2orS0State_8730A_ROM+0x48>)
    6840:	0011      	movs	r1, r2
    6842:	20ff      	movs	r0, #255	; 0xff
    6844:	681b      	ldr	r3, [r3, #0]
    6846:	4798      	blx	r3
    6848:	bd10      	pop	{r4, pc}
    684a:	247f      	movs	r4, #127	; 0x7f
    684c:	781a      	ldrb	r2, [r3, #0]
    684e:	4022      	ands	r2, r4
    6850:	701a      	strb	r2, [r3, #0]
    6852:	680b      	ldr	r3, [r1, #0]
    6854:	210c      	movs	r1, #12
    6856:	e7f0      	b.n	683a <PS_S3ToS2orS0State_8730A_ROM+0x1e>
    6858:	23000a28 	.word	0x23000a28
    685c:	230007ec 	.word	0x230007ec
    6860:	43000604 	.word	0x43000604
    6864:	2300094c 	.word	0x2300094c

00006868 <PS_S1ToS0orS2State_8730A_ROM>:
    6868:	4b11      	ldr	r3, [pc, #68]	; (68b0 <PS_S1ToS0orS2State_8730A_ROM+0x48>)
    686a:	b510      	push	{r4, lr}
    686c:	681b      	ldr	r3, [r3, #0]
    686e:	4c11      	ldr	r4, [pc, #68]	; (68b4 <PS_S1ToS0orS2State_8730A_ROM+0x4c>)
    6870:	785b      	ldrb	r3, [r3, #1]
    6872:	079b      	lsls	r3, r3, #30
    6874:	d403      	bmi.n	687e <PS_S1ToS0orS2State_8730A_ROM+0x16>
    6876:	210c      	movs	r1, #12
    6878:	2001      	movs	r0, #1
    687a:	6823      	ldr	r3, [r4, #0]
    687c:	e00f      	b.n	689e <PS_S1ToS0orS2State_8730A_ROM+0x36>
    687e:	4b0e      	ldr	r3, [pc, #56]	; (68b8 <PS_S1ToS0orS2State_8730A_ROM+0x50>)
    6880:	681b      	ldr	r3, [r3, #0]
    6882:	4798      	blx	r3
    6884:	2801      	cmp	r0, #1
    6886:	d1f6      	bne.n	6876 <PS_S1ToS0orS2State_8730A_ROM+0xe>
    6888:	2240      	movs	r2, #64	; 0x40
    688a:	4b0c      	ldr	r3, [pc, #48]	; (68bc <PS_S1ToS0orS2State_8730A_ROM+0x54>)
    688c:	7819      	ldrb	r1, [r3, #0]
    688e:	430a      	orrs	r2, r1
    6890:	2180      	movs	r1, #128	; 0x80
    6892:	701a      	strb	r2, [r3, #0]
    6894:	781a      	ldrb	r2, [r3, #0]
    6896:	430a      	orrs	r2, r1
    6898:	701a      	strb	r2, [r3, #0]
    689a:	6823      	ldr	r3, [r4, #0]
    689c:	397c      	subs	r1, #124	; 0x7c
    689e:	4798      	blx	r3
    68a0:	2200      	movs	r2, #0
    68a2:	4b07      	ldr	r3, [pc, #28]	; (68c0 <PS_S1ToS0orS2State_8730A_ROM+0x58>)
    68a4:	0011      	movs	r1, r2
    68a6:	20ff      	movs	r0, #255	; 0xff
    68a8:	681b      	ldr	r3, [r3, #0]
    68aa:	4798      	blx	r3
    68ac:	bd10      	pop	{r4, pc}
    68ae:	46c0      	nop			; (mov r8, r8)
    68b0:	23000a28 	.word	0x23000a28
    68b4:	230007ec 	.word	0x230007ec
    68b8:	23000954 	.word	0x23000954
    68bc:	43000604 	.word	0x43000604
    68c0:	2300094c 	.word	0x2300094c

000068c4 <PS_S2ToS4State_8730A_ROM>:
    68c4:	21ff      	movs	r1, #255	; 0xff
    68c6:	4b0e      	ldr	r3, [pc, #56]	; (6900 <PS_S2ToS4State_8730A_ROM+0x3c>)
    68c8:	b510      	push	{r4, lr}
    68ca:	222d      	movs	r2, #45	; 0x2d
    68cc:	0008      	movs	r0, r1
    68ce:	681b      	ldr	r3, [r3, #0]
    68d0:	4798      	blx	r3
    68d2:	4b0c      	ldr	r3, [pc, #48]	; (6904 <PS_S2ToS4State_8730A_ROM+0x40>)
    68d4:	681b      	ldr	r3, [r3, #0]
    68d6:	4798      	blx	r3
    68d8:	2180      	movs	r1, #128	; 0x80
    68da:	4b0b      	ldr	r3, [pc, #44]	; (6908 <PS_S2ToS4State_8730A_ROM+0x44>)
    68dc:	2003      	movs	r0, #3
    68de:	0289      	lsls	r1, r1, #10
    68e0:	681b      	ldr	r3, [r3, #0]
    68e2:	4798      	blx	r3
    68e4:	4b09      	ldr	r3, [pc, #36]	; (690c <PS_S2ToS4State_8730A_ROM+0x48>)
    68e6:	681b      	ldr	r3, [r3, #0]
    68e8:	4798      	blx	r3
    68ea:	4b09      	ldr	r3, [pc, #36]	; (6910 <PS_S2ToS4State_8730A_ROM+0x4c>)
    68ec:	2100      	movs	r1, #0
    68ee:	2001      	movs	r0, #1
    68f0:	681b      	ldr	r3, [r3, #0]
    68f2:	4798      	blx	r3
    68f4:	4b07      	ldr	r3, [pc, #28]	; (6914 <PS_S2ToS4State_8730A_ROM+0x50>)
    68f6:	2001      	movs	r0, #1
    68f8:	681b      	ldr	r3, [r3, #0]
    68fa:	4798      	blx	r3
    68fc:	bd10      	pop	{r4, pc}
    68fe:	46c0      	nop			; (mov r8, r8)
    6900:	2300094c 	.word	0x2300094c
    6904:	23000948 	.word	0x23000948
    6908:	23000768 	.word	0x23000768
    690c:	2300093c 	.word	0x2300093c
    6910:	230007ec 	.word	0x230007ec
    6914:	23000978 	.word	0x23000978

00006918 <PS_S2ToS5State_8730A_ROM>:
    6918:	4b06      	ldr	r3, [pc, #24]	; (6934 <PS_S2ToS5State_8730A_ROM+0x1c>)
    691a:	b510      	push	{r4, lr}
    691c:	222e      	movs	r2, #46	; 0x2e
    691e:	216f      	movs	r1, #111	; 0x6f
    6920:	20ff      	movs	r0, #255	; 0xff
    6922:	681b      	ldr	r3, [r3, #0]
    6924:	4798      	blx	r3
    6926:	4b04      	ldr	r3, [pc, #16]	; (6938 <PS_S2ToS5State_8730A_ROM+0x20>)
    6928:	2102      	movs	r1, #2
    692a:	2001      	movs	r0, #1
    692c:	681b      	ldr	r3, [r3, #0]
    692e:	4798      	blx	r3
    6930:	bd10      	pop	{r4, pc}
    6932:	46c0      	nop			; (mov r8, r8)
    6934:	2300094c 	.word	0x2300094c
    6938:	230007ec 	.word	0x230007ec

0000693c <PS_S5ToS2State_8730A_ROM>:
    693c:	2200      	movs	r2, #0
    693e:	4b06      	ldr	r3, [pc, #24]	; (6958 <PS_S5ToS2State_8730A_ROM+0x1c>)
    6940:	b510      	push	{r4, lr}
    6942:	0011      	movs	r1, r2
    6944:	20ff      	movs	r0, #255	; 0xff
    6946:	681b      	ldr	r3, [r3, #0]
    6948:	4798      	blx	r3
    694a:	4b04      	ldr	r3, [pc, #16]	; (695c <PS_S5ToS2State_8730A_ROM+0x20>)
    694c:	2104      	movs	r1, #4
    694e:	2001      	movs	r0, #1
    6950:	681b      	ldr	r3, [r3, #0]
    6952:	4798      	blx	r3
    6954:	bd10      	pop	{r4, pc}
    6956:	46c0      	nop			; (mov r8, r8)
    6958:	2300094c 	.word	0x2300094c
    695c:	230007ec 	.word	0x230007ec

00006960 <PS_S4ToS2State_8730A_ROM>:
    6960:	b510      	push	{r4, lr}
    6962:	0004      	movs	r4, r0
    6964:	4b09      	ldr	r3, [pc, #36]	; (698c <PS_S4ToS2State_8730A_ROM+0x2c>)
    6966:	2000      	movs	r0, #0
    6968:	681b      	ldr	r3, [r3, #0]
    696a:	4798      	blx	r3
    696c:	4b08      	ldr	r3, [pc, #32]	; (6990 <PS_S4ToS2State_8730A_ROM+0x30>)
    696e:	681b      	ldr	r3, [r3, #0]
    6970:	4798      	blx	r3
    6972:	b12c      	cbz	r4, 6980 <PS_S4ToS2State_8730A_ROM+0x20>
    6974:	2200      	movs	r2, #0
    6976:	4b07      	ldr	r3, [pc, #28]	; (6994 <PS_S4ToS2State_8730A_ROM+0x34>)
    6978:	0011      	movs	r1, r2
    697a:	20ff      	movs	r0, #255	; 0xff
    697c:	681b      	ldr	r3, [r3, #0]
    697e:	4798      	blx	r3
    6980:	4b05      	ldr	r3, [pc, #20]	; (6998 <PS_S4ToS2State_8730A_ROM+0x38>)
    6982:	2104      	movs	r1, #4
    6984:	2001      	movs	r0, #1
    6986:	681b      	ldr	r3, [r3, #0]
    6988:	4798      	blx	r3
    698a:	bd10      	pop	{r4, pc}
    698c:	23000978 	.word	0x23000978
    6990:	23000940 	.word	0x23000940
    6994:	2300094c 	.word	0x2300094c
    6998:	230007ec 	.word	0x230007ec

0000699c <PS_S0ToS6State_8730A_ROM>:
    699c:	21ff      	movs	r1, #255	; 0xff
    699e:	4b07      	ldr	r3, [pc, #28]	; (69bc <PS_S0ToS6State_8730A_ROM+0x20>)
    69a0:	0008      	movs	r0, r1
    69a2:	b510      	push	{r4, lr}
    69a4:	222f      	movs	r2, #47	; 0x2f
    69a6:	681b      	ldr	r3, [r3, #0]
    69a8:	4798      	blx	r3
    69aa:	4b05      	ldr	r3, [pc, #20]	; (69c0 <PS_S0ToS6State_8730A_ROM+0x24>)
    69ac:	681b      	ldr	r3, [r3, #0]
    69ae:	4798      	blx	r3
    69b0:	4b04      	ldr	r3, [pc, #16]	; (69c4 <PS_S0ToS6State_8730A_ROM+0x28>)
    69b2:	2108      	movs	r1, #8
    69b4:	2001      	movs	r0, #1
    69b6:	681b      	ldr	r3, [r3, #0]
    69b8:	4798      	blx	r3
    69ba:	bd10      	pop	{r4, pc}
    69bc:	2300094c 	.word	0x2300094c
    69c0:	2300093c 	.word	0x2300093c
    69c4:	230007ec 	.word	0x230007ec

000069c8 <PS_S6ToS0State_8730A_ROM>:
    69c8:	4b07      	ldr	r3, [pc, #28]	; (69e8 <PS_S6ToS0State_8730A_ROM+0x20>)
    69ca:	b510      	push	{r4, lr}
    69cc:	681b      	ldr	r3, [r3, #0]
    69ce:	4798      	blx	r3
    69d0:	2200      	movs	r2, #0
    69d2:	4b06      	ldr	r3, [pc, #24]	; (69ec <PS_S6ToS0State_8730A_ROM+0x24>)
    69d4:	0011      	movs	r1, r2
    69d6:	20ff      	movs	r0, #255	; 0xff
    69d8:	681b      	ldr	r3, [r3, #0]
    69da:	4798      	blx	r3
    69dc:	4b04      	ldr	r3, [pc, #16]	; (69f0 <PS_S6ToS0State_8730A_ROM+0x28>)
    69de:	210c      	movs	r1, #12
    69e0:	2001      	movs	r0, #1
    69e2:	681b      	ldr	r3, [r3, #0]
    69e4:	4798      	blx	r3
    69e6:	bd10      	pop	{r4, pc}
    69e8:	23000940 	.word	0x23000940
    69ec:	2300094c 	.word	0x2300094c
    69f0:	230007ec 	.word	0x230007ec

000069f4 <InitKeepAlive_8730A_ROM>:
    69f4:	217a      	movs	r1, #122	; 0x7a
    69f6:	4b05      	ldr	r3, [pc, #20]	; (6a0c <InitKeepAlive_8730A_ROM+0x18>)
    69f8:	681b      	ldr	r3, [r3, #0]
    69fa:	781a      	ldrb	r2, [r3, #0]
    69fc:	400a      	ands	r2, r1
    69fe:	701a      	strb	r2, [r3, #0]
    6a00:	2100      	movs	r1, #0
    6a02:	220a      	movs	r2, #10
    6a04:	7099      	strb	r1, [r3, #2]
    6a06:	705a      	strb	r2, [r3, #1]
    6a08:	70d9      	strb	r1, [r3, #3]
    6a0a:	4770      	bx	lr
    6a0c:	230009c0 	.word	0x230009c0

00006a10 <H2CHDL_KeepAlive_8730A_ROM>:
    6a10:	b510      	push	{r4, lr}
    6a12:	2401      	movs	r4, #1
    6a14:	4b0c      	ldr	r3, [pc, #48]	; (6a48 <H2CHDL_KeepAlive_8730A_ROM+0x38>)
    6a16:	7802      	ldrb	r2, [r0, #0]
    6a18:	681b      	ldr	r3, [r3, #0]
    6a1a:	4022      	ands	r2, r4
    6a1c:	7819      	ldrb	r1, [r3, #0]
    6a1e:	43a1      	bics	r1, r4
    6a20:	4311      	orrs	r1, r2
    6a22:	7019      	strb	r1, [r3, #0]
    6a24:	7802      	ldrb	r2, [r0, #0]
    6a26:	b2c9      	uxtb	r1, r1
    6a28:	0892      	lsrs	r2, r2, #2
    6a2a:	4022      	ands	r2, r4
    6a2c:	3403      	adds	r4, #3
    6a2e:	0092      	lsls	r2, r2, #2
    6a30:	43a1      	bics	r1, r4
    6a32:	430a      	orrs	r2, r1
    6a34:	701a      	strb	r2, [r3, #0]
    6a36:	7802      	ldrb	r2, [r0, #0]
    6a38:	0792      	lsls	r2, r2, #30
    6a3a:	d501      	bpl.n	6a40 <H2CHDL_KeepAlive_8730A_ROM+0x30>
    6a3c:	7842      	ldrb	r2, [r0, #1]
    6a3e:	705a      	strb	r2, [r3, #1]
    6a40:	2200      	movs	r2, #0
    6a42:	709a      	strb	r2, [r3, #2]
    6a44:	70da      	strb	r2, [r3, #3]
    6a46:	bd10      	pop	{r4, pc}
    6a48:	230009c0 	.word	0x230009c0

00006a4c <UpdateRxFFReadPtr_DropPkt_8730A_ROM>:
    6a4c:	2280      	movs	r2, #128	; 0x80
    6a4e:	4b03      	ldr	r3, [pc, #12]	; (6a5c <UpdateRxFFReadPtr_DropPkt_8730A_ROM+0x10>)
    6a50:	0252      	lsls	r2, r2, #9
    6a52:	8018      	strh	r0, [r3, #0]
    6a54:	6819      	ldr	r1, [r3, #0]
    6a56:	430a      	orrs	r2, r1
    6a58:	601a      	str	r2, [r3, #0]
    6a5a:	4770      	bx	lr
    6a5c:	43000284 	.word	0x43000284

00006a60 <WaitRXDMAIdle_8730A_ROM>:
    6a60:	b570      	push	{r4, r5, r6, lr}
    6a62:	f240 74d1 	movw	r4, #2001	; 0x7d1
    6a66:	2502      	movs	r5, #2
    6a68:	4b0a      	ldr	r3, [pc, #40]	; (6a94 <WaitRXDMAIdle_8730A_ROM+0x34>)
    6a6a:	781b      	ldrb	r3, [r3, #0]
    6a6c:	0018      	movs	r0, r3
    6a6e:	4028      	ands	r0, r5
    6a70:	422b      	tst	r3, r5
    6a72:	d001      	beq.n	6a78 <WaitRXDMAIdle_8730A_ROM+0x18>
    6a74:	2001      	movs	r0, #1
    6a76:	e007      	b.n	6a88 <WaitRXDMAIdle_8730A_ROM+0x28>
    6a78:	3c01      	subs	r4, #1
    6a7a:	b2a4      	uxth	r4, r4
    6a7c:	b92c      	cbnz	r4, 6a8a <WaitRXDMAIdle_8730A_ROM+0x2a>
    6a7e:	2304      	movs	r3, #4
    6a80:	4a05      	ldr	r2, [pc, #20]	; (6a98 <WaitRXDMAIdle_8730A_ROM+0x38>)
    6a82:	7811      	ldrb	r1, [r2, #0]
    6a84:	430b      	orrs	r3, r1
    6a86:	7013      	strb	r3, [r2, #0]
    6a88:	bd70      	pop	{r4, r5, r6, pc}
    6a8a:	200a      	movs	r0, #10
    6a8c:	4b03      	ldr	r3, [pc, #12]	; (6a9c <WaitRXDMAIdle_8730A_ROM+0x3c>)
    6a8e:	4798      	blx	r3
    6a90:	e7ea      	b.n	6a68 <WaitRXDMAIdle_8730A_ROM+0x8>
    6a92:	46c0      	nop			; (mov r8, r8)
    6a94:	43000286 	.word	0x43000286
    6a98:	430001c1 	.word	0x430001c1
    6a9c:	0000030b 	.word	0x0000030b

00006aa0 <PageOffsetAlignment_8730A_ROM>:
    6aa0:	0003      	movs	r3, r0
    6aa2:	2207      	movs	r2, #7
    6aa4:	4390      	bics	r0, r2
    6aa6:	4213      	tst	r3, r2
    6aa8:	d001      	beq.n	6aae <PageOffsetAlignment_8730A_ROM+0xe>
    6aaa:	3008      	adds	r0, #8
    6aac:	b280      	uxth	r0, r0
    6aae:	4770      	bx	lr

00006ab0 <DropCHK_8730A_ROM>:
    6ab0:	2204      	movs	r2, #4
    6ab2:	4b06      	ldr	r3, [pc, #24]	; (6acc <DropCHK_8730A_ROM+0x1c>)
    6ab4:	0010      	movs	r0, r2
    6ab6:	7819      	ldrb	r1, [r3, #0]
    6ab8:	4211      	tst	r1, r2
    6aba:	d005      	beq.n	6ac8 <DropCHK_8730A_ROM+0x18>
    6abc:	7818      	ldrb	r0, [r3, #0]
    6abe:	2302      	movs	r3, #2
    6ac0:	4018      	ands	r0, r3
    6ac2:	4243      	negs	r3, r0
    6ac4:	4158      	adcs	r0, r3
    6ac6:	3001      	adds	r0, #1
    6ac8:	4770      	bx	lr
    6aca:	46c0      	nop			; (mov r8, r8)
    6acc:	43000286 	.word	0x43000286

00006ad0 <EnableWoWLAN_8730A_ROM>:
    6ad0:	4b10      	ldr	r3, [pc, #64]	; (6b14 <EnableWoWLAN_8730A_ROM+0x44>)
    6ad2:	b510      	push	{r4, lr}
    6ad4:	681b      	ldr	r3, [r3, #0]
    6ad6:	4798      	blx	r3
    6ad8:	2801      	cmp	r0, #1
    6ada:	d014      	beq.n	6b06 <EnableWoWLAN_8730A_ROM+0x36>
    6adc:	4b0e      	ldr	r3, [pc, #56]	; (6b18 <EnableWoWLAN_8730A_ROM+0x48>)
    6ade:	681a      	ldr	r2, [r3, #0]
    6ae0:	2301      	movs	r3, #1
    6ae2:	7a51      	ldrb	r1, [r2, #9]
    6ae4:	430b      	orrs	r3, r1
    6ae6:	7253      	strb	r3, [r2, #9]
    6ae8:	2802      	cmp	r0, #2
    6aea:	d107      	bne.n	6afc <EnableWoWLAN_8730A_ROM+0x2c>
    6aec:	2240      	movs	r2, #64	; 0x40
    6aee:	4b0b      	ldr	r3, [pc, #44]	; (6b1c <EnableWoWLAN_8730A_ROM+0x4c>)
    6af0:	701a      	strb	r2, [r3, #0]
    6af2:	4b0b      	ldr	r3, [pc, #44]	; (6b20 <EnableWoWLAN_8730A_ROM+0x50>)
    6af4:	2001      	movs	r0, #1
    6af6:	681b      	ldr	r3, [r3, #0]
    6af8:	4798      	blx	r3
    6afa:	bd10      	pop	{r4, pc}
    6afc:	2804      	cmp	r0, #4
    6afe:	d1f8      	bne.n	6af2 <EnableWoWLAN_8730A_ROM+0x22>
    6b00:	2241      	movs	r2, #65	; 0x41
    6b02:	4b06      	ldr	r3, [pc, #24]	; (6b1c <EnableWoWLAN_8730A_ROM+0x4c>)
    6b04:	e7f4      	b.n	6af0 <EnableWoWLAN_8730A_ROM+0x20>
    6b06:	2200      	movs	r2, #0
    6b08:	4b06      	ldr	r3, [pc, #24]	; (6b24 <EnableWoWLAN_8730A_ROM+0x54>)
    6b0a:	681b      	ldr	r3, [r3, #0]
    6b0c:	3334      	adds	r3, #52	; 0x34
    6b0e:	701a      	strb	r2, [r3, #0]
    6b10:	e7f3      	b.n	6afa <EnableWoWLAN_8730A_ROM+0x2a>
    6b12:	46c0      	nop			; (mov r8, r8)
    6b14:	23000840 	.word	0x23000840
    6b18:	230009bc 	.word	0x230009bc
    6b1c:	430001c7 	.word	0x430001c7
    6b20:	2300092c 	.word	0x2300092c
    6b24:	23000a28 	.word	0x23000a28

00006b28 <DisWoWLAN_8730A_ROM>:
    6b28:	b510      	push	{r4, lr}
    6b2a:	4b08      	ldr	r3, [pc, #32]	; (6b4c <DisWoWLAN_8730A_ROM+0x24>)
    6b2c:	681b      	ldr	r3, [r3, #0]
    6b2e:	3334      	adds	r3, #52	; 0x34
    6b30:	781a      	ldrb	r2, [r3, #0]
    6b32:	4b07      	ldr	r3, [pc, #28]	; (6b50 <DisWoWLAN_8730A_ROM+0x28>)
    6b34:	701a      	strb	r2, [r3, #0]
    6b36:	4b07      	ldr	r3, [pc, #28]	; (6b54 <DisWoWLAN_8730A_ROM+0x2c>)
    6b38:	681b      	ldr	r3, [r3, #0]
    6b3a:	7858      	ldrb	r0, [r3, #1]
    6b3c:	4b06      	ldr	r3, [pc, #24]	; (6b58 <DisWoWLAN_8730A_ROM+0x30>)
    6b3e:	681b      	ldr	r3, [r3, #0]
    6b40:	4798      	blx	r3
    6b42:	4b06      	ldr	r3, [pc, #24]	; (6b5c <DisWoWLAN_8730A_ROM+0x34>)
    6b44:	4a06      	ldr	r2, [pc, #24]	; (6b60 <DisWoWLAN_8730A_ROM+0x38>)
    6b46:	801a      	strh	r2, [r3, #0]
    6b48:	bd10      	pop	{r4, pc}
    6b4a:	46c0      	nop			; (mov r8, r8)
    6b4c:	23000a28 	.word	0x23000a28
    6b50:	430001c8 	.word	0x430001c8
    6b54:	230009c4 	.word	0x230009c4
    6b58:	2300085c 	.word	0x2300085c
    6b5c:	43000124 	.word	0x43000124
    6b60:	ffff8000 	.word	0xffff8000

00006b64 <H2CHDL_WoWLAN_8730A_ROM>:
    6b64:	2201      	movs	r2, #1
    6b66:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    6b68:	7804      	ldrb	r4, [r0, #0]
    6b6a:	4214      	tst	r4, r2
    6b6c:	d103      	bne.n	6b76 <H2CHDL_WoWLAN_8730A_ROM+0x12>
    6b6e:	4b39      	ldr	r3, [pc, #228]	; (6c54 <H2CHDL_WoWLAN_8730A_ROM+0xf0>)
    6b70:	681b      	ldr	r3, [r3, #0]
    6b72:	4798      	blx	r3
    6b74:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    6b76:	2502      	movs	r5, #2
    6b78:	4b37      	ldr	r3, [pc, #220]	; (6c58 <H2CHDL_WoWLAN_8730A_ROM+0xf4>)
    6b7a:	4014      	ands	r4, r2
    6b7c:	681b      	ldr	r3, [r3, #0]
    6b7e:	2708      	movs	r7, #8
    6b80:	78d9      	ldrb	r1, [r3, #3]
    6b82:	4391      	bics	r1, r2
    6b84:	4321      	orrs	r1, r4
    6b86:	70d9      	strb	r1, [r3, #3]
    6b88:	7801      	ldrb	r1, [r0, #0]
    6b8a:	78dc      	ldrb	r4, [r3, #3]
    6b8c:	0849      	lsrs	r1, r1, #1
    6b8e:	4011      	ands	r1, r2
    6b90:	43ac      	bics	r4, r5
    6b92:	0049      	lsls	r1, r1, #1
    6b94:	4321      	orrs	r1, r4
    6b96:	70d9      	strb	r1, [r3, #3]
    6b98:	7801      	ldrb	r1, [r0, #0]
    6b9a:	2404      	movs	r4, #4
    6b9c:	40e9      	lsrs	r1, r5
    6b9e:	4011      	ands	r1, r2
    6ba0:	40a9      	lsls	r1, r5
    6ba2:	78de      	ldrb	r6, [r3, #3]
    6ba4:	43a6      	bics	r6, r4
    6ba6:	4331      	orrs	r1, r6
    6ba8:	70d9      	strb	r1, [r3, #3]
    6baa:	7801      	ldrb	r1, [r0, #0]
    6bac:	78de      	ldrb	r6, [r3, #3]
    6bae:	08c9      	lsrs	r1, r1, #3
    6bb0:	4011      	ands	r1, r2
    6bb2:	43be      	bics	r6, r7
    6bb4:	00c9      	lsls	r1, r1, #3
    6bb6:	4331      	orrs	r1, r6
    6bb8:	70d9      	strb	r1, [r3, #3]
    6bba:	7801      	ldrb	r1, [r0, #0]
    6bbc:	78de      	ldrb	r6, [r3, #3]
    6bbe:	40e1      	lsrs	r1, r4
    6bc0:	4011      	ands	r1, r2
    6bc2:	40a1      	lsls	r1, r4
    6bc4:	19ff      	adds	r7, r7, r7
    6bc6:	43be      	bics	r6, r7
    6bc8:	4331      	orrs	r1, r6
    6bca:	70d9      	strb	r1, [r3, #3]
    6bcc:	7801      	ldrb	r1, [r0, #0]
    6bce:	78de      	ldrb	r6, [r3, #3]
    6bd0:	0949      	lsrs	r1, r1, #5
    6bd2:	4011      	ands	r1, r2
    6bd4:	3710      	adds	r7, #16
    6bd6:	43be      	bics	r6, r7
    6bd8:	0149      	lsls	r1, r1, #5
    6bda:	4331      	orrs	r1, r6
    6bdc:	2640      	movs	r6, #64	; 0x40
    6bde:	70d9      	strb	r1, [r3, #3]
    6be0:	7801      	ldrb	r1, [r0, #0]
    6be2:	0989      	lsrs	r1, r1, #6
    6be4:	400a      	ands	r2, r1
    6be6:	78d9      	ldrb	r1, [r3, #3]
    6be8:	0192      	lsls	r2, r2, #6
    6bea:	43b1      	bics	r1, r6
    6bec:	430a      	orrs	r2, r1
    6bee:	70da      	strb	r2, [r3, #3]
    6bf0:	7802      	ldrb	r2, [r0, #0]
    6bf2:	363f      	adds	r6, #63	; 0x3f
    6bf4:	09d2      	lsrs	r2, r2, #7
    6bf6:	01d1      	lsls	r1, r2, #7
    6bf8:	78da      	ldrb	r2, [r3, #3]
    6bfa:	4032      	ands	r2, r6
    6bfc:	430a      	orrs	r2, r1
    6bfe:	70da      	strb	r2, [r3, #3]
    6c00:	7841      	ldrb	r1, [r0, #1]
    6c02:	791a      	ldrb	r2, [r3, #4]
    6c04:	09c9      	lsrs	r1, r1, #7
    6c06:	01c9      	lsls	r1, r1, #7
    6c08:	4032      	ands	r2, r6
    6c0a:	430a      	orrs	r2, r1
    6c0c:	711a      	strb	r2, [r3, #4]
    6c0e:	4b13      	ldr	r3, [pc, #76]	; (6c5c <H2CHDL_WoWLAN_8730A_ROM+0xf8>)
    6c10:	781a      	ldrb	r2, [r3, #0]
    6c12:	4315      	orrs	r5, r2
    6c14:	701d      	strb	r5, [r3, #0]
    6c16:	7802      	ldrb	r2, [r0, #0]
    6c18:	4222      	tst	r2, r4
    6c1a:	d002      	beq.n	6c22 <H2CHDL_WoWLAN_8730A_ROM+0xbe>
    6c1c:	781a      	ldrb	r2, [r3, #0]
    6c1e:	4314      	orrs	r4, r2
    6c20:	701c      	strb	r4, [r3, #0]
    6c22:	2208      	movs	r2, #8
    6c24:	7801      	ldrb	r1, [r0, #0]
    6c26:	4211      	tst	r1, r2
    6c28:	d002      	beq.n	6c30 <H2CHDL_WoWLAN_8730A_ROM+0xcc>
    6c2a:	7819      	ldrb	r1, [r3, #0]
    6c2c:	430a      	orrs	r2, r1
    6c2e:	701a      	strb	r2, [r3, #0]
    6c30:	4d0b      	ldr	r5, [pc, #44]	; (6c60 <H2CHDL_WoWLAN_8730A_ROM+0xfc>)
    6c32:	2260      	movs	r2, #96	; 0x60
    6c34:	6828      	ldr	r0, [r5, #0]
    6c36:	2100      	movs	r1, #0
    6c38:	30d1      	adds	r0, #209	; 0xd1
    6c3a:	4c0a      	ldr	r4, [pc, #40]	; (6c64 <H2CHDL_WoWLAN_8730A_ROM+0x100>)
    6c3c:	30ff      	adds	r0, #255	; 0xff
    6c3e:	47a0      	blx	r4
    6c40:	f240 2300 	movw	r3, #512	; 0x200
    6c44:	2205      	movs	r2, #5
    6c46:	6828      	ldr	r0, [r5, #0]
    6c48:	2100      	movs	r1, #0
    6c4a:	54c2      	strb	r2, [r0, r3]
    6c4c:	3203      	adds	r2, #3
    6c4e:	30f5      	adds	r0, #245	; 0xf5
    6c50:	47a0      	blx	r4
    6c52:	e78f      	b.n	6b74 <H2CHDL_WoWLAN_8730A_ROM+0x10>
    6c54:	23000848 	.word	0x23000848
    6c58:	230009bc 	.word	0x230009bc
    6c5c:	43000690 	.word	0x43000690
    6c60:	230009ec 	.word	0x230009ec
    6c64:	00004ca9 	.word	0x00004ca9

00006c68 <DropDecision_8730A_ROM>:
    6c68:	2001      	movs	r0, #1
    6c6a:	4b04      	ldr	r3, [pc, #16]	; (6c7c <DropDecision_8730A_ROM+0x14>)
    6c6c:	681b      	ldr	r3, [r3, #0]
    6c6e:	7a5a      	ldrb	r2, [r3, #9]
    6c70:	4202      	tst	r2, r0
    6c72:	d002      	beq.n	6c7a <DropDecision_8730A_ROM+0x12>
    6c74:	78db      	ldrb	r3, [r3, #3]
    6c76:	091b      	lsrs	r3, r3, #4
    6c78:	4018      	ands	r0, r3
    6c7a:	4770      	bx	lr
    6c7c:	230009bc 	.word	0x230009bc

00006c80 <FwDisConnectWakeUpHost_8730A_ROM>:
    6c80:	2204      	movs	r2, #4
    6c82:	4b06      	ldr	r3, [pc, #24]	; (6c9c <FwDisConnectWakeUpHost_8730A_ROM+0x1c>)
    6c84:	28a0      	cmp	r0, #160	; 0xa0
    6c86:	d000      	beq.n	6c8a <FwDisConnectWakeUpHost_8730A_ROM+0xa>
    6c88:	2208      	movs	r2, #8
    6c8a:	701a      	strb	r2, [r3, #0]
    6c8c:	4b04      	ldr	r3, [pc, #16]	; (6ca0 <FwDisConnectWakeUpHost_8730A_ROM+0x20>)
    6c8e:	681a      	ldr	r2, [r3, #0]
    6c90:	2301      	movs	r3, #1
    6c92:	7a51      	ldrb	r1, [r2, #9]
    6c94:	430b      	orrs	r3, r1
    6c96:	7253      	strb	r3, [r2, #9]
    6c98:	4770      	bx	lr
    6c9a:	46c0      	nop			; (mov r8, r8)
    6c9c:	430001c7 	.word	0x430001c7
    6ca0:	230009bc 	.word	0x230009bc

00006ca4 <DestinationMatch_8730A_ROM>:
    6ca4:	b537      	push	{r0, r1, r2, r4, r5, lr}
    6ca6:	2400      	movs	r4, #0
    6ca8:	1d05      	adds	r5, r0, #4
    6caa:	b2ad      	uxth	r5, r5
    6cac:	4b0a      	ldr	r3, [pc, #40]	; (6cd8 <DestinationMatch_8730A_ROM+0x34>)
    6cae:	b2e1      	uxtb	r1, r4
    6cb0:	681b      	ldr	r3, [r3, #0]
    6cb2:	0028      	movs	r0, r5
    6cb4:	4798      	blx	r3
    6cb6:	466b      	mov	r3, sp
    6cb8:	5518      	strb	r0, [r3, r4]
    6cba:	3401      	adds	r4, #1
    6cbc:	2c06      	cmp	r4, #6
    6cbe:	d1f5      	bne.n	6cac <DestinationMatch_8730A_ROM+0x8>
    6cc0:	4b06      	ldr	r3, [pc, #24]	; (6cdc <DestinationMatch_8730A_ROM+0x38>)
    6cc2:	0022      	movs	r2, r4
    6cc4:	6818      	ldr	r0, [r3, #0]
    6cc6:	4669      	mov	r1, sp
    6cc8:	4b05      	ldr	r3, [pc, #20]	; (6ce0 <DestinationMatch_8730A_ROM+0x3c>)
    6cca:	300a      	adds	r0, #10
    6ccc:	4798      	blx	r3
    6cce:	4243      	negs	r3, r0
    6cd0:	4158      	adcs	r0, r3
    6cd2:	b2c0      	uxtb	r0, r0
    6cd4:	bd3e      	pop	{r1, r2, r3, r4, r5, pc}
    6cd6:	46c0      	nop			; (mov r8, r8)
    6cd8:	230007e0 	.word	0x230007e0
    6cdc:	230009bc 	.word	0x230009bc
    6ce0:	00004b19 	.word	0x00004b19

00006ce4 <PassSecurityInfoToDriver_8730A_ROM>:
    6ce4:	2300      	movs	r3, #0
    6ce6:	b510      	push	{r4, lr}
    6ce8:	4914      	ldr	r1, [pc, #80]	; (6d3c <PassSecurityInfoToDriver_8730A_ROM+0x58>)
    6cea:	4a15      	ldr	r2, [pc, #84]	; (6d40 <PassSecurityInfoToDriver_8730A_ROM+0x5c>)
    6cec:	189c      	adds	r4, r3, r2
    6cee:	680a      	ldr	r2, [r1, #0]
    6cf0:	18d2      	adds	r2, r2, r3
    6cf2:	32d1      	adds	r2, #209	; 0xd1
    6cf4:	32ff      	adds	r2, #255	; 0xff
    6cf6:	7812      	ldrb	r2, [r2, #0]
    6cf8:	3301      	adds	r3, #1
    6cfa:	7022      	strb	r2, [r4, #0]
    6cfc:	2b08      	cmp	r3, #8
    6cfe:	d1f4      	bne.n	6cea <PassSecurityInfoToDriver_8730A_ROM+0x6>
    6d00:	680a      	ldr	r2, [r1, #0]
    6d02:	33f9      	adds	r3, #249	; 0xf9
    6d04:	33ff      	adds	r3, #255	; 0xff
    6d06:	5cd2      	ldrb	r2, [r2, r3]
    6d08:	4b0e      	ldr	r3, [pc, #56]	; (6d44 <PassSecurityInfoToDriver_8730A_ROM+0x60>)
    6d0a:	701a      	strb	r2, [r3, #0]
    6d0c:	f240 2302 	movw	r3, #514	; 0x202
    6d10:	680a      	ldr	r2, [r1, #0]
    6d12:	5cd2      	ldrb	r2, [r2, r3]
    6d14:	4b0c      	ldr	r3, [pc, #48]	; (6d48 <PassSecurityInfoToDriver_8730A_ROM+0x64>)
    6d16:	701a      	strb	r2, [r3, #0]
    6d18:	f240 2303 	movw	r3, #515	; 0x203
    6d1c:	2201      	movs	r2, #1
    6d1e:	6809      	ldr	r1, [r1, #0]
    6d20:	54ca      	strb	r2, [r1, r3]
    6d22:	4b0a      	ldr	r3, [pc, #40]	; (6d4c <PassSecurityInfoToDriver_8730A_ROM+0x68>)
    6d24:	681b      	ldr	r3, [r3, #0]
    6d26:	785b      	ldrb	r3, [r3, #1]
    6d28:	b13b      	cbz	r3, 6d3a <PassSecurityInfoToDriver_8730A_ROM+0x56>
    6d2a:	4b09      	ldr	r3, [pc, #36]	; (6d50 <PassSecurityInfoToDriver_8730A_ROM+0x6c>)
    6d2c:	31d1      	adds	r1, #209	; 0xd1
    6d2e:	01c0      	lsls	r0, r0, #7
    6d30:	18c0      	adds	r0, r0, r3
    6d32:	31ff      	adds	r1, #255	; 0xff
    6d34:	4b07      	ldr	r3, [pc, #28]	; (6d54 <PassSecurityInfoToDriver_8730A_ROM+0x70>)
    6d36:	325f      	adds	r2, #95	; 0x5f
    6d38:	4798      	blx	r3
    6d3a:	bd10      	pop	{r4, pc}
    6d3c:	230009ec 	.word	0x230009ec
    6d40:	43000484 	.word	0x43000484
    6d44:	43000068 	.word	0x43000068
    6d48:	43000069 	.word	0x43000069
    6d4c:	230009c4 	.word	0x230009c4
    6d50:	43020000 	.word	0x43020000
    6d54:	00004b5d 	.word	0x00004b5d

00006d58 <GetRemoteControlInfo_8730A_ROM>:
    6d58:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    6d5a:	4e27      	ldr	r6, [pc, #156]	; (6df8 <GetRemoteControlInfo_8730A_ROM+0xa0>)
    6d5c:	01c4      	lsls	r4, r0, #7
    6d5e:	6830      	ldr	r0, [r6, #0]
    6d60:	4b26      	ldr	r3, [pc, #152]	; (6dfc <GetRemoteControlInfo_8730A_ROM+0xa4>)
    6d62:	30d1      	adds	r0, #209	; 0xd1
    6d64:	18e1      	adds	r1, r4, r3
    6d66:	2208      	movs	r2, #8
    6d68:	4d25      	ldr	r5, [pc, #148]	; (6e00 <GetRemoteControlInfo_8730A_ROM+0xa8>)
    6d6a:	30ff      	adds	r0, #255	; 0xff
    6d6c:	47a8      	blx	r5
    6d6e:	4a25      	ldr	r2, [pc, #148]	; (6e04 <GetRemoteControlInfo_8730A_ROM+0xac>)
    6d70:	6830      	ldr	r0, [r6, #0]
    6d72:	4b25      	ldr	r3, [pc, #148]	; (6e08 <GetRemoteControlInfo_8730A_ROM+0xb0>)
    6d74:	1881      	adds	r1, r0, r2
    6d76:	4a25      	ldr	r2, [pc, #148]	; (6e0c <GetRemoteControlInfo_8730A_ROM+0xb4>)
    6d78:	781f      	ldrb	r7, [r3, #0]
    6d7a:	54cf      	strb	r7, [r1, r3]
    6d7c:	3301      	adds	r3, #1
    6d7e:	4293      	cmp	r3, r2
    6d80:	d1fa      	bne.n	6d78 <GetRemoteControlInfo_8730A_ROM+0x20>
    6d82:	4b23      	ldr	r3, [pc, #140]	; (6e10 <GetRemoteControlInfo_8730A_ROM+0xb8>)
    6d84:	18e3      	adds	r3, r4, r3
    6d86:	781b      	ldrb	r3, [r3, #0]
    6d88:	2bdd      	cmp	r3, #221	; 0xdd
    6d8a:	d134      	bne.n	6df6 <GetRemoteControlInfo_8730A_ROM+0x9e>
    6d8c:	4b21      	ldr	r3, [pc, #132]	; (6e14 <GetRemoteControlInfo_8730A_ROM+0xbc>)
    6d8e:	18e3      	adds	r3, r4, r3
    6d90:	781b      	ldrb	r3, [r3, #0]
    6d92:	b2df      	uxtb	r7, r3
    6d94:	07db      	lsls	r3, r3, #31
    6d96:	d506      	bpl.n	6da6 <GetRemoteControlInfo_8730A_ROM+0x4e>
    6d98:	f240 2308 	movw	r3, #520	; 0x208
    6d9c:	18c0      	adds	r0, r0, r3
    6d9e:	4b1e      	ldr	r3, [pc, #120]	; (6e18 <GetRemoteControlInfo_8730A_ROM+0xc0>)
    6da0:	2208      	movs	r2, #8
    6da2:	18e1      	adds	r1, r4, r3
    6da4:	47a8      	blx	r5
    6da6:	07bb      	lsls	r3, r7, #30
    6da8:	d525      	bpl.n	6df6 <GetRemoteControlInfo_8730A_ROM+0x9e>
    6daa:	4b1c      	ldr	r3, [pc, #112]	; (6e1c <GetRemoteControlInfo_8730A_ROM+0xc4>)
    6dac:	6830      	ldr	r0, [r6, #0]
    6dae:	18e3      	adds	r3, r4, r3
    6db0:	781a      	ldrb	r2, [r3, #0]
    6db2:	f240 2300 	movw	r3, #512	; 0x200
    6db6:	54c2      	strb	r2, [r0, r3]
    6db8:	4b19      	ldr	r3, [pc, #100]	; (6e20 <GetRemoteControlInfo_8730A_ROM+0xc8>)
    6dba:	2208      	movs	r2, #8
    6dbc:	18e1      	adds	r1, r4, r3
    6dbe:	f240 2310 	movw	r3, #528	; 0x210
    6dc2:	18c0      	adds	r0, r0, r3
    6dc4:	47a8      	blx	r5
    6dc6:	f240 2218 	movw	r2, #536	; 0x218
    6dca:	4b16      	ldr	r3, [pc, #88]	; (6e24 <GetRemoteControlInfo_8730A_ROM+0xcc>)
    6dcc:	18e1      	adds	r1, r4, r3
    6dce:	6833      	ldr	r3, [r6, #0]
    6dd0:	1898      	adds	r0, r3, r2
    6dd2:	2208      	movs	r2, #8
    6dd4:	47a8      	blx	r5
    6dd6:	f240 2220 	movw	r2, #544	; 0x220
    6dda:	4b13      	ldr	r3, [pc, #76]	; (6e28 <GetRemoteControlInfo_8730A_ROM+0xd0>)
    6ddc:	18e1      	adds	r1, r4, r3
    6dde:	6833      	ldr	r3, [r6, #0]
    6de0:	1898      	adds	r0, r3, r2
    6de2:	2208      	movs	r2, #8
    6de4:	47a8      	blx	r5
    6de6:	4b11      	ldr	r3, [pc, #68]	; (6e2c <GetRemoteControlInfo_8730A_ROM+0xd4>)
    6de8:	6830      	ldr	r0, [r6, #0]
    6dea:	18e1      	adds	r1, r4, r3
    6dec:	f240 2328 	movw	r3, #552	; 0x228
    6df0:	2208      	movs	r2, #8
    6df2:	18c0      	adds	r0, r0, r3
    6df4:	47a8      	blx	r5
    6df6:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    6df8:	230009ec 	.word	0x230009ec
    6dfc:	43020000 	.word	0x43020000
    6e00:	00004b5d 	.word	0x00004b5d
    6e04:	bcfffd4c 	.word	0xbcfffd4c
    6e08:	43000484 	.word	0x43000484
    6e0c:	4300048c 	.word	0x4300048c
    6e10:	43020008 	.word	0x43020008
    6e14:	43020009 	.word	0x43020009
    6e18:	43020010 	.word	0x43020010
    6e1c:	4302000a 	.word	0x4302000a
    6e20:	43020018 	.word	0x43020018
    6e24:	43020020 	.word	0x43020020
    6e28:	43020028 	.word	0x43020028
    6e2c:	43020030 	.word	0x43020030

00006e30 <GetARPInfo_8730A_ROM>:
    6e30:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    6e32:	4b16      	ldr	r3, [pc, #88]	; (6e8c <GetARPInfo_8730A_ROM+0x5c>)
    6e34:	01c5      	lsls	r5, r0, #7
    6e36:	18ee      	adds	r6, r5, r3
    6e38:	4b15      	ldr	r3, [pc, #84]	; (6e90 <GetARPInfo_8730A_ROM+0x60>)
    6e3a:	7830      	ldrb	r0, [r6, #0]
    6e3c:	18eb      	adds	r3, r5, r3
    6e3e:	7819      	ldrb	r1, [r3, #0]
    6e40:	4b14      	ldr	r3, [pc, #80]	; (6e94 <GetARPInfo_8730A_ROM+0x64>)
    6e42:	b2c9      	uxtb	r1, r1
    6e44:	18eb      	adds	r3, r5, r3
    6e46:	781a      	ldrb	r2, [r3, #0]
    6e48:	4b13      	ldr	r3, [pc, #76]	; (6e98 <GetARPInfo_8730A_ROM+0x68>)
    6e4a:	b2d2      	uxtb	r2, r2
    6e4c:	681b      	ldr	r3, [r3, #0]
    6e4e:	b2c0      	uxtb	r0, r0
    6e50:	4798      	blx	r3
    6e52:	0004      	movs	r4, r0
    6e54:	4b11      	ldr	r3, [pc, #68]	; (6e9c <GetARPInfo_8730A_ROM+0x6c>)
    6e56:	4f12      	ldr	r7, [pc, #72]	; (6ea0 <GetARPInfo_8730A_ROM+0x70>)
    6e58:	18e9      	adds	r1, r5, r3
    6e5a:	4d12      	ldr	r5, [pc, #72]	; (6ea4 <GetARPInfo_8730A_ROM+0x74>)
    6e5c:	19a4      	adds	r4, r4, r6
    6e5e:	6828      	ldr	r0, [r5, #0]
    6e60:	2206      	movs	r2, #6
    6e62:	47b8      	blx	r7
    6e64:	0021      	movs	r1, r4
    6e66:	682b      	ldr	r3, [r5, #0]
    6e68:	2204      	movs	r2, #4
    6e6a:	1d98      	adds	r0, r3, #6
    6e6c:	3116      	adds	r1, #22
    6e6e:	47b8      	blx	r7
    6e70:	0021      	movs	r1, r4
    6e72:	6828      	ldr	r0, [r5, #0]
    6e74:	2206      	movs	r2, #6
    6e76:	311a      	adds	r1, #26
    6e78:	300a      	adds	r0, #10
    6e7a:	47b8      	blx	r7
    6e7c:	0021      	movs	r1, r4
    6e7e:	6828      	ldr	r0, [r5, #0]
    6e80:	2204      	movs	r2, #4
    6e82:	3120      	adds	r1, #32
    6e84:	3010      	adds	r0, #16
    6e86:	47b8      	blx	r7
    6e88:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    6e8a:	46c0      	nop			; (mov r8, r8)
    6e8c:	43020028 	.word	0x43020028
    6e90:	43020029 	.word	0x43020029
    6e94:	4302002c 	.word	0x4302002c
    6e98:	230008b4 	.word	0x230008b4
    6e9c:	43020038 	.word	0x43020038
    6ea0:	00004b5d 	.word	0x00004b5d
    6ea4:	230009ec 	.word	0x230009ec

00006ea8 <OnARP_8730A_ROM>:
    6ea8:	b5f0      	push	{r4, r5, r6, r7, lr}
    6eaa:	2400      	movs	r4, #0
    6eac:	1841      	adds	r1, r0, r1
    6eae:	b28b      	uxth	r3, r1
    6eb0:	4e5f      	ldr	r6, [pc, #380]	; (7030 <OnARP_8730A_ROM+0x188>)
    6eb2:	b08b      	sub	sp, #44	; 0x2c
    6eb4:	0005      	movs	r5, r0
    6eb6:	1d98      	adds	r0, r3, #6
    6eb8:	9301      	str	r3, [sp, #4]
    6eba:	0021      	movs	r1, r4
    6ebc:	6833      	ldr	r3, [r6, #0]
    6ebe:	9404      	str	r4, [sp, #16]
    6ec0:	9405      	str	r4, [sp, #20]
    6ec2:	b280      	uxth	r0, r0
    6ec4:	4798      	blx	r3
    6ec6:	2808      	cmp	r0, #8
    6ec8:	d002      	beq.n	6ed0 <OnARP_8730A_ROM+0x28>
    6eca:	2000      	movs	r0, #0
    6ecc:	b00b      	add	sp, #44	; 0x2c
    6ece:	bdf0      	pop	{r4, r5, r6, r7, pc}
    6ed0:	9b01      	ldr	r3, [sp, #4]
    6ed2:	0021      	movs	r1, r4
    6ed4:	1dd8      	adds	r0, r3, #7
    6ed6:	b280      	uxth	r0, r0
    6ed8:	6833      	ldr	r3, [r6, #0]
    6eda:	4798      	blx	r3
    6edc:	2806      	cmp	r0, #6
    6ede:	d1f4      	bne.n	6eca <OnARP_8730A_ROM+0x22>
    6ee0:	002c      	movs	r4, r5
    6ee2:	af06      	add	r7, sp, #24
    6ee4:	3510      	adds	r5, #16
    6ee6:	b2ab      	uxth	r3, r5
    6ee8:	003d      	movs	r5, r7
    6eea:	340a      	adds	r4, #10
    6eec:	b2a4      	uxth	r4, r4
    6eee:	9302      	str	r3, [sp, #8]
    6ef0:	0020      	movs	r0, r4
    6ef2:	6833      	ldr	r3, [r6, #0]
    6ef4:	2100      	movs	r1, #0
    6ef6:	4798      	blx	r3
    6ef8:	3401      	adds	r4, #1
    6efa:	9b02      	ldr	r3, [sp, #8]
    6efc:	b2a4      	uxth	r4, r4
    6efe:	7038      	strb	r0, [r7, #0]
    6f00:	3701      	adds	r7, #1
    6f02:	429c      	cmp	r4, r3
    6f04:	d1f4      	bne.n	6ef0 <OnARP_8730A_ROM+0x48>
    6f06:	4f4b      	ldr	r7, [pc, #300]	; (7034 <OnARP_8730A_ROM+0x18c>)
    6f08:	4c4b      	ldr	r4, [pc, #300]	; (7038 <OnARP_8730A_ROM+0x190>)
    6f0a:	6839      	ldr	r1, [r7, #0]
    6f0c:	2206      	movs	r2, #6
    6f0e:	0028      	movs	r0, r5
    6f10:	3110      	adds	r1, #16
    6f12:	47a0      	blx	r4
    6f14:	9403      	str	r4, [sp, #12]
    6f16:	b108      	cbz	r0, 6f1c <OnARP_8730A_ROM+0x74>
    6f18:	2001      	movs	r0, #1
    6f1a:	e7d7      	b.n	6ecc <OnARP_8730A_ROM+0x24>
    6f1c:	9b01      	ldr	r3, [sp, #4]
    6f1e:	9c01      	ldr	r4, [sp, #4]
    6f20:	3324      	adds	r3, #36	; 0x24
    6f22:	3420      	adds	r4, #32
    6f24:	b29b      	uxth	r3, r3
    6f26:	b2a4      	uxth	r4, r4
    6f28:	ad05      	add	r5, sp, #20
    6f2a:	9302      	str	r3, [sp, #8]
    6f2c:	0020      	movs	r0, r4
    6f2e:	6833      	ldr	r3, [r6, #0]
    6f30:	2100      	movs	r1, #0
    6f32:	4798      	blx	r3
    6f34:	3401      	adds	r4, #1
    6f36:	9b02      	ldr	r3, [sp, #8]
    6f38:	b2a4      	uxth	r4, r4
    6f3a:	7028      	strb	r0, [r5, #0]
    6f3c:	3501      	adds	r5, #1
    6f3e:	429c      	cmp	r4, r3
    6f40:	d1f4      	bne.n	6f2c <OnARP_8730A_ROM+0x84>
    6f42:	4b3e      	ldr	r3, [pc, #248]	; (703c <OnARP_8730A_ROM+0x194>)
    6f44:	2204      	movs	r2, #4
    6f46:	6819      	ldr	r1, [r3, #0]
    6f48:	a805      	add	r0, sp, #20
    6f4a:	3106      	adds	r1, #6
    6f4c:	9b03      	ldr	r3, [sp, #12]
    6f4e:	4798      	blx	r3
    6f50:	493b      	ldr	r1, [pc, #236]	; (7040 <OnARP_8730A_ROM+0x198>)
    6f52:	2800      	cmp	r0, #0
    6f54:	d163      	bne.n	701e <OnARP_8730A_ROM+0x176>
    6f56:	2304      	movs	r3, #4
    6f58:	780a      	ldrb	r2, [r1, #0]
    6f5a:	439a      	bics	r2, r3
    6f5c:	3b03      	subs	r3, #3
    6f5e:	431a      	orrs	r2, r3
    6f60:	700a      	strb	r2, [r1, #0]
    6f62:	683a      	ldr	r2, [r7, #0]
    6f64:	7891      	ldrb	r1, [r2, #2]
    6f66:	0008      	movs	r0, r1
    6f68:	4018      	ands	r0, r3
    6f6a:	4219      	tst	r1, r3
    6f6c:	d007      	beq.n	6f7e <OnARP_8730A_ROM+0xd6>
    6f6e:	2109      	movs	r1, #9
    6f70:	4a34      	ldr	r2, [pc, #208]	; (7044 <OnARP_8730A_ROM+0x19c>)
    6f72:	7011      	strb	r1, [r2, #0]
    6f74:	6839      	ldr	r1, [r7, #0]
    6f76:	7a4a      	ldrb	r2, [r1, #9]
    6f78:	4313      	orrs	r3, r2
    6f7a:	724b      	strb	r3, [r1, #9]
    6f7c:	e7a6      	b.n	6ecc <OnARP_8730A_ROM+0x24>
    6f7e:	4932      	ldr	r1, [pc, #200]	; (7048 <OnARP_8730A_ROM+0x1a0>)
    6f80:	6809      	ldr	r1, [r1, #0]
    6f82:	7809      	ldrb	r1, [r1, #0]
    6f84:	2902      	cmp	r1, #2
    6f86:	d10a      	bne.n	6f9e <OnARP_8730A_ROM+0xf6>
    6f88:	7852      	ldrb	r2, [r2, #1]
    6f8a:	0712      	lsls	r2, r2, #28
    6f8c:	d407      	bmi.n	6f9e <OnARP_8730A_ROM+0xf6>
    6f8e:	4a2d      	ldr	r2, [pc, #180]	; (7044 <OnARP_8730A_ROM+0x19c>)
    6f90:	3107      	adds	r1, #7
    6f92:	7011      	strb	r1, [r2, #0]
    6f94:	6839      	ldr	r1, [r7, #0]
    6f96:	7a4a      	ldrb	r2, [r1, #9]
    6f98:	431a      	orrs	r2, r3
    6f9a:	724a      	strb	r2, [r1, #9]
    6f9c:	e7bc      	b.n	6f18 <OnARP_8730A_ROM+0x70>
    6f9e:	9d01      	ldr	r5, [sp, #4]
    6fa0:	9c01      	ldr	r4, [sp, #4]
    6fa2:	3510      	adds	r5, #16
    6fa4:	ab08      	add	r3, sp, #32
    6fa6:	3416      	adds	r4, #22
    6fa8:	b2ad      	uxth	r5, r5
    6faa:	9302      	str	r3, [sp, #8]
    6fac:	b2a4      	uxth	r4, r4
    6fae:	0028      	movs	r0, r5
    6fb0:	6833      	ldr	r3, [r6, #0]
    6fb2:	2100      	movs	r1, #0
    6fb4:	4798      	blx	r3
    6fb6:	9b02      	ldr	r3, [sp, #8]
    6fb8:	3501      	adds	r5, #1
    6fba:	7018      	strb	r0, [r3, #0]
    6fbc:	b2ad      	uxth	r5, r5
    6fbe:	3301      	adds	r3, #1
    6fc0:	9302      	str	r3, [sp, #8]
    6fc2:	42a5      	cmp	r5, r4
    6fc4:	d1f3      	bne.n	6fae <OnARP_8730A_ROM+0x106>
    6fc6:	9901      	ldr	r1, [sp, #4]
    6fc8:	ad04      	add	r5, sp, #16
    6fca:	311a      	adds	r1, #26
    6fcc:	b28b      	uxth	r3, r1
    6fce:	9301      	str	r3, [sp, #4]
    6fd0:	0020      	movs	r0, r4
    6fd2:	6833      	ldr	r3, [r6, #0]
    6fd4:	2100      	movs	r1, #0
    6fd6:	4798      	blx	r3
    6fd8:	3401      	adds	r4, #1
    6fda:	9b01      	ldr	r3, [sp, #4]
    6fdc:	b2a4      	uxth	r4, r4
    6fde:	7028      	strb	r0, [r5, #0]
    6fe0:	3501      	adds	r5, #1
    6fe2:	429c      	cmp	r4, r3
    6fe4:	d1f4      	bne.n	6fd0 <OnARP_8730A_ROM+0x128>
    6fe6:	4b19      	ldr	r3, [pc, #100]	; (704c <OnARP_8730A_ROM+0x1a4>)
    6fe8:	681b      	ldr	r3, [r3, #0]
    6fea:	7959      	ldrb	r1, [r3, #5]
    6fec:	2901      	cmp	r1, #1
    6fee:	d105      	bne.n	6ffc <OnARP_8730A_ROM+0x154>
    6ff0:	7a1b      	ldrb	r3, [r3, #8]
    6ff2:	b91b      	cbnz	r3, 6ffc <OnARP_8730A_ROM+0x154>
    6ff4:	4b16      	ldr	r3, [pc, #88]	; (7050 <OnARP_8730A_ROM+0x1a8>)
    6ff6:	2004      	movs	r0, #4
    6ff8:	681b      	ldr	r3, [r3, #0]
    6ffa:	4798      	blx	r3
    6ffc:	4b15      	ldr	r3, [pc, #84]	; (7054 <OnARP_8730A_ROM+0x1ac>)
    6ffe:	a908      	add	r1, sp, #32
    7000:	681c      	ldr	r4, [r3, #0]
    7002:	2300      	movs	r3, #0
    7004:	a804      	add	r0, sp, #16
    7006:	001a      	movs	r2, r3
    7008:	47a0      	blx	r4
    700a:	6839      	ldr	r1, [r7, #0]
    700c:	2201      	movs	r2, #1
    700e:	7a4b      	ldrb	r3, [r1, #9]
    7010:	0018      	movs	r0, r3
    7012:	4010      	ands	r0, r2
    7014:	4213      	tst	r3, r2
    7016:	d100      	bne.n	701a <OnARP_8730A_ROM+0x172>
    7018:	e77e      	b.n	6f18 <OnARP_8730A_ROM+0x70>
    701a:	4393      	bics	r3, r2
    701c:	e7ad      	b.n	6f7a <OnARP_8730A_ROM+0xd2>
    701e:	2202      	movs	r2, #2
    7020:	780b      	ldrb	r3, [r1, #0]
    7022:	4393      	bics	r3, r2
    7024:	001a      	movs	r2, r3
    7026:	2305      	movs	r3, #5
    7028:	4313      	orrs	r3, r2
    702a:	700b      	strb	r3, [r1, #0]
    702c:	e774      	b.n	6f18 <OnARP_8730A_ROM+0x70>
    702e:	46c0      	nop			; (mov r8, r8)
    7030:	230007e0 	.word	0x230007e0
    7034:	230009bc 	.word	0x230009bc
    7038:	00004b19 	.word	0x00004b19
    703c:	230009ec 	.word	0x230009ec
    7040:	4300006a 	.word	0x4300006a
    7044:	430001c7 	.word	0x430001c7
    7048:	230009d0 	.word	0x230009d0
    704c:	23000a28 	.word	0x23000a28
    7050:	23000958 	.word	0x23000958
    7054:	23000928 	.word	0x23000928

00007058 <GetNDPInfo_8730A_ROM>:
    7058:	4b05      	ldr	r3, [pc, #20]	; (7070 <GetNDPInfo_8730A_ROM+0x18>)
    705a:	01c1      	lsls	r1, r0, #7
    705c:	b510      	push	{r4, lr}
    705e:	18c9      	adds	r1, r1, r3
    7060:	4b04      	ldr	r3, [pc, #16]	; (7074 <GetNDPInfo_8730A_ROM+0x1c>)
    7062:	2270      	movs	r2, #112	; 0x70
    7064:	6818      	ldr	r0, [r3, #0]
    7066:	4b04      	ldr	r3, [pc, #16]	; (7078 <GetNDPInfo_8730A_ROM+0x20>)
    7068:	3014      	adds	r0, #20
    706a:	4798      	blx	r3
    706c:	bd10      	pop	{r4, pc}
    706e:	46c0      	nop			; (mov r8, r8)
    7070:	43020000 	.word	0x43020000
    7074:	230009ec 	.word	0x230009ec
    7078:	00004b5d 	.word	0x00004b5d

0000707c <IsIPV6_8730A_ROM>:
    707c:	1840      	adds	r0, r0, r1
    707e:	b570      	push	{r4, r5, r6, lr}
    7080:	4d0a      	ldr	r5, [pc, #40]	; (70ac <IsIPV6_8730A_ROM+0x30>)
    7082:	b284      	uxth	r4, r0
    7084:	1da0      	adds	r0, r4, #6
    7086:	2100      	movs	r1, #0
    7088:	682b      	ldr	r3, [r5, #0]
    708a:	b280      	uxth	r0, r0
    708c:	4798      	blx	r3
    708e:	2100      	movs	r1, #0
    7090:	2886      	cmp	r0, #134	; 0x86
    7092:	d108      	bne.n	70a6 <IsIPV6_8730A_ROM+0x2a>
    7094:	3407      	adds	r4, #7
    7096:	682b      	ldr	r3, [r5, #0]
    7098:	b2a0      	uxth	r0, r4
    709a:	4798      	blx	r3
    709c:	0001      	movs	r1, r0
    709e:	39dd      	subs	r1, #221	; 0xdd
    70a0:	424b      	negs	r3, r1
    70a2:	4159      	adcs	r1, r3
    70a4:	b2c9      	uxtb	r1, r1
    70a6:	0008      	movs	r0, r1
    70a8:	bd70      	pop	{r4, r5, r6, pc}
    70aa:	46c0      	nop			; (mov r8, r8)
    70ac:	230007e0 	.word	0x230007e0

000070b0 <OnNS_8730A_ROM>:
    70b0:	b5f0      	push	{r4, r5, r6, r7, lr}
    70b2:	4b6d      	ldr	r3, [pc, #436]	; (7268 <OnNS_8730A_ROM+0x1b8>)
    70b4:	b097      	sub	sp, #92	; 0x5c
    70b6:	681b      	ldr	r3, [r3, #0]
    70b8:	0004      	movs	r4, r0
    70ba:	000d      	movs	r5, r1
    70bc:	4798      	blx	r3
    70be:	9007      	str	r0, [sp, #28]
    70c0:	2801      	cmp	r0, #1
    70c2:	d004      	beq.n	70ce <OnNS_8730A_ROM+0x1e>
    70c4:	2300      	movs	r3, #0
    70c6:	9307      	str	r3, [sp, #28]
    70c8:	9807      	ldr	r0, [sp, #28]
    70ca:	b017      	add	sp, #92	; 0x5c
    70cc:	bdf0      	pop	{r4, r5, r6, r7, pc}
    70ce:	1964      	adds	r4, r4, r5
    70d0:	b2a3      	uxth	r3, r4
    70d2:	0018      	movs	r0, r3
    70d4:	4d65      	ldr	r5, [pc, #404]	; (726c <OnNS_8730A_ROM+0x1bc>)
    70d6:	300e      	adds	r0, #14
    70d8:	9302      	str	r3, [sp, #8]
    70da:	2100      	movs	r1, #0
    70dc:	682b      	ldr	r3, [r5, #0]
    70de:	b280      	uxth	r0, r0
    70e0:	4798      	blx	r3
    70e2:	283a      	cmp	r0, #58	; 0x3a
    70e4:	d1ee      	bne.n	70c4 <OnNS_8730A_ROM+0x14>
    70e6:	9802      	ldr	r0, [sp, #8]
    70e8:	2100      	movs	r1, #0
    70ea:	3030      	adds	r0, #48	; 0x30
    70ec:	682b      	ldr	r3, [r5, #0]
    70ee:	b280      	uxth	r0, r0
    70f0:	4798      	blx	r3
    70f2:	2887      	cmp	r0, #135	; 0x87
    70f4:	d1e6      	bne.n	70c4 <OnNS_8730A_ROM+0x14>
    70f6:	9c02      	ldr	r4, [sp, #8]
    70f8:	9e02      	ldr	r6, [sp, #8]
    70fa:	3438      	adds	r4, #56	; 0x38
    70fc:	3648      	adds	r6, #72	; 0x48
    70fe:	b2a4      	uxth	r4, r4
    7100:	af12      	add	r7, sp, #72	; 0x48
    7102:	b2b6      	uxth	r6, r6
    7104:	0020      	movs	r0, r4
    7106:	2100      	movs	r1, #0
    7108:	682b      	ldr	r3, [r5, #0]
    710a:	4798      	blx	r3
    710c:	3401      	adds	r4, #1
    710e:	b2a4      	uxth	r4, r4
    7110:	7038      	strb	r0, [r7, #0]
    7112:	3701      	adds	r7, #1
    7114:	42b4      	cmp	r4, r6
    7116:	d1f5      	bne.n	7104 <OnNS_8730A_ROM+0x54>
    7118:	2300      	movs	r3, #0
    711a:	9306      	str	r3, [sp, #24]
    711c:	2338      	movs	r3, #56	; 0x38
    711e:	9a06      	ldr	r2, [sp, #24]
    7120:	4f53      	ldr	r7, [pc, #332]	; (7270 <OnNS_8730A_ROM+0x1c0>)
    7122:	4353      	muls	r3, r2
    7124:	9303      	str	r3, [sp, #12]
    7126:	9a03      	ldr	r2, [sp, #12]
    7128:	683b      	ldr	r3, [r7, #0]
    712a:	189b      	adds	r3, r3, r2
    712c:	7d1b      	ldrb	r3, [r3, #20]
    712e:	07db      	lsls	r3, r3, #31
    7130:	d5ca      	bpl.n	70c8 <OnNS_8730A_ROM+0x18>
    7132:	0013      	movs	r3, r2
    7134:	3316      	adds	r3, #22
    7136:	9308      	str	r3, [sp, #32]
    7138:	3306      	adds	r3, #6
    713a:	930a      	str	r3, [sp, #40]	; 0x28
    713c:	2300      	movs	r3, #0
    713e:	9209      	str	r2, [sp, #36]	; 0x24
    7140:	9304      	str	r3, [sp, #16]
    7142:	683a      	ldr	r2, [r7, #0]
    7144:	9b09      	ldr	r3, [sp, #36]	; 0x24
    7146:	9904      	ldr	r1, [sp, #16]
    7148:	18d3      	adds	r3, r2, r3
    714a:	7d5b      	ldrb	r3, [r3, #21]
    714c:	428b      	cmp	r3, r1
    714e:	d804      	bhi.n	715a <OnNS_8730A_ROM+0xaa>
    7150:	9b06      	ldr	r3, [sp, #24]
    7152:	2b01      	cmp	r3, #1
    7154:	d0b8      	beq.n	70c8 <OnNS_8730A_ROM+0x18>
    7156:	2301      	movs	r3, #1
    7158:	e7df      	b.n	711a <OnNS_8730A_ROM+0x6a>
    715a:	9b04      	ldr	r3, [sp, #16]
    715c:	9803      	ldr	r0, [sp, #12]
    715e:	011b      	lsls	r3, r3, #4
    7160:	302c      	adds	r0, #44	; 0x2c
    7162:	1819      	adds	r1, r3, r0
    7164:	4c43      	ldr	r4, [pc, #268]	; (7274 <OnNS_8730A_ROM+0x1c4>)
    7166:	1851      	adds	r1, r2, r1
    7168:	a812      	add	r0, sp, #72	; 0x48
    716a:	2210      	movs	r2, #16
    716c:	47a0      	blx	r4
    716e:	4b42      	ldr	r3, [pc, #264]	; (7278 <OnNS_8730A_ROM+0x1c8>)
    7170:	940b      	str	r4, [sp, #44]	; 0x2c
    7172:	781a      	ldrb	r2, [r3, #0]
    7174:	2800      	cmp	r0, #0
    7176:	d172      	bne.n	725e <OnNS_8730A_ROM+0x1ae>
    7178:	2120      	movs	r1, #32
    717a:	438a      	bics	r2, r1
    717c:	0011      	movs	r1, r2
    717e:	2208      	movs	r2, #8
    7180:	430a      	orrs	r2, r1
    7182:	701a      	strb	r2, [r3, #0]
    7184:	9c02      	ldr	r4, [sp, #8]
    7186:	9e02      	ldr	r6, [sp, #8]
    7188:	344a      	adds	r4, #74	; 0x4a
    718a:	ab0c      	add	r3, sp, #48	; 0x30
    718c:	3650      	adds	r6, #80	; 0x50
    718e:	b2a4      	uxth	r4, r4
    7190:	9305      	str	r3, [sp, #20]
    7192:	b2b6      	uxth	r6, r6
    7194:	0020      	movs	r0, r4
    7196:	682b      	ldr	r3, [r5, #0]
    7198:	2100      	movs	r1, #0
    719a:	4798      	blx	r3
    719c:	9b05      	ldr	r3, [sp, #20]
    719e:	3401      	adds	r4, #1
    71a0:	7018      	strb	r0, [r3, #0]
    71a2:	b2a4      	uxth	r4, r4
    71a4:	3301      	adds	r3, #1
    71a6:	9305      	str	r3, [sp, #20]
    71a8:	42b4      	cmp	r4, r6
    71aa:	d1f3      	bne.n	7194 <OnNS_8730A_ROM+0xe4>
    71ac:	9c02      	ldr	r4, [sp, #8]
    71ae:	9e02      	ldr	r6, [sp, #8]
    71b0:	3410      	adds	r4, #16
    71b2:	ab0e      	add	r3, sp, #56	; 0x38
    71b4:	3620      	adds	r6, #32
    71b6:	b2a4      	uxth	r4, r4
    71b8:	9305      	str	r3, [sp, #20]
    71ba:	b2b6      	uxth	r6, r6
    71bc:	0020      	movs	r0, r4
    71be:	682b      	ldr	r3, [r5, #0]
    71c0:	2100      	movs	r1, #0
    71c2:	4798      	blx	r3
    71c4:	9b05      	ldr	r3, [sp, #20]
    71c6:	3401      	adds	r4, #1
    71c8:	7018      	strb	r0, [r3, #0]
    71ca:	b2a4      	uxth	r4, r4
    71cc:	3301      	adds	r3, #1
    71ce:	9305      	str	r3, [sp, #20]
    71d0:	42a6      	cmp	r6, r4
    71d2:	d1f3      	bne.n	71bc <OnNS_8730A_ROM+0x10c>
    71d4:	683b      	ldr	r3, [r7, #0]
    71d6:	9a09      	ldr	r2, [sp, #36]	; 0x24
    71d8:	189a      	adds	r2, r3, r2
    71da:	7d12      	ldrb	r2, [r2, #20]
    71dc:	0792      	lsls	r2, r2, #30
    71de:	0fd4      	lsrs	r4, r2, #31
    71e0:	2a00      	cmp	r2, #0
    71e2:	da29      	bge.n	7238 <OnNS_8730A_ROM+0x188>
    71e4:	9a0a      	ldr	r2, [sp, #40]	; 0x28
    71e6:	a80e      	add	r0, sp, #56	; 0x38
    71e8:	1899      	adds	r1, r3, r2
    71ea:	2210      	movs	r2, #16
    71ec:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
    71ee:	4798      	blx	r3
    71f0:	b9e8      	cbnz	r0, 722e <OnNS_8730A_ROM+0x17e>
    71f2:	4b22      	ldr	r3, [pc, #136]	; (727c <OnNS_8730A_ROM+0x1cc>)
    71f4:	681b      	ldr	r3, [r3, #0]
    71f6:	781b      	ldrb	r3, [r3, #0]
    71f8:	2b02      	cmp	r3, #2
    71fa:	d10c      	bne.n	7216 <OnNS_8730A_ROM+0x166>
    71fc:	4b20      	ldr	r3, [pc, #128]	; (7280 <OnNS_8730A_ROM+0x1d0>)
    71fe:	681a      	ldr	r2, [r3, #0]
    7200:	7852      	ldrb	r2, [r2, #1]
    7202:	0712      	lsls	r2, r2, #28
    7204:	d407      	bmi.n	7216 <OnNS_8730A_ROM+0x166>
    7206:	210a      	movs	r1, #10
    7208:	4a1e      	ldr	r2, [pc, #120]	; (7284 <OnNS_8730A_ROM+0x1d4>)
    720a:	7011      	strb	r1, [r2, #0]
    720c:	681b      	ldr	r3, [r3, #0]
    720e:	7a5a      	ldrb	r2, [r3, #9]
    7210:	4314      	orrs	r4, r2
    7212:	725c      	strb	r4, [r3, #9]
    7214:	e758      	b.n	70c8 <OnNS_8730A_ROM+0x18>
    7216:	683b      	ldr	r3, [r7, #0]
    7218:	9a08      	ldr	r2, [sp, #32]
    721a:	a90c      	add	r1, sp, #48	; 0x30
    721c:	189b      	adds	r3, r3, r2
    721e:	2200      	movs	r2, #0
    7220:	9201      	str	r2, [sp, #4]
    7222:	9200      	str	r2, [sp, #0]
    7224:	4a18      	ldr	r2, [pc, #96]	; (7288 <OnNS_8730A_ROM+0x1d8>)
    7226:	a80e      	add	r0, sp, #56	; 0x38
    7228:	6814      	ldr	r4, [r2, #0]
    722a:	aa12      	add	r2, sp, #72	; 0x48
    722c:	47a0      	blx	r4
    722e:	9b04      	ldr	r3, [sp, #16]
    7230:	3301      	adds	r3, #1
    7232:	b2db      	uxtb	r3, r3
    7234:	9304      	str	r3, [sp, #16]
    7236:	e784      	b.n	7142 <OnNS_8730A_ROM+0x92>
    7238:	4a10      	ldr	r2, [pc, #64]	; (727c <OnNS_8730A_ROM+0x1cc>)
    723a:	6812      	ldr	r2, [r2, #0]
    723c:	7812      	ldrb	r2, [r2, #0]
    723e:	2a02      	cmp	r2, #2
    7240:	d1ea      	bne.n	7218 <OnNS_8730A_ROM+0x168>
    7242:	4a0f      	ldr	r2, [pc, #60]	; (7280 <OnNS_8730A_ROM+0x1d0>)
    7244:	6811      	ldr	r1, [r2, #0]
    7246:	7849      	ldrb	r1, [r1, #1]
    7248:	0709      	lsls	r1, r1, #28
    724a:	d4e5      	bmi.n	7218 <OnNS_8730A_ROM+0x168>
    724c:	210a      	movs	r1, #10
    724e:	4b0d      	ldr	r3, [pc, #52]	; (7284 <OnNS_8730A_ROM+0x1d4>)
    7250:	7019      	strb	r1, [r3, #0]
    7252:	2301      	movs	r3, #1
    7254:	6812      	ldr	r2, [r2, #0]
    7256:	7a51      	ldrb	r1, [r2, #9]
    7258:	430b      	orrs	r3, r1
    725a:	7253      	strb	r3, [r2, #9]
    725c:	e734      	b.n	70c8 <OnNS_8730A_ROM+0x18>
    725e:	2128      	movs	r1, #40	; 0x28
    7260:	430a      	orrs	r2, r1
    7262:	701a      	strb	r2, [r3, #0]
    7264:	e7e3      	b.n	722e <OnNS_8730A_ROM+0x17e>
    7266:	46c0      	nop			; (mov r8, r8)
    7268:	23000870 	.word	0x23000870
    726c:	230007e0 	.word	0x230007e0
    7270:	230009ec 	.word	0x230009ec
    7274:	00004b19 	.word	0x00004b19
    7278:	4300006d 	.word	0x4300006d
    727c:	230009d0 	.word	0x230009d0
    7280:	230009bc 	.word	0x230009bc
    7284:	430001c7 	.word	0x430001c7
    7288:	23000924 	.word	0x23000924

0000728c <Ipv4Match_8730A_ROM>:
    728c:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    728e:	2500      	movs	r5, #0
    7290:	1840      	adds	r0, r0, r1
    7292:	4f17      	ldr	r7, [pc, #92]	; (72f0 <Ipv4Match_8730A_ROM+0x64>)
    7294:	b284      	uxth	r4, r0
    7296:	1da0      	adds	r0, r4, #6
    7298:	0029      	movs	r1, r5
    729a:	683b      	ldr	r3, [r7, #0]
    729c:	9501      	str	r5, [sp, #4]
    729e:	b280      	uxth	r0, r0
    72a0:	4798      	blx	r3
    72a2:	2808      	cmp	r0, #8
    72a4:	d001      	beq.n	72aa <Ipv4Match_8730A_ROM+0x1e>
    72a6:	2000      	movs	r0, #0
    72a8:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    72aa:	1de0      	adds	r0, r4, #7
    72ac:	0029      	movs	r1, r5
    72ae:	683b      	ldr	r3, [r7, #0]
    72b0:	b280      	uxth	r0, r0
    72b2:	4798      	blx	r3
    72b4:	2800      	cmp	r0, #0
    72b6:	d1f6      	bne.n	72a6 <Ipv4Match_8730A_ROM+0x1a>
    72b8:	0025      	movs	r5, r4
    72ba:	341c      	adds	r4, #28
    72bc:	3518      	adds	r5, #24
    72be:	b2ad      	uxth	r5, r5
    72c0:	ae01      	add	r6, sp, #4
    72c2:	b2a4      	uxth	r4, r4
    72c4:	0028      	movs	r0, r5
    72c6:	2100      	movs	r1, #0
    72c8:	683b      	ldr	r3, [r7, #0]
    72ca:	4798      	blx	r3
    72cc:	3501      	adds	r5, #1
    72ce:	b2ad      	uxth	r5, r5
    72d0:	7030      	strb	r0, [r6, #0]
    72d2:	3601      	adds	r6, #1
    72d4:	42a5      	cmp	r5, r4
    72d6:	d1f5      	bne.n	72c4 <Ipv4Match_8730A_ROM+0x38>
    72d8:	4b06      	ldr	r3, [pc, #24]	; (72f4 <Ipv4Match_8730A_ROM+0x68>)
    72da:	2204      	movs	r2, #4
    72dc:	6819      	ldr	r1, [r3, #0]
    72de:	a801      	add	r0, sp, #4
    72e0:	4b05      	ldr	r3, [pc, #20]	; (72f8 <Ipv4Match_8730A_ROM+0x6c>)
    72e2:	3106      	adds	r1, #6
    72e4:	4798      	blx	r3
    72e6:	4243      	negs	r3, r0
    72e8:	4158      	adcs	r0, r3
    72ea:	b2c0      	uxtb	r0, r0
    72ec:	e7dc      	b.n	72a8 <Ipv4Match_8730A_ROM+0x1c>
    72ee:	46c0      	nop			; (mov r8, r8)
    72f0:	230007e0 	.word	0x230007e0
    72f4:	230009ec 	.word	0x230009ec
    72f8:	00004b19 	.word	0x00004b19

000072fc <MatchUnicastFilter_8730A_ROM>:
    72fc:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
    72fe:	4b29      	ldr	r3, [pc, #164]	; (73a4 <MatchUnicastFilter_8730A_ROM+0xa8>)
    7300:	0004      	movs	r4, r0
    7302:	681b      	ldr	r3, [r3, #0]
    7304:	000d      	movs	r5, r1
    7306:	4798      	blx	r3
    7308:	b910      	cbnz	r0, 7310 <MatchUnicastFilter_8730A_ROM+0x14>
    730a:	2300      	movs	r3, #0
    730c:	0018      	movs	r0, r3
    730e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
    7310:	4b25      	ldr	r3, [pc, #148]	; (73a8 <MatchUnicastFilter_8730A_ROM+0xac>)
    7312:	0029      	movs	r1, r5
    7314:	0020      	movs	r0, r4
    7316:	681b      	ldr	r3, [r3, #0]
    7318:	4798      	blx	r3
    731a:	2800      	cmp	r0, #0
    731c:	d0f5      	beq.n	730a <MatchUnicastFilter_8730A_ROM+0xe>
    731e:	1964      	adds	r4, r4, r5
    7320:	b2a4      	uxth	r4, r4
    7322:	0026      	movs	r6, r4
    7324:	4d21      	ldr	r5, [pc, #132]	; (73ac <MatchUnicastFilter_8730A_ROM+0xb0>)
    7326:	3608      	adds	r6, #8
    7328:	682b      	ldr	r3, [r5, #0]
    732a:	b2b6      	uxth	r6, r6
    732c:	785b      	ldrb	r3, [r3, #1]
    732e:	06db      	lsls	r3, r3, #27
    7330:	d41c      	bmi.n	736c <MatchUnicastFilter_8730A_ROM+0x70>
    7332:	682b      	ldr	r3, [r5, #0]
    7334:	785b      	ldrb	r3, [r3, #1]
    7336:	075b      	lsls	r3, r3, #29
    7338:	d5e7      	bpl.n	730a <MatchUnicastFilter_8730A_ROM+0xe>
    733a:	4d1d      	ldr	r5, [pc, #116]	; (73b0 <MatchUnicastFilter_8730A_ROM+0xb4>)
    733c:	2109      	movs	r1, #9
    733e:	0030      	movs	r0, r6
    7340:	682b      	ldr	r3, [r5, #0]
    7342:	4798      	blx	r3
    7344:	2811      	cmp	r0, #17
    7346:	d1e0      	bne.n	730a <MatchUnicastFilter_8730A_ROM+0xe>
    7348:	341c      	adds	r4, #28
    734a:	b2a4      	uxth	r4, r4
    734c:	2102      	movs	r1, #2
    734e:	0020      	movs	r0, r4
    7350:	682b      	ldr	r3, [r5, #0]
    7352:	4798      	blx	r3
    7354:	2800      	cmp	r0, #0
    7356:	d1d8      	bne.n	730a <MatchUnicastFilter_8730A_ROM+0xe>
    7358:	682b      	ldr	r3, [r5, #0]
    735a:	2103      	movs	r1, #3
    735c:	0020      	movs	r0, r4
    735e:	4798      	blx	r3
    7360:	0003      	movs	r3, r0
    7362:	3b89      	subs	r3, #137	; 0x89
    7364:	425a      	negs	r2, r3
    7366:	4153      	adcs	r3, r2
    7368:	b2db      	uxtb	r3, r3
    736a:	e7cf      	b.n	730c <MatchUnicastFilter_8730A_ROM+0x10>
    736c:	4f10      	ldr	r7, [pc, #64]	; (73b0 <MatchUnicastFilter_8730A_ROM+0xb4>)
    736e:	2109      	movs	r1, #9
    7370:	0030      	movs	r0, r6
    7372:	683b      	ldr	r3, [r7, #0]
    7374:	4798      	blx	r3
    7376:	2806      	cmp	r0, #6
    7378:	d1db      	bne.n	7332 <MatchUnicastFilter_8730A_ROM+0x36>
    737a:	2114      	movs	r1, #20
    737c:	0030      	movs	r0, r6
    737e:	683b      	ldr	r3, [r7, #0]
    7380:	4798      	blx	r3
    7382:	2800      	cmp	r0, #0
    7384:	d1d5      	bne.n	7332 <MatchUnicastFilter_8730A_ROM+0x36>
    7386:	2115      	movs	r1, #21
    7388:	0030      	movs	r0, r6
    738a:	683b      	ldr	r3, [r7, #0]
    738c:	4798      	blx	r3
    738e:	2850      	cmp	r0, #80	; 0x50
    7390:	d1cf      	bne.n	7332 <MatchUnicastFilter_8730A_ROM+0x36>
    7392:	683b      	ldr	r3, [r7, #0]
    7394:	2121      	movs	r1, #33	; 0x21
    7396:	0030      	movs	r0, r6
    7398:	4798      	blx	r3
    739a:	2205      	movs	r2, #5
    739c:	2301      	movs	r3, #1
    739e:	4210      	tst	r0, r2
    73a0:	d0c7      	beq.n	7332 <MatchUnicastFilter_8730A_ROM+0x36>
    73a2:	e7b3      	b.n	730c <MatchUnicastFilter_8730A_ROM+0x10>
    73a4:	23000858 	.word	0x23000858
    73a8:	23000878 	.word	0x23000878
    73ac:	230009bc 	.word	0x230009bc
    73b0:	230007e0 	.word	0x230007e0

000073b4 <WakeUpHostDecision_8730A_ROM>:
    73b4:	0003      	movs	r3, r0
    73b6:	2202      	movs	r2, #2
    73b8:	0008      	movs	r0, r1
    73ba:	b530      	push	{r4, r5, lr}
    73bc:	4010      	ands	r0, r2
    73be:	4211      	tst	r1, r2
    73c0:	d00b      	beq.n	73da <WakeUpHostDecision_8730A_ROM+0x26>
    73c2:	f248 0400 	movw	r4, #32768	; 0x8000
    73c6:	4a11      	ldr	r2, [pc, #68]	; (740c <WakeUpHostDecision_8730A_ROM+0x58>)
    73c8:	2000      	movs	r0, #0
    73ca:	6815      	ldr	r5, [r2, #0]
    73cc:	4225      	tst	r5, r4
    73ce:	d004      	beq.n	73da <WakeUpHostDecision_8730A_ROM+0x26>
    73d0:	6014      	str	r4, [r2, #0]
    73d2:	4a0f      	ldr	r2, [pc, #60]	; (7410 <WakeUpHostDecision_8730A_ROM+0x5c>)
    73d4:	3023      	adds	r0, #35	; 0x23
    73d6:	7010      	strb	r0, [r2, #0]
    73d8:	3822      	subs	r0, #34	; 0x22
    73da:	2401      	movs	r4, #1
    73dc:	000a      	movs	r2, r1
    73de:	4022      	ands	r2, r4
    73e0:	4221      	tst	r1, r4
    73e2:	d012      	beq.n	740a <WakeUpHostDecision_8730A_ROM+0x56>
    73e4:	0659      	lsls	r1, r3, #25
    73e6:	d503      	bpl.n	73f0 <WakeUpHostDecision_8730A_ROM+0x3c>
    73e8:	2022      	movs	r0, #34	; 0x22
    73ea:	4909      	ldr	r1, [pc, #36]	; (7410 <WakeUpHostDecision_8730A_ROM+0x5c>)
    73ec:	7008      	strb	r0, [r1, #0]
    73ee:	0010      	movs	r0, r2
    73f0:	b259      	sxtb	r1, r3
    73f2:	2900      	cmp	r1, #0
    73f4:	da03      	bge.n	73fe <WakeUpHostDecision_8730A_ROM+0x4a>
    73f6:	2021      	movs	r0, #33	; 0x21
    73f8:	4905      	ldr	r1, [pc, #20]	; (7410 <WakeUpHostDecision_8730A_ROM+0x5c>)
    73fa:	7008      	strb	r0, [r1, #0]
    73fc:	0010      	movs	r0, r2
    73fe:	069b      	lsls	r3, r3, #26
    7400:	d503      	bpl.n	740a <WakeUpHostDecision_8730A_ROM+0x56>
    7402:	2123      	movs	r1, #35	; 0x23
    7404:	0010      	movs	r0, r2
    7406:	4b02      	ldr	r3, [pc, #8]	; (7410 <WakeUpHostDecision_8730A_ROM+0x5c>)
    7408:	7019      	strb	r1, [r3, #0]
    740a:	bd30      	pop	{r4, r5, pc}
    740c:	43000124 	.word	0x43000124
    7410:	430001c7 	.word	0x430001c7

00007414 <CheckIV_8730A_ROM>:
    7414:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    7416:	000e      	movs	r6, r1
    7418:	4b42      	ldr	r3, [pc, #264]	; (7524 <CheckIV_8730A_ROM+0x110>)
    741a:	0004      	movs	r4, r0
    741c:	0017      	movs	r7, r2
    741e:	2100      	movs	r1, #0
    7420:	2208      	movs	r2, #8
    7422:	4668      	mov	r0, sp
    7424:	4798      	blx	r3
    7426:	0035      	movs	r5, r6
    7428:	2301      	movs	r3, #1
    742a:	401d      	ands	r5, r3
    742c:	421e      	tst	r6, r3
    742e:	d113      	bne.n	7458 <CheckIV_8730A_ROM+0x44>
    7430:	429f      	cmp	r7, r3
    7432:	d002      	beq.n	743a <CheckIV_8730A_ROM+0x26>
    7434:	2400      	movs	r4, #0
    7436:	0020      	movs	r0, r4
    7438:	bdfe      	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
    743a:	4b3b      	ldr	r3, [pc, #236]	; (7528 <CheckIV_8730A_ROM+0x114>)
    743c:	6819      	ldr	r1, [r3, #0]
    743e:	4b3b      	ldr	r3, [pc, #236]	; (752c <CheckIV_8730A_ROM+0x118>)
    7440:	681b      	ldr	r3, [r3, #0]
    7442:	781a      	ldrb	r2, [r3, #0]
    7444:	f240 2301 	movw	r3, #513	; 0x201
    7448:	54ca      	strb	r2, [r1, r3]
    744a:	3307      	adds	r3, #7
    744c:	18c9      	adds	r1, r1, r3
    744e:	2208      	movs	r2, #8
    7450:	4668      	mov	r0, sp
    7452:	4b37      	ldr	r3, [pc, #220]	; (7530 <CheckIV_8730A_ROM+0x11c>)
    7454:	4798      	blx	r3
    7456:	e017      	b.n	7488 <CheckIV_8730A_ROM+0x74>
    7458:	4b33      	ldr	r3, [pc, #204]	; (7528 <CheckIV_8730A_ROM+0x114>)
    745a:	6819      	ldr	r1, [r3, #0]
    745c:	4b33      	ldr	r3, [pc, #204]	; (752c <CheckIV_8730A_ROM+0x118>)
    745e:	681b      	ldr	r3, [r3, #0]
    7460:	785a      	ldrb	r2, [r3, #1]
    7462:	f240 2301 	movw	r3, #513	; 0x201
    7466:	54ca      	strb	r2, [r1, r3]
    7468:	3b01      	subs	r3, #1
    746a:	5ccb      	ldrb	r3, [r1, r3]
    746c:	2b02      	cmp	r3, #2
    746e:	d01c      	beq.n	74aa <CheckIV_8730A_ROM+0x96>
    7470:	d808      	bhi.n	7484 <CheckIV_8730A_ROM+0x70>
    7472:	4e2f      	ldr	r6, [pc, #188]	; (7530 <CheckIV_8730A_ROM+0x11c>)
    7474:	b9b3      	cbnz	r3, 74a4 <CheckIV_8730A_ROM+0x90>
    7476:	f240 2310 	movw	r3, #528	; 0x210
    747a:	2208      	movs	r2, #8
    747c:	4668      	mov	r0, sp
    747e:	18c9      	adds	r1, r1, r3
    7480:	47b0      	blx	r6
    7482:	e001      	b.n	7488 <CheckIV_8730A_ROM+0x74>
    7484:	2b03      	cmp	r3, #3
    7486:	d013      	beq.n	74b0 <CheckIV_8730A_ROM+0x9c>
    7488:	f240 2301 	movw	r3, #513	; 0x201
    748c:	4e26      	ldr	r6, [pc, #152]	; (7528 <CheckIV_8730A_ROM+0x114>)
    748e:	6832      	ldr	r2, [r6, #0]
    7490:	5cd2      	ldrb	r2, [r2, r3]
    7492:	2a04      	cmp	r2, #4
    7494:	d024      	beq.n	74e0 <CheckIV_8730A_ROM+0xcc>
    7496:	d80e      	bhi.n	74b6 <CheckIV_8730A_ROM+0xa2>
    7498:	2a01      	cmp	r2, #1
    749a:	d00f      	beq.n	74bc <CheckIV_8730A_ROM+0xa8>
    749c:	2a02      	cmp	r2, #2
    749e:	d01f      	beq.n	74e0 <CheckIV_8730A_ROM+0xcc>
    74a0:	2401      	movs	r4, #1
    74a2:	e013      	b.n	74cc <CheckIV_8730A_ROM+0xb8>
    74a4:	f240 2318 	movw	r3, #536	; 0x218
    74a8:	e7e7      	b.n	747a <CheckIV_8730A_ROM+0x66>
    74aa:	f240 2320 	movw	r3, #544	; 0x220
    74ae:	e7cd      	b.n	744c <CheckIV_8730A_ROM+0x38>
    74b0:	f240 2328 	movw	r3, #552	; 0x228
    74b4:	e7ca      	b.n	744c <CheckIV_8730A_ROM+0x38>
    74b6:	3a05      	subs	r2, #5
    74b8:	2a01      	cmp	r2, #1
    74ba:	d8f1      	bhi.n	74a0 <CheckIV_8730A_ROM+0x8c>
    74bc:	4b1d      	ldr	r3, [pc, #116]	; (7534 <CheckIV_8730A_ROM+0x120>)
    74be:	0021      	movs	r1, r4
    74c0:	4668      	mov	r0, sp
    74c2:	681b      	ldr	r3, [r3, #0]
    74c4:	4798      	blx	r3
    74c6:	0004      	movs	r4, r0
    74c8:	2800      	cmp	r0, #0
    74ca:	d0b3      	beq.n	7434 <CheckIV_8730A_ROM+0x20>
    74cc:	6830      	ldr	r0, [r6, #0]
    74ce:	b96d      	cbnz	r5, 74ec <CheckIV_8730A_ROM+0xd8>
    74d0:	f240 2308 	movw	r3, #520	; 0x208
    74d4:	2208      	movs	r2, #8
    74d6:	4669      	mov	r1, sp
    74d8:	18c0      	adds	r0, r0, r3
    74da:	4b15      	ldr	r3, [pc, #84]	; (7530 <CheckIV_8730A_ROM+0x11c>)
    74dc:	4798      	blx	r3
    74de:	e7aa      	b.n	7436 <CheckIV_8730A_ROM+0x22>
    74e0:	4b15      	ldr	r3, [pc, #84]	; (7538 <CheckIV_8730A_ROM+0x124>)
    74e2:	0021      	movs	r1, r4
    74e4:	4668      	mov	r0, sp
    74e6:	681b      	ldr	r3, [r3, #0]
    74e8:	4798      	blx	r3
    74ea:	e7ec      	b.n	74c6 <CheckIV_8730A_ROM+0xb2>
    74ec:	f240 2200 	movw	r2, #512	; 0x200
    74f0:	5c82      	ldrb	r2, [r0, r2]
    74f2:	2a02      	cmp	r2, #2
    74f4:	d012      	beq.n	751c <CheckIV_8730A_ROM+0x108>
    74f6:	d808      	bhi.n	750a <CheckIV_8730A_ROM+0xf6>
    74f8:	4d0d      	ldr	r5, [pc, #52]	; (7530 <CheckIV_8730A_ROM+0x11c>)
    74fa:	b962      	cbnz	r2, 7516 <CheckIV_8730A_ROM+0x102>
    74fc:	f240 2310 	movw	r3, #528	; 0x210
    7500:	2208      	movs	r2, #8
    7502:	4669      	mov	r1, sp
    7504:	18c0      	adds	r0, r0, r3
    7506:	47a8      	blx	r5
    7508:	e795      	b.n	7436 <CheckIV_8730A_ROM+0x22>
    750a:	2a03      	cmp	r2, #3
    750c:	d000      	beq.n	7510 <CheckIV_8730A_ROM+0xfc>
    750e:	e792      	b.n	7436 <CheckIV_8730A_ROM+0x22>
    7510:	f240 2328 	movw	r3, #552	; 0x228
    7514:	e7de      	b.n	74d4 <CheckIV_8730A_ROM+0xc0>
    7516:	f240 2318 	movw	r3, #536	; 0x218
    751a:	e7f1      	b.n	7500 <CheckIV_8730A_ROM+0xec>
    751c:	f240 2320 	movw	r3, #544	; 0x220
    7520:	e7d8      	b.n	74d4 <CheckIV_8730A_ROM+0xc0>
    7522:	46c0      	nop			; (mov r8, r8)
    7524:	00004ca9 	.word	0x00004ca9
    7528:	230009ec 	.word	0x230009ec
    752c:	230009d0 	.word	0x230009d0
    7530:	00004b5d 	.word	0x00004b5d
    7534:	230008d4 	.word	0x230008d4
    7538:	230008d0 	.word	0x230008d0

0000753c <H2CHDL_AOACGlobalInfo_8730A_ROM>:
    753c:	4b03      	ldr	r3, [pc, #12]	; (754c <H2CHDL_AOACGlobalInfo_8730A_ROM+0x10>)
    753e:	7802      	ldrb	r2, [r0, #0]
    7540:	681b      	ldr	r3, [r3, #0]
    7542:	701a      	strb	r2, [r3, #0]
    7544:	7842      	ldrb	r2, [r0, #1]
    7546:	705a      	strb	r2, [r3, #1]
    7548:	4770      	bx	lr
    754a:	46c0      	nop			; (mov r8, r8)
    754c:	230009d0 	.word	0x230009d0

00007550 <H2CHDL_AOACRsvdpage1_8730A_ROM>:
    7550:	4b0c      	ldr	r3, [pc, #48]	; (7584 <H2CHDL_AOACRsvdpage1_8730A_ROM+0x34>)
    7552:	4a0d      	ldr	r2, [pc, #52]	; (7588 <H2CHDL_AOACRsvdpage1_8730A_ROM+0x38>)
    7554:	781b      	ldrb	r3, [r3, #0]
    7556:	7801      	ldrb	r1, [r0, #0]
    7558:	6812      	ldr	r2, [r2, #0]
    755a:	1859      	adds	r1, r3, r1
    755c:	7011      	strb	r1, [r2, #0]
    755e:	7841      	ldrb	r1, [r0, #1]
    7560:	1859      	adds	r1, r3, r1
    7562:	7051      	strb	r1, [r2, #1]
    7564:	7881      	ldrb	r1, [r0, #2]
    7566:	1859      	adds	r1, r3, r1
    7568:	7091      	strb	r1, [r2, #2]
    756a:	78c1      	ldrb	r1, [r0, #3]
    756c:	1859      	adds	r1, r3, r1
    756e:	70d1      	strb	r1, [r2, #3]
    7570:	7901      	ldrb	r1, [r0, #4]
    7572:	1859      	adds	r1, r3, r1
    7574:	7111      	strb	r1, [r2, #4]
    7576:	7941      	ldrb	r1, [r0, #5]
    7578:	1859      	adds	r1, r3, r1
    757a:	7151      	strb	r1, [r2, #5]
    757c:	7981      	ldrb	r1, [r0, #6]
    757e:	185b      	adds	r3, r3, r1
    7580:	7193      	strb	r3, [r2, #6]
    7582:	4770      	bx	lr
    7584:	43000209 	.word	0x43000209
    7588:	230009cc 	.word	0x230009cc

0000758c <H2CHDL_AOACRsvdpage2_8730A_ROM>:
    758c:	4b0c      	ldr	r3, [pc, #48]	; (75c0 <H2CHDL_AOACRsvdpage2_8730A_ROM+0x34>)
    758e:	4a0d      	ldr	r2, [pc, #52]	; (75c4 <H2CHDL_AOACRsvdpage2_8730A_ROM+0x38>)
    7590:	781b      	ldrb	r3, [r3, #0]
    7592:	7801      	ldrb	r1, [r0, #0]
    7594:	6812      	ldr	r2, [r2, #0]
    7596:	1859      	adds	r1, r3, r1
    7598:	7011      	strb	r1, [r2, #0]
    759a:	7841      	ldrb	r1, [r0, #1]
    759c:	1859      	adds	r1, r3, r1
    759e:	7051      	strb	r1, [r2, #1]
    75a0:	7881      	ldrb	r1, [r0, #2]
    75a2:	1859      	adds	r1, r3, r1
    75a4:	7091      	strb	r1, [r2, #2]
    75a6:	78c1      	ldrb	r1, [r0, #3]
    75a8:	1859      	adds	r1, r3, r1
    75aa:	70d1      	strb	r1, [r2, #3]
    75ac:	7901      	ldrb	r1, [r0, #4]
    75ae:	1859      	adds	r1, r3, r1
    75b0:	7111      	strb	r1, [r2, #4]
    75b2:	7941      	ldrb	r1, [r0, #5]
    75b4:	1859      	adds	r1, r3, r1
    75b6:	7151      	strb	r1, [r2, #5]
    75b8:	7981      	ldrb	r1, [r0, #6]
    75ba:	185b      	adds	r3, r3, r1
    75bc:	7193      	strb	r3, [r2, #6]
    75be:	4770      	bx	lr
    75c0:	43000209 	.word	0x43000209
    75c4:	230009c8 	.word	0x230009c8

000075c8 <H2CHDL_AOACRsvdpage3_8730A_ROM>:
    75c8:	4a05      	ldr	r2, [pc, #20]	; (75e0 <H2CHDL_AOACRsvdpage3_8730A_ROM+0x18>)
    75ca:	4b06      	ldr	r3, [pc, #24]	; (75e4 <H2CHDL_AOACRsvdpage3_8730A_ROM+0x1c>)
    75cc:	6811      	ldr	r1, [r2, #0]
    75ce:	781b      	ldrb	r3, [r3, #0]
    75d0:	7802      	ldrb	r2, [r0, #0]
    75d2:	189a      	adds	r2, r3, r2
    75d4:	700a      	strb	r2, [r1, #0]
    75d6:	7842      	ldrb	r2, [r0, #1]
    75d8:	189b      	adds	r3, r3, r2
    75da:	704b      	strb	r3, [r1, #1]
    75dc:	4770      	bx	lr
    75de:	46c0      	nop			; (mov r8, r8)
    75e0:	230009c4 	.word	0x230009c4
    75e4:	43000209 	.word	0x43000209

000075e8 <GetAddr_8730A_ROM>:
    75e8:	4b07      	ldr	r3, [pc, #28]	; (7608 <GetAddr_8730A_ROM+0x20>)
    75ea:	4a08      	ldr	r2, [pc, #32]	; (760c <GetAddr_8730A_ROM+0x24>)
    75ec:	681b      	ldr	r3, [r3, #0]
    75ee:	4908      	ldr	r1, [pc, #32]	; (7610 <GetAddr_8730A_ROM+0x28>)
    75f0:	330a      	adds	r3, #10
    75f2:	7810      	ldrb	r0, [r2, #0]
    75f4:	7198      	strb	r0, [r3, #6]
    75f6:	0010      	movs	r0, r2
    75f8:	3808      	subs	r0, #8
    75fa:	7800      	ldrb	r0, [r0, #0]
    75fc:	3201      	adds	r2, #1
    75fe:	7018      	strb	r0, [r3, #0]
    7600:	3301      	adds	r3, #1
    7602:	428a      	cmp	r2, r1
    7604:	d1f5      	bne.n	75f2 <GetAddr_8730A_ROM+0xa>
    7606:	4770      	bx	lr
    7608:	230009bc 	.word	0x230009bc
    760c:	43000618 	.word	0x43000618
    7610:	4300061e 	.word	0x4300061e

00007614 <ARP_hdl_8730A_ROM>:
    7614:	b510      	push	{r4, lr}
    7616:	b128      	cbz	r0, 7624 <ARP_hdl_8730A_ROM+0x10>
    7618:	4b03      	ldr	r3, [pc, #12]	; (7628 <ARP_hdl_8730A_ROM+0x14>)
    761a:	681b      	ldr	r3, [r3, #0]
    761c:	7858      	ldrb	r0, [r3, #1]
    761e:	4b03      	ldr	r3, [pc, #12]	; (762c <ARP_hdl_8730A_ROM+0x18>)
    7620:	681b      	ldr	r3, [r3, #0]
    7622:	4798      	blx	r3
    7624:	bd10      	pop	{r4, pc}
    7626:	46c0      	nop			; (mov r8, r8)
    7628:	230009cc 	.word	0x230009cc
    762c:	23000864 	.word	0x23000864

00007630 <NDP_hdl_8730A_ROM>:
    7630:	b510      	push	{r4, lr}
    7632:	b128      	cbz	r0, 7640 <NDP_hdl_8730A_ROM+0x10>
    7634:	4b03      	ldr	r3, [pc, #12]	; (7644 <NDP_hdl_8730A_ROM+0x14>)
    7636:	681b      	ldr	r3, [r3, #0]
    7638:	7998      	ldrb	r0, [r3, #6]
    763a:	4b03      	ldr	r3, [pc, #12]	; (7648 <NDP_hdl_8730A_ROM+0x18>)
    763c:	681b      	ldr	r3, [r3, #0]
    763e:	4798      	blx	r3
    7640:	bd10      	pop	{r4, pc}
    7642:	46c0      	nop			; (mov r8, r8)
    7644:	230009cc 	.word	0x230009cc
    7648:	2300086c 	.word	0x2300086c

0000764c <InitDisconnectDecision_8730A_ROM>:
    764c:	b510      	push	{r4, lr}
    764e:	4c06      	ldr	r4, [pc, #24]	; (7668 <InitDisconnectDecision_8730A_ROM+0x1c>)
    7650:	2208      	movs	r2, #8
    7652:	2100      	movs	r1, #0
    7654:	6820      	ldr	r0, [r4, #0]
    7656:	4b05      	ldr	r3, [pc, #20]	; (766c <InitDisconnectDecision_8730A_ROM+0x20>)
    7658:	4798      	blx	r3
    765a:	2208      	movs	r2, #8
    765c:	6823      	ldr	r3, [r4, #0]
    765e:	705a      	strb	r2, [r3, #1]
    7660:	3a05      	subs	r2, #5
    7662:	709a      	strb	r2, [r3, #2]
    7664:	bd10      	pop	{r4, pc}
    7666:	46c0      	nop			; (mov r8, r8)
    7668:	230009b8 	.word	0x230009b8
    766c:	00004ca9 	.word	0x00004ca9

00007670 <H2CHDL_DisconnectDecision_8730A_ROM>:
    7670:	4b14      	ldr	r3, [pc, #80]	; (76c4 <H2CHDL_DisconnectDecision_8730A_ROM+0x54>)
    7672:	b570      	push	{r4, r5, r6, lr}
    7674:	0004      	movs	r4, r0
    7676:	681b      	ldr	r3, [r3, #0]
    7678:	4798      	blx	r3
    767a:	2001      	movs	r0, #1
    767c:	4b12      	ldr	r3, [pc, #72]	; (76c8 <H2CHDL_DisconnectDecision_8730A_ROM+0x58>)
    767e:	7821      	ldrb	r1, [r4, #0]
    7680:	681b      	ldr	r3, [r3, #0]
    7682:	4001      	ands	r1, r0
    7684:	781a      	ldrb	r2, [r3, #0]
    7686:	4382      	bics	r2, r0
    7688:	430a      	orrs	r2, r1
    768a:	701a      	strb	r2, [r3, #0]
    768c:	7822      	ldrb	r2, [r4, #0]
    768e:	0792      	lsls	r2, r2, #30
    7690:	d517      	bpl.n	76c2 <H2CHDL_DisconnectDecision_8730A_ROM+0x52>
    7692:	2504      	movs	r5, #4
    7694:	7862      	ldrb	r2, [r4, #1]
    7696:	705a      	strb	r2, [r3, #1]
    7698:	78a2      	ldrb	r2, [r4, #2]
    769a:	709a      	strb	r2, [r3, #2]
    769c:	7821      	ldrb	r1, [r4, #0]
    769e:	781a      	ldrb	r2, [r3, #0]
    76a0:	0889      	lsrs	r1, r1, #2
    76a2:	4001      	ands	r1, r0
    76a4:	43aa      	bics	r2, r5
    76a6:	0089      	lsls	r1, r1, #2
    76a8:	4311      	orrs	r1, r2
    76aa:	7019      	strb	r1, [r3, #0]
    76ac:	78e2      	ldrb	r2, [r4, #3]
    76ae:	b2c9      	uxtb	r1, r1
    76b0:	70da      	strb	r2, [r3, #3]
    76b2:	7822      	ldrb	r2, [r4, #0]
    76b4:	08d2      	lsrs	r2, r2, #3
    76b6:	4002      	ands	r2, r0
    76b8:	3007      	adds	r0, #7
    76ba:	00d2      	lsls	r2, r2, #3
    76bc:	4381      	bics	r1, r0
    76be:	430a      	orrs	r2, r1
    76c0:	701a      	strb	r2, [r3, #0]
    76c2:	bd70      	pop	{r4, r5, r6, pc}
    76c4:	230008a4 	.word	0x230008a4
    76c8:	230009b8 	.word	0x230009b8

000076cc <AesTkipIvFun_8730A_ROM>:
    76cc:	b5f0      	push	{r4, r5, r6, r7, lr}
    76ce:	000f      	movs	r7, r1
    76d0:	4b11      	ldr	r3, [pc, #68]	; (7718 <AesTkipIvFun_8730A_ROM+0x4c>)
    76d2:	b087      	sub	sp, #28
    76d4:	681b      	ldr	r3, [r3, #0]
    76d6:	0016      	movs	r6, r2
    76d8:	001c      	movs	r4, r3
    76da:	ad04      	add	r5, sp, #16
    76dc:	0013      	movs	r3, r2
    76de:	aa02      	add	r2, sp, #8
    76e0:	0029      	movs	r1, r5
    76e2:	3207      	adds	r2, #7
    76e4:	9001      	str	r0, [sp, #4]
    76e6:	47a0      	blx	r4
    76e8:	b957      	cbnz	r7, 7700 <AesTkipIvFun_8730A_ROM+0x34>
    76ea:	002a      	movs	r2, r5
    76ec:	0039      	movs	r1, r7
    76ee:	0038      	movs	r0, r7
    76f0:	7813      	ldrb	r3, [r2, #0]
    76f2:	2bff      	cmp	r3, #255	; 0xff
    76f4:	d10c      	bne.n	7710 <AesTkipIvFun_8730A_ROM+0x44>
    76f6:	3101      	adds	r1, #1
    76f8:	7010      	strb	r0, [r2, #0]
    76fa:	3201      	adds	r2, #1
    76fc:	2906      	cmp	r1, #6
    76fe:	d1f7      	bne.n	76f0 <AesTkipIvFun_8730A_ROM+0x24>
    7700:	4b06      	ldr	r3, [pc, #24]	; (771c <AesTkipIvFun_8730A_ROM+0x50>)
    7702:	0032      	movs	r2, r6
    7704:	0029      	movs	r1, r5
    7706:	681b      	ldr	r3, [r3, #0]
    7708:	9801      	ldr	r0, [sp, #4]
    770a:	4798      	blx	r3
    770c:	b007      	add	sp, #28
    770e:	bdf0      	pop	{r4, r5, r6, r7, pc}
    7710:	3301      	adds	r3, #1
    7712:	546b      	strb	r3, [r5, r1]
    7714:	e7f4      	b.n	7700 <AesTkipIvFun_8730A_ROM+0x34>
    7716:	46c0      	nop			; (mov r8, r8)
    7718:	230008c8 	.word	0x230008c8
    771c:	230008cc 	.word	0x230008cc

00007720 <WepIvFun_8730A_ROM>:
    7720:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    7722:	000c      	movs	r4, r1
    7724:	ad01      	add	r5, sp, #4
    7726:	0006      	movs	r6, r0
    7728:	0001      	movs	r1, r0
    772a:	2203      	movs	r2, #3
    772c:	0028      	movs	r0, r5
    772e:	4f0d      	ldr	r7, [pc, #52]	; (7764 <WepIvFun_8730A_ROM+0x44>)
    7730:	47b8      	blx	r7
    7732:	b97c      	cbnz	r4, 7754 <WepIvFun_8730A_ROM+0x34>
    7734:	782a      	ldrb	r2, [r5, #0]
    7736:	2aff      	cmp	r2, #255	; 0xff
    7738:	d10e      	bne.n	7758 <WepIvFun_8730A_ROM+0x38>
    773a:	786a      	ldrb	r2, [r5, #1]
    773c:	702c      	strb	r4, [r5, #0]
    773e:	2aff      	cmp	r2, #255	; 0xff
    7740:	d109      	bne.n	7756 <WepIvFun_8730A_ROM+0x36>
    7742:	78aa      	ldrb	r2, [r5, #2]
    7744:	706c      	strb	r4, [r5, #1]
    7746:	2aff      	cmp	r2, #255	; 0xff
    7748:	d109      	bne.n	775e <WepIvFun_8730A_ROM+0x3e>
    774a:	70ac      	strb	r4, [r5, #2]
    774c:	2203      	movs	r2, #3
    774e:	0029      	movs	r1, r5
    7750:	0030      	movs	r0, r6
    7752:	47b8      	blx	r7
    7754:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}
    7756:	2401      	movs	r4, #1
    7758:	3201      	adds	r2, #1
    775a:	552a      	strb	r2, [r5, r4]
    775c:	e7f6      	b.n	774c <WepIvFun_8730A_ROM+0x2c>
    775e:	2402      	movs	r4, #2
    7760:	e7fa      	b.n	7758 <WepIvFun_8730A_ROM+0x38>
    7762:	46c0      	nop			; (mov r8, r8)
    7764:	00004b5d 	.word	0x00004b5d

00007768 <RTmemcmpBackward_8730A_ROM>:
    7768:	2300      	movs	r3, #0
    776a:	b530      	push	{r4, r5, lr}
    776c:	1e54      	subs	r4, r2, #1
    776e:	1904      	adds	r4, r0, r4
    7770:	1889      	adds	r1, r1, r2
    7772:	18d0      	adds	r0, r2, r3
    7774:	b2c0      	uxtb	r0, r0
    7776:	b900      	cbnz	r0, 777a <RTmemcmpBackward_8730A_ROM+0x12>
    7778:	bd30      	pop	{r4, r5, pc}
    777a:	5ce0      	ldrb	r0, [r4, r3]
    777c:	3b01      	subs	r3, #1
    777e:	5ccd      	ldrb	r5, [r1, r3]
    7780:	4285      	cmp	r5, r0
    7782:	d2f6      	bcs.n	7772 <RTmemcmpBackward_8730A_ROM+0xa>
    7784:	2001      	movs	r0, #1
    7786:	e7f7      	b.n	7778 <RTmemcmpBackward_8730A_ROM+0x10>

00007788 <AssignIvToKeyRsc_8730A_ROM>:
    7788:	2b04      	cmp	r3, #4
    778a:	d10e      	bne.n	77aa <AssignIvToKeyRsc_8730A_ROM+0x22>
    778c:	7803      	ldrb	r3, [r0, #0]
    778e:	700b      	strb	r3, [r1, #0]
    7790:	7843      	ldrb	r3, [r0, #1]
    7792:	704b      	strb	r3, [r1, #1]
    7794:	7903      	ldrb	r3, [r0, #4]
    7796:	708b      	strb	r3, [r1, #2]
    7798:	7943      	ldrb	r3, [r0, #5]
    779a:	70cb      	strb	r3, [r1, #3]
    779c:	7983      	ldrb	r3, [r0, #6]
    779e:	710b      	strb	r3, [r1, #4]
    77a0:	79c3      	ldrb	r3, [r0, #7]
    77a2:	714b      	strb	r3, [r1, #5]
    77a4:	78c3      	ldrb	r3, [r0, #3]
    77a6:	7013      	strb	r3, [r2, #0]
    77a8:	4770      	bx	lr
    77aa:	2b02      	cmp	r3, #2
    77ac:	d1fc      	bne.n	77a8 <AssignIvToKeyRsc_8730A_ROM+0x20>
    77ae:	7883      	ldrb	r3, [r0, #2]
    77b0:	700b      	strb	r3, [r1, #0]
    77b2:	7803      	ldrb	r3, [r0, #0]
    77b4:	e7ed      	b.n	7792 <AssignIvToKeyRsc_8730A_ROM+0xa>

000077b6 <AssignKeyRscToIV_8730A_ROM>:
    77b6:	2a04      	cmp	r2, #4
    77b8:	d112      	bne.n	77e0 <AssignKeyRscToIV_8730A_ROM+0x2a>
    77ba:	780b      	ldrb	r3, [r1, #0]
    77bc:	7003      	strb	r3, [r0, #0]
    77be:	784b      	ldrb	r3, [r1, #1]
    77c0:	7043      	strb	r3, [r0, #1]
    77c2:	2300      	movs	r3, #0
    77c4:	7083      	strb	r3, [r0, #2]
    77c6:	2320      	movs	r3, #32
    77c8:	78c2      	ldrb	r2, [r0, #3]
    77ca:	4313      	orrs	r3, r2
    77cc:	70c3      	strb	r3, [r0, #3]
    77ce:	788b      	ldrb	r3, [r1, #2]
    77d0:	7103      	strb	r3, [r0, #4]
    77d2:	78cb      	ldrb	r3, [r1, #3]
    77d4:	7143      	strb	r3, [r0, #5]
    77d6:	790b      	ldrb	r3, [r1, #4]
    77d8:	7183      	strb	r3, [r0, #6]
    77da:	794b      	ldrb	r3, [r1, #5]
    77dc:	71c3      	strb	r3, [r0, #7]
    77de:	4770      	bx	lr
    77e0:	2a02      	cmp	r2, #2
    77e2:	d1f0      	bne.n	77c6 <AssignKeyRscToIV_8730A_ROM+0x10>
    77e4:	784b      	ldrb	r3, [r1, #1]
    77e6:	327d      	adds	r2, #125	; 0x7d
    77e8:	7003      	strb	r3, [r0, #0]
    77ea:	4013      	ands	r3, r2
    77ec:	3a5f      	subs	r2, #95	; 0x5f
    77ee:	4313      	orrs	r3, r2
    77f0:	7043      	strb	r3, [r0, #1]
    77f2:	780b      	ldrb	r3, [r1, #0]
    77f4:	e7e6      	b.n	77c4 <AssignKeyRscToIV_8730A_ROM+0xe>

000077f6 <AesTkipIvCheck_8730A_ROM>:
    77f6:	b5f0      	push	{r4, r5, r6, r7, lr}
    77f8:	0014      	movs	r4, r2
    77fa:	0005      	movs	r5, r0
    77fc:	b089      	sub	sp, #36	; 0x24
    77fe:	4f16      	ldr	r7, [pc, #88]	; (7858 <AesTkipIvCheck_8730A_ROM+0x62>)
    7800:	2208      	movs	r2, #8
    7802:	9101      	str	r1, [sp, #4]
    7804:	a804      	add	r0, sp, #16
    7806:	2100      	movs	r1, #0
    7808:	47b8      	blx	r7
    780a:	2208      	movs	r2, #8
    780c:	2100      	movs	r1, #0
    780e:	a806      	add	r0, sp, #24
    7810:	47b8      	blx	r7
    7812:	4f12      	ldr	r7, [pc, #72]	; (785c <AesTkipIvCheck_8730A_ROM+0x66>)
    7814:	aa02      	add	r2, sp, #8
    7816:	0023      	movs	r3, r4
    7818:	0028      	movs	r0, r5
    781a:	683e      	ldr	r6, [r7, #0]
    781c:	3207      	adds	r2, #7
    781e:	a904      	add	r1, sp, #16
    7820:	47b0      	blx	r6
    7822:	aa02      	add	r2, sp, #8
    7824:	0023      	movs	r3, r4
    7826:	9801      	ldr	r0, [sp, #4]
    7828:	3206      	adds	r2, #6
    782a:	a906      	add	r1, sp, #24
    782c:	683f      	ldr	r7, [r7, #0]
    782e:	47b8      	blx	r7
    7830:	4b0b      	ldr	r3, [pc, #44]	; (7860 <AesTkipIvCheck_8730A_ROM+0x6a>)
    7832:	2208      	movs	r2, #8
    7834:	681b      	ldr	r3, [r3, #0]
    7836:	a904      	add	r1, sp, #16
    7838:	a806      	add	r0, sp, #24
    783a:	4798      	blx	r3
    783c:	2300      	movs	r3, #0
    783e:	4298      	cmp	r0, r3
    7840:	dd06      	ble.n	7850 <AesTkipIvCheck_8730A_ROM+0x5a>
    7842:	4b08      	ldr	r3, [pc, #32]	; (7864 <AesTkipIvCheck_8730A_ROM+0x6e>)
    7844:	0022      	movs	r2, r4
    7846:	681b      	ldr	r3, [r3, #0]
    7848:	0028      	movs	r0, r5
    784a:	a906      	add	r1, sp, #24
    784c:	4798      	blx	r3
    784e:	2301      	movs	r3, #1
    7850:	0018      	movs	r0, r3
    7852:	b009      	add	sp, #36	; 0x24
    7854:	bdf0      	pop	{r4, r5, r6, r7, pc}
    7856:	46c0      	nop			; (mov r8, r8)
    7858:	00004ca9 	.word	0x00004ca9
    785c:	230008c8 	.word	0x230008c8
    7860:	230008c4 	.word	0x230008c4
    7864:	230008cc 	.word	0x230008cc

00007868 <WepIvCheck_8730A_ROM>:
    7868:	b537      	push	{r0, r1, r2, r4, r5, lr}
    786a:	0004      	movs	r4, r0
    786c:	2203      	movs	r2, #3
    786e:	4d0b      	ldr	r5, [pc, #44]	; (789c <WepIvCheck_8730A_ROM+0x34>)
    7870:	4668      	mov	r0, sp
    7872:	47a8      	blx	r5
    7874:	2203      	movs	r2, #3
    7876:	0021      	movs	r1, r4
    7878:	a801      	add	r0, sp, #4
    787a:	47a8      	blx	r5
    787c:	4b08      	ldr	r3, [pc, #32]	; (78a0 <WepIvCheck_8730A_ROM+0x38>)
    787e:	2203      	movs	r2, #3
    7880:	681b      	ldr	r3, [r3, #0]
    7882:	4668      	mov	r0, sp
    7884:	a901      	add	r1, sp, #4
    7886:	4798      	blx	r3
    7888:	2300      	movs	r3, #0
    788a:	4298      	cmp	r0, r3
    788c:	dd04      	ble.n	7898 <WepIvCheck_8730A_ROM+0x30>
    788e:	2203      	movs	r2, #3
    7890:	4669      	mov	r1, sp
    7892:	0020      	movs	r0, r4
    7894:	47a8      	blx	r5
    7896:	2301      	movs	r3, #1
    7898:	0018      	movs	r0, r3
    789a:	bd3e      	pop	{r1, r2, r3, r4, r5, pc}
    789c:	00004b5d 	.word	0x00004b5d
    78a0:	230008c4 	.word	0x230008c4

000078a4 <GetIV_8730A_ROM>:
    78a4:	b570      	push	{r4, r5, r6, lr}
    78a6:	001e      	movs	r6, r3
    78a8:	1840      	adds	r0, r0, r1
    78aa:	b285      	uxth	r5, r0
    78ac:	1aaa      	subs	r2, r5, r2
    78ae:	b294      	uxth	r4, r2
    78b0:	42ac      	cmp	r4, r5
    78b2:	d100      	bne.n	78b6 <GetIV_8730A_ROM+0x12>
    78b4:	bd70      	pop	{r4, r5, r6, pc}
    78b6:	4b05      	ldr	r3, [pc, #20]	; (78cc <GetIV_8730A_ROM+0x28>)
    78b8:	0020      	movs	r0, r4
    78ba:	2100      	movs	r1, #0
    78bc:	681b      	ldr	r3, [r3, #0]
    78be:	4798      	blx	r3
    78c0:	3401      	adds	r4, #1
    78c2:	7030      	strb	r0, [r6, #0]
    78c4:	b2a4      	uxth	r4, r4
    78c6:	3601      	adds	r6, #1
    78c8:	e7f2      	b.n	78b0 <GetIV_8730A_ROM+0xc>
    78ca:	46c0      	nop			; (mov r8, r8)
    78cc:	230007e0 	.word	0x230007e0

000078d0 <GetIVLenAndSecurityType_8730A_ROM>:
    78d0:	4b0e      	ldr	r3, [pc, #56]	; (790c <GetIVLenAndSecurityType_8730A_ROM+0x3c>)
    78d2:	681a      	ldr	r2, [r3, #0]
    78d4:	4b0e      	ldr	r3, [pc, #56]	; (7910 <GetIVLenAndSecurityType_8730A_ROM+0x40>)
    78d6:	681b      	ldr	r3, [r3, #0]
    78d8:	3331      	adds	r3, #49	; 0x31
    78da:	07c1      	lsls	r1, r0, #31
    78dc:	d40e      	bmi.n	78fc <GetIVLenAndSecurityType_8730A_ROM+0x2c>
    78de:	7812      	ldrb	r2, [r2, #0]
    78e0:	701a      	strb	r2, [r3, #0]
    78e2:	781b      	ldrb	r3, [r3, #0]
    78e4:	2008      	movs	r0, #8
    78e6:	2b04      	cmp	r3, #4
    78e8:	d007      	beq.n	78fa <GetIVLenAndSecurityType_8730A_ROM+0x2a>
    78ea:	d809      	bhi.n	7900 <GetIVLenAndSecurityType_8730A_ROM+0x30>
    78ec:	3804      	subs	r0, #4
    78ee:	2b01      	cmp	r3, #1
    78f0:	d003      	beq.n	78fa <GetIVLenAndSecurityType_8730A_ROM+0x2a>
    78f2:	3b02      	subs	r3, #2
    78f4:	425a      	negs	r2, r3
    78f6:	4153      	adcs	r3, r2
    78f8:	00d8      	lsls	r0, r3, #3
    78fa:	4770      	bx	lr
    78fc:	7852      	ldrb	r2, [r2, #1]
    78fe:	e7ef      	b.n	78e0 <GetIVLenAndSecurityType_8730A_ROM+0x10>
    7900:	3b05      	subs	r3, #5
    7902:	2004      	movs	r0, #4
    7904:	2b01      	cmp	r3, #1
    7906:	d9f8      	bls.n	78fa <GetIVLenAndSecurityType_8730A_ROM+0x2a>
    7908:	2000      	movs	r0, #0
    790a:	e7f6      	b.n	78fa <GetIVLenAndSecurityType_8730A_ROM+0x2a>
    790c:	230009d0 	.word	0x230009d0
    7910:	230009d4 	.word	0x230009d4

00007914 <GetMACHeaderLen_8730A_ROM>:
    7914:	000b      	movs	r3, r1
    7916:	b510      	push	{r4, lr}
    7918:	09c4      	lsrs	r4, r0, #7
    791a:	2040      	movs	r0, #64	; 0x40
    791c:	0064      	lsls	r4, r4, #1
    791e:	4003      	ands	r3, r0
    7920:	4201      	tst	r1, r0
    7922:	d004      	beq.n	792e <GetMACHeaderLen_8730A_ROM+0x1a>
    7924:	4b04      	ldr	r3, [pc, #16]	; (7938 <GetMACHeaderLen_8730A_ROM+0x24>)
    7926:	0010      	movs	r0, r2
    7928:	681b      	ldr	r3, [r3, #0]
    792a:	4798      	blx	r3
    792c:	0003      	movs	r3, r0
    792e:	0020      	movs	r0, r4
    7930:	3018      	adds	r0, #24
    7932:	1818      	adds	r0, r3, r0
    7934:	b2c0      	uxtb	r0, r0
    7936:	bd10      	pop	{r4, pc}
    7938:	230008b0 	.word	0x230008b0

0000793c <UpdateIV_8730A_ROM>:
    793c:	b5f7      	push	{r0, r1, r2, r4, r5, r6, r7, lr}
    793e:	4b17      	ldr	r3, [pc, #92]	; (799c <UpdateIV_8730A_ROM+0x60>)
    7940:	7902      	ldrb	r2, [r0, #4]
    7942:	0004      	movs	r4, r0
    7944:	681b      	ldr	r3, [r3, #0]
    7946:	000e      	movs	r6, r1
    7948:	7841      	ldrb	r1, [r0, #1]
    794a:	7800      	ldrb	r0, [r0, #0]
    794c:	4798      	blx	r3
    794e:	4b14      	ldr	r3, [pc, #80]	; (79a0 <UpdateIV_8730A_ROM+0x64>)
    7950:	9001      	str	r0, [sp, #4]
    7952:	681b      	ldr	r3, [r3, #0]
    7954:	7920      	ldrb	r0, [r4, #4]
    7956:	4798      	blx	r3
    7958:	4f12      	ldr	r7, [pc, #72]	; (79a4 <UpdateIV_8730A_ROM+0x68>)
    795a:	0005      	movs	r5, r0
    795c:	6838      	ldr	r0, [r7, #0]
    795e:	0003      	movs	r3, r0
    7960:	3331      	adds	r3, #49	; 0x31
    7962:	781a      	ldrb	r2, [r3, #0]
    7964:	2a04      	cmp	r2, #4
    7966:	d014      	beq.n	7992 <UpdateIV_8730A_ROM+0x56>
    7968:	d804      	bhi.n	7974 <UpdateIV_8730A_ROM+0x38>
    796a:	2a01      	cmp	r2, #1
    796c:	d005      	beq.n	797a <UpdateIV_8730A_ROM+0x3e>
    796e:	2a02      	cmp	r2, #2
    7970:	d00f      	beq.n	7992 <UpdateIV_8730A_ROM+0x56>
    7972:	bdf7      	pop	{r0, r1, r2, r4, r5, r6, r7, pc}
    7974:	3a05      	subs	r2, #5
    7976:	2a01      	cmp	r2, #1
    7978:	d8fb      	bhi.n	7972 <UpdateIV_8730A_ROM+0x36>
    797a:	4b0b      	ldr	r3, [pc, #44]	; (79a8 <UpdateIV_8730A_ROM+0x6c>)
    797c:	0031      	movs	r1, r6
    797e:	681b      	ldr	r3, [r3, #0]
    7980:	4798      	blx	r3
    7982:	9b01      	ldr	r3, [sp, #4]
    7984:	002a      	movs	r2, r5
    7986:	1b58      	subs	r0, r3, r5
    7988:	6839      	ldr	r1, [r7, #0]
    798a:	4b08      	ldr	r3, [pc, #32]	; (79ac <UpdateIV_8730A_ROM+0x70>)
    798c:	1820      	adds	r0, r4, r0
    798e:	4798      	blx	r3
    7990:	e7ef      	b.n	7972 <UpdateIV_8730A_ROM+0x36>
    7992:	4b07      	ldr	r3, [pc, #28]	; (79b0 <UpdateIV_8730A_ROM+0x74>)
    7994:	0031      	movs	r1, r6
    7996:	681b      	ldr	r3, [r3, #0]
    7998:	4798      	blx	r3
    799a:	e7f2      	b.n	7982 <UpdateIV_8730A_ROM+0x46>
    799c:	230008b4 	.word	0x230008b4
    79a0:	230008b0 	.word	0x230008b0
    79a4:	230009d4 	.word	0x230009d4
    79a8:	230008c0 	.word	0x230008c0
    79ac:	00004b5d 	.word	0x00004b5d
    79b0:	230008bc 	.word	0x230008bc
