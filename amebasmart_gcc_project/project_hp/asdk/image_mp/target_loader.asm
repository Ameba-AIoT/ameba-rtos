
/home/auto_jenkins_wlan5/workspace/sdk/auto-release-master/amebasmart_gcc_project/project_hp/asdk/image_mp/target_loader.axf:     file format elf32-littlearm


Disassembly of section .ram_image1.text:

30003050 <BOOT_SectionInit>:
30003050:	4800      	ldr	r0, [pc, #0]	; (30003054 <BOOT_SectionInit+0x4>)
30003052:	4770      	bx	lr
30003054:	60000020 	.word	0x60000020

30003058 <BOOT_NsStart>:
30003058:	b570      	push	{r4, r5, r6, lr}
3000305a:	4c23      	ldr	r4, [pc, #140]	; (300030e8 <BOOT_NsStart+0x90>)
3000305c:	4605      	mov	r5, r0
3000305e:	f384 8808 	msr	MSP, r4
30003062:	f44f 5280 	mov.w	r2, #4096	; 0x1000
30003066:	2100      	movs	r1, #0
30003068:	4620      	mov	r0, r4
3000306a:	4e20      	ldr	r6, [pc, #128]	; (300030ec <BOOT_NsStart+0x94>)
3000306c:	47b0      	blx	r6
3000306e:	4a20      	ldr	r2, [pc, #128]	; (300030f0 <BOOT_NsStart+0x98>)
30003070:	6953      	ldr	r3, [r2, #20]
30003072:	03d9      	lsls	r1, r3, #15
30003074:	d50b      	bpl.n	3000308e <BOOT_NsStart+0x36>
30003076:	f3bf 8f4f 	dsb	sy
3000307a:	f8c2 4270 	str.w	r4, [r2, #624]	; 0x270
3000307e:	3420      	adds	r4, #32
30003080:	f1b4 2f30 	cmp.w	r4, #805318656	; 0x30003000
30003084:	d1f9      	bne.n	3000307a <BOOT_NsStart+0x22>
30003086:	f3bf 8f4f 	dsb	sy
3000308a:	f3bf 8f6f 	isb	sy
3000308e:	f04f 2330 	mov.w	r3, #805318656	; 0x30003000
30003092:	f383 8808 	msr	MSP, r3
30003096:	f44f 5280 	mov.w	r2, #4096	; 0x1000
3000309a:	2100      	movs	r1, #0
3000309c:	4815      	ldr	r0, [pc, #84]	; (300030f4 <BOOT_NsStart+0x9c>)
3000309e:	47b0      	blx	r6
300030a0:	4a13      	ldr	r2, [pc, #76]	; (300030f0 <BOOT_NsStart+0x98>)
300030a2:	6953      	ldr	r3, [r2, #20]
300030a4:	03db      	lsls	r3, r3, #15
300030a6:	d50c      	bpl.n	300030c2 <BOOT_NsStart+0x6a>
300030a8:	f3bf 8f4f 	dsb	sy
300030ac:	4b11      	ldr	r3, [pc, #68]	; (300030f4 <BOOT_NsStart+0x9c>)
300030ae:	490e      	ldr	r1, [pc, #56]	; (300030e8 <BOOT_NsStart+0x90>)
300030b0:	f8c2 3270 	str.w	r3, [r2, #624]	; 0x270
300030b4:	3320      	adds	r3, #32
300030b6:	428b      	cmp	r3, r1
300030b8:	d1fa      	bne.n	300030b0 <BOOT_NsStart+0x58>
300030ba:	f3bf 8f4f 	dsb	sy
300030be:	f3bf 8f6f 	isb	sy
300030c2:	f025 0501 	bic.w	r5, r5, #1
300030c6:	086d      	lsrs	r5, r5, #1
300030c8:	006d      	lsls	r5, r5, #1
300030ca:	e92d 0ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
300030ce:	ed2d 8b10 	vpush	{d8-d15}
300030d2:	ec9f 0a20 	vscclrm	{s0-s31, VPR}
300030d6:	e89f 9fdf 	clrm	{r0, r1, r2, r3, r4, r6, r7, r8, r9, sl, fp, ip, APSR}
300030da:	47ac      	blxns	r5
300030dc:	ecbd 8b10 	vpop	{d8-d15}
300030e0:	e8bd 0ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
300030e4:	bd70      	pop	{r4, r5, r6, pc}
300030e6:	bf00      	nop
300030e8:	30002000 	.word	0x30002000
300030ec:	00012be5 	.word	0x00012be5
300030f0:	e000ed00 	.word	0xe000ed00
300030f4:	30001000 	.word	0x30001000

300030f8 <BOOT_RccConfig>:
300030f8:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
300030fc:	4c2f      	ldr	r4, [pc, #188]	; (300031bc <BOOT_RccConfig+0xc4>)
300030fe:	b090      	sub	sp, #64	; 0x40
30003100:	4f2f      	ldr	r7, [pc, #188]	; (300031c0 <BOOT_RccConfig+0xc8>)
30003102:	466d      	mov	r5, sp
30003104:	f10d 0c20 	add.w	ip, sp, #32
30003108:	683e      	ldr	r6, [r7, #0]
3000310a:	f10d 0e30 	add.w	lr, sp, #48	; 0x30
3000310e:	e894 000f 	ldmia.w	r4, {r0, r1, r2, r3}
30003112:	e885 000f 	stmia.w	r5, {r0, r1, r2, r3}
30003116:	f104 0310 	add.w	r3, r4, #16
3000311a:	2400      	movs	r4, #0
3000311c:	cb0f      	ldmia	r3, {r0, r1, r2, r3}
3000311e:	e88c 000f 	stmia.w	ip, {r0, r1, r2, r3}
30003122:	1c72      	adds	r2, r6, #1
30003124:	e9cd 4404 	strd	r4, r4, [sp, #16]
30003128:	e9cd 4406 	strd	r4, r4, [sp, #24]
3000312c:	e9cd 440c 	strd	r4, r4, [sp, #48]	; 0x30
30003130:	e9cd 440e 	strd	r4, r4, [sp, #56]	; 0x38
30003134:	d03f      	beq.n	300031b6 <BOOT_RccConfig+0xbe>
30003136:	463a      	mov	r2, r7
30003138:	6891      	ldr	r1, [r2, #8]
3000313a:	0fb3      	lsrs	r3, r6, #30
3000313c:	a810      	add	r0, sp, #64	; 0x40
3000313e:	2901      	cmp	r1, #1
30003140:	eb00 0383 	add.w	r3, r0, r3, lsl #2
30003144:	d10e      	bne.n	30003164 <BOOT_RccConfig+0x6c>
30003146:	f853 0c30 	ldr.w	r0, [r3, #-48]
3000314a:	6854      	ldr	r4, [r2, #4]
3000314c:	4330      	orrs	r0, r6
3000314e:	ae10      	add	r6, sp, #64	; 0x40
30003150:	0fa1      	lsrs	r1, r4, #30
30003152:	f843 0c30 	str.w	r0, [r3, #-48]
30003156:	eb06 0181 	add.w	r1, r6, r1, lsl #2
3000315a:	f851 3c10 	ldr.w	r3, [r1, #-16]
3000315e:	4323      	orrs	r3, r4
30003160:	f841 3c10 	str.w	r3, [r1, #-16]
30003164:	f852 6f0c 	ldr.w	r6, [r2, #12]!
30003168:	1c73      	adds	r3, r6, #1
3000316a:	d1e5      	bne.n	30003138 <BOOT_RccConfig+0x40>
3000316c:	9b0c      	ldr	r3, [sp, #48]	; 0x30
3000316e:	9c04      	ldr	r4, [sp, #16]
30003170:	af0d      	add	r7, sp, #52	; 0x34
30003172:	ae05      	add	r6, sp, #20
30003174:	4a13      	ldr	r2, [pc, #76]	; (300031c4 <BOOT_RccConfig+0xcc>)
30003176:	f85c 0b04 	ldr.w	r0, [ip], #4
3000317a:	f023 4340 	bic.w	r3, r3, #3221225472	; 0xc0000000
3000317e:	f855 1b04 	ldr.w	r1, [r5], #4
30003182:	f024 4440 	bic.w	r4, r4, #3221225472	; 0xc0000000
30003186:	f850 8002 	ldr.w	r8, [r0, r2]
3000318a:	45e6      	cmp	lr, ip
3000318c:	ea43 0308 	orr.w	r3, r3, r8
30003190:	5083      	str	r3, [r0, r2]
30003192:	588b      	ldr	r3, [r1, r2]
30003194:	ea44 0403 	orr.w	r4, r4, r3
30003198:	508c      	str	r4, [r1, r2]
3000319a:	d004      	beq.n	300031a6 <BOOT_RccConfig+0xae>
3000319c:	f857 3b04 	ldr.w	r3, [r7], #4
300031a0:	f856 4b04 	ldr.w	r4, [r6], #4
300031a4:	e7e7      	b.n	30003176 <BOOT_RccConfig+0x7e>
300031a6:	200a      	movs	r0, #10
300031a8:	4b07      	ldr	r3, [pc, #28]	; (300031c8 <BOOT_RccConfig+0xd0>)
300031aa:	4798      	blx	r3
300031ac:	4b07      	ldr	r3, [pc, #28]	; (300031cc <BOOT_RccConfig+0xd4>)
300031ae:	b010      	add	sp, #64	; 0x40
300031b0:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
300031b4:	4718      	bx	r3
300031b6:	4623      	mov	r3, r4
300031b8:	e7da      	b.n	30003170 <BOOT_RccConfig+0x78>
300031ba:	bf00      	nop
300031bc:	3000cb00 	.word	0x3000cb00
300031c0:	3000efa8 	.word	0x3000efa8
300031c4:	42008000 	.word	0x42008000
300031c8:	00009b2d 	.word	0x00009b2d
300031cc:	30009175 	.word	0x30009175

300031d0 <BOOT_CACHEWRR_Set>:
300031d0:	4b14      	ldr	r3, [pc, #80]	; (30003224 <BOOT_CACHEWRR_Set+0x54>)
300031d2:	b430      	push	{r4, r5}
300031d4:	681a      	ldr	r2, [r3, #0]
300031d6:	2a01      	cmp	r2, #1
300031d8:	d10f      	bne.n	300031fa <BOOT_CACHEWRR_Set+0x2a>
300031da:	f04f 20e0 	mov.w	r0, #3758153728	; 0xe000e000
300031de:	685d      	ldr	r5, [r3, #4]
300031e0:	689c      	ldr	r4, [r3, #8]
300031e2:	6882      	ldr	r2, [r0, #8]
300031e4:	4910      	ldr	r1, [pc, #64]	; (30003228 <BOOT_CACHEWRR_Set+0x58>)
300031e6:	f422 5240 	bic.w	r2, r2, #12288	; 0x3000
300031ea:	ea42 3205 	orr.w	r2, r2, r5, lsl #12
300031ee:	6082      	str	r2, [r0, #8]
300031f0:	f8c1 40a0 	str.w	r4, [r1, #160]	; 0xa0
300031f4:	68da      	ldr	r2, [r3, #12]
300031f6:	f8c1 20a4 	str.w	r2, [r1, #164]	; 0xa4
300031fa:	691a      	ldr	r2, [r3, #16]
300031fc:	2a01      	cmp	r2, #1
300031fe:	d10f      	bne.n	30003220 <BOOT_CACHEWRR_Set+0x50>
30003200:	f04f 21e0 	mov.w	r1, #3758153728	; 0xe000e000
30003204:	695d      	ldr	r5, [r3, #20]
30003206:	4a08      	ldr	r2, [pc, #32]	; (30003228 <BOOT_CACHEWRR_Set+0x58>)
30003208:	e9d3 4006 	ldrd	r4, r0, [r3, #24]
3000320c:	688b      	ldr	r3, [r1, #8]
3000320e:	f423 1340 	bic.w	r3, r3, #3145728	; 0x300000
30003212:	ea43 5305 	orr.w	r3, r3, r5, lsl #20
30003216:	608b      	str	r3, [r1, #8]
30003218:	f8c2 40a8 	str.w	r4, [r2, #168]	; 0xa8
3000321c:	f8c2 00ac 	str.w	r0, [r2, #172]	; 0xac
30003220:	bc30      	pop	{r4, r5}
30003222:	4770      	bx	lr
30003224:	3000ef84 	.word	0x3000ef84
30003228:	e0042000 	.word	0xe0042000

3000322c <BOOT_TCMSet>:
3000322c:	2300      	movs	r3, #0
3000322e:	b5f0      	push	{r4, r5, r6, r7, lr}
30003230:	4d82      	ldr	r5, [pc, #520]	; (3000343c <BOOT_TCMSet+0x210>)
30003232:	461c      	mov	r4, r3
30003234:	f855 2023 	ldr.w	r2, [r5, r3, lsl #2]
30003238:	3307      	adds	r3, #7
3000323a:	4282      	cmp	r2, r0
3000323c:	d003      	beq.n	30003246 <BOOT_TCMSet+0x1a>
3000323e:	3401      	adds	r4, #1
30003240:	2c07      	cmp	r4, #7
30003242:	d1f7      	bne.n	30003234 <BOOT_TCMSet+0x8>
30003244:	bdf0      	pop	{r4, r5, r6, r7, pc}
30003246:	2c00      	cmp	r4, #0
30003248:	d0fc      	beq.n	30003244 <BOOT_TCMSet+0x18>
3000324a:	f3bf 8f4f 	dsb	sy
3000324e:	f3bf 8f6f 	isb	sy
30003252:	487b      	ldr	r0, [pc, #492]	; (30003440 <BOOT_TCMSet+0x214>)
30003254:	2300      	movs	r3, #0
30003256:	f8c0 3250 	str.w	r3, [r0, #592]	; 0x250
3000325a:	f3bf 8f4f 	dsb	sy
3000325e:	f3bf 8f6f 	isb	sy
30003262:	6942      	ldr	r2, [r0, #20]
30003264:	03d1      	lsls	r1, r2, #15
30003266:	d520      	bpl.n	300032aa <BOOT_TCMSet+0x7e>
30003268:	f8c0 3084 	str.w	r3, [r0, #132]	; 0x84
3000326c:	f3bf 8f4f 	dsb	sy
30003270:	f8d0 3080 	ldr.w	r3, [r0, #128]	; 0x80
30003274:	f643 76e0 	movw	r6, #16352	; 0x3fe0
30003278:	f3c3 0ec9 	ubfx	lr, r3, #3, #10
3000327c:	f3c3 334e 	ubfx	r3, r3, #13, #15
30003280:	ea4f 1c43 	mov.w	ip, r3, lsl #5
30003284:	ea0c 0106 	and.w	r1, ip, r6
30003288:	4673      	mov	r3, lr
3000328a:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
3000328e:	3b01      	subs	r3, #1
30003290:	f8c0 2274 	str.w	r2, [r0, #628]	; 0x274
30003294:	1c5a      	adds	r2, r3, #1
30003296:	d1f8      	bne.n	3000328a <BOOT_TCMSet+0x5e>
30003298:	f1ac 0c20 	sub.w	ip, ip, #32
3000329c:	f11c 0f20 	cmn.w	ip, #32
300032a0:	d1f0      	bne.n	30003284 <BOOT_TCMSet+0x58>
300032a2:	f3bf 8f4f 	dsb	sy
300032a6:	f3bf 8f6f 	isb	sy
300032aa:	f3bf 8f4f 	dsb	sy
300032ae:	f3bf 8f6f 	isb	sy
300032b2:	4863      	ldr	r0, [pc, #396]	; (30003440 <BOOT_TCMSet+0x214>)
300032b4:	2200      	movs	r2, #0
300032b6:	6943      	ldr	r3, [r0, #20]
300032b8:	f423 3300 	bic.w	r3, r3, #131072	; 0x20000
300032bc:	6143      	str	r3, [r0, #20]
300032be:	f8c0 2250 	str.w	r2, [r0, #592]	; 0x250
300032c2:	f3bf 8f4f 	dsb	sy
300032c6:	f3bf 8f6f 	isb	sy
300032ca:	f8c0 2084 	str.w	r2, [r0, #132]	; 0x84
300032ce:	f3bf 8f4f 	dsb	sy
300032d2:	6943      	ldr	r3, [r0, #20]
300032d4:	f423 3380 	bic.w	r3, r3, #65536	; 0x10000
300032d8:	6143      	str	r3, [r0, #20]
300032da:	f3bf 8f4f 	dsb	sy
300032de:	f8d0 3080 	ldr.w	r3, [r0, #128]	; 0x80
300032e2:	f643 7ee0 	movw	lr, #16352	; 0x3fe0
300032e6:	f3c3 06c9 	ubfx	r6, r3, #3, #10
300032ea:	f3c3 334e 	ubfx	r3, r3, #13, #15
300032ee:	ea4f 1c43 	mov.w	ip, r3, lsl #5
300032f2:	ea0c 010e 	and.w	r1, ip, lr
300032f6:	4633      	mov	r3, r6
300032f8:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
300032fc:	3b01      	subs	r3, #1
300032fe:	1c5f      	adds	r7, r3, #1
30003300:	f8c0 2274 	str.w	r2, [r0, #628]	; 0x274
30003304:	d1f8      	bne.n	300032f8 <BOOT_TCMSet+0xcc>
30003306:	f1ac 0c20 	sub.w	ip, ip, #32
3000330a:	f11c 0f20 	cmn.w	ip, #32
3000330e:	d1f0      	bne.n	300032f2 <BOOT_TCMSet+0xc6>
30003310:	f3bf 8f4f 	dsb	sy
30003314:	f3bf 8f6f 	isb	sy
30003318:	f04f 4082 	mov.w	r0, #1090519040	; 0x41000000
3000331c:	ebc4 02c4 	rsb	r2, r4, r4, lsl #3
30003320:	00e3      	lsls	r3, r4, #3
30003322:	6b41      	ldr	r1, [r0, #52]	; 0x34
30003324:	eb05 0282 	add.w	r2, r5, r2, lsl #2
30003328:	f441 5180 	orr.w	r1, r1, #4096	; 0x1000
3000332c:	6341      	str	r1, [r0, #52]	; 0x34
3000332e:	6911      	ldr	r1, [r2, #16]
30003330:	2903      	cmp	r1, #3
30003332:	d00f      	beq.n	30003354 <BOOT_TCMSet+0x128>
30003334:	f04f 26e0 	mov.w	r6, #3758153728	; 0xe000e000
30003338:	4842      	ldr	r0, [pc, #264]	; (30003444 <BOOT_TCMSet+0x218>)
3000333a:	f04f 0c00 	mov.w	ip, #0
3000333e:	4f42      	ldr	r7, [pc, #264]	; (30003448 <BOOT_TCMSet+0x21c>)
30003340:	68b2      	ldr	r2, [r6, #8]
30003342:	f422 1240 	bic.w	r2, r2, #3145728	; 0x300000
30003346:	ea42 5201 	orr.w	r2, r2, r1, lsl #20
3000334a:	60b2      	str	r2, [r6, #8]
3000334c:	f8c0 c0a8 	str.w	ip, [r0, #168]	; 0xa8
30003350:	f8c0 70ac 	str.w	r7, [r0, #172]	; 0xac
30003354:	1b1a      	subs	r2, r3, r4
30003356:	eb05 0282 	add.w	r2, r5, r2, lsl #2
3000335a:	68d1      	ldr	r1, [r2, #12]
3000335c:	2903      	cmp	r1, #3
3000335e:	d00f      	beq.n	30003380 <BOOT_TCMSet+0x154>
30003360:	f04f 26e0 	mov.w	r6, #3758153728	; 0xe000e000
30003364:	4837      	ldr	r0, [pc, #220]	; (30003444 <BOOT_TCMSet+0x218>)
30003366:	f04f 0c00 	mov.w	ip, #0
3000336a:	4f37      	ldr	r7, [pc, #220]	; (30003448 <BOOT_TCMSet+0x21c>)
3000336c:	68b2      	ldr	r2, [r6, #8]
3000336e:	f422 5240 	bic.w	r2, r2, #12288	; 0x3000
30003372:	ea42 3201 	orr.w	r2, r2, r1, lsl #12
30003376:	60b2      	str	r2, [r6, #8]
30003378:	f8c0 c0a0 	str.w	ip, [r0, #160]	; 0xa0
3000337c:	f8c0 70a4 	str.w	r7, [r0, #164]	; 0xa4
30003380:	1b1a      	subs	r2, r3, r4
30003382:	4930      	ldr	r1, [pc, #192]	; (30003444 <BOOT_TCMSet+0x218>)
30003384:	eb05 0282 	add.w	r2, r5, r2, lsl #2
30003388:	6950      	ldr	r0, [r2, #20]
3000338a:	6608      	str	r0, [r1, #96]	; 0x60
3000338c:	6990      	ldr	r0, [r2, #24]
3000338e:	6648      	str	r0, [r1, #100]	; 0x64
30003390:	6852      	ldr	r2, [r2, #4]
30003392:	2a01      	cmp	r2, #1
30003394:	d032      	beq.n	300033fc <BOOT_TCMSet+0x1d0>
30003396:	1b1b      	subs	r3, r3, r4
30003398:	eb05 0583 	add.w	r5, r5, r3, lsl #2
3000339c:	68ab      	ldr	r3, [r5, #8]
3000339e:	2b01      	cmp	r3, #1
300033a0:	f47f af50 	bne.w	30003244 <BOOT_TCMSet+0x18>
300033a4:	4826      	ldr	r0, [pc, #152]	; (30003440 <BOOT_TCMSet+0x214>)
300033a6:	6943      	ldr	r3, [r0, #20]
300033a8:	f413 3380 	ands.w	r3, r3, #65536	; 0x10000
300033ac:	f47f af4a 	bne.w	30003244 <BOOT_TCMSet+0x18>
300033b0:	f8c0 3084 	str.w	r3, [r0, #132]	; 0x84
300033b4:	f3bf 8f4f 	dsb	sy
300033b8:	f8d0 5080 	ldr.w	r5, [r0, #128]	; 0x80
300033bc:	f643 76e0 	movw	r6, #16352	; 0x3fe0
300033c0:	f3c5 344e 	ubfx	r4, r5, #13, #15
300033c4:	f3c5 05c9 	ubfx	r5, r5, #3, #10
300033c8:	0164      	lsls	r4, r4, #5
300033ca:	ea04 0106 	and.w	r1, r4, r6
300033ce:	462b      	mov	r3, r5
300033d0:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
300033d4:	3b01      	subs	r3, #1
300033d6:	f8c0 2260 	str.w	r2, [r0, #608]	; 0x260
300033da:	1c5a      	adds	r2, r3, #1
300033dc:	d1f8      	bne.n	300033d0 <BOOT_TCMSet+0x1a4>
300033de:	3c20      	subs	r4, #32
300033e0:	f114 0f20 	cmn.w	r4, #32
300033e4:	d1f1      	bne.n	300033ca <BOOT_TCMSet+0x19e>
300033e6:	f3bf 8f4f 	dsb	sy
300033ea:	6943      	ldr	r3, [r0, #20]
300033ec:	f443 3380 	orr.w	r3, r3, #65536	; 0x10000
300033f0:	6143      	str	r3, [r0, #20]
300033f2:	f3bf 8f4f 	dsb	sy
300033f6:	f3bf 8f6f 	isb	sy
300033fa:	bdf0      	pop	{r4, r5, r6, r7, pc}
300033fc:	4910      	ldr	r1, [pc, #64]	; (30003440 <BOOT_TCMSet+0x214>)
300033fe:	694a      	ldr	r2, [r1, #20]
30003400:	f412 3200 	ands.w	r2, r2, #131072	; 0x20000
30003404:	d1c7      	bne.n	30003396 <BOOT_TCMSet+0x16a>
30003406:	f3bf 8f4f 	dsb	sy
3000340a:	f3bf 8f6f 	isb	sy
3000340e:	f8c1 2250 	str.w	r2, [r1, #592]	; 0x250
30003412:	f3bf 8f4f 	dsb	sy
30003416:	f3bf 8f6f 	isb	sy
3000341a:	694a      	ldr	r2, [r1, #20]
3000341c:	f442 3200 	orr.w	r2, r2, #131072	; 0x20000
30003420:	614a      	str	r2, [r1, #20]
30003422:	f3bf 8f4f 	dsb	sy
30003426:	f3bf 8f6f 	isb	sy
3000342a:	1b1b      	subs	r3, r3, r4
3000342c:	eb05 0583 	add.w	r5, r5, r3, lsl #2
30003430:	68ab      	ldr	r3, [r5, #8]
30003432:	2b01      	cmp	r3, #1
30003434:	f47f af06 	bne.w	30003244 <BOOT_TCMSet+0x18>
30003438:	e7b4      	b.n	300033a4 <BOOT_TCMSet+0x178>
3000343a:	bf00      	nop
3000343c:	3000eec0 	.word	0x3000eec0
30003440:	e000ed00 	.word	0xe000ed00
30003444:	e0042000 	.word	0xe0042000
30003448:	ffff0000 	.word	0xffff0000

3000344c <BOOT_GRstConfig>:
3000344c:	b538      	push	{r3, r4, r5, lr}
3000344e:	4d0a      	ldr	r5, [pc, #40]	; (30003478 <BOOT_GRstConfig+0x2c>)
30003450:	f241 040a 	movw	r4, #4106	; 0x100a
30003454:	2028      	movs	r0, #40	; 0x28
30003456:	4a09      	ldr	r2, [pc, #36]	; (3000347c <BOOT_GRstConfig+0x30>)
30003458:	882b      	ldrh	r3, [r5, #0]
3000345a:	b29b      	uxth	r3, r3
3000345c:	431c      	orrs	r4, r3
3000345e:	802c      	strh	r4, [r5, #0]
30003460:	4790      	blx	r2
30003462:	88ab      	ldrh	r3, [r5, #4]
30003464:	b29b      	uxth	r3, r3
30003466:	ea23 0304 	bic.w	r3, r3, r4
3000346a:	80ab      	strh	r3, [r5, #4]
3000346c:	886b      	ldrh	r3, [r5, #2]
3000346e:	b29b      	uxth	r3, r3
30003470:	431c      	orrs	r4, r3
30003472:	806c      	strh	r4, [r5, #2]
30003474:	bd38      	pop	{r3, r4, r5, pc}
30003476:	bf00      	nop
30003478:	42008200 	.word	0x42008200
3000347c:	00009b2d 	.word	0x00009b2d

30003480 <BOOT_DDR_LCDC_HPR>:
30003480:	4a04      	ldr	r2, [pc, #16]	; (30003494 <BOOT_DDR_LCDC_HPR+0x14>)
30003482:	f8d2 3290 	ldr.w	r3, [r2, #656]	; 0x290
30003486:	f423 6340 	bic.w	r3, r3, #3072	; 0xc00
3000348a:	f443 6300 	orr.w	r3, r3, #2048	; 0x800
3000348e:	f8c2 3290 	str.w	r3, [r2, #656]	; 0x290
30003492:	4770      	bx	lr
30003494:	42008000 	.word	0x42008000

30003498 <BOOT_DDR_Init>:
30003498:	f04f 4382 	mov.w	r3, #1090519040	; 0x41000000
3000349c:	b530      	push	{r4, r5, lr}
3000349e:	4d15      	ldr	r5, [pc, #84]	; (300034f4 <BOOT_DDR_Init+0x5c>)
300034a0:	b099      	sub	sp, #100	; 0x64
300034a2:	6b1c      	ldr	r4, [r3, #48]	; 0x30
300034a4:	47a8      	blx	r5
300034a6:	2803      	cmp	r0, #3
300034a8:	d11e      	bne.n	300034e8 <BOOT_DDR_Init+0x50>
300034aa:	f044 74c0 	orr.w	r4, r4, #25165824	; 0x1800000
300034ae:	4a12      	ldr	r2, [pc, #72]	; (300034f8 <BOOT_DDR_Init+0x60>)
300034b0:	f04f 4382 	mov.w	r3, #1090519040	; 0x41000000
300034b4:	4911      	ldr	r1, [pc, #68]	; (300034fc <BOOT_DDR_Init+0x64>)
300034b6:	a801      	add	r0, sp, #4
300034b8:	631c      	str	r4, [r3, #48]	; 0x30
300034ba:	f8d2 3204 	ldr.w	r3, [r2, #516]	; 0x204
300034be:	f423 5380 	bic.w	r3, r3, #4096	; 0x1000
300034c2:	f8c2 3204 	str.w	r3, [r2, #516]	; 0x204
300034c6:	4788      	blx	r1
300034c8:	a801      	add	r0, sp, #4
300034ca:	4b0d      	ldr	r3, [pc, #52]	; (30003500 <BOOT_DDR_Init+0x68>)
300034cc:	4798      	blx	r3
300034ce:	4b0d      	ldr	r3, [pc, #52]	; (30003504 <BOOT_DDR_Init+0x6c>)
300034d0:	4798      	blx	r3
300034d2:	4b0d      	ldr	r3, [pc, #52]	; (30003508 <BOOT_DDR_Init+0x70>)
300034d4:	4798      	blx	r3
300034d6:	4b0d      	ldr	r3, [pc, #52]	; (3000350c <BOOT_DDR_Init+0x74>)
300034d8:	4798      	blx	r3
300034da:	2101      	movs	r1, #1
300034dc:	f240 10ff 	movw	r0, #511	; 0x1ff
300034e0:	4b0b      	ldr	r3, [pc, #44]	; (30003510 <BOOT_DDR_Init+0x78>)
300034e2:	4798      	blx	r3
300034e4:	b019      	add	sp, #100	; 0x64
300034e6:	bd30      	pop	{r4, r5, pc}
300034e8:	47a8      	blx	r5
300034ea:	2802      	cmp	r0, #2
300034ec:	d0dd      	beq.n	300034aa <BOOT_DDR_Init+0x12>
300034ee:	f044 7480 	orr.w	r4, r4, #16777216	; 0x1000000
300034f2:	e7dc      	b.n	300034ae <BOOT_DDR_Init+0x16>
300034f4:	0000c0f9 	.word	0x0000c0f9
300034f8:	42008000 	.word	0x42008000
300034fc:	30008aed 	.word	0x30008aed
30003500:	30009065 	.word	0x30009065
30003504:	30008a2d 	.word	0x30008a2d
30003508:	30009135 	.word	0x30009135
3000350c:	30009139 	.word	0x30009139
30003510:	30008999 	.word	0x30008999

30003514 <BOOT_PSRAM_Init>:
30003514:	b530      	push	{r4, r5, lr}
30003516:	b089      	sub	sp, #36	; 0x24
30003518:	4b1d      	ldr	r3, [pc, #116]	; (30003590 <BOOT_PSRAM_Init+0x7c>)
3000351a:	4c1e      	ldr	r4, [pc, #120]	; (30003594 <BOOT_PSRAM_Init+0x80>)
3000351c:	a801      	add	r0, sp, #4
3000351e:	4798      	blx	r3
30003520:	a801      	add	r0, sp, #4
30003522:	4b1d      	ldr	r3, [pc, #116]	; (30003598 <BOOT_PSRAM_Init+0x84>)
30003524:	4798      	blx	r3
30003526:	4b1d      	ldr	r3, [pc, #116]	; (3000359c <BOOT_PSRAM_Init+0x88>)
30003528:	4798      	blx	r3
3000352a:	6823      	ldr	r3, [r4, #0]
3000352c:	2b01      	cmp	r3, #1
3000352e:	d01a      	beq.n	30003566 <BOOT_PSRAM_Init+0x52>
30003530:	4b1b      	ldr	r3, [pc, #108]	; (300035a0 <BOOT_PSRAM_Init+0x8c>)
30003532:	2249      	movs	r2, #73	; 0x49
30003534:	491b      	ldr	r1, [pc, #108]	; (300035a4 <BOOT_PSRAM_Init+0x90>)
30003536:	2004      	movs	r0, #4
30003538:	f006 feee 	bl	3000a318 <rtk_log_write>
3000353c:	4b1a      	ldr	r3, [pc, #104]	; (300035a8 <BOOT_PSRAM_Init+0x94>)
3000353e:	4798      	blx	r3
30003540:	4b1a      	ldr	r3, [pc, #104]	; (300035ac <BOOT_PSRAM_Init+0x98>)
30003542:	f8d3 3268 	ldr.w	r3, [r3, #616]	; 0x268
30003546:	009b      	lsls	r3, r3, #2
30003548:	d514      	bpl.n	30003574 <BOOT_PSRAM_Init+0x60>
3000354a:	4a19      	ldr	r2, [pc, #100]	; (300035b0 <BOOT_PSRAM_Init+0x9c>)
3000354c:	4b19      	ldr	r3, [pc, #100]	; (300035b4 <BOOT_PSRAM_Init+0xa0>)
3000354e:	6812      	ldr	r2, [r2, #0]
30003550:	605a      	str	r2, [r3, #4]
30003552:	6962      	ldr	r2, [r4, #20]
30003554:	2020      	movs	r0, #32
30003556:	2101      	movs	r1, #1
30003558:	4b17      	ldr	r3, [pc, #92]	; (300035b8 <BOOT_PSRAM_Init+0xa4>)
3000355a:	fbb0 f2f2 	udiv	r2, r0, r2
3000355e:	4608      	mov	r0, r1
30003560:	4798      	blx	r3
30003562:	b009      	add	sp, #36	; 0x24
30003564:	bd30      	pop	{r4, r5, pc}
30003566:	4b15      	ldr	r3, [pc, #84]	; (300035bc <BOOT_PSRAM_Init+0xa8>)
30003568:	4798      	blx	r3
3000356a:	4b10      	ldr	r3, [pc, #64]	; (300035ac <BOOT_PSRAM_Init+0x98>)
3000356c:	f8d3 3268 	ldr.w	r3, [r3, #616]	; 0x268
30003570:	009b      	lsls	r3, r3, #2
30003572:	d4ea      	bmi.n	3000354a <BOOT_PSRAM_Init+0x36>
30003574:	4d12      	ldr	r5, [pc, #72]	; (300035c0 <BOOT_PSRAM_Init+0xac>)
30003576:	47a8      	blx	r5
30003578:	b930      	cbnz	r0, 30003588 <BOOT_PSRAM_Init+0x74>
3000357a:	4b12      	ldr	r3, [pc, #72]	; (300035c4 <BOOT_PSRAM_Init+0xb0>)
3000357c:	4798      	blx	r3
3000357e:	4a0d      	ldr	r2, [pc, #52]	; (300035b4 <BOOT_PSRAM_Init+0xa0>)
30003580:	4b0b      	ldr	r3, [pc, #44]	; (300035b0 <BOOT_PSRAM_Init+0x9c>)
30003582:	6852      	ldr	r2, [r2, #4]
30003584:	601a      	str	r2, [r3, #0]
30003586:	e7e4      	b.n	30003552 <BOOT_PSRAM_Init+0x3e>
30003588:	47a8      	blx	r5
3000358a:	2801      	cmp	r0, #1
3000358c:	d1e1      	bne.n	30003552 <BOOT_PSRAM_Init+0x3e>
3000358e:	e7f4      	b.n	3000357a <BOOT_PSRAM_Init+0x66>
30003590:	30009b01 	.word	0x30009b01
30003594:	3000f8f8 	.word	0x3000f8f8
30003598:	30009b21 	.word	0x30009b21
3000359c:	30009b89 	.word	0x30009b89
300035a0:	3000c7b8 	.word	0x3000c7b8
300035a4:	3000c7c4 	.word	0x3000c7c4
300035a8:	30009e2d 	.word	0x30009e2d
300035ac:	42008000 	.word	0x42008000
300035b0:	3000f13c 	.word	0x3000f13c
300035b4:	41002000 	.word	0x41002000
300035b8:	3000a005 	.word	0x3000a005
300035bc:	30009d55 	.word	0x30009d55
300035c0:	0000c0f9 	.word	0x0000c0f9
300035c4:	30009e69 	.word	0x30009e69

300035c8 <BOOT_WakeFromPG>:
300035c8:	b530      	push	{r4, r5, lr}
300035ca:	4c4a      	ldr	r4, [pc, #296]	; (300036f4 <BOOT_WakeFromPG+0x12c>)
300035cc:	b083      	sub	sp, #12
300035ce:	f000 fef3 	bl	300043b8 <BOOT_RAM_TZCfg>
300035d2:	69a3      	ldr	r3, [r4, #24]
300035d4:	b10b      	cbz	r3, 300035da <BOOT_WakeFromPG+0x12>
300035d6:	4a48      	ldr	r2, [pc, #288]	; (300036f8 <BOOT_WakeFromPG+0x130>)
300035d8:	6093      	str	r3, [r2, #8]
300035da:	4b48      	ldr	r3, [pc, #288]	; (300036fc <BOOT_WakeFromPG+0x134>)
300035dc:	f06f 0c1a 	mvn.w	ip, #26
300035e0:	4847      	ldr	r0, [pc, #284]	; (30003700 <BOOT_WakeFromPG+0x138>)
300035e2:	f103 0e50 	add.w	lr, r3, #80	; 0x50
300035e6:	ebac 0c04 	sub.w	ip, ip, r4
300035ea:	eb0c 0203 	add.w	r2, ip, r3
300035ee:	f813 1f01 	ldrb.w	r1, [r3, #1]!
300035f2:	4402      	add	r2, r0
300035f4:	4573      	cmp	r3, lr
300035f6:	f882 1300 	strb.w	r1, [r2, #768]	; 0x300
300035fa:	d1f6      	bne.n	300035ea <BOOT_WakeFromPG+0x22>
300035fc:	6822      	ldr	r2, [r4, #0]
300035fe:	4b3e      	ldr	r3, [pc, #248]	; (300036f8 <BOOT_WakeFromPG+0x130>)
30003600:	f422 7280 	bic.w	r2, r2, #256	; 0x100
30003604:	e9d4 5101 	ldrd	r5, r1, [r4, #4]
30003608:	6002      	str	r2, [r0, #0]
3000360a:	6045      	str	r5, [r0, #4]
3000360c:	6081      	str	r1, [r0, #8]
3000360e:	6a59      	ldr	r1, [r3, #36]	; 0x24
30003610:	4a3c      	ldr	r2, [pc, #240]	; (30003704 <BOOT_WakeFromPG+0x13c>)
30003612:	f441 2170 	orr.w	r1, r1, #983040	; 0xf0000
30003616:	4c3c      	ldr	r4, [pc, #240]	; (30003708 <BOOT_WakeFromPG+0x140>)
30003618:	6259      	str	r1, [r3, #36]	; 0x24
3000361a:	6a51      	ldr	r1, [r2, #36]	; 0x24
3000361c:	f441 21e0 	orr.w	r1, r1, #458752	; 0x70000
30003620:	6251      	str	r1, [r2, #36]	; 0x24
30003622:	f8d3 1088 	ldr.w	r1, [r3, #136]	; 0x88
30003626:	f441 0170 	orr.w	r1, r1, #15728640	; 0xf00000
3000362a:	f8c3 1088 	str.w	r1, [r3, #136]	; 0x88
3000362e:	f8d2 1088 	ldr.w	r1, [r2, #136]	; 0x88
30003632:	f441 0170 	orr.w	r1, r1, #15728640	; 0xf00000
30003636:	f8c2 1088 	str.w	r1, [r2, #136]	; 0x88
3000363a:	f8d3 208c 	ldr.w	r2, [r3, #140]	; 0x8c
3000363e:	f442 6240 	orr.w	r2, r2, #3072	; 0xc00
30003642:	f8c3 208c 	str.w	r2, [r3, #140]	; 0x8c
30003646:	f7ff fdc3 	bl	300031d0 <BOOT_CACHEWRR_Set>
3000364a:	4b30      	ldr	r3, [pc, #192]	; (3000370c <BOOT_WakeFromPG+0x144>)
3000364c:	6818      	ldr	r0, [r3, #0]
3000364e:	f7ff fded 	bl	3000322c <BOOT_TCMSet>
30003652:	2201      	movs	r2, #1
30003654:	492e      	ldr	r1, [pc, #184]	; (30003710 <BOOT_WakeFromPG+0x148>)
30003656:	f04f 2040 	mov.w	r0, #1073758208	; 0x40004000
3000365a:	47a0      	blx	r4
3000365c:	4b2d      	ldr	r3, [pc, #180]	; (30003714 <BOOT_WakeFromPG+0x14c>)
3000365e:	4798      	blx	r3
30003660:	4b2d      	ldr	r3, [pc, #180]	; (30003718 <BOOT_WakeFromPG+0x150>)
30003662:	4798      	blx	r3
30003664:	0783      	lsls	r3, r0, #30
30003666:	9001      	str	r0, [sp, #4]
30003668:	d530      	bpl.n	300036cc <BOOT_WakeFromPG+0x104>
3000366a:	4b2c      	ldr	r3, [pc, #176]	; (3000371c <BOOT_WakeFromPG+0x154>)
3000366c:	2249      	movs	r2, #73	; 0x49
3000366e:	492c      	ldr	r1, [pc, #176]	; (30003720 <BOOT_WakeFromPG+0x158>)
30003670:	2004      	movs	r0, #4
30003672:	f006 fe51 	bl	3000a318 <rtk_log_write>
30003676:	2201      	movs	r2, #1
30003678:	492a      	ldr	r1, [pc, #168]	; (30003724 <BOOT_WakeFromPG+0x15c>)
3000367a:	f04f 4082 	mov.w	r0, #1090519040	; 0x41000000
3000367e:	47a0      	blx	r4
30003680:	2201      	movs	r2, #1
30003682:	4929      	ldr	r1, [pc, #164]	; (30003728 <BOOT_WakeFromPG+0x160>)
30003684:	f04f 4081 	mov.w	r0, #1082130432	; 0x40800000
30003688:	47a0      	blx	r4
3000368a:	f7ff ff05 	bl	30003498 <BOOT_DDR_Init>
3000368e:	4a27      	ldr	r2, [pc, #156]	; (3000372c <BOOT_WakeFromPG+0x164>)
30003690:	f8d2 3290 	ldr.w	r3, [r2, #656]	; 0x290
30003694:	f423 6340 	bic.w	r3, r3, #3072	; 0xc00
30003698:	f443 6300 	orr.w	r3, r3, #2048	; 0x800
3000369c:	f8c2 3290 	str.w	r3, [r2, #656]	; 0x290
300036a0:	4b23      	ldr	r3, [pc, #140]	; (30003730 <BOOT_WakeFromPG+0x168>)
300036a2:	2000      	movs	r0, #0
300036a4:	4a17      	ldr	r2, [pc, #92]	; (30003704 <BOOT_WakeFromPG+0x13c>)
300036a6:	689c      	ldr	r4, [r3, #8]
300036a8:	685b      	ldr	r3, [r3, #4]
300036aa:	6063      	str	r3, [r4, #4]
300036ac:	6094      	str	r4, [r2, #8]
300036ae:	f006 f919 	bl	300098e4 <Fault_Hanlder_Redirect>
300036b2:	4b20      	ldr	r3, [pc, #128]	; (30003734 <BOOT_WakeFromPG+0x16c>)
300036b4:	f383 8888 	msr	MSP_NS, r3
300036b8:	f5a3 6300 	sub.w	r3, r3, #2048	; 0x800
300036bc:	f383 8809 	msr	PSP, r3
300036c0:	6860      	ldr	r0, [r4, #4]
300036c2:	b003      	add	sp, #12
300036c4:	e8bd 4030 	ldmia.w	sp!, {r4, r5, lr}
300036c8:	f7ff bcc6 	b.w	30003058 <BOOT_NsStart>
300036cc:	f04f 4090 	mov.w	r0, #1207959552	; 0x48000000
300036d0:	f04f 4188 	mov.w	r1, #1140850688	; 0x44000000
300036d4:	2201      	movs	r2, #1
300036d6:	47a0      	blx	r4
300036d8:	4a14      	ldr	r2, [pc, #80]	; (3000372c <BOOT_WakeFromPG+0x164>)
300036da:	f8d2 32fc 	ldr.w	r3, [r2, #764]	; 0x2fc
300036de:	f043 0302 	orr.w	r3, r3, #2
300036e2:	f8c2 32fc 	str.w	r3, [r2, #764]	; 0x2fc
300036e6:	f7ff ff15 	bl	30003514 <BOOT_PSRAM_Init>
300036ea:	2096      	movs	r0, #150	; 0x96
300036ec:	4b12      	ldr	r3, [pc, #72]	; (30003738 <BOOT_WakeFromPG+0x170>)
300036ee:	4798      	blx	r3
300036f0:	e7d6      	b.n	300036a0 <BOOT_WakeFromPG+0xd8>
300036f2:	bf00      	nop
300036f4:	3000f0d0 	.word	0x3000f0d0
300036f8:	e000ed00 	.word	0xe000ed00
300036fc:	3000f0eb 	.word	0x3000f0eb
30003700:	e000e100 	.word	0xe000e100
30003704:	e002ed00 	.word	0xe002ed00
30003708:	0000b479 	.word	0x0000b479
3000370c:	3000f964 	.word	0x3000f964
30003710:	40001000 	.word	0x40001000
30003714:	0000d835 	.word	0x0000d835
30003718:	3000af89 	.word	0x3000af89
3000371c:	3000c7cc 	.word	0x3000c7cc
30003720:	3000c7c4 	.word	0x3000c7c4
30003724:	40080000 	.word	0x40080000
30003728:	40040000 	.word	0x40040000
3000372c:	42008000 	.word	0x42008000
30003730:	60000020 	.word	0x60000020
30003734:	2001c000 	.word	0x2001c000
30003738:	00009b2d 	.word	0x00009b2d

3000373c <BOOT_SCBConfig_HP>:
3000373c:	4b0e      	ldr	r3, [pc, #56]	; (30003778 <BOOT_SCBConfig_HP+0x3c>)
3000373e:	4a0f      	ldr	r2, [pc, #60]	; (3000377c <BOOT_SCBConfig_HP+0x40>)
30003740:	6a59      	ldr	r1, [r3, #36]	; 0x24
30003742:	f441 2170 	orr.w	r1, r1, #983040	; 0xf0000
30003746:	6259      	str	r1, [r3, #36]	; 0x24
30003748:	6a51      	ldr	r1, [r2, #36]	; 0x24
3000374a:	f441 21e0 	orr.w	r1, r1, #458752	; 0x70000
3000374e:	6251      	str	r1, [r2, #36]	; 0x24
30003750:	f8d3 1088 	ldr.w	r1, [r3, #136]	; 0x88
30003754:	f441 0170 	orr.w	r1, r1, #15728640	; 0xf00000
30003758:	f8c3 1088 	str.w	r1, [r3, #136]	; 0x88
3000375c:	f8d2 1088 	ldr.w	r1, [r2, #136]	; 0x88
30003760:	f441 0170 	orr.w	r1, r1, #15728640	; 0xf00000
30003764:	f8c2 1088 	str.w	r1, [r2, #136]	; 0x88
30003768:	f8d3 208c 	ldr.w	r2, [r3, #140]	; 0x8c
3000376c:	f442 6240 	orr.w	r2, r2, #3072	; 0xc00
30003770:	f8c3 208c 	str.w	r2, [r3, #140]	; 0x8c
30003774:	4770      	bx	lr
30003776:	bf00      	nop
30003778:	e000ed00 	.word	0xe000ed00
3000377c:	e002ed00 	.word	0xe002ed00

30003780 <BOOT_SCBVTORBackup_HP>:
30003780:	4a02      	ldr	r2, [pc, #8]	; (3000378c <BOOT_SCBVTORBackup_HP+0xc>)
30003782:	4b03      	ldr	r3, [pc, #12]	; (30003790 <BOOT_SCBVTORBackup_HP+0x10>)
30003784:	6892      	ldr	r2, [r2, #8]
30003786:	619a      	str	r2, [r3, #24]
30003788:	4770      	bx	lr
3000378a:	bf00      	nop
3000378c:	e000ed00 	.word	0xe000ed00
30003790:	3000f0d0 	.word	0x3000f0d0

30003794 <BOOT_SCBVTORReFill_HP>:
30003794:	4a02      	ldr	r2, [pc, #8]	; (300037a0 <BOOT_SCBVTORReFill_HP+0xc>)
30003796:	4b03      	ldr	r3, [pc, #12]	; (300037a4 <BOOT_SCBVTORReFill_HP+0x10>)
30003798:	6992      	ldr	r2, [r2, #24]
3000379a:	609a      	str	r2, [r3, #8]
3000379c:	4770      	bx	lr
3000379e:	bf00      	nop
300037a0:	3000f0d0 	.word	0x3000f0d0
300037a4:	e000ed00 	.word	0xe000ed00

300037a8 <BOOT_NVICBackup_HP>:
300037a8:	491a      	ldr	r1, [pc, #104]	; (30003814 <BOOT_NVICBackup_HP+0x6c>)
300037aa:	f06f 001a 	mvn.w	r0, #26
300037ae:	b410      	push	{r4}
300037b0:	4c19      	ldr	r4, [pc, #100]	; (30003818 <BOOT_NVICBackup_HP+0x70>)
300037b2:	680a      	ldr	r2, [r1, #0]
300037b4:	f104 031b 	add.w	r3, r4, #27
300037b8:	1b00      	subs	r0, r0, r4
300037ba:	6022      	str	r2, [r4, #0]
300037bc:	f104 0c6b 	add.w	ip, r4, #107	; 0x6b
300037c0:	684a      	ldr	r2, [r1, #4]
300037c2:	6062      	str	r2, [r4, #4]
300037c4:	688a      	ldr	r2, [r1, #8]
300037c6:	60a2      	str	r2, [r4, #8]
300037c8:	18c2      	adds	r2, r0, r3
300037ca:	440a      	add	r2, r1
300037cc:	f892 2300 	ldrb.w	r2, [r2, #768]	; 0x300
300037d0:	f803 2f01 	strb.w	r2, [r3, #1]!
300037d4:	4563      	cmp	r3, ip
300037d6:	d1f7      	bne.n	300037c8 <BOOT_NVICBackup_HP+0x20>
300037d8:	4b10      	ldr	r3, [pc, #64]	; (3000381c <BOOT_NVICBackup_HP+0x74>)
300037da:	695b      	ldr	r3, [r3, #20]
300037dc:	03db      	lsls	r3, r3, #15
300037de:	d513      	bpl.n	30003808 <BOOT_NVICBackup_HP+0x60>
300037e0:	f004 021f 	and.w	r2, r4, #31
300037e4:	4b0c      	ldr	r3, [pc, #48]	; (30003818 <BOOT_NVICBackup_HP+0x70>)
300037e6:	f102 016c 	add.w	r1, r2, #108	; 0x6c
300037ea:	b982      	cbnz	r2, 3000380e <BOOT_NVICBackup_HP+0x66>
300037ec:	f3bf 8f4f 	dsb	sy
300037f0:	480a      	ldr	r0, [pc, #40]	; (3000381c <BOOT_NVICBackup_HP+0x74>)
300037f2:	4419      	add	r1, r3
300037f4:	f8c0 3270 	str.w	r3, [r0, #624]	; 0x270
300037f8:	3320      	adds	r3, #32
300037fa:	1aca      	subs	r2, r1, r3
300037fc:	2a00      	cmp	r2, #0
300037fe:	dcf9      	bgt.n	300037f4 <BOOT_NVICBackup_HP+0x4c>
30003800:	f3bf 8f4f 	dsb	sy
30003804:	f3bf 8f6f 	isb	sy
30003808:	f85d 4b04 	ldr.w	r4, [sp], #4
3000380c:	4770      	bx	lr
3000380e:	f024 031f 	bic.w	r3, r4, #31
30003812:	e7eb      	b.n	300037ec <BOOT_NVICBackup_HP+0x44>
30003814:	e000e100 	.word	0xe000e100
30003818:	3000f0d0 	.word	0x3000f0d0
3000381c:	e000ed00 	.word	0xe000ed00

30003820 <BOOT_NVICReFill_HP>:
30003820:	b510      	push	{r4, lr}
30003822:	f06f 0c1a 	mvn.w	ip, #26
30003826:	4c0d      	ldr	r4, [pc, #52]	; (3000385c <BOOT_NVICReFill_HP+0x3c>)
30003828:	480d      	ldr	r0, [pc, #52]	; (30003860 <BOOT_NVICReFill_HP+0x40>)
3000382a:	f104 031b 	add.w	r3, r4, #27
3000382e:	f104 0e6b 	add.w	lr, r4, #107	; 0x6b
30003832:	ebac 0c04 	sub.w	ip, ip, r4
30003836:	eb0c 0203 	add.w	r2, ip, r3
3000383a:	f813 1f01 	ldrb.w	r1, [r3, #1]!
3000383e:	4402      	add	r2, r0
30003840:	4573      	cmp	r3, lr
30003842:	f882 1300 	strb.w	r1, [r2, #768]	; 0x300
30003846:	d1f6      	bne.n	30003836 <BOOT_NVICReFill_HP+0x16>
30003848:	e9d4 3100 	ldrd	r3, r1, [r4]
3000384c:	68a2      	ldr	r2, [r4, #8]
3000384e:	f423 7380 	bic.w	r3, r3, #256	; 0x100
30003852:	6003      	str	r3, [r0, #0]
30003854:	6041      	str	r1, [r0, #4]
30003856:	6082      	str	r2, [r0, #8]
30003858:	bd10      	pop	{r4, pc}
3000385a:	bf00      	nop
3000385c:	3000f0d0 	.word	0x3000f0d0
30003860:	e000e100 	.word	0xe000e100

30003864 <BOOT_LoadImages>:
30003864:	b510      	push	{r4, lr}
30003866:	4b07      	ldr	r3, [pc, #28]	; (30003884 <BOOT_LoadImages+0x20>)
30003868:	b082      	sub	sp, #8
3000386a:	4798      	blx	r3
3000386c:	4604      	mov	r4, r0
3000386e:	4b06      	ldr	r3, [pc, #24]	; (30003888 <BOOT_LoadImages+0x24>)
30003870:	2004      	movs	r0, #4
30003872:	3401      	adds	r4, #1
30003874:	2249      	movs	r2, #73	; 0x49
30003876:	4905      	ldr	r1, [pc, #20]	; (3000388c <BOOT_LoadImages+0x28>)
30003878:	9400      	str	r4, [sp, #0]
3000387a:	f006 fd4d 	bl	3000a318 <rtk_log_write>
3000387e:	2001      	movs	r0, #1
30003880:	b002      	add	sp, #8
30003882:	bd10      	pop	{r4, pc}
30003884:	3000553d 	.word	0x3000553d
30003888:	3000c7dc 	.word	0x3000c7dc
3000388c:	3000c7c4 	.word	0x3000c7c4

30003890 <BOOT_ReasonSet>:
30003890:	b570      	push	{r4, r5, r6, lr}
30003892:	4e7a      	ldr	r6, [pc, #488]	; (30003a7c <BOOT_ReasonSet+0x1ec>)
30003894:	b082      	sub	sp, #8
30003896:	2004      	movs	r0, #4
30003898:	4b79      	ldr	r3, [pc, #484]	; (30003a80 <BOOT_ReasonSet+0x1f0>)
3000389a:	6a34      	ldr	r4, [r6, #32]
3000389c:	2249      	movs	r2, #73	; 0x49
3000389e:	4979      	ldr	r1, [pc, #484]	; (30003a84 <BOOT_ReasonSet+0x1f4>)
300038a0:	f024 457f 	bic.w	r5, r4, #4278190080	; 0xff000000
300038a4:	6235      	str	r5, [r6, #32]
300038a6:	f8c6 5264 	str.w	r5, [r6, #612]	; 0x264
300038aa:	9500      	str	r5, [sp, #0]
300038ac:	f006 fd34 	bl	3000a318 <rtk_log_write>
300038b0:	0220      	lsls	r0, r4, #8
300038b2:	f100 80d9 	bmi.w	30003a68 <BOOT_ReasonSet+0x1d8>
300038b6:	0261      	lsls	r1, r4, #9
300038b8:	f100 80cd 	bmi.w	30003a56 <BOOT_ReasonSet+0x1c6>
300038bc:	02a2      	lsls	r2, r4, #10
300038be:	f100 80c1 	bmi.w	30003a44 <BOOT_ReasonSet+0x1b4>
300038c2:	02e3      	lsls	r3, r4, #11
300038c4:	f100 80b5 	bmi.w	30003a32 <BOOT_ReasonSet+0x1a2>
300038c8:	0366      	lsls	r6, r4, #13
300038ca:	f100 80a9 	bmi.w	30003a20 <BOOT_ReasonSet+0x190>
300038ce:	03a0      	lsls	r0, r4, #14
300038d0:	f100 809d 	bmi.w	30003a0e <BOOT_ReasonSet+0x17e>
300038d4:	03e1      	lsls	r1, r4, #15
300038d6:	f100 8091 	bmi.w	300039fc <BOOT_ReasonSet+0x16c>
300038da:	0422      	lsls	r2, r4, #16
300038dc:	f100 8085 	bmi.w	300039ea <BOOT_ReasonSet+0x15a>
300038e0:	0463      	lsls	r3, r4, #17
300038e2:	d479      	bmi.n	300039d8 <BOOT_ReasonSet+0x148>
300038e4:	04a6      	lsls	r6, r4, #18
300038e6:	d46e      	bmi.n	300039c6 <BOOT_ReasonSet+0x136>
300038e8:	04e0      	lsls	r0, r4, #19
300038ea:	d463      	bmi.n	300039b4 <BOOT_ReasonSet+0x124>
300038ec:	0521      	lsls	r1, r4, #20
300038ee:	d458      	bmi.n	300039a2 <BOOT_ReasonSet+0x112>
300038f0:	0562      	lsls	r2, r4, #21
300038f2:	d44d      	bmi.n	30003990 <BOOT_ReasonSet+0x100>
300038f4:	05a3      	lsls	r3, r4, #22
300038f6:	d442      	bmi.n	3000397e <BOOT_ReasonSet+0xee>
300038f8:	05e6      	lsls	r6, r4, #23
300038fa:	d437      	bmi.n	3000396c <BOOT_ReasonSet+0xdc>
300038fc:	0620      	lsls	r0, r4, #24
300038fe:	d42c      	bmi.n	3000395a <BOOT_ReasonSet+0xca>
30003900:	06a1      	lsls	r1, r4, #26
30003902:	d421      	bmi.n	30003948 <BOOT_ReasonSet+0xb8>
30003904:	06e2      	lsls	r2, r4, #27
30003906:	d416      	bmi.n	30003936 <BOOT_ReasonSet+0xa6>
30003908:	0723      	lsls	r3, r4, #28
3000390a:	d40b      	bmi.n	30003924 <BOOT_ReasonSet+0x94>
3000390c:	b945      	cbnz	r5, 30003920 <BOOT_ReasonSet+0x90>
3000390e:	4b5e      	ldr	r3, [pc, #376]	; (30003a88 <BOOT_ReasonSet+0x1f8>)
30003910:	2249      	movs	r2, #73	; 0x49
30003912:	495e      	ldr	r1, [pc, #376]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
30003914:	2004      	movs	r0, #4
30003916:	b002      	add	sp, #8
30003918:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
3000391c:	f006 bd24 	b.w	3000a368 <rtk_log_write_nano>
30003920:	4b5b      	ldr	r3, [pc, #364]	; (30003a90 <BOOT_ReasonSet+0x200>)
30003922:	e7f5      	b.n	30003910 <BOOT_ReasonSet+0x80>
30003924:	485b      	ldr	r0, [pc, #364]	; (30003a94 <BOOT_ReasonSet+0x204>)
30003926:	2249      	movs	r2, #73	; 0x49
30003928:	4b5b      	ldr	r3, [pc, #364]	; (30003a98 <BOOT_ReasonSet+0x208>)
3000392a:	9000      	str	r0, [sp, #0]
3000392c:	2004      	movs	r0, #4
3000392e:	4957      	ldr	r1, [pc, #348]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
30003930:	f006 fd1a 	bl	3000a368 <rtk_log_write_nano>
30003934:	e7ea      	b.n	3000390c <BOOT_ReasonSet+0x7c>
30003936:	4959      	ldr	r1, [pc, #356]	; (30003a9c <BOOT_ReasonSet+0x20c>)
30003938:	2249      	movs	r2, #73	; 0x49
3000393a:	4b57      	ldr	r3, [pc, #348]	; (30003a98 <BOOT_ReasonSet+0x208>)
3000393c:	2004      	movs	r0, #4
3000393e:	9100      	str	r1, [sp, #0]
30003940:	4952      	ldr	r1, [pc, #328]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
30003942:	f006 fd11 	bl	3000a368 <rtk_log_write_nano>
30003946:	e7df      	b.n	30003908 <BOOT_ReasonSet+0x78>
30003948:	4955      	ldr	r1, [pc, #340]	; (30003aa0 <BOOT_ReasonSet+0x210>)
3000394a:	2249      	movs	r2, #73	; 0x49
3000394c:	4b52      	ldr	r3, [pc, #328]	; (30003a98 <BOOT_ReasonSet+0x208>)
3000394e:	2004      	movs	r0, #4
30003950:	9100      	str	r1, [sp, #0]
30003952:	494e      	ldr	r1, [pc, #312]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
30003954:	f006 fd08 	bl	3000a368 <rtk_log_write_nano>
30003958:	e7d4      	b.n	30003904 <BOOT_ReasonSet+0x74>
3000395a:	4952      	ldr	r1, [pc, #328]	; (30003aa4 <BOOT_ReasonSet+0x214>)
3000395c:	2249      	movs	r2, #73	; 0x49
3000395e:	4b4e      	ldr	r3, [pc, #312]	; (30003a98 <BOOT_ReasonSet+0x208>)
30003960:	2004      	movs	r0, #4
30003962:	9100      	str	r1, [sp, #0]
30003964:	4949      	ldr	r1, [pc, #292]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
30003966:	f006 fcff 	bl	3000a368 <rtk_log_write_nano>
3000396a:	e7c9      	b.n	30003900 <BOOT_ReasonSet+0x70>
3000396c:	494e      	ldr	r1, [pc, #312]	; (30003aa8 <BOOT_ReasonSet+0x218>)
3000396e:	2249      	movs	r2, #73	; 0x49
30003970:	4b49      	ldr	r3, [pc, #292]	; (30003a98 <BOOT_ReasonSet+0x208>)
30003972:	2004      	movs	r0, #4
30003974:	9100      	str	r1, [sp, #0]
30003976:	4945      	ldr	r1, [pc, #276]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
30003978:	f006 fcf6 	bl	3000a368 <rtk_log_write_nano>
3000397c:	e7be      	b.n	300038fc <BOOT_ReasonSet+0x6c>
3000397e:	494b      	ldr	r1, [pc, #300]	; (30003aac <BOOT_ReasonSet+0x21c>)
30003980:	2249      	movs	r2, #73	; 0x49
30003982:	4b45      	ldr	r3, [pc, #276]	; (30003a98 <BOOT_ReasonSet+0x208>)
30003984:	2004      	movs	r0, #4
30003986:	9100      	str	r1, [sp, #0]
30003988:	4940      	ldr	r1, [pc, #256]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
3000398a:	f006 fced 	bl	3000a368 <rtk_log_write_nano>
3000398e:	e7b3      	b.n	300038f8 <BOOT_ReasonSet+0x68>
30003990:	4947      	ldr	r1, [pc, #284]	; (30003ab0 <BOOT_ReasonSet+0x220>)
30003992:	2249      	movs	r2, #73	; 0x49
30003994:	4b40      	ldr	r3, [pc, #256]	; (30003a98 <BOOT_ReasonSet+0x208>)
30003996:	2004      	movs	r0, #4
30003998:	9100      	str	r1, [sp, #0]
3000399a:	493c      	ldr	r1, [pc, #240]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
3000399c:	f006 fce4 	bl	3000a368 <rtk_log_write_nano>
300039a0:	e7a8      	b.n	300038f4 <BOOT_ReasonSet+0x64>
300039a2:	4944      	ldr	r1, [pc, #272]	; (30003ab4 <BOOT_ReasonSet+0x224>)
300039a4:	2249      	movs	r2, #73	; 0x49
300039a6:	4b3c      	ldr	r3, [pc, #240]	; (30003a98 <BOOT_ReasonSet+0x208>)
300039a8:	2004      	movs	r0, #4
300039aa:	9100      	str	r1, [sp, #0]
300039ac:	4937      	ldr	r1, [pc, #220]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
300039ae:	f006 fcdb 	bl	3000a368 <rtk_log_write_nano>
300039b2:	e79d      	b.n	300038f0 <BOOT_ReasonSet+0x60>
300039b4:	4940      	ldr	r1, [pc, #256]	; (30003ab8 <BOOT_ReasonSet+0x228>)
300039b6:	2249      	movs	r2, #73	; 0x49
300039b8:	4b37      	ldr	r3, [pc, #220]	; (30003a98 <BOOT_ReasonSet+0x208>)
300039ba:	2004      	movs	r0, #4
300039bc:	9100      	str	r1, [sp, #0]
300039be:	4933      	ldr	r1, [pc, #204]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
300039c0:	f006 fcd2 	bl	3000a368 <rtk_log_write_nano>
300039c4:	e792      	b.n	300038ec <BOOT_ReasonSet+0x5c>
300039c6:	493d      	ldr	r1, [pc, #244]	; (30003abc <BOOT_ReasonSet+0x22c>)
300039c8:	2249      	movs	r2, #73	; 0x49
300039ca:	4b33      	ldr	r3, [pc, #204]	; (30003a98 <BOOT_ReasonSet+0x208>)
300039cc:	2004      	movs	r0, #4
300039ce:	9100      	str	r1, [sp, #0]
300039d0:	492e      	ldr	r1, [pc, #184]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
300039d2:	f006 fcc9 	bl	3000a368 <rtk_log_write_nano>
300039d6:	e787      	b.n	300038e8 <BOOT_ReasonSet+0x58>
300039d8:	4939      	ldr	r1, [pc, #228]	; (30003ac0 <BOOT_ReasonSet+0x230>)
300039da:	2249      	movs	r2, #73	; 0x49
300039dc:	4b2e      	ldr	r3, [pc, #184]	; (30003a98 <BOOT_ReasonSet+0x208>)
300039de:	2004      	movs	r0, #4
300039e0:	9100      	str	r1, [sp, #0]
300039e2:	492a      	ldr	r1, [pc, #168]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
300039e4:	f006 fcc0 	bl	3000a368 <rtk_log_write_nano>
300039e8:	e77c      	b.n	300038e4 <BOOT_ReasonSet+0x54>
300039ea:	4936      	ldr	r1, [pc, #216]	; (30003ac4 <BOOT_ReasonSet+0x234>)
300039ec:	2249      	movs	r2, #73	; 0x49
300039ee:	4b2a      	ldr	r3, [pc, #168]	; (30003a98 <BOOT_ReasonSet+0x208>)
300039f0:	2004      	movs	r0, #4
300039f2:	9100      	str	r1, [sp, #0]
300039f4:	4925      	ldr	r1, [pc, #148]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
300039f6:	f006 fcb7 	bl	3000a368 <rtk_log_write_nano>
300039fa:	e771      	b.n	300038e0 <BOOT_ReasonSet+0x50>
300039fc:	4932      	ldr	r1, [pc, #200]	; (30003ac8 <BOOT_ReasonSet+0x238>)
300039fe:	2249      	movs	r2, #73	; 0x49
30003a00:	4b25      	ldr	r3, [pc, #148]	; (30003a98 <BOOT_ReasonSet+0x208>)
30003a02:	2004      	movs	r0, #4
30003a04:	9100      	str	r1, [sp, #0]
30003a06:	4921      	ldr	r1, [pc, #132]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
30003a08:	f006 fcae 	bl	3000a368 <rtk_log_write_nano>
30003a0c:	e765      	b.n	300038da <BOOT_ReasonSet+0x4a>
30003a0e:	492f      	ldr	r1, [pc, #188]	; (30003acc <BOOT_ReasonSet+0x23c>)
30003a10:	2249      	movs	r2, #73	; 0x49
30003a12:	4b21      	ldr	r3, [pc, #132]	; (30003a98 <BOOT_ReasonSet+0x208>)
30003a14:	2004      	movs	r0, #4
30003a16:	9100      	str	r1, [sp, #0]
30003a18:	491c      	ldr	r1, [pc, #112]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
30003a1a:	f006 fca5 	bl	3000a368 <rtk_log_write_nano>
30003a1e:	e759      	b.n	300038d4 <BOOT_ReasonSet+0x44>
30003a20:	492b      	ldr	r1, [pc, #172]	; (30003ad0 <BOOT_ReasonSet+0x240>)
30003a22:	2249      	movs	r2, #73	; 0x49
30003a24:	4b1c      	ldr	r3, [pc, #112]	; (30003a98 <BOOT_ReasonSet+0x208>)
30003a26:	2004      	movs	r0, #4
30003a28:	9100      	str	r1, [sp, #0]
30003a2a:	4918      	ldr	r1, [pc, #96]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
30003a2c:	f006 fc9c 	bl	3000a368 <rtk_log_write_nano>
30003a30:	e74d      	b.n	300038ce <BOOT_ReasonSet+0x3e>
30003a32:	4928      	ldr	r1, [pc, #160]	; (30003ad4 <BOOT_ReasonSet+0x244>)
30003a34:	2249      	movs	r2, #73	; 0x49
30003a36:	4b18      	ldr	r3, [pc, #96]	; (30003a98 <BOOT_ReasonSet+0x208>)
30003a38:	2004      	movs	r0, #4
30003a3a:	9100      	str	r1, [sp, #0]
30003a3c:	4913      	ldr	r1, [pc, #76]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
30003a3e:	f006 fc93 	bl	3000a368 <rtk_log_write_nano>
30003a42:	e741      	b.n	300038c8 <BOOT_ReasonSet+0x38>
30003a44:	4924      	ldr	r1, [pc, #144]	; (30003ad8 <BOOT_ReasonSet+0x248>)
30003a46:	2249      	movs	r2, #73	; 0x49
30003a48:	4b13      	ldr	r3, [pc, #76]	; (30003a98 <BOOT_ReasonSet+0x208>)
30003a4a:	2004      	movs	r0, #4
30003a4c:	9100      	str	r1, [sp, #0]
30003a4e:	490f      	ldr	r1, [pc, #60]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
30003a50:	f006 fc8a 	bl	3000a368 <rtk_log_write_nano>
30003a54:	e735      	b.n	300038c2 <BOOT_ReasonSet+0x32>
30003a56:	4921      	ldr	r1, [pc, #132]	; (30003adc <BOOT_ReasonSet+0x24c>)
30003a58:	2249      	movs	r2, #73	; 0x49
30003a5a:	4b0f      	ldr	r3, [pc, #60]	; (30003a98 <BOOT_ReasonSet+0x208>)
30003a5c:	2004      	movs	r0, #4
30003a5e:	9100      	str	r1, [sp, #0]
30003a60:	490a      	ldr	r1, [pc, #40]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
30003a62:	f006 fc81 	bl	3000a368 <rtk_log_write_nano>
30003a66:	e729      	b.n	300038bc <BOOT_ReasonSet+0x2c>
30003a68:	491d      	ldr	r1, [pc, #116]	; (30003ae0 <BOOT_ReasonSet+0x250>)
30003a6a:	2249      	movs	r2, #73	; 0x49
30003a6c:	4b0a      	ldr	r3, [pc, #40]	; (30003a98 <BOOT_ReasonSet+0x208>)
30003a6e:	2004      	movs	r0, #4
30003a70:	9100      	str	r1, [sp, #0]
30003a72:	4906      	ldr	r1, [pc, #24]	; (30003a8c <BOOT_ReasonSet+0x1fc>)
30003a74:	f006 fc78 	bl	3000a368 <rtk_log_write_nano>
30003a78:	e71d      	b.n	300038b6 <BOOT_ReasonSet+0x26>
30003a7a:	bf00      	nop
30003a7c:	42008000 	.word	0x42008000
30003a80:	3000c7f4 	.word	0x3000c7f4
30003a84:	3000c7c4 	.word	0x3000c7c4
30003a88:	3000c8dc 	.word	0x3000c8dc
30003a8c:	3000c810 	.word	0x3000c810
30003a90:	3000c7c0 	.word	0x3000c7c0
30003a94:	3000c8d8 	.word	0x3000c8d8
30003a98:	3000c80c 	.word	0x3000c80c
30003a9c:	3000c8d4 	.word	0x3000c8d4
30003aa0:	3000c8cc 	.word	0x3000c8cc
30003aa4:	3000c8c4 	.word	0x3000c8c4
30003aa8:	3000c8bc 	.word	0x3000c8bc
30003aac:	3000c8b4 	.word	0x3000c8b4
30003ab0:	3000c8ac 	.word	0x3000c8ac
30003ab4:	3000c8a4 	.word	0x3000c8a4
30003ab8:	3000c89c 	.word	0x3000c89c
30003abc:	3000c894 	.word	0x3000c894
30003ac0:	3000c88c 	.word	0x3000c88c
30003ac4:	3000c884 	.word	0x3000c884
30003ac8:	3000c874 	.word	0x3000c874
30003acc:	3000c864 	.word	0x3000c864
30003ad0:	3000c854 	.word	0x3000c854
30003ad4:	3000c844 	.word	0x3000c844
30003ad8:	3000c834 	.word	0x3000c834
30003adc:	3000c824 	.word	0x3000c824
30003ae0:	3000c814 	.word	0x3000c814

30003ae4 <BOOT_Enable_KM0>:
30003ae4:	b510      	push	{r4, lr}
30003ae6:	4c12      	ldr	r4, [pc, #72]	; (30003b30 <BOOT_Enable_KM0+0x4c>)
30003ae8:	47a0      	blx	r4
30003aea:	b110      	cbz	r0, 30003af2 <BOOT_Enable_KM0+0xe>
30003aec:	4b11      	ldr	r3, [pc, #68]	; (30003b34 <BOOT_Enable_KM0+0x50>)
30003aee:	4798      	blx	r3
30003af0:	b1a8      	cbz	r0, 30003b1e <BOOT_Enable_KM0+0x3a>
30003af2:	47a0      	blx	r4
30003af4:	b940      	cbnz	r0, 30003b08 <BOOT_Enable_KM0+0x24>
30003af6:	4a10      	ldr	r2, [pc, #64]	; (30003b38 <BOOT_Enable_KM0+0x54>)
30003af8:	f8d2 3268 	ldr.w	r3, [r2, #616]	; 0x268
30003afc:	f043 7300 	orr.w	r3, r3, #33554432	; 0x2000000
30003b00:	f8c2 3268 	str.w	r3, [r2, #616]	; 0x268
30003b04:	bf40      	sev
30003b06:	bd10      	pop	{r4, pc}
30003b08:	4b0a      	ldr	r3, [pc, #40]	; (30003b34 <BOOT_Enable_KM0+0x50>)
30003b0a:	4798      	blx	r3
30003b0c:	2800      	cmp	r0, #0
30003b0e:	d0f2      	beq.n	30003af6 <BOOT_Enable_KM0+0x12>
30003b10:	4b0a      	ldr	r3, [pc, #40]	; (30003b3c <BOOT_Enable_KM0+0x58>)
30003b12:	2249      	movs	r2, #73	; 0x49
30003b14:	490a      	ldr	r1, [pc, #40]	; (30003b40 <BOOT_Enable_KM0+0x5c>)
30003b16:	2004      	movs	r0, #4
30003b18:	f006 fbfe 	bl	3000a318 <rtk_log_write>
30003b1c:	e7eb      	b.n	30003af6 <BOOT_Enable_KM0+0x12>
30003b1e:	4b09      	ldr	r3, [pc, #36]	; (30003b44 <BOOT_Enable_KM0+0x60>)
30003b20:	2249      	movs	r2, #73	; 0x49
30003b22:	4907      	ldr	r1, [pc, #28]	; (30003b40 <BOOT_Enable_KM0+0x5c>)
30003b24:	2004      	movs	r0, #4
30003b26:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
30003b2a:	f006 bbf5 	b.w	3000a318 <rtk_log_write>
30003b2e:	bf00      	nop
30003b30:	0000c09d 	.word	0x0000c09d
30003b34:	0000c0ad 	.word	0x0000c0ad
30003b38:	42008000 	.word	0x42008000
30003b3c:	3000c914 	.word	0x3000c914
30003b40:	3000c7c4 	.word	0x3000c7c4
30003b44:	3000c8f0 	.word	0x3000c8f0

30003b48 <BOOT_AP_Clk_Get>:
30003b48:	4a06      	ldr	r2, [pc, #24]	; (30003b64 <BOOT_AP_Clk_Get+0x1c>)
30003b4a:	6913      	ldr	r3, [r2, #16]
30003b4c:	2b00      	cmp	r3, #0
30003b4e:	db03      	blt.n	30003b58 <BOOT_AP_Clk_Get+0x10>
30003b50:	6810      	ldr	r0, [r2, #0]
30003b52:	fbb0 f0f3 	udiv	r0, r0, r3
30003b56:	4770      	bx	lr
30003b58:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
30003b5c:	6850      	ldr	r0, [r2, #4]
30003b5e:	fbb0 f0f3 	udiv	r0, r0, r3
30003b62:	4770      	bx	lr
30003b64:	3000f044 	.word	0x3000f044

30003b68 <BOOT_SOC_ClkSet>:
30003b68:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30003b6c:	4f78      	ldr	r7, [pc, #480]	; (30003d50 <BOOT_SOC_ClkSet+0x1e8>)
30003b6e:	b085      	sub	sp, #20
30003b70:	4b78      	ldr	r3, [pc, #480]	; (30003d54 <BOOT_SOC_ClkSet+0x1ec>)
30003b72:	4798      	blx	r3
30003b74:	693b      	ldr	r3, [r7, #16]
30003b76:	9003      	str	r0, [sp, #12]
30003b78:	2b00      	cmp	r3, #0
30003b7a:	f2c0 8098 	blt.w	30003cae <BOOT_SOC_ClkSet+0x146>
30003b7e:	683a      	ldr	r2, [r7, #0]
30003b80:	fbb2 f8f3 	udiv	r8, r2, r3
30003b84:	4b74      	ldr	r3, [pc, #464]	; (30003d58 <BOOT_SOC_ClkSet+0x1f0>)
30003b86:	68f9      	ldr	r1, [r7, #12]
30003b88:	fba3 0302 	umull	r0, r3, r3, r2
30003b8c:	09de      	lsrs	r6, r3, #7
30003b8e:	095d      	lsrs	r5, r3, #5
30003b90:	099b      	lsrs	r3, r3, #6
30003b92:	3e01      	subs	r6, #1
30003b94:	3d01      	subs	r5, #1
30003b96:	1e5c      	subs	r4, r3, #1
30003b98:	fbb2 f9f1 	udiv	r9, r2, r1
30003b9c:	4a6f      	ldr	r2, [pc, #444]	; (30003d5c <BOOT_SOC_ClkSet+0x1f4>)
30003b9e:	4790      	blx	r2
30003ba0:	2801      	cmp	r0, #1
30003ba2:	f000 8081 	beq.w	30003ca8 <BOOT_SOC_ClkSet+0x140>
30003ba6:	f8d7 a008 	ldr.w	sl, [r7, #8]
30003baa:	f1ba 0f03 	cmp.w	sl, #3
30003bae:	f000 8099 	beq.w	30003ce4 <BOOT_SOC_ClkSet+0x17c>
30003bb2:	4b6b      	ldr	r3, [pc, #428]	; (30003d60 <BOOT_SOC_ClkSet+0x1f8>)
30003bb4:	2201      	movs	r2, #1
30003bb6:	f1b9 0ffa 	cmp.w	r9, #250	; 0xfa
30003bba:	f883 2110 	strb.w	r2, [r3, #272]	; 0x110
30003bbe:	f200 80bc 	bhi.w	30003d3a <BOOT_SOC_ClkSet+0x1d2>
30003bc2:	f8df a1dc 	ldr.w	sl, [pc, #476]	; 30003da0 <BOOT_SOC_ClkSet+0x238>
30003bc6:	f89a 3000 	ldrb.w	r3, [sl]
30003bca:	2b01      	cmp	r3, #1
30003bcc:	d103      	bne.n	30003bd6 <BOOT_SOC_ClkSet+0x6e>
30003bce:	f5b8 7f66 	cmp.w	r8, #920	; 0x398
30003bd2:	f200 80ad 	bhi.w	30003d30 <BOOT_SOC_ClkSet+0x1c8>
30003bd6:	2001      	movs	r0, #1
30003bd8:	f8df b1c8 	ldr.w	fp, [pc, #456]	; 30003da4 <BOOT_SOC_ClkSet+0x23c>
30003bdc:	f005 fad8 	bl	30009190 <CLK_SWITCH_XTAL>
30003be0:	6838      	ldr	r0, [r7, #0]
30003be2:	4b60      	ldr	r3, [pc, #384]	; (30003d64 <BOOT_SOC_ClkSet+0x1fc>)
30003be4:	0236      	lsls	r6, r6, #8
30003be6:	fb0b f000 	mul.w	r0, fp, r0
30003bea:	032d      	lsls	r5, r5, #12
30003bec:	4798      	blx	r3
30003bee:	495e      	ldr	r1, [pc, #376]	; (30003d68 <BOOT_SOC_ClkSet+0x200>)
30003bf0:	68fa      	ldr	r2, [r7, #12]
30003bf2:	0523      	lsls	r3, r4, #20
30003bf4:	485d      	ldr	r0, [pc, #372]	; (30003d6c <BOOT_SOC_ClkSet+0x204>)
30003bf6:	f406 66e0 	and.w	r6, r6, #1792	; 0x700
30003bfa:	f8d1 4228 	ldr.w	r4, [r1, #552]	; 0x228
30003bfe:	3a01      	subs	r2, #1
30003c00:	b2ad      	uxth	r5, r5
30003c02:	f403 03e0 	and.w	r3, r3, #7340032	; 0x700000
30003c06:	4020      	ands	r0, r4
30003c08:	f002 0207 	and.w	r2, r2, #7
30003c0c:	4302      	orrs	r2, r0
30003c0e:	2000      	movs	r0, #0
30003c10:	4316      	orrs	r6, r2
30003c12:	4335      	orrs	r5, r6
30003c14:	432b      	orrs	r3, r5
30003c16:	f8c1 3228 	str.w	r3, [r1, #552]	; 0x228
30003c1a:	f005 fab9 	bl	30009190 <CLK_SWITCH_XTAL>
30003c1e:	f89a 3000 	ldrb.w	r3, [sl]
30003c22:	2b00      	cmp	r3, #0
30003c24:	d040      	beq.n	30003ca8 <BOOT_SOC_ClkSet+0x140>
30003c26:	693b      	ldr	r3, [r7, #16]
30003c28:	2b00      	cmp	r3, #0
30003c2a:	db47      	blt.n	30003cbc <BOOT_SOC_ClkSet+0x154>
30003c2c:	f04f 4282 	mov.w	r2, #1090519040	; 0x41000000
30003c30:	6913      	ldr	r3, [r2, #16]
30003c32:	f043 030c 	orr.w	r3, r3, #12
30003c36:	6113      	str	r3, [r2, #16]
30003c38:	2200      	movs	r2, #0
30003c3a:	4b49      	ldr	r3, [pc, #292]	; (30003d60 <BOOT_SOC_ClkSet+0x1f8>)
30003c3c:	f883 2111 	strb.w	r2, [r3, #273]	; 0x111
30003c40:	f04f 4182 	mov.w	r1, #1090519040	; 0x41000000
30003c44:	693b      	ldr	r3, [r7, #16]
30003c46:	690a      	ldr	r2, [r1, #16]
30003c48:	3b01      	subs	r3, #1
30003c4a:	f022 0203 	bic.w	r2, r2, #3
30003c4e:	f003 0303 	and.w	r3, r3, #3
30003c52:	4313      	orrs	r3, r2
30003c54:	4a44      	ldr	r2, [pc, #272]	; (30003d68 <BOOT_SOC_ClkSet+0x200>)
30003c56:	610b      	str	r3, [r1, #16]
30003c58:	f8d2 3224 	ldr.w	r3, [r2, #548]	; 0x224
30003c5c:	079b      	lsls	r3, r3, #30
30003c5e:	d13a      	bne.n	30003cd6 <BOOT_SOC_ClkSet+0x16e>
30003c60:	2404      	movs	r4, #4
30003c62:	4b43      	ldr	r3, [pc, #268]	; (30003d70 <BOOT_SOC_ClkSet+0x208>)
30003c64:	2249      	movs	r2, #73	; 0x49
30003c66:	4943      	ldr	r1, [pc, #268]	; (30003d74 <BOOT_SOC_ClkSet+0x20c>)
30003c68:	2004      	movs	r0, #4
30003c6a:	f8cd 9000 	str.w	r9, [sp]
30003c6e:	f006 fb53 	bl	3000a318 <rtk_log_write>
30003c72:	4b41      	ldr	r3, [pc, #260]	; (30003d78 <BOOT_SOC_ClkSet+0x210>)
30003c74:	2249      	movs	r2, #73	; 0x49
30003c76:	493f      	ldr	r1, [pc, #252]	; (30003d74 <BOOT_SOC_ClkSet+0x20c>)
30003c78:	2004      	movs	r0, #4
30003c7a:	f8cd 8000 	str.w	r8, [sp]
30003c7e:	f006 fb4b 	bl	3000a318 <rtk_log_write>
30003c82:	493c      	ldr	r1, [pc, #240]	; (30003d74 <BOOT_SOC_ClkSet+0x20c>)
30003c84:	2249      	movs	r2, #73	; 0x49
30003c86:	4b3d      	ldr	r3, [pc, #244]	; (30003d7c <BOOT_SOC_ClkSet+0x214>)
30003c88:	2004      	movs	r0, #4
30003c8a:	9400      	str	r4, [sp, #0]
30003c8c:	f006 fb44 	bl	3000a318 <rtk_log_write>
30003c90:	4b3b      	ldr	r3, [pc, #236]	; (30003d80 <BOOT_SOC_ClkSet+0x218>)
30003c92:	2000      	movs	r0, #0
30003c94:	4798      	blx	r3
30003c96:	4601      	mov	r1, r0
30003c98:	4b3a      	ldr	r3, [pc, #232]	; (30003d84 <BOOT_SOC_ClkSet+0x21c>)
30003c9a:	2000      	movs	r0, #0
30003c9c:	f021 01ff 	bic.w	r1, r1, #255	; 0xff
30003ca0:	b005      	add	sp, #20
30003ca2:	e8bd 4ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30003ca6:	4718      	bx	r3
30003ca8:	b005      	add	sp, #20
30003caa:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30003cae:	f023 4800 	bic.w	r8, r3, #2147483648	; 0x80000000
30003cb2:	e9d7 2300 	ldrd	r2, r3, [r7]
30003cb6:	fbb3 f8f8 	udiv	r8, r3, r8
30003cba:	e763      	b.n	30003b84 <BOOT_SOC_ClkSet+0x1c>
30003cbc:	6878      	ldr	r0, [r7, #4]
30003cbe:	4b32      	ldr	r3, [pc, #200]	; (30003d88 <BOOT_SOC_ClkSet+0x220>)
30003cc0:	fb0b f000 	mul.w	r0, fp, r0
30003cc4:	4798      	blx	r3
30003cc6:	4b31      	ldr	r3, [pc, #196]	; (30003d8c <BOOT_SOC_ClkSet+0x224>)
30003cc8:	2001      	movs	r0, #1
30003cca:	4798      	blx	r3
30003ccc:	4b24      	ldr	r3, [pc, #144]	; (30003d60 <BOOT_SOC_ClkSet+0x1f8>)
30003cce:	2201      	movs	r2, #1
30003cd0:	f883 2111 	strb.w	r2, [r3, #273]	; 0x111
30003cd4:	e7b4      	b.n	30003c40 <BOOT_SOC_ClkSet+0xd8>
30003cd6:	4c2e      	ldr	r4, [pc, #184]	; (30003d90 <BOOT_SOC_ClkSet+0x228>)
30003cd8:	4b2e      	ldr	r3, [pc, #184]	; (30003d94 <BOOT_SOC_ClkSet+0x22c>)
30003cda:	4798      	blx	r3
30003cdc:	fba4 3400 	umull	r3, r4, r4, r0
30003ce0:	0ca4      	lsrs	r4, r4, #18
30003ce2:	e7be      	b.n	30003c62 <BOOT_SOC_ClkSet+0xfa>
30003ce4:	4b2c      	ldr	r3, [pc, #176]	; (30003d98 <BOOT_SOC_ClkSet+0x230>)
30003ce6:	2001      	movs	r0, #1
30003ce8:	4798      	blx	r3
30003cea:	4b1d      	ldr	r3, [pc, #116]	; (30003d60 <BOOT_SOC_ClkSet+0x1f8>)
30003cec:	f5b9 7fa7 	cmp.w	r9, #334	; 0x14e
30003cf0:	f883 a110 	strb.w	sl, [r3, #272]	; 0x110
30003cf4:	d226      	bcs.n	30003d44 <BOOT_SOC_ClkSet+0x1dc>
30003cf6:	f8df a0a8 	ldr.w	sl, [pc, #168]	; 30003da0 <BOOT_SOC_ClkSet+0x238>
30003cfa:	f89a 3000 	ldrb.w	r3, [sl]
30003cfe:	2b01      	cmp	r3, #1
30003d00:	f47f af69 	bne.w	30003bd6 <BOOT_SOC_ClkSet+0x6e>
30003d04:	f89d 300c 	ldrb.w	r3, [sp, #12]
30003d08:	075a      	lsls	r2, r3, #29
30003d0a:	d508      	bpl.n	30003d1e <BOOT_SOC_ClkSet+0x1b6>
30003d0c:	f5b8 6f96 	cmp.w	r8, #1200	; 0x4b0
30003d10:	f67f af61 	bls.w	30003bd6 <BOOT_SOC_ClkSet+0x6e>
30003d14:	f240 11f9 	movw	r1, #505	; 0x1f9
30003d18:	4820      	ldr	r0, [pc, #128]	; (30003d9c <BOOT_SOC_ClkSet+0x234>)
30003d1a:	f008 f8e1 	bl	3000bee0 <io_assert_failed>
30003d1e:	f5b8 6fa5 	cmp.w	r8, #1320	; 0x528
30003d22:	f67f af58 	bls.w	30003bd6 <BOOT_SOC_ClkSet+0x6e>
30003d26:	f240 11fb 	movw	r1, #507	; 0x1fb
30003d2a:	481c      	ldr	r0, [pc, #112]	; (30003d9c <BOOT_SOC_ClkSet+0x234>)
30003d2c:	f008 f8d8 	bl	3000bee0 <io_assert_failed>
30003d30:	f240 2102 	movw	r1, #514	; 0x202
30003d34:	4819      	ldr	r0, [pc, #100]	; (30003d9c <BOOT_SOC_ClkSet+0x234>)
30003d36:	f008 f8d3 	bl	3000bee0 <io_assert_failed>
30003d3a:	f44f 7100 	mov.w	r1, #512	; 0x200
30003d3e:	4817      	ldr	r0, [pc, #92]	; (30003d9c <BOOT_SOC_ClkSet+0x234>)
30003d40:	f008 f8ce 	bl	3000bee0 <io_assert_failed>
30003d44:	f44f 71fb 	mov.w	r1, #502	; 0x1f6
30003d48:	4814      	ldr	r0, [pc, #80]	; (30003d9c <BOOT_SOC_ClkSet+0x234>)
30003d4a:	f008 f8c9 	bl	3000bee0 <io_assert_failed>
30003d4e:	bf00      	nop
30003d50:	3000f044 	.word	0x3000f044
30003d54:	3000af89 	.word	0x3000af89
30003d58:	51eb851f 	.word	0x51eb851f
30003d5c:	0000c0f9 	.word	0x0000c0f9
30003d60:	23020000 	.word	0x23020000
30003d64:	3000748d 	.word	0x3000748d
30003d68:	42008000 	.word	0x42008000
30003d6c:	ff8f08f8 	.word	0xff8f08f8
30003d70:	3000c9a4 	.word	0x3000c9a4
30003d74:	3000c7c4 	.word	0x3000c7c4
30003d78:	3000c9b8 	.word	0x3000c9b8
30003d7c:	3000c9cc 	.word	0x3000c9cc
30003d80:	00009979 	.word	0x00009979
30003d84:	0000994d 	.word	0x0000994d
30003d88:	30007555 	.word	0x30007555
30003d8c:	30007591 	.word	0x30007591
30003d90:	431bde83 	.word	0x431bde83
30003d94:	000099f5 	.word	0x000099f5
30003d98:	30009425 	.word	0x30009425
30003d9c:	3000c92c 	.word	0x3000c92c
30003da0:	3000eebe 	.word	0x3000eebe
30003da4:	000f4240 	.word	0x000f4240

30003da8 <BOOT_Disable_AP>:
30003da8:	f04f 4382 	mov.w	r3, #1090519040	; 0x41000000
30003dac:	4906      	ldr	r1, [pc, #24]	; (30003dc8 <BOOT_Disable_AP+0x20>)
30003dae:	685a      	ldr	r2, [r3, #4]
30003db0:	f042 0202 	orr.w	r2, r2, #2
30003db4:	605a      	str	r2, [r3, #4]
30003db6:	685a      	ldr	r2, [r3, #4]
30003db8:	f042 0230 	orr.w	r2, r2, #48	; 0x30
30003dbc:	605a      	str	r2, [r3, #4]
30003dbe:	6f8b      	ldr	r3, [r1, #120]	; 0x78
30003dc0:	f043 0303 	orr.w	r3, r3, #3
30003dc4:	678b      	str	r3, [r1, #120]	; 0x78
30003dc6:	4770      	bx	lr
30003dc8:	41000200 	.word	0x41000200

30003dcc <BOOT_Enable_AP>:
30003dcc:	b538      	push	{r3, r4, r5, lr}
30003dce:	f04f 4482 	mov.w	r4, #1090519040	; 0x41000000
30003dd2:	2201      	movs	r2, #1
30003dd4:	2104      	movs	r1, #4
30003dd6:	481e      	ldr	r0, [pc, #120]	; (30003e50 <BOOT_Enable_AP+0x84>)
30003dd8:	4b1e      	ldr	r3, [pc, #120]	; (30003e54 <BOOT_Enable_AP+0x88>)
30003dda:	4798      	blx	r3
30003ddc:	6863      	ldr	r3, [r4, #4]
30003dde:	4d1e      	ldr	r5, [pc, #120]	; (30003e58 <BOOT_Enable_AP+0x8c>)
30003de0:	2032      	movs	r0, #50	; 0x32
30003de2:	f043 0302 	orr.w	r3, r3, #2
30003de6:	6063      	str	r3, [r4, #4]
30003de8:	6863      	ldr	r3, [r4, #4]
30003dea:	f043 0330 	orr.w	r3, r3, #48	; 0x30
30003dee:	6063      	str	r3, [r4, #4]
30003df0:	6823      	ldr	r3, [r4, #0]
30003df2:	f043 0302 	orr.w	r3, r3, #2
30003df6:	6023      	str	r3, [r4, #0]
30003df8:	47a8      	blx	r5
30003dfa:	6823      	ldr	r3, [r4, #0]
30003dfc:	2032      	movs	r0, #50	; 0x32
30003dfe:	f043 0304 	orr.w	r3, r3, #4
30003e02:	6023      	str	r3, [r4, #0]
30003e04:	6823      	ldr	r3, [r4, #0]
30003e06:	f043 0301 	orr.w	r3, r3, #1
30003e0a:	6023      	str	r3, [r4, #0]
30003e0c:	6823      	ldr	r3, [r4, #0]
30003e0e:	f043 0310 	orr.w	r3, r3, #16
30003e12:	6023      	str	r3, [r4, #0]
30003e14:	47a8      	blx	r5
30003e16:	6823      	ldr	r3, [r4, #0]
30003e18:	f44f 70fa 	mov.w	r0, #500	; 0x1f4
30003e1c:	f043 0340 	orr.w	r3, r3, #64	; 0x40
30003e20:	6023      	str	r3, [r4, #0]
30003e22:	47a8      	blx	r5
30003e24:	6862      	ldr	r2, [r4, #4]
30003e26:	490d      	ldr	r1, [pc, #52]	; (30003e5c <BOOT_Enable_AP+0x90>)
30003e28:	f022 0210 	bic.w	r2, r2, #16
30003e2c:	4b0c      	ldr	r3, [pc, #48]	; (30003e60 <BOOT_Enable_AP+0x94>)
30003e2e:	6062      	str	r2, [r4, #4]
30003e30:	6862      	ldr	r2, [r4, #4]
30003e32:	f022 0202 	bic.w	r2, r2, #2
30003e36:	6062      	str	r2, [r4, #4]
30003e38:	68e2      	ldr	r2, [r4, #12]
30003e3a:	f042 0201 	orr.w	r2, r2, #1
30003e3e:	60e2      	str	r2, [r4, #12]
30003e40:	68a2      	ldr	r2, [r4, #8]
30003e42:	f042 0201 	orr.w	r2, r2, #1
30003e46:	60a2      	str	r2, [r4, #8]
30003e48:	684a      	ldr	r2, [r1, #4]
30003e4a:	4313      	orrs	r3, r2
30003e4c:	604b      	str	r3, [r1, #4]
30003e4e:	bd38      	pop	{r3, r4, r5, pc}
30003e50:	4200c000 	.word	0x4200c000
30003e54:	0000aab9 	.word	0x0000aab9
30003e58:	00009b2d 	.word	0x00009b2d
30003e5c:	41000200 	.word	0x41000200
30003e60:	01001111 	.word	0x01001111

30003e64 <BOOT_Share_Memory_Patch>:
30003e64:	f04f 4282 	mov.w	r2, #1090519040	; 0x41000000
30003e68:	481a      	ldr	r0, [pc, #104]	; (30003ed4 <BOOT_Share_Memory_Patch+0x70>)
30003e6a:	491b      	ldr	r1, [pc, #108]	; (30003ed8 <BOOT_Share_Memory_Patch+0x74>)
30003e6c:	b410      	push	{r4}
30003e6e:	6b13      	ldr	r3, [r2, #48]	; 0x30
30003e70:	f04f 5408 	mov.w	r4, #570425344	; 0x22000000
30003e74:	f443 23c0 	orr.w	r3, r3, #393216	; 0x60000
30003e78:	6313      	str	r3, [r2, #48]	; 0x30
30003e7a:	6823      	ldr	r3, [r4, #0]
30003e7c:	6804      	ldr	r4, [r0, #0]
30003e7e:	f500 3000 	add.w	r0, r0, #131072	; 0x20000
30003e82:	6809      	ldr	r1, [r1, #0]
30003e84:	4423      	add	r3, r4
30003e86:	6804      	ldr	r4, [r0, #0]
30003e88:	f500 205e 	add.w	r0, r0, #909312	; 0xde000
30003e8c:	440b      	add	r3, r1
30003e8e:	4913      	ldr	r1, [pc, #76]	; (30003edc <BOOT_Share_Memory_Patch+0x78>)
30003e90:	4423      	add	r3, r4
30003e92:	6809      	ldr	r1, [r1, #0]
30003e94:	6844      	ldr	r4, [r0, #4]
30003e96:	f04f 5000 	mov.w	r0, #536870912	; 0x20000000
30003e9a:	440b      	add	r3, r1
30003e9c:	6b11      	ldr	r1, [r2, #48]	; 0x30
30003e9e:	f421 21c0 	bic.w	r1, r1, #393216	; 0x60000
30003ea2:	4423      	add	r3, r4
30003ea4:	6311      	str	r1, [r2, #48]	; 0x30
30003ea6:	4a0e      	ldr	r2, [pc, #56]	; (30003ee0 <BOOT_Share_Memory_Patch+0x7c>)
30003ea8:	6804      	ldr	r4, [r0, #0]
30003eaa:	f500 3000 	add.w	r0, r0, #131072	; 0x20000
30003eae:	6812      	ldr	r2, [r2, #0]
30003eb0:	4423      	add	r3, r4
30003eb2:	490c      	ldr	r1, [pc, #48]	; (30003ee4 <BOOT_Share_Memory_Patch+0x80>)
30003eb4:	6804      	ldr	r4, [r0, #0]
30003eb6:	4413      	add	r3, r2
30003eb8:	6809      	ldr	r1, [r1, #0]
30003eba:	f04f 520c 	mov.w	r2, #587202560	; 0x23000000
30003ebe:	480a      	ldr	r0, [pc, #40]	; (30003ee8 <BOOT_Share_Memory_Patch+0x84>)
30003ec0:	4423      	add	r3, r4
30003ec2:	f85d 4b04 	ldr.w	r4, [sp], #4
30003ec6:	6812      	ldr	r2, [r2, #0]
30003ec8:	440b      	add	r3, r1
30003eca:	6800      	ldr	r0, [r0, #0]
30003ecc:	4413      	add	r3, r2
30003ece:	4418      	add	r0, r3
30003ed0:	4770      	bx	lr
30003ed2:	bf00      	nop
30003ed4:	2200c000 	.word	0x2200c000
30003ed8:	22018000 	.word	0x22018000
30003edc:	22100000 	.word	0x22100000
30003ee0:	20010000 	.word	0x20010000
30003ee4:	20030000 	.word	0x20030000
30003ee8:	23010000 	.word	0x23010000

30003eec <BOOT_Log_Init>:
30003eec:	4b10      	ldr	r3, [pc, #64]	; (30003f30 <BOOT_Log_Init+0x44>)
30003eee:	b510      	push	{r4, lr}
30003ef0:	781b      	ldrb	r3, [r3, #0]
30003ef2:	b93b      	cbnz	r3, 30003f04 <BOOT_Log_Init+0x18>
30003ef4:	4c0f      	ldr	r4, [pc, #60]	; (30003f34 <BOOT_Log_Init+0x48>)
30003ef6:	4623      	mov	r3, r4
30003ef8:	2201      	movs	r2, #1
30003efa:	2102      	movs	r1, #2
30003efc:	480e      	ldr	r0, [pc, #56]	; (30003f38 <BOOT_Log_Init+0x4c>)
30003efe:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
30003f02:	4718      	bx	r3
30003f04:	4b0d      	ldr	r3, [pc, #52]	; (30003f3c <BOOT_Log_Init+0x50>)
30003f06:	4798      	blx	r3
30003f08:	3802      	subs	r0, #2
30003f0a:	2801      	cmp	r0, #1
30003f0c:	d9f2      	bls.n	30003ef4 <BOOT_Log_Init+0x8>
30003f0e:	4b0c      	ldr	r3, [pc, #48]	; (30003f40 <BOOT_Log_Init+0x54>)
30003f10:	4798      	blx	r3
30003f12:	4c08      	ldr	r4, [pc, #32]	; (30003f34 <BOOT_Log_Init+0x48>)
30003f14:	2200      	movs	r2, #0
30003f16:	2101      	movs	r1, #1
30003f18:	4807      	ldr	r0, [pc, #28]	; (30003f38 <BOOT_Log_Init+0x4c>)
30003f1a:	47a0      	blx	r4
30003f1c:	2101      	movs	r1, #1
30003f1e:	4806      	ldr	r0, [pc, #24]	; (30003f38 <BOOT_Log_Init+0x4c>)
30003f20:	4b08      	ldr	r3, [pc, #32]	; (30003f44 <BOOT_Log_Init+0x58>)
30003f22:	4798      	blx	r3
30003f24:	2201      	movs	r2, #1
30003f26:	4804      	ldr	r0, [pc, #16]	; (30003f38 <BOOT_Log_Init+0x4c>)
30003f28:	4611      	mov	r1, r2
30003f2a:	47a0      	blx	r4
30003f2c:	e7e3      	b.n	30003ef6 <BOOT_Log_Init+0xa>
30003f2e:	bf00      	nop
30003f30:	3000f960 	.word	0x3000f960
30003f34:	0000aab9 	.word	0x0000aab9
30003f38:	4200c000 	.word	0x4200c000
30003f3c:	0000c0f9 	.word	0x0000c0f9
30003f40:	3000b20d 	.word	0x3000b20d
30003f44:	0000aaa5 	.word	0x0000aaa5

30003f48 <BOOT_Image1>:
30003f48:	4885      	ldr	r0, [pc, #532]	; (30004160 <BOOT_Image1+0x218>)
30003f4a:	2100      	movs	r1, #0
30003f4c:	4a85      	ldr	r2, [pc, #532]	; (30004164 <BOOT_Image1+0x21c>)
30003f4e:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
30003f52:	1a12      	subs	r2, r2, r0
30003f54:	b084      	sub	sp, #16
30003f56:	4c84      	ldr	r4, [pc, #528]	; (30004168 <BOOT_Image1+0x220>)
30003f58:	47a0      	blx	r4
30003f5a:	f7ff fc99 	bl	30003890 <BOOT_ReasonSet>
30003f5e:	f003 fb47 	bl	300075f0 <BOOT_Reason>
30003f62:	b128      	cbz	r0, 30003f70 <BOOT_Image1+0x28>
30003f64:	4a81      	ldr	r2, [pc, #516]	; (3000416c <BOOT_Image1+0x224>)
30003f66:	4b82      	ldr	r3, [pc, #520]	; (30004170 <BOOT_Image1+0x228>)
30003f68:	f8d2 2100 	ldr.w	r2, [r2, #256]	; 0x100
30003f6c:	429a      	cmp	r2, r3
30003f6e:	d008      	beq.n	30003f82 <BOOT_Image1+0x3a>
30003f70:	4d7e      	ldr	r5, [pc, #504]	; (3000416c <BOOT_Image1+0x224>)
30003f72:	f44f 7200 	mov.w	r2, #512	; 0x200
30003f76:	2100      	movs	r1, #0
30003f78:	4628      	mov	r0, r5
30003f7a:	47a0      	blx	r4
30003f7c:	4b7c      	ldr	r3, [pc, #496]	; (30004170 <BOOT_Image1+0x228>)
30003f7e:	f8c5 3100 	str.w	r3, [r5, #256]	; 0x100
30003f82:	f008 fa23 	bl	3000c3cc <BOOT_VerCheck>
30003f86:	4b7b      	ldr	r3, [pc, #492]	; (30004174 <BOOT_Image1+0x22c>)
30003f88:	4798      	blx	r3
30003f8a:	b920      	cbnz	r0, 30003f96 <BOOT_Image1+0x4e>
30003f8c:	4a7a      	ldr	r2, [pc, #488]	; (30004178 <BOOT_Image1+0x230>)
30003f8e:	6893      	ldr	r3, [r2, #8]
30003f90:	f043 0302 	orr.w	r3, r3, #2
30003f94:	6093      	str	r3, [r2, #8]
30003f96:	4d79      	ldr	r5, [pc, #484]	; (3000417c <BOOT_Image1+0x234>)
30003f98:	4b79      	ldr	r3, [pc, #484]	; (30004180 <BOOT_Image1+0x238>)
30003f9a:	4798      	blx	r3
30003f9c:	782c      	ldrb	r4, [r5, #0]
30003f9e:	2c00      	cmp	r4, #0
30003fa0:	f000 80d0 	beq.w	30004144 <BOOT_Image1+0x1fc>
30003fa4:	2000      	movs	r0, #0
30003fa6:	4b77      	ldr	r3, [pc, #476]	; (30004184 <BOOT_Image1+0x23c>)
30003fa8:	4798      	blx	r3
30003faa:	4b77      	ldr	r3, [pc, #476]	; (30004188 <BOOT_Image1+0x240>)
30003fac:	2001      	movs	r0, #1
30003fae:	4798      	blx	r3
30003fb0:	4e76      	ldr	r6, [pc, #472]	; (3000418c <BOOT_Image1+0x244>)
30003fb2:	f7ff fdd9 	bl	30003b68 <BOOT_SOC_ClkSet>
30003fb6:	f241 040a 	movw	r4, #4106	; 0x100a
30003fba:	2028      	movs	r0, #40	; 0x28
30003fbc:	8833      	ldrh	r3, [r6, #0]
30003fbe:	4f74      	ldr	r7, [pc, #464]	; (30004190 <BOOT_Image1+0x248>)
30003fc0:	b29b      	uxth	r3, r3
30003fc2:	431c      	orrs	r4, r3
30003fc4:	4b73      	ldr	r3, [pc, #460]	; (30004194 <BOOT_Image1+0x24c>)
30003fc6:	8034      	strh	r4, [r6, #0]
30003fc8:	4798      	blx	r3
30003fca:	88b3      	ldrh	r3, [r6, #4]
30003fcc:	4a72      	ldr	r2, [pc, #456]	; (30004198 <BOOT_Image1+0x250>)
30003fce:	b29b      	uxth	r3, r3
30003fd0:	ea23 0304 	bic.w	r3, r3, r4
30003fd4:	80b3      	strh	r3, [r6, #4]
30003fd6:	8873      	ldrh	r3, [r6, #2]
30003fd8:	b29b      	uxth	r3, r3
30003fda:	431c      	orrs	r4, r3
30003fdc:	8074      	strh	r4, [r6, #2]
30003fde:	4790      	blx	r2
30003fe0:	f000 043f 	and.w	r4, r0, #63	; 0x3f
30003fe4:	4b6d      	ldr	r3, [pc, #436]	; (3000419c <BOOT_Image1+0x254>)
30003fe6:	2249      	movs	r2, #73	; 0x49
30003fe8:	9003      	str	r0, [sp, #12]
30003fea:	2004      	movs	r0, #4
30003fec:	496c      	ldr	r1, [pc, #432]	; (300041a0 <BOOT_Image1+0x258>)
30003fee:	9400      	str	r4, [sp, #0]
30003ff0:	f006 f992 	bl	3000a318 <rtk_log_write>
30003ff4:	f014 0604 	ands.w	r6, r4, #4
30003ff8:	f8d7 325c 	ldr.w	r3, [r7, #604]	; 0x25c
30003ffc:	f000 8087 	beq.w	3000410e <BOOT_Image1+0x1c6>
30004000:	f423 7398 	bic.w	r3, r3, #304	; 0x130
30004004:	2204      	movs	r2, #4
30004006:	f04f 4188 	mov.w	r1, #1140850688	; 0x44000000
3000400a:	f04f 4090 	mov.w	r0, #1207959552	; 0x48000000
3000400e:	f8c7 325c 	str.w	r3, [r7, #604]	; 0x25c
30004012:	4b56      	ldr	r3, [pc, #344]	; (3000416c <BOOT_Image1+0x224>)
30004014:	f883 2112 	strb.w	r2, [r3, #274]	; 0x112
30004018:	2201      	movs	r2, #1
3000401a:	4b62      	ldr	r3, [pc, #392]	; (300041a4 <BOOT_Image1+0x25c>)
3000401c:	4798      	blx	r3
3000401e:	f8d7 32fc 	ldr.w	r3, [r7, #764]	; 0x2fc
30004022:	f043 0302 	orr.w	r3, r3, #2
30004026:	f8c7 32fc 	str.w	r3, [r7, #764]	; 0x2fc
3000402a:	4a59      	ldr	r2, [pc, #356]	; (30004190 <BOOT_Image1+0x248>)
3000402c:	f8d2 325c 	ldr.w	r3, [r2, #604]	; 0x25c
30004030:	f023 0303 	bic.w	r3, r3, #3
30004034:	f043 0302 	orr.w	r3, r3, #2
30004038:	f8c2 325c 	str.w	r3, [r2, #604]	; 0x25c
3000403c:	f7ff ff56 	bl	30003eec <BOOT_Log_Init>
30004040:	f3ef 8208 	mrs	r2, MSP
30004044:	4b58      	ldr	r3, [pc, #352]	; (300041a8 <BOOT_Image1+0x260>)
30004046:	2004      	movs	r0, #4
30004048:	4955      	ldr	r1, [pc, #340]	; (300041a0 <BOOT_Image1+0x258>)
3000404a:	9200      	str	r2, [sp, #0]
3000404c:	2249      	movs	r2, #73	; 0x49
3000404e:	f006 f963 	bl	3000a318 <rtk_log_write>
30004052:	4a56      	ldr	r2, [pc, #344]	; (300041ac <BOOT_Image1+0x264>)
30004054:	4b56      	ldr	r3, [pc, #344]	; (300041b0 <BOOT_Image1+0x268>)
30004056:	2004      	movs	r0, #4
30004058:	4951      	ldr	r1, [pc, #324]	; (300041a0 <BOOT_Image1+0x258>)
3000405a:	4f56      	ldr	r7, [pc, #344]	; (300041b4 <BOOT_Image1+0x26c>)
3000405c:	e9cd 3200 	strd	r3, r2, [sp]
30004060:	4b55      	ldr	r3, [pc, #340]	; (300041b8 <BOOT_Image1+0x270>)
30004062:	2249      	movs	r2, #73	; 0x49
30004064:	f006 f958 	bl	3000a318 <rtk_log_write>
30004068:	4a54      	ldr	r2, [pc, #336]	; (300041bc <BOOT_Image1+0x274>)
3000406a:	e842 f200 	tt	r2, r2
3000406e:	f3c2 5280 	ubfx	r2, r2, #22, #1
30004072:	494b      	ldr	r1, [pc, #300]	; (300041a0 <BOOT_Image1+0x258>)
30004074:	2004      	movs	r0, #4
30004076:	4b52      	ldr	r3, [pc, #328]	; (300041c0 <BOOT_Image1+0x278>)
30004078:	9200      	str	r2, [sp, #0]
3000407a:	2249      	movs	r2, #73	; 0x49
3000407c:	f006 f94c 	bl	3000a318 <rtk_log_write>
30004080:	4b50      	ldr	r3, [pc, #320]	; (300041c4 <BOOT_Image1+0x27c>)
30004082:	681b      	ldr	r3, [r3, #0]
30004084:	4798      	blx	r3
30004086:	f7ff f837 	bl	300030f8 <BOOT_RccConfig>
3000408a:	f7ff f8a1 	bl	300031d0 <BOOT_CACHEWRR_Set>
3000408e:	4b4e      	ldr	r3, [pc, #312]	; (300041c8 <BOOT_Image1+0x280>)
30004090:	6818      	ldr	r0, [r3, #0]
30004092:	f7ff f8cb 	bl	3000322c <BOOT_TCMSet>
30004096:	4b4d      	ldr	r3, [pc, #308]	; (300041cc <BOOT_Image1+0x284>)
30004098:	695b      	ldr	r3, [r3, #20]
3000409a:	03db      	lsls	r3, r3, #15
3000409c:	d514      	bpl.n	300040c8 <BOOT_Image1+0x180>
3000409e:	f007 031f 	and.w	r3, r7, #31
300040a2:	f103 0174 	add.w	r1, r3, #116	; 0x74
300040a6:	2b00      	cmp	r3, #0
300040a8:	d156      	bne.n	30004158 <BOOT_Image1+0x210>
300040aa:	463b      	mov	r3, r7
300040ac:	f3bf 8f4f 	dsb	sy
300040b0:	4846      	ldr	r0, [pc, #280]	; (300041cc <BOOT_Image1+0x284>)
300040b2:	4419      	add	r1, r3
300040b4:	f8c0 3268 	str.w	r3, [r0, #616]	; 0x268
300040b8:	3320      	adds	r3, #32
300040ba:	1aca      	subs	r2, r1, r3
300040bc:	2a00      	cmp	r2, #0
300040be:	dcf9      	bgt.n	300040b4 <BOOT_Image1+0x16c>
300040c0:	f3bf 8f4f 	dsb	sy
300040c4:	f3bf 8f6f 	isb	sy
300040c8:	4b31      	ldr	r3, [pc, #196]	; (30004190 <BOOT_Image1+0x248>)
300040ca:	2200      	movs	r2, #0
300040cc:	f8c3 7270 	str.w	r7, [r3, #624]	; 0x270
300040d0:	4b26      	ldr	r3, [pc, #152]	; (3000416c <BOOT_Image1+0x224>)
300040d2:	f8c3 210c 	str.w	r2, [r3, #268]	; 0x10c
300040d6:	f001 fedd 	bl	30005e94 <flash_highspeed_setup>
300040da:	4b3d      	ldr	r3, [pc, #244]	; (300041d0 <BOOT_Image1+0x288>)
300040dc:	2249      	movs	r2, #73	; 0x49
300040de:	4930      	ldr	r1, [pc, #192]	; (300041a0 <BOOT_Image1+0x258>)
300040e0:	2004      	movs	r0, #4
300040e2:	9400      	str	r4, [sp, #0]
300040e4:	f006 f918 	bl	3000a318 <rtk_log_write>
300040e8:	2e00      	cmp	r6, #0
300040ea:	f040 810c 	bne.w	30004306 <BOOT_Image1+0x3be>
300040ee:	782b      	ldrb	r3, [r5, #0]
300040f0:	2b00      	cmp	r3, #0
300040f2:	d07b      	beq.n	300041ec <BOOT_Image1+0x2a4>
300040f4:	4e37      	ldr	r6, [pc, #220]	; (300041d4 <BOOT_Image1+0x28c>)
300040f6:	4d38      	ldr	r5, [pc, #224]	; (300041d8 <BOOT_Image1+0x290>)
300040f8:	4c29      	ldr	r4, [pc, #164]	; (300041a0 <BOOT_Image1+0x258>)
300040fa:	462b      	mov	r3, r5
300040fc:	2245      	movs	r2, #69	; 0x45
300040fe:	4621      	mov	r1, r4
30004100:	2002      	movs	r0, #2
30004102:	f006 f909 	bl	3000a318 <rtk_log_write>
30004106:	f241 3088 	movw	r0, #5000	; 0x1388
3000410a:	47b0      	blx	r6
3000410c:	e7f5      	b.n	300040fa <BOOT_Image1+0x1b2>
3000410e:	f023 0330 	bic.w	r3, r3, #48	; 0x30
30004112:	2202      	movs	r2, #2
30004114:	4931      	ldr	r1, [pc, #196]	; (300041dc <BOOT_Image1+0x294>)
30004116:	f04f 4082 	mov.w	r0, #1090519040	; 0x41000000
3000411a:	f8c7 325c 	str.w	r3, [r7, #604]	; 0x25c
3000411e:	4b13      	ldr	r3, [pc, #76]	; (3000416c <BOOT_Image1+0x224>)
30004120:	f8df 8080 	ldr.w	r8, [pc, #128]	; 300041a4 <BOOT_Image1+0x25c>
30004124:	f883 2112 	strb.w	r2, [r3, #274]	; 0x112
30004128:	2201      	movs	r2, #1
3000412a:	47c0      	blx	r8
3000412c:	2201      	movs	r2, #1
3000412e:	492c      	ldr	r1, [pc, #176]	; (300041e0 <BOOT_Image1+0x298>)
30004130:	f04f 4081 	mov.w	r0, #1082130432	; 0x40800000
30004134:	47c0      	blx	r8
30004136:	f8d7 32fc 	ldr.w	r3, [r7, #764]	; 0x2fc
3000413a:	f043 0303 	orr.w	r3, r3, #3
3000413e:	f8c7 32fc 	str.w	r3, [r7, #764]	; 0x2fc
30004142:	e772      	b.n	3000402a <BOOT_Image1+0xe2>
30004144:	4b10      	ldr	r3, [pc, #64]	; (30004188 <BOOT_Image1+0x240>)
30004146:	2001      	movs	r0, #1
30004148:	4798      	blx	r3
3000414a:	4620      	mov	r0, r4
3000414c:	4b25      	ldr	r3, [pc, #148]	; (300041e4 <BOOT_Image1+0x29c>)
3000414e:	4798      	blx	r3
30004150:	4b25      	ldr	r3, [pc, #148]	; (300041e8 <BOOT_Image1+0x2a0>)
30004152:	2001      	movs	r0, #1
30004154:	4798      	blx	r3
30004156:	e72b      	b.n	30003fb0 <BOOT_Image1+0x68>
30004158:	f027 031f 	bic.w	r3, r7, #31
3000415c:	e7a6      	b.n	300040ac <BOOT_Image1+0x164>
3000415e:	bf00      	nop
30004160:	3000f0d0 	.word	0x3000f0d0
30004164:	3000f9e0 	.word	0x3000f9e0
30004168:	00012be5 	.word	0x00012be5
3000416c:	23020000 	.word	0x23020000
30004170:	6969a5a5 	.word	0x6969a5a5
30004174:	0000c179 	.word	0x0000c179
30004178:	2001c00c 	.word	0x2001c00c
3000417c:	3000f961 	.word	0x3000f961
30004180:	30009445 	.word	0x30009445
30004184:	300092b9 	.word	0x300092b9
30004188:	300091d9 	.word	0x300091d9
3000418c:	42008200 	.word	0x42008200
30004190:	42008000 	.word	0x42008000
30004194:	00009b2d 	.word	0x00009b2d
30004198:	3000af89 	.word	0x3000af89
3000419c:	3000c9e0 	.word	0x3000c9e0
300041a0:	3000c7c4 	.word	0x3000c7c4
300041a4:	0000b479 	.word	0x0000b479
300041a8:	3000c9f8 	.word	0x3000c9f8
300041ac:	3000ca24 	.word	0x3000ca24
300041b0:	3000ca30 	.word	0x3000ca30
300041b4:	2001c01c 	.word	0x2001c01c
300041b8:	3000ca10 	.word	0x3000ca10
300041bc:	30003f49 	.word	0x30003f49
300041c0:	3000ca3c 	.word	0x3000ca3c
300041c4:	00033000 	.word	0x00033000
300041c8:	3000f964 	.word	0x3000f964
300041cc:	e000ed00 	.word	0xe000ed00
300041d0:	3000ca54 	.word	0x3000ca54
300041d4:	00009be5 	.word	0x00009be5
300041d8:	3000ca90 	.word	0x3000ca90
300041dc:	40080000 	.word	0x40080000
300041e0:	40040000 	.word	0x40040000
300041e4:	30009405 	.word	0x30009405
300041e8:	30009375 	.word	0x30009375
300041ec:	4b5b      	ldr	r3, [pc, #364]	; (3000435c <BOOT_Image1+0x414>)
300041ee:	4798      	blx	r3
300041f0:	2801      	cmp	r0, #1
300041f2:	f04f 0249 	mov.w	r2, #73	; 0x49
300041f6:	495a      	ldr	r1, [pc, #360]	; (30004360 <BOOT_Image1+0x418>)
300041f8:	bf08      	it	eq
300041fa:	4b5a      	ldreq	r3, [pc, #360]	; (30004364 <BOOT_Image1+0x41c>)
300041fc:	f04f 0004 	mov.w	r0, #4
30004200:	bf18      	it	ne
30004202:	4b59      	ldrne	r3, [pc, #356]	; (30004368 <BOOT_Image1+0x420>)
30004204:	f006 f888 	bl	3000a318 <rtk_log_write>
30004208:	f7ff f946 	bl	30003498 <BOOT_DDR_Init>
3000420c:	4a57      	ldr	r2, [pc, #348]	; (3000436c <BOOT_Image1+0x424>)
3000420e:	f8d2 3290 	ldr.w	r3, [r2, #656]	; 0x290
30004212:	f423 6340 	bic.w	r3, r3, #3072	; 0xc00
30004216:	f443 6300 	orr.w	r3, r3, #2048	; 0x800
3000421a:	f8c2 3290 	str.w	r3, [r2, #656]	; 0x290
3000421e:	4b53      	ldr	r3, [pc, #332]	; (3000436c <BOOT_Image1+0x424>)
30004220:	2400      	movs	r4, #0
30004222:	f8c3 42d0 	str.w	r4, [r3, #720]	; 0x2d0
30004226:	f8c3 42d4 	str.w	r4, [r3, #724]	; 0x2d4
3000422a:	f8c3 42d8 	str.w	r4, [r3, #728]	; 0x2d8
3000422e:	f8c3 40d0 	str.w	r4, [r3, #208]	; 0xd0
30004232:	f7ff fe17 	bl	30003e64 <BOOT_Share_Memory_Patch>
30004236:	4b4e      	ldr	r3, [pc, #312]	; (30004370 <BOOT_Image1+0x428>)
30004238:	4798      	blx	r3
3000423a:	1c45      	adds	r5, r0, #1
3000423c:	4b4d      	ldr	r3, [pc, #308]	; (30004374 <BOOT_Image1+0x42c>)
3000423e:	2004      	movs	r0, #4
30004240:	4947      	ldr	r1, [pc, #284]	; (30004360 <BOOT_Image1+0x418>)
30004242:	2249      	movs	r2, #73	; 0x49
30004244:	9500      	str	r5, [sp, #0]
30004246:	f006 f867 	bl	3000a318 <rtk_log_write>
3000424a:	f000 f8b5 	bl	300043b8 <BOOT_RAM_TZCfg>
3000424e:	4622      	mov	r2, r4
30004250:	2101      	movs	r1, #1
30004252:	4849      	ldr	r0, [pc, #292]	; (30004378 <BOOT_Image1+0x430>)
30004254:	4b49      	ldr	r3, [pc, #292]	; (3000437c <BOOT_Image1+0x434>)
30004256:	4798      	blx	r3
30004258:	4b49      	ldr	r3, [pc, #292]	; (30004380 <BOOT_Image1+0x438>)
3000425a:	2008      	movs	r0, #8
3000425c:	4798      	blx	r3
3000425e:	f7ff fc41 	bl	30003ae4 <BOOT_Enable_KM0>
30004262:	4b48      	ldr	r3, [pc, #288]	; (30004384 <BOOT_Image1+0x43c>)
30004264:	781b      	ldrb	r3, [r3, #0]
30004266:	2b00      	cmp	r3, #0
30004268:	d067      	beq.n	3000433a <BOOT_Image1+0x3f2>
3000426a:	f7ff fdaf 	bl	30003dcc <BOOT_Enable_AP>
3000426e:	4a46      	ldr	r2, [pc, #280]	; (30004388 <BOOT_Image1+0x440>)
30004270:	f892 3066 	ldrb.w	r3, [r2, #102]	; 0x66
30004274:	f043 0306 	orr.w	r3, r3, #6
30004278:	f882 3066 	strb.w	r3, [r2, #102]	; 0x66
3000427c:	4942      	ldr	r1, [pc, #264]	; (30004388 <BOOT_Image1+0x440>)
3000427e:	2000      	movs	r0, #0
30004280:	4a42      	ldr	r2, [pc, #264]	; (3000438c <BOOT_Image1+0x444>)
30004282:	f891 306a 	ldrb.w	r3, [r1, #106]	; 0x6a
30004286:	4c42      	ldr	r4, [pc, #264]	; (30004390 <BOOT_Image1+0x448>)
30004288:	f043 0380 	orr.w	r3, r3, #128	; 0x80
3000428c:	f881 306a 	strb.w	r3, [r1, #106]	; 0x6a
30004290:	6895      	ldr	r5, [r2, #8]
30004292:	6813      	ldr	r3, [r2, #0]
30004294:	606b      	str	r3, [r5, #4]
30004296:	60a5      	str	r5, [r4, #8]
30004298:	f005 fb24 	bl	300098e4 <Fault_Hanlder_Redirect>
3000429c:	4b3d      	ldr	r3, [pc, #244]	; (30004394 <BOOT_Image1+0x44c>)
3000429e:	493e      	ldr	r1, [pc, #248]	; (30004398 <BOOT_Image1+0x450>)
300042a0:	6a5a      	ldr	r2, [r3, #36]	; 0x24
300042a2:	f442 2270 	orr.w	r2, r2, #983040	; 0xf0000
300042a6:	625a      	str	r2, [r3, #36]	; 0x24
300042a8:	6a62      	ldr	r2, [r4, #36]	; 0x24
300042aa:	f442 22e0 	orr.w	r2, r2, #458752	; 0x70000
300042ae:	6262      	str	r2, [r4, #36]	; 0x24
300042b0:	f8d3 2088 	ldr.w	r2, [r3, #136]	; 0x88
300042b4:	f442 0270 	orr.w	r2, r2, #15728640	; 0xf00000
300042b8:	f8c3 2088 	str.w	r2, [r3, #136]	; 0x88
300042bc:	f8d4 2088 	ldr.w	r2, [r4, #136]	; 0x88
300042c0:	f442 0270 	orr.w	r2, r2, #15728640	; 0xf00000
300042c4:	f8c4 2088 	str.w	r2, [r4, #136]	; 0x88
300042c8:	f8d3 208c 	ldr.w	r2, [r3, #140]	; 0x8c
300042cc:	f442 6240 	orr.w	r2, r2, #3072	; 0xc00
300042d0:	f8c3 208c 	str.w	r2, [r3, #140]	; 0x8c
300042d4:	689b      	ldr	r3, [r3, #8]
300042d6:	618b      	str	r3, [r1, #24]
300042d8:	f7ff fa66 	bl	300037a8 <BOOT_NVICBackup_HP>
300042dc:	4b2f      	ldr	r3, [pc, #188]	; (3000439c <BOOT_Image1+0x454>)
300042de:	f383 8888 	msr	MSP_NS, r3
300042e2:	f5a3 6300 	sub.w	r3, r3, #2048	; 0x800
300042e6:	f383 8809 	msr	PSP, r3
300042ea:	6869      	ldr	r1, [r5, #4]
300042ec:	2004      	movs	r0, #4
300042ee:	4b2c      	ldr	r3, [pc, #176]	; (300043a0 <BOOT_Image1+0x458>)
300042f0:	2249      	movs	r2, #73	; 0x49
300042f2:	9100      	str	r1, [sp, #0]
300042f4:	491a      	ldr	r1, [pc, #104]	; (30004360 <BOOT_Image1+0x418>)
300042f6:	f006 f80f 	bl	3000a318 <rtk_log_write>
300042fa:	6868      	ldr	r0, [r5, #4]
300042fc:	b004      	add	sp, #16
300042fe:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
30004302:	f7fe bea9 	b.w	30003058 <BOOT_NsStart>
30004306:	2004      	movs	r0, #4
30004308:	2249      	movs	r2, #73	; 0x49
3000430a:	4915      	ldr	r1, [pc, #84]	; (30004360 <BOOT_Image1+0x418>)
3000430c:	4b25      	ldr	r3, [pc, #148]	; (300043a4 <BOOT_Image1+0x45c>)
3000430e:	f006 f803 	bl	3000a318 <rtk_log_write>
30004312:	4b25      	ldr	r3, [pc, #148]	; (300043a8 <BOOT_Image1+0x460>)
30004314:	4798      	blx	r3
30004316:	f7ff f8fd 	bl	30003514 <BOOT_PSRAM_Init>
3000431a:	7828      	ldrb	r0, [r5, #0]
3000431c:	2800      	cmp	r0, #0
3000431e:	f47f af7e 	bne.w	3000421e <BOOT_Image1+0x2d6>
30004322:	4b22      	ldr	r3, [pc, #136]	; (300043ac <BOOT_Image1+0x464>)
30004324:	4798      	blx	r3
30004326:	2800      	cmp	r0, #0
30004328:	f43f af79 	beq.w	3000421e <BOOT_Image1+0x2d6>
3000432c:	4b20      	ldr	r3, [pc, #128]	; (300043b0 <BOOT_Image1+0x468>)
3000432e:	2257      	movs	r2, #87	; 0x57
30004330:	490b      	ldr	r1, [pc, #44]	; (30004360 <BOOT_Image1+0x418>)
30004332:	2003      	movs	r0, #3
30004334:	f005 fff0 	bl	3000a318 <rtk_log_write>
30004338:	e771      	b.n	3000421e <BOOT_Image1+0x2d6>
3000433a:	f04f 4382 	mov.w	r3, #1090519040	; 0x41000000
3000433e:	491d      	ldr	r1, [pc, #116]	; (300043b4 <BOOT_Image1+0x46c>)
30004340:	685a      	ldr	r2, [r3, #4]
30004342:	f042 0202 	orr.w	r2, r2, #2
30004346:	605a      	str	r2, [r3, #4]
30004348:	685a      	ldr	r2, [r3, #4]
3000434a:	f042 0230 	orr.w	r2, r2, #48	; 0x30
3000434e:	605a      	str	r2, [r3, #4]
30004350:	6f8b      	ldr	r3, [r1, #120]	; 0x78
30004352:	f043 0303 	orr.w	r3, r3, #3
30004356:	678b      	str	r3, [r1, #120]	; 0x78
30004358:	e790      	b.n	3000427c <BOOT_Image1+0x334>
3000435a:	bf00      	nop
3000435c:	3000b011 	.word	0x3000b011
30004360:	3000c7c4 	.word	0x3000c7c4
30004364:	3000cac8 	.word	0x3000cac8
30004368:	3000cad4 	.word	0x3000cad4
3000436c:	42008000 	.word	0x42008000
30004370:	3000553d 	.word	0x3000553d
30004374:	3000c7dc 	.word	0x3000c7dc
30004378:	4200c000 	.word	0x4200c000
3000437c:	3000b1bd 	.word	0x3000b1bd
30004380:	0000e1d9 	.word	0x0000e1d9
30004384:	3000eebe 	.word	0x3000eebe
30004388:	42008200 	.word	0x42008200
3000438c:	60000020 	.word	0x60000020
30004390:	e002ed00 	.word	0xe002ed00
30004394:	e000ed00 	.word	0xe000ed00
30004398:	3000f0d0 	.word	0x3000f0d0
3000439c:	2001c000 	.word	0x2001c000
300043a0:	3000cae0 	.word	0x3000cae0
300043a4:	3000ca70 	.word	0x3000ca70
300043a8:	300099a5 	.word	0x300099a5
300043ac:	300092d9 	.word	0x300092d9
300043b0:	3000ca80 	.word	0x3000ca80
300043b4:	41000200 	.word	0x41000200

300043b8 <BOOT_RAM_TZCfg>:
300043b8:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
300043bc:	4862      	ldr	r0, [pc, #392]	; (30004548 <BOOT_RAM_TZCfg+0x190>)
300043be:	b087      	sub	sp, #28
300043c0:	4c62      	ldr	r4, [pc, #392]	; (3000454c <BOOT_RAM_TZCfg+0x194>)
300043c2:	4b63      	ldr	r3, [pc, #396]	; (30004550 <BOOT_RAM_TZCfg+0x198>)
300043c4:	f10d 0a10 	add.w	sl, sp, #16
300043c8:	4962      	ldr	r1, [pc, #392]	; (30004554 <BOOT_RAM_TZCfg+0x19c>)
300043ca:	f024 4270 	bic.w	r2, r4, #4026531840	; 0xf0000000
300043ce:	f10d 0b08 	add.w	fp, sp, #8
300043d2:	f8df 91b0 	ldr.w	r9, [pc, #432]	; 30004584 <BOOT_RAM_TZCfg+0x1cc>
300043d6:	e9cd 0303 	strd	r0, r3, [sp, #12]
300043da:	485f      	ldr	r0, [pc, #380]	; (30004558 <BOOT_RAM_TZCfg+0x1a0>)
300043dc:	e9cd 2101 	strd	r2, r1, [sp, #4]
300043e0:	9005      	str	r0, [sp, #20]
300043e2:	f85a 5b04 	ldr.w	r5, [sl], #4
300043e6:	2400      	movs	r4, #0
300043e8:	f85b 6b04 	ldr.w	r6, [fp], #4
300043ec:	2701      	movs	r7, #1
300043ee:	3504      	adds	r5, #4
300043f0:	e00a      	b.n	30004408 <BOOT_RAM_TZCfg+0x50>
300043f2:	f8c8 3004 	str.w	r3, [r8, #4]
300043f6:	6c32      	ldr	r2, [r6, #64]	; 0x40
300043f8:	fa07 f304 	lsl.w	r3, r7, r4
300043fc:	3401      	adds	r4, #1
300043fe:	350c      	adds	r5, #12
30004400:	4313      	orrs	r3, r2
30004402:	2c08      	cmp	r4, #8
30004404:	6433      	str	r3, [r6, #64]	; 0x40
30004406:	d017      	beq.n	30004438 <BOOT_RAM_TZCfg+0x80>
30004408:	f855 3c04 	ldr.w	r3, [r5, #-4]
3000440c:	eb06 08c4 	add.w	r8, r6, r4, lsl #3
30004410:	f023 4270 	bic.w	r2, r3, #4026531840	; 0xf0000000
30004414:	3301      	adds	r3, #1
30004416:	d00f      	beq.n	30004438 <BOOT_RAM_TZCfg+0x80>
30004418:	f846 2034 	str.w	r2, [r6, r4, lsl #3]
3000441c:	682b      	ldr	r3, [r5, #0]
3000441e:	454b      	cmp	r3, r9
30004420:	f023 4370 	bic.w	r3, r3, #4026531840	; 0xf0000000
30004424:	d1e5      	bne.n	300043f2 <BOOT_RAM_TZCfg+0x3a>
30004426:	4b4d      	ldr	r3, [pc, #308]	; (3000455c <BOOT_RAM_TZCfg+0x1a4>)
30004428:	4798      	blx	r3
3000442a:	3801      	subs	r0, #1
3000442c:	4949      	ldr	r1, [pc, #292]	; (30004554 <BOOT_RAM_TZCfg+0x19c>)
3000442e:	f020 4070 	bic.w	r0, r0, #4026531840	; 0xf0000000
30004432:	f8c8 0004 	str.w	r0, [r8, #4]
30004436:	e7de      	b.n	300043f6 <BOOT_RAM_TZCfg+0x3e>
30004438:	428e      	cmp	r6, r1
3000443a:	d074      	beq.n	30004526 <BOOT_RAM_TZCfg+0x16e>
3000443c:	2301      	movs	r3, #1
3000443e:	6473      	str	r3, [r6, #68]	; 0x44
30004440:	ab04      	add	r3, sp, #16
30004442:	459b      	cmp	fp, r3
30004444:	d1cd      	bne.n	300043e2 <BOOT_RAM_TZCfg+0x2a>
30004446:	4b46      	ldr	r3, [pc, #280]	; (30004560 <BOOT_RAM_TZCfg+0x1a8>)
30004448:	f46f 3100 	mvn.w	r1, #131072	; 0x20000
3000444c:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
30004450:	4844      	ldr	r0, [pc, #272]	; (30004564 <BOOT_RAM_TZCfg+0x1ac>)
30004452:	6019      	str	r1, [r3, #0]
30004454:	2501      	movs	r5, #1
30004456:	605a      	str	r2, [r3, #4]
30004458:	2100      	movs	r1, #0
3000445a:	4a43      	ldr	r2, [pc, #268]	; (30004568 <BOOT_RAM_TZCfg+0x1b0>)
3000445c:	4c43      	ldr	r4, [pc, #268]	; (3000456c <BOOT_RAM_TZCfg+0x1b4>)
3000445e:	f8c0 5398 	str.w	r5, [r0, #920]	; 0x398
30004462:	6813      	ldr	r3, [r2, #0]
30004464:	3301      	adds	r3, #1
30004466:	d019      	beq.n	3000449c <BOOT_RAM_TZCfg+0xe4>
30004468:	f8c4 10d8 	str.w	r1, [r4, #216]	; 0xd8
3000446c:	320c      	adds	r2, #12
3000446e:	f852 3c0c 	ldr.w	r3, [r2, #-12]
30004472:	3101      	adds	r1, #1
30004474:	f023 031f 	bic.w	r3, r3, #31
30004478:	2908      	cmp	r1, #8
3000447a:	f8c4 30dc 	str.w	r3, [r4, #220]	; 0xdc
3000447e:	e952 0302 	ldrd	r0, r3, [r2, #-8]
30004482:	ea4f 0343 	mov.w	r3, r3, lsl #1
30004486:	f020 001f 	bic.w	r0, r0, #31
3000448a:	f003 0302 	and.w	r3, r3, #2
3000448e:	ea43 0300 	orr.w	r3, r3, r0
30004492:	f043 0301 	orr.w	r3, r3, #1
30004496:	f8c4 30e0 	str.w	r3, [r4, #224]	; 0xe0
3000449a:	d1e2      	bne.n	30004462 <BOOT_RAM_TZCfg+0xaa>
3000449c:	4833      	ldr	r0, [pc, #204]	; (3000456c <BOOT_RAM_TZCfg+0x1b4>)
3000449e:	2201      	movs	r2, #1
300044a0:	4b33      	ldr	r3, [pc, #204]	; (30004570 <BOOT_RAM_TZCfg+0x1b8>)
300044a2:	f8c0 20d0 	str.w	r2, [r0, #208]	; 0xd0
300044a6:	f8d3 3268 	ldr.w	r3, [r3, #616]	; 0x268
300044aa:	f013 5300 	ands.w	r3, r3, #536870912	; 0x20000000
300044ae:	d11e      	bne.n	300044ee <BOOT_RAM_TZCfg+0x136>
300044b0:	f8c0 3084 	str.w	r3, [r0, #132]	; 0x84
300044b4:	f3bf 8f4f 	dsb	sy
300044b8:	f8d0 5080 	ldr.w	r5, [r0, #128]	; 0x80
300044bc:	f643 76e0 	movw	r6, #16352	; 0x3fe0
300044c0:	f3c5 344e 	ubfx	r4, r5, #13, #15
300044c4:	f3c5 05c9 	ubfx	r5, r5, #3, #10
300044c8:	0164      	lsls	r4, r4, #5
300044ca:	ea04 0106 	and.w	r1, r4, r6
300044ce:	462b      	mov	r3, r5
300044d0:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
300044d4:	3b01      	subs	r3, #1
300044d6:	f8c0 2274 	str.w	r2, [r0, #628]	; 0x274
300044da:	1c5a      	adds	r2, r3, #1
300044dc:	d1f8      	bne.n	300044d0 <BOOT_RAM_TZCfg+0x118>
300044de:	3c20      	subs	r4, #32
300044e0:	f114 0f20 	cmn.w	r4, #32
300044e4:	d1f1      	bne.n	300044ca <BOOT_RAM_TZCfg+0x112>
300044e6:	f3bf 8f4f 	dsb	sy
300044ea:	f3bf 8f6f 	isb	sy
300044ee:	4b1f      	ldr	r3, [pc, #124]	; (3000456c <BOOT_RAM_TZCfg+0x1b4>)
300044f0:	f649 75f7 	movw	r5, #40951	; 0x9ff7
300044f4:	491f      	ldr	r1, [pc, #124]	; (30004574 <BOOT_RAM_TZCfg+0x1bc>)
300044f6:	691c      	ldr	r4, [r3, #16]
300044f8:	4a1f      	ldr	r2, [pc, #124]	; (30004578 <BOOT_RAM_TZCfg+0x1c0>)
300044fa:	f044 0408 	orr.w	r4, r4, #8
300044fe:	611c      	str	r4, [r3, #16]
30004500:	f04f 34ff 	mov.w	r4, #4294967295	; 0xffffffff
30004504:	68d8      	ldr	r0, [r3, #12]
30004506:	4028      	ands	r0, r5
30004508:	4301      	orrs	r1, r0
3000450a:	60d9      	str	r1, [r3, #12]
3000450c:	f46f 0180 	mvn.w	r1, #4194304	; 0x400000
30004510:	f06f 03c0 	mvn.w	r3, #192	; 0xc0
30004514:	f8c2 4280 	str.w	r4, [r2, #640]	; 0x280
30004518:	f8c2 1284 	str.w	r1, [r2, #644]	; 0x284
3000451c:	f8c2 3288 	str.w	r3, [r2, #648]	; 0x288
30004520:	b007      	add	sp, #28
30004522:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30004526:	4b15      	ldr	r3, [pc, #84]	; (3000457c <BOOT_RAM_TZCfg+0x1c4>)
30004528:	781b      	ldrb	r3, [r3, #0]
3000452a:	2b00      	cmp	r3, #0
3000452c:	d188      	bne.n	30004440 <BOOT_RAM_TZCfg+0x88>
3000452e:	9b01      	ldr	r3, [sp, #4]
30004530:	638b      	str	r3, [r1, #56]	; 0x38
30004532:	4b13      	ldr	r3, [pc, #76]	; (30004580 <BOOT_RAM_TZCfg+0x1c8>)
30004534:	f023 4370 	bic.w	r3, r3, #4026531840	; 0xf0000000
30004538:	63cb      	str	r3, [r1, #60]	; 0x3c
3000453a:	6c0b      	ldr	r3, [r1, #64]	; 0x40
3000453c:	f043 0380 	orr.w	r3, r3, #128	; 0x80
30004540:	640b      	str	r3, [r1, #64]	; 0x40
30004542:	2301      	movs	r3, #1
30004544:	644b      	str	r3, [r1, #68]	; 0x44
30004546:	e77b      	b.n	30004440 <BOOT_RAM_TZCfg+0x88>
30004548:	51001200 	.word	0x51001200
3000454c:	70180000 	.word	0x70180000
30004550:	3000d688 	.word	0x3000d688
30004554:	51001a00 	.word	0x51001a00
30004558:	3000d6e8 	.word	0x3000d6e8
3000455c:	3000b035 	.word	0x3000b035
30004560:	51001b00 	.word	0x51001b00
30004564:	500e0000 	.word	0x500e0000
30004568:	3000d748 	.word	0x3000d748
3000456c:	e000ed00 	.word	0xe000ed00
30004570:	42008000 	.word	0x42008000
30004574:	05fa6000 	.word	0x05fa6000
30004578:	e000e100 	.word	0xe000e100
3000457c:	3000eebe 	.word	0x3000eebe
30004580:	704fffff 	.word	0x704fffff
30004584:	607fffff 	.word	0x607fffff

30004588 <BOOT_SbootEn_Check>:
30004588:	b570      	push	{r4, r5, r6, lr}
3000458a:	4b1d      	ldr	r3, [pc, #116]	; (30004600 <BOOT_SbootEn_Check+0x78>)
3000458c:	4604      	mov	r4, r0
3000458e:	4798      	blx	r3
30004590:	b1c0      	cbz	r0, 300045c4 <BOOT_SbootEn_Check+0x3c>
30004592:	4b1c      	ldr	r3, [pc, #112]	; (30004604 <BOOT_SbootEn_Check+0x7c>)
30004594:	4798      	blx	r3
30004596:	4d1c      	ldr	r5, [pc, #112]	; (30004608 <BOOT_SbootEn_Check+0x80>)
30004598:	2320      	movs	r3, #32
3000459a:	4e1c      	ldr	r6, [pc, #112]	; (3000460c <BOOT_SbootEn_Check+0x84>)
3000459c:	f000 011f 	and.w	r1, r0, #31
300045a0:	f04f 0c01 	mov.w	ip, #1
300045a4:	5d8a      	ldrb	r2, [r1, r6]
300045a6:	3b01      	subs	r3, #1
300045a8:	1c48      	adds	r0, r1, #1
300045aa:	b2d2      	uxtb	r2, r2
300045ac:	2aff      	cmp	r2, #255	; 0xff
300045ae:	5462      	strb	r2, [r4, r1]
300045b0:	f000 011f 	and.w	r1, r0, #31
300045b4:	bf18      	it	ne
300045b6:	f885 c000 	strbne.w	ip, [r5]
300045ba:	f013 03ff 	ands.w	r3, r3, #255	; 0xff
300045be:	d1f1      	bne.n	300045a4 <BOOT_SbootEn_Check+0x1c>
300045c0:	7828      	ldrb	r0, [r5, #0]
300045c2:	bd70      	pop	{r4, r5, r6, pc}
300045c4:	4b12      	ldr	r3, [pc, #72]	; (30004610 <BOOT_SbootEn_Check+0x88>)
300045c6:	4798      	blx	r3
300045c8:	b910      	cbnz	r0, 300045d0 <BOOT_SbootEn_Check+0x48>
300045ca:	4b0f      	ldr	r3, [pc, #60]	; (30004608 <BOOT_SbootEn_Check+0x80>)
300045cc:	7818      	ldrb	r0, [r3, #0]
300045ce:	bd70      	pop	{r4, r5, r6, pc}
300045d0:	2201      	movs	r2, #1
300045d2:	4d0d      	ldr	r5, [pc, #52]	; (30004608 <BOOT_SbootEn_Check+0x80>)
300045d4:	4b0b      	ldr	r3, [pc, #44]	; (30004604 <BOOT_SbootEn_Check+0x7c>)
300045d6:	702a      	strb	r2, [r5, #0]
300045d8:	4798      	blx	r3
300045da:	4e0e      	ldr	r6, [pc, #56]	; (30004614 <BOOT_SbootEn_Check+0x8c>)
300045dc:	2320      	movs	r3, #32
300045de:	f000 021f 	and.w	r2, r0, #31
300045e2:	6831      	ldr	r1, [r6, #0]
300045e4:	3b01      	subs	r3, #1
300045e6:	1c50      	adds	r0, r2, #1
300045e8:	f101 4184 	add.w	r1, r1, #1107296256	; 0x42000000
300045ec:	f013 03ff 	ands.w	r3, r3, #255	; 0xff
300045f0:	5c89      	ldrb	r1, [r1, r2]
300045f2:	54a1      	strb	r1, [r4, r2]
300045f4:	f000 021f 	and.w	r2, r0, #31
300045f8:	d1f3      	bne.n	300045e2 <BOOT_SbootEn_Check+0x5a>
300045fa:	7828      	ldrb	r0, [r5, #0]
300045fc:	e7e1      	b.n	300045c2 <BOOT_SbootEn_Check+0x3a>
300045fe:	bf00      	nop
30004600:	0000c2a1 	.word	0x0000c2a1
30004604:	0000ea4d 	.word	0x0000ea4d
30004608:	3000f140 	.word	0x3000f140
3000460c:	42000720 	.word	0x42000720
30004610:	0000c149 	.word	0x0000c149
30004614:	3000efa4 	.word	0x3000efa4

30004618 <BOOT_LoadRDPImg>:
30004618:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
3000461c:	4b49      	ldr	r3, [pc, #292]	; (30004744 <BOOT_LoadRDPImg+0x12c>)
3000461e:	b0d9      	sub	sp, #356	; 0x164
30004620:	468b      	mov	fp, r1
30004622:	4690      	mov	r8, r2
30004624:	4605      	mov	r5, r0
30004626:	2400      	movs	r4, #0
30004628:	2202      	movs	r2, #2
3000462a:	e893 0003 	ldmia.w	r3, {r0, r1}
3000462e:	ab06      	add	r3, sp, #24
30004630:	e883 0003 	stmia.w	r3, {r0, r1}
30004634:	4641      	mov	r1, r8
30004636:	4658      	mov	r0, fp
30004638:	9400      	str	r4, [sp, #0]
3000463a:	f000 fc67 	bl	30004f0c <BOOT_LoadSubImage>
3000463e:	2801      	cmp	r0, #1
30004640:	d06f      	beq.n	30004722 <BOOT_LoadRDPImg+0x10a>
30004642:	4b41      	ldr	r3, [pc, #260]	; (30004748 <BOOT_LoadRDPImg+0x130>)
30004644:	4620      	mov	r0, r4
30004646:	4798      	blx	r3
30004648:	2101      	movs	r1, #1
3000464a:	4b40      	ldr	r3, [pc, #256]	; (3000474c <BOOT_LoadRDPImg+0x134>)
3000464c:	4620      	mov	r0, r4
3000464e:	4798      	blx	r3
30004650:	4b3f      	ldr	r3, [pc, #252]	; (30004750 <BOOT_LoadRDPImg+0x138>)
30004652:	2120      	movs	r1, #32
30004654:	a810      	add	r0, sp, #64	; 0x40
30004656:	f8df a110 	ldr.w	sl, [pc, #272]	; 30004768 <BOOT_LoadRDPImg+0x150>
3000465a:	af08      	add	r7, sp, #32
3000465c:	9404      	str	r4, [sp, #16]
3000465e:	4798      	blx	r3
30004660:	f105 0120 	add.w	r1, r5, #32
30004664:	2210      	movs	r2, #16
30004666:	a80c      	add	r0, sp, #48	; 0x30
30004668:	f8df 9100 	ldr.w	r9, [pc, #256]	; 3000476c <BOOT_LoadRDPImg+0x154>
3000466c:	47d0      	blx	sl
3000466e:	ab06      	add	r3, sp, #24
30004670:	9305      	str	r3, [sp, #20]
30004672:	2210      	movs	r2, #16
30004674:	4641      	mov	r1, r8
30004676:	a818      	add	r0, sp, #96	; 0x60
30004678:	f000 fb7a 	bl	30004d70 <BOOT_ImgCopy>
3000467c:	2310      	movs	r3, #16
3000467e:	aa0c      	add	r2, sp, #48	; 0x30
30004680:	a818      	add	r0, sp, #96	; 0x60
30004682:	4619      	mov	r1, r3
30004684:	9700      	str	r7, [sp, #0]
30004686:	47c8      	blx	r9
30004688:	683b      	ldr	r3, [r7, #0]
3000468a:	4a32      	ldr	r2, [pc, #200]	; (30004754 <BOOT_LoadRDPImg+0x13c>)
3000468c:	4293      	cmp	r3, r2
3000468e:	d14c      	bne.n	3000472a <BOOT_LoadRDPImg+0x112>
30004690:	9905      	ldr	r1, [sp, #20]
30004692:	687b      	ldr	r3, [r7, #4]
30004694:	f851 2b04 	ldr.w	r2, [r1], #4
30004698:	9105      	str	r1, [sp, #20]
3000469a:	492f      	ldr	r1, [pc, #188]	; (30004758 <BOOT_LoadRDPImg+0x140>)
3000469c:	428b      	cmp	r3, r1
3000469e:	d144      	bne.n	3000472a <BOOT_LoadRDPImg+0x112>
300046a0:	9200      	str	r2, [sp, #0]
300046a2:	2004      	movs	r0, #4
300046a4:	68fe      	ldr	r6, [r7, #12]
300046a6:	2249      	movs	r2, #73	; 0x49
300046a8:	4b2c      	ldr	r3, [pc, #176]	; (3000475c <BOOT_LoadRDPImg+0x144>)
300046aa:	3e20      	subs	r6, #32
300046ac:	492c      	ldr	r1, [pc, #176]	; (30004760 <BOOT_LoadRDPImg+0x148>)
300046ae:	9601      	str	r6, [sp, #4]
300046b0:	68bd      	ldr	r5, [r7, #8]
300046b2:	3520      	adds	r5, #32
300046b4:	9502      	str	r5, [sp, #8]
300046b6:	f005 fe2f 	bl	3000a318 <rtk_log_write>
300046ba:	f1bb 0f00 	cmp.w	fp, #0
300046be:	d006      	beq.n	300046ce <BOOT_LoadRDPImg+0xb6>
300046c0:	9a04      	ldr	r2, [sp, #16]
300046c2:	f10b 0304 	add.w	r3, fp, #4
300046c6:	f84b 6032 	str.w	r6, [fp, r2, lsl #3]
300046ca:	f843 5032 	str.w	r5, [r3, r2, lsl #3]
300046ce:	b1dd      	cbz	r5, 30004708 <BOOT_LoadRDPImg+0xf0>
300046d0:	f5b5 7f80 	cmp.w	r5, #256	; 0x100
300046d4:	462c      	mov	r4, r5
300046d6:	4641      	mov	r1, r8
300046d8:	a818      	add	r0, sp, #96	; 0x60
300046da:	bf28      	it	cs
300046dc:	f44f 7480 	movcs.w	r4, #256	; 0x100
300046e0:	4622      	mov	r2, r4
300046e2:	44a0      	add	r8, r4
300046e4:	f000 fb44 	bl	30004d70 <BOOT_ImgCopy>
300046e8:	2310      	movs	r3, #16
300046ea:	aa0c      	add	r2, sp, #48	; 0x30
300046ec:	4621      	mov	r1, r4
300046ee:	9600      	str	r6, [sp, #0]
300046f0:	a818      	add	r0, sp, #96	; 0x60
300046f2:	47c8      	blx	r9
300046f4:	ab18      	add	r3, sp, #96	; 0x60
300046f6:	f1a4 0110 	sub.w	r1, r4, #16
300046fa:	2210      	movs	r2, #16
300046fc:	a80c      	add	r0, sp, #48	; 0x30
300046fe:	4426      	add	r6, r4
30004700:	4419      	add	r1, r3
30004702:	47d0      	blx	sl
30004704:	1b2d      	subs	r5, r5, r4
30004706:	d1e3      	bne.n	300046d0 <BOOT_LoadRDPImg+0xb8>
30004708:	9b04      	ldr	r3, [sp, #16]
3000470a:	b13b      	cbz	r3, 3000471c <BOOT_LoadRDPImg+0x104>
3000470c:	2100      	movs	r1, #0
3000470e:	4b0f      	ldr	r3, [pc, #60]	; (3000474c <BOOT_LoadRDPImg+0x134>)
30004710:	4608      	mov	r0, r1
30004712:	4798      	blx	r3
30004714:	2002      	movs	r0, #2
30004716:	b059      	add	sp, #356	; 0x164
30004718:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
3000471c:	2301      	movs	r3, #1
3000471e:	9304      	str	r3, [sp, #16]
30004720:	e7a7      	b.n	30004672 <BOOT_LoadRDPImg+0x5a>
30004722:	2002      	movs	r0, #2
30004724:	b059      	add	sp, #356	; 0x164
30004726:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
3000472a:	9b04      	ldr	r3, [sp, #16]
3000472c:	aa06      	add	r2, sp, #24
3000472e:	2002      	movs	r0, #2
30004730:	490b      	ldr	r1, [pc, #44]	; (30004760 <BOOT_LoadRDPImg+0x148>)
30004732:	f852 4023 	ldr.w	r4, [r2, r3, lsl #2]
30004736:	2245      	movs	r2, #69	; 0x45
30004738:	4b0a      	ldr	r3, [pc, #40]	; (30004764 <BOOT_LoadRDPImg+0x14c>)
3000473a:	9400      	str	r4, [sp, #0]
3000473c:	f005 fdec 	bl	3000a318 <rtk_log_write>
30004740:	2000      	movs	r0, #0
30004742:	e7e8      	b.n	30004716 <BOOT_LoadRDPImg+0xfe>
30004744:	3000cc00 	.word	0x3000cc00
30004748:	00002429 	.word	0x00002429
3000474c:	00002479 	.word	0x00002479
30004750:	00004641 	.word	0x00004641
30004754:	35393138 	.word	0x35393138
30004758:	31313738 	.word	0x31313738
3000475c:	3000cb5c 	.word	0x3000cb5c
30004760:	3000c7c4 	.word	0x3000c7c4
30004764:	3000cb50 	.word	0x3000cb50
30004768:	00012a1d 	.word	0x00012a1d
3000476c:	000046f5 	.word	0x000046f5

30004770 <_BOOT_Validate_ImgHash>:
30004770:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30004774:	f5ad 7d51 	sub.w	sp, sp, #836	; 0x344
30004778:	4605      	mov	r5, r0
3000477a:	4690      	mov	r8, r2
3000477c:	461c      	mov	r4, r3
3000477e:	9103      	str	r1, [sp, #12]
30004780:	2100      	movs	r1, #0
30004782:	22e0      	movs	r2, #224	; 0xe0
30004784:	4b8e      	ldr	r3, [pc, #568]	; (300049c0 <_BOOT_Validate_ImgHash+0x250>)
30004786:	a818      	add	r0, sp, #96	; 0x60
30004788:	f88d 101f 	strb.w	r1, [sp, #31]
3000478c:	4798      	blx	r3
3000478e:	4628      	mov	r0, r5
30004790:	f10d 021f 	add.w	r2, sp, #31
30004794:	f10d 011e 	add.w	r1, sp, #30
30004798:	f007 fffe 	bl	3000c798 <__SBOOT_GetMdType_veneer>
3000479c:	9001      	str	r0, [sp, #4]
3000479e:	2800      	cmp	r0, #0
300047a0:	d169      	bne.n	30004876 <_BOOT_Validate_ImgHash+0x106>
300047a2:	f89d 301f 	ldrb.w	r3, [sp, #31]
300047a6:	2b00      	cmp	r3, #0
300047a8:	d06a      	beq.n	30004880 <_BOOT_Validate_ImgHash+0x110>
300047aa:	2101      	movs	r1, #1
300047ac:	4605      	mov	r5, r0
300047ae:	4b85      	ldr	r3, [pc, #532]	; (300049c4 <_BOOT_Validate_ImgHash+0x254>)
300047b0:	4608      	mov	r0, r1
300047b2:	4798      	blx	r3
300047b4:	4629      	mov	r1, r5
300047b6:	ab18      	add	r3, sp, #96	; 0x60
300047b8:	2220      	movs	r2, #32
300047ba:	f89d 001e 	ldrb.w	r0, [sp, #30]
300047be:	4d82      	ldr	r5, [pc, #520]	; (300049c8 <_BOOT_Validate_ImgHash+0x258>)
300047c0:	47a8      	blx	r5
300047c2:	2800      	cmp	r0, #0
300047c4:	f040 8091 	bne.w	300048ea <_BOOT_Validate_ImgHash+0x17a>
300047c8:	b344      	cbz	r4, 3000481c <_BOOT_Validate_ImgHash+0xac>
300047ca:	aad0      	add	r2, sp, #832	; 0x340
300047cc:	1e63      	subs	r3, r4, #1
300047ce:	ae50      	add	r6, sp, #320	; 0x140
300047d0:	f8df a20c 	ldr.w	sl, [pc, #524]	; 300049e0 <_BOOT_Validate_ImgHash+0x270>
300047d4:	f012 0218 	ands.w	r2, r2, #24
300047d8:	b2db      	uxtb	r3, r3
300047da:	f026 091f 	bic.w	r9, r6, #31
300047de:	4c7b      	ldr	r4, [pc, #492]	; (300049cc <_BOOT_Validate_ImgHash+0x25c>)
300047e0:	9200      	str	r2, [sp, #0]
300047e2:	f108 0208 	add.w	r2, r8, #8
300047e6:	bf08      	it	eq
300047e8:	46b1      	moveq	r9, r6
300047ea:	eb02 03c3 	add.w	r3, r2, r3, lsl #3
300047ee:	9302      	str	r3, [sp, #8]
300047f0:	f009 031f 	and.w	r3, r9, #31
300047f4:	9304      	str	r3, [sp, #16]
300047f6:	e9d8 b700 	ldrd	fp, r7, [r8]
300047fa:	f10b 4278 	add.w	r2, fp, #4160749568	; 0xf8000000
300047fe:	f1b2 6f00 	cmp.w	r2, #134217728	; 0x8000000
30004802:	f0c0 80ab 	bcc.w	3000495c <_BOOT_Validate_ImgHash+0x1ec>
30004806:	4639      	mov	r1, r7
30004808:	4658      	mov	r0, fp
3000480a:	aa18      	add	r2, sp, #96	; 0x60
3000480c:	47d0      	blx	sl
3000480e:	2800      	cmp	r0, #0
30004810:	d16b      	bne.n	300048ea <_BOOT_Validate_ImgHash+0x17a>
30004812:	f108 0808 	add.w	r8, r8, #8
30004816:	9b02      	ldr	r3, [sp, #8]
30004818:	4543      	cmp	r3, r8
3000481a:	d1ec      	bne.n	300047f6 <_BOOT_Validate_ImgHash+0x86>
3000481c:	ac08      	add	r4, sp, #32
3000481e:	a918      	add	r1, sp, #96	; 0x60
30004820:	4b6b      	ldr	r3, [pc, #428]	; (300049d0 <_BOOT_Validate_ImgHash+0x260>)
30004822:	4620      	mov	r0, r4
30004824:	4798      	blx	r3
30004826:	4601      	mov	r1, r0
30004828:	2800      	cmp	r0, #0
3000482a:	d15e      	bne.n	300048ea <_BOOT_Validate_ImgHash+0x17a>
3000482c:	2001      	movs	r0, #1
3000482e:	4b65      	ldr	r3, [pc, #404]	; (300049c4 <_BOOT_Validate_ImgHash+0x254>)
30004830:	4798      	blx	r3
30004832:	f89d 501e 	ldrb.w	r5, [sp, #30]
30004836:	4b67      	ldr	r3, [pc, #412]	; (300049d4 <_BOOT_Validate_ImgHash+0x264>)
30004838:	4798      	blx	r3
3000483a:	fbb0 f3f5 	udiv	r3, r0, r5
3000483e:	fb03 0315 	mls	r3, r3, r5, r0
30004842:	b2db      	uxtb	r3, r3
30004844:	b1bd      	cbz	r5, 30004876 <_BOOT_Validate_ImgHash+0x106>
30004846:	2000      	movs	r0, #0
30004848:	9f03      	ldr	r7, [sp, #12]
3000484a:	4606      	mov	r6, r0
3000484c:	1c5a      	adds	r2, r3, #1
3000484e:	5cf9      	ldrb	r1, [r7, r3]
30004850:	5ce3      	ldrb	r3, [r4, r3]
30004852:	3001      	adds	r0, #1
30004854:	b2d2      	uxtb	r2, r2
30004856:	404b      	eors	r3, r1
30004858:	431e      	orrs	r6, r3
3000485a:	fbb2 f3f5 	udiv	r3, r2, r5
3000485e:	fb05 2213 	mls	r2, r5, r3, r2
30004862:	b2d3      	uxtb	r3, r2
30004864:	b2c2      	uxtb	r2, r0
30004866:	4295      	cmp	r5, r2
30004868:	d8f0      	bhi.n	3000484c <_BOOT_Validate_ImgHash+0xdc>
3000486a:	2e00      	cmp	r6, #0
3000486c:	9b01      	ldr	r3, [sp, #4]
3000486e:	bf18      	it	ne
30004870:	f06f 0306 	mvnne.w	r3, #6
30004874:	9301      	str	r3, [sp, #4]
30004876:	9801      	ldr	r0, [sp, #4]
30004878:	f50d 7d51 	add.w	sp, sp, #836	; 0x344
3000487c:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30004880:	a918      	add	r1, sp, #96	; 0x60
30004882:	f89d 001e 	ldrb.w	r0, [sp, #30]
30004886:	4b54      	ldr	r3, [pc, #336]	; (300049d8 <_BOOT_Validate_ImgHash+0x268>)
30004888:	4798      	blx	r3
3000488a:	bb70      	cbnz	r0, 300048ea <_BOOT_Validate_ImgHash+0x17a>
3000488c:	b334      	cbz	r4, 300048dc <_BOOT_Validate_ImgHash+0x16c>
3000488e:	aad0      	add	r2, sp, #832	; 0x340
30004890:	1e63      	subs	r3, r4, #1
30004892:	ae50      	add	r6, sp, #320	; 0x140
30004894:	f8df a14c 	ldr.w	sl, [pc, #332]	; 300049e4 <_BOOT_Validate_ImgHash+0x274>
30004898:	f012 0218 	ands.w	r2, r2, #24
3000489c:	b2db      	uxtb	r3, r3
3000489e:	f026 091f 	bic.w	r9, r6, #31
300048a2:	4c4a      	ldr	r4, [pc, #296]	; (300049cc <_BOOT_Validate_ImgHash+0x25c>)
300048a4:	9200      	str	r2, [sp, #0]
300048a6:	f108 0208 	add.w	r2, r8, #8
300048aa:	bf08      	it	eq
300048ac:	46b1      	moveq	r9, r6
300048ae:	eb02 03c3 	add.w	r3, r2, r3, lsl #3
300048b2:	9302      	str	r3, [sp, #8]
300048b4:	f009 031f 	and.w	r3, r9, #31
300048b8:	9304      	str	r3, [sp, #16]
300048ba:	e9d8 b700 	ldrd	fp, r7, [r8]
300048be:	f10b 4278 	add.w	r2, fp, #4160749568	; 0xf8000000
300048c2:	f1b2 6f00 	cmp.w	r2, #134217728	; 0x8000000
300048c6:	d318      	bcc.n	300048fa <_BOOT_Validate_ImgHash+0x18a>
300048c8:	4639      	mov	r1, r7
300048ca:	4658      	mov	r0, fp
300048cc:	aa18      	add	r2, sp, #96	; 0x60
300048ce:	47d0      	blx	sl
300048d0:	b958      	cbnz	r0, 300048ea <_BOOT_Validate_ImgHash+0x17a>
300048d2:	f108 0808 	add.w	r8, r8, #8
300048d6:	9b02      	ldr	r3, [sp, #8]
300048d8:	4543      	cmp	r3, r8
300048da:	d1ee      	bne.n	300048ba <_BOOT_Validate_ImgHash+0x14a>
300048dc:	ac08      	add	r4, sp, #32
300048de:	a918      	add	r1, sp, #96	; 0x60
300048e0:	4b3e      	ldr	r3, [pc, #248]	; (300049dc <_BOOT_Validate_ImgHash+0x26c>)
300048e2:	4620      	mov	r0, r4
300048e4:	4798      	blx	r3
300048e6:	2800      	cmp	r0, #0
300048e8:	d0a3      	beq.n	30004832 <_BOOT_Validate_ImgHash+0xc2>
300048ea:	f06f 030a 	mvn.w	r3, #10
300048ee:	9301      	str	r3, [sp, #4]
300048f0:	9801      	ldr	r0, [sp, #4]
300048f2:	f50d 7d51 	add.w	sp, sp, #836	; 0x344
300048f6:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
300048fa:	2f00      	cmp	r7, #0
300048fc:	d0e9      	beq.n	300048d2 <_BOOT_Validate_ImgHash+0x162>
300048fe:	f8cd 8014 	str.w	r8, [sp, #20]
30004902:	f8dd 8010 	ldr.w	r8, [sp, #16]
30004906:	f5b7 7f00 	cmp.w	r7, #512	; 0x200
3000490a:	463d      	mov	r5, r7
3000490c:	4659      	mov	r1, fp
3000490e:	4630      	mov	r0, r6
30004910:	bf28      	it	cs
30004912:	f44f 7500 	movcs.w	r5, #512	; 0x200
30004916:	462a      	mov	r2, r5
30004918:	f000 fa2a 	bl	30004d70 <BOOT_ImgCopy>
3000491c:	6962      	ldr	r2, [r4, #20]
3000491e:	03d3      	lsls	r3, r2, #15
30004920:	d510      	bpl.n	30004944 <_BOOT_Validate_ImgHash+0x1d4>
30004922:	9b00      	ldr	r3, [sp, #0]
30004924:	18e8      	adds	r0, r5, r3
30004926:	4440      	add	r0, r8
30004928:	f3bf 8f4f 	dsb	sy
3000492c:	4448      	add	r0, r9
3000492e:	464a      	mov	r2, r9
30004930:	f8c4 2270 	str.w	r2, [r4, #624]	; 0x270
30004934:	3220      	adds	r2, #32
30004936:	1a83      	subs	r3, r0, r2
30004938:	2b00      	cmp	r3, #0
3000493a:	dcf9      	bgt.n	30004930 <_BOOT_Validate_ImgHash+0x1c0>
3000493c:	f3bf 8f4f 	dsb	sy
30004940:	f3bf 8f6f 	isb	sy
30004944:	aa18      	add	r2, sp, #96	; 0x60
30004946:	4629      	mov	r1, r5
30004948:	4630      	mov	r0, r6
3000494a:	47d0      	blx	sl
3000494c:	2800      	cmp	r0, #0
3000494e:	d1cc      	bne.n	300048ea <_BOOT_Validate_ImgHash+0x17a>
30004950:	1b7f      	subs	r7, r7, r5
30004952:	44ab      	add	fp, r5
30004954:	d1d7      	bne.n	30004906 <_BOOT_Validate_ImgHash+0x196>
30004956:	f8dd 8014 	ldr.w	r8, [sp, #20]
3000495a:	e7ba      	b.n	300048d2 <_BOOT_Validate_ImgHash+0x162>
3000495c:	2f00      	cmp	r7, #0
3000495e:	f43f af58 	beq.w	30004812 <_BOOT_Validate_ImgHash+0xa2>
30004962:	f8cd 8014 	str.w	r8, [sp, #20]
30004966:	f8dd 8010 	ldr.w	r8, [sp, #16]
3000496a:	f5b7 7f00 	cmp.w	r7, #512	; 0x200
3000496e:	463d      	mov	r5, r7
30004970:	4659      	mov	r1, fp
30004972:	4630      	mov	r0, r6
30004974:	bf28      	it	cs
30004976:	f44f 7500 	movcs.w	r5, #512	; 0x200
3000497a:	462a      	mov	r2, r5
3000497c:	f000 f9f8 	bl	30004d70 <BOOT_ImgCopy>
30004980:	6962      	ldr	r2, [r4, #20]
30004982:	03d2      	lsls	r2, r2, #15
30004984:	d510      	bpl.n	300049a8 <_BOOT_Validate_ImgHash+0x238>
30004986:	9b00      	ldr	r3, [sp, #0]
30004988:	18e8      	adds	r0, r5, r3
3000498a:	4440      	add	r0, r8
3000498c:	f3bf 8f4f 	dsb	sy
30004990:	4448      	add	r0, r9
30004992:	464a      	mov	r2, r9
30004994:	f8c4 2270 	str.w	r2, [r4, #624]	; 0x270
30004998:	3220      	adds	r2, #32
3000499a:	1a83      	subs	r3, r0, r2
3000499c:	2b00      	cmp	r3, #0
3000499e:	dcf9      	bgt.n	30004994 <_BOOT_Validate_ImgHash+0x224>
300049a0:	f3bf 8f4f 	dsb	sy
300049a4:	f3bf 8f6f 	isb	sy
300049a8:	aa18      	add	r2, sp, #96	; 0x60
300049aa:	4629      	mov	r1, r5
300049ac:	4630      	mov	r0, r6
300049ae:	47d0      	blx	sl
300049b0:	2800      	cmp	r0, #0
300049b2:	d19a      	bne.n	300048ea <_BOOT_Validate_ImgHash+0x17a>
300049b4:	1b7f      	subs	r7, r7, r5
300049b6:	44ab      	add	fp, r5
300049b8:	d1d7      	bne.n	3000496a <_BOOT_Validate_ImgHash+0x1fa>
300049ba:	f8dd 8014 	ldr.w	r8, [sp, #20]
300049be:	e728      	b.n	30004812 <_BOOT_Validate_ImgHash+0xa2>
300049c0:	00012be5 	.word	0x00012be5
300049c4:	000024dd 	.word	0x000024dd
300049c8:	3000c095 	.word	0x3000c095
300049cc:	e000ed00 	.word	0xe000ed00
300049d0:	00005bb9 	.word	0x00005bb9
300049d4:	0000ea4d 	.word	0x0000ea4d
300049d8:	00004ffd 	.word	0x00004ffd
300049dc:	000053f1 	.word	0x000053f1
300049e0:	00005b4d 	.word	0x00005b4d
300049e4:	00005385 	.word	0x00005385

300049e8 <BOOT_Validate_ImgHash>:
300049e8:	b530      	push	{r4, r5, lr}
300049ea:	b085      	sub	sp, #20
300049ec:	4d0a      	ldr	r5, [pc, #40]	; (30004a18 <BOOT_Validate_ImgHash+0x30>)
300049ee:	4604      	mov	r4, r0
300049f0:	9301      	str	r3, [sp, #4]
300049f2:	e9cd 2102 	strd	r2, r1, [sp, #8]
300049f6:	47a8      	blx	r5
300049f8:	9903      	ldr	r1, [sp, #12]
300049fa:	e9dd 3201 	ldrd	r3, r2, [sp, #4]
300049fe:	b128      	cbz	r0, 30004a0c <BOOT_Validate_ImgHash+0x24>
30004a00:	4620      	mov	r0, r4
30004a02:	b005      	add	sp, #20
30004a04:	e8bd 4030 	ldmia.w	sp!, {r4, r5, lr}
30004a08:	f007 beba 	b.w	3000c780 <__SBOOT_Validate_ImgHash_veneer>
30004a0c:	4620      	mov	r0, r4
30004a0e:	b005      	add	sp, #20
30004a10:	e8bd 4030 	ldmia.w	sp!, {r4, r5, lr}
30004a14:	f7ff beac 	b.w	30004770 <_BOOT_Validate_ImgHash>
30004a18:	3000b4ed 	.word	0x3000b4ed

30004a1c <BOOT_SignatureCheck>:
30004a1c:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
30004a20:	4616      	mov	r6, r2
30004a22:	b089      	sub	sp, #36	; 0x24
30004a24:	4a76      	ldr	r2, [pc, #472]	; (30004c00 <BOOT_SignatureCheck+0x1e4>)
30004a26:	4698      	mov	r8, r3
30004a28:	4b76      	ldr	r3, [pc, #472]	; (30004c04 <BOOT_SignatureCheck+0x1e8>)
30004a2a:	4607      	mov	r7, r0
30004a2c:	460d      	mov	r5, r1
30004a2e:	7814      	ldrb	r4, [r2, #0]
30004a30:	f10d 0c20 	add.w	ip, sp, #32
30004a34:	f8dd 9040 	ldr.w	r9, [sp, #64]	; 0x40
30004a38:	cb0f      	ldmia	r3, {r0, r1, r2, r3}
30004a3a:	e90c 000f 	stmdb	ip, {r0, r1, r2, r3}
30004a3e:	b91c      	cbnz	r4, 30004a48 <BOOT_SignatureCheck+0x2c>
30004a40:	2001      	movs	r0, #1
30004a42:	b009      	add	sp, #36	; 0x24
30004a44:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
30004a48:	4b6f      	ldr	r3, [pc, #444]	; (30004c08 <BOOT_SignatureCheck+0x1ec>)
30004a4a:	2000      	movs	r0, #0
30004a4c:	4798      	blx	r3
30004a4e:	7cfb      	ldrb	r3, [r7, #19]
30004a50:	7cba      	ldrb	r2, [r7, #18]
30004a52:	f10d 010f 	add.w	r1, sp, #15
30004a56:	f10d 000e 	add.w	r0, sp, #14
30004a5a:	f007 fe95 	bl	3000c788 <__SBOOT_Validate_Algorithm_veneer>
30004a5e:	bb00      	cbnz	r0, 30004aa2 <BOOT_SignatureCheck+0x86>
30004a60:	f8d8 101c 	ldr.w	r1, [r8, #28]
30004a64:	2900      	cmp	r1, #0
30004a66:	f000 80c8 	beq.w	30004bfa <BOOT_SignatureCheck+0x1de>
30004a6a:	4604      	mov	r4, r0
30004a6c:	e002      	b.n	30004a74 <BOOT_SignatureCheck+0x58>
30004a6e:	b2d4      	uxtb	r4, r2
30004a70:	428c      	cmp	r4, r1
30004a72:	d208      	bcs.n	30004a86 <BOOT_SignatureCheck+0x6a>
30004a74:	eb04 03c4 	add.w	r3, r4, r4, lsl #3
30004a78:	1c62      	adds	r2, r4, #1
30004a7a:	eb08 0383 	add.w	r3, r8, r3, lsl #2
30004a7e:	f8d3 30b0 	ldr.w	r3, [r3, #176]	; 0xb0
30004a82:	454b      	cmp	r3, r9
30004a84:	d1f3      	bne.n	30004a6e <BOOT_SignatureCheck+0x52>
30004a86:	2224      	movs	r2, #36	; 0x24
30004a88:	23b4      	movs	r3, #180	; 0xb4
30004a8a:	fb12 3204 	smlabb	r2, r2, r4, r3
30004a8e:	4442      	add	r2, r8
30004a90:	f107 0870 	add.w	r8, r7, #112	; 0x70
30004a94:	f89d 000e 	ldrb.w	r0, [sp, #14]
30004a98:	4641      	mov	r1, r8
30004a9a:	f007 fe85 	bl	3000c7a8 <__SBOOT_Validate_PubKey_veneer>
30004a9e:	2800      	cmp	r0, #0
30004aa0:	d075      	beq.n	30004b8e <BOOT_SignatureCheck+0x172>
30004aa2:	7c7a      	ldrb	r2, [r7, #17]
30004aa4:	a908      	add	r1, sp, #32
30004aa6:	9001      	str	r0, [sp, #4]
30004aa8:	2002      	movs	r0, #2
30004aaa:	eb01 0282 	add.w	r2, r1, r2, lsl #2
30004aae:	4b57      	ldr	r3, [pc, #348]	; (30004c0c <BOOT_SignatureCheck+0x1f0>)
30004ab0:	4957      	ldr	r1, [pc, #348]	; (30004c10 <BOOT_SignatureCheck+0x1f4>)
30004ab2:	f852 2c10 	ldr.w	r2, [r2, #-16]
30004ab6:	9200      	str	r2, [sp, #0]
30004ab8:	2245      	movs	r2, #69	; 0x45
30004aba:	f005 fc2d 	bl	3000a318 <rtk_log_write>
30004abe:	2e00      	cmp	r6, #0
30004ac0:	d041      	beq.n	30004b46 <BOOT_SignatureCheck+0x12a>
30004ac2:	f106 39ff 	add.w	r9, r6, #4294967295	; 0xffffffff
30004ac6:	f04f 0800 	mov.w	r8, #0
30004aca:	4e52      	ldr	r6, [pc, #328]	; (30004c14 <BOOT_SignatureCheck+0x1f8>)
30004acc:	1d2f      	adds	r7, r5, #4
30004ace:	fa5f f389 	uxtb.w	r3, r9
30004ad2:	f105 0908 	add.w	r9, r5, #8
30004ad6:	4c50      	ldr	r4, [pc, #320]	; (30004c18 <BOOT_SignatureCheck+0x1fc>)
30004ad8:	eb09 09c3 	add.w	r9, r9, r3, lsl #3
30004adc:	e003      	b.n	30004ae6 <BOOT_SignatureCheck+0xca>
30004ade:	3508      	adds	r5, #8
30004ae0:	3708      	adds	r7, #8
30004ae2:	45a9      	cmp	r9, r5
30004ae4:	d02f      	beq.n	30004b46 <BOOT_SignatureCheck+0x12a>
30004ae6:	f857 0c04 	ldr.w	r0, [r7, #-4]
30004aea:	f100 4378 	add.w	r3, r0, #4160749568	; 0xf8000000
30004aee:	f1b3 6f00 	cmp.w	r3, #134217728	; 0x8000000
30004af2:	d3f4      	bcc.n	30004ade <BOOT_SignatureCheck+0xc2>
30004af4:	2100      	movs	r1, #0
30004af6:	686a      	ldr	r2, [r5, #4]
30004af8:	47b0      	blx	r6
30004afa:	6963      	ldr	r3, [r4, #20]
30004afc:	03d9      	lsls	r1, r3, #15
30004afe:	d5ee      	bpl.n	30004ade <BOOT_SignatureCheck+0xc2>
30004b00:	f857 3c04 	ldr.w	r3, [r7, #-4]
30004b04:	686a      	ldr	r2, [r5, #4]
30004b06:	ea03 0102 	and.w	r1, r3, r2
30004b0a:	3101      	adds	r1, #1
30004b0c:	d01f      	beq.n	30004b4e <BOOT_SignatureCheck+0x132>
30004b0e:	f013 011f 	ands.w	r1, r3, #31
30004b12:	d002      	beq.n	30004b1a <BOOT_SignatureCheck+0xfe>
30004b14:	440a      	add	r2, r1
30004b16:	f023 031f 	bic.w	r3, r3, #31
30004b1a:	2a00      	cmp	r2, #0
30004b1c:	dddf      	ble.n	30004ade <BOOT_SignatureCheck+0xc2>
30004b1e:	f003 011f 	and.w	r1, r3, #31
30004b22:	4411      	add	r1, r2
30004b24:	f3bf 8f4f 	dsb	sy
30004b28:	4419      	add	r1, r3
30004b2a:	f8c4 3270 	str.w	r3, [r4, #624]	; 0x270
30004b2e:	3320      	adds	r3, #32
30004b30:	1aca      	subs	r2, r1, r3
30004b32:	2a00      	cmp	r2, #0
30004b34:	dcf9      	bgt.n	30004b2a <BOOT_SignatureCheck+0x10e>
30004b36:	f3bf 8f4f 	dsb	sy
30004b3a:	f3bf 8f6f 	isb	sy
30004b3e:	3508      	adds	r5, #8
30004b40:	3708      	adds	r7, #8
30004b42:	45a9      	cmp	r9, r5
30004b44:	d1cf      	bne.n	30004ae6 <BOOT_SignatureCheck+0xca>
30004b46:	2000      	movs	r0, #0
30004b48:	b009      	add	sp, #36	; 0x24
30004b4a:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
30004b4e:	f8c4 8084 	str.w	r8, [r4, #132]	; 0x84
30004b52:	f3bf 8f4f 	dsb	sy
30004b56:	f8d4 3080 	ldr.w	r3, [r4, #128]	; 0x80
30004b5a:	f643 7ee0 	movw	lr, #16352	; 0x3fe0
30004b5e:	f3c3 304e 	ubfx	r0, r3, #13, #15
30004b62:	f3c3 0cc9 	ubfx	ip, r3, #3, #10
30004b66:	0140      	lsls	r0, r0, #5
30004b68:	ea00 010e 	and.w	r1, r0, lr
30004b6c:	4663      	mov	r3, ip
30004b6e:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
30004b72:	3b01      	subs	r3, #1
30004b74:	f8c4 2274 	str.w	r2, [r4, #628]	; 0x274
30004b78:	1c5a      	adds	r2, r3, #1
30004b7a:	d1f8      	bne.n	30004b6e <BOOT_SignatureCheck+0x152>
30004b7c:	3820      	subs	r0, #32
30004b7e:	f110 0f20 	cmn.w	r0, #32
30004b82:	d1f1      	bne.n	30004b68 <BOOT_SignatureCheck+0x14c>
30004b84:	f3bf 8f4f 	dsb	sy
30004b88:	f3bf 8f6f 	isb	sy
30004b8c:	e7d7      	b.n	30004b3e <BOOT_SignatureCheck+0x122>
30004b8e:	f507 7380 	add.w	r3, r7, #256	; 0x100
30004b92:	4642      	mov	r2, r8
30004b94:	f89d 100f 	ldrb.w	r1, [sp, #15]
30004b98:	9301      	str	r3, [sp, #4]
30004b9a:	f44f 7380 	mov.w	r3, #256	; 0x100
30004b9e:	f89d 000e 	ldrb.w	r0, [sp, #14]
30004ba2:	9300      	str	r3, [sp, #0]
30004ba4:	463b      	mov	r3, r7
30004ba6:	f007 fe03 	bl	3000c7b0 <__SBOOT_Validate_Signature_veneer>
30004baa:	2800      	cmp	r0, #0
30004bac:	f47f af79 	bne.w	30004aa2 <BOOT_SignatureCheck+0x86>
30004bb0:	f107 0830 	add.w	r8, r7, #48	; 0x30
30004bb4:	4b19      	ldr	r3, [pc, #100]	; (30004c1c <BOOT_SignatureCheck+0x200>)
30004bb6:	f89d 400f 	ldrb.w	r4, [sp, #15]
30004bba:	4798      	blx	r3
30004bbc:	4641      	mov	r1, r8
30004bbe:	b1b0      	cbz	r0, 30004bee <BOOT_SignatureCheck+0x1d2>
30004bc0:	4620      	mov	r0, r4
30004bc2:	4633      	mov	r3, r6
30004bc4:	462a      	mov	r2, r5
30004bc6:	f007 fddb 	bl	3000c780 <__SBOOT_Validate_ImgHash_veneer>
30004bca:	2800      	cmp	r0, #0
30004bcc:	f47f af69 	bne.w	30004aa2 <BOOT_SignatureCheck+0x86>
30004bd0:	7c7c      	ldrb	r4, [r7, #17]
30004bd2:	a808      	add	r0, sp, #32
30004bd4:	4b12      	ldr	r3, [pc, #72]	; (30004c20 <BOOT_SignatureCheck+0x204>)
30004bd6:	2249      	movs	r2, #73	; 0x49
30004bd8:	eb00 0484 	add.w	r4, r0, r4, lsl #2
30004bdc:	490c      	ldr	r1, [pc, #48]	; (30004c10 <BOOT_SignatureCheck+0x1f4>)
30004bde:	2004      	movs	r0, #4
30004be0:	f854 4c10 	ldr.w	r4, [r4, #-16]
30004be4:	9400      	str	r4, [sp, #0]
30004be6:	f005 fb97 	bl	3000a318 <rtk_log_write>
30004bea:	2001      	movs	r0, #1
30004bec:	e729      	b.n	30004a42 <BOOT_SignatureCheck+0x26>
30004bee:	4620      	mov	r0, r4
30004bf0:	4633      	mov	r3, r6
30004bf2:	462a      	mov	r2, r5
30004bf4:	f7ff fdbc 	bl	30004770 <_BOOT_Validate_ImgHash>
30004bf8:	e7e7      	b.n	30004bca <BOOT_SignatureCheck+0x1ae>
30004bfa:	22b4      	movs	r2, #180	; 0xb4
30004bfc:	e747      	b.n	30004a8e <BOOT_SignatureCheck+0x72>
30004bfe:	bf00      	nop
30004c00:	3000f140 	.word	0x3000f140
30004c04:	3000cc08 	.word	0x3000cc08
30004c08:	00002451 	.word	0x00002451
30004c0c:	3000cb7c 	.word	0x3000cb7c
30004c10:	3000c7c4 	.word	0x3000c7c4
30004c14:	00012be5 	.word	0x00012be5
30004c18:	e000ed00 	.word	0xe000ed00
30004c1c:	3000b4ed 	.word	0x3000b4ed
30004c20:	3000cb6c 	.word	0x3000cb6c

30004c24 <BOOT_CertificateCheck>:
30004c24:	b5f0      	push	{r4, r5, r6, r7, lr}
30004c26:	b08d      	sub	sp, #52	; 0x34
30004c28:	4604      	mov	r4, r0
30004c2a:	460e      	mov	r6, r1
30004c2c:	a804      	add	r0, sp, #16
30004c2e:	f7ff fcab 	bl	30004588 <BOOT_SbootEn_Check>
30004c32:	b910      	cbnz	r0, 30004c3a <BOOT_CertificateCheck+0x16>
30004c34:	2001      	movs	r0, #1
30004c36:	b00d      	add	sp, #52	; 0x34
30004c38:	bdf0      	pop	{r4, r5, r6, r7, pc}
30004c3a:	4b1a      	ldr	r3, [pc, #104]	; (30004ca4 <BOOT_CertificateCheck+0x80>)
30004c3c:	2000      	movs	r0, #0
30004c3e:	4798      	blx	r3
30004c40:	7ce3      	ldrb	r3, [r4, #19]
30004c42:	7ca2      	ldrb	r2, [r4, #18]
30004c44:	f10d 010f 	add.w	r1, sp, #15
30004c48:	f10d 000e 	add.w	r0, sp, #14
30004c4c:	f007 fd9c 	bl	3000c788 <__SBOOT_Validate_Algorithm_veneer>
30004c50:	4605      	mov	r5, r0
30004c52:	b948      	cbnz	r0, 30004c68 <BOOT_CertificateCheck+0x44>
30004c54:	f104 0720 	add.w	r7, r4, #32
30004c58:	aa04      	add	r2, sp, #16
30004c5a:	f89d 000e 	ldrb.w	r0, [sp, #14]
30004c5e:	4639      	mov	r1, r7
30004c60:	f007 fda2 	bl	3000c7a8 <__SBOOT_Validate_PubKey_veneer>
30004c64:	4605      	mov	r5, r0
30004c66:	b148      	cbz	r0, 30004c7c <BOOT_CertificateCheck+0x58>
30004c68:	4b0f      	ldr	r3, [pc, #60]	; (30004ca8 <BOOT_CertificateCheck+0x84>)
30004c6a:	2245      	movs	r2, #69	; 0x45
30004c6c:	490f      	ldr	r1, [pc, #60]	; (30004cac <BOOT_CertificateCheck+0x88>)
30004c6e:	2002      	movs	r0, #2
30004c70:	9500      	str	r5, [sp, #0]
30004c72:	f005 fb51 	bl	3000a318 <rtk_log_write>
30004c76:	2000      	movs	r0, #0
30004c78:	b00d      	add	sp, #52	; 0x34
30004c7a:	bdf0      	pop	{r4, r5, r6, r7, pc}
30004c7c:	490c      	ldr	r1, [pc, #48]	; (30004cb0 <BOOT_CertificateCheck+0x8c>)
30004c7e:	eb06 06c6 	add.w	r6, r6, r6, lsl #3
30004c82:	69a0      	ldr	r0, [r4, #24]
30004c84:	4623      	mov	r3, r4
30004c86:	eb01 1606 	add.w	r6, r1, r6, lsl #4
30004c8a:	463a      	mov	r2, r7
30004c8c:	9000      	str	r0, [sp, #0]
30004c8e:	f89d 100f 	ldrb.w	r1, [sp, #15]
30004c92:	f89d 000e 	ldrb.w	r0, [sp, #14]
30004c96:	9601      	str	r6, [sp, #4]
30004c98:	f007 fd8a 	bl	3000c7b0 <__SBOOT_Validate_Signature_veneer>
30004c9c:	4605      	mov	r5, r0
30004c9e:	2800      	cmp	r0, #0
30004ca0:	d0c8      	beq.n	30004c34 <BOOT_CertificateCheck+0x10>
30004ca2:	e7e1      	b.n	30004c68 <BOOT_CertificateCheck+0x44>
30004ca4:	00002451 	.word	0x00002451
30004ca8:	3000cb98 	.word	0x3000cb98
30004cac:	3000c7c4 	.word	0x3000c7c4
30004cb0:	3000f744 	.word	0x3000f744

30004cb4 <BOOT_Extract_SignatureCheck>:
30004cb4:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
30004cb8:	b08c      	sub	sp, #48	; 0x30
30004cba:	4604      	mov	r4, r0
30004cbc:	460e      	mov	r6, r1
30004cbe:	4617      	mov	r7, r2
30004cc0:	a804      	add	r0, sp, #16
30004cc2:	f7ff fc61 	bl	30004588 <BOOT_SbootEn_Check>
30004cc6:	b918      	cbnz	r0, 30004cd0 <BOOT_Extract_SignatureCheck+0x1c>
30004cc8:	2001      	movs	r0, #1
30004cca:	b00c      	add	sp, #48	; 0x30
30004ccc:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
30004cd0:	4b22      	ldr	r3, [pc, #136]	; (30004d5c <BOOT_Extract_SignatureCheck+0xa8>)
30004cd2:	2000      	movs	r0, #0
30004cd4:	4798      	blx	r3
30004cd6:	7ce3      	ldrb	r3, [r4, #19]
30004cd8:	7ca2      	ldrb	r2, [r4, #18]
30004cda:	f10d 010f 	add.w	r1, sp, #15
30004cde:	f10d 000e 	add.w	r0, sp, #14
30004ce2:	f007 fd51 	bl	3000c788 <__SBOOT_Validate_Algorithm_veneer>
30004ce6:	4605      	mov	r5, r0
30004ce8:	b948      	cbnz	r0, 30004cfe <BOOT_Extract_SignatureCheck+0x4a>
30004cea:	f104 0870 	add.w	r8, r4, #112	; 0x70
30004cee:	aa04      	add	r2, sp, #16
30004cf0:	f89d 000e 	ldrb.w	r0, [sp, #14]
30004cf4:	4641      	mov	r1, r8
30004cf6:	f007 fd57 	bl	3000c7a8 <__SBOOT_Validate_PubKey_veneer>
30004cfa:	4605      	mov	r5, r0
30004cfc:	b150      	cbz	r0, 30004d14 <BOOT_Extract_SignatureCheck+0x60>
30004cfe:	4b18      	ldr	r3, [pc, #96]	; (30004d60 <BOOT_Extract_SignatureCheck+0xac>)
30004d00:	2245      	movs	r2, #69	; 0x45
30004d02:	4918      	ldr	r1, [pc, #96]	; (30004d64 <BOOT_Extract_SignatureCheck+0xb0>)
30004d04:	2002      	movs	r0, #2
30004d06:	9500      	str	r5, [sp, #0]
30004d08:	f005 fb06 	bl	3000a318 <rtk_log_write>
30004d0c:	2000      	movs	r0, #0
30004d0e:	b00c      	add	sp, #48	; 0x30
30004d10:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
30004d14:	f504 7380 	add.w	r3, r4, #256	; 0x100
30004d18:	4642      	mov	r2, r8
30004d1a:	f89d 100f 	ldrb.w	r1, [sp, #15]
30004d1e:	9301      	str	r3, [sp, #4]
30004d20:	f44f 7380 	mov.w	r3, #256	; 0x100
30004d24:	f89d 000e 	ldrb.w	r0, [sp, #14]
30004d28:	9300      	str	r3, [sp, #0]
30004d2a:	4623      	mov	r3, r4
30004d2c:	f007 fd40 	bl	3000c7b0 <__SBOOT_Validate_Signature_veneer>
30004d30:	4605      	mov	r5, r0
30004d32:	2800      	cmp	r0, #0
30004d34:	d1e3      	bne.n	30004cfe <BOOT_Extract_SignatureCheck+0x4a>
30004d36:	463b      	mov	r3, r7
30004d38:	4632      	mov	r2, r6
30004d3a:	f104 0130 	add.w	r1, r4, #48	; 0x30
30004d3e:	f89d 000f 	ldrb.w	r0, [sp, #15]
30004d42:	f007 fd1d 	bl	3000c780 <__SBOOT_Validate_ImgHash_veneer>
30004d46:	4605      	mov	r5, r0
30004d48:	2800      	cmp	r0, #0
30004d4a:	d1d8      	bne.n	30004cfe <BOOT_Extract_SignatureCheck+0x4a>
30004d4c:	2004      	movs	r0, #4
30004d4e:	4b06      	ldr	r3, [pc, #24]	; (30004d68 <BOOT_Extract_SignatureCheck+0xb4>)
30004d50:	2249      	movs	r2, #73	; 0x49
30004d52:	4904      	ldr	r1, [pc, #16]	; (30004d64 <BOOT_Extract_SignatureCheck+0xb0>)
30004d54:	f005 fae0 	bl	3000a318 <rtk_log_write>
30004d58:	2001      	movs	r0, #1
30004d5a:	e7b6      	b.n	30004cca <BOOT_Extract_SignatureCheck+0x16>
30004d5c:	00002451 	.word	0x00002451
30004d60:	3000cbd8 	.word	0x3000cbd8
30004d64:	3000c7c4 	.word	0x3000c7c4
30004d68:	3000cbbc 	.word	0x3000cbbc
30004d6c:	00000000 	.word	0x00000000

30004d70 <BOOT_ImgCopy>:
30004d70:	b510      	push	{r4, lr}
30004d72:	b082      	sub	sp, #8
30004d74:	4b09      	ldr	r3, [pc, #36]	; (30004d9c <BOOT_ImgCopy+0x2c>)
30004d76:	4604      	mov	r4, r0
30004d78:	e9cd 2100 	strd	r2, r1, [sp]
30004d7c:	4798      	blx	r3
30004d7e:	e9dd 2100 	ldrd	r2, r1, [sp]
30004d82:	b128      	cbz	r0, 30004d90 <BOOT_ImgCopy+0x20>
30004d84:	4620      	mov	r0, r4
30004d86:	4b06      	ldr	r3, [pc, #24]	; (30004da0 <BOOT_ImgCopy+0x30>)
30004d88:	b002      	add	sp, #8
30004d8a:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
30004d8e:	4718      	bx	r3
30004d90:	4620      	mov	r0, r4
30004d92:	b002      	add	sp, #8
30004d94:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
30004d98:	f005 b9fc 	b.w	3000a194 <NandImgCopy>
30004d9c:	3000b4ed 	.word	0x3000b4ed
30004da0:	00012a1d 	.word	0x00012a1d

30004da4 <BOOT_OTA_SlotSelect>:
30004da4:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30004da8:	4f4b      	ldr	r7, [pc, #300]	; (30004ed8 <BOOT_OTA_SlotSelect+0x134>)
30004daa:	4d4c      	ldr	r5, [pc, #304]	; (30004edc <BOOT_OTA_SlotSelect+0x138>)
30004dac:	4c4c      	ldr	r4, [pc, #304]	; (30004ee0 <BOOT_OTA_SlotSelect+0x13c>)
30004dae:	463e      	mov	r6, r7
30004db0:	f8df 9148 	ldr.w	r9, [pc, #328]	; 30004efc <BOOT_OTA_SlotSelect+0x158>
30004db4:	f8df 8148 	ldr.w	r8, [pc, #328]	; 30004f00 <BOOT_OTA_SlotSelect+0x15c>
30004db8:	f8df a148 	ldr.w	sl, [pc, #328]	; 30004f04 <BOOT_OTA_SlotSelect+0x160>
30004dbc:	f8df b148 	ldr.w	fp, [pc, #328]	; 30004f08 <BOOT_OTA_SlotSelect+0x164>
30004dc0:	ed2d 8b02 	vpush	{d8}
30004dc4:	ed9f 8b42 	vldr	d8, [pc, #264]	; 30004ed0 <BOOT_OTA_SlotSelect+0x12c>
30004dc8:	b083      	sub	sp, #12
30004dca:	f855 1b04 	ldr.w	r1, [r5], #4
30004dce:	9101      	str	r1, [sp, #4]
30004dd0:	47d0      	blx	sl
30004dd2:	9901      	ldr	r1, [sp, #4]
30004dd4:	f44f 72b2 	mov.w	r2, #356	; 0x164
30004dd8:	2800      	cmp	r0, #0
30004dda:	d041      	beq.n	30004e60 <BOOT_OTA_SlotSelect+0xbc>
30004ddc:	4620      	mov	r0, r4
30004dde:	4b41      	ldr	r3, [pc, #260]	; (30004ee4 <BOOT_OTA_SlotSelect+0x140>)
30004de0:	4798      	blx	r3
30004de2:	2208      	movs	r2, #8
30004de4:	4659      	mov	r1, fp
30004de6:	4620      	mov	r0, r4
30004de8:	4b3f      	ldr	r3, [pc, #252]	; (30004ee8 <BOOT_OTA_SlotSelect+0x144>)
30004dea:	4798      	blx	r3
30004dec:	2800      	cmp	r0, #0
30004dee:	d03b      	beq.n	30004e68 <BOOT_OTA_SlotSelect+0xc4>
30004df0:	ed86 8b00 	vstr	d8, [r6]
30004df4:	4b3d      	ldr	r3, [pc, #244]	; (30004eec <BOOT_OTA_SlotSelect+0x148>)
30004df6:	f504 74b2 	add.w	r4, r4, #356	; 0x164
30004dfa:	3608      	adds	r6, #8
30004dfc:	f509 79c8 	add.w	r9, r9, #400	; 0x190
30004e00:	42ab      	cmp	r3, r5
30004e02:	f108 0890 	add.w	r8, r8, #144	; 0x90
30004e06:	d1e0      	bne.n	30004dca <BOOT_OTA_SlotSelect+0x26>
30004e08:	4b39      	ldr	r3, [pc, #228]	; (30004ef0 <BOOT_OTA_SlotSelect+0x14c>)
30004e0a:	4798      	blx	r3
30004e0c:	b178      	cbz	r0, 30004e2e <BOOT_OTA_SlotSelect+0x8a>
30004e0e:	4b39      	ldr	r3, [pc, #228]	; (30004ef4 <BOOT_OTA_SlotSelect+0x150>)
30004e10:	2100      	movs	r1, #0
30004e12:	2401      	movs	r4, #1
30004e14:	f8b3 006e 	ldrh.w	r0, [r3, #110]	; 0x6e
30004e18:	460b      	mov	r3, r1
30004e1a:	b280      	uxth	r0, r0
30004e1c:	fa04 f203 	lsl.w	r2, r4, r3
30004e20:	3301      	adds	r3, #1
30004e22:	4202      	tst	r2, r0
30004e24:	bf08      	it	eq
30004e26:	3101      	addeq	r1, #1
30004e28:	2b10      	cmp	r3, #16
30004e2a:	d1f7      	bne.n	30004e1c <BOOT_OTA_SlotSelect+0x78>
30004e2c:	0408      	lsls	r0, r1, #16
30004e2e:	e9d7 2300 	ldrd	r2, r3, [r7]
30004e32:	e9d7 4102 	ldrd	r4, r1, [r7, #8]
30004e36:	4282      	cmp	r2, r0
30004e38:	f173 0500 	sbcs.w	r5, r3, #0
30004e3c:	db32      	blt.n	30004ea4 <BOOT_OTA_SlotSelect+0x100>
30004e3e:	4284      	cmp	r4, r0
30004e40:	f171 0000 	sbcs.w	r0, r1, #0
30004e44:	da3e      	bge.n	30004ec4 <BOOT_OTA_SlotSelect+0x120>
30004e46:	482c      	ldr	r0, [pc, #176]	; (30004ef8 <BOOT_OTA_SlotSelect+0x154>)
30004e48:	2501      	movs	r5, #1
30004e4a:	7005      	strb	r5, [r0, #0]
30004e4c:	42a2      	cmp	r2, r4
30004e4e:	418b      	sbcs	r3, r1
30004e50:	bfb4      	ite	lt
30004e52:	2001      	movlt	r0, #1
30004e54:	2000      	movge	r0, #0
30004e56:	b003      	add	sp, #12
30004e58:	ecbd 8b02 	vpop	{d8}
30004e5c:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30004e60:	4620      	mov	r0, r4
30004e62:	f005 f997 	bl	3000a194 <NandImgCopy>
30004e66:	e7bc      	b.n	30004de2 <BOOT_OTA_SlotSelect+0x3e>
30004e68:	f855 1c04 	ldr.w	r1, [r5, #-4]
30004e6c:	69a3      	ldr	r3, [r4, #24]
30004e6e:	4419      	add	r1, r3
30004e70:	9101      	str	r1, [sp, #4]
30004e72:	47d0      	blx	sl
30004e74:	9901      	ldr	r1, [sp, #4]
30004e76:	2290      	movs	r2, #144	; 0x90
30004e78:	b9e0      	cbnz	r0, 30004eb4 <BOOT_OTA_SlotSelect+0x110>
30004e7a:	4640      	mov	r0, r8
30004e7c:	f005 f98a 	bl	3000a194 <NandImgCopy>
30004e80:	6869      	ldr	r1, [r5, #4]
30004e82:	9101      	str	r1, [sp, #4]
30004e84:	47d0      	blx	sl
30004e86:	9901      	ldr	r1, [sp, #4]
30004e88:	f44f 72c8 	mov.w	r2, #400	; 0x190
30004e8c:	b1b0      	cbz	r0, 30004ebc <BOOT_OTA_SlotSelect+0x118>
30004e8e:	4648      	mov	r0, r9
30004e90:	4b14      	ldr	r3, [pc, #80]	; (30004ee4 <BOOT_OTA_SlotSelect+0x140>)
30004e92:	4798      	blx	r3
30004e94:	8aa2      	ldrh	r2, [r4, #20]
30004e96:	8ae3      	ldrh	r3, [r4, #22]
30004e98:	ea43 4302 	orr.w	r3, r3, r2, lsl #16
30004e9c:	6033      	str	r3, [r6, #0]
30004e9e:	2300      	movs	r3, #0
30004ea0:	6073      	str	r3, [r6, #4]
30004ea2:	e7a7      	b.n	30004df4 <BOOT_OTA_SlotSelect+0x50>
30004ea4:	4284      	cmp	r4, r0
30004ea6:	f171 0000 	sbcs.w	r0, r1, #0
30004eaa:	dacc      	bge.n	30004e46 <BOOT_OTA_SlotSelect+0xa2>
30004eac:	4812      	ldr	r0, [pc, #72]	; (30004ef8 <BOOT_OTA_SlotSelect+0x154>)
30004eae:	2500      	movs	r5, #0
30004eb0:	7005      	strb	r5, [r0, #0]
30004eb2:	e7cb      	b.n	30004e4c <BOOT_OTA_SlotSelect+0xa8>
30004eb4:	4640      	mov	r0, r8
30004eb6:	4b0b      	ldr	r3, [pc, #44]	; (30004ee4 <BOOT_OTA_SlotSelect+0x140>)
30004eb8:	4798      	blx	r3
30004eba:	e7e1      	b.n	30004e80 <BOOT_OTA_SlotSelect+0xdc>
30004ebc:	4648      	mov	r0, r9
30004ebe:	f005 f969 	bl	3000a194 <NandImgCopy>
30004ec2:	e7e7      	b.n	30004e94 <BOOT_OTA_SlotSelect+0xf0>
30004ec4:	480c      	ldr	r0, [pc, #48]	; (30004ef8 <BOOT_OTA_SlotSelect+0x154>)
30004ec6:	2502      	movs	r5, #2
30004ec8:	7005      	strb	r5, [r0, #0]
30004eca:	e7bf      	b.n	30004e4c <BOOT_OTA_SlotSelect+0xa8>
30004ecc:	f3af 8000 	nop.w
30004ed0:	ffffffff 	.word	0xffffffff
30004ed4:	ffffffff 	.word	0xffffffff
30004ed8:	3000f8d8 	.word	0x3000f8d8
30004edc:	3000f72c 	.word	0x3000f72c
30004ee0:	3000f144 	.word	0x3000f144
30004ee4:	00012a1d 	.word	0x00012a1d
30004ee8:	000129bd 	.word	0x000129bd
30004eec:	3000f734 	.word	0x3000f734
30004ef0:	0000c21d 	.word	0x0000c21d
30004ef4:	42000300 	.word	0x42000300
30004ef8:	3000f8d4 	.word	0x3000f8d4
30004efc:	3000f40c 	.word	0x3000f40c
30004f00:	3000f744 	.word	0x3000f744
30004f04:	3000b4ed 	.word	0x3000b4ed
30004f08:	3000ce70 	.word	0x3000ce70

30004f0c <BOOT_LoadSubImage>:
30004f0c:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30004f10:	b08f      	sub	sp, #60	; 0x3c
30004f12:	469a      	mov	sl, r3
30004f14:	f89d 3060 	ldrb.w	r3, [sp, #96]	; 0x60
30004f18:	9305      	str	r3, [sp, #20]
30004f1a:	2a00      	cmp	r2, #0
30004f1c:	d038      	beq.n	30004f90 <BOOT_LoadSubImage+0x84>
30004f1e:	460f      	mov	r7, r1
30004f20:	4691      	mov	r9, r2
30004f22:	2600      	movs	r6, #0
30004f24:	4605      	mov	r5, r0
30004f26:	4b52      	ldr	r3, [pc, #328]	; (30005070 <BOOT_LoadSubImage+0x164>)
30004f28:	4798      	blx	r3
30004f2a:	2220      	movs	r2, #32
30004f2c:	4639      	mov	r1, r7
30004f2e:	2800      	cmp	r0, #0
30004f30:	d032      	beq.n	30004f98 <BOOT_LoadSubImage+0x8c>
30004f32:	4b50      	ldr	r3, [pc, #320]	; (30005074 <BOOT_LoadSubImage+0x168>)
30004f34:	a806      	add	r0, sp, #24
30004f36:	4798      	blx	r3
30004f38:	9b06      	ldr	r3, [sp, #24]
30004f3a:	4a4f      	ldr	r2, [pc, #316]	; (30005078 <BOOT_LoadSubImage+0x16c>)
30004f3c:	4293      	cmp	r3, r2
30004f3e:	d132      	bne.n	30004fa6 <BOOT_LoadSubImage+0x9a>
30004f40:	9b07      	ldr	r3, [sp, #28]
30004f42:	4a4e      	ldr	r2, [pc, #312]	; (3000507c <BOOT_LoadSubImage+0x170>)
30004f44:	4293      	cmp	r3, r2
30004f46:	d12e      	bne.n	30004fa6 <BOOT_LoadSubImage+0x9a>
30004f48:	9a09      	ldr	r2, [sp, #36]	; 0x24
30004f4a:	4b4d      	ldr	r3, [pc, #308]	; (30005080 <BOOT_LoadSubImage+0x174>)
30004f4c:	9c08      	ldr	r4, [sp, #32]
30004f4e:	f1a2 0820 	sub.w	r8, r2, #32
30004f52:	4413      	add	r3, r2
30004f54:	3420      	adds	r4, #32
30004f56:	f1b3 6f00 	cmp.w	r3, #134217728	; 0x8000000
30004f5a:	d301      	bcc.n	30004f60 <BOOT_LoadSubImage+0x54>
30004f5c:	2c20      	cmp	r4, #32
30004f5e:	d829      	bhi.n	30004fb4 <BOOT_LoadSubImage+0xa8>
30004f60:	2c20      	cmp	r4, #32
30004f62:	bf08      	it	eq
30004f64:	46b8      	moveq	r8, r7
30004f66:	b17d      	cbz	r5, 30004f88 <BOOT_LoadSubImage+0x7c>
30004f68:	1d2b      	adds	r3, r5, #4
30004f6a:	f845 8036 	str.w	r8, [r5, r6, lsl #3]
30004f6e:	2249      	movs	r2, #73	; 0x49
30004f70:	4944      	ldr	r1, [pc, #272]	; (30005084 <BOOT_LoadSubImage+0x178>)
30004f72:	2004      	movs	r0, #4
30004f74:	e9cd 8401 	strd	r8, r4, [sp, #4]
30004f78:	f843 4036 	str.w	r4, [r3, r6, lsl #3]
30004f7c:	f85a 3026 	ldr.w	r3, [sl, r6, lsl #2]
30004f80:	9300      	str	r3, [sp, #0]
30004f82:	4b41      	ldr	r3, [pc, #260]	; (30005088 <BOOT_LoadSubImage+0x17c>)
30004f84:	f005 f9c8 	bl	3000a318 <rtk_log_write>
30004f88:	3601      	adds	r6, #1
30004f8a:	4427      	add	r7, r4
30004f8c:	454e      	cmp	r6, r9
30004f8e:	d1ca      	bne.n	30004f26 <BOOT_LoadSubImage+0x1a>
30004f90:	2001      	movs	r0, #1
30004f92:	b00f      	add	sp, #60	; 0x3c
30004f94:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30004f98:	a806      	add	r0, sp, #24
30004f9a:	f005 f8fb 	bl	3000a194 <NandImgCopy>
30004f9e:	9b06      	ldr	r3, [sp, #24]
30004fa0:	4a35      	ldr	r2, [pc, #212]	; (30005078 <BOOT_LoadSubImage+0x16c>)
30004fa2:	4293      	cmp	r3, r2
30004fa4:	d0cc      	beq.n	30004f40 <BOOT_LoadSubImage+0x34>
30004fa6:	9b05      	ldr	r3, [sp, #20]
30004fa8:	2b01      	cmp	r3, #1
30004faa:	d056      	beq.n	3000505a <BOOT_LoadSubImage+0x14e>
30004fac:	2000      	movs	r0, #0
30004fae:	b00f      	add	sp, #60	; 0x3c
30004fb0:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30004fb4:	4b2e      	ldr	r3, [pc, #184]	; (30005070 <BOOT_LoadSubImage+0x164>)
30004fb6:	4798      	blx	r3
30004fb8:	4622      	mov	r2, r4
30004fba:	4639      	mov	r1, r7
30004fbc:	b318      	cbz	r0, 30005006 <BOOT_LoadSubImage+0xfa>
30004fbe:	4640      	mov	r0, r8
30004fc0:	4b2c      	ldr	r3, [pc, #176]	; (30005074 <BOOT_LoadSubImage+0x168>)
30004fc2:	4798      	blx	r3
30004fc4:	4931      	ldr	r1, [pc, #196]	; (3000508c <BOOT_LoadSubImage+0x180>)
30004fc6:	694b      	ldr	r3, [r1, #20]
30004fc8:	03db      	lsls	r3, r3, #15
30004fca:	d5cc      	bpl.n	30004f66 <BOOT_LoadSubImage+0x5a>
30004fcc:	ea08 0304 	and.w	r3, r8, r4
30004fd0:	3301      	adds	r3, #1
30004fd2:	d020      	beq.n	30005016 <BOOT_LoadSubImage+0x10a>
30004fd4:	f018 021f 	ands.w	r2, r8, #31
30004fd8:	d119      	bne.n	3000500e <BOOT_LoadSubImage+0x102>
30004fda:	4622      	mov	r2, r4
30004fdc:	4643      	mov	r3, r8
30004fde:	2a00      	cmp	r2, #0
30004fe0:	ddc1      	ble.n	30004f66 <BOOT_LoadSubImage+0x5a>
30004fe2:	f003 011f 	and.w	r1, r3, #31
30004fe6:	4411      	add	r1, r2
30004fe8:	f3bf 8f4f 	dsb	sy
30004fec:	4827      	ldr	r0, [pc, #156]	; (3000508c <BOOT_LoadSubImage+0x180>)
30004fee:	4419      	add	r1, r3
30004ff0:	f8c0 3270 	str.w	r3, [r0, #624]	; 0x270
30004ff4:	3320      	adds	r3, #32
30004ff6:	1aca      	subs	r2, r1, r3
30004ff8:	2a00      	cmp	r2, #0
30004ffa:	dcf9      	bgt.n	30004ff0 <BOOT_LoadSubImage+0xe4>
30004ffc:	f3bf 8f4f 	dsb	sy
30005000:	f3bf 8f6f 	isb	sy
30005004:	e7af      	b.n	30004f66 <BOOT_LoadSubImage+0x5a>
30005006:	4640      	mov	r0, r8
30005008:	f005 f8c4 	bl	3000a194 <NandImgCopy>
3000500c:	e7da      	b.n	30004fc4 <BOOT_LoadSubImage+0xb8>
3000500e:	4422      	add	r2, r4
30005010:	f028 031f 	bic.w	r3, r8, #31
30005014:	e7e3      	b.n	30004fde <BOOT_LoadSubImage+0xd2>
30005016:	2300      	movs	r3, #0
30005018:	f8c1 3084 	str.w	r3, [r1, #132]	; 0x84
3000501c:	f3bf 8f4f 	dsb	sy
30005020:	f8d1 3080 	ldr.w	r3, [r1, #128]	; 0x80
30005024:	f643 7be0 	movw	fp, #16352	; 0x3fe0
30005028:	f3c3 3c4e 	ubfx	ip, r3, #13, #15
3000502c:	f3c3 0ec9 	ubfx	lr, r3, #3, #10
30005030:	ea4f 1c4c 	mov.w	ip, ip, lsl #5
30005034:	ea0c 000b 	and.w	r0, ip, fp
30005038:	4673      	mov	r3, lr
3000503a:	ea40 7283 	orr.w	r2, r0, r3, lsl #30
3000503e:	3b01      	subs	r3, #1
30005040:	f8c1 2274 	str.w	r2, [r1, #628]	; 0x274
30005044:	d2f9      	bcs.n	3000503a <BOOT_LoadSubImage+0x12e>
30005046:	f1ac 0c20 	sub.w	ip, ip, #32
3000504a:	f11c 0f20 	cmn.w	ip, #32
3000504e:	d1f1      	bne.n	30005034 <BOOT_LoadSubImage+0x128>
30005050:	f3bf 8f4f 	dsb	sy
30005054:	f3bf 8f6f 	isb	sy
30005058:	e785      	b.n	30004f66 <BOOT_LoadSubImage+0x5a>
3000505a:	f85a 4026 	ldr.w	r4, [sl, r6, lsl #2]
3000505e:	2249      	movs	r2, #73	; 0x49
30005060:	4b0b      	ldr	r3, [pc, #44]	; (30005090 <BOOT_LoadSubImage+0x184>)
30005062:	2004      	movs	r0, #4
30005064:	4907      	ldr	r1, [pc, #28]	; (30005084 <BOOT_LoadSubImage+0x178>)
30005066:	9400      	str	r4, [sp, #0]
30005068:	f005 f956 	bl	3000a318 <rtk_log_write>
3000506c:	e79e      	b.n	30004fac <BOOT_LoadSubImage+0xa0>
3000506e:	bf00      	nop
30005070:	3000b4ed 	.word	0x3000b4ed
30005074:	00012a1d 	.word	0x00012a1d
30005078:	35393138 	.word	0x35393138
3000507c:	31313738 	.word	0x31313738
30005080:	f7ffffe0 	.word	0xf7ffffe0
30005084:	3000c7c4 	.word	0x3000c7c4
30005088:	3000cb5c 	.word	0x3000cb5c
3000508c:	e000ed00 	.word	0xe000ed00
30005090:	3000cb50 	.word	0x3000cb50

30005094 <BOOT_RSIPIvSet>:
30005094:	b530      	push	{r4, r5, lr}
30005096:	2300      	movs	r3, #0
30005098:	b083      	sub	sp, #12
3000509a:	4a09      	ldr	r2, [pc, #36]	; (300050c0 <BOOT_RSIPIvSet+0x2c>)
3000509c:	4605      	mov	r5, r0
3000509e:	460c      	mov	r4, r1
300050a0:	e9cd 3300 	strd	r3, r3, [sp]
300050a4:	4790      	blx	r2
300050a6:	b148      	cbz	r0, 300050bc <BOOT_RSIPIvSet+0x28>
300050a8:	f105 0120 	add.w	r1, r5, #32
300050ac:	4b05      	ldr	r3, [pc, #20]	; (300050c4 <BOOT_RSIPIvSet+0x30>)
300050ae:	2208      	movs	r2, #8
300050b0:	4668      	mov	r0, sp
300050b2:	4798      	blx	r3
300050b4:	4669      	mov	r1, sp
300050b6:	b2e0      	uxtb	r0, r4
300050b8:	4b03      	ldr	r3, [pc, #12]	; (300050c8 <BOOT_RSIPIvSet+0x34>)
300050ba:	4798      	blx	r3
300050bc:	b003      	add	sp, #12
300050be:	bd30      	pop	{r4, r5, pc}
300050c0:	0000c129 	.word	0x0000c129
300050c4:	00012a1d 	.word	0x00012a1d
300050c8:	3000b2dd 	.word	0x3000b2dd

300050cc <BOOT_OTFCheck>:
300050cc:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
300050d0:	4d1c      	ldr	r5, [pc, #112]	; (30005144 <BOOT_OTFCheck+0x78>)
300050d2:	b082      	sub	sp, #8
300050d4:	4604      	mov	r4, r0
300050d6:	4688      	mov	r8, r1
300050d8:	4617      	mov	r7, r2
300050da:	461e      	mov	r6, r3
300050dc:	47a8      	blx	r5
300050de:	b148      	cbz	r0, 300050f4 <BOOT_OTFCheck+0x28>
300050e0:	f104 4378 	add.w	r3, r4, #4160749568	; 0xf8000000
300050e4:	f1b3 6f00 	cmp.w	r3, #134217728	; 0x8000000
300050e8:	d204      	bcs.n	300050f4 <BOOT_OTFCheck+0x28>
300050ea:	f108 4378 	add.w	r3, r8, #4160749568	; 0xf8000000
300050ee:	f1b3 6f00 	cmp.w	r3, #134217728	; 0x8000000
300050f2:	d302      	bcc.n	300050fa <BOOT_OTFCheck+0x2e>
300050f4:	b002      	add	sp, #8
300050f6:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
300050fa:	4b13      	ldr	r3, [pc, #76]	; (30005148 <BOOT_OTFCheck+0x7c>)
300050fc:	2249      	movs	r2, #73	; 0x49
300050fe:	4913      	ldr	r1, [pc, #76]	; (3000514c <BOOT_OTFCheck+0x80>)
30005100:	2004      	movs	r0, #4
30005102:	f005 f909 	bl	3000a318 <rtk_log_write>
30005106:	4b12      	ldr	r3, [pc, #72]	; (30005150 <BOOT_OTFCheck+0x84>)
30005108:	4798      	blx	r3
3000510a:	4603      	mov	r3, r0
3000510c:	b150      	cbz	r0, 30005124 <BOOT_OTFCheck+0x58>
3000510e:	2801      	cmp	r0, #1
30005110:	d009      	beq.n	30005126 <BOOT_OTFCheck+0x5a>
30005112:	4b10      	ldr	r3, [pc, #64]	; (30005154 <BOOT_OTFCheck+0x88>)
30005114:	2245      	movs	r2, #69	; 0x45
30005116:	490d      	ldr	r1, [pc, #52]	; (3000514c <BOOT_OTFCheck+0x80>)
30005118:	2002      	movs	r0, #2
3000511a:	b002      	add	sp, #8
3000511c:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
30005120:	f005 b8fa 	b.w	3000a318 <rtk_log_write>
30005124:	2302      	movs	r3, #2
30005126:	4630      	mov	r0, r6
30005128:	4621      	mov	r1, r4
3000512a:	9301      	str	r3, [sp, #4]
3000512c:	4642      	mov	r2, r8
3000512e:	2301      	movs	r3, #1
30005130:	4c09      	ldr	r4, [pc, #36]	; (30005158 <BOOT_OTFCheck+0x8c>)
30005132:	9700      	str	r7, [sp, #0]
30005134:	47a0      	blx	r4
30005136:	2001      	movs	r0, #1
30005138:	4b08      	ldr	r3, [pc, #32]	; (3000515c <BOOT_OTFCheck+0x90>)
3000513a:	b002      	add	sp, #8
3000513c:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
30005140:	4718      	bx	r3
30005142:	bf00      	nop
30005144:	0000c129 	.word	0x0000c129
30005148:	3000cc8c 	.word	0x3000cc8c
3000514c:	3000c7c4 	.word	0x3000c7c4
30005150:	0000c245 	.word	0x0000c245
30005154:	3000cc9c 	.word	0x3000cc9c
30005158:	3000c36d 	.word	0x3000c36d
3000515c:	3000b2f9 	.word	0x3000b2f9

30005160 <BOOT_OTA_Region_Init>:
30005160:	b510      	push	{r4, lr}
30005162:	4c0a      	ldr	r4, [pc, #40]	; (3000518c <BOOT_OTA_Region_Init+0x2c>)
30005164:	2200      	movs	r2, #0
30005166:	2002      	movs	r0, #2
30005168:	4621      	mov	r1, r4
3000516a:	f005 f98f 	bl	3000a48c <flash_get_layout_info>
3000516e:	2200      	movs	r2, #0
30005170:	1d21      	adds	r1, r4, #4
30005172:	2003      	movs	r0, #3
30005174:	f005 f98a 	bl	3000a48c <flash_get_layout_info>
30005178:	e9d4 2300 	ldrd	r2, r3, [r4]
3000517c:	f502 5280 	add.w	r2, r2, #4096	; 0x1000
30005180:	f503 5380 	add.w	r3, r3, #4096	; 0x1000
30005184:	e9c4 2302 	strd	r2, r3, [r4, #8]
30005188:	bd10      	pop	{r4, pc}
3000518a:	bf00      	nop
3000518c:	3000f72c 	.word	0x3000f72c

30005190 <BOOT_OTA_RDP>:
30005190:	b510      	push	{r4, lr}
30005192:	4b16      	ldr	r3, [pc, #88]	; (300051ec <BOOT_OTA_RDP+0x5c>)
30005194:	4696      	mov	lr, r2
30005196:	3202      	adds	r2, #2
30005198:	f853 2022 	ldr.w	r2, [r3, r2, lsl #2]
3000519c:	f502 5280 	add.w	r2, r2, #4096	; 0x1000
300051a0:	b169      	cbz	r1, 300051be <BOOT_OTA_RDP+0x2e>
300051a2:	f101 3cff 	add.w	ip, r1, #4294967295	; 0xffffffff
300051a6:	4603      	mov	r3, r0
300051a8:	fa5f f48c 	uxtb.w	r4, ip
300051ac:	f100 0c08 	add.w	ip, r0, #8
300051b0:	eb0c 0cc4 	add.w	ip, ip, r4, lsl #3
300051b4:	685c      	ldr	r4, [r3, #4]
300051b6:	3308      	adds	r3, #8
300051b8:	4563      	cmp	r3, ip
300051ba:	4422      	add	r2, r4
300051bc:	d1fa      	bne.n	300051b4 <BOOT_OTA_RDP+0x24>
300051be:	eb00 01c1 	add.w	r1, r0, r1, lsl #3
300051c2:	4b0b      	ldr	r3, [pc, #44]	; (300051f0 <BOOT_OTA_RDP+0x60>)
300051c4:	f44f 70c8 	mov.w	r0, #400	; 0x190
300051c8:	fb00 300e 	mla	r0, r0, lr, r3
300051cc:	f7ff fa24 	bl	30004618 <BOOT_LoadRDPImg>
300051d0:	b100      	cbz	r0, 300051d4 <BOOT_OTA_RDP+0x44>
300051d2:	bd10      	pop	{r4, pc}
300051d4:	4b07      	ldr	r3, [pc, #28]	; (300051f4 <BOOT_OTA_RDP+0x64>)
300051d6:	2245      	movs	r2, #69	; 0x45
300051d8:	4907      	ldr	r1, [pc, #28]	; (300051f8 <BOOT_OTA_RDP+0x68>)
300051da:	2002      	movs	r0, #2
300051dc:	4c07      	ldr	r4, [pc, #28]	; (300051fc <BOOT_OTA_RDP+0x6c>)
300051de:	f005 f89b 	bl	3000a318 <rtk_log_write>
300051e2:	f44f 707a 	mov.w	r0, #1000	; 0x3e8
300051e6:	47a0      	blx	r4
300051e8:	e7fb      	b.n	300051e2 <BOOT_OTA_RDP+0x52>
300051ea:	bf00      	nop
300051ec:	3000f72c 	.word	0x3000f72c
300051f0:	3000f40c 	.word	0x3000f40c
300051f4:	3000ccb0 	.word	0x3000ccb0
300051f8:	3000c7c4 	.word	0x3000c7c4
300051fc:	00009be5 	.word	0x00009be5

30005200 <BOOT_OTA_AP>:
30005200:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
30005204:	3202      	adds	r2, #2
30005206:	b087      	sub	sp, #28
30005208:	4c26      	ldr	r4, [pc, #152]	; (300052a4 <BOOT_OTA_AP+0xa4>)
3000520a:	4607      	mov	r7, r0
3000520c:	4b26      	ldr	r3, [pc, #152]	; (300052a8 <BOOT_OTA_AP+0xa8>)
3000520e:	ad02      	add	r5, sp, #8
30005210:	f854 4022 	ldr.w	r4, [r4, r2, lsl #2]
30005214:	460e      	mov	r6, r1
30005216:	cb0f      	ldmia	r3, {r0, r1, r2, r3}
30005218:	f504 5480 	add.w	r4, r4, #4096	; 0x1000
3000521c:	e885 000f 	stmia.w	r5, {r0, r1, r2, r3}
30005220:	b16e      	cbz	r6, 3000523e <BOOT_OTA_AP+0x3e>
30005222:	f106 3cff 	add.w	ip, r6, #4294967295	; 0xffffffff
30005226:	463b      	mov	r3, r7
30005228:	fa5f f28c 	uxtb.w	r2, ip
3000522c:	f107 0c08 	add.w	ip, r7, #8
30005230:	eb0c 0cc2 	add.w	ip, ip, r2, lsl #3
30005234:	685a      	ldr	r2, [r3, #4]
30005236:	3308      	adds	r3, #8
30005238:	4563      	cmp	r3, ip
3000523a:	4414      	add	r4, r2
3000523c:	d1fa      	bne.n	30005234 <BOOT_OTA_AP+0x34>
3000523e:	f8df 8084 	ldr.w	r8, [pc, #132]	; 300052c4 <BOOT_OTA_AP+0xc4>
30005242:	4623      	mov	r3, r4
30005244:	4a19      	ldr	r2, [pc, #100]	; (300052ac <BOOT_OTA_AP+0xac>)
30005246:	2003      	movs	r0, #3
30005248:	4641      	mov	r1, r8
3000524a:	f8df 907c 	ldr.w	r9, [pc, #124]	; 300052c8 <BOOT_OTA_AP+0xc8>
3000524e:	47c8      	blx	r9
30005250:	2101      	movs	r1, #1
30005252:	2003      	movs	r0, #3
30005254:	4b16      	ldr	r3, [pc, #88]	; (300052b0 <BOOT_OTA_AP+0xb0>)
30005256:	4798      	blx	r3
30005258:	2201      	movs	r2, #1
3000525a:	4914      	ldr	r1, [pc, #80]	; (300052ac <BOOT_OTA_AP+0xac>)
3000525c:	2303      	movs	r3, #3
3000525e:	4640      	mov	r0, r8
30005260:	f7ff ff34 	bl	300050cc <BOOT_OTFCheck>
30005264:	4b13      	ldr	r3, [pc, #76]	; (300052b4 <BOOT_OTA_AP+0xb4>)
30005266:	4798      	blx	r3
30005268:	2800      	cmp	r0, #0
3000526a:	bf14      	ite	ne
3000526c:	4641      	movne	r1, r8
3000526e:	4621      	moveq	r1, r4
30005270:	2401      	movs	r4, #1
30005272:	462b      	mov	r3, r5
30005274:	eb07 00c6 	add.w	r0, r7, r6, lsl #3
30005278:	2204      	movs	r2, #4
3000527a:	9400      	str	r4, [sp, #0]
3000527c:	f7ff fe46 	bl	30004f0c <BOOT_LoadSubImage>
30005280:	42a0      	cmp	r0, r4
30005282:	d103      	bne.n	3000528c <BOOT_OTA_AP+0x8c>
30005284:	2004      	movs	r0, #4
30005286:	b007      	add	sp, #28
30005288:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
3000528c:	4b0a      	ldr	r3, [pc, #40]	; (300052b8 <BOOT_OTA_AP+0xb8>)
3000528e:	2245      	movs	r2, #69	; 0x45
30005290:	490a      	ldr	r1, [pc, #40]	; (300052bc <BOOT_OTA_AP+0xbc>)
30005292:	2002      	movs	r0, #2
30005294:	4c0a      	ldr	r4, [pc, #40]	; (300052c0 <BOOT_OTA_AP+0xc0>)
30005296:	f005 f83f 	bl	3000a318 <rtk_log_write>
3000529a:	f44f 707a 	mov.w	r0, #1000	; 0x3e8
3000529e:	47a0      	blx	r4
300052a0:	e7fb      	b.n	3000529a <BOOT_OTA_AP+0x9a>
300052a2:	bf00      	nop
300052a4:	3000f72c 	.word	0x3000f72c
300052a8:	3000ce3c 	.word	0x3000ce3c
300052ac:	0fffffe0 	.word	0x0fffffe0
300052b0:	3000b351 	.word	0x3000b351
300052b4:	3000b4ed 	.word	0x3000b4ed
300052b8:	3000cccc 	.word	0x3000cccc
300052bc:	3000c7c4 	.word	0x3000c7c4
300052c0:	00009be5 	.word	0x00009be5
300052c4:	0e000000 	.word	0x0e000000
300052c8:	3000b311 	.word	0x3000b311

300052cc <BOOT_OTA_LoadIMG2>:
300052cc:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
300052d0:	4b7e      	ldr	r3, [pc, #504]	; (300054cc <BOOT_OTA_LoadIMG2+0x200>)
300052d2:	b08f      	sub	sp, #60	; 0x3c
300052d4:	4605      	mov	r5, r0
300052d6:	f8df 8254 	ldr.w	r8, [pc, #596]	; 3000552c <BOOT_OTA_LoadIMG2+0x260>
300052da:	ae08      	add	r6, sp, #32
300052dc:	e893 0007 	ldmia.w	r3, {r0, r1, r2}
300052e0:	330c      	adds	r3, #12
300052e2:	e886 0007 	stmia.w	r6, {r0, r1, r2}
300052e6:	2200      	movs	r2, #0
300052e8:	e9cd 2206 	strd	r2, r2, [sp, #24]
300052ec:	e893 0007 	ldmia.w	r3, {r0, r1, r2}
300052f0:	ab0b      	add	r3, sp, #44	; 0x2c
300052f2:	e883 0007 	stmia.w	r3, {r0, r1, r2}
300052f6:	f44f 73c8 	mov.w	r3, #400	; 0x190
300052fa:	1caa      	adds	r2, r5, #2
300052fc:	4974      	ldr	r1, [pc, #464]	; (300054d0 <BOOT_OTA_LoadIMG2+0x204>)
300052fe:	fb03 f305 	mul.w	r3, r3, r5
30005302:	f851 4022 	ldr.w	r4, [r1, r2, lsl #2]
30005306:	461f      	mov	r7, r3
30005308:	9205      	str	r2, [sp, #20]
3000530a:	9304      	str	r3, [sp, #16]
3000530c:	47c0      	blx	r8
3000530e:	b148      	cbz	r0, 30005324 <BOOT_OTA_LoadIMG2+0x58>
30005310:	4970      	ldr	r1, [pc, #448]	; (300054d4 <BOOT_OTA_LoadIMG2+0x208>)
30005312:	2208      	movs	r2, #8
30005314:	4b70      	ldr	r3, [pc, #448]	; (300054d8 <BOOT_OTA_LoadIMG2+0x20c>)
30005316:	a806      	add	r0, sp, #24
30005318:	4439      	add	r1, r7
3000531a:	4798      	blx	r3
3000531c:	a906      	add	r1, sp, #24
3000531e:	2001      	movs	r0, #1
30005320:	4b6e      	ldr	r3, [pc, #440]	; (300054dc <BOOT_OTA_LoadIMG2+0x210>)
30005322:	4798      	blx	r3
30005324:	f8df b208 	ldr.w	fp, [pc, #520]	; 30005530 <BOOT_OTA_LoadIMG2+0x264>
30005328:	f504 5480 	add.w	r4, r4, #4096	; 0x1000
3000532c:	4a6c      	ldr	r2, [pc, #432]	; (300054e0 <BOOT_OTA_LoadIMG2+0x214>)
3000532e:	2000      	movs	r0, #0
30005330:	4623      	mov	r3, r4
30005332:	4659      	mov	r1, fp
30005334:	4f6b      	ldr	r7, [pc, #428]	; (300054e4 <BOOT_OTA_LoadIMG2+0x218>)
30005336:	f04f 0801 	mov.w	r8, #1
3000533a:	47b8      	blx	r7
3000533c:	2101      	movs	r1, #1
3000533e:	2000      	movs	r0, #0
30005340:	4b69      	ldr	r3, [pc, #420]	; (300054e8 <BOOT_OTA_LoadIMG2+0x21c>)
30005342:	4798      	blx	r3
30005344:	2301      	movs	r3, #1
30005346:	4966      	ldr	r1, [pc, #408]	; (300054e0 <BOOT_OTA_LoadIMG2+0x214>)
30005348:	4658      	mov	r0, fp
3000534a:	461a      	mov	r2, r3
3000534c:	f8df 91e4 	ldr.w	r9, [pc, #484]	; 30005534 <BOOT_OTA_LoadIMG2+0x268>
30005350:	f7ff febc 	bl	300050cc <BOOT_OTFCheck>
30005354:	47c8      	blx	r9
30005356:	2800      	cmp	r0, #0
30005358:	bf0c      	ite	eq
3000535a:	46a2      	moveq	sl, r4
3000535c:	46da      	movne	sl, fp
3000535e:	4633      	mov	r3, r6
30005360:	2203      	movs	r2, #3
30005362:	4651      	mov	r1, sl
30005364:	4861      	ldr	r0, [pc, #388]	; (300054ec <BOOT_OTA_LoadIMG2+0x220>)
30005366:	f8cd 8000 	str.w	r8, [sp]
3000536a:	f7ff fdcf 	bl	30004f0c <BOOT_LoadSubImage>
3000536e:	b918      	cbnz	r0, 30005378 <BOOT_OTA_LoadIMG2+0xac>
30005370:	2000      	movs	r0, #0
30005372:	b00f      	add	sp, #60	; 0x3c
30005374:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30005378:	4e5c      	ldr	r6, [pc, #368]	; (300054ec <BOOT_OTA_LoadIMG2+0x220>)
3000537a:	47c8      	blx	r9
3000537c:	2800      	cmp	r0, #0
3000537e:	bf08      	it	eq
30005380:	46a3      	moveq	fp, r4
30005382:	4640      	mov	r0, r8
30005384:	6872      	ldr	r2, [r6, #4]
30005386:	68f3      	ldr	r3, [r6, #12]
30005388:	f8c6 b000 	str.w	fp, [r6]
3000538c:	4413      	add	r3, r2
3000538e:	6972      	ldr	r2, [r6, #20]
30005390:	f8df b1a4 	ldr.w	fp, [pc, #420]	; 30005538 <BOOT_OTA_LoadIMG2+0x26c>
30005394:	441a      	add	r2, r3
30005396:	4b56      	ldr	r3, [pc, #344]	; (300054f0 <BOOT_OTA_LoadIMG2+0x224>)
30005398:	eb0a 0102 	add.w	r1, sl, r2
3000539c:	61b1      	str	r1, [r6, #24]
3000539e:	1e51      	subs	r1, r2, #1
300053a0:	400b      	ands	r3, r1
300053a2:	4659      	mov	r1, fp
300053a4:	f503 5380 	add.w	r3, r3, #4096	; 0x1000
300053a8:	441c      	add	r4, r3
300053aa:	1a9b      	subs	r3, r3, r2
300053ac:	4a51      	ldr	r2, [pc, #324]	; (300054f4 <BOOT_OTA_LoadIMG2+0x228>)
300053ae:	61f3      	str	r3, [r6, #28]
300053b0:	4623      	mov	r3, r4
300053b2:	47b8      	blx	r7
300053b4:	4641      	mov	r1, r8
300053b6:	4640      	mov	r0, r8
300053b8:	4b4b      	ldr	r3, [pc, #300]	; (300054e8 <BOOT_OTA_LoadIMG2+0x21c>)
300053ba:	4798      	blx	r3
300053bc:	2302      	movs	r3, #2
300053be:	4642      	mov	r2, r8
300053c0:	494c      	ldr	r1, [pc, #304]	; (300054f4 <BOOT_OTA_LoadIMG2+0x228>)
300053c2:	4658      	mov	r0, fp
300053c4:	f7ff fe82 	bl	300050cc <BOOT_OTFCheck>
300053c8:	47c8      	blx	r9
300053ca:	ab0b      	add	r3, sp, #44	; 0x2c
300053cc:	2800      	cmp	r0, #0
300053ce:	bf0c      	ite	eq
300053d0:	4621      	moveq	r1, r4
300053d2:	4659      	movne	r1, fp
300053d4:	2203      	movs	r2, #3
300053d6:	f106 0020 	add.w	r0, r6, #32
300053da:	f8cd 8000 	str.w	r8, [sp]
300053de:	f7ff fd95 	bl	30004f0c <BOOT_LoadSubImage>
300053e2:	2800      	cmp	r0, #0
300053e4:	d0c4      	beq.n	30005370 <BOOT_OTA_LoadIMG2+0xa4>
300053e6:	47c8      	blx	r9
300053e8:	4f43      	ldr	r7, [pc, #268]	; (300054f8 <BOOT_OTA_LoadIMG2+0x22c>)
300053ea:	2800      	cmp	r0, #0
300053ec:	bf08      	it	eq
300053ee:	46a3      	moveq	fp, r4
300053f0:	9b04      	ldr	r3, [sp, #16]
300053f2:	f8c6 b020 	str.w	fp, [r6, #32]
300053f6:	441f      	add	r7, r3
300053f8:	4b40      	ldr	r3, [pc, #256]	; (300054fc <BOOT_OTA_LoadIMG2+0x230>)
300053fa:	4798      	blx	r3
300053fc:	4540      	cmp	r0, r8
300053fe:	d036      	beq.n	3000546e <BOOT_OTA_LoadIMG2+0x1a2>
30005400:	4b3f      	ldr	r3, [pc, #252]	; (30005500 <BOOT_OTA_LoadIMG2+0x234>)
30005402:	781b      	ldrb	r3, [r3, #0]
30005404:	bb43      	cbnz	r3, 30005458 <BOOT_OTA_LoadIMG2+0x18c>
30005406:	2207      	movs	r2, #7
30005408:	f44f 71b2 	mov.w	r1, #356	; 0x164
3000540c:	2400      	movs	r4, #0
3000540e:	4b3d      	ldr	r3, [pc, #244]	; (30005504 <BOOT_OTA_LoadIMG2+0x238>)
30005410:	4638      	mov	r0, r7
30005412:	9400      	str	r4, [sp, #0]
30005414:	fb01 3305 	mla	r3, r1, r5, r3
30005418:	4934      	ldr	r1, [pc, #208]	; (300054ec <BOOT_OTA_LoadIMG2+0x220>)
3000541a:	f7ff faff 	bl	30004a1c <BOOT_SignatureCheck>
3000541e:	3501      	adds	r5, #1
30005420:	4604      	mov	r4, r0
30005422:	b958      	cbnz	r0, 3000543c <BOOT_OTA_LoadIMG2+0x170>
30005424:	4938      	ldr	r1, [pc, #224]	; (30005508 <BOOT_OTA_LoadIMG2+0x23c>)
30005426:	2249      	movs	r2, #73	; 0x49
30005428:	4b38      	ldr	r3, [pc, #224]	; (3000550c <BOOT_OTA_LoadIMG2+0x240>)
3000542a:	9500      	str	r5, [sp, #0]
3000542c:	e9cd 1001 	strd	r1, r0, [sp, #4]
30005430:	2004      	movs	r0, #4
30005432:	4937      	ldr	r1, [pc, #220]	; (30005510 <BOOT_OTA_LoadIMG2+0x244>)
30005434:	f004 ff70 	bl	3000a318 <rtk_log_write>
30005438:	4620      	mov	r0, r4
3000543a:	e79a      	b.n	30005372 <BOOT_OTA_LoadIMG2+0xa6>
3000543c:	4935      	ldr	r1, [pc, #212]	; (30005514 <BOOT_OTA_LoadIMG2+0x248>)
3000543e:	2249      	movs	r2, #73	; 0x49
30005440:	4b32      	ldr	r3, [pc, #200]	; (3000550c <BOOT_OTA_LoadIMG2+0x240>)
30005442:	9500      	str	r5, [sp, #0]
30005444:	e9cd 1001 	strd	r1, r0, [sp, #4]
30005448:	4931      	ldr	r1, [pc, #196]	; (30005510 <BOOT_OTA_LoadIMG2+0x244>)
3000544a:	2004      	movs	r0, #4
3000544c:	f004 ff64 	bl	3000a318 <rtk_log_write>
30005450:	2001      	movs	r0, #1
30005452:	b00f      	add	sp, #60	; 0x3c
30005454:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30005458:	2107      	movs	r1, #7
3000545a:	460c      	mov	r4, r1
3000545c:	462a      	mov	r2, r5
3000545e:	4823      	ldr	r0, [pc, #140]	; (300054ec <BOOT_OTA_LoadIMG2+0x220>)
30005460:	f7ff fece 	bl	30005200 <BOOT_OTA_AP>
30005464:	4404      	add	r4, r0
30005466:	2c0d      	cmp	r4, #13
30005468:	d82b      	bhi.n	300054c2 <BOOT_OTA_LoadIMG2+0x1f6>
3000546a:	b2e2      	uxtb	r2, r4
3000546c:	e7cc      	b.n	30005408 <BOOT_OTA_LoadIMG2+0x13c>
3000546e:	4b2a      	ldr	r3, [pc, #168]	; (30005518 <BOOT_OTA_LoadIMG2+0x24c>)
30005470:	2249      	movs	r2, #73	; 0x49
30005472:	4927      	ldr	r1, [pc, #156]	; (30005510 <BOOT_OTA_LoadIMG2+0x244>)
30005474:	2004      	movs	r0, #4
30005476:	f004 ff4f 	bl	3000a318 <rtk_log_write>
3000547a:	4b15      	ldr	r3, [pc, #84]	; (300054d0 <BOOT_OTA_LoadIMG2+0x204>)
3000547c:	9a05      	ldr	r2, [sp, #20]
3000547e:	f106 0138 	add.w	r1, r6, #56	; 0x38
30005482:	f853 2022 	ldr.w	r2, [r3, r2, lsl #2]
30005486:	f502 5280 	add.w	r2, r2, #4096	; 0x1000
3000548a:	6873      	ldr	r3, [r6, #4]
3000548c:	3608      	adds	r6, #8
3000548e:	428e      	cmp	r6, r1
30005490:	441a      	add	r2, r3
30005492:	d1fa      	bne.n	3000548a <BOOT_OTA_LoadIMG2+0x1be>
30005494:	4921      	ldr	r1, [pc, #132]	; (3000551c <BOOT_OTA_LoadIMG2+0x250>)
30005496:	4638      	mov	r0, r7
30005498:	f7ff f8be 	bl	30004618 <BOOT_LoadRDPImg>
3000549c:	b130      	cbz	r0, 300054ac <BOOT_OTA_LoadIMG2+0x1e0>
3000549e:	4b18      	ldr	r3, [pc, #96]	; (30005500 <BOOT_OTA_LoadIMG2+0x234>)
300054a0:	1dc4      	adds	r4, r0, #7
300054a2:	781b      	ldrb	r3, [r3, #0]
300054a4:	2b00      	cmp	r3, #0
300054a6:	d0de      	beq.n	30005466 <BOOT_OTA_LoadIMG2+0x19a>
300054a8:	b2e1      	uxtb	r1, r4
300054aa:	e7d7      	b.n	3000545c <BOOT_OTA_LoadIMG2+0x190>
300054ac:	4b1c      	ldr	r3, [pc, #112]	; (30005520 <BOOT_OTA_LoadIMG2+0x254>)
300054ae:	2245      	movs	r2, #69	; 0x45
300054b0:	4917      	ldr	r1, [pc, #92]	; (30005510 <BOOT_OTA_LoadIMG2+0x244>)
300054b2:	2002      	movs	r0, #2
300054b4:	4c1b      	ldr	r4, [pc, #108]	; (30005524 <BOOT_OTA_LoadIMG2+0x258>)
300054b6:	f004 ff2f 	bl	3000a318 <rtk_log_write>
300054ba:	f44f 707a 	mov.w	r0, #1000	; 0x3e8
300054be:	47a0      	blx	r4
300054c0:	e7fb      	b.n	300054ba <BOOT_OTA_LoadIMG2+0x1ee>
300054c2:	f240 1131 	movw	r1, #305	; 0x131
300054c6:	4818      	ldr	r0, [pc, #96]	; (30005528 <BOOT_OTA_LoadIMG2+0x25c>)
300054c8:	f006 fd0a 	bl	3000bee0 <io_assert_failed>
300054cc:	3000ce4c 	.word	0x3000ce4c
300054d0:	3000f72c 	.word	0x3000f72c
300054d4:	3000f42c 	.word	0x3000f42c
300054d8:	00012a1d 	.word	0x00012a1d
300054dc:	3000b2dd 	.word	0x3000b2dd
300054e0:	0cffffe0 	.word	0x0cffffe0
300054e4:	3000b311 	.word	0x3000b311
300054e8:	3000b351 	.word	0x3000b351
300054ec:	3000f864 	.word	0x3000f864
300054f0:	fffff000 	.word	0xfffff000
300054f4:	0dffffe0 	.word	0x0dffffe0
300054f8:	3000f40c 	.word	0x3000f40c
300054fc:	0000c109 	.word	0x0000c109
30005500:	3000eebe 	.word	0x3000eebe
30005504:	3000f144 	.word	0x3000f144
30005508:	3000cd10 	.word	0x3000cd10
3000550c:	3000ccec 	.word	0x3000ccec
30005510:	3000c7c4 	.word	0x3000c7c4
30005514:	3000cd08 	.word	0x3000cd08
30005518:	3000cce4 	.word	0x3000cce4
3000551c:	3000f89c 	.word	0x3000f89c
30005520:	3000ccb0 	.word	0x3000ccb0
30005524:	00009be5 	.word	0x00009be5
30005528:	3000cdc8 	.word	0x3000cdc8
3000552c:	0000c129 	.word	0x0000c129
30005530:	0c000000 	.word	0x0c000000
30005534:	3000b4ed 	.word	0x3000b4ed
30005538:	0d000000 	.word	0x0d000000

3000553c <BOOT_OTA_IMG2>:
3000553c:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
30005540:	4c4c      	ldr	r4, [pc, #304]	; (30005674 <BOOT_OTA_IMG2+0x138>)
30005542:	b084      	sub	sp, #16
30005544:	2200      	movs	r2, #0
30005546:	2002      	movs	r0, #2
30005548:	4621      	mov	r1, r4
3000554a:	4d4b      	ldr	r5, [pc, #300]	; (30005678 <BOOT_OTA_IMG2+0x13c>)
3000554c:	f004 ff9e 	bl	3000a48c <flash_get_layout_info>
30005550:	2200      	movs	r2, #0
30005552:	1d21      	adds	r1, r4, #4
30005554:	2003      	movs	r0, #3
30005556:	f004 ff99 	bl	3000a48c <flash_get_layout_info>
3000555a:	e9d4 2300 	ldrd	r2, r3, [r4]
3000555e:	f503 5380 	add.w	r3, r3, #4096	; 0x1000
30005562:	f502 5280 	add.w	r2, r2, #4096	; 0x1000
30005566:	e9c4 2302 	strd	r2, r3, [r4, #8]
3000556a:	f7ff fc1b 	bl	30004da4 <BOOT_OTA_SlotSelect>
3000556e:	782b      	ldrb	r3, [r5, #0]
30005570:	b373      	cbz	r3, 300055d0 <BOOT_OTA_IMG2+0x94>
30005572:	f04f 0a00 	mov.w	sl, #0
30005576:	4604      	mov	r4, r0
30005578:	4f40      	ldr	r7, [pc, #256]	; (3000567c <BOOT_OTA_IMG2+0x140>)
3000557a:	f44f 76b2 	mov.w	r6, #356	; 0x164
3000557e:	f8df 911c 	ldr.w	r9, [pc, #284]	; 3000569c <BOOT_OTA_IMG2+0x160>
30005582:	46d0      	mov	r8, sl
30005584:	4621      	mov	r1, r4
30005586:	fb06 7004 	mla	r0, r6, r4, r7
3000558a:	f7ff fb4b 	bl	30004c24 <BOOT_CertificateCheck>
3000558e:	2801      	cmp	r0, #1
30005590:	d043      	beq.n	3000561a <BOOT_OTA_IMG2+0xde>
30005592:	f10a 0a01 	add.w	sl, sl, #1
30005596:	782b      	ldrb	r3, [r5, #0]
30005598:	3401      	adds	r4, #1
3000559a:	fa5f fa8a 	uxtb.w	sl, sl
3000559e:	f004 0401 	and.w	r4, r4, #1
300055a2:	4553      	cmp	r3, sl
300055a4:	d8ee      	bhi.n	30005584 <BOOT_OTA_IMG2+0x48>
300055a6:	459a      	cmp	sl, r3
300055a8:	d012      	beq.n	300055d0 <BOOT_OTA_IMG2+0x94>
300055aa:	4935      	ldr	r1, [pc, #212]	; (30005680 <BOOT_OTA_IMG2+0x144>)
300055ac:	1c60      	adds	r0, r4, #1
300055ae:	4b35      	ldr	r3, [pc, #212]	; (30005684 <BOOT_OTA_IMG2+0x148>)
300055b0:	2249      	movs	r2, #73	; 0x49
300055b2:	f851 1034 	ldr.w	r1, [r1, r4, lsl #3]
300055b6:	9000      	str	r0, [sp, #0]
300055b8:	b28d      	uxth	r5, r1
300055ba:	0c08      	lsrs	r0, r1, #16
300055bc:	4932      	ldr	r1, [pc, #200]	; (30005688 <BOOT_OTA_IMG2+0x14c>)
300055be:	9502      	str	r5, [sp, #8]
300055c0:	9001      	str	r0, [sp, #4]
300055c2:	2004      	movs	r0, #4
300055c4:	f004 fea8 	bl	3000a318 <rtk_log_write>
300055c8:	4620      	mov	r0, r4
300055ca:	b004      	add	sp, #16
300055cc:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
300055d0:	4b2e      	ldr	r3, [pc, #184]	; (3000568c <BOOT_OTA_IMG2+0x150>)
300055d2:	2245      	movs	r2, #69	; 0x45
300055d4:	492c      	ldr	r1, [pc, #176]	; (30005688 <BOOT_OTA_IMG2+0x14c>)
300055d6:	2002      	movs	r0, #2
300055d8:	f004 fe9e 	bl	3000a318 <rtk_log_write>
300055dc:	4b2c      	ldr	r3, [pc, #176]	; (30005690 <BOOT_OTA_IMG2+0x154>)
300055de:	f383 8808 	msr	MSP, r3
300055e2:	f641 727c 	movw	r2, #8060	; 0x1f7c
300055e6:	4b2b      	ldr	r3, [pc, #172]	; (30005694 <BOOT_OTA_IMG2+0x158>)
300055e8:	2100      	movs	r1, #0
300055ea:	482b      	ldr	r0, [pc, #172]	; (30005698 <BOOT_OTA_IMG2+0x15c>)
300055ec:	4798      	blx	r3
300055ee:	4a2b      	ldr	r2, [pc, #172]	; (3000569c <BOOT_OTA_IMG2+0x160>)
300055f0:	6953      	ldr	r3, [r2, #20]
300055f2:	03db      	lsls	r3, r3, #15
300055f4:	d50c      	bpl.n	30005610 <BOOT_OTA_IMG2+0xd4>
300055f6:	f3bf 8f4f 	dsb	sy
300055fa:	4b27      	ldr	r3, [pc, #156]	; (30005698 <BOOT_OTA_IMG2+0x15c>)
300055fc:	4928      	ldr	r1, [pc, #160]	; (300056a0 <BOOT_OTA_IMG2+0x164>)
300055fe:	f8c2 3270 	str.w	r3, [r2, #624]	; 0x270
30005602:	3320      	adds	r3, #32
30005604:	428b      	cmp	r3, r1
30005606:	d1fa      	bne.n	300055fe <BOOT_OTA_IMG2+0xc2>
30005608:	f3bf 8f4f 	dsb	sy
3000560c:	f3bf 8f6f 	isb	sy
30005610:	4c24      	ldr	r4, [pc, #144]	; (300056a4 <BOOT_OTA_IMG2+0x168>)
30005612:	f44f 707a 	mov.w	r0, #1000	; 0x3e8
30005616:	47a0      	blx	r4
30005618:	e7fb      	b.n	30005612 <BOOT_OTA_IMG2+0xd6>
3000561a:	4620      	mov	r0, r4
3000561c:	f7ff fe56 	bl	300052cc <BOOT_OTA_LoadIMG2>
30005620:	2801      	cmp	r0, #1
30005622:	d103      	bne.n	3000562c <BOOT_OTA_IMG2+0xf0>
30005624:	782b      	ldrb	r3, [r5, #0]
30005626:	459a      	cmp	sl, r3
30005628:	d1bf      	bne.n	300055aa <BOOT_OTA_IMG2+0x6e>
3000562a:	e7d1      	b.n	300055d0 <BOOT_OTA_IMG2+0x94>
3000562c:	f8d9 3014 	ldr.w	r3, [r9, #20]
30005630:	03d9      	lsls	r1, r3, #15
30005632:	d5ae      	bpl.n	30005592 <BOOT_OTA_IMG2+0x56>
30005634:	f8c9 8084 	str.w	r8, [r9, #132]	; 0x84
30005638:	f3bf 8f4f 	dsb	sy
3000563c:	f8d9 3080 	ldr.w	r3, [r9, #128]	; 0x80
30005640:	f643 7ee0 	movw	lr, #16352	; 0x3fe0
30005644:	f3c3 304e 	ubfx	r0, r3, #13, #15
30005648:	f3c3 0cc9 	ubfx	ip, r3, #3, #10
3000564c:	0140      	lsls	r0, r0, #5
3000564e:	ea00 010e 	and.w	r1, r0, lr
30005652:	4663      	mov	r3, ip
30005654:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
30005658:	3b01      	subs	r3, #1
3000565a:	f8c9 2274 	str.w	r2, [r9, #628]	; 0x274
3000565e:	1c5a      	adds	r2, r3, #1
30005660:	d1f8      	bne.n	30005654 <BOOT_OTA_IMG2+0x118>
30005662:	3820      	subs	r0, #32
30005664:	f110 0f20 	cmn.w	r0, #32
30005668:	d1f1      	bne.n	3000564e <BOOT_OTA_IMG2+0x112>
3000566a:	f3bf 8f4f 	dsb	sy
3000566e:	f3bf 8f6f 	isb	sy
30005672:	e78e      	b.n	30005592 <BOOT_OTA_IMG2+0x56>
30005674:	3000f72c 	.word	0x3000f72c
30005678:	3000f8d4 	.word	0x3000f8d4
3000567c:	3000f144 	.word	0x3000f144
30005680:	3000f8d8 	.word	0x3000f8d8
30005684:	3000cd18 	.word	0x3000cd18
30005688:	3000c7c4 	.word	0x3000c7c4
3000568c:	3000cd44 	.word	0x3000cd44
30005690:	30002ffc 	.word	0x30002ffc
30005694:	00012be5 	.word	0x00012be5
30005698:	30001000 	.word	0x30001000
3000569c:	e000ed00 	.word	0xe000ed00
300056a0:	30002f80 	.word	0x30002f80
300056a4:	00009be5 	.word	0x00009be5

300056a8 <BOOT_OTA_AP_Linux>:
300056a8:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
300056ac:	4b46      	ldr	r3, [pc, #280]	; (300057c8 <BOOT_OTA_AP_Linux+0x120>)
300056ae:	4605      	mov	r5, r0
300056b0:	f8df 9138 	ldr.w	r9, [pc, #312]	; 300057ec <BOOT_OTA_AP_Linux+0x144>
300056b4:	ed2d 8b02 	vpush	{d8}
300056b8:	b0eb      	sub	sp, #428	; 0x1ac
300056ba:	e893 0007 	ldmia.w	r3, {r0, r1, r2}
300056be:	af03      	add	r7, sp, #12
300056c0:	f899 3000 	ldrb.w	r3, [r9]
300056c4:	e887 0007 	stmia.w	r7, {r0, r1, r2}
300056c8:	2b00      	cmp	r3, #0
300056ca:	d072      	beq.n	300057b2 <BOOT_OTA_AP_Linux+0x10a>
300056cc:	4b3f      	ldr	r3, [pc, #252]	; (300057cc <BOOT_OTA_AP_Linux+0x124>)
300056ce:	f44f 72b2 	mov.w	r2, #356	; 0x164
300056d2:	2600      	movs	r6, #0
300056d4:	f8df b118 	ldr.w	fp, [pc, #280]	; 300057f0 <BOOT_OTA_AP_Linux+0x148>
300056d8:	fb02 3305 	mla	r3, r2, r5, r3
300056dc:	f8df 8114 	ldr.w	r8, [pc, #276]	; 300057f4 <BOOT_OTA_AP_Linux+0x14c>
300056e0:	4c3b      	ldr	r4, [pc, #236]	; (300057d0 <BOOT_OTA_AP_Linux+0x128>)
300056e2:	ee08 3a10 	vmov	s16, r3
300056e6:	e017      	b.n	30005718 <BOOT_OTA_AP_Linux+0x70>
300056e8:	a806      	add	r0, sp, #24
300056ea:	4b3a      	ldr	r3, [pc, #232]	; (300057d4 <BOOT_OTA_AP_Linux+0x12c>)
300056ec:	4798      	blx	r3
300056ee:	2301      	movs	r3, #1
300056f0:	f50a 5180 	add.w	r1, sl, #4096	; 0x1000
300056f4:	2203      	movs	r2, #3
300056f6:	4640      	mov	r0, r8
300056f8:	9300      	str	r3, [sp, #0]
300056fa:	463b      	mov	r3, r7
300056fc:	f7ff fc06 	bl	30004f0c <BOOT_LoadSubImage>
30005700:	2801      	cmp	r0, #1
30005702:	f105 0501 	add.w	r5, r5, #1
30005706:	d015      	beq.n	30005734 <BOOT_OTA_AP_Linux+0x8c>
30005708:	3601      	adds	r6, #1
3000570a:	f899 3000 	ldrb.w	r3, [r9]
3000570e:	f005 0501 	and.w	r5, r5, #1
30005712:	b2f6      	uxtb	r6, r6
30005714:	42b3      	cmp	r3, r6
30005716:	d944      	bls.n	300057a2 <BOOT_OTA_AP_Linux+0xfa>
30005718:	1d2b      	adds	r3, r5, #4
3000571a:	f85b a023 	ldr.w	sl, [fp, r3, lsl #2]
3000571e:	4b2e      	ldr	r3, [pc, #184]	; (300057d8 <BOOT_OTA_AP_Linux+0x130>)
30005720:	4798      	blx	r3
30005722:	f44f 72c8 	mov.w	r2, #400	; 0x190
30005726:	4651      	mov	r1, sl
30005728:	2800      	cmp	r0, #0
3000572a:	d1dd      	bne.n	300056e8 <BOOT_OTA_AP_Linux+0x40>
3000572c:	a806      	add	r0, sp, #24
3000572e:	f004 fd31 	bl	3000a194 <NandImgCopy>
30005732:	e7dc      	b.n	300056ee <BOOT_OTA_AP_Linux+0x46>
30005734:	2302      	movs	r3, #2
30005736:	2203      	movs	r2, #3
30005738:	4641      	mov	r1, r8
3000573a:	a806      	add	r0, sp, #24
3000573c:	9300      	str	r3, [sp, #0]
3000573e:	ee18 3a10 	vmov	r3, s16
30005742:	f7ff f96b 	bl	30004a1c <BOOT_SignatureCheck>
30005746:	2801      	cmp	r0, #1
30005748:	d022      	beq.n	30005790 <BOOT_OTA_AP_Linux+0xe8>
3000574a:	6963      	ldr	r3, [r4, #20]
3000574c:	03db      	lsls	r3, r3, #15
3000574e:	d5db      	bpl.n	30005708 <BOOT_OTA_AP_Linux+0x60>
30005750:	2300      	movs	r3, #0
30005752:	f8c4 3084 	str.w	r3, [r4, #132]	; 0x84
30005756:	f3bf 8f4f 	dsb	sy
3000575a:	f8d4 3080 	ldr.w	r3, [r4, #128]	; 0x80
3000575e:	f643 7ee0 	movw	lr, #16352	; 0x3fe0
30005762:	f3c3 304e 	ubfx	r0, r3, #13, #15
30005766:	f3c3 0cc9 	ubfx	ip, r3, #3, #10
3000576a:	0140      	lsls	r0, r0, #5
3000576c:	ea00 010e 	and.w	r1, r0, lr
30005770:	4663      	mov	r3, ip
30005772:	ea41 7283 	orr.w	r2, r1, r3, lsl #30
30005776:	3b01      	subs	r3, #1
30005778:	f8c4 2274 	str.w	r2, [r4, #628]	; 0x274
3000577c:	d2f9      	bcs.n	30005772 <BOOT_OTA_AP_Linux+0xca>
3000577e:	3820      	subs	r0, #32
30005780:	f110 0f20 	cmn.w	r0, #32
30005784:	d1f2      	bne.n	3000576c <BOOT_OTA_AP_Linux+0xc4>
30005786:	f3bf 8f4f 	dsb	sy
3000578a:	f3bf 8f6f 	isb	sy
3000578e:	e7bb      	b.n	30005708 <BOOT_OTA_AP_Linux+0x60>
30005790:	4b12      	ldr	r3, [pc, #72]	; (300057dc <BOOT_OTA_AP_Linux+0x134>)
30005792:	2249      	movs	r2, #73	; 0x49
30005794:	4912      	ldr	r1, [pc, #72]	; (300057e0 <BOOT_OTA_AP_Linux+0x138>)
30005796:	2004      	movs	r0, #4
30005798:	9500      	str	r5, [sp, #0]
3000579a:	f004 fdbd 	bl	3000a318 <rtk_log_write>
3000579e:	f899 3000 	ldrb.w	r3, [r9]
300057a2:	429e      	cmp	r6, r3
300057a4:	d005      	beq.n	300057b2 <BOOT_OTA_AP_Linux+0x10a>
300057a6:	2000      	movs	r0, #0
300057a8:	b06b      	add	sp, #428	; 0x1ac
300057aa:	ecbd 8b02 	vpop	{d8}
300057ae:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
300057b2:	4b0c      	ldr	r3, [pc, #48]	; (300057e4 <BOOT_OTA_AP_Linux+0x13c>)
300057b4:	2245      	movs	r2, #69	; 0x45
300057b6:	490a      	ldr	r1, [pc, #40]	; (300057e0 <BOOT_OTA_AP_Linux+0x138>)
300057b8:	2002      	movs	r0, #2
300057ba:	4c0b      	ldr	r4, [pc, #44]	; (300057e8 <BOOT_OTA_AP_Linux+0x140>)
300057bc:	f004 fdac 	bl	3000a318 <rtk_log_write>
300057c0:	f44f 707a 	mov.w	r0, #1000	; 0x3e8
300057c4:	47a0      	blx	r4
300057c6:	e7fb      	b.n	300057c0 <BOOT_OTA_AP_Linux+0x118>
300057c8:	3000ce64 	.word	0x3000ce64
300057cc:	3000f144 	.word	0x3000f144
300057d0:	e000ed00 	.word	0xe000ed00
300057d4:	00012a1d 	.word	0x00012a1d
300057d8:	3000b4ed 	.word	0x3000b4ed
300057dc:	3000cd74 	.word	0x3000cd74
300057e0:	3000c7c4 	.word	0x3000c7c4
300057e4:	3000cccc 	.word	0x3000cccc
300057e8:	00009be5 	.word	0x00009be5
300057ec:	3000f8d4 	.word	0x3000f8d4
300057f0:	3000f72c 	.word	0x3000f72c
300057f4:	3000f864 	.word	0x3000f864

300057f8 <FLASH_ClockDiv>:
300057f8:	2200      	movs	r2, #0
300057fa:	2180      	movs	r1, #128	; 0x80
300057fc:	b538      	push	{r3, r4, r5, lr}
300057fe:	4605      	mov	r5, r0
30005800:	4c1e      	ldr	r4, [pc, #120]	; (3000587c <FLASH_ClockDiv+0x84>)
30005802:	4610      	mov	r0, r2
30005804:	47a0      	blx	r4
30005806:	f04f 4282 	mov.w	r2, #1090519040	; 0x41000000
3000580a:	022d      	lsls	r5, r5, #8
3000580c:	230a      	movs	r3, #10
3000580e:	6c11      	ldr	r1, [r2, #64]	; 0x40
30005810:	f405 6570 	and.w	r5, r5, #3840	; 0xf00
30005814:	f421 5140 	bic.w	r1, r1, #12288	; 0x3000
30005818:	6411      	str	r1, [r2, #64]	; 0x40
3000581a:	6c11      	ldr	r1, [r2, #64]	; 0x40
3000581c:	f421 6170 	bic.w	r1, r1, #3840	; 0xf00
30005820:	430d      	orrs	r5, r1
30005822:	4917      	ldr	r1, [pc, #92]	; (30005880 <FLASH_ClockDiv+0x88>)
30005824:	6415      	str	r5, [r2, #64]	; 0x40
30005826:	e001      	b.n	3000582c <FLASH_ClockDiv+0x34>
30005828:	2a00      	cmp	r2, #0
3000582a:	db02      	blt.n	30005832 <FLASH_ClockDiv+0x3a>
3000582c:	3b01      	subs	r3, #1
3000582e:	6c0a      	ldr	r2, [r1, #64]	; 0x40
30005830:	d1fa      	bne.n	30005828 <FLASH_ClockDiv+0x30>
30005832:	230a      	movs	r3, #10
30005834:	4912      	ldr	r1, [pc, #72]	; (30005880 <FLASH_ClockDiv+0x88>)
30005836:	e000      	b.n	3000583a <FLASH_ClockDiv+0x42>
30005838:	b11b      	cbz	r3, 30005842 <FLASH_ClockDiv+0x4a>
3000583a:	6c0a      	ldr	r2, [r1, #64]	; 0x40
3000583c:	3b01      	subs	r3, #1
3000583e:	0092      	lsls	r2, r2, #2
30005840:	d5fa      	bpl.n	30005838 <FLASH_ClockDiv+0x40>
30005842:	f04f 4182 	mov.w	r1, #1090519040	; 0x41000000
30005846:	220a      	movs	r2, #10
30005848:	6c0b      	ldr	r3, [r1, #64]	; 0x40
3000584a:	f443 5300 	orr.w	r3, r3, #8192	; 0x2000
3000584e:	640b      	str	r3, [r1, #64]	; 0x40
30005850:	6c0b      	ldr	r3, [r1, #64]	; 0x40
30005852:	f443 5380 	orr.w	r3, r3, #4096	; 0x1000
30005856:	640b      	str	r3, [r1, #64]	; 0x40
30005858:	e000      	b.n	3000585c <FLASH_ClockDiv+0x64>
3000585a:	b12a      	cbz	r2, 30005868 <FLASH_ClockDiv+0x70>
3000585c:	6c0b      	ldr	r3, [r1, #64]	; 0x40
3000585e:	3a01      	subs	r2, #1
30005860:	f003 0303 	and.w	r3, r3, #3
30005864:	2b03      	cmp	r3, #3
30005866:	d1f8      	bne.n	3000585a <FLASH_ClockDiv+0x62>
30005868:	2201      	movs	r2, #1
3000586a:	2180      	movs	r1, #128	; 0x80
3000586c:	f44f 7080 	mov.w	r0, #256	; 0x100
30005870:	47a0      	blx	r4
30005872:	2003      	movs	r0, #3
30005874:	4b03      	ldr	r3, [pc, #12]	; (30005884 <FLASH_ClockDiv+0x8c>)
30005876:	4798      	blx	r3
30005878:	2001      	movs	r0, #1
3000587a:	bd38      	pop	{r3, r4, r5, pc}
3000587c:	0000b479 	.word	0x0000b479
30005880:	42008800 	.word	0x42008800
30005884:	0000b569 	.word	0x0000b569

30005888 <FLASH_CalibrationPhaseIdx>:
30005888:	2200      	movs	r2, #0
3000588a:	2180      	movs	r1, #128	; 0x80
3000588c:	b538      	push	{r3, r4, r5, lr}
3000588e:	4604      	mov	r4, r0
30005890:	4d1a      	ldr	r5, [pc, #104]	; (300058fc <FLASH_CalibrationPhaseIdx+0x74>)
30005892:	4610      	mov	r0, r2
30005894:	47a8      	blx	r5
30005896:	f04f 4182 	mov.w	r1, #1090519040	; 0x41000000
3000589a:	230a      	movs	r3, #10
3000589c:	6c0a      	ldr	r2, [r1, #64]	; 0x40
3000589e:	f422 5280 	bic.w	r2, r2, #4096	; 0x1000
300058a2:	640a      	str	r2, [r1, #64]	; 0x40
300058a4:	6c0a      	ldr	r2, [r1, #64]	; 0x40
300058a6:	f422 5200 	bic.w	r2, r2, #8192	; 0x2000
300058aa:	640a      	str	r2, [r1, #64]	; 0x40
300058ac:	e000      	b.n	300058b0 <FLASH_CalibrationPhaseIdx+0x28>
300058ae:	b11b      	cbz	r3, 300058b8 <FLASH_CalibrationPhaseIdx+0x30>
300058b0:	6c0a      	ldr	r2, [r1, #64]	; 0x40
300058b2:	3b01      	subs	r3, #1
300058b4:	0792      	lsls	r2, r2, #30
300058b6:	d1fa      	bne.n	300058ae <FLASH_CalibrationPhaseIdx+0x26>
300058b8:	f04f 4182 	mov.w	r1, #1090519040	; 0x41000000
300058bc:	00a4      	lsls	r4, r4, #2
300058be:	220a      	movs	r2, #10
300058c0:	6c0b      	ldr	r3, [r1, #64]	; 0x40
300058c2:	f004 047c 	and.w	r4, r4, #124	; 0x7c
300058c6:	f023 037c 	bic.w	r3, r3, #124	; 0x7c
300058ca:	431c      	orrs	r4, r3
300058cc:	640c      	str	r4, [r1, #64]	; 0x40
300058ce:	6c0b      	ldr	r3, [r1, #64]	; 0x40
300058d0:	f443 5300 	orr.w	r3, r3, #8192	; 0x2000
300058d4:	640b      	str	r3, [r1, #64]	; 0x40
300058d6:	6c0b      	ldr	r3, [r1, #64]	; 0x40
300058d8:	f443 5380 	orr.w	r3, r3, #4096	; 0x1000
300058dc:	640b      	str	r3, [r1, #64]	; 0x40
300058de:	e000      	b.n	300058e2 <FLASH_CalibrationPhaseIdx+0x5a>
300058e0:	b12a      	cbz	r2, 300058ee <FLASH_CalibrationPhaseIdx+0x66>
300058e2:	6c0b      	ldr	r3, [r1, #64]	; 0x40
300058e4:	3a01      	subs	r2, #1
300058e6:	f003 0303 	and.w	r3, r3, #3
300058ea:	2b03      	cmp	r3, #3
300058ec:	d1f8      	bne.n	300058e0 <FLASH_CalibrationPhaseIdx+0x58>
300058ee:	2000      	movs	r0, #0
300058f0:	2201      	movs	r2, #1
300058f2:	2180      	movs	r1, #128	; 0x80
300058f4:	47a8      	blx	r5
300058f6:	2001      	movs	r0, #1
300058f8:	bd38      	pop	{r3, r4, r5, pc}
300058fa:	bf00      	nop
300058fc:	0000b479 	.word	0x0000b479

30005900 <FLASH_CalibrationNewCmd>:
30005900:	f04f 4382 	mov.w	r3, #1090519040	; 0x41000000
30005904:	2801      	cmp	r0, #1
30005906:	f04f 4282 	mov.w	r2, #1090519040	; 0x41000000
3000590a:	f04f 0001 	mov.w	r0, #1
3000590e:	6c1b      	ldr	r3, [r3, #64]	; 0x40
30005910:	bf0c      	ite	eq
30005912:	f443 4300 	orreq.w	r3, r3, #32768	; 0x8000
30005916:	f423 4300 	bicne.w	r3, r3, #32768	; 0x8000
3000591a:	6413      	str	r3, [r2, #64]	; 0x40
3000591c:	4770      	bx	lr
3000591e:	bf00      	nop

30005920 <FLASH_CalibrationNew>:
30005920:	0052      	lsls	r2, r2, #1
30005922:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30005926:	3201      	adds	r2, #1
30005928:	b087      	sub	sp, #28
3000592a:	460d      	mov	r5, r1
3000592c:	461f      	mov	r7, r3
3000592e:	4b4d      	ldr	r3, [pc, #308]	; (30005a64 <FLASH_CalibrationNew+0x144>)
30005930:	fa5f f882 	uxtb.w	r8, r2
30005934:	f89d 4040 	ldrb.w	r4, [sp, #64]	; 0x40
30005938:	f89d a044 	ldrb.w	sl, [sp, #68]	; 0x44
3000593c:	9003      	str	r0, [sp, #12]
3000593e:	4798      	blx	r3
30005940:	4629      	mov	r1, r5
30005942:	2800      	cmp	r0, #0
30005944:	d07f      	beq.n	30005a46 <FLASH_CalibrationNew+0x126>
30005946:	4848      	ldr	r0, [pc, #288]	; (30005a68 <FLASH_CalibrationNew+0x148>)
30005948:	4a48      	ldr	r2, [pc, #288]	; (30005a6c <FLASH_CalibrationNew+0x14c>)
3000594a:	4790      	blx	r2
3000594c:	4b46      	ldr	r3, [pc, #280]	; (30005a68 <FLASH_CalibrationNew+0x148>)
3000594e:	f04f 4188 	mov.w	r1, #1140850688	; 0x44000000
30005952:	0324      	lsls	r4, r4, #12
30005954:	f04f 4282 	mov.w	r2, #1090519040	; 0x41000000
30005958:	7958      	ldrb	r0, [r3, #5]
3000595a:	45d0      	cmp	r8, sl
3000595c:	b2a4      	uxth	r4, r4
3000595e:	6148      	str	r0, [r1, #20]
30005960:	f8d1 011c 	ldr.w	r0, [r1, #284]	; 0x11c
30005964:	f420 4070 	bic.w	r0, r0, #61440	; 0xf000
30005968:	ea44 0400 	orr.w	r4, r4, r0
3000596c:	f8c1 411c 	str.w	r4, [r1, #284]	; 0x11c
30005970:	6c11      	ldr	r1, [r2, #64]	; 0x40
30005972:	f421 4100 	bic.w	r1, r1, #32768	; 0x8000
30005976:	6411      	str	r1, [r2, #64]	; 0x40
30005978:	6c11      	ldr	r1, [r2, #64]	; 0x40
3000597a:	f441 4100 	orr.w	r1, r1, #32768	; 0x8000
3000597e:	6411      	str	r1, [r2, #64]	; 0x40
30005980:	d36e      	bcc.n	30005a60 <FLASH_CalibrationNew+0x140>
30005982:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
30005986:	2500      	movs	r5, #0
30005988:	f8df b0f4 	ldr.w	fp, [pc, #244]	; 30005a80 <FLASH_CalibrationNew+0x160>
3000598c:	f04f 6400 	mov.w	r4, #134217728	; 0x8000000
30005990:	4e37      	ldr	r6, [pc, #220]	; (30005a70 <FLASH_CalibrationNew+0x150>)
30005992:	f8df 90f0 	ldr.w	r9, [pc, #240]	; 30005a84 <FLASH_CalibrationNew+0x164>
30005996:	9302      	str	r3, [sp, #8]
30005998:	9301      	str	r3, [sp, #4]
3000599a:	e00e      	b.n	300059ba <FLASH_CalibrationNew+0x9a>
3000599c:	9b01      	ldr	r3, [sp, #4]
3000599e:	443d      	add	r5, r7
300059a0:	f8cd a008 	str.w	sl, [sp, #8]
300059a4:	3301      	adds	r3, #1
300059a6:	d102      	bne.n	300059ae <FLASH_CalibrationNew+0x8e>
300059a8:	2501      	movs	r5, #1
300059aa:	f8cd a004 	str.w	sl, [sp, #4]
300059ae:	eb07 030a 	add.w	r3, r7, sl
300059b2:	fa5f fa83 	uxtb.w	sl, r3
300059b6:	45d0      	cmp	r8, sl
300059b8:	d33c      	bcc.n	30005a34 <FLASH_CalibrationNew+0x114>
300059ba:	4650      	mov	r0, sl
300059bc:	f7ff ff64 	bl	30005888 <FLASH_CalibrationPhaseIdx>
300059c0:	6972      	ldr	r2, [r6, #20]
300059c2:	03d3      	lsls	r3, r2, #15
300059c4:	d507      	bpl.n	300059d6 <FLASH_CalibrationNew+0xb6>
300059c6:	f3bf 8f4f 	dsb	sy
300059ca:	f8c6 425c 	str.w	r4, [r6, #604]	; 0x25c
300059ce:	f3bf 8f4f 	dsb	sy
300059d2:	f3bf 8f6f 	isb	sy
300059d6:	47d8      	blx	fp
300059d8:	6821      	ldr	r1, [r4, #0]
300059da:	2208      	movs	r2, #8
300059dc:	a804      	add	r0, sp, #16
300059de:	9104      	str	r1, [sp, #16]
300059e0:	6861      	ldr	r1, [r4, #4]
300059e2:	4b24      	ldr	r3, [pc, #144]	; (30005a74 <FLASH_CalibrationNew+0x154>)
300059e4:	9105      	str	r1, [sp, #20]
300059e6:	4649      	mov	r1, r9
300059e8:	4798      	blx	r3
300059ea:	2800      	cmp	r0, #0
300059ec:	d0d6      	beq.n	3000599c <FLASH_CalibrationNew+0x7c>
300059ee:	2d00      	cmp	r5, #0
300059f0:	dddd      	ble.n	300059ae <FLASH_CalibrationNew+0x8e>
300059f2:	9b02      	ldr	r3, [sp, #8]
300059f4:	9903      	ldr	r1, [sp, #12]
300059f6:	061a      	lsls	r2, r3, #24
300059f8:	9b01      	ldr	r3, [sp, #4]
300059fa:	ea42 4203 	orr.w	r2, r2, r3, lsl #16
300059fe:	eb03 0365 	add.w	r3, r3, r5, asr #1
30005a02:	4315      	orrs	r5, r2
30005a04:	f881 3061 	strb.w	r3, [r1, #97]	; 0x61
30005a08:	f04f 4282 	mov.w	r2, #1090519040	; 0x41000000
30005a0c:	4c16      	ldr	r4, [pc, #88]	; (30005a68 <FLASH_CalibrationNew+0x148>)
30005a0e:	6c13      	ldr	r3, [r2, #64]	; 0x40
30005a10:	f423 4300 	bic.w	r3, r3, #32768	; 0x8000
30005a14:	6413      	str	r3, [r2, #64]	; 0x40
30005a16:	f04f 4388 	mov.w	r3, #1140850688	; 0x44000000
30005a1a:	79a2      	ldrb	r2, [r4, #6]
30005a1c:	615a      	str	r2, [r3, #20]
30005a1e:	4b11      	ldr	r3, [pc, #68]	; (30005a64 <FLASH_CalibrationNew+0x144>)
30005a20:	4798      	blx	r3
30005a22:	b1a0      	cbz	r0, 30005a4e <FLASH_CalibrationNew+0x12e>
30005a24:	4620      	mov	r0, r4
30005a26:	7921      	ldrb	r1, [r4, #4]
30005a28:	4b10      	ldr	r3, [pc, #64]	; (30005a6c <FLASH_CalibrationNew+0x14c>)
30005a2a:	4798      	blx	r3
30005a2c:	4628      	mov	r0, r5
30005a2e:	b007      	add	sp, #28
30005a30:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30005a34:	2d00      	cmp	r5, #0
30005a36:	dcdc      	bgt.n	300059f2 <FLASH_CalibrationNew+0xd2>
30005a38:	9b02      	ldr	r3, [sp, #8]
30005a3a:	9a01      	ldr	r2, [sp, #4]
30005a3c:	061b      	lsls	r3, r3, #24
30005a3e:	ea43 4302 	orr.w	r3, r3, r2, lsl #16
30005a42:	431d      	orrs	r5, r3
30005a44:	e7e0      	b.n	30005a08 <FLASH_CalibrationNew+0xe8>
30005a46:	4808      	ldr	r0, [pc, #32]	; (30005a68 <FLASH_CalibrationNew+0x148>)
30005a48:	4a0b      	ldr	r2, [pc, #44]	; (30005a78 <FLASH_CalibrationNew+0x158>)
30005a4a:	4790      	blx	r2
30005a4c:	e77e      	b.n	3000594c <FLASH_CalibrationNew+0x2c>
30005a4e:	4b06      	ldr	r3, [pc, #24]	; (30005a68 <FLASH_CalibrationNew+0x148>)
30005a50:	7919      	ldrb	r1, [r3, #4]
30005a52:	4618      	mov	r0, r3
30005a54:	4b08      	ldr	r3, [pc, #32]	; (30005a78 <FLASH_CalibrationNew+0x158>)
30005a56:	4798      	blx	r3
30005a58:	4628      	mov	r0, r5
30005a5a:	b007      	add	sp, #28
30005a5c:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30005a60:	4d06      	ldr	r5, [pc, #24]	; (30005a7c <FLASH_CalibrationNew+0x15c>)
30005a62:	e7d1      	b.n	30005a08 <FLASH_CalibrationNew+0xe8>
30005a64:	3000b4ed 	.word	0x3000b4ed
30005a68:	2001c01c 	.word	0x2001c01c
30005a6c:	00009c99 	.word	0x00009c99
30005a70:	e000ed00 	.word	0xe000ed00
30005a74:	000129bd 	.word	0x000129bd
30005a78:	0000b7e9 	.word	0x0000b7e9
30005a7c:	ffff0000 	.word	0xffff0000
30005a80:	3000b379 	.word	0x3000b379
30005a84:	00054060 	.word	0x00054060

30005a88 <FLASH_Calibration>:
30005a88:	f04f 4388 	mov.w	r3, #1140850688	; 0x44000000
30005a8c:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30005a90:	695c      	ldr	r4, [r3, #20]
30005a92:	b087      	sub	sp, #28
30005a94:	4616      	mov	r6, r2
30005a96:	f04f 4282 	mov.w	r2, #1090519040	; 0x41000000
30005a9a:	9403      	str	r4, [sp, #12]
30005a9c:	f8d3 311c 	ldr.w	r3, [r3, #284]	; 0x11c
30005aa0:	9101      	str	r1, [sp, #4]
30005aa2:	9304      	str	r3, [sp, #16]
30005aa4:	9005      	str	r0, [sp, #20]
30005aa6:	6c13      	ldr	r3, [r2, #64]	; 0x40
30005aa8:	7944      	ldrb	r4, [r0, #5]
30005aaa:	f423 4300 	bic.w	r3, r3, #32768	; 0x8000
30005aae:	6413      	str	r3, [r2, #64]	; 0x40
30005ab0:	4b38      	ldr	r3, [pc, #224]	; (30005b94 <FLASH_Calibration+0x10c>)
30005ab2:	4798      	blx	r3
30005ab4:	9901      	ldr	r1, [sp, #4]
30005ab6:	2800      	cmp	r0, #0
30005ab8:	d057      	beq.n	30005b6a <FLASH_Calibration+0xe2>
30005aba:	4837      	ldr	r0, [pc, #220]	; (30005b98 <FLASH_Calibration+0x110>)
30005abc:	f04f 4a88 	mov.w	sl, #1140850688	; 0x44000000
30005ac0:	4b36      	ldr	r3, [pc, #216]	; (30005b9c <FLASH_Calibration+0x114>)
30005ac2:	4798      	blx	r3
30005ac4:	f8ca 4014 	str.w	r4, [sl, #20]
30005ac8:	eb16 0644 	adds.w	r6, r6, r4, lsl #1
30005acc:	d057      	beq.n	30005b7e <FLASH_Calibration+0xf6>
30005ace:	2700      	movs	r7, #0
30005ad0:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
30005ad4:	f8df 80d0 	ldr.w	r8, [pc, #208]	; 30005ba8 <FLASH_Calibration+0x120>
30005ad8:	f04f 6400 	mov.w	r4, #134217728	; 0x8000000
30005adc:	f8df 90cc 	ldr.w	r9, [pc, #204]	; 30005bac <FLASH_Calibration+0x124>
30005ae0:	46bb      	mov	fp, r7
30005ae2:	4d2f      	ldr	r5, [pc, #188]	; (30005ba0 <FLASH_Calibration+0x118>)
30005ae4:	9302      	str	r3, [sp, #8]
30005ae6:	9701      	str	r7, [sp, #4]
30005ae8:	f8da 111c 	ldr.w	r1, [sl, #284]	; 0x11c
30005aec:	ea4f 330b 	mov.w	r3, fp, lsl #12
30005af0:	f421 4170 	bic.w	r1, r1, #61440	; 0xf000
30005af4:	b29b      	uxth	r3, r3
30005af6:	430b      	orrs	r3, r1
30005af8:	f8ca 311c 	str.w	r3, [sl, #284]	; 0x11c
30005afc:	696b      	ldr	r3, [r5, #20]
30005afe:	03db      	lsls	r3, r3, #15
30005b00:	d507      	bpl.n	30005b12 <FLASH_Calibration+0x8a>
30005b02:	f3bf 8f4f 	dsb	sy
30005b06:	f8c5 425c 	str.w	r4, [r5, #604]	; 0x25c
30005b0a:	f3bf 8f4f 	dsb	sy
30005b0e:	f3bf 8f6f 	isb	sy
30005b12:	47c0      	blx	r8
30005b14:	6823      	ldr	r3, [r4, #0]
30005b16:	f8d9 1000 	ldr.w	r1, [r9]
30005b1a:	4299      	cmp	r1, r3
30005b1c:	d108      	bne.n	30005b30 <FLASH_Calibration+0xa8>
30005b1e:	9b02      	ldr	r3, [sp, #8]
30005b20:	3701      	adds	r7, #1
30005b22:	3301      	adds	r3, #1
30005b24:	f04f 0301 	mov.w	r3, #1
30005b28:	bf08      	it	eq
30005b2a:	f8cd b008 	streq.w	fp, [sp, #8]
30005b2e:	9301      	str	r3, [sp, #4]
30005b30:	f10b 0b01 	add.w	fp, fp, #1
30005b34:	45b3      	cmp	fp, r6
30005b36:	d1d7      	bne.n	30005ae8 <FLASH_Calibration+0x60>
30005b38:	2f00      	cmp	r7, #0
30005b3a:	dd04      	ble.n	30005b46 <FLASH_Calibration+0xbe>
30005b3c:	9b02      	ldr	r3, [sp, #8]
30005b3e:	eb03 0767 	add.w	r7, r3, r7, asr #1
30005b42:	9b05      	ldr	r3, [sp, #20]
30005b44:	775f      	strb	r7, [r3, #29]
30005b46:	4b13      	ldr	r3, [pc, #76]	; (30005b94 <FLASH_Calibration+0x10c>)
30005b48:	4798      	blx	r3
30005b4a:	b1e8      	cbz	r0, 30005b88 <FLASH_Calibration+0x100>
30005b4c:	4812      	ldr	r0, [pc, #72]	; (30005b98 <FLASH_Calibration+0x110>)
30005b4e:	4b13      	ldr	r3, [pc, #76]	; (30005b9c <FLASH_Calibration+0x114>)
30005b50:	7901      	ldrb	r1, [r0, #4]
30005b52:	4798      	blx	r3
30005b54:	9a03      	ldr	r2, [sp, #12]
30005b56:	f04f 4388 	mov.w	r3, #1140850688	; 0x44000000
30005b5a:	9801      	ldr	r0, [sp, #4]
30005b5c:	615a      	str	r2, [r3, #20]
30005b5e:	9a04      	ldr	r2, [sp, #16]
30005b60:	f8c3 211c 	str.w	r2, [r3, #284]	; 0x11c
30005b64:	b007      	add	sp, #28
30005b66:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30005b6a:	480b      	ldr	r0, [pc, #44]	; (30005b98 <FLASH_Calibration+0x110>)
30005b6c:	f04f 4a88 	mov.w	sl, #1140850688	; 0x44000000
30005b70:	4b0c      	ldr	r3, [pc, #48]	; (30005ba4 <FLASH_Calibration+0x11c>)
30005b72:	4798      	blx	r3
30005b74:	f8ca 4014 	str.w	r4, [sl, #20]
30005b78:	eb16 0644 	adds.w	r6, r6, r4, lsl #1
30005b7c:	d1a7      	bne.n	30005ace <FLASH_Calibration+0x46>
30005b7e:	4b05      	ldr	r3, [pc, #20]	; (30005b94 <FLASH_Calibration+0x10c>)
30005b80:	9601      	str	r6, [sp, #4]
30005b82:	4798      	blx	r3
30005b84:	2800      	cmp	r0, #0
30005b86:	d1e1      	bne.n	30005b4c <FLASH_Calibration+0xc4>
30005b88:	4803      	ldr	r0, [pc, #12]	; (30005b98 <FLASH_Calibration+0x110>)
30005b8a:	4b06      	ldr	r3, [pc, #24]	; (30005ba4 <FLASH_Calibration+0x11c>)
30005b8c:	7901      	ldrb	r1, [r0, #4]
30005b8e:	4798      	blx	r3
30005b90:	e7e0      	b.n	30005b54 <FLASH_Calibration+0xcc>
30005b92:	bf00      	nop
30005b94:	3000b4ed 	.word	0x3000b4ed
30005b98:	2001c01c 	.word	0x2001c01c
30005b9c:	00009c99 	.word	0x00009c99
30005ba0:	e000ed00 	.word	0xe000ed00
30005ba4:	0000b7e9 	.word	0x0000b7e9
30005ba8:	3000b379 	.word	0x3000b379
30005bac:	00054060 	.word	0x00054060

30005bb0 <_flash_calibration_highspeed>:
30005bb0:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30005bb4:	f04f 0800 	mov.w	r8, #0
30005bb8:	b087      	sub	sp, #28
30005bba:	4683      	mov	fp, r0
30005bbc:	460e      	mov	r6, r1
30005bbe:	4647      	mov	r7, r8
30005bc0:	4644      	mov	r4, r8
30005bc2:	4d28      	ldr	r5, [pc, #160]	; (30005c64 <_flash_calibration_highspeed+0xb4>)
30005bc4:	f04f 0a01 	mov.w	sl, #1
30005bc8:	46c1      	mov	r9, r8
30005bca:	fa5f fc84 	uxtb.w	ip, r4
30005bce:	2301      	movs	r3, #1
30005bd0:	4632      	mov	r2, r6
30005bd2:	4659      	mov	r1, fp
30005bd4:	4628      	mov	r0, r5
30005bd6:	e9cd ca00 	strd	ip, sl, [sp]
30005bda:	f7ff fea1 	bl	30005920 <FLASH_CalibrationNew>
30005bde:	eb04 0384 	add.w	r3, r4, r4, lsl #2
30005be2:	3401      	adds	r4, #1
30005be4:	b2c2      	uxtb	r2, r0
30005be6:	005b      	lsls	r3, r3, #1
30005be8:	b12a      	cbz	r2, 30005bf6 <_flash_calibration_highspeed+0x46>
30005bea:	f3c0 4207 	ubfx	r2, r0, #16, #8
30005bee:	eb03 6810 	add.w	r8, r3, r0, lsr #24
30005bf2:	b907      	cbnz	r7, 30005bf6 <_flash_calibration_highspeed+0x46>
30005bf4:	18d7      	adds	r7, r2, r3
30005bf6:	2c05      	cmp	r4, #5
30005bf8:	f885 9061 	strb.w	r9, [r5, #97]	; 0x61
30005bfc:	d1e5      	bne.n	30005bca <_flash_calibration_highspeed+0x1a>
30005bfe:	b927      	cbnz	r7, 30005c0a <_flash_calibration_highspeed+0x5a>
30005c00:	4638      	mov	r0, r7
30005c02:	776f      	strb	r7, [r5, #29]
30005c04:	b007      	add	sp, #28
30005c06:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30005c0a:	eb07 0208 	add.w	r2, r7, r8
30005c0e:	4b16      	ldr	r3, [pc, #88]	; (30005c68 <_flash_calibration_highspeed+0xb8>)
30005c10:	0852      	lsrs	r2, r2, #1
30005c12:	fba3 1602 	umull	r1, r6, r3, r2
30005c16:	08f6      	lsrs	r6, r6, #3
30005c18:	eb06 0686 	add.w	r6, r6, r6, lsl #2
30005c1c:	ebb2 0646 	subs.w	r6, r2, r6, lsl #1
30005c20:	d107      	bne.n	30005c32 <_flash_calibration_highspeed+0x82>
30005c22:	3201      	adds	r2, #1
30005c24:	fba3 1302 	umull	r1, r3, r3, r2
30005c28:	08de      	lsrs	r6, r3, #3
30005c2a:	eb06 0686 	add.w	r6, r6, r6, lsl #2
30005c2e:	eba2 0646 	sub.w	r6, r2, r6, lsl #1
30005c32:	4c0d      	ldr	r4, [pc, #52]	; (30005c68 <_flash_calibration_highspeed+0xb8>)
30005c34:	2004      	movs	r0, #4
30005c36:	9202      	str	r2, [sp, #8]
30005c38:	fba4 2402 	umull	r2, r4, r4, r2
30005c3c:	4b0b      	ldr	r3, [pc, #44]	; (30005c6c <_flash_calibration_highspeed+0xbc>)
30005c3e:	2249      	movs	r2, #73	; 0x49
30005c40:	490b      	ldr	r1, [pc, #44]	; (30005c70 <_flash_calibration_highspeed+0xc0>)
30005c42:	08e4      	lsrs	r4, r4, #3
30005c44:	f8cd 8004 	str.w	r8, [sp, #4]
30005c48:	9604      	str	r6, [sp, #16]
30005c4a:	9403      	str	r4, [sp, #12]
30005c4c:	b2e4      	uxtb	r4, r4
30005c4e:	9700      	str	r7, [sp, #0]
30005c50:	f004 fb62 	bl	3000a318 <rtk_log_write>
30005c54:	2001      	movs	r0, #1
30005c56:	f885 6061 	strb.w	r6, [r5, #97]	; 0x61
30005c5a:	772c      	strb	r4, [r5, #28]
30005c5c:	776c      	strb	r4, [r5, #29]
30005c5e:	b007      	add	sp, #28
30005c60:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30005c64:	2001c01c 	.word	0x2001c01c
30005c68:	cccccccd 	.word	0xcccccccd
30005c6c:	3000d3c0 	.word	0x3000d3c0
30005c70:	3000d610 	.word	0x3000d610

30005c74 <flash_calibration_highspeed>:
30005c74:	b570      	push	{r4, r5, r6, lr}
30005c76:	4d1c      	ldr	r5, [pc, #112]	; (30005ce8 <flash_calibration_highspeed+0x74>)
30005c78:	2300      	movs	r3, #0
30005c7a:	4604      	mov	r4, r0
30005c7c:	b082      	sub	sp, #8
30005c7e:	792e      	ldrb	r6, [r5, #4]
30005c80:	f885 3060 	strb.w	r3, [r5, #96]	; 0x60
30005c84:	f7ff fdb8 	bl	300057f8 <FLASH_ClockDiv>
30005c88:	4621      	mov	r1, r4
30005c8a:	4630      	mov	r0, r6
30005c8c:	f7ff ff90 	bl	30005bb0 <_flash_calibration_highspeed>
30005c90:	b1e8      	cbz	r0, 30005cce <flash_calibration_highspeed+0x5a>
30005c92:	f04f 4282 	mov.w	r2, #1090519040	; 0x41000000
30005c96:	f895 0061 	ldrb.w	r0, [r5, #97]	; 0x61
30005c9a:	6c13      	ldr	r3, [r2, #64]	; 0x40
30005c9c:	f443 4300 	orr.w	r3, r3, #32768	; 0x8000
30005ca0:	6413      	str	r3, [r2, #64]	; 0x40
30005ca2:	f7ff fdf1 	bl	30005888 <FLASH_CalibrationPhaseIdx>
30005ca6:	4b11      	ldr	r3, [pc, #68]	; (30005cec <flash_calibration_highspeed+0x78>)
30005ca8:	4798      	blx	r3
30005caa:	b960      	cbnz	r0, 30005cc6 <flash_calibration_highspeed+0x52>
30005cac:	4630      	mov	r0, r6
30005cae:	4b10      	ldr	r3, [pc, #64]	; (30005cf0 <flash_calibration_highspeed+0x7c>)
30005cb0:	4798      	blx	r3
30005cb2:	2004      	movs	r0, #4
30005cb4:	4b0f      	ldr	r3, [pc, #60]	; (30005cf4 <flash_calibration_highspeed+0x80>)
30005cb6:	2249      	movs	r2, #73	; 0x49
30005cb8:	490f      	ldr	r1, [pc, #60]	; (30005cf8 <flash_calibration_highspeed+0x84>)
30005cba:	9400      	str	r4, [sp, #0]
30005cbc:	f004 fb2c 	bl	3000a318 <rtk_log_write>
30005cc0:	2000      	movs	r0, #0
30005cc2:	b002      	add	sp, #8
30005cc4:	bd70      	pop	{r4, r5, r6, pc}
30005cc6:	4630      	mov	r0, r6
30005cc8:	4b0c      	ldr	r3, [pc, #48]	; (30005cfc <flash_calibration_highspeed+0x88>)
30005cca:	4798      	blx	r3
30005ccc:	e7f1      	b.n	30005cb2 <flash_calibration_highspeed+0x3e>
30005cce:	2001      	movs	r0, #1
30005cd0:	4b0b      	ldr	r3, [pc, #44]	; (30005d00 <flash_calibration_highspeed+0x8c>)
30005cd2:	4798      	blx	r3
30005cd4:	2002      	movs	r0, #2
30005cd6:	4b0b      	ldr	r3, [pc, #44]	; (30005d04 <flash_calibration_highspeed+0x90>)
30005cd8:	2245      	movs	r2, #69	; 0x45
30005cda:	4907      	ldr	r1, [pc, #28]	; (30005cf8 <flash_calibration_highspeed+0x84>)
30005cdc:	9400      	str	r4, [sp, #0]
30005cde:	f004 fb1b 	bl	3000a318 <rtk_log_write>
30005ce2:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
30005ce6:	e7ec      	b.n	30005cc2 <flash_calibration_highspeed+0x4e>
30005ce8:	2001c01c 	.word	0x2001c01c
30005cec:	3000b4ed 	.word	0x3000b4ed
30005cf0:	0000c045 	.word	0x0000c045
30005cf4:	3000d414 	.word	0x3000d414
30005cf8:	3000d610 	.word	0x3000d610
30005cfc:	0000a53d 	.word	0x0000a53d
30005d00:	0000b569 	.word	0x0000b569
30005d04:	3000d42c 	.word	0x3000d42c

30005d08 <flash_rx_mode_switch>:
30005d08:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30005d0c:	2300      	movs	r3, #0
30005d0e:	b08b      	sub	sp, #44	; 0x2c
30005d10:	4d51      	ldr	r5, [pc, #324]	; (30005e58 <flash_rx_mode_switch+0x150>)
30005d12:	4606      	mov	r6, r0
30005d14:	ac05      	add	r4, sp, #20
30005d16:	f88d 300b 	strb.w	r3, [sp, #11]
30005d1a:	2e04      	cmp	r6, #4
30005d1c:	cd0f      	ldmia	r5!, {r0, r1, r2, r3}
30005d1e:	c40f      	stmia	r4!, {r0, r1, r2, r3}
30005d20:	682b      	ldr	r3, [r5, #0]
30005d22:	6023      	str	r3, [r4, #0]
30005d24:	d876      	bhi.n	30005e14 <flash_rx_mode_switch+0x10c>
30005d26:	b270      	sxtb	r0, r6
30005d28:	ab05      	add	r3, sp, #20
30005d2a:	4f4c      	ldr	r7, [pc, #304]	; (30005e5c <flash_rx_mode_switch+0x154>)
30005d2c:	f1c0 0404 	rsb	r4, r0, #4
30005d30:	f1c0 0022 	rsb	r0, r0, #34	; 0x22
30005d34:	f8df b150 	ldr.w	fp, [pc, #336]	; 30005e88 <flash_rx_mode_switch+0x180>
30005d38:	fa4f f884 	sxtb.w	r8, r4
30005d3c:	fa47 f580 	sxtab	r5, r7, r0
30005d40:	b2e4      	uxtb	r4, r4
30005d42:	f8df a148 	ldr.w	sl, [pc, #328]	; 30005e8c <flash_rx_mode_switch+0x184>
30005d46:	eb03 0888 	add.w	r8, r3, r8, lsl #2
30005d4a:	f8df 9144 	ldr.w	r9, [pc, #324]	; 30005e90 <flash_rx_mode_switch+0x188>
30005d4e:	e033      	b.n	30005db8 <flash_rx_mode_switch+0xb0>
30005d50:	2c04      	cmp	r4, #4
30005d52:	d039      	beq.n	30005dc8 <flash_rx_mode_switch+0xc0>
30005d54:	2c02      	cmp	r4, #2
30005d56:	d037      	beq.n	30005dc8 <flash_rx_mode_switch+0xc0>
30005d58:	683b      	ldr	r3, [r7, #0]
30005d5a:	2b03      	cmp	r3, #3
30005d5c:	d03a      	beq.n	30005dd4 <flash_rx_mode_switch+0xcc>
30005d5e:	2300      	movs	r3, #0
30005d60:	4620      	mov	r0, r4
30005d62:	777b      	strb	r3, [r7, #29]
30005d64:	4b3e      	ldr	r3, [pc, #248]	; (30005e60 <flash_rx_mode_switch+0x158>)
30005d66:	4798      	blx	r3
30005d68:	4b3e      	ldr	r3, [pc, #248]	; (30005e64 <flash_rx_mode_switch+0x15c>)
30005d6a:	695a      	ldr	r2, [r3, #20]
30005d6c:	03d2      	lsls	r2, r2, #15
30005d6e:	d509      	bpl.n	30005d84 <flash_rx_mode_switch+0x7c>
30005d70:	f3bf 8f4f 	dsb	sy
30005d74:	f04f 6200 	mov.w	r2, #134217728	; 0x8000000
30005d78:	f8c3 225c 	str.w	r2, [r3, #604]	; 0x25c
30005d7c:	f3bf 8f4f 	dsb	sy
30005d80:	f3bf 8f6f 	isb	sy
30005d84:	47d0      	blx	sl
30005d86:	f04f 6300 	mov.w	r3, #134217728	; 0x8000000
30005d8a:	4937      	ldr	r1, [pc, #220]	; (30005e68 <flash_rx_mode_switch+0x160>)
30005d8c:	a803      	add	r0, sp, #12
30005d8e:	681a      	ldr	r2, [r3, #0]
30005d90:	9203      	str	r2, [sp, #12]
30005d92:	2208      	movs	r2, #8
30005d94:	685b      	ldr	r3, [r3, #4]
30005d96:	9304      	str	r3, [sp, #16]
30005d98:	47c8      	blx	r9
30005d9a:	2800      	cmp	r0, #0
30005d9c:	d041      	beq.n	30005e22 <flash_rx_mode_switch+0x11a>
30005d9e:	f897 3060 	ldrb.w	r3, [r7, #96]	; 0x60
30005da2:	bb6b      	cbnz	r3, 30005e00 <flash_rx_mode_switch+0xf8>
30005da4:	1e63      	subs	r3, r4, #1
30005da6:	f1c4 0605 	rsb	r6, r4, #5
30005daa:	f1a8 0804 	sub.w	r8, r8, #4
30005dae:	3d01      	subs	r5, #1
30005db0:	b2dc      	uxtb	r4, r3
30005db2:	b2f6      	uxtb	r6, r6
30005db4:	2cff      	cmp	r4, #255	; 0xff
30005db6:	d02d      	beq.n	30005e14 <flash_rx_mode_switch+0x10c>
30005db8:	47d8      	blx	fp
30005dba:	2800      	cmp	r0, #0
30005dbc:	d1c8      	bne.n	30005d50 <flash_rx_mode_switch+0x48>
30005dbe:	7778      	strb	r0, [r7, #29]
30005dc0:	4620      	mov	r0, r4
30005dc2:	4b2a      	ldr	r3, [pc, #168]	; (30005e6c <flash_rx_mode_switch+0x164>)
30005dc4:	4798      	blx	r3
30005dc6:	e7cf      	b.n	30005d68 <flash_rx_mode_switch+0x60>
30005dc8:	2301      	movs	r3, #1
30005dca:	f887 3063 	strb.w	r3, [r7, #99]	; 0x63
30005dce:	683b      	ldr	r3, [r7, #0]
30005dd0:	2b03      	cmp	r3, #3
30005dd2:	d1c4      	bne.n	30005d5e <flash_rx_mode_switch+0x56>
30005dd4:	f10d 020b 	add.w	r2, sp, #11
30005dd8:	2101      	movs	r1, #1
30005dda:	2085      	movs	r0, #133	; 0x85
30005ddc:	4b24      	ldr	r3, [pc, #144]	; (30005e70 <flash_rx_mode_switch+0x168>)
30005dde:	4798      	blx	r3
30005de0:	f89d 300b 	ldrb.w	r3, [sp, #11]
30005de4:	7829      	ldrb	r1, [r5, #0]
30005de6:	f10d 020b 	add.w	r2, sp, #11
30005dea:	f003 030f 	and.w	r3, r3, #15
30005dee:	2081      	movs	r0, #129	; 0x81
30005df0:	ea43 1301 	orr.w	r3, r3, r1, lsl #4
30005df4:	2101      	movs	r1, #1
30005df6:	f88d 300b 	strb.w	r3, [sp, #11]
30005dfa:	4b1e      	ldr	r3, [pc, #120]	; (30005e74 <flash_rx_mode_switch+0x16c>)
30005dfc:	4798      	blx	r3
30005dfe:	e7ae      	b.n	30005d5e <flash_rx_mode_switch+0x56>
30005e00:	f8d8 3000 	ldr.w	r3, [r8]
30005e04:	2249      	movs	r2, #73	; 0x49
30005e06:	491c      	ldr	r1, [pc, #112]	; (30005e78 <flash_rx_mode_switch+0x170>)
30005e08:	2004      	movs	r0, #4
30005e0a:	9300      	str	r3, [sp, #0]
30005e0c:	4b1b      	ldr	r3, [pc, #108]	; (30005e7c <flash_rx_mode_switch+0x174>)
30005e0e:	f004 fa83 	bl	3000a318 <rtk_log_write>
30005e12:	e7c7      	b.n	30005da4 <flash_rx_mode_switch+0x9c>
30005e14:	2e05      	cmp	r6, #5
30005e16:	d015      	beq.n	30005e44 <flash_rx_mode_switch+0x13c>
30005e18:	2600      	movs	r6, #0
30005e1a:	4630      	mov	r0, r6
30005e1c:	b00b      	add	sp, #44	; 0x2c
30005e1e:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30005e22:	ab0a      	add	r3, sp, #40	; 0x28
30005e24:	4606      	mov	r6, r0
30005e26:	2249      	movs	r2, #73	; 0x49
30005e28:	4913      	ldr	r1, [pc, #76]	; (30005e78 <flash_rx_mode_switch+0x170>)
30005e2a:	eb03 0484 	add.w	r4, r3, r4, lsl #2
30005e2e:	4b14      	ldr	r3, [pc, #80]	; (30005e80 <flash_rx_mode_switch+0x178>)
30005e30:	f854 0c14 	ldr.w	r0, [r4, #-20]
30005e34:	9000      	str	r0, [sp, #0]
30005e36:	2004      	movs	r0, #4
30005e38:	f004 fa6e 	bl	3000a318 <rtk_log_write>
30005e3c:	4630      	mov	r0, r6
30005e3e:	b00b      	add	sp, #44	; 0x2c
30005e40:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30005e44:	4b0f      	ldr	r3, [pc, #60]	; (30005e84 <flash_rx_mode_switch+0x17c>)
30005e46:	2245      	movs	r2, #69	; 0x45
30005e48:	490b      	ldr	r1, [pc, #44]	; (30005e78 <flash_rx_mode_switch+0x170>)
30005e4a:	2002      	movs	r0, #2
30005e4c:	f04f 36ff 	mov.w	r6, #4294967295	; 0xffffffff
30005e50:	f004 fa62 	bl	3000a318 <rtk_log_write>
30005e54:	e7e1      	b.n	30005e1a <flash_rx_mode_switch+0x112>
30005e56:	bf00      	nop
30005e58:	3000d5e4 	.word	0x3000d5e4
30005e5c:	2001c01c 	.word	0x2001c01c
30005e60:	0000a53d 	.word	0x0000a53d
30005e64:	e000ed00 	.word	0xe000ed00
30005e68:	00054060 	.word	0x00054060
30005e6c:	0000c045 	.word	0x0000c045
30005e70:	30006395 	.word	0x30006395
30005e74:	30006841 	.word	0x30006841
30005e78:	3000d610 	.word	0x3000d610
30005e7c:	3000d460 	.word	0x3000d460
30005e80:	3000d450 	.word	0x3000d450
30005e84:	3000d478 	.word	0x3000d478
30005e88:	3000b4ed 	.word	0x3000b4ed
30005e8c:	3000b379 	.word	0x3000b379
30005e90:	000129bd 	.word	0x000129bd

30005e94 <flash_highspeed_setup>:
30005e94:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
30005e98:	4b8b      	ldr	r3, [pc, #556]	; (300060c8 <flash_highspeed_setup+0x234>)
30005e9a:	b087      	sub	sp, #28
30005e9c:	4798      	blx	r3
30005e9e:	4a8b      	ldr	r2, [pc, #556]	; (300060cc <flash_highspeed_setup+0x238>)
30005ea0:	2400      	movs	r4, #0
30005ea2:	f44f 4300 	mov.w	r3, #32768	; 0x8000
30005ea6:	8812      	ldrh	r2, [r2, #0]
30005ea8:	421a      	tst	r2, r3
30005eaa:	d104      	bne.n	30005eb6 <flash_highspeed_setup+0x22>
30005eac:	3401      	adds	r4, #1
30005eae:	085b      	lsrs	r3, r3, #1
30005eb0:	b2e4      	uxtb	r4, r4
30005eb2:	2c10      	cmp	r4, #16
30005eb4:	d1f8      	bne.n	30005ea8 <flash_highspeed_setup+0x14>
30005eb6:	4986      	ldr	r1, [pc, #536]	; (300060d0 <flash_highspeed_setup+0x23c>)
30005eb8:	2300      	movs	r3, #0
30005eba:	f44f 4200 	mov.w	r2, #32768	; 0x8000
30005ebe:	8809      	ldrh	r1, [r1, #0]
30005ec0:	4211      	tst	r1, r2
30005ec2:	d104      	bne.n	30005ece <flash_highspeed_setup+0x3a>
30005ec4:	3301      	adds	r3, #1
30005ec6:	0852      	lsrs	r2, r2, #1
30005ec8:	b2db      	uxtb	r3, r3
30005eca:	2b10      	cmp	r3, #16
30005ecc:	d1f8      	bne.n	30005ec0 <flash_highspeed_setup+0x2c>
30005ece:	2b06      	cmp	r3, #6
30005ed0:	4980      	ldr	r1, [pc, #512]	; (300060d4 <flash_highspeed_setup+0x240>)
30005ed2:	4d81      	ldr	r5, [pc, #516]	; (300060d8 <flash_highspeed_setup+0x244>)
30005ed4:	bf28      	it	cs
30005ed6:	2306      	movcs	r3, #6
30005ed8:	4401      	add	r1, r0
30005eda:	f1c3 0209 	rsb	r2, r3, #9
30005ede:	fba5 3501 	umull	r3, r5, r5, r1
30005ee2:	4b7e      	ldr	r3, [pc, #504]	; (300060dc <flash_highspeed_setup+0x248>)
30005ee4:	0eed      	lsrs	r5, r5, #27
30005ee6:	497e      	ldr	r1, [pc, #504]	; (300060e0 <flash_highspeed_setup+0x24c>)
30005ee8:	b2d2      	uxtb	r2, r2
30005eea:	3d01      	subs	r5, #1
30005eec:	b2ed      	uxtb	r5, r5
30005eee:	4295      	cmp	r5, r2
30005ef0:	bf38      	it	cc
30005ef2:	4615      	movcc	r5, r2
30005ef4:	1c6a      	adds	r2, r5, #1
30005ef6:	fbb0 f0f2 	udiv	r0, r0, r2
30005efa:	2249      	movs	r2, #73	; 0x49
30005efc:	9000      	str	r0, [sp, #0]
30005efe:	2004      	movs	r0, #4
30005f00:	f004 fa0a 	bl	3000a318 <rtk_log_write>
30005f04:	b672      	cpsid	i
30005f06:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30005f0a:	f8df 920c 	ldr.w	r9, [pc, #524]	; 30006118 <flash_highspeed_setup+0x284>
30005f0e:	4e75      	ldr	r6, [pc, #468]	; (300060e4 <flash_highspeed_setup+0x250>)
30005f10:	6813      	ldr	r3, [r2, #0]
30005f12:	f443 6300 	orr.w	r3, r3, #2048	; 0x800
30005f16:	6013      	str	r3, [r2, #0]
30005f18:	f8d2 3138 	ldr.w	r3, [r2, #312]	; 0x138
30005f1c:	f023 033f 	bic.w	r3, r3, #63	; 0x3f
30005f20:	f043 030c 	orr.w	r3, r3, #12
30005f24:	f8c2 3138 	str.w	r3, [r2, #312]	; 0x138
30005f28:	47c8      	blx	r9
30005f2a:	2800      	cmp	r0, #0
30005f2c:	d05c      	beq.n	30005fe8 <flash_highspeed_setup+0x154>
30005f2e:	2103      	movs	r1, #3
30005f30:	aa05      	add	r2, sp, #20
30005f32:	f896 0055 	ldrb.w	r0, [r6, #85]	; 0x55
30005f36:	4f6c      	ldr	r7, [pc, #432]	; (300060e8 <flash_highspeed_setup+0x254>)
30005f38:	47b8      	blx	r7
30005f3a:	f89d 3016 	ldrb.w	r3, [sp, #22]
30005f3e:	2201      	movs	r2, #1
30005f40:	4967      	ldr	r1, [pc, #412]	; (300060e0 <flash_highspeed_setup+0x24c>)
30005f42:	9302      	str	r3, [sp, #8]
30005f44:	3b11      	subs	r3, #17
30005f46:	2004      	movs	r0, #4
30005f48:	fa02 f303 	lsl.w	r3, r2, r3
30005f4c:	2249      	movs	r2, #73	; 0x49
30005f4e:	9303      	str	r3, [sp, #12]
30005f50:	f89d 3015 	ldrb.w	r3, [sp, #21]
30005f54:	9301      	str	r3, [sp, #4]
30005f56:	f89d 3014 	ldrb.w	r3, [sp, #20]
30005f5a:	9300      	str	r3, [sp, #0]
30005f5c:	4b63      	ldr	r3, [pc, #396]	; (300060ec <flash_highspeed_setup+0x258>)
30005f5e:	f004 f9db 	bl	3000a318 <rtk_log_write>
30005f62:	f89d 8015 	ldrb.w	r8, [sp, #21]
30005f66:	f89d 3016 	ldrb.w	r3, [sp, #22]
30005f6a:	ea4f 2808 	mov.w	r8, r8, lsl #8
30005f6e:	ea48 4803 	orr.w	r8, r8, r3, lsl #16
30005f72:	f89d 3014 	ldrb.w	r3, [sp, #20]
30005f76:	ea48 0803 	orr.w	r8, r8, r3
30005f7a:	47c8      	blx	r9
30005f7c:	4a5c      	ldr	r2, [pc, #368]	; (300060f0 <flash_highspeed_setup+0x25c>)
30005f7e:	4b5d      	ldr	r3, [pc, #372]	; (300060f4 <flash_highspeed_setup+0x260>)
30005f80:	2800      	cmp	r0, #0
30005f82:	bf08      	it	eq
30005f84:	4613      	moveq	r3, r2
30005f86:	7a1a      	ldrb	r2, [r3, #8]
30005f88:	2aff      	cmp	r2, #255	; 0xff
30005f8a:	d111      	bne.n	30005fb0 <flash_highspeed_setup+0x11c>
30005f8c:	4b5a      	ldr	r3, [pc, #360]	; (300060f8 <flash_highspeed_setup+0x264>)
30005f8e:	2200      	movs	r2, #0
30005f90:	601a      	str	r2, [r3, #0]
30005f92:	4953      	ldr	r1, [pc, #332]	; (300060e0 <flash_highspeed_setup+0x24c>)
30005f94:	2003      	movs	r0, #3
30005f96:	4b59      	ldr	r3, [pc, #356]	; (300060fc <flash_highspeed_setup+0x268>)
30005f98:	2257      	movs	r2, #87	; 0x57
30005f9a:	f004 f9bd 	bl	3000a318 <rtk_log_write>
30005f9e:	f240 212e 	movw	r1, #558	; 0x22e
30005fa2:	4857      	ldr	r0, [pc, #348]	; (30006100 <flash_highspeed_setup+0x26c>)
30005fa4:	f005 ff9c 	bl	3000bee0 <io_assert_failed>
30005fa8:	7f1a      	ldrb	r2, [r3, #28]
30005faa:	3314      	adds	r3, #20
30005fac:	2aff      	cmp	r2, #255	; 0xff
30005fae:	d0ed      	beq.n	30005f8c <flash_highspeed_setup+0xf8>
30005fb0:	e9d3 1200 	ldrd	r1, r2, [r3]
30005fb4:	ea08 0202 	and.w	r2, r8, r2
30005fb8:	428a      	cmp	r2, r1
30005fba:	d1f5      	bne.n	30005fa8 <flash_highspeed_setup+0x114>
30005fbc:	f8df 8138 	ldr.w	r8, [pc, #312]	; 300060f8 <flash_highspeed_setup+0x264>
30005fc0:	f8c8 3000 	str.w	r3, [r8]
30005fc4:	2b00      	cmp	r3, #0
30005fc6:	d0e4      	beq.n	30005f92 <flash_highspeed_setup+0xfe>
30005fc8:	7a1a      	ldrb	r2, [r3, #8]
30005fca:	2a05      	cmp	r2, #5
30005fcc:	f200 80a6 	bhi.w	3000611c <flash_highspeed_setup+0x288>
30005fd0:	3a01      	subs	r2, #1
30005fd2:	4844      	ldr	r0, [pc, #272]	; (300060e4 <flash_highspeed_setup+0x250>)
30005fd4:	2a04      	cmp	r2, #4
30005fd6:	f200 8152 	bhi.w	3000627e <flash_highspeed_setup+0x3ea>
30005fda:	e8df f012 	tbh	[pc, r2, lsl #1]
30005fde:	0107      	.short	0x0107
30005fe0:	00ff0104 	.word	0x00ff0104
30005fe4:	00f600fc 	.word	0x00f600fc
30005fe8:	ab05      	add	r3, sp, #20
30005fea:	4602      	mov	r2, r0
30005fec:	4601      	mov	r1, r0
30005fee:	4f45      	ldr	r7, [pc, #276]	; (30006104 <flash_highspeed_setup+0x270>)
30005ff0:	9300      	str	r3, [sp, #0]
30005ff2:	2302      	movs	r3, #2
30005ff4:	f896 0055 	ldrb.w	r0, [r6, #85]	; 0x55
30005ff8:	47b8      	blx	r7
30005ffa:	f89d 3015 	ldrb.w	r3, [sp, #21]
30005ffe:	2249      	movs	r2, #73	; 0x49
30006000:	4937      	ldr	r1, [pc, #220]	; (300060e0 <flash_highspeed_setup+0x24c>)
30006002:	9301      	str	r3, [sp, #4]
30006004:	2004      	movs	r0, #4
30006006:	f89d 3014 	ldrb.w	r3, [sp, #20]
3000600a:	9300      	str	r3, [sp, #0]
3000600c:	4b3e      	ldr	r3, [pc, #248]	; (30006108 <flash_highspeed_setup+0x274>)
3000600e:	f004 f983 	bl	3000a318 <rtk_log_write>
30006012:	f89d 7015 	ldrb.w	r7, [sp, #21]
30006016:	f89d 3016 	ldrb.w	r3, [sp, #22]
3000601a:	023f      	lsls	r7, r7, #8
3000601c:	ea47 4703 	orr.w	r7, r7, r3, lsl #16
30006020:	f89d 3014 	ldrb.w	r3, [sp, #20]
30006024:	431f      	orrs	r7, r3
30006026:	47c8      	blx	r9
30006028:	4a31      	ldr	r2, [pc, #196]	; (300060f0 <flash_highspeed_setup+0x25c>)
3000602a:	4b32      	ldr	r3, [pc, #200]	; (300060f4 <flash_highspeed_setup+0x260>)
3000602c:	2800      	cmp	r0, #0
3000602e:	bf08      	it	eq
30006030:	4613      	moveq	r3, r2
30006032:	7a1a      	ldrb	r2, [r3, #8]
30006034:	2aff      	cmp	r2, #255	; 0xff
30006036:	d111      	bne.n	3000605c <flash_highspeed_setup+0x1c8>
30006038:	4b2f      	ldr	r3, [pc, #188]	; (300060f8 <flash_highspeed_setup+0x264>)
3000603a:	2200      	movs	r2, #0
3000603c:	601a      	str	r2, [r3, #0]
3000603e:	4928      	ldr	r1, [pc, #160]	; (300060e0 <flash_highspeed_setup+0x24c>)
30006040:	2003      	movs	r0, #3
30006042:	4b2e      	ldr	r3, [pc, #184]	; (300060fc <flash_highspeed_setup+0x268>)
30006044:	2257      	movs	r2, #87	; 0x57
30006046:	f004 f967 	bl	3000a318 <rtk_log_write>
3000604a:	f240 216e 	movw	r1, #622	; 0x26e
3000604e:	482c      	ldr	r0, [pc, #176]	; (30006100 <flash_highspeed_setup+0x26c>)
30006050:	f005 ff46 	bl	3000bee0 <io_assert_failed>
30006054:	7f1a      	ldrb	r2, [r3, #28]
30006056:	3314      	adds	r3, #20
30006058:	2aff      	cmp	r2, #255	; 0xff
3000605a:	d0ed      	beq.n	30006038 <flash_highspeed_setup+0x1a4>
3000605c:	e9d3 1200 	ldrd	r1, r2, [r3]
30006060:	403a      	ands	r2, r7
30006062:	428a      	cmp	r2, r1
30006064:	d1f6      	bne.n	30006054 <flash_highspeed_setup+0x1c0>
30006066:	4a24      	ldr	r2, [pc, #144]	; (300060f8 <flash_highspeed_setup+0x264>)
30006068:	6013      	str	r3, [r2, #0]
3000606a:	2b00      	cmp	r3, #0
3000606c:	d0e7      	beq.n	3000603e <flash_highspeed_setup+0x1aa>
3000606e:	7a1a      	ldrb	r2, [r3, #8]
30006070:	2a04      	cmp	r2, #4
30006072:	d821      	bhi.n	300060b8 <flash_highspeed_setup+0x224>
30006074:	2a04      	cmp	r2, #4
30006076:	d807      	bhi.n	30006088 <flash_highspeed_setup+0x1f4>
30006078:	e8df f002 	tbb	[pc, r2]
3000607c:	069fa303 	.word	0x069fa303
30006080:	9b          	.byte	0x9b
30006081:	00          	.byte	0x00
30006082:	4818      	ldr	r0, [pc, #96]	; (300060e4 <flash_highspeed_setup+0x250>)
30006084:	4b21      	ldr	r3, [pc, #132]	; (3000610c <flash_highspeed_setup+0x278>)
30006086:	4798      	blx	r3
30006088:	68f3      	ldr	r3, [r6, #12]
3000608a:	2b00      	cmp	r3, #0
3000608c:	f040 80c0 	bne.w	30006210 <flash_highspeed_setup+0x37c>
30006090:	4e1f      	ldr	r6, [pc, #124]	; (30006110 <flash_highspeed_setup+0x27c>)
30006092:	20a0      	movs	r0, #160	; 0xa0
30006094:	47b0      	blx	r6
30006096:	2800      	cmp	r0, #0
30006098:	f040 80b2 	bne.w	30006200 <flash_highspeed_setup+0x36c>
3000609c:	4620      	mov	r0, r4
3000609e:	f7ff fe33 	bl	30005d08 <flash_rx_mode_switch>
300060a2:	4b1c      	ldr	r3, [pc, #112]	; (30006114 <flash_highspeed_setup+0x280>)
300060a4:	4798      	blx	r3
300060a6:	2801      	cmp	r0, #1
300060a8:	d002      	beq.n	300060b0 <flash_highspeed_setup+0x21c>
300060aa:	4628      	mov	r0, r5
300060ac:	f7ff fde2 	bl	30005c74 <flash_calibration_highspeed>
300060b0:	b662      	cpsie	i
300060b2:	b007      	add	sp, #28
300060b4:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
300060b8:	2afe      	cmp	r2, #254	; 0xfe
300060ba:	d1e5      	bne.n	30006088 <flash_highspeed_setup+0x1f4>
300060bc:	691b      	ldr	r3, [r3, #16]
300060be:	2b00      	cmp	r3, #0
300060c0:	f000 80d3 	beq.w	3000626a <flash_highspeed_setup+0x3d6>
300060c4:	4798      	blx	r3
300060c6:	e7df      	b.n	30006088 <flash_highspeed_setup+0x1f4>
300060c8:	3000750d 	.word	0x3000750d
300060cc:	3000d914 	.word	0x3000d914
300060d0:	3000d916 	.word	0x3000d916
300060d4:	0c65d3ff 	.word	0x0c65d3ff
300060d8:	a530e8a5 	.word	0xa530e8a5
300060dc:	3000d498 	.word	0x3000d498
300060e0:	3000d610 	.word	0x3000d610
300060e4:	2001c01c 	.word	0x2001c01c
300060e8:	30006395 	.word	0x30006395
300060ec:	3000d4ac 	.word	0x3000d4ac
300060f0:	3000d918 	.word	0x3000d918
300060f4:	3000d7fc 	.word	0x3000d7fc
300060f8:	3000f924 	.word	0x3000f924
300060fc:	3000d4d4 	.word	0x3000d4d4
30006100:	3000d4f8 	.word	0x3000d4f8
30006104:	0000b8c1 	.word	0x0000b8c1
30006108:	3000d598 	.word	0x3000d598
3000610c:	0000bfcd 	.word	0x0000bfcd
30006110:	0000b991 	.word	0x0000b991
30006114:	0000c0f9 	.word	0x0000c0f9
30006118:	3000b4ed 	.word	0x3000b4ed
3000611c:	2afe      	cmp	r2, #254	; 0xfe
3000611e:	d104      	bne.n	3000612a <flash_highspeed_setup+0x296>
30006120:	691b      	ldr	r3, [r3, #16]
30006122:	2b00      	cmp	r3, #0
30006124:	f000 80a6 	beq.w	30006274 <flash_highspeed_setup+0x3e0>
30006128:	4798      	blx	r3
3000612a:	4b56      	ldr	r3, [pc, #344]	; (30006284 <flash_highspeed_setup+0x3f0>)
3000612c:	4798      	blx	r3
3000612e:	b110      	cbz	r0, 30006136 <flash_highspeed_setup+0x2a2>
30006130:	2304      	movs	r3, #4
30006132:	f886 3062 	strb.w	r3, [r6, #98]	; 0x62
30006136:	68f2      	ldr	r2, [r6, #12]
30006138:	2101      	movs	r1, #1
3000613a:	f8d8 3000 	ldr.w	r3, [r8]
3000613e:	9204      	str	r2, [sp, #16]
30006140:	2200      	movs	r2, #0
30006142:	f896 0056 	ldrb.w	r0, [r6, #86]	; 0x56
30006146:	9205      	str	r2, [sp, #20]
30006148:	aa05      	add	r2, sp, #20
3000614a:	f8d3 800c 	ldr.w	r8, [r3, #12]
3000614e:	47b8      	blx	r7
30006150:	69b3      	ldr	r3, [r6, #24]
30006152:	2b00      	cmp	r3, #0
30006154:	d169      	bne.n	3000622a <flash_highspeed_setup+0x396>
30006156:	4b4c      	ldr	r3, [pc, #304]	; (30006288 <flash_highspeed_setup+0x3f4>)
30006158:	781b      	ldrb	r3, [r3, #0]
3000615a:	2b00      	cmp	r3, #0
3000615c:	d16d      	bne.n	3000623a <flash_highspeed_setup+0x3a6>
3000615e:	2701      	movs	r7, #1
30006160:	9b05      	ldr	r3, [sp, #20]
30006162:	a805      	add	r0, sp, #20
30006164:	463a      	mov	r2, r7
30006166:	a904      	add	r1, sp, #16
30006168:	ea03 0308 	and.w	r3, r3, r8
3000616c:	9305      	str	r3, [sp, #20]
3000616e:	4b47      	ldr	r3, [pc, #284]	; (3000628c <flash_highspeed_setup+0x3f8>)
30006170:	4798      	blx	r3
30006172:	2800      	cmp	r0, #0
30006174:	d092      	beq.n	3000609c <flash_highspeed_setup+0x208>
30006176:	f896 3059 	ldrb.w	r3, [r6, #89]	; 0x59
3000617a:	aa04      	add	r2, sp, #16
3000617c:	f896 0058 	ldrb.w	r0, [r6, #88]	; 0x58
30006180:	b19b      	cbz	r3, 300061aa <flash_highspeed_setup+0x316>
30006182:	2101      	movs	r1, #1
30006184:	4f42      	ldr	r7, [pc, #264]	; (30006290 <flash_highspeed_setup+0x3fc>)
30006186:	47b8      	blx	r7
30006188:	f10d 0211 	add.w	r2, sp, #17
3000618c:	f896 0059 	ldrb.w	r0, [r6, #89]	; 0x59
30006190:	2101      	movs	r1, #1
30006192:	47b8      	blx	r7
30006194:	4b3f      	ldr	r3, [pc, #252]	; (30006294 <flash_highspeed_setup+0x400>)
30006196:	e9dd 1204 	ldrd	r1, r2, [sp, #16]
3000619a:	e9cd 2100 	strd	r2, r1, [sp]
3000619e:	2004      	movs	r0, #4
300061a0:	2249      	movs	r2, #73	; 0x49
300061a2:	493d      	ldr	r1, [pc, #244]	; (30006298 <flash_highspeed_setup+0x404>)
300061a4:	f004 f8b8 	bl	3000a318 <rtk_log_write>
300061a8:	e778      	b.n	3000609c <flash_highspeed_setup+0x208>
300061aa:	4639      	mov	r1, r7
300061ac:	4b38      	ldr	r3, [pc, #224]	; (30006290 <flash_highspeed_setup+0x3fc>)
300061ae:	4798      	blx	r3
300061b0:	e7f0      	b.n	30006194 <flash_highspeed_setup+0x300>
300061b2:	483a      	ldr	r0, [pc, #232]	; (3000629c <flash_highspeed_setup+0x408>)
300061b4:	4b3a      	ldr	r3, [pc, #232]	; (300062a0 <flash_highspeed_setup+0x40c>)
300061b6:	4798      	blx	r3
300061b8:	e766      	b.n	30006088 <flash_highspeed_setup+0x1f4>
300061ba:	4838      	ldr	r0, [pc, #224]	; (3000629c <flash_highspeed_setup+0x408>)
300061bc:	4b39      	ldr	r3, [pc, #228]	; (300062a4 <flash_highspeed_setup+0x410>)
300061be:	4798      	blx	r3
300061c0:	e762      	b.n	30006088 <flash_highspeed_setup+0x1f4>
300061c2:	4836      	ldr	r0, [pc, #216]	; (3000629c <flash_highspeed_setup+0x408>)
300061c4:	4b38      	ldr	r3, [pc, #224]	; (300062a8 <flash_highspeed_setup+0x414>)
300061c6:	4798      	blx	r3
300061c8:	e75e      	b.n	30006088 <flash_highspeed_setup+0x1f4>
300061ca:	4b38      	ldr	r3, [pc, #224]	; (300062ac <flash_highspeed_setup+0x418>)
300061cc:	4798      	blx	r3
300061ce:	4b2e      	ldr	r3, [pc, #184]	; (30006288 <flash_highspeed_setup+0x3f4>)
300061d0:	2201      	movs	r2, #1
300061d2:	701a      	strb	r2, [r3, #0]
300061d4:	e7a9      	b.n	3000612a <flash_highspeed_setup+0x296>
300061d6:	4b36      	ldr	r3, [pc, #216]	; (300062b0 <flash_highspeed_setup+0x41c>)
300061d8:	4798      	blx	r3
300061da:	e7a6      	b.n	3000612a <flash_highspeed_setup+0x296>
300061dc:	4b33      	ldr	r3, [pc, #204]	; (300062ac <flash_highspeed_setup+0x418>)
300061de:	4798      	blx	r3
300061e0:	2300      	movs	r3, #0
300061e2:	60f3      	str	r3, [r6, #12]
300061e4:	e7a1      	b.n	3000612a <flash_highspeed_setup+0x296>
300061e6:	4b31      	ldr	r3, [pc, #196]	; (300062ac <flash_highspeed_setup+0x418>)
300061e8:	4798      	blx	r3
300061ea:	e79e      	b.n	3000612a <flash_highspeed_setup+0x296>
300061ec:	4b31      	ldr	r3, [pc, #196]	; (300062b4 <flash_highspeed_setup+0x420>)
300061ee:	4798      	blx	r3
300061f0:	f89d 3014 	ldrb.w	r3, [sp, #20]
300061f4:	2bc8      	cmp	r3, #200	; 0xc8
300061f6:	d02b      	beq.n	30006250 <flash_highspeed_setup+0x3bc>
300061f8:	2331      	movs	r3, #49	; 0x31
300061fa:	f886 3059 	strb.w	r3, [r6, #89]	; 0x59
300061fe:	e794      	b.n	3000612a <flash_highspeed_setup+0x296>
30006200:	2100      	movs	r1, #0
30006202:	4b2d      	ldr	r3, [pc, #180]	; (300062b8 <flash_highspeed_setup+0x424>)
30006204:	20a0      	movs	r0, #160	; 0xa0
30006206:	4798      	blx	r3
30006208:	2100      	movs	r1, #0
3000620a:	22a0      	movs	r2, #160	; 0xa0
3000620c:	4b2b      	ldr	r3, [pc, #172]	; (300062bc <flash_highspeed_setup+0x428>)
3000620e:	e7c4      	b.n	3000619a <flash_highspeed_setup+0x306>
30006210:	20b0      	movs	r0, #176	; 0xb0
30006212:	4e2b      	ldr	r6, [pc, #172]	; (300062c0 <flash_highspeed_setup+0x42c>)
30006214:	47b0      	blx	r6
30006216:	f040 0101 	orr.w	r1, r0, #1
3000621a:	b2c9      	uxtb	r1, r1
3000621c:	4281      	cmp	r1, r0
3000621e:	f43f af38 	beq.w	30006092 <flash_highspeed_setup+0x1fe>
30006222:	20b0      	movs	r0, #176	; 0xb0
30006224:	4b24      	ldr	r3, [pc, #144]	; (300062b8 <flash_highspeed_setup+0x424>)
30006226:	4798      	blx	r3
30006228:	e733      	b.n	30006092 <flash_highspeed_setup+0x1fe>
3000622a:	f10d 0215 	add.w	r2, sp, #21
3000622e:	2101      	movs	r1, #1
30006230:	f896 0057 	ldrb.w	r0, [r6, #87]	; 0x57
30006234:	47b8      	blx	r7
30006236:	2702      	movs	r7, #2
30006238:	e792      	b.n	30006160 <flash_highspeed_setup+0x2cc>
3000623a:	f10d 0215 	add.w	r2, sp, #21
3000623e:	2102      	movs	r1, #2
30006240:	2015      	movs	r0, #21
30006242:	47b8      	blx	r7
30006244:	9b04      	ldr	r3, [sp, #16]
30006246:	2703      	movs	r7, #3
30006248:	f443 3300 	orr.w	r3, r3, #131072	; 0x20000
3000624c:	9304      	str	r3, [sp, #16]
3000624e:	e787      	b.n	30006160 <flash_highspeed_setup+0x2cc>
30006250:	f89d 3015 	ldrb.w	r3, [sp, #21]
30006254:	2b40      	cmp	r3, #64	; 0x40
30006256:	d002      	beq.n	3000625e <flash_highspeed_setup+0x3ca>
30006258:	2b65      	cmp	r3, #101	; 0x65
3000625a:	f47f af66 	bne.w	3000612a <flash_highspeed_setup+0x296>
3000625e:	f89d 3016 	ldrb.w	r3, [sp, #22]
30006262:	2b15      	cmp	r3, #21
30006264:	f67f af61 	bls.w	3000612a <flash_highspeed_setup+0x296>
30006268:	e7c6      	b.n	300061f8 <flash_highspeed_setup+0x364>
3000626a:	f44f 7120 	mov.w	r1, #640	; 0x280
3000626e:	4815      	ldr	r0, [pc, #84]	; (300062c4 <flash_highspeed_setup+0x430>)
30006270:	f005 fe36 	bl	3000bee0 <io_assert_failed>
30006274:	f44f 7115 	mov.w	r1, #596	; 0x254
30006278:	4812      	ldr	r0, [pc, #72]	; (300062c4 <flash_highspeed_setup+0x430>)
3000627a:	f005 fe31 	bl	3000bee0 <io_assert_failed>
3000627e:	4b12      	ldr	r3, [pc, #72]	; (300062c8 <flash_highspeed_setup+0x434>)
30006280:	4798      	blx	r3
30006282:	e752      	b.n	3000612a <flash_highspeed_setup+0x296>
30006284:	0000c1d9 	.word	0x0000c1d9
30006288:	3000f920 	.word	0x3000f920
3000628c:	000129bd 	.word	0x000129bd
30006290:	30006841 	.word	0x30006841
30006294:	3000d56c 	.word	0x3000d56c
30006298:	3000d610 	.word	0x3000d610
3000629c:	2001c01c 	.word	0x2001c01c
300062a0:	0000be71 	.word	0x0000be71
300062a4:	0000bee9 	.word	0x0000bee9
300062a8:	0000bf59 	.word	0x0000bf59
300062ac:	0000a3c5 	.word	0x0000a3c5
300062b0:	0000a345 	.word	0x0000a345
300062b4:	0000a441 	.word	0x0000a441
300062b8:	0000bd0d 	.word	0x0000bd0d
300062bc:	3000d5a8 	.word	0x3000d5a8
300062c0:	0000b991 	.word	0x0000b991
300062c4:	3000d4f8 	.word	0x3000d4f8
300062c8:	0000a4c1 	.word	0x0000a4c1

300062cc <FLASH_UserMode_Enter>:
300062cc:	f3bf 8f4f 	dsb	sy
300062d0:	f3bf 8f6f 	isb	sy
300062d4:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
300062d8:	6a93      	ldr	r3, [r2, #40]	; 0x28
300062da:	07db      	lsls	r3, r3, #31
300062dc:	d4fc      	bmi.n	300062d8 <FLASH_UserMode_Enter+0xc>
300062de:	6813      	ldr	r3, [r2, #0]
300062e0:	f04f 4188 	mov.w	r1, #1140850688	; 0x44000000
300062e4:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
300062e8:	6013      	str	r3, [r2, #0]
300062ea:	680b      	ldr	r3, [r1, #0]
300062ec:	2b00      	cmp	r3, #0
300062ee:	dafc      	bge.n	300062ea <FLASH_UserMode_Enter+0x1e>
300062f0:	4770      	bx	lr
300062f2:	bf00      	nop

300062f4 <FLASH_UserMode_Exit>:
300062f4:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
300062f8:	6813      	ldr	r3, [r2, #0]
300062fa:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
300062fe:	6013      	str	r3, [r2, #0]
30006300:	6813      	ldr	r3, [r2, #0]
30006302:	2b00      	cmp	r3, #0
30006304:	dbfc      	blt.n	30006300 <FLASH_UserMode_Exit+0xc>
30006306:	f3bf 8f6f 	isb	sy
3000630a:	4770      	bx	lr

3000630c <FLASH_RxCmd_InUserMode>:
3000630c:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
30006310:	460c      	mov	r4, r1
30006312:	4606      	mov	r6, r0
30006314:	2100      	movs	r1, #0
30006316:	481c      	ldr	r0, [pc, #112]	; (30006388 <FLASH_RxCmd_InUserMode+0x7c>)
30006318:	4d1c      	ldr	r5, [pc, #112]	; (3000638c <FLASH_RxCmd_InUserMode+0x80>)
3000631a:	4617      	mov	r7, r2
3000631c:	47a8      	blx	r5
3000631e:	f04f 4c88 	mov.w	ip, #1140850688	; 0x44000000
30006322:	f024 407f 	bic.w	r0, r4, #4278190080	; 0xff000000
30006326:	1e7a      	subs	r2, r7, #1
30006328:	f8dc 3118 	ldr.w	r3, [ip, #280]	; 0x118
3000632c:	1911      	adds	r1, r2, r4
3000632e:	f423 2370 	bic.w	r3, r3, #983040	; 0xf0000
30006332:	f8cc 3118 	str.w	r3, [ip, #280]	; 0x118
30006336:	2300      	movs	r3, #0
30006338:	f8cc 0004 	str.w	r0, [ip, #4]
3000633c:	f8cc 3130 	str.w	r3, [ip, #304]	; 0x130
30006340:	4b13      	ldr	r3, [pc, #76]	; (30006390 <FLASH_RxCmd_InUserMode+0x84>)
30006342:	f8dc 0000 	ldr.w	r0, [ip]
30006346:	4003      	ands	r3, r0
30006348:	f443 7340 	orr.w	r3, r3, #768	; 0x300
3000634c:	f8cc 3000 	str.w	r3, [ip]
30006350:	2301      	movs	r3, #1
30006352:	f88c 6060 	strb.w	r6, [ip, #96]	; 0x60
30006356:	f8cc 3008 	str.w	r3, [ip, #8]
3000635a:	e003      	b.n	30006364 <FLASH_RxCmd_InUserMode+0x58>
3000635c:	f8dc 3028 	ldr.w	r3, [ip, #40]	; 0x28
30006360:	0718      	lsls	r0, r3, #28
30006362:	d40c      	bmi.n	3000637e <FLASH_RxCmd_InUserMode+0x72>
30006364:	428a      	cmp	r2, r1
30006366:	d1f9      	bne.n	3000635c <FLASH_RxCmd_InUserMode+0x50>
30006368:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
3000636c:	6893      	ldr	r3, [r2, #8]
3000636e:	07db      	lsls	r3, r3, #31
30006370:	d4fc      	bmi.n	3000636c <FLASH_RxCmd_InUserMode+0x60>
30006372:	4805      	ldr	r0, [pc, #20]	; (30006388 <FLASH_RxCmd_InUserMode+0x7c>)
30006374:	462b      	mov	r3, r5
30006376:	7901      	ldrb	r1, [r0, #4]
30006378:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
3000637c:	4718      	bx	r3
3000637e:	f89c 3060 	ldrb.w	r3, [ip, #96]	; 0x60
30006382:	f802 3f01 	strb.w	r3, [r2, #1]!
30006386:	e7ed      	b.n	30006364 <FLASH_RxCmd_InUserMode+0x58>
30006388:	2001c01c 	.word	0x2001c01c
3000638c:	00009c99 	.word	0x00009c99
30006390:	fff0fcff 	.word	0xfff0fcff

30006394 <FLASH_RxCmd>:
30006394:	b510      	push	{r4, lr}
30006396:	f3bf 8f4f 	dsb	sy
3000639a:	f3bf 8f6f 	isb	sy
3000639e:	f04f 4488 	mov.w	r4, #1140850688	; 0x44000000
300063a2:	6aa3      	ldr	r3, [r4, #40]	; 0x28
300063a4:	07db      	lsls	r3, r3, #31
300063a6:	d4fc      	bmi.n	300063a2 <FLASH_RxCmd+0xe>
300063a8:	6823      	ldr	r3, [r4, #0]
300063aa:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
300063ae:	6023      	str	r3, [r4, #0]
300063b0:	f04f 4488 	mov.w	r4, #1140850688	; 0x44000000
300063b4:	6823      	ldr	r3, [r4, #0]
300063b6:	2b00      	cmp	r3, #0
300063b8:	dafc      	bge.n	300063b4 <FLASH_RxCmd+0x20>
300063ba:	f7ff ffa7 	bl	3000630c <FLASH_RxCmd_InUserMode>
300063be:	6823      	ldr	r3, [r4, #0]
300063c0:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
300063c4:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
300063c8:	6023      	str	r3, [r4, #0]
300063ca:	6813      	ldr	r3, [r2, #0]
300063cc:	2b00      	cmp	r3, #0
300063ce:	dbfc      	blt.n	300063ca <FLASH_RxCmd+0x36>
300063d0:	f3bf 8f6f 	isb	sy
300063d4:	bd10      	pop	{r4, pc}
300063d6:	bf00      	nop

300063d8 <FLASH_WaitBusy_InUserMode>:
300063d8:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
300063dc:	2300      	movs	r3, #0
300063de:	f04f 4488 	mov.w	r4, #1140850688	; 0x44000000
300063e2:	4606      	mov	r6, r0
300063e4:	b936      	cbnz	r6, 300063f4 <FLASH_WaitBusy_InUserMode+0x1c>
300063e6:	6aa3      	ldr	r3, [r4, #40]	; 0x28
300063e8:	f003 0301 	and.w	r3, r3, #1
300063ec:	2b00      	cmp	r3, #0
300063ee:	d1f9      	bne.n	300063e4 <FLASH_WaitBusy_InUserMode+0xc>
300063f0:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
300063f4:	2e04      	cmp	r6, #4
300063f6:	d036      	beq.n	30006466 <FLASH_WaitBusy_InUserMode+0x8e>
300063f8:	2e01      	cmp	r6, #1
300063fa:	d03a      	beq.n	30006472 <FLASH_WaitBusy_InUserMode+0x9a>
300063fc:	2e03      	cmp	r6, #3
300063fe:	d1f5      	bne.n	300063ec <FLASH_WaitBusy_InUserMode+0x14>
30006400:	4f32      	ldr	r7, [pc, #200]	; (300064cc <FLASH_WaitBusy_InUserMode+0xf4>)
30006402:	2100      	movs	r1, #0
30006404:	f8df 80cc 	ldr.w	r8, [pc, #204]	; 300064d4 <FLASH_WaitBusy_InUserMode+0xfc>
30006408:	4638      	mov	r0, r7
3000640a:	f897 5056 	ldrb.w	r5, [r7, #86]	; 0x56
3000640e:	47c0      	blx	r8
30006410:	f8d4 3118 	ldr.w	r3, [r4, #280]	; 0x118
30006414:	2201      	movs	r2, #1
30006416:	f423 2370 	bic.w	r3, r3, #983040	; 0xf0000
3000641a:	f8c4 3118 	str.w	r3, [r4, #280]	; 0x118
3000641e:	2300      	movs	r3, #0
30006420:	6062      	str	r2, [r4, #4]
30006422:	f8c4 3130 	str.w	r3, [r4, #304]	; 0x130
30006426:	4b2a      	ldr	r3, [pc, #168]	; (300064d0 <FLASH_WaitBusy_InUserMode+0xf8>)
30006428:	6821      	ldr	r1, [r4, #0]
3000642a:	400b      	ands	r3, r1
3000642c:	f443 7340 	orr.w	r3, r3, #768	; 0x300
30006430:	6023      	str	r3, [r4, #0]
30006432:	f884 5060 	strb.w	r5, [r4, #96]	; 0x60
30006436:	60a2      	str	r2, [r4, #8]
30006438:	6aa3      	ldr	r3, [r4, #40]	; 0x28
3000643a:	071a      	lsls	r2, r3, #28
3000643c:	d5fc      	bpl.n	30006438 <FLASH_WaitBusy_InUserMode+0x60>
3000643e:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
30006442:	b2dd      	uxtb	r5, r3
30006444:	68a3      	ldr	r3, [r4, #8]
30006446:	07db      	lsls	r3, r3, #31
30006448:	d4fc      	bmi.n	30006444 <FLASH_WaitBusy_InUserMode+0x6c>
3000644a:	7939      	ldrb	r1, [r7, #4]
3000644c:	481f      	ldr	r0, [pc, #124]	; (300064cc <FLASH_WaitBusy_InUserMode+0xf4>)
3000644e:	47c0      	blx	r8
30006450:	697b      	ldr	r3, [r7, #20]
30006452:	421d      	tst	r5, r3
30006454:	d0d4      	beq.n	30006400 <FLASH_WaitBusy_InUserMode+0x28>
30006456:	693b      	ldr	r3, [r7, #16]
30006458:	421d      	tst	r5, r3
3000645a:	bf14      	ite	ne
3000645c:	2301      	movne	r3, #1
3000645e:	2300      	moveq	r3, #0
30006460:	2b00      	cmp	r3, #0
30006462:	d1bf      	bne.n	300063e4 <FLASH_WaitBusy_InUserMode+0xc>
30006464:	e7c4      	b.n	300063f0 <FLASH_WaitBusy_InUserMode+0x18>
30006466:	68a3      	ldr	r3, [r4, #8]
30006468:	f003 0301 	and.w	r3, r3, #1
3000646c:	2b00      	cmp	r3, #0
3000646e:	d1b9      	bne.n	300063e4 <FLASH_WaitBusy_InUserMode+0xc>
30006470:	e7be      	b.n	300063f0 <FLASH_WaitBusy_InUserMode+0x18>
30006472:	4f16      	ldr	r7, [pc, #88]	; (300064cc <FLASH_WaitBusy_InUserMode+0xf4>)
30006474:	2100      	movs	r1, #0
30006476:	f8df 805c 	ldr.w	r8, [pc, #92]	; 300064d4 <FLASH_WaitBusy_InUserMode+0xfc>
3000647a:	4638      	mov	r0, r7
3000647c:	f897 5056 	ldrb.w	r5, [r7, #86]	; 0x56
30006480:	47c0      	blx	r8
30006482:	f8d4 3118 	ldr.w	r3, [r4, #280]	; 0x118
30006486:	f423 2370 	bic.w	r3, r3, #983040	; 0xf0000
3000648a:	f8c4 3118 	str.w	r3, [r4, #280]	; 0x118
3000648e:	2300      	movs	r3, #0
30006490:	6066      	str	r6, [r4, #4]
30006492:	f8c4 3130 	str.w	r3, [r4, #304]	; 0x130
30006496:	4b0e      	ldr	r3, [pc, #56]	; (300064d0 <FLASH_WaitBusy_InUserMode+0xf8>)
30006498:	6822      	ldr	r2, [r4, #0]
3000649a:	4013      	ands	r3, r2
3000649c:	f443 7340 	orr.w	r3, r3, #768	; 0x300
300064a0:	6023      	str	r3, [r4, #0]
300064a2:	f884 5060 	strb.w	r5, [r4, #96]	; 0x60
300064a6:	60a6      	str	r6, [r4, #8]
300064a8:	6aa3      	ldr	r3, [r4, #40]	; 0x28
300064aa:	0718      	lsls	r0, r3, #28
300064ac:	d5fc      	bpl.n	300064a8 <FLASH_WaitBusy_InUserMode+0xd0>
300064ae:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
300064b2:	b2dd      	uxtb	r5, r3
300064b4:	68a3      	ldr	r3, [r4, #8]
300064b6:	07d9      	lsls	r1, r3, #31
300064b8:	d4fc      	bmi.n	300064b4 <FLASH_WaitBusy_InUserMode+0xdc>
300064ba:	7939      	ldrb	r1, [r7, #4]
300064bc:	4803      	ldr	r0, [pc, #12]	; (300064cc <FLASH_WaitBusy_InUserMode+0xf4>)
300064be:	47c0      	blx	r8
300064c0:	693b      	ldr	r3, [r7, #16]
300064c2:	402b      	ands	r3, r5
300064c4:	2b00      	cmp	r3, #0
300064c6:	d18d      	bne.n	300063e4 <FLASH_WaitBusy_InUserMode+0xc>
300064c8:	e792      	b.n	300063f0 <FLASH_WaitBusy_InUserMode+0x18>
300064ca:	bf00      	nop
300064cc:	2001c01c 	.word	0x2001c01c
300064d0:	fff0fcff 	.word	0xfff0fcff
300064d4:	00009c99 	.word	0x00009c99

300064d8 <FLASH_WriteEn_InUserMode>:
300064d8:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
300064dc:	f8df 90b0 	ldr.w	r9, [pc, #176]	; 30006590 <FLASH_WriteEn_InUserMode+0xb8>
300064e0:	f04f 0800 	mov.w	r8, #0
300064e4:	4f27      	ldr	r7, [pc, #156]	; (30006584 <FLASH_WriteEn_InUserMode+0xac>)
300064e6:	f04f 4488 	mov.w	r4, #1140850688	; 0x44000000
300064ea:	2601      	movs	r6, #1
300064ec:	2100      	movs	r1, #0
300064ee:	4648      	mov	r0, r9
300064f0:	f899 5056 	ldrb.w	r5, [r9, #86]	; 0x56
300064f4:	47b8      	blx	r7
300064f6:	f8d4 2118 	ldr.w	r2, [r4, #280]	; 0x118
300064fa:	4b23      	ldr	r3, [pc, #140]	; (30006588 <FLASH_WriteEn_InUserMode+0xb0>)
300064fc:	f422 2270 	bic.w	r2, r2, #983040	; 0xf0000
30006500:	f8c4 2118 	str.w	r2, [r4, #280]	; 0x118
30006504:	6066      	str	r6, [r4, #4]
30006506:	f8c4 8130 	str.w	r8, [r4, #304]	; 0x130
3000650a:	6822      	ldr	r2, [r4, #0]
3000650c:	4013      	ands	r3, r2
3000650e:	f443 7340 	orr.w	r3, r3, #768	; 0x300
30006512:	6023      	str	r3, [r4, #0]
30006514:	f884 5060 	strb.w	r5, [r4, #96]	; 0x60
30006518:	60a6      	str	r6, [r4, #8]
3000651a:	6aa3      	ldr	r3, [r4, #40]	; 0x28
3000651c:	0719      	lsls	r1, r3, #28
3000651e:	d5fc      	bpl.n	3000651a <FLASH_WriteEn_InUserMode+0x42>
30006520:	f894 5060 	ldrb.w	r5, [r4, #96]	; 0x60
30006524:	b2ed      	uxtb	r5, r5
30006526:	68a3      	ldr	r3, [r4, #8]
30006528:	07da      	lsls	r2, r3, #31
3000652a:	d4fc      	bmi.n	30006526 <FLASH_WriteEn_InUserMode+0x4e>
3000652c:	f899 1004 	ldrb.w	r1, [r9, #4]
30006530:	4648      	mov	r0, r9
30006532:	47b8      	blx	r7
30006534:	f8d9 3010 	ldr.w	r3, [r9, #16]
30006538:	401d      	ands	r5, r3
3000653a:	d1d7      	bne.n	300064ec <FLASH_WriteEn_InUserMode+0x14>
3000653c:	f8d4 0118 	ldr.w	r0, [r4, #280]	; 0x118
30006540:	4b12      	ldr	r3, [pc, #72]	; (3000658c <FLASH_WriteEn_InUserMode+0xb4>)
30006542:	6821      	ldr	r1, [r4, #0]
30006544:	6826      	ldr	r6, [r4, #0]
30006546:	f899 2054 	ldrb.w	r2, [r9, #84]	; 0x54
3000654a:	4033      	ands	r3, r6
3000654c:	6023      	str	r3, [r4, #0]
3000654e:	f8d4 3118 	ldr.w	r3, [r4, #280]	; 0x118
30006552:	f423 2370 	bic.w	r3, r3, #983040	; 0xf0000
30006556:	f8c4 3118 	str.w	r3, [r4, #280]	; 0x118
3000655a:	2301      	movs	r3, #1
3000655c:	6065      	str	r5, [r4, #4]
3000655e:	f8c4 5130 	str.w	r5, [r4, #304]	; 0x130
30006562:	f884 2060 	strb.w	r2, [r4, #96]	; 0x60
30006566:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
3000656a:	60a3      	str	r3, [r4, #8]
3000656c:	6893      	ldr	r3, [r2, #8]
3000656e:	07db      	lsls	r3, r3, #31
30006570:	d4fc      	bmi.n	3000656c <FLASH_WriteEn_InUserMode+0x94>
30006572:	f8c2 0118 	str.w	r0, [r2, #280]	; 0x118
30006576:	2003      	movs	r0, #3
30006578:	6011      	str	r1, [r2, #0]
3000657a:	e8bd 43f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
3000657e:	f7ff bf2b 	b.w	300063d8 <FLASH_WaitBusy_InUserMode>
30006582:	bf00      	nop
30006584:	00009c99 	.word	0x00009c99
30006588:	fff0fcff 	.word	0xfff0fcff
3000658c:	ffc0fcff 	.word	0xffc0fcff
30006590:	2001c01c 	.word	0x2001c01c

30006594 <FLASH_TxCmd_InUserMode>:
30006594:	f04f 4388 	mov.w	r3, #1140850688	; 0x44000000
30006598:	b4f0      	push	{r4, r5, r6, r7}
3000659a:	4c13      	ldr	r4, [pc, #76]	; (300065e8 <FLASH_TxCmd_InUserMode+0x54>)
3000659c:	f8d3 5118 	ldr.w	r5, [r3, #280]	; 0x118
300065a0:	681e      	ldr	r6, [r3, #0]
300065a2:	681f      	ldr	r7, [r3, #0]
300065a4:	403c      	ands	r4, r7
300065a6:	601c      	str	r4, [r3, #0]
300065a8:	f8d3 4118 	ldr.w	r4, [r3, #280]	; 0x118
300065ac:	f424 2470 	bic.w	r4, r4, #983040	; 0xf0000
300065b0:	f8c3 4118 	str.w	r4, [r3, #280]	; 0x118
300065b4:	2400      	movs	r4, #0
300065b6:	605c      	str	r4, [r3, #4]
300065b8:	f8c3 1130 	str.w	r1, [r3, #304]	; 0x130
300065bc:	f883 0060 	strb.w	r0, [r3, #96]	; 0x60
300065c0:	b131      	cbz	r1, 300065d0 <FLASH_TxCmd_InUserMode+0x3c>
300065c2:	1850      	adds	r0, r2, r1
300065c4:	f812 1b01 	ldrb.w	r1, [r2], #1
300065c8:	4282      	cmp	r2, r0
300065ca:	f883 1060 	strb.w	r1, [r3, #96]	; 0x60
300065ce:	d1f9      	bne.n	300065c4 <FLASH_TxCmd_InUserMode+0x30>
300065d0:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
300065d4:	2301      	movs	r3, #1
300065d6:	6093      	str	r3, [r2, #8]
300065d8:	6893      	ldr	r3, [r2, #8]
300065da:	07db      	lsls	r3, r3, #31
300065dc:	d4fc      	bmi.n	300065d8 <FLASH_TxCmd_InUserMode+0x44>
300065de:	f8c2 5118 	str.w	r5, [r2, #280]	; 0x118
300065e2:	6016      	str	r6, [r2, #0]
300065e4:	bcf0      	pop	{r4, r5, r6, r7}
300065e6:	4770      	bx	lr
300065e8:	ffc0fcff 	.word	0xffc0fcff

300065ec <FLASH_TxData>:
300065ec:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
300065f0:	b2c3      	uxtb	r3, r0
300065f2:	b085      	sub	sp, #20
300065f4:	9303      	str	r3, [sp, #12]
300065f6:	440b      	add	r3, r1
300065f8:	f5b3 7f80 	cmp.w	r3, #256	; 0x100
300065fc:	e9cd 2001 	strd	r2, r0, [sp, #4]
30006600:	f200 810e 	bhi.w	30006820 <FLASH_TxData+0x234>
30006604:	460e      	mov	r6, r1
30006606:	f3bf 8f4f 	dsb	sy
3000660a:	f3bf 8f6f 	isb	sy
3000660e:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30006612:	6a93      	ldr	r3, [r2, #40]	; 0x28
30006614:	07dc      	lsls	r4, r3, #31
30006616:	d4fc      	bmi.n	30006612 <FLASH_TxData+0x26>
30006618:	6813      	ldr	r3, [r2, #0]
3000661a:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
3000661e:	6013      	str	r3, [r2, #0]
30006620:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30006624:	6813      	ldr	r3, [r2, #0]
30006626:	2b00      	cmp	r3, #0
30006628:	dafc      	bge.n	30006624 <FLASH_TxData+0x38>
3000662a:	f8df b210 	ldr.w	fp, [pc, #528]	; 3000683c <FLASH_TxData+0x250>
3000662e:	f04f 0900 	mov.w	r9, #0
30006632:	4d7e      	ldr	r5, [pc, #504]	; (3000682c <FLASH_TxData+0x240>)
30006634:	f04f 4a88 	mov.w	sl, #1140850688	; 0x44000000
30006638:	2701      	movs	r7, #1
3000663a:	2100      	movs	r1, #0
3000663c:	4658      	mov	r0, fp
3000663e:	f89b 4056 	ldrb.w	r4, [fp, #86]	; 0x56
30006642:	47a8      	blx	r5
30006644:	f8da 2118 	ldr.w	r2, [sl, #280]	; 0x118
30006648:	4b79      	ldr	r3, [pc, #484]	; (30006830 <FLASH_TxData+0x244>)
3000664a:	f422 2270 	bic.w	r2, r2, #983040	; 0xf0000
3000664e:	f8ca 2118 	str.w	r2, [sl, #280]	; 0x118
30006652:	f8ca 7004 	str.w	r7, [sl, #4]
30006656:	f8ca 9130 	str.w	r9, [sl, #304]	; 0x130
3000665a:	f8da 2000 	ldr.w	r2, [sl]
3000665e:	4013      	ands	r3, r2
30006660:	f443 7340 	orr.w	r3, r3, #768	; 0x300
30006664:	f8ca 3000 	str.w	r3, [sl]
30006668:	f88a 4060 	strb.w	r4, [sl, #96]	; 0x60
3000666c:	f8ca 7008 	str.w	r7, [sl, #8]
30006670:	f8da 3028 	ldr.w	r3, [sl, #40]	; 0x28
30006674:	0718      	lsls	r0, r3, #28
30006676:	d5fb      	bpl.n	30006670 <FLASH_TxData+0x84>
30006678:	f89a 4060 	ldrb.w	r4, [sl, #96]	; 0x60
3000667c:	f04f 0801 	mov.w	r8, #1
30006680:	b2e4      	uxtb	r4, r4
30006682:	f8da 3008 	ldr.w	r3, [sl, #8]
30006686:	07d9      	lsls	r1, r3, #31
30006688:	d4fb      	bmi.n	30006682 <FLASH_TxData+0x96>
3000668a:	f89b 1004 	ldrb.w	r1, [fp, #4]
3000668e:	4658      	mov	r0, fp
30006690:	47a8      	blx	r5
30006692:	f8db 1010 	ldr.w	r1, [fp, #16]
30006696:	4021      	ands	r1, r4
30006698:	d1cf      	bne.n	3000663a <FLASH_TxData+0x4e>
3000669a:	f8da 2118 	ldr.w	r2, [sl, #280]	; 0x118
3000669e:	4c65      	ldr	r4, [pc, #404]	; (30006834 <FLASH_TxData+0x248>)
300066a0:	f8da 0000 	ldr.w	r0, [sl]
300066a4:	f8da 7000 	ldr.w	r7, [sl]
300066a8:	403c      	ands	r4, r7
300066aa:	f04f 4788 	mov.w	r7, #1140850688	; 0x44000000
300066ae:	f8ca 4000 	str.w	r4, [sl]
300066b2:	f8da 4118 	ldr.w	r4, [sl, #280]	; 0x118
300066b6:	f424 2470 	bic.w	r4, r4, #983040	; 0xf0000
300066ba:	f8ca 4118 	str.w	r4, [sl, #280]	; 0x118
300066be:	f8ca 1004 	str.w	r1, [sl, #4]
300066c2:	f8ca 1130 	str.w	r1, [sl, #304]	; 0x130
300066c6:	f89b 1054 	ldrb.w	r1, [fp, #84]	; 0x54
300066ca:	f88a 1060 	strb.w	r1, [sl, #96]	; 0x60
300066ce:	2101      	movs	r1, #1
300066d0:	f8ca 1008 	str.w	r1, [sl, #8]
300066d4:	68bc      	ldr	r4, [r7, #8]
300066d6:	f014 0401 	ands.w	r4, r4, #1
300066da:	d1fb      	bne.n	300066d4 <FLASH_TxData+0xe8>
300066dc:	f8c7 2118 	str.w	r2, [r7, #280]	; 0x118
300066e0:	6038      	str	r0, [r7, #0]
300066e2:	2003      	movs	r0, #3
300066e4:	f7ff fe78 	bl	300063d8 <FLASH_WaitBusy_InUserMode>
300066e8:	6839      	ldr	r1, [r7, #0]
300066ea:	4b52      	ldr	r3, [pc, #328]	; (30006834 <FLASH_TxData+0x248>)
300066ec:	6838      	ldr	r0, [r7, #0]
300066ee:	4003      	ands	r3, r0
300066f0:	603b      	str	r3, [r7, #0]
300066f2:	f8d7 3118 	ldr.w	r3, [r7, #280]	; 0x118
300066f6:	f89b 0062 	ldrb.w	r0, [fp, #98]	; 0x62
300066fa:	f423 2c70 	bic.w	ip, r3, #983040	; 0xf0000
300066fe:	0403      	lsls	r3, r0, #16
30006700:	2804      	cmp	r0, #4
30006702:	f403 2370 	and.w	r3, r3, #983040	; 0xf0000
30006706:	ea43 030c 	orr.w	r3, r3, ip
3000670a:	f8c7 3118 	str.w	r3, [r7, #280]	; 0x118
3000670e:	f026 437f 	bic.w	r3, r6, #4278190080	; 0xff000000
30006712:	607c      	str	r4, [r7, #4]
30006714:	f8c7 3130 	str.w	r3, [r7, #304]	; 0x130
30006718:	f04f 0302 	mov.w	r3, #2
3000671c:	f887 3060 	strb.w	r3, [r7, #96]	; 0x60
30006720:	d103      	bne.n	3000672a <FLASH_TxData+0x13e>
30006722:	9b02      	ldr	r3, [sp, #8]
30006724:	0e1b      	lsrs	r3, r3, #24
30006726:	f887 3060 	strb.w	r3, [r7, #96]	; 0x60
3000672a:	9a02      	ldr	r2, [sp, #8]
3000672c:	f04f 4388 	mov.w	r3, #1140850688	; 0x44000000
30006730:	f3c2 4007 	ubfx	r0, r2, #16, #8
30006734:	f3c2 2a07 	ubfx	sl, r2, #8, #8
30006738:	f883 0060 	strb.w	r0, [r3, #96]	; 0x60
3000673c:	f883 a060 	strb.w	sl, [r3, #96]	; 0x60
30006740:	9a03      	ldr	r2, [sp, #12]
30006742:	f883 2060 	strb.w	r2, [r3, #96]	; 0x60
30006746:	2e00      	cmp	r6, #0
30006748:	d068      	beq.n	3000681c <FLASH_TxData+0x230>
3000674a:	9a01      	ldr	r2, [sp, #4]
3000674c:	1e57      	subs	r7, r2, #1
3000674e:	e006      	b.n	3000675e <FLASH_TxData+0x172>
30006750:	3401      	adds	r4, #1
30006752:	f817 2f01 	ldrb.w	r2, [r7, #1]!
30006756:	42a6      	cmp	r6, r4
30006758:	f883 2060 	strb.w	r2, [r3, #96]	; 0x60
3000675c:	d002      	beq.n	30006764 <FLASH_TxData+0x178>
3000675e:	6a9a      	ldr	r2, [r3, #40]	; 0x28
30006760:	0792      	lsls	r2, r2, #30
30006762:	d4f5      	bmi.n	30006750 <FLASH_TxData+0x164>
30006764:	2301      	movs	r3, #1
30006766:	f04f 4088 	mov.w	r0, #1140850688	; 0x44000000
3000676a:	1e67      	subs	r7, r4, #1
3000676c:	6083      	str	r3, [r0, #8]
3000676e:	9b01      	ldr	r3, [sp, #4]
30006770:	443b      	add	r3, r7
30006772:	461f      	mov	r7, r3
30006774:	e002      	b.n	3000677c <FLASH_TxData+0x190>
30006776:	6a83      	ldr	r3, [r0, #40]	; 0x28
30006778:	079b      	lsls	r3, r3, #30
3000677a:	d449      	bmi.n	30006810 <FLASH_TxData+0x224>
3000677c:	42b4      	cmp	r4, r6
3000677e:	d3fa      	bcc.n	30006776 <FLASH_TxData+0x18a>
30006780:	f04f 4388 	mov.w	r3, #1140850688	; 0x44000000
30006784:	689e      	ldr	r6, [r3, #8]
30006786:	f016 0601 	ands.w	r6, r6, #1
3000678a:	d1fb      	bne.n	30006784 <FLASH_TxData+0x198>
3000678c:	4637      	mov	r7, r6
3000678e:	f8df 90ac 	ldr.w	r9, [pc, #172]	; 3000683c <FLASH_TxData+0x250>
30006792:	f04f 4488 	mov.w	r4, #1140850688	; 0x44000000
30006796:	6019      	str	r1, [r3, #0]
30006798:	2100      	movs	r1, #0
3000679a:	4648      	mov	r0, r9
3000679c:	f89b a056 	ldrb.w	sl, [fp, #86]	; 0x56
300067a0:	47a8      	blx	r5
300067a2:	f8d4 1118 	ldr.w	r1, [r4, #280]	; 0x118
300067a6:	2001      	movs	r0, #1
300067a8:	4a21      	ldr	r2, [pc, #132]	; (30006830 <FLASH_TxData+0x244>)
300067aa:	2300      	movs	r3, #0
300067ac:	f421 2170 	bic.w	r1, r1, #983040	; 0xf0000
300067b0:	f8c4 1118 	str.w	r1, [r4, #280]	; 0x118
300067b4:	6060      	str	r0, [r4, #4]
300067b6:	f8c4 7130 	str.w	r7, [r4, #304]	; 0x130
300067ba:	6821      	ldr	r1, [r4, #0]
300067bc:	400a      	ands	r2, r1
300067be:	f442 7240 	orr.w	r2, r2, #768	; 0x300
300067c2:	6022      	str	r2, [r4, #0]
300067c4:	f884 a060 	strb.w	sl, [r4, #96]	; 0x60
300067c8:	60a0      	str	r0, [r4, #8]
300067ca:	b943      	cbnz	r3, 300067de <FLASH_TxData+0x1f2>
300067cc:	6aa3      	ldr	r3, [r4, #40]	; 0x28
300067ce:	071a      	lsls	r2, r3, #28
300067d0:	d5fc      	bpl.n	300067cc <FLASH_TxData+0x1e0>
300067d2:	f894 6060 	ldrb.w	r6, [r4, #96]	; 0x60
300067d6:	4643      	mov	r3, r8
300067d8:	b2f6      	uxtb	r6, r6
300067da:	2b00      	cmp	r3, #0
300067dc:	d0f6      	beq.n	300067cc <FLASH_TxData+0x1e0>
300067de:	68a3      	ldr	r3, [r4, #8]
300067e0:	07db      	lsls	r3, r3, #31
300067e2:	d4fc      	bmi.n	300067de <FLASH_TxData+0x1f2>
300067e4:	f89b 1004 	ldrb.w	r1, [fp, #4]
300067e8:	4648      	mov	r0, r9
300067ea:	47a8      	blx	r5
300067ec:	f8db 3010 	ldr.w	r3, [fp, #16]
300067f0:	421e      	tst	r6, r3
300067f2:	d1d1      	bne.n	30006798 <FLASH_TxData+0x1ac>
300067f4:	6823      	ldr	r3, [r4, #0]
300067f6:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
300067fa:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
300067fe:	6023      	str	r3, [r4, #0]
30006800:	6813      	ldr	r3, [r2, #0]
30006802:	2b00      	cmp	r3, #0
30006804:	dbfc      	blt.n	30006800 <FLASH_TxData+0x214>
30006806:	f3bf 8f6f 	isb	sy
3000680a:	b005      	add	sp, #20
3000680c:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30006810:	f817 3f01 	ldrb.w	r3, [r7, #1]!
30006814:	3401      	adds	r4, #1
30006816:	f880 3060 	strb.w	r3, [r0, #96]	; 0x60
3000681a:	e7af      	b.n	3000677c <FLASH_TxData+0x190>
3000681c:	4634      	mov	r4, r6
3000681e:	e7a1      	b.n	30006764 <FLASH_TxData+0x178>
30006820:	f640 2159 	movw	r1, #2649	; 0xa59
30006824:	4804      	ldr	r0, [pc, #16]	; (30006838 <FLASH_TxData+0x24c>)
30006826:	f005 fb5b 	bl	3000bee0 <io_assert_failed>
3000682a:	bf00      	nop
3000682c:	00009c99 	.word	0x00009c99
30006830:	fff0fcff 	.word	0xfff0fcff
30006834:	ffc0fcff 	.word	0xffc0fcff
30006838:	3000e7e4 	.word	0x3000e7e4
3000683c:	2001c01c 	.word	0x2001c01c

30006840 <FLASH_SetStatus>:
30006840:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30006844:	ed2d 8b02 	vpush	{d8}
30006848:	b083      	sub	sp, #12
3000684a:	4681      	mov	r9, r0
3000684c:	ee08 2a10 	vmov	s16, r2
30006850:	9101      	str	r1, [sp, #4]
30006852:	f3bf 8f4f 	dsb	sy
30006856:	f3bf 8f6f 	isb	sy
3000685a:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
3000685e:	6a93      	ldr	r3, [r2, #40]	; 0x28
30006860:	07dc      	lsls	r4, r3, #31
30006862:	d4fc      	bmi.n	3000685e <FLASH_SetStatus+0x1e>
30006864:	6813      	ldr	r3, [r2, #0]
30006866:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
3000686a:	6013      	str	r3, [r2, #0]
3000686c:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30006870:	6813      	ldr	r3, [r2, #0]
30006872:	2b00      	cmp	r3, #0
30006874:	dafc      	bge.n	30006870 <FLASH_SetStatus+0x30>
30006876:	f8df b15c 	ldr.w	fp, [pc, #348]	; 300069d4 <FLASH_SetStatus+0x194>
3000687a:	f04f 0800 	mov.w	r8, #0
3000687e:	4d52      	ldr	r5, [pc, #328]	; (300069c8 <FLASH_SetStatus+0x188>)
30006880:	f04f 4a88 	mov.w	sl, #1140850688	; 0x44000000
30006884:	2601      	movs	r6, #1
30006886:	2100      	movs	r1, #0
30006888:	4658      	mov	r0, fp
3000688a:	f89b 4056 	ldrb.w	r4, [fp, #86]	; 0x56
3000688e:	47a8      	blx	r5
30006890:	f8da 0118 	ldr.w	r0, [sl, #280]	; 0x118
30006894:	494d      	ldr	r1, [pc, #308]	; (300069cc <FLASH_SetStatus+0x18c>)
30006896:	f420 2070 	bic.w	r0, r0, #983040	; 0xf0000
3000689a:	f8ca 0118 	str.w	r0, [sl, #280]	; 0x118
3000689e:	f8ca 6004 	str.w	r6, [sl, #4]
300068a2:	f8ca 8130 	str.w	r8, [sl, #304]	; 0x130
300068a6:	f8da 0000 	ldr.w	r0, [sl]
300068aa:	4001      	ands	r1, r0
300068ac:	f441 7140 	orr.w	r1, r1, #768	; 0x300
300068b0:	f8ca 1000 	str.w	r1, [sl]
300068b4:	f88a 4060 	strb.w	r4, [sl, #96]	; 0x60
300068b8:	f8ca 6008 	str.w	r6, [sl, #8]
300068bc:	f8da 1028 	ldr.w	r1, [sl, #40]	; 0x28
300068c0:	0708      	lsls	r0, r1, #28
300068c2:	d5fb      	bpl.n	300068bc <FLASH_SetStatus+0x7c>
300068c4:	f89a 4060 	ldrb.w	r4, [sl, #96]	; 0x60
300068c8:	2701      	movs	r7, #1
300068ca:	b2e4      	uxtb	r4, r4
300068cc:	f8da 1008 	ldr.w	r1, [sl, #8]
300068d0:	07c9      	lsls	r1, r1, #31
300068d2:	d4fb      	bmi.n	300068cc <FLASH_SetStatus+0x8c>
300068d4:	f89b 1004 	ldrb.w	r1, [fp, #4]
300068d8:	4658      	mov	r0, fp
300068da:	47a8      	blx	r5
300068dc:	f8db 1010 	ldr.w	r1, [fp, #16]
300068e0:	4021      	ands	r1, r4
300068e2:	d1d0      	bne.n	30006886 <FLASH_SetStatus+0x46>
300068e4:	f8da c118 	ldr.w	ip, [sl, #280]	; 0x118
300068e8:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
300068ec:	f8da 4000 	ldr.w	r4, [sl]
300068f0:	4837      	ldr	r0, [pc, #220]	; (300069d0 <FLASH_SetStatus+0x190>)
300068f2:	f8da 6000 	ldr.w	r6, [sl]
300068f6:	4030      	ands	r0, r6
300068f8:	f8ca 0000 	str.w	r0, [sl]
300068fc:	f8da 0118 	ldr.w	r0, [sl, #280]	; 0x118
30006900:	f420 2070 	bic.w	r0, r0, #983040	; 0xf0000
30006904:	f8ca 0118 	str.w	r0, [sl, #280]	; 0x118
30006908:	f8ca 1004 	str.w	r1, [sl, #4]
3000690c:	f8ca 1130 	str.w	r1, [sl, #304]	; 0x130
30006910:	f89b 1054 	ldrb.w	r1, [fp, #84]	; 0x54
30006914:	f88a 1060 	strb.w	r1, [sl, #96]	; 0x60
30006918:	2101      	movs	r1, #1
3000691a:	f8ca 1008 	str.w	r1, [sl, #8]
3000691e:	6896      	ldr	r6, [r2, #8]
30006920:	f016 0601 	ands.w	r6, r6, #1
30006924:	d1fb      	bne.n	3000691e <FLASH_SetStatus+0xde>
30006926:	f8c2 c118 	str.w	ip, [r2, #280]	; 0x118
3000692a:	2003      	movs	r0, #3
3000692c:	6014      	str	r4, [r2, #0]
3000692e:	46b0      	mov	r8, r6
30006930:	f7ff fd52 	bl	300063d8 <FLASH_WaitBusy_InUserMode>
30006934:	f04f 4488 	mov.w	r4, #1140850688	; 0x44000000
30006938:	4648      	mov	r0, r9
3000693a:	f8df 9098 	ldr.w	r9, [pc, #152]	; 300069d4 <FLASH_SetStatus+0x194>
3000693e:	ee18 2a10 	vmov	r2, s16
30006942:	f89d 1004 	ldrb.w	r1, [sp, #4]
30006946:	f7ff fe25 	bl	30006594 <FLASH_TxCmd_InUserMode>
3000694a:	2100      	movs	r1, #0
3000694c:	4648      	mov	r0, r9
3000694e:	f89b a056 	ldrb.w	sl, [fp, #86]	; 0x56
30006952:	47a8      	blx	r5
30006954:	f8d4 0118 	ldr.w	r0, [r4, #280]	; 0x118
30006958:	2301      	movs	r3, #1
3000695a:	491c      	ldr	r1, [pc, #112]	; (300069cc <FLASH_SetStatus+0x18c>)
3000695c:	2200      	movs	r2, #0
3000695e:	f420 2070 	bic.w	r0, r0, #983040	; 0xf0000
30006962:	f8c4 0118 	str.w	r0, [r4, #280]	; 0x118
30006966:	6063      	str	r3, [r4, #4]
30006968:	f8c4 8130 	str.w	r8, [r4, #304]	; 0x130
3000696c:	6820      	ldr	r0, [r4, #0]
3000696e:	4001      	ands	r1, r0
30006970:	f441 7140 	orr.w	r1, r1, #768	; 0x300
30006974:	6021      	str	r1, [r4, #0]
30006976:	f884 a060 	strb.w	sl, [r4, #96]	; 0x60
3000697a:	60a3      	str	r3, [r4, #8]
3000697c:	b942      	cbnz	r2, 30006990 <FLASH_SetStatus+0x150>
3000697e:	6aa2      	ldr	r2, [r4, #40]	; 0x28
30006980:	0712      	lsls	r2, r2, #28
30006982:	d5fc      	bpl.n	3000697e <FLASH_SetStatus+0x13e>
30006984:	f894 6060 	ldrb.w	r6, [r4, #96]	; 0x60
30006988:	463a      	mov	r2, r7
3000698a:	b2f6      	uxtb	r6, r6
3000698c:	2a00      	cmp	r2, #0
3000698e:	d0f6      	beq.n	3000697e <FLASH_SetStatus+0x13e>
30006990:	68a2      	ldr	r2, [r4, #8]
30006992:	07d3      	lsls	r3, r2, #31
30006994:	d4fc      	bmi.n	30006990 <FLASH_SetStatus+0x150>
30006996:	f89b 1004 	ldrb.w	r1, [fp, #4]
3000699a:	4648      	mov	r0, r9
3000699c:	47a8      	blx	r5
3000699e:	f8db 2010 	ldr.w	r2, [fp, #16]
300069a2:	4216      	tst	r6, r2
300069a4:	d1d1      	bne.n	3000694a <FLASH_SetStatus+0x10a>
300069a6:	6823      	ldr	r3, [r4, #0]
300069a8:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
300069ac:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
300069b0:	6023      	str	r3, [r4, #0]
300069b2:	6813      	ldr	r3, [r2, #0]
300069b4:	2b00      	cmp	r3, #0
300069b6:	dbfc      	blt.n	300069b2 <FLASH_SetStatus+0x172>
300069b8:	f3bf 8f6f 	isb	sy
300069bc:	b003      	add	sp, #12
300069be:	ecbd 8b02 	vpop	{d8}
300069c2:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
300069c6:	bf00      	nop
300069c8:	00009c99 	.word	0x00009c99
300069cc:	fff0fcff 	.word	0xfff0fcff
300069d0:	ffc0fcff 	.word	0xffc0fcff
300069d4:	2001c01c 	.word	0x2001c01c

300069d8 <FLASH_SetStatusBits>:
300069d8:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
300069dc:	4604      	mov	r4, r0
300069de:	b085      	sub	sp, #20
300069e0:	460d      	mov	r5, r1
300069e2:	f3bf 8f4f 	dsb	sy
300069e6:	f3bf 8f6f 	isb	sy
300069ea:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
300069ee:	6a93      	ldr	r3, [r2, #40]	; 0x28
300069f0:	f013 0801 	ands.w	r8, r3, #1
300069f4:	d1fb      	bne.n	300069ee <FLASH_SetStatusBits+0x16>
300069f6:	6813      	ldr	r3, [r2, #0]
300069f8:	f04f 4988 	mov.w	r9, #1140850688	; 0x44000000
300069fc:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
30006a00:	6013      	str	r3, [r2, #0]
30006a02:	f8d9 3000 	ldr.w	r3, [r9]
30006a06:	2b00      	cmp	r3, #0
30006a08:	dafb      	bge.n	30006a02 <FLASH_SetStatusBits+0x2a>
30006a0a:	4fa2      	ldr	r7, [pc, #648]	; (30006c94 <FLASH_SetStatusBits+0x2bc>)
30006a0c:	2100      	movs	r1, #0
30006a0e:	4ea2      	ldr	r6, [pc, #648]	; (30006c98 <FLASH_SetStatusBits+0x2c0>)
30006a10:	4638      	mov	r0, r7
30006a12:	f897 a056 	ldrb.w	sl, [r7, #86]	; 0x56
30006a16:	47b0      	blx	r6
30006a18:	f8d9 3118 	ldr.w	r3, [r9, #280]	; 0x118
30006a1c:	2201      	movs	r2, #1
30006a1e:	f423 2370 	bic.w	r3, r3, #983040	; 0xf0000
30006a22:	f8c9 3118 	str.w	r3, [r9, #280]	; 0x118
30006a26:	2300      	movs	r3, #0
30006a28:	f8c9 2004 	str.w	r2, [r9, #4]
30006a2c:	f8c9 3130 	str.w	r3, [r9, #304]	; 0x130
30006a30:	4b9a      	ldr	r3, [pc, #616]	; (30006c9c <FLASH_SetStatusBits+0x2c4>)
30006a32:	f8d9 1000 	ldr.w	r1, [r9]
30006a36:	400b      	ands	r3, r1
30006a38:	f443 7340 	orr.w	r3, r3, #768	; 0x300
30006a3c:	f8c9 3000 	str.w	r3, [r9]
30006a40:	f889 a060 	strb.w	sl, [r9, #96]	; 0x60
30006a44:	f8c9 2008 	str.w	r2, [r9, #8]
30006a48:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30006a4c:	f1b8 0f00 	cmp.w	r8, #0
30006a50:	d108      	bne.n	30006a64 <FLASH_SetStatusBits+0x8c>
30006a52:	6a93      	ldr	r3, [r2, #40]	; 0x28
30006a54:	0719      	lsls	r1, r3, #28
30006a56:	d5fc      	bpl.n	30006a52 <FLASH_SetStatusBits+0x7a>
30006a58:	f892 3060 	ldrb.w	r3, [r2, #96]	; 0x60
30006a5c:	f04f 0801 	mov.w	r8, #1
30006a60:	f88d 300c 	strb.w	r3, [sp, #12]
30006a64:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30006a68:	6893      	ldr	r3, [r2, #8]
30006a6a:	07db      	lsls	r3, r3, #31
30006a6c:	d4fc      	bmi.n	30006a68 <FLASH_SetStatusBits+0x90>
30006a6e:	7939      	ldrb	r1, [r7, #4]
30006a70:	4888      	ldr	r0, [pc, #544]	; (30006c94 <FLASH_SetStatusBits+0x2bc>)
30006a72:	47b0      	blx	r6
30006a74:	2d01      	cmp	r5, #1
30006a76:	f89d 300c 	ldrb.w	r3, [sp, #12]
30006a7a:	f000 8113 	beq.w	30006ca4 <FLASH_SetStatusBits+0x2cc>
30006a7e:	ea23 0304 	bic.w	r3, r3, r4
30006a82:	f88d 300c 	strb.w	r3, [sp, #12]
30006a86:	69bb      	ldr	r3, [r7, #24]
30006a88:	2b00      	cmp	r3, #0
30006a8a:	f040 8113 	bne.w	30006cb4 <FLASH_SetStatusBits+0x2dc>
30006a8e:	2401      	movs	r4, #1
30006a90:	f897 a059 	ldrb.w	sl, [r7, #89]	; 0x59
30006a94:	f1ba 0f00 	cmp.w	sl, #0
30006a98:	d147      	bne.n	30006b2a <FLASH_SetStatusBits+0x152>
30006a9a:	f7ff fd1d 	bl	300064d8 <FLASH_WriteEn_InUserMode>
30006a9e:	4621      	mov	r1, r4
30006aa0:	aa03      	add	r2, sp, #12
30006aa2:	f897 0058 	ldrb.w	r0, [r7, #88]	; 0x58
30006aa6:	46d1      	mov	r9, sl
30006aa8:	4d7a      	ldr	r5, [pc, #488]	; (30006c94 <FLASH_SetStatusBits+0x2bc>)
30006aaa:	f04f 4488 	mov.w	r4, #1140850688	; 0x44000000
30006aae:	f7ff fd71 	bl	30006594 <FLASH_TxCmd_InUserMode>
30006ab2:	2100      	movs	r1, #0
30006ab4:	4628      	mov	r0, r5
30006ab6:	f897 b056 	ldrb.w	fp, [r7, #86]	; 0x56
30006aba:	47b0      	blx	r6
30006abc:	f8d4 1118 	ldr.w	r1, [r4, #280]	; 0x118
30006ac0:	2001      	movs	r0, #1
30006ac2:	4a76      	ldr	r2, [pc, #472]	; (30006c9c <FLASH_SetStatusBits+0x2c4>)
30006ac4:	2300      	movs	r3, #0
30006ac6:	f421 2170 	bic.w	r1, r1, #983040	; 0xf0000
30006aca:	f8c4 1118 	str.w	r1, [r4, #280]	; 0x118
30006ace:	6060      	str	r0, [r4, #4]
30006ad0:	f8c4 9130 	str.w	r9, [r4, #304]	; 0x130
30006ad4:	6821      	ldr	r1, [r4, #0]
30006ad6:	400a      	ands	r2, r1
30006ad8:	f442 7240 	orr.w	r2, r2, #768	; 0x300
30006adc:	6022      	str	r2, [r4, #0]
30006ade:	f884 b060 	strb.w	fp, [r4, #96]	; 0x60
30006ae2:	60a0      	str	r0, [r4, #8]
30006ae4:	b94b      	cbnz	r3, 30006afa <FLASH_SetStatusBits+0x122>
30006ae6:	6aa3      	ldr	r3, [r4, #40]	; 0x28
30006ae8:	071a      	lsls	r2, r3, #28
30006aea:	d5fc      	bpl.n	30006ae6 <FLASH_SetStatusBits+0x10e>
30006aec:	f894 a060 	ldrb.w	sl, [r4, #96]	; 0x60
30006af0:	4643      	mov	r3, r8
30006af2:	fa5f fa8a 	uxtb.w	sl, sl
30006af6:	2b00      	cmp	r3, #0
30006af8:	d0f5      	beq.n	30006ae6 <FLASH_SetStatusBits+0x10e>
30006afa:	68a3      	ldr	r3, [r4, #8]
30006afc:	07db      	lsls	r3, r3, #31
30006afe:	d4fc      	bmi.n	30006afa <FLASH_SetStatusBits+0x122>
30006b00:	7939      	ldrb	r1, [r7, #4]
30006b02:	4628      	mov	r0, r5
30006b04:	47b0      	blx	r6
30006b06:	693b      	ldr	r3, [r7, #16]
30006b08:	ea1a 0f03 	tst.w	sl, r3
30006b0c:	d1d1      	bne.n	30006ab2 <FLASH_SetStatusBits+0xda>
30006b0e:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30006b12:	6813      	ldr	r3, [r2, #0]
30006b14:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
30006b18:	6013      	str	r3, [r2, #0]
30006b1a:	6813      	ldr	r3, [r2, #0]
30006b1c:	2b00      	cmp	r3, #0
30006b1e:	dbfc      	blt.n	30006b1a <FLASH_SetStatusBits+0x142>
30006b20:	f3bf 8f6f 	isb	sy
30006b24:	b005      	add	sp, #20
30006b26:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30006b2a:	f7ff fcd5 	bl	300064d8 <FLASH_WriteEn_InUserMode>
30006b2e:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30006b32:	4b5b      	ldr	r3, [pc, #364]	; (30006ca0 <FLASH_SetStatusBits+0x2c8>)
30006b34:	f8d2 0118 	ldr.w	r0, [r2, #280]	; 0x118
30006b38:	6811      	ldr	r1, [r2, #0]
30006b3a:	6815      	ldr	r5, [r2, #0]
30006b3c:	f897 4058 	ldrb.w	r4, [r7, #88]	; 0x58
30006b40:	402b      	ands	r3, r5
30006b42:	2500      	movs	r5, #0
30006b44:	6013      	str	r3, [r2, #0]
30006b46:	f8d2 3118 	ldr.w	r3, [r2, #280]	; 0x118
30006b4a:	f423 2370 	bic.w	r3, r3, #983040	; 0xf0000
30006b4e:	f8c2 3118 	str.w	r3, [r2, #280]	; 0x118
30006b52:	2301      	movs	r3, #1
30006b54:	6055      	str	r5, [r2, #4]
30006b56:	f8c2 3130 	str.w	r3, [r2, #304]	; 0x130
30006b5a:	f882 4060 	strb.w	r4, [r2, #96]	; 0x60
30006b5e:	f89d 400c 	ldrb.w	r4, [sp, #12]
30006b62:	f882 4060 	strb.w	r4, [r2, #96]	; 0x60
30006b66:	6093      	str	r3, [r2, #8]
30006b68:	6893      	ldr	r3, [r2, #8]
30006b6a:	f013 0901 	ands.w	r9, r3, #1
30006b6e:	d1fb      	bne.n	30006b68 <FLASH_SetStatusBits+0x190>
30006b70:	f8df b120 	ldr.w	fp, [pc, #288]	; 30006c94 <FLASH_SetStatusBits+0x2bc>
30006b74:	f04f 4488 	mov.w	r4, #1140850688	; 0x44000000
30006b78:	f8c2 0118 	str.w	r0, [r2, #280]	; 0x118
30006b7c:	6011      	str	r1, [r2, #0]
30006b7e:	2100      	movs	r1, #0
30006b80:	4658      	mov	r0, fp
30006b82:	f897 a056 	ldrb.w	sl, [r7, #86]	; 0x56
30006b86:	47b0      	blx	r6
30006b88:	f8d4 1118 	ldr.w	r1, [r4, #280]	; 0x118
30006b8c:	2001      	movs	r0, #1
30006b8e:	2500      	movs	r5, #0
30006b90:	4b42      	ldr	r3, [pc, #264]	; (30006c9c <FLASH_SetStatusBits+0x2c4>)
30006b92:	f421 2170 	bic.w	r1, r1, #983040	; 0xf0000
30006b96:	f8c4 1118 	str.w	r1, [r4, #280]	; 0x118
30006b9a:	6060      	str	r0, [r4, #4]
30006b9c:	f8c4 5130 	str.w	r5, [r4, #304]	; 0x130
30006ba0:	6821      	ldr	r1, [r4, #0]
30006ba2:	400b      	ands	r3, r1
30006ba4:	f443 7340 	orr.w	r3, r3, #768	; 0x300
30006ba8:	6023      	str	r3, [r4, #0]
30006baa:	f884 a060 	strb.w	sl, [r4, #96]	; 0x60
30006bae:	60a0      	str	r0, [r4, #8]
30006bb0:	b94d      	cbnz	r5, 30006bc6 <FLASH_SetStatusBits+0x1ee>
30006bb2:	6aa3      	ldr	r3, [r4, #40]	; 0x28
30006bb4:	0718      	lsls	r0, r3, #28
30006bb6:	d5fc      	bpl.n	30006bb2 <FLASH_SetStatusBits+0x1da>
30006bb8:	f894 9060 	ldrb.w	r9, [r4, #96]	; 0x60
30006bbc:	4645      	mov	r5, r8
30006bbe:	fa5f f989 	uxtb.w	r9, r9
30006bc2:	2d00      	cmp	r5, #0
30006bc4:	d0f5      	beq.n	30006bb2 <FLASH_SetStatusBits+0x1da>
30006bc6:	68a3      	ldr	r3, [r4, #8]
30006bc8:	07d9      	lsls	r1, r3, #31
30006bca:	d4fc      	bmi.n	30006bc6 <FLASH_SetStatusBits+0x1ee>
30006bcc:	7939      	ldrb	r1, [r7, #4]
30006bce:	4658      	mov	r0, fp
30006bd0:	47b0      	blx	r6
30006bd2:	693b      	ldr	r3, [r7, #16]
30006bd4:	ea19 0303 	ands.w	r3, r9, r3
30006bd8:	d1d1      	bne.n	30006b7e <FLASH_SetStatusBits+0x1a6>
30006bda:	9301      	str	r3, [sp, #4]
30006bdc:	f7ff fc7c 	bl	300064d8 <FLASH_WriteEn_InUserMode>
30006be0:	f8d4 0118 	ldr.w	r0, [r4, #280]	; 0x118
30006be4:	4a2e      	ldr	r2, [pc, #184]	; (30006ca0 <FLASH_SetStatusBits+0x2c8>)
30006be6:	6821      	ldr	r1, [r4, #0]
30006be8:	f8d4 c000 	ldr.w	ip, [r4]
30006bec:	ea0c 0202 	and.w	r2, ip, r2
30006bf0:	6022      	str	r2, [r4, #0]
30006bf2:	f8d4 2118 	ldr.w	r2, [r4, #280]	; 0x118
30006bf6:	f422 2270 	bic.w	r2, r2, #983040	; 0xf0000
30006bfa:	f8c4 2118 	str.w	r2, [r4, #280]	; 0x118
30006bfe:	9b01      	ldr	r3, [sp, #4]
30006c00:	6063      	str	r3, [r4, #4]
30006c02:	2301      	movs	r3, #1
30006c04:	f8c4 3130 	str.w	r3, [r4, #304]	; 0x130
30006c08:	f897 2059 	ldrb.w	r2, [r7, #89]	; 0x59
30006c0c:	f884 2060 	strb.w	r2, [r4, #96]	; 0x60
30006c10:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30006c14:	f89d c00d 	ldrb.w	ip, [sp, #13]
30006c18:	f884 c060 	strb.w	ip, [r4, #96]	; 0x60
30006c1c:	60a3      	str	r3, [r4, #8]
30006c1e:	6893      	ldr	r3, [r2, #8]
30006c20:	f013 0801 	ands.w	r8, r3, #1
30006c24:	d1fb      	bne.n	30006c1e <FLASH_SetStatusBits+0x246>
30006c26:	46c1      	mov	r9, r8
30006c28:	f8df a068 	ldr.w	sl, [pc, #104]	; 30006c94 <FLASH_SetStatusBits+0x2bc>
30006c2c:	f04f 4488 	mov.w	r4, #1140850688	; 0x44000000
30006c30:	f8c2 0118 	str.w	r0, [r2, #280]	; 0x118
30006c34:	6011      	str	r1, [r2, #0]
30006c36:	2100      	movs	r1, #0
30006c38:	4650      	mov	r0, sl
30006c3a:	f897 b056 	ldrb.w	fp, [r7, #86]	; 0x56
30006c3e:	47b0      	blx	r6
30006c40:	f8d4 1118 	ldr.w	r1, [r4, #280]	; 0x118
30006c44:	2001      	movs	r0, #1
30006c46:	4a15      	ldr	r2, [pc, #84]	; (30006c9c <FLASH_SetStatusBits+0x2c4>)
30006c48:	2300      	movs	r3, #0
30006c4a:	f421 2170 	bic.w	r1, r1, #983040	; 0xf0000
30006c4e:	f8c4 1118 	str.w	r1, [r4, #280]	; 0x118
30006c52:	6060      	str	r0, [r4, #4]
30006c54:	f8c4 9130 	str.w	r9, [r4, #304]	; 0x130
30006c58:	6821      	ldr	r1, [r4, #0]
30006c5a:	400a      	ands	r2, r1
30006c5c:	f442 7240 	orr.w	r2, r2, #768	; 0x300
30006c60:	6022      	str	r2, [r4, #0]
30006c62:	f884 b060 	strb.w	fp, [r4, #96]	; 0x60
30006c66:	60a0      	str	r0, [r4, #8]
30006c68:	b94b      	cbnz	r3, 30006c7e <FLASH_SetStatusBits+0x2a6>
30006c6a:	6aa3      	ldr	r3, [r4, #40]	; 0x28
30006c6c:	071a      	lsls	r2, r3, #28
30006c6e:	d5fc      	bpl.n	30006c6a <FLASH_SetStatusBits+0x292>
30006c70:	f894 8060 	ldrb.w	r8, [r4, #96]	; 0x60
30006c74:	462b      	mov	r3, r5
30006c76:	fa5f f888 	uxtb.w	r8, r8
30006c7a:	2b00      	cmp	r3, #0
30006c7c:	d0f5      	beq.n	30006c6a <FLASH_SetStatusBits+0x292>
30006c7e:	68a3      	ldr	r3, [r4, #8]
30006c80:	07db      	lsls	r3, r3, #31
30006c82:	d4fc      	bmi.n	30006c7e <FLASH_SetStatusBits+0x2a6>
30006c84:	7939      	ldrb	r1, [r7, #4]
30006c86:	4650      	mov	r0, sl
30006c88:	47b0      	blx	r6
30006c8a:	693b      	ldr	r3, [r7, #16]
30006c8c:	ea18 0f03 	tst.w	r8, r3
30006c90:	d1d1      	bne.n	30006c36 <FLASH_SetStatusBits+0x25e>
30006c92:	e73c      	b.n	30006b0e <FLASH_SetStatusBits+0x136>
30006c94:	2001c01c 	.word	0x2001c01c
30006c98:	00009c99 	.word	0x00009c99
30006c9c:	fff0fcff 	.word	0xfff0fcff
30006ca0:	ffc0fcff 	.word	0xffc0fcff
30006ca4:	4323      	orrs	r3, r4
30006ca6:	b2db      	uxtb	r3, r3
30006ca8:	f88d 300c 	strb.w	r3, [sp, #12]
30006cac:	69bb      	ldr	r3, [r7, #24]
30006cae:	2b00      	cmp	r3, #0
30006cb0:	f43f aeed 	beq.w	30006a8e <FLASH_SetStatusBits+0xb6>
30006cb4:	2100      	movs	r1, #0
30006cb6:	4820      	ldr	r0, [pc, #128]	; (30006d38 <FLASH_SetStatusBits+0x360>)
30006cb8:	f897 9057 	ldrb.w	r9, [r7, #87]	; 0x57
30006cbc:	47b0      	blx	r6
30006cbe:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30006cc2:	f04f 0c01 	mov.w	ip, #1
30006cc6:	2300      	movs	r3, #0
30006cc8:	491c      	ldr	r1, [pc, #112]	; (30006d3c <FLASH_SetStatusBits+0x364>)
30006cca:	f8d2 0118 	ldr.w	r0, [r2, #280]	; 0x118
30006cce:	f420 2070 	bic.w	r0, r0, #983040	; 0xf0000
30006cd2:	f8c2 0118 	str.w	r0, [r2, #280]	; 0x118
30006cd6:	f8c2 c004 	str.w	ip, [r2, #4]
30006cda:	f8c2 3130 	str.w	r3, [r2, #304]	; 0x130
30006cde:	6810      	ldr	r0, [r2, #0]
30006ce0:	4001      	ands	r1, r0
30006ce2:	4618      	mov	r0, r3
30006ce4:	f441 7140 	orr.w	r1, r1, #768	; 0x300
30006ce8:	6011      	str	r1, [r2, #0]
30006cea:	f882 9060 	strb.w	r9, [r2, #96]	; 0x60
30006cee:	f8c2 c008 	str.w	ip, [r2, #8]
30006cf2:	f89d 100d 	ldrb.w	r1, [sp, #13]
30006cf6:	b94b      	cbnz	r3, 30006d0c <FLASH_SetStatusBits+0x334>
30006cf8:	6a93      	ldr	r3, [r2, #40]	; 0x28
30006cfa:	0718      	lsls	r0, r3, #28
30006cfc:	d5fc      	bpl.n	30006cf8 <FLASH_SetStatusBits+0x320>
30006cfe:	f892 1060 	ldrb.w	r1, [r2, #96]	; 0x60
30006d02:	4643      	mov	r3, r8
30006d04:	2001      	movs	r0, #1
30006d06:	b2c9      	uxtb	r1, r1
30006d08:	2b00      	cmp	r3, #0
30006d0a:	d0f5      	beq.n	30006cf8 <FLASH_SetStatusBits+0x320>
30006d0c:	b108      	cbz	r0, 30006d12 <FLASH_SetStatusBits+0x33a>
30006d0e:	f88d 100d 	strb.w	r1, [sp, #13]
30006d12:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30006d16:	6893      	ldr	r3, [r2, #8]
30006d18:	07d9      	lsls	r1, r3, #31
30006d1a:	d4fc      	bmi.n	30006d16 <FLASH_SetStatusBits+0x33e>
30006d1c:	7939      	ldrb	r1, [r7, #4]
30006d1e:	0a24      	lsrs	r4, r4, #8
30006d20:	4805      	ldr	r0, [pc, #20]	; (30006d38 <FLASH_SetStatusBits+0x360>)
30006d22:	47b0      	blx	r6
30006d24:	f89d 300d 	ldrb.w	r3, [sp, #13]
30006d28:	2d01      	cmp	r5, #1
30006d2a:	bf0c      	ite	eq
30006d2c:	4323      	orreq	r3, r4
30006d2e:	43a3      	bicne	r3, r4
30006d30:	2402      	movs	r4, #2
30006d32:	f88d 300d 	strb.w	r3, [sp, #13]
30006d36:	e6ab      	b.n	30006a90 <FLASH_SetStatusBits+0xb8>
30006d38:	2001c01c 	.word	0x2001c01c
30006d3c:	fff0fcff 	.word	0xfff0fcff

30006d40 <FLASH_Erase>:
30006d40:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30006d44:	2802      	cmp	r0, #2
30006d46:	b085      	sub	sp, #20
30006d48:	9000      	str	r0, [sp, #0]
30006d4a:	f200 80ed 	bhi.w	30006f28 <FLASH_Erase+0x1e8>
30006d4e:	f8df b1f4 	ldr.w	fp, [pc, #500]	; 30006f44 <FLASH_Erase+0x204>
30006d52:	f3c1 4407 	ubfx	r4, r1, #16, #8
30006d56:	f3c1 2007 	ubfx	r0, r1, #8, #8
30006d5a:	b2ca      	uxtb	r2, r1
30006d5c:	f89b 3062 	ldrb.w	r3, [fp, #98]	; 0x62
30006d60:	2b04      	cmp	r3, #4
30006d62:	9301      	str	r3, [sp, #4]
30006d64:	f000 80bd 	beq.w	30006ee2 <FLASH_Erase+0x1a2>
30006d68:	2303      	movs	r3, #3
30006d6a:	f88d 400c 	strb.w	r4, [sp, #12]
30006d6e:	f88d 000d 	strb.w	r0, [sp, #13]
30006d72:	9301      	str	r3, [sp, #4]
30006d74:	f88d 200e 	strb.w	r2, [sp, #14]
30006d78:	f3bf 8f4f 	dsb	sy
30006d7c:	f3bf 8f6f 	isb	sy
30006d80:	f04f 4188 	mov.w	r1, #1140850688	; 0x44000000
30006d84:	6a8a      	ldr	r2, [r1, #40]	; 0x28
30006d86:	07d0      	lsls	r0, r2, #31
30006d88:	d4fc      	bmi.n	30006d84 <FLASH_Erase+0x44>
30006d8a:	680a      	ldr	r2, [r1, #0]
30006d8c:	f042 4200 	orr.w	r2, r2, #2147483648	; 0x80000000
30006d90:	600a      	str	r2, [r1, #0]
30006d92:	f04f 4188 	mov.w	r1, #1140850688	; 0x44000000
30006d96:	680a      	ldr	r2, [r1, #0]
30006d98:	2a00      	cmp	r2, #0
30006d9a:	dafc      	bge.n	30006d96 <FLASH_Erase+0x56>
30006d9c:	4e65      	ldr	r6, [pc, #404]	; (30006f34 <FLASH_Erase+0x1f4>)
30006d9e:	f04f 0900 	mov.w	r9, #0
30006da2:	f8df a1a0 	ldr.w	sl, [pc, #416]	; 30006f44 <FLASH_Erase+0x204>
30006da6:	f04f 4488 	mov.w	r4, #1140850688	; 0x44000000
30006daa:	2701      	movs	r7, #1
30006dac:	2100      	movs	r1, #0
30006dae:	4650      	mov	r0, sl
30006db0:	f89b 5056 	ldrb.w	r5, [fp, #86]	; 0x56
30006db4:	47b0      	blx	r6
30006db6:	f8d4 1118 	ldr.w	r1, [r4, #280]	; 0x118
30006dba:	4a5f      	ldr	r2, [pc, #380]	; (30006f38 <FLASH_Erase+0x1f8>)
30006dbc:	f421 2170 	bic.w	r1, r1, #983040	; 0xf0000
30006dc0:	f8c4 1118 	str.w	r1, [r4, #280]	; 0x118
30006dc4:	6067      	str	r7, [r4, #4]
30006dc6:	f8c4 9130 	str.w	r9, [r4, #304]	; 0x130
30006dca:	6821      	ldr	r1, [r4, #0]
30006dcc:	400a      	ands	r2, r1
30006dce:	f442 7240 	orr.w	r2, r2, #768	; 0x300
30006dd2:	6022      	str	r2, [r4, #0]
30006dd4:	f884 5060 	strb.w	r5, [r4, #96]	; 0x60
30006dd8:	60a7      	str	r7, [r4, #8]
30006dda:	6aa2      	ldr	r2, [r4, #40]	; 0x28
30006ddc:	0712      	lsls	r2, r2, #28
30006dde:	d5fc      	bpl.n	30006dda <FLASH_Erase+0x9a>
30006de0:	f894 5060 	ldrb.w	r5, [r4, #96]	; 0x60
30006de4:	f04f 0801 	mov.w	r8, #1
30006de8:	b2ed      	uxtb	r5, r5
30006dea:	68a2      	ldr	r2, [r4, #8]
30006dec:	07d3      	lsls	r3, r2, #31
30006dee:	d4fc      	bmi.n	30006dea <FLASH_Erase+0xaa>
30006df0:	f89b 1004 	ldrb.w	r1, [fp, #4]
30006df4:	4650      	mov	r0, sl
30006df6:	47b0      	blx	r6
30006df8:	f8db 2010 	ldr.w	r2, [fp, #16]
30006dfc:	402a      	ands	r2, r5
30006dfe:	d1d5      	bne.n	30006dac <FLASH_Erase+0x6c>
30006e00:	f8d4 5118 	ldr.w	r5, [r4, #280]	; 0x118
30006e04:	494d      	ldr	r1, [pc, #308]	; (30006f3c <FLASH_Erase+0x1fc>)
30006e06:	6820      	ldr	r0, [r4, #0]
30006e08:	6827      	ldr	r7, [r4, #0]
30006e0a:	4039      	ands	r1, r7
30006e0c:	6021      	str	r1, [r4, #0]
30006e0e:	f8d4 1118 	ldr.w	r1, [r4, #280]	; 0x118
30006e12:	f421 2170 	bic.w	r1, r1, #983040	; 0xf0000
30006e16:	f8c4 1118 	str.w	r1, [r4, #280]	; 0x118
30006e1a:	6062      	str	r2, [r4, #4]
30006e1c:	f8c4 2130 	str.w	r2, [r4, #304]	; 0x130
30006e20:	f89b 2054 	ldrb.w	r2, [fp, #84]	; 0x54
30006e24:	f884 2060 	strb.w	r2, [r4, #96]	; 0x60
30006e28:	2201      	movs	r2, #1
30006e2a:	60a2      	str	r2, [r4, #8]
30006e2c:	f04f 4488 	mov.w	r4, #1140850688	; 0x44000000
30006e30:	68a2      	ldr	r2, [r4, #8]
30006e32:	07d7      	lsls	r7, r2, #31
30006e34:	d4fc      	bmi.n	30006e30 <FLASH_Erase+0xf0>
30006e36:	f8c4 5118 	str.w	r5, [r4, #280]	; 0x118
30006e3a:	6020      	str	r0, [r4, #0]
30006e3c:	2003      	movs	r0, #3
30006e3e:	f7ff facb 	bl	300063d8 <FLASH_WaitBusy_InUserMode>
30006e42:	9b00      	ldr	r3, [sp, #0]
30006e44:	2b00      	cmp	r3, #0
30006e46:	d04f      	beq.n	30006ee8 <FLASH_Erase+0x1a8>
30006e48:	9b00      	ldr	r3, [sp, #0]
30006e4a:	aa03      	add	r2, sp, #12
30006e4c:	9901      	ldr	r1, [sp, #4]
30006e4e:	2b01      	cmp	r3, #1
30006e50:	bf0c      	ite	eq
30006e52:	f89b 005b 	ldrbeq.w	r0, [fp, #91]	; 0x5b
30006e56:	f89b 005c 	ldrbne.w	r0, [fp, #92]	; 0x5c
30006e5a:	f7ff fb9b 	bl	30006594 <FLASH_TxCmd_InUserMode>
30006e5e:	2500      	movs	r5, #0
30006e60:	f8df 90e0 	ldr.w	r9, [pc, #224]	; 30006f44 <FLASH_Erase+0x204>
30006e64:	f04f 4488 	mov.w	r4, #1140850688	; 0x44000000
30006e68:	462f      	mov	r7, r5
30006e6a:	2100      	movs	r1, #0
30006e6c:	4648      	mov	r0, r9
30006e6e:	f89b a056 	ldrb.w	sl, [fp, #86]	; 0x56
30006e72:	47b0      	blx	r6
30006e74:	f8d4 0118 	ldr.w	r0, [r4, #280]	; 0x118
30006e78:	2301      	movs	r3, #1
30006e7a:	492f      	ldr	r1, [pc, #188]	; (30006f38 <FLASH_Erase+0x1f8>)
30006e7c:	2200      	movs	r2, #0
30006e7e:	f420 2070 	bic.w	r0, r0, #983040	; 0xf0000
30006e82:	f8c4 0118 	str.w	r0, [r4, #280]	; 0x118
30006e86:	6063      	str	r3, [r4, #4]
30006e88:	f8c4 7130 	str.w	r7, [r4, #304]	; 0x130
30006e8c:	6820      	ldr	r0, [r4, #0]
30006e8e:	4001      	ands	r1, r0
30006e90:	f441 7140 	orr.w	r1, r1, #768	; 0x300
30006e94:	6021      	str	r1, [r4, #0]
30006e96:	f884 a060 	strb.w	sl, [r4, #96]	; 0x60
30006e9a:	60a3      	str	r3, [r4, #8]
30006e9c:	b942      	cbnz	r2, 30006eb0 <FLASH_Erase+0x170>
30006e9e:	6aa2      	ldr	r2, [r4, #40]	; 0x28
30006ea0:	0712      	lsls	r2, r2, #28
30006ea2:	d5fc      	bpl.n	30006e9e <FLASH_Erase+0x15e>
30006ea4:	f894 5060 	ldrb.w	r5, [r4, #96]	; 0x60
30006ea8:	4642      	mov	r2, r8
30006eaa:	b2ed      	uxtb	r5, r5
30006eac:	2a00      	cmp	r2, #0
30006eae:	d0f6      	beq.n	30006e9e <FLASH_Erase+0x15e>
30006eb0:	68a2      	ldr	r2, [r4, #8]
30006eb2:	07d3      	lsls	r3, r2, #31
30006eb4:	d4fc      	bmi.n	30006eb0 <FLASH_Erase+0x170>
30006eb6:	f89b 1004 	ldrb.w	r1, [fp, #4]
30006eba:	4648      	mov	r0, r9
30006ebc:	47b0      	blx	r6
30006ebe:	f8db 2010 	ldr.w	r2, [fp, #16]
30006ec2:	4215      	tst	r5, r2
30006ec4:	d1d1      	bne.n	30006e6a <FLASH_Erase+0x12a>
30006ec6:	6823      	ldr	r3, [r4, #0]
30006ec8:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30006ecc:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
30006ed0:	6023      	str	r3, [r4, #0]
30006ed2:	6813      	ldr	r3, [r2, #0]
30006ed4:	2b00      	cmp	r3, #0
30006ed6:	dbfc      	blt.n	30006ed2 <FLASH_Erase+0x192>
30006ed8:	f3bf 8f6f 	isb	sy
30006edc:	b005      	add	sp, #20
30006ede:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30006ee2:	ba09      	rev	r1, r1
30006ee4:	9103      	str	r1, [sp, #12]
30006ee6:	e747      	b.n	30006d78 <FLASH_Erase+0x38>
30006ee8:	f8d4 5118 	ldr.w	r5, [r4, #280]	; 0x118
30006eec:	4a13      	ldr	r2, [pc, #76]	; (30006f3c <FLASH_Erase+0x1fc>)
30006eee:	6820      	ldr	r0, [r4, #0]
30006ef0:	6821      	ldr	r1, [r4, #0]
30006ef2:	400a      	ands	r2, r1
30006ef4:	f04f 4188 	mov.w	r1, #1140850688	; 0x44000000
30006ef8:	6022      	str	r2, [r4, #0]
30006efa:	f8d4 2118 	ldr.w	r2, [r4, #280]	; 0x118
30006efe:	f422 2270 	bic.w	r2, r2, #983040	; 0xf0000
30006f02:	f8c4 2118 	str.w	r2, [r4, #280]	; 0x118
30006f06:	9b00      	ldr	r3, [sp, #0]
30006f08:	6063      	str	r3, [r4, #4]
30006f0a:	f8c4 3130 	str.w	r3, [r4, #304]	; 0x130
30006f0e:	f89b 205a 	ldrb.w	r2, [fp, #90]	; 0x5a
30006f12:	f884 2060 	strb.w	r2, [r4, #96]	; 0x60
30006f16:	2201      	movs	r2, #1
30006f18:	60a2      	str	r2, [r4, #8]
30006f1a:	688a      	ldr	r2, [r1, #8]
30006f1c:	07d4      	lsls	r4, r2, #31
30006f1e:	d4fc      	bmi.n	30006f1a <FLASH_Erase+0x1da>
30006f20:	f8c1 5118 	str.w	r5, [r1, #280]	; 0x118
30006f24:	6008      	str	r0, [r1, #0]
30006f26:	e79a      	b.n	30006e5e <FLASH_Erase+0x11e>
30006f28:	f640 21f6 	movw	r1, #2806	; 0xaf6
30006f2c:	4804      	ldr	r0, [pc, #16]	; (30006f40 <FLASH_Erase+0x200>)
30006f2e:	f004 ffd7 	bl	3000bee0 <io_assert_failed>
30006f32:	bf00      	nop
30006f34:	00009c99 	.word	0x00009c99
30006f38:	fff0fcff 	.word	0xfff0fcff
30006f3c:	ffc0fcff 	.word	0xffc0fcff
30006f40:	3000e7e4 	.word	0x3000e7e4
30006f44:	2001c01c 	.word	0x2001c01c

30006f48 <FLASH_RxBasic>:
30006f48:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
30006f4c:	4682      	mov	sl, r0
30006f4e:	4689      	mov	r9, r1
30006f50:	4615      	mov	r5, r2
30006f52:	461c      	mov	r4, r3
30006f54:	f3bf 8f4f 	dsb	sy
30006f58:	f3bf 8f6f 	isb	sy
30006f5c:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30006f60:	6a93      	ldr	r3, [r2, #40]	; 0x28
30006f62:	07de      	lsls	r6, r3, #31
30006f64:	d4fc      	bmi.n	30006f60 <FLASH_RxBasic+0x18>
30006f66:	6813      	ldr	r3, [r2, #0]
30006f68:	f04f 4688 	mov.w	r6, #1140850688	; 0x44000000
30006f6c:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
30006f70:	6013      	str	r3, [r2, #0]
30006f72:	6833      	ldr	r3, [r6, #0]
30006f74:	2b00      	cmp	r3, #0
30006f76:	dafc      	bge.n	30006f72 <FLASH_RxBasic+0x2a>
30006f78:	f8df 80d4 	ldr.w	r8, [pc, #212]	; 30007050 <FLASH_RxBasic+0x108>
30006f7c:	2100      	movs	r1, #0
30006f7e:	4f32      	ldr	r7, [pc, #200]	; (30007048 <FLASH_RxBasic+0x100>)
30006f80:	4640      	mov	r0, r8
30006f82:	47b8      	blx	r7
30006f84:	2300      	movs	r3, #0
30006f86:	f025 427f 	bic.w	r2, r5, #4278190080	; 0xff000000
30006f8a:	f888 3063 	strb.w	r3, [r8, #99]	; 0x63
30006f8e:	6072      	str	r2, [r6, #4]
30006f90:	f8c6 3130 	str.w	r3, [r6, #304]	; 0x130
30006f94:	f898 1062 	ldrb.w	r1, [r8, #98]	; 0x62
30006f98:	f8d6 2118 	ldr.w	r2, [r6, #280]	; 0x118
30006f9c:	040b      	lsls	r3, r1, #16
30006f9e:	2904      	cmp	r1, #4
30006fa0:	f422 2270 	bic.w	r2, r2, #983040	; 0xf0000
30006fa4:	f403 2370 	and.w	r3, r3, #983040	; 0xf0000
30006fa8:	ea43 0302 	orr.w	r3, r3, r2
30006fac:	f8c6 3118 	str.w	r3, [r6, #280]	; 0x118
30006fb0:	4b26      	ldr	r3, [pc, #152]	; (3000704c <FLASH_RxBasic+0x104>)
30006fb2:	6832      	ldr	r2, [r6, #0]
30006fb4:	ea03 0302 	and.w	r3, r3, r2
30006fb8:	f443 7340 	orr.w	r3, r3, #768	; 0x300
30006fbc:	6033      	str	r3, [r6, #0]
30006fbe:	f886 a060 	strb.w	sl, [r6, #96]	; 0x60
30006fc2:	d103      	bne.n	30006fcc <FLASH_RxBasic+0x84>
30006fc4:	ea4f 6319 	mov.w	r3, r9, lsr #24
30006fc8:	f886 3060 	strb.w	r3, [r6, #96]	; 0x60
30006fcc:	f04f 4388 	mov.w	r3, #1140850688	; 0x44000000
30006fd0:	f3c9 4207 	ubfx	r2, r9, #16, #8
30006fd4:	f883 2060 	strb.w	r2, [r3, #96]	; 0x60
30006fd8:	f3c9 2207 	ubfx	r2, r9, #8, #8
30006fdc:	fa5f f989 	uxtb.w	r9, r9
30006fe0:	f883 2060 	strb.w	r2, [r3, #96]	; 0x60
30006fe4:	f883 9060 	strb.w	r9, [r3, #96]	; 0x60
30006fe8:	f898 2063 	ldrb.w	r2, [r8, #99]	; 0x63
30006fec:	b112      	cbz	r2, 30006ff4 <FLASH_RxBasic+0xac>
30006fee:	2200      	movs	r2, #0
30006ff0:	f883 2060 	strb.w	r2, [r3, #96]	; 0x60
30006ff4:	f04f 4c88 	mov.w	ip, #1140850688	; 0x44000000
30006ff8:	1e61      	subs	r1, r4, #1
30006ffa:	2301      	movs	r3, #1
30006ffc:	194a      	adds	r2, r1, r5
30006ffe:	f8cc 3008 	str.w	r3, [ip, #8]
30007002:	e003      	b.n	3000700c <FLASH_RxBasic+0xc4>
30007004:	f8dc 3028 	ldr.w	r3, [ip, #40]	; 0x28
30007008:	0718      	lsls	r0, r3, #28
3000700a:	d417      	bmi.n	3000703c <FLASH_RxBasic+0xf4>
3000700c:	4291      	cmp	r1, r2
3000700e:	d1f9      	bne.n	30007004 <FLASH_RxBasic+0xbc>
30007010:	f04f 4488 	mov.w	r4, #1140850688	; 0x44000000
30007014:	68a3      	ldr	r3, [r4, #8]
30007016:	07db      	lsls	r3, r3, #31
30007018:	d4fc      	bmi.n	30007014 <FLASH_RxBasic+0xcc>
3000701a:	f898 1004 	ldrb.w	r1, [r8, #4]
3000701e:	480c      	ldr	r0, [pc, #48]	; (30007050 <FLASH_RxBasic+0x108>)
30007020:	47b8      	blx	r7
30007022:	6823      	ldr	r3, [r4, #0]
30007024:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30007028:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
3000702c:	6023      	str	r3, [r4, #0]
3000702e:	6813      	ldr	r3, [r2, #0]
30007030:	2b00      	cmp	r3, #0
30007032:	dbfc      	blt.n	3000702e <FLASH_RxBasic+0xe6>
30007034:	f3bf 8f6f 	isb	sy
30007038:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
3000703c:	f89c 3060 	ldrb.w	r3, [ip, #96]	; 0x60
30007040:	f801 3f01 	strb.w	r3, [r1, #1]!
30007044:	e7e2      	b.n	3000700c <FLASH_RxBasic+0xc4>
30007046:	bf00      	nop
30007048:	00009c99 	.word	0x00009c99
3000704c:	fff0fcff 	.word	0xfff0fcff
30007050:	2001c01c 	.word	0x2001c01c

30007054 <FLASH_TxBasic>:
30007054:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30007058:	b085      	sub	sp, #20
3000705a:	4616      	mov	r6, r2
3000705c:	e9cd 1002 	strd	r1, r0, [sp, #8]
30007060:	9301      	str	r3, [sp, #4]
30007062:	f3bf 8f4f 	dsb	sy
30007066:	f3bf 8f6f 	isb	sy
3000706a:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
3000706e:	6a93      	ldr	r3, [r2, #40]	; 0x28
30007070:	07dc      	lsls	r4, r3, #31
30007072:	d4fc      	bmi.n	3000706e <FLASH_TxBasic+0x1a>
30007074:	6813      	ldr	r3, [r2, #0]
30007076:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
3000707a:	6013      	str	r3, [r2, #0]
3000707c:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30007080:	6813      	ldr	r3, [r2, #0]
30007082:	2b00      	cmp	r3, #0
30007084:	dafc      	bge.n	30007080 <FLASH_TxBasic+0x2c>
30007086:	f8df b200 	ldr.w	fp, [pc, #512]	; 30007288 <FLASH_TxBasic+0x234>
3000708a:	f04f 0900 	mov.w	r9, #0
3000708e:	4d7b      	ldr	r5, [pc, #492]	; (3000727c <FLASH_TxBasic+0x228>)
30007090:	f04f 4a88 	mov.w	sl, #1140850688	; 0x44000000
30007094:	2701      	movs	r7, #1
30007096:	2100      	movs	r1, #0
30007098:	4658      	mov	r0, fp
3000709a:	f89b 4056 	ldrb.w	r4, [fp, #86]	; 0x56
3000709e:	47a8      	blx	r5
300070a0:	f8da 2118 	ldr.w	r2, [sl, #280]	; 0x118
300070a4:	4b76      	ldr	r3, [pc, #472]	; (30007280 <FLASH_TxBasic+0x22c>)
300070a6:	f422 2270 	bic.w	r2, r2, #983040	; 0xf0000
300070aa:	f8ca 2118 	str.w	r2, [sl, #280]	; 0x118
300070ae:	f8ca 7004 	str.w	r7, [sl, #4]
300070b2:	f8ca 9130 	str.w	r9, [sl, #304]	; 0x130
300070b6:	f8da 2000 	ldr.w	r2, [sl]
300070ba:	4013      	ands	r3, r2
300070bc:	f443 7340 	orr.w	r3, r3, #768	; 0x300
300070c0:	f8ca 3000 	str.w	r3, [sl]
300070c4:	f88a 4060 	strb.w	r4, [sl, #96]	; 0x60
300070c8:	f8ca 7008 	str.w	r7, [sl, #8]
300070cc:	f8da 3028 	ldr.w	r3, [sl, #40]	; 0x28
300070d0:	0718      	lsls	r0, r3, #28
300070d2:	d5fb      	bpl.n	300070cc <FLASH_TxBasic+0x78>
300070d4:	f89a 4060 	ldrb.w	r4, [sl, #96]	; 0x60
300070d8:	f04f 0801 	mov.w	r8, #1
300070dc:	b2e4      	uxtb	r4, r4
300070de:	f8da 3008 	ldr.w	r3, [sl, #8]
300070e2:	07d9      	lsls	r1, r3, #31
300070e4:	d4fb      	bmi.n	300070de <FLASH_TxBasic+0x8a>
300070e6:	f89b 1004 	ldrb.w	r1, [fp, #4]
300070ea:	4658      	mov	r0, fp
300070ec:	47a8      	blx	r5
300070ee:	f8db 1010 	ldr.w	r1, [fp, #16]
300070f2:	4021      	ands	r1, r4
300070f4:	d1cf      	bne.n	30007096 <FLASH_TxBasic+0x42>
300070f6:	f8da 2118 	ldr.w	r2, [sl, #280]	; 0x118
300070fa:	4c62      	ldr	r4, [pc, #392]	; (30007284 <FLASH_TxBasic+0x230>)
300070fc:	f8da 0000 	ldr.w	r0, [sl]
30007100:	f8da 7000 	ldr.w	r7, [sl]
30007104:	403c      	ands	r4, r7
30007106:	f04f 4788 	mov.w	r7, #1140850688	; 0x44000000
3000710a:	f8ca 4000 	str.w	r4, [sl]
3000710e:	f8da 4118 	ldr.w	r4, [sl, #280]	; 0x118
30007112:	f424 2470 	bic.w	r4, r4, #983040	; 0xf0000
30007116:	f8ca 4118 	str.w	r4, [sl, #280]	; 0x118
3000711a:	f8ca 1004 	str.w	r1, [sl, #4]
3000711e:	f8ca 1130 	str.w	r1, [sl, #304]	; 0x130
30007122:	f89b 1054 	ldrb.w	r1, [fp, #84]	; 0x54
30007126:	f88a 1060 	strb.w	r1, [sl, #96]	; 0x60
3000712a:	2101      	movs	r1, #1
3000712c:	f8ca 1008 	str.w	r1, [sl, #8]
30007130:	68bc      	ldr	r4, [r7, #8]
30007132:	f014 0401 	ands.w	r4, r4, #1
30007136:	d1fb      	bne.n	30007130 <FLASH_TxBasic+0xdc>
30007138:	f8c7 2118 	str.w	r2, [r7, #280]	; 0x118
3000713c:	6038      	str	r0, [r7, #0]
3000713e:	2003      	movs	r0, #3
30007140:	f7ff f94a 	bl	300063d8 <FLASH_WaitBusy_InUserMode>
30007144:	6839      	ldr	r1, [r7, #0]
30007146:	4b4f      	ldr	r3, [pc, #316]	; (30007284 <FLASH_TxBasic+0x230>)
30007148:	6838      	ldr	r0, [r7, #0]
3000714a:	4003      	ands	r3, r0
3000714c:	603b      	str	r3, [r7, #0]
3000714e:	f8d7 3118 	ldr.w	r3, [r7, #280]	; 0x118
30007152:	f89b 0062 	ldrb.w	r0, [fp, #98]	; 0x62
30007156:	f423 2c70 	bic.w	ip, r3, #983040	; 0xf0000
3000715a:	0403      	lsls	r3, r0, #16
3000715c:	2804      	cmp	r0, #4
3000715e:	f403 2370 	and.w	r3, r3, #983040	; 0xf0000
30007162:	ea43 030c 	orr.w	r3, r3, ip
30007166:	f8c7 3118 	str.w	r3, [r7, #280]	; 0x118
3000716a:	f026 437f 	bic.w	r3, r6, #4278190080	; 0xff000000
3000716e:	607c      	str	r4, [r7, #4]
30007170:	f8c7 3130 	str.w	r3, [r7, #304]	; 0x130
30007174:	9b03      	ldr	r3, [sp, #12]
30007176:	f887 3060 	strb.w	r3, [r7, #96]	; 0x60
3000717a:	d103      	bne.n	30007184 <FLASH_TxBasic+0x130>
3000717c:	9b02      	ldr	r3, [sp, #8]
3000717e:	0e1b      	lsrs	r3, r3, #24
30007180:	f887 3060 	strb.w	r3, [r7, #96]	; 0x60
30007184:	9a02      	ldr	r2, [sp, #8]
30007186:	f04f 4388 	mov.w	r3, #1140850688	; 0x44000000
3000718a:	f3c2 4007 	ubfx	r0, r2, #16, #8
3000718e:	fa5f fa82 	uxtb.w	sl, r2
30007192:	f883 0060 	strb.w	r0, [r3, #96]	; 0x60
30007196:	f3c2 2007 	ubfx	r0, r2, #8, #8
3000719a:	f883 0060 	strb.w	r0, [r3, #96]	; 0x60
3000719e:	f883 a060 	strb.w	sl, [r3, #96]	; 0x60
300071a2:	2e00      	cmp	r6, #0
300071a4:	d067      	beq.n	30007276 <FLASH_TxBasic+0x222>
300071a6:	9a01      	ldr	r2, [sp, #4]
300071a8:	1e57      	subs	r7, r2, #1
300071aa:	e006      	b.n	300071ba <FLASH_TxBasic+0x166>
300071ac:	3401      	adds	r4, #1
300071ae:	f817 2f01 	ldrb.w	r2, [r7, #1]!
300071b2:	42a6      	cmp	r6, r4
300071b4:	f883 2060 	strb.w	r2, [r3, #96]	; 0x60
300071b8:	d002      	beq.n	300071c0 <FLASH_TxBasic+0x16c>
300071ba:	6a9a      	ldr	r2, [r3, #40]	; 0x28
300071bc:	0792      	lsls	r2, r2, #30
300071be:	d4f5      	bmi.n	300071ac <FLASH_TxBasic+0x158>
300071c0:	2301      	movs	r3, #1
300071c2:	f04f 4088 	mov.w	r0, #1140850688	; 0x44000000
300071c6:	6083      	str	r3, [r0, #8]
300071c8:	1e63      	subs	r3, r4, #1
300071ca:	9a01      	ldr	r2, [sp, #4]
300071cc:	441a      	add	r2, r3
300071ce:	e002      	b.n	300071d6 <FLASH_TxBasic+0x182>
300071d0:	6a83      	ldr	r3, [r0, #40]	; 0x28
300071d2:	079f      	lsls	r7, r3, #30
300071d4:	d449      	bmi.n	3000726a <FLASH_TxBasic+0x216>
300071d6:	42b4      	cmp	r4, r6
300071d8:	d3fa      	bcc.n	300071d0 <FLASH_TxBasic+0x17c>
300071da:	f04f 4388 	mov.w	r3, #1140850688	; 0x44000000
300071de:	689e      	ldr	r6, [r3, #8]
300071e0:	f016 0601 	ands.w	r6, r6, #1
300071e4:	d1fb      	bne.n	300071de <FLASH_TxBasic+0x18a>
300071e6:	4637      	mov	r7, r6
300071e8:	f8df 909c 	ldr.w	r9, [pc, #156]	; 30007288 <FLASH_TxBasic+0x234>
300071ec:	f04f 4488 	mov.w	r4, #1140850688	; 0x44000000
300071f0:	6019      	str	r1, [r3, #0]
300071f2:	2100      	movs	r1, #0
300071f4:	4648      	mov	r0, r9
300071f6:	f89b a056 	ldrb.w	sl, [fp, #86]	; 0x56
300071fa:	47a8      	blx	r5
300071fc:	f8d4 1118 	ldr.w	r1, [r4, #280]	; 0x118
30007200:	2001      	movs	r0, #1
30007202:	4a1f      	ldr	r2, [pc, #124]	; (30007280 <FLASH_TxBasic+0x22c>)
30007204:	2300      	movs	r3, #0
30007206:	f421 2170 	bic.w	r1, r1, #983040	; 0xf0000
3000720a:	f8c4 1118 	str.w	r1, [r4, #280]	; 0x118
3000720e:	6060      	str	r0, [r4, #4]
30007210:	f8c4 7130 	str.w	r7, [r4, #304]	; 0x130
30007214:	6821      	ldr	r1, [r4, #0]
30007216:	400a      	ands	r2, r1
30007218:	f442 7240 	orr.w	r2, r2, #768	; 0x300
3000721c:	6022      	str	r2, [r4, #0]
3000721e:	f884 a060 	strb.w	sl, [r4, #96]	; 0x60
30007222:	60a0      	str	r0, [r4, #8]
30007224:	b943      	cbnz	r3, 30007238 <FLASH_TxBasic+0x1e4>
30007226:	6aa3      	ldr	r3, [r4, #40]	; 0x28
30007228:	071a      	lsls	r2, r3, #28
3000722a:	d5fc      	bpl.n	30007226 <FLASH_TxBasic+0x1d2>
3000722c:	f894 6060 	ldrb.w	r6, [r4, #96]	; 0x60
30007230:	4643      	mov	r3, r8
30007232:	b2f6      	uxtb	r6, r6
30007234:	2b00      	cmp	r3, #0
30007236:	d0f6      	beq.n	30007226 <FLASH_TxBasic+0x1d2>
30007238:	68a3      	ldr	r3, [r4, #8]
3000723a:	07db      	lsls	r3, r3, #31
3000723c:	d4fc      	bmi.n	30007238 <FLASH_TxBasic+0x1e4>
3000723e:	f89b 1004 	ldrb.w	r1, [fp, #4]
30007242:	4648      	mov	r0, r9
30007244:	47a8      	blx	r5
30007246:	f8db 3010 	ldr.w	r3, [fp, #16]
3000724a:	421e      	tst	r6, r3
3000724c:	d1d1      	bne.n	300071f2 <FLASH_TxBasic+0x19e>
3000724e:	6823      	ldr	r3, [r4, #0]
30007250:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
30007254:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
30007258:	6023      	str	r3, [r4, #0]
3000725a:	6813      	ldr	r3, [r2, #0]
3000725c:	2b00      	cmp	r3, #0
3000725e:	dbfc      	blt.n	3000725a <FLASH_TxBasic+0x206>
30007260:	f3bf 8f6f 	isb	sy
30007264:	b005      	add	sp, #20
30007266:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
3000726a:	f812 3f01 	ldrb.w	r3, [r2, #1]!
3000726e:	3401      	adds	r4, #1
30007270:	f880 3060 	strb.w	r3, [r0, #96]	; 0x60
30007274:	e7af      	b.n	300071d6 <FLASH_TxBasic+0x182>
30007276:	4634      	mov	r4, r6
30007278:	e7a2      	b.n	300071c0 <FLASH_TxBasic+0x16c>
3000727a:	bf00      	nop
3000727c:	00009c99 	.word	0x00009c99
30007280:	fff0fcff 	.word	0xfff0fcff
30007284:	ffc0fcff 	.word	0xffc0fcff
30007288:	2001c01c 	.word	0x2001c01c

3000728c <IPC_SEMTake>:
3000728c:	280f      	cmp	r0, #15
3000728e:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
30007292:	d83e      	bhi.n	30007312 <IPC_SEMTake+0x86>
30007294:	4b21      	ldr	r3, [pc, #132]	; (3000731c <IPC_SEMTake+0x90>)
30007296:	4680      	mov	r8, r0
30007298:	460c      	mov	r4, r1
3000729a:	681b      	ldr	r3, [r3, #0]
3000729c:	b10b      	cbz	r3, 300072a2 <IPC_SEMTake+0x16>
3000729e:	2009      	movs	r0, #9
300072a0:	4798      	blx	r3
300072a2:	4b1f      	ldr	r3, [pc, #124]	; (30007320 <IPC_SEMTake+0x94>)
300072a4:	4798      	blx	r3
300072a6:	2802      	cmp	r0, #2
300072a8:	d914      	bls.n	300072d4 <IPC_SEMTake+0x48>
300072aa:	4d1e      	ldr	r5, [pc, #120]	; (30007324 <IPC_SEMTake+0x98>)
300072ac:	4445      	add	r5, r8
300072ae:	00ad      	lsls	r5, r5, #2
300072b0:	682b      	ldr	r3, [r5, #0]
300072b2:	b35b      	cbz	r3, 3000730c <IPC_SEMTake+0x80>
300072b4:	b15c      	cbz	r4, 300072ce <IPC_SEMTake+0x42>
300072b6:	4e1c      	ldr	r6, [pc, #112]	; (30007328 <IPC_SEMTake+0x9c>)
300072b8:	e001      	b.n	300072be <IPC_SEMTake+0x32>
300072ba:	682b      	ldr	r3, [r5, #0]
300072bc:	b333      	cbz	r3, 3000730c <IPC_SEMTake+0x80>
300072be:	6833      	ldr	r3, [r6, #0]
300072c0:	b10b      	cbz	r3, 300072c6 <IPC_SEMTake+0x3a>
300072c2:	2001      	movs	r0, #1
300072c4:	4798      	blx	r3
300072c6:	1c63      	adds	r3, r4, #1
300072c8:	d0f7      	beq.n	300072ba <IPC_SEMTake+0x2e>
300072ca:	3c01      	subs	r4, #1
300072cc:	d1f5      	bne.n	300072ba <IPC_SEMTake+0x2e>
300072ce:	2000      	movs	r0, #0
300072d0:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
300072d4:	2201      	movs	r2, #1
300072d6:	4e15      	ldr	r6, [pc, #84]	; (3000732c <IPC_SEMTake+0xa0>)
300072d8:	4f13      	ldr	r7, [pc, #76]	; (30007328 <IPC_SEMTake+0x9c>)
300072da:	fa02 f808 	lsl.w	r8, r2, r8
300072de:	e007      	b.n	300072f0 <IPC_SEMTake+0x64>
300072e0:	2c00      	cmp	r4, #0
300072e2:	d0f4      	beq.n	300072ce <IPC_SEMTake+0x42>
300072e4:	683b      	ldr	r3, [r7, #0]
300072e6:	b10b      	cbz	r3, 300072ec <IPC_SEMTake+0x60>
300072e8:	2001      	movs	r0, #1
300072ea:	4798      	blx	r3
300072ec:	3c01      	subs	r4, #1
300072ee:	d0ee      	beq.n	300072ce <IPC_SEMTake+0x42>
300072f0:	f8b6 30f4 	ldrh.w	r3, [r6, #244]	; 0xf4
300072f4:	b29b      	uxth	r3, r3
300072f6:	ea13 0f08 	tst.w	r3, r8
300072fa:	d1f1      	bne.n	300072e0 <IPC_SEMTake+0x54>
300072fc:	ea43 0308 	orr.w	r3, r3, r8
30007300:	2001      	movs	r0, #1
30007302:	b29b      	uxth	r3, r3
30007304:	f8a6 30f4 	strh.w	r3, [r6, #244]	; 0xf4
30007308:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
3000730c:	2001      	movs	r0, #1
3000730e:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
30007312:	f44f 718b 	mov.w	r1, #278	; 0x116
30007316:	4806      	ldr	r0, [pc, #24]	; (30007330 <IPC_SEMTake+0xa4>)
30007318:	f004 fde2 	bl	3000bee0 <io_assert_failed>
3000731c:	3000f8ec 	.word	0x3000f8ec
30007320:	3000b535 	.word	0x3000b535
30007324:	108020f0 	.word	0x108020f0
30007328:	3000f8e8 	.word	0x3000f8e8
3000732c:	42008200 	.word	0x42008200
30007330:	3000ce78 	.word	0x3000ce78

30007334 <IPC_SEMFree>:
30007334:	280f      	cmp	r0, #15
30007336:	b510      	push	{r4, lr}
30007338:	d81b      	bhi.n	30007372 <IPC_SEMFree+0x3e>
3000733a:	4b10      	ldr	r3, [pc, #64]	; (3000737c <IPC_SEMFree+0x48>)
3000733c:	4604      	mov	r4, r0
3000733e:	4798      	blx	r3
30007340:	2802      	cmp	r0, #2
30007342:	d810      	bhi.n	30007366 <IPC_SEMFree+0x32>
30007344:	4a0e      	ldr	r2, [pc, #56]	; (30007380 <IPC_SEMFree+0x4c>)
30007346:	2001      	movs	r0, #1
30007348:	f8b2 30f4 	ldrh.w	r3, [r2, #244]	; 0xf4
3000734c:	40a0      	lsls	r0, r4
3000734e:	b29c      	uxth	r4, r3
30007350:	ea24 0400 	bic.w	r4, r4, r0
30007354:	f8a2 40f4 	strh.w	r4, [r2, #244]	; 0xf4
30007358:	4b0a      	ldr	r3, [pc, #40]	; (30007384 <IPC_SEMFree+0x50>)
3000735a:	681b      	ldr	r3, [r3, #0]
3000735c:	b10b      	cbz	r3, 30007362 <IPC_SEMFree+0x2e>
3000735e:	2009      	movs	r0, #9
30007360:	4798      	blx	r3
30007362:	2001      	movs	r0, #1
30007364:	bd10      	pop	{r4, pc}
30007366:	4b08      	ldr	r3, [pc, #32]	; (30007388 <IPC_SEMFree+0x54>)
30007368:	2201      	movs	r2, #1
3000736a:	4423      	add	r3, r4
3000736c:	009b      	lsls	r3, r3, #2
3000736e:	601a      	str	r2, [r3, #0]
30007370:	e7f2      	b.n	30007358 <IPC_SEMFree+0x24>
30007372:	f44f 71ac 	mov.w	r1, #344	; 0x158
30007376:	4805      	ldr	r0, [pc, #20]	; (3000738c <IPC_SEMFree+0x58>)
30007378:	f004 fdb2 	bl	3000bee0 <io_assert_failed>
3000737c:	3000b535 	.word	0x3000b535
30007380:	42008200 	.word	0x42008200
30007384:	3000f8f0 	.word	0x3000f8f0
30007388:	108020f0 	.word	0x108020f0
3000738c:	3000ce78 	.word	0x3000ce78

30007390 <OTP_Read8>:
30007390:	f5b0 6f00 	cmp.w	r0, #2048	; 0x800
30007394:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
30007398:	4689      	mov	r9, r1
3000739a:	b083      	sub	sp, #12
3000739c:	d260      	bcs.n	30007460 <OTP_Read8+0xd0>
3000739e:	4b33      	ldr	r3, [pc, #204]	; (3000746c <OTP_Read8+0xdc>)
300073a0:	4680      	mov	r8, r0
300073a2:	4a33      	ldr	r2, [pc, #204]	; (30007470 <OTP_Read8+0xe0>)
300073a4:	e843 f300 	tt	r3, r3
300073a8:	f413 0f80 	tst.w	r3, #4194304	; 0x400000
300073ac:	4d31      	ldr	r5, [pc, #196]	; (30007474 <OTP_Read8+0xe4>)
300073ae:	4e32      	ldr	r6, [pc, #200]	; (30007478 <OTP_Read8+0xe8>)
300073b0:	bf18      	it	ne
300073b2:	4615      	movne	r5, r2
300073b4:	4c31      	ldr	r4, [pc, #196]	; (3000747c <OTP_Read8+0xec>)
300073b6:	e001      	b.n	300073bc <OTP_Read8+0x2c>
300073b8:	f002 ffd6 	bl	3000a368 <rtk_log_write_nano>
300073bc:	f44f 717a 	mov.w	r1, #1000	; 0x3e8
300073c0:	2002      	movs	r0, #2
300073c2:	f7ff ff63 	bl	3000728c <IPC_SEMTake>
300073c6:	2801      	cmp	r0, #1
300073c8:	4633      	mov	r3, r6
300073ca:	f04f 0245 	mov.w	r2, #69	; 0x45
300073ce:	4621      	mov	r1, r4
300073d0:	f04f 0002 	mov.w	r0, #2
300073d4:	d1f0      	bne.n	300073b8 <OTP_Read8+0x28>
300073d6:	4b2a      	ldr	r3, [pc, #168]	; (30007480 <OTP_Read8+0xf0>)
300073d8:	681a      	ldr	r2, [r3, #0]
300073da:	07d2      	lsls	r2, r2, #31
300073dc:	d52d      	bpl.n	3000743a <OTP_Read8+0xaa>
300073de:	f04f 4284 	mov.w	r2, #1107296256	; 0x42000000
300073e2:	6953      	ldr	r3, [r2, #20]
300073e4:	f023 437f 	bic.w	r3, r3, #4278190080	; 0xff000000
300073e8:	6153      	str	r3, [r2, #20]
300073ea:	ea4f 2308 	mov.w	r3, r8, lsl #8
300073ee:	602b      	str	r3, [r5, #0]
300073f0:	682b      	ldr	r3, [r5, #0]
300073f2:	2b00      	cmp	r3, #0
300073f4:	db10      	blt.n	30007418 <OTP_Read8+0x88>
300073f6:	2400      	movs	r4, #0
300073f8:	4e22      	ldr	r6, [pc, #136]	; (30007484 <OTP_Read8+0xf4>)
300073fa:	f644 6720 	movw	r7, #20000	; 0x4e20
300073fe:	e001      	b.n	30007404 <OTP_Read8+0x74>
30007400:	42bc      	cmp	r4, r7
30007402:	d01f      	beq.n	30007444 <OTP_Read8+0xb4>
30007404:	2005      	movs	r0, #5
30007406:	3401      	adds	r4, #1
30007408:	47b0      	blx	r6
3000740a:	682b      	ldr	r3, [r5, #0]
3000740c:	2b00      	cmp	r3, #0
3000740e:	daf7      	bge.n	30007400 <OTP_Read8+0x70>
30007410:	f644 6320 	movw	r3, #20000	; 0x4e20
30007414:	429c      	cmp	r4, r3
30007416:	d015      	beq.n	30007444 <OTP_Read8+0xb4>
30007418:	682b      	ldr	r3, [r5, #0]
3000741a:	2400      	movs	r4, #0
3000741c:	f889 3000 	strb.w	r3, [r9]
30007420:	f04f 4284 	mov.w	r2, #1107296256	; 0x42000000
30007424:	2002      	movs	r0, #2
30007426:	6953      	ldr	r3, [r2, #20]
30007428:	f023 437f 	bic.w	r3, r3, #4278190080	; 0xff000000
3000742c:	6153      	str	r3, [r2, #20]
3000742e:	f7ff ff81 	bl	30007334 <IPC_SEMFree>
30007432:	4620      	mov	r0, r4
30007434:	b003      	add	sp, #12
30007436:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
3000743a:	681a      	ldr	r2, [r3, #0]
3000743c:	f042 0201 	orr.w	r2, r2, #1
30007440:	601a      	str	r2, [r3, #0]
30007442:	e7cc      	b.n	300073de <OTP_Read8+0x4e>
30007444:	21ff      	movs	r1, #255	; 0xff
30007446:	f8cd 8000 	str.w	r8, [sp]
3000744a:	4b0f      	ldr	r3, [pc, #60]	; (30007488 <OTP_Read8+0xf8>)
3000744c:	2245      	movs	r2, #69	; 0x45
3000744e:	f889 1000 	strb.w	r1, [r9]
30007452:	2002      	movs	r0, #2
30007454:	4909      	ldr	r1, [pc, #36]	; (3000747c <OTP_Read8+0xec>)
30007456:	f04f 34ff 	mov.w	r4, #4294967295	; 0xffffffff
3000745a:	f002 ff85 	bl	3000a368 <rtk_log_write_nano>
3000745e:	e7df      	b.n	30007420 <OTP_Read8+0x90>
30007460:	23ff      	movs	r3, #255	; 0xff
30007462:	f04f 34ff 	mov.w	r4, #4294967295	; 0xffffffff
30007466:	700b      	strb	r3, [r1, #0]
30007468:	e7e3      	b.n	30007432 <OTP_Read8+0xa2>
3000746a:	bf00      	nop
3000746c:	3000bad9 	.word	0x3000bad9
30007470:	5200000c 	.word	0x5200000c
30007474:	42000008 	.word	0x42000008
30007478:	3000cef0 	.word	0x3000cef0
3000747c:	3000cf0c 	.word	0x3000cf0c
30007480:	42008000 	.word	0x42008000
30007484:	00009b2d 	.word	0x00009b2d
30007488:	3000cf10 	.word	0x3000cf10

3000748c <PLL_NP_ClkSet>:
3000748c:	b570      	push	{r4, r5, r6, lr}
3000748e:	4604      	mov	r4, r0
30007490:	4b1b      	ldr	r3, [pc, #108]	; (30007500 <PLL_NP_ClkSet+0x74>)
30007492:	4798      	blx	r3
30007494:	4d1b      	ldr	r5, [pc, #108]	; (30007504 <PLL_NP_ClkSet+0x78>)
30007496:	08c2      	lsrs	r2, r0, #3
30007498:	fbb4 f3f0 	udiv	r3, r4, r0
3000749c:	fb00 4c13 	mls	ip, r0, r3, r4
300074a0:	1e9c      	subs	r4, r3, #2
300074a2:	6c6b      	ldr	r3, [r5, #68]	; 0x44
300074a4:	0164      	lsls	r4, r4, #5
300074a6:	f423 63fc 	bic.w	r3, r3, #2016	; 0x7e0
300074aa:	f404 64fc 	and.w	r4, r4, #2016	; 0x7e0
300074ae:	431c      	orrs	r4, r3
300074b0:	fbbc f3f2 	udiv	r3, ip, r2
300074b4:	646c      	str	r4, [r5, #68]	; 0x44
300074b6:	041c      	lsls	r4, r3, #16
300074b8:	fb02 c013 	mls	r0, r2, r3, ip
300074bc:	2300      	movs	r3, #0
300074be:	6cee      	ldr	r6, [r5, #76]	; 0x4c
300074c0:	f404 24e0 	and.w	r4, r4, #458752	; 0x70000
300074c4:	4619      	mov	r1, r3
300074c6:	fa1f fc86 	uxth.w	ip, r6
300074ca:	4e0f      	ldr	r6, [pc, #60]	; (30007508 <PLL_NP_ClkSet+0x7c>)
300074cc:	ea50 314f 	orrs.w	r1, r0, pc, lsl #13
300074d0:	ea44 040c 	orr.w	r4, r4, ip
300074d4:	f004 ffd2 	bl	3000c47c <__aeabi_uldivmod>
300074d8:	4603      	mov	r3, r0
300074da:	2001      	movs	r0, #1
300074dc:	ea44 44c3 	orr.w	r4, r4, r3, lsl #19
300074e0:	64ec      	str	r4, [r5, #76]	; 0x4c
300074e2:	47b0      	blx	r6
300074e4:	6c6b      	ldr	r3, [r5, #68]	; 0x44
300074e6:	203c      	movs	r0, #60	; 0x3c
300074e8:	f443 4300 	orr.w	r3, r3, #32768	; 0x8000
300074ec:	646b      	str	r3, [r5, #68]	; 0x44
300074ee:	6c6b      	ldr	r3, [r5, #68]	; 0x44
300074f0:	f423 4300 	bic.w	r3, r3, #32768	; 0x8000
300074f4:	646b      	str	r3, [r5, #68]	; 0x44
300074f6:	4633      	mov	r3, r6
300074f8:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
300074fc:	4718      	bx	r3
300074fe:	bf00      	nop
30007500:	000099f5 	.word	0x000099f5
30007504:	42008800 	.word	0x42008800
30007508:	00009b2d 	.word	0x00009b2d

3000750c <PLL_NP_ClkGet>:
3000750c:	b538      	push	{r3, r4, r5, lr}
3000750e:	4b0f      	ldr	r3, [pc, #60]	; (3000754c <PLL_NP_ClkGet+0x40>)
30007510:	4798      	blx	r3
30007512:	4b0f      	ldr	r3, [pc, #60]	; (30007550 <PLL_NP_ClkGet+0x44>)
30007514:	6c59      	ldr	r1, [r3, #68]	; 0x44
30007516:	6cda      	ldr	r2, [r3, #76]	; 0x4c
30007518:	6cdb      	ldr	r3, [r3, #76]	; 0x4c
3000751a:	f3c1 1145 	ubfx	r1, r1, #5, #6
3000751e:	f3c2 4c02 	ubfx	ip, r2, #16, #3
30007522:	0cdb      	lsrs	r3, r3, #19
30007524:	3102      	adds	r1, #2
30007526:	fba3 4300 	umull	r4, r3, r3, r0
3000752a:	461d      	mov	r5, r3
3000752c:	ea54 355f 	orrs.w	r5, r4, pc, lsr #13
30007530:	46a6      	mov	lr, r4
30007532:	462c      	mov	r4, r5
30007534:	fbe0 e40c 	umlal	lr, r4, r0, ip
30007538:	4672      	mov	r2, lr
3000753a:	4623      	mov	r3, r4
3000753c:	ea52 03df 	orrs.w	r3, r2, pc, lsr #3
30007540:	4614      	mov	r4, r2
30007542:	fbe1 4300 	umlal	r4, r3, r1, r0
30007546:	4620      	mov	r0, r4
30007548:	bd38      	pop	{r3, r4, r5, pc}
3000754a:	bf00      	nop
3000754c:	000099f5 	.word	0x000099f5
30007550:	42008800 	.word	0x42008800

30007554 <PLL_AP_ClkSet>:
30007554:	b510      	push	{r4, lr}
30007556:	4604      	mov	r4, r0
30007558:	4b0a      	ldr	r3, [pc, #40]	; (30007584 <PLL_AP_ClkSet+0x30>)
3000755a:	4798      	blx	r3
3000755c:	fbb4 f0f0 	udiv	r0, r4, r0
30007560:	f1a0 0319 	sub.w	r3, r0, #25
30007564:	2b0f      	cmp	r3, #15
30007566:	d808      	bhi.n	3000757a <PLL_AP_ClkSet+0x26>
30007568:	4a07      	ldr	r2, [pc, #28]	; (30007588 <PLL_AP_ClkSet+0x34>)
3000756a:	3802      	subs	r0, #2
3000756c:	6953      	ldr	r3, [r2, #20]
3000756e:	b2c0      	uxtb	r0, r0
30007570:	f023 03ff 	bic.w	r3, r3, #255	; 0xff
30007574:	4318      	orrs	r0, r3
30007576:	6150      	str	r0, [r2, #20]
30007578:	bd10      	pop	{r4, pc}
3000757a:	f240 1191 	movw	r1, #401	; 0x191
3000757e:	4803      	ldr	r0, [pc, #12]	; (3000758c <PLL_AP_ClkSet+0x38>)
30007580:	f004 fcae 	bl	3000bee0 <io_assert_failed>
30007584:	000099f5 	.word	0x000099f5
30007588:	41000300 	.word	0x41000300
3000758c:	3000cf24 	.word	0x3000cf24

30007590 <PLL_AP>:
30007590:	2801      	cmp	r0, #1
30007592:	b538      	push	{r3, r4, r5, lr}
30007594:	d009      	beq.n	300075aa <PLL_AP+0x1a>
30007596:	4b13      	ldr	r3, [pc, #76]	; (300075e4 <PLL_AP+0x54>)
30007598:	681a      	ldr	r2, [r3, #0]
3000759a:	f022 0209 	bic.w	r2, r2, #9
3000759e:	601a      	str	r2, [r3, #0]
300075a0:	681a      	ldr	r2, [r3, #0]
300075a2:	f022 0206 	bic.w	r2, r2, #6
300075a6:	601a      	str	r2, [r3, #0]
300075a8:	bd38      	pop	{r3, r4, r5, pc}
300075aa:	4b0f      	ldr	r3, [pc, #60]	; (300075e8 <PLL_AP+0x58>)
300075ac:	6f9a      	ldr	r2, [r3, #120]	; 0x78
300075ae:	f012 0f05 	tst.w	r2, #5
300075b2:	d00f      	beq.n	300075d4 <PLL_AP+0x44>
300075b4:	4d0d      	ldr	r5, [pc, #52]	; (300075ec <PLL_AP+0x5c>)
300075b6:	4c0b      	ldr	r4, [pc, #44]	; (300075e4 <PLL_AP+0x54>)
300075b8:	2001      	movs	r0, #1
300075ba:	6823      	ldr	r3, [r4, #0]
300075bc:	f043 0306 	orr.w	r3, r3, #6
300075c0:	6023      	str	r3, [r4, #0]
300075c2:	47a8      	blx	r5
300075c4:	6823      	ldr	r3, [r4, #0]
300075c6:	f043 0309 	orr.w	r3, r3, #9
300075ca:	6023      	str	r3, [r4, #0]
300075cc:	6863      	ldr	r3, [r4, #4]
300075ce:	2b00      	cmp	r3, #0
300075d0:	dafc      	bge.n	300075cc <PLL_AP+0x3c>
300075d2:	bd38      	pop	{r3, r4, r5, pc}
300075d4:	6f9a      	ldr	r2, [r3, #120]	; 0x78
300075d6:	20a0      	movs	r0, #160	; 0xa0
300075d8:	4d04      	ldr	r5, [pc, #16]	; (300075ec <PLL_AP+0x5c>)
300075da:	f042 0207 	orr.w	r2, r2, #7
300075de:	679a      	str	r2, [r3, #120]	; 0x78
300075e0:	47a8      	blx	r5
300075e2:	e7e8      	b.n	300075b6 <PLL_AP+0x26>
300075e4:	41000300 	.word	0x41000300
300075e8:	42008800 	.word	0x42008800
300075ec:	00009b2d 	.word	0x00009b2d

300075f0 <BOOT_Reason>:
300075f0:	4b02      	ldr	r3, [pc, #8]	; (300075fc <BOOT_Reason+0xc>)
300075f2:	f8b3 0064 	ldrh.w	r0, [r3, #100]	; 0x64
300075f6:	b280      	uxth	r0, r0
300075f8:	4770      	bx	lr
300075fa:	bf00      	nop
300075fc:	42008200 	.word	0x42008200

30007600 <ddr_init_index>:
30007600:	b570      	push	{r4, r5, r6, lr}
30007602:	4b11      	ldr	r3, [pc, #68]	; (30007648 <ddr_init_index+0x48>)
30007604:	b082      	sub	sp, #8
30007606:	4798      	blx	r3
30007608:	4604      	mov	r4, r0
3000760a:	4b10      	ldr	r3, [pc, #64]	; (3000764c <ddr_init_index+0x4c>)
3000760c:	4798      	blx	r3
3000760e:	2c03      	cmp	r4, #3
30007610:	f3c0 40c4 	ubfx	r0, r0, #19, #5
30007614:	d014      	beq.n	30007640 <ddr_init_index+0x40>
30007616:	2c01      	cmp	r4, #1
30007618:	d00c      	beq.n	30007634 <ddr_init_index+0x34>
3000761a:	4e0d      	ldr	r6, [pc, #52]	; (30007650 <ddr_init_index+0x50>)
3000761c:	4d0d      	ldr	r5, [pc, #52]	; (30007654 <ddr_init_index+0x54>)
3000761e:	4c0e      	ldr	r4, [pc, #56]	; (30007658 <ddr_init_index+0x58>)
30007620:	462b      	mov	r3, r5
30007622:	2245      	movs	r2, #69	; 0x45
30007624:	4621      	mov	r1, r4
30007626:	2002      	movs	r0, #2
30007628:	f002 fe76 	bl	3000a318 <rtk_log_write>
3000762c:	f242 7010 	movw	r0, #10000	; 0x2710
30007630:	47b0      	blx	r6
30007632:	e7f5      	b.n	30007620 <ddr_init_index+0x20>
30007634:	2808      	cmp	r0, #8
30007636:	d003      	beq.n	30007640 <ddr_init_index+0x40>
30007638:	2809      	cmp	r0, #9
3000763a:	d1ee      	bne.n	3000761a <ddr_init_index+0x1a>
3000763c:	2000      	movs	r0, #0
3000763e:	e000      	b.n	30007642 <ddr_init_index+0x42>
30007640:	4620      	mov	r0, r4
30007642:	b002      	add	sp, #8
30007644:	bd70      	pop	{r4, r5, r6, pc}
30007646:	bf00      	nop
30007648:	30008ad1 	.word	0x30008ad1
3000764c:	3000af89 	.word	0x3000af89
30007650:	00009be5 	.word	0x00009be5
30007654:	3000cf98 	.word	0x3000cf98
30007658:	3000cfb4 	.word	0x3000cfb4

3000765c <rxi316_perf_tune>:
3000765c:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30007660:	4606      	mov	r6, r0
30007662:	6800      	ldr	r0, [r0, #0]
30007664:	b0c3      	sub	sp, #268	; 0x10c
30007666:	7985      	ldrb	r5, [r0, #6]
30007668:	2d00      	cmp	r5, #0
3000766a:	f000 8332 	beq.w	30007cd2 <rxi316_perf_tune+0x676>
3000766e:	2d01      	cmp	r5, #1
30007670:	d003      	beq.n	3000767a <rxi316_perf_tune+0x1e>
30007672:	2d03      	cmp	r5, #3
30007674:	bf14      	ite	ne
30007676:	2502      	movne	r5, #2
30007678:	2500      	moveq	r5, #0
3000767a:	68f3      	ldr	r3, [r6, #12]
3000767c:	2420      	movs	r4, #32
3000767e:	f04f 0a00 	mov.w	sl, #0
30007682:	4684      	mov	ip, r0
30007684:	685b      	ldr	r3, [r3, #4]
30007686:	46a3      	mov	fp, r4
30007688:	4627      	mov	r7, r4
3000768a:	46a0      	mov	r8, r4
3000768c:	7859      	ldrb	r1, [r3, #1]
3000768e:	46a1      	mov	r9, r4
30007690:	781a      	ldrb	r2, [r3, #0]
30007692:	9123      	str	r1, [sp, #140]	; 0x8c
30007694:	7899      	ldrb	r1, [r3, #2]
30007696:	9124      	str	r1, [sp, #144]	; 0x90
30007698:	78d9      	ldrb	r1, [r3, #3]
3000769a:	9125      	str	r1, [sp, #148]	; 0x94
3000769c:	7919      	ldrb	r1, [r3, #4]
3000769e:	9126      	str	r1, [sp, #152]	; 0x98
300076a0:	7959      	ldrb	r1, [r3, #5]
300076a2:	9127      	str	r1, [sp, #156]	; 0x9c
300076a4:	7999      	ldrb	r1, [r3, #6]
300076a6:	9128      	str	r1, [sp, #160]	; 0xa0
300076a8:	79d9      	ldrb	r1, [r3, #7]
300076aa:	9129      	str	r1, [sp, #164]	; 0xa4
300076ac:	7a19      	ldrb	r1, [r3, #8]
300076ae:	912a      	str	r1, [sp, #168]	; 0xa8
300076b0:	7a59      	ldrb	r1, [r3, #9]
300076b2:	912b      	str	r1, [sp, #172]	; 0xac
300076b4:	7a99      	ldrb	r1, [r3, #10]
300076b6:	912c      	str	r1, [sp, #176]	; 0xb0
300076b8:	7ad9      	ldrb	r1, [r3, #11]
300076ba:	912d      	str	r1, [sp, #180]	; 0xb4
300076bc:	7b19      	ldrb	r1, [r3, #12]
300076be:	912e      	str	r1, [sp, #184]	; 0xb8
300076c0:	7b59      	ldrb	r1, [r3, #13]
300076c2:	912f      	str	r1, [sp, #188]	; 0xbc
300076c4:	7b99      	ldrb	r1, [r3, #14]
300076c6:	9130      	str	r1, [sp, #192]	; 0xc0
300076c8:	7bd9      	ldrb	r1, [r3, #15]
300076ca:	9131      	str	r1, [sp, #196]	; 0xc4
300076cc:	7c19      	ldrb	r1, [r3, #16]
300076ce:	941d      	str	r4, [sp, #116]	; 0x74
300076d0:	9132      	str	r1, [sp, #200]	; 0xc8
300076d2:	7c59      	ldrb	r1, [r3, #17]
300076d4:	9133      	str	r1, [sp, #204]	; 0xcc
300076d6:	7c99      	ldrb	r1, [r3, #18]
300076d8:	9134      	str	r1, [sp, #208]	; 0xd0
300076da:	7cd9      	ldrb	r1, [r3, #19]
300076dc:	9135      	str	r1, [sp, #212]	; 0xd4
300076de:	7d19      	ldrb	r1, [r3, #20]
300076e0:	9136      	str	r1, [sp, #216]	; 0xd8
300076e2:	7d59      	ldrb	r1, [r3, #21]
300076e4:	9137      	str	r1, [sp, #220]	; 0xdc
300076e6:	7d99      	ldrb	r1, [r3, #22]
300076e8:	9138      	str	r1, [sp, #224]	; 0xe0
300076ea:	7dd9      	ldrb	r1, [r3, #23]
300076ec:	9139      	str	r1, [sp, #228]	; 0xe4
300076ee:	7e19      	ldrb	r1, [r3, #24]
300076f0:	913a      	str	r1, [sp, #232]	; 0xe8
300076f2:	7e59      	ldrb	r1, [r3, #25]
300076f4:	913b      	str	r1, [sp, #236]	; 0xec
300076f6:	7e99      	ldrb	r1, [r3, #26]
300076f8:	913c      	str	r1, [sp, #240]	; 0xf0
300076fa:	7ed9      	ldrb	r1, [r3, #27]
300076fc:	913d      	str	r1, [sp, #244]	; 0xf4
300076fe:	7f19      	ldrb	r1, [r3, #28]
30007700:	913e      	str	r1, [sp, #248]	; 0xf8
30007702:	7f59      	ldrb	r1, [r3, #29]
30007704:	913f      	str	r1, [sp, #252]	; 0xfc
30007706:	7f99      	ldrb	r1, [r3, #30]
30007708:	7fdb      	ldrb	r3, [r3, #31]
3000770a:	9140      	str	r1, [sp, #256]	; 0x100
3000770c:	4651      	mov	r1, sl
3000770e:	9341      	str	r3, [sp, #260]	; 0x104
30007710:	4653      	mov	r3, sl
30007712:	9409      	str	r4, [sp, #36]	; 0x24
30007714:	941c      	str	r4, [sp, #112]	; 0x70
30007716:	941b      	str	r4, [sp, #108]	; 0x6c
30007718:	940a      	str	r4, [sp, #40]	; 0x28
3000771a:	e9cd 4405 	strd	r4, r4, [sp, #20]
3000771e:	e9cd 4403 	strd	r4, r4, [sp, #12]
30007722:	e9cd 4401 	strd	r4, r4, [sp, #4]
30007726:	e9cd 4407 	strd	r4, r4, [sp, #28]
3000772a:	e9cd 4419 	strd	r4, r4, [sp, #100]	; 0x64
3000772e:	e9cd 4417 	strd	r4, r4, [sp, #92]	; 0x5c
30007732:	e9cd 4415 	strd	r4, r4, [sp, #84]	; 0x54
30007736:	e9cd 4413 	strd	r4, r4, [sp, #76]	; 0x4c
3000773a:	e9cd 4411 	strd	r4, r4, [sp, #68]	; 0x44
3000773e:	e9cd 440f 	strd	r4, r4, [sp, #60]	; 0x3c
30007742:	e9cd 440d 	strd	r4, r4, [sp, #52]	; 0x34
30007746:	e9cd 440b 	strd	r4, r4, [sp, #44]	; 0x2c
3000774a:	b94a      	cbnz	r2, 30007760 <rxi316_perf_tune+0x104>
3000774c:	3101      	adds	r1, #1
3000774e:	4699      	mov	r9, r3
30007750:	3301      	adds	r3, #1
30007752:	2b20      	cmp	r3, #32
30007754:	d069      	beq.n	3000782a <rxi316_perf_tune+0x1ce>
30007756:	aa22      	add	r2, sp, #136	; 0x88
30007758:	f852 2023 	ldr.w	r2, [r2, r3, lsl #2]
3000775c:	2a00      	cmp	r2, #0
3000775e:	d0f5      	beq.n	3000774c <rxi316_perf_tune+0xf0>
30007760:	2a01      	cmp	r2, #1
30007762:	d05d      	beq.n	30007820 <rxi316_perf_tune+0x1c4>
30007764:	2a02      	cmp	r2, #2
30007766:	d06e      	beq.n	30007846 <rxi316_perf_tune+0x1ea>
30007768:	2a03      	cmp	r2, #3
3000776a:	d06f      	beq.n	3000784c <rxi316_perf_tune+0x1f0>
3000776c:	2a04      	cmp	r2, #4
3000776e:	d070      	beq.n	30007852 <rxi316_perf_tune+0x1f6>
30007770:	2a05      	cmp	r2, #5
30007772:	d071      	beq.n	30007858 <rxi316_perf_tune+0x1fc>
30007774:	2a06      	cmp	r2, #6
30007776:	d072      	beq.n	3000785e <rxi316_perf_tune+0x202>
30007778:	2a07      	cmp	r2, #7
3000777a:	f000 82ac 	beq.w	30007cd6 <rxi316_perf_tune+0x67a>
3000777e:	2a08      	cmp	r2, #8
30007780:	f000 837e 	beq.w	30007e80 <rxi316_perf_tune+0x824>
30007784:	2a09      	cmp	r2, #9
30007786:	f000 8383 	beq.w	30007e90 <rxi316_perf_tune+0x834>
3000778a:	2a0a      	cmp	r2, #10
3000778c:	f000 838a 	beq.w	30007ea4 <rxi316_perf_tune+0x848>
30007790:	2a0b      	cmp	r2, #11
30007792:	f000 8383 	beq.w	30007e9c <rxi316_perf_tune+0x840>
30007796:	2a0c      	cmp	r2, #12
30007798:	f000 8388 	beq.w	30007eac <rxi316_perf_tune+0x850>
3000779c:	2a0d      	cmp	r2, #13
3000779e:	f000 838d 	beq.w	30007ebc <rxi316_perf_tune+0x860>
300077a2:	2a0e      	cmp	r2, #14
300077a4:	f000 8386 	beq.w	30007eb4 <rxi316_perf_tune+0x858>
300077a8:	2a0f      	cmp	r2, #15
300077aa:	f000 838b 	beq.w	30007ec4 <rxi316_perf_tune+0x868>
300077ae:	2a10      	cmp	r2, #16
300077b0:	f000 838c 	beq.w	30007ecc <rxi316_perf_tune+0x870>
300077b4:	2a11      	cmp	r2, #17
300077b6:	f000 838b 	beq.w	30007ed0 <rxi316_perf_tune+0x874>
300077ba:	2a12      	cmp	r2, #18
300077bc:	f000 838a 	beq.w	30007ed4 <rxi316_perf_tune+0x878>
300077c0:	2a13      	cmp	r2, #19
300077c2:	f000 838b 	beq.w	30007edc <rxi316_perf_tune+0x880>
300077c6:	2a14      	cmp	r2, #20
300077c8:	f000 8386 	beq.w	30007ed8 <rxi316_perf_tune+0x87c>
300077cc:	2a15      	cmp	r2, #21
300077ce:	f000 838d 	beq.w	30007eec <rxi316_perf_tune+0x890>
300077d2:	2a16      	cmp	r2, #22
300077d4:	f000 8388 	beq.w	30007ee8 <rxi316_perf_tune+0x88c>
300077d8:	2a17      	cmp	r2, #23
300077da:	f000 8397 	beq.w	30007f0c <rxi316_perf_tune+0x8b0>
300077de:	2a18      	cmp	r2, #24
300077e0:	f000 8392 	beq.w	30007f08 <rxi316_perf_tune+0x8ac>
300077e4:	2a19      	cmp	r2, #25
300077e6:	f000 838d 	beq.w	30007f04 <rxi316_perf_tune+0x8a8>
300077ea:	2a1a      	cmp	r2, #26
300077ec:	f000 8388 	beq.w	30007f00 <rxi316_perf_tune+0x8a4>
300077f0:	2a1b      	cmp	r2, #27
300077f2:	f000 8383 	beq.w	30007efc <rxi316_perf_tune+0x8a0>
300077f6:	2a1c      	cmp	r2, #28
300077f8:	f000 837e 	beq.w	30007ef8 <rxi316_perf_tune+0x89c>
300077fc:	2a1d      	cmp	r2, #29
300077fe:	f000 8379 	beq.w	30007ef4 <rxi316_perf_tune+0x898>
30007802:	2a1e      	cmp	r2, #30
30007804:	f000 8374 	beq.w	30007ef0 <rxi316_perf_tune+0x894>
30007808:	2a1f      	cmp	r2, #31
3000780a:	f000 836b 	beq.w	30007ee4 <rxi316_perf_tune+0x888>
3000780e:	2a20      	cmp	r2, #32
30007810:	f000 8366 	beq.w	30007ee0 <rxi316_perf_tune+0x884>
30007814:	9809      	ldr	r0, [sp, #36]	; 0x24
30007816:	2a21      	cmp	r2, #33	; 0x21
30007818:	bf08      	it	eq
3000781a:	4618      	moveq	r0, r3
3000781c:	9009      	str	r0, [sp, #36]	; 0x24
3000781e:	e797      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007820:	4698      	mov	r8, r3
30007822:	3301      	adds	r3, #1
30007824:	3101      	adds	r1, #1
30007826:	2b20      	cmp	r3, #32
30007828:	d195      	bne.n	30007756 <rxi316_perf_tune+0xfa>
3000782a:	f89c 2002 	ldrb.w	r2, [ip, #2]
3000782e:	f8dc 3008 	ldr.w	r3, [ip, #8]
30007832:	b1ba      	cbz	r2, 30007864 <rxi316_perf_tune+0x208>
30007834:	2a01      	cmp	r2, #1
30007836:	f000 832e 	beq.w	30007e96 <rxi316_perf_tune+0x83a>
3000783a:	2a03      	cmp	r2, #3
3000783c:	bf0c      	ite	eq
3000783e:	2204      	moveq	r2, #4
30007840:	2203      	movne	r2, #3
30007842:	921e      	str	r2, [sp, #120]	; 0x78
30007844:	e010      	b.n	30007868 <rxi316_perf_tune+0x20c>
30007846:	3101      	adds	r1, #1
30007848:	461f      	mov	r7, r3
3000784a:	e781      	b.n	30007750 <rxi316_perf_tune+0xf4>
3000784c:	3101      	adds	r1, #1
3000784e:	469b      	mov	fp, r3
30007850:	e77e      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007852:	3101      	adds	r1, #1
30007854:	9301      	str	r3, [sp, #4]
30007856:	e77b      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007858:	3101      	adds	r1, #1
3000785a:	9302      	str	r3, [sp, #8]
3000785c:	e778      	b.n	30007750 <rxi316_perf_tune+0xf4>
3000785e:	3101      	adds	r1, #1
30007860:	9303      	str	r3, [sp, #12]
30007862:	e775      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007864:	2201      	movs	r2, #1
30007866:	921e      	str	r2, [sp, #120]	; 0x78
30007868:	3b02      	subs	r3, #2
3000786a:	428b      	cmp	r3, r1
3000786c:	d005      	beq.n	3000787a <rxi316_perf_tune+0x21e>
3000786e:	4bab      	ldr	r3, [pc, #684]	; (30007b1c <rxi316_perf_tune+0x4c0>)
30007870:	2245      	movs	r2, #69	; 0x45
30007872:	49ab      	ldr	r1, [pc, #684]	; (30007b20 <rxi316_perf_tune+0x4c4>)
30007874:	2002      	movs	r0, #2
30007876:	f002 fd4f 	bl	3000a318 <rtk_log_write>
3000787a:	9b1e      	ldr	r3, [sp, #120]	; 0x78
3000787c:	459a      	cmp	sl, r3
3000787e:	d005      	beq.n	3000788c <rxi316_perf_tune+0x230>
30007880:	4ba8      	ldr	r3, [pc, #672]	; (30007b24 <rxi316_perf_tune+0x4c8>)
30007882:	2245      	movs	r2, #69	; 0x45
30007884:	49a6      	ldr	r1, [pc, #664]	; (30007b20 <rxi316_perf_tune+0x4c4>)
30007886:	2002      	movs	r0, #2
30007888:	f002 fd46 	bl	3000a318 <rtk_log_write>
3000788c:	9b0a      	ldr	r3, [sp, #40]	; 0x28
3000788e:	2b20      	cmp	r3, #32
30007890:	f000 82ef 	beq.w	30007e72 <rxi316_perf_tune+0x816>
30007894:	1b5a      	subs	r2, r3, r5
30007896:	3a05      	subs	r2, #5
30007898:	f002 030f 	and.w	r3, r2, #15
3000789c:	930a      	str	r3, [sp, #40]	; 0x28
3000789e:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
300078a0:	2b20      	cmp	r3, #32
300078a2:	f000 82e4 	beq.w	30007e6e <rxi316_perf_tune+0x812>
300078a6:	1b5a      	subs	r2, r3, r5
300078a8:	3a06      	subs	r2, #6
300078aa:	0112      	lsls	r2, r2, #4
300078ac:	b2d2      	uxtb	r2, r2
300078ae:	9b0c      	ldr	r3, [sp, #48]	; 0x30
300078b0:	2b20      	cmp	r3, #32
300078b2:	f000 82d8 	beq.w	30007e66 <rxi316_perf_tune+0x80a>
300078b6:	1b5b      	subs	r3, r3, r5
300078b8:	3b07      	subs	r3, #7
300078ba:	021b      	lsls	r3, r3, #8
300078bc:	f403 6370 	and.w	r3, r3, #3840	; 0xf00
300078c0:	930b      	str	r3, [sp, #44]	; 0x2c
300078c2:	9b0d      	ldr	r3, [sp, #52]	; 0x34
300078c4:	2b20      	cmp	r3, #32
300078c6:	f000 8209 	beq.w	30007cdc <rxi316_perf_tune+0x680>
300078ca:	1b5b      	subs	r3, r3, r5
300078cc:	3b08      	subs	r3, #8
300078ce:	031b      	lsls	r3, r3, #12
300078d0:	b29b      	uxth	r3, r3
300078d2:	930c      	str	r3, [sp, #48]	; 0x30
300078d4:	9b0e      	ldr	r3, [sp, #56]	; 0x38
300078d6:	2b20      	cmp	r3, #32
300078d8:	f000 8207 	beq.w	30007cea <rxi316_perf_tune+0x68e>
300078dc:	1b5b      	subs	r3, r3, r5
300078de:	3b09      	subs	r3, #9
300078e0:	041b      	lsls	r3, r3, #16
300078e2:	f403 2370 	and.w	r3, r3, #983040	; 0xf0000
300078e6:	930d      	str	r3, [sp, #52]	; 0x34
300078e8:	9b0f      	ldr	r3, [sp, #60]	; 0x3c
300078ea:	2b20      	cmp	r3, #32
300078ec:	f000 8204 	beq.w	30007cf8 <rxi316_perf_tune+0x69c>
300078f0:	1b5b      	subs	r3, r3, r5
300078f2:	3b0a      	subs	r3, #10
300078f4:	051b      	lsls	r3, r3, #20
300078f6:	f403 0370 	and.w	r3, r3, #15728640	; 0xf00000
300078fa:	930e      	str	r3, [sp, #56]	; 0x38
300078fc:	9b10      	ldr	r3, [sp, #64]	; 0x40
300078fe:	2b20      	cmp	r3, #32
30007900:	f000 8201 	beq.w	30007d06 <rxi316_perf_tune+0x6aa>
30007904:	1b5b      	subs	r3, r3, r5
30007906:	3b0b      	subs	r3, #11
30007908:	061b      	lsls	r3, r3, #24
3000790a:	f003 6370 	and.w	r3, r3, #251658240	; 0xf000000
3000790e:	930f      	str	r3, [sp, #60]	; 0x3c
30007910:	9b11      	ldr	r3, [sp, #68]	; 0x44
30007912:	2b20      	cmp	r3, #32
30007914:	f000 81fe 	beq.w	30007d14 <rxi316_perf_tune+0x6b8>
30007918:	1b5b      	subs	r3, r3, r5
3000791a:	3b0c      	subs	r3, #12
3000791c:	071b      	lsls	r3, r3, #28
3000791e:	9310      	str	r3, [sp, #64]	; 0x40
30007920:	9b12      	ldr	r3, [sp, #72]	; 0x48
30007922:	2b20      	cmp	r3, #32
30007924:	f000 81fd 	beq.w	30007d22 <rxi316_perf_tune+0x6c6>
30007928:	1b5b      	subs	r3, r3, r5
3000792a:	3b0d      	subs	r3, #13
3000792c:	f003 030f 	and.w	r3, r3, #15
30007930:	9311      	str	r3, [sp, #68]	; 0x44
30007932:	9b13      	ldr	r3, [sp, #76]	; 0x4c
30007934:	2b20      	cmp	r3, #32
30007936:	f000 81fa 	beq.w	30007d2e <rxi316_perf_tune+0x6d2>
3000793a:	1b5b      	subs	r3, r3, r5
3000793c:	3b0e      	subs	r3, #14
3000793e:	011b      	lsls	r3, r3, #4
30007940:	b2db      	uxtb	r3, r3
30007942:	9312      	str	r3, [sp, #72]	; 0x48
30007944:	9b14      	ldr	r3, [sp, #80]	; 0x50
30007946:	2b20      	cmp	r3, #32
30007948:	f000 81f7 	beq.w	30007d3a <rxi316_perf_tune+0x6de>
3000794c:	1b5b      	subs	r3, r3, r5
3000794e:	3b0f      	subs	r3, #15
30007950:	021b      	lsls	r3, r3, #8
30007952:	f403 6370 	and.w	r3, r3, #3840	; 0xf00
30007956:	9313      	str	r3, [sp, #76]	; 0x4c
30007958:	9b15      	ldr	r3, [sp, #84]	; 0x54
3000795a:	2b20      	cmp	r3, #32
3000795c:	f000 81f4 	beq.w	30007d48 <rxi316_perf_tune+0x6ec>
30007960:	1b5b      	subs	r3, r3, r5
30007962:	031b      	lsls	r3, r3, #12
30007964:	f403 4370 	and.w	r3, r3, #61440	; 0xf000
30007968:	9321      	str	r3, [sp, #132]	; 0x84
3000796a:	9b16      	ldr	r3, [sp, #88]	; 0x58
3000796c:	2b20      	cmp	r3, #32
3000796e:	f000 81f2 	beq.w	30007d56 <rxi316_perf_tune+0x6fa>
30007972:	1b5b      	subs	r3, r3, r5
30007974:	3b11      	subs	r3, #17
30007976:	041b      	lsls	r3, r3, #16
30007978:	f403 2370 	and.w	r3, r3, #983040	; 0xf0000
3000797c:	9320      	str	r3, [sp, #128]	; 0x80
3000797e:	9b17      	ldr	r3, [sp, #92]	; 0x5c
30007980:	2b20      	cmp	r3, #32
30007982:	f000 81ef 	beq.w	30007d64 <rxi316_perf_tune+0x708>
30007986:	1b5b      	subs	r3, r3, r5
30007988:	3b12      	subs	r3, #18
3000798a:	051b      	lsls	r3, r3, #20
3000798c:	f403 0370 	and.w	r3, r3, #15728640	; 0xf00000
30007990:	931f      	str	r3, [sp, #124]	; 0x7c
30007992:	9b18      	ldr	r3, [sp, #96]	; 0x60
30007994:	2b20      	cmp	r3, #32
30007996:	f000 81ec 	beq.w	30007d72 <rxi316_perf_tune+0x716>
3000799a:	1b5b      	subs	r3, r3, r5
3000799c:	3b13      	subs	r3, #19
3000799e:	061b      	lsls	r3, r3, #24
300079a0:	f003 6370 	and.w	r3, r3, #251658240	; 0xf000000
300079a4:	931e      	str	r3, [sp, #120]	; 0x78
300079a6:	9b19      	ldr	r3, [sp, #100]	; 0x64
300079a8:	2b20      	cmp	r3, #32
300079aa:	f000 81e9 	beq.w	30007d80 <rxi316_perf_tune+0x724>
300079ae:	1b5b      	subs	r3, r3, r5
300079b0:	3b14      	subs	r3, #20
300079b2:	071b      	lsls	r3, r3, #28
300079b4:	9319      	str	r3, [sp, #100]	; 0x64
300079b6:	9b1a      	ldr	r3, [sp, #104]	; 0x68
300079b8:	2b20      	cmp	r3, #32
300079ba:	f000 81e8 	beq.w	30007d8e <rxi316_perf_tune+0x732>
300079be:	1b5b      	subs	r3, r3, r5
300079c0:	3b15      	subs	r3, #21
300079c2:	f003 030f 	and.w	r3, r3, #15
300079c6:	9318      	str	r3, [sp, #96]	; 0x60
300079c8:	9b07      	ldr	r3, [sp, #28]
300079ca:	2b20      	cmp	r3, #32
300079cc:	f000 81e5 	beq.w	30007d9a <rxi316_perf_tune+0x73e>
300079d0:	1b5b      	subs	r3, r3, r5
300079d2:	3b02      	subs	r3, #2
300079d4:	f003 031f 	and.w	r3, r3, #31
300079d8:	9317      	str	r3, [sp, #92]	; 0x5c
300079da:	9b08      	ldr	r3, [sp, #32]
300079dc:	2b20      	cmp	r3, #32
300079de:	f000 81e2 	beq.w	30007da6 <rxi316_perf_tune+0x74a>
300079e2:	1b5b      	subs	r3, r3, r5
300079e4:	3b02      	subs	r3, #2
300079e6:	015b      	lsls	r3, r3, #5
300079e8:	f403 7378 	and.w	r3, r3, #992	; 0x3e0
300079ec:	9316      	str	r3, [sp, #88]	; 0x58
300079ee:	9b1b      	ldr	r3, [sp, #108]	; 0x6c
300079f0:	2b20      	cmp	r3, #32
300079f2:	f000 81df 	beq.w	30007db4 <rxi316_perf_tune+0x758>
300079f6:	1b5b      	subs	r3, r3, r5
300079f8:	f1b9 0f20 	cmp.w	r9, #32
300079fc:	f1a3 0302 	sub.w	r3, r3, #2
30007a00:	ea4f 2383 	mov.w	r3, r3, lsl #10
30007a04:	f403 43f8 	and.w	r3, r3, #31744	; 0x7c00
30007a08:	9315      	str	r3, [sp, #84]	; 0x54
30007a0a:	f000 81da 	beq.w	30007dc2 <rxi316_perf_tune+0x766>
30007a0e:	eba9 0105 	sub.w	r1, r9, r5
30007a12:	f1b8 0f20 	cmp.w	r8, #32
30007a16:	f1a1 0102 	sub.w	r1, r1, #2
30007a1a:	f001 030f 	and.w	r3, r1, #15
30007a1e:	9314      	str	r3, [sp, #80]	; 0x50
30007a20:	f000 81d5 	beq.w	30007dce <rxi316_perf_tune+0x772>
30007a24:	eba8 0105 	sub.w	r1, r8, r5
30007a28:	2f20      	cmp	r7, #32
30007a2a:	f1a1 0103 	sub.w	r1, r1, #3
30007a2e:	ea4f 1101 	mov.w	r1, r1, lsl #4
30007a32:	b2c9      	uxtb	r1, r1
30007a34:	f000 81d0 	beq.w	30007dd8 <rxi316_perf_tune+0x77c>
30007a38:	1b7b      	subs	r3, r7, r5
30007a3a:	f1bb 0f20 	cmp.w	fp, #32
30007a3e:	f1a3 0304 	sub.w	r3, r3, #4
30007a42:	ea4f 2303 	mov.w	r3, r3, lsl #8
30007a46:	f403 6370 	and.w	r3, r3, #3840	; 0xf00
30007a4a:	9308      	str	r3, [sp, #32]
30007a4c:	f000 81cb 	beq.w	30007de6 <rxi316_perf_tune+0x78a>
30007a50:	ebab 0305 	sub.w	r3, fp, r5
30007a54:	3b05      	subs	r3, #5
30007a56:	031b      	lsls	r3, r3, #12
30007a58:	b29b      	uxth	r3, r3
30007a5a:	9307      	str	r3, [sp, #28]
30007a5c:	9b01      	ldr	r3, [sp, #4]
30007a5e:	2b20      	cmp	r3, #32
30007a60:	f000 81c8 	beq.w	30007df4 <rxi316_perf_tune+0x798>
30007a64:	eba3 0c05 	sub.w	ip, r3, r5
30007a68:	9b02      	ldr	r3, [sp, #8]
30007a6a:	f1ac 0c06 	sub.w	ip, ip, #6
30007a6e:	2b20      	cmp	r3, #32
30007a70:	ea4f 4c0c 	mov.w	ip, ip, lsl #16
30007a74:	f40c 2c70 	and.w	ip, ip, #983040	; 0xf0000
30007a78:	f000 81c2 	beq.w	30007e00 <rxi316_perf_tune+0x7a4>
30007a7c:	eba3 0e05 	sub.w	lr, r3, r5
30007a80:	9b03      	ldr	r3, [sp, #12]
30007a82:	f1ae 0e07 	sub.w	lr, lr, #7
30007a86:	2b20      	cmp	r3, #32
30007a88:	ea4f 5e0e 	mov.w	lr, lr, lsl #20
30007a8c:	f40e 0e70 	and.w	lr, lr, #15728640	; 0xf00000
30007a90:	f000 81bc 	beq.w	30007e0c <rxi316_perf_tune+0x7b0>
30007a94:	eba3 0805 	sub.w	r8, r3, r5
30007a98:	9b04      	ldr	r3, [sp, #16]
30007a9a:	f1a8 0808 	sub.w	r8, r8, #8
30007a9e:	2b20      	cmp	r3, #32
30007aa0:	ea4f 6808 	mov.w	r8, r8, lsl #24
30007aa4:	f008 6870 	and.w	r8, r8, #251658240	; 0xf000000
30007aa8:	f000 81b6 	beq.w	30007e18 <rxi316_perf_tune+0x7bc>
30007aac:	1b5b      	subs	r3, r3, r5
30007aae:	3b09      	subs	r3, #9
30007ab0:	071b      	lsls	r3, r3, #28
30007ab2:	9302      	str	r3, [sp, #8]
30007ab4:	9b05      	ldr	r3, [sp, #20]
30007ab6:	2b20      	cmp	r3, #32
30007ab8:	f000 81b5 	beq.w	30007e26 <rxi316_perf_tune+0x7ca>
30007abc:	eba3 0905 	sub.w	r9, r3, r5
30007ac0:	9b06      	ldr	r3, [sp, #24]
30007ac2:	f1a9 090a 	sub.w	r9, r9, #10
30007ac6:	2b20      	cmp	r3, #32
30007ac8:	f009 090f 	and.w	r9, r9, #15
30007acc:	f000 81b1 	beq.w	30007e32 <rxi316_perf_tune+0x7d6>
30007ad0:	1b58      	subs	r0, r3, r5
30007ad2:	9b1c      	ldr	r3, [sp, #112]	; 0x70
30007ad4:	380b      	subs	r0, #11
30007ad6:	2b20      	cmp	r3, #32
30007ad8:	ea4f 1000 	mov.w	r0, r0, lsl #4
30007adc:	b2c0      	uxtb	r0, r0
30007ade:	f000 81ad 	beq.w	30007e3c <rxi316_perf_tune+0x7e0>
30007ae2:	eba3 0b05 	sub.w	fp, r3, r5
30007ae6:	9b09      	ldr	r3, [sp, #36]	; 0x24
30007ae8:	f1ab 0b02 	sub.w	fp, fp, #2
30007aec:	2b20      	cmp	r3, #32
30007aee:	f00b 0b1f 	and.w	fp, fp, #31
30007af2:	f000 81a9 	beq.w	30007e48 <rxi316_perf_tune+0x7ec>
30007af6:	eba3 0a05 	sub.w	sl, r3, r5
30007afa:	9b1d      	ldr	r3, [sp, #116]	; 0x74
30007afc:	f1aa 0a02 	sub.w	sl, sl, #2
30007b00:	2b20      	cmp	r3, #32
30007b02:	f00a 0a1f 	and.w	sl, sl, #31
30007b06:	f000 81a5 	beq.w	30007e54 <rxi316_perf_tune+0x7f8>
30007b0a:	1b5f      	subs	r7, r3, r5
30007b0c:	2c20      	cmp	r4, #32
30007b0e:	f1a7 0702 	sub.w	r7, r7, #2
30007b12:	f007 071f 	and.w	r7, r7, #31
30007b16:	f000 81a2 	beq.w	30007e5e <rxi316_perf_tune+0x802>
30007b1a:	e005      	b.n	30007b28 <rxi316_perf_tune+0x4cc>
30007b1c:	3000cfb8 	.word	0x3000cfb8
30007b20:	3000cfb4 	.word	0x3000cfb4
30007b24:	3000cff4 	.word	0x3000cff4
30007b28:	1b63      	subs	r3, r4, r5
30007b2a:	3b02      	subs	r3, #2
30007b2c:	015b      	lsls	r3, r3, #5
30007b2e:	f403 7378 	and.w	r3, r3, #992	; 0x3e0
30007b32:	9301      	str	r3, [sp, #4]
30007b34:	9c0a      	ldr	r4, [sp, #40]	; 0x28
30007b36:	ea40 0009 	orr.w	r0, r0, r9
30007b3a:	4314      	orrs	r4, r2
30007b3c:	4622      	mov	r2, r4
30007b3e:	e9dd 4511 	ldrd	r4, r5, [sp, #68]	; 0x44
30007b42:	432c      	orrs	r4, r5
30007b44:	9d14      	ldr	r5, [sp, #80]	; 0x50
30007b46:	4329      	orrs	r1, r5
30007b48:	e9dd 5316 	ldrd	r5, r3, [sp, #88]	; 0x58
30007b4c:	431d      	orrs	r5, r3
30007b4e:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
30007b50:	431a      	orrs	r2, r3
30007b52:	9b13      	ldr	r3, [sp, #76]	; 0x4c
30007b54:	431c      	orrs	r4, r3
30007b56:	9b08      	ldr	r3, [sp, #32]
30007b58:	4319      	orrs	r1, r3
30007b5a:	9b15      	ldr	r3, [sp, #84]	; 0x54
30007b5c:	431d      	orrs	r5, r3
30007b5e:	9b0c      	ldr	r3, [sp, #48]	; 0x30
30007b60:	431a      	orrs	r2, r3
30007b62:	9b21      	ldr	r3, [sp, #132]	; 0x84
30007b64:	431c      	orrs	r4, r3
30007b66:	9b07      	ldr	r3, [sp, #28]
30007b68:	4319      	orrs	r1, r3
30007b6a:	9b0d      	ldr	r3, [sp, #52]	; 0x34
30007b6c:	431a      	orrs	r2, r3
30007b6e:	9b20      	ldr	r3, [sp, #128]	; 0x80
30007b70:	ea41 010c 	orr.w	r1, r1, ip
30007b74:	431c      	orrs	r4, r3
30007b76:	9b01      	ldr	r3, [sp, #4]
30007b78:	ea41 010e 	orr.w	r1, r1, lr
30007b7c:	431f      	orrs	r7, r3
30007b7e:	9b0e      	ldr	r3, [sp, #56]	; 0x38
30007b80:	ea41 0108 	orr.w	r1, r1, r8
30007b84:	431a      	orrs	r2, r3
30007b86:	9b1f      	ldr	r3, [sp, #124]	; 0x7c
30007b88:	431c      	orrs	r4, r3
30007b8a:	9b0f      	ldr	r3, [sp, #60]	; 0x3c
30007b8c:	431a      	orrs	r2, r3
30007b8e:	9b1e      	ldr	r3, [sp, #120]	; 0x78
30007b90:	4323      	orrs	r3, r4
30007b92:	9c10      	ldr	r4, [sp, #64]	; 0x40
30007b94:	4322      	orrs	r2, r4
30007b96:	9c19      	ldr	r4, [sp, #100]	; 0x64
30007b98:	4323      	orrs	r3, r4
30007b9a:	9c02      	ldr	r4, [sp, #8]
30007b9c:	430c      	orrs	r4, r1
30007b9e:	49ba      	ldr	r1, [pc, #744]	; (30007e88 <rxi316_perf_tune+0x82c>)
30007ba0:	f8c1 2400 	str.w	r2, [r1, #1024]	; 0x400
30007ba4:	f8c1 3404 	str.w	r3, [r1, #1028]	; 0x404
30007ba8:	9b18      	ldr	r3, [sp, #96]	; 0x60
30007baa:	f8c1 3408 	str.w	r3, [r1, #1032]	; 0x408
30007bae:	f8c1 540c 	str.w	r5, [r1, #1036]	; 0x40c
30007bb2:	f8c1 4410 	str.w	r4, [r1, #1040]	; 0x410
30007bb6:	f8c1 0414 	str.w	r0, [r1, #1044]	; 0x414
30007bba:	f8c1 b418 	str.w	fp, [r1, #1048]	; 0x418
30007bbe:	f8c1 a41c 	str.w	sl, [r1, #1052]	; 0x41c
30007bc2:	f8c1 7420 	str.w	r7, [r1, #1056]	; 0x420
30007bc6:	68f3      	ldr	r3, [r6, #12]
30007bc8:	689a      	ldr	r2, [r3, #8]
30007bca:	e9d2 3200 	ldrd	r3, r2, [r2]
30007bce:	f003 0301 	and.w	r3, r3, #1
30007bd2:	ea43 4302 	orr.w	r3, r3, r2, lsl #16
30007bd6:	f8c1 3460 	str.w	r3, [r1, #1120]	; 0x460
30007bda:	68f3      	ldr	r3, [r6, #12]
30007bdc:	689a      	ldr	r2, [r3, #8]
30007bde:	7a13      	ldrb	r3, [r2, #8]
30007be0:	6950      	ldr	r0, [r2, #20]
30007be2:	ea43 6300 	orr.w	r3, r3, r0, lsl #24
30007be6:	e9d2 0203 	ldrd	r0, r2, [r2, #12]
30007bea:	0200      	lsls	r0, r0, #8
30007bec:	0412      	lsls	r2, r2, #16
30007bee:	b280      	uxth	r0, r0
30007bf0:	f402 027f 	and.w	r2, r2, #16711680	; 0xff0000
30007bf4:	4303      	orrs	r3, r0
30007bf6:	4313      	orrs	r3, r2
30007bf8:	f8c1 3464 	str.w	r3, [r1, #1124]	; 0x464
30007bfc:	68f3      	ldr	r3, [r6, #12]
30007bfe:	6898      	ldr	r0, [r3, #8]
30007c00:	e9d0 3207 	ldrd	r3, r2, [r0, #28]
30007c04:	7e00      	ldrb	r0, [r0, #24]
30007c06:	021b      	lsls	r3, r3, #8
30007c08:	0412      	lsls	r2, r2, #16
30007c0a:	b29b      	uxth	r3, r3
30007c0c:	f402 027f 	and.w	r2, r2, #16711680	; 0xff0000
30007c10:	4313      	orrs	r3, r2
30007c12:	4303      	orrs	r3, r0
30007c14:	f8c1 3468 	str.w	r3, [r1, #1128]	; 0x468
30007c18:	68f3      	ldr	r3, [r6, #12]
30007c1a:	689a      	ldr	r2, [r3, #8]
30007c1c:	e9d2 2309 	ldrd	r2, r3, [r2, #36]	; 0x24
30007c20:	041b      	lsls	r3, r3, #16
30007c22:	f002 023f 	and.w	r2, r2, #63	; 0x3f
30007c26:	f403 2370 	and.w	r3, r3, #983040	; 0xf0000
30007c2a:	4313      	orrs	r3, r2
30007c2c:	f8c1 3480 	str.w	r3, [r1, #1152]	; 0x480
30007c30:	6833      	ldr	r3, [r6, #0]
30007c32:	781b      	ldrb	r3, [r3, #0]
30007c34:	2b03      	cmp	r3, #3
30007c36:	f000 811f 	beq.w	30007e78 <rxi316_perf_tune+0x81c>
30007c3a:	68f3      	ldr	r3, [r6, #12]
30007c3c:	6899      	ldr	r1, [r3, #8]
30007c3e:	6b0b      	ldr	r3, [r1, #48]	; 0x30
30007c40:	005b      	lsls	r3, r3, #1
30007c42:	f003 0302 	and.w	r3, r3, #2
30007c46:	6b8a      	ldr	r2, [r1, #56]	; 0x38
30007c48:	6ac8      	ldr	r0, [r1, #44]	; 0x2c
30007c4a:	0412      	lsls	r2, r2, #16
30007c4c:	6b49      	ldr	r1, [r1, #52]	; 0x34
30007c4e:	f000 0001 	and.w	r0, r0, #1
30007c52:	f402 2270 	and.w	r2, r2, #983040	; 0xf0000
30007c56:	0209      	lsls	r1, r1, #8
30007c58:	431a      	orrs	r2, r3
30007c5a:	f401 6170 	and.w	r1, r1, #3840	; 0xf00
30007c5e:	4b8a      	ldr	r3, [pc, #552]	; (30007e88 <rxi316_perf_tune+0x82c>)
30007c60:	4302      	orrs	r2, r0
30007c62:	488a      	ldr	r0, [pc, #552]	; (30007e8c <rxi316_perf_tune+0x830>)
30007c64:	430a      	orrs	r2, r1
30007c66:	4310      	orrs	r0, r2
30007c68:	f8c3 0484 	str.w	r0, [r3, #1156]	; 0x484
30007c6c:	68f2      	ldr	r2, [r6, #12]
30007c6e:	6892      	ldr	r2, [r2, #8]
30007c70:	6bd2      	ldr	r2, [r2, #60]	; 0x3c
30007c72:	f8c3 2488 	str.w	r2, [r3, #1160]	; 0x488
30007c76:	68f2      	ldr	r2, [r6, #12]
30007c78:	6892      	ldr	r2, [r2, #8]
30007c7a:	6c12      	ldr	r2, [r2, #64]	; 0x40
30007c7c:	f8c3 248c 	str.w	r2, [r3, #1164]	; 0x48c
30007c80:	68f2      	ldr	r2, [r6, #12]
30007c82:	6892      	ldr	r2, [r2, #8]
30007c84:	6c52      	ldr	r2, [r2, #68]	; 0x44
30007c86:	f8c3 2490 	str.w	r2, [r3, #1168]	; 0x490
30007c8a:	68f2      	ldr	r2, [r6, #12]
30007c8c:	6892      	ldr	r2, [r2, #8]
30007c8e:	6c92      	ldr	r2, [r2, #72]	; 0x48
30007c90:	f8c3 2494 	str.w	r2, [r3, #1172]	; 0x494
30007c94:	68f2      	ldr	r2, [r6, #12]
30007c96:	6892      	ldr	r2, [r2, #8]
30007c98:	6cd2      	ldr	r2, [r2, #76]	; 0x4c
30007c9a:	f8c3 2498 	str.w	r2, [r3, #1176]	; 0x498
30007c9e:	68f2      	ldr	r2, [r6, #12]
30007ca0:	6892      	ldr	r2, [r2, #8]
30007ca2:	6d12      	ldr	r2, [r2, #80]	; 0x50
30007ca4:	f002 020f 	and.w	r2, r2, #15
30007ca8:	f8c3 24a0 	str.w	r2, [r3, #1184]	; 0x4a0
30007cac:	f8d3 2470 	ldr.w	r2, [r3, #1136]	; 0x470
30007cb0:	f022 023f 	bic.w	r2, r2, #63	; 0x3f
30007cb4:	f042 0204 	orr.w	r2, r2, #4
30007cb8:	f8c3 2470 	str.w	r2, [r3, #1136]	; 0x470
30007cbc:	f8d3 2474 	ldr.w	r2, [r3, #1140]	; 0x474
30007cc0:	f422 4270 	bic.w	r2, r2, #61440	; 0xf000
30007cc4:	f442 4280 	orr.w	r2, r2, #16384	; 0x4000
30007cc8:	f8c3 2474 	str.w	r2, [r3, #1140]	; 0x474
30007ccc:	b043      	add	sp, #268	; 0x10c
30007cce:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30007cd2:	2502      	movs	r5, #2
30007cd4:	e4d1      	b.n	3000767a <rxi316_perf_tune+0x1e>
30007cd6:	3101      	adds	r1, #1
30007cd8:	9304      	str	r3, [sp, #16]
30007cda:	e539      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007cdc:	f44f 4370 	mov.w	r3, #61440	; 0xf000
30007ce0:	930c      	str	r3, [sp, #48]	; 0x30
30007ce2:	9b0e      	ldr	r3, [sp, #56]	; 0x38
30007ce4:	2b20      	cmp	r3, #32
30007ce6:	f47f adf9 	bne.w	300078dc <rxi316_perf_tune+0x280>
30007cea:	f44f 2370 	mov.w	r3, #983040	; 0xf0000
30007cee:	930d      	str	r3, [sp, #52]	; 0x34
30007cf0:	9b0f      	ldr	r3, [sp, #60]	; 0x3c
30007cf2:	2b20      	cmp	r3, #32
30007cf4:	f47f adfc 	bne.w	300078f0 <rxi316_perf_tune+0x294>
30007cf8:	f44f 0370 	mov.w	r3, #15728640	; 0xf00000
30007cfc:	930e      	str	r3, [sp, #56]	; 0x38
30007cfe:	9b10      	ldr	r3, [sp, #64]	; 0x40
30007d00:	2b20      	cmp	r3, #32
30007d02:	f47f adff 	bne.w	30007904 <rxi316_perf_tune+0x2a8>
30007d06:	f04f 6370 	mov.w	r3, #251658240	; 0xf000000
30007d0a:	930f      	str	r3, [sp, #60]	; 0x3c
30007d0c:	9b11      	ldr	r3, [sp, #68]	; 0x44
30007d0e:	2b20      	cmp	r3, #32
30007d10:	f47f ae02 	bne.w	30007918 <rxi316_perf_tune+0x2bc>
30007d14:	f04f 4370 	mov.w	r3, #4026531840	; 0xf0000000
30007d18:	9310      	str	r3, [sp, #64]	; 0x40
30007d1a:	9b12      	ldr	r3, [sp, #72]	; 0x48
30007d1c:	2b20      	cmp	r3, #32
30007d1e:	f47f ae03 	bne.w	30007928 <rxi316_perf_tune+0x2cc>
30007d22:	230f      	movs	r3, #15
30007d24:	9311      	str	r3, [sp, #68]	; 0x44
30007d26:	9b13      	ldr	r3, [sp, #76]	; 0x4c
30007d28:	2b20      	cmp	r3, #32
30007d2a:	f47f ae06 	bne.w	3000793a <rxi316_perf_tune+0x2de>
30007d2e:	23f0      	movs	r3, #240	; 0xf0
30007d30:	9312      	str	r3, [sp, #72]	; 0x48
30007d32:	9b14      	ldr	r3, [sp, #80]	; 0x50
30007d34:	2b20      	cmp	r3, #32
30007d36:	f47f ae09 	bne.w	3000794c <rxi316_perf_tune+0x2f0>
30007d3a:	f44f 6370 	mov.w	r3, #3840	; 0xf00
30007d3e:	9313      	str	r3, [sp, #76]	; 0x4c
30007d40:	9b15      	ldr	r3, [sp, #84]	; 0x54
30007d42:	2b20      	cmp	r3, #32
30007d44:	f47f ae0c 	bne.w	30007960 <rxi316_perf_tune+0x304>
30007d48:	f44f 4370 	mov.w	r3, #61440	; 0xf000
30007d4c:	9321      	str	r3, [sp, #132]	; 0x84
30007d4e:	9b16      	ldr	r3, [sp, #88]	; 0x58
30007d50:	2b20      	cmp	r3, #32
30007d52:	f47f ae0e 	bne.w	30007972 <rxi316_perf_tune+0x316>
30007d56:	f44f 2370 	mov.w	r3, #983040	; 0xf0000
30007d5a:	9320      	str	r3, [sp, #128]	; 0x80
30007d5c:	9b17      	ldr	r3, [sp, #92]	; 0x5c
30007d5e:	2b20      	cmp	r3, #32
30007d60:	f47f ae11 	bne.w	30007986 <rxi316_perf_tune+0x32a>
30007d64:	f44f 0370 	mov.w	r3, #15728640	; 0xf00000
30007d68:	931f      	str	r3, [sp, #124]	; 0x7c
30007d6a:	9b18      	ldr	r3, [sp, #96]	; 0x60
30007d6c:	2b20      	cmp	r3, #32
30007d6e:	f47f ae14 	bne.w	3000799a <rxi316_perf_tune+0x33e>
30007d72:	f04f 6370 	mov.w	r3, #251658240	; 0xf000000
30007d76:	931e      	str	r3, [sp, #120]	; 0x78
30007d78:	9b19      	ldr	r3, [sp, #100]	; 0x64
30007d7a:	2b20      	cmp	r3, #32
30007d7c:	f47f ae17 	bne.w	300079ae <rxi316_perf_tune+0x352>
30007d80:	f04f 4370 	mov.w	r3, #4026531840	; 0xf0000000
30007d84:	9319      	str	r3, [sp, #100]	; 0x64
30007d86:	9b1a      	ldr	r3, [sp, #104]	; 0x68
30007d88:	2b20      	cmp	r3, #32
30007d8a:	f47f ae18 	bne.w	300079be <rxi316_perf_tune+0x362>
30007d8e:	230f      	movs	r3, #15
30007d90:	9318      	str	r3, [sp, #96]	; 0x60
30007d92:	9b07      	ldr	r3, [sp, #28]
30007d94:	2b20      	cmp	r3, #32
30007d96:	f47f ae1b 	bne.w	300079d0 <rxi316_perf_tune+0x374>
30007d9a:	231f      	movs	r3, #31
30007d9c:	9317      	str	r3, [sp, #92]	; 0x5c
30007d9e:	9b08      	ldr	r3, [sp, #32]
30007da0:	2b20      	cmp	r3, #32
30007da2:	f47f ae1e 	bne.w	300079e2 <rxi316_perf_tune+0x386>
30007da6:	f44f 7378 	mov.w	r3, #992	; 0x3e0
30007daa:	9316      	str	r3, [sp, #88]	; 0x58
30007dac:	9b1b      	ldr	r3, [sp, #108]	; 0x6c
30007dae:	2b20      	cmp	r3, #32
30007db0:	f47f ae21 	bne.w	300079f6 <rxi316_perf_tune+0x39a>
30007db4:	f44f 43f8 	mov.w	r3, #31744	; 0x7c00
30007db8:	f1b9 0f20 	cmp.w	r9, #32
30007dbc:	9315      	str	r3, [sp, #84]	; 0x54
30007dbe:	f47f ae26 	bne.w	30007a0e <rxi316_perf_tune+0x3b2>
30007dc2:	230f      	movs	r3, #15
30007dc4:	f1b8 0f20 	cmp.w	r8, #32
30007dc8:	9314      	str	r3, [sp, #80]	; 0x50
30007dca:	f47f ae2b 	bne.w	30007a24 <rxi316_perf_tune+0x3c8>
30007dce:	2f20      	cmp	r7, #32
30007dd0:	f04f 01f0 	mov.w	r1, #240	; 0xf0
30007dd4:	f47f ae30 	bne.w	30007a38 <rxi316_perf_tune+0x3dc>
30007dd8:	f44f 6370 	mov.w	r3, #3840	; 0xf00
30007ddc:	f1bb 0f20 	cmp.w	fp, #32
30007de0:	9308      	str	r3, [sp, #32]
30007de2:	f47f ae35 	bne.w	30007a50 <rxi316_perf_tune+0x3f4>
30007de6:	f44f 4370 	mov.w	r3, #61440	; 0xf000
30007dea:	9307      	str	r3, [sp, #28]
30007dec:	9b01      	ldr	r3, [sp, #4]
30007dee:	2b20      	cmp	r3, #32
30007df0:	f47f ae38 	bne.w	30007a64 <rxi316_perf_tune+0x408>
30007df4:	9b02      	ldr	r3, [sp, #8]
30007df6:	f44f 2c70 	mov.w	ip, #983040	; 0xf0000
30007dfa:	2b20      	cmp	r3, #32
30007dfc:	f47f ae3e 	bne.w	30007a7c <rxi316_perf_tune+0x420>
30007e00:	9b03      	ldr	r3, [sp, #12]
30007e02:	f44f 0e70 	mov.w	lr, #15728640	; 0xf00000
30007e06:	2b20      	cmp	r3, #32
30007e08:	f47f ae44 	bne.w	30007a94 <rxi316_perf_tune+0x438>
30007e0c:	9b04      	ldr	r3, [sp, #16]
30007e0e:	f04f 6870 	mov.w	r8, #251658240	; 0xf000000
30007e12:	2b20      	cmp	r3, #32
30007e14:	f47f ae4a 	bne.w	30007aac <rxi316_perf_tune+0x450>
30007e18:	f04f 4370 	mov.w	r3, #4026531840	; 0xf0000000
30007e1c:	9302      	str	r3, [sp, #8]
30007e1e:	9b05      	ldr	r3, [sp, #20]
30007e20:	2b20      	cmp	r3, #32
30007e22:	f47f ae4b 	bne.w	30007abc <rxi316_perf_tune+0x460>
30007e26:	9b06      	ldr	r3, [sp, #24]
30007e28:	f04f 090f 	mov.w	r9, #15
30007e2c:	2b20      	cmp	r3, #32
30007e2e:	f47f ae4f 	bne.w	30007ad0 <rxi316_perf_tune+0x474>
30007e32:	9b1c      	ldr	r3, [sp, #112]	; 0x70
30007e34:	20f0      	movs	r0, #240	; 0xf0
30007e36:	2b20      	cmp	r3, #32
30007e38:	f47f ae53 	bne.w	30007ae2 <rxi316_perf_tune+0x486>
30007e3c:	9b09      	ldr	r3, [sp, #36]	; 0x24
30007e3e:	f04f 0b1f 	mov.w	fp, #31
30007e42:	2b20      	cmp	r3, #32
30007e44:	f47f ae57 	bne.w	30007af6 <rxi316_perf_tune+0x49a>
30007e48:	9b1d      	ldr	r3, [sp, #116]	; 0x74
30007e4a:	f04f 0a1f 	mov.w	sl, #31
30007e4e:	2b20      	cmp	r3, #32
30007e50:	f47f ae5b 	bne.w	30007b0a <rxi316_perf_tune+0x4ae>
30007e54:	2c20      	cmp	r4, #32
30007e56:	f04f 071f 	mov.w	r7, #31
30007e5a:	f47f ae65 	bne.w	30007b28 <rxi316_perf_tune+0x4cc>
30007e5e:	f44f 7378 	mov.w	r3, #992	; 0x3e0
30007e62:	9301      	str	r3, [sp, #4]
30007e64:	e666      	b.n	30007b34 <rxi316_perf_tune+0x4d8>
30007e66:	f44f 6370 	mov.w	r3, #3840	; 0xf00
30007e6a:	930b      	str	r3, [sp, #44]	; 0x2c
30007e6c:	e529      	b.n	300078c2 <rxi316_perf_tune+0x266>
30007e6e:	22f0      	movs	r2, #240	; 0xf0
30007e70:	e51d      	b.n	300078ae <rxi316_perf_tune+0x252>
30007e72:	230f      	movs	r3, #15
30007e74:	930a      	str	r3, [sp, #40]	; 0x28
30007e76:	e512      	b.n	3000789e <rxi316_perf_tune+0x242>
30007e78:	68f2      	ldr	r2, [r6, #12]
30007e7a:	2302      	movs	r3, #2
30007e7c:	6891      	ldr	r1, [r2, #8]
30007e7e:	e6e2      	b.n	30007c46 <rxi316_perf_tune+0x5ea>
30007e80:	3101      	adds	r1, #1
30007e82:	9305      	str	r3, [sp, #20]
30007e84:	e464      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007e86:	bf00      	nop
30007e88:	41100000 	.word	0x41100000
30007e8c:	40000008 	.word	0x40000008
30007e90:	3101      	adds	r1, #1
30007e92:	9306      	str	r3, [sp, #24]
30007e94:	e45c      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007e96:	2202      	movs	r2, #2
30007e98:	921e      	str	r2, [sp, #120]	; 0x78
30007e9a:	e4e5      	b.n	30007868 <rxi316_perf_tune+0x20c>
30007e9c:	f10a 0a01 	add.w	sl, sl, #1
30007ea0:	9308      	str	r3, [sp, #32]
30007ea2:	e455      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007ea4:	f10a 0a01 	add.w	sl, sl, #1
30007ea8:	9307      	str	r3, [sp, #28]
30007eaa:	e451      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007eac:	f10a 0a01 	add.w	sl, sl, #1
30007eb0:	931b      	str	r3, [sp, #108]	; 0x6c
30007eb2:	e44d      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007eb4:	f10a 0a01 	add.w	sl, sl, #1
30007eb8:	461c      	mov	r4, r3
30007eba:	e449      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007ebc:	f10a 0a01 	add.w	sl, sl, #1
30007ec0:	931d      	str	r3, [sp, #116]	; 0x74
30007ec2:	e445      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007ec4:	f10a 0a01 	add.w	sl, sl, #1
30007ec8:	931c      	str	r3, [sp, #112]	; 0x70
30007eca:	e441      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007ecc:	930a      	str	r3, [sp, #40]	; 0x28
30007ece:	e43f      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007ed0:	930b      	str	r3, [sp, #44]	; 0x2c
30007ed2:	e43d      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007ed4:	930c      	str	r3, [sp, #48]	; 0x30
30007ed6:	e43b      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007ed8:	930e      	str	r3, [sp, #56]	; 0x38
30007eda:	e439      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007edc:	930d      	str	r3, [sp, #52]	; 0x34
30007ede:	e437      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007ee0:	931a      	str	r3, [sp, #104]	; 0x68
30007ee2:	e435      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007ee4:	9319      	str	r3, [sp, #100]	; 0x64
30007ee6:	e433      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007ee8:	9310      	str	r3, [sp, #64]	; 0x40
30007eea:	e431      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007eec:	930f      	str	r3, [sp, #60]	; 0x3c
30007eee:	e42f      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007ef0:	9318      	str	r3, [sp, #96]	; 0x60
30007ef2:	e42d      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007ef4:	9317      	str	r3, [sp, #92]	; 0x5c
30007ef6:	e42b      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007ef8:	9316      	str	r3, [sp, #88]	; 0x58
30007efa:	e429      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007efc:	9315      	str	r3, [sp, #84]	; 0x54
30007efe:	e427      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007f00:	9314      	str	r3, [sp, #80]	; 0x50
30007f02:	e425      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007f04:	9313      	str	r3, [sp, #76]	; 0x4c
30007f06:	e423      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007f08:	9312      	str	r3, [sp, #72]	; 0x48
30007f0a:	e421      	b.n	30007750 <rxi316_perf_tune+0xf4>
30007f0c:	9311      	str	r3, [sp, #68]	; 0x44
30007f0e:	e41f      	b.n	30007750 <rxi316_perf_tune+0xf4>

30007f10 <rxi316_dram_init>:
30007f10:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30007f14:	4604      	mov	r4, r0
30007f16:	b08f      	sub	sp, #60	; 0x3c
30007f18:	4b10      	ldr	r3, [pc, #64]	; (30007f5c <rxi316_dram_init+0x4c>)
30007f1a:	4798      	blx	r3
30007f1c:	f894 b014 	ldrb.w	fp, [r4, #20]
30007f20:	2101      	movs	r1, #1
30007f22:	900c      	str	r0, [sp, #48]	; 0x30
30007f24:	fa01 f30b 	lsl.w	r3, r1, fp
30007f28:	9303      	str	r3, [sp, #12]
30007f2a:	6823      	ldr	r3, [r4, #0]
30007f2c:	78da      	ldrb	r2, [r3, #3]
30007f2e:	6923      	ldr	r3, [r4, #16]
30007f30:	fa03 f30b 	lsl.w	r3, r3, fp
30007f34:	b1a2      	cbz	r2, 30007f60 <rxi316_dram_init+0x50>
30007f36:	428a      	cmp	r2, r1
30007f38:	f000 8477 	beq.w	3000882a <rxi316_dram_init+0x91a>
30007f3c:	2a02      	cmp	r2, #2
30007f3e:	bf0b      	itete	eq
30007f40:	2206      	moveq	r2, #6
30007f42:	220a      	movne	r2, #10
30007f44:	2540      	moveq	r5, #64	; 0x40
30007f46:	2580      	movne	r5, #128	; 0x80
30007f48:	9207      	str	r2, [sp, #28]
30007f4a:	bf0c      	ite	eq
30007f4c:	2204      	moveq	r2, #4
30007f4e:	2208      	movne	r2, #8
30007f50:	9208      	str	r2, [sp, #32]
30007f52:	bf0c      	ite	eq
30007f54:	2208      	moveq	r2, #8
30007f56:	2210      	movne	r2, #16
30007f58:	9202      	str	r2, [sp, #8]
30007f5a:	e007      	b.n	30007f6c <rxi316_dram_init+0x5c>
30007f5c:	30008ad1 	.word	0x30008ad1
30007f60:	2203      	movs	r2, #3
30007f62:	25c0      	movs	r5, #192	; 0xc0
30007f64:	9108      	str	r1, [sp, #32]
30007f66:	9207      	str	r2, [sp, #28]
30007f68:	2202      	movs	r2, #2
30007f6a:	9202      	str	r2, [sp, #8]
30007f6c:	6860      	ldr	r0, [r4, #4]
30007f6e:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
30007f72:	4ac0      	ldr	r2, [pc, #768]	; (30008274 <rxi316_dram_init+0x364>)
30007f74:	6806      	ldr	r6, [r0, #0]
30007f76:	960d      	str	r6, [sp, #52]	; 0x34
30007f78:	6846      	ldr	r6, [r0, #4]
30007f7a:	6880      	ldr	r0, [r0, #8]
30007f7c:	9609      	str	r6, [sp, #36]	; 0x24
30007f7e:	9004      	str	r0, [sp, #16]
30007f80:	f8c2 1230 	str.w	r1, [r2, #560]	; 0x230
30007f84:	6820      	ldr	r0, [r4, #0]
30007f86:	7981      	ldrb	r1, [r0, #6]
30007f88:	6882      	ldr	r2, [r0, #8]
30007f8a:	b129      	cbz	r1, 30007f98 <rxi316_dram_init+0x88>
30007f8c:	2901      	cmp	r1, #1
30007f8e:	f000 8444 	beq.w	3000881a <rxi316_dram_init+0x90a>
30007f92:	2903      	cmp	r1, #3
30007f94:	f000 8453 	beq.w	3000883e <rxi316_dram_init+0x92e>
30007f98:	3a06      	subs	r2, #6
30007f9a:	7881      	ldrb	r1, [r0, #2]
30007f9c:	f002 020f 	and.w	r2, r2, #15
30007fa0:	7940      	ldrb	r0, [r0, #5]
30007fa2:	0109      	lsls	r1, r1, #4
30007fa4:	0280      	lsls	r0, r0, #10
30007fa6:	f001 0130 	and.w	r1, r1, #48	; 0x30
30007faa:	f400 6040 	and.w	r0, r0, #3072	; 0xc00
30007fae:	4301      	orrs	r1, r0
30007fb0:	430d      	orrs	r5, r1
30007fb2:	432a      	orrs	r2, r5
30007fb4:	4daf      	ldr	r5, [pc, #700]	; (30008274 <rxi316_dram_init+0x364>)
30007fb6:	f8c5 2224 	str.w	r2, [r5, #548]	; 0x224
30007fba:	68e2      	ldr	r2, [r4, #12]
30007fbc:	6812      	ldr	r2, [r2, #0]
30007fbe:	f892 206c 	ldrb.w	r2, [r2, #108]	; 0x6c
30007fc2:	f002 0207 	and.w	r2, r2, #7
30007fc6:	f8c5 2228 	str.w	r2, [r5, #552]	; 0x228
30007fca:	68e2      	ldr	r2, [r4, #12]
30007fcc:	6811      	ldr	r1, [r2, #0]
30007fce:	6822      	ldr	r2, [r4, #0]
30007fd0:	6848      	ldr	r0, [r1, #4]
30007fd2:	7817      	ldrb	r7, [r2, #0]
30007fd4:	688a      	ldr	r2, [r1, #8]
30007fd6:	9005      	str	r0, [sp, #20]
30007fd8:	0300      	lsls	r0, r0, #12
30007fda:	0412      	lsls	r2, r2, #16
30007fdc:	9701      	str	r7, [sp, #4]
30007fde:	f400 5080 	and.w	r0, r0, #4096	; 0x1000
30007fe2:	f007 070f 	and.w	r7, r7, #15
30007fe6:	f402 3280 	and.w	r2, r2, #65536	; 0x10000
30007fea:	4302      	orrs	r2, r0
30007fec:	68c8      	ldr	r0, [r1, #12]
30007fee:	0440      	lsls	r0, r0, #17
30007ff0:	433a      	orrs	r2, r7
30007ff2:	f400 3600 	and.w	r6, r0, #131072	; 0x20000
30007ff6:	6908      	ldr	r0, [r1, #16]
30007ff8:	0480      	lsls	r0, r0, #18
30007ffa:	4332      	orrs	r2, r6
30007ffc:	f400 2080 	and.w	r0, r0, #262144	; 0x40000
30008000:	4310      	orrs	r0, r2
30008002:	694a      	ldr	r2, [r1, #20]
30008004:	04d2      	lsls	r2, r2, #19
30008006:	f402 2200 	and.w	r2, r2, #524288	; 0x80000
3000800a:	4310      	orrs	r0, r2
3000800c:	698a      	ldr	r2, [r1, #24]
3000800e:	0552      	lsls	r2, r2, #21
30008010:	f402 1200 	and.w	r2, r2, #2097152	; 0x200000
30008014:	4310      	orrs	r0, r2
30008016:	69ca      	ldr	r2, [r1, #28]
30008018:	0592      	lsls	r2, r2, #22
3000801a:	f402 0280 	and.w	r2, r2, #4194304	; 0x400000
3000801e:	4310      	orrs	r0, r2
30008020:	6a8a      	ldr	r2, [r1, #40]	; 0x28
30008022:	0612      	lsls	r2, r2, #24
30008024:	f002 7240 	and.w	r2, r2, #50331648	; 0x3000000
30008028:	4302      	orrs	r2, r0
3000802a:	e9d1 1008 	ldrd	r1, r0, [r1, #32]
3000802e:	06c0      	lsls	r0, r0, #27
30008030:	0789      	lsls	r1, r1, #30
30008032:	f000 6000 	and.w	r0, r0, #134217728	; 0x8000000
30008036:	f001 4180 	and.w	r1, r1, #1073741824	; 0x40000000
3000803a:	4310      	orrs	r0, r2
3000803c:	9a03      	ldr	r2, [sp, #12]
3000803e:	0212      	lsls	r2, r2, #8
30008040:	4308      	orrs	r0, r1
30008042:	f402 62e0 	and.w	r2, r2, #1792	; 0x700
30008046:	4310      	orrs	r0, r2
30008048:	6068      	str	r0, [r5, #4]
3000804a:	6822      	ldr	r2, [r4, #0]
3000804c:	7992      	ldrb	r2, [r2, #6]
3000804e:	2a00      	cmp	r2, #0
30008050:	f040 831d 	bne.w	3000868e <rxi316_dram_init+0x77e>
30008054:	686a      	ldr	r2, [r5, #4]
30008056:	f042 0210 	orr.w	r2, r2, #16
3000805a:	606a      	str	r2, [r5, #4]
3000805c:	68e2      	ldr	r2, [r4, #12]
3000805e:	9803      	ldr	r0, [sp, #12]
30008060:	6812      	ldr	r2, [r2, #0]
30008062:	6861      	ldr	r1, [r4, #4]
30008064:	f100 3aff 	add.w	sl, r0, #4294967295	; 0xffffffff
30008068:	6ad0      	ldr	r0, [r2, #44]	; 0x2c
3000806a:	f8d1 e010 	ldr.w	lr, [r1, #16]
3000806e:	9006      	str	r0, [sp, #24]
30008070:	6b10      	ldr	r0, [r2, #48]	; 0x30
30008072:	fa2e f80b 	lsr.w	r8, lr, fp
30008076:	6949      	ldr	r1, [r1, #20]
30008078:	ea1e 0f0a 	tst.w	lr, sl
3000807c:	900a      	str	r0, [sp, #40]	; 0x28
3000807e:	fa21 fe0b 	lsr.w	lr, r1, fp
30008082:	bf18      	it	ne
30008084:	f108 0801 	addne.w	r8, r8, #1
30008088:	ea11 0f0a 	tst.w	r1, sl
3000808c:	990a      	ldr	r1, [sp, #40]	; 0x28
3000808e:	ea4f 3808 	mov.w	r8, r8, lsl #12
30008092:	ea4f 0141 	mov.w	r1, r1, lsl #1
30008096:	bf18      	it	ne
30008098:	f10e 0e01 	addne.w	lr, lr, #1
3000809c:	f408 38f8 	and.w	r8, r8, #126976	; 0x1f000
300080a0:	f001 0102 	and.w	r1, r1, #2
300080a4:	ea4f 5e0e 	mov.w	lr, lr, lsl #20
300080a8:	e9d2 9c0d 	ldrd	r9, ip, [r2, #52]	; 0x34
300080ac:	e9d2 5011 	ldrd	r5, r0, [r2, #68]	; 0x44
300080b0:	ea4f 0989 	mov.w	r9, r9, lsl #2
300080b4:	f00e 7ef8 	and.w	lr, lr, #32505856	; 0x1f00000
300080b8:	900b      	str	r0, [sp, #44]	; 0x2c
300080ba:	ea4f 0ccc 	mov.w	ip, ip, lsl #3
300080be:	9806      	ldr	r0, [sp, #24]
300080c0:	f009 0904 	and.w	r9, r9, #4
300080c4:	f00c 0c08 	and.w	ip, ip, #8
300080c8:	01ad      	lsls	r5, r5, #6
300080ca:	f000 0001 	and.w	r0, r0, #1
300080ce:	ea41 0109 	orr.w	r1, r1, r9
300080d2:	f005 0540 	and.w	r5, r5, #64	; 0x40
300080d6:	4301      	orrs	r1, r0
300080d8:	e9d2 760f 	ldrd	r7, r6, [r2, #60]	; 0x3c
300080dc:	ea41 0c0c 	orr.w	ip, r1, ip
300080e0:	990b      	ldr	r1, [sp, #44]	; 0x2c
300080e2:	013f      	lsls	r7, r7, #4
300080e4:	0176      	lsls	r6, r6, #5
300080e6:	f007 0710 	and.w	r7, r7, #16
300080ea:	0208      	lsls	r0, r1, #8
300080ec:	f006 0620 	and.w	r6, r6, #32
300080f0:	6cd1      	ldr	r1, [r2, #76]	; 0x4c
300080f2:	ea4c 0707 	orr.w	r7, ip, r7
300080f6:	f400 6070 	and.w	r0, r0, #3840	; 0xf00
300080fa:	06c9      	lsls	r1, r1, #27
300080fc:	4337      	orrs	r7, r6
300080fe:	f001 6100 	and.w	r1, r1, #134217728	; 0x8000000
30008102:	432f      	orrs	r7, r5
30008104:	e9d2 6214 	ldrd	r6, r2, [r2, #80]	; 0x50
30008108:	4307      	orrs	r7, r0
3000810a:	0736      	lsls	r6, r6, #28
3000810c:	430f      	orrs	r7, r1
3000810e:	0752      	lsls	r2, r2, #29
30008110:	f006 5680 	and.w	r6, r6, #268435456	; 0x10000000
30008114:	f002 5200 	and.w	r2, r2, #536870912	; 0x20000000
30008118:	433e      	orrs	r6, r7
3000811a:	4316      	orrs	r6, r2
3000811c:	4a55      	ldr	r2, [pc, #340]	; (30008274 <rxi316_dram_init+0x364>)
3000811e:	ea46 0608 	orr.w	r6, r6, r8
30008122:	ea46 060e 	orr.w	r6, r6, lr
30008126:	6096      	str	r6, [r2, #8]
30008128:	6862      	ldr	r2, [r4, #4]
3000812a:	6990      	ldr	r0, [r2, #24]
3000812c:	9a02      	ldr	r2, [sp, #8]
3000812e:	2a10      	cmp	r2, #16
30008130:	6822      	ldr	r2, [r4, #0]
30008132:	7811      	ldrb	r1, [r2, #0]
30008134:	f000 8327 	beq.w	30008786 <rxi316_dram_init+0x876>
30008138:	9e02      	ldr	r6, [sp, #8]
3000813a:	7915      	ldrb	r5, [r2, #4]
3000813c:	2e08      	cmp	r6, #8
3000813e:	f000 82b4 	beq.w	300086aa <rxi316_dram_init+0x79a>
30008142:	9d02      	ldr	r5, [sp, #8]
30008144:	2d04      	cmp	r5, #4
30008146:	f000 83ad 	beq.w	300088a4 <rxi316_dram_init+0x994>
3000814a:	f1a1 0602 	sub.w	r6, r1, #2
3000814e:	9d02      	ldr	r5, [sp, #8]
30008150:	fab6 f686 	clz	r6, r6
30008154:	0976      	lsrs	r6, r6, #5
30008156:	2909      	cmp	r1, #9
30008158:	bf14      	ite	ne
3000815a:	4637      	movne	r7, r6
3000815c:	f046 0701 	orreq.w	r7, r6, #1
30008160:	2d02      	cmp	r5, #2
30008162:	bf08      	it	eq
30008164:	4338      	orreq	r0, r7
30008166:	7915      	ldrb	r5, [r2, #4]
30008168:	2f00      	cmp	r7, #0
3000816a:	f040 82ac 	bne.w	300086c6 <rxi316_dram_init+0x7b6>
3000816e:	2903      	cmp	r1, #3
30008170:	f000 8368 	beq.w	30008844 <rxi316_dram_init+0x934>
30008174:	2600      	movs	r6, #0
30008176:	f44f 5180 	mov.w	r1, #4096	; 0x1000
3000817a:	4637      	mov	r7, r6
3000817c:	433e      	orrs	r6, r7
3000817e:	4f3d      	ldr	r7, [pc, #244]	; (30008274 <rxi316_dram_init+0x364>)
30008180:	430e      	orrs	r6, r1
30008182:	4306      	orrs	r6, r0
30008184:	ea46 05c5 	orr.w	r5, r6, r5, lsl #3
30008188:	637d      	str	r5, [r7, #52]	; 0x34
3000818a:	6862      	ldr	r2, [r4, #4]
3000818c:	9904      	ldr	r1, [sp, #16]
3000818e:	69d2      	ldr	r2, [r2, #28]
30008190:	ea42 02c1 	orr.w	r2, r2, r1, lsl #3
30008194:	63ba      	str	r2, [r7, #56]	; 0x38
30008196:	6822      	ldr	r2, [r4, #0]
30008198:	7810      	ldrb	r0, [r2, #0]
3000819a:	6862      	ldr	r2, [r4, #4]
3000819c:	2803      	cmp	r0, #3
3000819e:	6a11      	ldr	r1, [r2, #32]
300081a0:	f000 82e4 	beq.w	3000876c <rxi316_dram_init+0x85c>
300081a4:	2809      	cmp	r0, #9
300081a6:	f000 8306 	beq.w	300087b6 <rxi316_dram_init+0x8a6>
300081aa:	4832      	ldr	r0, [pc, #200]	; (30008274 <rxi316_dram_init+0x364>)
300081ac:	63c1      	str	r1, [r0, #60]	; 0x3c
300081ae:	6862      	ldr	r2, [r4, #4]
300081b0:	6a52      	ldr	r2, [r2, #36]	; 0x24
300081b2:	6402      	str	r2, [r0, #64]	; 0x40
300081b4:	6860      	ldr	r0, [r4, #4]
300081b6:	68c5      	ldr	r5, [r0, #12]
300081b8:	6802      	ldr	r2, [r0, #0]
300081ba:	6841      	ldr	r1, [r0, #4]
300081bc:	442a      	add	r2, r5
300081be:	6880      	ldr	r0, [r0, #8]
300081c0:	4429      	add	r1, r5
300081c2:	ea1a 0f02 	tst.w	sl, r2
300081c6:	fa22 f20b 	lsr.w	r2, r2, fp
300081ca:	fa21 f60b 	lsr.w	r6, r1, fp
300081ce:	fa20 f50b 	lsr.w	r5, r0, fp
300081d2:	bf18      	it	ne
300081d4:	3201      	addne	r2, #1
300081d6:	ea1a 0f01 	tst.w	sl, r1
300081da:	6821      	ldr	r1, [r4, #0]
300081dc:	bf18      	it	ne
300081de:	3601      	addne	r6, #1
300081e0:	ea1a 0f00 	tst.w	sl, r0
300081e4:	7808      	ldrb	r0, [r1, #0]
300081e6:	bf18      	it	ne
300081e8:	3501      	addne	r5, #1
300081ea:	68a1      	ldr	r1, [r4, #8]
300081ec:	3802      	subs	r0, #2
300081ee:	6dcf      	ldr	r7, [r1, #92]	; 0x5c
300081f0:	2801      	cmp	r0, #1
300081f2:	f240 82b4 	bls.w	3000875e <rxi316_dram_init+0x84e>
300081f6:	698f      	ldr	r7, [r1, #24]
300081f8:	fbb7 f0f3 	udiv	r0, r7, r3
300081fc:	fb03 7710 	mls	r7, r3, r0, r7
30008200:	b107      	cbz	r7, 30008204 <rxi316_dram_init+0x2f4>
30008202:	3001      	adds	r0, #1
30008204:	6a4f      	ldr	r7, [r1, #36]	; 0x24
30008206:	fbb7 f1f3 	udiv	r1, r7, r3
3000820a:	fb03 7711 	mls	r7, r3, r1, r7
3000820e:	b107      	cbz	r7, 30008212 <rxi316_dram_init+0x302>
30008210:	3101      	adds	r1, #1
30008212:	0176      	lsls	r6, r6, #5
30008214:	f002 021f 	and.w	r2, r2, #31
30008218:	02ad      	lsls	r5, r5, #10
3000821a:	f406 7678 	and.w	r6, r6, #992	; 0x3e0
3000821e:	0500      	lsls	r0, r0, #20
30008220:	f405 45f8 	and.w	r5, r5, #31744	; 0x7c00
30008224:	4316      	orrs	r6, r2
30008226:	f000 70f8 	and.w	r0, r0, #32505856	; 0x1f00000
3000822a:	4a12      	ldr	r2, [pc, #72]	; (30008274 <rxi316_dram_init+0x364>)
3000822c:	432e      	orrs	r6, r5
3000822e:	4306      	orrs	r6, r0
30008230:	0648      	lsls	r0, r1, #25
30008232:	f000 5078 	and.w	r0, r0, #1040187392	; 0x3e000000
30008236:	4330      	orrs	r0, r6
30008238:	6310      	str	r0, [r2, #48]	; 0x30
3000823a:	68a0      	ldr	r0, [r4, #8]
3000823c:	6802      	ldr	r2, [r0, #0]
3000823e:	fbb2 f1f3 	udiv	r1, r2, r3
30008242:	fb03 2211 	mls	r2, r3, r1, r2
30008246:	2a00      	cmp	r2, #0
30008248:	f000 8287 	beq.w	3000875a <rxi316_dram_init+0x84a>
3000824c:	f101 0901 	add.w	r9, r1, #1
30008250:	6842      	ldr	r2, [r0, #4]
30008252:	fbb2 f0f3 	udiv	r0, r2, r3
30008256:	fb03 2210 	mls	r2, r3, r0, r2
3000825a:	2a00      	cmp	r2, #0
3000825c:	f000 827b 	beq.w	30008756 <rxi316_dram_init+0x846>
30008260:	38c7      	subs	r0, #199	; 0xc7
30008262:	68e2      	ldr	r2, [r4, #12]
30008264:	6811      	ldr	r1, [r2, #0]
30008266:	4a04      	ldr	r2, [pc, #16]	; (30008278 <rxi316_dram_init+0x368>)
30008268:	ea02 2000 	and.w	r0, r2, r0, lsl #8
3000826c:	e9d1 2117 	ldrd	r2, r1, [r1, #92]	; 0x5c
30008270:	900a      	str	r0, [sp, #40]	; 0x28
30008272:	e003      	b.n	3000827c <rxi316_dram_init+0x36c>
30008274:	41100000 	.word	0x41100000
30008278:	00ffff00 	.word	0x00ffff00
3000827c:	0712      	lsls	r2, r2, #28
3000827e:	ea40 7141 	orr.w	r1, r0, r1, lsl #29
30008282:	f002 5280 	and.w	r2, r2, #268435456	; 0x10000000
30008286:	430a      	orrs	r2, r1
30008288:	fa5f f189 	uxtb.w	r1, r9
3000828c:	430a      	orrs	r2, r1
3000828e:	49c2      	ldr	r1, [pc, #776]	; (30008598 <rxi316_dram_init+0x688>)
30008290:	f042 7280 	orr.w	r2, r2, #16777216	; 0x1000000
30008294:	610a      	str	r2, [r1, #16]
30008296:	68a5      	ldr	r5, [r4, #8]
30008298:	692a      	ldr	r2, [r5, #16]
3000829a:	fbb2 fcf3 	udiv	ip, r2, r3
3000829e:	fb03 221c 	mls	r2, r3, ip, r2
300082a2:	2a00      	cmp	r2, #0
300082a4:	f000 8254 	beq.w	30008750 <rxi316_dram_init+0x840>
300082a8:	f10c 0201 	add.w	r2, ip, #1
300082ac:	9206      	str	r2, [sp, #24]
300082ae:	69aa      	ldr	r2, [r5, #24]
300082b0:	fbb2 f0f3 	udiv	r0, r2, r3
300082b4:	fb03 2210 	mls	r2, r3, r0, r2
300082b8:	b102      	cbz	r2, 300082bc <rxi316_dram_init+0x3ac>
300082ba:	3001      	adds	r0, #1
300082bc:	68ea      	ldr	r2, [r5, #12]
300082be:	fbb2 f1f3 	udiv	r1, r2, r3
300082c2:	fb03 2211 	mls	r2, r3, r1, r2
300082c6:	9104      	str	r1, [sp, #16]
300082c8:	b10a      	cbz	r2, 300082ce <rxi316_dram_init+0x3be>
300082ca:	3101      	adds	r1, #1
300082cc:	9104      	str	r1, [sp, #16]
300082ce:	6822      	ldr	r2, [r4, #0]
300082d0:	7816      	ldrb	r6, [r2, #0]
300082d2:	2e09      	cmp	r6, #9
300082d4:	f000 826b 	beq.w	300087ae <rxi316_dram_init+0x89e>
300082d8:	6a6a      	ldr	r2, [r5, #36]	; 0x24
300082da:	2e02      	cmp	r6, #2
300082dc:	fbb2 f1f3 	udiv	r1, r2, r3
300082e0:	fb03 2211 	mls	r2, r3, r1, r2
300082e4:	f000 828b 	beq.w	300087fe <rxi316_dram_init+0x8ee>
300082e8:	b102      	cbz	r2, 300082ec <rxi316_dram_init+0x3dc>
300082ea:	3101      	adds	r1, #1
300082ec:	68e2      	ldr	r2, [r4, #12]
300082ee:	0349      	lsls	r1, r1, #13
300082f0:	6d2d      	ldr	r5, [r5, #80]	; 0x50
300082f2:	6812      	ldr	r2, [r2, #0]
300082f4:	f401 31f0 	and.w	r1, r1, #122880	; 0x1e000
300082f8:	fa25 f60b 	lsr.w	r6, r5, fp
300082fc:	ea15 0f0a 	tst.w	r5, sl
30008300:	6e52      	ldr	r2, [r2, #100]	; 0x64
30008302:	bf18      	it	ne
30008304:	3601      	addne	r6, #1
30008306:	05d2      	lsls	r2, r2, #23
30008308:	f402 0200 	and.w	r2, r2, #8388608	; 0x800000
3000830c:	ea42 6606 	orr.w	r6, r2, r6, lsl #24
30008310:	9a06      	ldr	r2, [sp, #24]
30008312:	0112      	lsls	r2, r2, #4
30008314:	f402 72f8 	and.w	r2, r2, #496	; 0x1f0
30008318:	4332      	orrs	r2, r6
3000831a:	0246      	lsls	r6, r0, #9
3000831c:	f406 56f0 	and.w	r6, r6, #7680	; 0x1e00
30008320:	4316      	orrs	r6, r2
30008322:	9a04      	ldr	r2, [sp, #16]
30008324:	f002 020f 	and.w	r2, r2, #15
30008328:	4316      	orrs	r6, r2
3000832a:	4a9b      	ldr	r2, [pc, #620]	; (30008598 <rxi316_dram_init+0x688>)
3000832c:	4331      	orrs	r1, r6
3000832e:	f441 11a0 	orr.w	r1, r1, #1310720	; 0x140000
30008332:	6151      	str	r1, [r2, #20]
30008334:	68a7      	ldr	r7, [r4, #8]
30008336:	697a      	ldr	r2, [r7, #20]
30008338:	fbb2 fef3 	udiv	lr, r2, r3
3000833c:	fb03 221e 	mls	r2, r3, lr, r2
30008340:	b10a      	cbz	r2, 30008346 <rxi316_dram_init+0x436>
30008342:	f10e 0e01 	add.w	lr, lr, #1
30008346:	6939      	ldr	r1, [r7, #16]
30008348:	fbb1 f2f3 	udiv	r2, r1, r3
3000834c:	fb03 1112 	mls	r1, r3, r2, r1
30008350:	b101      	cbz	r1, 30008354 <rxi316_dram_init+0x444>
30008352:	3201      	adds	r2, #1
30008354:	68fd      	ldr	r5, [r7, #12]
30008356:	fbb5 f1f3 	udiv	r1, r5, r3
3000835a:	fb03 5511 	mls	r5, r3, r1, r5
3000835e:	b105      	cbz	r5, 30008362 <rxi316_dram_init+0x452>
30008360:	3101      	adds	r1, #1
30008362:	6b3d      	ldr	r5, [r7, #48]	; 0x30
30008364:	fbb5 f6f3 	udiv	r6, r5, r3
30008368:	fb03 5516 	mls	r5, r3, r6, r5
3000836c:	b105      	cbz	r5, 30008370 <rxi316_dram_init+0x460>
3000836e:	3601      	adds	r6, #1
30008370:	4411      	add	r1, r2
30008372:	68bd      	ldr	r5, [r7, #8]
30008374:	42b1      	cmp	r1, r6
30008376:	460a      	mov	r2, r1
30008378:	bf38      	it	cc
3000837a:	4632      	movcc	r2, r6
3000837c:	920b      	str	r2, [sp, #44]	; 0x2c
3000837e:	fbb5 f2f3 	udiv	r2, r5, r3
30008382:	fb03 5512 	mls	r5, r3, r2, r5
30008386:	b905      	cbnz	r5, 3000838a <rxi316_dram_init+0x47a>
30008388:	3a01      	subs	r2, #1
3000838a:	6afd      	ldr	r5, [r7, #44]	; 0x2c
3000838c:	ea15 0f0a 	tst.w	r5, sl
30008390:	fa25 fc0b 	lsr.w	ip, r5, fp
30008394:	69fd      	ldr	r5, [r7, #28]
30008396:	bf18      	it	ne
30008398:	f10c 0c01 	addne.w	ip, ip, #1
3000839c:	fa25 f60b 	lsr.w	r6, r5, fp
300083a0:	ea15 0f0a 	tst.w	r5, sl
300083a4:	9d01      	ldr	r5, [sp, #4]
300083a6:	bf18      	it	ne
300083a8:	3601      	addne	r6, #1
300083aa:	2d09      	cmp	r5, #9
300083ac:	f000 81fa 	beq.w	300087a4 <rxi316_dram_init+0x894>
300083b0:	9d01      	ldr	r5, [sp, #4]
300083b2:	2d02      	cmp	r5, #2
300083b4:	f000 822b 	beq.w	3000880e <rxi316_dram_init+0x8fe>
300083b8:	2d03      	cmp	r5, #3
300083ba:	f000 829b 	beq.w	300088f4 <rxi316_dram_init+0x9e4>
300083be:	9d09      	ldr	r5, [sp, #36]	; 0x24
300083c0:	9902      	ldr	r1, [sp, #8]
300083c2:	440d      	add	r5, r1
300083c4:	46a8      	mov	r8, r5
300083c6:	fa28 f50b 	lsr.w	r5, r8, fp
300083ca:	ea18 0f0a 	tst.w	r8, sl
300083ce:	f8d7 8040 	ldr.w	r8, [r7, #64]	; 0x40
300083d2:	bf18      	it	ne
300083d4:	3501      	addne	r5, #1
300083d6:	fbb8 f7f3 	udiv	r7, r8, r3
300083da:	fb03 8817 	mls	r8, r3, r7, r8
300083de:	f1b8 0f00 	cmp.w	r8, #0
300083e2:	d000      	beq.n	300083e6 <rxi316_dram_init+0x4d6>
300083e4:	3701      	adds	r7, #1
300083e6:	990b      	ldr	r1, [sp, #44]	; 0x2c
300083e8:	f00e 0e0f 	and.w	lr, lr, #15
300083ec:	ea4f 3c8c 	mov.w	ip, ip, lsl #14
300083f0:	0109      	lsls	r1, r1, #4
300083f2:	f40c 3ce0 	and.w	ip, ip, #114688	; 0x1c000
300083f6:	063f      	lsls	r7, r7, #24
300083f8:	f401 717c 	and.w	r1, r1, #1008	; 0x3f0
300083fc:	f007 57f8 	and.w	r7, r7, #520093696	; 0x1f000000
30008400:	ea41 0e0e 	orr.w	lr, r1, lr
30008404:	0291      	lsls	r1, r2, #10
30008406:	f401 5170 	and.w	r1, r1, #15360	; 0x3c00
3000840a:	ea4e 0e01 	orr.w	lr, lr, r1
3000840e:	0471      	lsls	r1, r6, #17
30008410:	ea4e 0c0c 	orr.w	ip, lr, ip
30008414:	f401 2160 	and.w	r1, r1, #917504	; 0xe0000
30008418:	ea4c 0c01 	orr.w	ip, ip, r1
3000841c:	0529      	lsls	r1, r5, #20
3000841e:	f401 0170 	and.w	r1, r1, #15728640	; 0xf00000
30008422:	ea4c 0c01 	orr.w	ip, ip, r1
30008426:	495c      	ldr	r1, [pc, #368]	; (30008598 <rxi316_dram_init+0x688>)
30008428:	ea4c 0707 	orr.w	r7, ip, r7
3000842c:	618f      	str	r7, [r1, #24]
3000842e:	6821      	ldr	r1, [r4, #0]
30008430:	780f      	ldrb	r7, [r1, #0]
30008432:	2f02      	cmp	r7, #2
30008434:	f000 815e 	beq.w	300086f4 <rxi316_dram_init+0x7e4>
30008438:	2f09      	cmp	r7, #9
3000843a:	f000 815b 	beq.w	300086f4 <rxi316_dram_init+0x7e4>
3000843e:	68e1      	ldr	r1, [r4, #12]
30008440:	2f03      	cmp	r7, #3
30008442:	6809      	ldr	r1, [r1, #0]
30008444:	6e89      	ldr	r1, [r1, #104]	; 0x68
30008446:	f001 010f 	and.w	r1, r1, #15
3000844a:	9109      	str	r1, [sp, #36]	; 0x24
3000844c:	68a1      	ldr	r1, [r4, #8]
3000844e:	f000 81a7 	beq.w	300087a0 <rxi316_dram_init+0x890>
30008452:	6a0f      	ldr	r7, [r1, #32]
30008454:	ea17 0f0a 	tst.w	r7, sl
30008458:	fa27 f10b 	lsr.w	r1, r7, fp
3000845c:	d000      	beq.n	30008460 <rxi316_dram_init+0x550>
3000845e:	3101      	adds	r1, #1
30008460:	9f01      	ldr	r7, [sp, #4]
30008462:	2f02      	cmp	r7, #2
30008464:	f000 815c 	beq.w	30008720 <rxi316_dram_init+0x810>
30008468:	9f01      	ldr	r7, [sp, #4]
3000846a:	2f03      	cmp	r7, #3
3000846c:	f000 81a5 	beq.w	300087ba <rxi316_dram_init+0x8aa>
30008470:	9f01      	ldr	r7, [sp, #4]
30008472:	2f09      	cmp	r7, #9
30008474:	f000 822e 	beq.w	300088d4 <rxi316_dram_init+0x9c4>
30008478:	f04f 0e00 	mov.w	lr, #0
3000847c:	46f0      	mov	r8, lr
3000847e:	f8cd e004 	str.w	lr, [sp, #4]
30008482:	9f06      	ldr	r7, [sp, #24]
30008484:	0109      	lsls	r1, r1, #4
30008486:	ea4f 1c57 	mov.w	ip, r7, lsr #5
3000848a:	9f09      	ldr	r7, [sp, #36]	; 0x24
3000848c:	b2c9      	uxtb	r1, r1
3000848e:	4339      	orrs	r1, r7
30008490:	9f01      	ldr	r7, [sp, #4]
30008492:	4339      	orrs	r1, r7
30008494:	4f40      	ldr	r7, [pc, #256]	; (30008598 <rxi316_dram_init+0x688>)
30008496:	ea41 0108 	orr.w	r1, r1, r8
3000849a:	ea41 010e 	orr.w	r1, r1, lr
3000849e:	61f9      	str	r1, [r7, #28]
300084a0:	68a7      	ldr	r7, [r4, #8]
300084a2:	6c3c      	ldr	r4, [r7, #64]	; 0x40
300084a4:	fbb4 f1f3 	udiv	r1, r4, r3
300084a8:	fb03 4311 	mls	r3, r3, r1, r4
300084ac:	b103      	cbz	r3, 300084b0 <rxi316_dram_init+0x5a0>
300084ae:	3101      	adds	r1, #1
300084b0:	6b7c      	ldr	r4, [r7, #52]	; 0x34
300084b2:	094b      	lsrs	r3, r1, #5
300084b4:	ea4f 2119 	mov.w	r1, r9, lsr #8
300084b8:	f8d7 903c 	ldr.w	r9, [r7, #60]	; 0x3c
300084bc:	ea14 0f0a 	tst.w	r4, sl
300084c0:	fa24 f80b 	lsr.w	r8, r4, fp
300084c4:	6bbc      	ldr	r4, [r7, #56]	; 0x38
300084c6:	ea4f 1212 	mov.w	r2, r2, lsr #4
300084ca:	bf18      	it	ne
300084cc:	f108 0801 	addne.w	r8, r8, #1
300084d0:	9301      	str	r3, [sp, #4]
300084d2:	ea14 0f0a 	tst.w	r4, sl
300084d6:	fa24 fe0b 	lsr.w	lr, r4, fp
300084da:	9b02      	ldr	r3, [sp, #8]
300084dc:	fa29 f40b 	lsr.w	r4, r9, fp
300084e0:	bf18      	it	ne
300084e2:	f10e 0e01 	addne.w	lr, lr, #1
300084e6:	6aff      	ldr	r7, [r7, #44]	; 0x2c
300084e8:	ea19 0f0a 	tst.w	r9, sl
300084ec:	fa23 f90b 	lsr.w	r9, r3, fp
300084f0:	ea4f 06d6 	mov.w	r6, r6, lsr #3
300084f4:	bf18      	it	ne
300084f6:	3401      	addne	r4, #1
300084f8:	45b9      	cmp	r9, r7
300084fa:	ea4f 1515 	mov.w	r5, r5, lsr #4
300084fe:	ea4f 1010 	mov.w	r0, r0, lsr #4
30008502:	f240 80f5 	bls.w	300086f0 <rxi316_dram_init+0x7e0>
30008506:	eba9 0707 	sub.w	r7, r9, r7
3000850a:	fa27 f40b 	lsr.w	r4, r7, fp
3000850e:	ea17 070a 	ands.w	r7, r7, sl
30008512:	d001      	beq.n	30008518 <rxi316_dram_init+0x608>
30008514:	3401      	adds	r4, #1
30008516:	2700      	movs	r7, #0
30008518:	9b05      	ldr	r3, [sp, #20]
3000851a:	b10b      	cbz	r3, 30008520 <rxi316_dram_init+0x610>
3000851c:	073b      	lsls	r3, r7, #28
3000851e:	9305      	str	r3, [sp, #20]
30008520:	ea4f 0c8c 	mov.w	ip, ip, lsl #2
30008524:	9b01      	ldr	r3, [sp, #4]
30008526:	0289      	lsls	r1, r1, #10
30008528:	02c0      	lsls	r0, r0, #11
3000852a:	f00c 0c04 	and.w	ip, ip, #4
3000852e:	f401 6180 	and.w	r1, r1, #1024	; 0x400
30008532:	01d2      	lsls	r2, r2, #7
30008534:	f400 6000 	and.w	r0, r0, #2048	; 0x800
30008538:	0236      	lsls	r6, r6, #8
3000853a:	ea4c 0c01 	orr.w	ip, ip, r1
3000853e:	b2d2      	uxtb	r2, r2
30008540:	026d      	lsls	r5, r5, #9
30008542:	f406 7680 	and.w	r6, r6, #256	; 0x100
30008546:	ea4c 0c00 	orr.w	ip, ip, r0
3000854a:	00df      	lsls	r7, r3, #3
3000854c:	f405 7500 	and.w	r5, r5, #512	; 0x200
30008550:	ea4f 3888 	mov.w	r8, r8, lsl #14
30008554:	ea4c 0202 	orr.w	r2, ip, r2
30008558:	f007 0708 	and.w	r7, r7, #8
3000855c:	ea4f 4e4e 	mov.w	lr, lr, lsl #17
30008560:	f408 38e0 	and.w	r8, r8, #114688	; 0x1c000
30008564:	4332      	orrs	r2, r6
30008566:	0524      	lsls	r4, r4, #20
30008568:	f40e 2e60 	and.w	lr, lr, #917504	; 0xe0000
3000856c:	490a      	ldr	r1, [pc, #40]	; (30008598 <rxi316_dram_init+0x688>)
3000856e:	ea42 0305 	orr.w	r3, r2, r5
30008572:	f404 0470 	and.w	r4, r4, #15728640	; 0xf00000
30008576:	9a05      	ldr	r2, [sp, #20]
30008578:	433b      	orrs	r3, r7
3000857a:	ea43 0308 	orr.w	r3, r3, r8
3000857e:	ea43 030e 	orr.w	r3, r3, lr
30008582:	4323      	orrs	r3, r4
30008584:	4313      	orrs	r3, r2
30008586:	9a04      	ldr	r2, [sp, #16]
30008588:	f3c2 1200 	ubfx	r2, r2, #4, #1
3000858c:	620b      	str	r3, [r1, #32]
3000858e:	f8c1 2090 	str.w	r2, [r1, #144]	; 0x90
30008592:	9b03      	ldr	r3, [sp, #12]
30008594:	e002      	b.n	3000859c <rxi316_dram_init+0x68c>
30008596:	bf00      	nop
30008598:	41100000 	.word	0x41100000
3000859c:	2b01      	cmp	r3, #1
3000859e:	d002      	beq.n	300085a6 <rxi316_dram_init+0x696>
300085a0:	9b08      	ldr	r3, [sp, #32]
300085a2:	3303      	adds	r3, #3
300085a4:	9307      	str	r3, [sp, #28]
300085a6:	4bb4      	ldr	r3, [pc, #720]	; (30008878 <rxi316_dram_init+0x968>)
300085a8:	f64f 71ff 	movw	r1, #65535	; 0xffff
300085ac:	9a07      	ldr	r2, [sp, #28]
300085ae:	f8c3 2094 	str.w	r2, [r3, #148]	; 0x94
300085b2:	f44f 62e0 	mov.w	r2, #1792	; 0x700
300085b6:	f8c3 10cc 	str.w	r1, [r3, #204]	; 0xcc
300085ba:	60da      	str	r2, [r3, #12]
300085bc:	f8d3 2100 	ldr.w	r2, [r3, #256]	; 0x100
300085c0:	430a      	orrs	r2, r1
300085c2:	49ae      	ldr	r1, [pc, #696]	; (3000887c <rxi316_dram_init+0x96c>)
300085c4:	f8c3 2100 	str.w	r2, [r3, #256]	; 0x100
300085c8:	f8d1 2268 	ldr.w	r2, [r1, #616]	; 0x268
300085cc:	0096      	lsls	r6, r2, #2
300085ce:	f140 8089 	bpl.w	300086e4 <rxi316_dram_init+0x7d4>
300085d2:	f04f 4200 	mov.w	r2, #2147483648	; 0x80000000
300085d6:	4ca8      	ldr	r4, [pc, #672]	; (30008878 <rxi316_dram_init+0x968>)
300085d8:	601a      	str	r2, [r3, #0]
300085da:	691a      	ldr	r2, [r3, #16]
300085dc:	f042 5280 	orr.w	r2, r2, #268435456	; 0x10000000
300085e0:	611a      	str	r2, [r3, #16]
300085e2:	f8d1 32fc 	ldr.w	r3, [r1, #764]	; 0x2fc
300085e6:	f043 0303 	orr.w	r3, r3, #3
300085ea:	f8c1 32fc 	str.w	r3, [r1, #764]	; 0x2fc
300085ee:	f44f 0160 	mov.w	r1, #14680064	; 0xe00000
300085f2:	9b0c      	ldr	r3, [sp, #48]	; 0x30
300085f4:	2b01      	cmp	r3, #1
300085f6:	4ba2      	ldr	r3, [pc, #648]	; (30008880 <rxi316_dram_init+0x970>)
300085f8:	bf0c      	ite	eq
300085fa:	4aa2      	ldreq	r2, [pc, #648]	; (30008884 <rxi316_dram_init+0x974>)
300085fc:	4aa2      	ldrne	r2, [pc, #648]	; (30008888 <rxi316_dram_init+0x978>)
300085fe:	601a      	str	r2, [r3, #0]
30008600:	2203      	movs	r2, #3
30008602:	68e0      	ldr	r0, [r4, #12]
30008604:	4ba1      	ldr	r3, [pc, #644]	; (3000888c <rxi316_dram_init+0x97c>)
30008606:	4003      	ands	r3, r0
30008608:	60e3      	str	r3, [r4, #12]
3000860a:	2308      	movs	r3, #8
3000860c:	62a1      	str	r1, [r4, #40]	; 0x28
3000860e:	62e2      	str	r2, [r4, #44]	; 0x2c
30008610:	6023      	str	r3, [r4, #0]
30008612:	6823      	ldr	r3, [r4, #0]
30008614:	071d      	lsls	r5, r3, #28
30008616:	d5fc      	bpl.n	30008612 <rxi316_dram_init+0x702>
30008618:	4b9d      	ldr	r3, [pc, #628]	; (30008890 <rxi316_dram_init+0x980>)
3000861a:	2001      	movs	r0, #1
3000861c:	4798      	blx	r3
3000861e:	68e2      	ldr	r2, [r4, #12]
30008620:	4b9a      	ldr	r3, [pc, #616]	; (3000888c <rxi316_dram_init+0x97c>)
30008622:	f44f 1100 	mov.w	r1, #2097152	; 0x200000
30008626:	4013      	ands	r3, r2
30008628:	2203      	movs	r2, #3
3000862a:	60e3      	str	r3, [r4, #12]
3000862c:	2308      	movs	r3, #8
3000862e:	62a1      	str	r1, [r4, #40]	; 0x28
30008630:	62e2      	str	r2, [r4, #44]	; 0x2c
30008632:	4a91      	ldr	r2, [pc, #580]	; (30008878 <rxi316_dram_init+0x968>)
30008634:	6023      	str	r3, [r4, #0]
30008636:	6813      	ldr	r3, [r2, #0]
30008638:	071c      	lsls	r4, r3, #28
3000863a:	d5fc      	bpl.n	30008636 <rxi316_dram_init+0x726>
3000863c:	68d0      	ldr	r0, [r2, #12]
3000863e:	f04f 7190 	mov.w	r1, #18874368	; 0x1200000
30008642:	4b92      	ldr	r3, [pc, #584]	; (3000888c <rxi316_dram_init+0x97c>)
30008644:	4003      	ands	r3, r0
30008646:	2008      	movs	r0, #8
30008648:	60d3      	str	r3, [r2, #12]
3000864a:	6291      	str	r1, [r2, #40]	; 0x28
3000864c:	6ad3      	ldr	r3, [r2, #44]	; 0x2c
3000864e:	498a      	ldr	r1, [pc, #552]	; (30008878 <rxi316_dram_init+0x968>)
30008650:	f023 0302 	bic.w	r3, r3, #2
30008654:	62d3      	str	r3, [r2, #44]	; 0x2c
30008656:	6010      	str	r0, [r2, #0]
30008658:	680b      	ldr	r3, [r1, #0]
3000865a:	0718      	lsls	r0, r3, #28
3000865c:	d5fc      	bpl.n	30008658 <rxi316_dram_init+0x748>
3000865e:	4a86      	ldr	r2, [pc, #536]	; (30008878 <rxi316_dram_init+0x968>)
30008660:	f44f 63c0 	mov.w	r3, #1536	; 0x600
30008664:	f04f 4100 	mov.w	r1, #2147483648	; 0x80000000
30008668:	60d3      	str	r3, [r2, #12]
3000866a:	980a      	ldr	r0, [sp, #40]	; 0x28
3000866c:	6913      	ldr	r3, [r2, #16]
3000866e:	4318      	orrs	r0, r3
30008670:	6110      	str	r0, [r2, #16]
30008672:	6913      	ldr	r3, [r2, #16]
30008674:	f023 5380 	bic.w	r3, r3, #268435456	; 0x10000000
30008678:	6113      	str	r3, [r2, #16]
3000867a:	6011      	str	r1, [r2, #0]
3000867c:	68d3      	ldr	r3, [r2, #12]
3000867e:	f3c3 030a 	ubfx	r3, r3, #0, #11
30008682:	f5b3 6fc0 	cmp.w	r3, #1536	; 0x600
30008686:	d1f9      	bne.n	3000867c <rxi316_dram_init+0x76c>
30008688:	b00f      	add	sp, #60	; 0x3c
3000868a:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
3000868e:	2a01      	cmp	r2, #1
30008690:	f000 80c6 	beq.w	30008820 <rxi316_dram_init+0x910>
30008694:	2a02      	cmp	r2, #2
30008696:	f000 8138 	beq.w	3000890a <rxi316_dram_init+0x9fa>
3000869a:	2a03      	cmp	r2, #3
3000869c:	f47f acde 	bne.w	3000805c <rxi316_dram_init+0x14c>
300086a0:	686a      	ldr	r2, [r5, #4]
300086a2:	f042 0280 	orr.w	r2, r2, #128	; 0x80
300086a6:	606a      	str	r2, [r5, #4]
300086a8:	e4d8      	b.n	3000805c <rxi316_dram_init+0x14c>
300086aa:	f1a1 0602 	sub.w	r6, r1, #2
300086ae:	2909      	cmp	r1, #9
300086b0:	fab6 f686 	clz	r6, r6
300086b4:	ea4f 1656 	mov.w	r6, r6, lsr #5
300086b8:	d002      	beq.n	300086c0 <rxi316_dram_init+0x7b0>
300086ba:	2e00      	cmp	r6, #0
300086bc:	f43f ad57 	beq.w	3000816e <rxi316_dram_init+0x25e>
300086c0:	2503      	movs	r5, #3
300086c2:	4328      	orrs	r0, r5
300086c4:	7915      	ldrb	r5, [r2, #4]
300086c6:	68a2      	ldr	r2, [r4, #8]
300086c8:	9f09      	ldr	r7, [sp, #36]	; 0x24
300086ca:	6dd2      	ldr	r2, [r2, #92]	; 0x5c
300086cc:	013f      	lsls	r7, r7, #4
300086ce:	2e00      	cmp	r6, #0
300086d0:	f000 80c0 	beq.w	30008854 <rxi316_dram_init+0x944>
300086d4:	1e91      	subs	r1, r2, #2
300086d6:	2906      	cmp	r1, #6
300086d8:	f200 809c 	bhi.w	30008814 <rxi316_dram_init+0x904>
300086dc:	1e56      	subs	r6, r2, #1
300086de:	2100      	movs	r1, #0
300086e0:	0276      	lsls	r6, r6, #9
300086e2:	e54b      	b.n	3000817c <rxi316_dram_init+0x26c>
300086e4:	2201      	movs	r2, #1
300086e6:	601a      	str	r2, [r3, #0]
300086e8:	681a      	ldr	r2, [r3, #0]
300086ea:	07d2      	lsls	r2, r2, #31
300086ec:	d5fc      	bpl.n	300086e8 <rxi316_dram_init+0x7d8>
300086ee:	e7b6      	b.n	3000865e <rxi316_dram_init+0x74e>
300086f0:	2701      	movs	r7, #1
300086f2:	e711      	b.n	30008518 <rxi316_dram_init+0x608>
300086f4:	68a1      	ldr	r1, [r4, #8]
300086f6:	2f03      	cmp	r7, #3
300086f8:	f8d1 c020 	ldr.w	ip, [r1, #32]
300086fc:	d04e      	beq.n	3000879c <rxi316_dram_init+0x88c>
300086fe:	2f02      	cmp	r7, #2
30008700:	f040 810e 	bne.w	30008920 <rxi316_dram_init+0xa10>
30008704:	fbbc f1f3 	udiv	r1, ip, r3
30008708:	fb03 cc11 	mls	ip, r3, r1, ip
3000870c:	f1bc 0f00 	cmp.w	ip, #0
30008710:	f000 8093 	beq.w	3000883a <rxi316_dram_init+0x92a>
30008714:	9709      	str	r7, [sp, #36]	; 0x24
30008716:	3101      	adds	r1, #1
30008718:	9f01      	ldr	r7, [sp, #4]
3000871a:	2f02      	cmp	r7, #2
3000871c:	f47f aea4 	bne.w	30008468 <rxi316_dram_init+0x558>
30008720:	4f5c      	ldr	r7, [pc, #368]	; (30008894 <rxi316_dram_init+0x984>)
30008722:	f04f 0e00 	mov.w	lr, #0
30008726:	f8df 8170 	ldr.w	r8, [pc, #368]	; 30008898 <rxi316_dram_init+0x988>
3000872a:	fbb7 f7f3 	udiv	r7, r7, r3
3000872e:	fbb8 f8f3 	udiv	r8, r8, r3
30008732:	09ff      	lsrs	r7, r7, #7
30008734:	ea4f 2898 	mov.w	r8, r8, lsr #10
30008738:	3701      	adds	r7, #1
3000873a:	f108 0c01 	add.w	ip, r8, #1
3000873e:	f8df 815c 	ldr.w	r8, [pc, #348]	; 3000889c <rxi316_dram_init+0x98c>
30008742:	023f      	lsls	r7, r7, #8
30008744:	ea08 380c 	and.w	r8, r8, ip, lsl #12
30008748:	f407 6770 	and.w	r7, r7, #3840	; 0xf00
3000874c:	9701      	str	r7, [sp, #4]
3000874e:	e698      	b.n	30008482 <rxi316_dram_init+0x572>
30008750:	f8cd c018 	str.w	ip, [sp, #24]
30008754:	e5ab      	b.n	300082ae <rxi316_dram_init+0x39e>
30008756:	38c8      	subs	r0, #200	; 0xc8
30008758:	e583      	b.n	30008262 <rxi316_dram_init+0x352>
3000875a:	4689      	mov	r9, r1
3000875c:	e578      	b.n	30008250 <rxi316_dram_init+0x340>
3000875e:	ea1a 0f07 	tst.w	sl, r7
30008762:	fa27 f00b 	lsr.w	r0, r7, fp
30008766:	f43f ad4d 	beq.w	30008204 <rxi316_dram_init+0x2f4>
3000876a:	e54a      	b.n	30008202 <rxi316_dram_init+0x2f2>
3000876c:	9a0d      	ldr	r2, [sp, #52]	; 0x34
3000876e:	1f50      	subs	r0, r2, #5
30008770:	00c0      	lsls	r0, r0, #3
30008772:	68e2      	ldr	r2, [r4, #12]
30008774:	6812      	ldr	r2, [r2, #0]
30008776:	6b92      	ldr	r2, [r2, #56]	; 0x38
30008778:	fab2 f282 	clz	r2, r2
3000877c:	0952      	lsrs	r2, r2, #5
3000877e:	ea41 2142 	orr.w	r1, r1, r2, lsl #9
30008782:	4301      	orrs	r1, r0
30008784:	e511      	b.n	300081aa <rxi316_dram_init+0x29a>
30008786:	f1a1 0602 	sub.w	r6, r1, #2
3000878a:	2909      	cmp	r1, #9
3000878c:	fab6 f686 	clz	r6, r6
30008790:	ea4f 1656 	mov.w	r6, r6, lsr #5
30008794:	f000 80b7 	beq.w	30008906 <rxi316_dram_init+0x9f6>
30008798:	4637      	mov	r7, r6
3000879a:	e4e4      	b.n	30008166 <rxi316_dram_init+0x256>
3000879c:	2702      	movs	r7, #2
3000879e:	9709      	str	r7, [sp, #36]	; 0x24
300087a0:	6a09      	ldr	r1, [r1, #32]
300087a2:	e65d      	b.n	30008460 <rxi316_dram_init+0x550>
300087a4:	e9dd 1508 	ldrd	r1, r5, [sp, #32]
300087a8:	440d      	add	r5, r1
300087aa:	46a8      	mov	r8, r5
300087ac:	e60b      	b.n	300083c6 <rxi316_dram_init+0x4b6>
300087ae:	9a08      	ldr	r2, [sp, #32]
300087b0:	fa22 f10b 	lsr.w	r1, r2, fp
300087b4:	e59a      	b.n	300082ec <rxi316_dram_init+0x3dc>
300087b6:	2000      	movs	r0, #0
300087b8:	e7db      	b.n	30008772 <rxi316_dram_init+0x862>
300087ba:	f242 7710 	movw	r7, #10000	; 0x2710
300087be:	f8df 80e0 	ldr.w	r8, [pc, #224]	; 300088a0 <rxi316_dram_init+0x990>
300087c2:	f8df e0d4 	ldr.w	lr, [pc, #212]	; 30008898 <rxi316_dram_init+0x988>
300087c6:	fbb7 f7f3 	udiv	r7, r7, r3
300087ca:	444f      	add	r7, r9
300087cc:	fbb8 f8f3 	udiv	r8, r8, r3
300087d0:	fbbe fef3 	udiv	lr, lr, r3
300087d4:	09ff      	lsrs	r7, r7, #7
300087d6:	ea4f 2898 	mov.w	r8, r8, lsr #10
300087da:	3701      	adds	r7, #1
300087dc:	ea4f 2e9e 	mov.w	lr, lr, lsr #10
300087e0:	f108 0c01 	add.w	ip, r8, #1
300087e4:	f8df 80b4 	ldr.w	r8, [pc, #180]	; 3000889c <rxi316_dram_init+0x98c>
300087e8:	023f      	lsls	r7, r7, #8
300087ea:	f10e 0e01 	add.w	lr, lr, #1
300087ee:	ea08 380c 	and.w	r8, r8, ip, lsl #12
300087f2:	f407 6770 	and.w	r7, r7, #3840	; 0xf00
300087f6:	ea4f 5e8e 	mov.w	lr, lr, lsl #22
300087fa:	9701      	str	r7, [sp, #4]
300087fc:	e641      	b.n	30008482 <rxi316_dram_init+0x572>
300087fe:	9e08      	ldr	r6, [sp, #32]
30008800:	3e02      	subs	r6, #2
30008802:	fa26 f60b 	lsr.w	r6, r6, fp
30008806:	b102      	cbz	r2, 3000880a <rxi316_dram_init+0x8fa>
30008808:	3101      	adds	r1, #1
3000880a:	4431      	add	r1, r6
3000880c:	e56e      	b.n	300082ec <rxi316_dram_init+0x3dc>
3000880e:	f8dd 801c 	ldr.w	r8, [sp, #28]
30008812:	e5d8      	b.n	300083c6 <rxi316_dram_init+0x4b6>
30008814:	2100      	movs	r1, #0
30008816:	460e      	mov	r6, r1
30008818:	e4b0      	b.n	3000817c <rxi316_dram_init+0x26c>
3000881a:	3a07      	subs	r2, #7
3000881c:	f7ff bbbd 	b.w	30007f9a <rxi316_dram_init+0x8a>
30008820:	686a      	ldr	r2, [r5, #4]
30008822:	f042 0220 	orr.w	r2, r2, #32
30008826:	606a      	str	r2, [r5, #4]
30008828:	e418      	b.n	3000805c <rxi316_dram_init+0x14c>
3000882a:	2204      	movs	r2, #4
3000882c:	2102      	movs	r1, #2
3000882e:	2500      	movs	r5, #0
30008830:	9202      	str	r2, [sp, #8]
30008832:	e9cd 2107 	strd	r2, r1, [sp, #28]
30008836:	f7ff bb99 	b.w	30007f6c <rxi316_dram_init+0x5c>
3000883a:	9709      	str	r7, [sp, #36]	; 0x24
3000883c:	e610      	b.n	30008460 <rxi316_dram_init+0x550>
3000883e:	3a08      	subs	r2, #8
30008840:	f7ff bbab 	b.w	30007f9a <rxi316_dram_init+0x8a>
30008844:	9a09      	ldr	r2, [sp, #36]	; 0x24
30008846:	2a04      	cmp	r2, #4
30008848:	d909      	bls.n	3000885e <rxi316_dram_init+0x94e>
3000884a:	2a0b      	cmp	r2, #11
3000884c:	d874      	bhi.n	30008938 <rxi316_dram_init+0xa28>
3000884e:	1f17      	subs	r7, r2, #4
30008850:	013f      	lsls	r7, r7, #4
30008852:	e005      	b.n	30008860 <rxi316_dram_init+0x950>
30008854:	2903      	cmp	r1, #3
30008856:	d005      	beq.n	30008864 <rxi316_dram_init+0x954>
30008858:	f44f 5180 	mov.w	r1, #4096	; 0x1000
3000885c:	e48e      	b.n	3000817c <rxi316_dram_init+0x26c>
3000885e:	2710      	movs	r7, #16
30008860:	68a2      	ldr	r2, [r4, #8]
30008862:	6dd2      	ldr	r2, [r2, #92]	; 0x5c
30008864:	2a04      	cmp	r2, #4
30008866:	d956      	bls.n	30008916 <rxi316_dram_init+0xa06>
30008868:	1f51      	subs	r1, r2, #5
3000886a:	2903      	cmp	r1, #3
3000886c:	d85b      	bhi.n	30008926 <rxi316_dram_init+0xa16>
3000886e:	1f16      	subs	r6, r2, #4
30008870:	f44f 5180 	mov.w	r1, #4096	; 0x1000
30008874:	0276      	lsls	r6, r6, #9
30008876:	e481      	b.n	3000817c <rxi316_dram_init+0x26c>
30008878:	41100000 	.word	0x41100000
3000887c:	42008000 	.word	0x42008000
30008880:	41011000 	.word	0x41011000
30008884:	44000103 	.word	0x44000103
30008888:	44000021 	.word	0x44000021
3000888c:	fff8fffc 	.word	0xfff8fffc
30008890:	00009b2d 	.word	0x00009b2d
30008894:	00061a80 	.word	0x00061a80
30008898:	0bebc200 	.word	0x0bebc200
3000889c:	003ff000 	.word	0x003ff000
300088a0:	1dcd6500 	.word	0x1dcd6500
300088a4:	f1a1 0c09 	sub.w	ip, r1, #9
300088a8:	f1a1 0602 	sub.w	r6, r1, #2
300088ac:	1e8d      	subs	r5, r1, #2
300088ae:	fabc fc8c 	clz	ip, ip
300088b2:	fab6 f686 	clz	r6, r6
300088b6:	2d01      	cmp	r5, #1
300088b8:	ea4f 1c5c 	mov.w	ip, ip, lsr #5
300088bc:	ea4f 1656 	mov.w	r6, r6, lsr #5
300088c0:	ea4c 0706 	orr.w	r7, ip, r6
300088c4:	d903      	bls.n	300088ce <rxi316_dram_init+0x9be>
300088c6:	f1bc 0f00 	cmp.w	ip, #0
300088ca:	f43f ac4c 	beq.w	30008166 <rxi316_dram_init+0x256>
300088ce:	f040 0002 	orr.w	r0, r0, #2
300088d2:	e448      	b.n	30008166 <rxi316_dram_init+0x256>
300088d4:	4f1e      	ldr	r7, [pc, #120]	; (30008950 <rxi316_dram_init+0xa40>)
300088d6:	f04f 0e00 	mov.w	lr, #0
300088da:	fbb7 f8f3 	udiv	r8, r7, r3
300088de:	ea4f 2898 	mov.w	r8, r8, lsr #10
300088e2:	f8cd e004 	str.w	lr, [sp, #4]
300088e6:	f108 0701 	add.w	r7, r8, #1
300088ea:	f8df 8068 	ldr.w	r8, [pc, #104]	; 30008954 <rxi316_dram_init+0xa44>
300088ee:	ea08 3807 	and.w	r8, r8, r7, lsl #12
300088f2:	e5c6      	b.n	30008482 <rxi316_dram_init+0x572>
300088f4:	9909      	ldr	r1, [sp, #36]	; 0x24
300088f6:	f101 0802 	add.w	r8, r1, #2
300088fa:	990d      	ldr	r1, [sp, #52]	; 0x34
300088fc:	eba8 0801 	sub.w	r8, r8, r1
30008900:	9908      	ldr	r1, [sp, #32]
30008902:	4488      	add	r8, r1
30008904:	e55f      	b.n	300083c6 <rxi316_dram_init+0x4b6>
30008906:	2504      	movs	r5, #4
30008908:	e6db      	b.n	300086c2 <rxi316_dram_init+0x7b2>
3000890a:	686a      	ldr	r2, [r5, #4]
3000890c:	f042 0240 	orr.w	r2, r2, #64	; 0x40
30008910:	606a      	str	r2, [r5, #4]
30008912:	f7ff bba3 	b.w	3000805c <rxi316_dram_init+0x14c>
30008916:	f44f 5180 	mov.w	r1, #4096	; 0x1000
3000891a:	f44f 7600 	mov.w	r6, #512	; 0x200
3000891e:	e42d      	b.n	3000817c <rxi316_dram_init+0x26c>
30008920:	2702      	movs	r7, #2
30008922:	9709      	str	r7, [sp, #36]	; 0x24
30008924:	e595      	b.n	30008452 <rxi316_dram_init+0x542>
30008926:	f1a2 0109 	sub.w	r1, r2, #9
3000892a:	2905      	cmp	r1, #5
3000892c:	d80b      	bhi.n	30008946 <rxi316_dram_init+0xa36>
3000892e:	0856      	lsrs	r6, r2, #1
30008930:	f44f 5180 	mov.w	r1, #4096	; 0x1000
30008934:	0276      	lsls	r6, r6, #9
30008936:	e421      	b.n	3000817c <rxi316_dram_init+0x26c>
30008938:	9a09      	ldr	r2, [sp, #36]	; 0x24
3000893a:	f040 0004 	orr.w	r0, r0, #4
3000893e:	f1a2 070c 	sub.w	r7, r2, #12
30008942:	013f      	lsls	r7, r7, #4
30008944:	e78c      	b.n	30008860 <rxi316_dram_init+0x950>
30008946:	f44f 5180 	mov.w	r1, #4096	; 0x1000
3000894a:	2600      	movs	r6, #0
3000894c:	e416      	b.n	3000817c <rxi316_dram_init+0x26c>
3000894e:	bf00      	nop
30008950:	0bebc200 	.word	0x0bebc200
30008954:	003ff000 	.word	0x003ff000

30008958 <rxi316_ftend_init>:
30008958:	2300      	movs	r3, #0
3000895a:	b510      	push	{r4, lr}
3000895c:	4c0c      	ldr	r4, [pc, #48]	; (30008990 <rxi316_ftend_init+0x38>)
3000895e:	f8c4 3818 	str.w	r3, [r4, #2072]	; 0x818
30008962:	f8c4 3858 	str.w	r3, [r4, #2136]	; 0x858
30008966:	f8c4 3898 	str.w	r3, [r4, #2200]	; 0x898
3000896a:	f7fe fe49 	bl	30007600 <ddr_init_index>
3000896e:	4a09      	ldr	r2, [pc, #36]	; (30008994 <rxi316_ftend_init+0x3c>)
30008970:	f8d4 3104 	ldr.w	r3, [r4, #260]	; 0x104
30008974:	f852 1020 	ldr.w	r1, [r2, r0, lsl #2]
30008978:	f023 02ff 	bic.w	r2, r3, #255	; 0xff
3000897c:	68cb      	ldr	r3, [r1, #12]
3000897e:	689b      	ldr	r3, [r3, #8]
30008980:	6a5b      	ldr	r3, [r3, #36]	; 0x24
30008982:	3301      	adds	r3, #1
30008984:	b2db      	uxtb	r3, r3
30008986:	4313      	orrs	r3, r2
30008988:	f8c4 3104 	str.w	r3, [r4, #260]	; 0x104
3000898c:	bd10      	pop	{r4, pc}
3000898e:	bf00      	nop
30008990:	41101000 	.word	0x41101000
30008994:	3000ecf8 	.word	0x3000ecf8

30008998 <rxi316_DynSre_init>:
30008998:	b538      	push	{r3, r4, r5, lr}
3000899a:	4b20      	ldr	r3, [pc, #128]	; (30008a1c <rxi316_DynSre_init+0x84>)
3000899c:	4604      	mov	r4, r0
3000899e:	460d      	mov	r5, r1
300089a0:	4798      	blx	r3
300089a2:	2802      	cmp	r0, #2
300089a4:	d91c      	bls.n	300089e0 <rxi316_DynSre_init+0x48>
300089a6:	4b1e      	ldr	r3, [pc, #120]	; (30008a20 <rxi316_DynSre_init+0x88>)
300089a8:	691b      	ldr	r3, [r3, #16]
300089aa:	f3c3 230f 	ubfx	r3, r3, #8, #16
300089ae:	3b46      	subs	r3, #70	; 0x46
300089b0:	42a3      	cmp	r3, r4
300089b2:	d92d      	bls.n	30008a10 <rxi316_DynSre_init+0x78>
300089b4:	2d01      	cmp	r5, #1
300089b6:	d017      	beq.n	300089e8 <rxi316_DynSre_init+0x50>
300089b8:	4a19      	ldr	r2, [pc, #100]	; (30008a20 <rxi316_DynSre_init+0x88>)
300089ba:	6893      	ldr	r3, [r2, #8]
300089bc:	f023 0380 	bic.w	r3, r3, #128	; 0x80
300089c0:	6093      	str	r3, [r2, #8]
300089c2:	4b17      	ldr	r3, [pc, #92]	; (30008a20 <rxi316_DynSre_init+0x88>)
300089c4:	f44f 1200 	mov.w	r2, #2097152	; 0x200000
300089c8:	f04f 4100 	mov.w	r1, #2147483648	; 0x80000000
300089cc:	625a      	str	r2, [r3, #36]	; 0x24
300089ce:	629a      	str	r2, [r3, #40]	; 0x28
300089d0:	69da      	ldr	r2, [r3, #28]
300089d2:	f422 6270 	bic.w	r2, r2, #3840	; 0xf00
300089d6:	f442 7200 	orr.w	r2, r2, #512	; 0x200
300089da:	61da      	str	r2, [r3, #28]
300089dc:	6019      	str	r1, [r3, #0]
300089de:	bd38      	pop	{r3, r4, r5, pc}
300089e0:	2d01      	cmp	r5, #1
300089e2:	f04f 0400 	mov.w	r4, #0
300089e6:	d1e7      	bne.n	300089b8 <rxi316_DynSre_init+0x20>
300089e8:	4b0d      	ldr	r3, [pc, #52]	; (30008a20 <rxi316_DynSre_init+0x88>)
300089ea:	f64f 70fe 	movw	r0, #65534	; 0xfffe
300089ee:	4a0d      	ldr	r2, [pc, #52]	; (30008a24 <rxi316_DynSre_init+0x8c>)
300089f0:	f8d3 1100 	ldr.w	r1, [r3, #256]	; 0x100
300089f4:	4004      	ands	r4, r0
300089f6:	400a      	ands	r2, r1
300089f8:	f8c3 2100 	str.w	r2, [r3, #256]	; 0x100
300089fc:	f8d3 0100 	ldr.w	r0, [r3, #256]	; 0x100
30008a00:	4304      	orrs	r4, r0
30008a02:	f8c3 4100 	str.w	r4, [r3, #256]	; 0x100
30008a06:	689a      	ldr	r2, [r3, #8]
30008a08:	f042 0280 	orr.w	r2, r2, #128	; 0x80
30008a0c:	609a      	str	r2, [r3, #8]
30008a0e:	e7d8      	b.n	300089c2 <rxi316_DynSre_init+0x2a>
30008a10:	f240 410f 	movw	r1, #1039	; 0x40f
30008a14:	4804      	ldr	r0, [pc, #16]	; (30008a28 <rxi316_DynSre_init+0x90>)
30008a16:	f003 fa63 	bl	3000bee0 <io_assert_failed>
30008a1a:	bf00      	nop
30008a1c:	3000b535 	.word	0x3000b535
30008a20:	41100000 	.word	0x41100000
30008a24:	ffff0000 	.word	0xffff0000
30008a28:	3000d030 	.word	0x3000d030

30008a2c <ddr_init>:
30008a2c:	b500      	push	{lr}
30008a2e:	b083      	sub	sp, #12
30008a30:	f7fe fde6 	bl	30007600 <ddr_init_index>
30008a34:	4b06      	ldr	r3, [pc, #24]	; (30008a50 <ddr_init+0x24>)
30008a36:	f853 0020 	ldr.w	r0, [r3, r0, lsl #2]
30008a3a:	9001      	str	r0, [sp, #4]
30008a3c:	f7fe fe0e 	bl	3000765c <rxi316_perf_tune>
30008a40:	9801      	ldr	r0, [sp, #4]
30008a42:	f7ff fa65 	bl	30007f10 <rxi316_dram_init>
30008a46:	b003      	add	sp, #12
30008a48:	f85d eb04 	ldr.w	lr, [sp], #4
30008a4c:	f7ff bf84 	b.w	30008958 <rxi316_ftend_init>
30008a50:	3000ecf8 	.word	0x3000ecf8

30008a54 <DDR_PHY_ChipInfo.part.0>:
30008a54:	4b1a      	ldr	r3, [pc, #104]	; (30008ac0 <DDR_PHY_ChipInfo.part.0+0x6c>)
30008a56:	b570      	push	{r4, r5, r6, lr}
30008a58:	4798      	blx	r3
30008a5a:	3807      	subs	r0, #7
30008a5c:	280a      	cmp	r0, #10
30008a5e:	d816      	bhi.n	30008a8e <DDR_PHY_ChipInfo.part.0+0x3a>
30008a60:	e8df f000 	tbb	[pc, r0]
30008a64:	2924150b 	.word	0x2924150b
30008a68:	06101515 	.word	0x06101515
30008a6c:	0610      	.short	0x0610
30008a6e:	0b          	.byte	0x0b
30008a6f:	00          	.byte	0x00
30008a70:	2305      	movs	r3, #5
30008a72:	4a14      	ldr	r2, [pc, #80]	; (30008ac4 <DDR_PHY_ChipInfo.part.0+0x70>)
30008a74:	4618      	mov	r0, r3
30008a76:	7013      	strb	r3, [r2, #0]
30008a78:	bd70      	pop	{r4, r5, r6, pc}
30008a7a:	2303      	movs	r3, #3
30008a7c:	4a11      	ldr	r2, [pc, #68]	; (30008ac4 <DDR_PHY_ChipInfo.part.0+0x70>)
30008a7e:	4618      	mov	r0, r3
30008a80:	7013      	strb	r3, [r2, #0]
30008a82:	bd70      	pop	{r4, r5, r6, pc}
30008a84:	2304      	movs	r3, #4
30008a86:	4a0f      	ldr	r2, [pc, #60]	; (30008ac4 <DDR_PHY_ChipInfo.part.0+0x70>)
30008a88:	4618      	mov	r0, r3
30008a8a:	7013      	strb	r3, [r2, #0]
30008a8c:	bd70      	pop	{r4, r5, r6, pc}
30008a8e:	2414      	movs	r4, #20
30008a90:	4e0d      	ldr	r6, [pc, #52]	; (30008ac8 <DDR_PHY_ChipInfo.part.0+0x74>)
30008a92:	4d0e      	ldr	r5, [pc, #56]	; (30008acc <DDR_PHY_ChipInfo.part.0+0x78>)
30008a94:	4633      	mov	r3, r6
30008a96:	2245      	movs	r2, #69	; 0x45
30008a98:	4629      	mov	r1, r5
30008a9a:	2002      	movs	r0, #2
30008a9c:	f001 fc3c 	bl	3000a318 <rtk_log_write>
30008aa0:	3c01      	subs	r4, #1
30008aa2:	d1f7      	bne.n	30008a94 <DDR_PHY_ChipInfo.part.0+0x40>
30008aa4:	4b07      	ldr	r3, [pc, #28]	; (30008ac4 <DDR_PHY_ChipInfo.part.0+0x70>)
30008aa6:	4620      	mov	r0, r4
30008aa8:	701c      	strb	r4, [r3, #0]
30008aaa:	bd70      	pop	{r4, r5, r6, pc}
30008aac:	2301      	movs	r3, #1
30008aae:	4a05      	ldr	r2, [pc, #20]	; (30008ac4 <DDR_PHY_ChipInfo.part.0+0x70>)
30008ab0:	4618      	mov	r0, r3
30008ab2:	7013      	strb	r3, [r2, #0]
30008ab4:	bd70      	pop	{r4, r5, r6, pc}
30008ab6:	2302      	movs	r3, #2
30008ab8:	4a02      	ldr	r2, [pc, #8]	; (30008ac4 <DDR_PHY_ChipInfo.part.0+0x70>)
30008aba:	4618      	mov	r0, r3
30008abc:	7013      	strb	r3, [r2, #0]
30008abe:	bd70      	pop	{r4, r5, r6, pc}
30008ac0:	3000aeb9 	.word	0x3000aeb9
30008ac4:	3000ee90 	.word	0x3000ee90
30008ac8:	3000d0b0 	.word	0x3000d0b0
30008acc:	3000d0cc 	.word	0x3000d0cc

30008ad0 <DDR_PHY_ChipInfo_ddrtype>:
30008ad0:	b510      	push	{r4, lr}
30008ad2:	4c04      	ldr	r4, [pc, #16]	; (30008ae4 <DDR_PHY_ChipInfo_ddrtype+0x14>)
30008ad4:	7820      	ldrb	r0, [r4, #0]
30008ad6:	b100      	cbz	r0, 30008ada <DDR_PHY_ChipInfo_ddrtype+0xa>
30008ad8:	bd10      	pop	{r4, pc}
30008ada:	4b03      	ldr	r3, [pc, #12]	; (30008ae8 <DDR_PHY_ChipInfo_ddrtype+0x18>)
30008adc:	4798      	blx	r3
30008ade:	7020      	strb	r0, [r4, #0]
30008ae0:	bd10      	pop	{r4, pc}
30008ae2:	bf00      	nop
30008ae4:	3000f8f4 	.word	0x3000f8f4
30008ae8:	3000b011 	.word	0x3000b011

30008aec <DDR_PHY_StructInit>:
30008aec:	f240 2315 	movw	r3, #533	; 0x215
30008af0:	6583      	str	r3, [r0, #88]	; 0x58
30008af2:	4770      	bx	lr

30008af4 <DDR_PHY_DLL_CLK_DIV>:
30008af4:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
30008af6:	4d70      	ldr	r5, [pc, #448]	; (30008cb8 <DDR_PHY_DLL_CLK_DIV+0x1c4>)
30008af8:	4c70      	ldr	r4, [pc, #448]	; (30008cbc <DDR_PHY_DLL_CLK_DIV+0x1c8>)
30008afa:	fba5 3200 	umull	r3, r2, r5, r0
30008afe:	2365      	movs	r3, #101	; 0x65
30008b00:	eb00 0e04 	add.w	lr, r0, r4
30008b04:	0952      	lsrs	r2, r2, #5
30008b06:	fb00 f303 	mul.w	r3, r0, r3
30008b0a:	486d      	ldr	r0, [pc, #436]	; (30008cc0 <DDR_PHY_DLL_CLK_DIV+0x1cc>)
30008b0c:	f1a2 0c03 	sub.w	ip, r2, #3
30008b10:	6a47      	ldr	r7, [r0, #36]	; 0x24
30008b12:	eb0c 018c 	add.w	r1, ip, ip, lsl #2
30008b16:	ea4f 2c0c 	mov.w	ip, ip, lsl #8
30008b1a:	ebae 0ec1 	sub.w	lr, lr, r1, lsl #3
30008b1e:	fa1f fc8c 	uxth.w	ip, ip
30008b22:	ea4f 2ece 	mov.w	lr, lr, lsl #11
30008b26:	fba5 1e0e 	umull	r1, lr, r5, lr
30008b2a:	4966      	ldr	r1, [pc, #408]	; (30008cc4 <DDR_PHY_DLL_CLK_DIV+0x1d0>)
30008b2c:	fba1 6103 	umull	r6, r1, r1, r3
30008b30:	4e65      	ldr	r6, [pc, #404]	; (30008cc8 <DDR_PHY_DLL_CLK_DIV+0x1d4>)
30008b32:	0a09      	lsrs	r1, r1, #8
30008b34:	1a52      	subs	r2, r2, r1
30008b36:	3903      	subs	r1, #3
30008b38:	02d2      	lsls	r2, r2, #11
30008b3a:	eb02 125e 	add.w	r2, r2, lr, lsr #5
30008b3e:	ea06 2ece 	and.w	lr, r6, lr, lsl #11
30008b42:	4e62      	ldr	r6, [pc, #392]	; (30008ccc <DDR_PHY_DLL_CLK_DIV+0x1d8>)
30008b44:	403e      	ands	r6, r7
30008b46:	ea4e 0606 	orr.w	r6, lr, r6
30008b4a:	6246      	str	r6, [r0, #36]	; 0x24
30008b4c:	4e60      	ldr	r6, [pc, #384]	; (30008cd0 <DDR_PHY_DLL_CLK_DIV+0x1dc>)
30008b4e:	fba6 6303 	umull	r6, r3, r6, r3
30008b52:	eb04 1353 	add.w	r3, r4, r3, lsr #5
30008b56:	6a84      	ldr	r4, [r0, #40]	; 0x28
30008b58:	f424 447f 	bic.w	r4, r4, #65280	; 0xff00
30008b5c:	ea4c 0404 	orr.w	r4, ip, r4
30008b60:	6284      	str	r4, [r0, #40]	; 0x28
30008b62:	eb01 0481 	add.w	r4, r1, r1, lsl #2
30008b66:	b2c9      	uxtb	r1, r1
30008b68:	eba3 03c4 	sub.w	r3, r3, r4, lsl #3
30008b6c:	02db      	lsls	r3, r3, #11
30008b6e:	fba5 4303 	umull	r4, r3, r5, r3
30008b72:	6a45      	ldr	r5, [r0, #36]	; 0x24
30008b74:	4c57      	ldr	r4, [pc, #348]	; (30008cd4 <DDR_PHY_DLL_CLK_DIV+0x1e0>)
30008b76:	eba2 1253 	sub.w	r2, r2, r3, lsr #5
30008b7a:	f3c3 134a 	ubfx	r3, r3, #5, #11
30008b7e:	402c      	ands	r4, r5
30008b80:	4d55      	ldr	r5, [pc, #340]	; (30008cd8 <DDR_PHY_DLL_CLK_DIV+0x1e4>)
30008b82:	eb02 1242 	add.w	r2, r2, r2, lsl #5
30008b86:	4323      	orrs	r3, r4
30008b88:	02d2      	lsls	r2, r2, #11
30008b8a:	6243      	str	r3, [r0, #36]	; 0x24
30008b8c:	6a84      	ldr	r4, [r0, #40]	; 0x28
30008b8e:	fba5 3202 	umull	r3, r2, r5, r2
30008b92:	4b52      	ldr	r3, [pc, #328]	; (30008cdc <DDR_PHY_DLL_CLK_DIV+0x1e8>)
30008b94:	f024 04ff 	bic.w	r4, r4, #255	; 0xff
30008b98:	4321      	orrs	r1, r4
30008b9a:	2401      	movs	r4, #1
30008b9c:	6281      	str	r1, [r0, #40]	; 0x28
30008b9e:	6a01      	ldr	r1, [r0, #32]
30008ba0:	400b      	ands	r3, r1
30008ba2:	494f      	ldr	r1, [pc, #316]	; (30008ce0 <DDR_PHY_DLL_CLK_DIV+0x1ec>)
30008ba4:	ea43 3292 	orr.w	r2, r3, r2, lsr #14
30008ba8:	6202      	str	r2, [r0, #32]
30008baa:	6784      	str	r4, [r0, #120]	; 0x78
30008bac:	7808      	ldrb	r0, [r1, #0]
30008bae:	28ff      	cmp	r0, #255	; 0xff
30008bb0:	d075      	beq.n	30008c9e <DDR_PHY_DLL_CLK_DIV+0x1aa>
30008bb2:	4c4c      	ldr	r4, [pc, #304]	; (30008ce4 <DDR_PHY_DLL_CLK_DIV+0x1f0>)
30008bb4:	0145      	lsls	r5, r0, #5
30008bb6:	4b42      	ldr	r3, [pc, #264]	; (30008cc0 <DDR_PHY_DLL_CLK_DIV+0x1cc>)
30008bb8:	eb04 1040 	add.w	r0, r4, r0, lsl #5
30008bbc:	5962      	ldr	r2, [r4, r5]
30008bbe:	4e4a      	ldr	r6, [pc, #296]	; (30008ce8 <DDR_PHY_DLL_CLK_DIV+0x1f4>)
30008bc0:	611a      	str	r2, [r3, #16]
30008bc2:	6842      	ldr	r2, [r0, #4]
30008bc4:	615a      	str	r2, [r3, #20]
30008bc6:	6882      	ldr	r2, [r0, #8]
30008bc8:	f8c3 2098 	str.w	r2, [r3, #152]	; 0x98
30008bcc:	e9d0 1203 	ldrd	r1, r2, [r0, #12]
30008bd0:	f8c3 10a0 	str.w	r1, [r3, #160]	; 0xa0
30008bd4:	609a      	str	r2, [r3, #8]
30008bd6:	f8d3 209c 	ldr.w	r2, [r3, #156]	; 0x9c
30008bda:	f022 52f8 	bic.w	r2, r2, #520093696	; 0x1f000000
30008bde:	f8c3 209c 	str.w	r2, [r3, #156]	; 0x9c
30008be2:	7830      	ldrb	r0, [r6, #0]
30008be4:	2800      	cmp	r0, #0
30008be6:	d056      	beq.n	30008c96 <DDR_PHY_DLL_CLK_DIV+0x1a2>
30008be8:	2801      	cmp	r0, #1
30008bea:	442c      	add	r4, r5
30008bec:	4b34      	ldr	r3, [pc, #208]	; (30008cc0 <DDR_PHY_DLL_CLK_DIV+0x1cc>)
30008bee:	d037      	beq.n	30008c60 <DDR_PHY_DLL_CLK_DIV+0x16c>
30008bf0:	2222      	movs	r2, #34	; 0x22
30008bf2:	f8c3 2088 	str.w	r2, [r3, #136]	; 0x88
30008bf6:	6962      	ldr	r2, [r4, #20]
30008bf8:	f8c3 2080 	str.w	r2, [r3, #128]	; 0x80
30008bfc:	e9d4 1206 	ldrd	r1, r2, [r4, #24]
30008c00:	f8c3 1084 	str.w	r1, [r3, #132]	; 0x84
30008c04:	f8c3 2094 	str.w	r2, [r3, #148]	; 0x94
30008c08:	f8d3 2084 	ldr.w	r2, [r3, #132]	; 0x84
30008c0c:	4937      	ldr	r1, [pc, #220]	; (30008cec <DDR_PHY_DLL_CLK_DIV+0x1f8>)
30008c0e:	f422 427f 	bic.w	r2, r2, #65280	; 0xff00
30008c12:	f8c3 2084 	str.w	r2, [r3, #132]	; 0x84
30008c16:	f8d1 3268 	ldr.w	r3, [r1, #616]	; 0x268
30008c1a:	009b      	lsls	r3, r3, #2
30008c1c:	d516      	bpl.n	30008c4c <DDR_PHY_DLL_CLK_DIV+0x158>
30008c1e:	2800      	cmp	r0, #0
30008c20:	d044      	beq.n	30008cac <DDR_PHY_DLL_CLK_DIV+0x1b8>
30008c22:	4b27      	ldr	r3, [pc, #156]	; (30008cc0 <DDR_PHY_DLL_CLK_DIV+0x1cc>)
30008c24:	4a32      	ldr	r2, [pc, #200]	; (30008cf0 <DDR_PHY_DLL_CLK_DIV+0x1fc>)
30008c26:	601a      	str	r2, [r3, #0]
30008c28:	4b25      	ldr	r3, [pc, #148]	; (30008cc0 <DDR_PHY_DLL_CLK_DIV+0x1cc>)
30008c2a:	f8d3 2084 	ldr.w	r2, [r3, #132]	; 0x84
30008c2e:	f422 2270 	bic.w	r2, r2, #983040	; 0xf0000
30008c32:	f442 2240 	orr.w	r2, r2, #786432	; 0xc0000
30008c36:	f8c3 2084 	str.w	r2, [r3, #132]	; 0x84
30008c3a:	699a      	ldr	r2, [r3, #24]
30008c3c:	f422 1260 	bic.w	r2, r2, #3670016	; 0x380000
30008c40:	619a      	str	r2, [r3, #24]
30008c42:	699a      	ldr	r2, [r3, #24]
30008c44:	f042 5280 	orr.w	r2, r2, #268435456	; 0x10000000
30008c48:	619a      	str	r2, [r3, #24]
30008c4a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
30008c4c:	b920      	cbnz	r0, 30008c58 <DDR_PHY_DLL_CLK_DIV+0x164>
30008c4e:	4b29      	ldr	r3, [pc, #164]	; (30008cf4 <DDR_PHY_DLL_CLK_DIV+0x200>)
30008c50:	4798      	blx	r3
30008c52:	2801      	cmp	r0, #1
30008c54:	7030      	strb	r0, [r6, #0]
30008c56:	d025      	beq.n	30008ca4 <DDR_PHY_DLL_CLK_DIV+0x1b0>
30008c58:	4b19      	ldr	r3, [pc, #100]	; (30008cc0 <DDR_PHY_DLL_CLK_DIV+0x1cc>)
30008c5a:	4a27      	ldr	r2, [pc, #156]	; (30008cf8 <DDR_PHY_DLL_CLK_DIV+0x204>)
30008c5c:	601a      	str	r2, [r3, #0]
30008c5e:	e7e3      	b.n	30008c28 <DDR_PHY_DLL_CLK_DIV+0x134>
30008c60:	2233      	movs	r2, #51	; 0x33
30008c62:	6965      	ldr	r5, [r4, #20]
30008c64:	4921      	ldr	r1, [pc, #132]	; (30008cec <DDR_PHY_DLL_CLK_DIV+0x1f8>)
30008c66:	f8c3 2088 	str.w	r2, [r3, #136]	; 0x88
30008c6a:	f8c3 5080 	str.w	r5, [r3, #128]	; 0x80
30008c6e:	e9d4 0206 	ldrd	r0, r2, [r4, #24]
30008c72:	f8c3 0084 	str.w	r0, [r3, #132]	; 0x84
30008c76:	f8c3 2094 	str.w	r2, [r3, #148]	; 0x94
30008c7a:	f8d3 2084 	ldr.w	r2, [r3, #132]	; 0x84
30008c7e:	f422 427f 	bic.w	r2, r2, #65280	; 0xff00
30008c82:	f8c3 2084 	str.w	r2, [r3, #132]	; 0x84
30008c86:	f8d1 3268 	ldr.w	r3, [r1, #616]	; 0x268
30008c8a:	009a      	lsls	r2, r3, #2
30008c8c:	d50a      	bpl.n	30008ca4 <DDR_PHY_DLL_CLK_DIV+0x1b0>
30008c8e:	4b0c      	ldr	r3, [pc, #48]	; (30008cc0 <DDR_PHY_DLL_CLK_DIV+0x1cc>)
30008c90:	4a1a      	ldr	r2, [pc, #104]	; (30008cfc <DDR_PHY_DLL_CLK_DIV+0x208>)
30008c92:	601a      	str	r2, [r3, #0]
30008c94:	e7c8      	b.n	30008c28 <DDR_PHY_DLL_CLK_DIV+0x134>
30008c96:	4b17      	ldr	r3, [pc, #92]	; (30008cf4 <DDR_PHY_DLL_CLK_DIV+0x200>)
30008c98:	4798      	blx	r3
30008c9a:	7030      	strb	r0, [r6, #0]
30008c9c:	e7a4      	b.n	30008be8 <DDR_PHY_DLL_CLK_DIV+0xf4>
30008c9e:	f7ff fed9 	bl	30008a54 <DDR_PHY_ChipInfo.part.0>
30008ca2:	e786      	b.n	30008bb2 <DDR_PHY_DLL_CLK_DIV+0xbe>
30008ca4:	4b06      	ldr	r3, [pc, #24]	; (30008cc0 <DDR_PHY_DLL_CLK_DIV+0x1cc>)
30008ca6:	4a16      	ldr	r2, [pc, #88]	; (30008d00 <DDR_PHY_DLL_CLK_DIV+0x20c>)
30008ca8:	601a      	str	r2, [r3, #0]
30008caa:	e7bd      	b.n	30008c28 <DDR_PHY_DLL_CLK_DIV+0x134>
30008cac:	4b11      	ldr	r3, [pc, #68]	; (30008cf4 <DDR_PHY_DLL_CLK_DIV+0x200>)
30008cae:	4798      	blx	r3
30008cb0:	2801      	cmp	r0, #1
30008cb2:	7030      	strb	r0, [r6, #0]
30008cb4:	d1b5      	bne.n	30008c22 <DDR_PHY_DLL_CLK_DIV+0x12e>
30008cb6:	e7ea      	b.n	30008c8e <DDR_PHY_DLL_CLK_DIV+0x19a>
30008cb8:	cccccccd 	.word	0xcccccccd
30008cbc:	001fff88 	.word	0x001fff88
30008cc0:	41011000 	.word	0x41011000
30008cc4:	10624dd3 	.word	0x10624dd3
30008cc8:	07ff0000 	.word	0x07ff0000
30008ccc:	f800ffff 	.word	0xf800ffff
30008cd0:	51eb851f 	.word	0x51eb851f
30008cd4:	fffff800 	.word	0xfffff800
30008cd8:	d1b71759 	.word	0xd1b71759
30008cdc:	fff80000 	.word	0xfff80000
30008ce0:	3000ee90 	.word	0x3000ee90
30008ce4:	3000d0d4 	.word	0x3000d0d4
30008ce8:	3000f8f4 	.word	0x3000f8f4
30008cec:	42008000 	.word	0x42008000
30008cf0:	4c000821 	.word	0x4c000821
30008cf4:	3000b011 	.word	0x3000b011
30008cf8:	40000021 	.word	0x40000021
30008cfc:	4c000903 	.word	0x4c000903
30008d00:	40000103 	.word	0x40000103

30008d04 <DDR_PHY_CRT_Init>:
30008d04:	4a32      	ldr	r2, [pc, #200]	; (30008dd0 <DDR_PHY_CRT_Init+0xcc>)
30008d06:	2005      	movs	r0, #5
30008d08:	b538      	push	{r3, r4, r5, lr}
30008d0a:	f8d2 325c 	ldr.w	r3, [r2, #604]	; 0x25c
30008d0e:	4c31      	ldr	r4, [pc, #196]	; (30008dd4 <DDR_PHY_CRT_Init+0xd0>)
30008d10:	f443 7380 	orr.w	r3, r3, #256	; 0x100
30008d14:	4d30      	ldr	r5, [pc, #192]	; (30008dd8 <DDR_PHY_CRT_Init+0xd4>)
30008d16:	f8c2 325c 	str.w	r3, [r2, #604]	; 0x25c
30008d1a:	f8d4 308c 	ldr.w	r3, [r4, #140]	; 0x8c
30008d1e:	f043 0301 	orr.w	r3, r3, #1
30008d22:	f8c4 308c 	str.w	r3, [r4, #140]	; 0x8c
30008d26:	f8d4 308c 	ldr.w	r3, [r4, #140]	; 0x8c
30008d2a:	f043 0330 	orr.w	r3, r3, #48	; 0x30
30008d2e:	f8c4 308c 	str.w	r3, [r4, #140]	; 0x8c
30008d32:	68e3      	ldr	r3, [r4, #12]
30008d34:	f023 03ff 	bic.w	r3, r3, #255	; 0xff
30008d38:	60e3      	str	r3, [r4, #12]
30008d3a:	f8d4 308c 	ldr.w	r3, [r4, #140]	; 0x8c
30008d3e:	f043 0302 	orr.w	r3, r3, #2
30008d42:	f8c4 308c 	str.w	r3, [r4, #140]	; 0x8c
30008d46:	f8d4 308c 	ldr.w	r3, [r4, #140]	; 0x8c
30008d4a:	f023 0308 	bic.w	r3, r3, #8
30008d4e:	f8c4 308c 	str.w	r3, [r4, #140]	; 0x8c
30008d52:	47a8      	blx	r5
30008d54:	f8d4 308c 	ldr.w	r3, [r4, #140]	; 0x8c
30008d58:	f44f 70fa 	mov.w	r0, #500	; 0x1f4
30008d5c:	f043 0304 	orr.w	r3, r3, #4
30008d60:	f8c4 308c 	str.w	r3, [r4, #140]	; 0x8c
30008d64:	f8d4 308c 	ldr.w	r3, [r4, #140]	; 0x8c
30008d68:	f043 0308 	orr.w	r3, r3, #8
30008d6c:	f8c4 308c 	str.w	r3, [r4, #140]	; 0x8c
30008d70:	6aa3      	ldr	r3, [r4, #40]	; 0x28
30008d72:	f043 6380 	orr.w	r3, r3, #67108864	; 0x4000000
30008d76:	62a3      	str	r3, [r4, #40]	; 0x28
30008d78:	47a8      	blx	r5
30008d7a:	6aa2      	ldr	r2, [r4, #40]	; 0x28
30008d7c:	f641 73ff 	movw	r3, #8191	; 0x1fff
30008d80:	4916      	ldr	r1, [pc, #88]	; (30008ddc <DDR_PHY_CRT_Init+0xd8>)
30008d82:	f022 6280 	bic.w	r2, r2, #67108864	; 0x4000000
30008d86:	62a2      	str	r2, [r4, #40]	; 0x28
30008d88:	69e2      	ldr	r2, [r4, #28]
30008d8a:	f042 0202 	orr.w	r2, r2, #2
30008d8e:	61e2      	str	r2, [r4, #28]
30008d90:	69e2      	ldr	r2, [r4, #28]
30008d92:	f042 0208 	orr.w	r2, r2, #8
30008d96:	61e2      	str	r2, [r4, #28]
30008d98:	68a2      	ldr	r2, [r4, #8]
30008d9a:	f042 4200 	orr.w	r2, r2, #2147483648	; 0x80000000
30008d9e:	60a2      	str	r2, [r4, #8]
30008da0:	6862      	ldr	r2, [r4, #4]
30008da2:	4313      	orrs	r3, r2
30008da4:	6063      	str	r3, [r4, #4]
30008da6:	68a3      	ldr	r3, [r4, #8]
30008da8:	f443 23e0 	orr.w	r3, r3, #458752	; 0x70000
30008dac:	60a3      	str	r3, [r4, #8]
30008dae:	f8c4 1240 	str.w	r1, [r4, #576]	; 0x240
30008db2:	f8d4 3238 	ldr.w	r3, [r4, #568]	; 0x238
30008db6:	f023 0303 	bic.w	r3, r3, #3
30008dba:	f043 0302 	orr.w	r3, r3, #2
30008dbe:	f8c4 3238 	str.w	r3, [r4, #568]	; 0x238
30008dc2:	f8d4 3238 	ldr.w	r3, [r4, #568]	; 0x238
30008dc6:	f443 23c0 	orr.w	r3, r3, #393216	; 0x60000
30008dca:	f8c4 3238 	str.w	r3, [r4, #568]	; 0x238
30008dce:	bd38      	pop	{r3, r4, r5, pc}
30008dd0:	42008000 	.word	0x42008000
30008dd4:	41011000 	.word	0x41011000
30008dd8:	00009b2d 	.word	0x00009b2d
30008ddc:	90161f00 	.word	0x90161f00

30008de0 <DDR_PHY_R240_ZQ_CAL>:
30008de0:	b538      	push	{r3, r4, r5, lr}
30008de2:	4b1e      	ldr	r3, [pc, #120]	; (30008e5c <DDR_PHY_R240_ZQ_CAL+0x7c>)
30008de4:	7818      	ldrb	r0, [r3, #0]
30008de6:	28ff      	cmp	r0, #255	; 0xff
30008de8:	d034      	beq.n	30008e54 <DDR_PHY_R240_ZQ_CAL+0x74>
30008dea:	4a1d      	ldr	r2, [pc, #116]	; (30008e60 <DDR_PHY_R240_ZQ_CAL+0x80>)
30008dec:	eb00 0080 	add.w	r0, r0, r0, lsl #2
30008df0:	4b1c      	ldr	r3, [pc, #112]	; (30008e64 <DDR_PHY_R240_ZQ_CAL+0x84>)
30008df2:	2400      	movs	r4, #0
30008df4:	eb02 0180 	add.w	r1, r2, r0, lsl #2
30008df8:	f852 5020 	ldr.w	r5, [r2, r0, lsl #2]
30008dfc:	f8d3 014c 	ldr.w	r0, [r3, #332]	; 0x14c
30008e00:	7c0a      	ldrb	r2, [r1, #16]
30008e02:	f020 003e 	bic.w	r0, r0, #62	; 0x3e
30008e06:	0052      	lsls	r2, r2, #1
30008e08:	f002 023e 	and.w	r2, r2, #62	; 0x3e
30008e0c:	4302      	orrs	r2, r0
30008e0e:	6848      	ldr	r0, [r1, #4]
30008e10:	f8c3 214c 	str.w	r2, [r3, #332]	; 0x14c
30008e14:	f8c3 5180 	str.w	r5, [r3, #384]	; 0x180
30008e18:	4a13      	ldr	r2, [pc, #76]	; (30008e68 <DDR_PHY_R240_ZQ_CAL+0x88>)
30008e1a:	f8c3 4184 	str.w	r4, [r3, #388]	; 0x184
30008e1e:	f8c3 0188 	str.w	r0, [r3, #392]	; 0x188
30008e22:	f8c3 418c 	str.w	r4, [r3, #396]	; 0x18c
30008e26:	e9d1 0102 	ldrd	r0, r1, [r1, #8]
30008e2a:	f8c3 0190 	str.w	r0, [r3, #400]	; 0x190
30008e2e:	f8c3 2194 	str.w	r2, [r3, #404]	; 0x194
30008e32:	f8c3 1198 	str.w	r1, [r3, #408]	; 0x198
30008e36:	f8c3 219c 	str.w	r2, [r3, #412]	; 0x19c
30008e3a:	f8d3 2130 	ldr.w	r2, [r3, #304]	; 0x130
30008e3e:	f042 6200 	orr.w	r2, r2, #134217728	; 0x8000000
30008e42:	f8c3 2130 	str.w	r2, [r3, #304]	; 0x130
30008e46:	f8d3 2130 	ldr.w	r2, [r3, #304]	; 0x130
30008e4a:	f022 6200 	bic.w	r2, r2, #134217728	; 0x8000000
30008e4e:	f8c3 2130 	str.w	r2, [r3, #304]	; 0x130
30008e52:	bd38      	pop	{r3, r4, r5, pc}
30008e54:	f7ff fdfe 	bl	30008a54 <DDR_PHY_ChipInfo.part.0>
30008e58:	e7c7      	b.n	30008dea <DDR_PHY_R240_ZQ_CAL+0xa>
30008e5a:	bf00      	nop
30008e5c:	3000ee90 	.word	0x3000ee90
30008e60:	3000d194 	.word	0x3000d194
30008e64:	41011000 	.word	0x41011000
30008e68:	00262626 	.word	0x00262626

30008e6c <DDR_PHY_ZQ_SET_SEL>:
30008e6c:	b410      	push	{r4}
30008e6e:	4b25      	ldr	r3, [pc, #148]	; (30008f04 <DDR_PHY_ZQ_SET_SEL+0x98>)
30008e70:	f04f 3011 	mov.w	r0, #286331153	; 0x11111111
30008e74:	4a24      	ldr	r2, [pc, #144]	; (30008f08 <DDR_PHY_ZQ_SET_SEL+0x9c>)
30008e76:	4c25      	ldr	r4, [pc, #148]	; (30008f0c <DDR_PHY_ZQ_SET_SEL+0xa0>)
30008e78:	4925      	ldr	r1, [pc, #148]	; (30008f10 <DDR_PHY_ZQ_SET_SEL+0xa4>)
30008e7a:	f8c3 21c0 	str.w	r2, [r3, #448]	; 0x1c0
30008e7e:	f8c3 21c4 	str.w	r2, [r3, #452]	; 0x1c4
30008e82:	f8c3 21c8 	str.w	r2, [r3, #456]	; 0x1c8
30008e86:	f8c3 21cc 	str.w	r2, [r3, #460]	; 0x1cc
30008e8a:	f8c3 41d0 	str.w	r4, [r3, #464]	; 0x1d0
30008e8e:	f8c3 41d4 	str.w	r4, [r3, #468]	; 0x1d4
30008e92:	f64f 74ff 	movw	r4, #65535	; 0xffff
30008e96:	f8c3 21d8 	str.w	r2, [r3, #472]	; 0x1d8
30008e9a:	f8c3 21dc 	str.w	r2, [r3, #476]	; 0x1dc
30008e9e:	2200      	movs	r2, #0
30008ea0:	f8c3 01e0 	str.w	r0, [r3, #480]	; 0x1e0
30008ea4:	f8c3 01e4 	str.w	r0, [r3, #484]	; 0x1e4
30008ea8:	f8c3 11e8 	str.w	r1, [r3, #488]	; 0x1e8
30008eac:	f8c3 11ec 	str.w	r1, [r3, #492]	; 0x1ec
30008eb0:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
30008eb4:	f8c3 21f0 	str.w	r2, [r3, #496]	; 0x1f0
30008eb8:	f8c3 2204 	str.w	r2, [r3, #516]	; 0x204
30008ebc:	f8c3 2208 	str.w	r2, [r3, #520]	; 0x208
30008ec0:	f8c3 220c 	str.w	r2, [r3, #524]	; 0x20c
30008ec4:	f8c3 2210 	str.w	r2, [r3, #528]	; 0x210
30008ec8:	f8c3 2214 	str.w	r2, [r3, #532]	; 0x214
30008ecc:	f8c3 2218 	str.w	r2, [r3, #536]	; 0x218
30008ed0:	f8c3 221c 	str.w	r2, [r3, #540]	; 0x21c
30008ed4:	f8c3 2220 	str.w	r2, [r3, #544]	; 0x220
30008ed8:	f8c3 4224 	str.w	r4, [r3, #548]	; 0x224
30008edc:	f8c3 1228 	str.w	r1, [r3, #552]	; 0x228
30008ee0:	f8c3 122c 	str.w	r1, [r3, #556]	; 0x22c
30008ee4:	f8c3 1230 	str.w	r1, [r3, #560]	; 0x230
30008ee8:	f8c3 1234 	str.w	r1, [r3, #564]	; 0x234
30008eec:	f8c3 1328 	str.w	r1, [r3, #808]	; 0x328
30008ef0:	f8c3 132c 	str.w	r1, [r3, #812]	; 0x32c
30008ef4:	f8c3 1330 	str.w	r1, [r3, #816]	; 0x330
30008ef8:	f8c3 1334 	str.w	r1, [r3, #820]	; 0x334
30008efc:	f85d 4b04 	ldr.w	r4, [sp], #4
30008f00:	4770      	bx	lr
30008f02:	bf00      	nop
30008f04:	41011000 	.word	0x41011000
30008f08:	ffff1111 	.word	0xffff1111
30008f0c:	ffff3333 	.word	0xffff3333
30008f10:	ffff0000 	.word	0xffff0000

30008f14 <DDR_PHY_DELAY_TAP_SET>:
30008f14:	b510      	push	{r4, lr}
30008f16:	4b15      	ldr	r3, [pc, #84]	; (30008f6c <DDR_PHY_DELAY_TAP_SET+0x58>)
30008f18:	f04f 3288 	mov.w	r2, #2290649224	; 0x88888888
30008f1c:	4914      	ldr	r1, [pc, #80]	; (30008f70 <DDR_PHY_DELAY_TAP_SET+0x5c>)
30008f1e:	4c15      	ldr	r4, [pc, #84]	; (30008f74 <DDR_PHY_DELAY_TAP_SET+0x60>)
30008f20:	f8c3 2100 	str.w	r2, [r3, #256]	; 0x100
30008f24:	f8c3 2104 	str.w	r2, [r3, #260]	; 0x104
30008f28:	f8c3 2108 	str.w	r2, [r3, #264]	; 0x108
30008f2c:	f8c3 110c 	str.w	r1, [r3, #268]	; 0x10c
30008f30:	f8c3 2110 	str.w	r2, [r3, #272]	; 0x110
30008f34:	7820      	ldrb	r0, [r4, #0]
30008f36:	b1a0      	cbz	r0, 30008f62 <DDR_PHY_DELAY_TAP_SET+0x4e>
30008f38:	2801      	cmp	r0, #1
30008f3a:	4b0c      	ldr	r3, [pc, #48]	; (30008f6c <DDR_PHY_DELAY_TAP_SET+0x58>)
30008f3c:	f04f 3188 	mov.w	r1, #2290649224	; 0x88888888
30008f40:	bf0c      	ite	eq
30008f42:	4a0d      	ldreq	r2, [pc, #52]	; (30008f78 <DDR_PHY_DELAY_TAP_SET+0x64>)
30008f44:	4a0d      	ldrne	r2, [pc, #52]	; (30008f7c <DDR_PHY_DELAY_TAP_SET+0x68>)
30008f46:	f8c3 2154 	str.w	r2, [r3, #340]	; 0x154
30008f4a:	f640 0288 	movw	r2, #2184	; 0x888
30008f4e:	4b07      	ldr	r3, [pc, #28]	; (30008f6c <DDR_PHY_DELAY_TAP_SET+0x58>)
30008f50:	f8c3 1480 	str.w	r1, [r3, #1152]	; 0x480
30008f54:	f8c3 1484 	str.w	r1, [r3, #1156]	; 0x484
30008f58:	f8c3 2488 	str.w	r2, [r3, #1160]	; 0x488
30008f5c:	f8c3 248c 	str.w	r2, [r3, #1164]	; 0x48c
30008f60:	bd10      	pop	{r4, pc}
30008f62:	4b07      	ldr	r3, [pc, #28]	; (30008f80 <DDR_PHY_DELAY_TAP_SET+0x6c>)
30008f64:	4798      	blx	r3
30008f66:	7020      	strb	r0, [r4, #0]
30008f68:	e7e6      	b.n	30008f38 <DDR_PHY_DELAY_TAP_SET+0x24>
30008f6a:	bf00      	nop
30008f6c:	41011000 	.word	0x41011000
30008f70:	20888888 	.word	0x20888888
30008f74:	3000f8f4 	.word	0x3000f8f4
30008f78:	7c033333 	.word	0x7c033333
30008f7c:	7c011111 	.word	0x7c011111
30008f80:	3000b011 	.word	0x3000b011

30008f84 <DDR_PHY_READ_CTRL>:
30008f84:	4b10      	ldr	r3, [pc, #64]	; (30008fc8 <DDR_PHY_READ_CTRL+0x44>)
30008f86:	b510      	push	{r4, lr}
30008f88:	7818      	ldrb	r0, [r3, #0]
30008f8a:	28ff      	cmp	r0, #255	; 0xff
30008f8c:	d019      	beq.n	30008fc2 <DDR_PHY_READ_CTRL+0x3e>
30008f8e:	4a0f      	ldr	r2, [pc, #60]	; (30008fcc <DDR_PHY_READ_CTRL+0x48>)
30008f90:	eb00 0080 	add.w	r0, r0, r0, lsl #2
30008f94:	4b0e      	ldr	r3, [pc, #56]	; (30008fd0 <DDR_PHY_READ_CTRL+0x4c>)
30008f96:	2108      	movs	r1, #8
30008f98:	eb02 0080 	add.w	r0, r2, r0, lsl #2
30008f9c:	22fe      	movs	r2, #254	; 0xfe
30008f9e:	7c44      	ldrb	r4, [r0, #17]
30008fa0:	f8c3 45f8 	str.w	r4, [r3, #1528]	; 0x5f8
30008fa4:	7c84      	ldrb	r4, [r0, #18]
30008fa6:	7cc0      	ldrb	r0, [r0, #19]
30008fa8:	f8c3 45fc 	str.w	r4, [r3, #1532]	; 0x5fc
30008fac:	f8c3 0170 	str.w	r0, [r3, #368]	; 0x170
30008fb0:	f8c3 1408 	str.w	r1, [r3, #1032]	; 0x408
30008fb4:	f8c3 140c 	str.w	r1, [r3, #1036]	; 0x40c
30008fb8:	f8c3 2418 	str.w	r2, [r3, #1048]	; 0x418
30008fbc:	f8c3 241c 	str.w	r2, [r3, #1052]	; 0x41c
30008fc0:	bd10      	pop	{r4, pc}
30008fc2:	f7ff fd47 	bl	30008a54 <DDR_PHY_ChipInfo.part.0>
30008fc6:	e7e2      	b.n	30008f8e <DDR_PHY_READ_CTRL+0xa>
30008fc8:	3000ee90 	.word	0x3000ee90
30008fcc:	3000d194 	.word	0x3000d194
30008fd0:	41011000 	.word	0x41011000

30008fd4 <DDR_PHY_READ_LEVELING>:
30008fd4:	b508      	push	{r3, lr}
30008fd6:	4b21      	ldr	r3, [pc, #132]	; (3000905c <DDR_PHY_READ_LEVELING+0x88>)
30008fd8:	7818      	ldrb	r0, [r3, #0]
30008fda:	28ff      	cmp	r0, #255	; 0xff
30008fdc:	d03b      	beq.n	30009056 <DDR_PHY_READ_LEVELING+0x82>
30008fde:	4b20      	ldr	r3, [pc, #128]	; (30009060 <DDR_PHY_READ_LEVELING+0x8c>)
30008fe0:	b1e8      	cbz	r0, 3000901e <DDR_PHY_READ_LEVELING+0x4a>
30008fe2:	f04f 3112 	mov.w	r1, #303174162	; 0x12121212
30008fe6:	f04f 3011 	mov.w	r0, #286331153	; 0x11111111
30008fea:	220f      	movs	r2, #15
30008fec:	f8c3 1440 	str.w	r1, [r3, #1088]	; 0x440
30008ff0:	f8c3 1448 	str.w	r1, [r3, #1096]	; 0x448
30008ff4:	f8c3 0444 	str.w	r0, [r3, #1092]	; 0x444
30008ff8:	f8c3 044c 	str.w	r0, [r3, #1100]	; 0x44c
30008ffc:	f8c3 0458 	str.w	r0, [r3, #1112]	; 0x458
30009000:	f8c3 0460 	str.w	r0, [r3, #1120]	; 0x460
30009004:	f8c3 145c 	str.w	r1, [r3, #1116]	; 0x45c
30009008:	f8c3 1464 	str.w	r1, [r3, #1124]	; 0x464
3000900c:	f8c3 2450 	str.w	r2, [r3, #1104]	; 0x450
30009010:	f8c3 2454 	str.w	r2, [r3, #1108]	; 0x454
30009014:	f8c3 2468 	str.w	r2, [r3, #1128]	; 0x468
30009018:	f8c3 246c 	str.w	r2, [r3, #1132]	; 0x46c
3000901c:	bd08      	pop	{r3, pc}
3000901e:	f04f 3210 	mov.w	r2, #269488144	; 0x10101010
30009022:	2110      	movs	r1, #16
30009024:	f8c3 2440 	str.w	r2, [r3, #1088]	; 0x440
30009028:	f8c3 2448 	str.w	r2, [r3, #1096]	; 0x448
3000902c:	f8c3 2458 	str.w	r2, [r3, #1112]	; 0x458
30009030:	f8c3 2460 	str.w	r2, [r3, #1120]	; 0x460
30009034:	f8c3 2444 	str.w	r2, [r3, #1092]	; 0x444
30009038:	f8c3 244c 	str.w	r2, [r3, #1100]	; 0x44c
3000903c:	f8c3 245c 	str.w	r2, [r3, #1116]	; 0x45c
30009040:	f8c3 2464 	str.w	r2, [r3, #1124]	; 0x464
30009044:	f8c3 1450 	str.w	r1, [r3, #1104]	; 0x450
30009048:	f8c3 1454 	str.w	r1, [r3, #1108]	; 0x454
3000904c:	f8c3 1468 	str.w	r1, [r3, #1128]	; 0x468
30009050:	f8c3 146c 	str.w	r1, [r3, #1132]	; 0x46c
30009054:	bd08      	pop	{r3, pc}
30009056:	f7ff fcfd 	bl	30008a54 <DDR_PHY_ChipInfo.part.0>
3000905a:	e7c0      	b.n	30008fde <DDR_PHY_READ_LEVELING+0xa>
3000905c:	3000ee90 	.word	0x3000ee90
30009060:	41011000 	.word	0x41011000

30009064 <DDR_PHY_Init>:
30009064:	b510      	push	{r4, lr}
30009066:	6d80      	ldr	r0, [r0, #88]	; 0x58
30009068:	4c2f      	ldr	r4, [pc, #188]	; (30009128 <DDR_PHY_Init+0xc4>)
3000906a:	f7ff fd43 	bl	30008af4 <DDR_PHY_DLL_CLK_DIV>
3000906e:	f7ff fe49 	bl	30008d04 <DDR_PHY_CRT_Init>
30009072:	f7ff feb5 	bl	30008de0 <DDR_PHY_R240_ZQ_CAL>
30009076:	f7ff fef9 	bl	30008e6c <DDR_PHY_ZQ_SET_SEL>
3000907a:	f04f 1211 	mov.w	r2, #1114129	; 0x110011
3000907e:	2300      	movs	r3, #0
30009080:	f44f 5180 	mov.w	r1, #4096	; 0x1000
30009084:	4829      	ldr	r0, [pc, #164]	; (3000912c <DDR_PHY_Init+0xc8>)
30009086:	f8c4 24f0 	str.w	r2, [r4, #1264]	; 0x4f0
3000908a:	f8c4 24f4 	str.w	r2, [r4, #1268]	; 0x4f4
3000908e:	f8c4 34e8 	str.w	r3, [r4, #1256]	; 0x4e8
30009092:	f8c4 34ec 	str.w	r3, [r4, #1260]	; 0x4ec
30009096:	f8c4 017c 	str.w	r0, [r4, #380]	; 0x17c
3000909a:	f8c4 12a4 	str.w	r1, [r4, #676]	; 0x2a4
3000909e:	f7ff ff39 	bl	30008f14 <DDR_PHY_DELAY_TAP_SET>
300090a2:	f7ff ff6f 	bl	30008f84 <DDR_PHY_READ_CTRL>
300090a6:	f04f 3322 	mov.w	r3, #572662306	; 0x22222222
300090aa:	4a21      	ldr	r2, [pc, #132]	; (30009130 <DDR_PHY_Init+0xcc>)
300090ac:	f8c4 2178 	str.w	r2, [r4, #376]	; 0x178
300090b0:	f8c4 3420 	str.w	r3, [r4, #1056]	; 0x420
300090b4:	f8c4 3424 	str.w	r3, [r4, #1060]	; 0x424
300090b8:	f8c4 3430 	str.w	r3, [r4, #1072]	; 0x430
300090bc:	f8c4 3434 	str.w	r3, [r4, #1076]	; 0x434
300090c0:	f7ff ff88 	bl	30008fd4 <DDR_PHY_READ_LEVELING>
300090c4:	f8d4 31f8 	ldr.w	r3, [r4, #504]	; 0x1f8
300090c8:	f04f 31d8 	mov.w	r1, #3638089944	; 0xd8d8d8d8
300090cc:	f04f 328c 	mov.w	r2, #2358021260	; 0x8c8c8c8c
300090d0:	f423 437e 	bic.w	r3, r3, #65024	; 0xfe00
300090d4:	f443 4364 	orr.w	r3, r3, #58368	; 0xe400
300090d8:	f8c4 31f8 	str.w	r3, [r4, #504]	; 0x1f8
300090dc:	f8c4 11fc 	str.w	r1, [r4, #508]	; 0x1fc
300090e0:	f8d4 32d0 	ldr.w	r3, [r4, #720]	; 0x2d0
300090e4:	f043 032f 	orr.w	r3, r3, #47	; 0x2f
300090e8:	f8c4 32d0 	str.w	r3, [r4, #720]	; 0x2d0
300090ec:	f8c4 21fc 	str.w	r2, [r4, #508]	; 0x1fc
300090f0:	f8d4 323c 	ldr.w	r3, [r4, #572]	; 0x23c
300090f4:	f043 0303 	orr.w	r3, r3, #3
300090f8:	f8c4 323c 	str.w	r3, [r4, #572]	; 0x23c
300090fc:	f8d4 323c 	ldr.w	r3, [r4, #572]	; 0x23c
30009100:	f043 030c 	orr.w	r3, r3, #12
30009104:	f8c4 323c 	str.w	r3, [r4, #572]	; 0x23c
30009108:	f8d4 3238 	ldr.w	r3, [r4, #568]	; 0x238
3000910c:	f023 0303 	bic.w	r3, r3, #3
30009110:	f8c4 3238 	str.w	r3, [r4, #568]	; 0x238
30009114:	f8d4 3238 	ldr.w	r3, [r4, #568]	; 0x238
30009118:	f423 537c 	bic.w	r3, r3, #16128	; 0x3f00
3000911c:	f443 5380 	orr.w	r3, r3, #4096	; 0x1000
30009120:	f8c4 3238 	str.w	r3, [r4, #568]	; 0x238
30009124:	bd10      	pop	{r4, pc}
30009126:	bf00      	nop
30009128:	41011000 	.word	0x41011000
3000912c:	00045500 	.word	0x00045500
30009130:	11117777 	.word	0x11117777

30009134 <DDR_PHY_CAL>:
30009134:	4770      	bx	lr
30009136:	bf00      	nop

30009138 <DDR_PHY_AutoGating>:
30009138:	4a0c      	ldr	r2, [pc, #48]	; (3000916c <DDR_PHY_AutoGating+0x34>)
3000913a:	2032      	movs	r0, #50	; 0x32
3000913c:	490c      	ldr	r1, [pc, #48]	; (30009170 <DDR_PHY_AutoGating+0x38>)
3000913e:	b508      	push	{r3, lr}
30009140:	6893      	ldr	r3, [r2, #8]
30009142:	f023 5300 	bic.w	r3, r3, #536870912	; 0x20000000
30009146:	6093      	str	r3, [r2, #8]
30009148:	f8d2 30ac 	ldr.w	r3, [r2, #172]	; 0xac
3000914c:	f023 033c 	bic.w	r3, r3, #60	; 0x3c
30009150:	f043 0314 	orr.w	r3, r3, #20
30009154:	f8c2 30ac 	str.w	r3, [r2, #172]	; 0xac
30009158:	4788      	blx	r1
3000915a:	f04f 4282 	mov.w	r2, #1090519040	; 0x41000000
3000915e:	f8d2 31e0 	ldr.w	r3, [r2, #480]	; 0x1e0
30009162:	f043 0303 	orr.w	r3, r3, #3
30009166:	f8c2 31e0 	str.w	r3, [r2, #480]	; 0x1e0
3000916a:	bd08      	pop	{r3, pc}
3000916c:	41011000 	.word	0x41011000
30009170:	00009b2d 	.word	0x00009b2d

30009174 <SDM32K_Enable>:
30009174:	4b04      	ldr	r3, [pc, #16]	; (30009188 <SDM32K_Enable+0x14>)
30009176:	f44f 11f0 	mov.w	r1, #1966080	; 0x1e0000
3000917a:	4a04      	ldr	r2, [pc, #16]	; (3000918c <SDM32K_Enable+0x18>)
3000917c:	61d9      	str	r1, [r3, #28]
3000917e:	6819      	ldr	r1, [r3, #0]
30009180:	430a      	orrs	r2, r1
30009182:	601a      	str	r2, [r3, #0]
30009184:	4770      	bx	lr
30009186:	bf00      	nop
30009188:	42008e00 	.word	0x42008e00
3000918c:	c0060000 	.word	0xc0060000

30009190 <CLK_SWITCH_XTAL>:
30009190:	b508      	push	{r3, lr}
30009192:	2801      	cmp	r0, #1
30009194:	4b0a      	ldr	r3, [pc, #40]	; (300091c0 <CLK_SWITCH_XTAL+0x30>)
30009196:	f8d3 2224 	ldr.w	r2, [r3, #548]	; 0x224
3000919a:	d00d      	beq.n	300091b8 <CLK_SWITCH_XTAL+0x28>
3000919c:	4b09      	ldr	r3, [pc, #36]	; (300091c4 <CLK_SWITCH_XTAL+0x34>)
3000919e:	4313      	orrs	r3, r2
300091a0:	4907      	ldr	r1, [pc, #28]	; (300091c0 <CLK_SWITCH_XTAL+0x30>)
300091a2:	4a09      	ldr	r2, [pc, #36]	; (300091c8 <CLK_SWITCH_XTAL+0x38>)
300091a4:	f8c1 3224 	str.w	r3, [r1, #548]	; 0x224
300091a8:	4790      	blx	r2
300091aa:	4a08      	ldr	r2, [pc, #32]	; (300091cc <CLK_SWITCH_XTAL+0x3c>)
300091ac:	4b08      	ldr	r3, [pc, #32]	; (300091d0 <CLK_SWITCH_XTAL+0x40>)
300091ae:	fba2 2000 	umull	r2, r0, r2, r0
300091b2:	0c80      	lsrs	r0, r0, #18
300091b4:	6018      	str	r0, [r3, #0]
300091b6:	bd08      	pop	{r3, pc}
300091b8:	4b06      	ldr	r3, [pc, #24]	; (300091d4 <CLK_SWITCH_XTAL+0x44>)
300091ba:	4013      	ands	r3, r2
300091bc:	e7f0      	b.n	300091a0 <CLK_SWITCH_XTAL+0x10>
300091be:	bf00      	nop
300091c0:	42008000 	.word	0x42008000
300091c4:	80000500 	.word	0x80000500
300091c8:	3000b23d 	.word	0x3000b23d
300091cc:	431bde83 	.word	0x431bde83
300091d0:	2001c700 	.word	0x2001c700
300091d4:	7ffffaff 	.word	0x7ffffaff

300091d8 <SWR_MEM>:
300091d8:	2801      	cmp	r0, #1
300091da:	d00e      	beq.n	300091fa <SWR_MEM+0x22>
300091dc:	4b31      	ldr	r3, [pc, #196]	; (300092a4 <SWR_MEM+0xcc>)
300091de:	2000      	movs	r0, #0
300091e0:	f8d3 20c0 	ldr.w	r2, [r3, #192]	; 0xc0
300091e4:	f022 0201 	bic.w	r2, r2, #1
300091e8:	f8c3 20c0 	str.w	r2, [r3, #192]	; 0xc0
300091ec:	f8d3 20c0 	ldr.w	r2, [r3, #192]	; 0xc0
300091f0:	f022 0202 	bic.w	r2, r2, #2
300091f4:	f8c3 20c0 	str.w	r2, [r3, #192]	; 0xc0
300091f8:	4770      	bx	lr
300091fa:	b510      	push	{r4, lr}
300091fc:	4c29      	ldr	r4, [pc, #164]	; (300092a4 <SWR_MEM+0xcc>)
300091fe:	4a2a      	ldr	r2, [pc, #168]	; (300092a8 <SWR_MEM+0xd0>)
30009200:	f8d4 30e4 	ldr.w	r3, [r4, #228]	; 0xe4
30009204:	f423 5340 	bic.w	r3, r3, #12288	; 0x3000
30009208:	f443 5380 	orr.w	r3, r3, #4096	; 0x1000
3000920c:	f8c4 30e4 	str.w	r3, [r4, #228]	; 0xe4
30009210:	f8d4 30d0 	ldr.w	r3, [r4, #208]	; 0xd0
30009214:	f023 0330 	bic.w	r3, r3, #48	; 0x30
30009218:	f8c4 30d0 	str.w	r3, [r4, #208]	; 0xd0
3000921c:	f8d4 30d8 	ldr.w	r3, [r4, #216]	; 0xd8
30009220:	f023 0303 	bic.w	r3, r3, #3
30009224:	f8c4 30d8 	str.w	r3, [r4, #216]	; 0xd8
30009228:	f8d4 30d0 	ldr.w	r3, [r4, #208]	; 0xd0
3000922c:	f023 7370 	bic.w	r3, r3, #62914560	; 0x3c00000
30009230:	f8c4 30d0 	str.w	r3, [r4, #208]	; 0xd0
30009234:	4790      	blx	r2
30009236:	2803      	cmp	r0, #3
30009238:	f8d4 30d8 	ldr.w	r3, [r4, #216]	; 0xd8
3000923c:	d026      	beq.n	3000928c <SWR_MEM+0xb4>
3000923e:	f043 0303 	orr.w	r3, r3, #3
30009242:	f8c4 30d8 	str.w	r3, [r4, #216]	; 0xd8
30009246:	f8d4 30d0 	ldr.w	r3, [r4, #208]	; 0xd0
3000924a:	f043 7340 	orr.w	r3, r3, #50331648	; 0x3000000
3000924e:	f8c4 30d0 	str.w	r3, [r4, #208]	; 0xd0
30009252:	4c14      	ldr	r4, [pc, #80]	; (300092a4 <SWR_MEM+0xcc>)
30009254:	4b15      	ldr	r3, [pc, #84]	; (300092ac <SWR_MEM+0xd4>)
30009256:	4798      	blx	r3
30009258:	f8d4 30c0 	ldr.w	r3, [r4, #192]	; 0xc0
3000925c:	4a14      	ldr	r2, [pc, #80]	; (300092b0 <SWR_MEM+0xd8>)
3000925e:	f44f 7096 	mov.w	r0, #300	; 0x12c
30009262:	f043 0302 	orr.w	r3, r3, #2
30009266:	f8c4 30c0 	str.w	r3, [r4, #192]	; 0xc0
3000926a:	4790      	blx	r2
3000926c:	f8d4 20c0 	ldr.w	r2, [r4, #192]	; 0xc0
30009270:	4b10      	ldr	r3, [pc, #64]	; (300092b4 <SWR_MEM+0xdc>)
30009272:	f042 0201 	orr.w	r2, r2, #1
30009276:	f8c4 20c0 	str.w	r2, [r4, #192]	; 0xc0
3000927a:	e001      	b.n	30009280 <SWR_MEM+0xa8>
3000927c:	3b01      	subs	r3, #1
3000927e:	d00e      	beq.n	3000929e <SWR_MEM+0xc6>
30009280:	f8d4 20c0 	ldr.w	r2, [r4, #192]	; 0xc0
30009284:	03d2      	lsls	r2, r2, #15
30009286:	d5f9      	bpl.n	3000927c <SWR_MEM+0xa4>
30009288:	2000      	movs	r0, #0
3000928a:	bd10      	pop	{r4, pc}
3000928c:	f8c4 30d8 	str.w	r3, [r4, #216]	; 0xd8
30009290:	f8d4 30d0 	ldr.w	r3, [r4, #208]	; 0xd0
30009294:	f443 0340 	orr.w	r3, r3, #12582912	; 0xc00000
30009298:	f8c4 30d0 	str.w	r3, [r4, #208]	; 0xd0
3000929c:	e7d9      	b.n	30009252 <SWR_MEM+0x7a>
3000929e:	2001      	movs	r0, #1
300092a0:	bd10      	pop	{r4, pc}
300092a2:	bf00      	nop
300092a4:	42008800 	.word	0x42008800
300092a8:	3000b011 	.word	0x3000b011
300092ac:	3000948d 	.word	0x3000948d
300092b0:	00009b2d 	.word	0x00009b2d
300092b4:	000186a0 	.word	0x000186a0

300092b8 <SWR_MEM_Manual>:
300092b8:	4a06      	ldr	r2, [pc, #24]	; (300092d4 <SWR_MEM_Manual+0x1c>)
300092ba:	f8d2 30c0 	ldr.w	r3, [r2, #192]	; 0xc0
300092be:	b120      	cbz	r0, 300092ca <SWR_MEM_Manual+0x12>
300092c0:	f443 2300 	orr.w	r3, r3, #524288	; 0x80000
300092c4:	f8c2 30c0 	str.w	r3, [r2, #192]	; 0xc0
300092c8:	4770      	bx	lr
300092ca:	f423 2300 	bic.w	r3, r3, #524288	; 0x80000
300092ce:	f8c2 30c0 	str.w	r3, [r2, #192]	; 0xc0
300092d2:	4770      	bx	lr
300092d4:	42008800 	.word	0x42008800

300092d8 <SWR_MEM_Mode_Set>:
300092d8:	2801      	cmp	r0, #1
300092da:	b538      	push	{r3, r4, r5, lr}
300092dc:	d83f      	bhi.n	3000935e <SWR_MEM_Mode_Set+0x86>
300092de:	d015      	beq.n	3000930c <SWR_MEM_Mode_Set+0x34>
300092e0:	4921      	ldr	r1, [pc, #132]	; (30009368 <SWR_MEM_Mode_Set+0x90>)
300092e2:	f8d1 30c0 	ldr.w	r3, [r1, #192]	; 0xc0
300092e6:	0358      	lsls	r0, r3, #13
300092e8:	d50e      	bpl.n	30009308 <SWR_MEM_Mode_Set+0x30>
300092ea:	f8d1 20d0 	ldr.w	r2, [r1, #208]	; 0xd0
300092ee:	f242 7310 	movw	r3, #10000	; 0x2710
300092f2:	f022 0240 	bic.w	r2, r2, #64	; 0x40
300092f6:	f8c1 20d0 	str.w	r2, [r1, #208]	; 0xd0
300092fa:	e001      	b.n	30009300 <SWR_MEM_Mode_Set+0x28>
300092fc:	3b01      	subs	r3, #1
300092fe:	d02c      	beq.n	3000935a <SWR_MEM_Mode_Set+0x82>
30009300:	f8d1 20c0 	ldr.w	r2, [r1, #192]	; 0xc0
30009304:	0352      	lsls	r2, r2, #13
30009306:	d4f9      	bmi.n	300092fc <SWR_MEM_Mode_Set+0x24>
30009308:	2000      	movs	r0, #0
3000930a:	bd38      	pop	{r3, r4, r5, pc}
3000930c:	4c16      	ldr	r4, [pc, #88]	; (30009368 <SWR_MEM_Mode_Set+0x90>)
3000930e:	f8d4 30c0 	ldr.w	r3, [r4, #192]	; 0xc0
30009312:	035b      	lsls	r3, r3, #13
30009314:	d4f8      	bmi.n	30009308 <SWR_MEM_Mode_Set+0x30>
30009316:	f8d4 30d4 	ldr.w	r3, [r4, #212]	; 0xd4
3000931a:	2064      	movs	r0, #100	; 0x64
3000931c:	4d13      	ldr	r5, [pc, #76]	; (3000936c <SWR_MEM_Mode_Set+0x94>)
3000931e:	f443 7380 	orr.w	r3, r3, #256	; 0x100
30009322:	f8c4 30d4 	str.w	r3, [r4, #212]	; 0xd4
30009326:	47a8      	blx	r5
30009328:	f8d4 30d0 	ldr.w	r3, [r4, #208]	; 0xd0
3000932c:	2064      	movs	r0, #100	; 0x64
3000932e:	f043 0340 	orr.w	r3, r3, #64	; 0x40
30009332:	f8c4 30d0 	str.w	r3, [r4, #208]	; 0xd0
30009336:	47a8      	blx	r5
30009338:	f8d4 20d4 	ldr.w	r2, [r4, #212]	; 0xd4
3000933c:	f242 7310 	movw	r3, #10000	; 0x2710
30009340:	f422 7280 	bic.w	r2, r2, #256	; 0x100
30009344:	f8c4 20d4 	str.w	r2, [r4, #212]	; 0xd4
30009348:	e001      	b.n	3000934e <SWR_MEM_Mode_Set+0x76>
3000934a:	3b01      	subs	r3, #1
3000934c:	d005      	beq.n	3000935a <SWR_MEM_Mode_Set+0x82>
3000934e:	f8d4 20c0 	ldr.w	r2, [r4, #192]	; 0xc0
30009352:	0355      	lsls	r5, r2, #13
30009354:	d5f9      	bpl.n	3000934a <SWR_MEM_Mode_Set+0x72>
30009356:	2000      	movs	r0, #0
30009358:	e7d7      	b.n	3000930a <SWR_MEM_Mode_Set+0x32>
3000935a:	2001      	movs	r0, #1
3000935c:	bd38      	pop	{r3, r4, r5, pc}
3000935e:	217a      	movs	r1, #122	; 0x7a
30009360:	4803      	ldr	r0, [pc, #12]	; (30009370 <SWR_MEM_Mode_Set+0x98>)
30009362:	f002 fdbd 	bl	3000bee0 <io_assert_failed>
30009366:	bf00      	nop
30009368:	42008800 	.word	0x42008800
3000936c:	00009b2d 	.word	0x00009b2d
30009370:	3000d20c 	.word	0x3000d20c

30009374 <SWR_AUDIO>:
30009374:	2801      	cmp	r0, #1
30009376:	d00e      	beq.n	30009396 <SWR_AUDIO+0x22>
30009378:	4b1e      	ldr	r3, [pc, #120]	; (300093f4 <SWR_AUDIO+0x80>)
3000937a:	2000      	movs	r0, #0
3000937c:	f8d3 2080 	ldr.w	r2, [r3, #128]	; 0x80
30009380:	f022 0201 	bic.w	r2, r2, #1
30009384:	f8c3 2080 	str.w	r2, [r3, #128]	; 0x80
30009388:	f8d3 2080 	ldr.w	r2, [r3, #128]	; 0x80
3000938c:	f022 0202 	bic.w	r2, r2, #2
30009390:	f8c3 2080 	str.w	r2, [r3, #128]	; 0x80
30009394:	4770      	bx	lr
30009396:	b510      	push	{r4, lr}
30009398:	4c16      	ldr	r4, [pc, #88]	; (300093f4 <SWR_AUDIO+0x80>)
3000939a:	4b17      	ldr	r3, [pc, #92]	; (300093f8 <SWR_AUDIO+0x84>)
3000939c:	4798      	blx	r3
3000939e:	f8d4 30a4 	ldr.w	r3, [r4, #164]	; 0xa4
300093a2:	4a16      	ldr	r2, [pc, #88]	; (300093fc <SWR_AUDIO+0x88>)
300093a4:	f44f 7096 	mov.w	r0, #300	; 0x12c
300093a8:	f423 5340 	bic.w	r3, r3, #12288	; 0x3000
300093ac:	f443 5380 	orr.w	r3, r3, #4096	; 0x1000
300093b0:	f8c4 30a4 	str.w	r3, [r4, #164]	; 0xa4
300093b4:	f8d4 3090 	ldr.w	r3, [r4, #144]	; 0x90
300093b8:	f023 0330 	bic.w	r3, r3, #48	; 0x30
300093bc:	f8c4 3090 	str.w	r3, [r4, #144]	; 0x90
300093c0:	f8d4 3080 	ldr.w	r3, [r4, #128]	; 0x80
300093c4:	f043 0302 	orr.w	r3, r3, #2
300093c8:	f8c4 3080 	str.w	r3, [r4, #128]	; 0x80
300093cc:	4790      	blx	r2
300093ce:	f8d4 2080 	ldr.w	r2, [r4, #128]	; 0x80
300093d2:	4b0b      	ldr	r3, [pc, #44]	; (30009400 <SWR_AUDIO+0x8c>)
300093d4:	f042 0201 	orr.w	r2, r2, #1
300093d8:	f8c4 2080 	str.w	r2, [r4, #128]	; 0x80
300093dc:	e001      	b.n	300093e2 <SWR_AUDIO+0x6e>
300093de:	3b01      	subs	r3, #1
300093e0:	d005      	beq.n	300093ee <SWR_AUDIO+0x7a>
300093e2:	f8d4 2080 	ldr.w	r2, [r4, #128]	; 0x80
300093e6:	03d2      	lsls	r2, r2, #15
300093e8:	d5f9      	bpl.n	300093de <SWR_AUDIO+0x6a>
300093ea:	2000      	movs	r0, #0
300093ec:	bd10      	pop	{r4, pc}
300093ee:	2001      	movs	r0, #1
300093f0:	bd10      	pop	{r4, pc}
300093f2:	bf00      	nop
300093f4:	42008800 	.word	0x42008800
300093f8:	300095e9 	.word	0x300095e9
300093fc:	00009b2d 	.word	0x00009b2d
30009400:	000186a0 	.word	0x000186a0

30009404 <SWR_AUDIO_Manual>:
30009404:	4a06      	ldr	r2, [pc, #24]	; (30009420 <SWR_AUDIO_Manual+0x1c>)
30009406:	f8d2 3080 	ldr.w	r3, [r2, #128]	; 0x80
3000940a:	b120      	cbz	r0, 30009416 <SWR_AUDIO_Manual+0x12>
3000940c:	f443 2300 	orr.w	r3, r3, #524288	; 0x80000
30009410:	f8c2 3080 	str.w	r3, [r2, #128]	; 0x80
30009414:	4770      	bx	lr
30009416:	f423 2300 	bic.w	r3, r3, #524288	; 0x80000
3000941a:	f8c2 3080 	str.w	r3, [r2, #128]	; 0x80
3000941e:	4770      	bx	lr
30009420:	42008800 	.word	0x42008800

30009424 <SWR_BST_MODE_Set>:
30009424:	4a06      	ldr	r2, [pc, #24]	; (30009440 <SWR_BST_MODE_Set+0x1c>)
30009426:	6813      	ldr	r3, [r2, #0]
30009428:	b118      	cbz	r0, 30009432 <SWR_BST_MODE_Set+0xe>
3000942a:	f443 1340 	orr.w	r3, r3, #3145728	; 0x300000
3000942e:	6013      	str	r3, [r2, #0]
30009430:	4770      	bx	lr
30009432:	f423 1340 	bic.w	r3, r3, #3145728	; 0x300000
30009436:	f443 1380 	orr.w	r3, r3, #1048576	; 0x100000
3000943a:	6013      	str	r3, [r2, #0]
3000943c:	4770      	bx	lr
3000943e:	bf00      	nop
30009440:	42008100 	.word	0x42008100

30009444 <SWR_Calib_DCore>:
30009444:	4b0f      	ldr	r3, [pc, #60]	; (30009484 <SWR_Calib_DCore+0x40>)
30009446:	f240 3231 	movw	r2, #817	; 0x331
3000944a:	490f      	ldr	r1, [pc, #60]	; (30009488 <SWR_Calib_DCore+0x44>)
3000944c:	6d18      	ldr	r0, [r3, #80]	; 0x50
3000944e:	4001      	ands	r1, r0
30009450:	430a      	orrs	r2, r1
30009452:	651a      	str	r2, [r3, #80]	; 0x50
30009454:	6c1a      	ldr	r2, [r3, #64]	; 0x40
30009456:	f442 22c0 	orr.w	r2, r2, #393216	; 0x60000
3000945a:	641a      	str	r2, [r3, #64]	; 0x40
3000945c:	f8d3 2084 	ldr.w	r2, [r3, #132]	; 0x84
30009460:	f042 0201 	orr.w	r2, r2, #1
30009464:	f8c3 2084 	str.w	r2, [r3, #132]	; 0x84
30009468:	f8d3 2090 	ldr.w	r2, [r3, #144]	; 0x90
3000946c:	f442 5280 	orr.w	r2, r2, #4096	; 0x1000
30009470:	f8c3 2090 	str.w	r2, [r3, #144]	; 0x90
30009474:	685a      	ldr	r2, [r3, #4]
30009476:	f022 02ff 	bic.w	r2, r2, #255	; 0xff
3000947a:	f042 0204 	orr.w	r2, r2, #4
3000947e:	605a      	str	r2, [r3, #4]
30009480:	4770      	bx	lr
30009482:	bf00      	nop
30009484:	42008100 	.word	0x42008100
30009488:	fffffccc 	.word	0xfffffccc

3000948c <SWR_Calib_MEM>:
3000948c:	4b51      	ldr	r3, [pc, #324]	; (300095d4 <SWR_Calib_MEM+0x148>)
3000948e:	f240 3231 	movw	r2, #817	; 0x331
30009492:	4951      	ldr	r1, [pc, #324]	; (300095d8 <SWR_Calib_MEM+0x14c>)
30009494:	f04f 0c00 	mov.w	ip, #0
30009498:	b510      	push	{r4, lr}
3000949a:	f8d3 00e0 	ldr.w	r0, [r3, #224]	; 0xe0
3000949e:	b082      	sub	sp, #8
300094a0:	4001      	ands	r1, r0
300094a2:	f240 70e4 	movw	r0, #2020	; 0x7e4
300094a6:	430a      	orrs	r2, r1
300094a8:	f10d 0107 	add.w	r1, sp, #7
300094ac:	f8c3 20e0 	str.w	r2, [r3, #224]	; 0xe0
300094b0:	f8d3 20d0 	ldr.w	r2, [r3, #208]	; 0xd0
300094b4:	f442 22c0 	orr.w	r2, r2, #393216	; 0x60000
300094b8:	f8c3 20d0 	str.w	r2, [r3, #208]	; 0xd0
300094bc:	f8d3 20f8 	ldr.w	r2, [r3, #248]	; 0xf8
300094c0:	f042 0201 	orr.w	r2, r2, #1
300094c4:	f8c3 20f8 	str.w	r2, [r3, #248]	; 0xf8
300094c8:	f8d3 20fc 	ldr.w	r2, [r3, #252]	; 0xfc
300094cc:	f442 5280 	orr.w	r2, r2, #4096	; 0x1000
300094d0:	f8c3 20fc 	str.w	r2, [r3, #252]	; 0xfc
300094d4:	f88d c007 	strb.w	ip, [sp, #7]
300094d8:	f7fd ff5a 	bl	30007390 <OTP_Read8>
300094dc:	f89d 3007 	ldrb.w	r3, [sp, #7]
300094e0:	f003 02f0 	and.w	r2, r3, #240	; 0xf0
300094e4:	2af0      	cmp	r2, #240	; 0xf0
300094e6:	d156      	bne.n	30009596 <SWR_Calib_MEM+0x10a>
300094e8:	f003 030f 	and.w	r3, r3, #15
300094ec:	2b0f      	cmp	r3, #15
300094ee:	d04e      	beq.n	3000958e <SWR_Calib_MEM+0x102>
300094f0:	2302      	movs	r3, #2
300094f2:	4c3a      	ldr	r4, [pc, #232]	; (300095dc <SWR_Calib_MEM+0x150>)
300094f4:	f10d 0106 	add.w	r1, sp, #6
300094f8:	f240 70e4 	movw	r0, #2020	; 0x7e4
300094fc:	7023      	strb	r3, [r4, #0]
300094fe:	f7fd ff47 	bl	30007390 <OTP_Read8>
30009502:	7822      	ldrb	r2, [r4, #0]
30009504:	f89d 3006 	ldrb.w	r3, [sp, #6]
30009508:	2a03      	cmp	r2, #3
3000950a:	d05a      	beq.n	300095c2 <SWR_Calib_MEM+0x136>
3000950c:	2a02      	cmp	r2, #2
3000950e:	bf0c      	ite	eq
30009510:	f003 030f 	andeq.w	r3, r3, #15
30009514:	2300      	movne	r3, #0
30009516:	492f      	ldr	r1, [pc, #188]	; (300095d4 <SWR_Calib_MEM+0x148>)
30009518:	4831      	ldr	r0, [pc, #196]	; (300095e0 <SWR_Calib_MEM+0x154>)
3000951a:	f8d1 20d0 	ldr.w	r2, [r1, #208]	; 0xd0
3000951e:	f022 020f 	bic.w	r2, r2, #15
30009522:	4313      	orrs	r3, r2
30009524:	f8c1 30d0 	str.w	r3, [r1, #208]	; 0xd0
30009528:	6c83      	ldr	r3, [r0, #72]	; 0x48
3000952a:	f3c3 1382 	ubfx	r3, r3, #6, #3
3000952e:	2b07      	cmp	r3, #7
30009530:	d03a      	beq.n	300095a8 <SWR_Calib_MEM+0x11c>
30009532:	2300      	movs	r3, #0
30009534:	f10d 0107 	add.w	r1, sp, #7
30009538:	f240 70e5 	movw	r0, #2021	; 0x7e5
3000953c:	f88d 3007 	strb.w	r3, [sp, #7]
30009540:	f7fd ff26 	bl	30007390 <OTP_Read8>
30009544:	f89d 3007 	ldrb.w	r3, [sp, #7]
30009548:	f003 02f0 	and.w	r2, r3, #240	; 0xf0
3000954c:	2af0      	cmp	r2, #240	; 0xf0
3000954e:	d124      	bne.n	3000959a <SWR_Calib_MEM+0x10e>
30009550:	f003 030f 	and.w	r3, r3, #15
30009554:	2b0f      	cmp	r3, #15
30009556:	d022      	beq.n	3000959e <SWR_Calib_MEM+0x112>
30009558:	2302      	movs	r3, #2
3000955a:	4c22      	ldr	r4, [pc, #136]	; (300095e4 <SWR_Calib_MEM+0x158>)
3000955c:	f10d 0107 	add.w	r1, sp, #7
30009560:	f240 70e5 	movw	r0, #2021	; 0x7e5
30009564:	7023      	strb	r3, [r4, #0]
30009566:	f7fd ff13 	bl	30007390 <OTP_Read8>
3000956a:	7822      	ldrb	r2, [r4, #0]
3000956c:	f89d 3007 	ldrb.w	r3, [sp, #7]
30009570:	2a03      	cmp	r2, #3
30009572:	d028      	beq.n	300095c6 <SWR_Calib_MEM+0x13a>
30009574:	2a02      	cmp	r2, #2
30009576:	d029      	beq.n	300095cc <SWR_Calib_MEM+0x140>
30009578:	2300      	movs	r3, #0
3000957a:	4916      	ldr	r1, [pc, #88]	; (300095d4 <SWR_Calib_MEM+0x148>)
3000957c:	f8d1 20d0 	ldr.w	r2, [r1, #208]	; 0xd0
30009580:	f022 7270 	bic.w	r2, r2, #62914560	; 0x3c00000
30009584:	4313      	orrs	r3, r2
30009586:	f8c1 30d0 	str.w	r3, [r1, #208]	; 0xd0
3000958a:	b002      	add	sp, #8
3000958c:	bd10      	pop	{r4, pc}
3000958e:	4b13      	ldr	r3, [pc, #76]	; (300095dc <SWR_Calib_MEM+0x150>)
30009590:	2201      	movs	r2, #1
30009592:	701a      	strb	r2, [r3, #0]
30009594:	e7cd      	b.n	30009532 <SWR_Calib_MEM+0xa6>
30009596:	2303      	movs	r3, #3
30009598:	e7ab      	b.n	300094f2 <SWR_Calib_MEM+0x66>
3000959a:	2303      	movs	r3, #3
3000959c:	e7dd      	b.n	3000955a <SWR_Calib_MEM+0xce>
3000959e:	4b11      	ldr	r3, [pc, #68]	; (300095e4 <SWR_Calib_MEM+0x158>)
300095a0:	2201      	movs	r2, #1
300095a2:	701a      	strb	r2, [r3, #0]
300095a4:	b002      	add	sp, #8
300095a6:	bd10      	pop	{r4, pc}
300095a8:	6c83      	ldr	r3, [r0, #72]	; 0x48
300095aa:	f413 6f60 	tst.w	r3, #3584	; 0xe00
300095ae:	d1c0      	bne.n	30009532 <SWR_Calib_MEM+0xa6>
300095b0:	f8d1 30e4 	ldr.w	r3, [r1, #228]	; 0xe4
300095b4:	f423 73e0 	bic.w	r3, r3, #448	; 0x1c0
300095b8:	f443 6360 	orr.w	r3, r3, #3584	; 0xe00
300095bc:	f8c1 30e4 	str.w	r3, [r1, #228]	; 0xe4
300095c0:	e7b7      	b.n	30009532 <SWR_Calib_MEM+0xa6>
300095c2:	091b      	lsrs	r3, r3, #4
300095c4:	e7a7      	b.n	30009516 <SWR_Calib_MEM+0x8a>
300095c6:	091b      	lsrs	r3, r3, #4
300095c8:	059b      	lsls	r3, r3, #22
300095ca:	e7d6      	b.n	3000957a <SWR_Calib_MEM+0xee>
300095cc:	f003 030f 	and.w	r3, r3, #15
300095d0:	059b      	lsls	r3, r3, #22
300095d2:	e7d2      	b.n	3000957a <SWR_Calib_MEM+0xee>
300095d4:	42008800 	.word	0x42008800
300095d8:	fffffccc 	.word	0xfffffccc
300095dc:	3000ee94 	.word	0x3000ee94
300095e0:	42008100 	.word	0x42008100
300095e4:	3000ee93 	.word	0x3000ee93

300095e8 <SWR_Calib_AUD>:
300095e8:	4b48      	ldr	r3, [pc, #288]	; (3000970c <SWR_Calib_AUD+0x124>)
300095ea:	f240 3231 	movw	r2, #817	; 0x331
300095ee:	4948      	ldr	r1, [pc, #288]	; (30009710 <SWR_Calib_AUD+0x128>)
300095f0:	f04f 0c00 	mov.w	ip, #0
300095f4:	b510      	push	{r4, lr}
300095f6:	f8d3 00a0 	ldr.w	r0, [r3, #160]	; 0xa0
300095fa:	b082      	sub	sp, #8
300095fc:	4001      	ands	r1, r0
300095fe:	f240 70e6 	movw	r0, #2022	; 0x7e6
30009602:	430a      	orrs	r2, r1
30009604:	f10d 0107 	add.w	r1, sp, #7
30009608:	f8c3 20a0 	str.w	r2, [r3, #160]	; 0xa0
3000960c:	f8d3 2090 	ldr.w	r2, [r3, #144]	; 0x90
30009610:	f442 22c0 	orr.w	r2, r2, #393216	; 0x60000
30009614:	f8c3 2090 	str.w	r2, [r3, #144]	; 0x90
30009618:	f8d3 20b8 	ldr.w	r2, [r3, #184]	; 0xb8
3000961c:	f042 0201 	orr.w	r2, r2, #1
30009620:	f8c3 20b8 	str.w	r2, [r3, #184]	; 0xb8
30009624:	f8d3 20bc 	ldr.w	r2, [r3, #188]	; 0xbc
30009628:	f442 5280 	orr.w	r2, r2, #4096	; 0x1000
3000962c:	f8c3 20bc 	str.w	r2, [r3, #188]	; 0xbc
30009630:	f88d c007 	strb.w	ip, [sp, #7]
30009634:	f7fd feac 	bl	30007390 <OTP_Read8>
30009638:	f89d 3007 	ldrb.w	r3, [sp, #7]
3000963c:	f003 02f0 	and.w	r2, r3, #240	; 0xf0
30009640:	2af0      	cmp	r2, #240	; 0xf0
30009642:	d150      	bne.n	300096e6 <SWR_Calib_AUD+0xfe>
30009644:	f003 030f 	and.w	r3, r3, #15
30009648:	2b0f      	cmp	r3, #15
3000964a:	d048      	beq.n	300096de <SWR_Calib_AUD+0xf6>
3000964c:	2302      	movs	r3, #2
3000964e:	4c31      	ldr	r4, [pc, #196]	; (30009714 <SWR_Calib_AUD+0x12c>)
30009650:	f10d 0106 	add.w	r1, sp, #6
30009654:	f240 70e6 	movw	r0, #2022	; 0x7e6
30009658:	7023      	strb	r3, [r4, #0]
3000965a:	f7fd fe99 	bl	30007390 <OTP_Read8>
3000965e:	7822      	ldrb	r2, [r4, #0]
30009660:	f89d 3006 	ldrb.w	r3, [sp, #6]
30009664:	2a03      	cmp	r2, #3
30009666:	d047      	beq.n	300096f8 <SWR_Calib_AUD+0x110>
30009668:	2a02      	cmp	r2, #2
3000966a:	bf0c      	ite	eq
3000966c:	f003 030f 	andeq.w	r3, r3, #15
30009670:	2300      	movne	r3, #0
30009672:	4926      	ldr	r1, [pc, #152]	; (3000970c <SWR_Calib_AUD+0x124>)
30009674:	f8d1 2090 	ldr.w	r2, [r1, #144]	; 0x90
30009678:	f022 020f 	bic.w	r2, r2, #15
3000967c:	4313      	orrs	r3, r2
3000967e:	f8c1 3090 	str.w	r3, [r1, #144]	; 0x90
30009682:	2300      	movs	r3, #0
30009684:	f10d 0107 	add.w	r1, sp, #7
30009688:	f240 70e7 	movw	r0, #2023	; 0x7e7
3000968c:	f88d 3007 	strb.w	r3, [sp, #7]
30009690:	f7fd fe7e 	bl	30007390 <OTP_Read8>
30009694:	f89d 3007 	ldrb.w	r3, [sp, #7]
30009698:	f003 02f0 	and.w	r2, r3, #240	; 0xf0
3000969c:	2af0      	cmp	r2, #240	; 0xf0
3000969e:	d124      	bne.n	300096ea <SWR_Calib_AUD+0x102>
300096a0:	f003 030f 	and.w	r3, r3, #15
300096a4:	2b0f      	cmp	r3, #15
300096a6:	d022      	beq.n	300096ee <SWR_Calib_AUD+0x106>
300096a8:	2302      	movs	r3, #2
300096aa:	4c1b      	ldr	r4, [pc, #108]	; (30009718 <SWR_Calib_AUD+0x130>)
300096ac:	f10d 0107 	add.w	r1, sp, #7
300096b0:	f240 70e7 	movw	r0, #2023	; 0x7e7
300096b4:	7023      	strb	r3, [r4, #0]
300096b6:	f7fd fe6b 	bl	30007390 <OTP_Read8>
300096ba:	7822      	ldrb	r2, [r4, #0]
300096bc:	f89d 3007 	ldrb.w	r3, [sp, #7]
300096c0:	2a03      	cmp	r2, #3
300096c2:	d01b      	beq.n	300096fc <SWR_Calib_AUD+0x114>
300096c4:	2a02      	cmp	r2, #2
300096c6:	d01c      	beq.n	30009702 <SWR_Calib_AUD+0x11a>
300096c8:	2300      	movs	r3, #0
300096ca:	4910      	ldr	r1, [pc, #64]	; (3000970c <SWR_Calib_AUD+0x124>)
300096cc:	f8d1 2090 	ldr.w	r2, [r1, #144]	; 0x90
300096d0:	f022 7270 	bic.w	r2, r2, #62914560	; 0x3c00000
300096d4:	4313      	orrs	r3, r2
300096d6:	f8c1 3090 	str.w	r3, [r1, #144]	; 0x90
300096da:	b002      	add	sp, #8
300096dc:	bd10      	pop	{r4, pc}
300096de:	4b0d      	ldr	r3, [pc, #52]	; (30009714 <SWR_Calib_AUD+0x12c>)
300096e0:	2201      	movs	r2, #1
300096e2:	701a      	strb	r2, [r3, #0]
300096e4:	e7cd      	b.n	30009682 <SWR_Calib_AUD+0x9a>
300096e6:	2303      	movs	r3, #3
300096e8:	e7b1      	b.n	3000964e <SWR_Calib_AUD+0x66>
300096ea:	2303      	movs	r3, #3
300096ec:	e7dd      	b.n	300096aa <SWR_Calib_AUD+0xc2>
300096ee:	4b0a      	ldr	r3, [pc, #40]	; (30009718 <SWR_Calib_AUD+0x130>)
300096f0:	2201      	movs	r2, #1
300096f2:	701a      	strb	r2, [r3, #0]
300096f4:	b002      	add	sp, #8
300096f6:	bd10      	pop	{r4, pc}
300096f8:	091b      	lsrs	r3, r3, #4
300096fa:	e7ba      	b.n	30009672 <SWR_Calib_AUD+0x8a>
300096fc:	091b      	lsrs	r3, r3, #4
300096fe:	059b      	lsls	r3, r3, #22
30009700:	e7e3      	b.n	300096ca <SWR_Calib_AUD+0xe2>
30009702:	f003 030f 	and.w	r3, r3, #15
30009706:	059b      	lsls	r3, r3, #22
30009708:	e7df      	b.n	300096ca <SWR_Calib_AUD+0xe2>
3000970a:	bf00      	nop
3000970c:	42008800 	.word	0x42008800
30009710:	fffffccc 	.word	0xfffffccc
30009714:	3000ee92 	.word	0x3000ee92
30009718:	3000ee91 	.word	0x3000ee91

3000971c <HANDLER_SecureFault>:
3000971c:	f3ef 8088 	mrs	r0, MSP_NS
30009720:	f3ef 8189 	mrs	r1, PSP_NS
30009724:	4672      	mov	r2, lr
30009726:	f3ef 8388 	mrs	r3, MSP_NS
3000972a:	e923 0ff0 	stmdb	r3!, {r4, r5, r6, r7, r8, r9, sl, fp}
3000972e:	f04f 0304 	mov.w	r3, #4
30009732:	f000 b831 	b.w	30009798 <Fault_Handler>
30009736:	4770      	bx	lr

30009738 <HANDLER_MemFault>:
30009738:	f3ef 8008 	mrs	r0, MSP
3000973c:	f3ef 8109 	mrs	r1, PSP
30009740:	4672      	mov	r2, lr
30009742:	f04f 0303 	mov.w	r3, #3
30009746:	e92d 0ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
3000974a:	f000 b825 	b.w	30009798 <Fault_Handler>
3000974e:	4770      	bx	lr

30009750 <HANDLER_BusFault>:
30009750:	f3ef 8008 	mrs	r0, MSP
30009754:	f3ef 8109 	mrs	r1, PSP
30009758:	4672      	mov	r2, lr
3000975a:	f04f 0302 	mov.w	r3, #2
3000975e:	e92d 0ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
30009762:	f000 b819 	b.w	30009798 <Fault_Handler>
30009766:	4770      	bx	lr

30009768 <HANDLER_UsageFault>:
30009768:	f3ef 8008 	mrs	r0, MSP
3000976c:	f3ef 8109 	mrs	r1, PSP
30009770:	4672      	mov	r2, lr
30009772:	f04f 0301 	mov.w	r3, #1
30009776:	e92d 0ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
3000977a:	f000 b80d 	b.w	30009798 <Fault_Handler>
3000977e:	4770      	bx	lr

30009780 <HANDLER_HardFault>:
30009780:	f3ef 8008 	mrs	r0, MSP
30009784:	f3ef 8109 	mrs	r1, PSP
30009788:	4672      	mov	r2, lr
3000978a:	f04f 0300 	mov.w	r3, #0
3000978e:	e92d 0ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
30009792:	f000 b801 	b.w	30009798 <Fault_Handler>
30009796:	4770      	bx	lr

30009798 <Fault_Handler>:
30009798:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
3000979c:	b095      	sub	sp, #84	; 0x54
3000979e:	4614      	mov	r4, r2
300097a0:	4605      	mov	r5, r0
300097a2:	2244      	movs	r2, #68	; 0x44
300097a4:	460e      	mov	r6, r1
300097a6:	a803      	add	r0, sp, #12
300097a8:	2100      	movs	r1, #0
300097aa:	461f      	mov	r7, r3
300097ac:	f002 fff0 	bl	3000c790 <____wrap_memset_veneer>
300097b0:	07e2      	lsls	r2, r4, #31
300097b2:	f004 0340 	and.w	r3, r4, #64	; 0x40
300097b6:	d554      	bpl.n	30009862 <Fault_Handler+0xca>
300097b8:	2b00      	cmp	r3, #0
300097ba:	d05b      	beq.n	30009874 <Fault_Handler+0xdc>
300097bc:	f004 000c 	and.w	r0, r4, #12
300097c0:	f04f 0800 	mov.w	r8, #0
300097c4:	f1a0 000c 	sub.w	r0, r0, #12
300097c8:	fab0 f080 	clz	r0, r0
300097cc:	0940      	lsrs	r0, r0, #5
300097ce:	f1a5 0c24 	sub.w	ip, r5, #36	; 0x24
300097d2:	ab04      	add	r3, sp, #16
300097d4:	1f29      	subs	r1, r5, #4
300097d6:	9403      	str	r4, [sp, #12]
300097d8:	f85c 2f04 	ldr.w	r2, [ip, #4]!
300097dc:	458c      	cmp	ip, r1
300097de:	f843 2b04 	str.w	r2, [r3], #4
300097e2:	d1f9      	bne.n	300097d8 <Fault_Handler+0x40>
300097e4:	2800      	cmp	r0, #0
300097e6:	bf14      	ite	ne
300097e8:	4631      	movne	r1, r6
300097ea:	4629      	moveq	r1, r5
300097ec:	f10d 0e30 	add.w	lr, sp, #48	; 0x30
300097f0:	f1a1 0c04 	sub.w	ip, r1, #4
300097f4:	f101 021c 	add.w	r2, r1, #28
300097f8:	f85c 3f04 	ldr.w	r3, [ip, #4]!
300097fc:	4594      	cmp	ip, r2
300097fe:	f84e 3b04 	str.w	r3, [lr], #4
30009802:	d1f9      	bne.n	300097f8 <Fault_Handler+0x60>
30009804:	aa03      	add	r2, sp, #12
30009806:	6bc8      	ldr	r0, [r1, #60]	; 0x3c
30009808:	f001 f854 	bl	3000a8b4 <crash_dump>
3000980c:	2f04      	cmp	r7, #4
3000980e:	d042      	beq.n	30009896 <Fault_Handler+0xfe>
30009810:	f1b8 0f00 	cmp.w	r8, #0
30009814:	d136      	bne.n	30009884 <Fault_Handler+0xec>
30009816:	f8df 80c8 	ldr.w	r8, [pc, #200]	; 300098e0 <Fault_Handler+0x148>
3000981a:	2241      	movs	r2, #65	; 0x41
3000981c:	4b28      	ldr	r3, [pc, #160]	; (300098c0 <Fault_Handler+0x128>)
3000981e:	f8d8 0024 	ldr.w	r0, [r8, #36]	; 0x24
30009822:	4928      	ldr	r1, [pc, #160]	; (300098c4 <Fault_Handler+0x12c>)
30009824:	9000      	str	r0, [sp, #0]
30009826:	2001      	movs	r0, #1
30009828:	f000 fd76 	bl	3000a318 <rtk_log_write>
3000982c:	f8d8 300c 	ldr.w	r3, [r8, #12]
30009830:	2241      	movs	r2, #65	; 0x41
30009832:	4924      	ldr	r1, [pc, #144]	; (300098c4 <Fault_Handler+0x12c>)
30009834:	9300      	str	r3, [sp, #0]
30009836:	2001      	movs	r0, #1
30009838:	4b23      	ldr	r3, [pc, #140]	; (300098c8 <Fault_Handler+0x130>)
3000983a:	f000 fd6d 	bl	3000a318 <rtk_log_write>
3000983e:	f3ef 8314 	mrs	r3, CONTROL
30009842:	2241      	movs	r2, #65	; 0x41
30009844:	9300      	str	r3, [sp, #0]
30009846:	491f      	ldr	r1, [pc, #124]	; (300098c4 <Fault_Handler+0x12c>)
30009848:	2001      	movs	r0, #1
3000984a:	4b20      	ldr	r3, [pc, #128]	; (300098cc <Fault_Handler+0x134>)
3000984c:	f000 fd64 	bl	3000a318 <rtk_log_write>
30009850:	463b      	mov	r3, r7
30009852:	4622      	mov	r2, r4
30009854:	4631      	mov	r1, r6
30009856:	4628      	mov	r0, r5
30009858:	f002 ffa2 	bl	3000c7a0 <__INT_HardFault_C_veneer>
3000985c:	b015      	add	sp, #84	; 0x54
3000985e:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
30009862:	2b00      	cmp	r3, #0
30009864:	d0aa      	beq.n	300097bc <Fault_Handler+0x24>
30009866:	4b1a      	ldr	r3, [pc, #104]	; (300098d0 <Fault_Handler+0x138>)
30009868:	2241      	movs	r2, #65	; 0x41
3000986a:	4916      	ldr	r1, [pc, #88]	; (300098c4 <Fault_Handler+0x12c>)
3000986c:	2001      	movs	r0, #1
3000986e:	f000 fd53 	bl	3000a318 <rtk_log_write>
30009872:	e7fe      	b.n	30009872 <Fault_Handler+0xda>
30009874:	f3ef 8894 	mrs	r8, CONTROL_NS
30009878:	f014 0008 	ands.w	r0, r4, #8
3000987c:	d0a7      	beq.n	300097ce <Fault_Handler+0x36>
3000987e:	f3c8 0040 	ubfx	r0, r8, #1, #1
30009882:	e7a4      	b.n	300097ce <Fault_Handler+0x36>
30009884:	4b13      	ldr	r3, [pc, #76]	; (300098d4 <Fault_Handler+0x13c>)
30009886:	2241      	movs	r2, #65	; 0x41
30009888:	490e      	ldr	r1, [pc, #56]	; (300098c4 <Fault_Handler+0x12c>)
3000988a:	2001      	movs	r0, #1
3000988c:	f8cd 8000 	str.w	r8, [sp]
30009890:	f000 fd42 	bl	3000a318 <rtk_log_write>
30009894:	e7bf      	b.n	30009816 <Fault_Handler+0x7e>
30009896:	f8df 9048 	ldr.w	r9, [pc, #72]	; 300098e0 <Fault_Handler+0x148>
3000989a:	2241      	movs	r2, #65	; 0x41
3000989c:	4b0e      	ldr	r3, [pc, #56]	; (300098d8 <Fault_Handler+0x140>)
3000989e:	f8d9 00e4 	ldr.w	r0, [r9, #228]	; 0xe4
300098a2:	4908      	ldr	r1, [pc, #32]	; (300098c4 <Fault_Handler+0x12c>)
300098a4:	9000      	str	r0, [sp, #0]
300098a6:	2001      	movs	r0, #1
300098a8:	f000 fd36 	bl	3000a318 <rtk_log_write>
300098ac:	f8d9 30e8 	ldr.w	r3, [r9, #232]	; 0xe8
300098b0:	2241      	movs	r2, #65	; 0x41
300098b2:	4904      	ldr	r1, [pc, #16]	; (300098c4 <Fault_Handler+0x12c>)
300098b4:	9300      	str	r3, [sp, #0]
300098b6:	2001      	movs	r0, #1
300098b8:	4b08      	ldr	r3, [pc, #32]	; (300098dc <Fault_Handler+0x144>)
300098ba:	f000 fd2d 	bl	3000a318 <rtk_log_write>
300098be:	e7a7      	b.n	30009810 <Fault_Handler+0x78>
300098c0:	3000d314 	.word	0x3000d314
300098c4:	3000d2cc 	.word	0x3000d2cc
300098c8:	3000d328 	.word	0x3000d328
300098cc:	3000d33c 	.word	0x3000d33c
300098d0:	3000d280 	.word	0x3000d280
300098d4:	3000d2fc 	.word	0x3000d2fc
300098d8:	3000d2d4 	.word	0x3000d2d4
300098dc:	3000d2e8 	.word	0x3000d2e8
300098e0:	e000ed00 	.word	0xe000ed00

300098e4 <Fault_Hanlder_Redirect>:
300098e4:	4b08      	ldr	r3, [pc, #32]	; (30009908 <Fault_Hanlder_Redirect+0x24>)
300098e6:	4909      	ldr	r1, [pc, #36]	; (3000990c <Fault_Hanlder_Redirect+0x28>)
300098e8:	4a09      	ldr	r2, [pc, #36]	; (30009910 <Fault_Hanlder_Redirect+0x2c>)
300098ea:	b410      	push	{r4}
300098ec:	6008      	str	r0, [r1, #0]
300098ee:	4c09      	ldr	r4, [pc, #36]	; (30009914 <Fault_Hanlder_Redirect+0x30>)
300098f0:	60da      	str	r2, [r3, #12]
300098f2:	4809      	ldr	r0, [pc, #36]	; (30009918 <Fault_Hanlder_Redirect+0x34>)
300098f4:	4909      	ldr	r1, [pc, #36]	; (3000991c <Fault_Hanlder_Redirect+0x38>)
300098f6:	4a0a      	ldr	r2, [pc, #40]	; (30009920 <Fault_Hanlder_Redirect+0x3c>)
300098f8:	e9c3 4004 	strd	r4, r0, [r3, #16]
300098fc:	e9c3 1206 	strd	r1, r2, [r3, #24]
30009900:	f85d 4b04 	ldr.w	r4, [sp], #4
30009904:	4770      	bx	lr
30009906:	bf00      	nop
30009908:	30000000 	.word	0x30000000
3000990c:	3000f968 	.word	0x3000f968
30009910:	30009781 	.word	0x30009781
30009914:	30009739 	.word	0x30009739
30009918:	30009751 	.word	0x30009751
3000991c:	30009769 	.word	0x30009769
30009920:	3000971d 	.word	0x3000971d

30009924 <ChipInfo_InitPsramInfoFromMemInfo>:
30009924:	b538      	push	{r3, r4, r5, lr}
30009926:	460c      	mov	r4, r1
30009928:	4b1d      	ldr	r3, [pc, #116]	; (300099a0 <ChipInfo_InitPsramInfoFromMemInfo+0x7c>)
3000992a:	2228      	movs	r2, #40	; 0x28
3000992c:	2100      	movs	r1, #0
3000992e:	4605      	mov	r5, r0
30009930:	4620      	mov	r0, r4
30009932:	4798      	blx	r3
30009934:	2300      	movs	r3, #0
30009936:	6123      	str	r3, [r4, #16]
30009938:	b1bd      	cbz	r5, 3000996a <ChipInfo_InitPsramInfoFromMemInfo+0x46>
3000993a:	fab4 f284 	clz	r2, r4
3000993e:	0952      	lsrs	r2, r2, #5
30009940:	b19c      	cbz	r4, 3000996a <ChipInfo_InitPsramInfoFromMemInfo+0x46>
30009942:	782b      	ldrb	r3, [r5, #0]
30009944:	f013 0304 	ands.w	r3, r3, #4
30009948:	d010      	beq.n	3000996c <ChipInfo_InitPsramInfoFromMemInfo+0x48>
3000994a:	78ab      	ldrb	r3, [r5, #2]
3000994c:	f003 0307 	and.w	r3, r3, #7
30009950:	2b01      	cmp	r3, #1
30009952:	d00f      	beq.n	30009974 <ChipInfo_InitPsramInfoFromMemInfo+0x50>
30009954:	2b02      	cmp	r3, #2
30009956:	d10b      	bne.n	30009970 <ChipInfo_InitPsramInfoFromMemInfo+0x4c>
30009958:	6023      	str	r3, [r4, #0]
3000995a:	78ab      	ldrb	r3, [r5, #2]
3000995c:	08db      	lsrs	r3, r3, #3
3000995e:	2b06      	cmp	r3, #6
30009960:	d019      	beq.n	30009996 <ChipInfo_InitPsramInfoFromMemInfo+0x72>
30009962:	2b07      	cmp	r3, #7
30009964:	d013      	beq.n	3000998e <ChipInfo_InitPsramInfoFromMemInfo+0x6a>
30009966:	2b04      	cmp	r3, #4
30009968:	d00d      	beq.n	30009986 <ChipInfo_InitPsramInfoFromMemInfo+0x62>
3000996a:	bd38      	pop	{r3, r4, r5, pc}
3000996c:	6023      	str	r3, [r4, #0]
3000996e:	bd38      	pop	{r3, r4, r5, pc}
30009970:	6022      	str	r2, [r4, #0]
30009972:	bd38      	pop	{r3, r4, r5, pc}
30009974:	6023      	str	r3, [r4, #0]
30009976:	78ab      	ldrb	r3, [r5, #2]
30009978:	08db      	lsrs	r3, r3, #3
3000997a:	2b05      	cmp	r3, #5
3000997c:	d1f5      	bne.n	3000996a <ChipInfo_InitPsramInfoFromMemInfo+0x46>
3000997e:	f44f 0300 	mov.w	r3, #8388608	; 0x800000
30009982:	60a3      	str	r3, [r4, #8]
30009984:	bd38      	pop	{r3, r4, r5, pc}
30009986:	f44f 0380 	mov.w	r3, #4194304	; 0x400000
3000998a:	60a3      	str	r3, [r4, #8]
3000998c:	bd38      	pop	{r3, r4, r5, pc}
3000998e:	f04f 7300 	mov.w	r3, #33554432	; 0x2000000
30009992:	60a3      	str	r3, [r4, #8]
30009994:	bd38      	pop	{r3, r4, r5, pc}
30009996:	f04f 7380 	mov.w	r3, #16777216	; 0x1000000
3000999a:	60a3      	str	r3, [r4, #8]
3000999c:	bd38      	pop	{r3, r4, r5, pc}
3000999e:	bf00      	nop
300099a0:	00012be5 	.word	0x00012be5

300099a4 <PSRAM_INFO_Update>:
300099a4:	4a48      	ldr	r2, [pc, #288]	; (30009ac8 <PSRAM_INFO_Update+0x124>)
300099a6:	4b49      	ldr	r3, [pc, #292]	; (30009acc <PSRAM_INFO_Update+0x128>)
300099a8:	b570      	push	{r4, r5, r6, lr}
300099aa:	4c49      	ldr	r4, [pc, #292]	; (30009ad0 <PSRAM_INFO_Update+0x12c>)
300099ac:	b084      	sub	sp, #16
300099ae:	f8d2 5228 	ldr.w	r5, [r2, #552]	; 0x228
300099b2:	4798      	blx	r3
300099b4:	4603      	mov	r3, r0
300099b6:	4621      	mov	r1, r4
300099b8:	a803      	add	r0, sp, #12
300099ba:	f3c5 2502 	ubfx	r5, r5, #8, #3
300099be:	9303      	str	r3, [sp, #12]
300099c0:	f7ff ffb0 	bl	30009924 <ChipInfo_InitPsramInfoFromMemInfo>
300099c4:	4943      	ldr	r1, [pc, #268]	; (30009ad4 <PSRAM_INFO_Update+0x130>)
300099c6:	3501      	adds	r5, #1
300099c8:	4a43      	ldr	r2, [pc, #268]	; (30009ad8 <PSRAM_INFO_Update+0x134>)
300099ca:	260a      	movs	r6, #10
300099cc:	6809      	ldr	r1, [r1, #0]
300099ce:	4b43      	ldr	r3, [pc, #268]	; (30009adc <PSRAM_INFO_Update+0x138>)
300099d0:	fb02 f101 	mul.w	r1, r2, r1
300099d4:	2220      	movs	r2, #32
300099d6:	6126      	str	r6, [r4, #16]
300099d8:	fbb1 f1f5 	udiv	r1, r1, r5
300099dc:	fbb3 f3f1 	udiv	r3, r3, r1
300099e0:	6163      	str	r3, [r4, #20]
300099e2:	0848      	lsrs	r0, r1, #1
300099e4:	fbb2 f3f3 	udiv	r3, r2, r3
300099e8:	6223      	str	r3, [r4, #32]
300099ea:	6823      	ldr	r3, [r4, #0]
300099ec:	60e0      	str	r0, [r4, #12]
300099ee:	2b01      	cmp	r3, #1
300099f0:	4b3b      	ldr	r3, [pc, #236]	; (30009ae0 <PSRAM_INFO_Update+0x13c>)
300099f2:	d01d      	beq.n	30009a30 <PSRAM_INFO_Update+0x8c>
300099f4:	4298      	cmp	r0, r3
300099f6:	d810      	bhi.n	30009a1a <PSRAM_INFO_Update+0x76>
300099f8:	2304      	movs	r3, #4
300099fa:	2208      	movs	r2, #8
300099fc:	461d      	mov	r5, r3
300099fe:	e9c4 3206 	strd	r3, r2, [r4, #24]
30009a02:	4838      	ldr	r0, [pc, #224]	; (30009ae4 <PSRAM_INFO_Update+0x140>)
30009a04:	2249      	movs	r2, #73	; 0x49
30009a06:	4b38      	ldr	r3, [pc, #224]	; (30009ae8 <PSRAM_INFO_Update+0x144>)
30009a08:	f880 5114 	strb.w	r5, [r0, #276]	; 0x114
30009a0c:	2004      	movs	r0, #4
30009a0e:	9100      	str	r1, [sp, #0]
30009a10:	4936      	ldr	r1, [pc, #216]	; (30009aec <PSRAM_INFO_Update+0x148>)
30009a12:	f000 fc81 	bl	3000a318 <rtk_log_write>
30009a16:	b004      	add	sp, #16
30009a18:	bd70      	pop	{r4, r5, r6, pc}
30009a1a:	4b35      	ldr	r3, [pc, #212]	; (30009af0 <PSRAM_INFO_Update+0x14c>)
30009a1c:	4298      	cmp	r0, r3
30009a1e:	d918      	bls.n	30009a52 <PSRAM_INFO_Update+0xae>
30009a20:	4b34      	ldr	r3, [pc, #208]	; (30009af4 <PSRAM_INFO_Update+0x150>)
30009a22:	4298      	cmp	r0, r3
30009a24:	d827      	bhi.n	30009a76 <PSRAM_INFO_Update+0xd2>
30009a26:	2306      	movs	r3, #6
30009a28:	461d      	mov	r5, r3
30009a2a:	e9c4 3306 	strd	r3, r3, [r4, #24]
30009a2e:	e7e8      	b.n	30009a02 <PSRAM_INFO_Update+0x5e>
30009a30:	4298      	cmp	r0, r3
30009a32:	d908      	bls.n	30009a46 <PSRAM_INFO_Update+0xa2>
30009a34:	4b2e      	ldr	r3, [pc, #184]	; (30009af0 <PSRAM_INFO_Update+0x14c>)
30009a36:	4298      	cmp	r0, r3
30009a38:	d811      	bhi.n	30009a5e <PSRAM_INFO_Update+0xba>
30009a3a:	2305      	movs	r3, #5
30009a3c:	220f      	movs	r2, #15
30009a3e:	461d      	mov	r5, r3
30009a40:	e9c4 3206 	strd	r3, r2, [r4, #24]
30009a44:	e7dd      	b.n	30009a02 <PSRAM_INFO_Update+0x5e>
30009a46:	2304      	movs	r3, #4
30009a48:	220f      	movs	r2, #15
30009a4a:	461d      	mov	r5, r3
30009a4c:	e9c4 3206 	strd	r3, r2, [r4, #24]
30009a50:	e7d7      	b.n	30009a02 <PSRAM_INFO_Update+0x5e>
30009a52:	2305      	movs	r3, #5
30009a54:	2208      	movs	r2, #8
30009a56:	461d      	mov	r5, r3
30009a58:	e9c4 3206 	strd	r3, r2, [r4, #24]
30009a5c:	e7d1      	b.n	30009a02 <PSRAM_INFO_Update+0x5e>
30009a5e:	4b25      	ldr	r3, [pc, #148]	; (30009af4 <PSRAM_INFO_Update+0x150>)
30009a60:	4298      	cmp	r0, r3
30009a62:	d917      	bls.n	30009a94 <PSRAM_INFO_Update+0xf0>
30009a64:	4b24      	ldr	r3, [pc, #144]	; (30009af8 <PSRAM_INFO_Update+0x154>)
30009a66:	4298      	cmp	r0, r3
30009a68:	d820      	bhi.n	30009aac <PSRAM_INFO_Update+0x108>
30009a6a:	2307      	movs	r3, #7
30009a6c:	2214      	movs	r2, #20
30009a6e:	461d      	mov	r5, r3
30009a70:	e9c4 3206 	strd	r3, r2, [r4, #24]
30009a74:	e7c5      	b.n	30009a02 <PSRAM_INFO_Update+0x5e>
30009a76:	4b20      	ldr	r3, [pc, #128]	; (30009af8 <PSRAM_INFO_Update+0x154>)
30009a78:	4298      	cmp	r0, r3
30009a7a:	d911      	bls.n	30009aa0 <PSRAM_INFO_Update+0xfc>
30009a7c:	4b1f      	ldr	r3, [pc, #124]	; (30009afc <PSRAM_INFO_Update+0x158>)
30009a7e:	4298      	cmp	r0, r3
30009a80:	d806      	bhi.n	30009a90 <PSRAM_INFO_Update+0xec>
30009a82:	68a3      	ldr	r3, [r4, #8]
30009a84:	f1b3 7f00 	cmp.w	r3, #33554432	; 0x2000000
30009a88:	d00a      	beq.n	30009aa0 <PSRAM_INFO_Update+0xfc>
30009a8a:	f1b3 7f80 	cmp.w	r3, #16777216	; 0x1000000
30009a8e:	d016      	beq.n	30009abe <PSRAM_INFO_Update+0x11a>
30009a90:	7e25      	ldrb	r5, [r4, #24]
30009a92:	e7b6      	b.n	30009a02 <PSRAM_INFO_Update+0x5e>
30009a94:	2306      	movs	r3, #6
30009a96:	2212      	movs	r2, #18
30009a98:	461d      	mov	r5, r3
30009a9a:	e9c4 3206 	strd	r3, r2, [r4, #24]
30009a9e:	e7b0      	b.n	30009a02 <PSRAM_INFO_Update+0x5e>
30009aa0:	2307      	movs	r3, #7
30009aa2:	2206      	movs	r2, #6
30009aa4:	461d      	mov	r5, r3
30009aa6:	e9c4 3206 	strd	r3, r2, [r4, #24]
30009aaa:	e7aa      	b.n	30009a02 <PSRAM_INFO_Update+0x5e>
30009aac:	4b13      	ldr	r3, [pc, #76]	; (30009afc <PSRAM_INFO_Update+0x158>)
30009aae:	4298      	cmp	r0, r3
30009ab0:	d8ee      	bhi.n	30009a90 <PSRAM_INFO_Update+0xec>
30009ab2:	2308      	movs	r3, #8
30009ab4:	221c      	movs	r2, #28
30009ab6:	461d      	mov	r5, r3
30009ab8:	e9c4 3206 	strd	r3, r2, [r4, #24]
30009abc:	e7a1      	b.n	30009a02 <PSRAM_INFO_Update+0x5e>
30009abe:	2306      	movs	r3, #6
30009ac0:	4635      	mov	r5, r6
30009ac2:	e9c4 6306 	strd	r6, r3, [r4, #24]
30009ac6:	e79c      	b.n	30009a02 <PSRAM_INFO_Update+0x5e>
30009ac8:	42008000 	.word	0x42008000
30009acc:	3000af89 	.word	0x3000af89
30009ad0:	3000f8f8 	.word	0x3000f8f8
30009ad4:	3000f044 	.word	0x3000f044
30009ad8:	000f4240 	.word	0x000f4240
30009adc:	3b9aca00 	.word	0x3b9aca00
30009ae0:	0632ea00 	.word	0x0632ea00
30009ae4:	23020000 	.word	0x23020000
30009ae8:	3000d350 	.word	0x3000d350
30009aec:	3000d36c 	.word	0x3000d36c
30009af0:	07ed6b40 	.word	0x07ed6b40
30009af4:	09e4f580 	.word	0x09e4f580
30009af8:	0bebc200 	.word	0x0bebc200
30009afc:	0ee6b280 	.word	0x0ee6b280

30009b00 <PSRAM_PHY_StructInit>:
30009b00:	2203      	movs	r2, #3
30009b02:	2301      	movs	r3, #1
30009b04:	210a      	movs	r1, #10
30009b06:	b410      	push	{r4}
30009b08:	e9c0 2202 	strd	r2, r2, [r0, #8]
30009b0c:	2402      	movs	r4, #2
30009b0e:	2207      	movs	r2, #7
30009b10:	e9c0 3400 	strd	r3, r4, [r0]
30009b14:	e9c0 1304 	strd	r1, r3, [r0, #16]
30009b18:	f85d 4b04 	ldr.w	r4, [sp], #4
30009b1c:	6182      	str	r2, [r0, #24]
30009b1e:	4770      	bx	lr

30009b20 <PSRAM_PHY_Init>:
30009b20:	b430      	push	{r4, r5}
30009b22:	6903      	ldr	r3, [r0, #16]
30009b24:	6842      	ldr	r2, [r0, #4]
30009b26:	6805      	ldr	r5, [r0, #0]
30009b28:	f003 031f 	and.w	r3, r3, #31
30009b2c:	0612      	lsls	r2, r2, #24
30009b2e:	4912      	ldr	r1, [pc, #72]	; (30009b78 <PSRAM_PHY_Init+0x58>)
30009b30:	ea43 7305 	orr.w	r3, r3, r5, lsl #28
30009b34:	4c11      	ldr	r4, [pc, #68]	; (30009b7c <PSRAM_PHY_Init+0x5c>)
30009b36:	f002 62e0 	and.w	r2, r2, #117440512	; 0x7000000
30009b3a:	4313      	orrs	r3, r2
30009b3c:	6882      	ldr	r2, [r0, #8]
30009b3e:	0412      	lsls	r2, r2, #16
30009b40:	f402 2270 	and.w	r2, r2, #983040	; 0xf0000
30009b44:	4313      	orrs	r3, r2
30009b46:	68c2      	ldr	r2, [r0, #12]
30009b48:	0212      	lsls	r2, r2, #8
30009b4a:	f402 6270 	and.w	r2, r2, #3840	; 0xf00
30009b4e:	4313      	orrs	r3, r2
30009b50:	604b      	str	r3, [r1, #4]
30009b52:	6823      	ldr	r3, [r4, #0]
30009b54:	2b01      	cmp	r3, #1
30009b56:	e9d0 3005 	ldrd	r3, r0, [r0, #20]
30009b5a:	bf08      	it	eq
30009b5c:	4a08      	ldreq	r2, [pc, #32]	; (30009b80 <PSRAM_PHY_Init+0x60>)
30009b5e:	ea4f 1303 	mov.w	r3, r3, lsl #4
30009b62:	f000 000f 	and.w	r0, r0, #15
30009b66:	bf18      	it	ne
30009b68:	4a06      	ldrne	r2, [pc, #24]	; (30009b84 <PSRAM_PHY_Init+0x64>)
30009b6a:	b2db      	uxtb	r3, r3
30009b6c:	4303      	orrs	r3, r0
30009b6e:	431a      	orrs	r2, r3
30009b70:	618a      	str	r2, [r1, #24]
30009b72:	bc30      	pop	{r4, r5}
30009b74:	4770      	bx	lr
30009b76:	bf00      	nop
30009b78:	41002000 	.word	0x41002000
30009b7c:	3000f8f8 	.word	0x3000f8f8
30009b80:	80000e00 	.word	0x80000e00
30009b84:	80000f00 	.word	0x80000f00

30009b88 <PSRAM_CTRL_Init>:
30009b88:	b470      	push	{r4, r5, r6}
30009b8a:	4b4c      	ldr	r3, [pc, #304]	; (30009cbc <PSRAM_CTRL_Init+0x134>)
30009b8c:	2400      	movs	r4, #0
30009b8e:	4a4c      	ldr	r2, [pc, #304]	; (30009cc0 <PSRAM_CTRL_Init+0x138>)
30009b90:	f24d 0101 	movw	r1, #53249	; 0xd001
30009b94:	609c      	str	r4, [r3, #8]
30009b96:	2001      	movs	r0, #1
30009b98:	601a      	str	r2, [r3, #0]
30009b9a:	4a4a      	ldr	r2, [pc, #296]	; (30009cc4 <PSRAM_CTRL_Init+0x13c>)
30009b9c:	6158      	str	r0, [r3, #20]
30009b9e:	f8c3 1120 	str.w	r1, [r3, #288]	; 0x120
30009ba2:	6811      	ldr	r1, [r2, #0]
30009ba4:	2902      	cmp	r1, #2
30009ba6:	d04a      	beq.n	30009c3e <PSRAM_CTRL_Init+0xb6>
30009ba8:	6954      	ldr	r4, [r2, #20]
30009baa:	f44f 717a 	mov.w	r1, #1000	; 0x3e8
30009bae:	69d0      	ldr	r0, [r2, #28]
30009bb0:	6915      	ldr	r5, [r2, #16]
30009bb2:	fbb0 f0f4 	udiv	r0, r0, r4
30009bb6:	fbb1 f1f4 	udiv	r1, r1, r4
30009bba:	6994      	ldr	r4, [r2, #24]
30009bbc:	f005 020f 	and.w	r2, r5, #15
30009bc0:	f241 5551 	movw	r5, #5457	; 0x1551
30009bc4:	f442 628e 	orr.w	r2, r2, #1136	; 0x470
30009bc8:	f8c3 5110 	str.w	r5, [r3, #272]	; 0x110
30009bcc:	f8c3 2134 	str.w	r2, [r3, #308]	; 0x134
30009bd0:	094a      	lsrs	r2, r1, #5
30009bd2:	0181      	lsls	r1, r0, #6
30009bd4:	f000 003f 	and.w	r0, r0, #63	; 0x3f
30009bd8:	4d3b      	ldr	r5, [pc, #236]	; (30009cc8 <PSRAM_CTRL_Init+0x140>)
30009bda:	f401 617c 	and.w	r1, r1, #4032	; 0xfc0
30009bde:	ea41 6102 	orr.w	r1, r1, r2, lsl #24
30009be2:	00a2      	lsls	r2, r4, #2
30009be4:	4301      	orrs	r1, r0
30009be6:	f24a 00a0 	movw	r0, #41120	; 0xa0a0
30009bea:	3a01      	subs	r2, #1
30009bec:	430d      	orrs	r5, r1
30009bee:	f242 0120 	movw	r1, #8224	; 0x2020
30009bf2:	f3c2 020b 	ubfx	r2, r2, #0, #12
30009bf6:	f8c3 5138 	str.w	r5, [r3, #312]	; 0x138
30009bfa:	f8c3 10e0 	str.w	r1, [r3, #224]	; 0xe0
30009bfe:	f44f 2180 	mov.w	r1, #262144	; 0x40000
30009c02:	f8c3 00f4 	str.w	r0, [r3, #244]	; 0xf4
30009c06:	f06f 4000 	mvn.w	r0, #2147483648	; 0x80000000
30009c0a:	f8c3 111c 	str.w	r1, [r3, #284]	; 0x11c
30009c0e:	f442 2184 	orr.w	r1, r2, #270336	; 0x42000
30009c12:	4420      	add	r0, r4
30009c14:	f8c3 1118 	str.w	r1, [r3, #280]	; 0x118
30009c18:	f640 71fe 	movw	r1, #4094	; 0xffe
30009c1c:	f8d3 511c 	ldr.w	r5, [r3, #284]	; 0x11c
30009c20:	ea01 0140 	and.w	r1, r1, r0, lsl #1
30009c24:	432a      	orrs	r2, r5
30009c26:	f442 4200 	orr.w	r2, r2, #32768	; 0x8000
30009c2a:	f8c3 211c 	str.w	r2, [r3, #284]	; 0x11c
30009c2e:	f8c3 113c 	str.w	r1, [r3, #316]	; 0x13c
30009c32:	4a22      	ldr	r2, [pc, #136]	; (30009cbc <PSRAM_CTRL_Init+0x134>)
30009c34:	6a93      	ldr	r3, [r2, #40]	; 0x28
30009c36:	061b      	lsls	r3, r3, #24
30009c38:	d5fc      	bpl.n	30009c34 <PSRAM_CTRL_Init+0xac>
30009c3a:	bc70      	pop	{r4, r5, r6}
30009c3c:	4770      	bx	lr
30009c3e:	6955      	ldr	r5, [r2, #20]
30009c40:	207d      	movs	r0, #125	; 0x7d
30009c42:	69d1      	ldr	r1, [r2, #28]
30009c44:	26a0      	movs	r6, #160	; 0xa0
30009c46:	6914      	ldr	r4, [r2, #16]
30009c48:	6992      	ldr	r2, [r2, #24]
30009c4a:	f004 040f 	and.w	r4, r4, #15
30009c4e:	0092      	lsls	r2, r2, #2
30009c50:	fbb1 f1f5 	udiv	r1, r1, r5
30009c54:	fbb0 f5f5 	udiv	r5, r0, r5
30009c58:	f245 5051 	movw	r0, #21841	; 0x5551
30009c5c:	f8c3 0110 	str.w	r0, [r3, #272]	; 0x110
30009c60:	f001 003f 	and.w	r0, r1, #63	; 0x3f
30009c64:	0189      	lsls	r1, r1, #6
30009c66:	ea40 6005 	orr.w	r0, r0, r5, lsl #24
30009c6a:	f641 4570 	movw	r5, #7280	; 0x1c70
30009c6e:	f401 617c 	and.w	r1, r1, #4032	; 0xfc0
30009c72:	4325      	orrs	r5, r4
30009c74:	2420      	movs	r4, #32
30009c76:	4308      	orrs	r0, r1
30009c78:	1e51      	subs	r1, r2, #1
30009c7a:	f8c3 5134 	str.w	r5, [r3, #308]	; 0x134
30009c7e:	3a02      	subs	r2, #2
30009c80:	f440 1080 	orr.w	r0, r0, #1048576	; 0x100000
30009c84:	f3c1 010b 	ubfx	r1, r1, #0, #12
30009c88:	f3c2 020b 	ubfx	r2, r2, #0, #12
30009c8c:	f8c3 0138 	str.w	r0, [r3, #312]	; 0x138
30009c90:	f44f 20a0 	mov.w	r0, #327680	; 0x50000
30009c94:	f8c3 60e0 	str.w	r6, [r3, #224]	; 0xe0
30009c98:	f8c3 40f4 	str.w	r4, [r3, #244]	; 0xf4
30009c9c:	f8c3 011c 	str.w	r0, [r3, #284]	; 0x11c
30009ca0:	f441 20a2 	orr.w	r0, r1, #331776	; 0x51000
30009ca4:	f8c3 0118 	str.w	r0, [r3, #280]	; 0x118
30009ca8:	f8d3 011c 	ldr.w	r0, [r3, #284]	; 0x11c
30009cac:	4301      	orrs	r1, r0
30009cae:	f441 4100 	orr.w	r1, r1, #32768	; 0x8000
30009cb2:	f8c3 111c 	str.w	r1, [r3, #284]	; 0x11c
30009cb6:	f8c3 213c 	str.w	r2, [r3, #316]	; 0x13c
30009cba:	e7ba      	b.n	30009c32 <PSRAM_CTRL_Init+0xaa>
30009cbc:	41012000 	.word	0x41012000
30009cc0:	00bfe000 	.word	0x00bfe000
30009cc4:	3000f8f8 	.word	0x3000f8f8
30009cc8:	00107000 	.word	0x00107000

30009ccc <PSRAM_REG_Write>:
30009ccc:	b430      	push	{r4, r5}
30009cce:	4c20      	ldr	r4, [pc, #128]	; (30009d50 <PSRAM_REG_Write+0x84>)
30009cd0:	6aa3      	ldr	r3, [r4, #40]	; 0x28
30009cd2:	f013 0301 	ands.w	r3, r3, #1
30009cd6:	d1fb      	bne.n	30009cd0 <PSRAM_REG_Write+0x4>
30009cd8:	6825      	ldr	r5, [r4, #0]
30009cda:	f045 4500 	orr.w	r5, r5, #2147483648	; 0x80000000
30009cde:	6025      	str	r5, [r4, #0]
30009ce0:	60a3      	str	r3, [r4, #8]
30009ce2:	6825      	ldr	r5, [r4, #0]
30009ce4:	f425 7540 	bic.w	r5, r5, #768	; 0x300
30009ce8:	6025      	str	r5, [r4, #0]
30009cea:	6063      	str	r3, [r4, #4]
30009cec:	f021 437f 	bic.w	r3, r1, #4278190080	; 0xff000000
30009cf0:	f8c4 3130 	str.w	r3, [r4, #304]	; 0x130
30009cf4:	23c0      	movs	r3, #192	; 0xc0
30009cf6:	f884 3060 	strb.w	r3, [r4, #96]	; 0x60
30009cfa:	f884 3060 	strb.w	r3, [r4, #96]	; 0x60
30009cfe:	0e03      	lsrs	r3, r0, #24
30009d00:	f884 3060 	strb.w	r3, [r4, #96]	; 0x60
30009d04:	f3c0 4307 	ubfx	r3, r0, #16, #8
30009d08:	f884 3060 	strb.w	r3, [r4, #96]	; 0x60
30009d0c:	f3c0 2307 	ubfx	r3, r0, #8, #8
30009d10:	b2c0      	uxtb	r0, r0
30009d12:	f884 3060 	strb.w	r3, [r4, #96]	; 0x60
30009d16:	f884 0060 	strb.w	r0, [r4, #96]	; 0x60
30009d1a:	b141      	cbz	r1, 30009d2e <PSRAM_REG_Write+0x62>
30009d1c:	3a01      	subs	r2, #1
30009d1e:	480c      	ldr	r0, [pc, #48]	; (30009d50 <PSRAM_REG_Write+0x84>)
30009d20:	4411      	add	r1, r2
30009d22:	f812 3f01 	ldrb.w	r3, [r2, #1]!
30009d26:	428a      	cmp	r2, r1
30009d28:	f880 3060 	strb.w	r3, [r0, #96]	; 0x60
30009d2c:	d1f9      	bne.n	30009d22 <PSRAM_REG_Write+0x56>
30009d2e:	4a08      	ldr	r2, [pc, #32]	; (30009d50 <PSRAM_REG_Write+0x84>)
30009d30:	2301      	movs	r3, #1
30009d32:	6093      	str	r3, [r2, #8]
30009d34:	6893      	ldr	r3, [r2, #8]
30009d36:	07d9      	lsls	r1, r3, #31
30009d38:	d4fc      	bmi.n	30009d34 <PSRAM_REG_Write+0x68>
30009d3a:	4a05      	ldr	r2, [pc, #20]	; (30009d50 <PSRAM_REG_Write+0x84>)
30009d3c:	6a93      	ldr	r3, [r2, #40]	; 0x28
30009d3e:	07db      	lsls	r3, r3, #31
30009d40:	d4fc      	bmi.n	30009d3c <PSRAM_REG_Write+0x70>
30009d42:	6813      	ldr	r3, [r2, #0]
30009d44:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
30009d48:	6013      	str	r3, [r2, #0]
30009d4a:	bc30      	pop	{r4, r5}
30009d4c:	4770      	bx	lr
30009d4e:	bf00      	nop
30009d50:	41012000 	.word	0x41012000

30009d54 <PSRAM_APM_DEVIC_Init>:
30009d54:	4b12      	ldr	r3, [pc, #72]	; (30009da0 <PSRAM_APM_DEVIC_Init+0x4c>)
30009d56:	2102      	movs	r1, #2
30009d58:	2000      	movs	r0, #0
30009d5a:	b530      	push	{r4, r5, lr}
30009d5c:	4c11      	ldr	r4, [pc, #68]	; (30009da4 <PSRAM_APM_DEVIC_Init+0x50>)
30009d5e:	b083      	sub	sp, #12
30009d60:	699d      	ldr	r5, [r3, #24]
30009d62:	466a      	mov	r2, sp
30009d64:	eb04 0c05 	add.w	ip, r4, r5
30009d68:	1eeb      	subs	r3, r5, #3
30009d6a:	4c0f      	ldr	r4, [pc, #60]	; (30009da8 <PSRAM_APM_DEVIC_Init+0x54>)
30009d6c:	408b      	lsls	r3, r1
30009d6e:	f81c cc03 	ldrb.w	ip, [ip, #-3]
30009d72:	f003 031c 	and.w	r3, r3, #28
30009d76:	ea4f 1c4c 	mov.w	ip, ip, lsl #5
30009d7a:	f043 0320 	orr.w	r3, r3, #32
30009d7e:	fa5f fc8c 	uxtb.w	ip, ip
30009d82:	f88d 3000 	strb.w	r3, [sp]
30009d86:	f88d c004 	strb.w	ip, [sp, #4]
30009d8a:	f88d 3001 	strb.w	r3, [sp, #1]
30009d8e:	f88d c005 	strb.w	ip, [sp, #5]
30009d92:	47a0      	blx	r4
30009d94:	aa01      	add	r2, sp, #4
30009d96:	2102      	movs	r1, #2
30009d98:	2004      	movs	r0, #4
30009d9a:	47a0      	blx	r4
30009d9c:	b003      	add	sp, #12
30009d9e:	bd30      	pop	{r4, r5, pc}
30009da0:	3000f8f8 	.word	0x3000f8f8
30009da4:	3000ee98 	.word	0x3000ee98
30009da8:	30009ccd 	.word	0x30009ccd

30009dac <PSRAM_WB_REG_Write>:
30009dac:	b430      	push	{r4, r5}
30009dae:	4c1e      	ldr	r4, [pc, #120]	; (30009e28 <PSRAM_WB_REG_Write+0x7c>)
30009db0:	6aa3      	ldr	r3, [r4, #40]	; 0x28
30009db2:	f013 0301 	ands.w	r3, r3, #1
30009db6:	d1fb      	bne.n	30009db0 <PSRAM_WB_REG_Write+0x4>
30009db8:	6825      	ldr	r5, [r4, #0]
30009dba:	b2c0      	uxtb	r0, r0
30009dbc:	f045 4500 	orr.w	r5, r5, #2147483648	; 0x80000000
30009dc0:	6025      	str	r5, [r4, #0]
30009dc2:	60a3      	str	r3, [r4, #8]
30009dc4:	6825      	ldr	r5, [r4, #0]
30009dc6:	f425 7540 	bic.w	r5, r5, #768	; 0x300
30009dca:	6025      	str	r5, [r4, #0]
30009dcc:	f021 457f 	bic.w	r5, r1, #4278190080	; 0xff000000
30009dd0:	6063      	str	r3, [r4, #4]
30009dd2:	f8c4 5130 	str.w	r5, [r4, #304]	; 0x130
30009dd6:	2560      	movs	r5, #96	; 0x60
30009dd8:	f884 5060 	strb.w	r5, [r4, #96]	; 0x60
30009ddc:	2501      	movs	r5, #1
30009dde:	f884 3060 	strb.w	r3, [r4, #96]	; 0x60
30009de2:	f884 5060 	strb.w	r5, [r4, #96]	; 0x60
30009de6:	f884 3060 	strb.w	r3, [r4, #96]	; 0x60
30009dea:	f884 3060 	strb.w	r3, [r4, #96]	; 0x60
30009dee:	f884 0060 	strb.w	r0, [r4, #96]	; 0x60
30009df2:	b141      	cbz	r1, 30009e06 <PSRAM_WB_REG_Write+0x5a>
30009df4:	3a01      	subs	r2, #1
30009df6:	480c      	ldr	r0, [pc, #48]	; (30009e28 <PSRAM_WB_REG_Write+0x7c>)
30009df8:	4411      	add	r1, r2
30009dfa:	f812 3f01 	ldrb.w	r3, [r2, #1]!
30009dfe:	428a      	cmp	r2, r1
30009e00:	f880 3060 	strb.w	r3, [r0, #96]	; 0x60
30009e04:	d1f9      	bne.n	30009dfa <PSRAM_WB_REG_Write+0x4e>
30009e06:	4a08      	ldr	r2, [pc, #32]	; (30009e28 <PSRAM_WB_REG_Write+0x7c>)
30009e08:	2301      	movs	r3, #1
30009e0a:	6093      	str	r3, [r2, #8]
30009e0c:	6893      	ldr	r3, [r2, #8]
30009e0e:	07d9      	lsls	r1, r3, #31
30009e10:	d4fc      	bmi.n	30009e0c <PSRAM_WB_REG_Write+0x60>
30009e12:	4a05      	ldr	r2, [pc, #20]	; (30009e28 <PSRAM_WB_REG_Write+0x7c>)
30009e14:	6a93      	ldr	r3, [r2, #40]	; 0x28
30009e16:	07db      	lsls	r3, r3, #31
30009e18:	d4fc      	bmi.n	30009e14 <PSRAM_WB_REG_Write+0x68>
30009e1a:	6813      	ldr	r3, [r2, #0]
30009e1c:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
30009e20:	6013      	str	r3, [r2, #0]
30009e22:	bc30      	pop	{r4, r5}
30009e24:	4770      	bx	lr
30009e26:	bf00      	nop
30009e28:	41012000 	.word	0x41012000

30009e2c <PSRAM_WB_DEVIC_Init>:
30009e2c:	490b      	ldr	r1, [pc, #44]	; (30009e5c <PSRAM_WB_DEVIC_Init+0x30>)
30009e2e:	4b0c      	ldr	r3, [pc, #48]	; (30009e60 <PSRAM_WB_DEVIC_Init+0x34>)
30009e30:	b510      	push	{r4, lr}
30009e32:	6988      	ldr	r0, [r1, #24]
30009e34:	b082      	sub	sp, #8
30009e36:	248f      	movs	r4, #143	; 0x8f
30009e38:	2102      	movs	r1, #2
30009e3a:	4403      	add	r3, r0
30009e3c:	aa01      	add	r2, sp, #4
30009e3e:	f88d 4004 	strb.w	r4, [sp, #4]
30009e42:	2000      	movs	r0, #0
30009e44:	f813 3c03 	ldrb.w	r3, [r3, #-3]
30009e48:	4c06      	ldr	r4, [pc, #24]	; (30009e64 <PSRAM_WB_DEVIC_Init+0x38>)
30009e4a:	011b      	lsls	r3, r3, #4
30009e4c:	f043 030c 	orr.w	r3, r3, #12
30009e50:	f88d 3005 	strb.w	r3, [sp, #5]
30009e54:	47a0      	blx	r4
30009e56:	b002      	add	sp, #8
30009e58:	bd10      	pop	{r4, pc}
30009e5a:	bf00      	nop
30009e5c:	3000f8f8 	.word	0x3000f8f8
30009e60:	3000eeb8 	.word	0x3000eeb8
30009e64:	30009dad 	.word	0x30009dad

30009e68 <PSRAM_calibration>:
30009e68:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
30009e6c:	f8df 9190 	ldr.w	r9, [pc, #400]	; 3000a000 <PSRAM_calibration+0x198>
30009e70:	b093      	sub	sp, #76	; 0x4c
30009e72:	2700      	movs	r7, #0
30009e74:	4a5b      	ldr	r2, [pc, #364]	; (30009fe4 <PSRAM_calibration+0x17c>)
30009e76:	f8d9 3004 	ldr.w	r3, [r9, #4]
30009e7a:	9711      	str	r7, [sp, #68]	; 0x44
30009e7c:	4c5a      	ldr	r4, [pc, #360]	; (30009fe8 <PSRAM_calibration+0x180>)
30009e7e:	4d5b      	ldr	r5, [pc, #364]	; (30009fec <PSRAM_calibration+0x184>)
30009e80:	e9cd 370b 	strd	r3, r7, [sp, #44]	; 0x2c
30009e84:	4013      	ands	r3, r2
30009e86:	2204      	movs	r2, #4
30009e88:	e9cd 770d 	strd	r7, r7, [sp, #52]	; 0x34
30009e8c:	e9cd 770f 	strd	r7, r7, [sp, #60]	; 0x3c
30009e90:	930a      	str	r3, [sp, #40]	; 0x28
30009e92:	f8d9 3000 	ldr.w	r3, [r9]
30009e96:	f023 0301 	bic.w	r3, r3, #1
30009e9a:	f8c9 3000 	str.w	r3, [r9]
30009e9e:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
30009ea2:	9209      	str	r2, [sp, #36]	; 0x24
30009ea4:	2201      	movs	r2, #1
30009ea6:	461e      	mov	r6, r3
30009ea8:	469b      	mov	fp, r3
30009eaa:	9306      	str	r3, [sp, #24]
30009eac:	9205      	str	r2, [sp, #20]
30009eae:	9b0a      	ldr	r3, [sp, #40]	; 0x28
30009eb0:	9a05      	ldr	r2, [sp, #20]
30009eb2:	9704      	str	r7, [sp, #16]
30009eb4:	2700      	movs	r7, #0
30009eb6:	ea43 7a02 	orr.w	sl, r3, r2, lsl #28
30009eba:	46b8      	mov	r8, r7
30009ebc:	e9cd 6b07 	strd	r6, fp, [sp, #28]
30009ec0:	f04f 3bff 	mov.w	fp, #4294967295	; 0xffffffff
30009ec4:	465e      	mov	r6, fp
30009ec6:	ea4a 0208 	orr.w	r2, sl, r8
30009eca:	4849      	ldr	r0, [pc, #292]	; (30009ff0 <PSRAM_calibration+0x188>)
30009ecc:	a90c      	add	r1, sp, #48	; 0x30
30009ece:	f8c9 2004 	str.w	r2, [r9, #4]
30009ed2:	f04f 42c0 	mov.w	r2, #1610612736	; 0x60000000
30009ed6:	f850 3b04 	ldr.w	r3, [r0], #4
30009eda:	6013      	str	r3, [r2, #0]
30009edc:	6963      	ldr	r3, [r4, #20]
30009ede:	03db      	lsls	r3, r3, #15
30009ee0:	d507      	bpl.n	30009ef2 <PSRAM_calibration+0x8a>
30009ee2:	f3bf 8f4f 	dsb	sy
30009ee6:	f8c4 2270 	str.w	r2, [r4, #624]	; 0x270
30009eea:	f3bf 8f4f 	dsb	sy
30009eee:	f3bf 8f6f 	isb	sy
30009ef2:	6813      	ldr	r3, [r2, #0]
30009ef4:	f502 22a0 	add.w	r2, r2, #327680	; 0x50000
30009ef8:	42aa      	cmp	r2, r5
30009efa:	f841 3b04 	str.w	r3, [r1], #4
30009efe:	d1ea      	bne.n	30009ed6 <PSRAM_calibration+0x6e>
30009f00:	2218      	movs	r2, #24
30009f02:	493b      	ldr	r1, [pc, #236]	; (30009ff0 <PSRAM_calibration+0x188>)
30009f04:	a80c      	add	r0, sp, #48	; 0x30
30009f06:	4b3b      	ldr	r3, [pc, #236]	; (30009ff4 <PSRAM_calibration+0x18c>)
30009f08:	4798      	blx	r3
30009f0a:	b9b0      	cbnz	r0, 30009f3a <PSRAM_calibration+0xd2>
30009f0c:	f1b6 3fff 	cmp.w	r6, #4294967295	; 0xffffffff
30009f10:	bf08      	it	eq
30009f12:	4646      	moveq	r6, r8
30009f14:	f1b8 0f1f 	cmp.w	r8, #31
30009f18:	eb06 0b07 	add.w	fp, r6, r7
30009f1c:	f107 0701 	add.w	r7, r7, #1
30009f20:	d102      	bne.n	30009f28 <PSRAM_calibration+0xc0>
30009f22:	9b04      	ldr	r3, [sp, #16]
30009f24:	429f      	cmp	r7, r3
30009f26:	dc1d      	bgt.n	30009f64 <PSRAM_calibration+0xfc>
30009f28:	f108 0801 	add.w	r8, r8, #1
30009f2c:	f1b8 0f20 	cmp.w	r8, #32
30009f30:	d1c9      	bne.n	30009ec6 <PSRAM_calibration+0x5e>
30009f32:	9f04      	ldr	r7, [sp, #16]
30009f34:	e9dd 6b07 	ldrd	r6, fp, [sp, #28]
30009f38:	e016      	b.n	30009f68 <PSRAM_calibration+0x100>
30009f3a:	1c73      	adds	r3, r6, #1
30009f3c:	d0f4      	beq.n	30009f28 <PSRAM_calibration+0xc0>
30009f3e:	9b04      	ldr	r3, [sp, #16]
30009f40:	42bb      	cmp	r3, r7
30009f42:	da0a      	bge.n	30009f5a <PSRAM_calibration+0xf2>
30009f44:	9b05      	ldr	r3, [sp, #20]
30009f46:	f8cd b020 	str.w	fp, [sp, #32]
30009f4a:	f04f 3bff 	mov.w	fp, #4294967295	; 0xffffffff
30009f4e:	9704      	str	r7, [sp, #16]
30009f50:	2700      	movs	r7, #0
30009f52:	9607      	str	r6, [sp, #28]
30009f54:	465e      	mov	r6, fp
30009f56:	9306      	str	r3, [sp, #24]
30009f58:	e7e6      	b.n	30009f28 <PSRAM_calibration+0xc0>
30009f5a:	f04f 3bff 	mov.w	fp, #4294967295	; 0xffffffff
30009f5e:	2700      	movs	r7, #0
30009f60:	465e      	mov	r6, fp
30009f62:	e7e1      	b.n	30009f28 <PSRAM_calibration+0xc0>
30009f64:	9b05      	ldr	r3, [sp, #20]
30009f66:	9306      	str	r3, [sp, #24]
30009f68:	9b05      	ldr	r3, [sp, #20]
30009f6a:	005b      	lsls	r3, r3, #1
30009f6c:	9305      	str	r3, [sp, #20]
30009f6e:	9b09      	ldr	r3, [sp, #36]	; 0x24
30009f70:	3b01      	subs	r3, #1
30009f72:	9309      	str	r3, [sp, #36]	; 0x24
30009f74:	d19b      	bne.n	30009eae <PSRAM_calibration+0x46>
30009f76:	9b06      	ldr	r3, [sp, #24]
30009f78:	2249      	movs	r2, #73	; 0x49
30009f7a:	491f      	ldr	r1, [pc, #124]	; (30009ff8 <PSRAM_calibration+0x190>)
30009f7c:	2004      	movs	r0, #4
30009f7e:	e9cd 7302 	strd	r7, r3, [sp, #8]
30009f82:	e9cd 6b00 	strd	r6, fp, [sp]
30009f86:	4b1d      	ldr	r3, [pc, #116]	; (30009ffc <PSRAM_calibration+0x194>)
30009f88:	f000 f9c6 	bl	3000a318 <rtk_log_write>
30009f8c:	2f08      	cmp	r7, #8
30009f8e:	dd24      	ble.n	30009fda <PSRAM_calibration+0x172>
30009f90:	ebbb 0306 	subs.w	r3, fp, r6
30009f94:	9806      	ldr	r0, [sp, #24]
30009f96:	bf48      	it	mi
30009f98:	3301      	addmi	r3, #1
30009f9a:	eb1b 0606 	adds.w	r6, fp, r6
30009f9e:	ea4f 0363 	mov.w	r3, r3, asr #1
30009fa2:	bf48      	it	mi
30009fa4:	3601      	addmi	r6, #1
30009fa6:	1e9a      	subs	r2, r3, #2
30009fa8:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
30009faa:	f003 617f 	and.w	r1, r3, #267386880	; 0xff00000
30009fae:	0414      	lsls	r4, r2, #16
30009fb0:	f3c6 0344 	ubfx	r3, r6, #1, #5
30009fb4:	0212      	lsls	r2, r2, #8
30009fb6:	ea41 7100 	orr.w	r1, r1, r0, lsl #28
30009fba:	f404 2470 	and.w	r4, r4, #983040	; 0xf0000
30009fbe:	f402 6270 	and.w	r2, r2, #3840	; 0xf00
30009fc2:	2001      	movs	r0, #1
30009fc4:	430b      	orrs	r3, r1
30009fc6:	490e      	ldr	r1, [pc, #56]	; (3000a000 <PSRAM_calibration+0x198>)
30009fc8:	4323      	orrs	r3, r4
30009fca:	4313      	orrs	r3, r2
30009fcc:	604b      	str	r3, [r1, #4]
30009fce:	680b      	ldr	r3, [r1, #0]
30009fd0:	4303      	orrs	r3, r0
30009fd2:	600b      	str	r3, [r1, #0]
30009fd4:	b013      	add	sp, #76	; 0x4c
30009fd6:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30009fda:	9809      	ldr	r0, [sp, #36]	; 0x24
30009fdc:	b013      	add	sp, #76	; 0x4c
30009fde:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
30009fe2:	bf00      	nop
30009fe4:	0fffffe0 	.word	0x0fffffe0
30009fe8:	e000ed00 	.word	0xe000ed00
30009fec:	601e0000 	.word	0x601e0000
30009ff0:	3000eea0 	.word	0x3000eea0
30009ff4:	000129bd 	.word	0x000129bd
30009ff8:	3000d36c 	.word	0x3000d36c
30009ffc:	3000d374 	.word	0x3000d374
3000a000:	41002000 	.word	0x41002000

3000a004 <PSRAM_AutoGating>:
3000a004:	b1c0      	cbz	r0, 3000a038 <PSRAM_AutoGating+0x34>
3000a006:	f04f 4382 	mov.w	r3, #1090519040	; 0x41000000
3000a00a:	4810      	ldr	r0, [pc, #64]	; (3000a04c <PSRAM_AutoGating+0x48>)
3000a00c:	f3c1 0108 	ubfx	r1, r1, #0, #9
3000a010:	0252      	lsls	r2, r2, #9
3000a012:	b410      	push	{r4}
3000a014:	f8d3 41e4 	ldr.w	r4, [r3, #484]	; 0x1e4
3000a018:	f402 5278 	and.w	r2, r2, #15872	; 0x3e00
3000a01c:	4020      	ands	r0, r4
3000a01e:	4301      	orrs	r1, r0
3000a020:	430a      	orrs	r2, r1
3000a022:	f8c3 21e4 	str.w	r2, [r3, #484]	; 0x1e4
3000a026:	f8d3 21e0 	ldr.w	r2, [r3, #480]	; 0x1e0
3000a02a:	f042 0201 	orr.w	r2, r2, #1
3000a02e:	f8c3 21e0 	str.w	r2, [r3, #480]	; 0x1e0
3000a032:	f85d 4b04 	ldr.w	r4, [sp], #4
3000a036:	4770      	bx	lr
3000a038:	f04f 4282 	mov.w	r2, #1090519040	; 0x41000000
3000a03c:	f8d2 31e0 	ldr.w	r3, [r2, #480]	; 0x1e0
3000a040:	f023 0301 	bic.w	r3, r3, #1
3000a044:	f8c2 31e0 	str.w	r3, [r2, #480]	; 0x1e0
3000a048:	4770      	bx	lr
3000a04a:	bf00      	nop
3000a04c:	ffffc000 	.word	0xffffc000

3000a050 <NAND_CHECK_IS_BAD_BLOCK>:
3000a050:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
3000a054:	4e24      	ldr	r6, [pc, #144]	; (3000a0e8 <NAND_CHECK_IS_BAD_BLOCK+0x98>)
3000a056:	0907      	lsrs	r7, r0, #4
3000a058:	0045      	lsls	r5, r0, #1
3000a05a:	b085      	sub	sp, #20
3000a05c:	f856 3027 	ldr.w	r3, [r6, r7, lsl #2]
3000a060:	f005 051e 	and.w	r5, r5, #30
3000a064:	40eb      	lsrs	r3, r5
3000a066:	f003 0303 	and.w	r3, r3, #3
3000a06a:	2b02      	cmp	r3, #2
3000a06c:	d02c      	beq.n	3000a0c8 <NAND_CHECK_IS_BAD_BLOCK+0x78>
3000a06e:	2b01      	cmp	r3, #1
3000a070:	d026      	beq.n	3000a0c0 <NAND_CHECK_IS_BAD_BLOCK+0x70>
3000a072:	4b1e      	ldr	r3, [pc, #120]	; (3000a0ec <NAND_CHECK_IS_BAD_BLOCK+0x9c>)
3000a074:	f04f 0801 	mov.w	r8, #1
3000a078:	2204      	movs	r2, #4
3000a07a:	4604      	mov	r4, r0
3000a07c:	f893 106c 	ldrb.w	r1, [r3, #108]	; 0x6c
3000a080:	0180      	lsls	r0, r0, #6
3000a082:	ab03      	add	r3, sp, #12
3000a084:	f8df 9070 	ldr.w	r9, [pc, #112]	; 3000a0f8 <NAND_CHECK_IS_BAD_BLOCK+0xa8>
3000a088:	fa08 f101 	lsl.w	r1, r8, r1
3000a08c:	47c8      	blx	r9
3000a08e:	f89d 300c 	ldrb.w	r3, [sp, #12]
3000a092:	f89d 200d 	ldrb.w	r2, [sp, #13]
3000a096:	4013      	ands	r3, r2
3000a098:	2bff      	cmp	r3, #255	; 0xff
3000a09a:	d019      	beq.n	3000a0d0 <NAND_CHECK_IS_BAD_BLOCK+0x80>
3000a09c:	2002      	movs	r0, #2
3000a09e:	f856 2027 	ldr.w	r2, [r6, r7, lsl #2]
3000a0a2:	4b13      	ldr	r3, [pc, #76]	; (3000a0f0 <NAND_CHECK_IS_BAD_BLOCK+0xa0>)
3000a0a4:	fa00 f505 	lsl.w	r5, r0, r5
3000a0a8:	4912      	ldr	r1, [pc, #72]	; (3000a0f4 <NAND_CHECK_IS_BAD_BLOCK+0xa4>)
3000a0aa:	9400      	str	r4, [sp, #0]
3000a0ac:	432a      	orrs	r2, r5
3000a0ae:	f846 2027 	str.w	r2, [r6, r7, lsl #2]
3000a0b2:	2245      	movs	r2, #69	; 0x45
3000a0b4:	f000 f930 	bl	3000a318 <rtk_log_write>
3000a0b8:	4640      	mov	r0, r8
3000a0ba:	b005      	add	sp, #20
3000a0bc:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
3000a0c0:	2000      	movs	r0, #0
3000a0c2:	b005      	add	sp, #20
3000a0c4:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
3000a0c8:	2001      	movs	r0, #1
3000a0ca:	b005      	add	sp, #20
3000a0cc:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
3000a0d0:	f856 3027 	ldr.w	r3, [r6, r7, lsl #2]
3000a0d4:	fa08 f005 	lsl.w	r0, r8, r5
3000a0d8:	4303      	orrs	r3, r0
3000a0da:	2000      	movs	r0, #0
3000a0dc:	f846 3027 	str.w	r3, [r6, r7, lsl #2]
3000a0e0:	b005      	add	sp, #20
3000a0e2:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
3000a0e6:	bf00      	nop
3000a0e8:	3000f928 	.word	0x3000f928
3000a0ec:	2001c01c 	.word	0x2001c01c
3000a0f0:	3000d5f8 	.word	0x3000d5f8
3000a0f4:	3000d60c 	.word	0x3000d60c
3000a0f8:	3000b4bd 	.word	0x3000b4bd

3000a0fc <Nand_Get_NandAddr>:
3000a0fc:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
3000a100:	4f10      	ldr	r7, [pc, #64]	; (3000a144 <Nand_Get_NandAddr+0x48>)
3000a102:	f101 4678 	add.w	r6, r1, #4160749568	; 0xf8000000
3000a106:	f100 4078 	add.w	r0, r0, #4160749568	; 0xf8000000
3000a10a:	f897 506c 	ldrb.w	r5, [r7, #108]	; 0x6c
3000a10e:	40e8      	lsrs	r0, r5
3000a110:	fa26 f505 	lsr.w	r5, r6, r5
3000a114:	09ad      	lsrs	r5, r5, #6
3000a116:	ebb5 1f90 	cmp.w	r5, r0, lsr #6
3000a11a:	d90f      	bls.n	3000a13c <Nand_Get_NandAddr+0x40>
3000a11c:	0984      	lsrs	r4, r0, #6
3000a11e:	f04f 0840 	mov.w	r8, #64	; 0x40
3000a122:	4620      	mov	r0, r4
3000a124:	3401      	adds	r4, #1
3000a126:	f7ff ff93 	bl	3000a050 <NAND_CHECK_IS_BAD_BLOCK>
3000a12a:	b128      	cbz	r0, 3000a138 <Nand_Get_NandAddr+0x3c>
3000a12c:	f897 306c 	ldrb.w	r3, [r7, #108]	; 0x6c
3000a130:	3501      	adds	r5, #1
3000a132:	fa08 f303 	lsl.w	r3, r8, r3
3000a136:	441e      	add	r6, r3
3000a138:	42a5      	cmp	r5, r4
3000a13a:	d8f2      	bhi.n	3000a122 <Nand_Get_NandAddr+0x26>
3000a13c:	4630      	mov	r0, r6
3000a13e:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
3000a142:	bf00      	nop
3000a144:	2001c01c 	.word	0x2001c01c

3000a148 <Nand_L2P_Table>:
3000a148:	b530      	push	{r4, r5, lr}
3000a14a:	b085      	sub	sp, #20
3000a14c:	f04f 6300 	mov.w	r3, #134217728	; 0x8000000
3000a150:	4605      	mov	r5, r0
3000a152:	aa03      	add	r2, sp, #12
3000a154:	a902      	add	r1, sp, #8
3000a156:	e9cd 3302 	strd	r3, r3, [sp, #8]
3000a15a:	f000 f9bf 	bl	3000a4dc <flash_get_layout_info_by_addr>
3000a15e:	4629      	mov	r1, r5
3000a160:	9802      	ldr	r0, [sp, #8]
3000a162:	f7ff ffcb 	bl	3000a0fc <Nand_Get_NandAddr>
3000a166:	9b03      	ldr	r3, [sp, #12]
3000a168:	4604      	mov	r4, r0
3000a16a:	f103 4378 	add.w	r3, r3, #4160749568	; 0xf8000000
3000a16e:	4283      	cmp	r3, r0
3000a170:	d302      	bcc.n	3000a178 <Nand_L2P_Table+0x30>
3000a172:	4620      	mov	r0, r4
3000a174:	b005      	add	sp, #20
3000a176:	bd30      	pop	{r4, r5, pc}
3000a178:	4b04      	ldr	r3, [pc, #16]	; (3000a18c <Nand_L2P_Table+0x44>)
3000a17a:	2245      	movs	r2, #69	; 0x45
3000a17c:	4904      	ldr	r1, [pc, #16]	; (3000a190 <Nand_L2P_Table+0x48>)
3000a17e:	2002      	movs	r0, #2
3000a180:	9500      	str	r5, [sp, #0]
3000a182:	f000 f8c9 	bl	3000a318 <rtk_log_write>
3000a186:	4620      	mov	r0, r4
3000a188:	b005      	add	sp, #20
3000a18a:	bd30      	pop	{r4, r5, pc}
3000a18c:	3000d618 	.word	0x3000d618
3000a190:	3000d60c 	.word	0x3000d60c

3000a194 <NandImgCopy>:
3000a194:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
3000a198:	4617      	mov	r7, r2
3000a19a:	4603      	mov	r3, r0
3000a19c:	b08b      	sub	sp, #44	; 0x2c
3000a19e:	f8df 910c 	ldr.w	r9, [pc, #268]	; 3000a2ac <NandImgCopy+0x118>
3000a1a2:	f107 38ff 	add.w	r8, r7, #4294967295	; 0xffffffff
3000a1a6:	4608      	mov	r0, r1
3000a1a8:	9309      	str	r3, [sp, #36]	; 0x24
3000a1aa:	f7ff ffcd 	bl	3000a148 <Nand_L2P_Table>
3000a1ae:	f899 606c 	ldrb.w	r6, [r9, #108]	; 0x6c
3000a1b2:	4480      	add	r8, r0
3000a1b4:	fa20 f406 	lsr.w	r4, r0, r6
3000a1b8:	fa28 f806 	lsr.w	r8, r8, r6
3000a1bc:	eba8 0804 	sub.w	r8, r8, r4
3000a1c0:	f1b8 3fff 	cmp.w	r8, #4294967295	; 0xffffffff
3000a1c4:	d057      	beq.n	3000a276 <NandImgCopy+0xe2>
3000a1c6:	463b      	mov	r3, r7
3000a1c8:	f04f 0a00 	mov.w	sl, #0
3000a1cc:	4647      	mov	r7, r8
3000a1ce:	4605      	mov	r5, r0
3000a1d0:	f04f 0b40 	mov.w	fp, #64	; 0x40
3000a1d4:	4698      	mov	r8, r3
3000a1d6:	f8cd a020 	str.w	sl, [sp, #32]
3000a1da:	e022      	b.n	3000a222 <NandImgCopy+0x8e>
3000a1dc:	f1c2 0320 	rsb	r3, r2, #32
3000a1e0:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
3000a1e4:	40d9      	lsrs	r1, r3
3000a1e6:	9b09      	ldr	r3, [sp, #36]	; 0x24
3000a1e8:	4029      	ands	r1, r5
3000a1ea:	4453      	add	r3, sl
3000a1ec:	b387      	cbz	r7, 3000a250 <NandImgCopy+0xbc>
3000a1ee:	2601      	movs	r6, #1
3000a1f0:	4620      	mov	r0, r4
3000a1f2:	fa06 f202 	lsl.w	r2, r6, r2
3000a1f6:	4e2a      	ldr	r6, [pc, #168]	; (3000a2a0 <NandImgCopy+0x10c>)
3000a1f8:	9205      	str	r2, [sp, #20]
3000a1fa:	1a52      	subs	r2, r2, r1
3000a1fc:	e9cd 2106 	strd	r2, r1, [sp, #24]
3000a200:	47b0      	blx	r6
3000a202:	f010 0f70 	tst.w	r0, #112	; 0x70
3000a206:	9b05      	ldr	r3, [sp, #20]
3000a208:	e9dd 2106 	ldrd	r2, r1, [sp, #24]
3000a20c:	d136      	bne.n	3000a27c <NandImgCopy+0xe8>
3000a20e:	eba8 0803 	sub.w	r8, r8, r3
3000a212:	4415      	add	r5, r2
3000a214:	4492      	add	sl, r2
3000a216:	4488      	add	r8, r1
3000a218:	f899 606c 	ldrb.w	r6, [r9, #108]	; 0x6c
3000a21c:	3f01      	subs	r7, #1
3000a21e:	fa25 f406 	lsr.w	r4, r5, r6
3000a222:	9b08      	ldr	r3, [sp, #32]
3000a224:	4632      	mov	r2, r6
3000a226:	09a6      	lsrs	r6, r4, #6
3000a228:	ebb3 1f94 	cmp.w	r3, r4, lsr #6
3000a22c:	d8d6      	bhi.n	3000a1dc <NandImgCopy+0x48>
3000a22e:	e005      	b.n	3000a23c <NandImgCopy+0xa8>
3000a230:	f899 306c 	ldrb.w	r3, [r9, #108]	; 0x6c
3000a234:	3440      	adds	r4, #64	; 0x40
3000a236:	fa0b f303 	lsl.w	r3, fp, r3
3000a23a:	441d      	add	r5, r3
3000a23c:	4630      	mov	r0, r6
3000a23e:	3601      	adds	r6, #1
3000a240:	f7ff ff06 	bl	3000a050 <NAND_CHECK_IS_BAD_BLOCK>
3000a244:	2800      	cmp	r0, #0
3000a246:	d1f3      	bne.n	3000a230 <NandImgCopy+0x9c>
3000a248:	f899 206c 	ldrb.w	r2, [r9, #108]	; 0x6c
3000a24c:	9608      	str	r6, [sp, #32]
3000a24e:	e7c5      	b.n	3000a1dc <NandImgCopy+0x48>
3000a250:	4642      	mov	r2, r8
3000a252:	4620      	mov	r0, r4
3000a254:	4d12      	ldr	r5, [pc, #72]	; (3000a2a0 <NandImgCopy+0x10c>)
3000a256:	47a8      	blx	r5
3000a258:	f010 0f70 	tst.w	r0, #112	; 0x70
3000a25c:	d00b      	beq.n	3000a276 <NandImgCopy+0xe2>
3000a25e:	09a1      	lsrs	r1, r4, #6
3000a260:	f004 043f 	and.w	r4, r4, #63	; 0x3f
3000a264:	4b0f      	ldr	r3, [pc, #60]	; (3000a2a4 <NandImgCopy+0x110>)
3000a266:	2249      	movs	r2, #73	; 0x49
3000a268:	9400      	str	r4, [sp, #0]
3000a26a:	e9cd 1001 	strd	r1, r0, [sp, #4]
3000a26e:	490e      	ldr	r1, [pc, #56]	; (3000a2a8 <NandImgCopy+0x114>)
3000a270:	2004      	movs	r0, #4
3000a272:	f000 f851 	bl	3000a318 <rtk_log_write>
3000a276:	b00b      	add	sp, #44	; 0x2c
3000a278:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
3000a27c:	1ac9      	subs	r1, r1, r3
3000a27e:	09a3      	lsrs	r3, r4, #6
3000a280:	f004 043f 	and.w	r4, r4, #63	; 0x3f
3000a284:	4492      	add	sl, r2
3000a286:	4488      	add	r8, r1
3000a288:	4415      	add	r5, r2
3000a28a:	9002      	str	r0, [sp, #8]
3000a28c:	2249      	movs	r2, #73	; 0x49
3000a28e:	9301      	str	r3, [sp, #4]
3000a290:	2004      	movs	r0, #4
3000a292:	4b04      	ldr	r3, [pc, #16]	; (3000a2a4 <NandImgCopy+0x110>)
3000a294:	4904      	ldr	r1, [pc, #16]	; (3000a2a8 <NandImgCopy+0x114>)
3000a296:	9400      	str	r4, [sp, #0]
3000a298:	f000 f83e 	bl	3000a318 <rtk_log_write>
3000a29c:	e7bc      	b.n	3000a218 <NandImgCopy+0x84>
3000a29e:	bf00      	nop
3000a2a0:	3000b4bd 	.word	0x3000b4bd
3000a2a4:	3000d644 	.word	0x3000d644
3000a2a8:	3000d60c 	.word	0x3000d60c
3000a2ac:	2001c01c 	.word	0x2001c01c

3000a2b0 <_init>:
3000a2b0:	4770      	bx	lr
3000a2b2:	bf00      	nop

3000a2b4 <rtk_log_level_get>:
3000a2b4:	4b13      	ldr	r3, [pc, #76]	; (3000a304 <rtk_log_level_get+0x50>)
3000a2b6:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
3000a2ba:	681a      	ldr	r2, [r3, #0]
3000a2bc:	4606      	mov	r6, r0
3000a2be:	2a03      	cmp	r2, #3
3000a2c0:	d917      	bls.n	3000a2f2 <rtk_log_level_get+0x3e>
3000a2c2:	b1d0      	cbz	r0, 3000a2fa <rtk_log_level_get+0x46>
3000a2c4:	2704      	movs	r7, #4
3000a2c6:	4d10      	ldr	r5, [pc, #64]	; (3000a308 <rtk_log_level_get+0x54>)
3000a2c8:	2400      	movs	r4, #0
3000a2ca:	f8df 8048 	ldr.w	r8, [pc, #72]	; 3000a314 <rtk_log_level_get+0x60>
3000a2ce:	e002      	b.n	3000a2d6 <rtk_log_level_get+0x22>
3000a2d0:	3401      	adds	r4, #1
3000a2d2:	42a7      	cmp	r7, r4
3000a2d4:	d911      	bls.n	3000a2fa <rtk_log_level_get+0x46>
3000a2d6:	4628      	mov	r0, r5
3000a2d8:	4631      	mov	r1, r6
3000a2da:	350b      	adds	r5, #11
3000a2dc:	47c0      	blx	r8
3000a2de:	2800      	cmp	r0, #0
3000a2e0:	d1f6      	bne.n	3000a2d0 <rtk_log_level_get+0x1c>
3000a2e2:	eb04 0284 	add.w	r2, r4, r4, lsl #2
3000a2e6:	4b09      	ldr	r3, [pc, #36]	; (3000a30c <rtk_log_level_get+0x58>)
3000a2e8:	eb04 0442 	add.w	r4, r4, r2, lsl #1
3000a2ec:	5d18      	ldrb	r0, [r3, r4]
3000a2ee:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
3000a2f2:	681f      	ldr	r7, [r3, #0]
3000a2f4:	b108      	cbz	r0, 3000a2fa <rtk_log_level_get+0x46>
3000a2f6:	2f00      	cmp	r7, #0
3000a2f8:	d1e5      	bne.n	3000a2c6 <rtk_log_level_get+0x12>
3000a2fa:	4b05      	ldr	r3, [pc, #20]	; (3000a310 <rtk_log_level_get+0x5c>)
3000a2fc:	7818      	ldrb	r0, [r3, #0]
3000a2fe:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
3000a302:	bf00      	nop
3000a304:	3000f930 	.word	0x3000f930
3000a308:	3000f935 	.word	0x3000f935
3000a30c:	3000f934 	.word	0x3000f934
3000a310:	3000eebd 	.word	0x3000eebd
3000a314:	00012c89 	.word	0x00012c89

3000a318 <rtk_log_write>:
3000a318:	b1b9      	cbz	r1, 3000a34a <rtk_log_write+0x32>
3000a31a:	b408      	push	{r3}
3000a31c:	b570      	push	{r4, r5, r6, lr}
3000a31e:	4605      	mov	r5, r0
3000a320:	b083      	sub	sp, #12
3000a322:	4608      	mov	r0, r1
3000a324:	460c      	mov	r4, r1
3000a326:	4616      	mov	r6, r2
3000a328:	f7ff ffc4 	bl	3000a2b4 <rtk_log_level_get>
3000a32c:	42a8      	cmp	r0, r5
3000a32e:	d307      	bcc.n	3000a340 <rtk_log_write+0x28>
3000a330:	7823      	ldrb	r3, [r4, #0]
3000a332:	2b23      	cmp	r3, #35	; 0x23
3000a334:	d10a      	bne.n	3000a34c <rtk_log_write+0x34>
3000a336:	a908      	add	r1, sp, #32
3000a338:	9807      	ldr	r0, [sp, #28]
3000a33a:	9101      	str	r1, [sp, #4]
3000a33c:	f001 f90c 	bl	3000b558 <DiagVprintf>
3000a340:	b003      	add	sp, #12
3000a342:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
3000a346:	b001      	add	sp, #4
3000a348:	4770      	bx	lr
3000a34a:	4770      	bx	lr
3000a34c:	4621      	mov	r1, r4
3000a34e:	4632      	mov	r2, r6
3000a350:	4804      	ldr	r0, [pc, #16]	; (3000a364 <rtk_log_write+0x4c>)
3000a352:	f001 fbc1 	bl	3000bad8 <DiagPrintf>
3000a356:	a908      	add	r1, sp, #32
3000a358:	9807      	ldr	r0, [sp, #28]
3000a35a:	9101      	str	r1, [sp, #4]
3000a35c:	f001 f8fc 	bl	3000b558 <DiagVprintf>
3000a360:	e7ee      	b.n	3000a340 <rtk_log_write+0x28>
3000a362:	bf00      	nop
3000a364:	3000d67c 	.word	0x3000d67c

3000a368 <rtk_log_write_nano>:
3000a368:	b1b9      	cbz	r1, 3000a39a <rtk_log_write_nano+0x32>
3000a36a:	b408      	push	{r3}
3000a36c:	b570      	push	{r4, r5, r6, lr}
3000a36e:	4605      	mov	r5, r0
3000a370:	b083      	sub	sp, #12
3000a372:	4608      	mov	r0, r1
3000a374:	460c      	mov	r4, r1
3000a376:	4616      	mov	r6, r2
3000a378:	f7ff ff9c 	bl	3000a2b4 <rtk_log_level_get>
3000a37c:	42a8      	cmp	r0, r5
3000a37e:	d307      	bcc.n	3000a390 <rtk_log_write_nano+0x28>
3000a380:	7823      	ldrb	r3, [r4, #0]
3000a382:	2b23      	cmp	r3, #35	; 0x23
3000a384:	d10a      	bne.n	3000a39c <rtk_log_write_nano+0x34>
3000a386:	a908      	add	r1, sp, #32
3000a388:	9807      	ldr	r0, [sp, #28]
3000a38a:	9101      	str	r1, [sp, #4]
3000a38c:	f001 fbb2 	bl	3000baf4 <DiagVprintfNano>
3000a390:	b003      	add	sp, #12
3000a392:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
3000a396:	b001      	add	sp, #4
3000a398:	4770      	bx	lr
3000a39a:	4770      	bx	lr
3000a39c:	4621      	mov	r1, r4
3000a39e:	4632      	mov	r2, r6
3000a3a0:	4804      	ldr	r0, [pc, #16]	; (3000a3b4 <rtk_log_write_nano+0x4c>)
3000a3a2:	f001 fd8f 	bl	3000bec4 <DiagPrintfNano>
3000a3a6:	a908      	add	r1, sp, #32
3000a3a8:	9807      	ldr	r0, [sp, #28]
3000a3aa:	9101      	str	r1, [sp, #4]
3000a3ac:	f001 fba2 	bl	3000baf4 <DiagVprintfNano>
3000a3b0:	e7ee      	b.n	3000a390 <rtk_log_write_nano+0x28>
3000a3b2:	bf00      	nop
3000a3b4:	3000d67c 	.word	0x3000d67c

3000a3b8 <flash_init_userdef>:
3000a3b8:	4b14      	ldr	r3, [pc, #80]	; (3000a40c <flash_init_userdef+0x54>)
3000a3ba:	2201      	movs	r2, #1
3000a3bc:	2140      	movs	r1, #64	; 0x40
3000a3be:	f44f 6000 	mov.w	r0, #2048	; 0x800
3000a3c2:	b430      	push	{r4, r5}
3000a3c4:	e9c3 1203 	strd	r1, r2, [r3, #12]
3000a3c8:	2502      	movs	r5, #2
3000a3ca:	2400      	movs	r4, #0
3000a3cc:	f640 0104 	movw	r1, #2052	; 0x804
3000a3d0:	601a      	str	r2, [r3, #0]
3000a3d2:	22bb      	movs	r2, #187	; 0xbb
3000a3d4:	83d8      	strh	r0, [r3, #30]
3000a3d6:	8419      	strh	r1, [r3, #32]
3000a3d8:	203b      	movs	r0, #59	; 0x3b
3000a3da:	21eb      	movs	r1, #235	; 0xeb
3000a3dc:	629a      	str	r2, [r3, #40]	; 0x28
3000a3de:	4a0c      	ldr	r2, [pc, #48]	; (3000a410 <flash_init_userdef+0x58>)
3000a3e0:	6258      	str	r0, [r3, #36]	; 0x24
3000a3e2:	6319      	str	r1, [r3, #48]	; 0x30
3000a3e4:	f64a 3120 	movw	r1, #43808	; 0xab20
3000a3e8:	655a      	str	r2, [r3, #84]	; 0x54
3000a3ea:	22b9      	movs	r2, #185	; 0xb9
3000a3ec:	4809      	ldr	r0, [pc, #36]	; (3000a414 <flash_init_userdef+0x5c>)
3000a3ee:	f8a3 105c 	strh.w	r1, [r3, #92]	; 0x5c
3000a3f2:	6598      	str	r0, [r3, #88]	; 0x58
3000a3f4:	f883 205e 	strb.w	r2, [r3, #94]	; 0x5e
3000a3f8:	e9c3 5405 	strd	r5, r4, [r3, #20]
3000a3fc:	2406      	movs	r4, #6
3000a3fe:	f883 4022 	strb.w	r4, [r3, #34]	; 0x22
3000a402:	246b      	movs	r4, #107	; 0x6b
3000a404:	62dc      	str	r4, [r3, #44]	; 0x2c
3000a406:	bc30      	pop	{r4, r5}
3000a408:	4770      	bx	lr
3000a40a:	bf00      	nop
3000a40c:	2001c01c 	.word	0x2001c01c
3000a410:	00059f06 	.word	0x00059f06
3000a414:	d8600001 	.word	0xd8600001

3000a418 <nand_init_userdef>:
3000a418:	4b18      	ldr	r3, [pc, #96]	; (3000a47c <nand_init_userdef+0x64>)
3000a41a:	2001      	movs	r0, #1
3000a41c:	2200      	movs	r2, #0
3000a41e:	2102      	movs	r1, #2
3000a420:	b410      	push	{r4}
3000a422:	6018      	str	r0, [r3, #0]
3000a424:	f640 0408 	movw	r4, #2056	; 0x808
3000a428:	6159      	str	r1, [r3, #20]
3000a42a:	83dc      	strh	r4, [r3, #30]
3000a42c:	f44f 6400 	mov.w	r4, #2048	; 0x800
3000a430:	f8a3 1062 	strh.w	r1, [r3, #98]	; 0x62
3000a434:	841c      	strh	r4, [r3, #32]
3000a436:	246b      	movs	r4, #107	; 0x6b
3000a438:	619a      	str	r2, [r3, #24]
3000a43a:	62dc      	str	r4, [r3, #44]	; 0x2c
3000a43c:	4c10      	ldr	r4, [pc, #64]	; (3000a480 <nand_init_userdef+0x68>)
3000a43e:	775a      	strb	r2, [r3, #29]
3000a440:	f883 2022 	strb.w	r2, [r3, #34]	; 0x22
3000a444:	629a      	str	r2, [r3, #40]	; 0x28
3000a446:	631a      	str	r2, [r3, #48]	; 0x30
3000a448:	f883 2061 	strb.w	r2, [r3, #97]	; 0x61
3000a44c:	65da      	str	r2, [r3, #92]	; 0x5c
3000a44e:	e9c3 0003 	strd	r0, r0, [r3, #12]
3000a452:	203b      	movs	r0, #59	; 0x3b
3000a454:	6258      	str	r0, [r3, #36]	; 0x24
3000a456:	200b      	movs	r0, #11
3000a458:	e9c3 1211 	strd	r1, r2, [r3, #68]	; 0x44
3000a45c:	6698      	str	r0, [r3, #104]	; 0x68
3000a45e:	2008      	movs	r0, #8
3000a460:	4908      	ldr	r1, [pc, #32]	; (3000a484 <nand_init_userdef+0x6c>)
3000a462:	64d8      	str	r0, [r3, #76]	; 0x4c
3000a464:	f241 0013 	movw	r0, #4115	; 0x1013
3000a468:	6599      	str	r1, [r3, #88]	; 0x58
3000a46a:	4907      	ldr	r1, [pc, #28]	; (3000a488 <nand_init_userdef+0x70>)
3000a46c:	f8a3 006e 	strh.w	r0, [r3, #110]	; 0x6e
3000a470:	6719      	str	r1, [r3, #112]	; 0x70
3000a472:	e9c3 2414 	strd	r2, r4, [r3, #80]	; 0x50
3000a476:	f85d 4b04 	ldr.w	r4, [sp], #4
3000a47a:	4770      	bx	lr
3000a47c:	2001c01c 	.word	0x2001c01c
3000a480:	000f9f06 	.word	0x000f9f06
3000a484:	d800001f 	.word	0xd800001f
3000a488:	10300804 	.word	0x10300804

3000a48c <flash_get_layout_info>:
3000a48c:	b530      	push	{r4, r5, lr}
3000a48e:	4d10      	ldr	r5, [pc, #64]	; (3000a4d0 <flash_get_layout_info+0x44>)
3000a490:	b083      	sub	sp, #12
3000a492:	4604      	mov	r4, r0
3000a494:	682b      	ldr	r3, [r5, #0]
3000a496:	2bff      	cmp	r3, #255	; 0xff
3000a498:	d011      	beq.n	3000a4be <flash_get_layout_info+0x32>
3000a49a:	46ac      	mov	ip, r5
3000a49c:	e003      	b.n	3000a4a6 <flash_get_layout_info+0x1a>
3000a49e:	f85c 3f0c 	ldr.w	r3, [ip, #12]!
3000a4a2:	2bff      	cmp	r3, #255	; 0xff
3000a4a4:	d00b      	beq.n	3000a4be <flash_get_layout_info+0x32>
3000a4a6:	429c      	cmp	r4, r3
3000a4a8:	d1f9      	bne.n	3000a49e <flash_get_layout_info+0x12>
3000a4aa:	b111      	cbz	r1, 3000a4b2 <flash_get_layout_info+0x26>
3000a4ac:	f8dc 3004 	ldr.w	r3, [ip, #4]
3000a4b0:	600b      	str	r3, [r1, #0]
3000a4b2:	b15a      	cbz	r2, 3000a4cc <flash_get_layout_info+0x40>
3000a4b4:	f8dc 3008 	ldr.w	r3, [ip, #8]
3000a4b8:	6013      	str	r3, [r2, #0]
3000a4ba:	b003      	add	sp, #12
3000a4bc:	bd30      	pop	{r4, r5, pc}
3000a4be:	4b05      	ldr	r3, [pc, #20]	; (3000a4d4 <flash_get_layout_info+0x48>)
3000a4c0:	2245      	movs	r2, #69	; 0x45
3000a4c2:	4905      	ldr	r1, [pc, #20]	; (3000a4d8 <flash_get_layout_info+0x4c>)
3000a4c4:	2002      	movs	r0, #2
3000a4c6:	9400      	str	r4, [sp, #0]
3000a4c8:	f7ff ff26 	bl	3000a318 <rtk_log_write>
3000a4cc:	b003      	add	sp, #12
3000a4ce:	bd30      	pop	{r4, r5, pc}
3000a4d0:	3000f058 	.word	0x3000f058
3000a4d4:	3000d7a8 	.word	0x3000d7a8
3000a4d8:	3000d610 	.word	0x3000d610

3000a4dc <flash_get_layout_info_by_addr>:
3000a4dc:	4b0f      	ldr	r3, [pc, #60]	; (3000a51c <flash_get_layout_info_by_addr+0x40>)
3000a4de:	b530      	push	{r4, r5, lr}
3000a4e0:	681c      	ldr	r4, [r3, #0]
3000a4e2:	b083      	sub	sp, #12
3000a4e4:	4605      	mov	r5, r0
3000a4e6:	2cff      	cmp	r4, #255	; 0xff
3000a4e8:	d009      	beq.n	3000a4fe <flash_get_layout_info_by_addr+0x22>
3000a4ea:	685c      	ldr	r4, [r3, #4]
3000a4ec:	42ac      	cmp	r4, r5
3000a4ee:	d802      	bhi.n	3000a4f6 <flash_get_layout_info_by_addr+0x1a>
3000a4f0:	6898      	ldr	r0, [r3, #8]
3000a4f2:	42a8      	cmp	r0, r5
3000a4f4:	d20c      	bcs.n	3000a510 <flash_get_layout_info_by_addr+0x34>
3000a4f6:	f853 4f0c 	ldr.w	r4, [r3, #12]!
3000a4fa:	2cff      	cmp	r4, #255	; 0xff
3000a4fc:	d1f5      	bne.n	3000a4ea <flash_get_layout_info_by_addr+0xe>
3000a4fe:	4b08      	ldr	r3, [pc, #32]	; (3000a520 <flash_get_layout_info_by_addr+0x44>)
3000a500:	2245      	movs	r2, #69	; 0x45
3000a502:	4908      	ldr	r1, [pc, #32]	; (3000a524 <flash_get_layout_info_by_addr+0x48>)
3000a504:	2002      	movs	r0, #2
3000a506:	9500      	str	r5, [sp, #0]
3000a508:	f7ff ff06 	bl	3000a318 <rtk_log_write>
3000a50c:	b003      	add	sp, #12
3000a50e:	bd30      	pop	{r4, r5, pc}
3000a510:	600c      	str	r4, [r1, #0]
3000a512:	689b      	ldr	r3, [r3, #8]
3000a514:	6013      	str	r3, [r2, #0]
3000a516:	b003      	add	sp, #12
3000a518:	bd30      	pop	{r4, r5, pc}
3000a51a:	bf00      	nop
3000a51c:	3000f058 	.word	0x3000f058
3000a520:	3000d7d0 	.word	0x3000d7d0
3000a524:	3000d610 	.word	0x3000d610

3000a528 <TaskExitError>:
3000a528:	b672      	cpsid	i
3000a52a:	e7fe      	b.n	3000a52a <TaskExitError+0x2>

3000a52c <thumb_backtrace_from_stack>:
3000a52c:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
3000a530:	b085      	sub	sp, #20
3000a532:	4b7d      	ldr	r3, [pc, #500]	; (3000a728 <thumb_backtrace_from_stack+0x1fc>)
3000a534:	9203      	str	r2, [sp, #12]
3000a536:	f023 0301 	bic.w	r3, r3, #1
3000a53a:	680a      	ldr	r2, [r1, #0]
3000a53c:	f022 0901 	bic.w	r9, r2, #1
3000a540:	4599      	cmp	r9, r3
3000a542:	e9cd 3001 	strd	r3, r0, [sp, #4]
3000a546:	f000 80e3 	beq.w	3000a710 <thumb_backtrace_from_stack+0x1e4>
3000a54a:	6803      	ldr	r3, [r0, #0]
3000a54c:	460e      	mov	r6, r1
3000a54e:	2502      	movs	r5, #2
3000a550:	f8df e200 	ldr.w	lr, [pc, #512]	; 3000a754 <thumb_backtrace_from_stack+0x228>
3000a554:	4a75      	ldr	r2, [pc, #468]	; (3000a72c <thumb_backtrace_from_stack+0x200>)
3000a556:	9300      	str	r3, [sp, #0]
3000a558:	eba9 0405 	sub.w	r4, r9, r5
3000a55c:	f834 1c02 	ldrh.w	r1, [r4, #-2]
3000a560:	8820      	ldrh	r0, [r4, #0]
3000a562:	0acb      	lsrs	r3, r1, #11
3000a564:	f400 477f 	and.w	r7, r0, #65280	; 0xff00
3000a568:	3b1d      	subs	r3, #29
3000a56a:	2b02      	cmp	r3, #2
3000a56c:	d93a      	bls.n	3000a5e4 <thumb_backtrace_from_stack+0xb8>
3000a56e:	f5b7 4f35 	cmp.w	r7, #46336	; 0xb500
3000a572:	f000 80b3 	beq.w	3000a6dc <thumb_backtrace_from_stack+0x1b0>
3000a576:	f5b7 4f34 	cmp.w	r7, #46080	; 0xb400
3000a57a:	d179      	bne.n	3000a670 <thumb_backtrace_from_stack+0x144>
3000a57c:	b2c0      	uxtb	r0, r0
3000a57e:	f001 ff95 	bl	3000c4ac <__popcountsi2>
3000a582:	1ca3      	adds	r3, r4, #2
3000a584:	4683      	mov	fp, r0
3000a586:	4599      	cmp	r9, r3
3000a588:	d325      	bcc.n	3000a5d6 <thumb_backtrace_from_stack+0xaa>
3000a58a:	f04f 0a00 	mov.w	sl, #0
3000a58e:	2502      	movs	r5, #2
3000a590:	f8df 81c0 	ldr.w	r8, [pc, #448]	; 3000a754 <thumb_backtrace_from_stack+0x228>
3000a594:	4f65      	ldr	r7, [pc, #404]	; (3000a72c <thumb_backtrace_from_stack+0x200>)
3000a596:	46ac      	mov	ip, r5
3000a598:	8818      	ldrh	r0, [r3, #0]
3000a59a:	44a4      	add	ip, r4
3000a59c:	0ac3      	lsrs	r3, r0, #11
3000a59e:	f8bc 2002 	ldrh.w	r2, [ip, #2]
3000a5a2:	3b1d      	subs	r3, #29
3000a5a4:	2b02      	cmp	r3, #2
3000a5a6:	d970      	bls.n	3000a68a <thumb_backtrace_from_stack+0x15e>
3000a5a8:	f400 437f 	and.w	r3, r0, #65280	; 0xff00
3000a5ac:	f5b3 4f35 	cmp.w	r3, #46336	; 0xb500
3000a5b0:	f000 80a6 	beq.w	3000a700 <thumb_backtrace_from_stack+0x1d4>
3000a5b4:	f020 037f 	bic.w	r3, r0, #127	; 0x7f
3000a5b8:	f24b 0280 	movw	r2, #45184	; 0xb080
3000a5bc:	4293      	cmp	r3, r2
3000a5be:	d102      	bne.n	3000a5c6 <thumb_backtrace_from_stack+0x9a>
3000a5c0:	f000 007f 	and.w	r0, r0, #127	; 0x7f
3000a5c4:	4483      	add	fp, r0
3000a5c6:	3502      	adds	r5, #2
3000a5c8:	1963      	adds	r3, r4, r5
3000a5ca:	46ac      	mov	ip, r5
3000a5cc:	454b      	cmp	r3, r9
3000a5ce:	d9e3      	bls.n	3000a598 <thumb_backtrace_from_stack+0x6c>
3000a5d0:	f1ba 0f00 	cmp.w	sl, #0
3000a5d4:	d117      	bne.n	3000a606 <thumb_backtrace_from_stack+0xda>
3000a5d6:	9b03      	ldr	r3, [sp, #12]
3000a5d8:	6819      	ldr	r1, [r3, #0]
3000a5da:	2900      	cmp	r1, #0
3000a5dc:	f000 809e 	beq.w	3000a71c <thumb_backtrace_from_stack+0x1f0>
3000a5e0:	460b      	mov	r3, r1
3000a5e2:	e016      	b.n	3000a612 <thumb_backtrace_from_stack+0xe6>
3000a5e4:	ea40 4001 	orr.w	r0, r0, r1, lsl #16
3000a5e8:	ea00 030e 	and.w	r3, r0, lr
3000a5ec:	4293      	cmp	r3, r2
3000a5ee:	d13e      	bne.n	3000a66e <thumb_backtrace_from_stack+0x142>
3000a5f0:	f3c0 000b 	ubfx	r0, r0, #0, #12
3000a5f4:	f04f 0a01 	mov.w	sl, #1
3000a5f8:	f001 ff58 	bl	3000c4ac <__popcountsi2>
3000a5fc:	1ca3      	adds	r3, r4, #2
3000a5fe:	f100 0b01 	add.w	fp, r0, #1
3000a602:	4599      	cmp	r9, r3
3000a604:	d2c3      	bcs.n	3000a58e <thumb_backtrace_from_stack+0x62>
3000a606:	ebab 010a 	sub.w	r1, fp, sl
3000a60a:	9b00      	ldr	r3, [sp, #0]
3000a60c:	f853 3021 	ldr.w	r3, [r3, r1, lsl #2]
3000a610:	4619      	mov	r1, r3
3000a612:	9a00      	ldr	r2, [sp, #0]
3000a614:	f023 0301 	bic.w	r3, r3, #1
3000a618:	eb02 008b 	add.w	r0, r2, fp, lsl #2
3000a61c:	9a02      	ldr	r2, [sp, #8]
3000a61e:	6010      	str	r0, [r2, #0]
3000a620:	9a01      	ldr	r2, [sp, #4]
3000a622:	429a      	cmp	r2, r3
3000a624:	d078      	beq.n	3000a718 <thumb_backtrace_from_stack+0x1ec>
3000a626:	f833 2c02 	ldrh.w	r2, [r3, #-2]
3000a62a:	f402 427f 	and.w	r2, r2, #65280	; 0xff00
3000a62e:	f5b2 4f8e 	cmp.w	r2, #18176	; 0x4700
3000a632:	d016      	beq.n	3000a662 <thumb_backtrace_from_stack+0x136>
3000a634:	f853 2c04 	ldr.w	r2, [r3, #-4]
3000a638:	483d      	ldr	r0, [pc, #244]	; (3000a730 <thumb_backtrace_from_stack+0x204>)
3000a63a:	4c3e      	ldr	r4, [pc, #248]	; (3000a734 <thumb_backtrace_from_stack+0x208>)
3000a63c:	4010      	ands	r0, r2
3000a63e:	42a0      	cmp	r0, r4
3000a640:	d063      	beq.n	3000a70a <thumb_backtrace_from_stack+0x1de>
3000a642:	f002 407e 	and.w	r0, r2, #4261412864	; 0xfe000000
3000a646:	f1b0 4f7a 	cmp.w	r0, #4194304000	; 0xfa000000
3000a64a:	d05e      	beq.n	3000a70a <thumb_backtrace_from_stack+0x1de>
3000a64c:	483a      	ldr	r0, [pc, #232]	; (3000a738 <thumb_backtrace_from_stack+0x20c>)
3000a64e:	4002      	ands	r2, r0
3000a650:	483a      	ldr	r0, [pc, #232]	; (3000a73c <thumb_backtrace_from_stack+0x210>)
3000a652:	4282      	cmp	r2, r0
3000a654:	d059      	beq.n	3000a70a <thumb_backtrace_from_stack+0x1de>
3000a656:	f833 3c04 	ldrh.w	r3, [r3, #-4]
3000a65a:	0adb      	lsrs	r3, r3, #11
3000a65c:	3b1d      	subs	r3, #29
3000a65e:	2b02      	cmp	r3, #2
3000a660:	d953      	bls.n	3000a70a <thumb_backtrace_from_stack+0x1de>
3000a662:	3902      	subs	r1, #2
3000a664:	2000      	movs	r0, #0
3000a666:	6031      	str	r1, [r6, #0]
3000a668:	b005      	add	sp, #20
3000a66a:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
3000a66e:	3502      	adds	r5, #2
3000a670:	3502      	adds	r5, #2
3000a672:	f5b5 5f00 	cmp.w	r5, #8192	; 0x2000
3000a676:	f6ff af6f 	blt.w	3000a558 <thumb_backtrace_from_stack+0x2c>
3000a67a:	1ca3      	adds	r3, r4, #2
3000a67c:	4599      	cmp	r9, r3
3000a67e:	d34f      	bcc.n	3000a720 <thumb_backtrace_from_stack+0x1f4>
3000a680:	f04f 0a00 	mov.w	sl, #0
3000a684:	f04f 3bff 	mov.w	fp, #4294967295	; 0xffffffff
3000a688:	e781      	b.n	3000a58e <thumb_backtrace_from_stack+0x62>
3000a68a:	ea42 4000 	orr.w	r0, r2, r0, lsl #16
3000a68e:	3502      	adds	r5, #2
3000a690:	ea00 0308 	and.w	r3, r0, r8
3000a694:	42bb      	cmp	r3, r7
3000a696:	d02d      	beq.n	3000a6f4 <thumb_backtrace_from_stack+0x1c8>
3000a698:	4b29      	ldr	r3, [pc, #164]	; (3000a740 <thumb_backtrace_from_stack+0x214>)
3000a69a:	4a2a      	ldr	r2, [pc, #168]	; (3000a744 <thumb_backtrace_from_stack+0x218>)
3000a69c:	4003      	ands	r3, r0
3000a69e:	4293      	cmp	r3, r2
3000a6a0:	d009      	beq.n	3000a6b6 <thumb_backtrace_from_stack+0x18a>
3000a6a2:	4b29      	ldr	r3, [pc, #164]	; (3000a748 <thumb_backtrace_from_stack+0x21c>)
3000a6a4:	4a29      	ldr	r2, [pc, #164]	; (3000a74c <thumb_backtrace_from_stack+0x220>)
3000a6a6:	4003      	ands	r3, r0
3000a6a8:	4293      	cmp	r3, r2
3000a6aa:	d002      	beq.n	3000a6b2 <thumb_backtrace_from_stack+0x186>
3000a6ac:	4a28      	ldr	r2, [pc, #160]	; (3000a750 <thumb_backtrace_from_stack+0x224>)
3000a6ae:	4293      	cmp	r3, r2
3000a6b0:	d189      	bne.n	3000a5c6 <thumb_backtrace_from_stack+0x9a>
3000a6b2:	b2c0      	uxtb	r0, r0
3000a6b4:	e786      	b.n	3000a5c4 <thumb_backtrace_from_stack+0x98>
3000a6b6:	0d83      	lsrs	r3, r0, #22
3000a6b8:	f3c0 11c0 	ubfx	r1, r0, #7, #1
3000a6bc:	0ac2      	lsrs	r2, r0, #11
3000a6be:	f000 007f 	and.w	r0, r0, #127	; 0x7f
3000a6c2:	f003 0310 	and.w	r3, r3, #16
3000a6c6:	f002 020e 	and.w	r2, r2, #14
3000a6ca:	3080      	adds	r0, #128	; 0x80
3000a6cc:	f1c3 031e 	rsb	r3, r3, #30
3000a6d0:	430a      	orrs	r2, r1
3000a6d2:	1a9b      	subs	r3, r3, r2
3000a6d4:	4098      	lsls	r0, r3
3000a6d6:	f53f af76 	bmi.w	3000a5c6 <thumb_backtrace_from_stack+0x9a>
3000a6da:	e773      	b.n	3000a5c4 <thumb_backtrace_from_stack+0x98>
3000a6dc:	b2c0      	uxtb	r0, r0
3000a6de:	f04f 0a01 	mov.w	sl, #1
3000a6e2:	f001 fee3 	bl	3000c4ac <__popcountsi2>
3000a6e6:	1ca3      	adds	r3, r4, #2
3000a6e8:	f100 0b01 	add.w	fp, r0, #1
3000a6ec:	4599      	cmp	r9, r3
3000a6ee:	f4bf af4e 	bcs.w	3000a58e <thumb_backtrace_from_stack+0x62>
3000a6f2:	e788      	b.n	3000a606 <thumb_backtrace_from_stack+0xda>
3000a6f4:	f3c0 000b 	ubfx	r0, r0, #0, #12
3000a6f8:	f001 fed8 	bl	3000c4ac <__popcountsi2>
3000a6fc:	3001      	adds	r0, #1
3000a6fe:	e761      	b.n	3000a5c4 <thumb_backtrace_from_stack+0x98>
3000a700:	b2c0      	uxtb	r0, r0
3000a702:	f001 fed3 	bl	3000c4ac <__popcountsi2>
3000a706:	3001      	adds	r0, #1
3000a708:	e75c      	b.n	3000a5c4 <thumb_backtrace_from_stack+0x98>
3000a70a:	3904      	subs	r1, #4
3000a70c:	2000      	movs	r0, #0
3000a70e:	e7aa      	b.n	3000a666 <thumb_backtrace_from_stack+0x13a>
3000a710:	2001      	movs	r0, #1
3000a712:	b005      	add	sp, #20
3000a714:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
3000a718:	2001      	movs	r0, #1
3000a71a:	e7a4      	b.n	3000a666 <thumb_backtrace_from_stack+0x13a>
3000a71c:	468a      	mov	sl, r1
3000a71e:	e772      	b.n	3000a606 <thumb_backtrace_from_stack+0xda>
3000a720:	f04f 3bff 	mov.w	fp, #4294967295	; 0xffffffff
3000a724:	e757      	b.n	3000a5d6 <thumb_backtrace_from_stack+0xaa>
3000a726:	bf00      	nop
3000a728:	3000a529 	.word	0x3000a529
3000a72c:	e92d4000 	.word	0xe92d4000
3000a730:	0ffffff0 	.word	0x0ffffff0
3000a734:	012fff10 	.word	0x012fff10
3000a738:	f800d000 	.word	0xf800d000
3000a73c:	f000c000 	.word	0xf000c000
3000a740:	fbff8f00 	.word	0xfbff8f00
3000a744:	f1ad0d00 	.word	0xf1ad0d00
3000a748:	ffbf0f00 	.word	0xffbf0f00
3000a74c:	ed2d0b00 	.word	0xed2d0b00
3000a750:	ed2d0a00 	.word	0xed2d0a00
3000a754:	fffff000 	.word	0xfffff000

3000a758 <do_stack_backtrace>:
3000a758:	b570      	push	{r4, r5, r6, lr}
3000a75a:	4d22      	ldr	r5, [pc, #136]	; (3000a7e4 <do_stack_backtrace+0x8c>)
3000a75c:	b084      	sub	sp, #16
3000a75e:	682b      	ldr	r3, [r5, #0]
3000a760:	b90b      	cbnz	r3, 3000a766 <do_stack_backtrace+0xe>
3000a762:	2308      	movs	r3, #8
3000a764:	602b      	str	r3, [r5, #0]
3000a766:	2800      	cmp	r0, #0
3000a768:	d032      	beq.n	3000a7d0 <do_stack_backtrace+0x78>
3000a76a:	2900      	cmp	r1, #0
3000a76c:	d030      	beq.n	3000a7d0 <do_stack_backtrace+0x78>
3000a76e:	9001      	str	r0, [sp, #4]
3000a770:	06d2      	lsls	r2, r2, #27
3000a772:	69cc      	ldr	r4, [r1, #28]
3000a774:	694b      	ldr	r3, [r1, #20]
3000a776:	4602      	mov	r2, r0
3000a778:	bf4c      	ite	mi
3000a77a:	3120      	addmi	r1, #32
3000a77c:	3168      	addpl	r1, #104	; 0x68
3000a77e:	f3c4 0408 	ubfx	r4, r4, #0, #9
3000a782:	4819      	ldr	r0, [pc, #100]	; (3000a7e8 <do_stack_backtrace+0x90>)
3000a784:	9303      	str	r3, [sp, #12]
3000a786:	9102      	str	r1, [sp, #8]
3000a788:	f001 fb9c 	bl	3000bec4 <DiagPrintfNano>
3000a78c:	9b01      	ldr	r3, [sp, #4]
3000a78e:	4a17      	ldr	r2, [pc, #92]	; (3000a7ec <do_stack_backtrace+0x94>)
3000a790:	4917      	ldr	r1, [pc, #92]	; (3000a7f0 <do_stack_backtrace+0x98>)
3000a792:	4818      	ldr	r0, [pc, #96]	; (3000a7f4 <do_stack_backtrace+0x9c>)
3000a794:	f001 fb96 	bl	3000bec4 <DiagPrintfNano>
3000a798:	b9ac      	cbnz	r4, 3000a7c6 <do_stack_backtrace+0x6e>
3000a79a:	682b      	ldr	r3, [r5, #0]
3000a79c:	b1b3      	cbz	r3, 3000a7cc <do_stack_backtrace+0x74>
3000a79e:	4e16      	ldr	r6, [pc, #88]	; (3000a7f8 <do_stack_backtrace+0xa0>)
3000a7a0:	e007      	b.n	3000a7b2 <do_stack_backtrace+0x5a>
3000a7a2:	9901      	ldr	r1, [sp, #4]
3000a7a4:	f021 0101 	bic.w	r1, r1, #1
3000a7a8:	f001 fb8c 	bl	3000bec4 <DiagPrintfNano>
3000a7ac:	682b      	ldr	r3, [r5, #0]
3000a7ae:	42a3      	cmp	r3, r4
3000a7b0:	d90c      	bls.n	3000a7cc <do_stack_backtrace+0x74>
3000a7b2:	aa03      	add	r2, sp, #12
3000a7b4:	a901      	add	r1, sp, #4
3000a7b6:	a802      	add	r0, sp, #8
3000a7b8:	3401      	adds	r4, #1
3000a7ba:	f7ff feb7 	bl	3000a52c <thumb_backtrace_from_stack>
3000a7be:	4603      	mov	r3, r0
3000a7c0:	4630      	mov	r0, r6
3000a7c2:	2b00      	cmp	r3, #0
3000a7c4:	d0ed      	beq.n	3000a7a2 <do_stack_backtrace+0x4a>
3000a7c6:	480d      	ldr	r0, [pc, #52]	; (3000a7fc <do_stack_backtrace+0xa4>)
3000a7c8:	f001 fb7c 	bl	3000bec4 <DiagPrintfNano>
3000a7cc:	b004      	add	sp, #16
3000a7ce:	bd70      	pop	{r4, r5, r6, pc}
3000a7d0:	4669      	mov	r1, sp
3000a7d2:	9102      	str	r1, [sp, #8]
3000a7d4:	4678      	mov	r0, pc
3000a7d6:	9001      	str	r0, [sp, #4]
3000a7d8:	4673      	mov	r3, lr
3000a7da:	9303      	str	r3, [sp, #12]
3000a7dc:	f3ef 8303 	mrs	r3, PSR
3000a7e0:	e7c6      	b.n	3000a770 <do_stack_backtrace+0x18>
3000a7e2:	bf00      	nop
3000a7e4:	3000f0c4 	.word	0x3000f0c4
3000a7e8:	3000d9a4 	.word	0x3000d9a4
3000a7ec:	3000d9e4 	.word	0x3000d9e4
3000a7f0:	3000da50 	.word	0x3000da50
3000a7f4:	3000da84 	.word	0x3000da84
3000a7f8:	3000dac8 	.word	0x3000dac8
3000a7fc:	3000c7c0 	.word	0x3000c7c0

3000a800 <crash_dump_memory>:
3000a800:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
3000a804:	460e      	mov	r6, r1
3000a806:	4680      	mov	r8, r0
3000a808:	4b21      	ldr	r3, [pc, #132]	; (3000a890 <crash_dump_memory+0x90>)
3000a80a:	f1a6 0080 	sub.w	r0, r6, #128	; 0x80
3000a80e:	4a21      	ldr	r2, [pc, #132]	; (3000a894 <crash_dump_memory+0x94>)
3000a810:	f8d3 127c 	ldr.w	r1, [r3, #636]	; 0x27c
3000a814:	4644      	mov	r4, r8
3000a816:	f5b0 5f7e 	cmp.w	r0, #16256	; 0x3f80
3000a81a:	4b1f      	ldr	r3, [pc, #124]	; (3000a898 <crash_dump_memory+0x98>)
3000a81c:	481f      	ldr	r0, [pc, #124]	; (3000a89c <crash_dump_memory+0x9c>)
3000a81e:	bf88      	it	hi
3000a820:	2680      	movhi	r6, #128	; 0x80
3000a822:	f011 0fc0 	tst.w	r1, #192	; 0xc0
3000a826:	4f1e      	ldr	r7, [pc, #120]	; (3000a8a0 <crash_dump_memory+0xa0>)
3000a828:	bf14      	ite	ne
3000a82a:	4611      	movne	r1, r2
3000a82c:	4619      	moveq	r1, r3
3000a82e:	f001 fb49 	bl	3000bec4 <DiagPrintfNano>
3000a832:	4632      	mov	r2, r6
3000a834:	4641      	mov	r1, r8
3000a836:	481b      	ldr	r0, [pc, #108]	; (3000a8a4 <crash_dump_memory+0xa4>)
3000a838:	f001 fb44 	bl	3000bec4 <DiagPrintfNano>
3000a83c:	f8df 906c 	ldr.w	r9, [pc, #108]	; 3000a8ac <crash_dump_memory+0xac>
3000a840:	2300      	movs	r3, #0
3000a842:	f8df a06c 	ldr.w	sl, [pc, #108]	; 3000a8b0 <crash_dump_memory+0xb0>
3000a846:	e007      	b.n	3000a858 <crash_dump_memory+0x58>
3000a848:	6821      	ldr	r1, [r4, #0]
3000a84a:	4638      	mov	r0, r7
3000a84c:	f001 fb3a 	bl	3000bec4 <DiagPrintfNano>
3000a850:	42ae      	cmp	r6, r5
3000a852:	d00f      	beq.n	3000a874 <crash_dump_memory+0x74>
3000a854:	3404      	adds	r4, #4
3000a856:	462b      	mov	r3, r5
3000a858:	1c5d      	adds	r5, r3, #1
3000a85a:	b183      	cbz	r3, 3000a87e <crash_dump_memory+0x7e>
3000a85c:	079b      	lsls	r3, r3, #30
3000a85e:	d1f3      	bne.n	3000a848 <crash_dump_memory+0x48>
3000a860:	4621      	mov	r1, r4
3000a862:	4648      	mov	r0, r9
3000a864:	f001 fb2e 	bl	3000bec4 <DiagPrintfNano>
3000a868:	6821      	ldr	r1, [r4, #0]
3000a86a:	4638      	mov	r0, r7
3000a86c:	f001 fb2a 	bl	3000bec4 <DiagPrintfNano>
3000a870:	42ae      	cmp	r6, r5
3000a872:	d1ef      	bne.n	3000a854 <crash_dump_memory+0x54>
3000a874:	480c      	ldr	r0, [pc, #48]	; (3000a8a8 <crash_dump_memory+0xa8>)
3000a876:	e8bd 47f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
3000a87a:	f001 bb23 	b.w	3000bec4 <DiagPrintfNano>
3000a87e:	4641      	mov	r1, r8
3000a880:	4650      	mov	r0, sl
3000a882:	f001 fb1f 	bl	3000bec4 <DiagPrintfNano>
3000a886:	6821      	ldr	r1, [r4, #0]
3000a888:	4638      	mov	r0, r7
3000a88a:	f001 fb1b 	bl	3000bec4 <DiagPrintfNano>
3000a88e:	e7e1      	b.n	3000a854 <crash_dump_memory+0x54>
3000a890:	42008000 	.word	0x42008000
3000a894:	3000dad0 	.word	0x3000dad0
3000a898:	3000dad4 	.word	0x3000dad4
3000a89c:	3000dad8 	.word	0x3000dad8
3000a8a0:	3000db44 	.word	0x3000db44
3000a8a4:	3000dafc 	.word	0x3000dafc
3000a8a8:	3000c7c0 	.word	0x3000c7c0
3000a8ac:	3000db4c 	.word	0x3000db4c
3000a8b0:	3000db38 	.word	0x3000db38

3000a8b4 <crash_dump>:
3000a8b4:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
3000a8b8:	4c62      	ldr	r4, [pc, #392]	; (3000aa44 <crash_dump+0x190>)
3000a8ba:	4615      	mov	r5, r2
3000a8bc:	468a      	mov	sl, r1
3000a8be:	f04f 23e0 	mov.w	r3, #3758153728	; 0xe000e000
3000a8c2:	4861      	ldr	r0, [pc, #388]	; (3000aa48 <crash_dump+0x194>)
3000a8c4:	ed2d 8b02 	vpush	{d8}
3000a8c8:	e9d2 2109 	ldrd	r2, r1, [r2, #36]	; 0x24
3000a8cc:	b083      	sub	sp, #12
3000a8ce:	e9c4 2100 	strd	r2, r1, [r4]
3000a8d2:	6aea      	ldr	r2, [r5, #44]	; 0x2c
3000a8d4:	60a2      	str	r2, [r4, #8]
3000a8d6:	6b2a      	ldr	r2, [r5, #48]	; 0x30
3000a8d8:	60e2      	str	r2, [r4, #12]
3000a8da:	686a      	ldr	r2, [r5, #4]
3000a8dc:	6122      	str	r2, [r4, #16]
3000a8de:	68aa      	ldr	r2, [r5, #8]
3000a8e0:	6162      	str	r2, [r4, #20]
3000a8e2:	68ea      	ldr	r2, [r5, #12]
3000a8e4:	61a2      	str	r2, [r4, #24]
3000a8e6:	692a      	ldr	r2, [r5, #16]
3000a8e8:	61e2      	str	r2, [r4, #28]
3000a8ea:	696a      	ldr	r2, [r5, #20]
3000a8ec:	6222      	str	r2, [r4, #32]
3000a8ee:	69aa      	ldr	r2, [r5, #24]
3000a8f0:	6262      	str	r2, [r4, #36]	; 0x24
3000a8f2:	69ea      	ldr	r2, [r5, #28]
3000a8f4:	62a2      	str	r2, [r4, #40]	; 0x28
3000a8f6:	6a2a      	ldr	r2, [r5, #32]
3000a8f8:	62e2      	str	r2, [r4, #44]	; 0x2c
3000a8fa:	6b6a      	ldr	r2, [r5, #52]	; 0x34
3000a8fc:	6322      	str	r2, [r4, #48]	; 0x30
3000a8fe:	6baa      	ldr	r2, [r5, #56]	; 0x38
3000a900:	6362      	str	r2, [r4, #52]	; 0x34
3000a902:	6bea      	ldr	r2, [r5, #60]	; 0x3c
3000a904:	63a2      	str	r2, [r4, #56]	; 0x38
3000a906:	6c2a      	ldr	r2, [r5, #64]	; 0x40
3000a908:	63e2      	str	r2, [r4, #60]	; 0x3c
3000a90a:	682a      	ldr	r2, [r5, #0]
3000a90c:	6422      	str	r2, [r4, #64]	; 0x40
3000a90e:	f8d3 2d24 	ldr.w	r2, [r3, #3364]	; 0xd24
3000a912:	6462      	str	r2, [r4, #68]	; 0x44
3000a914:	4a4d      	ldr	r2, [pc, #308]	; (3000aa4c <crash_dump+0x198>)
3000a916:	f892 1028 	ldrb.w	r1, [r2, #40]	; 0x28
3000a91a:	f884 1048 	strb.w	r1, [r4, #72]	; 0x48
3000a91e:	f8d3 1d34 	ldr.w	r1, [r3, #3380]	; 0xd34
3000a922:	64e1      	str	r1, [r4, #76]	; 0x4c
3000a924:	f892 1029 	ldrb.w	r1, [r2, #41]	; 0x29
3000a928:	f884 1050 	strb.w	r1, [r4, #80]	; 0x50
3000a92c:	f8d3 1d38 	ldr.w	r1, [r3, #3384]	; 0xd38
3000a930:	6561      	str	r1, [r4, #84]	; 0x54
3000a932:	8d51      	ldrh	r1, [r2, #42]	; 0x2a
3000a934:	f8a4 1058 	strh.w	r1, [r4, #88]	; 0x58
3000a938:	f8d3 1d2c 	ldr.w	r1, [r3, #3372]	; 0xd2c
3000a93c:	65e1      	str	r1, [r4, #92]	; 0x5c
3000a93e:	8e11      	ldrh	r1, [r2, #48]	; 0x30
3000a940:	8f92      	ldrh	r2, [r2, #60]	; 0x3c
3000a942:	b289      	uxth	r1, r1
3000a944:	b292      	uxth	r2, r2
3000a946:	6621      	str	r1, [r4, #96]	; 0x60
3000a948:	6662      	str	r2, [r4, #100]	; 0x64
3000a94a:	f8d3 2de4 	ldr.w	r2, [r3, #3556]	; 0xde4
3000a94e:	66a2      	str	r2, [r4, #104]	; 0x68
3000a950:	f8d3 3de8 	ldr.w	r3, [r3, #3560]	; 0xde8
3000a954:	66e3      	str	r3, [r4, #108]	; 0x6c
3000a956:	f001 fab5 	bl	3000bec4 <DiagPrintfNano>
3000a95a:	4b3d      	ldr	r3, [pc, #244]	; (3000aa50 <crash_dump+0x19c>)
3000a95c:	681b      	ldr	r3, [r3, #0]
3000a95e:	2b00      	cmp	r3, #0
3000a960:	d06b      	beq.n	3000aa3a <crash_dump+0x186>
3000a962:	4651      	mov	r1, sl
3000a964:	4628      	mov	r0, r5
3000a966:	4798      	blx	r3
3000a968:	ee08 0a10 	vmov	s16, r0
3000a96c:	4835      	ldr	r0, [pc, #212]	; (3000aa44 <crash_dump+0x190>)
3000a96e:	46a8      	mov	r8, r5
3000a970:	f000 f88e 	bl	3000aa90 <fault_diagnosis>
3000a974:	4837      	ldr	r0, [pc, #220]	; (3000aa54 <crash_dump+0x1a0>)
3000a976:	f001 faa5 	bl	3000bec4 <DiagPrintfNano>
3000a97a:	6ba9      	ldr	r1, [r5, #56]	; 0x38
3000a97c:	4836      	ldr	r0, [pc, #216]	; (3000aa58 <crash_dump+0x1a4>)
3000a97e:	462e      	mov	r6, r5
3000a980:	f001 faa0 	bl	3000bec4 <DiagPrintfNano>
3000a984:	6be9      	ldr	r1, [r5, #60]	; 0x3c
3000a986:	4835      	ldr	r0, [pc, #212]	; (3000aa5c <crash_dump+0x1a8>)
3000a988:	f001 fa9c 	bl	3000bec4 <DiagPrintfNano>
3000a98c:	6c29      	ldr	r1, [r5, #64]	; 0x40
3000a98e:	4834      	ldr	r0, [pc, #208]	; (3000aa60 <crash_dump+0x1ac>)
3000a990:	f001 fa98 	bl	3000bec4 <DiagPrintfNano>
3000a994:	f858 1b20 	ldr.w	r1, [r8], #32
3000a998:	4832      	ldr	r0, [pc, #200]	; (3000aa64 <crash_dump+0x1b0>)
3000a99a:	f001 fa93 	bl	3000bec4 <DiagPrintfNano>
3000a99e:	4647      	mov	r7, r8
3000a9a0:	f105 0330 	add.w	r3, r5, #48	; 0x30
3000a9a4:	f8df b0d0 	ldr.w	fp, [pc, #208]	; 3000aa78 <crash_dump+0x1c4>
3000a9a8:	f857 2f04 	ldr.w	r2, [r7, #4]!
3000a9ac:	f8cd 8004 	str.w	r8, [sp, #4]
3000a9b0:	46a8      	mov	r8, r5
3000a9b2:	461d      	mov	r5, r3
3000a9b4:	492c      	ldr	r1, [pc, #176]	; (3000aa68 <crash_dump+0x1b4>)
3000a9b6:	4658      	mov	r0, fp
3000a9b8:	f8df 90d0 	ldr.w	r9, [pc, #208]	; 3000aa8c <crash_dump+0x1d8>
3000a9bc:	f001 fa82 	bl	3000bec4 <DiagPrintfNano>
3000a9c0:	42af      	cmp	r7, r5
3000a9c2:	d008      	beq.n	3000a9d6 <crash_dump+0x122>
3000a9c4:	f857 2f04 	ldr.w	r2, [r7, #4]!
3000a9c8:	4658      	mov	r0, fp
3000a9ca:	f859 1b04 	ldr.w	r1, [r9], #4
3000a9ce:	f001 fa79 	bl	3000bec4 <DiagPrintfNano>
3000a9d2:	42af      	cmp	r7, r5
3000a9d4:	d1f6      	bne.n	3000a9c4 <crash_dump+0x110>
3000a9d6:	4645      	mov	r5, r8
3000a9d8:	4f24      	ldr	r7, [pc, #144]	; (3000aa6c <crash_dump+0x1b8>)
3000a9da:	f8dd 8004 	ldr.w	r8, [sp, #4]
3000a9de:	4924      	ldr	r1, [pc, #144]	; (3000aa70 <crash_dump+0x1bc>)
3000a9e0:	f8df 9094 	ldr.w	r9, [pc, #148]	; 3000aa78 <crash_dump+0x1c4>
3000a9e4:	e001      	b.n	3000a9ea <crash_dump+0x136>
3000a9e6:	f857 1b04 	ldr.w	r1, [r7], #4
3000a9ea:	f856 2f04 	ldr.w	r2, [r6, #4]!
3000a9ee:	4648      	mov	r0, r9
3000a9f0:	f001 fa68 	bl	3000bec4 <DiagPrintfNano>
3000a9f4:	4546      	cmp	r6, r8
3000a9f6:	d1f6      	bne.n	3000a9e6 <crash_dump+0x132>
3000a9f8:	6b6a      	ldr	r2, [r5, #52]	; 0x34
3000a9fa:	491e      	ldr	r1, [pc, #120]	; (3000aa74 <crash_dump+0x1c0>)
3000a9fc:	481e      	ldr	r0, [pc, #120]	; (3000aa78 <crash_dump+0x1c4>)
3000a9fe:	f001 fa61 	bl	3000bec4 <DiagPrintfNano>
3000aa02:	ee18 1a10 	vmov	r1, s16
3000aa06:	4650      	mov	r0, sl
3000aa08:	f7ff fefa 	bl	3000a800 <crash_dump_memory>
3000aa0c:	481b      	ldr	r0, [pc, #108]	; (3000aa7c <crash_dump+0x1c8>)
3000aa0e:	f001 fa59 	bl	3000bec4 <DiagPrintfNano>
3000aa12:	6c22      	ldr	r2, [r4, #64]	; 0x40
3000aa14:	4651      	mov	r1, sl
3000aa16:	6ba0      	ldr	r0, [r4, #56]	; 0x38
3000aa18:	f7ff fe9e 	bl	3000a758 <do_stack_backtrace>
3000aa1c:	4818      	ldr	r0, [pc, #96]	; (3000aa80 <crash_dump+0x1cc>)
3000aa1e:	f001 fa51 	bl	3000bec4 <DiagPrintfNano>
3000aa22:	4818      	ldr	r0, [pc, #96]	; (3000aa84 <crash_dump+0x1d0>)
3000aa24:	f001 fa4e 	bl	3000bec4 <DiagPrintfNano>
3000aa28:	4817      	ldr	r0, [pc, #92]	; (3000aa88 <crash_dump+0x1d4>)
3000aa2a:	f001 fa4b 	bl	3000bec4 <DiagPrintfNano>
3000aa2e:	2000      	movs	r0, #0
3000aa30:	b003      	add	sp, #12
3000aa32:	ecbd 8b02 	vpop	{d8}
3000aa36:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
3000aa3a:	2380      	movs	r3, #128	; 0x80
3000aa3c:	ee08 3a10 	vmov	s16, r3
3000aa40:	e794      	b.n	3000a96c <crash_dump+0xb8>
3000aa42:	bf00      	nop
3000aa44:	3000f96c 	.word	0x3000f96c
3000aa48:	3000db60 	.word	0x3000db60
3000aa4c:	e000ed00 	.word	0xe000ed00
3000aa50:	3000f968 	.word	0x3000f968
3000aa54:	3000db84 	.word	0x3000db84
3000aa58:	3000dbac 	.word	0x3000dbac
3000aa5c:	3000dbbc 	.word	0x3000dbbc
3000aa60:	3000dbcc 	.word	0x3000dbcc
3000aa64:	3000dbdc 	.word	0x3000dbdc
3000aa68:	3000db58 	.word	0x3000db58
3000aa6c:	3000dcb4 	.word	0x3000dcb4
3000aa70:	3000db5c 	.word	0x3000db5c
3000aa74:	3000dbfc 	.word	0x3000dbfc
3000aa78:	3000dbec 	.word	0x3000dbec
3000aa7c:	3000dc00 	.word	0x3000dc00
3000aa80:	3000dc24 	.word	0x3000dc24
3000aa84:	3000dc50 	.word	0x3000dc50
3000aa88:	3000dc7c 	.word	0x3000dc7c
3000aa8c:	3000dcd4 	.word	0x3000dcd4

3000aa90 <fault_diagnosis>:
3000aa90:	b510      	push	{r4, lr}
3000aa92:	4604      	mov	r4, r0
3000aa94:	4888      	ldr	r0, [pc, #544]	; (3000acb8 <fault_diagnosis+0x228>)
3000aa96:	6ba1      	ldr	r1, [r4, #56]	; 0x38
3000aa98:	f001 fa14 	bl	3000bec4 <DiagPrintfNano>
3000aa9c:	f894 305c 	ldrb.w	r3, [r4, #92]	; 0x5c
3000aaa0:	079b      	lsls	r3, r3, #30
3000aaa2:	f100 808a 	bmi.w	3000abba <fault_diagnosis+0x12a>
3000aaa6:	6ea1      	ldr	r1, [r4, #104]	; 0x68
3000aaa8:	b1e1      	cbz	r1, 3000aae4 <fault_diagnosis+0x54>
3000aaaa:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
3000aaae:	07d8      	lsls	r0, r3, #31
3000aab0:	f100 8094 	bmi.w	3000abdc <fault_diagnosis+0x14c>
3000aab4:	079a      	lsls	r2, r3, #30
3000aab6:	f100 809a 	bmi.w	3000abee <fault_diagnosis+0x15e>
3000aaba:	0758      	lsls	r0, r3, #29
3000aabc:	f100 80a1 	bmi.w	3000ac02 <fault_diagnosis+0x172>
3000aac0:	0719      	lsls	r1, r3, #28
3000aac2:	f100 80a8 	bmi.w	3000ac16 <fault_diagnosis+0x186>
3000aac6:	06da      	lsls	r2, r3, #27
3000aac8:	f100 80af 	bmi.w	3000ac2a <fault_diagnosis+0x19a>
3000aacc:	0698      	lsls	r0, r3, #26
3000aace:	f100 80b6 	bmi.w	3000ac3e <fault_diagnosis+0x1ae>
3000aad2:	0619      	lsls	r1, r3, #24
3000aad4:	f100 80bd 	bmi.w	3000ac52 <fault_diagnosis+0x1c2>
3000aad8:	065a      	lsls	r2, r3, #25
3000aada:	d503      	bpl.n	3000aae4 <fault_diagnosis+0x54>
3000aadc:	f013 0f28 	tst.w	r3, #40	; 0x28
3000aae0:	f040 8186 	bne.w	3000adf0 <fault_diagnosis+0x360>
3000aae4:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
3000aae8:	b199      	cbz	r1, 3000ab12 <fault_diagnosis+0x82>
3000aaea:	07cb      	lsls	r3, r1, #31
3000aaec:	f100 80b9 	bmi.w	3000ac62 <fault_diagnosis+0x1d2>
3000aaf0:	0788      	lsls	r0, r1, #30
3000aaf2:	f100 80bf 	bmi.w	3000ac74 <fault_diagnosis+0x1e4>
3000aaf6:	070a      	lsls	r2, r1, #28
3000aaf8:	f100 80c5 	bmi.w	3000ac86 <fault_diagnosis+0x1f6>
3000aafc:	06cb      	lsls	r3, r1, #27
3000aafe:	f100 80cb 	bmi.w	3000ac98 <fault_diagnosis+0x208>
3000ab02:	0688      	lsls	r0, r1, #26
3000ab04:	f100 80d1 	bmi.w	3000acaa <fault_diagnosis+0x21a>
3000ab08:	060a      	lsls	r2, r1, #24
3000ab0a:	d502      	bpl.n	3000ab12 <fault_diagnosis+0x82>
3000ab0c:	078b      	lsls	r3, r1, #30
3000ab0e:	f040 8194 	bne.w	3000ae3a <fault_diagnosis+0x3aa>
3000ab12:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
3000ab16:	b1b1      	cbz	r1, 3000ab46 <fault_diagnosis+0xb6>
3000ab18:	07c8      	lsls	r0, r1, #31
3000ab1a:	f100 80f7 	bmi.w	3000ad0c <fault_diagnosis+0x27c>
3000ab1e:	078a      	lsls	r2, r1, #30
3000ab20:	f100 80fd 	bmi.w	3000ad1e <fault_diagnosis+0x28e>
3000ab24:	074b      	lsls	r3, r1, #29
3000ab26:	f100 8103 	bmi.w	3000ad30 <fault_diagnosis+0x2a0>
3000ab2a:	0708      	lsls	r0, r1, #28
3000ab2c:	f100 8109 	bmi.w	3000ad42 <fault_diagnosis+0x2b2>
3000ab30:	06ca      	lsls	r2, r1, #27
3000ab32:	f100 810f 	bmi.w	3000ad54 <fault_diagnosis+0x2c4>
3000ab36:	068b      	lsls	r3, r1, #26
3000ab38:	f100 8115 	bmi.w	3000ad66 <fault_diagnosis+0x2d6>
3000ab3c:	0608      	lsls	r0, r1, #24
3000ab3e:	d502      	bpl.n	3000ab46 <fault_diagnosis+0xb6>
3000ab40:	0789      	lsls	r1, r1, #30
3000ab42:	f100 8180 	bmi.w	3000ae46 <fault_diagnosis+0x3b6>
3000ab46:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
3000ab4a:	b1b9      	cbz	r1, 3000ab7c <fault_diagnosis+0xec>
3000ab4c:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
3000ab50:	07da      	lsls	r2, r3, #31
3000ab52:	f100 810f 	bmi.w	3000ad74 <fault_diagnosis+0x2e4>
3000ab56:	0798      	lsls	r0, r3, #30
3000ab58:	f100 8115 	bmi.w	3000ad86 <fault_diagnosis+0x2f6>
3000ab5c:	0759      	lsls	r1, r3, #29
3000ab5e:	f100 811d 	bmi.w	3000ad9c <fault_diagnosis+0x30c>
3000ab62:	071a      	lsls	r2, r3, #28
3000ab64:	f100 8125 	bmi.w	3000adb2 <fault_diagnosis+0x322>
3000ab68:	06db      	lsls	r3, r3, #27
3000ab6a:	f100 812d 	bmi.w	3000adc8 <fault_diagnosis+0x338>
3000ab6e:	f894 3059 	ldrb.w	r3, [r4, #89]	; 0x59
3000ab72:	07d8      	lsls	r0, r3, #31
3000ab74:	f100 8133 	bmi.w	3000adde <fault_diagnosis+0x34e>
3000ab78:	0799      	lsls	r1, r3, #30
3000ab7a:	d424      	bmi.n	3000abc6 <fault_diagnosis+0x136>
3000ab7c:	f994 305f 	ldrsb.w	r3, [r4, #95]	; 0x5f
3000ab80:	2b00      	cmp	r3, #0
3000ab82:	db00      	blt.n	3000ab86 <fault_diagnosis+0xf6>
3000ab84:	bd10      	pop	{r4, pc}
3000ab86:	6e21      	ldr	r1, [r4, #96]	; 0x60
3000ab88:	2900      	cmp	r1, #0
3000ab8a:	d0fb      	beq.n	3000ab84 <fault_diagnosis+0xf4>
3000ab8c:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
3000ab90:	07da      	lsls	r2, r3, #31
3000ab92:	f100 8143 	bmi.w	3000ae1c <fault_diagnosis+0x38c>
3000ab96:	0798      	lsls	r0, r3, #30
3000ab98:	f100 8138 	bmi.w	3000ae0c <fault_diagnosis+0x37c>
3000ab9c:	0759      	lsls	r1, r3, #29
3000ab9e:	f100 812d 	bmi.w	3000adfc <fault_diagnosis+0x36c>
3000aba2:	071a      	lsls	r2, r3, #28
3000aba4:	f100 8141 	bmi.w	3000ae2a <fault_diagnosis+0x39a>
3000aba8:	06db      	lsls	r3, r3, #27
3000abaa:	d5eb      	bpl.n	3000ab84 <fault_diagnosis+0xf4>
3000abac:	6e21      	ldr	r1, [r4, #96]	; 0x60
3000abae:	4a43      	ldr	r2, [pc, #268]	; (3000acbc <fault_diagnosis+0x22c>)
3000abb0:	4843      	ldr	r0, [pc, #268]	; (3000acc0 <fault_diagnosis+0x230>)
3000abb2:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
3000abb6:	f001 b985 	b.w	3000bec4 <DiagPrintfNano>
3000abba:	4a42      	ldr	r2, [pc, #264]	; (3000acc4 <fault_diagnosis+0x234>)
3000abbc:	6de1      	ldr	r1, [r4, #92]	; 0x5c
3000abbe:	4842      	ldr	r0, [pc, #264]	; (3000acc8 <fault_diagnosis+0x238>)
3000abc0:	f001 f980 	bl	3000bec4 <DiagPrintfNano>
3000abc4:	e76f      	b.n	3000aaa6 <fault_diagnosis+0x16>
3000abc6:	4a41      	ldr	r2, [pc, #260]	; (3000accc <fault_diagnosis+0x23c>)
3000abc8:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
3000abcc:	4840      	ldr	r0, [pc, #256]	; (3000acd0 <fault_diagnosis+0x240>)
3000abce:	f001 f979 	bl	3000bec4 <DiagPrintfNano>
3000abd2:	f994 305f 	ldrsb.w	r3, [r4, #95]	; 0x5f
3000abd6:	2b00      	cmp	r3, #0
3000abd8:	dad4      	bge.n	3000ab84 <fault_diagnosis+0xf4>
3000abda:	e7d4      	b.n	3000ab86 <fault_diagnosis+0xf6>
3000abdc:	4a3d      	ldr	r2, [pc, #244]	; (3000acd4 <fault_diagnosis+0x244>)
3000abde:	483e      	ldr	r0, [pc, #248]	; (3000acd8 <fault_diagnosis+0x248>)
3000abe0:	f001 f970 	bl	3000bec4 <DiagPrintfNano>
3000abe4:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
3000abe8:	079a      	lsls	r2, r3, #30
3000abea:	f57f af66 	bpl.w	3000aaba <fault_diagnosis+0x2a>
3000abee:	4a3b      	ldr	r2, [pc, #236]	; (3000acdc <fault_diagnosis+0x24c>)
3000abf0:	6ea1      	ldr	r1, [r4, #104]	; 0x68
3000abf2:	4839      	ldr	r0, [pc, #228]	; (3000acd8 <fault_diagnosis+0x248>)
3000abf4:	f001 f966 	bl	3000bec4 <DiagPrintfNano>
3000abf8:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
3000abfc:	0758      	lsls	r0, r3, #29
3000abfe:	f57f af5f 	bpl.w	3000aac0 <fault_diagnosis+0x30>
3000ac02:	6ea1      	ldr	r1, [r4, #104]	; 0x68
3000ac04:	4a36      	ldr	r2, [pc, #216]	; (3000ace0 <fault_diagnosis+0x250>)
3000ac06:	4834      	ldr	r0, [pc, #208]	; (3000acd8 <fault_diagnosis+0x248>)
3000ac08:	f001 f95c 	bl	3000bec4 <DiagPrintfNano>
3000ac0c:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
3000ac10:	0719      	lsls	r1, r3, #28
3000ac12:	f57f af58 	bpl.w	3000aac6 <fault_diagnosis+0x36>
3000ac16:	4a33      	ldr	r2, [pc, #204]	; (3000ace4 <fault_diagnosis+0x254>)
3000ac18:	6ea1      	ldr	r1, [r4, #104]	; 0x68
3000ac1a:	482f      	ldr	r0, [pc, #188]	; (3000acd8 <fault_diagnosis+0x248>)
3000ac1c:	f001 f952 	bl	3000bec4 <DiagPrintfNano>
3000ac20:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
3000ac24:	06da      	lsls	r2, r3, #27
3000ac26:	f57f af51 	bpl.w	3000aacc <fault_diagnosis+0x3c>
3000ac2a:	4a2f      	ldr	r2, [pc, #188]	; (3000ace8 <fault_diagnosis+0x258>)
3000ac2c:	6ea1      	ldr	r1, [r4, #104]	; 0x68
3000ac2e:	482a      	ldr	r0, [pc, #168]	; (3000acd8 <fault_diagnosis+0x248>)
3000ac30:	f001 f948 	bl	3000bec4 <DiagPrintfNano>
3000ac34:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
3000ac38:	0698      	lsls	r0, r3, #26
3000ac3a:	f57f af4a 	bpl.w	3000aad2 <fault_diagnosis+0x42>
3000ac3e:	6ea1      	ldr	r1, [r4, #104]	; 0x68
3000ac40:	4a2a      	ldr	r2, [pc, #168]	; (3000acec <fault_diagnosis+0x25c>)
3000ac42:	4825      	ldr	r0, [pc, #148]	; (3000acd8 <fault_diagnosis+0x248>)
3000ac44:	f001 f93e 	bl	3000bec4 <DiagPrintfNano>
3000ac48:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
3000ac4c:	0619      	lsls	r1, r3, #24
3000ac4e:	f57f af43 	bpl.w	3000aad8 <fault_diagnosis+0x48>
3000ac52:	4a27      	ldr	r2, [pc, #156]	; (3000acf0 <fault_diagnosis+0x260>)
3000ac54:	6ea1      	ldr	r1, [r4, #104]	; 0x68
3000ac56:	4820      	ldr	r0, [pc, #128]	; (3000acd8 <fault_diagnosis+0x248>)
3000ac58:	f001 f934 	bl	3000bec4 <DiagPrintfNano>
3000ac5c:	f894 3068 	ldrb.w	r3, [r4, #104]	; 0x68
3000ac60:	e73a      	b.n	3000aad8 <fault_diagnosis+0x48>
3000ac62:	4a24      	ldr	r2, [pc, #144]	; (3000acf4 <fault_diagnosis+0x264>)
3000ac64:	4824      	ldr	r0, [pc, #144]	; (3000acf8 <fault_diagnosis+0x268>)
3000ac66:	f001 f92d 	bl	3000bec4 <DiagPrintfNano>
3000ac6a:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
3000ac6e:	0788      	lsls	r0, r1, #30
3000ac70:	f57f af41 	bpl.w	3000aaf6 <fault_diagnosis+0x66>
3000ac74:	4a21      	ldr	r2, [pc, #132]	; (3000acfc <fault_diagnosis+0x26c>)
3000ac76:	4820      	ldr	r0, [pc, #128]	; (3000acf8 <fault_diagnosis+0x268>)
3000ac78:	f001 f924 	bl	3000bec4 <DiagPrintfNano>
3000ac7c:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
3000ac80:	070a      	lsls	r2, r1, #28
3000ac82:	f57f af3b 	bpl.w	3000aafc <fault_diagnosis+0x6c>
3000ac86:	4a1e      	ldr	r2, [pc, #120]	; (3000ad00 <fault_diagnosis+0x270>)
3000ac88:	481b      	ldr	r0, [pc, #108]	; (3000acf8 <fault_diagnosis+0x268>)
3000ac8a:	f001 f91b 	bl	3000bec4 <DiagPrintfNano>
3000ac8e:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
3000ac92:	06cb      	lsls	r3, r1, #27
3000ac94:	f57f af35 	bpl.w	3000ab02 <fault_diagnosis+0x72>
3000ac98:	4a1a      	ldr	r2, [pc, #104]	; (3000ad04 <fault_diagnosis+0x274>)
3000ac9a:	4817      	ldr	r0, [pc, #92]	; (3000acf8 <fault_diagnosis+0x268>)
3000ac9c:	f001 f912 	bl	3000bec4 <DiagPrintfNano>
3000aca0:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
3000aca4:	0688      	lsls	r0, r1, #26
3000aca6:	f57f af2f 	bpl.w	3000ab08 <fault_diagnosis+0x78>
3000acaa:	4a17      	ldr	r2, [pc, #92]	; (3000ad08 <fault_diagnosis+0x278>)
3000acac:	4812      	ldr	r0, [pc, #72]	; (3000acf8 <fault_diagnosis+0x268>)
3000acae:	f001 f909 	bl	3000bec4 <DiagPrintfNano>
3000acb2:	f894 1048 	ldrb.w	r1, [r4, #72]	; 0x48
3000acb6:	e727      	b.n	3000ab08 <fault_diagnosis+0x78>
3000acb8:	3000dce4 	.word	0x3000dce4
3000acbc:	3000e578 	.word	0x3000e578
3000acc0:	3000e4d0 	.word	0x3000e4d0
3000acc4:	3000dd00 	.word	0x3000dd00
3000acc8:	3000dd2c 	.word	0x3000dd2c
3000accc:	3000e434 	.word	0x3000e434
3000acd0:	3000e278 	.word	0x3000e278
3000acd4:	3000dd44 	.word	0x3000dd44
3000acd8:	3000dd74 	.word	0x3000dd74
3000acdc:	3000dd8c 	.word	0x3000dd8c
3000ace0:	3000ddc4 	.word	0x3000ddc4
3000ace4:	3000ddfc 	.word	0x3000ddfc
3000ace8:	3000de34 	.word	0x3000de34
3000acec:	3000de64 	.word	0x3000de64
3000acf0:	3000dea0 	.word	0x3000dea0
3000acf4:	3000df10 	.word	0x3000df10
3000acf8:	3000df54 	.word	0x3000df54
3000acfc:	3000df6c 	.word	0x3000df6c
3000ad00:	3000dfa8 	.word	0x3000dfa8
3000ad04:	3000dfe0 	.word	0x3000dfe0
3000ad08:	3000e014 	.word	0x3000e014
3000ad0c:	4a51      	ldr	r2, [pc, #324]	; (3000ae54 <fault_diagnosis+0x3c4>)
3000ad0e:	4852      	ldr	r0, [pc, #328]	; (3000ae58 <fault_diagnosis+0x3c8>)
3000ad10:	f001 f8d8 	bl	3000bec4 <DiagPrintfNano>
3000ad14:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
3000ad18:	078a      	lsls	r2, r1, #30
3000ad1a:	f57f af03 	bpl.w	3000ab24 <fault_diagnosis+0x94>
3000ad1e:	4a4f      	ldr	r2, [pc, #316]	; (3000ae5c <fault_diagnosis+0x3cc>)
3000ad20:	484d      	ldr	r0, [pc, #308]	; (3000ae58 <fault_diagnosis+0x3c8>)
3000ad22:	f001 f8cf 	bl	3000bec4 <DiagPrintfNano>
3000ad26:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
3000ad2a:	074b      	lsls	r3, r1, #29
3000ad2c:	f57f aefd 	bpl.w	3000ab2a <fault_diagnosis+0x9a>
3000ad30:	4a4b      	ldr	r2, [pc, #300]	; (3000ae60 <fault_diagnosis+0x3d0>)
3000ad32:	4849      	ldr	r0, [pc, #292]	; (3000ae58 <fault_diagnosis+0x3c8>)
3000ad34:	f001 f8c6 	bl	3000bec4 <DiagPrintfNano>
3000ad38:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
3000ad3c:	0708      	lsls	r0, r1, #28
3000ad3e:	f57f aef7 	bpl.w	3000ab30 <fault_diagnosis+0xa0>
3000ad42:	4a48      	ldr	r2, [pc, #288]	; (3000ae64 <fault_diagnosis+0x3d4>)
3000ad44:	4844      	ldr	r0, [pc, #272]	; (3000ae58 <fault_diagnosis+0x3c8>)
3000ad46:	f001 f8bd 	bl	3000bec4 <DiagPrintfNano>
3000ad4a:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
3000ad4e:	06ca      	lsls	r2, r1, #27
3000ad50:	f57f aef1 	bpl.w	3000ab36 <fault_diagnosis+0xa6>
3000ad54:	4a44      	ldr	r2, [pc, #272]	; (3000ae68 <fault_diagnosis+0x3d8>)
3000ad56:	4840      	ldr	r0, [pc, #256]	; (3000ae58 <fault_diagnosis+0x3c8>)
3000ad58:	f001 f8b4 	bl	3000bec4 <DiagPrintfNano>
3000ad5c:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
3000ad60:	068b      	lsls	r3, r1, #26
3000ad62:	f57f aeeb 	bpl.w	3000ab3c <fault_diagnosis+0xac>
3000ad66:	4a41      	ldr	r2, [pc, #260]	; (3000ae6c <fault_diagnosis+0x3dc>)
3000ad68:	483b      	ldr	r0, [pc, #236]	; (3000ae58 <fault_diagnosis+0x3c8>)
3000ad6a:	f001 f8ab 	bl	3000bec4 <DiagPrintfNano>
3000ad6e:	f894 1050 	ldrb.w	r1, [r4, #80]	; 0x50
3000ad72:	e6e3      	b.n	3000ab3c <fault_diagnosis+0xac>
3000ad74:	4a3e      	ldr	r2, [pc, #248]	; (3000ae70 <fault_diagnosis+0x3e0>)
3000ad76:	483f      	ldr	r0, [pc, #252]	; (3000ae74 <fault_diagnosis+0x3e4>)
3000ad78:	f001 f8a4 	bl	3000bec4 <DiagPrintfNano>
3000ad7c:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
3000ad80:	0798      	lsls	r0, r3, #30
3000ad82:	f57f aeeb 	bpl.w	3000ab5c <fault_diagnosis+0xcc>
3000ad86:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
3000ad8a:	4a3b      	ldr	r2, [pc, #236]	; (3000ae78 <fault_diagnosis+0x3e8>)
3000ad8c:	4839      	ldr	r0, [pc, #228]	; (3000ae74 <fault_diagnosis+0x3e4>)
3000ad8e:	f001 f899 	bl	3000bec4 <DiagPrintfNano>
3000ad92:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
3000ad96:	0759      	lsls	r1, r3, #29
3000ad98:	f57f aee3 	bpl.w	3000ab62 <fault_diagnosis+0xd2>
3000ad9c:	4a37      	ldr	r2, [pc, #220]	; (3000ae7c <fault_diagnosis+0x3ec>)
3000ad9e:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
3000ada2:	4834      	ldr	r0, [pc, #208]	; (3000ae74 <fault_diagnosis+0x3e4>)
3000ada4:	f001 f88e 	bl	3000bec4 <DiagPrintfNano>
3000ada8:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
3000adac:	071a      	lsls	r2, r3, #28
3000adae:	f57f aedb 	bpl.w	3000ab68 <fault_diagnosis+0xd8>
3000adb2:	4a33      	ldr	r2, [pc, #204]	; (3000ae80 <fault_diagnosis+0x3f0>)
3000adb4:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
3000adb8:	482e      	ldr	r0, [pc, #184]	; (3000ae74 <fault_diagnosis+0x3e4>)
3000adba:	f001 f883 	bl	3000bec4 <DiagPrintfNano>
3000adbe:	f894 3058 	ldrb.w	r3, [r4, #88]	; 0x58
3000adc2:	06db      	lsls	r3, r3, #27
3000adc4:	f57f aed3 	bpl.w	3000ab6e <fault_diagnosis+0xde>
3000adc8:	4a2e      	ldr	r2, [pc, #184]	; (3000ae84 <fault_diagnosis+0x3f4>)
3000adca:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
3000adce:	4829      	ldr	r0, [pc, #164]	; (3000ae74 <fault_diagnosis+0x3e4>)
3000add0:	f001 f878 	bl	3000bec4 <DiagPrintfNano>
3000add4:	f894 3059 	ldrb.w	r3, [r4, #89]	; 0x59
3000add8:	07d8      	lsls	r0, r3, #31
3000adda:	f57f aecd 	bpl.w	3000ab78 <fault_diagnosis+0xe8>
3000adde:	4a2a      	ldr	r2, [pc, #168]	; (3000ae88 <fault_diagnosis+0x3f8>)
3000ade0:	f8b4 1058 	ldrh.w	r1, [r4, #88]	; 0x58
3000ade4:	4823      	ldr	r0, [pc, #140]	; (3000ae74 <fault_diagnosis+0x3e4>)
3000ade6:	f001 f86d 	bl	3000bec4 <DiagPrintfNano>
3000adea:	f894 3059 	ldrb.w	r3, [r4, #89]	; 0x59
3000adee:	e6c3      	b.n	3000ab78 <fault_diagnosis+0xe8>
3000adf0:	4a26      	ldr	r2, [pc, #152]	; (3000ae8c <fault_diagnosis+0x3fc>)
3000adf2:	6ee1      	ldr	r1, [r4, #108]	; 0x6c
3000adf4:	4826      	ldr	r0, [pc, #152]	; (3000ae90 <fault_diagnosis+0x400>)
3000adf6:	f001 f865 	bl	3000bec4 <DiagPrintfNano>
3000adfa:	e673      	b.n	3000aae4 <fault_diagnosis+0x54>
3000adfc:	4a25      	ldr	r2, [pc, #148]	; (3000ae94 <fault_diagnosis+0x404>)
3000adfe:	6e21      	ldr	r1, [r4, #96]	; 0x60
3000ae00:	4825      	ldr	r0, [pc, #148]	; (3000ae98 <fault_diagnosis+0x408>)
3000ae02:	f001 f85f 	bl	3000bec4 <DiagPrintfNano>
3000ae06:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
3000ae0a:	e6ca      	b.n	3000aba2 <fault_diagnosis+0x112>
3000ae0c:	4a23      	ldr	r2, [pc, #140]	; (3000ae9c <fault_diagnosis+0x40c>)
3000ae0e:	6e21      	ldr	r1, [r4, #96]	; 0x60
3000ae10:	4821      	ldr	r0, [pc, #132]	; (3000ae98 <fault_diagnosis+0x408>)
3000ae12:	f001 f857 	bl	3000bec4 <DiagPrintfNano>
3000ae16:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
3000ae1a:	e6bf      	b.n	3000ab9c <fault_diagnosis+0x10c>
3000ae1c:	4a20      	ldr	r2, [pc, #128]	; (3000aea0 <fault_diagnosis+0x410>)
3000ae1e:	481e      	ldr	r0, [pc, #120]	; (3000ae98 <fault_diagnosis+0x408>)
3000ae20:	f001 f850 	bl	3000bec4 <DiagPrintfNano>
3000ae24:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
3000ae28:	e6b5      	b.n	3000ab96 <fault_diagnosis+0x106>
3000ae2a:	4a1e      	ldr	r2, [pc, #120]	; (3000aea4 <fault_diagnosis+0x414>)
3000ae2c:	6e21      	ldr	r1, [r4, #96]	; 0x60
3000ae2e:	481a      	ldr	r0, [pc, #104]	; (3000ae98 <fault_diagnosis+0x408>)
3000ae30:	f001 f848 	bl	3000bec4 <DiagPrintfNano>
3000ae34:	f894 3060 	ldrb.w	r3, [r4, #96]	; 0x60
3000ae38:	e6b6      	b.n	3000aba8 <fault_diagnosis+0x118>
3000ae3a:	4a1b      	ldr	r2, [pc, #108]	; (3000aea8 <fault_diagnosis+0x418>)
3000ae3c:	6ce1      	ldr	r1, [r4, #76]	; 0x4c
3000ae3e:	481b      	ldr	r0, [pc, #108]	; (3000aeac <fault_diagnosis+0x41c>)
3000ae40:	f001 f840 	bl	3000bec4 <DiagPrintfNano>
3000ae44:	e665      	b.n	3000ab12 <fault_diagnosis+0x82>
3000ae46:	4a1a      	ldr	r2, [pc, #104]	; (3000aeb0 <fault_diagnosis+0x420>)
3000ae48:	6d61      	ldr	r1, [r4, #84]	; 0x54
3000ae4a:	481a      	ldr	r0, [pc, #104]	; (3000aeb4 <fault_diagnosis+0x424>)
3000ae4c:	f001 f83a 	bl	3000bec4 <DiagPrintfNano>
3000ae50:	e679      	b.n	3000ab46 <fault_diagnosis+0xb6>
3000ae52:	bf00      	nop
3000ae54:	3000e0a8 	.word	0x3000e0a8
3000ae58:	3000e0dc 	.word	0x3000e0dc
3000ae5c:	3000e0f4 	.word	0x3000e0f4
3000ae60:	3000e12c 	.word	0x3000e12c
3000ae64:	3000e164 	.word	0x3000e164
3000ae68:	3000e18c 	.word	0x3000e18c
3000ae6c:	3000e1b4 	.word	0x3000e1b4
3000ae70:	3000e230 	.word	0x3000e230
3000ae74:	3000e278 	.word	0x3000e278
3000ae78:	3000e290 	.word	0x3000e290
3000ae7c:	3000e2dc 	.word	0x3000e2dc
3000ae80:	3000e33c 	.word	0x3000e33c
3000ae84:	3000e384 	.word	0x3000e384
3000ae88:	3000e3e0 	.word	0x3000e3e0
3000ae8c:	3000ded0 	.word	0x3000ded0
3000ae90:	3000def8 	.word	0x3000def8
3000ae94:	3000e51c 	.word	0x3000e51c
3000ae98:	3000e4d0 	.word	0x3000e4d0
3000ae9c:	3000e4e8 	.word	0x3000e4e8
3000aea0:	3000e4a0 	.word	0x3000e4a0
3000aea4:	3000e548 	.word	0x3000e548
3000aea8:	3000e060 	.word	0x3000e060
3000aeac:	3000e090 	.word	0x3000e090
3000aeb0:	3000e1f4 	.word	0x3000e1f4
3000aeb4:	3000e218 	.word	0x3000e218

3000aeb8 <ChipInfo_Get>:
3000aeb8:	b570      	push	{r4, r5, r6, lr}
3000aeba:	4c12      	ldr	r4, [pc, #72]	; (3000af04 <ChipInfo_Get+0x4c>)
3000aebc:	7820      	ldrb	r0, [r4, #0]
3000aebe:	28ff      	cmp	r0, #255	; 0xff
3000aec0:	d107      	bne.n	3000aed2 <ChipInfo_Get+0x1a>
3000aec2:	4d11      	ldr	r5, [pc, #68]	; (3000af08 <ChipInfo_Get+0x50>)
3000aec4:	f895 3113 	ldrb.w	r3, [r5, #275]	; 0x113
3000aec8:	b123      	cbz	r3, 3000aed4 <ChipInfo_Get+0x1c>
3000aeca:	f895 0113 	ldrb.w	r0, [r5, #275]	; 0x113
3000aece:	b2c0      	uxtb	r0, r0
3000aed0:	7020      	strb	r0, [r4, #0]
3000aed2:	bd70      	pop	{r4, r5, r6, pc}
3000aed4:	4621      	mov	r1, r4
3000aed6:	f240 70ff 	movw	r0, #2047	; 0x7ff
3000aeda:	f7fc fa59 	bl	30007390 <OTP_Read8>
3000aede:	7820      	ldrb	r0, [r4, #0]
3000aee0:	28ff      	cmp	r0, #255	; 0xff
3000aee2:	d002      	beq.n	3000aeea <ChipInfo_Get+0x32>
3000aee4:	f885 0113 	strb.w	r0, [r5, #275]	; 0x113
3000aee8:	bd70      	pop	{r4, r5, r6, pc}
3000aeea:	4e08      	ldr	r6, [pc, #32]	; (3000af0c <ChipInfo_Get+0x54>)
3000aeec:	4d08      	ldr	r5, [pc, #32]	; (3000af10 <ChipInfo_Get+0x58>)
3000aeee:	4c09      	ldr	r4, [pc, #36]	; (3000af14 <ChipInfo_Get+0x5c>)
3000aef0:	462b      	mov	r3, r5
3000aef2:	2257      	movs	r2, #87	; 0x57
3000aef4:	4621      	mov	r1, r4
3000aef6:	2003      	movs	r0, #3
3000aef8:	f7ff fa0e 	bl	3000a318 <rtk_log_write>
3000aefc:	f242 7010 	movw	r0, #10000	; 0x2710
3000af00:	47b0      	blx	r6
3000af02:	e7f5      	b.n	3000aef0 <ChipInfo_Get+0x38>
3000af04:	3000f0c8 	.word	0x3000f0c8
3000af08:	23020000 	.word	0x23020000
3000af0c:	00009be5 	.word	0x00009be5
3000af10:	3000e5a8 	.word	0x3000e5a8
3000af14:	3000e5cc 	.word	0x3000e5cc

3000af18 <ChipInfo_BDNum>:
3000af18:	4b16      	ldr	r3, [pc, #88]	; (3000af74 <ChipInfo_BDNum+0x5c>)
3000af1a:	b510      	push	{r4, lr}
3000af1c:	4798      	blx	r3
3000af1e:	28ff      	cmp	r0, #255	; 0xff
3000af20:	d01e      	beq.n	3000af60 <ChipInfo_BDNum+0x48>
3000af22:	f3c0 1147 	ubfx	r1, r0, #5, #8
3000af26:	f000 0c1f 	and.w	ip, r0, #31
3000af2a:	4b13      	ldr	r3, [pc, #76]	; (3000af78 <ChipInfo_BDNum+0x60>)
3000af2c:	2200      	movs	r2, #0
3000af2e:	f240 34fe 	movw	r4, #1022	; 0x3fe
3000af32:	f64f 70ff 	movw	r0, #65535	; 0xffff
3000af36:	e004      	b.n	3000af42 <ChipInfo_BDNum+0x2a>
3000af38:	895c      	ldrh	r4, [r3, #10]
3000af3a:	4284      	cmp	r4, r0
3000af3c:	d008      	beq.n	3000af50 <ChipInfo_BDNum+0x38>
3000af3e:	f813 2f08 	ldrb.w	r2, [r3, #8]!
3000af42:	4291      	cmp	r1, r2
3000af44:	d1f8      	bne.n	3000af38 <ChipInfo_BDNum+0x20>
3000af46:	785a      	ldrb	r2, [r3, #1]
3000af48:	4562      	cmp	r2, ip
3000af4a:	d1f5      	bne.n	3000af38 <ChipInfo_BDNum+0x20>
3000af4c:	4620      	mov	r0, r4
3000af4e:	bd10      	pop	{r4, pc}
3000af50:	4b0a      	ldr	r3, [pc, #40]	; (3000af7c <ChipInfo_BDNum+0x64>)
3000af52:	2257      	movs	r2, #87	; 0x57
3000af54:	490a      	ldr	r1, [pc, #40]	; (3000af80 <ChipInfo_BDNum+0x68>)
3000af56:	2003      	movs	r0, #3
3000af58:	f7ff f9de 	bl	3000a318 <rtk_log_write>
3000af5c:	4620      	mov	r0, r4
3000af5e:	bd10      	pop	{r4, pc}
3000af60:	4b08      	ldr	r3, [pc, #32]	; (3000af84 <ChipInfo_BDNum+0x6c>)
3000af62:	2257      	movs	r2, #87	; 0x57
3000af64:	4906      	ldr	r1, [pc, #24]	; (3000af80 <ChipInfo_BDNum+0x68>)
3000af66:	2003      	movs	r0, #3
3000af68:	f64f 74ff 	movw	r4, #65535	; 0xffff
3000af6c:	f7ff f9d4 	bl	3000a318 <rtk_log_write>
3000af70:	e7ec      	b.n	3000af4c <ChipInfo_BDNum+0x34>
3000af72:	bf00      	nop
3000af74:	3000aeb9 	.word	0x3000aeb9
3000af78:	3000e6bc 	.word	0x3000e6bc
3000af7c:	3000e604 	.word	0x3000e604
3000af80:	3000e5cc 	.word	0x3000e5cc
3000af84:	3000e5d8 	.word	0x3000e5d8

3000af88 <ChipInfo_MCMInfo>:
3000af88:	b530      	push	{r4, r5, lr}
3000af8a:	4c1c      	ldr	r4, [pc, #112]	; (3000affc <ChipInfo_MCMInfo+0x74>)
3000af8c:	b085      	sub	sp, #20
3000af8e:	4b1c      	ldr	r3, [pc, #112]	; (3000b000 <ChipInfo_MCMInfo+0x78>)
3000af90:	4798      	blx	r3
3000af92:	7823      	ldrb	r3, [r4, #0]
3000af94:	f013 033f 	ands.w	r3, r3, #63	; 0x3f
3000af98:	d115      	bne.n	3000afc6 <ChipInfo_MCMInfo+0x3e>
3000af9a:	f240 32fe 	movw	r2, #1022	; 0x3fe
3000af9e:	4d19      	ldr	r5, [pc, #100]	; (3000b004 <ChipInfo_MCMInfo+0x7c>)
3000afa0:	f64f 7cff 	movw	ip, #65535	; 0xffff
3000afa4:	e005      	b.n	3000afb2 <ChipInfo_MCMInfo+0x2a>
3000afa6:	b2cb      	uxtb	r3, r1
3000afa8:	eb05 02c3 	add.w	r2, r5, r3, lsl #3
3000afac:	8852      	ldrh	r2, [r2, #2]
3000afae:	4562      	cmp	r2, ip
3000afb0:	d01b      	beq.n	3000afea <ChipInfo_MCMInfo+0x62>
3000afb2:	4290      	cmp	r0, r2
3000afb4:	f103 0101 	add.w	r1, r3, #1
3000afb8:	d1f5      	bne.n	3000afa6 <ChipInfo_MCMInfo+0x1e>
3000afba:	eb05 03c3 	add.w	r3, r5, r3, lsl #3
3000afbe:	685b      	ldr	r3, [r3, #4]
3000afc0:	6023      	str	r3, [r4, #0]
3000afc2:	069b      	lsls	r3, r3, #26
3000afc4:	d011      	beq.n	3000afea <ChipInfo_MCMInfo+0x62>
3000afc6:	6823      	ldr	r3, [r4, #0]
3000afc8:	2000      	movs	r0, #0
3000afca:	b2dc      	uxtb	r4, r3
3000afcc:	f3c3 2107 	ubfx	r1, r3, #8, #8
3000afd0:	f3c3 4207 	ubfx	r2, r3, #16, #8
3000afd4:	0e1b      	lsrs	r3, r3, #24
3000afd6:	f364 0007 	bfi	r0, r4, #0, #8
3000afda:	f361 200f 	bfi	r0, r1, #8, #8
3000afde:	f362 4017 	bfi	r0, r2, #16, #8
3000afe2:	f363 601f 	bfi	r0, r3, #24, #8
3000afe6:	b005      	add	sp, #20
3000afe8:	bd30      	pop	{r4, r5, pc}
3000afea:	9000      	str	r0, [sp, #0]
3000afec:	2257      	movs	r2, #87	; 0x57
3000afee:	4b06      	ldr	r3, [pc, #24]	; (3000b008 <ChipInfo_MCMInfo+0x80>)
3000aff0:	2003      	movs	r0, #3
3000aff2:	4906      	ldr	r1, [pc, #24]	; (3000b00c <ChipInfo_MCMInfo+0x84>)
3000aff4:	f7ff f990 	bl	3000a318 <rtk_log_write>
3000aff8:	e7e5      	b.n	3000afc6 <ChipInfo_MCMInfo+0x3e>
3000affa:	bf00      	nop
3000affc:	3000f9dc 	.word	0x3000f9dc
3000b000:	3000af19 	.word	0x3000af19
3000b004:	3000e6bc 	.word	0x3000e6bc
3000b008:	3000e618 	.word	0x3000e618
3000b00c:	3000e5cc 	.word	0x3000e5cc

3000b010 <ChipInfo_DDRType>:
3000b010:	b500      	push	{lr}
3000b012:	b083      	sub	sp, #12
3000b014:	4b06      	ldr	r3, [pc, #24]	; (3000b030 <ChipInfo_DDRType+0x20>)
3000b016:	4798      	blx	r3
3000b018:	9001      	str	r0, [sp, #4]
3000b01a:	f010 0002 	ands.w	r0, r0, #2
3000b01e:	d003      	beq.n	3000b028 <ChipInfo_DDRType+0x18>
3000b020:	f89d 0006 	ldrb.w	r0, [sp, #6]
3000b024:	f000 0007 	and.w	r0, r0, #7
3000b028:	b003      	add	sp, #12
3000b02a:	f85d fb04 	ldr.w	pc, [sp], #4
3000b02e:	bf00      	nop
3000b030:	3000af89 	.word	0x3000af89

3000b034 <ChipInfo_PsramBoundary>:
3000b034:	b530      	push	{r4, r5, lr}
3000b036:	4b1d      	ldr	r3, [pc, #116]	; (3000b0ac <ChipInfo_PsramBoundary+0x78>)
3000b038:	b085      	sub	sp, #20
3000b03a:	4798      	blx	r3
3000b03c:	0742      	lsls	r2, r0, #29
3000b03e:	4d1c      	ldr	r5, [pc, #112]	; (3000b0b0 <ChipInfo_PsramBoundary+0x7c>)
3000b040:	f3c0 44c4 	ubfx	r4, r0, #19, #5
3000b044:	9003      	str	r0, [sp, #12]
3000b046:	d519      	bpl.n	3000b07c <ChipInfo_PsramBoundary+0x48>
3000b048:	1f23      	subs	r3, r4, #4
3000b04a:	2b04      	cmp	r3, #4
3000b04c:	d819      	bhi.n	3000b082 <ChipInfo_PsramBoundary+0x4e>
3000b04e:	b194      	cbz	r4, 3000b076 <ChipInfo_PsramBoundary+0x42>
3000b050:	1e63      	subs	r3, r4, #1
3000b052:	f44f 0480 	mov.w	r4, #4194304	; 0x400000
3000b056:	409c      	lsls	r4, r3
3000b058:	bf48      	it	mi
3000b05a:	3407      	addmi	r4, #7
3000b05c:	10e4      	asrs	r4, r4, #3
3000b05e:	f104 44c0 	add.w	r4, r4, #1610612736	; 0x60000000
3000b062:	42ac      	cmp	r4, r5
3000b064:	d004      	beq.n	3000b070 <ChipInfo_PsramBoundary+0x3c>
3000b066:	4b13      	ldr	r3, [pc, #76]	; (3000b0b4 <ChipInfo_PsramBoundary+0x80>)
3000b068:	f8d3 3268 	ldr.w	r3, [r3, #616]	; 0x268
3000b06c:	009b      	lsls	r3, r3, #2
3000b06e:	d512      	bpl.n	3000b096 <ChipInfo_PsramBoundary+0x62>
3000b070:	4620      	mov	r0, r4
3000b072:	b005      	add	sp, #20
3000b074:	bd30      	pop	{r4, r5, pc}
3000b076:	f04f 44c0 	mov.w	r4, #1610612736	; 0x60000000
3000b07a:	e7f2      	b.n	3000b062 <ChipInfo_PsramBoundary+0x2e>
3000b07c:	1fe3      	subs	r3, r4, #7
3000b07e:	2b03      	cmp	r3, #3
3000b080:	d9e5      	bls.n	3000b04e <ChipInfo_PsramBoundary+0x1a>
3000b082:	4c0b      	ldr	r4, [pc, #44]	; (3000b0b0 <ChipInfo_PsramBoundary+0x7c>)
3000b084:	2245      	movs	r2, #69	; 0x45
3000b086:	4b0c      	ldr	r3, [pc, #48]	; (3000b0b8 <ChipInfo_PsramBoundary+0x84>)
3000b088:	2002      	movs	r0, #2
3000b08a:	490c      	ldr	r1, [pc, #48]	; (3000b0bc <ChipInfo_PsramBoundary+0x88>)
3000b08c:	f7ff f944 	bl	3000a318 <rtk_log_write>
3000b090:	4620      	mov	r0, r4
3000b092:	b005      	add	sp, #20
3000b094:	bd30      	pop	{r4, r5, pc}
3000b096:	4b0a      	ldr	r3, [pc, #40]	; (3000b0c0 <ChipInfo_PsramBoundary+0x8c>)
3000b098:	2257      	movs	r2, #87	; 0x57
3000b09a:	4908      	ldr	r1, [pc, #32]	; (3000b0bc <ChipInfo_PsramBoundary+0x88>)
3000b09c:	2003      	movs	r0, #3
3000b09e:	e9cd 5400 	strd	r5, r4, [sp]
3000b0a2:	f7ff f939 	bl	3000a318 <rtk_log_write>
3000b0a6:	4620      	mov	r0, r4
3000b0a8:	b005      	add	sp, #20
3000b0aa:	bd30      	pop	{r4, r5, pc}
3000b0ac:	3000af89 	.word	0x3000af89
3000b0b0:	60800000 	.word	0x60800000
3000b0b4:	42008000 	.word	0x42008000
3000b0b8:	3000e640 	.word	0x3000e640
3000b0bc:	3000e5cc 	.word	0x3000e5cc
3000b0c0:	3000e660 	.word	0x3000e660

3000b0c4 <print_unsigned_num>:
3000b0c4:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
3000b0c8:	b089      	sub	sp, #36	; 0x24
3000b0ca:	4606      	mov	r6, r0
3000b0cc:	460d      	mov	r5, r1
3000b0ce:	4690      	mov	r8, r2
3000b0d0:	f10d 0a0c 	add.w	sl, sp, #12
3000b0d4:	469b      	mov	fp, r3
3000b0d6:	2400      	movs	r4, #0
3000b0d8:	9f13      	ldr	r7, [sp, #76]	; 0x4c
3000b0da:	46d1      	mov	r9, sl
3000b0dc:	e00f      	b.n	3000b0fe <print_unsigned_num+0x3a>
3000b0de:	f10c 0357 	add.w	r3, ip, #87	; 0x57
3000b0e2:	2f01      	cmp	r7, #1
3000b0e4:	b2db      	uxtb	r3, r3
3000b0e6:	d018      	beq.n	3000b11a <print_unsigned_num+0x56>
3000b0e8:	4546      	cmp	r6, r8
3000b0ea:	4606      	mov	r6, r0
3000b0ec:	f104 0201 	add.w	r2, r4, #1
3000b0f0:	f809 3b01 	strb.w	r3, [r9], #1
3000b0f4:	f175 0000 	sbcs.w	r0, r5, #0
3000b0f8:	460d      	mov	r5, r1
3000b0fa:	d312      	bcc.n	3000b122 <print_unsigned_num+0x5e>
3000b0fc:	4614      	mov	r4, r2
3000b0fe:	4630      	mov	r0, r6
3000b100:	4629      	mov	r1, r5
3000b102:	4642      	mov	r2, r8
3000b104:	2300      	movs	r3, #0
3000b106:	f001 f9b9 	bl	3000c47c <__aeabi_uldivmod>
3000b10a:	2a09      	cmp	r2, #9
3000b10c:	fa5f fc82 	uxtb.w	ip, r2
3000b110:	f10c 0330 	add.w	r3, ip, #48	; 0x30
3000b114:	d8e3      	bhi.n	3000b0de <print_unsigned_num+0x1a>
3000b116:	b2db      	uxtb	r3, r3
3000b118:	e7e6      	b.n	3000b0e8 <print_unsigned_num+0x24>
3000b11a:	f10c 0337 	add.w	r3, ip, #55	; 0x37
3000b11e:	b2db      	uxtb	r3, r3
3000b120:	e7e2      	b.n	3000b0e8 <print_unsigned_num+0x24>
3000b122:	4617      	mov	r7, r2
3000b124:	9a14      	ldr	r2, [sp, #80]	; 0x50
3000b126:	b192      	cbz	r2, 3000b14e <print_unsigned_num+0x8a>
3000b128:	9a12      	ldr	r2, [sp, #72]	; 0x48
3000b12a:	f1bb 0f30 	cmp.w	fp, #48	; 0x30
3000b12e:	f102 32ff 	add.w	r2, r2, #4294967295	; 0xffffffff
3000b132:	9212      	str	r2, [sp, #72]	; 0x48
3000b134:	d136      	bne.n	3000b1a4 <print_unsigned_num+0xe0>
3000b136:	202d      	movs	r0, #45	; 0x2d
3000b138:	4d1f      	ldr	r5, [pc, #124]	; (3000b1b8 <print_unsigned_num+0xf4>)
3000b13a:	9301      	str	r3, [sp, #4]
3000b13c:	47a8      	blx	r5
3000b13e:	9b12      	ldr	r3, [sp, #72]	; 0x48
3000b140:	2b00      	cmp	r3, #0
3000b142:	9b01      	ldr	r3, [sp, #4]
3000b144:	dd1f      	ble.n	3000b186 <print_unsigned_num+0xc2>
3000b146:	9a12      	ldr	r2, [sp, #72]	; 0x48
3000b148:	4297      	cmp	r7, r2
3000b14a:	db08      	blt.n	3000b15e <print_unsigned_num+0x9a>
3000b14c:	e018      	b.n	3000b180 <print_unsigned_num+0xbc>
3000b14e:	9a12      	ldr	r2, [sp, #72]	; 0x48
3000b150:	2a00      	cmp	r2, #0
3000b152:	dd25      	ble.n	3000b1a0 <print_unsigned_num+0xdc>
3000b154:	9912      	ldr	r1, [sp, #72]	; 0x48
3000b156:	9a14      	ldr	r2, [sp, #80]	; 0x50
3000b158:	42b9      	cmp	r1, r7
3000b15a:	4d17      	ldr	r5, [pc, #92]	; (3000b1b8 <print_unsigned_num+0xf4>)
3000b15c:	dd0d      	ble.n	3000b17a <print_unsigned_num+0xb6>
3000b15e:	9e12      	ldr	r6, [sp, #72]	; 0x48
3000b160:	4698      	mov	r8, r3
3000b162:	3e01      	subs	r6, #1
3000b164:	4658      	mov	r0, fp
3000b166:	47a8      	blx	r5
3000b168:	42b7      	cmp	r7, r6
3000b16a:	d1fa      	bne.n	3000b162 <print_unsigned_num+0x9e>
3000b16c:	9912      	ldr	r1, [sp, #72]	; 0x48
3000b16e:	43fa      	mvns	r2, r7
3000b170:	4643      	mov	r3, r8
3000b172:	440a      	add	r2, r1
3000b174:	9914      	ldr	r1, [sp, #80]	; 0x50
3000b176:	3101      	adds	r1, #1
3000b178:	440a      	add	r2, r1
3000b17a:	9914      	ldr	r1, [sp, #80]	; 0x50
3000b17c:	9214      	str	r2, [sp, #80]	; 0x50
3000b17e:	b111      	cbz	r1, 3000b186 <print_unsigned_num+0xc2>
3000b180:	f01b 0fdf 	tst.w	fp, #223	; 0xdf
3000b184:	d013      	beq.n	3000b1ae <print_unsigned_num+0xea>
3000b186:	4454      	add	r4, sl
3000b188:	4618      	mov	r0, r3
3000b18a:	e001      	b.n	3000b190 <print_unsigned_num+0xcc>
3000b18c:	f814 0d01 	ldrb.w	r0, [r4, #-1]!
3000b190:	47a8      	blx	r5
3000b192:	45a2      	cmp	sl, r4
3000b194:	d1fa      	bne.n	3000b18c <print_unsigned_num+0xc8>
3000b196:	9b14      	ldr	r3, [sp, #80]	; 0x50
3000b198:	19d8      	adds	r0, r3, r7
3000b19a:	b009      	add	sp, #36	; 0x24
3000b19c:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
3000b1a0:	4d05      	ldr	r5, [pc, #20]	; (3000b1b8 <print_unsigned_num+0xf4>)
3000b1a2:	e7f0      	b.n	3000b186 <print_unsigned_num+0xc2>
3000b1a4:	9a12      	ldr	r2, [sp, #72]	; 0x48
3000b1a6:	2a00      	cmp	r2, #0
3000b1a8:	dcd4      	bgt.n	3000b154 <print_unsigned_num+0x90>
3000b1aa:	4d03      	ldr	r5, [pc, #12]	; (3000b1b8 <print_unsigned_num+0xf4>)
3000b1ac:	e7e8      	b.n	3000b180 <print_unsigned_num+0xbc>
3000b1ae:	202d      	movs	r0, #45	; 0x2d
3000b1b0:	9301      	str	r3, [sp, #4]
3000b1b2:	47a8      	blx	r5
3000b1b4:	9b01      	ldr	r3, [sp, #4]
3000b1b6:	e7e6      	b.n	3000b186 <print_unsigned_num+0xc2>
3000b1b8:	0000abb5 	.word	0x0000abb5

3000b1bc <LOGUART_INTConfig>:
3000b1bc:	b570      	push	{r4, r5, r6, lr}
3000b1be:	460c      	mov	r4, r1
3000b1c0:	4b10      	ldr	r3, [pc, #64]	; (3000b204 <LOGUART_INTConfig+0x48>)
3000b1c2:	4615      	mov	r5, r2
3000b1c4:	4798      	blx	r3
3000b1c6:	f404 63e0 	and.w	r3, r4, #1792	; 0x700
3000b1ca:	f404 62a0 	and.w	r2, r4, #1280	; 0x500
3000b1ce:	f5b3 7f00 	cmp.w	r3, #512	; 0x200
3000b1d2:	bf18      	it	ne
3000b1d4:	f5b2 7f80 	cmpne.w	r2, #256	; 0x100
3000b1d8:	d002      	beq.n	3000b1e0 <LOGUART_INTConfig+0x24>
3000b1da:	f5b3 6f80 	cmp.w	r3, #1024	; 0x400
3000b1de:	d101      	bne.n	3000b1e4 <LOGUART_INTConfig+0x28>
3000b1e0:	f420 60e0 	bic.w	r0, r0, #1792	; 0x700
3000b1e4:	2d01      	cmp	r5, #1
3000b1e6:	d007      	beq.n	3000b1f8 <LOGUART_INTConfig+0x3c>
3000b1e8:	f424 64e0 	bic.w	r4, r4, #1792	; 0x700
3000b1ec:	4b06      	ldr	r3, [pc, #24]	; (3000b208 <LOGUART_INTConfig+0x4c>)
3000b1ee:	ea20 0004 	bic.w	r0, r0, r4
3000b1f2:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
3000b1f6:	4718      	bx	r3
3000b1f8:	4320      	orrs	r0, r4
3000b1fa:	4b03      	ldr	r3, [pc, #12]	; (3000b208 <LOGUART_INTConfig+0x4c>)
3000b1fc:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
3000b200:	4718      	bx	r3
3000b202:	bf00      	nop
3000b204:	0000ac55 	.word	0x0000ac55
3000b208:	0000ac61 	.word	0x0000ac61

3000b20c <LOGUART_WaitTxComplete>:
3000b20c:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
3000b20e:	4c07      	ldr	r4, [pc, #28]	; (3000b22c <LOGUART_WaitTxComplete+0x20>)
3000b210:	4d07      	ldr	r5, [pc, #28]	; (3000b230 <LOGUART_WaitTxComplete+0x24>)
3000b212:	6963      	ldr	r3, [r4, #20]
3000b214:	4e07      	ldr	r6, [pc, #28]	; (3000b234 <LOGUART_WaitTxComplete+0x28>)
3000b216:	402b      	ands	r3, r5
3000b218:	42b3      	cmp	r3, r6
3000b21a:	d006      	beq.n	3000b22a <LOGUART_WaitTxComplete+0x1e>
3000b21c:	4f06      	ldr	r7, [pc, #24]	; (3000b238 <LOGUART_WaitTxComplete+0x2c>)
3000b21e:	2001      	movs	r0, #1
3000b220:	47b8      	blx	r7
3000b222:	6963      	ldr	r3, [r4, #20]
3000b224:	402b      	ands	r3, r5
3000b226:	42b3      	cmp	r3, r6
3000b228:	d1f9      	bne.n	3000b21e <LOGUART_WaitTxComplete+0x12>
3000b22a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
3000b22c:	4200c000 	.word	0x4200c000
3000b230:	000f0140 	.word	0x000f0140
3000b234:	000f0040 	.word	0x000f0040
3000b238:	00009b2d 	.word	0x00009b2d

3000b23c <CPU_ClkGet>:
3000b23c:	b570      	push	{r4, r5, r6, lr}
3000b23e:	4d1e      	ldr	r5, [pc, #120]	; (3000b2b8 <CPU_ClkGet+0x7c>)
3000b240:	4b1e      	ldr	r3, [pc, #120]	; (3000b2bc <CPU_ClkGet+0x80>)
3000b242:	f8d5 6224 	ldr.w	r6, [r5, #548]	; 0x224
3000b246:	f8d5 427c 	ldr.w	r4, [r5, #636]	; 0x27c
3000b24a:	f3c4 1481 	ubfx	r4, r4, #6, #2
3000b24e:	4798      	blx	r3
3000b250:	b92c      	cbnz	r4, 3000b25e <CPU_ClkGet+0x22>
3000b252:	2801      	cmp	r0, #1
3000b254:	d007      	beq.n	3000b266 <CPU_ClkGet+0x2a>
3000b256:	07b2      	lsls	r2, r6, #30
3000b258:	d407      	bmi.n	3000b26a <CPU_ClkGet+0x2e>
3000b25a:	4819      	ldr	r0, [pc, #100]	; (3000b2c0 <CPU_ClkGet+0x84>)
3000b25c:	bd70      	pop	{r4, r5, r6, pc}
3000b25e:	2c01      	cmp	r4, #1
3000b260:	d007      	beq.n	3000b272 <CPU_ClkGet+0x36>
3000b262:	2000      	movs	r0, #0
3000b264:	bd70      	pop	{r4, r5, r6, pc}
3000b266:	4817      	ldr	r0, [pc, #92]	; (3000b2c4 <CPU_ClkGet+0x88>)
3000b268:	bd70      	pop	{r4, r5, r6, pc}
3000b26a:	4b17      	ldr	r3, [pc, #92]	; (3000b2c8 <CPU_ClkGet+0x8c>)
3000b26c:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
3000b270:	4718      	bx	r3
3000b272:	2801      	cmp	r0, #1
3000b274:	d011      	beq.n	3000b29a <CPU_ClkGet+0x5e>
3000b276:	05f3      	lsls	r3, r6, #23
3000b278:	d51c      	bpl.n	3000b2b4 <CPU_ClkGet+0x78>
3000b27a:	4914      	ldr	r1, [pc, #80]	; (3000b2cc <CPU_ClkGet+0x90>)
3000b27c:	f8d5 3228 	ldr.w	r3, [r5, #552]	; 0x228
3000b280:	6c48      	ldr	r0, [r1, #68]	; 0x44
3000b282:	4a13      	ldr	r2, [pc, #76]	; (3000b2d0 <CPU_ClkGet+0x94>)
3000b284:	f003 0307 	and.w	r3, r3, #7
3000b288:	f3c0 1045 	ubfx	r0, r0, #5, #6
3000b28c:	3301      	adds	r3, #1
3000b28e:	3002      	adds	r0, #2
3000b290:	fb02 f000 	mul.w	r0, r2, r0
3000b294:	fbb0 f0f3 	udiv	r0, r0, r3
3000b298:	bd70      	pop	{r4, r5, r6, pc}
3000b29a:	f8d5 326c 	ldr.w	r3, [r5, #620]	; 0x26c
3000b29e:	f413 0f7f 	tst.w	r3, #16711680	; 0xff0000
3000b2a2:	f3c3 4007 	ubfx	r0, r3, #16, #8
3000b2a6:	d003      	beq.n	3000b2b0 <CPU_ClkGet+0x74>
3000b2a8:	4b0a      	ldr	r3, [pc, #40]	; (3000b2d4 <CPU_ClkGet+0x98>)
3000b2aa:	fb03 f000 	mul.w	r0, r3, r0
3000b2ae:	bd70      	pop	{r4, r5, r6, pc}
3000b2b0:	4809      	ldr	r0, [pc, #36]	; (3000b2d8 <CPU_ClkGet+0x9c>)
3000b2b2:	bd70      	pop	{r4, r5, r6, pc}
3000b2b4:	4806      	ldr	r0, [pc, #24]	; (3000b2d0 <CPU_ClkGet+0x94>)
3000b2b6:	bd70      	pop	{r4, r5, r6, pc}
3000b2b8:	42008000 	.word	0x42008000
3000b2bc:	0000c0f9 	.word	0x0000c0f9
3000b2c0:	003d0900 	.word	0x003d0900
3000b2c4:	01312d00 	.word	0x01312d00
3000b2c8:	000099f5 	.word	0x000099f5
3000b2cc:	42008800 	.word	0x42008800
3000b2d0:	02625a00 	.word	0x02625a00
3000b2d4:	000f4240 	.word	0x000f4240
3000b2d8:	02faf080 	.word	0x02faf080

3000b2dc <RSIP_IV_Set>:
3000b2dc:	4b05      	ldr	r3, [pc, #20]	; (3000b2f4 <RSIP_IV_Set+0x18>)
3000b2de:	eb03 00c0 	add.w	r0, r3, r0, lsl #3
3000b2e2:	680b      	ldr	r3, [r1, #0]
3000b2e4:	ba1b      	rev	r3, r3
3000b2e6:	f8c0 3084 	str.w	r3, [r0, #132]	; 0x84
3000b2ea:	684b      	ldr	r3, [r1, #4]
3000b2ec:	ba1b      	rev	r3, r3
3000b2ee:	f8c0 3080 	str.w	r3, [r0, #128]	; 0x80
3000b2f2:	4770      	bx	lr
3000b2f4:	42008c00 	.word	0x42008c00

3000b2f8 <RSIP_OTF_Cmd>:
3000b2f8:	4a04      	ldr	r2, [pc, #16]	; (3000b30c <RSIP_OTF_Cmd+0x14>)
3000b2fa:	2801      	cmp	r0, #1
3000b2fc:	6953      	ldr	r3, [r2, #20]
3000b2fe:	bf0c      	ite	eq
3000b300:	f443 7380 	orreq.w	r3, r3, #256	; 0x100
3000b304:	f423 7380 	bicne.w	r3, r3, #256	; 0x100
3000b308:	6153      	str	r3, [r2, #20]
3000b30a:	4770      	bx	lr
3000b30c:	42008c00 	.word	0x42008c00

3000b310 <RSIP_MMU_Config>:
3000b310:	eb00 0040 	add.w	r0, r0, r0, lsl #1
3000b314:	311f      	adds	r1, #31
3000b316:	321f      	adds	r2, #31
3000b318:	331f      	adds	r3, #31
3000b31a:	0080      	lsls	r0, r0, #2
3000b31c:	0949      	lsrs	r1, r1, #5
3000b31e:	0952      	lsrs	r2, r2, #5
3000b320:	095b      	lsrs	r3, r3, #5
3000b322:	0249      	lsls	r1, r1, #9
3000b324:	0252      	lsls	r2, r2, #9
3000b326:	025b      	lsls	r3, r3, #9
3000b328:	b410      	push	{r4}
3000b32a:	4c08      	ldr	r4, [pc, #32]	; (3000b34c <RSIP_MMU_Config+0x3c>)
3000b32c:	4404      	add	r4, r0
3000b32e:	f8d4 0108 	ldr.w	r0, [r4, #264]	; 0x108
3000b332:	f020 0001 	bic.w	r0, r0, #1
3000b336:	f8c4 0108 	str.w	r0, [r4, #264]	; 0x108
3000b33a:	f8c4 1100 	str.w	r1, [r4, #256]	; 0x100
3000b33e:	f8c4 2104 	str.w	r2, [r4, #260]	; 0x104
3000b342:	f8c4 3108 	str.w	r3, [r4, #264]	; 0x108
3000b346:	f85d 4b04 	ldr.w	r4, [sp], #4
3000b34a:	4770      	bx	lr
3000b34c:	42008c00 	.word	0x42008c00

3000b350 <RSIP_MMU_Cmd>:
3000b350:	eb00 0040 	add.w	r0, r0, r0, lsl #1
3000b354:	4b07      	ldr	r3, [pc, #28]	; (3000b374 <RSIP_MMU_Cmd+0x24>)
3000b356:	0080      	lsls	r0, r0, #2
3000b358:	4403      	add	r3, r0
3000b35a:	f8d3 2108 	ldr.w	r2, [r3, #264]	; 0x108
3000b35e:	b121      	cbz	r1, 3000b36a <RSIP_MMU_Cmd+0x1a>
3000b360:	f042 0201 	orr.w	r2, r2, #1
3000b364:	f8c3 2108 	str.w	r2, [r3, #264]	; 0x108
3000b368:	4770      	bx	lr
3000b36a:	f022 0201 	bic.w	r2, r2, #1
3000b36e:	f8c3 2108 	str.w	r2, [r3, #264]	; 0x108
3000b372:	4770      	bx	lr
3000b374:	42008c00 	.word	0x42008c00

3000b378 <RSIP_MMU_Cache_Clean>:
3000b378:	4a02      	ldr	r2, [pc, #8]	; (3000b384 <RSIP_MMU_Cache_Clean+0xc>)
3000b37a:	6993      	ldr	r3, [r2, #24]
3000b37c:	f043 0304 	orr.w	r3, r3, #4
3000b380:	6193      	str	r3, [r2, #24]
3000b382:	4770      	bx	lr
3000b384:	42008c00 	.word	0x42008c00

3000b388 <NAND_RxData>:
3000b388:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
3000b38c:	460c      	mov	r4, r1
3000b38e:	4616      	mov	r6, r2
3000b390:	461f      	mov	r7, r3
3000b392:	2800      	cmp	r0, #0
3000b394:	f000 8087 	beq.w	3000b4a6 <NAND_RxData+0x11e>
3000b398:	4605      	mov	r5, r0
3000b39a:	f8df 9118 	ldr.w	r9, [pc, #280]	; 3000b4b4 <NAND_RxData+0x12c>
3000b39e:	2000      	movs	r0, #0
3000b3a0:	f8df 8114 	ldr.w	r8, [pc, #276]	; 3000b4b8 <NAND_RxData+0x130>
3000b3a4:	47c0      	blx	r8
3000b3a6:	f04f 4388 	mov.w	r3, #1140850688	; 0x44000000
3000b3aa:	f026 427f 	bic.w	r2, r6, #4278190080	; 0xff000000
3000b3ae:	2000      	movs	r0, #0
3000b3b0:	6819      	ldr	r1, [r3, #0]
3000b3b2:	f041 4100 	orr.w	r1, r1, #2147483648	; 0x80000000
3000b3b6:	6019      	str	r1, [r3, #0]
3000b3b8:	6819      	ldr	r1, [r3, #0]
3000b3ba:	f441 7140 	orr.w	r1, r1, #768	; 0x300
3000b3be:	6019      	str	r1, [r3, #0]
3000b3c0:	605a      	str	r2, [r3, #4]
3000b3c2:	f8c3 0130 	str.w	r0, [r3, #304]	; 0x130
3000b3c6:	f899 c062 	ldrb.w	ip, [r9, #98]	; 0x62
3000b3ca:	f8d3 1118 	ldr.w	r1, [r3, #280]	; 0x118
3000b3ce:	ea4f 420c 	mov.w	r2, ip, lsl #16
3000b3d2:	f1bc 0f03 	cmp.w	ip, #3
3000b3d6:	f421 2170 	bic.w	r1, r1, #983040	; 0xf0000
3000b3da:	f402 2270 	and.w	r2, r2, #983040	; 0xf0000
3000b3de:	ea42 0201 	orr.w	r2, r2, r1
3000b3e2:	f8c3 2118 	str.w	r2, [r3, #280]	; 0x118
3000b3e6:	f04f 0201 	mov.w	r2, #1
3000b3ea:	611a      	str	r2, [r3, #16]
3000b3ec:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
3000b3f0:	f883 5060 	strb.w	r5, [r3, #96]	; 0x60
3000b3f4:	bf08      	it	eq
3000b3f6:	f883 0060 	strbeq.w	r0, [r3, #96]	; 0x60
3000b3fa:	f3c4 2307 	ubfx	r3, r4, #8, #8
3000b3fe:	b2e4      	uxtb	r4, r4
3000b400:	f882 3060 	strb.w	r3, [r2, #96]	; 0x60
3000b404:	2301      	movs	r3, #1
3000b406:	f882 4060 	strb.w	r4, [r2, #96]	; 0x60
3000b40a:	f017 0403 	ands.w	r4, r7, #3
3000b40e:	6093      	str	r3, [r2, #8]
3000b410:	d137      	bne.n	3000b482 <NAND_RxData+0xfa>
3000b412:	463b      	mov	r3, r7
3000b414:	f04f 4e88 	mov.w	lr, #1140850688	; 0x44000000
3000b418:	42b4      	cmp	r4, r6
3000b41a:	d216      	bcs.n	3000b44a <NAND_RxData+0xc2>
3000b41c:	f8de 2024 	ldr.w	r2, [lr, #36]	; 0x24
3000b420:	f012 0f3c 	tst.w	r2, #60	; 0x3c
3000b424:	f3c2 0583 	ubfx	r5, r2, #2, #4
3000b428:	d00c      	beq.n	3000b444 <NAND_RxData+0xbc>
3000b42a:	00ad      	lsls	r5, r5, #2
3000b42c:	1f19      	subs	r1, r3, #4
3000b42e:	f1a5 0c04 	sub.w	ip, r5, #4
3000b432:	449c      	add	ip, r3
3000b434:	f8de 0060 	ldr.w	r0, [lr, #96]	; 0x60
3000b438:	f841 0f04 	str.w	r0, [r1, #4]!
3000b43c:	458c      	cmp	ip, r1
3000b43e:	d1f9      	bne.n	3000b434 <NAND_RxData+0xac>
3000b440:	442b      	add	r3, r5
3000b442:	442c      	add	r4, r5
3000b444:	1b32      	subs	r2, r6, r4
3000b446:	2a03      	cmp	r2, #3
3000b448:	d8e6      	bhi.n	3000b418 <NAND_RxData+0x90>
3000b44a:	1e63      	subs	r3, r4, #1
3000b44c:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
3000b450:	441f      	add	r7, r3
3000b452:	e002      	b.n	3000b45a <NAND_RxData+0xd2>
3000b454:	6a93      	ldr	r3, [r2, #40]	; 0x28
3000b456:	071b      	lsls	r3, r3, #28
3000b458:	d40d      	bmi.n	3000b476 <NAND_RxData+0xee>
3000b45a:	42b4      	cmp	r4, r6
3000b45c:	d3fa      	bcc.n	3000b454 <NAND_RxData+0xcc>
3000b45e:	2004      	movs	r0, #4
3000b460:	47c0      	blx	r8
3000b462:	2000      	movs	r0, #0
3000b464:	47c0      	blx	r8
3000b466:	f04f 4288 	mov.w	r2, #1140850688	; 0x44000000
3000b46a:	6813      	ldr	r3, [r2, #0]
3000b46c:	f023 4300 	bic.w	r3, r3, #2147483648	; 0x80000000
3000b470:	6013      	str	r3, [r2, #0]
3000b472:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
3000b476:	f892 3060 	ldrb.w	r3, [r2, #96]	; 0x60
3000b47a:	3401      	adds	r4, #1
3000b47c:	f807 3f01 	strb.w	r3, [r7, #1]!
3000b480:	e7eb      	b.n	3000b45a <NAND_RxData+0xd2>
3000b482:	f1c4 0004 	rsb	r0, r4, #4
3000b486:	463b      	mov	r3, r7
3000b488:	2400      	movs	r4, #0
3000b48a:	e009      	b.n	3000b4a0 <NAND_RxData+0x118>
3000b48c:	6a91      	ldr	r1, [r2, #40]	; 0x28
3000b48e:	0709      	lsls	r1, r1, #28
3000b490:	d504      	bpl.n	3000b49c <NAND_RxData+0x114>
3000b492:	f892 1060 	ldrb.w	r1, [r2, #96]	; 0x60
3000b496:	3401      	adds	r4, #1
3000b498:	7019      	strb	r1, [r3, #0]
3000b49a:	193b      	adds	r3, r7, r4
3000b49c:	42a0      	cmp	r0, r4
3000b49e:	d007      	beq.n	3000b4b0 <NAND_RxData+0x128>
3000b4a0:	42b4      	cmp	r4, r6
3000b4a2:	d3f3      	bcc.n	3000b48c <NAND_RxData+0x104>
3000b4a4:	e7d1      	b.n	3000b44a <NAND_RxData+0xc2>
3000b4a6:	f8df 900c 	ldr.w	r9, [pc, #12]	; 3000b4b4 <NAND_RxData+0x12c>
3000b4aa:	f899 5008 	ldrb.w	r5, [r9, #8]
3000b4ae:	e776      	b.n	3000b39e <NAND_RxData+0x16>
3000b4b0:	4604      	mov	r4, r0
3000b4b2:	e7af      	b.n	3000b414 <NAND_RxData+0x8c>
3000b4b4:	2001c01c 	.word	0x2001c01c
3000b4b8:	0000b9c1 	.word	0x0000b9c1

3000b4bc <NAND_Page_Read>:
3000b4bc:	b530      	push	{r4, r5, lr}
3000b4be:	b085      	sub	sp, #20
3000b4c0:	4c07      	ldr	r4, [pc, #28]	; (3000b4e0 <NAND_Page_Read+0x24>)
3000b4c2:	4d08      	ldr	r5, [pc, #32]	; (3000b4e4 <NAND_Page_Read+0x28>)
3000b4c4:	9301      	str	r3, [sp, #4]
3000b4c6:	e9cd 2102 	strd	r2, r1, [sp, #8]
3000b4ca:	47a0      	blx	r4
3000b4cc:	4604      	mov	r4, r0
3000b4ce:	9903      	ldr	r1, [sp, #12]
3000b4d0:	7a28      	ldrb	r0, [r5, #8]
3000b4d2:	4d05      	ldr	r5, [pc, #20]	; (3000b4e8 <NAND_Page_Read+0x2c>)
3000b4d4:	e9dd 3201 	ldrd	r3, r2, [sp, #4]
3000b4d8:	47a8      	blx	r5
3000b4da:	4620      	mov	r0, r4
3000b4dc:	b005      	add	sp, #20
3000b4de:	bd30      	pop	{r4, r5, pc}
3000b4e0:	0000bd85 	.word	0x0000bd85
3000b4e4:	2001c01c 	.word	0x2001c01c
3000b4e8:	3000b389 	.word	0x3000b389

3000b4ec <SYSCFG_BootFromNor>:
3000b4ec:	4b0e      	ldr	r3, [pc, #56]	; (3000b528 <SYSCFG_BootFromNor+0x3c>)
3000b4ee:	b510      	push	{r4, lr}
3000b4f0:	f8d3 227c 	ldr.w	r2, [r3, #636]	; 0x27c
3000b4f4:	f8d3 3280 	ldr.w	r3, [r3, #640]	; 0x280
3000b4f8:	0392      	lsls	r2, r2, #14
3000b4fa:	d50a      	bpl.n	3000b512 <SYSCFG_BootFromNor+0x26>
3000b4fc:	f003 4370 	and.w	r3, r3, #4026531840	; 0xf0000000
3000b500:	f1b3 4f60 	cmp.w	r3, #3758096384	; 0xe0000000
3000b504:	d105      	bne.n	3000b512 <SYSCFG_BootFromNor+0x26>
3000b506:	4b09      	ldr	r3, [pc, #36]	; (3000b52c <SYSCFG_BootFromNor+0x40>)
3000b508:	4798      	blx	r3
3000b50a:	3800      	subs	r0, #0
3000b50c:	bf18      	it	ne
3000b50e:	2001      	movne	r0, #1
3000b510:	bd10      	pop	{r4, pc}
3000b512:	4c07      	ldr	r4, [pc, #28]	; (3000b530 <SYSCFG_BootFromNor+0x44>)
3000b514:	47a0      	blx	r4
3000b516:	2802      	cmp	r0, #2
3000b518:	d004      	beq.n	3000b524 <SYSCFG_BootFromNor+0x38>
3000b51a:	47a0      	blx	r4
3000b51c:	2801      	cmp	r0, #1
3000b51e:	d1f2      	bne.n	3000b506 <SYSCFG_BootFromNor+0x1a>
3000b520:	2000      	movs	r0, #0
3000b522:	bd10      	pop	{r4, pc}
3000b524:	2001      	movs	r0, #1
3000b526:	bd10      	pop	{r4, pc}
3000b528:	42008000 	.word	0x42008000
3000b52c:	0000c0bd 	.word	0x0000c0bd
3000b530:	0000c1a9 	.word	0x0000c1a9

3000b534 <SYSCFG_RLVersion>:
3000b534:	4b07      	ldr	r3, [pc, #28]	; (3000b554 <SYSCFG_RLVersion+0x20>)
3000b536:	f8d3 2274 	ldr.w	r2, [r3, #628]	; 0x274
3000b53a:	f022 4270 	bic.w	r2, r2, #4026531840	; 0xf0000000
3000b53e:	f042 4120 	orr.w	r1, r2, #2684354560	; 0xa0000000
3000b542:	f8c3 1274 	str.w	r1, [r3, #628]	; 0x274
3000b546:	f8d3 0274 	ldr.w	r0, [r3, #628]	; 0x274
3000b54a:	f8c3 2274 	str.w	r2, [r3, #628]	; 0x274
3000b54e:	f3c0 4003 	ubfx	r0, r0, #16, #4
3000b552:	4770      	bx	lr
3000b554:	42008000 	.word	0x42008000

3000b558 <DiagVprintf>:
3000b558:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
3000b55c:	4604      	mov	r4, r0
3000b55e:	7800      	ldrb	r0, [r0, #0]
3000b560:	b089      	sub	sp, #36	; 0x24
3000b562:	2800      	cmp	r0, #0
3000b564:	f000 82ab 	beq.w	3000babe <DiagVprintf+0x566>
3000b568:	f04f 0a00 	mov.w	sl, #0
3000b56c:	4688      	mov	r8, r1
3000b56e:	4f89      	ldr	r7, [pc, #548]	; (3000b794 <DiagVprintf+0x23c>)
3000b570:	4655      	mov	r5, sl
3000b572:	f8cd a010 	str.w	sl, [sp, #16]
3000b576:	e005      	b.n	3000b584 <DiagVprintf+0x2c>
3000b578:	3501      	adds	r5, #1
3000b57a:	47b8      	blx	r7
3000b57c:	7820      	ldrb	r0, [r4, #0]
3000b57e:	2800      	cmp	r0, #0
3000b580:	f000 808b 	beq.w	3000b69a <DiagVprintf+0x142>
3000b584:	2825      	cmp	r0, #37	; 0x25
3000b586:	f104 0401 	add.w	r4, r4, #1
3000b58a:	d1f5      	bne.n	3000b578 <DiagVprintf+0x20>
3000b58c:	f04f 0a00 	mov.w	sl, #0
3000b590:	4656      	mov	r6, sl
3000b592:	46d1      	mov	r9, sl
3000b594:	46d4      	mov	ip, sl
3000b596:	7823      	ldrb	r3, [r4, #0]
3000b598:	2b7a      	cmp	r3, #122	; 0x7a
3000b59a:	d87c      	bhi.n	3000b696 <DiagVprintf+0x13e>
3000b59c:	e8df f013 	tbh	[pc, r3, lsl #1]
3000b5a0:	007b00af 	.word	0x007b00af
3000b5a4:	007b007b 	.word	0x007b007b
3000b5a8:	007b007b 	.word	0x007b007b
3000b5ac:	007b007b 	.word	0x007b007b
3000b5b0:	007b007b 	.word	0x007b007b
3000b5b4:	007b007b 	.word	0x007b007b
3000b5b8:	007b007b 	.word	0x007b007b
3000b5bc:	007b007b 	.word	0x007b007b
3000b5c0:	007b007b 	.word	0x007b007b
3000b5c4:	007b007b 	.word	0x007b007b
3000b5c8:	007b007b 	.word	0x007b007b
3000b5cc:	007b007b 	.word	0x007b007b
3000b5d0:	007b007b 	.word	0x007b007b
3000b5d4:	007b007b 	.word	0x007b007b
3000b5d8:	007b007b 	.word	0x007b007b
3000b5dc:	007b007b 	.word	0x007b007b
3000b5e0:	007b00c4 	.word	0x007b00c4
3000b5e4:	007b007b 	.word	0x007b007b
3000b5e8:	01af007b 	.word	0x01af007b
3000b5ec:	007b007b 	.word	0x007b007b
3000b5f0:	007b007b 	.word	0x007b007b
3000b5f4:	007b007b 	.word	0x007b007b
3000b5f8:	01ab007b 	.word	0x01ab007b
3000b5fc:	007b007b 	.word	0x007b007b
3000b600:	00b100c4 	.word	0x00b100c4
3000b604:	00b100b1 	.word	0x00b100b1
3000b608:	00b100b1 	.word	0x00b100b1
3000b60c:	00b100b1 	.word	0x00b100b1
3000b610:	00b100b1 	.word	0x00b100b1
3000b614:	007b007b 	.word	0x007b007b
3000b618:	007b007b 	.word	0x007b007b
3000b61c:	007b007b 	.word	0x007b007b
3000b620:	007b007b 	.word	0x007b007b
3000b624:	012c014f 	.word	0x012c014f
3000b628:	007b007b 	.word	0x007b007b
3000b62c:	007b007b 	.word	0x007b007b
3000b630:	007b007b 	.word	0x007b007b
3000b634:	007b007b 	.word	0x007b007b
3000b638:	007b0128 	.word	0x007b0128
3000b63c:	00fc007b 	.word	0x00fc007b
3000b640:	007b007b 	.word	0x007b007b
3000b644:	007b007b 	.word	0x007b007b
3000b648:	007b007b 	.word	0x007b007b
3000b64c:	007b007b 	.word	0x007b007b
3000b650:	007b0081 	.word	0x007b0081
3000b654:	007b007b 	.word	0x007b007b
3000b658:	007b007b 	.word	0x007b007b
3000b65c:	007b007b 	.word	0x007b007b
3000b660:	007b007b 	.word	0x007b007b
3000b664:	012c014f 	.word	0x012c014f
3000b668:	007b00ca 	.word	0x007b00ca
3000b66c:	007b007b 	.word	0x007b007b
3000b670:	00ca007b 	.word	0x00ca007b
3000b674:	007b007b 	.word	0x007b007b
3000b678:	007b0128 	.word	0x007b0128
3000b67c:	00fc007b 	.word	0x00fc007b
3000b680:	007b01e6 	.word	0x007b01e6
3000b684:	01b4007b 	.word	0x01b4007b
3000b688:	017d007b 	.word	0x017d007b
3000b68c:	007b007b 	.word	0x007b007b
3000b690:	007b0083 	.word	0x007b0083
3000b694:	012a      	.short	0x012a
3000b696:	f04f 35ff 	mov.w	r5, #4294967295	; 0xffffffff
3000b69a:	4628      	mov	r0, r5
3000b69c:	b009      	add	sp, #36	; 0x24
3000b69e:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
3000b6a2:	2301      	movs	r3, #1
3000b6a4:	9304      	str	r3, [sp, #16]
3000b6a6:	f1bc 0f01 	cmp.w	ip, #1
3000b6aa:	f340 81d4 	ble.w	3000ba56 <DiagVprintf+0x4fe>
3000b6ae:	f108 0807 	add.w	r8, r8, #7
3000b6b2:	f028 0807 	bic.w	r8, r8, #7
3000b6b6:	f8d8 1004 	ldr.w	r1, [r8, #4]
3000b6ba:	f858 0b08 	ldr.w	r0, [r8], #8
3000b6be:	f1ba 0f00 	cmp.w	sl, #0
3000b6c2:	f000 8168 	beq.w	3000b996 <DiagVprintf+0x43e>
3000b6c6:	2300      	movs	r3, #0
3000b6c8:	2210      	movs	r2, #16
3000b6ca:	9302      	str	r3, [sp, #8]
3000b6cc:	9b04      	ldr	r3, [sp, #16]
3000b6ce:	9301      	str	r3, [sp, #4]
3000b6d0:	2300      	movs	r3, #0
3000b6d2:	9300      	str	r3, [sp, #0]
3000b6d4:	464b      	mov	r3, r9
3000b6d6:	f7ff fcf5 	bl	3000b0c4 <print_unsigned_num>
3000b6da:	4286      	cmp	r6, r0
3000b6dc:	dd0e      	ble.n	3000b6fc <DiagVprintf+0x1a4>
3000b6de:	f1b9 0f20 	cmp.w	r9, #32
3000b6e2:	d10b      	bne.n	3000b6fc <DiagVprintf+0x1a4>
3000b6e4:	eba6 0900 	sub.w	r9, r6, r0
3000b6e8:	f1b9 0f00 	cmp.w	r9, #0
3000b6ec:	f340 81e4 	ble.w	3000bab8 <DiagVprintf+0x560>
3000b6f0:	2020      	movs	r0, #32
3000b6f2:	47b8      	blx	r7
3000b6f4:	f1b9 0901 	subs.w	r9, r9, #1
3000b6f8:	d1fa      	bne.n	3000b6f0 <DiagVprintf+0x198>
3000b6fa:	4630      	mov	r0, r6
3000b6fc:	4405      	add	r5, r0
3000b6fe:	3401      	adds	r4, #1
3000b700:	e73c      	b.n	3000b57c <DiagVprintf+0x24>
3000b702:	f04f 0920 	mov.w	r9, #32
3000b706:	2600      	movs	r6, #0
3000b708:	e005      	b.n	3000b716 <DiagVprintf+0x1be>
3000b70a:	f814 3f01 	ldrb.w	r3, [r4, #1]!
3000b70e:	f1a2 0630 	sub.w	r6, r2, #48	; 0x30
3000b712:	2b00      	cmp	r3, #0
3000b714:	d0c1      	beq.n	3000b69a <DiagVprintf+0x142>
3000b716:	f1a3 0130 	sub.w	r1, r3, #48	; 0x30
3000b71a:	eb06 0286 	add.w	r2, r6, r6, lsl #2
3000b71e:	2909      	cmp	r1, #9
3000b720:	eb03 0242 	add.w	r2, r3, r2, lsl #1
3000b724:	d9f1      	bls.n	3000b70a <DiagVprintf+0x1b2>
3000b726:	e736      	b.n	3000b596 <DiagVprintf+0x3e>
3000b728:	3401      	adds	r4, #1
3000b72a:	4699      	mov	r9, r3
3000b72c:	7823      	ldrb	r3, [r4, #0]
3000b72e:	2b00      	cmp	r3, #0
3000b730:	d1e9      	bne.n	3000b706 <DiagVprintf+0x1ae>
3000b732:	e7b2      	b.n	3000b69a <DiagVprintf+0x142>
3000b734:	f1bc 0f01 	cmp.w	ip, #1
3000b738:	f340 8186 	ble.w	3000ba48 <DiagVprintf+0x4f0>
3000b73c:	f108 0807 	add.w	r8, r8, #7
3000b740:	f028 0807 	bic.w	r8, r8, #7
3000b744:	f8d8 1004 	ldr.w	r1, [r8, #4]
3000b748:	f858 0b08 	ldr.w	r0, [r8], #8
3000b74c:	2900      	cmp	r1, #0
3000b74e:	f2c0 8189 	blt.w	3000ba64 <DiagVprintf+0x50c>
3000b752:	2200      	movs	r2, #0
3000b754:	9202      	str	r2, [sp, #8]
3000b756:	f1ba 0f00 	cmp.w	sl, #0
3000b75a:	f000 816c 	beq.w	3000ba36 <DiagVprintf+0x4de>
3000b75e:	9b04      	ldr	r3, [sp, #16]
3000b760:	220a      	movs	r2, #10
3000b762:	9301      	str	r3, [sp, #4]
3000b764:	2300      	movs	r3, #0
3000b766:	9300      	str	r3, [sp, #0]
3000b768:	464b      	mov	r3, r9
3000b76a:	f7ff fcab 	bl	3000b0c4 <print_unsigned_num>
3000b76e:	4286      	cmp	r6, r0
3000b770:	ddc4      	ble.n	3000b6fc <DiagVprintf+0x1a4>
3000b772:	f1b9 0f20 	cmp.w	r9, #32
3000b776:	d1c1      	bne.n	3000b6fc <DiagVprintf+0x1a4>
3000b778:	eba6 0900 	sub.w	r9, r6, r0
3000b77c:	f1b9 0f00 	cmp.w	r9, #0
3000b780:	f340 819a 	ble.w	3000bab8 <DiagVprintf+0x560>
3000b784:	2020      	movs	r0, #32
3000b786:	47b8      	blx	r7
3000b788:	f1b9 0901 	subs.w	r9, r9, #1
3000b78c:	d1fa      	bne.n	3000b784 <DiagVprintf+0x22c>
3000b78e:	4630      	mov	r0, r6
3000b790:	e7b4      	b.n	3000b6fc <DiagVprintf+0x1a4>
3000b792:	bf00      	nop
3000b794:	0000abb5 	.word	0x0000abb5
3000b798:	f1bc 0f01 	cmp.w	ip, #1
3000b79c:	f340 811e 	ble.w	3000b9dc <DiagVprintf+0x484>
3000b7a0:	f108 0807 	add.w	r8, r8, #7
3000b7a4:	f028 0807 	bic.w	r8, r8, #7
3000b7a8:	f8d8 1004 	ldr.w	r1, [r8, #4]
3000b7ac:	f858 0b08 	ldr.w	r0, [r8], #8
3000b7b0:	f1ba 0f00 	cmp.w	sl, #0
3000b7b4:	f000 8119 	beq.w	3000b9ea <DiagVprintf+0x492>
3000b7b8:	2300      	movs	r3, #0
3000b7ba:	2208      	movs	r2, #8
3000b7bc:	9302      	str	r3, [sp, #8]
3000b7be:	9b04      	ldr	r3, [sp, #16]
3000b7c0:	9301      	str	r3, [sp, #4]
3000b7c2:	2300      	movs	r3, #0
3000b7c4:	9300      	str	r3, [sp, #0]
3000b7c6:	464b      	mov	r3, r9
3000b7c8:	f7ff fc7c 	bl	3000b0c4 <print_unsigned_num>
3000b7cc:	4286      	cmp	r6, r0
3000b7ce:	dd95      	ble.n	3000b6fc <DiagVprintf+0x1a4>
3000b7d0:	f1b9 0f20 	cmp.w	r9, #32
3000b7d4:	d192      	bne.n	3000b6fc <DiagVprintf+0x1a4>
3000b7d6:	eba6 0900 	sub.w	r9, r6, r0
3000b7da:	f1b9 0f00 	cmp.w	r9, #0
3000b7de:	f340 816b 	ble.w	3000bab8 <DiagVprintf+0x560>
3000b7e2:	2020      	movs	r0, #32
3000b7e4:	47b8      	blx	r7
3000b7e6:	f1b9 0901 	subs.w	r9, r9, #1
3000b7ea:	d1fa      	bne.n	3000b7e2 <DiagVprintf+0x28a>
3000b7ec:	4630      	mov	r0, r6
3000b7ee:	e785      	b.n	3000b6fc <DiagVprintf+0x1a4>
3000b7f0:	f10c 0c01 	add.w	ip, ip, #1
3000b7f4:	3401      	adds	r4, #1
3000b7f6:	e6ce      	b.n	3000b596 <DiagVprintf+0x3e>
3000b7f8:	2e01      	cmp	r6, #1
3000b7fa:	f858 1b04 	ldr.w	r1, [r8], #4
3000b7fe:	bfd4      	ite	le
3000b800:	2200      	movle	r2, #0
3000b802:	2201      	movgt	r2, #1
3000b804:	fa5f fb81 	uxtb.w	fp, r1
3000b808:	f1ba 0f00 	cmp.w	sl, #0
3000b80c:	d102      	bne.n	3000b814 <DiagVprintf+0x2bc>
3000b80e:	2a00      	cmp	r2, #0
3000b810:	f040 812e 	bne.w	3000ba70 <DiagVprintf+0x518>
3000b814:	4658      	mov	r0, fp
3000b816:	9205      	str	r2, [sp, #20]
3000b818:	47b8      	blx	r7
3000b81a:	1c69      	adds	r1, r5, #1
3000b81c:	f1ba 0f00 	cmp.w	sl, #0
3000b820:	f000 811d 	beq.w	3000ba5e <DiagVprintf+0x506>
3000b824:	9a05      	ldr	r2, [sp, #20]
3000b826:	2a00      	cmp	r2, #0
3000b828:	f000 8119 	beq.w	3000ba5e <DiagVprintf+0x506>
3000b82c:	f106 3bff 	add.w	fp, r6, #4294967295	; 0xffffffff
3000b830:	4648      	mov	r0, r9
3000b832:	47b8      	blx	r7
3000b834:	f1bb 0b01 	subs.w	fp, fp, #1
3000b838:	d1fa      	bne.n	3000b830 <DiagVprintf+0x2d8>
3000b83a:	4435      	add	r5, r6
3000b83c:	e75f      	b.n	3000b6fe <DiagVprintf+0x1a6>
3000b83e:	f1bc 0f01 	cmp.w	ip, #1
3000b842:	f340 80dd 	ble.w	3000ba00 <DiagVprintf+0x4a8>
3000b846:	f108 0807 	add.w	r8, r8, #7
3000b84a:	f028 0807 	bic.w	r8, r8, #7
3000b84e:	f8d8 1004 	ldr.w	r1, [r8, #4]
3000b852:	f858 0b08 	ldr.w	r0, [r8], #8
3000b856:	f1ba 0f00 	cmp.w	sl, #0
3000b85a:	f000 80d8 	beq.w	3000ba0e <DiagVprintf+0x4b6>
3000b85e:	2300      	movs	r3, #0
3000b860:	2202      	movs	r2, #2
3000b862:	9302      	str	r3, [sp, #8]
3000b864:	9b04      	ldr	r3, [sp, #16]
3000b866:	9301      	str	r3, [sp, #4]
3000b868:	2300      	movs	r3, #0
3000b86a:	9300      	str	r3, [sp, #0]
3000b86c:	464b      	mov	r3, r9
3000b86e:	f7ff fc29 	bl	3000b0c4 <print_unsigned_num>
3000b872:	4286      	cmp	r6, r0
3000b874:	f77f af42 	ble.w	3000b6fc <DiagVprintf+0x1a4>
3000b878:	f1b9 0f20 	cmp.w	r9, #32
3000b87c:	f47f af3e 	bne.w	3000b6fc <DiagVprintf+0x1a4>
3000b880:	eba6 0900 	sub.w	r9, r6, r0
3000b884:	f1b9 0f00 	cmp.w	r9, #0
3000b888:	f340 8116 	ble.w	3000bab8 <DiagVprintf+0x560>
3000b88c:	2020      	movs	r0, #32
3000b88e:	47b8      	blx	r7
3000b890:	f1b9 0901 	subs.w	r9, r9, #1
3000b894:	d1fa      	bne.n	3000b88c <DiagVprintf+0x334>
3000b896:	4630      	mov	r0, r6
3000b898:	e730      	b.n	3000b6fc <DiagVprintf+0x1a4>
3000b89a:	f1bc 0f01 	cmp.w	ip, #1
3000b89e:	f340 80c1 	ble.w	3000ba24 <DiagVprintf+0x4cc>
3000b8a2:	f108 0807 	add.w	r8, r8, #7
3000b8a6:	f028 0807 	bic.w	r8, r8, #7
3000b8aa:	f8d8 1004 	ldr.w	r1, [r8, #4]
3000b8ae:	f858 0b08 	ldr.w	r0, [r8], #8
3000b8b2:	f1ba 0f00 	cmp.w	sl, #0
3000b8b6:	f000 80bc 	beq.w	3000ba32 <DiagVprintf+0x4da>
3000b8ba:	2300      	movs	r3, #0
3000b8bc:	220a      	movs	r2, #10
3000b8be:	9302      	str	r3, [sp, #8]
3000b8c0:	9b04      	ldr	r3, [sp, #16]
3000b8c2:	9301      	str	r3, [sp, #4]
3000b8c4:	2300      	movs	r3, #0
3000b8c6:	9300      	str	r3, [sp, #0]
3000b8c8:	464b      	mov	r3, r9
3000b8ca:	f7ff fbfb 	bl	3000b0c4 <print_unsigned_num>
3000b8ce:	4286      	cmp	r6, r0
3000b8d0:	f77f af14 	ble.w	3000b6fc <DiagVprintf+0x1a4>
3000b8d4:	f1b9 0f20 	cmp.w	r9, #32
3000b8d8:	f47f af10 	bne.w	3000b6fc <DiagVprintf+0x1a4>
3000b8dc:	eba6 0900 	sub.w	r9, r6, r0
3000b8e0:	f1b9 0f00 	cmp.w	r9, #0
3000b8e4:	f340 80e8 	ble.w	3000bab8 <DiagVprintf+0x560>
3000b8e8:	2020      	movs	r0, #32
3000b8ea:	47b8      	blx	r7
3000b8ec:	f1b9 0901 	subs.w	r9, r9, #1
3000b8f0:	d1fa      	bne.n	3000b8e8 <DiagVprintf+0x390>
3000b8f2:	4630      	mov	r0, r6
3000b8f4:	e702      	b.n	3000b6fc <DiagVprintf+0x1a4>
3000b8f6:	3401      	adds	r4, #1
3000b8f8:	f10a 0a01 	add.w	sl, sl, #1
3000b8fc:	e64b      	b.n	3000b596 <DiagVprintf+0x3e>
3000b8fe:	2025      	movs	r0, #37	; 0x25
3000b900:	3501      	adds	r5, #1
3000b902:	3401      	adds	r4, #1
3000b904:	47b8      	blx	r7
3000b906:	e639      	b.n	3000b57c <DiagVprintf+0x24>
3000b908:	4643      	mov	r3, r8
3000b90a:	f853 2b04 	ldr.w	r2, [r3], #4
3000b90e:	7810      	ldrb	r0, [r2, #0]
3000b910:	9305      	str	r3, [sp, #20]
3000b912:	2800      	cmp	r0, #0
3000b914:	f000 80d5 	beq.w	3000bac2 <DiagVprintf+0x56a>
3000b918:	4694      	mov	ip, r2
3000b91a:	f1c2 0801 	rsb	r8, r2, #1
3000b91e:	eb08 010c 	add.w	r1, r8, ip
3000b922:	f81c 3f01 	ldrb.w	r3, [ip, #1]!
3000b926:	2b00      	cmp	r3, #0
3000b928:	d1f9      	bne.n	3000b91e <DiagVprintf+0x3c6>
3000b92a:	9106      	str	r1, [sp, #24]
3000b92c:	f1ba 0f00 	cmp.w	sl, #0
3000b930:	d03c      	beq.n	3000b9ac <DiagVprintf+0x454>
3000b932:	2a00      	cmp	r2, #0
3000b934:	f000 80cb 	beq.w	3000bace <DiagVprintf+0x576>
3000b938:	b168      	cbz	r0, 3000b956 <DiagVprintf+0x3fe>
3000b93a:	4693      	mov	fp, r2
3000b93c:	f1c2 0801 	rsb	r8, r2, #1
3000b940:	47b8      	blx	r7
3000b942:	eb0b 0c08 	add.w	ip, fp, r8
3000b946:	f81b 0f01 	ldrb.w	r0, [fp, #1]!
3000b94a:	2800      	cmp	r0, #0
3000b94c:	d1f8      	bne.n	3000b940 <DiagVprintf+0x3e8>
3000b94e:	4465      	add	r5, ip
3000b950:	f1ba 0f00 	cmp.w	sl, #0
3000b954:	d006      	beq.n	3000b964 <DiagVprintf+0x40c>
3000b956:	9b06      	ldr	r3, [sp, #24]
3000b958:	eba6 0b03 	sub.w	fp, r6, r3
3000b95c:	f1bb 0f00 	cmp.w	fp, #0
3000b960:	f300 80a1 	bgt.w	3000baa6 <DiagVprintf+0x54e>
3000b964:	f8dd 8014 	ldr.w	r8, [sp, #20]
3000b968:	3401      	adds	r4, #1
3000b96a:	e607      	b.n	3000b57c <DiagVprintf+0x24>
3000b96c:	f858 0b04 	ldr.w	r0, [r8], #4
3000b970:	f1ba 0f00 	cmp.w	sl, #0
3000b974:	d127      	bne.n	3000b9c6 <DiagVprintf+0x46e>
3000b976:	2308      	movs	r3, #8
3000b978:	2201      	movs	r2, #1
3000b97a:	4651      	mov	r1, sl
3000b97c:	f8cd a008 	str.w	sl, [sp, #8]
3000b980:	e9cd 3200 	strd	r3, r2, [sp]
3000b984:	2330      	movs	r3, #48	; 0x30
3000b986:	2210      	movs	r2, #16
3000b988:	f7ff fb9c 	bl	3000b0c4 <print_unsigned_num>
3000b98c:	2301      	movs	r3, #1
3000b98e:	4405      	add	r5, r0
3000b990:	3401      	adds	r4, #1
3000b992:	9304      	str	r3, [sp, #16]
3000b994:	e5f2      	b.n	3000b57c <DiagVprintf+0x24>
3000b996:	9a04      	ldr	r2, [sp, #16]
3000b998:	464b      	mov	r3, r9
3000b99a:	f8cd a008 	str.w	sl, [sp, #8]
3000b99e:	9201      	str	r2, [sp, #4]
3000b9a0:	2210      	movs	r2, #16
3000b9a2:	9600      	str	r6, [sp, #0]
3000b9a4:	f7ff fb8e 	bl	3000b0c4 <print_unsigned_num>
3000b9a8:	4405      	add	r5, r0
3000b9aa:	e6a8      	b.n	3000b6fe <DiagVprintf+0x1a6>
3000b9ac:	9b06      	ldr	r3, [sp, #24]
3000b9ae:	eba6 0803 	sub.w	r8, r6, r3
3000b9b2:	f1b8 0f00 	cmp.w	r8, #0
3000b9b6:	dc66      	bgt.n	3000ba86 <DiagVprintf+0x52e>
3000b9b8:	2a00      	cmp	r2, #0
3000b9ba:	f000 808a 	beq.w	3000bad2 <DiagVprintf+0x57a>
3000b9be:	7810      	ldrb	r0, [r2, #0]
3000b9c0:	2800      	cmp	r0, #0
3000b9c2:	d1ba      	bne.n	3000b93a <DiagVprintf+0x3e2>
3000b9c4:	e7ce      	b.n	3000b964 <DiagVprintf+0x40c>
3000b9c6:	2300      	movs	r3, #0
3000b9c8:	2210      	movs	r2, #16
3000b9ca:	2100      	movs	r1, #0
3000b9cc:	9302      	str	r3, [sp, #8]
3000b9ce:	9300      	str	r3, [sp, #0]
3000b9d0:	2301      	movs	r3, #1
3000b9d2:	9301      	str	r3, [sp, #4]
3000b9d4:	2330      	movs	r3, #48	; 0x30
3000b9d6:	f7ff fb75 	bl	3000b0c4 <print_unsigned_num>
3000b9da:	e7d7      	b.n	3000b98c <DiagVprintf+0x434>
3000b9dc:	f858 0b04 	ldr.w	r0, [r8], #4
3000b9e0:	2100      	movs	r1, #0
3000b9e2:	f1ba 0f00 	cmp.w	sl, #0
3000b9e6:	f47f aee7 	bne.w	3000b7b8 <DiagVprintf+0x260>
3000b9ea:	9a04      	ldr	r2, [sp, #16]
3000b9ec:	464b      	mov	r3, r9
3000b9ee:	f8cd a008 	str.w	sl, [sp, #8]
3000b9f2:	9201      	str	r2, [sp, #4]
3000b9f4:	2208      	movs	r2, #8
3000b9f6:	9600      	str	r6, [sp, #0]
3000b9f8:	f7ff fb64 	bl	3000b0c4 <print_unsigned_num>
3000b9fc:	4405      	add	r5, r0
3000b9fe:	e67e      	b.n	3000b6fe <DiagVprintf+0x1a6>
3000ba00:	f858 0b04 	ldr.w	r0, [r8], #4
3000ba04:	2100      	movs	r1, #0
3000ba06:	f1ba 0f00 	cmp.w	sl, #0
3000ba0a:	f47f af28 	bne.w	3000b85e <DiagVprintf+0x306>
3000ba0e:	9a04      	ldr	r2, [sp, #16]
3000ba10:	464b      	mov	r3, r9
3000ba12:	f8cd a008 	str.w	sl, [sp, #8]
3000ba16:	9201      	str	r2, [sp, #4]
3000ba18:	2202      	movs	r2, #2
3000ba1a:	9600      	str	r6, [sp, #0]
3000ba1c:	f7ff fb52 	bl	3000b0c4 <print_unsigned_num>
3000ba20:	4405      	add	r5, r0
3000ba22:	e66c      	b.n	3000b6fe <DiagVprintf+0x1a6>
3000ba24:	f858 0b04 	ldr.w	r0, [r8], #4
3000ba28:	2100      	movs	r1, #0
3000ba2a:	f1ba 0f00 	cmp.w	sl, #0
3000ba2e:	f47f af44 	bne.w	3000b8ba <DiagVprintf+0x362>
3000ba32:	f8cd a008 	str.w	sl, [sp, #8]
3000ba36:	9a04      	ldr	r2, [sp, #16]
3000ba38:	464b      	mov	r3, r9
3000ba3a:	9600      	str	r6, [sp, #0]
3000ba3c:	9201      	str	r2, [sp, #4]
3000ba3e:	220a      	movs	r2, #10
3000ba40:	f7ff fb40 	bl	3000b0c4 <print_unsigned_num>
3000ba44:	4405      	add	r5, r0
3000ba46:	e65a      	b.n	3000b6fe <DiagVprintf+0x1a6>
3000ba48:	f8d8 0000 	ldr.w	r0, [r8]
3000ba4c:	f108 0204 	add.w	r2, r8, #4
3000ba50:	17c1      	asrs	r1, r0, #31
3000ba52:	4690      	mov	r8, r2
3000ba54:	e67a      	b.n	3000b74c <DiagVprintf+0x1f4>
3000ba56:	f858 0b04 	ldr.w	r0, [r8], #4
3000ba5a:	2100      	movs	r1, #0
3000ba5c:	e62f      	b.n	3000b6be <DiagVprintf+0x166>
3000ba5e:	460d      	mov	r5, r1
3000ba60:	3401      	adds	r4, #1
3000ba62:	e58b      	b.n	3000b57c <DiagVprintf+0x24>
3000ba64:	4240      	negs	r0, r0
3000ba66:	f04f 0201 	mov.w	r2, #1
3000ba6a:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
3000ba6e:	e671      	b.n	3000b754 <DiagVprintf+0x1fc>
3000ba70:	f106 3aff 	add.w	sl, r6, #4294967295	; 0xffffffff
3000ba74:	4648      	mov	r0, r9
3000ba76:	47b8      	blx	r7
3000ba78:	f1ba 0a01 	subs.w	sl, sl, #1
3000ba7c:	d1fa      	bne.n	3000ba74 <DiagVprintf+0x51c>
3000ba7e:	4658      	mov	r0, fp
3000ba80:	4435      	add	r5, r6
3000ba82:	47b8      	blx	r7
3000ba84:	e63b      	b.n	3000b6fe <DiagVprintf+0x1a6>
3000ba86:	4623      	mov	r3, r4
3000ba88:	46c3      	mov	fp, r8
3000ba8a:	464c      	mov	r4, r9
3000ba8c:	9207      	str	r2, [sp, #28]
3000ba8e:	4699      	mov	r9, r3
3000ba90:	4620      	mov	r0, r4
3000ba92:	47b8      	blx	r7
3000ba94:	f1bb 0b01 	subs.w	fp, fp, #1
3000ba98:	d1fa      	bne.n	3000ba90 <DiagVprintf+0x538>
3000ba9a:	464b      	mov	r3, r9
3000ba9c:	9a07      	ldr	r2, [sp, #28]
3000ba9e:	46a1      	mov	r9, r4
3000baa0:	4445      	add	r5, r8
3000baa2:	461c      	mov	r4, r3
3000baa4:	e788      	b.n	3000b9b8 <DiagVprintf+0x460>
3000baa6:	465e      	mov	r6, fp
3000baa8:	4648      	mov	r0, r9
3000baaa:	47b8      	blx	r7
3000baac:	3e01      	subs	r6, #1
3000baae:	d1fb      	bne.n	3000baa8 <DiagVprintf+0x550>
3000bab0:	445d      	add	r5, fp
3000bab2:	f8dd 8014 	ldr.w	r8, [sp, #20]
3000bab6:	e622      	b.n	3000b6fe <DiagVprintf+0x1a6>
3000bab8:	4606      	mov	r6, r0
3000baba:	4630      	mov	r0, r6
3000babc:	e61e      	b.n	3000b6fc <DiagVprintf+0x1a4>
3000babe:	4605      	mov	r5, r0
3000bac0:	e5eb      	b.n	3000b69a <DiagVprintf+0x142>
3000bac2:	9006      	str	r0, [sp, #24]
3000bac4:	f1ba 0f00 	cmp.w	sl, #0
3000bac8:	f47f af33 	bne.w	3000b932 <DiagVprintf+0x3da>
3000bacc:	e76e      	b.n	3000b9ac <DiagVprintf+0x454>
3000bace:	3d01      	subs	r5, #1
3000bad0:	e741      	b.n	3000b956 <DiagVprintf+0x3fe>
3000bad2:	3d01      	subs	r5, #1
3000bad4:	e746      	b.n	3000b964 <DiagVprintf+0x40c>
3000bad6:	bf00      	nop

3000bad8 <DiagPrintf>:
3000bad8:	b40f      	push	{r0, r1, r2, r3}
3000bada:	b500      	push	{lr}
3000badc:	b083      	sub	sp, #12
3000bade:	a904      	add	r1, sp, #16
3000bae0:	f851 0b04 	ldr.w	r0, [r1], #4
3000bae4:	9101      	str	r1, [sp, #4]
3000bae6:	f7ff fd37 	bl	3000b558 <DiagVprintf>
3000baea:	b003      	add	sp, #12
3000baec:	f85d eb04 	ldr.w	lr, [sp], #4
3000baf0:	b004      	add	sp, #16
3000baf2:	4770      	bx	lr

3000baf4 <DiagVprintfNano>:
3000baf4:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
3000baf8:	4604      	mov	r4, r0
3000bafa:	7800      	ldrb	r0, [r0, #0]
3000bafc:	b089      	sub	sp, #36	; 0x24
3000bafe:	2800      	cmp	r0, #0
3000bb00:	f000 81d9 	beq.w	3000beb6 <DiagVprintfNano+0x3c2>
3000bb04:	468b      	mov	fp, r1
3000bb06:	f04f 0800 	mov.w	r8, #0
3000bb0a:	4fa4      	ldr	r7, [pc, #656]	; (3000bd9c <DiagVprintfNano+0x2a8>)
3000bb0c:	e005      	b.n	3000bb1a <DiagVprintfNano+0x26>
3000bb0e:	f108 0801 	add.w	r8, r8, #1
3000bb12:	47b8      	blx	r7
3000bb14:	7820      	ldrb	r0, [r4, #0]
3000bb16:	2800      	cmp	r0, #0
3000bb18:	d06d      	beq.n	3000bbf6 <DiagVprintfNano+0x102>
3000bb1a:	2825      	cmp	r0, #37	; 0x25
3000bb1c:	f104 0401 	add.w	r4, r4, #1
3000bb20:	d1f5      	bne.n	3000bb0e <DiagVprintfNano+0x1a>
3000bb22:	2500      	movs	r5, #0
3000bb24:	7821      	ldrb	r1, [r4, #0]
3000bb26:	462a      	mov	r2, r5
3000bb28:	2900      	cmp	r1, #0
3000bb2a:	f000 8081 	beq.w	3000bc30 <DiagVprintfNano+0x13c>
3000bb2e:	f1a1 0320 	sub.w	r3, r1, #32
3000bb32:	b2d8      	uxtb	r0, r3
3000bb34:	2858      	cmp	r0, #88	; 0x58
3000bb36:	d85c      	bhi.n	3000bbf2 <DiagVprintfNano+0xfe>
3000bb38:	2b58      	cmp	r3, #88	; 0x58
3000bb3a:	d85a      	bhi.n	3000bbf2 <DiagVprintfNano+0xfe>
3000bb3c:	e8df f013 	tbh	[pc, r3, lsl #1]
3000bb40:	0059005f 	.word	0x0059005f
3000bb44:	00590059 	.word	0x00590059
3000bb48:	01270059 	.word	0x01270059
3000bb4c:	00590059 	.word	0x00590059
3000bb50:	00590059 	.word	0x00590059
3000bb54:	00590059 	.word	0x00590059
3000bb58:	00590059 	.word	0x00590059
3000bb5c:	00590059 	.word	0x00590059
3000bb60:	007a005f 	.word	0x007a005f
3000bb64:	007a007a 	.word	0x007a007a
3000bb68:	007a007a 	.word	0x007a007a
3000bb6c:	007a007a 	.word	0x007a007a
3000bb70:	007a007a 	.word	0x007a007a
3000bb74:	00590059 	.word	0x00590059
3000bb78:	00590059 	.word	0x00590059
3000bb7c:	00590059 	.word	0x00590059
3000bb80:	00590059 	.word	0x00590059
3000bb84:	00db0059 	.word	0x00db0059
3000bb88:	00590059 	.word	0x00590059
3000bb8c:	00590059 	.word	0x00590059
3000bb90:	00590059 	.word	0x00590059
3000bb94:	00590059 	.word	0x00590059
3000bb98:	00590059 	.word	0x00590059
3000bb9c:	00590059 	.word	0x00590059
3000bba0:	00590059 	.word	0x00590059
3000bba4:	00590059 	.word	0x00590059
3000bba8:	00590059 	.word	0x00590059
3000bbac:	00590059 	.word	0x00590059
3000bbb0:	00590059 	.word	0x00590059
3000bbb4:	00590059 	.word	0x00590059
3000bbb8:	00590059 	.word	0x00590059
3000bbbc:	00590059 	.word	0x00590059
3000bbc0:	00590059 	.word	0x00590059
3000bbc4:	00db0059 	.word	0x00db0059
3000bbc8:	0059007c 	.word	0x0059007c
3000bbcc:	00590059 	.word	0x00590059
3000bbd0:	007c0059 	.word	0x007c0059
3000bbd4:	00590059 	.word	0x00590059
3000bbd8:	00590059 	.word	0x00590059
3000bbdc:	00590059 	.word	0x00590059
3000bbe0:	00590059 	.word	0x00590059
3000bbe4:	01720059 	.word	0x01720059
3000bbe8:	01320059 	.word	0x01320059
3000bbec:	00590059 	.word	0x00590059
3000bbf0:	00e8      	.short	0x00e8
3000bbf2:	f04f 38ff 	mov.w	r8, #4294967295	; 0xffffffff
3000bbf6:	4640      	mov	r0, r8
3000bbf8:	b009      	add	sp, #36	; 0x24
3000bbfa:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
3000bbfe:	3401      	adds	r4, #1
3000bc00:	460a      	mov	r2, r1
3000bc02:	7821      	ldrb	r1, [r4, #0]
3000bc04:	2900      	cmp	r1, #0
3000bc06:	d0f6      	beq.n	3000bbf6 <DiagVprintfNano+0x102>
3000bc08:	2500      	movs	r5, #0
3000bc0a:	e005      	b.n	3000bc18 <DiagVprintfNano+0x124>
3000bc0c:	f814 1f01 	ldrb.w	r1, [r4, #1]!
3000bc10:	f1a3 0530 	sub.w	r5, r3, #48	; 0x30
3000bc14:	2900      	cmp	r1, #0
3000bc16:	d0ee      	beq.n	3000bbf6 <DiagVprintfNano+0x102>
3000bc18:	f1a1 0030 	sub.w	r0, r1, #48	; 0x30
3000bc1c:	eb05 0385 	add.w	r3, r5, r5, lsl #2
3000bc20:	2809      	cmp	r0, #9
3000bc22:	eb01 0343 	add.w	r3, r1, r3, lsl #1
3000bc26:	d9f1      	bls.n	3000bc0c <DiagVprintfNano+0x118>
3000bc28:	7821      	ldrb	r1, [r4, #0]
3000bc2a:	2900      	cmp	r1, #0
3000bc2c:	f47f af7f 	bne.w	3000bb2e <DiagVprintfNano+0x3a>
3000bc30:	3401      	adds	r4, #1
3000bc32:	e76f      	b.n	3000bb14 <DiagVprintfNano+0x20>
3000bc34:	2220      	movs	r2, #32
3000bc36:	e7e7      	b.n	3000bc08 <DiagVprintfNano+0x114>
3000bc38:	4659      	mov	r1, fp
3000bc3a:	f851 3b04 	ldr.w	r3, [r1], #4
3000bc3e:	2b00      	cmp	r3, #0
3000bc40:	9101      	str	r1, [sp, #4]
3000bc42:	f2c0 8116 	blt.w	3000be72 <DiagVprintfNano+0x37e>
3000bc46:	2100      	movs	r1, #0
3000bc48:	9100      	str	r1, [sp, #0]
3000bc4a:	f10d 0914 	add.w	r9, sp, #20
3000bc4e:	f04f 0b00 	mov.w	fp, #0
3000bc52:	f8df c14c 	ldr.w	ip, [pc, #332]	; 3000bda0 <DiagVprintfNano+0x2ac>
3000bc56:	4649      	mov	r1, r9
3000bc58:	fbac 0e03 	umull	r0, lr, ip, r3
3000bc5c:	4618      	mov	r0, r3
3000bc5e:	46da      	mov	sl, fp
3000bc60:	f10b 0b01 	add.w	fp, fp, #1
3000bc64:	ea4f 0ede 	mov.w	lr, lr, lsr #3
3000bc68:	2809      	cmp	r0, #9
3000bc6a:	eb0e 068e 	add.w	r6, lr, lr, lsl #2
3000bc6e:	eba3 0646 	sub.w	r6, r3, r6, lsl #1
3000bc72:	4673      	mov	r3, lr
3000bc74:	f106 0630 	add.w	r6, r6, #48	; 0x30
3000bc78:	b2f6      	uxtb	r6, r6
3000bc7a:	f801 6b01 	strb.w	r6, [r1], #1
3000bc7e:	d8eb      	bhi.n	3000bc58 <DiagVprintfNano+0x164>
3000bc80:	9b00      	ldr	r3, [sp, #0]
3000bc82:	2b00      	cmp	r3, #0
3000bc84:	f000 810c 	beq.w	3000bea0 <DiagVprintfNano+0x3ac>
3000bc88:	2a30      	cmp	r2, #48	; 0x30
3000bc8a:	f105 35ff 	add.w	r5, r5, #4294967295	; 0xffffffff
3000bc8e:	f040 810e 	bne.w	3000beae <DiagVprintfNano+0x3ba>
3000bc92:	202d      	movs	r0, #45	; 0x2d
3000bc94:	9202      	str	r2, [sp, #8]
3000bc96:	47b8      	blx	r7
3000bc98:	2d00      	cmp	r5, #0
3000bc9a:	9a02      	ldr	r2, [sp, #8]
3000bc9c:	dd19      	ble.n	3000bcd2 <DiagVprintfNano+0x1de>
3000bc9e:	45ab      	cmp	fp, r5
3000bca0:	da12      	bge.n	3000bcc8 <DiagVprintfNano+0x1d4>
3000bca2:	9403      	str	r4, [sp, #12]
3000bca4:	4614      	mov	r4, r2
3000bca6:	9502      	str	r5, [sp, #8]
3000bca8:	3d01      	subs	r5, #1
3000bcaa:	4620      	mov	r0, r4
3000bcac:	47b8      	blx	r7
3000bcae:	45ab      	cmp	fp, r5
3000bcb0:	d1fa      	bne.n	3000bca8 <DiagVprintfNano+0x1b4>
3000bcb2:	9800      	ldr	r0, [sp, #0]
3000bcb4:	ea6f 030b 	mvn.w	r3, fp
3000bcb8:	9d02      	ldr	r5, [sp, #8]
3000bcba:	4622      	mov	r2, r4
3000bcbc:	1c41      	adds	r1, r0, #1
3000bcbe:	9c03      	ldr	r4, [sp, #12]
3000bcc0:	442b      	add	r3, r5
3000bcc2:	440b      	add	r3, r1
3000bcc4:	9300      	str	r3, [sp, #0]
3000bcc6:	b120      	cbz	r0, 3000bcd2 <DiagVprintfNano+0x1de>
3000bcc8:	f012 0fdf 	tst.w	r2, #223	; 0xdf
3000bccc:	d101      	bne.n	3000bcd2 <DiagVprintfNano+0x1de>
3000bcce:	202d      	movs	r0, #45	; 0x2d
3000bcd0:	47b8      	blx	r7
3000bcd2:	eb09 050a 	add.w	r5, r9, sl
3000bcd6:	4630      	mov	r0, r6
3000bcd8:	e001      	b.n	3000bcde <DiagVprintfNano+0x1ea>
3000bcda:	f815 0d01 	ldrb.w	r0, [r5, #-1]!
3000bcde:	47b8      	blx	r7
3000bce0:	45a9      	cmp	r9, r5
3000bce2:	d1fa      	bne.n	3000bcda <DiagVprintfNano+0x1e6>
3000bce4:	f108 0801 	add.w	r8, r8, #1
3000bce8:	3401      	adds	r4, #1
3000bcea:	44d0      	add	r8, sl
3000bcec:	e9dd 3b00 	ldrd	r3, fp, [sp]
3000bcf0:	4443      	add	r3, r8
3000bcf2:	4698      	mov	r8, r3
3000bcf4:	e70e      	b.n	3000bb14 <DiagVprintfNano+0x20>
3000bcf6:	f85b 3b04 	ldr.w	r3, [fp], #4
3000bcfa:	2d01      	cmp	r5, #1
3000bcfc:	fa5f f983 	uxtb.w	r9, r3
3000bd00:	f300 80bb 	bgt.w	3000be7a <DiagVprintfNano+0x386>
3000bd04:	4648      	mov	r0, r9
3000bd06:	f108 0801 	add.w	r8, r8, #1
3000bd0a:	3401      	adds	r4, #1
3000bd0c:	47b8      	blx	r7
3000bd0e:	e701      	b.n	3000bb14 <DiagVprintfNano+0x20>
3000bd10:	4659      	mov	r1, fp
3000bd12:	f10d 0914 	add.w	r9, sp, #20
3000bd16:	2600      	movs	r6, #0
3000bd18:	f851 3b04 	ldr.w	r3, [r1], #4
3000bd1c:	4648      	mov	r0, r9
3000bd1e:	9100      	str	r1, [sp, #0]
3000bd20:	e000      	b.n	3000bd24 <DiagVprintfNano+0x230>
3000bd22:	465e      	mov	r6, fp
3000bd24:	f003 010f 	and.w	r1, r3, #15
3000bd28:	f106 0b01 	add.w	fp, r6, #1
3000bd2c:	2909      	cmp	r1, #9
3000bd2e:	f101 0a57 	add.w	sl, r1, #87	; 0x57
3000bd32:	bf98      	it	ls
3000bd34:	f101 0a30 	addls.w	sl, r1, #48	; 0x30
3000bd38:	0919      	lsrs	r1, r3, #4
3000bd3a:	2b0f      	cmp	r3, #15
3000bd3c:	f800 ab01 	strb.w	sl, [r0], #1
3000bd40:	460b      	mov	r3, r1
3000bd42:	d8ee      	bhi.n	3000bd22 <DiagVprintfNano+0x22e>
3000bd44:	2d00      	cmp	r5, #0
3000bd46:	f340 808f 	ble.w	3000be68 <DiagVprintfNano+0x374>
3000bd4a:	455d      	cmp	r5, fp
3000bd4c:	f340 808c 	ble.w	3000be68 <DiagVprintfNano+0x374>
3000bd50:	9402      	str	r4, [sp, #8]
3000bd52:	4614      	mov	r4, r2
3000bd54:	9501      	str	r5, [sp, #4]
3000bd56:	3d01      	subs	r5, #1
3000bd58:	4620      	mov	r0, r4
3000bd5a:	47b8      	blx	r7
3000bd5c:	45ab      	cmp	fp, r5
3000bd5e:	d1fa      	bne.n	3000bd56 <DiagVprintfNano+0x262>
3000bd60:	e9dd 5401 	ldrd	r5, r4, [sp, #4]
3000bd64:	eba5 030b 	sub.w	r3, r5, fp
3000bd68:	9301      	str	r3, [sp, #4]
3000bd6a:	eb09 0506 	add.w	r5, r9, r6
3000bd6e:	4650      	mov	r0, sl
3000bd70:	e001      	b.n	3000bd76 <DiagVprintfNano+0x282>
3000bd72:	f815 0d01 	ldrb.w	r0, [r5, #-1]!
3000bd76:	47b8      	blx	r7
3000bd78:	454d      	cmp	r5, r9
3000bd7a:	d1fa      	bne.n	3000bd72 <DiagVprintfNano+0x27e>
3000bd7c:	f108 0801 	add.w	r8, r8, #1
3000bd80:	3401      	adds	r4, #1
3000bd82:	44b0      	add	r8, r6
3000bd84:	e9dd b300 	ldrd	fp, r3, [sp]
3000bd88:	4443      	add	r3, r8
3000bd8a:	4698      	mov	r8, r3
3000bd8c:	e6c2      	b.n	3000bb14 <DiagVprintfNano+0x20>
3000bd8e:	2025      	movs	r0, #37	; 0x25
3000bd90:	f108 0801 	add.w	r8, r8, #1
3000bd94:	3401      	adds	r4, #1
3000bd96:	47b8      	blx	r7
3000bd98:	e6bc      	b.n	3000bb14 <DiagVprintfNano+0x20>
3000bd9a:	bf00      	nop
3000bd9c:	0000abb5 	.word	0x0000abb5
3000bda0:	cccccccd 	.word	0xcccccccd
3000bda4:	4659      	mov	r1, fp
3000bda6:	f10d 0914 	add.w	r9, sp, #20
3000bdaa:	f04f 0b00 	mov.w	fp, #0
3000bdae:	f8df c110 	ldr.w	ip, [pc, #272]	; 3000bec0 <DiagVprintfNano+0x3cc>
3000bdb2:	f851 3b04 	ldr.w	r3, [r1], #4
3000bdb6:	9100      	str	r1, [sp, #0]
3000bdb8:	4649      	mov	r1, r9
3000bdba:	fbac 0e03 	umull	r0, lr, ip, r3
3000bdbe:	4618      	mov	r0, r3
3000bdc0:	46da      	mov	sl, fp
3000bdc2:	f10b 0b01 	add.w	fp, fp, #1
3000bdc6:	ea4f 0ede 	mov.w	lr, lr, lsr #3
3000bdca:	2809      	cmp	r0, #9
3000bdcc:	eb0e 068e 	add.w	r6, lr, lr, lsl #2
3000bdd0:	eba3 0646 	sub.w	r6, r3, r6, lsl #1
3000bdd4:	4673      	mov	r3, lr
3000bdd6:	f106 0630 	add.w	r6, r6, #48	; 0x30
3000bdda:	b2f6      	uxtb	r6, r6
3000bddc:	f801 6b01 	strb.w	r6, [r1], #1
3000bde0:	d8eb      	bhi.n	3000bdba <DiagVprintfNano+0x2c6>
3000bde2:	2d00      	cmp	r5, #0
3000bde4:	dd43      	ble.n	3000be6e <DiagVprintfNano+0x37a>
3000bde6:	455d      	cmp	r5, fp
3000bde8:	dd41      	ble.n	3000be6e <DiagVprintfNano+0x37a>
3000bdea:	9402      	str	r4, [sp, #8]
3000bdec:	4614      	mov	r4, r2
3000bdee:	9501      	str	r5, [sp, #4]
3000bdf0:	3d01      	subs	r5, #1
3000bdf2:	4620      	mov	r0, r4
3000bdf4:	47b8      	blx	r7
3000bdf6:	45ab      	cmp	fp, r5
3000bdf8:	d1fa      	bne.n	3000bdf0 <DiagVprintfNano+0x2fc>
3000bdfa:	e9dd 5401 	ldrd	r5, r4, [sp, #4]
3000bdfe:	eba5 050b 	sub.w	r5, r5, fp
3000be02:	eb09 0b0a 	add.w	fp, r9, sl
3000be06:	4630      	mov	r0, r6
3000be08:	e001      	b.n	3000be0e <DiagVprintfNano+0x31a>
3000be0a:	f81b 0d01 	ldrb.w	r0, [fp, #-1]!
3000be0e:	47b8      	blx	r7
3000be10:	45d9      	cmp	r9, fp
3000be12:	d1fa      	bne.n	3000be0a <DiagVprintfNano+0x316>
3000be14:	f108 0801 	add.w	r8, r8, #1
3000be18:	f8dd b000 	ldr.w	fp, [sp]
3000be1c:	3401      	adds	r4, #1
3000be1e:	44d0      	add	r8, sl
3000be20:	44a8      	add	r8, r5
3000be22:	e677      	b.n	3000bb14 <DiagVprintfNano+0x20>
3000be24:	f85b 6b04 	ldr.w	r6, [fp], #4
3000be28:	7833      	ldrb	r3, [r6, #0]
3000be2a:	b14b      	cbz	r3, 3000be40 <DiagVprintfNano+0x34c>
3000be2c:	4633      	mov	r3, r6
3000be2e:	f1c6 0901 	rsb	r9, r6, #1
3000be32:	eb09 0003 	add.w	r0, r9, r3
3000be36:	f813 1f01 	ldrb.w	r1, [r3, #1]!
3000be3a:	2900      	cmp	r1, #0
3000be3c:	d1f9      	bne.n	3000be32 <DiagVprintfNano+0x33e>
3000be3e:	1a2d      	subs	r5, r5, r0
3000be40:	2d00      	cmp	r5, #0
3000be42:	dc24      	bgt.n	3000be8e <DiagVprintfNano+0x39a>
3000be44:	2e00      	cmp	r6, #0
3000be46:	d038      	beq.n	3000beba <DiagVprintfNano+0x3c6>
3000be48:	7830      	ldrb	r0, [r6, #0]
3000be4a:	2800      	cmp	r0, #0
3000be4c:	f43f aef0 	beq.w	3000bc30 <DiagVprintfNano+0x13c>
3000be50:	f1c6 0901 	rsb	r9, r6, #1
3000be54:	47b8      	blx	r7
3000be56:	eb06 0309 	add.w	r3, r6, r9
3000be5a:	f816 0f01 	ldrb.w	r0, [r6, #1]!
3000be5e:	2800      	cmp	r0, #0
3000be60:	d1f8      	bne.n	3000be54 <DiagVprintfNano+0x360>
3000be62:	4498      	add	r8, r3
3000be64:	3401      	adds	r4, #1
3000be66:	e655      	b.n	3000bb14 <DiagVprintfNano+0x20>
3000be68:	2300      	movs	r3, #0
3000be6a:	9301      	str	r3, [sp, #4]
3000be6c:	e77d      	b.n	3000bd6a <DiagVprintfNano+0x276>
3000be6e:	2500      	movs	r5, #0
3000be70:	e7c7      	b.n	3000be02 <DiagVprintfNano+0x30e>
3000be72:	2101      	movs	r1, #1
3000be74:	425b      	negs	r3, r3
3000be76:	9100      	str	r1, [sp, #0]
3000be78:	e6e7      	b.n	3000bc4a <DiagVprintfNano+0x156>
3000be7a:	1e6e      	subs	r6, r5, #1
3000be7c:	4692      	mov	sl, r2
3000be7e:	4650      	mov	r0, sl
3000be80:	47b8      	blx	r7
3000be82:	3e01      	subs	r6, #1
3000be84:	d1fb      	bne.n	3000be7e <DiagVprintfNano+0x38a>
3000be86:	f108 38ff 	add.w	r8, r8, #4294967295	; 0xffffffff
3000be8a:	44a8      	add	r8, r5
3000be8c:	e73a      	b.n	3000bd04 <DiagVprintfNano+0x210>
3000be8e:	46a9      	mov	r9, r5
3000be90:	4692      	mov	sl, r2
3000be92:	4650      	mov	r0, sl
3000be94:	47b8      	blx	r7
3000be96:	f1b9 0901 	subs.w	r9, r9, #1
3000be9a:	d1fa      	bne.n	3000be92 <DiagVprintfNano+0x39e>
3000be9c:	44a8      	add	r8, r5
3000be9e:	e7d1      	b.n	3000be44 <DiagVprintfNano+0x350>
3000bea0:	2d00      	cmp	r5, #0
3000bea2:	f77f af16 	ble.w	3000bcd2 <DiagVprintfNano+0x1de>
3000bea6:	455d      	cmp	r5, fp
3000bea8:	f73f aefb 	bgt.w	3000bca2 <DiagVprintfNano+0x1ae>
3000beac:	e711      	b.n	3000bcd2 <DiagVprintfNano+0x1de>
3000beae:	2d00      	cmp	r5, #0
3000beb0:	f73f aef5 	bgt.w	3000bc9e <DiagVprintfNano+0x1aa>
3000beb4:	e708      	b.n	3000bcc8 <DiagVprintfNano+0x1d4>
3000beb6:	4680      	mov	r8, r0
3000beb8:	e69d      	b.n	3000bbf6 <DiagVprintfNano+0x102>
3000beba:	f108 38ff 	add.w	r8, r8, #4294967295	; 0xffffffff
3000bebe:	e6b7      	b.n	3000bc30 <DiagVprintfNano+0x13c>
3000bec0:	cccccccd 	.word	0xcccccccd

3000bec4 <DiagPrintfNano>:
3000bec4:	b40f      	push	{r0, r1, r2, r3}
3000bec6:	b500      	push	{lr}
3000bec8:	b083      	sub	sp, #12
3000beca:	a904      	add	r1, sp, #16
3000becc:	f851 0b04 	ldr.w	r0, [r1], #4
3000bed0:	9101      	str	r1, [sp, #4]
3000bed2:	f7ff fe0f 	bl	3000baf4 <DiagVprintfNano>
3000bed6:	b003      	add	sp, #12
3000bed8:	f85d eb04 	ldr.w	lr, [sp], #4
3000bedc:	b004      	add	sp, #16
3000bede:	4770      	bx	lr

3000bee0 <io_assert_failed>:
3000bee0:	460c      	mov	r4, r1
3000bee2:	4b05      	ldr	r3, [pc, #20]	; (3000bef8 <io_assert_failed+0x18>)
3000bee4:	2245      	movs	r2, #69	; 0x45
3000bee6:	4905      	ldr	r1, [pc, #20]	; (3000befc <io_assert_failed+0x1c>)
3000bee8:	b500      	push	{lr}
3000beea:	b083      	sub	sp, #12
3000beec:	e9cd 0400 	strd	r0, r4, [sp]
3000bef0:	2002      	movs	r0, #2
3000bef2:	f7fe fa39 	bl	3000a368 <rtk_log_write_nano>
3000bef6:	e7fe      	b.n	3000bef6 <io_assert_failed+0x16>
3000bef8:	3000e7ac 	.word	0x3000e7ac
3000befc:	3000c810 	.word	0x3000c810

3000bf00 <CRYPTO_SetSecurityModeAD>:
3000bf00:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
3000bf04:	b082      	sub	sp, #8
3000bf06:	e9dd 7609 	ldrd	r7, r6, [sp, #36]	; 0x24
3000bf0a:	2800      	cmp	r0, #0
3000bf0c:	f000 80ab 	beq.w	3000c066 <CRYPTO_SetSecurityModeAD+0x166>
3000bf10:	4698      	mov	r8, r3
3000bf12:	7803      	ldrb	r3, [r0, #0]
3000bf14:	4604      	mov	r4, r0
3000bf16:	2b01      	cmp	r3, #1
3000bf18:	f040 80a9 	bne.w	3000c06e <CRYPTO_SetSecurityModeAD+0x16e>
3000bf1c:	4615      	mov	r5, r2
3000bf1e:	2200      	movs	r2, #0
3000bf20:	6041      	str	r1, [r0, #4]
3000bf22:	8102      	strh	r2, [r0, #8]
3000bf24:	1c4a      	adds	r2, r1, #1
3000bf26:	d009      	beq.n	3000bf3c <CRYPTO_SetSecurityModeAD+0x3c>
3000bf28:	f001 0230 	and.w	r2, r1, #48	; 0x30
3000bf2c:	f081 0180 	eor.w	r1, r1, #128	; 0x80
3000bf30:	2a20      	cmp	r2, #32
3000bf32:	f3c1 11c0 	ubfx	r1, r1, #7, #1
3000bf36:	bf08      	it	eq
3000bf38:	7203      	strbeq	r3, [r0, #8]
3000bf3a:	7241      	strb	r1, [r0, #9]
3000bf3c:	1c6b      	adds	r3, r5, #1
3000bf3e:	60e5      	str	r5, [r4, #12]
3000bf40:	d069      	beq.n	3000c016 <CRYPTO_SetSecurityModeAD+0x116>
3000bf42:	f005 0303 	and.w	r3, r5, #3
3000bf46:	f3c5 0280 	ubfx	r2, r5, #2, #1
3000bf4a:	7422      	strb	r2, [r4, #16]
3000bf4c:	f1a3 0202 	sub.w	r2, r3, #2
3000bf50:	2b01      	cmp	r3, #1
3000bf52:	fab2 f282 	clz	r2, r2
3000bf56:	ea4f 1252 	mov.w	r2, r2, lsr #5
3000bf5a:	7462      	strb	r2, [r4, #17]
3000bf5c:	fab3 f283 	clz	r2, r3
3000bf60:	ea4f 1252 	mov.w	r2, r2, lsr #5
3000bf64:	74a2      	strb	r2, [r4, #18]
3000bf66:	bf0c      	ite	eq
3000bf68:	2201      	moveq	r2, #1
3000bf6a:	2200      	movne	r2, #0
3000bf6c:	74e2      	strb	r2, [r4, #19]
3000bf6e:	d03f      	beq.n	3000bff0 <CRYPTO_SetSecurityModeAD+0xf0>
3000bf70:	2200      	movs	r2, #0
3000bf72:	7522      	strb	r2, [r4, #20]
3000bf74:	2b00      	cmp	r3, #0
3000bf76:	d149      	bne.n	3000c00c <CRYPTO_SetSecurityModeAD+0x10c>
3000bf78:	2314      	movs	r3, #20
3000bf7a:	62a3      	str	r3, [r4, #40]	; 0x28
3000bf7c:	9b08      	ldr	r3, [sp, #32]
3000bf7e:	6226      	str	r6, [r4, #32]
3000bf80:	e9c4 3811 	strd	r3, r8, [r4, #68]	; 0x44
3000bf84:	2e00      	cmp	r6, #0
3000bf86:	d03e      	beq.n	3000c006 <CRYPTO_SetSecurityModeAD+0x106>
3000bf88:	7d23      	ldrb	r3, [r4, #20]
3000bf8a:	f104 0064 	add.w	r0, r4, #100	; 0x64
3000bf8e:	2b30      	cmp	r3, #48	; 0x30
3000bf90:	65e0      	str	r0, [r4, #92]	; 0x5c
3000bf92:	d04b      	beq.n	3000c02c <CRYPTO_SetSecurityModeAD+0x12c>
3000bf94:	2b40      	cmp	r3, #64	; 0x40
3000bf96:	d049      	beq.n	3000c02c <CRYPTO_SetSecurityModeAD+0x12c>
3000bf98:	f104 03a4 	add.w	r3, r4, #164	; 0xa4
3000bf9c:	2240      	movs	r2, #64	; 0x40
3000bf9e:	2136      	movs	r1, #54	; 0x36
3000bfa0:	f8df 80ec 	ldr.w	r8, [pc, #236]	; 3000c090 <CRYPTO_SetSecurityModeAD+0x190>
3000bfa4:	6623      	str	r3, [r4, #96]	; 0x60
3000bfa6:	47c0      	blx	r8
3000bfa8:	2240      	movs	r2, #64	; 0x40
3000bfaa:	215c      	movs	r1, #92	; 0x5c
3000bfac:	6e20      	ldr	r0, [r4, #96]	; 0x60
3000bfae:	47c0      	blx	r8
3000bfb0:	4b33      	ldr	r3, [pc, #204]	; (3000c080 <CRYPTO_SetSecurityModeAD+0x180>)
3000bfb2:	681b      	ldr	r3, [r3, #0]
3000bfb4:	b993      	cbnz	r3, 3000bfdc <CRYPTO_SetSecurityModeAD+0xdc>
3000bfb6:	2f00      	cmp	r7, #0
3000bfb8:	d05d      	beq.n	3000c076 <CRYPTO_SetSecurityModeAD+0x176>
3000bfba:	6267      	str	r7, [r4, #36]	; 0x24
3000bfbc:	e000      	b.n	3000bfc0 <CRYPTO_SetSecurityModeAD+0xc0>
3000bfbe:	6a67      	ldr	r7, [r4, #36]	; 0x24
3000bfc0:	6de1      	ldr	r1, [r4, #92]	; 0x5c
3000bfc2:	5cfa      	ldrb	r2, [r7, r3]
3000bfc4:	5cc8      	ldrb	r0, [r1, r3]
3000bfc6:	4042      	eors	r2, r0
3000bfc8:	54ca      	strb	r2, [r1, r3]
3000bfca:	6e21      	ldr	r1, [r4, #96]	; 0x60
3000bfcc:	6a62      	ldr	r2, [r4, #36]	; 0x24
3000bfce:	5cc8      	ldrb	r0, [r1, r3]
3000bfd0:	5cd2      	ldrb	r2, [r2, r3]
3000bfd2:	4042      	eors	r2, r0
3000bfd4:	54ca      	strb	r2, [r1, r3]
3000bfd6:	3301      	adds	r3, #1
3000bfd8:	429e      	cmp	r6, r3
3000bfda:	d1f0      	bne.n	3000bfbe <CRYPTO_SetSecurityModeAD+0xbe>
3000bfdc:	3501      	adds	r5, #1
3000bfde:	d003      	beq.n	3000bfe8 <CRYPTO_SetSecurityModeAD+0xe8>
3000bfe0:	2201      	movs	r2, #1
3000bfe2:	2300      	movs	r3, #0
3000bfe4:	85a2      	strh	r2, [r4, #44]	; 0x2c
3000bfe6:	6323      	str	r3, [r4, #48]	; 0x30
3000bfe8:	2000      	movs	r0, #0
3000bfea:	b002      	add	sp, #8
3000bfec:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
3000bff0:	f005 03f0 	and.w	r3, r5, #240	; 0xf0
3000bff4:	2b40      	cmp	r3, #64	; 0x40
3000bff6:	d033      	beq.n	3000c060 <CRYPTO_SetSecurityModeAD+0x160>
3000bff8:	d813      	bhi.n	3000c022 <CRYPTO_SetSecurityModeAD+0x122>
3000bffa:	2b10      	cmp	r3, #16
3000bffc:	d023      	beq.n	3000c046 <CRYPTO_SetSecurityModeAD+0x146>
3000bffe:	2b20      	cmp	r3, #32
3000c000:	d124      	bne.n	3000c04c <CRYPTO_SetSecurityModeAD+0x14c>
3000c002:	7523      	strb	r3, [r4, #20]
3000c004:	e00b      	b.n	3000c01e <CRYPTO_SetSecurityModeAD+0x11e>
3000c006:	e9c4 6617 	strd	r6, r6, [r4, #92]	; 0x5c
3000c00a:	e7e7      	b.n	3000bfdc <CRYPTO_SetSecurityModeAD+0xdc>
3000c00c:	2b02      	cmp	r3, #2
3000c00e:	d105      	bne.n	3000c01c <CRYPTO_SetSecurityModeAD+0x11c>
3000c010:	2310      	movs	r3, #16
3000c012:	62a3      	str	r3, [r4, #40]	; 0x28
3000c014:	e7b2      	b.n	3000bf7c <CRYPTO_SetSecurityModeAD+0x7c>
3000c016:	2300      	movs	r3, #0
3000c018:	6123      	str	r3, [r4, #16]
3000c01a:	7523      	strb	r3, [r4, #20]
3000c01c:	2300      	movs	r3, #0
3000c01e:	62a3      	str	r3, [r4, #40]	; 0x28
3000c020:	e7ac      	b.n	3000bf7c <CRYPTO_SetSecurityModeAD+0x7c>
3000c022:	2b80      	cmp	r3, #128	; 0x80
3000c024:	d112      	bne.n	3000c04c <CRYPTO_SetSecurityModeAD+0x14c>
3000c026:	2340      	movs	r3, #64	; 0x40
3000c028:	7523      	strb	r3, [r4, #20]
3000c02a:	e7f8      	b.n	3000c01e <CRYPTO_SetSecurityModeAD+0x11e>
3000c02c:	f104 03e4 	add.w	r3, r4, #228	; 0xe4
3000c030:	2280      	movs	r2, #128	; 0x80
3000c032:	2136      	movs	r1, #54	; 0x36
3000c034:	f8df 8058 	ldr.w	r8, [pc, #88]	; 3000c090 <CRYPTO_SetSecurityModeAD+0x190>
3000c038:	6623      	str	r3, [r4, #96]	; 0x60
3000c03a:	47c0      	blx	r8
3000c03c:	2280      	movs	r2, #128	; 0x80
3000c03e:	215c      	movs	r1, #92	; 0x5c
3000c040:	6e20      	ldr	r0, [r4, #96]	; 0x60
3000c042:	47c0      	blx	r8
3000c044:	e7b4      	b.n	3000bfb0 <CRYPTO_SetSecurityModeAD+0xb0>
3000c046:	231c      	movs	r3, #28
3000c048:	7523      	strb	r3, [r4, #20]
3000c04a:	e7e8      	b.n	3000c01e <CRYPTO_SetSecurityModeAD+0x11e>
3000c04c:	4b0d      	ldr	r3, [pc, #52]	; (3000c084 <CRYPTO_SetSecurityModeAD+0x184>)
3000c04e:	2245      	movs	r2, #69	; 0x45
3000c050:	490d      	ldr	r1, [pc, #52]	; (3000c088 <CRYPTO_SetSecurityModeAD+0x188>)
3000c052:	2002      	movs	r0, #2
3000c054:	9500      	str	r5, [sp, #0]
3000c056:	f7fe f95f 	bl	3000a318 <rtk_log_write>
3000c05a:	2300      	movs	r3, #0
3000c05c:	7523      	strb	r3, [r4, #20]
3000c05e:	e7de      	b.n	3000c01e <CRYPTO_SetSecurityModeAD+0x11e>
3000c060:	2330      	movs	r3, #48	; 0x30
3000c062:	7523      	strb	r3, [r4, #20]
3000c064:	e7db      	b.n	3000c01e <CRYPTO_SetSecurityModeAD+0x11e>
3000c066:	21ce      	movs	r1, #206	; 0xce
3000c068:	4808      	ldr	r0, [pc, #32]	; (3000c08c <CRYPTO_SetSecurityModeAD+0x18c>)
3000c06a:	f7ff ff39 	bl	3000bee0 <io_assert_failed>
3000c06e:	21cf      	movs	r1, #207	; 0xcf
3000c070:	4806      	ldr	r0, [pc, #24]	; (3000c08c <CRYPTO_SetSecurityModeAD+0x18c>)
3000c072:	f7ff ff35 	bl	3000bee0 <io_assert_failed>
3000c076:	f44f 7195 	mov.w	r1, #298	; 0x12a
3000c07a:	4804      	ldr	r0, [pc, #16]	; (3000c08c <CRYPTO_SetSecurityModeAD+0x18c>)
3000c07c:	f7ff ff30 	bl	3000bee0 <io_assert_failed>
3000c080:	2001c200 	.word	0x2001c200
3000c084:	3000e858 	.word	0x3000e858
3000c088:	3000e770 	.word	0x3000e770
3000c08c:	3000e7e4 	.word	0x3000e7e4
3000c090:	00012be5 	.word	0x00012be5

3000c094 <rtl_crypto_hmac_sha2_init>:
3000c094:	b570      	push	{r4, r5, r6, lr}
3000c096:	4c94      	ldr	r4, [pc, #592]	; (3000c2e8 <rtl_crypto_hmac_sha2_init+0x254>)
3000c098:	460d      	mov	r5, r1
3000c09a:	e844 f400 	tt	r4, r4
3000c09e:	4611      	mov	r1, r2
3000c0a0:	f414 0f80 	tst.w	r4, #4194304	; 0x400000
3000c0a4:	4a91      	ldr	r2, [pc, #580]	; (3000c2ec <rtl_crypto_hmac_sha2_init+0x258>)
3000c0a6:	461c      	mov	r4, r3
3000c0a8:	4b91      	ldr	r3, [pc, #580]	; (3000c2f0 <rtl_crypto_hmac_sha2_init+0x25c>)
3000c0aa:	b084      	sub	sp, #16
3000c0ac:	bf18      	it	ne
3000c0ae:	4613      	movne	r3, r2
3000c0b0:	6b5b      	ldr	r3, [r3, #52]	; 0x34
3000c0b2:	2b00      	cmp	r3, #0
3000c0b4:	db05      	blt.n	3000c0c2 <rtl_crypto_hmac_sha2_init+0x2e>
3000c0b6:	2d00      	cmp	r5, #0
3000c0b8:	f000 8109 	beq.w	3000c2ce <rtl_crypto_hmac_sha2_init+0x23a>
3000c0bc:	07ae      	lsls	r6, r5, #30
3000c0be:	f040 8103 	bne.w	3000c2c8 <rtl_crypto_hmac_sha2_init+0x234>
3000c0c2:	2820      	cmp	r0, #32
3000c0c4:	bf18      	it	ne
3000c0c6:	281c      	cmpne	r0, #28
3000c0c8:	bf0c      	ite	eq
3000c0ca:	2301      	moveq	r3, #1
3000c0cc:	2300      	movne	r3, #0
3000c0ce:	d02e      	beq.n	3000c12e <rtl_crypto_hmac_sha2_init+0x9a>
3000c0d0:	2840      	cmp	r0, #64	; 0x40
3000c0d2:	bf18      	it	ne
3000c0d4:	2830      	cmpne	r0, #48	; 0x30
3000c0d6:	d102      	bne.n	3000c0de <rtl_crypto_hmac_sha2_init+0x4a>
3000c0d8:	2980      	cmp	r1, #128	; 0x80
3000c0da:	f200 8100 	bhi.w	3000c2de <rtl_crypto_hmac_sha2_init+0x24a>
3000c0de:	4e85      	ldr	r6, [pc, #532]	; (3000c2f4 <rtl_crypto_hmac_sha2_init+0x260>)
3000c0e0:	7832      	ldrb	r2, [r6, #0]
3000c0e2:	2a01      	cmp	r2, #1
3000c0e4:	f040 80f6 	bne.w	3000c2d4 <rtl_crypto_hmac_sha2_init+0x240>
3000c0e8:	2830      	cmp	r0, #48	; 0x30
3000c0ea:	f000 80b4 	beq.w	3000c256 <rtl_crypto_hmac_sha2_init+0x1c2>
3000c0ee:	d825      	bhi.n	3000c13c <rtl_crypto_hmac_sha2_init+0xa8>
3000c0f0:	281c      	cmp	r0, #28
3000c0f2:	f000 8091 	beq.w	3000c218 <rtl_crypto_hmac_sha2_init+0x184>
3000c0f6:	2b00      	cmp	r3, #0
3000c0f8:	d075      	beq.n	3000c1e6 <rtl_crypto_hmac_sha2_init+0x152>
3000c0fa:	2c00      	cmp	r4, #0
3000c0fc:	d078      	beq.n	3000c1f0 <rtl_crypto_hmac_sha2_init+0x15c>
3000c0fe:	4a7e      	ldr	r2, [pc, #504]	; (3000c2f8 <rtl_crypto_hmac_sha2_init+0x264>)
3000c100:	4b7e      	ldr	r3, [pc, #504]	; (3000c2fc <rtl_crypto_hmac_sha2_init+0x268>)
3000c102:	487f      	ldr	r0, [pc, #508]	; (3000c300 <rtl_crypto_hmac_sha2_init+0x26c>)
3000c104:	f8df c218 	ldr.w	ip, [pc, #536]	; 3000c320 <rtl_crypto_hmac_sha2_init+0x28c>
3000c108:	e9c4 2302 	strd	r2, r3, [r4, #8]
3000c10c:	4b7d      	ldr	r3, [pc, #500]	; (3000c304 <rtl_crypto_hmac_sha2_init+0x270>)
3000c10e:	2225      	movs	r2, #37	; 0x25
3000c110:	e9c4 0304 	strd	r0, r3, [r4, #16]
3000c114:	4b7c      	ldr	r3, [pc, #496]	; (3000c308 <rtl_crypto_hmac_sha2_init+0x274>)
3000c116:	e9c4 c306 	strd	ip, r3, [r4, #24]
3000c11a:	4b7c      	ldr	r3, [pc, #496]	; (3000c30c <rtl_crypto_hmac_sha2_init+0x278>)
3000c11c:	6223      	str	r3, [r4, #32]
3000c11e:	f103 5374 	add.w	r3, r3, #1023410176	; 0x3d000000
3000c122:	f5a3 0323 	sub.w	r3, r3, #10682368	; 0xa30000
3000c126:	f6a3 4392 	subw	r3, r3, #3218	; 0xc92
3000c12a:	6263      	str	r3, [r4, #36]	; 0x24
3000c12c:	e044      	b.n	3000c1b8 <rtl_crypto_hmac_sha2_init+0x124>
3000c12e:	2940      	cmp	r1, #64	; 0x40
3000c130:	d9d5      	bls.n	3000c0de <rtl_crypto_hmac_sha2_init+0x4a>
3000c132:	f44f 71ac 	mov.w	r1, #344	; 0x158
3000c136:	4876      	ldr	r0, [pc, #472]	; (3000c310 <rtl_crypto_hmac_sha2_init+0x27c>)
3000c138:	f7ff fed2 	bl	3000bee0 <io_assert_failed>
3000c13c:	2840      	cmp	r0, #64	; 0x40
3000c13e:	d152      	bne.n	3000c1e6 <rtl_crypto_hmac_sha2_init+0x152>
3000c140:	2c00      	cmp	r4, #0
3000c142:	d067      	beq.n	3000c214 <rtl_crypto_hmac_sha2_init+0x180>
3000c144:	4b6c      	ldr	r3, [pc, #432]	; (3000c2f8 <rtl_crypto_hmac_sha2_init+0x264>)
3000c146:	4a73      	ldr	r2, [pc, #460]	; (3000c314 <rtl_crypto_hmac_sha2_init+0x280>)
3000c148:	4873      	ldr	r0, [pc, #460]	; (3000c318 <rtl_crypto_hmac_sha2_init+0x284>)
3000c14a:	f8df e21c 	ldr.w	lr, [pc, #540]	; 3000c368 <rtl_crypto_hmac_sha2_init+0x2d4>
3000c14e:	e9c4 3202 	strd	r3, r2, [r4, #8]
3000c152:	f103 53d6 	add.w	r3, r3, #448790528	; 0x1ac00000
3000c156:	4a71      	ldr	r2, [pc, #452]	; (3000c31c <rtl_crypto_hmac_sha2_init+0x288>)
3000c158:	f503 4340 	add.w	r3, r3, #49152	; 0xc000
3000c15c:	33d4      	adds	r3, #212	; 0xd4
3000c15e:	6163      	str	r3, [r4, #20]
3000c160:	f103 535a 	add.w	r3, r3, #914358272	; 0x36800000
3000c164:	f503 13e8 	add.w	r3, r3, #1900544	; 0x1d0000
3000c168:	f203 734a 	addw	r3, r3, #1866	; 0x74a
3000c16c:	6123      	str	r3, [r4, #16]
3000c16e:	4b64      	ldr	r3, [pc, #400]	; (3000c300 <rtl_crypto_hmac_sha2_init+0x26c>)
3000c170:	e9c4 3006 	strd	r3, r0, [r4, #24]
3000c174:	f103 530a 	add.w	r3, r3, #578813952	; 0x22800000
3000c178:	f503 1339 	add.w	r3, r3, #3031040	; 0x2e4000
3000c17c:	f203 337f 	addw	r3, r3, #895	; 0x37f
3000c180:	6263      	str	r3, [r4, #36]	; 0x24
3000c182:	4b60      	ldr	r3, [pc, #384]	; (3000c304 <rtl_crypto_hmac_sha2_init+0x270>)
3000c184:	6223      	str	r3, [r4, #32]
3000c186:	f103 6309 	add.w	r3, r3, #143654912	; 0x8900000
3000c18a:	f503 23d1 	add.w	r3, r3, #428032	; 0x68800
3000c18e:	f203 5397 	addw	r3, r3, #1431	; 0x597
3000c192:	62e3      	str	r3, [r4, #44]	; 0x2c
3000c194:	4b62      	ldr	r3, [pc, #392]	; (3000c320 <rtl_crypto_hmac_sha2_init+0x28c>)
3000c196:	62a3      	str	r3, [r4, #40]	; 0x28
3000c198:	f103 435a 	add.w	r3, r3, #3657433088	; 0xda000000
3000c19c:	f503 1340 	add.w	r3, r3, #3145728	; 0x300000
3000c1a0:	f503 53cd 	add.w	r3, r3, #6560	; 0x19a0
3000c1a4:	6363      	str	r3, [r4, #52]	; 0x34
3000c1a6:	4b58      	ldr	r3, [pc, #352]	; (3000c308 <rtl_crypto_hmac_sha2_init+0x274>)
3000c1a8:	6323      	str	r3, [r4, #48]	; 0x30
3000c1aa:	4b58      	ldr	r3, [pc, #352]	; (3000c30c <rtl_crypto_hmac_sha2_init+0x278>)
3000c1ac:	e9c4 3e0e 	strd	r3, lr, [r4, #56]	; 0x38
3000c1b0:	4b5c      	ldr	r3, [pc, #368]	; (3000c324 <rtl_crypto_hmac_sha2_init+0x290>)
3000c1b2:	e9c4 3210 	strd	r3, r2, [r4, #64]	; 0x40
3000c1b6:	2285      	movs	r2, #133	; 0x85
3000c1b8:	2300      	movs	r3, #0
3000c1ba:	9102      	str	r1, [sp, #8]
3000c1bc:	9501      	str	r5, [sp, #4]
3000c1be:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
3000c1c2:	9300      	str	r3, [sp, #0]
3000c1c4:	4d58      	ldr	r5, [pc, #352]	; (3000c328 <rtl_crypto_hmac_sha2_init+0x294>)
3000c1c6:	484b      	ldr	r0, [pc, #300]	; (3000c2f4 <rtl_crypto_hmac_sha2_init+0x260>)
3000c1c8:	f886 3034 	strb.w	r3, [r6, #52]	; 0x34
3000c1cc:	f886 3040 	strb.w	r3, [r6, #64]	; 0x40
3000c1d0:	47a8      	blx	r5
3000c1d2:	4605      	mov	r5, r0
3000c1d4:	4b55      	ldr	r3, [pc, #340]	; (3000c32c <rtl_crypto_hmac_sha2_init+0x298>)
3000c1d6:	4620      	mov	r0, r4
3000c1d8:	4798      	blx	r3
3000c1da:	2301      	movs	r3, #1
3000c1dc:	4628      	mov	r0, r5
3000c1de:	f884 304b 	strb.w	r3, [r4, #75]	; 0x4b
3000c1e2:	b004      	add	sp, #16
3000c1e4:	bd70      	pop	{r4, r5, r6, pc}
3000c1e6:	f04f 35ff 	mov.w	r5, #4294967295	; 0xffffffff
3000c1ea:	4628      	mov	r0, r5
3000c1ec:	b004      	add	sp, #16
3000c1ee:	bd70      	pop	{r4, r5, r6, pc}
3000c1f0:	2225      	movs	r2, #37	; 0x25
3000c1f2:	2300      	movs	r3, #0
3000c1f4:	9102      	str	r1, [sp, #8]
3000c1f6:	4c4c      	ldr	r4, [pc, #304]	; (3000c328 <rtl_crypto_hmac_sha2_init+0x294>)
3000c1f8:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
3000c1fc:	483d      	ldr	r0, [pc, #244]	; (3000c2f4 <rtl_crypto_hmac_sha2_init+0x260>)
3000c1fe:	f886 3034 	strb.w	r3, [r6, #52]	; 0x34
3000c202:	f886 3040 	strb.w	r3, [r6, #64]	; 0x40
3000c206:	e9cd 3500 	strd	r3, r5, [sp]
3000c20a:	47a0      	blx	r4
3000c20c:	4605      	mov	r5, r0
3000c20e:	4628      	mov	r0, r5
3000c210:	b004      	add	sp, #16
3000c212:	bd70      	pop	{r4, r5, r6, pc}
3000c214:	2285      	movs	r2, #133	; 0x85
3000c216:	e7ec      	b.n	3000c1f2 <rtl_crypto_hmac_sha2_init+0x15e>
3000c218:	2c00      	cmp	r4, #0
3000c21a:	d051      	beq.n	3000c2c0 <rtl_crypto_hmac_sha2_init+0x22c>
3000c21c:	4b44      	ldr	r3, [pc, #272]	; (3000c330 <rtl_crypto_hmac_sha2_init+0x29c>)
3000c21e:	f8df e128 	ldr.w	lr, [pc, #296]	; 3000c348 <rtl_crypto_hmac_sha2_init+0x2b4>
3000c222:	4a44      	ldr	r2, [pc, #272]	; (3000c334 <rtl_crypto_hmac_sha2_init+0x2a0>)
3000c224:	4844      	ldr	r0, [pc, #272]	; (3000c338 <rtl_crypto_hmac_sha2_init+0x2a4>)
3000c226:	e9c4 e302 	strd	lr, r3, [r4, #8]
3000c22a:	f103 4379 	add.w	r3, r3, #4177526784	; 0xf9000000
3000c22e:	f503 0374 	add.w	r3, r3, #15990784	; 0xf40000
3000c232:	f503 6301 	add.w	r3, r3, #2064	; 0x810
3000c236:	6123      	str	r3, [r4, #16]
3000c238:	4b40      	ldr	r3, [pc, #256]	; (3000c33c <rtl_crypto_hmac_sha2_init+0x2a8>)
3000c23a:	e9c4 2305 	strd	r2, r3, [r4, #20]
3000c23e:	f103 43d1 	add.w	r3, r3, #1753219072	; 0x68800000
3000c242:	2215      	movs	r2, #21
3000c244:	f503 13c0 	add.w	r3, r3, #1572864	; 0x180000
3000c248:	f503 631e 	add.w	r3, r3, #2528	; 0x9e0
3000c24c:	61e3      	str	r3, [r4, #28]
3000c24e:	4b3c      	ldr	r3, [pc, #240]	; (3000c340 <rtl_crypto_hmac_sha2_init+0x2ac>)
3000c250:	e9c4 0308 	strd	r0, r3, [r4, #32]
3000c254:	e7b0      	b.n	3000c1b8 <rtl_crypto_hmac_sha2_init+0x124>
3000c256:	2c00      	cmp	r4, #0
3000c258:	d034      	beq.n	3000c2c4 <rtl_crypto_hmac_sha2_init+0x230>
3000c25a:	4b3a      	ldr	r3, [pc, #232]	; (3000c344 <rtl_crypto_hmac_sha2_init+0x2b0>)
3000c25c:	483a      	ldr	r0, [pc, #232]	; (3000c348 <rtl_crypto_hmac_sha2_init+0x2b4>)
3000c25e:	f8df e0d4 	ldr.w	lr, [pc, #212]	; 3000c334 <rtl_crypto_hmac_sha2_init+0x2a0>
3000c262:	4a36      	ldr	r2, [pc, #216]	; (3000c33c <rtl_crypto_hmac_sha2_init+0x2a8>)
3000c264:	f8df c0d0 	ldr.w	ip, [pc, #208]	; 3000c338 <rtl_crypto_hmac_sha2_init+0x2a4>
3000c268:	e9c4 3002 	strd	r3, r0, [r4, #8]
3000c26c:	f103 43d6 	add.w	r3, r3, #1795162112	; 0x6b000000
3000c270:	f5a3 137b 	sub.w	r3, r3, #4112384	; 0x3ec000
3000c274:	f6a3 0356 	subw	r3, r3, #2134	; 0x856
3000c278:	6163      	str	r3, [r4, #20]
3000c27a:	4b34      	ldr	r3, [pc, #208]	; (3000c34c <rtl_crypto_hmac_sha2_init+0x2b8>)
3000c27c:	6123      	str	r3, [r4, #16]
3000c27e:	f103 434e 	add.w	r3, r3, #3456106496	; 0xce000000
3000c282:	f5a3 1325 	sub.w	r3, r3, #2703360	; 0x294000
3000c286:	f6a3 4313 	subw	r3, r3, #3091	; 0xc13
3000c28a:	61e3      	str	r3, [r4, #28]
3000c28c:	4b30      	ldr	r3, [pc, #192]	; (3000c350 <rtl_crypto_hmac_sha2_init+0x2bc>)
3000c28e:	61a3      	str	r3, [r4, #24]
3000c290:	4b30      	ldr	r3, [pc, #192]	; (3000c354 <rtl_crypto_hmac_sha2_init+0x2c0>)
3000c292:	e9c4 3e08 	strd	r3, lr, [r4, #32]
3000c296:	4b30      	ldr	r3, [pc, #192]	; (3000c358 <rtl_crypto_hmac_sha2_init+0x2c4>)
3000c298:	f8df e0a4 	ldr.w	lr, [pc, #164]	; 3000c340 <rtl_crypto_hmac_sha2_init+0x2ac>
3000c29c:	e9c4 320a 	strd	r3, r2, [r4, #40]	; 0x28
3000c2a0:	f103 7392 	add.w	r3, r3, #19136512	; 0x1240000
3000c2a4:	2245      	movs	r2, #69	; 0x45
3000c2a6:	f503 436e 	add.w	r3, r3, #60928	; 0xee00
3000c2aa:	33aa      	adds	r3, #170	; 0xaa
3000c2ac:	6363      	str	r3, [r4, #52]	; 0x34
3000c2ae:	4b2b      	ldr	r3, [pc, #172]	; (3000c35c <rtl_crypto_hmac_sha2_init+0x2c8>)
3000c2b0:	6323      	str	r3, [r4, #48]	; 0x30
3000c2b2:	4b2b      	ldr	r3, [pc, #172]	; (3000c360 <rtl_crypto_hmac_sha2_init+0x2cc>)
3000c2b4:	e9c4 3c0e 	strd	r3, ip, [r4, #56]	; 0x38
3000c2b8:	4b2a      	ldr	r3, [pc, #168]	; (3000c364 <rtl_crypto_hmac_sha2_init+0x2d0>)
3000c2ba:	e9c4 3e10 	strd	r3, lr, [r4, #64]	; 0x40
3000c2be:	e77b      	b.n	3000c1b8 <rtl_crypto_hmac_sha2_init+0x124>
3000c2c0:	2215      	movs	r2, #21
3000c2c2:	e796      	b.n	3000c1f2 <rtl_crypto_hmac_sha2_init+0x15e>
3000c2c4:	2245      	movs	r2, #69	; 0x45
3000c2c6:	e794      	b.n	3000c1f2 <rtl_crypto_hmac_sha2_init+0x15e>
3000c2c8:	f06f 0505 	mvn.w	r5, #5
3000c2cc:	e78d      	b.n	3000c1ea <rtl_crypto_hmac_sha2_init+0x156>
3000c2ce:	f06f 0503 	mvn.w	r5, #3
3000c2d2:	e78a      	b.n	3000c1ea <rtl_crypto_hmac_sha2_init+0x156>
3000c2d4:	f44f 71af 	mov.w	r1, #350	; 0x15e
3000c2d8:	480d      	ldr	r0, [pc, #52]	; (3000c310 <rtl_crypto_hmac_sha2_init+0x27c>)
3000c2da:	f7ff fe01 	bl	3000bee0 <io_assert_failed>
3000c2de:	f240 115b 	movw	r1, #347	; 0x15b
3000c2e2:	480b      	ldr	r0, [pc, #44]	; (3000c310 <rtl_crypto_hmac_sha2_init+0x27c>)
3000c2e4:	f7ff fdfc 	bl	3000bee0 <io_assert_failed>
3000c2e8:	3000bad9 	.word	0x3000bad9
3000c2ec:	500c8000 	.word	0x500c8000
3000c2f0:	400c8000 	.word	0x400c8000
3000c2f4:	2001c460 	.word	0x2001c460
3000c2f8:	6a09e667 	.word	0x6a09e667
3000c2fc:	bb67ae85 	.word	0xbb67ae85
3000c300:	3c6ef372 	.word	0x3c6ef372
3000c304:	a54ff53a 	.word	0xa54ff53a
3000c308:	9b05688c 	.word	0x9b05688c
3000c30c:	1f83d9ab 	.word	0x1f83d9ab
3000c310:	3000e7e4 	.word	0x3000e7e4
3000c314:	f3bcc908 	.word	0xf3bcc908
3000c318:	fe94f82b 	.word	0xfe94f82b
3000c31c:	137e2179 	.word	0x137e2179
3000c320:	510e527f 	.word	0x510e527f
3000c324:	5be0cd19 	.word	0x5be0cd19
3000c328:	3000bf01 	.word	0x3000bf01
3000c32c:	00004c99 	.word	0x00004c99
3000c330:	367cd507 	.word	0x367cd507
3000c334:	f70e5939 	.word	0xf70e5939
3000c338:	64f98fa7 	.word	0x64f98fa7
3000c33c:	ffc00b31 	.word	0xffc00b31
3000c340:	befa4fa4 	.word	0xbefa4fa4
3000c344:	cbbb9d5d 	.word	0xcbbb9d5d
3000c348:	c1059ed8 	.word	0xc1059ed8
3000c34c:	629a292a 	.word	0x629a292a
3000c350:	9159015a 	.word	0x9159015a
3000c354:	152fecd8 	.word	0x152fecd8
3000c358:	67332667 	.word	0x67332667
3000c35c:	8eb44a87 	.word	0x8eb44a87
3000c360:	db0c2e0d 	.word	0xdb0c2e0d
3000c364:	47b5481d 	.word	0x47b5481d
3000c368:	fb41bd6b 	.word	0xfb41bd6b

3000c36c <RSIP_OTF_Enable>:
3000c36c:	b1c3      	cbz	r3, 3000c3a0 <RSIP_OTF_Enable+0x34>
3000c36e:	b510      	push	{r4, lr}
3000c370:	9b02      	ldr	r3, [sp, #8]
3000c372:	2b07      	cmp	r3, #7
3000c374:	d81c      	bhi.n	3000c3b0 <RSIP_OTF_Enable+0x44>
3000c376:	9b03      	ldr	r3, [sp, #12]
3000c378:	3b01      	subs	r3, #1
3000c37a:	2b01      	cmp	r3, #1
3000c37c:	d81d      	bhi.n	3000c3ba <RSIP_OTF_Enable+0x4e>
3000c37e:	4b11      	ldr	r3, [pc, #68]	; (3000c3c4 <RSIP_OTF_Enable+0x58>)
3000c380:	9c03      	ldr	r4, [sp, #12]
3000c382:	eb03 00c0 	add.w	r0, r3, r0, lsl #3
3000c386:	ea42 0244 	orr.w	r2, r2, r4, lsl #1
3000c38a:	9c02      	ldr	r4, [sp, #8]
3000c38c:	6c43      	ldr	r3, [r0, #68]	; 0x44
3000c38e:	4321      	orrs	r1, r4
3000c390:	f042 0201 	orr.w	r2, r2, #1
3000c394:	f023 0301 	bic.w	r3, r3, #1
3000c398:	6443      	str	r3, [r0, #68]	; 0x44
3000c39a:	6401      	str	r1, [r0, #64]	; 0x40
3000c39c:	6442      	str	r2, [r0, #68]	; 0x44
3000c39e:	bd10      	pop	{r4, pc}
3000c3a0:	4b08      	ldr	r3, [pc, #32]	; (3000c3c4 <RSIP_OTF_Enable+0x58>)
3000c3a2:	eb03 00c0 	add.w	r0, r3, r0, lsl #3
3000c3a6:	6c43      	ldr	r3, [r0, #68]	; 0x44
3000c3a8:	f023 0301 	bic.w	r3, r3, #1
3000c3ac:	6443      	str	r3, [r0, #68]	; 0x44
3000c3ae:	4770      	bx	lr
3000c3b0:	f44f 7153 	mov.w	r1, #844	; 0x34c
3000c3b4:	4804      	ldr	r0, [pc, #16]	; (3000c3c8 <RSIP_OTF_Enable+0x5c>)
3000c3b6:	f7ff fd93 	bl	3000bee0 <io_assert_failed>
3000c3ba:	f240 314d 	movw	r1, #845	; 0x34d
3000c3be:	4802      	ldr	r0, [pc, #8]	; (3000c3c8 <RSIP_OTF_Enable+0x5c>)
3000c3c0:	f7ff fd8e 	bl	3000bee0 <io_assert_failed>
3000c3c4:	42008c00 	.word	0x42008c00
3000c3c8:	3000e7e4 	.word	0x3000e7e4

3000c3cc <BOOT_VerCheck>:
3000c3cc:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
3000c3d0:	4b20      	ldr	r3, [pc, #128]	; (3000c454 <BOOT_VerCheck+0x88>)
3000c3d2:	4798      	blx	r3
3000c3d4:	b108      	cbz	r0, 3000c3da <BOOT_VerCheck+0xe>
3000c3d6:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
3000c3da:	2503      	movs	r5, #3
3000c3dc:	4c1e      	ldr	r4, [pc, #120]	; (3000c458 <BOOT_VerCheck+0x8c>)
3000c3de:	4b1f      	ldr	r3, [pc, #124]	; (3000c45c <BOOT_VerCheck+0x90>)
3000c3e0:	2257      	movs	r2, #87	; 0x57
3000c3e2:	4621      	mov	r1, r4
3000c3e4:	4628      	mov	r0, r5
3000c3e6:	4e1e      	ldr	r6, [pc, #120]	; (3000c460 <BOOT_VerCheck+0x94>)
3000c3e8:	f8df 808c 	ldr.w	r8, [pc, #140]	; 3000c478 <BOOT_VerCheck+0xac>
3000c3ec:	4f1d      	ldr	r7, [pc, #116]	; (3000c464 <BOOT_VerCheck+0x98>)
3000c3ee:	f7fd ff93 	bl	3000a318 <rtk_log_write>
3000c3f2:	2257      	movs	r2, #87	; 0x57
3000c3f4:	4621      	mov	r1, r4
3000c3f6:	2003      	movs	r0, #3
3000c3f8:	4633      	mov	r3, r6
3000c3fa:	f7fd ff8d 	bl	3000a318 <rtk_log_write>
3000c3fe:	4643      	mov	r3, r8
3000c400:	2257      	movs	r2, #87	; 0x57
3000c402:	4621      	mov	r1, r4
3000c404:	2003      	movs	r0, #3
3000c406:	f7fd ff87 	bl	3000a318 <rtk_log_write>
3000c40a:	463b      	mov	r3, r7
3000c40c:	2257      	movs	r2, #87	; 0x57
3000c40e:	4621      	mov	r1, r4
3000c410:	2003      	movs	r0, #3
3000c412:	f7fd ff81 	bl	3000a318 <rtk_log_write>
3000c416:	2257      	movs	r2, #87	; 0x57
3000c418:	4621      	mov	r1, r4
3000c41a:	2003      	movs	r0, #3
3000c41c:	4b12      	ldr	r3, [pc, #72]	; (3000c468 <BOOT_VerCheck+0x9c>)
3000c41e:	f7fd ff7b 	bl	3000a318 <rtk_log_write>
3000c422:	4633      	mov	r3, r6
3000c424:	2257      	movs	r2, #87	; 0x57
3000c426:	4621      	mov	r1, r4
3000c428:	2003      	movs	r0, #3
3000c42a:	f7fd ff75 	bl	3000a318 <rtk_log_write>
3000c42e:	4b0f      	ldr	r3, [pc, #60]	; (3000c46c <BOOT_VerCheck+0xa0>)
3000c430:	2257      	movs	r2, #87	; 0x57
3000c432:	4621      	mov	r1, r4
3000c434:	2003      	movs	r0, #3
3000c436:	f7fd ff6f 	bl	3000a318 <rtk_log_write>
3000c43a:	3d01      	subs	r5, #1
3000c43c:	d1d9      	bne.n	3000c3f2 <BOOT_VerCheck+0x26>
3000c43e:	4906      	ldr	r1, [pc, #24]	; (3000c458 <BOOT_VerCheck+0x8c>)
3000c440:	2003      	movs	r0, #3
3000c442:	4b0b      	ldr	r3, [pc, #44]	; (3000c470 <BOOT_VerCheck+0xa4>)
3000c444:	2257      	movs	r2, #87	; 0x57
3000c446:	f7fd ff67 	bl	3000a318 <rtk_log_write>
3000c44a:	211e      	movs	r1, #30
3000c44c:	4809      	ldr	r0, [pc, #36]	; (3000c474 <BOOT_VerCheck+0xa8>)
3000c44e:	f7ff fd47 	bl	3000bee0 <io_assert_failed>
3000c452:	bf00      	nop
3000c454:	3000b535 	.word	0x3000b535
3000c458:	3000c7c4 	.word	0x3000c7c4
3000c45c:	3000e87c 	.word	0x3000e87c
3000c460:	3000e8b8 	.word	0x3000e8b8
3000c464:	3000e930 	.word	0x3000e930
3000c468:	3000e96c 	.word	0x3000e96c
3000c46c:	3000e9a8 	.word	0x3000e9a8
3000c470:	3000c7c0 	.word	0x3000c7c0
3000c474:	3000e9e4 	.word	0x3000e9e4
3000c478:	3000e8f4 	.word	0x3000e8f4

3000c47c <__aeabi_uldivmod>:
3000c47c:	b953      	cbnz	r3, 3000c494 <__aeabi_uldivmod+0x18>
3000c47e:	b94a      	cbnz	r2, 3000c494 <__aeabi_uldivmod+0x18>
3000c480:	2900      	cmp	r1, #0
3000c482:	bf08      	it	eq
3000c484:	2800      	cmpeq	r0, #0
3000c486:	bf1c      	itt	ne
3000c488:	f04f 31ff 	movne.w	r1, #4294967295	; 0xffffffff
3000c48c:	f04f 30ff 	movne.w	r0, #4294967295	; 0xffffffff
3000c490:	f000 b974 	b.w	3000c77c <__aeabi_idiv0>
3000c494:	f1ad 0c08 	sub.w	ip, sp, #8
3000c498:	e96d ce04 	strd	ip, lr, [sp, #-16]!
3000c49c:	f000 f819 	bl	3000c4d2 <__udivmoddi4>
3000c4a0:	f8dd e004 	ldr.w	lr, [sp, #4]
3000c4a4:	e9dd 2302 	ldrd	r2, r3, [sp, #8]
3000c4a8:	b004      	add	sp, #16
3000c4aa:	4770      	bx	lr

3000c4ac <__popcountsi2>:
3000c4ac:	0843      	lsrs	r3, r0, #1
3000c4ae:	f003 3355 	and.w	r3, r3, #1431655765	; 0x55555555
3000c4b2:	1ac0      	subs	r0, r0, r3
3000c4b4:	f000 3333 	and.w	r3, r0, #858993459	; 0x33333333
3000c4b8:	0880      	lsrs	r0, r0, #2
3000c4ba:	f000 3033 	and.w	r0, r0, #858993459	; 0x33333333
3000c4be:	4418      	add	r0, r3
3000c4c0:	f04f 3301 	mov.w	r3, #16843009	; 0x1010101
3000c4c4:	eb00 1010 	add.w	r0, r0, r0, lsr #4
3000c4c8:	f000 300f 	and.w	r0, r0, #252645135	; 0xf0f0f0f
3000c4cc:	4358      	muls	r0, r3
3000c4ce:	0e00      	lsrs	r0, r0, #24
3000c4d0:	4770      	bx	lr

3000c4d2 <__udivmoddi4>:
3000c4d2:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
3000c4d6:	9e09      	ldr	r6, [sp, #36]	; 0x24
3000c4d8:	4688      	mov	r8, r1
3000c4da:	468e      	mov	lr, r1
3000c4dc:	4615      	mov	r5, r2
3000c4de:	4604      	mov	r4, r0
3000c4e0:	4619      	mov	r1, r3
3000c4e2:	2b00      	cmp	r3, #0
3000c4e4:	f040 80c6 	bne.w	3000c674 <__udivmoddi4+0x1a2>
3000c4e8:	4542      	cmp	r2, r8
3000c4ea:	fab2 f782 	clz	r7, r2
3000c4ee:	d946      	bls.n	3000c57e <__udivmoddi4+0xac>
3000c4f0:	b14f      	cbz	r7, 3000c506 <__udivmoddi4+0x34>
3000c4f2:	f1c7 0c20 	rsb	ip, r7, #32
3000c4f6:	fa08 f307 	lsl.w	r3, r8, r7
3000c4fa:	40bd      	lsls	r5, r7
3000c4fc:	40bc      	lsls	r4, r7
3000c4fe:	fa20 fc0c 	lsr.w	ip, r0, ip
3000c502:	ea4c 0e03 	orr.w	lr, ip, r3
3000c506:	ea4f 4815 	mov.w	r8, r5, lsr #16
3000c50a:	fa1f fc85 	uxth.w	ip, r5
3000c50e:	0c22      	lsrs	r2, r4, #16
3000c510:	fbbe f9f8 	udiv	r9, lr, r8
3000c514:	fb08 e319 	mls	r3, r8, r9, lr
3000c518:	fb09 fa0c 	mul.w	sl, r9, ip
3000c51c:	ea42 4303 	orr.w	r3, r2, r3, lsl #16
3000c520:	459a      	cmp	sl, r3
3000c522:	d928      	bls.n	3000c576 <__udivmoddi4+0xa4>
3000c524:	18eb      	adds	r3, r5, r3
3000c526:	f109 30ff 	add.w	r0, r9, #4294967295	; 0xffffffff
3000c52a:	d204      	bcs.n	3000c536 <__udivmoddi4+0x64>
3000c52c:	459a      	cmp	sl, r3
3000c52e:	d902      	bls.n	3000c536 <__udivmoddi4+0x64>
3000c530:	f1a9 0002 	sub.w	r0, r9, #2
3000c534:	442b      	add	r3, r5
3000c536:	eba3 030a 	sub.w	r3, r3, sl
3000c53a:	b2a4      	uxth	r4, r4
3000c53c:	fbb3 f2f8 	udiv	r2, r3, r8
3000c540:	fb08 3312 	mls	r3, r8, r2, r3
3000c544:	fb02 fc0c 	mul.w	ip, r2, ip
3000c548:	ea44 4403 	orr.w	r4, r4, r3, lsl #16
3000c54c:	45a4      	cmp	ip, r4
3000c54e:	d914      	bls.n	3000c57a <__udivmoddi4+0xa8>
3000c550:	192c      	adds	r4, r5, r4
3000c552:	f102 33ff 	add.w	r3, r2, #4294967295	; 0xffffffff
3000c556:	d203      	bcs.n	3000c560 <__udivmoddi4+0x8e>
3000c558:	45a4      	cmp	ip, r4
3000c55a:	d901      	bls.n	3000c560 <__udivmoddi4+0x8e>
3000c55c:	1e93      	subs	r3, r2, #2
3000c55e:	442c      	add	r4, r5
3000c560:	eba4 040c 	sub.w	r4, r4, ip
3000c564:	ea43 4000 	orr.w	r0, r3, r0, lsl #16
3000c568:	b11e      	cbz	r6, 3000c572 <__udivmoddi4+0xa0>
3000c56a:	40fc      	lsrs	r4, r7
3000c56c:	2300      	movs	r3, #0
3000c56e:	6034      	str	r4, [r6, #0]
3000c570:	6073      	str	r3, [r6, #4]
3000c572:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
3000c576:	4648      	mov	r0, r9
3000c578:	e7dd      	b.n	3000c536 <__udivmoddi4+0x64>
3000c57a:	4613      	mov	r3, r2
3000c57c:	e7f0      	b.n	3000c560 <__udivmoddi4+0x8e>
3000c57e:	b902      	cbnz	r2, 3000c582 <__udivmoddi4+0xb0>
3000c580:	deff      	udf	#255	; 0xff
3000c582:	bb8f      	cbnz	r7, 3000c5e8 <__udivmoddi4+0x116>
3000c584:	eba8 0302 	sub.w	r3, r8, r2
3000c588:	2101      	movs	r1, #1
3000c58a:	ea4f 4e15 	mov.w	lr, r5, lsr #16
3000c58e:	b2aa      	uxth	r2, r5
3000c590:	0c20      	lsrs	r0, r4, #16
3000c592:	fbb3 fcfe 	udiv	ip, r3, lr
3000c596:	fb0e 331c 	mls	r3, lr, ip, r3
3000c59a:	fb0c f802 	mul.w	r8, ip, r2
3000c59e:	ea40 4303 	orr.w	r3, r0, r3, lsl #16
3000c5a2:	4598      	cmp	r8, r3
3000c5a4:	d962      	bls.n	3000c66c <__udivmoddi4+0x19a>
3000c5a6:	18eb      	adds	r3, r5, r3
3000c5a8:	f10c 30ff 	add.w	r0, ip, #4294967295	; 0xffffffff
3000c5ac:	d204      	bcs.n	3000c5b8 <__udivmoddi4+0xe6>
3000c5ae:	4598      	cmp	r8, r3
3000c5b0:	d902      	bls.n	3000c5b8 <__udivmoddi4+0xe6>
3000c5b2:	f1ac 0002 	sub.w	r0, ip, #2
3000c5b6:	442b      	add	r3, r5
3000c5b8:	eba3 0308 	sub.w	r3, r3, r8
3000c5bc:	b2a4      	uxth	r4, r4
3000c5be:	fbb3 fcfe 	udiv	ip, r3, lr
3000c5c2:	fb0e 331c 	mls	r3, lr, ip, r3
3000c5c6:	fb0c f202 	mul.w	r2, ip, r2
3000c5ca:	ea44 4403 	orr.w	r4, r4, r3, lsl #16
3000c5ce:	42a2      	cmp	r2, r4
3000c5d0:	d94e      	bls.n	3000c670 <__udivmoddi4+0x19e>
3000c5d2:	192c      	adds	r4, r5, r4
3000c5d4:	f10c 33ff 	add.w	r3, ip, #4294967295	; 0xffffffff
3000c5d8:	d204      	bcs.n	3000c5e4 <__udivmoddi4+0x112>
3000c5da:	42a2      	cmp	r2, r4
3000c5dc:	d902      	bls.n	3000c5e4 <__udivmoddi4+0x112>
3000c5de:	f1ac 0302 	sub.w	r3, ip, #2
3000c5e2:	442c      	add	r4, r5
3000c5e4:	1aa4      	subs	r4, r4, r2
3000c5e6:	e7bd      	b.n	3000c564 <__udivmoddi4+0x92>
3000c5e8:	f1c7 0c20 	rsb	ip, r7, #32
3000c5ec:	40bd      	lsls	r5, r7
3000c5ee:	fa08 f307 	lsl.w	r3, r8, r7
3000c5f2:	40bc      	lsls	r4, r7
3000c5f4:	fa28 f90c 	lsr.w	r9, r8, ip
3000c5f8:	ea4f 4e15 	mov.w	lr, r5, lsr #16
3000c5fc:	fa20 fc0c 	lsr.w	ip, r0, ip
3000c600:	ea4c 0203 	orr.w	r2, ip, r3
3000c604:	b2ab      	uxth	r3, r5
3000c606:	fbb9 fcfe 	udiv	ip, r9, lr
3000c60a:	0c11      	lsrs	r1, r2, #16
3000c60c:	fb0e 901c 	mls	r0, lr, ip, r9
3000c610:	fb0c f803 	mul.w	r8, ip, r3
3000c614:	ea41 4000 	orr.w	r0, r1, r0, lsl #16
3000c618:	4580      	cmp	r8, r0
3000c61a:	d923      	bls.n	3000c664 <__udivmoddi4+0x192>
3000c61c:	1828      	adds	r0, r5, r0
3000c61e:	f10c 31ff 	add.w	r1, ip, #4294967295	; 0xffffffff
3000c622:	d204      	bcs.n	3000c62e <__udivmoddi4+0x15c>
3000c624:	4580      	cmp	r8, r0
3000c626:	d902      	bls.n	3000c62e <__udivmoddi4+0x15c>
3000c628:	f1ac 0102 	sub.w	r1, ip, #2
3000c62c:	4428      	add	r0, r5
3000c62e:	eba0 0008 	sub.w	r0, r0, r8
3000c632:	b292      	uxth	r2, r2
3000c634:	fbb0 fcfe 	udiv	ip, r0, lr
3000c638:	fb0e 001c 	mls	r0, lr, ip, r0
3000c63c:	fb0c f803 	mul.w	r8, ip, r3
3000c640:	ea42 4300 	orr.w	r3, r2, r0, lsl #16
3000c644:	4598      	cmp	r8, r3
3000c646:	d90f      	bls.n	3000c668 <__udivmoddi4+0x196>
3000c648:	18eb      	adds	r3, r5, r3
3000c64a:	f10c 32ff 	add.w	r2, ip, #4294967295	; 0xffffffff
3000c64e:	d204      	bcs.n	3000c65a <__udivmoddi4+0x188>
3000c650:	4598      	cmp	r8, r3
3000c652:	d902      	bls.n	3000c65a <__udivmoddi4+0x188>
3000c654:	f1ac 0202 	sub.w	r2, ip, #2
3000c658:	442b      	add	r3, r5
3000c65a:	eba3 0308 	sub.w	r3, r3, r8
3000c65e:	ea42 4101 	orr.w	r1, r2, r1, lsl #16
3000c662:	e792      	b.n	3000c58a <__udivmoddi4+0xb8>
3000c664:	4661      	mov	r1, ip
3000c666:	e7e2      	b.n	3000c62e <__udivmoddi4+0x15c>
3000c668:	4662      	mov	r2, ip
3000c66a:	e7f6      	b.n	3000c65a <__udivmoddi4+0x188>
3000c66c:	4660      	mov	r0, ip
3000c66e:	e7a3      	b.n	3000c5b8 <__udivmoddi4+0xe6>
3000c670:	4663      	mov	r3, ip
3000c672:	e7b7      	b.n	3000c5e4 <__udivmoddi4+0x112>
3000c674:	4543      	cmp	r3, r8
3000c676:	d905      	bls.n	3000c684 <__udivmoddi4+0x1b2>
3000c678:	b10e      	cbz	r6, 3000c67e <__udivmoddi4+0x1ac>
3000c67a:	e9c6 0800 	strd	r0, r8, [r6]
3000c67e:	2100      	movs	r1, #0
3000c680:	4608      	mov	r0, r1
3000c682:	e776      	b.n	3000c572 <__udivmoddi4+0xa0>
3000c684:	fab3 fc83 	clz	ip, r3
3000c688:	f1bc 0f00 	cmp.w	ip, #0
3000c68c:	d110      	bne.n	3000c6b0 <__udivmoddi4+0x1de>
3000c68e:	4543      	cmp	r3, r8
3000c690:	d301      	bcc.n	3000c696 <__udivmoddi4+0x1c4>
3000c692:	4282      	cmp	r2, r0
3000c694:	d80a      	bhi.n	3000c6ac <__udivmoddi4+0x1da>
3000c696:	1a84      	subs	r4, r0, r2
3000c698:	eb68 0303 	sbc.w	r3, r8, r3
3000c69c:	2001      	movs	r0, #1
3000c69e:	469e      	mov	lr, r3
3000c6a0:	2e00      	cmp	r6, #0
3000c6a2:	d068      	beq.n	3000c776 <__udivmoddi4+0x2a4>
3000c6a4:	e9c6 4e00 	strd	r4, lr, [r6]
3000c6a8:	2100      	movs	r1, #0
3000c6aa:	e762      	b.n	3000c572 <__udivmoddi4+0xa0>
3000c6ac:	4660      	mov	r0, ip
3000c6ae:	e7f7      	b.n	3000c6a0 <__udivmoddi4+0x1ce>
3000c6b0:	f1cc 0e20 	rsb	lr, ip, #32
3000c6b4:	fa03 f30c 	lsl.w	r3, r3, ip
3000c6b8:	fa02 f50c 	lsl.w	r5, r2, ip
3000c6bc:	fa00 f70c 	lsl.w	r7, r0, ip
3000c6c0:	fa22 f40e 	lsr.w	r4, r2, lr
3000c6c4:	fa28 fb0e 	lsr.w	fp, r8, lr
3000c6c8:	fa20 f10e 	lsr.w	r1, r0, lr
3000c6cc:	431c      	orrs	r4, r3
3000c6ce:	fa08 f30c 	lsl.w	r3, r8, ip
3000c6d2:	ea4f 4914 	mov.w	r9, r4, lsr #16
3000c6d6:	4319      	orrs	r1, r3
3000c6d8:	b2a3      	uxth	r3, r4
3000c6da:	0c0a      	lsrs	r2, r1, #16
3000c6dc:	fbbb f8f9 	udiv	r8, fp, r9
3000c6e0:	fb09 bb18 	mls	fp, r9, r8, fp
3000c6e4:	fb08 fa03 	mul.w	sl, r8, r3
3000c6e8:	ea42 420b 	orr.w	r2, r2, fp, lsl #16
3000c6ec:	4592      	cmp	sl, r2
3000c6ee:	d93e      	bls.n	3000c76e <__udivmoddi4+0x29c>
3000c6f0:	18a2      	adds	r2, r4, r2
3000c6f2:	f108 30ff 	add.w	r0, r8, #4294967295	; 0xffffffff
3000c6f6:	d204      	bcs.n	3000c702 <__udivmoddi4+0x230>
3000c6f8:	4592      	cmp	sl, r2
3000c6fa:	d902      	bls.n	3000c702 <__udivmoddi4+0x230>
3000c6fc:	f1a8 0002 	sub.w	r0, r8, #2
3000c700:	4422      	add	r2, r4
3000c702:	eba2 020a 	sub.w	r2, r2, sl
3000c706:	b289      	uxth	r1, r1
3000c708:	fbb2 f8f9 	udiv	r8, r2, r9
3000c70c:	fb09 2218 	mls	r2, r9, r8, r2
3000c710:	fb08 f303 	mul.w	r3, r8, r3
3000c714:	ea41 4202 	orr.w	r2, r1, r2, lsl #16
3000c718:	4293      	cmp	r3, r2
3000c71a:	d92a      	bls.n	3000c772 <__udivmoddi4+0x2a0>
3000c71c:	18a2      	adds	r2, r4, r2
3000c71e:	f108 31ff 	add.w	r1, r8, #4294967295	; 0xffffffff
3000c722:	d204      	bcs.n	3000c72e <__udivmoddi4+0x25c>
3000c724:	4293      	cmp	r3, r2
3000c726:	d902      	bls.n	3000c72e <__udivmoddi4+0x25c>
3000c728:	f1a8 0102 	sub.w	r1, r8, #2
3000c72c:	4422      	add	r2, r4
3000c72e:	ea41 4000 	orr.w	r0, r1, r0, lsl #16
3000c732:	1ad2      	subs	r2, r2, r3
3000c734:	fba0 9805 	umull	r9, r8, r0, r5
3000c738:	4542      	cmp	r2, r8
3000c73a:	464b      	mov	r3, r9
3000c73c:	4641      	mov	r1, r8
3000c73e:	d302      	bcc.n	3000c746 <__udivmoddi4+0x274>
3000c740:	d106      	bne.n	3000c750 <__udivmoddi4+0x27e>
3000c742:	454f      	cmp	r7, r9
3000c744:	d204      	bcs.n	3000c750 <__udivmoddi4+0x27e>
3000c746:	3801      	subs	r0, #1
3000c748:	ebb9 0305 	subs.w	r3, r9, r5
3000c74c:	eb68 0104 	sbc.w	r1, r8, r4
3000c750:	b18e      	cbz	r6, 3000c776 <__udivmoddi4+0x2a4>
3000c752:	1afc      	subs	r4, r7, r3
3000c754:	eb62 0701 	sbc.w	r7, r2, r1
3000c758:	fa07 fe0e 	lsl.w	lr, r7, lr
3000c75c:	fa24 f30c 	lsr.w	r3, r4, ip
3000c760:	fa27 f70c 	lsr.w	r7, r7, ip
3000c764:	ea4e 0303 	orr.w	r3, lr, r3
3000c768:	e9c6 3700 	strd	r3, r7, [r6]
3000c76c:	e79c      	b.n	3000c6a8 <__udivmoddi4+0x1d6>
3000c76e:	4640      	mov	r0, r8
3000c770:	e7c7      	b.n	3000c702 <__udivmoddi4+0x230>
3000c772:	4641      	mov	r1, r8
3000c774:	e7db      	b.n	3000c72e <__udivmoddi4+0x25c>
3000c776:	4631      	mov	r1, r6
3000c778:	e6fb      	b.n	3000c572 <__udivmoddi4+0xa0>
	...

3000c77c <__aeabi_idiv0>:
3000c77c:	4770      	bx	lr
3000c77e:	bf00      	nop

3000c780 <__SBOOT_Validate_ImgHash_veneer>:
3000c780:	f85f f000 	ldr.w	pc, [pc]	; 3000c784 <__SBOOT_Validate_ImgHash_veneer+0x4>
3000c784:	00003541 	.word	0x00003541

3000c788 <__SBOOT_Validate_Algorithm_veneer>:
3000c788:	f85f f000 	ldr.w	pc, [pc]	; 3000c78c <__SBOOT_Validate_Algorithm_veneer+0x4>
3000c78c:	000033ed 	.word	0x000033ed

3000c790 <____wrap_memset_veneer>:
3000c790:	f85f f000 	ldr.w	pc, [pc]	; 3000c794 <____wrap_memset_veneer+0x4>
3000c794:	00012efd 	.word	0x00012efd

3000c798 <__SBOOT_GetMdType_veneer>:
3000c798:	f85f f000 	ldr.w	pc, [pc]	; 3000c79c <__SBOOT_GetMdType_veneer+0x4>
3000c79c:	0000319d 	.word	0x0000319d

3000c7a0 <__INT_HardFault_C_veneer>:
3000c7a0:	f85f f000 	ldr.w	pc, [pc]	; 3000c7a4 <__INT_HardFault_C_veneer+0x4>
3000c7a4:	0000d8ed 	.word	0x0000d8ed

3000c7a8 <__SBOOT_Validate_PubKey_veneer>:
3000c7a8:	f85f f000 	ldr.w	pc, [pc]	; 3000c7ac <__SBOOT_Validate_PubKey_veneer+0x4>
3000c7ac:	00003495 	.word	0x00003495

3000c7b0 <__SBOOT_Validate_Signature_veneer>:
3000c7b0:	f85f f000 	ldr.w	pc, [pc]	; 3000c7b4 <__SBOOT_Validate_Signature_veneer+0x4>
3000c7b4:	00003431 	.word	0x00003431
3000c7b8:	74696e49 	.word	0x74696e49
3000c7bc:	0d425720 	.word	0x0d425720
3000c7c0:	0000000a 	.word	0x0000000a
3000c7c4:	544f4f42 	.word	0x544f4f42
3000c7c8:	00000000 	.word	0x00000000
3000c7cc:	6e496552 	.word	0x6e496552
3000c7d0:	44207469 	.word	0x44207469
3000c7d4:	0a0d5244 	.word	0x0a0d5244
3000c7d8:	00000000 	.word	0x00000000
3000c7dc:	32474d49 	.word	0x32474d49
3000c7e0:	4f4f4220 	.word	0x4f4f4220
3000c7e4:	72662054 	.word	0x72662054
3000c7e8:	4f206d6f 	.word	0x4f206d6f
3000c7ec:	25204154 	.word	0x25204154
3000c7f0:	00000a64 	.word	0x00000a64
3000c7f4:	20344d4b 	.word	0x20344d4b
3000c7f8:	544f4f42 	.word	0x544f4f42
3000c7fc:	41455220 	.word	0x41455220
3000c800:	204e4f53 	.word	0x204e4f53
3000c804:	203a7825 	.word	0x203a7825
3000c808:	00000000 	.word	0x00000000
3000c80c:	00207325 	.word	0x00207325
3000c810:	00000023 	.word	0x00000023
3000c814:	54454447 	.word	0x54454447
3000c818:	44565f31 	.word	0x44565f31
3000c81c:	505f3831 	.word	0x505f3831
3000c820:	0000534f 	.word	0x0000534f
3000c824:	54454447 	.word	0x54454447
3000c828:	44565f31 	.word	0x44565f31
3000c82c:	4e5f3831 	.word	0x4e5f3831
3000c830:	00004745 	.word	0x00004745
3000c834:	54454447 	.word	0x54454447
3000c838:	3044565f 	.word	0x3044565f
3000c83c:	4f505f39 	.word	0x4f505f39
3000c840:	00000053 	.word	0x00000053
3000c844:	54454447 	.word	0x54454447
3000c848:	3044565f 	.word	0x3044565f
3000c84c:	454e5f39 	.word	0x454e5f39
3000c850:	00000047 	.word	0x00000047
3000c854:	575f5041 	.word	0x575f5041
3000c858:	324d5241 	.word	0x324d5241
3000c85c:	49524550 	.word	0x49524550
3000c860:	00000000 	.word	0x00000000
3000c864:	5f344d4b 	.word	0x5f344d4b
3000c868:	4d524157 	.word	0x4d524157
3000c86c:	52455032 	.word	0x52455032
3000c870:	00000049 	.word	0x00000049
3000c874:	5f304d4b 	.word	0x5f304d4b
3000c878:	4d524157 	.word	0x4d524157
3000c87c:	52455032 	.word	0x52455032
3000c880:	00000049 	.word	0x00000049
3000c884:	34474457 	.word	0x34474457
3000c888:	00000000 	.word	0x00000000
3000c88c:	33474457 	.word	0x33474457
3000c890:	00000000 	.word	0x00000000
3000c894:	32474457 	.word	0x32474457
3000c898:	00000000 	.word	0x00000000
3000c89c:	31474457 	.word	0x31474457
3000c8a0:	00000000 	.word	0x00000000
3000c8a4:	47445749 	.word	0x47445749
3000c8a8:	00000000 	.word	0x00000000
3000c8ac:	59535041 	.word	0x59535041
3000c8b0:	00000053 	.word	0x00000053
3000c8b4:	5953504e 	.word	0x5953504e
3000c8b8:	00000053 	.word	0x00000053
3000c8bc:	5953504c 	.word	0x5953504c
3000c8c0:	00000053 	.word	0x00000053
3000c8c4:	504c5344 	.word	0x504c5344
3000c8c8:	00000000 	.word	0x00000000
3000c8cc:	5f524f42 	.word	0x5f524f42
3000c8d0:	00434341 	.word	0x00434341
3000c8d4:	00524f42 	.word	0x00524f42
3000c8d8:	004d4854 	.word	0x004d4854
3000c8dc:	74696e49 	.word	0x74696e49
3000c8e0:	206c6169 	.word	0x206c6169
3000c8e4:	65776f50 	.word	0x65776f50
3000c8e8:	6e6f2072 	.word	0x6e6f2072
3000c8ec:	0000000a 	.word	0x0000000a
3000c8f0:	61736944 	.word	0x61736944
3000c8f4:	20656c62 	.word	0x20656c62
3000c8f8:	20304d4b 	.word	0x20304d4b
3000c8fc:	746f6f42 	.word	0x746f6f42
3000c900:	6e692020 	.word	0x6e692020
3000c904:	73655420 	.word	0x73655420
3000c908:	6f6d2074 	.word	0x6f6d2074
3000c90c:	0a0d6564 	.word	0x0a0d6564
3000c910:	00000000 	.word	0x00000000
3000c914:	61707942 	.word	0x61707942
3000c918:	4f207373 	.word	0x4f207373
3000c91c:	61205054 	.word	0x61205054
3000c920:	6c6f7475 	.word	0x6c6f7475
3000c924:	0d64616f 	.word	0x0d64616f
3000c928:	0000000a 	.word	0x0000000a
3000c92c:	6d6f682f 	.word	0x6d6f682f
3000c930:	75612f65 	.word	0x75612f65
3000c934:	6a5f6f74 	.word	0x6a5f6f74
3000c938:	696b6e65 	.word	0x696b6e65
3000c93c:	775f736e 	.word	0x775f736e
3000c940:	356e616c 	.word	0x356e616c
3000c944:	726f772f 	.word	0x726f772f
3000c948:	6170736b 	.word	0x6170736b
3000c94c:	732f6563 	.word	0x732f6563
3000c950:	612f6b64 	.word	0x612f6b64
3000c954:	2d6f7475 	.word	0x2d6f7475
3000c958:	656c6572 	.word	0x656c6572
3000c95c:	2d657361 	.word	0x2d657361
3000c960:	7473616d 	.word	0x7473616d
3000c964:	632f7265 	.word	0x632f7265
3000c968:	6f706d6f 	.word	0x6f706d6f
3000c96c:	746e656e 	.word	0x746e656e
3000c970:	636f732f 	.word	0x636f732f
3000c974:	656d612f 	.word	0x656d612f
3000c978:	6d736162 	.word	0x6d736162
3000c97c:	2f747261 	.word	0x2f747261
3000c980:	64616f6c 	.word	0x64616f6c
3000c984:	622f7265 	.word	0x622f7265
3000c988:	6c746f6f 	.word	0x6c746f6f
3000c98c:	6564616f 	.word	0x6564616f
3000c990:	6f622f72 	.word	0x6f622f72
3000c994:	6f6c746f 	.word	0x6f6c746f
3000c998:	72656461 	.word	0x72656461
3000c99c:	2e70685f 	.word	0x2e70685f
3000c9a0:	00000063 	.word	0x00000063
3000c9a4:	4620504e 	.word	0x4620504e
3000c9a8:	20716572 	.word	0x20716572
3000c9ac:	20756c25 	.word	0x20756c25
3000c9b0:	0a7a484d 	.word	0x0a7a484d
3000c9b4:	00000000 	.word	0x00000000
3000c9b8:	46205041 	.word	0x46205041
3000c9bc:	20716572 	.word	0x20716572
3000c9c0:	20756c25 	.word	0x20756c25
3000c9c4:	0a7a484d 	.word	0x0a7a484d
3000c9c8:	00000000 	.word	0x00000000
3000c9cc:	4620504c 	.word	0x4620504c
3000c9d0:	20716572 	.word	0x20716572
3000c9d4:	20756c25 	.word	0x20756c25
3000c9d8:	0a7a484d 	.word	0x0a7a484d
3000c9dc:	00000000 	.word	0x00000000
3000c9e0:	696d656d 	.word	0x696d656d
3000c9e4:	2e6f666e 	.word	0x2e6f666e
3000c9e8:	5f6d656d 	.word	0x5f6d656d
3000c9ec:	65707974 	.word	0x65707974
3000c9f0:	2578303d 	.word	0x2578303d
3000c9f4:	00000a78 	.word	0x00000a78
3000c9f8:	31474d49 	.word	0x31474d49
3000c9fc:	544e4520 	.word	0x544e4520
3000ca00:	4d205245 	.word	0x4d205245
3000ca04:	5b3a5053 	.word	0x5b3a5053
3000ca08:	78383025 	.word	0x78383025
3000ca0c:	00000a5d 	.word	0x00000a5d
3000ca10:	6c697542 	.word	0x6c697542
3000ca14:	69542064 	.word	0x69542064
3000ca18:	203a656d 	.word	0x203a656d
3000ca1c:	25207325 	.word	0x25207325
3000ca20:	00000a73 	.word	0x00000a73
3000ca24:	303a3031 	.word	0x303a3031
3000ca28:	30333a39 	.word	0x30333a39
3000ca2c:	00000000 	.word	0x00000000
3000ca30:	2074634f 	.word	0x2074634f
3000ca34:	32203820 	.word	0x32203820
3000ca38:	00353230 	.word	0x00353230
3000ca3c:	31474d49 	.word	0x31474d49
3000ca40:	43455320 	.word	0x43455320
3000ca44:	20455255 	.word	0x20455255
3000ca48:	54415453 	.word	0x54415453
3000ca4c:	25203a45 	.word	0x25203a45
3000ca50:	00000a64 	.word	0x00000a64
3000ca54:	6d656d28 	.word	0x6d656d28
3000ca58:	6f666e69 	.word	0x6f666e69
3000ca5c:	6d656d2e 	.word	0x6d656d2e
3000ca60:	7079745f 	.word	0x7079745f
3000ca64:	303d3d65 	.word	0x303d3d65
3000ca68:	0a782578 	.word	0x0a782578
3000ca6c:	00000000 	.word	0x00000000
3000ca70:	74696e49 	.word	0x74696e49
3000ca74:	52535020 	.word	0x52535020
3000ca78:	0a0d4d41 	.word	0x0a0d4d41
3000ca7c:	00000000 	.word	0x00000000
3000ca80:	20746573 	.word	0x20746573
3000ca84:	206d6670 	.word	0x206d6670
3000ca88:	6c696166 	.word	0x6c696166
3000ca8c:	00000a0d 	.word	0x00000a0d
3000ca90:	4f525245 	.word	0x4f525245
3000ca94:	20212152 	.word	0x20212152
3000ca98:	756f6853 	.word	0x756f6853
3000ca9c:	4e20646c 	.word	0x4e20646c
3000caa0:	6520746f 	.word	0x6520746f
3000caa4:	6c62616e 	.word	0x6c62616e
3000caa8:	654d2065 	.word	0x654d2065
3000caac:	7277536d 	.word	0x7277536d
3000cab0:	6c6e4f5f 	.word	0x6c6e4f5f
3000cab4:	6e692079 	.word	0x6e692079
3000cab8:	52444420 	.word	0x52444420
3000cabc:	69684320 	.word	0x69684320
3000cac0:	21212170 	.word	0x21212170
3000cac4:	00000a0d 	.word	0x00000a0d
3000cac8:	74696e49 	.word	0x74696e49
3000cacc:	52444420 	.word	0x52444420
3000cad0:	000a0d32 	.word	0x000a0d32
3000cad4:	74696e49 	.word	0x74696e49
3000cad8:	52444420 	.word	0x52444420
3000cadc:	000a0d33 	.word	0x000a0d33
3000cae0:	72617453 	.word	0x72617453
3000cae4:	6f4e2074 	.word	0x6f4e2074
3000cae8:	6365536e 	.word	0x6365536e
3000caec:	20657275 	.word	0x20657275
3000caf0:	78302040 	.word	0x78302040
3000caf4:	2e207825 	.word	0x2e207825
3000caf8:	0a0d2e2e 	.word	0x0a0d2e2e
3000cafc:	00000000 	.word	0x00000000
3000cb00:	00000208 	.word	0x00000208
3000cb04:	0000020c 	.word	0x0000020c
3000cb08:	00000210 	.word	0x00000210
3000cb0c:	00000008 	.word	0x00000008
3000cb10:	00000214 	.word	0x00000214
3000cb14:	00000218 	.word	0x00000218
3000cb18:	0000021c 	.word	0x0000021c
3000cb1c:	0000000c 	.word	0x0000000c
3000cb20:	20344d4b 	.word	0x20344d4b
3000cb24:	33474d49 	.word	0x33474d49
3000cb28:	00000000 	.word	0x00000000
3000cb2c:	20344d4b 	.word	0x20344d4b
3000cb30:	0043534e 	.word	0x0043534e
3000cb34:	31474d49 	.word	0x31474d49
3000cb38:	00000000 	.word	0x00000000
3000cb3c:	32474d49 	.word	0x32474d49
3000cb40:	00000000 	.word	0x00000000
3000cb44:	42205041 	.word	0x42205041
3000cb48:	4626314c 	.word	0x4626314c
3000cb4c:	00005049 	.word	0x00005049
3000cb50:	49207325 	.word	0x49207325
3000cb54:	6c61766e 	.word	0x6c61766e
3000cb58:	000a6469 	.word	0x000a6469
3000cb5c:	255b7325 	.word	0x255b7325
3000cb60:	3a783830 	.word	0x3a783830
3000cb64:	0a5d7825 	.word	0x0a5d7825
3000cb68:	00000000 	.word	0x00000000
3000cb6c:	56207325 	.word	0x56207325
3000cb70:	46495245 	.word	0x46495245
3000cb74:	41502059 	.word	0x41502059
3000cb78:	000a5353 	.word	0x000a5353
3000cb7c:	56207325 	.word	0x56207325
3000cb80:	46495245 	.word	0x46495245
3000cb84:	41462059 	.word	0x41462059
3000cb88:	202c4c49 	.word	0x202c4c49
3000cb8c:	20746572 	.word	0x20746572
3000cb90:	6425203d 	.word	0x6425203d
3000cb94:	0000000a 	.word	0x0000000a
3000cb98:	74726543 	.word	0x74726543
3000cb9c:	63696669 	.word	0x63696669
3000cba0:	20657461 	.word	0x20657461
3000cba4:	49524556 	.word	0x49524556
3000cba8:	46205946 	.word	0x46205946
3000cbac:	2c4c4941 	.word	0x2c4c4941
3000cbb0:	74657220 	.word	0x74657220
3000cbb4:	25203d20 	.word	0x25203d20
3000cbb8:	00000a64 	.word	0x00000a64
3000cbbc:	706d6f43 	.word	0x706d6f43
3000cbc0:	73736572 	.word	0x73736572
3000cbc4:	49206465 	.word	0x49206465
3000cbc8:	5620676d 	.word	0x5620676d
3000cbcc:	46495245 	.word	0x46495245
3000cbd0:	41502059 	.word	0x41502059
3000cbd4:	000a5353 	.word	0x000a5353
3000cbd8:	706d6f43 	.word	0x706d6f43
3000cbdc:	73736572 	.word	0x73736572
3000cbe0:	49206465 	.word	0x49206465
3000cbe4:	5620676d 	.word	0x5620676d
3000cbe8:	46495245 	.word	0x46495245
3000cbec:	41462059 	.word	0x41462059
3000cbf0:	202c4c49 	.word	0x202c4c49
3000cbf4:	20746572 	.word	0x20746572
3000cbf8:	6425203d 	.word	0x6425203d
3000cbfc:	0000000a 	.word	0x0000000a
3000cc00:	3000cb20 	.word	0x3000cb20
3000cc04:	3000cb2c 	.word	0x3000cb2c
3000cc08:	3000cb34 	.word	0x3000cb34
3000cc0c:	3000cb3c 	.word	0x3000cb3c
3000cc10:	3000cb24 	.word	0x3000cb24
3000cc14:	3000cb44 	.word	0x3000cb44
3000cc18:	20304d4b 	.word	0x20304d4b
3000cc1c:	20504958 	.word	0x20504958
3000cc20:	00474d49 	.word	0x00474d49
3000cc24:	20304d4b 	.word	0x20304d4b
3000cc28:	4d415253 	.word	0x4d415253
3000cc2c:	00000000 	.word	0x00000000
3000cc30:	20304d4b 	.word	0x20304d4b
3000cc34:	4d415244 	.word	0x4d415244
3000cc38:	00000000 	.word	0x00000000
3000cc3c:	20344d4b 	.word	0x20344d4b
3000cc40:	20504958 	.word	0x20504958
3000cc44:	00474d49 	.word	0x00474d49
3000cc48:	20344d4b 	.word	0x20344d4b
3000cc4c:	4d415253 	.word	0x4d415253
3000cc50:	00000000 	.word	0x00000000
3000cc54:	20344d4b 	.word	0x20344d4b
3000cc58:	4d415244 	.word	0x4d415244
3000cc5c:	00000000 	.word	0x00000000
3000cc60:	58205041 	.word	0x58205041
3000cc64:	49205049 	.word	0x49205049
3000cc68:	0000474d 	.word	0x0000474d
3000cc6c:	42205041 	.word	0x42205041
3000cc70:	5320314c 	.word	0x5320314c
3000cc74:	004d4152 	.word	0x004d4152
3000cc78:	42205041 	.word	0x42205041
3000cc7c:	4420314c 	.word	0x4420314c
3000cc80:	004d4152 	.word	0x004d4152
3000cc84:	46205041 	.word	0x46205041
3000cc88:	00005049 	.word	0x00005049
3000cc8c:	32474d49 	.word	0x32474d49
3000cc90:	46544f20 	.word	0x46544f20
3000cc94:	0a4e4520 	.word	0x0a4e4520
3000cc98:	00000000 	.word	0x00000000
3000cc9c:	2046544f 	.word	0x2046544f
3000cca0:	65646f4d 	.word	0x65646f4d
3000cca4:	72726520 	.word	0x72726520
3000cca8:	0a0d726f 	.word	0x0a0d726f
3000ccac:	00000000 	.word	0x00000000
3000ccb0:	6c696146 	.word	0x6c696146
3000ccb4:	206f7420 	.word	0x206f7420
3000ccb8:	64616f6c 	.word	0x64616f6c
3000ccbc:	50445220 	.word	0x50445220
3000ccc0:	616d6920 	.word	0x616d6920
3000ccc4:	0a216567 	.word	0x0a216567
3000ccc8:	00000000 	.word	0x00000000
3000cccc:	6c696146 	.word	0x6c696146
3000ccd0:	206f7420 	.word	0x206f7420
3000ccd4:	64616f6c 	.word	0x64616f6c
3000ccd8:	20504120 	.word	0x20504120
3000ccdc:	67616d69 	.word	0x67616d69
3000cce0:	000a2165 	.word	0x000a2165
3000cce4:	20504452 	.word	0x20504452
3000cce8:	000a4e45 	.word	0x000a4e45
3000ccec:	32474d49 	.word	0x32474d49
3000ccf0:	41544f28 	.word	0x41544f28
3000ccf4:	20296425 	.word	0x20296425
3000ccf8:	202c7325 	.word	0x202c7325
3000ccfc:	3a746572 	.word	0x3a746572
3000cd00:	0a642520 	.word	0x0a642520
3000cd04:	00000000 	.word	0x00000000
3000cd08:	494c4156 	.word	0x494c4156
3000cd0c:	00000044 	.word	0x00000044
3000cd10:	41564e49 	.word	0x41564e49
3000cd14:	0044494c 	.word	0x0044494c
3000cd18:	32474d49 	.word	0x32474d49
3000cd1c:	4f4f4220 	.word	0x4f4f4220
3000cd20:	72662054 	.word	0x72662054
3000cd24:	4f206d6f 	.word	0x4f206d6f
3000cd28:	25204154 	.word	0x25204154
3000cd2c:	56202c64 	.word	0x56202c64
3000cd30:	69737265 	.word	0x69737265
3000cd34:	203a6e6f 	.word	0x203a6e6f
3000cd38:	2e646c25 	.word	0x2e646c25
3000cd3c:	20646c25 	.word	0x20646c25
3000cd40:	0000000a 	.word	0x0000000a
3000cd44:	2041544f 	.word	0x2041544f
3000cd48:	74726543 	.word	0x74726543
3000cd4c:	63696669 	.word	0x63696669
3000cd50:	20657461 	.word	0x20657461
3000cd54:	4d492026 	.word	0x4d492026
3000cd58:	69203247 	.word	0x69203247
3000cd5c:	6c61766e 	.word	0x6c61766e
3000cd60:	202c6469 	.word	0x202c6469
3000cd64:	544f4f42 	.word	0x544f4f42
3000cd68:	49414620 	.word	0x49414620
3000cd6c:	0a21214c 	.word	0x0a21214c
3000cd70:	00000000 	.word	0x00000000
3000cd74:	42205041 	.word	0x42205041
3000cd78:	20544f4f 	.word	0x20544f4f
3000cd7c:	6d6f7266 	.word	0x6d6f7266
3000cd80:	41544f20 	.word	0x41544f20
3000cd84:	0a642520 	.word	0x0a642520
3000cd88:	00000000 	.word	0x00000000
3000cd8c:	72747845 	.word	0x72747845
3000cd90:	20746361 	.word	0x20746361
3000cd94:	6d6f7266 	.word	0x6d6f7266
3000cd98:	25783020 	.word	0x25783020
3000cd9c:	6f742078 	.word	0x6f742078
3000cda0:	65764f20 	.word	0x65764f20
3000cda4:	64697272 	.word	0x64697272
3000cda8:	78302065 	.word	0x78302065
3000cdac:	202c7825 	.word	0x202c7825
3000cdb0:	706d6f43 	.word	0x706d6f43
3000cdb4:	73736572 	.word	0x73736572
3000cdb8:	6e654c20 	.word	0x6e654c20
3000cdbc:	20736920 	.word	0x20736920
3000cdc0:	78257830 	.word	0x78257830
3000cdc4:	0000000a 	.word	0x0000000a
3000cdc8:	6d6f682f 	.word	0x6d6f682f
3000cdcc:	75612f65 	.word	0x75612f65
3000cdd0:	6a5f6f74 	.word	0x6a5f6f74
3000cdd4:	696b6e65 	.word	0x696b6e65
3000cdd8:	775f736e 	.word	0x775f736e
3000cddc:	356e616c 	.word	0x356e616c
3000cde0:	726f772f 	.word	0x726f772f
3000cde4:	6170736b 	.word	0x6170736b
3000cde8:	732f6563 	.word	0x732f6563
3000cdec:	612f6b64 	.word	0x612f6b64
3000cdf0:	2d6f7475 	.word	0x2d6f7475
3000cdf4:	656c6572 	.word	0x656c6572
3000cdf8:	2d657361 	.word	0x2d657361
3000cdfc:	7473616d 	.word	0x7473616d
3000ce00:	632f7265 	.word	0x632f7265
3000ce04:	6f706d6f 	.word	0x6f706d6f
3000ce08:	746e656e 	.word	0x746e656e
3000ce0c:	636f732f 	.word	0x636f732f
3000ce10:	656d612f 	.word	0x656d612f
3000ce14:	6d736162 	.word	0x6d736162
3000ce18:	2f747261 	.word	0x2f747261
3000ce1c:	64616f6c 	.word	0x64616f6c
3000ce20:	622f7265 	.word	0x622f7265
3000ce24:	6c746f6f 	.word	0x6c746f6f
3000ce28:	6564616f 	.word	0x6564616f
3000ce2c:	6f622f72 	.word	0x6f622f72
3000ce30:	6f5f746f 	.word	0x6f5f746f
3000ce34:	685f6174 	.word	0x685f6174
3000ce38:	00632e70 	.word	0x00632e70
3000ce3c:	3000cc60 	.word	0x3000cc60
3000ce40:	3000cc6c 	.word	0x3000cc6c
3000ce44:	3000cc78 	.word	0x3000cc78
3000ce48:	3000cc84 	.word	0x3000cc84
3000ce4c:	3000cc18 	.word	0x3000cc18
3000ce50:	3000cc24 	.word	0x3000cc24
3000ce54:	3000cc30 	.word	0x3000cc30
3000ce58:	3000cc3c 	.word	0x3000cc3c
3000ce5c:	3000cc48 	.word	0x3000cc48
3000ce60:	3000cc54 	.word	0x3000cc54
3000ce64:	3000cc6c 	.word	0x3000cc6c
3000ce68:	3000cc78 	.word	0x3000cc78
3000ce6c:	3000cc84 	.word	0x3000cc84

3000ce70 <ImagePattern>:
3000ce70:	35393138 31313738 6d6f682f 75612f65     81958711/home/au
3000ce80:	6a5f6f74 696b6e65 775f736e 356e616c     to_jenkins_wlan5
3000ce90:	726f772f 6170736b 732f6563 612f6b64     /workspace/sdk/a
3000cea0:	2d6f7475 656c6572 2d657361 7473616d     uto-release-mast
3000ceb0:	632f7265 6f706d6f 746e656e 636f732f     er/component/soc
3000cec0:	656d612f 6d736162 2f747261 696c7766     /amebasmart/fwli
3000ced0:	61722f62 6f635f6d 6e6f6d6d 656d612f     b/ram_common/ame
3000cee0:	695f6162 615f6370 632e6970 00000000     ba_ipc_api.c....
3000cef0:	2050544f 64616572 74656720 20776820     OTP read get hw 
3000cf00:	616d6573 69616620 00000a6c 0050544f     sema fail...OTP.
3000cf10:	5f50544f 64616552 61462038 25206c69     OTP_Read8 Fail %
3000cf20:	000a2078 6d6f682f 75612f65 6a5f6f74     x ../home/auto_j
3000cf30:	696b6e65 775f736e 356e616c 726f772f     enkins_wlan5/wor
3000cf40:	6170736b 732f6563 612f6b64 2d6f7475     kspace/sdk/auto-
3000cf50:	656c6572 2d657361 7473616d 632f7265     release-master/c
3000cf60:	6f706d6f 746e656e 636f732f 656d612f     omponent/soc/ame
3000cf70:	6d736162 2f747261 696c7766 61722f62     basmart/fwlib/ra
3000cf80:	6f635f6d 6e6f6d6d 656d612f 705f6162     m_common/ameba_p
3000cf90:	632e6c6c 00000000 20524444 657a6953     ll.c....DDR Size
3000cfa0:	20736920 20746f6e 666e6f63 72756769      is not configur
3000cfb0:	000a6465 00524444 415b203e 20524444     ed..DDR.> [ADDR 
3000cfc0:	414d4552 52452050 5d524f52 6f63203a     REMAP ERROR]: co
3000cfd0:	725f756c 70616d65 7469625f 3d212073     lu_remap_bits !=
3000cfe0:	61726420 6f635f6d 625f756c 21737469      dram_colu_bits!
3000cff0:	000a2121 415b203e 20524444 414d4552     !!..> [ADDR REMA
3000d000:	52452050 5d524f52 6162203a 725f6b6e     P ERROR]: bank_r
3000d010:	70616d65 7469625f 3d212073 61726420     emap_bits != dra
3000d020:	61625f6d 625f6b6e 21737469 000a2121     m_bank_bits!!!..
3000d030:	6d6f682f 75612f65 6a5f6f74 696b6e65     /home/auto_jenki
3000d040:	775f736e 356e616c 726f772f 6170736b     ns_wlan5/workspa
3000d050:	732f6563 612f6b64 2d6f7475 656c6572     ce/sdk/auto-rele
3000d060:	2d657361 7473616d 632f7265 6f706d6f     ase-master/compo
3000d070:	746e656e 636f732f 656d612f 6d736162     nent/soc/amebasm
3000d080:	2f747261 696c7766 61722f62 6f635f6d     art/fwlib/ram_co
3000d090:	6e6f6d6d 656d612f 725f6162 31336978     mmon/ameba_rxi31
3000d0a0:	72645f36 695f6d61 2e74696e 00000063     6_dram_init.c...
3000d0b0:	4d415244 20736920 20746f6e 696c6143     DRAM is not Cali
3000d0c0:	69617262 0a0d6e6f 00000000 50524444     braion......DDRP
3000d0d0:	00005948                                HY..

3000d0d4 <ddrphy_tx_scan>:
3000d0d4:	00000000 00080000 00080808 00000000     ................
3000d0e4:	20000000 33333333 000c0022 00000000     ... 3333".......
3000d0f4:	0e0e001a 00170e0e 12171717 00000012     ................
3000d104:	20000fc1 22222222 000c0012 34000000     ... """".......4
3000d114:	0f0f0016 00180f0f 0e181818 0000000e     ................
3000d124:	200003c1 22222222 000c0012 34000000     ... """".......4
3000d134:	0f0f001b 00180f0f 13181818 00000013     ................
3000d144:	20000fc1 22222222 000c0012 34000000     ... """".......4
3000d154:	1110001b 00191111 13191919 00000013     ................
3000d164:	20000ffd 22222222 000c0012 34000000     ... """".......4
3000d174:	1313001c 001c1313 121c1c1c 00000012     ................
3000d184:	20000ffd 22222222 000c0012 34000000     ... """".......4

3000d194 <ddrphy_zq_rx_scan>:
	...
3000d19c:	26262626 26262626 0303030f 0e141414     &&&&&&&&........
3000d1ac:	140e0e0e 26243c34 261c3029 0303030f     ....4<$&)0.&....
3000d1bc:	090b0a0a 0a0a0909 2615231e 2613201b     .........#.&. .&
3000d1cc:	04020204 0b0f0c0c 0c0b0b0b 26233a30     ............0:#&
3000d1dc:	26182722 04020205 0e141414 140e0e0e     "'.&............
3000d1ec:	26243c34 261b2f28 0303030f 0a0c0c0c     4<$&(/.&........
3000d1fc:	0c0a0a0a 2616251f 2614221d 04020205     .....%.&.".&....
3000d20c:	6d6f682f 75612f65 6a5f6f74 696b6e65     /home/auto_jenki
3000d21c:	775f736e 356e616c 726f772f 6170736b     ns_wlan5/workspa
3000d22c:	732f6563 612f6b64 2d6f7475 656c6572     ce/sdk/auto-rele
3000d23c:	2d657361 7473616d 632f7265 6f706d6f     ase-master/compo
3000d24c:	746e656e 636f732f 656d612f 6d736162     nent/soc/amebasm
3000d25c:	2f747261 696c7766 61722f62 6f635f6d     art/fwlib/ram_co
3000d26c:	6e6f6d6d 656d612f 735f6162 632e7277     mmon/ameba_swr.c
3000d27c:	00000000 6e6f4e0a 6365532d 20657275     .....Non-Secure 
3000d28c:	6c726f77 61632064 746f6e6e 6d756420     world cannot dum
3000d29c:	65722070 74736967 20737265 756f6261     p registers abou
3000d2ac:	78652074 74706563 206e6f69 6d6f7266     t exception from
3000d2bc:	63657320 20657275 6c726f77 000a2164      secure world!..
3000d2cc:	4c554146 00000054 52534653 203d2020     FAULT...SFSR  = 
3000d2dc:	30257830 0d786c38 0000000a 52414653     0x%08lx.....SFAR
3000d2ec:	203d2020 30257830 0d786c38 0000000a       = 0x%08lx.....
3000d2fc:	544e4f43 5f4c4f52 3d20534e 25783020     CONTROL_NS = 0x%
3000d30c:	586c3830 00000a0d 53434853 203d2052     08lX....SHCSR = 
3000d31c:	30257830 0d786c38 0000000a 43524941     0x%08lx.....AIRC
3000d32c:	203d2052 30257830 0d786c38 0000000a     R = 0x%08lx.....
3000d33c:	544e4f43 204c4f52 7830203d 6c383025     CONTROL = 0x%08l
3000d34c:	000a0d78 41525350 7443204d 43206c72     x...PSRAM Ctrl C
3000d35c:	203a4b4c 20756c25 0a207a48 00000000     LK: %lu Hz .....
3000d36c:	41525350 0000004d 4e6c6143 206e696d     PSRAM...CalNmin 
3000d37c:	7825203d 6c614320 78616d4e 25203d20     = %x CalNmax = %
3000d38c:	69572078 776f646e 657a6953 25203d20     x WindowSize = %
3000d39c:	68702078 3a657361 20782520 0000000a     x phase: %x ....
3000d3ac:	004f4931 00004f32 004f4932 00004f34     1IO.2O..2IO.4O..
3000d3bc:	004f4934 72617473 64695f74 64253a78     4IO.start_idx:%d
3000d3cc:	6e65202c 64695f64 64253a78 696d202c     , end_idx:%d, mi
3000d3dc:	656c6464 7864695f 2c64253a 6e696620     ddle_idx:%d, fin
3000d3ec:	705f6c61 65736168 7864695f 2c64253a     al_phase_idx:%d,
3000d3fc:	6e696620 735f6c61 74666968 7864695f      final_shift_idx
3000d40c:	2064253a 0000000a 53414c46 41432048     :%d ....FLASH CA
3000d41c:	5b42494c 78257830 5d4b4f20 0000000a     LIB[0x%x OK]....
3000d42c:	53414c46 41432048 5b42494c 78257830     FLASH CALIB[0x%x
3000d43c:	49414620 75202c4c 58206573 5d4c4154      FAIL, use XTAL]
3000d44c:	0000000a 73616c46 65522068 25206461     ....Flash Read %
3000d45c:	00000a73 73616c46 65522068 25206461     s...Flash Read %
3000d46c:	46202c73 0a4c4941 00000000 73616c46     s, FAIL.....Flas
3000d47c:	77532068 68637469 61655220 6f4d2064     h Switch Read Mo
3000d48c:	46206564 0a4c4941 00000000 43495053     de FAIL.....SPIC
3000d49c:	4b4c4320 6425203a 0a7a4820 00000000      CLK: %d Hz.....
3000d4ac:	73616c46 44492068 7825203a 2d78252d     Flash ID: %x-%x-
3000d4bc:	28207825 61706143 79746963 6425203a     %x (Capacity: %d
3000d4cc:	69622d4d 000a2974 73696854 616c6620     M-bit)..This fla
3000d4dc:	74206873 20657079 6e207369 7320746f     sh type is not s
3000d4ec:	6f707075 64657472 00000a21 6d6f682f     upported!.../hom
3000d4fc:	75612f65 6a5f6f74 696b6e65 775f736e     e/auto_jenkins_w
3000d50c:	356e616c 726f772f 6170736b 732f6563     lan5/workspace/s
3000d51c:	612f6b64 2d6f7475 656c6572 2d657361     dk/auto-release-
3000d52c:	7473616d 632f7265 6f706d6f 746e656e     master/component
3000d53c:	636f732f 656d612f 6d736162 2f747261     /soc/amebasmart/
3000d54c:	696c7766 61722f62 70685f6d 656d612f     fwlib/ram_hp/ame
3000d55c:	665f6162 6873616c 2e6b6c63 00000063     ba_flashclk.c...
3000d56c:	73616c46 74732068 73757461 67657220     Flash status reg
3000d57c:	65747369 68632072 65676e61 78303a64     ister changed:0x
3000d58c:	2d207825 7830203e 000a7825 444e414e     %x -> 0x%x..NAND
3000d59c:	3a444920 2d782520 000a7825 73616c46      ID: %x-%x..Flas
3000d5ac:	6c612068 6c62206c 206b636f 6f6c6e75     h all block unlo
3000d5bc:	64656b63 6546202e 72757461 65722065     cked. Feature re
3000d5cc:	78302067 78323025 74657320 206f7420     g 0x%02x set to 
3000d5dc:	30257830 000a7832 3000d3ac 3000d3b0     0x%02x.....0...0
3000d5ec:	3000d3b4 3000d3b8 3000d3bc 206b6c42     ...0...0...0Blk 
3000d5fc:	6c257830 73692078 21424220 0000000a     0x%lx is BB!....
3000d60c:	444e414e 53414c46 00000048 646e614e     NANDFLASH...Nand
3000d61c:	64644120 78302072 72207825 6f696765      Addr 0x%x regio
3000d62c:	6142206e 6c422064 206b636f 206f6f74     n Bad Block too 
3000d63c:	6863756d 0000000a 64616552 67615020     much....Read Pag
3000d64c:	78302065 20786c25 4220666f 6b636f6c     e 0x%lx of Block
3000d65c:	25783020 614d2078 61462079 202c6c69      0x%x May Fail, 
3000d66c:	74617473 30207375 21782578 0000000a     status 0x%x!....
3000d67c:	2d73255b 205d6325 00000000              [%s-%c] ....

3000d688 <mpc1_config>:
3000d688:	00000000 7017ffff 00000000 70500000     .......p......Pp
3000d698:	607fffff 00000000 ffffffff ffffffff     ...`............
3000d6a8:	00000000 ffffffff ffffffff 00000000     ................
3000d6b8:	ffffffff ffffffff 00000000 ffffffff     ................
3000d6c8:	ffffffff 00000000 ffffffff ffffffff     ................
3000d6d8:	00000000 ffffffff ffffffff 00000000     ................

3000d6e8 <mpc2_config>:
3000d6e8:	00014000 0001efff 00000000 00020000     .@..............
3000d6f8:	0003ffff 00000000 ffffffff ffffffff     ................
3000d708:	00000000 ffffffff ffffffff 00000000     ................
3000d718:	ffffffff ffffffff 00000000 ffffffff     ................
3000d728:	ffffffff 00000000 ffffffff ffffffff     ................
3000d738:	00000000 ffffffff ffffffff 00000000     ................

3000d748 <sau_config>:
3000d748:	0001e000 00053fff 00000000 08000000     .....?..........
3000d758:	09ffffff 00000000 0c000000 ffffffff     ................
3000d768:	00000000 ffffffff ffffffff 00000000     ................
3000d778:	ffffffff ffffffff 00000000 ffffffff     ................
3000d788:	ffffffff 00000000 ffffffff ffffffff     ................
3000d798:	00000000 ffffffff ffffffff 00000000     ................
3000d7a8:	73616c66 65675f68 616c5f74 74756f79     flash_get_layout
3000d7b8:	666e695f 7962206f 70797420 64252065     _info by type %d
3000d7c8:	69614620 00000a6c 73616c66 65675f68      Fail...flash_ge
3000d7d8:	616c5f74 74756f79 666e695f 7962206f     t_layout_info by
3000d7e8:	64646120 78302072 46207825 0a6c6961      addr 0x%x Fail.
3000d7f8:	00000000                                ....

3000d7fc <Flash_AVL>:
3000d7fc:	000000ef 000000ff 00000000 000043fc     .............C..
3000d80c:	00000000 000000a1 000000ff 00000000     ................
3000d81c:	0000fffc 00000000 0000000b 000000ff     ................
3000d82c:	00000000 000043fc 00000000 0000000e     .....C..........
3000d83c:	000000ff 00000000 000043fc 00000000     .........C......
3000d84c:	000000c8 000000ff 00000001 000043fc     .............C..
3000d85c:	00000000 000028c2 0000ffff 00000005     .....(..........
3000d86c:	000200fc 00000000 000000c2 000000ff     ................
3000d87c:	00000002 000000fc 00000000 00000068     ............h...
3000d88c:	000000ff 00000002 000000fc 00000000     ................
3000d89c:	00000051 000000ff 00000002 000000fc     Q...............
3000d8ac:	00000000 0000001c 000000ff 00000003     ................
3000d8bc:	000000fc 00000000 00000020 000000ff     ........ .......
3000d8cc:	00000004 000000fc 00000000 00000085     ................
3000d8dc:	000000ff 00000000 000043fc 00000000     .........C......
3000d8ec:	00000000 000000ff 000000fe ffffffff     ................
3000d8fc:	3000a3b9 000000ff ffffffff 000000ff     ...0............
3000d90c:	ffffffff 00000000                       ........

3000d914 <Flash_ReadMode>:
3000d914:	                                         ..

3000d916 <Flash_Speed>:
3000d916:	                                         ..

3000d918 <NAND_AVL>:
3000d918:	000000ef 000000ff 00000000 ffffffff     ................
3000d928:	00000000 000000c8 000000ff 00000001     ................
3000d938:	ffffffff 00000000 000000e5 000000ff     ................
3000d948:	00000001 ffffffff 00000000 000000c2     ................
3000d958:	000000ff 00000002 ffffffff 00000000     ................
3000d968:	0000002c 000000ff 00000004 ffffffff     ,...............
	...
3000d980:	000000ff 000000fe ffffffff 3000a419     ...............0
3000d990:	000000ff ffffffff 000000ff ffffffff     ................
3000d9a0:	00000000 72617453 74732074 206b6361     ....Start stack 
3000d9b0:	6b636162 63617274 20676e69 20726f66     backtracing for 
3000d9c0:	30207073 38302578 70202c78 78302063     sp 0x%08x, pc 0x
3000d9d0:	78383025 726c202c 25783020 0a783830     %08x, lr 0x%08x.
3000d9e0:	00000000 6d6f682f 75612f65 6a5f6f74     ..../home/auto_j
3000d9f0:	696b6e65 775f736e 356e616c 726f772f     enkins_wlan5/wor
3000da00:	6170736b 732f6563 612f6b64 2d6f7475     kspace/sdk/auto-
3000da10:	656c6572 2d657361 7473616d 612f7265     release-master/a
3000da20:	6162656d 72616d73 63675f74 72705f63     mebasmart_gcc_pr
3000da30:	63656a6f 72702f74 63656a6f 70685f74     oject/project_hp
3000da40:	6473612f 6d692f6b 5f656761 0000706d     /asdk/image_mp..
3000da50:	74706f2f 6b74722f 6f6f742d 6168636c     /opt/rtk-toolcha
3000da60:	612f6e69 2d6b6473 332e3031 342d312e     in/asdk-10.3.1-4
3000da70:	2f343533 756e696c 656e2f78 62696c77     354/linux/newlib
3000da80:	00000000 622f7325 612f6e69 6e2d6d72     ....%s/bin/arm-n
3000da90:	2d656e6f 69626165 6464612d 696c3272     one-eabi-addr2li
3000daa0:	2d20656e 73252065 7261742f 5f746567     ne -e %s/target_
3000dab0:	32676d69 6678612e 66612d20 20436970     img2.axf -afpiC 
3000dac0:	30257830 00007838 25783020 00783830     0x%08x.. 0x%08x.
3000dad0:	00344d4b 00304d4b 3d3d3d3d 3d3d3d3d     KM4.KM0.========
3000dae0:	73253d3d 61745320 44206b63 20706d75     ==%s Stack Dump 
3000daf0:	3d3d3d3d 3d3d3d3d 000a3d3d 72727543     ==========..Curr
3000db00:	20746e65 63617453 6f50206b 65746e69     ent Stack Pointe
3000db10:	203d2072 30257830 202c7838 20646e61     r = 0x%08x, and 
3000db20:	706d7564 61747320 64206b63 68747065     dump stack depth
3000db30:	25203d20 000a0d75 2578305b 5d783830      = %u...[0x%08x]
3000db40:	00000020 78383025 00000020 305b0a0d      ...%08x .....[0
3000db50:	38302578 00205d78 00003052 00003452     x%08x] .R0..R4..
3000db60:	3d3d3d0a 3d3d3d3d 203d3d3d 73617243     .========== Cras
3000db70:	75442068 3d20706d 3d3d3d3d 3d3d3d3d     h Dump =========
3000db80:	00000a3d 3d3d3d3d 3d3d3d3d 52203d3d     =...========== R
3000db90:	73696765 20726574 706d7544 3d3d3d20     egister Dump ===
3000dba0:	3d3d3d3d 0a3d3d3d 00000000 4c20205b     =======.....[  L
3000dbb0:	30205d52 38302578 00000a78 5020205b     R] 0x%08x...[  P
3000dbc0:	30205d43 38302578 00000a78 5350785b     C] 0x%08x...[xPS
3000dbd0:	30205d52 38302578 00000a78 4358455b     R] 0x%08x...[EXC
3000dbe0:	30205d52 38302578 00000a78 7325205b     R] 0x%08x...[ %s
3000dbf0:	7830205d 78383025 0000000a 00323152     ] 0x%08x....R12.
3000dc00:	3d3d3d3d 3d3d3d3d 53203d3d 6b636174     ========== Stack
3000dc10:	61725420 3d206563 3d3d3d3d 3d3d3d3d      Trace =========
3000dc20:	00000a3d 3d3d3d3d 3d3d3d3d 45203d3d     =...========== E
3000dc30:	6f20646e 74532066 206b6361 63617254     nd of Stack Trac
3000dc40:	3d3d2065 3d3d3d3d 3d3d3d3d 0000000a     e ==========....
3000dc50:	3d3d3d3d 3d3d3d3d 45203d3d 6f20646e     ========== End o
3000dc60:	72432066 20687361 706d7544 3d3d3d20     f Crash Dump ===
3000dc70:	3d3d3d3d 0a3d3d3d 00000000 0a0d0a0d     =======.........
3000dc80:	00000000 00003552 00003652 00003752     ....R5..R6..R7..
3000dc90:	00003852 00003952 00303152 00313152     R8..R9..R10.R11.
3000dca0:	00003152 00003252 00003352              R1..R2..R3..

3000dcac <register_string>:
3000dcac:	00000000 3000db5c 3000dc84 3000dc88     ....\..0...0...0
3000dcbc:	3000dc8c 3000dc90 3000dc94 3000dc98     ...0...0...0...0
3000dccc:	3000dc9c 3000db58 3000dca0 3000dca4     ...0X..0...0...0
3000dcdc:	3000dca8 3000dbfc 65637845 6f697470     ...0...0Exceptio
3000dcec:	6163206e 74686775 206e6f20 78383025     n caught on %08x
3000dcfc:	0000000a 64726148 75616620 6920746c     ....Hard fault i
3000dd0c:	61632073 64657375 20796220 6c696166     s caused by fail
3000dd1c:	76206465 6f746365 65662072 00686374     ed vector fetch.
3000dd2c:	52534648 305b203a 38302578 2d205d78     HFSR: [0x%08x] -
3000dd3c:	7325203e 0000000a 75636553 79746972     > %s....Security
3000dd4c:	75616620 6920746c 61632073 64657375      fault is caused
3000dd5c:	20796220 61766e49 2064696c 72746e65      by Invalid entr
3000dd6c:	6f702079 00746e69 52534653 305b203a     y point.SFSR: [0
3000dd7c:	38302578 2d205d78 7325203e 0000000a     x%08x] -> %s....
3000dd8c:	75636553 79746972 75616620 6920746c     Security fault i
3000dd9c:	61632073 64657375 20796220 61766e49     s caused by Inva
3000ddac:	2064696c 65746e69 74697267 69732079     lid integrity si
3000ddbc:	74616e67 00657275 75636553 79746972     gnature.Security
3000ddcc:	75616620 6920746c 61632073 64657375      fault is caused
3000dddc:	20796220 61766e49 2064696c 65637865      by Invalid exce
3000ddec:	6f697470 6572206e 6e727574 00000000     ption return....
3000ddfc:	75636553 79746972 75616620 6920746c     Security fault i
3000de0c:	61632073 64657375 20796220 72747441     s caused by Attr
3000de1c:	74756269 206e6f69 74696e75 6f697620     ibution unit vio
3000de2c:	6974616c 00006e6f 75636553 79746972     lation..Security
3000de3c:	75616620 6920746c 61632073 64657375      fault is caused
3000de4c:	20796220 61766e49 2064696c 6e617274      by Invalid tran
3000de5c:	69746973 00006e6f 75636553 79746972     sition..Security
3000de6c:	75616620 6920746c 61632073 64657375      fault is caused
3000de7c:	20796220 797a614c 61747320 70206574      by Lazy state p
3000de8c:	65736572 74617672 206e6f69 6f727265     reservation erro
3000de9c:	00000072 75636553 79746972 75616620     r...Security fau
3000deac:	6920746c 61632073 64657375 20796220     lt is caused by 
3000debc:	797a614c 61747320 65206574 726f7272     Lazy state error
3000decc:	00000000 20656854 75636573 79746972     ....The security
3000dedc:	75616620 6f20746c 72756363 20646572      fault occurred 
3000deec:	72646461 20737365 00007369 52414653     address is..SFAR
3000defc:	305b203a 38302578 2d205d78 7325203e     : [0x%08x] -> %s
3000df0c:	0000000a 6f6d654d 6d207972 67616e61     ....Memory manag
3000df1c:	6e656d65 61662074 20746c75 63207369     ement fault is c
3000df2c:	65737561 79622064 736e6920 63757274     aused by instruc
3000df3c:	6e6f6974 63636120 20737365 6c6f6976     tion access viol
3000df4c:	6f697461 0000006e 53464d4d 5b203a52     ation...MMFSR: [
3000df5c:	30257830 205d7838 25203e2d 00000a73     0x%08x] -> %s...
3000df6c:	6f6d654d 6d207972 67616e61 6e656d65     Memory managemen
3000df7c:	61662074 20746c75 63207369 65737561     t fault is cause
3000df8c:	79622064 74616420 63612061 73736563     d by data access
3000df9c:	6f697620 6974616c 00006e6f 6f6d654d      violation..Memo
3000dfac:	6d207972 67616e61 6e656d65 61662074     ry management fa
3000dfbc:	20746c75 63207369 65737561 79622064     ult is caused by
3000dfcc:	736e7520 6b636174 20676e69 6f727265      unstacking erro
3000dfdc:	00000072 6f6d654d 6d207972 67616e61     r...Memory manag
3000dfec:	6e656d65 61662074 20746c75 63207369     ement fault is c
3000dffc:	65737561 79622064 61747320 6e696b63     aused by stackin
3000e00c:	72652067 00726f72 6f6d654d 6d207972     g error.Memory m
3000e01c:	67616e61 6e656d65 61662074 20746c75     anagement fault 
3000e02c:	63207369 65737561 79622064 6f6c6620     is caused by flo
3000e03c:	6e697461 6f702d67 20746e69 797a616c     ating-point lazy
3000e04c:	61747320 70206574 65736572 74617672      state preservat
3000e05c:	006e6f69 20656854 6f6d656d 6d207972     ion.The memory m
3000e06c:	67616e61 6e656d65 61662074 20746c75     anagement fault 
3000e07c:	7563636f 64657272 64646120 73736572     occurred address
3000e08c:	00736920 52414d4d 305b203a 38302578      is.MMAR: [0x%08
3000e09c:	2d205d78 7325203e 0000000a 20737542     x] -> %s....Bus 
3000e0ac:	6c756166 73692074 75616320 20646573     fault is caused 
3000e0bc:	69207962 7274736e 69746375 61206e6f     by instruction a
3000e0cc:	73656363 69762073 74616c6f 006e6f69     ccess violation.
3000e0dc:	52534642 305b203a 38302578 2d205d78     BFSR: [0x%08x] -
3000e0ec:	7325203e 0000000a 20737542 6c756166     > %s....Bus faul
3000e0fc:	73692074 75616320 20646573 70207962     t is caused by p
3000e10c:	69636572 64206573 20617461 65636361     recise data acce
3000e11c:	76207373 616c6f69 6e6f6974 00000000     ss violation....
3000e12c:	20737542 6c756166 73692074 75616320     Bus fault is cau
3000e13c:	20646573 69207962 6572706d 65736963     sed by imprecise
3000e14c:	74616420 63612061 73736563 6f697620      data access vio
3000e15c:	6974616c 00006e6f 20737542 6c756166     lation..Bus faul
3000e16c:	73692074 75616320 20646573 75207962     t is caused by u
3000e17c:	6174736e 6e696b63 72652067 00726f72     nstacking error.
3000e18c:	20737542 6c756166 73692074 75616320     Bus fault is cau
3000e19c:	20646573 73207962 6b636174 20676e69     sed by stacking 
3000e1ac:	6f727265 00000072 20737542 6c756166     error...Bus faul
3000e1bc:	73692074 75616320 20646573 66207962     t is caused by f
3000e1cc:	74616f6c 2d676e69 6e696f70 616c2074     loating-point la
3000e1dc:	7320797a 65746174 65727020 76726573     zy state preserv
3000e1ec:	6f697461 0000006e 20656854 20737562     ation...The bus 
3000e1fc:	6c756166 636f2074 72727563 61206465     fault occurred a
3000e20c:	65726464 69207373 00000073 52414642     ddress is...BFAR
3000e21c:	305b203a 38302578 2d205d78 7325203e     : [0x%08x] -> %s
3000e22c:	0000000a 67617355 61662065 20746c75     ....Usage fault 
3000e23c:	63207369 65737561 79622064 74746120     is caused by att
3000e24c:	74706d65 6f742073 65786520 65747563     empts to execute
3000e25c:	206e6120 65646e75 656e6966 6e692064      an undefined in
3000e26c:	75727473 6f697463 0000006e 52534655     struction...UFSR
3000e27c:	305b203a 38302578 2d205d78 7325203e     : [0x%08x] -> %s
3000e28c:	0000000a 67617355 61662065 20746c75     ....Usage fault 
3000e29c:	63207369 65737561 79622064 74746120     is caused by att
3000e2ac:	74706d65 6f742073 69777320 20686374     empts to switch 
3000e2bc:	61206f74 6e69206e 696c6176 74732064     to an invalid st
3000e2cc:	20657461 672e6528 41202c2e 00294d52     ate (e.g., ARM).
3000e2dc:	67617355 61662065 20746c75 63207369     Usage fault is c
3000e2ec:	65737561 79622064 74746120 74706d65     aused by attempt
3000e2fc:	6f742073 206f6420 65206e61 70656378     s to do an excep
3000e30c:	6e6f6974 74697720 20612068 20646162     tion with a bad 
3000e31c:	756c6176 6e692065 65687420 43584520     value in the EXC
3000e32c:	5445525f 204e5255 626d756e 00007265     _RETURN number..
3000e33c:	67617355 61662065 20746c75 63207369     Usage fault is c
3000e34c:	65737561 79622064 74746120 74706d65     aused by attempt
3000e35c:	6f742073 65786520 65747563 63206120     s to execute a c
3000e36c:	6f72706f 73736563 6920726f 7274736e     oprocessor instr
3000e37c:	69746375 00006e6f 67617355 61662065     uction..Usage fa
3000e38c:	20746c75 63207369 65737561 79622064     ult is caused by
3000e39c:	646e6920 74616369 74207365 20746168      indicates that 
3000e3ac:	74732061 206b6361 7265766f 776f6c66     a stack overflow
3000e3bc:	61682820 61776472 63206572 6b636568      (hardware check
3000e3cc:	61682029 61742073 206e656b 63616c70     ) has taken plac
3000e3dc:	00000065 67617355 61662065 20746c75     e...Usage fault 
3000e3ec:	63207369 65737561 79622064 646e6920     is caused by ind
3000e3fc:	74616369 74207365 20746168 75206e61     icates that an u
3000e40c:	696c616e 64656e67 63636120 20737365     naligned access 
3000e41c:	6c756166 61682074 61742073 206e656b     fault has taken 
3000e42c:	63616c70 00000065 67617355 61662065     place...Usage fa
3000e43c:	20746c75 63207369 65737561 79622064     ult is caused by
3000e44c:	646e4920 74616369 61207365 76696420      Indicates a div
3000e45c:	20656469 7a207962 206f7265 20736168     ide by zero has 
3000e46c:	656b6174 6c70206e 20656361 6e616328     taken place (can
3000e47c:	20656220 20746573 796c6e6f 20666920      be set only if 
3000e48c:	5f564944 52545f30 73692050 74657320     DIV_0_TRP is set
3000e49c:	00000029 75626544 61662067 20746c75     )...Debug fault 
3000e4ac:	63207369 65737561 79622064 6c616820     is caused by hal
3000e4bc:	65722074 73657571 20646574 4e206e69     t requested in N
3000e4cc:	00434956 52534644 305b203a 38302578     VIC.DFSR: [0x%08
3000e4dc:	2d205d78 7325203e 0000000a 75626544     x] -> %s....Debu
3000e4ec:	61662067 20746c75 63207369 65737561     g fault is cause
3000e4fc:	79622064 504b4220 6e692054 75727473     d by BKPT instru
3000e50c:	6f697463 7865206e 74756365 00006465     ction executed..
3000e51c:	75626544 61662067 20746c75 63207369     Debug fault is c
3000e52c:	65737561 79622064 54574420 74616d20     aused by DWT mat
3000e53c:	6f206863 72756363 00646572 75626544     ch occurred.Debu
3000e54c:	61662067 20746c75 63207369 65737561     g fault is cause
3000e55c:	79622064 63655620 20726f74 63746566     d by Vector fetc
3000e56c:	636f2068 72727563 00006465 75626544     h occurred..Debu
3000e57c:	61662067 20746c75 63207369 65737561     g fault is cause
3000e58c:	79622064 42444520 20515247 6e676973     d by EDBGRQ sign
3000e59c:	61206c61 72657373 00646574 61656c50     al asserted.Plea
3000e5ac:	70206573 72676f72 63206d61 69706968     se program chipi
3000e5bc:	206f666e 4f206e69 21205054 0000000a     nfo in OTP !....
3000e5cc:	50494843 4f464e49 00000000 2050544f     CHIPINFO....OTP 
3000e5dc:	70696863 6f666e69 20736920 61766e69     chipinfo is inva
3000e5ec:	2c64696c 656c7020 20657361 676f7270     lid, please prog
3000e5fc:	206d6172 00000a21 61766e49 2064696c     ram !...Invalid 
3000e60c:	6e204442 65626d75 000a2172 204d434d     BD number!..MCM 
3000e61c:	6f6d656d 69207972 206f666e 61766e69     memory info inva
3000e62c:	2164696c 42202121 6d754e64 7525203a     lid!!! BdNum: %u
3000e63c:	000a0d20 3a525245 69685420 68432073      ...ERR: This Ch
3000e64c:	4e207069 5320746f 6f707075 0a0d7472     ip Not Support..
3000e65c:	00000a0d 41525350 726f204d 41524420     ....PSRAM or DRA
3000e66c:	6e45204d 6e692064 79616c20 2074756f     M End in layout 
3000e67c:	30207369 2c782578 74756220 74636120     is 0x%x, but act
3000e68c:	6c6c6175 73692079 25783020 730a0d78     ually is 0x%x..s
3000e69c:	4d207465 75204350 676e6973 74636120     et MPC using act
3000e6ac:	6c6c6175 61762079 0d65756c 0000000a     ually value.....

3000e6bc <chipinfo>:
3000e6bc:	03fe0600 0129014c 03ff0700 014101ca     ....L.).......A.
3000e6cc:	04000800 0129014c 04010801 0129014c     ....L.).....L.).
3000e6dc:	04020900 01530292 04030901 01530292     ......S.......S.
3000e6ec:	04040a00 01490252 04050a01 01490252     ....R.I.....R.I.
3000e6fc:	040e0b00 0129018c 040d0c00 0129014c     ......).....L.).
3000e70c:	040f0d00 01530292 04100e00 01490252     ......S.....R.I.
3000e71c:	04110f00 01530002 04121000 01490002     ......S.......I.
3000e72c:	04131100 01410002 ffffffff 00000000     ......A.........
3000e73c:	00000042 00000041 44415041 73255020     B...A...APAD P%s
3000e74c:	2064255f 63207369 69666e6f 65727567     _%d is configure
3000e75c:	6f742064 6e756620 20444963 20756c25     d to funcID %lu 
3000e76c:	0000000a 504d4f52 48435441 00000000     ....ROMPATCH....
3000e77c:	20445753 20444150 5f732550 69206425     SWD PAD P%s_%d i
3000e78c:	6f632073 6769666e 64657275 206f7420     s configured to 
3000e79c:	636e7566 25204449 0a20756c 00000000     funcID %lu .....
3000e7ac:	64206f69 65766972 61702072 656d6172     io driver parame
3000e7bc:	73726574 72726520 2021726f 656c6966     ters error! file
3000e7cc:	6d616e5f 25203a65 6c202c73 3a656e69     _name: %s, line:
3000e7dc:	0a642520 00000000 6d6f682f 75612f65      %d...../home/au
3000e7ec:	6a5f6f74 696b6e65 775f736e 356e616c     to_jenkins_wlan5
3000e7fc:	726f772f 6170736b 732f6563 612f6b64     /workspace/sdk/a
3000e80c:	2d6f7475 656c6572 2d657361 7473616d     uto-release-mast
3000e81c:	632f7265 6f706d6f 746e656e 636f732f     er/component/soc
3000e82c:	656d612f 6d736162 2f747261 2f62696c     /amebasmart/lib/
3000e83c:	70696863 6f666e69 656d612f 725f6162     chipinfo/ameba_r
3000e84c:	705f6d6f 68637461 0000632e 74206f4e     om_patch.c..No t
3000e85c:	20736968 68747561 7079745f 6c252865     his auth_type(%l
3000e86c:	66202975 5320726f 0a324148 00000000     u) for SHA2.....
3000e87c:	2a2a2a2a 2a2a2a2a 2a2a2a2a 2a2a2a2a     ****************
3000e88c:	2a2a2a2a 2a2a2a2a 57202a2a 494e5241     ********** WARNI
3000e89c:	2a20474e 2a2a2a2a 2a2a2a2a 2a2a2a2a     NG *************
3000e8ac:	2a2a2a2a 2a2a2a2a 000a2a2a 2020202a     **********..*   
3000e8bc:	20202020 20202020 20202020 20202020                     
3000e8cc:	20202020 20202020 20202020 20202020                     
3000e8dc:	20202020 20202020 20202020 20202020                     
3000e8ec:	20202020 000a2a20 2020202a 20202020          *..*       
3000e8fc:	20202020 75432020 6e657272 68632074           Current ch
3000e90c:	76207069 69737265 69206e6f 43412073     ip version is AC
3000e91c:	21217475 20202021 20202020 20202020     ut!!!           
3000e92c:	000a2a20 2020202a 204b5452 204b4453      *..*   RTK SDK 
3000e93c:	73656f64 746f6e20 70757320 74726f70     does not support
3000e94c:	72756320 746e6572 72657620 6e6f6973      current version
3000e95c:	796e6120 726f6d20 20202e65 000a2a20      any more.   *..
3000e96c:	2020202a 50202020 7361656c 6f632065     *      Please co
3000e97c:	6361746e 54522074 4d50204b 726f6620     ntact RTK PM for
3000e98c:	726f6d20 72702065 6375646f 6e692074      more product in
3000e99c:	20216f66 20202020 000a2a20 2a2a2a2a     fo!      *..****
3000e9ac:	2a2a2a2a 2a2a2a2a 2a2a2a2a 2a2a2a2a     ****************
3000e9bc:	2a2a2a2a 2a2a2a2a 2a2a2a2a 2a2a2a2a     ****************
3000e9cc:	2a2a2a2a 2a2a2a2a 2a2a2a2a 2a2a2a2a     ****************
3000e9dc:	2a2a2a2a 000a2a2a 6d6f682f 75612f65     ******../home/au
3000e9ec:	6a5f6f74 696b6e65 775f736e 356e616c     to_jenkins_wlan5
3000e9fc:	726f772f 6170736b 732f6563 612f6b64     /workspace/sdk/a
3000ea0c:	2d6f7475 656c6572 2d657361 7473616d     uto-release-mast
3000ea1c:	632f7265 6f706d6f 746e656e 636f732f     er/component/soc
3000ea2c:	656d612f 6d736162 2f747261 2f62696c     /amebasmart/lib/
3000ea3c:	746f6f62 64616f6c 622f7265 6c746f6f     bootloader/bootl
3000ea4c:	6564616f 70685f72 62696c5f 0000632e     oader_hp_lib.c..
