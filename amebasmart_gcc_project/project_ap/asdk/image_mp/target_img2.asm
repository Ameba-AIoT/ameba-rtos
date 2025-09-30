
/home/auto_jenkins_wlan5/workspace/sdk/auto-release-master/amebasmart_gcc_project/project_ap/asdk/image_mp/target_img2.axf:     file format elf32-littlearm


Disassembly of section .code:

60300000 <__dram_text_start__>:
60300000:	ea000030 	b	603000c8 <_boot>
60300004:	ea000009 	b	60300030 <FreeRTOS_UndefinedHandler>
60300008:	ea00015c 	b	60300580 <FreeRTOS_SWI_Handler>
6030000c:	ea00001f 	b	60300090 <FreeRTOS_PrefetchAbortHandler>
60300010:	ea000012 	b	60300060 <FreeRTOS_DataAbortHandler>
60300014:	eafffffe 	b	60300014 <__dram_text_start__+0x14>
60300018:	ea000190 	b	60300660 <FreeRTOS_IRQ_Handler>
6030001c:	eaffffff 	b	60300020 <FreeRTOS_FIQHandler>

60300020 <FreeRTOS_FIQHandler>:
60300020:	e92d500f 	push	{r0, r1, r2, r3, ip, lr}
60300024:	eb002b8b 	bl	6030ae58 <FIQHandler>
60300028:	e8bd500f 	pop	{r0, r1, r2, r3, ip, lr}
6030002c:	e25ef004 	subs	pc, lr, #4

60300030 <FreeRTOS_UndefinedHandler>:
60300030:	e92d500f 	push	{r0, r1, r2, r3, ip, lr}
60300034:	e59f0080 	ldr	r0, [pc, #128]	; 603000bc <FreeRTOS_PrefetchAbortHandler+0x2c>
60300038:	e24e1004 	sub	r1, lr, #4
6030003c:	e5801000 	str	r1, [r0]
60300040:	e92d0ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp}
60300044:	e1a0000d 	mov	r0, sp
60300048:	e14f1000 	mrs	r1, SPSR
6030004c:	eb002c04 	bl	6030b064 <UndefinedExceptionHandler>
60300050:	e8bd0ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp}
60300054:	e8bd500f 	pop	{r0, r1, r2, r3, ip, lr}
60300058:	e1b0f00e 	movs	pc, lr
6030005c:	e320f000 	nop	{0}

60300060 <FreeRTOS_DataAbortHandler>:
60300060:	e92d500f 	push	{r0, r1, r2, r3, ip, lr}
60300064:	e59f0054 	ldr	r0, [pc, #84]	; 603000c0 <FreeRTOS_PrefetchAbortHandler+0x30>
60300068:	e24e1008 	sub	r1, lr, #8
6030006c:	e5801000 	str	r1, [r0]
60300070:	e92d0ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp}
60300074:	e1a0000d 	mov	r0, sp
60300078:	e14f1000 	mrs	r1, SPSR
6030007c:	eb002c05 	bl	6030b098 <DataAbortHandler>
60300080:	e8bd0ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp}
60300084:	e8bd500f 	pop	{r0, r1, r2, r3, ip, lr}
60300088:	e25ef008 	subs	pc, lr, #8
6030008c:	e320f000 	nop	{0}

60300090 <FreeRTOS_PrefetchAbortHandler>:
60300090:	e92d500f 	push	{r0, r1, r2, r3, ip, lr}
60300094:	e59f0028 	ldr	r0, [pc, #40]	; 603000c4 <FreeRTOS_PrefetchAbortHandler+0x34>
60300098:	e24e1004 	sub	r1, lr, #4
6030009c:	e5801000 	str	r1, [r0]
603000a0:	e92d0ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp}
603000a4:	e1a0000d 	mov	r0, sp
603000a8:	e14f1000 	mrs	r1, SPSR
603000ac:	eb002c0a 	bl	6030b0dc <PrefetchAbortHandler>
603000b0:	e8bd0ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp}
603000b4:	e8bd500f 	pop	{r0, r1, r2, r3, ip, lr}
603000b8:	e25ef004 	subs	pc, lr, #4
603000bc:	603179e0 	.word	0x603179e0
603000c0:	603179d8 	.word	0x603179d8
603000c4:	603179dc 	.word	0x603179dc

603000c8 <_boot>:
603000c8:	e59f0274 	ldr	r0, [pc, #628]	; 60300344 <putc_error+0x8>
603000cc:	ee0c0f10 	mcr	15, 0, r0, cr12, cr0, {0}
603000d0:	e3a00000 	mov	r0, #0
603000d4:	ee080f17 	mcr	15, 0, r0, cr8, cr7, {0}
603000d8:	ee070f15 	mcr	15, 0, r0, cr7, cr5, {0}
603000dc:	ee070fd5 	mcr	15, 0, r0, cr7, cr5, {6}
603000e0:	f57ff04f 	dsb	sy
603000e4:	f57ff06f 	isb	sy
603000e8:	eb000060 	bl	60300270 <invalidate_dcache>
603000ec:	ee110f10 	mrc	15, 0, r0, cr1, cr0, {0}
603000f0:	e3c00a02 	bic	r0, r0, #8192	; 0x2000
603000f4:	e3c00007 	bic	r0, r0, #7
603000f8:	e3800002 	orr	r0, r0, #2
603000fc:	e3800b02 	orr	r0, r0, #2048	; 0x800
60300100:	e3c00a01 	bic	r0, r0, #4096	; 0x1000
60300104:	ee010f10 	mcr	15, 0, r0, cr1, cr0, {0}
60300108:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
6030010c:	e20030ff 	and	r3, r0, #255	; 0xff
60300110:	e2833001 	add	r3, r3, #1
60300114:	e10f0000 	mrs	r0, CPSR
60300118:	e3e0101f 	mvn	r1, #31
6030011c:	e0012000 	and	r2, r1, r0
60300120:	e3822011 	orr	r2, r2, #17
60300124:	e129f002 	msr	CPSR_fc, r2
60300128:	e3a00b01 	mov	r0, #1024	; 0x400
6030012c:	e0010390 	mul	r1, r0, r3
60300130:	e59f2210 	ldr	r2, [pc, #528]	; 60300348 <putc_error+0xc>
60300134:	e081d002 	add	sp, r1, r2
60300138:	e10f0000 	mrs	r0, CPSR
6030013c:	e3e0101f 	mvn	r1, #31
60300140:	e0012000 	and	r2, r1, r0
60300144:	e3822012 	orr	r2, r2, #18
60300148:	e129f002 	msr	CPSR_fc, r2
6030014c:	e3a00b01 	mov	r0, #1024	; 0x400
60300150:	e0010390 	mul	r1, r0, r3
60300154:	e59f21f0 	ldr	r2, [pc, #496]	; 6030034c <putc_error+0x10>
60300158:	e081d002 	add	sp, r1, r2
6030015c:	e10f0000 	mrs	r0, CPSR
60300160:	e3e0101f 	mvn	r1, #31
60300164:	e0012000 	and	r2, r1, r0
60300168:	e3822017 	orr	r2, r2, #23
6030016c:	e129f002 	msr	CPSR_fc, r2
60300170:	e3a00b01 	mov	r0, #1024	; 0x400
60300174:	e0010390 	mul	r1, r0, r3
60300178:	e59f21d0 	ldr	r2, [pc, #464]	; 60300350 <putc_error+0x14>
6030017c:	e081d002 	add	sp, r1, r2
60300180:	e10f0000 	mrs	r0, CPSR
60300184:	e3e0101f 	mvn	r1, #31
60300188:	e0012000 	and	r2, r1, r0
6030018c:	e382201b 	orr	r2, r2, #27
60300190:	e129f002 	msr	CPSR_fc, r2
60300194:	e3a00b01 	mov	r0, #1024	; 0x400
60300198:	e0010390 	mul	r1, r0, r3
6030019c:	e59f21b0 	ldr	r2, [pc, #432]	; 60300354 <putc_error+0x18>
603001a0:	e081d002 	add	sp, r1, r2
603001a4:	e10f0000 	mrs	r0, CPSR
603001a8:	e3e0101f 	mvn	r1, #31
603001ac:	e0012000 	and	r2, r1, r0
603001b0:	e382201f 	orr	r2, r2, #31
603001b4:	e129f002 	msr	CPSR_fc, r2
603001b8:	e3a00b01 	mov	r0, #1024	; 0x400
603001bc:	e0010390 	mul	r1, r0, r3
603001c0:	e59f2190 	ldr	r2, [pc, #400]	; 60300358 <putc_error+0x1c>
603001c4:	e081d002 	add	sp, r1, r2
603001c8:	e10f0000 	mrs	r0, CPSR
603001cc:	e3e0101f 	mvn	r1, #31
603001d0:	e0012000 	and	r2, r1, r0
603001d4:	e3822013 	orr	r2, r2, #19
603001d8:	e129f002 	msr	CPSR_fc, r2
603001dc:	e3a00b02 	mov	r0, #2048	; 0x800
603001e0:	e0010390 	mul	r1, r0, r3
603001e4:	e59f2170 	ldr	r2, [pc, #368]	; 6030035c <putc_error+0x20>
603001e8:	e081d002 	add	sp, r1, r2
603001ec:	ee110f30 	mrc	15, 0, r0, cr1, cr0, {1}
603001f0:	e3800040 	orr	r0, r0, #64	; 0x40
603001f4:	e3800001 	orr	r0, r0, #1
603001f8:	ee010f30 	mcr	15, 0, r0, cr1, cr0, {1}
603001fc:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
60300200:	e20000ff 	and	r0, r0, #255	; 0xff
60300204:	e2800001 	add	r0, r0, #1
60300208:	eb002d03 	bl	6030b61c <setupMMUTable>
6030020c:	e59f014c 	ldr	r0, [pc, #332]	; 60300360 <putc_error+0x24>
60300210:	ee030f10 	mcr	15, 0, r0, cr3, cr0, {0}
60300214:	ee110f10 	mrc	15, 0, r0, cr1, cr0, {0}
60300218:	e3c00a02 	bic	r0, r0, #8192	; 0x2000
6030021c:	e3c00002 	bic	r0, r0, #2
60300220:	e3800a01 	orr	r0, r0, #4096	; 0x1000
60300224:	e3800004 	orr	r0, r0, #4
60300228:	e3800001 	orr	r0, r0, #1
6030022c:	f57ff04f 	dsb	sy
60300230:	ee010f10 	mcr	15, 0, r0, cr1, cr0, {0}
60300234:	f57ff06f 	isb	sy
60300238:	ee111f50 	mrc	15, 0, r1, cr1, cr0, {2}
6030023c:	e381160f 	orr	r1, r1, #15728640	; 0xf00000
60300240:	ee011f50 	mcr	15, 0, r1, cr1, cr0, {2}
60300244:	eef81a10 	vmrs	r1, fpexc
60300248:	e3811101 	orr	r1, r1, #1073741824	; 0x40000000
6030024c:	eee81a10 	vmsr	fpexc, r1
60300250:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
60300254:	e20030ff 	and	r3, r0, #255	; 0xff
60300258:	e2833001 	add	r3, r3, #1
6030025c:	e3530001 	cmp	r3, #1
60300260:	ca002c67 	bgt	6030b404 <vPortSecondaryStart>
60300264:	eb002ae8 	bl	6030ae0c <app_start>
60300268:	ebffffff 	bl	6030026c <exit>

6030026c <exit>:
6030026c:	ea001d7d 	b	60307868 <_exit>

60300270 <invalidate_dcache>:
60300270:	ee300f30 	mrc	15, 1, r0, cr0, cr0, {1}
60300274:	e2103407 	ands	r3, r0, #117440512	; 0x7000000
60300278:	e1a03c23 	lsr	r3, r3, #24
6030027c:	0a00001a 	beq	603002ec <finished>
60300280:	e3a0a000 	mov	sl, #0

60300284 <loop1>:
60300284:	e08a20aa 	add	r2, sl, sl, lsr #1
60300288:	e1a01230 	lsr	r1, r0, r2
6030028c:	e2011007 	and	r1, r1, #7
60300290:	e3510002 	cmp	r1, #2
60300294:	ba000011 	blt	603002e0 <skip>
60300298:	ee40af10 	mcr	15, 2, sl, cr0, cr0, {0}
6030029c:	f57ff06f 	isb	sy
603002a0:	ee301f10 	mrc	15, 1, r1, cr0, cr0, {0}
603002a4:	e2012007 	and	r2, r1, #7
603002a8:	e2822004 	add	r2, r2, #4
603002ac:	e59f40b0 	ldr	r4, [pc, #176]	; 60300364 <putc_error+0x28>
603002b0:	e01441a1 	ands	r4, r4, r1, lsr #3
603002b4:	e16f5f14 	clz	r5, r4
603002b8:	e59f70a8 	ldr	r7, [pc, #168]	; 60300368 <putc_error+0x2c>
603002bc:	e01776a1 	ands	r7, r7, r1, lsr #13

603002c0 <loop2>:
603002c0:	e1a09004 	mov	r9, r4

603002c4 <loop3>:
603002c4:	e18ab519 	orr	fp, sl, r9, lsl r5
603002c8:	e18bb217 	orr	fp, fp, r7, lsl r2
603002cc:	ee07bf56 	mcr	15, 0, fp, cr7, cr6, {2}
603002d0:	e2599001 	subs	r9, r9, #1
603002d4:	aafffffa 	bge	603002c4 <loop3>
603002d8:	e2577001 	subs	r7, r7, #1
603002dc:	aafffff7 	bge	603002c0 <loop2>

603002e0 <skip>:
603002e0:	e28aa002 	add	sl, sl, #2
603002e4:	e153000a 	cmp	r3, sl
603002e8:	caffffe5 	bgt	60300284 <loop1>

603002ec <finished>:
603002ec:	e3a0a000 	mov	sl, #0
603002f0:	ee40af10 	mcr	15, 2, sl, cr0, cr0, {0}
603002f4:	f57ff04f 	dsb	sy
603002f8:	f57ff06f 	isb	sy
603002fc:	e12fff1e 	bx	lr

60300300 <__PutCharacter>:
60300300:	e59f1064 	ldr	r1, [pc, #100]	; 6030036c <putc_error+0x30>
60300304:	e3510000 	cmp	r1, #0
60300308:	0a00000b 	beq	6030033c <putc_error>
6030030c:	e350000a 	cmp	r0, #10
60300310:	1a000004 	bne	60300328 <__PutCharacter+0x28>
60300314:	e5912014 	ldr	r2, [r1, #20]
60300318:	e3120502 	tst	r2, #8388608	; 0x800000
6030031c:	0afffffc 	beq	60300314 <__PutCharacter+0x14>
60300320:	e3a0200d 	mov	r2, #13
60300324:	e5812068 	str	r2, [r1, #104]	; 0x68
60300328:	e5912014 	ldr	r2, [r1, #20]
6030032c:	e3120502 	tst	r2, #8388608	; 0x800000
60300330:	0afffffc 	beq	60300328 <__PutCharacter+0x28>
60300334:	e5810068 	str	r0, [r1, #104]	; 0x68
60300338:	e12fff1e 	bx	lr

6030033c <putc_error>:
6030033c:	e3e00000 	mvn	r0, #0
60300340:	e12fff1e 	bx	lr
60300344:	60300000 	.word	0x60300000
60300348:	6032a000 	.word	0x6032a000
6030034c:	6032a800 	.word	0x6032a800
60300350:	6032c000 	.word	0x6032c000
60300354:	6032c800 	.word	0x6032c800
60300358:	6032d000 	.word	0x6032d000
6030035c:	6032b000 	.word	0x6032b000
60300360:	55555555 	.word	0x55555555
60300364:	000003ff 	.word	0x000003ff
60300368:	00007fff 	.word	0x00007fff
6030036c:	4200c000 	.word	0x4200c000

60300370 <cpu_suspend>:
60300370:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60300374:	e1a0500d 	mov	r5, sp
60300378:	e92d0003 	push	{r0, r1}
6030037c:	e1a00005 	mov	r0, r5
60300380:	eb004de7 	bl	60313b24 <cpu_suspend_save>
60300384:	e8bd8001 	pop	{r0, pc}
60300388:	e320f000 	nop	{0}
6030038c:	e320f000 	nop	{0}

60300390 <cpu_do_suspend>:
60300390:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60300394:	f1020013 	cps	#19
60300398:	ee1d4f10 	mrc	15, 0, r4, cr13, cr0, {0}
6030039c:	ee1d5f70 	mrc	15, 0, r5, cr13, cr0, {3}
603003a0:	e8a00030 	stmia	r0!, {r4, r5}
603003a4:	ee1c4f10 	mrc	15, 0, r4, cr12, cr0, {0}
603003a8:	ee1a6f12 	mrc	15, 0, r6, cr10, cr2, {0}
603003ac:	ec575f02 	mrrc	15, 0, r5, r7, cr2
603003b0:	ee12bf50 	mrc	15, 0, fp, cr2, cr0, {2}
603003b4:	ee118f10 	mrc	15, 0, r8, cr1, cr0, {0}
603003b8:	ee119f30 	mrc	15, 0, r9, cr1, cr0, {1}
603003bc:	ee11af50 	mrc	15, 0, sl, cr1, cr0, {2}
603003c0:	e8a00ff0 	stmia	r0!, {r4, r5, r6, r7, r8, r9, sl, fp}
603003c4:	f1020012 	cps	#18
603003c8:	e14f2000 	mrs	r2, SPSR
603003cc:	e4802004 	str	r2, [r0], #4
603003d0:	e480d004 	str	sp, [r0], #4
603003d4:	e480e004 	str	lr, [r0], #4
603003d8:	f1020011 	cps	#17
603003dc:	e14f2000 	mrs	r2, SPSR
603003e0:	e4802004 	str	r2, [r0], #4
603003e4:	e480d004 	str	sp, [r0], #4
603003e8:	e480e004 	str	lr, [r0], #4
603003ec:	f1020017 	cps	#23
603003f0:	e14f2000 	mrs	r2, SPSR
603003f4:	e4802004 	str	r2, [r0], #4
603003f8:	e480d004 	str	sp, [r0], #4
603003fc:	e480e004 	str	lr, [r0], #4
60300400:	f102001b 	cps	#27
60300404:	e14f2000 	mrs	r2, SPSR
60300408:	e4802004 	str	r2, [r0], #4
6030040c:	e480d004 	str	sp, [r0], #4
60300410:	e480e004 	str	lr, [r0], #4
60300414:	f1020013 	cps	#19
60300418:	e14f2000 	mrs	r2, SPSR
6030041c:	e4802004 	str	r2, [r0], #4
60300420:	e480d004 	str	sp, [r0], #4
60300424:	e480e004 	str	lr, [r0], #4
60300428:	f102001f 	cps	#31
6030042c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

60300430 <cpu_resume>:
60300430:	e59f10cc 	ldr	r1, [pc, #204]	; 60300504 <cpu_resume+0xd4>
60300434:	e2810004 	add	r0, r1, #4
60300438:	e3a0c000 	mov	ip, #0
6030043c:	ee07cf15 	mcr	15, 0, ip, cr7, cr5, {0}
60300440:	ee0dcf30 	mcr	15, 0, ip, cr13, cr0, {1}
60300444:	e8b00030 	ldm	r0!, {r4, r5}
60300448:	ee0d4f10 	mcr	15, 0, r4, cr13, cr0, {0}
6030044c:	ee0d5f70 	mcr	15, 0, r5, cr13, cr0, {3}
60300450:	e8b00ff0 	ldm	r0!, {r4, r5, r6, r7, r8, r9, sl, fp}
60300454:	ee0c4f10 	mcr	15, 0, r4, cr12, cr0, {0}
60300458:	ee08cf17 	mcr	15, 0, ip, cr8, cr7, {0}
6030045c:	ee0a6f12 	mcr	15, 0, r6, cr10, cr2, {0}
60300460:	ec4c1f12 	mcrr	15, 1, r1, ip, cr2
60300464:	ec475f02 	mcrr	15, 0, r5, r7, cr2
60300468:	ee02bf50 	mcr	15, 0, fp, cr2, cr0, {2}
6030046c:	ee019f30 	mcr	15, 0, r9, cr1, cr0, {1}
60300470:	ee01af50 	mcr	15, 0, sl, cr1, cr0, {2}
60300474:	f57ff06f 	isb	sy
60300478:	f57ff04f 	dsb	sy
6030047c:	ee018f10 	mcr	15, 0, r8, cr1, cr0, {0}
60300480:	f1020012 	cps	#18
60300484:	e4902004 	ldr	r2, [r0], #4
60300488:	e490d004 	ldr	sp, [r0], #4
6030048c:	e490e004 	ldr	lr, [r0], #4
60300490:	e16ff002 	msr	SPSR_fsxc, r2
60300494:	f1020011 	cps	#17
60300498:	e4902004 	ldr	r2, [r0], #4
6030049c:	e490d004 	ldr	sp, [r0], #4
603004a0:	e490e004 	ldr	lr, [r0], #4
603004a4:	e16ff002 	msr	SPSR_fsxc, r2
603004a8:	f1020017 	cps	#23
603004ac:	e4902004 	ldr	r2, [r0], #4
603004b0:	e490d004 	ldr	sp, [r0], #4
603004b4:	e490e004 	ldr	lr, [r0], #4
603004b8:	e16ff002 	msr	SPSR_fsxc, r2
603004bc:	f102001b 	cps	#27
603004c0:	e4902004 	ldr	r2, [r0], #4
603004c4:	e490d004 	ldr	sp, [r0], #4
603004c8:	e490e004 	ldr	lr, [r0], #4
603004cc:	e16ff002 	msr	SPSR_fsxc, r2
603004d0:	f1020013 	cps	#19
603004d4:	e4902004 	ldr	r2, [r0], #4
603004d8:	e490d004 	ldr	sp, [r0], #4
603004dc:	e490e004 	ldr	lr, [r0], #4
603004e0:	e16ff002 	msr	SPSR_fsxc, r2
603004e4:	f57ff06f 	isb	sy
603004e8:	f57ff04f 	dsb	sy
603004ec:	f102001f 	cps	#31
603004f0:	e59f100c 	ldr	r1, [pc, #12]	; 60300504 <cpu_resume+0xd4>
603004f4:	e591d000 	ldr	sp, [r1]
603004f8:	f57ff06f 	isb	sy
603004fc:	e3a00000 	mov	r0, #0
60300500:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
60300504:	6031c880 	.word	0x6031c880

60300508 <FreeRTOS_IPI_FLASHPG_Handler>:
60300508:	e10f1000 	mrs	r1, CPSR
6030050c:	f10c0080 	cpsid	i
60300510:	f57ff04f 	dsb	sy
60300514:	f57ff06f 	isb	sy
60300518:	e3082400 	movw	r2, #33792	; 0x8400
6030051c:	e3462031 	movt	r2, #24625	; 0x6031
60300520:	e5923000 	ldr	r3, [r2]
60300524:	e2833001 	add	r3, r3, #1
60300528:	e5823000 	str	r3, [r2]
6030052c:	f57ff05f 	dmb	sy
60300530:	e5923000 	ldr	r3, [r2]
60300534:	e3530000 	cmp	r3, #0
60300538:	0a000003 	beq	6030054c <FreeRTOS_IPI_FLASHPG_Handler+0x44>
6030053c:	e320f002 	wfe
60300540:	e5923000 	ldr	r3, [r2]
60300544:	e3530000 	cmp	r3, #0
60300548:	1afffffb 	bne	6030053c <FreeRTOS_IPI_FLASHPG_Handler+0x34>
6030054c:	e121f001 	msr	CPSR_c, r1
60300550:	e12fff1e 	bx	lr

60300554 <__arm_smccc_smc>:
60300554:	e1a0c00d 	mov	ip, sp
60300558:	e92d00f0 	push	{r4, r5, r6, r7}
6030055c:	e89c00f0 	ldm	ip, {r4, r5, r6, r7}
60300560:	e1600070 	.word	0xe1600070
60300564:	e8bd00f0 	pop	{r4, r5, r6, r7}
60300568:	e59dc010 	ldr	ip, [sp, #16]
6030056c:	e88c000f 	stm	ip, {r0, r1, r2, r3}
60300570:	e12fff1e 	bx	lr
	...

60300580 <FreeRTOS_SWI_Handler>:
60300580:	f96d051f 	srsdb	sp!, #31
60300584:	f102001f 	cps	#31
60300588:	e92d5fff 	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
6030058c:	e59f2250 	ldr	r2, [pc, #592]	; 603007e4 <ulPortTaskHasFPUContextConst>
60300590:	e5923000 	ldr	r3, [r2]
60300594:	e3530000 	cmp	r3, #0
60300598:	1ef11a10 	vmrsne	r1, fpscr
6030059c:	1d2d0b20 	vpushne	{d0-d15}
603005a0:	1d6d0b20 	vpushne	{d16-d31}
603005a4:	152d1004 	pushne	{r1}		; (strne r1, [sp, #-4]!)
603005a8:	e52d3004 	push	{r3}		; (str r3, [sp, #-4]!)
603005ac:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
603005b0:	e20000ff 	and	r0, r0, #255	; 0xff
603005b4:	e59f1224 	ldr	r1, [pc, #548]	; 603007e0 <pxCurrentTCBConst>
603005b8:	e7912100 	ldr	r2, [r1, r0, lsl #2]
603005bc:	e582d000 	str	sp, [r2]
603005c0:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
603005c4:	e20000ff 	and	r0, r0, #255	; 0xff
603005c8:	e59f1218 	ldr	r1, [pc, #536]	; 603007e8 <vTaskSwitchContextConst>
603005cc:	e12fff31 	blx	r1
603005d0:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
603005d4:	e20000ff 	and	r0, r0, #255	; 0xff
603005d8:	e59f1200 	ldr	r1, [pc, #512]	; 603007e0 <pxCurrentTCBConst>
603005dc:	e7912100 	ldr	r2, [r1, r0, lsl #2]
603005e0:	e592d000 	ldr	sp, [r2]
603005e4:	e59f01f8 	ldr	r0, [pc, #504]	; 603007e4 <ulPortTaskHasFPUContextConst>
603005e8:	e49d1004 	pop	{r1}		; (ldr r1, [sp], #4)
603005ec:	e5801000 	str	r1, [r0]
603005f0:	e3510000 	cmp	r1, #0
603005f4:	149d0004 	popne	{r0}		; (ldrne r0, [sp], #4)
603005f8:	1cfd0b20 	vpopne	{d16-d31}
603005fc:	1cbd0b20 	vpopne	{d0-d15}
60300600:	1ee10a10 	vmsrne	fpscr, r0
60300604:	e8bd5fff 	pop	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
60300608:	f8bd0a00 	rfeia	sp!

6030060c <vPortRestoreTaskContext>:
6030060c:	f102001f 	cps	#31
60300610:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
60300614:	e20000ff 	and	r0, r0, #255	; 0xff
60300618:	e59f11c8 	ldr	r1, [pc, #456]	; 603007e8 <vTaskSwitchContextConst>
6030061c:	e12fff31 	blx	r1
60300620:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
60300624:	e20000ff 	and	r0, r0, #255	; 0xff
60300628:	e59f11b0 	ldr	r1, [pc, #432]	; 603007e0 <pxCurrentTCBConst>
6030062c:	e7912100 	ldr	r2, [r1, r0, lsl #2]
60300630:	e592d000 	ldr	sp, [r2]
60300634:	e59f01a8 	ldr	r0, [pc, #424]	; 603007e4 <ulPortTaskHasFPUContextConst>
60300638:	e49d1004 	pop	{r1}		; (ldr r1, [sp], #4)
6030063c:	e5801000 	str	r1, [r0]
60300640:	e3510000 	cmp	r1, #0
60300644:	149d0004 	popne	{r0}		; (ldrne r0, [sp], #4)
60300648:	1cfd0b20 	vpopne	{d16-d31}
6030064c:	1cbd0b20 	vpopne	{d0-d15}
60300650:	1ee10a10 	vmsrne	fpscr, r0
60300654:	e8bd5fff 	pop	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
60300658:	f8bd0a00 	rfeia	sp!
6030065c:	e320f000 	nop	{0}

60300660 <FreeRTOS_IRQ_Handler>:
60300660:	e24ee004 	sub	lr, lr, #4
60300664:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
60300668:	e14fe000 	mrs	lr, SPSR
6030066c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
60300670:	e92d101f 	push	{r0, r1, r2, r3, r4, ip}
60300674:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
60300678:	e20020ff 	and	r2, r0, #255	; 0xff
6030067c:	e59f3170 	ldr	r3, [pc, #368]	; 603007f4 <vApplicationFPUSafeIRQHandlerConst+0x4>
60300680:	e7931102 	ldr	r1, [r3, r2, lsl #2]
60300684:	e2814001 	add	r4, r1, #1
60300688:	e7834102 	str	r4, [r3, r2, lsl #2]
6030068c:	f1020013 	cps	#19
60300690:	e1a0200d 	mov	r2, sp
60300694:	e2022004 	and	r2, r2, #4
60300698:	e04dd002 	sub	sp, sp, r2
6030069c:	e92d401f 	push	{r0, r1, r2, r3, r4, lr}
603006a0:	e59f1144 	ldr	r1, [pc, #324]	; 603007ec <vApplicationIRQHandlerConst>
603006a4:	e12fff31 	blx	r1
603006a8:	e8bd401f 	pop	{r0, r1, r2, r3, r4, lr}
603006ac:	e08dd002 	add	sp, sp, r2
603006b0:	f1020012 	cps	#18
603006b4:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
603006b8:	e20020ff 	and	r2, r0, #255	; 0xff
603006bc:	e59f3130 	ldr	r3, [pc, #304]	; 603007f4 <vApplicationFPUSafeIRQHandlerConst+0x4>
603006c0:	e7931102 	ldr	r1, [r3, r2, lsl #2]
603006c4:	e2414001 	sub	r4, r1, #1
603006c8:	e7834102 	str	r4, [r3, r2, lsl #2]
603006cc:	e3540000 	cmp	r4, #0
603006d0:	1a000005 	bne	603006ec <exit_without_switch>
603006d4:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
603006d8:	e20020ff 	and	r2, r0, #255	; 0xff
603006dc:	e59f1114 	ldr	r1, [pc, #276]	; 603007f8 <vApplicationFPUSafeIRQHandlerConst+0x8>
603006e0:	e7910102 	ldr	r0, [r1, r2, lsl #2]
603006e4:	e3500000 	cmp	r0, #0
603006e8:	1a000004 	bne	60300700 <switch_before_exit>

603006ec <exit_without_switch>:
603006ec:	e8bd101f 	pop	{r0, r1, r2, r3, r4, ip}
603006f0:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
603006f4:	e16ff00e 	msr	SPSR_fsxc, lr
603006f8:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
603006fc:	e1b0f00e 	movs	pc, lr

60300700 <switch_before_exit>:
60300700:	e3a00000 	mov	r0, #0
60300704:	e7810102 	str	r0, [r1, r2, lsl #2]
60300708:	e8bd101f 	pop	{r0, r1, r2, r3, r4, ip}
6030070c:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
60300710:	e16ff00e 	msr	SPSR_fsxc, lr
60300714:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
60300718:	f96d051f 	srsdb	sp!, #31
6030071c:	f102001f 	cps	#31
60300720:	e92d5fff 	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
60300724:	e59f20b8 	ldr	r2, [pc, #184]	; 603007e4 <ulPortTaskHasFPUContextConst>
60300728:	e5923000 	ldr	r3, [r2]
6030072c:	e3530000 	cmp	r3, #0
60300730:	1ef11a10 	vmrsne	r1, fpscr
60300734:	1d2d0b20 	vpushne	{d0-d15}
60300738:	1d6d0b20 	vpushne	{d16-d31}
6030073c:	152d1004 	pushne	{r1}		; (strne r1, [sp, #-4]!)
60300740:	e52d3004 	push	{r3}		; (str r3, [sp, #-4]!)
60300744:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
60300748:	e20000ff 	and	r0, r0, #255	; 0xff
6030074c:	e59f108c 	ldr	r1, [pc, #140]	; 603007e0 <pxCurrentTCBConst>
60300750:	e7912100 	ldr	r2, [r1, r0, lsl #2]
60300754:	e582d000 	str	sp, [r2]
60300758:	f1020012 	cps	#18
6030075c:	e59f1084 	ldr	r1, [pc, #132]	; 603007e8 <vTaskSwitchContextConst>
60300760:	e12fff31 	blx	r1
60300764:	f102001f 	cps	#31
60300768:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
6030076c:	e20000ff 	and	r0, r0, #255	; 0xff
60300770:	e59f1068 	ldr	r1, [pc, #104]	; 603007e0 <pxCurrentTCBConst>
60300774:	e7912100 	ldr	r2, [r1, r0, lsl #2]
60300778:	e592d000 	ldr	sp, [r2]
6030077c:	e59f0060 	ldr	r0, [pc, #96]	; 603007e4 <ulPortTaskHasFPUContextConst>
60300780:	e49d1004 	pop	{r1}		; (ldr r1, [sp], #4)
60300784:	e5801000 	str	r1, [r0]
60300788:	e3510000 	cmp	r1, #0
6030078c:	149d0004 	popne	{r0}		; (ldrne r0, [sp], #4)
60300790:	1cfd0b20 	vpopne	{d16-d31}
60300794:	1cbd0b20 	vpopne	{d0-d15}
60300798:	1ee10a10 	vmsrne	fpscr, r0
6030079c:	e8bd5fff 	pop	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
603007a0:	f8bd0a00 	rfeia	sp!
603007a4:	e320f000 	nop	{0}
603007a8:	e320f000 	nop	{0}
603007ac:	e320f000 	nop	{0}

603007b0 <vApplicationIRQHandler>:
603007b0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
603007b4:	eef11a10 	vmrs	r1, fpscr
603007b8:	ed2d0b20 	vpush	{d0-d15}
603007bc:	ed6d0b20 	vpush	{d16-d31}
603007c0:	e52d1004 	push	{r1}		; (str r1, [sp, #-4]!)
603007c4:	e59f1024 	ldr	r1, [pc, #36]	; 603007f0 <vApplicationFPUSafeIRQHandlerConst>
603007c8:	e12fff31 	blx	r1
603007cc:	e49d0004 	pop	{r0}		; (ldr r0, [sp], #4)
603007d0:	ecfd0b20 	vpop	{d16-d31}
603007d4:	ecbd0b20 	vpop	{d0-d15}
603007d8:	eee10a10 	vmsr	fpscr, r0
603007dc:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

603007e0 <pxCurrentTCBConst>:
603007e0:	60317ff0 	.word	0x60317ff0

603007e4 <ulPortTaskHasFPUContextConst>:
603007e4:	6031840c 	.word	0x6031840c

603007e8 <vTaskSwitchContextConst>:
603007e8:	6030e2c4 	.word	0x6030e2c4

603007ec <vApplicationIRQHandlerConst>:
603007ec:	603007b0 	.word	0x603007b0

603007f0 <vApplicationFPUSafeIRQHandlerConst>:
603007f0:	6030ad1c 	.word	0x6030ad1c
603007f4:	60318404 	.word	0x60318404
603007f8:	60318410 	.word	0x60318410

603007fc <deregister_tm_clones>:
603007fc:	e59f0018 	ldr	r0, [pc, #24]	; 6030081c <deregister_tm_clones+0x20>
60300800:	e59f3018 	ldr	r3, [pc, #24]	; 60300820 <deregister_tm_clones+0x24>
60300804:	e1530000 	cmp	r3, r0
60300808:	012fff1e 	bxeq	lr
6030080c:	e59f3010 	ldr	r3, [pc, #16]	; 60300824 <deregister_tm_clones+0x28>
60300810:	e3530000 	cmp	r3, #0
60300814:	012fff1e 	bxeq	lr
60300818:	e12fff13 	bx	r3
6030081c:	603172c4 	.word	0x603172c4
60300820:	603172c4 	.word	0x603172c4
60300824:	00000000 	.word	0x00000000

60300828 <register_tm_clones>:
60300828:	e59f0024 	ldr	r0, [pc, #36]	; 60300854 <register_tm_clones+0x2c>
6030082c:	e59f1024 	ldr	r1, [pc, #36]	; 60300858 <register_tm_clones+0x30>
60300830:	e0413000 	sub	r3, r1, r0
60300834:	e1a01fa3 	lsr	r1, r3, #31
60300838:	e0811143 	add	r1, r1, r3, asr #2
6030083c:	e1b010c1 	asrs	r1, r1, #1
60300840:	012fff1e 	bxeq	lr
60300844:	e59f3010 	ldr	r3, [pc, #16]	; 6030085c <register_tm_clones+0x34>
60300848:	e3530000 	cmp	r3, #0
6030084c:	012fff1e 	bxeq	lr
60300850:	e12fff13 	bx	r3
60300854:	603172c4 	.word	0x603172c4
60300858:	603172c4 	.word	0x603172c4
6030085c:	00000000 	.word	0x00000000

60300860 <__do_global_dtors_aux>:
60300860:	e92d4010 	push	{r4, lr}
60300864:	e59f402c 	ldr	r4, [pc, #44]	; 60300898 <__do_global_dtors_aux+0x38>
60300868:	e5d43000 	ldrb	r3, [r4]
6030086c:	e3530000 	cmp	r3, #0
60300870:	18bd8010 	popne	{r4, pc}
60300874:	ebffffe0 	bl	603007fc <deregister_tm_clones>
60300878:	e59f301c 	ldr	r3, [pc, #28]	; 6030089c <__do_global_dtors_aux+0x3c>
6030087c:	e3530000 	cmp	r3, #0
60300880:	0a000001 	beq	6030088c <__do_global_dtors_aux+0x2c>
60300884:	e59f0014 	ldr	r0, [pc, #20]	; 603008a0 <__do_global_dtors_aux+0x40>
60300888:	e320f000 	nop	{0}
6030088c:	e3a03001 	mov	r3, #1
60300890:	e5c43000 	strb	r3, [r4]
60300894:	e8bd8010 	pop	{r4, pc}
60300898:	60317300 	.word	0x60317300
6030089c:	00000000 	.word	0x00000000
603008a0:	60316ed4 	.word	0x60316ed4

603008a4 <frame_dummy>:
603008a4:	e59f301c 	ldr	r3, [pc, #28]	; 603008c8 <frame_dummy+0x24>
603008a8:	e3530000 	cmp	r3, #0
603008ac:	0a000004 	beq	603008c4 <frame_dummy+0x20>
603008b0:	e92d4010 	push	{r4, lr}
603008b4:	e59f1010 	ldr	r1, [pc, #16]	; 603008cc <frame_dummy+0x28>
603008b8:	e59f0010 	ldr	r0, [pc, #16]	; 603008d0 <frame_dummy+0x2c>
603008bc:	e320f000 	nop	{0}
603008c0:	e8bd4010 	pop	{r4, lr}
603008c4:	eaffffd7 	b	60300828 <register_tm_clones>
603008c8:	00000000 	.word	0x00000000
603008cc:	60317304 	.word	0x60317304
603008d0:	60316ed4 	.word	0x60316ed4

603008d4 <__aeabi_uldivmod>:
603008d4:	e3530000 	cmp	r3, #0
603008d8:	03520000 	cmpeq	r2, #0
603008dc:	1a000004 	bne	603008f4 <__aeabi_uldivmod+0x20>
603008e0:	e3510000 	cmp	r1, #0
603008e4:	03500000 	cmpeq	r0, #0
603008e8:	13e01000 	mvnne	r1, #0
603008ec:	13e00000 	mvnne	r0, #0
603008f0:	ea000054 	b	60300a48 <__aeabi_idiv0>
603008f4:	e24dd008 	sub	sp, sp, #8
603008f8:	e92d6000 	push	{sp, lr}
603008fc:	eb00000b 	bl	60300930 <__udivmoddi4>
60300900:	e59de004 	ldr	lr, [sp, #4]
60300904:	e28dd008 	add	sp, sp, #8
60300908:	e8bd000c 	pop	{r2, r3}
6030090c:	e12fff1e 	bx	lr

60300910 <__ctzdi2>:
60300910:	e3500000 	cmp	r0, #0
60300914:	11a01000 	movne	r1, r0
60300918:	03a00020 	moveq	r0, #32
6030091c:	e6ff1f31 	rbit	r1, r1
60300920:	13a00000 	movne	r0, #0
60300924:	e16f1f11 	clz	r1, r1
60300928:	e0800001 	add	r0, r0, r1
6030092c:	e12fff1e 	bx	lr

60300930 <__udivmoddi4>:
60300930:	e1500002 	cmp	r0, r2
60300934:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
60300938:	e1a0e001 	mov	lr, r1
6030093c:	e0de1003 	sbcs	r1, lr, r3
60300940:	e1a0c000 	mov	ip, r0
60300944:	33a00000 	movcc	r0, #0
60300948:	e59d601c 	ldr	r6, [sp, #28]
6030094c:	31a01000 	movcc	r1, r0
60300950:	3a000036 	bcc	60300a30 <__udivmoddi4+0x100>
60300954:	e3530000 	cmp	r3, #0
60300958:	016f4f12 	clzeq	r4, r2
6030095c:	116f4f13 	clzne	r4, r3
60300960:	02844020 	addeq	r4, r4, #32
60300964:	e35e0000 	cmp	lr, #0
60300968:	016f1f1c 	clzeq	r1, ip
6030096c:	116f1f1e 	clzne	r1, lr
60300970:	02811020 	addeq	r1, r1, #32
60300974:	e0444001 	sub	r4, r4, r1
60300978:	e2447020 	sub	r7, r4, #32
6030097c:	e2645020 	rsb	r5, r4, #32
60300980:	e1a03413 	lsl	r3, r3, r4
60300984:	e1833712 	orr	r3, r3, r2, lsl r7
60300988:	e1833532 	orr	r3, r3, r2, lsr r5
6030098c:	e1a02412 	lsl	r2, r2, r4
60300990:	e15c0002 	cmp	ip, r2
60300994:	e0de1003 	sbcs	r1, lr, r3
60300998:	33a00000 	movcc	r0, #0
6030099c:	31a01000 	movcc	r1, r0
603009a0:	3a000005 	bcc	603009bc <__udivmoddi4+0x8c>
603009a4:	e3a00001 	mov	r0, #1
603009a8:	e05cc002 	subs	ip, ip, r2
603009ac:	e0cee003 	sbc	lr, lr, r3
603009b0:	e1a01710 	lsl	r1, r0, r7
603009b4:	e1811530 	orr	r1, r1, r0, lsr r5
603009b8:	e1a00410 	lsl	r0, r0, r4
603009bc:	e3540000 	cmp	r4, #0
603009c0:	0a00001a 	beq	60300a30 <__udivmoddi4+0x100>
603009c4:	e1a020a2 	lsr	r2, r2, #1
603009c8:	e1a08004 	mov	r8, r4
603009cc:	e1822f83 	orr	r2, r2, r3, lsl #31
603009d0:	e1a030a3 	lsr	r3, r3, #1
603009d4:	e15c0002 	cmp	ip, r2
603009d8:	e0de9003 	sbcs	r9, lr, r3
603009dc:	3a000016 	bcc	60300a3c <__udivmoddi4+0x10c>
603009e0:	e05cc002 	subs	ip, ip, r2
603009e4:	e0cee003 	sbc	lr, lr, r3
603009e8:	e09cc00c 	adds	ip, ip, ip
603009ec:	e0aee00e 	adc	lr, lr, lr
603009f0:	e29cc001 	adds	ip, ip, #1
603009f4:	e2aee000 	adc	lr, lr, #0
603009f8:	e2588001 	subs	r8, r8, #1
603009fc:	1afffff4 	bne	603009d4 <__udivmoddi4+0xa4>
60300a00:	e090000c 	adds	r0, r0, ip
60300a04:	e1a0c43c 	lsr	ip, ip, r4
60300a08:	e18cc51e 	orr	ip, ip, lr, lsl r5
60300a0c:	e0a1100e 	adc	r1, r1, lr
60300a10:	e18cc73e 	orr	ip, ip, lr, lsr r7
60300a14:	e1a0e43e 	lsr	lr, lr, r4
60300a18:	e1a0341e 	lsl	r3, lr, r4
60300a1c:	e183371c 	orr	r3, r3, ip, lsl r7
60300a20:	e1a0441c 	lsl	r4, ip, r4
60300a24:	e183353c 	orr	r3, r3, ip, lsr r5
60300a28:	e0500004 	subs	r0, r0, r4
60300a2c:	e0c11003 	sbc	r1, r1, r3
60300a30:	e3560000 	cmp	r6, #0
60300a34:	18865000 	stmne	r6, {ip, lr}
60300a38:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
60300a3c:	e09cc00c 	adds	ip, ip, ip
60300a40:	e0aee00e 	adc	lr, lr, lr
60300a44:	eaffffeb 	b	603009f8 <__udivmoddi4+0xc8>

60300a48 <__aeabi_idiv0>:
60300a48:	e12fff1e 	bx	lr

60300a4c <__sflush_r>:
60300a4c:	e1d130bc 	ldrh	r3, [r1, #12]
60300a50:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60300a54:	e1a05000 	mov	r5, r0
60300a58:	e1d120fc 	ldrsh	r2, [r1, #12]
60300a5c:	e1a04001 	mov	r4, r1
60300a60:	e3130008 	tst	r3, #8
60300a64:	1a000058 	bne	60300bcc <__sflush_r+0x180>
60300a68:	e5913004 	ldr	r3, [r1, #4]
60300a6c:	e3822b02 	orr	r2, r2, #2048	; 0x800
60300a70:	e1c120bc 	strh	r2, [r1, #12]
60300a74:	e3530000 	cmp	r3, #0
60300a78:	ca000004 	bgt	60300a90 <__sflush_r+0x44>
60300a7c:	e5913040 	ldr	r3, [r1, #64]	; 0x40
60300a80:	e3530000 	cmp	r3, #0
60300a84:	ca000001 	bgt	60300a90 <__sflush_r+0x44>
60300a88:	e3a00000 	mov	r0, #0
60300a8c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60300a90:	e594802c 	ldr	r8, [r4, #44]	; 0x2c
60300a94:	e3580000 	cmp	r8, #0
60300a98:	0afffffa 	beq	60300a88 <__sflush_r+0x3c>
60300a9c:	e2127a01 	ands	r7, r2, #4096	; 0x1000
60300aa0:	e5956000 	ldr	r6, [r5]
60300aa4:	e3a03000 	mov	r3, #0
60300aa8:	e5853000 	str	r3, [r5]
60300aac:	15940054 	ldrne	r0, [r4, #84]	; 0x54
60300ab0:	1a000011 	bne	60300afc <__sflush_r+0xb0>
60300ab4:	e5941020 	ldr	r1, [r4, #32]
60300ab8:	e3a03001 	mov	r3, #1
60300abc:	e1a02007 	mov	r2, r7
60300ac0:	e1a00005 	mov	r0, r5
60300ac4:	e12fff38 	blx	r8
60300ac8:	e3700001 	cmn	r0, #1
60300acc:	1a00000a 	bne	60300afc <__sflush_r+0xb0>
60300ad0:	e5953000 	ldr	r3, [r5]
60300ad4:	e3530000 	cmp	r3, #0
60300ad8:	0a000007 	beq	60300afc <__sflush_r+0xb0>
60300adc:	e3530016 	cmp	r3, #22
60300ae0:	1353001d 	cmpne	r3, #29
60300ae4:	05856000 	streq	r6, [r5]
60300ae8:	11d430bc 	ldrhne	r3, [r4, #12]
60300aec:	0affffe5 	beq	60300a88 <__sflush_r+0x3c>
60300af0:	e3833040 	orr	r3, r3, #64	; 0x40
60300af4:	e1c430bc 	strh	r3, [r4, #12]
60300af8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60300afc:	e1d430bc 	ldrh	r3, [r4, #12]
60300b00:	e3130004 	tst	r3, #4
60300b04:	0a000005 	beq	60300b20 <__sflush_r+0xd4>
60300b08:	e5943004 	ldr	r3, [r4, #4]
60300b0c:	e0400003 	sub	r0, r0, r3
60300b10:	e5943034 	ldr	r3, [r4, #52]	; 0x34
60300b14:	e3530000 	cmp	r3, #0
60300b18:	15943040 	ldrne	r3, [r4, #64]	; 0x40
60300b1c:	10400003 	subne	r0, r0, r3
60300b20:	e3a03000 	mov	r3, #0
60300b24:	e1a02000 	mov	r2, r0
60300b28:	e594702c 	ldr	r7, [r4, #44]	; 0x2c
60300b2c:	e1a00005 	mov	r0, r5
60300b30:	e5941020 	ldr	r1, [r4, #32]
60300b34:	e12fff37 	blx	r7
60300b38:	e1d430bc 	ldrh	r3, [r4, #12]
60300b3c:	e3700001 	cmn	r0, #1
60300b40:	1a000006 	bne	60300b60 <__sflush_r+0x114>
60300b44:	e5951000 	ldr	r1, [r5]
60300b48:	e351001d 	cmp	r1, #29
60300b4c:	8affffe7 	bhi	60300af0 <__sflush_r+0xa4>
60300b50:	e59f20e4 	ldr	r2, [pc, #228]	; 60300c3c <__sflush_r+0x1f0>
60300b54:	e1e02132 	mvn	r2, r2, lsr r1
60300b58:	e3120001 	tst	r2, #1
60300b5c:	1affffe3 	bne	60300af0 <__sflush_r+0xa4>
60300b60:	e3c33b02 	bic	r3, r3, #2048	; 0x800
60300b64:	e3a02000 	mov	r2, #0
60300b68:	e5842004 	str	r2, [r4, #4]
60300b6c:	e6bf3073 	sxth	r3, r3
60300b70:	e5942010 	ldr	r2, [r4, #16]
60300b74:	e3130a01 	tst	r3, #4096	; 0x1000
60300b78:	e1c430bc 	strh	r3, [r4, #12]
60300b7c:	e5842000 	str	r2, [r4]
60300b80:	0a000005 	beq	60300b9c <__sflush_r+0x150>
60300b84:	e3700001 	cmn	r0, #1
60300b88:	1a000002 	bne	60300b98 <__sflush_r+0x14c>
60300b8c:	e5953000 	ldr	r3, [r5]
60300b90:	e3530000 	cmp	r3, #0
60300b94:	1a000000 	bne	60300b9c <__sflush_r+0x150>
60300b98:	e5840054 	str	r0, [r4, #84]	; 0x54
60300b9c:	e5941034 	ldr	r1, [r4, #52]	; 0x34
60300ba0:	e5856000 	str	r6, [r5]
60300ba4:	e3510000 	cmp	r1, #0
60300ba8:	0affffb6 	beq	60300a88 <__sflush_r+0x3c>
60300bac:	e2843044 	add	r3, r4, #68	; 0x44
60300bb0:	e1510003 	cmp	r1, r3
60300bb4:	0a000001 	beq	60300bc0 <__sflush_r+0x174>
60300bb8:	e1a00005 	mov	r0, r5
60300bbc:	eb00328f 	bl	6030d600 <__wrap__free_r>
60300bc0:	e3a00000 	mov	r0, #0
60300bc4:	e5840034 	str	r0, [r4, #52]	; 0x34
60300bc8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60300bcc:	e5917010 	ldr	r7, [r1, #16]
60300bd0:	e3570000 	cmp	r7, #0
60300bd4:	0affffab 	beq	60300a88 <__sflush_r+0x3c>
60300bd8:	e3130003 	tst	r3, #3
60300bdc:	e5916000 	ldr	r6, [r1]
60300be0:	13a03000 	movne	r3, #0
60300be4:	e5817000 	str	r7, [r1]
60300be8:	e0466007 	sub	r6, r6, r7
60300bec:	05913014 	ldreq	r3, [r1, #20]
60300bf0:	e5813008 	str	r3, [r1, #8]
60300bf4:	e3560000 	cmp	r6, #0
60300bf8:	daffffa2 	ble	60300a88 <__sflush_r+0x3c>
60300bfc:	e5948028 	ldr	r8, [r4, #40]	; 0x28
60300c00:	e1a03006 	mov	r3, r6
60300c04:	e5941020 	ldr	r1, [r4, #32]
60300c08:	e1a02007 	mov	r2, r7
60300c0c:	e1a00005 	mov	r0, r5
60300c10:	e12fff38 	blx	r8
60300c14:	e3500000 	cmp	r0, #0
60300c18:	ca000004 	bgt	60300c30 <__sflush_r+0x1e4>
60300c1c:	e1d430bc 	ldrh	r3, [r4, #12]
60300c20:	e3e00000 	mvn	r0, #0
60300c24:	e3833040 	orr	r3, r3, #64	; 0x40
60300c28:	e1c430bc 	strh	r3, [r4, #12]
60300c2c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60300c30:	e0877000 	add	r7, r7, r0
60300c34:	e0466000 	sub	r6, r6, r0
60300c38:	eaffffed 	b	60300bf4 <__sflush_r+0x1a8>
60300c3c:	20400001 	.word	0x20400001

60300c40 <_fflush_r>:
60300c40:	e5913010 	ldr	r3, [r1, #16]
60300c44:	e92d4070 	push	{r4, r5, r6, lr}
60300c48:	e3530000 	cmp	r3, #0
60300c4c:	1a000002 	bne	60300c5c <_fflush_r+0x1c>
60300c50:	e3a05000 	mov	r5, #0
60300c54:	e1a00005 	mov	r0, r5
60300c58:	e8bd8070 	pop	{r4, r5, r6, pc}
60300c5c:	e1a05000 	mov	r5, r0
60300c60:	e1a04001 	mov	r4, r1
60300c64:	e3500000 	cmp	r0, #0
60300c68:	0a000003 	beq	60300c7c <_fflush_r+0x3c>
60300c6c:	e5903018 	ldr	r3, [r0, #24]
60300c70:	e3530000 	cmp	r3, #0
60300c74:	1a000000 	bne	60300c7c <_fflush_r+0x3c>
60300c78:	eb000098 	bl	60300ee0 <__sinit>
60300c7c:	e1d430fc 	ldrsh	r3, [r4, #12]
60300c80:	e3530000 	cmp	r3, #0
60300c84:	0afffff1 	beq	60300c50 <_fflush_r+0x10>
60300c88:	e5942064 	ldr	r2, [r4, #100]	; 0x64
60300c8c:	e3120001 	tst	r2, #1
60300c90:	1a000003 	bne	60300ca4 <_fflush_r+0x64>
60300c94:	e3130c02 	tst	r3, #512	; 0x200
60300c98:	1a000001 	bne	60300ca4 <_fflush_r+0x64>
60300c9c:	e5940058 	ldr	r0, [r4, #88]	; 0x58
60300ca0:	eb00015c 	bl	60301218 <__retarget_lock_acquire_recursive>
60300ca4:	e1a00005 	mov	r0, r5
60300ca8:	e1a01004 	mov	r1, r4
60300cac:	ebffff66 	bl	60300a4c <__sflush_r>
60300cb0:	e5943064 	ldr	r3, [r4, #100]	; 0x64
60300cb4:	e1a05000 	mov	r5, r0
60300cb8:	e3130001 	tst	r3, #1
60300cbc:	1affffe4 	bne	60300c54 <_fflush_r+0x14>
60300cc0:	e1d430bc 	ldrh	r3, [r4, #12]
60300cc4:	e3130c02 	tst	r3, #512	; 0x200
60300cc8:	1affffe1 	bne	60300c54 <_fflush_r+0x14>
60300ccc:	e5940058 	ldr	r0, [r4, #88]	; 0x58
60300cd0:	eb000156 	bl	60301230 <__retarget_lock_release_recursive>
60300cd4:	eaffffde 	b	60300c54 <_fflush_r+0x14>

60300cd8 <fflush>:
60300cd8:	e2501000 	subs	r1, r0, #0
60300cdc:	1a000003 	bne	60300cf0 <fflush+0x18>
60300ce0:	e59f3014 	ldr	r3, [pc, #20]	; 60300cfc <fflush+0x24>
60300ce4:	e59f1014 	ldr	r1, [pc, #20]	; 60300d00 <fflush+0x28>
60300ce8:	e5930000 	ldr	r0, [r3]
60300cec:	ea00010e 	b	6030112c <_fwalk_reent>
60300cf0:	e59f300c 	ldr	r3, [pc, #12]	; 60300d04 <fflush+0x2c>
60300cf4:	e5930000 	ldr	r0, [r3]
60300cf8:	eaffffd0 	b	60300c40 <_fflush_r>
60300cfc:	603151d8 	.word	0x603151d8
60300d00:	60300c40 	.word	0x60300c40
60300d04:	60317000 	.word	0x60317000

60300d08 <std>:
60300d08:	e92d4010 	push	{r4, lr}
60300d0c:	e3a03000 	mov	r3, #0
60300d10:	e1a04000 	mov	r4, r0
60300d14:	e5803000 	str	r3, [r0]
60300d18:	e280005c 	add	r0, r0, #92	; 0x5c
60300d1c:	e5003058 	str	r3, [r0, #-88]	; 0xffffffa8
60300d20:	e5003054 	str	r3, [r0, #-84]	; 0xffffffac
60300d24:	e14015b0 	strh	r1, [r0, #-80]	; 0xffffffb0
60300d28:	e1a01003 	mov	r1, r3
60300d2c:	e5803008 	str	r3, [r0, #8]
60300d30:	e14024be 	strh	r2, [r0, #-78]	; 0xffffffb2
60300d34:	e3a02008 	mov	r2, #8
60300d38:	e500304c 	str	r3, [r0, #-76]	; 0xffffffb4
60300d3c:	e5003048 	str	r3, [r0, #-72]	; 0xffffffb8
60300d40:	e5003044 	str	r3, [r0, #-68]	; 0xffffffbc
60300d44:	eb005027 	bl	60314de8 <__wrap_memset>
60300d48:	e59f3028 	ldr	r3, [pc, #40]	; 60300d78 <std+0x70>
60300d4c:	e5843024 	str	r3, [r4, #36]	; 0x24
60300d50:	e2840058 	add	r0, r4, #88	; 0x58
60300d54:	e59f3020 	ldr	r3, [pc, #32]	; 60300d7c <std+0x74>
60300d58:	e5843028 	str	r3, [r4, #40]	; 0x28
60300d5c:	e59f301c 	ldr	r3, [pc, #28]	; 60300d80 <std+0x78>
60300d60:	e584302c 	str	r3, [r4, #44]	; 0x2c
60300d64:	e59f3018 	ldr	r3, [pc, #24]	; 60300d84 <std+0x7c>
60300d68:	e5844020 	str	r4, [r4, #32]
60300d6c:	e5843030 	str	r3, [r4, #48]	; 0x30
60300d70:	e8bd4010 	pop	{r4, lr}
60300d74:	ea000123 	b	60301208 <__retarget_lock_init_recursive>
60300d78:	603013e0 	.word	0x603013e0
60300d7c:	60301418 	.word	0x60301418
60300d80:	6030146c 	.word	0x6030146c
60300d84:	6030149c 	.word	0x6030149c

60300d88 <_cleanup_r>:
60300d88:	e5901004 	ldr	r1, [r0, #4]
60300d8c:	e59f3050 	ldr	r3, [pc, #80]	; 60300de4 <_cleanup_r+0x5c>
60300d90:	e92d4010 	push	{r4, lr}
60300d94:	e1a04000 	mov	r4, r0
60300d98:	e1510003 	cmp	r1, r3
60300d9c:	0a000000 	beq	60300da4 <_cleanup_r+0x1c>
60300da0:	eb000671 	bl	6030276c <_fclose_r>
60300da4:	e5941008 	ldr	r1, [r4, #8]
60300da8:	e59f3038 	ldr	r3, [pc, #56]	; 60300de8 <_cleanup_r+0x60>
60300dac:	e1510003 	cmp	r1, r3
60300db0:	0a000001 	beq	60300dbc <_cleanup_r+0x34>
60300db4:	e1a00004 	mov	r0, r4
60300db8:	eb00066b 	bl	6030276c <_fclose_r>
60300dbc:	e594100c 	ldr	r1, [r4, #12]
60300dc0:	e59f3024 	ldr	r3, [pc, #36]	; 60300dec <_cleanup_r+0x64>
60300dc4:	e1510003 	cmp	r1, r3
60300dc8:	0a000001 	beq	60300dd4 <_cleanup_r+0x4c>
60300dcc:	e1a00004 	mov	r0, r4
60300dd0:	eb000665 	bl	6030276c <_fclose_r>
60300dd4:	e1a00004 	mov	r0, r4
60300dd8:	e59f1010 	ldr	r1, [pc, #16]	; 60300df0 <_cleanup_r+0x68>
60300ddc:	e8bd4010 	pop	{r4, lr}
60300de0:	ea0000d1 	b	6030112c <_fwalk_reent>
60300de4:	6031731c 	.word	0x6031731c
60300de8:	60317384 	.word	0x60317384
60300dec:	603173ec 	.word	0x603173ec
60300df0:	6030276c 	.word	0x6030276c

60300df4 <__fp_lock>:
60300df4:	e5903064 	ldr	r3, [r0, #100]	; 0x64
60300df8:	e3130001 	tst	r3, #1
60300dfc:	1a000007 	bne	60300e20 <__fp_lock+0x2c>
60300e00:	e1d030bc 	ldrh	r3, [r0, #12]
60300e04:	e3130c02 	tst	r3, #512	; 0x200
60300e08:	1a000004 	bne	60300e20 <__fp_lock+0x2c>
60300e0c:	e5900058 	ldr	r0, [r0, #88]	; 0x58
60300e10:	e92d4010 	push	{r4, lr}
60300e14:	eb0000ff 	bl	60301218 <__retarget_lock_acquire_recursive>
60300e18:	e3a00000 	mov	r0, #0
60300e1c:	e8bd8010 	pop	{r4, pc}
60300e20:	e3a00000 	mov	r0, #0
60300e24:	e12fff1e 	bx	lr

60300e28 <__fp_unlock>:
60300e28:	e5903064 	ldr	r3, [r0, #100]	; 0x64
60300e2c:	e3130001 	tst	r3, #1
60300e30:	1a000007 	bne	60300e54 <__fp_unlock+0x2c>
60300e34:	e1d030bc 	ldrh	r3, [r0, #12]
60300e38:	e3130c02 	tst	r3, #512	; 0x200
60300e3c:	1a000004 	bne	60300e54 <__fp_unlock+0x2c>
60300e40:	e5900058 	ldr	r0, [r0, #88]	; 0x58
60300e44:	e92d4010 	push	{r4, lr}
60300e48:	eb0000f8 	bl	60301230 <__retarget_lock_release_recursive>
60300e4c:	e3a00000 	mov	r0, #0
60300e50:	e8bd8010 	pop	{r4, pc}
60300e54:	e3a00000 	mov	r0, #0
60300e58:	e12fff1e 	bx	lr

60300e5c <__sfmoreglue>:
60300e5c:	e92d4070 	push	{r4, r5, r6, lr}
60300e60:	e3a02068 	mov	r2, #104	; 0x68
60300e64:	e2415001 	sub	r5, r1, #1
60300e68:	e1a06001 	mov	r6, r1
60300e6c:	e0050592 	mul	r5, r2, r5
60300e70:	e2851074 	add	r1, r5, #116	; 0x74
60300e74:	eb0031dc 	bl	6030d5ec <__wrap__malloc_r>
60300e78:	e2504000 	subs	r4, r0, #0
60300e7c:	0a000005 	beq	60300e98 <__sfmoreglue+0x3c>
60300e80:	e3a01000 	mov	r1, #0
60300e84:	e284000c 	add	r0, r4, #12
60300e88:	e2852068 	add	r2, r5, #104	; 0x68
60300e8c:	e8840042 	stm	r4, {r1, r6}
60300e90:	e5840008 	str	r0, [r4, #8]
60300e94:	eb004fd3 	bl	60314de8 <__wrap_memset>
60300e98:	e1a00004 	mov	r0, r4
60300e9c:	e8bd8070 	pop	{r4, r5, r6, pc}

60300ea0 <_cleanup>:
60300ea0:	e59f3004 	ldr	r3, [pc, #4]	; 60300eac <_cleanup+0xc>
60300ea4:	e5930000 	ldr	r0, [r3]
60300ea8:	eaffffb6 	b	60300d88 <_cleanup_r>
60300eac:	603151d8 	.word	0x603151d8

60300eb0 <__sfp_lock_acquire>:
60300eb0:	e59f0000 	ldr	r0, [pc]	; 60300eb8 <__sfp_lock_acquire+0x8>
60300eb4:	ea0000d7 	b	60301218 <__retarget_lock_acquire_recursive>
60300eb8:	6031745b 	.word	0x6031745b

60300ebc <__sfp_lock_release>:
60300ebc:	e59f0000 	ldr	r0, [pc]	; 60300ec4 <__sfp_lock_release+0x8>
60300ec0:	ea0000da 	b	60301230 <__retarget_lock_release_recursive>
60300ec4:	6031745b 	.word	0x6031745b

60300ec8 <__sinit_lock_acquire>:
60300ec8:	e59f0000 	ldr	r0, [pc]	; 60300ed0 <__sinit_lock_acquire+0x8>
60300ecc:	ea0000d1 	b	60301218 <__retarget_lock_acquire_recursive>
60300ed0:	6031745c 	.word	0x6031745c

60300ed4 <__sinit_lock_release>:
60300ed4:	e59f0000 	ldr	r0, [pc]	; 60300edc <__sinit_lock_release+0x8>
60300ed8:	ea0000d4 	b	60301230 <__retarget_lock_release_recursive>
60300edc:	6031745c 	.word	0x6031745c

60300ee0 <__sinit>:
60300ee0:	e92d4070 	push	{r4, r5, r6, lr}
60300ee4:	e1a04000 	mov	r4, r0
60300ee8:	ebfffff6 	bl	60300ec8 <__sinit_lock_acquire>
60300eec:	e5943018 	ldr	r3, [r4, #24]
60300ef0:	e3530000 	cmp	r3, #0
60300ef4:	0a000001 	beq	60300f00 <__sinit+0x20>
60300ef8:	e8bd4070 	pop	{r4, r5, r6, lr}
60300efc:	eafffff4 	b	60300ed4 <__sinit_lock_release>
60300f00:	e58430d8 	str	r3, [r4, #216]	; 0xd8
60300f04:	e58430dc 	str	r3, [r4, #220]	; 0xdc
60300f08:	e58430e0 	str	r3, [r4, #224]	; 0xe0
60300f0c:	e59f3078 	ldr	r3, [pc, #120]	; 60300f8c <__sinit+0xac>
60300f10:	e59f0078 	ldr	r0, [pc, #120]	; 60300f90 <__sinit+0xb0>
60300f14:	e59f2078 	ldr	r2, [pc, #120]	; 60300f94 <__sinit+0xb4>
60300f18:	e5842028 	str	r2, [r4, #40]	; 0x28
60300f1c:	e2806068 	add	r6, r0, #104	; 0x68
60300f20:	e5933000 	ldr	r3, [r3]
60300f24:	e28050d0 	add	r5, r0, #208	; 0xd0
60300f28:	e1530004 	cmp	r3, r4
60300f2c:	03a02001 	moveq	r2, #1
60300f30:	05832018 	streq	r2, [r3, #24]
60300f34:	e5902020 	ldr	r2, [r0, #32]
60300f38:	e5840004 	str	r0, [r4, #4]
60300f3c:	e3520000 	cmp	r2, #0
60300f40:	e5846008 	str	r6, [r4, #8]
60300f44:	e584500c 	str	r5, [r4, #12]
60300f48:	1a00000c 	bne	60300f80 <__sinit+0xa0>
60300f4c:	e58300e0 	str	r0, [r3, #224]	; 0xe0
60300f50:	e3a01003 	mov	r1, #3
60300f54:	e58310dc 	str	r1, [r3, #220]	; 0xdc
60300f58:	e3a01004 	mov	r1, #4
60300f5c:	ebffff69 	bl	60300d08 <std>
60300f60:	e3a02001 	mov	r2, #1
60300f64:	e3a01009 	mov	r1, #9
60300f68:	e1a00006 	mov	r0, r6
60300f6c:	ebffff65 	bl	60300d08 <std>
60300f70:	e3a02002 	mov	r2, #2
60300f74:	e3a01012 	mov	r1, #18
60300f78:	e1a00005 	mov	r0, r5
60300f7c:	ebffff61 	bl	60300d08 <std>
60300f80:	e3a03001 	mov	r3, #1
60300f84:	e5843018 	str	r3, [r4, #24]
60300f88:	eaffffda 	b	60300ef8 <__sinit+0x18>
60300f8c:	603151d8 	.word	0x603151d8
60300f90:	6031731c 	.word	0x6031731c
60300f94:	60300d88 	.word	0x60300d88

60300f98 <__sfp>:
60300f98:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60300f9c:	e1a07000 	mov	r7, r0
60300fa0:	ebffffc2 	bl	60300eb0 <__sfp_lock_acquire>
60300fa4:	e59f30d0 	ldr	r3, [pc, #208]	; 6030107c <__sfp+0xe4>
60300fa8:	e5936000 	ldr	r6, [r3]
60300fac:	e5963018 	ldr	r3, [r6, #24]
60300fb0:	e3530000 	cmp	r3, #0
60300fb4:	1a000001 	bne	60300fc0 <__sfp+0x28>
60300fb8:	e1a00006 	mov	r0, r6
60300fbc:	ebffffc7 	bl	60300ee0 <__sinit>
60300fc0:	e28660d8 	add	r6, r6, #216	; 0xd8
60300fc4:	e9960018 	ldmib	r6, {r3, r4}
60300fc8:	e2533001 	subs	r3, r3, #1
60300fcc:	5a000004 	bpl	60300fe4 <__sfp+0x4c>
60300fd0:	e5963000 	ldr	r3, [r6]
60300fd4:	e3530000 	cmp	r3, #0
60300fd8:	0a00001c 	beq	60301050 <__sfp+0xb8>
60300fdc:	e5966000 	ldr	r6, [r6]
60300fe0:	eafffff7 	b	60300fc4 <__sfp+0x2c>
60300fe4:	e1d450fc 	ldrsh	r5, [r4, #12]
60300fe8:	e3550000 	cmp	r5, #0
60300fec:	1a000015 	bne	60301048 <__sfp+0xb0>
60300ff0:	e59f3088 	ldr	r3, [pc, #136]	; 60301080 <__sfp+0xe8>
60300ff4:	e2840058 	add	r0, r4, #88	; 0x58
60300ff8:	e584300c 	str	r3, [r4, #12]
60300ffc:	e5845064 	str	r5, [r4, #100]	; 0x64
60301000:	eb000080 	bl	60301208 <__retarget_lock_init_recursive>
60301004:	ebffffac 	bl	60300ebc <__sfp_lock_release>
60301008:	e3a02008 	mov	r2, #8
6030100c:	e1a01005 	mov	r1, r5
60301010:	e284005c 	add	r0, r4, #92	; 0x5c
60301014:	e5845000 	str	r5, [r4]
60301018:	e5845008 	str	r5, [r4, #8]
6030101c:	e5845004 	str	r5, [r4, #4]
60301020:	e5845010 	str	r5, [r4, #16]
60301024:	e5845014 	str	r5, [r4, #20]
60301028:	e5845018 	str	r5, [r4, #24]
6030102c:	eb004f6d 	bl	60314de8 <__wrap_memset>
60301030:	e5845034 	str	r5, [r4, #52]	; 0x34
60301034:	e5845038 	str	r5, [r4, #56]	; 0x38
60301038:	e5845048 	str	r5, [r4, #72]	; 0x48
6030103c:	e584504c 	str	r5, [r4, #76]	; 0x4c
60301040:	e1a00004 	mov	r0, r4
60301044:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60301048:	e2844068 	add	r4, r4, #104	; 0x68
6030104c:	eaffffdd 	b	60300fc8 <__sfp+0x30>
60301050:	e3a01004 	mov	r1, #4
60301054:	e1a00007 	mov	r0, r7
60301058:	ebffff7f 	bl	60300e5c <__sfmoreglue>
6030105c:	e1a04000 	mov	r4, r0
60301060:	e3500000 	cmp	r0, #0
60301064:	e5860000 	str	r0, [r6]
60301068:	1affffdb 	bne	60300fdc <__sfp+0x44>
6030106c:	ebffff92 	bl	60300ebc <__sfp_lock_release>
60301070:	e3a0300c 	mov	r3, #12
60301074:	e5873000 	str	r3, [r7]
60301078:	eafffff0 	b	60301040 <__sfp+0xa8>
6030107c:	603151d8 	.word	0x603151d8
60301080:	ffff0001 	.word	0xffff0001

60301084 <__fp_lock_all>:
60301084:	e92d4010 	push	{r4, lr}
60301088:	ebffff88 	bl	60300eb0 <__sfp_lock_acquire>
6030108c:	e59f300c 	ldr	r3, [pc, #12]	; 603010a0 <__fp_lock_all+0x1c>
60301090:	e8bd4010 	pop	{r4, lr}
60301094:	e59f1008 	ldr	r1, [pc, #8]	; 603010a4 <__fp_lock_all+0x20>
60301098:	e5930000 	ldr	r0, [r3]
6030109c:	ea00000a 	b	603010cc <_fwalk>
603010a0:	60317000 	.word	0x60317000
603010a4:	60300df4 	.word	0x60300df4

603010a8 <__fp_unlock_all>:
603010a8:	e59f3014 	ldr	r3, [pc, #20]	; 603010c4 <__fp_unlock_all+0x1c>
603010ac:	e92d4010 	push	{r4, lr}
603010b0:	e59f1010 	ldr	r1, [pc, #16]	; 603010c8 <__fp_unlock_all+0x20>
603010b4:	e5930000 	ldr	r0, [r3]
603010b8:	eb000003 	bl	603010cc <_fwalk>
603010bc:	e8bd4010 	pop	{r4, lr}
603010c0:	eaffff7d 	b	60300ebc <__sfp_lock_release>
603010c4:	60317000 	.word	0x60317000
603010c8:	60300e28 	.word	0x60300e28

603010cc <_fwalk>:
603010cc:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
603010d0:	e1a07001 	mov	r7, r1
603010d4:	e28040d8 	add	r4, r0, #216	; 0xd8
603010d8:	e3a06000 	mov	r6, #0
603010dc:	e5945008 	ldr	r5, [r4, #8]
603010e0:	e5948004 	ldr	r8, [r4, #4]
603010e4:	e2588001 	subs	r8, r8, #1
603010e8:	5a000004 	bpl	60301100 <_fwalk+0x34>
603010ec:	e5944000 	ldr	r4, [r4]
603010f0:	e3540000 	cmp	r4, #0
603010f4:	1afffff8 	bne	603010dc <_fwalk+0x10>
603010f8:	e1a00006 	mov	r0, r6
603010fc:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60301100:	e1d530bc 	ldrh	r3, [r5, #12]
60301104:	e3530001 	cmp	r3, #1
60301108:	9a000005 	bls	60301124 <_fwalk+0x58>
6030110c:	e1d530fe 	ldrsh	r3, [r5, #14]
60301110:	e3730001 	cmn	r3, #1
60301114:	0a000002 	beq	60301124 <_fwalk+0x58>
60301118:	e1a00005 	mov	r0, r5
6030111c:	e12fff37 	blx	r7
60301120:	e1866000 	orr	r6, r6, r0
60301124:	e2855068 	add	r5, r5, #104	; 0x68
60301128:	eaffffed 	b	603010e4 <_fwalk+0x18>

6030112c <_fwalk_reent>:
6030112c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
60301130:	e1a06000 	mov	r6, r0
60301134:	e1a08001 	mov	r8, r1
60301138:	e28040d8 	add	r4, r0, #216	; 0xd8
6030113c:	e3a07000 	mov	r7, #0
60301140:	e5945008 	ldr	r5, [r4, #8]
60301144:	e5949004 	ldr	r9, [r4, #4]
60301148:	e2599001 	subs	r9, r9, #1
6030114c:	5a000004 	bpl	60301164 <_fwalk_reent+0x38>
60301150:	e5944000 	ldr	r4, [r4]
60301154:	e3540000 	cmp	r4, #0
60301158:	1afffff8 	bne	60301140 <_fwalk_reent+0x14>
6030115c:	e1a00007 	mov	r0, r7
60301160:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
60301164:	e1d530bc 	ldrh	r3, [r5, #12]
60301168:	e3530001 	cmp	r3, #1
6030116c:	9a000006 	bls	6030118c <_fwalk_reent+0x60>
60301170:	e1d530fe 	ldrsh	r3, [r5, #14]
60301174:	e3730001 	cmn	r3, #1
60301178:	0a000003 	beq	6030118c <_fwalk_reent+0x60>
6030117c:	e1a01005 	mov	r1, r5
60301180:	e1a00006 	mov	r0, r6
60301184:	e12fff38 	blx	r8
60301188:	e1877000 	orr	r7, r7, r0
6030118c:	e2855068 	add	r5, r5, #104	; 0x68
60301190:	eaffffec 	b	60301148 <_fwalk_reent+0x1c>

60301194 <__libc_init_array>:
60301194:	e92d4070 	push	{r4, r5, r6, lr}
60301198:	e3a06000 	mov	r6, #0
6030119c:	e59f5050 	ldr	r5, [pc, #80]	; 603011f4 <__libc_init_array+0x60>
603011a0:	e59f4050 	ldr	r4, [pc, #80]	; 603011f8 <__libc_init_array+0x64>
603011a4:	e0444005 	sub	r4, r4, r5
603011a8:	e1a04144 	asr	r4, r4, #2
603011ac:	e1560004 	cmp	r6, r4
603011b0:	1a00000b 	bne	603011e4 <__libc_init_array+0x50>
603011b4:	e59f5040 	ldr	r5, [pc, #64]	; 603011fc <__libc_init_array+0x68>
603011b8:	e3a06000 	mov	r6, #0
603011bc:	e59f403c 	ldr	r4, [pc, #60]	; 60301200 <__libc_init_array+0x6c>
603011c0:	eb002710 	bl	6030ae08 <_init>
603011c4:	e0444005 	sub	r4, r4, r5
603011c8:	e1a04144 	asr	r4, r4, #2
603011cc:	e1560004 	cmp	r6, r4
603011d0:	08bd8070 	popeq	{r4, r5, r6, pc}
603011d4:	e4953004 	ldr	r3, [r5], #4
603011d8:	e2866001 	add	r6, r6, #1
603011dc:	e12fff33 	blx	r3
603011e0:	eafffff9 	b	603011cc <__libc_init_array+0x38>
603011e4:	e4953004 	ldr	r3, [r5], #4
603011e8:	e2866001 	add	r6, r6, #1
603011ec:	e12fff33 	blx	r3
603011f0:	eaffffed 	b	603011ac <__libc_init_array+0x18>
603011f4:	60316fac 	.word	0x60316fac
603011f8:	60316fac 	.word	0x60316fac
603011fc:	60316fac 	.word	0x60316fac
60301200:	60316fb0 	.word	0x60316fb0

60301204 <__retarget_lock_init>:
60301204:	e12fff1e 	bx	lr

60301208 <__retarget_lock_init_recursive>:
60301208:	e12fff1e 	bx	lr

6030120c <__retarget_lock_close>:
6030120c:	e12fff1e 	bx	lr

60301210 <__retarget_lock_close_recursive>:
60301210:	e12fff1e 	bx	lr

60301214 <__retarget_lock_acquire>:
60301214:	e12fff1e 	bx	lr

60301218 <__retarget_lock_acquire_recursive>:
60301218:	e12fff1e 	bx	lr

6030121c <__retarget_lock_try_acquire>:
6030121c:	e3a00001 	mov	r0, #1
60301220:	e12fff1e 	bx	lr

60301224 <__retarget_lock_try_acquire_recursive>:
60301224:	e3a00001 	mov	r0, #1
60301228:	e12fff1e 	bx	lr

6030122c <__retarget_lock_release>:
6030122c:	e12fff1e 	bx	lr

60301230 <__retarget_lock_release_recursive>:
60301230:	e12fff1e 	bx	lr

60301234 <cleanup_glue>:
60301234:	e92d4070 	push	{r4, r5, r6, lr}
60301238:	e1a04001 	mov	r4, r1
6030123c:	e5911000 	ldr	r1, [r1]
60301240:	e1a05000 	mov	r5, r0
60301244:	e3510000 	cmp	r1, #0
60301248:	0a000000 	beq	60301250 <cleanup_glue+0x1c>
6030124c:	ebfffff8 	bl	60301234 <cleanup_glue>
60301250:	e1a01004 	mov	r1, r4
60301254:	e1a00005 	mov	r0, r5
60301258:	e8bd4070 	pop	{r4, r5, r6, lr}
6030125c:	ea0030e7 	b	6030d600 <__wrap__free_r>

60301260 <_reclaim_reent>:
60301260:	e59f3174 	ldr	r3, [pc, #372]	; 603013dc <_reclaim_reent+0x17c>
60301264:	e5933000 	ldr	r3, [r3]
60301268:	e1530000 	cmp	r3, r0
6030126c:	012fff1e 	bxeq	lr
60301270:	e5903024 	ldr	r3, [r0, #36]	; 0x24
60301274:	e92d4070 	push	{r4, r5, r6, lr}
60301278:	e1a04000 	mov	r4, r0
6030127c:	e3530000 	cmp	r3, #0
60301280:	0a000009 	beq	603012ac <_reclaim_reent+0x4c>
60301284:	e593300c 	ldr	r3, [r3, #12]
60301288:	e3530000 	cmp	r3, #0
6030128c:	13a05000 	movne	r5, #0
60301290:	1a000045 	bne	603013ac <_reclaim_reent+0x14c>
60301294:	e5943024 	ldr	r3, [r4, #36]	; 0x24
60301298:	e5931000 	ldr	r1, [r3]
6030129c:	e3510000 	cmp	r1, #0
603012a0:	0a000001 	beq	603012ac <_reclaim_reent+0x4c>
603012a4:	e1a00004 	mov	r0, r4
603012a8:	eb0030d4 	bl	6030d600 <__wrap__free_r>
603012ac:	e5941014 	ldr	r1, [r4, #20]
603012b0:	e3510000 	cmp	r1, #0
603012b4:	0a000001 	beq	603012c0 <_reclaim_reent+0x60>
603012b8:	e1a00004 	mov	r0, r4
603012bc:	eb0030cf 	bl	6030d600 <__wrap__free_r>
603012c0:	e5941024 	ldr	r1, [r4, #36]	; 0x24
603012c4:	e3510000 	cmp	r1, #0
603012c8:	0a000001 	beq	603012d4 <_reclaim_reent+0x74>
603012cc:	e1a00004 	mov	r0, r4
603012d0:	eb0030ca 	bl	6030d600 <__wrap__free_r>
603012d4:	e5941038 	ldr	r1, [r4, #56]	; 0x38
603012d8:	e3510000 	cmp	r1, #0
603012dc:	0a000001 	beq	603012e8 <_reclaim_reent+0x88>
603012e0:	e1a00004 	mov	r0, r4
603012e4:	eb0030c5 	bl	6030d600 <__wrap__free_r>
603012e8:	e594103c 	ldr	r1, [r4, #60]	; 0x3c
603012ec:	e3510000 	cmp	r1, #0
603012f0:	0a000001 	beq	603012fc <_reclaim_reent+0x9c>
603012f4:	e1a00004 	mov	r0, r4
603012f8:	eb0030c0 	bl	6030d600 <__wrap__free_r>
603012fc:	e5941040 	ldr	r1, [r4, #64]	; 0x40
60301300:	e3510000 	cmp	r1, #0
60301304:	0a000001 	beq	60301310 <_reclaim_reent+0xb0>
60301308:	e1a00004 	mov	r0, r4
6030130c:	eb0030bb 	bl	6030d600 <__wrap__free_r>
60301310:	e59410ec 	ldr	r1, [r4, #236]	; 0xec
60301314:	e3510000 	cmp	r1, #0
60301318:	0a000001 	beq	60301324 <_reclaim_reent+0xc4>
6030131c:	e1a00004 	mov	r0, r4
60301320:	eb0030b6 	bl	6030d600 <__wrap__free_r>
60301324:	e59410e8 	ldr	r1, [r4, #232]	; 0xe8
60301328:	e3510000 	cmp	r1, #0
6030132c:	0a000001 	beq	60301338 <_reclaim_reent+0xd8>
60301330:	e1a00004 	mov	r0, r4
60301334:	eb0030b1 	bl	6030d600 <__wrap__free_r>
60301338:	e5943048 	ldr	r3, [r4, #72]	; 0x48
6030133c:	e3530000 	cmp	r3, #0
60301340:	0a000004 	beq	60301358 <_reclaim_reent+0xf8>
60301344:	e5931088 	ldr	r1, [r3, #136]	; 0x88
60301348:	e3510000 	cmp	r1, #0
6030134c:	0a000001 	beq	60301358 <_reclaim_reent+0xf8>
60301350:	e1a00004 	mov	r0, r4
60301354:	eb0030a9 	bl	6030d600 <__wrap__free_r>
60301358:	e5941034 	ldr	r1, [r4, #52]	; 0x34
6030135c:	e3510000 	cmp	r1, #0
60301360:	0a000001 	beq	6030136c <_reclaim_reent+0x10c>
60301364:	e1a00004 	mov	r0, r4
60301368:	eb0030a4 	bl	6030d600 <__wrap__free_r>
6030136c:	e5943018 	ldr	r3, [r4, #24]
60301370:	e3530000 	cmp	r3, #0
60301374:	08bd8070 	popeq	{r4, r5, r6, pc}
60301378:	e5943028 	ldr	r3, [r4, #40]	; 0x28
6030137c:	e1a00004 	mov	r0, r4
60301380:	e12fff33 	blx	r3
60301384:	e59410d8 	ldr	r1, [r4, #216]	; 0xd8
60301388:	e3510000 	cmp	r1, #0
6030138c:	08bd8070 	popeq	{r4, r5, r6, pc}
60301390:	e1a00004 	mov	r0, r4
60301394:	e8bd4070 	pop	{r4, r5, r6, lr}
60301398:	eaffffa5 	b	60301234 <cleanup_glue>
6030139c:	e7911005 	ldr	r1, [r1, r5]
603013a0:	e3510000 	cmp	r1, #0
603013a4:	1a000007 	bne	603013c8 <_reclaim_reent+0x168>
603013a8:	e2855004 	add	r5, r5, #4
603013ac:	e5943024 	ldr	r3, [r4, #36]	; 0x24
603013b0:	e3550080 	cmp	r5, #128	; 0x80
603013b4:	e593100c 	ldr	r1, [r3, #12]
603013b8:	1afffff7 	bne	6030139c <_reclaim_reent+0x13c>
603013bc:	e1a00004 	mov	r0, r4
603013c0:	eb00308e 	bl	6030d600 <__wrap__free_r>
603013c4:	eaffffb2 	b	60301294 <_reclaim_reent+0x34>
603013c8:	e5916000 	ldr	r6, [r1]
603013cc:	e1a00004 	mov	r0, r4
603013d0:	eb00308a 	bl	6030d600 <__wrap__free_r>
603013d4:	e1a01006 	mov	r1, r6
603013d8:	eafffff0 	b	603013a0 <_reclaim_reent+0x140>
603013dc:	60317000 	.word	0x60317000

603013e0 <__sread>:
603013e0:	e92d4010 	push	{r4, lr}
603013e4:	e1a04001 	mov	r4, r1
603013e8:	e1d110fe 	ldrsh	r1, [r1, #14]
603013ec:	eb000919 	bl	60303858 <_read_r>
603013f0:	e3500000 	cmp	r0, #0
603013f4:	a5943054 	ldrge	r3, [r4, #84]	; 0x54
603013f8:	b1d430bc 	ldrhlt	r3, [r4, #12]
603013fc:	a0833000 	addge	r3, r3, r0
60301400:	a5843054 	strge	r3, [r4, #84]	; 0x54
60301404:	b3c33a01 	biclt	r3, r3, #4096	; 0x1000
60301408:	b1c430bc 	strhlt	r3, [r4, #12]
6030140c:	e8bd8010 	pop	{r4, pc}

60301410 <__seofread>:
60301410:	e3a00000 	mov	r0, #0
60301414:	e12fff1e 	bx	lr

60301418 <__swrite>:
60301418:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
6030141c:	e1a07003 	mov	r7, r3
60301420:	e1d130bc 	ldrh	r3, [r1, #12]
60301424:	e1a05000 	mov	r5, r0
60301428:	e1a04001 	mov	r4, r1
6030142c:	e1a06002 	mov	r6, r2
60301430:	e3130c01 	tst	r3, #256	; 0x100
60301434:	0a000003 	beq	60301448 <__swrite+0x30>
60301438:	e1d110fe 	ldrsh	r1, [r1, #14]
6030143c:	e3a03002 	mov	r3, #2
60301440:	e3a02000 	mov	r2, #0
60301444:	eb00055d 	bl	603029c0 <_lseek_r>
60301448:	e1d430bc 	ldrh	r3, [r4, #12]
6030144c:	e1a02006 	mov	r2, r6
60301450:	e1d410fe 	ldrsh	r1, [r4, #14]
60301454:	e1a00005 	mov	r0, r5
60301458:	e3c33a01 	bic	r3, r3, #4096	; 0x1000
6030145c:	e1c430bc 	strh	r3, [r4, #12]
60301460:	e1a03007 	mov	r3, r7
60301464:	e8bd41f0 	pop	{r4, r5, r6, r7, r8, lr}
60301468:	ea000042 	b	60301578 <_write_r>

6030146c <__sseek>:
6030146c:	e92d4010 	push	{r4, lr}
60301470:	e1a04001 	mov	r4, r1
60301474:	e1d110fe 	ldrsh	r1, [r1, #14]
60301478:	eb000550 	bl	603029c0 <_lseek_r>
6030147c:	e3700001 	cmn	r0, #1
60301480:	e1d430bc 	ldrh	r3, [r4, #12]
60301484:	15840054 	strne	r0, [r4, #84]	; 0x54
60301488:	03c33a01 	biceq	r3, r3, #4096	; 0x1000
6030148c:	13833a01 	orrne	r3, r3, #4096	; 0x1000
60301490:	01c430bc 	strheq	r3, [r4, #12]
60301494:	11c430bc 	strhne	r3, [r4, #12]
60301498:	e8bd8010 	pop	{r4, pc}

6030149c <__sclose>:
6030149c:	e1d110fe 	ldrsh	r1, [r1, #14]
603014a0:	ea00005c 	b	60301618 <_close_r>

603014a4 <strncpy>:
603014a4:	e1803001 	orr	r3, r0, r1
603014a8:	e3520003 	cmp	r2, #3
603014ac:	e92d4070 	push	{r4, r5, r6, lr}
603014b0:	93a0e000 	movls	lr, #0
603014b4:	e7e13053 	ubfx	r3, r3, #0, #2
603014b8:	83a0e001 	movhi	lr, #1
603014bc:	e3530000 	cmp	r3, #0
603014c0:	13a0e000 	movne	lr, #0
603014c4:	e35e0000 	cmp	lr, #0
603014c8:	01a03000 	moveq	r3, r0
603014cc:	0a00000e 	beq	6030150c <strncpy+0x68>
603014d0:	e59f606c 	ldr	r6, [pc, #108]	; 60301544 <strncpy+0xa0>
603014d4:	e1a0c001 	mov	ip, r1
603014d8:	e59f5068 	ldr	r5, [pc, #104]	; 60301548 <strncpy+0xa4>
603014dc:	e1a03000 	mov	r3, r0
603014e0:	e1a0100c 	mov	r1, ip
603014e4:	e49c4004 	ldr	r4, [ip], #4
603014e8:	e084e006 	add	lr, r4, r6
603014ec:	e1cee004 	bic	lr, lr, r4
603014f0:	e11e0005 	tst	lr, r5
603014f4:	1a000004 	bne	6030150c <strncpy+0x68>
603014f8:	e2422004 	sub	r2, r2, #4
603014fc:	e1a0100c 	mov	r1, ip
60301500:	e3520003 	cmp	r2, #3
60301504:	e4834004 	str	r4, [r3], #4
60301508:	8afffff4 	bhi	603014e0 <strncpy+0x3c>
6030150c:	e2411001 	sub	r1, r1, #1
60301510:	e3520000 	cmp	r2, #0
60301514:	0a000004 	beq	6030152c <strncpy+0x88>
60301518:	e5f1c001 	ldrb	ip, [r1, #1]!
6030151c:	e2422001 	sub	r2, r2, #1
60301520:	e4c3c001 	strb	ip, [r3], #1
60301524:	e35c0000 	cmp	ip, #0
60301528:	1afffff8 	bne	60301510 <strncpy+0x6c>
6030152c:	e0832002 	add	r2, r3, r2
60301530:	e3a01000 	mov	r1, #0
60301534:	e1520003 	cmp	r2, r3
60301538:	08bd8070 	popeq	{r4, r5, r6, pc}
6030153c:	e4c31001 	strb	r1, [r3], #1
60301540:	eafffffb 	b	60301534 <strncpy+0x90>
60301544:	fefefeff 	.word	0xfefefeff
60301548:	80808080 	.word	0x80808080

6030154c <vprintf>:
6030154c:	e1a03001 	mov	r3, r1
60301550:	e59f100c 	ldr	r1, [pc, #12]	; 60301564 <vprintf+0x18>
60301554:	e1a02000 	mov	r2, r0
60301558:	e5910000 	ldr	r0, [r1]
6030155c:	e5901008 	ldr	r1, [r0, #8]
60301560:	ea000d08 	b	60304988 <_vfprintf_r>
60301564:	60317000 	.word	0x60317000

60301568 <_vprintf_r>:
60301568:	e1a03002 	mov	r3, r2
6030156c:	e1a02001 	mov	r2, r1
60301570:	e5901008 	ldr	r1, [r0, #8]
60301574:	ea000d03 	b	60304988 <_vfprintf_r>

60301578 <_write_r>:
60301578:	e92d4070 	push	{r4, r5, r6, lr}
6030157c:	e1a04000 	mov	r4, r0
60301580:	e59f502c 	ldr	r5, [pc, #44]	; 603015b4 <_write_r+0x3c>
60301584:	e1a00001 	mov	r0, r1
60301588:	e1a01002 	mov	r1, r2
6030158c:	e3a02000 	mov	r2, #0
60301590:	e5852000 	str	r2, [r5]
60301594:	e1a02003 	mov	r2, r3
60301598:	eb002513 	bl	6030a9ec <_write>
6030159c:	e3700001 	cmn	r0, #1
603015a0:	18bd8070 	popne	{r4, r5, r6, pc}
603015a4:	e5953000 	ldr	r3, [r5]
603015a8:	e3530000 	cmp	r3, #0
603015ac:	15843000 	strne	r3, [r4]
603015b0:	e8bd8070 	pop	{r4, r5, r6, pc}
603015b4:	60317460 	.word	0x60317460

603015b8 <__assert_func>:
603015b8:	e92d401f 	push	{r0, r1, r2, r3, r4, lr}
603015bc:	e1a0c002 	mov	ip, r2
603015c0:	e1a02003 	mov	r2, r3
603015c4:	e59f302c 	ldr	r3, [pc, #44]	; 603015f8 <__assert_func+0x40>
603015c8:	e35c0000 	cmp	ip, #0
603015cc:	e1a0e000 	mov	lr, r0
603015d0:	e5933000 	ldr	r3, [r3]
603015d4:	e593000c 	ldr	r0, [r3, #12]
603015d8:	059f301c 	ldreq	r3, [pc, #28]	; 603015fc <__assert_func+0x44>
603015dc:	159f301c 	ldrne	r3, [pc, #28]	; 60301600 <__assert_func+0x48>
603015e0:	01a0c003 	moveq	ip, r3
603015e4:	e88d100a 	stm	sp, {r1, r3, ip}
603015e8:	e59f1014 	ldr	r1, [pc, #20]	; 60301604 <__assert_func+0x4c>
603015ec:	e1a0300e 	mov	r3, lr
603015f0:	eb0004b9 	bl	603028dc <fiprintf>
603015f4:	eb00153f 	bl	60306af8 <abort>
603015f8:	60317000 	.word	0x60317000
603015fc:	60316dce 	.word	0x60316dce
60301600:	60316d93 	.word	0x60316d93
60301604:	60316da0 	.word	0x60316da0

60301608 <__assert>:
60301608:	e1a03002 	mov	r3, r2
6030160c:	e92d4010 	push	{r4, lr}
60301610:	e3a02000 	mov	r2, #0
60301614:	ebffffe7 	bl	603015b8 <__assert_func>

60301618 <_close_r>:
60301618:	e92d4070 	push	{r4, r5, r6, lr}
6030161c:	e1a04000 	mov	r4, r0
60301620:	e59f5024 	ldr	r5, [pc, #36]	; 6030164c <_close_r+0x34>
60301624:	e1a00001 	mov	r0, r1
60301628:	e3a03000 	mov	r3, #0
6030162c:	e5853000 	str	r3, [r5]
60301630:	eb0024f9 	bl	6030aa1c <_close>
60301634:	e3700001 	cmn	r0, #1
60301638:	18bd8070 	popne	{r4, r5, r6, pc}
6030163c:	e5953000 	ldr	r3, [r5]
60301640:	e3530000 	cmp	r3, #0
60301644:	15843000 	strne	r3, [r4]
60301648:	e8bd8070 	pop	{r4, r5, r6, pc}
6030164c:	60317460 	.word	0x60317460

60301650 <quorem>:
60301650:	e5903010 	ldr	r3, [r0, #16]
60301654:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60301658:	e5914010 	ldr	r4, [r1, #16]
6030165c:	e24dd014 	sub	sp, sp, #20
60301660:	e1530004 	cmp	r3, r4
60301664:	b3a00000 	movlt	r0, #0
60301668:	ba00004d 	blt	603017a4 <quorem+0x154>
6030166c:	e2444001 	sub	r4, r4, #1
60301670:	e2806014 	add	r6, r0, #20
60301674:	e2818014 	add	r8, r1, #20
60301678:	e1a0a001 	mov	sl, r1
6030167c:	e0863104 	add	r3, r6, r4, lsl #2
60301680:	e58d3004 	str	r3, [sp, #4]
60301684:	e7963104 	ldr	r3, [r6, r4, lsl #2]
60301688:	e1a07000 	mov	r7, r0
6030168c:	e7981104 	ldr	r1, [r8, r4, lsl #2]
60301690:	e1a0b104 	lsl	fp, r4, #2
60301694:	e1a00003 	mov	r0, r3
60301698:	e58d3008 	str	r3, [sp, #8]
6030169c:	e2811001 	add	r1, r1, #1
603016a0:	e58d100c 	str	r1, [sp, #12]
603016a4:	eb0017fe 	bl	603076a4 <__udivsi3>
603016a8:	e59d3008 	ldr	r3, [sp, #8]
603016ac:	e59d100c 	ldr	r1, [sp, #12]
603016b0:	e0889104 	add	r9, r8, r4, lsl #2
603016b4:	e1a05000 	mov	r5, r0
603016b8:	e1530001 	cmp	r3, r1
603016bc:	3a00001c 	bcc	60301734 <quorem+0xe4>
603016c0:	e3a0c000 	mov	ip, #0
603016c4:	e1a01008 	mov	r1, r8
603016c8:	e1a00006 	mov	r0, r6
603016cc:	e1a0e00c 	mov	lr, ip
603016d0:	e4912004 	ldr	r2, [r1], #4
603016d4:	e1590001 	cmp	r9, r1
603016d8:	e6ff3072 	uxth	r3, r2
603016dc:	e1a02822 	lsr	r2, r2, #16
603016e0:	e023c395 	mla	r3, r5, r3, ip
603016e4:	e1a0c823 	lsr	ip, r3, #16
603016e8:	e6ff3073 	uxth	r3, r3
603016ec:	e04e3003 	sub	r3, lr, r3
603016f0:	e590e000 	ldr	lr, [r0]
603016f4:	e022c295 	mla	r2, r5, r2, ip
603016f8:	e6f3307e 	uxtah	r3, r3, lr
603016fc:	e1a0c822 	lsr	ip, r2, #16
60301700:	e6ff2072 	uxth	r2, r2
60301704:	e062282e 	rsb	r2, r2, lr, lsr #16
60301708:	e0822843 	add	r2, r2, r3, asr #16
6030170c:	e6ff3073 	uxth	r3, r3
60301710:	e1833802 	orr	r3, r3, r2, lsl #16
60301714:	e1a0e842 	asr	lr, r2, #16
60301718:	e4803004 	str	r3, [r0], #4
6030171c:	2affffeb 	bcs	603016d0 <quorem+0x80>
60301720:	e796300b 	ldr	r3, [r6, fp]
60301724:	e3530000 	cmp	r3, #0
60301728:	059d3004 	ldreq	r3, [sp, #4]
6030172c:	02433004 	subeq	r3, r3, #4
60301730:	0a000022 	beq	603017c0 <quorem+0x170>
60301734:	e1a0100a 	mov	r1, sl
60301738:	e1a00007 	mov	r0, r7
6030173c:	eb0006e4 	bl	603032d4 <__mcmp>
60301740:	e3500000 	cmp	r0, #0
60301744:	ba000015 	blt	603017a0 <quorem+0x150>
60301748:	e2855001 	add	r5, r5, #1
6030174c:	e1a00006 	mov	r0, r6
60301750:	e3a0c000 	mov	ip, #0
60301754:	e4981004 	ldr	r1, [r8], #4
60301758:	e590e000 	ldr	lr, [r0]
6030175c:	e1590008 	cmp	r9, r8
60301760:	e6ff3071 	uxth	r3, r1
60301764:	e1a02821 	lsr	r2, r1, #16
60301768:	e04c3003 	sub	r3, ip, r3
6030176c:	e062282e 	rsb	r2, r2, lr, lsr #16
60301770:	e6f3307e 	uxtah	r3, r3, lr
60301774:	e0822843 	add	r2, r2, r3, asr #16
60301778:	e6ff3073 	uxth	r3, r3
6030177c:	e1833802 	orr	r3, r3, r2, lsl #16
60301780:	e1a0c842 	asr	ip, r2, #16
60301784:	e4803004 	str	r3, [r0], #4
60301788:	2afffff1 	bcs	60301754 <quorem+0x104>
6030178c:	e7962104 	ldr	r2, [r6, r4, lsl #2]
60301790:	e0863104 	add	r3, r6, r4, lsl #2
60301794:	e3520000 	cmp	r2, #0
60301798:	02433004 	subeq	r3, r3, #4
6030179c:	0a000011 	beq	603017e8 <quorem+0x198>
603017a0:	e1a00005 	mov	r0, r5
603017a4:	e28dd014 	add	sp, sp, #20
603017a8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
603017ac:	e5922000 	ldr	r2, [r2]
603017b0:	e2433004 	sub	r3, r3, #4
603017b4:	e3520000 	cmp	r2, #0
603017b8:	1a000003 	bne	603017cc <quorem+0x17c>
603017bc:	e2444001 	sub	r4, r4, #1
603017c0:	e1a02003 	mov	r2, r3
603017c4:	e1560003 	cmp	r6, r3
603017c8:	3afffff7 	bcc	603017ac <quorem+0x15c>
603017cc:	e5874010 	str	r4, [r7, #16]
603017d0:	eaffffd7 	b	60301734 <quorem+0xe4>
603017d4:	e5922000 	ldr	r2, [r2]
603017d8:	e2433004 	sub	r3, r3, #4
603017dc:	e3520000 	cmp	r2, #0
603017e0:	1a000003 	bne	603017f4 <quorem+0x1a4>
603017e4:	e2444001 	sub	r4, r4, #1
603017e8:	e1a02003 	mov	r2, r3
603017ec:	e1560003 	cmp	r6, r3
603017f0:	3afffff7 	bcc	603017d4 <quorem+0x184>
603017f4:	e5874010 	str	r4, [r7, #16]
603017f8:	eaffffe8 	b	603017a0 <quorem+0x150>

603017fc <_dtoa_r>:
603017fc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60301800:	ec576b10 	vmov	r6, r7, d0
60301804:	e5908024 	ldr	r8, [r0, #36]	; 0x24
60301808:	ed2d8b02 	vpush	{d8}
6030180c:	e24dd044 	sub	sp, sp, #68	; 0x44
60301810:	e1a05000 	mov	r5, r0
60301814:	e3580000 	cmp	r8, #0
60301818:	e59d4070 	ldr	r4, [sp, #112]	; 0x70
6030181c:	e58d101c 	str	r1, [sp, #28]
60301820:	e58d2024 	str	r2, [sp, #36]	; 0x24
60301824:	e58d3030 	str	r3, [sp, #48]	; 0x30
60301828:	ed8d0b02 	vstr	d0, [sp, #8]
6030182c:	1a00000b 	bne	60301860 <_dtoa_r+0x64>
60301830:	e3a00010 	mov	r0, #16
60301834:	eb002f6b 	bl	6030d5e8 <__wrap_malloc>
60301838:	e3500000 	cmp	r0, #0
6030183c:	e1a02000 	mov	r2, r0
60301840:	03a010ea 	moveq	r1, #234	; 0xea
60301844:	e5850024 	str	r0, [r5, #36]	; 0x24
60301848:	059f33c0 	ldreq	r3, [pc, #960]	; 60301c10 <_dtoa_r+0x414>
6030184c:	0a0000d3 	beq	60301ba0 <_dtoa_r+0x3a4>
60301850:	e5808004 	str	r8, [r0, #4]
60301854:	e5808008 	str	r8, [r0, #8]
60301858:	e5808000 	str	r8, [r0]
6030185c:	e580800c 	str	r8, [r0, #12]
60301860:	e5953024 	ldr	r3, [r5, #36]	; 0x24
60301864:	e5931000 	ldr	r1, [r3]
60301868:	e3510000 	cmp	r1, #0
6030186c:	0a000009 	beq	60301898 <_dtoa_r+0x9c>
60301870:	e5932004 	ldr	r2, [r3, #4]
60301874:	e3a03001 	mov	r3, #1
60301878:	e5812004 	str	r2, [r1, #4]
6030187c:	e1a00005 	mov	r0, r5
60301880:	e1a03213 	lsl	r3, r3, r2
60301884:	e5813008 	str	r3, [r1, #8]
60301888:	eb0004ac 	bl	60302b40 <_Bfree>
6030188c:	e5953024 	ldr	r3, [r5, #36]	; 0x24
60301890:	e3a02000 	mov	r2, #0
60301894:	e5832000 	str	r2, [r3]
60301898:	e2573000 	subs	r3, r7, #0
6030189c:	b3c33102 	biclt	r3, r3, #-2147483648	; 0x80000000
603018a0:	b58d300c 	strlt	r3, [sp, #12]
603018a4:	a3a03000 	movge	r3, #0
603018a8:	b3a02001 	movlt	r2, #1
603018ac:	a5843000 	strge	r3, [r4]
603018b0:	b5842000 	strlt	r2, [r4]
603018b4:	e59f3358 	ldr	r3, [pc, #856]	; 60301c14 <_dtoa_r+0x418>
603018b8:	e59d400c 	ldr	r4, [sp, #12]
603018bc:	e1d33004 	bics	r3, r3, r4
603018c0:	1a000011 	bne	6030190c <_dtoa_r+0x110>
603018c4:	e59d2030 	ldr	r2, [sp, #48]	; 0x30
603018c8:	e302370f 	movw	r3, #9999	; 0x270f
603018cc:	e5823000 	str	r3, [r2]
603018d0:	e7f33054 	ubfx	r3, r4, #0, #20
603018d4:	e1933006 	orrs	r3, r3, r6
603018d8:	0a00039c 	beq	60302750 <_dtoa_r+0xf54>
603018dc:	e59d3074 	ldr	r3, [sp, #116]	; 0x74
603018e0:	e3530000 	cmp	r3, #0
603018e4:	e59f332c 	ldr	r3, [pc, #812]	; 60301c18 <_dtoa_r+0x41c>
603018e8:	e58d3004 	str	r3, [sp, #4]
603018ec:	0a000002 	beq	603018fc <_dtoa_r+0x100>
603018f0:	e2833003 	add	r3, r3, #3
603018f4:	e59d2074 	ldr	r2, [sp, #116]	; 0x74
603018f8:	e5823000 	str	r3, [r2]
603018fc:	e59d0004 	ldr	r0, [sp, #4]
60301900:	e28dd044 	add	sp, sp, #68	; 0x44
60301904:	ecbd8b02 	vpop	{d8}
60301908:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
6030190c:	ed9d8b02 	vldr	d8, [sp, #8]
60301910:	eeb58b40 	vcmp.f64	d8, #0.0
60301914:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60301918:	1a00000a 	bne	60301948 <_dtoa_r+0x14c>
6030191c:	e59d2030 	ldr	r2, [sp, #48]	; 0x30
60301920:	e3a03001 	mov	r3, #1
60301924:	e5823000 	str	r3, [r2]
60301928:	e59d3074 	ldr	r3, [sp, #116]	; 0x74
6030192c:	e3530000 	cmp	r3, #0
60301930:	059f32e4 	ldreq	r3, [pc, #740]	; 60301c1c <_dtoa_r+0x420>
60301934:	159f02e4 	ldrne	r0, [pc, #740]	; 60301c20 <_dtoa_r+0x424>
60301938:	15830000 	strne	r0, [r3]
6030193c:	12403001 	subne	r3, r0, #1
60301940:	e58d3004 	str	r3, [sp, #4]
60301944:	eaffffec 	b	603018fc <_dtoa_r+0x100>
60301948:	e28d103c 	add	r1, sp, #60	; 0x3c
6030194c:	eeb00b48 	vmov.f64	d0, d8
60301950:	e28d2038 	add	r2, sp, #56	; 0x38
60301954:	e1a00005 	mov	r0, r5
60301958:	eb00071c 	bl	603035d0 <__d2b>
6030195c:	e7ea1a54 	ubfx	r1, r4, #20, #11
60301960:	e1a0b000 	mov	fp, r0
60301964:	e3510000 	cmp	r1, #0
60301968:	e59d0038 	ldr	r0, [sp, #56]	; 0x38
6030196c:	0a000047 	beq	60301a90 <_dtoa_r+0x294>
60301970:	ee18ca90 	vmov	ip, s17
60301974:	ec532b18 	vmov	r2, r3, d8
60301978:	e2411fff 	sub	r1, r1, #1020	; 0x3fc
6030197c:	e3a06000 	mov	r6, #0
60301980:	e2411003 	sub	r1, r1, #3
60301984:	e7f3c05c 	ubfx	ip, ip, #0, #20
60301988:	e38c35ff 	orr	r3, ip, #1069547520	; 0x3fc00000
6030198c:	e3833603 	orr	r3, r3, #3145728	; 0x300000
60301990:	ec432b31 	vmov	d17, r2, r3
60301994:	eef70b08 	vmov.f64	d16, #120	; 0x3fc00000  1.5
60301998:	eddf2b96 	vldr	d18, [pc, #600]	; 60301bf8 <_dtoa_r+0x3fc>
6030199c:	ee071a90 	vmov	s15, r1
603019a0:	ee710be0 	vsub.f64	d16, d17, d16
603019a4:	eddf1b95 	vldr	d17, [pc, #596]	; 60301c00 <_dtoa_r+0x404>
603019a8:	eddf3b96 	vldr	d19, [pc, #600]	; 60301c08 <_dtoa_r+0x40c>
603019ac:	ee401ba2 	vmla.f64	d17, d16, d18
603019b0:	eef82be7 	vcvt.f64.s32	d18, s15
603019b4:	eef00b61 	vmov.f64	d16, d17
603019b8:	ee420ba3 	vmla.f64	d16, d18, d19
603019bc:	eef50bc0 	vcmpe.f64	d16, #0.0
603019c0:	eefd7be0 	vcvt.s32.f64	s15, d16
603019c4:	eef1fa10 	vmrs	APSR_nzcv, fpscr
603019c8:	ee17aa90 	vmov	sl, s15
603019cc:	5a000003 	bpl	603019e0 <_dtoa_r+0x1e4>
603019d0:	eef81be7 	vcvt.f64.s32	d17, s15
603019d4:	eef41b60 	vcmp.f64	d17, d16
603019d8:	eef1fa10 	vmrs	APSR_nzcv, fpscr
603019dc:	124aa001 	subne	sl, sl, #1
603019e0:	e35a0016 	cmp	sl, #22
603019e4:	83a03001 	movhi	r3, #1
603019e8:	8a000006 	bhi	60301a08 <_dtoa_r+0x20c>
603019ec:	e59f3238 	ldr	r3, [pc, #568]	; 60301c2c <_dtoa_r+0x430>
603019f0:	e083318a 	add	r3, r3, sl, lsl #3
603019f4:	edd30b00 	vldr	d16, [r3]
603019f8:	e3a03000 	mov	r3, #0
603019fc:	eeb48be0 	vcmpe.f64	d8, d16
60301a00:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60301a04:	424aa001 	submi	sl, sl, #1
60301a08:	e0401001 	sub	r1, r0, r1
60301a0c:	e58d302c 	str	r3, [sp, #44]	; 0x2c
60301a10:	e2513001 	subs	r3, r1, #1
60301a14:	e58d3014 	str	r3, [sp, #20]
60301a18:	42613001 	rsbmi	r3, r1, #1
60301a1c:	53a03000 	movpl	r3, #0
60301a20:	458d3010 	strmi	r3, [sp, #16]
60301a24:	43a03000 	movmi	r3, #0
60301a28:	558d3010 	strpl	r3, [sp, #16]
60301a2c:	458d3014 	strmi	r3, [sp, #20]
60301a30:	e35a0000 	cmp	sl, #0
60301a34:	ba000029 	blt	60301ae0 <_dtoa_r+0x2e4>
60301a38:	e59d3014 	ldr	r3, [sp, #20]
60301a3c:	e3a09000 	mov	r9, #0
60301a40:	e58da028 	str	sl, [sp, #40]	; 0x28
60301a44:	e083300a 	add	r3, r3, sl
60301a48:	e58d3014 	str	r3, [sp, #20]
60301a4c:	e59d301c 	ldr	r3, [sp, #28]
60301a50:	e3530009 	cmp	r3, #9
60301a54:	8a000055 	bhi	60301bb0 <_dtoa_r+0x3b4>
60301a58:	e3530005 	cmp	r3, #5
60301a5c:	c2433004 	subgt	r3, r3, #4
60301a60:	c58d301c 	strgt	r3, [sp, #28]
60301a64:	c3a04000 	movgt	r4, #0
60301a68:	d3a04001 	movle	r4, #1
60301a6c:	e59d301c 	ldr	r3, [sp, #28]
60301a70:	e2433002 	sub	r3, r3, #2
60301a74:	e3530003 	cmp	r3, #3
60301a78:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
60301a7c:	ea000055 	b	60301bd8 <_dtoa_r+0x3dc>
60301a80:	60301afc 	.word	0x60301afc
60301a84:	60301b3c 	.word	0x60301b3c
60301a88:	60301b34 	.word	0x60301b34
60301a8c:	60301ba8 	.word	0x60301ba8
60301a90:	e59d103c 	ldr	r1, [sp, #60]	; 0x3c
60301a94:	e0801001 	add	r1, r0, r1
60301a98:	e2813e43 	add	r3, r1, #1072	; 0x430
60301a9c:	e2833002 	add	r3, r3, #2
60301aa0:	e3530020 	cmp	r3, #32
60301aa4:	c2812e41 	addgt	r2, r1, #1040	; 0x410
60301aa8:	c2633040 	rsbgt	r3, r3, #64	; 0x40
60301aac:	c2822002 	addgt	r2, r2, #2
60301ab0:	d2633020 	rsble	r3, r3, #32
60301ab4:	e2411001 	sub	r1, r1, #1
60301ab8:	c1a06236 	lsrgt	r6, r6, r2
60301abc:	c1863314 	orrgt	r3, r6, r4, lsl r3
60301ac0:	d1a03316 	lslle	r3, r6, r3
60301ac4:	ee073a90 	vmov	s15, r3
60301ac8:	e3a06001 	mov	r6, #1
60301acc:	eeb87b67 	vcvt.f64.u32	d7, s15
60301ad0:	ee17ca90 	vmov	ip, s15
60301ad4:	ec532b17 	vmov	r2, r3, d7
60301ad8:	e24c361f 	sub	r3, ip, #32505856	; 0x1f00000
60301adc:	eaffffab 	b	60301990 <_dtoa_r+0x194>
60301ae0:	e59d3010 	ldr	r3, [sp, #16]
60301ae4:	e26a9000 	rsb	r9, sl, #0
60301ae8:	e043300a 	sub	r3, r3, sl
60301aec:	e58d3010 	str	r3, [sp, #16]
60301af0:	e3a03000 	mov	r3, #0
60301af4:	e58d3028 	str	r3, [sp, #40]	; 0x28
60301af8:	eaffffd3 	b	60301a4c <_dtoa_r+0x250>
60301afc:	e3a03000 	mov	r3, #0
60301b00:	e58d3018 	str	r3, [sp, #24]
60301b04:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
60301b08:	e3530000 	cmp	r3, #0
60301b0c:	c59d3024 	ldrgt	r3, [sp, #36]	; 0x24
60301b10:	c58d3020 	strgt	r3, [sp, #32]
60301b14:	c1a08003 	movgt	r8, r3
60301b18:	ca000010 	bgt	60301b60 <_dtoa_r+0x364>
60301b1c:	e3a03001 	mov	r3, #1
60301b20:	e58d3020 	str	r3, [sp, #32]
60301b24:	e1a08003 	mov	r8, r3
60301b28:	e1a02003 	mov	r2, r3
60301b2c:	e58d2024 	str	r2, [sp, #36]	; 0x24
60301b30:	ea00000a 	b	60301b60 <_dtoa_r+0x364>
60301b34:	e3a03001 	mov	r3, #1
60301b38:	eafffff0 	b	60301b00 <_dtoa_r+0x304>
60301b3c:	e3a03000 	mov	r3, #0
60301b40:	e58d3018 	str	r3, [sp, #24]
60301b44:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
60301b48:	e08a3003 	add	r3, sl, r3
60301b4c:	e58d3020 	str	r3, [sp, #32]
60301b50:	e2838001 	add	r8, r3, #1
60301b54:	e3580001 	cmp	r8, #1
60301b58:	a1a03008 	movge	r3, r8
60301b5c:	b3a03001 	movlt	r3, #1
60301b60:	e5950024 	ldr	r0, [r5, #36]	; 0x24
60301b64:	e3a02000 	mov	r2, #0
60301b68:	e5802004 	str	r2, [r0, #4]
60301b6c:	e3a02004 	mov	r2, #4
60301b70:	e282c014 	add	ip, r2, #20
60301b74:	e5901004 	ldr	r1, [r0, #4]
60301b78:	e15c0003 	cmp	ip, r3
60301b7c:	9a000018 	bls	60301be4 <_dtoa_r+0x3e8>
60301b80:	e1a00005 	mov	r0, r5
60301b84:	eb0003b4 	bl	60302a5c <_Balloc>
60301b88:	e2503000 	subs	r3, r0, #0
60301b8c:	e58d3004 	str	r3, [sp, #4]
60301b90:	1a000028 	bne	60301c38 <_dtoa_r+0x43c>
60301b94:	e59f3094 	ldr	r3, [pc, #148]	; 60301c30 <_dtoa_r+0x434>
60301b98:	e1a02000 	mov	r2, r0
60301b9c:	e30011aa 	movw	r1, #426	; 0x1aa
60301ba0:	e59f007c 	ldr	r0, [pc, #124]	; 60301c24 <_dtoa_r+0x428>
60301ba4:	ebfffe83 	bl	603015b8 <__assert_func>
60301ba8:	e3a03001 	mov	r3, #1
60301bac:	eaffffe3 	b	60301b40 <_dtoa_r+0x344>
60301bb0:	e3a04001 	mov	r4, #1
60301bb4:	e3a03000 	mov	r3, #0
60301bb8:	e58d4018 	str	r4, [sp, #24]
60301bbc:	e58d301c 	str	r3, [sp, #28]
60301bc0:	e3e03000 	mvn	r3, #0
60301bc4:	e3a02000 	mov	r2, #0
60301bc8:	e1a08003 	mov	r8, r3
60301bcc:	e58d3020 	str	r3, [sp, #32]
60301bd0:	e3a03012 	mov	r3, #18
60301bd4:	eaffffd4 	b	60301b2c <_dtoa_r+0x330>
60301bd8:	e3a03001 	mov	r3, #1
60301bdc:	e58d3018 	str	r3, [sp, #24]
60301be0:	eafffff6 	b	60301bc0 <_dtoa_r+0x3c4>
60301be4:	e2811001 	add	r1, r1, #1
60301be8:	e1a02082 	lsl	r2, r2, #1
60301bec:	e5801004 	str	r1, [r0, #4]
60301bf0:	eaffffde 	b	60301b70 <_dtoa_r+0x374>
60301bf4:	e320f000 	nop	{0}
60301bf8:	636f4361 	.word	0x636f4361
60301bfc:	3fd287a7 	.word	0x3fd287a7
60301c00:	8b60c8b3 	.word	0x8b60c8b3
60301c04:	3fc68a28 	.word	0x3fc68a28
60301c08:	509f79fb 	.word	0x509f79fb
60301c0c:	3fd34413 	.word	0x3fd34413
60301c10:	60316dde 	.word	0x60316dde
60301c14:	7ff00000 	.word	0x7ff00000
60301c18:	60316dd8 	.word	0x60316dd8
60301c1c:	60316ddc 	.word	0x60316ddc
60301c20:	60316ddd 	.word	0x60316ddd
60301c24:	60316df5 	.word	0x60316df5
60301c28:	603152e0 	.word	0x603152e0
60301c2c:	603151f0 	.word	0x603151f0
60301c30:	60316e3c 	.word	0x60316e3c
60301c34:	60316dcf 	.word	0x60316dcf
60301c38:	e5953024 	ldr	r3, [r5, #36]	; 0x24
60301c3c:	e358000e 	cmp	r8, #14
60301c40:	83a04000 	movhi	r4, #0
60301c44:	92044001 	andls	r4, r4, #1
60301c48:	e59d2004 	ldr	r2, [sp, #4]
60301c4c:	e3540000 	cmp	r4, #0
60301c50:	e5832000 	str	r2, [r3]
60301c54:	0a00003c 	beq	60301d4c <_dtoa_r+0x550>
60301c58:	e35a0000 	cmp	sl, #0
60301c5c:	da000063 	ble	60301df0 <_dtoa_r+0x5f4>
60301c60:	e31a0c01 	tst	sl, #256	; 0x100
60301c64:	e51f2040 	ldr	r2, [pc, #-64]	; 60301c2c <_dtoa_r+0x430>
60301c68:	e20a300f 	and	r3, sl, #15
60301c6c:	e51f104c 	ldr	r1, [pc, #-76]	; 60301c28 <_dtoa_r+0x42c>
60301c70:	e0823183 	add	r3, r2, r3, lsl #3
60301c74:	151f2054 	ldrne	r2, [pc, #-84]	; 60301c28 <_dtoa_r+0x42c>
60301c78:	03a02002 	moveq	r2, #2
60301c7c:	edd30b00 	vldr	d16, [r3]
60301c80:	e1a0324a 	asr	r3, sl, #4
60301c84:	1dd21b08 	vldrne	d17, [r2, #32]
60301c88:	1203300f 	andne	r3, r3, #15
60301c8c:	13a02003 	movne	r2, #3
60301c90:	1ec81b21 	vdivne.f64	d17, d8, d17
60301c94:	1dcd1b02 	vstrne	d17, [sp, #8]
60301c98:	e3530000 	cmp	r3, #0
60301c9c:	1a00004c 	bne	60301dd4 <_dtoa_r+0x5d8>
60301ca0:	eddd1b02 	vldr	d17, [sp, #8]
60301ca4:	eec10ba0 	vdiv.f64	d16, d17, d16
60301ca8:	edcd0b02 	vstr	d16, [sp, #8]
60301cac:	e59d302c 	ldr	r3, [sp, #44]	; 0x2c
60301cb0:	e3530000 	cmp	r3, #0
60301cb4:	0a000067 	beq	60301e58 <_dtoa_r+0x65c>
60301cb8:	eddd0b02 	vldr	d16, [sp, #8]
60301cbc:	eef71b00 	vmov.f64	d17, #112	; 0x3f800000  1.0
60301cc0:	eef40be1 	vcmpe.f64	d16, d17
60301cc4:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60301cc8:	5a000062 	bpl	60301e58 <_dtoa_r+0x65c>
60301ccc:	e3580000 	cmp	r8, #0
60301cd0:	0a000060 	beq	60301e58 <_dtoa_r+0x65c>
60301cd4:	e59d3020 	ldr	r3, [sp, #32]
60301cd8:	e3530000 	cmp	r3, #0
60301cdc:	da000019 	ble	60301d48 <_dtoa_r+0x54c>
60301ce0:	eef21b04 	vmov.f64	d17, #36	; 0x41200000  10.0
60301ce4:	e24a1001 	sub	r1, sl, #1
60301ce8:	e2822001 	add	r2, r2, #1
60301cec:	e1a00003 	mov	r0, r3
60301cf0:	ee600ba1 	vmul.f64	d16, d16, d17
60301cf4:	edcd0b02 	vstr	d16, [sp, #8]
60301cf8:	ee072a90 	vmov	s15, r2
60301cfc:	eddd0b02 	vldr	d16, [sp, #8]
60301d00:	e3500000 	cmp	r0, #0
60301d04:	eef81be7 	vcvt.f64.s32	d17, s15
60301d08:	eeb17b0c 	vmov.f64	d7, #28	; 0x40e00000  7.0
60301d0c:	ee017ba0 	vmla.f64	d7, d17, d16
60301d10:	ee17ca90 	vmov	ip, s15
60301d14:	ec532b17 	vmov	r2, r3, d7
60301d18:	e24c350d 	sub	r3, ip, #54525952	; 0x3400000
60301d1c:	1a000050 	bne	60301e64 <_dtoa_r+0x668>
60301d20:	eef11b04 	vmov.f64	d17, #20	; 0x40a00000  5.0
60301d24:	ee700be1 	vsub.f64	d16, d16, d17
60301d28:	ec432b31 	vmov	d17, r2, r3
60301d2c:	eef40be1 	vcmpe.f64	d16, d17
60301d30:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60301d34:	ca0001ab 	bgt	603023e8 <_dtoa_r+0xbec>
60301d38:	eef11b61 	vneg.f64	d17, d17
60301d3c:	eef40be1 	vcmpe.f64	d16, d17
60301d40:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60301d44:	4a0001a4 	bmi	603023dc <_dtoa_r+0xbe0>
60301d48:	ed8d8b02 	vstr	d8, [sp, #8]
60301d4c:	e59d303c 	ldr	r3, [sp, #60]	; 0x3c
60301d50:	e35a000e 	cmp	sl, #14
60301d54:	e1e04003 	mvn	r4, r3
60301d58:	e1a04fa4 	lsr	r4, r4, #31
60301d5c:	c3a04000 	movgt	r4, #0
60301d60:	e3540000 	cmp	r4, #0
60301d64:	0a0000b2 	beq	60302034 <_dtoa_r+0x838>
60301d68:	e51f3144 	ldr	r3, [pc, #-324]	; 60301c2c <_dtoa_r+0x430>
60301d6c:	e3580000 	cmp	r8, #0
60301d70:	e59d2024 	ldr	r2, [sp, #36]	; 0x24
60301d74:	e083318a 	add	r3, r3, sl, lsl #3
60301d78:	edd31b00 	vldr	d17, [r3]
60301d7c:	c3a03000 	movgt	r3, #0
60301d80:	d3a03001 	movle	r3, #1
60301d84:	e0133fa2 	ands	r3, r3, r2, lsr #31
60301d88:	059d7004 	ldreq	r7, [sp, #4]
60301d8c:	0ef23b04 	vmoveq.f64	d19, #36	; 0x41200000  10.0
60301d90:	0ddd0b02 	vldreq	d16, [sp, #8]
60301d94:	0a000086 	beq	60301fb4 <_dtoa_r+0x7b8>
60301d98:	e3580000 	cmp	r8, #0
60301d9c:	1a00018e 	bne	603023dc <_dtoa_r+0xbe0>
60301da0:	eef10b04 	vmov.f64	d16, #20	; 0x40a00000  5.0
60301da4:	e1a04008 	mov	r4, r8
60301da8:	e1a06008 	mov	r6, r8
60301dac:	ee611ba0 	vmul.f64	d17, d17, d16
60301db0:	eddd0b02 	vldr	d16, [sp, #8]
60301db4:	eef41be0 	vcmpe.f64	d17, d16
60301db8:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60301dbc:	aa00015d 	bge	60302338 <_dtoa_r+0xb3c>
60301dc0:	e59d7004 	ldr	r7, [sp, #4]
60301dc4:	e3a03031 	mov	r3, #49	; 0x31
60301dc8:	e28aa001 	add	sl, sl, #1
60301dcc:	e4c73001 	strb	r3, [r7], #1
60301dd0:	ea00015b 	b	60302344 <_dtoa_r+0xb48>
60301dd4:	e3130001 	tst	r3, #1
60301dd8:	e2811008 	add	r1, r1, #8
60301ddc:	12822001 	addne	r2, r2, #1
60301de0:	e1a030c3 	asr	r3, r3, #1
60301de4:	1d511b02 	vldrne	d17, [r1, #-8]
60301de8:	1e600ba1 	vmulne.f64	d16, d16, d17
60301dec:	eaffffa9 	b	60301c98 <_dtoa_r+0x49c>
60301df0:	03a02002 	moveq	r2, #2
60301df4:	0affffac 	beq	60301cac <_dtoa_r+0x4b0>
60301df8:	e26a3000 	rsb	r3, sl, #0
60301dfc:	e51f21d8 	ldr	r2, [pc, #-472]	; 60301c2c <_dtoa_r+0x430>
60301e00:	e203100f 	and	r1, r3, #15
60301e04:	e3a00000 	mov	r0, #0
60301e08:	e1a03243 	asr	r3, r3, #4
60301e0c:	e0822181 	add	r2, r2, r1, lsl #3
60301e10:	e51f11f0 	ldr	r1, [pc, #-496]	; 60301c28 <_dtoa_r+0x42c>
60301e14:	edd20b00 	vldr	d16, [r2]
60301e18:	e3a02002 	mov	r2, #2
60301e1c:	ee680b20 	vmul.f64	d16, d8, d16
60301e20:	edcd0b02 	vstr	d16, [sp, #8]
60301e24:	e3530000 	cmp	r3, #0
60301e28:	1a000002 	bne	60301e38 <_dtoa_r+0x63c>
60301e2c:	e3500000 	cmp	r0, #0
60301e30:	1dcd0b02 	vstrne	d16, [sp, #8]
60301e34:	eaffff9c 	b	60301cac <_dtoa_r+0x4b0>
60301e38:	e3130001 	tst	r3, #1
60301e3c:	e2811008 	add	r1, r1, #8
60301e40:	12822001 	addne	r2, r2, #1
60301e44:	11a00004 	movne	r0, r4
60301e48:	e1a030c3 	asr	r3, r3, #1
60301e4c:	1d511b02 	vldrne	d17, [r1, #-8]
60301e50:	1e600ba1 	vmulne.f64	d16, d16, d17
60301e54:	eafffff2 	b	60301e24 <_dtoa_r+0x628>
60301e58:	e1a0100a 	mov	r1, sl
60301e5c:	e1a00008 	mov	r0, r8
60301e60:	eaffffa4 	b	60301cf8 <_dtoa_r+0x4fc>
60301e64:	ec432b31 	vmov	d17, r2, r3
60301e68:	e51f3244 	ldr	r3, [pc, #-580]	; 60301c2c <_dtoa_r+0x430>
60301e6c:	e0833180 	add	r3, r3, r0, lsl #3
60301e70:	ed533b02 	vldr	d19, [r3, #-8]
60301e74:	e59d3018 	ldr	r3, [sp, #24]
60301e78:	e3530000 	cmp	r3, #0
60301e7c:	e59d3004 	ldr	r3, [sp, #4]
60301e80:	e0834000 	add	r4, r3, r0
60301e84:	0a000017 	beq	60301ee8 <_dtoa_r+0x6ec>
60301e88:	eef64b00 	vmov.f64	d20, #96	; 0x3f000000  0.5
60301e8c:	e1a07003 	mov	r7, r3
60301e90:	eec42ba3 	vdiv.f64	d18, d20, d19
60301e94:	eef73b00 	vmov.f64	d19, #112	; 0x3f800000  1.0
60301e98:	ee721be1 	vsub.f64	d17, d18, d17
60301e9c:	eef22b04 	vmov.f64	d18, #36	; 0x41200000  10.0
60301ea0:	eefd7be0 	vcvt.s32.f64	s15, d16
60301ea4:	eef84be7 	vcvt.f64.s32	d20, s15
60301ea8:	ee173a90 	vmov	r3, s15
60301eac:	ee700be4 	vsub.f64	d16, d16, d20
60301eb0:	e2833030 	add	r3, r3, #48	; 0x30
60301eb4:	e4c73001 	strb	r3, [r7], #1
60301eb8:	eef40be1 	vcmpe.f64	d16, d17
60301ebc:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60301ec0:	4a000022 	bmi	60301f50 <_dtoa_r+0x754>
60301ec4:	ee734be0 	vsub.f64	d20, d19, d16
60301ec8:	eef44be1 	vcmpe.f64	d20, d17
60301ecc:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60301ed0:	4a000023 	bmi	60301f64 <_dtoa_r+0x768>
60301ed4:	e1570004 	cmp	r7, r4
60301ed8:	0affff9a 	beq	60301d48 <_dtoa_r+0x54c>
60301edc:	ee611ba2 	vmul.f64	d17, d17, d18
60301ee0:	ee600ba2 	vmul.f64	d16, d16, d18
60301ee4:	eaffffed 	b	60301ea0 <_dtoa_r+0x6a4>
60301ee8:	ee611ba3 	vmul.f64	d17, d17, d19
60301eec:	e59d7004 	ldr	r7, [sp, #4]
60301ef0:	e1a02004 	mov	r2, r4
60301ef4:	eef22b04 	vmov.f64	d18, #36	; 0x41200000  10.0
60301ef8:	eefd7be0 	vcvt.s32.f64	s15, d16
60301efc:	ee173a90 	vmov	r3, s15
60301f00:	eef83be7 	vcvt.f64.s32	d19, s15
60301f04:	e2833030 	add	r3, r3, #48	; 0x30
60301f08:	e4c73001 	strb	r3, [r7], #1
60301f0c:	ee700be3 	vsub.f64	d16, d16, d19
60301f10:	e1570004 	cmp	r7, r4
60301f14:	1a00000f 	bne	60301f58 <_dtoa_r+0x75c>
60301f18:	eef62b00 	vmov.f64	d18, #96	; 0x3f000000  0.5
60301f1c:	ee713ba2 	vadd.f64	d19, d17, d18
60301f20:	eef40be3 	vcmpe.f64	d16, d19
60301f24:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60301f28:	ca00000d 	bgt	60301f64 <_dtoa_r+0x768>
60301f2c:	ee721be1 	vsub.f64	d17, d18, d17
60301f30:	eef40be1 	vcmpe.f64	d16, d17
60301f34:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60301f38:	5affff82 	bpl	60301d48 <_dtoa_r+0x54c>
60301f3c:	e1a07002 	mov	r7, r2
60301f40:	e2422001 	sub	r2, r2, #1
60301f44:	e5573001 	ldrb	r3, [r7, #-1]
60301f48:	e3530030 	cmp	r3, #48	; 0x30
60301f4c:	0afffffa 	beq	60301f3c <_dtoa_r+0x740>
60301f50:	e1a0a001 	mov	sl, r1
60301f54:	ea00002a 	b	60302004 <_dtoa_r+0x808>
60301f58:	ee600ba2 	vmul.f64	d16, d16, d18
60301f5c:	eaffffe5 	b	60301ef8 <_dtoa_r+0x6fc>
60301f60:	e1a0100a 	mov	r1, sl
60301f64:	e1a03007 	mov	r3, r7
60301f68:	e1a07003 	mov	r7, r3
60301f6c:	e5732001 	ldrb	r2, [r3, #-1]!
60301f70:	e3520039 	cmp	r2, #57	; 0x39
60301f74:	1a000006 	bne	60301f94 <_dtoa_r+0x798>
60301f78:	e59d2004 	ldr	r2, [sp, #4]
60301f7c:	e1520003 	cmp	r2, r3
60301f80:	1afffff8 	bne	60301f68 <_dtoa_r+0x76c>
60301f84:	e59d0004 	ldr	r0, [sp, #4]
60301f88:	e2811001 	add	r1, r1, #1
60301f8c:	e3a02030 	mov	r2, #48	; 0x30
60301f90:	e5c02000 	strb	r2, [r0]
60301f94:	e5d32000 	ldrb	r2, [r3]
60301f98:	e2822001 	add	r2, r2, #1
60301f9c:	e5c32000 	strb	r2, [r3]
60301fa0:	eaffffea 	b	60301f50 <_dtoa_r+0x754>
60301fa4:	ee600ba3 	vmul.f64	d16, d16, d19
60301fa8:	eef50b40 	vcmp.f64	d16, #0.0
60301fac:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60301fb0:	0a000013 	beq	60302004 <_dtoa_r+0x808>
60301fb4:	eec02ba1 	vdiv.f64	d18, d16, d17
60301fb8:	eefd7be2 	vcvt.s32.f64	s15, d18
60301fbc:	ee173a90 	vmov	r3, s15
60301fc0:	eef82be7 	vcvt.f64.s32	d18, s15
60301fc4:	e2832030 	add	r2, r3, #48	; 0x30
60301fc8:	e4c72001 	strb	r2, [r7], #1
60301fcc:	e59d2004 	ldr	r2, [sp, #4]
60301fd0:	ee420be1 	vmls.f64	d16, d18, d17
60301fd4:	e0472002 	sub	r2, r7, r2
60301fd8:	e1580002 	cmp	r8, r2
60301fdc:	1afffff0 	bne	60301fa4 <_dtoa_r+0x7a8>
60301fe0:	ee700ba0 	vadd.f64	d16, d16, d16
60301fe4:	eef40be1 	vcmpe.f64	d16, d17
60301fe8:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60301fec:	caffffdb 	bgt	60301f60 <_dtoa_r+0x764>
60301ff0:	eef40b61 	vcmp.f64	d16, d17
60301ff4:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60301ff8:	1a000001 	bne	60302004 <_dtoa_r+0x808>
60301ffc:	e3130001 	tst	r3, #1
60302000:	1affffd6 	bne	60301f60 <_dtoa_r+0x764>
60302004:	e1a0100b 	mov	r1, fp
60302008:	e1a00005 	mov	r0, r5
6030200c:	eb0002cb 	bl	60302b40 <_Bfree>
60302010:	e59d2030 	ldr	r2, [sp, #48]	; 0x30
60302014:	e3a03000 	mov	r3, #0
60302018:	e5c73000 	strb	r3, [r7]
6030201c:	e28a3001 	add	r3, sl, #1
60302020:	e5823000 	str	r3, [r2]
60302024:	e59d3074 	ldr	r3, [sp, #116]	; 0x74
60302028:	e3530000 	cmp	r3, #0
6030202c:	15837000 	strne	r7, [r3]
60302030:	eafffe31 	b	603018fc <_dtoa_r+0x100>
60302034:	e59d2018 	ldr	r2, [sp, #24]
60302038:	e3520000 	cmp	r2, #0
6030203c:	01a04009 	moveq	r4, r9
60302040:	059d7010 	ldreq	r7, [sp, #16]
60302044:	059d6018 	ldreq	r6, [sp, #24]
60302048:	0a000013 	beq	6030209c <_dtoa_r+0x8a0>
6030204c:	e59d201c 	ldr	r2, [sp, #28]
60302050:	e3520001 	cmp	r2, #1
60302054:	ca0000ca 	bgt	60302384 <_dtoa_r+0xb88>
60302058:	e3560000 	cmp	r6, #0
6030205c:	e59d7010 	ldr	r7, [sp, #16]
60302060:	12833e43 	addne	r3, r3, #1072	; 0x430
60302064:	e1a04009 	mov	r4, r9
60302068:	12833003 	addne	r3, r3, #3
6030206c:	059d3038 	ldreq	r3, [sp, #56]	; 0x38
60302070:	02633036 	rsbeq	r3, r3, #54	; 0x36
60302074:	e59d2010 	ldr	r2, [sp, #16]
60302078:	e3a01001 	mov	r1, #1
6030207c:	e1a00005 	mov	r0, r5
60302080:	e0822003 	add	r2, r2, r3
60302084:	e58d2010 	str	r2, [sp, #16]
60302088:	e59d2014 	ldr	r2, [sp, #20]
6030208c:	e0823003 	add	r3, r2, r3
60302090:	e58d3014 	str	r3, [sp, #20]
60302094:	eb000378 	bl	60302e7c <__i2b>
60302098:	e1a06000 	mov	r6, r0
6030209c:	e59d3014 	ldr	r3, [sp, #20]
603020a0:	e3570000 	cmp	r7, #0
603020a4:	c3530000 	cmpgt	r3, #0
603020a8:	da000008 	ble	603020d0 <_dtoa_r+0x8d4>
603020ac:	e1570003 	cmp	r7, r3
603020b0:	e59d2010 	ldr	r2, [sp, #16]
603020b4:	b1a03007 	movlt	r3, r7
603020b8:	e0422003 	sub	r2, r2, r3
603020bc:	e58d2010 	str	r2, [sp, #16]
603020c0:	e59d2014 	ldr	r2, [sp, #20]
603020c4:	e0477003 	sub	r7, r7, r3
603020c8:	e0423003 	sub	r3, r2, r3
603020cc:	e58d3014 	str	r3, [sp, #20]
603020d0:	e3590000 	cmp	r9, #0
603020d4:	0a000019 	beq	60302140 <_dtoa_r+0x944>
603020d8:	e59d3018 	ldr	r3, [sp, #24]
603020dc:	e3530000 	cmp	r3, #0
603020e0:	0a0000b6 	beq	603023c0 <_dtoa_r+0xbc4>
603020e4:	e3540000 	cmp	r4, #0
603020e8:	da00000e 	ble	60302128 <_dtoa_r+0x92c>
603020ec:	e1a01006 	mov	r1, r6
603020f0:	e1a02004 	mov	r2, r4
603020f4:	e1a00005 	mov	r0, r5
603020f8:	eb0003e2 	bl	60303088 <__pow5mult>
603020fc:	e1a0200b 	mov	r2, fp
60302100:	e1a01000 	mov	r1, r0
60302104:	e1a06000 	mov	r6, r0
60302108:	e1a00005 	mov	r0, r5
6030210c:	eb00036a 	bl	60302ebc <__multiply>
60302110:	e1a0100b 	mov	r1, fp
60302114:	e58d0034 	str	r0, [sp, #52]	; 0x34
60302118:	e1a00005 	mov	r0, r5
6030211c:	eb000287 	bl	60302b40 <_Bfree>
60302120:	e59d3034 	ldr	r3, [sp, #52]	; 0x34
60302124:	e1a0b003 	mov	fp, r3
60302128:	e0592004 	subs	r2, r9, r4
6030212c:	0a000003 	beq	60302140 <_dtoa_r+0x944>
60302130:	e1a0100b 	mov	r1, fp
60302134:	e1a00005 	mov	r0, r5
60302138:	eb0003d2 	bl	60303088 <__pow5mult>
6030213c:	e1a0b000 	mov	fp, r0
60302140:	e3a01001 	mov	r1, #1
60302144:	e1a00005 	mov	r0, r5
60302148:	eb00034b 	bl	60302e7c <__i2b>
6030214c:	e59d3028 	ldr	r3, [sp, #40]	; 0x28
60302150:	e1a04000 	mov	r4, r0
60302154:	e3530000 	cmp	r3, #0
60302158:	da00009a 	ble	603023c8 <_dtoa_r+0xbcc>
6030215c:	e1a02003 	mov	r2, r3
60302160:	e1a01000 	mov	r1, r0
60302164:	e1a00005 	mov	r0, r5
60302168:	eb0003c6 	bl	60303088 <__pow5mult>
6030216c:	e59d301c 	ldr	r3, [sp, #28]
60302170:	e1a04000 	mov	r4, r0
60302174:	e3530001 	cmp	r3, #1
60302178:	c3a09000 	movgt	r9, #0
6030217c:	ca000018 	bgt	603021e4 <_dtoa_r+0x9e8>
60302180:	e59d3008 	ldr	r3, [sp, #8]
60302184:	e3530000 	cmp	r3, #0
60302188:	1a000091 	bne	603023d4 <_dtoa_r+0xbd8>
6030218c:	e59d300c 	ldr	r3, [sp, #12]
60302190:	e7f33053 	ubfx	r3, r3, #0, #20
60302194:	e3530000 	cmp	r3, #0
60302198:	159d9008 	ldrne	r9, [sp, #8]
6030219c:	1a00000c 	bne	603021d4 <_dtoa_r+0x9d8>
603021a0:	e59d300c 	ldr	r3, [sp, #12]
603021a4:	e3c39102 	bic	r9, r3, #-2147483648	; 0x80000000
603021a8:	e1a09a29 	lsr	r9, r9, #20
603021ac:	e1a09a09 	lsl	r9, r9, #20
603021b0:	e3590000 	cmp	r9, #0
603021b4:	0a000006 	beq	603021d4 <_dtoa_r+0x9d8>
603021b8:	e59d3010 	ldr	r3, [sp, #16]
603021bc:	e3a09001 	mov	r9, #1
603021c0:	e2833001 	add	r3, r3, #1
603021c4:	e58d3010 	str	r3, [sp, #16]
603021c8:	e59d3014 	ldr	r3, [sp, #20]
603021cc:	e2833001 	add	r3, r3, #1
603021d0:	e58d3014 	str	r3, [sp, #20]
603021d4:	e59d3028 	ldr	r3, [sp, #40]	; 0x28
603021d8:	e3530000 	cmp	r3, #0
603021dc:	03a00001 	moveq	r0, #1
603021e0:	0a000004 	beq	603021f8 <_dtoa_r+0x9fc>
603021e4:	e5943010 	ldr	r3, [r4, #16]
603021e8:	e0843103 	add	r3, r4, r3, lsl #2
603021ec:	e5930010 	ldr	r0, [r3, #16]
603021f0:	eb0002e4 	bl	60302d88 <__hi0bits>
603021f4:	e2600020 	rsb	r0, r0, #32
603021f8:	e59d3014 	ldr	r3, [sp, #20]
603021fc:	e0830000 	add	r0, r3, r0
60302200:	e210001f 	ands	r0, r0, #31
60302204:	0a000005 	beq	60302220 <_dtoa_r+0xa24>
60302208:	e2603020 	rsb	r3, r0, #32
6030220c:	e3530004 	cmp	r3, #4
60302210:	c260001c 	rsbgt	r0, r0, #28
60302214:	ca000002 	bgt	60302224 <_dtoa_r+0xa28>
60302218:	0a000008 	beq	60302240 <_dtoa_r+0xa44>
6030221c:	e1a00003 	mov	r0, r3
60302220:	e280001c 	add	r0, r0, #28
60302224:	e59d3010 	ldr	r3, [sp, #16]
60302228:	e0877000 	add	r7, r7, r0
6030222c:	e0833000 	add	r3, r3, r0
60302230:	e58d3010 	str	r3, [sp, #16]
60302234:	e59d3014 	ldr	r3, [sp, #20]
60302238:	e0833000 	add	r3, r3, r0
6030223c:	e58d3014 	str	r3, [sp, #20]
60302240:	e59d3010 	ldr	r3, [sp, #16]
60302244:	e3530000 	cmp	r3, #0
60302248:	da000004 	ble	60302260 <_dtoa_r+0xa64>
6030224c:	e1a0100b 	mov	r1, fp
60302250:	e1a02003 	mov	r2, r3
60302254:	e1a00005 	mov	r0, r5
60302258:	eb0003d2 	bl	603031a8 <__lshift>
6030225c:	e1a0b000 	mov	fp, r0
60302260:	e59d3014 	ldr	r3, [sp, #20]
60302264:	e3530000 	cmp	r3, #0
60302268:	da000004 	ble	60302280 <_dtoa_r+0xa84>
6030226c:	e1a01004 	mov	r1, r4
60302270:	e1a02003 	mov	r2, r3
60302274:	e1a00005 	mov	r0, r5
60302278:	eb0003ca 	bl	603031a8 <__lshift>
6030227c:	e1a04000 	mov	r4, r0
60302280:	e59d302c 	ldr	r3, [sp, #44]	; 0x2c
60302284:	e3530000 	cmp	r3, #0
60302288:	0a000015 	beq	603022e4 <_dtoa_r+0xae8>
6030228c:	e1a01004 	mov	r1, r4
60302290:	e1a0000b 	mov	r0, fp
60302294:	eb00040e 	bl	603032d4 <__mcmp>
60302298:	e3500000 	cmp	r0, #0
6030229c:	aa000010 	bge	603022e4 <_dtoa_r+0xae8>
603022a0:	e3a03000 	mov	r3, #0
603022a4:	e1a0100b 	mov	r1, fp
603022a8:	e3a0200a 	mov	r2, #10
603022ac:	e1a00005 	mov	r0, r5
603022b0:	eb000241 	bl	60302bbc <__multadd>
603022b4:	e59d3018 	ldr	r3, [sp, #24]
603022b8:	e24aa001 	sub	sl, sl, #1
603022bc:	e1a0b000 	mov	fp, r0
603022c0:	e3530000 	cmp	r3, #0
603022c4:	0a000005 	beq	603022e0 <_dtoa_r+0xae4>
603022c8:	e1a01006 	mov	r1, r6
603022cc:	e3a03000 	mov	r3, #0
603022d0:	e3a0200a 	mov	r2, #10
603022d4:	e1a00005 	mov	r0, r5
603022d8:	eb000237 	bl	60302bbc <__multadd>
603022dc:	e1a06000 	mov	r6, r0
603022e0:	e59d8020 	ldr	r8, [sp, #32]
603022e4:	e59d201c 	ldr	r2, [sp, #28]
603022e8:	e3580000 	cmp	r8, #0
603022ec:	d3a03001 	movle	r3, #1
603022f0:	c3a03000 	movgt	r3, #0
603022f4:	e3520002 	cmp	r2, #2
603022f8:	d3a03000 	movle	r3, #0
603022fc:	e3530000 	cmp	r3, #0
60302300:	0a00003c 	beq	603023f8 <_dtoa_r+0xbfc>
60302304:	e3580000 	cmp	r8, #0
60302308:	1a00000a 	bne	60302338 <_dtoa_r+0xb3c>
6030230c:	e1a01004 	mov	r1, r4
60302310:	e1a03008 	mov	r3, r8
60302314:	e3a02005 	mov	r2, #5
60302318:	e1a00005 	mov	r0, r5
6030231c:	eb000226 	bl	60302bbc <__multadd>
60302320:	e1a01000 	mov	r1, r0
60302324:	e1a04000 	mov	r4, r0
60302328:	e1a0000b 	mov	r0, fp
6030232c:	eb0003e8 	bl	603032d4 <__mcmp>
60302330:	e3500000 	cmp	r0, #0
60302334:	cafffea1 	bgt	60301dc0 <_dtoa_r+0x5c4>
60302338:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
6030233c:	e59d7004 	ldr	r7, [sp, #4]
60302340:	e1e0a003 	mvn	sl, r3
60302344:	e3a08000 	mov	r8, #0
60302348:	e1a01004 	mov	r1, r4
6030234c:	e1a00005 	mov	r0, r5
60302350:	eb0001fa 	bl	60302b40 <_Bfree>
60302354:	e3560000 	cmp	r6, #0
60302358:	0affff29 	beq	60302004 <_dtoa_r+0x808>
6030235c:	e3580000 	cmp	r8, #0
60302360:	11580006 	cmpne	r8, r6
60302364:	0a000002 	beq	60302374 <_dtoa_r+0xb78>
60302368:	e1a01008 	mov	r1, r8
6030236c:	e1a00005 	mov	r0, r5
60302370:	eb0001f2 	bl	60302b40 <_Bfree>
60302374:	e1a01006 	mov	r1, r6
60302378:	e1a00005 	mov	r0, r5
6030237c:	eb0001ef 	bl	60302b40 <_Bfree>
60302380:	eaffff1f 	b	60302004 <_dtoa_r+0x808>
60302384:	e2483001 	sub	r3, r8, #1
60302388:	e1590003 	cmp	r9, r3
6030238c:	b0439009 	sublt	r9, r3, r9
60302390:	a0494003 	subge	r4, r9, r3
60302394:	b59d2028 	ldrlt	r2, [sp, #40]	; 0x28
60302398:	b0822009 	addlt	r2, r2, r9
6030239c:	b1a09003 	movlt	r9, r3
603023a0:	b58d2028 	strlt	r2, [sp, #40]	; 0x28
603023a4:	e3580000 	cmp	r8, #0
603023a8:	a1a03008 	movge	r3, r8
603023ac:	b59d3010 	ldrlt	r3, [sp, #16]
603023b0:	a59d7010 	ldrge	r7, [sp, #16]
603023b4:	b0437008 	sublt	r7, r3, r8
603023b8:	b3a03000 	movlt	r3, #0
603023bc:	eaffff2c 	b	60302074 <_dtoa_r+0x878>
603023c0:	e1a02009 	mov	r2, r9
603023c4:	eaffff59 	b	60302130 <_dtoa_r+0x934>
603023c8:	e59d301c 	ldr	r3, [sp, #28]
603023cc:	e3530001 	cmp	r3, #1
603023d0:	daffff6a 	ble	60302180 <_dtoa_r+0x984>
603023d4:	e3a09000 	mov	r9, #0
603023d8:	eaffff7d 	b	603021d4 <_dtoa_r+0x9d8>
603023dc:	e3a04000 	mov	r4, #0
603023e0:	e1a06004 	mov	r6, r4
603023e4:	eaffffd3 	b	60302338 <_dtoa_r+0xb3c>
603023e8:	e1a0a001 	mov	sl, r1
603023ec:	e1a04000 	mov	r4, r0
603023f0:	e1a06000 	mov	r6, r0
603023f4:	eafffe71 	b	60301dc0 <_dtoa_r+0x5c4>
603023f8:	e59d3018 	ldr	r3, [sp, #24]
603023fc:	e3530000 	cmp	r3, #0
60302400:	0a0000a3 	beq	60302694 <_dtoa_r+0xe98>
60302404:	e3570000 	cmp	r7, #0
60302408:	da000004 	ble	60302420 <_dtoa_r+0xc24>
6030240c:	e1a01006 	mov	r1, r6
60302410:	e1a02007 	mov	r2, r7
60302414:	e1a00005 	mov	r0, r5
60302418:	eb000362 	bl	603031a8 <__lshift>
6030241c:	e1a06000 	mov	r6, r0
60302420:	e3590000 	cmp	r9, #0
60302424:	01a00006 	moveq	r0, r6
60302428:	0a000012 	beq	60302478 <_dtoa_r+0xc7c>
6030242c:	e5961004 	ldr	r1, [r6, #4]
60302430:	e1a00005 	mov	r0, r5
60302434:	eb000188 	bl	60302a5c <_Balloc>
60302438:	e2507000 	subs	r7, r0, #0
6030243c:	1a000003 	bne	60302450 <_dtoa_r+0xc54>
60302440:	e51f3818 	ldr	r3, [pc, #-2072]	; 60301c30 <_dtoa_r+0x434>
60302444:	e1a02007 	mov	r2, r7
60302448:	e30012ea 	movw	r1, #746	; 0x2ea
6030244c:	eafffdd3 	b	60301ba0 <_dtoa_r+0x3a4>
60302450:	e5962010 	ldr	r2, [r6, #16]
60302454:	e286100c 	add	r1, r6, #12
60302458:	e287000c 	add	r0, r7, #12
6030245c:	e2822002 	add	r2, r2, #2
60302460:	e1a02102 	lsl	r2, r2, #2
60302464:	eb004a62 	bl	60314df4 <__wrap_memcpy>
60302468:	e3a02001 	mov	r2, #1
6030246c:	e1a01007 	mov	r1, r7
60302470:	e1a00005 	mov	r0, r5
60302474:	eb00034b 	bl	603031a8 <__lshift>
60302478:	e59d3004 	ldr	r3, [sp, #4]
6030247c:	e2833001 	add	r3, r3, #1
60302480:	e58d3010 	str	r3, [sp, #16]
60302484:	e59d3004 	ldr	r3, [sp, #4]
60302488:	e0833008 	add	r3, r3, r8
6030248c:	e58d3028 	str	r3, [sp, #40]	; 0x28
60302490:	e59d3008 	ldr	r3, [sp, #8]
60302494:	e1a08006 	mov	r8, r6
60302498:	e1a06000 	mov	r6, r0
6030249c:	e2033001 	and	r3, r3, #1
603024a0:	e58d3024 	str	r3, [sp, #36]	; 0x24
603024a4:	e59d3010 	ldr	r3, [sp, #16]
603024a8:	e1a01004 	mov	r1, r4
603024ac:	e1a0000b 	mov	r0, fp
603024b0:	e2433001 	sub	r3, r3, #1
603024b4:	e58d3008 	str	r3, [sp, #8]
603024b8:	ebfffc64 	bl	60301650 <quorem>
603024bc:	e1a01008 	mov	r1, r8
603024c0:	e58d0014 	str	r0, [sp, #20]
603024c4:	e2809030 	add	r9, r0, #48	; 0x30
603024c8:	e1a0000b 	mov	r0, fp
603024cc:	eb000380 	bl	603032d4 <__mcmp>
603024d0:	e1a02006 	mov	r2, r6
603024d4:	e58d0020 	str	r0, [sp, #32]
603024d8:	e1a01004 	mov	r1, r4
603024dc:	e1a00005 	mov	r0, r5
603024e0:	eb000391 	bl	6030332c <__mdiff>
603024e4:	e590200c 	ldr	r2, [r0, #12]
603024e8:	e1a07000 	mov	r7, r0
603024ec:	e3520000 	cmp	r2, #0
603024f0:	159d2018 	ldrne	r2, [sp, #24]
603024f4:	1a000003 	bne	60302508 <_dtoa_r+0xd0c>
603024f8:	e1a01000 	mov	r1, r0
603024fc:	e1a0000b 	mov	r0, fp
60302500:	eb000373 	bl	603032d4 <__mcmp>
60302504:	e1a02000 	mov	r2, r0
60302508:	e1a01007 	mov	r1, r7
6030250c:	e1a00005 	mov	r0, r5
60302510:	e58d202c 	str	r2, [sp, #44]	; 0x2c
60302514:	eb000189 	bl	60302b40 <_Bfree>
60302518:	e59d301c 	ldr	r3, [sp, #28]
6030251c:	e59d202c 	ldr	r2, [sp, #44]	; 0x2c
60302520:	e59d7010 	ldr	r7, [sp, #16]
60302524:	e1831002 	orr	r1, r3, r2
60302528:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
6030252c:	e1913003 	orrs	r3, r1, r3
60302530:	1a000009 	bne	6030255c <_dtoa_r+0xd60>
60302534:	e3590039 	cmp	r9, #57	; 0x39
60302538:	0a000020 	beq	603025c0 <_dtoa_r+0xdc4>
6030253c:	e59d3020 	ldr	r3, [sp, #32]
60302540:	e3530000 	cmp	r3, #0
60302544:	da000001 	ble	60302550 <_dtoa_r+0xd54>
60302548:	e59d3014 	ldr	r3, [sp, #20]
6030254c:	e2839031 	add	r9, r3, #49	; 0x31
60302550:	e59d3008 	ldr	r3, [sp, #8]
60302554:	e5c39000 	strb	r9, [r3]
60302558:	eaffff7a 	b	60302348 <_dtoa_r+0xb4c>
6030255c:	e59d3020 	ldr	r3, [sp, #32]
60302560:	e3530000 	cmp	r3, #0
60302564:	ba000005 	blt	60302580 <_dtoa_r+0xd84>
60302568:	e59d1020 	ldr	r1, [sp, #32]
6030256c:	e59d301c 	ldr	r3, [sp, #28]
60302570:	e1833001 	orr	r3, r3, r1
60302574:	e59d1024 	ldr	r1, [sp, #36]	; 0x24
60302578:	e1933001 	orrs	r3, r3, r1
6030257c:	1a00001b 	bne	603025f0 <_dtoa_r+0xdf4>
60302580:	e3520000 	cmp	r2, #0
60302584:	dafffff1 	ble	60302550 <_dtoa_r+0xd54>
60302588:	e1a0100b 	mov	r1, fp
6030258c:	e3a02001 	mov	r2, #1
60302590:	e1a00005 	mov	r0, r5
60302594:	eb000303 	bl	603031a8 <__lshift>
60302598:	e1a01004 	mov	r1, r4
6030259c:	e1a0b000 	mov	fp, r0
603025a0:	eb00034b 	bl	603032d4 <__mcmp>
603025a4:	e3500000 	cmp	r0, #0
603025a8:	ca000002 	bgt	603025b8 <_dtoa_r+0xdbc>
603025ac:	1affffe7 	bne	60302550 <_dtoa_r+0xd54>
603025b0:	e3190001 	tst	r9, #1
603025b4:	0affffe5 	beq	60302550 <_dtoa_r+0xd54>
603025b8:	e3590039 	cmp	r9, #57	; 0x39
603025bc:	1affffe1 	bne	60302548 <_dtoa_r+0xd4c>
603025c0:	e59d2008 	ldr	r2, [sp, #8]
603025c4:	e3a03039 	mov	r3, #57	; 0x39
603025c8:	e5c23000 	strb	r3, [r2]
603025cc:	e1a03007 	mov	r3, r7
603025d0:	e1a07003 	mov	r7, r3
603025d4:	e2433001 	sub	r3, r3, #1
603025d8:	e5572001 	ldrb	r2, [r7, #-1]
603025dc:	e3520039 	cmp	r2, #57	; 0x39
603025e0:	0a000054 	beq	60302738 <_dtoa_r+0xf3c>
603025e4:	e2822001 	add	r2, r2, #1
603025e8:	e5c32000 	strb	r2, [r3]
603025ec:	eaffff55 	b	60302348 <_dtoa_r+0xb4c>
603025f0:	e3520000 	cmp	r2, #0
603025f4:	da000005 	ble	60302610 <_dtoa_r+0xe14>
603025f8:	e3590039 	cmp	r9, #57	; 0x39
603025fc:	12893001 	addne	r3, r9, #1
60302600:	159d2008 	ldrne	r2, [sp, #8]
60302604:	0affffed 	beq	603025c0 <_dtoa_r+0xdc4>
60302608:	e5c23000 	strb	r3, [r2]
6030260c:	eaffff4d 	b	60302348 <_dtoa_r+0xb4c>
60302610:	e59d3010 	ldr	r3, [sp, #16]
60302614:	e59d2028 	ldr	r2, [sp, #40]	; 0x28
60302618:	e5439001 	strb	r9, [r3, #-1]
6030261c:	e1530002 	cmp	r3, r2
60302620:	0a000032 	beq	603026f0 <_dtoa_r+0xef4>
60302624:	e1a0100b 	mov	r1, fp
60302628:	e3a03000 	mov	r3, #0
6030262c:	e3a0200a 	mov	r2, #10
60302630:	e1a00005 	mov	r0, r5
60302634:	eb000160 	bl	60302bbc <__multadd>
60302638:	e3a03000 	mov	r3, #0
6030263c:	e1a0b000 	mov	fp, r0
60302640:	e3a0200a 	mov	r2, #10
60302644:	e1a01008 	mov	r1, r8
60302648:	e1a00005 	mov	r0, r5
6030264c:	e1580006 	cmp	r8, r6
60302650:	1a000006 	bne	60302670 <_dtoa_r+0xe74>
60302654:	eb000158 	bl	60302bbc <__multadd>
60302658:	e1a08000 	mov	r8, r0
6030265c:	e1a06000 	mov	r6, r0
60302660:	e59d3010 	ldr	r3, [sp, #16]
60302664:	e2833001 	add	r3, r3, #1
60302668:	e58d3010 	str	r3, [sp, #16]
6030266c:	eaffff8c 	b	603024a4 <_dtoa_r+0xca8>
60302670:	eb000151 	bl	60302bbc <__multadd>
60302674:	e1a01006 	mov	r1, r6
60302678:	e1a08000 	mov	r8, r0
6030267c:	e3a03000 	mov	r3, #0
60302680:	e3a0200a 	mov	r2, #10
60302684:	e1a00005 	mov	r0, r5
60302688:	eb00014b 	bl	60302bbc <__multadd>
6030268c:	e1a06000 	mov	r6, r0
60302690:	eafffff2 	b	60302660 <_dtoa_r+0xe64>
60302694:	e59d7004 	ldr	r7, [sp, #4]
60302698:	e1a01004 	mov	r1, r4
6030269c:	e1a0000b 	mov	r0, fp
603026a0:	ebfffbea 	bl	60301650 <quorem>
603026a4:	e59d3004 	ldr	r3, [sp, #4]
603026a8:	e2809030 	add	r9, r0, #48	; 0x30
603026ac:	e4c79001 	strb	r9, [r7], #1
603026b0:	e0472003 	sub	r2, r7, r3
603026b4:	e1580002 	cmp	r8, r2
603026b8:	da000006 	ble	603026d8 <_dtoa_r+0xedc>
603026bc:	e1a0100b 	mov	r1, fp
603026c0:	e3a03000 	mov	r3, #0
603026c4:	e3a0200a 	mov	r2, #10
603026c8:	e1a00005 	mov	r0, r5
603026cc:	eb00013a 	bl	60302bbc <__multadd>
603026d0:	e1a0b000 	mov	fp, r0
603026d4:	eaffffef 	b	60302698 <_dtoa_r+0xe9c>
603026d8:	e3580000 	cmp	r8, #0
603026dc:	e59d3004 	ldr	r3, [sp, #4]
603026e0:	c1a07008 	movgt	r7, r8
603026e4:	d3a07001 	movle	r7, #1
603026e8:	e0837007 	add	r7, r3, r7
603026ec:	e3a08000 	mov	r8, #0
603026f0:	e1a0100b 	mov	r1, fp
603026f4:	e3a02001 	mov	r2, #1
603026f8:	e1a00005 	mov	r0, r5
603026fc:	eb0002a9 	bl	603031a8 <__lshift>
60302700:	e1a01004 	mov	r1, r4
60302704:	e1a0b000 	mov	fp, r0
60302708:	eb0002f1 	bl	603032d4 <__mcmp>
6030270c:	e3500000 	cmp	r0, #0
60302710:	caffffad 	bgt	603025cc <_dtoa_r+0xdd0>
60302714:	1a000001 	bne	60302720 <_dtoa_r+0xf24>
60302718:	e3190001 	tst	r9, #1
6030271c:	1affffaa 	bne	603025cc <_dtoa_r+0xdd0>
60302720:	e1a03007 	mov	r3, r7
60302724:	e5772001 	ldrb	r2, [r7, #-1]!
60302728:	e3520030 	cmp	r2, #48	; 0x30
6030272c:	0afffffb 	beq	60302720 <_dtoa_r+0xf24>
60302730:	e1a07003 	mov	r7, r3
60302734:	eaffff03 	b	60302348 <_dtoa_r+0xb4c>
60302738:	e59d2004 	ldr	r2, [sp, #4]
6030273c:	e1520003 	cmp	r2, r3
60302740:	1affffa2 	bne	603025d0 <_dtoa_r+0xdd4>
60302744:	e28aa001 	add	sl, sl, #1
60302748:	e3a03031 	mov	r3, #49	; 0x31
6030274c:	eaffffad 	b	60302608 <_dtoa_r+0xe0c>
60302750:	e59d3074 	ldr	r3, [sp, #116]	; 0x74
60302754:	e3530000 	cmp	r3, #0
60302758:	e51f3b2c 	ldr	r3, [pc, #-2860]	; 60301c34 <_dtoa_r+0x438>
6030275c:	e58d3004 	str	r3, [sp, #4]
60302760:	12833008 	addne	r3, r3, #8
60302764:	1afffc62 	bne	603018f4 <_dtoa_r+0xf8>
60302768:	eafffc63 	b	603018fc <_dtoa_r+0x100>

6030276c <_fclose_r>:
6030276c:	e92d4070 	push	{r4, r5, r6, lr}
60302770:	e2514000 	subs	r4, r1, #0
60302774:	1a000002 	bne	60302784 <_fclose_r+0x18>
60302778:	e3a06000 	mov	r6, #0
6030277c:	e1a00006 	mov	r0, r6
60302780:	e8bd8070 	pop	{r4, r5, r6, pc}
60302784:	e1a05000 	mov	r5, r0
60302788:	e3500000 	cmp	r0, #0
6030278c:	0a000003 	beq	603027a0 <_fclose_r+0x34>
60302790:	e5903018 	ldr	r3, [r0, #24]
60302794:	e3530000 	cmp	r3, #0
60302798:	1a000000 	bne	603027a0 <_fclose_r+0x34>
6030279c:	ebfff9cf 	bl	60300ee0 <__sinit>
603027a0:	e5943064 	ldr	r3, [r4, #100]	; 0x64
603027a4:	e3130001 	tst	r3, #1
603027a8:	1a000004 	bne	603027c0 <_fclose_r+0x54>
603027ac:	e1d430bc 	ldrh	r3, [r4, #12]
603027b0:	e3130c02 	tst	r3, #512	; 0x200
603027b4:	1a000001 	bne	603027c0 <_fclose_r+0x54>
603027b8:	e5940058 	ldr	r0, [r4, #88]	; 0x58
603027bc:	ebfffa95 	bl	60301218 <__retarget_lock_acquire_recursive>
603027c0:	e1d430fc 	ldrsh	r3, [r4, #12]
603027c4:	e3530000 	cmp	r3, #0
603027c8:	1a000005 	bne	603027e4 <_fclose_r+0x78>
603027cc:	e5940064 	ldr	r0, [r4, #100]	; 0x64
603027d0:	e2106001 	ands	r6, r0, #1
603027d4:	1affffe7 	bne	60302778 <_fclose_r+0xc>
603027d8:	e5940058 	ldr	r0, [r4, #88]	; 0x58
603027dc:	ebfffa93 	bl	60301230 <__retarget_lock_release_recursive>
603027e0:	eaffffe5 	b	6030277c <_fclose_r+0x10>
603027e4:	e1a01004 	mov	r1, r4
603027e8:	e1a00005 	mov	r0, r5
603027ec:	ebfff896 	bl	60300a4c <__sflush_r>
603027f0:	e5943030 	ldr	r3, [r4, #48]	; 0x30
603027f4:	e1a06000 	mov	r6, r0
603027f8:	e3530000 	cmp	r3, #0
603027fc:	0a000004 	beq	60302814 <_fclose_r+0xa8>
60302800:	e5941020 	ldr	r1, [r4, #32]
60302804:	e1a00005 	mov	r0, r5
60302808:	e12fff33 	blx	r3
6030280c:	e3500000 	cmp	r0, #0
60302810:	b3e06000 	mvnlt	r6, #0
60302814:	e1d430bc 	ldrh	r3, [r4, #12]
60302818:	e3130080 	tst	r3, #128	; 0x80
6030281c:	0a000002 	beq	6030282c <_fclose_r+0xc0>
60302820:	e5941010 	ldr	r1, [r4, #16]
60302824:	e1a00005 	mov	r0, r5
60302828:	eb002b74 	bl	6030d600 <__wrap__free_r>
6030282c:	e5941034 	ldr	r1, [r4, #52]	; 0x34
60302830:	e3510000 	cmp	r1, #0
60302834:	0a000006 	beq	60302854 <_fclose_r+0xe8>
60302838:	e2843044 	add	r3, r4, #68	; 0x44
6030283c:	e1510003 	cmp	r1, r3
60302840:	0a000001 	beq	6030284c <_fclose_r+0xe0>
60302844:	e1a00005 	mov	r0, r5
60302848:	eb002b6c 	bl	6030d600 <__wrap__free_r>
6030284c:	e3a03000 	mov	r3, #0
60302850:	e5843034 	str	r3, [r4, #52]	; 0x34
60302854:	e5941048 	ldr	r1, [r4, #72]	; 0x48
60302858:	e3510000 	cmp	r1, #0
6030285c:	0a000003 	beq	60302870 <_fclose_r+0x104>
60302860:	e1a00005 	mov	r0, r5
60302864:	eb002b65 	bl	6030d600 <__wrap__free_r>
60302868:	e3a03000 	mov	r3, #0
6030286c:	e5843048 	str	r3, [r4, #72]	; 0x48
60302870:	ebfff98e 	bl	60300eb0 <__sfp_lock_acquire>
60302874:	e3a03000 	mov	r3, #0
60302878:	e1c430bc 	strh	r3, [r4, #12]
6030287c:	e5943064 	ldr	r3, [r4, #100]	; 0x64
60302880:	e3130001 	tst	r3, #1
60302884:	1a000001 	bne	60302890 <_fclose_r+0x124>
60302888:	e5940058 	ldr	r0, [r4, #88]	; 0x58
6030288c:	ebfffa67 	bl	60301230 <__retarget_lock_release_recursive>
60302890:	e5940058 	ldr	r0, [r4, #88]	; 0x58
60302894:	ebfffa5d 	bl	60301210 <__retarget_lock_close_recursive>
60302898:	ebfff987 	bl	60300ebc <__sfp_lock_release>
6030289c:	eaffffb6 	b	6030277c <_fclose_r+0x10>

603028a0 <fclose>:
603028a0:	e59f3008 	ldr	r3, [pc, #8]	; 603028b0 <fclose+0x10>
603028a4:	e1a01000 	mov	r1, r0
603028a8:	e5930000 	ldr	r0, [r3]
603028ac:	eaffffae 	b	6030276c <_fclose_r>
603028b0:	60317000 	.word	0x60317000

603028b4 <_fiprintf_r>:
603028b4:	e92d000c 	push	{r2, r3}
603028b8:	e92d4007 	push	{r0, r1, r2, lr}
603028bc:	e59d2010 	ldr	r2, [sp, #16]
603028c0:	e28d3014 	add	r3, sp, #20
603028c4:	e58d3004 	str	r3, [sp, #4]
603028c8:	eb000442 	bl	603039d8 <_vfiprintf_r>
603028cc:	e28dd00c 	add	sp, sp, #12
603028d0:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
603028d4:	e28dd008 	add	sp, sp, #8
603028d8:	e12fff1e 	bx	lr

603028dc <fiprintf>:
603028dc:	e92d000e 	push	{r1, r2, r3}
603028e0:	e92d4003 	push	{r0, r1, lr}
603028e4:	e1a01000 	mov	r1, r0
603028e8:	e59f0020 	ldr	r0, [pc, #32]	; 60302910 <fiprintf+0x34>
603028ec:	e28d3010 	add	r3, sp, #16
603028f0:	e59d200c 	ldr	r2, [sp, #12]
603028f4:	e58d3004 	str	r3, [sp, #4]
603028f8:	e5900000 	ldr	r0, [r0]
603028fc:	eb000435 	bl	603039d8 <_vfiprintf_r>
60302900:	e28dd008 	add	sp, sp, #8
60302904:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
60302908:	e28dd00c 	add	sp, sp, #12
6030290c:	e12fff1e 	bx	lr
60302910:	60317000 	.word	0x60317000

60302914 <_setlocale_r>:
60302914:	e92d4010 	push	{r4, lr}
60302918:	e2524000 	subs	r4, r2, #0
6030291c:	1a000001 	bne	60302928 <_setlocale_r+0x14>
60302920:	e59f0044 	ldr	r0, [pc, #68]	; 6030296c <_setlocale_r+0x58>
60302924:	e8bd8010 	pop	{r4, pc}
60302928:	e59f1040 	ldr	r1, [pc, #64]	; 60302970 <_setlocale_r+0x5c>
6030292c:	e1a00004 	mov	r0, r4
60302930:	eb00493b 	bl	60314e24 <__wrap_strcmp>
60302934:	e3500000 	cmp	r0, #0
60302938:	0afffff8 	beq	60302920 <_setlocale_r+0xc>
6030293c:	e59f1028 	ldr	r1, [pc, #40]	; 6030296c <_setlocale_r+0x58>
60302940:	e1a00004 	mov	r0, r4
60302944:	eb004936 	bl	60314e24 <__wrap_strcmp>
60302948:	e3500000 	cmp	r0, #0
6030294c:	0afffff3 	beq	60302920 <_setlocale_r+0xc>
60302950:	e59f101c 	ldr	r1, [pc, #28]	; 60302974 <_setlocale_r+0x60>
60302954:	e1a00004 	mov	r0, r4
60302958:	eb004931 	bl	60314e24 <__wrap_strcmp>
6030295c:	e3500000 	cmp	r0, #0
60302960:	0affffee 	beq	60302920 <_setlocale_r+0xc>
60302964:	e3a00000 	mov	r0, #0
60302968:	e8bd8010 	pop	{r4, pc}
6030296c:	60316e4d 	.word	0x60316e4d
60302970:	60316e4f 	.word	0x60316e4f
60302974:	60316dce 	.word	0x60316dce

60302978 <__locale_mb_cur_max>:
60302978:	e59f3004 	ldr	r3, [pc, #4]	; 60302984 <__locale_mb_cur_max+0xc>
6030297c:	e5d30128 	ldrb	r0, [r3, #296]	; 0x128
60302980:	e12fff1e 	bx	lr
60302984:	603170f4 	.word	0x603170f4

60302988 <setlocale>:
60302988:	e59f300c 	ldr	r3, [pc, #12]	; 6030299c <setlocale+0x14>
6030298c:	e1a02001 	mov	r2, r1
60302990:	e1a01000 	mov	r1, r0
60302994:	e5930000 	ldr	r0, [r3]
60302998:	eaffffdd 	b	60302914 <_setlocale_r>
6030299c:	60317000 	.word	0x60317000

603029a0 <__localeconv_l>:
603029a0:	e28000f0 	add	r0, r0, #240	; 0xf0
603029a4:	e12fff1e 	bx	lr

603029a8 <_localeconv_r>:
603029a8:	e59f0000 	ldr	r0, [pc]	; 603029b0 <_localeconv_r+0x8>
603029ac:	e12fff1e 	bx	lr
603029b0:	603171e4 	.word	0x603171e4

603029b4 <localeconv>:
603029b4:	e59f0000 	ldr	r0, [pc]	; 603029bc <localeconv+0x8>
603029b8:	e12fff1e 	bx	lr
603029bc:	603171e4 	.word	0x603171e4

603029c0 <_lseek_r>:
603029c0:	e92d4070 	push	{r4, r5, r6, lr}
603029c4:	e1a04000 	mov	r4, r0
603029c8:	e59f502c 	ldr	r5, [pc, #44]	; 603029fc <_lseek_r+0x3c>
603029cc:	e1a00001 	mov	r0, r1
603029d0:	e1a01002 	mov	r1, r2
603029d4:	e3a02000 	mov	r2, #0
603029d8:	e5852000 	str	r2, [r5]
603029dc:	e1a02003 	mov	r2, r3
603029e0:	eb002011 	bl	6030aa2c <_lseek>
603029e4:	e3700001 	cmn	r0, #1
603029e8:	18bd8070 	popne	{r4, r5, r6, pc}
603029ec:	e5953000 	ldr	r3, [r5]
603029f0:	e3530000 	cmp	r3, #0
603029f4:	15843000 	strne	r3, [r4]
603029f8:	e8bd8070 	pop	{r4, r5, r6, pc}
603029fc:	60317460 	.word	0x60317460

60302a00 <_mbtowc_r>:
60302a00:	e59fc010 	ldr	ip, [pc, #16]	; 60302a18 <_mbtowc_r+0x18>
60302a04:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
60302a08:	e59ce0e4 	ldr	lr, [ip, #228]	; 0xe4
60302a0c:	e1a0c00e 	mov	ip, lr
60302a10:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
60302a14:	e12fff1c 	bx	ip
60302a18:	603170f4 	.word	0x603170f4

60302a1c <__ascii_mbtowc>:
60302a1c:	e3510000 	cmp	r1, #0
60302a20:	e24dd008 	sub	sp, sp, #8
60302a24:	028d1004 	addeq	r1, sp, #4
60302a28:	e3520000 	cmp	r2, #0
60302a2c:	01a00002 	moveq	r0, r2
60302a30:	0a000007 	beq	60302a54 <__ascii_mbtowc+0x38>
60302a34:	e3530000 	cmp	r3, #0
60302a38:	03e00001 	mvneq	r0, #1
60302a3c:	0a000004 	beq	60302a54 <__ascii_mbtowc+0x38>
60302a40:	e5d23000 	ldrb	r3, [r2]
60302a44:	e5813000 	str	r3, [r1]
60302a48:	e5d22000 	ldrb	r2, [r2]
60302a4c:	e2520000 	subs	r0, r2, #0
60302a50:	13a00001 	movne	r0, #1
60302a54:	e28dd008 	add	sp, sp, #8
60302a58:	e12fff1e 	bx	lr

60302a5c <_Balloc>:
60302a5c:	e92d4070 	push	{r4, r5, r6, lr}
60302a60:	e1a04000 	mov	r4, r0
60302a64:	e5906024 	ldr	r6, [r0, #36]	; 0x24
60302a68:	e1a05001 	mov	r5, r1
60302a6c:	e3560000 	cmp	r6, #0
60302a70:	1a00000d 	bne	60302aac <_Balloc+0x50>
60302a74:	e3a00010 	mov	r0, #16
60302a78:	eb002ada 	bl	6030d5e8 <__wrap_malloc>
60302a7c:	e1a02000 	mov	r2, r0
60302a80:	e3500000 	cmp	r0, #0
60302a84:	e5840024 	str	r0, [r4, #36]	; 0x24
60302a88:	1a000003 	bne	60302a9c <_Balloc+0x40>
60302a8c:	e59f30a4 	ldr	r3, [pc, #164]	; 60302b38 <_Balloc+0xdc>
60302a90:	e3a01066 	mov	r1, #102	; 0x66
60302a94:	e59f00a0 	ldr	r0, [pc, #160]	; 60302b3c <_Balloc+0xe0>
60302a98:	ebfffac6 	bl	603015b8 <__assert_func>
60302a9c:	e5806004 	str	r6, [r0, #4]
60302aa0:	e5806008 	str	r6, [r0, #8]
60302aa4:	e5806000 	str	r6, [r0]
60302aa8:	e580600c 	str	r6, [r0, #12]
60302aac:	e5946024 	ldr	r6, [r4, #36]	; 0x24
60302ab0:	e596300c 	ldr	r3, [r6, #12]
60302ab4:	e3530000 	cmp	r3, #0
60302ab8:	0a000013 	beq	60302b0c <_Balloc+0xb0>
60302abc:	e5943024 	ldr	r3, [r4, #36]	; 0x24
60302ac0:	e593300c 	ldr	r3, [r3, #12]
60302ac4:	e7930105 	ldr	r0, [r3, r5, lsl #2]
60302ac8:	e3500000 	cmp	r0, #0
60302acc:	15902000 	ldrne	r2, [r0]
60302ad0:	17832105 	strne	r2, [r3, r5, lsl #2]
60302ad4:	1a000008 	bne	60302afc <_Balloc+0xa0>
60302ad8:	e3a01001 	mov	r1, #1
60302adc:	e1a00004 	mov	r0, r4
60302ae0:	e1a06511 	lsl	r6, r1, r5
60302ae4:	e2862005 	add	r2, r6, #5
60302ae8:	e1a02102 	lsl	r2, r2, #2
60302aec:	eb002ac5 	bl	6030d608 <__wrap__calloc_r>
60302af0:	e3500000 	cmp	r0, #0
60302af4:	0a00000d 	beq	60302b30 <_Balloc+0xd4>
60302af8:	e9800060 	stmib	r0, {r5, r6}
60302afc:	e3a03000 	mov	r3, #0
60302b00:	e5803010 	str	r3, [r0, #16]
60302b04:	e580300c 	str	r3, [r0, #12]
60302b08:	e8bd8070 	pop	{r4, r5, r6, pc}
60302b0c:	e3a02021 	mov	r2, #33	; 0x21
60302b10:	e3a01004 	mov	r1, #4
60302b14:	e1a00004 	mov	r0, r4
60302b18:	eb002aba 	bl	6030d608 <__wrap__calloc_r>
60302b1c:	e5943024 	ldr	r3, [r4, #36]	; 0x24
60302b20:	e586000c 	str	r0, [r6, #12]
60302b24:	e593300c 	ldr	r3, [r3, #12]
60302b28:	e3530000 	cmp	r3, #0
60302b2c:	1affffe2 	bne	60302abc <_Balloc+0x60>
60302b30:	e3a00000 	mov	r0, #0
60302b34:	e8bd8070 	pop	{r4, r5, r6, pc}
60302b38:	60316dde 	.word	0x60316dde
60302b3c:	60316e57 	.word	0x60316e57

60302b40 <_Bfree>:
60302b40:	e92d4070 	push	{r4, r5, r6, lr}
60302b44:	e1a05000 	mov	r5, r0
60302b48:	e5906024 	ldr	r6, [r0, #36]	; 0x24
60302b4c:	e1a04001 	mov	r4, r1
60302b50:	e3560000 	cmp	r6, #0
60302b54:	1a00000d 	bne	60302b90 <_Bfree+0x50>
60302b58:	e3a00010 	mov	r0, #16
60302b5c:	eb002aa1 	bl	6030d5e8 <__wrap_malloc>
60302b60:	e1a02000 	mov	r2, r0
60302b64:	e3500000 	cmp	r0, #0
60302b68:	e5850024 	str	r0, [r5, #36]	; 0x24
60302b6c:	1a000003 	bne	60302b80 <_Bfree+0x40>
60302b70:	e59f303c 	ldr	r3, [pc, #60]	; 60302bb4 <_Bfree+0x74>
60302b74:	e3a0108a 	mov	r1, #138	; 0x8a
60302b78:	e59f0038 	ldr	r0, [pc, #56]	; 60302bb8 <_Bfree+0x78>
60302b7c:	ebfffa8d 	bl	603015b8 <__assert_func>
60302b80:	e5806004 	str	r6, [r0, #4]
60302b84:	e5806008 	str	r6, [r0, #8]
60302b88:	e5806000 	str	r6, [r0]
60302b8c:	e580600c 	str	r6, [r0, #12]
60302b90:	e3540000 	cmp	r4, #0
60302b94:	08bd8070 	popeq	{r4, r5, r6, pc}
60302b98:	e5953024 	ldr	r3, [r5, #36]	; 0x24
60302b9c:	e5942004 	ldr	r2, [r4, #4]
60302ba0:	e593300c 	ldr	r3, [r3, #12]
60302ba4:	e7931102 	ldr	r1, [r3, r2, lsl #2]
60302ba8:	e5841000 	str	r1, [r4]
60302bac:	e7834102 	str	r4, [r3, r2, lsl #2]
60302bb0:	e8bd8070 	pop	{r4, r5, r6, pc}
60302bb4:	60316dde 	.word	0x60316dde
60302bb8:	60316e57 	.word	0x60316e57

60302bbc <__multadd>:
60302bbc:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60302bc0:	e281c014 	add	ip, r1, #20
60302bc4:	e5915010 	ldr	r5, [r1, #16]
60302bc8:	e1a07000 	mov	r7, r0
60302bcc:	e1a04001 	mov	r4, r1
60302bd0:	e1a06003 	mov	r6, r3
60302bd4:	e3a00000 	mov	r0, #0
60302bd8:	e59c3000 	ldr	r3, [ip]
60302bdc:	e2800001 	add	r0, r0, #1
60302be0:	e1550000 	cmp	r5, r0
60302be4:	e6ff1073 	uxth	r1, r3
60302be8:	e1a0e823 	lsr	lr, r3, #16
60302bec:	e0216192 	mla	r1, r2, r1, r6
60302bf0:	e1a03821 	lsr	r3, r1, #16
60302bf4:	e6ff1071 	uxth	r1, r1
60302bf8:	e0233e92 	mla	r3, r2, lr, r3
60302bfc:	e0811803 	add	r1, r1, r3, lsl #16
60302c00:	e1a06823 	lsr	r6, r3, #16
60302c04:	e48c1004 	str	r1, [ip], #4
60302c08:	cafffff2 	bgt	60302bd8 <__multadd+0x1c>
60302c0c:	e3560000 	cmp	r6, #0
60302c10:	0a00001b 	beq	60302c84 <__multadd+0xc8>
60302c14:	e5943008 	ldr	r3, [r4, #8]
60302c18:	e1530005 	cmp	r3, r5
60302c1c:	ca000014 	bgt	60302c74 <__multadd+0xb8>
60302c20:	e5941004 	ldr	r1, [r4, #4]
60302c24:	e1a00007 	mov	r0, r7
60302c28:	e2811001 	add	r1, r1, #1
60302c2c:	ebffff8a 	bl	60302a5c <_Balloc>
60302c30:	e2508000 	subs	r8, r0, #0
60302c34:	1a000004 	bne	60302c4c <__multadd+0x90>
60302c38:	e59f304c 	ldr	r3, [pc, #76]	; 60302c8c <__multadd+0xd0>
60302c3c:	e1a02008 	mov	r2, r8
60302c40:	e59f0048 	ldr	r0, [pc, #72]	; 60302c90 <__multadd+0xd4>
60302c44:	e3a010b5 	mov	r1, #181	; 0xb5
60302c48:	ebfffa5a 	bl	603015b8 <__assert_func>
60302c4c:	e5942010 	ldr	r2, [r4, #16]
60302c50:	e284100c 	add	r1, r4, #12
60302c54:	e288000c 	add	r0, r8, #12
60302c58:	e2822002 	add	r2, r2, #2
60302c5c:	e1a02102 	lsl	r2, r2, #2
60302c60:	eb004863 	bl	60314df4 <__wrap_memcpy>
60302c64:	e1a01004 	mov	r1, r4
60302c68:	e1a04008 	mov	r4, r8
60302c6c:	e1a00007 	mov	r0, r7
60302c70:	ebffffb2 	bl	60302b40 <_Bfree>
60302c74:	e0843105 	add	r3, r4, r5, lsl #2
60302c78:	e2855001 	add	r5, r5, #1
60302c7c:	e5836014 	str	r6, [r3, #20]
60302c80:	e5845010 	str	r5, [r4, #16]
60302c84:	e1a00004 	mov	r0, r4
60302c88:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60302c8c:	60316e3c 	.word	0x60316e3c
60302c90:	60316e57 	.word	0x60316e57

60302c94 <__s2b>:
60302c94:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
60302c98:	e1a05002 	mov	r5, r2
60302c9c:	e59f20d8 	ldr	r2, [pc, #216]	; 60302d7c <__s2b+0xe8>
60302ca0:	e1a07003 	mov	r7, r3
60302ca4:	e2833008 	add	r3, r3, #8
60302ca8:	e1a04001 	mov	r4, r1
60302cac:	e1a06000 	mov	r6, r0
60302cb0:	e0c12293 	smull	r2, r1, r3, r2
60302cb4:	e1a03fc3 	asr	r3, r3, #31
60302cb8:	e3a02001 	mov	r2, #1
60302cbc:	e06330c1 	rsb	r3, r3, r1, asr #1
60302cc0:	e3a01000 	mov	r1, #0
60302cc4:	e1520003 	cmp	r2, r3
60302cc8:	ba000008 	blt	60302cf0 <__s2b+0x5c>
60302ccc:	e1a00006 	mov	r0, r6
60302cd0:	ebffff61 	bl	60302a5c <_Balloc>
60302cd4:	e3500000 	cmp	r0, #0
60302cd8:	1a000007 	bne	60302cfc <__s2b+0x68>
60302cdc:	e1a02000 	mov	r2, r0
60302ce0:	e59f3098 	ldr	r3, [pc, #152]	; 60302d80 <__s2b+0xec>
60302ce4:	e59f0098 	ldr	r0, [pc, #152]	; 60302d84 <__s2b+0xf0>
60302ce8:	e3a010ce 	mov	r1, #206	; 0xce
60302cec:	ebfffa31 	bl	603015b8 <__assert_func>
60302cf0:	e1a02082 	lsl	r2, r2, #1
60302cf4:	e2811001 	add	r1, r1, #1
60302cf8:	eafffff1 	b	60302cc4 <__s2b+0x30>
60302cfc:	e59d3020 	ldr	r3, [sp, #32]
60302d00:	e3550009 	cmp	r5, #9
60302d04:	e5803014 	str	r3, [r0, #20]
60302d08:	d284400a 	addle	r4, r4, #10
60302d0c:	e3a03001 	mov	r3, #1
60302d10:	d3a05009 	movle	r5, #9
60302d14:	e5803010 	str	r3, [r0, #16]
60302d18:	da00000c 	ble	60302d50 <__s2b+0xbc>
60302d1c:	e2849009 	add	r9, r4, #9
60302d20:	e0844005 	add	r4, r4, r5
60302d24:	e1a08009 	mov	r8, r9
60302d28:	e4d83001 	ldrb	r3, [r8], #1
60302d2c:	e1a01000 	mov	r1, r0
60302d30:	e3a0200a 	mov	r2, #10
60302d34:	e1a00006 	mov	r0, r6
60302d38:	e2433030 	sub	r3, r3, #48	; 0x30
60302d3c:	ebffff9e 	bl	60302bbc <__multadd>
60302d40:	e1580004 	cmp	r8, r4
60302d44:	1afffff7 	bne	60302d28 <__s2b+0x94>
60302d48:	e2454008 	sub	r4, r5, #8
60302d4c:	e0894004 	add	r4, r9, r4
60302d50:	e0455004 	sub	r5, r5, r4
60302d54:	e0843005 	add	r3, r4, r5
60302d58:	e1530007 	cmp	r3, r7
60302d5c:	a8bd87f0 	popge	{r4, r5, r6, r7, r8, r9, sl, pc}
60302d60:	e4d43001 	ldrb	r3, [r4], #1
60302d64:	e1a01000 	mov	r1, r0
60302d68:	e3a0200a 	mov	r2, #10
60302d6c:	e1a00006 	mov	r0, r6
60302d70:	e2433030 	sub	r3, r3, #48	; 0x30
60302d74:	ebffff90 	bl	60302bbc <__multadd>
60302d78:	eafffff5 	b	60302d54 <__s2b+0xc0>
60302d7c:	38e38e39 	.word	0x38e38e39
60302d80:	60316e3c 	.word	0x60316e3c
60302d84:	60316e57 	.word	0x60316e57

60302d88 <__hi0bits>:
60302d88:	e1a02820 	lsr	r2, r0, #16
60302d8c:	e1a03000 	mov	r3, r0
60302d90:	e1a02802 	lsl	r2, r2, #16
60302d94:	e3520000 	cmp	r2, #0
60302d98:	01a03800 	lsleq	r3, r0, #16
60302d9c:	13a00000 	movne	r0, #0
60302da0:	03a00010 	moveq	r0, #16
60302da4:	e31304ff 	tst	r3, #-16777216	; 0xff000000
60302da8:	01a03403 	lsleq	r3, r3, #8
60302dac:	02800008 	addeq	r0, r0, #8
60302db0:	e313020f 	tst	r3, #-268435456	; 0xf0000000
60302db4:	01a03203 	lsleq	r3, r3, #4
60302db8:	02800004 	addeq	r0, r0, #4
60302dbc:	e3130103 	tst	r3, #-1073741824	; 0xc0000000
60302dc0:	01a03103 	lsleq	r3, r3, #2
60302dc4:	02800002 	addeq	r0, r0, #2
60302dc8:	e3530000 	cmp	r3, #0
60302dcc:	b12fff1e 	bxlt	lr
60302dd0:	e3130101 	tst	r3, #1073741824	; 0x40000000
60302dd4:	12800001 	addne	r0, r0, #1
60302dd8:	03a00020 	moveq	r0, #32
60302ddc:	e12fff1e 	bx	lr

60302de0 <__lo0bits>:
60302de0:	e5903000 	ldr	r3, [r0]
60302de4:	e1a02000 	mov	r2, r0
60302de8:	e2130007 	ands	r0, r3, #7
60302dec:	0a000009 	beq	60302e18 <__lo0bits+0x38>
60302df0:	e3130001 	tst	r3, #1
60302df4:	1a00001c 	bne	60302e6c <__lo0bits+0x8c>
60302df8:	e3130002 	tst	r3, #2
60302dfc:	11a030a3 	lsrne	r3, r3, #1
60302e00:	13a00001 	movne	r0, #1
60302e04:	01a03123 	lsreq	r3, r3, #2
60302e08:	15823000 	strne	r3, [r2]
60302e0c:	03a00002 	moveq	r0, #2
60302e10:	05823000 	streq	r3, [r2]
60302e14:	e12fff1e 	bx	lr
60302e18:	e6ff1073 	uxth	r1, r3
60302e1c:	e3510000 	cmp	r1, #0
60302e20:	01a03823 	lsreq	r3, r3, #16
60302e24:	03a00010 	moveq	r0, #16
60302e28:	e6ef1073 	uxtb	r1, r3
60302e2c:	e3510000 	cmp	r1, #0
60302e30:	01a03423 	lsreq	r3, r3, #8
60302e34:	02800008 	addeq	r0, r0, #8
60302e38:	e313000f 	tst	r3, #15
60302e3c:	01a03223 	lsreq	r3, r3, #4
60302e40:	02800004 	addeq	r0, r0, #4
60302e44:	e3130003 	tst	r3, #3
60302e48:	01a03123 	lsreq	r3, r3, #2
60302e4c:	02800002 	addeq	r0, r0, #2
60302e50:	e3130001 	tst	r3, #1
60302e54:	1a000002 	bne	60302e64 <__lo0bits+0x84>
60302e58:	e2800001 	add	r0, r0, #1
60302e5c:	e1b030a3 	lsrs	r3, r3, #1
60302e60:	0a000003 	beq	60302e74 <__lo0bits+0x94>
60302e64:	e5823000 	str	r3, [r2]
60302e68:	e12fff1e 	bx	lr
60302e6c:	e3a00000 	mov	r0, #0
60302e70:	e12fff1e 	bx	lr
60302e74:	e3a00020 	mov	r0, #32
60302e78:	e12fff1e 	bx	lr

60302e7c <__i2b>:
60302e7c:	e92d4010 	push	{r4, lr}
60302e80:	e1a04001 	mov	r4, r1
60302e84:	e3a01001 	mov	r1, #1
60302e88:	ebfffef3 	bl	60302a5c <_Balloc>
60302e8c:	e2502000 	subs	r2, r0, #0
60302e90:	1a000003 	bne	60302ea4 <__i2b+0x28>
60302e94:	e59f3018 	ldr	r3, [pc, #24]	; 60302eb4 <__i2b+0x38>
60302e98:	e3a01d05 	mov	r1, #320	; 0x140
60302e9c:	e59f0014 	ldr	r0, [pc, #20]	; 60302eb8 <__i2b+0x3c>
60302ea0:	ebfff9c4 	bl	603015b8 <__assert_func>
60302ea4:	e3a03001 	mov	r3, #1
60302ea8:	e5824014 	str	r4, [r2, #20]
60302eac:	e5823010 	str	r3, [r2, #16]
60302eb0:	e8bd8010 	pop	{r4, pc}
60302eb4:	60316e3c 	.word	0x60316e3c
60302eb8:	60316e57 	.word	0x60316e57

60302ebc <__multiply>:
60302ebc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60302ec0:	e1a06002 	mov	r6, r2
60302ec4:	e5912010 	ldr	r2, [r1, #16]
60302ec8:	e1a04001 	mov	r4, r1
60302ecc:	e24dd014 	sub	sp, sp, #20
60302ed0:	e5963010 	ldr	r3, [r6, #16]
60302ed4:	e1520003 	cmp	r2, r3
60302ed8:	b1a03001 	movlt	r3, r1
60302edc:	b1a04006 	movlt	r4, r6
60302ee0:	b1a06003 	movlt	r6, r3
60302ee4:	e5949010 	ldr	r9, [r4, #16]
60302ee8:	e5967010 	ldr	r7, [r6, #16]
60302eec:	e5943008 	ldr	r3, [r4, #8]
60302ef0:	e5941004 	ldr	r1, [r4, #4]
60302ef4:	e0895007 	add	r5, r9, r7
60302ef8:	e1530005 	cmp	r3, r5
60302efc:	b2811001 	addlt	r1, r1, #1
60302f00:	ebfffed5 	bl	60302a5c <_Balloc>
60302f04:	e3500000 	cmp	r0, #0
60302f08:	1a000004 	bne	60302f20 <__multiply+0x64>
60302f0c:	e1a02000 	mov	r2, r0
60302f10:	e59f3168 	ldr	r3, [pc, #360]	; 60303080 <__multiply+0x1c4>
60302f14:	e59f0168 	ldr	r0, [pc, #360]	; 60303084 <__multiply+0x1c8>
60302f18:	e300115d 	movw	r1, #349	; 0x15d
60302f1c:	ebfff9a5 	bl	603015b8 <__assert_func>
60302f20:	e280c014 	add	ip, r0, #20
60302f24:	e3a02000 	mov	r2, #0
60302f28:	e08c8105 	add	r8, ip, r5, lsl #2
60302f2c:	e1a0300c 	mov	r3, ip
60302f30:	e1530008 	cmp	r3, r8
60302f34:	3a000018 	bcc	60302f9c <__multiply+0xe0>
60302f38:	e284e014 	add	lr, r4, #20
60302f3c:	e2863014 	add	r3, r6, #20
60302f40:	e08ee109 	add	lr, lr, r9, lsl #2
60302f44:	e0832107 	add	r2, r3, r7, lsl #2
60302f48:	e58d2008 	str	r2, [sp, #8]
60302f4c:	e04e2004 	sub	r2, lr, r4
60302f50:	e2422015 	sub	r2, r2, #21
60302f54:	e2841015 	add	r1, r4, #21
60302f58:	e3c22003 	bic	r2, r2, #3
60302f5c:	e15e0001 	cmp	lr, r1
60302f60:	e2822004 	add	r2, r2, #4
60302f64:	33a02004 	movcc	r2, #4
60302f68:	e58d2004 	str	r2, [sp, #4]
60302f6c:	e59d2008 	ldr	r2, [sp, #8]
60302f70:	e58d300c 	str	r3, [sp, #12]
60302f74:	e1520003 	cmp	r2, r3
60302f78:	8a000009 	bhi	60302fa4 <__multiply+0xe8>
60302f7c:	e3550000 	cmp	r5, #0
60302f80:	da000002 	ble	60302f90 <__multiply+0xd4>
60302f84:	e5383004 	ldr	r3, [r8, #-4]!
60302f88:	e3530000 	cmp	r3, #0
60302f8c:	0a000039 	beq	60303078 <__multiply+0x1bc>
60302f90:	e5805010 	str	r5, [r0, #16]
60302f94:	e28dd014 	add	sp, sp, #20
60302f98:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
60302f9c:	e4832004 	str	r2, [r3], #4
60302fa0:	eaffffe2 	b	60302f30 <__multiply+0x74>
60302fa4:	e1d3a0b0 	ldrh	sl, [r3]
60302fa8:	e35a0000 	cmp	sl, #0
60302fac:	0a000015 	beq	60303008 <__multiply+0x14c>
60302fb0:	e2847014 	add	r7, r4, #20
60302fb4:	e1a0900c 	mov	r9, ip
60302fb8:	e3a06000 	mov	r6, #0
60302fbc:	e4972004 	ldr	r2, [r7], #4
60302fc0:	e5991000 	ldr	r1, [r9]
60302fc4:	e15e0007 	cmp	lr, r7
60302fc8:	e6ffb072 	uxth	fp, r2
60302fcc:	e1a02822 	lsr	r2, r2, #16
60302fd0:	e6ff1071 	uxth	r1, r1
60302fd4:	e0211b9a 	mla	r1, sl, fp, r1
60302fd8:	e0811006 	add	r1, r1, r6
60302fdc:	e5996000 	ldr	r6, [r9]
60302fe0:	e1a06826 	lsr	r6, r6, #16
60302fe4:	e022629a 	mla	r2, sl, r2, r6
60302fe8:	e0822821 	add	r2, r2, r1, lsr #16
60302fec:	e6ff1071 	uxth	r1, r1
60302ff0:	e1811802 	orr	r1, r1, r2, lsl #16
60302ff4:	e1a06822 	lsr	r6, r2, #16
60302ff8:	e4891004 	str	r1, [r9], #4
60302ffc:	8affffee 	bhi	60302fbc <__multiply+0x100>
60303000:	e59d2004 	ldr	r2, [sp, #4]
60303004:	e78c6002 	str	r6, [ip, r2]
60303008:	e59d200c 	ldr	r2, [sp, #12]
6030300c:	e2833004 	add	r3, r3, #4
60303010:	e1d290b2 	ldrh	r9, [r2, #2]
60303014:	e3590000 	cmp	r9, #0
60303018:	0a000014 	beq	60303070 <__multiply+0x1b4>
6030301c:	e59c1000 	ldr	r1, [ip]
60303020:	e2846014 	add	r6, r4, #20
60303024:	e1a0700c 	mov	r7, ip
60303028:	e3a0a000 	mov	sl, #0
6030302c:	e1d6b0b0 	ldrh	fp, [r6]
60303030:	e6ff1071 	uxth	r1, r1
60303034:	e1d720b2 	ldrh	r2, [r7, #2]
60303038:	e0222b99 	mla	r2, r9, fp, r2
6030303c:	e082a00a 	add	sl, r2, sl
60303040:	e181180a 	orr	r1, r1, sl, lsl #16
60303044:	e4871004 	str	r1, [r7], #4
60303048:	e4962004 	ldr	r2, [r6], #4
6030304c:	e1d710b0 	ldrh	r1, [r7]
60303050:	e15e0006 	cmp	lr, r6
60303054:	e1a02822 	lsr	r2, r2, #16
60303058:	e0211299 	mla	r1, r9, r2, r1
6030305c:	e081182a 	add	r1, r1, sl, lsr #16
60303060:	e1a0a821 	lsr	sl, r1, #16
60303064:	8afffff0 	bhi	6030302c <__multiply+0x170>
60303068:	e59d2004 	ldr	r2, [sp, #4]
6030306c:	e78c1002 	str	r1, [ip, r2]
60303070:	e28cc004 	add	ip, ip, #4
60303074:	eaffffbc 	b	60302f6c <__multiply+0xb0>
60303078:	e2455001 	sub	r5, r5, #1
6030307c:	eaffffbe 	b	60302f7c <__multiply+0xc0>
60303080:	60316e3c 	.word	0x60316e3c
60303084:	60316e57 	.word	0x60316e57

60303088 <__pow5mult>:
60303088:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
6030308c:	e1a05002 	mov	r5, r2
60303090:	e2122003 	ands	r2, r2, #3
60303094:	e1a06000 	mov	r6, r0
60303098:	01a07001 	moveq	r7, r1
6030309c:	0a000005 	beq	603030b8 <__pow5mult+0x30>
603030a0:	e59fc0f4 	ldr	ip, [pc, #244]	; 6030319c <__pow5mult+0x114>
603030a4:	e2422001 	sub	r2, r2, #1
603030a8:	e3a03000 	mov	r3, #0
603030ac:	e79c2102 	ldr	r2, [ip, r2, lsl #2]
603030b0:	ebfffec1 	bl	60302bbc <__multadd>
603030b4:	e1a07000 	mov	r7, r0
603030b8:	e1b05145 	asrs	r5, r5, #2
603030bc:	0a000034 	beq	60303194 <__pow5mult+0x10c>
603030c0:	e5964024 	ldr	r4, [r6, #36]	; 0x24
603030c4:	e3540000 	cmp	r4, #0
603030c8:	1a00000d 	bne	60303104 <__pow5mult+0x7c>
603030cc:	e3a00010 	mov	r0, #16
603030d0:	eb002944 	bl	6030d5e8 <__wrap_malloc>
603030d4:	e1a02000 	mov	r2, r0
603030d8:	e3500000 	cmp	r0, #0
603030dc:	e5860024 	str	r0, [r6, #36]	; 0x24
603030e0:	1a000003 	bne	603030f4 <__pow5mult+0x6c>
603030e4:	e59f30b4 	ldr	r3, [pc, #180]	; 603031a0 <__pow5mult+0x118>
603030e8:	e30011ae 	movw	r1, #430	; 0x1ae
603030ec:	e59f00b0 	ldr	r0, [pc, #176]	; 603031a4 <__pow5mult+0x11c>
603030f0:	ebfff930 	bl	603015b8 <__assert_func>
603030f4:	e5804004 	str	r4, [r0, #4]
603030f8:	e5804008 	str	r4, [r0, #8]
603030fc:	e5804000 	str	r4, [r0]
60303100:	e580400c 	str	r4, [r0, #12]
60303104:	e5968024 	ldr	r8, [r6, #36]	; 0x24
60303108:	e5984008 	ldr	r4, [r8, #8]
6030310c:	e3540000 	cmp	r4, #0
60303110:	1a000006 	bne	60303130 <__pow5mult+0xa8>
60303114:	e1a00006 	mov	r0, r6
60303118:	e3001271 	movw	r1, #625	; 0x271
6030311c:	ebffff56 	bl	60302e7c <__i2b>
60303120:	e1a04000 	mov	r4, r0
60303124:	e3a03000 	mov	r3, #0
60303128:	e5880008 	str	r0, [r8, #8]
6030312c:	e5803000 	str	r3, [r0]
60303130:	e3a09000 	mov	r9, #0
60303134:	e3150001 	tst	r5, #1
60303138:	0a000008 	beq	60303160 <__pow5mult+0xd8>
6030313c:	e1a01007 	mov	r1, r7
60303140:	e1a02004 	mov	r2, r4
60303144:	e1a00006 	mov	r0, r6
60303148:	ebffff5b 	bl	60302ebc <__multiply>
6030314c:	e1a08000 	mov	r8, r0
60303150:	e1a01007 	mov	r1, r7
60303154:	e1a07008 	mov	r7, r8
60303158:	e1a00006 	mov	r0, r6
6030315c:	ebfffe77 	bl	60302b40 <_Bfree>
60303160:	e1b050c5 	asrs	r5, r5, #1
60303164:	0a00000a 	beq	60303194 <__pow5mult+0x10c>
60303168:	e5940000 	ldr	r0, [r4]
6030316c:	e3500000 	cmp	r0, #0
60303170:	1a000005 	bne	6030318c <__pow5mult+0x104>
60303174:	e1a02004 	mov	r2, r4
60303178:	e1a01004 	mov	r1, r4
6030317c:	e1a00006 	mov	r0, r6
60303180:	ebffff4d 	bl	60302ebc <__multiply>
60303184:	e5840000 	str	r0, [r4]
60303188:	e5809000 	str	r9, [r0]
6030318c:	e1a04000 	mov	r4, r0
60303190:	eaffffe7 	b	60303134 <__pow5mult+0xac>
60303194:	e1a00007 	mov	r0, r7
60303198:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
6030319c:	603151e0 	.word	0x603151e0
603031a0:	60316dde 	.word	0x60316dde
603031a4:	60316e57 	.word	0x60316e57

603031a8 <__lshift>:
603031a8:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
603031ac:	e1a04001 	mov	r4, r1
603031b0:	e5911004 	ldr	r1, [r1, #4]
603031b4:	e1a07000 	mov	r7, r0
603031b8:	e1a09002 	mov	r9, r2
603031bc:	e5948010 	ldr	r8, [r4, #16]
603031c0:	e1a0a2c2 	asr	sl, r2, #5
603031c4:	e5943008 	ldr	r3, [r4, #8]
603031c8:	e08882c2 	add	r8, r8, r2, asr #5
603031cc:	e2886001 	add	r6, r8, #1
603031d0:	e1530006 	cmp	r3, r6
603031d4:	ba000008 	blt	603031fc <__lshift+0x54>
603031d8:	e1a00007 	mov	r0, r7
603031dc:	ebfffe1e 	bl	60302a5c <_Balloc>
603031e0:	e2505000 	subs	r5, r0, #0
603031e4:	1a000007 	bne	60303208 <__lshift+0x60>
603031e8:	e59f30dc 	ldr	r3, [pc, #220]	; 603032cc <__lshift+0x124>
603031ec:	e1a02005 	mov	r2, r5
603031f0:	e59f00d8 	ldr	r0, [pc, #216]	; 603032d0 <__lshift+0x128>
603031f4:	e30011d9 	movw	r1, #473	; 0x1d9
603031f8:	ebfff8ee 	bl	603015b8 <__assert_func>
603031fc:	e2811001 	add	r1, r1, #1
60303200:	e1a03083 	lsl	r3, r3, #1
60303204:	eafffff1 	b	603031d0 <__lshift+0x28>
60303208:	e3a03000 	mov	r3, #0
6030320c:	e2851014 	add	r1, r5, #20
60303210:	e2852010 	add	r2, r5, #16
60303214:	e1a00003 	mov	r0, r3
60303218:	e153000a 	cmp	r3, sl
6030321c:	ba000021 	blt	603032a8 <__lshift+0x100>
60303220:	e5940010 	ldr	r0, [r4, #16]
60303224:	e1caafca 	bic	sl, sl, sl, asr #31
60303228:	e2843014 	add	r3, r4, #20
6030322c:	e219901f 	ands	r9, r9, #31
60303230:	e081110a 	add	r1, r1, sl, lsl #2
60303234:	e0830100 	add	r0, r3, r0, lsl #2
60303238:	0a00001d 	beq	603032b4 <__lshift+0x10c>
6030323c:	e2692020 	rsb	r2, r9, #32
60303240:	e1a0a001 	mov	sl, r1
60303244:	e3a0c000 	mov	ip, #0
60303248:	e593e000 	ldr	lr, [r3]
6030324c:	e18cc91e 	orr	ip, ip, lr, lsl r9
60303250:	e48ac004 	str	ip, [sl], #4
60303254:	e493c004 	ldr	ip, [r3], #4
60303258:	e1500003 	cmp	r0, r3
6030325c:	e1a0c23c 	lsr	ip, ip, r2
60303260:	8afffff8 	bhi	60303248 <__lshift+0xa0>
60303264:	e0403004 	sub	r3, r0, r4
60303268:	e2842015 	add	r2, r4, #21
6030326c:	e2433015 	sub	r3, r3, #21
60303270:	e1500002 	cmp	r0, r2
60303274:	e3c33003 	bic	r3, r3, #3
60303278:	e2833004 	add	r3, r3, #4
6030327c:	33a03004 	movcc	r3, #4
60303280:	e35c0000 	cmp	ip, #0
60303284:	12886002 	addne	r6, r8, #2
60303288:	e781c003 	str	ip, [r1, r3]
6030328c:	e1a00007 	mov	r0, r7
60303290:	e2466001 	sub	r6, r6, #1
60303294:	e1a01004 	mov	r1, r4
60303298:	e5856010 	str	r6, [r5, #16]
6030329c:	ebfffe27 	bl	60302b40 <_Bfree>
603032a0:	e1a00005 	mov	r0, r5
603032a4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
603032a8:	e2833001 	add	r3, r3, #1
603032ac:	e5a20004 	str	r0, [r2, #4]!
603032b0:	eaffffd8 	b	60303218 <__lshift+0x70>
603032b4:	e2411004 	sub	r1, r1, #4
603032b8:	e4932004 	ldr	r2, [r3], #4
603032bc:	e5a12004 	str	r2, [r1, #4]!
603032c0:	e1500003 	cmp	r0, r3
603032c4:	8afffffb 	bhi	603032b8 <__lshift+0x110>
603032c8:	eaffffef 	b	6030328c <__lshift+0xe4>
603032cc:	60316e3c 	.word	0x60316e3c
603032d0:	60316e57 	.word	0x60316e57

603032d4 <__mcmp>:
603032d4:	e5912010 	ldr	r2, [r1, #16]
603032d8:	e1a03000 	mov	r3, r0
603032dc:	e5900010 	ldr	r0, [r0, #16]
603032e0:	e0500002 	subs	r0, r0, r2
603032e4:	112fff1e 	bxne	lr
603032e8:	e2833014 	add	r3, r3, #20
603032ec:	e2811014 	add	r1, r1, #20
603032f0:	e083c102 	add	ip, r3, r2, lsl #2
603032f4:	e0811102 	add	r1, r1, r2, lsl #2
603032f8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
603032fc:	e53ce004 	ldr	lr, [ip, #-4]!
60303300:	e5312004 	ldr	r2, [r1, #-4]!
60303304:	e15e0002 	cmp	lr, r2
60303308:	0a000002 	beq	60303318 <__mcmp+0x44>
6030330c:	3a000004 	bcc	60303324 <__mcmp+0x50>
60303310:	e3a00001 	mov	r0, #1
60303314:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)
60303318:	e153000c 	cmp	r3, ip
6030331c:	3afffff6 	bcc	603032fc <__mcmp+0x28>
60303320:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)
60303324:	e3e00000 	mvn	r0, #0
60303328:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

6030332c <__mdiff>:
6030332c:	e92d4ff8 	push	{r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
60303330:	e1a04001 	mov	r4, r1
60303334:	e1a06000 	mov	r6, r0
60303338:	e1a01002 	mov	r1, r2
6030333c:	e1a00004 	mov	r0, r4
60303340:	e1a08002 	mov	r8, r2
60303344:	ebffffe2 	bl	603032d4 <__mcmp>
60303348:	e2505000 	subs	r5, r0, #0
6030334c:	1a00000b 	bne	60303380 <__mdiff+0x54>
60303350:	e1a01005 	mov	r1, r5
60303354:	e1a00006 	mov	r0, r6
60303358:	ebfffdbf 	bl	60302a5c <_Balloc>
6030335c:	e2502000 	subs	r2, r0, #0
60303360:	13a03001 	movne	r3, #1
60303364:	03001232 	movweq	r1, #562	; 0x232
60303368:	059f3138 	ldreq	r3, [pc, #312]	; 603034a8 <__mdiff+0x17c>
6030336c:	15823010 	strne	r3, [r2, #16]
60303370:	15825014 	strne	r5, [r2, #20]
60303374:	1a00003e 	bne	60303474 <__mdiff+0x148>
60303378:	e59f012c 	ldr	r0, [pc, #300]	; 603034ac <__mdiff+0x180>
6030337c:	ebfff88d 	bl	603015b8 <__assert_func>
60303380:	a1a03008 	movge	r3, r8
60303384:	a1a08004 	movge	r8, r4
60303388:	e1a00006 	mov	r0, r6
6030338c:	a1a04003 	movge	r4, r3
60303390:	a3a05000 	movge	r5, #0
60303394:	b3a05001 	movlt	r5, #1
60303398:	e5981004 	ldr	r1, [r8, #4]
6030339c:	ebfffdae 	bl	60302a5c <_Balloc>
603033a0:	e2502000 	subs	r2, r0, #0
603033a4:	059f30fc 	ldreq	r3, [pc, #252]	; 603034a8 <__mdiff+0x17c>
603033a8:	03a01d09 	moveq	r1, #576	; 0x240
603033ac:	0afffff1 	beq	60303378 <__mdiff+0x4c>
603033b0:	e594e010 	ldr	lr, [r4, #16]
603033b4:	e2880014 	add	r0, r8, #20
603033b8:	e582500c 	str	r5, [r2, #12]
603033bc:	e284a014 	add	sl, r4, #20
603033c0:	e5985010 	ldr	r5, [r8, #16]
603033c4:	e2827014 	add	r7, r2, #20
603033c8:	e08ae10e 	add	lr, sl, lr, lsl #2
603033cc:	e288c010 	add	ip, r8, #16
603033d0:	e1a0b007 	mov	fp, r7
603033d4:	e3a01000 	mov	r1, #0
603033d8:	e0806105 	add	r6, r0, r5, lsl #2
603033dc:	e49a8004 	ldr	r8, [sl], #4
603033e0:	e5bc3004 	ldr	r3, [ip, #4]!
603033e4:	e15e000a 	cmp	lr, sl
603033e8:	e6ff9078 	uxth	r9, r8
603033ec:	e1a08828 	lsr	r8, r8, #16
603033f0:	e6f11073 	uxtah	r1, r1, r3
603033f4:	e0683823 	rsb	r3, r8, r3, lsr #16
603033f8:	e0419009 	sub	r9, r1, r9
603033fc:	e0833849 	add	r3, r3, r9, asr #16
60303400:	e6ff9079 	uxth	r9, r9
60303404:	e1a01843 	asr	r1, r3, #16
60303408:	e1893803 	orr	r3, r9, r3, lsl #16
6030340c:	e48b3004 	str	r3, [fp], #4
60303410:	8afffff1 	bhi	603033dc <__mdiff+0xb0>
60303414:	e04e3004 	sub	r3, lr, r4
60303418:	e2844015 	add	r4, r4, #21
6030341c:	e2433015 	sub	r3, r3, #21
60303420:	e15e0004 	cmp	lr, r4
60303424:	e3c33003 	bic	r3, r3, #3
60303428:	e2833004 	add	r3, r3, #4
6030342c:	33a03004 	movcc	r3, #4
60303430:	e0800003 	add	r0, r0, r3
60303434:	e0873003 	add	r3, r7, r3
60303438:	e1a07003 	mov	r7, r3
6030343c:	e1a04000 	mov	r4, r0
60303440:	e1540006 	cmp	r4, r6
60303444:	3a00000c 	bcc	6030347c <__mdiff+0x150>
60303448:	e2861003 	add	r1, r6, #3
6030344c:	e0411000 	sub	r1, r1, r0
60303450:	e2400003 	sub	r0, r0, #3
60303454:	e1560000 	cmp	r6, r0
60303458:	e3c11003 	bic	r1, r1, #3
6030345c:	33a01000 	movcc	r1, #0
60303460:	e0831001 	add	r1, r3, r1
60303464:	e5313004 	ldr	r3, [r1, #-4]!
60303468:	e3530000 	cmp	r3, #0
6030346c:	0a00000b 	beq	603034a0 <__mdiff+0x174>
60303470:	e5825010 	str	r5, [r2, #16]
60303474:	e1a00002 	mov	r0, r2
60303478:	e8bd8ff8 	pop	{r3, r4, r5, r6, r7, r8, r9, sl, fp, pc}
6030347c:	e4948004 	ldr	r8, [r4], #4
60303480:	e6f1c078 	uxtah	ip, r1, r8
60303484:	e1a0e84c 	asr	lr, ip, #16
60303488:	e6ffc07c 	uxth	ip, ip
6030348c:	e08ee828 	add	lr, lr, r8, lsr #16
60303490:	e18cc80e 	orr	ip, ip, lr, lsl #16
60303494:	e1a0184e 	asr	r1, lr, #16
60303498:	e487c004 	str	ip, [r7], #4
6030349c:	eaffffe7 	b	60303440 <__mdiff+0x114>
603034a0:	e2455001 	sub	r5, r5, #1
603034a4:	eaffffee 	b	60303464 <__mdiff+0x138>
603034a8:	60316e3c 	.word	0x60316e3c
603034ac:	60316e57 	.word	0x60316e57

603034b0 <__ulp>:
603034b0:	e24dd008 	sub	sp, sp, #8
603034b4:	e59f1060 	ldr	r1, [pc, #96]	; 6030351c <__ulp+0x6c>
603034b8:	ed8d0b00 	vstr	d0, [sp]
603034bc:	e59d3004 	ldr	r3, [sp, #4]
603034c0:	e0011003 	and	r1, r1, r3
603034c4:	e241150d 	sub	r1, r1, #54525952	; 0x3400000
603034c8:	e3510000 	cmp	r1, #0
603034cc:	c3a02000 	movgt	r2, #0
603034d0:	c1a03001 	movgt	r3, r1
603034d4:	ca00000d 	bgt	60303510 <__ulp+0x60>
603034d8:	e2611000 	rsb	r1, r1, #0
603034dc:	e3a03000 	mov	r3, #0
603034e0:	e3510505 	cmp	r1, #20971520	; 0x1400000
603034e4:	e3a02000 	mov	r2, #0
603034e8:	e1a00a41 	asr	r0, r1, #20
603034ec:	b3a01702 	movlt	r1, #524288	; 0x80000
603034f0:	b1a03051 	asrlt	r3, r1, r0
603034f4:	ba000005 	blt	60303510 <__ulp+0x60>
603034f8:	e2401014 	sub	r1, r0, #20
603034fc:	e351001e 	cmp	r1, #30
60303500:	d3a00102 	movle	r0, #-2147483648	; 0x80000000
60303504:	c3a01001 	movgt	r1, #1
60303508:	d1a01130 	lsrle	r1, r0, r1
6030350c:	e1a02001 	mov	r2, r1
60303510:	ec432b10 	vmov	d0, r2, r3
60303514:	e28dd008 	add	sp, sp, #8
60303518:	e12fff1e 	bx	lr
6030351c:	7ff00000 	.word	0x7ff00000

60303520 <__b2d>:
60303520:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60303524:	e2807014 	add	r7, r0, #20
60303528:	e5904010 	ldr	r4, [r0, #16]
6030352c:	e0874104 	add	r4, r7, r4, lsl #2
60303530:	e2446004 	sub	r6, r4, #4
60303534:	e5145004 	ldr	r5, [r4, #-4]
60303538:	e1a00005 	mov	r0, r5
6030353c:	ebfffe11 	bl	60302d88 <__hi0bits>
60303540:	e59fc084 	ldr	ip, [pc, #132]	; 603035cc <__b2d+0xac>
60303544:	e2603020 	rsb	r3, r0, #32
60303548:	e350000a 	cmp	r0, #10
6030354c:	e5813000 	str	r3, [r1]
60303550:	ca000009 	bgt	6030357c <__b2d+0x5c>
60303554:	e1570006 	cmp	r7, r6
60303558:	e260e00b 	rsb	lr, r0, #11
6030355c:	23a01000 	movcs	r1, #0
60303560:	e2800015 	add	r0, r0, #21
60303564:	e18c3e35 	orr	r3, ip, r5, lsr lr
60303568:	35141008 	ldrcc	r1, [r4, #-8]
6030356c:	e1a01e31 	lsr	r1, r1, lr
60303570:	e1812015 	orr	r2, r1, r5, lsl r0
60303574:	ec432b10 	vmov	d0, r2, r3
60303578:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
6030357c:	e1570006 	cmp	r7, r6
60303580:	32446008 	subcc	r6, r4, #8
60303584:	23a01000 	movcs	r1, #0
60303588:	35141008 	ldrcc	r1, [r4, #-8]
6030358c:	e250000b 	subs	r0, r0, #11
60303590:	0a00000a 	beq	603035c0 <__b2d+0xa0>
60303594:	e1560007 	cmp	r6, r7
60303598:	e260e020 	rsb	lr, r0, #32
6030359c:	93a0c000 	movls	ip, #0
603035a0:	e1a03e31 	lsr	r3, r1, lr
603035a4:	8516c004 	ldrhi	ip, [r6, #-4]
603035a8:	e1835015 	orr	r5, r3, r5, lsl r0
603035ac:	e38535ff 	orr	r3, r5, #1069547520	; 0x3fc00000
603035b0:	e3833603 	orr	r3, r3, #3145728	; 0x300000
603035b4:	e1a0ce3c 	lsr	ip, ip, lr
603035b8:	e18c2011 	orr	r2, ip, r1, lsl r0
603035bc:	eaffffec 	b	60303574 <__b2d+0x54>
603035c0:	e185300c 	orr	r3, r5, ip
603035c4:	e1a02001 	mov	r2, r1
603035c8:	eaffffe9 	b	60303574 <__b2d+0x54>
603035cc:	3ff00000 	.word	0x3ff00000

603035d0 <__d2b>:
603035d0:	e92d43f7 	push	{r0, r1, r2, r4, r5, r6, r7, r8, r9, lr}
603035d4:	e1a09001 	mov	r9, r1
603035d8:	e3a01001 	mov	r1, #1
603035dc:	ec576b10 	vmov	r6, r7, d0
603035e0:	e1a08002 	mov	r8, r2
603035e4:	ebfffd1c 	bl	60302a5c <_Balloc>
603035e8:	e2504000 	subs	r4, r0, #0
603035ec:	1a000004 	bne	60303604 <__d2b+0x34>
603035f0:	e59f30e0 	ldr	r3, [pc, #224]	; 603036d8 <__d2b+0x108>
603035f4:	e1a02004 	mov	r2, r4
603035f8:	e59f00dc 	ldr	r0, [pc, #220]	; 603036dc <__d2b+0x10c>
603035fc:	e300130a 	movw	r1, #778	; 0x30a
60303600:	ebfff7ec 	bl	603015b8 <__assert_func>
60303604:	e7ea5a57 	ubfx	r5, r7, #20, #11
60303608:	e7f33057 	ubfx	r3, r7, #0, #20
6030360c:	e3550000 	cmp	r5, #0
60303610:	13833601 	orrne	r3, r3, #1048576	; 0x100000
60303614:	e3560000 	cmp	r6, #0
60303618:	e58d3004 	str	r3, [sp, #4]
6030361c:	0a00001c 	beq	60303694 <__d2b+0xc4>
60303620:	e1a0000d 	mov	r0, sp
60303624:	e58d6000 	str	r6, [sp]
60303628:	ebfffdec 	bl	60302de0 <__lo0bits>
6030362c:	e3500000 	cmp	r0, #0
60303630:	e59d3000 	ldr	r3, [sp]
60303634:	12601020 	rsbne	r1, r0, #32
60303638:	05843014 	streq	r3, [r4, #20]
6030363c:	159d2004 	ldrne	r2, [sp, #4]
60303640:	11833112 	orrne	r3, r3, r2, lsl r1
60303644:	15843014 	strne	r3, [r4, #20]
60303648:	11a02032 	lsrne	r2, r2, r0
6030364c:	158d2004 	strne	r2, [sp, #4]
60303650:	e59d3004 	ldr	r3, [sp, #4]
60303654:	e5843018 	str	r3, [r4, #24]
60303658:	e3530000 	cmp	r3, #0
6030365c:	13a01002 	movne	r1, #2
60303660:	03a01001 	moveq	r1, #1
60303664:	e5841010 	str	r1, [r4, #16]
60303668:	e3550000 	cmp	r5, #0
6030366c:	0a000010 	beq	603036b4 <__d2b+0xe4>
60303670:	e2455e43 	sub	r5, r5, #1072	; 0x430
60303674:	e2455003 	sub	r5, r5, #3
60303678:	e0855000 	add	r5, r5, r0
6030367c:	e2600035 	rsb	r0, r0, #53	; 0x35
60303680:	e5895000 	str	r5, [r9]
60303684:	e5880000 	str	r0, [r8]
60303688:	e1a00004 	mov	r0, r4
6030368c:	e28dd00c 	add	sp, sp, #12
60303690:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
60303694:	e28d0004 	add	r0, sp, #4
60303698:	ebfffdd0 	bl	60302de0 <__lo0bits>
6030369c:	e59d3004 	ldr	r3, [sp, #4]
603036a0:	e3a01001 	mov	r1, #1
603036a4:	e2800020 	add	r0, r0, #32
603036a8:	e5843014 	str	r3, [r4, #20]
603036ac:	e5841010 	str	r1, [r4, #16]
603036b0:	eaffffec 	b	60303668 <__d2b+0x98>
603036b4:	e0843101 	add	r3, r4, r1, lsl #2
603036b8:	e2400e43 	sub	r0, r0, #1072	; 0x430
603036bc:	e2400002 	sub	r0, r0, #2
603036c0:	e5890000 	str	r0, [r9]
603036c4:	e5930010 	ldr	r0, [r3, #16]
603036c8:	ebfffdae 	bl	60302d88 <__hi0bits>
603036cc:	e0601281 	rsb	r1, r0, r1, lsl #5
603036d0:	e5881000 	str	r1, [r8]
603036d4:	eaffffeb 	b	60303688 <__d2b+0xb8>
603036d8:	60316e3c 	.word	0x60316e3c
603036dc:	60316e57 	.word	0x60316e57

603036e0 <__ratio>:
603036e0:	e92d4ff7 	push	{r0, r1, r2, r4, r5, r6, r7, r8, r9, sl, fp, lr}
603036e4:	e1a09000 	mov	r9, r0
603036e8:	e1a08001 	mov	r8, r1
603036ec:	e1a0100d 	mov	r1, sp
603036f0:	ebffff8a 	bl	60303520 <__b2d>
603036f4:	e28d1004 	add	r1, sp, #4
603036f8:	e1a00008 	mov	r0, r8
603036fc:	ec554b10 	vmov	r4, r5, d0
60303700:	ee10aa10 	vmov	sl, s0
60303704:	ebffff85 	bl	60303520 <__b2d>
60303708:	e5993010 	ldr	r3, [r9, #16]
6030370c:	ec576b10 	vmov	r6, r7, d0
60303710:	e5982010 	ldr	r2, [r8, #16]
60303714:	ee100a10 	vmov	r0, s0
60303718:	e0432002 	sub	r2, r3, r2
6030371c:	e89d1008 	ldm	sp, {r3, ip}
60303720:	e043300c 	sub	r3, r3, ip
60303724:	e0833282 	add	r3, r3, r2, lsl #5
60303728:	e3530000 	cmp	r3, #0
6030372c:	d0633603 	rsble	r3, r3, r3, lsl #12
60303730:	c1a02005 	movgt	r2, r5
60303734:	d1a02007 	movle	r2, r7
60303738:	c1a01007 	movgt	r1, r7
6030373c:	c082ba03 	addgt	fp, r2, r3, lsl #20
60303740:	d1a0b005 	movle	fp, r5
60303744:	d0821a03 	addle	r1, r2, r3, lsl #20
60303748:	ec4bab30 	vmov	d16, sl, fp
6030374c:	ec410b31 	vmov	d17, r0, r1
60303750:	ee800ba1 	vdiv.f64	d0, d16, d17
60303754:	e28dd00c 	add	sp, sp, #12
60303758:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

6030375c <_mprec_log10>:
6030375c:	e3500017 	cmp	r0, #23
60303760:	ceb70b00 	vmovgt.f64	d0, #112	; 0x3f800000  1.0
60303764:	cef20b04 	vmovgt.f64	d16, #36	; 0x41200000  10.0
60303768:	ca000003 	bgt	6030377c <_mprec_log10+0x20>
6030376c:	e59f3018 	ldr	r3, [pc, #24]	; 6030378c <_mprec_log10+0x30>
60303770:	e0830180 	add	r0, r3, r0, lsl #3
60303774:	ed900b04 	vldr	d0, [r0, #16]
60303778:	e12fff1e 	bx	lr
6030377c:	e2500001 	subs	r0, r0, #1
60303780:	ee200b20 	vmul.f64	d0, d0, d16
60303784:	1afffffc 	bne	6030377c <_mprec_log10+0x20>
60303788:	e12fff1e 	bx	lr
6030378c:	603151e0 	.word	0x603151e0

60303790 <__copybits>:
60303790:	e2411001 	sub	r1, r1, #1
60303794:	e592c010 	ldr	ip, [r2, #16]
60303798:	e2823014 	add	r3, r2, #20
6030379c:	e92d4010 	push	{r4, lr}
603037a0:	e1a012c1 	asr	r1, r1, #5
603037a4:	e2811001 	add	r1, r1, #1
603037a8:	e083c10c 	add	ip, r3, ip, lsl #2
603037ac:	e240e004 	sub	lr, r0, #4
603037b0:	e0801101 	add	r1, r0, r1, lsl #2
603037b4:	e153000c 	cmp	r3, ip
603037b8:	3a00000b 	bcc	603037ec <__copybits+0x5c>
603037bc:	e04c3002 	sub	r3, ip, r2
603037c0:	e2822011 	add	r2, r2, #17
603037c4:	e2433011 	sub	r3, r3, #17
603037c8:	e152000c 	cmp	r2, ip
603037cc:	e3c33003 	bic	r3, r3, #3
603037d0:	83a03000 	movhi	r3, #0
603037d4:	e0800003 	add	r0, r0, r3
603037d8:	e3a03000 	mov	r3, #0
603037dc:	e1500001 	cmp	r0, r1
603037e0:	28bd8010 	popcs	{r4, pc}
603037e4:	e4803004 	str	r3, [r0], #4
603037e8:	eafffffb 	b	603037dc <__copybits+0x4c>
603037ec:	e4934004 	ldr	r4, [r3], #4
603037f0:	e5ae4004 	str	r4, [lr, #4]!
603037f4:	eaffffee 	b	603037b4 <__copybits+0x24>

603037f8 <__any_on>:
603037f8:	e2802014 	add	r2, r0, #20
603037fc:	e5900010 	ldr	r0, [r0, #16]
60303800:	e1a032c1 	asr	r3, r1, #5
60303804:	e1500003 	cmp	r0, r3
60303808:	ba00000b 	blt	6030383c <__any_on+0x44>
6030380c:	da000005 	ble	60303828 <__any_on+0x30>
60303810:	e211101f 	ands	r1, r1, #31
60303814:	0a000003 	beq	60303828 <__any_on+0x30>
60303818:	e7920103 	ldr	r0, [r2, r3, lsl #2]
6030381c:	e1a0c130 	lsr	ip, r0, r1
60303820:	e150011c 	cmp	r0, ip, lsl r1
60303824:	1a000009 	bne	60303850 <__any_on+0x58>
60303828:	e0823103 	add	r3, r2, r3, lsl #2
6030382c:	e1530002 	cmp	r3, r2
60303830:	8a000003 	bhi	60303844 <__any_on+0x4c>
60303834:	e3a00000 	mov	r0, #0
60303838:	e12fff1e 	bx	lr
6030383c:	e1a03000 	mov	r3, r0
60303840:	eafffff8 	b	60303828 <__any_on+0x30>
60303844:	e5331004 	ldr	r1, [r3, #-4]!
60303848:	e3510000 	cmp	r1, #0
6030384c:	0afffff6 	beq	6030382c <__any_on+0x34>
60303850:	e3a00001 	mov	r0, #1
60303854:	e12fff1e 	bx	lr

60303858 <_read_r>:
60303858:	e92d4070 	push	{r4, r5, r6, lr}
6030385c:	e1a04000 	mov	r4, r0
60303860:	e59f502c 	ldr	r5, [pc, #44]	; 60303894 <_read_r+0x3c>
60303864:	e1a00001 	mov	r0, r1
60303868:	e1a01002 	mov	r1, r2
6030386c:	e3a02000 	mov	r2, #0
60303870:	e5852000 	str	r2, [r5]
60303874:	e1a02003 	mov	r2, r3
60303878:	eb001c50 	bl	6030a9c0 <_read>
6030387c:	e3700001 	cmn	r0, #1
60303880:	18bd8070 	popne	{r4, r5, r6, pc}
60303884:	e5953000 	ldr	r3, [r5]
60303888:	e3530000 	cmp	r3, #0
6030388c:	15843000 	strne	r3, [r4]
60303890:	e8bd8070 	pop	{r4, r5, r6, pc}
60303894:	60317460 	.word	0x60317460

60303898 <frexp>:
60303898:	ee101a90 	vmov	r1, s1
6030389c:	e3a02000 	mov	r2, #0
603038a0:	e5802000 	str	r2, [r0]
603038a4:	e59f207c 	ldr	r2, [pc, #124]	; 60303928 <frexp+0x90>
603038a8:	e3c13102 	bic	r3, r1, #-2147483648	; 0x80000000
603038ac:	e1530002 	cmp	r3, r2
603038b0:	c12fff1e 	bxgt	lr
603038b4:	ee102a10 	vmov	r2, s0
603038b8:	e1932002 	orrs	r2, r3, r2
603038bc:	012fff1e 	bxeq	lr
603038c0:	e59f2064 	ldr	r2, [pc, #100]	; 6030392c <frexp+0x94>
603038c4:	e0022001 	and	r2, r2, r1
603038c8:	e3520000 	cmp	r2, #0
603038cc:	1a000005 	bne	603038e8 <frexp+0x50>
603038d0:	eddf0b12 	vldr	d16, [pc, #72]	; 60303920 <frexp+0x88>
603038d4:	e3e02035 	mvn	r2, #53	; 0x35
603038d8:	e5802000 	str	r2, [r0]
603038dc:	ee200b20 	vmul.f64	d0, d0, d16
603038e0:	ee101a90 	vmov	r1, s1
603038e4:	e3c13102 	bic	r3, r1, #-2147483648	; 0x80000000
603038e8:	e1a03a43 	asr	r3, r3, #20
603038ec:	e5902000 	ldr	r2, [r0]
603038f0:	e2433fff 	sub	r3, r3, #1020	; 0x3fc
603038f4:	e3c1147f 	bic	r1, r1, #2130706432	; 0x7f000000
603038f8:	e2433002 	sub	r3, r3, #2
603038fc:	e3c1160f 	bic	r1, r1, #15728640	; 0xf00000
60303900:	e0823003 	add	r3, r2, r3
60303904:	e5803000 	str	r3, [r0]
60303908:	ec532b10 	vmov	r2, r3, d0
6030390c:	e38135ff 	orr	r3, r1, #1069547520	; 0x3fc00000
60303910:	e3833602 	orr	r3, r3, #2097152	; 0x200000
60303914:	ec432b10 	vmov	d0, r2, r3
60303918:	e12fff1e 	bx	lr
6030391c:	e320f000 	nop	{0}
60303920:	00000000 	.word	0x00000000
60303924:	43500000 	.word	0x43500000
60303928:	7fefffff 	.word	0x7fefffff
6030392c:	7ff00000 	.word	0x7ff00000

60303930 <__sprint_r>:
60303930:	e92d4ff8 	push	{r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
60303934:	e1a04002 	mov	r4, r2
60303938:	e5923008 	ldr	r3, [r2, #8]
6030393c:	e3530000 	cmp	r3, #0
60303940:	1a000002 	bne	60303950 <__sprint_r+0x20>
60303944:	e1a00003 	mov	r0, r3
60303948:	e5823004 	str	r3, [r2, #4]
6030394c:	e8bd8ff8 	pop	{r3, r4, r5, r6, r7, r8, r9, sl, fp, pc}
60303950:	e5913064 	ldr	r3, [r1, #100]	; 0x64
60303954:	e1a07001 	mov	r7, r1
60303958:	e3130a02 	tst	r3, #8192	; 0x2000
6030395c:	0a00001b 	beq	603039d0 <__sprint_r+0xa0>
60303960:	e5925000 	ldr	r5, [r2]
60303964:	e1a08000 	mov	r8, r0
60303968:	e2855008 	add	r5, r5, #8
6030396c:	e5156004 	ldr	r6, [r5, #-4]
60303970:	e3a09000 	mov	r9, #0
60303974:	e515b008 	ldr	fp, [r5, #-8]
60303978:	e1a0a126 	lsr	sl, r6, #2
6030397c:	e15a0009 	cmp	sl, r9
60303980:	ca00000a 	bgt	603039b0 <__sprint_r+0x80>
60303984:	e5940008 	ldr	r0, [r4, #8]
60303988:	e3c66003 	bic	r6, r6, #3
6030398c:	e2855008 	add	r5, r5, #8
60303990:	e0400006 	sub	r0, r0, r6
60303994:	e5840008 	str	r0, [r4, #8]
60303998:	e3500000 	cmp	r0, #0
6030399c:	1afffff2 	bne	6030396c <__sprint_r+0x3c>
603039a0:	e3a03000 	mov	r3, #0
603039a4:	e5843008 	str	r3, [r4, #8]
603039a8:	e5843004 	str	r3, [r4, #4]
603039ac:	e8bd8ff8 	pop	{r3, r4, r5, r6, r7, r8, r9, sl, fp, pc}
603039b0:	e79b1109 	ldr	r1, [fp, r9, lsl #2]
603039b4:	e1a02007 	mov	r2, r7
603039b8:	e1a00008 	mov	r0, r8
603039bc:	eb000c8c 	bl	60306bf4 <_fputwc_r>
603039c0:	e3700001 	cmn	r0, #1
603039c4:	0afffff5 	beq	603039a0 <__sprint_r+0x70>
603039c8:	e2899001 	add	r9, r9, #1
603039cc:	eaffffea 	b	6030397c <__sprint_r+0x4c>
603039d0:	eb000cbc 	bl	60306cc8 <__sfvwrite_r>
603039d4:	eafffff1 	b	603039a0 <__sprint_r+0x70>

603039d8 <_vfiprintf_r>:
603039d8:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
603039dc:	e1a08001 	mov	r8, r1
603039e0:	e1a04003 	mov	r4, r3
603039e4:	e24dd0ec 	sub	sp, sp, #236	; 0xec
603039e8:	e1a05003 	mov	r5, r3
603039ec:	e2509000 	subs	r9, r0, #0
603039f0:	e58d2008 	str	r2, [sp, #8]
603039f4:	0a000003 	beq	60303a08 <_vfiprintf_r+0x30>
603039f8:	e5993018 	ldr	r3, [r9, #24]
603039fc:	e3530000 	cmp	r3, #0
60303a00:	1a000000 	bne	60303a08 <_vfiprintf_r+0x30>
60303a04:	ebfff535 	bl	60300ee0 <__sinit>
60303a08:	e5983064 	ldr	r3, [r8, #100]	; 0x64
60303a0c:	e3130001 	tst	r3, #1
60303a10:	1a000004 	bne	60303a28 <_vfiprintf_r+0x50>
60303a14:	e1d830bc 	ldrh	r3, [r8, #12]
60303a18:	e3130c02 	tst	r3, #512	; 0x200
60303a1c:	1a000001 	bne	60303a28 <_vfiprintf_r+0x50>
60303a20:	e5980058 	ldr	r0, [r8, #88]	; 0x58
60303a24:	ebfff5fb 	bl	60301218 <__retarget_lock_acquire_recursive>
60303a28:	e1d830fc 	ldrsh	r3, [r8, #12]
60303a2c:	e3130a02 	tst	r3, #8192	; 0x2000
60303a30:	1a000004 	bne	60303a48 <_vfiprintf_r+0x70>
60303a34:	e3833a02 	orr	r3, r3, #8192	; 0x2000
60303a38:	e1c830bc 	strh	r3, [r8, #12]
60303a3c:	e5983064 	ldr	r3, [r8, #100]	; 0x64
60303a40:	e3c33a02 	bic	r3, r3, #8192	; 0x2000
60303a44:	e5883064 	str	r3, [r8, #100]	; 0x64
60303a48:	e1d830bc 	ldrh	r3, [r8, #12]
60303a4c:	e3130008 	tst	r3, #8
60303a50:	0a000002 	beq	60303a60 <_vfiprintf_r+0x88>
60303a54:	e5983010 	ldr	r3, [r8, #16]
60303a58:	e3530000 	cmp	r3, #0
60303a5c:	1a000012 	bne	60303aac <_vfiprintf_r+0xd4>
60303a60:	e1a01008 	mov	r1, r8
60303a64:	e1a00009 	mov	r0, r9
60303a68:	eb000bd6 	bl	603069c8 <__swsetup_r>
60303a6c:	e3500000 	cmp	r0, #0
60303a70:	0a00000d 	beq	60303aac <_vfiprintf_r+0xd4>
60303a74:	e5983064 	ldr	r3, [r8, #100]	; 0x64
60303a78:	e3130001 	tst	r3, #1
60303a7c:	0a000004 	beq	60303a94 <_vfiprintf_r+0xbc>
60303a80:	e3e03000 	mvn	r3, #0
60303a84:	e58d300c 	str	r3, [sp, #12]
60303a88:	e59d000c 	ldr	r0, [sp, #12]
60303a8c:	e28dd0ec 	add	sp, sp, #236	; 0xec
60303a90:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
60303a94:	e1d830bc 	ldrh	r3, [r8, #12]
60303a98:	e3130c02 	tst	r3, #512	; 0x200
60303a9c:	1afffff7 	bne	60303a80 <_vfiprintf_r+0xa8>
60303aa0:	e5980058 	ldr	r0, [r8, #88]	; 0x58
60303aa4:	ebfff5e1 	bl	60301230 <__retarget_lock_release_recursive>
60303aa8:	eafffff4 	b	60303a80 <_vfiprintf_r+0xa8>
60303aac:	e1d830bc 	ldrh	r3, [r8, #12]
60303ab0:	e203201a 	and	r2, r3, #26
60303ab4:	e352000a 	cmp	r2, #10
60303ab8:	1a000010 	bne	60303b00 <_vfiprintf_r+0x128>
60303abc:	e1d820fe 	ldrsh	r2, [r8, #14]
60303ac0:	e3520000 	cmp	r2, #0
60303ac4:	ba00000d 	blt	60303b00 <_vfiprintf_r+0x128>
60303ac8:	e5982064 	ldr	r2, [r8, #100]	; 0x64
60303acc:	e3120001 	tst	r2, #1
60303ad0:	1a000003 	bne	60303ae4 <_vfiprintf_r+0x10c>
60303ad4:	e3130c02 	tst	r3, #512	; 0x200
60303ad8:	1a000001 	bne	60303ae4 <_vfiprintf_r+0x10c>
60303adc:	e5980058 	ldr	r0, [r8, #88]	; 0x58
60303ae0:	ebfff5d2 	bl	60301230 <__retarget_lock_release_recursive>
60303ae4:	e59d2008 	ldr	r2, [sp, #8]
60303ae8:	e1a03004 	mov	r3, r4
60303aec:	e1a01008 	mov	r1, r8
60303af0:	e1a00009 	mov	r0, r9
60303af4:	e28dd0ec 	add	sp, sp, #236	; 0xec
60303af8:	e8bd4ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60303afc:	ea00036d 	b	603048b8 <__sbprintf>
60303b00:	e3a03000 	mov	r3, #0
60303b04:	e28d7044 	add	r7, sp, #68	; 0x44
60303b08:	e58d3040 	str	r3, [sp, #64]	; 0x40
60303b0c:	e58d7038 	str	r7, [sp, #56]	; 0x38
60303b10:	e58d303c 	str	r3, [sp, #60]	; 0x3c
60303b14:	e58d3018 	str	r3, [sp, #24]
60303b18:	e58d301c 	str	r3, [sp, #28]
60303b1c:	e58d3020 	str	r3, [sp, #32]
60303b20:	e58d3024 	str	r3, [sp, #36]	; 0x24
60303b24:	e58d300c 	str	r3, [sp, #12]
60303b28:	e59d2008 	ldr	r2, [sp, #8]
60303b2c:	e5d23000 	ldrb	r3, [r2]
60303b30:	e1a04002 	mov	r4, r2
60303b34:	e2822001 	add	r2, r2, #1
60303b38:	e3530000 	cmp	r3, #0
60303b3c:	13530025 	cmpne	r3, #37	; 0x25
60303b40:	1afffff9 	bne	60303b2c <_vfiprintf_r+0x154>
60303b44:	e59d3008 	ldr	r3, [sp, #8]
60303b48:	e0546003 	subs	r6, r4, r3
60303b4c:	0a000013 	beq	60303ba0 <_vfiprintf_r+0x1c8>
60303b50:	e8870048 	stm	r7, {r3, r6}
60303b54:	e59d3040 	ldr	r3, [sp, #64]	; 0x40
60303b58:	e0833006 	add	r3, r3, r6
60303b5c:	e58d3040 	str	r3, [sp, #64]	; 0x40
60303b60:	e59d303c 	ldr	r3, [sp, #60]	; 0x3c
60303b64:	e2833001 	add	r3, r3, #1
60303b68:	e58d303c 	str	r3, [sp, #60]	; 0x3c
60303b6c:	e3530007 	cmp	r3, #7
60303b70:	d2877008 	addle	r7, r7, #8
60303b74:	da000006 	ble	60303b94 <_vfiprintf_r+0x1bc>
60303b78:	e28d2038 	add	r2, sp, #56	; 0x38
60303b7c:	e1a01008 	mov	r1, r8
60303b80:	e1a00009 	mov	r0, r9
60303b84:	ebffff69 	bl	60303930 <__sprint_r>
60303b88:	e3500000 	cmp	r0, #0
60303b8c:	1a0002cc 	bne	603046c4 <_vfiprintf_r+0xcec>
60303b90:	e28d7044 	add	r7, sp, #68	; 0x44
60303b94:	e59d300c 	ldr	r3, [sp, #12]
60303b98:	e0833006 	add	r3, r3, r6
60303b9c:	e58d300c 	str	r3, [sp, #12]
60303ba0:	e5d43000 	ldrb	r3, [r4]
60303ba4:	e3530000 	cmp	r3, #0
60303ba8:	0a0002e1 	beq	60304734 <_vfiprintf_r+0xd5c>
60303bac:	e3a03000 	mov	r3, #0
60303bb0:	e2842001 	add	r2, r4, #1
60303bb4:	e1a04003 	mov	r4, r3
60303bb8:	e3e01000 	mvn	r1, #0
60303bbc:	e5cd3033 	strb	r3, [sp, #51]	; 0x33
60303bc0:	e58d1004 	str	r1, [sp, #4]
60303bc4:	e58d3010 	str	r3, [sp, #16]
60303bc8:	e4d23001 	ldrb	r3, [r2], #1
60303bcc:	e58d2008 	str	r2, [sp, #8]
60303bd0:	e2432020 	sub	r2, r3, #32
60303bd4:	e352005a 	cmp	r2, #90	; 0x5a
60303bd8:	979ff102 	ldrls	pc, [pc, r2, lsl #2]
60303bdc:	ea000250 	b	60304524 <_vfiprintf_r+0xb4c>
60303be0:	60303de8 	.word	0x60303de8
60303be4:	60304524 	.word	0x60304524
60303be8:	60304524 	.word	0x60304524
60303bec:	60303e00 	.word	0x60303e00
60303bf0:	60304524 	.word	0x60304524
60303bf4:	60304524 	.word	0x60304524
60303bf8:	60304524 	.word	0x60304524
60303bfc:	60303d98 	.word	0x60303d98
60303c00:	60304524 	.word	0x60304524
60303c04:	60304524 	.word	0x60304524
60303c08:	60303e08 	.word	0x60303e08
60303c0c:	60303e28 	.word	0x60303e28
60303c10:	60304524 	.word	0x60304524
60303c14:	60303e20 	.word	0x60303e20
60303c18:	60303e30 	.word	0x60303e30
60303c1c:	60304524 	.word	0x60304524
60303c20:	60303e84 	.word	0x60303e84
60303c24:	60303e8c 	.word	0x60303e8c
60303c28:	60303e8c 	.word	0x60303e8c
60303c2c:	60303e8c 	.word	0x60303e8c
60303c30:	60303e8c 	.word	0x60303e8c
60303c34:	60303e8c 	.word	0x60303e8c
60303c38:	60303e8c 	.word	0x60303e8c
60303c3c:	60303e8c 	.word	0x60303e8c
60303c40:	60303e8c 	.word	0x60303e8c
60303c44:	60303e8c 	.word	0x60303e8c
60303c48:	60304524 	.word	0x60304524
60303c4c:	60304524 	.word	0x60304524
60303c50:	60304524 	.word	0x60304524
60303c54:	60304524 	.word	0x60304524
60303c58:	60304524 	.word	0x60304524
60303c5c:	60304524 	.word	0x60304524
60303c60:	60304524 	.word	0x60304524
60303c64:	60304524 	.word	0x60304524
60303c68:	60304524 	.word	0x60304524
60303c6c:	60303f0c 	.word	0x60303f0c
60303c70:	60303f34 	.word	0x60303f34
60303c74:	60304524 	.word	0x60304524
60303c78:	60304524 	.word	0x60304524
60303c7c:	60304524 	.word	0x60304524
60303c80:	60304524 	.word	0x60304524
60303c84:	60304524 	.word	0x60304524
60303c88:	60304524 	.word	0x60304524
60303c8c:	60304524 	.word	0x60304524
60303c90:	60304524 	.word	0x60304524
60303c94:	60304524 	.word	0x60304524
60303c98:	60304524 	.word	0x60304524
60303c9c:	60304094 	.word	0x60304094
60303ca0:	60304524 	.word	0x60304524
60303ca4:	60304524 	.word	0x60304524
60303ca8:	60304524 	.word	0x60304524
60303cac:	60304124 	.word	0x60304124
60303cb0:	60304524 	.word	0x60304524
60303cb4:	60304410 	.word	0x60304410
60303cb8:	60304524 	.word	0x60304524
60303cbc:	60304524 	.word	0x60304524
60303cc0:	6030446c 	.word	0x6030446c
60303cc4:	60304524 	.word	0x60304524
60303cc8:	60304524 	.word	0x60304524
60303ccc:	60304524 	.word	0x60304524
60303cd0:	60304524 	.word	0x60304524
60303cd4:	60304524 	.word	0x60304524
60303cd8:	60304524 	.word	0x60304524
60303cdc:	60304524 	.word	0x60304524
60303ce0:	60304524 	.word	0x60304524
60303ce4:	60304524 	.word	0x60304524
60303ce8:	60304524 	.word	0x60304524
60303cec:	60303f0c 	.word	0x60303f0c
60303cf0:	60303f38 	.word	0x60303f38
60303cf4:	60304524 	.word	0x60304524
60303cf8:	60304524 	.word	0x60304524
60303cfc:	60304524 	.word	0x60304524
60303d00:	60303ec0 	.word	0x60303ec0
60303d04:	60303f38 	.word	0x60303f38
60303d08:	60303f04 	.word	0x60303f04
60303d0c:	60304524 	.word	0x60304524
60303d10:	60303ee4 	.word	0x60303ee4
60303d14:	60304524 	.word	0x60304524
60303d18:	60304028 	.word	0x60304028
60303d1c:	60304098 	.word	0x60304098
60303d20:	603040fc 	.word	0x603040fc
60303d24:	60303f04 	.word	0x60303f04
60303d28:	60304524 	.word	0x60304524
60303d2c:	60304124 	.word	0x60304124
60303d30:	60303de0 	.word	0x60303de0
60303d34:	60304414 	.word	0x60304414
60303d38:	60304524 	.word	0x60304524
60303d3c:	60304524 	.word	0x60304524
60303d40:	60303d4c 	.word	0x60303d4c
60303d44:	60304524 	.word	0x60304524
60303d48:	60303de0 	.word	0x60303de0
60303d4c:	e59f2b38 	ldr	r2, [pc, #2872]	; 6030488c <_vfiprintf_r+0xeb4>
60303d50:	e58d2018 	str	r2, [sp, #24]
60303d54:	e2142020 	ands	r2, r4, #32
60303d58:	0a0001c5 	beq	60304474 <_vfiprintf_r+0xa9c>
60303d5c:	e2855007 	add	r5, r5, #7
60303d60:	e3c52007 	bic	r2, r5, #7
60303d64:	e1a0a002 	mov	sl, r2
60303d68:	e5926004 	ldr	r6, [r2, #4]
60303d6c:	e49a5008 	ldr	r5, [sl], #8
60303d70:	e1952006 	orrs	r2, r5, r6
60303d74:	12042001 	andne	r2, r4, #1
60303d78:	03a02000 	moveq	r2, #0
60303d7c:	e3520000 	cmp	r2, #0
60303d80:	15cd3035 	strbne	r3, [sp, #53]	; 0x35
60303d84:	13844002 	orrne	r4, r4, #2
60303d88:	13a02030 	movne	r2, #48	; 0x30
60303d8c:	e3c44b01 	bic	r4, r4, #1024	; 0x400
60303d90:	15cd2034 	strbne	r2, [sp, #52]	; 0x34
60303d94:	ea0000e0 	b	6030411c <_vfiprintf_r+0x744>
60303d98:	e1a00009 	mov	r0, r9
60303d9c:	ebfffb01 	bl	603029a8 <_localeconv_r>
60303da0:	e5903004 	ldr	r3, [r0, #4]
60303da4:	e58d3024 	str	r3, [sp, #36]	; 0x24
60303da8:	e1a00003 	mov	r0, r3
60303dac:	eb004419 	bl	60314e18 <__wrap_strlen>
60303db0:	e58d0020 	str	r0, [sp, #32]
60303db4:	e1a00009 	mov	r0, r9
60303db8:	ebfffafa 	bl	603029a8 <_localeconv_r>
60303dbc:	e5903008 	ldr	r3, [r0, #8]
60303dc0:	e58d301c 	str	r3, [sp, #28]
60303dc4:	e1cd21dc 	ldrd	r2, [sp, #28]
60303dc8:	e3530000 	cmp	r3, #0
60303dcc:	13520000 	cmpne	r2, #0
60303dd0:	0a000002 	beq	60303de0 <_vfiprintf_r+0x408>
60303dd4:	e5d23000 	ldrb	r3, [r2]
60303dd8:	e3530000 	cmp	r3, #0
60303ddc:	13844b01 	orrne	r4, r4, #1024	; 0x400
60303de0:	e59d2008 	ldr	r2, [sp, #8]
60303de4:	eaffff77 	b	60303bc8 <_vfiprintf_r+0x1f0>
60303de8:	e5dd3033 	ldrb	r3, [sp, #51]	; 0x33
60303dec:	e3530000 	cmp	r3, #0
60303df0:	03a03020 	moveq	r3, #32
60303df4:	1afffff9 	bne	60303de0 <_vfiprintf_r+0x408>
60303df8:	e5cd3033 	strb	r3, [sp, #51]	; 0x33
60303dfc:	eafffff7 	b	60303de0 <_vfiprintf_r+0x408>
60303e00:	e3844001 	orr	r4, r4, #1
60303e04:	eafffff5 	b	60303de0 <_vfiprintf_r+0x408>
60303e08:	e4953004 	ldr	r3, [r5], #4
60303e0c:	e58d3010 	str	r3, [sp, #16]
60303e10:	e3530000 	cmp	r3, #0
60303e14:	aafffff1 	bge	60303de0 <_vfiprintf_r+0x408>
60303e18:	e2633000 	rsb	r3, r3, #0
60303e1c:	e58d3010 	str	r3, [sp, #16]
60303e20:	e3844004 	orr	r4, r4, #4
60303e24:	eaffffed 	b	60303de0 <_vfiprintf_r+0x408>
60303e28:	e3a0302b 	mov	r3, #43	; 0x2b
60303e2c:	eafffff1 	b	60303df8 <_vfiprintf_r+0x420>
60303e30:	e59d2008 	ldr	r2, [sp, #8]
60303e34:	e4d23001 	ldrb	r3, [r2], #1
60303e38:	e353002a 	cmp	r3, #42	; 0x2a
60303e3c:	1a00000d 	bne	60303e78 <_vfiprintf_r+0x4a0>
60303e40:	e4950004 	ldr	r0, [r5], #4
60303e44:	e58d2008 	str	r2, [sp, #8]
60303e48:	e1803fc0 	orr	r3, r0, r0, asr #31
60303e4c:	e58d3004 	str	r3, [sp, #4]
60303e50:	eaffffe2 	b	60303de0 <_vfiprintf_r+0x408>
60303e54:	e59d3004 	ldr	r3, [sp, #4]
60303e58:	e3a0000a 	mov	r0, #10
60303e5c:	e0231390 	mla	r3, r0, r3, r1
60303e60:	e58d3004 	str	r3, [sp, #4]
60303e64:	e4d23001 	ldrb	r3, [r2], #1
60303e68:	e2431030 	sub	r1, r3, #48	; 0x30
60303e6c:	e3510009 	cmp	r1, #9
60303e70:	9afffff7 	bls	60303e54 <_vfiprintf_r+0x47c>
60303e74:	eaffff54 	b	60303bcc <_vfiprintf_r+0x1f4>
60303e78:	e3a01000 	mov	r1, #0
60303e7c:	e58d1004 	str	r1, [sp, #4]
60303e80:	eafffff8 	b	60303e68 <_vfiprintf_r+0x490>
60303e84:	e3844080 	orr	r4, r4, #128	; 0x80
60303e88:	eaffffd4 	b	60303de0 <_vfiprintf_r+0x408>
60303e8c:	e59d2008 	ldr	r2, [sp, #8]
60303e90:	e3a01000 	mov	r1, #0
60303e94:	e58d1010 	str	r1, [sp, #16]
60303e98:	e59d1010 	ldr	r1, [sp, #16]
60303e9c:	e2433030 	sub	r3, r3, #48	; 0x30
60303ea0:	e3a0000a 	mov	r0, #10
60303ea4:	e0233190 	mla	r3, r0, r1, r3
60303ea8:	e58d3010 	str	r3, [sp, #16]
60303eac:	e4d23001 	ldrb	r3, [r2], #1
60303eb0:	e2431030 	sub	r1, r3, #48	; 0x30
60303eb4:	e3510009 	cmp	r1, #9
60303eb8:	9afffff6 	bls	60303e98 <_vfiprintf_r+0x4c0>
60303ebc:	eaffff42 	b	60303bcc <_vfiprintf_r+0x1f4>
60303ec0:	e59d3008 	ldr	r3, [sp, #8]
60303ec4:	e5d33000 	ldrb	r3, [r3]
60303ec8:	e3530068 	cmp	r3, #104	; 0x68
60303ecc:	03844c02 	orreq	r4, r4, #512	; 0x200
60303ed0:	13844040 	orrne	r4, r4, #64	; 0x40
60303ed4:	059d3008 	ldreq	r3, [sp, #8]
60303ed8:	02833001 	addeq	r3, r3, #1
60303edc:	058d3008 	streq	r3, [sp, #8]
60303ee0:	eaffffbe 	b	60303de0 <_vfiprintf_r+0x408>
60303ee4:	e59d3008 	ldr	r3, [sp, #8]
60303ee8:	e5d33000 	ldrb	r3, [r3]
60303eec:	e353006c 	cmp	r3, #108	; 0x6c
60303ef0:	13844010 	orrne	r4, r4, #16
60303ef4:	059d3008 	ldreq	r3, [sp, #8]
60303ef8:	02833001 	addeq	r3, r3, #1
60303efc:	058d3008 	streq	r3, [sp, #8]
60303f00:	1affffb6 	bne	60303de0 <_vfiprintf_r+0x408>
60303f04:	e3844020 	orr	r4, r4, #32
60303f08:	eaffffb4 	b	60303de0 <_vfiprintf_r+0x408>
60303f0c:	e1a0a005 	mov	sl, r5
60303f10:	e3a0b000 	mov	fp, #0
60303f14:	e5cdb033 	strb	fp, [sp, #51]	; 0x33
60303f18:	e49a3004 	ldr	r3, [sl], #4
60303f1c:	e5cd3084 	strb	r3, [sp, #132]	; 0x84
60303f20:	e3a03001 	mov	r3, #1
60303f24:	e1a0600b 	mov	r6, fp
60303f28:	e58d3004 	str	r3, [sp, #4]
60303f2c:	e28db084 	add	fp, sp, #132	; 0x84
60303f30:	ea000090 	b	60304178 <_vfiprintf_r+0x7a0>
60303f34:	e3844010 	orr	r4, r4, #16
60303f38:	e3140020 	tst	r4, #32
60303f3c:	0a000029 	beq	60303fe8 <_vfiprintf_r+0x610>
60303f40:	e2855007 	add	r5, r5, #7
60303f44:	e3c53007 	bic	r3, r5, #7
60303f48:	e1a0a003 	mov	sl, r3
60303f4c:	e5936004 	ldr	r6, [r3, #4]
60303f50:	e49a5008 	ldr	r5, [sl], #8
60303f54:	e3560000 	cmp	r6, #0
60303f58:	aa000202 	bge	60304768 <_vfiprintf_r+0xd90>
60303f5c:	e2755000 	rsbs	r5, r5, #0
60303f60:	e3a0302d 	mov	r3, #45	; 0x2d
60303f64:	e2e66000 	rsc	r6, r6, #0
60303f68:	e5cd3033 	strb	r3, [sp, #51]	; 0x33
60303f6c:	e3a03001 	mov	r3, #1
60303f70:	e59d2004 	ldr	r2, [sp, #4]
60303f74:	e3720001 	cmn	r2, #1
60303f78:	11a01004 	movne	r1, r4
60303f7c:	1a000149 	bne	603044a8 <_vfiprintf_r+0xad0>
60303f80:	e3530001 	cmp	r3, #1
60303f84:	0a0001fc 	beq	6030477c <_vfiprintf_r+0xda4>
60303f88:	e28db0e8 	add	fp, sp, #232	; 0xe8
60303f8c:	e3530002 	cmp	r3, #2
60303f90:	0a000156 	beq	603044f0 <_vfiprintf_r+0xb18>
60303f94:	e2053007 	and	r3, r5, #7
60303f98:	e1a051a5 	lsr	r5, r5, #3
60303f9c:	e1855e86 	orr	r5, r5, r6, lsl #29
60303fa0:	e1a061a6 	lsr	r6, r6, #3
60303fa4:	e1a0200b 	mov	r2, fp
60303fa8:	e2833030 	add	r3, r3, #48	; 0x30
60303fac:	e1951006 	orrs	r1, r5, r6
60303fb0:	e56b3001 	strb	r3, [fp, #-1]!
60303fb4:	1afffff6 	bne	60303f94 <_vfiprintf_r+0x5bc>
60303fb8:	e3530030 	cmp	r3, #48	; 0x30
60303fbc:	03a01000 	moveq	r1, #0
60303fc0:	12041001 	andne	r1, r4, #1
60303fc4:	e3510000 	cmp	r1, #0
60303fc8:	13a03030 	movne	r3, #48	; 0x30
60303fcc:	154b3001 	strbne	r3, [fp, #-1]
60303fd0:	1242b002 	subne	fp, r2, #2
60303fd4:	e28d30e8 	add	r3, sp, #232	; 0xe8
60303fd8:	e59d6004 	ldr	r6, [sp, #4]
60303fdc:	e043300b 	sub	r3, r3, fp
60303fe0:	e58d3004 	str	r3, [sp, #4]
60303fe4:	ea000063 	b	60304178 <_vfiprintf_r+0x7a0>
60303fe8:	e1a0a005 	mov	sl, r5
60303fec:	e3140010 	tst	r4, #16
60303ff0:	e49a6004 	ldr	r6, [sl], #4
60303ff4:	0a000002 	beq	60304004 <_vfiprintf_r+0x62c>
60303ff8:	e1a05006 	mov	r5, r6
60303ffc:	e1a06fc6 	asr	r6, r6, #31
60304000:	eaffffd3 	b	60303f54 <_vfiprintf_r+0x57c>
60304004:	e3140040 	tst	r4, #64	; 0x40
60304008:	16bf5076 	sxthne	r5, r6
6030400c:	17a067d6 	sbfxne	r6, r6, #15, #1
60304010:	1affffcf 	bne	60303f54 <_vfiprintf_r+0x57c>
60304014:	e3140c02 	tst	r4, #512	; 0x200
60304018:	16af5076 	sxtbne	r5, r6
6030401c:	17a063d6 	sbfxne	r6, r6, #7, #1
60304020:	1affffcb 	bne	60303f54 <_vfiprintf_r+0x57c>
60304024:	eafffff3 	b	60303ff8 <_vfiprintf_r+0x620>
60304028:	e285a004 	add	sl, r5, #4
6030402c:	e3140020 	tst	r4, #32
60304030:	0a000006 	beq	60304050 <_vfiprintf_r+0x678>
60304034:	e5953000 	ldr	r3, [r5]
60304038:	e59d200c 	ldr	r2, [sp, #12]
6030403c:	e59d100c 	ldr	r1, [sp, #12]
60304040:	e1a02fc2 	asr	r2, r2, #31
60304044:	e8830006 	stm	r3, {r1, r2}
60304048:	e1a0500a 	mov	r5, sl
6030404c:	eafffeb5 	b	60303b28 <_vfiprintf_r+0x150>
60304050:	e3140010 	tst	r4, #16
60304054:	0a000003 	beq	60304068 <_vfiprintf_r+0x690>
60304058:	e5953000 	ldr	r3, [r5]
6030405c:	e59d200c 	ldr	r2, [sp, #12]
60304060:	e5832000 	str	r2, [r3]
60304064:	eafffff7 	b	60304048 <_vfiprintf_r+0x670>
60304068:	e3140040 	tst	r4, #64	; 0x40
6030406c:	15953000 	ldrne	r3, [r5]
60304070:	159d200c 	ldrne	r2, [sp, #12]
60304074:	11c320b0 	strhne	r2, [r3]
60304078:	1afffff2 	bne	60304048 <_vfiprintf_r+0x670>
6030407c:	e3140c02 	tst	r4, #512	; 0x200
60304080:	15953000 	ldrne	r3, [r5]
60304084:	159d200c 	ldrne	r2, [sp, #12]
60304088:	15c32000 	strbne	r2, [r3]
6030408c:	1affffed 	bne	60304048 <_vfiprintf_r+0x670>
60304090:	eafffff0 	b	60304058 <_vfiprintf_r+0x680>
60304094:	e3844010 	orr	r4, r4, #16
60304098:	e2143020 	ands	r3, r4, #32
6030409c:	0a000009 	beq	603040c8 <_vfiprintf_r+0x6f0>
603040a0:	e2855007 	add	r5, r5, #7
603040a4:	e3c53007 	bic	r3, r5, #7
603040a8:	e1a0a003 	mov	sl, r3
603040ac:	e5936004 	ldr	r6, [r3, #4]
603040b0:	e49a5008 	ldr	r5, [sl], #8
603040b4:	e3c44b01 	bic	r4, r4, #1024	; 0x400
603040b8:	e3a03000 	mov	r3, #0
603040bc:	e3a02000 	mov	r2, #0
603040c0:	e5cd2033 	strb	r2, [sp, #51]	; 0x33
603040c4:	eaffffa9 	b	60303f70 <_vfiprintf_r+0x598>
603040c8:	e1a0a005 	mov	sl, r5
603040cc:	e2146010 	ands	r6, r4, #16
603040d0:	e49a5004 	ldr	r5, [sl], #4
603040d4:	0a000001 	beq	603040e0 <_vfiprintf_r+0x708>
603040d8:	e1a06003 	mov	r6, r3
603040dc:	eafffff4 	b	603040b4 <_vfiprintf_r+0x6dc>
603040e0:	e2143040 	ands	r3, r4, #64	; 0x40
603040e4:	16ff5075 	uxthne	r5, r5
603040e8:	1afffff1 	bne	603040b4 <_vfiprintf_r+0x6dc>
603040ec:	e2146c02 	ands	r6, r4, #512	; 0x200
603040f0:	0affffef 	beq	603040b4 <_vfiprintf_r+0x6dc>
603040f4:	e6ef5075 	uxtb	r5, r5
603040f8:	eafffff6 	b	603040d8 <_vfiprintf_r+0x700>
603040fc:	e1a0a005 	mov	sl, r5
60304100:	e3073830 	movw	r3, #30768	; 0x7830
60304104:	e3a06000 	mov	r6, #0
60304108:	e1cd33b4 	strh	r3, [sp, #52]	; 0x34
6030410c:	e3844002 	orr	r4, r4, #2
60304110:	e59f3774 	ldr	r3, [pc, #1908]	; 6030488c <_vfiprintf_r+0xeb4>
60304114:	e49a5004 	ldr	r5, [sl], #4
60304118:	e58d3018 	str	r3, [sp, #24]
6030411c:	e3a03002 	mov	r3, #2
60304120:	eaffffe5 	b	603040bc <_vfiprintf_r+0x6e4>
60304124:	e1a0a005 	mov	sl, r5
60304128:	e59d3004 	ldr	r3, [sp, #4]
6030412c:	e3a05000 	mov	r5, #0
60304130:	e5cd5033 	strb	r5, [sp, #51]	; 0x33
60304134:	e3730001 	cmn	r3, #1
60304138:	e49ab004 	ldr	fp, [sl], #4
6030413c:	0a000009 	beq	60304168 <_vfiprintf_r+0x790>
60304140:	e1a02003 	mov	r2, r3
60304144:	e1a01005 	mov	r1, r5
60304148:	e1a0000b 	mov	r0, fp
6030414c:	eb00432b 	bl	60314e00 <__wrap_memchr>
60304150:	e2503000 	subs	r3, r0, #0
60304154:	1043300b 	subne	r3, r3, fp
60304158:	158d3004 	strne	r3, [sp, #4]
6030415c:	1a000004 	bne	60304174 <_vfiprintf_r+0x79c>
60304160:	e1a06003 	mov	r6, r3
60304164:	ea000003 	b	60304178 <_vfiprintf_r+0x7a0>
60304168:	e1a0000b 	mov	r0, fp
6030416c:	eb004329 	bl	60314e18 <__wrap_strlen>
60304170:	e58d0004 	str	r0, [sp, #4]
60304174:	e1a06005 	mov	r6, r5
60304178:	e59d3004 	ldr	r3, [sp, #4]
6030417c:	e1560003 	cmp	r6, r3
60304180:	a1a03006 	movge	r3, r6
60304184:	e58d3014 	str	r3, [sp, #20]
60304188:	e5dd3033 	ldrb	r3, [sp, #51]	; 0x33
6030418c:	e3530000 	cmp	r3, #0
60304190:	159d3014 	ldrne	r3, [sp, #20]
60304194:	12833001 	addne	r3, r3, #1
60304198:	158d3014 	strne	r3, [sp, #20]
6030419c:	e2143002 	ands	r3, r4, #2
603041a0:	e58d3028 	str	r3, [sp, #40]	; 0x28
603041a4:	159d3014 	ldrne	r3, [sp, #20]
603041a8:	12833002 	addne	r3, r3, #2
603041ac:	158d3014 	strne	r3, [sp, #20]
603041b0:	e2143084 	ands	r3, r4, #132	; 0x84
603041b4:	e58d302c 	str	r3, [sp, #44]	; 0x2c
603041b8:	1a00001a 	bne	60304228 <_vfiprintf_r+0x850>
603041bc:	e59d3010 	ldr	r3, [sp, #16]
603041c0:	e59d2014 	ldr	r2, [sp, #20]
603041c4:	e0435002 	sub	r5, r3, r2
603041c8:	e3550000 	cmp	r5, #0
603041cc:	da000015 	ble	60304228 <_vfiprintf_r+0x850>
603041d0:	e59d303c 	ldr	r3, [sp, #60]	; 0x3c
603041d4:	e2871008 	add	r1, r7, #8
603041d8:	e59f06b0 	ldr	r0, [pc, #1712]	; 60304890 <_vfiprintf_r+0xeb8>
603041dc:	e3550010 	cmp	r5, #16
603041e0:	e59d2040 	ldr	r2, [sp, #64]	; 0x40
603041e4:	e2833001 	add	r3, r3, #1
603041e8:	e5870000 	str	r0, [r7]
603041ec:	ca0000d3 	bgt	60304540 <_vfiprintf_r+0xb68>
603041f0:	e3530007 	cmp	r3, #7
603041f4:	e5875004 	str	r5, [r7, #4]
603041f8:	e58d303c 	str	r3, [sp, #60]	; 0x3c
603041fc:	e0855002 	add	r5, r5, r2
60304200:	d1a07001 	movle	r7, r1
60304204:	e58d5040 	str	r5, [sp, #64]	; 0x40
60304208:	da000006 	ble	60304228 <_vfiprintf_r+0x850>
6030420c:	e28d2038 	add	r2, sp, #56	; 0x38
60304210:	e1a01008 	mov	r1, r8
60304214:	e1a00009 	mov	r0, r9
60304218:	ebfffdc4 	bl	60303930 <__sprint_r>
6030421c:	e3500000 	cmp	r0, #0
60304220:	1a000127 	bne	603046c4 <_vfiprintf_r+0xcec>
60304224:	e28d7044 	add	r7, sp, #68	; 0x44
60304228:	e5dd3033 	ldrb	r3, [sp, #51]	; 0x33
6030422c:	e3530000 	cmp	r3, #0
60304230:	0a000013 	beq	60304284 <_vfiprintf_r+0x8ac>
60304234:	e59d303c 	ldr	r3, [sp, #60]	; 0x3c
60304238:	e28d2033 	add	r2, sp, #51	; 0x33
6030423c:	e5872000 	str	r2, [r7]
60304240:	e3a02001 	mov	r2, #1
60304244:	e2833001 	add	r3, r3, #1
60304248:	e5872004 	str	r2, [r7, #4]
6030424c:	e59d2040 	ldr	r2, [sp, #64]	; 0x40
60304250:	e3530007 	cmp	r3, #7
60304254:	d2877008 	addle	r7, r7, #8
60304258:	e58d303c 	str	r3, [sp, #60]	; 0x3c
6030425c:	e2822001 	add	r2, r2, #1
60304260:	e58d2040 	str	r2, [sp, #64]	; 0x40
60304264:	da000006 	ble	60304284 <_vfiprintf_r+0x8ac>
60304268:	e28d2038 	add	r2, sp, #56	; 0x38
6030426c:	e1a01008 	mov	r1, r8
60304270:	e1a00009 	mov	r0, r9
60304274:	ebfffdad 	bl	60303930 <__sprint_r>
60304278:	e3500000 	cmp	r0, #0
6030427c:	1a000110 	bne	603046c4 <_vfiprintf_r+0xcec>
60304280:	e28d7044 	add	r7, sp, #68	; 0x44
60304284:	e59d3028 	ldr	r3, [sp, #40]	; 0x28
60304288:	e3530000 	cmp	r3, #0
6030428c:	0a000013 	beq	603042e0 <_vfiprintf_r+0x908>
60304290:	e59d303c 	ldr	r3, [sp, #60]	; 0x3c
60304294:	e28d2034 	add	r2, sp, #52	; 0x34
60304298:	e5872000 	str	r2, [r7]
6030429c:	e3a02002 	mov	r2, #2
603042a0:	e2833001 	add	r3, r3, #1
603042a4:	e5872004 	str	r2, [r7, #4]
603042a8:	e59d2040 	ldr	r2, [sp, #64]	; 0x40
603042ac:	e3530007 	cmp	r3, #7
603042b0:	d2877008 	addle	r7, r7, #8
603042b4:	e58d303c 	str	r3, [sp, #60]	; 0x3c
603042b8:	e2822002 	add	r2, r2, #2
603042bc:	e58d2040 	str	r2, [sp, #64]	; 0x40
603042c0:	da000006 	ble	603042e0 <_vfiprintf_r+0x908>
603042c4:	e28d2038 	add	r2, sp, #56	; 0x38
603042c8:	e1a01008 	mov	r1, r8
603042cc:	e1a00009 	mov	r0, r9
603042d0:	ebfffd96 	bl	60303930 <__sprint_r>
603042d4:	e3500000 	cmp	r0, #0
603042d8:	1a0000f9 	bne	603046c4 <_vfiprintf_r+0xcec>
603042dc:	e28d7044 	add	r7, sp, #68	; 0x44
603042e0:	e59d302c 	ldr	r3, [sp, #44]	; 0x2c
603042e4:	e3530080 	cmp	r3, #128	; 0x80
603042e8:	1a00001a 	bne	60304358 <_vfiprintf_r+0x980>
603042ec:	e59d3010 	ldr	r3, [sp, #16]
603042f0:	e59d2014 	ldr	r2, [sp, #20]
603042f4:	e0435002 	sub	r5, r3, r2
603042f8:	e3550000 	cmp	r5, #0
603042fc:	da000015 	ble	60304358 <_vfiprintf_r+0x980>
60304300:	e59d303c 	ldr	r3, [sp, #60]	; 0x3c
60304304:	e2871008 	add	r1, r7, #8
60304308:	e59f0584 	ldr	r0, [pc, #1412]	; 60304894 <_vfiprintf_r+0xebc>
6030430c:	e3550010 	cmp	r5, #16
60304310:	e59d2040 	ldr	r2, [sp, #64]	; 0x40
60304314:	e2833001 	add	r3, r3, #1
60304318:	e5870000 	str	r0, [r7]
6030431c:	ca000098 	bgt	60304584 <_vfiprintf_r+0xbac>
60304320:	e3530007 	cmp	r3, #7
60304324:	e5875004 	str	r5, [r7, #4]
60304328:	e58d303c 	str	r3, [sp, #60]	; 0x3c
6030432c:	e0855002 	add	r5, r5, r2
60304330:	d1a07001 	movle	r7, r1
60304334:	e58d5040 	str	r5, [sp, #64]	; 0x40
60304338:	da000006 	ble	60304358 <_vfiprintf_r+0x980>
6030433c:	e28d2038 	add	r2, sp, #56	; 0x38
60304340:	e1a01008 	mov	r1, r8
60304344:	e1a00009 	mov	r0, r9
60304348:	ebfffd78 	bl	60303930 <__sprint_r>
6030434c:	e3500000 	cmp	r0, #0
60304350:	1a0000db 	bne	603046c4 <_vfiprintf_r+0xcec>
60304354:	e28d7044 	add	r7, sp, #68	; 0x44
60304358:	e59d3004 	ldr	r3, [sp, #4]
6030435c:	e0466003 	sub	r6, r6, r3
60304360:	e3560000 	cmp	r6, #0
60304364:	c59f5528 	ldrgt	r5, [pc, #1320]	; 60304894 <_vfiprintf_r+0xebc>
60304368:	ca0000a6 	bgt	60304608 <_vfiprintf_r+0xc30>
6030436c:	e59d3004 	ldr	r3, [sp, #4]
60304370:	e59d0040 	ldr	r0, [sp, #64]	; 0x40
60304374:	e5873004 	str	r3, [r7, #4]
60304378:	e0800003 	add	r0, r0, r3
6030437c:	e59d303c 	ldr	r3, [sp, #60]	; 0x3c
60304380:	e587b000 	str	fp, [r7]
60304384:	e2833001 	add	r3, r3, #1
60304388:	e58d0040 	str	r0, [sp, #64]	; 0x40
6030438c:	e3530007 	cmp	r3, #7
60304390:	e58d303c 	str	r3, [sp, #60]	; 0x3c
60304394:	d2877008 	addle	r7, r7, #8
60304398:	da000006 	ble	603043b8 <_vfiprintf_r+0x9e0>
6030439c:	e28d2038 	add	r2, sp, #56	; 0x38
603043a0:	e1a01008 	mov	r1, r8
603043a4:	e1a00009 	mov	r0, r9
603043a8:	ebfffd60 	bl	60303930 <__sprint_r>
603043ac:	e3500000 	cmp	r0, #0
603043b0:	1a0000c3 	bne	603046c4 <_vfiprintf_r+0xcec>
603043b4:	e28d7044 	add	r7, sp, #68	; 0x44
603043b8:	e3140004 	tst	r4, #4
603043bc:	1a0000a7 	bne	60304660 <_vfiprintf_r+0xc88>
603043c0:	e59d300c 	ldr	r3, [sp, #12]
603043c4:	e59d2010 	ldr	r2, [sp, #16]
603043c8:	e59d1014 	ldr	r1, [sp, #20]
603043cc:	e1520001 	cmp	r2, r1
603043d0:	a0833002 	addge	r3, r3, r2
603043d4:	b0833001 	addlt	r3, r3, r1
603043d8:	e58d300c 	str	r3, [sp, #12]
603043dc:	e59d3040 	ldr	r3, [sp, #64]	; 0x40
603043e0:	e3530000 	cmp	r3, #0
603043e4:	0a000005 	beq	60304400 <_vfiprintf_r+0xa28>
603043e8:	e28d2038 	add	r2, sp, #56	; 0x38
603043ec:	e1a01008 	mov	r1, r8
603043f0:	e1a00009 	mov	r0, r9
603043f4:	ebfffd4d 	bl	60303930 <__sprint_r>
603043f8:	e3500000 	cmp	r0, #0
603043fc:	1a0000b0 	bne	603046c4 <_vfiprintf_r+0xcec>
60304400:	e3a03000 	mov	r3, #0
60304404:	e28d7044 	add	r7, sp, #68	; 0x44
60304408:	e58d303c 	str	r3, [sp, #60]	; 0x3c
6030440c:	eaffff0d 	b	60304048 <_vfiprintf_r+0x670>
60304410:	e3844010 	orr	r4, r4, #16
60304414:	e2143020 	ands	r3, r4, #32
60304418:	0a000006 	beq	60304438 <_vfiprintf_r+0xa60>
6030441c:	e2855007 	add	r5, r5, #7
60304420:	e3c53007 	bic	r3, r5, #7
60304424:	e1a0a003 	mov	sl, r3
60304428:	e5936004 	ldr	r6, [r3, #4]
6030442c:	e49a5008 	ldr	r5, [sl], #8
60304430:	e3a03001 	mov	r3, #1
60304434:	eaffff20 	b	603040bc <_vfiprintf_r+0x6e4>
60304438:	e1a0a005 	mov	sl, r5
6030443c:	e2146010 	ands	r6, r4, #16
60304440:	e49a5004 	ldr	r5, [sl], #4
60304444:	0a000001 	beq	60304450 <_vfiprintf_r+0xa78>
60304448:	e1a06003 	mov	r6, r3
6030444c:	eafffff7 	b	60304430 <_vfiprintf_r+0xa58>
60304450:	e2143040 	ands	r3, r4, #64	; 0x40
60304454:	16ff5075 	uxthne	r5, r5
60304458:	1afffff4 	bne	60304430 <_vfiprintf_r+0xa58>
6030445c:	e2146c02 	ands	r6, r4, #512	; 0x200
60304460:	0afffff2 	beq	60304430 <_vfiprintf_r+0xa58>
60304464:	e6ef5075 	uxtb	r5, r5
60304468:	eafffff6 	b	60304448 <_vfiprintf_r+0xa70>
6030446c:	e59f2424 	ldr	r2, [pc, #1060]	; 60304898 <_vfiprintf_r+0xec0>
60304470:	eafffe36 	b	60303d50 <_vfiprintf_r+0x378>
60304474:	e1a0a005 	mov	sl, r5
60304478:	e2146010 	ands	r6, r4, #16
6030447c:	e49a5004 	ldr	r5, [sl], #4
60304480:	0a000001 	beq	6030448c <_vfiprintf_r+0xab4>
60304484:	e1a06002 	mov	r6, r2
60304488:	eafffe38 	b	60303d70 <_vfiprintf_r+0x398>
6030448c:	e2142040 	ands	r2, r4, #64	; 0x40
60304490:	16ff5075 	uxthne	r5, r5
60304494:	1afffe35 	bne	60303d70 <_vfiprintf_r+0x398>
60304498:	e2146c02 	ands	r6, r4, #512	; 0x200
6030449c:	0afffe33 	beq	60303d70 <_vfiprintf_r+0x398>
603044a0:	e6ef5075 	uxtb	r5, r5
603044a4:	eafffff6 	b	60304484 <_vfiprintf_r+0xaac>
603044a8:	e59d0004 	ldr	r0, [sp, #4]
603044ac:	e1852006 	orr	r2, r5, r6
603044b0:	e3c44080 	bic	r4, r4, #128	; 0x80
603044b4:	e3520000 	cmp	r2, #0
603044b8:	03500000 	cmpeq	r0, #0
603044bc:	13a02001 	movne	r2, #1
603044c0:	03a02000 	moveq	r2, #0
603044c4:	1afffead 	bne	60303f80 <_vfiprintf_r+0x5a8>
603044c8:	e3530000 	cmp	r3, #0
603044cc:	1a000011 	bne	60304518 <_vfiprintf_r+0xb40>
603044d0:	e2112001 	ands	r2, r1, #1
603044d4:	e58d2004 	str	r2, [sp, #4]
603044d8:	0a00000f 	beq	6030451c <_vfiprintf_r+0xb44>
603044dc:	e3a02030 	mov	r2, #48	; 0x30
603044e0:	e58d3004 	str	r3, [sp, #4]
603044e4:	e5cd20e7 	strb	r2, [sp, #231]	; 0xe7
603044e8:	e28db0e7 	add	fp, sp, #231	; 0xe7
603044ec:	eafffeb8 	b	60303fd4 <_vfiprintf_r+0x5fc>
603044f0:	e205300f 	and	r3, r5, #15
603044f4:	e59d2018 	ldr	r2, [sp, #24]
603044f8:	e1a05225 	lsr	r5, r5, #4
603044fc:	e1855e06 	orr	r5, r5, r6, lsl #28
60304500:	e1a06226 	lsr	r6, r6, #4
60304504:	e7d23003 	ldrb	r3, [r2, r3]
60304508:	e56b3001 	strb	r3, [fp, #-1]!
6030450c:	e1953006 	orrs	r3, r5, r6
60304510:	1afffff6 	bne	603044f0 <_vfiprintf_r+0xb18>
60304514:	eafffeae 	b	60303fd4 <_vfiprintf_r+0x5fc>
60304518:	e58d2004 	str	r2, [sp, #4]
6030451c:	e28db0e8 	add	fp, sp, #232	; 0xe8
60304520:	eafffeab 	b	60303fd4 <_vfiprintf_r+0x5fc>
60304524:	e3530000 	cmp	r3, #0
60304528:	0a000081 	beq	60304734 <_vfiprintf_r+0xd5c>
6030452c:	e3a0b000 	mov	fp, #0
60304530:	e1a0a005 	mov	sl, r5
60304534:	e5cd3084 	strb	r3, [sp, #132]	; 0x84
60304538:	e5cdb033 	strb	fp, [sp, #51]	; 0x33
6030453c:	eafffe77 	b	60303f20 <_vfiprintf_r+0x548>
60304540:	e3a00010 	mov	r0, #16
60304544:	e3530007 	cmp	r3, #7
60304548:	e0822000 	add	r2, r2, r0
6030454c:	e5870004 	str	r0, [r7, #4]
60304550:	e58d2040 	str	r2, [sp, #64]	; 0x40
60304554:	e58d303c 	str	r3, [sp, #60]	; 0x3c
60304558:	da000006 	ble	60304578 <_vfiprintf_r+0xba0>
6030455c:	e28d2038 	add	r2, sp, #56	; 0x38
60304560:	e1a01008 	mov	r1, r8
60304564:	e1a00009 	mov	r0, r9
60304568:	ebfffcf0 	bl	60303930 <__sprint_r>
6030456c:	e3500000 	cmp	r0, #0
60304570:	1a000053 	bne	603046c4 <_vfiprintf_r+0xcec>
60304574:	e28d1044 	add	r1, sp, #68	; 0x44
60304578:	e2455010 	sub	r5, r5, #16
6030457c:	e1a07001 	mov	r7, r1
60304580:	eaffff12 	b	603041d0 <_vfiprintf_r+0x7f8>
60304584:	e3a00010 	mov	r0, #16
60304588:	e3530007 	cmp	r3, #7
6030458c:	e0822000 	add	r2, r2, r0
60304590:	e5870004 	str	r0, [r7, #4]
60304594:	e58d2040 	str	r2, [sp, #64]	; 0x40
60304598:	e58d303c 	str	r3, [sp, #60]	; 0x3c
6030459c:	da000006 	ble	603045bc <_vfiprintf_r+0xbe4>
603045a0:	e28d2038 	add	r2, sp, #56	; 0x38
603045a4:	e1a01008 	mov	r1, r8
603045a8:	e1a00009 	mov	r0, r9
603045ac:	ebfffcdf 	bl	60303930 <__sprint_r>
603045b0:	e3500000 	cmp	r0, #0
603045b4:	1a000042 	bne	603046c4 <_vfiprintf_r+0xcec>
603045b8:	e28d1044 	add	r1, sp, #68	; 0x44
603045bc:	e2455010 	sub	r5, r5, #16
603045c0:	e1a07001 	mov	r7, r1
603045c4:	eaffff4d 	b	60304300 <_vfiprintf_r+0x928>
603045c8:	e3a00010 	mov	r0, #16
603045cc:	e3530007 	cmp	r3, #7
603045d0:	e0811000 	add	r1, r1, r0
603045d4:	e5870004 	str	r0, [r7, #4]
603045d8:	e58d1040 	str	r1, [sp, #64]	; 0x40
603045dc:	e58d303c 	str	r3, [sp, #60]	; 0x3c
603045e0:	da000006 	ble	60304600 <_vfiprintf_r+0xc28>
603045e4:	e28d2038 	add	r2, sp, #56	; 0x38
603045e8:	e1a01008 	mov	r1, r8
603045ec:	e1a00009 	mov	r0, r9
603045f0:	ebfffcce 	bl	60303930 <__sprint_r>
603045f4:	e3500000 	cmp	r0, #0
603045f8:	1a000031 	bne	603046c4 <_vfiprintf_r+0xcec>
603045fc:	e28d2044 	add	r2, sp, #68	; 0x44
60304600:	e2466010 	sub	r6, r6, #16
60304604:	e1a07002 	mov	r7, r2
60304608:	e59d303c 	ldr	r3, [sp, #60]	; 0x3c
6030460c:	e2872008 	add	r2, r7, #8
60304610:	e59d1040 	ldr	r1, [sp, #64]	; 0x40
60304614:	e3560010 	cmp	r6, #16
60304618:	e2833001 	add	r3, r3, #1
6030461c:	e5875000 	str	r5, [r7]
60304620:	caffffe8 	bgt	603045c8 <_vfiprintf_r+0xbf0>
60304624:	e3530007 	cmp	r3, #7
60304628:	e0861001 	add	r1, r6, r1
6030462c:	e5876004 	str	r6, [r7, #4]
60304630:	d1a07002 	movle	r7, r2
60304634:	e58d1040 	str	r1, [sp, #64]	; 0x40
60304638:	e58d303c 	str	r3, [sp, #60]	; 0x3c
6030463c:	daffff4a 	ble	6030436c <_vfiprintf_r+0x994>
60304640:	e28d2038 	add	r2, sp, #56	; 0x38
60304644:	e1a01008 	mov	r1, r8
60304648:	e1a00009 	mov	r0, r9
6030464c:	ebfffcb7 	bl	60303930 <__sprint_r>
60304650:	e3500000 	cmp	r0, #0
60304654:	1a00001a 	bne	603046c4 <_vfiprintf_r+0xcec>
60304658:	e28d7044 	add	r7, sp, #68	; 0x44
6030465c:	eaffff42 	b	6030436c <_vfiprintf_r+0x994>
60304660:	e59d3010 	ldr	r3, [sp, #16]
60304664:	e59d2014 	ldr	r2, [sp, #20]
60304668:	e0434002 	sub	r4, r3, r2
6030466c:	e3540000 	cmp	r4, #0
60304670:	c3a06010 	movgt	r6, #16
60304674:	c59f5214 	ldrgt	r5, [pc, #532]	; 60304890 <_vfiprintf_r+0xeb8>
60304678:	daffff50 	ble	603043c0 <_vfiprintf_r+0x9e8>
6030467c:	e59d303c 	ldr	r3, [sp, #60]	; 0x3c
60304680:	e3540010 	cmp	r4, #16
60304684:	e59d2040 	ldr	r2, [sp, #64]	; 0x40
60304688:	e2833001 	add	r3, r3, #1
6030468c:	e5875000 	str	r5, [r7]
60304690:	ca000017 	bgt	603046f4 <_vfiprintf_r+0xd1c>
60304694:	e5874004 	str	r4, [r7, #4]
60304698:	e3530007 	cmp	r3, #7
6030469c:	e0844002 	add	r4, r4, r2
603046a0:	e58d303c 	str	r3, [sp, #60]	; 0x3c
603046a4:	e58d4040 	str	r4, [sp, #64]	; 0x40
603046a8:	daffff44 	ble	603043c0 <_vfiprintf_r+0x9e8>
603046ac:	e28d2038 	add	r2, sp, #56	; 0x38
603046b0:	e1a01008 	mov	r1, r8
603046b4:	e1a00009 	mov	r0, r9
603046b8:	ebfffc9c 	bl	60303930 <__sprint_r>
603046bc:	e3500000 	cmp	r0, #0
603046c0:	0affff3e 	beq	603043c0 <_vfiprintf_r+0x9e8>
603046c4:	e5983064 	ldr	r3, [r8, #100]	; 0x64
603046c8:	e3130001 	tst	r3, #1
603046cc:	1a000004 	bne	603046e4 <_vfiprintf_r+0xd0c>
603046d0:	e1d830bc 	ldrh	r3, [r8, #12]
603046d4:	e3130c02 	tst	r3, #512	; 0x200
603046d8:	1a000001 	bne	603046e4 <_vfiprintf_r+0xd0c>
603046dc:	e5980058 	ldr	r0, [r8, #88]	; 0x58
603046e0:	ebfff2d2 	bl	60301230 <__retarget_lock_release_recursive>
603046e4:	e1d830bc 	ldrh	r3, [r8, #12]
603046e8:	e3130040 	tst	r3, #64	; 0x40
603046ec:	0afffce5 	beq	60303a88 <_vfiprintf_r+0xb0>
603046f0:	eafffce2 	b	60303a80 <_vfiprintf_r+0xa8>
603046f4:	e3530007 	cmp	r3, #7
603046f8:	e2822010 	add	r2, r2, #16
603046fc:	e5876004 	str	r6, [r7, #4]
60304700:	d2877008 	addle	r7, r7, #8
60304704:	e58d2040 	str	r2, [sp, #64]	; 0x40
60304708:	e58d303c 	str	r3, [sp, #60]	; 0x3c
6030470c:	da000006 	ble	6030472c <_vfiprintf_r+0xd54>
60304710:	e28d2038 	add	r2, sp, #56	; 0x38
60304714:	e1a01008 	mov	r1, r8
60304718:	e1a00009 	mov	r0, r9
6030471c:	ebfffc83 	bl	60303930 <__sprint_r>
60304720:	e3500000 	cmp	r0, #0
60304724:	1affffe6 	bne	603046c4 <_vfiprintf_r+0xcec>
60304728:	e28d7044 	add	r7, sp, #68	; 0x44
6030472c:	e2444010 	sub	r4, r4, #16
60304730:	eaffffd1 	b	6030467c <_vfiprintf_r+0xca4>
60304734:	e59d3040 	ldr	r3, [sp, #64]	; 0x40
60304738:	e3530000 	cmp	r3, #0
6030473c:	1a000002 	bne	6030474c <_vfiprintf_r+0xd74>
60304740:	e3a03000 	mov	r3, #0
60304744:	e58d303c 	str	r3, [sp, #60]	; 0x3c
60304748:	eaffffdd 	b	603046c4 <_vfiprintf_r+0xcec>
6030474c:	e28d2038 	add	r2, sp, #56	; 0x38
60304750:	e1a01008 	mov	r1, r8
60304754:	e1a00009 	mov	r0, r9
60304758:	ebfffc74 	bl	60303930 <__sprint_r>
6030475c:	e3500000 	cmp	r0, #0
60304760:	0afffff6 	beq	60304740 <_vfiprintf_r+0xd68>
60304764:	eaffffd6 	b	603046c4 <_vfiprintf_r+0xcec>
60304768:	e59d3004 	ldr	r3, [sp, #4]
6030476c:	e3730001 	cmn	r3, #1
60304770:	11a01004 	movne	r1, r4
60304774:	13a03001 	movne	r3, #1
60304778:	1affff4a 	bne	603044a8 <_vfiprintf_r+0xad0>
6030477c:	e355000a 	cmp	r5, #10
60304780:	e2d63000 	sbcs	r3, r6, #0
60304784:	32855030 	addcc	r5, r5, #48	; 0x30
60304788:	35cd50e7 	strbcc	r5, [sp, #231]	; 0xe7
6030478c:	3affff55 	bcc	603044e8 <_vfiprintf_r+0xb10>
60304790:	e3a03000 	mov	r3, #0
60304794:	e58d3014 	str	r3, [sp, #20]
60304798:	e28d30e8 	add	r3, sp, #232	; 0xe8
6030479c:	e58d3028 	str	r3, [sp, #40]	; 0x28
603047a0:	e2043b01 	and	r3, r4, #1024	; 0x400
603047a4:	e58d302c 	str	r3, [sp, #44]	; 0x2c
603047a8:	e59d3028 	ldr	r3, [sp, #40]	; 0x28
603047ac:	e3a0200a 	mov	r2, #10
603047b0:	e1a00005 	mov	r0, r5
603047b4:	e1a01006 	mov	r1, r6
603047b8:	e243b001 	sub	fp, r3, #1
603047bc:	e3a03000 	mov	r3, #0
603047c0:	ebfff043 	bl	603008d4 <__aeabi_uldivmod>
603047c4:	e59d3028 	ldr	r3, [sp, #40]	; 0x28
603047c8:	e2822030 	add	r2, r2, #48	; 0x30
603047cc:	e5432001 	strb	r2, [r3, #-1]
603047d0:	e59d3014 	ldr	r3, [sp, #20]
603047d4:	e2833001 	add	r3, r3, #1
603047d8:	e58d3014 	str	r3, [sp, #20]
603047dc:	e59d302c 	ldr	r3, [sp, #44]	; 0x2c
603047e0:	e3530000 	cmp	r3, #0
603047e4:	0a00001c 	beq	6030485c <_vfiprintf_r+0xe84>
603047e8:	e59d301c 	ldr	r3, [sp, #28]
603047ec:	e355000a 	cmp	r5, #10
603047f0:	e59d1014 	ldr	r1, [sp, #20]
603047f4:	e5d32000 	ldrb	r2, [r3]
603047f8:	e2d63000 	sbcs	r3, r6, #0
603047fc:	23a03001 	movcs	r3, #1
60304800:	33a03000 	movcc	r3, #0
60304804:	e35200ff 	cmp	r2, #255	; 0xff
60304808:	03a03000 	moveq	r3, #0
6030480c:	12033001 	andne	r3, r3, #1
60304810:	e1520001 	cmp	r2, r1
60304814:	13a03000 	movne	r3, #0
60304818:	02033001 	andeq	r3, r3, #1
6030481c:	e3530000 	cmp	r3, #0
60304820:	0a00000d 	beq	6030485c <_vfiprintf_r+0xe84>
60304824:	e59d3020 	ldr	r3, [sp, #32]
60304828:	e59d1024 	ldr	r1, [sp, #36]	; 0x24
6030482c:	e04bb003 	sub	fp, fp, r3
60304830:	e1a02003 	mov	r2, r3
60304834:	e1a0000b 	mov	r0, fp
60304838:	eb003c61 	bl	603139c4 <__wrap_strncpy>
6030483c:	e59d301c 	ldr	r3, [sp, #28]
60304840:	e5d33001 	ldrb	r3, [r3, #1]
60304844:	e3530000 	cmp	r3, #0
60304848:	159d301c 	ldrne	r3, [sp, #28]
6030484c:	12833001 	addne	r3, r3, #1
60304850:	158d301c 	strne	r3, [sp, #28]
60304854:	13a03000 	movne	r3, #0
60304858:	e58d3014 	str	r3, [sp, #20]
6030485c:	e3a03000 	mov	r3, #0
60304860:	e3a0200a 	mov	r2, #10
60304864:	e1a00005 	mov	r0, r5
60304868:	e1a01006 	mov	r1, r6
6030486c:	ebfff018 	bl	603008d4 <__aeabi_uldivmod>
60304870:	e355000a 	cmp	r5, #10
60304874:	e2d63000 	sbcs	r3, r6, #0
60304878:	3afffdd5 	bcc	60303fd4 <_vfiprintf_r+0x5fc>
6030487c:	e1a05000 	mov	r5, r0
60304880:	e1a06001 	mov	r6, r1
60304884:	e58db028 	str	fp, [sp, #40]	; 0x28
60304888:	eaffffc6 	b	603047a8 <_vfiprintf_r+0xdd0>
6030488c:	60316eb0 	.word	0x60316eb0
60304890:	60315308 	.word	0x60315308
60304894:	60315318 	.word	0x60315318
60304898:	60316e9f 	.word	0x60316e9f

6030489c <vfiprintf>:
6030489c:	e1a03002 	mov	r3, r2
603048a0:	e1a02001 	mov	r2, r1
603048a4:	e1a01000 	mov	r1, r0
603048a8:	e59f0004 	ldr	r0, [pc, #4]	; 603048b4 <vfiprintf+0x18>
603048ac:	e5900000 	ldr	r0, [r0]
603048b0:	eafffc48 	b	603039d8 <_vfiprintf_r>
603048b4:	60317000 	.word	0x60317000

603048b8 <__sbprintf>:
603048b8:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
603048bc:	e1a08003 	mov	r8, r3
603048c0:	e1d130bc 	ldrh	r3, [r1, #12]
603048c4:	e24dde46 	sub	sp, sp, #1120	; 0x460
603048c8:	e1a05002 	mov	r5, r2
603048cc:	e24dd008 	sub	sp, sp, #8
603048d0:	e3c33002 	bic	r3, r3, #2
603048d4:	e1a0700d 	mov	r7, sp
603048d8:	e1a06000 	mov	r6, r0
603048dc:	e28d0058 	add	r0, sp, #88	; 0x58
603048e0:	e1a04001 	mov	r4, r1
603048e4:	e1c730bc 	strh	r3, [r7, #12]
603048e8:	e5913064 	ldr	r3, [r1, #100]	; 0x64
603048ec:	e58d3064 	str	r3, [sp, #100]	; 0x64
603048f0:	e1d130be 	ldrh	r3, [r1, #14]
603048f4:	e1c730be 	strh	r3, [r7, #14]
603048f8:	e5913020 	ldr	r3, [r1, #32]
603048fc:	e58d3020 	str	r3, [sp, #32]
60304900:	e5913028 	ldr	r3, [r1, #40]	; 0x28
60304904:	e58d3028 	str	r3, [sp, #40]	; 0x28
60304908:	e28d3068 	add	r3, sp, #104	; 0x68
6030490c:	e58d3000 	str	r3, [sp]
60304910:	e58d3010 	str	r3, [sp, #16]
60304914:	e3a03b01 	mov	r3, #1024	; 0x400
60304918:	e58d3008 	str	r3, [sp, #8]
6030491c:	e58d3014 	str	r3, [sp, #20]
60304920:	e3a03000 	mov	r3, #0
60304924:	e58d3018 	str	r3, [sp, #24]
60304928:	ebfff236 	bl	60301208 <__retarget_lock_init_recursive>
6030492c:	e1a02005 	mov	r2, r5
60304930:	e1a03008 	mov	r3, r8
60304934:	e1a01007 	mov	r1, r7
60304938:	e1a00006 	mov	r0, r6
6030493c:	ebfffc25 	bl	603039d8 <_vfiprintf_r>
60304940:	e2505000 	subs	r5, r0, #0
60304944:	ba000004 	blt	6030495c <__sbprintf+0xa4>
60304948:	e1a01007 	mov	r1, r7
6030494c:	e1a00006 	mov	r0, r6
60304950:	ebfff0ba 	bl	60300c40 <_fflush_r>
60304954:	e3500000 	cmp	r0, #0
60304958:	13e05000 	mvnne	r5, #0
6030495c:	e1dd30bc 	ldrh	r3, [sp, #12]
60304960:	e59d0058 	ldr	r0, [sp, #88]	; 0x58
60304964:	e3130040 	tst	r3, #64	; 0x40
60304968:	11d430bc 	ldrhne	r3, [r4, #12]
6030496c:	13833040 	orrne	r3, r3, #64	; 0x40
60304970:	11c430bc 	strhne	r3, [r4, #12]
60304974:	ebfff225 	bl	60301210 <__retarget_lock_close_recursive>
60304978:	e1a00005 	mov	r0, r5
6030497c:	e28dde46 	add	sp, sp, #1120	; 0x460
60304980:	e28dd008 	add	sp, sp, #8
60304984:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

60304988 <_vfprintf_r>:
60304988:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
6030498c:	e1a07000 	mov	r7, r0
60304990:	e1a05001 	mov	r5, r1
60304994:	ed2d8b04 	vpush	{d8-d9}
60304998:	e24ddf4b 	sub	sp, sp, #300	; 0x12c
6030499c:	e1a0a002 	mov	sl, r2
603049a0:	e1a04003 	mov	r4, r3
603049a4:	e1a08003 	mov	r8, r3
603049a8:	ebfff7fe 	bl	603029a8 <_localeconv_r>
603049ac:	e5903000 	ldr	r3, [r0]
603049b0:	e58d3040 	str	r3, [sp, #64]	; 0x40
603049b4:	e1a00003 	mov	r0, r3
603049b8:	eb004116 	bl	60314e18 <__wrap_strlen>
603049bc:	e3570000 	cmp	r7, #0
603049c0:	e58d0028 	str	r0, [sp, #40]	; 0x28
603049c4:	0a000004 	beq	603049dc <_vfprintf_r+0x54>
603049c8:	e5973018 	ldr	r3, [r7, #24]
603049cc:	e3530000 	cmp	r3, #0
603049d0:	1a000001 	bne	603049dc <_vfprintf_r+0x54>
603049d4:	e1a00007 	mov	r0, r7
603049d8:	ebfff140 	bl	60300ee0 <__sinit>
603049dc:	e5953064 	ldr	r3, [r5, #100]	; 0x64
603049e0:	e3130001 	tst	r3, #1
603049e4:	1a000004 	bne	603049fc <_vfprintf_r+0x74>
603049e8:	e1d530bc 	ldrh	r3, [r5, #12]
603049ec:	e3130c02 	tst	r3, #512	; 0x200
603049f0:	1a000001 	bne	603049fc <_vfprintf_r+0x74>
603049f4:	e5950058 	ldr	r0, [r5, #88]	; 0x58
603049f8:	ebfff206 	bl	60301218 <__retarget_lock_acquire_recursive>
603049fc:	e1d530fc 	ldrsh	r3, [r5, #12]
60304a00:	e3130a02 	tst	r3, #8192	; 0x2000
60304a04:	1a000004 	bne	60304a1c <_vfprintf_r+0x94>
60304a08:	e3833a02 	orr	r3, r3, #8192	; 0x2000
60304a0c:	e1c530bc 	strh	r3, [r5, #12]
60304a10:	e5953064 	ldr	r3, [r5, #100]	; 0x64
60304a14:	e3c33a02 	bic	r3, r3, #8192	; 0x2000
60304a18:	e5853064 	str	r3, [r5, #100]	; 0x64
60304a1c:	e1d530bc 	ldrh	r3, [r5, #12]
60304a20:	e3130008 	tst	r3, #8
60304a24:	0a000002 	beq	60304a34 <_vfprintf_r+0xac>
60304a28:	e5953010 	ldr	r3, [r5, #16]
60304a2c:	e3530000 	cmp	r3, #0
60304a30:	1a000013 	bne	60304a84 <_vfprintf_r+0xfc>
60304a34:	e1a01005 	mov	r1, r5
60304a38:	e1a00007 	mov	r0, r7
60304a3c:	eb0007e1 	bl	603069c8 <__swsetup_r>
60304a40:	e3500000 	cmp	r0, #0
60304a44:	0a00000e 	beq	60304a84 <_vfprintf_r+0xfc>
60304a48:	e5953064 	ldr	r3, [r5, #100]	; 0x64
60304a4c:	e3130001 	tst	r3, #1
60304a50:	0a000005 	beq	60304a6c <_vfprintf_r+0xe4>
60304a54:	e3e03000 	mvn	r3, #0
60304a58:	e58d302c 	str	r3, [sp, #44]	; 0x2c
60304a5c:	e59d002c 	ldr	r0, [sp, #44]	; 0x2c
60304a60:	e28ddf4b 	add	sp, sp, #300	; 0x12c
60304a64:	ecbd8b04 	vpop	{d8-d9}
60304a68:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
60304a6c:	e1d530bc 	ldrh	r3, [r5, #12]
60304a70:	e3130c02 	tst	r3, #512	; 0x200
60304a74:	1afffff6 	bne	60304a54 <_vfprintf_r+0xcc>
60304a78:	e5950058 	ldr	r0, [r5, #88]	; 0x58
60304a7c:	ebfff1eb 	bl	60301230 <__retarget_lock_release_recursive>
60304a80:	eafffff3 	b	60304a54 <_vfprintf_r+0xcc>
60304a84:	e1d530bc 	ldrh	r3, [r5, #12]
60304a88:	e203201a 	and	r2, r3, #26
60304a8c:	e352000a 	cmp	r2, #10
60304a90:	1a000011 	bne	60304adc <_vfprintf_r+0x154>
60304a94:	e1d520fe 	ldrsh	r2, [r5, #14]
60304a98:	e3520000 	cmp	r2, #0
60304a9c:	ba00000e 	blt	60304adc <_vfprintf_r+0x154>
60304aa0:	e5952064 	ldr	r2, [r5, #100]	; 0x64
60304aa4:	e3120001 	tst	r2, #1
60304aa8:	1a000003 	bne	60304abc <_vfprintf_r+0x134>
60304aac:	e3130c02 	tst	r3, #512	; 0x200
60304ab0:	1a000001 	bne	60304abc <_vfprintf_r+0x134>
60304ab4:	e5950058 	ldr	r0, [r5, #88]	; 0x58
60304ab8:	ebfff1dc 	bl	60301230 <__retarget_lock_release_recursive>
60304abc:	e1a03004 	mov	r3, r4
60304ac0:	e1a0200a 	mov	r2, sl
60304ac4:	e1a01005 	mov	r1, r5
60304ac8:	e1a00007 	mov	r0, r7
60304acc:	e28ddf4b 	add	sp, sp, #300	; 0x12c
60304ad0:	ecbd8b04 	vpop	{d8-d9}
60304ad4:	e8bd4ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60304ad8:	ea000775 	b	603068b4 <__sbprintf>
60304adc:	f2808e30 	vmov.i64	d8, #0x0000000000000000
60304ae0:	e3a03000 	mov	r3, #0
60304ae4:	e28d4084 	add	r4, sp, #132	; 0x84
60304ae8:	e58d3080 	str	r3, [sp, #128]	; 0x80
60304aec:	e58d4078 	str	r4, [sp, #120]	; 0x78
60304af0:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60304af4:	e58d300c 	str	r3, [sp, #12]
60304af8:	e58d304c 	str	r3, [sp, #76]	; 0x4c
60304afc:	e58d3048 	str	r3, [sp, #72]	; 0x48
60304b00:	e58d3020 	str	r3, [sp, #32]
60304b04:	e58d3038 	str	r3, [sp, #56]	; 0x38
60304b08:	e58d3044 	str	r3, [sp, #68]	; 0x44
60304b0c:	e58d302c 	str	r3, [sp, #44]	; 0x2c
60304b10:	e1a0200a 	mov	r2, sl
60304b14:	e5d23000 	ldrb	r3, [r2]
60304b18:	e1a06002 	mov	r6, r2
60304b1c:	e2822001 	add	r2, r2, #1
60304b20:	e3530000 	cmp	r3, #0
60304b24:	13530025 	cmpne	r3, #37	; 0x25
60304b28:	1afffff9 	bne	60304b14 <_vfprintf_r+0x18c>
60304b2c:	e056900a 	subs	r9, r6, sl
60304b30:	0a000014 	beq	60304b88 <_vfprintf_r+0x200>
60304b34:	e59d3080 	ldr	r3, [sp, #128]	; 0x80
60304b38:	e584a000 	str	sl, [r4]
60304b3c:	e0833009 	add	r3, r3, r9
60304b40:	e58d3080 	str	r3, [sp, #128]	; 0x80
60304b44:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
60304b48:	e5849004 	str	r9, [r4, #4]
60304b4c:	e2833001 	add	r3, r3, #1
60304b50:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60304b54:	e3530007 	cmp	r3, #7
60304b58:	d2844008 	addle	r4, r4, #8
60304b5c:	da000006 	ble	60304b7c <_vfprintf_r+0x1f4>
60304b60:	e28d2078 	add	r2, sp, #120	; 0x78
60304b64:	e1a01005 	mov	r1, r5
60304b68:	e1a00007 	mov	r0, r7
60304b6c:	ebfffb6f 	bl	60303930 <__sprint_r>
60304b70:	e3500000 	cmp	r0, #0
60304b74:	1a00015f 	bne	603050f8 <_vfprintf_r+0x770>
60304b78:	e28d4084 	add	r4, sp, #132	; 0x84
60304b7c:	e59d302c 	ldr	r3, [sp, #44]	; 0x2c
60304b80:	e0833009 	add	r3, r3, r9
60304b84:	e58d302c 	str	r3, [sp, #44]	; 0x2c
60304b88:	e5d63000 	ldrb	r3, [r6]
60304b8c:	e3530000 	cmp	r3, #0
60304b90:	0a0006e9 	beq	6030673c <_vfprintf_r+0x1db4>
60304b94:	e3a02000 	mov	r2, #0
60304b98:	e2863001 	add	r3, r6, #1
60304b9c:	e1a0b002 	mov	fp, r2
60304ba0:	e3e06000 	mvn	r6, #0
60304ba4:	e3a0900a 	mov	r9, #10
60304ba8:	e1a01006 	mov	r1, r6
60304bac:	e5cd205b 	strb	r2, [sp, #91]	; 0x5b
60304bb0:	e58d2030 	str	r2, [sp, #48]	; 0x30
60304bb4:	e4d32001 	ldrb	r2, [r3], #1
60304bb8:	e58d2010 	str	r2, [sp, #16]
60304bbc:	e58d3024 	str	r3, [sp, #36]	; 0x24
60304bc0:	e59d3010 	ldr	r3, [sp, #16]
60304bc4:	e2433020 	sub	r3, r3, #32
60304bc8:	e353005a 	cmp	r3, #90	; 0x5a
60304bcc:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
60304bd0:	ea0003ff 	b	60305bd4 <_vfprintf_r+0x124c>
60304bd4:	60304d98 	.word	0x60304d98
60304bd8:	60305bd4 	.word	0x60305bd4
60304bdc:	60305bd4 	.word	0x60305bd4
60304be0:	60304db0 	.word	0x60304db0
60304be4:	60305bd4 	.word	0x60305bd4
60304be8:	60305bd4 	.word	0x60305bd4
60304bec:	60305bd4 	.word	0x60305bd4
60304bf0:	60304d40 	.word	0x60304d40
60304bf4:	60305bd4 	.word	0x60305bd4
60304bf8:	60305bd4 	.word	0x60305bd4
60304bfc:	60304db8 	.word	0x60304db8
60304c00:	60304dd8 	.word	0x60304dd8
60304c04:	60305bd4 	.word	0x60305bd4
60304c08:	60304dd0 	.word	0x60304dd0
60304c0c:	60304de0 	.word	0x60304de0
60304c10:	60305bd4 	.word	0x60305bd4
60304c14:	60304e30 	.word	0x60304e30
60304c18:	60304e38 	.word	0x60304e38
60304c1c:	60304e38 	.word	0x60304e38
60304c20:	60304e38 	.word	0x60304e38
60304c24:	60304e38 	.word	0x60304e38
60304c28:	60304e38 	.word	0x60304e38
60304c2c:	60304e38 	.word	0x60304e38
60304c30:	60304e38 	.word	0x60304e38
60304c34:	60304e38 	.word	0x60304e38
60304c38:	60304e38 	.word	0x60304e38
60304c3c:	60305bd4 	.word	0x60305bd4
60304c40:	60305bd4 	.word	0x60305bd4
60304c44:	60305bd4 	.word	0x60305bd4
60304c48:	60305bd4 	.word	0x60305bd4
60304c4c:	60305bd4 	.word	0x60305bd4
60304c50:	60305bd4 	.word	0x60305bd4
60304c54:	60305bd4 	.word	0x60305bd4
60304c58:	60304ffc 	.word	0x60304ffc
60304c5c:	60305bd4 	.word	0x60305bd4
60304c60:	60304ec4 	.word	0x60304ec4
60304c64:	60304f00 	.word	0x60304f00
60304c68:	60304ffc 	.word	0x60304ffc
60304c6c:	60304ffc 	.word	0x60304ffc
60304c70:	60304ffc 	.word	0x60304ffc
60304c74:	60305bd4 	.word	0x60305bd4
60304c78:	60305bd4 	.word	0x60305bd4
60304c7c:	60305bd4 	.word	0x60305bd4
60304c80:	60305bd4 	.word	0x60305bd4
60304c84:	60304e70 	.word	0x60304e70
60304c88:	60305bd4 	.word	0x60305bd4
60304c8c:	60305bd4 	.word	0x60305bd4
60304c90:	60305948 	.word	0x60305948
60304c94:	60305bd4 	.word	0x60305bd4
60304c98:	60305bd4 	.word	0x60305bd4
60304c9c:	60305bd4 	.word	0x60305bd4
60304ca0:	603059ec 	.word	0x603059ec
60304ca4:	60305bd4 	.word	0x60305bd4
60304ca8:	60305a5c 	.word	0x60305a5c
60304cac:	60305bd4 	.word	0x60305bd4
60304cb0:	60305bd4 	.word	0x60305bd4
60304cb4:	60305ac0 	.word	0x60305ac0
60304cb8:	60305bd4 	.word	0x60305bd4
60304cbc:	60305bd4 	.word	0x60305bd4
60304cc0:	60305bd4 	.word	0x60305bd4
60304cc4:	60305bd4 	.word	0x60305bd4
60304cc8:	60305bd4 	.word	0x60305bd4
60304ccc:	60305bd4 	.word	0x60305bd4
60304cd0:	60305bd4 	.word	0x60305bd4
60304cd4:	60305bd4 	.word	0x60305bd4
60304cd8:	60304ffc 	.word	0x60304ffc
60304cdc:	60305bd4 	.word	0x60305bd4
60304ce0:	60304ec4 	.word	0x60304ec4
60304ce4:	60304f04 	.word	0x60304f04
60304ce8:	60304ffc 	.word	0x60304ffc
60304cec:	60304ffc 	.word	0x60304ffc
60304cf0:	60304ffc 	.word	0x60304ffc
60304cf4:	60304e78 	.word	0x60304e78
60304cf8:	60304f04 	.word	0x60304f04
60304cfc:	60304ebc 	.word	0x60304ebc
60304d00:	60305bd4 	.word	0x60305bd4
60304d04:	60304e9c 	.word	0x60304e9c
60304d08:	60305bd4 	.word	0x60305bd4
60304d0c:	603058d4 	.word	0x603058d4
60304d10:	6030594c 	.word	0x6030594c
60304d14:	603059b8 	.word	0x603059b8
60304d18:	60304ebc 	.word	0x60304ebc
60304d1c:	60305bd4 	.word	0x60305bd4
60304d20:	603059ec 	.word	0x603059ec
60304d24:	60304d90 	.word	0x60304d90
60304d28:	60305a60 	.word	0x60305a60
60304d2c:	60305bd4 	.word	0x60305bd4
60304d30:	60305bd4 	.word	0x60305bd4
60304d34:	60305b1c 	.word	0x60305b1c
60304d38:	60305bd4 	.word	0x60305bd4
60304d3c:	60304d90 	.word	0x60304d90
60304d40:	e1a00007 	mov	r0, r7
60304d44:	ebfff717 	bl	603029a8 <_localeconv_r>
60304d48:	e5903004 	ldr	r3, [r0, #4]
60304d4c:	e58d3044 	str	r3, [sp, #68]	; 0x44
60304d50:	e1a00003 	mov	r0, r3
60304d54:	eb00402f 	bl	60314e18 <__wrap_strlen>
60304d58:	e58d0038 	str	r0, [sp, #56]	; 0x38
60304d5c:	e1a00007 	mov	r0, r7
60304d60:	ebfff710 	bl	603029a8 <_localeconv_r>
60304d64:	e5903008 	ldr	r3, [r0, #8]
60304d68:	e58d3020 	str	r3, [sp, #32]
60304d6c:	e3e01000 	mvn	r1, #0
60304d70:	e59d3038 	ldr	r3, [sp, #56]	; 0x38
60304d74:	e59d2020 	ldr	r2, [sp, #32]
60304d78:	e3530000 	cmp	r3, #0
60304d7c:	13520000 	cmpne	r2, #0
60304d80:	0a000002 	beq	60304d90 <_vfprintf_r+0x408>
60304d84:	e5d23000 	ldrb	r3, [r2]
60304d88:	e3530000 	cmp	r3, #0
60304d8c:	138bbb01 	orrne	fp, fp, #1024	; 0x400
60304d90:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
60304d94:	eaffff86 	b	60304bb4 <_vfprintf_r+0x22c>
60304d98:	e5dd305b 	ldrb	r3, [sp, #91]	; 0x5b
60304d9c:	e3530000 	cmp	r3, #0
60304da0:	03a03020 	moveq	r3, #32
60304da4:	1afffff9 	bne	60304d90 <_vfprintf_r+0x408>
60304da8:	e5cd305b 	strb	r3, [sp, #91]	; 0x5b
60304dac:	eafffff7 	b	60304d90 <_vfprintf_r+0x408>
60304db0:	e38bb001 	orr	fp, fp, #1
60304db4:	eafffff5 	b	60304d90 <_vfprintf_r+0x408>
60304db8:	e4983004 	ldr	r3, [r8], #4
60304dbc:	e58d3030 	str	r3, [sp, #48]	; 0x30
60304dc0:	e3530000 	cmp	r3, #0
60304dc4:	aafffff1 	bge	60304d90 <_vfprintf_r+0x408>
60304dc8:	e2633000 	rsb	r3, r3, #0
60304dcc:	e58d3030 	str	r3, [sp, #48]	; 0x30
60304dd0:	e38bb004 	orr	fp, fp, #4
60304dd4:	eaffffed 	b	60304d90 <_vfprintf_r+0x408>
60304dd8:	e3a0302b 	mov	r3, #43	; 0x2b
60304ddc:	eafffff1 	b	60304da8 <_vfprintf_r+0x420>
60304de0:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
60304de4:	e4d32001 	ldrb	r2, [r3], #1
60304de8:	e58d2010 	str	r2, [sp, #16]
60304dec:	e352002a 	cmp	r2, #42	; 0x2a
60304df0:	1a00000c 	bne	60304e28 <_vfprintf_r+0x4a0>
60304df4:	e4986004 	ldr	r6, [r8], #4
60304df8:	e58d3024 	str	r3, [sp, #36]	; 0x24
60304dfc:	e1560001 	cmp	r6, r1
60304e00:	b1a06001 	movlt	r6, r1
60304e04:	eaffffe1 	b	60304d90 <_vfprintf_r+0x408>
60304e08:	e0262699 	mla	r6, r9, r6, r2
60304e0c:	e4d32001 	ldrb	r2, [r3], #1
60304e10:	e58d2010 	str	r2, [sp, #16]
60304e14:	e59d2010 	ldr	r2, [sp, #16]
60304e18:	e2422030 	sub	r2, r2, #48	; 0x30
60304e1c:	e3520009 	cmp	r2, #9
60304e20:	9afffff8 	bls	60304e08 <_vfprintf_r+0x480>
60304e24:	eaffff64 	b	60304bbc <_vfprintf_r+0x234>
60304e28:	e3a06000 	mov	r6, #0
60304e2c:	eafffff8 	b	60304e14 <_vfprintf_r+0x48c>
60304e30:	e38bb080 	orr	fp, fp, #128	; 0x80
60304e34:	eaffffd5 	b	60304d90 <_vfprintf_r+0x408>
60304e38:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
60304e3c:	e3a02000 	mov	r2, #0
60304e40:	e58d2030 	str	r2, [sp, #48]	; 0x30
60304e44:	e59d2010 	ldr	r2, [sp, #16]
60304e48:	e59d0030 	ldr	r0, [sp, #48]	; 0x30
60304e4c:	e2422030 	sub	r2, r2, #48	; 0x30
60304e50:	e0222099 	mla	r2, r9, r0, r2
60304e54:	e58d2030 	str	r2, [sp, #48]	; 0x30
60304e58:	e4d32001 	ldrb	r2, [r3], #1
60304e5c:	e58d2010 	str	r2, [sp, #16]
60304e60:	e2422030 	sub	r2, r2, #48	; 0x30
60304e64:	e3520009 	cmp	r2, #9
60304e68:	9afffff5 	bls	60304e44 <_vfprintf_r+0x4bc>
60304e6c:	eaffff52 	b	60304bbc <_vfprintf_r+0x234>
60304e70:	e38bb008 	orr	fp, fp, #8
60304e74:	eaffffc5 	b	60304d90 <_vfprintf_r+0x408>
60304e78:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
60304e7c:	e5d33000 	ldrb	r3, [r3]
60304e80:	e3530068 	cmp	r3, #104	; 0x68
60304e84:	038bbc02 	orreq	fp, fp, #512	; 0x200
60304e88:	138bb040 	orrne	fp, fp, #64	; 0x40
60304e8c:	059d3024 	ldreq	r3, [sp, #36]	; 0x24
60304e90:	02833001 	addeq	r3, r3, #1
60304e94:	058d3024 	streq	r3, [sp, #36]	; 0x24
60304e98:	eaffffbc 	b	60304d90 <_vfprintf_r+0x408>
60304e9c:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
60304ea0:	e5d33000 	ldrb	r3, [r3]
60304ea4:	e353006c 	cmp	r3, #108	; 0x6c
60304ea8:	138bb010 	orrne	fp, fp, #16
60304eac:	059d3024 	ldreq	r3, [sp, #36]	; 0x24
60304eb0:	02833001 	addeq	r3, r3, #1
60304eb4:	058d3024 	streq	r3, [sp, #36]	; 0x24
60304eb8:	1affffb4 	bne	60304d90 <_vfprintf_r+0x408>
60304ebc:	e38bb020 	orr	fp, fp, #32
60304ec0:	eaffffb2 	b	60304d90 <_vfprintf_r+0x408>
60304ec4:	e1a02008 	mov	r2, r8
60304ec8:	e3a00000 	mov	r0, #0
60304ecc:	e5cd005b 	strb	r0, [sp, #91]	; 0x5b
60304ed0:	e4923004 	ldr	r3, [r2], #4
60304ed4:	e58d2014 	str	r2, [sp, #20]
60304ed8:	e5cd30c4 	strb	r3, [sp, #196]	; 0xc4
60304edc:	e3a06001 	mov	r6, #1
60304ee0:	e1a09000 	mov	r9, r0
60304ee4:	e1a03000 	mov	r3, r0
60304ee8:	e1a08000 	mov	r8, r0
60304eec:	e28da0c4 	add	sl, sp, #196	; 0xc4
60304ef0:	e58d0008 	str	r0, [sp, #8]
60304ef4:	e58d0018 	str	r0, [sp, #24]
60304ef8:	e58d001c 	str	r0, [sp, #28]
60304efc:	ea000180 	b	60305504 <_vfprintf_r+0xb7c>
60304f00:	e38bb010 	orr	fp, fp, #16
60304f04:	e31b0020 	tst	fp, #32
60304f08:	0a00002a 	beq	60304fb8 <_vfprintf_r+0x630>
60304f0c:	e2888007 	add	r8, r8, #7
60304f10:	e3c83007 	bic	r3, r8, #7
60304f14:	e1a02003 	mov	r2, r3
60304f18:	e5939004 	ldr	r9, [r3, #4]
60304f1c:	e4928008 	ldr	r8, [r2], #8
60304f20:	e58d2014 	str	r2, [sp, #20]
60304f24:	e3590000 	cmp	r9, #0
60304f28:	aa000610 	bge	60306770 <_vfprintf_r+0x1de8>
60304f2c:	e2788000 	rsbs	r8, r8, #0
60304f30:	e3a0302d 	mov	r3, #45	; 0x2d
60304f34:	e2e99000 	rsc	r9, r9, #0
60304f38:	e5cd305b 	strb	r3, [sp, #91]	; 0x5b
60304f3c:	e3a03001 	mov	r3, #1
60304f40:	e3760001 	cmn	r6, #1
60304f44:	11a0100b 	movne	r1, fp
60304f48:	1a000303 	bne	60305b5c <_vfprintf_r+0x11d4>
60304f4c:	e3530001 	cmp	r3, #1
60304f50:	0a00060a 	beq	60306780 <_vfprintf_r+0x1df8>
60304f54:	e28daf4a 	add	sl, sp, #296	; 0x128
60304f58:	e3530002 	cmp	r3, #2
60304f5c:	0a00030f 	beq	60305ba0 <_vfprintf_r+0x1218>
60304f60:	e2083007 	and	r3, r8, #7
60304f64:	e1a081a8 	lsr	r8, r8, #3
60304f68:	e1888e89 	orr	r8, r8, r9, lsl #29
60304f6c:	e1a091a9 	lsr	r9, r9, #3
60304f70:	e1a0200a 	mov	r2, sl
60304f74:	e2833030 	add	r3, r3, #48	; 0x30
60304f78:	e1981009 	orrs	r1, r8, r9
60304f7c:	e56a3001 	strb	r3, [sl, #-1]!
60304f80:	1afffff6 	bne	60304f60 <_vfprintf_r+0x5d8>
60304f84:	e3530030 	cmp	r3, #48	; 0x30
60304f88:	03a01000 	moveq	r1, #0
60304f8c:	120b1001 	andne	r1, fp, #1
60304f90:	e3510000 	cmp	r1, #0
60304f94:	13a03030 	movne	r3, #48	; 0x30
60304f98:	154a3001 	strbne	r3, [sl, #-1]
60304f9c:	1242a002 	subne	sl, r2, #2
60304fa0:	e28d3f4a 	add	r3, sp, #296	; 0x128
60304fa4:	e1a09006 	mov	r9, r6
60304fa8:	e043600a 	sub	r6, r3, sl
60304fac:	e3a03000 	mov	r3, #0
60304fb0:	e58d3008 	str	r3, [sp, #8]
60304fb4:	ea000028 	b	6030505c <_vfprintf_r+0x6d4>
60304fb8:	e1a03008 	mov	r3, r8
60304fbc:	e31b0010 	tst	fp, #16
60304fc0:	e4939004 	ldr	r9, [r3], #4
60304fc4:	e58d3014 	str	r3, [sp, #20]
60304fc8:	0a000002 	beq	60304fd8 <_vfprintf_r+0x650>
60304fcc:	e1a08009 	mov	r8, r9
60304fd0:	e1a09fc9 	asr	r9, r9, #31
60304fd4:	eaffffd2 	b	60304f24 <_vfprintf_r+0x59c>
60304fd8:	e31b0040 	tst	fp, #64	; 0x40
60304fdc:	16bf8079 	sxthne	r8, r9
60304fe0:	17a097d9 	sbfxne	r9, r9, #15, #1
60304fe4:	1affffce 	bne	60304f24 <_vfprintf_r+0x59c>
60304fe8:	e31b0c02 	tst	fp, #512	; 0x200
60304fec:	16af8079 	sxtbne	r8, r9
60304ff0:	17a093d9 	sbfxne	r9, r9, #7, #1
60304ff4:	1affffca 	bne	60304f24 <_vfprintf_r+0x59c>
60304ff8:	eafffff3 	b	60304fcc <_vfprintf_r+0x644>
60304ffc:	e2888007 	add	r8, r8, #7
60305000:	eddf0bec 	vldr	d16, [pc, #944]	; 603053b8 <_vfprintf_r+0xa30>
60305004:	e3c83007 	bic	r3, r8, #7
60305008:	ecb38b02 	vldmia	r3!, {d8}
6030500c:	eef01bc8 	vabs.f64	d17, d8
60305010:	e58d3014 	str	r3, [sp, #20]
60305014:	eef41b60 	vcmp.f64	d17, d16
60305018:	eef1fa10 	vmrs	APSR_nzcv, fpscr
6030501c:	da000012 	ble	6030506c <_vfprintf_r+0x6e4>
60305020:	eeb58bc0 	vcmpe.f64	d8, #0.0
60305024:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60305028:	43a0302d 	movmi	r3, #45	; 0x2d
6030502c:	45cd305b 	strbmi	r3, [sp, #91]	; 0x5b
60305030:	e59f2388 	ldr	r2, [pc, #904]	; 603053c0 <_vfprintf_r+0xa38>
60305034:	e59f0388 	ldr	r0, [pc, #904]	; 603053c4 <_vfprintf_r+0xa3c>
60305038:	e59d3010 	ldr	r3, [sp, #16]
6030503c:	e3cbb080 	bic	fp, fp, #128	; 0x80
60305040:	e3a06003 	mov	r6, #3
60305044:	e3530047 	cmp	r3, #71	; 0x47
60305048:	e3a03000 	mov	r3, #0
6030504c:	d1a0a002 	movle	sl, r2
60305050:	c1a0a000 	movgt	sl, r0
60305054:	e1a09003 	mov	r9, r3
60305058:	e58d3008 	str	r3, [sp, #8]
6030505c:	e1a08003 	mov	r8, r3
60305060:	e58d3018 	str	r3, [sp, #24]
60305064:	e58d301c 	str	r3, [sp, #28]
60305068:	ea000125 	b	60305504 <_vfprintf_r+0xb7c>
6030506c:	eeb48b48 	vcmp.f64	d8, d8
60305070:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60305074:	7a000006 	bvc	60305094 <_vfprintf_r+0x70c>
60305078:	ee183a90 	vmov	r3, s17
6030507c:	e59f2344 	ldr	r2, [pc, #836]	; 603053c8 <_vfprintf_r+0xa40>
60305080:	e59f0344 	ldr	r0, [pc, #836]	; 603053cc <_vfprintf_r+0xa44>
60305084:	e3530000 	cmp	r3, #0
60305088:	b3a0302d 	movlt	r3, #45	; 0x2d
6030508c:	b5cd305b 	strblt	r3, [sp, #91]	; 0x5b
60305090:	eaffffe8 	b	60305038 <_vfprintf_r+0x6b0>
60305094:	e59d3010 	ldr	r3, [sp, #16]
60305098:	e3c39020 	bic	r9, r3, #32
6030509c:	e3590041 	cmp	r9, #65	; 0x41
603050a0:	1a000020 	bne	60305128 <_vfprintf_r+0x7a0>
603050a4:	e3a03030 	mov	r3, #48	; 0x30
603050a8:	e5cd305c 	strb	r3, [sp, #92]	; 0x5c
603050ac:	e59d3010 	ldr	r3, [sp, #16]
603050b0:	e38bb002 	orr	fp, fp, #2
603050b4:	e3530061 	cmp	r3, #97	; 0x61
603050b8:	03a03078 	moveq	r3, #120	; 0x78
603050bc:	13a03058 	movne	r3, #88	; 0x58
603050c0:	e3560063 	cmp	r6, #99	; 0x63
603050c4:	e5cd305d 	strb	r3, [sp, #93]	; 0x5d
603050c8:	d28da0c4 	addle	sl, sp, #196	; 0xc4
603050cc:	d3a03000 	movle	r3, #0
603050d0:	d58d3008 	strle	r3, [sp, #8]
603050d4:	da00001b 	ble	60305148 <_vfprintf_r+0x7c0>
603050d8:	e2861001 	add	r1, r6, #1
603050dc:	e1a00007 	mov	r0, r7
603050e0:	eb002141 	bl	6030d5ec <__wrap__malloc_r>
603050e4:	e250a000 	subs	sl, r0, #0
603050e8:	1a00006e 	bne	603052a8 <_vfprintf_r+0x920>
603050ec:	e1d530bc 	ldrh	r3, [r5, #12]
603050f0:	e3833040 	orr	r3, r3, #64	; 0x40
603050f4:	e1c530bc 	strh	r3, [r5, #12]
603050f8:	e5953064 	ldr	r3, [r5, #100]	; 0x64
603050fc:	e3130001 	tst	r3, #1
60305100:	1a000004 	bne	60305118 <_vfprintf_r+0x790>
60305104:	e1d530bc 	ldrh	r3, [r5, #12]
60305108:	e3130c02 	tst	r3, #512	; 0x200
6030510c:	1a000001 	bne	60305118 <_vfprintf_r+0x790>
60305110:	e5950058 	ldr	r0, [r5, #88]	; 0x58
60305114:	ebfff045 	bl	60301230 <__retarget_lock_release_recursive>
60305118:	e1d530bc 	ldrh	r3, [r5, #12]
6030511c:	e3130040 	tst	r3, #64	; 0x40
60305120:	0afffe4d 	beq	60304a5c <_vfprintf_r+0xd4>
60305124:	eafffe4a 	b	60304a54 <_vfprintf_r+0xcc>
60305128:	e3760001 	cmn	r6, #1
6030512c:	e3a03000 	mov	r3, #0
60305130:	03a06006 	moveq	r6, #6
60305134:	e58d3008 	str	r3, [sp, #8]
60305138:	0a000002 	beq	60305148 <_vfprintf_r+0x7c0>
6030513c:	e1560003 	cmp	r6, r3
60305140:	03590047 	cmpeq	r9, #71	; 0x47
60305144:	03a06001 	moveq	r6, #1
60305148:	e38b3c01 	orr	r3, fp, #256	; 0x100
6030514c:	e58d3034 	str	r3, [sp, #52]	; 0x34
60305150:	ee183a90 	vmov	r3, s17
60305154:	e3530000 	cmp	r3, #0
60305158:	b3a0302d 	movlt	r3, #45	; 0x2d
6030515c:	a3a03000 	movge	r3, #0
60305160:	e58d303c 	str	r3, [sp, #60]	; 0x3c
60305164:	beb19b48 	vneglt.f64	d9, d8
60305168:	aeb09b48 	vmovge.f64	d9, d8
6030516c:	e3590041 	cmp	r9, #65	; 0x41
60305170:	1a00005d 	bne	603052ec <_vfprintf_r+0x964>
60305174:	e28d0060 	add	r0, sp, #96	; 0x60
60305178:	eeb00b49 	vmov.f64	d0, d9
6030517c:	ebfff9c5 	bl	60303898 <frexp>
60305180:	eef40b00 	vmov.f64	d16, #64	; 0x3e000000  0.125
60305184:	e59d2010 	ldr	r2, [sp, #16]
60305188:	e2460001 	sub	r0, r6, #1
6030518c:	ee200b20 	vmul.f64	d0, d0, d16
60305190:	e59f1244 	ldr	r1, [pc, #580]	; 603053dc <_vfprintf_r+0xa54>
60305194:	eef30b00 	vmov.f64	d16, #48	; 0x41800000  16.0
60305198:	eeb50b40 	vcmp.f64	d0, #0.0
6030519c:	eef1fa10 	vmrs	APSR_nzcv, fpscr
603051a0:	03a03001 	moveq	r3, #1
603051a4:	058d3060 	streq	r3, [sp, #96]	; 0x60
603051a8:	e59f3228 	ldr	r3, [pc, #552]	; 603053d8 <_vfprintf_r+0xa50>
603051ac:	e3520061 	cmp	r2, #97	; 0x61
603051b0:	11a01003 	movne	r1, r3
603051b4:	e1a0300a 	mov	r3, sl
603051b8:	ee200b20 	vmul.f64	d0, d0, d16
603051bc:	e3700001 	cmn	r0, #1
603051c0:	eefd7bc0 	vcvt.s32.f64	s15, d0
603051c4:	ee17ca90 	vmov	ip, s15
603051c8:	eef81be7 	vcvt.f64.s32	d17, s15
603051cc:	e7d1200c 	ldrb	r2, [r1, ip]
603051d0:	ee300b61 	vsub.f64	d0, d0, d17
603051d4:	e4c32001 	strb	r2, [r3], #1
603051d8:	e1a02000 	mov	r2, r0
603051dc:	0a000003 	beq	603051f0 <_vfprintf_r+0x868>
603051e0:	eeb50b40 	vcmp.f64	d0, #0.0
603051e4:	e2400001 	sub	r0, r0, #1
603051e8:	eef1fa10 	vmrs	APSR_nzcv, fpscr
603051ec:	1afffff1 	bne	603051b8 <_vfprintf_r+0x830>
603051f0:	eef60b00 	vmov.f64	d16, #96	; 0x3f000000  0.5
603051f4:	eeb40be0 	vcmpe.f64	d0, d16
603051f8:	eef1fa10 	vmrs	APSR_nzcv, fpscr
603051fc:	ca000004 	bgt	60305214 <_vfprintf_r+0x88c>
60305200:	eeb40b60 	vcmp.f64	d0, d16
60305204:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60305208:	1a000033 	bne	603052dc <_vfprintf_r+0x954>
6030520c:	e31c0001 	tst	ip, #1
60305210:	0a000031 	beq	603052dc <_vfprintf_r+0x954>
60305214:	e5d1c00f 	ldrb	ip, [r1, #15]
60305218:	e3a0e030 	mov	lr, #48	; 0x30
6030521c:	e58d3070 	str	r3, [sp, #112]	; 0x70
60305220:	e59d0070 	ldr	r0, [sp, #112]	; 0x70
60305224:	e2402001 	sub	r2, r0, #1
60305228:	e58d2070 	str	r2, [sp, #112]	; 0x70
6030522c:	e5502001 	ldrb	r2, [r0, #-1]
60305230:	e152000c 	cmp	r2, ip
60305234:	0a00001d 	beq	603052b0 <_vfprintf_r+0x928>
60305238:	e3520039 	cmp	r2, #57	; 0x39
6030523c:	12822001 	addne	r2, r2, #1
60305240:	05d1200a 	ldrbeq	r2, [r1, #10]
60305244:	16ef2072 	uxtbne	r2, r2
60305248:	e5402001 	strb	r2, [r0, #-1]
6030524c:	e043300a 	sub	r3, r3, sl
60305250:	e59d8060 	ldr	r8, [sp, #96]	; 0x60
60305254:	e3590047 	cmp	r9, #71	; 0x47
60305258:	e58d300c 	str	r3, [sp, #12]
6030525c:	1a000061 	bne	603053e8 <_vfprintf_r+0xa60>
60305260:	e3780003 	cmn	r8, #3
60305264:	a3a03000 	movge	r3, #0
60305268:	b3a03001 	movlt	r3, #1
6030526c:	e1580006 	cmp	r8, r6
60305270:	c3833001 	orrgt	r3, r3, #1
60305274:	e3530000 	cmp	r3, #0
60305278:	159d3010 	ldrne	r3, [sp, #16]
6030527c:	12433002 	subne	r3, r3, #2
60305280:	158d3010 	strne	r3, [sp, #16]
60305284:	1a000059 	bne	603053f0 <_vfprintf_r+0xa68>
60305288:	e59d300c 	ldr	r3, [sp, #12]
6030528c:	e1580003 	cmp	r8, r3
60305290:	ba000176 	blt	60305870 <_vfprintf_r+0xee8>
60305294:	e31b0001 	tst	fp, #1
60305298:	159d3028 	ldrne	r3, [sp, #40]	; 0x28
6030529c:	10886003 	addne	r6, r8, r3
603052a0:	01a06008 	moveq	r6, r8
603052a4:	ea000177 	b	60305888 <_vfprintf_r+0xf00>
603052a8:	e58da008 	str	sl, [sp, #8]
603052ac:	eaffffa5 	b	60305148 <_vfprintf_r+0x7c0>
603052b0:	e540e001 	strb	lr, [r0, #-1]
603052b4:	eaffffd9 	b	60305220 <_vfprintf_r+0x898>
603052b8:	e4c1e001 	strb	lr, [r1], #1
603052bc:	e04c0001 	sub	r0, ip, r1
603052c0:	e3500000 	cmp	r0, #0
603052c4:	aafffffb 	bge	603052b8 <_vfprintf_r+0x930>
603052c8:	e3720001 	cmn	r2, #1
603052cc:	a2822001 	addge	r2, r2, #1
603052d0:	b3a02000 	movlt	r2, #0
603052d4:	e0833002 	add	r3, r3, r2
603052d8:	eaffffdb 	b	6030524c <_vfprintf_r+0x8c4>
603052dc:	e1a01003 	mov	r1, r3
603052e0:	e083c002 	add	ip, r3, r2
603052e4:	e3a0e030 	mov	lr, #48	; 0x30
603052e8:	eafffff3 	b	603052bc <_vfprintf_r+0x934>
603052ec:	e3590046 	cmp	r9, #70	; 0x46
603052f0:	01a08006 	moveq	r8, r6
603052f4:	03a01003 	moveq	r1, #3
603052f8:	0a000003 	beq	6030530c <_vfprintf_r+0x984>
603052fc:	e3590045 	cmp	r9, #69	; 0x45
60305300:	e3a01002 	mov	r1, #2
60305304:	02868001 	addeq	r8, r6, #1
60305308:	11a08006 	movne	r8, r6
6030530c:	e28d3070 	add	r3, sp, #112	; 0x70
60305310:	eeb00b49 	vmov.f64	d0, d9
60305314:	e58d3004 	str	r3, [sp, #4]
60305318:	e1a02008 	mov	r2, r8
6030531c:	e28d3064 	add	r3, sp, #100	; 0x64
60305320:	e1a00007 	mov	r0, r7
60305324:	e58d3000 	str	r3, [sp]
60305328:	e28d3060 	add	r3, sp, #96	; 0x60
6030532c:	ebfff132 	bl	603017fc <_dtoa_r>
60305330:	e3590047 	cmp	r9, #71	; 0x47
60305334:	e1a0a000 	mov	sl, r0
60305338:	1a000003 	bne	6030534c <_vfprintf_r+0x9c4>
6030533c:	e31b0001 	tst	fp, #1
60305340:	1a000001 	bne	6030534c <_vfprintf_r+0x9c4>
60305344:	e59d3070 	ldr	r3, [sp, #112]	; 0x70
60305348:	eaffffbf 	b	6030524c <_vfprintf_r+0x8c4>
6030534c:	e08a3008 	add	r3, sl, r8
60305350:	e3590046 	cmp	r9, #70	; 0x46
60305354:	1a00000c 	bne	6030538c <_vfprintf_r+0xa04>
60305358:	eeb59b40 	vcmp.f64	d9, #0.0
6030535c:	e5da1000 	ldrb	r1, [sl]
60305360:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60305364:	13a02001 	movne	r2, #1
60305368:	03a02000 	moveq	r2, #0
6030536c:	e3510030 	cmp	r1, #48	; 0x30
60305370:	13a02000 	movne	r2, #0
60305374:	02022001 	andeq	r2, r2, #1
60305378:	e3520000 	cmp	r2, #0
6030537c:	12688001 	rsbne	r8, r8, #1
60305380:	158d8060 	strne	r8, [sp, #96]	; 0x60
60305384:	e59d2060 	ldr	r2, [sp, #96]	; 0x60
60305388:	e0833002 	add	r3, r3, r2
6030538c:	eeb59b40 	vcmp.f64	d9, #0.0
60305390:	e3a01030 	mov	r1, #48	; 0x30
60305394:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60305398:	058d3070 	streq	r3, [sp, #112]	; 0x70
6030539c:	e59d2070 	ldr	r2, [sp, #112]	; 0x70
603053a0:	e1530002 	cmp	r3, r2
603053a4:	9affffe6 	bls	60305344 <_vfprintf_r+0x9bc>
603053a8:	e2820001 	add	r0, r2, #1
603053ac:	e58d0070 	str	r0, [sp, #112]	; 0x70
603053b0:	e5c21000 	strb	r1, [r2]
603053b4:	eafffff8 	b	6030539c <_vfprintf_r+0xa14>
603053b8:	ffffffff 	.word	0xffffffff
603053bc:	7fefffff 	.word	0x7fefffff
603053c0:	60316ec1 	.word	0x60316ec1
603053c4:	60316ec5 	.word	0x60316ec5
603053c8:	60316ec9 	.word	0x60316ec9
603053cc:	60316ecd 	.word	0x60316ecd
603053d0:	66666667 	.word	0x66666667
603053d4:	60315328 	.word	0x60315328
603053d8:	60316e9f 	.word	0x60316e9f
603053dc:	60316eb0 	.word	0x60316eb0
603053e0:	60316ddc 	.word	0x60316ddc
603053e4:	60315338 	.word	0x60315338
603053e8:	e3590046 	cmp	r9, #70	; 0x46
603053ec:	0a0000f8 	beq	603057d4 <_vfprintf_r+0xe4c>
603053f0:	e59d3010 	ldr	r3, [sp, #16]
603053f4:	e2486001 	sub	r6, r8, #1
603053f8:	e58d6060 	str	r6, [sp, #96]	; 0x60
603053fc:	e3c32020 	bic	r2, r3, #32
60305400:	e5dd3010 	ldrb	r3, [sp, #16]
60305404:	e3520041 	cmp	r2, #65	; 0x41
60305408:	0283300f 	addeq	r3, r3, #15
6030540c:	06ef3073 	uxtbeq	r3, r3
60305410:	e3560000 	cmp	r6, #0
60305414:	b2686001 	rsblt	r6, r8, #1
60305418:	e5cd3068 	strb	r3, [sp, #104]	; 0x68
6030541c:	b3a0302d 	movlt	r3, #45	; 0x2d
60305420:	a3a0302b 	movge	r3, #43	; 0x2b
60305424:	e3560009 	cmp	r6, #9
60305428:	e5cd3069 	strb	r3, [sp, #105]	; 0x69
6030542c:	da0000e0 	ble	603057b4 <_vfprintf_r+0xe2c>
60305430:	e28d9077 	add	r9, sp, #119	; 0x77
60305434:	e1a00006 	mov	r0, r6
60305438:	e3a0100a 	mov	r1, #10
6030543c:	eb000901 	bl	60307848 <__aeabi_idivmod>
60305440:	e51f2078 	ldr	r2, [pc, #-120]	; 603053d0 <_vfprintf_r+0xa48>
60305444:	e1a08009 	mov	r8, r9
60305448:	e2811030 	add	r1, r1, #48	; 0x30
6030544c:	e1a03006 	mov	r3, r6
60305450:	e2499001 	sub	r9, r9, #1
60305454:	e3530063 	cmp	r3, #99	; 0x63
60305458:	e5481001 	strb	r1, [r8, #-1]
6030545c:	e0c21296 	smull	r1, r2, r6, r2
60305460:	e1a06fc6 	asr	r6, r6, #31
60305464:	e0666142 	rsb	r6, r6, r2, asr #2
60305468:	cafffff1 	bgt	60305434 <_vfprintf_r+0xaac>
6030546c:	e2483002 	sub	r3, r8, #2
60305470:	e28d006a 	add	r0, sp, #106	; 0x6a
60305474:	e1a02003 	mov	r2, r3
60305478:	e28d1077 	add	r1, sp, #119	; 0x77
6030547c:	e2866030 	add	r6, r6, #48	; 0x30
60305480:	e5496001 	strb	r6, [r9, #-1]
60305484:	e1520001 	cmp	r2, r1
60305488:	3a0000c6 	bcc	603057a8 <_vfprintf_r+0xe20>
6030548c:	e1530001 	cmp	r3, r1
60305490:	e28d2079 	add	r2, sp, #121	; 0x79
60305494:	90428008 	subls	r8, r2, r8
60305498:	83a08000 	movhi	r8, #0
6030549c:	e28d306a 	add	r3, sp, #106	; 0x6a
603054a0:	e0838008 	add	r8, r3, r8
603054a4:	e59d200c 	ldr	r2, [sp, #12]
603054a8:	e28d3068 	add	r3, sp, #104	; 0x68
603054ac:	e0483003 	sub	r3, r8, r3
603054b0:	e58d3048 	str	r3, [sp, #72]	; 0x48
603054b4:	e0836002 	add	r6, r3, r2
603054b8:	e3520001 	cmp	r2, #1
603054bc:	ca000001 	bgt	603054c8 <_vfprintf_r+0xb40>
603054c0:	e31b0001 	tst	fp, #1
603054c4:	0a000001 	beq	603054d0 <_vfprintf_r+0xb48>
603054c8:	e59d3028 	ldr	r3, [sp, #40]	; 0x28
603054cc:	e0866003 	add	r6, r6, r3
603054d0:	e3cb3b01 	bic	r3, fp, #1024	; 0x400
603054d4:	e3833c01 	orr	r3, r3, #256	; 0x100
603054d8:	e58d3034 	str	r3, [sp, #52]	; 0x34
603054dc:	e3a03000 	mov	r3, #0
603054e0:	e58d3018 	str	r3, [sp, #24]
603054e4:	e1a08003 	mov	r8, r3
603054e8:	e58d301c 	str	r3, [sp, #28]
603054ec:	e59d303c 	ldr	r3, [sp, #60]	; 0x3c
603054f0:	e3a09000 	mov	r9, #0
603054f4:	e59db034 	ldr	fp, [sp, #52]	; 0x34
603054f8:	e3530000 	cmp	r3, #0
603054fc:	13a0302d 	movne	r3, #45	; 0x2d
60305500:	15cd305b 	strbne	r3, [sp, #91]	; 0x5b
60305504:	e1590006 	cmp	r9, r6
60305508:	a1a03009 	movge	r3, r9
6030550c:	b1a03006 	movlt	r3, r6
60305510:	e58d3034 	str	r3, [sp, #52]	; 0x34
60305514:	e5dd305b 	ldrb	r3, [sp, #91]	; 0x5b
60305518:	e3530000 	cmp	r3, #0
6030551c:	159d3034 	ldrne	r3, [sp, #52]	; 0x34
60305520:	12833001 	addne	r3, r3, #1
60305524:	158d3034 	strne	r3, [sp, #52]	; 0x34
60305528:	e21b3002 	ands	r3, fp, #2
6030552c:	e58d303c 	str	r3, [sp, #60]	; 0x3c
60305530:	159d3034 	ldrne	r3, [sp, #52]	; 0x34
60305534:	12833002 	addne	r3, r3, #2
60305538:	158d3034 	strne	r3, [sp, #52]	; 0x34
6030553c:	e21b3084 	ands	r3, fp, #132	; 0x84
60305540:	e58d3050 	str	r3, [sp, #80]	; 0x50
60305544:	1a00001a 	bne	603055b4 <_vfprintf_r+0xc2c>
60305548:	e59d3030 	ldr	r3, [sp, #48]	; 0x30
6030554c:	e59d2034 	ldr	r2, [sp, #52]	; 0x34
60305550:	e0433002 	sub	r3, r3, r2
60305554:	e3530000 	cmp	r3, #0
60305558:	da000015 	ble	603055b4 <_vfprintf_r+0xc2c>
6030555c:	e59d207c 	ldr	r2, [sp, #124]	; 0x7c
60305560:	e2840008 	add	r0, r4, #8
60305564:	e51fc198 	ldr	ip, [pc, #-408]	; 603053d4 <_vfprintf_r+0xa4c>
60305568:	e3530010 	cmp	r3, #16
6030556c:	e59d1080 	ldr	r1, [sp, #128]	; 0x80
60305570:	e2822001 	add	r2, r2, #1
60305574:	e584c000 	str	ip, [r4]
60305578:	ca00019f 	bgt	60305bfc <_vfprintf_r+0x1274>
6030557c:	e3520007 	cmp	r2, #7
60305580:	e0811003 	add	r1, r1, r3
60305584:	e5843004 	str	r3, [r4, #4]
60305588:	d1a04000 	movle	r4, r0
6030558c:	e58d1080 	str	r1, [sp, #128]	; 0x80
60305590:	e58d207c 	str	r2, [sp, #124]	; 0x7c
60305594:	da000006 	ble	603055b4 <_vfprintf_r+0xc2c>
60305598:	e28d2078 	add	r2, sp, #120	; 0x78
6030559c:	e1a01005 	mov	r1, r5
603055a0:	e1a00007 	mov	r0, r7
603055a4:	ebfff8e1 	bl	60303930 <__sprint_r>
603055a8:	e3500000 	cmp	r0, #0
603055ac:	1a000447 	bne	603066d0 <_vfprintf_r+0x1d48>
603055b0:	e28d4084 	add	r4, sp, #132	; 0x84
603055b4:	e5dd305b 	ldrb	r3, [sp, #91]	; 0x5b
603055b8:	e3530000 	cmp	r3, #0
603055bc:	0a000013 	beq	60305610 <_vfprintf_r+0xc88>
603055c0:	e28d305b 	add	r3, sp, #91	; 0x5b
603055c4:	e5843000 	str	r3, [r4]
603055c8:	e3a03001 	mov	r3, #1
603055cc:	e5843004 	str	r3, [r4, #4]
603055d0:	e59d3080 	ldr	r3, [sp, #128]	; 0x80
603055d4:	e2833001 	add	r3, r3, #1
603055d8:	e58d3080 	str	r3, [sp, #128]	; 0x80
603055dc:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
603055e0:	e2833001 	add	r3, r3, #1
603055e4:	e58d307c 	str	r3, [sp, #124]	; 0x7c
603055e8:	e3530007 	cmp	r3, #7
603055ec:	d2844008 	addle	r4, r4, #8
603055f0:	da000006 	ble	60305610 <_vfprintf_r+0xc88>
603055f4:	e28d2078 	add	r2, sp, #120	; 0x78
603055f8:	e1a01005 	mov	r1, r5
603055fc:	e1a00007 	mov	r0, r7
60305600:	ebfff8ca 	bl	60303930 <__sprint_r>
60305604:	e3500000 	cmp	r0, #0
60305608:	1a000430 	bne	603066d0 <_vfprintf_r+0x1d48>
6030560c:	e28d4084 	add	r4, sp, #132	; 0x84
60305610:	e59d303c 	ldr	r3, [sp, #60]	; 0x3c
60305614:	e3530000 	cmp	r3, #0
60305618:	0a000013 	beq	6030566c <_vfprintf_r+0xce4>
6030561c:	e28d305c 	add	r3, sp, #92	; 0x5c
60305620:	e5843000 	str	r3, [r4]
60305624:	e3a03002 	mov	r3, #2
60305628:	e5843004 	str	r3, [r4, #4]
6030562c:	e59d3080 	ldr	r3, [sp, #128]	; 0x80
60305630:	e2833002 	add	r3, r3, #2
60305634:	e58d3080 	str	r3, [sp, #128]	; 0x80
60305638:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
6030563c:	e2833001 	add	r3, r3, #1
60305640:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60305644:	e3530007 	cmp	r3, #7
60305648:	d2844008 	addle	r4, r4, #8
6030564c:	da000006 	ble	6030566c <_vfprintf_r+0xce4>
60305650:	e28d2078 	add	r2, sp, #120	; 0x78
60305654:	e1a01005 	mov	r1, r5
60305658:	e1a00007 	mov	r0, r7
6030565c:	ebfff8b3 	bl	60303930 <__sprint_r>
60305660:	e3500000 	cmp	r0, #0
60305664:	1a000419 	bne	603066d0 <_vfprintf_r+0x1d48>
60305668:	e28d4084 	add	r4, sp, #132	; 0x84
6030566c:	e59d3050 	ldr	r3, [sp, #80]	; 0x50
60305670:	e3530080 	cmp	r3, #128	; 0x80
60305674:	1a00001d 	bne	603056f0 <_vfprintf_r+0xd68>
60305678:	e59d3030 	ldr	r3, [sp, #48]	; 0x30
6030567c:	e59d2034 	ldr	r2, [sp, #52]	; 0x34
60305680:	e0433002 	sub	r3, r3, r2
60305684:	e58d303c 	str	r3, [sp, #60]	; 0x3c
60305688:	e3530000 	cmp	r3, #0
6030568c:	da000017 	ble	603056f0 <_vfprintf_r+0xd68>
60305690:	e59d003c 	ldr	r0, [sp, #60]	; 0x3c
60305694:	e2841008 	add	r1, r4, #8
60305698:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
6030569c:	e3500010 	cmp	r0, #16
603056a0:	e51f02c4 	ldr	r0, [pc, #-708]	; 603053e4 <_vfprintf_r+0xa5c>
603056a4:	e59d2080 	ldr	r2, [sp, #128]	; 0x80
603056a8:	e2833001 	add	r3, r3, #1
603056ac:	e5840000 	str	r0, [r4]
603056b0:	ca000164 	bgt	60305c48 <_vfprintf_r+0x12c0>
603056b4:	e59d003c 	ldr	r0, [sp, #60]	; 0x3c
603056b8:	e3530007 	cmp	r3, #7
603056bc:	e5840004 	str	r0, [r4, #4]
603056c0:	d1a04001 	movle	r4, r1
603056c4:	e0802002 	add	r2, r0, r2
603056c8:	e58d307c 	str	r3, [sp, #124]	; 0x7c
603056cc:	e58d2080 	str	r2, [sp, #128]	; 0x80
603056d0:	da000006 	ble	603056f0 <_vfprintf_r+0xd68>
603056d4:	e28d2078 	add	r2, sp, #120	; 0x78
603056d8:	e1a01005 	mov	r1, r5
603056dc:	e1a00007 	mov	r0, r7
603056e0:	ebfff892 	bl	60303930 <__sprint_r>
603056e4:	e3500000 	cmp	r0, #0
603056e8:	1a0003f8 	bne	603066d0 <_vfprintf_r+0x1d48>
603056ec:	e28d4084 	add	r4, sp, #132	; 0x84
603056f0:	e0499006 	sub	r9, r9, r6
603056f4:	e3590000 	cmp	r9, #0
603056f8:	da000015 	ble	60305754 <_vfprintf_r+0xdcc>
603056fc:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
60305700:	e2841008 	add	r1, r4, #8
60305704:	e51f0328 	ldr	r0, [pc, #-808]	; 603053e4 <_vfprintf_r+0xa5c>
60305708:	e3590010 	cmp	r9, #16
6030570c:	e59d2080 	ldr	r2, [sp, #128]	; 0x80
60305710:	e2833001 	add	r3, r3, #1
60305714:	e5840000 	str	r0, [r4]
60305718:	ca00015d 	bgt	60305c94 <_vfprintf_r+0x130c>
6030571c:	e3530007 	cmp	r3, #7
60305720:	e5849004 	str	r9, [r4, #4]
60305724:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60305728:	e0899002 	add	r9, r9, r2
6030572c:	d1a04001 	movle	r4, r1
60305730:	e58d9080 	str	r9, [sp, #128]	; 0x80
60305734:	da000006 	ble	60305754 <_vfprintf_r+0xdcc>
60305738:	e28d2078 	add	r2, sp, #120	; 0x78
6030573c:	e1a01005 	mov	r1, r5
60305740:	e1a00007 	mov	r0, r7
60305744:	ebfff879 	bl	60303930 <__sprint_r>
60305748:	e3500000 	cmp	r0, #0
6030574c:	1a0003df 	bne	603066d0 <_vfprintf_r+0x1d48>
60305750:	e28d4084 	add	r4, sp, #132	; 0x84
60305754:	e59d0080 	ldr	r0, [sp, #128]	; 0x80
60305758:	e31b0c01 	tst	fp, #256	; 0x100
6030575c:	1a00015d 	bne	60305cd8 <_vfprintf_r+0x1350>
60305760:	e0860000 	add	r0, r6, r0
60305764:	e584a000 	str	sl, [r4]
60305768:	e5846004 	str	r6, [r4, #4]
6030576c:	e58d0080 	str	r0, [sp, #128]	; 0x80
60305770:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
60305774:	e2833001 	add	r3, r3, #1
60305778:	e58d307c 	str	r3, [sp, #124]	; 0x7c
6030577c:	e3530007 	cmp	r3, #7
60305780:	d2844008 	addle	r4, r4, #8
60305784:	da00018c 	ble	60305dbc <_vfprintf_r+0x1434>
60305788:	e28d2078 	add	r2, sp, #120	; 0x78
6030578c:	e1a01005 	mov	r1, r5
60305790:	e1a00007 	mov	r0, r7
60305794:	ebfff865 	bl	60303930 <__sprint_r>
60305798:	e3500000 	cmp	r0, #0
6030579c:	1a0003cb 	bne	603066d0 <_vfprintf_r+0x1d48>
603057a0:	e28d4084 	add	r4, sp, #132	; 0x84
603057a4:	ea000184 	b	60305dbc <_vfprintf_r+0x1434>
603057a8:	e4d2c001 	ldrb	ip, [r2], #1
603057ac:	e4c0c001 	strb	ip, [r0], #1
603057b0:	eaffff33 	b	60305484 <_vfprintf_r+0xafc>
603057b4:	e3520041 	cmp	r2, #65	; 0x41
603057b8:	e2866030 	add	r6, r6, #48	; 0x30
603057bc:	128d806b 	addne	r8, sp, #107	; 0x6b
603057c0:	028d806a 	addeq	r8, sp, #106	; 0x6a
603057c4:	13a03030 	movne	r3, #48	; 0x30
603057c8:	15cd306a 	strbne	r3, [sp, #106]	; 0x6a
603057cc:	e4c86001 	strb	r6, [r8], #1
603057d0:	eaffff33 	b	603054a4 <_vfprintf_r+0xb1c>
603057d4:	e20b3001 	and	r3, fp, #1
603057d8:	e3580000 	cmp	r8, #0
603057dc:	e1833006 	orr	r3, r3, r6
603057e0:	da000005 	ble	603057fc <_vfprintf_r+0xe74>
603057e4:	e3530000 	cmp	r3, #0
603057e8:	159d3028 	ldrne	r3, [sp, #40]	; 0x28
603057ec:	10883003 	addne	r3, r8, r3
603057f0:	1a000008 	bne	60305818 <_vfprintf_r+0xe90>
603057f4:	e1a06008 	mov	r6, r8
603057f8:	ea000007 	b	6030581c <_vfprintf_r+0xe94>
603057fc:	e3530000 	cmp	r3, #0
60305800:	03a03066 	moveq	r3, #102	; 0x66
60305804:	03a06001 	moveq	r6, #1
60305808:	058d3010 	streq	r3, [sp, #16]
6030580c:	0a000004 	beq	60305824 <_vfprintf_r+0xe9c>
60305810:	e59d3028 	ldr	r3, [sp, #40]	; 0x28
60305814:	e2833001 	add	r3, r3, #1
60305818:	e0866003 	add	r6, r6, r3
6030581c:	e3a03066 	mov	r3, #102	; 0x66
60305820:	e58d3010 	str	r3, [sp, #16]
60305824:	e21b3b01 	ands	r3, fp, #1024	; 0x400
60305828:	e58d301c 	str	r3, [sp, #28]
6030582c:	059d301c 	ldreq	r3, [sp, #28]
60305830:	058d3018 	streq	r3, [sp, #24]
60305834:	0affff2c 	beq	603054ec <_vfprintf_r+0xb64>
60305838:	e3a03000 	mov	r3, #0
6030583c:	e3580000 	cmp	r8, #0
60305840:	e58d3018 	str	r3, [sp, #24]
60305844:	e58d301c 	str	r3, [sp, #28]
60305848:	daffff27 	ble	603054ec <_vfprintf_r+0xb64>
6030584c:	e59d3020 	ldr	r3, [sp, #32]
60305850:	e5d33000 	ldrb	r3, [r3]
60305854:	e35300ff 	cmp	r3, #255	; 0xff
60305858:	1a00000c 	bne	60305890 <_vfprintf_r+0xf08>
6030585c:	e1cd21d8 	ldrd	r2, [sp, #24]
60305860:	e0833002 	add	r3, r3, r2
60305864:	e59d2038 	ldr	r2, [sp, #56]	; 0x38
60305868:	e0266392 	mla	r6, r2, r3, r6
6030586c:	eaffff1e 	b	603054ec <_vfprintf_r+0xb64>
60305870:	e59d300c 	ldr	r3, [sp, #12]
60305874:	e3580000 	cmp	r8, #0
60305878:	e59d2028 	ldr	r2, [sp, #40]	; 0x28
6030587c:	e0836002 	add	r6, r3, r2
60305880:	d2683001 	rsble	r3, r8, #1
60305884:	d0866003 	addle	r6, r6, r3
60305888:	e3a03067 	mov	r3, #103	; 0x67
6030588c:	eaffffe3 	b	60305820 <_vfprintf_r+0xe98>
60305890:	e1530008 	cmp	r3, r8
60305894:	aafffff0 	bge	6030585c <_vfprintf_r+0xed4>
60305898:	e0488003 	sub	r8, r8, r3
6030589c:	e59d3020 	ldr	r3, [sp, #32]
603058a0:	e5d33001 	ldrb	r3, [r3, #1]
603058a4:	e3530000 	cmp	r3, #0
603058a8:	059d3018 	ldreq	r3, [sp, #24]
603058ac:	02833001 	addeq	r3, r3, #1
603058b0:	058d3018 	streq	r3, [sp, #24]
603058b4:	0affffe4 	beq	6030584c <_vfprintf_r+0xec4>
603058b8:	e59d301c 	ldr	r3, [sp, #28]
603058bc:	e2833001 	add	r3, r3, #1
603058c0:	e58d301c 	str	r3, [sp, #28]
603058c4:	e59d3020 	ldr	r3, [sp, #32]
603058c8:	e2833001 	add	r3, r3, #1
603058cc:	e58d3020 	str	r3, [sp, #32]
603058d0:	eaffffdd 	b	6030584c <_vfprintf_r+0xec4>
603058d4:	e2883004 	add	r3, r8, #4
603058d8:	e31b0020 	tst	fp, #32
603058dc:	e58d3014 	str	r3, [sp, #20]
603058e0:	0a000007 	beq	60305904 <_vfprintf_r+0xf7c>
603058e4:	e5983000 	ldr	r3, [r8]
603058e8:	e59d202c 	ldr	r2, [sp, #44]	; 0x2c
603058ec:	e59d102c 	ldr	r1, [sp, #44]	; 0x2c
603058f0:	e1a02fc2 	asr	r2, r2, #31
603058f4:	e8830006 	stm	r3, {r1, r2}
603058f8:	e59d8014 	ldr	r8, [sp, #20]
603058fc:	e59da024 	ldr	sl, [sp, #36]	; 0x24
60305900:	eafffc82 	b	60304b10 <_vfprintf_r+0x188>
60305904:	e31b0010 	tst	fp, #16
60305908:	0a000003 	beq	6030591c <_vfprintf_r+0xf94>
6030590c:	e5983000 	ldr	r3, [r8]
60305910:	e59d202c 	ldr	r2, [sp, #44]	; 0x2c
60305914:	e5832000 	str	r2, [r3]
60305918:	eafffff6 	b	603058f8 <_vfprintf_r+0xf70>
6030591c:	e31b0040 	tst	fp, #64	; 0x40
60305920:	15983000 	ldrne	r3, [r8]
60305924:	159d202c 	ldrne	r2, [sp, #44]	; 0x2c
60305928:	11c320b0 	strhne	r2, [r3]
6030592c:	1afffff1 	bne	603058f8 <_vfprintf_r+0xf70>
60305930:	e31b0c02 	tst	fp, #512	; 0x200
60305934:	15983000 	ldrne	r3, [r8]
60305938:	159d202c 	ldrne	r2, [sp, #44]	; 0x2c
6030593c:	15c32000 	strbne	r2, [r3]
60305940:	1affffec 	bne	603058f8 <_vfprintf_r+0xf70>
60305944:	eafffff0 	b	6030590c <_vfprintf_r+0xf84>
60305948:	e38bb010 	orr	fp, fp, #16
6030594c:	e21b3020 	ands	r3, fp, #32
60305950:	0a00000a 	beq	60305980 <_vfprintf_r+0xff8>
60305954:	e2888007 	add	r8, r8, #7
60305958:	e3c83007 	bic	r3, r8, #7
6030595c:	e1a02003 	mov	r2, r3
60305960:	e5939004 	ldr	r9, [r3, #4]
60305964:	e4928008 	ldr	r8, [r2], #8
60305968:	e58d2014 	str	r2, [sp, #20]
6030596c:	e3cbbb01 	bic	fp, fp, #1024	; 0x400
60305970:	e3a03000 	mov	r3, #0
60305974:	e3a02000 	mov	r2, #0
60305978:	e5cd205b 	strb	r2, [sp, #91]	; 0x5b
6030597c:	eafffd6f 	b	60304f40 <_vfprintf_r+0x5b8>
60305980:	e1a02008 	mov	r2, r8
60305984:	e21b9010 	ands	r9, fp, #16
60305988:	e4928004 	ldr	r8, [r2], #4
6030598c:	e58d2014 	str	r2, [sp, #20]
60305990:	0a000001 	beq	6030599c <_vfprintf_r+0x1014>
60305994:	e1a09003 	mov	r9, r3
60305998:	eafffff3 	b	6030596c <_vfprintf_r+0xfe4>
6030599c:	e21b3040 	ands	r3, fp, #64	; 0x40
603059a0:	16ff8078 	uxthne	r8, r8
603059a4:	1afffff0 	bne	6030596c <_vfprintf_r+0xfe4>
603059a8:	e21b9c02 	ands	r9, fp, #512	; 0x200
603059ac:	0affffee 	beq	6030596c <_vfprintf_r+0xfe4>
603059b0:	e6ef8078 	uxtb	r8, r8
603059b4:	eafffff6 	b	60305994 <_vfprintf_r+0x100c>
603059b8:	e1a03008 	mov	r3, r8
603059bc:	e3a02078 	mov	r2, #120	; 0x78
603059c0:	e3a09000 	mov	r9, #0
603059c4:	e38bb002 	orr	fp, fp, #2
603059c8:	e58d2010 	str	r2, [sp, #16]
603059cc:	e4938004 	ldr	r8, [r3], #4
603059d0:	e58d3014 	str	r3, [sp, #20]
603059d4:	e3073830 	movw	r3, #30768	; 0x7830
603059d8:	e1cd35bc 	strh	r3, [sp, #92]	; 0x5c
603059dc:	e51f3608 	ldr	r3, [pc, #-1544]	; 603053dc <_vfprintf_r+0xa54>
603059e0:	e58d304c 	str	r3, [sp, #76]	; 0x4c
603059e4:	e3a03002 	mov	r3, #2
603059e8:	eaffffe1 	b	60305974 <_vfprintf_r+0xfec>
603059ec:	e1a03008 	mov	r3, r8
603059f0:	e3760001 	cmn	r6, #1
603059f4:	e3a08000 	mov	r8, #0
603059f8:	e5cd805b 	strb	r8, [sp, #91]	; 0x5b
603059fc:	e493a004 	ldr	sl, [r3], #4
60305a00:	e58d3014 	str	r3, [sp, #20]
60305a04:	0a00000d 	beq	60305a40 <_vfprintf_r+0x10b8>
60305a08:	e1a02006 	mov	r2, r6
60305a0c:	e1a01008 	mov	r1, r8
60305a10:	e1a0000a 	mov	r0, sl
60305a14:	eb003cf9 	bl	60314e00 <__wrap_memchr>
60305a18:	e2503000 	subs	r3, r0, #0
60305a1c:	e58d3008 	str	r3, [sp, #8]
60305a20:	0a000073 	beq	60305bf4 <_vfprintf_r+0x126c>
60305a24:	e043600a 	sub	r6, r3, sl
60305a28:	e1a09008 	mov	r9, r8
60305a2c:	e1a03008 	mov	r3, r8
60305a30:	e58d8008 	str	r8, [sp, #8]
60305a34:	e58d8018 	str	r8, [sp, #24]
60305a38:	e58d801c 	str	r8, [sp, #28]
60305a3c:	eafffeb0 	b	60305504 <_vfprintf_r+0xb7c>
60305a40:	e1a0000a 	mov	r0, sl
60305a44:	e1a09008 	mov	r9, r8
60305a48:	eb003cf2 	bl	60314e18 <__wrap_strlen>
60305a4c:	e1a06000 	mov	r6, r0
60305a50:	e58d8008 	str	r8, [sp, #8]
60305a54:	e1a03009 	mov	r3, r9
60305a58:	eafffd7f 	b	6030505c <_vfprintf_r+0x6d4>
60305a5c:	e38bb010 	orr	fp, fp, #16
60305a60:	e21b3020 	ands	r3, fp, #32
60305a64:	0a000007 	beq	60305a88 <_vfprintf_r+0x1100>
60305a68:	e2888007 	add	r8, r8, #7
60305a6c:	e3c83007 	bic	r3, r8, #7
60305a70:	e1a02003 	mov	r2, r3
60305a74:	e5939004 	ldr	r9, [r3, #4]
60305a78:	e4928008 	ldr	r8, [r2], #8
60305a7c:	e58d2014 	str	r2, [sp, #20]
60305a80:	e3a03001 	mov	r3, #1
60305a84:	eaffffba 	b	60305974 <_vfprintf_r+0xfec>
60305a88:	e1a02008 	mov	r2, r8
60305a8c:	e21b9010 	ands	r9, fp, #16
60305a90:	e4928004 	ldr	r8, [r2], #4
60305a94:	e58d2014 	str	r2, [sp, #20]
60305a98:	0a000001 	beq	60305aa4 <_vfprintf_r+0x111c>
60305a9c:	e1a09003 	mov	r9, r3
60305aa0:	eafffff6 	b	60305a80 <_vfprintf_r+0x10f8>
60305aa4:	e21b3040 	ands	r3, fp, #64	; 0x40
60305aa8:	16ff8078 	uxthne	r8, r8
60305aac:	1afffff3 	bne	60305a80 <_vfprintf_r+0x10f8>
60305ab0:	e21b9c02 	ands	r9, fp, #512	; 0x200
60305ab4:	0afffff1 	beq	60305a80 <_vfprintf_r+0x10f8>
60305ab8:	e6ef8078 	uxtb	r8, r8
60305abc:	eafffff6 	b	60305a9c <_vfprintf_r+0x1114>
60305ac0:	e51f36f0 	ldr	r3, [pc, #-1776]	; 603053d8 <_vfprintf_r+0xa50>
60305ac4:	e58d304c 	str	r3, [sp, #76]	; 0x4c
60305ac8:	e21b3020 	ands	r3, fp, #32
60305acc:	0a000014 	beq	60305b24 <_vfprintf_r+0x119c>
60305ad0:	e2888007 	add	r8, r8, #7
60305ad4:	e3c83007 	bic	r3, r8, #7
60305ad8:	e1a02003 	mov	r2, r3
60305adc:	e5939004 	ldr	r9, [r3, #4]
60305ae0:	e4928008 	ldr	r8, [r2], #8
60305ae4:	e58d2014 	str	r2, [sp, #20]
60305ae8:	e1983009 	orrs	r3, r8, r9
60305aec:	03a03000 	moveq	r3, #0
60305af0:	120b3001 	andne	r3, fp, #1
60305af4:	e3530000 	cmp	r3, #0
60305af8:	0a000004 	beq	60305b10 <_vfprintf_r+0x1188>
60305afc:	e3a03030 	mov	r3, #48	; 0x30
60305b00:	e38bb002 	orr	fp, fp, #2
60305b04:	e5cd305c 	strb	r3, [sp, #92]	; 0x5c
60305b08:	e59d3010 	ldr	r3, [sp, #16]
60305b0c:	e5cd305d 	strb	r3, [sp, #93]	; 0x5d
60305b10:	e3cbbb01 	bic	fp, fp, #1024	; 0x400
60305b14:	e3a03002 	mov	r3, #2
60305b18:	eaffff95 	b	60305974 <_vfprintf_r+0xfec>
60305b1c:	e51f3748 	ldr	r3, [pc, #-1864]	; 603053dc <_vfprintf_r+0xa54>
60305b20:	eaffffe7 	b	60305ac4 <_vfprintf_r+0x113c>
60305b24:	e1a02008 	mov	r2, r8
60305b28:	e21b9010 	ands	r9, fp, #16
60305b2c:	e4928004 	ldr	r8, [r2], #4
60305b30:	e58d2014 	str	r2, [sp, #20]
60305b34:	0a000001 	beq	60305b40 <_vfprintf_r+0x11b8>
60305b38:	e1a09003 	mov	r9, r3
60305b3c:	eaffffe9 	b	60305ae8 <_vfprintf_r+0x1160>
60305b40:	e21b3040 	ands	r3, fp, #64	; 0x40
60305b44:	16ff8078 	uxthne	r8, r8
60305b48:	1affffe6 	bne	60305ae8 <_vfprintf_r+0x1160>
60305b4c:	e21b9c02 	ands	r9, fp, #512	; 0x200
60305b50:	0affffe4 	beq	60305ae8 <_vfprintf_r+0x1160>
60305b54:	e6ef8078 	uxtb	r8, r8
60305b58:	eafffff6 	b	60305b38 <_vfprintf_r+0x11b0>
60305b5c:	e1882009 	orr	r2, r8, r9
60305b60:	e3cbb080 	bic	fp, fp, #128	; 0x80
60305b64:	e3520000 	cmp	r2, #0
60305b68:	03560000 	cmpeq	r6, #0
60305b6c:	13a02001 	movne	r2, #1
60305b70:	03a02000 	moveq	r2, #0
60305b74:	1afffcf4 	bne	60304f4c <_vfprintf_r+0x5c4>
60305b78:	e3530000 	cmp	r3, #0
60305b7c:	1a000011 	bne	60305bc8 <_vfprintf_r+0x1240>
60305b80:	e2116001 	ands	r6, r1, #1
60305b84:	0a000010 	beq	60305bcc <_vfprintf_r+0x1244>
60305b88:	e1a06003 	mov	r6, r3
60305b8c:	e3a02030 	mov	r2, #48	; 0x30
60305b90:	e5cd2127 	strb	r2, [sp, #295]	; 0x127
60305b94:	e28daf49 	add	sl, sp, #292	; 0x124
60305b98:	e28aa003 	add	sl, sl, #3
60305b9c:	eafffcff 	b	60304fa0 <_vfprintf_r+0x618>
60305ba0:	e208300f 	and	r3, r8, #15
60305ba4:	e59d204c 	ldr	r2, [sp, #76]	; 0x4c
60305ba8:	e1a08228 	lsr	r8, r8, #4
60305bac:	e1888e09 	orr	r8, r8, r9, lsl #28
60305bb0:	e1a09229 	lsr	r9, r9, #4
60305bb4:	e7d23003 	ldrb	r3, [r2, r3]
60305bb8:	e56a3001 	strb	r3, [sl, #-1]!
60305bbc:	e1983009 	orrs	r3, r8, r9
60305bc0:	1afffff6 	bne	60305ba0 <_vfprintf_r+0x1218>
60305bc4:	eafffcf5 	b	60304fa0 <_vfprintf_r+0x618>
60305bc8:	e1a06002 	mov	r6, r2
60305bcc:	e28daf4a 	add	sl, sp, #296	; 0x128
60305bd0:	eafffcf2 	b	60304fa0 <_vfprintf_r+0x618>
60305bd4:	e59d3010 	ldr	r3, [sp, #16]
60305bd8:	e3530000 	cmp	r3, #0
60305bdc:	0a0002d6 	beq	6030673c <_vfprintf_r+0x1db4>
60305be0:	e3a00000 	mov	r0, #0
60305be4:	e5cd30c4 	strb	r3, [sp, #196]	; 0xc4
60305be8:	e5cd005b 	strb	r0, [sp, #91]	; 0x5b
60305bec:	e58d8014 	str	r8, [sp, #20]
60305bf0:	eafffcb9 	b	60304edc <_vfprintf_r+0x554>
60305bf4:	e59d9008 	ldr	r9, [sp, #8]
60305bf8:	eaffff95 	b	60305a54 <_vfprintf_r+0x10cc>
60305bfc:	e3a0c010 	mov	ip, #16
60305c00:	e3520007 	cmp	r2, #7
60305c04:	e081100c 	add	r1, r1, ip
60305c08:	e584c004 	str	ip, [r4, #4]
60305c0c:	e58d1080 	str	r1, [sp, #128]	; 0x80
60305c10:	e58d207c 	str	r2, [sp, #124]	; 0x7c
60305c14:	da000008 	ble	60305c3c <_vfprintf_r+0x12b4>
60305c18:	e28d2078 	add	r2, sp, #120	; 0x78
60305c1c:	e1a01005 	mov	r1, r5
60305c20:	e1a00007 	mov	r0, r7
60305c24:	e58d3054 	str	r3, [sp, #84]	; 0x54
60305c28:	ebfff740 	bl	60303930 <__sprint_r>
60305c2c:	e3500000 	cmp	r0, #0
60305c30:	1a0002a6 	bne	603066d0 <_vfprintf_r+0x1d48>
60305c34:	e59d3054 	ldr	r3, [sp, #84]	; 0x54
60305c38:	e28d0084 	add	r0, sp, #132	; 0x84
60305c3c:	e2433010 	sub	r3, r3, #16
60305c40:	e1a04000 	mov	r4, r0
60305c44:	eafffe44 	b	6030555c <_vfprintf_r+0xbd4>
60305c48:	e3a00010 	mov	r0, #16
60305c4c:	e3530007 	cmp	r3, #7
60305c50:	e0822000 	add	r2, r2, r0
60305c54:	e5840004 	str	r0, [r4, #4]
60305c58:	e58d2080 	str	r2, [sp, #128]	; 0x80
60305c5c:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60305c60:	da000006 	ble	60305c80 <_vfprintf_r+0x12f8>
60305c64:	e28d2078 	add	r2, sp, #120	; 0x78
60305c68:	e1a01005 	mov	r1, r5
60305c6c:	e1a00007 	mov	r0, r7
60305c70:	ebfff72e 	bl	60303930 <__sprint_r>
60305c74:	e3500000 	cmp	r0, #0
60305c78:	1a000294 	bne	603066d0 <_vfprintf_r+0x1d48>
60305c7c:	e28d1084 	add	r1, sp, #132	; 0x84
60305c80:	e59d303c 	ldr	r3, [sp, #60]	; 0x3c
60305c84:	e1a04001 	mov	r4, r1
60305c88:	e2433010 	sub	r3, r3, #16
60305c8c:	e58d303c 	str	r3, [sp, #60]	; 0x3c
60305c90:	eafffe7e 	b	60305690 <_vfprintf_r+0xd08>
60305c94:	e3a00010 	mov	r0, #16
60305c98:	e3530007 	cmp	r3, #7
60305c9c:	e0822000 	add	r2, r2, r0
60305ca0:	e5840004 	str	r0, [r4, #4]
60305ca4:	e58d2080 	str	r2, [sp, #128]	; 0x80
60305ca8:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60305cac:	da000006 	ble	60305ccc <_vfprintf_r+0x1344>
60305cb0:	e28d2078 	add	r2, sp, #120	; 0x78
60305cb4:	e1a01005 	mov	r1, r5
60305cb8:	e1a00007 	mov	r0, r7
60305cbc:	ebfff71b 	bl	60303930 <__sprint_r>
60305cc0:	e3500000 	cmp	r0, #0
60305cc4:	1a000281 	bne	603066d0 <_vfprintf_r+0x1d48>
60305cc8:	e28d1084 	add	r1, sp, #132	; 0x84
60305ccc:	e2499010 	sub	r9, r9, #16
60305cd0:	e1a04001 	mov	r4, r1
60305cd4:	eafffe88 	b	603056fc <_vfprintf_r+0xd74>
60305cd8:	e59d3010 	ldr	r3, [sp, #16]
60305cdc:	e3530065 	cmp	r3, #101	; 0x65
60305ce0:	da0001e2 	ble	60306470 <_vfprintf_r+0x1ae8>
60305ce4:	eeb58b40 	vcmp.f64	d8, #0.0
60305ce8:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60305cec:	1a00006a 	bne	60305e9c <_vfprintf_r+0x1514>
60305cf0:	e51f3918 	ldr	r3, [pc, #-2328]	; 603053e0 <_vfprintf_r+0xa58>
60305cf4:	e5843000 	str	r3, [r4]
60305cf8:	e3a03001 	mov	r3, #1
60305cfc:	e0800003 	add	r0, r0, r3
60305d00:	e5843004 	str	r3, [r4, #4]
60305d04:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
60305d08:	e58d0080 	str	r0, [sp, #128]	; 0x80
60305d0c:	e2833001 	add	r3, r3, #1
60305d10:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60305d14:	e3530007 	cmp	r3, #7
60305d18:	d2844008 	addle	r4, r4, #8
60305d1c:	da000006 	ble	60305d3c <_vfprintf_r+0x13b4>
60305d20:	e28d2078 	add	r2, sp, #120	; 0x78
60305d24:	e1a01005 	mov	r1, r5
60305d28:	e1a00007 	mov	r0, r7
60305d2c:	ebfff6ff 	bl	60303930 <__sprint_r>
60305d30:	e3500000 	cmp	r0, #0
60305d34:	1a000265 	bne	603066d0 <_vfprintf_r+0x1d48>
60305d38:	e28d4084 	add	r4, sp, #132	; 0x84
60305d3c:	e59d3060 	ldr	r3, [sp, #96]	; 0x60
60305d40:	e59d200c 	ldr	r2, [sp, #12]
60305d44:	e1530002 	cmp	r3, r2
60305d48:	ba000001 	blt	60305d54 <_vfprintf_r+0x13cc>
60305d4c:	e31b0001 	tst	fp, #1
60305d50:	0a000019 	beq	60305dbc <_vfprintf_r+0x1434>
60305d54:	e59d3040 	ldr	r3, [sp, #64]	; 0x40
60305d58:	e5843000 	str	r3, [r4]
60305d5c:	e59d3028 	ldr	r3, [sp, #40]	; 0x28
60305d60:	e5843004 	str	r3, [r4, #4]
60305d64:	e59d2028 	ldr	r2, [sp, #40]	; 0x28
60305d68:	e59d3080 	ldr	r3, [sp, #128]	; 0x80
60305d6c:	e0833002 	add	r3, r3, r2
60305d70:	e58d3080 	str	r3, [sp, #128]	; 0x80
60305d74:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
60305d78:	e2833001 	add	r3, r3, #1
60305d7c:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60305d80:	e3530007 	cmp	r3, #7
60305d84:	d2844008 	addle	r4, r4, #8
60305d88:	da000006 	ble	60305da8 <_vfprintf_r+0x1420>
60305d8c:	e28d2078 	add	r2, sp, #120	; 0x78
60305d90:	e1a01005 	mov	r1, r5
60305d94:	e1a00007 	mov	r0, r7
60305d98:	ebfff6e4 	bl	60303930 <__sprint_r>
60305d9c:	e3500000 	cmp	r0, #0
60305da0:	1a00024a 	bne	603066d0 <_vfprintf_r+0x1d48>
60305da4:	e28d4084 	add	r4, sp, #132	; 0x84
60305da8:	e59d300c 	ldr	r3, [sp, #12]
60305dac:	e2436001 	sub	r6, r3, #1
60305db0:	e3560000 	cmp	r6, #0
60305db4:	c3a08010 	movgt	r8, #16
60305db8:	ca000027 	bgt	60305e5c <_vfprintf_r+0x14d4>
60305dbc:	e31b0004 	tst	fp, #4
60305dc0:	1a000229 	bne	6030666c <_vfprintf_r+0x1ce4>
60305dc4:	e59d302c 	ldr	r3, [sp, #44]	; 0x2c
60305dc8:	e59d2030 	ldr	r2, [sp, #48]	; 0x30
60305dcc:	e59d1034 	ldr	r1, [sp, #52]	; 0x34
60305dd0:	e1520001 	cmp	r2, r1
60305dd4:	a0833002 	addge	r3, r3, r2
60305dd8:	b0833001 	addlt	r3, r3, r1
60305ddc:	e58d302c 	str	r3, [sp, #44]	; 0x2c
60305de0:	e59d3080 	ldr	r3, [sp, #128]	; 0x80
60305de4:	e3530000 	cmp	r3, #0
60305de8:	0a000005 	beq	60305e04 <_vfprintf_r+0x147c>
60305dec:	e28d2078 	add	r2, sp, #120	; 0x78
60305df0:	e1a01005 	mov	r1, r5
60305df4:	e1a00007 	mov	r0, r7
60305df8:	ebfff6cc 	bl	60303930 <__sprint_r>
60305dfc:	e3500000 	cmp	r0, #0
60305e00:	1a000232 	bne	603066d0 <_vfprintf_r+0x1d48>
60305e04:	e3a03000 	mov	r3, #0
60305e08:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60305e0c:	e59d3008 	ldr	r3, [sp, #8]
60305e10:	e3530000 	cmp	r3, #0
60305e14:	1a000244 	bne	6030672c <_vfprintf_r+0x1da4>
60305e18:	e28d4084 	add	r4, sp, #132	; 0x84
60305e1c:	eafffeb5 	b	603058f8 <_vfprintf_r+0xf70>
60305e20:	e2822010 	add	r2, r2, #16
60305e24:	e3530007 	cmp	r3, #7
60305e28:	e5848004 	str	r8, [r4, #4]
60305e2c:	e58d2080 	str	r2, [sp, #128]	; 0x80
60305e30:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60305e34:	da000006 	ble	60305e54 <_vfprintf_r+0x14cc>
60305e38:	e28d2078 	add	r2, sp, #120	; 0x78
60305e3c:	e1a01005 	mov	r1, r5
60305e40:	e1a00007 	mov	r0, r7
60305e44:	ebfff6b9 	bl	60303930 <__sprint_r>
60305e48:	e3500000 	cmp	r0, #0
60305e4c:	1a00021f 	bne	603066d0 <_vfprintf_r+0x1d48>
60305e50:	e28d1084 	add	r1, sp, #132	; 0x84
60305e54:	e2466010 	sub	r6, r6, #16
60305e58:	e1a04001 	mov	r4, r1
60305e5c:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
60305e60:	e2841008 	add	r1, r4, #8
60305e64:	e51f0a88 	ldr	r0, [pc, #-2696]	; 603053e4 <_vfprintf_r+0xa5c>
60305e68:	e3560010 	cmp	r6, #16
60305e6c:	e59d2080 	ldr	r2, [sp, #128]	; 0x80
60305e70:	e2833001 	add	r3, r3, #1
60305e74:	e5840000 	str	r0, [r4]
60305e78:	caffffe8 	bgt	60305e20 <_vfprintf_r+0x1498>
60305e7c:	e5846004 	str	r6, [r4, #4]
60305e80:	e0866002 	add	r6, r6, r2
60305e84:	e58d6080 	str	r6, [sp, #128]	; 0x80
60305e88:	e3530007 	cmp	r3, #7
60305e8c:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60305e90:	d1a04001 	movle	r4, r1
60305e94:	daffffc8 	ble	60305dbc <_vfprintf_r+0x1434>
60305e98:	eafffe3a 	b	60305788 <_vfprintf_r+0xe00>
60305e9c:	e59d3060 	ldr	r3, [sp, #96]	; 0x60
60305ea0:	e3530000 	cmp	r3, #0
60305ea4:	ca00005f 	bgt	60306028 <_vfprintf_r+0x16a0>
60305ea8:	e51f3ad0 	ldr	r3, [pc, #-2768]	; 603053e0 <_vfprintf_r+0xa58>
60305eac:	e5843000 	str	r3, [r4]
60305eb0:	e3a03001 	mov	r3, #1
60305eb4:	e0800003 	add	r0, r0, r3
60305eb8:	e5843004 	str	r3, [r4, #4]
60305ebc:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
60305ec0:	e58d0080 	str	r0, [sp, #128]	; 0x80
60305ec4:	e2833001 	add	r3, r3, #1
60305ec8:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60305ecc:	e3530007 	cmp	r3, #7
60305ed0:	d2844008 	addle	r4, r4, #8
60305ed4:	da000006 	ble	60305ef4 <_vfprintf_r+0x156c>
60305ed8:	e28d2078 	add	r2, sp, #120	; 0x78
60305edc:	e1a01005 	mov	r1, r5
60305ee0:	e1a00007 	mov	r0, r7
60305ee4:	ebfff691 	bl	60303930 <__sprint_r>
60305ee8:	e3500000 	cmp	r0, #0
60305eec:	1a0001f7 	bne	603066d0 <_vfprintf_r+0x1d48>
60305ef0:	e28d4084 	add	r4, sp, #132	; 0x84
60305ef4:	e59d100c 	ldr	r1, [sp, #12]
60305ef8:	e59d2060 	ldr	r2, [sp, #96]	; 0x60
60305efc:	e59d3080 	ldr	r3, [sp, #128]	; 0x80
60305f00:	e1812002 	orr	r2, r1, r2
60305f04:	e20b1001 	and	r1, fp, #1
60305f08:	e1922001 	orrs	r2, r2, r1
60305f0c:	0affffaa 	beq	60305dbc <_vfprintf_r+0x1434>
60305f10:	e59d2040 	ldr	r2, [sp, #64]	; 0x40
60305f14:	e5842000 	str	r2, [r4]
60305f18:	e59d2028 	ldr	r2, [sp, #40]	; 0x28
60305f1c:	e5842004 	str	r2, [r4, #4]
60305f20:	e0823003 	add	r3, r2, r3
60305f24:	e58d3080 	str	r3, [sp, #128]	; 0x80
60305f28:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
60305f2c:	e2833001 	add	r3, r3, #1
60305f30:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60305f34:	e3530007 	cmp	r3, #7
60305f38:	d2844008 	addle	r4, r4, #8
60305f3c:	da000006 	ble	60305f5c <_vfprintf_r+0x15d4>
60305f40:	e28d2078 	add	r2, sp, #120	; 0x78
60305f44:	e1a01005 	mov	r1, r5
60305f48:	e1a00007 	mov	r0, r7
60305f4c:	ebfff677 	bl	60303930 <__sprint_r>
60305f50:	e3500000 	cmp	r0, #0
60305f54:	1a0001dd 	bne	603066d0 <_vfprintf_r+0x1d48>
60305f58:	e28d4084 	add	r4, sp, #132	; 0x84
60305f5c:	e59d6060 	ldr	r6, [sp, #96]	; 0x60
60305f60:	e3560000 	cmp	r6, #0
60305f64:	b2666000 	rsblt	r6, r6, #0
60305f68:	b1a02004 	movlt	r2, r4
60305f6c:	b3a08010 	movlt	r8, #16
60305f70:	ba000016 	blt	60305fd0 <_vfprintf_r+0x1648>
60305f74:	e59d300c 	ldr	r3, [sp, #12]
60305f78:	e59d200c 	ldr	r2, [sp, #12]
60305f7c:	e5843004 	str	r3, [r4, #4]
60305f80:	e59d3080 	ldr	r3, [sp, #128]	; 0x80
60305f84:	e584a000 	str	sl, [r4]
60305f88:	e0833002 	add	r3, r3, r2
60305f8c:	e58d3080 	str	r3, [sp, #128]	; 0x80
60305f90:	eafffdf6 	b	60305770 <_vfprintf_r+0xde8>
60305f94:	e2811010 	add	r1, r1, #16
60305f98:	e3530007 	cmp	r3, #7
60305f9c:	e5828004 	str	r8, [r2, #4]
60305fa0:	e58d1080 	str	r1, [sp, #128]	; 0x80
60305fa4:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60305fa8:	da000006 	ble	60305fc8 <_vfprintf_r+0x1640>
60305fac:	e28d2078 	add	r2, sp, #120	; 0x78
60305fb0:	e1a01005 	mov	r1, r5
60305fb4:	e1a00007 	mov	r0, r7
60305fb8:	ebfff65c 	bl	60303930 <__sprint_r>
60305fbc:	e3500000 	cmp	r0, #0
60305fc0:	1a0001c2 	bne	603066d0 <_vfprintf_r+0x1d48>
60305fc4:	e28d4084 	add	r4, sp, #132	; 0x84
60305fc8:	e2466010 	sub	r6, r6, #16
60305fcc:	e1a02004 	mov	r2, r4
60305fd0:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
60305fd4:	e2844008 	add	r4, r4, #8
60305fd8:	e51f0bfc 	ldr	r0, [pc, #-3068]	; 603053e4 <_vfprintf_r+0xa5c>
60305fdc:	e3560010 	cmp	r6, #16
60305fe0:	e59d1080 	ldr	r1, [sp, #128]	; 0x80
60305fe4:	e2833001 	add	r3, r3, #1
60305fe8:	e5820000 	str	r0, [r2]
60305fec:	caffffe8 	bgt	60305f94 <_vfprintf_r+0x160c>
60305ff0:	e5826004 	str	r6, [r2, #4]
60305ff4:	e3530007 	cmp	r3, #7
60305ff8:	e0866001 	add	r6, r6, r1
60305ffc:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60306000:	e58d6080 	str	r6, [sp, #128]	; 0x80
60306004:	daffffda 	ble	60305f74 <_vfprintf_r+0x15ec>
60306008:	e28d2078 	add	r2, sp, #120	; 0x78
6030600c:	e1a01005 	mov	r1, r5
60306010:	e1a00007 	mov	r0, r7
60306014:	ebfff645 	bl	60303930 <__sprint_r>
60306018:	e3500000 	cmp	r0, #0
6030601c:	1a0001ab 	bne	603066d0 <_vfprintf_r+0x1d48>
60306020:	e28d4084 	add	r4, sp, #132	; 0x84
60306024:	eaffffd2 	b	60305f74 <_vfprintf_r+0x15ec>
60306028:	e59d300c 	ldr	r3, [sp, #12]
6030602c:	e1580003 	cmp	r8, r3
60306030:	b1a09008 	movlt	r9, r8
60306034:	a1a09003 	movge	r9, r3
60306038:	e3590000 	cmp	r9, #0
6030603c:	da000010 	ble	60306084 <_vfprintf_r+0x16fc>
60306040:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
60306044:	e0890000 	add	r0, r9, r0
60306048:	e584a000 	str	sl, [r4]
6030604c:	e2833001 	add	r3, r3, #1
60306050:	e5849004 	str	r9, [r4, #4]
60306054:	e3530007 	cmp	r3, #7
60306058:	e58d0080 	str	r0, [sp, #128]	; 0x80
6030605c:	d2844008 	addle	r4, r4, #8
60306060:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60306064:	da000006 	ble	60306084 <_vfprintf_r+0x16fc>
60306068:	e28d2078 	add	r2, sp, #120	; 0x78
6030606c:	e1a01005 	mov	r1, r5
60306070:	e1a00007 	mov	r0, r7
60306074:	ebfff62d 	bl	60303930 <__sprint_r>
60306078:	e3500000 	cmp	r0, #0
6030607c:	1a000193 	bne	603066d0 <_vfprintf_r+0x1d48>
60306080:	e28d4084 	add	r4, sp, #132	; 0x84
60306084:	e3590000 	cmp	r9, #0
60306088:	a0486009 	subge	r6, r8, r9
6030608c:	b1a06008 	movlt	r6, r8
60306090:	e3560000 	cmp	r6, #0
60306094:	c3a09010 	movgt	r9, #16
60306098:	ca000063 	bgt	6030622c <_vfprintf_r+0x18a4>
6030609c:	e08a8008 	add	r8, sl, r8
603060a0:	e31b0b01 	tst	fp, #1024	; 0x400
603060a4:	0a000007 	beq	603060c8 <_vfprintf_r+0x1740>
603060a8:	e1cd21d8 	ldrd	r2, [sp, #24]
603060ac:	e3520000 	cmp	r2, #0
603060b0:	d3530000 	cmple	r3, #0
603060b4:	ca000073 	bgt	60306288 <_vfprintf_r+0x1900>
603060b8:	e59d300c 	ldr	r3, [sp, #12]
603060bc:	e08a3003 	add	r3, sl, r3
603060c0:	e1580003 	cmp	r8, r3
603060c4:	21a08003 	movcs	r8, r3
603060c8:	e59d3060 	ldr	r3, [sp, #96]	; 0x60
603060cc:	e59d200c 	ldr	r2, [sp, #12]
603060d0:	e1530002 	cmp	r3, r2
603060d4:	ba000001 	blt	603060e0 <_vfprintf_r+0x1758>
603060d8:	e31b0001 	tst	fp, #1
603060dc:	0a000014 	beq	60306134 <_vfprintf_r+0x17ac>
603060e0:	e59d3040 	ldr	r3, [sp, #64]	; 0x40
603060e4:	e5843000 	str	r3, [r4]
603060e8:	e59d3028 	ldr	r3, [sp, #40]	; 0x28
603060ec:	e5843004 	str	r3, [r4, #4]
603060f0:	e59d2028 	ldr	r2, [sp, #40]	; 0x28
603060f4:	e59d3080 	ldr	r3, [sp, #128]	; 0x80
603060f8:	e0833002 	add	r3, r3, r2
603060fc:	e58d3080 	str	r3, [sp, #128]	; 0x80
60306100:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
60306104:	e2833001 	add	r3, r3, #1
60306108:	e58d307c 	str	r3, [sp, #124]	; 0x7c
6030610c:	e3530007 	cmp	r3, #7
60306110:	d2844008 	addle	r4, r4, #8
60306114:	da000006 	ble	60306134 <_vfprintf_r+0x17ac>
60306118:	e28d2078 	add	r2, sp, #120	; 0x78
6030611c:	e1a01005 	mov	r1, r5
60306120:	e1a00007 	mov	r0, r7
60306124:	ebfff601 	bl	60303930 <__sprint_r>
60306128:	e3500000 	cmp	r0, #0
6030612c:	1a000167 	bne	603066d0 <_vfprintf_r+0x1d48>
60306130:	e28d4084 	add	r4, sp, #132	; 0x84
60306134:	e59d300c 	ldr	r3, [sp, #12]
60306138:	e59d6060 	ldr	r6, [sp, #96]	; 0x60
6030613c:	e08a2003 	add	r2, sl, r3
60306140:	e0436006 	sub	r6, r3, r6
60306144:	e0422008 	sub	r2, r2, r8
60306148:	e1560002 	cmp	r6, r2
6030614c:	a1a06002 	movge	r6, r2
60306150:	e3560000 	cmp	r6, #0
60306154:	da000011 	ble	603061a0 <_vfprintf_r+0x1818>
60306158:	e59d3080 	ldr	r3, [sp, #128]	; 0x80
6030615c:	e5848000 	str	r8, [r4]
60306160:	e0863003 	add	r3, r6, r3
60306164:	e58d3080 	str	r3, [sp, #128]	; 0x80
60306168:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
6030616c:	e5846004 	str	r6, [r4, #4]
60306170:	e2833001 	add	r3, r3, #1
60306174:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60306178:	e3530007 	cmp	r3, #7
6030617c:	d2844008 	addle	r4, r4, #8
60306180:	da000006 	ble	603061a0 <_vfprintf_r+0x1818>
60306184:	e28d2078 	add	r2, sp, #120	; 0x78
60306188:	e1a01005 	mov	r1, r5
6030618c:	e1a00007 	mov	r0, r7
60306190:	ebfff5e6 	bl	60303930 <__sprint_r>
60306194:	e3500000 	cmp	r0, #0
60306198:	1a00014c 	bne	603066d0 <_vfprintf_r+0x1d48>
6030619c:	e28d4084 	add	r4, sp, #132	; 0x84
603061a0:	e59d8060 	ldr	r8, [sp, #96]	; 0x60
603061a4:	e3560000 	cmp	r6, #0
603061a8:	e59d300c 	ldr	r3, [sp, #12]
603061ac:	e0438008 	sub	r8, r3, r8
603061b0:	a0488006 	subge	r8, r8, r6
603061b4:	e3580000 	cmp	r8, #0
603061b8:	c3a06010 	movgt	r6, #16
603061bc:	dafffefe 	ble	60305dbc <_vfprintf_r+0x1434>
603061c0:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
603061c4:	e2841008 	add	r1, r4, #8
603061c8:	e51f0dec 	ldr	r0, [pc, #-3564]	; 603053e4 <_vfprintf_r+0xa5c>
603061cc:	e3580010 	cmp	r8, #16
603061d0:	e59d2080 	ldr	r2, [sp, #128]	; 0x80
603061d4:	e2833001 	add	r3, r3, #1
603061d8:	e5840000 	str	r0, [r4]
603061dc:	ca000093 	bgt	60306430 <_vfprintf_r+0x1aa8>
603061e0:	e5848004 	str	r8, [r4, #4]
603061e4:	e0888002 	add	r8, r8, r2
603061e8:	e58d8080 	str	r8, [sp, #128]	; 0x80
603061ec:	eaffff25 	b	60305e88 <_vfprintf_r+0x1500>
603061f0:	e2822010 	add	r2, r2, #16
603061f4:	e3530007 	cmp	r3, #7
603061f8:	e5849004 	str	r9, [r4, #4]
603061fc:	e58d2080 	str	r2, [sp, #128]	; 0x80
60306200:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60306204:	da000006 	ble	60306224 <_vfprintf_r+0x189c>
60306208:	e28d2078 	add	r2, sp, #120	; 0x78
6030620c:	e1a01005 	mov	r1, r5
60306210:	e1a00007 	mov	r0, r7
60306214:	ebfff5c5 	bl	60303930 <__sprint_r>
60306218:	e3500000 	cmp	r0, #0
6030621c:	1a00012b 	bne	603066d0 <_vfprintf_r+0x1d48>
60306220:	e28d1084 	add	r1, sp, #132	; 0x84
60306224:	e2466010 	sub	r6, r6, #16
60306228:	e1a04001 	mov	r4, r1
6030622c:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
60306230:	e2841008 	add	r1, r4, #8
60306234:	e51f0e58 	ldr	r0, [pc, #-3672]	; 603053e4 <_vfprintf_r+0xa5c>
60306238:	e3560010 	cmp	r6, #16
6030623c:	e59d2080 	ldr	r2, [sp, #128]	; 0x80
60306240:	e2833001 	add	r3, r3, #1
60306244:	e5840000 	str	r0, [r4]
60306248:	caffffe8 	bgt	603061f0 <_vfprintf_r+0x1868>
6030624c:	e3530007 	cmp	r3, #7
60306250:	e5846004 	str	r6, [r4, #4]
60306254:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60306258:	e0866002 	add	r6, r6, r2
6030625c:	d1a04001 	movle	r4, r1
60306260:	e58d6080 	str	r6, [sp, #128]	; 0x80
60306264:	daffff8c 	ble	6030609c <_vfprintf_r+0x1714>
60306268:	e28d2078 	add	r2, sp, #120	; 0x78
6030626c:	e1a01005 	mov	r1, r5
60306270:	e1a00007 	mov	r0, r7
60306274:	ebfff5ad 	bl	60303930 <__sprint_r>
60306278:	e3500000 	cmp	r0, #0
6030627c:	1a000113 	bne	603066d0 <_vfprintf_r+0x1d48>
60306280:	e28d4084 	add	r4, sp, #132	; 0x84
60306284:	eaffff84 	b	6030609c <_vfprintf_r+0x1714>
60306288:	e59d3018 	ldr	r3, [sp, #24]
6030628c:	e59d2038 	ldr	r2, [sp, #56]	; 0x38
60306290:	e3530000 	cmp	r3, #0
60306294:	12433001 	subne	r3, r3, #1
60306298:	158d3018 	strne	r3, [sp, #24]
6030629c:	059d3020 	ldreq	r3, [sp, #32]
603062a0:	02433001 	subeq	r3, r3, #1
603062a4:	058d3020 	streq	r3, [sp, #32]
603062a8:	059d301c 	ldreq	r3, [sp, #28]
603062ac:	02433001 	subeq	r3, r3, #1
603062b0:	058d301c 	streq	r3, [sp, #28]
603062b4:	e59d3044 	ldr	r3, [sp, #68]	; 0x44
603062b8:	e5843000 	str	r3, [r4]
603062bc:	e59d3038 	ldr	r3, [sp, #56]	; 0x38
603062c0:	e5843004 	str	r3, [r4, #4]
603062c4:	e59d3080 	ldr	r3, [sp, #128]	; 0x80
603062c8:	e0833002 	add	r3, r3, r2
603062cc:	e58d3080 	str	r3, [sp, #128]	; 0x80
603062d0:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
603062d4:	e2833001 	add	r3, r3, #1
603062d8:	e58d307c 	str	r3, [sp, #124]	; 0x7c
603062dc:	e3530007 	cmp	r3, #7
603062e0:	d2844008 	addle	r4, r4, #8
603062e4:	da000006 	ble	60306304 <_vfprintf_r+0x197c>
603062e8:	e28d2078 	add	r2, sp, #120	; 0x78
603062ec:	e1a01005 	mov	r1, r5
603062f0:	e1a00007 	mov	r0, r7
603062f4:	ebfff58d 	bl	60303930 <__sprint_r>
603062f8:	e3500000 	cmp	r0, #0
603062fc:	1a0000f3 	bne	603066d0 <_vfprintf_r+0x1d48>
60306300:	e28d4084 	add	r4, sp, #132	; 0x84
60306304:	e59d300c 	ldr	r3, [sp, #12]
60306308:	e59d2020 	ldr	r2, [sp, #32]
6030630c:	e08a9003 	add	r9, sl, r3
60306310:	e0493008 	sub	r3, r9, r8
60306314:	e5d29000 	ldrb	r9, [r2]
60306318:	e1590003 	cmp	r9, r3
6030631c:	a1a09003 	movge	r9, r3
60306320:	e3590000 	cmp	r9, #0
60306324:	da000010 	ble	6030636c <_vfprintf_r+0x19e4>
60306328:	e59d3080 	ldr	r3, [sp, #128]	; 0x80
6030632c:	e1c480f0 	strd	r8, [r4]
60306330:	e0893003 	add	r3, r9, r3
60306334:	e58d3080 	str	r3, [sp, #128]	; 0x80
60306338:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
6030633c:	e2833001 	add	r3, r3, #1
60306340:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60306344:	e3530007 	cmp	r3, #7
60306348:	d2844008 	addle	r4, r4, #8
6030634c:	da000006 	ble	6030636c <_vfprintf_r+0x19e4>
60306350:	e28d2078 	add	r2, sp, #120	; 0x78
60306354:	e1a01005 	mov	r1, r5
60306358:	e1a00007 	mov	r0, r7
6030635c:	ebfff573 	bl	60303930 <__sprint_r>
60306360:	e3500000 	cmp	r0, #0
60306364:	1a0000d9 	bne	603066d0 <_vfprintf_r+0x1d48>
60306368:	e28d4084 	add	r4, sp, #132	; 0x84
6030636c:	e59d3020 	ldr	r3, [sp, #32]
60306370:	e3590000 	cmp	r9, #0
60306374:	e5d36000 	ldrb	r6, [r3]
60306378:	a0466009 	subge	r6, r6, r9
6030637c:	e3560000 	cmp	r6, #0
60306380:	da000015 	ble	603063dc <_vfprintf_r+0x1a54>
60306384:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
60306388:	e2841008 	add	r1, r4, #8
6030638c:	e51f0fb0 	ldr	r0, [pc, #-4016]	; 603053e4 <_vfprintf_r+0xa5c>
60306390:	e3560010 	cmp	r6, #16
60306394:	e59d2080 	ldr	r2, [sp, #128]	; 0x80
60306398:	e2833001 	add	r3, r3, #1
6030639c:	e5840000 	str	r0, [r4]
603063a0:	ca000011 	bgt	603063ec <_vfprintf_r+0x1a64>
603063a4:	e3530007 	cmp	r3, #7
603063a8:	e5846004 	str	r6, [r4, #4]
603063ac:	e58d307c 	str	r3, [sp, #124]	; 0x7c
603063b0:	e0866002 	add	r6, r6, r2
603063b4:	d1a04001 	movle	r4, r1
603063b8:	e58d6080 	str	r6, [sp, #128]	; 0x80
603063bc:	da000006 	ble	603063dc <_vfprintf_r+0x1a54>
603063c0:	e28d2078 	add	r2, sp, #120	; 0x78
603063c4:	e1a01005 	mov	r1, r5
603063c8:	e1a00007 	mov	r0, r7
603063cc:	ebfff557 	bl	60303930 <__sprint_r>
603063d0:	e3500000 	cmp	r0, #0
603063d4:	1a0000bd 	bne	603066d0 <_vfprintf_r+0x1d48>
603063d8:	e28d4084 	add	r4, sp, #132	; 0x84
603063dc:	e59d3020 	ldr	r3, [sp, #32]
603063e0:	e5d33000 	ldrb	r3, [r3]
603063e4:	e0888003 	add	r8, r8, r3
603063e8:	eaffff2e 	b	603060a8 <_vfprintf_r+0x1720>
603063ec:	e3a00010 	mov	r0, #16
603063f0:	e3530007 	cmp	r3, #7
603063f4:	e0822000 	add	r2, r2, r0
603063f8:	e5840004 	str	r0, [r4, #4]
603063fc:	e58d2080 	str	r2, [sp, #128]	; 0x80
60306400:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60306404:	da000006 	ble	60306424 <_vfprintf_r+0x1a9c>
60306408:	e28d2078 	add	r2, sp, #120	; 0x78
6030640c:	e1a01005 	mov	r1, r5
60306410:	e1a00007 	mov	r0, r7
60306414:	ebfff545 	bl	60303930 <__sprint_r>
60306418:	e3500000 	cmp	r0, #0
6030641c:	1a0000ab 	bne	603066d0 <_vfprintf_r+0x1d48>
60306420:	e28d1084 	add	r1, sp, #132	; 0x84
60306424:	e2466010 	sub	r6, r6, #16
60306428:	e1a04001 	mov	r4, r1
6030642c:	eaffffd4 	b	60306384 <_vfprintf_r+0x19fc>
60306430:	e2822010 	add	r2, r2, #16
60306434:	e3530007 	cmp	r3, #7
60306438:	e5846004 	str	r6, [r4, #4]
6030643c:	e58d2080 	str	r2, [sp, #128]	; 0x80
60306440:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60306444:	da000006 	ble	60306464 <_vfprintf_r+0x1adc>
60306448:	e28d2078 	add	r2, sp, #120	; 0x78
6030644c:	e1a01005 	mov	r1, r5
60306450:	e1a00007 	mov	r0, r7
60306454:	ebfff535 	bl	60303930 <__sprint_r>
60306458:	e3500000 	cmp	r0, #0
6030645c:	1a00009b 	bne	603066d0 <_vfprintf_r+0x1d48>
60306460:	e28d1084 	add	r1, sp, #132	; 0x84
60306464:	e2488010 	sub	r8, r8, #16
60306468:	e1a04001 	mov	r4, r1
6030646c:	eaffff53 	b	603061c0 <_vfprintf_r+0x1838>
60306470:	e59d207c 	ldr	r2, [sp, #124]	; 0x7c
60306474:	e2800001 	add	r0, r0, #1
60306478:	e59d100c 	ldr	r1, [sp, #12]
6030647c:	e2843008 	add	r3, r4, #8
60306480:	e2822001 	add	r2, r2, #1
60306484:	e3510001 	cmp	r1, #1
60306488:	ca000001 	bgt	60306494 <_vfprintf_r+0x1b0c>
6030648c:	e31b0001 	tst	fp, #1
60306490:	0a00006d 	beq	6030664c <_vfprintf_r+0x1cc4>
60306494:	e3a01001 	mov	r1, #1
60306498:	e3520007 	cmp	r2, #7
6030649c:	e584a000 	str	sl, [r4]
603064a0:	e5841004 	str	r1, [r4, #4]
603064a4:	e58d0080 	str	r0, [sp, #128]	; 0x80
603064a8:	e58d207c 	str	r2, [sp, #124]	; 0x7c
603064ac:	da000006 	ble	603064cc <_vfprintf_r+0x1b44>
603064b0:	e28d2078 	add	r2, sp, #120	; 0x78
603064b4:	e1a01005 	mov	r1, r5
603064b8:	e1a00007 	mov	r0, r7
603064bc:	ebfff51b 	bl	60303930 <__sprint_r>
603064c0:	e3500000 	cmp	r0, #0
603064c4:	1a000081 	bne	603066d0 <_vfprintf_r+0x1d48>
603064c8:	e28d3084 	add	r3, sp, #132	; 0x84
603064cc:	e59d2040 	ldr	r2, [sp, #64]	; 0x40
603064d0:	e5832000 	str	r2, [r3]
603064d4:	e59d2028 	ldr	r2, [sp, #40]	; 0x28
603064d8:	e5832004 	str	r2, [r3, #4]
603064dc:	e59d1028 	ldr	r1, [sp, #40]	; 0x28
603064e0:	e59d2080 	ldr	r2, [sp, #128]	; 0x80
603064e4:	e0822001 	add	r2, r2, r1
603064e8:	e58d2080 	str	r2, [sp, #128]	; 0x80
603064ec:	e59d207c 	ldr	r2, [sp, #124]	; 0x7c
603064f0:	e2822001 	add	r2, r2, #1
603064f4:	e58d207c 	str	r2, [sp, #124]	; 0x7c
603064f8:	e3520007 	cmp	r2, #7
603064fc:	d2833008 	addle	r3, r3, #8
60306500:	da000006 	ble	60306520 <_vfprintf_r+0x1b98>
60306504:	e28d2078 	add	r2, sp, #120	; 0x78
60306508:	e1a01005 	mov	r1, r5
6030650c:	e1a00007 	mov	r0, r7
60306510:	ebfff506 	bl	60303930 <__sprint_r>
60306514:	e3500000 	cmp	r0, #0
60306518:	1a00006c 	bne	603066d0 <_vfprintf_r+0x1d48>
6030651c:	e28d3084 	add	r3, sp, #132	; 0x84
60306520:	eeb58b40 	vcmp.f64	d8, #0.0
60306524:	e59d200c 	ldr	r2, [sp, #12]
60306528:	e2424001 	sub	r4, r2, #1
6030652c:	eef1fa10 	vmrs	APSR_nzcv, fpscr
60306530:	0a000022 	beq	603065c0 <_vfprintf_r+0x1c38>
60306534:	e59d107c 	ldr	r1, [sp, #124]	; 0x7c
60306538:	e28a0001 	add	r0, sl, #1
6030653c:	e59d2080 	ldr	r2, [sp, #128]	; 0x80
60306540:	e2811001 	add	r1, r1, #1
60306544:	e8830011 	stm	r3, {r0, r4}
60306548:	e59d000c 	ldr	r0, [sp, #12]
6030654c:	e2422001 	sub	r2, r2, #1
60306550:	e3510007 	cmp	r1, #7
60306554:	e58d107c 	str	r1, [sp, #124]	; 0x7c
60306558:	e0822000 	add	r2, r2, r0
6030655c:	d2833008 	addle	r3, r3, #8
60306560:	e58d2080 	str	r2, [sp, #128]	; 0x80
60306564:	da000006 	ble	60306584 <_vfprintf_r+0x1bfc>
60306568:	e28d2078 	add	r2, sp, #120	; 0x78
6030656c:	e1a01005 	mov	r1, r5
60306570:	e1a00007 	mov	r0, r7
60306574:	ebfff4ed 	bl	60303930 <__sprint_r>
60306578:	e3500000 	cmp	r0, #0
6030657c:	1a000053 	bne	603066d0 <_vfprintf_r+0x1d48>
60306580:	e28d3084 	add	r3, sp, #132	; 0x84
60306584:	e28d2068 	add	r2, sp, #104	; 0x68
60306588:	e5832000 	str	r2, [r3]
6030658c:	e59d2048 	ldr	r2, [sp, #72]	; 0x48
60306590:	e59d1048 	ldr	r1, [sp, #72]	; 0x48
60306594:	e5832004 	str	r2, [r3, #4]
60306598:	e59d2080 	ldr	r2, [sp, #128]	; 0x80
6030659c:	e0822001 	add	r2, r2, r1
603065a0:	e58d2080 	str	r2, [sp, #128]	; 0x80
603065a4:	e59d207c 	ldr	r2, [sp, #124]	; 0x7c
603065a8:	e2822001 	add	r2, r2, #1
603065ac:	e58d207c 	str	r2, [sp, #124]	; 0x7c
603065b0:	e3520007 	cmp	r2, #7
603065b4:	d2834008 	addle	r4, r3, #8
603065b8:	dafffdff 	ble	60305dbc <_vfprintf_r+0x1434>
603065bc:	eafffc71 	b	60305788 <_vfprintf_r+0xe00>
603065c0:	e3540000 	cmp	r4, #0
603065c4:	c3a06010 	movgt	r6, #16
603065c8:	daffffed 	ble	60306584 <_vfprintf_r+0x1bfc>
603065cc:	e59d207c 	ldr	r2, [sp, #124]	; 0x7c
603065d0:	e2830008 	add	r0, r3, #8
603065d4:	e59fc2b4 	ldr	ip, [pc, #692]	; 60306890 <_vfprintf_r+0x1f08>
603065d8:	e3540010 	cmp	r4, #16
603065dc:	e59d1080 	ldr	r1, [sp, #128]	; 0x80
603065e0:	e2822001 	add	r2, r2, #1
603065e4:	e583c000 	str	ip, [r3]
603065e8:	ca000007 	bgt	6030660c <_vfprintf_r+0x1c84>
603065ec:	e3520007 	cmp	r2, #7
603065f0:	e5834004 	str	r4, [r3, #4]
603065f4:	e58d207c 	str	r2, [sp, #124]	; 0x7c
603065f8:	e0844001 	add	r4, r4, r1
603065fc:	d1a03000 	movle	r3, r0
60306600:	e58d4080 	str	r4, [sp, #128]	; 0x80
60306604:	daffffde 	ble	60306584 <_vfprintf_r+0x1bfc>
60306608:	eaffffd6 	b	60306568 <_vfprintf_r+0x1be0>
6030660c:	e2811010 	add	r1, r1, #16
60306610:	e3520007 	cmp	r2, #7
60306614:	e5836004 	str	r6, [r3, #4]
60306618:	e58d1080 	str	r1, [sp, #128]	; 0x80
6030661c:	e58d207c 	str	r2, [sp, #124]	; 0x7c
60306620:	da000006 	ble	60306640 <_vfprintf_r+0x1cb8>
60306624:	e28d2078 	add	r2, sp, #120	; 0x78
60306628:	e1a01005 	mov	r1, r5
6030662c:	e1a00007 	mov	r0, r7
60306630:	ebfff4be 	bl	60303930 <__sprint_r>
60306634:	e3500000 	cmp	r0, #0
60306638:	1a000024 	bne	603066d0 <_vfprintf_r+0x1d48>
6030663c:	e28d0084 	add	r0, sp, #132	; 0x84
60306640:	e2444010 	sub	r4, r4, #16
60306644:	e1a03000 	mov	r3, r0
60306648:	eaffffdf 	b	603065cc <_vfprintf_r+0x1c44>
6030664c:	e3a01001 	mov	r1, #1
60306650:	e3520007 	cmp	r2, #7
60306654:	e584a000 	str	sl, [r4]
60306658:	e5841004 	str	r1, [r4, #4]
6030665c:	e58d0080 	str	r0, [sp, #128]	; 0x80
60306660:	e58d207c 	str	r2, [sp, #124]	; 0x7c
60306664:	daffffc6 	ble	60306584 <_vfprintf_r+0x1bfc>
60306668:	eaffffbe 	b	60306568 <_vfprintf_r+0x1be0>
6030666c:	e59d3030 	ldr	r3, [sp, #48]	; 0x30
60306670:	e59d2034 	ldr	r2, [sp, #52]	; 0x34
60306674:	e0436002 	sub	r6, r3, r2
60306678:	e3560000 	cmp	r6, #0
6030667c:	c3a09010 	movgt	r9, #16
60306680:	c59f820c 	ldrgt	r8, [pc, #524]	; 60306894 <_vfprintf_r+0x1f0c>
60306684:	dafffdce 	ble	60305dc4 <_vfprintf_r+0x143c>
60306688:	e59d307c 	ldr	r3, [sp, #124]	; 0x7c
6030668c:	e3560010 	cmp	r6, #16
60306690:	e59d2080 	ldr	r2, [sp, #128]	; 0x80
60306694:	e2833001 	add	r3, r3, #1
60306698:	e5848000 	str	r8, [r4]
6030669c:	ca000012 	bgt	603066ec <_vfprintf_r+0x1d64>
603066a0:	e5846004 	str	r6, [r4, #4]
603066a4:	e3530007 	cmp	r3, #7
603066a8:	e0866002 	add	r6, r6, r2
603066ac:	e58d307c 	str	r3, [sp, #124]	; 0x7c
603066b0:	e58d6080 	str	r6, [sp, #128]	; 0x80
603066b4:	dafffdc2 	ble	60305dc4 <_vfprintf_r+0x143c>
603066b8:	e28d2078 	add	r2, sp, #120	; 0x78
603066bc:	e1a01005 	mov	r1, r5
603066c0:	e1a00007 	mov	r0, r7
603066c4:	ebfff499 	bl	60303930 <__sprint_r>
603066c8:	e3500000 	cmp	r0, #0
603066cc:	0afffdbc 	beq	60305dc4 <_vfprintf_r+0x143c>
603066d0:	e59d3008 	ldr	r3, [sp, #8]
603066d4:	e3530000 	cmp	r3, #0
603066d8:	0afffa86 	beq	603050f8 <_vfprintf_r+0x770>
603066dc:	e1a01003 	mov	r1, r3
603066e0:	e1a00007 	mov	r0, r7
603066e4:	eb001bc5 	bl	6030d600 <__wrap__free_r>
603066e8:	eafffa82 	b	603050f8 <_vfprintf_r+0x770>
603066ec:	e3530007 	cmp	r3, #7
603066f0:	e2822010 	add	r2, r2, #16
603066f4:	e5849004 	str	r9, [r4, #4]
603066f8:	d2844008 	addle	r4, r4, #8
603066fc:	e58d2080 	str	r2, [sp, #128]	; 0x80
60306700:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60306704:	da000006 	ble	60306724 <_vfprintf_r+0x1d9c>
60306708:	e28d2078 	add	r2, sp, #120	; 0x78
6030670c:	e1a01005 	mov	r1, r5
60306710:	e1a00007 	mov	r0, r7
60306714:	ebfff485 	bl	60303930 <__sprint_r>
60306718:	e3500000 	cmp	r0, #0
6030671c:	1affffeb 	bne	603066d0 <_vfprintf_r+0x1d48>
60306720:	e28d4084 	add	r4, sp, #132	; 0x84
60306724:	e2466010 	sub	r6, r6, #16
60306728:	eaffffd6 	b	60306688 <_vfprintf_r+0x1d00>
6030672c:	e59d1008 	ldr	r1, [sp, #8]
60306730:	e1a00007 	mov	r0, r7
60306734:	eb001bb1 	bl	6030d600 <__wrap__free_r>
60306738:	eafffdb6 	b	60305e18 <_vfprintf_r+0x1490>
6030673c:	e59d3080 	ldr	r3, [sp, #128]	; 0x80
60306740:	e3530000 	cmp	r3, #0
60306744:	1a000002 	bne	60306754 <_vfprintf_r+0x1dcc>
60306748:	e3a03000 	mov	r3, #0
6030674c:	e58d307c 	str	r3, [sp, #124]	; 0x7c
60306750:	eafffa68 	b	603050f8 <_vfprintf_r+0x770>
60306754:	e28d2078 	add	r2, sp, #120	; 0x78
60306758:	e1a01005 	mov	r1, r5
6030675c:	e1a00007 	mov	r0, r7
60306760:	ebfff472 	bl	60303930 <__sprint_r>
60306764:	e3500000 	cmp	r0, #0
60306768:	0afffff6 	beq	60306748 <_vfprintf_r+0x1dc0>
6030676c:	eafffa61 	b	603050f8 <_vfprintf_r+0x770>
60306770:	e3760001 	cmn	r6, #1
60306774:	11a0100b 	movne	r1, fp
60306778:	13a03001 	movne	r3, #1
6030677c:	1afffcf6 	bne	60305b5c <_vfprintf_r+0x11d4>
60306780:	e358000a 	cmp	r8, #10
60306784:	e2d93000 	sbcs	r3, r9, #0
60306788:	32888030 	addcc	r8, r8, #48	; 0x30
6030678c:	35cd8127 	strbcc	r8, [sp, #295]	; 0x127
60306790:	3afffcff 	bcc	60305b94 <_vfprintf_r+0x120c>
60306794:	e3a03000 	mov	r3, #0
60306798:	e58d300c 	str	r3, [sp, #12]
6030679c:	e28d3f4a 	add	r3, sp, #296	; 0x128
603067a0:	e58d3008 	str	r3, [sp, #8]
603067a4:	e20b3b01 	and	r3, fp, #1024	; 0x400
603067a8:	e58d3018 	str	r3, [sp, #24]
603067ac:	e59d3008 	ldr	r3, [sp, #8]
603067b0:	e3a0200a 	mov	r2, #10
603067b4:	e1a00008 	mov	r0, r8
603067b8:	e1a01009 	mov	r1, r9
603067bc:	e243a001 	sub	sl, r3, #1
603067c0:	e3a03000 	mov	r3, #0
603067c4:	ebffe842 	bl	603008d4 <__aeabi_uldivmod>
603067c8:	e59d3008 	ldr	r3, [sp, #8]
603067cc:	e2822030 	add	r2, r2, #48	; 0x30
603067d0:	e5432001 	strb	r2, [r3, #-1]
603067d4:	e59d300c 	ldr	r3, [sp, #12]
603067d8:	e2833001 	add	r3, r3, #1
603067dc:	e58d300c 	str	r3, [sp, #12]
603067e0:	e59d3018 	ldr	r3, [sp, #24]
603067e4:	e3530000 	cmp	r3, #0
603067e8:	0a00001c 	beq	60306860 <_vfprintf_r+0x1ed8>
603067ec:	e59d3020 	ldr	r3, [sp, #32]
603067f0:	e358000a 	cmp	r8, #10
603067f4:	e59d100c 	ldr	r1, [sp, #12]
603067f8:	e5d32000 	ldrb	r2, [r3]
603067fc:	e2d93000 	sbcs	r3, r9, #0
60306800:	23a03001 	movcs	r3, #1
60306804:	33a03000 	movcc	r3, #0
60306808:	e35200ff 	cmp	r2, #255	; 0xff
6030680c:	03a03000 	moveq	r3, #0
60306810:	12033001 	andne	r3, r3, #1
60306814:	e1520001 	cmp	r2, r1
60306818:	13a03000 	movne	r3, #0
6030681c:	02033001 	andeq	r3, r3, #1
60306820:	e3530000 	cmp	r3, #0
60306824:	0a00000d 	beq	60306860 <_vfprintf_r+0x1ed8>
60306828:	e59d3038 	ldr	r3, [sp, #56]	; 0x38
6030682c:	e59d1044 	ldr	r1, [sp, #68]	; 0x44
60306830:	e04aa003 	sub	sl, sl, r3
60306834:	e1a02003 	mov	r2, r3
60306838:	e1a0000a 	mov	r0, sl
6030683c:	eb003460 	bl	603139c4 <__wrap_strncpy>
60306840:	e59d3020 	ldr	r3, [sp, #32]
60306844:	e5d33001 	ldrb	r3, [r3, #1]
60306848:	e3530000 	cmp	r3, #0
6030684c:	159d3020 	ldrne	r3, [sp, #32]
60306850:	12833001 	addne	r3, r3, #1
60306854:	158d3020 	strne	r3, [sp, #32]
60306858:	13a03000 	movne	r3, #0
6030685c:	e58d300c 	str	r3, [sp, #12]
60306860:	e3a03000 	mov	r3, #0
60306864:	e3a0200a 	mov	r2, #10
60306868:	e1a00008 	mov	r0, r8
6030686c:	e1a01009 	mov	r1, r9
60306870:	ebffe817 	bl	603008d4 <__aeabi_uldivmod>
60306874:	e358000a 	cmp	r8, #10
60306878:	e2d93000 	sbcs	r3, r9, #0
6030687c:	3afff9c7 	bcc	60304fa0 <_vfprintf_r+0x618>
60306880:	e1a08000 	mov	r8, r0
60306884:	e1a09001 	mov	r9, r1
60306888:	e58da008 	str	sl, [sp, #8]
6030688c:	eaffffc6 	b	603067ac <_vfprintf_r+0x1e24>
60306890:	60315338 	.word	0x60315338
60306894:	60315328 	.word	0x60315328

60306898 <vfprintf>:
60306898:	e1a03002 	mov	r3, r2
6030689c:	e1a02001 	mov	r2, r1
603068a0:	e1a01000 	mov	r1, r0
603068a4:	e59f0004 	ldr	r0, [pc, #4]	; 603068b0 <vfprintf+0x18>
603068a8:	e5900000 	ldr	r0, [r0]
603068ac:	eafff835 	b	60304988 <_vfprintf_r>
603068b0:	60317000 	.word	0x60317000

603068b4 <__sbprintf>:
603068b4:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
603068b8:	e1a08003 	mov	r8, r3
603068bc:	e1d130bc 	ldrh	r3, [r1, #12]
603068c0:	e24dde46 	sub	sp, sp, #1120	; 0x460
603068c4:	e1a05002 	mov	r5, r2
603068c8:	e24dd008 	sub	sp, sp, #8
603068cc:	e3c33002 	bic	r3, r3, #2
603068d0:	e1a0700d 	mov	r7, sp
603068d4:	e1a06000 	mov	r6, r0
603068d8:	e28d0058 	add	r0, sp, #88	; 0x58
603068dc:	e1a04001 	mov	r4, r1
603068e0:	e1c730bc 	strh	r3, [r7, #12]
603068e4:	e5913064 	ldr	r3, [r1, #100]	; 0x64
603068e8:	e58d3064 	str	r3, [sp, #100]	; 0x64
603068ec:	e1d130be 	ldrh	r3, [r1, #14]
603068f0:	e1c730be 	strh	r3, [r7, #14]
603068f4:	e5913020 	ldr	r3, [r1, #32]
603068f8:	e58d3020 	str	r3, [sp, #32]
603068fc:	e5913028 	ldr	r3, [r1, #40]	; 0x28
60306900:	e58d3028 	str	r3, [sp, #40]	; 0x28
60306904:	e28d3068 	add	r3, sp, #104	; 0x68
60306908:	e58d3000 	str	r3, [sp]
6030690c:	e58d3010 	str	r3, [sp, #16]
60306910:	e3a03b01 	mov	r3, #1024	; 0x400
60306914:	e58d3008 	str	r3, [sp, #8]
60306918:	e58d3014 	str	r3, [sp, #20]
6030691c:	e3a03000 	mov	r3, #0
60306920:	e58d3018 	str	r3, [sp, #24]
60306924:	ebffea37 	bl	60301208 <__retarget_lock_init_recursive>
60306928:	e1a02005 	mov	r2, r5
6030692c:	e1a03008 	mov	r3, r8
60306930:	e1a01007 	mov	r1, r7
60306934:	e1a00006 	mov	r0, r6
60306938:	ebfff812 	bl	60304988 <_vfprintf_r>
6030693c:	e2505000 	subs	r5, r0, #0
60306940:	ba000004 	blt	60306958 <__sbprintf+0xa4>
60306944:	e1a01007 	mov	r1, r7
60306948:	e1a00006 	mov	r0, r6
6030694c:	ebffe8bb 	bl	60300c40 <_fflush_r>
60306950:	e3500000 	cmp	r0, #0
60306954:	13e05000 	mvnne	r5, #0
60306958:	e1dd30bc 	ldrh	r3, [sp, #12]
6030695c:	e59d0058 	ldr	r0, [sp, #88]	; 0x58
60306960:	e3130040 	tst	r3, #64	; 0x40
60306964:	11d430bc 	ldrhne	r3, [r4, #12]
60306968:	13833040 	orrne	r3, r3, #64	; 0x40
6030696c:	11c430bc 	strhne	r3, [r4, #12]
60306970:	ebffea26 	bl	60301210 <__retarget_lock_close_recursive>
60306974:	e1a00005 	mov	r0, r5
60306978:	e28dde46 	add	sp, sp, #1120	; 0x460
6030697c:	e28dd008 	add	sp, sp, #8
60306980:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

60306984 <_wctomb_r>:
60306984:	e59fc010 	ldr	ip, [pc, #16]	; 6030699c <_wctomb_r+0x18>
60306988:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
6030698c:	e59ce0e0 	ldr	lr, [ip, #224]	; 0xe0
60306990:	e1a0c00e 	mov	ip, lr
60306994:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
60306998:	e12fff1c 	bx	ip
6030699c:	603170f4 	.word	0x603170f4

603069a0 <__ascii_wctomb>:
603069a0:	e1a03000 	mov	r3, r0
603069a4:	e2510000 	subs	r0, r1, #0
603069a8:	012fff1e 	bxeq	lr
603069ac:	e35200ff 	cmp	r2, #255	; 0xff
603069b0:	95c02000 	strbls	r2, [r0]
603069b4:	83a0208a 	movhi	r2, #138	; 0x8a
603069b8:	85832000 	strhi	r2, [r3]
603069bc:	83e00000 	mvnhi	r0, #0
603069c0:	93a00001 	movls	r0, #1
603069c4:	e12fff1e 	bx	lr

603069c8 <__swsetup_r>:
603069c8:	e59f3124 	ldr	r3, [pc, #292]	; 60306af4 <__swsetup_r+0x12c>
603069cc:	e92d4070 	push	{r4, r5, r6, lr}
603069d0:	e1a05000 	mov	r5, r0
603069d4:	e1a04001 	mov	r4, r1
603069d8:	e5930000 	ldr	r0, [r3]
603069dc:	e3500000 	cmp	r0, #0
603069e0:	0a000003 	beq	603069f4 <__swsetup_r+0x2c>
603069e4:	e5903018 	ldr	r3, [r0, #24]
603069e8:	e3530000 	cmp	r3, #0
603069ec:	1a000000 	bne	603069f4 <__swsetup_r+0x2c>
603069f0:	ebffe93a 	bl	60300ee0 <__sinit>
603069f4:	e1d430bc 	ldrh	r3, [r4, #12]
603069f8:	e1d420fc 	ldrsh	r2, [r4, #12]
603069fc:	e3130008 	tst	r3, #8
60306a00:	1a000019 	bne	60306a6c <__swsetup_r+0xa4>
60306a04:	e3130010 	tst	r3, #16
60306a08:	03a03009 	moveq	r3, #9
60306a0c:	05853000 	streq	r3, [r5]
60306a10:	0a00002d 	beq	60306acc <__swsetup_r+0x104>
60306a14:	e3130004 	tst	r3, #4
60306a18:	0a000010 	beq	60306a60 <__swsetup_r+0x98>
60306a1c:	e5941034 	ldr	r1, [r4, #52]	; 0x34
60306a20:	e3510000 	cmp	r1, #0
60306a24:	0a000006 	beq	60306a44 <__swsetup_r+0x7c>
60306a28:	e2843044 	add	r3, r4, #68	; 0x44
60306a2c:	e1510003 	cmp	r1, r3
60306a30:	0a000001 	beq	60306a3c <__swsetup_r+0x74>
60306a34:	e1a00005 	mov	r0, r5
60306a38:	eb001af0 	bl	6030d600 <__wrap__free_r>
60306a3c:	e3a03000 	mov	r3, #0
60306a40:	e5843034 	str	r3, [r4, #52]	; 0x34
60306a44:	e1d430bc 	ldrh	r3, [r4, #12]
60306a48:	e3c33024 	bic	r3, r3, #36	; 0x24
60306a4c:	e1c430bc 	strh	r3, [r4, #12]
60306a50:	e3a03000 	mov	r3, #0
60306a54:	e5843004 	str	r3, [r4, #4]
60306a58:	e5943010 	ldr	r3, [r4, #16]
60306a5c:	e5843000 	str	r3, [r4]
60306a60:	e1d430bc 	ldrh	r3, [r4, #12]
60306a64:	e3833008 	orr	r3, r3, #8
60306a68:	e1c430bc 	strh	r3, [r4, #12]
60306a6c:	e5943010 	ldr	r3, [r4, #16]
60306a70:	e3530000 	cmp	r3, #0
60306a74:	1a000006 	bne	60306a94 <__swsetup_r+0xcc>
60306a78:	e1d430bc 	ldrh	r3, [r4, #12]
60306a7c:	e2033d0a 	and	r3, r3, #640	; 0x280
60306a80:	e3530c02 	cmp	r3, #512	; 0x200
60306a84:	0a000002 	beq	60306a94 <__swsetup_r+0xcc>
60306a88:	e1a01004 	mov	r1, r4
60306a8c:	e1a00005 	mov	r0, r5
60306a90:	eb0001ba 	bl	60307180 <__smakebuf_r>
60306a94:	e1d400bc 	ldrh	r0, [r4, #12]
60306a98:	e1d420fc 	ldrsh	r2, [r4, #12]
60306a9c:	e2103001 	ands	r3, r0, #1
60306aa0:	0a00000d 	beq	60306adc <__swsetup_r+0x114>
60306aa4:	e3a03000 	mov	r3, #0
60306aa8:	e5843008 	str	r3, [r4, #8]
60306aac:	e5943014 	ldr	r3, [r4, #20]
60306ab0:	e2633000 	rsb	r3, r3, #0
60306ab4:	e5843018 	str	r3, [r4, #24]
60306ab8:	e5943010 	ldr	r3, [r4, #16]
60306abc:	e3530000 	cmp	r3, #0
60306ac0:	1a000009 	bne	60306aec <__swsetup_r+0x124>
60306ac4:	e2100080 	ands	r0, r0, #128	; 0x80
60306ac8:	08bd8070 	popeq	{r4, r5, r6, pc}
60306acc:	e3823040 	orr	r3, r2, #64	; 0x40
60306ad0:	e3e00000 	mvn	r0, #0
60306ad4:	e1c430bc 	strh	r3, [r4, #12]
60306ad8:	e8bd8070 	pop	{r4, r5, r6, pc}
60306adc:	e3100002 	tst	r0, #2
60306ae0:	05943014 	ldreq	r3, [r4, #20]
60306ae4:	e5843008 	str	r3, [r4, #8]
60306ae8:	eafffff2 	b	60306ab8 <__swsetup_r+0xf0>
60306aec:	e3a00000 	mov	r0, #0
60306af0:	e8bd8070 	pop	{r4, r5, r6, pc}
60306af4:	60317000 	.word	0x60317000

60306af8 <abort>:
60306af8:	e3a00006 	mov	r0, #6
60306afc:	e92d4010 	push	{r4, lr}
60306b00:	eb000242 	bl	60307410 <raise>
60306b04:	e3a00001 	mov	r0, #1
60306b08:	eb000356 	bl	60307868 <_exit>

60306b0c <__fputwc>:
60306b0c:	e92d43f7 	push	{r0, r1, r2, r4, r5, r6, r7, r8, r9, lr}
60306b10:	e1a08000 	mov	r8, r0
60306b14:	e1a06001 	mov	r6, r1
60306b18:	e1a05002 	mov	r5, r2
60306b1c:	ebffef95 	bl	60302978 <__locale_mb_cur_max>
60306b20:	e3500001 	cmp	r0, #1
60306b24:	1a000004 	bne	60306b3c <__fputwc+0x30>
60306b28:	e2463001 	sub	r3, r6, #1
60306b2c:	e35300fe 	cmp	r3, #254	; 0xfe
60306b30:	95cd6004 	strbls	r6, [sp, #4]
60306b34:	91a04000 	movls	r4, r0
60306b38:	9a00000e 	bls	60306b78 <__fputwc+0x6c>
60306b3c:	e285305c 	add	r3, r5, #92	; 0x5c
60306b40:	e1a02006 	mov	r2, r6
60306b44:	e28d1004 	add	r1, sp, #4
60306b48:	e1a00008 	mov	r0, r8
60306b4c:	eb00029f 	bl	603075d0 <_wcrtomb_r>
60306b50:	e1a04000 	mov	r4, r0
60306b54:	e3700001 	cmn	r0, #1
60306b58:	1a000006 	bne	60306b78 <__fputwc+0x6c>
60306b5c:	e1d530bc 	ldrh	r3, [r5, #12]
60306b60:	e1a06000 	mov	r6, r0
60306b64:	e3833040 	orr	r3, r3, #64	; 0x40
60306b68:	e1c530bc 	strh	r3, [r5, #12]
60306b6c:	e1a00006 	mov	r0, r6
60306b70:	e28dd00c 	add	sp, sp, #12
60306b74:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
60306b78:	e3a07000 	mov	r7, #0
60306b7c:	e28d9004 	add	r9, sp, #4
60306b80:	e1570004 	cmp	r7, r4
60306b84:	0afffff8 	beq	60306b6c <__fputwc+0x60>
60306b88:	e5953008 	ldr	r3, [r5, #8]
60306b8c:	e7d71009 	ldrb	r1, [r7, r9]
60306b90:	e2433001 	sub	r3, r3, #1
60306b94:	e5853008 	str	r3, [r5, #8]
60306b98:	e3530000 	cmp	r3, #0
60306b9c:	aa000007 	bge	60306bc0 <__fputwc+0xb4>
60306ba0:	e5952018 	ldr	r2, [r5, #24]
60306ba4:	e1530002 	cmp	r3, r2
60306ba8:	b3a03000 	movlt	r3, #0
60306bac:	a3a03001 	movge	r3, #1
60306bb0:	e351000a 	cmp	r1, #10
60306bb4:	03a03000 	moveq	r3, #0
60306bb8:	e3530000 	cmp	r3, #0
60306bbc:	0a000005 	beq	60306bd8 <__fputwc+0xcc>
60306bc0:	e5953000 	ldr	r3, [r5]
60306bc4:	e2832001 	add	r2, r3, #1
60306bc8:	e5852000 	str	r2, [r5]
60306bcc:	e5c31000 	strb	r1, [r3]
60306bd0:	e2877001 	add	r7, r7, #1
60306bd4:	eaffffe9 	b	60306b80 <__fputwc+0x74>
60306bd8:	e1a02005 	mov	r2, r5
60306bdc:	e1a00008 	mov	r0, r8
60306be0:	eb00022e 	bl	603074a0 <__swbuf_r>
60306be4:	e3700001 	cmn	r0, #1
60306be8:	1afffff8 	bne	60306bd0 <__fputwc+0xc4>
60306bec:	e1a06000 	mov	r6, r0
60306bf0:	eaffffdd 	b	60306b6c <__fputwc+0x60>

60306bf4 <_fputwc_r>:
60306bf4:	e5923064 	ldr	r3, [r2, #100]	; 0x64
60306bf8:	e92d4070 	push	{r4, r5, r6, lr}
60306bfc:	e1a05000 	mov	r5, r0
60306c00:	e1a06001 	mov	r6, r1
60306c04:	e1a04002 	mov	r4, r2
60306c08:	e3130001 	tst	r3, #1
60306c0c:	1a000004 	bne	60306c24 <_fputwc_r+0x30>
60306c10:	e1d230bc 	ldrh	r3, [r2, #12]
60306c14:	e3130c02 	tst	r3, #512	; 0x200
60306c18:	1a000001 	bne	60306c24 <_fputwc_r+0x30>
60306c1c:	e5920058 	ldr	r0, [r2, #88]	; 0x58
60306c20:	ebffe97c 	bl	60301218 <__retarget_lock_acquire_recursive>
60306c24:	e1d430fc 	ldrsh	r3, [r4, #12]
60306c28:	e3130a02 	tst	r3, #8192	; 0x2000
60306c2c:	1a000004 	bne	60306c44 <_fputwc_r+0x50>
60306c30:	e3833a02 	orr	r3, r3, #8192	; 0x2000
60306c34:	e1c430bc 	strh	r3, [r4, #12]
60306c38:	e5943064 	ldr	r3, [r4, #100]	; 0x64
60306c3c:	e3833a02 	orr	r3, r3, #8192	; 0x2000
60306c40:	e5843064 	str	r3, [r4, #100]	; 0x64
60306c44:	e1a00005 	mov	r0, r5
60306c48:	e1a02004 	mov	r2, r4
60306c4c:	e1a01006 	mov	r1, r6
60306c50:	ebffffad 	bl	60306b0c <__fputwc>
60306c54:	e5943064 	ldr	r3, [r4, #100]	; 0x64
60306c58:	e1a05000 	mov	r5, r0
60306c5c:	e3130001 	tst	r3, #1
60306c60:	1a000004 	bne	60306c78 <_fputwc_r+0x84>
60306c64:	e1d430bc 	ldrh	r3, [r4, #12]
60306c68:	e3130c02 	tst	r3, #512	; 0x200
60306c6c:	1a000001 	bne	60306c78 <_fputwc_r+0x84>
60306c70:	e5940058 	ldr	r0, [r4, #88]	; 0x58
60306c74:	ebffe96d 	bl	60301230 <__retarget_lock_release_recursive>
60306c78:	e1a00005 	mov	r0, r5
60306c7c:	e8bd8070 	pop	{r4, r5, r6, pc}

60306c80 <fputwc>:
60306c80:	e59f303c 	ldr	r3, [pc, #60]	; 60306cc4 <fputwc+0x44>
60306c84:	e92d4070 	push	{r4, r5, r6, lr}
60306c88:	e1a05000 	mov	r5, r0
60306c8c:	e1a06001 	mov	r6, r1
60306c90:	e5934000 	ldr	r4, [r3]
60306c94:	e3540000 	cmp	r4, #0
60306c98:	0a000004 	beq	60306cb0 <fputwc+0x30>
60306c9c:	e5943018 	ldr	r3, [r4, #24]
60306ca0:	e3530000 	cmp	r3, #0
60306ca4:	1a000001 	bne	60306cb0 <fputwc+0x30>
60306ca8:	e1a00004 	mov	r0, r4
60306cac:	ebffe88b 	bl	60300ee0 <__sinit>
60306cb0:	e1a02006 	mov	r2, r6
60306cb4:	e1a01005 	mov	r1, r5
60306cb8:	e1a00004 	mov	r0, r4
60306cbc:	e8bd4070 	pop	{r4, r5, r6, lr}
60306cc0:	eaffffcb 	b	60306bf4 <_fputwc_r>
60306cc4:	60317000 	.word	0x60317000

60306cc8 <__sfvwrite_r>:
60306cc8:	e5923008 	ldr	r3, [r2, #8]
60306ccc:	e92d4ff7 	push	{r0, r1, r2, r4, r5, r6, r7, r8, r9, sl, fp, lr}
60306cd0:	e3530000 	cmp	r3, #0
60306cd4:	1a000002 	bne	60306ce4 <__sfvwrite_r+0x1c>
60306cd8:	e3a00000 	mov	r0, #0
60306cdc:	e28dd00c 	add	sp, sp, #12
60306ce0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
60306ce4:	e1d130bc 	ldrh	r3, [r1, #12]
60306ce8:	e1a07000 	mov	r7, r0
60306cec:	e1a04001 	mov	r4, r1
60306cf0:	e1a09002 	mov	r9, r2
60306cf4:	e3130008 	tst	r3, #8
60306cf8:	0a00000d 	beq	60306d34 <__sfvwrite_r+0x6c>
60306cfc:	e5913010 	ldr	r3, [r1, #16]
60306d00:	e3530000 	cmp	r3, #0
60306d04:	0a00000a 	beq	60306d34 <__sfvwrite_r+0x6c>
60306d08:	e1d4a0bc 	ldrh	sl, [r4, #12]
60306d0c:	e5998000 	ldr	r8, [r9]
60306d10:	e21a5002 	ands	r5, sl, #2
60306d14:	1a000024 	bne	60306dac <__sfvwrite_r+0xe4>
60306d18:	e21aa001 	ands	sl, sl, #1
60306d1c:	11a00005 	movne	r0, r5
60306d20:	11a0b005 	movne	fp, r5
60306d24:	11a0a005 	movne	sl, r5
60306d28:	1a0000a6 	bne	60306fc8 <__sfvwrite_r+0x300>
60306d2c:	e1a0600a 	mov	r6, sl
60306d30:	ea000024 	b	60306dc8 <__sfvwrite_r+0x100>
60306d34:	e1a01004 	mov	r1, r4
60306d38:	e1a00007 	mov	r0, r7
60306d3c:	ebffff21 	bl	603069c8 <__swsetup_r>
60306d40:	e3500000 	cmp	r0, #0
60306d44:	0affffef 	beq	60306d08 <__sfvwrite_r+0x40>
60306d48:	e3e00000 	mvn	r0, #0
60306d4c:	eaffffe2 	b	60306cdc <__sfvwrite_r+0x14>
60306d50:	e598a000 	ldr	sl, [r8]
60306d54:	e2888008 	add	r8, r8, #8
60306d58:	e5185004 	ldr	r5, [r8, #-4]
60306d5c:	e3550000 	cmp	r5, #0
60306d60:	0afffffa 	beq	60306d50 <__sfvwrite_r+0x88>
60306d64:	e1550006 	cmp	r5, r6
60306d68:	e594b028 	ldr	fp, [r4, #40]	; 0x28
60306d6c:	31a03005 	movcc	r3, r5
60306d70:	21a03006 	movcs	r3, r6
60306d74:	e5941020 	ldr	r1, [r4, #32]
60306d78:	e1a0200a 	mov	r2, sl
60306d7c:	e1a00007 	mov	r0, r7
60306d80:	e12fff3b 	blx	fp
60306d84:	e3500000 	cmp	r0, #0
60306d88:	da00002c 	ble	60306e40 <__sfvwrite_r+0x178>
60306d8c:	e5993008 	ldr	r3, [r9, #8]
60306d90:	e08aa000 	add	sl, sl, r0
60306d94:	e0455000 	sub	r5, r5, r0
60306d98:	e0430000 	sub	r0, r3, r0
60306d9c:	e5890008 	str	r0, [r9, #8]
60306da0:	e3500000 	cmp	r0, #0
60306da4:	1affffec 	bne	60306d5c <__sfvwrite_r+0x94>
60306da8:	eaffffca 	b	60306cd8 <__sfvwrite_r+0x10>
60306dac:	e3a0a000 	mov	sl, #0
60306db0:	e59f6340 	ldr	r6, [pc, #832]	; 603070f8 <__sfvwrite_r+0x430>
60306db4:	e1a0500a 	mov	r5, sl
60306db8:	eaffffe7 	b	60306d5c <__sfvwrite_r+0x94>
60306dbc:	e598a000 	ldr	sl, [r8]
60306dc0:	e2888008 	add	r8, r8, #8
60306dc4:	e5186004 	ldr	r6, [r8, #-4]
60306dc8:	e3560000 	cmp	r6, #0
60306dcc:	0afffffa 	beq	60306dbc <__sfvwrite_r+0xf4>
60306dd0:	e1d430bc 	ldrh	r3, [r4, #12]
60306dd4:	e5940000 	ldr	r0, [r4]
60306dd8:	e5942008 	ldr	r2, [r4, #8]
60306ddc:	e3130c02 	tst	r3, #512	; 0x200
60306de0:	0a00004c 	beq	60306f18 <__sfvwrite_r+0x250>
60306de4:	e1520006 	cmp	r2, r6
60306de8:	8a000028 	bhi	60306e90 <__sfvwrite_r+0x1c8>
60306dec:	e3130d12 	tst	r3, #1152	; 0x480
60306df0:	0a000026 	beq	60306e90 <__sfvwrite_r+0x1c8>
60306df4:	e5945014 	ldr	r5, [r4, #20]
60306df8:	e5941010 	ldr	r1, [r4, #16]
60306dfc:	e0855085 	add	r5, r5, r5, lsl #1
60306e00:	e040b001 	sub	fp, r0, r1
60306e04:	e28b2001 	add	r2, fp, #1
60306e08:	e0855fa5 	add	r5, r5, r5, lsr #31
60306e0c:	e0822006 	add	r2, r2, r6
60306e10:	e1a050c5 	asr	r5, r5, #1
60306e14:	e1550002 	cmp	r5, r2
60306e18:	31a05002 	movcc	r5, r2
60306e1c:	e3130b01 	tst	r3, #1024	; 0x400
60306e20:	0a000030 	beq	60306ee8 <__sfvwrite_r+0x220>
60306e24:	e1a01005 	mov	r1, r5
60306e28:	e1a00007 	mov	r0, r7
60306e2c:	eb0019ee 	bl	6030d5ec <__wrap__malloc_r>
60306e30:	e2503000 	subs	r3, r0, #0
60306e34:	1a000005 	bne	60306e50 <__sfvwrite_r+0x188>
60306e38:	e3a0300c 	mov	r3, #12
60306e3c:	e5873000 	str	r3, [r7]
60306e40:	e1d430bc 	ldrh	r3, [r4, #12]
60306e44:	e3833040 	orr	r3, r3, #64	; 0x40
60306e48:	e1c430bc 	strh	r3, [r4, #12]
60306e4c:	eaffffbd 	b	60306d48 <__sfvwrite_r+0x80>
60306e50:	e1a0200b 	mov	r2, fp
60306e54:	e5941010 	ldr	r1, [r4, #16]
60306e58:	e58d3004 	str	r3, [sp, #4]
60306e5c:	eb0037e4 	bl	60314df4 <__wrap_memcpy>
60306e60:	e1d420bc 	ldrh	r2, [r4, #12]
60306e64:	e59d3004 	ldr	r3, [sp, #4]
60306e68:	e3c22d12 	bic	r2, r2, #1152	; 0x480
60306e6c:	e3822080 	orr	r2, r2, #128	; 0x80
60306e70:	e1c420bc 	strh	r2, [r4, #12]
60306e74:	e1a02006 	mov	r2, r6
60306e78:	e5843010 	str	r3, [r4, #16]
60306e7c:	e5845014 	str	r5, [r4, #20]
60306e80:	e083300b 	add	r3, r3, fp
60306e84:	e045500b 	sub	r5, r5, fp
60306e88:	e5843000 	str	r3, [r4]
60306e8c:	e5845008 	str	r5, [r4, #8]
60306e90:	e1520006 	cmp	r2, r6
60306e94:	e5940000 	ldr	r0, [r4]
60306e98:	21a02006 	movcs	r2, r6
60306e9c:	e1a0100a 	mov	r1, sl
60306ea0:	e58d2004 	str	r2, [sp, #4]
60306ea4:	eb0037d8 	bl	60314e0c <__wrap_memmove>
60306ea8:	e5943008 	ldr	r3, [r4, #8]
60306eac:	e1a05006 	mov	r5, r6
60306eb0:	e59d2004 	ldr	r2, [sp, #4]
60306eb4:	e0433002 	sub	r3, r3, r2
60306eb8:	e5843008 	str	r3, [r4, #8]
60306ebc:	e5943000 	ldr	r3, [r4]
60306ec0:	e0832002 	add	r2, r3, r2
60306ec4:	e5842000 	str	r2, [r4]
60306ec8:	e5990008 	ldr	r0, [r9, #8]
60306ecc:	e08aa005 	add	sl, sl, r5
60306ed0:	e0466005 	sub	r6, r6, r5
60306ed4:	e0405005 	sub	r5, r0, r5
60306ed8:	e5895008 	str	r5, [r9, #8]
60306edc:	e3550000 	cmp	r5, #0
60306ee0:	1affffb8 	bne	60306dc8 <__sfvwrite_r+0x100>
60306ee4:	eaffff7b 	b	60306cd8 <__sfvwrite_r+0x10>
60306ee8:	e1a02005 	mov	r2, r5
60306eec:	e1a00007 	mov	r0, r7
60306ef0:	eb0019bf 	bl	6030d5f4 <__wrap__realloc_r>
60306ef4:	e2503000 	subs	r3, r0, #0
60306ef8:	1affffdd 	bne	60306e74 <__sfvwrite_r+0x1ac>
60306efc:	e5941010 	ldr	r1, [r4, #16]
60306f00:	e1a00007 	mov	r0, r7
60306f04:	eb0019bd 	bl	6030d600 <__wrap__free_r>
60306f08:	e1d430bc 	ldrh	r3, [r4, #12]
60306f0c:	e3c33080 	bic	r3, r3, #128	; 0x80
60306f10:	e1c430bc 	strh	r3, [r4, #12]
60306f14:	eaffffc7 	b	60306e38 <__sfvwrite_r+0x170>
60306f18:	e5943010 	ldr	r3, [r4, #16]
60306f1c:	e1530000 	cmp	r3, r0
60306f20:	3a000002 	bcc	60306f30 <__sfvwrite_r+0x268>
60306f24:	e5945014 	ldr	r5, [r4, #20]
60306f28:	e1550006 	cmp	r5, r6
60306f2c:	9a000013 	bls	60306f80 <__sfvwrite_r+0x2b8>
60306f30:	e1520006 	cmp	r2, r6
60306f34:	e1a0100a 	mov	r1, sl
60306f38:	31a05002 	movcc	r5, r2
60306f3c:	21a05006 	movcs	r5, r6
60306f40:	e1a02005 	mov	r2, r5
60306f44:	eb0037b0 	bl	60314e0c <__wrap_memmove>
60306f48:	e5943008 	ldr	r3, [r4, #8]
60306f4c:	e5942000 	ldr	r2, [r4]
60306f50:	e0433005 	sub	r3, r3, r5
60306f54:	e5843008 	str	r3, [r4, #8]
60306f58:	e0822005 	add	r2, r2, r5
60306f5c:	e3530000 	cmp	r3, #0
60306f60:	e5842000 	str	r2, [r4]
60306f64:	1affffd7 	bne	60306ec8 <__sfvwrite_r+0x200>
60306f68:	e1a01004 	mov	r1, r4
60306f6c:	e1a00007 	mov	r0, r7
60306f70:	ebffe732 	bl	60300c40 <_fflush_r>
60306f74:	e3500000 	cmp	r0, #0
60306f78:	0affffd2 	beq	60306ec8 <__sfvwrite_r+0x200>
60306f7c:	eaffffaf 	b	60306e40 <__sfvwrite_r+0x178>
60306f80:	e376010a 	cmn	r6, #-2147483646	; 0x80000002
60306f84:	e1a01005 	mov	r1, r5
60306f88:	91a00006 	movls	r0, r6
60306f8c:	83e00102 	mvnhi	r0, #-2147483648	; 0x80000000
60306f90:	eb0001f5 	bl	6030776c <__divsi3>
60306f94:	e594b028 	ldr	fp, [r4, #40]	; 0x28
60306f98:	e0030095 	mul	r3, r5, r0
60306f9c:	e5941020 	ldr	r1, [r4, #32]
60306fa0:	e1a0200a 	mov	r2, sl
60306fa4:	e1a00007 	mov	r0, r7
60306fa8:	e12fff3b 	blx	fp
60306fac:	e2505000 	subs	r5, r0, #0
60306fb0:	caffffc4 	bgt	60306ec8 <__sfvwrite_r+0x200>
60306fb4:	eaffffa1 	b	60306e40 <__sfvwrite_r+0x178>
60306fb8:	e598b000 	ldr	fp, [r8]
60306fbc:	e3a00000 	mov	r0, #0
60306fc0:	e598a004 	ldr	sl, [r8, #4]
60306fc4:	e2888008 	add	r8, r8, #8
60306fc8:	e35a0000 	cmp	sl, #0
60306fcc:	0afffff9 	beq	60306fb8 <__sfvwrite_r+0x2f0>
60306fd0:	e3500000 	cmp	r0, #0
60306fd4:	1a000007 	bne	60306ff8 <__sfvwrite_r+0x330>
60306fd8:	e1a0200a 	mov	r2, sl
60306fdc:	e3a0100a 	mov	r1, #10
60306fe0:	e1a0000b 	mov	r0, fp
60306fe4:	eb003785 	bl	60314e00 <__wrap_memchr>
60306fe8:	e2505000 	subs	r5, r0, #0
60306fec:	12855001 	addne	r5, r5, #1
60306ff0:	028a5001 	addeq	r5, sl, #1
60306ff4:	1045500b 	subne	r5, r5, fp
60306ff8:	e5941010 	ldr	r1, [r4, #16]
60306ffc:	e155000a 	cmp	r5, sl
60307000:	e5943014 	ldr	r3, [r4, #20]
60307004:	31a02005 	movcc	r2, r5
60307008:	e5946008 	ldr	r6, [r4, #8]
6030700c:	21a0200a 	movcs	r2, sl
60307010:	e5940000 	ldr	r0, [r4]
60307014:	e0836006 	add	r6, r3, r6
60307018:	e1500001 	cmp	r0, r1
6030701c:	93a01000 	movls	r1, #0
60307020:	83a01001 	movhi	r1, #1
60307024:	e1520006 	cmp	r2, r6
60307028:	d3a01000 	movle	r1, #0
6030702c:	e3510000 	cmp	r1, #0
60307030:	0a00001a 	beq	603070a0 <__sfvwrite_r+0x3d8>
60307034:	e1a02006 	mov	r2, r6
60307038:	e1a0100b 	mov	r1, fp
6030703c:	eb003772 	bl	60314e0c <__wrap_memmove>
60307040:	e5943000 	ldr	r3, [r4]
60307044:	e1a01004 	mov	r1, r4
60307048:	e1a00007 	mov	r0, r7
6030704c:	e0833006 	add	r3, r3, r6
60307050:	e5843000 	str	r3, [r4]
60307054:	ebffe6f9 	bl	60300c40 <_fflush_r>
60307058:	e3500000 	cmp	r0, #0
6030705c:	1affff77 	bne	60306e40 <__sfvwrite_r+0x178>
60307060:	e0555006 	subs	r5, r5, r6
60307064:	13a00001 	movne	r0, #1
60307068:	1a000004 	bne	60307080 <__sfvwrite_r+0x3b8>
6030706c:	e1a01004 	mov	r1, r4
60307070:	e1a00007 	mov	r0, r7
60307074:	ebffe6f1 	bl	60300c40 <_fflush_r>
60307078:	e3500000 	cmp	r0, #0
6030707c:	1affff6f 	bne	60306e40 <__sfvwrite_r+0x178>
60307080:	e5992008 	ldr	r2, [r9, #8]
60307084:	e08bb006 	add	fp, fp, r6
60307088:	e04aa006 	sub	sl, sl, r6
6030708c:	e0426006 	sub	r6, r2, r6
60307090:	e5896008 	str	r6, [r9, #8]
60307094:	e3560000 	cmp	r6, #0
60307098:	1affffca 	bne	60306fc8 <__sfvwrite_r+0x300>
6030709c:	eaffff0d 	b	60306cd8 <__sfvwrite_r+0x10>
603070a0:	e1530002 	cmp	r3, r2
603070a4:	ca000007 	bgt	603070c8 <__sfvwrite_r+0x400>
603070a8:	e5946028 	ldr	r6, [r4, #40]	; 0x28
603070ac:	e1a0200b 	mov	r2, fp
603070b0:	e5941020 	ldr	r1, [r4, #32]
603070b4:	e1a00007 	mov	r0, r7
603070b8:	e12fff36 	blx	r6
603070bc:	e2506000 	subs	r6, r0, #0
603070c0:	caffffe6 	bgt	60307060 <__sfvwrite_r+0x398>
603070c4:	eaffff5d 	b	60306e40 <__sfvwrite_r+0x178>
603070c8:	e1a0100b 	mov	r1, fp
603070cc:	e58d2004 	str	r2, [sp, #4]
603070d0:	eb00374d 	bl	60314e0c <__wrap_memmove>
603070d4:	e59d2004 	ldr	r2, [sp, #4]
603070d8:	e5943008 	ldr	r3, [r4, #8]
603070dc:	e1a06002 	mov	r6, r2
603070e0:	e0433002 	sub	r3, r3, r2
603070e4:	e5843008 	str	r3, [r4, #8]
603070e8:	e5943000 	ldr	r3, [r4]
603070ec:	e0833002 	add	r3, r3, r2
603070f0:	e5843000 	str	r3, [r4]
603070f4:	eaffffd9 	b	60307060 <__sfvwrite_r+0x398>
603070f8:	7ffffc00 	.word	0x7ffffc00

603070fc <__swhatbuf_r>:
603070fc:	e92d4070 	push	{r4, r5, r6, lr}
60307100:	e1a06001 	mov	r6, r1
60307104:	e1d110fe 	ldrsh	r1, [r1, #14]
60307108:	e24dd058 	sub	sp, sp, #88	; 0x58
6030710c:	e1a04002 	mov	r4, r2
60307110:	e1a05003 	mov	r5, r3
60307114:	e3510000 	cmp	r1, #0
60307118:	aa00000a 	bge	60307148 <__swhatbuf_r+0x4c>
6030711c:	e1d610fc 	ldrsh	r1, [r6, #12]
60307120:	e3a03000 	mov	r3, #0
60307124:	e5853000 	str	r3, [r5]
60307128:	e2110080 	ands	r0, r1, #128	; 0x80
6030712c:	13a02040 	movne	r2, #64	; 0x40
60307130:	11a00003 	movne	r0, r3
60307134:	15842000 	strne	r2, [r4]
60307138:	03a03b01 	moveq	r3, #1024	; 0x400
6030713c:	05843000 	streq	r3, [r4]
60307140:	e28dd058 	add	sp, sp, #88	; 0x58
60307144:	e8bd8070 	pop	{r4, r5, r6, pc}
60307148:	e1a0200d 	mov	r2, sp
6030714c:	eb000137 	bl	60307630 <_fstat_r>
60307150:	e3500000 	cmp	r0, #0
60307154:	bafffff0 	blt	6030711c <__swhatbuf_r+0x20>
60307158:	e59d2004 	ldr	r2, [sp, #4]
6030715c:	e3a03b01 	mov	r3, #1024	; 0x400
60307160:	e3a00b02 	mov	r0, #2048	; 0x800
60307164:	e2022a0f 	and	r2, r2, #61440	; 0xf000
60307168:	e2422a02 	sub	r2, r2, #8192	; 0x2000
6030716c:	e16f2f12 	clz	r2, r2
60307170:	e1a022a2 	lsr	r2, r2, #5
60307174:	e5852000 	str	r2, [r5]
60307178:	e5843000 	str	r3, [r4]
6030717c:	eaffffef 	b	60307140 <__swhatbuf_r+0x44>

60307180 <__smakebuf_r>:
60307180:	e1d130bc 	ldrh	r3, [r1, #12]
60307184:	e92d4073 	push	{r0, r1, r4, r5, r6, lr}
60307188:	e1a04001 	mov	r4, r1
6030718c:	e3130002 	tst	r3, #2
60307190:	0a000006 	beq	603071b0 <__smakebuf_r+0x30>
60307194:	e2843047 	add	r3, r4, #71	; 0x47
60307198:	e5843000 	str	r3, [r4]
6030719c:	e5843010 	str	r3, [r4, #16]
603071a0:	e3a03001 	mov	r3, #1
603071a4:	e5843014 	str	r3, [r4, #20]
603071a8:	e28dd008 	add	sp, sp, #8
603071ac:	e8bd8070 	pop	{r4, r5, r6, pc}
603071b0:	e28d3004 	add	r3, sp, #4
603071b4:	e1a05000 	mov	r5, r0
603071b8:	e1a0200d 	mov	r2, sp
603071bc:	ebffffce 	bl	603070fc <__swhatbuf_r>
603071c0:	e59d1000 	ldr	r1, [sp]
603071c4:	e1a06000 	mov	r6, r0
603071c8:	e1a00005 	mov	r0, r5
603071cc:	eb001906 	bl	6030d5ec <__wrap__malloc_r>
603071d0:	e2503000 	subs	r3, r0, #0
603071d4:	1a000006 	bne	603071f4 <__smakebuf_r+0x74>
603071d8:	e1d430fc 	ldrsh	r3, [r4, #12]
603071dc:	e3130c02 	tst	r3, #512	; 0x200
603071e0:	1afffff0 	bne	603071a8 <__smakebuf_r+0x28>
603071e4:	e3c33003 	bic	r3, r3, #3
603071e8:	e3833002 	orr	r3, r3, #2
603071ec:	e1c430bc 	strh	r3, [r4, #12]
603071f0:	eaffffe7 	b	60307194 <__smakebuf_r+0x14>
603071f4:	e59f2058 	ldr	r2, [pc, #88]	; 60307254 <__smakebuf_r+0xd4>
603071f8:	e5852028 	str	r2, [r5, #40]	; 0x28
603071fc:	e5843000 	str	r3, [r4]
60307200:	e5843010 	str	r3, [r4, #16]
60307204:	e59d3000 	ldr	r3, [sp]
60307208:	e1d420bc 	ldrh	r2, [r4, #12]
6030720c:	e5843014 	str	r3, [r4, #20]
60307210:	e59d3004 	ldr	r3, [sp, #4]
60307214:	e3822080 	orr	r2, r2, #128	; 0x80
60307218:	e1c420bc 	strh	r2, [r4, #12]
6030721c:	e3530000 	cmp	r3, #0
60307220:	0a000007 	beq	60307244 <__smakebuf_r+0xc4>
60307224:	e1d410fe 	ldrsh	r1, [r4, #14]
60307228:	e1a00005 	mov	r0, r5
6030722c:	eb00010e 	bl	6030766c <_isatty_r>
60307230:	e3500000 	cmp	r0, #0
60307234:	11d430bc 	ldrhne	r3, [r4, #12]
60307238:	13c33003 	bicne	r3, r3, #3
6030723c:	13833001 	orrne	r3, r3, #1
60307240:	11c430bc 	strhne	r3, [r4, #12]
60307244:	e1d400bc 	ldrh	r0, [r4, #12]
60307248:	e1800006 	orr	r0, r0, r6
6030724c:	e1c400bc 	strh	r0, [r4, #12]
60307250:	eaffffd4 	b	603071a8 <__smakebuf_r+0x28>
60307254:	60300d88 	.word	0x60300d88

60307258 <_init_signal_r>:
60307258:	e92d4070 	push	{r4, r5, r6, lr}
6030725c:	e5905044 	ldr	r5, [r0, #68]	; 0x44
60307260:	e3550000 	cmp	r5, #0
60307264:	1a00000a 	bne	60307294 <_init_signal_r+0x3c>
60307268:	e1a04000 	mov	r4, r0
6030726c:	e3a01080 	mov	r1, #128	; 0x80
60307270:	eb0018dd 	bl	6030d5ec <__wrap__malloc_r>
60307274:	e3500000 	cmp	r0, #0
60307278:	e5840044 	str	r0, [r4, #68]	; 0x44
6030727c:	0a000006 	beq	6030729c <_init_signal_r+0x44>
60307280:	e2403004 	sub	r3, r0, #4
60307284:	e280007c 	add	r0, r0, #124	; 0x7c
60307288:	e5a35004 	str	r5, [r3, #4]!
6030728c:	e1530000 	cmp	r3, r0
60307290:	1afffffc 	bne	60307288 <_init_signal_r+0x30>
60307294:	e3a00000 	mov	r0, #0
60307298:	e8bd8070 	pop	{r4, r5, r6, pc}
6030729c:	e3e00000 	mvn	r0, #0
603072a0:	e8bd8070 	pop	{r4, r5, r6, pc}

603072a4 <_signal_r>:
603072a4:	e351001f 	cmp	r1, #31
603072a8:	e92d4070 	push	{r4, r5, r6, lr}
603072ac:	83a03016 	movhi	r3, #22
603072b0:	e1a04000 	mov	r4, r0
603072b4:	85803000 	strhi	r3, [r0]
603072b8:	8a00000b 	bhi	603072ec <_signal_r+0x48>
603072bc:	e5903044 	ldr	r3, [r0, #68]	; 0x44
603072c0:	e1a05001 	mov	r5, r1
603072c4:	e1a06002 	mov	r6, r2
603072c8:	e3530000 	cmp	r3, #0
603072cc:	0a000003 	beq	603072e0 <_signal_r+0x3c>
603072d0:	e5943044 	ldr	r3, [r4, #68]	; 0x44
603072d4:	e7930105 	ldr	r0, [r3, r5, lsl #2]
603072d8:	e7836105 	str	r6, [r3, r5, lsl #2]
603072dc:	e8bd8070 	pop	{r4, r5, r6, pc}
603072e0:	ebffffdc 	bl	60307258 <_init_signal_r>
603072e4:	e3500000 	cmp	r0, #0
603072e8:	0afffff8 	beq	603072d0 <_signal_r+0x2c>
603072ec:	e3e00000 	mvn	r0, #0
603072f0:	e8bd8070 	pop	{r4, r5, r6, pc}

603072f4 <_raise_r>:
603072f4:	e92d4070 	push	{r4, r5, r6, lr}
603072f8:	e351001f 	cmp	r1, #31
603072fc:	e1a04000 	mov	r4, r0
60307300:	9a000003 	bls	60307314 <_raise_r+0x20>
60307304:	e3a03016 	mov	r3, #22
60307308:	e5803000 	str	r3, [r0]
6030730c:	e3e00000 	mvn	r0, #0
60307310:	e8bd8070 	pop	{r4, r5, r6, pc}
60307314:	e5902044 	ldr	r2, [r0, #68]	; 0x44
60307318:	e1a05001 	mov	r5, r1
6030731c:	e3520000 	cmp	r2, #0
60307320:	0a000002 	beq	60307330 <_raise_r+0x3c>
60307324:	e7923101 	ldr	r3, [r2, r1, lsl #2]
60307328:	e3530000 	cmp	r3, #0
6030732c:	1a000006 	bne	6030734c <_raise_r+0x58>
60307330:	e1a00004 	mov	r0, r4
60307334:	eb000058 	bl	6030749c <_getpid_r>
60307338:	e1a02005 	mov	r2, r5
6030733c:	e1a01000 	mov	r1, r0
60307340:	e1a00004 	mov	r0, r4
60307344:	e8bd4070 	pop	{r4, r5, r6, lr}
60307348:	ea000044 	b	60307460 <_kill_r>
6030734c:	e3530001 	cmp	r3, #1
60307350:	0a000009 	beq	6030737c <_raise_r+0x88>
60307354:	e3730001 	cmn	r3, #1
60307358:	1a000003 	bne	6030736c <_raise_r+0x78>
6030735c:	e3a03016 	mov	r3, #22
60307360:	e5803000 	str	r3, [r0]
60307364:	e3a00001 	mov	r0, #1
60307368:	e8bd8070 	pop	{r4, r5, r6, pc}
6030736c:	e3a04000 	mov	r4, #0
60307370:	e1a00001 	mov	r0, r1
60307374:	e7824101 	str	r4, [r2, r1, lsl #2]
60307378:	e12fff33 	blx	r3
6030737c:	e3a00000 	mov	r0, #0
60307380:	e8bd8070 	pop	{r4, r5, r6, pc}

60307384 <__sigtramp_r>:
60307384:	e351001f 	cmp	r1, #31
60307388:	9a000001 	bls	60307394 <__sigtramp_r+0x10>
6030738c:	e3e00000 	mvn	r0, #0
60307390:	e12fff1e 	bx	lr
60307394:	e5903044 	ldr	r3, [r0, #68]	; 0x44
60307398:	e92d4070 	push	{r4, r5, r6, lr}
6030739c:	e1a04000 	mov	r4, r0
603073a0:	e1a05001 	mov	r5, r1
603073a4:	e3530000 	cmp	r3, #0
603073a8:	0a000005 	beq	603073c4 <__sigtramp_r+0x40>
603073ac:	e5942044 	ldr	r2, [r4, #68]	; 0x44
603073b0:	e7923105 	ldr	r3, [r2, r5, lsl #2]
603073b4:	e3530000 	cmp	r3, #0
603073b8:	1a000006 	bne	603073d8 <__sigtramp_r+0x54>
603073bc:	e3a00001 	mov	r0, #1
603073c0:	e8bd8070 	pop	{r4, r5, r6, pc}
603073c4:	ebffffa3 	bl	60307258 <_init_signal_r>
603073c8:	e3500000 	cmp	r0, #0
603073cc:	0afffff6 	beq	603073ac <__sigtramp_r+0x28>
603073d0:	e3e00000 	mvn	r0, #0
603073d4:	e8bd8070 	pop	{r4, r5, r6, pc}
603073d8:	e3730001 	cmn	r3, #1
603073dc:	0a000007 	beq	60307400 <__sigtramp_r+0x7c>
603073e0:	e3530001 	cmp	r3, #1
603073e4:	0a000007 	beq	60307408 <__sigtramp_r+0x84>
603073e8:	e3a04000 	mov	r4, #0
603073ec:	e1a00005 	mov	r0, r5
603073f0:	e7824105 	str	r4, [r2, r5, lsl #2]
603073f4:	e12fff33 	blx	r3
603073f8:	e1a00004 	mov	r0, r4
603073fc:	e8bd8070 	pop	{r4, r5, r6, pc}
60307400:	e3a00002 	mov	r0, #2
60307404:	e8bd8070 	pop	{r4, r5, r6, pc}
60307408:	e3a00003 	mov	r0, #3
6030740c:	e8bd8070 	pop	{r4, r5, r6, pc}

60307410 <raise>:
60307410:	e59f3008 	ldr	r3, [pc, #8]	; 60307420 <raise+0x10>
60307414:	e1a01000 	mov	r1, r0
60307418:	e5930000 	ldr	r0, [r3]
6030741c:	eaffffb4 	b	603072f4 <_raise_r>
60307420:	60317000 	.word	0x60317000

60307424 <signal>:
60307424:	e59f300c 	ldr	r3, [pc, #12]	; 60307438 <signal+0x14>
60307428:	e1a02001 	mov	r2, r1
6030742c:	e1a01000 	mov	r1, r0
60307430:	e5930000 	ldr	r0, [r3]
60307434:	eaffff9a 	b	603072a4 <_signal_r>
60307438:	60317000 	.word	0x60317000

6030743c <_init_signal>:
6030743c:	e59f3004 	ldr	r3, [pc, #4]	; 60307448 <_init_signal+0xc>
60307440:	e5930000 	ldr	r0, [r3]
60307444:	eaffff83 	b	60307258 <_init_signal_r>
60307448:	60317000 	.word	0x60317000

6030744c <__sigtramp>:
6030744c:	e59f3008 	ldr	r3, [pc, #8]	; 6030745c <__sigtramp+0x10>
60307450:	e1a01000 	mov	r1, r0
60307454:	e5930000 	ldr	r0, [r3]
60307458:	eaffffc9 	b	60307384 <__sigtramp_r>
6030745c:	60317000 	.word	0x60317000

60307460 <_kill_r>:
60307460:	e92d4070 	push	{r4, r5, r6, lr}
60307464:	e1a04000 	mov	r4, r0
60307468:	e59f5028 	ldr	r5, [pc, #40]	; 60307498 <_kill_r+0x38>
6030746c:	e1a00001 	mov	r0, r1
60307470:	e1a01002 	mov	r1, r2
60307474:	e3a03000 	mov	r3, #0
60307478:	e5853000 	str	r3, [r5]
6030747c:	eb000d76 	bl	6030aa5c <_kill>
60307480:	e3700001 	cmn	r0, #1
60307484:	18bd8070 	popne	{r4, r5, r6, pc}
60307488:	e5953000 	ldr	r3, [r5]
6030748c:	e3530000 	cmp	r3, #0
60307490:	15843000 	strne	r3, [r4]
60307494:	e8bd8070 	pop	{r4, r5, r6, pc}
60307498:	60317460 	.word	0x60317460

6030749c <_getpid_r>:
6030749c:	ea000d6c 	b	6030aa54 <_getpid>

603074a0 <__swbuf_r>:
603074a0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
603074a4:	e1a06001 	mov	r6, r1
603074a8:	e1a04002 	mov	r4, r2
603074ac:	e2505000 	subs	r5, r0, #0
603074b0:	0a000003 	beq	603074c4 <__swbuf_r+0x24>
603074b4:	e5953018 	ldr	r3, [r5, #24]
603074b8:	e3530000 	cmp	r3, #0
603074bc:	1a000000 	bne	603074c4 <__swbuf_r+0x24>
603074c0:	ebffe686 	bl	60300ee0 <__sinit>
603074c4:	e5943018 	ldr	r3, [r4, #24]
603074c8:	e5843008 	str	r3, [r4, #8]
603074cc:	e1d430bc 	ldrh	r3, [r4, #12]
603074d0:	e3130008 	tst	r3, #8
603074d4:	0a00002a 	beq	60307584 <__swbuf_r+0xe4>
603074d8:	e5943010 	ldr	r3, [r4, #16]
603074dc:	e3530000 	cmp	r3, #0
603074e0:	0a000027 	beq	60307584 <__swbuf_r+0xe4>
603074e4:	e1d430fc 	ldrsh	r3, [r4, #12]
603074e8:	e6ef6076 	uxtb	r6, r6
603074ec:	e1a07006 	mov	r7, r6
603074f0:	e3130a02 	tst	r3, #8192	; 0x2000
603074f4:	0a000029 	beq	603075a0 <__swbuf_r+0x100>
603074f8:	e5943010 	ldr	r3, [r4, #16]
603074fc:	e5940000 	ldr	r0, [r4]
60307500:	e0400003 	sub	r0, r0, r3
60307504:	e5943014 	ldr	r3, [r4, #20]
60307508:	e1530000 	cmp	r3, r0
6030750c:	ca000004 	bgt	60307524 <__swbuf_r+0x84>
60307510:	e1a01004 	mov	r1, r4
60307514:	e1a00005 	mov	r0, r5
60307518:	ebffe5c8 	bl	60300c40 <_fflush_r>
6030751c:	e3500000 	cmp	r0, #0
60307520:	1a00001c 	bne	60307598 <__swbuf_r+0xf8>
60307524:	e5943008 	ldr	r3, [r4, #8]
60307528:	e2800001 	add	r0, r0, #1
6030752c:	e2433001 	sub	r3, r3, #1
60307530:	e5843008 	str	r3, [r4, #8]
60307534:	e5943000 	ldr	r3, [r4]
60307538:	e2832001 	add	r2, r3, #1
6030753c:	e5842000 	str	r2, [r4]
60307540:	e5c36000 	strb	r6, [r3]
60307544:	e5943014 	ldr	r3, [r4, #20]
60307548:	e1530000 	cmp	r3, r0
6030754c:	0a000005 	beq	60307568 <__swbuf_r+0xc8>
60307550:	e1d430bc 	ldrh	r3, [r4, #12]
60307554:	e356000a 	cmp	r6, #10
60307558:	13a03000 	movne	r3, #0
6030755c:	02033001 	andeq	r3, r3, #1
60307560:	e3530000 	cmp	r3, #0
60307564:	0a000004 	beq	6030757c <__swbuf_r+0xdc>
60307568:	e1a01004 	mov	r1, r4
6030756c:	e1a00005 	mov	r0, r5
60307570:	ebffe5b2 	bl	60300c40 <_fflush_r>
60307574:	e3500000 	cmp	r0, #0
60307578:	1a000006 	bne	60307598 <__swbuf_r+0xf8>
6030757c:	e1a00007 	mov	r0, r7
60307580:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60307584:	e1a01004 	mov	r1, r4
60307588:	e1a00005 	mov	r0, r5
6030758c:	ebfffd0d 	bl	603069c8 <__swsetup_r>
60307590:	e3500000 	cmp	r0, #0
60307594:	0affffd2 	beq	603074e4 <__swbuf_r+0x44>
60307598:	e3e07000 	mvn	r7, #0
6030759c:	eafffff6 	b	6030757c <__swbuf_r+0xdc>
603075a0:	e3833a02 	orr	r3, r3, #8192	; 0x2000
603075a4:	e1c430bc 	strh	r3, [r4, #12]
603075a8:	e5943064 	ldr	r3, [r4, #100]	; 0x64
603075ac:	e3c33a02 	bic	r3, r3, #8192	; 0x2000
603075b0:	e5843064 	str	r3, [r4, #100]	; 0x64
603075b4:	eaffffcf 	b	603074f8 <__swbuf_r+0x58>

603075b8 <__swbuf>:
603075b8:	e59f300c 	ldr	r3, [pc, #12]	; 603075cc <__swbuf+0x14>
603075bc:	e1a02001 	mov	r2, r1
603075c0:	e1a01000 	mov	r1, r0
603075c4:	e5930000 	ldr	r0, [r3]
603075c8:	eaffffb4 	b	603074a0 <__swbuf_r>
603075cc:	60317000 	.word	0x60317000

603075d0 <_wcrtomb_r>:
603075d0:	e59fc038 	ldr	ip, [pc, #56]	; 60307610 <_wcrtomb_r+0x40>
603075d4:	e92d407f 	push	{r0, r1, r2, r3, r4, r5, r6, lr}
603075d8:	e1a05003 	mov	r5, r3
603075dc:	e1a04000 	mov	r4, r0
603075e0:	e59c60e0 	ldr	r6, [ip, #224]	; 0xe0
603075e4:	e251c000 	subs	ip, r1, #0
603075e8:	01a0200c 	moveq	r2, ip
603075ec:	028d1004 	addeq	r1, sp, #4
603075f0:	e12fff36 	blx	r6
603075f4:	e3700001 	cmn	r0, #1
603075f8:	03a03000 	moveq	r3, #0
603075fc:	05853000 	streq	r3, [r5]
60307600:	03a0308a 	moveq	r3, #138	; 0x8a
60307604:	05843000 	streq	r3, [r4]
60307608:	e28dd010 	add	sp, sp, #16
6030760c:	e8bd8070 	pop	{r4, r5, r6, pc}
60307610:	603170f4 	.word	0x603170f4

60307614 <wcrtomb>:
60307614:	e1a03002 	mov	r3, r2
60307618:	e1a02001 	mov	r2, r1
6030761c:	e1a01000 	mov	r1, r0
60307620:	e59f0004 	ldr	r0, [pc, #4]	; 6030762c <wcrtomb+0x18>
60307624:	e5900000 	ldr	r0, [r0]
60307628:	eaffffe8 	b	603075d0 <_wcrtomb_r>
6030762c:	60317000 	.word	0x60317000

60307630 <_fstat_r>:
60307630:	e92d4070 	push	{r4, r5, r6, lr}
60307634:	e1a04000 	mov	r4, r0
60307638:	e59f5028 	ldr	r5, [pc, #40]	; 60307668 <_fstat_r+0x38>
6030763c:	e1a00001 	mov	r0, r1
60307640:	e1a01002 	mov	r1, r2
60307644:	e3a03000 	mov	r3, #0
60307648:	e5853000 	str	r3, [r5]
6030764c:	eb000cfc 	bl	6030aa44 <_fstat>
60307650:	e3700001 	cmn	r0, #1
60307654:	18bd8070 	popne	{r4, r5, r6, pc}
60307658:	e5953000 	ldr	r3, [r5]
6030765c:	e3530000 	cmp	r3, #0
60307660:	15843000 	strne	r3, [r4]
60307664:	e8bd8070 	pop	{r4, r5, r6, pc}
60307668:	60317460 	.word	0x60317460

6030766c <_isatty_r>:
6030766c:	e92d4070 	push	{r4, r5, r6, lr}
60307670:	e1a04000 	mov	r4, r0
60307674:	e59f5024 	ldr	r5, [pc, #36]	; 603076a0 <_isatty_r+0x34>
60307678:	e1a00001 	mov	r0, r1
6030767c:	e3a03000 	mov	r3, #0
60307680:	e5853000 	str	r3, [r5]
60307684:	eb000ce6 	bl	6030aa24 <_isatty>
60307688:	e3700001 	cmn	r0, #1
6030768c:	18bd8070 	popne	{r4, r5, r6, pc}
60307690:	e5953000 	ldr	r3, [r5]
60307694:	e3530000 	cmp	r3, #0
60307698:	15843000 	strne	r3, [r4]
6030769c:	e8bd8070 	pop	{r4, r5, r6, pc}
603076a0:	60317460 	.word	0x60317460

603076a4 <__udivsi3>:
603076a4:	e2512001 	subs	r2, r1, #1
603076a8:	012fff1e 	bxeq	lr
603076ac:	3a000023 	bcc	60307740 <__udivsi3+0x9c>
603076b0:	e1500001 	cmp	r0, r1
603076b4:	9a00001a 	bls	60307724 <__udivsi3+0x80>
603076b8:	e1110002 	tst	r1, r2
603076bc:	0a00001b 	beq	60307730 <__udivsi3+0x8c>
603076c0:	e16f3f11 	clz	r3, r1
603076c4:	e16f2f10 	clz	r2, r0
603076c8:	e0432002 	sub	r2, r3, r2
603076cc:	e3a03001 	mov	r3, #1
603076d0:	e1a01211 	lsl	r1, r1, r2
603076d4:	e1a03213 	lsl	r3, r3, r2
603076d8:	e3a02000 	mov	r2, #0
603076dc:	e1500001 	cmp	r0, r1
603076e0:	20400001 	subcs	r0, r0, r1
603076e4:	21822003 	orrcs	r2, r2, r3
603076e8:	e15000a1 	cmp	r0, r1, lsr #1
603076ec:	204000a1 	subcs	r0, r0, r1, lsr #1
603076f0:	218220a3 	orrcs	r2, r2, r3, lsr #1
603076f4:	e1500121 	cmp	r0, r1, lsr #2
603076f8:	20400121 	subcs	r0, r0, r1, lsr #2
603076fc:	21822123 	orrcs	r2, r2, r3, lsr #2
60307700:	e15001a1 	cmp	r0, r1, lsr #3
60307704:	204001a1 	subcs	r0, r0, r1, lsr #3
60307708:	218221a3 	orrcs	r2, r2, r3, lsr #3
6030770c:	e3500000 	cmp	r0, #0
60307710:	11b03223 	lsrsne	r3, r3, #4
60307714:	11a01221 	lsrne	r1, r1, #4
60307718:	1affffef 	bne	603076dc <__udivsi3+0x38>
6030771c:	e1a00002 	mov	r0, r2
60307720:	e12fff1e 	bx	lr
60307724:	03a00001 	moveq	r0, #1
60307728:	13a00000 	movne	r0, #0
6030772c:	e12fff1e 	bx	lr
60307730:	e16f2f11 	clz	r2, r1
60307734:	e262201f 	rsb	r2, r2, #31
60307738:	e1a00230 	lsr	r0, r0, r2
6030773c:	e12fff1e 	bx	lr
60307740:	e3500000 	cmp	r0, #0
60307744:	13e00000 	mvnne	r0, #0
60307748:	eaffe4be 	b	60300a48 <__aeabi_idiv0>

6030774c <__aeabi_uidivmod>:
6030774c:	e3510000 	cmp	r1, #0
60307750:	0afffffa 	beq	60307740 <__udivsi3+0x9c>
60307754:	e92d4003 	push	{r0, r1, lr}
60307758:	ebffffd1 	bl	603076a4 <__udivsi3>
6030775c:	e8bd4006 	pop	{r1, r2, lr}
60307760:	e0030092 	mul	r3, r2, r0
60307764:	e0411003 	sub	r1, r1, r3
60307768:	e12fff1e 	bx	lr

6030776c <__divsi3>:
6030776c:	e3510000 	cmp	r1, #0
60307770:	0a000030 	beq	60307838 <.divsi3_skip_div0_test+0xc4>

60307774 <.divsi3_skip_div0_test>:
60307774:	e020c001 	eor	ip, r0, r1
60307778:	42611000 	rsbmi	r1, r1, #0
6030777c:	e2512001 	subs	r2, r1, #1
60307780:	0a00001f 	beq	60307804 <.divsi3_skip_div0_test+0x90>
60307784:	e1b03000 	movs	r3, r0
60307788:	42603000 	rsbmi	r3, r0, #0
6030778c:	e1530001 	cmp	r3, r1
60307790:	9a00001e 	bls	60307810 <.divsi3_skip_div0_test+0x9c>
60307794:	e1110002 	tst	r1, r2
60307798:	0a000020 	beq	60307820 <.divsi3_skip_div0_test+0xac>
6030779c:	e16f2f11 	clz	r2, r1
603077a0:	e16f0f13 	clz	r0, r3
603077a4:	e0420000 	sub	r0, r2, r0
603077a8:	e3a02001 	mov	r2, #1
603077ac:	e1a01011 	lsl	r1, r1, r0
603077b0:	e1a02012 	lsl	r2, r2, r0
603077b4:	e3a00000 	mov	r0, #0
603077b8:	e1530001 	cmp	r3, r1
603077bc:	20433001 	subcs	r3, r3, r1
603077c0:	21800002 	orrcs	r0, r0, r2
603077c4:	e15300a1 	cmp	r3, r1, lsr #1
603077c8:	204330a1 	subcs	r3, r3, r1, lsr #1
603077cc:	218000a2 	orrcs	r0, r0, r2, lsr #1
603077d0:	e1530121 	cmp	r3, r1, lsr #2
603077d4:	20433121 	subcs	r3, r3, r1, lsr #2
603077d8:	21800122 	orrcs	r0, r0, r2, lsr #2
603077dc:	e15301a1 	cmp	r3, r1, lsr #3
603077e0:	204331a1 	subcs	r3, r3, r1, lsr #3
603077e4:	218001a2 	orrcs	r0, r0, r2, lsr #3
603077e8:	e3530000 	cmp	r3, #0
603077ec:	11b02222 	lsrsne	r2, r2, #4
603077f0:	11a01221 	lsrne	r1, r1, #4
603077f4:	1affffef 	bne	603077b8 <.divsi3_skip_div0_test+0x44>
603077f8:	e35c0000 	cmp	ip, #0
603077fc:	42600000 	rsbmi	r0, r0, #0
60307800:	e12fff1e 	bx	lr
60307804:	e13c0000 	teq	ip, r0
60307808:	42600000 	rsbmi	r0, r0, #0
6030780c:	e12fff1e 	bx	lr
60307810:	33a00000 	movcc	r0, #0
60307814:	01a00fcc 	asreq	r0, ip, #31
60307818:	03800001 	orreq	r0, r0, #1
6030781c:	e12fff1e 	bx	lr
60307820:	e16f2f11 	clz	r2, r1
60307824:	e262201f 	rsb	r2, r2, #31
60307828:	e35c0000 	cmp	ip, #0
6030782c:	e1a00233 	lsr	r0, r3, r2
60307830:	42600000 	rsbmi	r0, r0, #0
60307834:	e12fff1e 	bx	lr
60307838:	e3500000 	cmp	r0, #0
6030783c:	c3e00102 	mvngt	r0, #-2147483648	; 0x80000000
60307840:	b3a00102 	movlt	r0, #-2147483648	; 0x80000000
60307844:	eaffe47f 	b	60300a48 <__aeabi_idiv0>

60307848 <__aeabi_idivmod>:
60307848:	e3510000 	cmp	r1, #0
6030784c:	0afffff9 	beq	60307838 <.divsi3_skip_div0_test+0xc4>
60307850:	e92d4003 	push	{r0, r1, lr}
60307854:	ebffffc6 	bl	60307774 <.divsi3_skip_div0_test>
60307858:	e8bd4006 	pop	{r1, r2, lr}
6030785c:	e0030092 	mul	r3, r2, r0
60307860:	e0411003 	sub	r1, r1, r3
60307864:	e12fff1e 	bx	lr

60307868 <_exit>:
60307868:	eafffffe 	b	60307868 <_exit>

6030786c <CmdRamHelp>:
6030786c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60307870:	e3065f34 	movw	r5, #28468	; 0x6f34
60307874:	e3465031 	movt	r5, #24625	; 0x6031
60307878:	e24dd008 	sub	sp, sp, #8
6030787c:	e3064ed4 	movw	r4, #28372	; 0x6ed4
60307880:	e3464031 	movt	r4, #24625	; 0x6031
60307884:	e0455004 	sub	r5, r5, r4
60307888:	e3a02041 	mov	r2, #65	; 0x41
6030788c:	e1a05225 	lsr	r5, r5, #4
60307890:	e3a00001 	mov	r0, #1
60307894:	e58d5000 	str	r5, [sp]
60307898:	e3053348 	movw	r3, #21320	; 0x5348
6030789c:	e3463031 	movt	r3, #24625	; 0x6031
603078a0:	e3051388 	movw	r1, #21384	; 0x5388
603078a4:	e3461031 	movt	r1, #24625	; 0x6031
603078a8:	eb001512 	bl	6030ccf8 <rtk_log_write_nano>
603078ac:	e3550000 	cmp	r5, #0
603078b0:	0a000010 	beq	603078f8 <CmdRamHelp+0x8c>
603078b4:	e3057390 	movw	r7, #21392	; 0x5390
603078b8:	e3467031 	movt	r7, #24625	; 0x6031
603078bc:	e3056394 	movw	r6, #21396	; 0x5394
603078c0:	e3466031 	movt	r6, #24625	; 0x6031
603078c4:	e0848205 	add	r8, r4, r5, lsl #4
603078c8:	e594c00c 	ldr	ip, [r4, #12]
603078cc:	e1a03007 	mov	r3, r7
603078d0:	e2844010 	add	r4, r4, #16
603078d4:	e3a02041 	mov	r2, #65	; 0x41
603078d8:	e1a01006 	mov	r1, r6
603078dc:	e3a00001 	mov	r0, #1
603078e0:	e35c0000 	cmp	ip, #0
603078e4:	0a000001 	beq	603078f0 <CmdRamHelp+0x84>
603078e8:	e58dc000 	str	ip, [sp]
603078ec:	eb001501 	bl	6030ccf8 <rtk_log_write_nano>
603078f0:	e1540008 	cmp	r4, r8
603078f4:	1afffff3 	bne	603078c8 <CmdRamHelp+0x5c>
603078f8:	e3a00001 	mov	r0, #1
603078fc:	e3a02041 	mov	r2, #65	; 0x41
60307900:	e58d5000 	str	r5, [sp]
60307904:	e3053398 	movw	r3, #21400	; 0x5398
60307908:	e3463031 	movt	r3, #24625	; 0x6031
6030790c:	e3051388 	movw	r1, #21384	; 0x5388
60307910:	e3461031 	movt	r1, #24625	; 0x6031
60307914:	eb0014f7 	bl	6030ccf8 <rtk_log_write_nano>
60307918:	e3a00001 	mov	r0, #1
6030791c:	e28dd008 	add	sp, sp, #8
60307920:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

60307924 <cmd_reboot>:
60307924:	e92d4070 	push	{r4, r5, r6, lr}
60307928:	e3a02041 	mov	r2, #65	; 0x41
6030792c:	e1a04000 	mov	r4, r0
60307930:	e1a05001 	mov	r5, r1
60307934:	e3a00001 	mov	r0, #1
60307938:	e30533d8 	movw	r3, #21464	; 0x53d8
6030793c:	e3463031 	movt	r3, #24625	; 0x6031
60307940:	e3051388 	movw	r1, #21384	; 0x5388
60307944:	e3461031 	movt	r1, #24625	; 0x6031
60307948:	eb0014ea 	bl	6030ccf8 <rtk_log_write_nano>
6030794c:	e3540000 	cmp	r4, #0
60307950:	1a000002 	bne	60307960 <cmd_reboot+0x3c>
60307954:	eb0008a4 	bl	60309bec <System_Reset>
60307958:	e3a00001 	mov	r0, #1
6030795c:	e8bd8070 	pop	{r4, r5, r6, pc}
60307960:	e5950000 	ldr	r0, [r5]
60307964:	e30513e8 	movw	r1, #21480	; 0x53e8
60307968:	e3461031 	movt	r1, #24625	; 0x6031
6030796c:	e3043a28 	movw	r3, #18984	; 0x4a28
60307970:	e3463031 	movt	r3, #24625	; 0x6031
60307974:	e12fff33 	blx	r3
60307978:	e2504000 	subs	r4, r0, #0
6030797c:	1afffff4 	bne	60307954 <cmd_reboot+0x30>
60307980:	e1a01004 	mov	r1, r4
60307984:	e3033f14 	movw	r3, #16148	; 0x3f14
60307988:	e3463031 	movt	r3, #24625	; 0x6031
6030798c:	e12fff33 	blx	r3
60307990:	e3a01c02 	mov	r1, #512	; 0x200
60307994:	e1a00004 	mov	r0, r4
60307998:	e3033edc 	movw	r3, #16092	; 0x3edc
6030799c:	e3463031 	movt	r3, #24625	; 0x6031
603079a0:	e12fff33 	blx	r3
603079a4:	eaffffea 	b	60307954 <cmd_reboot+0x30>

603079a8 <CmdTickPS>:
603079a8:	e92d4030 	push	{r4, r5, lr}
603079ac:	e1a05001 	mov	r5, r1
603079b0:	e24dd00c 	sub	sp, sp, #12
603079b4:	e30513f4 	movw	r1, #21492	; 0x53f4
603079b8:	e3461031 	movt	r1, #24625	; 0x6031
603079bc:	e5950000 	ldr	r0, [r5]
603079c0:	e3044a28 	movw	r4, #18984	; 0x4a28
603079c4:	e3464031 	movt	r4, #24625	; 0x6031
603079c8:	e12fff34 	blx	r4
603079cc:	e3500000 	cmp	r0, #0
603079d0:	0a000014 	beq	60307a28 <CmdTickPS+0x80>
603079d4:	e5950000 	ldr	r0, [r5]
603079d8:	e30513f8 	movw	r1, #21496	; 0x53f8
603079dc:	e3461031 	movt	r1, #24625	; 0x6031
603079e0:	e12fff34 	blx	r4
603079e4:	e3500000 	cmp	r0, #0
603079e8:	0a000039 	beq	60307ad4 <CmdTickPS+0x12c>
603079ec:	e5950000 	ldr	r0, [r5]
603079f0:	e30513fc 	movw	r1, #21500	; 0x53fc
603079f4:	e3461031 	movt	r1, #24625	; 0x6031
603079f8:	e12fff34 	blx	r4
603079fc:	e3500000 	cmp	r0, #0
60307a00:	0a000021 	beq	60307a8c <CmdTickPS+0xe4>
60307a04:	e5950000 	ldr	r0, [r5]
60307a08:	e305140c 	movw	r1, #21516	; 0x540c
60307a0c:	e3461031 	movt	r1, #24625	; 0x6031
60307a10:	e12fff34 	blx	r4
60307a14:	e3500000 	cmp	r0, #0
60307a18:	0a000004 	beq	60307a30 <CmdTickPS+0x88>
60307a1c:	e3a00001 	mov	r0, #1
60307a20:	e28dd00c 	add	sp, sp, #12
60307a24:	e8bd8030 	pop	{r4, r5, pc}
60307a28:	eb001797 	bl	6030d88c <pmu_release_wakelock>
60307a2c:	eaffffe8 	b	603079d4 <CmdTickPS+0x2c>
60307a30:	eb0017a0 	bl	6030d8b8 <pmu_get_wakelock_status>
60307a34:	e3a02041 	mov	r2, #65	; 0x41
60307a38:	e1a03000 	mov	r3, r0
60307a3c:	e3051388 	movw	r1, #21384	; 0x5388
60307a40:	e3461031 	movt	r1, #24625	; 0x6031
60307a44:	e58d3000 	str	r3, [sp]
60307a48:	e3a00001 	mov	r0, #1
60307a4c:	e3053410 	movw	r3, #21520	; 0x5410
60307a50:	e3463031 	movt	r3, #24625	; 0x6031
60307a54:	eb0014a7 	bl	6030ccf8 <rtk_log_write_nano>
60307a58:	eb001806 	bl	6030da78 <pmu_get_deepwakelock_status>
60307a5c:	e3a02041 	mov	r2, #65	; 0x41
60307a60:	e1a0c000 	mov	ip, r0
60307a64:	e3053420 	movw	r3, #21536	; 0x5420
60307a68:	e3463031 	movt	r3, #24625	; 0x6031
60307a6c:	e3a00001 	mov	r0, #1
60307a70:	e58dc000 	str	ip, [sp]
60307a74:	e3051388 	movw	r1, #21384	; 0x5388
60307a78:	e3461031 	movt	r1, #24625	; 0x6031
60307a7c:	eb00149d 	bl	6030ccf8 <rtk_log_write_nano>
60307a80:	e3a00001 	mov	r0, #1
60307a84:	e28dd00c 	add	sp, sp, #12
60307a88:	e8bd8030 	pop	{r4, r5, pc}
60307a8c:	e5950004 	ldr	r0, [r5, #4]
60307a90:	e3051404 	movw	r1, #21508	; 0x5404
60307a94:	e3461031 	movt	r1, #24625	; 0x6031
60307a98:	e12fff34 	blx	r4
60307a9c:	e3500000 	cmp	r0, #0
60307aa0:	0a000008 	beq	60307ac8 <CmdTickPS+0x120>
60307aa4:	e5950004 	ldr	r0, [r5, #4]
60307aa8:	e3051408 	movw	r1, #21512	; 0x5408
60307aac:	e3461031 	movt	r1, #24625	; 0x6031
60307ab0:	e12fff34 	blx	r4
60307ab4:	e3500000 	cmp	r0, #0
60307ab8:	1a000002 	bne	60307ac8 <CmdTickPS+0x120>
60307abc:	e3a00001 	mov	r0, #1
60307ac0:	eb001780 	bl	6030d8c8 <pmu_set_sleep_type>
60307ac4:	eaffffce 	b	60307a04 <CmdTickPS+0x5c>
60307ac8:	e3a00000 	mov	r0, #0
60307acc:	eb00177d 	bl	6030d8c8 <pmu_set_sleep_type>
60307ad0:	eaffffcb 	b	60307a04 <CmdTickPS+0x5c>
60307ad4:	eb001761 	bl	6030d860 <pmu_acquire_wakelock>
60307ad8:	eaffffc3 	b	603079ec <CmdTickPS+0x44>

60307adc <cmd_write_word>:
60307adc:	e92d40f0 	push	{r4, r5, r6, r7, lr}
60307ae0:	e1a07001 	mov	r7, r1
60307ae4:	e24dd00c 	sub	sp, sp, #12
60307ae8:	e3a02010 	mov	r2, #16
60307aec:	e5970000 	ldr	r0, [r7]
60307af0:	e3a01000 	mov	r1, #0
60307af4:	e3046c2c 	movw	r6, #19500	; 0x4c2c
60307af8:	e3466031 	movt	r6, #24625	; 0x6031
60307afc:	e12fff36 	blx	r6
60307b00:	e3c05003 	bic	r5, r0, #3
60307b04:	e1a04000 	mov	r4, r0
60307b08:	e3a02010 	mov	r2, #16
60307b0c:	e5970004 	ldr	r0, [r7, #4]
60307b10:	e3a01000 	mov	r1, #0
60307b14:	e12fff36 	blx	r6
60307b18:	e3a02041 	mov	r2, #65	; 0x41
60307b1c:	e1a06000 	mov	r6, r0
60307b20:	e58d0004 	str	r0, [sp, #4]
60307b24:	e58d5000 	str	r5, [sp]
60307b28:	e3a00001 	mov	r0, #1
60307b2c:	e3053434 	movw	r3, #21556	; 0x5434
60307b30:	e3463031 	movt	r3, #24625	; 0x6031
60307b34:	e3051394 	movw	r1, #21396	; 0x5394
60307b38:	e3461031 	movt	r1, #24625	; 0x6031
60307b3c:	eb00146d 	bl	6030ccf8 <rtk_log_write_nano>
60307b40:	e5856000 	str	r6, [r5]
60307b44:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
60307b48:	e3130004 	tst	r3, #4
60307b4c:	0a000009 	beq	60307b78 <cmd_write_word+0x9c>
60307b50:	e3c5303f 	bic	r3, r5, #63	; 0x3f
60307b54:	e3c4003f 	bic	r0, r4, #63	; 0x3f
60307b58:	e2833040 	add	r3, r3, #64	; 0x40
60307b5c:	e1500003 	cmp	r0, r3
60307b60:	2a000003 	bcs	60307b74 <cmd_write_word+0x98>
60307b64:	ee070f3a 	mcr	15, 0, r0, cr7, cr10, {1}
60307b68:	e2800040 	add	r0, r0, #64	; 0x40
60307b6c:	e1530000 	cmp	r3, r0
60307b70:	8afffffb 	bhi	60307b64 <cmd_write_word+0x88>
60307b74:	f57ff05f 	dmb	sy
60307b78:	e3a00000 	mov	r0, #0
60307b7c:	e28dd00c 	add	sp, sp, #12
60307b80:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}

60307b84 <cmd_dump_word>:
60307b84:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60307b88:	e30434dc 	movw	r3, #17628	; 0x44dc
60307b8c:	e3463031 	movt	r3, #24625	; 0x6031
60307b90:	e1a04000 	mov	r4, r0
60307b94:	e1a05001 	mov	r5, r1
60307b98:	e12fff33 	blx	r3
60307b9c:	e2443001 	sub	r3, r4, #1
60307ba0:	e3530002 	cmp	r3, #2
60307ba4:	8a000057 	bhi	60307d08 <cmd_dump_word+0x184>
60307ba8:	e1a06000 	mov	r6, r0
60307bac:	e3540003 	cmp	r4, #3
60307bb0:	0a000023 	beq	60307c44 <cmd_dump_word+0xc0>
60307bb4:	e3540001 	cmp	r4, #1
60307bb8:	1a00005b 	bne	60307d2c <cmd_dump_word+0x1a8>
60307bbc:	e3a02010 	mov	r2, #16
60307bc0:	e3a01000 	mov	r1, #0
60307bc4:	e5950000 	ldr	r0, [r5]
60307bc8:	e3043c2c 	movw	r3, #19500	; 0x4c2c
60307bcc:	e3463031 	movt	r3, #24625	; 0x6031
60307bd0:	e12fff33 	blx	r3
60307bd4:	e1a03000 	mov	r3, r0
60307bd8:	e3c00003 	bic	r0, r0, #3
60307bdc:	e2562000 	subs	r2, r6, #0
60307be0:	e280133e 	add	r1, r0, #-134217728	; 0xf8000000
60307be4:	13a02001 	movne	r2, #1
60307be8:	e371037e 	cmn	r1, #-134217727	; 0xf8000001
60307bec:	83a02000 	movhi	r2, #0
60307bf0:	e3520000 	cmp	r2, #0
60307bf4:	1a00006d 	bne	60307db0 <cmd_dump_word+0x22c>
60307bf8:	ee112f10 	mrc	15, 0, r2, cr1, cr0, {0}
60307bfc:	e3120004 	tst	r2, #4
60307c00:	0a00000b 	beq	60307c34 <cmd_dump_word+0xb0>
60307c04:	e0802104 	add	r2, r0, r4, lsl #2
60307c08:	e2422001 	sub	r2, r2, #1
60307c0c:	e3c2203f 	bic	r2, r2, #63	; 0x3f
60307c10:	e3c3303f 	bic	r3, r3, #63	; 0x3f
60307c14:	e2822040 	add	r2, r2, #64	; 0x40
60307c18:	e1530002 	cmp	r3, r2
60307c1c:	2a000003 	bcs	60307c30 <cmd_dump_word+0xac>
60307c20:	ee073f3e 	mcr	15, 0, r3, cr7, cr14, {1}
60307c24:	e2833040 	add	r3, r3, #64	; 0x40
60307c28:	e1520003 	cmp	r2, r3
60307c2c:	8afffffb 	bhi	60307c20 <cmd_dump_word+0x9c>
60307c30:	f57ff05f 	dmb	sy
60307c34:	e1a01004 	mov	r1, r4
60307c38:	eb001452 	bl	6030cd88 <rtk_log_memory_dump_word>
60307c3c:	e3a00001 	mov	r0, #1
60307c40:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60307c44:	e5953008 	ldr	r3, [r5, #8]
60307c48:	e3a0200a 	mov	r2, #10
60307c4c:	e3a01000 	mov	r1, #0
60307c50:	e5d37000 	ldrb	r7, [r3]
60307c54:	e5950004 	ldr	r0, [r5, #4]
60307c58:	e3048c2c 	movw	r8, #19500	; 0x4c2c
60307c5c:	e3468031 	movt	r8, #24625	; 0x6031
60307c60:	e12fff38 	blx	r8
60307c64:	e3a02010 	mov	r2, #16
60307c68:	e3a01000 	mov	r1, #0
60307c6c:	e1a04000 	mov	r4, r0
60307c70:	e5950000 	ldr	r0, [r5]
60307c74:	e12fff38 	blx	r8
60307c78:	e20770df 	and	r7, r7, #223	; 0xdf
60307c7c:	e1a03000 	mov	r3, r0
60307c80:	e3c00003 	bic	r0, r0, #3
60307c84:	e2562000 	subs	r2, r6, #0
60307c88:	e2477042 	sub	r7, r7, #66	; 0x42
60307c8c:	e280133e 	add	r1, r0, #-134217728	; 0xf8000000
60307c90:	13a02001 	movne	r2, #1
60307c94:	e16f7f17 	clz	r7, r7
60307c98:	e371037e 	cmn	r1, #-134217727	; 0xf8000001
60307c9c:	83a02000 	movhi	r2, #0
60307ca0:	e1a072a7 	lsr	r7, r7, #5
60307ca4:	e3520000 	cmp	r2, #0
60307ca8:	1a000040 	bne	60307db0 <cmd_dump_word+0x22c>
60307cac:	e2545000 	subs	r5, r4, #0
60307cb0:	13a05001 	movne	r5, #1
60307cb4:	e1150007 	tst	r5, r7
60307cb8:	0a000031 	beq	60307d84 <cmd_dump_word+0x200>
60307cbc:	ee112f10 	mrc	15, 0, r2, cr1, cr0, {0}
60307cc0:	e3120004 	tst	r2, #4
60307cc4:	0a00000b 	beq	60307cf8 <cmd_dump_word+0x174>
60307cc8:	e2442001 	sub	r2, r4, #1
60307ccc:	e0822000 	add	r2, r2, r0
60307cd0:	e3c2203f 	bic	r2, r2, #63	; 0x3f
60307cd4:	e3c3303f 	bic	r3, r3, #63	; 0x3f
60307cd8:	e2822040 	add	r2, r2, #64	; 0x40
60307cdc:	e1530002 	cmp	r3, r2
60307ce0:	2a000003 	bcs	60307cf4 <cmd_dump_word+0x170>
60307ce4:	ee073f3e 	mcr	15, 0, r3, cr7, cr14, {1}
60307ce8:	e2833040 	add	r3, r3, #64	; 0x40
60307cec:	e1520003 	cmp	r2, r3
60307cf0:	8afffffb 	bhi	60307ce4 <cmd_dump_word+0x160>
60307cf4:	f57ff05f 	dmb	sy
60307cf8:	e1a01004 	mov	r1, r4
60307cfc:	eb001454 	bl	6030ce54 <rtk_log_memory_dump_byte>
60307d00:	e3a00001 	mov	r0, #1
60307d04:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60307d08:	e3a02045 	mov	r2, #69	; 0x45
60307d0c:	e3a00002 	mov	r0, #2
60307d10:	e3053444 	movw	r3, #21572	; 0x5444
60307d14:	e3463031 	movt	r3, #24625	; 0x6031
60307d18:	e3051388 	movw	r1, #21384	; 0x5388
60307d1c:	e3461031 	movt	r1, #24625	; 0x6031
60307d20:	eb0013f4 	bl	6030ccf8 <rtk_log_write_nano>
60307d24:	e3a00000 	mov	r0, #0
60307d28:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60307d2c:	e3a0200a 	mov	r2, #10
60307d30:	e3a01000 	mov	r1, #0
60307d34:	e5950004 	ldr	r0, [r5, #4]
60307d38:	e3047c2c 	movw	r7, #19500	; 0x4c2c
60307d3c:	e3467031 	movt	r7, #24625	; 0x6031
60307d40:	e12fff37 	blx	r7
60307d44:	e3a02010 	mov	r2, #16
60307d48:	e3a01000 	mov	r1, #0
60307d4c:	e1a04000 	mov	r4, r0
60307d50:	e5950000 	ldr	r0, [r5]
60307d54:	e12fff37 	blx	r7
60307d58:	e1a03000 	mov	r3, r0
60307d5c:	e3c00003 	bic	r0, r0, #3
60307d60:	e2562000 	subs	r2, r6, #0
60307d64:	e280133e 	add	r1, r0, #-134217728	; 0xf8000000
60307d68:	13a02001 	movne	r2, #1
60307d6c:	e371037e 	cmn	r1, #-134217727	; 0xf8000001
60307d70:	83a02000 	movhi	r2, #0
60307d74:	e3520000 	cmp	r2, #0
60307d78:	1a00000c 	bne	60307db0 <cmd_dump_word+0x22c>
60307d7c:	e2545000 	subs	r5, r4, #0
60307d80:	13a05001 	movne	r5, #1
60307d84:	e3550000 	cmp	r5, #0
60307d88:	1affff9a 	bne	60307bf8 <cmd_dump_word+0x74>
60307d8c:	e3a02045 	mov	r2, #69	; 0x45
60307d90:	e3a00002 	mov	r0, #2
60307d94:	e3053484 	movw	r3, #21636	; 0x5484
60307d98:	e3463031 	movt	r3, #24625	; 0x6031
60307d9c:	e3051388 	movw	r1, #21384	; 0x5388
60307da0:	e3461031 	movt	r1, #24625	; 0x6031
60307da4:	eb0013d3 	bl	6030ccf8 <rtk_log_write_nano>
60307da8:	e1a00005 	mov	r0, r5
60307dac:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60307db0:	e3a02045 	mov	r2, #69	; 0x45
60307db4:	e3a00002 	mov	r0, #2
60307db8:	e3053460 	movw	r3, #21600	; 0x5460
60307dbc:	e3463031 	movt	r3, #24625	; 0x6031
60307dc0:	e3051388 	movw	r1, #21384	; 0x5388
60307dc4:	e3461031 	movt	r1, #24625	; 0x6031
60307dc8:	eb0013ca 	bl	6030ccf8 <rtk_log_write_nano>
60307dcc:	e3a00001 	mov	r0, #1
60307dd0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

60307dd4 <cmd_efuse_protect>:
60307dd4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60307dd8:	e1a0b000 	mov	fp, r0
60307ddc:	e24dd01c 	sub	sp, sp, #28
60307de0:	e3a00b02 	mov	r0, #2048	; 0x800
60307de4:	e1a05001 	mov	r5, r1
60307de8:	e58db008 	str	fp, [sp, #8]
60307dec:	eb001543 	bl	6030d300 <rtos_mem_zmalloc>
60307df0:	e2504000 	subs	r4, r0, #0
60307df4:	0a000152 	beq	60308344 <cmd_efuse_protect+0x570>
60307df8:	e5950000 	ldr	r0, [r5]
60307dfc:	e30514ac 	movw	r1, #21676	; 0x54ac
60307e00:	e3461031 	movt	r1, #24625	; 0x6031
60307e04:	e304ba28 	movw	fp, #18984	; 0x4a28
60307e08:	e346b031 	movt	fp, #24625	; 0x6031
60307e0c:	e12fff3b 	blx	fp
60307e10:	e2508000 	subs	r8, r0, #0
60307e14:	1a000030 	bne	60307edc <cmd_efuse_protect+0x108>
60307e18:	e59d3008 	ldr	r3, [sp, #8]
60307e1c:	e3530002 	cmp	r3, #2
60307e20:	8a00010f 	bhi	60308264 <cmd_efuse_protect+0x490>
60307e24:	e3a02b01 	mov	r2, #1024	; 0x400
60307e28:	e1a01008 	mov	r1, r8
60307e2c:	e1a00004 	mov	r0, r4
60307e30:	eb000582 	bl	60309440 <OTP_LogicalMap_Read>
60307e34:	e3700001 	cmn	r0, #1
60307e38:	0a000186 	beq	60308458 <cmd_efuse_protect+0x684>
60307e3c:	e3a09b01 	mov	r9, #1024	; 0x400
60307e40:	e30534d8 	movw	r3, #21720	; 0x54d8
60307e44:	e3463031 	movt	r3, #24625	; 0x6031
60307e48:	e305a394 	movw	sl, #21396	; 0x5394
60307e4c:	e346a031 	movt	sl, #24625	; 0x6031
60307e50:	e2447001 	sub	r7, r4, #1
60307e54:	e58d400c 	str	r4, [sp, #12]
60307e58:	e3a06000 	mov	r6, #0
60307e5c:	e1a0400a 	mov	r4, sl
60307e60:	e1a0a003 	mov	sl, r3
60307e64:	ea000009 	b	60307e90 <cmd_efuse_protect+0xbc>
60307e68:	e5f73001 	ldrb	r3, [r7, #1]!
60307e6c:	e58d3000 	str	r3, [sp]
60307e70:	e3a02049 	mov	r2, #73	; 0x49
60307e74:	e1a01004 	mov	r1, r4
60307e78:	e3a00004 	mov	r0, #4
60307e7c:	e30534e8 	movw	r3, #21736	; 0x54e8
60307e80:	e3463031 	movt	r3, #24625	; 0x6031
60307e84:	eb001377 	bl	6030cc68 <rtk_log_write>
60307e88:	e1560009 	cmp	r6, r9
60307e8c:	0a00000a 	beq	60307ebc <cmd_efuse_protect+0xe8>
60307e90:	e316000f 	tst	r6, #15
60307e94:	e0882006 	add	r2, r8, r6
60307e98:	e2866001 	add	r6, r6, #1
60307e9c:	1afffff1 	bne	60307e68 <cmd_efuse_protect+0x94>
60307ea0:	e58d2000 	str	r2, [sp]
60307ea4:	e1a0300a 	mov	r3, sl
60307ea8:	e3a02049 	mov	r2, #73	; 0x49
60307eac:	e1a01004 	mov	r1, r4
60307eb0:	e3a00004 	mov	r0, #4
60307eb4:	eb00136b 	bl	6030cc68 <rtk_log_write>
60307eb8:	eaffffea 	b	60307e68 <cmd_efuse_protect+0x94>
60307ebc:	e59d400c 	ldr	r4, [sp, #12]
60307ec0:	e30534b4 	movw	r3, #21684	; 0x54b4
60307ec4:	e3463031 	movt	r3, #24625	; 0x6031
60307ec8:	e3051394 	movw	r1, #21396	; 0x5394
60307ecc:	e3461031 	movt	r1, #24625	; 0x6031
60307ed0:	e3a02049 	mov	r2, #73	; 0x49
60307ed4:	e3a00004 	mov	r0, #4
60307ed8:	eb001362 	bl	6030cc68 <rtk_log_write>
60307edc:	e5950000 	ldr	r0, [r5]
60307ee0:	e30514f0 	movw	r1, #21744	; 0x54f0
60307ee4:	e3461031 	movt	r1, #24625	; 0x6031
60307ee8:	e12fff3b 	blx	fp
60307eec:	e2506000 	subs	r6, r0, #0
60307ef0:	1a0000bf 	bne	603081f4 <cmd_efuse_protect+0x420>
60307ef4:	e59d3008 	ldr	r3, [sp, #8]
60307ef8:	e3530003 	cmp	r3, #3
60307efc:	9a0000b0 	bls	603081c4 <cmd_efuse_protect+0x3f0>
60307f00:	e1a01006 	mov	r1, r6
60307f04:	e3a02010 	mov	r2, #16
60307f08:	e5950004 	ldr	r0, [r5, #4]
60307f0c:	e3047c2c 	movw	r7, #19500	; 0x4c2c
60307f10:	e3467031 	movt	r7, #24625	; 0x6031
60307f14:	e12fff37 	blx	r7
60307f18:	e1a01006 	mov	r1, r6
60307f1c:	e3a02010 	mov	r2, #16
60307f20:	e58d0010 	str	r0, [sp, #16]
60307f24:	e5950008 	ldr	r0, [r5, #8]
60307f28:	e12fff37 	blx	r7
60307f2c:	e595600c 	ldr	r6, [r5, #12]
60307f30:	e1a0a000 	mov	sl, r0
60307f34:	e3043b94 	movw	r3, #19348	; 0x4b94
60307f38:	e3463031 	movt	r3, #24625	; 0x6031
60307f3c:	e1a00006 	mov	r0, r6
60307f40:	e12fff33 	blx	r3
60307f44:	e1a08000 	mov	r8, r0
60307f48:	e3100001 	tst	r0, #1
60307f4c:	1a000138 	bne	60308434 <cmd_efuse_protect+0x660>
60307f50:	e1a030a0 	lsr	r3, r0, #1
60307f54:	e1a09003 	mov	r9, r3
60307f58:	e15a0003 	cmp	sl, r3
60307f5c:	e58d300c 	str	r3, [sp, #12]
60307f60:	1a0000d5 	bne	603082bc <cmd_efuse_protect+0x4e8>
60307f64:	e3c88001 	bic	r8, r8, #1
60307f68:	e58d8004 	str	r8, [sp, #4]
60307f6c:	e59d800c 	ldr	r8, [sp, #12]
60307f70:	e3a02049 	mov	r2, #73	; 0x49
60307f74:	e3a00004 	mov	r0, #4
60307f78:	e58d8000 	str	r8, [sp]
60307f7c:	e3053570 	movw	r3, #21872	; 0x5570
60307f80:	e3463031 	movt	r3, #24625	; 0x6031
60307f84:	e3051388 	movw	r1, #21384	; 0x5388
60307f88:	e3461031 	movt	r1, #24625	; 0x6031
60307f8c:	eb001335 	bl	6030cc68 <rtk_log_write>
60307f90:	e3580000 	cmp	r8, #0
60307f94:	0a00000a 	beq	60307fc4 <cmd_efuse_protect+0x1f0>
60307f98:	e59d300c 	ldr	r3, [sp, #12]
60307f9c:	e30486f4 	movw	r8, #18164	; 0x46f4
60307fa0:	e3468031 	movt	r8, #24625	; 0x6031
60307fa4:	e2449001 	sub	r9, r4, #1
60307fa8:	e086a083 	add	sl, r6, r3, lsl #1
60307fac:	e5d61001 	ldrb	r1, [r6, #1]
60307fb0:	e4d60002 	ldrb	r0, [r6], #2
60307fb4:	e12fff38 	blx	r8
60307fb8:	e156000a 	cmp	r6, sl
60307fbc:	e5e90001 	strb	r0, [r9, #1]!
60307fc0:	1afffff9 	bne	60307fac <cmd_efuse_protect+0x1d8>
60307fc4:	e59d100c 	ldr	r1, [sp, #12]
60307fc8:	e59d0010 	ldr	r0, [sp, #16]
60307fcc:	e1a02004 	mov	r2, r4
60307fd0:	eb000587 	bl	603095f4 <OTP_LogicalMap_Write>
60307fd4:	e3700001 	cmn	r0, #1
60307fd8:	0a00011e 	beq	60308458 <cmd_efuse_protect+0x684>
60307fdc:	e5950000 	ldr	r0, [r5]
60307fe0:	e305159c 	movw	r1, #21916	; 0x559c
60307fe4:	e3461031 	movt	r1, #24625	; 0x6031
60307fe8:	e12fff3b 	blx	fp
60307fec:	e3500000 	cmp	r0, #0
60307ff0:	0a0000bf 	beq	603082f4 <cmd_efuse_protect+0x520>
60307ff4:	e5950000 	ldr	r0, [r5]
60307ff8:	e30515cc 	movw	r1, #21964	; 0x55cc
60307ffc:	e3461031 	movt	r1, #24625	; 0x6031
60308000:	e12fff3b 	blx	fp
60308004:	e2506000 	subs	r6, r0, #0
60308008:	1a0000f8 	bne	603083f0 <cmd_efuse_protect+0x61c>
6030800c:	e59d3008 	ldr	r3, [sp, #8]
60308010:	e3530003 	cmp	r3, #3
60308014:	9a00006a 	bls	603081c4 <cmd_efuse_protect+0x3f0>
60308018:	e1a01006 	mov	r1, r6
6030801c:	e3a02010 	mov	r2, #16
60308020:	e5950004 	ldr	r0, [r5, #4]
60308024:	e3047c2c 	movw	r7, #19500	; 0x4c2c
60308028:	e3467031 	movt	r7, #24625	; 0x6031
6030802c:	e12fff37 	blx	r7
60308030:	e1a01006 	mov	r1, r6
60308034:	e3a02010 	mov	r2, #16
60308038:	e58d000c 	str	r0, [sp, #12]
6030803c:	e5950008 	ldr	r0, [r5, #8]
60308040:	e12fff37 	blx	r7
60308044:	e595600c 	ldr	r6, [r5, #12]
60308048:	e1a0a000 	mov	sl, r0
6030804c:	e3043b94 	movw	r3, #19348	; 0x4b94
60308050:	e3463031 	movt	r3, #24625	; 0x6031
60308054:	e1a00006 	mov	r0, r6
60308058:	e12fff33 	blx	r3
6030805c:	e1a08000 	mov	r8, r0
60308060:	e3100001 	tst	r0, #1
60308064:	1a0000f2 	bne	60308434 <cmd_efuse_protect+0x660>
60308068:	e1a030a0 	lsr	r3, r0, #1
6030806c:	e1a09003 	mov	r9, r3
60308070:	e15a0003 	cmp	sl, r3
60308074:	e58d3008 	str	r3, [sp, #8]
60308078:	0a00000a 	beq	603080a8 <cmd_efuse_protect+0x2d4>
6030807c:	e3a02057 	mov	r2, #87	; 0x57
60308080:	e3a00003 	mov	r0, #3
60308084:	e305352c 	movw	r3, #21804	; 0x552c
60308088:	e3463031 	movt	r3, #24625	; 0x6031
6030808c:	e3051388 	movw	r1, #21384	; 0x5388
60308090:	e3461031 	movt	r1, #24625	; 0x6031
60308094:	eb0012f3 	bl	6030cc68 <rtk_log_write>
60308098:	e159000a 	cmp	r9, sl
6030809c:	e1a03009 	mov	r3, r9
603080a0:	21a0300a 	movcs	r3, sl
603080a4:	e58d3008 	str	r3, [sp, #8]
603080a8:	e3c88001 	bic	r8, r8, #1
603080ac:	e58d8004 	str	r8, [sp, #4]
603080b0:	e59d8008 	ldr	r8, [sp, #8]
603080b4:	e3a02049 	mov	r2, #73	; 0x49
603080b8:	e3a00004 	mov	r0, #4
603080bc:	e58d8000 	str	r8, [sp]
603080c0:	e30535d4 	movw	r3, #21972	; 0x55d4
603080c4:	e3463031 	movt	r3, #24625	; 0x6031
603080c8:	e3051388 	movw	r1, #21384	; 0x5388
603080cc:	e3461031 	movt	r1, #24625	; 0x6031
603080d0:	eb0012e4 	bl	6030cc68 <rtk_log_write>
603080d4:	e1a03008 	mov	r3, r8
603080d8:	e3580000 	cmp	r8, #0
603080dc:	0a0000e7 	beq	60308480 <cmd_efuse_protect+0x6ac>
603080e0:	e1a0a006 	mov	sl, r6
603080e4:	e2449001 	sub	r9, r4, #1
603080e8:	e08a3083 	add	r3, sl, r3, lsl #1
603080ec:	e30486f4 	movw	r8, #18164	; 0x46f4
603080f0:	e3468031 	movt	r8, #24625	; 0x6031
603080f4:	e1a06009 	mov	r6, r9
603080f8:	e58d9010 	str	r9, [sp, #16]
603080fc:	e58d4014 	str	r4, [sp, #20]
60308100:	e1a04009 	mov	r4, r9
60308104:	e1a09008 	mov	r9, r8
60308108:	e1a08003 	mov	r8, r3
6030810c:	e5da1001 	ldrb	r1, [sl, #1]
60308110:	e4da0002 	ldrb	r0, [sl], #2
60308114:	e12fff39 	blx	r9
60308118:	e15a0008 	cmp	sl, r8
6030811c:	e5e40001 	strb	r0, [r4, #1]!
60308120:	1afffff9 	bne	6030810c <cmd_efuse_protect+0x338>
60308124:	e59d3010 	ldr	r3, [sp, #16]
60308128:	e59d2008 	ldr	r2, [sp, #8]
6030812c:	e59d4014 	ldr	r4, [sp, #20]
60308130:	e59d100c 	ldr	r1, [sp, #12]
60308134:	e0832002 	add	r2, r3, r2
60308138:	e3059600 	movw	r9, #22016	; 0x5600
6030813c:	e3469031 	movt	r9, #24625	; 0x6031
60308140:	e2643001 	rsb	r3, r4, #1
60308144:	e305a394 	movw	sl, #21396	; 0x5394
60308148:	e346a031 	movt	sl, #24625	; 0x6031
6030814c:	e58d700c 	str	r7, [sp, #12]
60308150:	e0838001 	add	r8, r3, r1
60308154:	e1a07009 	mov	r7, r9
60308158:	e58d5008 	str	r5, [sp, #8]
6030815c:	e1a09002 	mov	r9, r2
60308160:	ea000001 	b	6030816c <cmd_efuse_protect+0x398>
60308164:	e1590006 	cmp	r9, r6
60308168:	0a0000c2 	beq	60308478 <cmd_efuse_protect+0x6a4>
6030816c:	e0885006 	add	r5, r8, r6
60308170:	e5f62001 	ldrb	r2, [r6, #1]!
60308174:	e1a03007 	mov	r3, r7
60308178:	e1a0100a 	mov	r1, sl
6030817c:	e3a00004 	mov	r0, #4
60308180:	e58d2004 	str	r2, [sp, #4]
60308184:	e58d5000 	str	r5, [sp]
60308188:	e3a02049 	mov	r2, #73	; 0x49
6030818c:	eb0012b5 	bl	6030cc68 <rtk_log_write>
60308190:	e5d61000 	ldrb	r1, [r6]
60308194:	e1a00005 	mov	r0, r5
60308198:	eb0003fc 	bl	60309190 <OTP_Write8>
6030819c:	e3700001 	cmn	r0, #1
603081a0:	1affffef 	bne	60308164 <cmd_efuse_protect+0x390>
603081a4:	e3a02045 	mov	r2, #69	; 0x45
603081a8:	e3a00002 	mov	r0, #2
603081ac:	e3053610 	movw	r3, #22032	; 0x5610
603081b0:	e3463031 	movt	r3, #24625	; 0x6031
603081b4:	e3051388 	movw	r1, #21384	; 0x5388
603081b8:	e3461031 	movt	r1, #24625	; 0x6031
603081bc:	eb0012a9 	bl	6030cc68 <rtk_log_write>
603081c0:	ea000006 	b	603081e0 <cmd_efuse_protect+0x40c>
603081c4:	e30534f8 	movw	r3, #21752	; 0x54f8
603081c8:	e3463031 	movt	r3, #24625	; 0x6031
603081cc:	e3051388 	movw	r1, #21384	; 0x5388
603081d0:	e3461031 	movt	r1, #24625	; 0x6031
603081d4:	e3a02045 	mov	r2, #69	; 0x45
603081d8:	e3a00002 	mov	r0, #2
603081dc:	eb0012a1 	bl	6030cc68 <rtk_log_write>
603081e0:	e1a00004 	mov	r0, r4
603081e4:	eb00145b 	bl	6030d358 <rtos_mem_free>
603081e8:	e3a00000 	mov	r0, #0
603081ec:	e28dd01c 	add	sp, sp, #28
603081f0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
603081f4:	e5950000 	ldr	r0, [r5]
603081f8:	e305159c 	movw	r1, #21916	; 0x559c
603081fc:	e3461031 	movt	r1, #24625	; 0x6031
60308200:	e12fff3b 	blx	fp
60308204:	e2506000 	subs	r6, r0, #0
60308208:	1affff79 	bne	60307ff4 <cmd_efuse_protect+0x220>
6030820c:	e59d3008 	ldr	r3, [sp, #8]
60308210:	e3530002 	cmp	r3, #2
60308214:	93a07b02 	movls	r7, #2048	; 0x800
60308218:	8a000033 	bhi	603082ec <cmd_efuse_protect+0x518>
6030821c:	e1a08006 	mov	r8, r6
60308220:	ea000001 	b	6030822c <cmd_efuse_protect+0x458>
60308224:	e1580007 	cmp	r8, r7
60308228:	2a00004f 	bcs	6030836c <cmd_efuse_protect+0x598>
6030822c:	e0841008 	add	r1, r4, r8
60308230:	e1a00008 	mov	r0, r8
60308234:	eb0003cf 	bl	60309178 <OTP_Read8>
60308238:	e2888001 	add	r8, r8, #1
6030823c:	e3700001 	cmn	r0, #1
60308240:	1afffff7 	bne	60308224 <cmd_efuse_protect+0x450>
60308244:	e3a02045 	mov	r2, #69	; 0x45
60308248:	e3a00002 	mov	r0, #2
6030824c:	e30535a4 	movw	r3, #21924	; 0x55a4
60308250:	e3463031 	movt	r3, #24625	; 0x6031
60308254:	e3051388 	movw	r1, #21384	; 0x5388
60308258:	e3461031 	movt	r1, #24625	; 0x6031
6030825c:	eb001281 	bl	6030cc68 <rtk_log_write>
60308260:	eaffffde 	b	603081e0 <cmd_efuse_protect+0x40c>
60308264:	e1a01008 	mov	r1, r8
60308268:	e3a02010 	mov	r2, #16
6030826c:	e5950004 	ldr	r0, [r5, #4]
60308270:	e3047c2c 	movw	r7, #19500	; 0x4c2c
60308274:	e3467031 	movt	r7, #24625	; 0x6031
60308278:	e12fff37 	blx	r7
6030827c:	e1a01008 	mov	r1, r8
60308280:	e3a02010 	mov	r2, #16
60308284:	e1a08000 	mov	r8, r0
60308288:	e5950008 	ldr	r0, [r5, #8]
6030828c:	e12fff37 	blx	r7
60308290:	e1a01008 	mov	r1, r8
60308294:	e1a02000 	mov	r2, r0
60308298:	e1a09000 	mov	r9, r0
6030829c:	e1a00004 	mov	r0, r4
603082a0:	eb000466 	bl	60309440 <OTP_LogicalMap_Read>
603082a4:	e3700001 	cmn	r0, #1
603082a8:	0a00006a 	beq	60308458 <cmd_efuse_protect+0x684>
603082ac:	e0883009 	add	r3, r8, r9
603082b0:	e1580003 	cmp	r8, r3
603082b4:	3afffee1 	bcc	60307e40 <cmd_efuse_protect+0x6c>
603082b8:	eaffff00 	b	60307ec0 <cmd_efuse_protect+0xec>
603082bc:	e3a02057 	mov	r2, #87	; 0x57
603082c0:	e3a00003 	mov	r0, #3
603082c4:	e305352c 	movw	r3, #21804	; 0x552c
603082c8:	e3463031 	movt	r3, #24625	; 0x6031
603082cc:	e3051388 	movw	r1, #21384	; 0x5388
603082d0:	e3461031 	movt	r1, #24625	; 0x6031
603082d4:	eb001263 	bl	6030cc68 <rtk_log_write>
603082d8:	e159000a 	cmp	r9, sl
603082dc:	e1a03009 	mov	r3, r9
603082e0:	21a0300a 	movcs	r3, sl
603082e4:	e58d300c 	str	r3, [sp, #12]
603082e8:	eaffff1d 	b	60307f64 <cmd_efuse_protect+0x190>
603082ec:	e3047c2c 	movw	r7, #19500	; 0x4c2c
603082f0:	e3467031 	movt	r7, #24625	; 0x6031
603082f4:	e3a02010 	mov	r2, #16
603082f8:	e3a01000 	mov	r1, #0
603082fc:	e5950004 	ldr	r0, [r5, #4]
60308300:	e12fff37 	blx	r7
60308304:	e3a02010 	mov	r2, #16
60308308:	e1a06000 	mov	r6, r0
6030830c:	e3a01000 	mov	r1, #0
60308310:	e5950008 	ldr	r0, [r5, #8]
60308314:	e12fff37 	blx	r7
60308318:	e0867000 	add	r7, r6, r0
6030831c:	e1560007 	cmp	r6, r7
60308320:	3affffbd 	bcc	6030821c <cmd_efuse_protect+0x448>
60308324:	e3a02049 	mov	r2, #73	; 0x49
60308328:	e3a00004 	mov	r0, #4
6030832c:	e30534b4 	movw	r3, #21684	; 0x54b4
60308330:	e3463031 	movt	r3, #24625	; 0x6031
60308334:	e3051394 	movw	r1, #21396	; 0x5394
60308338:	e3461031 	movt	r1, #24625	; 0x6031
6030833c:	eb001249 	bl	6030cc68 <rtk_log_write>
60308340:	eaffff2b 	b	60307ff4 <cmd_efuse_protect+0x220>
60308344:	e3a02045 	mov	r2, #69	; 0x45
60308348:	e3a00002 	mov	r0, #2
6030834c:	e3053494 	movw	r3, #21652	; 0x5494
60308350:	e3463031 	movt	r3, #24625	; 0x6031
60308354:	e3051388 	movw	r1, #21384	; 0x5388
60308358:	e3461031 	movt	r1, #24625	; 0x6031
6030835c:	eb001265 	bl	6030ccf8 <rtk_log_write_nano>
60308360:	e3a00000 	mov	r0, #0
60308364:	e28dd01c 	add	sp, sp, #28
60308368:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
6030836c:	e0849006 	add	r9, r4, r6
60308370:	e30535b8 	movw	r3, #21944	; 0x55b8
60308374:	e3463031 	movt	r3, #24625	; 0x6031
60308378:	e305a394 	movw	sl, #21396	; 0x5394
6030837c:	e346a031 	movt	sl, #24625	; 0x6031
60308380:	e58d400c 	str	r4, [sp, #12]
60308384:	e1a08009 	mov	r8, r9
60308388:	e1a0400a 	mov	r4, sl
6030838c:	e1a0a003 	mov	sl, r3
60308390:	ea00000a 	b	603083c0 <cmd_efuse_protect+0x5ec>
60308394:	e4d83001 	ldrb	r3, [r8], #1
60308398:	e58d3000 	str	r3, [sp]
6030839c:	e3a02049 	mov	r2, #73	; 0x49
603083a0:	e1a01004 	mov	r1, r4
603083a4:	e2866001 	add	r6, r6, #1
603083a8:	e3a00004 	mov	r0, #4
603083ac:	e30534e8 	movw	r3, #21736	; 0x54e8
603083b0:	e3463031 	movt	r3, #24625	; 0x6031
603083b4:	eb00122b 	bl	6030cc68 <rtk_log_write>
603083b8:	e1560007 	cmp	r6, r7
603083bc:	2a000009 	bcs	603083e8 <cmd_efuse_protect+0x614>
603083c0:	e0483009 	sub	r3, r8, r9
603083c4:	e313000f 	tst	r3, #15
603083c8:	1afffff1 	bne	60308394 <cmd_efuse_protect+0x5c0>
603083cc:	e1a0300a 	mov	r3, sl
603083d0:	e3a02049 	mov	r2, #73	; 0x49
603083d4:	e1a01004 	mov	r1, r4
603083d8:	e3a00004 	mov	r0, #4
603083dc:	e58d6000 	str	r6, [sp]
603083e0:	eb001220 	bl	6030cc68 <rtk_log_write>
603083e4:	eaffffea 	b	60308394 <cmd_efuse_protect+0x5c0>
603083e8:	e59d400c 	ldr	r4, [sp, #12]
603083ec:	eaffffcc 	b	60308324 <cmd_efuse_protect+0x550>
603083f0:	e5950000 	ldr	r0, [r5]
603083f4:	e3051624 	movw	r1, #22052	; 0x5624
603083f8:	e3461031 	movt	r1, #24625	; 0x6031
603083fc:	e12fff3b 	blx	fp
60308400:	e3500000 	cmp	r0, #0
60308404:	1affff75 	bne	603081e0 <cmd_efuse_protect+0x40c>
60308408:	e59d3008 	ldr	r3, [sp, #8]
6030840c:	e3530001 	cmp	r3, #1
60308410:	8a000029 	bhi	603084bc <cmd_efuse_protect+0x6e8>
60308414:	e3a02045 	mov	r2, #69	; 0x45
60308418:	e3a00002 	mov	r0, #2
6030841c:	e305362c 	movw	r3, #22060	; 0x562c
60308420:	e3463031 	movt	r3, #24625	; 0x6031
60308424:	e3051388 	movw	r1, #21384	; 0x5388
60308428:	e3461031 	movt	r1, #24625	; 0x6031
6030842c:	eb00120d 	bl	6030cc68 <rtk_log_write>
60308430:	eaffff6a 	b	603081e0 <cmd_efuse_protect+0x40c>
60308434:	e3a02057 	mov	r2, #87	; 0x57
60308438:	e3a00003 	mov	r0, #3
6030843c:	e58d8000 	str	r8, [sp]
60308440:	e3053508 	movw	r3, #21768	; 0x5508
60308444:	e3463031 	movt	r3, #24625	; 0x6031
60308448:	e3051388 	movw	r1, #21384	; 0x5388
6030844c:	e3461031 	movt	r1, #24625	; 0x6031
60308450:	eb001204 	bl	6030cc68 <rtk_log_write>
60308454:	eaffff61 	b	603081e0 <cmd_efuse_protect+0x40c>
60308458:	e3a02045 	mov	r2, #69	; 0x45
6030845c:	e3a00002 	mov	r0, #2
60308460:	e30534b8 	movw	r3, #21688	; 0x54b8
60308464:	e3463031 	movt	r3, #24625	; 0x6031
60308468:	e3051388 	movw	r1, #21384	; 0x5388
6030846c:	e3461031 	movt	r1, #24625	; 0x6031
60308470:	eb0011fc 	bl	6030cc68 <rtk_log_write>
60308474:	eaffff59 	b	603081e0 <cmd_efuse_protect+0x40c>
60308478:	e59d5008 	ldr	r5, [sp, #8]
6030847c:	e59d700c 	ldr	r7, [sp, #12]
60308480:	e5950000 	ldr	r0, [r5]
60308484:	e3051624 	movw	r1, #22052	; 0x5624
60308488:	e3461031 	movt	r1, #24625	; 0x6031
6030848c:	e12fff3b 	blx	fp
60308490:	e3500000 	cmp	r0, #0
60308494:	1affff51 	bne	603081e0 <cmd_efuse_protect+0x40c>
60308498:	e5950004 	ldr	r0, [r5, #4]
6030849c:	e3a02010 	mov	r2, #16
603084a0:	e3a01000 	mov	r1, #0
603084a4:	e12fff37 	blx	r7
603084a8:	e3083730 	movw	r3, #34608	; 0x8730
603084ac:	e1500003 	cmp	r0, r3
603084b0:	1affffd7 	bne	60308414 <cmd_efuse_protect+0x640>
603084b4:	eb002d43 	bl	603139c8 <OTPSetCRC>
603084b8:	eaffff48 	b	603081e0 <cmd_efuse_protect+0x40c>
603084bc:	e3047c2c 	movw	r7, #19500	; 0x4c2c
603084c0:	e3467031 	movt	r7, #24625	; 0x6031
603084c4:	eafffff3 	b	60308498 <cmd_efuse_protect+0x6c4>

603084c8 <cmd_rom_table>:
603084c8:	e1a02000 	mov	r2, r0
603084cc:	e3063ed4 	movw	r3, #28372	; 0x6ed4
603084d0:	e3463031 	movt	r3, #24625	; 0x6031
603084d4:	e3a00006 	mov	r0, #6
603084d8:	e5823000 	str	r3, [r2]
603084dc:	e12fff1e 	bx	lr

603084e0 <shell_loguratRx_ipc_int>:
603084e0:	e92d4010 	push	{r4, lr}
603084e4:	e30c4e74 	movw	r4, #52852	; 0xce74
603084e8:	e3464031 	movt	r4, #24625	; 0x6031
603084ec:	e3a01000 	mov	r1, #0
603084f0:	e3a00001 	mov	r0, #1
603084f4:	eb000198 	bl	60308b5c <ipc_get_message>
603084f8:	e5901004 	ldr	r1, [r0, #4]
603084fc:	e5940008 	ldr	r0, [r4, #8]
60308500:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
60308504:	e3130004 	tst	r3, #4
60308508:	0a00000a 	beq	60308538 <shell_loguratRx_ipc_int+0x58>
6030850c:	e281207f 	add	r2, r1, #127	; 0x7f
60308510:	e3c2203f 	bic	r2, r2, #63	; 0x3f
60308514:	e3c1303f 	bic	r3, r1, #63	; 0x3f
60308518:	e2822040 	add	r2, r2, #64	; 0x40
6030851c:	e1530002 	cmp	r3, r2
60308520:	2a000003 	bcs	60308534 <shell_loguratRx_ipc_int+0x54>
60308524:	ee073f36 	mcr	15, 0, r3, cr7, cr6, {1}
60308528:	e2833040 	add	r3, r3, #64	; 0x40
6030852c:	e1520003 	cmp	r2, r3
60308530:	8afffffb 	bhi	60308524 <shell_loguratRx_ipc_int+0x44>
60308534:	f57ff05f 	dmb	sy
60308538:	e3a02080 	mov	r2, #128	; 0x80
6030853c:	e30437a0 	movw	r3, #18336	; 0x47a0
60308540:	e3463031 	movt	r3, #24625	; 0x6031
60308544:	e12fff33 	blx	r3
60308548:	e3a03001 	mov	r3, #1
6030854c:	e5c43004 	strb	r3, [r4, #4]
60308550:	e5943020 	ldr	r3, [r4, #32]
60308554:	e3530000 	cmp	r3, #0
60308558:	08bd8010 	popeq	{r4, pc}
6030855c:	e594301c 	ldr	r3, [r4, #28]
60308560:	e8bd4010 	pop	{r4, lr}
60308564:	e12fff13 	bx	r3

60308568 <shell_cmd_exec_ram>:
60308568:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
6030856c:	e1a04000 	mov	r4, r0
60308570:	eb002f63 	bl	60314304 <shell_get_argc>
60308574:	e1a09000 	mov	r9, r0
60308578:	e1a00004 	mov	r0, r4
6030857c:	eb002f7b 	bl	60314370 <shell_get_argv>
60308580:	e30c3e74 	movw	r3, #52852	; 0xce74
60308584:	e3463031 	movt	r3, #24625	; 0x6031
60308588:	e5908000 	ldr	r8, [r0]
6030858c:	e5935010 	ldr	r5, [r3, #16]
60308590:	e5937014 	ldr	r7, [r3, #20]
60308594:	e3570000 	cmp	r7, #0
60308598:	11a0a000 	movne	sl, r0
6030859c:	13046b1c 	movwne	r6, #19228	; 0x4b1c
603085a0:	13a04000 	movne	r4, #0
603085a4:	13466031 	movtne	r6, #24625	; 0x6031
603085a8:	1a000002 	bne	603085b8 <shell_cmd_exec_ram+0x50>
603085ac:	ea000011 	b	603085f8 <shell_cmd_exec_ram+0x90>
603085b0:	e1570004 	cmp	r7, r4
603085b4:	0a00000f 	beq	603085f8 <shell_cmd_exec_ram+0x90>
603085b8:	e7951204 	ldr	r1, [r5, r4, lsl #4]
603085bc:	e1a00008 	mov	r0, r8
603085c0:	e12fff36 	blx	r6
603085c4:	e0853204 	add	r3, r5, r4, lsl #4
603085c8:	e3500000 	cmp	r0, #0
603085cc:	e2844001 	add	r4, r4, #1
603085d0:	1afffff6 	bne	603085b0 <shell_cmd_exec_ram+0x48>
603085d4:	e5933008 	ldr	r3, [r3, #8]
603085d8:	e3530000 	cmp	r3, #0
603085dc:	0a000005 	beq	603085f8 <shell_cmd_exec_ram+0x90>
603085e0:	e2490001 	sub	r0, r9, #1
603085e4:	e28a1004 	add	r1, sl, #4
603085e8:	e6ff0070 	uxth	r0, r0
603085ec:	e12fff33 	blx	r3
603085f0:	e3a00001 	mov	r0, #1
603085f4:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
603085f8:	e3a00000 	mov	r0, #0
603085fc:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}

60308600 <shell_task_ram>:
60308600:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60308604:	e30c4e74 	movw	r4, #52852	; 0xce74
60308608:	e3464031 	movt	r4, #24625	; 0x6031
6030860c:	e3075464 	movw	r5, #29796	; 0x7464
60308610:	e3465031 	movt	r5, #24625	; 0x6031
60308614:	e594a008 	ldr	sl, [r4, #8]
60308618:	e30492e0 	movw	r9, #17120	; 0x42e0
6030861c:	e3469031 	movt	r9, #24625	; 0x6031
60308620:	e3058998 	movw	r8, #22936	; 0x5998
60308624:	e3468031 	movt	r8, #24625	; 0x6031
60308628:	e3056394 	movw	r6, #21396	; 0x5394
6030862c:	e3466031 	movt	r6, #24625	; 0x6031
60308630:	e305b9b0 	movw	fp, #22960	; 0x59b0
60308634:	e346b031 	movt	fp, #24625	; 0x6031
60308638:	e28a7001 	add	r7, sl, #1
6030863c:	e24dd00c 	sub	sp, sp, #12
60308640:	e3a00b01 	mov	r0, #1024	; 0x400
60308644:	e3a03001 	mov	r3, #1
60308648:	e5843020 	str	r3, [r4, #32]
6030864c:	e5c43006 	strb	r3, [r4, #6]
60308650:	eb0013c0 	bl	6030d558 <rtos_create_secure_context>
60308654:	e5950000 	ldr	r0, [r5]
60308658:	e3e01000 	mvn	r1, #0
6030865c:	eb00134e 	bl	6030d39c <rtos_sema_take>
60308660:	e5d43004 	ldrb	r3, [r4, #4]
60308664:	e3530000 	cmp	r3, #0
60308668:	0afffff9 	beq	60308654 <shell_task_ram+0x54>
6030866c:	e1a00007 	mov	r0, r7
60308670:	ebffffbc 	bl	60308568 <shell_cmd_exec_ram>
60308674:	e1a0c000 	mov	ip, r0
60308678:	e1a01006 	mov	r1, r6
6030867c:	e1a03008 	mov	r3, r8
60308680:	e3a02045 	mov	r2, #69	; 0x45
60308684:	e3a00002 	mov	r0, #2
60308688:	e35c0000 	cmp	ip, #0
6030868c:	1a000006 	bne	603086ac <shell_task_ram+0xac>
60308690:	e58d7000 	str	r7, [sp]
60308694:	eb001197 	bl	6030ccf8 <rtk_log_write_nano>
60308698:	e1a0300b 	mov	r3, fp
6030869c:	e1a01006 	mov	r1, r6
603086a0:	e3a02045 	mov	r2, #69	; 0x45
603086a4:	e3a00002 	mov	r0, #2
603086a8:	eb001192 	bl	6030ccf8 <rtk_log_write_nano>
603086ac:	e3a02000 	mov	r2, #0
603086b0:	e3a01080 	mov	r1, #128	; 0x80
603086b4:	e1a0000a 	mov	r0, sl
603086b8:	e12fff39 	blx	r9
603086bc:	e3a03000 	mov	r3, #0
603086c0:	e5c43004 	strb	r3, [r4, #4]
603086c4:	eaffffe2 	b	60308654 <shell_task_ram+0x54>

603086c8 <shell_give_sema>:
603086c8:	e30c3e74 	movw	r3, #52852	; 0xce74
603086cc:	e3463031 	movt	r3, #24625	; 0x6031
603086d0:	e5933020 	ldr	r3, [r3, #32]
603086d4:	e3530000 	cmp	r3, #0
603086d8:	012fff1e 	bxeq	lr
603086dc:	e3073464 	movw	r3, #29796	; 0x7464
603086e0:	e3463031 	movt	r3, #24625	; 0x6031
603086e4:	e5930000 	ldr	r0, [r3]
603086e8:	ea001354 	b	6030d440 <rtos_sema_give>

603086ec <shell_init_ram>:
603086ec:	e92d4010 	push	{r4, lr}
603086f0:	e30c2e74 	movw	r2, #52852	; 0xce74
603086f4:	e3462031 	movt	r2, #24625	; 0x6031
603086f8:	e24dd008 	sub	sp, sp, #8
603086fc:	e3060ed4 	movw	r0, #28372	; 0x6ed4
60308700:	e3460031 	movt	r0, #24625	; 0x6031
60308704:	e3063f34 	movw	r3, #28468	; 0x6f34
60308708:	e3463031 	movt	r3, #24625	; 0x6031
6030870c:	e0431000 	sub	r1, r3, r0
60308710:	e5820010 	str	r0, [r2, #16]
60308714:	e3a04000 	mov	r4, #0
60308718:	e3070464 	movw	r0, #29796	; 0x7464
6030871c:	e3460031 	movt	r0, #24625	; 0x6031
60308720:	e3a03001 	mov	r3, #1
60308724:	e1a01221 	lsr	r1, r1, #4
60308728:	e5821014 	str	r1, [r2, #20]
6030872c:	e5c24004 	strb	r4, [r2, #4]
60308730:	e30816c8 	movw	r1, #34504	; 0x86c8
60308734:	e3461030 	movt	r1, #24624	; 0x6030
60308738:	e5c23005 	strb	r3, [r2, #5]
6030873c:	e582101c 	str	r1, [r2, #28]
60308740:	eb001307 	bl	6030d364 <rtos_sema_create_binary>
60308744:	e3a02002 	mov	r2, #2
60308748:	e300180c 	movw	r1, #2060	; 0x80c
6030874c:	e58d2004 	str	r2, [sp, #4]
60308750:	e58d1000 	str	r1, [sp]
60308754:	e1a03004 	mov	r3, r4
60308758:	e1a00004 	mov	r0, r4
6030875c:	e3082600 	movw	r2, #34304	; 0x8600
60308760:	e3462030 	movt	r2, #24624	; 0x6030
60308764:	e30519b8 	movw	r1, #22968	; 0x59b8
60308768:	e3461031 	movt	r1, #24625	; 0x6031
6030876c:	eb001367 	bl	6030d510 <rtos_task_create>
60308770:	e1500004 	cmp	r0, r4
60308774:	1a000001 	bne	60308780 <shell_init_ram+0x94>
60308778:	e28dd008 	add	sp, sp, #8
6030877c:	e8bd8010 	pop	{r4, pc}
60308780:	e30509c4 	movw	r0, #22980	; 0x59c4
60308784:	e3460031 	movt	r0, #24625	; 0x6031
60308788:	e28dd008 	add	sp, sp, #8
6030878c:	e8bd4010 	pop	{r4, lr}
60308790:	ea002c67 	b	60313934 <DiagPrintf>

60308794 <ipc_table_init>:
60308794:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60308798:	e3063fa4 	movw	r3, #28580	; 0x6fa4
6030879c:	e3463031 	movt	r3, #24625	; 0x6031
603087a0:	e3064f34 	movw	r4, #28468	; 0x6f34
603087a4:	e3464031 	movt	r4, #24625	; 0x6031
603087a8:	e0433004 	sub	r3, r3, r4
603087ac:	e3042925 	movw	r2, #18725	; 0x4925
603087b0:	e3422492 	movt	r2, #9362	; 0x2492
603087b4:	e1a07123 	lsr	r7, r3, #2
603087b8:	e24dd00c 	sub	sp, sp, #12
603087bc:	e353001b 	cmp	r3, #27
603087c0:	e0872792 	umull	r2, r7, r2, r7
603087c4:	9a00003b 	bls	603088b8 <ipc_table_init+0x124>
603087c8:	e3a0ac86 	mov	sl, #34304	; 0x8600
603087cc:	e344a200 	movt	sl, #16896	; 0x4200
603087d0:	e30599e0 	movw	r9, #23008	; 0x59e0
603087d4:	e3469031 	movt	r9, #24625	; 0x6031
603087d8:	e3058a08 	movw	r8, #23048	; 0x5a08
603087dc:	e3468031 	movt	r8, #24625	; 0x6031
603087e0:	e1a06000 	mov	r6, r0
603087e4:	e3a05000 	mov	r5, #0
603087e8:	ea000014 	b	60308840 <ipc_table_init+0xac>
603087ec:	e3a01c05 	mov	r1, #1280	; 0x500
603087f0:	e3441100 	movt	r1, #16640	; 0x4100
603087f4:	e1560001 	cmp	r6, r1
603087f8:	0a00001c 	beq	60308870 <ipc_table_init+0xdc>
603087fc:	e3520000 	cmp	r2, #0
60308800:	13530020 	cmpne	r3, #32
60308804:	1a000000 	bne	6030880c <ipc_table_init+0x78>
60308808:	e28bb008 	add	fp, fp, #8
6030880c:	e1a00006 	mov	r0, r6
60308810:	eb000167 	bl	60308db4 <IPC_IERGet>
60308814:	e3a03001 	mov	r3, #1
60308818:	e0103b13 	ands	r3, r0, r3, lsl fp
6030881c:	1a00002d 	bne	603088d8 <ipc_table_init+0x144>
60308820:	e1c420dc 	ldrd	r2, [r4, #12]
60308824:	e6ef107b 	uxtb	r1, fp
60308828:	e1a00006 	mov	r0, r6
6030882c:	eb00017c 	bl	60308e24 <IPC_INTUserHandler>
60308830:	e2855001 	add	r5, r5, #1
60308834:	e284401c 	add	r4, r4, #28
60308838:	e1570005 	cmp	r7, r5
6030883c:	9a00001d 	bls	603088b8 <ipc_table_init+0x124>
60308840:	e5943014 	ldr	r3, [r4, #20]
60308844:	e594b018 	ldr	fp, [r4, #24]
60308848:	e3c32010 	bic	r2, r3, #16
6030884c:	e3530021 	cmp	r3, #33	; 0x21
60308850:	13530000 	cmpne	r3, #0
60308854:	0affffe4 	beq	603087ec <ipc_table_init+0x58>
60308858:	e3520001 	cmp	r2, #1
6030885c:	1a000017 	bne	603088c0 <ipc_table_init+0x12c>
60308860:	e3a02d16 	mov	r2, #1408	; 0x580
60308864:	e3442100 	movt	r2, #16640	; 0x4100
60308868:	e1560002 	cmp	r6, r2
6030886c:	1affffe6 	bne	6030880c <ipc_table_init+0x78>
60308870:	e3530010 	cmp	r3, #16
60308874:	13530001 	cmpne	r3, #1
60308878:	93a03018 	movls	r3, #24
6030887c:	83a03010 	movhi	r3, #16
60308880:	e1a00006 	mov	r0, r6
60308884:	e08bb003 	add	fp, fp, r3
60308888:	eb000149 	bl	60308db4 <IPC_IERGet>
6030888c:	e3a03001 	mov	r3, #1
60308890:	e0103b13 	ands	r3, r0, r3, lsl fp
60308894:	1a00000f 	bne	603088d8 <ipc_table_init+0x144>
60308898:	e1c420d4 	ldrd	r2, [r4, #4]
6030889c:	e6ef107b 	uxtb	r1, fp
603088a0:	e1a00006 	mov	r0, r6
603088a4:	e2855001 	add	r5, r5, #1
603088a8:	eb00015d 	bl	60308e24 <IPC_INTUserHandler>
603088ac:	e284401c 	add	r4, r4, #28
603088b0:	e1570005 	cmp	r7, r5
603088b4:	8affffe1 	bhi	60308840 <ipc_table_init+0xac>
603088b8:	e28dd00c 	add	sp, sp, #12
603088bc:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
603088c0:	e2432010 	sub	r2, r3, #16
603088c4:	e3d22010 	bics	r2, r2, #16
603088c8:	1affffd8 	bne	60308830 <ipc_table_init+0x9c>
603088cc:	e156000a 	cmp	r6, sl
603088d0:	1affffcc 	bne	60308808 <ipc_table_init+0x74>
603088d4:	eaffffe5 	b	60308870 <ipc_table_init+0xdc>
603088d8:	e1a03009 	mov	r3, r9
603088dc:	e3a02045 	mov	r2, #69	; 0x45
603088e0:	e1a01008 	mov	r1, r8
603088e4:	e3a00002 	mov	r0, #2
603088e8:	e58db000 	str	fp, [sp]
603088ec:	eb0010dd 	bl	6030cc68 <rtk_log_write>
603088f0:	eaffffce 	b	60308830 <ipc_table_init+0x9c>

603088f4 <IPC_TXHandler>:
603088f4:	e3a03902 	mov	r3, #32768	; 0x8000
603088f8:	e3443200 	movt	r3, #16896	; 0x4200
603088fc:	e92d4010 	push	{r4, lr}
60308900:	e1a04002 	mov	r4, r2
60308904:	e593027c 	ldr	r0, [r3, #636]	; 0x27c
60308908:	e7e10350 	ubfx	r0, r0, #6, #2
6030890c:	eb000152 	bl	60308e5c <IPC_GetDevById>
60308910:	e3a02000 	mov	r2, #0
60308914:	e6ef1074 	uxtb	r1, r4
60308918:	eb00011d 	bl	60308d94 <IPC_INTConfig>
6030891c:	e3073468 	movw	r3, #29800	; 0x7468
60308920:	e3463031 	movt	r3, #24625	; 0x6031
60308924:	e7930104 	ldr	r0, [r3, r4, lsl #2]
60308928:	e3500000 	cmp	r0, #0
6030892c:	08bd8010 	popeq	{r4, pc}
60308930:	e8bd4010 	pop	{r4, lr}
60308934:	ea0012c1 	b	6030d440 <rtos_sema_give>

60308938 <IPC_wait_idle>:
60308938:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
6030893c:	e1a04000 	mov	r4, r0
60308940:	e1a05001 	mov	r5, r1
60308944:	eb0004c7 	bl	60309c68 <CPU_InInterrupt>
60308948:	e3500000 	cmp	r0, #0
6030894c:	1a000015 	bne	603089a8 <IPC_wait_idle+0x70>
60308950:	e3073534 	movw	r3, #30004	; 0x7534
60308954:	e3463031 	movt	r3, #24625	; 0x6031
60308958:	e7933105 	ldr	r3, [r3, r5, lsl #2]
6030895c:	e3530000 	cmp	r3, #0
60308960:	0a000010 	beq	603089a8 <IPC_wait_idle+0x70>
60308964:	e3076468 	movw	r6, #29800	; 0x7468
60308968:	e3466031 	movt	r6, #24625	; 0x6031
6030896c:	e7963105 	ldr	r3, [r6, r5, lsl #2]
60308970:	e3530000 	cmp	r3, #0
60308974:	0a000020 	beq	603089fc <IPC_wait_idle+0xc4>
60308978:	e6ef7075 	uxtb	r7, r5
6030897c:	e3a02001 	mov	r2, #1
60308980:	e1a01007 	mov	r1, r7
60308984:	e1a00004 	mov	r0, r4
60308988:	eb000101 	bl	60308d94 <IPC_INTConfig>
6030898c:	e7960105 	ldr	r0, [r6, r5, lsl #2]
60308990:	e3e01000 	mvn	r1, #0
60308994:	eb001280 	bl	6030d39c <rtos_sema_take>
60308998:	e3500000 	cmp	r0, #0
6030899c:	1a000019 	bne	60308a08 <IPC_wait_idle+0xd0>
603089a0:	e3a00000 	mov	r0, #0
603089a4:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
603089a8:	e3a02001 	mov	r2, #1
603089ac:	e3093680 	movw	r3, #38528	; 0x9680
603089b0:	e3403098 	movt	r3, #152	; 0x98
603089b4:	e1a01512 	lsl	r1, r2, r5
603089b8:	ea000001 	b	603089c4 <IPC_wait_idle+0x8c>
603089bc:	e2533001 	subs	r3, r3, #1
603089c0:	0a000004 	beq	603089d8 <IPC_wait_idle+0xa0>
603089c4:	e5942000 	ldr	r2, [r4]
603089c8:	e1120001 	tst	r2, r1
603089cc:	1afffffa 	bne	603089bc <IPC_wait_idle+0x84>
603089d0:	e3a00000 	mov	r0, #0
603089d4:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
603089d8:	e3a02045 	mov	r2, #69	; 0x45
603089dc:	e3a00002 	mov	r0, #2
603089e0:	e3053a0c 	movw	r3, #23052	; 0x5a0c
603089e4:	e3463031 	movt	r3, #24625	; 0x6031
603089e8:	e3051a08 	movw	r1, #23048	; 0x5a08
603089ec:	e3461031 	movt	r1, #24625	; 0x6031
603089f0:	eb0010c0 	bl	6030ccf8 <rtk_log_write_nano>
603089f4:	e3a00002 	mov	r0, #2
603089f8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
603089fc:	e0860105 	add	r0, r6, r5, lsl #2
60308a00:	eb001257 	bl	6030d364 <rtos_sema_create_binary>
60308a04:	eaffffdb 	b	60308978 <IPC_wait_idle+0x40>
60308a08:	e3a02045 	mov	r2, #69	; 0x45
60308a0c:	e3a00002 	mov	r0, #2
60308a10:	e3053a24 	movw	r3, #23076	; 0x5a24
60308a14:	e3463031 	movt	r3, #24625	; 0x6031
60308a18:	e3051a08 	movw	r1, #23048	; 0x5a08
60308a1c:	e3461031 	movt	r1, #24625	; 0x6031
60308a20:	eb0010b4 	bl	6030ccf8 <rtk_log_write_nano>
60308a24:	e1a00004 	mov	r0, r4
60308a28:	e3a02000 	mov	r2, #0
60308a2c:	e1a01007 	mov	r1, r7
60308a30:	eb0000d7 	bl	60308d94 <IPC_INTConfig>
60308a34:	e3a00003 	mov	r0, #3
60308a38:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

60308a3c <ipc_send_message>:
60308a3c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
60308a40:	e3d03011 	bics	r3, r0, #17
60308a44:	e1a05000 	mov	r5, r0
60308a48:	e1a04001 	mov	r4, r1
60308a4c:	e1a08002 	mov	r8, r2
60308a50:	0a000002 	beq	60308a60 <ipc_send_message+0x24>
60308a54:	e2403020 	sub	r3, r0, #32
60308a58:	e3530001 	cmp	r3, #1
60308a5c:	8a00003a 	bhi	60308b4c <ipc_send_message+0x110>
60308a60:	e3540007 	cmp	r4, #7
60308a64:	8a000034 	bhi	60308b3c <ipc_send_message+0x100>
60308a68:	e3550021 	cmp	r5, #33	; 0x21
60308a6c:	83a06000 	movhi	r6, #0
60308a70:	93053ae0 	movwls	r3, #23264	; 0x5ae0
60308a74:	93052abc 	movwls	r2, #23228	; 0x5abc
60308a78:	93463031 	movtls	r3, #24625	; 0x6031
60308a7c:	93462031 	movtls	r2, #24625	; 0x6031
60308a80:	81a01004 	movhi	r1, r4
60308a84:	97936105 	ldrls	r6, [r3, r5, lsl #2]
60308a88:	97d21005 	ldrbls	r1, [r2, r5]
60308a8c:	e3a09001 	mov	r9, #1
60308a90:	90811004 	addls	r1, r1, r4
60308a94:	e5963000 	ldr	r3, [r6]
60308a98:	e1a07119 	lsl	r7, r9, r1
60308a9c:	e1130007 	tst	r3, r7
60308aa0:	1a00001f 	bne	60308b24 <ipc_send_message+0xe8>
60308aa4:	e3580000 	cmp	r8, #0
60308aa8:	0a00001a 	beq	60308b18 <ipc_send_message+0xdc>
60308aac:	e7e30255 	ubfx	r0, r5, #4, #4
60308ab0:	e205500f 	and	r5, r5, #15
60308ab4:	e0855080 	add	r5, r5, r0, lsl #1
60308ab8:	e30f0d00 	movw	r0, #64768	; 0xfd00
60308abc:	e3420301 	movt	r0, #8961	; 0x2301
60308ac0:	e0844185 	add	r4, r4, r5, lsl #3
60308ac4:	e1a01008 	mov	r1, r8
60308ac8:	e0804204 	add	r4, r0, r4, lsl #4
60308acc:	e3a02010 	mov	r2, #16
60308ad0:	e1a00004 	mov	r0, r4
60308ad4:	e30437a0 	movw	r3, #18336	; 0x47a0
60308ad8:	e3463031 	movt	r3, #24625	; 0x6031
60308adc:	e12fff33 	blx	r3
60308ae0:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
60308ae4:	e3130004 	tst	r3, #4
60308ae8:	0a00000a 	beq	60308b18 <ipc_send_message+0xdc>
60308aec:	e284300f 	add	r3, r4, #15
60308af0:	e3c3303f 	bic	r3, r3, #63	; 0x3f
60308af4:	e3c4003f 	bic	r0, r4, #63	; 0x3f
60308af8:	e2833040 	add	r3, r3, #64	; 0x40
60308afc:	e1500003 	cmp	r0, r3
60308b00:	2a000003 	bcs	60308b14 <ipc_send_message+0xd8>
60308b04:	ee070f3a 	mcr	15, 0, r0, cr7, cr10, {1}
60308b08:	e2800040 	add	r0, r0, #64	; 0x40
60308b0c:	e1530000 	cmp	r3, r0
60308b10:	8afffffb 	bhi	60308b04 <ipc_send_message+0xc8>
60308b14:	f57ff05f 	dmb	sy
60308b18:	e3a00000 	mov	r0, #0
60308b1c:	e5867000 	str	r7, [r6]
60308b20:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
60308b24:	e1a00006 	mov	r0, r6
60308b28:	ebffff82 	bl	60308938 <IPC_wait_idle>
60308b2c:	e3500000 	cmp	r0, #0
60308b30:	0affffdb 	beq	60308aa4 <ipc_send_message+0x68>
60308b34:	e1a00009 	mov	r0, r9
60308b38:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
60308b3c:	e3050a44 	movw	r0, #23108	; 0x5a44
60308b40:	e3460031 	movt	r0, #24625	; 0x6031
60308b44:	e3a010b6 	mov	r1, #182	; 0xb6
60308b48:	eb002b8f 	bl	6031398c <io_assert_failed>
60308b4c:	e3050a44 	movw	r0, #23108	; 0x5a44
60308b50:	e3460031 	movt	r0, #24625	; 0x6031
60308b54:	e3a010b5 	mov	r1, #181	; 0xb5
60308b58:	eb002b8b 	bl	6031398c <io_assert_failed>

60308b5c <ipc_get_message>:
60308b5c:	e200300f 	and	r3, r0, #15
60308b60:	e7e30250 	ubfx	r0, r0, #4, #4
60308b64:	e0833080 	add	r3, r3, r0, lsl #1
60308b68:	e30f0d00 	movw	r0, #64768	; 0xfd00
60308b6c:	e3420301 	movt	r0, #8961	; 0x2301
60308b70:	e0811183 	add	r1, r1, r3, lsl #3
60308b74:	e0800201 	add	r0, r0, r1, lsl #4
60308b78:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
60308b7c:	e3130004 	tst	r3, #4
60308b80:	012fff1e 	bxeq	lr
60308b84:	e280200f 	add	r2, r0, #15
60308b88:	e3c2203f 	bic	r2, r2, #63	; 0x3f
60308b8c:	e3c0303f 	bic	r3, r0, #63	; 0x3f
60308b90:	e2822040 	add	r2, r2, #64	; 0x40
60308b94:	e1530002 	cmp	r3, r2
60308b98:	2a000003 	bcs	60308bac <ipc_get_message+0x50>
60308b9c:	ee073f36 	mcr	15, 0, r3, cr7, cr6, {1}
60308ba0:	e2833040 	add	r3, r3, #64	; 0x40
60308ba4:	e1520003 	cmp	r2, r3
60308ba8:	8afffffb 	bhi	60308b9c <ipc_get_message+0x40>
60308bac:	f57ff05f 	dmb	sy
60308bb0:	e12fff1e 	bx	lr

60308bb4 <IPC_patch_function>:
60308bb4:	e30724ac 	movw	r2, #29868	; 0x74ac
60308bb8:	e3462031 	movt	r2, #24625	; 0x6031
60308bbc:	e30734b0 	movw	r3, #29872	; 0x74b0
60308bc0:	e3463031 	movt	r3, #24625	; 0x6031
60308bc4:	e5820000 	str	r0, [r2]
60308bc8:	e5831000 	str	r1, [r3]
60308bcc:	e12fff1e 	bx	lr

60308bd0 <IPC_SEMTake>:
60308bd0:	e350000f 	cmp	r0, #15
60308bd4:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60308bd8:	8a000042 	bhi	60308ce8 <IPC_SEMTake+0x118>
60308bdc:	e30734ac 	movw	r3, #29868	; 0x74ac
60308be0:	e3463031 	movt	r3, #24625	; 0x6031
60308be4:	e1a08000 	mov	r8, r0
60308be8:	e1a04001 	mov	r4, r1
60308bec:	e5933000 	ldr	r3, [r3]
60308bf0:	e3530000 	cmp	r3, #0
60308bf4:	0a000001 	beq	60308c00 <IPC_SEMTake+0x30>
60308bf8:	e3a00009 	mov	r0, #9
60308bfc:	e12fff33 	blx	r3
60308c00:	e3023a3c 	movw	r3, #10812	; 0x2a3c
60308c04:	e3463031 	movt	r3, #24625	; 0x6031
60308c08:	e12fff33 	blx	r3
60308c0c:	e3500002 	cmp	r0, #2
60308c10:	9a000019 	bls	60308c7c <IPC_SEMTake+0xac>
60308c14:	e30250f0 	movw	r5, #8432	; 0x20f0
60308c18:	e3415080 	movt	r5, #4224	; 0x1080
60308c1c:	e0885005 	add	r5, r8, r5
60308c20:	e1a05105 	lsl	r5, r5, #2
60308c24:	e5953000 	ldr	r3, [r5]
60308c28:	e3530000 	cmp	r3, #0
60308c2c:	0a00002b 	beq	60308ce0 <IPC_SEMTake+0x110>
60308c30:	e3540000 	cmp	r4, #0
60308c34:	0a00000e 	beq	60308c74 <IPC_SEMTake+0xa4>
60308c38:	e30764a8 	movw	r6, #29864	; 0x74a8
60308c3c:	e3466031 	movt	r6, #24625	; 0x6031
60308c40:	ea000002 	b	60308c50 <IPC_SEMTake+0x80>
60308c44:	e5953000 	ldr	r3, [r5]
60308c48:	e3530000 	cmp	r3, #0
60308c4c:	0a000023 	beq	60308ce0 <IPC_SEMTake+0x110>
60308c50:	e5963000 	ldr	r3, [r6]
60308c54:	e3530000 	cmp	r3, #0
60308c58:	0a000001 	beq	60308c64 <IPC_SEMTake+0x94>
60308c5c:	e3a00001 	mov	r0, #1
60308c60:	e12fff33 	blx	r3
60308c64:	e3740001 	cmn	r4, #1
60308c68:	0afffff5 	beq	60308c44 <IPC_SEMTake+0x74>
60308c6c:	e2544001 	subs	r4, r4, #1
60308c70:	1afffff3 	bne	60308c44 <IPC_SEMTake+0x74>
60308c74:	e3a00000 	mov	r0, #0
60308c78:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60308c7c:	e3a03001 	mov	r3, #1
60308c80:	e3a06c82 	mov	r6, #33280	; 0x8200
60308c84:	e3446200 	movt	r6, #16896	; 0x4200
60308c88:	e30774a8 	movw	r7, #29864	; 0x74a8
60308c8c:	e3467031 	movt	r7, #24625	; 0x6031
60308c90:	e1a08813 	lsl	r8, r3, r8
60308c94:	ea000008 	b	60308cbc <IPC_SEMTake+0xec>
60308c98:	e3540000 	cmp	r4, #0
60308c9c:	0afffff4 	beq	60308c74 <IPC_SEMTake+0xa4>
60308ca0:	e5973000 	ldr	r3, [r7]
60308ca4:	e3530000 	cmp	r3, #0
60308ca8:	0a000001 	beq	60308cb4 <IPC_SEMTake+0xe4>
60308cac:	e3a00001 	mov	r0, #1
60308cb0:	e12fff33 	blx	r3
60308cb4:	e2544001 	subs	r4, r4, #1
60308cb8:	0affffed 	beq	60308c74 <IPC_SEMTake+0xa4>
60308cbc:	e1d63fb4 	ldrh	r3, [r6, #244]	; 0xf4
60308cc0:	e1130008 	tst	r3, r8
60308cc4:	e6ff3073 	uxth	r3, r3
60308cc8:	1afffff2 	bne	60308c98 <IPC_SEMTake+0xc8>
60308ccc:	e1833008 	orr	r3, r3, r8
60308cd0:	e3a00001 	mov	r0, #1
60308cd4:	e6ff3073 	uxth	r3, r3
60308cd8:	e1c63fb4 	strh	r3, [r6, #244]	; 0xf4
60308cdc:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60308ce0:	e3a00001 	mov	r0, #1
60308ce4:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60308ce8:	e3050a44 	movw	r0, #23108	; 0x5a44
60308cec:	e3460031 	movt	r0, #24625	; 0x6031
60308cf0:	e3001116 	movw	r1, #278	; 0x116
60308cf4:	eb002b24 	bl	6031398c <io_assert_failed>

60308cf8 <IPC_SEMFree>:
60308cf8:	e350000f 	cmp	r0, #15
60308cfc:	e92d4010 	push	{r4, lr}
60308d00:	8a00001b 	bhi	60308d74 <IPC_SEMFree+0x7c>
60308d04:	e3023a3c 	movw	r3, #10812	; 0x2a3c
60308d08:	e3463031 	movt	r3, #24625	; 0x6031
60308d0c:	e1a04000 	mov	r4, r0
60308d10:	e12fff33 	blx	r3
60308d14:	e3500002 	cmp	r0, #2
60308d18:	8a00000e 	bhi	60308d58 <IPC_SEMFree+0x60>
60308d1c:	e3a03c82 	mov	r3, #33280	; 0x8200
60308d20:	e3443200 	movt	r3, #16896	; 0x4200
60308d24:	e3a02001 	mov	r2, #1
60308d28:	e1d30fb4 	ldrh	r0, [r3, #244]	; 0xf4
60308d2c:	e1c04412 	bic	r4, r0, r2, lsl r4
60308d30:	e1c34fb4 	strh	r4, [r3, #244]	; 0xf4
60308d34:	e30734b0 	movw	r3, #29872	; 0x74b0
60308d38:	e3463031 	movt	r3, #24625	; 0x6031
60308d3c:	e5933000 	ldr	r3, [r3]
60308d40:	e3530000 	cmp	r3, #0
60308d44:	0a000001 	beq	60308d50 <IPC_SEMFree+0x58>
60308d48:	e3a00009 	mov	r0, #9
60308d4c:	e12fff33 	blx	r3
60308d50:	e3a00001 	mov	r0, #1
60308d54:	e8bd8010 	pop	{r4, pc}
60308d58:	e30230f0 	movw	r3, #8432	; 0x20f0
60308d5c:	e3413080 	movt	r3, #4224	; 0x1080
60308d60:	e0843003 	add	r3, r4, r3
60308d64:	e3a02001 	mov	r2, #1
60308d68:	e1a03103 	lsl	r3, r3, #2
60308d6c:	e5832000 	str	r2, [r3]
60308d70:	eaffffef 	b	60308d34 <IPC_SEMFree+0x3c>
60308d74:	e3050a44 	movw	r0, #23108	; 0x5a44
60308d78:	e3460031 	movt	r0, #24625	; 0x6031
60308d7c:	e3a01f56 	mov	r1, #344	; 0x158
60308d80:	eb002b01 	bl	6031398c <io_assert_failed>

60308d84 <IPC_SEMDelayStub>:
60308d84:	e30734a8 	movw	r3, #29864	; 0x74a8
60308d88:	e3463031 	movt	r3, #24625	; 0x6031
60308d8c:	e5830000 	str	r0, [r3]
60308d90:	e12fff1e 	bx	lr

60308d94 <IPC_INTConfig>:
60308d94:	e3a03001 	mov	r3, #1
60308d98:	e3520001 	cmp	r2, #1
60308d9c:	e1a03113 	lsl	r3, r3, r1
60308da0:	e590100c 	ldr	r1, [r0, #12]
60308da4:	01813003 	orreq	r3, r1, r3
60308da8:	11c13003 	bicne	r3, r1, r3
60308dac:	e580300c 	str	r3, [r0, #12]
60308db0:	e12fff1e 	bx	lr

60308db4 <IPC_IERGet>:
60308db4:	e590000c 	ldr	r0, [r0, #12]
60308db8:	e12fff1e 	bx	lr

60308dbc <IPC_INTHandler>:
60308dbc:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
60308dc0:	e3079534 	movw	r9, #30004	; 0x7534
60308dc4:	e3469031 	movt	r9, #24625	; 0x6031
60308dc8:	e5906008 	ldr	r6, [r0, #8]
60308dcc:	e307a4b4 	movw	sl, #29876	; 0x74b4
60308dd0:	e346a031 	movt	sl, #24625	; 0x6031
60308dd4:	e1a08000 	mov	r8, r0
60308dd8:	e3a04000 	mov	r4, #0
60308ddc:	e3a07001 	mov	r7, #1
60308de0:	e1a05417 	lsl	r5, r7, r4
60308de4:	e1150006 	tst	r5, r6
60308de8:	0a000008 	beq	60308e10 <IPC_INTHandler+0x54>
60308dec:	e7993104 	ldr	r3, [r9, r4, lsl #2]
60308df0:	e1a02004 	mov	r2, r4
60308df4:	e3530000 	cmp	r3, #0
60308df8:	e1a01006 	mov	r1, r6
60308dfc:	05885008 	streq	r5, [r8, #8]
60308e00:	0a000002 	beq	60308e10 <IPC_INTHandler+0x54>
60308e04:	e79a0104 	ldr	r0, [sl, r4, lsl #2]
60308e08:	e12fff33 	blx	r3
60308e0c:	e5885008 	str	r5, [r8, #8]
60308e10:	e2844001 	add	r4, r4, #1
60308e14:	e3540020 	cmp	r4, #32
60308e18:	1afffff0 	bne	60308de0 <IPC_INTHandler+0x24>
60308e1c:	e3a00000 	mov	r0, #0
60308e20:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}

60308e24 <IPC_INTUserHandler>:
60308e24:	e3520000 	cmp	r2, #0
60308e28:	012fff1e 	bxeq	lr
60308e2c:	e307c534 	movw	ip, #30004	; 0x7534
60308e30:	e346c031 	movt	ip, #24625	; 0x6031
60308e34:	e351000f 	cmp	r1, #15
60308e38:	e78c2101 	str	r2, [ip, r1, lsl #2]
60308e3c:	e30724b4 	movw	r2, #29876	; 0x74b4
60308e40:	e3462031 	movt	r2, #24625	; 0x6031
60308e44:	e7823101 	str	r3, [r2, r1, lsl #2]
60308e48:	83a02001 	movhi	r2, #1
60308e4c:	8590300c 	ldrhi	r3, [r0, #12]
60308e50:	81831112 	orrhi	r1, r3, r2, lsl r1
60308e54:	8580100c 	strhi	r1, [r0, #12]
60308e58:	e12fff1e 	bx	lr

60308e5c <IPC_GetDevById>:
60308e5c:	e3500002 	cmp	r0, #2
60308e60:	8a00000c 	bhi	60308e98 <IPC_GetDevById+0x3c>
60308e64:	e1a03000 	mov	r3, r0
60308e68:	e3500001 	cmp	r0, #1
60308e6c:	0a000006 	beq	60308e8c <IPC_GetDevById+0x30>
60308e70:	e3530002 	cmp	r3, #2
60308e74:	e3a02c86 	mov	r2, #34304	; 0x8600
60308e78:	e3442200 	movt	r2, #16896	; 0x4200
60308e7c:	e3a00d16 	mov	r0, #1408	; 0x580
60308e80:	e3440100 	movt	r0, #16640	; 0x4100
60308e84:	11a00002 	movne	r0, r2
60308e88:	e12fff1e 	bx	lr
60308e8c:	e3a00c05 	mov	r0, #1280	; 0x500
60308e90:	e3440100 	movt	r0, #16640	; 0x4100
60308e94:	e12fff1e 	bx	lr
60308e98:	e3050b68 	movw	r0, #23400	; 0x5b68
60308e9c:	e3460031 	movt	r0, #24625	; 0x6031
60308ea0:	e3a010bb 	mov	r1, #187	; 0xbb
60308ea4:	e92d4010 	push	{r4, lr}
60308ea8:	eb002ab7 	bl	6031398c <io_assert_failed>

60308eac <OTP_Read8.part.0>:
60308eac:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
60308eb0:	e3055be0 	movw	r5, #23520	; 0x5be0
60308eb4:	e3465031 	movt	r5, #24625	; 0x6031
60308eb8:	e3054bfc 	movw	r4, #23548	; 0x5bfc
60308ebc:	e3464031 	movt	r4, #24625	; 0x6031
60308ec0:	e24dd00c 	sub	sp, sp, #12
60308ec4:	e1a08000 	mov	r8, r0
60308ec8:	e1a09001 	mov	r9, r1
60308ecc:	ea000000 	b	60308ed4 <OTP_Read8.part.0+0x28>
60308ed0:	eb000f88 	bl	6030ccf8 <rtk_log_write_nano>
60308ed4:	e3a01ffa 	mov	r1, #1000	; 0x3e8
60308ed8:	e3a00002 	mov	r0, #2
60308edc:	ebffff3b 	bl	60308bd0 <IPC_SEMTake>
60308ee0:	e1a0c000 	mov	ip, r0
60308ee4:	e1a03005 	mov	r3, r5
60308ee8:	e3a02045 	mov	r2, #69	; 0x45
60308eec:	e1a01004 	mov	r1, r4
60308ef0:	e3a00002 	mov	r0, #2
60308ef4:	e35c0001 	cmp	ip, #1
60308ef8:	1afffff4 	bne	60308ed0 <OTP_Read8.part.0+0x24>
60308efc:	e3a03902 	mov	r3, #32768	; 0x8000
60308f00:	e3443200 	movt	r3, #16896	; 0x4200
60308f04:	e3a05442 	mov	r5, #1107296256	; 0x42000000
60308f08:	e5932000 	ldr	r2, [r3]
60308f0c:	e3120001 	tst	r2, #1
60308f10:	05932000 	ldreq	r2, [r3]
60308f14:	03822001 	orreq	r2, r2, #1
60308f18:	05832000 	streq	r2, [r3]
60308f1c:	e1a03408 	lsl	r3, r8, #8
60308f20:	e5952014 	ldr	r2, [r5, #20]
60308f24:	e3c224ff 	bic	r2, r2, #-16777216	; 0xff000000
60308f28:	e5852014 	str	r2, [r5, #20]
60308f2c:	e5853008 	str	r3, [r5, #8]
60308f30:	e5953008 	ldr	r3, [r5, #8]
60308f34:	e3530000 	cmp	r3, #0
60308f38:	ba00000f 	blt	60308f7c <OTP_Read8.part.0+0xd0>
60308f3c:	e30c6b2c 	movw	r6, #52012	; 0xcb2c
60308f40:	e3466030 	movt	r6, #24624	; 0x6030
60308f44:	e3a04000 	mov	r4, #0
60308f48:	e3047e20 	movw	r7, #20000	; 0x4e20
60308f4c:	ea000001 	b	60308f58 <OTP_Read8.part.0+0xac>
60308f50:	e1540007 	cmp	r4, r7
60308f54:	0a000015 	beq	60308fb0 <OTP_Read8.part.0+0x104>
60308f58:	e3a00005 	mov	r0, #5
60308f5c:	e12fff36 	blx	r6
60308f60:	e5953008 	ldr	r3, [r5, #8]
60308f64:	e2844001 	add	r4, r4, #1
60308f68:	e3530000 	cmp	r3, #0
60308f6c:	aafffff7 	bge	60308f50 <OTP_Read8.part.0+0xa4>
60308f70:	e3043e20 	movw	r3, #20000	; 0x4e20
60308f74:	e1540003 	cmp	r4, r3
60308f78:	0a00000c 	beq	60308fb0 <OTP_Read8.part.0+0x104>
60308f7c:	e3a03442 	mov	r3, #1107296256	; 0x42000000
60308f80:	e3a04000 	mov	r4, #0
60308f84:	e5933008 	ldr	r3, [r3, #8]
60308f88:	e5c93000 	strb	r3, [r9]
60308f8c:	e3a02442 	mov	r2, #1107296256	; 0x42000000
60308f90:	e3a00002 	mov	r0, #2
60308f94:	e5923014 	ldr	r3, [r2, #20]
60308f98:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
60308f9c:	e5823014 	str	r3, [r2, #20]
60308fa0:	ebffff54 	bl	60308cf8 <IPC_SEMFree>
60308fa4:	e1a00004 	mov	r0, r4
60308fa8:	e28dd00c 	add	sp, sp, #12
60308fac:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
60308fb0:	e3e04000 	mvn	r4, #0
60308fb4:	e3a02045 	mov	r2, #69	; 0x45
60308fb8:	e5c94000 	strb	r4, [r9]
60308fbc:	e3a00002 	mov	r0, #2
60308fc0:	e58d8000 	str	r8, [sp]
60308fc4:	e3053c00 	movw	r3, #23552	; 0x5c00
60308fc8:	e3463031 	movt	r3, #24625	; 0x6031
60308fcc:	e3051bfc 	movw	r1, #23548	; 0x5bfc
60308fd0:	e3461031 	movt	r1, #24625	; 0x6031
60308fd4:	eb000f47 	bl	6030ccf8 <rtk_log_write_nano>
60308fd8:	eaffffeb 	b	60308f8c <OTP_Read8.part.0+0xe0>

60308fdc <OTP_ProgramMarginRead8.part.0>:
60308fdc:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
60308fe0:	e3055be0 	movw	r5, #23520	; 0x5be0
60308fe4:	e3465031 	movt	r5, #24625	; 0x6031
60308fe8:	e24dd014 	sub	sp, sp, #20
60308fec:	e3054bfc 	movw	r4, #23548	; 0x5bfc
60308ff0:	e3464031 	movt	r4, #24625	; 0x6031
60308ff4:	e1a08000 	mov	r8, r0
60308ff8:	e1a09001 	mov	r9, r1
60308ffc:	e58d100c 	str	r1, [sp, #12]
60309000:	ea000000 	b	60309008 <OTP_ProgramMarginRead8.part.0+0x2c>
60309004:	eb000f3b 	bl	6030ccf8 <rtk_log_write_nano>
60309008:	e3a01ffa 	mov	r1, #1000	; 0x3e8
6030900c:	e3a00002 	mov	r0, #2
60309010:	ebfffeee 	bl	60308bd0 <IPC_SEMTake>
60309014:	e1a0c000 	mov	ip, r0
60309018:	e1a03005 	mov	r3, r5
6030901c:	e3a02045 	mov	r2, #69	; 0x45
60309020:	e1a01004 	mov	r1, r4
60309024:	e3a00002 	mov	r0, #2
60309028:	e35c0001 	cmp	ip, #1
6030902c:	1afffff4 	bne	60309004 <OTP_ProgramMarginRead8.part.0+0x28>
60309030:	e3a03902 	mov	r3, #32768	; 0x8000
60309034:	e3443200 	movt	r3, #16896	; 0x4200
60309038:	e3a05442 	mov	r5, #1107296256	; 0x42000000
6030903c:	e5932000 	ldr	r2, [r3]
60309040:	e3120001 	tst	r2, #1
60309044:	05932000 	ldreq	r2, [r3]
60309048:	03822001 	orreq	r2, r2, #1
6030904c:	05832000 	streq	r2, [r3]
60309050:	e1a03408 	lsl	r3, r8, #8
60309054:	e5952014 	ldr	r2, [r5, #20]
60309058:	e3833601 	orr	r3, r3, #1048576	; 0x100000
6030905c:	e3c224ff 	bic	r2, r2, #-16777216	; 0xff000000
60309060:	e5852014 	str	r2, [r5, #20]
60309064:	e5853008 	str	r3, [r5, #8]
60309068:	e5953008 	ldr	r3, [r5, #8]
6030906c:	e3530000 	cmp	r3, #0
60309070:	ba00000f 	blt	603090b4 <OTP_ProgramMarginRead8.part.0+0xd8>
60309074:	e30c6b2c 	movw	r6, #52012	; 0xcb2c
60309078:	e3466030 	movt	r6, #24624	; 0x6030
6030907c:	e3a04000 	mov	r4, #0
60309080:	e3047e20 	movw	r7, #20000	; 0x4e20
60309084:	ea000001 	b	60309090 <OTP_ProgramMarginRead8.part.0+0xb4>
60309088:	e1540007 	cmp	r4, r7
6030908c:	0a000015 	beq	603090e8 <OTP_ProgramMarginRead8.part.0+0x10c>
60309090:	e3a00005 	mov	r0, #5
60309094:	e12fff36 	blx	r6
60309098:	e5953008 	ldr	r3, [r5, #8]
6030909c:	e2844001 	add	r4, r4, #1
603090a0:	e3530000 	cmp	r3, #0
603090a4:	aafffff7 	bge	60309088 <OTP_ProgramMarginRead8.part.0+0xac>
603090a8:	e3043e20 	movw	r3, #20000	; 0x4e20
603090ac:	e1540003 	cmp	r4, r3
603090b0:	0a00000c 	beq	603090e8 <OTP_ProgramMarginRead8.part.0+0x10c>
603090b4:	e3a03442 	mov	r3, #1107296256	; 0x42000000
603090b8:	e3a04000 	mov	r4, #0
603090bc:	e5933008 	ldr	r3, [r3, #8]
603090c0:	e5c93000 	strb	r3, [r9]
603090c4:	e3a02442 	mov	r2, #1107296256	; 0x42000000
603090c8:	e3a00002 	mov	r0, #2
603090cc:	e5923014 	ldr	r3, [r2, #20]
603090d0:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
603090d4:	e5823014 	str	r3, [r2, #20]
603090d8:	ebffff06 	bl	60308cf8 <IPC_SEMFree>
603090dc:	e1a00004 	mov	r0, r4
603090e0:	e28dd014 	add	sp, sp, #20
603090e4:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
603090e8:	e3e04000 	mvn	r4, #0
603090ec:	e3a02045 	mov	r2, #69	; 0x45
603090f0:	e5c94000 	strb	r4, [r9]
603090f4:	e3a00002 	mov	r0, #2
603090f8:	e58d8000 	str	r8, [sp]
603090fc:	e3053c00 	movw	r3, #23552	; 0x5c00
60309100:	e3463031 	movt	r3, #24625	; 0x6031
60309104:	e3051bfc 	movw	r1, #23548	; 0x5bfc
60309108:	e3461031 	movt	r1, #24625	; 0x6031
6030910c:	eb000ef9 	bl	6030ccf8 <rtk_log_write_nano>
60309110:	eaffffeb 	b	603090c4 <OTP_ProgramMarginRead8.part.0+0xe8>

60309114 <OTPGetCRC>:
60309114:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60309118:	e3a05442 	mov	r5, #1107296256	; 0x42000000
6030911c:	e3a03101 	mov	r3, #1073741824	; 0x40000000
60309120:	e5853034 	str	r3, [r5, #52]	; 0x34
60309124:	e5954000 	ldr	r4, [r5]
60309128:	e2144002 	ands	r4, r4, #2
6030912c:	1a00000b 	bne	60309160 <OTPGetCRC+0x4c>
60309130:	e30c6b2c 	movw	r6, #52012	; 0xcb2c
60309134:	e3466030 	movt	r6, #24624	; 0x6030
60309138:	e3047e20 	movw	r7, #20000	; 0x4e20
6030913c:	e3a00001 	mov	r0, #1
60309140:	e12fff36 	blx	r6
60309144:	e5953000 	ldr	r3, [r5]
60309148:	e2844001 	add	r4, r4, #1
6030914c:	e0542007 	subs	r2, r4, r7
60309150:	e2233002 	eor	r3, r3, #2
60309154:	13a02001 	movne	r2, #1
60309158:	e01230a3 	ands	r3, r2, r3, lsr #1
6030915c:	1afffff6 	bne	6030913c <OTPGetCRC+0x28>
60309160:	e3a03442 	mov	r3, #1107296256	; 0x42000000
60309164:	e3e02101 	mvn	r2, #1073741824	; 0x40000000
60309168:	e5930030 	ldr	r0, [r3, #48]	; 0x30
6030916c:	e5832034 	str	r2, [r3, #52]	; 0x34
60309170:	e1a00820 	lsr	r0, r0, #16
60309174:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

60309178 <OTP_Read8>:
60309178:	e3500b02 	cmp	r0, #2048	; 0x800
6030917c:	2a000000 	bcs	60309184 <OTP_Read8+0xc>
60309180:	eaffff49 	b	60308eac <OTP_Read8.part.0>
60309184:	e3e00000 	mvn	r0, #0
60309188:	e5c10000 	strb	r0, [r1]
6030918c:	e12fff1e 	bx	lr

60309190 <OTP_Write8>:
60309190:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60309194:	e3500b02 	cmp	r0, #2048	; 0x800
60309198:	e24dd024 	sub	sp, sp, #36	; 0x24
6030919c:	23e03000 	mvncs	r3, #0
603091a0:	e1cd00f8 	strd	r0, [sp, #8]
603091a4:	25cd301f 	strbcs	r3, [sp, #31]
603091a8:	2a00007f 	bcs	603093ac <OTP_Write8+0x21c>
603091ac:	e28d101f 	add	r1, sp, #31
603091b0:	ebffff89 	bl	60308fdc <OTP_ProgramMarginRead8.part.0>
603091b4:	e3700001 	cmn	r0, #1
603091b8:	0a00007b 	beq	603093ac <OTP_Write8+0x21c>
603091bc:	e59d2008 	ldr	r2, [sp, #8]
603091c0:	e5dd301f 	ldrb	r3, [sp, #31]
603091c4:	e59d900c 	ldr	r9, [sp, #12]
603091c8:	e3a0b902 	mov	fp, #32768	; 0x8000
603091cc:	e344b200 	movt	fp, #16896	; 0x4200
603091d0:	e3058c28 	movw	r8, #23592	; 0x5c28
603091d4:	e3468031 	movt	r8, #24625	; 0x6031
603091d8:	e3057bfc 	movw	r7, #23548	; 0x5bfc
603091dc:	e3467031 	movt	r7, #24625	; 0x6031
603091e0:	e1a02402 	lsl	r2, r2, #8
603091e4:	e58d2014 	str	r2, [sp, #20]
603091e8:	e3a02002 	mov	r2, #2
603091ec:	e58d2010 	str	r2, [sp, #16]
603091f0:	e1e03003 	mvn	r3, r3
603091f4:	e1839009 	orr	r9, r3, r9
603091f8:	e6ef9079 	uxtb	r9, r9
603091fc:	e35900ff 	cmp	r9, #255	; 0xff
60309200:	1a000001 	bne	6030920c <OTP_Write8+0x7c>
60309204:	ea000030 	b	603092cc <OTP_Write8+0x13c>
60309208:	eb000eba 	bl	6030ccf8 <rtk_log_write_nano>
6030920c:	e3a01ffa 	mov	r1, #1000	; 0x3e8
60309210:	e3a00002 	mov	r0, #2
60309214:	ebfffe6d 	bl	60308bd0 <IPC_SEMTake>
60309218:	e1a0c000 	mov	ip, r0
6030921c:	e1a03008 	mov	r3, r8
60309220:	e3a02045 	mov	r2, #69	; 0x45
60309224:	e1a01007 	mov	r1, r7
60309228:	e3a00002 	mov	r0, #2
6030922c:	e35c0001 	cmp	ip, #1
60309230:	1afffff4 	bne	60309208 <OTP_Write8+0x78>
60309234:	e59b3000 	ldr	r3, [fp]
60309238:	e3a04442 	mov	r4, #1107296256	; 0x42000000
6030923c:	e3130001 	tst	r3, #1
60309240:	059b3000 	ldreq	r3, [fp]
60309244:	03833001 	orreq	r3, r3, #1
60309248:	058b3000 	streq	r3, [fp]
6030924c:	e59d3014 	ldr	r3, [sp, #20]
60309250:	e5942014 	ldr	r2, [r4, #20]
60309254:	e1893003 	orr	r3, r9, r3
60309258:	e3833102 	orr	r3, r3, #-2147483648	; 0x80000000
6030925c:	e3822469 	orr	r2, r2, #1761607680	; 0x69000000
60309260:	e5842014 	str	r2, [r4, #20]
60309264:	e5843008 	str	r3, [r4, #8]
60309268:	e5943008 	ldr	r3, [r4, #8]
6030926c:	e3530000 	cmp	r3, #0
60309270:	b3a0a000 	movlt	sl, #0
60309274:	b30c5b2c 	movwlt	r5, #52012	; 0xcb2c
60309278:	b3046e20 	movwlt	r6, #20000	; 0x4e20
6030927c:	b3465030 	movtlt	r5, #24624	; 0x6030
60309280:	ba000002 	blt	60309290 <OTP_Write8+0x100>
60309284:	ea00000a 	b	603092b4 <OTP_Write8+0x124>
60309288:	e15a0006 	cmp	sl, r6
6030928c:	0a000021 	beq	60309318 <OTP_Write8+0x188>
60309290:	e3a00005 	mov	r0, #5
60309294:	e12fff35 	blx	r5
60309298:	e5943008 	ldr	r3, [r4, #8]
6030929c:	e28aa001 	add	sl, sl, #1
603092a0:	e3530000 	cmp	r3, #0
603092a4:	bafffff7 	blt	60309288 <OTP_Write8+0xf8>
603092a8:	e3042e20 	movw	r2, #20000	; 0x4e20
603092ac:	e15a0002 	cmp	sl, r2
603092b0:	0a000018 	beq	60309318 <OTP_Write8+0x188>
603092b4:	e3a02442 	mov	r2, #1107296256	; 0x42000000
603092b8:	e3a00002 	mov	r0, #2
603092bc:	e5923014 	ldr	r3, [r2, #20]
603092c0:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
603092c4:	e5823014 	str	r3, [r2, #20]
603092c8:	ebfffe8a 	bl	60308cf8 <IPC_SEMFree>
603092cc:	e59d0008 	ldr	r0, [sp, #8]
603092d0:	e28d101f 	add	r1, sp, #31
603092d4:	ebffff40 	bl	60308fdc <OTP_ProgramMarginRead8.part.0>
603092d8:	e3700001 	cmn	r0, #1
603092dc:	0a000027 	beq	60309380 <OTP_Write8+0x1f0>
603092e0:	e5dd301f 	ldrb	r3, [sp, #31]
603092e4:	e59d200c 	ldr	r2, [sp, #12]
603092e8:	e1530002 	cmp	r3, r2
603092ec:	0a00002c 	beq	603093a4 <OTP_Write8+0x214>
603092f0:	e59d2010 	ldr	r2, [sp, #16]
603092f4:	e3520001 	cmp	r2, #1
603092f8:	1a000003 	bne	6030930c <OTP_Write8+0x17c>
603092fc:	e3e04000 	mvn	r4, #0
60309300:	e1a00004 	mov	r0, r4
60309304:	e28dd024 	add	sp, sp, #36	; 0x24
60309308:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
6030930c:	e3a02001 	mov	r2, #1
60309310:	e58d2010 	str	r2, [sp, #16]
60309314:	eaffffb5 	b	603091f0 <OTP_Write8+0x60>
60309318:	e59d3008 	ldr	r3, [sp, #8]
6030931c:	e58d3000 	str	r3, [sp]
60309320:	e3a02045 	mov	r2, #69	; 0x45
60309324:	e3a00002 	mov	r0, #2
60309328:	e3053c44 	movw	r3, #23620	; 0x5c44
6030932c:	e3463031 	movt	r3, #24625	; 0x6031
60309330:	e3051bfc 	movw	r1, #23548	; 0x5bfc
60309334:	e3461031 	movt	r1, #24625	; 0x6031
60309338:	eb000e6e 	bl	6030ccf8 <rtk_log_write_nano>
6030933c:	e3a02442 	mov	r2, #1107296256	; 0x42000000
60309340:	e3a00002 	mov	r0, #2
60309344:	e3e04000 	mvn	r4, #0
60309348:	e5923014 	ldr	r3, [r2, #20]
6030934c:	e3c334ff 	bic	r3, r3, #-16777216	; 0xff000000
60309350:	e5823014 	str	r3, [r2, #20]
60309354:	ebfffe67 	bl	60308cf8 <IPC_SEMFree>
60309358:	e3a02045 	mov	r2, #69	; 0x45
6030935c:	e3a00002 	mov	r0, #2
60309360:	e3053c5c 	movw	r3, #23644	; 0x5c5c
60309364:	e3463031 	movt	r3, #24625	; 0x6031
60309368:	e3051bfc 	movw	r1, #23548	; 0x5bfc
6030936c:	e3461031 	movt	r1, #24625	; 0x6031
60309370:	eb000e60 	bl	6030ccf8 <rtk_log_write_nano>
60309374:	e1a00004 	mov	r0, r4
60309378:	e28dd024 	add	sp, sp, #36	; 0x24
6030937c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
60309380:	e1a04000 	mov	r4, r0
60309384:	e3a02045 	mov	r2, #69	; 0x45
60309388:	e3a00002 	mov	r0, #2
6030938c:	e3053c70 	movw	r3, #23664	; 0x5c70
60309390:	e3463031 	movt	r3, #24625	; 0x6031
60309394:	e3051bfc 	movw	r1, #23548	; 0x5bfc
60309398:	e3461031 	movt	r1, #24625	; 0x6031
6030939c:	eb000e55 	bl	6030ccf8 <rtk_log_write_nano>
603093a0:	eaffffd6 	b	60309300 <OTP_Write8+0x170>
603093a4:	e3a04000 	mov	r4, #0
603093a8:	eaffffd4 	b	60309300 <OTP_Write8+0x170>
603093ac:	e3a02045 	mov	r2, #69	; 0x45
603093b0:	e3a00002 	mov	r0, #2
603093b4:	e3053c14 	movw	r3, #23572	; 0x5c14
603093b8:	e3463031 	movt	r3, #24625	; 0x6031
603093bc:	e3051bfc 	movw	r1, #23548	; 0x5bfc
603093c0:	e3461031 	movt	r1, #24625	; 0x6031
603093c4:	e3e04000 	mvn	r4, #0
603093c8:	eb000e4a 	bl	6030ccf8 <rtk_log_write_nano>
603093cc:	eaffffcb 	b	60309300 <OTP_Write8+0x170>

603093d0 <OTP_Read32>:
603093d0:	e92d4070 	push	{r4, r5, r6, lr}
603093d4:	e3a03000 	mov	r3, #0
603093d8:	e24dd008 	sub	sp, sp, #8
603093dc:	e1a06001 	mov	r6, r1
603093e0:	e1a04000 	mov	r4, r0
603093e4:	e1a05003 	mov	r5, r3
603093e8:	e5813000 	str	r3, [r1]
603093ec:	e1a00004 	mov	r0, r4
603093f0:	e28d1007 	add	r1, sp, #7
603093f4:	e3540b02 	cmp	r4, #2048	; 0x800
603093f8:	2a00000d 	bcs	60309434 <OTP_Read32+0x64>
603093fc:	ebfffeaa 	bl	60308eac <OTP_Read8.part.0>
60309400:	e3700001 	cmn	r0, #1
60309404:	0a00000a 	beq	60309434 <OTP_Read32+0x64>
60309408:	e5963000 	ldr	r3, [r6]
6030940c:	e5dd2007 	ldrb	r2, [sp, #7]
60309410:	e2844001 	add	r4, r4, #1
60309414:	e1833512 	orr	r3, r3, r2, lsl r5
60309418:	e2855008 	add	r5, r5, #8
6030941c:	e3550020 	cmp	r5, #32
60309420:	e5863000 	str	r3, [r6]
60309424:	1afffff0 	bne	603093ec <OTP_Read32+0x1c>
60309428:	e3a00000 	mov	r0, #0
6030942c:	e28dd008 	add	sp, sp, #8
60309430:	e8bd8070 	pop	{r4, r5, r6, pc}
60309434:	e3e00000 	mvn	r0, #0
60309438:	e28dd008 	add	sp, sp, #8
6030943c:	e8bd8070 	pop	{r4, r5, r6, pc}

60309440 <OTP_LogicalMap_Read>:
60309440:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60309444:	e0817002 	add	r7, r1, r2
60309448:	e24dd01c 	sub	sp, sp, #28
6030944c:	e1a06001 	mov	r6, r1
60309450:	e3570b01 	cmp	r7, #1024	; 0x400
60309454:	e58d0008 	str	r0, [sp, #8]
60309458:	8a000059 	bhi	603095c4 <OTP_LogicalMap_Read+0x184>
6030945c:	e59d3008 	ldr	r3, [sp, #8]
60309460:	e3a04000 	mov	r4, #0
60309464:	e0433001 	sub	r3, r3, r1
60309468:	e3a010ff 	mov	r1, #255	; 0xff
6030946c:	e58d300c 	str	r3, [sp, #12]
60309470:	e30438c4 	movw	r3, #18628	; 0x48c4
60309474:	e3463031 	movt	r3, #24625	; 0x6031
60309478:	e3058cb0 	movw	r8, #23728	; 0x5cb0
6030947c:	e3468031 	movt	r8, #24625	; 0x6031
60309480:	e12fff33 	blx	r3
60309484:	ea00000c 	b	603094bc <OTP_LogicalMap_Read+0x7c>
60309488:	e3520002 	cmp	r2, #2
6030948c:	1a000006 	bne	603094ac <OTP_LogicalMap_Read+0x6c>
60309490:	e7e39c53 	ubfx	r9, r3, #24, #4
60309494:	e7e32653 	ubfx	r2, r3, #12, #4
60309498:	e1a09109 	lsl	r9, r9, #2
6030949c:	e2899004 	add	r9, r9, #4
603094a0:	e3520007 	cmp	r2, #7
603094a4:	0a00002e 	beq	60309564 <OTP_LogicalMap_Read+0x124>
603094a8:	e0844009 	add	r4, r4, r9
603094ac:	e3140003 	tst	r4, #3
603094b0:	1a00001d 	bne	6030952c <OTP_LogicalMap_Read+0xec>
603094b4:	e3540f7f 	cmp	r4, #508	; 0x1fc
603094b8:	8a000026 	bhi	60309558 <OTP_LogicalMap_Read+0x118>
603094bc:	e28d1014 	add	r1, sp, #20
603094c0:	e1a00004 	mov	r0, r4
603094c4:	ebffffc1 	bl	603093d0 <OTP_Read32>
603094c8:	e59d3014 	ldr	r3, [sp, #20]
603094cc:	e3730001 	cmn	r3, #1
603094d0:	0a000020 	beq	60309558 <OTP_LogicalMap_Read+0x118>
603094d4:	e1a02e23 	lsr	r2, r3, #28
603094d8:	e2844004 	add	r4, r4, #4
603094dc:	e3520001 	cmp	r2, #1
603094e0:	1affffe8 	bne	60309488 <OTP_LogicalMap_Read+0x48>
603094e4:	e7e32653 	ubfx	r2, r3, #12, #4
603094e8:	e3520007 	cmp	r2, #7
603094ec:	1affffee 	bne	603094ac <OTP_LogicalMap_Read+0x6c>
603094f0:	e7eb2053 	ubfx	r2, r3, #0, #12
603094f4:	e1570002 	cmp	r7, r2
603094f8:	83a01001 	movhi	r1, #1
603094fc:	93a01000 	movls	r1, #0
60309500:	e1560002 	cmp	r6, r2
60309504:	83a01000 	movhi	r1, #0
60309508:	e7e73853 	ubfx	r3, r3, #16, #8
6030950c:	e3510000 	cmp	r1, #0
60309510:	e5cd3013 	strb	r3, [sp, #19]
60309514:	0affffe4 	beq	603094ac <OTP_LogicalMap_Read+0x6c>
60309518:	e59d1008 	ldr	r1, [sp, #8]
6030951c:	e0422006 	sub	r2, r2, r6
60309520:	e3140003 	tst	r4, #3
60309524:	e7c13002 	strb	r3, [r1, r2]
60309528:	0affffe1 	beq	603094b4 <OTP_LogicalMap_Read+0x74>
6030952c:	e59d0014 	ldr	r0, [sp, #20]
60309530:	e58d0004 	str	r0, [sp, #4]
60309534:	e1a03008 	mov	r3, r8
60309538:	e3a02045 	mov	r2, #69	; 0x45
6030953c:	e3a00002 	mov	r0, #2
60309540:	e3051bfc 	movw	r1, #23548	; 0x5bfc
60309544:	e3461031 	movt	r1, #24625	; 0x6031
60309548:	e58d4000 	str	r4, [sp]
6030954c:	eb000de9 	bl	6030ccf8 <rtk_log_write_nano>
60309550:	e3540f7f 	cmp	r4, #508	; 0x1fc
60309554:	9affffd8 	bls	603094bc <OTP_LogicalMap_Read+0x7c>
60309558:	e3a00000 	mov	r0, #0
6030955c:	e28dd01c 	add	sp, sp, #28
60309560:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
60309564:	e7eb5053 	ubfx	r5, r3, #0, #12
60309568:	e085a009 	add	sl, r5, r9
6030956c:	e1a0b004 	mov	fp, r4
60309570:	e35b0b02 	cmp	fp, #2048	; 0x800
60309574:	e1a0000b 	mov	r0, fp
60309578:	23e03000 	mvncs	r3, #0
6030957c:	e28d1013 	add	r1, sp, #19
60309580:	25cd3013 	strbcs	r3, [sp, #19]
60309584:	2a000000 	bcs	6030958c <OTP_LogicalMap_Read+0x14c>
60309588:	ebfffe47 	bl	60308eac <OTP_Read8.part.0>
6030958c:	e1560005 	cmp	r6, r5
60309590:	93a03001 	movls	r3, #1
60309594:	83a03000 	movhi	r3, #0
60309598:	e1570005 	cmp	r7, r5
6030959c:	93a03000 	movls	r3, #0
603095a0:	e3530000 	cmp	r3, #0
603095a4:	e28bb001 	add	fp, fp, #1
603095a8:	159d200c 	ldrne	r2, [sp, #12]
603095ac:	15dd3013 	ldrbne	r3, [sp, #19]
603095b0:	17c23005 	strbne	r3, [r2, r5]
603095b4:	e2855001 	add	r5, r5, #1
603095b8:	e155000a 	cmp	r5, sl
603095bc:	1affffeb 	bne	60309570 <OTP_LogicalMap_Read+0x130>
603095c0:	eaffffb8 	b	603094a8 <OTP_LogicalMap_Read+0x68>
603095c4:	e1a03002 	mov	r3, r2
603095c8:	e3a00002 	mov	r0, #2
603095cc:	e58d3004 	str	r3, [sp, #4]
603095d0:	e58d1000 	str	r1, [sp]
603095d4:	e3a02045 	mov	r2, #69	; 0x45
603095d8:	e3053c84 	movw	r3, #23684	; 0x5c84
603095dc:	e3463031 	movt	r3, #24625	; 0x6031
603095e0:	e3051bfc 	movw	r1, #23548	; 0x5bfc
603095e4:	e3461031 	movt	r1, #24625	; 0x6031
603095e8:	eb000dc2 	bl	6030ccf8 <rtk_log_write_nano>
603095ec:	e3e00000 	mvn	r0, #0
603095f0:	eaffffd9 	b	6030955c <OTP_LogicalMap_Read+0x11c>

603095f4 <OTP_LogicalMap_Write>:
603095f4:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
603095f8:	e1a03000 	mov	r3, r0
603095fc:	e24dd044 	sub	sp, sp, #68	; 0x44
60309600:	e0810000 	add	r0, r1, r0
60309604:	e3500b01 	cmp	r0, #1024	; 0x400
60309608:	e58d2014 	str	r2, [sp, #20]
6030960c:	8a00013f 	bhi	60309b10 <OTP_LogicalMap_Write+0x51c>
60309610:	e3c3200f 	bic	r2, r3, #15
60309614:	e1a04001 	mov	r4, r1
60309618:	e203300f 	and	r3, r3, #15
6030961c:	e3510000 	cmp	r1, #0
60309620:	e58d2010 	str	r2, [sp, #16]
60309624:	e58d3008 	str	r3, [sp, #8]
60309628:	da000135 	ble	60309b04 <OTP_LogicalMap_Write+0x510>
6030962c:	e305bbfc 	movw	fp, #23548	; 0x5bfc
60309630:	e346b031 	movt	fp, #24625	; 0x6031
60309634:	e3053d1c 	movw	r3, #23836	; 0x5d1c
60309638:	e3463031 	movt	r3, #24625	; 0x6031
6030963c:	e58d300c 	str	r3, [sp, #12]
60309640:	e3053dc4 	movw	r3, #24004	; 0x5dc4
60309644:	e3463031 	movt	r3, #24625	; 0x6031
60309648:	e58d3020 	str	r3, [sp, #32]
6030964c:	e59d5008 	ldr	r5, [sp, #8]
60309650:	e3a02010 	mov	r2, #16
60309654:	e0843005 	add	r3, r4, r5
60309658:	e3530010 	cmp	r3, #16
6030965c:	e59d1010 	ldr	r1, [sp, #16]
60309660:	e28d0030 	add	r0, sp, #48	; 0x30
60309664:	31a04003 	movcc	r4, r3
60309668:	21a04002 	movcs	r4, r2
6030966c:	e58d3018 	str	r3, [sp, #24]
60309670:	ebffff72 	bl	60309440 <OTP_LogicalMap_Read>
60309674:	e3700001 	cmn	r0, #1
60309678:	e58d001c 	str	r0, [sp, #28]
6030967c:	0a000102 	beq	60309a8c <OTP_LogicalMap_Write+0x498>
60309680:	e1540005 	cmp	r4, r5
60309684:	9a00008b 	bls	603098b8 <OTP_LogicalMap_Write+0x2c4>
60309688:	e59d3008 	ldr	r3, [sp, #8]
6030968c:	e59d2014 	ldr	r2, [sp, #20]
60309690:	e3a07000 	mov	r7, #0
60309694:	e2435001 	sub	r5, r3, #1
60309698:	e0426003 	sub	r6, r2, r3
6030969c:	e28d1030 	add	r1, sp, #48	; 0x30
603096a0:	e0866004 	add	r6, r6, r4
603096a4:	e0815005 	add	r5, r1, r5
603096a8:	e1a04002 	mov	r4, r2
603096ac:	e1a0a007 	mov	sl, r7
603096b0:	e1a08007 	mov	r8, r7
603096b4:	e0439002 	sub	r9, r3, r2
603096b8:	e0893004 	add	r3, r9, r4
603096bc:	e5f52001 	ldrb	r2, [r5, #1]!
603096c0:	e4d40001 	ldrb	r0, [r4], #1
603096c4:	e1a0c123 	lsr	ip, r3, #2
603096c8:	e1520000 	cmp	r2, r0
603096cc:	0a00000c 	beq	60309704 <OTP_LogicalMap_Write+0x110>
603096d0:	e3a02001 	mov	r2, #1
603096d4:	e1a01002 	mov	r1, r2
603096d8:	e18aa312 	orr	sl, sl, r2, lsl r3
603096dc:	e58d0004 	str	r0, [sp, #4]
603096e0:	e58d3000 	str	r3, [sp]
603096e4:	e1877c11 	orr	r7, r7, r1, lsl ip
603096e8:	e0888001 	add	r8, r8, r1
603096ec:	e5c50000 	strb	r0, [r5]
603096f0:	e59d300c 	ldr	r3, [sp, #12]
603096f4:	e3a02049 	mov	r2, #73	; 0x49
603096f8:	e1a0100b 	mov	r1, fp
603096fc:	e3a00004 	mov	r0, #4
60309700:	eb000d7c 	bl	6030ccf8 <rtk_log_write_nano>
60309704:	e1560004 	cmp	r6, r4
60309708:	1affffea 	bne	603096b8 <OTP_LogicalMap_Write+0xc4>
6030970c:	e3580000 	cmp	r8, #0
60309710:	0a000068 	beq	603098b8 <OTP_LogicalMap_Write+0x2c4>
60309714:	e59d3008 	ldr	r3, [sp, #8]
60309718:	e3a02000 	mov	r2, #0
6030971c:	e3a01010 	mov	r1, #16
60309720:	e1a03123 	lsr	r3, r3, #2
60309724:	e1a00337 	lsr	r0, r7, r3
60309728:	e3100001 	tst	r0, #1
6030972c:	0a000005 	beq	60309748 <OTP_LogicalMap_Write+0x154>
60309730:	e1510003 	cmp	r1, r3
60309734:	21a01003 	movcs	r1, r3
60309738:	e1520003 	cmp	r2, r3
6030973c:	31a02003 	movcc	r2, r3
60309740:	e6ef1071 	uxtb	r1, r1
60309744:	e6ef2072 	uxtb	r2, r2
60309748:	e2833001 	add	r3, r3, #1
6030974c:	e3530004 	cmp	r3, #4
60309750:	1afffff3 	bne	60309724 <OTP_LogicalMap_Write+0x130>
60309754:	e0422001 	sub	r2, r2, r1
60309758:	e2823002 	add	r3, r2, #2
6030975c:	e1580003 	cmp	r8, r3
60309760:	9a00006c 	bls	60309918 <OTP_LogicalMap_Write+0x324>
60309764:	e1a09101 	lsl	r9, r1, #2
60309768:	e2822001 	add	r2, r2, #1
6030976c:	e59d3010 	ldr	r3, [sp, #16]
60309770:	e6ef9079 	uxtb	r9, r9
60309774:	e1a04102 	lsl	r4, r2, #2
60309778:	e0896003 	add	r6, r9, r3
6030977c:	e6ef4074 	uxtb	r4, r4
60309780:	e28d3030 	add	r3, sp, #48	; 0x30
60309784:	e3540010 	cmp	r4, #16
60309788:	e0839009 	add	r9, r3, r9
6030978c:	e6ff6076 	uxth	r6, r6
60309790:	8a0000b2 	bhi	60309a60 <OTP_LogicalMap_Write+0x46c>
60309794:	e3560b01 	cmp	r6, #1024	; 0x400
60309798:	8a0000c7 	bhi	60309abc <OTP_LogicalMap_Write+0x4c8>
6030979c:	e3540000 	cmp	r4, #0
603097a0:	0a00000f 	beq	603097e4 <OTP_LogicalMap_Write+0x1f0>
603097a4:	e2495001 	sub	r5, r9, #1
603097a8:	e305ad88 	movw	sl, #23944	; 0x5d88
603097ac:	e346a031 	movt	sl, #24625	; 0x6031
603097b0:	e0858004 	add	r8, r5, r4
603097b4:	e2697001 	rsb	r7, r9, #1
603097b8:	e0873005 	add	r3, r7, r5
603097bc:	e58d3000 	str	r3, [sp]
603097c0:	e1a0100b 	mov	r1, fp
603097c4:	e5f52001 	ldrb	r2, [r5, #1]!
603097c8:	e58d2004 	str	r2, [sp, #4]
603097cc:	e1a0300a 	mov	r3, sl
603097d0:	e3a02049 	mov	r2, #73	; 0x49
603097d4:	e3a00004 	mov	r0, #4
603097d8:	eb000d46 	bl	6030ccf8 <rtk_log_write_nano>
603097dc:	e1580005 	cmp	r8, r5
603097e0:	1afffff4 	bne	603097b8 <OTP_LogicalMap_Write+0x1c4>
603097e4:	e3a05000 	mov	r5, #0
603097e8:	e28d702c 	add	r7, sp, #44	; 0x2c
603097ec:	ea000004 	b	60309804 <OTP_LogicalMap_Write+0x210>
603097f0:	e3520003 	cmp	r2, #3
603097f4:	02855008 	addeq	r5, r5, #8
603097f8:	11a05001 	movne	r5, r1
603097fc:	e3550f7f 	cmp	r5, #508	; 0x1fc
60309800:	8a00000e 	bhi	60309840 <OTP_LogicalMap_Write+0x24c>
60309804:	e1a01007 	mov	r1, r7
60309808:	e1a00005 	mov	r0, r5
6030980c:	ebfffeef 	bl	603093d0 <OTP_Read32>
60309810:	e59d302c 	ldr	r3, [sp, #44]	; 0x2c
60309814:	e2851004 	add	r1, r5, #4
60309818:	e3730001 	cmn	r3, #1
6030981c:	e1a02e23 	lsr	r2, r3, #28
60309820:	0a000006 	beq	60309840 <OTP_LogicalMap_Write+0x24c>
60309824:	e3520002 	cmp	r2, #2
60309828:	1afffff0 	bne	603097f0 <OTP_LogicalMap_Write+0x1fc>
6030982c:	e7e33c53 	ubfx	r3, r3, #24, #4
60309830:	e2833001 	add	r3, r3, #1
60309834:	e0815103 	add	r5, r1, r3, lsl #2
60309838:	e3550f7f 	cmp	r5, #508	; 0x1fc
6030983c:	9afffff0 	bls	60309804 <OTP_LogicalMap_Write+0x210>
60309840:	e0852004 	add	r2, r5, r4
60309844:	e30031fd 	movw	r3, #509	; 0x1fd
60309848:	e1520003 	cmp	r2, r3
6030984c:	8a0000a2 	bhi	60309adc <OTP_LogicalMap_Write+0x4e8>
60309850:	e6ef1076 	uxtb	r1, r6
60309854:	e1a00005 	mov	r0, r5
60309858:	ebfffe4c 	bl	60309190 <OTP_Write8>
6030985c:	e7e31456 	ubfx	r1, r6, #8, #4
60309860:	e3811070 	orr	r1, r1, #112	; 0x70
60309864:	e2850001 	add	r0, r5, #1
60309868:	ebfffe48 	bl	60309190 <OTP_Write8>
6030986c:	e1a01144 	asr	r1, r4, #2
60309870:	e2411001 	sub	r1, r1, #1
60309874:	e3811020 	orr	r1, r1, #32
60309878:	e2850003 	add	r0, r5, #3
6030987c:	e6ef1071 	uxtb	r1, r1
60309880:	ebfffe42 	bl	60309190 <OTP_Write8>
60309884:	e3540000 	cmp	r4, #0
60309888:	12496001 	subne	r6, r9, #1
6030988c:	12699005 	rsbne	r9, r9, #5
60309890:	10895005 	addne	r5, r9, r5
60309894:	10864004 	addne	r4, r6, r4
60309898:	0a000004 	beq	603098b0 <OTP_LogicalMap_Write+0x2bc>
6030989c:	e0850006 	add	r0, r5, r6
603098a0:	e5f61001 	ldrb	r1, [r6, #1]!
603098a4:	ebfffe39 	bl	60309190 <OTP_Write8>
603098a8:	e1560004 	cmp	r6, r4
603098ac:	1afffffa 	bne	6030989c <OTP_LogicalMap_Write+0x2a8>
603098b0:	e3a03000 	mov	r3, #0
603098b4:	e58d301c 	str	r3, [sp, #28]
603098b8:	e59d3018 	ldr	r3, [sp, #24]
603098bc:	e59de014 	ldr	lr, [sp, #20]
603098c0:	e2434010 	sub	r4, r3, #16
603098c4:	e59d3008 	ldr	r3, [sp, #8]
603098c8:	e3a02049 	mov	r2, #73	; 0x49
603098cc:	e263c010 	rsb	ip, r3, #16
603098d0:	e59d3010 	ldr	r3, [sp, #16]
603098d4:	e1a0100b 	mov	r1, fp
603098d8:	e2833010 	add	r3, r3, #16
603098dc:	e3a00004 	mov	r0, #4
603098e0:	e58d3010 	str	r3, [sp, #16]
603098e4:	e88d0018 	stm	sp, {r3, r4}
603098e8:	e3053de4 	movw	r3, #24036	; 0x5de4
603098ec:	e3463031 	movt	r3, #24625	; 0x6031
603098f0:	e08ec00c 	add	ip, lr, ip
603098f4:	e58dc014 	str	ip, [sp, #20]
603098f8:	e3a0c000 	mov	ip, #0
603098fc:	e58dc008 	str	ip, [sp, #8]
60309900:	eb000cfc 	bl	6030ccf8 <rtk_log_write_nano>
60309904:	e3540000 	cmp	r4, #0
60309908:	caffff4f 	bgt	6030964c <OTP_LogicalMap_Write+0x58>
6030990c:	e59d001c 	ldr	r0, [sp, #28]
60309910:	e28dd044 	add	sp, sp, #68	; 0x44
60309914:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
60309918:	e3053da0 	movw	r3, #23968	; 0x5da0
6030991c:	e3463031 	movt	r3, #24625	; 0x6031
60309920:	e1dd51b0 	ldrh	r5, [sp, #16]
60309924:	e59d701c 	ldr	r7, [sp, #28]
60309928:	e28d8030 	add	r8, sp, #48	; 0x30
6030992c:	e3a06000 	mov	r6, #0
60309930:	e58d3024 	str	r3, [sp, #36]	; 0x24
60309934:	ea000005 	b	60309950 <OTP_LogicalMap_Write+0x35c>
60309938:	e2855001 	add	r5, r5, #1
6030993c:	e2866001 	add	r6, r6, #1
60309940:	e6ff5075 	uxth	r5, r5
60309944:	e2888001 	add	r8, r8, #1
60309948:	e3560010 	cmp	r6, #16
6030994c:	0a000041 	beq	60309a58 <OTP_LogicalMap_Write+0x464>
60309950:	e3a03001 	mov	r3, #1
60309954:	e01a3613 	ands	r3, sl, r3, lsl r6
60309958:	0afffff6 	beq	60309938 <OTP_LogicalMap_Write+0x344>
6030995c:	e5d89000 	ldrb	r9, [r8]
60309960:	e3550b01 	cmp	r5, #1024	; 0x400
60309964:	8a000033 	bhi	60309a38 <OTP_LogicalMap_Write+0x444>
60309968:	e59d3020 	ldr	r3, [sp, #32]
6030996c:	e3a02049 	mov	r2, #73	; 0x49
60309970:	e1a0100b 	mov	r1, fp
60309974:	e3a00004 	mov	r0, #4
60309978:	e88d0220 	stm	sp, {r5, r9}
6030997c:	e3a04000 	mov	r4, #0
60309980:	e28d702c 	add	r7, sp, #44	; 0x2c
60309984:	eb000cdb 	bl	6030ccf8 <rtk_log_write_nano>
60309988:	ea000004 	b	603099a0 <OTP_LogicalMap_Write+0x3ac>
6030998c:	e3520003 	cmp	r2, #3
60309990:	02844008 	addeq	r4, r4, #8
60309994:	11a04001 	movne	r4, r1
60309998:	e3540f7f 	cmp	r4, #508	; 0x1fc
6030999c:	8a00000e 	bhi	603099dc <OTP_LogicalMap_Write+0x3e8>
603099a0:	e1a01007 	mov	r1, r7
603099a4:	e1a00004 	mov	r0, r4
603099a8:	ebfffe88 	bl	603093d0 <OTP_Read32>
603099ac:	e59d302c 	ldr	r3, [sp, #44]	; 0x2c
603099b0:	e2841004 	add	r1, r4, #4
603099b4:	e3730001 	cmn	r3, #1
603099b8:	e1a02e23 	lsr	r2, r3, #28
603099bc:	0a00000e 	beq	603099fc <OTP_LogicalMap_Write+0x408>
603099c0:	e3520002 	cmp	r2, #2
603099c4:	1afffff0 	bne	6030998c <OTP_LogicalMap_Write+0x398>
603099c8:	e7e33c53 	ubfx	r3, r3, #24, #4
603099cc:	e2833001 	add	r3, r3, #1
603099d0:	e0814103 	add	r4, r1, r3, lsl #2
603099d4:	e3540f7f 	cmp	r4, #508	; 0x1fc
603099d8:	9afffff0 	bls	603099a0 <OTP_LogicalMap_Write+0x3ac>
603099dc:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
603099e0:	e3a02045 	mov	r2, #69	; 0x45
603099e4:	e1a0100b 	mov	r1, fp
603099e8:	e3a00002 	mov	r0, #2
603099ec:	e58d4000 	str	r4, [sp]
603099f0:	e3e07000 	mvn	r7, #0
603099f4:	eb000cbf 	bl	6030ccf8 <rtk_log_write_nano>
603099f8:	eaffffce 	b	60309938 <OTP_LogicalMap_Write+0x344>
603099fc:	e6ef1075 	uxtb	r1, r5
60309a00:	e1a00004 	mov	r0, r4
60309a04:	ebfffde1 	bl	60309190 <OTP_Write8>
60309a08:	e7e31455 	ubfx	r1, r5, #8, #4
60309a0c:	e3811070 	orr	r1, r1, #112	; 0x70
60309a10:	e2840001 	add	r0, r4, #1
60309a14:	ebfffddd 	bl	60309190 <OTP_Write8>
60309a18:	e1a01009 	mov	r1, r9
60309a1c:	e2840002 	add	r0, r4, #2
60309a20:	ebfffdda 	bl	60309190 <OTP_Write8>
60309a24:	e2840003 	add	r0, r4, #3
60309a28:	e3a0101f 	mov	r1, #31
60309a2c:	e3a07000 	mov	r7, #0
60309a30:	ebfffdd6 	bl	60309190 <OTP_Write8>
60309a34:	eaffffbf 	b	60309938 <OTP_LogicalMap_Write+0x344>
60309a38:	e3a02045 	mov	r2, #69	; 0x45
60309a3c:	e1a0100b 	mov	r1, fp
60309a40:	e3a00002 	mov	r0, #2
60309a44:	e58d5000 	str	r5, [sp]
60309a48:	e3053d5c 	movw	r3, #23900	; 0x5d5c
60309a4c:	e3463031 	movt	r3, #24625	; 0x6031
60309a50:	eb000ca8 	bl	6030ccf8 <rtk_log_write_nano>
60309a54:	eaffffc3 	b	60309968 <OTP_LogicalMap_Write+0x374>
60309a58:	e58d701c 	str	r7, [sp, #28]
60309a5c:	eaffff95 	b	603098b8 <OTP_LogicalMap_Write+0x2c4>
60309a60:	e3a02045 	mov	r2, #69	; 0x45
60309a64:	e1a0100b 	mov	r1, fp
60309a68:	e3a00002 	mov	r0, #2
60309a6c:	e58d4004 	str	r4, [sp, #4]
60309a70:	e58d6000 	str	r6, [sp]
60309a74:	e3053d30 	movw	r3, #23856	; 0x5d30
60309a78:	e3463031 	movt	r3, #24625	; 0x6031
60309a7c:	eb000c9d 	bl	6030ccf8 <rtk_log_write_nano>
60309a80:	e3e03000 	mvn	r3, #0
60309a84:	e58d301c 	str	r3, [sp, #28]
60309a88:	eaffff8a 	b	603098b8 <OTP_LogicalMap_Write+0x2c4>
60309a8c:	e59d3010 	ldr	r3, [sp, #16]
60309a90:	e58d3000 	str	r3, [sp]
60309a94:	e3a02045 	mov	r2, #69	; 0x45
60309a98:	e3a00002 	mov	r0, #2
60309a9c:	e3053cf4 	movw	r3, #23796	; 0x5cf4
60309aa0:	e3463031 	movt	r3, #24625	; 0x6031
60309aa4:	e3051bfc 	movw	r1, #23548	; 0x5bfc
60309aa8:	e3461031 	movt	r1, #24625	; 0x6031
60309aac:	eb000c91 	bl	6030ccf8 <rtk_log_write_nano>
60309ab0:	e59d001c 	ldr	r0, [sp, #28]
60309ab4:	e28dd044 	add	sp, sp, #68	; 0x44
60309ab8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
60309abc:	e3a02045 	mov	r2, #69	; 0x45
60309ac0:	e1a0100b 	mov	r1, fp
60309ac4:	e3a00002 	mov	r0, #2
60309ac8:	e58d6000 	str	r6, [sp]
60309acc:	e3053d5c 	movw	r3, #23900	; 0x5d5c
60309ad0:	e3463031 	movt	r3, #24625	; 0x6031
60309ad4:	eb000c87 	bl	6030ccf8 <rtk_log_write_nano>
60309ad8:	eaffff2f 	b	6030979c <OTP_LogicalMap_Write+0x1a8>
60309adc:	e3a02045 	mov	r2, #69	; 0x45
60309ae0:	e1a0100b 	mov	r1, fp
60309ae4:	e3a00002 	mov	r0, #2
60309ae8:	e58d5000 	str	r5, [sp]
60309aec:	e3053da0 	movw	r3, #23968	; 0x5da0
60309af0:	e3463031 	movt	r3, #24625	; 0x6031
60309af4:	eb000c7f 	bl	6030ccf8 <rtk_log_write_nano>
60309af8:	e3e03000 	mvn	r3, #0
60309afc:	e58d301c 	str	r3, [sp, #28]
60309b00:	eaffff6c 	b	603098b8 <OTP_LogicalMap_Write+0x2c4>
60309b04:	e3a03000 	mov	r3, #0
60309b08:	e58d301c 	str	r3, [sp, #28]
60309b0c:	eaffff7e 	b	6030990c <OTP_LogicalMap_Write+0x318>
60309b10:	e58d1004 	str	r1, [sp, #4]
60309b14:	e58d3000 	str	r3, [sp]
60309b18:	e3a02045 	mov	r2, #69	; 0x45
60309b1c:	e3a00002 	mov	r0, #2
60309b20:	e3053cc8 	movw	r3, #23752	; 0x5cc8
60309b24:	e3463031 	movt	r3, #24625	; 0x6031
60309b28:	e3051bfc 	movw	r1, #23548	; 0x5bfc
60309b2c:	e3461031 	movt	r1, #24625	; 0x6031
60309b30:	e3e0c000 	mvn	ip, #0
60309b34:	e58dc01c 	str	ip, [sp, #28]
60309b38:	eb000c6e 	bl	6030ccf8 <rtk_log_write_nano>
60309b3c:	eaffff72 	b	6030990c <OTP_LogicalMap_Write+0x318>

60309b40 <otp_logical_remain>:
60309b40:	e92d4010 	push	{r4, lr}
60309b44:	e3a04000 	mov	r4, #0
60309b48:	e24dd008 	sub	sp, sp, #8
60309b4c:	ea000004 	b	60309b64 <otp_logical_remain+0x24>
60309b50:	e3520003 	cmp	r2, #3
60309b54:	02844008 	addeq	r4, r4, #8
60309b58:	11a04001 	movne	r4, r1
60309b5c:	e3540f7f 	cmp	r4, #508	; 0x1fc
60309b60:	8a00000e 	bhi	60309ba0 <otp_logical_remain+0x60>
60309b64:	e28d1004 	add	r1, sp, #4
60309b68:	e1a00004 	mov	r0, r4
60309b6c:	ebfffe17 	bl	603093d0 <OTP_Read32>
60309b70:	e59d3004 	ldr	r3, [sp, #4]
60309b74:	e2841004 	add	r1, r4, #4
60309b78:	e3730001 	cmn	r3, #1
60309b7c:	e1a02e23 	lsr	r2, r3, #28
60309b80:	0a000006 	beq	60309ba0 <otp_logical_remain+0x60>
60309b84:	e3520002 	cmp	r2, #2
60309b88:	1afffff0 	bne	60309b50 <otp_logical_remain+0x10>
60309b8c:	e7e33c53 	ubfx	r3, r3, #24, #4
60309b90:	e2833001 	add	r3, r3, #1
60309b94:	e0814103 	add	r4, r1, r3, lsl #2
60309b98:	e3540f7f 	cmp	r4, #508	; 0x1fc
60309b9c:	9afffff0 	bls	60309b64 <otp_logical_remain+0x24>
60309ba0:	e2640f7f 	rsb	r0, r4, #508	; 0x1fc
60309ba4:	e2800001 	add	r0, r0, #1
60309ba8:	e28dd008 	add	sp, sp, #8
60309bac:	e8bd8010 	pop	{r4, pc}

60309bb0 <PLL_GetHBUSClk>:
60309bb0:	e3a03b22 	mov	r3, #34816	; 0x8800
60309bb4:	e3443200 	movt	r3, #16896	; 0x4200
60309bb8:	e3a01902 	mov	r1, #32768	; 0x8000
60309bbc:	e3441200 	movt	r1, #16896	; 0x4200
60309bc0:	e5933044 	ldr	r3, [r3, #68]	; 0x44
60309bc4:	e3a02c5a 	mov	r2, #23040	; 0x5a00
60309bc8:	e3402262 	movt	r2, #610	; 0x262
60309bcc:	e7e532d3 	ubfx	r3, r3, #5, #6
60309bd0:	e5910228 	ldr	r0, [r1, #552]	; 0x228
60309bd4:	e2833002 	add	r3, r3, #2
60309bd8:	e7e30650 	ubfx	r0, r0, #12, #4
60309bdc:	e0030392 	mul	r3, r2, r3
60309be0:	e2800001 	add	r0, r0, #1
60309be4:	e730f013 	udiv	r0, r3, r0
60309be8:	e12fff1e 	bx	lr

60309bec <System_Reset>:
60309bec:	e3a03902 	mov	r3, #32768	; 0x8000
60309bf0:	e3443200 	movt	r3, #16896	; 0x4200
60309bf4:	e593227c 	ldr	r2, [r3, #636]	; 0x27c
60309bf8:	e7e12352 	ubfx	r2, r2, #6, #2
60309bfc:	e3520003 	cmp	r2, #3
60309c00:	012fff1e 	bxeq	lr
60309c04:	e3a01202 	mov	r1, #536870912	; 0x20000000
60309c08:	e3090696 	movw	r0, #38550	; 0x9696
60309c0c:	e3490696 	movt	r0, #38550	; 0x9696
60309c10:	e1a02211 	lsl	r2, r1, r2
60309c14:	e583023c 	str	r0, [r3, #572]	; 0x23c
60309c18:	e3061969 	movw	r1, #26985	; 0x6969
60309c1c:	e3461969 	movt	r1, #26985	; 0x6969
60309c20:	e5832238 	str	r2, [r3, #568]	; 0x238
60309c24:	e583123c 	str	r1, [r3, #572]	; 0x23c
60309c28:	eafffffe 	b	60309c28 <System_Reset+0x3c>

60309c2c <vAssertCalled>:
60309c2c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
60309c30:	e1a04000 	mov	r4, r0
60309c34:	e24dd00c 	sub	sp, sp, #12
60309c38:	e1a05001 	mov	r5, r1
60309c3c:	eb000d8e 	bl	6030d27c <__rtos_critical_enter_os>
60309c40:	e3053e0c 	movw	r3, #24076	; 0x5e0c
60309c44:	e3463031 	movt	r3, #24625	; 0x6031
60309c48:	e3051394 	movw	r1, #21396	; 0x5394
60309c4c:	e3461031 	movt	r1, #24625	; 0x6031
60309c50:	e3a02045 	mov	r2, #69	; 0x45
60309c54:	e3a00002 	mov	r0, #2
60309c58:	e58d5004 	str	r5, [sp, #4]
60309c5c:	e58d4000 	str	r4, [sp]
60309c60:	eb000c24 	bl	6030ccf8 <rtk_log_write_nano>
60309c64:	eafffffe 	b	60309c64 <vAssertCalled+0x38>

60309c68 <CPU_InInterrupt>:
60309c68:	e10f3000 	mrs	r3, CPSR
60309c6c:	e203301f 	and	r3, r3, #31
60309c70:	e3530010 	cmp	r3, #16
60309c74:	0a000004 	beq	60309c8c <CPU_InInterrupt+0x24>
60309c78:	e10f0000 	mrs	r0, CPSR
60309c7c:	e200001f 	and	r0, r0, #31
60309c80:	e250001f 	subs	r0, r0, #31
60309c84:	13a00001 	movne	r0, #1
60309c88:	e12fff1e 	bx	lr
60309c8c:	e3a00000 	mov	r0, #0
60309c90:	e12fff1e 	bx	lr

60309c94 <linux_ipc_otp_instruction>:
60309c94:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60309c98:	e3a01006 	mov	r1, #6
60309c9c:	e3a00021 	mov	r0, #33	; 0x21
60309ca0:	ebfffbad 	bl	60308b5c <ipc_get_message>
60309ca4:	e5904004 	ldr	r4, [r0, #4]
60309ca8:	e5903008 	ldr	r3, [r0, #8]
60309cac:	ee112f10 	mrc	15, 0, r2, cr1, cr0, {0}
60309cb0:	e3120004 	tst	r2, #4
60309cb4:	0a00000e 	beq	60309cf4 <linux_ipc_otp_instruction+0x60>
60309cb8:	e0032004 	and	r2, r3, r4
60309cbc:	e3720001 	cmn	r2, #1
60309cc0:	0a00005b 	beq	60309e34 <linux_ipc_otp_instruction+0x1a0>
60309cc4:	e0832004 	add	r2, r3, r4
60309cc8:	e2422001 	sub	r2, r2, #1
60309ccc:	e3c2203f 	bic	r2, r2, #63	; 0x3f
60309cd0:	e3c4303f 	bic	r3, r4, #63	; 0x3f
60309cd4:	e2822040 	add	r2, r2, #64	; 0x40
60309cd8:	e1530002 	cmp	r3, r2
60309cdc:	2a000003 	bcs	60309cf0 <linux_ipc_otp_instruction+0x5c>
60309ce0:	ee073f3e 	mcr	15, 0, r3, cr7, cr14, {1}
60309ce4:	e2833040 	add	r3, r3, #64	; 0x40
60309ce8:	e1520003 	cmp	r2, r3
60309cec:	8afffffb 	bhi	60309ce0 <linux_ipc_otp_instruction+0x4c>
60309cf0:	f57ff05f 	dmb	sy
60309cf4:	e3540000 	cmp	r4, #0
60309cf8:	0a00000b 	beq	60309d2c <linux_ipc_otp_instruction+0x98>
60309cfc:	e5943008 	ldr	r3, [r4, #8]
60309d00:	e3530b01 	cmp	r3, #1024	; 0x400
60309d04:	8a000008 	bhi	60309d2c <linux_ipc_otp_instruction+0x98>
60309d08:	e5945000 	ldr	r5, [r4]
60309d0c:	e3550004 	cmp	r5, #4
60309d10:	979ff105 	ldrls	pc, [pc, r5, lsl #2]
60309d14:	ea000073 	b	60309ee8 <linux_ipc_otp_instruction+0x254>
60309d18:	6030a3e4 	.word	0x6030a3e4
60309d1c:	6030a154 	.word	0x6030a154
60309d20:	6030a278 	.word	0x6030a278
60309d24:	6030a064 	.word	0x6030a064
60309d28:	60309ef4 	.word	0x60309ef4
60309d2c:	e307c5c4 	movw	ip, #30148	; 0x75c4
60309d30:	e346c031 	movt	ip, #24625	; 0x6031
60309d34:	e3a03000 	mov	r3, #0
60309d38:	e3e02000 	mvn	r2, #0
60309d3c:	e1cc20f0 	strd	r2, [ip]
60309d40:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
60309d44:	e3130004 	tst	r3, #4
60309d48:	0a00000a 	beq	60309d78 <linux_ipc_otp_instruction+0xe4>
60309d4c:	e28c2001 	add	r2, ip, #1
60309d50:	e3c2203f 	bic	r2, r2, #63	; 0x3f
60309d54:	e3cc303f 	bic	r3, ip, #63	; 0x3f
60309d58:	e2822040 	add	r2, r2, #64	; 0x40
60309d5c:	e1530002 	cmp	r3, r2
60309d60:	2a000003 	bcs	60309d74 <linux_ipc_otp_instruction+0xe0>
60309d64:	ee073f3a 	mcr	15, 0, r3, cr7, cr10, {1}
60309d68:	e2833040 	add	r3, r3, #64	; 0x40
60309d6c:	e1520003 	cmp	r2, r3
60309d70:	8afffffb 	bhi	60309d64 <linux_ipc_otp_instruction+0xd0>
60309d74:	f57ff05f 	dmb	sy
60309d78:	e30745b4 	movw	r4, #30132	; 0x75b4
60309d7c:	e3464031 	movt	r4, #24625	; 0x6031
60309d80:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
60309d84:	e3130004 	tst	r3, #4
60309d88:	0a00000a 	beq	60309db8 <linux_ipc_otp_instruction+0x124>
60309d8c:	e284200f 	add	r2, r4, #15
60309d90:	e3c2203f 	bic	r2, r2, #63	; 0x3f
60309d94:	e3c4303f 	bic	r3, r4, #63	; 0x3f
60309d98:	e2822040 	add	r2, r2, #64	; 0x40
60309d9c:	e1530002 	cmp	r3, r2
60309da0:	2a000003 	bcs	60309db4 <linux_ipc_otp_instruction+0x120>
60309da4:	ee073f36 	mcr	15, 0, r3, cr7, cr6, {1}
60309da8:	e2833040 	add	r3, r3, #64	; 0x40
60309dac:	e1520003 	cmp	r2, r3
60309db0:	8afffffb 	bhi	60309da4 <linux_ipc_otp_instruction+0x110>
60309db4:	f57ff05f 	dmb	sy
60309db8:	e3a01006 	mov	r1, #6
60309dbc:	e3a00011 	mov	r0, #17
60309dc0:	e30725b4 	movw	r2, #30132	; 0x75b4
60309dc4:	e3462031 	movt	r2, #24625	; 0x6031
60309dc8:	e3a03000 	mov	r3, #0
60309dcc:	e8841008 	stm	r4, {r3, ip}
60309dd0:	e3a03002 	mov	r3, #2
60309dd4:	e5843008 	str	r3, [r4, #8]
60309dd8:	ebfffb17 	bl	60308a3c <ipc_send_message>
60309ddc:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
60309de0:	e3130004 	tst	r3, #4
60309de4:	0a00000a 	beq	60309e14 <linux_ipc_otp_instruction+0x180>
60309de8:	e59f28dc 	ldr	r2, [pc, #2268]	; 6030a6cc <linux_ipc_otp_instruction+0xa38>
60309dec:	e3c4303f 	bic	r3, r4, #63	; 0x3f
60309df0:	e3c2203f 	bic	r2, r2, #63	; 0x3f
60309df4:	e2822040 	add	r2, r2, #64	; 0x40
60309df8:	e1530002 	cmp	r3, r2
60309dfc:	2a000003 	bcs	60309e10 <linux_ipc_otp_instruction+0x17c>
60309e00:	ee073f3e 	mcr	15, 0, r3, cr7, cr14, {1}
60309e04:	e2833040 	add	r3, r3, #64	; 0x40
60309e08:	e1520003 	cmp	r2, r3
60309e0c:	8afffffb 	bhi	60309e00 <linux_ipc_otp_instruction+0x16c>
60309e10:	f57ff05f 	dmb	sy
60309e14:	e3053e24 	movw	r3, #24100	; 0x5e24
60309e18:	e3463031 	movt	r3, #24625	; 0x6031
60309e1c:	e3051a08 	movw	r1, #23048	; 0x5a08
60309e20:	e3461031 	movt	r1, #24625	; 0x6031
60309e24:	e8bd41f0 	pop	{r4, r5, r6, r7, r8, lr}
60309e28:	e3a02045 	mov	r2, #69	; 0x45
60309e2c:	e3a00002 	mov	r0, #2
60309e30:	ea000b8c 	b	6030cc68 <rtk_log_write>
60309e34:	ee307f30 	mrc	15, 1, r7, cr0, cr0, {1}
60309e38:	e3a05000 	mov	r5, #0
60309e3c:	e1a06005 	mov	r6, r5
60309e40:	e1a03637 	lsr	r3, r7, r6
60309e44:	e2033007 	and	r3, r3, #7
60309e48:	e2433002 	sub	r3, r3, #2
60309e4c:	e3530002 	cmp	r3, #2
60309e50:	8a00001f 	bhi	60309ed4 <linux_ipc_otp_instruction+0x240>
60309e54:	ee405f10 	mcr	15, 2, r5, cr0, cr0, {0}
60309e58:	ee308f10 	mrc	15, 1, r8, cr0, cr0, {0}
60309e5c:	e7e9c1d8 	ubfx	ip, r8, #3, #10
60309e60:	e35c0000 	cmp	ip, #0
60309e64:	e28ce001 	add	lr, ip, #1
60309e68:	0a00001f 	beq	60309eec <linux_ipc_otp_instruction+0x258>
60309e6c:	e1a0200e 	mov	r2, lr
60309e70:	e3a03000 	mov	r3, #0
60309e74:	e2831001 	add	r1, r3, #1
60309e78:	e1a020a2 	lsr	r2, r2, #1
60309e7c:	e1a00003 	mov	r0, r3
60309e80:	e3520001 	cmp	r2, #1
60309e84:	e6ef3071 	uxtb	r3, r1
60309e88:	1afffff9 	bne	60309e74 <linux_ipc_otp_instruction+0x1e0>
60309e8c:	e31e0001 	tst	lr, #1
60309e90:	12803002 	addne	r3, r0, #2
60309e94:	16ef3073 	uxtbne	r3, r3
60309e98:	e3530020 	cmp	r3, #32
60309e9c:	9263e020 	rsbls	lr, r3, #32
60309ea0:	8a00000b 	bhi	60309ed4 <linux_ipc_otp_instruction+0x240>
60309ea4:	e2080007 	and	r0, r8, #7
60309ea8:	e2800004 	add	r0, r0, #4
60309eac:	e7ee86d8 	ubfx	r8, r8, #13, #15
60309eb0:	e1851e1c 	orr	r1, r5, ip, lsl lr
60309eb4:	e1a03008 	mov	r3, r8
60309eb8:	e1812013 	orr	r2, r1, r3, lsl r0
60309ebc:	ee072f5e 	mcr	15, 0, r2, cr7, cr14, {2}
60309ec0:	e2533001 	subs	r3, r3, #1
60309ec4:	2afffffb 	bcs	60309eb8 <linux_ipc_otp_instruction+0x224>
60309ec8:	e25cc001 	subs	ip, ip, #1
60309ecc:	2afffff7 	bcs	60309eb0 <linux_ipc_otp_instruction+0x21c>
60309ed0:	f57ff05f 	dmb	sy
60309ed4:	e2866003 	add	r6, r6, #3
60309ed8:	e2855002 	add	r5, r5, #2
60309edc:	e3560015 	cmp	r6, #21
60309ee0:	1affffd6 	bne	60309e40 <linux_ipc_otp_instruction+0x1ac>
60309ee4:	eaffff82 	b	60309cf4 <linux_ipc_otp_instruction+0x60>
60309ee8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60309eec:	e3a0e020 	mov	lr, #32
60309ef0:	eaffffeb 	b	60309ea4 <linux_ipc_otp_instruction+0x210>
60309ef4:	e2845010 	add	r5, r4, #16
60309ef8:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
60309efc:	e3130004 	tst	r3, #4
60309f00:	0a00000b 	beq	60309f34 <linux_ipc_otp_instruction+0x2a0>
60309f04:	e2852fff 	add	r2, r5, #1020	; 0x3fc
60309f08:	e2822003 	add	r2, r2, #3
60309f0c:	e3c2103f 	bic	r1, r2, #63	; 0x3f
60309f10:	e2811040 	add	r1, r1, #64	; 0x40
60309f14:	e3c5203f 	bic	r2, r5, #63	; 0x3f
60309f18:	e1520001 	cmp	r2, r1
60309f1c:	2a000003 	bcs	60309f30 <linux_ipc_otp_instruction+0x29c>
60309f20:	ee072f36 	mcr	15, 0, r2, cr7, cr6, {1}
60309f24:	e2822040 	add	r2, r2, #64	; 0x40
60309f28:	e1510002 	cmp	r1, r2
60309f2c:	8afffffb 	bhi	60309f20 <linux_ipc_otp_instruction+0x28c>
60309f30:	f57ff05f 	dmb	sy
60309f34:	ebffff01 	bl	60309b40 <otp_logical_remain>
60309f38:	e6bf3f30 	rev	r3, r0
60309f3c:	e5843010 	str	r3, [r4, #16]
60309f40:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
60309f44:	e3130004 	tst	r3, #4
60309f48:	0a00000b 	beq	60309f7c <linux_ipc_otp_instruction+0x2e8>
60309f4c:	e2852fff 	add	r2, r5, #1020	; 0x3fc
60309f50:	e2822003 	add	r2, r2, #3
60309f54:	e3c2203f 	bic	r2, r2, #63	; 0x3f
60309f58:	e3c5303f 	bic	r3, r5, #63	; 0x3f
60309f5c:	e2822040 	add	r2, r2, #64	; 0x40
60309f60:	e1530002 	cmp	r3, r2
60309f64:	2a000003 	bcs	60309f78 <linux_ipc_otp_instruction+0x2e4>
60309f68:	ee073f3e 	mcr	15, 0, r3, cr7, cr14, {1}
60309f6c:	e2833040 	add	r3, r3, #64	; 0x40
60309f70:	e1520003 	cmp	r2, r3
60309f74:	8afffffb 	bhi	60309f68 <linux_ipc_otp_instruction+0x2d4>
60309f78:	f57ff05f 	dmb	sy
60309f7c:	e307c5c4 	movw	ip, #30148	; 0x75c4
60309f80:	e346c031 	movt	ip, #24625	; 0x6031
60309f84:	e3a03001 	mov	r3, #1
60309f88:	e58c3000 	str	r3, [ip]
60309f8c:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
60309f90:	e3130004 	tst	r3, #4
60309f94:	0a00000a 	beq	60309fc4 <linux_ipc_otp_instruction+0x330>
60309f98:	e28c2001 	add	r2, ip, #1
60309f9c:	e3c2203f 	bic	r2, r2, #63	; 0x3f
60309fa0:	e3cc303f 	bic	r3, ip, #63	; 0x3f
60309fa4:	e2822040 	add	r2, r2, #64	; 0x40
60309fa8:	e1530002 	cmp	r3, r2
60309fac:	2a000003 	bcs	60309fc0 <linux_ipc_otp_instruction+0x32c>
60309fb0:	ee073f3a 	mcr	15, 0, r3, cr7, cr10, {1}
60309fb4:	e2833040 	add	r3, r3, #64	; 0x40
60309fb8:	e1520003 	cmp	r2, r3
60309fbc:	8afffffb 	bhi	60309fb0 <linux_ipc_otp_instruction+0x31c>
60309fc0:	f57ff05f 	dmb	sy
60309fc4:	e30745b4 	movw	r4, #30132	; 0x75b4
60309fc8:	e3464031 	movt	r4, #24625	; 0x6031
60309fcc:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
60309fd0:	e3130004 	tst	r3, #4
60309fd4:	0a00000a 	beq	6030a004 <linux_ipc_otp_instruction+0x370>
60309fd8:	e284200f 	add	r2, r4, #15
60309fdc:	e3c2203f 	bic	r2, r2, #63	; 0x3f
60309fe0:	e3c4303f 	bic	r3, r4, #63	; 0x3f
60309fe4:	e2822040 	add	r2, r2, #64	; 0x40
60309fe8:	e1530002 	cmp	r3, r2
60309fec:	2a000003 	bcs	6030a000 <linux_ipc_otp_instruction+0x36c>
60309ff0:	ee073f36 	mcr	15, 0, r3, cr7, cr6, {1}
60309ff4:	e2833040 	add	r3, r3, #64	; 0x40
60309ff8:	e1520003 	cmp	r2, r3
60309ffc:	8afffffb 	bhi	60309ff0 <linux_ipc_otp_instruction+0x35c>
6030a000:	f57ff05f 	dmb	sy
6030a004:	e3a01006 	mov	r1, #6
6030a008:	e3a00011 	mov	r0, #17
6030a00c:	e30725b4 	movw	r2, #30132	; 0x75b4
6030a010:	e3462031 	movt	r2, #24625	; 0x6031
6030a014:	e3a03000 	mov	r3, #0
6030a018:	e8841008 	stm	r4, {r3, ip}
6030a01c:	e3a03002 	mov	r3, #2
6030a020:	e5843008 	str	r3, [r4, #8]
6030a024:	ebfffa84 	bl	60308a3c <ipc_send_message>
6030a028:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a02c:	e3130004 	tst	r3, #4
6030a030:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
6030a034:	e59f2690 	ldr	r2, [pc, #1680]	; 6030a6cc <linux_ipc_otp_instruction+0xa38>
6030a038:	e3c4303f 	bic	r3, r4, #63	; 0x3f
6030a03c:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a040:	e2822040 	add	r2, r2, #64	; 0x40
6030a044:	e1530002 	cmp	r3, r2
6030a048:	2a000003 	bcs	6030a05c <linux_ipc_otp_instruction+0x3c8>
6030a04c:	ee073f3e 	mcr	15, 0, r3, cr7, cr14, {1}
6030a050:	e2833040 	add	r3, r3, #64	; 0x40
6030a054:	e1520003 	cmp	r2, r3
6030a058:	8afffffb 	bhi	6030a04c <linux_ipc_otp_instruction+0x3b8>
6030a05c:	f57ff05f 	dmb	sy
6030a060:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
6030a064:	e5940004 	ldr	r0, [r4, #4]
6030a068:	e2842010 	add	r2, r4, #16
6030a06c:	e1a01003 	mov	r1, r3
6030a070:	ebfffd5f 	bl	603095f4 <OTP_LogicalMap_Write>
6030a074:	e307c5c4 	movw	ip, #30148	; 0x75c4
6030a078:	e346c031 	movt	ip, #24625	; 0x6031
6030a07c:	e58c0000 	str	r0, [ip]
6030a080:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a084:	e3130004 	tst	r3, #4
6030a088:	0a00000a 	beq	6030a0b8 <linux_ipc_otp_instruction+0x424>
6030a08c:	e28c2001 	add	r2, ip, #1
6030a090:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a094:	e3cc303f 	bic	r3, ip, #63	; 0x3f
6030a098:	e2822040 	add	r2, r2, #64	; 0x40
6030a09c:	e1530002 	cmp	r3, r2
6030a0a0:	2a000003 	bcs	6030a0b4 <linux_ipc_otp_instruction+0x420>
6030a0a4:	ee073f3a 	mcr	15, 0, r3, cr7, cr10, {1}
6030a0a8:	e2833040 	add	r3, r3, #64	; 0x40
6030a0ac:	e1520003 	cmp	r2, r3
6030a0b0:	8afffffb 	bhi	6030a0a4 <linux_ipc_otp_instruction+0x410>
6030a0b4:	f57ff05f 	dmb	sy
6030a0b8:	e30745b4 	movw	r4, #30132	; 0x75b4
6030a0bc:	e3464031 	movt	r4, #24625	; 0x6031
6030a0c0:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a0c4:	e3130004 	tst	r3, #4
6030a0c8:	0a00000a 	beq	6030a0f8 <linux_ipc_otp_instruction+0x464>
6030a0cc:	e284200f 	add	r2, r4, #15
6030a0d0:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a0d4:	e3c4303f 	bic	r3, r4, #63	; 0x3f
6030a0d8:	e2822040 	add	r2, r2, #64	; 0x40
6030a0dc:	e1530002 	cmp	r3, r2
6030a0e0:	2a000003 	bcs	6030a0f4 <linux_ipc_otp_instruction+0x460>
6030a0e4:	ee073f36 	mcr	15, 0, r3, cr7, cr6, {1}
6030a0e8:	e2833040 	add	r3, r3, #64	; 0x40
6030a0ec:	e1520003 	cmp	r2, r3
6030a0f0:	8afffffb 	bhi	6030a0e4 <linux_ipc_otp_instruction+0x450>
6030a0f4:	f57ff05f 	dmb	sy
6030a0f8:	e3a01006 	mov	r1, #6
6030a0fc:	e3a00011 	mov	r0, #17
6030a100:	e30725b4 	movw	r2, #30132	; 0x75b4
6030a104:	e3462031 	movt	r2, #24625	; 0x6031
6030a108:	e3a03000 	mov	r3, #0
6030a10c:	e8841008 	stm	r4, {r3, ip}
6030a110:	e3a03002 	mov	r3, #2
6030a114:	e5843008 	str	r3, [r4, #8]
6030a118:	ebfffa47 	bl	60308a3c <ipc_send_message>
6030a11c:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a120:	e3130004 	tst	r3, #4
6030a124:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
6030a128:	e59f259c 	ldr	r2, [pc, #1436]	; 6030a6cc <linux_ipc_otp_instruction+0xa38>
6030a12c:	e3c4303f 	bic	r3, r4, #63	; 0x3f
6030a130:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a134:	e2822040 	add	r2, r2, #64	; 0x40
6030a138:	e1530002 	cmp	r3, r2
6030a13c:	2affffc6 	bcs	6030a05c <linux_ipc_otp_instruction+0x3c8>
6030a140:	ee073f3e 	mcr	15, 0, r3, cr7, cr14, {1}
6030a144:	e2833040 	add	r3, r3, #64	; 0x40
6030a148:	e1520003 	cmp	r2, r3
6030a14c:	8afffffb 	bhi	6030a140 <linux_ipc_otp_instruction+0x4ac>
6030a150:	eaffffc1 	b	6030a05c <linux_ipc_otp_instruction+0x3c8>
6030a154:	e594600c 	ldr	r6, [r4, #12]
6030a158:	e3560000 	cmp	r6, #0
6030a15c:	1a000109 	bne	6030a588 <linux_ipc_otp_instruction+0x8f4>
6030a160:	e3530000 	cmp	r3, #0
6030a164:	0a00000a 	beq	6030a194 <linux_ipc_otp_instruction+0x500>
6030a168:	e284700f 	add	r7, r4, #15
6030a16c:	e5940004 	ldr	r0, [r4, #4]
6030a170:	e5f71001 	ldrb	r1, [r7, #1]!
6030a174:	e0860000 	add	r0, r6, r0
6030a178:	ebfffc04 	bl	60309190 <OTP_Write8>
6030a17c:	e5943008 	ldr	r3, [r4, #8]
6030a180:	e1a05006 	mov	r5, r6
6030a184:	e2866001 	add	r6, r6, #1
6030a188:	e1530006 	cmp	r3, r6
6030a18c:	cafffff6 	bgt	6030a16c <linux_ipc_otp_instruction+0x4d8>
6030a190:	e2855002 	add	r5, r5, #2
6030a194:	e307c5c4 	movw	ip, #30148	; 0x75c4
6030a198:	e346c031 	movt	ip, #24625	; 0x6031
6030a19c:	e3a03001 	mov	r3, #1
6030a1a0:	e88c0028 	stm	ip, {r3, r5}
6030a1a4:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a1a8:	e3130004 	tst	r3, #4
6030a1ac:	0a00000a 	beq	6030a1dc <linux_ipc_otp_instruction+0x548>
6030a1b0:	e28c2001 	add	r2, ip, #1
6030a1b4:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a1b8:	e3cc303f 	bic	r3, ip, #63	; 0x3f
6030a1bc:	e2822040 	add	r2, r2, #64	; 0x40
6030a1c0:	e1530002 	cmp	r3, r2
6030a1c4:	2a000003 	bcs	6030a1d8 <linux_ipc_otp_instruction+0x544>
6030a1c8:	ee073f3a 	mcr	15, 0, r3, cr7, cr10, {1}
6030a1cc:	e2833040 	add	r3, r3, #64	; 0x40
6030a1d0:	e1520003 	cmp	r2, r3
6030a1d4:	8afffffb 	bhi	6030a1c8 <linux_ipc_otp_instruction+0x534>
6030a1d8:	f57ff05f 	dmb	sy
6030a1dc:	e30745b4 	movw	r4, #30132	; 0x75b4
6030a1e0:	e3464031 	movt	r4, #24625	; 0x6031
6030a1e4:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a1e8:	e3130004 	tst	r3, #4
6030a1ec:	0a00000a 	beq	6030a21c <linux_ipc_otp_instruction+0x588>
6030a1f0:	e284200f 	add	r2, r4, #15
6030a1f4:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a1f8:	e3c4303f 	bic	r3, r4, #63	; 0x3f
6030a1fc:	e2822040 	add	r2, r2, #64	; 0x40
6030a200:	e1530002 	cmp	r3, r2
6030a204:	2a000003 	bcs	6030a218 <linux_ipc_otp_instruction+0x584>
6030a208:	ee073f36 	mcr	15, 0, r3, cr7, cr6, {1}
6030a20c:	e2833040 	add	r3, r3, #64	; 0x40
6030a210:	e1520003 	cmp	r2, r3
6030a214:	8afffffb 	bhi	6030a208 <linux_ipc_otp_instruction+0x574>
6030a218:	f57ff05f 	dmb	sy
6030a21c:	e3a01006 	mov	r1, #6
6030a220:	e3a00011 	mov	r0, #17
6030a224:	e30725b4 	movw	r2, #30132	; 0x75b4
6030a228:	e3462031 	movt	r2, #24625	; 0x6031
6030a22c:	e3a03000 	mov	r3, #0
6030a230:	e8841008 	stm	r4, {r3, ip}
6030a234:	e3a03002 	mov	r3, #2
6030a238:	e5843008 	str	r3, [r4, #8]
6030a23c:	ebfff9fe 	bl	60308a3c <ipc_send_message>
6030a240:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a244:	e3130004 	tst	r3, #4
6030a248:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
6030a24c:	e59f2478 	ldr	r2, [pc, #1144]	; 6030a6cc <linux_ipc_otp_instruction+0xa38>
6030a250:	e3c4303f 	bic	r3, r4, #63	; 0x3f
6030a254:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a258:	e2822040 	add	r2, r2, #64	; 0x40
6030a25c:	e1530002 	cmp	r3, r2
6030a260:	2affff7d 	bcs	6030a05c <linux_ipc_otp_instruction+0x3c8>
6030a264:	ee073f3e 	mcr	15, 0, r3, cr7, cr14, {1}
6030a268:	e2833040 	add	r3, r3, #64	; 0x40
6030a26c:	e1520003 	cmp	r2, r3
6030a270:	8afffffb 	bhi	6030a264 <linux_ipc_otp_instruction+0x5d0>
6030a274:	eaffff78 	b	6030a05c <linux_ipc_otp_instruction+0x3c8>
6030a278:	e2844010 	add	r4, r4, #16
6030a27c:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a280:	e3130004 	tst	r3, #4
6030a284:	0a00000b 	beq	6030a2b8 <linux_ipc_otp_instruction+0x624>
6030a288:	e2842fff 	add	r2, r4, #1020	; 0x3fc
6030a28c:	e2822003 	add	r2, r2, #3
6030a290:	e3c2103f 	bic	r1, r2, #63	; 0x3f
6030a294:	e2811040 	add	r1, r1, #64	; 0x40
6030a298:	e3c4203f 	bic	r2, r4, #63	; 0x3f
6030a29c:	e1520001 	cmp	r2, r1
6030a2a0:	2a000003 	bcs	6030a2b4 <linux_ipc_otp_instruction+0x620>
6030a2a4:	ee072f36 	mcr	15, 0, r2, cr7, cr6, {1}
6030a2a8:	e2822040 	add	r2, r2, #64	; 0x40
6030a2ac:	e1510002 	cmp	r1, r2
6030a2b0:	8afffffb 	bhi	6030a2a4 <linux_ipc_otp_instruction+0x610>
6030a2b4:	f57ff05f 	dmb	sy
6030a2b8:	e30775c4 	movw	r7, #30148	; 0x75c4
6030a2bc:	e3467031 	movt	r7, #24625	; 0x6031
6030a2c0:	e3a02b01 	mov	r2, #1024	; 0x400
6030a2c4:	e3a01000 	mov	r1, #0
6030a2c8:	e1a00004 	mov	r0, r4
6030a2cc:	ebfffc5b 	bl	60309440 <OTP_LogicalMap_Read>
6030a2d0:	e5870000 	str	r0, [r7]
6030a2d4:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a2d8:	e3130004 	tst	r3, #4
6030a2dc:	0a00000b 	beq	6030a310 <linux_ipc_otp_instruction+0x67c>
6030a2e0:	e2842fff 	add	r2, r4, #1020	; 0x3fc
6030a2e4:	e2822003 	add	r2, r2, #3
6030a2e8:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a2ec:	e3c4303f 	bic	r3, r4, #63	; 0x3f
6030a2f0:	e2822040 	add	r2, r2, #64	; 0x40
6030a2f4:	e1530002 	cmp	r3, r2
6030a2f8:	2a000003 	bcs	6030a30c <linux_ipc_otp_instruction+0x678>
6030a2fc:	ee073f3e 	mcr	15, 0, r3, cr7, cr14, {1}
6030a300:	e2833040 	add	r3, r3, #64	; 0x40
6030a304:	e1520003 	cmp	r2, r3
6030a308:	8afffffb 	bhi	6030a2fc <linux_ipc_otp_instruction+0x668>
6030a30c:	f57ff05f 	dmb	sy
6030a310:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a314:	e3130004 	tst	r3, #4
6030a318:	0a00000a 	beq	6030a348 <linux_ipc_otp_instruction+0x6b4>
6030a31c:	e59f23ac 	ldr	r2, [pc, #940]	; 6030a6d0 <linux_ipc_otp_instruction+0xa3c>
6030a320:	e3c7303f 	bic	r3, r7, #63	; 0x3f
6030a324:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a328:	e2822040 	add	r2, r2, #64	; 0x40
6030a32c:	e1530002 	cmp	r3, r2
6030a330:	2a000003 	bcs	6030a344 <linux_ipc_otp_instruction+0x6b0>
6030a334:	ee073f3a 	mcr	15, 0, r3, cr7, cr10, {1}
6030a338:	e2833040 	add	r3, r3, #64	; 0x40
6030a33c:	e1520003 	cmp	r2, r3
6030a340:	8afffffb 	bhi	6030a334 <linux_ipc_otp_instruction+0x6a0>
6030a344:	f57ff05f 	dmb	sy
6030a348:	e30745b4 	movw	r4, #30132	; 0x75b4
6030a34c:	e3464031 	movt	r4, #24625	; 0x6031
6030a350:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a354:	e3130004 	tst	r3, #4
6030a358:	0a00000a 	beq	6030a388 <linux_ipc_otp_instruction+0x6f4>
6030a35c:	e284200f 	add	r2, r4, #15
6030a360:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a364:	e3c4303f 	bic	r3, r4, #63	; 0x3f
6030a368:	e2822040 	add	r2, r2, #64	; 0x40
6030a36c:	e1530002 	cmp	r3, r2
6030a370:	2a000003 	bcs	6030a384 <linux_ipc_otp_instruction+0x6f0>
6030a374:	ee073f36 	mcr	15, 0, r3, cr7, cr6, {1}
6030a378:	e2833040 	add	r3, r3, #64	; 0x40
6030a37c:	e1520003 	cmp	r2, r3
6030a380:	8afffffb 	bhi	6030a374 <linux_ipc_otp_instruction+0x6e0>
6030a384:	f57ff05f 	dmb	sy
6030a388:	e3a01006 	mov	r1, #6
6030a38c:	e3a00011 	mov	r0, #17
6030a390:	e30725b4 	movw	r2, #30132	; 0x75b4
6030a394:	e3462031 	movt	r2, #24625	; 0x6031
6030a398:	e3a03000 	mov	r3, #0
6030a39c:	e8840088 	stm	r4, {r3, r7}
6030a3a0:	e3a03002 	mov	r3, #2
6030a3a4:	e5843008 	str	r3, [r4, #8]
6030a3a8:	ebfff9a3 	bl	60308a3c <ipc_send_message>
6030a3ac:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a3b0:	e3130004 	tst	r3, #4
6030a3b4:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
6030a3b8:	e59f230c 	ldr	r2, [pc, #780]	; 6030a6cc <linux_ipc_otp_instruction+0xa38>
6030a3bc:	e3c4303f 	bic	r3, r4, #63	; 0x3f
6030a3c0:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a3c4:	e2822040 	add	r2, r2, #64	; 0x40
6030a3c8:	e1530002 	cmp	r3, r2
6030a3cc:	2affff22 	bcs	6030a05c <linux_ipc_otp_instruction+0x3c8>
6030a3d0:	ee073f3e 	mcr	15, 0, r3, cr7, cr14, {1}
6030a3d4:	e2833040 	add	r3, r3, #64	; 0x40
6030a3d8:	e1520003 	cmp	r2, r3
6030a3dc:	8afffffb 	bhi	6030a3d0 <linux_ipc_otp_instruction+0x73c>
6030a3e0:	eaffff1d 	b	6030a05c <linux_ipc_otp_instruction+0x3c8>
6030a3e4:	e2846010 	add	r6, r4, #16
6030a3e8:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a3ec:	e3130004 	tst	r3, #4
6030a3f0:	0a00000b 	beq	6030a424 <linux_ipc_otp_instruction+0x790>
6030a3f4:	e2863fff 	add	r3, r6, #1020	; 0x3fc
6030a3f8:	e2833003 	add	r3, r3, #3
6030a3fc:	e3c3203f 	bic	r2, r3, #63	; 0x3f
6030a400:	e2822040 	add	r2, r2, #64	; 0x40
6030a404:	e3c6303f 	bic	r3, r6, #63	; 0x3f
6030a408:	e1530002 	cmp	r3, r2
6030a40c:	2a000003 	bcs	6030a420 <linux_ipc_otp_instruction+0x78c>
6030a410:	ee073f36 	mcr	15, 0, r3, cr7, cr6, {1}
6030a414:	e2833040 	add	r3, r3, #64	; 0x40
6030a418:	e1520003 	cmp	r2, r3
6030a41c:	8afffffb 	bhi	6030a410 <linux_ipc_otp_instruction+0x77c>
6030a420:	f57ff05f 	dmb	sy
6030a424:	e5943008 	ldr	r3, [r4, #8]
6030a428:	e30775c4 	movw	r7, #30148	; 0x75c4
6030a42c:	e3467031 	movt	r7, #24625	; 0x6031
6030a430:	e3530000 	cmp	r3, #0
6030a434:	c3a05000 	movgt	r5, #0
6030a438:	ca000005 	bgt	6030a454 <linux_ipc_otp_instruction+0x7c0>
6030a43c:	ea00000d 	b	6030a478 <linux_ipc_otp_instruction+0x7e4>
6030a440:	e5943008 	ldr	r3, [r4, #8]
6030a444:	e5875004 	str	r5, [r7, #4]
6030a448:	e1550003 	cmp	r5, r3
6030a44c:	e5870000 	str	r0, [r7]
6030a450:	aa000008 	bge	6030a478 <linux_ipc_otp_instruction+0x7e4>
6030a454:	e5940004 	ldr	r0, [r4, #4]
6030a458:	e0861005 	add	r1, r6, r5
6030a45c:	e0850000 	add	r0, r5, r0
6030a460:	ebfffb44 	bl	60309178 <OTP_Read8>
6030a464:	e2855001 	add	r5, r5, #1
6030a468:	e3500000 	cmp	r0, #0
6030a46c:	0afffff3 	beq	6030a440 <linux_ipc_otp_instruction+0x7ac>
6030a470:	e3e03000 	mvn	r3, #0
6030a474:	e5873000 	str	r3, [r7]
6030a478:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a47c:	e3130004 	tst	r3, #4
6030a480:	0a00000b 	beq	6030a4b4 <linux_ipc_otp_instruction+0x820>
6030a484:	e2863fff 	add	r3, r6, #1020	; 0x3fc
6030a488:	e2833003 	add	r3, r3, #3
6030a48c:	e3c3203f 	bic	r2, r3, #63	; 0x3f
6030a490:	e2822040 	add	r2, r2, #64	; 0x40
6030a494:	e3c6303f 	bic	r3, r6, #63	; 0x3f
6030a498:	e1530002 	cmp	r3, r2
6030a49c:	2a000003 	bcs	6030a4b0 <linux_ipc_otp_instruction+0x81c>
6030a4a0:	ee073f3e 	mcr	15, 0, r3, cr7, cr14, {1}
6030a4a4:	e2833040 	add	r3, r3, #64	; 0x40
6030a4a8:	e1520003 	cmp	r2, r3
6030a4ac:	8afffffb 	bhi	6030a4a0 <linux_ipc_otp_instruction+0x80c>
6030a4b0:	f57ff05f 	dmb	sy
6030a4b4:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a4b8:	e3130004 	tst	r3, #4
6030a4bc:	0a00000a 	beq	6030a4ec <linux_ipc_otp_instruction+0x858>
6030a4c0:	e59f2208 	ldr	r2, [pc, #520]	; 6030a6d0 <linux_ipc_otp_instruction+0xa3c>
6030a4c4:	e3c7303f 	bic	r3, r7, #63	; 0x3f
6030a4c8:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a4cc:	e2822040 	add	r2, r2, #64	; 0x40
6030a4d0:	e1530002 	cmp	r3, r2
6030a4d4:	2a000003 	bcs	6030a4e8 <linux_ipc_otp_instruction+0x854>
6030a4d8:	ee073f3a 	mcr	15, 0, r3, cr7, cr10, {1}
6030a4dc:	e2833040 	add	r3, r3, #64	; 0x40
6030a4e0:	e1520003 	cmp	r2, r3
6030a4e4:	8afffffb 	bhi	6030a4d8 <linux_ipc_otp_instruction+0x844>
6030a4e8:	f57ff05f 	dmb	sy
6030a4ec:	e30745b4 	movw	r4, #30132	; 0x75b4
6030a4f0:	e3464031 	movt	r4, #24625	; 0x6031
6030a4f4:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a4f8:	e3130004 	tst	r3, #4
6030a4fc:	0a00000a 	beq	6030a52c <linux_ipc_otp_instruction+0x898>
6030a500:	e284200f 	add	r2, r4, #15
6030a504:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a508:	e3c4303f 	bic	r3, r4, #63	; 0x3f
6030a50c:	e2822040 	add	r2, r2, #64	; 0x40
6030a510:	e1530002 	cmp	r3, r2
6030a514:	2a000003 	bcs	6030a528 <linux_ipc_otp_instruction+0x894>
6030a518:	ee073f36 	mcr	15, 0, r3, cr7, cr6, {1}
6030a51c:	e2833040 	add	r3, r3, #64	; 0x40
6030a520:	e1520003 	cmp	r2, r3
6030a524:	8afffffb 	bhi	6030a518 <linux_ipc_otp_instruction+0x884>
6030a528:	f57ff05f 	dmb	sy
6030a52c:	e3a01006 	mov	r1, #6
6030a530:	e3a00011 	mov	r0, #17
6030a534:	e30725b4 	movw	r2, #30132	; 0x75b4
6030a538:	e3462031 	movt	r2, #24625	; 0x6031
6030a53c:	e3a03000 	mov	r3, #0
6030a540:	e8840088 	stm	r4, {r3, r7}
6030a544:	e3a03002 	mov	r3, #2
6030a548:	e5843008 	str	r3, [r4, #8]
6030a54c:	ebfff93a 	bl	60308a3c <ipc_send_message>
6030a550:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a554:	e3130004 	tst	r3, #4
6030a558:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
6030a55c:	e59f2168 	ldr	r2, [pc, #360]	; 6030a6cc <linux_ipc_otp_instruction+0xa38>
6030a560:	e3c4303f 	bic	r3, r4, #63	; 0x3f
6030a564:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a568:	e2822040 	add	r2, r2, #64	; 0x40
6030a56c:	e1530002 	cmp	r3, r2
6030a570:	2afffeb9 	bcs	6030a05c <linux_ipc_otp_instruction+0x3c8>
6030a574:	ee073f3e 	mcr	15, 0, r3, cr7, cr14, {1}
6030a578:	e2833040 	add	r3, r3, #64	; 0x40
6030a57c:	e1520003 	cmp	r2, r3
6030a580:	8afffffb 	bhi	6030a574 <linux_ipc_otp_instruction+0x8e0>
6030a584:	eafffeb4 	b	6030a05c <linux_ipc_otp_instruction+0x3c8>
6030a588:	e5941004 	ldr	r1, [r4, #4]
6030a58c:	e3050e38 	movw	r0, #24120	; 0x5e38
6030a590:	e3460031 	movt	r0, #24625	; 0x6031
6030a594:	e0833001 	add	r3, r3, r1
6030a598:	e2432001 	sub	r2, r3, #1
6030a59c:	eb000c1c 	bl	6030d614 <__wrap_printf>
6030a5a0:	e5943008 	ldr	r3, [r4, #8]
6030a5a4:	e3530000 	cmp	r3, #0
6030a5a8:	da00000e 	ble	6030a5e8 <linux_ipc_otp_instruction+0x954>
6030a5ac:	e3058ea8 	movw	r8, #24232	; 0x5ea8
6030a5b0:	e3468031 	movt	r8, #24625	; 0x6031
6030a5b4:	e284700f 	add	r7, r4, #15
6030a5b8:	e3a06000 	mov	r6, #0
6030a5bc:	e5941004 	ldr	r1, [r4, #4]
6030a5c0:	e5f72001 	ldrb	r2, [r7, #1]!
6030a5c4:	e0861001 	add	r1, r6, r1
6030a5c8:	e1a00008 	mov	r0, r8
6030a5cc:	eb000c10 	bl	6030d614 <__wrap_printf>
6030a5d0:	e5943008 	ldr	r3, [r4, #8]
6030a5d4:	e1a05006 	mov	r5, r6
6030a5d8:	e2866001 	add	r6, r6, #1
6030a5dc:	e1530006 	cmp	r3, r6
6030a5e0:	cafffff5 	bgt	6030a5bc <linux_ipc_otp_instruction+0x928>
6030a5e4:	e2855002 	add	r5, r5, #2
6030a5e8:	e307c5c4 	movw	ip, #30148	; 0x75c4
6030a5ec:	e346c031 	movt	ip, #24625	; 0x6031
6030a5f0:	e3a03001 	mov	r3, #1
6030a5f4:	e88c0028 	stm	ip, {r3, r5}
6030a5f8:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a5fc:	e3130004 	tst	r3, #4
6030a600:	0a00000a 	beq	6030a630 <linux_ipc_otp_instruction+0x99c>
6030a604:	e28c2001 	add	r2, ip, #1
6030a608:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a60c:	e3cc303f 	bic	r3, ip, #63	; 0x3f
6030a610:	e2822040 	add	r2, r2, #64	; 0x40
6030a614:	e1530002 	cmp	r3, r2
6030a618:	2a000003 	bcs	6030a62c <linux_ipc_otp_instruction+0x998>
6030a61c:	ee073f3a 	mcr	15, 0, r3, cr7, cr10, {1}
6030a620:	e2833040 	add	r3, r3, #64	; 0x40
6030a624:	e1520003 	cmp	r2, r3
6030a628:	8afffffb 	bhi	6030a61c <linux_ipc_otp_instruction+0x988>
6030a62c:	f57ff05f 	dmb	sy
6030a630:	e30745b4 	movw	r4, #30132	; 0x75b4
6030a634:	e3464031 	movt	r4, #24625	; 0x6031
6030a638:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a63c:	e3130004 	tst	r3, #4
6030a640:	0a00000a 	beq	6030a670 <linux_ipc_otp_instruction+0x9dc>
6030a644:	e284200f 	add	r2, r4, #15
6030a648:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a64c:	e3c4303f 	bic	r3, r4, #63	; 0x3f
6030a650:	e2822040 	add	r2, r2, #64	; 0x40
6030a654:	e1530002 	cmp	r3, r2
6030a658:	2a000003 	bcs	6030a66c <linux_ipc_otp_instruction+0x9d8>
6030a65c:	ee073f36 	mcr	15, 0, r3, cr7, cr6, {1}
6030a660:	e2833040 	add	r3, r3, #64	; 0x40
6030a664:	e1520003 	cmp	r2, r3
6030a668:	8afffffb 	bhi	6030a65c <linux_ipc_otp_instruction+0x9c8>
6030a66c:	f57ff05f 	dmb	sy
6030a670:	e3a01006 	mov	r1, #6
6030a674:	e3a00011 	mov	r0, #17
6030a678:	e30725b4 	movw	r2, #30132	; 0x75b4
6030a67c:	e3462031 	movt	r2, #24625	; 0x6031
6030a680:	e3a03000 	mov	r3, #0
6030a684:	e8841008 	stm	r4, {r3, ip}
6030a688:	e3a03002 	mov	r3, #2
6030a68c:	e5843008 	str	r3, [r4, #8]
6030a690:	ebfff8e9 	bl	60308a3c <ipc_send_message>
6030a694:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030a698:	e3130004 	tst	r3, #4
6030a69c:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
6030a6a0:	e59f2024 	ldr	r2, [pc, #36]	; 6030a6cc <linux_ipc_otp_instruction+0xa38>
6030a6a4:	e3c4303f 	bic	r3, r4, #63	; 0x3f
6030a6a8:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030a6ac:	e2822040 	add	r2, r2, #64	; 0x40
6030a6b0:	e1530002 	cmp	r3, r2
6030a6b4:	2afffe68 	bcs	6030a05c <linux_ipc_otp_instruction+0x3c8>
6030a6b8:	ee073f3e 	mcr	15, 0, r3, cr7, cr14, {1}
6030a6bc:	e2833040 	add	r3, r3, #64	; 0x40
6030a6c0:	e1520003 	cmp	r2, r3
6030a6c4:	8afffffb 	bhi	6030a6b8 <linux_ipc_otp_instruction+0xa24>
6030a6c8:	eafffe63 	b	6030a05c <linux_ipc_otp_instruction+0x3c8>
6030a6cc:	603175c3 	.word	0x603175c3
6030a6d0:	603175c5 	.word	0x603175c5

6030a6d4 <gic_cpu_init>:
6030a6d4:	f57ff05f 	dmb	sy
6030a6d8:	e3a00a01 	mov	r0, #4096	; 0x1000
6030a6dc:	e34a0010 	movt	r0, #40976	; 0xa010
6030a6e0:	e3e0c000 	mvn	ip, #0
6030a6e4:	e580c380 	str	ip, [r0, #896]	; 0x380
6030a6e8:	f57ff05f 	dmb	sy
6030a6ec:	e3a03000 	mov	r3, #0
6030a6f0:	e34f3fff 	movt	r3, #65535	; 0xffff
6030a6f4:	e5803180 	str	r3, [r0, #384]	; 0x180
6030a6f8:	f57ff05f 	dmb	sy
6030a6fc:	e30f3fff 	movw	r3, #65535	; 0xffff
6030a700:	e5803100 	str	r3, [r0, #256]	; 0x100
6030a704:	f57ff05f 	dmb	sy
6030a708:	e3a03b05 	mov	r3, #5120	; 0x1400
6030a70c:	e34a3010 	movt	r3, #40976	; 0xa010
6030a710:	e30a10a0 	movw	r1, #41120	; 0xa0a0
6030a714:	e34a10a0 	movt	r1, #41120	; 0xa0a0
6030a718:	e3012420 	movw	r2, #5152	; 0x1420
6030a71c:	e34a2010 	movt	r2, #40976	; 0xa010
6030a720:	e580c080 	str	ip, [r0, #128]	; 0x80
6030a724:	f57ff05f 	dmb	sy
6030a728:	e4831004 	str	r1, [r3], #4
6030a72c:	e1530002 	cmp	r3, r2
6030a730:	1afffffb 	bne	6030a724 <gic_cpu_init+0x50>
6030a734:	f57ff05f 	dmb	sy
6030a738:	e3a02a02 	mov	r2, #8192	; 0x2000
6030a73c:	e34a2010 	movt	r2, #40976	; 0xa010
6030a740:	e3a030ff 	mov	r3, #255	; 0xff
6030a744:	e5823004 	str	r3, [r2, #4]
6030a748:	e5923000 	ldr	r3, [r2]
6030a74c:	f57ff05f 	dmb	sy
6030a750:	e3c33e1e 	bic	r3, r3, #480	; 0x1e0
6030a754:	e3c33003 	bic	r3, r3, #3
6030a758:	e3833007 	orr	r3, r3, #7
6030a75c:	f57ff05f 	dmb	sy
6030a760:	e5823000 	str	r3, [r2]
6030a764:	e12fff1e 	bx	lr

6030a768 <arm_gic_irq_enable>:
6030a768:	e200201f 	and	r2, r0, #31
6030a76c:	e3a03001 	mov	r3, #1
6030a770:	e1a03213 	lsl	r3, r3, r2
6030a774:	f57ff05f 	dmb	sy
6030a778:	e3a02c11 	mov	r2, #4352	; 0x1100
6030a77c:	e34a2010 	movt	r2, #40976	; 0xa010
6030a780:	e1a002a0 	lsr	r0, r0, #5
6030a784:	e7823100 	str	r3, [r2, r0, lsl #2]
6030a788:	e12fff1e 	bx	lr

6030a78c <arm_gic_irq_disable>:
6030a78c:	e200201f 	and	r2, r0, #31
6030a790:	e3a03001 	mov	r3, #1
6030a794:	e1a03213 	lsl	r3, r3, r2
6030a798:	f57ff05f 	dmb	sy
6030a79c:	e3a02d46 	mov	r2, #4480	; 0x1180
6030a7a0:	e34a2010 	movt	r2, #40976	; 0xa010
6030a7a4:	e1a002a0 	lsr	r0, r0, #5
6030a7a8:	e7823100 	str	r3, [r2, r0, lsl #2]
6030a7ac:	e12fff1e 	bx	lr

6030a7b0 <arm_gic_irq_set_priority>:
6030a7b0:	e6ef3071 	uxtb	r3, r1
6030a7b4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
6030a7b8:	f57ff05f 	dmb	sy
6030a7bc:	e3a0eb05 	mov	lr, #5120	; 0x1400
6030a7c0:	e34ae010 	movt	lr, #40976	; 0xa010
6030a7c4:	e1a0c220 	lsr	ip, r0, #4
6030a7c8:	e200100f 	and	r1, r0, #15
6030a7cc:	e7c0300e 	strb	r3, [r0, lr]
6030a7d0:	e3a03b07 	mov	r3, #7168	; 0x1c00
6030a7d4:	e34a3010 	movt	r3, #40976	; 0xa010
6030a7d8:	e083010c 	add	r0, r3, ip, lsl #2
6030a7dc:	e1a01081 	lsl	r1, r1, #1
6030a7e0:	e793310c 	ldr	r3, [r3, ip, lsl #2]
6030a7e4:	f57ff05f 	dmb	sy
6030a7e8:	e3a0c003 	mov	ip, #3
6030a7ec:	e3120001 	tst	r2, #1
6030a7f0:	13a02002 	movne	r2, #2
6030a7f4:	e1c3311c 	bic	r3, r3, ip, lsl r1
6030a7f8:	11833112 	orrne	r3, r3, r2, lsl r1
6030a7fc:	f57ff05f 	dmb	sy
6030a800:	e5803000 	str	r3, [r0]
6030a804:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

6030a808 <arm_gic_get_active>:
6030a808:	e3a03a02 	mov	r3, #8192	; 0x2000
6030a80c:	e34a3010 	movt	r3, #40976	; 0xa010
6030a810:	e593000c 	ldr	r0, [r3, #12]
6030a814:	f57ff05f 	dmb	sy
6030a818:	e7e90050 	ubfx	r0, r0, #0, #10
6030a81c:	e12fff1e 	bx	lr

6030a820 <arm_gic_clear_pending_irq>:
6030a820:	e200201f 	and	r2, r0, #31
6030a824:	e3a03001 	mov	r3, #1
6030a828:	e1a03213 	lsl	r3, r3, r2
6030a82c:	f57ff05f 	dmb	sy
6030a830:	e3a02d4a 	mov	r2, #4736	; 0x1280
6030a834:	e34a2010 	movt	r2, #40976	; 0xa010
6030a838:	e1a002a0 	lsr	r0, r0, #5
6030a83c:	e7823100 	str	r3, [r2, r0, lsl #2]
6030a840:	e12fff1e 	bx	lr

6030a844 <arm_gic_eoi>:
6030a844:	f57ff05f 	dmb	sy
6030a848:	e3a03a02 	mov	r3, #8192	; 0x2000
6030a84c:	e34a3010 	movt	r3, #40976	; 0xa010
6030a850:	e5830010 	str	r0, [r3, #16]
6030a854:	e12fff1e 	bx	lr

6030a858 <arm_gic_raise_softirq>:
6030a858:	e3500007 	cmp	r0, #7
6030a85c:	9351000f 	cmpls	r1, #15
6030a860:	812fff1e 	bxhi	lr
6030a864:	e2800010 	add	r0, r0, #16
6030a868:	e3a03001 	mov	r3, #1
6030a86c:	e1811013 	orr	r1, r1, r3, lsl r0
6030a870:	f57ff05f 	dmb	sy
6030a874:	e3a03a01 	mov	r3, #4096	; 0x1000
6030a878:	e34a3010 	movt	r3, #40976	; 0xa010
6030a87c:	e5831f00 	str	r1, [r3, #3840]	; 0xf00
6030a880:	e12fff1e 	bx	lr

6030a884 <arm_gic_init_secondary>:
6030a884:	e92d4010 	push	{r4, lr}
6030a888:	ebffff91 	bl	6030a6d4 <gic_cpu_init>
6030a88c:	e3a00000 	mov	r0, #0
6030a890:	e8bd8010 	pop	{r4, pc}

6030a894 <arm_gic_init>:
6030a894:	e3a01a01 	mov	r1, #4096	; 0x1000
6030a898:	e34a1010 	movt	r1, #40976	; 0xa010
6030a89c:	e92d4010 	push	{r4, lr}
6030a8a0:	e5912004 	ldr	r2, [r1, #4]
6030a8a4:	f57ff05f 	dmb	sy
6030a8a8:	e202201f 	and	r2, r2, #31
6030a8ac:	e2822001 	add	r2, r2, #1
6030a8b0:	e1a02282 	lsl	r2, r2, #5
6030a8b4:	e3520080 	cmp	r2, #128	; 0x80
6030a8b8:	31a03002 	movcc	r3, r2
6030a8bc:	23a03080 	movcs	r3, #128	; 0x80
6030a8c0:	f57ff05f 	dmb	sy
6030a8c4:	e3a00000 	mov	r0, #0
6030a8c8:	e3520020 	cmp	r2, #32
6030a8cc:	e5810000 	str	r0, [r1]
6030a8d0:	0a000032 	beq	6030a9a0 <arm_gic_init+0x10c>
6030a8d4:	e3a00b06 	mov	r0, #6144	; 0x1800
6030a8d8:	e34a0010 	movt	r0, #40976	; 0xa010
6030a8dc:	e3001101 	movw	r1, #257	; 0x101
6030a8e0:	e3401101 	movt	r1, #257	; 0x101
6030a8e4:	e3a02020 	mov	r2, #32
6030a8e8:	f57ff05f 	dmb	sy
6030a8ec:	e7821000 	str	r1, [r2, r0]
6030a8f0:	e2822004 	add	r2, r2, #4
6030a8f4:	e1530002 	cmp	r3, r2
6030a8f8:	8afffffa 	bhi	6030a8e8 <arm_gic_init+0x54>
6030a8fc:	e3a01b07 	mov	r1, #7168	; 0x1c00
6030a900:	e34a1010 	movt	r1, #40976	; 0xa010
6030a904:	e3a02020 	mov	r2, #32
6030a908:	e3a00000 	mov	r0, #0
6030a90c:	f57ff05f 	dmb	sy
6030a910:	e7810122 	str	r0, [r1, r2, lsr #2]
6030a914:	e2822010 	add	r2, r2, #16
6030a918:	e1530002 	cmp	r3, r2
6030a91c:	8afffffa 	bhi	6030a90c <arm_gic_init+0x78>
6030a920:	e3a00b05 	mov	r0, #5120	; 0x1400
6030a924:	e34a0010 	movt	r0, #40976	; 0xa010
6030a928:	e30a10a0 	movw	r1, #41120	; 0xa0a0
6030a92c:	e34a10a0 	movt	r1, #41120	; 0xa0a0
6030a930:	e3a02020 	mov	r2, #32
6030a934:	f57ff05f 	dmb	sy
6030a938:	e7821000 	str	r1, [r2, r0]
6030a93c:	e2822004 	add	r2, r2, #4
6030a940:	e1530002 	cmp	r3, r2
6030a944:	8afffffa 	bhi	6030a934 <arm_gic_init+0xa0>
6030a948:	e3a01d42 	mov	r1, #4224	; 0x1080
6030a94c:	e34a1010 	movt	r1, #40976	; 0xa010
6030a950:	e3a02020 	mov	r2, #32
6030a954:	e3e00000 	mvn	r0, #0
6030a958:	f57ff05f 	dmb	sy
6030a95c:	e78101a2 	str	r0, [r1, r2, lsr #3]
6030a960:	e2822020 	add	r2, r2, #32
6030a964:	e1530002 	cmp	r3, r2
6030a968:	8afffffa 	bhi	6030a958 <arm_gic_init+0xc4>
6030a96c:	e3a0cd4e 	mov	ip, #4992	; 0x1380
6030a970:	e34ac010 	movt	ip, #40976	; 0xa010
6030a974:	e3a00d46 	mov	r0, #4480	; 0x1180
6030a978:	e34a0010 	movt	r0, #40976	; 0xa010
6030a97c:	e3a02020 	mov	r2, #32
6030a980:	e3e01000 	mvn	r1, #0
6030a984:	f57ff05f 	dmb	sy
6030a988:	e78c11a2 	str	r1, [ip, r2, lsr #3]
6030a98c:	f57ff05f 	dmb	sy
6030a990:	e78011a2 	str	r1, [r0, r2, lsr #3]
6030a994:	e2822020 	add	r2, r2, #32
6030a998:	e1530002 	cmp	r3, r2
6030a99c:	8afffff8 	bhi	6030a984 <arm_gic_init+0xf0>
6030a9a0:	f57ff05f 	dmb	sy
6030a9a4:	e3a03a01 	mov	r3, #4096	; 0x1000
6030a9a8:	e34a3010 	movt	r3, #40976	; 0xa010
6030a9ac:	e3a02003 	mov	r2, #3
6030a9b0:	e5832000 	str	r2, [r3]
6030a9b4:	ebffff46 	bl	6030a6d4 <gic_cpu_init>
6030a9b8:	e3a00000 	mov	r0, #0
6030a9bc:	e8bd8010 	pop	{r4, pc}

6030a9c0 <_read>:
6030a9c0:	e2520000 	subs	r0, r2, #0
6030a9c4:	da000006 	ble	6030a9e4 <_read+0x24>
6030a9c8:	e2411001 	sub	r1, r1, #1
6030a9cc:	e081c000 	add	ip, r1, r0
6030a9d0:	e3a03000 	mov	r3, #0
6030a9d4:	e5e13001 	strb	r3, [r1, #1]!
6030a9d8:	e151000c 	cmp	r1, ip
6030a9dc:	1afffffc 	bne	6030a9d4 <_read+0x14>
6030a9e0:	e12fff1e 	bx	lr
6030a9e4:	e3a00000 	mov	r0, #0
6030a9e8:	e12fff1e 	bx	lr

6030a9ec <_write>:
6030a9ec:	e92d4070 	push	{r4, r5, r6, lr}
6030a9f0:	e2526000 	subs	r6, r2, #0
6030a9f4:	da000006 	ble	6030aa14 <_write+0x28>
6030a9f8:	e2414001 	sub	r4, r1, #1
6030a9fc:	e0845006 	add	r5, r4, r6
6030aa00:	e5f41001 	ldrb	r1, [r4, #1]!
6030aa04:	e1a00001 	mov	r0, r1
6030aa08:	ebffd63c 	bl	60300300 <__PutCharacter>
6030aa0c:	e1540005 	cmp	r4, r5
6030aa10:	1afffffa 	bne	6030aa00 <_write+0x14>
6030aa14:	e1a00006 	mov	r0, r6
6030aa18:	e8bd8070 	pop	{r4, r5, r6, pc}

6030aa1c <_close>:
6030aa1c:	e3a00000 	mov	r0, #0
6030aa20:	e12fff1e 	bx	lr

6030aa24 <_isatty>:
6030aa24:	e3a00001 	mov	r0, #1
6030aa28:	e12fff1e 	bx	lr

6030aa2c <_lseek>:
6030aa2c:	e3073460 	movw	r3, #29792	; 0x7460
6030aa30:	e3463031 	movt	r3, #24625	; 0x6031
6030aa34:	e3a0201d 	mov	r2, #29
6030aa38:	e3e00000 	mvn	r0, #0
6030aa3c:	e5832000 	str	r2, [r3]
6030aa40:	e12fff1e 	bx	lr

6030aa44 <_fstat>:
6030aa44:	e3a03a02 	mov	r3, #8192	; 0x2000
6030aa48:	e3a00000 	mov	r0, #0
6030aa4c:	e5813004 	str	r3, [r1, #4]
6030aa50:	e12fff1e 	bx	lr

6030aa54 <_getpid>:
6030aa54:	e3a00001 	mov	r0, #1
6030aa58:	e12fff1e 	bx	lr

6030aa5c <_kill>:
6030aa5c:	e3500001 	cmp	r0, #1
6030aa60:	0a000001 	beq	6030aa6c <_kill+0x10>
6030aa64:	e3a00000 	mov	r0, #0
6030aa68:	e12fff1e 	bx	lr
6030aa6c:	e1a00001 	mov	r0, r1
6030aa70:	e92d4010 	push	{r4, lr}
6030aa74:	ebfff37b 	bl	60307868 <_exit>
6030aa78:	e3a00000 	mov	r0, #0
6030aa7c:	e8bd8010 	pop	{r4, pc}

6030aa80 <vGetGenericTimerFreq>:
6030aa80:	e92d4010 	push	{r4, lr}
6030aa84:	e30434c8 	movw	r3, #17608	; 0x44c8
6030aa88:	e3463031 	movt	r3, #24625	; 0x6031
6030aa8c:	e12fff33 	blx	r3
6030aa90:	e3500001 	cmp	r0, #1
6030aa94:	0a000005 	beq	6030aab0 <vGetGenericTimerFreq+0x30>
6030aa98:	e3093bb0 	movw	r3, #39856	; 0x9bb0
6030aa9c:	e3463030 	movt	r3, #24624	; 0x6030
6030aaa0:	e12fff33 	blx	r3
6030aaa4:	e3a01000 	mov	r1, #0
6030aaa8:	e1a000a0 	lsr	r0, r0, #1
6030aaac:	e8bd8010 	pop	{r4, pc}
6030aab0:	e3a01000 	mov	r1, #0
6030aab4:	e30b0c20 	movw	r0, #48160	; 0xbc20
6030aab8:	e34000be 	movt	r0, #190	; 0xbe
6030aabc:	e8bd8010 	pop	{r4, pc}

6030aac0 <prvSetupHardware>:
6030aac0:	e92d4010 	push	{r4, lr}
6030aac4:	eb001e5a 	bl	60312434 <ulPortInterruptLock>
6030aac8:	e30434c8 	movw	r3, #17608	; 0x44c8
6030aacc:	e3463031 	movt	r3, #24625	; 0x6031
6030aad0:	e12fff33 	blx	r3
6030aad4:	e3500001 	cmp	r0, #1
6030aad8:	0a00000b 	beq	6030ab0c <prvSetupHardware+0x4c>
6030aadc:	e3093bb0 	movw	r3, #39856	; 0x9bb0
6030aae0:	e3463030 	movt	r3, #24624	; 0x6030
6030aae4:	e12fff33 	blx	r3
6030aae8:	e3a02000 	mov	r2, #0
6030aaec:	e1a000a0 	lsr	r0, r0, #1
6030aaf0:	e30735d0 	movw	r3, #30160	; 0x75d0
6030aaf4:	e3463031 	movt	r3, #24625	; 0x6031
6030aaf8:	e8830005 	stm	r3, {r0, r2}
6030aafc:	ebffff64 	bl	6030a894 <arm_gic_init>
6030ab00:	eb0001c6 	bl	6030b220 <psci_init>
6030ab04:	e8bd4010 	pop	{r4, lr}
6030ab08:	ea00025d 	b	6030b484 <smp_init>
6030ab0c:	e30b0c20 	movw	r0, #48160	; 0xbc20
6030ab10:	e34000be 	movt	r0, #190	; 0xbe
6030ab14:	e3a02000 	mov	r2, #0
6030ab18:	eafffff4 	b	6030aaf0 <prvSetupHardware+0x30>

6030ab1c <prvSetupHardwareSecondary>:
6030ab1c:	e92d4010 	push	{r4, lr}
6030ab20:	eb001e43 	bl	60312434 <ulPortInterruptLock>
6030ab24:	e8bd4010 	pop	{r4, lr}
6030ab28:	eaffff55 	b	6030a884 <arm_gic_init_secondary>

6030ab2c <vDisableIPIInterrupt>:
6030ab2c:	e30735d8 	movw	r3, #30168	; 0x75d8
6030ab30:	e3463031 	movt	r3, #24625	; 0x6031
6030ab34:	e92d4010 	push	{r4, lr}
6030ab38:	e3a02000 	mov	r2, #0
6030ab3c:	e3a00001 	mov	r0, #1
6030ab40:	e5832008 	str	r2, [r3, #8]
6030ab44:	e583200c 	str	r2, [r3, #12]
6030ab48:	ebffff0f 	bl	6030a78c <arm_gic_irq_disable>
6030ab4c:	e8bd4010 	pop	{r4, lr}
6030ab50:	e3a00001 	mov	r0, #1
6030ab54:	eaffff31 	b	6030a820 <arm_gic_clear_pending_irq>

6030ab58 <vConfigureIPIInterrupt>:
6030ab58:	e92d4070 	push	{r4, r5, r6, lr}
6030ab5c:	e30745d8 	movw	r4, #30168	; 0x75d8
6030ab60:	e3464031 	movt	r4, #24625	; 0x6031
6030ab64:	e3a05000 	mov	r5, #0
6030ab68:	e1a02005 	mov	r2, r5
6030ab6c:	e3a010a0 	mov	r1, #160	; 0xa0
6030ab70:	e1a00005 	mov	r0, r5
6030ab74:	e302347c 	movw	r3, #9340	; 0x247c
6030ab78:	e3463031 	movt	r3, #24625	; 0x6031
6030ab7c:	e5843000 	str	r3, [r4]
6030ab80:	e5845004 	str	r5, [r4, #4]
6030ab84:	ebffff09 	bl	6030a7b0 <arm_gic_irq_set_priority>
6030ab88:	e1a00005 	mov	r0, r5
6030ab8c:	ebfffef5 	bl	6030a768 <arm_gic_irq_enable>
6030ab90:	e1a02005 	mov	r2, r5
6030ab94:	e3a010a0 	mov	r1, #160	; 0xa0
6030ab98:	e3a00001 	mov	r0, #1
6030ab9c:	e30234b4 	movw	r3, #9396	; 0x24b4
6030aba0:	e3463031 	movt	r3, #24625	; 0x6031
6030aba4:	e5843008 	str	r3, [r4, #8]
6030aba8:	e584500c 	str	r5, [r4, #12]
6030abac:	ebfffeff 	bl	6030a7b0 <arm_gic_irq_set_priority>
6030abb0:	e3a00001 	mov	r0, #1
6030abb4:	ebfffeeb 	bl	6030a768 <arm_gic_irq_enable>
6030abb8:	e3a00002 	mov	r0, #2
6030abbc:	e1a02005 	mov	r2, r5
6030abc0:	e3a010a0 	mov	r1, #160	; 0xa0
6030abc4:	e3003508 	movw	r3, #1288	; 0x508
6030abc8:	e3463030 	movt	r3, #24624	; 0x6030
6030abcc:	e5843010 	str	r3, [r4, #16]
6030abd0:	e5845014 	str	r5, [r4, #20]
6030abd4:	ebfffef5 	bl	6030a7b0 <arm_gic_irq_set_priority>
6030abd8:	e8bd4070 	pop	{r4, r5, r6, lr}
6030abdc:	e3a00002 	mov	r0, #2
6030abe0:	eafffee0 	b	6030a768 <arm_gic_irq_enable>

6030abe4 <vConfigureSMPSendIPI>:
6030abe4:	e92d4010 	push	{r4, lr}
6030abe8:	e1a04000 	mov	r4, r0
6030abec:	eb000ba9 	bl	6030da98 <pmu_get_secondary_cpu_state>
6030abf0:	e2501000 	subs	r1, r0, #0
6030abf4:	18bd8010 	popne	{r4, pc}
6030abf8:	e1a00004 	mov	r0, r4
6030abfc:	e8bd4010 	pop	{r4, lr}
6030ac00:	eaffff14 	b	6030a858 <arm_gic_raise_softirq>

6030ac04 <vConfigureTickInterrupt>:
6030ac04:	e30735d8 	movw	r3, #30168	; 0x75d8
6030ac08:	e3463031 	movt	r3, #24625	; 0x6031
6030ac0c:	e92d4370 	push	{r4, r5, r6, r8, r9, lr}
6030ac10:	e30223e8 	movw	r2, #9192	; 0x23e8
6030ac14:	e3462031 	movt	r2, #24625	; 0x6031
6030ac18:	e3a06000 	mov	r6, #0
6030ac1c:	e58320d8 	str	r2, [r3, #216]	; 0xd8
6030ac20:	e58360dc 	str	r6, [r3, #220]	; 0xdc
6030ac24:	ec598f1e 	mrrc	15, 1, r8, r9, cr14
6030ac28:	e30735d0 	movw	r3, #30160	; 0x75d0
6030ac2c:	e3463031 	movt	r3, #24625	; 0x6031
6030ac30:	e3a02ffa 	mov	r2, #1000	; 0x3e8
6030ac34:	e1c300d0 	ldrd	r0, [r3]
6030ac38:	e3a03000 	mov	r3, #0
6030ac3c:	ebffd724 	bl	603008d4 <__aeabi_uldivmod>
6030ac40:	e0902008 	adds	r2, r0, r8
6030ac44:	e0a13009 	adc	r3, r1, r9
6030ac48:	ec432f3e 	mcrr	15, 3, r2, r3, cr14
6030ac4c:	e3a03001 	mov	r3, #1
6030ac50:	ee0e3f33 	mcr	15, 0, r3, cr14, cr3, {1}
6030ac54:	e3a0001b 	mov	r0, #27
6030ac58:	e1a02006 	mov	r2, r6
6030ac5c:	e3a010e0 	mov	r1, #224	; 0xe0
6030ac60:	ebfffed2 	bl	6030a7b0 <arm_gic_irq_set_priority>
6030ac64:	e8bd4370 	pop	{r4, r5, r6, r8, r9, lr}
6030ac68:	e3a0001b 	mov	r0, #27
6030ac6c:	eafffebd 	b	6030a768 <arm_gic_irq_enable>

6030ac70 <vClearTickInterrupt>:
6030ac70:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
6030ac74:	e24dd008 	sub	sp, sp, #8
6030ac78:	ec532f3e 	mrrc	15, 3, r2, r3, cr14
6030ac7c:	e1a00002 	mov	r0, r2
6030ac80:	e1a01003 	mov	r1, r3
6030ac84:	e1a05002 	mov	r5, r2
6030ac88:	e1a0a003 	mov	sl, r3
6030ac8c:	ec532f1e 	mrrc	15, 1, r2, r3, cr14
6030ac90:	e1520000 	cmp	r2, r0
6030ac94:	e0d33001 	sbcs	r3, r3, r1
6030ac98:	e1cd00f0 	strd	r0, [sp]
6030ac9c:	3a000015 	bcc	6030acf8 <vClearTickInterrupt+0x88>
6030aca0:	ec598f1e 	mrrc	15, 1, r8, r9, cr14
6030aca4:	e30735d0 	movw	r3, #30160	; 0x75d0
6030aca8:	e3463031 	movt	r3, #24625	; 0x6031
6030acac:	e3a02ffa 	mov	r2, #1000	; 0x3e8
6030acb0:	e1c300d0 	ldrd	r0, [r3]
6030acb4:	e3a03000 	mov	r3, #0
6030acb8:	ebffd705 	bl	603008d4 <__aeabi_uldivmod>
6030acbc:	e1a04001 	mov	r4, r1
6030acc0:	e1a02000 	mov	r2, r0
6030acc4:	e0580005 	subs	r0, r8, r5
6030acc8:	e0c9100a 	sbc	r1, r9, sl
6030accc:	e1a03004 	mov	r3, r4
6030acd0:	e1a06002 	mov	r6, r2
6030acd4:	ebffd6fe 	bl	603008d4 <__aeabi_uldivmod>
6030acd8:	e0244490 	mla	r4, r0, r4, r4
6030acdc:	e2800001 	add	r0, r0, #1
6030ace0:	e59d1004 	ldr	r1, [sp, #4]
6030ace4:	e0860690 	umull	r0, r6, r0, r6
6030ace8:	e0844006 	add	r4, r4, r6
6030acec:	e0902005 	adds	r2, r0, r5
6030acf0:	e0a43001 	adc	r3, r4, r1
6030acf4:	ec432f3e 	mcrr	15, 3, r2, r3, cr14
6030acf8:	e28dd008 	add	sp, sp, #8
6030acfc:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}

6030ad00 <vRegisterIRQHandler>:
6030ad00:	e350007f 	cmp	r0, #127	; 0x7f
6030ad04:	930735d8 	movwls	r3, #30168	; 0x75d8
6030ad08:	93463031 	movtls	r3, #24625	; 0x6031
6030ad0c:	9083c180 	addls	ip, r3, r0, lsl #3
6030ad10:	97831180 	strls	r1, [r3, r0, lsl #3]
6030ad14:	958c2004 	strls	r2, [ip, #4]
6030ad18:	e12fff1e 	bx	lr

6030ad1c <vApplicationFPUSafeIRQHandler>:
6030ad1c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
6030ad20:	eb001dde 	bl	603124a0 <ulPortGetCoreId>
6030ad24:	e3a05a02 	mov	r5, #8192	; 0x2000
6030ad28:	e34a5010 	movt	r5, #40976	; 0xa010
6030ad2c:	e3088404 	movw	r8, #33796	; 0x8404
6030ad30:	e3468031 	movt	r8, #24625	; 0x6031
6030ad34:	e30765d8 	movw	r6, #30168	; 0x75d8
6030ad38:	e3466031 	movt	r6, #24625	; 0x6031
6030ad3c:	e1a07000 	mov	r7, r0
6030ad40:	ebfffeb0 	bl	6030a808 <arm_gic_get_active>
6030ad44:	e7e94050 	ubfx	r4, r0, #0, #10
6030ad48:	e30033ff 	movw	r3, #1023	; 0x3ff
6030ad4c:	e1540003 	cmp	r4, r3
6030ad50:	08bd87f0 	popeq	{r4, r5, r6, r7, r8, r9, sl, pc}
6030ad54:	e3100d0e 	tst	r0, #896	; 0x380
6030ad58:	18bd87f0 	popne	{r4, r5, r6, r7, r8, r9, sl, pc}
6030ad5c:	e5959004 	ldr	r9, [r5, #4]
6030ad60:	e5953014 	ldr	r3, [r5, #20]
6030ad64:	e5853004 	str	r3, [r5, #4]
6030ad68:	f57ff04f 	dsb	sy
6030ad6c:	f57ff06f 	isb	sy
6030ad70:	e7983107 	ldr	r3, [r8, r7, lsl #2]
6030ad74:	e353000f 	cmp	r3, #15
6030ad78:	8a000002 	bhi	6030ad88 <vApplicationFPUSafeIRQHandler+0x6c>
6030ad7c:	f1080080 	cpsie	i
6030ad80:	f57ff04f 	dsb	sy
6030ad84:	f57ff06f 	isb	sy
6030ad88:	e7963184 	ldr	r3, [r6, r4, lsl #3]
6030ad8c:	e0862184 	add	r2, r6, r4, lsl #3
6030ad90:	e3530000 	cmp	r3, #0
6030ad94:	0a000008 	beq	6030adbc <vApplicationFPUSafeIRQHandler+0xa0>
6030ad98:	e5920004 	ldr	r0, [r2, #4]
6030ad9c:	e12fff33 	blx	r3
6030ada0:	f10c0080 	cpsid	i
6030ada4:	f57ff04f 	dsb	sy
6030ada8:	f57ff06f 	isb	sy
6030adac:	e1a00004 	mov	r0, r4
6030adb0:	ebfffea3 	bl	6030a844 <arm_gic_eoi>
6030adb4:	e5859004 	str	r9, [r5, #4]
6030adb8:	eaffffe0 	b	6030ad40 <vApplicationFPUSafeIRQHandler+0x24>
6030adbc:	e3050eb4 	movw	r0, #24244	; 0x5eb4
6030adc0:	e3460031 	movt	r0, #24625	; 0x6031
6030adc4:	e1a01004 	mov	r1, r4
6030adc8:	eb000a11 	bl	6030d614 <__wrap_printf>
6030adcc:	eafffffe 	b	6030adcc <vApplicationFPUSafeIRQHandler+0xb0>

6030add0 <irq_enable>:
6030add0:	e2800020 	add	r0, r0, #32
6030add4:	eafffe63 	b	6030a768 <arm_gic_irq_enable>

6030add8 <irq_register>:
6030add8:	e92d4070 	push	{r4, r5, r6, lr}
6030addc:	e2814020 	add	r4, r1, #32
6030ade0:	e1a05003 	mov	r5, r3
6030ade4:	e1a01000 	mov	r1, r0
6030ade8:	e1a00004 	mov	r0, r4
6030adec:	ebffffc3 	bl	6030ad00 <vRegisterIRQHandler>
6030adf0:	e1a00004 	mov	r0, r4
6030adf4:	e3a02000 	mov	r2, #0
6030adf8:	e1a01005 	mov	r1, r5
6030adfc:	ebfffe6b 	bl	6030a7b0 <arm_gic_irq_set_priority>
6030ae00:	e3a00001 	mov	r0, #1
6030ae04:	e8bd8070 	pop	{r4, r5, r6, pc}

6030ae08 <_init>:
6030ae08:	e12fff1e 	bx	lr

6030ae0c <app_start>:
6030ae0c:	e30c3f18 	movw	r3, #53016	; 0xcf18
6030ae10:	e3463031 	movt	r3, #24625	; 0x6031
6030ae14:	e92d4010 	push	{r4, lr}
6030ae18:	e3e02000 	mvn	r2, #0
6030ae1c:	e5832000 	str	r2, [r3]
6030ae20:	eb000934 	bl	6030d2f8 <rtos_mem_init>
6030ae24:	ebffd8da 	bl	60301194 <__libc_init_array>
6030ae28:	ebffff24 	bl	6030aac0 <prvSetupHardware>
6030ae2c:	e3a03902 	mov	r3, #32768	; 0x8000
6030ae30:	e3443200 	movt	r3, #16896	; 0x4200
6030ae34:	e3a02074 	mov	r2, #116	; 0x74
6030ae38:	e30c0a40 	movw	r0, #51776	; 0xca40
6030ae3c:	e3460031 	movt	r0, #24625	; 0x6031
6030ae40:	e5931270 	ldr	r1, [r3, #624]	; 0x270
6030ae44:	e30437a0 	movw	r3, #18336	; 0x47a0
6030ae48:	e3463031 	movt	r3, #24625	; 0x6031
6030ae4c:	e12fff33 	blx	r3
6030ae50:	e8bd4010 	pop	{r4, lr}
6030ae54:	ea000b7b 	b	6030dc48 <main>

6030ae58 <FIQHandler>:
6030ae58:	eafffffe 	b	6030ae58 <FIQHandler>

6030ae5c <Crash_GetExStack>:
6030ae5c:	e10fc000 	mrs	ip, CPSR
6030ae60:	e200001f 	and	r0, r0, #31
6030ae64:	e2400010 	sub	r0, r0, #16
6030ae68:	e350000b 	cmp	r0, #11
6030ae6c:	979ff100 	ldrls	pc, [pc, r0, lsl #2]
6030ae70:	ea00000b 	b	6030aea4 <Crash_GetExStack+0x48>
6030ae74:	6030aec8 	.word	0x6030aec8
6030ae78:	6030aea4 	.word	0x6030aea4
6030ae7c:	6030aed0 	.word	0x6030aed0
6030ae80:	6030aed8 	.word	0x6030aed8
6030ae84:	6030aea4 	.word	0x6030aea4
6030ae88:	6030aea4 	.word	0x6030aea4
6030ae8c:	6030aea4 	.word	0x6030aea4
6030ae90:	6030aee0 	.word	0x6030aee0
6030ae94:	6030aea4 	.word	0x6030aea4
6030ae98:	6030aea4 	.word	0x6030aea4
6030ae9c:	6030aea4 	.word	0x6030aea4
6030aea0:	6030aee8 	.word	0x6030aee8
6030aea4:	f102001f 	cps	#31
6030aea8:	e1a0000d 	mov	r0, sp
6030aeac:	e1a0300e 	mov	r3, lr
6030aeb0:	e129f00c 	msr	CPSR_fc, ip
6030aeb4:	f57ff04f 	dsb	sy
6030aeb8:	f57ff06f 	isb	sy
6030aebc:	e5810000 	str	r0, [r1]
6030aec0:	e5823000 	str	r3, [r2]
6030aec4:	e12fff1e 	bx	lr
6030aec8:	f1020010 	cps	#16
6030aecc:	eafffff5 	b	6030aea8 <Crash_GetExStack+0x4c>
6030aed0:	f1020012 	cps	#18
6030aed4:	eafffff3 	b	6030aea8 <Crash_GetExStack+0x4c>
6030aed8:	f1020013 	cps	#19
6030aedc:	eafffff1 	b	6030aea8 <Crash_GetExStack+0x4c>
6030aee0:	f1020017 	cps	#23
6030aee4:	eaffffef 	b	6030aea8 <Crash_GetExStack+0x4c>
6030aee8:	f102001b 	cps	#27
6030aeec:	eaffffed 	b	6030aea8 <Crash_GetExStack+0x4c>

6030aef0 <Crash_DumpReg>:
6030aef0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
6030aef4:	e59f7160 	ldr	r7, [pc, #352]	; 6030b05c <Crash_DumpReg+0x16c>
6030aef8:	e1a06000 	mov	r6, r0
6030aefc:	e24dd01c 	sub	sp, sp, #28
6030af00:	e1a08001 	mov	r8, r1
6030af04:	e305aeec 	movw	sl, #24300	; 0x5eec
6030af08:	e346a031 	movt	sl, #24625	; 0x6031
6030af0c:	eb001d63 	bl	603124a0 <ulPortGetCoreId>
6030af10:	e286901c 	add	r9, r6, #28
6030af14:	e3052ee4 	movw	r2, #24292	; 0x5ee4
6030af18:	e3462031 	movt	r2, #24625	; 0x6031
6030af1c:	e1a05000 	mov	r5, r0
6030af20:	e1a04007 	mov	r4, r7
6030af24:	e287b014 	add	fp, r7, #20
6030af28:	ea000000 	b	6030af30 <Crash_DumpReg+0x40>
6030af2c:	e5b42004 	ldr	r2, [r4, #4]!
6030af30:	e5b93004 	ldr	r3, [r9, #4]!
6030af34:	e1a01005 	mov	r1, r5
6030af38:	e1a0000a 	mov	r0, sl
6030af3c:	eb00227c 	bl	60313934 <DiagPrintf>
6030af40:	e154000b 	cmp	r4, fp
6030af44:	1afffff8 	bne	6030af2c <Crash_DumpReg+0x3c>
6030af48:	e3052ee8 	movw	r2, #24296	; 0x5ee8
6030af4c:	e3462031 	movt	r2, #24625	; 0x6031
6030af50:	e3059eec 	movw	r9, #24300	; 0x5eec
6030af54:	e3469031 	movt	r9, #24625	; 0x6031
6030af58:	e59f4100 	ldr	r4, [pc, #256]	; 6030b060 <Crash_DumpReg+0x170>
6030af5c:	e2466004 	sub	r6, r6, #4
6030af60:	ea000000 	b	6030af68 <Crash_DumpReg+0x78>
6030af64:	e4942004 	ldr	r2, [r4], #4
6030af68:	e5b63004 	ldr	r3, [r6, #4]!
6030af6c:	e1a01005 	mov	r1, r5
6030af70:	e1a00009 	mov	r0, r9
6030af74:	eb00226e 	bl	60313934 <DiagPrintf>
6030af78:	e1540007 	cmp	r4, r7
6030af7c:	1afffff8 	bne	6030af64 <Crash_DumpReg+0x74>
6030af80:	e1a02008 	mov	r2, r8
6030af84:	e1a01005 	mov	r1, r5
6030af88:	e3050f04 	movw	r0, #24324	; 0x5f04
6030af8c:	e3460031 	movt	r0, #24625	; 0x6031
6030af90:	eb002267 	bl	60313934 <DiagPrintf>
6030af94:	e1a00008 	mov	r0, r8
6030af98:	e28d2014 	add	r2, sp, #20
6030af9c:	e28d1010 	add	r1, sp, #16
6030afa0:	ebffffad 	bl	6030ae5c <Crash_GetExStack>
6030afa4:	e59d2014 	ldr	r2, [sp, #20]
6030afa8:	e1a01005 	mov	r1, r5
6030afac:	e3050f30 	movw	r0, #24368	; 0x5f30
6030afb0:	e3460031 	movt	r0, #24625	; 0x6031
6030afb4:	eb00225e 	bl	60313934 <DiagPrintf>
6030afb8:	e59d8010 	ldr	r8, [sp, #16]
6030afbc:	e3050f5c 	movw	r0, #24412	; 0x5f5c
6030afc0:	e3460031 	movt	r0, #24625	; 0x6031
6030afc4:	e3a03080 	mov	r3, #128	; 0x80
6030afc8:	e1a02008 	mov	r2, r8
6030afcc:	e1a01005 	mov	r1, r5
6030afd0:	e3056fa4 	movw	r6, #24484	; 0x5fa4
6030afd4:	e3466031 	movt	r6, #24625	; 0x6031
6030afd8:	e2887c02 	add	r7, r8, #512	; 0x200
6030afdc:	e1a04008 	mov	r4, r8
6030afe0:	eb002253 	bl	60313934 <DiagPrintf>
6030afe4:	e594200c 	ldr	r2, [r4, #12]
6030afe8:	e5943000 	ldr	r3, [r4]
6030afec:	e58d2008 	str	r2, [sp, #8]
6030aff0:	e1a01005 	mov	r1, r5
6030aff4:	e594c008 	ldr	ip, [r4, #8]
6030aff8:	e58dc004 	str	ip, [sp, #4]
6030affc:	e1a02004 	mov	r2, r4
6030b000:	e594c004 	ldr	ip, [r4, #4]
6030b004:	e1a00006 	mov	r0, r6
6030b008:	e2844010 	add	r4, r4, #16
6030b00c:	e58dc000 	str	ip, [sp]
6030b010:	eb002247 	bl	60313934 <DiagPrintf>
6030b014:	e1570004 	cmp	r7, r4
6030b018:	1afffff1 	bne	6030afe4 <Crash_DumpReg+0xf4>
6030b01c:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
6030b020:	e3130004 	tst	r3, #4
6030b024:	0a00000a 	beq	6030b054 <Crash_DumpReg+0x164>
6030b028:	e288307f 	add	r3, r8, #127	; 0x7f
6030b02c:	e3c3303f 	bic	r3, r3, #63	; 0x3f
6030b030:	e3c8803f 	bic	r8, r8, #63	; 0x3f
6030b034:	e2833040 	add	r3, r3, #64	; 0x40
6030b038:	e1580003 	cmp	r8, r3
6030b03c:	2a000003 	bcs	6030b050 <Crash_DumpReg+0x160>
6030b040:	ee078f3a 	mcr	15, 0, r8, cr7, cr10, {1}
6030b044:	e2888040 	add	r8, r8, #64	; 0x40
6030b048:	e1530008 	cmp	r3, r8
6030b04c:	8afffffb 	bhi	6030b040 <Crash_DumpReg+0x150>
6030b050:	f57ff05f 	dmb	sy
6030b054:	e28dd01c 	add	sp, sp, #28
6030b058:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
6030b05c:	60316100 	.word	0x60316100
6030b060:	603160e4 	.word	0x603160e4

6030b064 <UndefinedExceptionHandler>:
6030b064:	e30739e0 	movw	r3, #31200	; 0x79e0
6030b068:	e3463031 	movt	r3, #24625	; 0x6031
6030b06c:	e92d4070 	push	{r4, r5, r6, lr}
6030b070:	e1a05001 	mov	r5, r1
6030b074:	e1a04000 	mov	r4, r0
6030b078:	e5931000 	ldr	r1, [r3]
6030b07c:	e3050fd8 	movw	r0, #24536	; 0x5fd8
6030b080:	e3460031 	movt	r0, #24625	; 0x6031
6030b084:	eb00222a 	bl	60313934 <DiagPrintf>
6030b088:	e1a01005 	mov	r1, r5
6030b08c:	e1a00004 	mov	r0, r4
6030b090:	ebffff96 	bl	6030aef0 <Crash_DumpReg>
6030b094:	eafffffe 	b	6030b094 <UndefinedExceptionHandler+0x30>

6030b098 <DataAbortHandler>:
6030b098:	e92d4070 	push	{r4, r5, r6, lr}
6030b09c:	e1a04000 	mov	r4, r0
6030b0a0:	e1a05001 	mov	r5, r1
6030b0a4:	ee151f10 	mrc	15, 0, r1, cr5, cr0, {0}
6030b0a8:	e3060008 	movw	r0, #24584	; 0x6008
6030b0ac:	e3460031 	movt	r0, #24625	; 0x6031
6030b0b0:	eb00221f 	bl	60313934 <DiagPrintf>
6030b0b4:	e30739d8 	movw	r3, #31192	; 0x79d8
6030b0b8:	e3463031 	movt	r3, #24625	; 0x6031
6030b0bc:	e306003c 	movw	r0, #24636	; 0x603c
6030b0c0:	e3460031 	movt	r0, #24625	; 0x6031
6030b0c4:	e5931000 	ldr	r1, [r3]
6030b0c8:	eb002219 	bl	60313934 <DiagPrintf>
6030b0cc:	e1a01005 	mov	r1, r5
6030b0d0:	e1a00004 	mov	r0, r4
6030b0d4:	ebffff85 	bl	6030aef0 <Crash_DumpReg>
6030b0d8:	eafffffe 	b	6030b0d8 <DataAbortHandler+0x40>

6030b0dc <PrefetchAbortHandler>:
6030b0dc:	e92d4070 	push	{r4, r5, r6, lr}
6030b0e0:	e1a04000 	mov	r4, r0
6030b0e4:	e1a05001 	mov	r5, r1
6030b0e8:	ee151f30 	mrc	15, 0, r1, cr5, cr0, {1}
6030b0ec:	e3060070 	movw	r0, #24688	; 0x6070
6030b0f0:	e3460031 	movt	r0, #24625	; 0x6031
6030b0f4:	eb00220e 	bl	60313934 <DiagPrintf>
6030b0f8:	e30739dc 	movw	r3, #31196	; 0x79dc
6030b0fc:	e3463031 	movt	r3, #24625	; 0x6031
6030b100:	e306003c 	movw	r0, #24636	; 0x603c
6030b104:	e3460031 	movt	r0, #24625	; 0x6031
6030b108:	e5931000 	ldr	r1, [r3]
6030b10c:	eb002208 	bl	60313934 <DiagPrintf>
6030b110:	e1a01005 	mov	r1, r5
6030b114:	e1a00004 	mov	r0, r4
6030b118:	ebffff74 	bl	6030aef0 <Crash_DumpReg>
6030b11c:	eafffffe 	b	6030b11c <PrefetchAbortHandler+0x40>

6030b120 <psci_cpu_suspend>:
6030b120:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
6030b124:	e1a0e000 	mov	lr, r0
6030b128:	e24dd02c 	sub	sp, sp, #44	; 0x2c
6030b12c:	e3a03000 	mov	r3, #0
6030b130:	e1a02001 	mov	r2, r1
6030b134:	e28dc018 	add	ip, sp, #24
6030b138:	e3a00361 	mov	r0, #-2080374783	; 0x84000001
6030b13c:	e1a0100e 	mov	r1, lr
6030b140:	e58d3014 	str	r3, [sp, #20]
6030b144:	e58dc010 	str	ip, [sp, #16]
6030b148:	e58d300c 	str	r3, [sp, #12]
6030b14c:	e58d3008 	str	r3, [sp, #8]
6030b150:	e58d3004 	str	r3, [sp, #4]
6030b154:	e58d3000 	str	r3, [sp]
6030b158:	ebffd4fd 	bl	60300554 <__arm_smccc_smc>
6030b15c:	e59d0018 	ldr	r0, [sp, #24]
6030b160:	e28dd02c 	add	sp, sp, #44	; 0x2c
6030b164:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

6030b168 <psci_cpu_off>:
6030b168:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
6030b16c:	e3a03000 	mov	r3, #0
6030b170:	e24dd02c 	sub	sp, sp, #44	; 0x2c
6030b174:	e1a02003 	mov	r2, r3
6030b178:	e1a01000 	mov	r1, r0
6030b17c:	e28dc018 	add	ip, sp, #24
6030b180:	e3a003a1 	mov	r0, #-2080374782	; 0x84000002
6030b184:	e58d3014 	str	r3, [sp, #20]
6030b188:	e58d300c 	str	r3, [sp, #12]
6030b18c:	e58d3008 	str	r3, [sp, #8]
6030b190:	e58d3004 	str	r3, [sp, #4]
6030b194:	e58d3000 	str	r3, [sp]
6030b198:	e58dc010 	str	ip, [sp, #16]
6030b19c:	ebffd4ec 	bl	60300554 <__arm_smccc_smc>
6030b1a0:	e59d3018 	ldr	r3, [sp, #24]
6030b1a4:	e2833009 	add	r3, r3, #9
6030b1a8:	e3530009 	cmp	r3, #9
6030b1ac:	9306221c 	movwls	r2, #25116	; 0x621c
6030b1b0:	93462031 	movtls	r2, #24625	; 0x6031
6030b1b4:	83e00015 	mvnhi	r0, #21
6030b1b8:	97920103 	ldrls	r0, [r2, r3, lsl #2]
6030b1bc:	e28dd02c 	add	sp, sp, #44	; 0x2c
6030b1c0:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

6030b1c4 <psci_cpu_on>:
6030b1c4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
6030b1c8:	e3a03000 	mov	r3, #0
6030b1cc:	e24dd02c 	sub	sp, sp, #44	; 0x2c
6030b1d0:	e1a02001 	mov	r2, r1
6030b1d4:	e28dc018 	add	ip, sp, #24
6030b1d8:	e1a01000 	mov	r1, r0
6030b1dc:	e58d3014 	str	r3, [sp, #20]
6030b1e0:	e3a003e1 	mov	r0, #-2080374781	; 0x84000003
6030b1e4:	e58d300c 	str	r3, [sp, #12]
6030b1e8:	e58d3008 	str	r3, [sp, #8]
6030b1ec:	e58d3004 	str	r3, [sp, #4]
6030b1f0:	e58d3000 	str	r3, [sp]
6030b1f4:	e58dc010 	str	ip, [sp, #16]
6030b1f8:	ebffd4d5 	bl	60300554 <__arm_smccc_smc>
6030b1fc:	e59d3018 	ldr	r3, [sp, #24]
6030b200:	e2833009 	add	r3, r3, #9
6030b204:	e3530009 	cmp	r3, #9
6030b208:	9306221c 	movwls	r2, #25116	; 0x621c
6030b20c:	93462031 	movtls	r2, #24625	; 0x6031
6030b210:	83e00015 	mvnhi	r0, #21
6030b214:	97920103 	ldrls	r0, [r2, r3, lsl #2]
6030b218:	e28dd02c 	add	sp, sp, #44	; 0x2c
6030b21c:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

6030b220 <psci_init>:
6030b220:	e92d4030 	push	{r4, r5, lr}
6030b224:	e3a04000 	mov	r4, #0
6030b228:	e24dd02c 	sub	sp, sp, #44	; 0x2c
6030b22c:	e1a03004 	mov	r3, r4
6030b230:	e1a02004 	mov	r2, r4
6030b234:	e28d5018 	add	r5, sp, #24
6030b238:	e1a01004 	mov	r1, r4
6030b23c:	e3a00321 	mov	r0, #-2080374784	; 0x84000000
6030b240:	e58d4014 	str	r4, [sp, #20]
6030b244:	e1cd40fc 	strd	r4, [sp, #12]
6030b248:	e58d4008 	str	r4, [sp, #8]
6030b24c:	e58d4004 	str	r4, [sp, #4]
6030b250:	e58d4000 	str	r4, [sp]
6030b254:	ebffd4be 	bl	60300554 <__arm_smccc_smc>
6030b258:	e59d2018 	ldr	r2, [sp, #24]
6030b25c:	e30f3ffe 	movw	r3, #65534	; 0xfffe
6030b260:	e0033002 	and	r3, r3, r2
6030b264:	e1933822 	orrs	r3, r3, r2, lsr #16
6030b268:	0a00003d 	beq	6030b364 <psci_init+0x144>
6030b26c:	e1a01004 	mov	r1, r4
6030b270:	e1a03004 	mov	r3, r4
6030b274:	e1a02004 	mov	r2, r4
6030b278:	e3a00006 	mov	r0, #6
6030b27c:	e3480400 	movt	r0, #33792	; 0x8400
6030b280:	e58d4014 	str	r4, [sp, #20]
6030b284:	e1cd40fc 	strd	r4, [sp, #12]
6030b288:	e58d4008 	str	r4, [sp, #8]
6030b28c:	e58d4004 	str	r4, [sp, #4]
6030b290:	e58d4000 	str	r4, [sp]
6030b294:	ebffd4ae 	bl	60300554 <__arm_smccc_smc>
6030b298:	e59d1018 	ldr	r1, [sp, #24]
6030b29c:	e3510002 	cmp	r1, #2
6030b2a0:	0a000023 	beq	6030b334 <psci_init+0x114>
6030b2a4:	e3710001 	cmn	r1, #1
6030b2a8:	0a000027 	beq	6030b34c <psci_init+0x12c>
6030b2ac:	e3510001 	cmp	r1, #1
6030b2b0:	8a000013 	bhi	6030b304 <psci_init+0xe4>
6030b2b4:	e1a01004 	mov	r1, r4
6030b2b8:	e1cd40fc 	strd	r4, [sp, #12]
6030b2bc:	e1a03004 	mov	r3, r4
6030b2c0:	e1a02004 	mov	r2, r4
6030b2c4:	e3a00007 	mov	r0, #7
6030b2c8:	e3480400 	movt	r0, #33792	; 0x8400
6030b2cc:	e58d4014 	str	r4, [sp, #20]
6030b2d0:	e58d4008 	str	r4, [sp, #8]
6030b2d4:	e58d4004 	str	r4, [sp, #4]
6030b2d8:	e58d4000 	str	r4, [sp]
6030b2dc:	ebffd49c 	bl	60300554 <__arm_smccc_smc>
6030b2e0:	e59d1018 	ldr	r1, [sp, #24]
6030b2e4:	e21154ff 	ands	r5, r1, #-16777216	; 0xff000000
6030b2e8:	1a00000b 	bne	6030b31c <psci_init+0xfc>
6030b2ec:	e30601f0 	movw	r0, #25072	; 0x61f0
6030b2f0:	e3460031 	movt	r0, #24625	; 0x6031
6030b2f4:	eb0008c6 	bl	6030d614 <__wrap_printf>
6030b2f8:	e1a00005 	mov	r0, r5
6030b2fc:	e28dd02c 	add	sp, sp, #44	; 0x2c
6030b300:	e8bd8030 	pop	{r4, r5, pc}
6030b304:	e3060184 	movw	r0, #24964	; 0x6184
6030b308:	e3460031 	movt	r0, #24625	; 0x6031
6030b30c:	eb0008c0 	bl	6030d614 <__wrap_printf>
6030b310:	e1a00004 	mov	r0, r4
6030b314:	e28dd02c 	add	sp, sp, #44	; 0x2c
6030b318:	e8bd8030 	pop	{r4, r5, pc}
6030b31c:	e30601b4 	movw	r0, #25012	; 0x61b4
6030b320:	e3460031 	movt	r0, #24625	; 0x6031
6030b324:	eb0008ba 	bl	6030d614 <__wrap_printf>
6030b328:	e1a00004 	mov	r0, r4
6030b32c:	e28dd02c 	add	sp, sp, #44	; 0x2c
6030b330:	e8bd8030 	pop	{r4, r5, pc}
6030b334:	e306013c 	movw	r0, #24892	; 0x613c
6030b338:	e3460031 	movt	r0, #24625	; 0x6031
6030b33c:	eb0008b4 	bl	6030d614 <__wrap_printf>
6030b340:	e1a00004 	mov	r0, r4
6030b344:	e28dd02c 	add	sp, sp, #44	; 0x2c
6030b348:	e8bd8030 	pop	{r4, r5, pc}
6030b34c:	e3060160 	movw	r0, #24928	; 0x6160
6030b350:	e3460031 	movt	r0, #24625	; 0x6031
6030b354:	eb0008ae 	bl	6030d614 <__wrap_printf>
6030b358:	e1a00004 	mov	r0, r4
6030b35c:	e28dd02c 	add	sp, sp, #44	; 0x2c
6030b360:	e8bd8030 	pop	{r4, r5, pc}
6030b364:	e3060118 	movw	r0, #24856	; 0x6118
6030b368:	e3460031 	movt	r0, #24625	; 0x6031
6030b36c:	eb0008a8 	bl	6030d614 <__wrap_printf>
6030b370:	e3e00015 	mvn	r0, #21
6030b374:	eaffffe0 	b	6030b2fc <psci_init+0xdc>

6030b378 <rtk_core1_power_on>:
6030b378:	e92d4070 	push	{r4, r5, r6, lr}
6030b37c:	e3a06c02 	mov	r6, #512	; 0x200
6030b380:	e3446100 	movt	r6, #16640	; 0x4100
6030b384:	e3a04441 	mov	r4, #1090519040	; 0x41000000
6030b388:	e3a00032 	mov	r0, #50	; 0x32
6030b38c:	e5963004 	ldr	r3, [r6, #4]
6030b390:	e30c5b2c 	movw	r5, #52012	; 0xcb2c
6030b394:	e3465030 	movt	r5, #24624	; 0x6030
6030b398:	e3c33022 	bic	r3, r3, #34	; 0x22
6030b39c:	e5863004 	str	r3, [r6, #4]
6030b3a0:	e5943004 	ldr	r3, [r4, #4]
6030b3a4:	e3833020 	orr	r3, r3, #32
6030b3a8:	e5843004 	str	r3, [r4, #4]
6030b3ac:	e12fff35 	blx	r5
6030b3b0:	e5943000 	ldr	r3, [r4]
6030b3b4:	e3a00032 	mov	r0, #50	; 0x32
6030b3b8:	e3833030 	orr	r3, r3, #48	; 0x30
6030b3bc:	e5843000 	str	r3, [r4]
6030b3c0:	e12fff35 	blx	r5
6030b3c4:	e5943000 	ldr	r3, [r4]
6030b3c8:	e3a00f7d 	mov	r0, #500	; 0x1f4
6030b3cc:	e38330c0 	orr	r3, r3, #192	; 0xc0
6030b3d0:	e5843000 	str	r3, [r4]
6030b3d4:	e12fff35 	blx	r5
6030b3d8:	e5943004 	ldr	r3, [r4, #4]
6030b3dc:	e3a00032 	mov	r0, #50	; 0x32
6030b3e0:	e3c33030 	bic	r3, r3, #48	; 0x30
6030b3e4:	e5843004 	str	r3, [r4, #4]
6030b3e8:	e12fff35 	blx	r5
6030b3ec:	e5962004 	ldr	r2, [r6, #4]
6030b3f0:	e3013122 	movw	r3, #4386	; 0x1122
6030b3f4:	e3403100 	movt	r3, #256	; 0x100
6030b3f8:	e1823003 	orr	r3, r2, r3
6030b3fc:	e5863004 	str	r3, [r6, #4]
6030b400:	e8bd8070 	pop	{r4, r5, r6, pc}

6030b404 <vPortSecondaryStart>:
6030b404:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
6030b408:	e24dd00c 	sub	sp, sp, #12
6030b40c:	eb001c23 	bl	603124a0 <ulPortGetCoreId>
6030b410:	eb0009a0 	bl	6030da98 <pmu_get_secondary_cpu_state>
6030b414:	e3500000 	cmp	r0, #0
6030b418:	0a000012 	beq	6030b468 <vPortSecondaryStart+0x64>
6030b41c:	eb001c1f 	bl	603124a0 <ulPortGetCoreId>
6030b420:	e3a02049 	mov	r2, #73	; 0x49
6030b424:	e1a03000 	mov	r3, r0
6030b428:	e3051394 	movw	r1, #21396	; 0x5394
6030b42c:	e3461031 	movt	r1, #24625	; 0x6031
6030b430:	e3a00004 	mov	r0, #4
6030b434:	e58d3000 	str	r3, [sp]
6030b438:	e3063278 	movw	r3, #25208	; 0x6278
6030b43c:	e3463031 	movt	r3, #24625	; 0x6031
6030b440:	eb00062c 	bl	6030ccf8 <rtk_log_write_nano>
6030b444:	ebfffdb4 	bl	6030ab1c <prvSetupHardwareSecondary>
6030b448:	ebfffdc2 	bl	6030ab58 <vConfigureIPIInterrupt>
6030b44c:	ebfffdec 	bl	6030ac04 <vConfigureTickInterrupt>
6030b450:	e3a01000 	mov	r1, #0
6030b454:	e3a00001 	mov	r0, #1
6030b458:	eb00098a 	bl	6030da88 <pmu_set_secondary_cpu_state>
6030b45c:	e28dd00c 	add	sp, sp, #12
6030b460:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
6030b464:	eaffd468 	b	6030060c <vPortRestoreTaskContext>
6030b468:	eb00081d 	bl	6030d4e4 <rtos_sched_get_state>
6030b46c:	e3500001 	cmp	r0, #1
6030b470:	1affffe9 	bne	6030b41c <vPortSecondaryStart+0x18>
6030b474:	eb00081a 	bl	6030d4e4 <rtos_sched_get_state>
6030b478:	e3500001 	cmp	r0, #1
6030b47c:	0afffff9 	beq	6030b468 <vPortSecondaryStart+0x64>
6030b480:	eaffffe5 	b	6030b41c <vPortSecondaryStart+0x18>

6030b484 <smp_init>:
6030b484:	e92d40f0 	push	{r4, r5, r6, r7, lr}
6030b488:	e3a02049 	mov	r2, #73	; 0x49
6030b48c:	e24dd00c 	sub	sp, sp, #12
6030b490:	e3a00004 	mov	r0, #4
6030b494:	e3063284 	movw	r3, #25220	; 0x6284
6030b498:	e3463031 	movt	r3, #24625	; 0x6031
6030b49c:	e3051394 	movw	r1, #21396	; 0x5394
6030b4a0:	e3461031 	movt	r1, #24625	; 0x6031
6030b4a4:	eb000613 	bl	6030ccf8 <rtk_log_write_nano>
6030b4a8:	e30434c8 	movw	r3, #17608	; 0x44c8
6030b4ac:	e3463031 	movt	r3, #24625	; 0x6031
6030b4b0:	e12fff33 	blx	r3
6030b4b4:	e3500003 	cmp	r0, #3
6030b4b8:	0a000004 	beq	6030b4d0 <smp_init+0x4c>
6030b4bc:	ebffffad 	bl	6030b378 <rtk_core1_power_on>
6030b4c0:	e30c3b2c 	movw	r3, #52012	; 0xcb2c
6030b4c4:	e3463030 	movt	r3, #24624	; 0x6030
6030b4c8:	e3a00032 	mov	r0, #50	; 0x32
6030b4cc:	e12fff33 	blx	r3
6030b4d0:	e30050c8 	movw	r5, #200	; 0xc8
6030b4d4:	e3465030 	movt	r5, #24624	; 0x6030
6030b4d8:	e30672ac 	movw	r7, #25260	; 0x62ac
6030b4dc:	e3467031 	movt	r7, #24625	; 0x6031
6030b4e0:	e3056394 	movw	r6, #21396	; 0x5394
6030b4e4:	e3466031 	movt	r6, #24625	; 0x6031
6030b4e8:	e3a04000 	mov	r4, #0
6030b4ec:	eb001beb 	bl	603124a0 <ulPortGetCoreId>
6030b4f0:	e1a01005 	mov	r1, r5
6030b4f4:	e1500004 	cmp	r0, r4
6030b4f8:	0a000010 	beq	6030b540 <smp_init+0xbc>
6030b4fc:	e1a00004 	mov	r0, r4
6030b500:	ebffff2f 	bl	6030b1c4 <psci_cpu_on>
6030b504:	e3500000 	cmp	r0, #0
6030b508:	ba000004 	blt	6030b520 <smp_init+0x9c>
6030b50c:	e3540001 	cmp	r4, #1
6030b510:	e3a04001 	mov	r4, #1
6030b514:	1afffff4 	bne	6030b4ec <smp_init+0x68>
6030b518:	e28dd00c 	add	sp, sp, #12
6030b51c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
6030b520:	e58d0004 	str	r0, [sp, #4]
6030b524:	e1a03007 	mov	r3, r7
6030b528:	e3a02045 	mov	r2, #69	; 0x45
6030b52c:	e1a01006 	mov	r1, r6
6030b530:	e3a00002 	mov	r0, #2
6030b534:	e58d4000 	str	r4, [sp]
6030b538:	eb0005ee 	bl	6030ccf8 <rtk_log_write_nano>
6030b53c:	eafffff2 	b	6030b50c <smp_init+0x88>
6030b540:	e3a01000 	mov	r1, #0
6030b544:	eb00094f 	bl	6030da88 <pmu_set_secondary_cpu_state>
6030b548:	eaffffef 	b	6030b50c <smp_init+0x88>

6030b54c <SOCPS_SleepPG>:
6030b54c:	e92d4010 	push	{r4, lr}
6030b550:	eb00084e 	bl	6030d690 <pmu_exec_sleep_hook_funs>
6030b554:	e3500009 	cmp	r0, #9
6030b558:	0a000001 	beq	6030b564 <SOCPS_SleepPG+0x18>
6030b55c:	e8bd4010 	pop	{r4, lr}
6030b560:	ea00085c 	b	6030d6d8 <pmu_exec_wakeup_hook_funs>
6030b564:	e1a04000 	mov	r4, r0
6030b568:	e3a02049 	mov	r2, #73	; 0x49
6030b56c:	e3a00004 	mov	r0, #4
6030b570:	e30632c8 	movw	r3, #25288	; 0x62c8
6030b574:	e3463031 	movt	r3, #24625	; 0x6031
6030b578:	e3051394 	movw	r1, #21396	; 0x5394
6030b57c:	e3461031 	movt	r1, #24625	; 0x6031
6030b580:	eb0005dc 	bl	6030ccf8 <rtk_log_write_nano>
6030b584:	eb002235 	bl	60313e60 <SOCPS_SleepPG_LIB>
6030b588:	e3a02049 	mov	r2, #73	; 0x49
6030b58c:	e3a00004 	mov	r0, #4
6030b590:	e30632d0 	movw	r3, #25296	; 0x62d0
6030b594:	e3463031 	movt	r3, #24625	; 0x6031
6030b598:	e3051394 	movw	r1, #21396	; 0x5394
6030b59c:	e3461031 	movt	r1, #24625	; 0x6031
6030b5a0:	eb0005d4 	bl	6030ccf8 <rtk_log_write_nano>
6030b5a4:	e1a00004 	mov	r0, r4
6030b5a8:	eb00084a 	bl	6030d6d8 <pmu_exec_wakeup_hook_funs>
6030b5ac:	ebffffb4 	bl	6030b484 <smp_init>
6030b5b0:	e8bd4010 	pop	{r4, lr}
6030b5b4:	e3a01002 	mov	r1, #2
6030b5b8:	e3a00001 	mov	r0, #1
6030b5bc:	ea000931 	b	6030da88 <pmu_set_secondary_cpu_state>

6030b5c0 <SOCPS_SleepCG>:
6030b5c0:	e92d4010 	push	{r4, lr}
6030b5c4:	eb000831 	bl	6030d690 <pmu_exec_sleep_hook_funs>
6030b5c8:	e3500009 	cmp	r0, #9
6030b5cc:	1a000010 	bne	6030b614 <SOCPS_SleepCG+0x54>
6030b5d0:	e3a02049 	mov	r2, #73	; 0x49
6030b5d4:	e1a04000 	mov	r4, r0
6030b5d8:	e30632d8 	movw	r3, #25304	; 0x62d8
6030b5dc:	e3463031 	movt	r3, #24625	; 0x6031
6030b5e0:	e3a00004 	mov	r0, #4
6030b5e4:	e3051394 	movw	r1, #21396	; 0x5394
6030b5e8:	e3461031 	movt	r1, #24625	; 0x6031
6030b5ec:	eb0005c1 	bl	6030ccf8 <rtk_log_write_nano>
6030b5f0:	eb002227 	bl	60313e94 <SOCPS_SleepCG_LIB>
6030b5f4:	e3a00004 	mov	r0, #4
6030b5f8:	e3a02049 	mov	r2, #73	; 0x49
6030b5fc:	e30632e0 	movw	r3, #25312	; 0x62e0
6030b600:	e3463031 	movt	r3, #24625	; 0x6031
6030b604:	e3051394 	movw	r1, #21396	; 0x5394
6030b608:	e3461031 	movt	r1, #24625	; 0x6031
6030b60c:	eb0005b9 	bl	6030ccf8 <rtk_log_write_nano>
6030b610:	e1a00004 	mov	r0, r4
6030b614:	e8bd4010 	pop	{r4, lr}
6030b618:	ea00082e 	b	6030d6d8 <pmu_exec_wakeup_hook_funs>

6030b61c <setupMMUTable>:
6030b61c:	e3500001 	cmp	r0, #1
6030b620:	0a000001 	beq	6030b62c <setupMMUTable+0x10>
6030b624:	e3a00000 	mov	r0, #0
6030b628:	ea00008d 	b	6030b864 <enable_mmu_svc_mon>
6030b62c:	e92d4010 	push	{r4, lr}
6030b630:	e30c2f60 	movw	r2, #53088	; 0xcf60
6030b634:	e3462031 	movt	r2, #24625	; 0x6031
6030b638:	e24dd008 	sub	sp, sp, #8
6030b63c:	e3070300 	movw	r0, #29440	; 0x7300
6030b640:	e3460031 	movt	r0, #24625	; 0x6031
6030b644:	e0422000 	sub	r2, r2, r0
6030b648:	e3a01000 	mov	r1, #0
6030b64c:	eb00249c 	bl	603148c4 <_memset>
6030b650:	e3a03302 	mov	r3, #134217728	; 0x8000000
6030b654:	e1a02003 	mov	r2, r3
6030b658:	e3a04012 	mov	r4, #18
6030b65c:	e3a00302 	mov	r0, #134217728	; 0x8000000
6030b660:	e3a01000 	mov	r1, #0
6030b664:	e58d4000 	str	r4, [sp]
6030b668:	eb000041 	bl	6030b774 <mmap_add_region>
6030b66c:	e3a03202 	mov	r3, #536870912	; 0x20000000
6030b670:	e1a02003 	mov	r2, r3
6030b674:	e3a0c01a 	mov	ip, #26
6030b678:	e3a00202 	mov	r0, #536870912	; 0x20000000
6030b67c:	e3a01000 	mov	r1, #0
6030b680:	e58dc000 	str	ip, [sp]
6030b684:	eb00003a 	bl	6030b774 <mmap_add_region>
6030b688:	e3a0c018 	mov	ip, #24
6030b68c:	e3a00101 	mov	r0, #1073741824	; 0x40000000
6030b690:	e58dc000 	str	ip, [sp]
6030b694:	e3a01000 	mov	r1, #0
6030b698:	e3a03202 	mov	r3, #536870912	; 0x20000000
6030b69c:	e3a02101 	mov	r2, #1073741824	; 0x40000000
6030b6a0:	eb000033 	bl	6030b774 <mmap_add_region>
6030b6a4:	e3002000 	movw	r2, #0
6030b6a8:	e3462030 	movt	r2, #24624	; 0x6030
6030b6ac:	e3073000 	movw	r3, #28672	; 0x7000
6030b6b0:	e3463031 	movt	r3, #24625	; 0x6031
6030b6b4:	e0433002 	sub	r3, r3, r2
6030b6b8:	e1a00002 	mov	r0, r2
6030b6bc:	e1a01fc2 	asr	r1, r2, #31
6030b6c0:	e58d4000 	str	r4, [sp]
6030b6c4:	eb00002a 	bl	6030b774 <mmap_add_region>
6030b6c8:	e30e2000 	movw	r2, #57344	; 0xe000
6030b6cc:	e3462032 	movt	r2, #24626	; 0x6032
6030b6d0:	e30f3000 	movw	r3, #61440	; 0xf000
6030b6d4:	e3463032 	movt	r3, #24626	; 0x6032
6030b6d8:	e3a01019 	mov	r1, #25
6030b6dc:	e0433002 	sub	r3, r3, r2
6030b6e0:	e58d1000 	str	r1, [sp]
6030b6e4:	e1a00002 	mov	r0, r2
6030b6e8:	e1a01fc2 	asr	r1, r2, #31
6030b6ec:	eb000020 	bl	6030b774 <mmap_add_region>
6030b6f0:	e3003000 	movw	r3, #0
6030b6f4:	e3403000 	movt	r3, #0
6030b6f8:	e3530000 	cmp	r3, #0
6030b6fc:	0a000008 	beq	6030b724 <setupMMUTable+0x108>
6030b700:	e30f3fff 	movw	r3, #65535	; 0xffff
6030b704:	e3463fff 	movt	r3, #28671	; 0x6fff
6030b708:	e2432020 	sub	r2, r3, #32
6030b70c:	e59f105c 	ldr	r1, [pc, #92]	; 6030b770 <setupMMUTable+0x154>
6030b710:	e1a00002 	mov	r0, r2
6030b714:	e0413003 	sub	r3, r1, r3
6030b718:	e58d4000 	str	r4, [sp]
6030b71c:	e1a01fc2 	asr	r1, r2, #31
6030b720:	eb000013 	bl	6030b774 <mmap_add_region>
6030b724:	e3a0c01a 	mov	ip, #26
6030b728:	e3a03202 	mov	r3, #536870912	; 0x20000000
6030b72c:	e58dc000 	str	ip, [sp]
6030b730:	e3a02206 	mov	r2, #1610612736	; 0x60000000
6030b734:	e3a00206 	mov	r0, #1610612736	; 0x60000000
6030b738:	e3a01000 	mov	r1, #0
6030b73c:	eb00000c 	bl	6030b774 <mmap_add_region>
6030b740:	e3a03102 	mov	r3, #-2147483648	; 0x80000000
6030b744:	e3a0c018 	mov	ip, #24
6030b748:	e1a02003 	mov	r2, r3
6030b74c:	e3a00102 	mov	r0, #-2147483648	; 0x80000000
6030b750:	e3a01000 	mov	r1, #0
6030b754:	e58dc000 	str	ip, [sp]
6030b758:	eb000005 	bl	6030b774 <mmap_add_region>
6030b75c:	eb000012 	bl	6030b7ac <init_xlat_tables>
6030b760:	e3a00000 	mov	r0, #0
6030b764:	e28dd008 	add	sp, sp, #8
6030b768:	e8bd4010 	pop	{r4, lr}
6030b76c:	ea00003c 	b	6030b864 <enable_mmu_svc_mon>
6030b770:	7000001f 	.word	0x7000001f

6030b774 <mmap_add_region>:
6030b774:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
6030b778:	e3a0e101 	mov	lr, #1073741824	; 0x40000000
6030b77c:	e24dd01c 	sub	sp, sp, #28
6030b780:	e1cd00f0 	strd	r0, [sp]
6030b784:	e59dc020 	ldr	ip, [sp, #32]
6030b788:	e1a0100d 	mov	r1, sp
6030b78c:	e3070260 	movw	r0, #29280	; 0x7260
6030b790:	e3460031 	movt	r0, #24625	; 0x6031
6030b794:	e58de014 	str	lr, [sp, #20]
6030b798:	e1cd20f8 	strd	r2, [sp, #8]
6030b79c:	e58dc010 	str	ip, [sp, #16]
6030b7a0:	eb0001ae 	bl	6030be60 <mmap_add_region_ctx>
6030b7a4:	e28dd01c 	add	sp, sp, #28
6030b7a8:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

6030b7ac <init_xlat_tables>:
6030b7ac:	e92d4010 	push	{r4, lr}
6030b7b0:	e3074260 	movw	r4, #29280	; 0x7260
6030b7b4:	e3464031 	movt	r4, #24625	; 0x6031
6030b7b8:	e24dd008 	sub	sp, sp, #8
6030b7bc:	e594303c 	ldr	r3, [r4, #60]	; 0x3c
6030b7c0:	e3730001 	cmn	r3, #1
6030b7c4:	0a00000c 	beq	6030b7fc <init_xlat_tables+0x50>
6030b7c8:	e3a00060 	mov	r0, #96	; 0x60
6030b7cc:	e306c308 	movw	ip, #25352	; 0x6308
6030b7d0:	e346c031 	movt	ip, #24625	; 0x6031
6030b7d4:	e30632e8 	movw	r3, #25320	; 0x62e8
6030b7d8:	e3463031 	movt	r3, #24625	; 0x6031
6030b7dc:	e3051394 	movw	r1, #21396	; 0x5394
6030b7e0:	e3461031 	movt	r1, #24625	; 0x6031
6030b7e4:	e58d0004 	str	r0, [sp, #4]
6030b7e8:	e3a02049 	mov	r2, #73	; 0x49
6030b7ec:	e3a00004 	mov	r0, #4
6030b7f0:	e58dc000 	str	ip, [sp]
6030b7f4:	eb00051b 	bl	6030cc68 <rtk_log_write>
6030b7f8:	eafffffe 	b	6030b7f8 <init_xlat_tables+0x4c>
6030b7fc:	eb0003ca 	bl	6030c72c <xlat_arch_current_el>
6030b800:	e3500001 	cmp	r0, #1
6030b804:	0a000010 	beq	6030b84c <init_xlat_tables+0xa0>
6030b808:	e3500002 	cmp	r0, #2
6030b80c:	0a00000e 	beq	6030b84c <init_xlat_tables+0xa0>
6030b810:	e3500003 	cmp	r0, #3
6030b814:	0a00000c 	beq	6030b84c <init_xlat_tables+0xa0>
6030b818:	e3a00069 	mov	r0, #105	; 0x69
6030b81c:	e306c308 	movw	ip, #25352	; 0x6308
6030b820:	e346c031 	movt	ip, #24625	; 0x6031
6030b824:	e30632e8 	movw	r3, #25320	; 0x62e8
6030b828:	e3463031 	movt	r3, #24625	; 0x6031
6030b82c:	e3051394 	movw	r1, #21396	; 0x5394
6030b830:	e3461031 	movt	r1, #24625	; 0x6031
6030b834:	e58d0004 	str	r0, [sp, #4]
6030b838:	e3a02049 	mov	r2, #73	; 0x49
6030b83c:	e3a00004 	mov	r0, #4
6030b840:	e58dc000 	str	ip, [sp]
6030b844:	eb000507 	bl	6030cc68 <rtk_log_write>
6030b848:	eafffffe 	b	6030b848 <init_xlat_tables+0x9c>
6030b84c:	e584003c 	str	r0, [r4, #60]	; 0x3c
6030b850:	e3070260 	movw	r0, #29280	; 0x7260
6030b854:	e3460031 	movt	r0, #24625	; 0x6031
6030b858:	e28dd008 	add	sp, sp, #8
6030b85c:	e8bd4010 	pop	{r4, lr}
6030b860:	ea000295 	b	6030c2bc <init_xlat_tables_ctx>

6030b864 <enable_mmu_svc_mon>:
6030b864:	e92d4010 	push	{r4, lr}
6030b868:	e3073260 	movw	r3, #29280	; 0x7260
6030b86c:	e3463031 	movt	r3, #24625	; 0x6031
6030b870:	e24dd010 	sub	sp, sp, #16
6030b874:	e1a04000 	mov	r4, r0
6030b878:	e593c008 	ldr	ip, [r3, #8]
6030b87c:	e1c300d0 	ldrd	r0, [r3]
6030b880:	e5932020 	ldr	r2, [r3, #32]
6030b884:	e1cd00f0 	strd	r0, [sp]
6030b888:	e3a03001 	mov	r3, #1
6030b88c:	e1a01004 	mov	r1, r4
6030b890:	e58dc008 	str	ip, [sp, #8]
6030b894:	e58d300c 	str	r3, [sp, #12]
6030b898:	e30709e8 	movw	r0, #31208	; 0x79e8
6030b89c:	e3460031 	movt	r0, #24625	; 0x6031
6030b8a0:	eb0003fc 	bl	6030c898 <setup_mmu_cfg>
6030b8a4:	e1a00004 	mov	r0, r4
6030b8a8:	e28dd010 	add	sp, sp, #16
6030b8ac:	e8bd4010 	pop	{r4, lr}
6030b8b0:	ea000472 	b	6030ca80 <enable_mmu_direct_svc_mon>

6030b8b4 <xlat_desc>:
6030b8b4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
6030b8b8:	e1a04001 	mov	r4, r1
6030b8bc:	e24dd00c 	sub	sp, sp, #12
6030b8c0:	e3e01008 	mvn	r1, #8
6030b8c4:	e3e0c000 	mvn	ip, #0
6030b8c8:	e59de020 	ldr	lr, [sp, #32]
6030b8cc:	e0010e91 	mul	r1, r1, lr
6030b8d0:	e2811027 	add	r1, r1, #39	; 0x27
6030b8d4:	e1d2111c 	bics	r1, r2, ip, lsl r1
6030b8d8:	1a00002a 	bne	6030b988 <xlat_desc+0xd4>
6030b8dc:	e35e0003 	cmp	lr, #3
6030b8e0:	03a05003 	moveq	r5, #3
6030b8e4:	13a05001 	movne	r5, #1
6030b8e8:	e094c004 	adds	ip, r4, r4
6030b8ec:	e590003c 	ldr	r0, [r0, #60]	; 0x3c
6030b8f0:	e3140008 	tst	r4, #8
6030b8f4:	e20cc020 	and	ip, ip, #32
6030b8f8:	03822080 	orreq	r2, r2, #128	; 0x80
6030b8fc:	e185500c 	orr	r5, r5, ip
6030b900:	e1855002 	orr	r5, r5, r2
6030b904:	e1816003 	orr	r6, r1, r3
6030b908:	e3500001 	cmp	r0, #1
6030b90c:	0a000010 	beq	6030b954 <xlat_desc+0xa0>
6030b910:	e2403002 	sub	r3, r0, #2
6030b914:	e3530001 	cmp	r3, #1
6030b918:	8a000034 	bhi	6030b9f0 <xlat_desc+0x13c>
6030b91c:	e3855d11 	orr	r5, r5, #1088	; 0x440
6030b920:	e1a01006 	mov	r1, r6
6030b924:	e2147007 	ands	r7, r4, #7
6030b928:	0a00000f 	beq	6030b96c <xlat_desc+0xb8>
6030b92c:	e3140028 	tst	r4, #40	; 0x28
6030b930:	1a000042 	bne	6030ba40 <xlat_desc+0x18c>
6030b934:	e3570002 	cmp	r7, #2
6030b938:	0a000039 	beq	6030ba24 <xlat_desc+0x170>
6030b93c:	e3570001 	cmp	r7, #1
6030b940:	03855f82 	orreq	r5, r5, #520	; 0x208
6030b944:	1a00001c 	bne	6030b9bc <xlat_desc+0x108>
6030b948:	e1a00005 	mov	r0, r5
6030b94c:	e28dd00c 	add	sp, sp, #12
6030b950:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
6030b954:	e3140040 	tst	r4, #64	; 0x40
6030b958:	03855b01 	orreq	r5, r5, #1024	; 0x400
6030b95c:	01a01006 	moveq	r1, r6
6030b960:	1affffed 	bne	6030b91c <xlat_desc+0x68>
6030b964:	e2147007 	ands	r7, r4, #7
6030b968:	1affffef 	bne	6030b92c <xlat_desc+0x78>
6030b96c:	eb000355 	bl	6030c6c8 <xlat_arch_regime_get_xn_desc>
6030b970:	e1855000 	orr	r5, r5, r0
6030b974:	e3855f81 	orr	r5, r5, #516	; 0x204
6030b978:	e1861001 	orr	r1, r6, r1
6030b97c:	e1a00005 	mov	r0, r5
6030b980:	e28dd00c 	add	sp, sp, #12
6030b984:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
6030b988:	e3a00084 	mov	r0, #132	; 0x84
6030b98c:	e306c38c 	movw	ip, #25484	; 0x638c
6030b990:	e346c031 	movt	ip, #24625	; 0x6031
6030b994:	e30632e8 	movw	r3, #25320	; 0x62e8
6030b998:	e3463031 	movt	r3, #24625	; 0x6031
6030b99c:	e3051394 	movw	r1, #21396	; 0x5394
6030b9a0:	e3461031 	movt	r1, #24625	; 0x6031
6030b9a4:	e58d0004 	str	r0, [sp, #4]
6030b9a8:	e3a02049 	mov	r2, #73	; 0x49
6030b9ac:	e3a00004 	mov	r0, #4
6030b9b0:	e58dc000 	str	ip, [sp]
6030b9b4:	eb0004ab 	bl	6030cc68 <rtk_log_write>
6030b9b8:	eafffffe 	b	6030b9b8 <xlat_desc+0x104>
6030b9bc:	e3a000ee 	mov	r0, #238	; 0xee
6030b9c0:	e306c38c 	movw	ip, #25484	; 0x638c
6030b9c4:	e346c031 	movt	ip, #24625	; 0x6031
6030b9c8:	e30632e8 	movw	r3, #25320	; 0x62e8
6030b9cc:	e3463031 	movt	r3, #24625	; 0x6031
6030b9d0:	e3051394 	movw	r1, #21396	; 0x5394
6030b9d4:	e3461031 	movt	r1, #24625	; 0x6031
6030b9d8:	e58d0004 	str	r0, [sp, #4]
6030b9dc:	e3a02049 	mov	r2, #73	; 0x49
6030b9e0:	e3a00004 	mov	r0, #4
6030b9e4:	e58dc000 	str	ip, [sp]
6030b9e8:	eb00049e 	bl	6030cc68 <rtk_log_write>
6030b9ec:	eafffffe 	b	6030b9ec <xlat_desc+0x138>
6030b9f0:	e3a000a6 	mov	r0, #166	; 0xa6
6030b9f4:	e306c38c 	movw	ip, #25484	; 0x638c
6030b9f8:	e346c031 	movt	ip, #24625	; 0x6031
6030b9fc:	e30632e8 	movw	r3, #25320	; 0x62e8
6030ba00:	e3463031 	movt	r3, #24625	; 0x6031
6030ba04:	e3051394 	movw	r1, #21396	; 0x5394
6030ba08:	e3461031 	movt	r1, #24625	; 0x6031
6030ba0c:	e58d0004 	str	r0, [sp, #4]
6030ba10:	e3a02049 	mov	r2, #73	; 0x49
6030ba14:	e3a00004 	mov	r0, #4
6030ba18:	e58dc000 	str	ip, [sp]
6030ba1c:	eb000491 	bl	6030cc68 <rtk_log_write>
6030ba20:	eafffffe 	b	6030ba20 <xlat_desc+0x16c>
6030ba24:	e2044d06 	and	r4, r4, #384	; 0x180
6030ba28:	e3540d06 	cmp	r4, #384	; 0x180
6030ba2c:	0affffc5 	beq	6030b948 <xlat_desc+0x94>
6030ba30:	e3540c01 	cmp	r4, #256	; 0x100
6030ba34:	03855c02 	orreq	r5, r5, #512	; 0x200
6030ba38:	13855c03 	orrne	r5, r5, #768	; 0x300
6030ba3c:	eaffffc1 	b	6030b948 <xlat_desc+0x94>
6030ba40:	eb000320 	bl	6030c6c8 <xlat_arch_regime_get_xn_desc>
6030ba44:	e1855000 	orr	r5, r5, r0
6030ba48:	e1861001 	orr	r1, r6, r1
6030ba4c:	eaffffb8 	b	6030b934 <xlat_desc+0x80>

6030ba50 <xlat_tables_map_region>:
6030ba50:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
6030ba54:	e1a04002 	mov	r4, r2
6030ba58:	e24dd02c 	sub	sp, sp, #44	; 0x2c
6030ba5c:	e5902034 	ldr	r2, [r0, #52]	; 0x34
6030ba60:	e59d7054 	ldr	r7, [sp, #84]	; 0x54
6030ba64:	e1520007 	cmp	r2, r7
6030ba68:	93570003 	cmpls	r7, #3
6030ba6c:	9a00000c 	bls	6030baa4 <xlat_tables_map_region+0x54>
6030ba70:	e3000251 	movw	r0, #593	; 0x251
6030ba74:	e306c38c 	movw	ip, #25484	; 0x638c
6030ba78:	e346c031 	movt	ip, #24625	; 0x6031
6030ba7c:	e30632e8 	movw	r3, #25320	; 0x62e8
6030ba80:	e3463031 	movt	r3, #24625	; 0x6031
6030ba84:	e3051394 	movw	r1, #21396	; 0x5394
6030ba88:	e3461031 	movt	r1, #24625	; 0x6031
6030ba8c:	e58d0004 	str	r0, [sp, #4]
6030ba90:	e3a02049 	mov	r2, #73	; 0x49
6030ba94:	e3a00004 	mov	r0, #4
6030ba98:	e58dc000 	str	ip, [sp]
6030ba9c:	eb000471 	bl	6030cc68 <rtk_log_write>
6030baa0:	eafffffe 	b	6030baa0 <xlat_tables_map_region+0x50>
6030baa4:	e3e02008 	mvn	r2, #8
6030baa8:	e1a05001 	mov	r5, r1
6030baac:	e0020792 	mul	r2, r2, r7
6030bab0:	e5911008 	ldr	r1, [r1, #8]
6030bab4:	e1a06000 	mov	r6, r0
6030bab8:	e2820027 	add	r0, r2, #39	; 0x27
6030babc:	e595200c 	ldr	r2, [r5, #12]
6030bac0:	e1510004 	cmp	r1, r4
6030bac4:	e0812002 	add	r2, r1, r2
6030bac8:	e2422001 	sub	r2, r2, #1
6030bacc:	e58d2010 	str	r2, [sp, #16]
6030bad0:	e3a02001 	mov	r2, #1
6030bad4:	93a08000 	movls	r8, #0
6030bad8:	e1a02012 	lsl	r2, r2, r0
6030badc:	e58d0014 	str	r0, [sp, #20]
6030bae0:	e58d200c 	str	r2, [sp, #12]
6030bae4:	8a000059 	bhi	6030bc50 <xlat_tables_map_region+0x200>
6030bae8:	e083b188 	add	fp, r3, r8, lsl #3
6030baec:	e59d300c 	ldr	r3, [sp, #12]
6030baf0:	e3a0aa0f 	mov	sl, #61440	; 0xf000
6030baf4:	e34fafff 	movt	sl, #65535	; 0xffff
6030baf8:	e2433001 	sub	r3, r3, #1
6030bafc:	e0849003 	add	r9, r4, r3
6030bb00:	e58d301c 	str	r3, [sp, #28]
6030bb04:	e2873001 	add	r3, r7, #1
6030bb08:	e58d3018 	str	r3, [sp, #24]
6030bb0c:	e59d3050 	ldr	r3, [sp, #80]	; 0x50
6030bb10:	e1580003 	cmp	r8, r3
6030bb14:	2a0000b8 	bcs	6030bdfc <xlat_tables_map_region+0x3ac>
6030bb18:	e5953008 	ldr	r3, [r5, #8]
6030bb1c:	e595200c 	ldr	r2, [r5, #12]
6030bb20:	e1530004 	cmp	r3, r4
6030bb24:	e0832002 	add	r2, r3, r2
6030bb28:	e2422001 	sub	r2, r2, #1
6030bb2c:	83a00000 	movhi	r0, #0
6030bb30:	93a00001 	movls	r0, #1
6030bb34:	e59b1000 	ldr	r1, [fp]
6030bb38:	e1520009 	cmp	r2, r9
6030bb3c:	33a00000 	movcc	r0, #0
6030bb40:	e3500000 	cmp	r0, #0
6030bb44:	e2010003 	and	r0, r1, #3
6030bb48:	1a000019 	bne	6030bbb4 <xlat_tables_map_region+0x164>
6030bb4c:	e1530009 	cmp	r3, r9
6030bb50:	83a03000 	movhi	r3, #0
6030bb54:	93a03001 	movls	r3, #1
6030bb58:	e1520004 	cmp	r2, r4
6030bb5c:	23833001 	orrcs	r3, r3, #1
6030bb60:	e3530000 	cmp	r3, #0
6030bb64:	0a00002f 	beq	6030bc28 <xlat_tables_map_region+0x1d8>
6030bb68:	e3570003 	cmp	r7, #3
6030bb6c:	0a00003f 	beq	6030bc70 <xlat_tables_map_region+0x220>
6030bb70:	e3500000 	cmp	r0, #0
6030bb74:	0a00004a 	beq	6030bca4 <xlat_tables_map_region+0x254>
6030bb78:	e3500003 	cmp	r0, #3
6030bb7c:	0a00008a 	beq	6030bdac <xlat_tables_map_region+0x35c>
6030bb80:	e300022f 	movw	r0, #559	; 0x22f
6030bb84:	e306c38c 	movw	ip, #25484	; 0x638c
6030bb88:	e346c031 	movt	ip, #24625	; 0x6031
6030bb8c:	e30632e8 	movw	r3, #25320	; 0x62e8
6030bb90:	e3463031 	movt	r3, #24625	; 0x6031
6030bb94:	e3051394 	movw	r1, #21396	; 0x5394
6030bb98:	e3461031 	movt	r1, #24625	; 0x6031
6030bb9c:	e58d0004 	str	r0, [sp, #4]
6030bba0:	e3a02049 	mov	r2, #73	; 0x49
6030bba4:	e3a00004 	mov	r0, #4
6030bba8:	e58dc000 	str	ip, [sp]
6030bbac:	eb00042d 	bl	6030cc68 <rtk_log_write>
6030bbb0:	eafffffe 	b	6030bbb0 <xlat_tables_map_region+0x160>
6030bbb4:	e5952000 	ldr	r2, [r5]
6030bbb8:	e595c004 	ldr	ip, [r5, #4]
6030bbbc:	e0523003 	subs	r3, r2, r3
6030bbc0:	e2ccc000 	sbc	ip, ip, #0
6030bbc4:	e0932004 	adds	r2, r3, r4
6030bbc8:	e2ac3000 	adc	r3, ip, #0
6030bbcc:	e3570003 	cmp	r7, #3
6030bbd0:	0a000064 	beq	6030bd68 <xlat_tables_map_region+0x318>
6030bbd4:	e3500003 	cmp	r0, #3
6030bbd8:	0a000073 	beq	6030bdac <xlat_tables_map_region+0x35c>
6030bbdc:	e3500000 	cmp	r0, #0
6030bbe0:	1a000051 	bne	6030bd2c <xlat_tables_map_region+0x2dc>
6030bbe4:	e59d101c 	ldr	r1, [sp, #28]
6030bbe8:	e1120001 	tst	r2, r1
6030bbec:	13a01001 	movne	r1, #1
6030bbf0:	03a01000 	moveq	r1, #0
6030bbf4:	e3570000 	cmp	r7, #0
6030bbf8:	03811001 	orreq	r1, r1, #1
6030bbfc:	e3510000 	cmp	r1, #0
6030bc00:	1a000027 	bne	6030bca4 <xlat_tables_map_region+0x254>
6030bc04:	e5951014 	ldr	r1, [r5, #20]
6030bc08:	e59d0014 	ldr	r0, [sp, #20]
6030bc0c:	e1b01031 	lsrs	r1, r1, r0
6030bc10:	0a000023 	beq	6030bca4 <xlat_tables_map_region+0x254>
6030bc14:	e58d7000 	str	r7, [sp]
6030bc18:	e1a00006 	mov	r0, r6
6030bc1c:	e5951010 	ldr	r1, [r5, #16]
6030bc20:	ebffff23 	bl	6030b8b4 <xlat_desc>
6030bc24:	e1cb00f0 	strd	r0, [fp]
6030bc28:	e59d300c 	ldr	r3, [sp, #12]
6030bc2c:	e59d2010 	ldr	r2, [sp, #16]
6030bc30:	e0844003 	add	r4, r4, r3
6030bc34:	e2888001 	add	r8, r8, #1
6030bc38:	e28bb008 	add	fp, fp, #8
6030bc3c:	e1520004 	cmp	r2, r4
6030bc40:	e0833009 	add	r3, r3, r9
6030bc44:	9a000083 	bls	6030be58 <xlat_tables_map_region+0x408>
6030bc48:	e1a09003 	mov	r9, r3
6030bc4c:	eaffffae 	b	6030bb0c <xlat_tables_map_region+0xbc>
6030bc50:	e59d200c 	ldr	r2, [sp, #12]
6030bc54:	e2622000 	rsb	r2, r2, #0
6030bc58:	e0011002 	and	r1, r1, r2
6030bc5c:	e59d2014 	ldr	r2, [sp, #20]
6030bc60:	e0418004 	sub	r8, r1, r4
6030bc64:	e1a04001 	mov	r4, r1
6030bc68:	e1a08238 	lsr	r8, r8, r2
6030bc6c:	eaffff9d 	b	6030bae8 <xlat_tables_map_region+0x98>
6030bc70:	e3a00f89 	mov	r0, #548	; 0x224
6030bc74:	e306c38c 	movw	ip, #25484	; 0x638c
6030bc78:	e346c031 	movt	ip, #24625	; 0x6031
6030bc7c:	e30632e8 	movw	r3, #25320	; 0x62e8
6030bc80:	e3463031 	movt	r3, #24625	; 0x6031
6030bc84:	e3051394 	movw	r1, #21396	; 0x5394
6030bc88:	e3461031 	movt	r1, #24625	; 0x6031
6030bc8c:	e58d0004 	str	r0, [sp, #4]
6030bc90:	e3a02049 	mov	r2, #73	; 0x49
6030bc94:	e3a00004 	mov	r0, #4
6030bc98:	e58dc000 	str	ip, [sp]
6030bc9c:	eb0003f1 	bl	6030cc68 <rtk_log_write>
6030bca0:	eafffffe 	b	6030bca0 <xlat_tables_map_region+0x250>
6030bca4:	e596201c 	ldr	r2, [r6, #28]
6030bca8:	e5963018 	ldr	r3, [r6, #24]
6030bcac:	e1520003 	cmp	r2, r3
6030bcb0:	aa000053 	bge	6030be04 <xlat_tables_map_region+0x3b4>
6030bcb4:	e5963014 	ldr	r3, [r6, #20]
6030bcb8:	e0933602 	adds	r3, r3, r2, lsl #12
6030bcbc:	e2822001 	add	r2, r2, #1
6030bcc0:	e586201c 	str	r2, [r6, #28]
6030bcc4:	e1a02004 	mov	r2, r4
6030bcc8:	0a000014 	beq	6030bd20 <xlat_tables_map_region+0x2d0>
6030bccc:	e3830003 	orr	r0, r3, #3
6030bcd0:	e58b0000 	str	r0, [fp]
6030bcd4:	e3a00000 	mov	r0, #0
6030bcd8:	e58b0004 	str	r0, [fp, #4]
6030bcdc:	e59d0018 	ldr	r0, [sp, #24]
6030bce0:	e1a01005 	mov	r1, r5
6030bce4:	e58d0004 	str	r0, [sp, #4]
6030bce8:	e3a00c02 	mov	r0, #512	; 0x200
6030bcec:	e58d0000 	str	r0, [sp]
6030bcf0:	e1a00006 	mov	r0, r6
6030bcf4:	e58d3024 	str	r3, [sp, #36]	; 0x24
6030bcf8:	ebffff54 	bl	6030ba50 <xlat_tables_map_region>
6030bcfc:	e58d0020 	str	r0, [sp, #32]
6030bd00:	eb000266 	bl	6030c6a0 <is_dcache_enabled>
6030bd04:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
6030bd08:	e3500000 	cmp	r0, #0
6030bd0c:	1a00004d 	bne	6030be48 <xlat_tables_map_region+0x3f8>
6030bd10:	e59d3020 	ldr	r3, [sp, #32]
6030bd14:	e1530009 	cmp	r3, r9
6030bd18:	0affffc2 	beq	6030bc28 <xlat_tables_map_region+0x1d8>
6030bd1c:	e1a02003 	mov	r2, r3
6030bd20:	e1a00002 	mov	r0, r2
6030bd24:	e28dd02c 	add	sp, sp, #44	; 0x2c
6030bd28:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
6030bd2c:	e3500001 	cmp	r0, #1
6030bd30:	0affffbc 	beq	6030bc28 <xlat_tables_map_region+0x1d8>
6030bd34:	e300020f 	movw	r0, #527	; 0x20f
6030bd38:	e306c38c 	movw	ip, #25484	; 0x638c
6030bd3c:	e346c031 	movt	ip, #24625	; 0x6031
6030bd40:	e30632e8 	movw	r3, #25320	; 0x62e8
6030bd44:	e3463031 	movt	r3, #24625	; 0x6031
6030bd48:	e3051394 	movw	r1, #21396	; 0x5394
6030bd4c:	e3461031 	movt	r1, #24625	; 0x6031
6030bd50:	e58d0004 	str	r0, [sp, #4]
6030bd54:	e3a02049 	mov	r2, #73	; 0x49
6030bd58:	e3a00004 	mov	r0, #4
6030bd5c:	e58dc000 	str	ip, [sp]
6030bd60:	eb0003c0 	bl	6030cc68 <rtk_log_write>
6030bd64:	eafffffe 	b	6030bd64 <xlat_tables_map_region+0x314>
6030bd68:	e3500003 	cmp	r0, #3
6030bd6c:	0affffad 	beq	6030bc28 <xlat_tables_map_region+0x1d8>
6030bd70:	e3500000 	cmp	r0, #0
6030bd74:	0affffa6 	beq	6030bc14 <xlat_tables_map_region+0x1c4>
6030bd78:	e30001e7 	movw	r0, #487	; 0x1e7
6030bd7c:	e306c38c 	movw	ip, #25484	; 0x638c
6030bd80:	e346c031 	movt	ip, #24625	; 0x6031
6030bd84:	e30632e8 	movw	r3, #25320	; 0x62e8
6030bd88:	e3463031 	movt	r3, #24625	; 0x6031
6030bd8c:	e3051394 	movw	r1, #21396	; 0x5394
6030bd90:	e3461031 	movt	r1, #24625	; 0x6031
6030bd94:	e58d0004 	str	r0, [sp, #4]
6030bd98:	e3a02049 	mov	r2, #73	; 0x49
6030bd9c:	e3a00004 	mov	r0, #4
6030bda0:	e58dc000 	str	ip, [sp]
6030bda4:	eb0003af 	bl	6030cc68 <rtk_log_write>
6030bda8:	eafffffe 	b	6030bda8 <xlat_tables_map_region+0x358>
6030bdac:	e59d2018 	ldr	r2, [sp, #24]
6030bdb0:	e001300a 	and	r3, r1, sl
6030bdb4:	e58d2004 	str	r2, [sp, #4]
6030bdb8:	e1a01005 	mov	r1, r5
6030bdbc:	e3a02c02 	mov	r2, #512	; 0x200
6030bdc0:	e1a00006 	mov	r0, r6
6030bdc4:	e58d2000 	str	r2, [sp]
6030bdc8:	e1a02004 	mov	r2, r4
6030bdcc:	e58d3024 	str	r3, [sp, #36]	; 0x24
6030bdd0:	ebffff1e 	bl	6030ba50 <xlat_tables_map_region>
6030bdd4:	e58d0020 	str	r0, [sp, #32]
6030bdd8:	eb000230 	bl	6030c6a0 <is_dcache_enabled>
6030bddc:	e59d3024 	ldr	r3, [sp, #36]	; 0x24
6030bde0:	e3500000 	cmp	r0, #0
6030bde4:	1a000013 	bne	6030be38 <xlat_tables_map_region+0x3e8>
6030bde8:	e59d3020 	ldr	r3, [sp, #32]
6030bdec:	e1530009 	cmp	r3, r9
6030bdf0:	0affff8c 	beq	6030bc28 <xlat_tables_map_region+0x1d8>
6030bdf4:	e59d2020 	ldr	r2, [sp, #32]
6030bdf8:	eaffffc8 	b	6030bd20 <xlat_tables_map_region+0x2d0>
6030bdfc:	e2442001 	sub	r2, r4, #1
6030be00:	eaffffc6 	b	6030bd20 <xlat_tables_map_region+0x2d0>
6030be04:	e3a00072 	mov	r0, #114	; 0x72
6030be08:	e306c38c 	movw	ip, #25484	; 0x638c
6030be0c:	e346c031 	movt	ip, #24625	; 0x6031
6030be10:	e30632e8 	movw	r3, #25320	; 0x62e8
6030be14:	e3463031 	movt	r3, #24625	; 0x6031
6030be18:	e3051394 	movw	r1, #21396	; 0x5394
6030be1c:	e3461031 	movt	r1, #24625	; 0x6031
6030be20:	e58d0004 	str	r0, [sp, #4]
6030be24:	e3a02049 	mov	r2, #73	; 0x49
6030be28:	e3a00004 	mov	r0, #4
6030be2c:	e58dc000 	str	ip, [sp]
6030be30:	eb00038c 	bl	6030cc68 <rtk_log_write>
6030be34:	eafffffe 	b	6030be34 <xlat_tables_map_region+0x3e4>
6030be38:	e1a00003 	mov	r0, r3
6030be3c:	e3a01a01 	mov	r1, #4096	; 0x1000
6030be40:	eb000327 	bl	6030cae4 <clean_dcache_range>
6030be44:	eaffffe7 	b	6030bde8 <xlat_tables_map_region+0x398>
6030be48:	e1a00003 	mov	r0, r3
6030be4c:	e3a01a01 	mov	r1, #4096	; 0x1000
6030be50:	eb000323 	bl	6030cae4 <clean_dcache_range>
6030be54:	eaffffad 	b	6030bd10 <xlat_tables_map_region+0x2c0>
6030be58:	e1a02009 	mov	r2, r9
6030be5c:	eaffffaf 	b	6030bd20 <xlat_tables_map_region+0x2d0>

6030be60 <mmap_add_region_ctx>:
6030be60:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
6030be64:	e24dd034 	sub	sp, sp, #52	; 0x34
6030be68:	e591a00c 	ldr	sl, [r1, #12]
6030be6c:	e1cd00fc 	strd	r0, [sp, #12]
6030be70:	e35a0000 	cmp	sl, #0
6030be74:	0a0000ec 	beq	6030c22c <mmap_add_region_ctx+0x3cc>
6030be78:	e5d03038 	ldrb	r3, [r0, #56]	; 0x38
6030be7c:	e3530000 	cmp	r3, #0
6030be80:	1a00008b 	bne	6030c0b4 <mmap_add_region_ctx+0x254>
6030be84:	e59d1010 	ldr	r1, [sp, #16]
6030be88:	e8911100 	ldm	r1, {r8, ip}
6030be8c:	e5912014 	ldr	r2, [r1, #20]
6030be90:	e7eb3058 	ubfx	r3, r8, #0, #12
6030be94:	e3933000 	orrs	r3, r3, #0
6030be98:	1a0000ff 	bne	6030c29c <mmap_add_region_ctx+0x43c>
6030be9c:	e5916008 	ldr	r6, [r1, #8]
6030bea0:	e18a3006 	orr	r3, sl, r6
6030bea4:	e7eb3053 	ubfx	r3, r3, #0, #12
6030bea8:	e3530000 	cmp	r3, #0
6030beac:	1a0000fa 	bne	6030c29c <mmap_add_region_ctx+0x43c>
6030beb0:	e3520101 	cmp	r2, #1073741824	; 0x40000000
6030beb4:	13520602 	cmpne	r2, #2097152	; 0x200000
6030beb8:	13a03001 	movne	r3, #1
6030bebc:	03a03000 	moveq	r3, #0
6030bec0:	e3520a01 	cmp	r2, #4096	; 0x1000
6030bec4:	03a03000 	moveq	r3, #0
6030bec8:	12033001 	andne	r3, r3, #1
6030becc:	e3530000 	cmp	r3, #0
6030bed0:	1a0000f1 	bne	6030c29c <mmap_add_region_ctx+0x43c>
6030bed4:	e2583001 	subs	r3, r8, #1
6030bed8:	e2cc2000 	sbc	r2, ip, #0
6030bedc:	e093100a 	adds	r1, r3, sl
6030bee0:	e2a20000 	adc	r0, r2, #0
6030bee4:	e1510008 	cmp	r1, r8
6030bee8:	e0d0300c 	sbcs	r3, r0, ip
6030beec:	e08a3006 	add	r3, sl, r6
6030bef0:	e1a02003 	mov	r2, r3
6030bef4:	e58d301c 	str	r3, [sp, #28]
6030bef8:	e2425001 	sub	r5, r2, #1
6030befc:	33a03001 	movcc	r3, #1
6030bf00:	23a03000 	movcs	r3, #0
6030bf04:	e1560005 	cmp	r6, r5
6030bf08:	83833001 	orrhi	r3, r3, #1
6030bf0c:	e3530000 	cmp	r3, #0
6030bf10:	e58d1008 	str	r1, [sp, #8]
6030bf14:	e58d002c 	str	r0, [sp, #44]	; 0x2c
6030bf18:	1a0000e1 	bne	6030c2a4 <mmap_add_region_ctx+0x444>
6030bf1c:	e59d200c 	ldr	r2, [sp, #12]
6030bf20:	e5923008 	ldr	r3, [r2, #8]
6030bf24:	e1550003 	cmp	r5, r3
6030bf28:	8a0000dd 	bhi	6030c2a4 <mmap_add_region_ctx+0x444>
6030bf2c:	e5923000 	ldr	r3, [r2]
6030bf30:	e1530001 	cmp	r3, r1
6030bf34:	e5923004 	ldr	r3, [r2, #4]
6030bf38:	e0d33000 	sbcs	r3, r3, r0
6030bf3c:	3a0000d8 	bcc	6030c2a4 <mmap_add_region_ctx+0x444>
6030bf40:	e592100c 	ldr	r1, [r2, #12]
6030bf44:	e5923010 	ldr	r3, [r2, #16]
6030bf48:	e3a02018 	mov	r2, #24
6030bf4c:	e58d1014 	str	r1, [sp, #20]
6030bf50:	e0231392 	mla	r3, r2, r3, r1
6030bf54:	e58d3018 	str	r3, [sp, #24]
6030bf58:	e513300c 	ldr	r3, [r3, #-12]
6030bf5c:	e3530000 	cmp	r3, #0
6030bf60:	1a0000d3 	bne	6030c2b4 <mmap_add_region_ctx+0x454>
6030bf64:	e591b00c 	ldr	fp, [r1, #12]
6030bf68:	e5913008 	ldr	r3, [r1, #8]
6030bf6c:	e35b0000 	cmp	fp, #0
6030bf70:	e58d3020 	str	r3, [sp, #32]
6030bf74:	0a0000cc 	beq	6030c2ac <mmap_add_region_ctx+0x44c>
6030bf78:	e0819002 	add	r9, r1, r2
6030bf7c:	e1a03009 	mov	r3, r9
6030bf80:	e1a0100b 	mov	r1, fp
6030bf84:	e58db024 	str	fp, [sp, #36]	; 0x24
6030bf88:	e58d9028 	str	r9, [sp, #40]	; 0x28
6030bf8c:	e1a0b000 	mov	fp, r0
6030bf90:	e1a0900c 	mov	r9, ip
6030bf94:	ea00001f 	b	6030c018 <mmap_add_region_ctx+0x1b8>
6030bf98:	e1550000 	cmp	r5, r0
6030bf9c:	33a04000 	movcc	r4, #0
6030bfa0:	23a04001 	movcs	r4, #1
6030bfa4:	e1560002 	cmp	r6, r2
6030bfa8:	83a04000 	movhi	r4, #0
6030bfac:	e3540000 	cmp	r4, #0
6030bfb0:	1a000024 	bne	6030c048 <mmap_add_region_ctx+0x1e8>
6030bfb4:	e2511001 	subs	r1, r1, #1
6030bfb8:	e2c44000 	sbc	r4, r4, #0
6030bfbc:	e09cc00e 	adds	ip, ip, lr
6030bfc0:	e0a74004 	adc	r4, r7, r4
6030bfc4:	e15c0008 	cmp	ip, r8
6030bfc8:	e59dc008 	ldr	ip, [sp, #8]
6030bfcc:	e0d41009 	sbcs	r1, r4, r9
6030bfd0:	33a01001 	movcc	r1, #1
6030bfd4:	23a01000 	movcs	r1, #0
6030bfd8:	e15c000e 	cmp	ip, lr
6030bfdc:	e0dbc007 	sbcs	ip, fp, r7
6030bfe0:	33811001 	orrcc	r1, r1, #1
6030bfe4:	e1560000 	cmp	r6, r0
6030bfe8:	93a00000 	movls	r0, #0
6030bfec:	83a00001 	movhi	r0, #1
6030bff0:	e1550002 	cmp	r5, r2
6030bff4:	33800001 	orrcc	r0, r0, #1
6030bff8:	e3500000 	cmp	r0, #0
6030bffc:	0a00001b 	beq	6030c070 <mmap_add_region_ctx+0x210>
6030c000:	e3510000 	cmp	r1, #0
6030c004:	0a000019 	beq	6030c070 <mmap_add_region_ctx+0x210>
6030c008:	e593100c 	ldr	r1, [r3, #12]
6030c00c:	e2833018 	add	r3, r3, #24
6030c010:	e3510000 	cmp	r1, #0
6030c014:	0a000033 	beq	6030c0e8 <mmap_add_region_ctx+0x288>
6030c018:	e5132010 	ldr	r2, [r3, #-16]
6030c01c:	e241c001 	sub	ip, r1, #1
6030c020:	e08c0002 	add	r0, ip, r2
6030c024:	e1550000 	cmp	r5, r0
6030c028:	83a04000 	movhi	r4, #0
6030c02c:	93a04001 	movls	r4, #1
6030c030:	e1560002 	cmp	r6, r2
6030c034:	33a04000 	movcc	r4, #0
6030c038:	e513e018 	ldr	lr, [r3, #-24]	; 0xffffffe8
6030c03c:	e5137014 	ldr	r7, [r3, #-20]	; 0xffffffec
6030c040:	e3540000 	cmp	r4, #0
6030c044:	0affffd3 	beq	6030bf98 <mmap_add_region_ctx+0x138>
6030c048:	e052e00e 	subs	lr, r2, lr
6030c04c:	e2e77000 	rsc	r7, r7, #0
6030c050:	e056c008 	subs	ip, r6, r8
6030c054:	e2e90000 	rsc	r0, r9, #0
6030c058:	e1570000 	cmp	r7, r0
6030c05c:	015e000c 	cmpeq	lr, ip
6030c060:	1a000002 	bne	6030c070 <mmap_add_region_ctx+0x210>
6030c064:	e15a0001 	cmp	sl, r1
6030c068:	01560002 	cmpeq	r6, r2
6030c06c:	1affffe5 	bne	6030c008 <mmap_add_region_ctx+0x1a8>
6030c070:	e3e01000 	mvn	r1, #0
6030c074:	e306040c 	movw	r0, #25612	; 0x640c
6030c078:	e3460031 	movt	r0, #24625	; 0x6031
6030c07c:	eb000564 	bl	6030d614 <__wrap_printf>
6030c080:	e3a00fce 	mov	r0, #824	; 0x338
6030c084:	e306c38c 	movw	ip, #25484	; 0x638c
6030c088:	e346c031 	movt	ip, #24625	; 0x6031
6030c08c:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c090:	e3463031 	movt	r3, #24625	; 0x6031
6030c094:	e3051394 	movw	r1, #21396	; 0x5394
6030c098:	e3461031 	movt	r1, #24625	; 0x6031
6030c09c:	e58d0004 	str	r0, [sp, #4]
6030c0a0:	e3a02049 	mov	r2, #73	; 0x49
6030c0a4:	e3a00004 	mov	r0, #4
6030c0a8:	e58dc000 	str	ip, [sp]
6030c0ac:	eb0002ed 	bl	6030cc68 <rtk_log_write>
6030c0b0:	eafffffe 	b	6030c0b0 <mmap_add_region_ctx+0x250>
6030c0b4:	e3000333 	movw	r0, #819	; 0x333
6030c0b8:	e306c38c 	movw	ip, #25484	; 0x638c
6030c0bc:	e346c031 	movt	ip, #24625	; 0x6031
6030c0c0:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c0c4:	e3463031 	movt	r3, #24625	; 0x6031
6030c0c8:	e3051394 	movw	r1, #21396	; 0x5394
6030c0cc:	e3461031 	movt	r1, #24625	; 0x6031
6030c0d0:	e58d0004 	str	r0, [sp, #4]
6030c0d4:	e3a02049 	mov	r2, #73	; 0x49
6030c0d8:	e3a00004 	mov	r0, #4
6030c0dc:	e58dc000 	str	ip, [sp]
6030c0e0:	eb0002e0 	bl	6030cc68 <rtk_log_write>
6030c0e4:	eafffffe 	b	6030c0e4 <mmap_add_region_ctx+0x284>
6030c0e8:	e59db024 	ldr	fp, [sp, #36]	; 0x24
6030c0ec:	e59d3020 	ldr	r3, [sp, #32]
6030c0f0:	e59d9028 	ldr	r9, [sp, #40]	; 0x28
6030c0f4:	e08b3003 	add	r3, fp, r3
6030c0f8:	e2432001 	sub	r2, r3, #1
6030c0fc:	e1550002 	cmp	r5, r2
6030c100:	e59d4014 	ldr	r4, [sp, #20]
6030c104:	93a02001 	movls	r2, #1
6030c108:	9a00000b 	bls	6030c13c <mmap_add_region_ctx+0x2dc>
6030c10c:	e594b024 	ldr	fp, [r4, #36]	; 0x24
6030c110:	e5943020 	ldr	r3, [r4, #32]
6030c114:	e25b2000 	subs	r2, fp, #0
6030c118:	e08b3003 	add	r3, fp, r3
6030c11c:	13a02001 	movne	r2, #1
6030c120:	e2430001 	sub	r0, r3, #1
6030c124:	e1500005 	cmp	r0, r5
6030c128:	23a01000 	movcs	r1, #0
6030c12c:	32021001 	andcc	r1, r2, #1
6030c130:	e2844018 	add	r4, r4, #24
6030c134:	e3510000 	cmp	r1, #0
6030c138:	1afffff3 	bne	6030c10c <mmap_add_region_ctx+0x2ac>
6030c13c:	e59d101c 	ldr	r1, [sp, #28]
6030c140:	e15a000b 	cmp	sl, fp
6030c144:	e0410003 	sub	r0, r1, r3
6030c148:	e16f0f10 	clz	r0, r0
6030c14c:	e1a002a0 	lsr	r0, r0, #5
6030c150:	93a00000 	movls	r0, #0
6030c154:	e1120000 	tst	r2, r0
6030c158:	0a00000b 	beq	6030c18c <mmap_add_region_ctx+0x32c>
6030c15c:	e2844018 	add	r4, r4, #24
6030c160:	e1c420d8 	ldrd	r2, [r4, #8]
6030c164:	e3530000 	cmp	r3, #0
6030c168:	115a0003 	cmpne	sl, r3
6030c16c:	e0832002 	add	r2, r3, r2
6030c170:	83a03001 	movhi	r3, #1
6030c174:	93a03000 	movls	r3, #0
6030c178:	e1520001 	cmp	r2, r1
6030c17c:	13a03000 	movne	r3, #0
6030c180:	02033001 	andeq	r3, r3, #1
6030c184:	e3530000 	cmp	r3, #0
6030c188:	1afffff3 	bne	6030c15c <mmap_add_region_ctx+0x2fc>
6030c18c:	e59d3014 	ldr	r3, [sp, #20]
6030c190:	e59d2018 	ldr	r2, [sp, #24]
6030c194:	e1530002 	cmp	r3, r2
6030c198:	359d1018 	ldrcc	r1, [sp, #24]
6030c19c:	3a000003 	bcc	6030c1b0 <mmap_add_region_ctx+0x350>
6030c1a0:	ea000023 	b	6030c234 <mmap_add_region_ctx+0x3d4>
6030c1a4:	e1590001 	cmp	r9, r1
6030c1a8:	2a000021 	bcs	6030c234 <mmap_add_region_ctx+0x3d4>
6030c1ac:	e2899018 	add	r9, r9, #24
6030c1b0:	e599300c 	ldr	r3, [r9, #12]
6030c1b4:	e3530000 	cmp	r3, #0
6030c1b8:	1afffff9 	bne	6030c1a4 <mmap_add_region_ctx+0x344>
6030c1bc:	e58d9014 	str	r9, [sp, #20]
6030c1c0:	e59d3014 	ldr	r3, [sp, #20]
6030c1c4:	e1a01004 	mov	r1, r4
6030c1c8:	e0432004 	sub	r2, r3, r4
6030c1cc:	e2840018 	add	r0, r4, #24
6030c1d0:	eb00230d 	bl	60314e0c <__wrap_memmove>
6030c1d4:	e59d3018 	ldr	r3, [sp, #24]
6030c1d8:	e593300c 	ldr	r3, [r3, #12]
6030c1dc:	e3530000 	cmp	r3, #0
6030c1e0:	1a000020 	bne	6030c268 <mmap_add_region_ctx+0x408>
6030c1e4:	e59dc010 	ldr	ip, [sp, #16]
6030c1e8:	e8bc000f 	ldm	ip!, {r0, r1, r2, r3}
6030c1ec:	e8a4000f 	stmia	r4!, {r0, r1, r2, r3}
6030c1f0:	e59d200c 	ldr	r2, [sp, #12]
6030c1f4:	e89c0003 	ldm	ip, {r0, r1}
6030c1f8:	e8840003 	stm	r4, {r0, r1}
6030c1fc:	e59d1008 	ldr	r1, [sp, #8]
6030c200:	e5923028 	ldr	r3, [r2, #40]	; 0x28
6030c204:	e59d002c 	ldr	r0, [sp, #44]	; 0x2c
6030c208:	e1530001 	cmp	r3, r1
6030c20c:	e592302c 	ldr	r3, [r2, #44]	; 0x2c
6030c210:	e0d33000 	sbcs	r3, r3, r0
6030c214:	35821028 	strcc	r1, [r2, #40]	; 0x28
6030c218:	3582002c 	strcc	r0, [r2, #44]	; 0x2c
6030c21c:	e59d200c 	ldr	r2, [sp, #12]
6030c220:	e5923030 	ldr	r3, [r2, #48]	; 0x30
6030c224:	e1530005 	cmp	r3, r5
6030c228:	35825030 	strcc	r5, [r2, #48]	; 0x30
6030c22c:	e28dd034 	add	sp, sp, #52	; 0x34
6030c230:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
6030c234:	e300336b 	movw	r3, #875	; 0x36b
6030c238:	e306138c 	movw	r1, #25484	; 0x638c
6030c23c:	e3461031 	movt	r1, #24625	; 0x6031
6030c240:	e58d3004 	str	r3, [sp, #4]
6030c244:	e58d1000 	str	r1, [sp]
6030c248:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c24c:	e3463031 	movt	r3, #24625	; 0x6031
6030c250:	e3051394 	movw	r1, #21396	; 0x5394
6030c254:	e3461031 	movt	r1, #24625	; 0x6031
6030c258:	e3a02049 	mov	r2, #73	; 0x49
6030c25c:	e3a00004 	mov	r0, #4
6030c260:	eb000280 	bl	6030cc68 <rtk_log_write>
6030c264:	eafffffe 	b	6030c264 <mmap_add_region_ctx+0x404>
6030c268:	e3000377 	movw	r0, #887	; 0x377
6030c26c:	e306c38c 	movw	ip, #25484	; 0x638c
6030c270:	e346c031 	movt	ip, #24625	; 0x6031
6030c274:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c278:	e3463031 	movt	r3, #24625	; 0x6031
6030c27c:	e3051394 	movw	r1, #21396	; 0x5394
6030c280:	e3461031 	movt	r1, #24625	; 0x6031
6030c284:	e58d0004 	str	r0, [sp, #4]
6030c288:	e3a02049 	mov	r2, #73	; 0x49
6030c28c:	e3a00004 	mov	r0, #4
6030c290:	e58dc000 	str	ip, [sp]
6030c294:	eb000273 	bl	6030cc68 <rtk_log_write>
6030c298:	eafffffe 	b	6030c298 <mmap_add_region_ctx+0x438>
6030c29c:	e3e01015 	mvn	r1, #21
6030c2a0:	eaffff73 	b	6030c074 <mmap_add_region_ctx+0x214>
6030c2a4:	e3e01021 	mvn	r1, #33	; 0x21
6030c2a8:	eaffff71 	b	6030c074 <mmap_add_region_ctx+0x214>
6030c2ac:	e59d4014 	ldr	r4, [sp, #20]
6030c2b0:	eaffffc2 	b	6030c1c0 <mmap_add_region_ctx+0x360>
6030c2b4:	e3e0100b 	mvn	r1, #11
6030c2b8:	eaffff6d 	b	6030c074 <mmap_add_region_ctx+0x214>

6030c2bc <init_xlat_tables_ctx>:
6030c2bc:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
6030c2c0:	e2504000 	subs	r4, r0, #0
6030c2c4:	e24dd018 	sub	sp, sp, #24
6030c2c8:	0a00007b 	beq	6030c4bc <init_xlat_tables_ctx+0x200>
6030c2cc:	e5d43038 	ldrb	r3, [r4, #56]	; 0x38
6030c2d0:	e3530000 	cmp	r3, #0
6030c2d4:	1a00005e 	bne	6030c454 <init_xlat_tables_ctx+0x198>
6030c2d8:	e594303c 	ldr	r3, [r4, #60]	; 0x3c
6030c2dc:	e2433001 	sub	r3, r3, #1
6030c2e0:	e3530002 	cmp	r3, #2
6030c2e4:	9a00000c 	bls	6030c31c <init_xlat_tables_ctx+0x60>
6030c2e8:	e30004d7 	movw	r0, #1239	; 0x4d7
6030c2ec:	e306c38c 	movw	ip, #25484	; 0x638c
6030c2f0:	e346c031 	movt	ip, #24625	; 0x6031
6030c2f4:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c2f8:	e3463031 	movt	r3, #24625	; 0x6031
6030c2fc:	e3051394 	movw	r1, #21396	; 0x5394
6030c300:	e3461031 	movt	r1, #24625	; 0x6031
6030c304:	e58d0004 	str	r0, [sp, #4]
6030c308:	e3a02049 	mov	r2, #73	; 0x49
6030c30c:	e3a00004 	mov	r0, #4
6030c310:	e58dc000 	str	ip, [sp]
6030c314:	eb000253 	bl	6030cc68 <rtk_log_write>
6030c318:	eafffffe 	b	6030c318 <init_xlat_tables_ctx+0x5c>
6030c31c:	eb000121 	bl	6030c7a8 <is_mmu_enabled_ctx>
6030c320:	e3500000 	cmp	r0, #0
6030c324:	1a000057 	bne	6030c488 <init_xlat_tables_ctx+0x1cc>
6030c328:	e5946008 	ldr	r6, [r4, #8]
6030c32c:	e594500c 	ldr	r5, [r4, #12]
6030c330:	eb0000d8 	bl	6030c698 <xlat_get_min_virt_addr_space_size>
6030c334:	e2403001 	sub	r3, r0, #1
6030c338:	e1560003 	cmp	r6, r3
6030c33c:	3a000078 	bcc	6030c524 <init_xlat_tables_ctx+0x268>
6030c340:	e5942008 	ldr	r2, [r4, #8]
6030c344:	e2823001 	add	r3, r2, #1
6030c348:	e0136002 	ands	r6, r3, r2
6030c34c:	1a000067 	bne	6030c4f0 <init_xlat_tables_ctx+0x234>
6030c350:	e1a00005 	mov	r0, r5
6030c354:	eb0000ca 	bl	6030c684 <xlat_mmap_print>
6030c358:	e5942024 	ldr	r2, [r4, #36]	; 0x24
6030c35c:	e3520000 	cmp	r2, #0
6030c360:	0a000008 	beq	6030c388 <init_xlat_tables_ctx+0xcc>
6030c364:	f2c00010 	vmov.i32	d16, #0	; 0x00000000
6030c368:	e1a03006 	mov	r3, r6
6030c36c:	e5942020 	ldr	r2, [r4, #32]
6030c370:	e0822183 	add	r2, r2, r3, lsl #3
6030c374:	e2833001 	add	r3, r3, #1
6030c378:	edc20b00 	vstr	d16, [r2]
6030c37c:	e5942024 	ldr	r2, [r4, #36]	; 0x24
6030c380:	e1520003 	cmp	r2, r3
6030c384:	8afffff8 	bhi	6030c36c <init_xlat_tables_ctx+0xb0>
6030c388:	e5943018 	ldr	r3, [r4, #24]
6030c38c:	e3530000 	cmp	r3, #0
6030c390:	da00000e 	ble	6030c3d0 <init_xlat_tables_ctx+0x114>
6030c394:	f2c00010 	vmov.i32	d16, #0	; 0x00000000
6030c398:	e3a00000 	mov	r0, #0
6030c39c:	e1a01600 	lsl	r1, r0, #12
6030c3a0:	e3a02000 	mov	r2, #0
6030c3a4:	e5943014 	ldr	r3, [r4, #20]
6030c3a8:	e0833001 	add	r3, r3, r1
6030c3ac:	e0833182 	add	r3, r3, r2, lsl #3
6030c3b0:	e2822001 	add	r2, r2, #1
6030c3b4:	e3520c02 	cmp	r2, #512	; 0x200
6030c3b8:	edc30b00 	vstr	d16, [r3]
6030c3bc:	1afffff8 	bne	6030c3a4 <init_xlat_tables_ctx+0xe8>
6030c3c0:	e5943018 	ldr	r3, [r4, #24]
6030c3c4:	e2800001 	add	r0, r0, #1
6030c3c8:	e1530000 	cmp	r3, r0
6030c3cc:	cafffff2 	bgt	6030c39c <init_xlat_tables_ctx+0xe0>
6030c3d0:	e595300c 	ldr	r3, [r5, #12]
6030c3d4:	e3530000 	cmp	r3, #0
6030c3d8:	1a00000a 	bne	6030c408 <init_xlat_tables_ctx+0x14c>
6030c3dc:	ea00005d 	b	6030c558 <init_xlat_tables_ctx+0x29c>
6030c3e0:	e5950008 	ldr	r0, [r5, #8]
6030c3e4:	e595200c 	ldr	r2, [r5, #12]
6030c3e8:	e0803002 	add	r3, r0, r2
6030c3ec:	e2433001 	sub	r3, r3, #1
6030c3f0:	e1530006 	cmp	r3, r6
6030c3f4:	1a000075 	bne	6030c5d0 <init_xlat_tables_ctx+0x314>
6030c3f8:	e5953024 	ldr	r3, [r5, #36]	; 0x24
6030c3fc:	e2855018 	add	r5, r5, #24
6030c400:	e3530000 	cmp	r3, #0
6030c404:	0a000053 	beq	6030c558 <init_xlat_tables_ctx+0x29c>
6030c408:	e5942034 	ldr	r2, [r4, #52]	; 0x34
6030c40c:	e5940024 	ldr	r0, [r4, #36]	; 0x24
6030c410:	e5943020 	ldr	r3, [r4, #32]
6030c414:	e1a01005 	mov	r1, r5
6030c418:	e58d2004 	str	r2, [sp, #4]
6030c41c:	e58d0000 	str	r0, [sp]
6030c420:	e3a02000 	mov	r2, #0
6030c424:	e1a00004 	mov	r0, r4
6030c428:	ebfffd88 	bl	6030ba50 <xlat_tables_map_region>
6030c42c:	e5947020 	ldr	r7, [r4, #32]
6030c430:	e5948024 	ldr	r8, [r4, #36]	; 0x24
6030c434:	e1a06000 	mov	r6, r0
6030c438:	eb000098 	bl	6030c6a0 <is_dcache_enabled>
6030c43c:	e3500000 	cmp	r0, #0
6030c440:	0affffe6 	beq	6030c3e0 <init_xlat_tables_ctx+0x124>
6030c444:	e1a01188 	lsl	r1, r8, #3
6030c448:	e1a00007 	mov	r0, r7
6030c44c:	eb0001a4 	bl	6030cae4 <clean_dcache_range>
6030c450:	eaffffe2 	b	6030c3e0 <init_xlat_tables_ctx+0x124>
6030c454:	e30004d6 	movw	r0, #1238	; 0x4d6
6030c458:	e306c38c 	movw	ip, #25484	; 0x638c
6030c45c:	e346c031 	movt	ip, #24625	; 0x6031
6030c460:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c464:	e3463031 	movt	r3, #24625	; 0x6031
6030c468:	e3051394 	movw	r1, #21396	; 0x5394
6030c46c:	e3461031 	movt	r1, #24625	; 0x6031
6030c470:	e58d0004 	str	r0, [sp, #4]
6030c474:	e3a02049 	mov	r2, #73	; 0x49
6030c478:	e3a00004 	mov	r0, #4
6030c47c:	e58dc000 	str	ip, [sp]
6030c480:	eb0001f8 	bl	6030cc68 <rtk_log_write>
6030c484:	eafffffe 	b	6030c484 <init_xlat_tables_ctx+0x1c8>
6030c488:	e30004da 	movw	r0, #1242	; 0x4da
6030c48c:	e306c38c 	movw	ip, #25484	; 0x638c
6030c490:	e346c031 	movt	ip, #24625	; 0x6031
6030c494:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c498:	e3463031 	movt	r3, #24625	; 0x6031
6030c49c:	e3051394 	movw	r1, #21396	; 0x5394
6030c4a0:	e3461031 	movt	r1, #24625	; 0x6031
6030c4a4:	e58d0004 	str	r0, [sp, #4]
6030c4a8:	e3a02049 	mov	r2, #73	; 0x49
6030c4ac:	e3a00004 	mov	r0, #4
6030c4b0:	e58dc000 	str	ip, [sp]
6030c4b4:	eb0001eb 	bl	6030cc68 <rtk_log_write>
6030c4b8:	eafffffe 	b	6030c4b8 <init_xlat_tables_ctx+0x1fc>
6030c4bc:	e30004d5 	movw	r0, #1237	; 0x4d5
6030c4c0:	e306c38c 	movw	ip, #25484	; 0x638c
6030c4c4:	e346c031 	movt	ip, #24625	; 0x6031
6030c4c8:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c4cc:	e3463031 	movt	r3, #24625	; 0x6031
6030c4d0:	e3051394 	movw	r1, #21396	; 0x5394
6030c4d4:	e3461031 	movt	r1, #24625	; 0x6031
6030c4d8:	e58d0004 	str	r0, [sp, #4]
6030c4dc:	e3a02049 	mov	r2, #73	; 0x49
6030c4e0:	e3a00004 	mov	r0, #4
6030c4e4:	e58dc000 	str	ip, [sp]
6030c4e8:	eb0001de 	bl	6030cc68 <rtk_log_write>
6030c4ec:	eafffffe 	b	6030c4ec <init_xlat_tables_ctx+0x230>
6030c4f0:	e30004e1 	movw	r0, #1249	; 0x4e1
6030c4f4:	e306c38c 	movw	ip, #25484	; 0x638c
6030c4f8:	e346c031 	movt	ip, #24625	; 0x6031
6030c4fc:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c500:	e3463031 	movt	r3, #24625	; 0x6031
6030c504:	e3051394 	movw	r1, #21396	; 0x5394
6030c508:	e3461031 	movt	r1, #24625	; 0x6031
6030c50c:	e58d0004 	str	r0, [sp, #4]
6030c510:	e3a02049 	mov	r2, #73	; 0x49
6030c514:	e3a00004 	mov	r0, #4
6030c518:	e58dc000 	str	ip, [sp]
6030c51c:	eb0001d1 	bl	6030cc68 <rtk_log_write>
6030c520:	eafffffe 	b	6030c520 <init_xlat_tables_ctx+0x264>
6030c524:	e30004de 	movw	r0, #1246	; 0x4de
6030c528:	e306c38c 	movw	ip, #25484	; 0x638c
6030c52c:	e346c031 	movt	ip, #24625	; 0x6031
6030c530:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c534:	e3463031 	movt	r3, #24625	; 0x6031
6030c538:	e3051394 	movw	r1, #21396	; 0x5394
6030c53c:	e3461031 	movt	r1, #24625	; 0x6031
6030c540:	e58d0004 	str	r0, [sp, #4]
6030c544:	e3a02049 	mov	r2, #73	; 0x49
6030c548:	e3a00004 	mov	r0, #4
6030c54c:	e58dc000 	str	ip, [sp]
6030c550:	eb0001c4 	bl	6030cc68 <rtk_log_write>
6030c554:	eafffffe 	b	6030c554 <init_xlat_tables_ctx+0x298>
6030c558:	e5946000 	ldr	r6, [r4]
6030c55c:	e5945004 	ldr	r5, [r4, #4]
6030c560:	eb000049 	bl	6030c68c <xlat_arch_get_max_supported_pa>
6030c564:	e1500006 	cmp	r0, r6
6030c568:	e0d13005 	sbcs	r3, r1, r5
6030c56c:	3a000024 	bcc	6030c604 <init_xlat_tables_ctx+0x348>
6030c570:	e5942030 	ldr	r2, [r4, #48]	; 0x30
6030c574:	e5943008 	ldr	r3, [r4, #8]
6030c578:	e1520003 	cmp	r2, r3
6030c57c:	8a000033 	bhi	6030c650 <init_xlat_tables_ctx+0x394>
6030c580:	e5941028 	ldr	r1, [r4, #40]	; 0x28
6030c584:	e5940000 	ldr	r0, [r4]
6030c588:	e594302c 	ldr	r3, [r4, #44]	; 0x2c
6030c58c:	e5942004 	ldr	r2, [r4, #4]
6030c590:	e1500001 	cmp	r0, r1
6030c594:	e0d23003 	sbcs	r3, r2, r3
6030c598:	2a000026 	bcs	6030c638 <init_xlat_tables_ctx+0x37c>
6030c59c:	e3000508 	movw	r0, #1288	; 0x508
6030c5a0:	e306c38c 	movw	ip, #25484	; 0x638c
6030c5a4:	e346c031 	movt	ip, #24625	; 0x6031
6030c5a8:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c5ac:	e3463031 	movt	r3, #24625	; 0x6031
6030c5b0:	e3051394 	movw	r1, #21396	; 0x5394
6030c5b4:	e3461031 	movt	r1, #24625	; 0x6031
6030c5b8:	e58d0004 	str	r0, [sp, #4]
6030c5bc:	e3a02049 	mov	r2, #73	; 0x49
6030c5c0:	e3a00004 	mov	r0, #4
6030c5c4:	e58dc000 	str	ip, [sp]
6030c5c8:	eb0001a6 	bl	6030cc68 <rtk_log_write>
6030c5cc:	eafffffe 	b	6030c5cc <init_xlat_tables_ctx+0x310>
6030c5d0:	e5953010 	ldr	r3, [r5, #16]
6030c5d4:	e1cd21f0 	strd	r2, [sp, #16]
6030c5d8:	e3051394 	movw	r1, #21396	; 0x5394
6030c5dc:	e3461031 	movt	r1, #24625	; 0x6031
6030c5e0:	e1c540d0 	ldrd	r4, [r5]
6030c5e4:	e3063438 	movw	r3, #25656	; 0x6438
6030c5e8:	e3463031 	movt	r3, #24625	; 0x6031
6030c5ec:	e58d0000 	str	r0, [sp]
6030c5f0:	e3a02049 	mov	r2, #73	; 0x49
6030c5f4:	e3a00004 	mov	r0, #4
6030c5f8:	e1cd40f8 	strd	r4, [sp, #8]
6030c5fc:	eb000199 	bl	6030cc68 <rtk_log_write>
6030c600:	eafffffe 	b	6030c600 <init_xlat_tables_ctx+0x344>
6030c604:	e3000506 	movw	r0, #1286	; 0x506
6030c608:	e306c38c 	movw	ip, #25484	; 0x638c
6030c60c:	e346c031 	movt	ip, #24625	; 0x6031
6030c610:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c614:	e3463031 	movt	r3, #24625	; 0x6031
6030c618:	e3051394 	movw	r1, #21396	; 0x5394
6030c61c:	e3461031 	movt	r1, #24625	; 0x6031
6030c620:	e58d0004 	str	r0, [sp, #4]
6030c624:	e3a02049 	mov	r2, #73	; 0x49
6030c628:	e3a00004 	mov	r0, #4
6030c62c:	e58dc000 	str	ip, [sp]
6030c630:	eb00018c 	bl	6030cc68 <rtk_log_write>
6030c634:	eafffffe 	b	6030c634 <init_xlat_tables_ctx+0x378>
6030c638:	e3a03001 	mov	r3, #1
6030c63c:	e1a00004 	mov	r0, r4
6030c640:	e5c43038 	strb	r3, [r4, #56]	; 0x38
6030c644:	e28dd018 	add	sp, sp, #24
6030c648:	e8bd41f0 	pop	{r4, r5, r6, r7, r8, lr}
6030c64c:	ea00000d 	b	6030c688 <xlat_tables_print>
6030c650:	e3000507 	movw	r0, #1287	; 0x507
6030c654:	e306c38c 	movw	ip, #25484	; 0x638c
6030c658:	e346c031 	movt	ip, #24625	; 0x6031
6030c65c:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c660:	e3463031 	movt	r3, #24625	; 0x6031
6030c664:	e3051394 	movw	r1, #21396	; 0x5394
6030c668:	e3461031 	movt	r1, #24625	; 0x6031
6030c66c:	e58d0004 	str	r0, [sp, #4]
6030c670:	e3a02049 	mov	r2, #73	; 0x49
6030c674:	e3a00004 	mov	r0, #4
6030c678:	e58dc000 	str	ip, [sp]
6030c67c:	eb000179 	bl	6030cc68 <rtk_log_write>
6030c680:	eafffffe 	b	6030c680 <init_xlat_tables_ctx+0x3c4>

6030c684 <xlat_mmap_print>:
6030c684:	e12fff1e 	bx	lr

6030c688 <xlat_tables_print>:
6030c688:	e12fff1e 	bx	lr

6030c68c <xlat_arch_get_max_supported_pa>:
6030c68c:	e3a010ff 	mov	r1, #255	; 0xff
6030c690:	e3e00000 	mvn	r0, #0
6030c694:	e12fff1e 	bx	lr

6030c698 <xlat_get_min_virt_addr_space_size>:
6030c698:	e3a00402 	mov	r0, #33554432	; 0x2000000
6030c69c:	e12fff1e 	bx	lr

6030c6a0 <is_dcache_enabled>:
6030c6a0:	e10f3000 	mrs	r3, CPSR
6030c6a4:	e203301f 	and	r3, r3, #31
6030c6a8:	e353001a 	cmp	r3, #26
6030c6ac:	0a000002 	beq	6030c6bc <is_dcache_enabled+0x1c>
6030c6b0:	ee110f10 	mrc	15, 0, r0, cr1, cr0, {0}
6030c6b4:	e7e00150 	ubfx	r0, r0, #2, #1
6030c6b8:	e12fff1e 	bx	lr
6030c6bc:	ee910f10 	mrc	15, 4, r0, cr1, cr0, {0}
6030c6c0:	e7e00150 	ubfx	r0, r0, #2, #1
6030c6c4:	e12fff1e 	bx	lr

6030c6c8 <xlat_arch_regime_get_xn_desc>:
6030c6c8:	e3500001 	cmp	r0, #1
6030c6cc:	0a000013 	beq	6030c720 <xlat_arch_regime_get_xn_desc+0x58>
6030c6d0:	e3500002 	cmp	r0, #2
6030c6d4:	0a00000e 	beq	6030c714 <xlat_arch_regime_get_xn_desc+0x4c>
6030c6d8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
6030c6dc:	e3a00056 	mov	r0, #86	; 0x56
6030c6e0:	e24dd00c 	sub	sp, sp, #12
6030c6e4:	e306c488 	movw	ip, #25736	; 0x6488
6030c6e8:	e346c031 	movt	ip, #24625	; 0x6031
6030c6ec:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c6f0:	e3463031 	movt	r3, #24625	; 0x6031
6030c6f4:	e3051394 	movw	r1, #21396	; 0x5394
6030c6f8:	e3461031 	movt	r1, #24625	; 0x6031
6030c6fc:	e58d0004 	str	r0, [sp, #4]
6030c700:	e3a02049 	mov	r2, #73	; 0x49
6030c704:	e3a00004 	mov	r0, #4
6030c708:	e58dc000 	str	ip, [sp]
6030c70c:	eb000155 	bl	6030cc68 <rtk_log_write>
6030c710:	eafffffe 	b	6030c710 <xlat_arch_regime_get_xn_desc+0x48>
6030c714:	e3a00000 	mov	r0, #0
6030c718:	e3a01501 	mov	r1, #4194304	; 0x400000
6030c71c:	e12fff1e 	bx	lr
6030c720:	e3a00000 	mov	r0, #0
6030c724:	e3a01606 	mov	r1, #6291456	; 0x600000
6030c728:	e12fff1e 	bx	lr

6030c72c <xlat_arch_current_el>:
6030c72c:	e10f3000 	mrs	r3, CPSR
6030c730:	e203301f 	and	r3, r3, #31
6030c734:	e353001a 	cmp	r3, #26
6030c738:	0a000018 	beq	6030c7a0 <xlat_arch_current_el+0x74>
6030c73c:	e10f3000 	mrs	r3, CPSR
6030c740:	e203301f 	and	r3, r3, #31
6030c744:	e3530013 	cmp	r3, #19
6030c748:	0a000012 	beq	6030c798 <xlat_arch_current_el+0x6c>
6030c74c:	e10f3000 	mrs	r3, CPSR
6030c750:	e203301f 	and	r3, r3, #31
6030c754:	e3530016 	cmp	r3, #22
6030c758:	0a00000e 	beq	6030c798 <xlat_arch_current_el+0x6c>
6030c75c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
6030c760:	e3a0008a 	mov	r0, #138	; 0x8a
6030c764:	e24dd00c 	sub	sp, sp, #12
6030c768:	e306c488 	movw	ip, #25736	; 0x6488
6030c76c:	e346c031 	movt	ip, #24625	; 0x6031
6030c770:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c774:	e3463031 	movt	r3, #24625	; 0x6031
6030c778:	e3051394 	movw	r1, #21396	; 0x5394
6030c77c:	e3461031 	movt	r1, #24625	; 0x6031
6030c780:	e58d0004 	str	r0, [sp, #4]
6030c784:	e3a02049 	mov	r2, #73	; 0x49
6030c788:	e3a00004 	mov	r0, #4
6030c78c:	e58dc000 	str	ip, [sp]
6030c790:	eb000134 	bl	6030cc68 <rtk_log_write>
6030c794:	eafffffe 	b	6030c794 <xlat_arch_current_el+0x68>
6030c798:	e3a00001 	mov	r0, #1
6030c79c:	e12fff1e 	bx	lr
6030c7a0:	e3a00002 	mov	r0, #2
6030c7a4:	e12fff1e 	bx	lr

6030c7a8 <is_mmu_enabled_ctx>:
6030c7a8:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
6030c7ac:	e590303c 	ldr	r3, [r0, #60]	; 0x3c
6030c7b0:	e24dd00c 	sub	sp, sp, #12
6030c7b4:	e3530001 	cmp	r3, #1
6030c7b8:	0a00001e 	beq	6030c838 <is_mmu_enabled_ctx+0x90>
6030c7bc:	e3530002 	cmp	r3, #2
6030c7c0:	0a00000c 	beq	6030c7f8 <is_mmu_enabled_ctx+0x50>
6030c7c4:	e3a00042 	mov	r0, #66	; 0x42
6030c7c8:	e306c488 	movw	ip, #25736	; 0x6488
6030c7cc:	e346c031 	movt	ip, #24625	; 0x6031
6030c7d0:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c7d4:	e3463031 	movt	r3, #24625	; 0x6031
6030c7d8:	e3051394 	movw	r1, #21396	; 0x5394
6030c7dc:	e3461031 	movt	r1, #24625	; 0x6031
6030c7e0:	e58d0004 	str	r0, [sp, #4]
6030c7e4:	e3a02049 	mov	r2, #73	; 0x49
6030c7e8:	e3a00004 	mov	r0, #4
6030c7ec:	e58dc000 	str	ip, [sp]
6030c7f0:	eb00011c 	bl	6030cc68 <rtk_log_write>
6030c7f4:	eafffffe 	b	6030c7f4 <is_mmu_enabled_ctx+0x4c>
6030c7f8:	ebffffcb 	bl	6030c72c <xlat_arch_current_el>
6030c7fc:	e3500002 	cmp	r0, #2
6030c800:	0a00001c 	beq	6030c878 <is_mmu_enabled_ctx+0xd0>
6030c804:	e3a00043 	mov	r0, #67	; 0x43
6030c808:	e306c488 	movw	ip, #25736	; 0x6488
6030c80c:	e346c031 	movt	ip, #24625	; 0x6031
6030c810:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c814:	e3463031 	movt	r3, #24625	; 0x6031
6030c818:	e3051394 	movw	r1, #21396	; 0x5394
6030c81c:	e3461031 	movt	r1, #24625	; 0x6031
6030c820:	e58d0004 	str	r0, [sp, #4]
6030c824:	e3a02049 	mov	r2, #73	; 0x49
6030c828:	e3a00004 	mov	r0, #4
6030c82c:	e58dc000 	str	ip, [sp]
6030c830:	eb00010c 	bl	6030cc68 <rtk_log_write>
6030c834:	eafffffe 	b	6030c834 <is_mmu_enabled_ctx+0x8c>
6030c838:	ebffffbb 	bl	6030c72c <xlat_arch_current_el>
6030c83c:	e3500001 	cmp	r0, #1
6030c840:	0a000010 	beq	6030c888 <is_mmu_enabled_ctx+0xe0>
6030c844:	e3a0003f 	mov	r0, #63	; 0x3f
6030c848:	e306c488 	movw	ip, #25736	; 0x6488
6030c84c:	e346c031 	movt	ip, #24625	; 0x6031
6030c850:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c854:	e3463031 	movt	r3, #24625	; 0x6031
6030c858:	e3051394 	movw	r1, #21396	; 0x5394
6030c85c:	e3461031 	movt	r1, #24625	; 0x6031
6030c860:	e58d0004 	str	r0, [sp, #4]
6030c864:	e3a02049 	mov	r2, #73	; 0x49
6030c868:	e3a00004 	mov	r0, #4
6030c86c:	e58dc000 	str	ip, [sp]
6030c870:	eb0000fc 	bl	6030cc68 <rtk_log_write>
6030c874:	eafffffe 	b	6030c874 <is_mmu_enabled_ctx+0xcc>
6030c878:	ee910f10 	mrc	15, 4, r0, cr1, cr0, {0}
6030c87c:	e2000001 	and	r0, r0, #1
6030c880:	e28dd00c 	add	sp, sp, #12
6030c884:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)
6030c888:	ee110f10 	mrc	15, 0, r0, cr1, cr0, {0}
6030c88c:	e2000001 	and	r0, r0, #1
6030c890:	eafffffa 	b	6030c880 <is_mmu_enabled_ctx+0xd8>
6030c894:	00000000 	andeq	r0, r0, r0

6030c898 <setup_mmu_cfg>:
6030c898:	e92d4070 	push	{r4, r5, r6, lr}
6030c89c:	e1a04000 	mov	r4, r0
6030c8a0:	e24dd010 	sub	sp, sp, #16
6030c8a4:	e1a05001 	mov	r5, r1
6030c8a8:	e59d302c 	ldr	r3, [sp, #44]	; 0x2c
6030c8ac:	e3530001 	cmp	r3, #1
6030c8b0:	0a000030 	beq	6030c978 <setup_mmu_cfg+0xe0>
6030c8b4:	e3530002 	cmp	r3, #2
6030c8b8:	1a000021 	bne	6030c944 <setup_mmu_cfg+0xac>
6030c8bc:	e10f3000 	mrs	r3, CPSR
6030c8c0:	e203301f 	and	r3, r3, #31
6030c8c4:	e353001a 	cmp	r3, #26
6030c8c8:	03a06c35 	moveq	r6, #13568	; 0x3500
6030c8cc:	03486080 	movteq	r6, #32896	; 0x8080
6030c8d0:	1a000033 	bne	6030c9a4 <setup_mmu_cfg+0x10c>
6030c8d4:	e59d3028 	ldr	r3, [sp, #40]	; 0x28
6030c8d8:	e3730001 	cmn	r3, #1
6030c8dc:	0a00000a 	beq	6030c90c <setup_mmu_cfg+0x74>
6030c8e0:	e2830001 	add	r0, r3, #1
6030c8e4:	e3500402 	cmp	r0, #33554432	; 0x2000000
6030c8e8:	3a000047 	bcc	6030ca0c <setup_mmu_cfg+0x174>
6030c8ec:	e59d3028 	ldr	r3, [sp, #40]	; 0x28
6030c8f0:	e0131000 	ands	r1, r3, r0
6030c8f4:	1a000051 	bne	6030ca40 <setup_mmu_cfg+0x1a8>
6030c8f8:	e58d200c 	str	r2, [sp, #12]
6030c8fc:	ebffd003 	bl	60300910 <__ctzdi2>
6030c900:	e59d200c 	ldr	r2, [sp, #12]
6030c904:	e2603020 	rsb	r3, r0, #32
6030c908:	e1866003 	orr	r6, r6, r3
6030c90c:	e3150002 	tst	r5, #2
6030c910:	03866c35 	orreq	r6, r6, #13568	; 0x3500
6030c914:	ee103fd2 	mrc	15, 0, r3, cr0, cr2, {6}
6030c918:	e28f1f56 	add	r1, pc, #344	; 0x158
6030c91c:	e1c100d0 	ldrd	r0, [r1]
6030c920:	e3130a0f 	tst	r3, #61440	; 0xf000
6030c924:	13822001 	orrne	r2, r2, #1
6030c928:	e3a03000 	mov	r3, #0
6030c92c:	e1c400f0 	strd	r0, [r4]
6030c930:	e5846008 	str	r6, [r4, #8]
6030c934:	e584300c 	str	r3, [r4, #12]
6030c938:	e1c421f0 	strd	r2, [r4, #16]
6030c93c:	e28dd010 	add	sp, sp, #16
6030c940:	e8bd8070 	pop	{r4, r5, r6, pc}
6030c944:	e3a000ba 	mov	r0, #186	; 0xba
6030c948:	e306c488 	movw	ip, #25736	; 0x6488
6030c94c:	e346c031 	movt	ip, #24625	; 0x6031
6030c950:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c954:	e3463031 	movt	r3, #24625	; 0x6031
6030c958:	e3051394 	movw	r1, #21396	; 0x5394
6030c95c:	e3461031 	movt	r1, #24625	; 0x6031
6030c960:	e58d0004 	str	r0, [sp, #4]
6030c964:	e3a02049 	mov	r2, #73	; 0x49
6030c968:	e3a00004 	mov	r0, #4
6030c96c:	e58dc000 	str	ip, [sp]
6030c970:	eb0000bc 	bl	6030cc68 <rtk_log_write>
6030c974:	eafffffe 	b	6030c974 <setup_mmu_cfg+0xdc>
6030c978:	e10f3000 	mrs	r3, CPSR
6030c97c:	e203301f 	and	r3, r3, #31
6030c980:	e3530013 	cmp	r3, #19
6030c984:	0a000003 	beq	6030c998 <setup_mmu_cfg+0x100>
6030c988:	e10f3000 	mrs	r3, CPSR
6030c98c:	e203301f 	and	r3, r3, #31
6030c990:	e3530016 	cmp	r3, #22
6030c994:	1a00000f 	bne	6030c9d8 <setup_mmu_cfg+0x140>
6030c998:	e3a06000 	mov	r6, #0
6030c99c:	e3486080 	movt	r6, #32896	; 0x8080
6030c9a0:	eaffffcb 	b	6030c8d4 <setup_mmu_cfg+0x3c>
6030c9a4:	e3a000bb 	mov	r0, #187	; 0xbb
6030c9a8:	e306c488 	movw	ip, #25736	; 0x6488
6030c9ac:	e346c031 	movt	ip, #24625	; 0x6031
6030c9b0:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c9b4:	e3463031 	movt	r3, #24625	; 0x6031
6030c9b8:	e3051394 	movw	r1, #21396	; 0x5394
6030c9bc:	e3461031 	movt	r1, #24625	; 0x6031
6030c9c0:	e58d0004 	str	r0, [sp, #4]
6030c9c4:	e3a02049 	mov	r2, #73	; 0x49
6030c9c8:	e3a00004 	mov	r0, #4
6030c9cc:	e58dc000 	str	ip, [sp]
6030c9d0:	eb0000a4 	bl	6030cc68 <rtk_log_write>
6030c9d4:	eafffffe 	b	6030c9d4 <setup_mmu_cfg+0x13c>
6030c9d8:	e3a000b3 	mov	r0, #179	; 0xb3
6030c9dc:	e306c488 	movw	ip, #25736	; 0x6488
6030c9e0:	e346c031 	movt	ip, #24625	; 0x6031
6030c9e4:	e30632e8 	movw	r3, #25320	; 0x62e8
6030c9e8:	e3463031 	movt	r3, #24625	; 0x6031
6030c9ec:	e3051394 	movw	r1, #21396	; 0x5394
6030c9f0:	e3461031 	movt	r1, #24625	; 0x6031
6030c9f4:	e58d0004 	str	r0, [sp, #4]
6030c9f8:	e3a02049 	mov	r2, #73	; 0x49
6030c9fc:	e3a00004 	mov	r0, #4
6030ca00:	e58dc000 	str	ip, [sp]
6030ca04:	eb000097 	bl	6030cc68 <rtk_log_write>
6030ca08:	eafffffe 	b	6030ca08 <setup_mmu_cfg+0x170>
6030ca0c:	e3a000ce 	mov	r0, #206	; 0xce
6030ca10:	e306c488 	movw	ip, #25736	; 0x6488
6030ca14:	e346c031 	movt	ip, #24625	; 0x6031
6030ca18:	e30632e8 	movw	r3, #25320	; 0x62e8
6030ca1c:	e3463031 	movt	r3, #24625	; 0x6031
6030ca20:	e3051394 	movw	r1, #21396	; 0x5394
6030ca24:	e3461031 	movt	r1, #24625	; 0x6031
6030ca28:	e58d0004 	str	r0, [sp, #4]
6030ca2c:	e3a02049 	mov	r2, #73	; 0x49
6030ca30:	e3a00004 	mov	r0, #4
6030ca34:	e58dc000 	str	ip, [sp]
6030ca38:	eb00008a 	bl	6030cc68 <rtk_log_write>
6030ca3c:	eafffffe 	b	6030ca3c <setup_mmu_cfg+0x1a4>
6030ca40:	e3a000d2 	mov	r0, #210	; 0xd2
6030ca44:	e306c488 	movw	ip, #25736	; 0x6488
6030ca48:	e346c031 	movt	ip, #24625	; 0x6031
6030ca4c:	e30632e8 	movw	r3, #25320	; 0x62e8
6030ca50:	e3463031 	movt	r3, #24625	; 0x6031
6030ca54:	e3051394 	movw	r1, #21396	; 0x5394
6030ca58:	e3461031 	movt	r1, #24625	; 0x6031
6030ca5c:	e58d0004 	str	r0, [sp, #4]
6030ca60:	e3a02049 	mov	r2, #73	; 0x49
6030ca64:	e3a00004 	mov	r0, #4
6030ca68:	e58dc000 	str	ip, [sp]
6030ca6c:	eb00007d 	bl	6030cc68 <rtk_log_write>
6030ca70:	eafffffe 	b	6030ca70 <setup_mmu_cfg+0x1d8>
6030ca74:	e320f000 	nop	{0}
6030ca78:	004404ff 	.word	0x004404ff
6030ca7c:	00000000 	.word	0x00000000

6030ca80 <enable_mmu_direct_svc_mon>:
6030ca80:	ee080f17 	mcr	15, 0, r0, cr8, cr7, {0}
6030ca84:	e1a03000 	mov	r3, r0
6030ca88:	e59f004c 	ldr	r0, [pc, #76]	; 6030cadc <enable_mmu_direct_svc_mon+0x5c>
6030ca8c:	e5901000 	ldr	r1, [r0]
6030ca90:	ee0a1f12 	mcr	15, 0, r1, cr10, cr2, {0}
6030ca94:	e5902008 	ldr	r2, [r0, #8]
6030ca98:	ee022f50 	mcr	15, 0, r2, cr2, cr0, {2}
6030ca9c:	e5901010 	ldr	r1, [r0, #16]
6030caa0:	e5902014 	ldr	r2, [r0, #20]
6030caa4:	ec421f02 	mcrr	15, 0, r1, r2, cr2
6030caa8:	e3a01000 	mov	r1, #0
6030caac:	e3a02000 	mov	r2, #0
6030cab0:	ec421f12 	mcrr	15, 1, r1, r2, cr2
6030cab4:	f57ff04b 	dsb	ish
6030cab8:	f57ff06f 	isb	sy
6030cabc:	ee111f10 	mrc	15, 0, r1, cr1, cr0, {0}
6030cac0:	e59f2018 	ldr	r2, [pc, #24]	; 6030cae0 <enable_mmu_direct_svc_mon+0x60>
6030cac4:	e1811002 	orr	r1, r1, r2
6030cac8:	e3130001 	tst	r3, #1
6030cacc:	13c11004 	bicne	r1, r1, #4
6030cad0:	ee011f10 	mcr	15, 0, r1, cr1, cr0, {0}
6030cad4:	f57ff06f 	isb	sy
6030cad8:	e12fff1e 	bx	lr
6030cadc:	603179e8 	.word	0x603179e8
6030cae0:	00080005 	.word	0x00080005

6030cae4 <clean_dcache_range>:
6030cae4:	e3510000 	cmp	r1, #0
6030cae8:	0a00000b 	beq	6030cb1c <exit_loop_cmvac>
6030caec:	ee103f30 	mrc	15, 0, r3, cr0, cr0, {1}
6030caf0:	e7e33853 	ubfx	r3, r3, #16, #4
6030caf4:	e3a02004 	mov	r2, #4
6030caf8:	e1a02312 	lsl	r2, r2, r3
6030cafc:	e0801001 	add	r1, r0, r1
6030cb00:	e2423001 	sub	r3, r2, #1
6030cb04:	e1c00003 	bic	r0, r0, r3

6030cb08 <loop_cmvac>:
6030cb08:	ee070f3a 	mcr	15, 0, r0, cr7, cr10, {1}
6030cb0c:	e0800002 	add	r0, r0, r2
6030cb10:	e1500001 	cmp	r0, r1
6030cb14:	3afffffb 	bcc	6030cb08 <loop_cmvac>
6030cb18:	f57ff04f 	dsb	sy

6030cb1c <exit_loop_cmvac>:
6030cb1c:	e12fff1e 	bx	lr

6030cb20 <SOCPS_LPWAP_ipc_int>:
6030cb20:	e3a01001 	mov	r1, #1
6030cb24:	e1a00001 	mov	r0, r1
6030cb28:	eafff00b 	b	60308b5c <ipc_get_message>

6030cb2c <DelayUs>:
6030cb2c:	e92d4370 	push	{r4, r5, r6, r8, r9, lr}
6030cb30:	e30434c8 	movw	r3, #17608	; 0x44c8
6030cb34:	e3463031 	movt	r3, #24625	; 0x6031
6030cb38:	e1a06000 	mov	r6, r0
6030cb3c:	e12fff33 	blx	r3
6030cb40:	e3500003 	cmp	r0, #3
6030cb44:	0a000017 	beq	6030cba8 <DelayUs+0x7c>
6030cb48:	ee1e3f33 	mrc	15, 0, r3, cr14, cr3, {1}
6030cb4c:	e3130001 	tst	r3, #1
6030cb50:	0a000014 	beq	6030cba8 <DelayUs+0x7c>
6030cb54:	ec598f1e 	mrrc	15, 1, r8, r9, cr14
6030cb58:	ebfff7c8 	bl	6030aa80 <vGetGenericTimerFreq>
6030cb5c:	e0830096 	umull	r0, r3, r6, r0
6030cb60:	e3042240 	movw	r2, #16960	; 0x4240
6030cb64:	e340200f 	movt	r2, #15
6030cb68:	e1a04008 	mov	r4, r8
6030cb6c:	e0213196 	mla	r1, r6, r1, r3
6030cb70:	e3a03000 	mov	r3, #0
6030cb74:	ebffcf56 	bl	603008d4 <__aeabi_uldivmod>
6030cb78:	e0900008 	adds	r0, r0, r8
6030cb7c:	e0a11009 	adc	r1, r1, r9
6030cb80:	ec532f1e 	mrrc	15, 1, r2, r3, cr14
6030cb84:	e1520000 	cmp	r2, r0
6030cb88:	e0d33001 	sbcs	r3, r3, r1
6030cb8c:	28bd8370 	popcs	{r4, r5, r6, r8, r9, pc}
6030cb90:	e320f000 	nop	{0}
6030cb94:	ec532f1e 	mrrc	15, 1, r2, r3, cr14
6030cb98:	e1520000 	cmp	r2, r0
6030cb9c:	e0d33001 	sbcs	r3, r3, r1
6030cba0:	3afffffa 	bcc	6030cb90 <DelayUs+0x64>
6030cba4:	e8bd8370 	pop	{r4, r5, r6, r8, r9, pc}
6030cba8:	e3a03001 	mov	r3, #1
6030cbac:	ee0e3f33 	mcr	15, 0, r3, cr14, cr3, {1}
6030cbb0:	eaffffe7 	b	6030cb54 <DelayUs+0x28>

6030cbb4 <DelayMs>:
6030cbb4:	e3a02ffa 	mov	r2, #1000	; 0x3e8
6030cbb8:	e30c3b2c 	movw	r3, #52012	; 0xcb2c
6030cbbc:	e3463030 	movt	r3, #24624	; 0x6030
6030cbc0:	e0000092 	mul	r0, r2, r0
6030cbc4:	e12fff13 	bx	r3

6030cbc8 <rtk_log_level_get>:
6030cbc8:	e3073b98 	movw	r3, #31640	; 0x7b98
6030cbcc:	e3463031 	movt	r3, #24625	; 0x6031
6030cbd0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
6030cbd4:	e1a06000 	mov	r6, r0
6030cbd8:	e5932000 	ldr	r2, [r3]
6030cbdc:	e3520003 	cmp	r2, #3
6030cbe0:	9a000016 	bls	6030cc40 <rtk_log_level_get+0x78>
6030cbe4:	e3500000 	cmp	r0, #0
6030cbe8:	13a08004 	movne	r8, #4
6030cbec:	0a000018 	beq	6030cc54 <rtk_log_level_get+0x8c>
6030cbf0:	e3047a28 	movw	r7, #18984	; 0x4a28
6030cbf4:	e3467031 	movt	r7, #24625	; 0x6031
6030cbf8:	e59f5064 	ldr	r5, [pc, #100]	; 6030cc64 <rtk_log_level_get+0x9c>
6030cbfc:	e3a04000 	mov	r4, #0
6030cc00:	ea000003 	b	6030cc14 <rtk_log_level_get+0x4c>
6030cc04:	e2844001 	add	r4, r4, #1
6030cc08:	e285500b 	add	r5, r5, #11
6030cc0c:	e1580004 	cmp	r8, r4
6030cc10:	9a00000f 	bls	6030cc54 <rtk_log_level_get+0x8c>
6030cc14:	e1a00005 	mov	r0, r5
6030cc18:	e1a01006 	mov	r1, r6
6030cc1c:	e12fff37 	blx	r7
6030cc20:	e3500000 	cmp	r0, #0
6030cc24:	1afffff6 	bne	6030cc04 <rtk_log_level_get+0x3c>
6030cc28:	e3a0200b 	mov	r2, #11
6030cc2c:	e3073b9c 	movw	r3, #31644	; 0x7b9c
6030cc30:	e3463031 	movt	r3, #24625	; 0x6031
6030cc34:	e0040492 	mul	r4, r2, r4
6030cc38:	e7d30004 	ldrb	r0, [r3, r4]
6030cc3c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
6030cc40:	e5938000 	ldr	r8, [r3]
6030cc44:	e3500000 	cmp	r0, #0
6030cc48:	0a000001 	beq	6030cc54 <rtk_log_level_get+0x8c>
6030cc4c:	e3580000 	cmp	r8, #0
6030cc50:	1affffe6 	bne	6030cbf0 <rtk_log_level_get+0x28>
6030cc54:	e30732a0 	movw	r3, #29344	; 0x72a0
6030cc58:	e3463031 	movt	r3, #24625	; 0x6031
6030cc5c:	e5d30000 	ldrb	r0, [r3]
6030cc60:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
6030cc64:	60317b9d 	.word	0x60317b9d

6030cc68 <rtk_log_write>:
6030cc68:	e52d3004 	push	{r3}		; (str r3, [sp, #-4]!)
6030cc6c:	e92d4070 	push	{r4, r5, r6, lr}
6030cc70:	e2514000 	subs	r4, r1, #0
6030cc74:	e24dd00c 	sub	sp, sp, #12
6030cc78:	0a000005 	beq	6030cc94 <rtk_log_write+0x2c>
6030cc7c:	e1a05000 	mov	r5, r0
6030cc80:	e1a00004 	mov	r0, r4
6030cc84:	e1a06002 	mov	r6, r2
6030cc88:	ebffffce 	bl	6030cbc8 <rtk_log_level_get>
6030cc8c:	e1500005 	cmp	r0, r5
6030cc90:	2a000003 	bcs	6030cca4 <rtk_log_write+0x3c>
6030cc94:	e28dd00c 	add	sp, sp, #12
6030cc98:	e8bd4070 	pop	{r4, r5, r6, lr}
6030cc9c:	e28dd004 	add	sp, sp, #4
6030cca0:	e12fff1e 	bx	lr
6030cca4:	e3a0000a 	mov	r0, #10
6030cca8:	eb0000fa 	bl	6030d098 <rtos_critical_enter>
6030ccac:	e5d43000 	ldrb	r3, [r4]
6030ccb0:	e3530023 	cmp	r3, #35	; 0x23
6030ccb4:	1a000009 	bne	6030cce0 <rtk_log_write+0x78>
6030ccb8:	e28d1020 	add	r1, sp, #32
6030ccbc:	e59d001c 	ldr	r0, [sp, #28]
6030ccc0:	e58d1004 	str	r1, [sp, #4]
6030ccc4:	eb001766 	bl	60312a64 <DiagVprintf>
6030ccc8:	e3a0000a 	mov	r0, #10
6030cccc:	eb000127 	bl	6030d170 <rtos_critical_exit>
6030ccd0:	e28dd00c 	add	sp, sp, #12
6030ccd4:	e8bd4070 	pop	{r4, r5, r6, lr}
6030ccd8:	e28dd004 	add	sp, sp, #4
6030ccdc:	e12fff1e 	bx	lr
6030cce0:	e1a02006 	mov	r2, r6
6030cce4:	e1a01004 	mov	r1, r4
6030cce8:	e3060508 	movw	r0, #25864	; 0x6508
6030ccec:	e3460031 	movt	r0, #24625	; 0x6031
6030ccf0:	eb001b0f 	bl	60313934 <DiagPrintf>
6030ccf4:	eaffffef 	b	6030ccb8 <rtk_log_write+0x50>

6030ccf8 <rtk_log_write_nano>:
6030ccf8:	e52d3004 	push	{r3}		; (str r3, [sp, #-4]!)
6030ccfc:	e92d4070 	push	{r4, r5, r6, lr}
6030cd00:	e2514000 	subs	r4, r1, #0
6030cd04:	e24dd00c 	sub	sp, sp, #12
6030cd08:	0a000005 	beq	6030cd24 <rtk_log_write_nano+0x2c>
6030cd0c:	e1a05000 	mov	r5, r0
6030cd10:	e1a00004 	mov	r0, r4
6030cd14:	e1a06002 	mov	r6, r2
6030cd18:	ebffffaa 	bl	6030cbc8 <rtk_log_level_get>
6030cd1c:	e1500005 	cmp	r0, r5
6030cd20:	2a000003 	bcs	6030cd34 <rtk_log_write_nano+0x3c>
6030cd24:	e28dd00c 	add	sp, sp, #12
6030cd28:	e8bd4070 	pop	{r4, r5, r6, lr}
6030cd2c:	e28dd004 	add	sp, sp, #4
6030cd30:	e12fff1e 	bx	lr
6030cd34:	e3a0000a 	mov	r0, #10
6030cd38:	eb0000d6 	bl	6030d098 <rtos_critical_enter>
6030cd3c:	e5d43000 	ldrb	r3, [r4]
6030cd40:	e3530023 	cmp	r3, #35	; 0x23
6030cd44:	1a000009 	bne	6030cd70 <rtk_log_write_nano+0x78>
6030cd48:	e28d1020 	add	r1, sp, #32
6030cd4c:	e59d001c 	ldr	r0, [sp, #28]
6030cd50:	e58d1004 	str	r1, [sp, #4]
6030cd54:	eb00196e 	bl	60313314 <DiagVprintfNano>
6030cd58:	e3a0000a 	mov	r0, #10
6030cd5c:	eb000103 	bl	6030d170 <rtos_critical_exit>
6030cd60:	e28dd00c 	add	sp, sp, #12
6030cd64:	e8bd4070 	pop	{r4, r5, r6, lr}
6030cd68:	e28dd004 	add	sp, sp, #4
6030cd6c:	e12fff1e 	bx	lr
6030cd70:	e1a02006 	mov	r2, r6
6030cd74:	e1a01004 	mov	r1, r4
6030cd78:	e3060508 	movw	r0, #25864	; 0x6508
6030cd7c:	e3460031 	movt	r0, #24625	; 0x6031
6030cd80:	eb001af6 	bl	60313960 <DiagPrintfNano>
6030cd84:	eaffffef 	b	6030cd48 <rtk_log_write_nano+0x50>

6030cd88 <rtk_log_memory_dump_word>:
6030cd88:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
6030cd8c:	e2517000 	subs	r7, r1, #0
6030cd90:	e24dd00c 	sub	sp, sp, #12
6030cd94:	0a000025 	beq	6030ce30 <rtk_log_memory_dump_word+0xa8>
6030cd98:	e306951c 	movw	r9, #25884	; 0x651c
6030cd9c:	e3469031 	movt	r9, #24625	; 0x6031
6030cda0:	e3056394 	movw	r6, #21396	; 0x5394
6030cda4:	e3466031 	movt	r6, #24625	; 0x6031
6030cda8:	e1a08000 	mov	r8, r0
6030cdac:	e1a05000 	mov	r5, r0
6030cdb0:	e3a04000 	mov	r4, #0
6030cdb4:	ea00000a 	b	6030cde4 <rtk_log_memory_dump_word+0x5c>
6030cdb8:	e4953004 	ldr	r3, [r5], #4
6030cdbc:	e3a00001 	mov	r0, #1
6030cdc0:	e0844000 	add	r4, r4, r0
6030cdc4:	e58d3000 	str	r3, [sp]
6030cdc8:	e3a02041 	mov	r2, #65	; 0x41
6030cdcc:	e1a01006 	mov	r1, r6
6030cdd0:	e3063528 	movw	r3, #25896	; 0x6528
6030cdd4:	e3463031 	movt	r3, #24625	; 0x6031
6030cdd8:	ebffffc6 	bl	6030ccf8 <rtk_log_write_nano>
6030cddc:	e1540007 	cmp	r4, r7
6030cde0:	0a000012 	beq	6030ce30 <rtk_log_memory_dump_word+0xa8>
6030cde4:	e3540000 	cmp	r4, #0
6030cde8:	0a000008 	beq	6030ce10 <rtk_log_memory_dump_word+0x88>
6030cdec:	e3140007 	tst	r4, #7
6030cdf0:	1afffff0 	bne	6030cdb8 <rtk_log_memory_dump_word+0x30>
6030cdf4:	e1a03009 	mov	r3, r9
6030cdf8:	e3a02041 	mov	r2, #65	; 0x41
6030cdfc:	e1a01006 	mov	r1, r6
6030ce00:	e3a00001 	mov	r0, #1
6030ce04:	e58d5000 	str	r5, [sp]
6030ce08:	ebffffba 	bl	6030ccf8 <rtk_log_write_nano>
6030ce0c:	eaffffe9 	b	6030cdb8 <rtk_log_memory_dump_word+0x30>
6030ce10:	e3a02041 	mov	r2, #65	; 0x41
6030ce14:	e1a01006 	mov	r1, r6
6030ce18:	e3a00001 	mov	r0, #1
6030ce1c:	e58d8000 	str	r8, [sp]
6030ce20:	e3063514 	movw	r3, #25876	; 0x6514
6030ce24:	e3463031 	movt	r3, #24625	; 0x6031
6030ce28:	ebffffb2 	bl	6030ccf8 <rtk_log_write_nano>
6030ce2c:	eaffffe1 	b	6030cdb8 <rtk_log_memory_dump_word+0x30>
6030ce30:	e3a02041 	mov	r2, #65	; 0x41
6030ce34:	e3a00001 	mov	r0, #1
6030ce38:	e3053e20 	movw	r3, #24096	; 0x5e20
6030ce3c:	e3463031 	movt	r3, #24625	; 0x6031
6030ce40:	e3051394 	movw	r1, #21396	; 0x5394
6030ce44:	e3461031 	movt	r1, #24625	; 0x6031
6030ce48:	e28dd00c 	add	sp, sp, #12
6030ce4c:	e8bd43f0 	pop	{r4, r5, r6, r7, r8, r9, lr}
6030ce50:	eaffffa8 	b	6030ccf8 <rtk_log_write_nano>

6030ce54 <rtk_log_memory_dump_byte>:
6030ce54:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
6030ce58:	e2517000 	subs	r7, r1, #0
6030ce5c:	e24dd00c 	sub	sp, sp, #12
6030ce60:	0a000025 	beq	6030cefc <rtk_log_memory_dump_byte+0xa8>
6030ce64:	e306951c 	movw	r9, #25884	; 0x651c
6030ce68:	e3469031 	movt	r9, #24625	; 0x6031
6030ce6c:	e3056394 	movw	r6, #21396	; 0x5394
6030ce70:	e3466031 	movt	r6, #24625	; 0x6031
6030ce74:	e1a08000 	mov	r8, r0
6030ce78:	e1a05000 	mov	r5, r0
6030ce7c:	e3a04000 	mov	r4, #0
6030ce80:	ea00000a 	b	6030ceb0 <rtk_log_memory_dump_byte+0x5c>
6030ce84:	e4d53001 	ldrb	r3, [r5], #1
6030ce88:	e3a00001 	mov	r0, #1
6030ce8c:	e0844000 	add	r4, r4, r0
6030ce90:	e58d3000 	str	r3, [sp]
6030ce94:	e3a02041 	mov	r2, #65	; 0x41
6030ce98:	e1a01006 	mov	r1, r6
6030ce9c:	e3063530 	movw	r3, #25904	; 0x6530
6030cea0:	e3463031 	movt	r3, #24625	; 0x6031
6030cea4:	ebffff93 	bl	6030ccf8 <rtk_log_write_nano>
6030cea8:	e1540007 	cmp	r4, r7
6030ceac:	0a000012 	beq	6030cefc <rtk_log_memory_dump_byte+0xa8>
6030ceb0:	e3540000 	cmp	r4, #0
6030ceb4:	0a000008 	beq	6030cedc <rtk_log_memory_dump_byte+0x88>
6030ceb8:	e3140007 	tst	r4, #7
6030cebc:	1afffff0 	bne	6030ce84 <rtk_log_memory_dump_byte+0x30>
6030cec0:	e1a03009 	mov	r3, r9
6030cec4:	e3a02041 	mov	r2, #65	; 0x41
6030cec8:	e1a01006 	mov	r1, r6
6030cecc:	e3a00001 	mov	r0, #1
6030ced0:	e58d5000 	str	r5, [sp]
6030ced4:	ebffff87 	bl	6030ccf8 <rtk_log_write_nano>
6030ced8:	eaffffe9 	b	6030ce84 <rtk_log_memory_dump_byte+0x30>
6030cedc:	e3a02041 	mov	r2, #65	; 0x41
6030cee0:	e1a01006 	mov	r1, r6
6030cee4:	e3a00001 	mov	r0, #1
6030cee8:	e58d8000 	str	r8, [sp]
6030ceec:	e3063514 	movw	r3, #25876	; 0x6514
6030cef0:	e3463031 	movt	r3, #24625	; 0x6031
6030cef4:	ebffff7f 	bl	6030ccf8 <rtk_log_write_nano>
6030cef8:	eaffffe1 	b	6030ce84 <rtk_log_memory_dump_byte+0x30>
6030cefc:	e3a02041 	mov	r2, #65	; 0x41
6030cf00:	e3a00001 	mov	r0, #1
6030cf04:	e3053e20 	movw	r3, #24096	; 0x5e20
6030cf08:	e3463031 	movt	r3, #24625	; 0x6031
6030cf0c:	e3051394 	movw	r1, #21396	; 0x5394
6030cf10:	e3461031 	movt	r1, #24625	; 0x6031
6030cf14:	e28dd00c 	add	sp, sp, #12
6030cf18:	e8bd43f0 	pop	{r4, r5, r6, r7, r8, r9, lr}
6030cf1c:	eaffff75 	b	6030ccf8 <rtk_log_write_nano>

6030cf20 <os_heap_init>:
6030cf20:	e92d4800 	push	{fp, lr}
6030cf24:	e28db004 	add	fp, sp, #4
6030cf28:	e3023878 	movw	r3, #10360	; 0x2878
6030cf2c:	e3463031 	movt	r3, #24625	; 0x6031
6030cf30:	e12fff33 	blx	r3
6030cf34:	e1a03000 	mov	r3, r0
6030cf38:	e3530000 	cmp	r3, #0
6030cf3c:	0a000005 	beq	6030cf58 <os_heap_init+0x38>
6030cf40:	e3013000 	movw	r3, #4096	; 0x1000
6030cf44:	e340303d 	movt	r3, #61	; 0x3d
6030cf48:	e1a01003 	mov	r1, r3
6030cf4c:	e30f0000 	movw	r0, #61440	; 0xf000
6030cf50:	e3460032 	movt	r0, #24626	; 0x6032
6030cf54:	eb000025 	bl	6030cff0 <os_heap_add>
6030cf58:	e3073bf8 	movw	r3, #31736	; 0x7bf8
6030cf5c:	e3463031 	movt	r3, #24625	; 0x6031
6030cf60:	e5d33000 	ldrb	r3, [r3]
6030cf64:	e3530000 	cmp	r3, #0
6030cf68:	0a000000 	beq	6030cf70 <os_heap_init+0x50>
6030cf6c:	eb000004 	bl	6030cf84 <os_heap_sort>
6030cf70:	e3070bc8 	movw	r0, #31688	; 0x7bc8
6030cf74:	e3460031 	movt	r0, #24625	; 0x6031
6030cf78:	eb001387 	bl	60311d9c <vPortDefineHeapRegions>
6030cf7c:	e320f000 	nop	{0}
6030cf80:	e8bd8800 	pop	{fp, pc}

6030cf84 <os_heap_sort>:
6030cf84:	e3073bf8 	movw	r3, #31736	; 0x7bf8
6030cf88:	e3463031 	movt	r3, #24625	; 0x6031
6030cf8c:	e92d4070 	push	{r4, r5, r6, lr}
6030cf90:	e5d35000 	ldrb	r5, [r3]
6030cf94:	e2455001 	sub	r5, r5, #1
6030cf98:	e3550000 	cmp	r5, #0
6030cf9c:	d8bd8070 	pople	{r4, r5, r6, pc}
6030cfa0:	e3076bc8 	movw	r6, #31688	; 0x7bc8
6030cfa4:	e3466031 	movt	r6, #24625	; 0x6031
6030cfa8:	e086e185 	add	lr, r6, r5, lsl #3
6030cfac:	e1a03006 	mov	r3, r6
6030cfb0:	e5931008 	ldr	r1, [r3, #8]
6030cfb4:	e593c000 	ldr	ip, [r3]
6030cfb8:	e2832008 	add	r2, r3, #8
6030cfbc:	e15c0001 	cmp	ip, r1
6030cfc0:	85934004 	ldrhi	r4, [r3, #4]
6030cfc4:	81c200d0 	ldrdhi	r0, [r2]
6030cfc8:	81c300f0 	strdhi	r0, [r3]
6030cfcc:	8583c008 	strhi	ip, [r3, #8]
6030cfd0:	8583400c 	strhi	r4, [r3, #12]
6030cfd4:	e1a03002 	mov	r3, r2
6030cfd8:	e152000e 	cmp	r2, lr
6030cfdc:	1afffff3 	bne	6030cfb0 <os_heap_sort+0x2c>
6030cfe0:	e24ee008 	sub	lr, lr, #8
6030cfe4:	e2555001 	subs	r5, r5, #1
6030cfe8:	1affffef 	bne	6030cfac <os_heap_sort+0x28>
6030cfec:	e8bd8070 	pop	{r4, r5, r6, pc}

6030cff0 <os_heap_add>:
6030cff0:	e307cbf8 	movw	ip, #31736	; 0x7bf8
6030cff4:	e346c031 	movt	ip, #24625	; 0x6031
6030cff8:	e92d4010 	push	{r4, lr}
6030cffc:	e5dc3000 	ldrb	r3, [ip]
6030d000:	e3530005 	cmp	r3, #5
6030d004:	8a00000f 	bhi	6030d048 <os_heap_add+0x58>
6030d008:	e3072bc8 	movw	r2, #31688	; 0x7bc8
6030d00c:	e3462031 	movt	r2, #24625	; 0x6031
6030d010:	e3510000 	cmp	r1, #0
6030d014:	e082e183 	add	lr, r2, r3, lsl #3
6030d018:	e7820183 	str	r0, [r2, r3, lsl #3]
6030d01c:	13072bfc 	movwne	r2, #31740	; 0x7bfc
6030d020:	13462031 	movtne	r2, #24625	; 0x6031
6030d024:	e58e1004 	str	r1, [lr, #4]
6030d028:	12833001 	addne	r3, r3, #1
6030d02c:	1592e000 	ldrne	lr, [r2]
6030d030:	13a00001 	movne	r0, #1
6030d034:	108e1001 	addne	r1, lr, r1
6030d038:	15cc3000 	strbne	r3, [ip]
6030d03c:	01a00001 	moveq	r0, r1
6030d040:	15821000 	strne	r1, [r2]
6030d044:	e8bd8010 	pop	{r4, pc}
6030d048:	e305006c 	movw	r0, #20588	; 0x506c
6030d04c:	e3460031 	movt	r0, #24625	; 0x6031
6030d050:	e3a01026 	mov	r1, #38	; 0x26
6030d054:	eb001a4c 	bl	6031398c <io_assert_failed>

6030d058 <rtos_critical_is_in_interrupt>:
6030d058:	e10f3000 	mrs	r3, CPSR
6030d05c:	e203301f 	and	r3, r3, #31
6030d060:	e3530010 	cmp	r3, #16
6030d064:	0a000004 	beq	6030d07c <rtos_critical_is_in_interrupt+0x24>
6030d068:	e10f0000 	mrs	r0, CPSR
6030d06c:	e200001f 	and	r0, r0, #31
6030d070:	e250001f 	subs	r0, r0, #31
6030d074:	13a00001 	movne	r0, #1
6030d078:	e12fff1e 	bx	lr
6030d07c:	e3a00000 	mov	r0, #0
6030d080:	e12fff1e 	bx	lr

6030d084 <GetComponentCriticalNesting>:
6030d084:	e1a00080 	lsl	r0, r0, #1
6030d088:	e3073eec 	movw	r3, #32492	; 0x7eec
6030d08c:	e3463031 	movt	r3, #24625	; 0x6031
6030d090:	e19300b0 	ldrh	r0, [r3, r0]
6030d094:	e12fff1e 	bx	lr

6030d098 <rtos_critical_enter>:
6030d098:	e92d4030 	push	{r4, r5, lr}
6030d09c:	e350000a 	cmp	r0, #10
6030d0a0:	e24dd00c 	sub	sp, sp, #12
6030d0a4:	e1a04000 	mov	r4, r0
6030d0a8:	8a000025 	bhi	6030d144 <rtos_critical_enter+0xac>
6030d0ac:	eb0014e0 	bl	60312434 <ulPortInterruptLock>
6030d0b0:	eb0014fa 	bl	603124a0 <ulPortGetCoreId>
6030d0b4:	e3a0300b 	mov	r3, #11
6030d0b8:	e3075ec0 	movw	r5, #32448	; 0x7ec0
6030d0bc:	e3465031 	movt	r5, #24625	; 0x6031
6030d0c0:	e0204093 	mla	r0, r3, r0, r4
6030d0c4:	e1a00080 	lsl	r0, r0, #1
6030d0c8:	e19530b0 	ldrh	r3, [r5, r0]
6030d0cc:	e3530000 	cmp	r3, #0
6030d0d0:	1a00000a 	bne	6030d100 <rtos_critical_enter+0x68>
6030d0d4:	e3073c00 	movw	r3, #31744	; 0x7c00
6030d0d8:	e3463031 	movt	r3, #24625	; 0x6031
6030d0dc:	e0833304 	add	r3, r3, r4, lsl #6
6030d0e0:	e3a02001 	mov	r2, #1
6030d0e4:	e1931f9f 	ldrex	r1, [r3]
6030d0e8:	e3510000 	cmp	r1, #0
6030d0ec:	1320f002 	wfene
6030d0f0:	01831f92 	strexeq	r1, r2, [r3]
6030d0f4:	03510000 	cmpeq	r1, #0
6030d0f8:	1afffff9 	bne	6030d0e4 <rtos_critical_enter+0x4c>
6030d0fc:	f57ff05f 	dmb	sy
6030d100:	eb0014e6 	bl	603124a0 <ulPortGetCoreId>
6030d104:	e3a0300b 	mov	r3, #11
6030d108:	e0244093 	mla	r4, r3, r0, r4
6030d10c:	e1a04084 	lsl	r4, r4, #1
6030d110:	e19530b4 	ldrh	r3, [r5, r4]
6030d114:	e2833001 	add	r3, r3, #1
6030d118:	e6ff3073 	uxth	r3, r3
6030d11c:	e18530b4 	strh	r3, [r5, r4]
6030d120:	eb0014de 	bl	603124a0 <ulPortGetCoreId>
6030d124:	e3073eec 	movw	r3, #32492	; 0x7eec
6030d128:	e3463031 	movt	r3, #24625	; 0x6031
6030d12c:	e1a00080 	lsl	r0, r0, #1
6030d130:	e19320b0 	ldrh	r2, [r3, r0]
6030d134:	e2822001 	add	r2, r2, #1
6030d138:	e18320b0 	strh	r2, [r3, r0]
6030d13c:	e28dd00c 	add	sp, sp, #12
6030d140:	e8bd8030 	pop	{r4, r5, pc}
6030d144:	e3a02045 	mov	r2, #69	; 0x45
6030d148:	e3060568 	movw	r0, #25960	; 0x6568
6030d14c:	e3460031 	movt	r0, #24625	; 0x6031
6030d150:	e3063538 	movw	r3, #25912	; 0x6538
6030d154:	e3463031 	movt	r3, #24625	; 0x6031
6030d158:	e58d0000 	str	r0, [sp]
6030d15c:	e3051394 	movw	r1, #21396	; 0x5394
6030d160:	e3461031 	movt	r1, #24625	; 0x6031
6030d164:	e3a00002 	mov	r0, #2
6030d168:	ebfffee2 	bl	6030ccf8 <rtk_log_write_nano>
6030d16c:	eaffffce 	b	6030d0ac <rtos_critical_enter+0x14>

6030d170 <rtos_critical_exit>:
6030d170:	e92d40f0 	push	{r4, r5, r6, r7, lr}
6030d174:	e350000a 	cmp	r0, #10
6030d178:	e24dd00c 	sub	sp, sp, #12
6030d17c:	e1a04000 	mov	r4, r0
6030d180:	8a000025 	bhi	6030d21c <rtos_critical_exit+0xac>
6030d184:	eb0014c5 	bl	603124a0 <ulPortGetCoreId>
6030d188:	e3075eec 	movw	r5, #32492	; 0x7eec
6030d18c:	e3465031 	movt	r5, #24625	; 0x6031
6030d190:	e1a00080 	lsl	r0, r0, #1
6030d194:	e3a0700b 	mov	r7, #11
6030d198:	e3076ec0 	movw	r6, #32448	; 0x7ec0
6030d19c:	e3466031 	movt	r6, #24625	; 0x6031
6030d1a0:	e19530b0 	ldrh	r3, [r5, r0]
6030d1a4:	e2433001 	sub	r3, r3, #1
6030d1a8:	e18530b0 	strh	r3, [r5, r0]
6030d1ac:	eb0014bb 	bl	603124a0 <ulPortGetCoreId>
6030d1b0:	e0234097 	mla	r3, r7, r0, r4
6030d1b4:	e1a03083 	lsl	r3, r3, #1
6030d1b8:	e19620b3 	ldrh	r2, [r6, r3]
6030d1bc:	e2422001 	sub	r2, r2, #1
6030d1c0:	e6ff2072 	uxth	r2, r2
6030d1c4:	e18620b3 	strh	r2, [r6, r3]
6030d1c8:	eb0014b4 	bl	603124a0 <ulPortGetCoreId>
6030d1cc:	e0204097 	mla	r0, r7, r0, r4
6030d1d0:	e1a00080 	lsl	r0, r0, #1
6030d1d4:	e19630b0 	ldrh	r3, [r6, r0]
6030d1d8:	e6ff3073 	uxth	r3, r3
6030d1dc:	e3530000 	cmp	r3, #0
6030d1e0:	1a000006 	bne	6030d200 <rtos_critical_exit+0x90>
6030d1e4:	e3070c00 	movw	r0, #31744	; 0x7c00
6030d1e8:	e3460031 	movt	r0, #24625	; 0x6031
6030d1ec:	e0800304 	add	r0, r0, r4, lsl #6
6030d1f0:	f57ff05f 	dmb	sy
6030d1f4:	e5803000 	str	r3, [r0]
6030d1f8:	f57ff04f 	dsb	sy
6030d1fc:	e320f004 	sev
6030d200:	eb0014a6 	bl	603124a0 <ulPortGetCoreId>
6030d204:	e1a00080 	lsl	r0, r0, #1
6030d208:	e19530b0 	ldrh	r3, [r5, r0]
6030d20c:	e3530000 	cmp	r3, #0
6030d210:	0a00000c 	beq	6030d248 <rtos_critical_exit+0xd8>
6030d214:	e28dd00c 	add	sp, sp, #12
6030d218:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
6030d21c:	e3a02045 	mov	r2, #69	; 0x45
6030d220:	e3060554 	movw	r0, #25940	; 0x6554
6030d224:	e3460031 	movt	r0, #24625	; 0x6031
6030d228:	e3063538 	movw	r3, #25912	; 0x6538
6030d22c:	e3463031 	movt	r3, #24625	; 0x6031
6030d230:	e58d0000 	str	r0, [sp]
6030d234:	e3051394 	movw	r1, #21396	; 0x5394
6030d238:	e3461031 	movt	r1, #24625	; 0x6031
6030d23c:	e3a00002 	mov	r0, #2
6030d240:	ebfffeac 	bl	6030ccf8 <rtk_log_write_nano>
6030d244:	eaffffce 	b	6030d184 <rtos_critical_exit+0x14>
6030d248:	eb0000a5 	bl	6030d4e4 <rtos_sched_get_state>
6030d24c:	e3500001 	cmp	r0, #1
6030d250:	1a000004 	bne	6030d268 <rtos_critical_exit+0xf8>
6030d254:	f1080080 	cpsie	i
6030d258:	f57ff04f 	dsb	sy
6030d25c:	f57ff06f 	isb	sy
6030d260:	e28dd00c 	add	sp, sp, #12
6030d264:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
6030d268:	eb00148c 	bl	603124a0 <ulPortGetCoreId>
6030d26c:	eb000655 	bl	6030ebc8 <GetOSCriticalNesting>
6030d270:	e3500000 	cmp	r0, #0
6030d274:	1affffe6 	bne	6030d214 <rtos_critical_exit+0xa4>
6030d278:	eafffff5 	b	6030d254 <rtos_critical_exit+0xe4>

6030d27c <__rtos_critical_enter_os>:
6030d27c:	e10f3000 	mrs	r3, CPSR
6030d280:	e203301f 	and	r3, r3, #31
6030d284:	e3530010 	cmp	r3, #16
6030d288:	0a000011 	beq	6030d2d4 <__rtos_critical_enter_os+0x58>
6030d28c:	e10f3000 	mrs	r3, CPSR
6030d290:	e203301f 	and	r3, r3, #31
6030d294:	e353001f 	cmp	r3, #31
6030d298:	0a00000d 	beq	6030d2d4 <__rtos_critical_enter_os+0x58>
6030d29c:	e92d4070 	push	{r4, r5, r6, lr}
6030d2a0:	eb00147e 	bl	603124a0 <ulPortGetCoreId>
6030d2a4:	e3074eec 	movw	r4, #32492	; 0x7eec
6030d2a8:	e3464031 	movt	r4, #24625	; 0x6031
6030d2ac:	e1a00080 	lsl	r0, r0, #1
6030d2b0:	e19430b0 	ldrh	r3, [r4, r0]
6030d2b4:	e3530000 	cmp	r3, #0
6030d2b8:	0a000006 	beq	6030d2d8 <__rtos_critical_enter_os+0x5c>
6030d2bc:	eb001477 	bl	603124a0 <ulPortGetCoreId>
6030d2c0:	e1a00080 	lsl	r0, r0, #1
6030d2c4:	e19430b0 	ldrh	r3, [r4, r0]
6030d2c8:	e2833001 	add	r3, r3, #1
6030d2cc:	e18430b0 	strh	r3, [r4, r0]
6030d2d0:	e8bd8070 	pop	{r4, r5, r6, pc}
6030d2d4:	ea000625 	b	6030eb70 <vTaskEnterCritical>
6030d2d8:	eb001470 	bl	603124a0 <ulPortGetCoreId>
6030d2dc:	e1a05000 	mov	r5, r0
6030d2e0:	eb001453 	bl	60312434 <ulPortInterruptLock>
6030d2e4:	e3073ef0 	movw	r3, #32496	; 0x7ef0
6030d2e8:	e3463031 	movt	r3, #24625	; 0x6031
6030d2ec:	e7830105 	str	r0, [r3, r5, lsl #2]
6030d2f0:	eb001429 	bl	6031239c <xPortSpinLockTask>
6030d2f4:	eafffff0 	b	6030d2bc <__rtos_critical_enter_os+0x40>

6030d2f8 <rtos_mem_init>:
6030d2f8:	eaffff08 	b	6030cf20 <os_heap_init>

6030d2fc <rtos_mem_malloc>:
6030d2fc:	ea0013a3 	b	60312190 <pvPortMalloc>

6030d300 <rtos_mem_zmalloc>:
6030d300:	e92d4070 	push	{r4, r5, r6, lr}
6030d304:	e1a05000 	mov	r5, r0
6030d308:	eb0013a0 	bl	60312190 <pvPortMalloc>
6030d30c:	e2504000 	subs	r4, r0, #0
6030d310:	0a000002 	beq	6030d320 <rtos_mem_zmalloc+0x20>
6030d314:	e1a02005 	mov	r2, r5
6030d318:	e3a01000 	mov	r1, #0
6030d31c:	eb001eb1 	bl	60314de8 <__wrap_memset>
6030d320:	e1a00004 	mov	r0, r4
6030d324:	e8bd8070 	pop	{r4, r5, r6, pc}

6030d328 <rtos_mem_calloc>:
6030d328:	e92d4070 	push	{r4, r5, r6, lr}
6030d32c:	e0050091 	mul	r5, r1, r0
6030d330:	e1a00005 	mov	r0, r5
6030d334:	eb001395 	bl	60312190 <pvPortMalloc>
6030d338:	e2504000 	subs	r4, r0, #0
6030d33c:	0a000002 	beq	6030d34c <rtos_mem_calloc+0x24>
6030d340:	e1a02005 	mov	r2, r5
6030d344:	e3a01000 	mov	r1, #0
6030d348:	eb001ea6 	bl	60314de8 <__wrap_memset>
6030d34c:	e1a00004 	mov	r0, r4
6030d350:	e8bd8070 	pop	{r4, r5, r6, pc}

6030d354 <rtos_mem_realloc>:
6030d354:	ea00138f 	b	60312198 <pvPortReAlloc>

6030d358 <rtos_mem_free>:
6030d358:	e3500000 	cmp	r0, #0
6030d35c:	012fff1e 	bxeq	lr
6030d360:	ea001258 	b	60311cc8 <vPortFree>

6030d364 <rtos_sema_create_binary>:
6030d364:	e92d4010 	push	{r4, lr}
6030d368:	e2504000 	subs	r4, r0, #0
6030d36c:	0a000008 	beq	6030d394 <rtos_sema_create_binary+0x30>
6030d370:	e3a02003 	mov	r2, #3
6030d374:	e3a01000 	mov	r1, #0
6030d378:	e3a00001 	mov	r0, #1
6030d37c:	eb000dc3 	bl	60310a90 <xQueueGenericCreate>
6030d380:	e16f3f10 	clz	r3, r0
6030d384:	e5840000 	str	r0, [r4]
6030d388:	e1a032a3 	lsr	r3, r3, #5
6030d38c:	e2630000 	rsb	r0, r3, #0
6030d390:	e8bd8010 	pop	{r4, pc}
6030d394:	e3e00000 	mvn	r0, #0
6030d398:	e8bd8010 	pop	{r4, pc}

6030d39c <rtos_sema_take>:
6030d39c:	e92d4070 	push	{r4, r5, r6, lr}
6030d3a0:	e3a06000 	mov	r6, #0
6030d3a4:	e24dd008 	sub	sp, sp, #8
6030d3a8:	e1a04000 	mov	r4, r0
6030d3ac:	e1a05001 	mov	r5, r1
6030d3b0:	e58d6004 	str	r6, [sp, #4]
6030d3b4:	ebffff27 	bl	6030d058 <rtos_critical_is_in_interrupt>
6030d3b8:	e1500006 	cmp	r0, r6
6030d3bc:	0a00000b 	beq	6030d3f0 <rtos_sema_take+0x54>
6030d3c0:	e1a00004 	mov	r0, r4
6030d3c4:	e28d2004 	add	r2, sp, #4
6030d3c8:	e1a01006 	mov	r1, r6
6030d3cc:	eb00101b 	bl	60311440 <xQueueReceiveFromISR>
6030d3d0:	e1a04000 	mov	r4, r0
6030d3d4:	e3500001 	cmp	r0, #1
6030d3d8:	1a000016 	bne	6030d438 <rtos_sema_take+0x9c>
6030d3dc:	e59d0004 	ldr	r0, [sp, #4]
6030d3e0:	e1500006 	cmp	r0, r6
6030d3e4:	1a00000b 	bne	6030d418 <rtos_sema_take+0x7c>
6030d3e8:	e28dd008 	add	sp, sp, #8
6030d3ec:	e8bd8070 	pop	{r4, r5, r6, pc}
6030d3f0:	eb0000ff 	bl	6030d7f4 <pmu_yield_os_check>
6030d3f4:	e3500000 	cmp	r0, #0
6030d3f8:	11a01005 	movne	r1, r5
6030d3fc:	03a01000 	moveq	r1, #0
6030d400:	e1a00004 	mov	r0, r4
6030d404:	eb000f72 	bl	603111d4 <xQueueSemaphoreTake>
6030d408:	e2500001 	subs	r0, r0, #1
6030d40c:	13e00000 	mvnne	r0, #0
6030d410:	e28dd008 	add	sp, sp, #8
6030d414:	e8bd8070 	pop	{r4, r5, r6, pc}
6030d418:	eb001420 	bl	603124a0 <ulPortGetCoreId>
6030d41c:	e3083410 	movw	r3, #33808	; 0x8410
6030d420:	e3463031 	movt	r3, #24625	; 0x6031
6030d424:	e1a02000 	mov	r2, r0
6030d428:	e1a00006 	mov	r0, r6
6030d42c:	e7834102 	str	r4, [r3, r2, lsl #2]
6030d430:	e28dd008 	add	sp, sp, #8
6030d434:	e8bd8070 	pop	{r4, r5, r6, pc}
6030d438:	e3e00000 	mvn	r0, #0
6030d43c:	eaffffe9 	b	6030d3e8 <rtos_sema_take+0x4c>

6030d440 <rtos_sema_give>:
6030d440:	e92d4010 	push	{r4, lr}
6030d444:	e3a03000 	mov	r3, #0
6030d448:	e24dd008 	sub	sp, sp, #8
6030d44c:	e1a04000 	mov	r4, r0
6030d450:	e58d3004 	str	r3, [sp, #4]
6030d454:	ebfffeff 	bl	6030d058 <rtos_critical_is_in_interrupt>
6030d458:	e2503000 	subs	r3, r0, #0
6030d45c:	0a00000a 	beq	6030d48c <rtos_sema_give+0x4c>
6030d460:	e1a00004 	mov	r0, r4
6030d464:	e28d1004 	add	r1, sp, #4
6030d468:	eb000e92 	bl	60310eb8 <xQueueGiveFromISR>
6030d46c:	e59d3004 	ldr	r3, [sp, #4]
6030d470:	e1a04000 	mov	r4, r0
6030d474:	e3530000 	cmp	r3, #0
6030d478:	1a00000c 	bne	6030d4b0 <rtos_sema_give+0x70>
6030d47c:	e2540001 	subs	r0, r4, #1
6030d480:	13e00000 	mvnne	r0, #0
6030d484:	e28dd008 	add	sp, sp, #8
6030d488:	e8bd8010 	pop	{r4, pc}
6030d48c:	e1a02003 	mov	r2, r3
6030d490:	e1a01003 	mov	r1, r3
6030d494:	e1a00004 	mov	r0, r4
6030d498:	eb000da4 	bl	60310b30 <xQueueGenericSend>
6030d49c:	e1a04000 	mov	r4, r0
6030d4a0:	e2540001 	subs	r0, r4, #1
6030d4a4:	13e00000 	mvnne	r0, #0
6030d4a8:	e28dd008 	add	sp, sp, #8
6030d4ac:	e8bd8010 	pop	{r4, pc}
6030d4b0:	eb0013fa 	bl	603124a0 <ulPortGetCoreId>
6030d4b4:	e3083410 	movw	r3, #33808	; 0x8410
6030d4b8:	e3463031 	movt	r3, #24625	; 0x6031
6030d4bc:	e3a02001 	mov	r2, #1
6030d4c0:	e7832100 	str	r2, [r3, r0, lsl #2]
6030d4c4:	e2540001 	subs	r0, r4, #1
6030d4c8:	13e00000 	mvnne	r0, #0
6030d4cc:	e28dd008 	add	sp, sp, #8
6030d4d0:	e8bd8010 	pop	{r4, pc}

6030d4d4 <rtos_sched_start>:
6030d4d4:	e92d4010 	push	{r4, lr}
6030d4d8:	eb0006c0 	bl	6030efe0 <vTaskStartScheduler>
6030d4dc:	e3a00000 	mov	r0, #0
6030d4e0:	e8bd8010 	pop	{r4, pc}

6030d4e4 <rtos_sched_get_state>:
6030d4e4:	e92d4010 	push	{r4, lr}
6030d4e8:	eb000574 	bl	6030eac0 <xTaskGetSchedulerState>
6030d4ec:	e3500001 	cmp	r0, #1
6030d4f0:	08bd8010 	popeq	{r4, pc}
6030d4f4:	eb000571 	bl	6030eac0 <xTaskGetSchedulerState>
6030d4f8:	e3500000 	cmp	r0, #0
6030d4fc:	08bd8010 	popeq	{r4, pc}
6030d500:	eb00056e 	bl	6030eac0 <xTaskGetSchedulerState>
6030d504:	e3500002 	cmp	r0, #2
6030d508:	13e00000 	mvnne	r0, #0
6030d50c:	e8bd8010 	pop	{r4, pc}

6030d510 <rtos_task_create>:
6030d510:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
6030d514:	e1a0c002 	mov	ip, r2
6030d518:	e24dd00c 	sub	sp, sp, #12
6030d51c:	e59de010 	ldr	lr, [sp, #16]
6030d520:	e58d0004 	str	r0, [sp, #4]
6030d524:	e1a0000c 	mov	r0, ip
6030d528:	e1ddc1b4 	ldrh	ip, [sp, #20]
6030d52c:	e7ef215e 	ubfx	r2, lr, #2, #16
6030d530:	e58dc000 	str	ip, [sp]
6030d534:	eb000709 	bl	6030f160 <xTaskCreate>
6030d538:	e2500001 	subs	r0, r0, #1
6030d53c:	13e00000 	mvnne	r0, #0
6030d540:	e28dd00c 	add	sp, sp, #12
6030d544:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

6030d548 <rtos_task_delete>:
6030d548:	e92d4010 	push	{r4, lr}
6030d54c:	eb000728 	bl	6030f1f4 <vTaskDelete>
6030d550:	e3a00000 	mov	r0, #0
6030d554:	e8bd8010 	pop	{r4, pc}

6030d558 <rtos_create_secure_context>:
6030d558:	e12fff1e 	bx	lr

6030d55c <rtos_time_delay_ms>:
6030d55c:	e92d4010 	push	{r4, lr}
6030d560:	e1a04000 	mov	r4, r0
6030d564:	eb0000a2 	bl	6030d7f4 <pmu_yield_os_check>
6030d568:	e3500000 	cmp	r0, #0
6030d56c:	0a000002 	beq	6030d57c <rtos_time_delay_ms+0x20>
6030d570:	eb000552 	bl	6030eac0 <xTaskGetSchedulerState>
6030d574:	e3500002 	cmp	r0, #2
6030d578:	0a000004 	beq	6030d590 <rtos_time_delay_ms+0x34>
6030d57c:	e1a00004 	mov	r0, r4
6030d580:	e30c3bb4 	movw	r3, #52148	; 0xcbb4
6030d584:	e3463030 	movt	r3, #24624	; 0x6030
6030d588:	e8bd4010 	pop	{r4, lr}
6030d58c:	e12fff13 	bx	r3
6030d590:	ebfffeb0 	bl	6030d058 <rtos_critical_is_in_interrupt>
6030d594:	e3500000 	cmp	r0, #0
6030d598:	1afffff7 	bne	6030d57c <rtos_time_delay_ms+0x20>
6030d59c:	e1a00004 	mov	r0, r4
6030d5a0:	e8bd4010 	pop	{r4, lr}
6030d5a4:	ea000b8b 	b	603103d8 <vTaskDelay>

6030d5a8 <rtos_time_get_current_system_time_ms>:
6030d5a8:	e92d4010 	push	{r4, lr}
6030d5ac:	ebfffea9 	bl	6030d058 <rtos_critical_is_in_interrupt>
6030d5b0:	e3500000 	cmp	r0, #0
6030d5b4:	0a000001 	beq	6030d5c0 <rtos_time_get_current_system_time_ms+0x18>
6030d5b8:	e8bd4010 	pop	{r4, lr}
6030d5bc:	ea0002fc 	b	6030e1b4 <xTaskGetTickCountFromISR>
6030d5c0:	e8bd4010 	pop	{r4, lr}
6030d5c4:	ea0002f2 	b	6030e194 <xTaskGetTickCount>

6030d5c8 <rtos_time_get_current_pended_time_ms>:
6030d5c8:	e92d4010 	push	{r4, lr}
6030d5cc:	ebfffea1 	bl	6030d058 <rtos_critical_is_in_interrupt>
6030d5d0:	e3500000 	cmp	r0, #0
6030d5d4:	0a000001 	beq	6030d5e0 <rtos_time_get_current_pended_time_ms+0x18>
6030d5d8:	e8bd4010 	pop	{r4, lr}
6030d5dc:	ea0002f8 	b	6030e1c4 <xTaskGetPendedTicksFromISR>
6030d5e0:	e8bd4010 	pop	{r4, lr}
6030d5e4:	ea0002ee 	b	6030e1a4 <xTaskGetPendedTicks>

6030d5e8 <__wrap_malloc>:
6030d5e8:	eaffff43 	b	6030d2fc <rtos_mem_malloc>

6030d5ec <__wrap__malloc_r>:
6030d5ec:	e1a00001 	mov	r0, r1
6030d5f0:	eaffff41 	b	6030d2fc <rtos_mem_malloc>

6030d5f4 <__wrap__realloc_r>:
6030d5f4:	e1a00001 	mov	r0, r1
6030d5f8:	e1a01002 	mov	r1, r2
6030d5fc:	eaffff54 	b	6030d354 <rtos_mem_realloc>

6030d600 <__wrap__free_r>:
6030d600:	e1a00001 	mov	r0, r1
6030d604:	eaffff53 	b	6030d358 <rtos_mem_free>

6030d608 <__wrap__calloc_r>:
6030d608:	e1a00001 	mov	r0, r1
6030d60c:	e1a01002 	mov	r1, r2
6030d610:	eaffff44 	b	6030d328 <rtos_mem_calloc>

6030d614 <__wrap_printf>:
6030d614:	e92d000f 	push	{r0, r1, r2, r3}
6030d618:	e3a0000a 	mov	r0, #10
6030d61c:	e92d4010 	push	{r4, lr}
6030d620:	e24dd008 	sub	sp, sp, #8
6030d624:	ebfffe9b 	bl	6030d098 <rtos_critical_enter>
6030d628:	e28d3014 	add	r3, sp, #20
6030d62c:	e58d3004 	str	r3, [sp, #4]
6030d630:	ebfff18c 	bl	60309c68 <CPU_InInterrupt>
6030d634:	e59d1004 	ldr	r1, [sp, #4]
6030d638:	e3500000 	cmp	r0, #0
6030d63c:	e59d0010 	ldr	r0, [sp, #16]
6030d640:	0a000008 	beq	6030d668 <__wrap_printf+0x54>
6030d644:	eb001506 	bl	60312a64 <DiagVprintf>
6030d648:	e1a04000 	mov	r4, r0
6030d64c:	e3a0000a 	mov	r0, #10
6030d650:	ebfffec6 	bl	6030d170 <rtos_critical_exit>
6030d654:	e1a00004 	mov	r0, r4
6030d658:	e28dd008 	add	sp, sp, #8
6030d65c:	e8bd4010 	pop	{r4, lr}
6030d660:	e28dd010 	add	sp, sp, #16
6030d664:	e12fff1e 	bx	lr
6030d668:	ebffcfb7 	bl	6030154c <vprintf>
6030d66c:	e3073000 	movw	r3, #28672	; 0x7000
6030d670:	e3463031 	movt	r3, #24625	; 0x6031
6030d674:	e1a04000 	mov	r4, r0
6030d678:	e5933000 	ldr	r3, [r3]
6030d67c:	e5930008 	ldr	r0, [r3, #8]
6030d680:	ebffcd94 	bl	60300cd8 <fflush>
6030d684:	eafffff0 	b	6030d64c <__wrap_printf+0x38>

6030d688 <rtk_diag_init>:
6030d688:	e3a00000 	mov	r0, #0
6030d68c:	e12fff1e 	bx	lr

6030d690 <pmu_exec_sleep_hook_funs>:
6030d690:	e92d4070 	push	{r4, r5, r6, lr}
6030d694:	e3074f04 	movw	r4, #32516	; 0x7f04
6030d698:	e3464031 	movt	r4, #24625	; 0x6031
6030d69c:	e3a05000 	mov	r5, #0
6030d6a0:	e5943004 	ldr	r3, [r4, #4]
6030d6a4:	e3a00000 	mov	r0, #0
6030d6a8:	e1530000 	cmp	r3, r0
6030d6ac:	0a000003 	beq	6030d6c0 <pmu_exec_sleep_hook_funs+0x30>
6030d6b0:	e5941008 	ldr	r1, [r4, #8]
6030d6b4:	e12fff33 	blx	r3
6030d6b8:	e3500000 	cmp	r0, #0
6030d6bc:	0a000003 	beq	6030d6d0 <pmu_exec_sleep_hook_funs+0x40>
6030d6c0:	e2855001 	add	r5, r5, #1
6030d6c4:	e2844014 	add	r4, r4, #20
6030d6c8:	e3550009 	cmp	r5, #9
6030d6cc:	1afffff3 	bne	6030d6a0 <pmu_exec_sleep_hook_funs+0x10>
6030d6d0:	e1a00005 	mov	r0, r5
6030d6d4:	e8bd8070 	pop	{r4, r5, r6, pc}

6030d6d8 <pmu_exec_wakeup_hook_funs>:
6030d6d8:	e92d4070 	push	{r4, r5, r6, lr}
6030d6dc:	e2506000 	subs	r6, r0, #0
6030d6e0:	08bd8070 	popeq	{r4, r5, r6, pc}
6030d6e4:	e3074f04 	movw	r4, #32516	; 0x7f04
6030d6e8:	e3464031 	movt	r4, #24625	; 0x6031
6030d6ec:	e3a05000 	mov	r5, #0
6030d6f0:	e594300c 	ldr	r3, [r4, #12]
6030d6f4:	e3a00000 	mov	r0, #0
6030d6f8:	e2855001 	add	r5, r5, #1
6030d6fc:	e1530000 	cmp	r3, r0
6030d700:	0a000001 	beq	6030d70c <pmu_exec_wakeup_hook_funs+0x34>
6030d704:	e5941010 	ldr	r1, [r4, #16]
6030d708:	e12fff33 	blx	r3
6030d70c:	e2844014 	add	r4, r4, #20
6030d710:	e1560005 	cmp	r6, r5
6030d714:	1afffff5 	bne	6030d6f0 <pmu_exec_wakeup_hook_funs+0x18>
6030d718:	e8bd8070 	pop	{r4, r5, r6, pc}

6030d71c <pmu_set_sysactive_time>:
6030d71c:	e3072fe8 	movw	r2, #32744	; 0x7fe8
6030d720:	e3462031 	movt	r2, #24625	; 0x6031
6030d724:	e92d4070 	push	{r4, r5, r6, lr}
6030d728:	e3073fec 	movw	r3, #32748	; 0x7fec
6030d72c:	e3463031 	movt	r3, #24625	; 0x6031
6030d730:	e5925000 	ldr	r5, [r2]
6030d734:	e5934000 	ldr	r4, [r3]
6030d738:	e3550000 	cmp	r5, #0
6030d73c:	1a00001b 	bne	6030d7b0 <pmu_set_sysactive_time+0x94>
6030d740:	e1500004 	cmp	r0, r4
6030d744:	21a04000 	movcs	r4, r0
6030d748:	e3540000 	cmp	r4, #0
6030d74c:	e5835000 	str	r5, [r3]
6030d750:	1a000001 	bne	6030d75c <pmu_set_sysactive_time+0x40>
6030d754:	e1a00005 	mov	r0, r5
6030d758:	e8bd8070 	pop	{r4, r5, r6, pc}
6030d75c:	ebffff91 	bl	6030d5a8 <rtos_time_get_current_system_time_ms>
6030d760:	e3700001 	cmn	r0, #1
6030d764:	1a00001c 	bne	6030d7dc <pmu_set_sysactive_time+0xc0>
6030d768:	e0804004 	add	r4, r0, r4
6030d76c:	ebffff95 	bl	6030d5c8 <rtos_time_get_current_pended_time_ms>
6030d770:	e3700001 	cmn	r0, #1
6030d774:	1a000016 	bne	6030d7d4 <pmu_set_sysactive_time+0xb8>
6030d778:	e3073fe4 	movw	r3, #32740	; 0x7fe4
6030d77c:	e3463031 	movt	r3, #24625	; 0x6031
6030d780:	e0840000 	add	r0, r4, r0
6030d784:	e5932000 	ldr	r2, [r3]
6030d788:	e1520000 	cmp	r2, r0
6030d78c:	3a00000c 	bcc	6030d7c4 <pmu_set_sysactive_time+0xa8>
6030d790:	e0421000 	sub	r1, r2, r0
6030d794:	e371010a 	cmn	r1, #-2147483646	; 0x80000002
6030d798:	8a000001 	bhi	6030d7a4 <pmu_set_sysactive_time+0x88>
6030d79c:	e1520000 	cmp	r2, r0
6030d7a0:	1affffeb 	bne	6030d754 <pmu_set_sysactive_time+0x38>
6030d7a4:	e5830000 	str	r0, [r3]
6030d7a8:	e1a00005 	mov	r0, r5
6030d7ac:	e8bd8070 	pop	{r4, r5, r6, pc}
6030d7b0:	e3a05001 	mov	r5, #1
6030d7b4:	e1500004 	cmp	r0, r4
6030d7b8:	9affffe5 	bls	6030d754 <pmu_set_sysactive_time+0x38>
6030d7bc:	e5830000 	str	r0, [r3]
6030d7c0:	eafffff8 	b	6030d7a8 <pmu_set_sysactive_time+0x8c>
6030d7c4:	e0402002 	sub	r2, r0, r2
6030d7c8:	e372010a 	cmn	r2, #-2147483646	; 0x80000002
6030d7cc:	9afffff4 	bls	6030d7a4 <pmu_set_sysactive_time+0x88>
6030d7d0:	eaffffdf 	b	6030d754 <pmu_set_sysactive_time+0x38>
6030d7d4:	ebffff7b 	bl	6030d5c8 <rtos_time_get_current_pended_time_ms>
6030d7d8:	eaffffe6 	b	6030d778 <pmu_set_sysactive_time+0x5c>
6030d7dc:	ebffff71 	bl	6030d5a8 <rtos_time_get_current_system_time_ms>
6030d7e0:	e0804004 	add	r4, r0, r4
6030d7e4:	ebffff77 	bl	6030d5c8 <rtos_time_get_current_pended_time_ms>
6030d7e8:	e3700001 	cmn	r0, #1
6030d7ec:	0affffe1 	beq	6030d778 <pmu_set_sysactive_time+0x5c>
6030d7f0:	eafffff7 	b	6030d7d4 <pmu_set_sysactive_time+0xb8>

6030d7f4 <pmu_yield_os_check>:
6030d7f4:	e30732a8 	movw	r3, #29352	; 0x72a8
6030d7f8:	e3463031 	movt	r3, #24625	; 0x6031
6030d7fc:	e5930000 	ldr	r0, [r3]
6030d800:	e12fff1e 	bx	lr

6030d804 <pmu_ready_to_sleep>:
6030d804:	e92d4010 	push	{r4, lr}
6030d808:	ebffff66 	bl	6030d5a8 <rtos_time_get_current_system_time_ms>
6030d80c:	e3073fe4 	movw	r3, #32740	; 0x7fe4
6030d810:	e3463031 	movt	r3, #24625	; 0x6031
6030d814:	e5933000 	ldr	r3, [r3]
6030d818:	e1530000 	cmp	r3, r0
6030d81c:	2a000008 	bcs	6030d844 <pmu_ready_to_sleep+0x40>
6030d820:	e0400003 	sub	r0, r0, r3
6030d824:	e370010a 	cmn	r0, #-2147483646	; 0x80000002
6030d828:	8a00000a 	bhi	6030d858 <pmu_ready_to_sleep+0x54>
6030d82c:	e30732b4 	movw	r3, #29364	; 0x72b4
6030d830:	e3463031 	movt	r3, #24625	; 0x6031
6030d834:	e5930000 	ldr	r0, [r3]
6030d838:	e16f0f10 	clz	r0, r0
6030d83c:	e1a002a0 	lsr	r0, r0, #5
6030d840:	e8bd8010 	pop	{r4, pc}
6030d844:	e0432000 	sub	r2, r3, r0
6030d848:	e372010a 	cmn	r2, #-2147483646	; 0x80000002
6030d84c:	8afffff6 	bhi	6030d82c <pmu_ready_to_sleep+0x28>
6030d850:	e1530000 	cmp	r3, r0
6030d854:	9afffff4 	bls	6030d82c <pmu_ready_to_sleep+0x28>
6030d858:	e3a00000 	mov	r0, #0
6030d85c:	e8bd8010 	pop	{r4, pc}

6030d860 <pmu_acquire_wakelock>:
6030d860:	e92d4010 	push	{r4, lr}
6030d864:	e1a04000 	mov	r4, r0
6030d868:	eb0012f1 	bl	60312434 <ulPortInterruptLock>
6030d86c:	e30732b4 	movw	r3, #29364	; 0x72b4
6030d870:	e3463031 	movt	r3, #24625	; 0x6031
6030d874:	e3a01001 	mov	r1, #1
6030d878:	e5932000 	ldr	r2, [r3]
6030d87c:	e1822411 	orr	r2, r2, r1, lsl r4
6030d880:	e5832000 	str	r2, [r3]
6030d884:	e8bd4010 	pop	{r4, lr}
6030d888:	ea0012ee 	b	60312448 <ulPortInterruptUnLock>

6030d88c <pmu_release_wakelock>:
6030d88c:	e92d4010 	push	{r4, lr}
6030d890:	e1a04000 	mov	r4, r0
6030d894:	eb0012e6 	bl	60312434 <ulPortInterruptLock>
6030d898:	e30732b4 	movw	r3, #29364	; 0x72b4
6030d89c:	e3463031 	movt	r3, #24625	; 0x6031
6030d8a0:	e3a01001 	mov	r1, #1
6030d8a4:	e5932000 	ldr	r2, [r3]
6030d8a8:	e1c22411 	bic	r2, r2, r1, lsl r4
6030d8ac:	e5832000 	str	r2, [r3]
6030d8b0:	e8bd4010 	pop	{r4, lr}
6030d8b4:	ea0012e3 	b	60312448 <ulPortInterruptUnLock>

6030d8b8 <pmu_get_wakelock_status>:
6030d8b8:	e30732b4 	movw	r3, #29364	; 0x72b4
6030d8bc:	e3463031 	movt	r3, #24625	; 0x6031
6030d8c0:	e5930000 	ldr	r0, [r3]
6030d8c4:	e12fff1e 	bx	lr

6030d8c8 <pmu_set_sleep_type>:
6030d8c8:	e3073fe0 	movw	r3, #32736	; 0x7fe0
6030d8cc:	e3463031 	movt	r3, #24625	; 0x6031
6030d8d0:	e1a02000 	mov	r2, r0
6030d8d4:	e3a00000 	mov	r0, #0
6030d8d8:	e5832000 	str	r2, [r3]
6030d8dc:	e12fff1e 	bx	lr

6030d8e0 <pmu_get_sleep_type>:
6030d8e0:	e3073fe0 	movw	r3, #32736	; 0x7fe0
6030d8e4:	e3463031 	movt	r3, #24625	; 0x6031
6030d8e8:	e5930000 	ldr	r0, [r3]
6030d8ec:	e12fff1e 	bx	lr

6030d8f0 <pmu_get_sleep_time>:
6030d8f0:	e92d4070 	push	{r4, r5, r6, lr}
6030d8f4:	e30742ac 	movw	r4, #29356	; 0x72ac
6030d8f8:	e3464031 	movt	r4, #24625	; 0x6031
6030d8fc:	e5940000 	ldr	r0, [r4]
6030d900:	e3500000 	cmp	r0, #0
6030d904:	0a000013 	beq	6030d958 <pmu_get_sleep_time+0x68>
6030d908:	e30752b0 	movw	r5, #29360	; 0x72b0
6030d90c:	e3465031 	movt	r5, #24625	; 0x6031
6030d910:	e5953000 	ldr	r3, [r5]
6030d914:	e1500003 	cmp	r0, r3
6030d918:	2a000003 	bcs	6030d92c <pmu_get_sleep_time+0x3c>
6030d91c:	e3e03000 	mvn	r3, #0
6030d920:	e5853000 	str	r3, [r5]
6030d924:	e5843000 	str	r3, [r4]
6030d928:	e8bd8070 	pop	{r4, r5, r6, pc}
6030d92c:	e30439e0 	movw	r3, #18912	; 0x49e0
6030d930:	e3463031 	movt	r3, #24625	; 0x6031
6030d934:	e12fff33 	blx	r3
6030d938:	e5943000 	ldr	r3, [r4]
6030d93c:	e5952000 	ldr	r2, [r5]
6030d940:	e2833001 	add	r3, r3, #1
6030d944:	e0433002 	sub	r3, r3, r2
6030d948:	e731f310 	udiv	r1, r0, r3
6030d94c:	e0600391 	mls	r0, r1, r3, r0
6030d950:	e0800002 	add	r0, r0, r2
6030d954:	e8bd8070 	pop	{r4, r5, r6, pc}
6030d958:	e30722b0 	movw	r2, #29360	; 0x72b0
6030d95c:	e3462031 	movt	r2, #24625	; 0x6031
6030d960:	e3e03000 	mvn	r3, #0
6030d964:	e1a00003 	mov	r0, r3
6030d968:	e5823000 	str	r3, [r2]
6030d96c:	e5843000 	str	r3, [r4]
6030d970:	e8bd8070 	pop	{r4, r5, r6, pc}

6030d974 <pmu_pre_sleep_processing>:
6030d974:	e92d4070 	push	{r4, r5, r6, lr}
6030d978:	e1a05000 	mov	r5, r0
6030d97c:	eb000204 	bl	6030e194 <xTaskGetTickCount>
6030d980:	e3073f00 	movw	r3, #32512	; 0x7f00
6030d984:	e3463031 	movt	r3, #24625	; 0x6031
6030d988:	e5933000 	ldr	r3, [r3]
6030d98c:	e1500003 	cmp	r0, r3
6030d990:	3a000004 	bcc	6030d9a8 <pmu_pre_sleep_processing+0x34>
6030d994:	e30732a4 	movw	r3, #29348	; 0x72a4
6030d998:	e3463031 	movt	r3, #24625	; 0x6031
6030d99c:	e5933000 	ldr	r3, [r3]
6030d9a0:	e3530000 	cmp	r3, #0
6030d9a4:	0a000023 	beq	6030da38 <pmu_pre_sleep_processing+0xc4>
6030d9a8:	ebffffd0 	bl	6030d8f0 <pmu_get_sleep_time>
6030d9ac:	e3073fc0 	movw	r3, #32704	; 0x7fc0
6030d9b0:	e3463031 	movt	r3, #24625	; 0x6031
6030d9b4:	e3a02000 	mov	r2, #0
6030d9b8:	e9830005 	stmib	r3, {r0, r2}
6030d9bc:	e3074fe0 	movw	r4, #32736	; 0x7fe0
6030d9c0:	e3464031 	movt	r4, #24625	; 0x6031
6030d9c4:	e5942000 	ldr	r2, [r4]
6030d9c8:	e5832000 	str	r2, [r3]
6030d9cc:	ee112f10 	mrc	15, 0, r2, cr1, cr0, {0}
6030d9d0:	e3120004 	tst	r2, #4
6030d9d4:	0a00000a 	beq	6030da04 <pmu_pre_sleep_processing+0x90>
6030d9d8:	e59f2078 	ldr	r2, [pc, #120]	; 6030da58 <pmu_pre_sleep_processing+0xe4>
6030d9dc:	e3c3303f 	bic	r3, r3, #63	; 0x3f
6030d9e0:	e3c2203f 	bic	r2, r2, #63	; 0x3f
6030d9e4:	e2822040 	add	r2, r2, #64	; 0x40
6030d9e8:	e1530002 	cmp	r3, r2
6030d9ec:	2a000003 	bcs	6030da00 <pmu_pre_sleep_processing+0x8c>
6030d9f0:	ee073f3e 	mcr	15, 0, r3, cr7, cr14, {1}
6030d9f4:	e2833040 	add	r3, r3, #64	; 0x40
6030d9f8:	e1520003 	cmp	r2, r3
6030d9fc:	8afffffb 	bhi	6030d9f0 <pmu_pre_sleep_processing+0x7c>
6030da00:	f57ff05f 	dmb	sy
6030da04:	e30434fc 	movw	r3, #17660	; 0x44fc
6030da08:	e3463031 	movt	r3, #24625	; 0x6031
6030da0c:	e12fff33 	blx	r3
6030da10:	e3073fe8 	movw	r3, #32744	; 0x7fe8
6030da14:	e3463031 	movt	r3, #24625	; 0x6031
6030da18:	e5941000 	ldr	r1, [r4]
6030da1c:	e3a02001 	mov	r2, #1
6030da20:	e5850000 	str	r0, [r5]
6030da24:	e1510002 	cmp	r1, r2
6030da28:	e5832000 	str	r2, [r3]
6030da2c:	0a000007 	beq	6030da50 <pmu_pre_sleep_processing+0xdc>
6030da30:	e8bd4070 	pop	{r4, r5, r6, lr}
6030da34:	eafff6c4 	b	6030b54c <SOCPS_SleepPG>
6030da38:	ebffffac 	bl	6030d8f0 <pmu_get_sleep_time>
6030da3c:	e3073fc0 	movw	r3, #32704	; 0x7fc0
6030da40:	e3463031 	movt	r3, #24625	; 0x6031
6030da44:	e3a02001 	mov	r2, #1
6030da48:	e9830005 	stmib	r3, {r0, r2}
6030da4c:	eaffffda 	b	6030d9bc <pmu_pre_sleep_processing+0x48>
6030da50:	e8bd4070 	pop	{r4, r5, r6, lr}
6030da54:	eafff6d9 	b	6030b5c0 <SOCPS_SleepCG>
6030da58:	60317fdf 	.word	0x60317fdf

6030da5c <pmu_acquire_deepwakelock>:
6030da5c:	e30732a4 	movw	r3, #29348	; 0x72a4
6030da60:	e3463031 	movt	r3, #24625	; 0x6031
6030da64:	e3a01001 	mov	r1, #1
6030da68:	e5932000 	ldr	r2, [r3]
6030da6c:	e1822011 	orr	r2, r2, r1, lsl r0
6030da70:	e5832000 	str	r2, [r3]
6030da74:	e12fff1e 	bx	lr

6030da78 <pmu_get_deepwakelock_status>:
6030da78:	e30732a4 	movw	r3, #29348	; 0x72a4
6030da7c:	e3463031 	movt	r3, #24625	; 0x6031
6030da80:	e5930000 	ldr	r0, [r3]
6030da84:	e12fff1e 	bx	lr

6030da88 <pmu_set_secondary_cpu_state>:
6030da88:	e3073ef8 	movw	r3, #32504	; 0x7ef8
6030da8c:	e3463031 	movt	r3, #24625	; 0x6031
6030da90:	e7831100 	str	r1, [r3, r0, lsl #2]
6030da94:	e12fff1e 	bx	lr

6030da98 <pmu_get_secondary_cpu_state>:
6030da98:	e3073ef8 	movw	r3, #32504	; 0x7ef8
6030da9c:	e3463031 	movt	r3, #24625	; 0x6031
6030daa0:	e7930100 	ldr	r0, [r3, r0, lsl #2]
6030daa4:	e12fff1e 	bx	lr

6030daa8 <pmu_secondary_cpu_state_is_running>:
6030daa8:	e3073ef8 	movw	r3, #32504	; 0x7ef8
6030daac:	e3463031 	movt	r3, #24625	; 0x6031
6030dab0:	e7930100 	ldr	r0, [r3, r0, lsl #2]
6030dab4:	e16f0f10 	clz	r0, r0
6030dab8:	e1a002a0 	lsr	r0, r0, #5
6030dabc:	e12fff1e 	bx	lr

6030dac0 <pmu_secondary_cpu_state_is_hotplug>:
6030dac0:	e3073ef8 	movw	r3, #32504	; 0x7ef8
6030dac4:	e3463031 	movt	r3, #24625	; 0x6031
6030dac8:	e7930100 	ldr	r0, [r3, r0, lsl #2]
6030dacc:	e2400001 	sub	r0, r0, #1
6030dad0:	e16f0f10 	clz	r0, r0
6030dad4:	e1a002a0 	lsr	r0, r0, #5
6030dad8:	e12fff1e 	bx	lr

6030dadc <fs_init_thread>:
6030dadc:	e3a00000 	mov	r0, #0
6030dae0:	eafffe98 	b	6030d548 <rtos_task_delete>

6030dae4 <app_uart_rx_pin_wake_int_handler>:
6030dae4:	e92d4010 	push	{r4, lr}
6030dae8:	e3a01000 	mov	r1, #0
6030daec:	e5900014 	ldr	r0, [r0, #20]
6030daf0:	e3033ff4 	movw	r3, #16372	; 0x3ff4
6030daf4:	e3463031 	movt	r3, #24625	; 0x6031
6030daf8:	e12fff33 	blx	r3
6030dafc:	e3010388 	movw	r0, #5000	; 0x1388
6030db00:	ebffff05 	bl	6030d71c <pmu_set_sysactive_time>
6030db04:	e3a00000 	mov	r0, #0
6030db08:	e8bd8010 	pop	{r4, pc}

6030db0c <app_pre_example>:
6030db0c:	e12fff1e 	bx	lr

6030db10 <app_example>:
6030db10:	e12fff1e 	bx	lr

6030db14 <app_filesystem_init>:
6030db14:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
6030db18:	e3a03000 	mov	r3, #0
6030db1c:	e24dd00c 	sub	sp, sp, #12
6030db20:	e3a0c005 	mov	ip, #5
6030db24:	e3a00a01 	mov	r0, #4096	; 0x1000
6030db28:	e30d2adc 	movw	r2, #56028	; 0xdadc
6030db2c:	e3462030 	movt	r2, #24624	; 0x6030
6030db30:	e88d1001 	stm	sp, {r0, ip}
6030db34:	e306157c 	movw	r1, #25980	; 0x657c
6030db38:	e3461031 	movt	r1, #24625	; 0x6031
6030db3c:	e1a00003 	mov	r0, r3
6030db40:	ebfffe72 	bl	6030d510 <rtos_task_create>
6030db44:	e28dd00c 	add	sp, sp, #12
6030db48:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

6030db4c <app_uart_rx_pin_wake_init>:
6030db4c:	e92d4030 	push	{r4, r5, lr}
6030db50:	f2c00010 	vmov.i32	d16, #0	; 0x00000000
6030db54:	e24dd024 	sub	sp, sp, #36	; 0x24
6030db58:	e3a04037 	mov	r4, #55	; 0x37
6030db5c:	e1a00004 	mov	r0, r4
6030db60:	edcd0b05 	vstr	d16, [sp, #20]
6030db64:	e3a01000 	mov	r1, #0
6030db68:	e3033ff4 	movw	r3, #16372	; 0x3ff4
6030db6c:	e3463031 	movt	r3, #24625	; 0x6031
6030db70:	e3a02002 	mov	r2, #2
6030db74:	e3a05001 	mov	r5, #1
6030db78:	e58d2008 	str	r2, [sp, #8]
6030db7c:	e58d200c 	str	r2, [sp, #12]
6030db80:	e58d5010 	str	r5, [sp, #16]
6030db84:	e58d401c 	str	r4, [sp, #28]
6030db88:	e12fff33 	blx	r3
6030db8c:	e59d001c 	ldr	r0, [sp, #28]
6030db90:	e3a01000 	mov	r1, #0
6030db94:	e3043188 	movw	r3, #16776	; 0x4188
6030db98:	e3463031 	movt	r3, #24625	; 0x6031
6030db9c:	e12fff33 	blx	r3
6030dba0:	e5dd100c 	ldrb	r1, [sp, #12]
6030dba4:	e5dd001c 	ldrb	r0, [sp, #28]
6030dba8:	e30432ac 	movw	r3, #17068	; 0x42ac
6030dbac:	e3463031 	movt	r3, #24625	; 0x6031
6030dbb0:	e12fff33 	blx	r3
6030dbb4:	e59d0018 	ldr	r0, [sp, #24]
6030dbb8:	e1cd21d0 	ldrd	r2, [sp, #16]
6030dbbc:	e1a01005 	mov	r1, r5
6030dbc0:	e58d0000 	str	r0, [sp]
6030dbc4:	e3035f4c 	movw	r5, #16204	; 0x3f4c
6030dbc8:	e3465031 	movt	r5, #24625	; 0x6031
6030dbcc:	e59d001c 	ldr	r0, [sp, #28]
6030dbd0:	e12fff35 	blx	r5
6030dbd4:	e3a0100a 	mov	r1, #10
6030dbd8:	e3a03003 	mov	r3, #3
6030dbdc:	e3a02b35 	mov	r2, #54272	; 0xd400
6030dbe0:	e3442200 	movt	r2, #16896	; 0x4200
6030dbe4:	e304002c 	movw	r0, #16428	; 0x402c
6030dbe8:	e3460031 	movt	r0, #24625	; 0x6031
6030dbec:	e30a5dd8 	movw	r5, #44504	; 0xadd8
6030dbf0:	e3465030 	movt	r5, #24624	; 0x6030
6030dbf4:	e12fff35 	blx	r5
6030dbf8:	e3a0000a 	mov	r0, #10
6030dbfc:	e30a3dd0 	movw	r3, #44496	; 0xadd0
6030dc00:	e3463030 	movt	r3, #24624	; 0x6030
6030dc04:	e12fff33 	blx	r3
6030dc08:	e28d2008 	add	r2, sp, #8
6030dc0c:	e1a00004 	mov	r0, r4
6030dc10:	e30d1ae4 	movw	r1, #56036	; 0xdae4
6030dc14:	e3461030 	movt	r1, #24624	; 0x6030
6030dc18:	e30431bc 	movw	r3, #16828	; 0x41bc
6030dc1c:	e3463031 	movt	r3, #24625	; 0x6031
6030dc20:	e12fff33 	blx	r3
6030dc24:	e28dd024 	add	sp, sp, #36	; 0x24
6030dc28:	e8bd8030 	pop	{r4, r5, pc}

6030dc2c <app_pmu_init>:
6030dc2c:	e92d4010 	push	{r4, lr}
6030dc30:	e3a00000 	mov	r0, #0
6030dc34:	ebffff23 	bl	6030d8c8 <pmu_set_sleep_type>
6030dc38:	e3a00000 	mov	r0, #0
6030dc3c:	ebffff86 	bl	6030da5c <pmu_acquire_deepwakelock>
6030dc40:	e8bd4010 	pop	{r4, lr}
6030dc44:	eaffffc0 	b	6030db4c <app_uart_rx_pin_wake_init>

6030dc48 <main>:
6030dc48:	e92d4010 	push	{r4, lr}
6030dc4c:	e3a02057 	mov	r2, #87	; 0x57
6030dc50:	e3a00003 	mov	r0, #3
6030dc54:	e306358c 	movw	r3, #25996	; 0x658c
6030dc58:	e3463031 	movt	r3, #24625	; 0x6031
6030dc5c:	e30615c8 	movw	r1, #26056	; 0x65c8
6030dc60:	e3461031 	movt	r1, #24625	; 0x6031
6030dc64:	ebfffbff 	bl	6030cc68 <rtk_log_write>
6030dc68:	e3a02045 	mov	r2, #69	; 0x45
6030dc6c:	e3a00002 	mov	r0, #2
6030dc70:	e30635d0 	movw	r3, #26064	; 0x65d0
6030dc74:	e3463031 	movt	r3, #24625	; 0x6031
6030dc78:	e30615c8 	movw	r1, #26056	; 0x65c8
6030dc7c:	e3461031 	movt	r1, #24625	; 0x6031
6030dc80:	ebfffbf8 	bl	6030cc68 <rtk_log_write>
6030dc84:	e3a02045 	mov	r2, #69	; 0x45
6030dc88:	e3a00002 	mov	r0, #2
6030dc8c:	e3063628 	movw	r3, #26152	; 0x6628
6030dc90:	e3463031 	movt	r3, #24625	; 0x6031
6030dc94:	e30615c8 	movw	r1, #26056	; 0x65c8
6030dc98:	e3461031 	movt	r1, #24625	; 0x6031
6030dc9c:	ebfffbf1 	bl	6030cc68 <rtk_log_write>
6030dca0:	e3a02045 	mov	r2, #69	; 0x45
6030dca4:	e3a00002 	mov	r0, #2
6030dca8:	e3063668 	movw	r3, #26216	; 0x6668
6030dcac:	e3463031 	movt	r3, #24625	; 0x6031
6030dcb0:	e30615c8 	movw	r1, #26056	; 0x65c8
6030dcb4:	e3461031 	movt	r1, #24625	; 0x6031
6030dcb8:	ebfffbea 	bl	6030cc68 <rtk_log_write>
6030dcbc:	e3a02045 	mov	r2, #69	; 0x45
6030dcc0:	e3a00002 	mov	r0, #2
6030dcc4:	e30636c4 	movw	r3, #26308	; 0x66c4
6030dcc8:	e3463031 	movt	r3, #24625	; 0x6031
6030dccc:	e30615c8 	movw	r1, #26056	; 0x65c8
6030dcd0:	e3461031 	movt	r1, #24625	; 0x6031
6030dcd4:	ebfffbe3 	bl	6030cc68 <rtk_log_write>
6030dcd8:	e3a02045 	mov	r2, #69	; 0x45
6030dcdc:	e3a00002 	mov	r0, #2
6030dce0:	e3063734 	movw	r3, #26420	; 0x6734
6030dce4:	e3463031 	movt	r3, #24625	; 0x6031
6030dce8:	e30615c8 	movw	r1, #26056	; 0x65c8
6030dcec:	e3461031 	movt	r1, #24625	; 0x6031
6030dcf0:	ebfffbdc 	bl	6030cc68 <rtk_log_write>
6030dcf4:	e3a02045 	mov	r2, #69	; 0x45
6030dcf8:	e3a00002 	mov	r0, #2
6030dcfc:	e30637a0 	movw	r3, #26528	; 0x67a0
6030dd00:	e3463031 	movt	r3, #24625	; 0x6031
6030dd04:	e30615c8 	movw	r1, #26056	; 0x65c8
6030dd08:	e3461031 	movt	r1, #24625	; 0x6031
6030dd0c:	ebfffbd5 	bl	6030cc68 <rtk_log_write>
6030dd10:	e3a02057 	mov	r2, #87	; 0x57
6030dd14:	e3a00003 	mov	r0, #3
6030dd18:	e30637f8 	movw	r3, #26616	; 0x67f8
6030dd1c:	e3463031 	movt	r3, #24625	; 0x6031
6030dd20:	e30615c8 	movw	r1, #26056	; 0x65c8
6030dd24:	e3461031 	movt	r1, #24625	; 0x6031
6030dd28:	ebfffbce 	bl	6030cc68 <rtk_log_write>
6030dd2c:	e30a4dd8 	movw	r4, #44504	; 0xadd8
6030dd30:	e3464030 	movt	r4, #24624	; 0x6030
6030dd34:	e3a01018 	mov	r1, #24
6030dd38:	e3a030a0 	mov	r3, #160	; 0xa0
6030dd3c:	e3a02d16 	mov	r2, #1408	; 0x580
6030dd40:	e3442100 	movt	r2, #16640	; 0x4100
6030dd44:	e3080dbc 	movw	r0, #36284	; 0x8dbc
6030dd48:	e3460030 	movt	r0, #24624	; 0x6030
6030dd4c:	e12fff34 	blx	r4
6030dd50:	e3a00018 	mov	r0, #24
6030dd54:	e30a3dd0 	movw	r3, #44496	; 0xadd0
6030dd58:	e3463030 	movt	r3, #24624	; 0x6030
6030dd5c:	e12fff33 	blx	r3
6030dd60:	e3a00d16 	mov	r0, #1408	; 0x580
6030dd64:	e3440100 	movt	r0, #16640	; 0x4100
6030dd68:	ebffea89 	bl	60308794 <ipc_table_init>
6030dd6c:	ebffffae 	bl	6030dc2c <app_pmu_init>
6030dd70:	ebffff67 	bl	6030db14 <app_filesystem_init>
6030dd74:	ebffff64 	bl	6030db0c <app_pre_example>
6030dd78:	e3a01000 	mov	r1, #0
6030dd7c:	e1a00001 	mov	r0, r1
6030dd80:	e3043400 	movw	r3, #17408	; 0x4400
6030dd84:	e3463031 	movt	r3, #24625	; 0x6031
6030dd88:	e12fff33 	blx	r3
6030dd8c:	ebffea56 	bl	603086ec <shell_init_ram>
6030dd90:	e3a01c02 	mov	r1, #512	; 0x200
6030dd94:	e3a00b02 	mov	r0, #2048	; 0x800
6030dd98:	ebfffe3a 	bl	6030d688 <rtk_diag_init>
6030dd9c:	ebffff5b 	bl	6030db10 <app_example>
6030dda0:	e30d1170 	movw	r1, #53616	; 0xd170
6030dda4:	e3461030 	movt	r1, #24624	; 0x6030
6030dda8:	e30d0098 	movw	r0, #53400	; 0xd098
6030ddac:	e3460030 	movt	r0, #24624	; 0x6030
6030ddb0:	ebffeb7f 	bl	60308bb4 <IPC_patch_function>
6030ddb4:	e30d055c 	movw	r0, #54620	; 0xd55c
6030ddb8:	e3460030 	movt	r0, #24624	; 0x6030
6030ddbc:	ebffebf0 	bl	60308d84 <IPC_SEMDelayStub>
6030ddc0:	e3a02049 	mov	r2, #73	; 0x49
6030ddc4:	e3a00004 	mov	r0, #4
6030ddc8:	e3063834 	movw	r3, #26676	; 0x6834
6030ddcc:	e3463031 	movt	r3, #24625	; 0x6031
6030ddd0:	e30615c8 	movw	r1, #26056	; 0x65c8
6030ddd4:	e3461031 	movt	r1, #24625	; 0x6031
6030ddd8:	ebfffba2 	bl	6030cc68 <rtk_log_write>
6030dddc:	ebfffdbc 	bl	6030d4d4 <rtos_sched_start>
6030dde0:	eafffffe 	b	6030dde0 <main+0x198>

6030dde4 <prvDeleteTCB>:
6030dde4:	e92d4010 	push	{r4, lr}
6030dde8:	e1a04000 	mov	r4, r0
6030ddec:	eb0012a0 	bl	60312874 <vPortCleanUpTCB>
6030ddf0:	e284007c 	add	r0, r4, #124	; 0x7c
6030ddf4:	ebffcd19 	bl	60301260 <_reclaim_reent>
6030ddf8:	e5d43171 	ldrb	r3, [r4, #369]	; 0x171
6030ddfc:	e3530000 	cmp	r3, #0
6030de00:	0a000008 	beq	6030de28 <prvDeleteTCB+0x44>
6030de04:	e3530001 	cmp	r3, #1
6030de08:	0a00000b 	beq	6030de3c <prvDeleteTCB+0x58>
6030de0c:	e3530002 	cmp	r3, #2
6030de10:	08bd8010 	popeq	{r4, pc}
6030de14:	e3060850 	movw	r0, #26704	; 0x6850
6030de18:	e3460031 	movt	r0, #24625	; 0x6031
6030de1c:	e8bd4010 	pop	{r4, lr}
6030de20:	e30110f7 	movw	r1, #4343	; 0x10f7
6030de24:	eaffef80 	b	60309c2c <vAssertCalled>
6030de28:	e5940030 	ldr	r0, [r4, #48]	; 0x30
6030de2c:	eb000fa5 	bl	60311cc8 <vPortFree>
6030de30:	e1a00004 	mov	r0, r4
6030de34:	e8bd4010 	pop	{r4, lr}
6030de38:	ea000fa2 	b	60311cc8 <vPortFree>
6030de3c:	e1a00004 	mov	r0, r4
6030de40:	e8bd4010 	pop	{r4, lr}
6030de44:	ea000f9f 	b	60311cc8 <vPortFree>

6030de48 <prvYieldCore>:
6030de48:	e92d4010 	push	{r4, lr}
6030de4c:	e1a04000 	mov	r4, r0
6030de50:	eb001192 	bl	603124a0 <ulPortGetCoreId>
6030de54:	e1500004 	cmp	r0, r4
6030de58:	1a00000a 	bne	6030de88 <prvYieldCore+0x40>
6030de5c:	e3073ff0 	movw	r3, #32752	; 0x7ff0
6030de60:	e3463031 	movt	r3, #24625	; 0x6031
6030de64:	e7933100 	ldr	r3, [r3, r0, lsl #2]
6030de68:	e5933058 	ldr	r3, [r3, #88]	; 0x58
6030de6c:	e3530000 	cmp	r3, #0
6030de70:	0a000007 	beq	6030de94 <prvYieldCore+0x4c>
6030de74:	e3083198 	movw	r3, #33176	; 0x8198
6030de78:	e3463031 	movt	r3, #24625	; 0x6031
6030de7c:	e3a02001 	mov	r2, #1
6030de80:	e7832104 	str	r2, [r3, r4, lsl #2]
6030de84:	e8bd8010 	pop	{r4, pc}
6030de88:	e1a00004 	mov	r0, r4
6030de8c:	e8bd4010 	pop	{r4, lr}
6030de90:	ea001190 	b	603124d8 <vPortYieldOtherCore>
6030de94:	eb00116d 	bl	60312450 <xPortCpuIsInInterrupt>
6030de98:	e3500000 	cmp	r0, #0
6030de9c:	1afffff4 	bne	6030de74 <prvYieldCore+0x2c>
6030dea0:	ef000000 	svc	0x00000000
6030dea4:	e8bd8010 	pop	{r4, pc}

6030dea8 <prvInitialiseNewTask.constprop.0>:
6030dea8:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
6030deac:	e59d4028 	ldr	r4, [sp, #40]	; 0x28
6030deb0:	e1a09102 	lsl	r9, r2, #2
6030deb4:	e1a0a001 	mov	sl, r1
6030deb8:	e1a02009 	mov	r2, r9
6030debc:	e1a06000 	mov	r6, r0
6030dec0:	e3a010a5 	mov	r1, #165	; 0xa5
6030dec4:	e5940030 	ldr	r0, [r4, #48]	; 0x30
6030dec8:	e59d5020 	ldr	r5, [sp, #32]
6030decc:	e59d8024 	ldr	r8, [sp, #36]	; 0x24
6030ded0:	e1a07003 	mov	r7, r3
6030ded4:	eb001bc3 	bl	60314de8 <__wrap_memset>
6030ded8:	e5942030 	ldr	r2, [r4, #48]	; 0x30
6030dedc:	e2499004 	sub	r9, r9, #4
6030dee0:	e0829009 	add	r9, r2, r9
6030dee4:	e35a0000 	cmp	sl, #0
6030dee8:	e3c9903f 	bic	r9, r9, #63	; 0x3f
6030deec:	05c4a03c 	strbeq	sl, [r4, #60]	; 0x3c
6030def0:	e5849054 	str	r9, [r4, #84]	; 0x54
6030def4:	0a00000b 	beq	6030df28 <prvInitialiseNewTask.constprop.0+0x80>
6030def8:	e24ac001 	sub	ip, sl, #1
6030defc:	e28a3017 	add	r3, sl, #23
6030df00:	e284103b 	add	r1, r4, #59	; 0x3b
6030df04:	ea000001 	b	6030df10 <prvInitialiseNewTask.constprop.0+0x68>
6030df08:	e15c0003 	cmp	ip, r3
6030df0c:	0a000003 	beq	6030df20 <prvInitialiseNewTask.constprop.0+0x78>
6030df10:	e5fce001 	ldrb	lr, [ip, #1]!
6030df14:	e5e1e001 	strb	lr, [r1, #1]!
6030df18:	e35e0000 	cmp	lr, #0
6030df1c:	1afffff9 	bne	6030df08 <prvInitialiseNewTask.constprop.0+0x60>
6030df20:	e3a03000 	mov	r3, #0
6030df24:	e5c43053 	strb	r3, [r4, #83]	; 0x53
6030df28:	e355000a 	cmp	r5, #10
6030df2c:	e3a0a000 	mov	sl, #0
6030df30:	23a0500a 	movcs	r5, #10
6030df34:	e2840004 	add	r0, r4, #4
6030df38:	e584502c 	str	r5, [r4, #44]	; 0x2c
6030df3c:	e5845064 	str	r5, [r4, #100]	; 0x64
6030df40:	e584a068 	str	sl, [r4, #104]	; 0x68
6030df44:	eb000979 	bl	60310530 <vListInitialiseItem>
6030df48:	e2840018 	add	r0, r4, #24
6030df4c:	eb000977 	bl	60310530 <vListInitialiseItem>
6030df50:	e265500b 	rsb	r5, r5, #11
6030df54:	e3a020f0 	mov	r2, #240	; 0xf0
6030df58:	e1a0100a 	mov	r1, sl
6030df5c:	e584a16c 	str	sl, [r4, #364]	; 0x16c
6030df60:	e284007c 	add	r0, r4, #124	; 0x7c
6030df64:	e5845018 	str	r5, [r4, #24]
6030df68:	e5844010 	str	r4, [r4, #16]
6030df6c:	e5844024 	str	r4, [r4, #36]	; 0x24
6030df70:	e584a058 	str	sl, [r4, #88]	; 0x58
6030df74:	e584a06c 	str	sl, [r4, #108]	; 0x6c
6030df78:	e584a078 	str	sl, [r4, #120]	; 0x78
6030df7c:	e584a070 	str	sl, [r4, #112]	; 0x70
6030df80:	e584a074 	str	sl, [r4, #116]	; 0x74
6030df84:	e5c4a170 	strb	sl, [r4, #368]	; 0x170
6030df88:	eb001b96 	bl	60314de8 <__wrap_memset>
6030df8c:	e307331c 	movw	r3, #29468	; 0x731c
6030df90:	e3463031 	movt	r3, #24625	; 0x6031
6030df94:	e2831068 	add	r1, r3, #104	; 0x68
6030df98:	e5843080 	str	r3, [r4, #128]	; 0x80
6030df9c:	e5841084 	str	r1, [r4, #132]	; 0x84
6030dfa0:	e28330d0 	add	r3, r3, #208	; 0xd0
6030dfa4:	e1a01006 	mov	r1, r6
6030dfa8:	e5843088 	str	r3, [r4, #136]	; 0x88
6030dfac:	e1a02007 	mov	r2, r7
6030dfb0:	e1a00009 	mov	r0, r9
6030dfb4:	e5c4a172 	strb	sl, [r4, #370]	; 0x172
6030dfb8:	eb0010a7 	bl	6031225c <pxPortInitialiseStack>
6030dfbc:	e3003288 	movw	r3, #648	; 0x288
6030dfc0:	e3463031 	movt	r3, #24625	; 0x6031
6030dfc4:	e0466003 	sub	r6, r6, r3
6030dfc8:	e16f6f16 	clz	r6, r6
6030dfcc:	e158000a 	cmp	r8, sl
6030dfd0:	e1a062a6 	lsr	r6, r6, #5
6030dfd4:	e3e03000 	mvn	r3, #0
6030dfd8:	e5840000 	str	r0, [r4]
6030dfdc:	e5843034 	str	r3, [r4, #52]	; 0x34
6030dfe0:	e5846038 	str	r6, [r4, #56]	; 0x38
6030dfe4:	15884000 	strne	r4, [r8]
6030dfe8:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}

6030dfec <prvYieldForTask.isra.0>:
6030dfec:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
6030dff0:	e1a06000 	mov	r6, r0
6030dff4:	e1a05002 	mov	r5, r2
6030dff8:	e1a04001 	mov	r4, r1
6030dffc:	eb001127 	bl	603124a0 <ulPortGetCoreId>
6030e000:	e596602c 	ldr	r6, [r6, #44]	; 0x2c
6030e004:	e3550000 	cmp	r5, #0
6030e008:	e2241001 	eor	r1, r4, #1
6030e00c:	e3a0c000 	mov	ip, #0
6030e010:	e3072ff0 	movw	r2, #32752	; 0x7ff0
6030e014:	e3462031 	movt	r2, #24625	; 0x6031
6030e018:	e3085198 	movw	r5, #33176	; 0x8198
6030e01c:	e3465031 	movt	r5, #24625	; 0x6031
6030e020:	12866001 	addne	r6, r6, #1
6030e024:	e2011001 	and	r1, r1, #1
6030e028:	e1a0300c 	mov	r3, ip
6030e02c:	e1a0e000 	mov	lr, r0
6030e030:	e3e00000 	mvn	r0, #0
6030e034:	e15e0003 	cmp	lr, r3
6030e038:	13a04000 	movne	r4, #0
6030e03c:	02014001 	andeq	r4, r1, #1
6030e040:	e3540000 	cmp	r4, #0
6030e044:	1a000013 	bne	6030e098 <prvYieldForTask.isra.0+0xac>
6030e048:	e7924103 	ldr	r4, [r2, r3, lsl #2]
6030e04c:	e5944038 	ldr	r4, [r4, #56]	; 0x38
6030e050:	e3540000 	cmp	r4, #0
6030e054:	1a00001a 	bne	6030e0c4 <prvYieldForTask.isra.0+0xd8>
6030e058:	e7927103 	ldr	r7, [r2, r3, lsl #2]
6030e05c:	e7924103 	ldr	r4, [r2, r3, lsl #2]
6030e060:	e5944034 	ldr	r4, [r4, #52]	; 0x34
6030e064:	e3540001 	cmp	r4, #1
6030e068:	8a00000a 	bhi	6030e098 <prvYieldForTask.isra.0+0xac>
6030e06c:	e597402c 	ldr	r4, [r7, #44]	; 0x2c
6030e070:	e7958103 	ldr	r8, [r5, r3, lsl #2]
6030e074:	e1540006 	cmp	r4, r6
6030e078:	a3a07000 	movge	r7, #0
6030e07c:	b3a07001 	movlt	r7, #1
6030e080:	e3580000 	cmp	r8, #0
6030e084:	13a07000 	movne	r7, #0
6030e088:	e3570000 	cmp	r7, #0
6030e08c:	128cc001 	addne	ip, ip, #1
6030e090:	11a06004 	movne	r6, r4
6030e094:	11a00003 	movne	r0, r3
6030e098:	e3530001 	cmp	r3, #1
6030e09c:	e3a03001 	mov	r3, #1
6030e0a0:	1affffe3 	bne	6030e034 <prvYieldForTask.isra.0+0x48>
6030e0a4:	e25cc000 	subs	ip, ip, #0
6030e0a8:	13a0c001 	movne	ip, #1
6030e0ac:	e3500001 	cmp	r0, #1
6030e0b0:	83a0c000 	movhi	ip, #0
6030e0b4:	e35c0000 	cmp	ip, #0
6030e0b8:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
6030e0bc:	e8bd41f0 	pop	{r4, r5, r6, r7, r8, lr}
6030e0c0:	eaffff60 	b	6030de48 <prvYieldCore>
6030e0c4:	e28cc001 	add	ip, ip, #1
6030e0c8:	e1a00003 	mov	r0, r3
6030e0cc:	eafffff4 	b	6030e0a4 <prvYieldForTask.isra.0+0xb8>

6030e0d0 <prvGetExpectedIdleTime>:
6030e0d0:	e3083100 	movw	r3, #33024	; 0x8100
6030e0d4:	e3463031 	movt	r3, #24625	; 0x6031
6030e0d8:	e92d4070 	push	{r4, r5, r6, lr}
6030e0dc:	e5935000 	ldr	r5, [r3]
6030e0e0:	eb0010d3 	bl	60312434 <ulPortInterruptLock>
6030e0e4:	e1a04000 	mov	r4, r0
6030e0e8:	eb0010ec 	bl	603124a0 <ulPortGetCoreId>
6030e0ec:	e3073ff0 	movw	r3, #32752	; 0x7ff0
6030e0f0:	e3463031 	movt	r3, #24625	; 0x6031
6030e0f4:	e1a02000 	mov	r2, r0
6030e0f8:	e1a00004 	mov	r0, r4
6030e0fc:	e7934102 	ldr	r4, [r3, r2, lsl #2]
6030e100:	eb0010d0 	bl	60312448 <ulPortInterruptUnLock>
6030e104:	e594302c 	ldr	r3, [r4, #44]	; 0x2c
6030e108:	e3530000 	cmp	r3, #0
6030e10c:	0a000001 	beq	6030e118 <prvGetExpectedIdleTime+0x48>
6030e110:	e3a00000 	mov	r0, #0
6030e114:	e8bd8070 	pop	{r4, r5, r6, pc}
6030e118:	e3a00001 	mov	r0, #1
6030e11c:	ebfffe61 	bl	6030daa8 <pmu_secondary_cpu_state_is_running>
6030e120:	e3500000 	cmp	r0, #0
6030e124:	0a000004 	beq	6030e13c <prvGetExpectedIdleTime+0x6c>
6030e128:	e3083004 	movw	r3, #32772	; 0x8004
6030e12c:	e3463031 	movt	r3, #24625	; 0x6031
6030e130:	e5933000 	ldr	r3, [r3]
6030e134:	e3530000 	cmp	r3, #0
6030e138:	1afffff4 	bne	6030e110 <prvGetExpectedIdleTime+0x40>
6030e13c:	e3550000 	cmp	r5, #0
6030e140:	1afffff2 	bne	6030e110 <prvGetExpectedIdleTime+0x40>
6030e144:	e3082134 	movw	r2, #33076	; 0x8134
6030e148:	e3462031 	movt	r2, #24625	; 0x6031
6030e14c:	e3083194 	movw	r3, #33172	; 0x8194
6030e150:	e3463031 	movt	r3, #24625	; 0x6031
6030e154:	e5920000 	ldr	r0, [r2]
6030e158:	e5933000 	ldr	r3, [r3]
6030e15c:	e0400003 	sub	r0, r0, r3
6030e160:	e8bd8070 	pop	{r4, r5, r6, pc}

6030e164 <xTaskGetCurrentYieldPending>:
6030e164:	e92d4010 	push	{r4, lr}
6030e168:	eb0010b1 	bl	60312434 <ulPortInterruptLock>
6030e16c:	e1a04000 	mov	r4, r0
6030e170:	eb0010ca 	bl	603124a0 <ulPortGetCoreId>
6030e174:	e3083198 	movw	r3, #33176	; 0x8198
6030e178:	e3463031 	movt	r3, #24625	; 0x6031
6030e17c:	e1a02000 	mov	r2, r0
6030e180:	e1a00004 	mov	r0, r4
6030e184:	e7934102 	ldr	r4, [r3, r2, lsl #2]
6030e188:	eb0010ae 	bl	60312448 <ulPortInterruptUnLock>
6030e18c:	e1a00004 	mov	r0, r4
6030e190:	e8bd8010 	pop	{r4, pc}

6030e194 <xTaskGetTickCount>:
6030e194:	e3083194 	movw	r3, #33172	; 0x8194
6030e198:	e3463031 	movt	r3, #24625	; 0x6031
6030e19c:	e5930000 	ldr	r0, [r3]
6030e1a0:	e12fff1e 	bx	lr

6030e1a4 <xTaskGetPendedTicks>:
6030e1a4:	e308313c 	movw	r3, #33084	; 0x813c
6030e1a8:	e3463031 	movt	r3, #24625	; 0x6031
6030e1ac:	e5930000 	ldr	r0, [r3]
6030e1b0:	e12fff1e 	bx	lr

6030e1b4 <xTaskGetTickCountFromISR>:
6030e1b4:	e3083194 	movw	r3, #33172	; 0x8194
6030e1b8:	e3463031 	movt	r3, #24625	; 0x6031
6030e1bc:	e5930000 	ldr	r0, [r3]
6030e1c0:	e12fff1e 	bx	lr

6030e1c4 <xTaskGetPendedTicksFromISR>:
6030e1c4:	e308313c 	movw	r3, #33084	; 0x813c
6030e1c8:	e3463031 	movt	r3, #24625	; 0x6031
6030e1cc:	e5930000 	ldr	r0, [r3]
6030e1d0:	e12fff1e 	bx	lr

6030e1d4 <pcTaskGetName>:
6030e1d4:	e92d4010 	push	{r4, lr}
6030e1d8:	e2504000 	subs	r4, r0, #0
6030e1dc:	0a000001 	beq	6030e1e8 <pcTaskGetName+0x14>
6030e1e0:	e284003c 	add	r0, r4, #60	; 0x3c
6030e1e4:	e8bd8010 	pop	{r4, pc}
6030e1e8:	eb001091 	bl	60312434 <ulPortInterruptLock>
6030e1ec:	e1a04000 	mov	r4, r0
6030e1f0:	eb0010aa 	bl	603124a0 <ulPortGetCoreId>
6030e1f4:	e3073ff0 	movw	r3, #32752	; 0x7ff0
6030e1f8:	e3463031 	movt	r3, #24625	; 0x6031
6030e1fc:	e1a02000 	mov	r2, r0
6030e200:	e1a00004 	mov	r0, r4
6030e204:	e7934102 	ldr	r4, [r3, r2, lsl #2]
6030e208:	eb00108e 	bl	60312448 <ulPortInterruptUnLock>
6030e20c:	e3540000 	cmp	r4, #0
6030e210:	1afffff2 	bne	6030e1e0 <pcTaskGetName+0xc>
6030e214:	e3001a54 	movw	r1, #2644	; 0xa54
6030e218:	e3060850 	movw	r0, #26704	; 0x6850
6030e21c:	e3460031 	movt	r0, #24625	; 0x6031
6030e220:	ebffee81 	bl	60309c2c <vAssertCalled>
6030e224:	e284003c 	add	r0, r4, #60	; 0x3c
6030e228:	e8bd8010 	pop	{r4, pc}

6030e22c <vTaskCompTick>:
6030e22c:	e3500001 	cmp	r0, #1
6030e230:	9a000012 	bls	6030e280 <vTaskCompTick+0x54>
6030e234:	e3082194 	movw	r2, #33172	; 0x8194
6030e238:	e3462031 	movt	r2, #24625	; 0x6031
6030e23c:	e5923000 	ldr	r3, [r2]
6030e240:	e5921000 	ldr	r1, [r2]
6030e244:	e0833000 	add	r3, r3, r0
6030e248:	e1530001 	cmp	r3, r1
6030e24c:	e5923000 	ldr	r3, [r2]
6030e250:	9a000008 	bls	6030e278 <vTaskCompTick+0x4c>
6030e254:	e2433001 	sub	r3, r3, #1
6030e258:	e308113c 	movw	r1, #33084	; 0x813c
6030e25c:	e3461031 	movt	r1, #24625	; 0x6031
6030e260:	e0830000 	add	r0, r3, r0
6030e264:	e5820000 	str	r0, [r2]
6030e268:	e5913000 	ldr	r3, [r1]
6030e26c:	e2833001 	add	r3, r3, #1
6030e270:	e5813000 	str	r3, [r1]
6030e274:	e12fff1e 	bx	lr
6030e278:	e3730001 	cmn	r3, #1
6030e27c:	1a000005 	bne	6030e298 <vTaskCompTick+0x6c>
6030e280:	e308313c 	movw	r3, #33084	; 0x813c
6030e284:	e3463031 	movt	r3, #24625	; 0x6031
6030e288:	e5932000 	ldr	r2, [r3]
6030e28c:	e0820000 	add	r0, r2, r0
6030e290:	e5830000 	str	r0, [r3]
6030e294:	e12fff1e 	bx	lr
6030e298:	e308113c 	movw	r1, #33084	; 0x813c
6030e29c:	e3461031 	movt	r1, #24625	; 0x6031
6030e2a0:	e5923000 	ldr	r3, [r2]
6030e2a4:	e591c000 	ldr	ip, [r1]
6030e2a8:	e083300c 	add	r3, r3, ip
6030e2ac:	e2833002 	add	r3, r3, #2
6030e2b0:	e0830000 	add	r0, r3, r0
6030e2b4:	e3e03001 	mvn	r3, #1
6030e2b8:	e5810000 	str	r0, [r1]
6030e2bc:	e5823000 	str	r3, [r2]
6030e2c0:	e12fff1e 	bx	lr

6030e2c4 <vTaskSwitchContext>:
6030e2c4:	e92d4ff8 	push	{r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
6030e2c8:	e1a04000 	mov	r4, r0
6030e2cc:	eb001058 	bl	60312434 <ulPortInterruptLock>
6030e2d0:	e1a08000 	mov	r8, r0
6030e2d4:	eb001030 	bl	6031239c <xPortSpinLockTask>
6030e2d8:	e1a00004 	mov	r0, r4
6030e2dc:	ebfffdf7 	bl	6030dac0 <pmu_secondary_cpu_state_is_hotplug>
6030e2e0:	e3500000 	cmp	r0, #0
6030e2e4:	1a000114 	bne	6030e73c <vTaskSwitchContext+0x478>
6030e2e8:	e30830f8 	movw	r3, #33016	; 0x80f8
6030e2ec:	e3463031 	movt	r3, #24625	; 0x6031
6030e2f0:	e5936000 	ldr	r6, [r3]
6030e2f4:	e3560000 	cmp	r6, #0
6030e2f8:	1a00009a 	bne	6030e568 <vTaskSwitchContext+0x2a4>
6030e2fc:	e3075ff0 	movw	r5, #32752	; 0x7ff0
6030e300:	e3465031 	movt	r5, #24625	; 0x6031
6030e304:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e308:	e5933058 	ldr	r3, [r3, #88]	; 0x58
6030e30c:	e3530000 	cmp	r3, #0
6030e310:	1a000187 	bne	6030e934 <vTaskSwitchContext+0x670>
6030e314:	e3083198 	movw	r3, #33176	; 0x8198
6030e318:	e3463031 	movt	r3, #24625	; 0x6031
6030e31c:	e3a02000 	mov	r2, #0
6030e320:	e3a00a09 	mov	r0, #36864	; 0x9000
6030e324:	e3440200 	movt	r0, #16896	; 0x4200
6030e328:	e7832104 	str	r2, [r3, r4, lsl #2]
6030e32c:	e30830e8 	movw	r3, #33000	; 0x80e8
6030e330:	e3463031 	movt	r3, #24625	; 0x6031
6030e334:	e30820e0 	movw	r2, #32992	; 0x80e0
6030e338:	e3462031 	movt	r2, #24625	; 0x6031
6030e33c:	e5901004 	ldr	r1, [r0, #4]
6030e340:	e7831106 	str	r1, [r3, r6, lsl #2]
6030e344:	e793c106 	ldr	ip, [r3, r6, lsl #2]
6030e348:	e7921106 	ldr	r1, [r2, r6, lsl #2]
6030e34c:	e15c0001 	cmp	ip, r1
6030e350:	9a000006 	bls	6030e370 <vTaskSwitchContext+0xac>
6030e354:	e793e106 	ldr	lr, [r3, r6, lsl #2]
6030e358:	e7927106 	ldr	r7, [r2, r6, lsl #2]
6030e35c:	e795c106 	ldr	ip, [r5, r6, lsl #2]
6030e360:	e59c1078 	ldr	r1, [ip, #120]	; 0x78
6030e364:	e0411007 	sub	r1, r1, r7
6030e368:	e081100e 	add	r1, r1, lr
6030e36c:	e58c1078 	str	r1, [ip, #120]	; 0x78
6030e370:	e7931106 	ldr	r1, [r3, r6, lsl #2]
6030e374:	e3560001 	cmp	r6, #1
6030e378:	e7821106 	str	r1, [r2, r6, lsl #2]
6030e37c:	e3a06001 	mov	r6, #1
6030e380:	1affffed 	bne	6030e33c <vTaskSwitchContext+0x78>
6030e384:	eb00102a 	bl	60312434 <ulPortInterruptLock>
6030e388:	e1a06000 	mov	r6, r0
6030e38c:	eb001043 	bl	603124a0 <ulPortGetCoreId>
6030e390:	e1a03000 	mov	r3, r0
6030e394:	e1a00006 	mov	r0, r6
6030e398:	e7956103 	ldr	r6, [r5, r3, lsl #2]
6030e39c:	eb001029 	bl	60312448 <ulPortInterruptUnLock>
6030e3a0:	e30a35a5 	movw	r3, #42405	; 0xa5a5
6030e3a4:	e34a35a5 	movt	r3, #42405	; 0xa5a5
6030e3a8:	e5962030 	ldr	r2, [r6, #48]	; 0x30
6030e3ac:	e5921000 	ldr	r1, [r2]
6030e3b0:	e1510003 	cmp	r1, r3
6030e3b4:	1a000002 	bne	6030e3c4 <vTaskSwitchContext+0x100>
6030e3b8:	e5923004 	ldr	r3, [r2, #4]
6030e3bc:	e1530001 	cmp	r3, r1
6030e3c0:	0a000128 	beq	6030e868 <vTaskSwitchContext+0x5a4>
6030e3c4:	eb00101a 	bl	60312434 <ulPortInterruptLock>
6030e3c8:	e1a06000 	mov	r6, r0
6030e3cc:	eb001033 	bl	603124a0 <ulPortGetCoreId>
6030e3d0:	e1a03000 	mov	r3, r0
6030e3d4:	e1a00006 	mov	r0, r6
6030e3d8:	e7957103 	ldr	r7, [r5, r3, lsl #2]
6030e3dc:	eb001019 	bl	60312448 <ulPortInterruptUnLock>
6030e3e0:	eb001013 	bl	60312434 <ulPortInterruptLock>
6030e3e4:	e1a06000 	mov	r6, r0
6030e3e8:	eb00102c 	bl	603124a0 <ulPortGetCoreId>
6030e3ec:	e1a03000 	mov	r3, r0
6030e3f0:	e1a00006 	mov	r0, r6
6030e3f4:	e7956103 	ldr	r6, [r5, r3, lsl #2]
6030e3f8:	eb001012 	bl	60312448 <ulPortInterruptUnLock>
6030e3fc:	e286103c 	add	r1, r6, #60	; 0x3c
6030e400:	e1a00007 	mov	r0, r7
6030e404:	eb001034 	bl	603124dc <vApplicationStackOverflowHook>
6030e408:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e40c:	e5933034 	ldr	r3, [r3, #52]	; 0x34
6030e410:	e3530000 	cmp	r3, #0
6030e414:	ba000003 	blt	6030e428 <vTaskSwitchContext+0x164>
6030e418:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e41c:	e5933034 	ldr	r3, [r3, #52]	; 0x34
6030e420:	e3530001 	cmp	r3, #1
6030e424:	da0000e9 	ble	6030e7d0 <vTaskSwitchContext+0x50c>
6030e428:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e42c:	e3086000 	movw	r6, #32768	; 0x8000
6030e430:	e3466031 	movt	r6, #24625	; 0x6031
6030e434:	e5932014 	ldr	r2, [r3, #20]
6030e438:	e5963000 	ldr	r3, [r6]
6030e43c:	e1520003 	cmp	r2, r3
6030e440:	0a00010f 	beq	6030e884 <vTaskSwitchContext+0x5c0>
6030e444:	e3089100 	movw	r9, #33024	; 0x8100
6030e448:	e3469031 	movt	r9, #24625	; 0x6031
6030e44c:	e3087004 	movw	r7, #32772	; 0x8004
6030e450:	e3467031 	movt	r7, #24625	; 0x6031
6030e454:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e458:	e3e02000 	mvn	r2, #0
6030e45c:	e3a0a014 	mov	sl, #20
6030e460:	e5832034 	str	r2, [r3, #52]	; 0x34
6030e464:	e5996000 	ldr	r6, [r9]
6030e468:	e003069a 	mul	r3, sl, r6
6030e46c:	e7973003 	ldr	r3, [r7, r3]
6030e470:	e3530000 	cmp	r3, #0
6030e474:	1a000008 	bne	6030e49c <vTaskSwitchContext+0x1d8>
6030e478:	e306b850 	movw	fp, #26704	; 0x6850
6030e47c:	e346b031 	movt	fp, #24625	; 0x6031
6030e480:	e3560000 	cmp	r6, #0
6030e484:	e2466001 	sub	r6, r6, #1
6030e488:	0a0000a7 	beq	6030e72c <vTaskSwitchContext+0x468>
6030e48c:	e003069a 	mul	r3, sl, r6
6030e490:	e7973003 	ldr	r3, [r7, r3]
6030e494:	e3530000 	cmp	r3, #0
6030e498:	0afffff8 	beq	6030e480 <vTaskSwitchContext+0x1bc>
6030e49c:	e3a01014 	mov	r1, #20
6030e4a0:	e308c12c 	movw	ip, #33068	; 0x812c
6030e4a4:	e346c031 	movt	ip, #24625	; 0x6031
6030e4a8:	e0010691 	mul	r1, r1, r6
6030e4ac:	e0870001 	add	r0, r7, r1
6030e4b0:	e2811008 	add	r1, r1, #8
6030e4b4:	e0871001 	add	r1, r7, r1
6030e4b8:	e5903004 	ldr	r3, [r0, #4]
6030e4bc:	ea000003 	b	6030e4d0 <vTaskSwitchContext+0x20c>
6030e4c0:	e795e104 	ldr	lr, [r5, r4, lsl #2]
6030e4c4:	e79c2104 	ldr	r2, [ip, r4, lsl #2]
6030e4c8:	e15e0002 	cmp	lr, r2
6030e4cc:	0a00000a 	beq	6030e4fc <vTaskSwitchContext+0x238>
6030e4d0:	e5933004 	ldr	r3, [r3, #4]
6030e4d4:	e5803004 	str	r3, [r0, #4]
6030e4d8:	e1530001 	cmp	r3, r1
6030e4dc:	05933004 	ldreq	r3, [r3, #4]
6030e4e0:	05803004 	streq	r3, [r0, #4]
6030e4e4:	e593200c 	ldr	r2, [r3, #12]
6030e4e8:	e7852104 	str	r2, [r5, r4, lsl #2]
6030e4ec:	e7952104 	ldr	r2, [r5, r4, lsl #2]
6030e4f0:	e5922038 	ldr	r2, [r2, #56]	; 0x38
6030e4f4:	e3520000 	cmp	r2, #0
6030e4f8:	1afffff0 	bne	6030e4c0 <vTaskSwitchContext+0x1fc>
6030e4fc:	e5896000 	str	r6, [r9]
6030e500:	e7950104 	ldr	r0, [r5, r4, lsl #2]
6030e504:	e2800004 	add	r0, r0, #4
6030e508:	eb00082c 	bl	603105c0 <uxListRemove>
6030e50c:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e510:	e5834034 	str	r4, [r3, #52]	; 0x34
6030e514:	e5993000 	ldr	r3, [r9]
6030e518:	e3530000 	cmp	r3, #0
6030e51c:	0a000007 	beq	6030e540 <vTaskSwitchContext+0x27c>
6030e520:	e3a01014 	mov	r1, #20
6030e524:	ea000001 	b	6030e530 <vTaskSwitchContext+0x26c>
6030e528:	e2533001 	subs	r3, r3, #1
6030e52c:	0a000003 	beq	6030e540 <vTaskSwitchContext+0x27c>
6030e530:	e0020391 	mul	r2, r1, r3
6030e534:	e7972002 	ldr	r2, [r7, r2]
6030e538:	e3520000 	cmp	r2, #0
6030e53c:	0afffff9 	beq	6030e528 <vTaskSwitchContext+0x264>
6030e540:	e5893000 	str	r3, [r9]
6030e544:	e3073000 	movw	r3, #28672	; 0x7000
6030e548:	e3463031 	movt	r3, #24625	; 0x6031
6030e54c:	e7952104 	ldr	r2, [r5, r4, lsl #2]
6030e550:	e282207c 	add	r2, r2, #124	; 0x7c
6030e554:	e5832000 	str	r2, [r3]
6030e558:	eb000f9a 	bl	603123c8 <xPortSpinUnLockTask>
6030e55c:	e1a00008 	mov	r0, r8
6030e560:	e8bd4ff8 	pop	{r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
6030e564:	ea000fb7 	b	60312448 <ulPortInterruptUnLock>
6030e568:	e3083198 	movw	r3, #33176	; 0x8198
6030e56c:	e3463031 	movt	r3, #24625	; 0x6031
6030e570:	e3a02001 	mov	r2, #1
6030e574:	e7832104 	str	r2, [r3, r4, lsl #2]
6030e578:	eb000fb4 	bl	60312450 <xPortCpuIsInInterrupt>
6030e57c:	e3500000 	cmp	r0, #0
6030e580:	1afffff4 	bne	6030e558 <vTaskSwitchContext+0x294>
6030e584:	e3075ff0 	movw	r5, #32752	; 0x7ff0
6030e588:	e3465031 	movt	r5, #24625	; 0x6031
6030e58c:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e590:	e5931038 	ldr	r1, [r3, #56]	; 0x38
6030e594:	e3510000 	cmp	r1, #0
6030e598:	1affffee 	bne	6030e558 <vTaskSwitchContext+0x294>
6030e59c:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e5a0:	e5933034 	ldr	r3, [r3, #52]	; 0x34
6030e5a4:	e3530000 	cmp	r3, #0
6030e5a8:	ba000003 	blt	6030e5bc <vTaskSwitchContext+0x2f8>
6030e5ac:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e5b0:	e5933034 	ldr	r3, [r3, #52]	; 0x34
6030e5b4:	e3530001 	cmp	r3, #1
6030e5b8:	daffffe6 	ble	6030e558 <vTaskSwitchContext+0x294>
6030e5bc:	e3a0ca09 	mov	ip, #36864	; 0x9000
6030e5c0:	e344c200 	movt	ip, #16896	; 0x4200
6030e5c4:	e30830e8 	movw	r3, #33000	; 0x80e8
6030e5c8:	e3463031 	movt	r3, #24625	; 0x6031
6030e5cc:	e30820e0 	movw	r2, #32992	; 0x80e0
6030e5d0:	e3462031 	movt	r2, #24625	; 0x6031
6030e5d4:	e59c0004 	ldr	r0, [ip, #4]
6030e5d8:	e7830101 	str	r0, [r3, r1, lsl #2]
6030e5dc:	e793e101 	ldr	lr, [r3, r1, lsl #2]
6030e5e0:	e7920101 	ldr	r0, [r2, r1, lsl #2]
6030e5e4:	e15e0000 	cmp	lr, r0
6030e5e8:	9a000006 	bls	6030e608 <vTaskSwitchContext+0x344>
6030e5ec:	e7936101 	ldr	r6, [r3, r1, lsl #2]
6030e5f0:	e7927101 	ldr	r7, [r2, r1, lsl #2]
6030e5f4:	e795e101 	ldr	lr, [r5, r1, lsl #2]
6030e5f8:	e59e0078 	ldr	r0, [lr, #120]	; 0x78
6030e5fc:	e0400007 	sub	r0, r0, r7
6030e600:	e0800006 	add	r0, r0, r6
6030e604:	e58e0078 	str	r0, [lr, #120]	; 0x78
6030e608:	e7930101 	ldr	r0, [r3, r1, lsl #2]
6030e60c:	e3510001 	cmp	r1, #1
6030e610:	e7820101 	str	r0, [r2, r1, lsl #2]
6030e614:	e3a01001 	mov	r1, #1
6030e618:	1affffed 	bne	6030e5d4 <vTaskSwitchContext+0x310>
6030e61c:	eb000f84 	bl	60312434 <ulPortInterruptLock>
6030e620:	e1a06000 	mov	r6, r0
6030e624:	eb000f9d 	bl	603124a0 <ulPortGetCoreId>
6030e628:	e1a03000 	mov	r3, r0
6030e62c:	e1a00006 	mov	r0, r6
6030e630:	e7956103 	ldr	r6, [r5, r3, lsl #2]
6030e634:	eb000f83 	bl	60312448 <ulPortInterruptUnLock>
6030e638:	e30a35a5 	movw	r3, #42405	; 0xa5a5
6030e63c:	e34a35a5 	movt	r3, #42405	; 0xa5a5
6030e640:	e5962030 	ldr	r2, [r6, #48]	; 0x30
6030e644:	e5921000 	ldr	r1, [r2]
6030e648:	e1510003 	cmp	r1, r3
6030e64c:	1a000002 	bne	6030e65c <vTaskSwitchContext+0x398>
6030e650:	e5923004 	ldr	r3, [r2, #4]
6030e654:	e1530001 	cmp	r3, r1
6030e658:	0a0000dd 	beq	6030e9d4 <vTaskSwitchContext+0x710>
6030e65c:	eb000f74 	bl	60312434 <ulPortInterruptLock>
6030e660:	e1a06000 	mov	r6, r0
6030e664:	eb000f8d 	bl	603124a0 <ulPortGetCoreId>
6030e668:	e1a03000 	mov	r3, r0
6030e66c:	e1a00006 	mov	r0, r6
6030e670:	e7957103 	ldr	r7, [r5, r3, lsl #2]
6030e674:	eb000f73 	bl	60312448 <ulPortInterruptUnLock>
6030e678:	eb000f6d 	bl	60312434 <ulPortInterruptLock>
6030e67c:	e1a06000 	mov	r6, r0
6030e680:	eb000f86 	bl	603124a0 <ulPortGetCoreId>
6030e684:	e1a03000 	mov	r3, r0
6030e688:	e1a00006 	mov	r0, r6
6030e68c:	e7956103 	ldr	r6, [r5, r3, lsl #2]
6030e690:	eb000f6c 	bl	60312448 <ulPortInterruptUnLock>
6030e694:	e286103c 	add	r1, r6, #60	; 0x3c
6030e698:	e1a00007 	mov	r0, r7
6030e69c:	eb000f8e 	bl	603124dc <vApplicationStackOverflowHook>
6030e6a0:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e6a4:	e3e02000 	mvn	r2, #0
6030e6a8:	e3086000 	movw	r6, #32768	; 0x8000
6030e6ac:	e3466031 	movt	r6, #24625	; 0x6031
6030e6b0:	e5832034 	str	r2, [r3, #52]	; 0x34
6030e6b4:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e6b8:	e5932014 	ldr	r2, [r3, #20]
6030e6bc:	e5963000 	ldr	r3, [r6]
6030e6c0:	e1520003 	cmp	r2, r3
6030e6c4:	0a0000a8 	beq	6030e96c <vTaskSwitchContext+0x6a8>
6030e6c8:	e3089100 	movw	r9, #33024	; 0x8100
6030e6cc:	e3469031 	movt	r9, #24625	; 0x6031
6030e6d0:	e308312c 	movw	r3, #33068	; 0x812c
6030e6d4:	e3463031 	movt	r3, #24625	; 0x6031
6030e6d8:	e7933104 	ldr	r3, [r3, r4, lsl #2]
6030e6dc:	e7853104 	str	r3, [r5, r4, lsl #2]
6030e6e0:	e7950104 	ldr	r0, [r5, r4, lsl #2]
6030e6e4:	e2800004 	add	r0, r0, #4
6030e6e8:	eb0007b4 	bl	603105c0 <uxListRemove>
6030e6ec:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e6f0:	e5834034 	str	r4, [r3, #52]	; 0x34
6030e6f4:	e5993000 	ldr	r3, [r9]
6030e6f8:	e3530000 	cmp	r3, #0
6030e6fc:	0affff8f 	beq	6030e540 <vTaskSwitchContext+0x27c>
6030e700:	e3087004 	movw	r7, #32772	; 0x8004
6030e704:	e3467031 	movt	r7, #24625	; 0x6031
6030e708:	e3a01014 	mov	r1, #20
6030e70c:	ea000001 	b	6030e718 <vTaskSwitchContext+0x454>
6030e710:	e2533001 	subs	r3, r3, #1
6030e714:	0affff89 	beq	6030e540 <vTaskSwitchContext+0x27c>
6030e718:	e0020391 	mul	r2, r1, r3
6030e71c:	e7972002 	ldr	r2, [r7, r2]
6030e720:	e3520000 	cmp	r2, #0
6030e724:	0afffff9 	beq	6030e710 <vTaskSwitchContext+0x44c>
6030e728:	eaffff84 	b	6030e540 <vTaskSwitchContext+0x27c>
6030e72c:	e1a0000b 	mov	r0, fp
6030e730:	e3001d88 	movw	r1, #3464	; 0xd88
6030e734:	ebffed3c 	bl	60309c2c <vAssertCalled>
6030e738:	eaffff53 	b	6030e48c <vTaskSwitchContext+0x1c8>
6030e73c:	e3075ff0 	movw	r5, #32752	; 0x7ff0
6030e740:	e3465031 	movt	r5, #24625	; 0x6031
6030e744:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e748:	e5933034 	ldr	r3, [r3, #52]	; 0x34
6030e74c:	e3530000 	cmp	r3, #0
6030e750:	ba000003 	blt	6030e764 <vTaskSwitchContext+0x4a0>
6030e754:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e758:	e5933034 	ldr	r3, [r3, #52]	; 0x34
6030e75c:	e3530001 	cmp	r3, #1
6030e760:	da000029 	ble	6030e80c <vTaskSwitchContext+0x548>
6030e764:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e768:	e3086000 	movw	r6, #32768	; 0x8000
6030e76c:	e3466031 	movt	r6, #24625	; 0x6031
6030e770:	e5932014 	ldr	r2, [r3, #20]
6030e774:	e5963000 	ldr	r3, [r6]
6030e778:	e1520003 	cmp	r2, r3
6030e77c:	0a00005d 	beq	6030e8f8 <vTaskSwitchContext+0x634>
6030e780:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e784:	e3e02000 	mvn	r2, #0
6030e788:	e3a06000 	mov	r6, #0
6030e78c:	e5832034 	str	r2, [r3, #52]	; 0x34
6030e790:	e308212c 	movw	r2, #33068	; 0x812c
6030e794:	e3462031 	movt	r2, #24625	; 0x6031
6030e798:	e3083198 	movw	r3, #33176	; 0x8198
6030e79c:	e3463031 	movt	r3, #24625	; 0x6031
6030e7a0:	e7922104 	ldr	r2, [r2, r4, lsl #2]
6030e7a4:	e7852104 	str	r2, [r5, r4, lsl #2]
6030e7a8:	e7836104 	str	r6, [r3, r4, lsl #2]
6030e7ac:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e7b0:	e5836058 	str	r6, [r3, #88]	; 0x58
6030e7b4:	ebfff0dc 	bl	6030ab2c <vDisableIPIInterrupt>
6030e7b8:	eb000f02 	bl	603123c8 <xPortSpinUnLockTask>
6030e7bc:	e1a00008 	mov	r0, r8
6030e7c0:	eb000f20 	bl	60312448 <ulPortInterruptUnLock>
6030e7c4:	e1a00006 	mov	r0, r6
6030e7c8:	e8bd4ff8 	pop	{r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
6030e7cc:	eafff265 	b	6030b168 <psci_cpu_off>
6030e7d0:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e7d4:	e5933034 	ldr	r3, [r3, #52]	; 0x34
6030e7d8:	e1530004 	cmp	r3, r4
6030e7dc:	1affff11 	bne	6030e428 <vTaskSwitchContext+0x164>
6030e7e0:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e7e4:	e5933034 	ldr	r3, [r3, #52]	; 0x34
6030e7e8:	e3730002 	cmn	r3, #2
6030e7ec:	1a00002e 	bne	6030e8ac <vTaskSwitchContext+0x5e8>
6030e7f0:	e3083000 	movw	r3, #32768	; 0x8000
6030e7f4:	e3463031 	movt	r3, #24625	; 0x6031
6030e7f8:	e5930000 	ldr	r0, [r3]
6030e7fc:	e7951104 	ldr	r1, [r5, r4, lsl #2]
6030e800:	e2811004 	add	r1, r1, #4
6030e804:	eb00074c 	bl	6031053c <vListInsertEnd>
6030e808:	eaffff0d 	b	6030e444 <vTaskSwitchContext+0x180>
6030e80c:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e810:	e5933034 	ldr	r3, [r3, #52]	; 0x34
6030e814:	e3730002 	cmn	r3, #2
6030e818:	0a00004a 	beq	6030e948 <vTaskSwitchContext+0x684>
6030e81c:	e7952104 	ldr	r2, [r5, r4, lsl #2]
6030e820:	e3083100 	movw	r3, #33024	; 0x8100
6030e824:	e3463031 	movt	r3, #24625	; 0x6031
6030e828:	e592102c 	ldr	r1, [r2, #44]	; 0x2c
6030e82c:	e3a00014 	mov	r0, #20
6030e830:	e5932000 	ldr	r2, [r3]
6030e834:	e1510002 	cmp	r1, r2
6030e838:	87952104 	ldrhi	r2, [r5, r4, lsl #2]
6030e83c:	8592202c 	ldrhi	r2, [r2, #44]	; 0x2c
6030e840:	85832000 	strhi	r2, [r3]
6030e844:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e848:	e7951104 	ldr	r1, [r5, r4, lsl #2]
6030e84c:	e593202c 	ldr	r2, [r3, #44]	; 0x2c
6030e850:	e3083004 	movw	r3, #32772	; 0x8004
6030e854:	e3463031 	movt	r3, #24625	; 0x6031
6030e858:	e2811004 	add	r1, r1, #4
6030e85c:	e0203290 	mla	r0, r0, r2, r3
6030e860:	eb000735 	bl	6031053c <vListInsertEnd>
6030e864:	eaffffc5 	b	6030e780 <vTaskSwitchContext+0x4bc>
6030e868:	e5921008 	ldr	r1, [r2, #8]
6030e86c:	e1510003 	cmp	r1, r3
6030e870:	1afffed3 	bne	6030e3c4 <vTaskSwitchContext+0x100>
6030e874:	e592300c 	ldr	r3, [r2, #12]
6030e878:	e1530001 	cmp	r3, r1
6030e87c:	1afffed0 	bne	6030e3c4 <vTaskSwitchContext+0x100>
6030e880:	eafffee0 	b	6030e408 <vTaskSwitchContext+0x144>
6030e884:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e888:	e3e02000 	mvn	r2, #0
6030e88c:	e5832034 	str	r2, [r3, #52]	; 0x34
6030e890:	e7950104 	ldr	r0, [r5, r4, lsl #2]
6030e894:	e2800004 	add	r0, r0, #4
6030e898:	eb000748 	bl	603105c0 <uxListRemove>
6030e89c:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e8a0:	e5933034 	ldr	r3, [r3, #52]	; 0x34
6030e8a4:	e3730002 	cmn	r3, #2
6030e8a8:	0a00002d 	beq	6030e964 <vTaskSwitchContext+0x6a0>
6030e8ac:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e8b0:	e3089100 	movw	r9, #33024	; 0x8100
6030e8b4:	e3469031 	movt	r9, #24625	; 0x6031
6030e8b8:	e593202c 	ldr	r2, [r3, #44]	; 0x2c
6030e8bc:	e3a00014 	mov	r0, #20
6030e8c0:	e5993000 	ldr	r3, [r9]
6030e8c4:	e3087004 	movw	r7, #32772	; 0x8004
6030e8c8:	e3467031 	movt	r7, #24625	; 0x6031
6030e8cc:	e1520003 	cmp	r2, r3
6030e8d0:	87953104 	ldrhi	r3, [r5, r4, lsl #2]
6030e8d4:	8593302c 	ldrhi	r3, [r3, #44]	; 0x2c
6030e8d8:	85893000 	strhi	r3, [r9]
6030e8dc:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e8e0:	e7951104 	ldr	r1, [r5, r4, lsl #2]
6030e8e4:	e593302c 	ldr	r3, [r3, #44]	; 0x2c
6030e8e8:	e2811004 	add	r1, r1, #4
6030e8ec:	e0207390 	mla	r0, r0, r3, r7
6030e8f0:	eb000711 	bl	6031053c <vListInsertEnd>
6030e8f4:	eafffed6 	b	6030e454 <vTaskSwitchContext+0x190>
6030e8f8:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e8fc:	e3e02000 	mvn	r2, #0
6030e900:	e5832034 	str	r2, [r3, #52]	; 0x34
6030e904:	e7950104 	ldr	r0, [r5, r4, lsl #2]
6030e908:	e2800004 	add	r0, r0, #4
6030e90c:	eb00072b 	bl	603105c0 <uxListRemove>
6030e910:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e914:	e5933034 	ldr	r3, [r3, #52]	; 0x34
6030e918:	e3730002 	cmn	r3, #2
6030e91c:	1affffbe 	bne	6030e81c <vTaskSwitchContext+0x558>
6030e920:	e5960000 	ldr	r0, [r6]
6030e924:	e7951104 	ldr	r1, [r5, r4, lsl #2]
6030e928:	e2811004 	add	r1, r1, #4
6030e92c:	eb000702 	bl	6031053c <vListInsertEnd>
6030e930:	eaffff92 	b	6030e780 <vTaskSwitchContext+0x4bc>
6030e934:	e3001d57 	movw	r1, #3415	; 0xd57
6030e938:	e3060850 	movw	r0, #26704	; 0x6850
6030e93c:	e3460031 	movt	r0, #24625	; 0x6031
6030e940:	ebffecb9 	bl	60309c2c <vAssertCalled>
6030e944:	eafffe72 	b	6030e314 <vTaskSwitchContext+0x50>
6030e948:	e3083000 	movw	r3, #32768	; 0x8000
6030e94c:	e3463031 	movt	r3, #24625	; 0x6031
6030e950:	e5930000 	ldr	r0, [r3]
6030e954:	e7951104 	ldr	r1, [r5, r4, lsl #2]
6030e958:	e2811004 	add	r1, r1, #4
6030e95c:	eb0006f6 	bl	6031053c <vListInsertEnd>
6030e960:	eaffff86 	b	6030e780 <vTaskSwitchContext+0x4bc>
6030e964:	e5960000 	ldr	r0, [r6]
6030e968:	eaffffa3 	b	6030e7fc <vTaskSwitchContext+0x538>
6030e96c:	e7950104 	ldr	r0, [r5, r4, lsl #2]
6030e970:	e2800004 	add	r0, r0, #4
6030e974:	eb000711 	bl	603105c0 <uxListRemove>
6030e978:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e97c:	e5933034 	ldr	r3, [r3, #52]	; 0x34
6030e980:	e3730002 	cmn	r3, #2
6030e984:	0a000019 	beq	6030e9f0 <vTaskSwitchContext+0x72c>
6030e988:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e98c:	e3089100 	movw	r9, #33024	; 0x8100
6030e990:	e3469031 	movt	r9, #24625	; 0x6031
6030e994:	e593202c 	ldr	r2, [r3, #44]	; 0x2c
6030e998:	e3a00014 	mov	r0, #20
6030e99c:	e5993000 	ldr	r3, [r9]
6030e9a0:	e3087004 	movw	r7, #32772	; 0x8004
6030e9a4:	e3467031 	movt	r7, #24625	; 0x6031
6030e9a8:	e1520003 	cmp	r2, r3
6030e9ac:	87953104 	ldrhi	r3, [r5, r4, lsl #2]
6030e9b0:	8593302c 	ldrhi	r3, [r3, #44]	; 0x2c
6030e9b4:	85893000 	strhi	r3, [r9]
6030e9b8:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030e9bc:	e7951104 	ldr	r1, [r5, r4, lsl #2]
6030e9c0:	e593302c 	ldr	r3, [r3, #44]	; 0x2c
6030e9c4:	e2811004 	add	r1, r1, #4
6030e9c8:	e0207390 	mla	r0, r0, r3, r7
6030e9cc:	eb0006da 	bl	6031053c <vListInsertEnd>
6030e9d0:	eaffff3e 	b	6030e6d0 <vTaskSwitchContext+0x40c>
6030e9d4:	e5921008 	ldr	r1, [r2, #8]
6030e9d8:	e1510003 	cmp	r1, r3
6030e9dc:	1affff1e 	bne	6030e65c <vTaskSwitchContext+0x398>
6030e9e0:	e592300c 	ldr	r3, [r2, #12]
6030e9e4:	e1530001 	cmp	r3, r1
6030e9e8:	1affff1b 	bne	6030e65c <vTaskSwitchContext+0x398>
6030e9ec:	eaffff2b 	b	6030e6a0 <vTaskSwitchContext+0x3dc>
6030e9f0:	e5960000 	ldr	r0, [r6]
6030e9f4:	e7951104 	ldr	r1, [r5, r4, lsl #2]
6030e9f8:	e2811004 	add	r1, r1, #4
6030e9fc:	eb0006ce 	bl	6031053c <vListInsertEnd>
6030ea00:	eaffff30 	b	6030e6c8 <vTaskSwitchContext+0x404>

6030ea04 <vTaskInternalSetTimeOutState>:
6030ea04:	e3082138 	movw	r2, #33080	; 0x8138
6030ea08:	e3462031 	movt	r2, #24625	; 0x6031
6030ea0c:	e3083194 	movw	r3, #33172	; 0x8194
6030ea10:	e3463031 	movt	r3, #24625	; 0x6031
6030ea14:	e5922000 	ldr	r2, [r2]
6030ea18:	e5933000 	ldr	r3, [r3]
6030ea1c:	e1c020f0 	strd	r2, [r0]
6030ea20:	e12fff1e 	bx	lr

6030ea24 <vTaskMissedYield>:
6030ea24:	e92d4010 	push	{r4, lr}
6030ea28:	eb000e9c 	bl	603124a0 <ulPortGetCoreId>
6030ea2c:	e3083198 	movw	r3, #33176	; 0x8198
6030ea30:	e3463031 	movt	r3, #24625	; 0x6031
6030ea34:	e3a02001 	mov	r2, #1
6030ea38:	e7832100 	str	r2, [r3, r0, lsl #2]
6030ea3c:	e8bd8010 	pop	{r4, pc}

6030ea40 <eTaskConfirmSleepModeStatus>:
6030ea40:	e3083140 	movw	r3, #33088	; 0x8140
6030ea44:	e3463031 	movt	r3, #24625	; 0x6031
6030ea48:	e5933000 	ldr	r3, [r3]
6030ea4c:	e3530000 	cmp	r3, #0
6030ea50:	0a000001 	beq	6030ea5c <eTaskConfirmSleepModeStatus+0x1c>
6030ea54:	e3a00000 	mov	r0, #0
6030ea58:	e12fff1e 	bx	lr
6030ea5c:	e92d4010 	push	{r4, lr}
6030ea60:	eb000e73 	bl	60312434 <ulPortInterruptLock>
6030ea64:	e1a04000 	mov	r4, r0
6030ea68:	eb000e8c 	bl	603124a0 <ulPortGetCoreId>
6030ea6c:	e3083198 	movw	r3, #33176	; 0x8198
6030ea70:	e3463031 	movt	r3, #24625	; 0x6031
6030ea74:	e1a02000 	mov	r2, r0
6030ea78:	e1a00004 	mov	r0, r4
6030ea7c:	e7934102 	ldr	r4, [r3, r2, lsl #2]
6030ea80:	eb000e70 	bl	60312448 <ulPortInterruptUnLock>
6030ea84:	e3540000 	cmp	r4, #0
6030ea88:	0a000001 	beq	6030ea94 <eTaskConfirmSleepModeStatus+0x54>
6030ea8c:	e3a00000 	mov	r0, #0
6030ea90:	e8bd8010 	pop	{r4, pc}
6030ea94:	e30830f0 	movw	r3, #33008	; 0x80f0
6030ea98:	e3463031 	movt	r3, #24625	; 0x6031
6030ea9c:	e3082158 	movw	r2, #33112	; 0x8158
6030eaa0:	e3462031 	movt	r2, #24625	; 0x6031
6030eaa4:	e5933000 	ldr	r3, [r3]
6030eaa8:	e5920000 	ldr	r0, [r2]
6030eaac:	e2433001 	sub	r3, r3, #1
6030eab0:	e1500003 	cmp	r0, r3
6030eab4:	13a00001 	movne	r0, #1
6030eab8:	03a00002 	moveq	r0, #2
6030eabc:	e8bd8010 	pop	{r4, pc}

6030eac0 <xTaskGetSchedulerState>:
6030eac0:	e3083154 	movw	r3, #33108	; 0x8154
6030eac4:	e3463031 	movt	r3, #24625	; 0x6031
6030eac8:	e92d4070 	push	{r4, r5, r6, lr}
6030eacc:	e5933000 	ldr	r3, [r3]
6030ead0:	e3530000 	cmp	r3, #0
6030ead4:	03a04001 	moveq	r4, #1
6030ead8:	1a000001 	bne	6030eae4 <xTaskGetSchedulerState+0x24>
6030eadc:	e1a00004 	mov	r0, r4
6030eae0:	e8bd8070 	pop	{r4, r5, r6, pc}
6030eae4:	eb000e52 	bl	60312434 <ulPortInterruptLock>
6030eae8:	e1a05000 	mov	r5, r0
6030eaec:	eb000e2a 	bl	6031239c <xPortSpinLockTask>
6030eaf0:	e30830f8 	movw	r3, #33016	; 0x80f8
6030eaf4:	e3463031 	movt	r3, #24625	; 0x6031
6030eaf8:	e5933000 	ldr	r3, [r3]
6030eafc:	e3530000 	cmp	r3, #0
6030eb00:	03a04002 	moveq	r4, #2
6030eb04:	13a04000 	movne	r4, #0
6030eb08:	eb000e2e 	bl	603123c8 <xPortSpinUnLockTask>
6030eb0c:	e1a00005 	mov	r0, r5
6030eb10:	eb000e4c 	bl	60312448 <ulPortInterruptUnLock>
6030eb14:	e1a00004 	mov	r0, r4
6030eb18:	e8bd8070 	pop	{r4, r5, r6, pc}

6030eb1c <vTaskYieldWithinAPI>:
6030eb1c:	e92d4010 	push	{r4, lr}
6030eb20:	eb000e43 	bl	60312434 <ulPortInterruptLock>
6030eb24:	e1a04000 	mov	r4, r0
6030eb28:	eb000e5c 	bl	603124a0 <ulPortGetCoreId>
6030eb2c:	e3073ff0 	movw	r3, #32752	; 0x7ff0
6030eb30:	e3463031 	movt	r3, #24625	; 0x6031
6030eb34:	e1a02000 	mov	r2, r0
6030eb38:	e1a00004 	mov	r0, r4
6030eb3c:	e7934102 	ldr	r4, [r3, r2, lsl #2]
6030eb40:	eb000e40 	bl	60312448 <ulPortInterruptUnLock>
6030eb44:	e5943058 	ldr	r3, [r4, #88]	; 0x58
6030eb48:	e3530000 	cmp	r3, #0
6030eb4c:	1a000001 	bne	6030eb58 <vTaskYieldWithinAPI+0x3c>
6030eb50:	ef000000 	svc	0x00000000
6030eb54:	e8bd8010 	pop	{r4, pc}
6030eb58:	eb000e50 	bl	603124a0 <ulPortGetCoreId>
6030eb5c:	e3083198 	movw	r3, #33176	; 0x8198
6030eb60:	e3463031 	movt	r3, #24625	; 0x6031
6030eb64:	e3a02001 	mov	r2, #1
6030eb68:	e7832100 	str	r2, [r3, r0, lsl #2]
6030eb6c:	e8bd8010 	pop	{r4, pc}

6030eb70 <vTaskEnterCritical>:
6030eb70:	e92d4070 	push	{r4, r5, r6, lr}
6030eb74:	eb000e2e 	bl	60312434 <ulPortInterruptLock>
6030eb78:	eb000e48 	bl	603124a0 <ulPortGetCoreId>
6030eb7c:	e3083154 	movw	r3, #33108	; 0x8154
6030eb80:	e3463031 	movt	r3, #24625	; 0x6031
6030eb84:	e5933000 	ldr	r3, [r3]
6030eb88:	e3530000 	cmp	r3, #0
6030eb8c:	08bd8070 	popeq	{r4, r5, r6, pc}
6030eb90:	e3075ff0 	movw	r5, #32752	; 0x7ff0
6030eb94:	e3465031 	movt	r5, #24625	; 0x6031
6030eb98:	e1a04000 	mov	r4, r0
6030eb9c:	e7953100 	ldr	r3, [r5, r0, lsl #2]
6030eba0:	e5933058 	ldr	r3, [r3, #88]	; 0x58
6030eba4:	e3530000 	cmp	r3, #0
6030eba8:	0a000004 	beq	6030ebc0 <vTaskEnterCritical+0x50>
6030ebac:	e7952104 	ldr	r2, [r5, r4, lsl #2]
6030ebb0:	e5923058 	ldr	r3, [r2, #88]	; 0x58
6030ebb4:	e2833001 	add	r3, r3, #1
6030ebb8:	e5823058 	str	r3, [r2, #88]	; 0x58
6030ebbc:	e8bd8070 	pop	{r4, r5, r6, pc}
6030ebc0:	eb000df5 	bl	6031239c <xPortSpinLockTask>
6030ebc4:	eafffff8 	b	6030ebac <vTaskEnterCritical+0x3c>

6030ebc8 <GetOSCriticalNesting>:
6030ebc8:	e3073ff0 	movw	r3, #32752	; 0x7ff0
6030ebcc:	e3463031 	movt	r3, #24625	; 0x6031
6030ebd0:	e7933100 	ldr	r3, [r3, r0, lsl #2]
6030ebd4:	e5930058 	ldr	r0, [r3, #88]	; 0x58
6030ebd8:	e12fff1e 	bx	lr

6030ebdc <vTaskExitCritical>:
6030ebdc:	e92d4070 	push	{r4, r5, r6, lr}
6030ebe0:	eb000e2e 	bl	603124a0 <ulPortGetCoreId>
6030ebe4:	e3083154 	movw	r3, #33108	; 0x8154
6030ebe8:	e3463031 	movt	r3, #24625	; 0x6031
6030ebec:	e5933000 	ldr	r3, [r3]
6030ebf0:	e3530000 	cmp	r3, #0
6030ebf4:	08bd8070 	popeq	{r4, r5, r6, pc}
6030ebf8:	e3075ff0 	movw	r5, #32752	; 0x7ff0
6030ebfc:	e3465031 	movt	r5, #24625	; 0x6031
6030ec00:	e1a04000 	mov	r4, r0
6030ec04:	e7953100 	ldr	r3, [r5, r0, lsl #2]
6030ec08:	e5933058 	ldr	r3, [r3, #88]	; 0x58
6030ec0c:	e3530000 	cmp	r3, #0
6030ec10:	0a000021 	beq	6030ec9c <vTaskExitCritical+0xc0>
6030ec14:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030ec18:	e5933058 	ldr	r3, [r3, #88]	; 0x58
6030ec1c:	e3530000 	cmp	r3, #0
6030ec20:	08bd8070 	popeq	{r4, r5, r6, pc}
6030ec24:	e7952104 	ldr	r2, [r5, r4, lsl #2]
6030ec28:	e7951104 	ldr	r1, [r5, r4, lsl #2]
6030ec2c:	e5923058 	ldr	r3, [r2, #88]	; 0x58
6030ec30:	e2433001 	sub	r3, r3, #1
6030ec34:	e5823058 	str	r3, [r2, #88]	; 0x58
6030ec38:	e5913058 	ldr	r3, [r1, #88]	; 0x58
6030ec3c:	e3530000 	cmp	r3, #0
6030ec40:	18bd8070 	popne	{r4, r5, r6, pc}
6030ec44:	eb000e01 	bl	60312450 <xPortCpuIsInInterrupt>
6030ec48:	e3500000 	cmp	r0, #0
6030ec4c:	1a000017 	bne	6030ecb0 <vTaskExitCritical+0xd4>
6030ec50:	e3083198 	movw	r3, #33176	; 0x8198
6030ec54:	e3463031 	movt	r3, #24625	; 0x6031
6030ec58:	e7933104 	ldr	r3, [r3, r4, lsl #2]
6030ec5c:	e3530000 	cmp	r3, #0
6030ec60:	0a000004 	beq	6030ec78 <vTaskExitCritical+0x9c>
6030ec64:	e30830f8 	movw	r3, #33016	; 0x80f8
6030ec68:	e3463031 	movt	r3, #24625	; 0x6031
6030ec6c:	e5933000 	ldr	r3, [r3]
6030ec70:	e3530000 	cmp	r3, #0
6030ec74:	0a00000f 	beq	6030ecb8 <vTaskExitCritical+0xdc>
6030ec78:	eb000dd2 	bl	603123c8 <xPortSpinUnLockTask>
6030ec7c:	e1a00004 	mov	r0, r4
6030ec80:	ebfff8ff 	bl	6030d084 <GetComponentCriticalNesting>
6030ec84:	e3500000 	cmp	r0, #0
6030ec88:	18bd8070 	popne	{r4, r5, r6, pc}
6030ec8c:	f1080080 	cpsie	i
6030ec90:	f57ff04f 	dsb	sy
6030ec94:	f57ff06f 	isb	sy
6030ec98:	e8bd8070 	pop	{r4, r5, r6, pc}
6030ec9c:	e3060850 	movw	r0, #26704	; 0x6850
6030eca0:	e3460031 	movt	r0, #24625	; 0x6031
6030eca4:	e3011288 	movw	r1, #4744	; 0x1288
6030eca8:	ebffebdf 	bl	60309c2c <vAssertCalled>
6030ecac:	eaffffd8 	b	6030ec14 <vTaskExitCritical+0x38>
6030ecb0:	e8bd4070 	pop	{r4, r5, r6, lr}
6030ecb4:	ea000dc3 	b	603123c8 <xPortSpinUnLockTask>
6030ecb8:	eb000dc2 	bl	603123c8 <xPortSpinUnLockTask>
6030ecbc:	ef000000 	svc	0x00000000
6030ecc0:	eaffffed 	b	6030ec7c <vTaskExitCritical+0xa0>

6030ecc4 <prvAddNewTaskToReadyList>:
6030ecc4:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
6030ecc8:	e3075ff0 	movw	r5, #32752	; 0x7ff0
6030eccc:	e3465031 	movt	r5, #24625	; 0x6031
6030ecd0:	e1a06000 	mov	r6, r0
6030ecd4:	ebffffa5 	bl	6030eb70 <vTaskEnterCritical>
6030ecd8:	e5957000 	ldr	r7, [r5]
6030ecdc:	e3570000 	cmp	r7, #0
6030ece0:	0a00001b 	beq	6030ed54 <prvAddNewTaskToReadyList+0x90>
6030ece4:	e5953004 	ldr	r3, [r5, #4]
6030ece8:	e3530000 	cmp	r3, #0
6030ecec:	0a000078 	beq	6030eed4 <prvAddNewTaskToReadyList+0x210>
6030ecf0:	e5953000 	ldr	r3, [r5]
6030ecf4:	e5952000 	ldr	r2, [r5]
6030ecf8:	e596102c 	ldr	r1, [r6, #44]	; 0x2c
6030ecfc:	e593302c 	ldr	r3, [r3, #44]	; 0x2c
6030ed00:	e5922034 	ldr	r2, [r2, #52]	; 0x34
6030ed04:	e3520001 	cmp	r2, #1
6030ed08:	83a02000 	movhi	r2, #0
6030ed0c:	93a02001 	movls	r2, #1
6030ed10:	e1510003 	cmp	r1, r3
6030ed14:	33a02000 	movcc	r2, #0
6030ed18:	e3520000 	cmp	r2, #0
6030ed1c:	e5952004 	ldr	r2, [r5, #4]
6030ed20:	01a03001 	moveq	r3, r1
6030ed24:	13a07000 	movne	r7, #0
6030ed28:	e592102c 	ldr	r1, [r2, #44]	; 0x2c
6030ed2c:	e5952004 	ldr	r2, [r5, #4]
6030ed30:	03e07000 	mvneq	r7, #0
6030ed34:	e1530001 	cmp	r3, r1
6030ed38:	e5922034 	ldr	r2, [r2, #52]	; 0x34
6030ed3c:	33a03000 	movcc	r3, #0
6030ed40:	23a03001 	movcs	r3, #1
6030ed44:	e3520001 	cmp	r2, #1
6030ed48:	83a03000 	movhi	r3, #0
6030ed4c:	e3530000 	cmp	r3, #0
6030ed50:	13a07001 	movne	r7, #1
6030ed54:	e30830f0 	movw	r3, #33008	; 0x80f0
6030ed58:	e3463031 	movt	r3, #24625	; 0x6031
6030ed5c:	e5932000 	ldr	r2, [r3]
6030ed60:	e2822001 	add	r2, r2, #1
6030ed64:	e5832000 	str	r2, [r3]
6030ed68:	e5933000 	ldr	r3, [r3]
6030ed6c:	e3530001 	cmp	r3, #1
6030ed70:	0a000030 	beq	6030ee38 <prvAddNewTaskToReadyList+0x174>
6030ed74:	e30820fc 	movw	r2, #33020	; 0x80fc
6030ed78:	e3462031 	movt	r2, #24625	; 0x6031
6030ed7c:	e2861004 	add	r1, r6, #4
6030ed80:	e5923000 	ldr	r3, [r2]
6030ed84:	e2833001 	add	r3, r3, #1
6030ed88:	e5823000 	str	r3, [r2]
6030ed8c:	e3e02000 	mvn	r2, #0
6030ed90:	e5862034 	str	r2, [r6, #52]	; 0x34
6030ed94:	e586305c 	str	r3, [r6, #92]	; 0x5c
6030ed98:	e5963034 	ldr	r3, [r6, #52]	; 0x34
6030ed9c:	e3730002 	cmn	r3, #2
6030eda0:	0a00001f 	beq	6030ee24 <prvAddNewTaskToReadyList+0x160>
6030eda4:	e3083100 	movw	r3, #33024	; 0x8100
6030eda8:	e3463031 	movt	r3, #24625	; 0x6031
6030edac:	e596202c 	ldr	r2, [r6, #44]	; 0x2c
6030edb0:	e5930000 	ldr	r0, [r3]
6030edb4:	e1520000 	cmp	r2, r0
6030edb8:	e3a00014 	mov	r0, #20
6030edbc:	85832000 	strhi	r2, [r3]
6030edc0:	e3083004 	movw	r3, #32772	; 0x8004
6030edc4:	e3463031 	movt	r3, #24625	; 0x6031
6030edc8:	e0203290 	mla	r0, r0, r2, r3
6030edcc:	eb0005da 	bl	6031053c <vListInsertEnd>
6030edd0:	e3083154 	movw	r3, #33108	; 0x8154
6030edd4:	e3463031 	movt	r3, #24625	; 0x6031
6030edd8:	e5933000 	ldr	r3, [r3]
6030eddc:	e3530000 	cmp	r3, #0
6030ede0:	0a000001 	beq	6030edec <prvAddNewTaskToReadyList+0x128>
6030ede4:	e3570001 	cmp	r7, #1
6030ede8:	9a000001 	bls	6030edf4 <prvAddNewTaskToReadyList+0x130>
6030edec:	e8bd47f0 	pop	{r4, r5, r6, r7, r8, r9, sl, lr}
6030edf0:	eaffff79 	b	6030ebdc <vTaskExitCritical>
6030edf4:	e7953107 	ldr	r3, [r5, r7, lsl #2]
6030edf8:	e3530000 	cmp	r3, #0
6030edfc:	0a000004 	beq	6030ee14 <prvAddNewTaskToReadyList+0x150>
6030ee00:	e7952107 	ldr	r2, [r5, r7, lsl #2]
6030ee04:	e596302c 	ldr	r3, [r6, #44]	; 0x2c
6030ee08:	e592202c 	ldr	r2, [r2, #44]	; 0x2c
6030ee0c:	e1520003 	cmp	r2, r3
6030ee10:	2afffff5 	bcs	6030edec <prvAddNewTaskToReadyList+0x128>
6030ee14:	e1a00007 	mov	r0, r7
6030ee18:	ebfffc0a 	bl	6030de48 <prvYieldCore>
6030ee1c:	e8bd47f0 	pop	{r4, r5, r6, r7, r8, r9, sl, lr}
6030ee20:	eaffff6d 	b	6030ebdc <vTaskExitCritical>
6030ee24:	e3083000 	movw	r3, #32768	; 0x8000
6030ee28:	e3463031 	movt	r3, #24625	; 0x6031
6030ee2c:	e5930000 	ldr	r0, [r3]
6030ee30:	eb0005c1 	bl	6031053c <vListInsertEnd>
6030ee34:	eaffffe5 	b	6030edd0 <prvAddNewTaskToReadyList+0x10c>
6030ee38:	e3084004 	movw	r4, #32772	; 0x8004
6030ee3c:	e3464031 	movt	r4, #24625	; 0x6031
6030ee40:	e28480dc 	add	r8, r4, #220	; 0xdc
6030ee44:	e1a00004 	mov	r0, r4
6030ee48:	e2844014 	add	r4, r4, #20
6030ee4c:	eb0005ae 	bl	6031050c <vListInitialise>
6030ee50:	e1580004 	cmp	r8, r4
6030ee54:	1afffffa 	bne	6030ee44 <prvAddNewTaskToReadyList+0x180>
6030ee58:	e3089104 	movw	r9, #33028	; 0x8104
6030ee5c:	e3469031 	movt	r9, #24625	; 0x6031
6030ee60:	e1a00009 	mov	r0, r9
6030ee64:	e3088118 	movw	r8, #33048	; 0x8118
6030ee68:	e3468031 	movt	r8, #24625	; 0x6031
6030ee6c:	eb0005a6 	bl	6031050c <vListInitialise>
6030ee70:	e1a00008 	mov	r0, r8
6030ee74:	eb0005a4 	bl	6031050c <vListInitialise>
6030ee78:	e308416c 	movw	r4, #33132	; 0x816c
6030ee7c:	e3464031 	movt	r4, #24625	; 0x6031
6030ee80:	e3080140 	movw	r0, #33088	; 0x8140
6030ee84:	e3460031 	movt	r0, #24625	; 0x6031
6030ee88:	eb00059f 	bl	6031050c <vListInitialise>
6030ee8c:	e1a00004 	mov	r0, r4
6030ee90:	eb00059d 	bl	6031050c <vListInitialise>
6030ee94:	e3080180 	movw	r0, #33152	; 0x8180
6030ee98:	e3460031 	movt	r0, #24625	; 0x6031
6030ee9c:	eb00059a 	bl	6031050c <vListInitialise>
6030eea0:	e3080158 	movw	r0, #33112	; 0x8158
6030eea4:	e3460031 	movt	r0, #24625	; 0x6031
6030eea8:	eb000597 	bl	6031050c <vListInitialise>
6030eeac:	e3073ff8 	movw	r3, #32760	; 0x7ff8
6030eeb0:	e3463031 	movt	r3, #24625	; 0x6031
6030eeb4:	e5839000 	str	r9, [r3]
6030eeb8:	e3073ffc 	movw	r3, #32764	; 0x7ffc
6030eebc:	e3463031 	movt	r3, #24625	; 0x6031
6030eec0:	e5838000 	str	r8, [r3]
6030eec4:	e3083000 	movw	r3, #32768	; 0x8000
6030eec8:	e3463031 	movt	r3, #24625	; 0x6031
6030eecc:	e5834000 	str	r4, [r3]
6030eed0:	eaffffa7 	b	6030ed74 <prvAddNewTaskToReadyList+0xb0>
6030eed4:	e3a07001 	mov	r7, #1
6030eed8:	eaffff9d 	b	6030ed54 <prvAddNewTaskToReadyList+0x90>

6030eedc <xTaskCreateStatic>:
6030eedc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
6030eee0:	e1a04000 	mov	r4, r0
6030eee4:	e24dd01c 	sub	sp, sp, #28
6030eee8:	e1a09001 	mov	r9, r1
6030eeec:	e1a07002 	mov	r7, r2
6030eef0:	e59d8044 	ldr	r8, [sp, #68]	; 0x44
6030eef4:	e59d6048 	ldr	r6, [sp, #72]	; 0x48
6030eef8:	e3580000 	cmp	r8, #0
6030eefc:	e1a05003 	mov	r5, r3
6030ef00:	13a0b001 	movne	fp, #1
6030ef04:	03a0b000 	moveq	fp, #0
6030ef08:	0a00002f 	beq	6030efcc <xTaskCreateStatic+0xf0>
6030ef0c:	e3560000 	cmp	r6, #0
6030ef10:	13a0a001 	movne	sl, #1
6030ef14:	03a0a000 	moveq	sl, #0
6030ef18:	0a00001c 	beq	6030ef90 <xTaskCreateStatic+0xb4>
6030ef1c:	e3a03f5e 	mov	r3, #376	; 0x178
6030ef20:	e58d3014 	str	r3, [sp, #20]
6030ef24:	e59d3014 	ldr	r3, [sp, #20]
6030ef28:	e3530f5e 	cmp	r3, #376	; 0x178
6030ef2c:	1a000021 	bne	6030efb8 <xTaskCreateStatic+0xdc>
6030ef30:	e59d3014 	ldr	r3, [sp, #20]
6030ef34:	e11b000a 	tst	fp, sl
6030ef38:	1a000002 	bne	6030ef48 <xTaskCreateStatic+0x6c>
6030ef3c:	e3a00000 	mov	r0, #0
6030ef40:	e28dd01c 	add	sp, sp, #28
6030ef44:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
6030ef48:	e59dc040 	ldr	ip, [sp, #64]	; 0x40
6030ef4c:	e5868030 	str	r8, [r6, #48]	; 0x30
6030ef50:	e1a03005 	mov	r3, r5
6030ef54:	e58dc000 	str	ip, [sp]
6030ef58:	e1a02007 	mov	r2, r7
6030ef5c:	e1a01009 	mov	r1, r9
6030ef60:	e1a00004 	mov	r0, r4
6030ef64:	e58d6008 	str	r6, [sp, #8]
6030ef68:	e3a0c002 	mov	ip, #2
6030ef6c:	e5c6c171 	strb	ip, [r6, #369]	; 0x171
6030ef70:	e28dc010 	add	ip, sp, #16
6030ef74:	e58dc004 	str	ip, [sp, #4]
6030ef78:	ebfffbca 	bl	6030dea8 <prvInitialiseNewTask.constprop.0>
6030ef7c:	e1a00006 	mov	r0, r6
6030ef80:	ebffff4f 	bl	6030ecc4 <prvAddNewTaskToReadyList>
6030ef84:	e59d0010 	ldr	r0, [sp, #16]
6030ef88:	e28dd01c 	add	sp, sp, #28
6030ef8c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
6030ef90:	e3001376 	movw	r1, #886	; 0x376
6030ef94:	e3060850 	movw	r0, #26704	; 0x6850
6030ef98:	e3460031 	movt	r0, #24625	; 0x6031
6030ef9c:	ebffeb22 	bl	60309c2c <vAssertCalled>
6030efa0:	e3a03f5e 	mov	r3, #376	; 0x178
6030efa4:	e58d3014 	str	r3, [sp, #20]
6030efa8:	e59d3014 	ldr	r3, [sp, #20]
6030efac:	e3530f5e 	cmp	r3, #376	; 0x178
6030efb0:	059d3014 	ldreq	r3, [sp, #20]
6030efb4:	0affffe0 	beq	6030ef3c <xTaskCreateStatic+0x60>
6030efb8:	e300137e 	movw	r1, #894	; 0x37e
6030efbc:	e3060850 	movw	r0, #26704	; 0x6850
6030efc0:	e3460031 	movt	r0, #24625	; 0x6031
6030efc4:	ebffeb18 	bl	60309c2c <vAssertCalled>
6030efc8:	eaffffd8 	b	6030ef30 <xTaskCreateStatic+0x54>
6030efcc:	e3060850 	movw	r0, #26704	; 0x6850
6030efd0:	e3460031 	movt	r0, #24625	; 0x6031
6030efd4:	e3001375 	movw	r1, #885	; 0x375
6030efd8:	ebffeb13 	bl	60309c2c <vAssertCalled>
6030efdc:	eaffffca 	b	6030ef0c <xTaskCreateStatic+0x30>

6030efe0 <vTaskStartScheduler>:
6030efe0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
6030efe4:	e308612c 	movw	r6, #33068	; 0x812c
6030efe8:	e3466031 	movt	r6, #24625	; 0x6031
6030efec:	e24dd03c 	sub	sp, sp, #60	; 0x3c
6030eff0:	e3007288 	movw	r7, #648	; 0x288
6030eff4:	e3467031 	movt	r7, #24625	; 0x6031
6030eff8:	e3075ff0 	movw	r5, #32752	; 0x7ff0
6030effc:	e3465031 	movt	r5, #24625	; 0x6031
6030f000:	eb0009a6 	bl	603116a0 <xTimerCreateTimerTask>
6030f004:	e3a04000 	mov	r4, #0
6030f008:	e1a0a000 	mov	sl, r0
6030f00c:	e3a08049 	mov	r8, #73	; 0x49
6030f010:	e35a0000 	cmp	sl, #0
6030f014:	0a000032 	beq	6030f0e4 <vTaskStartScheduler+0x104>
6030f018:	e30618c8 	movw	r1, #26824	; 0x68c8
6030f01c:	e3461031 	movt	r1, #24625	; 0x6031
6030f020:	e28d0021 	add	r0, sp, #33	; 0x21
6030f024:	e3a02000 	mov	r2, #0
6030f028:	e5cd8020 	strb	r8, [sp, #32]
6030f02c:	e5f13001 	ldrb	r3, [r1, #1]!
6030f030:	e1a0c002 	mov	ip, r2
6030f034:	e3530000 	cmp	r3, #0
6030f038:	e2822001 	add	r2, r2, #1
6030f03c:	e4c03001 	strb	r3, [r0], #1
6030f040:	1afffff9 	bne	6030f02c <vTaskStartScheduler+0x4c>
6030f044:	e28c1038 	add	r1, ip, #56	; 0x38
6030f048:	e2822038 	add	r2, r2, #56	; 0x38
6030f04c:	e08d2002 	add	r2, sp, r2
6030f050:	e08dc001 	add	ip, sp, r1
6030f054:	e3540000 	cmp	r4, #0
6030f058:	e2841030 	add	r1, r4, #48	; 0x30
6030f05c:	e28d0014 	add	r0, sp, #20
6030f060:	e5421018 	strb	r1, [r2, #-24]	; 0xffffffe8
6030f064:	e58d3014 	str	r3, [sp, #20]
6030f068:	e28d201c 	add	r2, sp, #28
6030f06c:	e28d1018 	add	r1, sp, #24
6030f070:	e54c3016 	strb	r3, [ip, #-22]	; 0xffffffea
6030f074:	e58d3018 	str	r3, [sp, #24]
6030f078:	0a00001d 	beq	6030f0f4 <vTaskStartScheduler+0x114>
6030f07c:	eb000d2e 	bl	6031253c <vApplicationGetPassiveIdleTaskMemory>
6030f080:	e59d1014 	ldr	r1, [sp, #20]
6030f084:	e59d2018 	ldr	r2, [sp, #24]
6030f088:	e3a0b000 	mov	fp, #0
6030f08c:	e1a0300b 	mov	r3, fp
6030f090:	e58d1008 	str	r1, [sp, #8]
6030f094:	e58d2004 	str	r2, [sp, #4]
6030f098:	e28d1020 	add	r1, sp, #32
6030f09c:	e59d201c 	ldr	r2, [sp, #28]
6030f0a0:	e1a00007 	mov	r0, r7
6030f0a4:	e58db000 	str	fp, [sp]
6030f0a8:	ebffff8b 	bl	6030eedc <xTaskCreateStatic>
6030f0ac:	e7850104 	str	r0, [r5, r4, lsl #2]
6030f0b0:	e1a09000 	mov	r9, r0
6030f0b4:	e4860004 	str	r0, [r6], #4
6030f0b8:	e7950104 	ldr	r0, [r5, r4, lsl #2]
6030f0bc:	e059a00b 	subs	sl, r9, fp
6030f0c0:	e2800004 	add	r0, r0, #4
6030f0c4:	13a0a001 	movne	sl, #1
6030f0c8:	eb00053c 	bl	603105c0 <uxListRemove>
6030f0cc:	e7953104 	ldr	r3, [r5, r4, lsl #2]
6030f0d0:	e3540001 	cmp	r4, #1
6030f0d4:	e5834034 	str	r4, [r3, #52]	; 0x34
6030f0d8:	1a000003 	bne	6030f0ec <vTaskStartScheduler+0x10c>
6030f0dc:	e159000b 	cmp	r9, fp
6030f0e0:	1a000005 	bne	6030f0fc <vTaskStartScheduler+0x11c>
6030f0e4:	e28dd03c 	add	sp, sp, #60	; 0x3c
6030f0e8:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
6030f0ec:	e3a04001 	mov	r4, #1
6030f0f0:	eaffffc6 	b	6030f010 <vTaskStartScheduler+0x30>
6030f0f4:	eb000d07 	bl	60312518 <vApplicationGetIdleTaskMemory>
6030f0f8:	eaffffe0 	b	6030f080 <vTaskStartScheduler+0xa0>
6030f0fc:	eb000ccc 	bl	60312434 <ulPortInterruptLock>
6030f100:	eb000ccb 	bl	60312434 <ulPortInterruptLock>
6030f104:	e1a06000 	mov	r6, r0
6030f108:	eb000ce4 	bl	603124a0 <ulPortGetCoreId>
6030f10c:	e1a03000 	mov	r3, r0
6030f110:	e1a00006 	mov	r0, r6
6030f114:	e7955103 	ldr	r5, [r5, r3, lsl #2]
6030f118:	eb000cca 	bl	60312448 <ulPortInterruptUnLock>
6030f11c:	e3073000 	movw	r3, #28672	; 0x7000
6030f120:	e3463031 	movt	r3, #24625	; 0x6031
6030f124:	e285507c 	add	r5, r5, #124	; 0x7c
6030f128:	e3081134 	movw	r1, #33076	; 0x8134
6030f12c:	e3461031 	movt	r1, #24625	; 0x6031
6030f130:	e5835000 	str	r5, [r3]
6030f134:	e3082154 	movw	r2, #33108	; 0x8154
6030f138:	e3462031 	movt	r2, #24625	; 0x6031
6030f13c:	e3083194 	movw	r3, #33172	; 0x8194
6030f140:	e3463031 	movt	r3, #24625	; 0x6031
6030f144:	e3e00000 	mvn	r0, #0
6030f148:	e5810000 	str	r0, [r1]
6030f14c:	e5824000 	str	r4, [r2]
6030f150:	e583b000 	str	fp, [r3]
6030f154:	eb000c7d 	bl	60312350 <xPortStartScheduler>
6030f158:	e28dd03c 	add	sp, sp, #60	; 0x3c
6030f15c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}

6030f160 <xTaskCreate>:
6030f160:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
6030f164:	e1a07000 	mov	r7, r0
6030f168:	e24dd014 	sub	sp, sp, #20
6030f16c:	e1a00102 	lsl	r0, r2, #2
6030f170:	e1a05002 	mov	r5, r2
6030f174:	e1a08001 	mov	r8, r1
6030f178:	e1a09003 	mov	r9, r3
6030f17c:	eb000c03 	bl	60312190 <pvPortMalloc>
6030f180:	e2504000 	subs	r4, r0, #0
6030f184:	03e00000 	mvneq	r0, #0
6030f188:	0a000013 	beq	6030f1dc <xTaskCreate+0x7c>
6030f18c:	e3a00f5e 	mov	r0, #376	; 0x178
6030f190:	eb000bfe 	bl	60312190 <pvPortMalloc>
6030f194:	e2506000 	subs	r6, r0, #0
6030f198:	0a000011 	beq	6030f1e4 <xTaskCreate+0x84>
6030f19c:	e3a0c000 	mov	ip, #0
6030f1a0:	e5c6c171 	strb	ip, [r6, #369]	; 0x171
6030f1a4:	e59dc034 	ldr	ip, [sp, #52]	; 0x34
6030f1a8:	e5864030 	str	r4, [r6, #48]	; 0x30
6030f1ac:	e58dc004 	str	ip, [sp, #4]
6030f1b0:	e59dc030 	ldr	ip, [sp, #48]	; 0x30
6030f1b4:	e1a03009 	mov	r3, r9
6030f1b8:	e1a02005 	mov	r2, r5
6030f1bc:	e1a01008 	mov	r1, r8
6030f1c0:	e1a00007 	mov	r0, r7
6030f1c4:	e58dc000 	str	ip, [sp]
6030f1c8:	e58d6008 	str	r6, [sp, #8]
6030f1cc:	ebfffb35 	bl	6030dea8 <prvInitialiseNewTask.constprop.0>
6030f1d0:	e1a00006 	mov	r0, r6
6030f1d4:	ebfffeba 	bl	6030ecc4 <prvAddNewTaskToReadyList>
6030f1d8:	e3a00001 	mov	r0, #1
6030f1dc:	e28dd014 	add	sp, sp, #20
6030f1e0:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
6030f1e4:	e1a00004 	mov	r0, r4
6030f1e8:	eb000ab6 	bl	60311cc8 <vPortFree>
6030f1ec:	e3e00000 	mvn	r0, #0
6030f1f0:	eafffff9 	b	6030f1dc <xTaskCreate+0x7c>

6030f1f4 <vTaskDelete>:
6030f1f4:	e92d4070 	push	{r4, r5, r6, lr}
6030f1f8:	e1a04000 	mov	r4, r0
6030f1fc:	ebfffe5b 	bl	6030eb70 <vTaskEnterCritical>
6030f200:	e3540000 	cmp	r4, #0
6030f204:	0a00005b 	beq	6030f378 <vTaskDelete+0x184>
6030f208:	e5943014 	ldr	r3, [r4, #20]
6030f20c:	e5945034 	ldr	r5, [r4, #52]	; 0x34
6030f210:	e3530000 	cmp	r3, #0
6030f214:	0a000013 	beq	6030f268 <vTaskDelete+0x74>
6030f218:	e2840004 	add	r0, r4, #4
6030f21c:	eb0004e7 	bl	603105c0 <uxListRemove>
6030f220:	e3500000 	cmp	r0, #0
6030f224:	1a00000f 	bne	6030f268 <vTaskDelete+0x74>
6030f228:	e308c100 	movw	ip, #33024	; 0x8100
6030f22c:	e346c031 	movt	ip, #24625	; 0x6031
6030f230:	e59c3000 	ldr	r3, [ip]
6030f234:	e3530000 	cmp	r3, #0
6030f238:	0a000009 	beq	6030f264 <vTaskDelete+0x70>
6030f23c:	e3081004 	movw	r1, #32772	; 0x8004
6030f240:	e3461031 	movt	r1, #24625	; 0x6031
6030f244:	e3a00014 	mov	r0, #20
6030f248:	ea000001 	b	6030f254 <vTaskDelete+0x60>
6030f24c:	e2533001 	subs	r3, r3, #1
6030f250:	0a000003 	beq	6030f264 <vTaskDelete+0x70>
6030f254:	e0020390 	mul	r2, r0, r3
6030f258:	e7912002 	ldr	r2, [r1, r2]
6030f25c:	e3520000 	cmp	r2, #0
6030f260:	0afffff9 	beq	6030f24c <vTaskDelete+0x58>
6030f264:	e58c3000 	str	r3, [ip]
6030f268:	e5943028 	ldr	r3, [r4, #40]	; 0x28
6030f26c:	e3530000 	cmp	r3, #0
6030f270:	0a000001 	beq	6030f27c <vTaskDelete+0x88>
6030f274:	e2840018 	add	r0, r4, #24
6030f278:	eb0004d0 	bl	603105c0 <uxListRemove>
6030f27c:	e30830fc 	movw	r3, #33020	; 0x80fc
6030f280:	e3463031 	movt	r3, #24625	; 0x6031
6030f284:	e3750001 	cmn	r5, #1
6030f288:	e5932000 	ldr	r2, [r3]
6030f28c:	e2822001 	add	r2, r2, #1
6030f290:	e5832000 	str	r2, [r3]
6030f294:	0a000011 	beq	6030f2e0 <vTaskDelete+0xec>
6030f298:	e2841004 	add	r1, r4, #4
6030f29c:	e3080180 	movw	r0, #33152	; 0x8180
6030f2a0:	e3460031 	movt	r0, #24625	; 0x6031
6030f2a4:	eb0004a4 	bl	6031053c <vListInsertEnd>
6030f2a8:	e30830f4 	movw	r3, #33012	; 0x80f4
6030f2ac:	e3463031 	movt	r3, #24625	; 0x6031
6030f2b0:	e3082154 	movw	r2, #33108	; 0x8154
6030f2b4:	e3462031 	movt	r2, #24625	; 0x6031
6030f2b8:	e5931000 	ldr	r1, [r3]
6030f2bc:	e2811001 	add	r1, r1, #1
6030f2c0:	e5831000 	str	r1, [r3]
6030f2c4:	e5923000 	ldr	r3, [r2]
6030f2c8:	e3530000 	cmp	r3, #0
6030f2cc:	0a000001 	beq	6030f2d8 <vTaskDelete+0xe4>
6030f2d0:	e3550001 	cmp	r5, #1
6030f2d4:	9a00001c 	bls	6030f34c <vTaskDelete+0x158>
6030f2d8:	e8bd4070 	pop	{r4, r5, r6, lr}
6030f2dc:	eafffe3e 	b	6030ebdc <vTaskExitCritical>
6030f2e0:	e30830f0 	movw	r3, #33008	; 0x80f0
6030f2e4:	e3463031 	movt	r3, #24625	; 0x6031
6030f2e8:	e1a00004 	mov	r0, r4
6030f2ec:	e5932000 	ldr	r2, [r3]
6030f2f0:	e2422001 	sub	r2, r2, #1
6030f2f4:	e5832000 	str	r2, [r3]
6030f2f8:	ebfffab9 	bl	6030dde4 <prvDeleteTCB>
6030f2fc:	e3073ff8 	movw	r3, #32760	; 0x7ff8
6030f300:	e3463031 	movt	r3, #24625	; 0x6031
6030f304:	e5932000 	ldr	r2, [r3]
6030f308:	e5922000 	ldr	r2, [r2]
6030f30c:	e3520000 	cmp	r2, #0
6030f310:	03083134 	movweq	r3, #33076	; 0x8134
6030f314:	15932000 	ldrne	r2, [r3]
6030f318:	03463031 	movteq	r3, #24625	; 0x6031
6030f31c:	13083134 	movwne	r3, #33076	; 0x8134
6030f320:	1592200c 	ldrne	r2, [r2, #12]
6030f324:	13463031 	movtne	r3, #24625	; 0x6031
6030f328:	05835000 	streq	r5, [r3]
6030f32c:	1592200c 	ldrne	r2, [r2, #12]
6030f330:	15922004 	ldrne	r2, [r2, #4]
6030f334:	15832000 	strne	r2, [r3]
6030f338:	e3083154 	movw	r3, #33108	; 0x8154
6030f33c:	e3463031 	movt	r3, #24625	; 0x6031
6030f340:	e8bd4070 	pop	{r4, r5, r6, lr}
6030f344:	e5933000 	ldr	r3, [r3]
6030f348:	eafffe23 	b	6030ebdc <vTaskExitCritical>
6030f34c:	e3e03002 	mvn	r3, #2
6030f350:	e5843034 	str	r3, [r4, #52]	; 0x34
6030f354:	eb000c51 	bl	603124a0 <ulPortGetCoreId>
6030f358:	e3073ff0 	movw	r3, #32752	; 0x7ff0
6030f35c:	e3463031 	movt	r3, #24625	; 0x6031
6030f360:	e7933100 	ldr	r3, [r3, r0, lsl #2]
6030f364:	e1530004 	cmp	r3, r4
6030f368:	0a00000c 	beq	6030f3a0 <vTaskDelete+0x1ac>
6030f36c:	e1a00005 	mov	r0, r5
6030f370:	ebfffab4 	bl	6030de48 <prvYieldCore>
6030f374:	eaffffd7 	b	6030f2d8 <vTaskDelete+0xe4>
6030f378:	eb000c2d 	bl	60312434 <ulPortInterruptLock>
6030f37c:	e1a04000 	mov	r4, r0
6030f380:	eb000c46 	bl	603124a0 <ulPortGetCoreId>
6030f384:	e3073ff0 	movw	r3, #32752	; 0x7ff0
6030f388:	e3463031 	movt	r3, #24625	; 0x6031
6030f38c:	e1a02000 	mov	r2, r0
6030f390:	e1a00004 	mov	r0, r4
6030f394:	e7934102 	ldr	r4, [r3, r2, lsl #2]
6030f398:	eb000c2a 	bl	60312448 <ulPortInterruptUnLock>
6030f39c:	eaffff99 	b	6030f208 <vTaskDelete+0x14>
6030f3a0:	ebfffe0d 	bl	6030ebdc <vTaskExitCritical>
6030f3a4:	ef000000 	svc	0x00000000
6030f3a8:	eaffffca 	b	6030f2d8 <vTaskDelete+0xe4>

6030f3ac <prvAddCurrentTaskToDelayedList>:
6030f3ac:	e3083194 	movw	r3, #33172	; 0x8194
6030f3b0:	e3463031 	movt	r3, #24625	; 0x6031
6030f3b4:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
6030f3b8:	e1a06001 	mov	r6, r1
6030f3bc:	e5938000 	ldr	r8, [r3]
6030f3c0:	e1a05000 	mov	r5, r0
6030f3c4:	eb000c1a 	bl	60312434 <ulPortInterruptLock>
6030f3c8:	e1a04000 	mov	r4, r0
6030f3cc:	eb000c33 	bl	603124a0 <ulPortGetCoreId>
6030f3d0:	e1a03000 	mov	r3, r0
6030f3d4:	e1a00004 	mov	r0, r4
6030f3d8:	e3074ff0 	movw	r4, #32752	; 0x7ff0
6030f3dc:	e3464031 	movt	r4, #24625	; 0x6031
6030f3e0:	e7947103 	ldr	r7, [r4, r3, lsl #2]
6030f3e4:	eb000c17 	bl	60312448 <ulPortInterruptUnLock>
6030f3e8:	e3a03000 	mov	r3, #0
6030f3ec:	e5c73172 	strb	r3, [r7, #370]	; 0x172
6030f3f0:	ebfffdde 	bl	6030eb70 <vTaskEnterCritical>
6030f3f4:	eb000c0e 	bl	60312434 <ulPortInterruptLock>
6030f3f8:	e1a07000 	mov	r7, r0
6030f3fc:	eb000c27 	bl	603124a0 <ulPortGetCoreId>
6030f400:	e1a03000 	mov	r3, r0
6030f404:	e1a00007 	mov	r0, r7
6030f408:	e7947103 	ldr	r7, [r4, r3, lsl #2]
6030f40c:	eb000c0d 	bl	60312448 <ulPortInterruptUnLock>
6030f410:	e5973034 	ldr	r3, [r7, #52]	; 0x34
6030f414:	e3730003 	cmn	r3, #3
6030f418:	0a000049 	beq	6030f544 <prvAddCurrentTaskToDelayedList+0x198>
6030f41c:	e2566000 	subs	r6, r6, #0
6030f420:	13a06001 	movne	r6, #1
6030f424:	e3750001 	cmn	r5, #1
6030f428:	13a06000 	movne	r6, #0
6030f42c:	e3560000 	cmp	r6, #0
6030f430:	1a000024 	bne	6030f4c8 <prvAddCurrentTaskToDelayedList+0x11c>
6030f434:	e0985005 	adds	r5, r8, r5
6030f438:	23a07001 	movcs	r7, #1
6030f43c:	33a07000 	movcc	r7, #0
6030f440:	eb000bfb 	bl	60312434 <ulPortInterruptLock>
6030f444:	e1a06000 	mov	r6, r0
6030f448:	eb000c14 	bl	603124a0 <ulPortGetCoreId>
6030f44c:	e1a03000 	mov	r3, r0
6030f450:	e1a00006 	mov	r0, r6
6030f454:	e7946103 	ldr	r6, [r4, r3, lsl #2]
6030f458:	eb000bfa 	bl	60312448 <ulPortInterruptUnLock>
6030f45c:	e3570000 	cmp	r7, #0
6030f460:	e5865004 	str	r5, [r6, #4]
6030f464:	0a000023 	beq	6030f4f8 <prvAddCurrentTaskToDelayedList+0x14c>
6030f468:	e3073ffc 	movw	r3, #32764	; 0x7ffc
6030f46c:	e3463031 	movt	r3, #24625	; 0x6031
6030f470:	e5936000 	ldr	r6, [r3]
6030f474:	eb000bee 	bl	60312434 <ulPortInterruptLock>
6030f478:	e1a05000 	mov	r5, r0
6030f47c:	eb000c07 	bl	603124a0 <ulPortGetCoreId>
6030f480:	e1a03000 	mov	r3, r0
6030f484:	e1a00005 	mov	r0, r5
6030f488:	e7945103 	ldr	r5, [r4, r3, lsl #2]
6030f48c:	eb000bed 	bl	60312448 <ulPortInterruptUnLock>
6030f490:	e2851004 	add	r1, r5, #4
6030f494:	e1a00006 	mov	r0, r6
6030f498:	eb000432 	bl	60310568 <vListInsert>
6030f49c:	eb000be4 	bl	60312434 <ulPortInterruptLock>
6030f4a0:	e1a05000 	mov	r5, r0
6030f4a4:	eb000bfd 	bl	603124a0 <ulPortGetCoreId>
6030f4a8:	e1a03000 	mov	r3, r0
6030f4ac:	e1a00005 	mov	r0, r5
6030f4b0:	e7944103 	ldr	r4, [r4, r3, lsl #2]
6030f4b4:	eb000be3 	bl	60312448 <ulPortInterruptUnLock>
6030f4b8:	e3e03001 	mvn	r3, #1
6030f4bc:	e5843034 	str	r3, [r4, #52]	; 0x34
6030f4c0:	e8bd41f0 	pop	{r4, r5, r6, r7, r8, lr}
6030f4c4:	eafffdc4 	b	6030ebdc <vTaskExitCritical>
6030f4c8:	eb000bd9 	bl	60312434 <ulPortInterruptLock>
6030f4cc:	e1a05000 	mov	r5, r0
6030f4d0:	eb000bf2 	bl	603124a0 <ulPortGetCoreId>
6030f4d4:	e1a03000 	mov	r3, r0
6030f4d8:	e1a00005 	mov	r0, r5
6030f4dc:	e7945103 	ldr	r5, [r4, r3, lsl #2]
6030f4e0:	eb000bd8 	bl	60312448 <ulPortInterruptUnLock>
6030f4e4:	e2851004 	add	r1, r5, #4
6030f4e8:	e3080158 	movw	r0, #33112	; 0x8158
6030f4ec:	e3460031 	movt	r0, #24625	; 0x6031
6030f4f0:	eb000411 	bl	6031053c <vListInsertEnd>
6030f4f4:	eaffffe8 	b	6030f49c <prvAddCurrentTaskToDelayedList+0xf0>
6030f4f8:	e3073ff8 	movw	r3, #32760	; 0x7ff8
6030f4fc:	e3463031 	movt	r3, #24625	; 0x6031
6030f500:	e5937000 	ldr	r7, [r3]
6030f504:	eb000bca 	bl	60312434 <ulPortInterruptLock>
6030f508:	e1a06000 	mov	r6, r0
6030f50c:	eb000be3 	bl	603124a0 <ulPortGetCoreId>
6030f510:	e1a03000 	mov	r3, r0
6030f514:	e1a00006 	mov	r0, r6
6030f518:	e7946103 	ldr	r6, [r4, r3, lsl #2]
6030f51c:	eb000bc9 	bl	60312448 <ulPortInterruptUnLock>
6030f520:	e2861004 	add	r1, r6, #4
6030f524:	e1a00007 	mov	r0, r7
6030f528:	eb00040e 	bl	60310568 <vListInsert>
6030f52c:	e3083134 	movw	r3, #33076	; 0x8134
6030f530:	e3463031 	movt	r3, #24625	; 0x6031
6030f534:	e5932000 	ldr	r2, [r3]
6030f538:	e1520005 	cmp	r2, r5
6030f53c:	85835000 	strhi	r5, [r3]
6030f540:	eaffffd5 	b	6030f49c <prvAddCurrentTaskToDelayedList+0xf0>
6030f544:	eb000bba 	bl	60312434 <ulPortInterruptLock>
6030f548:	e1a05000 	mov	r5, r0
6030f54c:	eb000bd3 	bl	603124a0 <ulPortGetCoreId>
6030f550:	e1a03000 	mov	r3, r0
6030f554:	e1a00005 	mov	r0, r5
6030f558:	e7945103 	ldr	r5, [r4, r3, lsl #2]
6030f55c:	eb000bb9 	bl	60312448 <ulPortInterruptUnLock>
6030f560:	e5953028 	ldr	r3, [r5, #40]	; 0x28
6030f564:	e3530000 	cmp	r3, #0
6030f568:	0affffd4 	beq	6030f4c0 <prvAddCurrentTaskToDelayedList+0x114>
6030f56c:	eb000bb0 	bl	60312434 <ulPortInterruptLock>
6030f570:	e1a05000 	mov	r5, r0
6030f574:	eb000bc9 	bl	603124a0 <ulPortGetCoreId>
6030f578:	e1a03000 	mov	r3, r0
6030f57c:	e1a00005 	mov	r0, r5
6030f580:	e7944103 	ldr	r4, [r4, r3, lsl #2]
6030f584:	eb000baf 	bl	60312448 <ulPortInterruptUnLock>
6030f588:	e2840018 	add	r0, r4, #24
6030f58c:	eb00040b 	bl	603105c0 <uxListRemove>
6030f590:	eaffffca 	b	6030f4c0 <prvAddCurrentTaskToDelayedList+0x114>

6030f594 <xTaskIncrementTick>:
6030f594:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
6030f598:	e24dd014 	sub	sp, sp, #20
6030f59c:	eb000bbf 	bl	603124a0 <ulPortGetCoreId>
6030f5a0:	e58d0000 	str	r0, [sp]
6030f5a4:	ebfffd71 	bl	6030eb70 <vTaskEnterCritical>
6030f5a8:	e30830f8 	movw	r3, #33016	; 0x80f8
6030f5ac:	e3463031 	movt	r3, #24625	; 0x6031
6030f5b0:	e5933000 	ldr	r3, [r3]
6030f5b4:	e3530000 	cmp	r3, #0
6030f5b8:	1a000038 	bne	6030f6a0 <xTaskIncrementTick+0x10c>
6030f5bc:	e3083194 	movw	r3, #33172	; 0x8194
6030f5c0:	e3463031 	movt	r3, #24625	; 0x6031
6030f5c4:	e5937000 	ldr	r7, [r3]
6030f5c8:	e2877001 	add	r7, r7, #1
6030f5cc:	e3570000 	cmp	r7, #0
6030f5d0:	e5837000 	str	r7, [r3]
6030f5d4:	0a00003b 	beq	6030f6c8 <xTaskIncrementTick+0x134>
6030f5d8:	e3083134 	movw	r3, #33076	; 0x8134
6030f5dc:	e3463031 	movt	r3, #24625	; 0x6031
6030f5e0:	e58d3008 	str	r3, [sp, #8]
6030f5e4:	e59d3008 	ldr	r3, [sp, #8]
6030f5e8:	e5933000 	ldr	r3, [r3]
6030f5ec:	e1530007 	cmp	r3, r7
6030f5f0:	9a000053 	bls	6030f744 <xTaskIncrementTick+0x1b0>
6030f5f4:	e3075ff0 	movw	r5, #32752	; 0x7ff0
6030f5f8:	e3465031 	movt	r5, #24625	; 0x6031
6030f5fc:	e3089004 	movw	r9, #32772	; 0x8004
6030f600:	e3469031 	movt	r9, #24625	; 0x6031
6030f604:	e3086198 	movw	r6, #33176	; 0x8198
6030f608:	e3466031 	movt	r6, #24625	; 0x6031
6030f60c:	e5953000 	ldr	r3, [r5]
6030f610:	e3a02014 	mov	r2, #20
6030f614:	e593302c 	ldr	r3, [r3, #44]	; 0x2c
6030f618:	e0030392 	mul	r3, r2, r3
6030f61c:	e3a02014 	mov	r2, #20
6030f620:	e7993003 	ldr	r3, [r9, r3]
6030f624:	e3530000 	cmp	r3, #0
6030f628:	13a03001 	movne	r3, #1
6030f62c:	15863000 	strne	r3, [r6]
6030f630:	e5953004 	ldr	r3, [r5, #4]
6030f634:	e593302c 	ldr	r3, [r3, #44]	; 0x2c
6030f638:	e0030392 	mul	r3, r2, r3
6030f63c:	e7993003 	ldr	r3, [r9, r3]
6030f640:	e3530000 	cmp	r3, #0
6030f644:	13a03001 	movne	r3, #1
6030f648:	15863004 	strne	r3, [r6, #4]
6030f64c:	e5963000 	ldr	r3, [r6]
6030f650:	e3530000 	cmp	r3, #0
6030f654:	0a000004 	beq	6030f66c <xTaskIncrementTick+0xd8>
6030f658:	e59d3000 	ldr	r3, [sp]
6030f65c:	e3530000 	cmp	r3, #0
6030f660:	0a00008f 	beq	6030f8a4 <xTaskIncrementTick+0x310>
6030f664:	e3a00000 	mov	r0, #0
6030f668:	ebfff9f6 	bl	6030de48 <prvYieldCore>
6030f66c:	e5964004 	ldr	r4, [r6, #4]
6030f670:	e3540000 	cmp	r4, #0
6030f674:	0a00000f 	beq	6030f6b8 <xTaskIncrementTick+0x124>
6030f678:	e59d3000 	ldr	r3, [sp]
6030f67c:	e3530001 	cmp	r3, #1
6030f680:	13a04000 	movne	r4, #0
6030f684:	0a000089 	beq	6030f8b0 <xTaskIncrementTick+0x31c>
6030f688:	e3a00001 	mov	r0, #1
6030f68c:	ebfff9ed 	bl	6030de48 <prvYieldCore>
6030f690:	ebfffd51 	bl	6030ebdc <vTaskExitCritical>
6030f694:	e1a00004 	mov	r0, r4
6030f698:	e28dd014 	add	sp, sp, #20
6030f69c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
6030f6a0:	e308313c 	movw	r3, #33084	; 0x813c
6030f6a4:	e3463031 	movt	r3, #24625	; 0x6031
6030f6a8:	e3a04000 	mov	r4, #0
6030f6ac:	e5932000 	ldr	r2, [r3]
6030f6b0:	e2822001 	add	r2, r2, #1
6030f6b4:	e5832000 	str	r2, [r3]
6030f6b8:	ebfffd47 	bl	6030ebdc <vTaskExitCritical>
6030f6bc:	e1a00004 	mov	r0, r4
6030f6c0:	e28dd014 	add	sp, sp, #20
6030f6c4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
6030f6c8:	e3074ff8 	movw	r4, #32760	; 0x7ff8
6030f6cc:	e3464031 	movt	r4, #24625	; 0x6031
6030f6d0:	e5943000 	ldr	r3, [r4]
6030f6d4:	e5933000 	ldr	r3, [r3]
6030f6d8:	e3530000 	cmp	r3, #0
6030f6dc:	1a00007e 	bne	6030f8dc <xTaskIncrementTick+0x348>
6030f6e0:	e3072ffc 	movw	r2, #32764	; 0x7ffc
6030f6e4:	e3462031 	movt	r2, #24625	; 0x6031
6030f6e8:	e5941000 	ldr	r1, [r4]
6030f6ec:	e3083138 	movw	r3, #33080	; 0x8138
6030f6f0:	e3463031 	movt	r3, #24625	; 0x6031
6030f6f4:	e5920000 	ldr	r0, [r2]
6030f6f8:	e5840000 	str	r0, [r4]
6030f6fc:	e5821000 	str	r1, [r2]
6030f700:	e5932000 	ldr	r2, [r3]
6030f704:	e2822001 	add	r2, r2, #1
6030f708:	e5832000 	str	r2, [r3]
6030f70c:	e5943000 	ldr	r3, [r4]
6030f710:	e5933000 	ldr	r3, [r3]
6030f714:	e3530000 	cmp	r3, #0
6030f718:	1a000058 	bne	6030f880 <xTaskIncrementTick+0x2ec>
6030f71c:	e3083134 	movw	r3, #33076	; 0x8134
6030f720:	e3463031 	movt	r3, #24625	; 0x6031
6030f724:	e1a02003 	mov	r2, r3
6030f728:	e58d3008 	str	r3, [sp, #8]
6030f72c:	e3e03000 	mvn	r3, #0
6030f730:	e5823000 	str	r3, [r2]
6030f734:	e59d3008 	ldr	r3, [sp, #8]
6030f738:	e5933000 	ldr	r3, [r3]
6030f73c:	e1530007 	cmp	r3, r7
6030f740:	8affffab 	bhi	6030f5f4 <xTaskIncrementTick+0x60>
6030f744:	e3074ff8 	movw	r4, #32760	; 0x7ff8
6030f748:	e3464031 	movt	r4, #24625	; 0x6031
6030f74c:	e3075ff0 	movw	r5, #32752	; 0x7ff0
6030f750:	e3465031 	movt	r5, #24625	; 0x6031
6030f754:	e3089004 	movw	r9, #32772	; 0x8004
6030f758:	e3469031 	movt	r9, #24625	; 0x6031
6030f75c:	e3086198 	movw	r6, #33176	; 0x8198
6030f760:	e3466031 	movt	r6, #24625	; 0x6031
6030f764:	e308a100 	movw	sl, #33024	; 0x8100
6030f768:	e346a031 	movt	sl, #24625	; 0x6031
6030f76c:	e3083000 	movw	r3, #32768	; 0x8000
6030f770:	e3463031 	movt	r3, #24625	; 0x6031
6030f774:	e58d9004 	str	r9, [sp, #4]
6030f778:	e58d300c 	str	r3, [sp, #12]
6030f77c:	ea000005 	b	6030f798 <xTaskIncrementTick+0x204>
6030f780:	e59d2000 	ldr	r2, [sp]
6030f784:	e7962102 	ldr	r2, [r6, r2, lsl #2]
6030f788:	e3520000 	cmp	r2, #0
6030f78c:	1a000024 	bne	6030f824 <xTaskIncrementTick+0x290>
6030f790:	e3a03001 	mov	r3, #1
6030f794:	e7863102 	str	r3, [r6, r2, lsl #2]
6030f798:	e5943000 	ldr	r3, [r4]
6030f79c:	e5933000 	ldr	r3, [r3]
6030f7a0:	e3530000 	cmp	r3, #0
6030f7a4:	0a00002b 	beq	6030f858 <xTaskIncrementTick+0x2c4>
6030f7a8:	e3a08014 	mov	r8, #20
6030f7ac:	e5943000 	ldr	r3, [r4]
6030f7b0:	e593300c 	ldr	r3, [r3, #12]
6030f7b4:	e593900c 	ldr	r9, [r3, #12]
6030f7b8:	e5992004 	ldr	r2, [r9, #4]
6030f7bc:	e1570002 	cmp	r7, r2
6030f7c0:	3a00003f 	bcc	6030f8c4 <xTaskIncrementTick+0x330>
6030f7c4:	e289b004 	add	fp, r9, #4
6030f7c8:	e1a0000b 	mov	r0, fp
6030f7cc:	eb00037b 	bl	603105c0 <uxListRemove>
6030f7d0:	e5993028 	ldr	r3, [r9, #40]	; 0x28
6030f7d4:	e3530000 	cmp	r3, #0
6030f7d8:	0a000001 	beq	6030f7e4 <xTaskIncrementTick+0x250>
6030f7dc:	e2890018 	add	r0, r9, #24
6030f7e0:	eb000376 	bl	603105c0 <uxListRemove>
6030f7e4:	e5993034 	ldr	r3, [r9, #52]	; 0x34
6030f7e8:	e3730002 	cmn	r3, #2
6030f7ec:	0a00001e 	beq	6030f86c <xTaskIncrementTick+0x2d8>
6030f7f0:	e599002c 	ldr	r0, [r9, #44]	; 0x2c
6030f7f4:	e59a3000 	ldr	r3, [sl]
6030f7f8:	e1a0100b 	mov	r1, fp
6030f7fc:	e1500003 	cmp	r0, r3
6030f800:	e59d3004 	ldr	r3, [sp, #4]
6030f804:	858a0000 	strhi	r0, [sl]
6030f808:	e0203098 	mla	r0, r8, r0, r3
6030f80c:	eb00034a 	bl	6031053c <vListInsertEnd>
6030f810:	e5952000 	ldr	r2, [r5]
6030f814:	e599302c 	ldr	r3, [r9, #44]	; 0x2c
6030f818:	e592202c 	ldr	r2, [r2, #44]	; 0x2c
6030f81c:	e1530002 	cmp	r3, r2
6030f820:	2affffd6 	bcs	6030f780 <xTaskIncrementTick+0x1ec>
6030f824:	e5952004 	ldr	r2, [r5, #4]
6030f828:	e592202c 	ldr	r2, [r2, #44]	; 0x2c
6030f82c:	e1530002 	cmp	r3, r2
6030f830:	3a000004 	bcc	6030f848 <xTaskIncrementTick+0x2b4>
6030f834:	e59d3000 	ldr	r3, [sp]
6030f838:	e7963103 	ldr	r3, [r6, r3, lsl #2]
6030f83c:	e3530000 	cmp	r3, #0
6030f840:	03a02001 	moveq	r2, #1
6030f844:	0affffd1 	beq	6030f790 <xTaskIncrementTick+0x1fc>
6030f848:	e5943000 	ldr	r3, [r4]
6030f84c:	e5933000 	ldr	r3, [r3]
6030f850:	e3530000 	cmp	r3, #0
6030f854:	1affffd4 	bne	6030f7ac <xTaskIncrementTick+0x218>
6030f858:	e59d2008 	ldr	r2, [sp, #8]
6030f85c:	e3e03000 	mvn	r3, #0
6030f860:	e59d9004 	ldr	r9, [sp, #4]
6030f864:	e5823000 	str	r3, [r2]
6030f868:	eaffff67 	b	6030f60c <xTaskIncrementTick+0x78>
6030f86c:	e59d300c 	ldr	r3, [sp, #12]
6030f870:	e1a0100b 	mov	r1, fp
6030f874:	e5930000 	ldr	r0, [r3]
6030f878:	eb00032f 	bl	6031053c <vListInsertEnd>
6030f87c:	eaffffe3 	b	6030f810 <xTaskIncrementTick+0x27c>
6030f880:	e5943000 	ldr	r3, [r4]
6030f884:	e3082134 	movw	r2, #33076	; 0x8134
6030f888:	e3462031 	movt	r2, #24625	; 0x6031
6030f88c:	e593300c 	ldr	r3, [r3, #12]
6030f890:	e58d2008 	str	r2, [sp, #8]
6030f894:	e593300c 	ldr	r3, [r3, #12]
6030f898:	e5933004 	ldr	r3, [r3, #4]
6030f89c:	e5823000 	str	r3, [r2]
6030f8a0:	eaffff4f 	b	6030f5e4 <xTaskIncrementTick+0x50>
6030f8a4:	e5963004 	ldr	r3, [r6, #4]
6030f8a8:	e3530000 	cmp	r3, #0
6030f8ac:	1a000008 	bne	6030f8d4 <xTaskIncrementTick+0x340>
6030f8b0:	e3a04001 	mov	r4, #1
6030f8b4:	ebfffcc8 	bl	6030ebdc <vTaskExitCritical>
6030f8b8:	e1a00004 	mov	r0, r4
6030f8bc:	e28dd014 	add	sp, sp, #20
6030f8c0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
6030f8c4:	e59d3008 	ldr	r3, [sp, #8]
6030f8c8:	e59d9004 	ldr	r9, [sp, #4]
6030f8cc:	e5832000 	str	r2, [r3]
6030f8d0:	eaffff4d 	b	6030f60c <xTaskIncrementTick+0x78>
6030f8d4:	e3a04001 	mov	r4, #1
6030f8d8:	eaffff6a 	b	6030f688 <xTaskIncrementTick+0xf4>
6030f8dc:	e3060850 	movw	r0, #26704	; 0x6850
6030f8e0:	e3460031 	movt	r0, #24625	; 0x6031
6030f8e4:	e3001bdd 	movw	r1, #3037	; 0xbdd
6030f8e8:	ebffe8cf 	bl	60309c2c <vAssertCalled>
6030f8ec:	eaffff7b 	b	6030f6e0 <xTaskIncrementTick+0x14c>

6030f8f0 <xTaskResumeAll.part.0>:
6030f8f0:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
6030f8f4:	e30840f8 	movw	r4, #33016	; 0x80f8
6030f8f8:	e3464031 	movt	r4, #24625	; 0x6031
6030f8fc:	e24dd00c 	sub	sp, sp, #12
6030f900:	ebfffc9a 	bl	6030eb70 <vTaskEnterCritical>
6030f904:	eb000ae5 	bl	603124a0 <ulPortGetCoreId>
6030f908:	e5943000 	ldr	r3, [r4]
6030f90c:	e1a05000 	mov	r5, r0
6030f910:	e3530000 	cmp	r3, #0
6030f914:	0a000061 	beq	6030faa0 <xTaskResumeAll.part.0+0x1b0>
6030f918:	e5943000 	ldr	r3, [r4]
6030f91c:	e2433001 	sub	r3, r3, #1
6030f920:	e5843000 	str	r3, [r4]
6030f924:	e5943000 	ldr	r3, [r4]
6030f928:	e3530000 	cmp	r3, #0
6030f92c:	13a04000 	movne	r4, #0
6030f930:	1a000056 	bne	6030fa90 <xTaskResumeAll.part.0+0x1a0>
6030f934:	e30830f0 	movw	r3, #33008	; 0x80f0
6030f938:	e3463031 	movt	r3, #24625	; 0x6031
6030f93c:	e5934000 	ldr	r4, [r3]
6030f940:	e3540000 	cmp	r4, #0
6030f944:	0a000051 	beq	6030fa90 <xTaskResumeAll.part.0+0x1a0>
6030f948:	e3086140 	movw	r6, #33088	; 0x8140
6030f94c:	e3466031 	movt	r6, #24625	; 0x6031
6030f950:	e5963000 	ldr	r3, [r6]
6030f954:	e3530000 	cmp	r3, #0
6030f958:	03089198 	movweq	r9, #33176	; 0x8198
6030f95c:	03469031 	movteq	r9, #24625	; 0x6031
6030f960:	0a00003b 	beq	6030fa54 <xTaskResumeAll.part.0+0x164>
6030f964:	e3078ff0 	movw	r8, #32752	; 0x7ff0
6030f968:	e3468031 	movt	r8, #24625	; 0x6031
6030f96c:	e3089198 	movw	r9, #33176	; 0x8198
6030f970:	e3469031 	movt	r9, #24625	; 0x6031
6030f974:	e3087100 	movw	r7, #33024	; 0x8100
6030f978:	e3467031 	movt	r7, #24625	; 0x6031
6030f97c:	e308a004 	movw	sl, #32772	; 0x8004
6030f980:	e346a031 	movt	sl, #24625	; 0x6031
6030f984:	e3083000 	movw	r3, #32768	; 0x8000
6030f988:	e3463031 	movt	r3, #24625	; 0x6031
6030f98c:	e58d3004 	str	r3, [sp, #4]
6030f990:	ea00000e 	b	6030f9d0 <xTaskResumeAll.part.0+0xe0>
6030f994:	e594302c 	ldr	r3, [r4, #44]	; 0x2c
6030f998:	e5972000 	ldr	r2, [r7]
6030f99c:	e1530002 	cmp	r3, r2
6030f9a0:	e020a390 	mla	r0, r0, r3, sl
6030f9a4:	85873000 	strhi	r3, [r7]
6030f9a8:	eb0002e3 	bl	6031053c <vListInsertEnd>
6030f9ac:	e7983105 	ldr	r3, [r8, r5, lsl #2]
6030f9b0:	e594202c 	ldr	r2, [r4, #44]	; 0x2c
6030f9b4:	e3a01001 	mov	r1, #1
6030f9b8:	e593302c 	ldr	r3, [r3, #44]	; 0x2c
6030f9bc:	e1520003 	cmp	r2, r3
6030f9c0:	27891105 	strcs	r1, [r9, r5, lsl #2]
6030f9c4:	e5963000 	ldr	r3, [r6]
6030f9c8:	e3530000 	cmp	r3, #0
6030f9cc:	0a00000f 	beq	6030fa10 <xTaskResumeAll.part.0+0x120>
6030f9d0:	e596300c 	ldr	r3, [r6, #12]
6030f9d4:	e593400c 	ldr	r4, [r3, #12]
6030f9d8:	e284b004 	add	fp, r4, #4
6030f9dc:	e2840018 	add	r0, r4, #24
6030f9e0:	eb0002f6 	bl	603105c0 <uxListRemove>
6030f9e4:	e1a0000b 	mov	r0, fp
6030f9e8:	eb0002f4 	bl	603105c0 <uxListRemove>
6030f9ec:	e5943034 	ldr	r3, [r4, #52]	; 0x34
6030f9f0:	e3a00014 	mov	r0, #20
6030f9f4:	e1a0100b 	mov	r1, fp
6030f9f8:	e3730002 	cmn	r3, #2
6030f9fc:	1affffe4 	bne	6030f994 <xTaskResumeAll.part.0+0xa4>
6030fa00:	e59d3004 	ldr	r3, [sp, #4]
6030fa04:	e5930000 	ldr	r0, [r3]
6030fa08:	eb0002cb 	bl	6031053c <vListInsertEnd>
6030fa0c:	eaffffe6 	b	6030f9ac <xTaskResumeAll.part.0+0xbc>
6030fa10:	e3540000 	cmp	r4, #0
6030fa14:	0a00000e 	beq	6030fa54 <xTaskResumeAll.part.0+0x164>
6030fa18:	e3073ff8 	movw	r3, #32760	; 0x7ff8
6030fa1c:	e3463031 	movt	r3, #24625	; 0x6031
6030fa20:	e5932000 	ldr	r2, [r3]
6030fa24:	e5922000 	ldr	r2, [r2]
6030fa28:	e3520000 	cmp	r2, #0
6030fa2c:	03083134 	movweq	r3, #33076	; 0x8134
6030fa30:	15932000 	ldrne	r2, [r3]
6030fa34:	03463031 	movteq	r3, #24625	; 0x6031
6030fa38:	13083134 	movwne	r3, #33076	; 0x8134
6030fa3c:	1592200c 	ldrne	r2, [r2, #12]
6030fa40:	13463031 	movtne	r3, #24625	; 0x6031
6030fa44:	03e02000 	mvneq	r2, #0
6030fa48:	1592200c 	ldrne	r2, [r2, #12]
6030fa4c:	15922004 	ldrne	r2, [r2, #4]
6030fa50:	e5832000 	str	r2, [r3]
6030fa54:	e308613c 	movw	r6, #33084	; 0x813c
6030fa58:	e3466031 	movt	r6, #24625	; 0x6031
6030fa5c:	e5964000 	ldr	r4, [r6]
6030fa60:	e3540000 	cmp	r4, #0
6030fa64:	0a000006 	beq	6030fa84 <xTaskResumeAll.part.0+0x194>
6030fa68:	e3a07001 	mov	r7, #1
6030fa6c:	ebfffec8 	bl	6030f594 <xTaskIncrementTick>
6030fa70:	e3500000 	cmp	r0, #0
6030fa74:	17897105 	strne	r7, [r9, r5, lsl #2]
6030fa78:	e2544001 	subs	r4, r4, #1
6030fa7c:	1afffffa 	bne	6030fa6c <xTaskResumeAll.part.0+0x17c>
6030fa80:	e5864000 	str	r4, [r6]
6030fa84:	e7994105 	ldr	r4, [r9, r5, lsl #2]
6030fa88:	e2544000 	subs	r4, r4, #0
6030fa8c:	13a04001 	movne	r4, #1
6030fa90:	ebfffc51 	bl	6030ebdc <vTaskExitCritical>
6030fa94:	e1a00004 	mov	r0, r4
6030fa98:	e28dd00c 	add	sp, sp, #12
6030fa9c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
6030faa0:	e3a01e99 	mov	r1, #2448	; 0x990
6030faa4:	e3060850 	movw	r0, #26704	; 0x6850
6030faa8:	e3460031 	movt	r0, #24625	; 0x6031
6030faac:	ebffe85e 	bl	60309c2c <vAssertCalled>
6030fab0:	eaffff98 	b	6030f918 <xTaskResumeAll.part.0+0x28>

6030fab4 <xTaskResumeAll>:
6030fab4:	e3083154 	movw	r3, #33108	; 0x8154
6030fab8:	e3463031 	movt	r3, #24625	; 0x6031
6030fabc:	e5930000 	ldr	r0, [r3]
6030fac0:	e3500000 	cmp	r0, #0
6030fac4:	012fff1e 	bxeq	lr
6030fac8:	eaffff88 	b	6030f8f0 <xTaskResumeAll.part.0>

6030facc <vTaskPlaceOnEventList>:
6030facc:	e92d4070 	push	{r4, r5, r6, lr}
6030fad0:	e1a05001 	mov	r5, r1
6030fad4:	e2504000 	subs	r4, r0, #0
6030fad8:	0a000011 	beq	6030fb24 <vTaskPlaceOnEventList+0x58>
6030fadc:	ebfffc23 	bl	6030eb70 <vTaskEnterCritical>
6030fae0:	eb000a53 	bl	60312434 <ulPortInterruptLock>
6030fae4:	e1a06000 	mov	r6, r0
6030fae8:	eb000a6c 	bl	603124a0 <ulPortGetCoreId>
6030faec:	e3073ff0 	movw	r3, #32752	; 0x7ff0
6030faf0:	e3463031 	movt	r3, #24625	; 0x6031
6030faf4:	e1a02000 	mov	r2, r0
6030faf8:	e1a00006 	mov	r0, r6
6030fafc:	e7936102 	ldr	r6, [r3, r2, lsl #2]
6030fb00:	eb000a50 	bl	60312448 <ulPortInterruptUnLock>
6030fb04:	e2861018 	add	r1, r6, #24
6030fb08:	e1a00004 	mov	r0, r4
6030fb0c:	eb000295 	bl	60310568 <vListInsert>
6030fb10:	e1a00005 	mov	r0, r5
6030fb14:	e3a01001 	mov	r1, #1
6030fb18:	ebfffe23 	bl	6030f3ac <prvAddCurrentTaskToDelayedList>
6030fb1c:	e8bd4070 	pop	{r4, r5, r6, lr}
6030fb20:	eafffc2d 	b	6030ebdc <vTaskExitCritical>
6030fb24:	e3060850 	movw	r0, #26704	; 0x6850
6030fb28:	e3460031 	movt	r0, #24625	; 0x6031
6030fb2c:	e3001da2 	movw	r1, #3490	; 0xda2
6030fb30:	ebffe83d 	bl	60309c2c <vAssertCalled>
6030fb34:	eaffffe8 	b	6030fadc <vTaskPlaceOnEventList+0x10>

6030fb38 <vTaskPlaceOnEventListRestricted>:
6030fb38:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
6030fb3c:	e1a06001 	mov	r6, r1
6030fb40:	e1a04002 	mov	r4, r2
6030fb44:	e2505000 	subs	r5, r0, #0
6030fb48:	0a000013 	beq	6030fb9c <vTaskPlaceOnEventListRestricted+0x64>
6030fb4c:	ebfffc07 	bl	6030eb70 <vTaskEnterCritical>
6030fb50:	eb000a37 	bl	60312434 <ulPortInterruptLock>
6030fb54:	e1a07000 	mov	r7, r0
6030fb58:	eb000a50 	bl	603124a0 <ulPortGetCoreId>
6030fb5c:	e3073ff0 	movw	r3, #32752	; 0x7ff0
6030fb60:	e3463031 	movt	r3, #24625	; 0x6031
6030fb64:	e1a02000 	mov	r2, r0
6030fb68:	e1a00007 	mov	r0, r7
6030fb6c:	e7937102 	ldr	r7, [r3, r2, lsl #2]
6030fb70:	eb000a34 	bl	60312448 <ulPortInterruptUnLock>
6030fb74:	e1a00005 	mov	r0, r5
6030fb78:	e2871018 	add	r1, r7, #24
6030fb7c:	eb00026e 	bl	6031053c <vListInsertEnd>
6030fb80:	e3540000 	cmp	r4, #0
6030fb84:	e1a01004 	mov	r1, r4
6030fb88:	01a00006 	moveq	r0, r6
6030fb8c:	13e00000 	mvnne	r0, #0
6030fb90:	ebfffe05 	bl	6030f3ac <prvAddCurrentTaskToDelayedList>
6030fb94:	e8bd41f0 	pop	{r4, r5, r6, r7, r8, lr}
6030fb98:	eafffc0f 	b	6030ebdc <vTaskExitCritical>
6030fb9c:	e3060850 	movw	r0, #26704	; 0x6850
6030fba0:	e3460031 	movt	r0, #24625	; 0x6031
6030fba4:	e3001dd5 	movw	r1, #3541	; 0xdd5
6030fba8:	ebffe81f 	bl	60309c2c <vAssertCalled>
6030fbac:	eaffffe6 	b	6030fb4c <vTaskPlaceOnEventListRestricted+0x14>

6030fbb0 <vTaskPlaceRemoveFromEventList>:
6030fbb0:	e92d4070 	push	{r4, r5, r6, lr}
6030fbb4:	ebfffbed 	bl	6030eb70 <vTaskEnterCritical>
6030fbb8:	e3074ff0 	movw	r4, #32752	; 0x7ff0
6030fbbc:	e3464031 	movt	r4, #24625	; 0x6031
6030fbc0:	eb000a1b 	bl	60312434 <ulPortInterruptLock>
6030fbc4:	e1a05000 	mov	r5, r0
6030fbc8:	eb000a34 	bl	603124a0 <ulPortGetCoreId>
6030fbcc:	e1a03000 	mov	r3, r0
6030fbd0:	e1a00005 	mov	r0, r5
6030fbd4:	e7945103 	ldr	r5, [r4, r3, lsl #2]
6030fbd8:	eb000a1a 	bl	60312448 <ulPortInterruptUnLock>
6030fbdc:	e2850018 	add	r0, r5, #24
6030fbe0:	eb000276 	bl	603105c0 <uxListRemove>
6030fbe4:	eb000a12 	bl	60312434 <ulPortInterruptLock>
6030fbe8:	e1a05000 	mov	r5, r0
6030fbec:	eb000a2b 	bl	603124a0 <ulPortGetCoreId>
6030fbf0:	e1a03000 	mov	r3, r0
6030fbf4:	e1a00005 	mov	r0, r5
6030fbf8:	e7945103 	ldr	r5, [r4, r3, lsl #2]
6030fbfc:	eb000a11 	bl	60312448 <ulPortInterruptUnLock>
6030fc00:	e2850004 	add	r0, r5, #4
6030fc04:	eb00026d 	bl	603105c0 <uxListRemove>
6030fc08:	eb000a24 	bl	603124a0 <ulPortGetCoreId>
6030fc0c:	e1a05000 	mov	r5, r0
6030fc10:	eb000a07 	bl	60312434 <ulPortInterruptLock>
6030fc14:	e1a06000 	mov	r6, r0
6030fc18:	eb000a20 	bl	603124a0 <ulPortGetCoreId>
6030fc1c:	e1a03000 	mov	r3, r0
6030fc20:	e1a00006 	mov	r0, r6
6030fc24:	e7944103 	ldr	r4, [r4, r3, lsl #2]
6030fc28:	eb000a06 	bl	60312448 <ulPortInterruptUnLock>
6030fc2c:	e5845034 	str	r5, [r4, #52]	; 0x34
6030fc30:	e8bd4070 	pop	{r4, r5, r6, lr}
6030fc34:	eafffbe8 	b	6030ebdc <vTaskExitCritical>

6030fc38 <xTaskRemoveFromEventList>:
6030fc38:	e92d4070 	push	{r4, r5, r6, lr}
6030fc3c:	e1a04000 	mov	r4, r0
6030fc40:	ebfffbca 	bl	6030eb70 <vTaskEnterCritical>
6030fc44:	e594300c 	ldr	r3, [r4, #12]
6030fc48:	e593400c 	ldr	r4, [r3, #12]
6030fc4c:	e3540000 	cmp	r4, #0
6030fc50:	0a00003f 	beq	6030fd54 <xTaskRemoveFromEventList+0x11c>
6030fc54:	e2845018 	add	r5, r4, #24
6030fc58:	e1a00005 	mov	r0, r5
6030fc5c:	eb000257 	bl	603105c0 <uxListRemove>
6030fc60:	e30830f8 	movw	r3, #33016	; 0x80f8
6030fc64:	e3463031 	movt	r3, #24625	; 0x6031
6030fc68:	e5933000 	ldr	r3, [r3]
6030fc6c:	e3530000 	cmp	r3, #0
6030fc70:	1a000021 	bne	6030fcfc <xTaskRemoveFromEventList+0xc4>
6030fc74:	e2845004 	add	r5, r4, #4
6030fc78:	e1a00005 	mov	r0, r5
6030fc7c:	eb00024f 	bl	603105c0 <uxListRemove>
6030fc80:	e5943034 	ldr	r3, [r4, #52]	; 0x34
6030fc84:	e3730002 	cmn	r3, #2
6030fc88:	0a00002b 	beq	6030fd3c <xTaskRemoveFromEventList+0x104>
6030fc8c:	e3083100 	movw	r3, #33024	; 0x8100
6030fc90:	e3463031 	movt	r3, #24625	; 0x6031
6030fc94:	e594202c 	ldr	r2, [r4, #44]	; 0x2c
6030fc98:	e3a00014 	mov	r0, #20
6030fc9c:	e5931000 	ldr	r1, [r3]
6030fca0:	e1520001 	cmp	r2, r1
6030fca4:	85832000 	strhi	r2, [r3]
6030fca8:	e3083004 	movw	r3, #32772	; 0x8004
6030fcac:	e3463031 	movt	r3, #24625	; 0x6031
6030fcb0:	e1a01005 	mov	r1, r5
6030fcb4:	e0203290 	mla	r0, r0, r2, r3
6030fcb8:	eb00021f 	bl	6031053c <vListInsertEnd>
6030fcbc:	e3073ff8 	movw	r3, #32760	; 0x7ff8
6030fcc0:	e3463031 	movt	r3, #24625	; 0x6031
6030fcc4:	e5932000 	ldr	r2, [r3]
6030fcc8:	e5922000 	ldr	r2, [r2]
6030fccc:	e3520000 	cmp	r2, #0
6030fcd0:	03083134 	movweq	r3, #33076	; 0x8134
6030fcd4:	15932000 	ldrne	r2, [r3]
6030fcd8:	03463031 	movteq	r3, #24625	; 0x6031
6030fcdc:	13083134 	movwne	r3, #33076	; 0x8134
6030fce0:	1592200c 	ldrne	r2, [r2, #12]
6030fce4:	13463031 	movtne	r3, #24625	; 0x6031
6030fce8:	03e02000 	mvneq	r2, #0
6030fcec:	1592200c 	ldrne	r2, [r2, #12]
6030fcf0:	15922004 	ldrne	r2, [r2, #4]
6030fcf4:	e5832000 	str	r2, [r3]
6030fcf8:	ea000003 	b	6030fd0c <xTaskRemoveFromEventList+0xd4>
6030fcfc:	e3080140 	movw	r0, #33088	; 0x8140
6030fd00:	e3460031 	movt	r0, #24625	; 0x6031
6030fd04:	e1a01005 	mov	r1, r5
6030fd08:	eb00020b 	bl	6031053c <vListInsertEnd>
6030fd0c:	e3a02000 	mov	r2, #0
6030fd10:	e3a01001 	mov	r1, #1
6030fd14:	e1a00004 	mov	r0, r4
6030fd18:	ebfff8b3 	bl	6030dfec <prvYieldForTask.isra.0>
6030fd1c:	eb0009df 	bl	603124a0 <ulPortGetCoreId>
6030fd20:	e3083198 	movw	r3, #33176	; 0x8198
6030fd24:	e3463031 	movt	r3, #24625	; 0x6031
6030fd28:	e7934100 	ldr	r4, [r3, r0, lsl #2]
6030fd2c:	ebfffbaa 	bl	6030ebdc <vTaskExitCritical>
6030fd30:	e2540000 	subs	r0, r4, #0
6030fd34:	13a00001 	movne	r0, #1
6030fd38:	e8bd8070 	pop	{r4, r5, r6, pc}
6030fd3c:	e3083000 	movw	r3, #32768	; 0x8000
6030fd40:	e3463031 	movt	r3, #24625	; 0x6031
6030fd44:	e1a01005 	mov	r1, r5
6030fd48:	e5930000 	ldr	r0, [r3]
6030fd4c:	eb0001fa 	bl	6031053c <vListInsertEnd>
6030fd50:	eaffffd9 	b	6030fcbc <xTaskRemoveFromEventList+0x84>
6030fd54:	e3060850 	movw	r0, #26704	; 0x6850
6030fd58:	e3460031 	movt	r0, #24625	; 0x6031
6030fd5c:	e3001e1e 	movw	r1, #3614	; 0xe1e
6030fd60:	ebffe7b1 	bl	60309c2c <vAssertCalled>
6030fd64:	eaffffba 	b	6030fc54 <xTaskRemoveFromEventList+0x1c>

6030fd68 <xTaskCheckForTimeOut>:
6030fd68:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
6030fd6c:	e1a05001 	mov	r5, r1
6030fd70:	e2504000 	subs	r4, r0, #0
6030fd74:	0a000040 	beq	6030fe7c <xTaskCheckForTimeOut+0x114>
6030fd78:	e3550000 	cmp	r5, #0
6030fd7c:	0a000039 	beq	6030fe68 <xTaskCheckForTimeOut+0x100>
6030fd80:	e3087194 	movw	r7, #33172	; 0x8194
6030fd84:	e3467031 	movt	r7, #24625	; 0x6031
6030fd88:	ebfffb78 	bl	6030eb70 <vTaskEnterCritical>
6030fd8c:	e5949004 	ldr	r9, [r4, #4]
6030fd90:	e5978000 	ldr	r8, [r7]
6030fd94:	eb0009a6 	bl	60312434 <ulPortInterruptLock>
6030fd98:	e1a06000 	mov	r6, r0
6030fd9c:	eb0009bf 	bl	603124a0 <ulPortGetCoreId>
6030fda0:	e1a03000 	mov	r3, r0
6030fda4:	e1a00006 	mov	r0, r6
6030fda8:	e3076ff0 	movw	r6, #32752	; 0x7ff0
6030fdac:	e3466031 	movt	r6, #24625	; 0x6031
6030fdb0:	e796a103 	ldr	sl, [r6, r3, lsl #2]
6030fdb4:	eb0009a3 	bl	60312448 <ulPortInterruptUnLock>
6030fdb8:	e5da0172 	ldrb	r0, [sl, #370]	; 0x172
6030fdbc:	e3500000 	cmp	r0, #0
6030fdc0:	1a00001d 	bne	6030fe3c <xTaskCheckForTimeOut+0xd4>
6030fdc4:	e5953000 	ldr	r3, [r5]
6030fdc8:	e3730001 	cmn	r3, #1
6030fdcc:	01a07000 	moveq	r7, r0
6030fdd0:	0a000009 	beq	6030fdfc <xTaskCheckForTimeOut+0x94>
6030fdd4:	e3082138 	movw	r2, #33080	; 0x8138
6030fdd8:	e3462031 	movt	r2, #24625	; 0x6031
6030fddc:	e5940000 	ldr	r0, [r4]
6030fde0:	e5921000 	ldr	r1, [r2]
6030fde4:	e1500001 	cmp	r0, r1
6030fde8:	0a000006 	beq	6030fe08 <xTaskCheckForTimeOut+0xa0>
6030fdec:	e5941004 	ldr	r1, [r4, #4]
6030fdf0:	e1510008 	cmp	r1, r8
6030fdf4:	93a07001 	movls	r7, #1
6030fdf8:	8a000002 	bhi	6030fe08 <xTaskCheckForTimeOut+0xa0>
6030fdfc:	ebfffb76 	bl	6030ebdc <vTaskExitCritical>
6030fe00:	e1a00007 	mov	r0, r7
6030fe04:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
6030fe08:	e0481009 	sub	r1, r8, r9
6030fe0c:	e1530001 	cmp	r3, r1
6030fe10:	80433008 	subhi	r3, r3, r8
6030fe14:	80839009 	addhi	r9, r3, r9
6030fe18:	85922000 	ldrhi	r2, [r2]
6030fe1c:	85973000 	ldrhi	r3, [r7]
6030fe20:	93a03000 	movls	r3, #0
6030fe24:	85859000 	strhi	r9, [r5]
6030fe28:	83a07000 	movhi	r7, #0
6030fe2c:	93a07001 	movls	r7, #1
6030fe30:	81c420f0 	strdhi	r2, [r4]
6030fe34:	95853000 	strls	r3, [r5]
6030fe38:	eaffffef 	b	6030fdfc <xTaskCheckForTimeOut+0x94>
6030fe3c:	eb00097c 	bl	60312434 <ulPortInterruptLock>
6030fe40:	e1a04000 	mov	r4, r0
6030fe44:	eb000995 	bl	603124a0 <ulPortGetCoreId>
6030fe48:	e1a03000 	mov	r3, r0
6030fe4c:	e1a00004 	mov	r0, r4
6030fe50:	e3a07001 	mov	r7, #1
6030fe54:	e7964103 	ldr	r4, [r6, r3, lsl #2]
6030fe58:	eb00097a 	bl	60312448 <ulPortInterruptUnLock>
6030fe5c:	e3a03000 	mov	r3, #0
6030fe60:	e5c43172 	strb	r3, [r4, #370]	; 0x172
6030fe64:	eaffffe4 	b	6030fdfc <xTaskCheckForTimeOut+0x94>
6030fe68:	e3060850 	movw	r0, #26704	; 0x6850
6030fe6c:	e3460031 	movt	r0, #24625	; 0x6031
6030fe70:	e3001e7e 	movw	r1, #3710	; 0xe7e
6030fe74:	ebffe76c 	bl	60309c2c <vAssertCalled>
6030fe78:	eaffffc0 	b	6030fd80 <xTaskCheckForTimeOut+0x18>
6030fe7c:	e3060850 	movw	r0, #26704	; 0x6850
6030fe80:	e3460031 	movt	r0, #24625	; 0x6031
6030fe84:	e3001e7d 	movw	r1, #3709	; 0xe7d
6030fe88:	ebffe767 	bl	60309c2c <vAssertCalled>
6030fe8c:	eaffffb9 	b	6030fd78 <xTaskCheckForTimeOut+0x10>

6030fe90 <xTaskPriorityInherit>:
6030fe90:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
6030fe94:	e2504000 	subs	r4, r0, #0
6030fe98:	0a00002e 	beq	6030ff58 <xTaskPriorityInherit+0xc8>
6030fe9c:	e594702c 	ldr	r7, [r4, #44]	; 0x2c
6030fea0:	e3075ff0 	movw	r5, #32752	; 0x7ff0
6030fea4:	e3465031 	movt	r5, #24625	; 0x6031
6030fea8:	eb000961 	bl	60312434 <ulPortInterruptLock>
6030feac:	e1a06000 	mov	r6, r0
6030feb0:	eb00097a 	bl	603124a0 <ulPortGetCoreId>
6030feb4:	e1a03000 	mov	r3, r0
6030feb8:	e1a00006 	mov	r0, r6
6030febc:	e7956103 	ldr	r6, [r5, r3, lsl #2]
6030fec0:	eb000960 	bl	60312448 <ulPortInterruptUnLock>
6030fec4:	e596302c 	ldr	r3, [r6, #44]	; 0x2c
6030fec8:	e1570003 	cmp	r7, r3
6030fecc:	2a000023 	bcs	6030ff60 <xTaskPriorityInherit+0xd0>
6030fed0:	e5943018 	ldr	r3, [r4, #24]
6030fed4:	e3530000 	cmp	r3, #0
6030fed8:	ba000009 	blt	6030ff04 <xTaskPriorityInherit+0x74>
6030fedc:	eb000954 	bl	60312434 <ulPortInterruptLock>
6030fee0:	e1a06000 	mov	r6, r0
6030fee4:	eb00096d 	bl	603124a0 <ulPortGetCoreId>
6030fee8:	e1a03000 	mov	r3, r0
6030feec:	e1a00006 	mov	r0, r6
6030fef0:	e7956103 	ldr	r6, [r5, r3, lsl #2]
6030fef4:	eb000953 	bl	60312448 <ulPortInterruptUnLock>
6030fef8:	e596302c 	ldr	r3, [r6, #44]	; 0x2c
6030fefc:	e263300b 	rsb	r3, r3, #11
6030ff00:	e5843018 	str	r3, [r4, #24]
6030ff04:	ebfffb19 	bl	6030eb70 <vTaskEnterCritical>
6030ff08:	e594302c 	ldr	r3, [r4, #44]	; 0x2c
6030ff0c:	e3a02014 	mov	r2, #20
6030ff10:	e3086004 	movw	r6, #32772	; 0x8004
6030ff14:	e3466031 	movt	r6, #24625	; 0x6031
6030ff18:	e0236392 	mla	r3, r2, r3, r6
6030ff1c:	e5942014 	ldr	r2, [r4, #20]
6030ff20:	e1520003 	cmp	r2, r3
6030ff24:	0a00001a 	beq	6030ff94 <xTaskPriorityInherit+0x104>
6030ff28:	eb000941 	bl	60312434 <ulPortInterruptLock>
6030ff2c:	e1a06000 	mov	r6, r0
6030ff30:	eb00095a 	bl	603124a0 <ulPortGetCoreId>
6030ff34:	e1a03000 	mov	r3, r0
6030ff38:	e1a00006 	mov	r0, r6
6030ff3c:	e7955103 	ldr	r5, [r5, r3, lsl #2]
6030ff40:	eb000940 	bl	60312448 <ulPortInterruptUnLock>
6030ff44:	e595302c 	ldr	r3, [r5, #44]	; 0x2c
6030ff48:	e584302c 	str	r3, [r4, #44]	; 0x2c
6030ff4c:	ebfffb22 	bl	6030ebdc <vTaskExitCritical>
6030ff50:	e3a00001 	mov	r0, #1
6030ff54:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
6030ff58:	e1a00004 	mov	r0, r4
6030ff5c:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
6030ff60:	e5944064 	ldr	r4, [r4, #100]	; 0x64
6030ff64:	eb000932 	bl	60312434 <ulPortInterruptLock>
6030ff68:	e1a06000 	mov	r6, r0
6030ff6c:	eb00094b 	bl	603124a0 <ulPortGetCoreId>
6030ff70:	e1a03000 	mov	r3, r0
6030ff74:	e1a00006 	mov	r0, r6
6030ff78:	e7955103 	ldr	r5, [r5, r3, lsl #2]
6030ff7c:	eb000931 	bl	60312448 <ulPortInterruptUnLock>
6030ff80:	e595002c 	ldr	r0, [r5, #44]	; 0x2c
6030ff84:	e1540000 	cmp	r4, r0
6030ff88:	23a00000 	movcs	r0, #0
6030ff8c:	33a00001 	movcc	r0, #1
6030ff90:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
6030ff94:	e2848004 	add	r8, r4, #4
6030ff98:	e1a00008 	mov	r0, r8
6030ff9c:	eb000187 	bl	603105c0 <uxListRemove>
6030ffa0:	eb000923 	bl	60312434 <ulPortInterruptLock>
6030ffa4:	e1a07000 	mov	r7, r0
6030ffa8:	eb00093c 	bl	603124a0 <ulPortGetCoreId>
6030ffac:	e7955100 	ldr	r5, [r5, r0, lsl #2]
6030ffb0:	e1a00007 	mov	r0, r7
6030ffb4:	eb000923 	bl	60312448 <ulPortInterruptUnLock>
6030ffb8:	e5942034 	ldr	r2, [r4, #52]	; 0x34
6030ffbc:	e595302c 	ldr	r3, [r5, #44]	; 0x2c
6030ffc0:	e3720002 	cmn	r2, #2
6030ffc4:	e584302c 	str	r3, [r4, #44]	; 0x2c
6030ffc8:	0a000009 	beq	6030fff4 <xTaskPriorityInherit+0x164>
6030ffcc:	e3082100 	movw	r2, #33024	; 0x8100
6030ffd0:	e3462031 	movt	r2, #24625	; 0x6031
6030ffd4:	e3a00014 	mov	r0, #20
6030ffd8:	e5921000 	ldr	r1, [r2]
6030ffdc:	e0206390 	mla	r0, r0, r3, r6
6030ffe0:	e1530001 	cmp	r3, r1
6030ffe4:	e1a01008 	mov	r1, r8
6030ffe8:	85823000 	strhi	r3, [r2]
6030ffec:	eb000152 	bl	6031053c <vListInsertEnd>
6030fff0:	eaffffd5 	b	6030ff4c <xTaskPriorityInherit+0xbc>
6030fff4:	e3083000 	movw	r3, #32768	; 0x8000
6030fff8:	e3463031 	movt	r3, #24625	; 0x6031
6030fffc:	e1a01008 	mov	r1, r8
60310000:	e5930000 	ldr	r0, [r3]
60310004:	eb00014c 	bl	6031053c <vListInsertEnd>
60310008:	eaffffcf 	b	6030ff4c <xTaskPriorityInherit+0xbc>

6031000c <xTaskPriorityDisinherit>:
6031000c:	e92d4070 	push	{r4, r5, r6, lr}
60310010:	e2504000 	subs	r4, r0, #0
60310014:	0a000013 	beq	60310068 <xTaskPriorityDisinherit+0x5c>
60310018:	eb000905 	bl	60312434 <ulPortInterruptLock>
6031001c:	e1a05000 	mov	r5, r0
60310020:	eb00091e 	bl	603124a0 <ulPortGetCoreId>
60310024:	e3073ff0 	movw	r3, #32752	; 0x7ff0
60310028:	e3463031 	movt	r3, #24625	; 0x6031
6031002c:	e1a02000 	mov	r2, r0
60310030:	e1a00005 	mov	r0, r5
60310034:	e7935102 	ldr	r5, [r3, r2, lsl #2]
60310038:	eb000902 	bl	60312448 <ulPortInterruptUnLock>
6031003c:	e1540005 	cmp	r4, r5
60310040:	1a00001e 	bne	603100c0 <xTaskPriorityDisinherit+0xb4>
60310044:	e5943068 	ldr	r3, [r4, #104]	; 0x68
60310048:	e3530000 	cmp	r3, #0
6031004c:	0a000020 	beq	603100d4 <xTaskPriorityDisinherit+0xc8>
60310050:	e594102c 	ldr	r1, [r4, #44]	; 0x2c
60310054:	e5942064 	ldr	r2, [r4, #100]	; 0x64
60310058:	e2433001 	sub	r3, r3, #1
6031005c:	e1510002 	cmp	r1, r2
60310060:	e5843068 	str	r3, [r4, #104]	; 0x68
60310064:	1a000002 	bne	60310074 <xTaskPriorityDisinherit+0x68>
60310068:	e3a05000 	mov	r5, #0
6031006c:	e1a00005 	mov	r0, r5
60310070:	e8bd8070 	pop	{r4, r5, r6, pc}
60310074:	ebfffabd 	bl	6030eb70 <vTaskEnterCritical>
60310078:	e5943068 	ldr	r3, [r4, #104]	; 0x68
6031007c:	e3530000 	cmp	r3, #0
60310080:	13a05000 	movne	r5, #0
60310084:	1a00000a 	bne	603100b4 <xTaskPriorityDisinherit+0xa8>
60310088:	e3073ff8 	movw	r3, #32760	; 0x7ff8
6031008c:	e3463031 	movt	r3, #24625	; 0x6031
60310090:	e5942014 	ldr	r2, [r4, #20]
60310094:	e5933000 	ldr	r3, [r3]
60310098:	e1520003 	cmp	r2, r3
6031009c:	0a000012 	beq	603100ec <xTaskPriorityDisinherit+0xe0>
603100a0:	e5943064 	ldr	r3, [r4, #100]	; 0x64
603100a4:	e3a05001 	mov	r5, #1
603100a8:	e263200b 	rsb	r2, r3, #11
603100ac:	e584302c 	str	r3, [r4, #44]	; 0x2c
603100b0:	e5842018 	str	r2, [r4, #24]
603100b4:	ebfffac8 	bl	6030ebdc <vTaskExitCritical>
603100b8:	e1a00005 	mov	r0, r5
603100bc:	e8bd8070 	pop	{r4, r5, r6, pc}
603100c0:	e3060850 	movw	r0, #26704	; 0x6850
603100c4:	e3460031 	movt	r0, #24625	; 0x6031
603100c8:	e30111a8 	movw	r1, #4520	; 0x11a8
603100cc:	ebffe6d6 	bl	60309c2c <vAssertCalled>
603100d0:	eaffffdb 	b	60310044 <xTaskPriorityDisinherit+0x38>
603100d4:	e30111a9 	movw	r1, #4521	; 0x11a9
603100d8:	e3060850 	movw	r0, #26704	; 0x6850
603100dc:	e3460031 	movt	r0, #24625	; 0x6031
603100e0:	ebffe6d1 	bl	60309c2c <vAssertCalled>
603100e4:	e5943068 	ldr	r3, [r4, #104]	; 0x68
603100e8:	eaffffd8 	b	60310050 <xTaskPriorityDisinherit+0x44>
603100ec:	e2840004 	add	r0, r4, #4
603100f0:	eb000132 	bl	603105c0 <uxListRemove>
603100f4:	e3500000 	cmp	r0, #0
603100f8:	1affffe8 	bne	603100a0 <xTaskPriorityDisinherit+0x94>
603100fc:	e308c100 	movw	ip, #33024	; 0x8100
60310100:	e346c031 	movt	ip, #24625	; 0x6031
60310104:	e59c3000 	ldr	r3, [ip]
60310108:	e3530000 	cmp	r3, #0
6031010c:	0a000009 	beq	60310138 <xTaskPriorityDisinherit+0x12c>
60310110:	e3081004 	movw	r1, #32772	; 0x8004
60310114:	e3461031 	movt	r1, #24625	; 0x6031
60310118:	e3a00014 	mov	r0, #20
6031011c:	ea000001 	b	60310128 <xTaskPriorityDisinherit+0x11c>
60310120:	e2533001 	subs	r3, r3, #1
60310124:	0a000003 	beq	60310138 <xTaskPriorityDisinherit+0x12c>
60310128:	e0020390 	mul	r2, r0, r3
6031012c:	e7912002 	ldr	r2, [r1, r2]
60310130:	e3520000 	cmp	r2, #0
60310134:	0afffff9 	beq	60310120 <xTaskPriorityDisinherit+0x114>
60310138:	e58c3000 	str	r3, [ip]
6031013c:	eaffffd7 	b	603100a0 <xTaskPriorityDisinherit+0x94>

60310140 <vTaskPriorityDisinheritAfterTimeout>:
60310140:	e92d4070 	push	{r4, r5, r6, lr}
60310144:	e2504000 	subs	r4, r0, #0
60310148:	08bd8070 	popeq	{r4, r5, r6, pc}
6031014c:	e5943068 	ldr	r3, [r4, #104]	; 0x68
60310150:	e1a05001 	mov	r5, r1
60310154:	e3530000 	cmp	r3, #0
60310158:	0a000023 	beq	603101ec <vTaskPriorityDisinheritAfterTimeout+0xac>
6031015c:	e5941064 	ldr	r1, [r4, #100]	; 0x64
60310160:	e594302c 	ldr	r3, [r4, #44]	; 0x2c
60310164:	e1550001 	cmp	r5, r1
60310168:	31a05001 	movcc	r5, r1
6031016c:	e1530005 	cmp	r3, r5
60310170:	08bd8070 	popeq	{r4, r5, r6, pc}
60310174:	e5943068 	ldr	r3, [r4, #104]	; 0x68
60310178:	e3530001 	cmp	r3, #1
6031017c:	18bd8070 	popne	{r4, r5, r6, pc}
60310180:	eb0008ab 	bl	60312434 <ulPortInterruptLock>
60310184:	e1a06000 	mov	r6, r0
60310188:	eb0008c4 	bl	603124a0 <ulPortGetCoreId>
6031018c:	e3073ff0 	movw	r3, #32752	; 0x7ff0
60310190:	e3463031 	movt	r3, #24625	; 0x6031
60310194:	e1a02000 	mov	r2, r0
60310198:	e1a00006 	mov	r0, r6
6031019c:	e7936102 	ldr	r6, [r3, r2, lsl #2]
603101a0:	eb0008a8 	bl	60312448 <ulPortInterruptUnLock>
603101a4:	e1540006 	cmp	r4, r6
603101a8:	0a00002b 	beq	6031025c <vTaskPriorityDisinheritAfterTimeout+0x11c>
603101ac:	e5943018 	ldr	r3, [r4, #24]
603101b0:	e594602c 	ldr	r6, [r4, #44]	; 0x2c
603101b4:	e3530000 	cmp	r3, #0
603101b8:	e584502c 	str	r5, [r4, #44]	; 0x2c
603101bc:	a265500b 	rsbge	r5, r5, #11
603101c0:	a5845018 	strge	r5, [r4, #24]
603101c4:	ebfffa69 	bl	6030eb70 <vTaskEnterCritical>
603101c8:	e3085004 	movw	r5, #32772	; 0x8004
603101cc:	e3465031 	movt	r5, #24625	; 0x6031
603101d0:	e3a02014 	mov	r2, #20
603101d4:	e5943014 	ldr	r3, [r4, #20]
603101d8:	e0265692 	mla	r6, r2, r6, r5
603101dc:	e1530006 	cmp	r3, r6
603101e0:	0a000006 	beq	60310200 <vTaskPriorityDisinheritAfterTimeout+0xc0>
603101e4:	e8bd4070 	pop	{r4, r5, r6, lr}
603101e8:	eafffa7b 	b	6030ebdc <vTaskExitCritical>
603101ec:	e3060850 	movw	r0, #26704	; 0x6850
603101f0:	e3460031 	movt	r0, #24625	; 0x6031
603101f4:	e30111f5 	movw	r1, #4597	; 0x11f5
603101f8:	ebffe68b 	bl	60309c2c <vAssertCalled>
603101fc:	eaffffd6 	b	6031015c <vTaskPriorityDisinheritAfterTimeout+0x1c>
60310200:	e2846004 	add	r6, r4, #4
60310204:	e1a00006 	mov	r0, r6
60310208:	eb0000ec 	bl	603105c0 <uxListRemove>
6031020c:	e5943034 	ldr	r3, [r4, #52]	; 0x34
60310210:	e3530000 	cmp	r3, #0
60310214:	ba000002 	blt	60310224 <vTaskPriorityDisinheritAfterTimeout+0xe4>
60310218:	e5943034 	ldr	r3, [r4, #52]	; 0x34
6031021c:	e3530001 	cmp	r3, #1
60310220:	daffffef 	ble	603101e4 <vTaskPriorityDisinheritAfterTimeout+0xa4>
60310224:	e5943034 	ldr	r3, [r4, #52]	; 0x34
60310228:	e3730002 	cmn	r3, #2
6031022c:	0a00000f 	beq	60310270 <vTaskPriorityDisinheritAfterTimeout+0x130>
60310230:	e3083100 	movw	r3, #33024	; 0x8100
60310234:	e3463031 	movt	r3, #24625	; 0x6031
60310238:	e594202c 	ldr	r2, [r4, #44]	; 0x2c
6031023c:	e3a00014 	mov	r0, #20
60310240:	e5931000 	ldr	r1, [r3]
60310244:	e0205290 	mla	r0, r0, r2, r5
60310248:	e1520001 	cmp	r2, r1
6031024c:	e1a01006 	mov	r1, r6
60310250:	85832000 	strhi	r2, [r3]
60310254:	eb0000b8 	bl	6031053c <vListInsertEnd>
60310258:	eaffffe1 	b	603101e4 <vTaskPriorityDisinheritAfterTimeout+0xa4>
6031025c:	e3060850 	movw	r0, #26704	; 0x6850
60310260:	e3460031 	movt	r0, #24625	; 0x6031
60310264:	e301120b 	movw	r1, #4619	; 0x120b
60310268:	ebffe66f 	bl	60309c2c <vAssertCalled>
6031026c:	eaffffce 	b	603101ac <vTaskPriorityDisinheritAfterTimeout+0x6c>
60310270:	e3083000 	movw	r3, #32768	; 0x8000
60310274:	e3463031 	movt	r3, #24625	; 0x6031
60310278:	e1a01006 	mov	r1, r6
6031027c:	e5930000 	ldr	r0, [r3]
60310280:	eb0000ad 	bl	6031053c <vListInsertEnd>
60310284:	eaffffd6 	b	603101e4 <vTaskPriorityDisinheritAfterTimeout+0xa4>

60310288 <prvIdleTask>:
60310288:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
6031028c:	e30840f4 	movw	r4, #33012	; 0x80f4
60310290:	e3464031 	movt	r4, #24625	; 0x6031
60310294:	e3087180 	movw	r7, #33152	; 0x8180
60310298:	e3467031 	movt	r7, #24625	; 0x6031
6031029c:	e30860f0 	movw	r6, #33008	; 0x80f0
603102a0:	e3466031 	movt	r6, #24625	; 0x6031
603102a4:	e3088004 	movw	r8, #32772	; 0x8004
603102a8:	e3468031 	movt	r8, #24625	; 0x6031
603102ac:	e3089154 	movw	r9, #33108	; 0x8154
603102b0:	e3469031 	movt	r9, #24625	; 0x6031
603102b4:	e308a0f8 	movw	sl, #33016	; 0x80f8
603102b8:	e346a031 	movt	sl, #24625	; 0x6031
603102bc:	ea000013 	b	60310310 <prvIdleTask+0x88>
603102c0:	ebfffa2a 	bl	6030eb70 <vTaskEnterCritical>
603102c4:	e5943000 	ldr	r3, [r4]
603102c8:	e3530000 	cmp	r3, #0
603102cc:	0a00000e 	beq	6031030c <prvIdleTask+0x84>
603102d0:	e597300c 	ldr	r3, [r7, #12]
603102d4:	e593500c 	ldr	r5, [r3, #12]
603102d8:	e2850004 	add	r0, r5, #4
603102dc:	e5953034 	ldr	r3, [r5, #52]	; 0x34
603102e0:	e3730001 	cmn	r3, #1
603102e4:	1a000022 	bne	60310374 <prvIdleTask+0xec>
603102e8:	eb0000b4 	bl	603105c0 <uxListRemove>
603102ec:	e5963000 	ldr	r3, [r6]
603102f0:	e1a00005 	mov	r0, r5
603102f4:	e2433001 	sub	r3, r3, #1
603102f8:	e5863000 	str	r3, [r6]
603102fc:	e5943000 	ldr	r3, [r4]
60310300:	e2433001 	sub	r3, r3, #1
60310304:	e5843000 	str	r3, [r4]
60310308:	ebfff6b5 	bl	6030dde4 <prvDeleteTCB>
6031030c:	ebfffa32 	bl	6030ebdc <vTaskExitCritical>
60310310:	e5943000 	ldr	r3, [r4]
60310314:	e3530000 	cmp	r3, #0
60310318:	1affffe8 	bne	603102c0 <prvIdleTask+0x38>
6031031c:	e5983000 	ldr	r3, [r8]
60310320:	e3530000 	cmp	r3, #0
60310324:	0a000000 	beq	6031032c <prvIdleTask+0xa4>
60310328:	ef000000 	svc	0x00000000
6031032c:	eb00085b 	bl	603124a0 <ulPortGetCoreId>
60310330:	e3500000 	cmp	r0, #0
60310334:	1a000010 	bne	6031037c <prvIdleTask+0xf4>
60310338:	ebfff764 	bl	6030e0d0 <prvGetExpectedIdleTime>
6031033c:	e3500001 	cmp	r0, #1
60310340:	9afffff2 	bls	60310310 <prvIdleTask+0x88>
60310344:	e5993000 	ldr	r3, [r9]
60310348:	e3530000 	cmp	r3, #0
6031034c:	1a00000d 	bne	60310388 <prvIdleTask+0x100>
60310350:	ebfff75e 	bl	6030e0d0 <prvGetExpectedIdleTime>
60310354:	e3500001 	cmp	r0, #1
60310358:	9a000000 	bls	60310360 <prvIdleTask+0xd8>
6031035c:	eb0008a8 	bl	60312604 <vPortSuppressTicksAndSleep>
60310360:	e5993000 	ldr	r3, [r9]
60310364:	e3530000 	cmp	r3, #0
60310368:	0affffe8 	beq	60310310 <prvIdleTask+0x88>
6031036c:	ebfffd5f 	bl	6030f8f0 <xTaskResumeAll.part.0>
60310370:	eaffffe6 	b	60310310 <prvIdleTask+0x88>
60310374:	ebfffa18 	bl	6030ebdc <vTaskExitCritical>
60310378:	eaffffe7 	b	6031031c <prvIdleTask+0x94>
6031037c:	e3e00000 	mvn	r0, #0
60310380:	eb00089f 	bl	60312604 <vPortSuppressTicksAndSleep>
60310384:	eaffffe1 	b	60310310 <prvIdleTask+0x88>
60310388:	ebfff9f8 	bl	6030eb70 <vTaskEnterCritical>
6031038c:	e59a3000 	ldr	r3, [sl]
60310390:	e2833001 	add	r3, r3, #1
60310394:	e58a3000 	str	r3, [sl]
60310398:	ebfffa0f 	bl	6030ebdc <vTaskExitCritical>
6031039c:	eaffffeb 	b	60310350 <prvIdleTask+0xc8>

603103a0 <vTaskSuspendAll>:
603103a0:	e3083154 	movw	r3, #33108	; 0x8154
603103a4:	e3463031 	movt	r3, #24625	; 0x6031
603103a8:	e5933000 	ldr	r3, [r3]
603103ac:	e3530000 	cmp	r3, #0
603103b0:	012fff1e 	bxeq	lr
603103b4:	e92d4010 	push	{r4, lr}
603103b8:	ebfff9ec 	bl	6030eb70 <vTaskEnterCritical>
603103bc:	e30830f8 	movw	r3, #33016	; 0x80f8
603103c0:	e3463031 	movt	r3, #24625	; 0x6031
603103c4:	e5932000 	ldr	r2, [r3]
603103c8:	e2822001 	add	r2, r2, #1
603103cc:	e5832000 	str	r2, [r3]
603103d0:	e8bd4010 	pop	{r4, lr}
603103d4:	eafffa00 	b	6030ebdc <vTaskExitCritical>

603103d8 <vTaskDelay>:
603103d8:	e92d4070 	push	{r4, r5, r6, lr}
603103dc:	e2504000 	subs	r4, r0, #0
603103e0:	0a00000a 	beq	60310410 <vTaskDelay+0x38>
603103e4:	e3085154 	movw	r5, #33108	; 0x8154
603103e8:	e3465031 	movt	r5, #24625	; 0x6031
603103ec:	e5953000 	ldr	r3, [r5]
603103f0:	e3530000 	cmp	r3, #0
603103f4:	1a000017 	bne	60310458 <vTaskDelay+0x80>
603103f8:	e3a01000 	mov	r1, #0
603103fc:	e1a00004 	mov	r0, r4
60310400:	ebfffbe9 	bl	6030f3ac <prvAddCurrentTaskToDelayedList>
60310404:	e5953000 	ldr	r3, [r5]
60310408:	e3530000 	cmp	r3, #0
6031040c:	1a00000d 	bne	60310448 <vTaskDelay+0x70>
60310410:	eb000807 	bl	60312434 <ulPortInterruptLock>
60310414:	e1a04000 	mov	r4, r0
60310418:	eb000820 	bl	603124a0 <ulPortGetCoreId>
6031041c:	e3073ff0 	movw	r3, #32752	; 0x7ff0
60310420:	e3463031 	movt	r3, #24625	; 0x6031
60310424:	e1a02000 	mov	r2, r0
60310428:	e1a00004 	mov	r0, r4
6031042c:	e7934102 	ldr	r4, [r3, r2, lsl #2]
60310430:	eb000804 	bl	60312448 <ulPortInterruptUnLock>
60310434:	e5943058 	ldr	r3, [r4, #88]	; 0x58
60310438:	e3530000 	cmp	r3, #0
6031043c:	1a00000d 	bne	60310478 <vTaskDelay+0xa0>
60310440:	ef000000 	svc	0x00000000
60310444:	e8bd8070 	pop	{r4, r5, r6, pc}
60310448:	ebfffd28 	bl	6030f8f0 <xTaskResumeAll.part.0>
6031044c:	e3500000 	cmp	r0, #0
60310450:	18bd8070 	popne	{r4, r5, r6, pc}
60310454:	eaffffed 	b	60310410 <vTaskDelay+0x38>
60310458:	ebfff9c4 	bl	6030eb70 <vTaskEnterCritical>
6031045c:	e30830f8 	movw	r3, #33016	; 0x80f8
60310460:	e3463031 	movt	r3, #24625	; 0x6031
60310464:	e5932000 	ldr	r2, [r3]
60310468:	e2822001 	add	r2, r2, #1
6031046c:	e5832000 	str	r2, [r3]
60310470:	ebfff9d9 	bl	6030ebdc <vTaskExitCritical>
60310474:	eaffffdf 	b	603103f8 <vTaskDelay+0x20>
60310478:	eb000808 	bl	603124a0 <ulPortGetCoreId>
6031047c:	e3083198 	movw	r3, #33176	; 0x8198
60310480:	e3463031 	movt	r3, #24625	; 0x6031
60310484:	e3a02001 	mov	r2, #1
60310488:	e7832100 	str	r2, [r3, r0, lsl #2]
6031048c:	e8bd8070 	pop	{r4, r5, r6, pc}

60310490 <pvTaskIncrementMutexHeldCount>:
60310490:	e92d4070 	push	{r4, r5, r6, lr}
60310494:	eb0007e6 	bl	60312434 <ulPortInterruptLock>
60310498:	e3074ff0 	movw	r4, #32752	; 0x7ff0
6031049c:	e3464031 	movt	r4, #24625	; 0x6031
603104a0:	e1a05000 	mov	r5, r0
603104a4:	eb0007fd 	bl	603124a0 <ulPortGetCoreId>
603104a8:	e1a03000 	mov	r3, r0
603104ac:	e1a00005 	mov	r0, r5
603104b0:	e7945103 	ldr	r5, [r4, r3, lsl #2]
603104b4:	eb0007e3 	bl	60312448 <ulPortInterruptUnLock>
603104b8:	e3550000 	cmp	r5, #0
603104bc:	0a000009 	beq	603104e8 <pvTaskIncrementMutexHeldCount+0x58>
603104c0:	eb0007db 	bl	60312434 <ulPortInterruptLock>
603104c4:	e1a05000 	mov	r5, r0
603104c8:	eb0007f4 	bl	603124a0 <ulPortGetCoreId>
603104cc:	e1a03000 	mov	r3, r0
603104d0:	e1a00005 	mov	r0, r5
603104d4:	e7945103 	ldr	r5, [r4, r3, lsl #2]
603104d8:	eb0007da 	bl	60312448 <ulPortInterruptUnLock>
603104dc:	e5953068 	ldr	r3, [r5, #104]	; 0x68
603104e0:	e2833001 	add	r3, r3, #1
603104e4:	e5853068 	str	r3, [r5, #104]	; 0x68
603104e8:	eb0007d1 	bl	60312434 <ulPortInterruptLock>
603104ec:	e1a05000 	mov	r5, r0
603104f0:	eb0007ea 	bl	603124a0 <ulPortGetCoreId>
603104f4:	e1a03000 	mov	r3, r0
603104f8:	e1a00005 	mov	r0, r5
603104fc:	e7944103 	ldr	r4, [r4, r3, lsl #2]
60310500:	eb0007d0 	bl	60312448 <ulPortInterruptUnLock>
60310504:	e1a00004 	mov	r0, r4
60310508:	e8bd8070 	pop	{r4, r5, r6, pc}

6031050c <vListInitialise>:
6031050c:	e2803008 	add	r3, r0, #8
60310510:	e3a02000 	mov	r2, #0
60310514:	e3e01000 	mvn	r1, #0
60310518:	e5803004 	str	r3, [r0, #4]
6031051c:	e5801008 	str	r1, [r0, #8]
60310520:	e580300c 	str	r3, [r0, #12]
60310524:	e5803010 	str	r3, [r0, #16]
60310528:	e5802000 	str	r2, [r0]
6031052c:	e12fff1e 	bx	lr

60310530 <vListInitialiseItem>:
60310530:	e3a03000 	mov	r3, #0
60310534:	e5803010 	str	r3, [r0, #16]
60310538:	e12fff1e 	bx	lr

6031053c <vListInsertEnd>:
6031053c:	e5903004 	ldr	r3, [r0, #4]
60310540:	e5813004 	str	r3, [r1, #4]
60310544:	e5932008 	ldr	r2, [r3, #8]
60310548:	e5812008 	str	r2, [r1, #8]
6031054c:	e5821004 	str	r1, [r2, #4]
60310550:	e5831008 	str	r1, [r3, #8]
60310554:	e5810010 	str	r0, [r1, #16]
60310558:	e5903000 	ldr	r3, [r0]
6031055c:	e2833001 	add	r3, r3, #1
60310560:	e5803000 	str	r3, [r0]
60310564:	e12fff1e 	bx	lr

60310568 <vListInsert>:
60310568:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
6031056c:	e591e000 	ldr	lr, [r1]
60310570:	e37e0001 	cmn	lr, #1
60310574:	12803008 	addne	r3, r0, #8
60310578:	0a00000d 	beq	603105b4 <vListInsert+0x4c>
6031057c:	e1a0c003 	mov	ip, r3
60310580:	e5933004 	ldr	r3, [r3, #4]
60310584:	e5932000 	ldr	r2, [r3]
60310588:	e152000e 	cmp	r2, lr
6031058c:	9afffffa 	bls	6031057c <vListInsert+0x14>
60310590:	e5813004 	str	r3, [r1, #4]
60310594:	e5831008 	str	r1, [r3, #8]
60310598:	e581c008 	str	ip, [r1, #8]
6031059c:	e58c1004 	str	r1, [ip, #4]
603105a0:	e5810010 	str	r0, [r1, #16]
603105a4:	e5903000 	ldr	r3, [r0]
603105a8:	e2833001 	add	r3, r3, #1
603105ac:	e5803000 	str	r3, [r0]
603105b0:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)
603105b4:	e590c010 	ldr	ip, [r0, #16]
603105b8:	e59c3004 	ldr	r3, [ip, #4]
603105bc:	eafffff3 	b	60310590 <vListInsert+0x28>

603105c0 <uxListRemove>:
603105c0:	e9900006 	ldmib	r0, {r1, r2}
603105c4:	e5903010 	ldr	r3, [r0, #16]
603105c8:	e5812008 	str	r2, [r1, #8]
603105cc:	e5901004 	ldr	r1, [r0, #4]
603105d0:	e5821004 	str	r1, [r2, #4]
603105d4:	e5932004 	ldr	r2, [r3, #4]
603105d8:	e1520000 	cmp	r2, r0
603105dc:	05902008 	ldreq	r2, [r0, #8]
603105e0:	05832004 	streq	r2, [r3, #4]
603105e4:	e3a02000 	mov	r2, #0
603105e8:	e5802010 	str	r2, [r0, #16]
603105ec:	e5932000 	ldr	r2, [r3]
603105f0:	e2422001 	sub	r2, r2, #1
603105f4:	e5832000 	str	r2, [r3]
603105f8:	e5930000 	ldr	r0, [r3]
603105fc:	e12fff1e 	bx	lr

60310600 <prvCopyDataToQueue>:
60310600:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60310604:	e1a04000 	mov	r4, r0
60310608:	e1a05002 	mov	r5, r2
6031060c:	e1a07001 	mov	r7, r1
60310610:	ebfff956 	bl	6030eb70 <vTaskEnterCritical>
60310614:	e5942040 	ldr	r2, [r4, #64]	; 0x40
60310618:	e5946038 	ldr	r6, [r4, #56]	; 0x38
6031061c:	e3520000 	cmp	r2, #0
60310620:	1a000008 	bne	60310648 <prvCopyDataToQueue+0x48>
60310624:	e5947000 	ldr	r7, [r4]
60310628:	e2866001 	add	r6, r6, #1
6031062c:	e3570000 	cmp	r7, #0
60310630:	11a05002 	movne	r5, r2
60310634:	0a000023 	beq	603106c8 <prvCopyDataToQueue+0xc8>
60310638:	e5846038 	str	r6, [r4, #56]	; 0x38
6031063c:	ebfff966 	bl	6030ebdc <vTaskExitCritical>
60310640:	e1a00005 	mov	r0, r5
60310644:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60310648:	e1a01007 	mov	r1, r7
6031064c:	e3550000 	cmp	r5, #0
60310650:	1a00000b 	bne	60310684 <prvCopyDataToQueue+0x84>
60310654:	e5940004 	ldr	r0, [r4, #4]
60310658:	eb0011e5 	bl	60314df4 <__wrap_memcpy>
6031065c:	e5943004 	ldr	r3, [r4, #4]
60310660:	e5942040 	ldr	r2, [r4, #64]	; 0x40
60310664:	e2866001 	add	r6, r6, #1
60310668:	e0833002 	add	r3, r3, r2
6031066c:	e5942008 	ldr	r2, [r4, #8]
60310670:	e5843004 	str	r3, [r4, #4]
60310674:	e1530002 	cmp	r3, r2
60310678:	25943000 	ldrcs	r3, [r4]
6031067c:	25843004 	strcs	r3, [r4, #4]
60310680:	eaffffec 	b	60310638 <prvCopyDataToQueue+0x38>
60310684:	e594000c 	ldr	r0, [r4, #12]
60310688:	eb0011d9 	bl	60314df4 <__wrap_memcpy>
6031068c:	e5942040 	ldr	r2, [r4, #64]	; 0x40
60310690:	e594300c 	ldr	r3, [r4, #12]
60310694:	e2621000 	rsb	r1, r2, #0
60310698:	e0433002 	sub	r3, r3, r2
6031069c:	e5942000 	ldr	r2, [r4]
603106a0:	e584300c 	str	r3, [r4, #12]
603106a4:	e1530002 	cmp	r3, r2
603106a8:	35943008 	ldrcc	r3, [r4, #8]
603106ac:	30833001 	addcc	r3, r3, r1
603106b0:	3584300c 	strcc	r3, [r4, #12]
603106b4:	e3550002 	cmp	r5, #2
603106b8:	0a000007 	beq	603106dc <prvCopyDataToQueue+0xdc>
603106bc:	e2866001 	add	r6, r6, #1
603106c0:	e3a05000 	mov	r5, #0
603106c4:	eaffffdb 	b	60310638 <prvCopyDataToQueue+0x38>
603106c8:	e5940008 	ldr	r0, [r4, #8]
603106cc:	ebfffe4e 	bl	6031000c <xTaskPriorityDisinherit>
603106d0:	e5847008 	str	r7, [r4, #8]
603106d4:	e1a05000 	mov	r5, r0
603106d8:	eaffffd6 	b	60310638 <prvCopyDataToQueue+0x38>
603106dc:	e3560001 	cmp	r6, #1
603106e0:	33a06001 	movcc	r6, #1
603106e4:	e3a05000 	mov	r5, #0
603106e8:	eaffffd2 	b	60310638 <prvCopyDataToQueue+0x38>

603106ec <prvNotifyQueueSetContainer>:
603106ec:	e92d4070 	push	{r4, r5, r6, lr}
603106f0:	e1a05001 	mov	r5, r1
603106f4:	e24dd008 	sub	sp, sp, #8
603106f8:	e5904048 	ldr	r4, [r0, #72]	; 0x48
603106fc:	e58d0004 	str	r0, [sp, #4]
60310700:	e3540000 	cmp	r4, #0
60310704:	0a000021 	beq	60310790 <prvNotifyQueueSetContainer+0xa4>
60310708:	e5942038 	ldr	r2, [r4, #56]	; 0x38
6031070c:	e594303c 	ldr	r3, [r4, #60]	; 0x3c
60310710:	e1520003 	cmp	r2, r3
60310714:	2a000025 	bcs	603107b0 <prvNotifyQueueSetContainer+0xc4>
60310718:	e5942038 	ldr	r2, [r4, #56]	; 0x38
6031071c:	e1530002 	cmp	r3, r2
60310720:	93a05000 	movls	r5, #0
60310724:	8a000002 	bhi	60310734 <prvNotifyQueueSetContainer+0x48>
60310728:	e1a00005 	mov	r0, r5
6031072c:	e28dd008 	add	sp, sp, #8
60310730:	e8bd8070 	pop	{r4, r5, r6, pc}
60310734:	e5d46045 	ldrb	r6, [r4, #69]	; 0x45
60310738:	e1a02005 	mov	r2, r5
6031073c:	e28d1004 	add	r1, sp, #4
60310740:	e1a00004 	mov	r0, r4
60310744:	e6af6076 	sxtb	r6, r6
60310748:	ebffffac 	bl	60310600 <prvCopyDataToQueue>
6031074c:	e3760001 	cmn	r6, #1
60310750:	e1a05000 	mov	r5, r0
60310754:	0a000005 	beq	60310770 <prvNotifyQueueSetContainer+0x84>
60310758:	e2866001 	add	r6, r6, #1
6031075c:	e1a00005 	mov	r0, r5
60310760:	e6af6076 	sxtb	r6, r6
60310764:	e5c46045 	strb	r6, [r4, #69]	; 0x45
60310768:	e28dd008 	add	sp, sp, #8
6031076c:	e8bd8070 	pop	{r4, r5, r6, pc}
60310770:	e5943024 	ldr	r3, [r4, #36]	; 0x24
60310774:	e3530000 	cmp	r3, #0
60310778:	0affffea 	beq	60310728 <prvNotifyQueueSetContainer+0x3c>
6031077c:	e2840024 	add	r0, r4, #36	; 0x24
60310780:	ebfffd2c 	bl	6030fc38 <xTaskRemoveFromEventList>
60310784:	e3500000 	cmp	r0, #0
60310788:	13a05001 	movne	r5, #1
6031078c:	eaffffe5 	b	60310728 <prvNotifyQueueSetContainer+0x3c>
60310790:	e30608d0 	movw	r0, #26832	; 0x68d0
60310794:	e3460031 	movt	r0, #24625	; 0x6031
60310798:	e30019eb 	movw	r1, #2539	; 0x9eb
6031079c:	ebffe522 	bl	60309c2c <vAssertCalled>
603107a0:	e5942038 	ldr	r2, [r4, #56]	; 0x38
603107a4:	e594303c 	ldr	r3, [r4, #60]	; 0x3c
603107a8:	e1520003 	cmp	r2, r3
603107ac:	3affffd9 	bcc	60310718 <prvNotifyQueueSetContainer+0x2c>
603107b0:	e30019ec 	movw	r1, #2540	; 0x9ec
603107b4:	e30608d0 	movw	r0, #26832	; 0x68d0
603107b8:	e3460031 	movt	r0, #24625	; 0x6031
603107bc:	ebffe51a 	bl	60309c2c <vAssertCalled>
603107c0:	e594303c 	ldr	r3, [r4, #60]	; 0x3c
603107c4:	eaffffd3 	b	60310718 <prvNotifyQueueSetContainer+0x2c>

603107c8 <prvUnlockQueue>:
603107c8:	e92d4070 	push	{r4, r5, r6, lr}
603107cc:	e1a05000 	mov	r5, r0
603107d0:	ebfff8e6 	bl	6030eb70 <vTaskEnterCritical>
603107d4:	e5d54045 	ldrb	r4, [r5, #69]	; 0x45
603107d8:	e6af4074 	sxtb	r4, r4
603107dc:	e3540000 	cmp	r4, #0
603107e0:	da000019 	ble	6031084c <prvUnlockQueue+0x84>
603107e4:	e2856024 	add	r6, r5, #36	; 0x24
603107e8:	ea000006 	b	60310808 <prvUnlockQueue+0x40>
603107ec:	ebffffbe 	bl	603106ec <prvNotifyQueueSetContainer>
603107f0:	e3500000 	cmp	r0, #0
603107f4:	1a00000f 	bne	60310838 <prvUnlockQueue+0x70>
603107f8:	e2444001 	sub	r4, r4, #1
603107fc:	e6af4074 	sxtb	r4, r4
60310800:	e3540000 	cmp	r4, #0
60310804:	0a000010 	beq	6031084c <prvUnlockQueue+0x84>
60310808:	e5953048 	ldr	r3, [r5, #72]	; 0x48
6031080c:	e3a01000 	mov	r1, #0
60310810:	e1a00005 	mov	r0, r5
60310814:	e1530001 	cmp	r3, r1
60310818:	1afffff3 	bne	603107ec <prvUnlockQueue+0x24>
6031081c:	e5953024 	ldr	r3, [r5, #36]	; 0x24
60310820:	e1a00006 	mov	r0, r6
60310824:	e3530000 	cmp	r3, #0
60310828:	0a000007 	beq	6031084c <prvUnlockQueue+0x84>
6031082c:	ebfffd01 	bl	6030fc38 <xTaskRemoveFromEventList>
60310830:	e3500000 	cmp	r0, #0
60310834:	0affffef 	beq	603107f8 <prvUnlockQueue+0x30>
60310838:	e2444001 	sub	r4, r4, #1
6031083c:	ebfff878 	bl	6030ea24 <vTaskMissedYield>
60310840:	e6af4074 	sxtb	r4, r4
60310844:	e3540000 	cmp	r4, #0
60310848:	1affffee 	bne	60310808 <prvUnlockQueue+0x40>
6031084c:	e3e03000 	mvn	r3, #0
60310850:	e5c53045 	strb	r3, [r5, #69]	; 0x45
60310854:	ebfff8e0 	bl	6030ebdc <vTaskExitCritical>
60310858:	ebfff8c4 	bl	6030eb70 <vTaskEnterCritical>
6031085c:	e5d54044 	ldrb	r4, [r5, #68]	; 0x44
60310860:	e6af4074 	sxtb	r4, r4
60310864:	e3540000 	cmp	r4, #0
60310868:	da000011 	ble	603108b4 <prvUnlockQueue+0xec>
6031086c:	e2856010 	add	r6, r5, #16
60310870:	ea000003 	b	60310884 <prvUnlockQueue+0xbc>
60310874:	e2444001 	sub	r4, r4, #1
60310878:	e6af4074 	sxtb	r4, r4
6031087c:	e3540000 	cmp	r4, #0
60310880:	0a00000b 	beq	603108b4 <prvUnlockQueue+0xec>
60310884:	e5953010 	ldr	r3, [r5, #16]
60310888:	e1a00006 	mov	r0, r6
6031088c:	e3530000 	cmp	r3, #0
60310890:	0a000007 	beq	603108b4 <prvUnlockQueue+0xec>
60310894:	ebfffce7 	bl	6030fc38 <xTaskRemoveFromEventList>
60310898:	e3500000 	cmp	r0, #0
6031089c:	0afffff4 	beq	60310874 <prvUnlockQueue+0xac>
603108a0:	e2444001 	sub	r4, r4, #1
603108a4:	ebfff85e 	bl	6030ea24 <vTaskMissedYield>
603108a8:	e6af4074 	sxtb	r4, r4
603108ac:	e3540000 	cmp	r4, #0
603108b0:	1afffff3 	bne	60310884 <prvUnlockQueue+0xbc>
603108b4:	e3e03000 	mvn	r3, #0
603108b8:	e5c53044 	strb	r3, [r5, #68]	; 0x44
603108bc:	e8bd4070 	pop	{r4, r5, r6, lr}
603108c0:	eafff8c5 	b	6030ebdc <vTaskExitCritical>

603108c4 <xQueueGenericReset>:
603108c4:	e92d4070 	push	{r4, r5, r6, lr}
603108c8:	e1a05001 	mov	r5, r1
603108cc:	e2504000 	subs	r4, r0, #0
603108d0:	0a000022 	beq	60310960 <xQueueGenericReset+0x9c>
603108d4:	ebfff8a5 	bl	6030eb70 <vTaskEnterCritical>
603108d8:	e5941040 	ldr	r1, [r4, #64]	; 0x40
603108dc:	e594303c 	ldr	r3, [r4, #60]	; 0x3c
603108e0:	e5942000 	ldr	r2, [r4]
603108e4:	e5842004 	str	r2, [r4, #4]
603108e8:	e3550000 	cmp	r5, #0
603108ec:	e0030193 	mul	r3, r3, r1
603108f0:	e0431001 	sub	r1, r3, r1
603108f4:	e0823003 	add	r3, r2, r3
603108f8:	e0822001 	add	r2, r2, r1
603108fc:	e5843008 	str	r3, [r4, #8]
60310900:	e3e01000 	mvn	r1, #0
60310904:	e3a03000 	mov	r3, #0
60310908:	e584200c 	str	r2, [r4, #12]
6031090c:	e5843038 	str	r3, [r4, #56]	; 0x38
60310910:	e5c41044 	strb	r1, [r4, #68]	; 0x44
60310914:	e5c41045 	strb	r1, [r4, #69]	; 0x45
60310918:	1a00000b 	bne	6031094c <xQueueGenericReset+0x88>
6031091c:	e5943010 	ldr	r3, [r4, #16]
60310920:	e3530000 	cmp	r3, #0
60310924:	1a000002 	bne	60310934 <xQueueGenericReset+0x70>
60310928:	ebfff8ab 	bl	6030ebdc <vTaskExitCritical>
6031092c:	e3a00001 	mov	r0, #1
60310930:	e8bd8070 	pop	{r4, r5, r6, pc}
60310934:	e2840010 	add	r0, r4, #16
60310938:	ebfffcbe 	bl	6030fc38 <xTaskRemoveFromEventList>
6031093c:	e3500000 	cmp	r0, #0
60310940:	0afffff8 	beq	60310928 <xQueueGenericReset+0x64>
60310944:	ebfff874 	bl	6030eb1c <vTaskYieldWithinAPI>
60310948:	eafffff6 	b	60310928 <xQueueGenericReset+0x64>
6031094c:	e2840010 	add	r0, r4, #16
60310950:	ebfffeed 	bl	6031050c <vListInitialise>
60310954:	e2840024 	add	r0, r4, #36	; 0x24
60310958:	ebfffeeb 	bl	6031050c <vListInitialise>
6031095c:	eafffff1 	b	60310928 <xQueueGenericReset+0x64>
60310960:	e30608d0 	movw	r0, #26832	; 0x68d0
60310964:	e3460031 	movt	r0, #24625	; 0x6031
60310968:	e3001103 	movw	r1, #259	; 0x103
6031096c:	ebffe4ae 	bl	60309c2c <vAssertCalled>
60310970:	eaffffd7 	b	603108d4 <xQueueGenericReset+0x10>

60310974 <xQueueGenericCreateStatic>:
60310974:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60310978:	e1a06001 	mov	r6, r1
6031097c:	e24dd008 	sub	sp, sp, #8
60310980:	e1a05002 	mov	r5, r2
60310984:	e1a04003 	mov	r4, r3
60310988:	e5dd7020 	ldrb	r7, [sp, #32]
6031098c:	e2508000 	subs	r8, r0, #0
60310990:	0a000024 	beq	60310a28 <xQueueGenericCreateStatic+0xb4>
60310994:	e3540000 	cmp	r4, #0
60310998:	0a000028 	beq	60310a40 <xQueueGenericCreateStatic+0xcc>
6031099c:	e2563000 	subs	r3, r6, #0
603109a0:	13a03001 	movne	r3, #1
603109a4:	e3550000 	cmp	r5, #0
603109a8:	03833001 	orreq	r3, r3, #1
603109ac:	e3530000 	cmp	r3, #0
603109b0:	0a000027 	beq	60310a54 <xQueueGenericCreateStatic+0xe0>
603109b4:	e16f3f16 	clz	r3, r6
603109b8:	e1a032a3 	lsr	r3, r3, #5
603109bc:	e3550000 	cmp	r5, #0
603109c0:	13833001 	orrne	r3, r3, #1
603109c4:	e3530000 	cmp	r3, #0
603109c8:	0a000026 	beq	60310a68 <xQueueGenericCreateStatic+0xf4>
603109cc:	e3a03054 	mov	r3, #84	; 0x54
603109d0:	e58d3004 	str	r3, [sp, #4]
603109d4:	e59d3004 	ldr	r3, [sp, #4]
603109d8:	e3530054 	cmp	r3, #84	; 0x54
603109dc:	1a000026 	bne	60310a7c <xQueueGenericCreateStatic+0x108>
603109e0:	e59d3004 	ldr	r3, [sp, #4]
603109e4:	e3540000 	cmp	r4, #0
603109e8:	0a00000b 	beq	60310a1c <xQueueGenericCreateStatic+0xa8>
603109ec:	e3560000 	cmp	r6, #0
603109f0:	01a05004 	moveq	r5, r4
603109f4:	e3a01001 	mov	r1, #1
603109f8:	e1a00004 	mov	r0, r4
603109fc:	e584803c 	str	r8, [r4, #60]	; 0x3c
60310a00:	e5845000 	str	r5, [r4]
60310a04:	e5846040 	str	r6, [r4, #64]	; 0x40
60310a08:	e5c41046 	strb	r1, [r4, #70]	; 0x46
60310a0c:	ebffffac 	bl	603108c4 <xQueueGenericReset>
60310a10:	e3a03000 	mov	r3, #0
60310a14:	e5c47050 	strb	r7, [r4, #80]	; 0x50
60310a18:	e5843048 	str	r3, [r4, #72]	; 0x48
60310a1c:	e1a00004 	mov	r0, r4
60310a20:	e28dd008 	add	sp, sp, #8
60310a24:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60310a28:	e3a01f4d 	mov	r1, #308	; 0x134
60310a2c:	e30608d0 	movw	r0, #26832	; 0x68d0
60310a30:	e3460031 	movt	r0, #24625	; 0x6031
60310a34:	ebffe47c 	bl	60309c2c <vAssertCalled>
60310a38:	e3540000 	cmp	r4, #0
60310a3c:	1affffd6 	bne	6031099c <xQueueGenericCreateStatic+0x28>
60310a40:	e3a01f4e 	mov	r1, #312	; 0x138
60310a44:	e30608d0 	movw	r0, #26832	; 0x68d0
60310a48:	e3460031 	movt	r0, #24625	; 0x6031
60310a4c:	ebffe476 	bl	60309c2c <vAssertCalled>
60310a50:	eaffffd1 	b	6031099c <xQueueGenericCreateStatic+0x28>
60310a54:	e3a01f4f 	mov	r1, #316	; 0x13c
60310a58:	e30608d0 	movw	r0, #26832	; 0x68d0
60310a5c:	e3460031 	movt	r0, #24625	; 0x6031
60310a60:	ebffe471 	bl	60309c2c <vAssertCalled>
60310a64:	eaffffd8 	b	603109cc <xQueueGenericCreateStatic+0x58>
60310a68:	e30608d0 	movw	r0, #26832	; 0x68d0
60310a6c:	e3460031 	movt	r0, #24625	; 0x6031
60310a70:	e300113d 	movw	r1, #317	; 0x13d
60310a74:	ebffe46c 	bl	60309c2c <vAssertCalled>
60310a78:	eaffffd3 	b	603109cc <xQueueGenericCreateStatic+0x58>
60310a7c:	e30608d0 	movw	r0, #26832	; 0x68d0
60310a80:	e3460031 	movt	r0, #24625	; 0x6031
60310a84:	e3001145 	movw	r1, #325	; 0x145
60310a88:	ebffe467 	bl	60309c2c <vAssertCalled>
60310a8c:	eaffffd3 	b	603109e0 <xQueueGenericCreateStatic+0x6c>

60310a90 <xQueueGenericCreate>:
60310a90:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60310a94:	e1a05001 	mov	r5, r1
60310a98:	e1a07002 	mov	r7, r2
60310a9c:	e2506000 	subs	r6, r0, #0
60310aa0:	0a00001d 	beq	60310b1c <xQueueGenericCreate+0x8c>
60310aa4:	e3550000 	cmp	r5, #0
60310aa8:	0a000012 	beq	60310af8 <xQueueGenericCreate+0x68>
60310aac:	e0000695 	mul	r0, r5, r6
60310ab0:	e2800054 	add	r0, r0, #84	; 0x54
60310ab4:	eb0005b5 	bl	60312190 <pvPortMalloc>
60310ab8:	e2504000 	subs	r4, r0, #0
60310abc:	0a000013 	beq	60310b10 <xQueueGenericCreate+0x80>
60310ac0:	e3a03000 	mov	r3, #0
60310ac4:	e5c43046 	strb	r3, [r4, #70]	; 0x46
60310ac8:	e2843054 	add	r3, r4, #84	; 0x54
60310acc:	e5843000 	str	r3, [r4]
60310ad0:	e3a01001 	mov	r1, #1
60310ad4:	e1a00004 	mov	r0, r4
60310ad8:	e584603c 	str	r6, [r4, #60]	; 0x3c
60310adc:	e5845040 	str	r5, [r4, #64]	; 0x40
60310ae0:	ebffff77 	bl	603108c4 <xQueueGenericReset>
60310ae4:	e3a03000 	mov	r3, #0
60310ae8:	e1a00004 	mov	r0, r4
60310aec:	e5c47050 	strb	r7, [r4, #80]	; 0x50
60310af0:	e5843048 	str	r3, [r4, #72]	; 0x48
60310af4:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60310af8:	e3a00054 	mov	r0, #84	; 0x54
60310afc:	eb0005a3 	bl	60312190 <pvPortMalloc>
60310b00:	e2504000 	subs	r4, r0, #0
60310b04:	15c45046 	strbne	r5, [r4, #70]	; 0x46
60310b08:	15844000 	strne	r4, [r4]
60310b0c:	1affffef 	bne	60310ad0 <xQueueGenericCreate+0x40>
60310b10:	e3a04000 	mov	r4, #0
60310b14:	e1a00004 	mov	r0, r4
60310b18:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60310b1c:	e30608d0 	movw	r0, #26832	; 0x68d0
60310b20:	e3460031 	movt	r0, #24625	; 0x6031
60310b24:	e300116e 	movw	r1, #366	; 0x16e
60310b28:	ebffe43f 	bl	60309c2c <vAssertCalled>
60310b2c:	eaffffdc 	b	60310aa4 <xQueueGenericCreate+0x14>

60310b30 <xQueueGenericSend>:
60310b30:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
60310b34:	e1a09001 	mov	r9, r1
60310b38:	e24dd014 	sub	sp, sp, #20
60310b3c:	e1a08003 	mov	r8, r3
60310b40:	e2504000 	subs	r4, r0, #0
60310b44:	e58d2004 	str	r2, [sp, #4]
60310b48:	0a000078 	beq	60310d30 <xQueueGenericSend+0x200>
60310b4c:	e3590000 	cmp	r9, #0
60310b50:	0a00007c 	beq	60310d48 <xQueueGenericSend+0x218>
60310b54:	e3580002 	cmp	r8, #2
60310b58:	13a06000 	movne	r6, #0
60310b5c:	11a07006 	movne	r7, r6
60310b60:	1a000005 	bne	60310b7c <xQueueGenericSend+0x4c>
60310b64:	ea00005c 	b	60310cdc <xQueueGenericSend+0x1ac>
60310b68:	ebfff81b 	bl	6030ebdc <vTaskExitCritical>
60310b6c:	e1a00004 	mov	r0, r4
60310b70:	ebffff14 	bl	603107c8 <prvUnlockQueue>
60310b74:	ebfffbce 	bl	6030fab4 <xTaskResumeAll>
60310b78:	e3a06001 	mov	r6, #1
60310b7c:	ebfff7fb 	bl	6030eb70 <vTaskEnterCritical>
60310b80:	e594c038 	ldr	ip, [r4, #56]	; 0x38
60310b84:	e594003c 	ldr	r0, [r4, #60]	; 0x3c
60310b88:	e15c0000 	cmp	ip, r0
60310b8c:	3a000031 	bcc	60310c58 <xQueueGenericSend+0x128>
60310b90:	e59d5004 	ldr	r5, [sp, #4]
60310b94:	e3550000 	cmp	r5, #0
60310b98:	0a00003f 	beq	60310c9c <xQueueGenericSend+0x16c>
60310b9c:	e3560000 	cmp	r6, #0
60310ba0:	0a000027 	beq	60310c44 <xQueueGenericSend+0x114>
60310ba4:	ebfff80c 	bl	6030ebdc <vTaskExitCritical>
60310ba8:	ebfffdfc 	bl	603103a0 <vTaskSuspendAll>
60310bac:	ebfff7ef 	bl	6030eb70 <vTaskEnterCritical>
60310bb0:	e5d43044 	ldrb	r3, [r4, #68]	; 0x44
60310bb4:	e35300ff 	cmp	r3, #255	; 0xff
60310bb8:	05c47044 	strbeq	r7, [r4, #68]	; 0x44
60310bbc:	e5d43045 	ldrb	r3, [r4, #69]	; 0x45
60310bc0:	e35300ff 	cmp	r3, #255	; 0xff
60310bc4:	05c47045 	strbeq	r7, [r4, #69]	; 0x45
60310bc8:	ebfff803 	bl	6030ebdc <vTaskExitCritical>
60310bcc:	e28d1004 	add	r1, sp, #4
60310bd0:	e28d0008 	add	r0, sp, #8
60310bd4:	ebfffc63 	bl	6030fd68 <xTaskCheckForTimeOut>
60310bd8:	e3500000 	cmp	r0, #0
60310bdc:	1a000032 	bne	60310cac <xQueueGenericSend+0x17c>
60310be0:	ebfff7e2 	bl	6030eb70 <vTaskEnterCritical>
60310be4:	e5942038 	ldr	r2, [r4, #56]	; 0x38
60310be8:	e594303c 	ldr	r3, [r4, #60]	; 0x3c
60310bec:	e1520003 	cmp	r2, r3
60310bf0:	1affffdc 	bne	60310b68 <xQueueGenericSend+0x38>
60310bf4:	e2845010 	add	r5, r4, #16
60310bf8:	ebfff7f7 	bl	6030ebdc <vTaskExitCritical>
60310bfc:	e59d1004 	ldr	r1, [sp, #4]
60310c00:	e1a00005 	mov	r0, r5
60310c04:	ebfffbb0 	bl	6030facc <vTaskPlaceOnEventList>
60310c08:	ebfff7d8 	bl	6030eb70 <vTaskEnterCritical>
60310c0c:	e5942038 	ldr	r2, [r4, #56]	; 0x38
60310c10:	e594303c 	ldr	r3, [r4, #60]	; 0x3c
60310c14:	e1520003 	cmp	r2, r3
60310c18:	0a00000c 	beq	60310c50 <xQueueGenericSend+0x120>
60310c1c:	ebfff7ee 	bl	6030ebdc <vTaskExitCritical>
60310c20:	e1a00005 	mov	r0, r5
60310c24:	ebfffbe1 	bl	6030fbb0 <vTaskPlaceRemoveFromEventList>
60310c28:	e1a00004 	mov	r0, r4
60310c2c:	ebfffee5 	bl	603107c8 <prvUnlockQueue>
60310c30:	ebfffb9f 	bl	6030fab4 <xTaskResumeAll>
60310c34:	e3500000 	cmp	r0, #0
60310c38:	1affffce 	bne	60310b78 <xQueueGenericSend+0x48>
60310c3c:	ebfff7b6 	bl	6030eb1c <vTaskYieldWithinAPI>
60310c40:	eaffffcc 	b	60310b78 <xQueueGenericSend+0x48>
60310c44:	e28d0008 	add	r0, sp, #8
60310c48:	ebfff76d 	bl	6030ea04 <vTaskInternalSetTimeOutState>
60310c4c:	eaffffd4 	b	60310ba4 <xQueueGenericSend+0x74>
60310c50:	ebfff7e1 	bl	6030ebdc <vTaskExitCritical>
60310c54:	eafffff3 	b	60310c28 <xQueueGenericSend+0xf8>
60310c58:	e1a02008 	mov	r2, r8
60310c5c:	e1a01009 	mov	r1, r9
60310c60:	e1a00004 	mov	r0, r4
60310c64:	e5943038 	ldr	r3, [r4, #56]	; 0x38
60310c68:	ebfffe64 	bl	60310600 <prvCopyDataToQueue>
60310c6c:	e5943048 	ldr	r3, [r4, #72]	; 0x48
60310c70:	e3530000 	cmp	r3, #0
60310c74:	1a000027 	bne	60310d18 <xQueueGenericSend+0x1e8>
60310c78:	e5943024 	ldr	r3, [r4, #36]	; 0x24
60310c7c:	e3530000 	cmp	r3, #0
60310c80:	1a00000f 	bne	60310cc4 <xQueueGenericSend+0x194>
60310c84:	e3500000 	cmp	r0, #0
60310c88:	1a000011 	bne	60310cd4 <xQueueGenericSend+0x1a4>
60310c8c:	ebfff7d2 	bl	6030ebdc <vTaskExitCritical>
60310c90:	e3a00001 	mov	r0, #1
60310c94:	e28dd014 	add	sp, sp, #20
60310c98:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
60310c9c:	ebfff7ce 	bl	6030ebdc <vTaskExitCritical>
60310ca0:	e1a00005 	mov	r0, r5
60310ca4:	e28dd014 	add	sp, sp, #20
60310ca8:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
60310cac:	e1a00004 	mov	r0, r4
60310cb0:	ebfffec4 	bl	603107c8 <prvUnlockQueue>
60310cb4:	ebfffb7e 	bl	6030fab4 <xTaskResumeAll>
60310cb8:	e3a00000 	mov	r0, #0
60310cbc:	e28dd014 	add	sp, sp, #20
60310cc0:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
60310cc4:	e2840024 	add	r0, r4, #36	; 0x24
60310cc8:	ebfffbda 	bl	6030fc38 <xTaskRemoveFromEventList>
60310ccc:	e3500000 	cmp	r0, #0
60310cd0:	0affffed 	beq	60310c8c <xQueueGenericSend+0x15c>
60310cd4:	ebfff790 	bl	6030eb1c <vTaskYieldWithinAPI>
60310cd8:	eaffffeb 	b	60310c8c <xQueueGenericSend+0x15c>
60310cdc:	e594303c 	ldr	r3, [r4, #60]	; 0x3c
60310ce0:	e3530001 	cmp	r3, #1
60310ce4:	1a00001f 	bne	60310d68 <xQueueGenericSend+0x238>
60310ce8:	ebfff7a0 	bl	6030eb70 <vTaskEnterCritical>
60310cec:	e3a02002 	mov	r2, #2
60310cf0:	e1a01009 	mov	r1, r9
60310cf4:	e1a00004 	mov	r0, r4
60310cf8:	e5943038 	ldr	r3, [r4, #56]	; 0x38
60310cfc:	e5945038 	ldr	r5, [r4, #56]	; 0x38
60310d00:	ebfffe3e 	bl	60310600 <prvCopyDataToQueue>
60310d04:	e5943048 	ldr	r3, [r4, #72]	; 0x48
60310d08:	e3530000 	cmp	r3, #0
60310d0c:	0affffd9 	beq	60310c78 <xQueueGenericSend+0x148>
60310d10:	e3550000 	cmp	r5, #0
60310d14:	1affffdc 	bne	60310c8c <xQueueGenericSend+0x15c>
60310d18:	e1a01008 	mov	r1, r8
60310d1c:	e1a00004 	mov	r0, r4
60310d20:	ebfffe71 	bl	603106ec <prvNotifyQueueSetContainer>
60310d24:	e3500000 	cmp	r0, #0
60310d28:	0affffd7 	beq	60310c8c <xQueueGenericSend+0x15c>
60310d2c:	eaffffe8 	b	60310cd4 <xQueueGenericSend+0x1a4>
60310d30:	e30608d0 	movw	r0, #26832	; 0x68d0
60310d34:	e3460031 	movt	r0, #24625	; 0x6031
60310d38:	e30012c6 	movw	r1, #710	; 0x2c6
60310d3c:	ebffe3ba 	bl	60309c2c <vAssertCalled>
60310d40:	e3590000 	cmp	r9, #0
60310d44:	1affff82 	bne	60310b54 <xQueueGenericSend+0x24>
60310d48:	e5943040 	ldr	r3, [r4, #64]	; 0x40
60310d4c:	e3530000 	cmp	r3, #0
60310d50:	0affff7f 	beq	60310b54 <xQueueGenericSend+0x24>
60310d54:	e30608d0 	movw	r0, #26832	; 0x68d0
60310d58:	e3460031 	movt	r0, #24625	; 0x6031
60310d5c:	e30012c7 	movw	r1, #711	; 0x2c7
60310d60:	ebffe3b1 	bl	60309c2c <vAssertCalled>
60310d64:	eaffff7a 	b	60310b54 <xQueueGenericSend+0x24>
60310d68:	e3a01fb2 	mov	r1, #712	; 0x2c8
60310d6c:	e30608d0 	movw	r0, #26832	; 0x68d0
60310d70:	e3460031 	movt	r0, #24625	; 0x6031
60310d74:	ebffe3ac 	bl	60309c2c <vAssertCalled>
60310d78:	eaffffda 	b	60310ce8 <xQueueGenericSend+0x1b8>

60310d7c <xQueueGenericSendFromISR>:
60310d7c:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60310d80:	e1a06001 	mov	r6, r1
60310d84:	e1a07002 	mov	r7, r2
60310d88:	e1a05003 	mov	r5, r3
60310d8c:	e2504000 	subs	r4, r0, #0
60310d90:	0a000023 	beq	60310e24 <xQueueGenericSendFromISR+0xa8>
60310d94:	e3560000 	cmp	r6, #0
60310d98:	0a000027 	beq	60310e3c <xQueueGenericSendFromISR+0xc0>
60310d9c:	e3550002 	cmp	r5, #2
60310da0:	0a00002d 	beq	60310e5c <xQueueGenericSendFromISR+0xe0>
60310da4:	ebfff771 	bl	6030eb70 <vTaskEnterCritical>
60310da8:	e5942038 	ldr	r2, [r4, #56]	; 0x38
60310dac:	e594303c 	ldr	r3, [r4, #60]	; 0x3c
60310db0:	e1520003 	cmp	r2, r3
60310db4:	23a05000 	movcs	r5, #0
60310db8:	2a00000b 	bcs	60310dec <xQueueGenericSendFromISR+0x70>
60310dbc:	e5d43045 	ldrb	r3, [r4, #69]	; 0x45
60310dc0:	e1a01006 	mov	r1, r6
60310dc4:	e1a02005 	mov	r2, r5
60310dc8:	e6af6073 	sxtb	r6, r3
60310dcc:	e1a00004 	mov	r0, r4
60310dd0:	ebfffe0a 	bl	60310600 <prvCopyDataToQueue>
60310dd4:	e3760001 	cmn	r6, #1
60310dd8:	0a000006 	beq	60310df8 <xQueueGenericSendFromISR+0x7c>
60310ddc:	e2866001 	add	r6, r6, #1
60310de0:	e3a05001 	mov	r5, #1
60310de4:	e6af6076 	sxtb	r6, r6
60310de8:	e5c46045 	strb	r6, [r4, #69]	; 0x45
60310dec:	ebfff77a 	bl	6030ebdc <vTaskExitCritical>
60310df0:	e1a00005 	mov	r0, r5
60310df4:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60310df8:	e5943048 	ldr	r3, [r4, #72]	; 0x48
60310dfc:	e3530000 	cmp	r3, #0
60310e00:	0a00001b 	beq	60310e74 <xQueueGenericSendFromISR+0xf8>
60310e04:	e1a01005 	mov	r1, r5
60310e08:	e1a00004 	mov	r0, r4
60310e0c:	ebfffe36 	bl	603106ec <prvNotifyQueueSetContainer>
60310e10:	e3570000 	cmp	r7, #0
60310e14:	13500000 	cmpne	r0, #0
60310e18:	1a00001d 	bne	60310e94 <xQueueGenericSendFromISR+0x118>
60310e1c:	e3a05001 	mov	r5, #1
60310e20:	eafffff1 	b	60310dec <xQueueGenericSendFromISR+0x70>
60310e24:	e3a01fdd 	mov	r1, #884	; 0x374
60310e28:	e30608d0 	movw	r0, #26832	; 0x68d0
60310e2c:	e3460031 	movt	r0, #24625	; 0x6031
60310e30:	ebffe37d 	bl	60309c2c <vAssertCalled>
60310e34:	e3560000 	cmp	r6, #0
60310e38:	1affffd7 	bne	60310d9c <xQueueGenericSendFromISR+0x20>
60310e3c:	e5943040 	ldr	r3, [r4, #64]	; 0x40
60310e40:	e3530000 	cmp	r3, #0
60310e44:	0affffd4 	beq	60310d9c <xQueueGenericSendFromISR+0x20>
60310e48:	e3001375 	movw	r1, #885	; 0x375
60310e4c:	e30608d0 	movw	r0, #26832	; 0x68d0
60310e50:	e3460031 	movt	r0, #24625	; 0x6031
60310e54:	ebffe374 	bl	60309c2c <vAssertCalled>
60310e58:	eaffffcf 	b	60310d9c <xQueueGenericSendFromISR+0x20>
60310e5c:	e594303c 	ldr	r3, [r4, #60]	; 0x3c
60310e60:	e3530001 	cmp	r3, #1
60310e64:	1a00000e 	bne	60310ea4 <xQueueGenericSendFromISR+0x128>
60310e68:	ebfff740 	bl	6030eb70 <vTaskEnterCritical>
60310e6c:	e5943038 	ldr	r3, [r4, #56]	; 0x38
60310e70:	eaffffd1 	b	60310dbc <xQueueGenericSendFromISR+0x40>
60310e74:	e5943024 	ldr	r3, [r4, #36]	; 0x24
60310e78:	e3530000 	cmp	r3, #0
60310e7c:	0affffe6 	beq	60310e1c <xQueueGenericSendFromISR+0xa0>
60310e80:	e2840024 	add	r0, r4, #36	; 0x24
60310e84:	ebfffb6b 	bl	6030fc38 <xTaskRemoveFromEventList>
60310e88:	e3570000 	cmp	r7, #0
60310e8c:	13500000 	cmpne	r0, #0
60310e90:	0affffe1 	beq	60310e1c <xQueueGenericSendFromISR+0xa0>
60310e94:	e3a03001 	mov	r3, #1
60310e98:	e1a05003 	mov	r5, r3
60310e9c:	e5873000 	str	r3, [r7]
60310ea0:	eaffffd1 	b	60310dec <xQueueGenericSendFromISR+0x70>
60310ea4:	e3001376 	movw	r1, #886	; 0x376
60310ea8:	e30608d0 	movw	r0, #26832	; 0x68d0
60310eac:	e3460031 	movt	r0, #24625	; 0x6031
60310eb0:	ebffe35d 	bl	60309c2c <vAssertCalled>
60310eb4:	eaffffeb 	b	60310e68 <xQueueGenericSendFromISR+0xec>

60310eb8 <xQueueGiveFromISR>:
60310eb8:	e92d4070 	push	{r4, r5, r6, lr}
60310ebc:	e1a05001 	mov	r5, r1
60310ec0:	e2504000 	subs	r4, r0, #0
60310ec4:	0a000023 	beq	60310f58 <xQueueGiveFromISR+0xa0>
60310ec8:	e5943040 	ldr	r3, [r4, #64]	; 0x40
60310ecc:	e3530000 	cmp	r3, #0
60310ed0:	1a000027 	bne	60310f74 <xQueueGiveFromISR+0xbc>
60310ed4:	e5943000 	ldr	r3, [r4]
60310ed8:	e3530000 	cmp	r3, #0
60310edc:	0a00002b 	beq	60310f90 <xQueueGiveFromISR+0xd8>
60310ee0:	ebfff722 	bl	6030eb70 <vTaskEnterCritical>
60310ee4:	e5942038 	ldr	r2, [r4, #56]	; 0x38
60310ee8:	e594303c 	ldr	r3, [r4, #60]	; 0x3c
60310eec:	e1530002 	cmp	r3, r2
60310ef0:	93a06000 	movls	r6, #0
60310ef4:	9a000009 	bls	60310f20 <xQueueGiveFromISR+0x68>
60310ef8:	e5d43045 	ldrb	r3, [r4, #69]	; 0x45
60310efc:	e2822001 	add	r2, r2, #1
60310f00:	e35300ff 	cmp	r3, #255	; 0xff
60310f04:	e5842038 	str	r2, [r4, #56]	; 0x38
60310f08:	e6af3073 	sxtb	r3, r3
60310f0c:	0a000006 	beq	60310f2c <xQueueGiveFromISR+0x74>
60310f10:	e2833001 	add	r3, r3, #1
60310f14:	e3a06001 	mov	r6, #1
60310f18:	e6af3073 	sxtb	r3, r3
60310f1c:	e5c43045 	strb	r3, [r4, #69]	; 0x45
60310f20:	ebfff72d 	bl	6030ebdc <vTaskExitCritical>
60310f24:	e1a00006 	mov	r0, r6
60310f28:	e8bd8070 	pop	{r4, r5, r6, pc}
60310f2c:	e5943048 	ldr	r3, [r4, #72]	; 0x48
60310f30:	e3530000 	cmp	r3, #0
60310f34:	0a00001d 	beq	60310fb0 <xQueueGiveFromISR+0xf8>
60310f38:	e3a01000 	mov	r1, #0
60310f3c:	e1a00004 	mov	r0, r4
60310f40:	ebfffde9 	bl	603106ec <prvNotifyQueueSetContainer>
60310f44:	e3550000 	cmp	r5, #0
60310f48:	13500000 	cmpne	r0, #0
60310f4c:	1a00001f 	bne	60310fd0 <xQueueGiveFromISR+0x118>
60310f50:	e3a06001 	mov	r6, #1
60310f54:	eafffff1 	b	60310f20 <xQueueGiveFromISR+0x68>
60310f58:	e30608d0 	movw	r0, #26832	; 0x68d0
60310f5c:	e3460031 	movt	r0, #24625	; 0x6031
60310f60:	e30013f3 	movw	r1, #1011	; 0x3f3
60310f64:	ebffe330 	bl	60309c2c <vAssertCalled>
60310f68:	e5943040 	ldr	r3, [r4, #64]	; 0x40
60310f6c:	e3530000 	cmp	r3, #0
60310f70:	0affffd7 	beq	60310ed4 <xQueueGiveFromISR+0x1c>
60310f74:	e30608d0 	movw	r0, #26832	; 0x68d0
60310f78:	e3460031 	movt	r0, #24625	; 0x6031
60310f7c:	e30013f7 	movw	r1, #1015	; 0x3f7
60310f80:	ebffe329 	bl	60309c2c <vAssertCalled>
60310f84:	e5943000 	ldr	r3, [r4]
60310f88:	e3530000 	cmp	r3, #0
60310f8c:	1affffd3 	bne	60310ee0 <xQueueGiveFromISR+0x28>
60310f90:	e5943008 	ldr	r3, [r4, #8]
60310f94:	e3530000 	cmp	r3, #0
60310f98:	0affffd0 	beq	60310ee0 <xQueueGiveFromISR+0x28>
60310f9c:	e3a01fff 	mov	r1, #1020	; 0x3fc
60310fa0:	e30608d0 	movw	r0, #26832	; 0x68d0
60310fa4:	e3460031 	movt	r0, #24625	; 0x6031
60310fa8:	ebffe31f 	bl	60309c2c <vAssertCalled>
60310fac:	eaffffcb 	b	60310ee0 <xQueueGiveFromISR+0x28>
60310fb0:	e5943024 	ldr	r3, [r4, #36]	; 0x24
60310fb4:	e3530000 	cmp	r3, #0
60310fb8:	0affffe4 	beq	60310f50 <xQueueGiveFromISR+0x98>
60310fbc:	e2840024 	add	r0, r4, #36	; 0x24
60310fc0:	ebfffb1c 	bl	6030fc38 <xTaskRemoveFromEventList>
60310fc4:	e3550000 	cmp	r5, #0
60310fc8:	13500000 	cmpne	r0, #0
60310fcc:	0affffdf 	beq	60310f50 <xQueueGiveFromISR+0x98>
60310fd0:	e3a03001 	mov	r3, #1
60310fd4:	e1a06003 	mov	r6, r3
60310fd8:	e5853000 	str	r3, [r5]
60310fdc:	eaffffcf 	b	60310f20 <xQueueGiveFromISR+0x68>

60310fe0 <xQueueReceive>:
60310fe0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60310fe4:	e1a06001 	mov	r6, r1
60310fe8:	e24dd010 	sub	sp, sp, #16
60310fec:	e2504000 	subs	r4, r0, #0
60310ff0:	e58d2004 	str	r2, [sp, #4]
60310ff4:	0a000062 	beq	60311184 <xQueueReceive+0x1a4>
60310ff8:	e3560000 	cmp	r6, #0
60310ffc:	0a000066 	beq	6031119c <xQueueReceive+0x1bc>
60311000:	ebfff6da 	bl	6030eb70 <vTaskEnterCritical>
60311004:	e5945038 	ldr	r5, [r4, #56]	; 0x38
60311008:	e3550000 	cmp	r5, #0
6031100c:	1a000045 	bne	60311128 <xQueueReceive+0x148>
60311010:	e59d3004 	ldr	r3, [sp, #4]
60311014:	e3530000 	cmp	r3, #0
60311018:	0a000037 	beq	603110fc <xQueueReceive+0x11c>
6031101c:	e28d0008 	add	r0, sp, #8
60311020:	e1a07005 	mov	r7, r5
60311024:	e2848024 	add	r8, r4, #36	; 0x24
60311028:	ebfff675 	bl	6030ea04 <vTaskInternalSetTimeOutState>
6031102c:	ebfff6ea 	bl	6030ebdc <vTaskExitCritical>
60311030:	ebfffcda 	bl	603103a0 <vTaskSuspendAll>
60311034:	ebfff6cd 	bl	6030eb70 <vTaskEnterCritical>
60311038:	e5d43044 	ldrb	r3, [r4, #68]	; 0x44
6031103c:	e35300ff 	cmp	r3, #255	; 0xff
60311040:	05c47044 	strbeq	r7, [r4, #68]	; 0x44
60311044:	e5d43045 	ldrb	r3, [r4, #69]	; 0x45
60311048:	e35300ff 	cmp	r3, #255	; 0xff
6031104c:	05c47045 	strbeq	r7, [r4, #69]	; 0x45
60311050:	ebfff6e1 	bl	6030ebdc <vTaskExitCritical>
60311054:	e28d1004 	add	r1, sp, #4
60311058:	e28d0008 	add	r0, sp, #8
6031105c:	ebfffb41 	bl	6030fd68 <xTaskCheckForTimeOut>
60311060:	e3500000 	cmp	r0, #0
60311064:	1a000015 	bne	603110c0 <xQueueReceive+0xe0>
60311068:	ebfff6c0 	bl	6030eb70 <vTaskEnterCritical>
6031106c:	e5943038 	ldr	r3, [r4, #56]	; 0x38
60311070:	e3530000 	cmp	r3, #0
60311074:	1a000024 	bne	6031110c <xQueueReceive+0x12c>
60311078:	ebfff6d7 	bl	6030ebdc <vTaskExitCritical>
6031107c:	e59d1004 	ldr	r1, [sp, #4]
60311080:	e1a00008 	mov	r0, r8
60311084:	ebfffa90 	bl	6030facc <vTaskPlaceOnEventList>
60311088:	ebfff6b8 	bl	6030eb70 <vTaskEnterCritical>
6031108c:	e5943038 	ldr	r3, [r4, #56]	; 0x38
60311090:	e3530000 	cmp	r3, #0
60311094:	0a000021 	beq	60311120 <xQueueReceive+0x140>
60311098:	ebfff6cf 	bl	6030ebdc <vTaskExitCritical>
6031109c:	e1a00008 	mov	r0, r8
603110a0:	ebfffac2 	bl	6030fbb0 <vTaskPlaceRemoveFromEventList>
603110a4:	e1a00004 	mov	r0, r4
603110a8:	ebfffdc6 	bl	603107c8 <prvUnlockQueue>
603110ac:	ebfffa80 	bl	6030fab4 <xTaskResumeAll>
603110b0:	e3500000 	cmp	r0, #0
603110b4:	1a000009 	bne	603110e0 <xQueueReceive+0x100>
603110b8:	ebfff697 	bl	6030eb1c <vTaskYieldWithinAPI>
603110bc:	ea000007 	b	603110e0 <xQueueReceive+0x100>
603110c0:	e1a00004 	mov	r0, r4
603110c4:	ebfffdbf 	bl	603107c8 <prvUnlockQueue>
603110c8:	ebfffa79 	bl	6030fab4 <xTaskResumeAll>
603110cc:	ebfff6a7 	bl	6030eb70 <vTaskEnterCritical>
603110d0:	e5943038 	ldr	r3, [r4, #56]	; 0x38
603110d4:	e3530000 	cmp	r3, #0
603110d8:	0a000007 	beq	603110fc <xQueueReceive+0x11c>
603110dc:	ebfff6be 	bl	6030ebdc <vTaskExitCritical>
603110e0:	ebfff6a2 	bl	6030eb70 <vTaskEnterCritical>
603110e4:	e5945038 	ldr	r5, [r4, #56]	; 0x38
603110e8:	e3550000 	cmp	r5, #0
603110ec:	1a00000d 	bne	60311128 <xQueueReceive+0x148>
603110f0:	e59d3004 	ldr	r3, [sp, #4]
603110f4:	e3530000 	cmp	r3, #0
603110f8:	1affffcb 	bne	6031102c <xQueueReceive+0x4c>
603110fc:	ebfff6b6 	bl	6030ebdc <vTaskExitCritical>
60311100:	e3a00000 	mov	r0, #0
60311104:	e28dd010 	add	sp, sp, #16
60311108:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
6031110c:	ebfff6b2 	bl	6030ebdc <vTaskExitCritical>
60311110:	e1a00004 	mov	r0, r4
60311114:	ebfffdab 	bl	603107c8 <prvUnlockQueue>
60311118:	ebfffa65 	bl	6030fab4 <xTaskResumeAll>
6031111c:	eaffffef 	b	603110e0 <xQueueReceive+0x100>
60311120:	ebfff6ad 	bl	6030ebdc <vTaskExitCritical>
60311124:	eaffffde 	b	603110a4 <xQueueReceive+0xc4>
60311128:	ebfff690 	bl	6030eb70 <vTaskEnterCritical>
6031112c:	e5942040 	ldr	r2, [r4, #64]	; 0x40
60311130:	e3520000 	cmp	r2, #0
60311134:	0a000008 	beq	6031115c <xQueueReceive+0x17c>
60311138:	e594100c 	ldr	r1, [r4, #12]
6031113c:	e5943008 	ldr	r3, [r4, #8]
60311140:	e0811002 	add	r1, r1, r2
60311144:	e1510003 	cmp	r1, r3
60311148:	e584100c 	str	r1, [r4, #12]
6031114c:	e1a00006 	mov	r0, r6
60311150:	25941000 	ldrcs	r1, [r4]
60311154:	2584100c 	strcs	r1, [r4, #12]
60311158:	eb000f25 	bl	60314df4 <__wrap_memcpy>
6031115c:	ebfff69e 	bl	6030ebdc <vTaskExitCritical>
60311160:	e2455001 	sub	r5, r5, #1
60311164:	e5845038 	str	r5, [r4, #56]	; 0x38
60311168:	e5943010 	ldr	r3, [r4, #16]
6031116c:	e3530000 	cmp	r3, #0
60311170:	1a000011 	bne	603111bc <xQueueReceive+0x1dc>
60311174:	ebfff698 	bl	6030ebdc <vTaskExitCritical>
60311178:	e3a00001 	mov	r0, #1
6031117c:	e28dd010 	add	sp, sp, #16
60311180:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60311184:	e30608d0 	movw	r0, #26832	; 0x68d0
60311188:	e3460031 	movt	r0, #24625	; 0x6031
6031118c:	e3001476 	movw	r1, #1142	; 0x476
60311190:	ebffe2a5 	bl	60309c2c <vAssertCalled>
60311194:	e3560000 	cmp	r6, #0
60311198:	1affff98 	bne	60311000 <xQueueReceive+0x20>
6031119c:	e5943040 	ldr	r3, [r4, #64]	; 0x40
603111a0:	e3530000 	cmp	r3, #0
603111a4:	0affff95 	beq	60311000 <xQueueReceive+0x20>
603111a8:	e30608d0 	movw	r0, #26832	; 0x68d0
603111ac:	e3460031 	movt	r0, #24625	; 0x6031
603111b0:	e300147a 	movw	r1, #1146	; 0x47a
603111b4:	ebffe29c 	bl	60309c2c <vAssertCalled>
603111b8:	eaffff90 	b	60311000 <xQueueReceive+0x20>
603111bc:	e2840010 	add	r0, r4, #16
603111c0:	ebfffa9c 	bl	6030fc38 <xTaskRemoveFromEventList>
603111c4:	e3500000 	cmp	r0, #0
603111c8:	0affffe9 	beq	60311174 <xQueueReceive+0x194>
603111cc:	ebfff652 	bl	6030eb1c <vTaskYieldWithinAPI>
603111d0:	eaffffe7 	b	60311174 <xQueueReceive+0x194>

603111d4 <xQueueSemaphoreTake>:
603111d4:	e92d40f0 	push	{r4, r5, r6, r7, lr}
603111d8:	e2504000 	subs	r4, r0, #0
603111dc:	e24dd014 	sub	sp, sp, #20
603111e0:	e58d1004 	str	r1, [sp, #4]
603111e4:	0a00007b 	beq	603113d8 <xQueueSemaphoreTake+0x204>
603111e8:	e5943040 	ldr	r3, [r4, #64]	; 0x40
603111ec:	e3530000 	cmp	r3, #0
603111f0:	1a00007f 	bne	603113f4 <xQueueSemaphoreTake+0x220>
603111f4:	ebfff65d 	bl	6030eb70 <vTaskEnterCritical>
603111f8:	e5943038 	ldr	r3, [r4, #56]	; 0x38
603111fc:	e3a06000 	mov	r6, #0
60311200:	e1a05006 	mov	r5, r6
60311204:	e1a07006 	mov	r7, r6
60311208:	e3530000 	cmp	r3, #0
6031120c:	1a00003a 	bne	603112fc <xQueueSemaphoreTake+0x128>
60311210:	e59d3004 	ldr	r3, [sp, #4]
60311214:	e3530000 	cmp	r3, #0
60311218:	0a000067 	beq	603113bc <xQueueSemaphoreTake+0x1e8>
6031121c:	e3550000 	cmp	r5, #0
60311220:	0a000042 	beq	60311330 <xQueueSemaphoreTake+0x15c>
60311224:	ebfff66c 	bl	6030ebdc <vTaskExitCritical>
60311228:	ebfffc5c 	bl	603103a0 <vTaskSuspendAll>
6031122c:	ebfff64f 	bl	6030eb70 <vTaskEnterCritical>
60311230:	e5d43044 	ldrb	r3, [r4, #68]	; 0x44
60311234:	e35300ff 	cmp	r3, #255	; 0xff
60311238:	05c47044 	strbeq	r7, [r4, #68]	; 0x44
6031123c:	e5d43045 	ldrb	r3, [r4, #69]	; 0x45
60311240:	e35300ff 	cmp	r3, #255	; 0xff
60311244:	05c47045 	strbeq	r7, [r4, #69]	; 0x45
60311248:	ebfff663 	bl	6030ebdc <vTaskExitCritical>
6031124c:	e28d1004 	add	r1, sp, #4
60311250:	e28d0008 	add	r0, sp, #8
60311254:	ebfffac3 	bl	6030fd68 <xTaskCheckForTimeOut>
60311258:	e3500000 	cmp	r0, #0
6031125c:	1a000019 	bne	603112c8 <xQueueSemaphoreTake+0xf4>
60311260:	ebfff642 	bl	6030eb70 <vTaskEnterCritical>
60311264:	e5943038 	ldr	r3, [r4, #56]	; 0x38
60311268:	e3530000 	cmp	r3, #0
6031126c:	1a000032 	bne	6031133c <xQueueSemaphoreTake+0x168>
60311270:	ebfff659 	bl	6030ebdc <vTaskExitCritical>
60311274:	e5943000 	ldr	r3, [r4]
60311278:	e3530000 	cmp	r3, #0
6031127c:	0a000048 	beq	603113a4 <xQueueSemaphoreTake+0x1d0>
60311280:	e2845024 	add	r5, r4, #36	; 0x24
60311284:	e59d1004 	ldr	r1, [sp, #4]
60311288:	e1a00005 	mov	r0, r5
6031128c:	ebfffa0e 	bl	6030facc <vTaskPlaceOnEventList>
60311290:	ebfff636 	bl	6030eb70 <vTaskEnterCritical>
60311294:	e5943038 	ldr	r3, [r4, #56]	; 0x38
60311298:	e3530000 	cmp	r3, #0
6031129c:	0a000031 	beq	60311368 <xQueueSemaphoreTake+0x194>
603112a0:	ebfff64d 	bl	6030ebdc <vTaskExitCritical>
603112a4:	e1a00005 	mov	r0, r5
603112a8:	ebfffa40 	bl	6030fbb0 <vTaskPlaceRemoveFromEventList>
603112ac:	e1a00004 	mov	r0, r4
603112b0:	ebfffd44 	bl	603107c8 <prvUnlockQueue>
603112b4:	ebfff9fe 	bl	6030fab4 <xTaskResumeAll>
603112b8:	e3500000 	cmp	r0, #0
603112bc:	1a000009 	bne	603112e8 <xQueueSemaphoreTake+0x114>
603112c0:	ebfff615 	bl	6030eb1c <vTaskYieldWithinAPI>
603112c4:	ea000007 	b	603112e8 <xQueueSemaphoreTake+0x114>
603112c8:	e1a00004 	mov	r0, r4
603112cc:	ebfffd3d 	bl	603107c8 <prvUnlockQueue>
603112d0:	ebfff9f7 	bl	6030fab4 <xTaskResumeAll>
603112d4:	ebfff625 	bl	6030eb70 <vTaskEnterCritical>
603112d8:	e5943038 	ldr	r3, [r4, #56]	; 0x38
603112dc:	e3530000 	cmp	r3, #0
603112e0:	0a00001a 	beq	60311350 <xQueueSemaphoreTake+0x17c>
603112e4:	ebfff63c 	bl	6030ebdc <vTaskExitCritical>
603112e8:	ebfff620 	bl	6030eb70 <vTaskEnterCritical>
603112ec:	e5943038 	ldr	r3, [r4, #56]	; 0x38
603112f0:	e3a05001 	mov	r5, #1
603112f4:	e3530000 	cmp	r3, #0
603112f8:	0affffc4 	beq	60311210 <xQueueSemaphoreTake+0x3c>
603112fc:	e5942000 	ldr	r2, [r4]
60311300:	e2433001 	sub	r3, r3, #1
60311304:	e3520000 	cmp	r2, #0
60311308:	e5843038 	str	r3, [r4, #56]	; 0x38
6031130c:	0a000043 	beq	60311420 <xQueueSemaphoreTake+0x24c>
60311310:	e5943010 	ldr	r3, [r4, #16]
60311314:	e3530000 	cmp	r3, #0
60311318:	1a00003a 	bne	60311408 <xQueueSemaphoreTake+0x234>
6031131c:	e3a06001 	mov	r6, #1
60311320:	ebfff62d 	bl	6030ebdc <vTaskExitCritical>
60311324:	e1a00006 	mov	r0, r6
60311328:	e28dd014 	add	sp, sp, #20
6031132c:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
60311330:	e28d0008 	add	r0, sp, #8
60311334:	ebfff5b2 	bl	6030ea04 <vTaskInternalSetTimeOutState>
60311338:	eaffffb9 	b	60311224 <xQueueSemaphoreTake+0x50>
6031133c:	ebfff626 	bl	6030ebdc <vTaskExitCritical>
60311340:	e1a00004 	mov	r0, r4
60311344:	ebfffd1f 	bl	603107c8 <prvUnlockQueue>
60311348:	ebfff9d9 	bl	6030fab4 <xTaskResumeAll>
6031134c:	eaffffe5 	b	603112e8 <xQueueSemaphoreTake+0x114>
60311350:	ebfff621 	bl	6030ebdc <vTaskExitCritical>
60311354:	e3560000 	cmp	r6, #0
60311358:	1a000004 	bne	60311370 <xQueueSemaphoreTake+0x19c>
6031135c:	e1a00006 	mov	r0, r6
60311360:	e28dd014 	add	sp, sp, #20
60311364:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
60311368:	ebfff61b 	bl	6030ebdc <vTaskExitCritical>
6031136c:	eaffffce 	b	603112ac <xQueueSemaphoreTake+0xd8>
60311370:	ebfff5fe 	bl	6030eb70 <vTaskEnterCritical>
60311374:	e5941024 	ldr	r1, [r4, #36]	; 0x24
60311378:	e5940008 	ldr	r0, [r4, #8]
6031137c:	e3510000 	cmp	r1, #0
60311380:	e3a06000 	mov	r6, #0
60311384:	15943030 	ldrne	r3, [r4, #48]	; 0x30
60311388:	15931000 	ldrne	r1, [r3]
6031138c:	1261100b 	rsbne	r1, r1, #11
60311390:	ebfffb6a 	bl	60310140 <vTaskPriorityDisinheritAfterTimeout>
60311394:	ebfff610 	bl	6030ebdc <vTaskExitCritical>
60311398:	e1a00006 	mov	r0, r6
6031139c:	e28dd014 	add	sp, sp, #20
603113a0:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
603113a4:	ebfff5f1 	bl	6030eb70 <vTaskEnterCritical>
603113a8:	e5940008 	ldr	r0, [r4, #8]
603113ac:	ebfffab7 	bl	6030fe90 <xTaskPriorityInherit>
603113b0:	e1a06000 	mov	r6, r0
603113b4:	ebfff608 	bl	6030ebdc <vTaskExitCritical>
603113b8:	eaffffb0 	b	60311280 <xQueueSemaphoreTake+0xac>
603113bc:	e3560000 	cmp	r6, #0
603113c0:	1a000019 	bne	6031142c <xQueueSemaphoreTake+0x258>
603113c4:	e3a06000 	mov	r6, #0
603113c8:	ebfff603 	bl	6030ebdc <vTaskExitCritical>
603113cc:	e1a00006 	mov	r0, r6
603113d0:	e28dd014 	add	sp, sp, #20
603113d4:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
603113d8:	e3a01e4f 	mov	r1, #1264	; 0x4f0
603113dc:	e30608d0 	movw	r0, #26832	; 0x68d0
603113e0:	e3460031 	movt	r0, #24625	; 0x6031
603113e4:	ebffe210 	bl	60309c2c <vAssertCalled>
603113e8:	e5943040 	ldr	r3, [r4, #64]	; 0x40
603113ec:	e3530000 	cmp	r3, #0
603113f0:	0affff7f 	beq	603111f4 <xQueueSemaphoreTake+0x20>
603113f4:	e30608d0 	movw	r0, #26832	; 0x68d0
603113f8:	e3460031 	movt	r0, #24625	; 0x6031
603113fc:	e30014f4 	movw	r1, #1268	; 0x4f4
60311400:	ebffe209 	bl	60309c2c <vAssertCalled>
60311404:	eaffff7a 	b	603111f4 <xQueueSemaphoreTake+0x20>
60311408:	e2840010 	add	r0, r4, #16
6031140c:	ebfffa09 	bl	6030fc38 <xTaskRemoveFromEventList>
60311410:	e3500000 	cmp	r0, #0
60311414:	0affffc0 	beq	6031131c <xQueueSemaphoreTake+0x148>
60311418:	ebfff5bf 	bl	6030eb1c <vTaskYieldWithinAPI>
6031141c:	eaffffbe 	b	6031131c <xQueueSemaphoreTake+0x148>
60311420:	ebfffc1a 	bl	60310490 <pvTaskIncrementMutexHeldCount>
60311424:	e5840008 	str	r0, [r4, #8]
60311428:	eaffffb8 	b	60311310 <xQueueSemaphoreTake+0x13c>
6031142c:	e30608d0 	movw	r0, #26832	; 0x68d0
60311430:	e3460031 	movt	r0, #24625	; 0x6031
60311434:	e3001533 	movw	r1, #1331	; 0x533
60311438:	ebffe1fb 	bl	60309c2c <vAssertCalled>
6031143c:	eaffffe0 	b	603113c4 <xQueueSemaphoreTake+0x1f0>

60311440 <xQueueReceiveFromISR>:
60311440:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60311444:	e1a06001 	mov	r6, r1
60311448:	e1a08002 	mov	r8, r2
6031144c:	e2504000 	subs	r4, r0, #0
60311450:	0a000027 	beq	603114f4 <xQueueReceiveFromISR+0xb4>
60311454:	e3560000 	cmp	r6, #0
60311458:	0a00002b 	beq	6031150c <xQueueReceiveFromISR+0xcc>
6031145c:	ebfff5c3 	bl	6030eb70 <vTaskEnterCritical>
60311460:	e5945038 	ldr	r5, [r4, #56]	; 0x38
60311464:	e3550000 	cmp	r5, #0
60311468:	1a000002 	bne	60311478 <xQueueReceiveFromISR+0x38>
6031146c:	ebfff5da 	bl	6030ebdc <vTaskExitCritical>
60311470:	e1a00005 	mov	r0, r5
60311474:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60311478:	e5d47044 	ldrb	r7, [r4, #68]	; 0x44
6031147c:	ebfff5bb 	bl	6030eb70 <vTaskEnterCritical>
60311480:	e5942040 	ldr	r2, [r4, #64]	; 0x40
60311484:	e6af7077 	sxtb	r7, r7
60311488:	e3520000 	cmp	r2, #0
6031148c:	1a000009 	bne	603114b8 <xQueueReceiveFromISR+0x78>
60311490:	ebfff5d1 	bl	6030ebdc <vTaskExitCritical>
60311494:	e2455001 	sub	r5, r5, #1
60311498:	e3770001 	cmn	r7, #1
6031149c:	e5845038 	str	r5, [r4, #56]	; 0x38
603114a0:	0a00000e 	beq	603114e0 <xQueueReceiveFromISR+0xa0>
603114a4:	e2877001 	add	r7, r7, #1
603114a8:	e3a05001 	mov	r5, #1
603114ac:	e6af7077 	sxtb	r7, r7
603114b0:	e5c47044 	strb	r7, [r4, #68]	; 0x44
603114b4:	eaffffec 	b	6031146c <xQueueReceiveFromISR+0x2c>
603114b8:	e594100c 	ldr	r1, [r4, #12]
603114bc:	e5943008 	ldr	r3, [r4, #8]
603114c0:	e0811002 	add	r1, r1, r2
603114c4:	e1510003 	cmp	r1, r3
603114c8:	e584100c 	str	r1, [r4, #12]
603114cc:	e1a00006 	mov	r0, r6
603114d0:	25941000 	ldrcs	r1, [r4]
603114d4:	2584100c 	strcs	r1, [r4, #12]
603114d8:	eb000e45 	bl	60314df4 <__wrap_memcpy>
603114dc:	eaffffeb 	b	60311490 <xQueueReceiveFromISR+0x50>
603114e0:	e5943010 	ldr	r3, [r4, #16]
603114e4:	e3530000 	cmp	r3, #0
603114e8:	1a00000f 	bne	6031152c <xQueueReceiveFromISR+0xec>
603114ec:	e3a05001 	mov	r5, #1
603114f0:	eaffffdd 	b	6031146c <xQueueReceiveFromISR+0x2c>
603114f4:	e30608d0 	movw	r0, #26832	; 0x68d0
603114f8:	e3460031 	movt	r0, #24625	; 0x6031
603114fc:	e3001625 	movw	r1, #1573	; 0x625
60311500:	ebffe1c9 	bl	60309c2c <vAssertCalled>
60311504:	e3560000 	cmp	r6, #0
60311508:	1affffd3 	bne	6031145c <xQueueReceiveFromISR+0x1c>
6031150c:	e5943040 	ldr	r3, [r4, #64]	; 0x40
60311510:	e3530000 	cmp	r3, #0
60311514:	0affffd0 	beq	6031145c <xQueueReceiveFromISR+0x1c>
60311518:	e30608d0 	movw	r0, #26832	; 0x68d0
6031151c:	e3460031 	movt	r0, #24625	; 0x6031
60311520:	e3001626 	movw	r1, #1574	; 0x626
60311524:	ebffe1c0 	bl	60309c2c <vAssertCalled>
60311528:	eaffffcb 	b	6031145c <xQueueReceiveFromISR+0x1c>
6031152c:	e2840010 	add	r0, r4, #16
60311530:	ebfff9c0 	bl	6030fc38 <xTaskRemoveFromEventList>
60311534:	e3580000 	cmp	r8, #0
60311538:	13500000 	cmpne	r0, #0
6031153c:	13a03001 	movne	r3, #1
60311540:	11a05003 	movne	r5, r3
60311544:	15883000 	strne	r3, [r8]
60311548:	0affffe7 	beq	603114ec <xQueueReceiveFromISR+0xac>
6031154c:	eaffffc6 	b	6031146c <xQueueReceiveFromISR+0x2c>

60311550 <vQueueAddToRegistry>:
60311550:	e308c1a0 	movw	ip, #33184	; 0x81a0
60311554:	e346c031 	movt	ip, #24625	; 0x6031
60311558:	e3a03000 	mov	r3, #0
6031155c:	ea000002 	b	6031156c <vQueueAddToRegistry+0x1c>
60311560:	e2833001 	add	r3, r3, #1
60311564:	e3530008 	cmp	r3, #8
60311568:	012fff1e 	bxeq	lr
6031156c:	e79c2183 	ldr	r2, [ip, r3, lsl #3]
60311570:	e3520000 	cmp	r2, #0
60311574:	1afffff9 	bne	60311560 <vQueueAddToRegistry+0x10>
60311578:	e08c2183 	add	r2, ip, r3, lsl #3
6031157c:	e78c1183 	str	r1, [ip, r3, lsl #3]
60311580:	e5820004 	str	r0, [r2, #4]
60311584:	e12fff1e 	bx	lr

60311588 <vQueueWaitForMessageRestricted>:
60311588:	e92d4070 	push	{r4, r5, r6, lr}
6031158c:	e1a04000 	mov	r4, r0
60311590:	e1a06001 	mov	r6, r1
60311594:	e1a05002 	mov	r5, r2
60311598:	ebfff574 	bl	6030eb70 <vTaskEnterCritical>
6031159c:	e5d43044 	ldrb	r3, [r4, #68]	; 0x44
603115a0:	e35300ff 	cmp	r3, #255	; 0xff
603115a4:	03a03000 	moveq	r3, #0
603115a8:	05c43044 	strbeq	r3, [r4, #68]	; 0x44
603115ac:	e5d43045 	ldrb	r3, [r4, #69]	; 0x45
603115b0:	e35300ff 	cmp	r3, #255	; 0xff
603115b4:	03a03000 	moveq	r3, #0
603115b8:	05c43045 	strbeq	r3, [r4, #69]	; 0x45
603115bc:	ebfff586 	bl	6030ebdc <vTaskExitCritical>
603115c0:	ebfff56a 	bl	6030eb70 <vTaskEnterCritical>
603115c4:	e5943038 	ldr	r3, [r4, #56]	; 0x38
603115c8:	e3530000 	cmp	r3, #0
603115cc:	0a000003 	beq	603115e0 <vQueueWaitForMessageRestricted+0x58>
603115d0:	ebfff581 	bl	6030ebdc <vTaskExitCritical>
603115d4:	e1a00004 	mov	r0, r4
603115d8:	e8bd4070 	pop	{r4, r5, r6, lr}
603115dc:	eafffc79 	b	603107c8 <prvUnlockQueue>
603115e0:	e2840024 	add	r0, r4, #36	; 0x24
603115e4:	e1a02005 	mov	r2, r5
603115e8:	e1a01006 	mov	r1, r6
603115ec:	ebfff951 	bl	6030fb38 <vTaskPlaceOnEventListRestricted>
603115f0:	ebfff579 	bl	6030ebdc <vTaskExitCritical>
603115f4:	e1a00004 	mov	r0, r4
603115f8:	e8bd4070 	pop	{r4, r5, r6, lr}
603115fc:	eafffc71 	b	603107c8 <prvUnlockQueue>

60311600 <prvCheckForValidListAndQueue>:
60311600:	e92d40f0 	push	{r4, r5, r6, r7, lr}
60311604:	e3084358 	movw	r4, #33624	; 0x8358
60311608:	e3464031 	movt	r4, #24625	; 0x6031
6031160c:	e24dd00c 	sub	sp, sp, #12
60311610:	ebfff556 	bl	6030eb70 <vTaskEnterCritical>
60311614:	e5945000 	ldr	r5, [r4]
60311618:	e3550000 	cmp	r5, #0
6031161c:	0a000002 	beq	6031162c <prvCheckForValidListAndQueue+0x2c>
60311620:	e28dd00c 	add	sp, sp, #12
60311624:	e8bd40f0 	pop	{r4, r5, r6, r7, lr}
60311628:	eafff56b 	b	6030ebdc <vTaskExitCritical>
6031162c:	e30872d8 	movw	r7, #33496	; 0x82d8
60311630:	e3467031 	movt	r7, #24625	; 0x6031
60311634:	e1a00007 	mov	r0, r7
60311638:	e30862ec 	movw	r6, #33516	; 0x82ec
6031163c:	e3466031 	movt	r6, #24625	; 0x6031
60311640:	ebfffbb1 	bl	6031050c <vListInitialise>
60311644:	e1a00006 	mov	r0, r6
60311648:	ebfffbaf 	bl	6031050c <vListInitialise>
6031164c:	e30821e0 	movw	r2, #33248	; 0x81e0
60311650:	e3462031 	movt	r2, #24625	; 0x6031
60311654:	e58d5000 	str	r5, [sp]
60311658:	e3a01010 	mov	r1, #16
6031165c:	e5827000 	str	r7, [r2]
60311660:	e30821e4 	movw	r2, #33252	; 0x81e4
60311664:	e3462031 	movt	r2, #24625	; 0x6031
60311668:	e3a0000f 	mov	r0, #15
6031166c:	e3083304 	movw	r3, #33540	; 0x8304
60311670:	e3463031 	movt	r3, #24625	; 0x6031
60311674:	e5826000 	str	r6, [r2]
60311678:	e30821e8 	movw	r2, #33256	; 0x81e8
6031167c:	e3462031 	movt	r2, #24625	; 0x6031
60311680:	ebfffcbb 	bl	60310974 <xQueueGenericCreateStatic>
60311684:	e3500000 	cmp	r0, #0
60311688:	e5840000 	str	r0, [r4]
6031168c:	0affffe3 	beq	60311620 <prvCheckForValidListAndQueue+0x20>
60311690:	e3061948 	movw	r1, #26952	; 0x6948
60311694:	e3461031 	movt	r1, #24625	; 0x6031
60311698:	ebffffac 	bl	60311550 <vQueueAddToRegistry>
6031169c:	eaffffdf 	b	60311620 <prvCheckForValidListAndQueue+0x20>

603116a0 <xTimerCreateTimerTask>:
603116a0:	e92d4010 	push	{r4, lr}
603116a4:	e24dd020 	sub	sp, sp, #32
603116a8:	ebffffd4 	bl	60311600 <prvCheckForValidListAndQueue>
603116ac:	e3083358 	movw	r3, #33624	; 0x8358
603116b0:	e3463031 	movt	r3, #24625	; 0x6031
603116b4:	e5933000 	ldr	r3, [r3]
603116b8:	e3530000 	cmp	r3, #0
603116bc:	0a00001a 	beq	6031172c <xTimerCreateTimerTask+0x8c>
603116c0:	e28d201c 	add	r2, sp, #28
603116c4:	e28d1018 	add	r1, sp, #24
603116c8:	e28d0014 	add	r0, sp, #20
603116cc:	e3a04000 	mov	r4, #0
603116d0:	e58d4014 	str	r4, [sp, #20]
603116d4:	e58d4018 	str	r4, [sp, #24]
603116d8:	eb0003a4 	bl	60312570 <vApplicationGetTimerTaskMemory>
603116dc:	e59d0018 	ldr	r0, [sp, #24]
603116e0:	e59dc014 	ldr	ip, [sp, #20]
603116e4:	e3a0100a 	mov	r1, #10
603116e8:	e1a03004 	mov	r3, r4
603116ec:	e98d1001 	stmib	sp, {r0, ip}
603116f0:	e58d1000 	str	r1, [sp]
603116f4:	e59d201c 	ldr	r2, [sp, #28]
603116f8:	e3061950 	movw	r1, #26960	; 0x6950
603116fc:	e3461031 	movt	r1, #24625	; 0x6031
60311700:	e30108bc 	movw	r0, #6332	; 0x18bc
60311704:	e3460031 	movt	r0, #24625	; 0x6031
60311708:	ebfff5f3 	bl	6030eedc <xTaskCreateStatic>
6031170c:	e308335c 	movw	r3, #33628	; 0x835c
60311710:	e3463031 	movt	r3, #24625	; 0x6031
60311714:	e1500004 	cmp	r0, r4
60311718:	e5830000 	str	r0, [r3]
6031171c:	13a00001 	movne	r0, #1
60311720:	0a000001 	beq	6031172c <xTimerCreateTimerTask+0x8c>
60311724:	e28dd020 	add	sp, sp, #32
60311728:	e8bd8010 	pop	{r4, pc}
6031172c:	e300110d 	movw	r1, #269	; 0x10d
60311730:	e3060958 	movw	r0, #26968	; 0x6958
60311734:	e3460031 	movt	r0, #24625	; 0x6031
60311738:	ebffe13b 	bl	60309c2c <vAssertCalled>
6031173c:	e3a00000 	mov	r0, #0
60311740:	e28dd020 	add	sp, sp, #32
60311744:	e8bd8010 	pop	{r4, pc}

60311748 <xTimerGenericCommand>:
60311748:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
6031174c:	e1a05001 	mov	r5, r1
60311750:	e24dd010 	sub	sp, sp, #16
60311754:	e1a07002 	mov	r7, r2
60311758:	e1a08003 	mov	r8, r3
6031175c:	e2504000 	subs	r4, r0, #0
60311760:	0a00001b 	beq	603117d4 <xTimerGenericCommand+0x8c>
60311764:	e3086358 	movw	r6, #33624	; 0x8358
60311768:	e3466031 	movt	r6, #24625	; 0x6031
6031176c:	e5960000 	ldr	r0, [r6]
60311770:	e3500000 	cmp	r0, #0
60311774:	0a00000e 	beq	603117b4 <xTimerGenericCommand+0x6c>
60311778:	e3550005 	cmp	r5, #5
6031177c:	e58d7004 	str	r7, [sp, #4]
60311780:	e58d5000 	str	r5, [sp]
60311784:	e58d4008 	str	r4, [sp, #8]
60311788:	ca00000b 	bgt	603117bc <xTimerGenericCommand+0x74>
6031178c:	ebfff4cb 	bl	6030eac0 <xTaskGetSchedulerState>
60311790:	e3500002 	cmp	r0, #2
60311794:	13a03000 	movne	r3, #0
60311798:	03a03000 	moveq	r3, #0
6031179c:	05960000 	ldreq	r0, [r6]
603117a0:	059d2028 	ldreq	r2, [sp, #40]	; 0x28
603117a4:	15960000 	ldrne	r0, [r6]
603117a8:	11a02003 	movne	r2, r3
603117ac:	e1a0100d 	mov	r1, sp
603117b0:	ebfffcde 	bl	60310b30 <xQueueGenericSend>
603117b4:	e28dd010 	add	sp, sp, #16
603117b8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
603117bc:	e3a03000 	mov	r3, #0
603117c0:	e1a02008 	mov	r2, r8
603117c4:	e1a0100d 	mov	r1, sp
603117c8:	ebfffd6b 	bl	60310d7c <xQueueGenericSendFromISR>
603117cc:	e28dd010 	add	sp, sp, #16
603117d0:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
603117d4:	e300117b 	movw	r1, #379	; 0x17b
603117d8:	e3060958 	movw	r0, #26968	; 0x6958
603117dc:	e3460031 	movt	r0, #24625	; 0x6031
603117e0:	ebffe111 	bl	60309c2c <vAssertCalled>
603117e4:	eaffffde 	b	60311764 <xTimerGenericCommand+0x1c>

603117e8 <prvSwitchTimerLists>:
603117e8:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
603117ec:	e30861e0 	movw	r6, #33248	; 0x81e0
603117f0:	e3466031 	movt	r6, #24625	; 0x6031
603117f4:	e3067958 	movw	r7, #26968	; 0x6958
603117f8:	e3467031 	movt	r7, #24625	; 0x6031
603117fc:	e24dd00c 	sub	sp, sp, #12
60311800:	e3a08000 	mov	r8, #0
60311804:	ea00000b 	b	60311838 <prvSwitchTimerLists+0x50>
60311808:	e593300c 	ldr	r3, [r3, #12]
6031180c:	e593400c 	ldr	r4, [r3, #12]
60311810:	e5939000 	ldr	r9, [r3]
60311814:	e2845004 	add	r5, r4, #4
60311818:	e1a00005 	mov	r0, r5
6031181c:	ebfffb67 	bl	603105c0 <uxListRemove>
60311820:	e5943020 	ldr	r3, [r4, #32]
60311824:	e1a00004 	mov	r0, r4
60311828:	e12fff33 	blx	r3
6031182c:	e5d43028 	ldrb	r3, [r4, #40]	; 0x28
60311830:	e3130004 	tst	r3, #4
60311834:	1a00000a 	bne	60311864 <prvSwitchTimerLists+0x7c>
60311838:	e5963000 	ldr	r3, [r6]
6031183c:	e5932000 	ldr	r2, [r3]
60311840:	e3520000 	cmp	r2, #0
60311844:	1affffef 	bne	60311808 <prvSwitchTimerLists+0x20>
60311848:	e30821e4 	movw	r2, #33252	; 0x81e4
6031184c:	e3462031 	movt	r2, #24625	; 0x6031
60311850:	e5921000 	ldr	r1, [r2]
60311854:	e5861000 	str	r1, [r6]
60311858:	e5823000 	str	r3, [r2]
6031185c:	e28dd00c 	add	sp, sp, #12
60311860:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
60311864:	e594c018 	ldr	ip, [r4, #24]
60311868:	e3a03000 	mov	r3, #0
6031186c:	e089c00c 	add	ip, r9, ip
60311870:	e1a00004 	mov	r0, r4
60311874:	e1a01005 	mov	r1, r5
60311878:	e159000c 	cmp	r9, ip
6031187c:	e1a02009 	mov	r2, r9
60311880:	2a000004 	bcs	60311898 <prvSwitchTimerLists+0xb0>
60311884:	e5960000 	ldr	r0, [r6]
60311888:	e584c004 	str	ip, [r4, #4]
6031188c:	e5844010 	str	r4, [r4, #16]
60311890:	ebfffb34 	bl	60310568 <vListInsert>
60311894:	eaffffe7 	b	60311838 <prvSwitchTimerLists+0x50>
60311898:	e1a01003 	mov	r1, r3
6031189c:	e58d8000 	str	r8, [sp]
603118a0:	ebffffa8 	bl	60311748 <xTimerGenericCommand>
603118a4:	e3500000 	cmp	r0, #0
603118a8:	1affffe2 	bne	60311838 <prvSwitchTimerLists+0x50>
603118ac:	e1a00007 	mov	r0, r7
603118b0:	e3001342 	movw	r1, #834	; 0x342
603118b4:	ebffe0dc 	bl	60309c2c <vAssertCalled>
603118b8:	eaffffde 	b	60311838 <prvSwitchTimerLists+0x50>

603118bc <prvTimerTask>:
603118bc:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
603118c0:	e30861e0 	movw	r6, #33248	; 0x81e0
603118c4:	e3466031 	movt	r6, #24625	; 0x6031
603118c8:	e24dd024 	sub	sp, sp, #36	; 0x24
603118cc:	e3085300 	movw	r5, #33536	; 0x8300
603118d0:	e3465031 	movt	r5, #24625	; 0x6031
603118d4:	e30871e4 	movw	r7, #33252	; 0x81e4
603118d8:	e3467031 	movt	r7, #24625	; 0x6031
603118dc:	e3084358 	movw	r4, #33624	; 0x8358
603118e0:	e3464031 	movt	r4, #24625	; 0x6031
603118e4:	e3063958 	movw	r3, #26968	; 0x6958
603118e8:	e3463031 	movt	r3, #24625	; 0x6031
603118ec:	e58d300c 	str	r3, [sp, #12]
603118f0:	e5963000 	ldr	r3, [r6]
603118f4:	e5939000 	ldr	r9, [r3]
603118f8:	e3590000 	cmp	r9, #0
603118fc:	0a000084 	beq	60311b14 <prvTimerTask+0x258>
60311900:	e593300c 	ldr	r3, [r3, #12]
60311904:	e5939000 	ldr	r9, [r3]
60311908:	ebfffaa4 	bl	603103a0 <vTaskSuspendAll>
6031190c:	ebfff220 	bl	6030e194 <xTaskGetTickCount>
60311910:	e5953000 	ldr	r3, [r5]
60311914:	e1a0a000 	mov	sl, r0
60311918:	e1500003 	cmp	r0, r3
6031191c:	3a000082 	bcc	60311b2c <prvTimerTask+0x270>
60311920:	e1590000 	cmp	r9, r0
60311924:	83a02000 	movhi	r2, #0
60311928:	e5850000 	str	r0, [r5]
6031192c:	8a000087 	bhi	60311b50 <prvTimerTask+0x294>
60311930:	ebfff85f 	bl	6030fab4 <xTaskResumeAll>
60311934:	e5963000 	ldr	r3, [r6]
60311938:	e593300c 	ldr	r3, [r3, #12]
6031193c:	e593b00c 	ldr	fp, [r3, #12]
60311940:	e28b8004 	add	r8, fp, #4
60311944:	e1a00008 	mov	r0, r8
60311948:	ebfffb1c 	bl	603105c0 <uxListRemove>
6031194c:	e5db3028 	ldrb	r3, [fp, #40]	; 0x28
60311950:	e3130004 	tst	r3, #4
60311954:	03c33001 	biceq	r3, r3, #1
60311958:	05cb3028 	strbeq	r3, [fp, #40]	; 0x28
6031195c:	1a00008c 	bne	60311b94 <prvTimerTask+0x2d8>
60311960:	e59b3020 	ldr	r3, [fp, #32]
60311964:	e1a0000b 	mov	r0, fp
60311968:	e12fff33 	blx	r3
6031196c:	e5940000 	ldr	r0, [r4]
60311970:	e3a02000 	mov	r2, #0
60311974:	e28d1010 	add	r1, sp, #16
60311978:	ebfffd98 	bl	60310fe0 <xQueueReceive>
6031197c:	e3500000 	cmp	r0, #0
60311980:	0affffda 	beq	603118f0 <prvTimerTask+0x34>
60311984:	e59d3010 	ldr	r3, [sp, #16]
60311988:	e59d0018 	ldr	r0, [sp, #24]
6031198c:	e3530000 	cmp	r3, #0
60311990:	ba000058 	blt	60311af8 <prvTimerTask+0x23c>
60311994:	e59da018 	ldr	sl, [sp, #24]
60311998:	e59a3014 	ldr	r3, [sl, #20]
6031199c:	e3530000 	cmp	r3, #0
603119a0:	0a000001 	beq	603119ac <prvTimerTask+0xf0>
603119a4:	e28a0004 	add	r0, sl, #4
603119a8:	ebfffb04 	bl	603105c0 <uxListRemove>
603119ac:	ebfff1f8 	bl	6030e194 <xTaskGetTickCount>
603119b0:	e5953000 	ldr	r3, [r5]
603119b4:	e1a0b000 	mov	fp, r0
603119b8:	e1500003 	cmp	r0, r3
603119bc:	3a00006b 	bcc	60311b70 <prvTimerTask+0x2b4>
603119c0:	e59d3010 	ldr	r3, [sp, #16]
603119c4:	e585b000 	str	fp, [r5]
603119c8:	e3530009 	cmp	r3, #9
603119cc:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
603119d0:	eaffffe5 	b	6031196c <prvTimerTask+0xb0>
603119d4:	603119fc 	.word	0x603119fc
603119d8:	603119fc 	.word	0x603119fc
603119dc:	603119fc 	.word	0x603119fc
603119e0:	60311ac8 	.word	0x60311ac8
603119e4:	60311a88 	.word	0x60311a88
603119e8:	60311ad8 	.word	0x60311ad8
603119ec:	603119fc 	.word	0x603119fc
603119f0:	603119fc 	.word	0x603119fc
603119f4:	60311ac8 	.word	0x60311ac8
603119f8:	60311a88 	.word	0x60311a88
603119fc:	e59d3014 	ldr	r3, [sp, #20]
60311a00:	e59a1018 	ldr	r1, [sl, #24]
60311a04:	e5da2028 	ldrb	r2, [sl, #40]	; 0x28
60311a08:	e0930001 	adds	r0, r3, r1
60311a0c:	e3822001 	orr	r2, r2, #1
60311a10:	e58a0004 	str	r0, [sl, #4]
60311a14:	e5ca2028 	strb	r2, [sl, #40]	; 0x28
60311a18:	e58aa010 	str	sl, [sl, #16]
60311a1c:	23a02001 	movcs	r2, #1
60311a20:	33a02000 	movcc	r2, #0
60311a24:	e150000b 	cmp	r0, fp
60311a28:	8a000052 	bhi	60311b78 <prvTimerTask+0x2bc>
60311a2c:	e04b3003 	sub	r3, fp, r3
60311a30:	e1510003 	cmp	r1, r3
60311a34:	8a000076 	bhi	60311c14 <prvTimerTask+0x358>
60311a38:	e59a3020 	ldr	r3, [sl, #32]
60311a3c:	e1a0000a 	mov	r0, sl
60311a40:	e12fff33 	blx	r3
60311a44:	e5da3028 	ldrb	r3, [sl, #40]	; 0x28
60311a48:	e3130004 	tst	r3, #4
60311a4c:	0affffc6 	beq	6031196c <prvTimerTask+0xb0>
60311a50:	e59a2018 	ldr	r2, [sl, #24]
60311a54:	e59d1014 	ldr	r1, [sp, #20]
60311a58:	e3a03000 	mov	r3, #0
60311a5c:	e0812002 	add	r2, r1, r2
60311a60:	e1a0000a 	mov	r0, sl
60311a64:	e1a01003 	mov	r1, r3
60311a68:	e58d3000 	str	r3, [sp]
60311a6c:	ebffff35 	bl	60311748 <xTimerGenericCommand>
60311a70:	e3500000 	cmp	r0, #0
60311a74:	1affffbc 	bne	6031196c <prvTimerTask+0xb0>
60311a78:	e59d000c 	ldr	r0, [sp, #12]
60311a7c:	e3a01fb7 	mov	r1, #732	; 0x2dc
60311a80:	ebffe069 	bl	60309c2c <vAssertCalled>
60311a84:	eaffffb8 	b	6031196c <prvTimerTask+0xb0>
60311a88:	e5da2028 	ldrb	r2, [sl, #40]	; 0x28
60311a8c:	e59d3014 	ldr	r3, [sp, #20]
60311a90:	e3822001 	orr	r2, r2, #1
60311a94:	e3530000 	cmp	r3, #0
60311a98:	e5ca2028 	strb	r2, [sl, #40]	; 0x28
60311a9c:	e58a3018 	str	r3, [sl, #24]
60311aa0:	0a000050 	beq	60311be8 <prvTimerTask+0x32c>
60311aa4:	e083300b 	add	r3, r3, fp
60311aa8:	e153000b 	cmp	r3, fp
60311aac:	e58a3004 	str	r3, [sl, #4]
60311ab0:	e58aa010 	str	sl, [sl, #16]
60311ab4:	9a000056 	bls	60311c14 <prvTimerTask+0x358>
60311ab8:	e5960000 	ldr	r0, [r6]
60311abc:	e28a1004 	add	r1, sl, #4
60311ac0:	ebfffaa8 	bl	60310568 <vListInsert>
60311ac4:	eaffffa8 	b	6031196c <prvTimerTask+0xb0>
60311ac8:	e5da3028 	ldrb	r3, [sl, #40]	; 0x28
60311acc:	e3c33001 	bic	r3, r3, #1
60311ad0:	e5ca3028 	strb	r3, [sl, #40]	; 0x28
60311ad4:	eaffffa4 	b	6031196c <prvTimerTask+0xb0>
60311ad8:	e5da3028 	ldrb	r3, [sl, #40]	; 0x28
60311adc:	e3130002 	tst	r3, #2
60311ae0:	13c33001 	bicne	r3, r3, #1
60311ae4:	15ca3028 	strbne	r3, [sl, #40]	; 0x28
60311ae8:	1affff9f 	bne	6031196c <prvTimerTask+0xb0>
60311aec:	e1a0000a 	mov	r0, sl
60311af0:	eb000074 	bl	60311cc8 <vPortFree>
60311af4:	eaffff9c 	b	6031196c <prvTimerTask+0xb0>
60311af8:	e59d3014 	ldr	r3, [sp, #20]
60311afc:	e59d101c 	ldr	r1, [sp, #28]
60311b00:	e12fff33 	blx	r3
60311b04:	e59d3010 	ldr	r3, [sp, #16]
60311b08:	e3530000 	cmp	r3, #0
60311b0c:	baffff96 	blt	6031196c <prvTimerTask+0xb0>
60311b10:	eaffff9f 	b	60311994 <prvTimerTask+0xd8>
60311b14:	ebfffa21 	bl	603103a0 <vTaskSuspendAll>
60311b18:	ebfff19d 	bl	6030e194 <xTaskGetTickCount>
60311b1c:	e5953000 	ldr	r3, [r5]
60311b20:	e1a0a000 	mov	sl, r0
60311b24:	e1500003 	cmp	r0, r3
60311b28:	2a000003 	bcs	60311b3c <prvTimerTask+0x280>
60311b2c:	ebffff2d 	bl	603117e8 <prvSwitchTimerLists>
60311b30:	e585a000 	str	sl, [r5]
60311b34:	ebfff7de 	bl	6030fab4 <xTaskResumeAll>
60311b38:	eaffff8b 	b	6031196c <prvTimerTask+0xb0>
60311b3c:	e5973000 	ldr	r3, [r7]
60311b40:	e585a000 	str	sl, [r5]
60311b44:	e5932000 	ldr	r2, [r3]
60311b48:	e16f2f12 	clz	r2, r2
60311b4c:	e1a022a2 	lsr	r2, r2, #5
60311b50:	e5940000 	ldr	r0, [r4]
60311b54:	e049100a 	sub	r1, r9, sl
60311b58:	ebfffe8a 	bl	60311588 <vQueueWaitForMessageRestricted>
60311b5c:	ebfff7d4 	bl	6030fab4 <xTaskResumeAll>
60311b60:	e3500000 	cmp	r0, #0
60311b64:	1affff80 	bne	6031196c <prvTimerTask+0xb0>
60311b68:	ebfff3eb 	bl	6030eb1c <vTaskYieldWithinAPI>
60311b6c:	eaffff7e 	b	6031196c <prvTimerTask+0xb0>
60311b70:	ebffff1c 	bl	603117e8 <prvSwitchTimerLists>
60311b74:	eaffff91 	b	603119c0 <prvTimerTask+0x104>
60311b78:	e2222001 	eor	r2, r2, #1
60311b7c:	e153000b 	cmp	r3, fp
60311b80:	93a02000 	movls	r2, #0
60311b84:	82022001 	andhi	r2, r2, #1
60311b88:	e3520000 	cmp	r2, #0
60311b8c:	1affffa9 	bne	60311a38 <prvTimerTask+0x17c>
60311b90:	eaffffc8 	b	60311ab8 <prvTimerTask+0x1fc>
60311b94:	e59b0018 	ldr	r0, [fp, #24]
60311b98:	e58bb010 	str	fp, [fp, #16]
60311b9c:	e0893000 	add	r3, r9, r0
60311ba0:	e15a0003 	cmp	sl, r3
60311ba4:	e58b3004 	str	r3, [fp, #4]
60311ba8:	3a00001d 	bcc	60311c24 <prvTimerTask+0x368>
60311bac:	e04aa009 	sub	sl, sl, r9
60311bb0:	e150000a 	cmp	r0, sl
60311bb4:	8a00001e 	bhi	60311c34 <prvTimerTask+0x378>
60311bb8:	e3a03000 	mov	r3, #0
60311bbc:	e1a02009 	mov	r2, r9
60311bc0:	e1a01003 	mov	r1, r3
60311bc4:	e1a0000b 	mov	r0, fp
60311bc8:	e58d3000 	str	r3, [sp]
60311bcc:	ebfffedd 	bl	60311748 <xTimerGenericCommand>
60311bd0:	e3500000 	cmp	r0, #0
60311bd4:	1affff61 	bne	60311960 <prvTimerTask+0xa4>
60311bd8:	e59d000c 	ldr	r0, [sp, #12]
60311bdc:	e3a01f79 	mov	r1, #484	; 0x1e4
60311be0:	ebffe011 	bl	60309c2c <vAssertCalled>
60311be4:	eaffff5d 	b	60311960 <prvTimerTask+0xa4>
60311be8:	e59d000c 	ldr	r0, [sp, #12]
60311bec:	e3a01e2f 	mov	r1, #752	; 0x2f0
60311bf0:	ebffe00d 	bl	60309c2c <vAssertCalled>
60311bf4:	e59a3018 	ldr	r3, [sl, #24]
60311bf8:	e58aa010 	str	sl, [sl, #16]
60311bfc:	e083200b 	add	r2, r3, fp
60311c00:	e152000b 	cmp	r2, fp
60311c04:	e58a2004 	str	r2, [sl, #4]
60311c08:	8affffaa 	bhi	60311ab8 <prvTimerTask+0x1fc>
60311c0c:	e3530000 	cmp	r3, #0
60311c10:	0affff55 	beq	6031196c <prvTimerTask+0xb0>
60311c14:	e5970000 	ldr	r0, [r7]
60311c18:	e28a1004 	add	r1, sl, #4
60311c1c:	ebfffa51 	bl	60310568 <vListInsert>
60311c20:	eaffff51 	b	6031196c <prvTimerTask+0xb0>
60311c24:	e5960000 	ldr	r0, [r6]
60311c28:	e1a01008 	mov	r1, r8
60311c2c:	ebfffa4d 	bl	60310568 <vListInsert>
60311c30:	eaffff4a 	b	60311960 <prvTimerTask+0xa4>
60311c34:	e5970000 	ldr	r0, [r7]
60311c38:	e1a01008 	mov	r1, r8
60311c3c:	ebfffa49 	bl	60310568 <vListInsert>
60311c40:	eaffff46 	b	60311960 <prvTimerTask+0xa4>

60311c44 <prvInsertBlockIntoFreeList>:
60311c44:	e3083378 	movw	r3, #33656	; 0x8378
60311c48:	e3463031 	movt	r3, #24625	; 0x6031
60311c4c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
60311c50:	e1a02003 	mov	r2, r3
60311c54:	e5933000 	ldr	r3, [r3]
60311c58:	e1530000 	cmp	r3, r0
60311c5c:	3afffffb 	bcc	60311c50 <prvInsertBlockIntoFreeList+0xc>
60311c60:	e592c004 	ldr	ip, [r2, #4]
60311c64:	e5901004 	ldr	r1, [r0, #4]
60311c68:	e082e00c 	add	lr, r2, ip
60311c6c:	e150000e 	cmp	r0, lr
60311c70:	0081100c 	addeq	r1, r1, ip
60311c74:	01a00002 	moveq	r0, r2
60311c78:	e080c001 	add	ip, r0, r1
60311c7c:	05821004 	streq	r1, [r2, #4]
60311c80:	e153000c 	cmp	r3, ip
60311c84:	0a000003 	beq	60311c98 <prvInsertBlockIntoFreeList+0x54>
60311c88:	e5803000 	str	r3, [r0]
60311c8c:	e1500002 	cmp	r0, r2
60311c90:	15820000 	strne	r0, [r2]
60311c94:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)
60311c98:	e308c360 	movw	ip, #33632	; 0x8360
60311c9c:	e346c031 	movt	ip, #24625	; 0x6031
60311ca0:	e59cc000 	ldr	ip, [ip]
60311ca4:	e153000c 	cmp	r3, ip
60311ca8:	0afffff6 	beq	60311c88 <prvInsertBlockIntoFreeList+0x44>
60311cac:	e5933004 	ldr	r3, [r3, #4]
60311cb0:	e0831001 	add	r1, r3, r1
60311cb4:	e5801004 	str	r1, [r0, #4]
60311cb8:	e5923000 	ldr	r3, [r2]
60311cbc:	e5933000 	ldr	r3, [r3]
60311cc0:	e5803000 	str	r3, [r0]
60311cc4:	eafffff0 	b	60311c8c <prvInsertBlockIntoFreeList+0x48>

60311cc8 <vPortFree>:
60311cc8:	e92d4070 	push	{r4, r5, r6, lr}
60311ccc:	e2504000 	subs	r4, r0, #0
60311cd0:	08bd8070 	popeq	{r4, r5, r6, pc}
60311cd4:	e3085364 	movw	r5, #33636	; 0x8364
60311cd8:	e3465031 	movt	r5, #24625	; 0x6031
60311cdc:	e514303c 	ldr	r3, [r4, #-60]	; 0xffffffc4
60311ce0:	e5952000 	ldr	r2, [r5]
60311ce4:	e1130002 	tst	r3, r2
60311ce8:	0a00001f 	beq	60311d6c <vPortFree+0xa4>
60311cec:	e5141040 	ldr	r1, [r4, #-64]	; 0xffffffc0
60311cf0:	e3510000 	cmp	r1, #0
60311cf4:	0a00000a 	beq	60311d24 <vPortFree+0x5c>
60311cf8:	e30012d3 	movw	r1, #723	; 0x2d3
60311cfc:	e30609d0 	movw	r0, #27088	; 0x69d0
60311d00:	e3460031 	movt	r0, #24625	; 0x6031
60311d04:	ebffdfc8 	bl	60309c2c <vAssertCalled>
60311d08:	e514303c 	ldr	r3, [r4, #-60]	; 0xffffffc4
60311d0c:	e5952000 	ldr	r2, [r5]
60311d10:	e1130002 	tst	r3, r2
60311d14:	08bd8070 	popeq	{r4, r5, r6, pc}
60311d18:	e5141040 	ldr	r1, [r4, #-64]	; 0xffffffc0
60311d1c:	e3510000 	cmp	r1, #0
60311d20:	18bd8070 	popne	{r4, r5, r6, pc}
60311d24:	e1c33002 	bic	r3, r3, r2
60311d28:	e504303c 	str	r3, [r4, #-60]	; 0xffffffc4
60311d2c:	ebfff38f 	bl	6030eb70 <vTaskEnterCritical>
60311d30:	e3083368 	movw	r3, #33640	; 0x8368
60311d34:	e3463031 	movt	r3, #24625	; 0x6031
60311d38:	e514203c 	ldr	r2, [r4, #-60]	; 0xffffffc4
60311d3c:	e2440040 	sub	r0, r4, #64	; 0x40
60311d40:	e5931000 	ldr	r1, [r3]
60311d44:	e0822001 	add	r2, r2, r1
60311d48:	e5832000 	str	r2, [r3]
60311d4c:	ebffffbc 	bl	60311c44 <prvInsertBlockIntoFreeList>
60311d50:	e3083374 	movw	r3, #33652	; 0x8374
60311d54:	e3463031 	movt	r3, #24625	; 0x6031
60311d58:	e5932000 	ldr	r2, [r3]
60311d5c:	e2822001 	add	r2, r2, #1
60311d60:	e5832000 	str	r2, [r3]
60311d64:	e8bd4070 	pop	{r4, r5, r6, lr}
60311d68:	eafff39b 	b	6030ebdc <vTaskExitCritical>
60311d6c:	e30012d2 	movw	r1, #722	; 0x2d2
60311d70:	e30609d0 	movw	r0, #27088	; 0x69d0
60311d74:	e3460031 	movt	r0, #24625	; 0x6031
60311d78:	ebffdfab 	bl	60309c2c <vAssertCalled>
60311d7c:	e5143040 	ldr	r3, [r4, #-64]	; 0xffffffc0
60311d80:	e3530000 	cmp	r3, #0
60311d84:	1affffdb 	bne	60311cf8 <vPortFree+0x30>
60311d88:	e514303c 	ldr	r3, [r4, #-60]	; 0xffffffc4
60311d8c:	e5952000 	ldr	r2, [r5]
60311d90:	e1130002 	tst	r3, r2
60311d94:	08bd8070 	popeq	{r4, r5, r6, pc}
60311d98:	eaffffe1 	b	60311d24 <vPortFree+0x5c>

60311d9c <vPortDefineHeapRegions>:
60311d9c:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60311da0:	e3089360 	movw	r9, #33632	; 0x8360
60311da4:	e3469031 	movt	r9, #24625	; 0x6031
60311da8:	e24dd00c 	sub	sp, sp, #12
60311dac:	e1a06000 	mov	r6, r0
60311db0:	e5993000 	ldr	r3, [r9]
60311db4:	e3530000 	cmp	r3, #0
60311db8:	1a00004c 	bne	60311ef0 <vPortDefineHeapRegions+0x154>
60311dbc:	e5902004 	ldr	r2, [r0, #4]
60311dc0:	e3520000 	cmp	r2, #0
60311dc4:	0a000051 	beq	60311f10 <vPortDefineHeapRegions+0x174>
60311dc8:	e3a07000 	mov	r7, #0
60311dcc:	e306b9d0 	movw	fp, #27088	; 0x69d0
60311dd0:	e346b031 	movt	fp, #24625	; 0x6031
60311dd4:	e308a378 	movw	sl, #33656	; 0x8378
60311dd8:	e346a031 	movt	sl, #24625	; 0x6031
60311ddc:	e2866008 	add	r6, r6, #8
60311de0:	e1a08007 	mov	r8, r7
60311de4:	ea000014 	b	60311e3c <vPortDefineHeapRegions+0xa0>
60311de8:	e1a01003 	mov	r1, r3
60311dec:	e88a0090 	stm	sl, {r4, r7}
60311df0:	e0823005 	add	r3, r2, r5
60311df4:	e2433040 	sub	r3, r3, #64	; 0x40
60311df8:	e3c3303f 	bic	r3, r3, #63	; 0x3f
60311dfc:	e3a00000 	mov	r0, #0
60311e00:	e1510000 	cmp	r1, r0
60311e04:	e5830004 	str	r0, [r3, #4]
60311e08:	e5830000 	str	r0, [r3]
60311e0c:	e0432005 	sub	r2, r3, r5
60311e10:	e5843000 	str	r3, [r4]
60311e14:	e5842004 	str	r2, [r4, #4]
60311e18:	15814000 	strne	r4, [r1]
60311e1c:	e2866008 	add	r6, r6, #8
60311e20:	e5162004 	ldr	r2, [r6, #-4]
60311e24:	e5941004 	ldr	r1, [r4, #4]
60311e28:	e2877001 	add	r7, r7, #1
60311e2c:	e0888001 	add	r8, r8, r1
60311e30:	e3520000 	cmp	r2, #0
60311e34:	e5893000 	str	r3, [r9]
60311e38:	0a000017 	beq	60311e9c <vPortDefineHeapRegions+0x100>
60311e3c:	e5164008 	ldr	r4, [r6, #-8]
60311e40:	e1a05004 	mov	r5, r4
60311e44:	e314003f 	tst	r4, #63	; 0x3f
60311e48:	0a000005 	beq	60311e64 <vPortDefineHeapRegions+0xc8>
60311e4c:	e284103f 	add	r1, r4, #63	; 0x3f
60311e50:	e3c1103f 	bic	r1, r1, #63	; 0x3f
60311e54:	e0822004 	add	r2, r2, r4
60311e58:	e0422001 	sub	r2, r2, r1
60311e5c:	e1a04001 	mov	r4, r1
60311e60:	e1a05001 	mov	r5, r1
60311e64:	e3570000 	cmp	r7, #0
60311e68:	0affffde 	beq	60311de8 <vPortDefineHeapRegions+0x4c>
60311e6c:	e3530000 	cmp	r3, #0
60311e70:	0a000017 	beq	60311ed4 <vPortDefineHeapRegions+0x138>
60311e74:	e1530005 	cmp	r3, r5
60311e78:	31a01003 	movcc	r1, r3
60311e7c:	3affffdb 	bcc	60311df0 <vPortDefineHeapRegions+0x54>
60311e80:	e3a01fde 	mov	r1, #888	; 0x378
60311e84:	e1a0000b 	mov	r0, fp
60311e88:	e58d2004 	str	r2, [sp, #4]
60311e8c:	ebffdf66 	bl	60309c2c <vAssertCalled>
60311e90:	e5991000 	ldr	r1, [r9]
60311e94:	e59d2004 	ldr	r2, [sp, #4]
60311e98:	eaffffd4 	b	60311df0 <vPortDefineHeapRegions+0x54>
60311e9c:	e308236c 	movw	r2, #33644	; 0x836c
60311ea0:	e3462031 	movt	r2, #24625	; 0x6031
60311ea4:	e3083368 	movw	r3, #33640	; 0x8368
60311ea8:	e3463031 	movt	r3, #24625	; 0x6031
60311eac:	e3580000 	cmp	r8, #0
60311eb0:	e5828000 	str	r8, [r2]
60311eb4:	e5838000 	str	r8, [r3]
60311eb8:	0a00001b 	beq	60311f2c <vPortDefineHeapRegions+0x190>
60311ebc:	e3083364 	movw	r3, #33636	; 0x8364
60311ec0:	e3463031 	movt	r3, #24625	; 0x6031
60311ec4:	e3a02102 	mov	r2, #-2147483648	; 0x80000000
60311ec8:	e5832000 	str	r2, [r3]
60311ecc:	e28dd00c 	add	sp, sp, #12
60311ed0:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
60311ed4:	e1a0000b 	mov	r0, fp
60311ed8:	e3001375 	movw	r1, #885	; 0x375
60311edc:	e58d2004 	str	r2, [sp, #4]
60311ee0:	ebffdf51 	bl	60309c2c <vAssertCalled>
60311ee4:	e5993000 	ldr	r3, [r9]
60311ee8:	e59d2004 	ldr	r2, [sp, #4]
60311eec:	eaffffe0 	b	60311e74 <vPortDefineHeapRegions+0xd8>
60311ef0:	e3001352 	movw	r1, #850	; 0x352
60311ef4:	e30609d0 	movw	r0, #27088	; 0x69d0
60311ef8:	e3460031 	movt	r0, #24625	; 0x6031
60311efc:	ebffdf4a 	bl	60309c2c <vAssertCalled>
60311f00:	e5962004 	ldr	r2, [r6, #4]
60311f04:	e3520000 	cmp	r2, #0
60311f08:	15993000 	ldrne	r3, [r9]
60311f0c:	1affffad 	bne	60311dc8 <vPortDefineHeapRegions+0x2c>
60311f10:	e308236c 	movw	r2, #33644	; 0x836c
60311f14:	e3462031 	movt	r2, #24625	; 0x6031
60311f18:	e3083368 	movw	r3, #33640	; 0x8368
60311f1c:	e3463031 	movt	r3, #24625	; 0x6031
60311f20:	e3a01000 	mov	r1, #0
60311f24:	e5821000 	str	r1, [r2]
60311f28:	e5831000 	str	r1, [r3]
60311f2c:	e30609d0 	movw	r0, #27088	; 0x69d0
60311f30:	e3460031 	movt	r0, #24625	; 0x6031
60311f34:	e30013b7 	movw	r1, #951	; 0x3b7
60311f38:	ebffdf3b 	bl	60309c2c <vAssertCalled>
60311f3c:	eaffffde 	b	60311ebc <vPortDefineHeapRegions+0x120>

60311f40 <vApplicationMallocFailedHook>:
60311f40:	e92d4030 	push	{r4, r5, lr}
60311f44:	e1a05000 	mov	r5, r0
60311f48:	e24dd014 	sub	sp, sp, #20
60311f4c:	ebfff2db 	bl	6030eac0 <xTaskGetSchedulerState>
60311f50:	e3500001 	cmp	r0, #1
60311f54:	03064a58 	movweq	r4, #27224	; 0x6a58
60311f58:	03464031 	movteq	r4, #24625	; 0x6031
60311f5c:	0a000002 	beq	60311f6c <vApplicationMallocFailedHook+0x2c>
60311f60:	e3a00000 	mov	r0, #0
60311f64:	ebfff09a 	bl	6030e1d4 <pcTaskGetName>
60311f68:	e1a04000 	mov	r4, r0
60311f6c:	ebfff2ff 	bl	6030eb70 <vTaskEnterCritical>
60311f70:	e3083368 	movw	r3, #33640	; 0x8368
60311f74:	e3463031 	movt	r3, #24625	; 0x6031
60311f78:	e306cab0 	movw	ip, #27312	; 0x6ab0
60311f7c:	e346c031 	movt	ip, #24625	; 0x6031
60311f80:	e5932000 	ldr	r2, [r3]
60311f84:	e58d2008 	str	r2, [sp, #8]
60311f88:	e3063a60 	movw	r3, #27232	; 0x6a60
60311f8c:	e3463031 	movt	r3, #24625	; 0x6031
60311f90:	e3051394 	movw	r1, #21396	; 0x5394
60311f94:	e3461031 	movt	r1, #24625	; 0x6031
60311f98:	e3a02045 	mov	r2, #69	; 0x45
60311f9c:	e3a00002 	mov	r0, #2
60311fa0:	e58d500c 	str	r5, [sp, #12]
60311fa4:	e58d4004 	str	r4, [sp, #4]
60311fa8:	e58dc000 	str	ip, [sp]
60311fac:	ebffeb51 	bl	6030ccf8 <rtk_log_write_nano>
60311fb0:	eafffffe 	b	60311fb0 <vApplicationMallocFailedHook+0x70>

60311fb4 <pvPortMallocBase>:
60311fb4:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
60311fb8:	e3087360 	movw	r7, #33632	; 0x8360
60311fbc:	e3467031 	movt	r7, #24625	; 0x6031
60311fc0:	e1a05000 	mov	r5, r0
60311fc4:	e1a06001 	mov	r6, r1
60311fc8:	e5973000 	ldr	r3, [r7]
60311fcc:	e3530000 	cmp	r3, #0
60311fd0:	0a000057 	beq	60312134 <pvPortMallocBase+0x180>
60311fd4:	e3088364 	movw	r8, #33636	; 0x8364
60311fd8:	e3468031 	movt	r8, #24625	; 0x6031
60311fdc:	ebfff2e3 	bl	6030eb70 <vTaskEnterCritical>
60311fe0:	e5983000 	ldr	r3, [r8]
60311fe4:	e1150003 	tst	r5, r3
60311fe8:	1a00004e 	bne	60312128 <pvPortMallocBase+0x174>
60311fec:	e2453001 	sub	r3, r5, #1
60311ff0:	e3730042 	cmn	r3, #66	; 0x42
60311ff4:	8a00004a 	bhi	60312124 <pvPortMallocBase+0x170>
60311ff8:	e2855040 	add	r5, r5, #64	; 0x40
60311ffc:	e315003f 	tst	r5, #63	; 0x3f
60312000:	1a000042 	bne	60312110 <pvPortMallocBase+0x15c>
60312004:	e308a368 	movw	sl, #33640	; 0x8368
60312008:	e346a031 	movt	sl, #24625	; 0x6031
6031200c:	e59a3000 	ldr	r3, [sl]
60312010:	e1530005 	cmp	r3, r5
60312014:	3a000043 	bcc	60312128 <pvPortMallocBase+0x174>
60312018:	e3083378 	movw	r3, #33656	; 0x8378
6031201c:	e3463031 	movt	r3, #24625	; 0x6031
60312020:	e1a09003 	mov	r9, r3
60312024:	e5934000 	ldr	r4, [r3]
60312028:	ea000001 	b	60312034 <pvPortMallocBase+0x80>
6031202c:	e1a09004 	mov	r9, r4
60312030:	e1a04003 	mov	r4, r3
60312034:	e5943004 	ldr	r3, [r4, #4]
60312038:	e1530005 	cmp	r3, r5
6031203c:	3a000002 	bcc	6031204c <pvPortMallocBase+0x98>
60312040:	e2843040 	add	r3, r4, #64	; 0x40
60312044:	e1530006 	cmp	r3, r6
60312048:	2a000002 	bcs	60312058 <pvPortMallocBase+0xa4>
6031204c:	e5943000 	ldr	r3, [r4]
60312050:	e3530000 	cmp	r3, #0
60312054:	1afffff4 	bne	6031202c <pvPortMallocBase+0x78>
60312058:	e5973000 	ldr	r3, [r7]
6031205c:	e1530004 	cmp	r3, r4
60312060:	0a000030 	beq	60312128 <pvPortMallocBase+0x174>
60312064:	e5943000 	ldr	r3, [r4]
60312068:	e5996000 	ldr	r6, [r9]
6031206c:	e5893000 	str	r3, [r9]
60312070:	e2866040 	add	r6, r6, #64	; 0x40
60312074:	e5943004 	ldr	r3, [r4, #4]
60312078:	e1530005 	cmp	r3, r5
6031207c:	3a000036 	bcc	6031215c <pvPortMallocBase+0x1a8>
60312080:	e0432005 	sub	r2, r3, r5
60312084:	e3520080 	cmp	r2, #128	; 0x80
60312088:	9a000008 	bls	603120b0 <pvPortMallocBase+0xfc>
6031208c:	e0847005 	add	r7, r4, r5
60312090:	e317003f 	tst	r7, #63	; 0x3f
60312094:	1a000036 	bne	60312174 <pvPortMallocBase+0x1c0>
60312098:	e5872004 	str	r2, [r7, #4]
6031209c:	e5845004 	str	r5, [r4, #4]
603120a0:	e5993000 	ldr	r3, [r9]
603120a4:	e5873000 	str	r3, [r7]
603120a8:	e5897000 	str	r7, [r9]
603120ac:	e5943004 	ldr	r3, [r4, #4]
603120b0:	e308136c 	movw	r1, #33644	; 0x836c
603120b4:	e3461031 	movt	r1, #24625	; 0x6031
603120b8:	e59a2000 	ldr	r2, [sl]
603120bc:	e5910000 	ldr	r0, [r1]
603120c0:	e0422003 	sub	r2, r2, r3
603120c4:	e1520000 	cmp	r2, r0
603120c8:	e58a2000 	str	r2, [sl]
603120cc:	35812000 	strcc	r2, [r1]
603120d0:	e5982000 	ldr	r2, [r8]
603120d4:	e3a01000 	mov	r1, #0
603120d8:	e1823003 	orr	r3, r2, r3
603120dc:	e3082370 	movw	r2, #33648	; 0x8370
603120e0:	e3462031 	movt	r2, #24625	; 0x6031
603120e4:	e884000a 	stm	r4, {r1, r3}
603120e8:	e5923000 	ldr	r3, [r2]
603120ec:	e2833001 	add	r3, r3, #1
603120f0:	e5823000 	str	r3, [r2]
603120f4:	ebfff2b8 	bl	6030ebdc <vTaskExitCritical>
603120f8:	e3560000 	cmp	r6, #0
603120fc:	0a00000a 	beq	6031212c <pvPortMallocBase+0x178>
60312100:	e316003f 	tst	r6, #63	; 0x3f
60312104:	1a00000f 	bne	60312148 <pvPortMallocBase+0x194>
60312108:	e1a00006 	mov	r0, r6
6031210c:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
60312110:	e3c5303f 	bic	r3, r5, #63	; 0x3f
60312114:	e2833040 	add	r3, r3, #64	; 0x40
60312118:	e1530005 	cmp	r3, r5
6031211c:	81a05003 	movhi	r5, r3
60312120:	8affffb7 	bhi	60312004 <pvPortMallocBase+0x50>
60312124:	e3a05000 	mov	r5, #0
60312128:	ebfff2ab 	bl	6030ebdc <vTaskExitCritical>
6031212c:	e1a00005 	mov	r0, r5
60312130:	ebffff82 	bl	60311f40 <vApplicationMallocFailedHook>
60312134:	e30609d0 	movw	r0, #27088	; 0x69d0
60312138:	e3460031 	movt	r0, #24625	; 0x6031
6031213c:	e300114e 	movw	r1, #334	; 0x14e
60312140:	ebffdeb9 	bl	60309c2c <vAssertCalled>
60312144:	eaffffa2 	b	60311fd4 <pvPortMallocBase+0x20>
60312148:	e30011cd 	movw	r1, #461	; 0x1cd
6031214c:	e30609d0 	movw	r0, #27088	; 0x69d0
60312150:	e3460031 	movt	r0, #24625	; 0x6031
60312154:	ebffdeb4 	bl	60309c2c <vAssertCalled>
60312158:	eaffffea 	b	60312108 <pvPortMallocBase+0x154>
6031215c:	e300118e 	movw	r1, #398	; 0x18e
60312160:	e30609d0 	movw	r0, #27088	; 0x69d0
60312164:	e3460031 	movt	r0, #24625	; 0x6031
60312168:	ebffdeaf 	bl	60309c2c <vAssertCalled>
6031216c:	e5943004 	ldr	r3, [r4, #4]
60312170:	eaffffc2 	b	60312080 <pvPortMallocBase+0xcc>
60312174:	e3001196 	movw	r1, #406	; 0x196
60312178:	e30609d0 	movw	r0, #27088	; 0x69d0
6031217c:	e3460031 	movt	r0, #24625	; 0x6031
60312180:	ebffdea9 	bl	60309c2c <vAssertCalled>
60312184:	e5942004 	ldr	r2, [r4, #4]
60312188:	e0422005 	sub	r2, r2, r5
6031218c:	eaffffc1 	b	60312098 <pvPortMallocBase+0xe4>

60312190 <pvPortMalloc>:
60312190:	e3a01000 	mov	r1, #0
60312194:	eaffff86 	b	60311fb4 <pvPortMallocBase>

60312198 <pvPortReAlloc>:
60312198:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
6031219c:	e1a04001 	mov	r4, r1
603121a0:	e2505000 	subs	r5, r0, #0
603121a4:	0a00001f 	beq	60312228 <pvPortReAlloc+0x90>
603121a8:	e3510000 	cmp	r1, #0
603121ac:	0a000026 	beq	6031224c <pvPortReAlloc+0xb4>
603121b0:	e3a01000 	mov	r1, #0
603121b4:	e1a00004 	mov	r0, r4
603121b8:	ebffff7d 	bl	60311fb4 <pvPortMallocBase>
603121bc:	e2506000 	subs	r6, r0, #0
603121c0:	0a000016 	beq	60312220 <pvPortReAlloc+0x88>
603121c4:	e3087364 	movw	r7, #33636	; 0x8364
603121c8:	e3467031 	movt	r7, #24625	; 0x6031
603121cc:	e515203c 	ldr	r2, [r5, #-60]	; 0xffffffc4
603121d0:	e1a01005 	mov	r1, r5
603121d4:	e5973000 	ldr	r3, [r7]
603121d8:	e1c22003 	bic	r2, r2, r3
603121dc:	e2422040 	sub	r2, r2, #64	; 0x40
603121e0:	e1520004 	cmp	r2, r4
603121e4:	21a02004 	movcs	r2, r4
603121e8:	eb000b01 	bl	60314df4 <__wrap_memcpy>
603121ec:	ebfff25f 	bl	6030eb70 <vTaskEnterCritical>
603121f0:	e3083368 	movw	r3, #33640	; 0x8368
603121f4:	e3463031 	movt	r3, #24625	; 0x6031
603121f8:	e515203c 	ldr	r2, [r5, #-60]	; 0xffffffc4
603121fc:	e5971000 	ldr	r1, [r7]
60312200:	e2450040 	sub	r0, r5, #64	; 0x40
60312204:	e1c22001 	bic	r2, r2, r1
60312208:	e5931000 	ldr	r1, [r3]
6031220c:	e505203c 	str	r2, [r5, #-60]	; 0xffffffc4
60312210:	e0812002 	add	r2, r1, r2
60312214:	e5832000 	str	r2, [r3]
60312218:	ebfffe89 	bl	60311c44 <prvInsertBlockIntoFreeList>
6031221c:	ebfff26e 	bl	6030ebdc <vTaskExitCritical>
60312220:	e1a00006 	mov	r0, r6
60312224:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60312228:	e3510000 	cmp	r1, #0
6031222c:	0a000003 	beq	60312240 <pvPortReAlloc+0xa8>
60312230:	e1a01005 	mov	r1, r5
60312234:	e1a00004 	mov	r0, r4
60312238:	e8bd41f0 	pop	{r4, r5, r6, r7, r8, lr}
6031223c:	eaffff5c 	b	60311fb4 <pvPortMallocBase>
60312240:	e1a06001 	mov	r6, r1
60312244:	e1a00006 	mov	r0, r6
60312248:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
6031224c:	e1a06001 	mov	r6, r1
60312250:	ebfffe9c 	bl	60311cc8 <vPortFree>
60312254:	e1a00006 	mov	r0, r6
60312258:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}

6031225c <pxPortInitialiseStack>:
6031225c:	e92d4010 	push	{r4, lr}
60312260:	e1a04000 	mov	r4, r0
60312264:	e1a03001 	mov	r3, r1
60312268:	e3130001 	tst	r3, #1
6031226c:	e3a01000 	mov	r1, #0
60312270:	e1a00002 	mov	r0, r2
60312274:	e5043010 	str	r3, [r4, #-16]
60312278:	03a0201f 	moveq	r2, #31
6031227c:	13a0203f 	movne	r2, #63	; 0x3f
60312280:	e5040048 	str	r0, [r4, #-72]	; 0xffffffb8
60312284:	e504200c 	str	r2, [r4, #-12]
60312288:	e2440f53 	sub	r0, r4, #332	; 0x14c
6031228c:	e3a02f41 	mov	r2, #260	; 0x104
60312290:	e5841000 	str	r1, [r4]
60312294:	e5041004 	str	r1, [r4, #-4]
60312298:	e5041008 	str	r1, [r4, #-8]
6031229c:	e5041014 	str	r1, [r4, #-20]	; 0xffffffec
603122a0:	e3013212 	movw	r3, #4626	; 0x1212
603122a4:	e3413212 	movt	r3, #4626	; 0x1212
603122a8:	e5043018 	str	r3, [r4, #-24]	; 0xffffffe8
603122ac:	e3013111 	movw	r3, #4369	; 0x1111
603122b0:	e3413111 	movt	r3, #4369	; 0x1111
603122b4:	e504301c 	str	r3, [r4, #-28]	; 0xffffffe4
603122b8:	e3013010 	movw	r3, #4112	; 0x1010
603122bc:	e3413010 	movt	r3, #4112	; 0x1010
603122c0:	e5043020 	str	r3, [r4, #-32]	; 0xffffffe0
603122c4:	e3003909 	movw	r3, #2313	; 0x909
603122c8:	e3403909 	movt	r3, #2313	; 0x909
603122cc:	e5043024 	str	r3, [r4, #-36]	; 0xffffffdc
603122d0:	e3003808 	movw	r3, #2056	; 0x808
603122d4:	e3403808 	movt	r3, #2056	; 0x808
603122d8:	e5043028 	str	r3, [r4, #-40]	; 0xffffffd8
603122dc:	e3003707 	movw	r3, #1799	; 0x707
603122e0:	e3403707 	movt	r3, #1799	; 0x707
603122e4:	e504302c 	str	r3, [r4, #-44]	; 0xffffffd4
603122e8:	e3003606 	movw	r3, #1542	; 0x606
603122ec:	e3403606 	movt	r3, #1542	; 0x606
603122f0:	e5043030 	str	r3, [r4, #-48]	; 0xffffffd0
603122f4:	e3003505 	movw	r3, #1285	; 0x505
603122f8:	e3403505 	movt	r3, #1285	; 0x505
603122fc:	e5043034 	str	r3, [r4, #-52]	; 0xffffffcc
60312300:	e3003404 	movw	r3, #1028	; 0x404
60312304:	e3403404 	movt	r3, #1028	; 0x404
60312308:	e5043038 	str	r3, [r4, #-56]	; 0xffffffc8
6031230c:	e3003303 	movw	r3, #771	; 0x303
60312310:	e3403303 	movt	r3, #771	; 0x303
60312314:	e504303c 	str	r3, [r4, #-60]	; 0xffffffc4
60312318:	e3003202 	movw	r3, #514	; 0x202
6031231c:	e3403202 	movt	r3, #514	; 0x202
60312320:	e5043040 	str	r3, [r4, #-64]	; 0xffffffc0
60312324:	e3003101 	movw	r3, #257	; 0x101
60312328:	e3403101 	movt	r3, #257	; 0x101
6031232c:	e5043044 	str	r3, [r4, #-68]	; 0xffffffbc
60312330:	eb000aac 	bl	60314de8 <__wrap_memset>
60312334:	e1a00004 	mov	r0, r4
60312338:	e308340c 	movw	r3, #33804	; 0x840c
6031233c:	e3463031 	movt	r3, #24625	; 0x6031
60312340:	e3a02001 	mov	r2, #1
60312344:	e5202150 	str	r2, [r0, #-336]!	; 0xfffffeb0
60312348:	e5832000 	str	r2, [r3]
6031234c:	e8bd8010 	pop	{r4, pc}

60312350 <xPortStartScheduler>:
60312350:	e92d4010 	push	{r4, lr}
60312354:	e10f3000 	mrs	r3, CPSR
60312358:	e203301f 	and	r3, r3, #31
6031235c:	e3530010 	cmp	r3, #16
60312360:	0a000007 	beq	60312384 <xPortStartScheduler+0x34>
60312364:	f10c0080 	cpsid	i
60312368:	f57ff04f 	dsb	sy
6031236c:	f57ff06f 	isb	sy
60312370:	ebffe1f8 	bl	6030ab58 <vConfigureIPIInterrupt>
60312374:	ebffe222 	bl	6030ac04 <vConfigureTickInterrupt>
60312378:	ebffb8a3 	bl	6030060c <vPortRestoreTaskContext>
6031237c:	e3a00000 	mov	r0, #0
60312380:	e8bd8010 	pop	{r4, pc}
60312384:	e300112b 	movw	r1, #299	; 0x12b
60312388:	e3060ab8 	movw	r0, #27320	; 0x6ab8
6031238c:	e3460031 	movt	r0, #24625	; 0x6031
60312390:	ebffde25 	bl	60309c2c <vAssertCalled>
60312394:	e3a00000 	mov	r0, #0
60312398:	e8bd8010 	pop	{r4, pc}

6031239c <xPortSpinLockTask>:
6031239c:	e3a02001 	mov	r2, #1
603123a0:	e30833c0 	movw	r3, #33728	; 0x83c0
603123a4:	e3463031 	movt	r3, #24625	; 0x6031
603123a8:	e1931f9f 	ldrex	r1, [r3]
603123ac:	e3510000 	cmp	r1, #0
603123b0:	1320f002 	wfene
603123b4:	01831f92 	strexeq	r1, r2, [r3]
603123b8:	03510000 	cmpeq	r1, #0
603123bc:	1afffff9 	bne	603123a8 <xPortSpinLockTask+0xc>
603123c0:	f57ff05f 	dmb	sy
603123c4:	e12fff1e 	bx	lr

603123c8 <xPortSpinUnLockTask>:
603123c8:	e3a02000 	mov	r2, #0
603123cc:	e30833c0 	movw	r3, #33728	; 0x83c0
603123d0:	e3463031 	movt	r3, #24625	; 0x6031
603123d4:	f57ff05f 	dmb	sy
603123d8:	e5832000 	str	r2, [r3]
603123dc:	f57ff04f 	dsb	sy
603123e0:	e320f004 	sev
603123e4:	e12fff1e 	bx	lr

603123e8 <FreeRTOS_Tick_Handler>:
603123e8:	e92d4010 	push	{r4, lr}
603123ec:	ee103fb0 	mrc	15, 0, r3, cr0, cr0, {5}
603123f0:	e6ef3073 	uxtb	r3, r3
603123f4:	e3530000 	cmp	r3, #0
603123f8:	13a04001 	movne	r4, #1
603123fc:	03a04000 	moveq	r4, #0
60312400:	1a000005 	bne	6031241c <FreeRTOS_Tick_Handler+0x34>
60312404:	ebfff462 	bl	6030f594 <xTaskIncrementTick>
60312408:	e3083410 	movw	r3, #33808	; 0x8410
6031240c:	e3463031 	movt	r3, #24625	; 0x6031
60312410:	e7830104 	str	r0, [r3, r4, lsl #2]
60312414:	e8bd4010 	pop	{r4, lr}
60312418:	eaffe214 	b	6030ac70 <vClearTickInterrupt>
6031241c:	ebffef50 	bl	6030e164 <xTaskGetCurrentYieldPending>
60312420:	e3083410 	movw	r3, #33808	; 0x8410
60312424:	e3463031 	movt	r3, #24625	; 0x6031
60312428:	e7830104 	str	r0, [r3, r4, lsl #2]
6031242c:	e8bd4010 	pop	{r4, lr}
60312430:	eaffe20e 	b	6030ac70 <vClearTickInterrupt>

60312434 <ulPortInterruptLock>:
60312434:	e10f0000 	mrs	r0, CPSR
60312438:	f10c0080 	cpsid	i
6031243c:	f57ff04f 	dsb	sy
60312440:	f57ff06f 	isb	sy
60312444:	e12fff1e 	bx	lr

60312448 <ulPortInterruptUnLock>:
60312448:	e121f000 	msr	CPSR_c, r0
6031244c:	e12fff1e 	bx	lr

60312450 <xPortCpuIsInInterrupt>:
60312450:	e10f3000 	mrs	r3, CPSR
60312454:	e203301f 	and	r3, r3, #31
60312458:	e3530010 	cmp	r3, #16
6031245c:	0a000004 	beq	60312474 <xPortCpuIsInInterrupt+0x24>
60312460:	e10f0000 	mrs	r0, CPSR
60312464:	e200001f 	and	r0, r0, #31
60312468:	e250001f 	subs	r0, r0, #31
6031246c:	13a00001 	movne	r0, #1
60312470:	e12fff1e 	bx	lr
60312474:	e3a00000 	mov	r0, #0
60312478:	e12fff1e 	bx	lr

6031247c <FreeRTOS_IPI_Handler>:
6031247c:	ee103fb0 	mrc	15, 0, r3, cr0, cr0, {5}
60312480:	e6ef3073 	uxtb	r3, r3
60312484:	e2533000 	subs	r3, r3, #0
60312488:	13a03001 	movne	r3, #1
6031248c:	e3082410 	movw	r2, #33808	; 0x8410
60312490:	e3462031 	movt	r2, #24625	; 0x6031
60312494:	e3a01001 	mov	r1, #1
60312498:	e7821103 	str	r1, [r2, r3, lsl #2]
6031249c:	e12fff1e 	bx	lr

603124a0 <ulPortGetCoreId>:
603124a0:	ee100fb0 	mrc	15, 0, r0, cr0, cr0, {5}
603124a4:	e6ef0070 	uxtb	r0, r0
603124a8:	e2500000 	subs	r0, r0, #0
603124ac:	13a00001 	movne	r0, #1
603124b0:	e12fff1e 	bx	lr

603124b4 <FreeRTOS_IPI_CPUHP_Handler>:
603124b4:	ee103fb0 	mrc	15, 0, r3, cr0, cr0, {5}
603124b8:	e6ef3073 	uxtb	r3, r3
603124bc:	e2533000 	subs	r3, r3, #0
603124c0:	13a03001 	movne	r3, #1
603124c4:	e3082410 	movw	r2, #33808	; 0x8410
603124c8:	e3462031 	movt	r2, #24625	; 0x6031
603124cc:	e3a01001 	mov	r1, #1
603124d0:	e7821103 	str	r1, [r2, r3, lsl #2]
603124d4:	e12fff1e 	bx	lr

603124d8 <vPortYieldOtherCore>:
603124d8:	eaffe1c1 	b	6030abe4 <vConfigureSMPSendIPI>

603124dc <vApplicationStackOverflowHook>:
603124dc:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
603124e0:	e3a02045 	mov	r2, #69	; 0x45
603124e4:	e24dd00c 	sub	sp, sp, #12
603124e8:	e3a00002 	mov	r0, #2
603124ec:	e3063b44 	movw	r3, #27460	; 0x6b44
603124f0:	e3463031 	movt	r3, #24625	; 0x6031
603124f4:	e58d1000 	str	r1, [sp]
603124f8:	e3051394 	movw	r1, #21396	; 0x5394
603124fc:	e3461031 	movt	r1, #24625	; 0x6031
60312500:	ebffe9fc 	bl	6030ccf8 <rtk_log_write_nano>
60312504:	e10f3000 	mrs	r3, CPSR
60312508:	f10c0080 	cpsid	i
6031250c:	f57ff04f 	dsb	sy
60312510:	f57ff06f 	isb	sy
60312514:	eafffffe 	b	60312514 <vApplicationStackOverflowHook+0x38>

60312518 <vApplicationGetIdleTaskMemory>:
60312518:	e3a0cb01 	mov	ip, #1024	; 0x400
6031251c:	e30c3418 	movw	r3, #50200	; 0xc418
60312520:	e3463031 	movt	r3, #24625	; 0x6031
60312524:	e5803000 	str	r3, [r0]
60312528:	e3083418 	movw	r3, #33816	; 0x8418
6031252c:	e3463031 	movt	r3, #24625	; 0x6031
60312530:	e5813000 	str	r3, [r1]
60312534:	e582c000 	str	ip, [r2]
60312538:	e12fff1e 	bx	lr

6031253c <vApplicationGetPassiveIdleTaskMemory>:
6031253c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
60312540:	e30cc590 	movw	ip, #50576	; 0xc590
60312544:	e346c031 	movt	ip, #24625	; 0x6031
60312548:	e3a0ef5e 	mov	lr, #376	; 0x178
6031254c:	e02cc39e 	mla	ip, lr, r3, ip
60312550:	e580c000 	str	ip, [r0]
60312554:	e3090418 	movw	r0, #37912	; 0x9418
60312558:	e3460031 	movt	r0, #24625	; 0x6031
6031255c:	e0800603 	add	r0, r0, r3, lsl #12
60312560:	e3a03b01 	mov	r3, #1024	; 0x400
60312564:	e5810000 	str	r0, [r1]
60312568:	e1c230b0 	strh	r3, [r2]
6031256c:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

60312570 <vApplicationGetTimerTaskMemory>:
60312570:	e3a0cb02 	mov	ip, #2048	; 0x800
60312574:	e30c3708 	movw	r3, #50952	; 0xc708
60312578:	e3463031 	movt	r3, #24625	; 0x6031
6031257c:	e5803000 	str	r3, [r0]
60312580:	e30a3418 	movw	r3, #42008	; 0xa418
60312584:	e3463031 	movt	r3, #24625	; 0x6031
60312588:	e5813000 	str	r3, [r1]
6031258c:	e582c000 	str	ip, [r2]
60312590:	e12fff1e 	bx	lr

60312594 <pmu_post_sleep_processing>:
60312594:	e92d4010 	push	{r4, lr}
60312598:	e3043510 	movw	r3, #17680	; 0x4510
6031259c:	e3463031 	movt	r3, #24625	; 0x6031
603125a0:	e24dd008 	sub	sp, sp, #8
603125a4:	e5900000 	ldr	r0, [r0]
603125a8:	e3a04000 	mov	r4, #0
603125ac:	e58d4004 	str	r4, [sp, #4]
603125b0:	e12fff33 	blx	r3
603125b4:	e3082380 	movw	r2, #33664	; 0x8380
603125b8:	e3462031 	movt	r2, #24625	; 0x6031
603125bc:	e3a03ffa 	mov	r3, #1000	; 0x3e8
603125c0:	e5921000 	ldr	r1, [r2]
603125c4:	e0800001 	add	r0, r0, r1
603125c8:	e200101f 	and	r1, r0, #31
603125cc:	e5821000 	str	r1, [r2]
603125d0:	e0830390 	umull	r0, r3, r0, r3
603125d4:	e1a007a0 	lsr	r0, r0, #15
603125d8:	e1800883 	orr	r0, r0, r3, lsl #17
603125dc:	e58d0004 	str	r0, [sp, #4]
603125e0:	e59d0004 	ldr	r0, [sp, #4]
603125e4:	ebffef10 	bl	6030e22c <vTaskCompTick>
603125e8:	e3073fe8 	movw	r3, #32744	; 0x7fe8
603125ec:	e3463031 	movt	r3, #24625	; 0x6031
603125f0:	e1a00004 	mov	r0, r4
603125f4:	e5834000 	str	r4, [r3]
603125f8:	e28dd008 	add	sp, sp, #8
603125fc:	e8bd4010 	pop	{r4, lr}
60312600:	eaffec45 	b	6030d71c <pmu_set_sysactive_time>

60312604 <vPortSuppressTicksAndSleep>:
60312604:	e92d43f0 	push	{r4, r5, r6, r7, r8, r9, lr}
60312608:	e24dd00c 	sub	sp, sp, #12
6031260c:	ee103fb0 	mrc	15, 0, r3, cr0, cr0, {5}
60312610:	e21340ff 	ands	r4, r3, #255	; 0xff
60312614:	0a000014 	beq	6031266c <vPortSuppressTicksAndSleep+0x68>
60312618:	ee103fb0 	mrc	15, 0, r3, cr0, cr0, {5}
6031261c:	e6ef3073 	uxtb	r3, r3
60312620:	e3530000 	cmp	r3, #0
60312624:	1a000001 	bne	60312630 <vPortSuppressTicksAndSleep+0x2c>
60312628:	e28dd00c 	add	sp, sp, #12
6031262c:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
60312630:	ebffec73 	bl	6030d804 <pmu_ready_to_sleep>
60312634:	e3500000 	cmp	r0, #0
60312638:	1a000037 	bne	6031271c <vPortSuppressTicksAndSleep+0x118>
6031263c:	e10f3000 	mrs	r3, CPSR
60312640:	f10c0080 	cpsid	i
60312644:	f57ff04f 	dsb	sy
60312648:	f57ff06f 	isb	sy
6031264c:	f57ff04f 	dsb	sy
60312650:	e320f003 	wfi
60312654:	f57ff06f 	isb	sy
60312658:	f1080080 	cpsie	i
6031265c:	f57ff04f 	dsb	sy
60312660:	f57ff06f 	isb	sy
60312664:	e28dd00c 	add	sp, sp, #12
60312668:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
6031266c:	e1a05000 	mov	r5, r0
60312670:	e10f3000 	mrs	r3, CPSR
60312674:	f10c0080 	cpsid	i
60312678:	f57ff04f 	dsb	sy
6031267c:	f57ff06f 	isb	sy
60312680:	ee1e3f33 	mrc	15, 0, r3, cr14, cr3, {1}
60312684:	e3833002 	orr	r3, r3, #2
60312688:	ee0e3f33 	mcr	15, 0, r3, cr14, cr3, {1}
6031268c:	e30762a8 	movw	r6, #29352	; 0x72a8
60312690:	e3466031 	movt	r6, #24625	; 0x6031
60312694:	e3a08001 	mov	r8, #1
60312698:	e30873c0 	movw	r7, #33728	; 0x83c0
6031269c:	e3467031 	movt	r7, #24625	; 0x6031
603126a0:	e5864000 	str	r4, [r6]
603126a4:	e1973f9f 	ldrex	r3, [r7]
603126a8:	e3530000 	cmp	r3, #0
603126ac:	1320f002 	wfene
603126b0:	01873f98 	strexeq	r3, r8, [r7]
603126b4:	03530000 	cmpeq	r3, #0
603126b8:	1afffff9 	bne	603126a4 <vPortSuppressTicksAndSleep+0xa0>
603126bc:	f57ff05f 	dmb	sy
603126c0:	ebfff0de 	bl	6030ea40 <eTaskConfirmSleepModeStatus>
603126c4:	f57ff05f 	dmb	sy
603126c8:	e5874000 	str	r4, [r7]
603126cc:	f57ff04f 	dsb	sy
603126d0:	e320f004 	sev
603126d4:	e3500000 	cmp	r0, #0
603126d8:	1a00001e 	bne	60312758 <vPortSuppressTicksAndSleep+0x154>
603126dc:	ee1e3f33 	mrc	15, 0, r3, cr14, cr3, {1}
603126e0:	e3c33002 	bic	r3, r3, #2
603126e4:	ee0e3f33 	mcr	15, 0, r3, cr14, cr3, {1}
603126e8:	e3a03c02 	mov	r3, #512	; 0x200
603126ec:	e3443100 	movt	r3, #16640	; 0x4100
603126f0:	e5933008 	ldr	r3, [r3, #8]
603126f4:	e3130802 	tst	r3, #131072	; 0x20000
603126f8:	0a000000 	beq	60312700 <vPortSuppressTicksAndSleep+0xfc>
603126fc:	e320f004 	sev
60312700:	e3a03001 	mov	r3, #1
60312704:	e5863000 	str	r3, [r6]
60312708:	f1080080 	cpsie	i
6031270c:	f57ff04f 	dsb	sy
60312710:	f57ff06f 	isb	sy
60312714:	e28dd00c 	add	sp, sp, #12
60312718:	e8bd83f0 	pop	{r4, r5, r6, r7, r8, r9, pc}
6031271c:	ebffec6f 	bl	6030d8e0 <pmu_get_sleep_type>
60312720:	e3500000 	cmp	r0, #0
60312724:	0a000015 	beq	60312780 <vPortSuppressTicksAndSleep+0x17c>
60312728:	e10f3000 	mrs	r3, CPSR
6031272c:	f10c0080 	cpsid	i
60312730:	f57ff04f 	dsb	sy
60312734:	f57ff06f 	isb	sy
60312738:	f57ff04f 	dsb	sy
6031273c:	e320f002 	wfe
60312740:	e320f002 	wfe
60312744:	f57ff06f 	isb	sy
60312748:	f1080080 	cpsie	i
6031274c:	f57ff04f 	dsb	sy
60312750:	f57ff06f 	isb	sy
60312754:	eaffffb3 	b	60312628 <vPortSuppressTicksAndSleep+0x24>
60312758:	ebffec29 	bl	6030d804 <pmu_ready_to_sleep>
6031275c:	e3500000 	cmp	r0, #0
60312760:	1a00000b 	bne	60312794 <vPortSuppressTicksAndSleep+0x190>
60312764:	ee1e3f33 	mrc	15, 0, r3, cr14, cr3, {1}
60312768:	e3c33002 	bic	r3, r3, #2
6031276c:	ee0e3f33 	mcr	15, 0, r3, cr14, cr3, {1}
60312770:	f57ff04f 	dsb	sy
60312774:	e320f003 	wfi
60312778:	f57ff06f 	isb	sy
6031277c:	eaffffd9 	b	603126e8 <vPortSuppressTicksAndSleep+0xe4>
60312780:	e3a01001 	mov	r1, #1
60312784:	e1a00001 	mov	r0, r1
60312788:	ebffecbe 	bl	6030da88 <pmu_set_secondary_cpu_state>
6031278c:	ef000000 	svc	0x00000000
60312790:	eaffffa4 	b	60312628 <vPortSuppressTicksAndSleep+0x24>
60312794:	e58d5004 	str	r5, [sp, #4]
60312798:	ebffec50 	bl	6030d8e0 <pmu_get_sleep_type>
6031279c:	e3500000 	cmp	r0, #0
603127a0:	0a00001e 	beq	60312820 <vPortSuppressTicksAndSleep+0x21c>
603127a4:	e3a03c02 	mov	r3, #512	; 0x200
603127a8:	e3443100 	movt	r3, #16640	; 0x4100
603127ac:	e5933008 	ldr	r3, [r3, #8]
603127b0:	e3130802 	tst	r3, #131072	; 0x20000
603127b4:	0affffcb 	beq	603126e8 <vPortSuppressTicksAndSleep+0xe4>
603127b8:	e28d0004 	add	r0, sp, #4
603127bc:	ebffec6c 	bl	6030d974 <pmu_pre_sleep_processing>
603127c0:	ebffec46 	bl	6030d8e0 <pmu_get_sleep_type>
603127c4:	e3500000 	cmp	r0, #0
603127c8:	1a00000e 	bne	60312808 <vPortSuppressTicksAndSleep+0x204>
603127cc:	ee1e3f33 	mrc	15, 0, r3, cr14, cr3, {1}
603127d0:	e3130001 	tst	r3, #1
603127d4:	1a000001 	bne	603127e0 <vPortSuppressTicksAndSleep+0x1dc>
603127d8:	e3a03001 	mov	r3, #1
603127dc:	ee0e3f33 	mcr	15, 0, r3, cr14, cr3, {1}
603127e0:	ec598f1e 	mrrc	15, 1, r8, r9, cr14
603127e4:	e30735d0 	movw	r3, #30160	; 0x75d0
603127e8:	e3463031 	movt	r3, #24625	; 0x6031
603127ec:	e3a02ffa 	mov	r2, #1000	; 0x3e8
603127f0:	e1c300d0 	ldrd	r0, [r3]
603127f4:	e3a03000 	mov	r3, #0
603127f8:	ebffb835 	bl	603008d4 <__aeabi_uldivmod>
603127fc:	e0902008 	adds	r2, r0, r8
60312800:	e0a13009 	adc	r3, r1, r9
60312804:	ec432f3e 	mcrr	15, 3, r2, r3, cr14
60312808:	ee1e3f33 	mrc	15, 0, r3, cr14, cr3, {1}
6031280c:	e3c33002 	bic	r3, r3, #2
60312810:	ee0e3f33 	mcr	15, 0, r3, cr14, cr3, {1}
60312814:	e28d0004 	add	r0, sp, #4
60312818:	ebffff5d 	bl	60312594 <pmu_post_sleep_processing>
6031281c:	eaffffb1 	b	603126e8 <vPortSuppressTicksAndSleep+0xe4>
60312820:	e1a00008 	mov	r0, r8
60312824:	ebffec9b 	bl	6030da98 <pmu_get_secondary_cpu_state>
60312828:	e3500002 	cmp	r0, #2
6031282c:	0affffad 	beq	603126e8 <vPortSuppressTicksAndSleep+0xe4>
60312830:	e1a00008 	mov	r0, r8
60312834:	ebffec97 	bl	6030da98 <pmu_get_secondary_cpu_state>
60312838:	e2501000 	subs	r1, r0, #0
6031283c:	1affffdd 	bne	603127b8 <vPortSuppressTicksAndSleep+0x1b4>
60312840:	f1080080 	cpsie	i
60312844:	f57ff04f 	dsb	sy
60312848:	f57ff06f 	isb	sy
6031284c:	e1a00008 	mov	r0, r8
60312850:	ebffe000 	bl	6030a858 <arm_gic_raise_softirq>
60312854:	ee1e3f33 	mrc	15, 0, r3, cr14, cr3, {1}
60312858:	e3c33002 	bic	r3, r3, #2
6031285c:	ee0e3f33 	mcr	15, 0, r3, cr14, cr3, {1}
60312860:	e3a00064 	mov	r0, #100	; 0x64
60312864:	e30c3b2c 	movw	r3, #52012	; 0xcb2c
60312868:	e3463030 	movt	r3, #24624	; 0x6030
6031286c:	e12fff33 	blx	r3
60312870:	eaffff9c 	b	603126e8 <vPortSuppressTicksAndSleep+0xe4>

60312874 <vPortCleanUpTCB>:
60312874:	e12fff1e 	bx	lr

60312878 <ChipInfo_PsramExists>:
60312878:	e3a00001 	mov	r0, #1
6031287c:	e12fff1e 	bx	lr

60312880 <print_unsigned_num>:
60312880:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60312884:	e1a06000 	mov	r6, r0
60312888:	e24dd024 	sub	sp, sp, #36	; 0x24
6031288c:	e28da00c 	add	sl, sp, #12
60312890:	e1a05001 	mov	r5, r1
60312894:	e59d704c 	ldr	r7, [sp, #76]	; 0x4c
60312898:	e1a08002 	mov	r8, r2
6031289c:	e1a0b003 	mov	fp, r3
603128a0:	e1a0900a 	mov	r9, sl
603128a4:	e3a04000 	mov	r4, #0
603128a8:	ea00000b 	b	603128dc <print_unsigned_num+0x5c>
603128ac:	e28c3057 	add	r3, ip, #87	; 0x57
603128b0:	e3570001 	cmp	r7, #1
603128b4:	e6ef3073 	uxtb	r3, r3
603128b8:	0a000012 	beq	60312908 <print_unsigned_num+0x88>
603128bc:	e1560008 	cmp	r6, r8
603128c0:	e2842001 	add	r2, r4, #1
603128c4:	e1a06000 	mov	r6, r0
603128c8:	e4c93001 	strb	r3, [r9], #1
603128cc:	e2d50000 	sbcs	r0, r5, #0
603128d0:	e1a05001 	mov	r5, r1
603128d4:	3a00000e 	bcc	60312914 <print_unsigned_num+0x94>
603128d8:	e1a04002 	mov	r4, r2
603128dc:	e1a00006 	mov	r0, r6
603128e0:	e1a01005 	mov	r1, r5
603128e4:	e1a02008 	mov	r2, r8
603128e8:	e3a03000 	mov	r3, #0
603128ec:	ebffb7f8 	bl	603008d4 <__aeabi_uldivmod>
603128f0:	e6efc072 	uxtb	ip, r2
603128f4:	e28c3030 	add	r3, ip, #48	; 0x30
603128f8:	e3520009 	cmp	r2, #9
603128fc:	8affffea 	bhi	603128ac <print_unsigned_num+0x2c>
60312900:	e6ef3073 	uxtb	r3, r3
60312904:	eaffffec 	b	603128bc <print_unsigned_num+0x3c>
60312908:	e28c3037 	add	r3, ip, #55	; 0x37
6031290c:	e6ef3073 	uxtb	r3, r3
60312910:	eaffffe9 	b	603128bc <print_unsigned_num+0x3c>
60312914:	e1a07002 	mov	r7, r2
60312918:	e59d2050 	ldr	r2, [sp, #80]	; 0x50
6031291c:	e3520000 	cmp	r2, #0
60312920:	e59d2048 	ldr	r2, [sp, #72]	; 0x48
60312924:	0a000010 	beq	6031296c <print_unsigned_num+0xec>
60312928:	e2422001 	sub	r2, r2, #1
6031292c:	e35b0030 	cmp	fp, #48	; 0x30
60312930:	e58d2048 	str	r2, [sp, #72]	; 0x48
60312934:	1a000035 	bne	60312a10 <print_unsigned_num+0x190>
60312938:	e3a0002d 	mov	r0, #45	; 0x2d
6031293c:	e3045218 	movw	r5, #16920	; 0x4218
60312940:	e3465031 	movt	r5, #24625	; 0x6031
60312944:	e58d3004 	str	r3, [sp, #4]
60312948:	e12fff35 	blx	r5
6031294c:	e59d3048 	ldr	r3, [sp, #72]	; 0x48
60312950:	e3530000 	cmp	r3, #0
60312954:	e59d3004 	ldr	r3, [sp, #4]
60312958:	da000021 	ble	603129e4 <print_unsigned_num+0x164>
6031295c:	e59d2048 	ldr	r2, [sp, #72]	; 0x48
60312960:	e1570002 	cmp	r7, r2
60312964:	ba00000a 	blt	60312994 <print_unsigned_num+0x114>
60312968:	ea00001b 	b	603129dc <print_unsigned_num+0x15c>
6031296c:	e3520000 	cmp	r2, #0
60312970:	d3045218 	movwle	r5, #16920	; 0x4218
60312974:	d3465031 	movtle	r5, #24625	; 0x6031
60312978:	da000019 	ble	603129e4 <print_unsigned_num+0x164>
6031297c:	e59d1048 	ldr	r1, [sp, #72]	; 0x48
60312980:	e3045218 	movw	r5, #16920	; 0x4218
60312984:	e3465031 	movt	r5, #24625	; 0x6031
60312988:	e59d2050 	ldr	r2, [sp, #80]	; 0x50
6031298c:	e1510007 	cmp	r1, r7
60312990:	da00000d 	ble	603129cc <print_unsigned_num+0x14c>
60312994:	e59d6048 	ldr	r6, [sp, #72]	; 0x48
60312998:	e1a08003 	mov	r8, r3
6031299c:	e1a0000b 	mov	r0, fp
603129a0:	e2466001 	sub	r6, r6, #1
603129a4:	e12fff35 	blx	r5
603129a8:	e1570006 	cmp	r7, r6
603129ac:	1afffffa 	bne	6031299c <print_unsigned_num+0x11c>
603129b0:	e59d1048 	ldr	r1, [sp, #72]	; 0x48
603129b4:	e1e02007 	mvn	r2, r7
603129b8:	e0822001 	add	r2, r2, r1
603129bc:	e59d1050 	ldr	r1, [sp, #80]	; 0x50
603129c0:	e1a03008 	mov	r3, r8
603129c4:	e2811001 	add	r1, r1, #1
603129c8:	e0822001 	add	r2, r2, r1
603129cc:	e59d1050 	ldr	r1, [sp, #80]	; 0x50
603129d0:	e58d2050 	str	r2, [sp, #80]	; 0x50
603129d4:	e3510000 	cmp	r1, #0
603129d8:	0a000001 	beq	603129e4 <print_unsigned_num+0x164>
603129dc:	e31b00df 	tst	fp, #223	; 0xdf
603129e0:	0a000010 	beq	60312a28 <print_unsigned_num+0x1a8>
603129e4:	e08a4004 	add	r4, sl, r4
603129e8:	e1a00003 	mov	r0, r3
603129ec:	ea000000 	b	603129f4 <print_unsigned_num+0x174>
603129f0:	e5740001 	ldrb	r0, [r4, #-1]!
603129f4:	e12fff35 	blx	r5
603129f8:	e15a0004 	cmp	sl, r4
603129fc:	1afffffb 	bne	603129f0 <print_unsigned_num+0x170>
60312a00:	e59d3050 	ldr	r3, [sp, #80]	; 0x50
60312a04:	e0830007 	add	r0, r3, r7
60312a08:	e28dd024 	add	sp, sp, #36	; 0x24
60312a0c:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
60312a10:	e59d2048 	ldr	r2, [sp, #72]	; 0x48
60312a14:	e3520000 	cmp	r2, #0
60312a18:	d3045218 	movwle	r5, #16920	; 0x4218
60312a1c:	d3465031 	movtle	r5, #24625	; 0x6031
60312a20:	caffffd5 	bgt	6031297c <print_unsigned_num+0xfc>
60312a24:	eaffffec 	b	603129dc <print_unsigned_num+0x15c>
60312a28:	e3a0002d 	mov	r0, #45	; 0x2d
60312a2c:	e58d3004 	str	r3, [sp, #4]
60312a30:	e12fff35 	blx	r5
60312a34:	e59d3004 	ldr	r3, [sp, #4]
60312a38:	eaffffe9 	b	603129e4 <print_unsigned_num+0x164>

60312a3c <SYSCFG_RLVersion>:
60312a3c:	e3a03902 	mov	r3, #32768	; 0x8000
60312a40:	e3443200 	movt	r3, #16896	; 0x4200
60312a44:	e5932274 	ldr	r2, [r3, #628]	; 0x274
60312a48:	e3c2220f 	bic	r2, r2, #-268435456	; 0xf0000000
60312a4c:	e382120a 	orr	r1, r2, #-1610612736	; 0xa0000000
60312a50:	e5831274 	str	r1, [r3, #628]	; 0x274
60312a54:	e5930274 	ldr	r0, [r3, #628]	; 0x274
60312a58:	e5832274 	str	r2, [r3, #628]	; 0x274
60312a5c:	e7e30850 	ubfx	r0, r0, #16, #4
60312a60:	e12fff1e 	bx	lr

60312a64 <DiagVprintf>:
60312a64:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60312a68:	e1a04000 	mov	r4, r0
60312a6c:	e5d00000 	ldrb	r0, [r0]
60312a70:	e24dd02c 	sub	sp, sp, #44	; 0x2c
60312a74:	e3500000 	cmp	r0, #0
60312a78:	0a00021d 	beq	603132f4 <DiagVprintf+0x890>
60312a7c:	e3a03000 	mov	r3, #0
60312a80:	e1a09001 	mov	r9, r1
60312a84:	e1a06003 	mov	r6, r3
60312a88:	e3a0a00a 	mov	sl, #10
60312a8c:	e58d3014 	str	r3, [sp, #20]
60312a90:	ea000006 	b	60312ab0 <DiagVprintf+0x4c>
60312a94:	e3045218 	movw	r5, #16920	; 0x4218
60312a98:	e3465031 	movt	r5, #24625	; 0x6031
60312a9c:	e2866001 	add	r6, r6, #1
60312aa0:	e12fff35 	blx	r5
60312aa4:	e5d40000 	ldrb	r0, [r4]
60312aa8:	e3500000 	cmp	r0, #0
60312aac:	0a000086 	beq	60312ccc <DiagVprintf+0x268>
60312ab0:	e2844001 	add	r4, r4, #1
60312ab4:	e3500025 	cmp	r0, #37	; 0x25
60312ab8:	1afffff5 	bne	60312a94 <DiagVprintf+0x30>
60312abc:	e3a0b000 	mov	fp, #0
60312ac0:	e1a0700b 	mov	r7, fp
60312ac4:	e1a0800b 	mov	r8, fp
60312ac8:	e1a0000b 	mov	r0, fp
60312acc:	e5d43000 	ldrb	r3, [r4]
60312ad0:	e353007a 	cmp	r3, #122	; 0x7a
60312ad4:	979ff103 	ldrls	pc, [pc, r3, lsl #2]
60312ad8:	ea00007a 	b	60312cc8 <DiagVprintf+0x264>
60312adc:	60312d1c 	.word	0x60312d1c
60312ae0:	60312cc8 	.word	0x60312cc8
60312ae4:	60312cc8 	.word	0x60312cc8
60312ae8:	60312cc8 	.word	0x60312cc8
60312aec:	60312cc8 	.word	0x60312cc8
60312af0:	60312cc8 	.word	0x60312cc8
60312af4:	60312cc8 	.word	0x60312cc8
60312af8:	60312cc8 	.word	0x60312cc8
60312afc:	60312cc8 	.word	0x60312cc8
60312b00:	60312cc8 	.word	0x60312cc8
60312b04:	60312cc8 	.word	0x60312cc8
60312b08:	60312cc8 	.word	0x60312cc8
60312b0c:	60312cc8 	.word	0x60312cc8
60312b10:	60312cc8 	.word	0x60312cc8
60312b14:	60312cc8 	.word	0x60312cc8
60312b18:	60312cc8 	.word	0x60312cc8
60312b1c:	60312cc8 	.word	0x60312cc8
60312b20:	60312cc8 	.word	0x60312cc8
60312b24:	60312cc8 	.word	0x60312cc8
60312b28:	60312cc8 	.word	0x60312cc8
60312b2c:	60312cc8 	.word	0x60312cc8
60312b30:	60312cc8 	.word	0x60312cc8
60312b34:	60312cc8 	.word	0x60312cc8
60312b38:	60312cc8 	.word	0x60312cc8
60312b3c:	60312cc8 	.word	0x60312cc8
60312b40:	60312cc8 	.word	0x60312cc8
60312b44:	60312cc8 	.word	0x60312cc8
60312b48:	60312cc8 	.word	0x60312cc8
60312b4c:	60312cc8 	.word	0x60312cc8
60312b50:	60312cc8 	.word	0x60312cc8
60312b54:	60312cc8 	.word	0x60312cc8
60312b58:	60312cc8 	.word	0x60312cc8
60312b5c:	60312d54 	.word	0x60312d54
60312b60:	60312cc8 	.word	0x60312cc8
60312b64:	60312cc8 	.word	0x60312cc8
60312b68:	60312cc8 	.word	0x60312cc8
60312b6c:	60312cc8 	.word	0x60312cc8
60312b70:	60313044 	.word	0x60313044
60312b74:	60312cc8 	.word	0x60312cc8
60312b78:	60312cc8 	.word	0x60312cc8
60312b7c:	60312cc8 	.word	0x60312cc8
60312b80:	60312cc8 	.word	0x60312cc8
60312b84:	60312cc8 	.word	0x60312cc8
60312b88:	60312cc8 	.word	0x60312cc8
60312b8c:	60312cc8 	.word	0x60312cc8
60312b90:	60313038 	.word	0x60313038
60312b94:	60312cc8 	.word	0x60312cc8
60312b98:	60312cc8 	.word	0x60312cc8
60312b9c:	60312d54 	.word	0x60312d54
60312ba0:	60312d24 	.word	0x60312d24
60312ba4:	60312d24 	.word	0x60312d24
60312ba8:	60312d24 	.word	0x60312d24
60312bac:	60312d24 	.word	0x60312d24
60312bb0:	60312d24 	.word	0x60312d24
60312bb4:	60312d24 	.word	0x60312d24
60312bb8:	60312d24 	.word	0x60312d24
60312bbc:	60312d24 	.word	0x60312d24
60312bc0:	60312d24 	.word	0x60312d24
60312bc4:	60312cc8 	.word	0x60312cc8
60312bc8:	60312cc8 	.word	0x60312cc8
60312bcc:	60312cc8 	.word	0x60312cc8
60312bd0:	60312cc8 	.word	0x60312cc8
60312bd4:	60312cc8 	.word	0x60312cc8
60312bd8:	60312cc8 	.word	0x60312cc8
60312bdc:	60312cc8 	.word	0x60312cc8
60312be0:	60312cc8 	.word	0x60312cc8
60312be4:	60312f1c 	.word	0x60312f1c
60312be8:	60312ea0 	.word	0x60312ea0
60312bec:	60312cc8 	.word	0x60312cc8
60312bf0:	60312cc8 	.word	0x60312cc8
60312bf4:	60312cc8 	.word	0x60312cc8
60312bf8:	60312cc8 	.word	0x60312cc8
60312bfc:	60312cc8 	.word	0x60312cc8
60312c00:	60312cc8 	.word	0x60312cc8
60312c04:	60312cc8 	.word	0x60312cc8
60312c08:	60312cc8 	.word	0x60312cc8
60312c0c:	60312e94 	.word	0x60312e94
60312c10:	60312cc8 	.word	0x60312cc8
60312c14:	60312cc8 	.word	0x60312cc8
60312c18:	60312e0c 	.word	0x60312e0c
60312c1c:	60312cc8 	.word	0x60312cc8
60312c20:	60312cc8 	.word	0x60312cc8
60312c24:	60312cc8 	.word	0x60312cc8
60312c28:	60312cc8 	.word	0x60312cc8
60312c2c:	60312cc8 	.word	0x60312cc8
60312c30:	60312cc8 	.word	0x60312cc8
60312c34:	60312cc8 	.word	0x60312cc8
60312c38:	60312cc8 	.word	0x60312cc8
60312c3c:	60312fa4 	.word	0x60312fa4
60312c40:	60312cc8 	.word	0x60312cc8
60312c44:	60312cc8 	.word	0x60312cc8
60312c48:	60312cc8 	.word	0x60312cc8
60312c4c:	60312cc8 	.word	0x60312cc8
60312c50:	60312cc8 	.word	0x60312cc8
60312c54:	60312cc8 	.word	0x60312cc8
60312c58:	60312cc8 	.word	0x60312cc8
60312c5c:	60312cc8 	.word	0x60312cc8
60312c60:	60312cc8 	.word	0x60312cc8
60312c64:	60312f1c 	.word	0x60312f1c
60312c68:	60312ea0 	.word	0x60312ea0
60312c6c:	60312d6c 	.word	0x60312d6c
60312c70:	60312cc8 	.word	0x60312cc8
60312c74:	60312cc8 	.word	0x60312cc8
60312c78:	60312cc8 	.word	0x60312cc8
60312c7c:	60312cc8 	.word	0x60312cc8
60312c80:	60312d6c 	.word	0x60312d6c
60312c84:	60312cc8 	.word	0x60312cc8
60312c88:	60312cc8 	.word	0x60312cc8
60312c8c:	60312e94 	.word	0x60312e94
60312c90:	60312cc8 	.word	0x60312cc8
60312c94:	60312cc8 	.word	0x60312cc8
60312c98:	60312e0c 	.word	0x60312e0c
60312c9c:	60313100 	.word	0x60313100
60312ca0:	60312cc8 	.word	0x60312cc8
60312ca4:	60312cc8 	.word	0x60312cc8
60312ca8:	60313060 	.word	0x60313060
60312cac:	60312cc8 	.word	0x60312cc8
60312cb0:	60312cd8 	.word	0x60312cd8
60312cb4:	60312cc8 	.word	0x60312cc8
60312cb8:	60312cc8 	.word	0x60312cc8
60312cbc:	60312fac 	.word	0x60312fac
60312cc0:	60312cc8 	.word	0x60312cc8
60312cc4:	60312e98 	.word	0x60312e98
60312cc8:	e3e06000 	mvn	r6, #0
60312ccc:	e1a00006 	mov	r0, r6
60312cd0:	e28dd02c 	add	sp, sp, #44	; 0x2c
60312cd4:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
60312cd8:	e3500001 	cmp	r0, #1
60312cdc:	c2899007 	addgt	r9, r9, #7
60312ce0:	c3c99007 	bicgt	r9, r9, #7
60312ce4:	d4990004 	ldrle	r0, [r9], #4
60312ce8:	d3a01000 	movle	r1, #0
60312cec:	c5991004 	ldrgt	r1, [r9, #4]
60312cf0:	c4990008 	ldrgt	r0, [r9], #8
60312cf4:	e35b0000 	cmp	fp, #0
60312cf8:	1a00012f 	bne	603131bc <DiagVprintf+0x758>
60312cfc:	e58db008 	str	fp, [sp, #8]
60312d00:	e59d2014 	ldr	r2, [sp, #20]
60312d04:	e58d2004 	str	r2, [sp, #4]
60312d08:	e1a03008 	mov	r3, r8
60312d0c:	e3a0200a 	mov	r2, #10
60312d10:	e58d7000 	str	r7, [sp]
60312d14:	ebfffed9 	bl	60312880 <print_unsigned_num>
60312d18:	e0866000 	add	r6, r6, r0
60312d1c:	e2844001 	add	r4, r4, #1
60312d20:	eaffff5f 	b	60312aa4 <DiagVprintf+0x40>
60312d24:	e3a08020 	mov	r8, #32
60312d28:	e3a07000 	mov	r7, #0
60312d2c:	ea000003 	b	60312d40 <DiagVprintf+0x2dc>
60312d30:	e5f43001 	ldrb	r3, [r4, #1]!
60312d34:	e2427030 	sub	r7, r2, #48	; 0x30
60312d38:	e3530000 	cmp	r3, #0
60312d3c:	0affffe2 	beq	60312ccc <DiagVprintf+0x268>
60312d40:	e022379a 	mla	r2, sl, r7, r3
60312d44:	e2433030 	sub	r3, r3, #48	; 0x30
60312d48:	e3530009 	cmp	r3, #9
60312d4c:	9afffff7 	bls	60312d30 <DiagVprintf+0x2cc>
60312d50:	eaffff5d 	b	60312acc <DiagVprintf+0x68>
60312d54:	e2844001 	add	r4, r4, #1
60312d58:	e1a08003 	mov	r8, r3
60312d5c:	e5d43000 	ldrb	r3, [r4]
60312d60:	e3530000 	cmp	r3, #0
60312d64:	1affffef 	bne	60312d28 <DiagVprintf+0x2c4>
60312d68:	eaffffd7 	b	60312ccc <DiagVprintf+0x268>
60312d6c:	e3500001 	cmp	r0, #1
60312d70:	c2899007 	addgt	r9, r9, #7
60312d74:	c3c99007 	bicgt	r9, r9, #7
60312d78:	d5990000 	ldrle	r0, [r9]
60312d7c:	d2892004 	addle	r2, r9, #4
60312d80:	c5991004 	ldrgt	r1, [r9, #4]
60312d84:	d1a01fc0 	asrle	r1, r0, #31
60312d88:	d1a09002 	movle	r9, r2
60312d8c:	c4990008 	ldrgt	r0, [r9], #8
60312d90:	e3510000 	cmp	r1, #0
60312d94:	a3a02000 	movge	r2, #0
60312d98:	ba000128 	blt	60313240 <DiagVprintf+0x7dc>
60312d9c:	e35b0000 	cmp	fp, #0
60312da0:	e58d2008 	str	r2, [sp, #8]
60312da4:	0affffd5 	beq	60312d00 <DiagVprintf+0x29c>
60312da8:	e59d3014 	ldr	r3, [sp, #20]
60312dac:	e58d3004 	str	r3, [sp, #4]
60312db0:	e3a0200a 	mov	r2, #10
60312db4:	e3a03000 	mov	r3, #0
60312db8:	e58d3000 	str	r3, [sp]
60312dbc:	e1a03008 	mov	r3, r8
60312dc0:	ebfffeae 	bl	60312880 <print_unsigned_num>
60312dc4:	e1570000 	cmp	r7, r0
60312dc8:	d3a03000 	movle	r3, #0
60312dcc:	c3a03001 	movgt	r3, #1
60312dd0:	e3580020 	cmp	r8, #32
60312dd4:	13a03000 	movne	r3, #0
60312dd8:	e3530000 	cmp	r3, #0
60312ddc:	0affffcd 	beq	60312d18 <DiagVprintf+0x2b4>
60312de0:	e0478000 	sub	r8, r7, r0
60312de4:	e3580000 	cmp	r8, #0
60312de8:	da00013f 	ble	603132ec <DiagVprintf+0x888>
60312dec:	e3045218 	movw	r5, #16920	; 0x4218
60312df0:	e3465031 	movt	r5, #24625	; 0x6031
60312df4:	e3a00020 	mov	r0, #32
60312df8:	e12fff35 	blx	r5
60312dfc:	e2588001 	subs	r8, r8, #1
60312e00:	1afffffb 	bne	60312df4 <DiagVprintf+0x390>
60312e04:	e1a00007 	mov	r0, r7
60312e08:	eaffffc2 	b	60312d18 <DiagVprintf+0x2b4>
60312e0c:	e3500001 	cmp	r0, #1
60312e10:	c2899007 	addgt	r9, r9, #7
60312e14:	c3c99007 	bicgt	r9, r9, #7
60312e18:	d4990004 	ldrle	r0, [r9], #4
60312e1c:	d3a01000 	movle	r1, #0
60312e20:	c5991004 	ldrgt	r1, [r9, #4]
60312e24:	c4990008 	ldrgt	r0, [r9], #8
60312e28:	e35b0000 	cmp	fp, #0
60312e2c:	0a0000cf 	beq	60313170 <DiagVprintf+0x70c>
60312e30:	e59d3014 	ldr	r3, [sp, #20]
60312e34:	e58d3004 	str	r3, [sp, #4]
60312e38:	e3a02008 	mov	r2, #8
60312e3c:	e3a03000 	mov	r3, #0
60312e40:	e58d3008 	str	r3, [sp, #8]
60312e44:	e58d3000 	str	r3, [sp]
60312e48:	e1a03008 	mov	r3, r8
60312e4c:	ebfffe8b 	bl	60312880 <print_unsigned_num>
60312e50:	e1570000 	cmp	r7, r0
60312e54:	d3a03000 	movle	r3, #0
60312e58:	c3a03001 	movgt	r3, #1
60312e5c:	e3580020 	cmp	r8, #32
60312e60:	13a03000 	movne	r3, #0
60312e64:	e3530000 	cmp	r3, #0
60312e68:	0affffaa 	beq	60312d18 <DiagVprintf+0x2b4>
60312e6c:	e0478000 	sub	r8, r7, r0
60312e70:	e3580000 	cmp	r8, #0
60312e74:	da00011c 	ble	603132ec <DiagVprintf+0x888>
60312e78:	e3045218 	movw	r5, #16920	; 0x4218
60312e7c:	e3465031 	movt	r5, #24625	; 0x6031
60312e80:	e3a00020 	mov	r0, #32
60312e84:	e12fff35 	blx	r5
60312e88:	e2588001 	subs	r8, r8, #1
60312e8c:	1afffffb 	bne	60312e80 <DiagVprintf+0x41c>
60312e90:	eaffffdb 	b	60312e04 <DiagVprintf+0x3a0>
60312e94:	e2800001 	add	r0, r0, #1
60312e98:	e2844001 	add	r4, r4, #1
60312e9c:	eaffff0a 	b	60312acc <DiagVprintf+0x68>
60312ea0:	e3570001 	cmp	r7, #1
60312ea4:	e4993004 	ldr	r3, [r9], #4
60312ea8:	d3a02000 	movle	r2, #0
60312eac:	c3a02001 	movgt	r2, #1
60312eb0:	e35b0000 	cmp	fp, #0
60312eb4:	13a01000 	movne	r1, #0
60312eb8:	02021001 	andeq	r1, r2, #1
60312ebc:	e6ef3073 	uxtb	r3, r3
60312ec0:	e3510000 	cmp	r1, #0
60312ec4:	e58d3018 	str	r3, [sp, #24]
60312ec8:	1a0000e0 	bne	60313250 <DiagVprintf+0x7ec>
60312ecc:	e59d0018 	ldr	r0, [sp, #24]
60312ed0:	e3045218 	movw	r5, #16920	; 0x4218
60312ed4:	e3465031 	movt	r5, #24625	; 0x6031
60312ed8:	e58d201c 	str	r2, [sp, #28]
60312edc:	e12fff35 	blx	r5
60312ee0:	e59d201c 	ldr	r2, [sp, #28]
60312ee4:	e2863001 	add	r3, r6, #1
60312ee8:	e35b0000 	cmp	fp, #0
60312eec:	03a02000 	moveq	r2, #0
60312ef0:	12022001 	andne	r2, r2, #1
60312ef4:	e3520000 	cmp	r2, #0
60312ef8:	01a06003 	moveq	r6, r3
60312efc:	0affff86 	beq	60312d1c <DiagVprintf+0x2b8>
60312f00:	e247b001 	sub	fp, r7, #1
60312f04:	e1a00008 	mov	r0, r8
60312f08:	e12fff35 	blx	r5
60312f0c:	e25bb001 	subs	fp, fp, #1
60312f10:	1afffffb 	bne	60312f04 <DiagVprintf+0x4a0>
60312f14:	e0866007 	add	r6, r6, r7
60312f18:	eaffff7f 	b	60312d1c <DiagVprintf+0x2b8>
60312f1c:	e3500001 	cmp	r0, #1
60312f20:	c2899007 	addgt	r9, r9, #7
60312f24:	c3c99007 	bicgt	r9, r9, #7
60312f28:	d4990004 	ldrle	r0, [r9], #4
60312f2c:	d3a01000 	movle	r1, #0
60312f30:	c5991004 	ldrgt	r1, [r9, #4]
60312f34:	c4990008 	ldrgt	r0, [r9], #8
60312f38:	e35b0000 	cmp	fp, #0
60312f3c:	0a0000b7 	beq	60313220 <DiagVprintf+0x7bc>
60312f40:	e59d3014 	ldr	r3, [sp, #20]
60312f44:	e58d3004 	str	r3, [sp, #4]
60312f48:	e3a02002 	mov	r2, #2
60312f4c:	e3a03000 	mov	r3, #0
60312f50:	e58d3008 	str	r3, [sp, #8]
60312f54:	e58d3000 	str	r3, [sp]
60312f58:	e1a03008 	mov	r3, r8
60312f5c:	ebfffe47 	bl	60312880 <print_unsigned_num>
60312f60:	e1570000 	cmp	r7, r0
60312f64:	d3a03000 	movle	r3, #0
60312f68:	c3a03001 	movgt	r3, #1
60312f6c:	e3580020 	cmp	r8, #32
60312f70:	13a03000 	movne	r3, #0
60312f74:	e3530000 	cmp	r3, #0
60312f78:	0affff66 	beq	60312d18 <DiagVprintf+0x2b4>
60312f7c:	e0478000 	sub	r8, r7, r0
60312f80:	e3580000 	cmp	r8, #0
60312f84:	da0000d8 	ble	603132ec <DiagVprintf+0x888>
60312f88:	e3045218 	movw	r5, #16920	; 0x4218
60312f8c:	e3465031 	movt	r5, #24625	; 0x6031
60312f90:	e3a00020 	mov	r0, #32
60312f94:	e12fff35 	blx	r5
60312f98:	e2588001 	subs	r8, r8, #1
60312f9c:	1afffffb 	bne	60312f90 <DiagVprintf+0x52c>
60312fa0:	eaffff97 	b	60312e04 <DiagVprintf+0x3a0>
60312fa4:	e3a03001 	mov	r3, #1
60312fa8:	e58d3014 	str	r3, [sp, #20]
60312fac:	e3500001 	cmp	r0, #1
60312fb0:	c2899007 	addgt	r9, r9, #7
60312fb4:	c3c99007 	bicgt	r9, r9, #7
60312fb8:	d4990004 	ldrle	r0, [r9], #4
60312fbc:	d3a01000 	movle	r1, #0
60312fc0:	c5991004 	ldrgt	r1, [r9, #4]
60312fc4:	c4990008 	ldrgt	r0, [r9], #8
60312fc8:	e35b0000 	cmp	fp, #0
60312fcc:	0a00005f 	beq	60313150 <DiagVprintf+0x6ec>
60312fd0:	e59d3014 	ldr	r3, [sp, #20]
60312fd4:	e58d3004 	str	r3, [sp, #4]
60312fd8:	e3a02010 	mov	r2, #16
60312fdc:	e3a03000 	mov	r3, #0
60312fe0:	e58d3008 	str	r3, [sp, #8]
60312fe4:	e58d3000 	str	r3, [sp]
60312fe8:	e1a03008 	mov	r3, r8
60312fec:	ebfffe23 	bl	60312880 <print_unsigned_num>
60312ff0:	e1570000 	cmp	r7, r0
60312ff4:	d3a03000 	movle	r3, #0
60312ff8:	c3a03001 	movgt	r3, #1
60312ffc:	e3580020 	cmp	r8, #32
60313000:	13a03000 	movne	r3, #0
60313004:	e3530000 	cmp	r3, #0
60313008:	0affff42 	beq	60312d18 <DiagVprintf+0x2b4>
6031300c:	e0478000 	sub	r8, r7, r0
60313010:	e3580000 	cmp	r8, #0
60313014:	da0000b4 	ble	603132ec <DiagVprintf+0x888>
60313018:	e3045218 	movw	r5, #16920	; 0x4218
6031301c:	e3465031 	movt	r5, #24625	; 0x6031
60313020:	e3a00020 	mov	r0, #32
60313024:	e12fff35 	blx	r5
60313028:	e2588001 	subs	r8, r8, #1
6031302c:	1afffffb 	bne	60313020 <DiagVprintf+0x5bc>
60313030:	e1a00007 	mov	r0, r7
60313034:	eaffff37 	b	60312d18 <DiagVprintf+0x2b4>
60313038:	e2844001 	add	r4, r4, #1
6031303c:	e28bb001 	add	fp, fp, #1
60313040:	eafffea1 	b	60312acc <DiagVprintf+0x68>
60313044:	e3a00025 	mov	r0, #37	; 0x25
60313048:	e3045218 	movw	r5, #16920	; 0x4218
6031304c:	e3465031 	movt	r5, #24625	; 0x6031
60313050:	e2866001 	add	r6, r6, #1
60313054:	e2844001 	add	r4, r4, #1
60313058:	e12fff35 	blx	r5
6031305c:	eafffe90 	b	60312aa4 <DiagVprintf+0x40>
60313060:	e1a03009 	mov	r3, r9
60313064:	e4932004 	ldr	r2, [r3], #4
60313068:	e58d3018 	str	r3, [sp, #24]
6031306c:	e5d20000 	ldrb	r0, [r2]
60313070:	e3500000 	cmp	r0, #0
60313074:	0a0000a0 	beq	603132fc <DiagVprintf+0x898>
60313078:	e1a0c002 	mov	ip, r2
6031307c:	e2629001 	rsb	r9, r2, #1
60313080:	e089100c 	add	r1, r9, ip
60313084:	e5fc3001 	ldrb	r3, [ip, #1]!
60313088:	e3530000 	cmp	r3, #0
6031308c:	1afffffb 	bne	60313080 <DiagVprintf+0x61c>
60313090:	e58d101c 	str	r1, [sp, #28]
60313094:	e35b0000 	cmp	fp, #0
60313098:	0a00003c 	beq	60313190 <DiagVprintf+0x72c>
6031309c:	e3520000 	cmp	r2, #0
603130a0:	0a000099 	beq	6031330c <DiagVprintf+0x8a8>
603130a4:	e3500000 	cmp	r0, #0
603130a8:	0a00000d 	beq	603130e4 <DiagVprintf+0x680>
603130ac:	e3045218 	movw	r5, #16920	; 0x4218
603130b0:	e3465031 	movt	r5, #24625	; 0x6031
603130b4:	e58d4020 	str	r4, [sp, #32]
603130b8:	e2629001 	rsb	r9, r2, #1
603130bc:	e1a04002 	mov	r4, r2
603130c0:	e12fff35 	blx	r5
603130c4:	e084c009 	add	ip, r4, r9
603130c8:	e5f40001 	ldrb	r0, [r4, #1]!
603130cc:	e3500000 	cmp	r0, #0
603130d0:	1afffffa 	bne	603130c0 <DiagVprintf+0x65c>
603130d4:	e59d4020 	ldr	r4, [sp, #32]
603130d8:	e086600c 	add	r6, r6, ip
603130dc:	e35b0000 	cmp	fp, #0
603130e0:	0a000003 	beq	603130f4 <DiagVprintf+0x690>
603130e4:	e59d301c 	ldr	r3, [sp, #28]
603130e8:	e0477003 	sub	r7, r7, r3
603130ec:	e3570000 	cmp	r7, #0
603130f0:	ca000073 	bgt	603132c4 <DiagVprintf+0x860>
603130f4:	e59d9018 	ldr	r9, [sp, #24]
603130f8:	e2844001 	add	r4, r4, #1
603130fc:	eafffe68 	b	60312aa4 <DiagVprintf+0x40>
60313100:	e35b0000 	cmp	fp, #0
60313104:	e4990004 	ldr	r0, [r9], #4
60313108:	e3a01000 	mov	r1, #0
6031310c:	13a03001 	movne	r3, #1
60313110:	03a02008 	moveq	r2, #8
60313114:	03a03001 	moveq	r3, #1
60313118:	158d3004 	strne	r3, [sp, #4]
6031311c:	01cd20f0 	strdeq	r2, [sp]
60313120:	01a0100b 	moveq	r1, fp
60313124:	e3a03030 	mov	r3, #48	; 0x30
60313128:	e3a02010 	mov	r2, #16
6031312c:	158d1008 	strne	r1, [sp, #8]
60313130:	158d1000 	strne	r1, [sp]
60313134:	058db008 	streq	fp, [sp, #8]
60313138:	ebfffdd0 	bl	60312880 <print_unsigned_num>
6031313c:	e2844001 	add	r4, r4, #1
60313140:	e3a03001 	mov	r3, #1
60313144:	e0866000 	add	r6, r6, r0
60313148:	e58d3014 	str	r3, [sp, #20]
6031314c:	eafffe54 	b	60312aa4 <DiagVprintf+0x40>
60313150:	e59d2014 	ldr	r2, [sp, #20]
60313154:	e58d2004 	str	r2, [sp, #4]
60313158:	e1a03008 	mov	r3, r8
6031315c:	e3a02010 	mov	r2, #16
60313160:	e58db008 	str	fp, [sp, #8]
60313164:	e58d7000 	str	r7, [sp]
60313168:	ebfffdc4 	bl	60312880 <print_unsigned_num>
6031316c:	eafffee9 	b	60312d18 <DiagVprintf+0x2b4>
60313170:	e59d2014 	ldr	r2, [sp, #20]
60313174:	e58d2004 	str	r2, [sp, #4]
60313178:	e1a03008 	mov	r3, r8
6031317c:	e3a02008 	mov	r2, #8
60313180:	e58db008 	str	fp, [sp, #8]
60313184:	e58d7000 	str	r7, [sp]
60313188:	ebfffdbc 	bl	60312880 <print_unsigned_num>
6031318c:	eafffee1 	b	60312d18 <DiagVprintf+0x2b4>
60313190:	e59d301c 	ldr	r3, [sp, #28]
60313194:	e0473003 	sub	r3, r7, r3
60313198:	e3530000 	cmp	r3, #0
6031319c:	e58d3020 	str	r3, [sp, #32]
603131a0:	ca000035 	bgt	6031327c <DiagVprintf+0x818>
603131a4:	e3520000 	cmp	r2, #0
603131a8:	0a000055 	beq	60313304 <DiagVprintf+0x8a0>
603131ac:	e5d20000 	ldrb	r0, [r2]
603131b0:	e3500000 	cmp	r0, #0
603131b4:	1affffbc 	bne	603130ac <DiagVprintf+0x648>
603131b8:	eaffffcd 	b	603130f4 <DiagVprintf+0x690>
603131bc:	e59d3014 	ldr	r3, [sp, #20]
603131c0:	e58d3004 	str	r3, [sp, #4]
603131c4:	e3a0200a 	mov	r2, #10
603131c8:	e3a03000 	mov	r3, #0
603131cc:	e58d3008 	str	r3, [sp, #8]
603131d0:	e58d3000 	str	r3, [sp]
603131d4:	e1a03008 	mov	r3, r8
603131d8:	ebfffda8 	bl	60312880 <print_unsigned_num>
603131dc:	e1570000 	cmp	r7, r0
603131e0:	d3a03000 	movle	r3, #0
603131e4:	c3a03001 	movgt	r3, #1
603131e8:	e3580020 	cmp	r8, #32
603131ec:	13a03000 	movne	r3, #0
603131f0:	e3530000 	cmp	r3, #0
603131f4:	0afffec7 	beq	60312d18 <DiagVprintf+0x2b4>
603131f8:	e0478000 	sub	r8, r7, r0
603131fc:	e3580000 	cmp	r8, #0
60313200:	da000039 	ble	603132ec <DiagVprintf+0x888>
60313204:	e3045218 	movw	r5, #16920	; 0x4218
60313208:	e3465031 	movt	r5, #24625	; 0x6031
6031320c:	e3a00020 	mov	r0, #32
60313210:	e12fff35 	blx	r5
60313214:	e2588001 	subs	r8, r8, #1
60313218:	1afffffb 	bne	6031320c <DiagVprintf+0x7a8>
6031321c:	eafffef8 	b	60312e04 <DiagVprintf+0x3a0>
60313220:	e59d2014 	ldr	r2, [sp, #20]
60313224:	e58d2004 	str	r2, [sp, #4]
60313228:	e1a03008 	mov	r3, r8
6031322c:	e3a02002 	mov	r2, #2
60313230:	e58db008 	str	fp, [sp, #8]
60313234:	e58d7000 	str	r7, [sp]
60313238:	ebfffd90 	bl	60312880 <print_unsigned_num>
6031323c:	eafffeb5 	b	60312d18 <DiagVprintf+0x2b4>
60313240:	e2700000 	rsbs	r0, r0, #0
60313244:	e2e11000 	rsc	r1, r1, #0
60313248:	e3a02001 	mov	r2, #1
6031324c:	eafffed2 	b	60312d9c <DiagVprintf+0x338>
60313250:	e3045218 	movw	r5, #16920	; 0x4218
60313254:	e3465031 	movt	r5, #24625	; 0x6031
60313258:	e247b001 	sub	fp, r7, #1
6031325c:	e1a00008 	mov	r0, r8
60313260:	e12fff35 	blx	r5
60313264:	e25bb001 	subs	fp, fp, #1
60313268:	1afffffb 	bne	6031325c <DiagVprintf+0x7f8>
6031326c:	e59d0018 	ldr	r0, [sp, #24]
60313270:	e0866007 	add	r6, r6, r7
60313274:	e12fff35 	blx	r5
60313278:	eafffea7 	b	60312d1c <DiagVprintf+0x2b8>
6031327c:	e1a09003 	mov	r9, r3
60313280:	e1a03004 	mov	r3, r4
60313284:	e3045218 	movw	r5, #16920	; 0x4218
60313288:	e3465031 	movt	r5, #24625	; 0x6031
6031328c:	e1a04008 	mov	r4, r8
60313290:	e1a08003 	mov	r8, r3
60313294:	e58d2024 	str	r2, [sp, #36]	; 0x24
60313298:	e1a00004 	mov	r0, r4
6031329c:	e12fff35 	blx	r5
603132a0:	e2599001 	subs	r9, r9, #1
603132a4:	1afffffb 	bne	60313298 <DiagVprintf+0x834>
603132a8:	e1a03008 	mov	r3, r8
603132ac:	e1a08004 	mov	r8, r4
603132b0:	e1a04003 	mov	r4, r3
603132b4:	e59d3020 	ldr	r3, [sp, #32]
603132b8:	e59d2024 	ldr	r2, [sp, #36]	; 0x24
603132bc:	e0866003 	add	r6, r6, r3
603132c0:	eaffffb7 	b	603131a4 <DiagVprintf+0x740>
603132c4:	e3045218 	movw	r5, #16920	; 0x4218
603132c8:	e3465031 	movt	r5, #24625	; 0x6031
603132cc:	e1a0b007 	mov	fp, r7
603132d0:	e1a00008 	mov	r0, r8
603132d4:	e12fff35 	blx	r5
603132d8:	e25bb001 	subs	fp, fp, #1
603132dc:	1afffffb 	bne	603132d0 <DiagVprintf+0x86c>
603132e0:	e59d9018 	ldr	r9, [sp, #24]
603132e4:	e0866007 	add	r6, r6, r7
603132e8:	eafffe8b 	b	60312d1c <DiagVprintf+0x2b8>
603132ec:	e1a07000 	mov	r7, r0
603132f0:	eafffec3 	b	60312e04 <DiagVprintf+0x3a0>
603132f4:	e1a06000 	mov	r6, r0
603132f8:	eafffe73 	b	60312ccc <DiagVprintf+0x268>
603132fc:	e58d001c 	str	r0, [sp, #28]
60313300:	eaffff63 	b	60313094 <DiagVprintf+0x630>
60313304:	e2466001 	sub	r6, r6, #1
60313308:	eaffff79 	b	603130f4 <DiagVprintf+0x690>
6031330c:	e2466001 	sub	r6, r6, #1
60313310:	eaffff73 	b	603130e4 <DiagVprintf+0x680>

60313314 <DiagVprintfNano>:
60313314:	e92d4ff0 	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
60313318:	e1a04000 	mov	r4, r0
6031331c:	e5d00000 	ldrb	r0, [r0]
60313320:	e24dd02c 	sub	sp, sp, #44	; 0x2c
60313324:	e3500000 	cmp	r0, #0
60313328:	0a00017d 	beq	60313924 <DiagVprintfNano+0x610>
6031332c:	e3046218 	movw	r6, #16920	; 0x4218
60313330:	e3466031 	movt	r6, #24625	; 0x6031
60313334:	e30c3ccd 	movw	r3, #52429	; 0xcccd
60313338:	e34c3ccc 	movt	r3, #52428	; 0xcccc
6031333c:	e1a08001 	mov	r8, r1
60313340:	e3a07000 	mov	r7, #0
60313344:	e58d3004 	str	r3, [sp, #4]
60313348:	ea000004 	b	60313360 <DiagVprintfNano+0x4c>
6031334c:	e2877001 	add	r7, r7, #1
60313350:	e12fff36 	blx	r6
60313354:	e5d40000 	ldrb	r0, [r4]
60313358:	e3500000 	cmp	r0, #0
6031335c:	0a000069 	beq	60313508 <DiagVprintfNano+0x1f4>
60313360:	e2844001 	add	r4, r4, #1
60313364:	e3500025 	cmp	r0, #37	; 0x25
60313368:	1afffff7 	bne	6031334c <DiagVprintfNano+0x38>
6031336c:	e5d42000 	ldrb	r2, [r4]
60313370:	e3a05000 	mov	r5, #0
60313374:	e1a03005 	mov	r3, r5
60313378:	e3a0000a 	mov	r0, #10
6031337c:	e3520000 	cmp	r2, #0
60313380:	0a000075 	beq	6031355c <DiagVprintfNano+0x248>
60313384:	e2421020 	sub	r1, r2, #32
60313388:	e6efc071 	uxtb	ip, r1
6031338c:	e35c0058 	cmp	ip, #88	; 0x58
60313390:	8a00005b 	bhi	60313504 <DiagVprintfNano+0x1f0>
60313394:	e3510058 	cmp	r1, #88	; 0x58
60313398:	979ff101 	ldrls	pc, [pc, r1, lsl #2]
6031339c:	ea000058 	b	60313504 <DiagVprintfNano+0x1f0>
603133a0:	60313514 	.word	0x60313514
603133a4:	60313504 	.word	0x60313504
603133a8:	60313504 	.word	0x60313504
603133ac:	60313504 	.word	0x60313504
603133b0:	60313504 	.word	0x60313504
603133b4:	60313758 	.word	0x60313758
603133b8:	60313504 	.word	0x60313504
603133bc:	60313504 	.word	0x60313504
603133c0:	60313504 	.word	0x60313504
603133c4:	60313504 	.word	0x60313504
603133c8:	60313504 	.word	0x60313504
603133cc:	60313504 	.word	0x60313504
603133d0:	60313504 	.word	0x60313504
603133d4:	60313504 	.word	0x60313504
603133d8:	60313504 	.word	0x60313504
603133dc:	60313504 	.word	0x60313504
603133e0:	60313514 	.word	0x60313514
603133e4:	60313564 	.word	0x60313564
603133e8:	60313564 	.word	0x60313564
603133ec:	60313564 	.word	0x60313564
603133f0:	60313564 	.word	0x60313564
603133f4:	60313564 	.word	0x60313564
603133f8:	60313564 	.word	0x60313564
603133fc:	60313564 	.word	0x60313564
60313400:	60313564 	.word	0x60313564
60313404:	60313564 	.word	0x60313564
60313408:	60313504 	.word	0x60313504
6031340c:	60313504 	.word	0x60313504
60313410:	60313504 	.word	0x60313504
60313414:	60313504 	.word	0x60313504
60313418:	60313504 	.word	0x60313504
6031341c:	60313504 	.word	0x60313504
60313420:	60313504 	.word	0x60313504
60313424:	60313504 	.word	0x60313504
60313428:	60313504 	.word	0x60313504
6031342c:	6031367c 	.word	0x6031367c
60313430:	60313504 	.word	0x60313504
60313434:	60313504 	.word	0x60313504
60313438:	60313504 	.word	0x60313504
6031343c:	60313504 	.word	0x60313504
60313440:	60313504 	.word	0x60313504
60313444:	60313504 	.word	0x60313504
60313448:	60313504 	.word	0x60313504
6031344c:	60313504 	.word	0x60313504
60313450:	60313504 	.word	0x60313504
60313454:	60313504 	.word	0x60313504
60313458:	60313504 	.word	0x60313504
6031345c:	60313504 	.word	0x60313504
60313460:	60313504 	.word	0x60313504
60313464:	60313504 	.word	0x60313504
60313468:	60313504 	.word	0x60313504
6031346c:	60313504 	.word	0x60313504
60313470:	60313504 	.word	0x60313504
60313474:	60313504 	.word	0x60313504
60313478:	60313504 	.word	0x60313504
6031347c:	60313504 	.word	0x60313504
60313480:	60313504 	.word	0x60313504
60313484:	60313504 	.word	0x60313504
60313488:	60313504 	.word	0x60313504
6031348c:	60313504 	.word	0x60313504
60313490:	60313504 	.word	0x60313504
60313494:	60313504 	.word	0x60313504
60313498:	60313504 	.word	0x60313504
6031349c:	60313504 	.word	0x60313504
603134a0:	60313504 	.word	0x60313504
603134a4:	60313504 	.word	0x60313504
603134a8:	60313504 	.word	0x60313504
603134ac:	6031367c 	.word	0x6031367c
603134b0:	6031356c 	.word	0x6031356c
603134b4:	60313504 	.word	0x60313504
603134b8:	60313504 	.word	0x60313504
603134bc:	60313504 	.word	0x60313504
603134c0:	60313504 	.word	0x60313504
603134c4:	6031356c 	.word	0x6031356c
603134c8:	60313504 	.word	0x60313504
603134cc:	60313504 	.word	0x60313504
603134d0:	60313504 	.word	0x60313504
603134d4:	60313504 	.word	0x60313504
603134d8:	60313504 	.word	0x60313504
603134dc:	60313504 	.word	0x60313504
603134e0:	60313504 	.word	0x60313504
603134e4:	60313504 	.word	0x60313504
603134e8:	60313504 	.word	0x60313504
603134ec:	60313828 	.word	0x60313828
603134f0:	60313504 	.word	0x60313504
603134f4:	6031376c 	.word	0x6031376c
603134f8:	60313504 	.word	0x60313504
603134fc:	60313504 	.word	0x60313504
60313500:	603136a0 	.word	0x603136a0
60313504:	e3e07000 	mvn	r7, #0
60313508:	e1a00007 	mov	r0, r7
6031350c:	e28dd02c 	add	sp, sp, #44	; 0x2c
60313510:	e8bd8ff0 	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
60313514:	e2844001 	add	r4, r4, #1
60313518:	e1a03002 	mov	r3, r2
6031351c:	e5d42000 	ldrb	r2, [r4]
60313520:	e3520000 	cmp	r2, #0
60313524:	0afffff7 	beq	60313508 <DiagVprintfNano+0x1f4>
60313528:	e3a05000 	mov	r5, #0
6031352c:	ea000003 	b	60313540 <DiagVprintfNano+0x22c>
60313530:	e5f42001 	ldrb	r2, [r4, #1]!
60313534:	e2415030 	sub	r5, r1, #48	; 0x30
60313538:	e3520000 	cmp	r2, #0
6031353c:	0afffff1 	beq	60313508 <DiagVprintfNano+0x1f4>
60313540:	e0212590 	mla	r1, r0, r5, r2
60313544:	e2422030 	sub	r2, r2, #48	; 0x30
60313548:	e3520009 	cmp	r2, #9
6031354c:	9afffff7 	bls	60313530 <DiagVprintfNano+0x21c>
60313550:	e5d42000 	ldrb	r2, [r4]
60313554:	e3520000 	cmp	r2, #0
60313558:	1affff89 	bne	60313384 <DiagVprintfNano+0x70>
6031355c:	e2844001 	add	r4, r4, #1
60313560:	eaffff7b 	b	60313354 <DiagVprintfNano+0x40>
60313564:	e3a03020 	mov	r3, #32
60313568:	eaffffee 	b	60313528 <DiagVprintfNano+0x214>
6031356c:	e1a01008 	mov	r1, r8
60313570:	e28da01c 	add	sl, sp, #28
60313574:	e59dc004 	ldr	ip, [sp, #4]
60313578:	e4912004 	ldr	r2, [r1], #4
6031357c:	e58d100c 	str	r1, [sp, #12]
60313580:	e3520000 	cmp	r2, #0
60313584:	b3a01001 	movlt	r1, #1
60313588:	a3a01000 	movge	r1, #0
6031358c:	b2622000 	rsblt	r2, r2, #0
60313590:	e58d1008 	str	r1, [sp, #8]
60313594:	e3a0b000 	mov	fp, #0
60313598:	e1a0100a 	mov	r1, sl
6031359c:	e3a0000a 	mov	r0, #10
603135a0:	e08e829c 	umull	r8, lr, ip, r2
603135a4:	e3520009 	cmp	r2, #9
603135a8:	e1a0800b 	mov	r8, fp
603135ac:	e1a0e1ae 	lsr	lr, lr, #3
603135b0:	e28bb001 	add	fp, fp, #1
603135b4:	e0692e90 	mls	r9, r0, lr, r2
603135b8:	e1a0200e 	mov	r2, lr
603135bc:	e2899030 	add	r9, r9, #48	; 0x30
603135c0:	e6ef9079 	uxtb	r9, r9
603135c4:	e4c19001 	strb	r9, [r1], #1
603135c8:	8afffff4 	bhi	603135a0 <DiagVprintfNano+0x28c>
603135cc:	e59d2008 	ldr	r2, [sp, #8]
603135d0:	e3520000 	cmp	r2, #0
603135d4:	1a0000ae 	bne	60313894 <DiagVprintfNano+0x580>
603135d8:	e3550000 	cmp	r5, #0
603135dc:	da000018 	ble	60313644 <DiagVprintfNano+0x330>
603135e0:	e155000b 	cmp	r5, fp
603135e4:	da000016 	ble	60313644 <DiagVprintfNano+0x330>
603135e8:	e58d4014 	str	r4, [sp, #20]
603135ec:	e1a04003 	mov	r4, r3
603135f0:	e58d5010 	str	r5, [sp, #16]
603135f4:	e2455001 	sub	r5, r5, #1
603135f8:	e1a00004 	mov	r0, r4
603135fc:	e12fff36 	blx	r6
60313600:	e15b0005 	cmp	fp, r5
60313604:	1afffffa 	bne	603135f4 <DiagVprintfNano+0x2e0>
60313608:	e59d1008 	ldr	r1, [sp, #8]
6031360c:	e59d5010 	ldr	r5, [sp, #16]
60313610:	e1e0200b 	mvn	r2, fp
60313614:	e0822005 	add	r2, r2, r5
60313618:	e3510000 	cmp	r1, #0
6031361c:	e2811001 	add	r1, r1, #1
60313620:	e1a03004 	mov	r3, r4
60313624:	e0822001 	add	r2, r2, r1
60313628:	e59d4014 	ldr	r4, [sp, #20]
6031362c:	e58d2008 	str	r2, [sp, #8]
60313630:	0a000003 	beq	60313644 <DiagVprintfNano+0x330>
60313634:	e31300df 	tst	r3, #223	; 0xdf
60313638:	1a000001 	bne	60313644 <DiagVprintfNano+0x330>
6031363c:	e3a0002d 	mov	r0, #45	; 0x2d
60313640:	e12fff36 	blx	r6
60313644:	e08a5008 	add	r5, sl, r8
60313648:	e1a00009 	mov	r0, r9
6031364c:	ea000000 	b	60313654 <DiagVprintfNano+0x340>
60313650:	e5750001 	ldrb	r0, [r5, #-1]!
60313654:	e12fff36 	blx	r6
60313658:	e15a0005 	cmp	sl, r5
6031365c:	1afffffb 	bne	60313650 <DiagVprintfNano+0x33c>
60313660:	e59d3008 	ldr	r3, [sp, #8]
60313664:	e2877001 	add	r7, r7, #1
60313668:	e0877008 	add	r7, r7, r8
6031366c:	e0837007 	add	r7, r3, r7
60313670:	e59d800c 	ldr	r8, [sp, #12]
60313674:	e2844001 	add	r4, r4, #1
60313678:	eaffff35 	b	60313354 <DiagVprintfNano+0x40>
6031367c:	e4989004 	ldr	r9, [r8], #4
60313680:	e3550001 	cmp	r5, #1
60313684:	e6ef9079 	uxtb	r9, r9
60313688:	ca000091 	bgt	603138d4 <DiagVprintfNano+0x5c0>
6031368c:	e1a00009 	mov	r0, r9
60313690:	e2877001 	add	r7, r7, #1
60313694:	e2844001 	add	r4, r4, #1
60313698:	e12fff36 	blx	r6
6031369c:	eaffff2c 	b	60313354 <DiagVprintfNano+0x40>
603136a0:	e1a02008 	mov	r2, r8
603136a4:	e28da01c 	add	sl, sp, #28
603136a8:	e1a0000a 	mov	r0, sl
603136ac:	e4921004 	ldr	r1, [r2], #4
603136b0:	e3a09000 	mov	r9, #0
603136b4:	e58d2008 	str	r2, [sp, #8]
603136b8:	ea000000 	b	603136c0 <DiagVprintfNano+0x3ac>
603136bc:	e1a09008 	mov	r9, r8
603136c0:	e201200f 	and	r2, r1, #15
603136c4:	e3520009 	cmp	r2, #9
603136c8:	e282b057 	add	fp, r2, #87	; 0x57
603136cc:	9282b030 	addls	fp, r2, #48	; 0x30
603136d0:	e1a02221 	lsr	r2, r1, #4
603136d4:	e351000f 	cmp	r1, #15
603136d8:	e2898001 	add	r8, r9, #1
603136dc:	e1a01002 	mov	r1, r2
603136e0:	e4c0b001 	strb	fp, [r0], #1
603136e4:	8afffff4 	bhi	603136bc <DiagVprintfNano+0x3a8>
603136e8:	e3550000 	cmp	r5, #0
603136ec:	da000074 	ble	603138c4 <DiagVprintfNano+0x5b0>
603136f0:	e1550008 	cmp	r5, r8
603136f4:	da000072 	ble	603138c4 <DiagVprintfNano+0x5b0>
603136f8:	e58d4010 	str	r4, [sp, #16]
603136fc:	e1a04003 	mov	r4, r3
60313700:	e58d500c 	str	r5, [sp, #12]
60313704:	e2455001 	sub	r5, r5, #1
60313708:	e1a00004 	mov	r0, r4
6031370c:	e12fff36 	blx	r6
60313710:	e1580005 	cmp	r8, r5
60313714:	1afffffa 	bne	60313704 <DiagVprintfNano+0x3f0>
60313718:	e59d500c 	ldr	r5, [sp, #12]
6031371c:	e59d4010 	ldr	r4, [sp, #16]
60313720:	e0458008 	sub	r8, r5, r8
60313724:	e08a5009 	add	r5, sl, r9
60313728:	e1a0000b 	mov	r0, fp
6031372c:	ea000000 	b	60313734 <DiagVprintfNano+0x420>
60313730:	e5750001 	ldrb	r0, [r5, #-1]!
60313734:	e12fff36 	blx	r6
60313738:	e155000a 	cmp	r5, sl
6031373c:	1afffffb 	bne	60313730 <DiagVprintfNano+0x41c>
60313740:	e2877001 	add	r7, r7, #1
60313744:	e0877009 	add	r7, r7, r9
60313748:	e0887007 	add	r7, r8, r7
6031374c:	e2844001 	add	r4, r4, #1
60313750:	e59d8008 	ldr	r8, [sp, #8]
60313754:	eafffefe 	b	60313354 <DiagVprintfNano+0x40>
60313758:	e3a00025 	mov	r0, #37	; 0x25
6031375c:	e2877001 	add	r7, r7, #1
60313760:	e2844001 	add	r4, r4, #1
60313764:	e12fff36 	blx	r6
60313768:	eafffef9 	b	60313354 <DiagVprintfNano+0x40>
6031376c:	e1a02008 	mov	r2, r8
60313770:	e28da01c 	add	sl, sp, #28
60313774:	e1a0000a 	mov	r0, sl
60313778:	e4921004 	ldr	r1, [r2], #4
6031377c:	e58d2008 	str	r2, [sp, #8]
60313780:	e59d2004 	ldr	r2, [sp, #4]
60313784:	e3a08000 	mov	r8, #0
60313788:	e3a0c00a 	mov	ip, #10
6031378c:	e08e9192 	umull	r9, lr, r2, r1
60313790:	e3510009 	cmp	r1, #9
60313794:	e1a0b008 	mov	fp, r8
60313798:	e1a0e1ae 	lsr	lr, lr, #3
6031379c:	e2888001 	add	r8, r8, #1
603137a0:	e0691e9c 	mls	r9, ip, lr, r1
603137a4:	e1a0100e 	mov	r1, lr
603137a8:	e2899030 	add	r9, r9, #48	; 0x30
603137ac:	e6ef9079 	uxtb	r9, r9
603137b0:	e4c09001 	strb	r9, [r0], #1
603137b4:	8afffff4 	bhi	6031378c <DiagVprintfNano+0x478>
603137b8:	e3550000 	cmp	r5, #0
603137bc:	da000042 	ble	603138cc <DiagVprintfNano+0x5b8>
603137c0:	e1550008 	cmp	r5, r8
603137c4:	da000040 	ble	603138cc <DiagVprintfNano+0x5b8>
603137c8:	e58d4010 	str	r4, [sp, #16]
603137cc:	e1a04003 	mov	r4, r3
603137d0:	e58d500c 	str	r5, [sp, #12]
603137d4:	e2455001 	sub	r5, r5, #1
603137d8:	e1a00004 	mov	r0, r4
603137dc:	e12fff36 	blx	r6
603137e0:	e1580005 	cmp	r8, r5
603137e4:	1afffffa 	bne	603137d4 <DiagVprintfNano+0x4c0>
603137e8:	e59d500c 	ldr	r5, [sp, #12]
603137ec:	e59d4010 	ldr	r4, [sp, #16]
603137f0:	e0458008 	sub	r8, r5, r8
603137f4:	e08a500b 	add	r5, sl, fp
603137f8:	e1a00009 	mov	r0, r9
603137fc:	ea000000 	b	60313804 <DiagVprintfNano+0x4f0>
60313800:	e5750001 	ldrb	r0, [r5, #-1]!
60313804:	e12fff36 	blx	r6
60313808:	e15a0005 	cmp	sl, r5
6031380c:	1afffffb 	bne	60313800 <DiagVprintfNano+0x4ec>
60313810:	e2877001 	add	r7, r7, #1
60313814:	e087700b 	add	r7, r7, fp
60313818:	e0887007 	add	r7, r8, r7
6031381c:	e2844001 	add	r4, r4, #1
60313820:	e59d8008 	ldr	r8, [sp, #8]
60313824:	eafffeca 	b	60313354 <DiagVprintfNano+0x40>
60313828:	e4989004 	ldr	r9, [r8], #4
6031382c:	e5d92000 	ldrb	r2, [r9]
60313830:	e3520000 	cmp	r2, #0
60313834:	0a000006 	beq	60313854 <DiagVprintfNano+0x540>
60313838:	e1a02009 	mov	r2, r9
6031383c:	e269a001 	rsb	sl, r9, #1
60313840:	e08a0002 	add	r0, sl, r2
60313844:	e5f21001 	ldrb	r1, [r2, #1]!
60313848:	e3510000 	cmp	r1, #0
6031384c:	1afffffb 	bne	60313840 <DiagVprintfNano+0x52c>
60313850:	e0455000 	sub	r5, r5, r0
60313854:	e3550000 	cmp	r5, #0
60313858:	ca000026 	bgt	603138f8 <DiagVprintfNano+0x5e4>
6031385c:	e3590000 	cmp	r9, #0
60313860:	0a000031 	beq	6031392c <DiagVprintfNano+0x618>
60313864:	e5d90000 	ldrb	r0, [r9]
60313868:	e3500000 	cmp	r0, #0
6031386c:	0affff3a 	beq	6031355c <DiagVprintfNano+0x248>
60313870:	e269a001 	rsb	sl, r9, #1
60313874:	e12fff36 	blx	r6
60313878:	e089300a 	add	r3, r9, sl
6031387c:	e5f90001 	ldrb	r0, [r9, #1]!
60313880:	e3500000 	cmp	r0, #0
60313884:	1afffffa 	bne	60313874 <DiagVprintfNano+0x560>
60313888:	e0877003 	add	r7, r7, r3
6031388c:	e2844001 	add	r4, r4, #1
60313890:	eafffeaf 	b	60313354 <DiagVprintfNano+0x40>
60313894:	e2455001 	sub	r5, r5, #1
60313898:	e3530030 	cmp	r3, #48	; 0x30
6031389c:	1a00001d 	bne	60313918 <DiagVprintfNano+0x604>
603138a0:	e3a0002d 	mov	r0, #45	; 0x2d
603138a4:	e58d3010 	str	r3, [sp, #16]
603138a8:	e12fff36 	blx	r6
603138ac:	e59d3010 	ldr	r3, [sp, #16]
603138b0:	e3550000 	cmp	r5, #0
603138b4:	daffff62 	ble	60313644 <DiagVprintfNano+0x330>
603138b8:	e15b0005 	cmp	fp, r5
603138bc:	baffff49 	blt	603135e8 <DiagVprintfNano+0x2d4>
603138c0:	eaffff5b 	b	60313634 <DiagVprintfNano+0x320>
603138c4:	e3a08000 	mov	r8, #0
603138c8:	eaffff95 	b	60313724 <DiagVprintfNano+0x410>
603138cc:	e3a08000 	mov	r8, #0
603138d0:	eaffffc7 	b	603137f4 <DiagVprintfNano+0x4e0>
603138d4:	e245a001 	sub	sl, r5, #1
603138d8:	e1a0b003 	mov	fp, r3
603138dc:	e1a0000b 	mov	r0, fp
603138e0:	e12fff36 	blx	r6
603138e4:	e25aa001 	subs	sl, sl, #1
603138e8:	1afffffb 	bne	603138dc <DiagVprintfNano+0x5c8>
603138ec:	e2477001 	sub	r7, r7, #1
603138f0:	e0857007 	add	r7, r5, r7
603138f4:	eaffff64 	b	6031368c <DiagVprintfNano+0x378>
603138f8:	e1a0a005 	mov	sl, r5
603138fc:	e1a0b003 	mov	fp, r3
60313900:	e1a0000b 	mov	r0, fp
60313904:	e12fff36 	blx	r6
60313908:	e25aa001 	subs	sl, sl, #1
6031390c:	1afffffb 	bne	60313900 <DiagVprintfNano+0x5ec>
60313910:	e0877005 	add	r7, r7, r5
60313914:	eaffffd0 	b	6031385c <DiagVprintfNano+0x548>
60313918:	e3550000 	cmp	r5, #0
6031391c:	caffffe5 	bgt	603138b8 <DiagVprintfNano+0x5a4>
60313920:	eaffff43 	b	60313634 <DiagVprintfNano+0x320>
60313924:	e1a07000 	mov	r7, r0
60313928:	eafffef6 	b	60313508 <DiagVprintfNano+0x1f4>
6031392c:	e2477001 	sub	r7, r7, #1
60313930:	eaffff09 	b	6031355c <DiagVprintfNano+0x248>

60313934 <DiagPrintf>:
60313934:	e92d000f 	push	{r0, r1, r2, r3}
60313938:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
6031393c:	e24dd00c 	sub	sp, sp, #12
60313940:	e28d1014 	add	r1, sp, #20
60313944:	e59d0010 	ldr	r0, [sp, #16]
60313948:	e58d1004 	str	r1, [sp, #4]
6031394c:	ebfffc44 	bl	60312a64 <DiagVprintf>
60313950:	e28dd00c 	add	sp, sp, #12
60313954:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
60313958:	e28dd010 	add	sp, sp, #16
6031395c:	e12fff1e 	bx	lr

60313960 <DiagPrintfNano>:
60313960:	e92d000f 	push	{r0, r1, r2, r3}
60313964:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
60313968:	e24dd00c 	sub	sp, sp, #12
6031396c:	e28d1014 	add	r1, sp, #20
60313970:	e59d0010 	ldr	r0, [sp, #16]
60313974:	e58d1004 	str	r1, [sp, #4]
60313978:	ebfffe65 	bl	60313314 <DiagVprintfNano>
6031397c:	e28dd00c 	add	sp, sp, #12
60313980:	e49de004 	pop	{lr}		; (ldr lr, [sp], #4)
60313984:	e28dd010 	add	sp, sp, #16
60313988:	e12fff1e 	bx	lr

6031398c <io_assert_failed>:
6031398c:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
60313990:	e1a0c000 	mov	ip, r0
60313994:	e24dd00c 	sub	sp, sp, #12
60313998:	e1a00001 	mov	r0, r1
6031399c:	e3063bd8 	movw	r3, #27608	; 0x6bd8
603139a0:	e3463031 	movt	r3, #24625	; 0x6031
603139a4:	e3051394 	movw	r1, #21396	; 0x5394
603139a8:	e3461031 	movt	r1, #24625	; 0x6031
603139ac:	e58d0004 	str	r0, [sp, #4]
603139b0:	e3a02045 	mov	r2, #69	; 0x45
603139b4:	e3a00002 	mov	r0, #2
603139b8:	e58dc000 	str	ip, [sp]
603139bc:	ebffe4cd 	bl	6030ccf8 <rtk_log_write_nano>
603139c0:	eafffffe 	b	603139c0 <io_assert_failed+0x34>

603139c4 <__wrap_strncpy>:
603139c4:	eaffb6b6 	b	603014a4 <strncpy>

603139c8 <OTPSetCRC>:
603139c8:	e92d4070 	push	{r4, r5, r6, lr}
603139cc:	e3085730 	movw	r5, #34608	; 0x8730
603139d0:	e24dd010 	sub	sp, sp, #16
603139d4:	ebffd5ce 	bl	60309114 <OTPGetCRC>
603139d8:	e3a04e37 	mov	r4, #880	; 0x370
603139dc:	e1a06000 	mov	r6, r0
603139e0:	e0855800 	add	r5, r5, r0, lsl #16
603139e4:	e1a00004 	mov	r0, r4
603139e8:	e28d100c 	add	r1, sp, #12
603139ec:	ebffd677 	bl	603093d0 <OTP_Read32>
603139f0:	e59d300c 	ldr	r3, [sp, #12]
603139f4:	e3730001 	cmn	r3, #1
603139f8:	0a00000d 	beq	60313a34 <OTPSetCRC+0x6c>
603139fc:	e2844004 	add	r4, r4, #4
60313a00:	e1530005 	cmp	r3, r5
60313a04:	0a000021 	beq	60313a90 <OTPSetCRC+0xc8>
60313a08:	e3540d0e 	cmp	r4, #896	; 0x380
60313a0c:	1afffff4 	bne	603139e4 <OTPSetCRC+0x1c>
60313a10:	e3a00002 	mov	r0, #2
60313a14:	e3a02045 	mov	r2, #69	; 0x45
60313a18:	e3063c2c 	movw	r3, #27692	; 0x6c2c
60313a1c:	e3463031 	movt	r3, #24625	; 0x6031
60313a20:	e3061b9c 	movw	r1, #27548	; 0x6b9c
60313a24:	e3461031 	movt	r1, #24625	; 0x6031
60313a28:	ebffe4b2 	bl	6030ccf8 <rtk_log_write_nano>
60313a2c:	e3e00000 	mvn	r0, #0
60313a30:	ea000017 	b	60313a94 <OTPSetCRC+0xcc>
60313a34:	e3a02049 	mov	r2, #73	; 0x49
60313a38:	e3a00004 	mov	r0, #4
60313a3c:	e1cd40f0 	strd	r4, [sp]
60313a40:	e3063c10 	movw	r3, #27664	; 0x6c10
60313a44:	e3463031 	movt	r3, #24625	; 0x6031
60313a48:	e3061b9c 	movw	r1, #27548	; 0x6b9c
60313a4c:	e3461031 	movt	r1, #24625	; 0x6031
60313a50:	ebffe4a8 	bl	6030ccf8 <rtk_log_write_nano>
60313a54:	e3a01030 	mov	r1, #48	; 0x30
60313a58:	e1a00004 	mov	r0, r4
60313a5c:	ebffd5cb 	bl	60309190 <OTP_Write8>
60313a60:	e3a01087 	mov	r1, #135	; 0x87
60313a64:	e2840001 	add	r0, r4, #1
60313a68:	ebffd5c8 	bl	60309190 <OTP_Write8>
60313a6c:	e6ef1076 	uxtb	r1, r6
60313a70:	e2840002 	add	r0, r4, #2
60313a74:	ebffd5c5 	bl	60309190 <OTP_Write8>
60313a78:	e7e71456 	ubfx	r1, r6, #8, #8
60313a7c:	e2840003 	add	r0, r4, #3
60313a80:	ebffd5c2 	bl	60309190 <OTP_Write8>
60313a84:	e2443fdd 	sub	r3, r4, #884	; 0x374
60313a88:	e3530008 	cmp	r3, #8
60313a8c:	9a000002 	bls	60313a9c <OTPSetCRC+0xd4>
60313a90:	e3a00000 	mov	r0, #0
60313a94:	e28dd010 	add	sp, sp, #16
60313a98:	e8bd8070 	pop	{r4, r5, r6, pc}
60313a9c:	e3a01000 	mov	r1, #0
60313aa0:	e2440004 	sub	r0, r4, #4
60313aa4:	ebffd5b9 	bl	60309190 <OTP_Write8>
60313aa8:	e3a01000 	mov	r1, #0
60313aac:	e2440003 	sub	r0, r4, #3
60313ab0:	ebffd5b6 	bl	60309190 <OTP_Write8>
60313ab4:	e3a01000 	mov	r1, #0
60313ab8:	e2440002 	sub	r0, r4, #2
60313abc:	ebffd5b3 	bl	60309190 <OTP_Write8>
60313ac0:	e3a01000 	mov	r1, #0
60313ac4:	e2440001 	sub	r0, r4, #1
60313ac8:	ebffd5b0 	bl	60309190 <OTP_Write8>
60313acc:	e3a00000 	mov	r0, #0
60313ad0:	eaffffef 	b	60313a94 <OTPSetCRC+0xcc>

60313ad4 <SOCPS_PG_Enter>:
60313ad4:	e3a00022 	mov	r0, #34	; 0x22
60313ad8:	e3400101 	movt	r0, #257	; 0x101
60313adc:	e92d4010 	push	{r4, lr}
60313ae0:	e3001430 	movw	r1, #1072	; 0x430
60313ae4:	e3461030 	movt	r1, #24624	; 0x6030
60313ae8:	e30c6bb4 	movw	r6, #52148	; 0xcbb4
60313aec:	e3466030 	movt	r6, #24624	; 0x6030
60313af0:	e3065c44 	movw	r5, #27716	; 0x6c44
60313af4:	e3465031 	movt	r5, #24625	; 0x6031
60313af8:	e3064c68 	movw	r4, #27752	; 0x6c68
60313afc:	e3464031 	movt	r4, #24625	; 0x6031
60313b00:	ebffdd86 	bl	6030b120 <psci_cpu_suspend>
60313b04:	e3a00ffa 	mov	r0, #1000	; 0x3e8
60313b08:	e12fff36 	blx	r6
60313b0c:	e1a03005 	mov	r3, r5
60313b10:	e3a02045 	mov	r2, #69	; 0x45
60313b14:	e1a01004 	mov	r1, r4
60313b18:	e3a00002 	mov	r0, #2
60313b1c:	ebffe451 	bl	6030cc68 <rtk_log_write>
60313b20:	eafffff7 	b	60313b04 <SOCPS_PG_Enter+0x30>

60313b24 <cpu_suspend_save>:
60313b24:	e1a03000 	mov	r3, r0
60313b28:	e30c0880 	movw	r0, #51328	; 0xc880
60313b2c:	e3460031 	movt	r0, #24625	; 0x6031
60313b30:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
60313b34:	e4803004 	str	r3, [r0], #4
60313b38:	ebffb214 	bl	60300390 <cpu_do_suspend>
60313b3c:	ee113f10 	mrc	15, 0, r3, cr1, cr0, {0}
60313b40:	e3130004 	tst	r3, #4
60313b44:	08bd81f0 	popeq	{r4, r5, r6, r7, r8, pc}
60313b48:	ee306f30 	mrc	15, 1, r6, cr0, cr0, {1}
60313b4c:	e3a04000 	mov	r4, #0
60313b50:	e1a05004 	mov	r5, r4
60313b54:	e1a03536 	lsr	r3, r6, r5
60313b58:	e2033007 	and	r3, r3, #7
60313b5c:	e2433002 	sub	r3, r3, #2
60313b60:	e3530002 	cmp	r3, #2
60313b64:	8a00001f 	bhi	60313be8 <cpu_suspend_save+0xc4>
60313b68:	ee404f10 	mcr	15, 2, r4, cr0, cr0, {0}
60313b6c:	ee307f10 	mrc	15, 1, r7, cr0, cr0, {0}
60313b70:	e7e9c1d7 	ubfx	ip, r7, #3, #10
60313b74:	e28ce001 	add	lr, ip, #1
60313b78:	e35c0000 	cmp	ip, #0
60313b7c:	0a00001e 	beq	60313bfc <cpu_suspend_save+0xd8>
60313b80:	e1a0200e 	mov	r2, lr
60313b84:	e3a03000 	mov	r3, #0
60313b88:	e2831001 	add	r1, r3, #1
60313b8c:	e1a020a2 	lsr	r2, r2, #1
60313b90:	e1a00003 	mov	r0, r3
60313b94:	e3520001 	cmp	r2, #1
60313b98:	e6ef3071 	uxtb	r3, r1
60313b9c:	1afffff9 	bne	60313b88 <cpu_suspend_save+0x64>
60313ba0:	e31e0001 	tst	lr, #1
60313ba4:	12803002 	addne	r3, r0, #2
60313ba8:	16ef3073 	uxtbne	r3, r3
60313bac:	e3530020 	cmp	r3, #32
60313bb0:	9263e020 	rsbls	lr, r3, #32
60313bb4:	8a00000b 	bhi	60313be8 <cpu_suspend_save+0xc4>
60313bb8:	e2070007 	and	r0, r7, #7
60313bbc:	e2800004 	add	r0, r0, #4
60313bc0:	e7ee76d7 	ubfx	r7, r7, #13, #15
60313bc4:	e1841e1c 	orr	r1, r4, ip, lsl lr
60313bc8:	e1a03007 	mov	r3, r7
60313bcc:	e1812013 	orr	r2, r1, r3, lsl r0
60313bd0:	ee072f5e 	mcr	15, 0, r2, cr7, cr14, {2}
60313bd4:	e2533001 	subs	r3, r3, #1
60313bd8:	2afffffb 	bcs	60313bcc <cpu_suspend_save+0xa8>
60313bdc:	e25cc001 	subs	ip, ip, #1
60313be0:	2afffff7 	bcs	60313bc4 <cpu_suspend_save+0xa0>
60313be4:	f57ff05f 	dmb	sy
60313be8:	e2855003 	add	r5, r5, #3
60313bec:	e2844002 	add	r4, r4, #2
60313bf0:	e3550015 	cmp	r5, #21
60313bf4:	1affffd6 	bne	60313b54 <cpu_suspend_save+0x30>
60313bf8:	e8bd81f0 	pop	{r4, r5, r6, r7, r8, pc}
60313bfc:	e3a0e020 	mov	lr, #32
60313c00:	eaffffec 	b	60313bb8 <cpu_suspend_save+0x94>

60313c04 <SOCPS_Save_GIC>:
60313c04:	e3a03a02 	mov	r3, #8192	; 0x2000
60313c08:	e34a3010 	movt	r3, #40976	; 0xa010
60313c0c:	e5932004 	ldr	r2, [r3, #4]
60313c10:	f57ff05f 	dmb	sy
60313c14:	e30cc8fc 	movw	ip, #51452	; 0xc8fc
60313c18:	e346c031 	movt	ip, #24625	; 0x6031
60313c1c:	e3a03a01 	mov	r3, #4096	; 0x1000
60313c20:	e34a3010 	movt	r3, #40976	; 0xa010
60313c24:	e58c2140 	str	r2, [ip, #320]	; 0x140
60313c28:	e5932100 	ldr	r2, [r3, #256]	; 0x100
60313c2c:	f57ff05f 	dmb	sy
60313c30:	e58c2000 	str	r2, [ip]
60313c34:	e5932104 	ldr	r2, [r3, #260]	; 0x104
60313c38:	f57ff05f 	dmb	sy
60313c3c:	e58c2004 	str	r2, [ip, #4]
60313c40:	e5932108 	ldr	r2, [r3, #264]	; 0x108
60313c44:	f57ff05f 	dmb	sy
60313c48:	e58c2008 	str	r2, [ip, #8]
60313c4c:	e593210c 	ldr	r2, [r3, #268]	; 0x10c
60313c50:	f57ff05f 	dmb	sy
60313c54:	e58c200c 	str	r2, [ip, #12]
60313c58:	e5932300 	ldr	r2, [r3, #768]	; 0x300
60313c5c:	f57ff05f 	dmb	sy
60313c60:	e58c2010 	str	r2, [ip, #16]
60313c64:	e5932304 	ldr	r2, [r3, #772]	; 0x304
60313c68:	f57ff05f 	dmb	sy
60313c6c:	e58c2014 	str	r2, [ip, #20]
60313c70:	e5932308 	ldr	r2, [r3, #776]	; 0x308
60313c74:	f57ff05f 	dmb	sy
60313c78:	e58c2018 	str	r2, [ip, #24]
60313c7c:	e593230c 	ldr	r2, [r3, #780]	; 0x30c
60313c80:	f57ff05f 	dmb	sy
60313c84:	e3011be0 	movw	r1, #7136	; 0x1be0
60313c88:	e34a1010 	movt	r1, #40976	; 0xa010
60313c8c:	e28c3020 	add	r3, ip, #32
60313c90:	e28c0040 	add	r0, ip, #64	; 0x40
60313c94:	e041100c 	sub	r1, r1, ip
60313c98:	e58c201c 	str	r2, [ip, #28]
60313c9c:	e7912003 	ldr	r2, [r1, r3]
60313ca0:	f57ff05f 	dmb	sy
60313ca4:	e4832004 	str	r2, [r3], #4
60313ca8:	e1530000 	cmp	r3, r0
60313cac:	1afffffa 	bne	60313c9c <SOCPS_Save_GIC+0x98>
60313cb0:	e59f304c 	ldr	r3, [pc, #76]	; 60313d04 <SOCPS_Save_GIC+0x100>
60313cb4:	e30117c4 	movw	r1, #6084	; 0x17c4
60313cb8:	e34a1010 	movt	r1, #40976	; 0xa010
60313cbc:	e2830080 	add	r0, r3, #128	; 0x80
60313cc0:	e041100c 	sub	r1, r1, ip
60313cc4:	e7912003 	ldr	r2, [r1, r3]
60313cc8:	f57ff05f 	dmb	sy
60313ccc:	e5a32004 	str	r2, [r3, #4]!
60313cd0:	e1530000 	cmp	r3, r0
60313cd4:	1afffffa 	bne	60313cc4 <SOCPS_Save_GIC+0xc0>
60313cd8:	e59f3028 	ldr	r3, [pc, #40]	; 60313d08 <SOCPS_Save_GIC+0x104>
60313cdc:	e3011344 	movw	r1, #4932	; 0x1344
60313ce0:	e34a1010 	movt	r1, #40976	; 0xa010
60313ce4:	e2830080 	add	r0, r3, #128	; 0x80
60313ce8:	e041100c 	sub	r1, r1, ip
60313cec:	e7912003 	ldr	r2, [r1, r3]
60313cf0:	f57ff05f 	dmb	sy
60313cf4:	e5a32004 	str	r2, [r3, #4]!
60313cf8:	e1530000 	cmp	r3, r0
60313cfc:	1afffffa 	bne	60313cec <SOCPS_Save_GIC+0xe8>
60313d00:	e12fff1e 	bx	lr
60313d04:	6031c938 	.word	0x6031c938
60313d08:	6031c9b8 	.word	0x6031c9b8

60313d0c <SOCPS_Restore_GIC>:
60313d0c:	e30c18fc 	movw	r1, #51452	; 0xc8fc
60313d10:	e3461031 	movt	r1, #24625	; 0x6031
60313d14:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
60313d18:	e5912140 	ldr	r2, [r1, #320]	; 0x140
60313d1c:	f57ff05f 	dmb	sy
60313d20:	e3a03a02 	mov	r3, #8192	; 0x2000
60313d24:	e34a3010 	movt	r3, #40976	; 0xa010
60313d28:	e5832004 	str	r2, [r3, #4]
60313d2c:	e5913000 	ldr	r3, [r1]
60313d30:	f57ff05f 	dmb	sy
60313d34:	e3a02a01 	mov	r2, #4096	; 0x1000
60313d38:	e34a2010 	movt	r2, #40976	; 0xa010
60313d3c:	e5823100 	str	r3, [r2, #256]	; 0x100
60313d40:	e5913004 	ldr	r3, [r1, #4]
60313d44:	f57ff05f 	dmb	sy
60313d48:	e5823104 	str	r3, [r2, #260]	; 0x104
60313d4c:	e5913008 	ldr	r3, [r1, #8]
60313d50:	f57ff05f 	dmb	sy
60313d54:	e5823108 	str	r3, [r2, #264]	; 0x108
60313d58:	e591300c 	ldr	r3, [r1, #12]
60313d5c:	f57ff05f 	dmb	sy
60313d60:	e582310c 	str	r3, [r2, #268]	; 0x10c
60313d64:	e5913010 	ldr	r3, [r1, #16]
60313d68:	f57ff05f 	dmb	sy
60313d6c:	e5823300 	str	r3, [r2, #768]	; 0x300
60313d70:	e5913014 	ldr	r3, [r1, #20]
60313d74:	f57ff05f 	dmb	sy
60313d78:	e5823304 	str	r3, [r2, #772]	; 0x304
60313d7c:	e5913018 	ldr	r3, [r1, #24]
60313d80:	f57ff05f 	dmb	sy
60313d84:	e5823308 	str	r3, [r2, #776]	; 0x308
60313d88:	e591c01c 	ldr	ip, [r1, #28]
60313d8c:	f57ff05f 	dmb	sy
60313d90:	e3010bdc 	movw	r0, #7132	; 0x1bdc
60313d94:	e34a0010 	movt	r0, #40976	; 0xa010
60313d98:	e582c30c 	str	ip, [r2, #780]	; 0x30c
60313d9c:	e2813020 	add	r3, r1, #32
60313da0:	e0400001 	sub	r0, r0, r1
60313da4:	e281c040 	add	ip, r1, #64	; 0x40
60313da8:	e4932004 	ldr	r2, [r3], #4
60313dac:	f57ff05f 	dmb	sy
60313db0:	e153000c 	cmp	r3, ip
60313db4:	e7802003 	str	r2, [r0, r3]
60313db8:	1afffffa 	bne	60313da8 <SOCPS_Restore_GIC+0x9c>
60313dbc:	e59f2094 	ldr	r2, [pc, #148]	; 60313e58 <SOCPS_Restore_GIC+0x14c>
60313dc0:	e301c7c4 	movw	ip, #6084	; 0x17c4
60313dc4:	e34ac010 	movt	ip, #40976	; 0xa010
60313dc8:	e282e080 	add	lr, r2, #128	; 0x80
60313dcc:	e1a03002 	mov	r3, r2
60313dd0:	e5b20004 	ldr	r0, [r2, #4]!
60313dd4:	f57ff05f 	dmb	sy
60313dd8:	e083300c 	add	r3, r3, ip
60313ddc:	e0433001 	sub	r3, r3, r1
60313de0:	e152000e 	cmp	r2, lr
60313de4:	e5830000 	str	r0, [r3]
60313de8:	1afffff7 	bne	60313dcc <SOCPS_Restore_GIC+0xc0>
60313dec:	e59f2068 	ldr	r2, [pc, #104]	; 60313e5c <SOCPS_Restore_GIC+0x150>
60313df0:	e301c344 	movw	ip, #4932	; 0x1344
60313df4:	e34ac010 	movt	ip, #40976	; 0xa010
60313df8:	e282e080 	add	lr, r2, #128	; 0x80
60313dfc:	e1a03002 	mov	r3, r2
60313e00:	e5b20004 	ldr	r0, [r2, #4]!
60313e04:	f57ff05f 	dmb	sy
60313e08:	e083300c 	add	r3, r3, ip
60313e0c:	e0433001 	sub	r3, r3, r1
60313e10:	e152000e 	cmp	r2, lr
60313e14:	e5830000 	str	r0, [r3]
60313e18:	1afffff7 	bne	60313dfc <SOCPS_Restore_GIC+0xf0>
60313e1c:	e3a02a02 	mov	r2, #8192	; 0x2000
60313e20:	e34a2010 	movt	r2, #40976	; 0xa010
60313e24:	e5923000 	ldr	r3, [r2]
60313e28:	f57ff05f 	dmb	sy
60313e2c:	e3c33e1e 	bic	r3, r3, #480	; 0x1e0
60313e30:	e3c33003 	bic	r3, r3, #3
60313e34:	e3833007 	orr	r3, r3, #7
60313e38:	f57ff05f 	dmb	sy
60313e3c:	e5823000 	str	r3, [r2]
60313e40:	f57ff05f 	dmb	sy
60313e44:	e3a03a01 	mov	r3, #4096	; 0x1000
60313e48:	e34a3010 	movt	r3, #40976	; 0xa010
60313e4c:	e3a02003 	mov	r2, #3
60313e50:	e5832000 	str	r2, [r3]
60313e54:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)
60313e58:	6031c938 	.word	0x6031c938
60313e5c:	6031c9b8 	.word	0x6031c9b8

60313e60 <SOCPS_SleepPG_LIB>:
60313e60:	e92d4010 	push	{r4, lr}
60313e64:	ebffff66 	bl	60313c04 <SOCPS_Save_GIC>
60313e68:	e3a01000 	mov	r1, #0
60313e6c:	e3a00020 	mov	r0, #32
60313e70:	e3072fc0 	movw	r2, #32704	; 0x7fc0
60313e74:	e3462031 	movt	r2, #24625	; 0x6031
60313e78:	ebffd2ef 	bl	60308a3c <ipc_send_message>
60313e7c:	e3a00000 	mov	r0, #0
60313e80:	e3031ad4 	movw	r1, #15060	; 0x3ad4
60313e84:	e3461031 	movt	r1, #24625	; 0x6031
60313e88:	ebffb138 	bl	60300370 <cpu_suspend>
60313e8c:	e8bd4010 	pop	{r4, lr}
60313e90:	eaffff9d 	b	60313d0c <SOCPS_Restore_GIC>

60313e94 <SOCPS_SleepCG_LIB>:
60313e94:	e3072fc0 	movw	r2, #32704	; 0x7fc0
60313e98:	e3462031 	movt	r2, #24625	; 0x6031
60313e9c:	e92d4010 	push	{r4, lr}
60313ea0:	e3a03001 	mov	r3, #1
60313ea4:	e5823000 	str	r3, [r2]
60313ea8:	f57ff06f 	isb	sy
60313eac:	e3a01000 	mov	r1, #0
60313eb0:	e3a00020 	mov	r0, #32
60313eb4:	ebffd2e0 	bl	60308a3c <ipc_send_message>
60313eb8:	e3a01441 	mov	r1, #1090519040	; 0x41000000
60313ebc:	e30c3b2c 	movw	r3, #52012	; 0xcb2c
60313ec0:	e3463030 	movt	r3, #24624	; 0x6030
60313ec4:	e591200c 	ldr	r2, [r1, #12]
60313ec8:	e3a00002 	mov	r0, #2
60313ecc:	e3c22001 	bic	r2, r2, #1
60313ed0:	e581200c 	str	r2, [r1, #12]
60313ed4:	e8bd4010 	pop	{r4, lr}
60313ed8:	e12fff13 	bx	r3

60313edc <BKUP_Set>:
60313edc:	e3500003 	cmp	r0, #3
60313ee0:	8a000006 	bhi	60313f00 <BKUP_Set+0x24>
60313ee4:	e1a00100 	lsl	r0, r0, #2
60313ee8:	e2800442 	add	r0, r0, #1107296256	; 0x42000000
60313eec:	e2800902 	add	r0, r0, #32768	; 0x8000
60313ef0:	e59030e0 	ldr	r3, [r0, #224]	; 0xe0
60313ef4:	e1831001 	orr	r1, r3, r1
60313ef8:	e58010e0 	str	r1, [r0, #224]	; 0xe0
60313efc:	e12fff1e 	bx	lr
60313f00:	e3060c6c 	movw	r0, #27756	; 0x6c6c
60313f04:	e3460031 	movt	r0, #24625	; 0x6031
60313f08:	e3a01033 	mov	r1, #51	; 0x33
60313f0c:	e92d4010 	push	{r4, lr}
60313f10:	ebfffe9d 	bl	6031398c <io_assert_failed>

60313f14 <BKUP_Clear>:
60313f14:	e3500003 	cmp	r0, #3
60313f18:	8a000006 	bhi	60313f38 <BKUP_Clear+0x24>
60313f1c:	e1a00100 	lsl	r0, r0, #2
60313f20:	e2800442 	add	r0, r0, #1107296256	; 0x42000000
60313f24:	e2800902 	add	r0, r0, #32768	; 0x8000
60313f28:	e59030e0 	ldr	r3, [r0, #224]	; 0xe0
60313f2c:	e1c31001 	bic	r1, r3, r1
60313f30:	e58010e0 	str	r1, [r0, #224]	; 0xe0
60313f34:	e12fff1e 	bx	lr
60313f38:	e3060c6c 	movw	r0, #27756	; 0x6c6c
60313f3c:	e3460031 	movt	r0, #24625	; 0x6031
60313f40:	e3a01043 	mov	r1, #67	; 0x43
60313f44:	e92d4010 	push	{r4, lr}
60313f48:	ebfffe8f 	bl	6031398c <io_assert_failed>

60313f4c <GPIO_INTMode>:
60313f4c:	e3510001 	cmp	r1, #1
60313f50:	e307c2b8 	movw	ip, #29368	; 0x72b8
60313f54:	e346c031 	movt	ip, #24625	; 0x6031
60313f58:	e7e112d0 	ubfx	r1, r0, #5, #2
60313f5c:	e92d4010 	push	{r4, lr}
60313f60:	e200001f 	and	r0, r0, #31
60313f64:	e79c1101 	ldr	r1, [ip, r1, lsl #2]
60313f68:	e3a0c001 	mov	ip, #1
60313f6c:	e1a0c01c 	lsl	ip, ip, r0
60313f70:	e1e0e00c 	mvn	lr, ip
60313f74:	1591c030 	ldrne	ip, [r1, #48]	; 0x30
60313f78:	100cc00e 	andne	ip, ip, lr
60313f7c:	0a000001 	beq	60313f88 <GPIO_INTMode+0x3c>
60313f80:	e581c030 	str	ip, [r1, #48]	; 0x30
60313f84:	e8bd8010 	pop	{r4, pc}
60313f88:	e3520002 	cmp	r2, #2
60313f8c:	0a000014 	beq	60313fe4 <GPIO_INTMode+0x98>
60313f90:	e5914068 	ldr	r4, [r1, #104]	; 0x68
60313f94:	e004400e 	and	r4, r4, lr
60313f98:	e5814068 	str	r4, [r1, #104]	; 0x68
60313f9c:	e5914038 	ldr	r4, [r1, #56]	; 0x38
60313fa0:	e004400e 	and	r4, r4, lr
60313fa4:	e1842012 	orr	r2, r4, r2, lsl r0
60313fa8:	e5812038 	str	r2, [r1, #56]	; 0x38
60313fac:	e591203c 	ldr	r2, [r1, #60]	; 0x3c
60313fb0:	e002200e 	and	r2, r2, lr
60313fb4:	e1823013 	orr	r3, r2, r3, lsl r0
60313fb8:	e581303c 	str	r3, [r1, #60]	; 0x3c
60313fbc:	e5913048 	ldr	r3, [r1, #72]	; 0x48
60313fc0:	e59d2008 	ldr	r2, [sp, #8]
60313fc4:	e00ee003 	and	lr, lr, r3
60313fc8:	e3520001 	cmp	r2, #1
60313fcc:	018ee00c 	orreq	lr, lr, ip
60313fd0:	e581e048 	str	lr, [r1, #72]	; 0x48
60313fd4:	e5913030 	ldr	r3, [r1, #48]	; 0x30
60313fd8:	e183c00c 	orr	ip, r3, ip
60313fdc:	e581c030 	str	ip, [r1, #48]	; 0x30
60313fe0:	e8bd8010 	pop	{r4, pc}
60313fe4:	e5913068 	ldr	r3, [r1, #104]	; 0x68
60313fe8:	e183300c 	orr	r3, r3, ip
60313fec:	e5813068 	str	r3, [r1, #104]	; 0x68
60313ff0:	eafffff1 	b	60313fbc <GPIO_INTMode+0x70>

60313ff4 <GPIO_INTConfig>:
60313ff4:	e7e1c2d0 	ubfx	ip, r0, #5, #2
60313ff8:	e30722b8 	movw	r2, #29368	; 0x72b8
60313ffc:	e3462031 	movt	r2, #24625	; 0x6031
60314000:	e3a03001 	mov	r3, #1
60314004:	e200001f 	and	r0, r0, #31
60314008:	e792210c 	ldr	r2, [r2, ip, lsl #2]
6031400c:	e1a00013 	lsl	r0, r3, r0
60314010:	e3510000 	cmp	r1, #0
60314014:	e582004c 	str	r0, [r2, #76]	; 0x4c
60314018:	e5923034 	ldr	r3, [r2, #52]	; 0x34
6031401c:	01800003 	orreq	r0, r0, r3
60314020:	11c30000 	bicne	r0, r3, r0
60314024:	e5820034 	str	r0, [r2, #52]	; 0x34
60314028:	e12fff1e 	bx	lr

6031402c <GPIO_INTHandler>:
6031402c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
60314030:	e3a03a0d 	mov	r3, #53248	; 0xd000
60314034:	e3443200 	movt	r3, #16896	; 0x4200
60314038:	e5906050 	ldr	r6, [r0, #80]	; 0x50
6031403c:	e5905040 	ldr	r5, [r0, #64]	; 0x40
60314040:	e1500003 	cmp	r0, r3
60314044:	e580504c 	str	r5, [r0, #76]	; 0x4c
60314048:	0a000037 	beq	6031412c <GPIO_INTHandler+0x100>
6031404c:	e3a03b35 	mov	r3, #54272	; 0xd400
60314050:	e3443200 	movt	r3, #16896	; 0x4200
60314054:	e1500003 	cmp	r0, r3
60314058:	0a000019 	beq	603140c4 <GPIO_INTHandler+0x98>
6031405c:	e3a09002 	mov	r9, #2
60314060:	e3409040 	movt	r9, #64	; 0x40
60314064:	e30c7d34 	movw	r7, #52532	; 0xcd34
60314068:	e3467031 	movt	r7, #24625	; 0x6031
6031406c:	e30c8cb4 	movw	r8, #52404	; 0xccb4
60314070:	e3468031 	movt	r8, #24625	; 0x6031
60314074:	e3a04000 	mov	r4, #0
60314078:	e1a03435 	lsr	r3, r5, r4
6031407c:	e3130001 	tst	r3, #1
60314080:	0a00000a 	beq	603140b0 <GPIO_INTHandler+0x84>
60314084:	e7973104 	ldr	r3, [r7, r4, lsl #2]
60314088:	e1a00436 	lsr	r0, r6, r4
6031408c:	e3530000 	cmp	r3, #0
60314090:	0a000006 	beq	603140b0 <GPIO_INTHandler+0x84>
60314094:	e3100001 	tst	r0, #1
60314098:	e1a02804 	lsl	r2, r4, #16
6031409c:	e1821009 	orr	r1, r2, r9
603140a0:	13821501 	orrne	r1, r2, #4194304	; 0x400000
603140a4:	e7980104 	ldr	r0, [r8, r4, lsl #2]
603140a8:	13811001 	orrne	r1, r1, #1
603140ac:	e12fff33 	blx	r3
603140b0:	e2844001 	add	r4, r4, #1
603140b4:	e3540020 	cmp	r4, #32
603140b8:	1affffee 	bne	60314078 <GPIO_INTHandler+0x4c>
603140bc:	e3a00000 	mov	r0, #0
603140c0:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
603140c4:	e3a09002 	mov	r9, #2
603140c8:	e3409020 	movt	r9, #32
603140cc:	e30c7c34 	movw	r7, #52276	; 0xcc34
603140d0:	e3467031 	movt	r7, #24625	; 0x6031
603140d4:	e30c8bb4 	movw	r8, #52148	; 0xcbb4
603140d8:	e3468031 	movt	r8, #24625	; 0x6031
603140dc:	e3a04000 	mov	r4, #0
603140e0:	e1a03435 	lsr	r3, r5, r4
603140e4:	e3130001 	tst	r3, #1
603140e8:	0a00000a 	beq	60314118 <GPIO_INTHandler+0xec>
603140ec:	e7973104 	ldr	r3, [r7, r4, lsl #2]
603140f0:	e1a00436 	lsr	r0, r6, r4
603140f4:	e3530000 	cmp	r3, #0
603140f8:	0a000006 	beq	60314118 <GPIO_INTHandler+0xec>
603140fc:	e3100001 	tst	r0, #1
60314100:	e1a02804 	lsl	r2, r4, #16
60314104:	e1821009 	orr	r1, r2, r9
60314108:	13821602 	orrne	r1, r2, #2097152	; 0x200000
6031410c:	e7980104 	ldr	r0, [r8, r4, lsl #2]
60314110:	13811001 	orrne	r1, r1, #1
60314114:	e12fff33 	blx	r3
60314118:	e2844001 	add	r4, r4, #1
6031411c:	e3540020 	cmp	r4, #32
60314120:	1affffee 	bne	603140e0 <GPIO_INTHandler+0xb4>
60314124:	e3a00000 	mov	r0, #0
60314128:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
6031412c:	e30c7b34 	movw	r7, #52020	; 0xcb34
60314130:	e3467031 	movt	r7, #24625	; 0x6031
60314134:	e30c8ab4 	movw	r8, #51892	; 0xcab4
60314138:	e3468031 	movt	r8, #24625	; 0x6031
6031413c:	e3a04000 	mov	r4, #0
60314140:	e1a03435 	lsr	r3, r5, r4
60314144:	e3130001 	tst	r3, #1
60314148:	0a000009 	beq	60314174 <GPIO_INTHandler+0x148>
6031414c:	e7973104 	ldr	r3, [r7, r4, lsl #2]
60314150:	e1a00436 	lsr	r0, r6, r4
60314154:	e3530000 	cmp	r3, #0
60314158:	0a000005 	beq	60314174 <GPIO_INTHandler+0x148>
6031415c:	e1a02804 	lsl	r2, r4, #16
60314160:	e3100001 	tst	r0, #1
60314164:	e3821002 	orr	r1, r2, #2
60314168:	e7980104 	ldr	r0, [r8, r4, lsl #2]
6031416c:	13821001 	orrne	r1, r2, #1
60314170:	e12fff33 	blx	r3
60314174:	e2844001 	add	r4, r4, #1
60314178:	e3540020 	cmp	r4, #32
6031417c:	1affffef 	bne	60314140 <GPIO_INTHandler+0x114>
60314180:	e3a00000 	mov	r0, #0
60314184:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}

60314188 <GPIO_Direction>:
60314188:	e7e1c2d0 	ubfx	ip, r0, #5, #2
6031418c:	e30722b8 	movw	r2, #29368	; 0x72b8
60314190:	e3462031 	movt	r2, #24625	; 0x6031
60314194:	e3a03001 	mov	r3, #1
60314198:	e200001f 	and	r0, r0, #31
6031419c:	e792210c 	ldr	r2, [r2, ip, lsl #2]
603141a0:	e1a00013 	lsl	r0, r3, r0
603141a4:	e3510000 	cmp	r1, #0
603141a8:	e5923004 	ldr	r3, [r2, #4]
603141ac:	01c30000 	biceq	r0, r3, r0
603141b0:	11830000 	orrne	r0, r3, r0
603141b4:	e5820004 	str	r0, [r2, #4]
603141b8:	e12fff1e 	bx	lr

603141bc <GPIO_UserRegIrq>:
603141bc:	e7e132d0 	ubfx	r3, r0, #5, #2
603141c0:	e3530000 	cmp	r3, #0
603141c4:	e200001f 	and	r0, r0, #31
603141c8:	1a000006 	bne	603141e8 <GPIO_UserRegIrq+0x2c>
603141cc:	e30ccb34 	movw	ip, #52020	; 0xcb34
603141d0:	e346c031 	movt	ip, #24625	; 0x6031
603141d4:	e30c3ab4 	movw	r3, #51892	; 0xcab4
603141d8:	e3463031 	movt	r3, #24625	; 0x6031
603141dc:	e78c1100 	str	r1, [ip, r0, lsl #2]
603141e0:	e7832100 	str	r2, [r3, r0, lsl #2]
603141e4:	e12fff1e 	bx	lr
603141e8:	e3530001 	cmp	r3, #1
603141ec:	030ccc34 	movweq	ip, #52276	; 0xcc34
603141f0:	030c3bb4 	movweq	r3, #52148	; 0xcbb4
603141f4:	130ccd34 	movwne	ip, #52532	; 0xcd34
603141f8:	130c3cb4 	movwne	r3, #52404	; 0xccb4
603141fc:	0346c031 	movteq	ip, #24625	; 0x6031
60314200:	03463031 	movteq	r3, #24625	; 0x6031
60314204:	1346c031 	movtne	ip, #24625	; 0x6031
60314208:	13463031 	movtne	r3, #24625	; 0x6031
6031420c:	e78c1100 	str	r1, [ip, r0, lsl #2]
60314210:	e7832100 	str	r2, [r3, r0, lsl #2]
60314214:	e12fff1e 	bx	lr

60314218 <LOGUART_PutChar>:
60314218:	e30c3f28 	movw	r3, #53032	; 0xcf28
6031421c:	e3463031 	movt	r3, #24625	; 0x6031
60314220:	e92d4070 	push	{r4, r5, r6, lr}
60314224:	e3a02902 	mov	r2, #32768	; 0x8000
60314228:	e3442200 	movt	r2, #16896	; 0x4200
6031422c:	e5933000 	ldr	r3, [r3]
60314230:	e592527c 	ldr	r5, [r2, #636]	; 0x27c
60314234:	e3530001 	cmp	r3, #1
60314238:	e7e15355 	ubfx	r5, r5, #6, #2
6031423c:	08bd8070 	popeq	{r4, r5, r6, pc}
60314240:	e3a0c014 	mov	ip, #20
60314244:	e3066ce4 	movw	r6, #27876	; 0x6ce4
60314248:	e3466031 	movt	r6, #24625	; 0x6031
6031424c:	e3a0e903 	mov	lr, #49152	; 0xc000
60314250:	e344e200 	movt	lr, #16896	; 0x4200
60314254:	e02c659c 	mla	ip, ip, r5, r6
60314258:	e3a03000 	mov	r3, #0
6031425c:	e59c400c 	ldr	r4, [ip, #12]
60314260:	ea000003 	b	60314274 <LOGUART_PutChar+0x5c>
60314264:	e59e1014 	ldr	r1, [lr, #20]
60314268:	e59c2004 	ldr	r2, [ip, #4]
6031426c:	e1110002 	tst	r1, r2
60314270:	1a000002 	bne	60314280 <LOGUART_PutChar+0x68>
60314274:	e2833001 	add	r3, r3, #1
60314278:	e1540003 	cmp	r4, r3
6031427c:	8afffff8 	bhi	60314264 <LOGUART_PutChar+0x4c>
60314280:	e3a03014 	mov	r3, #20
60314284:	e350000a 	cmp	r0, #10
60314288:	03a0200d 	moveq	r2, #13
6031428c:	e0050593 	mul	r5, r3, r5
60314290:	e7963005 	ldr	r3, [r6, r5]
60314294:	e1a03103 	lsl	r3, r3, #2
60314298:	e2833442 	add	r3, r3, #1107296256	; 0x42000000
6031429c:	e2833903 	add	r3, r3, #49152	; 0xc000
603142a0:	e583005c 	str	r0, [r3, #92]	; 0x5c
603142a4:	0583205c 	streq	r2, [r3, #92]	; 0x5c
603142a8:	e8bd8070 	pop	{r4, r5, r6, pc}

603142ac <PAD_PullCtrl>:
603142ac:	e3a03c8a 	mov	r3, #35328	; 0x8a00
603142b0:	e3443200 	movt	r3, #16896	; 0x4200
603142b4:	e3510001 	cmp	r1, #1
603142b8:	e7933100 	ldr	r3, [r3, r0, lsl #2]
603142bc:	e3c33c03 	bic	r3, r3, #768	; 0x300
603142c0:	03833c02 	orreq	r3, r3, #512	; 0x200
603142c4:	0a000001 	beq	603142d0 <PAD_PullCtrl+0x24>
603142c8:	e3510002 	cmp	r1, #2
603142cc:	03833c01 	orreq	r3, r3, #256	; 0x100
603142d0:	e3a02c8a 	mov	r2, #35328	; 0x8a00
603142d4:	e3442200 	movt	r2, #16896	; 0x4200
603142d8:	e7823100 	str	r3, [r2, r0, lsl #2]
603142dc:	e12fff1e 	bx	lr

603142e0 <shell_array_init>:
603142e0:	e3510000 	cmp	r1, #0
603142e4:	012fff1e 	bxeq	lr
603142e8:	e2411001 	sub	r1, r1, #1
603142ec:	e2403001 	sub	r3, r0, #1
603142f0:	e6e00071 	uxtab	r0, r0, r1
603142f4:	e5e32001 	strb	r2, [r3, #1]!
603142f8:	e1530000 	cmp	r3, r0
603142fc:	1afffffc 	bne	603142f4 <shell_array_init+0x14>
60314300:	e12fff1e 	bx	lr

60314304 <shell_get_argc>:
60314304:	e1a03000 	mov	r3, r0
60314308:	e5d00000 	ldrb	r0, [r0]
6031430c:	e3500000 	cmp	r0, #0
60314310:	012fff1e 	bxeq	lr
60314314:	e3a02000 	mov	r2, #0
60314318:	ea000001 	b	60314324 <shell_get_argc+0x20>
6031431c:	e3500000 	cmp	r0, #0
60314320:	0a00000d 	beq	6031435c <shell_get_argc+0x58>
60314324:	e3500020 	cmp	r0, #32
60314328:	05d30001 	ldrbeq	r0, [r3, #1]
6031432c:	02833001 	addeq	r3, r3, #1
60314330:	0afffff9 	beq	6031431c <shell_get_argc+0x18>
60314334:	e5d30000 	ldrb	r0, [r3]
60314338:	e2822001 	add	r2, r2, #1
6031433c:	e31000df 	tst	r0, #223	; 0xdf
60314340:	e6ff2072 	uxth	r2, r2
60314344:	0afffff4 	beq	6031431c <shell_get_argc+0x18>
60314348:	e5f30001 	ldrb	r0, [r3, #1]!
6031434c:	e31000df 	tst	r0, #223	; 0xdf
60314350:	1afffffc 	bne	60314348 <shell_get_argc+0x44>
60314354:	e3500000 	cmp	r0, #0
60314358:	1afffff1 	bne	60314324 <shell_get_argc+0x20>
6031435c:	e352000f 	cmp	r2, #15
60314360:	31a00002 	movcc	r0, r2
60314364:	23a0000f 	movcs	r0, #15
60314368:	e6ef0070 	uxtb	r0, r0
6031436c:	e12fff1e 	bx	lr

60314370 <shell_get_argv>:
60314370:	e59f3080 	ldr	r3, [pc, #128]	; 603143f8 <shell_get_argv+0x88>
60314374:	e3a01000 	mov	r1, #0
60314378:	e2832001 	add	r2, r3, #1
6031437c:	e283c040 	add	ip, r3, #64	; 0x40
60314380:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
60314384:	e5e31001 	strb	r1, [r3, #1]!
60314388:	e153000c 	cmp	r3, ip
6031438c:	1afffffc 	bne	60314384 <shell_get_argv+0x14>
60314390:	e59fe064 	ldr	lr, [pc, #100]	; 603143fc <shell_get_argv+0x8c>
60314394:	e3a0c000 	mov	ip, #0
60314398:	e5d03000 	ldrb	r3, [r0]
6031439c:	e3530000 	cmp	r3, #0
603143a0:	0a000011 	beq	603143ec <shell_get_argv+0x7c>
603143a4:	e4820004 	str	r0, [r2], #4
603143a8:	e5d03000 	ldrb	r3, [r0]
603143ac:	e31300df 	tst	r3, #223	; 0xdf
603143b0:	0a000002 	beq	603143c0 <shell_get_argv+0x50>
603143b4:	e5f03001 	ldrb	r3, [r0, #1]!
603143b8:	e31300df 	tst	r3, #223	; 0xdf
603143bc:	1afffffc 	bne	603143b4 <shell_get_argv+0x44>
603143c0:	e1a03000 	mov	r3, r0
603143c4:	e4c3c001 	strb	ip, [r3], #1
603143c8:	e5d01001 	ldrb	r1, [r0, #1]
603143cc:	e1a00003 	mov	r0, r3
603143d0:	e3510020 	cmp	r1, #32
603143d4:	1a000002 	bne	603143e4 <shell_get_argv+0x74>
603143d8:	e5f03001 	ldrb	r3, [r0, #1]!
603143dc:	e3530020 	cmp	r3, #32
603143e0:	0afffffc 	beq	603143d8 <shell_get_argv+0x68>
603143e4:	e15e0002 	cmp	lr, r2
603143e8:	1affffea 	bne	60314398 <shell_get_argv+0x28>
603143ec:	e30c0db4 	movw	r0, #52660	; 0xcdb4
603143f0:	e3460031 	movt	r0, #24625	; 0x6031
603143f4:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)
603143f8:	6031cdb3 	.word	0x6031cdb3
603143fc:	6031cdf4 	.word	0x6031cdf4

60314400 <shell_init_rom>:
60314400:	e92d4030 	push	{r4, r5, lr}
60314404:	e30cedf4 	movw	lr, #52724	; 0xcdf4
60314408:	e346e031 	movt	lr, #24625	; 0x6031
6031440c:	e24dd00c 	sub	sp, sp, #12
60314410:	e1a05000 	mov	r5, r0
60314414:	e1a0300e 	mov	r3, lr
60314418:	e28ec07f 	add	ip, lr, #127	; 0x7f
6031441c:	e3a02000 	mov	r2, #0
60314420:	e5ce2000 	strb	r2, [lr]
60314424:	e5e32001 	strb	r2, [r3, #1]!
60314428:	e153000c 	cmp	r3, ip
6031442c:	1afffffc 	bne	60314424 <shell_init_rom+0x24>
60314430:	e30c3e98 	movw	r3, #52888	; 0xce98
60314434:	e3463031 	movt	r3, #24625	; 0x6031
60314438:	e283c07f 	add	ip, r3, #127	; 0x7f
6031443c:	e5c32000 	strb	r2, [r3]
60314440:	e3a02000 	mov	r2, #0
60314444:	e5e32001 	strb	r2, [r3, #1]!
60314448:	e153000c 	cmp	r3, ip
6031444c:	1afffffc 	bne	60314444 <shell_init_rom+0x44>
60314450:	e30c4e74 	movw	r4, #52852	; 0xce74
60314454:	e3464031 	movt	r4, #24625	; 0x6031
60314458:	e3033934 	movw	r3, #14644	; 0x3934
6031445c:	e3463031 	movt	r3, #24625	; 0x6031
60314460:	e5c42000 	strb	r2, [r4]
60314464:	e3550000 	cmp	r5, #0
60314468:	e5c42001 	strb	r2, [r4, #1]
6031446c:	e5c42002 	strb	r2, [r4, #2]
60314470:	e5c42003 	strb	r2, [r4, #3]
60314474:	e5c42006 	strb	r2, [r4, #6]
60314478:	e584e008 	str	lr, [r4, #8]
6031447c:	e584300c 	str	r3, [r4, #12]
60314480:	0a000007 	beq	603144a4 <shell_init_rom+0xa4>
60314484:	e5841010 	str	r1, [r4, #16]
60314488:	e5845014 	str	r5, [r4, #20]
6031448c:	e3a03000 	mov	r3, #0
60314490:	e5843020 	str	r3, [r4, #32]
60314494:	e5c43004 	strb	r3, [r4, #4]
60314498:	e5c43005 	strb	r3, [r4, #5]
6031449c:	e28dd00c 	add	sp, sp, #12
603144a0:	e8bd8030 	pop	{r4, r5, pc}
603144a4:	e28d0004 	add	r0, sp, #4
603144a8:	e30834c8 	movw	r3, #33992	; 0x84c8
603144ac:	e3463030 	movt	r3, #24624	; 0x6030
603144b0:	e58d5004 	str	r5, [sp, #4]
603144b4:	e12fff33 	blx	r3
603144b8:	e59d3004 	ldr	r3, [sp, #4]
603144bc:	e5843010 	str	r3, [r4, #16]
603144c0:	e5840014 	str	r0, [r4, #20]
603144c4:	eafffff0 	b	6031448c <shell_init_rom+0x8c>

603144c8 <SYSCFG_CHIPType_Get>:
603144c8:	e3a03902 	mov	r3, #32768	; 0x8000
603144cc:	e3443200 	movt	r3, #16896	; 0x4200
603144d0:	e593027c 	ldr	r0, [r3, #636]	; 0x27c
603144d4:	e7e10450 	ubfx	r0, r0, #8, #2
603144d8:	e12fff1e 	bx	lr

603144dc <SYSCFG_OTP_RSIPEn>:
603144dc:	e3a03442 	mov	r3, #1107296256	; 0x42000000
603144e0:	e5932100 	ldr	r2, [r3, #256]	; 0x100
603144e4:	e5930368 	ldr	r0, [r3, #872]	; 0x368
603144e8:	e3120302 	tst	r2, #134217728	; 0x8000000
603144ec:	02200004 	eoreq	r0, r0, #4
603144f0:	07e00150 	ubfxeq	r0, r0, #2, #1
603144f4:	13a00001 	movne	r0, #1
603144f8:	e12fff1e 	bx	lr

603144fc <SYSTIMER_TickGet>:
603144fc:	e3a00a0b 	mov	r0, #45056	; 0xb000
60314500:	e3440200 	movt	r0, #16896	; 0x4200
60314504:	e3043540 	movw	r3, #17728	; 0x4540
60314508:	e3463031 	movt	r3, #24625	; 0x6031
6031450c:	e12fff13 	bx	r3

60314510 <SYSTIMER_GetPassTick>:
60314510:	e92d4010 	push	{r4, lr}
60314514:	e3043540 	movw	r3, #17728	; 0x4540
60314518:	e3463031 	movt	r3, #24625	; 0x6031
6031451c:	e1a04000 	mov	r4, r0
60314520:	e3a00a0b 	mov	r0, #45056	; 0xb000
60314524:	e3440200 	movt	r0, #16896	; 0x4200
60314528:	e12fff33 	blx	r3
6031452c:	e1500004 	cmp	r0, r4
60314530:	30444000 	subcc	r4, r4, r0
60314534:	20400004 	subcs	r0, r0, r4
60314538:	31e00004 	mvncc	r0, r4
6031453c:	e8bd8010 	pop	{r4, pc}

60314540 <RTIM_GetCount>:
60314540:	e3a01a0b 	mov	r1, #45056	; 0xb000
60314544:	e3441200 	movt	r1, #16896	; 0x4200
60314548:	e3a02cb2 	mov	r2, #45568	; 0xb200
6031454c:	e3442200 	movt	r2, #16896	; 0x4200
60314550:	e1a03000 	mov	r3, r0
60314554:	e1500001 	cmp	r0, r1
60314558:	11500002 	cmpne	r0, r2
6031455c:	0a00005f 	beq	603146e0 <RTIM_GetCount+0x1a0>
60314560:	e3a01b2d 	mov	r1, #46080	; 0xb400
60314564:	e3441200 	movt	r1, #16896	; 0x4200
60314568:	e3a02cb6 	mov	r2, #46592	; 0xb600
6031456c:	e3442200 	movt	r2, #16896	; 0x4200
60314570:	e1500001 	cmp	r0, r1
60314574:	11500002 	cmpne	r0, r2
60314578:	0a000058 	beq	603146e0 <RTIM_GetCount+0x1a0>
6031457c:	e3a01b2e 	mov	r1, #47104	; 0xb800
60314580:	e3441200 	movt	r1, #16896	; 0x4200
60314584:	e3a02cba 	mov	r2, #47616	; 0xba00
60314588:	e3442200 	movt	r2, #16896	; 0x4200
6031458c:	e1500001 	cmp	r0, r1
60314590:	11500002 	cmpne	r0, r2
60314594:	0a000051 	beq	603146e0 <RTIM_GetCount+0x1a0>
60314598:	e3a01b2f 	mov	r1, #48128	; 0xbc00
6031459c:	e3441200 	movt	r1, #16896	; 0x4200
603145a0:	e3a02cbe 	mov	r2, #48640	; 0xbe00
603145a4:	e3442200 	movt	r2, #16896	; 0x4200
603145a8:	e1500001 	cmp	r0, r1
603145ac:	11500002 	cmpne	r0, r2
603145b0:	0a00004a 	beq	603146e0 <RTIM_GetCount+0x1a0>
603145b4:	e3a01a0a 	mov	r1, #40960	; 0xa000
603145b8:	e3441100 	movt	r1, #16640	; 0x4100
603145bc:	e3a02ca2 	mov	r2, #41472	; 0xa200
603145c0:	e3442100 	movt	r2, #16640	; 0x4100
603145c4:	e1500001 	cmp	r0, r1
603145c8:	11500002 	cmpne	r0, r2
603145cc:	0a000043 	beq	603146e0 <RTIM_GetCount+0x1a0>
603145d0:	e3a01b29 	mov	r1, #41984	; 0xa400
603145d4:	e3441100 	movt	r1, #16640	; 0x4100
603145d8:	e3a02ca6 	mov	r2, #42496	; 0xa600
603145dc:	e3442100 	movt	r2, #16640	; 0x4100
603145e0:	e1500001 	cmp	r0, r1
603145e4:	11500002 	cmpne	r0, r2
603145e8:	0a00003c 	beq	603146e0 <RTIM_GetCount+0x1a0>
603145ec:	e3a01b2a 	mov	r1, #43008	; 0xa800
603145f0:	e3441100 	movt	r1, #16640	; 0x4100
603145f4:	e3a02caa 	mov	r2, #43520	; 0xaa00
603145f8:	e3442100 	movt	r2, #16640	; 0x4100
603145fc:	e1500001 	cmp	r0, r1
60314600:	11500002 	cmpne	r0, r2
60314604:	0a000035 	beq	603146e0 <RTIM_GetCount+0x1a0>
60314608:	e3a01a0b 	mov	r1, #45056	; 0xb000
6031460c:	e3451200 	movt	r1, #20992	; 0x5200
60314610:	e3a02cb2 	mov	r2, #45568	; 0xb200
60314614:	e3452200 	movt	r2, #20992	; 0x5200
60314618:	e1500001 	cmp	r0, r1
6031461c:	11500002 	cmpne	r0, r2
60314620:	0a00002e 	beq	603146e0 <RTIM_GetCount+0x1a0>
60314624:	e3a01b2d 	mov	r1, #46080	; 0xb400
60314628:	e3451200 	movt	r1, #20992	; 0x5200
6031462c:	e3a02cb6 	mov	r2, #46592	; 0xb600
60314630:	e3452200 	movt	r2, #20992	; 0x5200
60314634:	e1500001 	cmp	r0, r1
60314638:	11500002 	cmpne	r0, r2
6031463c:	0a000027 	beq	603146e0 <RTIM_GetCount+0x1a0>
60314640:	e3a01b2e 	mov	r1, #47104	; 0xb800
60314644:	e3451200 	movt	r1, #20992	; 0x5200
60314648:	e3a02cba 	mov	r2, #47616	; 0xba00
6031464c:	e3452200 	movt	r2, #20992	; 0x5200
60314650:	e1500001 	cmp	r0, r1
60314654:	11500002 	cmpne	r0, r2
60314658:	0a000020 	beq	603146e0 <RTIM_GetCount+0x1a0>
6031465c:	e3a01b2f 	mov	r1, #48128	; 0xbc00
60314660:	e3451200 	movt	r1, #20992	; 0x5200
60314664:	e3a02cbe 	mov	r2, #48640	; 0xbe00
60314668:	e3452200 	movt	r2, #20992	; 0x5200
6031466c:	e1500001 	cmp	r0, r1
60314670:	11500002 	cmpne	r0, r2
60314674:	0a000019 	beq	603146e0 <RTIM_GetCount+0x1a0>
60314678:	e3a01a0a 	mov	r1, #40960	; 0xa000
6031467c:	e3451100 	movt	r1, #20736	; 0x5100
60314680:	e3a02ca2 	mov	r2, #41472	; 0xa200
60314684:	e3452100 	movt	r2, #20736	; 0x5100
60314688:	e1500001 	cmp	r0, r1
6031468c:	11500002 	cmpne	r0, r2
60314690:	0a000012 	beq	603146e0 <RTIM_GetCount+0x1a0>
60314694:	e3a01b29 	mov	r1, #41984	; 0xa400
60314698:	e3451100 	movt	r1, #20736	; 0x5100
6031469c:	e3a02ca6 	mov	r2, #42496	; 0xa600
603146a0:	e3452100 	movt	r2, #20736	; 0x5100
603146a4:	e1500001 	cmp	r0, r1
603146a8:	11500002 	cmpne	r0, r2
603146ac:	0a00000b 	beq	603146e0 <RTIM_GetCount+0x1a0>
603146b0:	e3a01b2a 	mov	r1, #43008	; 0xa800
603146b4:	e3451100 	movt	r1, #20736	; 0x5100
603146b8:	e3a02caa 	mov	r2, #43520	; 0xaa00
603146bc:	e3452100 	movt	r2, #20736	; 0x5100
603146c0:	e1500001 	cmp	r0, r1
603146c4:	11500002 	cmpne	r0, r2
603146c8:	0a000004 	beq	603146e0 <RTIM_GetCount+0x1a0>
603146cc:	e3060d20 	movw	r0, #27936	; 0x6d20
603146d0:	e3460031 	movt	r0, #24625	; 0x6031
603146d4:	e3a010e2 	mov	r1, #226	; 0xe2
603146d8:	e92d4010 	push	{r4, lr}
603146dc:	ebfffcaa 	bl	6031398c <io_assert_failed>
603146e0:	e5932014 	ldr	r2, [r3, #20]
603146e4:	e5930014 	ldr	r0, [r3, #20]
603146e8:	e1520000 	cmp	r2, r0
603146ec:	15930014 	ldrne	r0, [r3, #20]
603146f0:	e12fff1e 	bx	lr

603146f4 <_2char2hex>:
603146f4:	e2403030 	sub	r3, r0, #48	; 0x30
603146f8:	e6ef2073 	uxtb	r2, r3
603146fc:	e3520009 	cmp	r2, #9
60314700:	97a30053 	sbfxls	r0, r3, #0, #4
60314704:	91a00200 	lslls	r0, r0, #4
60314708:	9a000008 	bls	60314730 <_2char2hex+0x3c>
6031470c:	e2403061 	sub	r3, r0, #97	; 0x61
60314710:	e3530005 	cmp	r3, #5
60314714:	9a000013 	bls	60314768 <_2char2hex+0x74>
60314718:	e2403041 	sub	r3, r0, #65	; 0x41
6031471c:	e3530005 	cmp	r3, #5
60314720:	92400037 	subls	r0, r0, #55	; 0x37
60314724:	83e0000f 	mvnhi	r0, #15
60314728:	97a33050 	sbfxls	r3, r0, #0, #4
6031472c:	91a00203 	lslls	r0, r3, #4
60314730:	e2413030 	sub	r3, r1, #48	; 0x30
60314734:	e6ef2073 	uxtb	r2, r3
60314738:	e3520009 	cmp	r2, #9
6031473c:	9a000010 	bls	60314784 <_2char2hex+0x90>
60314740:	e2413061 	sub	r3, r1, #97	; 0x61
60314744:	e3530005 	cmp	r3, #5
60314748:	9a000010 	bls	60314790 <_2char2hex+0x9c>
6031474c:	e2413041 	sub	r3, r1, #65	; 0x41
60314750:	e3530005 	cmp	r3, #5
60314754:	92411037 	subls	r1, r1, #55	; 0x37
60314758:	91800001 	orrls	r0, r0, r1
6031475c:	96ef0070 	uxtbls	r0, r0
60314760:	83a000ff 	movhi	r0, #255	; 0xff
60314764:	e12fff1e 	bx	lr
60314768:	e2403057 	sub	r3, r0, #87	; 0x57
6031476c:	e7a33053 	sbfx	r3, r3, #0, #4
60314770:	e1a00203 	lsl	r0, r3, #4
60314774:	e2413030 	sub	r3, r1, #48	; 0x30
60314778:	e6ef2073 	uxtb	r2, r3
6031477c:	e3520009 	cmp	r2, #9
60314780:	8affffee 	bhi	60314740 <_2char2hex+0x4c>
60314784:	e1800003 	orr	r0, r0, r3
60314788:	e6ef0070 	uxtb	r0, r0
6031478c:	e12fff1e 	bx	lr
60314790:	e2411057 	sub	r1, r1, #87	; 0x57
60314794:	e1800001 	orr	r0, r0, r1
60314798:	e6ef0070 	uxtb	r0, r0
6031479c:	e12fff1e 	bx	lr

603147a0 <_memcpy>:
603147a0:	e352000f 	cmp	r2, #15
603147a4:	9a000037 	bls	60314888 <_memcpy+0xe8>
603147a8:	e1803001 	orr	r3, r0, r1
603147ac:	e3130003 	tst	r3, #3
603147b0:	1a000040 	bne	603148b8 <_memcpy+0x118>
603147b4:	e2423010 	sub	r3, r2, #16
603147b8:	e92d40f0 	push	{r4, r5, r6, r7, lr}
603147bc:	e3c3c00f 	bic	ip, r3, #15
603147c0:	e2814020 	add	r4, r1, #32
603147c4:	e2816010 	add	r6, r1, #16
603147c8:	e2807010 	add	r7, r0, #16
603147cc:	e084400c 	add	r4, r4, ip
603147d0:	e1a05223 	lsr	r5, r3, #4
603147d4:	e516300c 	ldr	r3, [r6, #-12]
603147d8:	e516e010 	ldr	lr, [r6, #-16]
603147dc:	e516c008 	ldr	ip, [r6, #-8]
603147e0:	e507300c 	str	r3, [r7, #-12]
603147e4:	e2866010 	add	r6, r6, #16
603147e8:	e5163014 	ldr	r3, [r6, #-20]	; 0xffffffec
603147ec:	e507e010 	str	lr, [r7, #-16]
603147f0:	e507c008 	str	ip, [r7, #-8]
603147f4:	e5073004 	str	r3, [r7, #-4]
603147f8:	e1560004 	cmp	r6, r4
603147fc:	e2877010 	add	r7, r7, #16
60314800:	1afffff3 	bne	603147d4 <_memcpy+0x34>
60314804:	e2853001 	add	r3, r5, #1
60314808:	e312000c 	tst	r2, #12
6031480c:	e1a03203 	lsl	r3, r3, #4
60314810:	e202e00f 	and	lr, r2, #15
60314814:	e0811003 	add	r1, r1, r3
60314818:	01a0200e 	moveq	r2, lr
6031481c:	e0803003 	add	r3, r0, r3
60314820:	0a00000d 	beq	6031485c <_memcpy+0xbc>
60314824:	e2435004 	sub	r5, r3, #4
60314828:	e1a04001 	mov	r4, r1
6031482c:	e494c004 	ldr	ip, [r4], #4
60314830:	e5a5c004 	str	ip, [r5, #4]!
60314834:	e04ec004 	sub	ip, lr, r4
60314838:	e081c00c 	add	ip, r1, ip
6031483c:	e35c0003 	cmp	ip, #3
60314840:	8afffff9 	bhi	6031482c <_memcpy+0x8c>
60314844:	e24ec004 	sub	ip, lr, #4
60314848:	e3ccc003 	bic	ip, ip, #3
6031484c:	e28cc004 	add	ip, ip, #4
60314850:	e2022003 	and	r2, r2, #3
60314854:	e083300c 	add	r3, r3, ip
60314858:	e081100c 	add	r1, r1, ip
6031485c:	e3520000 	cmp	r2, #0
60314860:	e242c001 	sub	ip, r2, #1
60314864:	08bd80f0 	popeq	{r4, r5, r6, r7, pc}
60314868:	e28cc001 	add	ip, ip, #1
6031486c:	e2433001 	sub	r3, r3, #1
60314870:	e081c00c 	add	ip, r1, ip
60314874:	e4d12001 	ldrb	r2, [r1], #1
60314878:	e5e32001 	strb	r2, [r3, #1]!
6031487c:	e151000c 	cmp	r1, ip
60314880:	1afffffb 	bne	60314874 <_memcpy+0xd4>
60314884:	e8bd80f0 	pop	{r4, r5, r6, r7, pc}
60314888:	e3520000 	cmp	r2, #0
6031488c:	e1a03000 	mov	r3, r0
60314890:	e242c001 	sub	ip, r2, #1
60314894:	012fff1e 	bxeq	lr
60314898:	e28cc001 	add	ip, ip, #1
6031489c:	e2433001 	sub	r3, r3, #1
603148a0:	e081c00c 	add	ip, r1, ip
603148a4:	e4d12001 	ldrb	r2, [r1], #1
603148a8:	e5e32001 	strb	r2, [r3, #1]!
603148ac:	e151000c 	cmp	r1, ip
603148b0:	1afffffb 	bne	603148a4 <_memcpy+0x104>
603148b4:	e12fff1e 	bx	lr
603148b8:	e242c001 	sub	ip, r2, #1
603148bc:	e1a03000 	mov	r3, r0
603148c0:	eafffff4 	b	60314898 <_memcpy+0xf8>

603148c4 <_memset>:
603148c4:	e3100003 	tst	r0, #3
603148c8:	0a00003f 	beq	603149cc <_memset+0x108>
603148cc:	e3520000 	cmp	r2, #0
603148d0:	e242c001 	sub	ip, r2, #1
603148d4:	012fff1e 	bxeq	lr
603148d8:	e6ef2071 	uxtb	r2, r1
603148dc:	e1a03000 	mov	r3, r0
603148e0:	ea000002 	b	603148f0 <_memset+0x2c>
603148e4:	e24cc001 	sub	ip, ip, #1
603148e8:	e37c0001 	cmn	ip, #1
603148ec:	012fff1e 	bxeq	lr
603148f0:	e4c32001 	strb	r2, [r3], #1
603148f4:	e3130003 	tst	r3, #3
603148f8:	1afffff9 	bne	603148e4 <_memset+0x20>
603148fc:	e35c0003 	cmp	ip, #3
60314900:	9a000027 	bls	603149a4 <_memset+0xe0>
60314904:	e92d4030 	push	{r4, r5, lr}
60314908:	e6efe071 	uxtb	lr, r1
6031490c:	e35c000f 	cmp	ip, #15
60314910:	e18ee40e 	orr	lr, lr, lr, lsl #8
60314914:	e18ee80e 	orr	lr, lr, lr, lsl #16
60314918:	9a00002e 	bls	603149d8 <_memset+0x114>
6031491c:	e24c2010 	sub	r2, ip, #16
60314920:	e3c2400f 	bic	r4, r2, #15
60314924:	e2835020 	add	r5, r3, #32
60314928:	e0855004 	add	r5, r5, r4
6031492c:	e1a04222 	lsr	r4, r2, #4
60314930:	e2832010 	add	r2, r3, #16
60314934:	e502e010 	str	lr, [r2, #-16]
60314938:	e502e00c 	str	lr, [r2, #-12]
6031493c:	e502e008 	str	lr, [r2, #-8]
60314940:	e502e004 	str	lr, [r2, #-4]
60314944:	e2822010 	add	r2, r2, #16
60314948:	e1520005 	cmp	r2, r5
6031494c:	1afffff8 	bne	60314934 <_memset+0x70>
60314950:	e2842001 	add	r2, r4, #1
60314954:	e31c000c 	tst	ip, #12
60314958:	e0832202 	add	r2, r3, r2, lsl #4
6031495c:	e20cc00f 	and	ip, ip, #15
60314960:	0a000017 	beq	603149c4 <_memset+0x100>
60314964:	e24c3004 	sub	r3, ip, #4
60314968:	e3c33003 	bic	r3, r3, #3
6031496c:	e2833004 	add	r3, r3, #4
60314970:	e0823003 	add	r3, r2, r3
60314974:	e482e004 	str	lr, [r2], #4
60314978:	e1530002 	cmp	r3, r2
6031497c:	1afffffc 	bne	60314974 <_memset+0xb0>
60314980:	e20cc003 	and	ip, ip, #3
60314984:	e35c0000 	cmp	ip, #0
60314988:	08bd8030 	popeq	{r4, r5, pc}
6031498c:	e6ef2071 	uxtb	r2, r1
60314990:	e083c00c 	add	ip, r3, ip
60314994:	e4c32001 	strb	r2, [r3], #1
60314998:	e15c0003 	cmp	ip, r3
6031499c:	1afffffc 	bne	60314994 <_memset+0xd0>
603149a0:	e8bd8030 	pop	{r4, r5, pc}
603149a4:	e35c0000 	cmp	ip, #0
603149a8:	012fff1e 	bxeq	lr
603149ac:	e6ef2071 	uxtb	r2, r1
603149b0:	e083c00c 	add	ip, r3, ip
603149b4:	e4c32001 	strb	r2, [r3], #1
603149b8:	e15c0003 	cmp	ip, r3
603149bc:	1afffffc 	bne	603149b4 <_memset+0xf0>
603149c0:	e12fff1e 	bx	lr
603149c4:	e1a03002 	mov	r3, r2
603149c8:	eaffffed 	b	60314984 <_memset+0xc0>
603149cc:	e1a03000 	mov	r3, r0
603149d0:	e1a0c002 	mov	ip, r2
603149d4:	eaffffc8 	b	603148fc <_memset+0x38>
603149d8:	e1a02003 	mov	r2, r3
603149dc:	eaffffe0 	b	60314964 <_memset+0xa0>

603149e0 <_rand>:
603149e0:	e3a01a09 	mov	r1, #36864	; 0x9000
603149e4:	e3441100 	movt	r1, #16640	; 0x4100
603149e8:	e3a00001 	mov	r0, #1
603149ec:	e3a03001 	mov	r3, #1
603149f0:	e3403001 	movt	r3, #1
603149f4:	ea000001 	b	60314a00 <_rand+0x20>
603149f8:	e2533001 	subs	r3, r3, #1
603149fc:	0a000007 	beq	60314a20 <_rand+0x40>
60314a00:	e591200c 	ldr	r2, [r1, #12]
60314a04:	e352000f 	cmp	r2, #15
60314a08:	05810038 	streq	r0, [r1, #56]	; 0x38
60314a0c:	0afffff9 	beq	603149f8 <_rand+0x18>
60314a10:	e3520000 	cmp	r2, #0
60314a14:	0afffff7 	beq	603149f8 <_rand+0x18>
60314a18:	e5910010 	ldr	r0, [r1, #16]
60314a1c:	e12fff1e 	bx	lr
60314a20:	e5810038 	str	r0, [r1, #56]	; 0x38
60314a24:	eafffff0 	b	603149ec <_rand+0xc>

60314a28 <_strcmp>:
60314a28:	e1803001 	orr	r3, r0, r1
60314a2c:	e3130003 	tst	r3, #3
60314a30:	0a000005 	beq	60314a4c <_strcmp+0x24>
60314a34:	e5d03000 	ldrb	r3, [r0]
60314a38:	e5d12000 	ldrb	r2, [r1]
60314a3c:	e3530000 	cmp	r3, #0
60314a40:	1a00001d 	bne	60314abc <_strcmp+0x94>
60314a44:	e0430002 	sub	r0, r3, r2
60314a48:	e12fff1e 	bx	lr
60314a4c:	e5903000 	ldr	r3, [r0]
60314a50:	e5912000 	ldr	r2, [r1]
60314a54:	e1530002 	cmp	r3, r2
60314a58:	1afffff5 	bne	60314a34 <_strcmp+0xc>
60314a5c:	e92d4030 	push	{r4, r5, lr}
60314a60:	e281c004 	add	ip, r1, #4
60314a64:	e30f5eff 	movw	r5, #65279	; 0xfeff
60314a68:	e34f5efe 	movt	r5, #65278	; 0xfefe
60314a6c:	e3084080 	movw	r4, #32896	; 0x8080
60314a70:	e3484080 	movt	r4, #32896	; 0x8080
60314a74:	e280e004 	add	lr, r0, #4
60314a78:	ea000004 	b	60314a90 <_strcmp+0x68>
60314a7c:	e59c2000 	ldr	r2, [ip]
60314a80:	e49e3004 	ldr	r3, [lr], #4
60314a84:	e28cc004 	add	ip, ip, #4
60314a88:	e1530002 	cmp	r3, r2
60314a8c:	1a00000f 	bne	60314ad0 <_strcmp+0xa8>
60314a90:	e0832005 	add	r2, r3, r5
60314a94:	e1c23003 	bic	r3, r2, r3
60314a98:	e1a0100c 	mov	r1, ip
60314a9c:	e1a0000e 	mov	r0, lr
60314aa0:	e1130004 	tst	r3, r4
60314aa4:	0afffff4 	beq	60314a7c <_strcmp+0x54>
60314aa8:	e3a00000 	mov	r0, #0
60314aac:	e8bd8030 	pop	{r4, r5, pc}
60314ab0:	e5f03001 	ldrb	r3, [r0, #1]!
60314ab4:	e3530000 	cmp	r3, #0
60314ab8:	0a000015 	beq	60314b14 <_strcmp+0xec>
60314abc:	e1a0c001 	mov	ip, r1
60314ac0:	e4d12001 	ldrb	r2, [r1], #1
60314ac4:	e1520003 	cmp	r2, r3
60314ac8:	0afffff8 	beq	60314ab0 <_strcmp+0x88>
60314acc:	eaffffdc 	b	60314a44 <_strcmp+0x1c>
60314ad0:	e5d03000 	ldrb	r3, [r0]
60314ad4:	e5d12000 	ldrb	r2, [r1]
60314ad8:	e3530000 	cmp	r3, #0
60314adc:	1a000003 	bne	60314af0 <_strcmp+0xc8>
60314ae0:	ea000006 	b	60314b00 <_strcmp+0xd8>
60314ae4:	e5f03001 	ldrb	r3, [r0, #1]!
60314ae8:	e3530000 	cmp	r3, #0
60314aec:	0a000005 	beq	60314b08 <_strcmp+0xe0>
60314af0:	e1a0c001 	mov	ip, r1
60314af4:	e4d12001 	ldrb	r2, [r1], #1
60314af8:	e1520003 	cmp	r2, r3
60314afc:	0afffff8 	beq	60314ae4 <_strcmp+0xbc>
60314b00:	e0430002 	sub	r0, r3, r2
60314b04:	e8bd8030 	pop	{r4, r5, pc}
60314b08:	e5dc2001 	ldrb	r2, [ip, #1]
60314b0c:	e0430002 	sub	r0, r3, r2
60314b10:	e8bd8030 	pop	{r4, r5, pc}
60314b14:	e5dc2001 	ldrb	r2, [ip, #1]
60314b18:	eaffffc9 	b	60314a44 <_strcmp+0x1c>

60314b1c <_stricmp>:
60314b1c:	e4d03001 	ldrb	r3, [r0], #1
60314b20:	e4d12001 	ldrb	r2, [r1], #1
60314b24:	e1530002 	cmp	r3, r2
60314b28:	0a000013 	beq	60314b7c <_stricmp+0x60>
60314b2c:	e383c020 	orr	ip, r3, #32
60314b30:	e92d4010 	push	{r4, lr}
60314b34:	e24ce061 	sub	lr, ip, #97	; 0x61
60314b38:	e35e0019 	cmp	lr, #25
60314b3c:	8a00000c 	bhi	60314b74 <_stricmp+0x58>
60314b40:	e3824020 	orr	r4, r2, #32
60314b44:	e15c0004 	cmp	ip, r4
60314b48:	1a000009 	bne	60314b74 <_stricmp+0x58>
60314b4c:	e3530000 	cmp	r3, #0
60314b50:	0a00000d 	beq	60314b8c <_stricmp+0x70>
60314b54:	e4d03001 	ldrb	r3, [r0], #1
60314b58:	e4d12001 	ldrb	r2, [r1], #1
60314b5c:	e1530002 	cmp	r3, r2
60314b60:	0afffff9 	beq	60314b4c <_stricmp+0x30>
60314b64:	e383c020 	orr	ip, r3, #32
60314b68:	e24ce061 	sub	lr, ip, #97	; 0x61
60314b6c:	e35e0019 	cmp	lr, #25
60314b70:	9afffff2 	bls	60314b40 <_stricmp+0x24>
60314b74:	e3a00001 	mov	r0, #1
60314b78:	e8bd8010 	pop	{r4, pc}
60314b7c:	e3530000 	cmp	r3, #0
60314b80:	1affffe5 	bne	60314b1c <_stricmp>
60314b84:	e1a00003 	mov	r0, r3
60314b88:	e12fff1e 	bx	lr
60314b8c:	e1a00003 	mov	r0, r3
60314b90:	e8bd8010 	pop	{r4, pc}

60314b94 <_strlen>:
60314b94:	e3100003 	tst	r0, #3
60314b98:	0a00000b 	beq	60314bcc <_strlen+0x38>
60314b9c:	e1a03000 	mov	r3, r0
60314ba0:	ea000001 	b	60314bac <_strlen+0x18>
60314ba4:	e3130003 	tst	r3, #3
60314ba8:	0a000008 	beq	60314bd0 <_strlen+0x3c>
60314bac:	e1a01003 	mov	r1, r3
60314bb0:	e2833001 	add	r3, r3, #1
60314bb4:	e1a02003 	mov	r2, r3
60314bb8:	e5d1c000 	ldrb	ip, [r1]
60314bbc:	e35c0000 	cmp	ip, #0
60314bc0:	1afffff7 	bne	60314ba4 <_strlen+0x10>
60314bc4:	e0410000 	sub	r0, r1, r0
60314bc8:	e12fff1e 	bx	lr
60314bcc:	e1a02000 	mov	r2, r0
60314bd0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
60314bd4:	e30feeff 	movw	lr, #65279	; 0xfeff
60314bd8:	e34feefe 	movt	lr, #65278	; 0xfefe
60314bdc:	e5921000 	ldr	r1, [r2]
60314be0:	e308c080 	movw	ip, #32896	; 0x8080
60314be4:	e348c080 	movt	ip, #32896	; 0x8080
60314be8:	e081300e 	add	r3, r1, lr
60314bec:	e1c33001 	bic	r3, r3, r1
60314bf0:	e113000c 	tst	r3, ip
60314bf4:	1a000004 	bne	60314c0c <_strlen+0x78>
60314bf8:	e5b21004 	ldr	r1, [r2, #4]!
60314bfc:	e081300e 	add	r3, r1, lr
60314c00:	e1c33001 	bic	r3, r3, r1
60314c04:	e113000c 	tst	r3, ip
60314c08:	0afffffa 	beq	60314bf8 <_strlen+0x64>
60314c0c:	e5d23000 	ldrb	r3, [r2]
60314c10:	e3530000 	cmp	r3, #0
60314c14:	0a000002 	beq	60314c24 <_strlen+0x90>
60314c18:	e5f23001 	ldrb	r3, [r2, #1]!
60314c1c:	e3530000 	cmp	r3, #0
60314c20:	1afffffc 	bne	60314c18 <_strlen+0x84>
60314c24:	e0420000 	sub	r0, r2, r0
60314c28:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

60314c2c <_strtoul>:
60314c2c:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, sl, lr}
60314c30:	e1a0c000 	mov	ip, r0
60314c34:	e1a04000 	mov	r4, r0
60314c38:	e1a06001 	mov	r6, r1
60314c3c:	e1a05002 	mov	r5, r2
60314c40:	e1a0200c 	mov	r2, ip
60314c44:	e4dc3001 	ldrb	r3, [ip], #1
60314c48:	e243e009 	sub	lr, r3, #9
60314c4c:	e3530020 	cmp	r3, #32
60314c50:	0afffffa 	beq	60314c40 <_strtoul+0x14>
60314c54:	e353002c 	cmp	r3, #44	; 0x2c
60314c58:	135e0004 	cmpne	lr, #4
60314c5c:	93a0e001 	movls	lr, #1
60314c60:	83a0e000 	movhi	lr, #0
60314c64:	9afffff5 	bls	60314c40 <_strtoul+0x14>
60314c68:	e353002d 	cmp	r3, #45	; 0x2d
60314c6c:	0a000047 	beq	60314d90 <_strtoul+0x164>
60314c70:	e353002b 	cmp	r3, #43	; 0x2b
60314c74:	0282c002 	addeq	ip, r2, #2
60314c78:	05d23001 	ldrbeq	r3, [r2, #1]
60314c7c:	e3d52010 	bics	r2, r5, #16
60314c80:	0a000032 	beq	60314d50 <_strtoul+0x124>
60314c84:	e3e07000 	mvn	r7, #0
60314c88:	e1a09005 	mov	r9, r5
60314c8c:	e737f517 	udiv	r7, r7, r5
60314c90:	e0080795 	mul	r8, r5, r7
60314c94:	e1e08008 	mvn	r8, r8
60314c98:	e3a01000 	mov	r1, #0
60314c9c:	e1a00001 	mov	r0, r1
60314ca0:	ea000009 	b	60314ccc <_strtoul+0xa0>
60314ca4:	e0402007 	sub	r2, r0, r7
60314ca8:	e16f2f12 	clz	r2, r2
60314cac:	e1530008 	cmp	r3, r8
60314cb0:	e1a022a2 	lsr	r2, r2, #5
60314cb4:	d3a02000 	movle	r2, #0
60314cb8:	e3520000 	cmp	r2, #0
60314cbc:	1a000017 	bne	60314d20 <_strtoul+0xf4>
60314cc0:	e0203099 	mla	r0, r9, r0, r3
60314cc4:	e3a01001 	mov	r1, #1
60314cc8:	e4dc3001 	ldrb	r3, [ip], #1
60314ccc:	e2432030 	sub	r2, r3, #48	; 0x30
60314cd0:	e6efa072 	uxtb	sl, r2
60314cd4:	e35a0009 	cmp	sl, #9
60314cd8:	91a03002 	movls	r3, r2
60314cdc:	9a000008 	bls	60314d04 <_strtoul+0xd8>
60314ce0:	e3c32020 	bic	r2, r3, #32
60314ce4:	e2422041 	sub	r2, r2, #65	; 0x41
60314ce8:	e243a041 	sub	sl, r3, #65	; 0x41
60314cec:	e3520019 	cmp	r2, #25
60314cf0:	8a00000c 	bhi	60314d28 <_strtoul+0xfc>
60314cf4:	e35a001a 	cmp	sl, #26
60314cf8:	33a02037 	movcc	r2, #55	; 0x37
60314cfc:	23a02057 	movcs	r2, #87	; 0x57
60314d00:	e0433002 	sub	r3, r3, r2
60314d04:	e1550003 	cmp	r5, r3
60314d08:	da000006 	ble	60314d28 <_strtoul+0xfc>
60314d0c:	e1500007 	cmp	r0, r7
60314d10:	93a02000 	movls	r2, #0
60314d14:	83a02001 	movhi	r2, #1
60314d18:	e1922fa1 	orrs	r2, r2, r1, lsr #31
60314d1c:	0affffe0 	beq	60314ca4 <_strtoul+0x78>
60314d20:	e3e01000 	mvn	r1, #0
60314d24:	eaffffe7 	b	60314cc8 <_strtoul+0x9c>
60314d28:	e3710001 	cmn	r1, #1
60314d2c:	0a000011 	beq	60314d78 <_strtoul+0x14c>
60314d30:	e35e0000 	cmp	lr, #0
60314d34:	12600000 	rsbne	r0, r0, #0
60314d38:	e3560000 	cmp	r6, #0
60314d3c:	08bd87f0 	popeq	{r4, r5, r6, r7, r8, r9, sl, pc}
60314d40:	e3510000 	cmp	r1, #0
60314d44:	1a00000e 	bne	60314d84 <_strtoul+0x158>
60314d48:	e5864000 	str	r4, [r6]
60314d4c:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
60314d50:	e3530030 	cmp	r3, #48	; 0x30
60314d54:	0a000011 	beq	60314da0 <_strtoul+0x174>
60314d58:	e3550000 	cmp	r5, #0
60314d5c:	1affffc8 	bne	60314c84 <_strtoul+0x58>
60314d60:	e3a0500a 	mov	r5, #10
60314d64:	e3097999 	movw	r7, #39321	; 0x9999
60314d68:	e3417999 	movt	r7, #6553	; 0x1999
60314d6c:	e3a08005 	mov	r8, #5
60314d70:	e1a09005 	mov	r9, r5
60314d74:	eaffffc7 	b	60314c98 <_strtoul+0x6c>
60314d78:	e3560000 	cmp	r6, #0
60314d7c:	e1a00001 	mov	r0, r1
60314d80:	08bd87f0 	popeq	{r4, r5, r6, r7, r8, r9, sl, pc}
60314d84:	e24c4001 	sub	r4, ip, #1
60314d88:	e5864000 	str	r4, [r6]
60314d8c:	e8bd87f0 	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
60314d90:	e5d23001 	ldrb	r3, [r2, #1]
60314d94:	e282c002 	add	ip, r2, #2
60314d98:	e3a0e001 	mov	lr, #1
60314d9c:	eaffffb6 	b	60314c7c <_strtoul+0x50>
60314da0:	e5dc2000 	ldrb	r2, [ip]
60314da4:	e20220df 	and	r2, r2, #223	; 0xdf
60314da8:	e3520058 	cmp	r2, #88	; 0x58
60314dac:	1a000006 	bne	60314dcc <_strtoul+0x1a0>
60314db0:	e3a05010 	mov	r5, #16
60314db4:	e5dc3001 	ldrb	r3, [ip, #1]
60314db8:	e3a0800f 	mov	r8, #15
60314dbc:	e28cc002 	add	ip, ip, #2
60314dc0:	e1a09005 	mov	r9, r5
60314dc4:	e3e0720f 	mvn	r7, #-268435456	; 0xf0000000
60314dc8:	eaffffb2 	b	60314c98 <_strtoul+0x6c>
60314dcc:	e3550000 	cmp	r5, #0
60314dd0:	1affffab 	bne	60314c84 <_strtoul+0x58>
60314dd4:	e3a05008 	mov	r5, #8
60314dd8:	e3a08007 	mov	r8, #7
60314ddc:	e1a09005 	mov	r9, r5
60314de0:	e3e0720e 	mvn	r7, #-536870912	; 0xe0000000
60314de4:	eaffffab 	b	60314c98 <_strtoul+0x6c>

60314de8 <__wrap_memset>:
60314de8:	e30438c4 	movw	r3, #18628	; 0x48c4
60314dec:	e3463031 	movt	r3, #24625	; 0x6031
60314df0:	e12fff13 	bx	r3

60314df4 <__wrap_memcpy>:
60314df4:	e30437a0 	movw	r3, #18336	; 0x47a0
60314df8:	e3463031 	movt	r3, #24625	; 0x6031
60314dfc:	e12fff13 	bx	r3

60314e00 <__wrap_memchr>:
60314e00:	e3043e30 	movw	r3, #20016	; 0x4e30
60314e04:	e3463031 	movt	r3, #24625	; 0x6031
60314e08:	e12fff13 	bx	r3

60314e0c <__wrap_memmove>:
60314e0c:	e3043f10 	movw	r3, #20240	; 0x4f10
60314e10:	e3463031 	movt	r3, #24625	; 0x6031
60314e14:	e12fff13 	bx	r3

60314e18 <__wrap_strlen>:
60314e18:	e3043b94 	movw	r3, #19348	; 0x4b94
60314e1c:	e3463031 	movt	r3, #24625	; 0x6031
60314e20:	e12fff13 	bx	r3

60314e24 <__wrap_strcmp>:
60314e24:	e3043a28 	movw	r3, #18984	; 0x4a28
60314e28:	e3463031 	movt	r3, #24625	; 0x6031
60314e2c:	e12fff13 	bx	r3

60314e30 <_memchr>:
60314e30:	e6ef1071 	uxtb	r1, r1
60314e34:	e3100003 	tst	r0, #3
60314e38:	0a000032 	beq	60314f08 <_memchr+0xd8>
60314e3c:	e1a03000 	mov	r3, r0
60314e40:	e242c001 	sub	ip, r2, #1
60314e44:	e3520000 	cmp	r2, #0
60314e48:	1a000002 	bne	60314e58 <_memchr+0x28>
60314e4c:	ea000017 	b	60314eb0 <_memchr+0x80>
60314e50:	e25cc001 	subs	ip, ip, #1
60314e54:	3a000015 	bcc	60314eb0 <_memchr+0x80>
60314e58:	e1a00003 	mov	r0, r3
60314e5c:	e4d32001 	ldrb	r2, [r3], #1
60314e60:	e1520001 	cmp	r2, r1
60314e64:	012fff1e 	bxeq	lr
60314e68:	e3130003 	tst	r3, #3
60314e6c:	e1a00003 	mov	r0, r3
60314e70:	1afffff6 	bne	60314e50 <_memchr+0x20>
60314e74:	e35c0003 	cmp	ip, #3
60314e78:	e92d4070 	push	{r4, r5, r6, lr}
60314e7c:	8a00000d 	bhi	60314eb8 <_memchr+0x88>
60314e80:	e35c0000 	cmp	ip, #0
60314e84:	0a000007 	beq	60314ea8 <_memchr+0x78>
60314e88:	e1a02000 	mov	r2, r0
60314e8c:	e080300c 	add	r3, r0, ip
60314e90:	e1a00002 	mov	r0, r2
60314e94:	e4d2c001 	ldrb	ip, [r2], #1
60314e98:	e15c0001 	cmp	ip, r1
60314e9c:	08bd8070 	popeq	{r4, r5, r6, pc}
60314ea0:	e1520003 	cmp	r2, r3
60314ea4:	1afffff9 	bne	60314e90 <_memchr+0x60>
60314ea8:	e3a00000 	mov	r0, #0
60314eac:	e8bd8070 	pop	{r4, r5, r6, pc}
60314eb0:	e3a00000 	mov	r0, #0
60314eb4:	e12fff1e 	bx	lr
60314eb8:	e1814401 	orr	r4, r1, r1, lsl #8
60314ebc:	e30f6eff 	movw	r6, #65279	; 0xfeff
60314ec0:	e34f6efe 	movt	r6, #65278	; 0xfefe
60314ec4:	e3085080 	movw	r5, #32896	; 0x8080
60314ec8:	e3485080 	movt	r5, #32896	; 0x8080
60314ecc:	e1844804 	orr	r4, r4, r4, lsl #16
60314ed0:	e1a02000 	mov	r2, r0
60314ed4:	e1a00002 	mov	r0, r2
60314ed8:	e2822004 	add	r2, r2, #4
60314edc:	e5903000 	ldr	r3, [r0]
60314ee0:	e0233004 	eor	r3, r3, r4
60314ee4:	e083e006 	add	lr, r3, r6
60314ee8:	e1ce3003 	bic	r3, lr, r3
60314eec:	e1130005 	tst	r3, r5
60314ef0:	1affffe4 	bne	60314e88 <_memchr+0x58>
60314ef4:	e24cc004 	sub	ip, ip, #4
60314ef8:	e1a00002 	mov	r0, r2
60314efc:	e35c0003 	cmp	ip, #3
60314f00:	8afffff3 	bhi	60314ed4 <_memchr+0xa4>
60314f04:	eaffffdd 	b	60314e80 <_memchr+0x50>
60314f08:	e1a0c002 	mov	ip, r2
60314f0c:	eaffffd8 	b	60314e74 <_memchr+0x44>

60314f10 <_memmove>:
60314f10:	e1500001 	cmp	r0, r1
60314f14:	9a00000b 	bls	60314f48 <_memmove+0x38>
60314f18:	e0813002 	add	r3, r1, r2
60314f1c:	e1530000 	cmp	r3, r0
60314f20:	9a000008 	bls	60314f48 <_memmove+0x38>
60314f24:	e3520000 	cmp	r2, #0
60314f28:	e080c002 	add	ip, r0, r2
60314f2c:	012fff1e 	bxeq	lr
60314f30:	e1a0200c 	mov	r2, ip
60314f34:	e573c001 	ldrb	ip, [r3, #-1]!
60314f38:	e562c001 	strb	ip, [r2, #-1]!
60314f3c:	e1510003 	cmp	r1, r3
60314f40:	1afffffb 	bne	60314f34 <_memmove+0x24>
60314f44:	e12fff1e 	bx	lr
60314f48:	e352000f 	cmp	r2, #15
60314f4c:	8a00000b 	bhi	60314f80 <_memmove+0x70>
60314f50:	e3520000 	cmp	r2, #0
60314f54:	e1a03000 	mov	r3, r0
60314f58:	e242c001 	sub	ip, r2, #1
60314f5c:	012fff1e 	bxeq	lr
60314f60:	e28cc001 	add	ip, ip, #1
60314f64:	e2433001 	sub	r3, r3, #1
60314f68:	e081c00c 	add	ip, r1, ip
60314f6c:	e4d12001 	ldrb	r2, [r1], #1
60314f70:	e5e32001 	strb	r2, [r3, #1]!
60314f74:	e151000c 	cmp	r1, ip
60314f78:	1afffffb 	bne	60314f6c <_memmove+0x5c>
60314f7c:	e12fff1e 	bx	lr
60314f80:	e1803001 	orr	r3, r0, r1
60314f84:	e3130003 	tst	r3, #3
60314f88:	1a000034 	bne	60315060 <_memmove+0x150>
60314f8c:	e2423010 	sub	r3, r2, #16
60314f90:	e92d4030 	push	{r4, r5, lr}
60314f94:	e281c020 	add	ip, r1, #32
60314f98:	e3c3e00f 	bic	lr, r3, #15
60314f9c:	e08cc00e 	add	ip, ip, lr
60314fa0:	e2814010 	add	r4, r1, #16
60314fa4:	e2805010 	add	r5, r0, #16
60314fa8:	e1a0e223 	lsr	lr, r3, #4
60314fac:	e5143010 	ldr	r3, [r4, #-16]
60314fb0:	e5053010 	str	r3, [r5, #-16]
60314fb4:	e2844010 	add	r4, r4, #16
60314fb8:	e514301c 	ldr	r3, [r4, #-28]	; 0xffffffe4
60314fbc:	e505300c 	str	r3, [r5, #-12]
60314fc0:	e2855010 	add	r5, r5, #16
60314fc4:	e5143018 	ldr	r3, [r4, #-24]	; 0xffffffe8
60314fc8:	e5053018 	str	r3, [r5, #-24]	; 0xffffffe8
60314fcc:	e5143014 	ldr	r3, [r4, #-20]	; 0xffffffec
60314fd0:	e5053014 	str	r3, [r5, #-20]	; 0xffffffec
60314fd4:	e154000c 	cmp	r4, ip
60314fd8:	1afffff3 	bne	60314fac <_memmove+0x9c>
60314fdc:	e28e3001 	add	r3, lr, #1
60314fe0:	e312000c 	tst	r2, #12
60314fe4:	e1a03203 	lsl	r3, r3, #4
60314fe8:	e202e00f 	and	lr, r2, #15
60314fec:	e0811003 	add	r1, r1, r3
60314ff0:	01a0200e 	moveq	r2, lr
60314ff4:	e0803003 	add	r3, r0, r3
60314ff8:	0a00000d 	beq	60315034 <_memmove+0x124>
60314ffc:	e2435004 	sub	r5, r3, #4
60315000:	e1a04001 	mov	r4, r1
60315004:	e494c004 	ldr	ip, [r4], #4
60315008:	e5a5c004 	str	ip, [r5, #4]!
6031500c:	e04ec004 	sub	ip, lr, r4
60315010:	e081c00c 	add	ip, r1, ip
60315014:	e35c0003 	cmp	ip, #3
60315018:	8afffff9 	bhi	60315004 <_memmove+0xf4>
6031501c:	e24ec004 	sub	ip, lr, #4
60315020:	e3ccc003 	bic	ip, ip, #3
60315024:	e28cc004 	add	ip, ip, #4
60315028:	e2022003 	and	r2, r2, #3
6031502c:	e083300c 	add	r3, r3, ip
60315030:	e081100c 	add	r1, r1, ip
60315034:	e3520000 	cmp	r2, #0
60315038:	e242c001 	sub	ip, r2, #1
6031503c:	08bd8030 	popeq	{r4, r5, pc}
60315040:	e28cc001 	add	ip, ip, #1
60315044:	e2433001 	sub	r3, r3, #1
60315048:	e081c00c 	add	ip, r1, ip
6031504c:	e4d12001 	ldrb	r2, [r1], #1
60315050:	e5e32001 	strb	r2, [r3, #1]!
60315054:	e151000c 	cmp	r1, ip
60315058:	1afffffb 	bne	6031504c <_memmove+0x13c>
6031505c:	e8bd8030 	pop	{r4, r5, pc}
60315060:	e242c001 	sub	ip, r2, #1
60315064:	e1a03000 	mov	r3, r0
60315068:	eaffffbc 	b	60314f60 <_memmove+0x50>
6031506c:	6d6f682f 	.word	0x6d6f682f
60315070:	75612f65 	.word	0x75612f65
60315074:	6a5f6f74 	.word	0x6a5f6f74
60315078:	696b6e65 	.word	0x696b6e65
6031507c:	775f736e 	.word	0x775f736e
60315080:	356e616c 	.word	0x356e616c
60315084:	726f772f 	.word	0x726f772f
60315088:	6170736b 	.word	0x6170736b
6031508c:	732f6563 	.word	0x732f6563
60315090:	612f6b64 	.word	0x612f6b64
60315094:	2d6f7475 	.word	0x2d6f7475
60315098:	656c6572 	.word	0x656c6572
6031509c:	2d657361 	.word	0x2d657361
603150a0:	7473616d 	.word	0x7473616d
603150a4:	632f7265 	.word	0x632f7265
603150a8:	6f706d6f 	.word	0x6f706d6f
603150ac:	746e656e 	.word	0x746e656e
603150b0:	2f736f2f 	.word	0x2f736f2f
603150b4:	65657266 	.word	0x65657266
603150b8:	736f7472 	.word	0x736f7472
603150bc:	6572662f 	.word	0x6572662f
603150c0:	6f747265 	.word	0x6f747265
603150c4:	65685f73 	.word	0x65685f73
603150c8:	5f357061 	.word	0x5f357061
603150cc:	666e6f63 	.word	0x666e6f63
603150d0:	632e6769 	.word	0x632e6769
	...

603150d5 <_ctype_>:
603150d5:	20202000 20202020 28282020 20282828     .         ((((( 
603150e5:	20202020 20202020 20202020 20202020                     
603150f5:	10108820 10101010 10101010 10101010      ...............
60315105:	04040410 04040404 10040404 10101010     ................
60315115:	41411010 41414141 01010101 01010101     ..AAAAAA........
60315125:	01010101 01010101 01010101 10101010     ................
60315135:	42421010 42424242 02020202 02020202     ..BBBBBB........
60315145:	02020202 02020202 02020202 10101010     ................
60315155:	00000020 00000000 00000000 00000000      ...............
	...

603151d8 <_global_impure_ptr>:
603151d8:	60317004 00000000                       .p1`....

603151e0 <p05.0>:
603151e0:	00000005 00000019 0000007d 00000000     ........}.......

603151f0 <__mprec_tens>:
603151f0:	00000000 3ff00000 00000000 40240000     .......?......$@
60315200:	00000000 40590000 00000000 408f4000     ......Y@.....@.@
60315210:	00000000 40c38800 00000000 40f86a00     .......@.....j.@
60315220:	00000000 412e8480 00000000 416312d0     .......A......cA
60315230:	00000000 4197d784 00000000 41cdcd65     .......A....e..A
60315240:	20000000 4202a05f e8000000 42374876     ... _..B....vH7B
60315250:	a2000000 426d1a94 e5400000 42a2309c     ......mB..@..0.B
60315260:	1e900000 42d6bcc4 26340000 430c6bf5     .......B..4&.k.C
60315270:	37e08000 4341c379 85d8a000 43763457     ...7y.AC....W4vC
60315280:	674ec800 43abc16d 60913d00 43e158e4     ..Ngm..C.=.`.X.C
60315290:	78b58c40 4415af1d d6e2ef50 444b1ae4     @..x...DP.....KD
603152a0:	064dd592 4480f0cf c7e14af6 44b52d02     ..M....D.J...-.D
603152b0:	79d99db4 44ea7843                       ...yCx.D

603152b8 <__mprec_tinytens>:
603152b8:	97d889bc 3c9cd2b2 d5a8a733 3949f623     .......<3...#.I9
603152c8:	44f4a73d 32a50ffd cf8c979d 255bba08     =..D...2......[%
603152d8:	64ac6f43 0ac80628                       Co.d(...

603152e0 <__mprec_bigtens>:
603152e0:	37e08000 4341c379 b5056e17 4693b8b5     ...7y.AC.n.....F
603152f0:	e93ff9f5 4d384f03 f9301d32 5a827748     ..?..O8M2.0.Hw.Z
60315300:	7f73bf3c 75154fdd                       <.s..O.u

60315308 <blanks.1>:
60315308:	20202020 20202020 20202020 20202020                     

60315318 <zeroes.0>:
60315318:	30303030 30303030 30303030 30303030     0000000000000000

60315328 <blanks.1>:
60315328:	20202020 20202020 20202020 20202020                     

60315338 <zeroes.0>:
60315338:	30303030 30303030 30303030 30303030     0000000000000000
60315348:	2d2d2d2d 2d2d2d2d 2d2d2d2d 2d2d2d2d     ----------------
60315358:	4554202d 43205453 414d4d4f 4d20444e     - TEST COMMAND M
60315368:	2045444f 504c4548 2d782520 2d2d2d2d     ODE HELP %x-----
60315378:	2d2d2d2d 2d2d2d2d 2d2d2d2d 00000a2d     -------------...
60315388:	494e4f4d 00524f54 000a7325 00000023     MONITOR.%s..#...
60315398:	2d2d2d2d 2d2d2d2d 2d2d2d2d 2d2d2d2d     ----------------
603153a8:	4554202d 43205453 414d4d4f 4d20444e     - TEST COMMAND M
603153b8:	2045444f 20444e45 2d782520 2d2d2d2d     ODE END  %x-----
603153c8:	2d2d2d2d 2d2d2d2d 2d2d2d2d 00000a2d     -------------...
603153d8:	6f626552 6e69746f 2e2e2067 000d0a2e     Rebooting ......
603153e8:	74726175 6e727562 00000000 00000072     uartburn....r...
603153f8:	00000061 65707974 00000000 00006770     a...type....pg..
60315408:	00006763 00746567 6b636f6c 3a746962     cg..get.lockbit:
60315418:	20786c25 0000000a 706c7364 636f6c5f     %lx ....dslp_loc
60315428:	7469626b 786c253a 0000000a 3830255b     kbit:%lx....[%08
60315438:	25205d78 20783830 0000000a 6e6f7257     x] %08x ....Wron
60315448:	72612067 656d7567 6e20746e 65626d75     g argument numbe
60315458:	0a0d2172 00000000 50495352 616e6520     r!......RSIP ena
60315468:	64656c62 6143202c 2074276e 64616552     bled, Can't Read
60315478:	616c4620 0a2e6873 00000000 6e6f7257      Flash......Wron
60315488:	6d632067 0a0d2164 00000000 73756665     g cmd!......efus
60315498:	656d2065 616d206d 636f6c6c 69616620     e mem malloc fai
603154a8:	000a206c 70616d72 00000000 00000d0a     l ..rmap........
603154b8:	53554645 6f4c5f45 61636967 70614d6c     EFUSE_LogicalMap
603154c8:	6165525f 61662064 0a206c69 00000000     _Read fail .....
603154d8:	46450d0a 5b455355 6c333025 003a5d78     ..EFUSE[%03lx]:.
603154e8:	32302520 00000078 70616d77 00000000      %02x...wmap....
603154f8:	61766e49 2064696c 63677261 000a202e     Invalid argc. ..
60315508:	69727473 6c20676e 74676e65 6c252868     string length(%l
60315518:	73202975 6c756f68 65622064 64646f20     u) should be odd
60315528:	00000a20 73706f4f 7277203a 20657469      ...Oops: write 
60315538:	746e656c 6f6e2068 616d2074 20686374     lenth not match 
60315548:	75706e69 74732074 676e6972 6e656c20     input string len
60315558:	202c6774 6f6f6863 73206573 6c6c616d     tg, choose small
60315568:	6f207265 000a656e 73756665 6d772065     er one..efuse wm
60315578:	77207061 65746972 6e656c20 756c253a     ap write len:%lu
60315588:	7473202c 676e6972 6e656c20 756c253a     , string len:%lu
60315598:	0000000a 77617272 00000000 5f50544f     ....rraw....OTP_
603155a8:	64616552 61662038 0a206c69 00000000     Read8 fail .....
603155b8:	61520d0a 70614d77 3330255b 3a5d786c     ..RawMap[%03lx]:
603155c8:	00000000 77617277 00000000 73756665     ....wraw....efus
603155d8:	72772065 77207761 65746972 6e656c20     e wraw write len
603155e8:	756c253a 7473202c 676e6972 6e656c20     :%lu, string len
603155f8:	756c253a 0000000a 77617277 6c25203a     :%lu....wraw: %l
60315608:	78252078 00000a20 5f50544f 74697257     x %x ...OTP_Writ
60315618:	66203865 206c6961 0000000a 43544553     e8 fail ....SETC
60315628:	00004352 65726143 2c6c7566 6c6e4f20     RC..Careful, Onl
60315638:	20342079 20435243 72746e65 43202e79     y 4 CRC entry. C
60315648:	6920444d 66652073 20657375 43544553     MD is efuse SETC
60315658:	30204352 33373878 00000a30 0000003f     RC 0x8730...?...
60315668:	4c454809 7e282050 0a20293f 50200909     .HELP (~?) ... P
60315678:	746e6972 69687420 65682073 6d20706c     rint this help m
60315688:	61737365 00000a67 00005744 20574409     essag...DW...DW 
60315698:	6464413c 73736572 654c202c 6874676e     <Address, Length
603156a8:	09090a3e 6d754420 656d2070 79726f6d     >... Dump memory
603156b8:	726f7720 726f2064 61655220 77482064      word or Read Hw
603156c8:	726f7720 65722064 74736967 203b7265      word register; 
603156d8:	2009090a 3c205744 72646461 0909203e     ... DW <addr> ..
603156e8:	6d754420 6e6f2070 6f20796c 7720656e      Dump only one w
603156f8:	2064726f 74207461 73206568 20656d61     ord at the same 
60315708:	656d6974 6e75202c 203a7469 74794234     time, unit: 4Byt
60315718:	090a7365 57442009 64613c20 203e7264     es... DW <addr> 
60315728:	6e656c3c 4420093e 20706d75 20656874     <len>. Dump the 
60315738:	63657073 65696669 656c2064 6874676e     specified length
60315748:	20666f20 20656874 64726f77 6e75202c      of the word, un
60315758:	203a7469 74794234 090a7365 57442009     it: 4Bytes... DW
60315768:	64613c20 203e7264 6e656c3c 2062203e      <addr> <len> b 
60315778:	6d754420 68742070 70732065 66696365      Dump the specif
60315788:	20646569 676e656c 6f206874 68742066     ied length of th
60315798:	79622065 202c6574 74696e75 4231203a     e byte, unit: 1B
603157a8:	0a657479 00000000 00005745 20574509     yte.....EW...EW 
603157b8:	6464413c 73736572 6548202c 090a3e78     <Address, Hex>..
603157c8:	72572009 20657469 6f6d656d 64207972     . Write memory d
603157d8:	64726f77 20726f20 74697257 77482065     word or Write Hw
603157e8:	6f776420 72206472 73696765 20726574      dword register 
603157f8:	2009090a 206e6143 74697277 6e6f2065     ... Can write on
60315808:	6f20796c 6420656e 64726f77 20746120     ly one dword at 
60315818:	20656874 656d6173 6d697420 090a2065     the same time ..
60315828:	78452009 5745203a 64644120 73736572     . Ex: EW Address
60315838:	6c615620 0a206575 00000000 4f424552      Value .....REBO
60315848:	0000544f 42455209 20544f4f 2009090a     OT...REBOOT ... 
60315858:	6574693c 73202c6d 6e697274 3a203e67     <item, string> :
60315868:	09090a20 65746920 75203a6d 62747261      ... item: uartb
60315878:	206e7275 4e20726f 0a20412f 0a200909     urn or N/A ... .
60315888:	00000000 53554645 00000045 55464509     ....EFUSE....EFU
60315898:	0a204553 77200909 2070616d 72646461     SE ... wmap addr
603158a8:	78656828 656c2029 6568286e 64202978     (hex) len(hex) d
603158b8:	28617461 29786568 2009090a 70616d72     ata(hex)... rmap
603158c8:	09090a20 6d773c20 30207061 20303078      ... <wmap 0x00 
603158d8:	20327830 35393138 6665203e 5b657375     0x2 8195> efuse[
603158e8:	303d5d30 2c313878 75666520 5b206573     0]=0x81, efuse [
603158f8:	303d5d31 0a353978 3c200909 70616d77     1]=0x95... <wmap
60315908:	46783020 78302030 31312034 33333232      0xF0 0x4 112233
60315918:	203e3434 4678305b 303d5d30 2c313178     44> [0xF0]=0x11,
60315928:	78305b20 3d5d3146 32327830 305b202c      [0xF1]=0x22, [0
60315938:	5d324678 3378303d 5b202c33 33467830     xF2]=0x33, [0xF3
60315948:	78303d5d 000a3434 4b434954 00005350     ]=0x44..TICKPS..
60315958:	43495409 2053504b 2009090a 72203a72     .TICKPS ... r: r
60315968:	61656c65 6f206573 61772073 6f6c656b     elease os wakelo
60315978:	0a206b63 61200909 6361203a 72697571     ck ... a: acquir
60315988:	736f2065 6b617720 636f6c65 000a206b     e os wakelock ..
60315998:	6e750a0d 776f6e6b 6f63206e 6e616d6d     ..unknown comman
603159a8:	25272064 00002773 230a0a0d 00000a0d     d '%s'.....#....
603159b8:	6c656873 61745f6c 00006b73 61657243     shell_task..Crea
603159c8:	4c206574 5520676f 20545241 6b736154     te Log UART Task
603159d8:	72724520 000a2121 6e616843 206c656e      Err!!..Channel 
603159e8:	666e6f43 7463696c 726f6620 61684320     Conflict for Cha
603159f8:	6c656e6e 756c2520 0a0d2120 00000000     nnel %lu !......
60315a08:	00435049 43504920 71655220 74736575     IPC. IPC Request
60315a18:	6d695420 74756f65 00000a0d 43504920      Timeout.... IPC
60315a28:	74654720 6d655320 6f687061 54206572      Get Semaphore T
60315a38:	6f656d69 0a0d7475 00000000 6d6f682f     imeout....../hom
60315a48:	75612f65 6a5f6f74 696b6e65 775f736e     e/auto_jenkins_w
60315a58:	356e616c 726f772f 6170736b 732f6563     lan5/workspace/s
60315a68:	612f6b64 2d6f7475 656c6572 2d657361     dk/auto-release-
60315a78:	7473616d 632f7265 6f706d6f 746e656e     master/component
60315a88:	636f732f 656d612f 6d736162 2f747261     /soc/amebasmart/
60315a98:	696c7766 61722f62 6f635f6d 6e6f6d6d     fwlib/ram_common
60315aa8:	656d612f 695f6162 615f6370 632e6970     /ameba_ipc_api.c
60315ab8:	00000000                                ....

60315abc <CSWTCH.40>:
60315abc:	00000008 00000000 00000000 00000000     ................
60315acc:	00000008 00000000 00000000 00000000     ................
60315adc:	00000008                                ....

60315ae0 <CSWTCH.41>:
60315ae0:	42008600 42008600 00000000 00000000     ...B...B........
	...
60315b20:	41000500 41000500 00000000 00000000     ...A...A........
	...
60315b60:	41000580 41000580 6d6f682f 75612f65     ...A...A/home/au
60315b70:	6a5f6f74 696b6e65 775f736e 356e616c     to_jenkins_wlan5
60315b80:	726f772f 6170736b 732f6563 612f6b64     /workspace/sdk/a
60315b90:	2d6f7475 656c6572 2d657361 7473616d     uto-release-mast
60315ba0:	632f7265 6f706d6f 746e656e 636f732f     er/component/soc
60315bb0:	656d612f 6d736162 2f747261 696c7766     /amebasmart/fwli
60315bc0:	61722f62 6f635f6d 6e6f6d6d 656d612f     b/ram_common/ame
60315bd0:	695f6162 725f6370 632e6d61 00000000     ba_ipc_ram.c....
60315be0:	2050544f 64616572 74656720 20776820     OTP read get hw 
60315bf0:	616d6573 69616620 00000a6c 0050544f     sema fail...OTP.
60315c00:	5f50544f 64616552 61462038 25206c69     OTP_Read8 Fail %
60315c10:	000a2078 20524d50 64616552 72726520     x ..PMR Read err
60315c20:	0a21726f 00000000 2050544f 74697277     or!.....OTP writ
60315c30:	65672065 77682074 6d657320 61662061     e get hw sema fa
60315c40:	000a6c69 5f50544f 74697257 46203865     il..OTP_Write8 F
60315c50:	206c6961 0a207825 00000000 2050544f     ail %x .....OTP 
60315c60:	676f7270 206d6172 6f727265 000a2172     program error!..
60315c70:	32524d50 61655220 72652064 21726f72     PMR2 Read error!
60315c80:	0000000a 69676f4c 4d6c6163 52207061     ....LogicalMap R
60315c90:	20646165 6f727265 78252072 2078252b     ead error %x+%x 
60315ca0:	63786520 20646565 696d696c 00000a74      exceed limit...
60315cb0:	67696c61 6e656d6e 72652074 20726f72     alignment error 
60315cc0:	25207825 000a2078 69676f4c 4d6c6163     %x %x ..LogicalM
60315cd0:	57207061 65746972 72726520 2520726f     ap Write error %
60315ce0:	78252b78 78652020 64656563 6d696c20     x+%x  exceed lim
60315cf0:	000a7469 69676f4c 4d6c6163 52207061     it..LogicalMap R
60315d00:	20646165 6f727265 68772072 77206e65     ead error when w
60315d10:	65746972 25204020 000a2078 6477656e     rite @ %x ..newd
60315d20:	5b617461 3d5d7825 0a782520 00000000     ata[%x]= %x.....
60315d30:	5f50544f 505f4750 656b6361 6f575f74     OTP_PG_Packet_Wo
60315d40:	73206472 20657a69 6f727265 253a2072     rd size error :%
60315d50:	656c2078 78253a6e 00000a20 656b614d     x len:%x ...Make
60315d60:	72757320 544f2065 6f6c2050 61636967      sure OTP logica
60315d70:	7261206c 20206165 2078253a 66656420     l area  :%x  def
60315d80:	64656e69 0000000a 5f50544f 505f4750     ined....OTP_PG_P
60315d90:	656b6361 255b2074 25205d78 000a2078     acket [%x] %x ..
60315da0:	5f50544f 505f4750 656b6361 6f6e2074     OTP_PG_Packet no
60315db0:	6f6e6520 20686775 63617073 78252065      enough space %x
60315dc0:	00000a20 5f50544f 505f4750 656b6361      ...OTP_PG_Packe
60315dd0:	79422074 20206574 5d78255b 20782520     t Byte  [%x] %x 
60315de0:	0000000a 7478656e 69727720 63206574     ....next write c
60315df0:	656c6379 73614220 6c252065 6e632078     ycle Base %lx cn
60315e00:	25207374 0a20786c 00000000 65737341     ts %lx .....Asse
60315e10:	45207472 726f7272 73252021 0d75253a     rt Error! %s:%u.
60315e20:	0000000a 61766e49 2064696c 6d726150     ....Invalid Parm
60315e30:	72657465 000a2120 20344d4b 20435049     eter !..KM4 IPC 
60315e40:	65636552 20657669 756e694c 544f2078     Receive Linux OT
60315e50:	68702050 63697379 74206c61 20747365     P physical test 
60315e60:	73796870 6c616369 69727720 69206574     physical write i
60315e70:	7274736e 69746375 0a2e6e6f 73657420     nstruction.. tes
60315e80:	72772074 3a657469 72662020 61206d6f     t write:  from a
60315e90:	65726464 5b207373 205d7525 5b206f74     ddress [%u] to [
60315ea0:	2e5d7525 00000a20 5d75255b 7825202c     %u]. ...[%u], %x
60315eb0:	0000000a 20525349 20726f66 65746e69     ....ISR for inte
60315ec0:	70757272 64692074 756c2528 73692029     rrupt id(%lu) is
60315ed0:	746f6e20 67657220 65747369 21646572      not registered!
60315ee0:	0000200a 00003052 00003452 65726f43     . ..R0..R4..Core
60315ef0:	786c255b 7325205d 30203d20 38302578     [%lx] %s = 0x%08
60315f00:	000a786c 65726f43 786c255b 7250205d     lx..Core[%lx] Pr
60315f10:	6f697665 4d207375 2765646f 50432073     evious Mode's CP
60315f20:	69205253 78302073 6c383025 00000a78     SR is 0x%08lx...
60315f30:	65726f43 786c255b 7250205d 6f697665     Core[%lx] Previo
60315f40:	4d207375 2765646f 524c2073 20736920     us Mode's LR is 
60315f50:	30257830 0a786c38 00000000 65726f43     0x%08lx.....Core
60315f60:	786c255b 7543205d 6e657272 74532074     [%lx] Current St
60315f70:	206b6361 6e696f50 20726574 7830203d     ack Pointer = 0x
60315f80:	6c383025 61202c78 6420646e 20706d75     %08lx, and dump 
60315f90:	63617473 6564206b 20687470 6425203d     stack depth = %d
60315fa0:	00000a0d 65726f43 786c255b 255b205d     ....Core[%lx] [%
60315fb0:	786c3830 7830205d 6c383025 78302078     08lx] 0x%08lx 0x
60315fc0:	6c383025 78302078 6c383025 78302078     %08lx 0x%08lx 0x
60315fd0:	6c383025 00000a78 72646441 20737365     %08lx...Address 
60315fe0:	7420666f 75206568 6665646e 64656e69     of the undefined
60315ff0:	736e6920 63757274 6e6f6974 25783020      instruction 0x%
60316000:	0a783830 00000000 61746144 6f626120     08x.....Data abo
60316010:	77207472 20687469 61746144 75614620     rt with Data Fau
60316020:	5320746c 75746174 65522073 74736967     lt Status Regist
60316030:	20207265 30257830 000a7838 72646441     er  0x%08x..Addr
60316040:	20737365 4920666f 7274736e 69746375     ess of Instructi
60316050:	63206e6f 69737561 4420676e 20617461     on causing Data 
60316060:	726f6261 78302074 78383025 0000000a     abort 0x%08x....
60316070:	66657250 68637465 6f626120 77207472     Prefetch abort w
60316080:	20687469 74736e49 74637572 206e6f69     ith Instruction 
60316090:	6c756146 74532074 73757461 67655220     Fault Status Reg
603160a0:	65747369 78302072 78383025 0000000a     ister 0x%08x....
603160b0:	00003552 00003652 00003752 00003852     R5..R6..R7..R8..
603160c0:	00003952 00303152 00313152 00003152     R9..R10.R11.R1..
603160d0:	00003252 00003352 00323152 0000524c     R2..R3..R12.LR..

603160e0 <crash_reg_name>:
603160e0:	60315ee8 603160b0 603160b4 603160b8     .^1`.`1`.`1`.`1`
603160f0:	603160bc 603160c0 603160c4 603160c8     .`1`.`1`.`1`.`1`
60316100:	60315ee4 603160cc 603160d0 603160d4     .^1`.`1`.`1`.`1`
60316110:	603160d8 603160dc 666e6f43 7463696c     .`1`.`1`Conflict
60316120:	20676e69 49435350 72657620 6e6f6973     ing PSCI version
60316130:	74656420 65746365 000a2e64 73757254      detected...Trus
60316140:	20646574 6d20534f 61726769 6e6f6974     ted OS migration
60316150:	746f6e20 71657220 65726975 00000a64      not required...
60316160:	5247494d 5f455441 4f464e49 5059545f     MIGRATE_INFO_TYP
60316170:	6f6e2045 75732074 726f7070 2e646574     E not supported.
60316180:	0000000a 5247494d 5f455441 4f464e49     ....MIGRATE_INFO
60316190:	5059545f 65722045 6e727574 75206465     _TYPE returned u
603161a0:	6f6e6b6e 74206e77 20657079 29642528     nknown type (%d)
603161b0:	0000000a 5247494d 5f455441 4f464e49     ....MIGRATE_INFO
603161c0:	5f50555f 20555043 6f706572 64657472     _UP_CPU reported
603161d0:	766e6920 64696c61 79687020 61636973      invalid physica
603161e0:	4449206c 78302820 29786c25 0000000a     l ID (0x%lx)....
603161f0:	73757254 20646574 7220534f 64697365     Trusted OS resid
60316200:	20746e65 70206e6f 69737968 206c6163     ent on physical 
60316210:	20555043 6c257830 00000a78              CPU 0x%lx...

6031621c <CSWTCH.2>:
6031621c:	ffffffea ffffffea ffffffea ffffffea     ................
6031622c:	ffffffea ffffffea ffffffff ffffffea     ................
6031623c:	ffffffa1 00000000 31757063 776f7020     ........cpu1 pow
6031624c:	6f207265 000a6666 6f636553 7261646e     er off..Secondar
6031625c:	6f632079 70206572 7265776f 66666f20     y core power off
6031626c:	69616620 25203a6c 00000a64 25555043      fail: %d...CPU%
6031627c:	6f203a64 00000a6e 3a706d73 69724220     d: on...smp: Bri
6031628c:	6e69676e 70752067 63657320 61646e6f     nging up seconda
6031629c:	43207972 20735550 0a2e2e2e 00000000     ry CPUs ........
603162ac:	25555043 66203a64 656c6961 6f742064     CPU%d: failed to
603162bc:	6f6f6220 25203a74 00000a64 47505041      boot: %d...APPG
603162cc:	0000000a 57505041 0000000a 47435041     ....APPW....APCG
603162dc:	0000000a 57435041 0000000a 65737361     ....APCW....asse
603162ec:	65207472 20217272 656c6966 7325203a     rt err! file: %s
603162fc:	696c202c 203a656e 000a6425 6d6f682f     , line: %d../hom
6031630c:	75612f65 6a5f6f74 696b6e65 775f736e     e/auto_jenkins_w
6031631c:	356e616c 726f772f 6170736b 732f6563     lan5/workspace/s
6031632c:	612f6b64 2d6f7475 656c6572 2d657361     dk/auto-release-
6031633c:	7473616d 632f7265 6f706d6f 746e656e     master/component
6031634c:	636f732f 656d612f 6d736162 2f747261     /soc/amebasmart/
6031635c:	696c7766 70612f62 726f635f 6c782f65     fwlib/ap_core/xl
6031636c:	745f7461 656c6261 616c782f 61745f74     at_table/xlat_ta
6031637c:	73656c62 6e6f635f 74786574 0000632e     bles_context.c..
6031638c:	6d6f682f 75612f65 6a5f6f74 696b6e65     /home/auto_jenki
6031639c:	775f736e 356e616c 726f772f 6170736b     ns_wlan5/workspa
603163ac:	732f6563 612f6b64 2d6f7475 656c6572     ce/sdk/auto-rele
603163bc:	2d657361 7473616d 632f7265 6f706d6f     ase-master/compo
603163cc:	746e656e 636f732f 656d612f 6d736162     nent/soc/amebasm
603163dc:	2f747261 696c7766 70612f62 726f635f     art/fwlib/ap_cor
603163ec:	6c782f65 745f7461 656c6261 616c782f     e/xlat_table/xla
603163fc:	61745f74 73656c62 726f635f 00632e65     t_tables_core.c.
6031640c:	616d6d0a 64615f70 65725f64 6e6f6967     .mmap_add_region
6031641c:	6568635f 29286b63 69616620 2e64656c     _check() failed.
6031642c:	72726520 2520726f 00000a64 20746f4e      error %d...Not 
6031643c:	756f6e65 6d206867 726f6d65 6f742079     enough memory to
6031644c:	70616d20 67657220 3a6e6f69 4156200a      map region:. VA
6031645c:	2578303a 2020786c 303a4150 6c6c2578     :0x%lx  PA:0x%ll
6031646c:	73202078 3a657a69 7a257830 61202078     x  size:0x%zx  a
6031647c:	3a727474 78257830 0000000a 6d6f682f     ttr:0x%x..../hom
6031648c:	75612f65 6a5f6f74 696b6e65 775f736e     e/auto_jenkins_w
6031649c:	356e616c 726f772f 6170736b 732f6563     lan5/workspace/s
603164ac:	612f6b64 2d6f7475 656c6572 2d657361     dk/auto-release-
603164bc:	7473616d 632f7265 6f706d6f 746e656e     master/component
603164cc:	636f732f 656d612f 6d736162 2f747261     /soc/amebasmart/
603164dc:	696c7766 70612f62 726f635f 6c782f65     fwlib/ap_core/xl
603164ec:	745f7461 656c6261 616c782f 61745f74     at_table/xlat_ta
603164fc:	73656c62 6372615f 00632e68 2d73255b     bles_arch.c.[%s-
6031650c:	205d6325 00000000 3830255b 00205d78     %c] ....[%08x] .
6031651c:	255b0a0d 5d783830 00000020 78383025     ..[%08x] ...%08x
6031652c:	00000020 78323025 00000020 5d73255b      ...%02x ...[%s]
6031653c:	6d6f6320 656e6f70 695f746e 6e692064      component_id in
6031654c:	696c6176 000a0d64                       valid...

60316554 <__func__.0>:
60316554:	736f7472 6972635f 61636974 78655f6c     rtos_critical_ex
60316564:	00007469                                it..

60316568 <__func__.1>:
60316568:	736f7472 6972635f 61636974 6e655f6c     rtos_critical_en
60316578:	00726574 695f7366 5f74696e 65726874     ter.fs_init_thre
60316588:	00006461 2a2a2a2a 2a2a2a2a 2a2a2a2a     ad..************
60316598:	2a2a2a2a 2a2a2a2a 2a2a2a2a 57202a2a     ************** W
603165a8:	494e5241 2a20474e 2a2a2a2a 2a2a2a2a     ARNING *********
603165b8:	2a2a2a2a 2a2a2a2a 2a2a2a2a 000a2a2a     **************..
603165c8:	4e49414d 00000000 61656c50 72206573     MAIN....Please r
603165d8:	20646165 20656874 6361433c 43206568     ead the <Cache C
603165e8:	69736e6f 6e657473 57207963 206e6568     onsistency When 
603165f8:	6e697355 4d442067 63203e41 74706168     Using DMA> chapt
60316608:	69207265 6874206e 70412065 63696c70     er in the Applic
60316618:	6f697461 6f4e5f6e 702e6574 000a6664     ation_Note.pdf..
60316628:	65726548 20736920 656d6f73 69726220     Here is some bri
60316638:	6f206665 68742066 68632065 65747061     ef of the chapte
60316648:	6e612072 61632064 6562206e 6d657220     r and can be rem
60316658:	6465766f 206e6920 6e69616d 000a632e     oved in main.c..
60316668:	74726f43 412d7865 77203233 206c6c69     Cortex-A32 will 
60316678:	6f747561 6974616d 20796c63 66657270     automaticly pref
60316688:	68637465 74616420 6f742061 632d4420     etch data to D-c
60316698:	65686361 65766520 6874206e 64612065     ache even the ad
603166a8:	73657264 73692073 746f6e20 61657220     dress is not rea
603166b8:	72772f64 21657469 0000000a 63614344     d/write!....DCac
603166c8:	495f6568 6c61766e 74616469 78302865     he_Invalidate(0x
603166d8:	46464646 46464646 7830202c 46464646     FFFFFFFF, 0xFFFF
603166e8:	46464646 73692029 6d617320 73612065     FFFF) is same as
603166f8:	61434420 5f656863 61656c43 766e496e      DCache_CleanInv
60316708:	64696c61 28657461 46467830 46464646     alidate(0xFFFFFF
60316718:	202c4646 46467830 46464646 20294646     FF, 0xFFFFFFFF) 
60316728:	43206e69 0a323341 00000000 61766e49     in CA32.....Inva
60316738:	6164696c 62206574 6e612079 64646120     lidate by an add
60316748:	73736572 20736920 796c6e6f 72657020     ress is only per
60316758:	6d726f66 77206465 206e6568 20656874     formed when the 
60316768:	61746164 20736920 20746f6e 74726964     data is not dirt
60316778:	6f202c79 65687472 73697772 20612065     y, ortherwise a 
60316788:	61656c63 7369206e 72657020 6d726f66     clean is perform
60316798:	0a216465 00000000 65666572 6f742072     ed!.....refer to
603167a8:	4d525420 20666f20 32334143 6f626120      TRM of CA32 abo
603167b8:	27207475 61746164 65727020 63746566     ut 'data prefetc
603167c8:	676e6968 6e612027 49272064 6c61766e     hing' and 'Inval
603167d8:	74616469 20676e69 6320726f 6e61656c     idating or clean
603167e8:	20676e69 61632061 27656863 00000a2e     ing a cache'....
603167f8:	2a2a2a2a 2a2a2a2a 2a2a2a2a 2a2a2a2a     ****************
60316808:	2a2a2a2a 2a2a2a2a 2a2a2a2a 2a2a2a2a     ****************
60316818:	2a2a2a2a 2a2a2a2a 2a2a2a2a 2a2a2a2a     ****************
60316828:	2a2a2a2a 2a2a2a2a 000a2a2a 74726f43     **********..Cort
60316838:	412d7865 61745320 53207472 64656863     ex-A Start Sched
60316848:	72656c75 0000000a 6d6f682f 75612f65     uler..../home/au
60316858:	6a5f6f74 696b6e65 775f736e 356e616c     to_jenkins_wlan5
60316868:	726f772f 6170736b 732f6563 612f6b64     /workspace/sdk/a
60316878:	2d6f7475 656c6572 2d657361 7473616d     uto-release-mast
60316888:	632f7265 6f706d6f 746e656e 2f736f2f     er/component/os/
60316898:	65657266 736f7472 6572662f 6f747265     freertos/freerto
603168a8:	31765f73 2e322e30 6d735f31 6f532f70     s_v10.2.1_smp/So
603168b8:	65637275 7361742f 632e736b 00000000     urce/tasks.c....
603168c8:	454c4449 00000000 6d6f682f 75612f65     IDLE..../home/au
603168d8:	6a5f6f74 696b6e65 775f736e 356e616c     to_jenkins_wlan5
603168e8:	726f772f 6170736b 732f6563 612f6b64     /workspace/sdk/a
603168f8:	2d6f7475 656c6572 2d657361 7473616d     uto-release-mast
60316908:	632f7265 6f706d6f 746e656e 2f736f2f     er/component/os/
60316918:	65657266 736f7472 6572662f 6f747265     freertos/freerto
60316928:	31765f73 2e322e30 6d735f31 6f532f70     s_v10.2.1_smp/So
60316938:	65637275 6575712f 632e6575 00000000     urce/queue.c....
60316948:	51726d54 00000000 20726d54 00637653     TmrQ....Tmr Svc.
60316958:	6d6f682f 75612f65 6a5f6f74 696b6e65     /home/auto_jenki
60316968:	775f736e 356e616c 726f772f 6170736b     ns_wlan5/workspa
60316978:	732f6563 612f6b64 2d6f7475 656c6572     ce/sdk/auto-rele
60316988:	2d657361 7473616d 632f7265 6f706d6f     ase-master/compo
60316998:	746e656e 2f736f2f 65657266 736f7472     nent/os/freertos
603169a8:	6572662f 6f747265 31765f73 2e322e30     /freertos_v10.2.
603169b8:	6d735f31 6f532f70 65637275 6d69742f     1_smp/Source/tim
603169c8:	2e737265 00000063 6d6f682f 75612f65     ers.c.../home/au
603169d8:	6a5f6f74 696b6e65 775f736e 356e616c     to_jenkins_wlan5
603169e8:	726f772f 6170736b 732f6563 612f6b64     /workspace/sdk/a
603169f8:	2d6f7475 656c6572 2d657361 7473616d     uto-release-mast
60316a08:	632f7265 6f706d6f 746e656e 2f736f2f     er/component/os/
60316a18:	65657266 736f7472 6572662f 6f747265     freertos/freerto
60316a28:	31765f73 2e322e30 6d735f31 6f532f70     s_v10.2.1_smp/So
60316a38:	65637275 726f702f 6c626174 654d2f65     urce/portable/Me
60316a48:	6e614d6d 65682f67 355f7061 0000632e     mMang/heap_5.c..
60316a58:	73546f4e 0000006b 6c6c614d 6620636f     NoTsk...Malloc f
60316a68:	656c6961 43202e64 3a65726f 5d73255b     ailed. Core:[%s]
60316a78:	6154202c 5b3a6b73 2c5d7325 72665b20     , Task:[%s], [fr
60316a88:	68206565 20706165 657a6973 6425203a     ee heap size: %d
60316a98:	785b205d 746e6157 69536465 253a657a     ] [xWantedSize:%
60316aa8:	0a0d5d64 00000000 32334143 00000000     d]......CA32....
60316ab8:	6d6f682f 75612f65 6a5f6f74 696b6e65     /home/auto_jenki
60316ac8:	775f736e 356e616c 726f772f 6170736b     ns_wlan5/workspa
60316ad8:	732f6563 612f6b64 2d6f7475 656c6572     ce/sdk/auto-rele
60316ae8:	2d657361 7473616d 632f7265 6f706d6f     ase-master/compo
60316af8:	746e656e 2f736f2f 65657266 736f7472     nent/os/freertos
60316b08:	6572662f 6f747265 31765f73 2e322e30     /freertos_v10.2.
60316b18:	6d735f31 6f532f70 65637275 726f702f     1_smp/Source/por
60316b28:	6c626174 43472f65 52412f43 41435f4d     table/GCC/ARM_CA
60316b38:	702f3233 2e74726f 00000063 54530d0a     32/port.c.....ST
60316b48:	204b4341 5245564f 574f4c46 54202d20     ACK OVERFLOW - T
60316b58:	4e6b7361 28656d61 0a297325 0000000d     askName(%s).....
60316b68:	00000042 00000041 44415041 73255020     B...A...APAD P%s
60316b78:	2064255f 63207369 69666e6f 65727567     _%d is configure
60316b88:	6f742064 6e756620 20444963 20756c25     d to funcID %lu 
60316b98:	0000000a 504d4f52 48435441 00000000     ....ROMPATCH....
60316ba8:	20445753 20444150 5f732550 69206425     SWD PAD P%s_%d i
60316bb8:	6f632073 6769666e 64657275 206f7420     s configured to 
60316bc8:	636e7566 25204449 0a20756c 00000000     funcID %lu .....
60316bd8:	64206f69 65766972 61702072 656d6172     io driver parame
60316be8:	73726574 72726520 2021726f 656c6966     ters error! file
60316bf8:	6d616e5f 25203a65 6c202c73 3a656e69     _name: %s, line:
60316c08:	0a642520 00000000 74697277 66652065      %d.....write ef
60316c18:	20657375 78257830 206f7420 78257830     use 0x%x to 0x%x
60316c28:	0000000a 206c6c61 20435243 72746e65     ....all CRC entr
60316c38:	73692079 65737520 00000a64 6f727245     y is used...Erro
60316c48:	53202172 6c756f68 656e2064 20726576     r! Should never 
60316c58:	63616572 65682068 0a216572 00000000     reach here!.....
60316c68:	00434d50 6d6f682f 75612f65 6a5f6f74     PMC./home/auto_j
60316c78:	696b6e65 775f736e 356e616c 726f772f     enkins_wlan5/wor
60316c88:	6170736b 732f6563 612f6b64 2d6f7475     kspace/sdk/auto-
60316c98:	656c6572 2d657361 7473616d 632f7265     release-master/c
60316ca8:	6f706d6f 746e656e 636f732f 656d612f     omponent/soc/ame
60316cb8:	6d736162 2f747261 2f6d6f72 5f6d6f72     basmart/rom/rom_
60316cc8:	6d6d6f63 612f6e6f 6162656d 6361625f     common/ameba_bac
60316cd8:	5f70756b 2e676572 00000063              kup_reg.c...

60316ce4 <LOG_UART_IDX_FLAG>:
60316ce4:	00000001 00200000 00020000 0000cb9d     ...... .........
60316cf4:	00000008 00000000 00100000 00010000     ................
60316d04:	000bee33 00000008 00000003 00800000     3...............
60316d14:	00080000 002fb8cc 00000008 6d6f682f     ....../...../hom
60316d24:	75612f65 6a5f6f74 696b6e65 775f736e     e/auto_jenkins_w
60316d34:	356e616c 726f772f 6170736b 732f6563     lan5/workspace/s
60316d44:	612f6b64 2d6f7475 656c6572 2d657361     dk/auto-release-
60316d54:	7473616d 632f7265 6f706d6f 746e656e     master/component
60316d64:	636f732f 656d612f 6d736162 2f747261     /soc/amebasmart/
60316d74:	2f6d6f72 5f6d6f72 6d6d6f63 612f6e6f     rom/rom_common/a
60316d84:	6162656d 6d69745f 6d6f725f 2c00632e     meba_tim_rom.c.,
60316d94:	6e756620 6f697463 00203a6e 65737361      function: .asse
60316da4:	6f697472 2522206e 66202273 656c6961     rtion "%s" faile
60316db4:	66203a64 20656c69 22732522 696c202c     d: file "%s", li
60316dc4:	2520656e 25732564 49000a73 6e69666e     ne %d%s%s..Infin
60316dd4:	00797469 004e614e 45520030 20544e45     ity.NaN.0.REENT 
60316de4:	6c6c616d 7320636f 65636375 64656465     malloc succeeded
60316df4:	73732f00 6f742f64 61677962 53502f6f     ./ssd/tobygao/PS
60316e04:	50555350 54524f50 3232332d 30342f30     PSUPPORT-3220/40
60316e14:	6e2f3237 696c7765 2e342d62 656e2f31     72/newlib-4.1/ne
60316e24:	62696c77 62696c2f 74732f63 62696c64     wlib/libc/stdlib
60316e34:	6f74642f 00632e61 6c6c6142 7320636f     /dtoa.c.Balloc s
60316e44:	65636375 64656465 50004300 5849534f     ucceeded.C.POSIX
60316e54:	2f002e00 2f647373 79626f74 2f6f6167     .../ssd/tobygao/
60316e64:	53505350 4f505055 332d5452 2f303232     PSPSUPPORT-3220/
60316e74:	32373034 77656e2f 2d62696c 2f312e34     4072/newlib-4.1/
60316e84:	6c77656e 6c2f6269 2f636269 6c647473     newlib/libc/stdl
60316e94:	6d2f6269 63657270 3000632e 34333231     ib/mprec.c.01234
60316ea4:	38373635 43424139 00464544 33323130     56789ABCDEF.0123
60316eb4:	37363534 62613938 66656463 464e4900     456789abcdef.INF
60316ec4:	666e6900 4e414e00 6e616e00 00000000     .inf.NAN.nan....

60316ed4 <shell_cmd_table>:
60316ed4:	60315664 00000000 6030786c 60315668     dV1`....lx0`hV1`
60316ee4:	60315690 00000004 60307b84 60315694     .V1`.....{0`.V1`
60316ef4:	603157b0 00000002 60307adc 603157b4     .W1`.....z0`.W1`
60316f04:	60315844 00000004 60307924 6031584c     DX1`....$y0`LX1`
60316f14:	6031588c 00000008 60307dd4 60315894     .X1`.....}0`.X1`
60316f24:	60315950 00000004 603079a8 60315958     PY1`.....y0`XY1`

60316f34 <ipc_shell_table>:
60316f34:	00000001 603084e0 00000000 603088f4     ......0`......0`
	...
60316f50:	00000001 603084e0 00000000 603088f4     ......0`......0`
60316f60:	00000000 00000001 00000000              ............

60316f6c <ipc_linux_table>:
60316f6c:	00000001 60309c94 00000000 603088f4     ......0`......0`
60316f7c:	00000000 00000021 00000006              ....!.......

60316f88 <ipc_LPWHP_table>:
60316f88:	00000001 6030cb20 00000000 603088f4     .... .0`......0`
60316f98:	00000000 00000001 00000001              ............

/home/auto_jenkins_wlan5/workspace/sdk/auto-release-master/amebasmart_gcc_project/project_ap/asdk/image_mp/target_img2.axf:     file format elf32-littlearm


Disassembly of section .data:

60317000 <_impure_ptr>:
60317000:	04 70 31 60                                         .p1`

60317004 <impure_data>:
60317004:	00 00 00 00 1c 73 31 60 84 73 31 60 ec 73 31 60     .....s1`.s1`.s1`
	...

603170f4 <__global_locale>:
603170f4:	43 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     C...............
	...
60317114:	43 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     C...............
	...
60317134:	43 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     C...............
	...
60317154:	43 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     C...............
	...
60317174:	43 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     C...............
	...
60317194:	43 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     C...............
	...
603171b4:	43 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     C...............
	...
603171d4:	a0 69 30 60 1c 2a 30 60 00 00 00 00 d5 50 31 60     .i0`.*0`.....P1`
603171e4:	55 6e 31 60 ce 6d 31 60 ce 6d 31 60 ce 6d 31 60     Un1`.m1`.m1`.m1`
603171f4:	ce 6d 31 60 ce 6d 31 60 ce 6d 31 60 ce 6d 31 60     .m1`.m1`.m1`.m1`
60317204:	ce 6d 31 60 ce 6d 31 60 ff ff ff ff ff ff ff ff     .m1`.m1`........
60317214:	ff ff ff ff ff ff 00 00 01 00 41 53 43 49 49 00     ..........ASCII.
	...
6031723c:	00 00 41 53 43 49 49 00 00 00 00 00 00 00 00 00     ..ASCII.........
	...

60317260 <tf_xlat_ctx>:
60317260:	ff ff ff ff 00 00 00 00 ff ff ff ff 00 7a 31 60     .............z1`
60317270:	10 00 00 00 00 d0 31 60 0a 00 00 00 00 00 00 00     ......1`........
60317280:	40 cf 31 60 04 00 00 00 00 00 00 00 00 00 00 00     @.1`............
60317290:	00 00 00 00 01 00 00 00 00 00 00 00 ff ff ff ff     ................

603172a0 <rtk_log_default_level>:
603172a0:	04 00 00 00                                         ....

603172a4 <deepwakelock>:
603172a4:	01 00 00 00                                         ....

603172a8 <system_can_yield>:
603172a8:	01 00 00 00                                         ....

603172ac <timer_max_sleep_time>:
603172ac:	ff ff ff ff                                         ....

603172b0 <timer_min_sleep_time>:
603172b0:	ff ff ff ff                                         ....

603172b4 <wakelock>:
603172b4:	01 00 00 00                                         ....

603172b8 <GPIO_PORTx>:
603172b8:	00 d0 00 42 00 d4 00 42 00 d8 00 42                 ...B...B...B
